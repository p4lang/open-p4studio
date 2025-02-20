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

#include <mau-meter.h>
#include <address.h>
#include <algorithm>    // std::max


namespace MODEL_CHIP_NAMESPACE {


MauMeter::MauMeter(RmtObjectManager *om, int pipeIndex, int mauIndex, int logicalRowIndex,
                   Mau *mau)
    : MauObject(om, pipeIndex, mauIndex, kType, logicalRowIndex, mau)
{ }

MauMeter::~MauMeter() { }


void MauMeter::get_input_data(BitVector<kDataBusWidth> *data) {
  data->copy_from(data_in_local_);
}

void MauMeter::calculate_output(uint64_t present_time,
                                uint64_t relative_time, // just used to work out if forwarding
                                uint32_t addr,
                                bool is_byte_counter,
                                bool rng_enable,
                                uint64_t random_number,
                                int meter_time_scale,
                                bool meter_sweep,
                                BitVector<kDataBusWidth>* data_in,
                                uint8_t color_in,
                                int decrement,
                                bool meter_lpf_sat_ctl,
                                BitVector<kDataBusWidth>* data_out,
                                uint8_t* color_out)
{
  uint32_t timestamp;
  int32_t  peak_level;
  int32_t  committed_level;
  uint32_t peak_burst_mantissa;
  uint32_t peak_burst_exponent;
  uint32_t committed_burst_mantissa;
  uint32_t committed_burst_exponent;
  uint32_t peak_rate_mantissa;
  uint32_t peak_rate_relative_exponent;
  uint32_t committed_rate_mantissa;
  uint32_t committed_rate_relative_exponent;

  // This code is copied from the LPF case
  // detect an access to the same address in the cycle after a config write
  //  in this case the data is not forwarded and the alu sees the previous
  //  data. See XXX.
  bool cycle_after_virtual_write_same_address=false;
  if ( (relative_time == (config_write_T_+1)) &&
       (Address::meter_addr_get_vaddr(addr) == Address::meter_addr_get_vaddr(addr_)) )  {
    cycle_after_virtual_write_same_address=true;
    data_in_local_.copy_from( config_write_read_data_ );
    RMT_LOG(RmtDebug::verbose(),
            "MauMeter: %saddr=0x%x time=%" PRId64 " data_in=0x%016" PRIx64 "_%016" PRIx64 " "
            "(using previous RAM word since vmem write is not forwarded)\n",
            meter_sweep ? "SWEEP " : "",
            addr,present_time,data_in_local_.get_word(64),data_in_local_.get_word(0));
  } else {
    data_in_local_.copy_from( *data_in );
  }

  unpack( &data_in_local_,
          &timestamp,
          &peak_level,
          &committed_level,
          &peak_burst_mantissa,
          &peak_burst_exponent,
          &committed_burst_mantissa,
          &committed_burst_exponent,
          &peak_rate_mantissa,
          &peak_rate_relative_exponent,
          &committed_rate_mantissa,
          &committed_rate_relative_exponent );

  int peak_decrement =   0;
  int committed_decrement =   0;

  if (meter_sweep) {
    // do not decrement when in meter sweep
    peak_decrement =   0;
    committed_decrement =   0;
    // don't write the color out in the case of sweep
    *color_out = kMeterColorInhibit;
  }
  else { // not meter sweep
    switch ( color_in ) {
      case kMeterColorRed:
        peak_decrement      = 0;
        committed_decrement = 0;
        break;
      case kMeterColorYellow:
        // fall through
      case kMeterColorYellow2:
        peak_decrement      = decrement;
        committed_decrement = 0;
        break;
      case kMeterColorGreen:
        peak_decrement      = decrement;
        committed_decrement = decrement;
        break;
    }
  }

  uint64_t present_time_scaled = (present_time >> meter_time_scale) & kTimestampMask;
  // note: as all unsigned if present time has wrapped, subtraction will give large positive number
  //    but then masking will give the correct value
  uint64_t timestamp_diff = ( present_time_scaled - timestamp) & kTimestampMask;

  // calculate rate_exponenets from relative_exponents
  int peak_rate_exponent = calculate_rate_exponent( peak_rate_relative_exponent,
                                                    peak_burst_exponent);
  int committed_rate_exponent = calculate_rate_exponent( committed_rate_relative_exponent,
                                                         committed_burst_exponent);

  // When calcualting deltatime, the timestamp read from RAM is adjusted to pace of slower refill frequency.
  //  (higher rate exponent implies slower refill frequency).
  uint32_t dtime_mask = kTimestampMask & (kTimestampMask << std::max(committed_rate_exponent,peak_rate_exponent));
  // When updating timestamp into RAM, use faster bucket rate (faster refill frequency) to mask delta-time.
  uint32_t dtime_mask_for_timestamp = kTimestampMask & (kTimestampMask << std::min(committed_rate_exponent,peak_rate_exponent));
  uint64_t committed_rate_timestamp_diff = timestamp_diff;
  uint64_t peak_rate_timestamp_diff = timestamp_diff;
  if (committed_rate_exponent > peak_rate_exponent) {
    committed_rate_timestamp_diff = ( present_time_scaled - (timestamp & dtime_mask)) & kTimestampMask;
  } else {
    peak_rate_timestamp_diff = ( present_time_scaled - (timestamp & dtime_mask)) & kTimestampMask;
  }

  // In the new implementation (bucket intervals decoupled) there are no bits to clear
  int peak_bits_to_clear = 0;
  int committed_bits_to_clear = 0;

  // if meter_lpf_sat_ctl is on then use the old implementation (bucket intervals coupled)
  // 1. We determine which bucket has the slower rate (larger rate_exponent) and then use the the
  //    slower bucket's rate_exponent to generate a mask (call this slow_mask) for both buckets.
  //   (this is dtime_mask above)
  // 2. Mask the delta time that each bucket sees using slow_mask and then credit accordingly using the masked delta time.
  //    This has the affect of snapping the faster bucket to the same credit interval as the slower bucket and both buckets
  //    see the same procession of time.
  //   (do this by clearing bits in the timestamp in calculate_new_bucket_level() below)
  // 3. When generating the new timestamp to be written back to the RAM, we use slow_mask to mask off the delta time lsbs
  //     that were lost by both buckets. This ensures that we never lose delta time and instead accumulate enough delta time
  //     to credit the buckets (albeit at the interval of the slower bucket).
  if (meter_lpf_sat_ctl) {
    dtime_mask_for_timestamp      = dtime_mask;
    peak_bits_to_clear      = std::max( committed_rate_exponent-peak_rate_exponent , 0 );
    committed_bits_to_clear = std::max( peak_rate_exponent-committed_rate_exponent , 0 );
    // both buckets see the same timestamp diff
    committed_rate_timestamp_diff = timestamp_diff;
    peak_rate_timestamp_diff      = timestamp_diff;
  }

  RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauMeter),"Meter ---------------- calculate_output() %s%s%s----------------\n",
          meter_sweep?"SWEEP ":"",
          is_byte_counter?"":"PACKET ",
          rng_enable?"RNG_ENABLED ":"");
  RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauMeter),
          "Meter present_time = %" PRIu64 " | present_time_scaled = %" PRIu64 " | timestamp_diff = %" PRIu64

          " | random_number = 0x%" PRIx64 " | color_in = %d | peak_rate_exponent = %d | committed_rate_exponent = %d | committed_rate_timediff = %" PRIu64 " | peak_rate_timediff = %" PRIu64 " dtime-mask = 0x%x\n",
          present_time, present_time_scaled, timestamp_diff,random_number,static_cast<int>(color_in),
          peak_rate_exponent,committed_rate_exponent, committed_rate_timestamp_diff, peak_rate_timestamp_diff, dtime_mask);

  // Meter Inertia Fix (XXX)
  // detect forwarding case after snapped
  bool cycle_after_snapping_same_address=false;
  if ( snapped_ && (relative_time == (T_+1)) &&
       (Address::meter_addr_get_vaddr(addr) == Address::meter_addr_get_vaddr(addr_)) ) {
    // on cycle after snapping if we see the same address can't forward in time. So zero time diffs so nothing gets creditted.
    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauMeter),"Meter access to same address next cycle after snapping timestamp, zeroing time diffs\n");
    peak_rate_timestamp_diff      = 0;
    committed_rate_timestamp_diff = 0;
    cycle_after_snapping_same_address=true;
  }
  snapped_ = false;
  T_       = relative_time;
  addr_    = addr;


  RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauMeter),"Meter -- calculate peak / yellow bucket\n");
  // calculate peak / yellow bucket
  bool peak_saturated;
  int32_t new_peak_level = calculate_new_bucket_level(
      rng_enable,
      random_number,
      peak_rate_timestamp_diff,
      signextend<int32_t,kPeakLevelWidth>(peak_level),
      peak_burst_mantissa,
      peak_burst_exponent,
      peak_rate_mantissa,
      peak_rate_exponent,
      peak_bits_to_clear,
      peak_decrement,
      &peak_saturated);

  RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauMeter),"Meter -- calculate committed / green bucket\n");
  // calculate committed / green bucket
  bool committed_saturated;
  int32_t new_committed_level = calculate_new_bucket_level(
      rng_enable,
      random_number,
      committed_rate_timestamp_diff,
      signextend<int32_t,kCommittedLevelWidth>(committed_level),
      committed_burst_mantissa,
      committed_burst_exponent,
      committed_rate_mantissa,
      committed_rate_exponent,
      committed_bits_to_clear,
      committed_decrement,
      &committed_saturated);

  // calculate the new timestamp
  if ( committed_rate_exponent >= 28 ) {
    // XXX: special handling for 'all red' meters
    if ((committed_rate_mantissa == 0u) && (committed_rate_relative_exponent == 0u) &&
        (committed_burst_mantissa == 0u) && (committed_burst_exponent == 0u) &&
        (get_committed_level(&data_in_local_) == 0x7FFFFFu)) {
      *color_out = 3; // Red
      data_out->copy_from( data_in_local_ ); // No change
      return;
    }
    if (!meter_sweep) *color_out = 0;
    RMT_LOG(RmtDebug::error(kRelaxExponentCheck),
            "Meter -- unhandled case committed_rate_exponent>=28 (%d)\n",
            committed_rate_exponent);
    if (kRelaxExponentCheck) {
      // data_out remains zero in this case
      return;
    }
    THROW_ERROR(-2);
  }

  // When updating timestamp into RAM, use faster bucket rate (faster refill frequency) to mask delta-time.
  uint32_t new_timestamp = kTimestampMask & (timestamp + (( present_time_scaled - timestamp ) & dtime_mask_for_timestamp ));

  // Meter Inertia Fix (XXX)
  if ( cycle_after_snapping_same_address ) {
    // on cycle after snapping if we see the same address can't forward in time. Time diffs were zeroed out above so
    //  nothing was creditted, but also need to not loose any delta time, so set the timestamp same as last cycle
    new_timestamp = snapped_timestamp_;
    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauMeter),"Meter -- cycle after snapping same address new_timestamp = %d (0x%08x)\n",new_timestamp,new_timestamp);
  }
  else {
    // So, anytime both buckets saturate, snap the stored 28b timestamp to the current time instead of using the normal
    // new_timestamp computation. So this just affects the timestamp output in this scenario.
    if (meter_lpf_sat_ctl) {
      if ( peak_saturated && committed_saturated ) {
        new_timestamp = present_time_scaled;
        if (!cycle_after_virtual_write_same_address) {
          snapped_timestamp_ = new_timestamp;
          snapped_ = true;
          RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauMeter),"Meter -- snapped new_timestamp = %d (0x%08x)\n",new_timestamp,new_timestamp);
        } else {
          // XXX: Ignore SnapTS next cycle if ConfigWrite previous cycle
          RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauMeter),"Meter -- snapped new_timestamp = %d (0x%08x) "
                  "*** But saw VirtualWrite previous cycle so snapped timestamp will be ignored next cycle ***\n",
                  new_timestamp,new_timestamp);
        }
      }
    }
  }


  // This is copied from the LPF case
  if (!cycle_after_virtual_write_same_address) {
    // normal case
    data_out->copy_from( data_in_local_ );
    set_timestamp( new_timestamp, data_out );

    set_committed_level( new_committed_level, data_out );
    set_peak_level( new_peak_level, data_out );
  }
  else {
    // cycle_after_virtual_write_same_address: the RTL replays the same write data to avoid losing the write
    data_out->copy_from( config_write_write_data_ );
    // and now must update the cache with V set from the config write data
    //cache_V(relative_time, addr, get_vold( &config_write_write_data_ )); // No cached VOld in this case
  }

  if ( !meter_sweep ) { // meter sweep
    if ( new_peak_level < 0 ) {
      *color_out = kMeterColorRed;
    }
    else if ( new_committed_level < 0 ) {
      *color_out = kMeterColorYellow;
    }
    else {
      *color_out = kMeterColorGreen;
    }
  }

  RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauMeter),"Meter -- dtime_mask = 0x%08x | dtime_mask_for_timestamp = 0x%08x | timestamp     = %d (0x%08x)\n",
          dtime_mask,dtime_mask_for_timestamp,timestamp,timestamp);
  RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauMeter),"Meter -- color_out = %d           | new_timestamp = %d (0x%08x)\n",
          static_cast<int>(*color_out),new_timestamp,new_timestamp);

}

// Just calculate 'current' color using data_in
void MauMeter::get_curr_color(BitVector<kDataBusWidth>* data_in, uint8_t* color_out)
{
  uint32_t timestamp;
  int32_t  peak_level;
  int32_t  committed_level;
  uint32_t peak_burst_mantissa;
  uint32_t peak_burst_exponent;
  uint32_t committed_burst_mantissa;
  uint32_t committed_burst_exponent;
  uint32_t peak_rate_mantissa;
  uint32_t peak_rate_relative_exponent;
  uint32_t committed_rate_mantissa;
  uint32_t committed_rate_relative_exponent;

  unpack( data_in,
          &timestamp,
          &peak_level,
          &committed_level,
          &peak_burst_mantissa,
          &peak_burst_exponent,
          &committed_burst_mantissa,
          &committed_burst_exponent,
          &peak_rate_mantissa,
          &peak_rate_relative_exponent,
          &committed_rate_mantissa,
          &committed_rate_relative_exponent );

  if ( peak_level < 0 ) {
    *color_out = kMeterColorRed;
  }
  else if ( committed_level < 0 ) {
    *color_out = kMeterColorYellow;
  }
  else {
    *color_out = kMeterColorGreen;
  }
}


int MauMeter::calculate_burstsize_exponent_adj(int burstsize_exponent) {
  return (burstsize_exponent > 14) ? (burstsize_exponent - 14) : 0 ;
}
int MauMeter::calculate_rate_exponent(int rate_relative_exponent, int burstsize_exponent) {
  return (31 - rate_relative_exponent + calculate_burstsize_exponent_adj(burstsize_exponent));
}

int32_t MauMeter::calculate_new_bucket_level(
    bool     rng_enable,
    uint64_t random_number,
    uint64_t timestamp_diff,
    int32_t  level,
    uint32_t burstsize_mantissa,
    uint32_t burstsize_exponent,
    uint32_t rate_mantissa,
    uint32_t rate_exponent,
    uint32_t bits_to_clear,
    uint32_t decrement,
    bool    *saturated)
{
  *saturated=false;

  int burstsize_exponent_adj = calculate_burstsize_exponent_adj( burstsize_exponent );

  int bytecount_inc = 0;


  // probabalistic charging
  if ( rng_enable && (burstsize_exponent_adj != 0)) {
    // decrement is 14 bits, so this would all work at 32 bits wide, but use 64 just in case it changes
    int64_t mask = kProbabalistMask & ~( kProbabalistMask << burstsize_exponent_adj);
    uint64_t burstsize_fractional_bits = decrement & mask & kFractionalBitsMask;
    bytecount_inc = burstsize_fractional_bits > (mask & random_number) ? 1 : 0;
  }

  // if decrement==0 I think bytecount_inc should always be 0. I rely on this to not decrement on sweeps
  RMT_ASSERT( ! ( (decrement==0) && (bytecount_inc!=0) ));

  int bucket_level_charge = 0xffff & ( (decrement >> burstsize_exponent_adj ) + bytecount_inc );

  // the hardware reduces these two results to 23 bits because of saturation, but no need to in software
  int64_t timestamp_diff_rshifted = timestamp_diff >> rate_exponent;

  //  One bucket might need some bits of the timestamp cleared here so it sees the same value
  //  as the slow bucket
  if ( bits_to_clear > 0 ) {
    uint64_t mask = (~ UINT64_C(0)) << bits_to_clear;
    timestamp_diff_rshifted &= mask;
  }

  // buckets may store scaled values (burstsize_exponent_adj>0), so burst_size to compare to must also be scaled
  int64_t burst_size = static_cast<uint64_t>(burstsize_mantissa) << (burstsize_exponent - burstsize_exponent_adj);

  // increment might need more than 32 bits, so do everything at 64 bits.
  int64_t increment = timestamp_diff_rshifted * rate_mantissa;

  int64_t new_bucket_level = level + increment;

  // cap the bucket before the decrement
  if ( new_bucket_level >= burst_size ) {
    new_bucket_level = burst_size;
    *saturated=true;
    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauMeter),"Meter new_bucket_level limited to burstsize (%" PRIi64 ")\n",
            new_bucket_level);
  }

  new_bucket_level -= bucket_level_charge;

  RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauMeter),"Meter  level=%d\n",level);
  RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauMeter),"Meter  burstsize_mantissa = %d  |  burstsize_exponent = %d  "
          "| burst_size = %" PRIi64 " bytes\n",
          burstsize_mantissa,burstsize_exponent,burst_size);

  RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauMeter),"Meter  rate_mantissa = %d | rate_exponent = %d | (rate = %e TODO:FIXME bytes/clk)\n",
          rate_mantissa,rate_exponent,(static_cast<double>(rate_mantissa)/(UINT64_C(1)<<rate_exponent)));
  RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauMeter),"Meter  bits_to_clear             = %d\n",bits_to_clear);
  RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauMeter),"Meter  decrement                 = %d\n",decrement);

  RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauMeter),"Meter  bucket_level_charge       = %d\n",bucket_level_charge);
  RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauMeter),"Meter  timestamp_diff_rshifted   = %" PRIi64 "\n",timestamp_diff_rshifted);
  RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauMeter),"Meter  increment                 = %" PRIi64 "%s\n",increment,
          (increment!=0)?"  <-- NON ZERO INCREMENT":"");
  RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauMeter),"Meter >new_bucket_level          = %" PRIi64 "\n",new_bucket_level);

  // make sure the bucket doesn't underflow - it should never go more negative than this because once
  //  it goes negative the bucket stops being decremented after the pipe empties - and the size of
  //  the bucket is choosen to the maximum decrements in the pipe at any one time can't cause an underflow.
  RMT_ASSERT( new_bucket_level >= ( - (1<<(kCommittedLevelWidth-1))));

  //return new_bucket_level & kLevelMask;
  return new_bucket_level; // don't mask it now, do it on write
}




}
