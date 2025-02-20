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

#ifndef _SHARED_MAU_COLOR_SWITCHBOX_
#define _SHARED_MAU_COLOR_SWITCHBOX_

#include <cstdint>
#include <rmt-defs.h>
#include <mau-defs.h>
#include <mau-object.h>
#include <mau-color-switchbox-mux.h>

#include <register_includes/r_color0_mux_select.h>
#include <register_includes/r_color1_mux_select.h>
#include <register_includes/t_oflo_color_o_mux_select.h>

// Note: This was implemented after oflo2 was removed from the design, so it does not
//   implement any oflo2 functionality.

namespace MODEL_CHIP_NAMESPACE {

class Mau;
class MauSramRow;

class MauColorSwitchbox : public MauObject {

 public:
  MauColorSwitchbox(RmtObjectManager *om, int pipeIndex, int mauIndex,
                    int rowIndex, Mau *mau, MauSramRow *mauSramRow);
  ~MauColorSwitchbox();

  static bool kRelaxColorBusDiffAluCheck; // Defined in rmt-config.cpp

  static constexpr int kOverflowIndex = 2;
  // Get one of the output color buses, if this switchbox is in
  //   rows 0-3 then the output busses go up if 4-7 they go down.
  // There are 2 'normal' busses and one overflow bus
  void get_output_bus( int which_bus, uint8_t *output, bool *was_driven );

  void reset_resources();

  // note: setters OR into the current value, call reset_resources() to set everything to zero
  void    set_right_color_bus(uint8_t v, int mapram_col, int alu_index);
  uint8_t get_right_color_bus() { return right_color_bus_; }
  void    set_right_overflow_bus(uint8_t v, int mapram_col, int alu_index);
  uint8_t get_right_overflow_bus() { return right_overflow_bus_; }

 private:
  bool check_same_alu(int *first_mapram_col, int *first_alu_index,
                      int mapram_col, int alu_index, const char *bus_name);

 private:
  const int row_index_;
  MauColorSwitchboxMux< register_classes::RColor0MuxSelect,
              &register_classes::RColor0MuxSelect::r_color0_sel_oflo_color_r_i,
              &register_classes::RColor0MuxSelect::r_color0_sel_color_r_i >       r_color0_mux_;
  MauColorSwitchboxMux< register_classes::RColor1MuxSelect,
              &register_classes::RColor1MuxSelect::r_color1_sel_oflo_color_r_i,
              &register_classes::RColor1MuxSelect::r_color1_sel_color_r_i >       r_color1_mux_;
  MauColorSwitchboxMuxO< register_classes::TOfloColorOMuxSelect,
              &register_classes::TOfloColorOMuxSelect::t_oflo_color_o_mux_select> t_oflo_color_o_mux_;

  std::array< MauColorSwitchboxMuxBase*, kOverflowIndex+1 > muxes_;

  uint8_t right_color_bus_ = 0;
  uint8_t right_overflow_bus_ = 0;
  bool right_color_bus_was_set_ = false;
  bool right_overflow_bus_was_set_ = false;
  int first_homerow_mapram_col_ = -1;
  int first_homerow_alu_index_ = -1;
  int first_overflow_mapram_col_ = -1;
  int first_overflow_alu_index_ = -1;
};
}

#endif // _SHARED_MAU_COLOR_SWITCHBOX_
