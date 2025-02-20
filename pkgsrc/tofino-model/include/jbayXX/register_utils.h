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

#ifndef _JBAYXX_REGISTER_UTILS_
#define _JBAYXX_REGISTER_UTILS_

#include <cstddef>
#include <model_core/register_categories.h>
#include <mcn_test.h>
#include <rmt-defs.h>
#include <register_includes/reg.h>


#if MCN_TEST(MODEL_CHIP_NAMESPACE, jbay) || MCN_TEST(MODEL_CHIP_NAMESPACE, jbayB0)
#define BFN_REG_TOP(y)    BFN_CONCAT(jbay_reg_,y)
#define BFN_PTR_CHIP_REG  PTR_Jbay_reg
#define BFN_CHIP_REG      Jbay_reg
#define BFN_CHIP_REG_CONCAT(y) BFN_CONCAT(Jbay_reg_,y)
#else
static_assert(false, "Unexpected MODEL_CHIP_NAMESPACE");
#endif

namespace MODEL_CHIP_NAMESPACE {


class RegisterUtils {
 public:
    static constexpr int  kPipesMax = RmtDefs::kPipesMax;
    static constexpr int  kPipeMask = RmtDefs::kPipeMask;
    static constexpr int  kStagesMax = RmtDefs::kStagesMax;
    static constexpr int  kStageMask = RmtDefs::kStageMask;
    static constexpr int  kPipeStride = static_cast<int>( BFN_REG_TOP(pipes_array_element_size) );


    // Used in tm-regs.h to make 2 blocks of fake registers: one from the start of TM to the start of PRE,
    //  and one after the end of PRE up to the end of TM.
    static constexpr uint32_t kTmDefaultRegVal = 0xFFFFFFFF;
    static constexpr size_t kTmStartAddress  = BFN_REG_TOP(device_select_tm_top_address);
    static constexpr size_t kPreStartAddress = BFN_REG_TOP(device_select_tm_top_tm_pre_top_address);
    static constexpr size_t kPscStartAddress = BFN_REG_TOP(device_select_tm_top_tm_psc_top_address);
    static constexpr size_t kTmLastAddress   = BFN_REG_TOP(device_select_tm_top_tm_psc_top_psc_common_underflow_err_log_address);

    static const size_t     offset_tm_pre_common_blk() { return BFN_REG_TOP(device_select_tm_top_tm_pre_top_pre_common_blk_id_address); }
    static int              rdm_blk_word_off(int rb)   { return rb/16; }
    static int              rdm_blk_bit_off(int rb)    { return 2*(rb%16); } // PipeId is 2b

    static Dvsl_addrmap*      addr_devsel()            { return &(((BFN_PTR_CHIP_REG)0)->device_select);     }
    static Misc_regs*         addr_misc_regs()         { return &addr_devsel()->misc_regs; }
    static volatile uint32_t *addr_misc_regs_dbg_rst() { return &addr_misc_regs()->dbg_rst; }
    static Lfltr_rspec*       addr_lfltr(int p)        {
      return &addr_devsel()->lfltr[p];
    }
    static auto addr_pcie_regs() -> decltype( &addr_devsel()->pcie_bar01_regs )  {
      return &addr_devsel()->pcie_bar01_regs;
    }

    // TODO: these two will need to take the slice number?
    static auto addr_pmarb(int p) -> decltype( &(((BFN_PTR_CHIP_REG)0)->pipes[p].pardereg.pgstnreg) )  {
      return &(((BFN_PTR_CHIP_REG)0)->pipes[p].pardereg.pgstnreg);
    }
    static auto addr_dprsr(int p) -> decltype( &(((BFN_PTR_CHIP_REG)0)->pipes[p].pardereg.dprsrreg.dprsrreg) )  {
      return &(((BFN_PTR_CHIP_REG)0)->pipes[p].pardereg.dprsrreg.dprsrreg);
    }
    static auto addr_ingbuf(int p, int ipb) -> decltype( &addr_pmarb(p)->ipbprsr4reg[ipb].ipbreg ) {
      return &addr_pmarb(p)->ipbprsr4reg[ipb].ipbreg;
    }
    // No more eop_port_regs as of regs_43324_parde_jbay
    //static auto addr_egrbuf(int p, int epb) -> decltype( &addr_pmarb(p)->epbprsr4reg[epb].epbreg.eop_port_regs ) {
    //  return &addr_pmarb(p)->epbprsr4reg[epb].epbreg.eop_port_regs;
    //}
    static auto addr_egrbuf(int p, int epb) -> decltype( &addr_pmarb(p)->epbprsr4reg[epb].epbreg ) {
      return &addr_pmarb(p)->epbprsr4reg[epb].epbreg;
    }
    static auto addr_mirbuf(int p) -> decltype( &(((BFN_PTR_CHIP_REG)0)->pipes[p].pardereg.mirreg) )  {
      return &(((BFN_PTR_CHIP_REG)0)->pipes[p].pardereg.mirreg);
    }
    static auto addr_pgr(int p) -> decltype( &addr_pmarb(p)->pgrreg ) {
      return &addr_pmarb(p)->pgrreg;
    }
    static auto addr_s2p(int p) -> decltype( &addr_pmarb(p)->s2preg ) {
      return &addr_pmarb(p)->s2preg;
    }
    static auto addr_p2s(int p) ->decltype(&addr_pmarb(p)->p2sreg) {
      return &addr_pmarb(p)->p2sreg;
    }

    static Mau_addrmap* addr_mau(int p,int s) { return &(((BFN_PTR_CHIP_REG)0)->pipes[p].mau[s]);   }

    static Mau_addrmap& ref_mau(int p,int s)  { return (((BFN_PTR_CHIP_REG)0)->pipes[p].mau[s]);    }

    // For Parde tests OutWords. Usage:   auto& BFN_CHIP_REG = RegisterUtils::ref_jbay();
    static BFN_CHIP_REG& ref_jbay()  {
      typedef struct {  BFN_CHIP_REG jbay; } *PTR_Dummy;
      return ((PTR_Dummy)0)->jbay;
    }

    // IMEM helpers
    static volatile uint32_t *addr_imem32(int p, int s, int instrNum, int relPhvNum) {
      switch (relPhvNum%20) {
        case 19: case 18: case 17: case 16:
          return &ref_mau(p,s).dp.imem.imem_dark_subword32[relPhvNum/40][(relPhvNum/20)%2][(relPhvNum%20)-16][instrNum];
        case 15: case 14: case 13: case 12:
          return &ref_mau(p,s).dp.imem.imem_mocha_subword32[relPhvNum/40][(relPhvNum/20)%2][(relPhvNum%20)-12][instrNum];
        default:
          return &ref_mau(p,s).dp.imem.imem_subword32[relPhvNum/40][(relPhvNum/20)%2][(relPhvNum%20)-0][instrNum];
      }
    }
    static volatile uint32_t *addr_imem8(int p, int s, int instrNum, int relPhvNum) {
      switch (relPhvNum%20) {
        case 19: case 18: case 17: case 16:
          return &ref_mau(p,s).dp.imem.imem_dark_subword8[relPhvNum/40][(relPhvNum/20)%2][(relPhvNum%20)-16][instrNum];
        case 15: case 14: case 13: case 12:
          return &ref_mau(p,s).dp.imem.imem_mocha_subword8[relPhvNum/40][(relPhvNum/20)%2][(relPhvNum%20)-12][instrNum];
        default:
          return &ref_mau(p,s).dp.imem.imem_subword8[relPhvNum/40][(relPhvNum/20)%2][(relPhvNum%20)-0][instrNum];
      }
    }
    static volatile uint32_t *addr_imem16(int p, int s, int instrNum, int relPhvNum) {
      switch (relPhvNum%20) {
        case 19: case 18: case 17: case 16:
          return &ref_mau(p,s).dp.imem.imem_dark_subword16[relPhvNum/60][(relPhvNum/20)%3][(relPhvNum%20)-16][instrNum];
        case 15: case 14: case 13: case 12:
          return &ref_mau(p,s).dp.imem.imem_mocha_subword16[relPhvNum/60][(relPhvNum/20)%3][(relPhvNum%20)-12][instrNum];
        default:
          return &ref_mau(p,s).dp.imem.imem_subword16[relPhvNum/60][(relPhvNum/20)%3][(relPhvNum%20)-0][instrNum];
      }
    }
    // Semifore generates dumb names for these for some reason so put in aliases
    static void do_setp_imem_subword32_instr(uint32_t *addr, uint32_t val) {
      setp_mau_imem_addrmap_imem_subword32_imem_subword32_instr((void*)addr, val);
    }
    static void do_setp_imem_subword32_color(uint32_t *addr, uint32_t val) {
      setp_mau_imem_addrmap_imem_subword32_imem_subword32_color((void*)addr, val);
    }
    static void do_setp_imem_subword32_parity(uint32_t *addr, uint32_t val) {
      setp_mau_imem_addrmap_imem_subword32_imem_subword32_parity((void*)addr, val);
    }
    static void do_setp_imem_subword8_instr(uint32_t *addr, uint32_t val) {
      setp_mau_imem_addrmap_imem_subword8_imem_subword8_instr((void*)addr, val);
    }
    static void do_setp_imem_subword8_color(uint32_t *addr, uint32_t val) {
      setp_mau_imem_addrmap_imem_subword8_imem_subword8_color((void*)addr, val);
    }
    static void do_setp_imem_subword8_parity(uint32_t *addr, uint32_t val) {
      setp_mau_imem_addrmap_imem_subword8_imem_subword8_parity((void*)addr, val);
    }
    static void do_setp_imem_subword16_instr(uint32_t *addr, uint32_t val) {
      setp_mau_imem_addrmap_imem_subword16_imem_subword16_instr((void*)addr, val);
    }
    static void do_setp_imem_subword16_color(uint32_t *addr, uint32_t val) {
      setp_mau_imem_addrmap_imem_subword16_imem_subword16_color((void*)addr, val);
    }
    static void do_setp_imem_subword16_parity(uint32_t *addr, uint32_t val) {
      setp_mau_imem_addrmap_imem_subword16_imem_subword16_parity((void*)addr, val);
    }
    // And these
    static void do_setp_tcam_row_output_ctl_enabled_4bit_muxctl_enable(uint32_t *addr, uint32_t val) {
      setp_tcam_row_output_ctl_enabled_4bit_muxctl_enable((void*)addr, val);
    }
    static void do_setp_tcam_row_output_ctl_enabled_4bit_muxctl_select(uint32_t *addr, uint32_t val) {
      setp_tcam_row_output_ctl_enabled_4bit_muxctl_select((void*)addr, val);
    }
    static void do_setp_tcam_row_halfbyte_mux_ctl_tcam_row_search_thread(uint32_t *addr, uint32_t val) {
      setp_tcam_row_halfbyte_mux_ctl_tcam_row_search_thread((void*)addr, val);
    }
    static void do_setp_tcam_row_halfbyte_mux_ctl_tcam_row_halfbyte_mux_ctl_enable(uint32_t *addr, uint32_t val) {
      setp_tcam_row_halfbyte_mux_ctl_tcam_row_halfbyte_mux_ctl_enable((void*)addr, val);
    }
    static void do_setp_tcam_row_halfbyte_mux_ctl_tcam_row_halfbyte_mux_ctl_select(uint32_t *addr, uint32_t val) {
      setp_tcam_row_halfbyte_mux_ctl_tcam_row_halfbyte_mux_ctl_select((void*)addr, val);
    }



    // SNAPSHOT helpers
    static volatile uint32_t *addr_snapshot_capture32_hi(int p, int s, int relPhvNum) {
      int half = relPhvNum/40, phv_in_half = relPhvNum%40;
      int alu_grp = phv_in_half/20, phv_in_grp = phv_in_half%20;
      return &ref_mau(p,s).dp.snapshot_dp.snapshot_capture[half].
          mau_snapshot_capture_subword32b_hi[phv_in_grp][alu_grp];
    }
    static volatile uint32_t *addr_snapshot_capture32_lo(int p, int s, int relPhvNum) {
      int half = relPhvNum/40, phv_in_half = relPhvNum%40;
      int alu_grp = phv_in_half/20, phv_in_grp = phv_in_half%20;
      return &ref_mau(p,s).dp.snapshot_dp.snapshot_capture[half].
          mau_snapshot_capture_subword32b_lo[phv_in_grp][alu_grp];
    }
    static volatile uint32_t *addr_snapshot_capture16(int p, int s, int relPhvNum) {
      int half = relPhvNum/60, phv_in_half = relPhvNum%60;
      int alu_grp = phv_in_half/20, phv_in_grp = phv_in_half%20;
      return &ref_mau(p,s).dp.snapshot_dp.snapshot_capture[half].
          mau_snapshot_capture_subword16b[phv_in_grp][alu_grp];
    }
    static volatile uint32_t *addr_snapshot_capture8(int p, int s, int relPhvNum) {
      int half = relPhvNum/40, phv_in_half = relPhvNum%40;
      int alu_grp = phv_in_half/20, phv_in_grp = phv_in_half%20;
      return &ref_mau(p,s).dp.snapshot_dp.snapshot_capture[half].
          mau_snapshot_capture_subword8b[phv_in_grp][alu_grp];
    }


    //TODO:JBAY: No pgen regs yet
    //static auto resub_cfg_reg(int pipe) -> decltype( &addr_dprsr(pipe)->inp.iir.ingr.resub_pgen_tmfc_cfg ) {
    // return &addr_dprsr(pipe)->inp.iir.ingr.resub_pgen_tmfc_cfg;
    //}
    static auto resub_cfg_reg(int pipe) -> decltype( &addr_dprsr(pipe)->inp ) {
      return &addr_dprsr(pipe)->inp;
    }

    // These 2 funcs to allow wrapper DPI code to be generic
    static uint32_t dpi_addr_mau_first(int p, int s) {
      if ((p < 0) || (p >= kPipesMax) || (s < 0) || (s >= kStagesMax)) return 0xFFFFFFFF;
      return BFN_REG_TOP(pipes_mau_byte_address) +
          ( ((uint32_t)(p & kPipeMask)) * BFN_REG_TOP(pipes_array_element_size)) +
          ( ((uint32_t)(s & kStageMask)) * BFN_REG_TOP(pipes_mau_array_element_size));
    }
    static uint32_t dpi_addr_mau_last(int p, int s) {
      if ((p < 0) || (p >= kPipesMax) || (s < 0) || (s >= kStagesMax)) return 0xFFFFFFFF;
      return BFN_REG_TOP(pipes_mau_byte_address) +
          ( ((uint32_t)(p & kPipeMask)) * BFN_REG_TOP(pipes_array_element_size)) +
          ( ((uint32_t)(s & kStageMask)) * BFN_REG_TOP(pipes_mau_array_element_size)) +
          BFN_REG_TOP(pipes_mau_array_element_size) - 4;
    }

    static constexpr uint32_t vmac_c3_base_addr   = static_cast<uint32_t>(BFN_REG_TOP(eth100g_regs_byte_address));
    static constexpr uint32_t vmac_c3_stride      = static_cast<uint32_t>(BFN_REG_TOP(eth400g_p1_byte_address) - BFN_REG_TOP(eth100g_regs_byte_address));
    static constexpr uint32_t vmac_c3_cntr_offset = 0x8000u;
    static constexpr uint32_t vmac_c3_cntr_size   =     28u; // Counter access via 7 32b CSRs
    static constexpr uint32_t vmac_c4_base_addr   = static_cast<uint32_t>(BFN_REG_TOP(eth400g_p1_byte_address));
    static constexpr uint32_t vmac_c4_stride      = static_cast<uint32_t>(BFN_REG_TOP(eth400g_p2_byte_address) - BFN_REG_TOP(eth400g_p1_byte_address));
    static constexpr uint32_t vmac_c4_cntr_offset = 0xC000u;
    static constexpr uint32_t vmac_c4_cntr_size   = 0x2000u; // Direct counter access via address

    static int addr_decode(uint32_t addr, int *index) {
      int ret = -1;
      uint32_t pipe_tot_size = BFN_REG_TOP(pipes_array_count) * BFN_REG_TOP(pipes_array_element_size);
      static_assert(BFN_REG_TOP(device_select_byte_address) == 0, "Expected BFN_REG_TOP(device_select_byte_address to be 0");
      /* addr is unsigned and BFN_REG_TOP(device_select_byte_address is zero - comment out to get rid of warning
      if (addr < BFN_REG_TOP(device_select_byte_address) {
        ret = REG_LO;
      } else */ if (addr < BFN_REG_TOP(eth100g_regs_byte_address)) {
        ret = REG_DEVSEL;
        if (addr == BFN_REG_TOP(device_select_misc_regs_soft_reset_address)) {
          ret = REG_DEVSEL_SOFT_RESET;
        }
        if (addr >= BFN_REG_TOP(device_select_misc_regs_func_fuse_byte_address)) {
          uint32_t fuse_cnt = BFN_REG_TOP(device_select_misc_regs_func_fuse_array_count);
          uint32_t fuse_size = BFN_REG_TOP(device_select_misc_regs_func_fuse_array_element_size);
          uint32_t fuse_tot_size = fuse_cnt * fuse_size;
          if (addr < BFN_REG_TOP(device_select_misc_regs_func_fuse_byte_address) + fuse_tot_size) {
            ret = REG_DEVSEL_FUSE;
            uint32_t fuse_base = BFN_REG_TOP(device_select_misc_regs_func_fuse_byte_address);
            if (index != NULL) *index = (addr - fuse_base) / fuse_size;
          }
        }
      } else if (addr < BFN_REG_TOP(ethgpiobr_byte_address)) {
        if (addr < BFN_REG_TOP(eth400g_p1_byte_address)) {
          // Comira Umac3 - just one - calculate offset within
          uint32_t offset     = (addr - vmac_c3_base_addr) % vmac_c3_stride;
          uint32_t cntr_start = vmac_c3_cntr_offset;
          uint32_t cntr_end   = vmac_c3_cntr_offset + vmac_c3_cntr_size;
          ret = ((offset >= cntr_start) && (offset <= cntr_end)) ?REG_MACS_CNTRS :REG_MACS_OTHER;
        } else {
          // Comira Umac4 - one of 32 - calculate offset within
          uint32_t offset     = (addr - vmac_c4_base_addr) % vmac_c4_stride;
          uint32_t cntr_start = vmac_c4_cntr_offset;
          uint32_t cntr_end   = vmac_c4_cntr_offset + vmac_c4_cntr_size;
          ret = ((offset >= cntr_start) && (offset <= cntr_end)) ?REG_MACS_CNTRS :REG_MACS_OTHER;
        }
      } else if (addr < BFN_REG_TOP(serdes_byte_address)) {
        ret = REG_ETHS;
      } else if (addr < BFN_REG_TOP(pipes_byte_address)) {
        ret = REG_SERDES;
      } else if (addr < BFN_REG_TOP(pipes_byte_address) + pipe_tot_size) {
        ret = REG_PIPES;
      } else {
        ret = REG_HI;
      }
      return ret;
    }

  static constexpr uint32_t pipes_low = static_cast<uint32_t>(BFN_REG_TOP(pipes_address));
  static constexpr uint32_t pipes_high  = pipes_low + static_cast<uint32_t>((BFN_REG_TOP(pipes_array_element_size) * BFN_REG_TOP(pipes_array_count)));

  static bool is_pipe_address(uint32_t address) {
    return ((address >= pipes_low) && (address < pipes_high));
  }

  static int pipe_index_for_address(uint32_t address) {
    int pipe = -1;
    if (is_pipe_address(address)) {
      pipe = (address - pipes_low) / BFN_REG_TOP(pipes_array_element_size);
    }
    return pipe;
  }

  static int mau_index_for_address(uint32_t address) {
    int mau = -1;
    int pipe = pipe_index_for_address(address);
    if (pipe >= 0) {
      uint32_t mau_base = (pipe * BFN_REG_TOP(pipes_array_element_size)) + BFN_REG_TOP(pipes_mau_address);
      mau = (address - mau_base) / BFN_REG_TOP(pipes_mau_array_element_size);
    }
    return mau;
  }

  static bool is_mau_index(int index) {
    return (index >= 0) && (static_cast<uint64_t>(index) < BFN_REG_TOP(pipes_mau_array_count));
  }

};

}
#endif
