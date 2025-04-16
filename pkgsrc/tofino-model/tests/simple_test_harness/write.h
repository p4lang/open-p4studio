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

#ifndef _write_h_
#define _write_h_

#include <model_core/model.h>
#include "log.h"
#include "hex.h"

extern bool tcam_2bit_mode;
void convert_to_2bit(uint64_t &data0, uint64_t &data1);

extern std::unique_ptr<model_core::Model> GLOBAL_MODEL;

inline uint32_t read_reg(int die, volatile void *addr) {
    return GLOBAL_MODEL->InWord(die, addr); }
inline void write_reg(int die, volatile void *addr, uint32_t data) {
    GLOBAL_MODEL->OutWord(die, addr, data); }

#if MCN_TEST(MODEL_CHIP_NAMESPACE,rsvd1)

namespace bfn_mem {
#include "../mimic/hw/ftr/registers/include/mem.h"
}
#include "table_config.h"

struct StageRef {
    typedef table_config::gress_t gress_t;
    int         die, stage;
    gress_t     gress;
    uint64_t    addr;
    StageRef() = delete;
    StageRef(int chip, int pipe, int s, gress_t gr) : die(chip), stage(s), gress(gr) {
        addr = pipe * bfn_mem::tfc_mem_fpp_array_element_size; }
    struct MemRef {
        int             die;
        uint64_t        addr;
        MemRef(int d, uint64_t a) : die(d), addr(a) {}
        void read(const int index, uint64_t *data0, uint64_t *data1, uint64_t T) const {
            LOG5("    read 0x" << hex(addr/16 + index));
            GLOBAL_MODEL->IndirectRead(die, addr/16 + index, data0, data1); }
        void write(const int index, uint64_t data0, uint64_t data1, uint64_t T) const {
            LOG5("    write 0x" << hex(addr/16 + index));
            GLOBAL_MODEL->IndirectWrite(die, addr/16 + index, data0, data1); }
    };
// shorten the overlong names of the memory registers
#define BASE(reg)  bfn_mem::tfc_mem_fpp_ppu_pack_mem_##reg
#define ELSZ(reg)  bfn_mem::tfc_mem_fpp_ppu_pack_mem_##reg##_array_element_size
    MemRef sram_lookup(int unit) {
        if (gress == table_config::EGRESS)
            return MemRef(die, BASE(estm_mem_address) + addr +
                unit * ELSZ(estm_mem_stm_row_mem_ram));
        else
            return MemRef(die, BASE(istm_mem_address) + addr +
                unit * ELSZ(istm_mem_stm_row_mem_ram)); }
    MemRef tcam_lookup(int unit) {
        return MemRef(die, BASE(scm_mem_scm_stage_mem_tcam_address) + addr +
            unit * ELSZ(scm_mem_scm_stage_mem_tcam)); }
    MemRef tind_lookup(int unit) {
        int st = gress == table_config::EGRESS ? 13 - stage : stage;
        return MemRef(die, BASE(scm_mem_scm_stage_mem_tind_address) + addr +
            st * ELSZ(scm_mem_scm_stage_mem) + unit * ELSZ(scm_mem_scm_stage_mem_tind)); }
    MemRef lamb_lookup(int unit) {
        if (gress == table_config::EGRESS)
            return MemRef(die, BASE(eppu_mem_ppu_em_mem_em_lamb_table_address) + addr +
                stage * ELSZ(eppu_mem) + unit * ELSZ(eppu_mem_ppu_em_mem_em_lamb_table));
        else
            return MemRef(die, BASE(ippu_mem_ppu_em_mem_em_lamb_table_address) + addr +
                stage * ELSZ(ippu_mem) + unit * ELSZ(ippu_mem_ppu_em_mem_em_lamb_table));
    }
#undef BASE
#undef ELSZ
    MemRef mapram_lookup(int unit) {
        error("rsvd1 mapram access not implemented yet");
        return MemRef(die, 0); }
};

#else

#include <rmt-object-manager.h>
#include <mau.h>
#include <mau-tcam.h>
#include "table_config.h"

// non-WIP directly accesses via Mau object
struct StageRef {
    typedef table_config::gress_t gress_t;
    MODEL_CHIP_NAMESPACE::RmtObjectManager *om;
    MODEL_CHIP_NAMESPACE::Mau *mau;
    StageRef() = delete;
    StageRef(int chip, int pipe, int stage, gress_t) {
        GLOBAL_MODEL->GetObjectManager(chip, &om);
        mau = om->mau_lookup(pipe, stage); }
    struct ObjRef {
        enum { NONE, SRAM, TCAM, MAPRAM }       type;
        union {
            MODEL_CHIP_NAMESPACE::MauSram       *sram;
            MODEL_CHIP_NAMESPACE::MauTcam       *tcam;
            MODEL_CHIP_NAMESPACE::MauMapram     *mapram;
        };
        explicit ObjRef(MODEL_CHIP_NAMESPACE::MauSram *s) : type(SRAM) { sram = s; }
        explicit ObjRef(MODEL_CHIP_NAMESPACE::MauTcam *t) : type(TCAM) { tcam = t; }
        explicit ObjRef(MODEL_CHIP_NAMESPACE::MauMapram *m) : type(MAPRAM) { mapram = m; }
        explicit ObjRef() : type(NONE) { sram = nullptr; }
        void read(const int index, uint64_t *data0, uint64_t *data1, uint64_t T) const {
            switch (type) {
            case SRAM: sram->read(index, data0, data1, T); break;
            case TCAM: tcam->read(index, data0, data1, T); break;
            case MAPRAM: mapram->read(index, data0, data1, T); break;
            default: *data0 = *data1 = 0; } }
        void write(const int index, uint64_t data0, uint64_t data1, uint64_t T) const {
            switch (type) {
            case SRAM: sram->write(index, data0, data1, T); break;
            case TCAM: tcam->write(index, data0, data1, T); break;
            case MAPRAM: mapram->write(index, data0, data1, T); break;
            default: break; } }
    };
    ObjRef sram_lookup(int unit) { return ObjRef(mau->sram_lookup(unit)); }
    ObjRef tcam_lookup(int unit) { return ObjRef(mau->tcam_lookup(unit)); }
    ObjRef mapram_lookup(int unit) { return ObjRef(mau->mapram_lookup(unit)); }
    ObjRef lamb_lookup(int) {
        error("no lambs on target");
        return ObjRef(); }
    ObjRef tind_lookup(int) {
        error("no dedicated tind on target");
        return ObjRef(); }
};

#endif

#endif /* _write_h_ */
