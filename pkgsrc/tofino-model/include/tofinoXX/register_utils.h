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

#ifndef _TOFINOXX_REGISTER_UTILS_
#define _TOFINOXX_REGISTER_UTILS_

#include <cstddef>
#include <model_core/register_categories.h>
#include <rmt-defs.h>
#include <register_includes/reg.h>


namespace MODEL_CHIP_NAMESPACE {

  class RegisterUtils {
 public:
    static constexpr int  kPipesMax = RmtDefs::kPipesMax;
    static constexpr int  kPipeMask = RmtDefs::kPipeMask;
    static constexpr int  kStagesMax = RmtDefs::kStagesMax;
    static constexpr int  kStageMask = RmtDefs::kStageMask;


    // Used in tm-regs.h to make 2 blocks of fake registers: one from the start of TM to the start of PRE,
    //  and one after the end of PRE up to the end of TM.
    static constexpr uint32_t kTmDefaultRegVal = (BAD_DATA_WORD & 0xFFFF0000) | 0xD3FF;//Lower bits must be set to d3ff to keep driver happy
    static constexpr size_t kTmStartAddress  = tofino_device_select_tm_top_address;
    static constexpr size_t kPreStartAddress = tofino_device_select_tm_top_tm_pre_top_address;
    static constexpr size_t kPscStartAddress = tofino_device_select_tm_top_tm_psc_top_address;
    static constexpr size_t kTmLastAddress   = tofino_device_select_tm_top_tm_psc_top_psc_common_underflow_errlog_address;

    static const size_t     offset_tm_pre_common_blk() { return tofino_device_select_tm_top_tm_pre_top_pre_common_blk_id_address; }
    static int              rdm_blk_word_off(int rb)   { return rb/16; }
    static int              rdm_blk_bit_off(int rb)    { return 2*(rb%16); } // PipeId is 2b

    static Dvsl_addrmap*      addr_devsel()            { return &(((PTR_Tofino)0)->device_select);     }
    static Misc_regs*         addr_misc_regs()         { return &addr_devsel()->misc_regs; }
    static volatile uint32_t *addr_misc_regs_dbg_rst() { return &addr_misc_regs()->dbg_rst1; }
    static Lfltr_rspec*       addr_lfltr(int p)        {
      switch (p) {
        case 0:  return &addr_devsel()->lfltr0;
        case 1:  return &addr_devsel()->lfltr1;
        case 2:  return &addr_devsel()->lfltr2;
        case 3:  return &addr_devsel()->lfltr3;
        default: return NULL;
      }
    }
    static auto addr_pcie_regs() -> decltype( &addr_devsel()->pcie_bar01_regs.pcie_regs )  {
      return &addr_devsel()->pcie_bar01_regs.pcie_regs;
    }


    static Pmarb_rspec*     addr_pmarb(int p)     { return &(((PTR_Tofino)0)->pipes[p].pmarb);    }
    static Dprsr_reg_rspec* addr_dprsr(int p)     { return &(((PTR_Tofino)0)->pipes[p].deparser); }

    static auto addr_ingbuf(int p, int ipb) -> decltype( &addr_pmarb(p)->ibp18_reg.ibp_reg[ipb].ing_buf_regs ) {
      return &addr_pmarb(p)->ibp18_reg.ibp_reg[ipb].ing_buf_regs;
    }
    static auto addr_egrbuf(int p, int epb) -> decltype( &addr_pmarb(p)->ebp18_reg.ebp_reg[epb].epb_prsr_port_regs ) {
      return &addr_pmarb(p)->ebp18_reg.ebp_reg[epb].epb_prsr_port_regs;
    }
    static auto addr_mirbuf(int p) -> decltype( &addr_dprsr(p)->mirror ) {
      return &addr_dprsr(p)->mirror;
    }
    static Mau_addrmap*     addr_mau(int p,int s) { return &(((PTR_Tofino)0)->pipes[p].mau[s]);   }

    static Mau_addrmap&     ref_mau(int p,int s)  { return (((PTR_Tofino)0)->pipes[p].mau[s]);    }


    static volatile uint32_t *addr_imem32(int p, int s, int instrNum, int relPhvNum) {
      return &ref_mau(p,s).dp.imem.imem_subword32[relPhvNum][instrNum];
    }
    static volatile uint32_t *addr_imem8(int p, int s, int instrNum, int relPhvNum) {
      return &ref_mau(p,s).dp.imem.imem_subword8[relPhvNum][instrNum];
    }
    static volatile uint32_t *addr_imem16(int p, int s, int instrNum, int relPhvNum) {
      return &ref_mau(p,s).dp.imem.imem_subword16[relPhvNum][instrNum];
    }
    // Semifore generates dumb names for these for some reason so put in aliases
    static void do_setp_imem_subword32_instr(uint32_t *addr, uint32_t val) {
      setp_imem_subword32_imem_subword32_instr(addr, val);
    }
    static void do_setp_imem_subword32_color(uint32_t *addr, uint32_t val) {
      setp_imem_subword32_imem_subword32_color(addr, val);
    }
    static void do_setp_imem_subword32_parity(uint32_t *addr, uint32_t val) {
      setp_imem_subword32_imem_subword32_parity(addr, val);
    }
    static void do_setp_imem_subword8_instr(uint32_t *addr, uint32_t val) {
      setp_imem_subword8_imem_subword8_instr(addr, val);
    }
    static void do_setp_imem_subword8_color(uint32_t *addr, uint32_t val) {
      setp_imem_subword8_imem_subword8_color(addr, val);
    }
    static void do_setp_imem_subword8_parity(uint32_t *addr, uint32_t val) {
      setp_imem_subword8_imem_subword8_parity(addr, val);
    }
    static void do_setp_imem_subword16_instr(uint32_t *addr, uint32_t val) {
      setp_imem_subword16_imem_subword16_instr(addr, val);
    }
    static void do_setp_imem_subword16_color(uint32_t *addr, uint32_t val) {
      setp_imem_subword16_imem_subword16_color(addr, val);
    }
    static void do_setp_imem_subword16_parity(uint32_t *addr, uint32_t val) {
      setp_imem_subword16_imem_subword16_parity(addr, val);
    }
    // And these
    static void do_setp_tcam_row_output_ctl_enabled_4bit_muxctl_enable(uint32_t *addr, uint32_t val) {
      setp_tcam_row_output_ctl_enabled_4bit_muxctl_enable(addr, val);
    }
    static void do_setp_tcam_row_output_ctl_enabled_4bit_muxctl_select(uint32_t *addr, uint32_t val) {
      setp_tcam_row_output_ctl_enabled_4bit_muxctl_select(addr, val);
    }
    static void do_setp_tcam_row_halfbyte_mux_ctl_tcam_row_search_thread(uint32_t *addr, uint32_t val) {
      setp_tcam_row_halfbyte_mux_ctl_tcam_row_search_thread(addr, val);
    }
    static void do_setp_tcam_row_halfbyte_mux_ctl_tcam_row_halfbyte_mux_ctl_enable(uint32_t *addr, uint32_t val) {
      setp_tcam_row_halfbyte_mux_ctl_tcam_row_halfbyte_mux_ctl_enable(addr, val);
    }
    static void do_setp_tcam_row_halfbyte_mux_ctl_tcam_row_halfbyte_mux_ctl_select(uint32_t *addr, uint32_t val) {
      setp_tcam_row_halfbyte_mux_ctl_tcam_row_halfbyte_mux_ctl_select(addr, val);
    }



    // SNAPSHOT helpers
    static volatile uint32_t *addr_snapshot_capture32_hi(int p, int s, int relPhvNum) {
      return &ref_mau(p,s).dp.snapshot_dp.snapshot_capture[relPhvNum/32].
          mau_snapshot_capture_subword32b_hi[relPhvNum%32];
    }
    static volatile uint32_t *addr_snapshot_capture32_lo(int p, int s, int relPhvNum) {
      return &ref_mau(p,s).dp.snapshot_dp.snapshot_capture[relPhvNum/32].
          mau_snapshot_capture_subword32b_lo[relPhvNum%32];
    }
    static volatile uint32_t *addr_snapshot_capture16(int p, int s, int relPhvNum) {
      return &ref_mau(p,s).dp.snapshot_dp.snapshot_capture[relPhvNum/48].
          mau_snapshot_capture_subword16b[relPhvNum%48];
    }
    static volatile uint32_t *addr_snapshot_capture8(int p, int s, int relPhvNum) {
      return &ref_mau(p,s).dp.snapshot_dp.snapshot_capture[relPhvNum/32].
          mau_snapshot_capture_subword8b[relPhvNum%32];
    }


    static auto resub_cfg_reg(int pipe) -> decltype( &addr_dprsr(pipe)->inp.iir.ingr.resub_cfg ) {
      return &addr_dprsr(pipe)->inp.iir.ingr.resub_cfg;
    }

    // These 2 funcs to allow wrapper DPI code to be generic
    static uint32_t dpi_addr_mau_first(int p, int s) {
      if ((p < 0) || (p >= kPipesMax) || (s < 0) || (s >= kStagesMax)) return 0xFFFFFFFF;
      return tofino_pipes_mau_byte_address +
          ( ((uint32_t)(p & kPipeMask)) * tofino_pipes_array_element_size) +
          ( ((uint32_t)(s & kStageMask)) * tofino_pipes_mau_array_element_size);
    }
    static uint32_t dpi_addr_mau_last(int p, int s) {
      if ((p < 0) || (p >= kPipesMax) || (s < 0) || (s >= kStagesMax)) return 0xFFFFFFFF;
      return tofino_pipes_mau_byte_address +
          ( ((uint32_t)(p & kPipeMask)) * tofino_pipes_array_element_size) +
          ( ((uint32_t)(s & kStageMask)) * tofino_pipes_mau_array_element_size) +
          tofino_pipes_mau_array_element_size - 4;
    }

    static constexpr uint32_t vmac_c3_base_addr   = static_cast<uint32_t>(tofino_macs_byte_address);
    static constexpr uint32_t vmac_c3_stride      = static_cast<uint32_t>(tofino_macs_array_element_size);
    static constexpr uint32_t vmac_c3_cntr_offset =     0x8000u;
    static constexpr uint32_t vmac_c3_cntr_size   =         28u; // Counter access via 7 32b CSRs
    static constexpr uint32_t vmac_c4_base_addr   = 0xFFFFFFFFu; // No Comira4 on Tofino
    static constexpr uint32_t vmac_c4_stride      = 0xFFFFFFFFu; // No Comira4 on Tofino
    static constexpr uint32_t vmac_c4_cntr_offset = 0xFFFFFFFFu; // No Comira4 on Tofino
    static constexpr uint32_t vmac_c4_cntr_size   = 0xFFFFFFFFu; // No Comira4 on Tofino

    static int addr_decode(uint32_t addr, int *index) {
      int ret = -1;
      uint32_t pipe_tot_size = tofino_pipes_array_count * tofino_pipes_array_element_size;
      static_assert(tofino_device_select_byte_address == 0, "Expected tofino_device_select_byte_address to be 0");
      /* addr is unsigned and tofino_device_select_byte_address is zero
      if (addr < tofino_device_select_byte_address) {
        ret = REG_LO;
      } else */ if (addr < tofino_serdes_byte_address) {
        ret = REG_DEVSEL;
        if (addr == tofino_device_select_misc_regs_soft_reset_address) {
          ret = REG_DEVSEL_SOFT_RESET;
        }
        if (addr >= tofino_device_select_misc_regs_func_fuse_byte_address) {
          uint32_t fuse_cnt = tofino_device_select_misc_regs_func_fuse_array_count;
          uint32_t fuse_size = tofino_device_select_misc_regs_func_fuse_array_element_size;
          uint32_t fuse_tot_size = fuse_cnt * fuse_size;
          if (addr < tofino_device_select_misc_regs_func_fuse_byte_address + fuse_tot_size) {
            ret = REG_DEVSEL_FUSE;
            uint32_t fuse_base = tofino_device_select_misc_regs_func_fuse_byte_address;
            if (index != NULL) *index = (addr - fuse_base) / fuse_size;
          }
        }
      } else if (addr < tofino_macs_byte_address) {
        ret = REG_SERDES;
      } else if (addr < tofino_ethgpiobr_byte_address) {
        // Comira Umac3 - one of 65 - figure out offset within
        uint32_t offset = (addr - vmac_c3_base_addr) % vmac_c3_stride;
        uint32_t cntr_start = vmac_c3_cntr_offset;
        uint32_t cntr_end   = vmac_c3_cntr_offset + vmac_c3_cntr_size;
        ret = ((offset >= cntr_start) && (offset <= cntr_end)) ?REG_MACS_CNTRS :REG_MACS_OTHER;
      } else if (addr < tofino_pipes_byte_address) {
        ret = REG_ETHS;
      } else if (addr < tofino_pipes_byte_address + pipe_tot_size) {
        ret = REG_PIPES;
      } else {
        ret = REG_HI;
      }
      return ret;
    }

  };

};

#endif
