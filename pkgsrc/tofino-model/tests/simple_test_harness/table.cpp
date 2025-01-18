/*******************************************************************************
 *  Copyright (C) 2024 Intel Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing,
 *  software distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions
 *  and limitations under the License.
 *
 *
 *  SPDX-License-Identifier: Apache-2.0
 ******************************************************************************/

#include <mau.h>
#include <rmt-object-manager.h>
#include <register_utils.h>
#if !MCN_TEST(MODEL_CHIP_NAMESPACE,rsvd1)
#include <mau-tcam.h>
#endif // !WIP
#include <sstream>
#include "hex.h"
#include "json-ref.h"
#include "log.h"
#include "map.h"
#include "p4name.h"
#include "packet.h"
#include "script.h"
#include "table_config.h"
#include "util.h"
#include "write.h"


using namespace MODEL_CHIP_NAMESPACE;

struct word_t {
    uint64_t    data0 = 0, data1 = 0;
    // used for both tcam and sram
    // tcam -- data0/1 are the match0/1 of the ternary memory
    // sram -- data0 is lower 64 bits, data1 is upper.
};
inline word_t operator&(word_t a, word_t b) { a.data0 &= b.data0; a.data1 &= b.data1; return a; }
inline word_t operator|(word_t a, word_t b) { a.data0 |= b.data0; a.data1 |= b.data1; return a; }
inline word_t operator^(word_t a, word_t b) { a.data0 ^= b.data0; a.data1 ^= b.data1; return a; }
inline word_t operator~(word_t a) { a.data0 = ~a.data0; a.data1 = ~a.data1; return a; }

static unsigned build_counter_full_address(int addr, const table_config::table_t &counter) {
    unsigned full_addr = 0;
    switch (counter.entries_per_word()) {
    case 1:
        full_addr = addr << 3;
        break;
    case 2:
        full_addr = addr << 2;
        break;
    case 3:
        full_addr = ((addr >> 13) & 3) | ((addr & ~(3 << 13)) << 3);
        break;
    case 4:
        full_addr = addr << 1;
        break;
    case 6:
        full_addr = ((addr >> 13) & 3) | ((addr & ~(3 << 13)) << 3);
        break;
    default:
        error("bad table format for counter");
    }
    return full_addr | (1 << 19);
}

static unsigned build_stateful_full_address(int addr, const table_config::table_t &stateful,
    uint action_handle) {
    unsigned full_addr = 0;
    switch(stateful.entries_per_word()) {
    case 1:
        full_addr = addr << 7;  break;
    case 2:
        full_addr = addr << 6;  break;
    case 4:
        full_addr = addr << 5;  break;
    case 8:
        full_addr = addr << 4;  break;
    case 16:
        full_addr = addr << 3;  break;
    case 128:
        full_addr = addr;  break;
    default:
        error("bad table format for stateful alu");
    }
    // FIXME: Not yet able to pull the meter type into the full word address of the table,
    // because no access to the stateful alu instruction map as of this point
    full_addr |= (1 << 23);
    int type = stateful.find_meter_type(action_handle);
    switch (type) {
        case 0:
            full_addr |= (1 << 24);
            break;
        case 1:
            full_addr |= (3 << 24);
            break;
        case 2:
            full_addr |= (5 << 24);
            break;
        case 3:
            full_addr |= (7 << 24);
            break;
        default:
            break;
    }
    return full_addr;
}

static unsigned build_meter_full_address(int addr, const table_config::table_t &meter) {
    unsigned full_addr = 0;
    full_addr = addr << 7;
    full_addr |= (1 << 23);  // pfe bit
    if (meter.meter_color_aware)
        full_addr |= (6 << 24);  // meter type
    else
        full_addr |= (2 << 24);  // meter type
    return full_addr;
}

static unsigned build_adt_full_addr(int addr, const table_config::table_t &adt) {
    if (adt.width == 1) {
        unsigned bytes = 16/adt.fields.size();
        return addr * bytes * 2 + (bytes-1);  // huffman encoding for small adt
    } else {
        // large table huffman encoding -- see uArch fig 6-34/6-35
        unsigned upper = addr >> 10;
        addr &= 0x3ff;
        upper = upper * adt.width | (adt.width/2 - 1);
        return (upper << 15) | (addr << 5) | 0x1f;
    }
}

/** put the data 'value' into the bits corresponding to 'field' int 'data'
 *  we ignore field->start_bit here, assuming 'value' has already been shifted */
static void fill_sram_field(std::vector<word_t> &data, const table_config::field &field,
                            uint64_t value) {
    int bit = 128 * data.size() - field.start_offset - field.bit_width;
    bool ismask = value == ~0UL;
    if (field.bit_width < 64)
        value &= (1UL << field.bit_width) - 1;
    if (field.enable_pfe)
        value |= 1UL << (field.bit_width - 1);
    LOG3("sram_field " << field.name << "(" << field.start_bit << ".." <<
         (field.start_bit + field.bit_width - 1) << ") " << (bit + field.bit_width - 1) <<
         ':' << bit << (ismask ? " mask" : "") << " = 0x" << hex(value));
    assert((bit + field.bit_width - 1)/128U < data.size());
    int word = bit/128;
    bit %= 128;
    if (bit < 64) {
        data[word].data0 |= value << bit;
        if ((bit > 0) && (bit + field.bit_width > 64))
            data[word].data1 |= value >> (64 - bit);
    } else
        data[word].data1 |= value << (bit - 64);
    if (bit + field.bit_width > 128)
        data[word+1].data0 |= value >> (128 - bit);
}

static uint64_t get_sram_field_slice(std::vector<word_t> &data, const table_config::field &field,
        int lo, int hi) {
    int bit = 128 * data.size() - field.start_offset - field.bit_width + lo;
    if (hi < 0) hi = field.bit_width - 1;
    int width = hi - lo + 1;
    if (width <= 0 || hi >= field.bit_width)
        error("invalid field slice %s[%d:%d]", field.name.c_str(), hi, lo);
    assert((bit + width - 1)/128U < data.size());
    unsigned word = bit/128U;
    bit %= 128;
    uint64_t rv = 0;
    if (bit < 64) {
        rv = data[word].data0 >> bit;
        if (bit > 0)
            rv |= data[word].data1 << (64 - bit);
    } else {
        rv = data[word].data1 >> (bit - 64);
        if (bit > 64 && word + 1 < data.size())
            rv |= data[word+1].data1 << (128 - bit); }
    if (width < 64)
        rv &= (1UL << width) - 1;
    return rv;
}

static uint64_t get_sram_field(std::vector<word_t> &data, const table_config::field &field) {
    return get_sram_field_slice(data, field, 0, -1);
}

static void fill_tcam_field(std::vector<word_t> &data, const table_config::table_t &table,
                            const table_config::field &field, uint64_t data0, uint64_t data1) {
    uint64_t mask = ~uint64_t(0);
    int bit = table.mem_width * data.size() - field.start_offset - field.bit_width;
    LOG3("tcam_field " << field.name << " " << (bit + field.bit_width - 1) << ':' << bit <<
         " = data0:" << hex(data0) << " data1:" << hex(data1));
    assert(bit >= 0 && (bit + field.bit_width - 1)/table.mem_width < data.size());
    int word = bit/table.mem_width;
    bit %= table.mem_width;
    if (field.bit_width < 64) {
        data0 &= (1UL << field.bit_width) - 1;
        data1 &= (1UL << field.bit_width) - 1;
        mask &= (1UL << field.bit_width) - 1; }
    data[word].data0 = (data[word].data0 & ~(mask << bit)) | (data0 << bit);
    data[word].data1 = (data[word].data1 & ~(mask << bit)) | (data1 << bit);
    if (bit + field.bit_width > table.mem_width) {
        int shift = bit + field.bit_width - table.mem_width;
        data[word+1].data0 = (data[word].data0 & ~(mask >> shift)) | (data0 >> shift);
        data[word+1].data1 = (data[word].data1 & ~(mask >> shift)) | (data1 >> shift); }
}

static bool parse_value(std::vector<word_t> &value, bool ternary, char *&args)
{
    for (auto &v : value)
        v.data0 = v.data1 = UINT64_C(0);
    int len = -1, off = -1, l2base = -1, n, q[4] = {0}, l[5] = {0};
    if ((n = sscanf(args, "%u.%u%n.%u%n.%u%n",
                    &q[0], &q[1], &l[2], &q[2], &l[3], &q[3], &l[4])) > 1) {
        /* IPv4 address */
        len = l[n];
        for (int i = 3; i >= n; --i) {
            q[i] = q[n-1] % 256U;
            q[n-1] /= 256U; }
        if ((q[0] | q[1] | q[2] | q[3]) >= 256)
            return false;
        value[0].data1 = (q[0] << 24) | (q[1] << 16) | (q[2] << 8) | q[3];
        value[0].data0 = value[0].data1 ^ 0xffffffffU;
        args += len;
        if (ternary && sscanf(args, "/%u%n", &n, &len) >= 1 && n <= 32) {
            if (n < 32) {
                uint32_t mask = ~0U;
                mask <<= 32-n;
                value[0].data0 |= mask;
                value[0].data1 |= mask; }
            args += len; }
        return true;
    } else if (sscanf(args, "%" SCNu64 "..%" SCNu64 "%n", &value[0].data0,
                      &value[0].data1, &len) >= 2 ||
               sscanf(args, "0o%" SCNo64 "..0o%" SCNo64 "%n", &value[0].data0,
                      &value[0].data1, &len) >= 2 ||
               sscanf(args, "0x%" SCNx64 "..0x%" SCNx64 "%n", &value[0].data0,
                      &value[0].data1, &len) >= 2) {
        if (!ternary) return false;
        auto mask = value[0].data0 ^ value[0].data1;
        if (mask & (mask + 1)) {
            int shift = 1;
            while (mask & (mask + 1)) {
                mask |= mask >> shift;
                shift += shift; }
            error("range %" PRIu64 "..%" PRIu64 " does not fit in one tcam row, using %" PRIu64
                  "..%" PRIu64 " instead", value[0].data0, value[0].data1,
                  value[0].data0 & ~mask, value[0].data1 | mask); }
        value[0].data0 = ~value[0].data1 | mask;
        value[0].data1 |= mask;
        args += len;
        return true;
    } else if (sscanf(args, " 0x%n%*[0-9a-fA-F*]%n", &off, &len), len > 0) {
        l2base = 4;
    } else if (sscanf(args, " 0o%n%*[0-7*]%n", &off, &len), len > 0) {
        l2base = 3;
    } else if (sscanf(args, " 0b%n%*[01*]%n", &off, &len), len > 0) {
        l2base = 1;
    } else if (sscanf(args, "%" SCNu64 "%n", &value[0].data1, &len) >= 1) {
        // FIXME -- can't handle decimal constants >64 bits
        value[0].data0 = ~value[0].data1;
        args += len;
        return true;
    } else if (ternary && sscanf(args, " %*[*]%n", &len), len > 0) {
        for (auto &v : value)
            v.data0 = v.data1 = ~UINT64_C(0);
        args += len;
        return true;
    } else {
        return false; }
    unsigned bits = (len - off)*l2base;
    /* if the value doesn't cover the size of the operand, need to extend properly */
    for (unsigned i = bits/64; i < value.size(); ++i) {
        value[i].data0 = ~UINT64_C(0) << (bits%64);
        /* if the leading digit is *, extend that to all higher digits, otherwise 0 extend */
        if (ternary && args[off] == '*')
            value[i].data1 = ~UINT64_C(0) << (bits%64);
        bits = 0; }
    uint64_t mask = (1U << l2base) - 1;
    int bit = 0;
    for (int i = len-1; i >= off; --i, bit += l2base) {
        if ((bit+l2base-1)/64U >= value.size())
            value.resize(value.size()+1);
        unsigned j = bit/64U;
        unsigned b = bit%64;
        if (args[i] == '*') {
            if (!ternary) return false;
            value[j].data0 |= mask << b;
            value[j].data1 |= mask << b;
            if (b+l2base > 64) {
                value[j+1].data0 |= mask >> (64-b);
                value[j+1].data1 |= mask >> (64-b); }
        } else {
            uint64_t digit = args[i] - '0';
            if (args[i] >= 'a' && args[i] <= 'f')
                digit = args[i] - 'a' + 10;
            else if (args[i] >= 'A' && args[i] <= 'F')
                digit = args[i] - 'A' + 10;
            value[j].data0 |= (digit ^ mask) << b;
            value[j].data1 |= digit << b;
            if (b+l2base > 64) {
                value[j+1].data0 |= (digit ^ mask) >> (64-b);
                value[j+1].data1 |= digit >> (64-b); } } }
    args += len;
    return true;
}

static bool fill_ternary_field(const char *name, std::vector<word_t> &data,
                       const table_config::table_t &table, char *args) {
    std::vector<word_t>      value;
    assert(table.entries_per_word() == 1);
    for (auto &f : table.fields.at(0).iter(name))
        if ((size_t)f.start_bit + f.bit_width > value.size() * 64)
            value.resize((f.start_bit + f.bit_width + 63)/64U);
    if (value.empty()) return false;
    if (!parse_value(value, true, args)) {
        error("bad value %s for %s", Script::token(&args), name);
        return false; }
    for (auto &f : table.fields.at(0).iter(name)) {
        unsigned j = f.start_bit/64U;
        unsigned b = f.start_bit%64U;
        if (b && j+1 < value.size())
            fill_tcam_field(data, table, f, (value[j+1].data0 << (64-b)) | (value[j].data0 >> b),
                            (value[j+1].data1 << (64-b)) | (value[j].data1 >> b));
        else
            fill_tcam_field(data, table, f, value[j].data0 >> b, value[j].data1 >> b); }
    return true;
}

static uint64_t getu64(const char *p) {
    char buf[17];
    memcpy(buf, p, 16);
    buf[16] = 0;
    return strtoull(buf, 0, 16);
}

static bool fill_field(const char *name, unsigned pipes, std::vector<word_t> &data,
                       const table_config::field_map_t &fields, const table_config::remap_t *remap,
                       char *args, uint action_handle) {
    bool rv = false;
    char *end_args = args;
    errno = 0;
    uint64_t value = strtoull(args, &end_args, 0);
    uint64_t param_value = value;
    if (errno == ERANGE) {
        if (!remap && (args[1] == 'x' || args[1] == 'X')) {
            /* Hack to deal with arbitrarily large hex values */
            const char *p = args+2;
            int bits = (end_args - p) * 4;
            for (auto &f : ValuesForKey(fields, name)) {
                if (f.start_bit >= bits - 64)
                    fill_sram_field(data, f, getu64(p) >> (64 - (bits - f.start_bit)));
                else
                    fill_sram_field(data, f,
                        getu64(p + (bits-64-f.start_bit+3)/4) >> (f.start_bit%4));
                rv = true; }
        } else {
            error("value %.*s out of range for 64 bits", int(end_args-args), args);
            return true; }
    } else if (errno || args == end_args || isalnum(*end_args)) {
        error("expected a constant value: %s", args);
        return true;
    } else {
        if (remap) {
            for (auto &r : ValuesForKey(*remap, name)) {
                if (!r.res_name.empty()) {
                    auto *table_conf = table_config::get(r.res_name.c_str(), pipes);
                    auto table = table_conf->stage_tables.data()->table;
                    if (r.type == table_config::STATS_PTR) {
                        value = build_counter_full_address(param_value, table);
                    } else if (r.type == table_config::STFUL_PTR) {
                        value = build_stateful_full_address(param_value, table, action_handle);
                    } else if (r.type == table_config::METER_PTR) {
                        value = build_meter_full_address(param_value, table);
                    }
                }
                uint64_t mask = ~uint64_t(0);
                if (r.bit_width < 64) {
                    mask = ~(mask << r.bit_width);
                } else if (r.bit_width > 64)
                    error("immediate fields >64 bits not supported");
                uint64_t val = ((value >> r.start_bit) & mask) << r.immed_bit;
                for (auto &f : ValuesForKey(fields, r.immed_name)) {
                    fill_sram_field(data, f, val >> f.start_bit);
                    rv = true; } } }
        for (auto &f : ValuesForKey(fields, name)) {
            LOG1("value " << f.name << " data size=" << data.size() << " start_offset=" << f.start_offset);
            fill_sram_field(data, f, value >> f.start_bit);
            rv = true; } }
    return rv;
}

static bool fill_field(const char *name, unsigned pipes, std::vector<word_t> &data,
                       const table_config::table_t &table, int group,
                       const table_config::remap_t *remap, char *args,
                       uint action_handle) {
    if (group < 0) return false;
    group = group % table.entries_per_word();
    LOG1("group " << group);
    return fill_field(name, pipes, data, table.fields.at(group).by_name, remap,
                      args, action_handle);
}
static void fill_const_fields(std::vector<word_t> &data, const table_config::field_map_t &fields,
                              const table_config::remap_t *remap) {
    for (auto &r : Values(*remap)) {
        if (!r.constant) continue;
        uint64_t mask = ~uint64_t(0);
        if (r.bit_width < 64) {
            mask = ~(mask << r.bit_width);
        } else if (r.bit_width > 64)
            error("immediate fields >64 bits not supported");
        uint64_t val = ((r.constant_value >> r.start_bit) & mask) << r.immed_bit;
        for (auto &f : ValuesForKey(fields, r.immed_name))
            fill_sram_field(data, f, val >> f.start_bit); }
}

static bool fill_immed(const char *name, uint64_t &data, const table_config::remap_t *immed,
                       char *args) {
    bool rv = false;
    if (!immed) return rv;
    char *end_args = args;
    errno = 0;
    uint64_t value = strtoull(args, &end_args, 0);
    if (errno) {
        error("expected a constant value: %s", args);
        return true; }
    for (auto &r : ValuesForKey(*immed, name)) {
        uint64_t mask = ~uint64_t(0);
        if (r.bit_width < 64) {
            mask = ~(mask << r.bit_width);
        } else if (r.bit_width > 64)
            error("immediate fields >64 bits not supported");
        data |= ((value >> r.start_bit) & mask) << r.immed_bit;
        rv = true; }
    return rv;
}

static bool fill_overhead_field(const table_config::stage *st, table_config::field_source_t type,
                                uint64_t value, unsigned addr, std::vector<word_t> &match,
                                unsigned match_addr, std::vector<word_t> &indirect) {
    bool ok = false;
    int entry = addr % st->table.entries_per_word();
    for (auto field : st->table.fields.at(entry).iter(type)) {
        fill_sram_field(match, *field, value >> field->start_bit);
        ok = true; }
    if (st->indirect) {
        entry = match_addr % st->indirect.entries_per_word();
        for (auto field : st->indirect.fields.at(entry).iter(type)) {
            fill_sram_field(indirect, *field, value >> field->start_bit); }
            ok = true; }
    return ok;
}

static void write_tcam(unsigned pipes, const table_config *tc, const table_config::stage &cfg,
                       const table_config::table_t &table,
                       std::vector<word_t> data, int address) {
    if (table.width == 0) return;
    int ram = address / 512U;
    if (size_t(ram) > table.mem.size()) {
        error("tcam address 0x%x out of range", address);
        return; }
    address %= 512U;
    bool need_2bit_cvt = tcam_2bit_mode;
    for (int pipe = 0; pipes; pipe++, pipes >>= 1) {
        if (!(pipes & 1)) continue;
        int die = PIPE2DIE(pipe);
        int phy_pipe = PIPE_WITHIN_DIE(pipe);
        StageRef stage(die, phy_pipe, cfg.stage, tc->direction);
        for (unsigned word = 0; word < data.size(); word++) {
            int unit = table.mem.at(ram).units.at(word);
            auto tcam = stage.tcam_lookup(unit);
            if (need_2bit_cvt) {
#if !MCN_TEST(MODEL_CHIP_NAMESPACE,rsvd1)
                unsigned data0bit0 = data[word].data0 & 1, data1bit0 = data[word].data1 & 1;
                data[word].data0 >>= 1; data[word].data1 >>= 1;
#endif /* !rsvd1 */
                convert_to_2bit(data[word].data0, data[word].data1);
#if !MCN_TEST(MODEL_CHIP_NAMESPACE,rsvd1)
                data[word].data0 <<= 1; data[word].data0 |= data0bit0;
                data[word].data1 <<= 1; data[word].data1 |= data1bit0;
#endif /* !rsvd1 */
            }
            LOG1("write_tcam(" << phy_pipe << ", " << cfg.stage << ":" << unit << ":" << address <<
                 ", " << ram << ":" << word << ", " << hex(data[word].data0) << ", " <<
                 hex(data[word].data1) << "), die: " << die);
            tcam.write(address, data[word].data0, data[word].data1, UINT64_C(0)); }
        need_2bit_cvt = false;
    }
}

#if !MCN_TEST(MODEL_CHIP_NAMESPACE,rsvd1)
static void fill_hash_input(MauHashGenerator::InputT &hash_input,
                            MauHashGenerator::ValidT &hash_valid,
                            int bit, uint64_t word) {
    hash_input.set_word(word, bit);
    uint64_t mask = 0xff;
    while (mask) {
        if (word & mask)
            hash_valid.set_bit(bit/8);
        mask <<= 8;
        bit += 8; }
}
#endif // !WIP

static int parity(uint64_t val) {
    val ^= val >> 32;
    val ^= val >> 16;
    val ^= val >> 8;
    val ^= val >> 4;
    val ^= val >> 2;
    val ^= val >> 1;
    return val & 1;
}

static int calculate_hash_bit(const std::vector<uint64_t> &in,
                              const table_config::hash_column_t &col) {
    int rv = col.seed ? 1 : 0;
    for (unsigned i = 0; i < col.col.size() && i < in.size(); ++i)
        rv ^= parity(col.col[i] & in[i]);
    return rv;
}

/* we encode exact match addresses weirdly, as
 *    <actual match address> * <match entries per word> + <entry within word>
 * so the entry within the word is redundantly encoded both directly and in the vpn.
 * This means that we can use the same `write_sram` routine to write to any kind of
 * sram table, using addr % entries_per_word to select the entry within the word
 */
static int find_exact_addr(unsigned pipes, const table_config::stage &cfg,
                           const std::vector<word_t> &hash_input_, int address) {
    int way_no = address / cfg.table.entries_per_word();
    auto &way = cfg.ways.at(way_no);
    int rv = 0, ram = 0;
    if (cfg.hash_functions.size() > size_t(way.group)) {
        auto &fn = cfg.hash_functions.at(way.group);
        std::vector<uint64_t>   hash_input(16);
        for (int i = 0; i < 8; ++i) {
            hash_input[i*2] = hash_input_[i].data0;
            hash_input[i*2 + 1] = hash_input_[i].data1; }
        int bit = 1;
        const int index_bits = way.entry_bits + way.subword_bits;
        for (int i = way.entry_lo; i < way.entry_lo + index_bits; i++, bit <<= 1)
            if (fn.count(i) && calculate_hash_bit(hash_input, fn.at(i)))
                rv |= bit;
        bit = 1;
        int mask = 1;
        for (int i = way.select_lo; i < way.select_lo + way.select_bits; i++, bit <<= 1) {
            if (bit & way.select_mask) {
                if (fn.count(i) && calculate_hash_bit(hash_input, fn.at(i)))
                    ram |= mask;
                mask <<= 1; } }
    } else {
#if MCN_TEST(MODEL_CHIP_NAMESPACE,rsvd1)
        error("Can't extract hash from config on rsvd1 -- must have function in context.json");
#else
        int pipe = 0;
        while (pipes && !(pipes & (1 << pipe))) pipe++;
        int die = PIPE2DIE(pipe);
        int phy_pipe = PIPE_WITHIN_DIE(pipe);
        RmtObjectManager *om = NULL;
        GLOBAL_MODEL->GetObjectManager(die, &om);
        auto *mau = om->mau_lookup(phy_pipe, cfg.stage);
        auto *hash = mau->mau_input()->get_hash_generator();
        MauHashGenerator::InputT hash_input;
        MauHashGenerator::ValidT hash_valid;
        int bit = 0;
        for (auto &w : hash_input_) {
            fill_hash_input(hash_input, hash_valid, bit, w.data0);
            fill_hash_input(hash_input, hash_valid, bit+64, w.data1);
            bit += 128; }
        bit = 1;
        const int index_bits = way.entry_bits + way.subword_bits;
        for (int i = way.entry_lo; i < way.entry_lo + index_bits; i++, bit <<= 1)
            if (hash->CalculateOutputBit(hash_input, hash_valid, way.group, i))
                rv |= bit;
        bit = 1;
        int mask = 1;
        for (int i = way.select_lo; i < way.select_lo + way.select_bits; i++, bit <<= 1) {
            if (bit & way.select_mask) {
                if (hash->CalculateOutputBit(hash_input, hash_valid, way.group, i))
                    ram |= mask;
                mask <<= 1; } }
#endif // !WIP
    }
    int entries_per_set = std::max(1, cfg.table.entries_per_word() >> way.subword_bits);
    rv += way.mem.at(ram).vpn.at(address % entries_per_set) << way.entry_bits;
    rv = (rv * cfg.table.entries_per_word()) + address % cfg.table.entries_per_word();
    LOG3("find_exact_addr = " << rv << " (0x" << hex(rv) << ")");
    return rv;
}

static int find_hash_action_addr(unsigned pipes, const table_config::stage &cfg,
                                 const std::vector<word_t> &hash_input_) {
    auto &hash_function = cfg.hash_functions.at(0);
    std::vector<uint64_t>   hash_input(16);
    for (int i = 0; i < 8; ++i) {
        hash_input[i*2] = hash_input_[i].data0;
        hash_input[i*2 + 1] = hash_input_[i].data1; }
    int rv = 0;
    for (auto col : hash_function)
        if (calculate_hash_bit(hash_input, col.second))
            rv |= (1 << col.first);
    return rv;
}

static int find_unused_way(unsigned pipes, const table_config *tc, const table_config::stage &cfg,
                           const std::vector<word_t> &hash_input) {
    int pipe = 0;
    while (pipes && !(pipes & (1 << pipe))) pipe++;
    int die = PIPE2DIE(pipe);
    int phy_pipe = PIPE_WITHIN_DIE(pipe);
    StageRef stage(die, phy_pipe, cfg.stage, tc->direction);
    std::vector<word_t> current;
    current.resize(cfg.table.width);
    int addr = 0;
    bool warned = false;
    for (unsigned way = 0; way < cfg.ways.size(); ++way, addr += cfg.table.entries_per_word()) {
        unsigned bits = cfg.ways.at(way).entry_bits + cfg.ways.at(way).subword_bits;
        int address = find_exact_addr(pipes, cfg, hash_input, addr) / cfg.table.entries_per_word();
        auto *mem = cfg.table.vpn2mem.at(address >> bits);
        int ram = address >> bits;
        address = address & ((1 << bits) - 1);
        int subword_bits = cfg.ways.at(way).subword_bits;
        int entries_per_set = std::max(1, cfg.table.entries_per_word() >> subword_bits);
        int subword = address & ((1 << subword_bits) - 1);
        address >>= subword_bits;
        for (unsigned w = 0; w < current.size(); ++w) {
            int unit = mem->units.at(w);
            auto runit = cfg.ways.at(way).mtype == table_config::lamb ? stage.lamb_lookup(unit)
                                                                      : stage.sram_lookup(unit);
            runit.read(address, &current[w].data0, &current[w].data1, UINT64_C(0));
            LOG4("read_sram(" << cfg.stage << ":" << unit << ":" << address << ", " <<
                 way << ":" << ram << ":" << w << ") = " << hex(current[w].data1,16,'0') <<
                 hex(current[w].data0,16,'0')); }
        for (int i = 0; i < entries_per_set; ++i) {
            int entry = i + subword * entries_per_set;
            bool inuse = false;
            for (auto *field : cfg.table.fields.at(entry).iter(table_config::VERSION)) {
                if (get_sram_field(current, *field) == 0) {
                    LOG3("find_unused_way = " << (addr + entry) << " (0x" <<
                         hex(addr + entry) << ")");
                    return addr + entry;
                } else {
                    inuse = true; } }
            if (inuse) continue;
            for (auto *field : cfg.table.fields.at(entry).iter(table_config::VALID)) {
                if (get_sram_field(current, *field) == 0) {
                    LOG3("find_unused_way = " << (addr + entry) << " (0x" <<
                         hex(addr + entry) << ")");
                    return addr + entry;
                } else {
                    inuse = true; } }
            if (inuse) continue;
            if (!warned) {
                warning("no version/valid field in stage table %d:%d", cfg.stage, cfg.logical_id);
                warned = true; }
            for (auto &field : Values(cfg.table.fields.at(entry).by_name)) {
                if (get_sram_field(current, field) != 0) {
                    inuse = true;
                    break; } }
            if (!inuse) {
                LOG3("find_unused_way = " << (addr + entry) << " (0x" << hex(addr + entry) << ")");
                return addr + entry; } } }
    error("all ways in use");
    return 0;
}

/* we encode exact match addresses weirdly, as
 *    <actual match address> * <match entries per word> + <entry within word>
 * so the entry within the word is redundantly encoded both directly and in the vpn.
 * This means that we can use the same `write_sram` routine to write to any kind of
 * sram table, using addr % entries_per_word to select the entry within the word
 */
static void write_sram(unsigned pipes, const table_config *tc, const table_config::stage &cfg,
                       const table_config::table_t &table,
                       std::vector<word_t> data, int address) {
    if (table.width == 0) return;
    unsigned ram_depth = 1024U;
    unsigned subword_bits = 0;
    bool lamb = false;
    if (!cfg.ways.empty() && &cfg.table == &table) {
        lamb = cfg.ways.front().mtype == table_config::lamb;
        subword_bits = cfg.ways.front().subword_bits;
        ram_depth = 1U << (cfg.ways.front().entry_bits + subword_bits);
    } else if (table.mtype == table_config::scm_tind) {
        switch (table.mem_width) {
        case 64:  ram_depth = 64; break;
        case 128: ram_depth = 32; break;
        default:
            error("unexpected tind mem_width %d", table.mem_width);
            break; }
    } else {
        // ????
    }
    int vpn = address / (ram_depth * table.entries_per_word());
    if (vpn > 0 && !table.vpn2mem.count(1)) {
        /* wide action data tables come with the vpn pre-mulitplied by the vpn shift in the
         * context json.  Seems wrong, but we need to do that shift here if so */
        vpn *= table.width; }
    if (!table.vpn2mem.count(vpn)) {
        /* calculated vpn is not present in the table.  Probably a screwup with a wide ad
         * table and the vpn shift above, or some othe vpn problem.  But we can't really
         * do much about it, other than not crash */
        error("invalid vpn %d in write_sram for logical table 0x%x (%s)", vpn,
              cfg.stage*0x10 + cfg.logical_id, &table == &cfg.table ? "match" :
              &table == &cfg.indirect ? "indirect" : "attached");
        return; }
    auto *mem = table.vpn2mem.at(vpn);
    int entry = address % table.entries_per_word();
    int vpni = mem->vpn.size() > 1 ? entry >> subword_bits : 0;
    if (mem->vpn.at(vpni) != vpn) {
        error("vpn mismatch in table (%d != %d)", vpn, mem->vpn.at(vpni));
        return; }
    address = ((address / table.entries_per_word()) >> subword_bits) % ram_depth;
    std::vector<word_t> mask;
    mask.resize(data.size());
    for (auto &f : Values(table.fields.at(entry).by_name)) {
        if (f.source == table_config::CONSTANT)
            fill_sram_field(data, f, f.const_value);
        fill_sram_field(mask, f, ~0UL); }
    if (table.mtype == table_config::scm_tind) {
        assert(data.size() == 1);
        if (table.mem_width == 64) {
            // tinds are only 64 bits wide but packed with 2 words per 128 bit word.  We count
            // field bits from msb, so swap upper/lower halves to get the data to the right place
            // for even words and divide the address by 2;
            if ((address & 1) == 0) {
                std::swap(data[0].data0, data[0].data1);
                std::swap(mask[0].data0, mask[0].data1); }
            address >>= 1; } }
    for (int pipe = 0; pipes; pipe++, pipes >>= 1) {
        if (!(pipes & 1)) continue;
        int die = PIPE2DIE(pipe);
        int phy_pipe = PIPE_WITHIN_DIE(pipe);
        StageRef stage(die, phy_pipe, cfg.stage, tc->direction);
        for (unsigned word = 0; word < data.size(); word++) {
            int unit = mem->units.at(word);
            auto sram = lamb ? stage.lamb_lookup(unit)
                      : table.mtype == table_config::scm_tind ? stage.tind_lookup(unit)
                      : stage.sram_lookup(unit);
            word_t tmp;
            sram.read(address, &tmp.data0, &tmp.data1, UINT64_C(0));
            LOG4("read_sram(" << phy_pipe << ", " << cfg.stage << ":" << unit << ":" << address <<
                 ", " << vpn << ":" << word << ") = " << hex(tmp.data1,16,'0') <<
                 hex(tmp.data0,16,'0') << ", die: " << die);
            tmp = (tmp & ~mask[word]) | (data[word] & mask[word]);
            LOG5("  data = " << hex(data[word].data1,16,'0') << hex(data[word].data0,16,'0') <<
                 "  mask = " << hex(mask[word].data1,16,'0') << hex(mask[word].data0,16,'0'));
            LOG1("write_sram(" << phy_pipe << ", " << cfg.stage << ":" << unit << ":" << address <<
                 ", " << vpn << ":" << word << ", " << hex(tmp.data1,16,'0') <<
                 hex(tmp.data0,16,'0') << "), die: " << die);
            sram.write(address, tmp.data0, tmp.data1, UINT64_C(0)); }
    }
}

namespace {

// returns true iff 'str' ends with 'suffix'
bool endsWith(const std::string &str, const std::string &suffix) {
    return (str.size() >= suffix.size()) &&
        !str.compare(str.size() - suffix.size(), suffix.size(), suffix);
}

const table_config::action *find_action(const table_config::stage *st,
                                        const std::string &unique_suffix) {
    const table_config::action *candidate = nullptr;
    for (auto &act : st->actions) {
        if (act.name == unique_suffix) return &act;
        if (!endsWith(act.name, unique_suffix)) continue;
        if (candidate != nullptr) {
            error("Action suffix '%s' is not unique", unique_suffix.c_str());
            return nullptr; }
        candidate = &act; }
    return candidate;
}

}  // namespace

ScriptCommand(add, "<table> <addr> {<key>:<value>} <action>({<name>:<value>}) [= <handle>]") {
    char *name = token(&args);
    unsigned pipes = ALL_PIPES;
    bool add_all_stages = false;  // table that needs entries added to all stage tables
    auto table = table_config::get(name, pipes);
    if (!table) return;  // error output by table_config::get
    if (table->stage_tables.empty()) {
        error("Table %s has no stage tables?!", name);
        return; }
    const table_config::stage *st = &table->stage_tables.back();
    std::vector<word_t> hash_input, match, indirect, actiondata;
    const table_config::table_t *adtable = nullptr;
    uint action_handle = 0;
    const table_config::remap_t *immed_remap = nullptr;
    const std::set<std::string> *p4_params = nullptr;
    while (st->type == table_config::MATCH_NO_KEY && st > &table->stage_tables.front())
        --st;
    match.resize(st->table.width);
    if (table->stage_tables.size() > 1 && table->entries > 0 && st->entries >= table->entries) {
        // whether we need to add an entry to all stages (eg, for a dleft table) should be
        // specified in the context.json, but its not.  So we infer it if the stage tables
        // all have as many entries as the table as a whole
        add_all_stages = true;
        for (auto &s : table->stage_tables) {
            if (s.entries < table->entries) {
                add_all_stages = false;
                break; } } }

    // debug info
    LOG1("table config: type=" << st->type << " stage=" << st->stage << " logical_id=" << st->logical_id);
    LOG1("\ttable config: entries=" << st->entries);
    for (auto h : st->hash_input) {
        LOG1("\thash_input=" << h.first);
        LOG1("\t\tvalue=" << h.second.const_value);
    }
    for (auto f : st->hash_functions) {
        for (auto kv : f) {
            LOG1("\tidx=" << kv.first << " seed=" << kv.second.seed << " col=" <<
                 std::hex << kv.second.col << std::dec);
        }
    }
    LOG1("\ttbl_width=" << st->table.width << " bit_width=" << st->table.bit_width);
    for (auto a : st->actions) {
        LOG1("\taction=" << a.name);
    }

    if (st->type == table_config::HASH_MATCH || st->type == table_config::ATCAM_MATCH ||
        st->type == table_config::HASH_ACTION) {
        hash_input.resize(8);
    } else if (st->type == table_config::TERNARY_MATCH) {
        for (auto &w : match) {
            w.data0 = ~0UL;
#if MCN_TEST(MODEL_CHIP_NAMESPACE,rsvd1)
            w.data1 = ~0UL;
#else /* !rsvd1 */
            w.data1 = ~1UL;
#endif
        }
    } else if (st->type == table_config::PHASE0_MATCH) {
        hash_input.resize(1);
    } else if (st->type == table_config::ACTION_DATA) {
        if (table->binding != "indirect") {
            error("%s is not an indirect action data table", name);
            return; }
        if (table->match_tables.empty()) {
            error("No match table for action table %s", name);
            return; }
        match.clear();
    } else {
        error("%s is not a match or action data table", name);
        return; }
    indirect.resize(st->indirect.width);

    /* process the operands of the add command -- loop over the operands ignoring ,() looking
     * for name:value pairs (which might be keys or action data, a single number (the address)
     * and a single name (the action to run).  All may be in any order, though action data
     * params must be after the action (otherwise we don't know where to put them) and all
     * exact match keys and the address must be before the action (we need to figure out which
     * match group to use to store the action code).  Value seens are packed into the
     * 'hash_input', 'match', 'indirect' and 'actiondata' vectors in the format needed for
     * that table object */
    char punct;
    int addr = -1, match_addr = -1, exact_addr = -1;
    int act_index = -1;
    int adt_addr = -1;  // indirect action data table address (action_profile)
    std::set<std::string>       seen;
    std::vector<std::pair<const char *, char *>>  match_fields;
    while ((name = token(&args, " \t\r\n:(),", &punct))) {
        if (*name == '#') {
            name = nullptr;
            break; }
        if (*name == '=') {
            if (!*++name)
                name = token(&args);
            break; }
        int addr_stride = st->entries;
        if (st->type == table_config::HASH_MATCH) {
            addr_stride = st->ways.size() * st->table.entries_per_word(); }
        while (addr >= addr_stride && addr_stride > 0) {
            addr -= addr_stride;
            if (add_all_stages || st-- == &table->stage_tables.front()) {
                error("address too large for table %s", table->name.c_str());
                return; }
            addr_stride = st->entries;
            if (st->type == table_config::HASH_MATCH) {
                addr_stride = st->ways.size() * st->table.entries_per_word(); }
            assert(st->type == st[1].type);
            assert(size_t(st->table.width) == match.size());
            assert(size_t(st->indirect.width) == indirect.size()); }
        if (isspace(punct)) {
            punct = skip(&args);
            if (strchr(":(),", punct)) args++; }
        if (punct == ':') {
            skip(&args);
            if (addr < 0) {
                int len;
                if (name == (st->type == table_config::HASH_MATCH ? "way" : "priority")) {
                    if (sscanf(args, "%d%n", &addr, &len) < 1 || addr < 0) {
                        error("bad %s", name);
                        return; }
                    args += len;
                    continue;
                } else if (st->type == table_config::TERNARY_MATCH) {
                    error("must specify address (priority)");
                    return; } }
            if (seen.count(name)) {
                error("duplicate value for %s", name);
                token(&args);
                continue; }
            auto orig_name = seen.insert(name).first;
        retry_canon:
            bool ok = false;
            if (st->type == table_config::TERNARY_MATCH) {
                ok |= fill_ternary_field(name, match, st->table, args);
                if (st->indirect)
                    ok |= fill_field(name, pipes, indirect, st->indirect, match_addr, immed_remap,
                                     args, action_handle);
                if (adtable)
                    ok |= fill_field(name, pipes, actiondata, *adtable,
                                     adt_addr >= 0 ? adt_addr : match_addr, nullptr, args,
                                     action_handle);
            } else if (st->type == table_config::ACTION_DATA) {
                if (addr < 0 || !adtable)
                    error("need address and action name for action data table %s",
                          table->name.c_str());
                else
                    ok |= fill_field(name, pipes, actiondata, *adtable, addr, nullptr, args,
                                     action_handle);
            } else if (st->type == table_config::HASH_ACTION) {
                ok |= fill_field(name, pipes, hash_input, st->hash_input, nullptr,
                                 args, action_handle);
                if (adtable && addr >= 0)
                    ok |= fill_field(name, pipes, actiondata, *adtable, addr, nullptr,
                                     args, action_handle);
            } else if (addr < 0) {
                if ((ok = fill_field(name, pipes, hash_input, st->hash_input, nullptr, args,
                                     action_handle))) {
                    match_fields.emplace_back(name, args);
                } else if (!strchr(name, '.'))
                    error("%s is not a match field for table %s", name, table->name.c_str());
            } else {
                if (st->type != table_config::HASH_ACTION) {
                    ok |= fill_field(name, pipes, match, st->table, addr, immed_remap,
                            args, action_handle); }
                if (adtable)
                    ok |= fill_field(name, pipes, actiondata, *adtable,
                                     adt_addr >= 0 ? adt_addr : match_addr, nullptr, args,
                                     action_handle);
                if (fill_field(name, pipes, hash_input, st->hash_input, nullptr, args,
                               action_handle)) {
                    ok = true;
                    match_fields.emplace_back(name, args);
                    if (act_index >= 0)
                        error("must specify match fields before action"); } }
            if (!ok && p4_params && p4_params->count(name)) {
                // if mentioned in p4_params and nowhere else, must have been dead-code
                // eliminated by the compiler?  Should not be an error so ignore it.
                LOG3("ignoring dead? field " << name);
                ok = true; }
            if (ok) {
                seen.insert(name);
            } else {
                if (strchr(name, '.') || strchr(name, '$')) {
                    while (auto *p = strchr(name, '.')) *p = '_';
                    while (auto *p = strchr(name, '$')) *p = '_';
                    goto retry_canon; }
                error("no field %s related to table %s", orig_name->c_str(),
                      table->name.c_str()); }
            skip_to(&args, " \t\r\n(),");
        } else if (addr < 0 && isdigit(*name)) {
            if (st->type == table_config::PHASE0_MATCH) {
                error("explicit address not supported on phase 0 table %s", table->name.c_str());
            } else if (st->type == table_config::HASH_ACTION) {
                error("explicit address not supported on hash action table %s",
                      table->name.c_str());
            } else {
                char *p;
                addr = strtol(name, &p, 0);
                if (*p) {
                    error("invalid table address %s", name);
                    addr = -1; } }
        } else if (act_index >= 0 && adt_addr < 0 && isdigit(*name)) {
            if (st->adt_indirect) {
                char *p;
                adt_addr = strtol(name, &p, 0);
                if (*p)
                    error("invalid table address %s", name);
                else if (!fill_overhead_field(st, table_config::ADT_PTR,
                                              build_adt_full_addr(adt_addr, *adtable),
                                              addr, match, match_addr, indirect))
                    error("no indirect action data pointer in %s", table->name.c_str());
            } else {
                error("table %s does not have an indirect action data table", table->name.c_str());
            }
        } else if (act_index < 0) {
            if (st->type == table_config::TERNARY_MATCH) {
                if (addr < 0) {
                    error("must specify address (priority)");
                    return; }
            } else if (st->type == table_config::ACTION_DATA) {
                if (addr < 0) {
                    error("must specify action data address");
                    return; }
            } else if (st->type == table_config::PHASE0_MATCH) {
                addr = hash_input.at(0).data0;
                continue;
            } else if (st->type == table_config::HASH_ACTION) {
                addr = find_hash_action_addr(pipes, *st, hash_input);
            } else {
                if (addr < 0)
                    addr = find_unused_way(pipes, table, *st, hash_input);
                int mgroup = addr % st->table.entries_per_word();
                for (auto mf : match_fields)
                    fill_field(mf.first, pipes, match, st->table.fields.at(mgroup).by_name,
                               0, mf.second, 0);
            }

            if (st->type == table_config::TERNARY_MATCH) {
                int vpn = st->table.mem.at(addr/512).vpn.at(0);
                match_addr = addr % 512 + vpn * 512;
            } else if (st->type == table_config::ACTION_DATA) {
                match_addr = addr;
            } else if (st->type == table_config::HASH_ACTION) {
                match_addr = addr;
            } else {
                exact_addr = find_exact_addr(pipes, *st, hash_input, addr);
                match_addr = exact_addr / st->table.entries_per_word(); }
            const auto *act = find_action(st, name);
            if (act != nullptr) {
                act_index = act->code;
                if (act->action_data) {
                    adtable = &act->action_data;
                    actiondata.resize(act->action_data.width);
                    LOG1("setup action again width=" << act->action_data.width);
                }
                action_handle = act->handle;
                p4_params = &act->p4_parameters;
                if (st->type != table_config::ACTION_DATA &&
                    st->type != table_config::HASH_ACTION) {
                    immed_remap = &act->immed_remap;
                    for (auto &fover : act->field_override) {
                        LOG1("We are about to override 0x" << hex(fover.value));
                        // With hash dist, override is set to true
                        fill_overhead_field(st, fover.type, fover.value, addr, match,
                                            match_addr, indirect);
                    }
                    if (act->next_table >= 0)
                        fill_overhead_field(st, table_config::NEXT_TABLE, act->next_table, addr,
                                            match, match_addr, indirect); } }

            if (act_index < 0) {
                error("no action %s in table %s", name, table->name.c_str());
                return;
            } else if (st->type != table_config::ACTION_DATA &&
                       st->type != table_config::HASH_ACTION) {
                if (!fill_overhead_field(st, table_config::INSTRUCTION, act_index, addr, match,
                                         match_addr, indirect) && act_index > 0)
                    error("can't find address field for table %s", table->name.c_str()); }
        } else
            error("multiple actions for table %s?", table->name.c_str()); }

    /* figure out any match keys that haven't been supplied and warn about them */
    std::map<int, std::pair<bool, std::string>> match_field_info;
    if (st->table.fields.empty()) {
        /* hash_action table with no actual match?  should check the hash_input to
         * make sure it is all present.  Maybe do that in all exact match cases, as we
         * also want to ensure ghost bits specified? */
    } else {
        for (auto &f : st->table.fields.at(0).by_name) {
            if (f.second.source != table_config::SPEC) continue;
            auto &info = match_field_info[f.second.start_bit];
            if (seen.count(f.first))
                info.first = true;
            if (f.first.size() > info.second.size())
                info.second = f.first; } }
    for (auto &i : Values(match_field_info)) {
        if (!i.first)
            warning("No key value for %s, will match against 0", i.second.c_str()); }

    if (immed_remap) {
        if (st->indirect) {
            int entry = match_addr % st->indirect.entries_per_word();
            fill_const_fields(indirect, st->indirect.fields.at(entry).by_name, immed_remap); }
        int entry = addr % st->table.entries_per_word();
        fill_const_fields(match, st->table.fields.at(entry).by_name, immed_remap); }

    /* now actually go and write the various chunks of data to the rams/tcams of the tables */
    do {
        if (st->type == table_config::TERNARY_MATCH) {
            write_tcam(pipes, table, *st, st->table, match, addr);
            write_sram(pipes, table, *st, st->indirect, indirect, match_addr);
        } else if (st->type == table_config::PHASE0_MATCH) {
            set_port_phase0(addr, match.at(0).data0, match.at(0).data1);
        } else if (st->type != table_config::ACTION_DATA &&
                   st->type != table_config::HASH_ACTION) {
            auto entry = addr % st->table.entries_per_word();
            for (auto *field : st->table.fields.at(entry).iter(table_config::VERSION))
                fill_sram_field(match, *field, 0xf);
            for (auto *field : st->table.fields.at(entry).iter(table_config::VALID))
                fill_sram_field(match, *field, 1);
            write_sram(pipes, table, *st, st->table, match, exact_addr); }
        if (adtable) {
            if (adt_addr >= 0) {
                write_sram(pipes, table, *st, *adtable, actiondata, adt_addr);
            } else if (!st->adt_indirect) {
                write_sram(pipes, table, *st, *adtable, actiondata, match_addr);
            } else {
                /* indirect table, but no adt_addr specified -- assume the action data
                 * table will have its entries entered independently */ } }
        if (!add_all_stages) break;
        if (st == &table->stage_tables[0]) break;
        --st;
        assert(st->type == st[1].type);
        assert(st->type != table_config::PHASE0_MATCH);
        assert(size_t(st->table.width) == match.size());
        assert(size_t(st->indirect.width) == indirect.size());
        if (exact_addr >= 0) {
            exact_addr = find_exact_addr(pipes, *st, hash_input, addr);
            match_addr = exact_addr / st->table.entries_per_word(); }
    } while (true);

    for (auto a : actiondata) {
        LOG1("actiondata " << hex(a.data0, 16, '0') << " " << hex(a.data1, 16, '0'));
    }
    if (name) {
        char buf[32], *p = buf;
        if (st != table->stage_tables.data())
            p += sprintf(p, "%d.", int(st - table->stage_tables.data()));
        sprintf(p, "%d", match_addr);
        setenv(name, buf, 1); }
}

ScriptCommand(setdefault, "<table> <action>({<name>:<value>})") {
    const char *table_name = token(&args);
    unsigned pipes = ALL_PIPES;
    auto table = table_config::get(table_name, pipes);
    if (!table) return;
    const table_config::stage *st = &table->stage_tables.back();
    bool add_all_stages = false;  // table that needs entries added to all stage tables
    if (table->stage_tables.size() > 1 && table->entries > 0 && st->entries >= table->entries) {
        // whether we need to add an entry to all stages (eg, for a dleft table) should be
        // specified in the context.json, but its not.  So we infer it if the stage tables
        // all have as many entries as the table as a whole
        add_all_stages = true;
        for (auto &s : table->stage_tables) {
            if (s.entries < table->entries) {
                add_all_stages = false;
                break; } } }
    if (st->type == table_config::MATCH_NO_KEY) {
        error("%s is a match with no key table, so cannot set a default action", table_name);
        return;
    } else if (st->type == table_config::HASH_ACTION) {
        // For a hash_action table, we need to write the default action to every element
        // of the table (and NOT touch the miss regsiters, which might be needed to run
        // miss action on the associated gateway)
        // FIXME -- would be much faster to figure out the full word value to write all
        // the entries in a word at once, then just write that to all rows of the ram with
        // write_sram in a loop
        int key_pos = -1;
        for (auto &key_field : Values(st->hash_input)) {
            if (key_pos == key_field.start_offset) continue;  // skip duplicate names
            if (key_pos != -1) {
                error("hash action %s has mulitple key fields; only 1 supported for setdefault",
                      table_name);
                return; }
            uint64_t mask = 0, ignore_mask = (1ULL << key_field.bit_width) - 1;
            for (auto &col : Values(st->hash_functions.at(0))) {
                mask |= col.col.at(key_field.start_bit/64U) << (key_field.start_bit%64U);
                if (key_field.start_bit%64U + key_field.bit_width > 64) {
                    mask |= col.col.at(key_field.start_bit/64U + 1) >>
                            (64 - key_field.start_bit%64U); } }
            mask &= ignore_mask;
            ignore_mask &= ~mask;
            // iterate over all of the inputs that differ in bits relevant to the hash function
            uint64_t idx = 0;
            do {
                std::stringstream cmd;
                cmd << "add " << table_name << " " << key_field.name << ":"
                    << (idx << key_field.start_bit) << " " << args;
                if (Script::run("<hash action setdefaul>", cmd)) {
                    // abort if there was an error
                    return; }
                idx = ((idx | ignore_mask) + 1) & mask;
            } while (idx != 0); }
        return; }
    if (st->type != table_config::HASH_MATCH &&
        st->type != table_config::ATCAM_MATCH &&
        st->type != table_config::TERNARY_MATCH) {
        error("%s is not a match table", table_name);
        return; }
    auto action_name = token(&args, " \t\r\n(,");
    uint64_t immediate = 0;
    const auto *act = find_action(st, action_name);
    if (!act || act->full_address < 0) {
        error("no action %s in table %s", action_name, table_name);
        return; }
    std::vector<word_t> actiondata(act->action_data.width);
    char punct;
    std::set<std::string>       seen;
    while (auto name = token(&args, " \t\r\n:),", &punct)) {
        if (punct != ':') {
            error("sytax error");
            return; }
        if (seen.count(name)) {
            error("duplicate value for %s", name);
            token(&args);
            continue; }
        seen.insert(name);
        bool ok = fill_immed(name, immediate, &act->immed_remap, args);
        if (act->action_data)
            ok |= fill_field(name, pipes, actiondata, act->action_data, 0xff, nullptr, args, 0);
        if (!ok) error("no argument %s for %s table %s", name, action_name, table_name);
        skip_to(&args, " \t\r\n(),"); }
    for (auto &r : Values(act->immed_remap)) {
        if (!r.constant) continue;
        uint64_t mask = ~uint64_t(0);
        if (r.bit_width < 64) {
            mask = ~(mask << r.bit_width);
        } else if (r.bit_width > 64)
            error("immediate fields >64 bits not supported");
        immediate |= ((r.constant_value >> r.start_bit) & mask) << r.immed_bit; }

    do {
        for (int pipe = 0; (1U << pipe) <= pipes; pipe++) {
            if (!((1U << pipe) & pipes)) continue;
            int die = PIPE2DIE(pipe);
            int phy_pipe = PIPE_WITHIN_DIE(pipe);
#if MCN_TEST(MODEL_CHIP_NAMESPACE,rsvd1)
            bool egr = table->direction == table_config::EGRESS;
            auto &mrd = RegisterUtils::ref_ppu(phy_pipe, st->stage, egr).ppu_mrd;
            LOG3("pipe[" << phy_pipe << "]." << "ie"[egr] << "ppu[" << st->stage << "].mrd"
                 ".mrd_imem_map_erf.mrd_imem_map[" << st->physical_id << "][8] = " <<
                 table_name << "." << action_name << " = 0x" << hex(act->full_address) <<
                 ", die: " << die);
            write_reg(die, &mrd.mrd_imem_map_erf.mrd_imem_map[st->physical_id][8],
                      act->full_address);
            LOG3("pipe[" << phy_pipe << "]." << "ie"[egr] << "ppu[" << st->stage << "].mrd"
                 ".mrd_iad_map_erf.mrd_iad_map[" << st->physical_id << "][4] = 0x" <<
                 hex(immediate));
            write_reg(die, &mrd.mrd_iad_map_erf.mrd_iad_map[st->physical_id][4].mrd_iad_map_0_2,
                      immediate & 0xffffffff);
            write_reg(die, &mrd.mrd_iad_map_erf.mrd_iad_map[st->physical_id][4].mrd_iad_map_1_2,
                      immediate >> 32);
#else /* not rsvd1 */
            auto &merge = RegisterUtils::ref_mau(phy_pipe, st->stage).rams.match.merge;
            LOG3("pipe[" << phy_pipe << "].mau[" << st->stage << "].rams.match.merge"
                 ".mau_action_instruction_adr_miss_value[" << st->logical_id << "] = " <<
                 table_name << "." << action_name << " = 0x" << hex(act->full_address) <<
                 ", die: " << die);
            write_reg(die, &merge.mau_action_instruction_adr_miss_value[st->logical_id],
                      act->full_address);
            LOG3("pipe[" << phy_pipe << "].mau[" << st->stage << "].rams.match.merge"
                 ".mau_immediate_data_miss_value[" << st->logical_id << "] = 0x" <<
                 hex(immediate) << ", die: " << die);
            write_reg(die, &merge.mau_immediate_data_miss_value[st->logical_id], immediate);
#endif
            for (auto &fover : act->field_override) {
#if !MCN_TEST(MODEL_CHIP_NAMESPACE,rsvd1)
                auto addr = fover.value;
#endif
                switch(fover.type) {
                case table_config::STFUL_PTR:
                    // Need to expand 26 bit address back into 27 bit form with the extra type bit
                    // to denote a stateful instruction address
                    // addr = ((addr & ~0xffffff) << 1) | 0x1000000 | (addr & 0xffffff);
                    // fall through
                case table_config::METER_PTR:
#if MCN_TEST(MODEL_CHIP_NAMESPACE,rsvd1)
                    // FIXME -- need to know which physical stateful/meter it is, but the info
                    // FIXME -- is not in the context.json
#else /* not rsvd1 */
                    write_reg(die, &merge.mau_meter_adr_miss_value[st->logical_id], addr);
                    LOG3("pipe[" << phy_pipe << "].mau[" << st->stage << "].rams.match."
                         "merge.mau_meter_adr_miss_value[" << st->logical_id <<
                         "] = 0x" << hex(addr) << ", die: " << die);
#endif
                    break;
                case table_config::STATS_PTR:
#if MCN_TEST(MODEL_CHIP_NAMESPACE,rsvd1)
                    // FIXME -- need to know which physical stats unit it si, but the info
                    // FIXME -- is not in the context.json
#else /* not rsvd1 */
                    write_reg(die, &merge.mau_stats_adr_miss_value[st->logical_id], addr);
                    LOG3("pipe[" << phy_pipe << "].mau[" << st->stage << "].rams.match.merge."
                         "mau_stats_adr_miss_value[" << st->logical_id << "] = 0x" << hex(addr) <<
                         ", die: " << die);
#endif
                    break;
                default:
                    LOG3("ingnoring field_override type " << fover.type <<
                         " 0x" << hex(fover.value));
                    break; } }
            if (act->next_table_full >= 0) {
#if MCN_TEST(MODEL_CHIP_NAMESPACE,rsvd1)
                char *addr = (char *) &mrd.mrd_pred_map_erf.mrd_pred_map[st->physical_id][16];
                LOG3("pipe[" << phy_pipe << "]." << "ie"[egr] << "ppu[" << st->stage << "].mrd"
                     ".mrd_pred_map_erf.mrd_pred_map[" << st->physical_id << "][16] = (0x" <<
                     hex(act->next_table_global_exec) << ", 0x" <<
                     hex(act->next_table_long_brch) << ", 0x" << hex(act->next_table_full) <<
                     ", 0x" << hex(act->next_table_local_exec) << ")");
                write_reg(die, addr, act->next_table_global_exec | (act->next_table_long_brch<<24));
                write_reg(die, addr+4, act->next_table_full | (act->next_table_local_exec << 8));
#else /* not rsvd1 */
                LOG3("pipe[" << phy_pipe << "].mau[" << st->stage << "].rams.match.merge"
                     ".next_table_format_data[" << st->logical_id << "] = 0x" <<
                     hex(act->next_table_full) << ", die: " << die);
                unsigned next_mask = 0xff;
#if MCN_TEST(MODEL_CHIP_NAMESPACE,jbay) || MCN_TEST(MODEL_CHIP_NAMESPACE,jbayB0) || MCN_TEST(MODEL_CHIP_NAMESPACE,rsvd0)
                next_mask = 0x1ff;
#endif
                write_reg(die, &merge.next_table_format_data[st->logical_id],
                        (read_reg(die, &merge.next_table_format_data[st->logical_id]) & ~next_mask)
                        | act->next_table_full);
#endif
            }

#if MCN_TEST(MODEL_CHIP_NAMESPACE,jbay) || MCN_TEST(MODEL_CHIP_NAMESPACE,jbayB0) || MCN_TEST(MODEL_CHIP_NAMESPACE,rsvd0)
            LOG3("pipe[" << phy_pipe << "].mau[" << st->stage << "].rams.match.merge"
                 ".pred_miss_exec[" << st->logical_id << "] = 0x" <<
                 hex(act->next_table_global_exec | (act->next_table_local_exec << 16)) <<
                 ", die: " << die);
            write_reg(die, &merge.pred_miss_exec[st->logical_id],
                      act->next_table_global_exec | (act->next_table_local_exec << 16));
            LOG3("pipe[" << phy_pipe << "].mau[" << st->stage << "].rams.match"
                 ".merge.pred_miss_long_brch[" << st->logical_id << "] = 0x" <<
                 hex(act->next_table_long_brch) << ", die: " << die);
            write_reg(die, &merge.pred_miss_long_brch[st->logical_id], act->next_table_long_brch);
#endif
        }

        if (act->action_data) {
            unsigned max_addr = act->action_data.mem.size() * act->action_data.fields.size()
                              * 1024 - 1;
            write_sram(pipes, table, *st, act->action_data, actiondata, max_addr);
            if (act->action_data.width == 1) {
                // insert huffman bits
                unsigned bit_width = 128 / act->action_data.fields.size();
                max_addr *= bit_width / 4U;
                max_addr += bit_width/8U - 1;
            } else {
                // huffman bits for wide tables are split between bits 0-4 and bits 15+
                max_addr <<= 5;
                max_addr += 31;
                unsigned vpn = max_addr >> 15;
                vpn *= act->action_data.width;
                vpn += act->action_data.width/2U - 1;
                max_addr = (max_addr & 0x3fff) + (vpn << 15); }
            max_addr |= 1 << 22;  // enable bit
#if !MCN_TEST(MODEL_CHIP_NAMESPACE,rsvd1)
            for (int pipe = 0; (1U << pipe) <= pipes; pipe++) {
                if (!((1U << pipe) & pipes)) continue;
                int die = PIPE2DIE(pipe);
                int phy_pipe = PIPE_WITHIN_DIE(pipe);
                auto &merge = RegisterUtils::ref_mau(phy_pipe, st->stage).rams.match.merge;
                LOG3("mau[" << st->stage << "].rams.match.merge.mau_actiondata_adr_miss_value[" <<
                     st->logical_id << "] = " << hex(max_addr));
                write_reg(die, &merge.mau_actiondata_adr_miss_value[st->logical_id], max_addr); }
#endif // !WIP
        }
        if (!add_all_stages) break;
        if (st == &table->stage_tables[0]) break;
        --st;
        assert(st->type == st[1].type);
        assert(st->type != table_config::PHASE0_MATCH);
    } while (1);
}

static bool is_ternary_table(const char *name) {
    unsigned pipes = ALL_PIPES;
    if (auto table = table_config::get(name, pipes))
        return table->stage_tables.front().type == table_config::TERNARY_MATCH;
    return false;
}

struct synth2port_check {
    const table_config::stage   *st;
    table_config::gress_t       gress;
    const char  *name;
    const char  *table_type;
    const char  *what = nullptr;
    int         shift = 0;
    uint64_t    mask = ~UINT64_C(0);
    std::function<bool(uint64_t, uint64_t)> test;
    const char  *test_str = nullptr;
    const char  *envvar = nullptr;
    uint64_t val = 0;
    synth2port_check(table_config::gress_t gr, const table_config::stage *st, const char *n,
                     const char *tt) : st(st), gress(gr), name(n), table_type(tt) {}
    void parse_test(char **args) {
        char op = ' ';
        mask = ~(uint64_t)0;
        if ((what = Script::token(args, " \t\n\r[&=<>", &op))) {
            if (*what == '.' && *++what == 0 && !(what = Script::token(args, " \t\n\r[&=<>", &op)))
                return;
            if (isspace(op) && (op = Script::skip(args))) ++*args;
            if (op == '[') {
                int hi, lo, len = -1;
                if (sscanf(*args, "%d :%d ]%n", &hi, &lo, &len), len > 0) {
                    shift = lo;
                    if (hi - lo + 1 < 64)
                        mask = (UINT64_C(1) << (hi - lo + 1)) - 1;
                    else if (hi - lo + 1 > 64)
                        error("can't check more than 64 bits");
                    *args += len;
                    if ((op = Script::skip(args))) ++*args; }
            } else if (st->table.fields[0].by_name.count(what)) {
                for (auto &fld : ValuesForKey(st->table.fields[0].by_name, what)) {
                    if (fld.bit_width > 64)
                        error("can't check more than 64 bits at a time");
                    break; } }
            if (op == '&') {
                auto *p = Script::token(args, " \t\n\r=<>", &op);
                if (p) mask &= strtoull(p, &p, 0);
                if (!p || *p || !mask)
                    error("syntax error");
                if (isspace(op) && (op = Script::skip(args))) ++*args; }
            switch (op) {
            case '=':
                if (**args == '=') ++*args;
                test = std::equal_to<uint64_t>();
                test_str = "";
                break;
            case '<':
                if (**args == '=') {
                    ++*args;
                    test = std::less_equal<uint64_t>();
                    test_str = "<=";
                } else {
                    test = std::less<uint64_t>();
                    test_str = "<"; }
                break;
            case '>':
                if (**args == '=') {
                    ++*args;
                    test = std::greater_equal<uint64_t>();
                    test_str = ">=";
                } else {
                    test = std::greater<uint64_t>();
                    test_str = ">"; }
                break;
            default:
                what = nullptr;  // avoid crash with invalid test function
                error("syntax error");
                return; }
            auto *p = Script::token(args, " \t\n\r");
            if (p) {
                if (!*test_str && isalpha(*p)) {
                    envvar = p;
                    p += strcspn(p, " \t\n\r");
                } else {
                    errno = 0;
                    val = strtoull(p, &p, 0);
                    if (errno == ERANGE)
                        error("value too large for 64 bits"); } }
            if (!p || *p)
                error("syntax error"); } }
    bool operator()(uint64_t v) const { return test(v & mask, val); }
};

bool synth2port_read(int pipe, const table_config *tc, const table_config::stage *st,
                     uint32_t addr, uint64_t data[2]) {
    int die = PIPE2DIE(pipe);
    int phy_pipe = PIPE_WITHIN_DIE(pipe);
    StageRef stage(die, phy_pipe, st->stage, tc->direction);
    unsigned vpn = addr / 1024U;
    addr = addr % 1024U;
    uint64_t mapdata;
    data[0] = data[1] = 0;
    for (auto &m : st->table.mem) {
#if MCN_TEST(MODEL_CHIP_NAMESPACE,rsvd1)
        auto vpn_it = m.vpn.begin();
#endif
        for (int unit : m.units) {
#if MCN_TEST(MODEL_CHIP_NAMESPACE,rsvd1)
            if (*vpn_it++ != vpn) continue;
#else
            stage.mapram_lookup(unit).read(addr, &mapdata, 0, 0);
            if (mapdata != (vpn ^ 0x3f)) continue;
#endif
            stage.sram_lookup(unit).read(addr, &data[0], &data[1], 0);
            LOG3("synth2port_read(" << phy_pipe << ':' << st->stage << ':' << st->logical_id <<
                 ", 0x" << hex(addr) << ", 0x" << hex(data[0], 16, '0') <<
                 ", 0x" << hex(data[1], 16, '0') << "), die: " << die);
            return true; } }
    if (st->table.spare_bank >= 0) {
        int unit = st->table.spare_bank;
        stage.mapram_lookup(unit).read(addr, &mapdata, 0, 0);
        if (mapdata == (vpn ^ 0x3f)) {
            stage.sram_lookup(unit).read(addr, &data[0], &data[1], 0);
            LOG3("synth2port_read(" << phy_pipe << ':' << st->stage << ':' << st->logical_id <<
                 ", 0x" << hex(addr) << ", 0x" << hex(data[0], 16, '0') <<
                 ", 0x" << hex(data[1], 16, '0') << "), die: " << die);
            return true; } }
    return false;
}
bool synth2port_write(int pipe, const table_config *tc, const table_config::stage *st,
                      uint32_t addr, uint64_t data[2]) {
    int die = PIPE2DIE(pipe);
    int phy_pipe = PIPE_WITHIN_DIE(pipe);
    LOG3("synth2port_write(" << phy_pipe << ':' << st->stage << ':' << st->logical_id << ", 0x" <<
         hex(addr) << ", 0x" << hex(data[0], 16, '0') << ", 0x" << hex(data[1], 16, '0') << ')' <<
         ", die: " << die);
    StageRef stage(die, phy_pipe, st->stage, tc->direction);
    unsigned vpn = addr / 1024U;
    addr = addr % 1024U;
    uint64_t mapdata;
    int spare = -1;
    for (auto &m : st->table.mem) {
#if MCN_TEST(MODEL_CHIP_NAMESPACE,rsvd1)
        auto vpn_it = m.vpn.begin();
#endif
        for (int unit : m.units) {
#if MCN_TEST(MODEL_CHIP_NAMESPACE,rsvd1)
            if (*vpn_it++ != vpn) continue;
#else
            stage.mapram_lookup(unit).read(addr, &mapdata, 0, 0);
            if (mapdata != (vpn ^ 0x3f)) {
                if (spare == -1 && mapdata == 0)
                    spare = unit;
                continue; }
#endif
            LOG1("write_sram(" << phy_pipe << ", " << st->stage << ":" << unit << ":" << addr <<
                 ", " << vpn << ", " << hex(data[1],16,'0') << hex(data[0],16,'0') <<
                 "), die: " << die);
            stage.sram_lookup(unit).write(addr, data[0], data[1], 0);
            return true; } }
    if (st->table.spare_bank >= 0) {
        int unit = st->table.spare_bank;
        stage.mapram_lookup(unit).read(addr, &mapdata, 0, 0);
        if (mapdata == (vpn ^ 0x3f)) {
            LOG1("write_sram(" << phy_pipe << ", " << st->stage << ":" << unit << ":" << addr <<
                 ", " << vpn << ", " << hex(data[1],16,'0') << hex(data[0],16,'0') << "), die: " <<
                 die);
            stage.sram_lookup(unit).write(addr, data[0], data[1], 0);
            return true;
        } else if (spare == -1 && mapdata == 0) {
            spare = unit; } }
    if (spare >= 0) {
        mapdata = vpn ^ 0x3f;
        stage.mapram_lookup(spare).write(addr, mapdata, 0, 0);
        LOG1("write_mapram(" << phy_pipe << ", " << st->stage << ":" << spare << ":" << addr <<
             ", " << vpn << ", " << hex(mapdata,3,'0') << "), die: " << die);
        stage.sram_lookup(spare).write(addr, data[0], data[1], 0);
        LOG1("write_sram(" << phy_pipe << ", " << st->stage << ":" << spare << ":" << addr <<
             ", " << vpn << ", " << hex(data[1],16,'0') << hex(data[0],16,'0') << "), die: " <<
             die);
        return true; }
    error("no spare bank available");
    return false;
}

/* FIXME -- should check_synth2port should use synth2port_read rather than duplicating it? */
static bool check_synth2port_value(int pipe, const synth2port_check &check, int unit,
                                   int addr, int entry) {
#if !MCN_TEST(MODEL_CHIP_NAMESPACE,rsvd1)
    unsigned vpn = addr / 1024U;
    uint64_t mapdata = 0;
#endif
    addr = addr % 1024;
    std::vector<word_t> data(1);
    int die = PIPE2DIE(pipe);
    int phy_pipe = PIPE_WITHIN_DIE(pipe);
    StageRef stage(die, phy_pipe, check.st->stage, check.gress);
    stage.sram_lookup(unit).read(addr, &data[0].data0, &data[0].data1, 0);
#if MCN_TEST(MODEL_CHIP_NAMESPACE,rsvd1)
    LOG3("read_sram(" << check.st->stage << ":" << unit << ":" << addr << ") = " <<
         hex(data[0].data1,16,'0') << hex(data[0].data0,16,'0'));
#else
    stage.mapram_lookup(unit).read(addr, &mapdata, 0, 0);
    LOG3("read_sram(" << check.st->stage << ":" << unit << ":" << addr << ") = " <<
         hex(data[0].data1,16,'0') << hex(data[0].data0,16,'0') << "  mapram = " << hex(mapdata));
    if (mapdata != (vpn ^ 0x3f))
        return false;
#endif
    bool ok = false;
    for (auto &field : check.st->table.fields.at(entry).by_name) {
        if (!check.what) {
            uint64_t val = get_sram_field(data, field.second);
            if (!ok)  {
                printf("%s %s(physical %d):", check.table_type, check.name,
                       addr * check.st->table.entries_per_word() + entry);
                ok = true;
            }
            printf(" %s=%" PRIu64, field.first.c_str(), val);
        } else if (field.first == check.what) {
            uint64_t val = get_sram_field_slice(data, field.second, check.shift, -1);
            if (check.envvar) {
                char buf[32];
                sprintf(buf, "%" PRIu64, val & check.mask);
                setenv(check.envvar, buf, 1);
            } else if (!check(val)) {
                error("%s %s(physical %d) %s value is %" PRIu64 ", expected %s%" PRIu64 "\n",
                      check.table_type, check.name,
                      addr * check.st->table.entries_per_word() + entry,
                      check.what, val & check.mask, check.test_str, check.val); }
            ok = true;
            break; } }
    if (!ok) {
        if (check.what)
            error("%s %s has no field named %s", check.table_type, check.name, check.what);
        else
            error("%s %s has no fields?", check.table_type, check.name);
    } else if (!check.what) {
        printf("\n");
        fflush(stdout);
    }
    return true;
}

static void check_synth2port(int pipe, const synth2port_check &check, int addr, int entry) {
    bool found = false;
    for (auto &m : check.st->table.mem)
        for (size_t i = 0; i < m.units.size(); ++i) {
#if MCN_TEST(MODEL_CHIP_NAMESPACE,rsvd1)
            if (addr/1024U != m.vpn[i]) continue;
#endif
            found |= check_synth2port_value(pipe, check, m.units[i], addr, entry); }
    if (check.st->table.spare_bank >= 0)
        found |= check_synth2port_value(pipe, check, check.st->table.spare_bank, addr, entry);
    if (!found && check.what) {
        if (check.envvar)
            setenv(check.envvar, "0", 1);
        else if (check(0))
            warning("no value with matching vpn for %s %s, but 0 is ok",
                    check.table_type, check.name);
        else
            error("no value with matching vpn for %s %s", check.table_type, check.name);
    }
}

ScriptCommand(check_counter, "[<pipe>:]<table>(<index>) <field> = <value>") {
    unsigned pipes = pipe_prefix(&args);
    const char *table_name = token(&args, " \t\r\n()");
    auto table = table_config::get(table_name, pipes);
    if (!table) return;
    int pipe = 0;
    while (!(pipes & (1 << pipe))) ++pipe;
    auto index_str = token(&args, " \t\r\n()");
    int addr = index_str ? strtol(index_str, &index_str, 0) : 0;
    const table_config::stage *st = table->stage_tables.data();
    synth2port_check check(table->direction, st, table_name, "counter");
    if (index_str && *index_str == '.') {
        if (size_t(addr) >= table->stage_tables.size())
            error("%s has only %d stages", table_name, int(table->stage_tables.size()));
        else
            st += addr;
        addr = strtoull(index_str+1, &index_str, 0); }
    if (!index_str || *index_str) {
        error("syntax error");
        return; }
    check.parse_test(&args);
    if (st->type != table_config::STATISTICS) {
        error("%s is not a counter table", table_name);
        return; }
    int virt_addr, entry = 0;
    switch (st->table.entries_per_word()) {
    case 1: virt_addr = addr * 8U; break;
    case 2: entry = addr & 1; virt_addr = addr * 4U; addr /= 2U; break;
    case 3:
        if (is_ternary_table(table->match_tables.at(0).c_str())) {
            entry = (addr >> 13) & 3;
            addr = ((addr >> 2) & ~0x1fff) | (addr & 0x1fff);
        } else {
            entry = (addr >> 14) & 3;
            addr = ((addr >> 2) & ~0x3fff) | (addr & 0x3fff); }
        virt_addr = addr * 8U + entry;
        break;
    case 4: entry = addr & 3; virt_addr = addr * 2U; addr /= 4; break;
    case 6:
        if (is_ternary_table(table->match_tables.at(0).c_str())) {
            entry = ((addr >> 12) & 6) | (addr & 1);
            addr = ((addr >> 3) & ~0xfff) | ((addr >> 1) & 0xfff);
        } else {
            entry = ((addr >> 13) & 6) | (addr & 1);
            addr = ((addr >> 3) & ~0x1fff) | ((addr >> 1) & 0x1fff); }
        virt_addr = addr * 8U + entry/2U + ((entry&1) << 2);
        break;
    default:
        error("bad table format for %s", table_name);
        return; }
#if !MCN_TEST(MODEL_CHIP_NAMESPACE,rsvd1)
    std::vector<word_t> data(1);
    int die = PIPE2DIE(pipe);
    int phy_pipe = PIPE_WITHIN_DIE(pipe);
    RmtObjectManager *om = NULL;
    GLOBAL_MODEL->GetObjectManager(die, &om);
    auto *mau = om->mau_lookup(phy_pipe, st->stage);
    mau->mau_memory()->stats_virt_read_full(st->logical_id, virt_addr,
                                            &data[0].data0, &data[0].data1 , 0);
    LOG3("read_virt(" << st->stage << ":L" << st->logical_id << ":" << virt_addr <<
         ") = " << hex(data[0].data1,16,'0') << hex(data[0].data0,16,'0'));
    if (!check.what) {
        printf("counter %s(virtual %d):", table_name,
               (virt_addr/8U) * st->table.entries_per_word() + entry);
        for (auto &field : check.st->table.fields.at(entry).by_name) {
            uint64_t val = get_sram_field(data, field.second);
            printf(" %s=%" PRIu64, field.first.c_str(), val); }
        printf("\n");
        fflush(stdout); }
#endif // !WIP
#if 0
    // FIXME -- virt_read does not work properly if there are multiple counters attached
    // to one match table (one logical_id), so don't bother to check it for correctness
    else if (!strcmp(check.what, "packets") && !check(val))
        error("counter %s virtual value %" PRIu64 ", expected %s%" PRIu64 "\n",
              table_name, val, check.test_str, check.val);
#endif
    check_synth2port(pipe, check, addr, entry);
}

ScriptCommand(check_register, "[<pipe>:]<table>(<index>) <field> = <value>") {
    // FIXME -- refactor this better with check_counter, above
    unsigned pipes = pipe_prefix(&args);
    const char *table_name = token(&args, " \t\r\n()");
    auto table = table_config::get(table_name, pipes);
    if (!table) return;
    int pipe = 0;
    while (!(pipes & (1 << pipe))) ++pipe;
    auto index_str = token(&args, " \t\r\n()");
    int addr = index_str ? strtoull(index_str, &index_str, 0) : 0;
    const table_config::stage *st = table->stage_tables.data();
    synth2port_check check(table->direction, st, table_name, "register");
    if (index_str && *index_str == '.') {
        if (size_t(addr) >= table->stage_tables.size())
            error("%s has only %d stages", table_name, int(table->stage_tables.size()));
        else
            st += addr;
        addr = strtol(index_str+1, &index_str, 0); }
    if (!index_str || *index_str) {
        error("syntax error");
        return; }
    check.parse_test(&args);
    if (st->type != table_config::STATEFUL) {
        error("%s is not a stateful table", table_name);
        return; }
    unsigned epw = st->table.entries_per_word();
    int entry = addr & (epw - 1);
    addr /= epw;
    // FIXME -- How to do a virtual read of a stateful table?  MauMemory::virt_read
    // FIXME -- does not seem to work?
    check_synth2port(pipe, check, addr, entry);
}
void update_selector(int pipe, const table_config *tc, const table_config::stage *st, int addr,
                     int &word, int bit, bool remove, uint64_t data[2]) {
    if (bit/120U != (unsigned)word) {
        if (word >= 0)
            synth2port_write(pipe, tc, st, addr+word, data);
        if (bit >= 0) {
            word = bit/120;
            synth2port_read(pipe, tc, st, addr+word, data);
        } else
            word = -1;
    }
    bit %= 120;
    if (((data[bit/64] >> (bit%64)) & 1) == remove) {
        data[bit/64] ^= 1ULL << (bit%64);
        if (remove)
            data[1] -= 0x100000000000000ULL;
        else
            data[1] += 0x100000000000000ULL; }
}

ScriptCommand(selector, "[<pipe>:]<table>(<index>) [add|remove] <bits>...") {
    unsigned pipes = pipe_prefix(&args);
    const char *table_name = token(&args, " \t\r\n()");
    auto table = table_config::get(table_name, pipes);
    if (!table) return;
    auto index_str = token(&args, " \t\r\n()");
    int addr = index_str ? strtoull(index_str, &index_str, 0) : 0;
    auto st = table->stage_tables.begin();
    while (addr >= st->entries) {
        addr -= st->entries;
        if (++st == table->stage_tables.end()) {
            error("address %s out of range for selector %s", index_str, table_name);
            return; } }
    const char *cmd = token(&args, " \t\r\n,()");
    bool remove = false;
    if (!strcmp(cmd, "remove"))
        remove = true;
    else if (strcmp(cmd, "add"))
        error("selector command must be 'add' or 'remove'");

    for (int pipe = 0; pipes; pipe++, pipes >>= 1) {
        if (!(pipes & 1)) continue;
        int word = -1;
        uint64_t data[2] = { 0, 0 };
        const char *p = args;
        int bit, len;
        while (sscanf(p, "%i %n", &bit, &len) >= 1) {
            p += len;
            if (*p == ',') ++p;
            update_selector(pipe, table, &*st, addr, word, bit, remove, data);
        }
        update_selector(pipe, table, &*st, addr, word, -1, remove, data);
    }
}
