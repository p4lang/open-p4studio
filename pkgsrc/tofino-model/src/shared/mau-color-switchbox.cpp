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
#include <rmt-types.h>
#include <rmt-object-manager.h>
#include <mau-color-switchbox.h>
#include <mau-sram-row.h>

namespace MODEL_CHIP_NAMESPACE {

MauColorSwitchbox::MauColorSwitchbox(RmtObjectManager *om, int pipeIndex, int mauIndex,
                                     int rowIndex, Mau *mau, MauSramRow *mauSramRow) :
    MauObject(om, pipeIndex, mauIndex, RmtTypes::kRmtTypeMauColorSwitchbox, rowIndex, mau),
    row_index_(rowIndex),
    r_color0_mux_(chip_index(),pipeIndex,mauIndex,rowIndex),
    r_color1_mux_(chip_index(),pipeIndex,mauIndex,rowIndex),
    t_oflo_color_o_mux_(chip_index(),pipeIndex,mauIndex,rowIndex),
    muxes_{{ &r_color0_mux_,
             &r_color1_mux_,
             &t_oflo_color_o_mux_ }}
{
  reset_resources();
}
MauColorSwitchbox::~MauColorSwitchbox() {  }

void MauColorSwitchbox::get_output_bus( int which_bus, uint8_t *output, bool *was_driven ) {

  RMT_ASSERT( which_bus<=kOverflowIndex );
  MauColorSwitchboxMuxBase* mux = muxes_[which_bus];

  // This is super simple now: the two normal bus muxes can grab from the right_color_bus
  //   or overflow bus. The overlow mux can only grab from overflow

  if (mux->get_sel_color_i()) {
    if ( ( which_bus != kOverflowIndex ) ) { // overflow can't get from color bus
      *output |= right_color_bus_;
      *was_driven |= right_color_bus_was_set_;
    }
  }

  if ( mux->get_sel_oflo_color_r_i()) {
    *output |= right_overflow_bus_;
    *was_driven |= right_overflow_bus_was_set_;
  }


  RMT_LOG_OBJ(mau(), RmtDebug::verbose(),
              "MauColorSwitchbox::get_output_bus row=%d which_bus=%d sel_color_i=%d sel_oflo_color_r_i=%d "
              " r_bus=%d r_ofl_bus=%d output=%d\n",row_index_,which_bus,mux->get_sel_color_i(),
              mux->get_sel_oflo_color_r_i(),right_color_bus_,right_overflow_bus_,*output);
}


bool MauColorSwitchbox::check_same_alu(int *first_mapram_col, int *first_alu_index,
                                       int mapram_col, int alu_index, const char *bus_name) {
  RMT_ASSERT((first_mapram_col != nullptr) && (first_alu_index != nullptr));
  // XXX: check if >1 mapram driving homerow/overflow color bus that they
  // both use same ALU
  bool same = true;
  if (alu_index >= 0) {
    if (*first_alu_index < 0) {
      // OK - track as first color ALU
      *first_alu_index = alu_index;
      *first_mapram_col = mapram_col;
    } else if (alu_index != *first_alu_index) {
      // Not OK - diff ALUs - complain
      RMT_LOG_OBJ(mau(), RmtDebug::error(kRelaxColorBusDiffAluCheck),
                  "MauColorSwitchbox::set_right_%s_bus(R=%d) "
                  "Maprams %d,%d drive %s color bus but use diff ALUs (%d,%d)\n",
                  bus_name, row_index_, *first_mapram_col, mapram_col, bus_name,
                  *first_alu_index, alu_index);
      same = false;
    }
  }
  return same;
}

void MauColorSwitchbox::set_right_color_bus(uint8_t v, int mapram_col, int alu_index) {
  right_color_bus_was_set_ = true;
  right_color_bus_ |= v;
  RMT_LOG_OBJ(mau(), RmtDebug::verbose(),
              "MauColorSwitchbox::set_right_color_bus(%d) row=%d bus=%d\n",v,
              row_index_,right_color_bus_);
  // check that someone is set up to take from the bus
  int taken=0;
  for (auto const &m : muxes_)
    if ( m->get_sel_color_i() )
      ++taken;

  if (taken==0) {
    RMT_LOG_OBJ(mau(), RmtDebug::error(),
                "MauColorSwitchbox::set_right_color_bus: nothing set to take from this bus\n");
  }
  // if multiple maprams driving homerow bus check they consume same ALU
  (void)check_same_alu(&first_homerow_mapram_col_, &first_homerow_alu_index_,
                       mapram_col, alu_index, "homerow");
}

void MauColorSwitchbox::set_right_overflow_bus(uint8_t v, int mapram_col, int alu_index) {
  // TODO: should we apply same check as in set_right_color_bus above?
  right_overflow_bus_was_set_ = true;
  right_overflow_bus_ |= v;
  RMT_LOG_OBJ(mau(), RmtDebug::verbose(),
              "MauColorSwitchbox::set_right_overflow_bus(%d) row=%d bus=%d\n",v,
              row_index_,right_overflow_bus_);

  // check that someone is set up to take from the bus
  int taken=0;
  for (auto const &m : muxes_)
    if ( m->get_sel_oflo_color_r_i() )
      ++taken;

  if (taken==0) {
    RMT_LOG_OBJ(mau(), RmtDebug::error(),
                "MauColorSwitchbox::set_right_overflow_bus: nothing set to take from this bus\n");
  }

  // check not driving a disallowed vertical overflow bus. In tofino the vertical
  //  overflow bus is disallowed in the top half because it doesn't go anywhere
  //  In JBay all vertical overflow busses are disallowed due to memory core split
  // See XXX for details
  bool disallow_overflow = is_jbay_or_later() ?
      true :
      row_index_ >= 4;   // top half

  if ( disallow_overflow && muxes_[kOverflowIndex]->get_sel_oflo_color_r_i() ) {
    RMT_LOG_OBJ(mau(), RmtDebug::error(),
                "MauColorSwitchbox::set_right_overflow_bus: driving to disallowed overflow bus\n");
  }
  // if multiple maprams driving overflow bus check they consume same ALU
  (void)check_same_alu(&first_overflow_mapram_col_, &first_overflow_alu_index_,
                       mapram_col, alu_index, "overflow");
}

void MauColorSwitchbox::reset_resources() {
  right_color_bus_    = 0;
  right_overflow_bus_ = 0;
  right_color_bus_was_set_    = false;
  right_overflow_bus_was_set_ = false;
  first_homerow_mapram_col_ = -1;
  first_homerow_alu_index_ = -1;
  first_overflow_mapram_col_ = -1;
  first_overflow_alu_index_ = -1;
}

}
