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
#include "meter_util.h"
#include <iostream>
#include <string>
#include <array>
#include <cassert>

#include "gtest.h"

#include <bitvector.h>
#include <rmt-object-manager.h>
#include <model_core/model.h>
#include <mau.h>
#include <packet.h>
#include "input_xbar_util.h"
#include "tcam_row_vh_util.h"
#include <ipb.h>

extern std::unique_ptr<model_core::Model> GLOBAL_MODEL;

namespace MODEL_CHIP_TEST_NAMESPACE {

#define SKIP_LONG_TESTS 1


  bool meters_print = false;
  using namespace std;
  using namespace MODEL_CHIP_NAMESPACE;


void configure_logging(TestUtil& tu, int pipe, int stage, bool logging_on) {
    uint64_t pipes, stages, types, rows_tabs, cols, flags;
    pipes = ALL; stages = FEW; types = NON; rows_tabs = TOP; cols = kClockRate;
    pipes = ALL; stages = ALL; types = ALL; rows_tabs = ALL; cols = ALL; flags = ALL;

    tu.set_debug(false);
    tu.set_free_on_exit(true);
    tu.set_evaluate_all(false,false);

    //flags = FEW;
    // clear all
    tu.update_log_flags(pipes, stages, types, rows_tabs, cols, NON, NON);
    flags = 3; // Error, warn, as P4 summary is the same as warn
    tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);

    pipes = UINT64_C(1) << pipe;
    types = UINT64_C(1) << RmtTypes::kRmtTypeMauMeter;
    types = ALL; // temporary hack
    flags = ALL;
    if (logging_on) {
      tu.update_log_flags(pipes, 0, types, 0, 0, flags, ALL);
    }
    else {
      tu.update_log_flags(pipes, 0, types, 0, 0, NON, ALL);
    }
  }

  void run_test_case( uint64_t offered_rate, // Mega bits /second
                      double   run_time,   // seconds
                      uint64_t peak_rate_bits_per_second,
                      uint64_t peak_burst_size_milliseconds,
                      uint64_t committed_rate_bits_per_second,
                      uint64_t committed_burst_size_milliseconds,
                      int packet_size,
                      int   expected_green,
                      int   expected_yellow,
                      int   expected_red,
                      bool  logging_on
                      )
  {
    // run tests for less time - will get failures as some tests the numbers don't just
    //  scale down nicely
    constexpr bool quick_mode = false;

    int quick_mode_divisor = quick_mode ? 10 : 1;

#ifdef SKIP_LONG_TESTS
    if ( (expected_green +  expected_yellow + expected_red) > 5000) {
      printf("SKIP_LONG_TESTS is set, so skipping this test\n");
      return;
    }
#endif

    int chip = 202;
    int pipe = 0;
    int stage = 0;
    TestUtil tu(GLOBAL_MODEL.get(), chip, pipe, stage);

    // MetersTests sometimes don't setup shift/perentry_pos
    // so disable the checks
    MauLookupResult::kRelaxLookupShiftPfePosCheck = true;
    MauLookupResult::kRelaxLookupShiftOpPosCheck = true;
    MauColorSwitchbox::kRelaxColorBusDiffAluCheck = true;

    configure_logging(tu,pipe,stage,logging_on);

    // set meter row and which entry in meter to use
    int row=1;
    int entry=7;

    // set the MAU up so it runs a meter on every packet (in meter_util.h)
    setup_for_meter_test(tu,pipe,stage,row,entry);

    // configure the meter
    tu.meter_config(0,0,row, // pipe0, stage0
                    false,   // egress
                    false,   // red
                    false,   // lpf
                    true,    // byte
                    0);      // byte_count_adjust

    MeterEntry entry0;
    entry0.set_from_parameters(
        peak_rate_bits_per_second,
        peak_burst_size_milliseconds,
        committed_rate_bits_per_second,
        committed_burst_size_milliseconds );

    tu.sram_write(0 /*pipe*/, 0 /*stage*/, row, 6 /*col*/, entry /*index*/,
                  entry0.get_data0(), entry0.get_data1() );

    // TODO: should call tu.mux_config()  ???

    run_meter_test( tu,pipe,stage,row,entry,
                    offered_rate, // Mega bits /second
                    run_time / quick_mode_divisor,   // seconds
                    packet_size,
                    expected_green / quick_mode_divisor,
                    expected_yellow / quick_mode_divisor,
                    expected_red / quick_mode_divisor,
                    logging_on) ;

    tu.finish_test();
    tu.quieten_log_flags();
}


  TEST(BFN_TEST_NAME(MetersTest),MantissaExponentTest) {
    uint64_t burst_mantissa;
    uint64_t burst_exponent;
    int burstsize_exponent_adj;
    burst_size_to_mantissa_exponent( UINT64_C(8000000), &burst_mantissa, &burst_exponent, &burstsize_exponent_adj);
    EXPECT_EQ(  244u, burst_mantissa );
    EXPECT_EQ(  15u, burst_exponent );
    EXPECT_EQ(  1, burstsize_exponent_adj );

    burst_size_to_mantissa_exponent( UINT64_C(1000), &burst_mantissa, &burst_exponent, &burstsize_exponent_adj);
    EXPECT_EQ(  250u, burst_mantissa );
    EXPECT_EQ(  2u, burst_exponent );
    EXPECT_EQ(  0, burstsize_exponent_adj );
    EXPECT_EQ(1000u, mantissa_exponent_to_burst_size( burst_mantissa, burst_exponent));


    uint64_t mantissa;
    uint64_t relative_exponent;
    burstsize_exponent_adj = 0;
    rate_to_mantissa_exponent( UINT64_C( 1000000 ), burstsize_exponent_adj, &mantissa, &relative_exponent );
    EXPECT_EQ( mantissa , 419u );
    EXPECT_EQ( relative_exponent , 9u );
    // convert back to a rate
    uint64_t rate = mantissa_exponent_to_rate( mantissa, relative_exponent, burstsize_exponent_adj);
    EXPECT_TRUE( (rate > 990000) && (rate < 1010000) ); // it won't be exactly right

  }
  TEST(BFN_TEST_NAME(MetersTest),SimpleDecrement) {
    if (meters_print) RMT_UT_LOG_INFO("test_meters_tcam_lookup_packet()\n");

    int chip = 202;
    int pipe = 0;
    int stage = 0;
    TestUtil tu(GLOBAL_MODEL.get(), chip, pipe, stage);

    // MetersTests sometimes don't setup shift/perentry_pos
    // so disable the checks
    MauLookupResult::kRelaxLookupShiftPfePosCheck = true;
    MauLookupResult::kRelaxLookupShiftOpPosCheck = true;

    configure_logging(tu,pipe,stage,false);  // set last arg true for logging

    // set meter row and which entry in meter to use
    int row=1;
    int entry=7;

    // set the MAU up so it runs a meter on every packet (in meter_util.h)
    setup_for_meter_test(tu,pipe,stage,row,entry);

    // configure the meter
    tu.meter_config(0,0,row, // pipe0, stage0
                    false,   // egress
                    false,   // red
                    false,   // lpf
                    true,    // byte
                    0);      // byte_count_adjust

    MeterEntry entry0;
    entry0.set_from_parameters(
        UINT64_C( 9000000 ), // peak_rate_bits_per_second
        UINT64_C( 350  ),    //  peak_burst_size milli seconds
        UINT64_C( 1000000 ), //  committed_rate_bits_per_second,
        UINT64_C( 300 ) );   //  committed_burst_size milli seconds

    tu.sram_write(0 /*pipe*/, 0 /*stage*/, row, 6 /*col*/, entry /*index*/,
                  entry0.get_data0(), entry0.get_data1() );

    // TODO: should call tu.mux_config()  ???

    // Get a port - one associated with pipe 0
    // This also sets up basic config ingress parser and deparser
    Port *port = tu.port_get(16);
    port->ipb()->set_meta_enabled(true);
    port->ipb()->set_rx_enabled(true);

    // And a catch all entry which should match everything
    // Payload0 has to be 1 now to indicate a boundary
    tu.tcam_write_value_mask(0, 4, 0, 505,   // stage0 row4 col0 index511
                             0x0, 0x0, 1,0); // value mask payload0/1


    // Send a couple of packets without advancing time to committed_level should decrement
    //  by packet size
    int expected_level = entry0.committed_level;
    for (int i=0;i<2; ++i ) {
      // Construct a packet - DA,SA,SIP,DIP,Proto,SPort,DPort
      Packet *p_in = tu.packet_make("08:00:22:AA:BB:CC", "08:00:11:DD:EE:FF",
                                    "10.17.34.51", "10.68.85.102",
                                    TestUtil::kProtoTCP, 0x1188, 0x1199, "_______MIN__PAYLOAD_______");
      // set up rough time info
      p_in->setup_time_info( 0 );

      int pkt_len = p_in->len();
      //printf( "Packet Len = %d\n",pkt_len);

      Packet *p_out = tu.port_process_inbound(port, p_in);

      uint64_t data0,data1;
      tu.sram_read(0 /*pipe*/, 0 /*stage*/, row, 6 /*col*/, 7 /*index*/,
                   &data0, &data1 );
      MeterEntry read_back0( data0, data1 );

      expected_level -= pkt_len;
      EXPECT_LE(0, expected_level);
      EXPECT_EQ( static_cast<uint64_t>(expected_level), read_back0.committed_level );
      // Free packet(s)
      tu.packet_free(p_in, p_out);
    }

    tu.finish_test();
    tu.quieten_log_flags();
  }


  // //  A test case that shows up the hardware approximation - should have no red packets,
  // //     but actually gets loads
  // TEST(BFN_TEST_NAME(MetersTest),SHOW_APPROX_1) {  //
  //   if (meters_print) RMT_UT_LOG_INFO("test_meters_SHOW_APPROX()\n");
  //   // Yellow pkts because offered rate is much higher than CIR and below peak rate. Hence no red packets
  //   run_test_case( UINT64_C( 5000000 ), // offered rate  mega bits / second
  //                  0.1,                 // run_time seconds
  //                  UINT64_C( 6000000 ), // peak_rate_bits_per_second
  //                  UINT64_C( 1 ),       // peak_burst_size milli seconds
  //                  UINT64_C( 1000000 ), // committed_rate_bits_per_second,
  //                  UINT64_C( 30 ),      // committed_burst_size milli seconds
  //                  64,                  // packet size
  //                  0,                   // expected_green,
  //                  0,                   // expected_yellow,
  //                  0 ,                  // expected_red
  //                  true // logging_on
  //                  );
  // }

  // test case 1 - no expected results

  // test case 2 - no expected results

  TEST(BFN_TEST_NAME(MetersTest),TC3) {  // from omnet
    if (meters_print) RMT_UT_LOG_INFO("test_meters_TC3()\n");
    // Yellow pkts because offered rate is much higher than CIR and below peak rate. Hence no red packets
    run_test_case( UINT64_C( 9000000 ), // offered rate  mega bits / second
                   0.1,                 // run_time seconds
                   UINT64_C( 9000000 ), // peak_rate_bits_per_second
                   UINT64_C( 350 ), // peak_burst_size milli seconds
                   UINT64_C( 1000000 ), // committed_rate_bits_per_second,
                   UINT64_C( 300 ), // committed_burst_size milli seconds
                   64,                 // packet size
                   782,                // expected_green,
                   977,                // expected_yellow,
                   0 ,                  // expected_red
                   false // logging_on
                   );
  }

  TEST(BFN_TEST_NAME(MetersTest),TC4) {  // from omnet
    if (meters_print) RMT_UT_LOG_INFO("test_meters_TC4()\n");
    // Yellow pkts because offered rate is much higher than CIR and below peak rate. Hence no red packets
    run_test_case( UINT64_C( 30000000 ), // offered rate  mega bits / second
                   0.1,                 // run_time seconds
                   UINT64_C( 40000000 ), // peak_rate_bits_per_second
                   UINT64_C( 190 ), // peak_burst_size milli seconds
                   UINT64_C( 10000000 ), // committed_rate_bits_per_second,
                   UINT64_C( 100 ), // committed_burst_size milli seconds
                   768,                 // packet size
                   326,                // expected_green,
                   163,                // expected_yellow,
                   0 ,                  // expected_red
                   false // logging_on
                   );
  }

  TEST(BFN_TEST_NAME(MetersTest),TC5) {  // from omnet
    if (meters_print) RMT_UT_LOG_INFO("test_meters_TC5()\n");
    // Offered traffic matches with CIR
    run_test_case( UINT64_C( 50000000 ), // offered rate  mega bits / second
                   0.1,                 // run_time seconds
                   UINT64_C( 60000000 ), // peak_rate_bits_per_second
                   UINT64_C( 150 ), // peak_burst_size milli seconds
                   UINT64_C( 50000000 ), // committed_rate_bits_per_second,
                   UINT64_C( 100 ), // committed_burst_size milli seconds
                   256,                 // packet size
                   2442,                // expected_green,
                   0,                // expected_yellow,
                   0 ,                  // expected_red
                   false // logging_on
                   );
  }

  TEST(BFN_TEST_NAME(MetersTest),TC6) {  // from omnet
    if (meters_print) RMT_UT_LOG_INFO("test_meters_TC6()\n");
    // offered rate matched committed rate
    run_test_case( UINT64_C( 80000000 ), // offered rate  mega bits / second
                   0.1,                 // run_time seconds
                   UINT64_C( 88000000 ), // peak_rate_bits_per_second
                   UINT64_C( 90 ), // peak_burst_size milli seconds
                   UINT64_C( 80000000 ), // committed_rate_bits_per_second,
                   UINT64_C( 50 ), // committed_burst_size milli seconds
                   1560,                 // packet size
                   642,                // expected_green,
                   0,                // expected_yellow,
                   0 ,                  // expected_red
                   false // logging_on
                   );
  }

  TEST(BFN_TEST_NAME(MetersTest),TC7) {  // from omnet
    if (meters_print) RMT_UT_LOG_INFO("test_meters_TC7()\n");
    // Offered rate less than committed rate
    run_test_case( UINT64_C( 199000000 ), // offered rate  mega bits / second
                   0.1,                 // run_time seconds
                   UINT64_C( 230000000 ), // peak_rate_bits_per_second
                   UINT64_C( 77 ), // peak_burst_size milli seconds
                   UINT64_C( 200000000 ), // committed_rate_bits_per_second,
                   UINT64_C( 50 ), // committed_burst_size milli seconds
                   400,                 // packet size
                   6219,                // expected_green,
                   0,                // expected_yellow,
                   0 ,                  // expected_red
                   false // logging_on
                   );
  }

  TEST(BFN_TEST_NAME(MetersTest),TC8) {  // from omnet
    if (meters_print) RMT_UT_LOG_INFO("test_meters_TC8()\n");
    // Offered rate and committed rate match
    run_test_case( UINT64_C( 400000000 ), // offered rate  mega bits / second
                   0.1,                 // run_time seconds
                   UINT64_C( 410000000 ), // peak_rate_bits_per_second
                   UINT64_C( 130 ), // peak_burst_size milli seconds
                   UINT64_C( 400000000 ), // committed_rate_bits_per_second,
                   UINT64_C( 100 ), // committed_burst_size milli seconds
                   74,                 // packet size
                   67568,                // expected_green,
                   0,                // expected_yellow,
                   0 ,                  // expected_red
                   false // logging_on
                   );
  }

  TEST(BFN_TEST_NAME(MetersTest),TC9) {  // from omnet
    if (meters_print) RMT_UT_LOG_INFO("test_meters_TC9()\n");
    // offered rate is below cir.
    run_test_case( UINT64_C( 900000000 ), // offered rate  mega bits / second
                   0.1,                 // run_time seconds
                   UINT64_C( 1500000000 ), // peak_rate_bits_per_second
                   UINT64_C( 12 ), // peak_burst_size milli seconds
                   UINT64_C( 1000000000 ), // committed_rate_bits_per_second,
                   UINT64_C( 11 ), // committed_burst_size milli seconds
                   1560,                 // packet size
                   7212,                // expected_green,
                   0,                // expected_yellow,
                   0 ,                  // expected_red
                   false // logging_on
                   );
  }

  TEST(BFN_TEST_NAME(MetersTest),TC10) {  // from omnet
    if (meters_print) RMT_UT_LOG_INFO("test_meters_TC10()\n");
    // Offered rate is well below CIR to compensate for lower burst size interval. Hence no red packets
    run_test_case( UINT64_C( 500000000 ), // offered rate  mega bits / second
                   0.1,                 // run_time seconds
                   UINT64_C( 3400000000 ), // peak_rate_bits_per_second
                   UINT64_C( 6 ), // peak_burst_size milli seconds
                   UINT64_C( 3300000000 ), // committed_rate_bits_per_second,
                   UINT64_C( 5 ), // committed_burst_size milli seconds
                   800,                 // packet size
                   7813,                // expected_green,
                   0,                // expected_yellow,
                   0 ,                  // expected_red
                   false // logging_on
                   );
  }

  TEST(BFN_TEST_NAME(MetersTest),TC11) {  // from omnet
    if (meters_print) RMT_UT_LOG_INFO("test_meters_TC11()\n");
    //
    run_test_case( UINT64_C( 1000000000 ), // offered rate  mega bits / second
                   0.1,                 // run_time seconds
                   UINT64_C( 7800000000 ), // peak_rate_bits_per_second
                   UINT64_C( 4 ), // peak_burst_size milli seconds
                   UINT64_C( 7000000000 ), // committed_rate_bits_per_second,
                   UINT64_C( 3 ), // committed_burst_size milli seconds
                   768,                 // packet size
                   16277,                // expected_green,
                   0,                // expected_yellow,
                   0 ,                  // expected_red
                   false // logging_on
                   );
  }

  // test case 12 - no expected results

  // test case 13 - no expected results

  TEST(BFN_TEST_NAME(MetersTest),TC14) {  // from omnet
    if (meters_print) RMT_UT_LOG_INFO("test_meters_TC14()\n");
    //
    run_test_case( UINT64_C( 10000000 ), // offered rate  mega bits / second
                   0.1,                 // run_time seconds
                   UINT64_C( 9000000 ), // peak_rate_bits_per_second
                   UINT64_C( 350 ), // peak_burst_size milli seconds
                   UINT64_C( 1000000 ), // committed_rate_bits_per_second,
                   UINT64_C( 300 ), // committed_burst_size milli seconds
                   64,                 // packet size
                   782,                // expected_green,
                   1172,                // expected_yellow,
                   0 ,                  // expected_red
                   false // logging_on
                   );
  }

  TEST(BFN_TEST_NAME(MetersTest),TC15) {  // from omnet
    if (meters_print) RMT_UT_LOG_INFO("test_meters_TC15()\n");
    //
    run_test_case( UINT64_C( 50000000 ), // offered rate  mega bits / second
                   0.1,                 // run_time seconds
                   UINT64_C( 40000000 ), // peak_rate_bits_per_second
                   UINT64_C( 190 ), // peak_burst_size milli seconds
                   UINT64_C( 10000000 ), // committed_rate_bits_per_second,
                   UINT64_C( 100 ), // committed_burst_size milli seconds
                   768,                 // packet size
                   326,                // expected_green,
                   488,                // expected_yellow,
                   0 ,                  // expected_red
                   false // logging_on
                   );
  }

  TEST(BFN_TEST_NAME(MetersTest),TC16) {  // from omnet
    if (meters_print) RMT_UT_LOG_INFO("test_meters_TC16()\n");
    // within peak rate(Traffic was run for only 100millisec. Burst absorption of upto 100ms. Hence all Green
    run_test_case( UINT64_C( 62000000 ), // offered rate  mega bits / second
                   0.1,                 // run_time seconds
                   UINT64_C( 60000000 ), // peak_rate_bits_per_second
                   UINT64_C( 150 ), // peak_burst_size milli seconds
                   UINT64_C( 50000000 ), // committed_rate_bits_per_second,
                   UINT64_C( 100 ), // committed_burst_size milli seconds
                   256,                 // packet size
                   3028,                // expected_green,
                   0,                // expected_yellow,
                   0 ,                  // expected_red
                   false // logging_on
                   );
  }

  TEST(BFN_TEST_NAME(MetersTest),TC17) {  // from omnet
    if (meters_print) RMT_UT_LOG_INFO("test_meters_TC17()\n");
    //
    run_test_case( UINT64_C( 87000000 ), // offered rate  mega bits / second
                   0.1,                 // run_time seconds
                   UINT64_C( 88000000 ), // peak_rate_bits_per_second
                   UINT64_C( 90 ), // peak_burst_size milli seconds
                   UINT64_C( 80000000 ), // committed_rate_bits_per_second,
                   UINT64_C( 50 ), // committed_burst_size milli seconds
                   1560,                 // packet size
                   698,                // expected_green,
                   0,                // expected_yellow,
                   0 ,                  // expected_red
                   false // logging_on
                   );
  }

  TEST(BFN_TEST_NAME(MetersTest),TC18) {  // from omnet
    if (meters_print) RMT_UT_LOG_INFO("test_meters_TC18()\n");
    // "Although traffic rate is 201mbps (higher than CIR)
    run_test_case( UINT64_C( 201000000 ), // offered rate  mega bits / second
                   0.1,                 // run_time seconds
                   UINT64_C( 230000000 ), // peak_rate_bits_per_second
                   UINT64_C( 77 ), // peak_burst_size milli seconds
                   UINT64_C( 200000000 ), // committed_rate_bits_per_second,
                   UINT64_C( 50 ), // committed_burst_size milli seconds
                   400,                 // packet size
                   6282,                // expected_green,
                   0,                // expected_yellow,
                   0 ,                  // expected_red
                   false // logging_on
                   );
  }

  TEST(BFN_TEST_NAME(MetersTest),TC19) {  // from omnet
    if (meters_print) RMT_UT_LOG_INFO("test_meters_TC19()\n");
    // """"
    run_test_case( UINT64_C( 405000000 ), // offered rate  mega bits / second
                   0.1,                 // run_time seconds
                   UINT64_C( 410000000 ), // peak_rate_bits_per_second
                   UINT64_C( 130 ), // peak_burst_size milli seconds
                   UINT64_C( 400000000 ), // committed_rate_bits_per_second,
                   UINT64_C( 100 ), // committed_burst_size milli seconds
                   74,                 // packet size
                   68400,                // expected_green,
                   0,                // expected_yellow,
                   0 ,                  // expected_red
                   false // logging_on
                   );
  }

  TEST(BFN_TEST_NAME(MetersTest),TC20) {  // from omnet
    if (meters_print) RMT_UT_LOG_INFO("test_meters_TC20()\n");
    // offered rate more than cir but less than pir. Hence no red pkts.
    run_test_case( UINT64_C( 1500000000 ), // offered rate  mega bits / second
                   0.1,                 // run_time seconds
                   UINT64_C( 1500000000 ), // peak_rate_bits_per_second
                   UINT64_C( 12 ), // peak_burst_size milli seconds
                   UINT64_C( 1000000000 ), // committed_rate_bits_per_second,
                   UINT64_C( 11 ), // committed_burst_size milli seconds
                   1560,                 // packet size
                   8894,                // expected_green,
                   3126,                // expected_yellow,
                   0 ,                  // expected_red
                   false // logging_on
                   );
  }

  TEST(BFN_TEST_NAME(MetersTest),TC21) {  // from omnet
    if (meters_print) RMT_UT_LOG_INFO("test_meters_TC21()\n");
    //
    run_test_case( UINT64_C( 3400000000 ), // offered rate  mega bits / second
                   0.1,                 // run_time seconds
                   UINT64_C( 3400000000 ), // peak_rate_bits_per_second
                   UINT64_C( 6 ), // peak_burst_size milli seconds
                   UINT64_C( 3300000000 ), // committed_rate_bits_per_second,
                   UINT64_C( 5 ), // committed_burst_size milli seconds
                   800,                 // packet size
                   53135,                // expected_green,
                   0,                // expected_yellow,
                   0 ,                  // expected_red
                   false // logging_on
                   );
  }

  TEST(BFN_TEST_NAME(MetersTest),TC22) {  // from omnet
    if (meters_print) RMT_UT_LOG_INFO("test_meters_TC22()\n");
    //
    run_test_case( UINT64_C( 5100000000 ), // offered rate  mega bits / second
                   0.1,                 // run_time seconds
                   UINT64_C( 7800000000 ), // peak_rate_bits_per_second
                   UINT64_C( 4 ), // peak_burst_size milli seconds
                   UINT64_C( 7000000000 ), // committed_rate_bits_per_second,
                   UINT64_C( 3 ), // committed_burst_size milli seconds
                   768,                 // packet size
                   82988,                // expected_green,
                   0,                // expected_yellow,
                   0 ,                  // expected_red
                   false // logging_on
                   );
  }

  // test case 23 - no expected results

  // test case 24 - no expected results

  TEST(BFN_TEST_NAME(MetersTest),TC25) {  // from omnet
    if (meters_print) RMT_UT_LOG_INFO("test_meters_TC25()\n");
    //
    run_test_case( UINT64_C( 12000000 ), // offered rate  mega bits / second
                   0.1,                 // run_time seconds
                   UINT64_C( 9000000 ), // peak_rate_bits_per_second
                   UINT64_C( 350 ), // peak_burst_size milli seconds
                   UINT64_C( 1000000 ), // committed_rate_bits_per_second,
                   UINT64_C( 300 ), // committed_burst_size milli seconds
                   64,                 // packet size
                   782,                // expected_green,
                   1562,                // expected_yellow,
                   0 ,                  // expected_red
                   false // logging_on
                   );
  }

  TEST(BFN_TEST_NAME(MetersTest),TC26) {  // from omnet
    if (meters_print) RMT_UT_LOG_INFO("test_meters_TC26()\n");
    //
    run_test_case( UINT64_C( 100000000 ), // offered rate  mega bits / second
                   0.1,                 // run_time seconds
                   UINT64_C( 40000000 ), // peak_rate_bits_per_second
                   UINT64_C( 190 ), // peak_burst_size milli seconds
                   UINT64_C( 10000000 ), // committed_rate_bits_per_second,
                   UINT64_C( 100 ), // committed_burst_size milli seconds
                   768,                 // packet size
                   326,                // expected_green,
                   1302,                // expected_yellow,
                   0 ,                  // expected_red
                   false // logging_on
                   );
  }

  TEST(BFN_TEST_NAME(MetersTest),TC27) {  // from omnet
    if (meters_print) RMT_UT_LOG_INFO("test_meters_TC27()\n");
    //
    run_test_case( UINT64_C( 200000000 ), // offered rate  mega bits / second
                   0.1,                 // run_time seconds
                   UINT64_C( 60000000 ), // peak_rate_bits_per_second
                   UINT64_C( 150 ), // peak_burst_size milli seconds
                   UINT64_C( 50000000 ), // committed_rate_bits_per_second,
                   UINT64_C( 100 ), // committed_burst_size milli seconds
                   256,                 // packet size
                   4882,                // expected_green,
                   2442,                // expected_yellow,
                   2442 ,                  // expected_red
                   false // logging_on
                   );
  }

  TEST(BFN_TEST_NAME(MetersTest),TC28) {  // from omnet
    if (meters_print) RMT_UT_LOG_INFO("test_meters_TC28()\n");
    //
    run_test_case( UINT64_C( 89000000 ), // offered rate  mega bits / second
                   0.1,                 // run_time seconds
                   UINT64_C( 88000000 ), // peak_rate_bits_per_second
                   UINT64_C( 90 ), // peak_burst_size milli seconds
                   UINT64_C( 80000000 ), // committed_rate_bits_per_second,
                   UINT64_C( 50 ), // committed_burst_size milli seconds
                   1560,                 // packet size
                   714,                // expected_green,
                   0,                // expected_yellow,
                   0 ,                  // expected_red
                   false // logging_on
                   );
  }

  TEST(BFN_TEST_NAME(MetersTest),TC29) {  // from omnet
    if (meters_print) RMT_UT_LOG_INFO("test_meters_TC29()\n");
    //
    run_test_case( UINT64_C( 231000000 ), // offered rate  mega bits / second
                   0.1,                 // run_time seconds
                   UINT64_C( 230000000 ), // peak_rate_bits_per_second
                   UINT64_C( 77 ), // peak_burst_size milli seconds
                   UINT64_C( 200000000 ), // committed_rate_bits_per_second,
                   UINT64_C( 50 ), // committed_burst_size milli seconds
                   400,                 // packet size
                   7219,                // expected_green,
                   0,                // expected_yellow,
                   0 ,                  // expected_red
                   false // logging_on
                   );
  }

  TEST(BFN_TEST_NAME(MetersTest),TC30) {  // from omnet
    if (meters_print) RMT_UT_LOG_INFO("test_meters_TC30()\n");
    //
    run_test_case( UINT64_C( 412000000 ), // offered rate  mega bits / second
                   0.1,                 // run_time seconds
                   UINT64_C( 410000000 ), // peak_rate_bits_per_second
                   UINT64_C( 130 ), // peak_burst_size milli seconds
                   UINT64_C( 400000000 ), // committed_rate_bits_per_second,
                   UINT64_C( 100 ), // committed_burst_size milli seconds
                   74,                 // packet size
                   69590,                // expected_green,
                   0,                // expected_yellow,
                   0 ,                  // expected_red
                   false // logging_on
                   );
  }

  TEST(BFN_TEST_NAME(MetersTest),TC31) {  // from omnet
    if (meters_print) RMT_UT_LOG_INFO("test_meters_TC31()\n");
    //
    run_test_case( UINT64_C( 1600000000 ), // offered rate  mega bits / second
                   0.1,                 // run_time seconds
                   UINT64_C( 1500000000 ), // peak_rate_bits_per_second
                   UINT64_C( 12 ), // peak_burst_size milli seconds
                   UINT64_C( 1000000000 ), // committed_rate_bits_per_second,
                   UINT64_C( 11 ), // committed_burst_size milli seconds
                   1560,                 // packet size
                   8894,                // expected_green,
                   3927,                // expected_yellow,
                   0 ,                  // expected_red
                   false // logging_on
                   );
  }

  // This is only 0.3% out on the green packets, but this translates to 30% on yellow, so it fails.
  // Green  received = 53991    expected = 54140
  // Yellow received = 547      expected = 386
  // Red    received = 0        expected = 0

  TEST(BFN_TEST_NAME(MetersTest),DISABLED_TC32) {  // from omnet
    if (meters_print) RMT_UT_LOG_INFO("test_meters_TC32()\n");
    //
    run_test_case( UINT64_C( 3490000000 ), // offered rate  mega bits / second
                   0.1,                 // run_time seconds
                   UINT64_C( 3400000000 ), // peak_rate_bits_per_second
                   UINT64_C( 6 ), // peak_burst_size milli seconds
                   UINT64_C( 3300000000 ), // committed_rate_bits_per_second,
                   UINT64_C( 5 ), // committed_burst_size milli seconds
                   800,                 // packet size
                   54140,                // expected_green,
                   386,                // expected_yellow,
                   0 ,                  // expected_red
                   false // logging_on
                   );
  }

  TEST(BFN_TEST_NAME(MetersTest),TC33) {  // from omnet
    if (meters_print) RMT_UT_LOG_INFO("test_meters_TC33()\n");
    //
    run_test_case( UINT64_C( 7100000000 ), // offered rate  mega bits / second
                   0.1,                 // run_time seconds
                   UINT64_C( 7800000000 ), // peak_rate_bits_per_second
                   UINT64_C( 4 ), // peak_burst_size milli seconds
                   UINT64_C( 7000000000 ), // committed_rate_bits_per_second,
                   UINT64_C( 3 ), // committed_burst_size milli seconds
                   768,                 // packet size
                   115607,                // expected_green,
                   0,                // expected_yellow,
                   0 ,                  // expected_red
                   false // logging_on
                   );
  }


// Only build this next test for TofinoB0.
// It's DISABLED anyway so not too significant.
//
// If the test is built for more than 1 chip there are link
// problems with the extern C symbols being multiply defined.
//
// The symbols are referenced from .c files generated by
// lex/yacc so might be possible to put these in a namespace
// somehow but didn't really investigate further once it was
// pointed out the test was DISABLED anyway
//
#if MCN_TEST(MODEL_CHIP_NAMESPACE,tofinoB0)

extern "C" {
void log_file_parsed_cbs(uint64_t committed_burst_size);
void log_file_parsed_cir_persec(float committed_rate_bytes_per_second);
void log_file_parsed_pbs(uint64_t peak_burst_size);
void log_file_parsed_pir_persec(float peak_rate_bytes_per_second);
void log_file_parsed_total_pkts(uint64_t pkts);
void log_file_parsed_green_pkts(uint64_t pkts);
void log_file_parsed_yel_pkts(uint64_t pkts);
void log_file_parsed_red_pkts(uint64_t pkts);
void setup_bursty_meter();
void bursty_meter_inj_pkt(float inj_time, int pktlen, char* pktcolor, int);
void get_bursty_meter_test_result();
void print_bursty_meter_total_passed_count();
}

static MeterEntry Burstyentry;

void log_file_parsed_cbs(uint64_t committed_burst_size) {
  // let config bytes
  Burstyentry.set_from_log_file_cbs(committed_burst_size);
}

void log_file_parsed_cir_persec(float committed_rate_bytes_per_second) {
  Burstyentry.set_from_log_file_cir_persec(committed_rate_bytes_per_second);
}

void log_file_parsed_pbs(uint64_t peak_burst_size) {
  // let config bytes
  Burstyentry.set_from_log_file_pbs(peak_burst_size);
}

void log_file_parsed_pir_persec(float peak_rate_bytes_per_second) {
  Burstyentry.set_from_log_file_pir_persec(peak_rate_bytes_per_second);
}

void log_file_parsed_total_pkts(uint64_t pkts) {
  Burstyentry.set_from_log_file_total_pkts(pkts);
}
void log_file_parsed_green_pkts(uint64_t pkts) {
  Burstyentry.set_from_log_file_total_green_pkts(pkts);
}
void log_file_parsed_yel_pkts(uint64_t pkts) {
  Burstyentry.set_from_log_file_total_yel_pkts(pkts);
}
void log_file_parsed_red_pkts(uint64_t pkts) {
  Burstyentry.set_from_log_file_total_red_pkts(pkts);
}

TestUtil *bursty_tu;
static int g_bursty_tc_count = 0;
static int g_bursty_passed = 0;
static int g_bursty_failed = 0;

  void setup_bursty_meter()
  {
    //constexpr bool quick_mode = false;
    //int quick_mode_divisor = quick_mode ? 10 : 1;
    int chip = 202;
    int pipe = 0;
    int stage = 0;

    bursty_tu = new TestUtil(GLOBAL_MODEL.get(), chip, pipe, stage);


    // Spits out debugs for every packet...
    //configure_logging(*bursty_tu,pipe,stage, true);
    configure_logging(*bursty_tu,pipe,stage, false);

    // set meter row and which entry in meter to use
    int row=1;
    int entry=7;

    // set the MAU up so it runs a meter on every packet (in meter_util.h)
    setup_for_meter_test(*bursty_tu,pipe,stage,row,entry);

    // configure the meter
    bursty_tu->meter_config(0,0,row, // pipe0, stage0
                    false,   // egress
                    false,   // red
                    false,   // lpf
                    true,    // byte
                    0);      // byte_count_adjust

    Burstyentry.set_from_log_file();

    bursty_tu->sram_write(0 /*pipe*/, 0 /*stage*/, row, 6 /*col*/, entry /*index*/,
                  Burstyentry.get_data0(), Burstyentry.get_data1() );

    init_for_pkt_by_pkt_metertest(*bursty_tu, pipe, stage, row, entry);

    g_bursty_tc_count++;

  }

  bool result_within_tolerance_limit(uint64_t result, uint64_t expected_result,
                                     uint64_t total_pkts, int tolerance_limit_pcent)
  {
    int model_pcent , sim_pcent;

    model_pcent = 100 * result / total_pkts;
    sim_pcent = 100 * expected_result / total_pkts;

    if (model_pcent > sim_pcent) {
      return ((model_pcent - sim_pcent) <= tolerance_limit_pcent) ? true : false;
    } else {
      return ((sim_pcent - model_pcent) <= tolerance_limit_pcent) ? true : false;
    }
    return (false);
  }

#define METER_TOLERANCE_LIMIT 10  /* 10 % */

  void bursty_meter_inj_pkt(float inj_time, int pktlen, char* pktcolor, int loggingon)
  {
    int pipe = 0;
    int stage = 0;
    int row=1;
    int entry=7;
    int expected_color;


    if (strncmp(pktcolor, "GREEN", strlen("GREEN")) == 0) {
      expected_color = 0;
    } else if (strncmp(pktcolor, "YEL", strlen("YEL")) == 0) {
      expected_color = 1;
    } else {
      expected_color = 3;
    }
    if (pktlen > 0) {
      run_meter_pkt_by_pkt(*bursty_tu, pipe, stage, row, entry, inj_time * 1250000000, pktlen, expected_color, (loggingon) ? true : false);
    } else {
      // end of test. Check color pkt count
      uint64_t green, yel, red;
      get_color_count_for_pkt_by_pkt_metertest(&green, &yel, &red);
      if ((green == Burstyentry.expected_green_pkts) &&
          (yel == Burstyentry.expected_yel_pkts) &&
          (red == Burstyentry.expected_red_pkts)) {
          // Test passed.
          cout << "Test Number " << g_bursty_tc_count << " TEST PASSED\n";
          g_bursty_passed++;
      } else {
          uint64_t total_pkts = green + yel + red;
          if (!result_within_tolerance_limit(green, Burstyentry.expected_green_pkts, total_pkts,
                                             METER_TOLERANCE_LIMIT) ||
              !result_within_tolerance_limit(yel, Burstyentry.expected_yel_pkts,  total_pkts,
                                             METER_TOLERANCE_LIMIT) ||
              !result_within_tolerance_limit(red, Burstyentry.expected_red_pkts, total_pkts,
                                             METER_TOLERANCE_LIMIT)) {
            if (total_pkts > 4000) {
              cout << "Test Number " << g_bursty_tc_count << " TEST FAILED" << "\n";
              g_bursty_failed++;
            } else {
              cout << "Ignoring Test results as the number of packets injected in "
                      "100ms is less than 4000. The glacial packet arrival rate processing "
                      "need not match simulation results; TEST IGNORED " << "\n";
            }

          } else {
            cout << "Test Number " << g_bursty_tc_count << " TEST PASSED\n";
          }
      }
        cout << "Model Processed " << green << " Green Packets." << " Expected  " << Burstyentry.expected_green_pkts << " as per Simulation \n";
        cout << "Model Processed " << yel << " Yellow Packets." << " Expected  " << Burstyentry.expected_yel_pkts << " as per Simulation \n";
        cout << "Model Processed " << red << " Red Packets." << " Expected  " << Burstyentry.expected_red_pkts << " as per Simulation\n";
        cout << "\n" << "\n" << "\n";
        delete bursty_tu;
      }
  }



  void get_bursty_meter_test_result()
  {
      // end of test. Check color pkt count
      uint64_t green, yel, red;
      get_color_count_for_pkt_by_pkt_metertest(&green, &yel, &red);
      if ((green == Burstyentry.expected_green_pkts) &&
          (yel == Burstyentry.expected_yel_pkts) &&
          (red == Burstyentry.expected_red_pkts)) {
          // Test passed.
          cout << "Test Number " << g_bursty_tc_count << "  TEST PASSED\n";
      } else {
          uint64_t total_pkts = green + yel + red;
          if (!result_within_tolerance_limit(green, Burstyentry.expected_green_pkts, total_pkts,
                                             METER_TOLERANCE_LIMIT) ||
              !result_within_tolerance_limit(yel, Burstyentry.expected_yel_pkts, total_pkts,
                                             METER_TOLERANCE_LIMIT) ||
              !result_within_tolerance_limit(red, Burstyentry.expected_red_pkts, total_pkts,
                                             METER_TOLERANCE_LIMIT)) {
            if (total_pkts > 4000) {
              cout << "Test Number " << g_bursty_tc_count << " TEST FAILED" << "\n";
            } else {
              cout << "Ignoring Test results as the number of packets injected in "
                      "100ms is less than 4000. The glacial packet arrival rate processing "
                      "need not match simulation results; TEST IGNORED " << "\n";
            }
          } else {
            cout << "Test Number " << g_bursty_tc_count << " TEST PASSED\n";
          }
      }
      cout << "Model Processed " << green << " Green Packets." << " Expected  " << Burstyentry.expected_green_pkts << " as per Simulation \n";
      cout << "Model Processed " << yel << " Yellow Packets." << " Expected  " << Burstyentry.expected_yel_pkts << " as per Simulation \n";
      cout << "Model Processed " << red << " Red Packets." << " Expected  " << Burstyentry.expected_red_pkts << " as per Simulation\n";
      cout << "\n" << "\n" << "\n";
  }

  void print_bursty_meter_total_passed_count()
  {
    cout << "\n";
    cout << "\n";
    cout << "----------- FINAL RESULT After processing multiple log files ---------------------\n";
    cout << g_bursty_passed << " Meter Test cases Passed;" << g_bursty_failed << " test cases failed out of  " <<  g_bursty_tc_count << "  Test cases" << "\n";
    cout << "----------------------------------------------------------------------------------\n";
  }


extern "C" {
  extern void harlyn_model_bursty_meter_tc_entry_point();
}

  // This test case will open meter-logs and trigger bursty meter test case
  TEST(BFN_TEST_NAME(MetersTest),DISABLED_ParseLogTest) {
    if (1) RMT_UT_LOG_INFO("---------------------- test_bursty_meters_using_sim_log_files ()---------------------\n");
    harlyn_model_bursty_meter_tc_entry_point();
  }

#endif // MCN_TEST(MODEL_CHIP_NAMESPACE,tofinoB0)

}
