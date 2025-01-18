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

#ifndef _SHARED_MAU_SRAM_ROW_REG_
#define _SHARED_MAU_SRAM_ROW_REG_

#include <cstdint>
#include <rmt-defs.h>
#include <mau-defs.h>
#include <mau-object.h>
#include <bitvector.h>
#include <mau-meter-alu.h>
#include <mau-sram-row-chip-reg.h>

// Reg defs auto-generated from Semifore
#include <register_includes/l_oflo_adr_o_mux_select.h>
#include <register_includes/l_oflo2_adr_o_mux_select.h>
#include <register_includes/r_oflo_adr_o_mux_select.h>
#include <register_includes/r_oflo2_adr_o_mux_select.h>
#include <register_includes/b_oflo_adr_o_mux_select.h>
#include <register_includes/b_oflo2dn_adr_o_mux_select.h>
#include <register_includes/t_oflo2up_adr_o_mux_select.h>

#include <register_includes/l_stats_alu_o_mux_select.h>
#include <register_includes/l_meter_alu_o_mux_select.h>
#include <register_includes/l_oflo_wr_o_mux_select.h>
#include <register_includes/l_oflo2_wr_o_mux_select.h>
#include <register_includes/r_action_o_mux_select.h>
#include <register_includes/r_l_action_o_mux_select.h>
#include <register_includes/r_stats_alu_o_mux_select.h>
#include <register_includes/r_meter_alu_o_mux_select.h>
#include <register_includes/r_oflo_wr_o_mux_select.h>
#include <register_includes/r_oflo2_wr_o_mux_select.h>
#include <register_includes/b_oflo_wr_o_mux_select.h>
#include <register_includes/b_oflo2dn_wr_o_mux_select.h>
#include <register_includes/b_oflo2up_rd_o_mux_select.h>
#include <register_includes/t_oflo_rd_o_mux_select.h>
#include <register_includes/t_oflo2dn_rd_o_mux_select.h>
#include <register_includes/t_oflo2up_wr_o_mux_select.h>
#include <register_includes/r_oflo_color_write_o_mux_select.h>
#include <register_includes/b_oflo_color_write_o_mux_select.h>

#include <register_includes/mau_selector_action_adr_shift_array.h>
#include <register_includes/action_hv_xbar_disable_ram_adr.h>
#include <register_includes/synth2port_fabric_ctl_array2.h>
#include <register_includes/synth2port_hbus_members_array2.h>
#include <register_includes/synth2port_ctl.h>


namespace MODEL_CHIP_NAMESPACE {

  class MauSramRow;
  class MauLogicalRow;

  class MauSramRowReg : public MauObject {

    static constexpr int kType = RmtTypes::kRmtTypeMauSramRowReg;
    static constexpr int kDataBusWidth = MauDefs::kDataBusWidth;
    static constexpr int kRowsPerMau = MauDefs::kSramRowsPerMau;
    static constexpr int kRowAboveMiddle = (kRowsPerMau+1)/2; // Handle odd
    static constexpr int kRowBelowMiddle = kRowAboveMiddle-1;
    static constexpr int kRowAtTop = kRowsPerMau-1;
    static constexpr int kRowAtBottom = 0;
    static constexpr int kMapramColumns = MauDefs::kMapramColumnsPerMau;
    static_assert( (kMapramColumns <= 16), "MapramColMask must fit in uint16_t" );



    static constexpr int kMuxOpcodeNOP       = 0x0;
    static constexpr int kMuxOpcodeFetchAddr = 0x1;
    static constexpr int kMuxOpcodePushALU   = 0x2;

 public:
    static bool kMemoryCoreSplit;            // Defined in rmt-config.cpp
    static bool kRelaxOfloWrMuxCheck;        // Defined in rmt-config.cpp
    static bool kRelaxOfloRdMuxCheck;        // Defined in rmt-config.cpp
    static bool kRelaxSynth2PortFabricCheck; // Defined in rmt-config.cpp

    static inline int make_op(int opcode, uint8_t args0, uint8_t args1=0) {
      return ((opcode & 0xFF) << 24) | (args1 << 8) | (args0 << 0);
    }
    static inline int get_opcode(int op)    { return (op >> 24) & 0xFF; }
    static inline uint8_t get_args0(int op) { return static_cast<uint8_t>((op >> 0) & 0xFF); }
    static inline uint8_t get_args1(int op) { return static_cast<uint8_t>((op >> 8) & 0xFF); }

    static inline int make_op_fetch(uint8_t addrtype) {
      return make_op(kMuxOpcodeFetchAddr, addrtype);
    }
    static inline int make_op_push(uint8_t alutype, uint8_t alulogrow) {
      return make_op(kMuxOpcodePushALU, alutype, alulogrow);
    }
    static inline uint8_t get_addrtype(int op) {
      if (get_opcode(op) != kMuxOpcodeFetchAddr) return 0;
      return static_cast<uint8_t>(get_args0(op));
    }
    static inline uint8_t get_alutype(int op) {
      if (get_opcode(op) != kMuxOpcodePushALU) return 0;
      return static_cast<uint8_t>(get_args0(op));
    }
    static inline uint8_t get_alulogrow(int op) {
      if (get_opcode(op) != kMuxOpcodePushALU) return 0;
      return static_cast<uint8_t>(get_args1(op));
    }


 public:
    MauSramRowReg(RmtObjectManager *om, int pipeIndex, int mauIndex,
                  int rowIndex, MauSramRow *mauSramRow);
    virtual ~MauSramRowReg();

    inline bool row_odd()          const { return (row_index_ % 2) == 1; }
    inline bool row_above_middle() const { return (row_index_ == kRowAboveMiddle); }
    inline bool row_below_middle() const { return (row_index_ == kRowBelowMiddle); }
    inline bool row_at_top()       const { return (row_index_ == kRowAtTop); }
    inline bool row_at_bottom()    const { return (row_index_ == kRowAtBottom); }

    void set_row_above(MauSramRow *a);
    void set_row_below(MauSramRow *b);
    void set_logrow_left(MauLogicalRow *l);
    void set_logrow_right(MauLogicalRow *r);


    // SELECTOR READ address shifts - now have proper fields
    inline int selector_adr_shift_l() {
      //return (selector_action_ram_shift_.mau_selector_action_adr_shift() >> 6) & 0x7;
      return selector_action_ram_shift_.mau_selector_action_adr_shift_left();
    }
    inline int selector_adr_shift_r() {
      //return (selector_action_ram_shift_.mau_selector_action_adr_shift() >> 15) & 0x7;
      return selector_action_ram_shift_.mau_selector_action_adr_shift_right();
    }
    inline int selector_oflo_adr_shift_l() {
      //return (selector_action_ram_shift_.mau_selector_action_adr_shift() >> 0) & 0x7;
      return selector_action_ram_shift_.mau_selector_action_adr_shift_left_oflo();
    }
    inline int selector_oflo_adr_shift_r() {
      //return (selector_action_ram_shift_.mau_selector_action_adr_shift() >> 9) & 0x7;
      return selector_action_ram_shift_.mau_selector_action_adr_shift_right_oflo();
    }
    inline int selector_oflo2_adr_shift_l() {
      //return (selector_action_ram_shift_.mau_selector_action_adr_shift() >> 3) & 0x7;
      return selector_action_ram_shift_.mau_selector_action_adr_shift_left_oflo2();
    }
    inline int selector_oflo2_adr_shift_r() {
      //return (selector_action_ram_shift_.mau_selector_action_adr_shift() >> 12) & 0x7;
      return selector_action_ram_shift_.mau_selector_action_adr_shift_right_oflo2();
    }

    // DOES physical row produce a selector address
    inline bool produces_sel_addr() {
      // SelectorALU (one of MeterALU group) only on RHS - ie ODD logical rows
      // So only need to check the right hand _r mux config to see if
      // any *other* row is using the selector_addr from *this* physical row
      return ((l_oflo_adr_o_mux_select_.l_oflo_adr_o_sel_selector_adr_r_i() != 0) ||
              // Oflo2 NOT used so can ignore these oflo2 muxes
              //
              //(l_oflo2_adr_o_mux_select_.l_oflo2_adr_o_sel_selector_adr_r_i()     != 0) ||
              //(b_oflo2dn_adr_o_mux_select_.b_oflo2dn_adr_o_sel_selector_adr_r_i() != 0) ||
              //(t_oflo2up_adr_o_mux_select_.t_oflo2up_adr_o_sel_selector_adr_r_i() != 0) ||
              //
              (b_oflo_adr_o_mux_select_.b_oflo_adr_o_sel_selector_adr_r_i() != 0));
    }

    // SELECTOR READ addresses
    void selector_adr_l(uint32_t *addr);
    void selector_adr_r(uint32_t *addr);
    void oflo_adr_t(uint32_t *addr);
    void oflo2dn_adr_t(uint32_t *addr);
    void oflo2up_adr_b(uint32_t *addr);

    void l_oflo_adr(uint32_t *addr);
    void l_oflo2_adr(uint32_t *addr);
    void r_oflo_adr(uint32_t *addr);
    void r_oflo2_adr(uint32_t *addr);
    void b_oflo_adr(uint32_t *addr);
    void b_oflo2dn_adr(uint32_t *addr);
    void t_oflo2up_adr(uint32_t *addr);

    // WRITE waddresses
    void l_stats_wadr(uint32_t *waddr);
    void r_stats_wadr(uint32_t *waddr);
    void l_meter_wadr(uint32_t *waddr);
    void r_meter_wadr(uint32_t *waddr);
    void t_oflo_wadr(uint32_t *waddr);
    void t_oflo2dn_wadr(uint32_t *waddr);
    void b_oflo2up_wadr(uint32_t *waddr);

    void l_oflo_wadr(uint32_t *waddr);
    void l_oflo2_wadr(uint32_t *waddr);
    void r_oflo_wadr(uint32_t *waddr);
    void r_oflo2_wadr(uint32_t *waddr);
    void b_oflo_wadr(uint32_t *waddr);
    void b_oflo2dn_wadr(uint32_t *waddr);
    void t_oflo2up_wadr(uint32_t *waddr);

    // READ/WRITE data
    void action_rd_l(BitVector<kDataBusWidth> *data, uint32_t *addr, int op);
    void action_rd_r(BitVector<kDataBusWidth> *data, uint32_t *addr, int op);
    void stats_rd_l(BitVector<kDataBusWidth> *data, uint32_t *addr, int op);
    void stats_rd_r(BitVector<kDataBusWidth> *data, uint32_t *addr, int op);
    void stats_wr_l(BitVector<kDataBusWidth> *data, uint32_t *addr, int op);
    void stats_wr_r(BitVector<kDataBusWidth> *data, uint32_t *addr, int op);
    void meter_rd_l(BitVector<kDataBusWidth> *data, uint32_t *addr, int op);
    void meter_rd_r(BitVector<kDataBusWidth> *data, uint32_t *addr, int op);
    void meter_wr_l(BitVector<kDataBusWidth> *data, uint32_t *addr, int op);
    void meter_wr_r(BitVector<kDataBusWidth> *data, uint32_t *addr, int op);
    void oflo_rd_l(BitVector<kDataBusWidth> *data, uint32_t *addr, int op);
    void oflo_rd_r(BitVector<kDataBusWidth> *data, uint32_t *addr, int op);
    void oflo2_rd_l(BitVector<kDataBusWidth> *data, uint32_t *addr, int op);
    void oflo2_rd_r(BitVector<kDataBusWidth> *data, uint32_t *addr, int op);

    void oflo2dn_rd_b(BitVector<kDataBusWidth> *data, uint32_t *addr, int op);
    void oflo2dn_wr_t(BitVector<kDataBusWidth> *data, uint32_t *addr, int op);
    void oflo2up_rd_t(BitVector<kDataBusWidth> *data, uint32_t *addr, int op);
    void oflo2up_wr_b(BitVector<kDataBusWidth> *data, uint32_t *addr, int op);
    void oflo_rd_b(BitVector<kDataBusWidth> *data, uint32_t *addr, int op);
    void oflo_wr_t(BitVector<kDataBusWidth> *data, uint32_t *addr, int op);

    void l_stats_alu(BitVector<kDataBusWidth> *data, uint32_t *addr, int op=0);
    void l_meter_alu(BitVector<kDataBusWidth> *data, uint32_t *addr, int op=0);
    void l_oflo_wr(BitVector<kDataBusWidth> *data, uint32_t *addr, int op);
    void l_oflo2_wr(BitVector<kDataBusWidth> *data, uint32_t *addr, int op);
    void r_action(BitVector<kDataBusWidth> *data, uint32_t *addr, int op=0);
    void r_l_action(BitVector<kDataBusWidth> *data, uint32_t *addr, int op=0);
    void r_stats_alu(BitVector<kDataBusWidth> *data, uint32_t *addr, int op=0);
    void r_meter_alu(BitVector<kDataBusWidth> *data, uint32_t *addr, int op=0);
    void r_oflo_wr(BitVector<kDataBusWidth> *data, uint32_t *addr, int op);
    void r_oflo2_wr(BitVector<kDataBusWidth> *data, uint32_t *addr, int op);
    void b_oflo_wr(BitVector<kDataBusWidth> *data, uint32_t *addr, int op);
    void b_oflo2dn_wr(BitVector<kDataBusWidth> *data, uint32_t *addr, int op);
    void b_oflo2up_rd(BitVector<kDataBusWidth> *data, uint32_t *addr, int op);
    void t_oflo_rd(BitVector<kDataBusWidth> *data, uint32_t *addr, int op);
    void t_oflo2dn_rd(BitVector<kDataBusWidth> *data, uint32_t *addr, int op);
    void t_oflo2up_wr(BitVector<kDataBusWidth> *data, uint32_t *addr, int op);

    void b_oflo_wr_o_mux_write_cb();
    void t_oflo_rd_o_mux_write_cb();

    void b_oflo_color_write(uint8_t* color);
    void r_oflo_color_write(uint8_t* color);

    uint32_t b_color_alu_rows();
    uint32_t get_color_alu_rows();
    int      get_color_alu_logrow_index();
    int      get_color_alu_index();

    bool action_hv_xbar_disable_ram_adr(bool right_hand_side) {
      //WAS return 1 & (action_hv_xbar_disable_ram_adr_.action_hv_xbar_disable_ram_adr() >> ( right_hand_side?1:0 ));
      if (right_hand_side)
        return (action_hv_xbar_disable_ram_adr_.action_hv_xbar_disable_ram_adr_right() == 1);
      else
        return (action_hv_xbar_disable_ram_adr_.action_hv_xbar_disable_ram_adr_left() == 1);
    }

    bool synth2port_vpn_valid(int vpn);
    bool synth2port_fabric_check(bool is_stats,int alu, bool is_home_row, bool is_bottom, uint32_t ram_columns);

    bool synth2port_ctl_check();
    void mapram_config_update();
    void mapram_change_callback(int col);


 private:
    MauSramRowChipReg                             mau_sram_row_chip_reg_;
    MauSramRow                                   *mau_sram_row_;
    MauSramRow                                   *mau_sram_row_above_;
    MauSramRow                                   *mau_sram_row_below_;
    MauSramRowReg                                *regs_above_;
    MauSramRowReg                                *regs_below_;
    MauLogicalRow                                *mau_logical_row_left_;
    MauLogicalRow                                *mau_logical_row_right_;
    int                                           row_index_;
    uint32_t                                      curr_seq_;
    uint32_t                                      pending_seq_;
    uint16_t                                      synth2port_home_mapram_mask_;
    uint16_t                                      synth2port_oflo_mapram_mask_;
    uint16_t                                      color_home_mapram_mask_;
    uint16_t                                      color_oflo_mapram_mask_;

    register_classes::LOfloAdrOMuxSelect          l_oflo_adr_o_mux_select_;
    register_classes::LOflo2AdrOMuxSelect         l_oflo2_adr_o_mux_select_;
    register_classes::ROfloAdrOMuxSelect          r_oflo_adr_o_mux_select_;
    register_classes::ROflo2AdrOMuxSelect         r_oflo2_adr_o_mux_select_;
    register_classes::BOfloAdrOMuxSelect          b_oflo_adr_o_mux_select_;
    register_classes::BOflo2dnAdrOMuxSelect       b_oflo2dn_adr_o_mux_select_;
    register_classes::TOflo2upAdrOMuxSelect       t_oflo2up_adr_o_mux_select_;

    register_classes::LStatsAluOMuxSelect         l_stats_alu_o_mux_select_;
    register_classes::LMeterAluOMuxSelect         l_meter_alu_o_mux_select_;
    register_classes::LOfloWrOMuxSelect           l_oflo_wr_o_mux_select_;
    register_classes::LOflo2WrOMuxSelect          l_oflo2_wr_o_mux_select_;
    register_classes::RActionOMuxSelect           r_action_o_mux_select_;
    register_classes::RLActionOMuxSelect          r_l_action_o_mux_select_;
    register_classes::RStatsAluOMuxSelect         r_stats_alu_o_mux_select_;
    register_classes::RMeterAluOMuxSelect         r_meter_alu_o_mux_select_;
    register_classes::ROfloWrOMuxSelect           r_oflo_wr_o_mux_select_;
    register_classes::ROflo2WrOMuxSelect          r_oflo2_wr_o_mux_select_;
    register_classes::BOfloWrOMuxSelect           b_oflo_wr_o_mux_select_;
    register_classes::BOflo2dnWrOMuxSelect        b_oflo2dn_wr_o_mux_select_;
    register_classes::BOflo2upRdOMuxSelect        b_oflo2up_rd_o_mux_select_;
    register_classes::TOfloRdOMuxSelect           t_oflo_rd_o_mux_select_;
    register_classes::TOflo2dnRdOMuxSelect        t_oflo2dn_rd_o_mux_select_;
    register_classes::TOflo2upWrOMuxSelect        t_oflo2up_wr_o_mux_select_;

    register_classes::ActionHvXbarDisableRamAdr   action_hv_xbar_disable_ram_adr_;

    register_classes::ROfloColorWriteOMuxSelect   r_oflo_color_write_o_mux_select_;
    register_classes::BOfloColorWriteOMuxSelect   b_oflo_color_write_o_mux_select_;
    bool                                          row_has_color_write_switch_box_;

    // Depending on number of entries per action ram word (16x8b, 8x16b, 4x32b .. 1x128b)
    // this register specifies shift count
    register_classes::MauSelectorActionAdrShift   selector_action_ram_shift_;

    register_classes::Synth2portFabricCtlArray2   synth2port_fabric_ctl_;
    register_classes::Synth2portHbusMembersArray2 synth2port_hbus_members_;
    register_classes::Synth2portCtl               synth2port_ctl_;


    bool row_has_color_write_switch_box(int phys_row) {
      RMT_ASSERT( phys_row>=0 && phys_row<8 );
      // this code assumes that meters are on the right only, ie odd logical rows only
      int logrow = (phys_row*2) + 1;
      return   (1<<logrow) & MauDefs::kMeterAluLogicalRows;
     }

    // There are only color_write switch boxes on rows with meter ALUs
    int row_to_color_write_switch_box(int phys_row) {
      RMT_ASSERT( phys_row>=0 && phys_row<8 );
      if ( row_has_color_write_switch_box(phys_row) ) {
        // this code assumes that meters are on the right only, ie odd logical rows only
        int logrow = (phys_row*2) + 1;
        return MauMeterAlu::get_meter_alu_regs_index(logrow);
      }
      else {
        return 0; // should not be used, so just use registers for first row
      }
    }

    bool synth2port_fabric_check_static();

    enum Synth2PortRowUsedAs {
      kHomeRowNoOverflow,
      kHomeRowWithOverflow,
      kBottomOnly,
      kMiddleWithRams,
      kMiddleNoRams,
      kNone
    };

    struct Synth2PortUsedByAlu {
      bool used_ = false;
      int  alu_=0;
      Synth2PortRowUsedAs used_as_=kNone;
    };

    // synth2 port fabric can be used by up to 2 alus
    std::array< Synth2PortUsedByAlu, 2 > synth2port_used_by_alu_;
    bool synth2port_check_other_alu(bool is_stats, int alu, Synth2PortRowUsedAs this_alu_used_as, std::vector< Synth2PortRowUsedAs > other_alu_can_use_as );
    bool check_hbus_members( bool is_home_row, uint32_t ram_columns );
    bool check_data_muxes( bool is_stats, bool is_home_row, bool has_rams );


  };
}
#endif // _SHARED_MAU_SRAM_ROW_REG_
