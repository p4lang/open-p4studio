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

#ifndef _SHARED_MAU_COLOR_SWITCHBOX_MUX_
#define _SHARED_MAU_COLOR_SWITCHBOX_MUX_

#include <register_adapters.h>

namespace MODEL_CHIP_NAMESPACE {

// These classes just give a way of hiding a mux's associated register
//   behind a common interface even though the registers have different field
//   names for each mux. The register class and accessor methods for the
//   fields are specified using using a template.

class MauColorSwitchboxMuxBase {
 public:
  MauColorSwitchboxMuxBase() {}
  ~MauColorSwitchboxMuxBase() {}

  virtual  uint8_t& get_sel_oflo_color_r_i() = 0;
  virtual uint8_t& get_sel_color_i() = 0;
};

// template takes:
//   register classname and then member function names
//   to access the various fields (as these names are different for each
//   mux)
template < typename REG_T,
           uint8_t& ( REG_T::*SEL_OFLO_COLOR_R_I )(),
           uint8_t& ( REG_T::*SEL_COLOR_I )() >
class MauColorSwitchboxMux : public MauColorSwitchboxMuxBase {

 public:
  MauColorSwitchboxMux(int chip, int pipe, int mau, int row)
      : control_reg_(default_adapter(control_reg_, chip, pipe, mau, row )) {
    control_reg_.reset();
  }
  ~MauColorSwitchboxMux() {}

  uint8_t& get_sel_oflo_color_r_i() { return ( control_reg_.*SEL_OFLO_COLOR_R_I )(); }
  uint8_t& get_sel_color_i() { return ( control_reg_.*SEL_COLOR_I )(); }

 private:
  REG_T control_reg_;

};

// for overflow muxes which don't have sel_color
template < typename REG_T,
           uint8_t& ( REG_T::*SEL_OFLO_COLOR_R_I )() >
class MauColorSwitchboxMuxO : public MauColorSwitchboxMuxBase {

 public:
  MauColorSwitchboxMuxO(int chip, int pipe, int mau, int row)
      : control_reg_(default_adapter(control_reg_, chip, pipe, mau, row )),
        zero(0) {
    control_reg_.reset();
  }
  ~MauColorSwitchboxMuxO() {}

  uint8_t& get_sel_oflo_color_r_i() { return ( control_reg_.*SEL_OFLO_COLOR_R_I )(); }
  uint8_t& get_sel_color_i() { return zero; }

 private:
  REG_T control_reg_;
  uint8_t zero;
};

}

#endif // _SHARED_MAU_COLOR_SWITCHBOX_MUX_

