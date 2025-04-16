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

#include <string.h>
#include <sstream>
#include "json-ref.h"
#include "log.h"
#include "map.h"
#include "p4name.h"
#include "script.h"
#include "table_config.h"

std::map<std::string, table_config> table_config::all;
std::map<std::string, table_config *> table_config::all_by_suffix;

table_config::memory_type_t table_config::memory_type(std::string t) {
    static std::map<std::string, memory_type_t> types = {
        { "tcam", tcam }, { "sram", sram }, { "map_ram", map_ram }, { "gateway", gateway },
        { "ingress_buffer", ingress_buffer }, { "stm", stm }, { "scm", scm },
        { "scm-tind", scm_tind }, { "lamb", lamb } };
    auto it = types.find(t);
    if (it == types.end()) return unknown;
    return it->second;
}

class ternary_value {
    uint64_t    value, dont_care;
    int chkmask(int maskbits) const {
        uint64_t mask = (1U << maskbits) - 1;
        int shift = 0;
        while (mask && ((value | dont_care) >> shift)) {
            if ((mask & dont_care) && (mask & dont_care) != mask)
                return -1;
            mask <<= maskbits;
            shift += maskbits; }
        return shift - maskbits; }
 public:
    ternary_value(uint64_t v, uint64_t dc) : value(v), dont_care(dc) {}
    friend std::ostream &operator<<(std::ostream &out, const ternary_value &tv) {
        int shift, bits;
        if ((shift = tv.chkmask((bits = 4))) >= 0)
            out << "0x";
        else if ((shift = tv.chkmask((bits = 3))) >= 0)
            out << "0o";
        else if ((shift = tv.chkmask((bits = 1))) >= 0)
            out << "0b";
        else
            shift = 0, bits = 1;
        uint64_t mask = uint64_t((1U << bits) - 1) << shift;
        for (; mask; shift -= bits, mask >>= bits)
            if (mask & tv.dont_care)
                out << '*';
            else
                out << "0123456789abcdef"[(tv.value & mask) >> shift];
        return out; }
};

static table_config::field_source_t field_src(json::obj_ref obj) {
    if (!obj) return table_config::NONE;
    auto src = obj.to<std::string>();
    if (src == "") return table_config::NONE;
    if (src == "adt_ptr") return table_config::ADT_PTR;
    if (src == "constant") return table_config::CONSTANT;
    if (src == "immediate") return table_config::IMMEDIATE;
    if (src == "instr") return table_config::INSTRUCTION;
    if (src == "meter_ptr") return table_config::METER_PTR;
    if (src == "next_table") return table_config::NEXT_TABLE;
    if (src == "parity") return table_config::PARITY;
    if (src == "payload") return table_config::PAYLOAD;
    if (src == "proxy_hash") return table_config::PROXY_HASH;
    if (src == "range") return table_config::RANGE;
    if (src == "selection_length") return table_config::SEL_LEN;
    if (src == "sel_ptr") return table_config::SEL_PTR;
    if (src == "selection_length_shift") return table_config::SEL_SHIFT;
    if (src == "stats_ptr") return table_config::STATS_PTR;
    if (src == "stful_ptr") return table_config::STFUL_PTR;
    if (src == "spec") return table_config::SPEC;
    if (src == "valid") return table_config::VALID;
    if (src == "version") return table_config::VERSION;
    if (src == "zero") return table_config::ZERO;
    error("Unknown source %s", src.c_str());
    return table_config::NONE;
}

const char *table_config::field_source_type[] = {
        "NONE", "ADT_PTR", "CONSTANT", "IMMEDIATE", "INSTRUCTION", "METER_PTR",
        "NEXT_TABLE", "PARITY", "PAYLOAD", "PROXY_HASH", "RANGE", "SEL_LEN", "SEL_PTR",
        "SEL_SHIFT", "SPEC", "STATS_PTR", "STFUL_PTR", "VALID", "VERSION", "ZERO" };

const char *table_config::table_type[] = {
        "UNKNOWN", "match", "hash_match", "atcam_match", "ternary_match", "match_no_key",
        "phase0_match", "action_data", "selector", "statistics", "meter", "stateful",
        "condition", "hash_action" };

std::ostream &operator<<(std::ostream &out, table_config::field_source_t fs) {
    if (size_t(fs) < sizeof(table_config::field_source_type)/sizeof(const char *))
        return out << table_config::field_source_type[fs];
    else
        return out << "<fs_t " << int(fs) << ">";
}

std::ostream &operator<<(std::ostream &out, const table_config::field &f) {
    out << f.source << ' ' << f.name << "(" << f.start_bit << ".."
        << (f.start_bit + f.bit_width - 1) << ") at " << f.start_offset;
    if (f.enable_pfe) out << " {pfe}";
    if (f.source == table_config::CONSTANT) out << ", value=" << f.const_value;
    return out;
}

static void add_field_to_table(table_config::table_t &tbl, int entry, table_config::field f) {
    LOG3("  entry " << entry << " " << f);
    if (size_t(entry) >= tbl.fields.size())
        tbl.fields.resize(entry + 1);
    auto &field_map = tbl.fields[entry];
    auto source = f.source;
    auto pos = field_map.by_name.emplace(f.name, std::move(f));
    field_map.by_type.emplace(source, &pos->second);
}

std::vector<table_config::field *>
        table_config::table_t::find_all_fields(field_source_t type) const {
    LOG1("Find all fields " << type);
    std::vector<table_config::field *> rv;
    for (auto &en : fields) {
        for (auto &field : en.iter(type)) {
            rv.push_back(field);
            LOG1("Field found");
        }
        break;
    }
    return rv;
}

table_config::field *table_config::table_t::find_field(field_source_t type, int start_bit) const {
    for (auto &en : fields)
        for (auto &field : en.iter(type))
            if (field->start_bit <= start_bit && field->start_bit + field->bit_width > start_bit)
                return field;
    return nullptr;
}
const table_config::field *table_config::table_t::find_field(std::string name) const {
    for (auto &en : fields)
        for (auto &field : en.iter(name))
            return &field;
    return nullptr;
}

bool table_config::table_t::all_zero_fields() const {
    for (auto &en : fields)
        for (auto &f: Values(en.by_name))
            if (f.source != ZERO) return false;
    return true;
}

std::vector<table_config::field *>
     table_config::stage::find_all_fields(field_source_t type) const {
    return table.find_all_fields(type);
}

table_config::field *table_config::stage::find_field(field_source_t type, int start_bit) const {
    auto rv = table.find_field(type, start_bit);
    if (!rv) rv = indirect.find_field(type, start_bit);
    return rv;
}

const table_config::field *table_config::stage::find_field(std::string name) const {
    auto rv = table.find_field(name);
    if (!rv) rv = indirect.find_field(name);
    return rv;
}

static void get_pack_format_entries(table_config::table_t &tbl, const json::vector &entries) {
    int bits_per_word = tbl.width ? tbl.bit_width / tbl.width : 0;
    if (tbl.bit_width != tbl.width * bits_per_word)
        error("pack format bit_width not a mulitple of width");
    for (json::obj_ref entry : entries) {
        int number = entry["entry_number"].to<json::number>().val;
        if (tbl.fields.size() <= size_t(number))
            tbl.fields.resize(number+1);
        if (entry["field_list"]) {  // old_json
            for (json::obj_ref field : entry["field_list"].to<json::vector>()) {
                table_config::field f = {};
                f.name = field["name"].to<json::string>();
                f.source = field_src(field["source"]);
                f.start_offset = field["start_offset"];
                f.start_bit = field["start_bit"];
                f.bit_width = field["bit_width"];
                f.const_value = 0;
                add_field_to_table(tbl, number, std::move(f)); }
            continue; }
        for (json::obj_ref field : entry["fields"].to<json::vector>()) {
            table_config::field f = {};
            f.name = field["field_name"].to<json::string>();
            f.source = field_src(field["source"]);
            f.bit_width = field["field_width"];
            int lsb_offset = field["lsb_mem_word_offset"] +
                             field["lsb_mem_word_idx"] * bits_per_word;
            f.start_offset = tbl.bit_width - lsb_offset - f.bit_width;
            f.start_bit = field["start_bit"];
            // FIXME: Hopefully after context JSON changes, will not be necessary
            if (field["enable_pfe"].is<json::True>())
                f.enable_pfe = true;
            f.const_value = 0;
            if (field["const_tuples"])
                for (json::obj_ref k : field["const_tuples"].to<json::vector>())
                    f.const_value = k["value"] << k["dest_start"];
            add_field_to_table(tbl, number, std::move(f)); } }
}

static void get_pack_format(table_config::table_t &tbl, json::obj_ref pack_fmt) {
    if (pack_fmt.is<json::vector>()) {
        if (pack_fmt.to<json::vector>().size() != 1)
            error("more than one pack_format in table not handled");
        pack_fmt = pack_fmt[0]; }
    if (!pack_fmt) return;
    tbl.width = pack_fmt["number_memory_units_per_table_word"].to<json::number>().val;
    tbl.bit_width = pack_fmt["table_word_width"].to<json::number>().val;
    tbl.mem_width = pack_fmt["memory_word_width"].to<json::number>().val;
    if (pack_fmt["entries"]) {
        get_pack_format_entries(tbl, pack_fmt["entries"].to<json::vector>());
    } else if (pack_fmt["entry_list"]) {  // old json
        get_pack_format_entries(tbl, pack_fmt["entry_list"].to<json::vector>());
    } else if (pack_fmt["entries_per_table_word"]) {  // old json
        tbl.fields.resize(pack_fmt["entries_per_table_word"].to<json::number>().val); }
}

static void get_hash_input(table_config::field_map_t &hash_input, json::obj_ref match_group) {
    // old json
    if (match_group.is<json::vector>()) {
        if (match_group.to<json::vector>().size() != 1)
            error("more than one match_group_resource_allocation in table not handled");
        match_group = match_group[0]; }
    if (!match_group) return;
    if (match_group["field_list"]) {
        for (json::obj_ref field : match_group["field_list"].to<json::vector>()) {
            table_config::field f = {};
            f.name = field["name"].to<json::string>();
            f.source = field_src(field["source"]);
            f.start_offset = field["start_offset"].to<json::number>().val;
            f.start_bit = field["start_bit"].to<json::number>().val;
            f.bit_width = field["bit_width"].to<json::number>().val;
            LOG3("  hash input " << f.name << "(" << f.start_bit << ".." <<
                 (f.start_bit + f.bit_width - 1) << ") at " << f.start_offset);
            hash_input.emplace(f.name, std::move(f)); }
    } else if (match_group["match_groups"]) {
        std::vector<int>        groups;
        for (json::obj_ref el : match_group["match_groups"].to<json::vector>())
            groups.push_back(el[0].to<json::number>().val);
        table_config::field *prev = nullptr;
        for (auto &bit : match_group["match_group_phv_bit_scrambling"].to<json::map>()) {
            int bitnum = bit.second->to<json::number>().val;
            bitnum = groups[bitnum/128U]*128 + (bitnum%128U);
            auto fieldname = bit.first->to<std::string>();
            auto nameend = fieldname.find('[');
            if (nameend == std::string::npos) {
                error("Can't parse match_group_phv_bit_scrambling");
                return; }
            int fieldbit = atoi(&fieldname[nameend+1]);
            fieldname.resize(nameend);
            if (prev && prev->name == fieldname && prev->start_bit + prev->bit_width == fieldbit &&
                1024 - prev->start_offset == bitnum) {
                prev->bit_width++;
                prev->start_offset--;
                continue; }
            if (prev)
                LOG3("  hash input " << prev->name << "(" << prev->start_bit << ".." <<
                     (prev->start_bit + prev->bit_width - 1) << ") at " << prev->start_offset);
            table_config::field f = {};
            f.name = fieldname;
            f.source = table_config::SPEC;
            f.start_offset = 1023 - bitnum;
            f.start_bit = fieldbit;
            f.bit_width = 1;
            prev = &hash_input.emplace(f.name, std::move(f))->second; }
        if (prev)
            LOG3("  hash input " << prev->name << "(" << prev->start_bit << ".." <<
                 (prev->start_bit + prev->bit_width - 1) << ") at " << prev->start_offset); }
}

static void get_match_key(table_config::field_map_t &hash_input, json::obj_ref fields,
                          unsigned width) {
    int next_bit = width;
    hash_input.clear();
    for (json::obj_ref field : fields.to<json::vector>()) {
        table_config::field f = {};
        f.name = field["name"].to<json::string>();
        f.source = field_src(field["source"]);
        f.bit_width = field["bit_width"];
        // choose a free location entirely within a 64-bit word if possible
        if (((next_bit - f.bit_width) ^ (next_bit - 1)) ^ ~63)
            next_bit &= ~63U;
        if (next_bit < f.bit_width) {
            error("match key exceeds %d width", width);
            return; }
        next_bit -= f.bit_width;
        f.start_offset = next_bit;
        f.start_bit = field["start_bit"];
        // split into 64 bit or smaller chunks
        while (f.bit_width > 64) {
            table_config::field split = f;
            split.bit_width = 64;
            f.bit_width -= 64;
            f.start_bit += 64;
            split.start_offset += f.bit_width;
            LOG3("  hash input " << split.name << "(" << split.start_bit << ".." <<
                 (split.start_bit + split.bit_width - 1) << ") at " << split.start_offset);
            hash_input.emplace(split.name, std::move(split)); }
        LOG3("  hash input " << f.name << "(" << f.start_bit << ".." <<
             (f.start_bit + f.bit_width - 1) << ") at " << f.start_offset);
        hash_input.emplace(f.name, std::move(f)); }
}

static void get_hash_functions(std::vector<table_config::hash_func_t> &hash_functions,
                               table_config::field_map_t &hash_input, json::obj_ref funcs) {
    hash_functions.clear();
    for (json::obj_ref fn : funcs.to<json::vector>()) {
        hash_functions.push_back(table_config::hash_func_t());
        auto &func = hash_functions.back();
        for (json::obj_ref bits : fn["hash_bits"].to<json::vector>()) {
            int bit = bits["hash_bit"];
            for (json::obj_ref b : bits["bits_to_xor"].to<json::vector>()) {
                unsigned field_bit = b["field_bit"];
                int found = 0;
                for (auto &f : ValuesForKey(hash_input, b["field_name"].to<std::string>())) {
                    found = 1;
                    unsigned bit_idx = field_bit - f.start_bit;
                    if (bit_idx < unsigned(f.bit_width)) {
                        bit_idx += 1024 - f.start_offset - f.bit_width;
                        if (func[bit].col.size() <= bit_idx/64U)
                            func[bit].col.resize(1 + bit_idx/64U);
                        func[bit].col[bit_idx/64U] |= 1ULL << bit_idx%64U;
                        found = 2;
                        break; } }
                if (found == 0)
                    error("No field %s in match_key_fields", b["field_name"].c_str());
                else if (found == 1)
                    error("No bit %d in field %s in match_key_fields", field_bit,
                          b["field_name"].c_str()); }
            if (bits["seed"] && bits["seed"] != 0) {
                func[bit].seed = true; } } }
}

static void get_memory_resource(std::vector<table_config::mem> &mem, json::obj_ref mra) {
    if (mra) {
        for (json::obj_ref units : mra["memory_units_and_vpns"].to<json::vector>()) {
            table_config::mem m;
            for (json::obj_ref u : units["memory_units"].to<json::vector>())
                m.units.insert(m.units.begin(), u);
            for (json::obj_ref v : units["vpns"].to<json::vector>())
                m.vpn.push_back(v);
            mem.push_back(m); } }
}

static void get_memory_resource(table_config::table_t &tbl, json::obj_ref mra) {
    if (mra) {
        assert(tbl.vpn2mem.empty());
        get_memory_resource(tbl.mem, mra);
        if (mra["spare_bank_memory_unit"] && mra["spare_bank_memory_unit"].is<json::number>())
            tbl.spare_bank = mra["spare_bank_memory_unit"].to<json::number>().val;
        if (mra["memory_type"])
            tbl.mtype = table_config::memory_type(mra["memory_type"]);
        std::sort(tbl.mem.begin(), tbl.mem.end(),
                  [](const table_config::mem &a, const table_config::mem &b) -> bool {
                        return a.vpn < b.vpn; }); }
}

void setup_vpn2mem_map(table_config::table_t &tbl) {
    assert(tbl.vpn2mem.empty());
    for (auto &m : tbl.mem)
        for (auto vpn : m.vpn)
            tbl.vpn2mem[vpn] = &m;
}

static int getnum(json::obj_ref obj, const char *field, int def = -1) {
    if (obj[field]) return obj[field].to<json::number>().val;
    return def;
}

static void get_hash_way(std::vector<table_config::hash_way> &ways, json::obj_ref mra) {
    table_config::hash_way way;
    way.group = getnum(mra, "hash_function_id");
    way.entry_lo = getnum(mra, "hash_entry_bit_lo");
    way.entry_bits = getnum(mra, "number_entry_bits");
    way.subword_bits = getnum(mra, "number_subword_bits", 0);
    way.select_lo = getnum(mra, "hash_select_bit_lo");
    way.select_bits = getnum(mra, "number_select_bits");
    way.select_mask = getnum(mra, "hash_select_bit_mask", (1 << way.select_bits) - 1);
    way.mtype = table_config::unknown;
    if (mra["memory_type"]) way.mtype = table_config::memory_type(mra["memory_type"]);
    get_memory_resource(way.mem, mra);
    ways.push_back(std::move(way));
}

table_config::field_source_t table_config::access_type(json::obj_ref obj) {
    if (!obj) return table_config::NONE;
    unsigned pipes = this->pipes;
    if (auto *tbl = table_config::get(obj.c_str(), pipes)) {
        switch (tbl->type) {
        case table_config::STATISTICS: return table_config::STATS_PTR;
        case table_config::METER: return table_config::METER_PTR;
        case table_config::STATEFUL: return table_config::STFUL_PTR;
        case table_config::SELECTOR: return table_config::SEL_PTR;
        default: break; }
        error("Can't index %s table %s", table_config::table_type[tbl->type], tbl->name.c_str());
    } else {
        error("No table named %s", obj.c_str());
    }
    return table_config::NONE;
}

table_config::action::action(json::obj_ref &action, int default_code) {
    name = action["name"].to<std::string>();
    code = default_code;
    if (action["address_to_use"]) {  // old json
        code = action["address_to_use"].to<json::number>().val;
        LOG3("  action " << name << " code " << code); }
    if (action["full_address"])  // old json
        full_address = action["full_address"].to<json::number>().val;
    if (action["override_meter_addr"].is<json::True>())
        field_override.push_back(field_override_t {
            METER_PTR, action["override_meter_full_addr"].to<json::number>().val,
            action["override_meter_addr_pfe"].is<json::True>() });
    if (action["override_stat_addr"].is<json::True>())
        field_override.push_back(field_override_t {
            STATS_PTR, action["override_stat_full_addr"].to<json::number>().val,
            action["override_stat_addr_pfe"].is<json::True>() });
    if (action["override_stateful_addr"].is<json::True>()) {
        auto addr = action["override_stateful_full_addr"].to<json::number>().val;
        // Weird encoding -- the value is 27 bits, but only 26 are needed.  The full
        // 27 are provided here, but uses of the field assume the 26 bits, so need to
        // "squeeze out" bit 24 from the middle
        // addr = ((addr & ~0x1ffffff) >> 1) | (addr & 0xffffff);
        field_override.push_back(field_override_t {
            STFUL_PTR, addr, action["override_stateful_addr_pfe"].is<json::True>() }); }
    if (action["handle"].is<json::number>())
        handle = action["handle"];
}

static void setup_action_p4_parameters(table_config::action &action, json::obj_ref p4_params) {
    for (json::obj_ref param : p4_params.to<json::vector>()) {
        action.p4_parameters.insert(param["name"]);
        std::string pname = param["name"];
        LOG3("Adding param " << pname << " to action " << action.name);
    }
}

void table_config::setup_action_indirect_resources(json::obj_ref &act, table_config::action &action,
                                                   table_config &tbl, table_config::stage &st) {
    if (!act["indirect_resources"]) return;
    for (json::obj_ref rsrc : act["indirect_resources"].to<json::vector>()) {
        if (rsrc["access_mode"] != "index") continue;
        table_config::immed_field_remap indir = {};
        auto indir_name = rsrc["parameter_name"].to<json::string>();
        auto indir_res_name = rsrc["resource_name"].to<json::string>();
        auto indir_type = access_type(rsrc["resource_name"]);
        int bit_width = 0;
        if (indir_type == table_config::STATS_PTR)
            bit_width = 20;
        else if (indir_type == table_config::STFUL_PTR || indir_type == table_config::METER_PTR)
            bit_width = 27;
        auto vec = st.find_all_fields(indir_type);
        for (auto *f : vec) {
            indir.name = indir_name;
            indir.res_name = indir_res_name;
            indir.type = indir_type;
            indir.immed_name = f->name;
            indir.bit_width = bit_width;
            LOG3("  indirect " << indir.name << " -> " << indir.immed_name << "(0.." <<
                 (indir.bit_width - 1) << ")");
            action.immed_remap.emplace(indir.name, std::move(indir));
        }
        if (vec.empty()) {
            LOG3("  indirect " << indir.name << " -> " << indir.type);
            action.immed_remap.emplace(indir.name, std::move(indir));
        }
    }
}

void table_config::stage::setup_action_data(json::obj_ref stage_table) {
    for (int i = 0; size_t(i) < actions.size(); i++) {
        LOG3(" action data for " << actions[i].name);
        for (json::obj_ref pck_fmt : stage_table["pack_format"].to<json::vector>()) {
            if (pck_fmt["action_handle"] == actions[i].handle) {
                LOG3(" getting pack format for action with handle " << actions[i].handle);
                get_pack_format(actions[i].action_data, pck_fmt); } }
        get_memory_resource(actions[i].action_data, stage_table["memory_resource_allocation"]);
        setup_vpn2mem_map(actions[i].action_data); }
}

int table_config::stage::setup_action_data(const table_config &self, json::obj_ref adt) {
    if (!adt) return 0;
    if (adt->is<json::vector>()) {
        if (adt.to<json::vector>().empty())
            return 0;
        if (adt.to<json::vector>().size() != 1) {
            error("more than one action data table not handled");
            return -1; }
        adt = adt[0]; }
    auto name = adt["name"].to<std::string>();
    adt_indirect = adt["how_referenced"] == "indirect";
    unsigned pipes = self.pipes;
    json::obj_ref table = P4TableConfig(name.c_str(), pipes);
    if (!table) {
        error("No table named %s, referred to by %s", name.c_str(), self.name.c_str());
        return -1; }
    for (json::obj_ref stage_table : table["stage_tables"].to<json::vector>()) {
        if (stage_table["stage_number"] != stage) continue;
        setup_action_data(stage_table);
        return 1; }
    return 0;
}

static std::string find_immediate_field(const table_config::stage &tbl) {
    auto *f = tbl.find_field(table_config::IMMEDIATE, 0);
    return f ? f->name : std::string();
}

// comparator function to sort ways in priority order (lowest to highest).  There isn't
// a string priority order, just prority between columns in the same row on the same side,
// so we just sort based on column of the first ram in the way.  If ways in different rows
// or on both side match, then the result of the hardware will be garbage (bits from both
// matches getting wired-or together).  The compiler must ensure that doesn't happen for
// table that need this kind of priority (atcam/alpm)
static bool way_prio_order(const table_config::hash_way &a, const table_config::hash_way &b) {
    int acol = a.mem.empty() || a.mem[0].units.empty() ? 0 : a.mem[0].units[0] % 12;
    int bcol = b.mem.empty() || b.mem[0].units.empty() ? 0 : b.mem[0].units[0] % 12;
    // reverse the right side columns, as the outermost columns have highest priority
    if (acol >= 6) acol = 17 - acol;
    if (bcol >= 6) bcol = 17 - bcol;
    return acol > bcol;
}

int table_config::stage::setup(json::obj_ref table,
                               const std::map<int, std::string> &action_handles,
                               const std::map<std::string, int> &action_indexes,
                               json::obj_ref parent_table) {
    json::obj_ref action_format = table["action_format"];
    if (table["stage_table_type"] == "ternary_match") {
        type = TERNARY_MATCH;
        get_pack_format(this->table, table["pack_format"]);
        get_memory_resource(this->table, table["memory_resource_allocation"]);
        auto indir = table["ternary_indirection_stage_table"];
        if (indir || (indir = table["ternary_indirection_table"])) {
            get_pack_format(indirect, indir["pack_format"]);
            get_memory_resource(indirect, indir["memory_resource_allocation"]);
            setup_vpn2mem_map(indirect);
            action_format = indir["action_format"]; }
        entries = this->table.mem.size() * 512;
    } else if (table["stage_table_type"] == "hash_match") {
        type = HASH_MATCH;
        get_pack_format(this->table, table["pack_format"]);
        if (table["match_key_fields"]) {
            get_match_key(hash_input, table["match_key_fields"], 1024);
        } else if (parent_table["match_key_fields"]) {
            get_match_key(hash_input, parent_table["match_key_fields"], 1024);
        } else if (table["match_group_resource_allocation"]) {  // old json
            get_hash_input(hash_input, table["match_group_resource_allocation"]); }
        if (table["hash_functions"])
            get_hash_functions(hash_functions, hash_input, table["hash_functions"]);
        int ways = 0;
        json::vector *wayvec = nullptr;
        if (table["way_stage_tables"])
            wayvec = &table["way_stage_tables"].to<json::vector>();  // old json
        else if (table["ways"])
            wayvec = &table["ways"].to<json::vector>();
        if (wayvec) {
            json::obj_ref way0_pack_format;
            for (json::obj_ref way : *wayvec) {
                if (way["pack_format"] != table["pack_format"]) {
                    if (ways == 0) {
                        way0_pack_format = way["pack_format"];
                        get_pack_format(this->table, way["pack_format"]);
                    } else if (way["pack_format"] != way0_pack_format) {
                        error("way pack_format inconsistent across table (unsupported)");
                        return -1; } }
                get_memory_resource(this->table, way["memory_resource_allocation"]);
                get_hash_way(this->ways, way["memory_resource_allocation"]);
                ++ways; }
            if (this->ways.size() > 1)
                std::stable_sort(this->ways.begin(), this->ways.end(), way_prio_order); }
        entries = this->table.fields.size() * ways;
    } else if (table["stage_table_type"] == "hash_action") {
        type = HASH_ACTION;
        get_memory_resource(this->table, table["memory_resource_allocation"]);
        if (table["match_key_fields"])
            get_match_key(hash_input, table["match_key_fields"], 1024);
        else if (parent_table["match_key_fields"])
            get_match_key(hash_input, parent_table["match_key_fields"], 1024);
        if (table["hash_functions"])
            get_hash_functions(hash_functions, hash_input, table["hash_functions"]);
        if (table["size"])
            entries = table["size"];
    } else if (table["stage_table_type"] == "match_with_no_key") {
        type = MATCH_NO_KEY;
    } else if (table["stage_table_type"] == "phase_0_match") {
        type = PHASE0_MATCH;
        if (table["match_key_fields"])
            get_match_key(hash_input, table["match_key_fields"], 128);
        else if (parent_table["match_key_fields"])
            get_match_key(hash_input, parent_table["match_key_fields"], 128);
        if (table["pack_format"])
            get_pack_format(this->table, table["pack_format"]);
    } else if (table["stage_table_type"] == "statistics") {
        type = STATISTICS;;
        get_pack_format(this->table, table["pack_format"]);
        get_memory_resource(this->table, table["memory_resource_allocation"]);
        // FIXME -- the assembler does not generate memory layout information for stats
        // FIXME -- tables, and the glass compiler generates it incorrectly, so we recreate
        // FIXME -- it from scratch.
        int byte_width = 0;
        if (table["byte_width"])
            byte_width = table["byte_width"];
        else if (parent_table["byte_counter_resolution"])
            byte_width = parent_table["byte_counter_resolution"];
        int pkt_width = 0;
        if (table["pkt_width"])
            pkt_width = table["pkt_width"];
        else if (parent_table["packet_counter_resolution"])
            pkt_width = parent_table["packet_counter_resolution"];
        if (pkt_width + byte_width > 0) {
            int bit = 128 % (pkt_width+byte_width);
            if (bit && this->table.fields.size() == 6)
                bit--;
            for (int i = this->table.fields.size() - 1; i >= 0; --i) {
                this->table.fields[i].clear();
                if (i == 3 && this->table.fields.size() == 6)
                    bit++;
                if (byte_width) {
                    add_field_to_table(this->table, i, table_config::field
                        { "bytes", table_config::PAYLOAD, bit, 0, byte_width, false, 0 });
                    bit += byte_width; }
                if (pkt_width) {
                    add_field_to_table(this->table, i, table_config::field
                        { "packets", table_config::PAYLOAD, bit, 0, pkt_width, false, 0 });
                    bit += pkt_width; } }
            entries = this->table.mem.size() * this->table.fields.size();
        } else {
            error("can't figure out counter widths"); }
    } else if (table["stage_table_type"] == "action_data") {
        type = ACTION_DATA;
        entries = table["size"];
        setup_action_data(table);
    } else if (table["stage_table_type"] == "meter") {
        type = METER;
        get_pack_format(this->table, table["pack_format"]);
        get_memory_resource(this->table, table["memory_resource_allocation"]);
        if (parent_table["enable_color_aware_pfe"].is<json::True>())
            this->table.meter_color_aware = true;
    } else if (table["stage_table_type"] == "stateful") {
        type = STATEFUL;
        get_pack_format(this->table, table["pack_format"]);
        get_memory_resource(this->table, table["memory_resource_allocation"]);
        // assembler fails to generate memory layout info for registers, so infer it.
        if (parent_table["alu_width"]) {
            int alu_width = parent_table["alu_width"];
            int entry_width = 128 / this->table.fields.size();
            if (alu_width != entry_width && alu_width*2 != entry_width)
                error("invalid register layout for %s", parent_table["name"]->toString().c_str());
            int idx = -1;
            int start_bit = 128;
            for (auto &ent : this->table.fields) {
                ++idx;
                if (!ent.by_name.empty()) {
                    start_bit -= entry_width;
                    continue; }
                start_bit -= alu_width;
                add_field_to_table(this->table, idx, table_config::field
                    { "lo", table_config::PAYLOAD, start_bit, 0, alu_width, false, 0 });
                if (alu_width < entry_width) {
                    start_bit -= alu_width;
                    add_field_to_table(this->table, idx, table_config::field
                        { "hi", table_config::PAYLOAD, start_bit, 0, alu_width, false, 0 }); } } }
        if (parent_table["action_to_stateful_instruction_slot"]) {
            json::vector *slot_vec =
                &parent_table["action_to_stateful_instruction_slot"].to<json::vector>();
            for (json::obj_ref entry : *slot_vec) {
                uint key = entry["action_handle"];
                int slot = entry["instruction_slot"];
                this->table.stateful_alu_per_action[key] = slot;
            }
        }
    } else if (table["stage_table_type"] == "gateway") {
        type = CONDITION;
        return 1;  // ignore
    } else if (table["stage_table_type"] == "selection") {
        type = SELECTOR;
        entries = table["size"];
        get_pack_format(this->table, table["pack_format"]);
        get_memory_resource(this->table, table["memory_resource_allocation"]);
    } else {
        error("unhandled stage table type %s", table["stage_table_type"]->toString().c_str());
        return -1; }
    setup_vpn2mem_map(this->table);
    if (action_format)
        for (json::obj_ref el : action_format.to<json::vector>()) {
            auto actname = el["action_name"].to<std::string>();
            auto &action = actions[action_indexes.at(actname)];
            for (json::obj_ref immed : el["immediate_fields"].to<json::vector>()) {
                immed_field_remap f;
                f.name = immed["param_name"].to<std::string>();
                f.immed_name = find_immediate_field(*this);
                f.immed_bit = immed["dest_start"];
                f.bit_width = immed["dest_width"];
                f.start_bit = immed["param_shift"];
                if (immed["param_type"] == "constant") {
                    f.constant = true;
                    f.constant_value = immed["const_value"];
                } else {
                    f.constant = false;
                    f.constant_value = 0; }
                LOG3("  remap " << actname << ":" << f.name << "(" << f.start_bit << ".." <<
                     (f.start_bit + f.bit_width - 1) << ") -> " << f.immed_name << "(" <<
                     f.immed_bit << ".." << (f.immed_bit + f.bit_width - 1) << ")" <<
                     (f.constant ? " = " + std::to_string(f.constant_value) : ""));
                action.immed_remap.emplace(f.name, std::move(f)); }
            action.next_table = el["next_table"];
            action.next_table_full = el["next_table_full"];
            if (el["next_table_exec"]) {
                uint32_t val = el["next_table_exec"];
                action.next_table_global_exec = val & 0xffff;
                action.next_table_local_exec = val >> 16; }
            if (el["next_table_global_exec"])
                action.next_table_global_exec = el["next_table_global_exec"];
            if (el["next_table_local_exec"])
                action.next_table_local_exec = el["next_table_local_exec"];
            if (el["next_table_long_brch"])
                action.next_table_long_brch = el["next_table_long_brch"];
            action.code = el["vliw_instruction"];
            action.full_address = el["vliw_instruction_full"];
            LOG3("  action " << action.name << " code " << action.code <<
                 " address " << action.full_address); }
    if (table["action_to_immediate_mapping"])  // old json
        for (auto &el : table["action_to_immediate_mapping"].to<json::map>()) {
            std::string actname = el.first->to<std::string>();
            if (int handle = atoi(actname.c_str()))
                actname = action_handles.at(handle);
            auto &action = actions[action_indexes.at(actname)];
            for (json::obj_ref param : el.second->to<json::vector>())
                for (json::obj_ref p : param.to<json::vector>()) {
                    immed_field_remap f;
                    f.name = p["name"].to<std::string>();
                    f.immed_name = p["field_called"].to<std::string>(),
                    f.immed_bit = p["immediate_least_significant_bit"].to<json::number>().val;
                    f.start_bit = p["parameter_least_significant_bit"].to<json::number>().val;
                    f.bit_width = p["parameter_most_significant_bit"].to<json::number>().val -
                                  f.start_bit + 1;
                    f.constant = false;
                    f.constant_value = 0;
                    LOG3("  remap " << actname << ":" << f.name << "(" << f.start_bit << ".." <<
                         (f.start_bit + f.bit_width - 1) << ") -> " << f.immed_name << "(" <<
                         f.immed_bit << ".." << (f.immed_bit + f.bit_width - 1) << ")");
                    action.immed_remap.emplace(f.name, std::move(f)); } }
    if (table["action_to_next_table_mapping"])  // old json
        for (auto &el : table["action_to_next_table_mapping"].to<json::map>()) {
            std::string actname = el.first->to<std::string>();
            if (int handle = atoi(actname.c_str()))
                actname = action_handles.at(handle);
            if (!action_indexes.count(actname))
                // no such action -- ignore it?
                continue;
            auto &attr = el.second->to<json::map>();
            if (attr["next_table_full_address"])
                actions[action_indexes.at(actname)].next_table_full =
                    attr["next_table_full_address"]->to<json::number>().val; }
    if (table["vliw_resource_allocation"]) {  // old json
        for (auto &el : table["vliw_resource_allocation"].to<json::map>()) {
            std::string actname = el.first->to<std::string>();
            if (int handle = atoi(actname.c_str()))
                actname = action_handles.at(handle);
            if (!action_indexes.count(actname))
                // no such action -- ignore it?
                continue;
            auto &attr = el.second->to<json::map>();
            if (attr["address_to_use"])
                actions[action_indexes.at(actname)].code =
                    attr["address_to_use"]->to<json::number>().val;
            if (attr["full_address"])
                actions[action_indexes.at(actname)].full_address =
                    attr["full_address"]->to<json::number>().val; } }
    return 1;
}

static void find_names(std::map<std::string, std::string> &name_map, const std::string &name) {
    if (!name_map.count(name)) {
        const char *p = name.c_str();
        if (strncmp(p, "--validity_check--", 18) == 0) {
            if (name_map.count(p+18))
                name_map[p+18] = p+18;
            else
                name_map[p+18] = name; }
        while ((p = strchr(p, '.'))) {
            ++p;
            if (name_map.count(p) && name_map.at(p) != name)
                name_map[p] = p;
            else
                name_map[p] = name;
            if (!strcmp(p, "$valid")) {
                std::string hdr(name.c_str(), p - name.c_str() - 1);
                if (name_map.count(hdr))
                    name_map[hdr] = hdr;
                else
                    name_map[hdr] = name; } } }
    name_map[name] = name;
}
static void find_names(std::map<std::string, std::string> &name_map,
                       table_config::field_map_t &fields) {
    for (auto &n : fields)
        find_names(name_map, n.first);
}
static void find_names(std::map<std::string, std::string> &name_map,
                       std::vector<table_config::table_entry> &fields) {
    for (auto &el : fields)
        find_names(name_map, el.by_name);
}
static void find_names(std::map<std::string, std::string> &name_map,
                       table_config::remap_t &fields) {
    for (auto &n : fields)
        find_names(name_map, n.first);
}

static void add_aliases(std::map<std::string, std::string> &name_map,
                        table_config::field_map_t &fields) {
    for (auto &map : name_map) {
        assert(fields.count(map.first) == 0);
        for (auto f : ValuesForKey(fields, map.second)) {
            f.name = map.first;
            fields.emplace(f.name, std::move(f)); } }
}
static void add_aliases(std::map<std::string, std::string> &name_map,
                        std::vector<table_config::table_entry> &fields) {
    for (auto &el : fields)
        add_aliases(name_map, el.by_name);
}
static void add_aliases(std::map<std::string, std::string> &name_map,
                        table_config::remap_t &fields) {
    for (auto &map : name_map) {
        assert(fields.count(map.first) == 0);
        for (auto f : ValuesForKey(fields, map.second)) {
            f.name = map.first;
            fields.emplace(f.name, std::move(f)); } }
}

void table_config::stage::setup_field_aliases(const table_config &cfg) {
    std::map<std::string, std::string> name_map;
    find_names(name_map, hash_input);
    find_names(name_map, table.fields);
    find_names(name_map, indirect.fields);
    for (auto &act : actions)
        find_names(name_map, act.immed_remap);
    for (auto it = name_map.begin(); it != name_map.end(); ) {
        if (it->first == it->second) {
            it = name_map.erase(it);
        } else {
            LOG2(it->first << " is an abbreviation for " << it->second << " in " << cfg.name);
            ++it; } }
    if (!name_map.empty()) {
        add_aliases(name_map, hash_input);
        add_aliases(name_map, table.fields);
        add_aliases(name_map, indirect.fields);
        for (auto &act : actions)
            add_aliases(name_map, act.immed_remap); }
}

void table_config::bind_to_tables(std::string name, json::obj_ref bindings) {
    if (bindings) for (json::obj_ref b : bindings.to<json::vector>()) {
        unsigned pipes = this->pipes;
        if (auto tbl = const_cast<table_config *>(table_config::get(b["name"].c_str(), pipes))) {
            tbl->match_tables.push_back(name);
            if (!tbl->binding.empty() && tbl->binding != b["how_referenced"].to<std::string>())
                error("inconsistent binding for %s: %s and %s", tbl->name.c_str(),
                      tbl->binding.c_str(), b["how_referenced"].c_str());
            tbl->binding = b["how_referenced"].to<std::string>();
        } else
            error("No table named %s", b["name"].c_str()); }
}

std::ostream &operator<<(std::ostream &out, const json::obj_ref &o) {
    if (o.is<json::number>())
        out << o.to<json::number>().val;
    else
        out << o.to<json::string>();
    return out;
}

table_config *table_config::setup(json::obj_ref table) {
    setup_start = true;
    json::vector *stables = nullptr;
    if (table["direction"] == "ingress")
        direction = INGRESS;
    else if (table["direction"] == "egress")
        direction = EGRESS;
    else if (table["direction"] == "ghost")
        direction = GHOST;
    else
        error("Unknown direction \"%s\" for table %s\n", table["direction"].c_str(), name.c_str());
    if (table["table_type"] == "match")
        type = MATCH;
    else if (table["table_type"] == "action_data" || table["table_type"] == "action")
        type = ACTION_DATA;
    else if (table["table_type"] == "statistics")
        type = STATISTICS;
    else if (table["table_type"] == "meter")
        type = METER;
    else if (table["table_type"] == "stateful") {
        type = STATEFUL;
        if (table["stateful_counter_index"])
            counter_index = table["stateful_counter_index"];
    } else if (table["table_type"] == "condition")
        type = CONDITION;
    else if (table["table_type"] == "selection") {
        type = SELECTOR;
        if (table["max_port_pool_size"])
            selector_pool_size = table["max_port_pool_size"];
    } else {
        type = UNKNOWN;
        error("Unknown table type %s for table %s", table["table_type"].c_str(), name.c_str()); }
    if (table["size"])
        entries = table["size"];
    if (table["match_attributes"] && table["match_attributes"]["stage_tables"])
        stables = &table["match_attributes"]["stage_tables"].to<json::vector>();
    else if (table["stage_tables"])
        stables = &table["stage_tables"].to<json::vector>();  // old json or non-match tables
    if (!stables) return nullptr;
    std::map<int, std::string> action_handles;
    for (json::obj_ref st : *stables) {
        stage_tables.push_back(stage());
        auto &stage_table = stage_tables.back();
        stage_table.stage = st["stage_number"];
        if (st["stage_table_handle"])
            stage_table.logical_id = st["stage_table_handle"];  // old json
        if (st["logical_table_id"])
            stage_table.logical_id = st["logical_table_id"];
        if (st["physical_table_id"])
            stage_table.physical_id = st["physical_table_id"];
        LOG3("table " << name << " stage " << stage_table.stage);
        std::map<std::string, int> action_indexes;
        if (table["actions"]) {
            for (json::obj_ref action : table["actions"].to<json::vector>()) {
                int idx = stage_table.actions.size();
                stage_table.actions.emplace_back(action, idx);
                auto name = stage_table.actions.back().name;
                action_indexes[name] = idx;
                if (action["handle"])
                    action_handles[action["handle"].to<json::number>().val] = name; }
            if (stage_table.setup_action_data(*this, table["action_data_table_refs"]) < 0)
                return nullptr;
            if (stage_table.setup_action_data(*this, table["p4_action_data_tables"]) < 0)
                return nullptr;  /* old json */ }
        if (stage_table.setup(st, action_handles, action_indexes, table) < 0)
            return nullptr;
        stage_table.setup_field_aliases(*this);
        if (table["actions"]) {
            auto act = stage_table.actions.begin();
            for (json::obj_ref action : table["actions"].to<json::vector>()) {
                if (action["p4_parameters"])
                    setup_action_p4_parameters(*act, action["p4_parameters"]);
                if (action["handle"])
                    act->handle = action["handle"];
                setup_action_indirect_resources(action, *act++, *this, stage_table); } } }
    if (table["binding"]) {
        bool first = true;
        for (json::obj_ref bind : table["binding"].to<json::vector>()) {
            if (first)
                binding = bind.to<json::string>();
            else
                match_tables.push_back(bind.to<json::string>());
            first = false; } }
    bind_to_tables(name, table["action_data_table_refs"]);
    // bind_to_tables(name, table["selection_table_refs"]);  -- FIXME not dealing with these yet?
    // selection table and action data table have the same name!
    bind_to_tables(name, table["statistics_table_refs"]);
    bind_to_tables(name, table["meter_table_refs"]);
    bind_to_tables(name, table["stateful_table_refs"]);
    int hndl;
    if (table["default_action_handle"] && (hndl = table["default_action_handle"])) {
        if (action_handles.count(hndl) && table["actions"]) {
            default_action = action_handles.at(hndl);
            for (json::obj_ref action : table["actions"].to<json::vector>()) {
                if (action["handle"] != hndl) continue;
                std::stringstream buf;
                buf << default_action;
                const char *sep = "(";
                for (json::obj_ref param : action["p4_parameters"].to<json::vector>()) {
                    if (param["default_value"]) {
                        buf << sep << param["name"].to<json::string>() << ':';
                        if (param["default_value"].is<json::number>())
                            buf << param["default_value"].to<json::number>().val;
                        else
                            buf << param["default_value"].to<json::string>();
                        sep = ", "; } }
                if (*sep == ',') buf << ")";
                default_action = buf.str();
                break; }
            LOG3("  default action is " << default_action);
        } else {
            warning("default_action_handle %d not found in table %s", hndl, name.c_str()); }
    } else if (table["default_action"]) {  // old json
        default_action = table["default_action"]["name"].to<json::string>();
        if (table["default_action_parameters"]) {
            std::stringstream buf;
            buf << default_action << "(";
            const char *sep = "";
            for(auto &param : table["default_action_parameters"].to<json::map>()) {
                buf << sep << param.first->to<json::string>() << ':'
                    << param.second->to<json::number>().val;
                sep = ", "; }
            buf << ")";
            default_action = buf.str(); }
        LOG3("  default action is " << default_action); }

    if (table["static_entries"]) {
        for (json::obj_ref entry : table["static_entries"].to<json::vector>()) {
            std::stringstream buf;

            auto priority = entry["priority"].to<json::number>().val;
            // auto is_default = entry["is_default_entry"];  // what TODO with this?

            // priority is reverse encoded (0 is highest priority), but hardware needs
            // normal priority (0 is lowest).  So we need to know how many entries there
            // are and subtract the priority from that
            int entries = 0;
            bool ternary = false;
            for (auto &st : stage_tables) {
                if (st.type == TERNARY_MATCH) ternary = true;
                entries += st.entries; }
            if (ternary)
                buf << (entries - 1 - priority) << " ";

            unsigned action_handle = entry["action_handle"];

            for (json::obj_ref match : entry["match_key_fields_values"].to<json::vector>()) {
                auto name = match["field_name"].to<json::string>();
                buf << " " << name << ":";
                if (auto value = match["value"]) {
                    if (auto mask = match["mask"]) {
                        uint64_t v = strtoull(value.to<json::string>().c_str(), 0, 0);
                        uint64_t dc = ~strtoull(mask.to<json::string>().c_str(), 0, 0);
                        int width = find_field(name)->bit_width;
                        if (width < 64)
                            dc &= (1ULL << width) - 1;
                        buf << ternary_value(v, dc);
                    } else {
                        buf << value; }
                } else if (auto rs = match["range_start"]) {
                    buf << rs << ".." << match["range_end"];
                } else {
                    error("bad match key value in static_entries for %s", this->name.c_str());
                    continue; } }

            buf << " " << action_handles.at(action_handle) << "(";

            const char *sep = "";
            for (json::obj_ref param : entry["action_parameters_values"].to<json::vector>()) {
                buf << sep << param["parameter_name"] << ':' << param["value"];
                sep = ", ";
            }
            buf << ")";

            static_entries.push_back(buf.str());
        }

        LOG3("static entries:");
        for (auto& ent : static_entries) LOG3(ent);
    }

    return this;
}

const table_config *table_config::get(const char *name, unsigned &pipes) {
    auto it = all_by_suffix.find(name);
    if (it != all_by_suffix.end()) {
        pipes &= it->second->pipes;
        LOG3("[cached] table_config " << name << " is " << it->second->name <<
             " in " << pipe_set_str(pipes));
        return it->second; }
    unsigned try_pipes = pipes;
    auto json_cfg = P4TableConfig(name, try_pipes);
    if (!json_cfg) {
        error("No matching table for suffix '%s'", name);
        return nullptr; }
    auto full_name = (*json_cfg)["name"].to<json::string>();
    table_config *rv = &all[full_name];
    rv->name = full_name;
    rv->pipes = try_pipes;
    pipes &= try_pipes;
    try {
        if (!rv->setup_start)
            rv = rv->setup(json_cfg);
    } catch (std::bad_cast& ex) {
        error("corrupt table config json");
        rv = nullptr; }
    if (!rv) {
        all.erase(full_name);
    } else {
        all_by_suffix[name] = rv; }
    LOG3("[loaded] table_config " << name << " is " << rv->name << " in " << pipe_set_str(pipes));
    return rv;
}
