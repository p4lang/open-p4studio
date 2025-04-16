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

#ifndef _UTESTS_METER_UTIL_
#define _UTESTS_METER_UTIL_

#include <iostream>
#include <string>
#include <array>
#include <cassert>

#include "gtest.h"

#include <utests/test_namespace.h>
#include <model_core/model.h>
#include <ipb.h>
#include <bitvector.h>
#include <rmt-object-manager.h>
#include <mau.h>
#include <port.h>
#include <packet.h>

extern std::unique_ptr<model_core::Model> GLOBAL_MODEL;

namespace MODEL_CHIP_TEST_NAMESPACE {

class TestUtil;

// TODO: leaving the kClockRate at the Tofino value for the moment as
// otherwise the MeterTestJbay.Mantissa test fails
// static uint64_t kClockRate = MODEL_CHIP_NAMESPACE::RmtDefs::kRmtClocksPerSec;
static uint64_t kClockRate = UINT64_C(1250000000);

void rate_to_mantissa_exponent( uint64_t rate_bits_per_s, int burstsize_exponent_adj, uint64_t *rate_mantissa, uint64_t *rate_relative_exponent );
uint64_t mantissa_exponent_to_rate( uint64_t mantissa, uint64_t relative_exponent, int burstsize_exponent_adj);
void burst_size_to_mantissa_exponent( uint64_t burst_size, uint64_t *mantissa, uint64_t *exponent, int *exponent_adj);
uint64_t mantissa_exponent_to_burst_size( uint64_t mantissa, uint64_t exponent);

void run_meter_test( TestUtil& tu, int pipe,int stage,int row, int entry,
                     uint64_t offered_rate, // Mega bits /second
                     double   run_time,   // seconds
                     int packet_size,
                     int   expected_green,
                     int   expected_yellow,
                     int   expected_red,
                     bool logging_on
                     );
void setup_for_meter_test(TestUtil& tu,int pipe,int stage,int row, int entry,bool lpf=false);

void run_meter_pkt_by_pkt( TestUtil& tu, int pipe,int stage,int row, int entry,
                           uint64_t cycle, int packet_len,
                           int   expected_color,
                           bool logging_on);
void init_for_pkt_by_pkt_metertest(TestUtil& tu, int pipe,int stage,int row, int entry);
void get_color_count_for_pkt_by_pkt_metertest(uint64_t *green, uint64_t *yel, uint64_t *red);

struct MeterEntry {
  uint64_t timestamp=0;
  uint64_t peak_level=0;
  uint64_t committed_level=0;
  uint64_t peak_burst_size_mantissa=0;
  uint64_t peak_burst_size_exponent=0;
  uint64_t committed_burst_size_mantissa=0;
  uint64_t committed_burst_size_exponent=0;
  uint64_t peak_rate_mantissa=0;
  uint64_t peak_rate_exponent=0;
  uint64_t committed_rate_mantissa=0;
  uint64_t committed_rate_exponent=0;

  int      cbs_bsize_exponent_adj=0;
  int      pbs_bsize_exponent_adj=0;
  uint64_t expected_green_pkts = 0;
  uint64_t expected_yel_pkts = 0;
  uint64_t expected_red_pkts = 0;
  uint64_t total_offered_pkts = 0;


  MeterEntry() {};
  MeterEntry(uint64_t data0, uint64_t data1) { set_from(data0,data1); }

  uint64_t get_data1() {
    return ((timestamp & 0xfffffff) << (100-64) ) |
        ((peak_level & 0x7fffff ) << (77-64) ) |
        ((committed_level>>10) & 0x1fff);
  }
  uint64_t get_data0() {
    return ((committed_level & 0x3ff) << 54) |
        ((peak_burst_size_exponent & 0x1f) << 49) |
        ((peak_burst_size_mantissa & 0xff) << 41) |
        ((committed_burst_size_exponent & 0x1f) << 36 ) |
        ((committed_burst_size_mantissa & 0xff) << 28) |
        ((peak_rate_exponent & 0x1f) << 23) |
        ((peak_rate_mantissa & 0x1ff) << 14) |
        ((committed_rate_exponent & 0x1f) << 9 ) |
        ((committed_rate_mantissa & 0x1ff) << 0);
  }
  void set_from(uint64_t data0, uint64_t data1) {
    timestamp = (data1>> (100-64)  ) & 0xfffffff;
    peak_level = (data1>>  (77-64)  ) & 0x7fffff;
    committed_level = (data1 & 0x1fff) << 10;
    committed_level |= (data0>> 54  ) & 0x3ff;
    peak_burst_size_exponent = (data0>> 49  ) & 0x1f;
    peak_burst_size_mantissa = (data0>> 41  ) & 0xff;
    committed_burst_size_exponent = (data0>> 36   ) & 0x1f;
    committed_burst_size_mantissa = (data0>> 28  ) & 0xff;
    peak_rate_exponent = (data0>> 23  ) & 0x1f;
    peak_rate_mantissa = (data0>> 14  ) & 0x1ff;
    committed_rate_exponent = (data0>> 9  ) & 0x1f;
    committed_rate_mantissa = (data0>> 0  ) & 0x1ff;
  }

  void set_from_parameters( uint64_t peak_rate_bits_per_second,
                            uint64_t peak_burst_size_ms,
                            uint64_t committed_rate_bits_per_second,
                            uint64_t committed_burst_size_ms ) {
    int burstsize_exponent_adj;
    uint64_t peak_burst_size = (peak_burst_size_ms * peak_rate_bits_per_second) / ( 8 * 1000 );
    uint64_t committed_burst_size = (committed_burst_size_ms * committed_rate_bits_per_second) / (8 * 1000);

    burst_size_to_mantissa_exponent( peak_burst_size,
                                     &peak_burst_size_mantissa, &peak_burst_size_exponent,
                                     &burstsize_exponent_adj);
    rate_to_mantissa_exponent( peak_rate_bits_per_second, burstsize_exponent_adj,
                               &peak_rate_mantissa, &peak_rate_exponent );

    burst_size_to_mantissa_exponent( committed_burst_size,
                                     &committed_burst_size_mantissa, &committed_burst_size_exponent,
                                     &burstsize_exponent_adj);
    rate_to_mantissa_exponent( committed_rate_bits_per_second, burstsize_exponent_adj,
                               &committed_rate_mantissa, &committed_rate_exponent );

    // set the buckets to full
    peak_level      = peak_burst_size_mantissa << (peak_burst_size_exponent - burstsize_exponent_adj);
    committed_level = committed_burst_size_mantissa << (committed_burst_size_exponent - burstsize_exponent_adj);

  }

  void set_from_log_file() {
    // set the buckets to full
    peak_level      = peak_burst_size_mantissa << (peak_burst_size_exponent - pbs_bsize_exponent_adj);
    committed_level = committed_burst_size_mantissa << (committed_burst_size_exponent - cbs_bsize_exponent_adj);

    printf("Starting with Peak-Level %" PRIu64 "   Committed Level %" PRIu64 "\n", peak_level, committed_level);
    printf("Starting with Peak burst mantissa %" PRIu64 "   Committed burst mantissa %" PRIu64 "\n", peak_burst_size_mantissa,
           committed_burst_size_mantissa);

  }
  void set_from_log_file_cbs(uint64_t committed_burst_size) {
    burst_size_to_mantissa_exponent( committed_burst_size,
                                     &committed_burst_size_mantissa, &committed_burst_size_exponent,
                                     &cbs_bsize_exponent_adj);
  }
  void set_from_log_file_cir_persec(float committed_rate_bytes_per_second) {
    rate_to_mantissa_exponent( (uint64_t)(committed_rate_bytes_per_second * 8), cbs_bsize_exponent_adj,
                               &committed_rate_mantissa, &committed_rate_exponent );
  }
  void set_from_log_file_pbs(uint64_t peak_burst_size) {
    burst_size_to_mantissa_exponent( peak_burst_size,
                                     &peak_burst_size_mantissa, &peak_burst_size_exponent,
                                     &pbs_bsize_exponent_adj);
  }
  void set_from_log_file_pir_persec(float peak_rate_bytes_per_second) {
    rate_to_mantissa_exponent((uint64_t)(peak_rate_bytes_per_second * 8), pbs_bsize_exponent_adj,
                               &peak_rate_mantissa, &peak_rate_exponent );
  }

  void set_from_log_file_total_pkts(uint64_t pkts) {
    total_offered_pkts = pkts;
  }
  void set_from_log_file_total_green_pkts(uint64_t pkts) {
    expected_green_pkts = pkts;
  }
  void set_from_log_file_total_yel_pkts(uint64_t pkts) {
    expected_yel_pkts = pkts;
  }
  void set_from_log_file_total_red_pkts(uint64_t pkts) {
    expected_red_pkts = pkts;
  }
};



}

#endif
