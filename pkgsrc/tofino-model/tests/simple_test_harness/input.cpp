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

#include <stdlib.h>
#include <inttypes.h>
#include <common/rmt-util.h>
#include <mcn_test.h>
#include <register_utils.h>
#include <generated-versions.h>

#include "bson.h"
#include "hex.h"
#include "input.h"
#include "log.h"
#include "p4name.h"
#include "script.h"
#include "util.h"

using namespace MODEL_CHIP_NAMESPACE;

#define STRINGIFY_RAW(X)  #X
#define STRINGIFY(X) STRINGIFY_RAW(X)

int load_cfg_blob (FILE *fd, model_core::Model *intf, unsigned pipes) {
    rewind(fd);
    uint32_t    PCIe_pipe_bit;
    int         PCIe_pipe_id_shift;
    uint64_t    pipe_addr_pipe_bit;
    int         pipe_addr_pipe_id_shift;
    switch (intf->GetType(0)) {
#if MCN_TEST(MODEL_CHIP_NAMESPACE,tofino) || MCN_TEST(MODEL_CHIP_NAMESPACE,tofinoB0)
    // case model_core::ChipType::kTofino:  -- same as A0
    case model_core::ChipType::kTofinoA0:
    case model_core::ChipType::kTofinoB0:
        // FIXME -- these should be in rmt-def.h or a header file somewhere...
        PCIe_pipe_bit = 1U << 25;
        PCIe_pipe_id_shift = 23;
        pipe_addr_pipe_bit = 1ULL << 41;
        pipe_addr_pipe_id_shift = 37;
        break;
#elif MCN_TEST(MODEL_CHIP_NAMESPACE,jbay) || MCN_TEST(MODEL_CHIP_NAMESPACE,jbayB0) || MCN_TEST(MODEL_CHIP_NAMESPACE,rsvd0)
    // case model_core::ChipType::kJbay:  -- same as A0
    case model_core::ChipType::kJbayA0:
    case model_core::ChipType::kJbayB0:
    // case model_core::ChipType::kRsvd0:  -- same as A0
    case model_core::ChipType::kRsvd1:
    case model_core::ChipType::kRsvd2:
        // FIXME -- these should be in rmt-def.h or a header file somewhere...
        PCIe_pipe_bit = 1U << 26;
        PCIe_pipe_id_shift = 24;
        pipe_addr_pipe_bit = 1ULL << 41;
        pipe_addr_pipe_id_shift = 39;
        break;
#elif MCN_TEST(MODEL_CHIP_NAMESPACE,rsvd1)
    case model_core::ChipType::kRsvd3:
        // FIXME -- these should be in rmt-def.h or a header file somewhere...
        PCIe_pipe_bit = 1U << 27;
        PCIe_pipe_id_shift = 23;
        pipe_addr_pipe_bit = 1ULL << 41;
        pipe_addr_pipe_id_shift = 36;
        break;
#endif
    default:
        std::cerr << "Unexpected chip type " << intf->GetType(0) << std::endl;
        return -1; }

    uint32_t atom_typ = 0;
    unsigned seen_pipe = 0;
    int stages = -1;
    while (fread(&atom_typ, 4, 1, fd) == 1) {
        if ((atom_typ >> 24) == 'H') {
            json::map       header;
            fd >> binary(header);
            auto target = header["target"]->to<std::string>();
            if (target.substr(0, 6) != "tofino" ||
#if MCN_TEST(MODEL_CHIP_NAMESPACE,rsvd1)
                target[6] != '5'
#elif MCN_TEST(MODEL_CHIP_NAMESPACE,rsvd0)
                target[6] != '3'
#elif MCN_TEST(MODEL_CHIP_NAMESPACE,jbay) || MCN_TEST(MODEL_CHIP_NAMESPACE,jbayB0)
                target[6] != '2'
#else
                (target[6] != '1' && target[6] != 0)
#endif
            ) {
                std::cerr << "Can't load binary for target '" << target << "' on chip type "
                          << STRINGIFY(MODEL_CHIP_NAMESPACE) << std::endl;
                return -1; }
            if (header["stages"]) {
                stages = header["stages"]->to<json::number>().val;
            } else if (target == "tofino2m") {
                stages = 12;
            } else if (target == "tofino2h") {
                stages = 6; }
            LOG1("Loading " << header["target"]->to<std::string>() << " program with " <<
                 stages << " stages");
            auto reg_version = header["reg_version"]->to<std::string>();
            if (reg_version != BFNREGS_TAG)
                std::cerr << "WARNING: loading binary for " << reg_version << " does not match "
                          << "model version " << BFNREGS_TAG << std::endl;
        } else if ((atom_typ >> 24) == 'C') {
            // future improvement -- context json in the binary file
            std::unique_ptr<json::obj>  cjson;
            fd >> binary(cjson);
            LoadP4Json(std::move(cjson), pipes);
        } else if ((atom_typ >> 24) == 'R') {
            // R block -- writing a single 32-bit register via 32-bit PCIe address
            uint32_t reg_addr = 0, reg_data = 0;
            unsigned pipe_mask = pipes;
            if (fread(&reg_addr, 4, 1, fd) != 1) return -1;
            if (fread(&reg_data, 4, 1, fd) != 1) return -1;
            if (reg_addr & PCIe_pipe_bit) {
                int pipe_addr = (reg_addr >> PCIe_pipe_id_shift) & 3;
                reg_addr &= ~(3 << PCIe_pipe_id_shift);
                seen_pipe |= 1 << pipe_addr;
                if (seen_pipe != 1)
                    pipe_mask &= 1U << pipe_addr;
            } else if (((reg_addr >> PCIe_pipe_id_shift) & 3) == 0) {
                reg_addr |= PCIe_pipe_bit; }
            for (int die = 0; die < PKG_SIZE; die++)
                for (int i = 0; i < RmtDefs::kPipesMax; ++i)
                    if (pipe_mask & (1U << (i + die * RmtDefs::kPipesMax)))
                        intf->OutWord(die, reg_addr + (i << PCIe_pipe_id_shift), reg_data);
        } else if ((atom_typ >> 24) == 'B') {
            // B block -- write a range of 32-bit registers via 64-bit PCIe address
            // size of the range is specified as count * width (in bits), which must
            // always be a multiple of 32

            uint64_t addr = 0;
            uint32_t count = 0;
            uint32_t width = 0;
            unsigned pipe_mask = pipes;

            if (fread(&addr, 8, 1, fd) != 1) return -1;
            if (fread(&width, 4, 1, fd) != 1) return -1;
            if (fread(&count, 4, 1, fd) != 1) return -1;

            LOG3("B" << hex(addr) << " " << width << " x 0x" << hex(count));
            if (addr & PCIe_pipe_bit) {
                int pipe_addr = (addr >> PCIe_pipe_id_shift) & 3;
                addr &= ~(3 << PCIe_pipe_id_shift);
                seen_pipe |= 1 << pipe_addr;
                if (seen_pipe != 1)
                    pipe_mask &= 1U << pipe_addr;
            } else if (((addr >> PCIe_pipe_id_shift) & 3) == 0) {
                addr |= PCIe_pipe_bit;
            }

            count = (uint64_t)count * width / 32;

            for (bool first = true; count > 0; --count, first = false) {
                uint32_t data;
                if (fread(&data, 4, 1, fd) != 1) return -1;
                for (int die = 0; die < PKG_SIZE; die++) {
                    for (int i = 0; i < RmtDefs::kPipesMax; ++i) {
                        if (pipe_mask & (1U << (i + die * RmtDefs::kPipesMax))) {
                            if (first)
                                LOG3("  writing to " << hex(addr + (i << PCIe_pipe_id_shift)));
                            intf->OutWord(die, addr + (i << PCIe_pipe_id_shift), data);
                        }
                    }
                }
                addr += 4;
            }

        } else if ((atom_typ >> 24) == 'D') {
            // D block -- write a range of 128-bit memory via 64-bit chip address
            // size of the range is specified as count * width (in bits), which must
            // always be a multiple of 64

            uint64_t dma_addr = 0;
            uint32_t dma_elem_count = 0;
            uint32_t dma_elem_width = 0;
            unsigned pipe_mask = pipes;

            if (fread(&dma_addr, 8, 1, fd) != 1) return -1;
            if (fread(&dma_elem_width, 4, 1, fd) != 1) return -1;
            if (fread(&dma_elem_count, 4, 1, fd) != 1) return -1;

            LOG3("D" << hex(dma_addr) << " " << dma_elem_width << " x 0x" << hex(dma_elem_count));
            if (dma_addr & pipe_addr_pipe_bit) {
                int pipe_addr = (dma_addr >> pipe_addr_pipe_id_shift) & 3;
                dma_addr &= ~(3ULL << pipe_addr_pipe_id_shift);
                seen_pipe |= 1 << pipe_addr;
                if (seen_pipe != 1)
                    pipe_mask &= 1U << pipe_addr;
            } else if (((dma_addr >> pipe_addr_pipe_id_shift) & 3) == 0) {
                dma_addr |= pipe_addr_pipe_bit;
            }

            dma_elem_width /= 8;

            int offset = 0;
            bool first = true;
            for (unsigned i = 16; i <= dma_elem_count*dma_elem_width; i += 16) {
                uint64_t chunk[2];
                if (fread(chunk, sizeof(uint64_t), 2, fd) != 2) return -1;
                for (int die = 0; die < PKG_SIZE; die++) {
                    for (int i = 0; i < RmtDefs::kPipesMax; ++i) {
                        if (pipe_mask & (1U << (i + die * RmtDefs::kPipesMax))) {
                            auto addr = dma_addr + offset +
                                    ((uint64_t)i << pipe_addr_pipe_id_shift);
                            if (first)
                                LOG3("  writing to " << hex(addr));
                            intf->IndirectWrite(die, addr, chunk[0], chunk[1]);
                        }
                    }
                }
                offset++;
                first = false;
            }

            int processed_bytes = (dma_elem_count * dma_elem_width / 16) * 16;
            int trailing_bytes = dma_elem_count * dma_elem_width - processed_bytes;

            uint64_t *trailer_64 = (uint64_t *) calloc(sizeof(uint64_t),2);
            uint8_t *trailer = (uint8_t *) trailer_64;
            for (int i = 0; i < trailing_bytes; i++){
                if (fread(trailer+i, 1, 1, fd) != 1) return -1;
            }
            if (trailing_bytes == 8) {
                for (int die = 0; die < PKG_SIZE; die++) {
                    for (int i = 0; i < RmtDefs::kPipesMax; ++i) {
                        if (pipe_mask & (1U << (i + die * RmtDefs::kPipesMax))) {
                            auto addr = dma_addr + offset +
                                    ((uint64_t)i << pipe_addr_pipe_id_shift);
                            intf->IndirectWrite(die, addr, trailer_64[0], trailer_64[1]);
                        }
                    }
                }
            }
        } else if ((atom_typ >> 24) == 'S') {
            // S block -- 'scanset' writing multiple data to a single 32-bit PCIE address
            uint64_t sel_addr = 0, reg_addr = 0;
            uint32_t sel_data = 0, width = 0, count = 0;
            unsigned pipe_mask = pipes;
            if (fread(&sel_addr, 8, 1, fd) != 1) return -1;
            if (fread(&sel_data, 4, 1, fd) != 1) return -1;
            if (fread(&reg_addr, 8, 1, fd) != 1) return -1;
            if (fread(&width, 4, 1, fd) != 1) return -1;
            if (fread(&count, 4, 1, fd) != 1) return -1;
            if (width % 32) return -1;   // width must be multiple of 32
            count = (uint64_t)count * width / 32;
            std::vector<uint32_t> reg_data(count);
            if (fread(&reg_data[0], 4, count, fd) != count) return -1;
            if (sel_addr & PCIe_pipe_bit) {
                if (!(reg_addr & PCIe_pipe_bit)) return -1;
                unsigned pipe_addr = (sel_addr >> PCIe_pipe_id_shift) & 3U;
                if (pipe_addr != ((reg_addr >> PCIe_pipe_id_shift) & 3U))
                    return -1;
                sel_addr &= ~(3 << PCIe_pipe_id_shift);
                reg_addr &= ~(3 << PCIe_pipe_id_shift);
                seen_pipe |= 1 << pipe_addr;
                if (seen_pipe != 1)
                    pipe_mask &= 1U << pipe_addr;
            } else if (((reg_addr >> PCIe_pipe_id_shift) & 3) == 0) {
                if (sel_addr)
                    sel_addr |= PCIe_pipe_bit;
                reg_addr |= PCIe_pipe_bit; }
            for (int die = 0; die < PKG_SIZE; die++) {
                for (int i = 0; i < RmtDefs::kPipesMax; ++i) {
                    if (pipe_mask & (1U << (i + die * RmtDefs::kPipesMax))) {
                        if (sel_addr)
                            intf->OutWord(die, sel_addr + (i << PCIe_pipe_id_shift), sel_data);
                        uint32_t off = 0;
                        for (auto data : reg_data) {
                            intf->OutWord(die, reg_addr + off + (i << PCIe_pipe_id_shift), data);
                            off = (off + 4) % (width/8);
                        }
                    }
                }
            }

        } else if ((atom_typ >> 24) == 'r') {
            // r block -- writing a single 32-bit register without checks
            uint32_t reg_addr = 0, reg_data = 0;
            if (fread(&reg_addr, 4, 1, fd) != 1) return -1;
            if (fread(&reg_data, 4, 1, fd) != 1) return -1;

            for (int die = 0; die < PKG_SIZE; die++) {
                intf->OutWord(die, reg_addr, reg_data);
            }

        } else if ((atom_typ >> 24) == 'd') {
            // d block -- write a range of 128-bit memory via 64-bit chip address
            // without checks
            // size of the range is specified as count * width (in bits), which must
            // always be a multiple of 64
            uint64_t dma_addr = 0;
            uint32_t dma_elem_count = 0;
            uint32_t dma_elem_width = 0;

            if (fread(&dma_addr, 8, 1, fd) != 1) return -1;
            if (fread(&dma_elem_width, 4, 1, fd) != 1) return -1;
            if (fread(&dma_elem_count, 4, 1, fd) != 1) return -1;

            LOG3("d" << hex(dma_addr) << " " << dma_elem_width << " x 0x" << hex(dma_elem_count));

            dma_elem_width /= 8;

            int offset = 0;
            for (unsigned i = 16; i <= dma_elem_count * dma_elem_width; i += 16) {
                uint64_t chunk[2];
                if (fread(chunk, sizeof(uint64_t), 2, fd) != 2) return -1;
                auto addr = dma_addr + offset;
                LOG3("  writing to " << hex(addr));
                for (int die = 0; die < PKG_SIZE; die++) {
                    intf->IndirectWrite(die, addr, chunk[0], chunk[1]);
                }
                offset++;
            }

        } else {
            fprintf(stderr, "\n");
            fprintf(stderr, "Parse error: atom_typ=%x (%c)\n", atom_typ, atom_typ >> 24 );
            fprintf(stderr, "fpos=%lu <%lxh>\n", ftell(fd), ftell(fd) );
            fprintf(stderr, "\n");
            fflush(stderr);

            return -1;
        }

    }
    if (seen_pipe > 1 && (~pipes & seen_pipe)) {
        fprintf(stderr, "Binary for %s contains data for %s\n", pipe_set_str(pipes).c_str(),
                pipe_set_str(~pipes & seen_pipe).c_str());
        /* just a warning for now -- should be an error? */ }

#if 0  /* stage extension now set from context.json */
#if MCN_TEST(MODEL_CHIP_NAMESPACE,jbay) || MCN_TEST(MODEL_CHIP_NAMESPACE,jbayB0) || MCN_TEST(MODEL_CHIP_NAMESPACE,rsvd0)
    if (!raw_mode) {
    // only for non-tofino at the moment -- may need support there?  regs are different
    if (stages > 0 && stages < intf->GetNumStages(0)) {
        fprintf(stderr, "Need to pad out unused stages %d..%d\n", stages, intf->GetNumStages(0)-1);
        for (unsigned pipe = 0; (1U << pipe) <= pipes; ++pipe) {
            if (!((pipes >> pipe) & 1)) continue;
            int die = PIPE2DIE(pipe);
            int phy_pipe = PIPE_WITHIN_DIE(pipe);
            uint32_t adr_dist_pipe_delay[2] = {};
            uint32_t eop_bus_delay[2] = {};
            uint32_t exact_match_delay_thread = 0;
            uint32_t pipelength_added_stages[2] = {};
            uint32_t phv_ingress_thread[2][14] = {}, phv_egress_thread[2][14] = {};
            for (int stage = stages - 1; stage < intf->GetNumStages(die); ++stage) {
                bool first = stage < stages;
                bool last = stage == intf->GetNumStages(die) - 1;
                auto &regs = RegisterUtils::ref_mau(phy_pipe, stage);
                auto &merge = regs.rams.match.merge;
                auto &adrdist = regs.rams.match.adrdist;
                auto &dp = regs.dp;
                if (first) {
                    adr_dist_pipe_delay[0] = intf->InWord(die, &adrdist.adr_dist_pipe_delay[0][0]);
                    adr_dist_pipe_delay[1] = intf->InWord(die, &adrdist.adr_dist_pipe_delay[1][0]);
                    eop_bus_delay[0] = intf->InWord(die, &adrdist.deferred_eop_bus_delay[0]);
                    eop_bus_delay[1] = intf->InWord(die, &adrdist.deferred_eop_bus_delay[1]);
                    exact_match_delay_thread =
                            intf->InWord(die, &merge.exact_match_delay_thread[0]);
                    pipelength_added_stages[0] = intf->InWord(die, &dp.pipelength_added_stages[0]);
                    pipelength_added_stages[1] = intf->InWord(die, &dp.pipelength_added_stages[1]);
                    for (int i = 0; i < 2; ++i) {
                      for (int j = 0; j < 14; ++j) {
                        phv_ingress_thread[i][j] = intf->InWord(die, &dp.phv_ingress_thread[i][j]);
                        phv_egress_thread[i][j] = intf->InWord(die, &dp.phv_egress_thread[i][j]);
                      }
                    }
                } else {
                    intf->OutWord(die, &adrdist.adr_dist_pipe_delay[0][0], adr_dist_pipe_delay[0]);
                    intf->OutWord(die, &adrdist.adr_dist_pipe_delay[0][1], adr_dist_pipe_delay[0]);
                    intf->OutWord(die, &adrdist.adr_dist_pipe_delay[1][0], adr_dist_pipe_delay[1]);
                    intf->OutWord(die, &adrdist.adr_dist_pipe_delay[1][1], adr_dist_pipe_delay[1]);
                    for (int i = 0; i < 2; ++i) {
                      for (int j = 0; j < 14; ++j) {
                        intf->OutWord(die, &dp.phv_ingress_thread[i][j], phv_ingress_thread[i][j]);
                        intf->OutWord(die, &dp.phv_ingress_thread_imem[i][j],
                                phv_ingress_thread[i][j]);
                        intf->OutWord(die, &dp.phv_egress_thread[i][j], phv_egress_thread[i][j]);
                        intf->OutWord(die, &dp.phv_egress_thread_imem[i][j],
                                phv_egress_thread[i][j]);
                      }
                    }
                    intf->OutWord(die, &dp.cur_stage_dependency_on_prev[0], 1);
                    intf->OutWord(die, &dp.cur_stage_dependency_on_prev[1], 1);
                    intf->OutWord(die, &dp.pipelength_added_stages[0], pipelength_added_stages[0]);
                    intf->OutWord(die, &dp.pipelength_added_stages[1], pipelength_added_stages[1]);
                    intf->OutWord(die, &merge.exact_match_delay_thread[0],
                            exact_match_delay_thread);
                    intf->OutWord(die, &merge.exact_match_delay_thread[1],
                            exact_match_delay_thread);
                    intf->OutWord(die, &merge.exact_match_delay_thread[2],
                            exact_match_delay_thread);
                    intf->OutWord(die, &merge.pred_stage_id, stage);
                }
                if (!last) {
                    intf->OutWord(die, &adrdist.deferred_eop_bus_delay[0],
                            (eop_bus_delay[0] & ~0x3e0) | 0x20);
                    intf->OutWord(die, &adrdist.deferred_eop_bus_delay[1],
                            (eop_bus_delay[1] & ~0x3e0) | 0x20);
                    intf->OutWord(die, &dp.next_stage_dependency_on_cur[0], 1);
                    intf->OutWord(die, &dp.next_stage_dependency_on_cur[1], 1);
                    intf->OutWord(die, &merge.mpr_bus_dep, 0x3);
                } else {
                    intf->OutWord(die, &adrdist.deferred_eop_bus_delay[0], eop_bus_delay[0]);
                    intf->OutWord(die, &adrdist.deferred_eop_bus_delay[1], eop_bus_delay[1]);
                }
            }
        }
    }
    }
#endif
#endif

    return ferror(fd) ? -1 : 0;
}

const char *str_to_bytes(const char *str, std::vector<uint8_t> &pkt_bytes, std::vector<uint8_t> *mask) {
    pkt_bytes.clear();
    if (mask) mask->clear();
    while (true) {
        if (str[0] == ' ' || str[0] == '\t') {
            str++;
            continue; }
        if (str[0] == '\0' || str[0] == '\r' || str[0] == '\n' || str[0] == '#' || str[0] == '$')
            break;
        int val = 0;
        int maskval = 0xff;
        if (mask && str[0] == '.') {
            for (int i = 1; i <= 8; ++i)
                switch (str[i]) {
                case '0': break;
                case '1': val |= 0x100 >> i; break;
                case '*': maskval &= ~0x100 >> i; break;
                default: return nullptr; }
            str += 9;
        } else {
            int nextValidIndex = 1;
            while(str[nextValidIndex] == ' ') nextValidIndex++;
            if (mask && str[nextValidIndex] == '*')
                maskval &= ~0xf;
            else
                val |= model_common::Util::hexchar2int(str[nextValidIndex]);
            if (mask && str[0] == '*')
                maskval &= ~0xf0;
            else
                val |= model_common::Util::hexchar2int(str[0]) << 4;
            str += nextValidIndex+1;
        }
        if (val < 0) return nullptr;
        pkt_bytes.push_back(val);
        if (mask) mask->push_back(maskval); }
    return str;
}
