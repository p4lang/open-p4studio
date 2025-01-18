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
#if !MCN_TEST(MODEL_CHIP_NAMESPACE,rsvd1)
#include <mau-tcam.h>
#endif // !WIP
#include "script.h"
#include "write.h"

using namespace MODEL_CHIP_NAMESPACE;

bool tcam_2bit_mode = true;

ScriptCommand(tcam_2bit_mode, "true | false") {
    char *arg = token(&args);
    if (!strcasecmp(arg, "true"))
        tcam_2bit_mode = true;
    else if (!strcasecmp(arg, "false"))
        tcam_2bit_mode = false;
    else
        error("bad boolean %s", arg);
}

static struct { uint8_t d1, d0; } cvt_2bit[4][4] = {
    { { 0b00, 0b00 }, { 0b00, 0b00 }, { 0b00, 0b00 }, { 0b10, 0b00 }},
    { { 0b00, 0b00 }, { 0b00, 0b00 }, { 0b01, 0b00 }, { 0b11, 0b00 }},
    { { 0b00, 0b00 }, { 0b00, 0b10 }, { 0b00, 0b00 }, { 0b10, 0b10 }},
    { { 0b00, 0b01 }, { 0b00, 0b11 }, { 0b01, 0b01 }, { 0b11, 0b11 }}
};

void convert_to_2bit(uint64_t &data0, uint64_t &data1) {
    uint64_t    mask = 3U;
    int         shift = 0;
    for (; mask; mask <<= 2, shift += 2) {
        auto &cvt = cvt_2bit[(data0 & mask) >> shift][(data1 & mask) >> shift];
        data0 = (data0 & ~mask) | ((uint64_t)cvt.d0 << shift);
        data1 = (data1 & ~mask) | ((uint64_t)cvt.d1 << shift); }
}

ScriptCommand(write_raw_tcam, "<stage>:<unit>:<addr> <data> [& <mask>]") {
    int stage, unit, addr, len;

    if (sscanf(args, "%i :%i :%i %n", &stage, &unit, &addr, &len) < 3) {
        error("bad address");
        return; }
    args += len;
    if (addr < 0 || addr >= 512) { error("Address %d out of range", addr); return; }
    uint64_t    data0 = ~0UL, data1 = ~0UL, mask;
    if (sscanf(args, "%" PRIx64 " & %" PRIx64 " %n", &data1, &mask, &len) >= 2) {
        data0 = ~data1 | ~mask;
        data1 |= ~mask;
    } else {
        data0 = ~0UL; data1 = ~0UL;
        int digit;
        while (*args) {
            switch(*args++) {
            case ' ': case '\t': case '\r': case '\n': continue;
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
                digit = args[-1] - '0'; break;
            case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
                digit = args[-1] - 'a' + 10; break;
            case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
                digit = args[-1] - 'A' + 10; break;
            case '*':
                data0 <<= 4; data0 |= 0xf;
                data1 <<= 4; data1 |= 0xf;
                continue;
            default:
                error("bad tcam value");
                return; }
            data0 <<= 4; data0 |= 0xf^digit;
            data1 <<= 4; data1 |= digit; } }
    if (tcam_2bit_mode)
        convert_to_2bit(data0, data1);

#if !MCN_TEST(MODEL_CHIP_NAMESPACE,rsvd1)
    RmtObjectManager *om = NULL;
    GLOBAL_MODEL->GetObjectManager(0, &om);
    // FIXME:
    // This command uses always pipe 0, when the need arises and we add
    // an optional pipe to the command, we need to fix this to use correct die
    // and pipe.
    auto *mau = om->mau_lookup(0, stage);
    if (!mau) { error("No mau stage %d", stage); return; }
    auto *tcam = mau->tcam_lookup(unit);
    if (!tcam) { error("No tcam unit %d", unit); return; }
    tcam->write(addr, data0<<1, data1<<1, UINT64_C(0));
#endif // !WIP
}

ScriptCommand(write_raw_sram, "<stage>:<unit>:<addr> <data> [& <mask>] [<< <shift>]") {
    int stage, unit, addr, len;

    if (sscanf(args, "%i :%i :%i %n", &stage, &unit, &addr, &len) < 3) {
        error("bad address");
        return; }
    args += len;
#if !MCN_TEST(MODEL_CHIP_NAMESPACE,rsvd1)
    RmtObjectManager *om = NULL;
    GLOBAL_MODEL->GetObjectManager(0, &om);
    // FIXME:
    // This command uses always pipe 0, when the need arises and we add
    // an optional pipe to the command, we need to fix this to use correct die
    // and pipe.
    auto *mau = om->mau_lookup(0, stage);
    if (!mau) { error("No mau stage %d", stage); return; }
    auto *sram = mau->sram_lookup(unit); // MauSram
    if (!sram) { error("No sram unit %d", unit); return; }
#endif // !WIP
    if (addr < 0 || addr >= 1024) { error("Address %d out of range", addr); return; }

    uint64_t    data0, data1, val, mask;
    int         shift;
    if (sscanf(args, "%" PRIx64 " & %" PRIx64 " << %u %n", &val, &mask, &shift, &len) >= 3) {
        data0 = data1 = 0;
#if !MCN_TEST(MODEL_CHIP_NAMESPACE,rsvd1)
        sram->read(addr, &data0, &data1, UINT64_C(0));
#endif // !WIP
        if (shift < 64) {
            data0 &= ~(mask << shift);
            data1 &= ~(mask >> (64-shift));
            data0 |= val << shift;
            data1 |= val >> (64-shift);
        } else if (shift >= 128) {
            error("shift %d too large", shift);
        } else {
            data1 &= ~(mask << (shift-64));
            data1 |= val << (shift-64); }
        args += len;
    } else {
        data0 = data1 = 0;
        int digit;
        while (*args) {
            switch(*args++) {
            case ' ': case '\t': case '\r': case '\n': continue;
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
                digit = args[-1] - '0'; break;
            case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
                digit = args[-1] - 'a' + 10; break;
            case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
                digit = args[-1] - 'A' + 10; break;
            case '#': *args = 0; continue;
            default:
                error("bad sram value");
                return; }
            data1 <<= 4; data1 |= (data0 >> 60) & 0xf;
            data0 <<= 4; data0 |= digit; } }
#if !MCN_TEST(MODEL_CHIP_NAMESPACE,rsvd1)
    sram->write(addr, data0, data1, UINT64_C(0));
#endif // !WIP
}
