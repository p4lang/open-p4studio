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

#include <dlfcn.h>
#include <iostream>
#include <cstdio>
#include <fstream>
#include <deque>
#include <string.h>
#include <unistd.h>

#include <utests/test_namespace.h>

#include <common/rmt.h>
#include <rmt-debug.h>
#include <rmt-object-manager.h>
#if !MCN_TEST(MODEL_CHIP_NAMESPACE,rsvd1)
#include <parser-shared.h>
#endif
#include <port.h>
#include <register_utils.h>
#include <rmt-packet-coordinator.h>

#include "hex.h"
#include "input.h"
#include "log.h"
#include "p4name.h"
#include "packet.h"
#include "script.h"
#include "util.h"
#include "write.h"


using namespace MODEL_CHIP_NAMESPACE;

static std::set<int>    initialized_ports;
static std::map<int, uint64_t> port_input_time;
static FILE *           observationLog = NULL;
static bool             use_rmt_port_space = false;

bool set_observationLog(char *log) {
    observationLog = fopen(log, "w");
    if (observationLog == NULL) {
        return false;
    }
    setlinebuf(observationLog);
    return true;
}

int port_rmt2mau(int rmt_port) {
    // left shift
    return Port::port_map(rmt_port, -RmtDefs::kMacChanShift);
}

int port_mau2rmt(int core_port) {
    // right shift
    return Port::port_map(core_port, RmtDefs::kMacChanShift);
}

/**
 * @param port Port number in MAU port space.
 */
static void init_port(int port) {
    if (initialized_ports.count(port)) return;
#if !MCN_TEST(MODEL_CHIP_NAMESPACE,rsvd1)
    // FIXME WIP is completely different here?
    int die = Port::get_die_num(port);
    int pipe = Port::get_pipe_num(port);
    int ipb = Port::get_ipb_num(port);
    int epb = Port::get_epb_num(port);
#if MCN_TEST(MODEL_CHIP_NAMESPACE,jbay) || MCN_TEST(MODEL_CHIP_NAMESPACE,jbayB0) || MCN_TEST(MODEL_CHIP_NAMESPACE,rsvd0)
    int ipb_chan = Port::get_ipb_chan(port);
    int epb_chan = Port::get_epb_chan(port);
#elif MCN_TEST(MODEL_CHIP_NAMESPACE,tofino) || MCN_TEST(MODEL_CHIP_NAMESPACE,tofinoB0)
    int ipb_chan = Port::get_parser_chan(port);
    int epb_chan = Port::get_parser_chan(port);
#else
#error "unknown chip type " MODEL_CHIP_NAMESPACE
#endif
    LOG3("Initializing port " << port << ":"
#if MCN_TEST(MODEL_CHIP_NAMESPACE,rsvd0)
         << " die=" << die
#endif
         << " pipe=" << pipe << " ipb=" << ipb << " epb=" << epb
         << " ipb_chan=" << ipb_chan << " epb_chan=" << epb_chan
#if MCN_TEST(MODEL_CHIP_NAMESPACE,jbay) || MCN_TEST(MODEL_CHIP_NAMESPACE,jbayB0) || MCN_TEST(MODEL_CHIP_NAMESPACE,rsvd0)
         << " mac=" << Port::get_mac_num(port) << " mac_port=" << Port::get_mac_chan(port)
#endif
         );
    if (port >= (RmtDefsShared::kPortsTotal * PKG_SIZE) || ipb >= RmtDefs::kParsers) {
        error("invalid port %d", port);
        return; }
    auto ing_buf_regs = RegisterUtils::addr_ingbuf(pipe, ipb);
    volatile void *addr = 0;
#if MCN_TEST(MODEL_CHIP_NAMESPACE,jbay) || MCN_TEST(MODEL_CHIP_NAMESPACE,jbayB0) || MCN_TEST(MODEL_CHIP_NAMESPACE,rsvd0)
    switch(ipb_chan) {
      case 0: addr = &ing_buf_regs->chan0_group.chnl_ctrl; break;
      case 1: addr = &ing_buf_regs->chan1_group.chnl_ctrl; break;
      case 2: addr = &ing_buf_regs->chan2_group.chnl_ctrl; break;
      case 3: addr = &ing_buf_regs->chan3_group.chnl_ctrl; break;
      case 4: addr = &ing_buf_regs->chan4_group.chnl_ctrl; break;
      case 5: addr = &ing_buf_regs->chan5_group.chnl_ctrl; break;
      case 6: addr = &ing_buf_regs->chan6_group.chnl_ctrl; break;
      case 7: addr = &ing_buf_regs->chan7_group.chnl_ctrl; break;
    }
    write_reg(die, addr, (read_reg(die, addr) & ~0x7fc0) | (port << 6) | 0x20000);
    // ingress_port + enable meta1 (for phase0)
#else
    switch(ipb_chan) {
      case 0: addr = &ing_buf_regs->chan0_group.chnl_metadata_fix2; break;
      case 1: addr = &ing_buf_regs->chan1_group.chnl_metadata_fix2; break;
      case 2: addr = &ing_buf_regs->chan2_group.chnl_metadata_fix2; break;
      case 3: addr = &ing_buf_regs->chan3_group.chnl_metadata_fix2; break;
    }
    write_reg(die, addr, (read_reg(die, addr) & ~0x1ff) | port);  // ingress_port
#endif

    switch(ipb_chan) {
      case 0: addr = &ing_buf_regs->chan0_group.chnl_ctrl; break;
      case 1: addr = &ing_buf_regs->chan1_group.chnl_ctrl; break;
      case 2: addr = &ing_buf_regs->chan2_group.chnl_ctrl; break;
      case 3: addr = &ing_buf_regs->chan3_group.chnl_ctrl; break;
#if MCN_TEST(MODEL_CHIP_NAMESPACE,jbay) || MCN_TEST(MODEL_CHIP_NAMESPACE,jbayB0) || MCN_TEST(MODEL_CHIP_NAMESPACE,rsvd0)
      case 4: addr = &ing_buf_regs->chan4_group.chnl_ctrl; break;
      case 5: addr = &ing_buf_regs->chan5_group.chnl_ctrl; break;
      case 6: addr = &ing_buf_regs->chan6_group.chnl_ctrl; break;
      case 7: addr = &ing_buf_regs->chan7_group.chnl_ctrl; break;
#endif
    }
    write_reg(die, addr, read_reg(die, addr) | 1);  // ingress channel enable
#if MCN_TEST(MODEL_CHIP_NAMESPACE,jbay) || MCN_TEST(MODEL_CHIP_NAMESPACE,jbayB0) || MCN_TEST(MODEL_CHIP_NAMESPACE,rsvd0)
    addr = &ing_buf_regs->glb_group.port_en;    // ingress port enable
    write_reg(die, addr, read_reg(die, addr) | 1 << ipb_chan);
#endif

    auto eg_buf_regs = RegisterUtils::addr_egrbuf(pipe, epb);
#if MCN_TEST(MODEL_CHIP_NAMESPACE,jbay) || MCN_TEST(MODEL_CHIP_NAMESPACE,jbayB0) || MCN_TEST(MODEL_CHIP_NAMESPACE,rsvd0)
    switch(epb_chan) {
      case 0: addr = &eg_buf_regs->chan0_group.chnl_ctrl; break;
      case 1: addr = &eg_buf_regs->chan1_group.chnl_ctrl; break;
      case 2: addr = &eg_buf_regs->chan2_group.chnl_ctrl; break;
      case 3: addr = &eg_buf_regs->chan3_group.chnl_ctrl; break;
      case 4: addr = &eg_buf_regs->chan4_group.chnl_ctrl; break;
      case 5: addr = &eg_buf_regs->chan5_group.chnl_ctrl; break;
      case 6: addr = &eg_buf_regs->chan6_group.chnl_ctrl; break;
      case 7: addr = &eg_buf_regs->chan7_group.chnl_ctrl; break;
    }
#else
    addr = &eg_buf_regs->chnl_ctrl[epb_chan];
#endif
    write_reg(die, addr, read_reg(die, addr) | 0x10000);  // egress channel enable

#if MCN_TEST(MODEL_CHIP_NAMESPACE,jbay) || MCN_TEST(MODEL_CHIP_NAMESPACE,jbayB0) || MCN_TEST(MODEL_CHIP_NAMESPACE,rsvd0)
    // for jbay, need to enable the mac (not for tofino?)
    auto dprsr_regs = RegisterUtils::addr_dprsr(pipe);
    switch(Port::get_mac_num(port)) {
      case 0: addr = &dprsr_regs->inp.icr.mac0_en; break;
      case 1: addr = &dprsr_regs->inp.icr.mac1_en; break;
      case 2: addr = &dprsr_regs->inp.icr.mac2_en; break;
      case 3: addr = &dprsr_regs->inp.icr.mac3_en; break;
      case 4: addr = &dprsr_regs->inp.icr.mac4_en; break;
      case 5: addr = &dprsr_regs->inp.icr.mac5_en; break;
      case 6: addr = &dprsr_regs->inp.icr.mac6_en; break;
      case 7: addr = &dprsr_regs->inp.icr.mac7_en; break;
      case 8: addr = &dprsr_regs->inp.icr.mac8_en; break;
    }
    uint32_t bit = 1 << Port::get_chan_num(port);   // mac enable
    // NOT using get_mac_chan here as that does WIP channel reduction scaling, and we've
    // already done that above
    write_reg(die, addr, read_reg(die, addr) | bit);
#endif
#endif // !WIP

    initialized_ports.insert(port);
}

void set_port_phase0(int port, uint64_t data0, uint64_t data1) {
    int ipb = Port::get_ipb_num(port);
    int die = Port::get_die_num(port);
    if (port >= (RmtDefsShared::kPortsTotal * PKG_SIZE) || ipb >= RmtDefs::kParsers) {
        error("invalid port %d", port);
        return; }
    int pipe = Port::get_pipe_num(port);
#if MCN_TEST(MODEL_CHIP_NAMESPACE,rsvd0)
    // FIXME -- metadata now goes in memory -- parser.ipb_mem.meta_phase0_16byte
    // or pipe.parde.pgr_ph0_rspec.phase0_mem_word?
    // RmtObjectManager *om = NULL;
    // GLOBAL_MODEL->GetObjectManager(die, &om);
    // auto *IPB = om->ipb_lookup(pipe, ipb);
    uint64_t addr = 0x20000000000 * pipe + 0x6080000000 + 0x36000 + Port::get_port_num(port);
    //              pipe                   parde        pgr_ph0_rspec
    GLOBAL_MODEL->IndirectWrite(die, addr, data0, data1);
#elif MCN_TEST(MODEL_CHIP_NAMESPACE,rsvd1)
    // FIXME -- where does WIP metadata go?
#else
    volatile void *addr = 0;
    auto ing_buf_regs = RegisterUtils::addr_ingbuf(pipe, ipb);
#if MCN_TEST(MODEL_CHIP_NAMESPACE,jbay) || MCN_TEST(MODEL_CHIP_NAMESPACE,jbayB0)
    switch(Port::get_ipb_chan(port)) {
      case 0: addr = &ing_buf_regs->chan0_group.chnl_meta.chnl_meta_0_4; break;
      case 1: addr = &ing_buf_regs->chan1_group.chnl_meta.chnl_meta_0_4; break;
      case 2: addr = &ing_buf_regs->chan2_group.chnl_meta.chnl_meta_0_4; break;
      case 3: addr = &ing_buf_regs->chan3_group.chnl_meta.chnl_meta_0_4; break;
      case 4: addr = &ing_buf_regs->chan4_group.chnl_meta.chnl_meta_0_4; break;
      case 5: addr = &ing_buf_regs->chan5_group.chnl_meta.chnl_meta_0_4; break;
      case 6: addr = &ing_buf_regs->chan6_group.chnl_meta.chnl_meta_0_4; break;
      case 7: addr = &ing_buf_regs->chan7_group.chnl_meta.chnl_meta_0_4; break;
    }
#else
    switch(Port::get_parser_chan(port)) {
      case 0: addr = &ing_buf_regs->chan0_group.chnl_metadata_fix; break;
      case 1: addr = &ing_buf_regs->chan1_group.chnl_metadata_fix; break;
      case 2: addr = &ing_buf_regs->chan2_group.chnl_metadata_fix; break;
      case 3: addr = &ing_buf_regs->chan3_group.chnl_metadata_fix; break;
    }
#endif
    write_reg(die, addr, data0);
    write_reg(die, (char *)addr + 4, data0 >> 32);
#if MCN_TEST(MODEL_CHIP_NAMESPACE,jbay) || MCN_TEST(MODEL_CHIP_NAMESPACE,jbayB0)
    write_reg(die, (char *)addr + 8, data1);
    write_reg(die, (char *)addr + 12, data1 >> 32);
#endif
#endif
}

/**
 * On Cloudbreak there are two port spaces -- RMT port space (corresponding to the interfaces)
 * which are dense, and MAU port space, where only even ports are valid.
 * MAU_port[6:0] == (RMT_port[6:0] * 2)[6:0]
 * We mostly do stuff in MAU port space (init_port above and reinit_all_ports, as well as the
 * various interface commands (packet, expect, wire), but whenever we call an rmt function we
 * need to convert to RMT port space before the call.
 */
void reinit_all_ports() {
    initialized_ports.clear();
    int stride = 1 << (RmtDefs::kPortParserWidth + RmtDefs::kPortParserChanWidth);
    int perpipe = RmtDefs::kParsers * RmtDefs::kParserChannels;
    for (int die = 0; die < PKG_SIZE; die++)
        for (int pipe = 0; pipe < RmtDefsShared::kPortsTotal; pipe += stride)
            for (int port = 0; port < perpipe; port += 1 << RmtDefs::kMacChanShift)
                init_port(die * RmtDefsShared::kPortsTotal + pipe + port);
}

ScriptCommand(use_rmt_port_space, "0|1|true|false") {
    if (!strcmp(args, "0") || !strcmp(args, "false"))
        use_rmt_port_space = false;
    else if (!strcmp(args, "1") || !strcmp(args, "true"))
        use_rmt_port_space = true;
    else
        error("usage: use_rmt_port_space 0|1|true|false");
    LOG3("use_rmt_port_space: " << use_rmt_port_space);
}

ScriptCommand(packet, "<port> <packet data>") {
    std::vector<uint8_t> pkt;
    const char  *rest;
    int         port, len;
    if (sscanf(args, "%d %n", &port, &len) < 1) {
        error("usage: packet <port> <data>");
        return;
    } else
        args += len;
    if (!(rest = str_to_bytes(args, pkt)) || *rest == '$')
        error("Malformed packet data");
#if MCN_TEST(MODEL_CHIP_NAMESPACE,rsvd0)
    if (!use_rmt_port_space && (port & 1))
        error("only even ports usable on rsvd0");
#endif
    int rmt_port = port;
    if (!use_rmt_port_space) rmt_port = port_mau2rmt(port);
    int mau_port = port_rmt2mau(rmt_port);
    LOG3("packet: port: " << port);
    LOG3("packet: rmt_port: " << rmt_port);
    LOG3("packet: mau_port: " << mau_port);
    if(!raw_mode) init_port(mau_port);
    update_packet_trace();
    uint64_t now;
    rmt_time_get(now);

#if 0
    now += 8000;  /* 8 nsec */
    if (port_input_time[mau_port] > now)
        now = port_input_time[mau_port];
    rmt_time_set(now);
#else
    if (port_input_time[mau_port] > now + 8000) {
        rmt_timer_increment(port_input_time[mau_port] - now);
        now = port_input_time[mau_port];
    } else {
        rmt_timer_increment(8000);
        now += 8000; }
#endif

    LOG2(pkt.size() << " byte packet in MAU port " << mau_port <<
         " (RMT port " << rmt_port << ") at time " << now);
    port_input_time[mau_port] = now + 80*len + 8000; /* 80psec/byte (100 Gb/sec) */
    // add 4 bytes for ethernet CRC
    pkt.resize(pkt.size() + 4);
    rmt_packet_receive(PORT2DIE(rmt_port), ASIC_PORT(rmt_port), &pkt[0], (int)pkt.size());
#if 0
#if 0
    now += 8000;  /* 8 nsec */
    rmt_time_set(now);
#else
    rmt_timer_increment(8000); /* 8 nsec */
#endif
#endif
}

struct expect_t {
    std::vector<uint8_t> pkt, mask;
    bool                full = false;
};

struct expect_port_t {
    std::deque<expect_t>                patterns;
    std::deque<std::vector<uint8_t>>    packets;
    bool                                more = false;
    void match(int);
    void finish(int);
};

static std::map<int, expect_port_t> expecting;
static std::mutex expecting_mutex;
static int error_count;

static char *match_pattern(uint8_t pkt, uint8_t mask) {
    static char buffer[10];
    sprintf(buffer, "%02x", pkt);
    if (mask == 0xff) { /* ok */ }
    else if (mask == 0x0f) { buffer[0] = '*'; }
    else if (mask == 0xf0) { buffer[1] = '*'; }
    else if (mask == 0x00) { buffer[0] = buffer[1] = '*'; }
    else {
        char *p = buffer;
        *p++ = '.';
        for (uint8_t bit = 0x80; bit; bit >>= 1)
            *p++ = mask & bit ? pkt & bit ? '1' : '0' : '*';
        *p = 0; }
    return buffer;
}

/**
 * @param port Port number in MAU port space.
 */
void expect_port_t::match(int port) {
    bool tty = isatty(1);
    while (!patterns.empty() && !packets.empty()) {
        auto &pkt = packets.front();
        auto &match = patterns.front();
        printf("packet output on port %d:\n", port);
        int mismatch = -1;
        for (size_t i = 0; i < pkt.size(); i += 16) {
            printf("%04zx ", i);
            size_t j;
            for (j = 0; j < 16 && i+j < pkt.size(); j++) {
                if (j == 8) putchar(' ');
                bool ok = i+j >= match.pkt.size() || (pkt[i+j] & match.mask[i+j]) == match.pkt[i+j];
                if (tty && !ok)
                    printf(" \033[1;31m%02x\033[0m", pkt[i+j]);
                else
                    printf(" %02x", pkt[i+j]);
                if (mismatch < 0 && !ok)
                    mismatch = i+j; }
            for(; j < 16; j++) printf(j == 8 ? "    " : "   ");
            printf("  ");
            for (j = 0; j < 16 && i+j < pkt.size(); j++) {
                char ch = pkt[i+j] & 0x7f;
                if (ch < ' ' || ch >= 127) ch = '.';
                putchar(ch); }
            printf("\n"); }

        if (mismatch >= 0) {
            printf("mismatch from expected(%s) at byte 0x%x\n",
                   match_pattern(match.pkt[mismatch], match.mask[mismatch]), mismatch);
            ++error_count;
        } else if (pkt.size() < match.pkt.size()) {
            printf("shorter than expected (%zd)\n", match.pkt.size());
            ++error_count;
        } else if (pkt.size() > match.pkt.size() && match.full) {
            printf("longer than expected (%zd)\n", match.pkt.size());
            ++error_count; }
        fflush(stdout);
        packets.pop_front();
        patterns.pop_front();
    }
}

/**
 * @param port Port number in MAU port space.
 */
void expect_port_t::finish(int port) {
    match(port);
    while (!packets.empty()) {
        auto &pkt = packets.front();
        printf("%spacket output on port %d:\n", more ? "" : "unexpected ", port);
        for (size_t i = 0; i < pkt.size(); i += 16) {
            printf("%04zx ", i);
            size_t j;
            for (j = 0; j < 16 && i+j < pkt.size(); j++) {
                if (j == 8) putchar(' ');
                printf(" %02x", pkt[i+j]); }
            for(; j < 16; j++) printf(j == 8 ? "    " : "   ");
            printf("  ");
            for (j = 0; j < 16 && i+j < pkt.size(); j++) {
                char ch = pkt[i+j] & 0x7f;
                if (ch < ' ' || ch >= 127) ch = '.';
                putchar(ch); }
            printf("\n"); }
        fflush(stdout);
        packets.pop_front();
        if (!more) error_count++;
    }
    if (!patterns.empty()) {
        error("%zd expected packet%s on port %d not seen", patterns.size(),
              patterns.size() > 1 ? "s" : "", port);
        ++error_count;
        patterns.clear();
    }
}

ScriptCommand(expect, "<port> <packet data>") {
    expect_t    packet;
    const char  *rest;
    int         port, len;
    if (sscanf(args, "%d %n", &port, &len) < 1) {
        error("usage: expect <port> <data>");
        return;
    } else
        args += len;
#if MCN_TEST(MODEL_CHIP_NAMESPACE,rsvd0)
    if (!use_rmt_port_space && (port & 1))
        error("only even ports usable on rsvd0");
#endif
    int rmt_port = port;
    if (!use_rmt_port_space) rmt_port = port_mau2rmt(port);
    int mau_port = port_rmt2mau(rmt_port);
    LOG3("expect: port: " << port);
    LOG3("expect: rmt_port: " << rmt_port);
    LOG3("expect: mau_port: " << mau_port);
    init_port(mau_port);
    std::lock_guard<std::mutex> lock(expecting_mutex);
    if (!*args || *args == '#') {
        expecting[mau_port].more = true;
    } else if (!(rest = str_to_bytes(args, packet.pkt, &packet.mask))) {
        error("Malformed packet data");
    } else if (packet.pkt.size() > 0) {
        packet.full = *rest == '$';
        expecting[mau_port].patterns.emplace_back(std::move(packet));
        expecting[mau_port].match(mau_port); }
}

/**
 * Using expecting_mutex to guard this too.
 * Oddness -- the "from" port (key) is in MAU port space while the "to" port is in RMT port space.
 * MAU_port[6:0] == (RMT_port[6:0] * 2)[6:0] on Cloudbreak -- on other targets they are the same.
 */
static std::map<int, int> wire;

ScriptCommand(wire, "<port> -> <port>") {
    int from, to, len=0;
    if (sscanf(args,"%d ->%d %n", &from, &to, &len) < 2 || args[len]) {
        error("usage: wire <port> -> <port>");
        return; }
#if MCN_TEST(MODEL_CHIP_NAMESPACE,rsvd0)
    if (!use_rmt_port_space && ((from & 1) || (to & 1)))
        error("only even ports usable on rsvd0");
#endif
    if (!use_rmt_port_space)
        to = port_mau2rmt(to); // `to` is always in RMT port space
    else
        from = port_rmt2mau(from); // `from` is always in MAU port space
    LOG3("wire: from (MAU port space): " << from);
    LOG3("wire: to (RMT port space): " << to);
    std::lock_guard<std::mutex> lock(expecting_mutex);
    wire[from] = to;
}

/**
 * @param port Port number within die (chip) in RMT port space.
 */
void packet_output(int asic_id, int port, uint8_t *buf, int len) {
    /* We use global port numbers in other places, need to convert. */
    port += asic_id * RmtDefsShared::kPortsTotal;

    int mau_port = port_rmt2mau(port);
    LOG2("packet_output: asic_id: " << asic_id << ", port: " << port << ", mau_port: " << mau_port);
    if (wire.count(mau_port)) {
        rmt_packet_receive(PORT2DIE(wire[mau_port]), ASIC_PORT(wire[mau_port]), buf, len);
        LOG3("packet_output: wire[" << mau_port << "]: " << wire[mau_port]);
    }
    // drop 4-byte ethernet CRC
    len -= 4;
    // If an observation log is present, log this packet.
    if (observationLog) {
        if (!use_rmt_port_space)
            fprintf(observationLog, "%d ", mau_port);
        else
            fprintf(observationLog, "%d ", port);
        for (auto i = 0; i < len; i++) {
            fprintf(observationLog, "%02X", buf[i]);
        }
        fprintf(observationLog, "\n");
    }

    {
        // Check against expected.
        std::lock_guard<std::mutex> lock(expecting_mutex);
        expecting[mau_port].packets.emplace_back(buf, buf+len);
        expecting[mau_port].match(mau_port);
    }
}

int check_missing_expected() {
    LOG3("check_missing_expected()");
    std::lock_guard<std::mutex> lock(expecting_mutex);
    for (auto it = expecting.begin(); it != expecting.end();) {
        it->second.finish(it->first);
        if (it->second.more) {
            it->second.packets.clear();
            it->second.patterns.clear();
            ++it;
        } else {
            it = expecting.erase(it); } }
    int rv = error_count;
    error_count = 0;
    return rv;
}

/**
 * @brief Wait until all model internal queues are empty.
 */
void wait_for_idle(void) {
    int ret = 0;
    int counter = 0;
    /**
     * TODO:
     * RmtPacketCoordinator::wait_for_idle() used here is not an official model interface.
     * Interface to wait for package to be idle does not exist at the moment, so we use
     * wait_for_idle() here in the meantime.
     * We need to loop until all dies are idle simultaneously.
     * Otherwise it may happen that not all packets are processed in case of multi-die
     * package (Cloudbreak).
     * RmtPacketCoordinator::wait_for_idle() returns 0 when it was idle and did not need
     * to wait (1 if it had to wait to be idle).
     * So we loop here until all dies were already idle without a need to wait
     * during the same cycle.
     */
    do {
        ret = 0;
        for (int die = 0; die < PKG_SIZE; die++) {
            RmtObjectManager *om = NULL;
            GLOBAL_MODEL->GetObjectManager(die, &om);
            LOG3("before wait_for_idle(): " << die);
            ret += om->packet_coordinator_get()->wait_for_idle();
            LOG3("after wait_for_idle(): " << die << ", ret: " << ret);
        }
        counter++;
    } while (ret != 0);
    LOG2("Waited for idle " << counter << " times");
}

ScriptCommand(wait, "") {
    wait_for_idle();
    check_missing_expected();
}

#if MCN_TEST(MODEL_CHIP_NAMESPACE,jbay) || MCN_TEST(MODEL_CHIP_NAMESPACE,jbayB0) || MCN_TEST(MODEL_CHIP_NAMESPACE,rsvd0)
ScriptCommand(ghost, "queue|run [pipe <pipe>] <qid>:<qlength> ...") {
    RmtObjectManager *om = NULL;
    std::string act = token(&args, " \t\r\n");
    if (act != "run" && act != "queue") {
        error("ghost needs 'queue' or 'run' command");
        return; }
    int qid, qlen, len, pipe = 0;
    unsigned pipe_warn = 0;
    int die = PIPE2DIE(pipe);
    int phy_pipe = PIPE_WITHIN_DIE(pipe);
    GLOBAL_MODEL->GetObjectManager(die, &om);
    Pipe *pipe_obj = om->pipe_lookup(phy_pipe);
    Parser *parser = om->parser_lookup(phy_pipe, 0)->ingress();
    while (sscanf(args, " pipe%d%i:%i %n", &pipe, &qid, &qlen, &len) >= 3 ||
           sscanf(args, "%i:%i %n", &qid, &qlen, &len) >= 2) {
        if (pipe < 0 || pipe > 3) {
            error("invalid pipe %d", pipe);
            break; }
        if (qid < 0 || qid >= 0x800) {
            error("invalid qid %d", qid);
            break; }
        if (qlen < 0 || qlen >= 0x40000) {
            error("invalid qlength %d", qlen);
            break; }
        die = PIPE2DIE(pipe);
        phy_pipe = PIPE_WITHIN_DIE(pipe);
        om = NULL;
        GLOBAL_MODEL->GetObjectManager(die, &om);
        pipe_obj = om->pipe_lookup(phy_pipe);
        parser = om->parser_lookup(phy_pipe, 0)->ingress();
        ParseMergeReg *prs_merge = parser->get_prs_merge();
        uint32_t ghost_md = (qlen << 13) | (qid << 2) | phy_pipe;
        LOG2("ghost queue input " << hex(ghost_md));
        prs_merge->set_tm_status_input(ghost_md);
        if (act == "run") {
            while(Phv *ghost_phv = parser->get_ghost_phv()) {
                if (ghost_phv->ghost())
                    pipe_obj->run_maus(ghost_phv);
                else
                    pipe_warn |= 1 << pipe; } }
        args += len; }
    if (act == "run") {
        while(Phv *ghost_phv = parser->get_ghost_phv()) {
            if (ghost_phv->ghost())
                pipe_obj->run_maus(ghost_phv);
            else
                pipe_warn |= 1 << pipe; } }
    if (pipe_warn)
        warning("ghost packet with no ghost thread in %s", pipe_set_str(pipe_warn).c_str());
    if (*args)
        error("bad ghost_queue: %s", args);
}
#endif
