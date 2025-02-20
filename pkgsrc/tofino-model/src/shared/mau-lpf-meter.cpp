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

#include <mau-lpf-meter.h>
#include <mau-chip-lpf-meter.h>
#include <algorithm>

namespace MODEL_CHIP_NAMESPACE {


MauLpfMeter::MauLpfMeter(RmtObjectManager *om, int pipeIndex, int mauIndex, int logicalRowIndex,
              Mau *mau)
    : MauObject(om, pipeIndex, mauIndex, kType, logicalRowIndex, mau),
      T_(UINT64_C(0xFFFFFFFFFFFFFFFF)), addr_(0xFFFFFFFF), V_(0u)
{
}

MauLpfMeter::~MauLpfMeter() { }


void MauLpfMeter::get_input_data(BitVector<kDataBusWidth> *data) {
  data->copy_from(data_in_local_);
}

void MauLpfMeter::calculate_output(uint64_t present_time,
                                   uint64_t relative_time, // just used to work out if forwarding
                                   uint32_t addr,
                                   bool meter_sweep,
                                   bool red_enable,
                                   bool red_only_mode, // JBay only
                                   uint32_t red_nodrop_value,
                                   uint32_t red_drop_value,
                                   uint32_t D, // from meter hash group FIFO
                                   uint64_t random_number,  // from LFSR
                                   int meter_time_scale,
                                   BitVector<kDataBusWidth>* data_in,
                                   bool meter_lpf_sat_ctl, // LPF Inertia Fix (XXX)
                                   BitVector<kDataBusWidth>* data_out,
                                   uint32_t *action_data_out
                                   )
{
  RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauMeter),
          "LpfMeter ---------------- calculate_output() %s%s----------------\n",
          meter_sweep?"SWEEP ":"",
          red_enable?"RED ":"");

  // detect an access to the same address in the cycle after a config write
  //  in this case the data is not forwarded and the alu sees the previous
  //  data. See XXX.
  bool cycle_after_virtual_write_same_address=false;
  if ( (relative_time == (config_write_T_+1)) &&
       (Address::meter_addr_get_vaddr(addr) == Address::meter_addr_get_vaddr(addr_)) )  {

    snapped_ = false;
    cycle_after_virtual_write_same_address=true;
    data_in_local_.copy_from( config_write_read_data_ );
    RMT_LOG(RmtDebug::verbose(),
            "MauLpfMeter: %saddr=0x%x time=%" PRId64 " data_in=0x%016" PRIx64 "_%016" PRIx64 " "
            "(using previous RAM word since vmem write is not forwarded)\n",
            meter_sweep ? "SWEEP " : "",
            addr,present_time,data_in_local_.get_word(64),data_in_local_.get_word(0));
  } else {
    data_in_local_.copy_from( *data_in );
  }


  bool rate_enable;
  uint32_t timestamp;
  uint32_t V; // also called VOld in the document
  int32_t lpf_action_scale;
  int32_t time_constant_mantissa;
  int32_t rise_time_constant_exponent;
  int32_t fall_time_constant_exponent;
  int32_t red_probability_scale;
  int32_t red_level_exponent;
  int32_t red_level_max;
  int32_t red_d_level100;
  int32_t red_level0;

  unpack( &data_in_local_,
          &timestamp,
          &rate_enable,
          &V,
          &lpf_action_scale,
          &time_constant_mantissa,
          &rise_time_constant_exponent,
          &fall_time_constant_exponent,
          &red_probability_scale,
          &red_level_exponent,
          &red_level_max,
          &red_d_level100,
          &red_level0 );

  RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauMeter), " Entry = %s | "
          "timestamp = %d | V = %d | lpf_action_scale = %d | time_constant_mantissa = %d \n"
          "         rise_time_constant_exponent = %d | fall_time_constant_exponent = %d\n"
          "         red_probability_scale = %d | red_level_exponent = %d\n"
          "         red_level_max = %d | red_d_level100 = %d | red_level0 = %d\n",
          rate_enable?"Rate":"Sampled",timestamp,V,lpf_action_scale,
          time_constant_mantissa,rise_time_constant_exponent,
          fall_time_constant_exponent,
          red_probability_scale,red_level_exponent,red_level_max,
          red_d_level100,red_level0);

  uint64_t present_time_scaled = (present_time >> meter_time_scale) & kTimestampMask;

  RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauMeter),
          "Meter present_time = %" PRIu64
          " | present_time_scaled = %" PRIu64 " | D = %d (0x%x)"
          " | random_number = 0x%" PRIx64 " | red_nodrop = %d | red_drop = %d\n",
          present_time,present_time_scaled, D, D, random_number,red_nodrop_value,red_drop_value);

  // note: as all unsigned if present time has wrapped, subtraction will give large positive number
  //    but then masking will give the correct value
  uint64_t timestamp_diff = ( present_time_scaled - timestamp) & kTimestampMask;

  // This all looks a bit wierd because the document is defined in terms of how the hardware
  //   works, so this code generally follows that. However, it avoids transforming the equations
  //   into the final forms in the document (in case there is an error in the transformation)

  int time_constant_exponent;
  if ( rate_enable ) {
    // rate filters don't use separate rise/fall time constants
    //  should have both programmed the same
    RMT_ASSERT( rise_time_constant_exponent == fall_time_constant_exponent );
    time_constant_exponent = rise_time_constant_exponent;
  }
  else {
    // sampled value can have different time constants

    // NOTE maybe use cached value here
    // We use the cached value if calculate_output gets called at time T+1
    // with same addr as at time T.
    // This is to reflect fact that Tofino can't forward the new V value
    // in a single cycle for the time_constant calculation below.

    if ( D > maybe_use_cache_V(relative_time, addr, V) ) {
      time_constant_exponent = rise_time_constant_exponent;
    }
    else {
      time_constant_exponent = fall_time_constant_exponent;
    }
  }

  // LPF Inertia Fix (XXX)
  // detect forwarding case after snapped
  bool cycle_after_snapping_same_address=false;
  if ( snapped_ && (relative_time == (T_+1)) &&
       (Address::meter_addr_get_vaddr(addr) == Address::meter_addr_get_vaddr(addr_)) ) {
    // on cycle after snapping if we see the same address can't forward in time. So force
    // delta time to 0, we won't apply any decay on this cycle, but we won't lose any
    // delta time either because we output the previous timestamp below
    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauMeter),"LPF access to same address next cycle after snapping timestamp, zeroing time diff\n");
    timestamp_diff = 0;
    cycle_after_snapping_same_address=true;
  }
  snapped_ = false;


  // Update cached value V
  cache_V(relative_time, addr, V);

  // LPF equation 7
  int SL = timestamp_diff ? leading_1s_position( timestamp_diff ) - time_constant_exponent  :  0;
  // LPF equation 8
  int delta_time_exponent = timestamp_diff ? leading_1s_position( timestamp_diff ) : 0;

  // effective_manitissa is 1.mantissa (mantissa is 9b, so effective mantissa is 10b which is
  //  considered as a number between 1.0 and 1.998046875)
  uint64_t effective_mantissa = (1 << kTimeConstantMantissaWidth) | time_constant_mantissa;

  // mask off any bits in the effective mantissa which are below the binary point, as these
  //  should not contribute to the delta time (these bits can't always be programmed to 0
  //  as they may be zero in one time constant (rise/fall), but not the other)
  effective_mantissa &=  (UINT64_C(0x200) | ((UINT64_C(0x1ff<<9)) >> std::min(time_constant_exponent,9)));

  // LPF equation 9: sometimes delta time is 0, otherwise:
  //  << delta_time_exponent is the same as x2**delta_time_exponent as in document
  //  >> kTimeConstantMantissaWidth is shift down by 9 as in document to normalise number
  int delta_time = ( delta_time_exponent < 0 ) || (SL <= -8) ?
      0 :
      ( effective_mantissa <<   delta_time_exponent ) >> kTimeConstantMantissaWidth;
  uint32_t new_timestamp = timestamp + delta_time;

  uint64_t present_time_scaled_for_compare = present_time_scaled;
  if ( timestamp > present_time_scaled_for_compare ) {
    // time has wrapped, so timestamp appears to be in future, fix this before compare
    present_time_scaled_for_compare += (1 << kTimestampWidth);
  }

  if (new_timestamp > present_time_scaled_for_compare) {
    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauMeter),
            " New timestamp %d appears in future (present=%" PRIi64 ") delta_time=%d fixing up\n",
	    new_timestamp,present_time_scaled_for_compare,delta_time);
    SL -= 1;
    delta_time_exponent -= 1;
    delta_time = ( delta_time_exponent < 0 ) || (SL <= -8) ?
      0 :
      ( effective_mantissa <<   delta_time_exponent ) >> kTimeConstantMantissaWidth;
    new_timestamp = (timestamp + delta_time) & kTimestampMask;
  }

  int SR1=0;
  int SR2=0;
  // work out SR1 and SR2 using instructions in last column of "Table 6.45: LPF Equations"
  //  calculation is the same for both rate and sampled filters
  if ( -7 <= SL  && SL < 0 ) {
    // Use LPF Equation 6
    SR1 = -SL;
    SR2 = (2*SR1) + 1;
  }
  else if ( SL >= 0 ) {
    // if SL is 0 or is positive, then the Taylor series is a bad approximation and
    //  the SR1/SR2 shift values are substituted from "Table 6.44: LPF Table Approximation for Delta Times>C"
    switch ( SL ) {
      case 0:   SR1 = 2;   SR2 = 3;    break;
      case 1:   SR1 = 3;   SR2 = 7;    break;
      case 2:   SR1 = 6;   SR2 = 9;    break;
      case 3:   SR1 = 12;  SR2 = 14;   break;
      default:  SR1 = 0;   SR2 = 0;    break; // unused
    }
  }

  uint64_t Vnew; // calculate at 64 bits to handle saturation
  uint64_t V_64 = V;
  uint64_t D_64 = D;

  if ( red_only_mode ) { // JBay only
    Vnew = D;
  }
  // "Table 6.45: LPF Equations"
  else if ( timestamp_diff == 0 || SL <= -8 ) {
    if (rate_enable) {      // Rate
      Vnew = V_64 + D_64;
    }
    else {                  // Sampled
      Vnew = V_64;
    }
  }
  else if ( SL >= 4 ) {
    // Last row: Both filter types
    Vnew = D_64;
  }
  else {
    assert(SL >= -7);
    if (rate_enable) {      // Rate
      if ( SL >=0 ) {         //    S >= 0
        Vnew =     (V_64 >> SR1) + (V_64 >> SR2) + D_64;
      }
      else if ( SL >= -7 ) {  //     -7 <= SL < 0
        Vnew = V_64 - (V_64 >> SR1) + (V_64 >> SR2) + D_64;
      }
      // else SL <= -8, asserted not possible above
    }
    else {             // Sampled
      if ( SL >=0 ) {         //    S >= 0
        Vnew =     (V_64 >> SR1) + (V_64 >> SR2) + D_64 - (D_64>>SR1) - (D_64>>SR2);
      }
      else if ( SL >= -7 ) {  //     -7 <= SL < 0
        Vnew = V_64 - (V_64 >> SR1) + (V_64 >> SR2) +     (D_64>>SR1) - (D_64>>SR2);
      }
      // else SL <= -8, asserted not possible above
    }
  }
  // RTL only calculates 33 bits
  Vnew &= UINT64_C( 0x1FFFFFFFF );

  // Saturate Vnew at 32 bits
  uint32_t Vnew_saturated = Vnew > UINT64_C( 0xFFFFFFFF ) ? UINT64_C( 0xFFFFFFFF ) : Vnew;


  // LPF Inertia Fix (XXX)
  if (cycle_after_snapping_same_address) {
    // For single-cycle forwarding if the same entry is accessed the cycle after we
    // detected negative saturation, then force delta time to 0 (done above) and output the same
    // snapped timestamp to the RAM word again so we don't loose any delta time.
    new_timestamp = snapped_timestamp_;
    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauMeter),"LPF -- cycle after snapping same address new_timestamp = %d (0x%08x)\n",new_timestamp,new_timestamp);
  }
  else {
    if (meter_lpf_sat_ctl) {
      // When SL >= 4 there is negative saturation on Vold, ie Vold has decayed to 0 and
      // there's no more decay we can apply, so we can throw away the fractional time and
      // snap the timestamp to current time.
      if ( SL >= 4 ) {
        new_timestamp = present_time_scaled;
        if (!cycle_after_virtual_write_same_address) {
          snapped_timestamp_ = new_timestamp;
          snapped_ = true;
          RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauMeter),"LPF -- snapped new_timestamp = %d (0x%08x)\n",new_timestamp,new_timestamp);
        } else {
          // XXX: Ignore SnapTS next cycle if ConfigWrite previous cycle
          RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauMeter),"LPF -- snapped new_timestamp = %d (0x%08x) "
                  "*** But saw VirtualWrite previous cycle so snapped timestamp will be ignored next cycle ***\n",
                  new_timestamp,new_timestamp);
        }
      }
    }
  }

  if (!cycle_after_virtual_write_same_address) {
    // normal case
    data_out->copy_from( data_in_local_ );
    set_timestamp( new_timestamp, data_out );
    set_vold( Vnew_saturated, data_out );
  }
  else {
    // cycle_after_virtual_write_same_address: the RTL replays the same write data to avoid losing the write
    data_out->copy_from( config_write_write_data_ );
    // and now must update the cache with V set from the config write data
    cache_V(relative_time, addr, get_vold( &config_write_write_data_ ));
  }

  // LPF meters run as normal for meter sweeps, but just action data out to 0
  if (  meter_sweep ) {
    *action_data_out = 0;
  }
  else {
    if ( red_enable ) {
      bool red_disable = MauChipLpfMeter::red_disable( &data_in_local_ );
      calculate_red_action_data(
          Vnew, // RED uses the 33b unsaturated Vnew
          red_nodrop_value,
          red_drop_value,
          red_probability_scale,
          red_level_exponent,
          red_level_max,
          red_d_level100,
          red_level0,
          random_number,
          red_disable,
          action_data_out );
    }
    else {
      *action_data_out = Vnew_saturated >> lpf_action_scale;
    }
  }
  RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauMeter),
          " timestamp_diff = %" PRIi64 " | delta_time_exponent = %d | time_constant_exponent = %d | effective_mantissa = %" PRIu64 "\n",
          timestamp_diff, delta_time_exponent, time_constant_exponent, effective_mantissa);
  RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauMeter),
          " delta_time = %d%s | new_timestamp = %d | SL = %d | SR1 = %d | SR2 = %d \n",
          delta_time, delta_time?" (NON ZERO DELTA)":"",
          new_timestamp, SL, SR1, SR2);


  RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauMeter),
          " action_data_out = 0x%08x Vnew = 0x%" PRIx64 " , Vnew_sat = 0x%x (%u)\n",*action_data_out,Vnew,Vnew_saturated,Vnew_saturated);

}

void MauLpfMeter::calculate_red_action_data(
        uint64_t Vnew,
        uint32_t red_nodrop_value,
        uint32_t red_drop_value,
        uint32_t red_probability_scale,
        uint32_t red_level_exponent,
        uint32_t red_level_max,
        uint32_t red_d_level100,
        uint32_t red_level0,
        uint64_t random_number,
        bool     red_disable,
        uint32_t *action_data_out )
{
  uint64_t Q = Vnew;

  bool drop = false;

  // R = RNG[7:0] ^ RNG[15:22]
  uint32_t R = (0xff & random_number)  ^ reverse_bits( (random_number>>15)&0xff );

  // document says:
  // red_scale_mask[6:0] = ~{7'h7F << red_probability_scale}
  // red_probability_scale_bits[6:0] = red_scale_mask & (LFSR[32:26] ^ LFSR[14:8])
  uint32_t red_scale_mask = (1 << red_probability_scale) - 1;

  // red_scale_mask & (RNG[26:32] ^ RNG[14:8])
  // note: as bits are reversed in byte and we only want 7 bits, have to >> and extra 1 bit
  uint32_t red_probability_scale_bits = red_scale_mask &
      ( ( 0x7f & (reverse_bits((random_number >> 26 ))>>1)) ^
        ( 0x7f & (random_number >> 8 )));
  // LPF equation 10
  uint32_t multiplier_output = (( red_d_level100 * R ) >> 8 ) & 0xff;
  uint32_t adder_output = (multiplier_output + red_level0);
  // LPF equation 11
  uint32_t drop_threshold = std::min( adder_output, red_level_max );

  // LPF equation 12
  if ( ( Q > (drop_threshold << red_level_exponent ) && ( red_probability_scale_bits == 0 )) ||
       ( Q > (red_level_max << red_level_exponent)) ) {
    drop = true;
  }

  RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauMeter),
          "RED R = %x | mask = %x | Q = %" PRIx64 " | drop_thresh = %x | scale_bits = %x\n"
          "         mult = %x | adder = %x%s\n",
          R, red_scale_mask, Q, drop_threshold, red_probability_scale_bits,multiplier_output,adder_output,
          red_disable?" | red_disabled":"");


  if ( drop && ( Vnew < ( red_level0 << red_level_exponent ))) {
    RMT_LOG(RmtDebug::error(RmtDebug::kRmtDebugMauMeter),
            "MauLpfMeter::calculate_red_action_data: dropping when Vnew is less than Level0!\n");
    RMT_ASSERT(0);
  }
  if ( (!drop) && ( Vnew > ((red_level0 + red_d_level100) << red_level_exponent ))) {
    RMT_LOG(RmtDebug::error(RmtDebug::kRmtDebugMauMeter),
            "MauLpfMeter::calculate_red_action_data: not dropping when Vnew is greater than Level100!\n");
    RMT_ASSERT(0);
  }
  if ( (!drop) && ( Vnew > (red_level_max << red_level_exponent ))) {
    RMT_LOG(RmtDebug::error(RmtDebug::kRmtDebugMauMeter),
            "MauLpfMeter::calculate_red_action_data: not dropping when Vnew is greater than red_level_max!\n");
    RMT_ASSERT(0);
  }

  if ( drop && ! red_disable) {
    *action_data_out = red_drop_value;
  }
  else {
    *action_data_out = red_nodrop_value;
  }

}

}
