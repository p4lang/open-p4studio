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

#include <utests/test_util.h>
#include <mau.h>
#include <iostream>
#include <string>
#include <array>
#include <cassert>

#include "gtest.h"
#include "cmp_helper.h"
#include <rmt-object-manager.h>
#include <model_core/model.h>
#include <mau-stateful-alu.h>

extern std::unique_ptr<model_core::Model> GLOBAL_MODEL;

namespace MODEL_CHIP_TEST_NAMESPACE {

struct salu_cfg {
  // Program salu_mathunit_ctl
  int salu_mathunit_output_scale;
  int salu_mathunit_exponent_invert;
  int salu_mathunit_exponent_shift;

  // salu_const_regfile
  int salu_const_regfile[4];

  // salu_mathtable
  int salu_mathtable[4];

  // salu_instr_output_alu [instruction index] - Tofino compatible
  int salu_output_cmpfn[4];
  int salu_output_asrc[4];

  // JBay version of above, used in JBay if tofino_backwards_compatible is false
  int salu_output_cmpfn_jbay[4][4];
  int salu_output_asrc_jbay[4][4];


  // salu_instr_state_alu [instruction index][0:2-lo, 1:1-lo, 2:2-hi, 3:1-hi]
  int salu_const_src[4][4];
  int salu_regfile_const[4][4];
  int salu_bsrc_phv_index[4][4];
  int salu_bsrc_phv[4][4];
  int salu_asrc_memory_index[4][4];
  int salu_asrc_memory[4][4];
  int salu_op[4][4];
  int salu_arith[4][4];
  int salu_pred[4][4];

  // salu_instr_common [instruction index]
  int salu_alu2_lo_bsrc_math[4];
  int salu_alu2_lo_math_src[4];
  int salu_datasize[4];
  int salu_op_dual[4];
  // JBay only
  int salu_divide_enable[4];
  int salu_lmatch_sbus_listen[4];
  int salu_match_sbus_listen[4];
  int salu_minmax_mask_ctl[4];
  int salu_minmax_ctl[4];
  int salu_minmax_enable[4];
  int salu_minmax_postinc_enable[4];
  int salu_minmax_postdec_enable[4];
  int salu_minmax_postmod_value[4];
  int salu_sbus_match_comb[4];
  int salu_sbus_learn_comb[4];

  // salu_instr_tmatch_alu [instr][alu0,1] - JBay only
  int salu_tmatch_vld_ctl[4][2];
  int salu_tmatch_invert[4][2];

  // tmatch_mask [alu0,1] - JBay only
  uint64_t tmatch_mask[2];


  // salu_instr_cmp_alu [instruction index] [0:cmp-lo 1:cmp-hi for Tofino, 4 for JBay]
  int salu_cmp_const_src[4][4];
  int salu_cmp_regfile_const[4][4];
  int salu_cmp_bsrc_input[4][4];
  int salu_cmp_bsrc_sign[4][4];
  int salu_cmp_bsrc_enable[4][4];
  int salu_cmp_asrc_input[4][4];
  int salu_cmp_asrc_sign[4][4];
  int salu_cmp_asrc_enable[4][4];
  int salu_cmp_opcode[4][4];

  // JBay only
  int salu_cmp_asrc_mask_enable[4][4]; // TODO: add a test for this
  int salu_cmp_bsrc_mask_enable[4][4]; // TODO: add a test for this
  int salu_cmp_mask_input[4][4];       // TODO: add a test for this
  int salu_cmp_tmatch_enable[4][2];    // only on ALU0,1
  int salu_cmp_sbus_or[4][4];
  int salu_cmp_sbus_and[4][4];
  int salu_cmp_sbus_invert[4][4];

  uint32_t stateful_clear_action_output;

  // Tofino 2 cmp alus -> 0xF . JBay 4 cmp alus -> 0xFFFF
  constexpr static int kAllCmp = (1<<(1<<MODEL_CHIP_NAMESPACE::MauDefs::kStatefulAluCmpAlus))-1;
};




  using namespace MODEL_CHIP_NAMESPACE;

  int input_to_table_idx(int val, bool normalize, bool even_normilazation=true) {
    if (!val) return 0;

    // Get the leading ones position of a 32-bit input
    int l1p =  31 - __builtin_clz (val);
    if (normalize) {
      // Round up to the next even (or odd) bit depending on the type of
      // normilazation requested.
      if ((l1p & 1) && even_normilazation) {
        l1p += 1;
      } else if (!(l1p & 1) && !even_normilazation) {
        l1p += 1;
      }
    }

    // Extract the 4 bit index from val starting at the modified leading ones
    // position.  If the leading ones position is less than 3, meaning there
    // are not four bits to extract then pad the lsbs with zero.
    // This is accomplished by left shifting the value so that the leading ones
    // position is the msb (drops any high bits and pads the lsbs with zero)
    // and then right shifting so that the result is a four bit value with the
    // leading ones position in bit three.
    int index = static_cast<int>((static_cast<uint64_t>(val) << (63-l1p)) >> 60);
    return index;
  }

  void math_scale(const std::vector<double> &vals, int *scale, bool *scale_positive) {
    double max_val = 0.0;
    for (auto x : vals) {
      max_val = x >= max_val ? x : max_val;
    }

    *scale = 1;
    *scale_positive = true;
    if (0.5 > max_val) {
      while (!(0.5 <= (max_val*static_cast<double>(*scale)))) {
        *scale = *scale * 2;
      }
    } else if (max_val >= 1.0) {
      while (!(1.0 > (max_val/static_cast<double>(*scale)))) {
        *scale = *scale * 2;
      }
      *scale_positive = false;
    }
  }

  void apply_math_scale(const std::vector<double> &vals, std::vector<double> &scaled_vals, int scale, bool positive) {
    for (std::vector<double>::size_type i=0; i<vals.size(); ++i) {
      if (scale == 1) {
        scaled_vals[i] = vals[i];
      } else if (positive) {
        scaled_vals[i] = vals[i] * static_cast<double>(scale);
      } else {
        scaled_vals[i] = vals[i] / static_cast<double>(scale);
      }
    }
  }

  int get_math_table_lut(int c, bool is_reciprical, bool is_sqrd, bool is_sqrt, std::vector<int> &lut_contents) {
    double max_val = 0;
    int input_size = 32;

    double C = static_cast<double>(c);
    std::vector<double> func_output(input_size);
    std::vector<int> tbl_idx(input_size);
    bool normalize = is_sqrt;
    for (int i=1; i<input_size; ++i) {
      tbl_idx[i] = input_to_table_idx(i, normalize);
      if (is_reciprical && is_sqrd) { // c / (x^2)
        func_output[i] = C/static_cast<double>(tbl_idx[i]*tbl_idx[i]);
      } else if (is_reciprical && is_sqrt) { // c / sqrt(x)
        double x = static_cast<double>(tbl_idx[i]);
        if (normalize) func_output[i] = (C/sqrt(x/2) + C/sqrt((x+1)/2)) / 2.0;
        else           func_output[i] = (C/sqrt(x) + C/sqrt(x+1)) / 2.0;
      } else if (is_reciprical) { // c / x
        func_output[i] = C/static_cast<double>(tbl_idx[i]);
      } else if (is_reciprical && is_sqrd) { // c * x^2
        func_output[i] = C*static_cast<double>(tbl_idx[i]*tbl_idx[i]);
      } else if (is_reciprical && is_sqrt) { // c * sqrt(x)
        double x = static_cast<double>(tbl_idx[i]);
        if (normalize) func_output[i] = (C*sqrt(x/2) + C*sqrt((x+1)/2)) / 2.0;
        else           func_output[i] = (C*sqrt(x) + C*sqrt(x+1)) / 2.0;
      } else { // c * x
        func_output[i] = C*static_cast<double>(tbl_idx[i]);
      }
      max_val = func_output[i] > max_val ? func_output[i] : max_val;
    }
    int scale = 1;
    bool scale_positive = true;
    math_scale(func_output, &scale, &scale_positive);

    std::vector<double> scaled_tbl_entry = func_output;
    apply_math_scale(func_output, scaled_tbl_entry, scale, scale_positive);

    for (std::vector<double>::size_type i=0; i<tbl_idx.size(); ++i) {
      lut_contents[tbl_idx[i]] = static_cast<int>(scaled_tbl_entry[i] * 256.0);
    }

    /*
    printf("\n\n %d %s sqrt(X)\n", c, is_reciprical?"/":"*");
    printf("Val\tIdx\tIdeal\tScaled (%s%d)\tTableData\n", scale_positive?"":"-", scale);
    for (int i=0; i<input_size; ++i) {
      printf(" %2d\t%2d\t%f\t%f\t%d\n", i, tbl_idx[i], func_output[i], scaled_tbl_entry[i], lut_contents[tbl_idx[i]]);
    }
    */

    return (scale_positive ? 1 : -1) * log2(scale);
  }

void setup_salu(struct salu_cfg *cfg, TestUtil *tu, int count, bool tofino_backwards_compatible=true) {
    int pipe = tu->get_pipe();
    int stage = tu->get_stage();
    uint32_t x = 0;

    auto& mau_base = RegisterUtils::ref_mau(pipe,stage);

    for (int alu=0; alu<count; ++alu) { // For each ALU in the pipe+stage...
      // Program salu_mathunit_ctl
      x = 0;
      setp_salu_mathunit_ctl_salu_mathunit_output_scale(&x,    cfg[alu].salu_mathunit_output_scale);
      setp_salu_mathunit_ctl_salu_mathunit_exponent_invert(&x, cfg[alu].salu_mathunit_exponent_invert);
      setp_salu_mathunit_ctl_salu_mathunit_exponent_shift(&x,  cfg[alu].salu_mathunit_exponent_shift);
      //auto& addr = TestUtil::kTofinoPtr->pipes[pipe].mau[stage].rams.map_alu.meter_group[alu].stateful.salu_mathunit_ctl;
      auto& addr = mau_base.rams.map_alu.meter_group[alu].stateful.salu_mathunit_ctl;
      tu->OutWord(&addr, x);

      // Program salu_const_regfile
      for (int i=0; i<4; ++i) {
        x = 0;
        setp_salu_const_regfile_salu_const_regfile(&x, cfg[alu].salu_const_regfile[i]);
        //auto& addr = TestUtil::kTofinoPtr->pipes[pipe].mau[stage].rams.map_alu.meter_group[alu].stateful.salu_const_regfile[i];
        auto& addr = mau_base.rams.map_alu.meter_group[alu].stateful.salu_const_regfile[i];
        tu->OutWord(&addr, x);
#ifdef MODEL_CHIP_JBAY_OR_LATER
        // program the two extra MSBs - just sign extend the 32 bit salu_const_regfile[] for now
        //  TODO: make salu_const_regfile[] 64 bits and add some tests that change the two extra bits
        auto& e_addr = mau_base.rams.map_alu.meter_group[alu].stateful.salu_const_regfile_msbs[i];
        uint32_t extra_bits = ( 0x80000000 & cfg[alu].salu_const_regfile[i] ) ? 0x3 : 0x0;
        tu->OutWord(&e_addr, extra_bits);
#endif
      }

      // Program salu_mathtable
      for (int i=0; i<4; ++i) {
        x = 0;
        setp_salu_mathtable_salu_mathtable(&x, cfg[alu].salu_mathtable[i]);
        //auto& addr = TestUtil::kTofinoPtr->pipes[pipe].mau[stage].rams.map_alu.meter_group[alu].stateful.salu_mathtable[i];
        auto& addr = mau_base.rams.map_alu.meter_group[alu].stateful.salu_mathtable[i];
        tu->OutWord(&addr, x);
      }

      // Program salu_instr_output_alu
      for (int i=0; i<4; ++i) {
        x = 0;
#ifdef MODEL_CHIP_JBAY_OR_LATER
        if ( ! tofino_backwards_compatible ) {
          // JBay mode for new tests, program all 4 output alus
          for (int out_alu=0;out_alu<4;++out_alu) {
            x = 0;
            setp_salu_instr_output_alu_salu_output_cmpfn(&x, cfg[alu].salu_output_cmpfn_jbay[i][out_alu]);
            int asrc = cfg[alu].salu_output_asrc_jbay[i][out_alu];
            setp_salu_instr_output_alu_salu_output_asrc (&x, asrc);
            auto& addr = mau_base.rams.map_alu.meter_group[alu].stateful.salu_instr_output_alu[i][out_alu];
            tu->OutWord(&addr, x);
          }
        }
        else {
          // Tofino compatible mode for old tests, just program out alu 0
          setp_salu_instr_output_alu_salu_output_cmpfn(&x, cfg[alu].salu_output_cmpfn[i]);
          // remap asrc for JBay
          int asrc = cfg[alu].salu_output_asrc[i];
          if (asrc == 4 || asrc == 5) asrc = asrc==4 ? 5 : 4;
          setp_salu_instr_output_alu_salu_output_asrc (&x, asrc);
          auto& addr = mau_base.rams.map_alu.meter_group[alu].stateful.salu_instr_output_alu[i][0]; // JBAY add extra dimension
          tu->OutWord(&addr, x);
        }
#else
        setp_salu_instr_output_alu_salu_output_cmpfn(&x, cfg[alu].salu_output_cmpfn[i]);
        setp_salu_instr_output_alu_salu_output_asrc (&x, cfg[alu].salu_output_asrc[i]);
        auto& addr = mau_base.rams.map_alu.meter_group[alu].stateful.salu_instr_output_alu[i];
        tu->OutWord(&addr, x);
#endif
      }

      // Program salu_instr_state_alu
      for (int instr=0; instr<4; ++instr) {
        for (int a=0; a<4; ++a) { // 0-3 == alu2-lo, alu1-lo, alu2-hi, alu1-hi
          x = 0;
          setp_salu_instr_state_alu_salu_const_src(&x,       cfg[alu].salu_const_src[instr][a]);
          setp_salu_instr_state_alu_salu_regfile_const(&x,     cfg[alu].salu_regfile_const[instr][a]);

#ifdef MODEL_CHIP_JBAY_OR_LATER
          // translate to JBAY
          int bsrc=0;
          if ( cfg[alu].salu_bsrc_phv[instr][a] ) {
            if ( cfg[alu].salu_bsrc_phv_index[instr][a] )
              bsrc = 1; // phv_hi
            else
              bsrc = 0; // phv_lo
          }
          else {
            bsrc = 4; // const
          }
          setp_salu_instr_state_alu_salu_bsrc_input(&x, bsrc);
          int asrc=0;
          if ( cfg[alu].salu_asrc_memory[instr][a] ) {
            if ( cfg[alu].salu_asrc_memory_index[instr][a] )
              asrc = 1; // mem_hi
            else
              asrc = 0; // mem_lo
          }
          else {
            asrc = 4; // const
          }
          setp_salu_instr_state_alu_salu_asrc_input(&x, asrc);
#else
          setp_salu_instr_state_alu_salu_bsrc_phv_index(&x,    cfg[alu].salu_bsrc_phv_index[instr][a]);
          setp_salu_instr_state_alu_salu_bsrc_phv(&x,          cfg[alu].salu_bsrc_phv[instr][a]);
          setp_salu_instr_state_alu_salu_asrc_memory_index(&x, cfg[alu].salu_asrc_memory_index[instr][a]);
          setp_salu_instr_state_alu_salu_asrc_memory(&x,       cfg[alu].salu_asrc_memory[instr][a]);
#endif


          setp_salu_instr_state_alu_salu_op(&x,                cfg[alu].salu_op[instr][a]);
          setp_salu_instr_state_alu_salu_arith(&x,             cfg[alu].salu_arith[instr][a]);
          setp_salu_instr_state_alu_salu_pred(&x,              cfg[alu].salu_pred[instr][a]);
          auto& addr = mau_base.rams.map_alu.meter_group[alu].stateful.salu_instr_state_alu[instr][a];
          tu->OutWord(&addr, x);
        }
      }

      // Program salu_instr_common
      for (int instr=0; instr<4; ++instr) {
        x = 0;
        setp_salu_instr_common_salu_alu2_lo_bsrc_math(&x, cfg[alu].salu_alu2_lo_bsrc_math[instr]);
        setp_salu_instr_common_salu_alu2_lo_math_src(&x,  cfg[alu].salu_alu2_lo_math_src[instr]);
        setp_salu_instr_common_salu_datasize(&x,          cfg[alu].salu_datasize[instr]);
        setp_salu_instr_common_salu_op_dual(&x,           cfg[alu].salu_op_dual[instr]);
#ifdef MODEL_CHIP_JBAY_OR_LATER
        setp_salu_instr_common_salu_divide_enable(&x, cfg[alu].salu_divide_enable[instr]);
        setp_salu_instr_common_salu_lmatch_sbus_listen(&x, cfg[alu].salu_lmatch_sbus_listen[instr]);
        setp_salu_instr_common_salu_match_sbus_listen(&x, cfg[alu].salu_match_sbus_listen[instr]);
        setp_salu_instr_common_salu_minmax_mask_ctl(&x, cfg[alu].salu_minmax_mask_ctl[instr]);
        setp_salu_instr_common_salu_minmax_ctl(&x, cfg[alu].salu_minmax_ctl[instr]);
        setp_salu_instr_common_salu_minmax_enable(&x, cfg[alu].salu_minmax_enable[instr]);
        setp_salu_instr_common_salu_minmax_postinc_enable(&x, cfg[alu].salu_minmax_postinc_enable[instr]);
        setp_salu_instr_common_salu_minmax_postdec_enable(&x, cfg[alu].salu_minmax_postdec_enable[instr]);
        //  no longer exists
        // setp_salu_instr_common_salu_minmax_postmod_value(&x, cfg[alu].salu_minmax_postmod_value[instr]);
        // Always set it to 1 which means get the value from the PHV
        setp_salu_instr_common_salu_minmax_postmod_value_ctl(&x, 1);
        setp_salu_instr_common_salu_sbus_match_comb(&x, cfg[alu].salu_sbus_match_comb[instr]);
        setp_salu_instr_common_salu_sbus_learn_comb(&x, cfg[alu].salu_sbus_learn_comb[instr]);
#endif
        //auto& addr = TestUtil::kTofinoPtr->pipes[pipe].mau[stage].rams.map_alu.meter_group[alu].stateful.salu_instr_common[instr];
        auto& addr = mau_base.rams.map_alu.meter_group[alu].stateful.salu_instr_common[instr];
        tu->OutWord(&addr, x);
      }

      // Program salu_instr_cmp_alu
      for (int instr=0; instr<4; ++instr) {
        for (int a=0; a<MauDefs::kStatefulAluCmpAlus; ++a) {
          x = 0;
#ifdef MODEL_CHIP_JBAY_OR_LATER
          if (!tofino_backwards_compatible) { // program this unconditionally so minmax works
            setp_salu_instr_cmp_alu_salu_cmp_regfile_adr(&x, cfg[alu].salu_cmp_const_src[instr][a]);
          }
          if ( cfg[alu].salu_cmp_regfile_const[instr][a] ) {
            // From Regfile
            // Tofino uses salu_cmp_const_src for the regfile address too - but JBay has a dedicated field
            setp_salu_instr_cmp_alu_salu_cmp_regfile_adr(&x, cfg[alu].salu_cmp_const_src[instr][a]);
          }
          else {
            setp_salu_instr_cmp_alu_salu_cmp_const_src(&x,   cfg[alu].salu_cmp_const_src[instr][a]);
          }

          // JBay only register fields
          setp_salu_instr_cmp_alu_salu_cmp_asrc_mask_enable(&x, cfg[alu].salu_cmp_asrc_mask_enable[instr][a]);
          setp_salu_instr_cmp_alu_salu_cmp_bsrc_mask_enable(&x, cfg[alu].salu_cmp_bsrc_mask_enable[instr][a]);
          setp_salu_instr_cmp_alu_salu_cmp_mask_input(&x, cfg[alu].salu_cmp_mask_input[instr][a]);
          if (a<2) {
            setp_salu_instr_cmp_alu_salu_cmp_tmatch_enable(&x, cfg[alu].salu_cmp_tmatch_enable[instr][a]);
          }
          setp_salu_instr_cmp_alu_salu_cmp_sbus_or(&x, cfg[alu].salu_cmp_sbus_or[instr][a]);
          setp_salu_instr_cmp_alu_salu_cmp_sbus_and(&x, cfg[alu].salu_cmp_sbus_and[instr][a]);
          setp_salu_instr_cmp_alu_salu_cmp_sbus_invert(&x, cfg[alu].salu_cmp_sbus_invert[instr][a]);

#else
          // Tofino uses salu_cmp_const_src for the regfile address too
          setp_salu_instr_cmp_alu_salu_cmp_const_src(&x,   cfg[alu].salu_cmp_const_src[instr][a]);
#endif
          setp_salu_instr_cmp_alu_salu_cmp_regfile_const(&x, cfg[alu].salu_cmp_regfile_const[instr][a]);

          setp_salu_instr_cmp_alu_salu_cmp_bsrc_input(&x,    cfg[alu].salu_cmp_bsrc_input[instr][a]);
          setp_salu_instr_cmp_alu_salu_cmp_bsrc_sign(&x,     cfg[alu].salu_cmp_bsrc_sign[instr][a]);
          setp_salu_instr_cmp_alu_salu_cmp_bsrc_enable(&x,   cfg[alu].salu_cmp_bsrc_enable[instr][a]);
          setp_salu_instr_cmp_alu_salu_cmp_asrc_input(&x,    cfg[alu].salu_cmp_asrc_input[instr][a]);
          setp_salu_instr_cmp_alu_salu_cmp_asrc_sign(&x,     cfg[alu].salu_cmp_asrc_sign[instr][a]);
          setp_salu_instr_cmp_alu_salu_cmp_asrc_enable(&x,   cfg[alu].salu_cmp_asrc_enable[instr][a]);
          int cmp_opcode = cfg[alu].salu_cmp_opcode[instr][a];
          if (tofino_backwards_compatible) {
            if ( a==2 || a==3 ) { // will only happen on Jbay
              cmp_opcode = 2; // disable
            }
          }
          setp_salu_instr_cmp_alu_salu_cmp_opcode(&x,        cmp_opcode);
          //auto& addr = TestUtil::kTofinoPtr->pipes[pipe].mau[stage].rams.map_alu.meter_group[alu].stateful.salu_instr_cmp_alu[instr][a];
          auto& addr = mau_base.rams.map_alu.meter_group[alu].stateful.salu_instr_cmp_alu[instr][a];
          tu->OutWord(&addr, x);
        }
      }
#ifdef MODEL_CHIP_JBAY_OR_LATER
      // TMatch instructions for JBay
      for (int a=0;a<2;++a) {
        for (int instr=0;instr<4;++instr) {
          uint32_t x=0;
          setp_salu_instr_tmatch_alu_salu_tmatch_vld_ctl(&x,  cfg[alu].salu_tmatch_vld_ctl[instr][a]);
          setp_salu_instr_tmatch_alu_salu_tmatch_invert(&x,  cfg[alu].salu_tmatch_invert[instr][a]);
          auto& addr = mau_base.rams.map_alu.meter_group[alu].stateful.salu_instr_tmatch_alu[instr][a];
          tu->OutWord(&addr, x);
        }
        // 64 bit mask from config turns into two 32bit writes
        uint32_t m0 = cfg[alu].tmatch_mask[a] & 0xFFFFFFFF;
        uint32_t m1 = (cfg[alu].tmatch_mask[a]>>32) & 0xFFFFFFFF;
        auto& a0 = mau_base.rams.map_alu.meter_group[alu].stateful.tmatch_mask[a][0];
        auto& a1 = mau_base.rams.map_alu.meter_group[alu].stateful.tmatch_mask[a][1];
        tu->OutWord(&a0, m0);
        tu->OutWord(&a1, m1);

      }
      // program JBay's stateful clear
      auto& addr_cl = mau_base.rams.map_alu.meter_group[alu].stateful.stateful_clear_action_output;
      tu->OutWord(&addr_cl, cfg[alu].stateful_clear_action_output);
#endif
    }


  }

  static inline uint32_t mk_addr(int op_code, int index, int width, bool dual) {
    int x = width * (dual ? 2:1);
    switch(x) {
      case 1:
        break;
      case 8:
        index = index << 3;
        break;
      case 16:
        index = index << 4;
        break;
      case 32:
        index = index << 5;
        break;
      case 64:
        index = index << 6;
        break;
      case 128:
        index = index << 7;
        break;
    }
    return (op_code << 24) | (1 << 23) | index;
  }



void tmatch_test(TestUtil& tu,
                 salu_cfg& base_cfg,
                 MauStatefulAlu* alu,
                 uint64_t tmatch_mask_0,
                 uint64_t tmatch_mask_1,
                 std::array<bool,4> match_bus,
                 std::array<bool,4> learn_or_match_bus,
                 bool expected_alu_0_out,
                 bool expected_alu_1_out,
                 bool expected_learn,
                 bool expected_match,
                 uint64_t data_in_word_0 = UINT64_C(0x0123456789ABCDEF),
                 uint64_t data_in_word_1 = UINT64_C(0x8877665544332211)
                 ) {

  uint32_t addr;
  BitVector<MauDefs::kStatefulMeterAluDataBits> phv{};
  BitVector<128> data_in = {};
  BitVector<128> data_out = {};
  BitVector<128> action   = {};
  uint64_t present_time = 0;
  bool ingress = true;
  uint64_t lo_exp;
  uint64_t hi_exp;
  uint64_t lo_out;
  uint64_t hi_out;
  uint32_t expected_action_out = 1  // out alu0 has all bits selected and pred shift =0, so single bit is always 1
      | (( ( 1<< ((expected_alu_0_out ? 1 : 0) +
                  (expected_alu_1_out ? 2 : 0)   ))) << 4); // 4 bits are placed at lowest position too

  // Single width 64 bit Lo: 0x44332211 + 0x76543210 + 0xFFFF0000 > 0 (true)
  //                     Hi: 0 + 0 + 3 > 0 (true)
  // should do same this as 32 bit
  struct salu_cfg cfg = base_cfg;
  cfg.salu_datasize[1] = 6;
  cfg.salu_cmp_const_src[1][0] = 3;
  cfg.salu_cmp_const_src[1][1] = 1;
  cfg.salu_cmp_regfile_const[1][0] = 1;
  cfg.salu_cmp_regfile_const[1][1] = 1;
  cfg.salu_cmp_bsrc_input[1][0] = 0; // alu0 phv-lo
  cfg.salu_cmp_bsrc_input[1][1] = 1; // alu1 phv-hi
  cfg.salu_cmp_bsrc_sign[1][0] = 0;
  cfg.salu_cmp_bsrc_enable[1][0] = 1;
  cfg.salu_cmp_bsrc_enable[1][1] = 1;
  cfg.salu_cmp_asrc_input[1][0] = 0; // alu0 ram-lo
  cfg.salu_cmp_asrc_input[1][1] = 1; // alu1 ram-hi
  cfg.salu_cmp_asrc_sign[1][0] = 0;
  cfg.salu_cmp_asrc_enable[1][0] = 1;
  cfg.salu_cmp_asrc_enable[1][1] = 1;
  cfg.salu_cmp_opcode[1][0] = 2; // disabled, so we can see TMatch output
  cfg.salu_cmp_opcode[1][1] = 2; // disabled, so we can see TMatch output

    // set up TMatch
    cfg.tmatch_mask[0] = tmatch_mask_0;
    cfg.tmatch_mask[1] = tmatch_mask_1;
    cfg.salu_cmp_tmatch_enable[1][0] = 1; // instr 1, alu 0
    cfg.salu_cmp_tmatch_enable[1][1] = 1; // instr 1, alu 1

    setup_salu(&cfg, &tu, 1);

    addr = 0;
    phv.set_word(0xBBBBBBBB76543210, 0);
    phv.set_word(0xAAAAAAAAFEDCBA98, 64);
    data_in.set_word(data_in_word_0, 0);
    data_in.set_word(data_in_word_1, 64);
    alu->reset_resources();
    alu->calculate_output(mk_addr(1, addr, 32, false), phv, &data_in, &data_out, &action,
                              present_time,ingress,match_bus,learn_or_match_bus);
    lo_exp = data_in.get_word(0);
    hi_exp = data_in.get_word(64);
    lo_out = data_out.get_word(0);
    hi_out = data_out.get_word(64);
    EXPECT_EQ(expected_action_out, action.get_word(0,32));
    EXPECT_EQ(lo_exp, lo_out);
    EXPECT_EQ(hi_exp, hi_out);
    EXPECT_EQ(expected_match, alu->get_match_output());
    EXPECT_EQ(expected_learn, alu->get_learn_output());

}

struct MinmaxTestParams {
  int      width=8;
  bool     is_max=false;
  uint64_t data_in_word_0 = UINT64_C(0x0);
  uint64_t data_in_word_1 = UINT64_C(0x0);
  uint32_t phv_mask = 0xFFFF;
  bool     mask_from_regfile = false;
  uint32_t addr = 0;
  int      instr = 1;
  uint32_t expected_index = 0;
  uint32_t expected_value = 0;
  bool     postdec_enable = false;
  bool     postinc_enable = false;
  int      postmod_value  = 0;
  // Only checked in postdec_enable or postinc_enable is set
  uint64_t data_out_word_0 = UINT64_C(0x0);
  uint64_t data_out_word_1 = UINT64_C(0x0);

};

void minmax_test(TestUtil& tu,
                 salu_cfg& base_cfg,
                 MauStatefulAlu* alu,
                 const MinmaxTestParams tp ) {
  RMT_ASSERT( tp.width==16 || tp.width==8 );

  BitVector<MauDefs::kStatefulMeterAluDataBits> phv{};
  phv.set_word(tp.phv_mask,0,32);
  // set the post mod value in the phv
  uint8_t pm_val = tp.postmod_value;
  int pm_which_byte = (tp.width == 16) ? 1 : 2 ;
  phv.set_byte( pm_val, pm_which_byte );
  BitVector<128> data_in = {};
  BitVector<128> data_out = {};
  BitVector<128> action   = {};
  uint64_t present_time = 0;
  bool ingress = true;
  uint64_t lo_exp;
  uint64_t hi_exp;
  uint64_t lo_out;
  uint64_t hi_out;
  std::array<bool,4> match_bus{};
  std::array<bool,4> learn_or_match_bus{};

  struct salu_cfg cfg = base_cfg;

  // This test was defined when the Min instruction returned the lowest index
  //  in the case of equal values, now the default is to return the highest
  //  index, but put it back here to return the lowest index.
  MauChipStatefulAlu::kMinReturnsHighestIndex = false;

  cfg.salu_minmax_mask_ctl[tp.instr] = tp.mask_from_regfile ? 1 : 0;
  cfg.salu_minmax_ctl[tp.instr] = (tp.width==16 ? 2:0) |
                                    (tp.is_max  ? 1:0);
  cfg.salu_minmax_enable[tp.instr] = 1;
  cfg.salu_minmax_postinc_enable[tp.instr] = tp.postinc_enable;
  cfg.salu_minmax_postdec_enable[tp.instr] = tp.postdec_enable;
  cfg.salu_minmax_postmod_value[tp.instr] = tp.postmod_value;

  setup_salu(&cfg, &tu, 1, false /* not tofino_backwards_compatible */ );

  data_in.set_word(tp.data_in_word_0, 0);
  data_in.set_word(tp.data_in_word_1, 64);
  alu->reset_resources();
  alu->calculate_output(mk_addr(tp.instr, tp.addr, 128, false), phv, &data_in, &data_out, &action,
                        present_time,ingress,match_bus,learn_or_match_bus);
    lo_out = data_out.get_word(0);
    hi_out = data_out.get_word(64);
    if (tp.postinc_enable || tp.postdec_enable) {
      lo_exp = tp.data_out_word_0;
      hi_exp = tp.data_out_word_1;
    }
    else {
      lo_exp = data_in.get_word(0);
      hi_exp = data_in.get_word(64);
    }
    // Would need to update these to work with 64 bit inputs to use them here
    //EXPECT_PRED_FORMAT2(CmpHelperIntHex ,lo_exp, lo_out);
    //EXPECT_PRED_FORMAT2(CmpHelperIntHex ,hi_exp, hi_out);
    EXPECT_EQ(lo_exp, lo_out);
    EXPECT_EQ(hi_exp, hi_out);
    EXPECT_PRED_FORMAT2(CmpHelperIntHex ,tp.expected_value,action.get_word( 0,32)); // OUT ALU 0
    EXPECT_PRED_FORMAT2(CmpHelperIntHex ,0,action.get_word(32,32)); // OUT ALU 2
    EXPECT_PRED_FORMAT2(CmpHelperIntHex ,tp.expected_index,action.get_word(64,32)); // OUT ALU 1
    EXPECT_PRED_FORMAT2(CmpHelperIntHex ,0,action.get_word(96,32)); // OUT ALU 3
    EXPECT_EQ(static_cast<int>(tp.expected_index),alu->get_minmax_index());
}

  TEST(BFN_TEST_NAME(StatefulAluTest), OneBitOps) {
    int chip = 0, pipe = 1, stage = 2;
    int logical_rows[4] = {3,7,11,15};
    TestUtil tu(GLOBAL_MODEL.get(), chip, pipe, stage);
    RmtObjectManager *om = tu.get_objmgr();
    ASSERT_TRUE(om != NULL);
    std::array<MauStatefulAlu*, 4> alus = {};
    //tu.set_debug(true);
    //tu.update_log_flags(UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0));
    //tu.update_log_flags(UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0));
    om->update_log_type_levels(UINT64_C(~0), UINT64_C(~0), RMT_LOG_TYPE_P4, UINT64_C(~0), UINT64_C(0));

    for (int row=0; row<4; ++row) {
      EXPECT_NE(nullptr, om);
      EXPECT_NE(nullptr, om->mau_get(pipe, stage));
      EXPECT_NE(nullptr, om->mau_get(pipe, stage)->logical_row_lookup(logical_rows[row]));
      EXPECT_NE(nullptr, om->mau_get(pipe, stage)->logical_row_lookup(logical_rows[row])->mau_meter_alu());
      EXPECT_NE(nullptr, om->mau_get(pipe, stage)->logical_row_lookup(logical_rows[row])->mau_meter_alu()->get_salu());
      alus[row] = om->mau_get(pipe, stage)->logical_row_lookup(logical_rows[row])->mau_meter_alu()->get_salu();
    }

    struct salu_cfg cfg[4] = {};

    for (int alu=0; alu<4; ++alu) {
      for (int instr=0; instr<4; ++instr) {
        // Set Cmp-Hi to output a false (set opcode to !=).
        cfg[alu].salu_cmp_opcode[instr][1] = 1;

        // A-Src must be memory for 1-bit ops.
        cfg[alu].salu_asrc_memory[instr][1] = 1;

        // A-Src must be memory for 1-bit ops.
        cfg[alu].salu_asrc_memory[instr][1] = 1;

        // Program the 4 AT instructions.
        cfg[alu].salu_op[instr][1] = (0 == alu) ? instr :     // ALU 0 gets set/clr bit
                                     (1 == alu) ? 4+instr/2 : // ALU 1 gets read bit
                                                  6+instr;    // ALU 2,3 gets AT ops

        // Disable predication.
        cfg[alu].salu_pred[instr][1] = salu_cfg::kAllCmp;

        // Set the out ALU to return ALU-Lo's result.
        cfg[alu].salu_output_cmpfn[instr] = 2;
        cfg[alu].salu_output_asrc[instr] = 5;
      }
    }

    // Program the ALUs.
    setup_salu(cfg, &tu, 4);

    // Inputs and outputs for the tests.
    uint32_t addr = 0;
    BitVector<MauDefs::kStatefulMeterAluDataBits> phv{};
    BitVector<128> data_in = {};
    BitVector<128> data_out = {};
    BitVector<128> data_exp = {};
    BitVector<128> action   = {};
    uint64_t present_time = 0;
    bool ingress = true;
    std::array<bool,4> match_bus{};
    std::array<bool,4> learn_or_match_bus{};

    phv.fill_all_ones();
    // Set all bits with SET_BIT
    for (addr=0; addr<128; ++addr) {
      alus[0]->reset_resources();
      alus[0]->calculate_output(mk_addr(0, addr, 1, false), phv, &data_in, &data_out, &action,
                                present_time,ingress,match_bus,learn_or_match_bus);
      data_exp.set_bit(addr);
      uint64_t lo_exp = data_exp.get_word(0);
      uint64_t hi_exp = data_exp.get_word(64);
      uint64_t lo_out = data_out.get_word(0);
      uint64_t hi_out = data_out.get_word(64);
      EXPECT_EQ(action.get_word(0,32), 0u);
      EXPECT_EQ(lo_exp, lo_out);
      EXPECT_EQ(hi_exp, hi_out);
      data_in.copy_from(data_out);
    }
    // Clear all bits with CLR_BIT
    for (addr=0; addr<128; ++addr) {
      alus[0]->reset_resources();
      alus[0]->calculate_output(mk_addr(2, addr, 1, false), phv, &data_in, &data_out, &action,
                                present_time,ingress,match_bus,learn_or_match_bus);
      data_exp.clear_bit(addr);
      uint64_t lo_exp = data_exp.get_word(0);
      uint64_t hi_exp = data_exp.get_word(64);
      uint64_t lo_out = data_out.get_word(0);
      uint64_t hi_out = data_out.get_word(64);
      EXPECT_EQ(action.get_word(0,32), 1u);
      EXPECT_EQ(lo_exp, lo_out);
      EXPECT_EQ(hi_exp, hi_out);
      data_in.copy_from(data_out);
    }
    // Set all bits with SET_BITC
    for (addr=0; addr<128; ++addr) {
      alus[0]->reset_resources();
      alus[0]->calculate_output(mk_addr(1, addr, 1, false), phv, &data_in, &data_out, &action,
                                present_time,ingress,match_bus,learn_or_match_bus);
      data_exp.set_bit(addr);
      uint64_t lo_exp = data_exp.get_word(0);
      uint64_t hi_exp = data_exp.get_word(64);
      uint64_t lo_out = data_out.get_word(0);
      uint64_t hi_out = data_out.get_word(64);
      EXPECT_EQ(action.get_word(0,32), 1u);
      EXPECT_EQ(lo_exp, lo_out);
      EXPECT_EQ(hi_exp, hi_out);
      data_in.copy_from(data_out);
    }
    // Clear all bits with CLR_BITC
    for (addr=0; addr<128; ++addr) {
      alus[0]->reset_resources();
      alus[0]->calculate_output(mk_addr(3, addr, 1, false), phv, &data_in, &data_out, &action,
                                present_time,ingress,match_bus,learn_or_match_bus);
      data_exp.clear_bit(addr);
      uint64_t lo_exp = data_exp.get_word(0);
      uint64_t hi_exp = data_exp.get_word(64);
      uint64_t lo_out = data_out.get_word(0);
      uint64_t hi_out = data_out.get_word(64);
      EXPECT_EQ(action.get_word(0,32), 0u);
      EXPECT_EQ(lo_exp, lo_out);
      EXPECT_EQ(hi_exp, hi_out);
      data_in.copy_from(data_out);
    }

    // Set all bits with SET_BIT_AT
    for (addr=0; addr<120; ++addr) {
      alus[2]->reset_resources();
      alus[2]->calculate_output(mk_addr(0, addr, 1, false), phv, &data_in, &data_out, &action,
                                present_time,ingress,match_bus,learn_or_match_bus);
      data_exp.set_bit(addr);
      data_exp.set_byte(addr+1, 15);
      uint64_t lo_exp = data_exp.get_word(0);
      uint64_t hi_exp = data_exp.get_word(64);
      uint64_t lo_out = data_out.get_word(0);
      uint64_t hi_out = data_out.get_word(64);
      EXPECT_EQ(action.get_word(0,32), 0u);
      EXPECT_EQ(lo_exp, lo_out);
      EXPECT_EQ(hi_exp, hi_out);
      data_in.copy_from(data_out);
    }
    // Clear all bits with CLR_BIT_AT
    for (addr=0; addr<120; ++addr) {
      alus[3]->reset_resources();
      alus[3]->calculate_output(mk_addr(2, addr, 1, false), phv, &data_in, &data_out, &action,
                                present_time,ingress,match_bus,learn_or_match_bus);
      data_exp.clear_bit(addr);
      data_exp.set_byte(119-addr, 15);
      uint64_t lo_exp = data_exp.get_word(0);
      uint64_t hi_exp = data_exp.get_word(64);
      uint64_t lo_out = data_out.get_word(0);
      uint64_t hi_out = data_out.get_word(64);
      EXPECT_EQ(action.get_word(0,32), 1u);
      EXPECT_EQ(lo_exp, lo_out);
      EXPECT_EQ(hi_exp, hi_out);
      data_in.copy_from(data_out);
    }
    // Set all bits with SET_BITC_AT
    for (addr=0; addr<120; ++addr) {
      alus[3]->reset_resources();
      alus[3]->calculate_output(mk_addr(1, addr, 1, false), phv, &data_in, &data_out, &action,
                                present_time,ingress,match_bus,learn_or_match_bus);
      data_exp.set_bit(addr);
      data_exp.set_byte(addr+1, 15);
      uint64_t lo_exp = data_exp.get_word(0);
      uint64_t hi_exp = data_exp.get_word(64);
      uint64_t lo_out = data_out.get_word(0);
      uint64_t hi_out = data_out.get_word(64);
      EXPECT_EQ(action.get_word(0,32), 1u);
      EXPECT_EQ(lo_exp, lo_out);
      EXPECT_EQ(hi_exp, hi_out);
      data_in.copy_from(data_out);
    }
    // Clear all bits with CLR_BITC_AT
    for (addr=0; addr<120; ++addr) {
      alus[3]->reset_resources();
      alus[3]->calculate_output(mk_addr(3, addr, 1, false), phv, &data_in, &data_out, &action,
                                present_time,ingress,match_bus,learn_or_match_bus);
      data_exp.clear_bit(addr);
      data_exp.set_byte(119-addr, 15);
      uint64_t lo_exp = data_exp.get_word(0);
      uint64_t hi_exp = data_exp.get_word(64);
      uint64_t lo_out = data_out.get_word(0);
      uint64_t hi_out = data_out.get_word(64);
      EXPECT_EQ(action.get_word(0,32), 0u);
      EXPECT_EQ(lo_exp, lo_out);
      EXPECT_EQ(hi_exp, hi_out);
      data_in.copy_from(data_out);
    }
  }

  TEST(BFN_TEST_NAME(StatefulAluTest), CmpAlu) {
    int chip = 0, pipe = 1, stage = 2;
    int logical_rows[4] = {3,7,11,15};
    TestUtil tu(GLOBAL_MODEL.get(), chip, pipe, stage);
    RmtObjectManager *om = tu.get_objmgr();
    ASSERT_TRUE(om != NULL);
    std::array<MauStatefulAlu*, 4> alus = {};
    //tu.set_debug(true);
    tu.update_log_flags(UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0));
    //tu.update_log_flags(UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0));
    om->update_log_type_levels(UINT64_C(~0), UINT64_C(~0), RMT_LOG_TYPE_P4, UINT64_C(~0), UINT64_C(0));

    for (int row=0; row<4; ++row) {
      EXPECT_NE(nullptr, om);
      EXPECT_NE(nullptr, om->mau_get(pipe, stage));
      EXPECT_NE(nullptr, om->mau_get(pipe, stage)->logical_row_lookup(logical_rows[row]));
      EXPECT_NE(nullptr, om->mau_get(pipe, stage)->logical_row_lookup(logical_rows[row])->mau_meter_alu());
      EXPECT_NE(nullptr, om->mau_get(pipe, stage)->logical_row_lookup(logical_rows[row])->mau_meter_alu()->get_salu());
      alus[row] = om->mau_get(pipe, stage)->logical_row_lookup(logical_rows[row])->mau_meter_alu()->get_salu();
    }

    struct salu_cfg base_cfg[4] = {};
    struct salu_cfg cfg = {};

    for (int alu=0; alu<4; ++alu) {
      for (int instr=0; instr<4; ++instr) {
        // Program the State-ALUs to do a Set-Hi
        base_cfg[alu].salu_op[instr][alu] = 16;

        // Predicate all State-ALUs.
        base_cfg[alu].salu_pred[instr][alu] = 0;

        // Set the out ALU to return the Compare ALU's result.
        base_cfg[alu].salu_output_cmpfn[instr] = salu_cfg::kAllCmp;
        base_cfg[alu].salu_output_asrc[instr] = 6;
      }
      // Set the register file constants.
      base_cfg[alu].salu_const_regfile[0] = 0x00000080;
      base_cfg[alu].salu_const_regfile[1] = 0x03;
      base_cfg[alu].salu_const_regfile[2] = 0x06;
      base_cfg[alu].salu_const_regfile[3] = 0xFFFF0000;
    }

    //
    // 8-Bit Operations
    //
    // 0+0+0<0 and 0+0+0>0
    cfg = base_cfg[0];
    cfg.salu_datasize[1] = 3;
    cfg.salu_op_dual[1] = 0;
    cfg.salu_output_asrc[1] = 6;
    cfg.salu_cmp_const_src[1][0] = 0;
    cfg.salu_cmp_const_src[1][1] = 0;
    cfg.salu_cmp_regfile_const[1][0] = 0;
    cfg.salu_cmp_regfile_const[1][1] = 0;
    cfg.salu_cmp_bsrc_input[1][0] = 0;
    cfg.salu_cmp_bsrc_sign[1][0] = 0;
    cfg.salu_cmp_bsrc_enable[1][0] = 0;
    cfg.salu_cmp_asrc_input[1][0] = 0;
    cfg.salu_cmp_asrc_sign[1][0] = 0;
    cfg.salu_cmp_asrc_enable[1][0] = 0;
    cfg.salu_cmp_opcode[1][0] = 4;
    cfg.salu_cmp_opcode[1][1] = 7;
    setup_salu(&cfg, &tu, 1);

    uint32_t addr = 2;
    BitVector<MauDefs::kStatefulMeterAluDataBits> phv{};
    phv.set_word(0xFEDCBA9876543210, 0);
    BitVector<128> data_in = {};
    BitVector<128> data_out = {};
    data_in.set_word(0x0123456789ABCDEF, 0);
    data_in.set_word(0x8877665544332211, 64);
    BitVector<128> action   = {};
    action.set32(0,~0);
    uint64_t present_time = 0;
    bool ingress = true;
    std::array<bool,4> match_bus{};
    std::array<bool,4> learn_or_match_bus{};

    //tu.update_log_flags(UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0));
    alus[0]->reset_resources();
    alus[0]->calculate_output(mk_addr(1, addr, 8, false), phv, &data_in, &data_out, &action,
                              present_time,ingress,match_bus,learn_or_match_bus);
    //tu.update_log_flags(UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0));
    uint64_t lo_exp = data_in.get_word(0);
    uint64_t hi_exp = data_in.get_word(64);
    uint64_t lo_out = data_out.get_word(0);
    uint64_t hi_out = data_out.get_word(64);
    EXPECT_EQ(static_cast<uint32_t>(1 << 4 | 1), action.get_word(0,32));
    EXPECT_EQ(lo_exp, lo_out);
    EXPECT_EQ(hi_exp, hi_out);

    // 0+0+0<=0 and 0+0+0>=0
    cfg = base_cfg[0];
    cfg.salu_datasize[1] = 3;
    cfg.salu_op_dual[1] = 0;
    cfg.salu_output_asrc[1] = 6;
    cfg.salu_cmp_const_src[1][0] = 0;
    cfg.salu_cmp_const_src[1][1] = 0;
    cfg.salu_cmp_regfile_const[1][0] = 0;
    cfg.salu_cmp_regfile_const[1][1] = 0;
    cfg.salu_cmp_bsrc_input[1][0] = 0;
    cfg.salu_cmp_bsrc_sign[1][0] = 0;
    cfg.salu_cmp_bsrc_enable[1][0] = 0;
    cfg.salu_cmp_asrc_input[1][0] = 0;
    cfg.salu_cmp_asrc_sign[1][0] = 0;
    cfg.salu_cmp_asrc_enable[1][0] = 0;
    cfg.salu_cmp_opcode[1][0] = 9;
    cfg.salu_cmp_opcode[1][1] = 10;
    setup_salu(&cfg, &tu, 1);

    addr = 2;
    phv.set_word(0xFEDCBA9876543210, 0);
    data_in.set_word(0x0123456789ABCDEF, 0);
    data_in.set_word(0x8877665544332211, 64);
    action.set32(0,~0);
    //tu.update_log_flags(UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0));
    alus[0]->reset_resources();
    alus[0]->calculate_output(mk_addr(1, addr, 8, false), phv, &data_in, &data_out, &action,
                              present_time,ingress,match_bus,learn_or_match_bus);
    //tu.update_log_flags(UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0));
    lo_exp = data_in.get_word(0);
    hi_exp = data_in.get_word(64);
    lo_out = data_out.get_word(0);
    hi_out = data_out.get_word(64);
    EXPECT_EQ(static_cast<uint32_t>(8 << 4 | 1), action.get_word(0,32));
    EXPECT_EQ(lo_exp, lo_out);
    EXPECT_EQ(hi_exp, hi_out);



    //
    // 8-Bit Operations
    //
    cfg = base_cfg[0];
    cfg.salu_datasize[0] = 3;
    cfg.salu_datasize[1] = 3;
    cfg.salu_datasize[2] = 3;
    cfg.salu_datasize[3] = 3;
    cfg.salu_op_dual[3] = 1;

    // 8-Bit 1+2+3 == 0 | -2 + -4 + 6
    cfg.salu_cmp_const_src[3][0] = 1;
    cfg.salu_cmp_const_src[3][1] = 2;
    cfg.salu_cmp_regfile_const[3][0] = 1;
    cfg.salu_cmp_regfile_const[3][1] = 1;
    cfg.salu_cmp_bsrc_input[3][0] = 0;
    cfg.salu_cmp_bsrc_input[3][1] = 1;
    cfg.salu_cmp_bsrc_sign[3][0] = 0;
    cfg.salu_cmp_bsrc_sign[3][1] = 1;
    cfg.salu_cmp_bsrc_enable[3][0] = 1;
    cfg.salu_cmp_bsrc_enable[3][1] = 1;
    cfg.salu_cmp_asrc_input[3][0] = 0;
    cfg.salu_cmp_asrc_input[3][1] = 1;
    cfg.salu_cmp_asrc_sign[3][0] = 0;
    cfg.salu_cmp_asrc_sign[3][1] = 1;
    cfg.salu_cmp_asrc_enable[3][0] = 1;
    cfg.salu_cmp_asrc_enable[3][1] = 1;
    cfg.salu_cmp_opcode[3][0] = 0;
    cfg.salu_cmp_opcode[3][1] = 0;


    // Program the ALUs.
    setup_salu(&cfg, &tu, 1);

    // Inputs and outputs for the tests.
    addr = 1;
    phv.set_word(0x0402, 0);
    data_in.set_word(0x02010000, 0);
    action.set32(0,~0);

    //tu.update_log_flags(UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0));
    alus[0]->reset_resources();
    alus[0]->calculate_output(mk_addr(3, addr, 8, true), phv, &data_in, &data_out, &action,
                              present_time,ingress,match_bus,learn_or_match_bus);
    //tu.update_log_flags(UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0));
    lo_exp = data_in.get_word(0);
    hi_exp = data_in.get_word(64);
    lo_out = data_out.get_word(0);
    hi_out = data_out.get_word(64);
    EXPECT_EQ(static_cast<uint32_t>(4 << 4 | 1), action.get_word(0,32));
    EXPECT_EQ(lo_exp, lo_out);
    EXPECT_EQ(hi_exp, hi_out);

    // Single width 16 bit Lo: 0x2211 + 0x3210 + 0 != 0 (true)
    //                     Hi: 0 + 0 + 0 != 0 (false)
    cfg = base_cfg[0];
    cfg.salu_datasize[1] = 4;
    cfg.salu_op_dual[1] = 0;
    cfg.salu_cmp_const_src[1][0] = 3;
    cfg.salu_cmp_regfile_const[1][0] = 1;
    cfg.salu_cmp_bsrc_input[1][0] = 0; // phv-lo
    cfg.salu_cmp_bsrc_sign[1][0] = 0;
    cfg.salu_cmp_bsrc_enable[1][0] = 1;
    cfg.salu_cmp_asrc_input[1][0] = 0; // ram-lo
    cfg.salu_cmp_asrc_sign[1][0] = 0;
    cfg.salu_cmp_asrc_enable[1][0] = 1;
    cfg.salu_cmp_opcode[1][0] = 1;
    cfg.salu_cmp_opcode[1][1] = 1;
    setup_salu(&cfg, &tu, 1);

    addr = 4;
    phv.set_word(0xFEDCBA9876543210, 0);
    data_in.set_word(0x0123456789ABCDEF, 0);
    data_in.set_word(0x8877665544332211, 64);
    //tu.update_log_flags(UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0));
    alus[0]->reset_resources();
    alus[0]->calculate_output(mk_addr(1, addr, 16, false), phv, &data_in, &data_out, &action,
                              present_time,ingress,match_bus,learn_or_match_bus);
    //tu.update_log_flags(UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0));
    lo_exp = data_in.get_word(0);
    hi_exp = data_in.get_word(64);
    lo_out = data_out.get_word(0);
    hi_out = data_out.get_word(64);
    EXPECT_EQ(static_cast<uint32_t>(2 << 4 | 1), action.get_word(0,32));
    EXPECT_EQ(lo_exp, lo_out);
    EXPECT_EQ(hi_exp, hi_out);

    // Single width 32 bit Lo: 0x44332211 + 0x76543210 + 0xFFFF0000 > 0 (true)
    //                     Hi: 0 + 0 + 3 > 0 (true)
    cfg = base_cfg[0];
    cfg.salu_datasize[1] = 5;
    cfg.salu_op_dual[1] = 0;
    cfg.salu_cmp_const_src[1][0] = 3;
    cfg.salu_cmp_const_src[1][1] = 1;
    cfg.salu_cmp_regfile_const[1][0] = 1;
    cfg.salu_cmp_regfile_const[1][1] = 1;
    cfg.salu_cmp_bsrc_input[1][0] = 0; // phv-lo
    cfg.salu_cmp_bsrc_sign[1][0] = 0;
    cfg.salu_cmp_bsrc_enable[1][0] = 1;
    cfg.salu_cmp_asrc_input[1][0] = 0; // ram-lo
    cfg.salu_cmp_asrc_sign[1][0] = 0;
    cfg.salu_cmp_asrc_enable[1][0] = 1;
    cfg.salu_cmp_opcode[1][0] = 4;
    cfg.salu_cmp_opcode[1][1] = 4;
    setup_salu(&cfg, &tu, 1);

    addr = 2;
    phv.set_word(0xFEDCBA9876543210, 0);
    data_in.set_word(0x0123456789ABCDEF, 0);
    data_in.set_word(0x8877665544332211, 64);
    //tu.update_log_flags(UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0));
    alus[0]->reset_resources();
    alus[0]->calculate_output(mk_addr(1, addr, 32, false), phv, &data_in, &data_out, &action,
                              present_time,ingress,match_bus,learn_or_match_bus);
    //tu.update_log_flags(UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0));
    lo_exp = data_in.get_word(0);
    hi_exp = data_in.get_word(64);
    lo_out = data_out.get_word(0);
    hi_out = data_out.get_word(64);
    EXPECT_EQ(static_cast<uint32_t>(8 << 4 | 1), action.get_word(0,32));
    EXPECT_EQ(lo_exp, lo_out);
    EXPECT_EQ(hi_exp, hi_out);


    // Double width 8 bit Lo: 0 + 0 + 0 <= 0 (true)
    //                    Hi: 5 - 7 + 0 <= 0 (true)
    cfg = base_cfg[0];
    cfg.salu_datasize[1] = 3;
    cfg.salu_op_dual[1] = 1;
    cfg.salu_cmp_const_src[1][0] = 0;
    cfg.salu_cmp_const_src[1][1] = 0;
    cfg.salu_cmp_regfile_const[1][0] = 0;
    cfg.salu_cmp_regfile_const[1][1] = 0;
    cfg.salu_cmp_bsrc_input[1][1] = 0; // phv-lo
    cfg.salu_cmp_bsrc_sign[1][1] = 1;
    cfg.salu_cmp_bsrc_enable[1][1] = 1;
    cfg.salu_cmp_asrc_input[1][1] = 0; // ram-lo
    cfg.salu_cmp_asrc_sign[1][1] = 0;
    cfg.salu_cmp_asrc_enable[1][1] = 1;
    cfg.salu_cmp_opcode[1][0] = 5;
    cfg.salu_cmp_opcode[1][1] = 5;
    setup_salu(&cfg, &tu, 1);

    addr = 2;
    phv.set_word(0xFEDCBA9876543207, 0);
    data_in.set_word(0x0123450589ABCDEF, 0);
    data_in.set_word(0x8877665544332211, 64);
    //tu.update_log_flags(UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0));
    alus[0]->reset_resources();
    alus[0]->calculate_output(mk_addr(1, addr, 8, true), phv, &data_in, &data_out, &action,
                              present_time,ingress,match_bus,learn_or_match_bus);
    //tu.update_log_flags(UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0));
    lo_exp = data_in.get_word(0);
    hi_exp = data_in.get_word(64);
    lo_out = data_out.get_word(0);
    hi_out = data_out.get_word(64);
    EXPECT_EQ(static_cast<uint32_t>(8 << 4 | 1), action.get_word(0,32));
    EXPECT_EQ(lo_exp, lo_out);
    EXPECT_EQ(hi_exp, hi_out);

    // Double width 16 bit Lo: 0 + 0 + 0 >= 0 (true)
    //                     Hi: 5 - 7 + 0 >= 0 (false)
    cfg = base_cfg[0];
    cfg.salu_datasize[1] = 4;
    cfg.salu_op_dual[1] = 1;
    cfg.salu_cmp_const_src[1][0] = 0;
    cfg.salu_cmp_const_src[1][1] = 0;
    cfg.salu_cmp_regfile_const[1][0] = 0;
    cfg.salu_cmp_regfile_const[1][1] = 0;
    cfg.salu_cmp_bsrc_input[1][1] = 0; // phv-lo
    cfg.salu_cmp_bsrc_sign[1][1] = 1;
    cfg.salu_cmp_bsrc_enable[1][1] = 1;
    cfg.salu_cmp_asrc_input[1][1] = 0; // ram-lo
    cfg.salu_cmp_asrc_sign[1][1] = 0;
    cfg.salu_cmp_asrc_enable[1][1] = 1;
    cfg.salu_cmp_opcode[1][0] = 6;
    cfg.salu_cmp_opcode[1][1] = 6;
    setup_salu(&cfg, &tu, 1);

    addr = 1;
    phv.set_word(0xFEDCBA9876540007, 0);
    data_in.set_word(0x0123000589ABCDEF, 0);
    data_in.set_word(0x8877665544332211, 64);
    //tu.update_log_flags(UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0));
    alus[0]->reset_resources();
    alus[0]->calculate_output(mk_addr(1, addr, 16, true), phv, &data_in, &data_out, &action,
                              present_time,ingress,match_bus,learn_or_match_bus);
    //tu.update_log_flags(UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0));
    lo_exp = data_in.get_word(0);
    hi_exp = data_in.get_word(64);
    lo_out = data_out.get_word(0);
    hi_out = data_out.get_word(64);
    EXPECT_EQ(static_cast<uint32_t>(2 << 4 | 1), action.get_word(0,32));
    EXPECT_EQ(lo_exp, lo_out);
    EXPECT_EQ(hi_exp, hi_out);

    // Double width 32 bit Lo: 1,000,000 + 1,000,000 + 3 < 0 (false)
    //                     Hi: 1,000,000 - 1,000,000 - 65536 < 0 (true)
    cfg = base_cfg[0];
    cfg.salu_datasize[1] = 5;
    cfg.salu_op_dual[1] = 1;
    cfg.salu_cmp_const_src[1][0] = 1;
    cfg.salu_cmp_const_src[1][1] = 3;
    cfg.salu_cmp_regfile_const[1][0] = 1;
    cfg.salu_cmp_regfile_const[1][1] = 1;
    cfg.salu_cmp_bsrc_input[1][0] = 0; // phv-lo
    cfg.salu_cmp_bsrc_input[1][1] = 1; // phv-hi
    cfg.salu_cmp_bsrc_sign[1][0] = 0;
    cfg.salu_cmp_bsrc_sign[1][1] = 1;
    cfg.salu_cmp_bsrc_enable[1][0] = 1;
    cfg.salu_cmp_bsrc_enable[1][1] = 1;
    cfg.salu_cmp_asrc_input[1][0] = 0; // ram-lo
    cfg.salu_cmp_asrc_input[1][1] = 1; // ram-hi
    cfg.salu_cmp_asrc_sign[1][0] = 0;
    cfg.salu_cmp_asrc_sign[1][1] = 0;
    cfg.salu_cmp_asrc_enable[1][0] = 1;
    cfg.salu_cmp_asrc_enable[1][1] = 1;
    cfg.salu_cmp_opcode[1][0] = 7;
    cfg.salu_cmp_opcode[1][1] = 7;
    setup_salu(&cfg, &tu, 1);

    addr = 1;
    phv.set_word(0x000f4240000f4240, 0);
    data_in.set_word(0x0123000589ABCDEF, 0);
    data_in.set_word(0x000F4240000F4240, 64);
    //tu.update_log_flags(UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0));
    alus[0]->reset_resources();
    alus[0]->calculate_output(mk_addr(1, addr, 32, true), phv, &data_in, &data_out, &action,
                              present_time,ingress,match_bus,learn_or_match_bus);
    //tu.update_log_flags(UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0));
    lo_exp = data_in.get_word(0);
    hi_exp = data_in.get_word(64);
    lo_out = data_out.get_word(0);
    hi_out = data_out.get_word(64);
    EXPECT_EQ(static_cast<uint32_t>(4 << 4 | 1), action.get_word(0,32));
    EXPECT_EQ(lo_exp, lo_out);
    EXPECT_EQ(hi_exp, hi_out);

    // Double width 32 bit Lo: 1,500,000,000 + 1,500,000,000 - 3 > 0 (true)
    //                     Hi: 1,000,000 - 1,000,000 - 65536 > 0 (false)
    cfg = base_cfg[0];
    cfg.salu_datasize[1] = 5;
    cfg.salu_op_dual[1] = 1;
    cfg.salu_cmp_const_src[1][0] = 0xD; // -3
    cfg.salu_cmp_const_src[1][1] = 3;
    cfg.salu_cmp_regfile_const[1][0] = 0;
    cfg.salu_cmp_regfile_const[1][1] = 1;
    cfg.salu_cmp_bsrc_input[1][0] = 0; // phv-lo
    cfg.salu_cmp_bsrc_input[1][1] = 1; // phv-hi
    cfg.salu_cmp_bsrc_sign[1][0] = 0;
    cfg.salu_cmp_bsrc_sign[1][1] = 1;
    cfg.salu_cmp_bsrc_enable[1][0] = 1;
    cfg.salu_cmp_bsrc_enable[1][1] = 1;
    cfg.salu_cmp_asrc_input[1][0] = 0; // ram-lo
    cfg.salu_cmp_asrc_input[1][1] = 1; // ram-hi
    cfg.salu_cmp_asrc_sign[1][0] = 0;
    cfg.salu_cmp_asrc_sign[1][1] = 0;
    cfg.salu_cmp_asrc_enable[1][0] = 1;
    cfg.salu_cmp_asrc_enable[1][1] = 1;
    cfg.salu_cmp_opcode[1][0] = 8;
    cfg.salu_cmp_opcode[1][1] = 8;
    setup_salu(&cfg, &tu, 1);

    addr = 0;
    phv.set_word(0x000F424059682F00, 0);
    data_in.set_word(0x000F424059682F00, 0);
    data_in.set_word(0x0000000000000000, 64);
    //tu.update_log_flags(UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0));
    alus[0]->reset_resources();
    alus[0]->calculate_output(mk_addr(1, addr, 32, true), phv, &data_in, &data_out, &action,
                              present_time,ingress,match_bus,learn_or_match_bus);
    //tu.update_log_flags(UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0));
    lo_exp = data_in.get_word(0);
    hi_exp = data_in.get_word(64);
    lo_out = data_out.get_word(0);
    hi_out = data_out.get_word(64);
    EXPECT_EQ(static_cast<uint32_t>(2 << 4 | 1), action.get_word(0,32));
    EXPECT_EQ(lo_exp, lo_out);
    EXPECT_EQ(hi_exp, hi_out);

    // Double width 32 bit Lo: 1,000,000 + 1,000,000 - 3 <= 0 (false) (unsigned)
    //                     Hi: 1,500,000,000 - 1,500,000,000 - 65536 <= 0 (true) (unsigned)
    cfg = base_cfg[0];
    cfg.salu_datasize[1] = 5;
    cfg.salu_op_dual[1] = 1;
    cfg.salu_cmp_const_src[1][0] = 0xD; // -3
    cfg.salu_cmp_const_src[1][1] = 3;
    cfg.salu_cmp_regfile_const[1][0] = 0;
    cfg.salu_cmp_regfile_const[1][1] = 1;
    cfg.salu_cmp_bsrc_input[1][1] = 0; // phv-lo
    cfg.salu_cmp_bsrc_input[1][0] = 1; // phv-hi
    cfg.salu_cmp_bsrc_sign[1][0] = 0;
    cfg.salu_cmp_bsrc_sign[1][1] = 1;
    cfg.salu_cmp_bsrc_enable[1][0] = 1;
    cfg.salu_cmp_bsrc_enable[1][1] = 1;
    cfg.salu_cmp_asrc_input[1][1] = 0; // ram-lo
    cfg.salu_cmp_asrc_input[1][0] = 1; // ram-hi
    cfg.salu_cmp_asrc_sign[1][0] = 0;
    cfg.salu_cmp_asrc_sign[1][1] = 0;
    cfg.salu_cmp_asrc_enable[1][0] = 1;
    cfg.salu_cmp_asrc_enable[1][1] = 1;
    cfg.salu_cmp_opcode[1][0] = 9;
    cfg.salu_cmp_opcode[1][1] = 9;
    setup_salu(&cfg, &tu, 1);

    addr = 0;
    phv.set_word(0x000F424059682F00, 0);
    data_in.set_word(0x000F424059682F00, 0);
    data_in.set_word(0x0000000000000000, 64);
    //tu.update_log_flags(UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0));
    alus[0]->reset_resources();
    alus[0]->calculate_output(mk_addr(1, addr, 32, true), phv, &data_in, &data_out, &action,
                              present_time,ingress,match_bus,learn_or_match_bus);
    //tu.update_log_flags(UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0));
    lo_exp = data_in.get_word(0);
    hi_exp = data_in.get_word(64);
    lo_out = data_out.get_word(0);
    hi_out = data_out.get_word(64);
    EXPECT_EQ(static_cast<uint32_t>(4 << 4 | 1), action.get_word(0,32));
    EXPECT_EQ(lo_exp, lo_out);
    EXPECT_EQ(hi_exp, hi_out);

    // Double width 32 bit Lo: 1,000,000 + 1,000,000 - 3 >= 0 (true) (unsigned)
    //                     Hi: 1,500,000,000 - 1,500,000,000 - 65536 >= 0 (false) (unsigned)
    cfg = base_cfg[0];
    cfg.salu_datasize[1] = 5;
    cfg.salu_op_dual[1] = 1;
    cfg.salu_cmp_const_src[1][0] = 0xD; // -3
    cfg.salu_cmp_const_src[1][1] = 3;
    cfg.salu_cmp_regfile_const[1][0] = 0;
    cfg.salu_cmp_regfile_const[1][1] = 1;
    cfg.salu_cmp_bsrc_input[1][1] = 0; // phv-lo
    cfg.salu_cmp_bsrc_input[1][0] = 1; // phv-hi
    cfg.salu_cmp_bsrc_sign[1][0] = 0;
    cfg.salu_cmp_bsrc_sign[1][1] = 1;
    cfg.salu_cmp_bsrc_enable[1][0] = 1;
    cfg.salu_cmp_bsrc_enable[1][1] = 1;
    cfg.salu_cmp_asrc_input[1][1] = 0; // ram-lo
    cfg.salu_cmp_asrc_input[1][0] = 1; // ram-hi
    cfg.salu_cmp_asrc_sign[1][0] = 0;
    cfg.salu_cmp_asrc_sign[1][1] = 0;
    cfg.salu_cmp_asrc_enable[1][0] = 1;
    cfg.salu_cmp_asrc_enable[1][1] = 1;
    cfg.salu_cmp_opcode[1][0] = 10;
    cfg.salu_cmp_opcode[1][1] = 10;
    setup_salu(&cfg, &tu, 1);

    addr = 0;
    phv.set_word(0x000F424059682F00, 0);
    data_in.set_word(0x000F424059682F00, 0);
    data_in.set_word(0x0000000000000000, 64);
    //tu.update_log_flags(UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0));
    alus[0]->reset_resources();
    alus[0]->calculate_output(mk_addr(1, addr, 32, true), phv, &data_in, &data_out, &action,
                              present_time,ingress,match_bus,learn_or_match_bus);
    //tu.update_log_flags(UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0));
    lo_exp = data_in.get_word(0);
    hi_exp = data_in.get_word(64);
    lo_out = data_out.get_word(0);
    hi_out = data_out.get_word(64);
    EXPECT_EQ(static_cast<uint32_t>(2 << 4 | 1), action.get_word(0,32));
    EXPECT_EQ(lo_exp, lo_out);
    EXPECT_EQ(hi_exp, hi_out);

    // Single width 16 bit Lo: -0 - 1 + 0 < 0 (true) (unsigned)
    //                     Hi: -0 - 0x8000 + 0 < 0 (true) (unsigned)
    cfg = base_cfg[0];
    cfg.salu_datasize[1] = 4;
    cfg.salu_op_dual[1] = 0;
    cfg.salu_cmp_const_src[1][0] = 0;
    cfg.salu_cmp_const_src[1][1] = 0;
    cfg.salu_cmp_regfile_const[1][0] = 0;
    cfg.salu_cmp_regfile_const[1][1] = 0;
    cfg.salu_cmp_bsrc_input[1][0] = 0; // phv-lo
    cfg.salu_cmp_bsrc_input[1][1] = 1; // phv-hi
    cfg.salu_cmp_bsrc_sign[1][0] = 1;
    cfg.salu_cmp_bsrc_sign[1][1] = 1;
    cfg.salu_cmp_bsrc_enable[1][0] = 1;
    cfg.salu_cmp_bsrc_enable[1][1] = 1;
    cfg.salu_cmp_asrc_input[1][0] = 0; // ram-lo
    cfg.salu_cmp_asrc_input[1][1] = 1; // ram-hi
    cfg.salu_cmp_asrc_sign[1][0] = 1;
    cfg.salu_cmp_asrc_sign[1][1] = 1;
    cfg.salu_cmp_asrc_enable[1][0] = 1;
    cfg.salu_cmp_asrc_enable[1][1] = 1;
    cfg.salu_cmp_opcode[1][0] = 11;
    cfg.salu_cmp_opcode[1][1] = 11;
    setup_salu(&cfg, &tu, 1);

    addr = 0;
    phv.set_word(0x000F424080000001, 0);
    data_in.set_word(0x0000000000000000, 0);
    data_in.set_word(0x000F424059682F00, 64);
    //tu.update_log_flags(UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0));
    alus[0]->reset_resources();
    alus[0]->calculate_output(mk_addr(1, addr, 16, false), phv, &data_in, &data_out, &action,
                              present_time,ingress,match_bus,learn_or_match_bus);
    //tu.update_log_flags(UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0));
    lo_exp = data_in.get_word(0);
    hi_exp = data_in.get_word(64);
    lo_out = data_out.get_word(0);
    hi_out = data_out.get_word(64);
    EXPECT_EQ(static_cast<uint32_t>(8 << 4 | 1), action.get_word(0,32));
    EXPECT_EQ(lo_exp, lo_out);
    EXPECT_EQ(hi_exp, hi_out);

    // Single width 8 bit Lo: 0xFF + 0xFF + 0x80 >UUS 0 (true)
    //                    Hi: 0xFF - 1 + 0 >UUS 0 (false)
    cfg = base_cfg[0];
    cfg.salu_datasize[1] = 3;
    cfg.salu_op_dual[1] = 0;
    cfg.salu_cmp_const_src[1][0] = 0;
    cfg.salu_cmp_const_src[1][1] = 0;
    cfg.salu_cmp_regfile_const[1][0] = 1;
    cfg.salu_cmp_regfile_const[1][1] = 0;
    cfg.salu_cmp_bsrc_input[1][0] = 0; // phv-lo
    cfg.salu_cmp_bsrc_input[1][1] = 1; // phv-hi
    cfg.salu_cmp_bsrc_sign[1][0] = 0;
    cfg.salu_cmp_bsrc_sign[1][1] = 1;
    cfg.salu_cmp_bsrc_enable[1][0] = 1;
    cfg.salu_cmp_bsrc_enable[1][1] = 1;
    cfg.salu_cmp_asrc_input[1][0] = 0; // ram-lo
    cfg.salu_cmp_asrc_input[1][1] = 1; // ram-hi
    cfg.salu_cmp_asrc_sign[1][0] = 0;
    cfg.salu_cmp_asrc_sign[1][1] = 0;
    cfg.salu_cmp_asrc_enable[1][0] = 1;
    cfg.salu_cmp_asrc_enable[1][1] = 1;
    cfg.salu_cmp_opcode[1][0] = 12;
    cfg.salu_cmp_opcode[1][1] = 12;
    setup_salu(&cfg, &tu, 1);

    addr = 14;
    phv.set_word(0x000F4240800001FF, 0);
    data_in.set_word(0x0000000000000000, 0);
    data_in.set_word(0xFFFF424059682F00, 64);
    //tu.update_log_flags(UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0));
    alus[0]->reset_resources();
    alus[0]->calculate_output(mk_addr(1, addr, 8, false), phv, &data_in, &data_out, &action,
                              present_time,ingress,match_bus,learn_or_match_bus);
    //tu.update_log_flags(UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0));
    lo_exp = data_in.get_word(0);
    hi_exp = data_in.get_word(64);
    lo_out = data_out.get_word(0);
    hi_out = data_out.get_word(64);
    EXPECT_EQ(static_cast<uint32_t>(2 << 4 | 1), action.get_word(0,32));
    EXPECT_EQ(lo_exp, lo_out);
    EXPECT_EQ(hi_exp, hi_out);

    // Single width 8 bit Lo: 0xFF + 0xFF + 0x80 <=UUS 0 (false)
    //                    Hi: 0xFF - 1 + 0 <=UUS 0 (true)
    cfg = base_cfg[0];
    cfg.salu_datasize[1] = 3;
    cfg.salu_op_dual[1] = 0;
    cfg.salu_cmp_const_src[1][0] = 0;
    cfg.salu_cmp_const_src[1][1] = 0;
    cfg.salu_cmp_regfile_const[1][0] = 1;
    cfg.salu_cmp_regfile_const[1][1] = 0;
    cfg.salu_cmp_bsrc_input[1][0] = 0; // phv-lo
    cfg.salu_cmp_bsrc_input[1][1] = 1; // phv-hi
    cfg.salu_cmp_bsrc_sign[1][0] = 0;
    cfg.salu_cmp_bsrc_sign[1][1] = 1;
    cfg.salu_cmp_bsrc_enable[1][0] = 1;
    cfg.salu_cmp_bsrc_enable[1][1] = 1;
    cfg.salu_cmp_asrc_input[1][0] = 0; // ram-lo
    cfg.salu_cmp_asrc_input[1][1] = 1; // ram-hi
    cfg.salu_cmp_asrc_sign[1][0] = 0;
    cfg.salu_cmp_asrc_sign[1][1] = 0;
    cfg.salu_cmp_asrc_enable[1][0] = 1;
    cfg.salu_cmp_asrc_enable[1][1] = 1;
    cfg.salu_cmp_opcode[1][0] = 13;
    cfg.salu_cmp_opcode[1][1] = 13;
    setup_salu(&cfg, &tu, 1);

    addr = 14;
    phv.set_word(0x000F4240800001FF, 0);
    data_in.set_word(0x0000000000000000, 0);
    data_in.set_word(0xFFFF424059682F00, 64);
    //tu.update_log_flags(UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0));
    alus[0]->reset_resources();
    alus[0]->calculate_output(mk_addr(1, addr, 8, false), phv, &data_in, &data_out, &action,
                              present_time,ingress,match_bus,learn_or_match_bus);
    //tu.update_log_flags(UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0));
    lo_exp = data_in.get_word(0);
    hi_exp = data_in.get_word(64);
    lo_out = data_out.get_word(0);
    hi_out = data_out.get_word(64);
    EXPECT_EQ(static_cast<uint32_t>(4 << 4 | 1), action.get_word(0,32));
    EXPECT_EQ(lo_exp, lo_out);
    EXPECT_EQ(hi_exp, hi_out);

    // Single width 8 bit Lo: 0xFF + 0xFF + 0x80 >=UUS 0 (true)
    //                    Hi: 0xFF - 1 + 0 >=UUS 0 (false)
    cfg = base_cfg[0];
    cfg.salu_datasize[1] = 3;
    cfg.salu_op_dual[1] = 0;
    cfg.salu_cmp_const_src[1][0] = 0;
    cfg.salu_cmp_const_src[1][1] = 0;
    cfg.salu_cmp_regfile_const[1][0] = 1;
    cfg.salu_cmp_regfile_const[1][1] = 0;
    cfg.salu_cmp_bsrc_input[1][0] = 0; // phv-lo
    cfg.salu_cmp_bsrc_input[1][1] = 1; // phv-hi
    cfg.salu_cmp_bsrc_sign[1][0] = 0;
    cfg.salu_cmp_bsrc_sign[1][1] = 1;
    cfg.salu_cmp_bsrc_enable[1][0] = 1;
    cfg.salu_cmp_bsrc_enable[1][1] = 1;
    cfg.salu_cmp_asrc_input[1][0] = 0; // ram-lo
    cfg.salu_cmp_asrc_input[1][1] = 1; // ram-hi
    cfg.salu_cmp_asrc_sign[1][0] = 0;
    cfg.salu_cmp_asrc_sign[1][1] = 0;
    cfg.salu_cmp_asrc_enable[1][0] = 1;
    cfg.salu_cmp_asrc_enable[1][1] = 1;
    cfg.salu_cmp_opcode[1][0] = 14;
    cfg.salu_cmp_opcode[1][1] = 14;
    setup_salu(&cfg, &tu, 1);

    addr = 14;
    phv.set_word(0x000F4240800001FF, 0);
    data_in.set_word(0x0000000000000000, 0);
    data_in.set_word(0xFFFF424059682F00, 64);
    //tu.update_log_flags(UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0));
    alus[0]->reset_resources();
    alus[0]->calculate_output(mk_addr(1, addr, 8, false), phv, &data_in, &data_out, &action,
                              present_time,ingress,match_bus,learn_or_match_bus);
    //tu.update_log_flags(UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0));
    lo_exp = data_in.get_word(0);
    hi_exp = data_in.get_word(64);
    lo_out = data_out.get_word(0);
    hi_out = data_out.get_word(64);
    EXPECT_EQ(static_cast<uint32_t>(2 << 4 | 1), action.get_word(0,32));
    EXPECT_EQ(lo_exp, lo_out);
    EXPECT_EQ(hi_exp, hi_out);

    // Single width 8 bit Lo: 0xFF + 0xFF + 0x80 <UUS 0 (false)
    //                    Hi: 0xFF - 1 + 0 <UUS 0 (true)
    cfg = base_cfg[0];
    cfg.salu_datasize[1] = 3;
    cfg.salu_op_dual[1] = 0;
    cfg.salu_cmp_const_src[1][0] = 0;
    cfg.salu_cmp_const_src[1][1] = 0;
    cfg.salu_cmp_regfile_const[1][0] = 1;
    cfg.salu_cmp_regfile_const[1][1] = 0;
    cfg.salu_cmp_bsrc_input[1][0] = 0; // phv-lo
    cfg.salu_cmp_bsrc_input[1][1] = 1; // phv-hi
    cfg.salu_cmp_bsrc_sign[1][0] = 0;
    cfg.salu_cmp_bsrc_sign[1][1] = 1;
    cfg.salu_cmp_bsrc_enable[1][0] = 1;
    cfg.salu_cmp_bsrc_enable[1][1] = 1;
    cfg.salu_cmp_asrc_input[1][0] = 0; // ram-lo
    cfg.salu_cmp_asrc_input[1][1] = 1; // ram-hi
    cfg.salu_cmp_asrc_sign[1][0] = 0;
    cfg.salu_cmp_asrc_sign[1][1] = 0;
    cfg.salu_cmp_asrc_enable[1][0] = 1;
    cfg.salu_cmp_asrc_enable[1][1] = 1;
    cfg.salu_cmp_opcode[1][0] = 15;
    cfg.salu_cmp_opcode[1][1] = 15;
    setup_salu(&cfg, &tu, 1);

    addr = 14;
    phv.set_word(0x000F4240800001FF, 0);
    data_in.set_word(0x0000000000000000, 0);
    data_in.set_word(0xFFFF424059682F00, 64);
    //tu.update_log_flags(UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0));
    alus[0]->reset_resources();
    alus[0]->calculate_output(mk_addr(1, addr, 8, false), phv, &data_in, &data_out, &action,
                              present_time,ingress,match_bus,learn_or_match_bus);
    //tu.update_log_flags(UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0));
    lo_exp = data_in.get_word(0);
    hi_exp = data_in.get_word(64);
    lo_out = data_out.get_word(0);
    hi_out = data_out.get_word(64);
    EXPECT_EQ(static_cast<uint32_t>(4 << 4 | 1), action.get_word(0,32));
    EXPECT_EQ(lo_exp, lo_out);
    EXPECT_EQ(hi_exp, hi_out);



    //
    //
    //
    // State ALU Logical Ops
    //
    //
    //

    BitVector<128> data_exp = {};
    uint32_t exp_action = 0;
    for (int logical_op=0; logical_op<16; ++logical_op) {
      for (int dual=0; dual<2; ++dual) {
        cfg = base_cfg[0];
        cfg.salu_datasize[2] = 3;
        cfg.salu_op_dual[2] = dual;
        cfg.salu_pred[2][1] = salu_cfg::kAllCmp; // ALU1-Lo
        cfg.salu_pred[2][3] = salu_cfg::kAllCmp; // ALU1-Hi
        cfg.salu_pred[2][0] = salu_cfg::kAllCmp; // ALU2-Lo
        cfg.salu_pred[2][2] = salu_cfg::kAllCmp; // ALU2-Hi
        cfg.salu_asrc_memory[2][1] = 1; // A = Ram Word (ALU1-Lo)
        cfg.salu_asrc_memory[2][3] = 1; // A = Ram Word (ALU1-Hi)
        cfg.salu_asrc_memory[2][0] = 1; // A = Ram Word (ALU2-Lo)
        cfg.salu_asrc_memory[2][2] = 1; // A = Ram Word (ALU2-Hi)
        cfg.salu_bsrc_phv[2][1] = 1;    // B = PHV (ALU1-Lo)
        cfg.salu_bsrc_phv[2][3] = 1;    // B = PHV (ALU1-Hi)
        cfg.salu_bsrc_phv[2][0] = 1;    // B = PHV (ALU2-Lo)
        cfg.salu_bsrc_phv[2][2] = 1;    // B = PHV (ALU2-Hi)
        cfg.salu_asrc_memory_index[2][1] = 0; // A = Ram Word Lo (ALU1-Lo)
        cfg.salu_asrc_memory_index[2][3] = 1; // A = Ram Word Hi (ALU1-Hi)
        cfg.salu_asrc_memory_index[2][0] = 0; // A = Ram Word Lo (ALU2-Lo)
        cfg.salu_asrc_memory_index[2][2] = 1; // A = Ram Word Hi (ALU2-Hi)
        cfg.salu_bsrc_phv_index[2][1] = 0;    // B = PHV Lo (ALU1-Lo)
        cfg.salu_bsrc_phv_index[2][3] = 1;    // B = PHV Hi (ALU1-Hi)
        cfg.salu_bsrc_phv_index[2][0] = 0;    // B = PHV Lo (ALU2-Lo)
        cfg.salu_bsrc_phv_index[2][2] = 1;    // B = PHV Hi (ALU2-Hi)
        cfg.salu_output_asrc[2] = 4;     // Always output ALU-Hi
        cfg.salu_output_cmpfn[2] = salu_cfg::kAllCmp;  // No predication on output ALU
        cfg.salu_op[2][1] = logical_op;
        cfg.salu_op[2][3] = logical_op;
        cfg.salu_op[2][0] = logical_op;
        cfg.salu_op[2][2] = logical_op;
        setup_salu(&cfg, &tu, 1);

        // PHV Hi/Lo CC/33
        // Double Width RamHi/Lo DD/44
        // Single Width RamHi/Lo 66/55
        addr = 7;
        phv.set_word(0xFFFFFFFFFFFFCC33, 0);
        data_in.set_word(0x55FFFFFFFFFFFFFF, 0);
        data_in.set_word(0xDD44FFFFFFFFFF66, 64);
        data_exp.copy_from(data_in);

        uint8_t A_hi_d = 0xDD;
        uint8_t A_lo_d = 0x44;
        uint8_t A_hi_s = 0x66;
        uint8_t A_lo_s = 0x55;
        uint8_t B_hi = 0xCC;
        uint8_t B_lo =0x33;
        switch (logical_op) {
          case 0: // SetZ
            if (dual) {
              data_exp.set_word(0x55FFFFFFFFFFFFFF, 0);
              data_exp.set_word(0x0000FFFFFFFFFF66, 64);
            } else {
              data_exp.set_word(0x00FFFFFFFFFFFFFF, 0);
              data_exp.set_word(0xDD44FFFFFFFFFF66, 64);
            }
            exp_action = 0;
            break;
          case 1: // Nor
            if (dual) {
              data_exp.set_word(0x55FFFFFFFFFFFFFF, 0);
              data_exp.set_word(0x2288FFFFFFFFFF66, 64);
              exp_action = 0x22;
            } else {
              data_exp.set_word(0x88FFFFFFFFFFFFFF, 0);
              data_exp.set_word(0xDD44FFFFFFFFFF66, 64);
              exp_action = 0x11;
            }
            break;
          case 2: // AND-CA
            if (dual) {
              uint8_t lo = ~A_lo_d & B_lo;
              uint8_t hi = ~A_hi_d & B_hi;
              data_exp.set_byte(lo, 14);
              data_exp.set_byte(hi, 15);
              exp_action = hi;
            } else {
              uint8_t lo = ~A_lo_s & B_lo;
              uint8_t hi = ~A_hi_s & B_hi;
              data_exp.set_byte(lo, 7);
              exp_action = hi;
            }
            break;
          case 3: // NOT A
            if (dual) {
              uint8_t lo = ~A_lo_d;
              uint8_t hi = ~A_hi_d;
              data_exp.set_byte(lo, 14);
              data_exp.set_byte(hi, 15);
              exp_action = hi;
            } else {
              uint8_t lo = ~A_lo_s;
              uint8_t hi = ~A_hi_s;
              data_exp.set_byte(lo, 7);
              exp_action = hi;
            }
            break;
          case 4: // AND-WIP
            if (dual) {
              uint8_t lo = A_lo_d & ~B_lo;
              uint8_t hi = A_hi_d & ~B_hi;
              data_exp.set_byte(lo, 14);
              data_exp.set_byte(hi, 15);
              exp_action = hi;
            } else {
              uint8_t lo = A_lo_s & ~B_lo;
              uint8_t hi = A_hi_s & ~B_hi;
              data_exp.set_byte(lo, 7);
              exp_action = hi;
            }
            break;
          case 5: // NOT B
            if (dual) {
              uint8_t lo = ~B_lo;
              uint8_t hi = ~B_hi;
              data_exp.set_byte(lo, 14);
              data_exp.set_byte(hi, 15);
              exp_action = hi;
            } else {
              uint8_t lo = ~B_lo;
              uint8_t hi = ~B_hi;
              data_exp.set_byte(lo, 7);
              exp_action = hi;
            }
            break;
          case 6: // XOR
            if (dual) {
              uint8_t lo = A_lo_d ^ B_lo;
              uint8_t hi = A_hi_d ^ B_hi;
              data_exp.set_byte(lo, 14);
              data_exp.set_byte(hi, 15);
              exp_action = hi;
            } else {
              uint8_t lo = A_lo_s ^ B_lo;
              uint8_t hi = A_hi_s ^ B_hi;
              data_exp.set_byte(lo, 7);
              exp_action = hi;
            }
            break;
          case 7: // NAND
            if (dual) {
              uint8_t lo = ~A_lo_d | ~B_lo;
              uint8_t hi = ~A_hi_d | ~B_hi;
              data_exp.set_byte(lo, 14);
              data_exp.set_byte(hi, 15);
              exp_action = hi;
            } else {
              uint8_t lo = ~A_lo_s | ~B_lo;
              uint8_t hi = ~A_hi_s | ~B_hi;
              data_exp.set_byte(lo, 7);
              exp_action = hi;
            }
            break;
          case 8: // AND
            if (dual) {
              uint8_t lo = A_lo_d & B_lo;
              uint8_t hi = A_hi_d & B_hi;
              data_exp.set_byte(lo, 14);
              data_exp.set_byte(hi, 15);
              exp_action = hi;
            } else {
              uint8_t lo = A_lo_s & B_lo;
              uint8_t hi = A_hi_s & B_hi;
              data_exp.set_byte(lo, 7);
              exp_action = hi;
            }
            break;
          case 9: // XNOR
            if (dual) {
              uint8_t lo = ~(A_lo_d ^ B_lo);
              uint8_t hi = ~(A_hi_d ^ B_hi);
              data_exp.set_byte(lo, 14);
              data_exp.set_byte(hi, 15);
              exp_action = hi;
            } else {
              uint8_t lo = ~(A_lo_s ^ B_lo);
              uint8_t hi = ~(A_hi_s ^ B_hi);
              data_exp.set_byte(lo, 7);
              exp_action = hi;
            }
            break;
          case 10: // B
            if (dual) {
              uint8_t lo = B_lo;
              uint8_t hi = B_hi;
              data_exp.set_byte(lo, 14);
              data_exp.set_byte(hi, 15);
              exp_action = hi;
            } else {
              uint8_t lo = B_lo;
              uint8_t hi = B_hi;
              data_exp.set_byte(lo, 7);
              exp_action = hi;
            }
            break;
          case 11: // OR CA
            if (dual) {
              uint8_t lo = ~A_lo_d | B_lo;
              uint8_t hi = ~A_hi_d | B_hi;
              data_exp.set_byte(lo, 14);
              data_exp.set_byte(hi, 15);
              exp_action = hi;
            } else {
              uint8_t lo = ~A_lo_s | B_lo;
              uint8_t hi = ~A_hi_s | B_hi;
              data_exp.set_byte(lo, 7);
              exp_action = hi;
            }
            break;
          case 12: // A
            if (dual) {
              uint8_t lo = A_lo_d;
              uint8_t hi = A_hi_d;
              data_exp.set_byte(lo, 14);
              data_exp.set_byte(hi, 15);
              exp_action = hi;
            } else {
              uint8_t lo = A_lo_s;
              uint8_t hi = A_hi_s;
              data_exp.set_byte(lo, 7);
              exp_action = hi;
            }
            break;
          case 13: // OR WIP
            if (dual) {
              uint8_t lo = A_lo_d | ~B_lo;
              uint8_t hi = A_hi_d | ~B_hi;
              data_exp.set_byte(lo, 14);
              data_exp.set_byte(hi, 15);
              exp_action = hi;
            } else {
              uint8_t lo = A_lo_s | ~B_lo;
              uint8_t hi = A_hi_s | ~B_hi;
              data_exp.set_byte(lo, 7);
              exp_action = hi;
            }
            break;
          case 14: // OR
            if (dual) {
              uint8_t lo = A_lo_d | B_lo;
              uint8_t hi = A_hi_d | B_hi;
              data_exp.set_byte(lo, 14);
              data_exp.set_byte(hi, 15);
              exp_action = hi;
            } else {
              uint8_t lo = A_lo_s | B_lo;
              uint8_t hi = A_hi_s | B_hi;
              data_exp.set_byte(lo, 7);
              exp_action = hi;
            }
            break;
          case 15: // HI
            if (dual) {
              uint8_t lo = 0xFF;
              uint8_t hi = 0xFF;
              data_exp.set_byte(lo, 14);
              data_exp.set_byte(hi, 15);
              exp_action = hi;
            } else {
              uint8_t lo = 0xFF;
              uint8_t hi = 0xFF;
              data_exp.set_byte(lo, 7);
              exp_action = hi;
            }
            break;
          default:
            break;
        }

        //tu.update_log_flags(UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0));
        alus[0]->reset_resources();
        alus[0]->calculate_output(mk_addr(2, addr, 8, dual), phv, &data_in, &data_out, &action,
                                  present_time,ingress,match_bus,learn_or_match_bus);
        //tu.update_log_flags(UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0));

        lo_exp = data_exp.get_word(0);
        hi_exp = data_exp.get_word(64);
        lo_out = data_out.get_word(0);
        hi_out = data_out.get_word(64);
        EXPECT_EQ(exp_action, action.get_word(0,32));
        EXPECT_EQ(lo_exp, lo_out);
        EXPECT_EQ(hi_exp, hi_out);
      }
    }



    //
    //
    //
    // State ALU Arithmetic Ops
    //
    //
    //

    for (int arithmetic_op=0; arithmetic_op<16; ++arithmetic_op) {
#ifdef MODEL_CHIP_JBAY_OR_LATER
      // TODO: write a test for AddC and SubC
      // This test doesn't work for JBay, these ops only work for width=32b dual
      if (arithmetic_op == 8 || arithmetic_op == 9) {
        continue;
      }
#endif
      for (int dual=0; dual<2; ++dual) {
        cfg = base_cfg[0];
        cfg.salu_datasize[2] = 4;
        cfg.salu_op_dual[2] = dual;
        cfg.salu_pred[2][1] = salu_cfg::kAllCmp; // ALU1-Lo
        cfg.salu_pred[2][3] = salu_cfg::kAllCmp; // ALU1-Hi
        cfg.salu_pred[2][0] = salu_cfg::kAllCmp; // ALU2-Lo
        cfg.salu_pred[2][2] = salu_cfg::kAllCmp; // ALU2-Hi
        cfg.salu_asrc_memory[2][1] = 1; // A = Ram Word (ALU1-Lo)
        cfg.salu_asrc_memory[2][3] = 1; // A = Ram Word (ALU1-Hi)
        cfg.salu_asrc_memory[2][0] = 1; // A = Ram Word (ALU2-Lo)
        cfg.salu_asrc_memory[2][2] = 1; // A = Ram Word (ALU2-Hi)
        cfg.salu_bsrc_phv[2][1] = 1;    // B = PHV (ALU1-Lo)
        cfg.salu_bsrc_phv[2][3] = 1;    // B = PHV (ALU1-Hi)
        cfg.salu_bsrc_phv[2][0] = 1;    // B = PHV (ALU2-Lo)
        cfg.salu_bsrc_phv[2][2] = 1;    // B = PHV (ALU2-Hi)
        cfg.salu_asrc_memory_index[2][1] = 0; // A = Ram Word Lo (ALU1-Lo)
        cfg.salu_asrc_memory_index[2][3] = 1; // A = Ram Word Hi (ALU1-Hi)
        cfg.salu_asrc_memory_index[2][0] = 0; // A = Ram Word Lo (ALU2-Lo)
        cfg.salu_asrc_memory_index[2][2] = 1; // A = Ram Word Hi (ALU2-Hi)
        cfg.salu_bsrc_phv_index[2][1] = 0;    // B = PHV Lo (ALU1-Lo)
        cfg.salu_bsrc_phv_index[2][3] = 1;    // B = PHV Hi (ALU1-Hi)
        cfg.salu_bsrc_phv_index[2][0] = 0;    // B = PHV Lo (ALU2-Lo)
        cfg.salu_bsrc_phv_index[2][2] = 1;    // B = PHV Hi (ALU2-Hi)
        cfg.salu_output_asrc[2] = 4;     // Always output ALU-Hi
        cfg.salu_output_cmpfn[2] = salu_cfg::kAllCmp;  // No predication on output ALU
        cfg.salu_arith[2][1] = 1;
        cfg.salu_arith[2][3] = 1;
        cfg.salu_arith[2][0] = 1;
        cfg.salu_arith[2][2] = 1;
        cfg.salu_op[2][1] = arithmetic_op;
        cfg.salu_op[2][3] = arithmetic_op;
        cfg.salu_op[2][0] = arithmetic_op;
        cfg.salu_op[2][2] = arithmetic_op;
        setup_salu(&cfg, &tu, 1);

        // PHV Hi/Lo 0xABCD/0x1234
        // Double Width RamHi/Lo 0x2222/0x1111
        // Single Width RamHi/Lo 0x4444/0x3333
        addr = 3;
        phv.set_word(0xFFFFFFF0FEED1234, 0);
        data_in.set_word(0x3333FFFFFFFFFFFF, 0);
        data_in.set_word(0x22221111FFFF4444, 64);
        data_exp.copy_from(data_in);

        uint64_t A_hi_d = 0x2222;
        uint64_t A_lo_d = 0x1111;
        uint64_t A_hi_s = 0x4444;
        uint64_t A_lo_s = 0x3333;
        uint64_t B_hi = 0xFEED;
        uint64_t B_lo =0x1234;
        uint64_t hi = 0, lo = 0;
        uint64_t x = 0;
        int64_t  y = 0;
        switch (arithmetic_op) {
          case 0: // Sat Unsigned Add
            if (dual) {
              lo = (A_lo_d + B_lo) & 0xFFFF;
              lo = (lo < A_lo_d) ? 0xFFFF : ((lo < B_lo) ? 0xFFFF : lo);
              hi = (A_hi_d + B_hi) & 0xFFFF;
              hi = (hi < A_hi_d) ? 0xFFFF : ((hi < B_hi) ? 0xFFFF : hi);
              data_exp.set_word(lo,  96, 16);
              data_exp.set_word(hi, 112, 16);
            } else {
              lo = (A_lo_s + B_lo) & 0xFFFF;
              lo = (lo < A_lo_s) ? 0xFFFF : ((lo < B_lo) ? 0xFFFF : lo);
              hi = (A_hi_s + B_hi) & 0xFFFF;
              hi = (hi < A_hi_s) ? 0xFFFF : ((hi < B_hi) ? 0xFFFF : hi);
              data_exp.set_word(lo, 48, 16);
            }
            exp_action = hi & 0xFFFF;
            break;
          case 1: // Sat Signed Add
            if (dual) {
              y = (int16_t)A_lo_d + (int16_t)B_lo;
              lo = y < -32768 ? -32768 : y;
              lo = y >  32767 ? 32767 : y;
              y = (int16_t)A_hi_d + (int16_t)B_hi;
              hi = y < -32768 ? -32768 : y;
              hi = y >  32767 ? 32767 : y;
              data_exp.set_word(lo,  96, 16);
              data_exp.set_word(hi, 112, 16);
            } else {
              y = (int16_t)A_lo_s + (int16_t)B_lo;
              lo = y < -32768 ? -32768 : y;
              lo = y >  32767 ? 32767 : y;
              y = (int16_t)A_hi_s + (int16_t)B_hi;
              hi = y < -32768 ? -32768 : y;
              hi = y >  32767 ? 32767 : y;
              data_exp.set_word(lo, 48, 16);
            }
            exp_action = hi & 0xFFFF;
            break;
          case 2: // Sat Unsigned Sub
            if (dual) {
              x  = A_lo_d - B_lo;
              lo = (x & ~UINT64_C(0xFFFF)) ? 0 : x;
              x  = A_hi_d - B_hi;
              hi = (x & ~UINT64_C(0xFFFF)) ? 0 : x;
              data_exp.set_word(lo,  96, 16);
              data_exp.set_word(hi, 112, 16);
            } else {
              x  = A_lo_s - B_lo;
              lo = (x & ~UINT64_C(0xFFFF)) ? 0 : x;
              x  = A_hi_s - B_hi;
              hi = (x & ~UINT64_C(0xFFFF)) ? 0 : x;
              data_exp.set_word(lo, 48, 16);
            }
            exp_action = hi;
            break;
          case 3: // Sat Signed Sub
            if (dual) {
              y  = (int16_t)A_lo_d - (int16_t)B_lo;
              lo = y < -32768 ? -32768 : y;
              lo = y >  32767 ?  32767 : y;
              y  = (int16_t)A_hi_d - (int16_t)B_hi;
              hi = y < -32768 ? -32768 : y;
              hi = y >  32767 ?  32767 : y;
              data_exp.set_word(lo,  96, 16);
              data_exp.set_word(hi, 112, 16);
            } else {
              y  = (int16_t)A_lo_s - (int16_t)B_lo;
              lo = y < -32768 ? -32768 : y;
              lo = y >  32767 ?  32767 : y;
              y  = (int16_t)A_hi_s - (int16_t)B_hi;
              hi = y < -32768 ? -32768 : y;
              hi = y >  32767 ?  32767 : y;
              data_exp.set_word(lo, 48, 16);
            }
            exp_action = hi & 0xFFFF;
            break;
          case 4: // Min Unsigned
            if (dual) {
              lo = A_lo_d < B_lo ? A_lo_d : B_lo;
              hi = A_hi_d < B_hi ? A_hi_d : B_hi;
              data_exp.set_word(lo,  96, 16);
              data_exp.set_word(hi, 112, 16);
            } else {
              lo = A_lo_s < B_lo ? A_lo_s : B_lo;
              hi = A_hi_s < B_hi ? A_hi_s : B_hi;
              data_exp.set_word(lo, 48, 16);
            }
            exp_action = hi;
            break;
          case 5: // Min Signed
            if (dual) {
              lo = (int16_t)A_lo_d < (int16_t)B_lo ? A_lo_d : B_lo;
              hi = (int16_t)A_hi_d < (int16_t)B_hi ? A_hi_d : B_hi;
              data_exp.set_word(lo,  96, 16);
              data_exp.set_word(hi, 112, 16);
            } else {
              lo = (int16_t)A_lo_s < (int16_t)B_lo ? A_lo_s : B_lo;
              hi = (int16_t)A_hi_s < (int16_t)B_hi ? A_hi_s : B_hi;
              data_exp.set_word(lo, 48, 16);
            }
            exp_action = hi & 0xFFFF;
            break;
          case 6: // Max Unsigned
            if (dual) {
              lo = A_lo_d > B_lo ? A_lo_d : B_lo;
              hi = A_hi_d > B_hi ? A_hi_d : B_hi;
              data_exp.set_word(lo,  96, 16);
              data_exp.set_word(hi, 112, 16);
            } else {
              lo = A_lo_s > B_lo ? A_lo_s : B_lo;
              hi = A_hi_s > B_hi ? A_hi_s : B_hi;
              data_exp.set_word(lo, 48, 16);
            }
            exp_action = hi;
            break;
          case 7: // Max Signed
            if (dual) {
              lo = (int16_t)A_lo_d > (int16_t)B_lo ? A_lo_d : B_lo;
              hi = (int16_t)A_hi_d > (int16_t)B_hi ? A_hi_d : B_hi;
              data_exp.set_word(lo,  96, 16);
              data_exp.set_word(hi, 112, 16);
            } else {
              lo = (int16_t)A_lo_s > (int16_t)B_lo ? A_lo_s : B_lo;
              hi = (int16_t)A_hi_s > (int16_t)B_hi ? A_hi_s : B_hi;
              data_exp.set_word(lo, 48, 16);
            }
            exp_action = hi;
            break;
          case 8: // NOP
            if (dual) {
              lo = 0;
              hi = 0;
              data_exp.set_word(lo,  96, 16);
              data_exp.set_word(hi, 112, 16);
            } else {
              lo = 0;
              hi = 0;
              data_exp.set_word(lo, 48, 16);
            }
            exp_action = hi;
            break;
          case 9: // NOP
            if (dual) {
              lo = 0;
              hi = 0;
              data_exp.set_word(lo,  96, 16);
              data_exp.set_word(hi, 112, 16);
            } else {
              lo = 0;
              hi = 0;
              data_exp.set_word(lo, 48, 16);
            }
            exp_action = hi;
            break;
          case 10: // Sat Unsigned Sub Reverse
            if (dual) {
              x  = B_lo - A_lo_d;
              lo = (x & ~UINT64_C(0xFFFF)) ? 0 : x;
              x  = B_hi - A_hi_d;
              hi = (x & ~UINT64_C(0xFFFF)) ? 0 : x;
              data_exp.set_word(lo,  96, 16);
              data_exp.set_word(hi, 112, 16);
            } else {
              x  = B_lo - A_lo_s;
              lo = (x & ~UINT64_C(0xFFFF)) ? 0 : x;
              x  = B_hi - A_hi_s;
              hi = (x & ~UINT64_C(0xFFFF)) ? 0 : x;
              data_exp.set_word(lo, 48, 16);
            }
            exp_action = hi;
            break;
          case 11: // Sat Signed Sub Reverse
            if (dual) {
              y  = (int16_t)B_lo - (int16_t)A_lo_d;
              lo = y < -32768 ? -32768 : y;
              lo = y >  32767 ?  32767 : y;
              y  = (int16_t)B_hi - (int16_t)A_hi_d;
              hi = y < -32768 ? -32768 : y;
              hi = y >  32767 ?  32767 : y;
              data_exp.set_word(lo,  96, 16);
              data_exp.set_word(hi, 112, 16);
            } else {
              y  = (int16_t)B_lo - (int16_t)A_lo_s;
              lo = y < -32768 ? -32768 : y;
              lo = y >  32767 ?  32767 : y;
              y  = (int16_t)B_hi - (int16_t)A_hi_s;
              hi = y < -32768 ? -32768 : y;
              hi = y >  32767 ?  32767 : y;
              data_exp.set_word(lo, 48, 16);
            }
            exp_action = hi & 0xFFFF;
            break;
          case 12: // Add
            if (dual) {
              lo = (A_lo_d + B_lo) & 0xFFFF;
              hi = (A_hi_d + B_hi) & 0xFFFF;
              data_exp.set_word(lo,  96, 16);
              data_exp.set_word(hi, 112, 16);
            } else {
              lo = (A_lo_s + B_lo) & 0xFFFF;
              hi = (A_hi_s + B_hi) & 0xFFFF;
              data_exp.set_word(lo, 48, 16);
            }
            exp_action = hi & 0xFFFF;
            break;
          case 13: // NOP
            if (dual) {
              lo = 0;
              hi = 0;
              data_exp.set_word(lo,  96, 16);
              data_exp.set_word(hi, 112, 16);
            } else {
              lo = 0;
              hi = 0;
              data_exp.set_word(lo, 48, 16);
            }
            exp_action = hi;
            break;
          case 14: // Sub
            if (dual) {
              lo = (A_lo_d - B_lo) & 0xFFFF;
              hi = (A_hi_d - B_hi) & 0xFFFF;
              data_exp.set_word(lo,  96, 16);
              data_exp.set_word(hi, 112, 16);
            } else {
              lo = (A_lo_s - B_lo) & 0xFFFF;
              hi = (A_hi_s - B_hi) & 0xFFFF;
              data_exp.set_word(lo, 48, 16);
            }
            exp_action = hi & 0xFFFF;
            break;
          case 15: // Sub Reverse
            if (dual) {
              lo = (B_lo - A_lo_d) & 0xFFFF;
              hi = (B_hi - A_hi_d) & 0xFFFF;
              data_exp.set_word(lo,  96, 16);
              data_exp.set_word(hi, 112, 16);
            } else {
              lo = (B_lo - A_lo_s) & 0xFFFF;
              hi = (B_hi - A_hi_s) & 0xFFFF;
              data_exp.set_word(lo, 48, 16);
            }
            exp_action = hi & 0xFFFF;
            break;
        }
        //tu.update_log_flags(UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0));
        alus[0]->reset_resources();
        alus[0]->calculate_output(mk_addr(2, addr, 16, dual), phv, &data_in, &data_out, &action,
                                  present_time,ingress,match_bus,learn_or_match_bus);
        //tu.update_log_flags(UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0));

        lo_exp = data_exp.get_word(0);
        hi_exp = data_exp.get_word(64);
        lo_out = data_out.get_word(0);
        hi_out = data_out.get_word(64);
        EXPECT_EQ(exp_action, action.get_word(0,32));
        EXPECT_EQ(lo_exp, lo_out);
        EXPECT_EQ(hi_exp, hi_out);
      }
    }


#ifdef MODEL_CHIP_JBAY_OR_LATER
    // Single width 32 bit RNG Lo: 0x00000000 + 0x76543210 + 0xFFFF0000 > 0 (true)
    //                         Hi: 0 + 0 + 3 > 0 (true)
    cfg = base_cfg[0];
    cfg.salu_datasize[1] = 5;
    cfg.salu_op_dual[1] = 0;
    cfg.salu_cmp_const_src[1][0] = 3;
    cfg.salu_cmp_const_src[1][1] = 1;
    cfg.salu_cmp_regfile_const[1][0] = 1;
    cfg.salu_cmp_regfile_const[1][1] = 1;
    cfg.salu_cmp_bsrc_input[1][0] = 2; // RNG
    cfg.salu_cmp_bsrc_sign[1][0] = 0;
    cfg.salu_cmp_bsrc_enable[1][0] = 1;
    cfg.salu_cmp_asrc_input[1][0] = 0; // ram-lo
    cfg.salu_cmp_asrc_sign[1][0] = 0;
    cfg.salu_cmp_asrc_enable[1][0] = 1;
    cfg.salu_cmp_opcode[1][0] = 4;
    cfg.salu_cmp_opcode[1][1] = 4;
    setup_salu(&cfg, &tu, 1);

    addr = 2;
    phv.set_word(0xFEDCBA9800000000, 0);
    data_in.set_word(0x0123456789ABCDEF, 0);
    data_in.set_word(0x8877665500000000, 64);
    //tu.update_log_flags(UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0));
    alus[0]->reset_resources();
    alus[0]->set_random_number_value( UINT64_C( 0x76543210 ));
    alus[0]->calculate_output(mk_addr(1, addr, 32, false), phv, &data_in, &data_out, &action,
                              present_time,ingress,match_bus,learn_or_match_bus);
    //tu.update_log_flags(UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0));
    lo_exp = data_in.get_word(0);
    hi_exp = data_in.get_word(64);
    lo_out = data_out.get_word(0);
    hi_out = data_out.get_word(64);
    EXPECT_EQ(static_cast<uint32_t>(8 << 4 | 1), action.get_word(0,32));
    EXPECT_EQ(lo_exp, lo_out);
    EXPECT_EQ(hi_exp, hi_out);

    // Single width 32 bit RNG Lo: 0x00001234 + 0x0 + 0xFFFF0000 > 0 (false)
    //                         Hi: 0 + 0 + 3 > 0 (true)
    cfg = base_cfg[0];
    cfg.salu_datasize[1] = 5;
    cfg.salu_op_dual[1] = 0;
    cfg.salu_cmp_const_src[1][0] = 3;
    cfg.salu_cmp_const_src[1][1] = 1;
    cfg.salu_cmp_regfile_const[1][0] = 1;
    cfg.salu_cmp_regfile_const[1][1] = 1;
    cfg.salu_cmp_bsrc_input[1][0] = 2; // RNG
    cfg.salu_cmp_bsrc_sign[1][0] = 0;
    cfg.salu_cmp_bsrc_enable[1][0] = 1;
    cfg.salu_cmp_asrc_input[1][0] = 0; // ram-lo
    cfg.salu_cmp_asrc_sign[1][0] = 0;
    cfg.salu_cmp_asrc_enable[1][0] = 1;
    cfg.salu_cmp_opcode[1][0] = 4;
    cfg.salu_cmp_opcode[1][1] = 4;
    setup_salu(&cfg, &tu, 1);

    addr = 2;
    phv.set_word(0xFEDCBA9800000000, 0);
    data_in.set_word(0x0123456789ABCDEF, 0);
    data_in.set_word(0x8877665500000000, 64);
    //tu.update_log_flags(UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0));
    alus[0]->reset_resources();
    alus[0]->set_random_number_value( UINT64_C( 0x00001234 ));
    alus[0]->calculate_output(mk_addr(1, addr, 32, false), phv, &data_in, &data_out, &action,
                              present_time,ingress,match_bus,learn_or_match_bus);
    //tu.update_log_flags(UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0));
    lo_exp = data_in.get_word(0);
    hi_exp = data_in.get_word(64);
    lo_out = data_out.get_word(0);
    hi_out = data_out.get_word(64);
    EXPECT_EQ(static_cast<uint32_t>(4 << 4 | 1), action.get_word(0,32));
    EXPECT_EQ(lo_exp, lo_out);
    EXPECT_EQ(hi_exp, hi_out);

#endif


    //
    //
    //
    // Instruction constants
    //
    //
    //
    {
#ifdef MODEL_CHIP_JBAY_OR_LATER
      int max_c = 0x3f;
      int special_value = 0x3E;
#else
      int max_c = 0xf;
      int special_value = 0xE;
#endif

      for (int c=0; c<max_c; ++c) {
        // Single width 32-bit:
        // if (B + C == 0) ALU-1-Lo = B'
        // Where B is PHV input and B' is instruction constant
        cfg = base_cfg[0];

        cfg.salu_cmp_const_src[2][0] = c;
        cfg.salu_cmp_regfile_const[2][0] = 0;
        cfg.salu_cmp_bsrc_input[2][0] = 1;
        cfg.salu_cmp_bsrc_sign[2][0] = 0;
        cfg.salu_cmp_bsrc_enable[2][0] = 1;
        cfg.salu_cmp_asrc_input[2][0] = 0;
        cfg.salu_cmp_asrc_sign[2][0] = 0;
        cfg.salu_cmp_asrc_enable[2][0] = 0;
        cfg.salu_cmp_opcode[2][0] = 0;

        cfg.salu_cmp_const_src[2][1] = 0;
        cfg.salu_cmp_regfile_const[2][1] = 0;
        cfg.salu_cmp_bsrc_input[2][1] = 0;
        cfg.salu_cmp_bsrc_sign[2][1] = 0;
        cfg.salu_cmp_bsrc_enable[2][1] = 0;
        cfg.salu_cmp_asrc_input[2][1] = 0;
        cfg.salu_cmp_asrc_sign[2][1] = 0;
        cfg.salu_cmp_asrc_enable[2][1] = 0;
        cfg.salu_cmp_opcode[2][1] = 1;


        cfg.salu_datasize[2] = 5;
        cfg.salu_op_dual[2] = 0;
        cfg.salu_pred[2][1] = 0x2; // ALU1-Lo
        cfg.salu_pred[2][3] = 0x0; // ALU1-Hi
        cfg.salu_pred[2][0] = 0x0; // ALU2-Lo
        cfg.salu_pred[2][2] = 0x0; // ALU2-Hi
        cfg.salu_asrc_memory[2][1] = 1; // A = Ram Word (ALU1-Lo)
        cfg.salu_bsrc_phv[2][1] = 0;    // B = Const (ALU1-Lo)
        cfg.salu_asrc_memory_index[2][1] = 0; // A = Ram Word Lo (ALU1-Lo)
        cfg.salu_regfile_const[2][1] = 0; // Const from instruciton
        cfg.salu_output_asrc[2] = 5;     // Always output ALU-Lo
        cfg.salu_output_cmpfn[2] = salu_cfg::kAllCmp;  // No predication on output ALU
        cfg.salu_arith[2][1] = 0; // Logical Op
        cfg.salu_op[2][1] = 10; // OpCode = logical B
        cfg.salu_const_src[2][1] = c; // Const value
        setup_salu(&cfg, &tu, 1);

        addr = 1;
        phv.set_word(UINT64_C(0x00000002FFFFFFFF), 0);
        data_in.set_word(0x12345678FFFFFFFF, 0);
        data_in.set_word(0xFFFFFFFFFFFFFFFF, 64);
        data_exp.copy_from(data_in);
        data_exp.set_word(special_value == c ? 0xFFFFFFFEFFFFFFFF:0x12345678FFFFFFFF , 0);
        exp_action = special_value == c ? 0xFFFFFFFE : 0x12345678;

        //tu.update_log_flags(UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0));
        alus[0]->reset_resources();
        alus[0]->calculate_output(mk_addr(2, addr, 32, false), phv, &data_in, &data_out, &action,
                                  present_time,ingress,match_bus,learn_or_match_bus);
        //tu.update_log_flags(UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0));

        lo_exp = data_exp.get_word(0);
        hi_exp = data_exp.get_word(64);
        lo_out = data_out.get_word(0);
        hi_out = data_out.get_word(64);
        EXPECT_EQ(exp_action, action.get_word(0,32));
        EXPECT_EQ(lo_exp, lo_out);
        EXPECT_EQ(hi_exp, hi_out);
      }

      //
      //
      //
      // Specific Use Cases
      //
      //
      //
      {
        cfg = base_cfg[0];
        cfg.salu_datasize[0] = 4;
        cfg.salu_op_dual[0] = 0;
        cfg.salu_output_cmpfn[0] = 0;
        cfg.salu_bsrc_phv_index[0][1] = 0; // PHV Lo
        cfg.salu_bsrc_phv[0][1] = 1; // Use PHV
        cfg.salu_asrc_memory_index[0][1] = 0; // RAM Lo
        cfg.salu_asrc_memory[0][1] = 1; // Use RAM
        cfg.salu_pred[0][1] = salu_cfg::kAllCmp;
        cfg.salu_arith[0][1] = 1;
        cfg.salu_op[0][1] = 1; // Saturating Add Signed

        setup_salu(&cfg, &tu, 1);

        addr = 0;
        phv.set_word(UINT64_C(0x0000000000005CDB), 0);
        data_in.set_word(0x0000000000005F91, 0);
        data_in.set_word(0x0000000000000000, 64);
        data_exp.copy_from(data_in);
        data_exp.set_word(0x0000000000007FFF , 0);
        exp_action = 0;
        //tu.update_log_flags(UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0));
        alus[0]->reset_resources();
        alus[0]->calculate_output(mk_addr(0, addr, 16, false), phv, &data_in, &data_out, &action,
                                  present_time,ingress,match_bus,learn_or_match_bus);
        //tu.update_log_flags(UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0));
        EXPECT_EQ(exp_action, action.get_word(0,32));
        EXPECT_EQ(data_exp.get_word(0), data_out.get_word(0));
        EXPECT_EQ(data_exp.get_word(64), data_out.get_word(64));

        phv.set_word(UINT64_C(0x000000000000B1E0), 0);
        data_in.set_word(0x000000000000B000, 0);
        data_in.set_word(0x0000000000000000, 64);
        data_exp.copy_from(data_in);
        data_exp.set_word(0x0000000000008000 , 0);
        exp_action = 0;
        //tu.update_log_flags(UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0));
        alus[0]->reset_resources();
        alus[0]->calculate_output(mk_addr(0, addr, 16, false), phv, &data_in, &data_out, &action,
                                  present_time,ingress,match_bus,learn_or_match_bus);
        //tu.update_log_flags(UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0));
        EXPECT_EQ(exp_action, action.get_word(0,32));
        EXPECT_EQ(data_exp.get_word(0), data_out.get_word(0));
        EXPECT_EQ(data_exp.get_word(64), data_out.get_word(64));
      }
      {
        // 64-bit Signed Counter
        cfg = base_cfg[0];

        // ram.lo < 0 signed
        cfg.salu_const_regfile[3] = 0x0;
        cfg.salu_cmp_const_src[3][0] = 3;
        cfg.salu_cmp_regfile_const[3][0] = 1;
        cfg.salu_cmp_bsrc_input[3][0] = 0;
        cfg.salu_cmp_bsrc_sign[3][0] = 0;
        cfg.salu_cmp_bsrc_enable[3][0] = 0;
        cfg.salu_cmp_asrc_input[3][0] = 0;
        cfg.salu_cmp_asrc_sign[3][0] = 0;
        cfg.salu_cmp_asrc_enable[3][0] = 1;
        cfg.salu_cmp_opcode[3][0] = 7;
        // ram.lo + phv.lo < 0 signed
        cfg.salu_cmp_const_src[3][1] = 0;
        cfg.salu_cmp_regfile_const[3][1] = 0;
        cfg.salu_cmp_bsrc_input[3][1] = 0;
        cfg.salu_cmp_bsrc_sign[3][1] = 0;
        cfg.salu_cmp_bsrc_enable[3][1] = 1;
        cfg.salu_cmp_asrc_input[3][1] = 0;
        cfg.salu_cmp_asrc_sign[3][1] = 0;
        cfg.salu_cmp_asrc_enable[3][1] = 1;
        cfg.salu_cmp_opcode[3][1] = 7;


        cfg.salu_datasize[3] = 5;
        cfg.salu_op_dual[3] = 1;
        cfg.salu_pred[3][1] = salu_cfg::kAllCmp; // ALU1-Lo
        cfg.salu_pred[3][0] = 0x0; // ALU2-Lo
        cfg.salu_pred[3][3] = 0x2; // ALU1-Hi (!cond_hi && cond_lo)
        cfg.salu_pred[3][2] = 0x4; // ALU2-Hi (cond_hi && !cond_lo)

        // ALU-1-Lo (ram.lo + phv.lo)
        cfg.salu_asrc_memory[3][1] = 1; // A = Ram Word (ALU1-Lo)
        cfg.salu_bsrc_phv[3][1] = 1;    // B = PHV (ALU1-Lo)
        cfg.salu_bsrc_phv_index[3][1] = 0;    // B = PHV-Lo (ALU1-Lo)
        cfg.salu_asrc_memory_index[3][1] = 0; // A = Ram Word Lo (ALU1-Lo)
        cfg.salu_op[3][1] = 12;
        cfg.salu_arith[3][1] = 1;

        // ALU-1-Hi (ram.hi + 1)
        cfg.salu_asrc_memory[3][3] = 1; // A = Ram Word
        cfg.salu_asrc_memory_index[3][3] = 1; // A = Ram Word Hi
        cfg.salu_bsrc_phv[3][3] = 0;    // B = Const
        cfg.salu_regfile_const[3][3] = 0;    // Instruction Const
        cfg.salu_const_src[3][3] = 1;    // Const value: 1
        cfg.salu_op[3][3] = 12;
        cfg.salu_arith[3][3] = 1;

        // ALU-2-Hi (ram.hi - 1)
        cfg.salu_asrc_memory[3][2] = 1; // A = Ram Word
        cfg.salu_asrc_memory_index[3][2] = 1; // A = Ram Word Hi
        cfg.salu_bsrc_phv[3][2] = 0;    // B = Const
        cfg.salu_regfile_const[3][2] = 0;    // Instruction Const
        cfg.salu_const_src[3][2] = 1;    // Const value: 1
        cfg.salu_op[3][2] = 14;
        cfg.salu_arith[3][2] = 1;

        cfg.salu_output_asrc[3] = 0;     // No output
        cfg.salu_output_cmpfn[3] = 0x0;  // No output
        setup_salu(&cfg, &tu, 1);

        // Increment by 5 twice.
        addr = 0;
        phv.set_word(UINT64_C(0x0000000000000005), 0);
        data_in.set_word(0x0000000000000000, 0);
        data_in.set_word(0x0000000000000000, 64);
        data_exp.copy_from(data_in);
        data_exp.set_word(0x000000000000000a , 0);
        exp_action = 0;
        //tu.update_log_flags(UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0));
        alus[0]->reset_resources();
        alus[0]->calculate_output(mk_addr(3, addr, 32, true), phv, &data_in, &data_out, &action,
                                  present_time,ingress,match_bus,learn_or_match_bus);
        alus[0]->reset_resources();
        alus[0]->calculate_output(mk_addr(3, addr, 32, true), phv, &data_out, &data_out, &action,
                                  present_time,ingress,match_bus,learn_or_match_bus);
        //tu.update_log_flags(UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0));
        EXPECT_EQ(exp_action, action.get_word(0,32));
        EXPECT_EQ(data_exp.get_word(0), data_out.get_word(0));
        EXPECT_EQ(data_exp.get_word(64), data_out.get_word(64));

        // Increment by -10 three times.
        phv.set_word(UINT64_C(0x00000000FFFFFFF6), 0);
        data_exp.set_word(0x0000000000000000, 0);
        //tu.update_log_flags(UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0));
        alus[0]->reset_resources();
        alus[0]->calculate_output(mk_addr(3, addr, 32, true), phv, &data_out, &data_out, &action,
                                  present_time,ingress,match_bus,learn_or_match_bus);
        //tu.update_log_flags(UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0));
        EXPECT_EQ(exp_action, action.get_word(0,32));
        EXPECT_EQ(data_exp.get_word(0), data_out.get_word(0));
        EXPECT_EQ(data_exp.get_word(64), data_out.get_word(64));
        data_exp.set_word(0xFFFFFFFFFFFFFFF6, 0);
        alus[0]->reset_resources();
        alus[0]->calculate_output(mk_addr(3, addr, 32, true), phv, &data_out, &data_out, &action,
                                  present_time,ingress,match_bus,learn_or_match_bus);
        EXPECT_EQ(exp_action, action.get_word(0,32));
        EXPECT_EQ(data_exp.get_word(0), data_out.get_word(0));
        EXPECT_EQ(data_exp.get_word(64), data_out.get_word(64));
        data_exp.set_word(0xFFFFFFFFFFFFFFEC, 0);
        alus[0]->reset_resources();
        alus[0]->calculate_output(mk_addr(3, addr, 32, true), phv, &data_out, &data_out, &action,
                                  present_time,ingress,match_bus,learn_or_match_bus);
        EXPECT_EQ(exp_action, action.get_word(0,32));
        EXPECT_EQ(data_exp.get_word(0), data_out.get_word(0));
        EXPECT_EQ(data_exp.get_word(64), data_out.get_word(64));
        // Add 1
        phv.set_word(UINT64_C(0x0000000000000001), 0);
        data_exp.set_word(0xFFFFFFFFFFFFFFED, 0);
        //tu.update_log_flags(UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0));
        alus[0]->reset_resources();
        alus[0]->calculate_output(mk_addr(3, addr, 32, true), phv, &data_out, &data_out, &action,
                                  present_time,ingress,match_bus,learn_or_match_bus);
        //tu.update_log_flags(UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0));
        EXPECT_EQ(exp_action, action.get_word(0,32));
        EXPECT_EQ(data_exp.get_word(0), data_out.get_word(0));
        EXPECT_EQ(data_exp.get_word(64), data_out.get_word(64));
        // Add 0
        phv.set_word(UINT64_C(0x0000000000000000), 0);
        data_exp.set_word(0xFFFFFFFFFFFFFFED, 0);
        //tu.update_log_flags(UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0));
        alus[0]->reset_resources();
        alus[0]->calculate_output(mk_addr(3, addr, 32, true), phv, &data_out, &data_out, &action,
                                  present_time,ingress,match_bus,learn_or_match_bus);
        //tu.update_log_flags(UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0));
        EXPECT_EQ(exp_action, action.get_word(0,32));
        EXPECT_EQ(data_exp.get_word(0), data_out.get_word(0));
        EXPECT_EQ(data_exp.get_word(64), data_out.get_word(64));


        // Test carry over from lo to hi.
        phv.set_word(UINT64_C(0x0000000000000123), 0);
        data_in.set_word( 0x00000000FFFFFFF0, 0);
        data_exp.set_word(0x0000000100000113 , 0);
        //tu.update_log_flags(UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0));
        alus[0]->reset_resources();
        alus[0]->calculate_output(mk_addr(3, addr, 32, true), phv, &data_in, &data_out, &action,
                                  present_time,ingress,match_bus,learn_or_match_bus);
        //tu.update_log_flags(UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0));
        EXPECT_EQ(exp_action, action.get_word(0,32));
        EXPECT_EQ(data_exp.get_word(0), data_out.get_word(0));
        EXPECT_EQ(data_exp.get_word(64), data_out.get_word(64));


        // Test carry over from hi to lo. (cntr=1, phv=-10, new_cntr=-9)
        phv.set_word(UINT64_C(0x00000000FFFFFFF6), 0);
        data_in.set_word( 0x0000000000000001, 0);
        data_exp.set_word(0xFFFFFFFFFFFFFFF7 , 0);
        //tu.update_log_flags(UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0));
        alus[0]->reset_resources();
        alus[0]->calculate_output(mk_addr(3, addr, 32, true), phv, &data_in, &data_out, &action,
                                  present_time,ingress,match_bus,learn_or_match_bus);
        //tu.update_log_flags(UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0));
        EXPECT_EQ(exp_action, action.get_word(0,32));
        EXPECT_EQ(data_exp.get_word(0), data_out.get_word(0));
        EXPECT_EQ(data_exp.get_word(64), data_out.get_word(64));


        uint64_t x = 0x0000000000000000;
        data_in.set_word(x, 0);
        int64_t y = x;//(x & 0x7FFFFFFF) | ((x >> 1) & UINT64_C(0xFFFFFFFF10000000));
        for (int i=0; i<64; ++i) {
          y += 0x08000000;
          phv.set_word(UINT64_C(0x0000000008000000), 0);
          //tu.update_log_flags(UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0));
          alus[0]->reset_resources();
          alus[0]->calculate_output(mk_addr(3, addr, 32, true), phv, &data_in, &data_out, &action,
                                    present_time,ingress,match_bus,learn_or_match_bus);
          //tu.update_log_flags(UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0));
          data_in.copy_from(data_out);
        }
        x = data_out.get_word(0);
        int64_t z = x;//(x & 0x7FFFFFFF) | ((x >> 1) & UINT64_C(0xFFFFFFFF10000000));
        EXPECT_EQ(exp_action, action.get_word(0,32));
        EXPECT_EQ(y, z);
        EXPECT_EQ(data_exp.get_word(64), data_out.get_word(64));

        x = 0x8000000000000000;
        data_in.set_word(x, 0);
        y = x;//(x & 0x7FFFFFFF) | ((x >> 1) & UINT64_C(0xFFFFFFFF10000000));
        for (int i=0; i<100000; ++i) {
          y += 0x76543210;
          phv.set_word(UINT64_C(0x0000000076543210), 0);
          //tu.update_log_flags(UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0));
          alus[0]->reset_resources();
          alus[0]->calculate_output(mk_addr(3, addr, 32, true), phv, &data_in, &data_out, &action,
                                    present_time,ingress,match_bus,learn_or_match_bus);
          //tu.update_log_flags(UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0));
          data_in.copy_from(data_out);
        }
        x = data_out.get_word(0);
        z = x;//(x & 0x7FFFFFFF) | ((x >> 1) & UINT64_C(0xFFFFFFFF10000000));
        EXPECT_EQ(exp_action, action.get_word(0,32));
        EXPECT_EQ(y, z);
        EXPECT_EQ(data_exp.get_word(64), data_out.get_word(64));

        x = 0x8000000FF0000000;
        data_in.set_word(x, 0);
        y = x;//(x & 0x7FFFFFFF) | ((x >> 1) & UINT64_C(0xFFFFFFFF10000000));
        for (int i=0; i<100000; ++i) {
          y += 0x76543210;
          phv.set_word(UINT64_C(0x0000000076543210), 0);
          //tu.update_log_flags(UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0));
          alus[0]->reset_resources();
          alus[0]->calculate_output(mk_addr(3, addr, 32, true), phv, &data_in, &data_out, &action,
                                    present_time,ingress,match_bus,learn_or_match_bus);
          //tu.update_log_flags(UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0));
          data_in.copy_from(data_out);
        }
        x = data_out.get_word(0);
        z = x;//(x & 0x7FFFFFFF) | ((x >> 1) & UINT64_C(0xFFFFFFFF10000000));
        EXPECT_EQ(exp_action, action.get_word(0,32));
        EXPECT_EQ(y, z);
        EXPECT_EQ(data_exp.get_word(64), data_out.get_word(64));

        x = 0x00000000000000FF;
        data_in.set_word(x, 0);
        y = x;//(x & 0x7FFFFFFF) | ((x >> 1) & UINT64_C(0xFFFFFFFF10000000));
        for (int i=0; i<100000; ++i) {
          y += -1;
          phv.set_word(UINT64_C(0x00000000FFFFFFFF), 0);
          //tu.update_log_flags(UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0));
          alus[0]->reset_resources();
          alus[0]->calculate_output(mk_addr(3, addr, 32, true), phv, &data_in, &data_out, &action,
                                    present_time,ingress,match_bus,learn_or_match_bus);
          //tu.update_log_flags(UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0));
          data_in.copy_from(data_out);
        }
        x = data_out.get_word(0);
        z = x;//(x & 0x7FFFFFFF) | ((x >> 1) & UINT64_C(0xFFFFFFFF10000000));
        EXPECT_EQ(exp_action, action.get_word(0,32));
        EXPECT_EQ(y, z);
        EXPECT_EQ(data_exp.get_word(64), data_out.get_word(64));

        x = 0x00000000800000FF;
        data_in.set_word(x, 0);
        y = x;//(x & 0x7FFFFFFF) | ((x >> 1) & UINT64_C(0xFFFFFFFF10000000));
        for (int i=0; i<100000; ++i) {
          y += -1;
          phv.set_word(UINT64_C(0x00000000FFFFFFFF), 0);
          //tu.update_log_flags(UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0));
          alus[0]->reset_resources();
          alus[0]->calculate_output(mk_addr(3, addr, 32, true), phv, &data_in, &data_out, &action,
                                    present_time,ingress,match_bus,learn_or_match_bus);
          //tu.update_log_flags(UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0));
          data_in.copy_from(data_out);
        }
        x = data_out.get_word(0);
        z = x;//(x & 0x7FFFFFFF) | ((x >> 1) & UINT64_C(0xFFFFFFFF10000000));
        EXPECT_EQ(exp_action, action.get_word(0,32));
        EXPECT_EQ(y, z);
        EXPECT_EQ(data_exp.get_word(64), data_out.get_word(64));
      }
    }
  }

  TEST(BFN_TEST_NAME(StatefulAluTest), MathUnit) {
    int chip = 0, pipe = 1, stage = 2;
    int logical_rows[4] = {3,7,11,15};
    TestUtil tu(GLOBAL_MODEL.get(), chip, pipe, stage);
    RmtObjectManager *om = tu.get_objmgr();
    ASSERT_TRUE(om != NULL);
    std::array<MauStatefulAlu*, 4> alus = {};
    //tu.set_debug(true);
    //tu.update_log_flags(UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0));
    //tu.update_log_flags(UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0));
    om->update_log_type_levels(UINT64_C(~0), UINT64_C(~0), RMT_LOG_TYPE_P4, UINT64_C(~0), UINT64_C(0));

    uint32_t addr = 0;
    BitVector<MauDefs::kStatefulMeterAluDataBits> phv{};
    phv.set_word(0, 0);
    BitVector<128> data_in = {};
    BitVector<128> data_out = {};
    BitVector<128> data_exp = {};
    BitVector<128> action   = {};
    uint64_t present_time = 0;
    bool ingress = true;
    std::array<bool,4> match_bus{};
    std::array<bool,4> learn_or_match_bus{};

    for (int row=0; row<4; ++row) {
      EXPECT_NE(nullptr, om);
      EXPECT_NE(nullptr, om->mau_get(pipe, stage));
      EXPECT_NE(nullptr, om->mau_get(pipe, stage)->logical_row_lookup(logical_rows[row]));
      EXPECT_NE(nullptr, om->mau_get(pipe, stage)->logical_row_lookup(logical_rows[row])->mau_meter_alu());
      EXPECT_NE(nullptr, om->mau_get(pipe, stage)->logical_row_lookup(logical_rows[row])->mau_meter_alu()->get_salu());
      alus[row] = om->mau_get(pipe, stage)->logical_row_lookup(logical_rows[row])->mau_meter_alu()->get_salu();
    }

    struct salu_cfg base_cfg = {};
    struct salu_cfg cfg = {};

    for (int instr=0; instr<4; ++instr) {
      for (int state_alu=0; state_alu<4; ++state_alu) {
        // Program the State-ALU-2-lo to do a Logical-B operation and all
        // others to do a Logical-SetZ
        base_cfg.salu_op[instr][state_alu] = state_alu ? 0 : 10;

        // Predicate-on State-ALU-2-Lo only.
        base_cfg.salu_pred[instr][state_alu] = state_alu ? 0 : salu_cfg::kAllCmp;
      }
      base_cfg.salu_alu2_lo_bsrc_math[instr] = 1; // Use the math unit
      base_cfg.salu_alu2_lo_math_src[instr]  = 0; // Use PHV-Lo as the math unit input
      // Let instruction 0 be width 1 and instructions 1-3 be widths 8, 16, and 32
      base_cfg.salu_datasize[instr] = instr ? instr+2 : 0;

      // Set the out ALU to return the State-ALU-lo's result.
      base_cfg.salu_output_cmpfn[instr] = salu_cfg::kAllCmp;
      base_cfg.salu_output_asrc[instr] = 5;
    }

    // Run y/sqrt(x) for the codel algorithm where y is 100ms or 1000us and x
    // is 1 through 400.
    // 100ms == 100,000,000ns
    // 1000us == 1,000,000ns
    cfg = base_cfg;
    cfg.salu_mathunit_output_scale    = 0;
    cfg.salu_mathunit_exponent_invert = 1;
    cfg.salu_mathunit_exponent_shift  = 2;
    for (int c=1000000; c<=100000000; c+=(100000000-1000000)) {
      cfg = base_cfg;
      std::vector<int> lut(16);
      int scale = get_math_table_lut(c, true, false, true, lut);
      for (int i=0; i<4; ++i) {
        cfg.salu_mathtable[i] = lut[i*4] | (lut[i*4 + 1] << 8) |
                                (lut[i*4 + 2] << 16) | (lut[i*4 + 3] << 24);
      }
      cfg.salu_mathunit_output_scale = scale;
      // FIXME - No formula for the output_scale, hardcoding for now.
      cfg.salu_mathunit_output_scale = c==1000000?13:19;
      cfg.salu_mathunit_exponent_invert = 1;
      cfg.salu_mathunit_exponent_shift  = 2;
      setup_salu(&cfg, &tu, 1);

      for (int phv_i=1; phv_i<401; ++phv_i) {
        phv.set_word(phv_i,0);
        bool dbg = false;//phv==159 && c==1000000;
        addr = 0;
        data_in.set_word(0, 0);
        data_in.set_word(0, 64);
        data_exp.copy_from(data_in);
        data_exp.set_word(0x000000000000000a , 0);
        if (dbg) tu.update_log_flags(UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0));
        alus[0]->reset_resources();
        alus[0]->calculate_output(mk_addr(3, addr, 32, true), phv, &data_in, &data_out, &action,
                                  present_time,ingress,match_bus,learn_or_match_bus);
        if (dbg) tu.update_log_flags(UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0));
        EXPECT_EQ(action.get_word(0), data_out.get_word(0));
        double ideal = static_cast<double>(c) / sqrt( static_cast<double>(phv.get_word(0)) );
        double error = 100.0 * (static_cast<double>(data_out.get_word(0)) - ideal) / ideal;
        //int idx = input_to_table_idx(phv, true);
        //printf("%d/sqrt(%3lu) == %8lu index %2d ideal is %8.2f error  % 2.2f%%\n", c, phv, data_out.get_word(0), idx, ideal, error);
        EXPECT_EQ(true, error < 6.0);
        EXPECT_EQ(true, error > -6.0);
      }
    }
    tu.update_log_flags(UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0));
  }

// New JBay outputs: divider, modulus and address
#ifdef MODEL_CHIP_JBAY_OR_LATER
TEST(BFN_TEST_NAME(StatefulAluTest), DivModAddr) {
  int chip = 0, pipe = 1, stage = 2;
  int logical_rows[4] = {3,7,11,15};
  TestUtil tu(GLOBAL_MODEL.get(), chip, pipe, stage);
  RmtObjectManager *om = tu.get_objmgr();
  ASSERT_TRUE(om != NULL);
  std::array<MauStatefulAlu*, 4> alus = {};
  //tu.set_debug(true);
  //tu.update_log_flags(UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0));
  //tu.update_log_flags(UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0));
  om->update_log_type_levels(UINT64_C(~0), UINT64_C(~0), RMT_LOG_TYPE_P4, UINT64_C(~0), UINT64_C(0));

  for (int row=0; row<4; ++row) {
    alus[row] = om->mau_get(pipe, stage)->logical_row_lookup(logical_rows[row])->mau_meter_alu()->get_salu();
  }

  struct salu_cfg cfg = {};

  for (int instr=0; instr<4; ++instr) {
    // Program the State-ALUs to do a Set-Hi
    // cfg.salu_op[instr][alu] = 16;

    // Predicate all State-ALUs.
    for (int alu=0;alu<4;++alu) {
      cfg.salu_pred[instr][alu] = 0;
    }

    // Set the out ALU to return the divider's result
    cfg.salu_output_cmpfn[instr] = salu_cfg::kAllCmp;
    cfg.salu_output_asrc[instr] = 8;

    // Program ALU2-Hi (index 2) sources - note uses subset
    //   that is tofino compatible as this is what setup can do
    cfg.salu_bsrc_phv_index[instr][2] = 0; // PHV Lo
    cfg.salu_bsrc_phv[instr][2] = 1; // Use PHV
    cfg.salu_asrc_memory_index[instr][2] = 0; // RAM Lo
    cfg.salu_asrc_memory[instr][2] = 1; // Use RAM

  }
  // Set the register file constants.
  cfg.salu_const_regfile[0] = 0x00000080;
  cfg.salu_const_regfile[1] = 0x03;
  cfg.salu_const_regfile[2] = 0x06;
  cfg.salu_const_regfile[3] = 0xFFFF0000;


  cfg.salu_datasize[1] = 4;
  cfg.salu_op_dual[1] = 0;
  cfg.salu_divide_enable[1] = 1;
  cfg.salu_output_asrc[1] = 8;
  cfg.salu_cmp_const_src[1][0] = 0;
  cfg.salu_cmp_const_src[1][1] = 0;
  cfg.salu_cmp_regfile_const[1][0] = 0;
  cfg.salu_cmp_regfile_const[1][1] = 0;
  cfg.salu_cmp_bsrc_input[1][0] = 0;
  cfg.salu_cmp_bsrc_sign[1][0] = 0;
  cfg.salu_cmp_bsrc_enable[1][0] = 0;
  cfg.salu_cmp_asrc_input[1][0] = 0;
  cfg.salu_cmp_asrc_sign[1][0] = 0;
  cfg.salu_cmp_asrc_enable[1][0] = 0;
  cfg.salu_cmp_opcode[1][0] = 4;
  cfg.salu_cmp_opcode[1][1] = 7;
  setup_salu(&cfg, &tu, 1);

  auto& mau_base = RegisterUtils::ref_mau(pipe,stage);
  // set output alu1 to send the modulus to the action out (third 32 bit word)
  // set output alu2 to send the address to the action out (second 32 bit word)
  for (int i=0;i<4;++i) {
    // set output alu1 to send the modulus to the action out (third 32 bit word)
    int asrc = 9; // modulus
    int out_alu = 1;
    int alu=0; // which meter group
    uint32_t x=0;
    setp_salu_instr_output_alu_salu_output_asrc (&x, asrc);
    setp_salu_instr_output_alu_salu_output_cmpfn(&x, 0xFFFF); // always predicated on
    auto& addr = mau_base.rams.map_alu.meter_group[alu].stateful.salu_instr_output_alu[i][out_alu];
    tu.OutWord(&addr, x);

    // set output alu2 to send the address to the action out (second 32 bit word)
    x = 0;
    asrc = 7; // address
    out_alu = 2;
    setp_salu_instr_output_alu_salu_output_asrc (&x, asrc);
    setp_salu_instr_output_alu_salu_output_cmpfn(&x, 0xFFFF); // always predicated on
    auto& addr2 = mau_base.rams.map_alu.meter_group[alu].stateful.salu_instr_output_alu[i][out_alu];
    tu.OutWord(&addr2, x);

  }

  // will be shifted up 4 as width=16, in mk_addr(), must refer to word 0 for other tests to work
  uint32_t index = 0x3ABC0;
  BitVector<MauDefs::kStatefulMeterAluDataBits> phv{};
  phv.set_word(0xFEDCBA9876540022, 0); // 16 lowest bits are B
  BitVector<128> data_in = {};
  BitVector<128> data_out = {};
  data_in.set_word(0x0123456789AB0044, 0); // 16 lowest bits are A
  data_in.set_word(0x8877665544332211, 64);
  BitVector<128> action   = {};
  action.set32(0,~0);
  uint64_t present_time = 0;
  bool ingress = true;
  std::array<bool,4> match_bus{};
  std::array<bool,4> learn_or_match_bus{};

  //tu.update_log_flags(UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0));
  alus[0]->reset_resources();
  alus[0]->calculate_output(mk_addr(1, index, 16, false), phv, &data_in, &data_out, &action,
                            present_time,ingress,match_bus,learn_or_match_bus);
  //tu.update_log_flags(UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0));
  uint64_t lo_exp = data_in.get_word(0);
  uint64_t hi_exp = data_in.get_word(64);
  uint64_t lo_out = data_out.get_word(0);
  uint64_t hi_out = data_out.get_word(64);
  EXPECT_EQ( static_cast<uint32_t>( 2 ), action.get_word(0,32));  // div
  EXPECT_EQ( static_cast<uint32_t>( 0 ), action.get_word(64,32)); // mod
  // check address comes out - to get 4 bit op shift instr up by 1 and set bottom bit
  EXPECT_EQ( static_cast<uint32_t>( (index<<4) | (((1 /*instr*/<<1)|1) << 23) ) , action.get_word(32,32));
  EXPECT_EQ(lo_exp, lo_out);
  EXPECT_EQ(hi_exp, hi_out);

  int stage_id = 0x1A;
  index = 0; // make it easier to see
  {
    // turn on stage id insertion into address output
    int alu=0;
    auto& c_addr = mau_base.rams.map_alu.meter_group[alu].stateful.stateful_ctl;
    uint32_t v = tu.InWord(&c_addr);
    setp_stateful_ctl_salu_stage_id(&v, stage_id );
    setp_stateful_ctl_salu_stage_id_enable(&v, 1 );
    tu.OutWord(&c_addr, v);
  }

  // Try other values with mod=3
  phv.set_word(0xFEDCBA9876540022, 0); // 16 lowest bits are B
  data_in.set_word(0x0123456789AB008B, 0); // 16 lowest bits are A
  data_in.set_word(0x8877665544332211, 64);
  action.set32(0,~0);
  //tu.update_log_flags(UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0));
  alus[0]->reset_resources();
  alus[0]->calculate_output(mk_addr(1, index, 16, false), phv, &data_in, &data_out, &action,
                            present_time,ingress,match_bus,learn_or_match_bus);
  //tu.update_log_flags(UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0));
  EXPECT_EQ( static_cast<uint32_t>( 4 ), action.get_word(0,32));  // div
  EXPECT_EQ( static_cast<uint32_t>( 3 ), action.get_word(64,32)); // mod

  // check address comes out with stage_id inserted
  //  note: to get to 4b op (or type), shift instr up by 1 and set bottom bit
  EXPECT_PRED_FORMAT2(CmpHelperIntHex , static_cast<uint32_t>( (index<<4) | (stage_id<<23) ), action.get_word(32,32) );


  // Try other values: divisor negative   0x8B / -0x22
  phv.set_word(0xFEDCBA987654FFDE, 0); // 16 lowest bits are B = -0x22
  data_in.set_word(0x0123456789AB008B, 0); // 16 lowest bits are A
  data_in.set_word(0x8877665544332211, 64);
  action.set32(0,~0);
  //tu.update_log_flags(UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0));
  alus[0]->reset_resources();
  alus[0]->calculate_output(mk_addr(1, index, 16, false), phv, &data_in, &data_out, &action,
                            present_time,ingress,match_bus,learn_or_match_bus);
  //tu.update_log_flags(UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0));
  EXPECT_EQ( static_cast<uint32_t>( 0xFFFC ), action.get_word(0,32));  // div  -4
  EXPECT_EQ( static_cast<uint32_t>( 0xFFE1 ), action.get_word(64,32)); // mod -31

  // Try other values: divisor negative   0x6 / -0x10
  phv.set_word(0xFEDCBA987654FFF0, 0); // 16 lowest bits are B = -16
  data_in.set_word(0x0123456789AB0006, 0); // 16 lowest bits are A = 6
  data_in.set_word(0x8877665544332211, 64);
  action.set32(0,~0);
  //tu.update_log_flags(UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0));
  alus[0]->reset_resources();
  alus[0]->calculate_output(mk_addr(1, index, 16, false), phv, &data_in, &data_out, &action,
                            present_time,ingress,match_bus,learn_or_match_bus);
  //tu.update_log_flags(UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0));
  EXPECT_EQ( static_cast<uint32_t>( 0x0000 ), action.get_word(0,32));  // div  0
  EXPECT_EQ( static_cast<uint32_t>( 0xFFF6 ), action.get_word(64,32)); // mod -10

  // Try other values: divisor negative, mod should be 0   0x20 / -0x10
  phv.set_word(0xFEDCBA987654FFF0, 0); // 16 lowest bits are B = -16
  data_in.set_word(0x0123456789AB0020, 0); // 16 lowest bits are A = 32
  data_in.set_word(0x8877665544332211, 64);
  action.set32(0,~0);
  //tu.update_log_flags(UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0));
  alus[0]->reset_resources();
  alus[0]->calculate_output(mk_addr(1, index, 16, false), phv, &data_in, &data_out, &action,
                            present_time,ingress,match_bus,learn_or_match_bus);
  //tu.update_log_flags(UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0));
  EXPECT_EQ( static_cast<uint32_t>( 0xFFFE ), action.get_word(0,32));  // div -2
  EXPECT_EQ( static_cast<uint32_t>( 0x0000 ), action.get_word(64,32)); // mod 0

  // Try other values: divisor negative   -0x6 / -0x10
  phv.set_word(0xFEDCBA987654FFF0, 0); // 16 lowest bits are B = -16
  data_in.set_word(0x0123456789ABFFFA, 0); // 16 lowest bits are A = -6
  data_in.set_word(0x8877665544332211, 64);
  action.set32(0,~0);
  //tu.update_log_flags(UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0));
  alus[0]->reset_resources();
  alus[0]->calculate_output(mk_addr(1, index, 16, false), phv, &data_in, &data_out, &action,
                            present_time,ingress,match_bus,learn_or_match_bus);
  //tu.update_log_flags(UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0));
  EXPECT_EQ( static_cast<uint32_t>( 0x0000 ), action.get_word(0,32));  // div  0
  EXPECT_EQ( static_cast<uint32_t>( 0xFFFA ), action.get_word(64,32)); // mod -6

  // Try other values: divisor negative   -0x6 / 0x10
  phv.set_word(0xFEDCBA9876540010, 0); // 16 lowest bits are B = 16
  data_in.set_word(0x0123456789ABFFFA, 0); // 16 lowest bits are A = -6
  data_in.set_word(0x8877665544332211, 64);
  action.set32(0,~0);
  //tu.update_log_flags(UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0));
  alus[0]->reset_resources();
  alus[0]->calculate_output(mk_addr(1, index, 16, false), phv, &data_in, &data_out, &action,
                            present_time,ingress,match_bus,learn_or_match_bus);
  //tu.update_log_flags(UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0));
  EXPECT_EQ( static_cast<uint32_t>( 0x0000 ), action.get_word(0,32));  // div  0
  EXPECT_EQ( static_cast<uint32_t>( 0x000A ), action.get_word(64,32)); // mod 10

  // Try other values: dividend negative
  phv.set_word(0xFEDCBA9876540022, 0); // 16 lowest bits are B = 0x22
  data_in.set_word(0x0123456789ABFF75, 0); // 16 lowest bits are A = - 0x8b
  data_in.set_word(0x8877665544332211, 64);
  action.set32(0,~0);
  //tu.update_log_flags(UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0));
  alus[0]->reset_resources();
  alus[0]->calculate_output(mk_addr(1, index, 16, false), phv, &data_in, &data_out, &action,
                            present_time,ingress,match_bus,learn_or_match_bus);
  //tu.update_log_flags(UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0));
  EXPECT_EQ( static_cast<uint32_t>( 0xFFFC ), action.get_word(0,32));  // div  -4
  EXPECT_EQ( static_cast<uint32_t>( 31 ), action.get_word(64,32));     // mod =31

  // Try other values both negative
  phv.set_word(0xFEDCBA987654FFDE, 0); // 16 lowest bits are B = -0x22
  data_in.set_word(0x0123456789ABFF75, 0); // 16 lowest bits are A = - 0x8b
  data_in.set_word(0x8877665544332211, 64);
  action.set32(0,~0);
  //tu.update_log_flags(UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0));
  alus[0]->reset_resources();
  alus[0]->calculate_output(mk_addr(1, index, 16, false), phv, &data_in, &data_out, &action,
                            present_time,ingress,match_bus,learn_or_match_bus);
  //tu.update_log_flags(UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0));
  EXPECT_EQ( static_cast<uint32_t>( 4 ), action.get_word(0,32));  // div  4
  EXPECT_EQ( static_cast<uint32_t>( 0xFFFD ), action.get_word(64,32)); // mod - 3


  // Divide by zero tests: should give -max for negative numbers, +max otherwise
  //  And mod should return A always
  phv.set_word(0xFEDCBA9876540000, 0); // 16 lowest bits are B = 0
  data_in.set_word(0x0123456789ABFF75, 0); // 16 lowest bits are A = - 0x8b
  data_in.set_word(0x8877665544332211, 64);
  action.set32(0,~0);
  //tu.update_log_flags(UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0));
  alus[0]->reset_resources();
  alus[0]->calculate_output(mk_addr(1, index, 16, false), phv, &data_in, &data_out, &action,
                            present_time,ingress,match_bus,learn_or_match_bus);
  //tu.update_log_flags(UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0));
  EXPECT_EQ( static_cast<uint32_t>( 0x8000 ), action.get_word(0,32));  // div  -max
  EXPECT_EQ( static_cast<uint32_t>( 0xFF75 ), action.get_word(64,32)); // mod = A

  // Divide by zero tests: should give -max for negative numbers, +max otherwise
  //  And mod should return A always
  phv.set_word(0xFEDCBA9876540000, 0); // 16 lowest bits are B = 0
  data_in.set_word(0x0123456789AB008B, 0); // 16 lowest bits are A = + 0x8b
  data_in.set_word(0x8877665544332211, 64);
  action.set32(0,~0);
  //tu.update_log_flags(UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0));
  alus[0]->reset_resources();
  alus[0]->calculate_output(mk_addr(1, index, 16, false), phv, &data_in, &data_out, &action,
                            present_time,ingress,match_bus,learn_or_match_bus);
  //tu.update_log_flags(UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0));
  EXPECT_EQ( static_cast<uint32_t>( 0x7FFF ), action.get_word(0,32));  // div  +max
  EXPECT_EQ( static_cast<uint32_t>( 0x008B ), action.get_word(64,32)); // mod = A

  // Divide by zero tests: should give -max for negative numbers, +max otherwise
  //  And mod should return A always
  phv.set_word(0xFEDCBA9876540000, 0); // 16 lowest bits are B = 0
  data_in.set_word(0x0123456789AB0000, 0); // 16 lowest bits are A = 0
  data_in.set_word(0x8877665544332211, 64);
  action.set32(0,~0);
  //tu.update_log_flags(UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0));
  alus[0]->reset_resources();
  alus[0]->calculate_output(mk_addr(1, index, 16, false), phv, &data_in, &data_out, &action,
                            present_time,ingress,match_bus,learn_or_match_bus);
  //tu.update_log_flags(UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0));
  EXPECT_EQ( static_cast<uint32_t>( 0x7FFF ), action.get_word(0,32));  // div  +max
  EXPECT_EQ( static_cast<uint32_t>( 0x0000 ), action.get_word(64,32)); // mod = A

}
#endif


#ifdef MODEL_CHIP_JBAY_OR_LATER
  TEST(BFN_TEST_NAME(StatefulAluTest), CmpAlu64) {
    int chip = 0, pipe = 1, stage = 2;
    int logical_rows[4] = {3,7,11,15};
    TestUtil tu(GLOBAL_MODEL.get(), chip, pipe, stage);
    RmtObjectManager *om = tu.get_objmgr();
    ASSERT_TRUE(om != NULL);
    std::array<MauStatefulAlu*, 4> alus = {};
    //tu.set_debug(true);
    //tu.update_log_flags(UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0));
    //tu.update_log_flags(UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0));
    //om->update_log_type_levels(UINT64_C(~0), UINT64_C(~0), RMT_LOG_TYPE_P4, UINT64_C(~0), UINT64_C(0));

    for (int row=0; row<4; ++row) {
      EXPECT_NE(nullptr, om);
      EXPECT_NE(nullptr, om->mau_get(pipe, stage));
      EXPECT_NE(nullptr, om->mau_get(pipe, stage)->logical_row_lookup(logical_rows[row]));
      EXPECT_NE(nullptr, om->mau_get(pipe, stage)->logical_row_lookup(logical_rows[row])->mau_meter_alu());
      EXPECT_NE(nullptr, om->mau_get(pipe, stage)->logical_row_lookup(logical_rows[row])->mau_meter_alu()->get_salu());
      alus[row] = om->mau_get(pipe, stage)->logical_row_lookup(logical_rows[row])->mau_meter_alu()->get_salu();
    }

    struct salu_cfg base_cfg[4] = {};
    struct salu_cfg cfg = {};

    for (int alu=0; alu<4; ++alu) {
      for (int instr=0; instr<4; ++instr) {
        // Program the State-ALUs to do a Set-Hi
        base_cfg[alu].salu_op[instr][alu] = 16;

        // Predicate all State-ALUs.
        base_cfg[alu].salu_pred[instr][alu] = 0;

        // Set the out ALU to return the Compare ALU's result.
        base_cfg[alu].salu_output_cmpfn[instr] = salu_cfg::kAllCmp;
        base_cfg[alu].salu_output_asrc[instr] = 6;
      }
      // Set the register file constants.
      base_cfg[alu].salu_const_regfile[0] = 0x00000080;
      base_cfg[alu].salu_const_regfile[1] = 0x03;
      base_cfg[alu].salu_const_regfile[2] = 0x06;
      base_cfg[alu].salu_const_regfile[3] = 0xFFFF0000;
    }


    uint32_t addr;
    BitVector<MauDefs::kStatefulMeterAluDataBits> phv{};
    BitVector<128> data_in = {};
    BitVector<128> data_out = {};
    BitVector<128> action   = {};
    uint64_t present_time = 0;
    bool ingress = true;
    std::array<bool,4> match_bus{};
    std::array<bool,4> learn_or_match_bus{};
    uint64_t lo_exp;
    uint64_t hi_exp;
    uint64_t lo_out;
    uint64_t hi_out;

    // This is a tofino test to make sure config is ok
    // Single width 32 bit Lo: 0x44332211 + 0x76543210 + 0xFFFF0000 > 0 (true)
    //                     Hi: 0 + 0 + 3 > 0 (true)
    cfg = base_cfg[0];
    cfg.salu_datasize[1] = 5;
    cfg.salu_op_dual[1] = 0;
    cfg.salu_cmp_const_src[1][0] = 3;
    cfg.salu_cmp_const_src[1][1] = 1;
    cfg.salu_cmp_regfile_const[1][0] = 1;
    cfg.salu_cmp_regfile_const[1][1] = 1;
    cfg.salu_cmp_bsrc_input[1][0] = 0; // phv-lo
    cfg.salu_cmp_bsrc_sign[1][0] = 0;
    cfg.salu_cmp_bsrc_enable[1][0] = 1;
    cfg.salu_cmp_asrc_input[1][0] = 0; // ram-lo
    cfg.salu_cmp_asrc_sign[1][0] = 0;
    cfg.salu_cmp_asrc_enable[1][0] = 1;
    cfg.salu_cmp_opcode[1][0] = 4;
    cfg.salu_cmp_opcode[1][1] = 4;
    setup_salu(&cfg, &tu, 1);

    addr = 2;
    phv.set_word(0xFEDCBA9876543210, 0);
    data_in.set_word(0x0123456789ABCDEF, 0);
    data_in.set_word(0x8877665544332211, 64);
    //tu.update_log_flags(UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0));
    alus[0]->reset_resources();
    alus[0]->calculate_output(mk_addr(1, addr, 32, false), phv, &data_in, &data_out, &action,
                              present_time,ingress,match_bus,learn_or_match_bus);
    //tu.update_log_flags(UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0));
    lo_exp = data_in.get_word(0);
    hi_exp = data_in.get_word(64);
    lo_out = data_out.get_word(0);
    hi_out = data_out.get_word(64);
    EXPECT_EQ(static_cast<uint32_t>(8 << 4 | 1), action.get_word(0,32));
    EXPECT_EQ(lo_exp, lo_out);
    EXPECT_EQ(hi_exp, hi_out);



    // Single width 64 bit Lo: 0x44332211 + 0x76543210 + 0xFFFF0000 > 0 (true)
    //                     Hi: 0 + 0 + 3 > 0 (true)
    // should do same this as 32 bit
    cfg = base_cfg[0];
    cfg.salu_datasize[1] = 6;
    cfg.salu_op_dual[1] = 0;
    cfg.salu_cmp_const_src[1][0] = 3;
    cfg.salu_cmp_const_src[1][1] = 1;
    cfg.salu_cmp_regfile_const[1][0] = 1;
    cfg.salu_cmp_regfile_const[1][1] = 1;
    cfg.salu_cmp_bsrc_input[1][0] = 0; // phv-lo
    cfg.salu_cmp_bsrc_sign[1][0] = 0;
    cfg.salu_cmp_bsrc_enable[1][0] = 1;
    cfg.salu_cmp_asrc_input[1][0] = 0; // ram-lo
    cfg.salu_cmp_asrc_sign[1][0] = 0;
    cfg.salu_cmp_asrc_enable[1][0] = 1;
    cfg.salu_cmp_opcode[1][0] = 4;
    cfg.salu_cmp_opcode[1][1] = 4;
    setup_salu(&cfg, &tu, 1);

    addr = 2;
    phv.set_word(0xBBBBBBBB76543210, 0);
    phv.set_word(0xAAAAAAAAFEDCBA98, 64);
    data_in.set_word(0x0123456789ABCDEF, 0);
    data_in.set_word(0x8877665544332211, 64);
    //tu.update_log_flags(UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0));
    alus[0]->reset_resources();
    alus[0]->calculate_output(mk_addr(1, addr, 32, false), phv, &data_in, &data_out, &action,
                              present_time,ingress,match_bus,learn_or_match_bus);
    //tu.update_log_flags(UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0));
    lo_exp = data_in.get_word(0);
    hi_exp = data_in.get_word(64);
    lo_out = data_out.get_word(0);
    hi_out = data_out.get_word(64);
    EXPECT_EQ(static_cast<uint32_t>(8 << 4 | 1), action.get_word(0,32));
    EXPECT_EQ(lo_exp, lo_out);
    EXPECT_EQ(hi_exp, hi_out);

    //quieten
    tu.update_log_flags(UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0));

  }


  TEST(BFN_TEST_NAME(StatefulAluTest), TMatch) {
    int chip = 0, pipe = 1, stage = 2;
    int logical_rows[4] = {3,7,11,15};
    TestUtil tu(GLOBAL_MODEL.get(), chip, pipe, stage);
    RmtObjectManager *om = tu.get_objmgr();
    ASSERT_TRUE(om != NULL);
    std::array<MauStatefulAlu*, 4> alus = {};
    //tu.set_debug(true);
    tu.update_log_flags(UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0));
    //tu.update_log_flags(UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0));
    om->update_log_type_levels(UINT64_C(~0), UINT64_C(~0), RMT_LOG_TYPE_P4, UINT64_C(~0), UINT64_C(0));

    for (int row=0; row<4; ++row) {
      EXPECT_NE(nullptr, om);
      EXPECT_NE(nullptr, om->mau_get(pipe, stage));
      EXPECT_NE(nullptr, om->mau_get(pipe, stage)->logical_row_lookup(logical_rows[row]));
      EXPECT_NE(nullptr, om->mau_get(pipe, stage)->logical_row_lookup(logical_rows[row])->mau_meter_alu());
      EXPECT_NE(nullptr, om->mau_get(pipe, stage)->logical_row_lookup(logical_rows[row])->mau_meter_alu()->get_salu());
      alus[row] = om->mau_get(pipe, stage)->logical_row_lookup(logical_rows[row])->mau_meter_alu()->get_salu();
    }

    struct salu_cfg base_cfg[4] = {};

    for (int alu=0; alu<4; ++alu) {
      for (int instr=0; instr<4; ++instr) {
        // Program the State-ALUs to do a Set-Hi
        base_cfg[alu].salu_op[instr][alu] = 16;

        // Predicate all State-ALUs.
        base_cfg[alu].salu_pred[instr][alu] = 0;

        // Set the out ALU to return the Compare ALU's result.
        base_cfg[alu].salu_output_cmpfn[instr] = salu_cfg::kAllCmp;
        base_cfg[alu].salu_output_asrc[instr] = 6;

        base_cfg[alu].salu_op_dual[instr] = 0;
      }
      // Set the register file constants.
      base_cfg[alu].salu_const_regfile[0] = 0x00000080;
      base_cfg[alu].salu_const_regfile[1] = 0x03;
      base_cfg[alu].salu_const_regfile[2] = 0x06;
      base_cfg[alu].salu_const_regfile[3] = 0xFFFF0000;


    }

    tu.update_log_flags(UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0));


    // these are single width, so match follows ALU0 out
    // All bits are checked, so expect tmatch=0 from both ALUs
    tmatch_test( tu, base_cfg[0], alus[0],
                 UINT64_C(0x0000000000000000), // tmatch_mask alu0
                 UINT64_C(0x0000000000000000), // tmatch_mask alu1
                 { 0,0,0,0 },   // match bus
                 { 0,0,0,0 },   // learn or match bus
                 0,             // expected ALU0 out
                 0,             // expected ALU1 out
                 0,             // expected learn out
                 0              // expected match out
                 );
    // No bits are checked, so expect tmatch=1 from both ALUs
    tmatch_test( tu, base_cfg[0], alus[0],
                 UINT64_C(0xFFFFFFFFFFFFFFFF), // tmatch_mask alu0
                 UINT64_C(0xFFFFFFFFFFFFFFFF), // tmatch_mask alu1
                 { 0,0,0,0 },   // match bus
                 { 0,0,0,0 },   // learn or match bus
                 1,             // expected ALU0 out
                 1,             // expected ALU1 out
                 0,             // expected learn out
                 1              // expected match out
                 );

    // Few bits are checked
    tmatch_test( tu, base_cfg[0], alus[0],
                 UINT64_C(0xFFFFFFFFFFFFFFF0), // tmatch_mask alu0   0 vs F
                 UINT64_C(0xFFFFFFFFFFFFFFF9), // tmatch_mask alu1   8 vs 1
                 { 0,0,0,0 },   // match bus
                 { 0,0,0,0 },   // learn or match bus
                 0,             // expected ALU0 out
                 1,             // expected ALU1 out
                 0,             // expected learn out
                 1              // expected match out
                 );

    /***********************
     * Learn/Match tests
     */

    // Few bits are checked
    tmatch_test( tu, base_cfg[0], alus[0],
                 UINT64_C(0xBFFFFFFFFFFFFFFF), // tmatch_mask alu0
                 UINT64_C(0xFFFFFFFFFFFFFFFE), // tmatch_mask alu1
                 { 0,0,0,0 },   // match bus
                 { 0,0,0,0 },   // learn or match bus
                 1,             // expected ALU0 out
                 0,             // expected ALU1 out
                 0,             // expected learn out
                 1              // expected match out
                 );

    // Invert bottom 7 bit check
    base_cfg[0].salu_tmatch_invert[1][0] = 0x7F; // instr=1, alu=0
    tmatch_test( tu, base_cfg[0], alus[0],
                 UINT64_C(0xFFFFFFFFFFFFFF80), // tmatch_mask alu0
                 UINT64_C(0xFFFFFFFFFFFFFFFE), // tmatch_mask alu1
                 { 0,0,0,0 },   // match bus
                 { 0,0,0,0 },   // learn or match bus
                 1,             // expected ALU0 out
                 0,             // expected ALU1 out
                 0,             // expected learn out
                 1              // expected match out
                 );
    // Remove invert, check it doesn't match now
    base_cfg[0].salu_tmatch_invert[1][0] = 0x00; // instr=1, alu=0
    tmatch_test( tu, base_cfg[0], alus[0],
                 UINT64_C(0xFFFFFFFFFFFFFF80), // tmatch_mask alu0
                 UINT64_C(0xFFFFFFFFFFFFFFFE), // tmatch_mask alu1
                 { 0,0,0,0 },   // match bus
                 { 0,0,0,0 },   // learn or match bus
                 0,             // expected ALU0 out
                 0,             // expected ALU1 out
                 0,             // expected learn out
                 0              // expected match out
                 );

    // try vld ctl - goes to learn, bottom bit is 1, so learn is zero
    base_cfg[0].salu_tmatch_vld_ctl[1][0] = 1;
    tmatch_test( tu, base_cfg[0], alus[0],
                 UINT64_C(0xBFFFFFFFFFFFFFFF), // tmatch_mask alu0
                 UINT64_C(0xFFFFFFFFFFFFFFFE), // tmatch_mask alu1
                 { 0,0,0,0 },   // match bus
                 { 0,0,0,0 },   // learn or match bus
                 1,             // expected ALU0 out
                 0,             // expected ALU1 out
                 0,             // expected learn out
                 1,             // expected match out
                 UINT64_C(0x0000000000000001), // ram lo in
                 UINT64_C(0x0000000000000001)  // ram hi in
                 );
    // bottom bit is 0, so learn is 1
    tmatch_test( tu, base_cfg[0], alus[0],
                 UINT64_C(0xBFFFFFFFFFFFFFFF), // tmatch_mask alu0
                 UINT64_C(0xFFFFFFFFFFFFFFFE), // tmatch_mask alu1
                 { 0,0,0,0 },   // match bus
                 { 0,0,0,0 },   // learn or match bus
                 1,             // expected ALU0 out
                 0,             // expected ALU1 out
                 1,             // expected learn out
                 1,             // expected match out
                 UINT64_C(0x0000000000000000), // ram lo in
                 UINT64_C(0x0000000000000001)  // ram hi in
                 );
    base_cfg[0].salu_tmatch_vld_ctl[1][0] = 0; // alu 0
    base_cfg[0].salu_tmatch_vld_ctl[1][1] = 1; // alu 1
    tmatch_test( tu, base_cfg[0], alus[0],
                 UINT64_C(0xBFFFFFFFFFFFFFFF), // tmatch_mask alu0
                 UINT64_C(0xFFFFFFFFFFFFFFFE), // tmatch_mask alu1
                 { 0,0,0,0 },   // match bus
                 { 0,0,0,0 },   // learn or match bus
                 1,             // expected ALU0 out
                 0,             // expected ALU1 out
                 0,             // expected learn out
                 1,             // expected match out
                 UINT64_C(0x0000000000000000), // ram lo in
                 UINT64_C(0x0000000000000001)  // ram hi in
                 );
    // single width, learn output is 1 anyway as now controlled by salu_sbus_learn_comb
    tmatch_test( tu, base_cfg[0], alus[0],
                 UINT64_C(0xBFFFFFFFFFFFFFFF), // tmatch_mask alu0
                 UINT64_C(0xFFFFFFFFFFFFFFFF), // tmatch_mask alu1
                 { 0,0,0,0 },   // match bus
                 { 0,0,0,0 },   // learn or match bus
                 1,             // expected ALU0 out
                 1,             // expected ALU1 out
                 1,             // expected learn out
                 1,             // expected match out
                 UINT64_C(0x0000000000000000), // ram lo in
                 UINT64_C(0x0000000000000000)  // ram hi in
                 );
    // dual, so learn should be 1 now
    base_cfg[0].salu_op_dual[1] = 1; // instr 1 is dual
    tmatch_test( tu, base_cfg[0], alus[0],
                 UINT64_C(0xBFFFFFFFFFFFFFFF), // tmatch_mask alu0
                 UINT64_C(0xFFFFFFFFFFFFFFFF), // tmatch_mask alu1
                 { 0,0,0,0 },   // match bus
                 { 0,0,0,0 },   // learn or match bus
                 1,             // expected ALU0 out
                 1,             // expected ALU1 out
                 1,             // expected learn out
                 1,             // expected match out
                 UINT64_C(0x0000000000000000), // ram lo in
                 UINT64_C(0x0000000000000000)  // ram hi in
                 );
    base_cfg[0].salu_op_dual[1] = 0;
    base_cfg[0].salu_tmatch_vld_ctl[1][0] = 0; // alu 0
    base_cfg[0].salu_tmatch_vld_ctl[1][1] = 0; // alu 1


    /***********************
     * SBus Tests
     */
    // all bits set, but no config should not change alu0 or 1
    tmatch_test( tu, base_cfg[0], alus[0],
                 UINT64_C(0x0000000000000000), // tmatch_mask alu0
                 UINT64_C(0x0000000000000000), // tmatch_mask alu1
                 { 1,1,1,1 },   // match bus
                 { 1,1,1,1 },   // learn or match bus
                 0,             // expected ALU0 out
                 0,             // expected ALU1 out
                 0,             // expected learn out
                 0              // expected match out
                 );
    base_cfg[0].salu_match_sbus_listen[1]  = 0x7;
    base_cfg[0].salu_lmatch_sbus_listen[1] = 0x7;
    base_cfg[0].salu_cmp_sbus_or[1][0]     = 0;
    base_cfg[0].salu_cmp_sbus_and[1][0]    = 0;
    base_cfg[0].salu_cmp_sbus_invert[1][0] = 0;
    base_cfg[0].salu_cmp_sbus_or[1][1]     = 0;
    base_cfg[0].salu_cmp_sbus_and[1][1]    = 0;
    base_cfg[0].salu_cmp_sbus_invert[1][1] = 0;
    tmatch_test( tu, base_cfg[0], alus[0],
                 UINT64_C(0x0000000000000000), // tmatch_mask alu0
                 UINT64_C(0x0000000000000000), // tmatch_mask alu1
                 { 1,1,1,1 },   // match bus
                 { 1,1,1,1 },   // learn or match bus
                 0,             // expected ALU0 out
                 0,             // expected ALU1 out
                 0,             // expected learn out
                 0              // expected match out
                 );
    base_cfg[0].salu_match_sbus_listen[1]  = 0x7;
    base_cfg[0].salu_lmatch_sbus_listen[1] = 0x7;
    base_cfg[0].salu_cmp_sbus_or[1][0]     = 1;
    base_cfg[0].salu_cmp_sbus_and[1][0]    = 0;
    base_cfg[0].salu_cmp_sbus_invert[1][0] = 0;
    base_cfg[0].salu_cmp_sbus_or[1][1]     = 0;
    base_cfg[0].salu_cmp_sbus_and[1][1]    = 0;
    base_cfg[0].salu_cmp_sbus_invert[1][1] = 0;
    tmatch_test( tu, base_cfg[0], alus[0],
                 UINT64_C(0x0000000000000000), // tmatch_mask alu0
                 UINT64_C(0x0000000000000000), // tmatch_mask alu1
                 { 1,1,1,1 },   // match bus
                 { 1,1,1,1 },   // learn or match bus
                 1,             // expected ALU0 out
                 0,             // expected ALU1 out
                 0,             // expected learn out
                 0              // expected match out
                 );
    base_cfg[0].salu_match_sbus_listen[1]  = 0x7;
    base_cfg[0].salu_lmatch_sbus_listen[1] = 0x7;
    base_cfg[0].salu_cmp_sbus_or[1][0]     = 1;
    base_cfg[0].salu_cmp_sbus_and[1][0]    = 0;
    base_cfg[0].salu_cmp_sbus_invert[1][0] = 0;
    base_cfg[0].salu_cmp_sbus_or[1][1]     = 0;
    base_cfg[0].salu_cmp_sbus_and[1][1]    = 0;
    base_cfg[0].salu_cmp_sbus_invert[1][1] = 0;
    tmatch_test( tu, base_cfg[0], alus[0],
                 UINT64_C(0x0000000000000000), // tmatch_mask alu0
                 UINT64_C(0x0000000000000000), // tmatch_mask alu1
                 { 0,1,1,1 },   // match bus
                 { 0,1,1,1 },   // learn or match bus
                 1,             // expected ALU0 out
                 0,             // expected ALU1 out
                 0,             // expected learn out
                 0              // expected match out
                 );
    base_cfg[0].salu_match_sbus_listen[1]  = 0x0;
    base_cfg[0].salu_lmatch_sbus_listen[1] = 0x7;
    base_cfg[0].salu_cmp_sbus_or[1][0]     = 1;
    base_cfg[0].salu_cmp_sbus_and[1][0]    = 0;
    base_cfg[0].salu_cmp_sbus_invert[1][0] = 0;
    base_cfg[0].salu_cmp_sbus_or[1][1]     = 1;
    base_cfg[0].salu_cmp_sbus_and[1][1]    = 0;
    base_cfg[0].salu_cmp_sbus_invert[1][1] = 0;
    tmatch_test( tu, base_cfg[0], alus[0],
                 UINT64_C(0x0000000000000000), // tmatch_mask alu0
                 UINT64_C(0x0000000000000000), // tmatch_mask alu1
                 { 0,1,1,1 },   // match bus
                 { 0,0,0,0 },   // learn or match bus
                 0,             // expected ALU0 out
                 0,             // expected ALU1 out
                 0,             // expected learn out
                 0              // expected match out
                 );
    base_cfg[0].salu_match_sbus_listen[1]  = 0x1;
    base_cfg[0].salu_lmatch_sbus_listen[1] = 0x7;
    base_cfg[0].salu_cmp_sbus_or[1][0]     = 1;
    base_cfg[0].salu_cmp_sbus_and[1][0]    = 0;
    base_cfg[0].salu_cmp_sbus_invert[1][0] = 0;
    base_cfg[0].salu_cmp_sbus_or[1][1]     = 1;
    base_cfg[0].salu_cmp_sbus_and[1][1]    = 0;
    base_cfg[0].salu_cmp_sbus_invert[1][1] = 0;
    tmatch_test( tu, base_cfg[0], alus[0],
                 UINT64_C(0x0000000000000000), // tmatch_mask alu0
                 UINT64_C(0x0000000000000000), // tmatch_mask alu1
                 { 0,0,1,1 },   // match bus
                 { 0,0,0,0 },   // learn or match bus
                 0,             // expected ALU0 out
                 0,             // expected ALU1 out
                 0,             // expected learn out
                 0              // expected match out
                 );
    base_cfg[0].salu_match_sbus_listen[1]  = 0x3;
    base_cfg[0].salu_lmatch_sbus_listen[1] = 0x7;
    base_cfg[0].salu_cmp_sbus_or[1][0]     = 1;
    base_cfg[0].salu_cmp_sbus_and[1][0]    = 0;
    base_cfg[0].salu_cmp_sbus_invert[1][0] = 0;
    base_cfg[0].salu_cmp_sbus_or[1][1]     = 1;
    base_cfg[0].salu_cmp_sbus_and[1][1]    = 0;
    base_cfg[0].salu_cmp_sbus_invert[1][1] = 0;
    tmatch_test( tu, base_cfg[0], alus[0],
                 UINT64_C(0x0000000000000000), // tmatch_mask alu0
                 UINT64_C(0x0000000000000000), // tmatch_mask alu1
                 { 0,0,1,1 },   // match bus
                 { 0,0,0,0 },   // learn or match bus
                 1,             // expected ALU0 out
                 1,             // expected ALU1 out
                 0,             // expected learn out
                 0              // expected match out
                 );
    base_cfg[0].salu_match_sbus_listen[1]  = 0x3;
    base_cfg[0].salu_lmatch_sbus_listen[1] = 0x1;
    base_cfg[0].salu_cmp_sbus_or[1][0]     = 0;
    base_cfg[0].salu_cmp_sbus_and[1][0]    = 1;
    base_cfg[0].salu_cmp_sbus_invert[1][0] = 0;
    base_cfg[0].salu_cmp_sbus_or[1][1]     = 1;
    base_cfg[0].salu_cmp_sbus_and[1][1]    = 0;
    base_cfg[0].salu_cmp_sbus_invert[1][1] = 0;
    tmatch_test( tu, base_cfg[0], alus[0],
                 UINT64_C(0x0000000000000000), // tmatch_mask alu0
                 UINT64_C(0x0000000000000000), // tmatch_mask alu1
                 { 0,0,0,0 },   // match bus
                 { 0,1,0,0 },   // learn or match bus
                 0,             // expected ALU0 out
                 1,             // expected ALU1 out
                 0,             // expected learn out
                 0              // expected match out
                 );
    base_cfg[0].salu_match_sbus_listen[1]  = 0x3;
    base_cfg[0].salu_lmatch_sbus_listen[1] = 0x1;
    base_cfg[0].salu_cmp_sbus_or[1][0]     = 0;
    base_cfg[0].salu_cmp_sbus_and[1][0]    = 1;
    base_cfg[0].salu_cmp_sbus_invert[1][0] = 0;
    base_cfg[0].salu_cmp_sbus_or[1][1]     = 1;
    base_cfg[0].salu_cmp_sbus_and[1][1]    = 0;
    base_cfg[0].salu_cmp_sbus_invert[1][1] = 0;
    tmatch_test( tu, base_cfg[0], alus[0],
                 UINT64_C(0xFFFFFFFFFFFFFFFF), // tmatch_mask alu0
                 UINT64_C(0x0000000000000000), // tmatch_mask alu1
                 { 0,0,0,0 },   // match bus
                 { 0,1,0,0 },   // learn or match bus
                 1,             // expected ALU0 out
                 1,             // expected ALU1 out
                 0,             // expected learn out
                 1              // expected match out
                 );
    base_cfg[0].salu_match_sbus_listen[1]  = 0x1;
    base_cfg[0].salu_lmatch_sbus_listen[1] = 0x7;
    base_cfg[0].salu_cmp_sbus_or[1][0]     = 1;
    base_cfg[0].salu_cmp_sbus_and[1][0]    = 0;
    base_cfg[0].salu_cmp_sbus_invert[1][0] = 1;
    base_cfg[0].salu_cmp_sbus_or[1][1]     = 1;
    base_cfg[0].salu_cmp_sbus_and[1][1]    = 0;
    base_cfg[0].salu_cmp_sbus_invert[1][1] = 0;
    tmatch_test( tu, base_cfg[0], alus[0],
                 UINT64_C(0x0000000000000000), // tmatch_mask alu0
                 UINT64_C(0x0000000000000000), // tmatch_mask alu1
                 { 0,0,1,1 },   // match bus
                 { 0,0,0,0 },   // learn or match bus
                 1,             // expected ALU0 out
                 0,             // expected ALU1 out
                 0,             // expected learn out
                 0              // expected match out
                 );

    base_cfg[0].salu_match_sbus_listen[1]  = 0x0;
    base_cfg[0].salu_lmatch_sbus_listen[1] = 0x0;
    base_cfg[0].salu_cmp_sbus_or[1][0]     = 0;
    base_cfg[0].salu_cmp_sbus_and[1][0]    = 0;
    base_cfg[0].salu_cmp_sbus_invert[1][0] = 0;
    base_cfg[0].salu_cmp_sbus_or[1][1]     = 0;
    base_cfg[0].salu_cmp_sbus_and[1][1]    = 0;
    base_cfg[0].salu_cmp_sbus_invert[1][1] = 0;


    tu.update_log_flags(UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0));

  }


  TEST(BFN_TEST_NAME(StatefulAluTest), MinMax) {
    int chip = 0, pipe = 1, stage = 2;
    int logical_rows[4] = {3,7,11,15};
    TestUtil tu(GLOBAL_MODEL.get(), chip, pipe, stage);
    RmtObjectManager *om = tu.get_objmgr();
    ASSERT_TRUE(om != NULL);
    std::array<MauStatefulAlu*, 4> alus = {};
    //tu.set_debug(true);
    tu.update_log_flags(UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0));
    //tu.update_log_flags(UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0));
    om->update_log_type_levels(UINT64_C(~0), UINT64_C(~0), RMT_LOG_TYPE_P4, UINT64_C(~0), UINT64_C(0));

    for (int row=0; row<4; ++row) {
      EXPECT_NE(nullptr, om);
      EXPECT_NE(nullptr, om->mau_get(pipe, stage));
      EXPECT_NE(nullptr, om->mau_get(pipe, stage)->logical_row_lookup(logical_rows[row]));
      EXPECT_NE(nullptr, om->mau_get(pipe, stage)->logical_row_lookup(logical_rows[row])->mau_meter_alu());
      EXPECT_NE(nullptr, om->mau_get(pipe, stage)->logical_row_lookup(logical_rows[row])->mau_meter_alu()->get_salu());
      alus[row] = om->mau_get(pipe, stage)->logical_row_lookup(logical_rows[row])->mau_meter_alu()->get_salu();
    }

    struct salu_cfg base_cfg = {};

    for (int instr=0; instr<4; ++instr) {
      base_cfg.salu_datasize[instr] = 7;
      base_cfg.salu_op_dual[instr] = 0;

      for (int out_alu=0;out_alu<4;out_alu++) {
        // Set the out ALU to return either state out hi or state out lo
        base_cfg.salu_output_cmpfn_jbay[instr][out_alu] = salu_cfg::kAllCmp;
        base_cfg.salu_output_asrc_jbay[instr][out_alu] = out_alu%2 ? 5 /*Hi*/ : 4 /*Lo*/;
      }

      for (int state_alu=0;state_alu<4;state_alu++) {
        // Program the State-ALUs to do a Noop
        base_cfg.salu_op[instr][state_alu] = 13;
        // Predicate all State-ALUs.
        base_cfg.salu_pred[instr][state_alu] = 0xFFFF;

        base_cfg.salu_arith[instr][state_alu] = 1;
      }
    }

    // regfile addr comes from alu3's instr, do a reverse mapping
    base_cfg.salu_cmp_const_src[0][3] = 3; // instr 0
    base_cfg.salu_cmp_const_src[1][3] = 2; // instr 1
    base_cfg.salu_cmp_const_src[2][3] = 1; // instr 2
    base_cfg.salu_cmp_const_src[3][3] = 0; // instr 3

    base_cfg.salu_const_regfile[0] = 0x000000FF;
    base_cfg.salu_const_regfile[1] = 0x000000AA;
    base_cfg.salu_const_regfile[2] = 0x0000FFFF;
    base_cfg.salu_const_regfile[3] = 0x0000AAAA;

    //tu.update_log_flags(UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0), UINT64_C(~0));

    MinmaxTestParams r;

    // simple 8 bit max
    r = {};
    r.width=8;
    r.is_max=true;
    r.data_in_word_0 = UINT64_C(0x0011223344558877);
    r.data_in_word_1 = UINT64_C(0x0011223344556677);
    r.expected_index = 1;
    r.expected_value = 0x88;
    minmax_test( tu, base_cfg, alus[0], r );

    // 8 bit max test, check finds highest index with multiple matches
    r = {};
    r.width=8;
    r.is_max=true;
    r.data_in_word_0 = UINT64_C(0x0011223344558877);
    r.data_in_word_1 = UINT64_C(0x0011223344558877);
    r.expected_index = 9;
    r.expected_value = 0x88;
    minmax_test( tu, base_cfg, alus[0], r );

    // 8 bit test, check finds in higher word
    r = {};
    r.width=8;
    r.is_max=true;
    r.data_in_word_0 = UINT64_C(0x0011223344557777);
    r.data_in_word_1 = UINT64_C(0x0011223344558877);
    r.expected_index = 9;
    r.expected_value = 0x88;
    minmax_test( tu, base_cfg, alus[0], r );

    // 8 bit test, phv mask masks out highest value
    r = {};
    r.width=8;
    r.is_max=true;
    r.data_in_word_0 = UINT64_C(0x0011223344557711);
    r.data_in_word_1 = UINT64_C(0x0011223344558877);
    r.phv_mask = 0xFDFF;
    r.expected_index = 8;
    r.expected_value = 0x77;
    minmax_test( tu, base_cfg, alus[0], r );

    // 8 bit test, regfile mask
    r = {};
    r.width=8;
    r.is_max=true;
    r.data_in_word_0 = UINT64_C(0x1199229944997799);
    r.data_in_word_1 = UINT64_C(0x00AA22AA44AA8899);
    r.mask_from_regfile = true;
    r.instr = 0; // selects mask in regfile 3 = 0xAAAA
    r.expected_index = 9;
    r.expected_value = 0x88;
    minmax_test( tu, base_cfg, alus[0], r );


    // simple 8 bit min
    r = {};
    r.width=8;
    r.is_max=false;
    r.data_in_word_0 = UINT64_C(0xFF11223344558877);
    r.data_in_word_1 = UINT64_C(0xFF22223344556677);
    r.expected_index = 6;
    r.expected_value = 0x11;
    minmax_test( tu, base_cfg, alus[0], r );

    // 8 bit test min, check finds lowest index with multiple matches
    r = {};
    r.width=8;
    r.is_max=false;
    r.data_in_word_0 = UINT64_C(0xFF11223344558877);
    r.data_in_word_1 = UINT64_C(0xFF11223344556677);
    r.expected_index = 6;
    r.expected_value = 0x11;
    minmax_test( tu, base_cfg, alus[0], r );

    // 8 bit test min, mask out low value in word 0
    r = {};
    r.width=8;
    r.is_max=false;
    r.data_in_word_0 = UINT64_C(0xFF11223344558877);
    r.data_in_word_1 = UINT64_C(0xFF11223344556677);
    r.phv_mask = 0xFFBF;
    r.expected_index = 14;
    r.expected_value = 0x11;
    minmax_test( tu, base_cfg, alus[0], r );

    // 16 bit max
    r = {};
    r.width=16;
    r.is_max=true;
    r.data_in_word_0 = UINT64_C(0x0011223344558877);
    r.data_in_word_1 = UINT64_C(0x0011223344556677);
    r.expected_index = 0;
    r.expected_value = 0x8877;
    minmax_test( tu, base_cfg, alus[0], r );

    // 16 bit test, check finds lowest index with multiple matches
    r = {};
    r.width=16;
    r.is_max=true;
    r.data_in_word_0 = UINT64_C(0x0011223344558877);
    r.data_in_word_1 = UINT64_C(0x0011223344558877);
    r.expected_index = 4;
    r.expected_value = 0x8877;
    minmax_test( tu, base_cfg, alus[0], r );

    // 16 bit test, check finds in higher word
    r = {};
    r.width=16;
    r.is_max=true;
    r.data_in_word_0 = UINT64_C(0x0011223344557777);
    r.data_in_word_1 = UINT64_C(0x0011223344558877);
    r.expected_index = 4;
    r.expected_value = 0x8877;
    minmax_test( tu, base_cfg, alus[0], r );

    // 16 bit test, phv mask masks out highest value
    r = {};
    r.width=16;
    r.is_max=true;
    r.data_in_word_0 = UINT64_C(0x0011223344557711);
    r.data_in_word_1 = UINT64_C(0x0011223344558877);
    r.phv_mask = 0xEF;
    r.expected_index = 0;
    r.expected_value = 0x7711;
    minmax_test( tu, base_cfg, alus[0], r );

    // 16 bit test, regfile mask
    r = {};
    r.width=16;
    r.is_max=true;
    r.data_in_word_0 = UINT64_C(0x1111445599337711);
    r.data_in_word_1 = UINT64_C(0x0011223344558899);
    r.mask_from_regfile = true;
    r.instr = 2; // selects mask in regfile 1 = 0xAA
    r.expected_index = 1;
    r.expected_value = 0x9933;
    minmax_test( tu, base_cfg, alus[0], r );


    // simple 16 bit min
    r = {};
    r.width=16;
    r.is_max=false;
    r.data_in_word_0 = UINT64_C(0xFF11223344558877);
    r.data_in_word_1 = UINT64_C(0xFF22993344556677);
    r.expected_index = 2;
    r.expected_value = 0x2233;
    minmax_test( tu, base_cfg, alus[0], r );

    // 16 bit test min, check finds lowest index with multiple matches
    r = {};
    r.width=16;
    r.is_max=false;
    r.data_in_word_0 = UINT64_C(0xFF11223344558877);
    r.data_in_word_1 = UINT64_C(0xFF11223344556677);
    r.expected_index = 2;
    r.expected_value = 0x2233;
    minmax_test( tu, base_cfg, alus[0], r );

    // 16 bit test min, mask out low value in word 0
    r = {};
    r.width=16;
    r.is_max=false;
    r.data_in_word_0 = UINT64_C(0xFF11223344558877);
    r.data_in_word_1 = UINT64_C(0xFF11223344556677);
    r.phv_mask = 0xFB;
    r.expected_index = 6;
    r.expected_value = 0x2233;
    minmax_test( tu, base_cfg, alus[0], r );


    // 16 bit max posmod dec
    r = {};
    r.width=16;
    r.is_max=true;
    r.data_in_word_0  = UINT64_C(0x0011223344558877);
    r.data_in_word_1  = UINT64_C(0x0011223344556677);
    r.data_out_word_0 = UINT64_C(0x0011223344558874);
    r.data_out_word_1 = UINT64_C(0x0011223344556677);
    r.instr = 1;
    r.postdec_enable = true;
    r.postmod_value  = 3;
    r.expected_index = 0;
    r.expected_value = 0x8874;
    minmax_test( tu, base_cfg, alus[0], r );

    // 16 bit max postmod inc
    r = {};
    r.width=16;
    r.is_max=true;
    r.data_in_word_0  = UINT64_C(0x0011223344558877);
    r.data_in_word_1  = UINT64_C(0x0011223344556677);
    r.data_out_word_0 = UINT64_C(0x001122334455887A);
    r.data_out_word_1 = UINT64_C(0x0011223344556677);
    r.instr = 1;
    r.postinc_enable = true;
    r.postmod_value  = 3;
    r.expected_index = 0;
    r.expected_value = 0x887A;
    minmax_test( tu, base_cfg, alus[0], r );

    // 16 bit min postmod inc
    r = {};
    r.width=16;
    r.is_max=false;
    r.data_in_word_0  = UINT64_C(0x0011223344558877);
    r.data_in_word_1  = UINT64_C(0x0011223344556677);
    r.data_out_word_0 = UINT64_C(0x0018223344558877);
    r.data_out_word_1 = UINT64_C(0x0011223344556677);
    r.instr = 1;
    r.postinc_enable = true;
    r.postmod_value  = 7;
    r.expected_index = 3;
    r.expected_value = 0x0018;
    minmax_test( tu, base_cfg, alus[0], r );

    //quieten
    tu.update_log_flags(UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0));

  }
#endif

}

