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
#include <string>
#include <rmt-log.h>
#include <rmt-object-manager.h>
#include <mau-sram-row-reg.h>
#include <mau-sram-row.h>
#include <register_adapters.h>



namespace MODEL_CHIP_NAMESPACE {
  MauSramRowReg::MauSramRowReg(RmtObjectManager *om, int pipeIndex, int mauIndex,
                               int rowIndex, MauSramRow *mauSramRow)

      : MauObject(om, pipeIndex, mauIndex, kType, rowIndex),
        mau_sram_row_chip_reg_(om, pipeIndex, mauIndex, rowIndex,mauSramRow),
        mau_sram_row_(mauSramRow),
        mau_sram_row_above_(NULL), mau_sram_row_below_(NULL),
        regs_above_(NULL), regs_below_(NULL),
        mau_logical_row_left_(NULL), mau_logical_row_right_(NULL),
        row_index_(rowIndex), curr_seq_(0u), pending_seq_(1u),
        synth2port_home_mapram_mask_(0), synth2port_oflo_mapram_mask_(0),
        color_home_mapram_mask_(0), color_oflo_mapram_mask_(0),
        // Only 4 selector addr switchboxes now (rows 1,3,5,7) but we still
        // instantiate on each row with even row X being a duplicate of odd
        // row X+1. The X_adr() funcs skip even rows calling the appropriate
        // func above (or above/below if oflo2)
        l_oflo_adr_o_mux_select_(default_adapter(l_oflo_adr_o_mux_select_,chip_index(),pipeIndex,mauIndex,rowIndex/2)),
        l_oflo2_adr_o_mux_select_(default_adapter(l_oflo2_adr_o_mux_select_,chip_index(),pipeIndex,mauIndex,rowIndex/2)),
        r_oflo_adr_o_mux_select_(default_adapter(r_oflo_adr_o_mux_select_,chip_index(),pipeIndex,mauIndex,rowIndex/2)),
        r_oflo2_adr_o_mux_select_(default_adapter(r_oflo2_adr_o_mux_select_,chip_index(),pipeIndex,mauIndex,rowIndex/2)),
        b_oflo_adr_o_mux_select_(default_adapter(b_oflo_adr_o_mux_select_,chip_index(),pipeIndex,mauIndex,rowIndex/2)),
        b_oflo2dn_adr_o_mux_select_(default_adapter(b_oflo2dn_adr_o_mux_select_,chip_index(),pipeIndex,mauIndex,rowIndex/2)),
        t_oflo2up_adr_o_mux_select_(default_adapter(t_oflo2up_adr_o_mux_select_,chip_index(),pipeIndex,mauIndex,rowIndex/2)),
        l_stats_alu_o_mux_select_(default_adapter(l_stats_alu_o_mux_select_,chip_index(),pipeIndex,mauIndex,rowIndex)),
        l_meter_alu_o_mux_select_(default_adapter(l_meter_alu_o_mux_select_,chip_index(),pipeIndex,mauIndex,rowIndex)),
        l_oflo_wr_o_mux_select_(default_adapter(l_oflo_wr_o_mux_select_,chip_index(),pipeIndex,mauIndex,rowIndex)),
        l_oflo2_wr_o_mux_select_(default_adapter(l_oflo2_wr_o_mux_select_,chip_index(),pipeIndex,mauIndex,rowIndex)),
        r_action_o_mux_select_(default_adapter(r_action_o_mux_select_,chip_index(),pipeIndex,mauIndex,rowIndex)),
        r_l_action_o_mux_select_(default_adapter(r_l_action_o_mux_select_,chip_index(),pipeIndex,mauIndex,rowIndex)),
        r_stats_alu_o_mux_select_(default_adapter(r_stats_alu_o_mux_select_,chip_index(),pipeIndex,mauIndex,rowIndex)),
        r_meter_alu_o_mux_select_(default_adapter(r_meter_alu_o_mux_select_,chip_index(),pipeIndex,mauIndex,rowIndex)),
        r_oflo_wr_o_mux_select_(default_adapter(r_oflo_wr_o_mux_select_,chip_index(),pipeIndex,mauIndex,rowIndex)),
        r_oflo2_wr_o_mux_select_(default_adapter(r_oflo2_wr_o_mux_select_,chip_index(),pipeIndex,mauIndex,rowIndex)),
        b_oflo_wr_o_mux_select_(default_adapter(b_oflo_wr_o_mux_select_,chip_index(),pipeIndex,mauIndex,rowIndex,
                                [this](){this->b_oflo_wr_o_mux_write_cb();})),
        b_oflo2dn_wr_o_mux_select_(default_adapter(b_oflo2dn_wr_o_mux_select_,chip_index(),pipeIndex,mauIndex,rowIndex)),
        b_oflo2up_rd_o_mux_select_(default_adapter(b_oflo2up_rd_o_mux_select_,chip_index(),pipeIndex,mauIndex,rowIndex)),
        t_oflo_rd_o_mux_select_(default_adapter(t_oflo_rd_o_mux_select_,chip_index(),pipeIndex,mauIndex,rowIndex,
                                [this](){this->t_oflo_rd_o_mux_write_cb();})),
        t_oflo2dn_rd_o_mux_select_(default_adapter(t_oflo2dn_rd_o_mux_select_,chip_index(),pipeIndex,mauIndex,rowIndex)),
        t_oflo2up_wr_o_mux_select_(default_adapter(t_oflo2up_wr_o_mux_select_,chip_index(),pipeIndex,mauIndex,rowIndex)),
        action_hv_xbar_disable_ram_adr_(default_adapter(action_hv_xbar_disable_ram_adr_,chip_index(),pipeIndex,mauIndex,rowIndex)),
        r_oflo_color_write_o_mux_select_(default_adapter(r_oflo_color_write_o_mux_select_,chip_index(),pipeIndex,mauIndex,
                                         row_to_color_write_switch_box(rowIndex))),
        b_oflo_color_write_o_mux_select_(default_adapter(b_oflo_color_write_o_mux_select_,chip_index(),pipeIndex,mauIndex,
                                         row_to_color_write_switch_box(rowIndex))),
        row_has_color_write_switch_box_( row_has_color_write_switch_box(rowIndex) ),
        selector_action_ram_shift_(default_adapter(selector_action_ram_shift_,chip_index(),pipeIndex,mauIndex,rowIndex)),
        synth2port_fabric_ctl_(default_adapter(synth2port_fabric_ctl_,chip_index(),pipeIndex,mauIndex,rowIndex)),
        synth2port_hbus_members_(default_adapter(synth2port_hbus_members_,chip_index(),pipeIndex,mauIndex,rowIndex)),
        synth2port_ctl_(default_adapter(synth2port_ctl_,chip_index(),pipeIndex,mauIndex,rowIndex))
  {

    l_oflo_adr_o_mux_select_.reset();
    l_oflo2_adr_o_mux_select_.reset();
    r_oflo_adr_o_mux_select_.reset();
    r_oflo2_adr_o_mux_select_.reset();
    b_oflo_adr_o_mux_select_.reset();
    b_oflo2dn_adr_o_mux_select_.reset();
    t_oflo2up_adr_o_mux_select_.reset();
    l_stats_alu_o_mux_select_.reset();
    l_meter_alu_o_mux_select_.reset();
    l_oflo_wr_o_mux_select_.reset();
    l_oflo2_wr_o_mux_select_.reset();
    r_action_o_mux_select_.reset();
    r_l_action_o_mux_select_.reset();
    r_stats_alu_o_mux_select_.reset();
    r_meter_alu_o_mux_select_.reset();
    r_oflo_wr_o_mux_select_.reset();
    r_oflo2_wr_o_mux_select_.reset();
    b_oflo_wr_o_mux_select_.reset();
    b_oflo2dn_wr_o_mux_select_.reset();
    b_oflo2up_rd_o_mux_select_.reset();
    t_oflo_rd_o_mux_select_.reset();
    t_oflo2dn_rd_o_mux_select_.reset();
    t_oflo2up_wr_o_mux_select_.reset();
    action_hv_xbar_disable_ram_adr_.reset();
    r_oflo_color_write_o_mux_select_.reset();
    b_oflo_color_write_o_mux_select_.reset();
    selector_action_ram_shift_.reset();
    synth2port_fabric_ctl_.reset();
    synth2port_hbus_members_.reset();
    synth2port_ctl_.reset();
  }
  MauSramRowReg::~MauSramRowReg() {  }


  void MauSramRowReg::set_row_above(MauSramRow *a) {
    mau_sram_row_above_ = a;
    if (a != NULL) regs_above_ = a->row_registers(); else regs_above_ = NULL;
  }
  void MauSramRowReg::set_row_below(MauSramRow *b) {
    mau_sram_row_below_ = b;
    if (b != NULL) regs_below_ = b->row_registers(); else regs_below_ = NULL;
  }
  void MauSramRowReg::set_logrow_left(MauLogicalRow *l) {
    mau_logical_row_left_ = l;
  }
  void MauSramRowReg::set_logrow_right(MauLogicalRow *r) {
    mau_logical_row_right_ = r;
  }



  // READ addresses
  //
  // These are the sources the fabric funcs want to select/mux
  // and correspond to actual data variables stored per logical row
  // NOTE: NO oflo_wr/oflo2_wr - these are targets (functions) only
  void MauSramRowReg::selector_adr_l(uint32_t *addr) {
    RMT_ASSERT(mau_logical_row_left_ != NULL);
    RMT_ASSERT(row_odd());
    mau_logical_row_left_->selector_rd_addr(addr);
  }
  void MauSramRowReg::selector_adr_r(uint32_t *addr) {
    RMT_ASSERT(mau_logical_row_right_ != NULL);
    RMT_ASSERT(row_odd());
    mau_logical_row_right_->selector_rd_addr(addr);
  }
  // These refer to sources in the row above/below and so will
  // (ultimately) also resolve to actual data variables somewhere
  void MauSramRowReg::oflo_adr_t(uint32_t *addr) {
    if (row_below_middle()) RMT_ASSERT(!kMemoryCoreSplit);
    RMT_ASSERT(regs_above_ != NULL);
    regs_above_->b_oflo_adr(addr);
  }
  void MauSramRowReg::oflo2dn_adr_t(uint32_t *addr) {
    if (regs_above_ != NULL) regs_above_->b_oflo2dn_adr(addr);
  }
  void MauSramRowReg::oflo2up_adr_b(uint32_t *addr) {
    if (regs_below_ != NULL) regs_below_->t_oflo2up_adr(addr);
  }

  // These are composite sources which mux real sources from this
  // row plus sources from above/below
  void MauSramRowReg::l_oflo_adr(uint32_t *addr) {
    if (row_odd()) {
      register_classes::LOfloAdrOMuxSelect *mux = &l_oflo_adr_o_mux_select_;
      //   l_oflo_adr_o_mux_select_l_oflo_adr_o_sel_oflo_adr_t_i
      if (mux->l_oflo_adr_o_sel_oflo_adr_t_i() != 0) oflo_adr_t(addr);
      //   l_oflo_adr_o_mux_select_l_oflo_adr_o_sel_selector_adr_r_i
      if (mux->l_oflo_adr_o_sel_selector_adr_r_i() != 0) selector_adr_r(addr);
    }
    else oflo_adr_t(addr);
  }
  void MauSramRowReg::l_oflo2_adr(uint32_t *addr) {
    RMT_ASSERT(row_odd());
    register_classes::LOflo2AdrOMuxSelect *mux = &l_oflo2_adr_o_mux_select_;
    //   l_oflo2_adr_o_mux_select_l_oflo2_adr_o_sel_selector_adr_r_i
    if (mux->l_oflo2_adr_o_sel_selector_adr_r_i() != 0) selector_adr_r(addr);
    //   l_oflo2_adr_o_mux_select_l_oflo2_adr_o_sel_oflo2up_adr_b_i
    if (mux->l_oflo2_adr_o_sel_oflo2up_adr_b_i() != 0) oflo2up_adr_b(addr);
    //   l_oflo2_adr_o_mux_select_l_oflo2_adr_o_sel_oflo2dn_adr_t_i
    if (mux->l_oflo2_adr_o_sel_oflo2dn_adr_t_i() != 0) oflo2dn_adr_t(addr);
  }
  void MauSramRowReg::r_oflo_adr(uint32_t *addr) {
    if (row_odd()) {
      register_classes::ROfloAdrOMuxSelect *mux = &r_oflo_adr_o_mux_select_;
      // I THINK THIS IS ACTUALLY JUST SELECTING OFLO_ADR_T - BAD NAME
      //   r_oflo_adr_o_mux_select_r_oflo_adr_o_mux_select
      if (mux->r_oflo_adr_o_mux_select() != 0) oflo_adr_t(addr);
    }
    else oflo_adr_t(addr);
  }
  void MauSramRowReg::r_oflo2_adr(uint32_t *addr) {
    RMT_ASSERT(row_odd());
    register_classes::ROflo2AdrOMuxSelect *mux = &r_oflo2_adr_o_mux_select_;
    //   r_oflo2_adr_o_mux_select_r_oflo2_adr_o_sel_selector_adr_l_i
    if (mux->r_oflo2_adr_o_sel_selector_adr_l_i() != 0) selector_adr_l(addr);
    //   r_oflo2_adr_o_mux_select_r_oflo2_adr_o_sel_oflo2up_adr_b_i
    if (mux->r_oflo2_adr_o_sel_oflo2up_adr_b_i() != 0) oflo2up_adr_b(addr);
    //   r_oflo2_adr_o_mux_select_r_oflo2_adr_o_sel_oflo2dn_adr_t_i
    if (mux->r_oflo2_adr_o_sel_oflo2dn_adr_t_i() != 0) oflo2dn_adr_t(addr);
  }
  void MauSramRowReg::b_oflo_adr(uint32_t *addr) {
    if (row_odd()) {
      register_classes::BOfloAdrOMuxSelect *mux = &b_oflo_adr_o_mux_select_;
      //   b_oflo_adr_o_mux_select_b_oflo_adr_o_sel_selector_adr_l_i
      if (mux->b_oflo_adr_o_sel_selector_adr_l_i() != 0) selector_adr_l(addr);
      //   b_oflo_adr_o_mux_select_b_oflo_adr_o_sel_selector_adr_r_i
      if (mux->b_oflo_adr_o_sel_selector_adr_r_i() != 0) selector_adr_r(addr);
      //   b_oflo_adr_o_mux_select_b_oflo_adr_o_sel_oflo_adr_t_i
      if (mux->b_oflo_adr_o_sel_oflo_adr_t_i() != 0) oflo_adr_t(addr);
    }
    else oflo_adr_t(addr);
  }

  void MauSramRowReg::b_oflo2dn_adr(uint32_t *addr) {
    if (row_odd()) {
      register_classes::BOflo2dnAdrOMuxSelect *mux = &b_oflo2dn_adr_o_mux_select_;
      //   b_oflo2dn_adr_o_mux_select_b_oflo2dn_adr_o_sel_selector_adr_l_i
      if (mux->b_oflo2dn_adr_o_sel_selector_adr_l_i() != 0) selector_adr_l(addr);
      //   b_oflo2dn_adr_o_mux_select_b_oflo2dn_adr_o_sel_selector_adr_r_i
      if (mux->b_oflo2dn_adr_o_sel_selector_adr_r_i() != 0) selector_adr_r(addr);
      //   b_oflo2dn_adr_o_mux_select_b_oflo2dn_adr_o_sel_oflo2dn_adr_t_i
      if (mux->b_oflo2dn_adr_o_sel_oflo2dn_adr_t_i() != 0) oflo2dn_adr_t(addr);
    }
    else oflo2dn_adr_t(addr);
  }
  void MauSramRowReg::t_oflo2up_adr(uint32_t *addr) {
    if (row_odd()) {
      register_classes::TOflo2upAdrOMuxSelect *mux = &t_oflo2up_adr_o_mux_select_;
      //   t_oflo2up_adr_o_mux_select_t_oflo2up_adr_o_sel_selector_adr_l_i
      if (mux->t_oflo2up_adr_o_sel_selector_adr_l_i() != 0) selector_adr_l(addr);
      //   t_oflo2up_adr_o_mux_select_t_oflo2up_adr_o_sel_selector_adr_r_i
      if (mux->t_oflo2up_adr_o_sel_selector_adr_r_i() != 0) selector_adr_r(addr);
      //   t_oflo2up_adr_o_mux_select_t_oflo2up_adr_o_sel_oflo2up_adr_b_i
      if (mux->t_oflo2up_adr_o_sel_oflo2up_adr_b_i() != 0) oflo2up_adr_b(addr);
    }
    else oflo2up_adr_b(addr);
  }


  // WRITE addresses
  //
  // These are the sources the fabric funcs want to select/mux
  // and correspond to actual data variables stored per logical row
  // NOTE: NO oflo_wr/oflo2_wr - these are targets (functions) only
  void MauSramRowReg::l_stats_wadr(uint32_t *waddr) {
    RMT_ASSERT(0); // all wadr register are gone!
    RMT_ASSERT(mau_logical_row_left_ != NULL);
    // mau_logical_row_left_->stats_wr_addr(waddr);
  }
  void MauSramRowReg::r_stats_wadr(uint32_t *waddr) {
    RMT_ASSERT(0); // all wadr register are gone!
    RMT_ASSERT(mau_logical_row_right_ != NULL);
    // mau_logical_row_right_->stats_wr_addr(waddr);
  }
  void MauSramRowReg::l_meter_wadr(uint32_t *waddr) {
    RMT_ASSERT(0); // all wadr register are gone!
    RMT_ASSERT(mau_logical_row_left_ != NULL);
    // mau_logical_row_left_->meter_wr_addr(waddr);
  }
  void MauSramRowReg::r_meter_wadr(uint32_t *waddr) {
    RMT_ASSERT(0); // all wadr register are gone!
    RMT_ASSERT(mau_logical_row_right_ != NULL);
    // mau_logical_row_right_->meter_wr_addr(waddr);
  }
  // These refer to sources in the row above/below and so will
  // (ultimately) also resolve to actual data variables somewhere
  void MauSramRowReg::t_oflo_wadr(uint32_t *waddr) {
    RMT_ASSERT(0); // all wadr register are gone!
    // if (regs_above_ != NULL) regs_above_->b_oflo_wadr(waddr);
  }
  void MauSramRowReg::t_oflo2dn_wadr(uint32_t *waddr) {
    RMT_ASSERT(0); // all wadr register are gone!
    // if (regs_above_ != NULL) regs_above_->b_oflo2dn_wadr(waddr);
  }
  void MauSramRowReg::b_oflo2up_wadr(uint32_t *waddr) {
    RMT_ASSERT(0); // all wadr register are gone!
    // if (regs_below_ != NULL) regs_below_->t_oflo2up_wadr(waddr);
  }

  // These are composite sources which mux real sources from this
  // row plus sources from above/below
  void MauSramRowReg::l_oflo_wadr(uint32_t *waddr) {
    RMT_ASSERT(0); // all wadr register are gone!
    //register_classes::LOfloWadrOMuxSelect *mux = &l_oflo_wadr_o_mux_select_;
    //if (mux->l_oflo_wadr_o_sel_r_stats_wadr_i() != 0) r_stats_wadr(waddr);
    //if (mux->l_oflo_wadr_o_sel_r_meter_wadr_i() != 0) r_meter_wadr(waddr);
    //if (mux->l_oflo_wadr_o_sel_t_oflo_wadr_i() != 0) t_oflo_wadr(waddr);
  }
  void MauSramRowReg::l_oflo2_wadr(uint32_t *waddr) {
    RMT_ASSERT(0); // all wadr register are gone!
    //register_classes::LOflo2WadrOMuxSelect *mux = &l_oflo2_wadr_o_mux_select_;
    //if (mux->l_oflo2_wadr_o_sel_r_stats_wadr_i() != 0) r_stats_wadr(waddr);
    //if (mux->l_oflo2_wadr_o_sel_r_meter_wadr_i() != 0) r_meter_wadr(waddr);
    //if (mux->l_oflo2_wadr_o_sel_t_oflo2dn_wadr_i() != 0) t_oflo2dn_wadr(waddr);
    //if (mux->l_oflo2_wadr_o_sel_b_oflo2up_wadr_i() != 0) b_oflo2up_wadr(waddr);
  }
  void MauSramRowReg::r_oflo_wadr(uint32_t *waddr) {
    RMT_ASSERT(0); // all wadr register are gone!
    //register_classes::ROfloWadrOMuxSelect *mux = &r_oflo_wadr_o_mux_select_;
    // I THINK THIS IS ACTUALLY JUST SELECTING OFLO_WADR_T - BAD NAME
    //   r_oflo_wadr_o_mux_select_r_oflo_wadr_o_mux_select
    // if (mux->r_oflo_wadr_o_mux_select() != 0) t_oflo_wadr(waddr);
  }
  void MauSramRowReg::r_oflo2_wadr(uint32_t *waddr) {
    RMT_ASSERT(0); // all wadr register are gone!
    //register_classes::ROflo2WadrOMuxSelect *mux = &r_oflo2_wadr_o_mux_select_;
    // if (mux->r_oflo2_wadr_o_sel_l_stats_wadr_i() != 0) l_stats_wadr(waddr);
    //if (mux->r_oflo2_wadr_o_sel_l_meter_wadr_i() != 0) l_meter_wadr(waddr);
    //if (mux->r_oflo2_wadr_o_sel_t_oflo2dn_wadr_i() != 0) t_oflo2dn_wadr(waddr);
    //if (mux->r_oflo2_wadr_o_sel_b_oflo2up_wadr_i() != 0) b_oflo2up_wadr(waddr);
  }
  void MauSramRowReg::b_oflo_wadr(uint32_t *waddr) {
    RMT_ASSERT(0); // all wadr register are gone!
    //register_classes::BOfloWadrOMuxSelect *mux = &b_oflo_wadr_o_mux_select_;
    //if (mux->b_oflo_wadr_o_sel_t_oflo_wadr_i() != 0) t_oflo_wadr(waddr);
    //if (mux->b_oflo_wadr_o_sel_l_stats_wadr_i() != 0) l_stats_wadr(waddr);
    //if (mux->b_oflo_wadr_o_sel_l_meter_wadr_i() != 0) l_meter_wadr(waddr);
    //if (mux->b_oflo_wadr_o_sel_r_stats_wadr_i() != 0) r_stats_wadr(waddr);
    //if (mux->b_oflo_wadr_o_sel_r_meter_wadr_i() != 0) r_meter_wadr(waddr);
  }
  void MauSramRowReg::b_oflo2dn_wadr(uint32_t *waddr) {
    RMT_ASSERT(0); // all wadr register are gone!
    //register_classes::BOflo2dnWadrOMuxSelect *mux = &b_oflo2dn_wadr_o_mux_select_;
    //if (mux->b_oflo2dn_wadr_o_sel_t_oflo2dn_wadr_i() != 0) t_oflo2dn_wadr(waddr);
    //if (mux->b_oflo2dn_wadr_o_sel_l_stats_wadr_i() != 0) l_stats_wadr(waddr);
    //if (mux->b_oflo2dn_wadr_o_sel_l_meter_wadr_i() != 0) l_meter_wadr(waddr);
    //if (mux->b_oflo2dn_wadr_o_sel_r_stats_wadr_i() != 0) r_stats_wadr(waddr);
    //if (mux->b_oflo2dn_wadr_o_sel_r_meter_wadr_i() != 0) r_meter_wadr(waddr);
  }
  void MauSramRowReg::t_oflo2up_wadr(uint32_t *waddr) {
    RMT_ASSERT(0); // all wadr register are gone!
    //register_classes::TOflo2upWadrOMuxSelect *mux = &t_oflo2up_wadr_o_mux_select_;
    //if (mux->t_oflo2up_wadr_o_sel_b_oflo2up_wadr_i() != 0) b_oflo2up_wadr(waddr);
    //if (mux->t_oflo2up_wadr_o_sel_l_stats_wadr_i() != 0) l_stats_wadr(waddr);
    //if (mux->t_oflo2up_wadr_o_sel_l_meter_wadr_i() != 0) l_meter_wadr(waddr);
    //if (mux->t_oflo2up_wadr_o_sel_r_stats_wadr_i() != 0) r_stats_wadr(waddr);
    //if (mux->t_oflo2up_wadr_o_sel_r_meter_wadr_i() != 0) r_meter_wadr(waddr);
  }



  // READ/WRITE data
  //
  // These are the data sources the fabric funcs want to select/mux
  // and correspond to actual data VARIABLES stored per logical row
  // NOTE: NO oflo_wr/oflo2_wr - these are functions only
  void MauSramRowReg::action_rd_l(BitVector<kDataBusWidth> *data,
                                  uint32_t *addr, int op) {
    RMT_ASSERT(mau_logical_row_left_ != NULL);
    mau_logical_row_left_->action_rd_data(data);
    mau_logical_row_left_->action_rd_addr(addr);
  }
  void MauSramRowReg::action_rd_r(BitVector<kDataBusWidth> *data,
                                  uint32_t *addr, int op) {
    RMT_ASSERT(mau_logical_row_right_ != NULL);
    mau_logical_row_right_->action_rd_data(data);
    mau_logical_row_right_->action_rd_addr(addr);
  }
  void MauSramRowReg::stats_rd_l(BitVector<kDataBusWidth> *data,
                                 uint32_t *addr, int op) {
    RMT_ASSERT(mau_logical_row_left_ != NULL);
    RMT_ASSERT(get_opcode(op) != kMuxOpcodePushALU);
    mau_logical_row_left_->stats_rd_data(data);
    mau_logical_row_left_->stats_rd_addr(addr);
  }
  void MauSramRowReg::stats_rd_r(BitVector<kDataBusWidth> *data,
                                 uint32_t *addr, int op) {
    RMT_ASSERT(mau_logical_row_right_ != NULL);
    RMT_ASSERT(get_opcode(op) != kMuxOpcodePushALU);
    mau_logical_row_right_->stats_rd_data(data);
    mau_logical_row_right_->stats_rd_addr(addr);
  }
  void MauSramRowReg::stats_wr_l(BitVector<kDataBusWidth> *data,
                                 uint32_t *addr, int op) {
    RMT_ASSERT(mau_logical_row_left_ != NULL);
    mau_logical_row_left_->stats_wr_data(data);
    mau_logical_row_left_->stats_wr_addr(addr);
  }
  void MauSramRowReg::stats_wr_r(BitVector<kDataBusWidth> *data,
                                 uint32_t *addr, int op) {
    RMT_ASSERT(mau_logical_row_right_ != NULL);
    mau_logical_row_right_->stats_wr_data(data);
    mau_logical_row_right_->stats_wr_addr(addr);
  }
  void MauSramRowReg::meter_rd_l(BitVector<kDataBusWidth> *data,
                                 uint32_t *addr, int op) {
    RMT_ASSERT(mau_logical_row_left_ != NULL);
    mau_logical_row_left_->meter_rd_data(data);
    mau_logical_row_left_->meter_rd_addr(addr);
  }
  void MauSramRowReg::meter_rd_r(BitVector<kDataBusWidth> *data,
                                 uint32_t *addr, int op) {
    RMT_ASSERT(mau_logical_row_right_ != NULL);
    mau_logical_row_right_->meter_rd_data(data);
    mau_logical_row_right_->meter_rd_addr(addr);
  }
  void MauSramRowReg::meter_wr_l(BitVector<kDataBusWidth> *data,
                                 uint32_t *addr, int op) {
    RMT_ASSERT(mau_logical_row_left_ != NULL);
    mau_logical_row_left_->meter_wr_data(data);
    mau_logical_row_left_->meter_wr_addr(addr);
  }
  void MauSramRowReg::meter_wr_r(BitVector<kDataBusWidth> *data,
                                 uint32_t *addr, int op) {
    RMT_ASSERT(mau_logical_row_right_ != NULL);
    mau_logical_row_right_->meter_wr_data(data);
    mau_logical_row_right_->meter_wr_addr(addr);
  }
  void MauSramRowReg::oflo_rd_l(BitVector<kDataBusWidth> *data,
                                uint32_t *addr, int op) {
    RMT_ASSERT(mau_logical_row_left_ != NULL);
    RMT_ASSERT(get_opcode(op) != kMuxOpcodePushALU);
    mau_logical_row_left_->oflo_rd_data(data);
    mau_logical_row_left_->oflo_rd_addr(addr, get_addrtype(op));
  }
  void MauSramRowReg::oflo_rd_r(BitVector<kDataBusWidth> *data,
                                uint32_t *addr, int op) {
    RMT_ASSERT(mau_logical_row_right_ != NULL);
    RMT_ASSERT(get_opcode(op) != kMuxOpcodePushALU);
    mau_logical_row_right_->oflo_rd_data(data);
    mau_logical_row_right_->oflo_rd_addr(addr, get_addrtype(op));
  }
  void MauSramRowReg::oflo2_rd_l(BitVector<kDataBusWidth> *data,
                                 uint32_t *addr, int op) {
    RMT_ASSERT(mau_logical_row_left_ != NULL);
    mau_logical_row_left_->oflo2_rd_data(data);
    mau_logical_row_left_->oflo2_rd_addr(addr, get_addrtype(op));
  }
  void MauSramRowReg::oflo2_rd_r(BitVector<kDataBusWidth> *data,
                                 uint32_t *addr, int op) {
    RMT_ASSERT(mau_logical_row_right_ != NULL);
    mau_logical_row_right_->oflo2_rd_data(data);
    mau_logical_row_right_->oflo2_rd_addr(addr, get_addrtype(op));
  }

  // These refer to sources in the row above/below and so will
  // (ultimately) also resolve to actual data variables somewhere
  // NB. regs_above_ regs_below_ could be NULL if we are top/bot row
  void MauSramRowReg::oflo2dn_rd_b(BitVector<kDataBusWidth> *data,
                                   uint32_t *addr, int op) {
    if (regs_below_ != NULL) regs_below_->t_oflo2dn_rd(data, addr, op);
  }
  void MauSramRowReg::oflo2dn_wr_t(BitVector<kDataBusWidth> *data,
                                   uint32_t *addr, int op) {
    if (regs_above_ != NULL) regs_above_->b_oflo2dn_wr(data, addr, op);
  }
  void MauSramRowReg::oflo2up_rd_t(BitVector<kDataBusWidth> *data,
                                   uint32_t *addr, int op) {
    if (regs_above_ != NULL) regs_above_->b_oflo2up_rd(data, addr, op);
  }
  void MauSramRowReg::oflo2up_wr_b(BitVector<kDataBusWidth> *data,
                                   uint32_t *addr, int op) {
    if (regs_below_ != NULL) regs_below_->t_oflo2up_wr(data, addr, op);
  }
  void MauSramRowReg::oflo_rd_b(BitVector<kDataBusWidth> *data,
                                uint32_t *addr, int op) {
    if (row_above_middle()) RMT_ASSERT(!kMemoryCoreSplit);
    RMT_ASSERT((regs_below_ != NULL) && "Trying to access row BELOW row0");
    regs_below_->t_oflo_rd(data, addr, op);
  }
  void MauSramRowReg::oflo_wr_t(BitVector<kDataBusWidth> *data,
                                uint32_t *addr, int op) {
    if (row_below_middle()) RMT_ASSERT(!kMemoryCoreSplit);
    RMT_ASSERT((regs_above_ != NULL) && "Trying to access row ABOVE row7");
    regs_above_->b_oflo_wr(data, addr, op);
  }

  // These are composite sources which mux real sources from this
  // row plus sources from above/below
  void MauSramRowReg::l_stats_alu(BitVector<kDataBusWidth> *data, uint32_t *addr, int op) {
    if (op == 0) op = make_op_fetch(AddrType::kStats);
    register_classes::LStatsAluOMuxSelect *mux = &l_stats_alu_o_mux_select_;
    //   l_stats_alu_o_mux_select_l_stats_alu_o_sel_stats_rd_l_i
    if (mux->l_stats_alu_o_sel_stats_rd_l_i() != 0) stats_rd_l(data, addr, op);
    //   l_stats_alu_o_mux_select_l_stats_alu_o_sel_oflo2_rd_r_i
    if (mux->l_stats_alu_o_sel_oflo2_rd_r_i() != 0) oflo2_rd_r(data, addr, op);
    //   l_stats_alu_o_mux_select_l_stats_alu_o_sel_oflo_rd_b_i
    if (mux->l_stats_alu_o_sel_oflo_rd_b_i() != 0) oflo_rd_b(data, addr, op);
    //   l_stats_alu_o_mux_select_l_stats_alu_o_sel_oflo2dn_rd_b_i
    if (mux->l_stats_alu_o_sel_oflo2dn_rd_b_i() != 0) oflo2dn_rd_b(data, addr, op);
    //   l_stats_alu_o_mux_select_l_stats_alu_o_sel_oflo2up_rd_t_i
    if (mux->l_stats_alu_o_sel_oflo2up_rd_t_i() != 0) oflo2up_rd_t(data, addr, op);
  }
  void MauSramRowReg::l_meter_alu(BitVector<kDataBusWidth> *data, uint32_t *addr, int op) {
    if (op == 0) op = make_op_fetch(AddrType::kMeter);
    register_classes::LMeterAluOMuxSelect *mux = &l_meter_alu_o_mux_select_;
    //   l_meter_alu_o_mux_select_l_meter_alu_o_sel_meter_rd_l_i
    if (mux->l_meter_alu_o_sel_meter_rd_l_i() != 0) meter_rd_l(data, addr, op);
    //   l_meter_alu_o_mux_select_l_meter_alu_o_sel_oflo2_rd_r_i
    if (mux->l_meter_alu_o_sel_oflo2_rd_r_i() != 0) oflo2_rd_r(data, addr, op);
    //   l_meter_alu_o_mux_select_l_meter_alu_o_sel_oflo_rd_b_i
    if (mux->l_meter_alu_o_sel_oflo_rd_b_i() != 0) oflo_rd_b(data, addr, op);
    //   l_meter_alu_o_mux_select_l_meter_alu_o_sel_oflo2dn_rd_b_i
    if (mux->l_meter_alu_o_sel_oflo2dn_rd_b_i() != 0) oflo2dn_rd_b(data, addr, op);
    //   l_meter_alu_o_mux_select_l_meter_alu_o_sel_oflo2up_rd_t_i
    if (mux->l_meter_alu_o_sel_oflo2up_rd_t_i() != 0) oflo2up_rd_t(data, addr, op);
  }
  void MauSramRowReg::l_oflo_wr(BitVector<kDataBusWidth> *data,
                                uint32_t *addr, int op) {
    register_classes::LOfloWrOMuxSelect *mux = &l_oflo_wr_o_mux_select_;
    //   l_oflo_wr_o_mux_select_l_oflo_wr_o_sel_oflo_wr_t_i
    if (mux->l_oflo_wr_o_sel_oflo_wr_t_i() != 0) oflo_wr_t(data, addr, op);
    //   l_oflo_wr_o_mux_select_l_oflo_wr_o_sel_stats_wr_r_i
    if (mux->l_oflo_wr_o_sel_stats_wr_r_i() != 0) stats_wr_r(data, addr, op);
    //   l_oflo_wr_o_mux_select_l_oflo_wr_o_sel_meter_wr_r_i
    if (mux->l_oflo_wr_o_sel_meter_wr_r_i() != 0) meter_wr_r(data, addr, op);
  }
  void MauSramRowReg::l_oflo2_wr(BitVector<kDataBusWidth> *data,
                                 uint32_t *addr, int op) {
    register_classes::LOflo2WrOMuxSelect *mux = &l_oflo2_wr_o_mux_select_;
    //   l_oflo2_wr_o_mux_select_l_oflo2_wr_o_sel_stats_wr_r_i
    if (mux->l_oflo2_wr_o_sel_stats_wr_r_i() != 0) stats_wr_r(data, addr, op);
    //   l_oflo2_wr_o_mux_select_l_oflo2_wr_o_sel_meter_wr_r_i
    if (mux->l_oflo2_wr_o_sel_meter_wr_r_i() != 0) meter_wr_r(data, addr, op);
    //   l_oflo2_wr_o_mux_select_l_oflo2_wr_o_sel_oflo2up_wr_b_i
    if (mux->l_oflo2_wr_o_sel_oflo2up_wr_b_i() != 0) oflo2up_wr_b(data, addr, op);
    //   l_oflo2_wr_o_mux_select_l_oflo2_wr_o_sel_oflo2dn_wr_t_i
    if (mux->l_oflo2_wr_o_sel_oflo2dn_wr_t_i() != 0) oflo2dn_wr_t(data, addr, op);
  }
  void MauSramRowReg::r_action(BitVector<kDataBusWidth> *data, uint32_t *addr, int op) {
    if (op == 0) op = make_op_fetch(AddrType::kAction);
    register_classes::RActionOMuxSelect *mux = &r_action_o_mux_select_;
    //   r_action_o_mux_select_r_action_o_sel_oflo_rd_l_i
    if (mux->r_action_o_sel_oflo_rd_l_i() != 0) oflo_rd_l(data, addr, op);
    //   r_action_o_mux_select_r_action_o_sel_oflo2_rd_l_i
    if (mux->r_action_o_sel_oflo2_rd_l_i() != 0) oflo2_rd_l(data, addr, op);
    //   r_action_o_mux_select_r_action_o_sel_oflo_rd_b_i
    if (mux->r_action_o_sel_oflo_rd_b_i() != 0) oflo_rd_b(data, addr, op);
    //   r_action_o_mux_select_r_action_o_sel_oflo2dn_rd_b_i
    if (mux->r_action_o_sel_oflo2dn_rd_b_i() != 0) oflo2dn_rd_b(data, addr, op);
    //   r_action_o_mux_select_r_action_o_sel_oflo2up_rd_t_i
    if (mux->r_action_o_sel_oflo2up_rd_t_i() != 0) oflo2up_rd_t(data, addr, op);
    //   r_action_o_mux_select_r_action_o_sel_action_rd_r_i
    if (mux->r_action_o_sel_action_rd_r_i() != 0) action_rd_r(data, addr, op);
  }
  void MauSramRowReg::r_l_action(BitVector<kDataBusWidth> *data, uint32_t *addr, int op) {
    if (op == 0) op = make_op_fetch(AddrType::kAction);
    register_classes::RLActionOMuxSelect *mux = &r_l_action_o_mux_select_;
    //  r_l_action_o_mux_select_r_l_action_o_sel_action_rd_l_i
    if (mux->r_l_action_o_sel_action_rd_l_i() != 0) action_rd_l(data, addr, op);
    //  r_l_action_o_mux_select_r_l_action_o_sel_oflo2_rd_r_i
    if (mux->r_l_action_o_sel_oflo2_rd_r_i() != 0) oflo2_rd_r(data, addr, op);
    //  r_l_action_o_mux_select_r_l_action_o_sel_oflo_rd_b_i
    if (mux->r_l_action_o_sel_oflo_rd_b_i() != 0) oflo_rd_b(data, addr, op);
    //  r_l_action_o_mux_select_r_l_action_o_sel_oflo2dn_rd_b_i
    if (mux->r_l_action_o_sel_oflo2dn_rd_b_i() != 0) oflo2dn_rd_b(data, addr, op);
    //  r_l_action_o_mux_select_r_l_action_o_sel_oflo2up_rd_t_i
    if (mux->r_l_action_o_sel_oflo2up_rd_t_i() != 0) oflo2up_rd_t(data, addr, op);
  }
  void MauSramRowReg::r_stats_alu(BitVector<kDataBusWidth> *data, uint32_t *addr, int op) {
    if (op == 0) op = make_op_fetch(AddrType::kStats);
    register_classes::RStatsAluOMuxSelect *mux = &r_stats_alu_o_mux_select_;
    //   r_stats_alu_o_mux_select_r_stats_alu_o_sel_stats_rd_r_i
    if (mux->r_stats_alu_o_sel_stats_rd_r_i() != 0) stats_rd_r(data, addr, op);
    //   r_stats_alu_o_mux_select_r_stats_alu_o_sel_oflo_rd_l_i
    if (mux->r_stats_alu_o_sel_oflo_rd_l_i() != 0) oflo_rd_l(data, addr, op);
    //   r_stats_alu_o_mux_select_r_stats_alu_o_sel_oflo2_rd_l_i
    if (mux->r_stats_alu_o_sel_oflo2_rd_l_i() != 0) oflo2_rd_l(data, addr, op);
    //   r_stats_alu_o_mux_select_r_stats_alu_o_sel_oflo_rd_b_i
    if (mux->r_stats_alu_o_sel_oflo_rd_b_i() != 0) oflo_rd_b(data, addr, op);
    //   r_stats_alu_o_mux_select_r_stats_alu_o_sel_oflo2dn_rd_b_i
    if (mux->r_stats_alu_o_sel_oflo2dn_rd_b_i() != 0) oflo2dn_rd_b(data, addr, op);
    //   r_stats_alu_o_mux_select_r_stats_alu_o_sel_oflo2up_rd_t_i
    if (mux->r_stats_alu_o_sel_oflo2up_rd_t_i() != 0) oflo2up_rd_t(data, addr, op);
  }
  void MauSramRowReg::r_meter_alu(BitVector<kDataBusWidth> *data, uint32_t *addr, int op) {
    if (op == 0) op = make_op_fetch(AddrType::kMeter);
    register_classes::RMeterAluOMuxSelect *mux = &r_meter_alu_o_mux_select_;
    //   r_meter_alu_o_mux_select_r_meter_alu_o_sel_meter_rd_r_i
    if (mux->r_meter_alu_o_sel_meter_rd_r_i() != 0) meter_rd_r(data, addr, op);
    //   r_meter_alu_o_mux_select_r_meter_alu_o_sel_oflo_rd_l_i
    if (mux->r_meter_alu_o_sel_oflo_rd_l_i() != 0) oflo_rd_l(data, addr, op);
    //   r_meter_alu_o_mux_select_r_meter_alu_o_sel_oflo2_rd_l_i
    if (mux->r_meter_alu_o_sel_oflo2_rd_l_i() != 0) oflo2_rd_l(data, addr, op);
    //   r_meter_alu_o_mux_select_r_meter_alu_o_sel_oflo_rd_b_i
    if (mux->r_meter_alu_o_sel_oflo_rd_b_i() != 0) oflo_rd_b(data, addr, op);
    //   r_meter_alu_o_mux_select_r_meter_alu_o_sel_oflo2dn_rd_b_i
    if (mux->r_meter_alu_o_sel_oflo2dn_rd_b_i() != 0) oflo2dn_rd_b(data, addr, op);
    //   r_meter_alu_o_mux_select_r_meter_alu_o_sel_oflo2up_rd_t_i
    if (mux->r_meter_alu_o_sel_oflo2up_rd_t_i() != 0) oflo2up_rd_t(data, addr, op);
  }
  void MauSramRowReg::r_oflo_wr(BitVector<kDataBusWidth> *data,
                                uint32_t *addr, int op) {
    register_classes::ROfloWrOMuxSelect *mux = &r_oflo_wr_o_mux_select_;
    // I THINK THIS IS ACTUALLY JUST SELECTING OFLO_WR_T - BAD NAME
    //   r_oflo_wr_o_mux_select_r_oflo_wr_o_mux_select
    if (mux->r_oflo_wr_o_mux_select() != 0) oflo_wr_t(data, addr, op);
  }
  void MauSramRowReg::r_oflo2_wr(BitVector<kDataBusWidth> *data, uint32_t *addr, int op) {
    register_classes::ROflo2WrOMuxSelect *mux = &r_oflo2_wr_o_mux_select_;
    //   r_oflo2_wr_o_mux_select_r_oflo2_wr_o_sel_stats_wr_l_i
    if (mux->r_oflo2_wr_o_sel_stats_wr_l_i() != 0) stats_wr_l(data, addr, op);
    //   r_oflo2_wr_o_mux_select_r_oflo2_wr_o_sel_meter_wr_l_i
    if (mux->r_oflo2_wr_o_sel_meter_wr_l_i() != 0) meter_wr_l(data, addr, op);
    //   r_oflo2_wr_o_mux_select_r_oflo2_wr_o_sel_oflo2up_wr_b_i
    if (mux->r_oflo2_wr_o_sel_oflo2up_wr_b_i() != 0) oflo2up_wr_b(data, addr, op);
    //   r_oflo2_wr_o_mux_select_r_oflo2_wr_o_sel_oflo2dn_wr_t_i
    if (mux->r_oflo2_wr_o_sel_oflo2dn_wr_t_i() != 0) oflo2dn_wr_t(data, addr, op);
  }
  void MauSramRowReg::b_oflo_wr(BitVector<kDataBusWidth> *data,
                                uint32_t *addr, int op) {
    register_classes::BOfloWrOMuxSelect *mux = &b_oflo_wr_o_mux_select_;
    //   b_oflo_wr_o_mux_select_b_oflo_wr_o_sel_meter_wr_l_i
    if (mux->b_oflo_wr_o_sel_meter_wr_l_i() != 0) meter_wr_l(data, addr, op);
    //   b_oflo_wr_o_mux_select_b_oflo_wr_o_sel_stats_wr_l_i
    if (mux->b_oflo_wr_o_sel_stats_wr_l_i() != 0) stats_wr_l(data, addr, op);
    //   b_oflo_wr_o_mux_select_b_oflo_wr_o_sel_meter_wr_r_i
    if (mux->b_oflo_wr_o_sel_meter_wr_r_i() != 0) meter_wr_r(data, addr, op);
    //   b_oflo_wr_o_mux_select_b_oflo_wr_o_sel_stats_wr_r_i
    if (mux->b_oflo_wr_o_sel_stats_wr_r_i() != 0) stats_wr_r(data, addr, op);
    //   b_oflo_wr_o_mux_select_b_oflo_wr_o_sel_oflo_wr_t_i
    if (mux->b_oflo_wr_o_sel_oflo_wr_t_i() != 0) oflo_wr_t(data, addr, op);
  }
  void MauSramRowReg::b_oflo2dn_wr(BitVector<kDataBusWidth> *data, uint32_t *addr, int op) {
    register_classes::BOflo2dnWrOMuxSelect *mux = &b_oflo2dn_wr_o_mux_select_;
    //   b_oflo2dn_wr_o_mux_select_b_oflo2dn_wr_o_sel_meter_wr_l_i
    if (mux->b_oflo2dn_wr_o_sel_meter_wr_l_i() != 0) meter_wr_l(data, addr, op);
    //   b_oflo2dn_wr_o_mux_select_b_oflo2dn_wr_o_sel_stats_wr_l_i
    if (mux->b_oflo2dn_wr_o_sel_stats_wr_l_i() != 0) stats_wr_l(data, addr, op);
    //   b_oflo2dn_wr_o_mux_select_b_oflo2dn_wr_o_sel_stats_wr_r_i
    if (mux->b_oflo2dn_wr_o_sel_stats_wr_r_i() != 0) stats_wr_r(data, addr, op);
    //   b_oflo2dn_wr_o_mux_select_b_oflo2dn_wr_o_sel_meter_wr_r_i
    if (mux->b_oflo2dn_wr_o_sel_meter_wr_r_i() != 0) meter_wr_r(data, addr, op);
    //   b_oflo2dn_wr_o_mux_select_b_oflo2dn_wr_o_sel_oflo2dn_wr_t_i
    if (mux->b_oflo2dn_wr_o_sel_oflo2dn_wr_t_i() != 0) oflo2dn_wr_t(data, addr, op);
  }
  void MauSramRowReg::b_oflo2up_rd(BitVector<kDataBusWidth> *data,
                                   uint32_t *addr, int op) {
    register_classes::BOflo2upRdOMuxSelect *mux = &b_oflo2up_rd_o_mux_select_;
    //   b_oflo2up_rd_o_mux_select_b_oflo2up_rd_o_sel_oflo2_rd_r_i
    if (mux->b_oflo2up_rd_o_sel_oflo2_rd_r_i() != 0) oflo2_rd_r(data, addr, op);
    //   b_oflo2up_rd_o_mux_select_b_oflo2up_rd_o_sel_oflo2_rd_l_i
    if (mux->b_oflo2up_rd_o_sel_oflo2_rd_l_i() != 0) oflo2_rd_l(data, addr, op);
    //   b_oflo2up_rd_o_mux_select_b_oflo2up_rd_o_sel_oflo2up_rd_t_i
    if (mux->b_oflo2up_rd_o_sel_oflo2up_rd_t_i() != 0) oflo2up_rd_t(data, addr, op);
  }
  void MauSramRowReg::t_oflo_rd(BitVector<kDataBusWidth> *data,
                                uint32_t *addr, int op) {
    register_classes::TOfloRdOMuxSelect *mux = &t_oflo_rd_o_mux_select_;
    //   t_oflo_rd_o_mux_select_t_oflo_rd_o_sel_oflo_rd_l_i
    if (mux->t_oflo_rd_o_sel_oflo_rd_l_i() != 0) oflo_rd_l(data, addr, op);
    //   t_oflo_rd_o_mux_select_t_oflo_rd_o_sel_oflo_rd_r_i
    if (mux->t_oflo_rd_o_sel_oflo_rd_r_i() != 0) oflo_rd_r(data, addr, op);
    //   t_oflo_rd_o_mux_select_t_oflo_rd_o_sel_oflo_rd_b_i
    if (mux->t_oflo_rd_o_sel_oflo_rd_b_i() != 0) oflo_rd_b(data, addr, op);
  }
  void MauSramRowReg::t_oflo2dn_rd(BitVector<kDataBusWidth> *data, uint32_t *addr, int op) {
    register_classes::TOflo2dnRdOMuxSelect *mux = &t_oflo2dn_rd_o_mux_select_;
    //   t_oflo2dn_rd_o_mux_select_t_oflo2dn_rd_o_sel_oflo2_rd_l_i
    if (mux->t_oflo2dn_rd_o_sel_oflo2_rd_l_i() != 0) oflo2_rd_l(data, addr, op);
    //   t_oflo2dn_rd_o_mux_select_t_oflo2dn_rd_o_sel_oflo2_rd_r_i
    if (mux->t_oflo2dn_rd_o_sel_oflo2_rd_r_i() != 0) oflo2_rd_r(data, addr, op);
    //   t_oflo2dn_rd_o_mux_select_t_oflo2dn_rd_o_sel_oflo2dn_rd_b_i
    if (mux->t_oflo2dn_rd_o_sel_oflo2dn_rd_b_i() != 0) oflo2dn_rd_b(data, addr, op);
  }
  void MauSramRowReg::t_oflo2up_wr(BitVector<kDataBusWidth> *data,
                                   uint32_t *addr, int op) {
    register_classes::TOflo2upWrOMuxSelect *mux = &t_oflo2up_wr_o_mux_select_;
    //   t_oflo2up_wr_o_mux_select_t_oflo2up_wr_o_sel_stats_wr_l_i
    if (mux->t_oflo2up_wr_o_sel_stats_wr_l_i() != 0) stats_wr_l(data, addr, op);
    //   t_oflo2up_wr_o_mux_select_t_oflo2up_wr_o_sel_meter_wr_l_i
    if (mux->t_oflo2up_wr_o_sel_meter_wr_l_i() != 0) meter_wr_l(data, addr, op);
    //   t_oflo2up_wr_o_mux_select_t_oflo2up_wr_o_sel_stats_wr_r_i
    if (mux->t_oflo2up_wr_o_sel_stats_wr_r_i() != 0) stats_wr_r(data, addr, op);
    //   t_oflo2up_wr_o_mux_select_t_oflo2up_wr_o_sel_meter_wr_r_i
    if (mux->t_oflo2up_wr_o_sel_meter_wr_r_i() != 0) meter_wr_r(data, addr, op);
    //   t_oflo2up_wr_o_mux_select_t_oflo2up_wr_o_sel_oflo2up_wr_b_i
    if (mux->t_oflo2up_wr_o_sel_oflo2up_wr_b_i() != 0) oflo2up_wr_b(data, addr, op);
  }


  // Validate programming of b_oflo_wr_o_mux
  // This (these) are the muxes that hook up per-row write data (stats_wr_data|meter_wr_data)
  // written by ALUs to the logical row oflo_wr_data bus.
  // For any given physical row at most one bit should be enabled in this mux.
  void MauSramRowReg::b_oflo_wr_o_mux_write_cb() {
    uint8_t sel = 0;
    int inputs = 0;
    register_classes::BOfloWrOMuxSelect *mux = &b_oflo_wr_o_mux_select_;
    if (mux->b_oflo_wr_o_sel_oflo_wr_t_i()  != 0) { sel |= 1<<0; inputs++; }
    if (mux->b_oflo_wr_o_sel_stats_wr_r_i() != 0) { sel |= 1<<1; inputs++; }
    if (mux->b_oflo_wr_o_sel_meter_wr_r_i() != 0) { sel |= 1<<2; inputs++; }
    if (mux->b_oflo_wr_o_sel_stats_wr_l_i() != 0) { sel |= 1<<3; inputs++; }
    if (mux->b_oflo_wr_o_sel_meter_wr_l_i() != 0) { sel |= 1<<4; inputs++; }
    if (kMemoryCoreSplit && row_above_middle() && (inputs > 0)) {
      RMT_LOG(RmtDebug::error(),
              "MauSramRowReg: b_oflo_wr_o_mux[%d] providing data for row below middle "
              " (0x%02x) but memory core is split!\n", row_index_, sel);
    }
    if (inputs > 1) {
      RMT_LOG(RmtDebug::error(kRelaxOfloWrMuxCheck),
              "MauSramRowReg: b_oflo_wr_o_mux multiple active inputs (0x%02x)\n", sel);
      if (!kRelaxOfloWrMuxCheck) { THROW_ERROR(-2); }
    }
  }
  // Validate programming of t_oflo_rd_o_mux
  // Track all overflow rows up at MAU level - check on reset_resources()
  // Should not be more than 6 contiguous rows so no more than 5 contiguous oflows
  void MauSramRowReg::t_oflo_rd_o_mux_write_cb() {
    uint8_t sel = 0;
    int inputs = 0;
    register_classes::TOfloRdOMuxSelect *mux = &t_oflo_rd_o_mux_select_;
    if (mux->t_oflo_rd_o_sel_oflo_rd_b_i() != 0) { sel |= 1<<0; inputs++; }
    if (mux->t_oflo_rd_o_sel_oflo_rd_r_i() != 0) { sel |= 1<<1; inputs++; }
    if (mux->t_oflo_rd_o_sel_oflo_rd_l_i() != 0) { sel |= 1<<2; inputs++; }
    if (kMemoryCoreSplit && row_below_middle() && (inputs > 0)) {
      RMT_LOG(RmtDebug::error(),
              "MauSramRowReg: t_oflo_rd_o_mux[%d] providing data for row above middle "
              "(0x%02x) but memory core is split!\n", row_index_, sel);
    }
    mau_sram_row_->mau()->set_data_oflo(row_index_, mux->t_oflo_rd_o_sel_oflo_rd_b_i());
  }

  void MauSramRowReg::b_oflo_color_write(uint8_t* color) {
    RMT_ASSERT(row_at_top() || (regs_above_ != NULL));
    if (row_has_color_write_switch_box_) {
      if (b_oflo_color_write_o_mux_select_.b_oflo_color_write_o_sel_r_color_write_i()) {
        mau_logical_row_right_->color_write_data(color);
      }
      if (b_oflo_color_write_o_mux_select_.b_oflo_color_write_o_sel_t_oflo_color_write_i()) {
        if (row_below_middle()) RMT_ASSERT(!kMemoryCoreSplit);
        if (regs_above_ != NULL) regs_above_->b_oflo_color_write(color);
      }
    }
    else { // no switchbox, always take from above
      if (regs_above_ != NULL) regs_above_->b_oflo_color_write(color);
    }
  }
  void MauSramRowReg::r_oflo_color_write(uint8_t* color) {
    RMT_ASSERT(row_at_top() || (regs_above_ != NULL));
    if (row_has_color_write_switch_box_) {
      if (r_oflo_color_write_o_mux_select_.r_oflo_color_write_o_mux_select()) {
        if (row_below_middle()) RMT_ASSERT(!kMemoryCoreSplit);
        if (regs_above_ != NULL) regs_above_->b_oflo_color_write(color);
      }
    }
    else { // no switchbox, always take from above
      if (regs_above_ != NULL) regs_above_->b_oflo_color_write(color);
    }
  }


  // Ripped off r|b_oflo_color_write funcs above
  uint32_t MauSramRowReg::b_color_alu_rows() {
    RMT_ASSERT(row_at_top() || (regs_above_ != NULL));
    uint32_t rows = 0u;
    if (row_has_color_write_switch_box_) {
      if (b_oflo_color_write_o_mux_select_.b_oflo_color_write_o_sel_r_color_write_i()) {
        RMT_ASSERT(mau_logical_row_right_->mau_meter_alu() != NULL);
        rows |= 1u << mau_logical_row_right_->logical_row_index();
      }
      if (b_oflo_color_write_o_mux_select_.b_oflo_color_write_o_sel_t_oflo_color_write_i()) {
        if (row_below_middle()) RMT_ASSERT(!kMemoryCoreSplit);
        if (regs_above_ != NULL) rows |= regs_above_->b_color_alu_rows();
      }
    } else { // no switchbox, always take from above
      if (regs_above_ != NULL) rows |= regs_above_->b_color_alu_rows();
    }
    return rows;
  }
  uint32_t MauSramRowReg::get_color_alu_rows() {
    RMT_ASSERT(row_at_top() || (regs_above_ != NULL));
    uint32_t rows = 0u;
    if (row_has_color_write_switch_box_) {
      if (r_oflo_color_write_o_mux_select_.r_oflo_color_write_o_mux_select()) {
        if (row_below_middle()) RMT_ASSERT(!kMemoryCoreSplit);
        if (regs_above_ != NULL) rows |= regs_above_->b_color_alu_rows();
      }
    } else { // no switchbox, always take from above
      if (regs_above_ != NULL) rows |= regs_above_->b_color_alu_rows();
    }
    return rows;
  }
  int MauSramRowReg::get_color_alu_logrow_index() {
    uint32_t rows = get_color_alu_rows();
    RMT_ASSERT((MauDefs::kMeterAluLogicalRows & rows) == rows);
    // Return highest row or -1
    return (rows == 0u) ?-1 :__builtin_clzl(1u) - __builtin_clzl(rows);
  }
  int MauSramRowReg::get_color_alu_index() {
    int alurow = get_color_alu_logrow_index();
    return (alurow < 0) ?-1 :MauMeterAlu::get_meter_alu_regs_index(alurow);
  }


  // check things that always must be true
  bool MauSramRowReg::synth2port_fabric_check_static() {
    bool ok = true;
     // check all the unused busses are zero
     for (auto bus : {1,2}) { // unused busses
       for (auto right: {0,1} ) { // left and right
         if ( synth2port_fabric_ctl_.oflo_to_vbus_below(bus,right) ) {
           ok = false;
           RMT_LOG(RmtDebug::error(kRelaxSynth2PortFabricCheck),
                   "MauSramRowReg: synth2port_fabric_ctl_[%d][%s].oflo_to_vbus_below should be false\n",
                   bus,right?"right":"left");
         }
         if ( synth2port_fabric_ctl_.oflo_to_vbus_above(bus,right) ) {
           ok = false;
           RMT_LOG(RmtDebug::error(kRelaxSynth2PortFabricCheck),
                   "MauSramRowReg: synth2port_fabric_ctl_[%d][%s].oflo_to_vbus_above should be false\n",
                   bus,right?"right":"left");
         }
         if ( synth2port_fabric_ctl_.synth2port_connect_below(bus,right) ) {
           ok = false;
           RMT_LOG(RmtDebug::error(kRelaxSynth2PortFabricCheck),
                   "MauSramRowReg: synth2port_fabric_ctl_[%d][%s].synth2port_connect_below should be false\n",
                   bus,right?"right":"left");
         }
         if ( synth2port_fabric_ctl_.synth2port_connect_above(bus,right) ) {
           ok = false;
           RMT_LOG(RmtDebug::error(kRelaxSynth2PortFabricCheck),
                   "MauSramRowReg: synth2port_fabric_ctl_[%d][%s].synth2port_connect_above should be false\n",
                   bus,right?"right":"left");
         }
         if ( synth2port_fabric_ctl_.synth2port_connect_below2above(bus,right)  ) {
           ok = false;
           RMT_LOG(RmtDebug::error(kRelaxSynth2PortFabricCheck),
                   "MauSramRowReg: synth2port_fabric_ctl_[%d][%s].synth2port_connect_below2above should be false\n",
                   bus,right?"right":"left");
         }
       }
     }


     // check overflow,left register is all zero apart from below2above which should match right

     if ( synth2port_fabric_ctl_.oflo_to_vbus_below(0,0) ){
       ok = false;
       RMT_LOG(RmtDebug::error(kRelaxSynth2PortFabricCheck),
               "MauSramRowReg: synth2port_fabric_ctl_[0][left].oflo_to_vbus_below should be false\n");
     }
     if ( synth2port_fabric_ctl_.oflo_to_vbus_above(0,0) ) {
       ok = false;
       RMT_LOG(RmtDebug::error(kRelaxSynth2PortFabricCheck),
               "MauSramRowReg: synth2port_fabric_ctl_[0][left].oflo_to_vbus_above should be false\n");
     }
     if ( synth2port_fabric_ctl_.synth2port_connect_below(0,0) ) {
       ok = false;
       RMT_LOG(RmtDebug::error(kRelaxSynth2PortFabricCheck),
               "MauSramRowReg: synth2port_fabric_ctl_[0][left].synth2port_connect_below should be false\n");
     }
     if ( synth2port_fabric_ctl_.synth2port_connect_above(0,0) ) {
       ok = false;
       RMT_LOG(RmtDebug::error(kRelaxSynth2PortFabricCheck),
               "MauSramRowReg: synth2port_fabric_ctl_[0][left].synth2port_connect_above should be false\n");
     }
     if ( synth2port_fabric_ctl_.synth2port_connect_below2above(0,0) != synth2port_fabric_ctl_.synth2port_connect_below2above(0,1)  ) {
       ok = false;
       RMT_LOG(RmtDebug::error(kRelaxSynth2PortFabricCheck),
               "MauSramRowReg: synth2port_fabric_ctl_[0][left].synth2port_connect_below2above (%d) must be same as right (%d)\n",
               synth2port_fabric_ctl_.synth2port_connect_below2above(0,0), synth2port_fabric_ctl_.synth2port_connect_below2above(0,1));
     }

     // check hbus members that should be zero (all except [stats or oflo][right])

     for (auto bus : {0,1,2}) { // all busses
       for (auto right: {0,1} ) { // left and right
         // check all left registers are zero and also bus 2 on right
         if ( right==0 || (bus==2) ) {
           if ( 0 != synth2port_hbus_members_.synth2port_hbus_members(bus,right) ) {
             ok = false;
             RMT_LOG(RmtDebug::error(kRelaxSynth2PortFabricCheck),
                     "MauSramRowReg: synth2port_hbus_members[%d][%d] should be zero was 0x%x\n",
                     bus,right,
                     synth2port_hbus_members_.synth2port_hbus_members(bus,right));
           }
         }
       }
     }

     return ok;
   }
   //
   bool MauSramRowReg::check_hbus_members( bool is_home_row, uint32_t ram_columns ) {
     int bus = is_home_row ? 0 : 1 ;
     uint32_t hbus_members = synth2port_hbus_members_.synth2port_hbus_members( bus, 1 /*right*/ );
     if ( ram_columns != hbus_members ) {
       RMT_LOG(RmtDebug::error(kRelaxSynth2PortFabricCheck),
               "MauSramRowReg: synth2port_hbus_members[%d][%d]=0x%x but ram columns=0x%x\n",
               bus,1,hbus_members,ram_columns);
       return false;
     }
     return true;
   }
   bool MauSramRowReg::check_data_muxes(bool is_stats, bool is_home_row, bool has_rams ) {
     bool ok=true;

     // check left muxes are off
     ok &= (l_stats_alu_o_mux_select_.l_stats_alu_o_sel_stats_rd_l_i() == 0);
     ok &= (l_meter_alu_o_mux_select_.l_meter_alu_o_sel_meter_rd_l_i() == 0);
     // check meter mux is off (meter data goes on the stats bus
     ok &= (r_meter_alu_o_mux_select_.r_meter_alu_o_sel_meter_rd_r_i() == 0);

     if ( has_rams ) {
       if ( is_home_row ) {
         // both meter and stats use stats rd data
         ok &= (r_stats_alu_o_mux_select_.r_stats_alu_o_sel_stats_rd_r_i() != 0);
         if (!ok) {
           RMT_LOG(RmtDebug::error(kRelaxSynth2PortFabricCheck),
                   "MauSramRowReg: synth2port data muxes wrong for %s_alu home_row "
                   "stats_rd_r=%d stats_rd_l=%d meter_rd_r=%d meter_rd_l=%d\n",
                   is_stats?"stats":"meter",
                   r_stats_alu_o_mux_select_.r_stats_alu_o_sel_stats_rd_r_i(),
                   l_stats_alu_o_mux_select_.l_stats_alu_o_sel_stats_rd_l_i(),
                   r_meter_alu_o_mux_select_.r_meter_alu_o_sel_meter_rd_r_i(),
                   l_meter_alu_o_mux_select_.l_meter_alu_o_sel_meter_rd_l_i());
         }
       }
     }
     else { // doesn't have rams
       bool stats_rd = r_stats_alu_o_mux_select_.r_stats_alu_o_sel_stats_rd_r_i();
       if (stats_rd) {
           // just warn about this redundant programming
           RMT_LOG(RmtDebug::kRmtDebugWarn,
                   "MauSramRowReg: synth2port data muxes unnecessary for %s_alu row with no rams "
                   "stats_rd_r=%d stats_rd_l=%d meter_rd_r=%d meter_rd_l=%d\n",
                   is_stats?"stats":"meter",
                   r_stats_alu_o_mux_select_.r_stats_alu_o_sel_stats_rd_r_i(),
                   l_stats_alu_o_mux_select_.l_stats_alu_o_sel_stats_rd_l_i(),
                   r_meter_alu_o_mux_select_.r_meter_alu_o_sel_meter_rd_r_i(),
                   l_meter_alu_o_mux_select_.l_meter_alu_o_sel_meter_rd_l_i());
       }
     }
     return ok;
   }
   bool MauSramRowReg::synth2port_fabric_check(bool is_stats,int alu, bool is_home_row, bool is_bottom, uint32_t ram_columns) {
     bool ok=true;
     bool has_rams = ram_columns != 0;
     ok &= synth2port_fabric_check_static();

     ok &= check_hbus_members( is_home_row, ram_columns );

     ok &= check_data_muxes( is_stats, is_home_row, has_rams );

     // check the things that depend on where the row is - all in the oflo,right register (0,1)

     auto stats_to_vbus_below            = synth2port_fabric_ctl_.stats_to_vbus_below(0,1);
     auto oflo_to_vbus_below             = synth2port_fabric_ctl_.oflo_to_vbus_below(0,1);
     auto oflo_to_vbus_above             = synth2port_fabric_ctl_.oflo_to_vbus_above(0,1);
     auto synth2port_connect_below       = synth2port_fabric_ctl_.synth2port_connect_below(0,1);
     auto synth2port_connect_above       = synth2port_fabric_ctl_.synth2port_connect_above(0,1);
     auto synth2port_connect_below2above = synth2port_fabric_ctl_.synth2port_connect_below2above(0,1);

     if (kMemoryCoreSplit) {
       if (row_above_middle()) {
         if    (   stats_to_vbus_below            != 0
                || oflo_to_vbus_below             != 0
                || synth2port_connect_below       != 0
                || synth2port_connect_below2above != 0
               ) {
           ok = false;
           RMT_LOG(RmtDebug::error(kRelaxSynth2PortFabricCheck),
                   "MauSramRowReg: synth2port_fabric_ctl invalid config for row above middle "
                   "given memory core is split!  stats_to_vbus_below=%d oflo_to_vbus_below=%d "
                   "synth2port_connect_below=%d synth2port_connect_below2above=%d\n",
                   stats_to_vbus_below,oflo_to_vbus_below,synth2port_connect_below,
                   synth2port_connect_below2above);
         }
       } else if (row_below_middle()) {
         if    (   oflo_to_vbus_above             != 0
                || synth2port_connect_above       != 0
                || synth2port_connect_below2above != 0
               ) {
           ok = false;
           RMT_LOG(RmtDebug::error(kRelaxSynth2PortFabricCheck),
                   "MauSramRowReg: synth2port_fabric_ctl invalid config for row below middle "
                   "given memory core is split!  oflo_to_vbus_above=%d "
                   "synth2port_connect_above=%d synth2port_connect_below2above=%d\n",
                   oflo_to_vbus_above,synth2port_connect_above,
                   synth2port_connect_below2above);
         }
       }
     }

     if      (  is_home_row &&  is_bottom ) {   // home row no overflow
       Synth2PortRowUsedAs this_uses_as = kHomeRowNoOverflow;
       if      (   stats_to_vbus_below            == 0
                && oflo_to_vbus_below             == 0
                && oflo_to_vbus_above             == 0
                && synth2port_connect_below       == 0
                && synth2port_connect_above       == 0
                && synth2port_connect_below2above == 0
                   ) {
         // other alu allowed = none
         ok &= synth2port_check_other_alu( is_stats, alu, this_uses_as, { } );
       }
       else if (   stats_to_vbus_below            == 0
                && oflo_to_vbus_below             == 1
                && oflo_to_vbus_above             == 1
                && synth2port_connect_below       == 1
                && synth2port_connect_above       == 1
                && synth2port_connect_below2above == 1
                   ) {
         // other alu allowed = middle, no rams
         ok &= synth2port_check_other_alu( is_stats, alu, this_uses_as, { kMiddleWithRams } );
       }
       else if (   stats_to_vbus_below            == 0
                && oflo_to_vbus_below             == 1
                && oflo_to_vbus_above             == 1
                && synth2port_connect_below       == 0
                && synth2port_connect_above       == 0
                && synth2port_connect_below2above == 1
                   ) {
         // other alu allowed = middle, no rams
         ok &= synth2port_check_other_alu( is_stats, alu, this_uses_as, { kMiddleNoRams } );
       }
       else if (   stats_to_vbus_below            == 0
                && oflo_to_vbus_below             == 0
                && oflo_to_vbus_above             == 1
                && synth2port_connect_below       == 0
                && synth2port_connect_above       == 1
                && synth2port_connect_below2above == 0
                   ) {
         // other alu allowed = bottom only
         ok &= synth2port_check_other_alu( is_stats, alu, this_uses_as, { kBottomOnly } );
       }
       else {
         ok = false;
         RMT_LOG(RmtDebug::error(kRelaxSynth2PortFabricCheck),
                 "MauSramRowReg: synth2port_fabric_ctl invalid config for home row without overflow: "
                 "stats_to_vbus_below=%d oflo_to_vbus_below=%d oflo_to_vbus_above=%d "
                 "synth2port_connect_below=%d synth2port_connect_above=%d synth2port_connect_below2above=%d\n",
                 stats_to_vbus_below,oflo_to_vbus_below,oflo_to_vbus_above,synth2port_connect_below,synth2port_connect_above,
                 synth2port_connect_below2above);
       }
     }
     else if (  is_home_row && !is_bottom ) {   // home row with overflow

       Synth2PortRowUsedAs this_uses_as = kHomeRowWithOverflow;
       if      (   stats_to_vbus_below            == 1
                && oflo_to_vbus_below             == 0
                && oflo_to_vbus_above             == 0
                && synth2port_connect_below       == 1
                && synth2port_connect_above       == 0
                && synth2port_connect_below2above == 0
                   ) {
         // other alu allowed = none
         ok &= synth2port_check_other_alu( is_stats, alu, this_uses_as, { } );
       }
       else if (   stats_to_vbus_below            == 1
                && oflo_to_vbus_below             == 0
                && oflo_to_vbus_above             == 1
                && synth2port_connect_below       == 1
                && synth2port_connect_above       == 1
                && synth2port_connect_below2above == 0
                   ) {
         // other alu allowed = bottom only
         ok &= synth2port_check_other_alu( is_stats, alu, this_uses_as, { kBottomOnly } );
       }
       else {
         ok = false;
         RMT_LOG(RmtDebug::error(kRelaxSynth2PortFabricCheck),
                 "MauSramRowReg: synth2port_fabric_ctl invalid config for home row with overflow: "
                 "stats_to_vbus_below=%d oflo_to_vbus_below=%d oflo_to_vbus_above=%d "
                 "synth2port_connect_below=%d synth2port_connect_above=%d synth2port_connect_below2above=%d\n",
                 stats_to_vbus_below,oflo_to_vbus_below,oflo_to_vbus_above,synth2port_connect_below,synth2port_connect_above,
                 synth2port_connect_below2above);
       }
     }
     else if ( !is_home_row &&  is_bottom ) {   // bottom only
       Synth2PortRowUsedAs this_uses_as = kBottomOnly;
       if      (   stats_to_vbus_below            == 0
                && oflo_to_vbus_below             == 0
                && oflo_to_vbus_above             == 1
                && synth2port_connect_below       == 0
                && synth2port_connect_above       == 1
                && synth2port_connect_below2above == 0
                   ) {
         // other alu allowed = none or homerow no overflow
         ok &= synth2port_check_other_alu( is_stats, alu, this_uses_as, { kNone, kHomeRowNoOverflow } );
       }
       else if (   stats_to_vbus_below            == 1
                && oflo_to_vbus_below             == 0
                && oflo_to_vbus_above             == 1
                && synth2port_connect_below       == 1
                && synth2port_connect_above       == 1
                && synth2port_connect_below2above == 0
                   ) {
         // other alu allowed = homerow with overflow
         ok &= synth2port_check_other_alu( is_stats, alu, this_uses_as, { kHomeRowWithOverflow } );
       }
       else {
         ok = false;
         RMT_LOG(RmtDebug::error(kRelaxSynth2PortFabricCheck),
                 "MauSramRowReg: synth2port_fabric_ctl invalid config for bottom row: "
                 "stats_to_vbus_below=%d oflo_to_vbus_below=%d oflo_to_vbus_above=%d "
                 "synth2port_connect_below=%d synth2port_connect_above=%d synth2port_connect_below2above=%d\n",
                 stats_to_vbus_below,oflo_to_vbus_below,oflo_to_vbus_above,synth2port_connect_below,synth2port_connect_above,
                 synth2port_connect_below2above);
       }
     }
     else if ( !is_home_row && !is_bottom ) {
       if ( has_rams ) {                        // middle with rams

         Synth2PortRowUsedAs this_uses_as = kMiddleWithRams;
         if      (   stats_to_vbus_below               == 0
                     && oflo_to_vbus_below             == 1
                     && oflo_to_vbus_above             == 1
                     && synth2port_connect_below       == 1
                     && synth2port_connect_above       == 1
                     && synth2port_connect_below2above == 1
                     ) {
           // other alu allowed = none or homerow no overflow
           ok &= synth2port_check_other_alu( is_stats, alu, this_uses_as, { kNone, kHomeRowNoOverflow } );
         }
         else {
           ok = false;
           RMT_LOG(RmtDebug::error(kRelaxSynth2PortFabricCheck),
                   "MauSramRowReg: synth2port_fabric_ctl invalid config for middle row with rams "
                   "stats_to_vbus_below=%d oflo_to_vbus_below=%d oflo_to_vbus_above=%d "
                   "synth2port_connect_below=%d synth2port_connect_above=%d synth2port_connect_below2above=%d\n",
                   stats_to_vbus_below,oflo_to_vbus_below,oflo_to_vbus_above,synth2port_connect_below,synth2port_connect_above,
                   synth2port_connect_below2above);
         }
       }
       else {                                   // middle no rams
         Synth2PortRowUsedAs this_uses_as = kMiddleNoRams;
         if      (   stats_to_vbus_below               == 0
                     && oflo_to_vbus_below             == 1
                     && oflo_to_vbus_above             == 1
                     && synth2port_connect_below       == 0
                     && synth2port_connect_above       == 0
                     && synth2port_connect_below2above == 1
                     ) {
           // other alu allowed = none or homerow no overflow
           ok &= synth2port_check_other_alu( is_stats, alu, this_uses_as, { kNone, kHomeRowNoOverflow } );
         }
         else if    (   stats_to_vbus_below            == 0
                     && oflo_to_vbus_below             == 1
                     && oflo_to_vbus_above             == 1
                     && synth2port_connect_below       == 1  // redundant
                     && synth2port_connect_above       == 1  // redundant
                     && synth2port_connect_below2above == 1
                     ) {
           // Programming the two bits above is redundant but allowed, so just warn about it.
           // other alu allowed = none or homerow no overflow
           ok &= synth2port_check_other_alu( is_stats, alu, this_uses_as, { kNone, kHomeRowNoOverflow } );
           RMT_LOG(RmtDebug::kRmtDebugWarn,
                   "MauSramRowReg: synth2port_fabric_ctl unnecessary config for middle row no rams "
                   "stats_to_vbus_below=%d oflo_to_vbus_below=%d oflo_to_vbus_above=%d "
                   "synth2port_connect_below=%d synth2port_connect_above=%d synth2port_connect_below2above=%d\n",
                   stats_to_vbus_below,oflo_to_vbus_below,oflo_to_vbus_above,synth2port_connect_below,synth2port_connect_above,
                   synth2port_connect_below2above);
         }
         else {
           ok = false;
           RMT_LOG(RmtDebug::error(kRelaxSynth2PortFabricCheck),
                   "MauSramRowReg: synth2port_fabric_ctl invalid config for middle row no rams "
                   "stats_to_vbus_below=%d oflo_to_vbus_below=%d oflo_to_vbus_above=%d "
                   "synth2port_connect_below=%d synth2port_connect_above=%d synth2port_connect_below2above=%d\n",
                   stats_to_vbus_below,oflo_to_vbus_below,oflo_to_vbus_above,synth2port_connect_below,synth2port_connect_above,
                   synth2port_connect_below2above);
         }
       }
     }

     ok &= synth2port_ctl_check();

     //synth2port_hbus_members_.synth2port_hbus_members

     if (!ok && !kRelaxSynth2PortFabricCheck) { THROW_ERROR(-2); }
     return ok;
   }


   bool MauSramRowReg::synth2port_check_other_alu(bool is_stats, int alu, Synth2PortRowUsedAs this_alu_used_as,
                                                  std::vector< Synth2PortRowUsedAs > other_alu_can_use_as ) {

     alu += is_stats ? 0 : MauDefs::kNumStatsAlus;

     Synth2PortUsedByAlu& this_alu  = synth2port_used_by_alu_[0];
     Synth2PortUsedByAlu& other_alu = synth2port_used_by_alu_[1];

     if ( synth2port_used_by_alu_[0].used_ && synth2port_used_by_alu_[0].alu_ == alu ) {
       this_alu  = synth2port_used_by_alu_[0];
       other_alu = synth2port_used_by_alu_[1];
     }
     else if ( synth2port_used_by_alu_[1].used_ && synth2port_used_by_alu_[1].alu_ == alu ) {
       this_alu  = synth2port_used_by_alu_[1];
       other_alu = synth2port_used_by_alu_[0];
     }
     else if ( !synth2port_used_by_alu_[0].used_ ) {
       this_alu  = synth2port_used_by_alu_[0];
       other_alu = synth2port_used_by_alu_[1];
     }
     else if ( !synth2port_used_by_alu_[1].used_ ) {
       this_alu  = synth2port_used_by_alu_[1];
       other_alu = synth2port_used_by_alu_[0];
     }
     else {
       RMT_ASSERT( 0 ); // it shouldn't be possible to use the fabric by more than 2 alus
     }

     if ( this_alu.used_ ) {
       if ( this_alu.used_as_ != this_alu_used_as ) {
         RMT_LOG(RmtDebug::error(kRelaxSynth2PortFabricCheck),
                 "MauSramRowReg: synth2port alu %d used as %d and %d\n",
                 this_alu.alu_,this_alu.used_as_, this_alu_used_as);
         RMT_ASSERT( 0 ); // shouldn't be possible to used the row as different types for same alu
       }
     }
     else {
       this_alu.used_ = true;
       this_alu.alu_  = alu;
       this_alu.used_as_ = this_alu_used_as;
     }

     if ( !other_alu.used_ ) {
       return true;
     }

     for (auto allowed_use : other_alu_can_use_as ) {
       if ( other_alu.used_as_ == allowed_use )
         return true;
     }

     // TODO: improve this message!
     RMT_LOG(RmtDebug::error(kRelaxSynth2PortFabricCheck),
             "MauSramRowReg: synth2port fabric used in incompatible ways by alus %d and %d\n",
             this_alu.alu_,other_alu.alu_);

     return false;
   }


   bool MauSramRowReg::synth2port_vpn_valid(int vpn) {
     bool vpn_valid = mau_sram_row_chip_reg_.synth2port_vpn_valid(vpn);
     if (!vpn_valid) {
       // XXX: Check if any synth2port rams used this row or below
       // (these checks suggested by MikeF 18th Jan 2021)
       bool used_for_synth2port = (
           (synth2port_hbus_members_.synth2port_hbus_members(0,0) != 0) ||
           (synth2port_hbus_members_.synth2port_hbus_members(0,1) != 0) ||
           (synth2port_fabric_ctl_.stats_to_vbus_below(0,0) != 0) ||
           (synth2port_fabric_ctl_.stats_to_vbus_below(0,1) != 0) );
       // If no synth2port in use, we override and say VPN *is* valid
       // but log a warning (the H/W will flag an interrupt too)
       // otherwise an error
       if (!used_for_synth2port) vpn_valid = true;
       //RMT_LOG( (used_for_synth2port) ?RmtDebug::error(kRelaxSynth2PortFabricCheck) :RmtDebug::warn(),
       // XXX: Downgrade error here to warn
       RMT_LOG( RmtDebug::warn(),
                "MauSramRowReg: VPN=%d outside synth2port_vpn_base/lim - %s "
               "synth2port RAMs in use this row or below\n",
                vpn, (used_for_synth2port) ?"*NOT* OK as" :"OK since no" );
     }
     return vpn_valid;
   }
   bool MauSramRowReg::synth2port_ctl_check() {
     // First of all refresh knowledge of maprams on this row
     // Figure out whether maprams are synth2port/color and whether they use oflo
     //
     mapram_config_update();

     // XXX: check synth2port_ctl_ config
     //
     // MikeF said 15th April 2019:
     // synth2port_enable - must be set to 1 if there are any overflow or home row
     //                     synth2port RAMs on the row OR if there are any oflo color RAMs on the row.
     // synth2port_mapram_color - should be set for each color RAM
     //
     // NB. Must shift exp_color_maprams_ such that bit0 => mapram[6], bit1 => mapram[7] etc to match CSR
     bool exp_en =  ((synth2port_home_mapram_mask_|synth2port_oflo_mapram_mask_|color_oflo_mapram_mask_) != 0);
     uint16_t exp_color_maprams = (color_home_mapram_mask_|color_oflo_mapram_mask_) >> (kMapramColumns/2);

     bool en = (synth2port_ctl_.synth2port_enable() == 1);
     uint16_t color_maprams = static_cast<uint16_t>( synth2port_ctl_.synth2port_mapram_color() );

     if ((exp_color_maprams == color_maprams) && (exp_en == en)) return true;

     RMT_LOG(RmtDebug::error(kRelaxSynth2PortFabricCheck),
             "MauSramRowReg: invalid synth2port_ctl.{synth2port_enable|synth2port_mapram_color} config "
             "synth2port_enable=%c (expected %c) synth2port_mapram_color=0x%02x (expected 0x%02x)\n",
             en?'T':'F', exp_en?'T':'F', color_maprams, exp_color_maprams);
     return false;
   }
   void MauSramRowReg::mapram_config_update() {
     if (curr_seq_ == pending_seq_) return; // No change so bail
     RMT_ASSERT(curr_seq_ <= pending_seq_);

     // Mapram config has changed so reprocess all maprams on row
     // NB we stay in while loop until config changes stop
     //
     Mau *mau = mau_sram_row_->mau();
     RMT_ASSERT(mau != nullptr);
     uint16_t mask[4];
     mask[0] = mask[1] = mask[2] = mask[3] = 0;

     while (curr_seq_ < pending_seq_) {
       curr_seq_ = pending_seq_;

       mask[0] = mask[1] = mask[2] = mask[3] = 0;
       for (int col = kMapramColumns/2; col < kMapramColumns; col++) {
         MauMapram *mapram = mau->mapram_lookup(row_index_, col);
         if (mapram != nullptr) {
           if      (mapram->is_synth2port_home_mapram()) mask[0] |= 1<<col;
           else if (mapram->is_synth2port_oflo_mapram()) mask[1] |= 1<<col;
           else if (mapram->is_color_home_mapram()) mask[2] |= 1<<col;
           else if (mapram->is_color_oflo_mapram()) mask[3] |= 1<<col;
         }
       }
     }
     // Now update member vars
     synth2port_home_mapram_mask_ = mask[0];
     synth2port_oflo_mapram_mask_ = mask[1];
     color_home_mapram_mask_ = mask[2];
     color_oflo_mapram_mask_ = mask[3];
   }
   void MauSramRowReg::mapram_change_callback(int col) {
     RMT_ASSERT((col >= (kMapramColumns/2)) && (col < kMapramColumns));
     pending_seq_++; // Processing happens later in mapram_config_update
   }

}
