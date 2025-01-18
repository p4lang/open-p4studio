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

#include <rmt-log.h>
#include <tcam-row-vh-xbar-with-reg.h>
#include <register_adapters.h>


namespace MODEL_CHIP_NAMESPACE {

uint8_t TcamRowVhWithReg::CalculateTopNibble(int bus,
                                             const BitVector<kTotalMatchBits>&  input,
                                             const BitVector<kTotalMatchBytes>& input_valid,
                                             const BitVector<kVersionDataWidth>& ingress_version_bits,
                                             const BitVector<kVersionDataWidth>& egress_version_bits) {
  uint8_t byte=0;
  const uint8_t enable = half_byte_mux_ctl_[bus].tcam_row_halfbyte_mux_ctl_enable();
  if ( enable ) {
    switch ( half_byte_mux_ctl_[bus].tcam_row_halfbyte_mux_ctl_select() ) {
      case 0:
        // Code 0 selects the low nibble from the first stage
        if ( output_byte_enable_[5] )
          byte = input.get_byte( output_byte_source_[5]  ) & 0xf;
        break;
      case 1:
        // Code 1 selects the hi  nibble from the first stage
        if ( output_byte_enable_[5] )
          byte = input.get_byte( output_byte_source_[5] ) >> 4;
        break;
      case 2:
        // Code 2 selects the valid bit xbar output. Lo nibble for even rows, hi nibble for odd rows.
        for (int b=0;b<4;++b) {
          int sel = bus ? b+4 : b;
          byte |= (valid_bit_enable_[sel] && input_valid.get_bit(valid_bit_sel_[sel])) << b;
        }
        break;
      case 3:
        // Code 3 selects the valid bit xbar output on bits 1:0 and version bits on bits 3:2.
        for (int b=0;b<2;++b) {
          int sel = bus ? b+4 : b;
          byte |= (valid_bit_enable_[sel] && input_valid.get_bit(valid_bit_sel_[sel])) << b;
        }
        if( half_byte_mux_ctl_[bus].tcam_row_search_thread() ) {
          byte |= (egress_version_bits.get_byte( 0 ) & 0x3) << 2;
        }
        else {
          byte |= (ingress_version_bits.get_byte( 0 ) & 0x3) << 2;
        }
        break;
    }
  }
  return byte;
}
}
