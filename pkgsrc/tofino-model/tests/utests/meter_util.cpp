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
#include <register_includes/reg.h> // for with registers test
#include "meter_util.h"
#include "input_xbar_util.h"


namespace MODEL_CHIP_TEST_NAMESPACE {

  using namespace MODEL_CHIP_NAMESPACE;

  int RATE_ROUNDING_TOLERANCE = 15;

void rate_to_mantissa_exponent( uint64_t rate_bits_per_s, int burstsize_exponent_adj, uint64_t *rate_mantissa, uint64_t *rate_relative_exponent ) {
  int debug=0;
  assert( (burstsize_exponent_adj) >=0 && (burstsize_exponent_adj <= ( 31 -14 )));

  uint64_t rate_bytes_per_s = rate_bits_per_s / 8;
  if (debug) printf( "rate_bytes_per_s = %" PRIu64 "\n", rate_bytes_per_s );

  double bytes_per_cycle = static_cast<double>( rate_bytes_per_s ) / kClockRate;

  if (debug) printf( "bytes_per_cycle = %f\n", bytes_per_cycle );

  //int negative_exponent = 0 , rate_rounded = 0, pcent_rounded;
  int negative_exponent = 0;
  double t = bytes_per_cycle;
  // max allowed is 511, so must stop if we would overshoot
  // also negative_exponent must 31 or less, so rate_relative_exponent stays positive
  //while ( t < (511/2.0) && (negative_exponent<31)) {
  while ( t < (511/2.0) && (negative_exponent<27)) {  // can't handle >=28 at present 1/9/15
    negative_exponent++;
    t = t*2;
#if 0
    // Inorder to favour more frequent refill/crediting buckets,
    // rate_exponent should be as less as possible.
    // Remember that in an attempt to keep exponent minimal,
    // fraction part of mantissa could lead to significant loss. In such cases
    // lets sacrifice re-fill frequency. (Go to next higher exponent).
    // Basically try to find M and E such that E is as minimal as possible and
    // loss of fraction portion in M is not much.
    // If Modifying CIR/PIR is needed to achieve this, change CIR/PIR  by +- 10% to 15%
    if (t > 1) {
      // check fraction portion of  t
      double frac = t - int(t);
      double frac2 = 1;
      frac2 -= frac;
      pcent_rounded = (int)((frac2 * 100)/t);
      if (debug) printf(" Mantissa = %f, Mantissa-fraction = %f, Mantissa-frac-away-from-1 = %f, Pcent_rounding needed = %d \n", t, frac, frac2, pcent_rounded);
      if (pcent_rounded < RATE_ROUNDING_TOLERANCE) {
        t += frac2;
        rate_rounded = 1;
        break;
      }
    }
#endif
    if (debug) printf( "t = %f exponent = %d\n", t, negative_exponent );
  }

#if 0
  if (rate_rounded) {
    if (debug) printf( "Rate rounded by %d Percent. New Rate per sec = %" PRIu64 "\n", pcent_rounded,  rate_bytes_per_s + ((rate_bytes_per_s * pcent_rounded) / 100) );
  }
#endif

  *rate_mantissa = ( static_cast<int>(t) ) & 0x1ff;
  assert( *rate_mantissa != 0 ); // this could happen if the requested rate is too small
  if (debug) printf( "rate_mantissa = %" PRIu64 "\n", *rate_mantissa );
  *rate_relative_exponent = 31 - negative_exponent;
  if (debug) printf( "rate_relative_exponent = %" PRIu64 "\n", *rate_relative_exponent );

}

uint64_t mantissa_exponent_to_rate( uint64_t mantissa, uint64_t relative_exponent, int burstsize_exponent_adj) {
  int debug=0;

  int rate_exponent = 31- relative_exponent + 0 + burstsize_exponent_adj;
  double bytes_per_cycle = static_cast<double>(mantissa) * pow( 2.0, - rate_exponent );
  uint64_t rate = 8 * static_cast<uint64_t>( bytes_per_cycle * kClockRate );
  if (debug) printf( "rate = %" PRIu64 "\n", rate );
  return rate;

}


void burst_size_to_mantissa_exponent( uint64_t burst_size, uint64_t *mantissa, uint64_t *exponent, int *exponent_adj) {

  uint64_t t = burst_size;
  *exponent = 0;
  while ( t > 255 ) {
    (*exponent)++;
    t = t>>1;
  }
  if (*exponent > 31) {
    printf("WARNING exponent too high burst_size = %" PRIu64 " mantissa = %" PRIu64 " exponent = %" PRIu64 "\n", burst_size, *mantissa, *exponent);
    *exponent=31;
  }
  *mantissa = t & 0xff;
  *exponent_adj = (*exponent > 14) ? (*exponent - 14) : 0;
  printf( "burst_size = %" PRIu64 " mantissa = %" PRIu64 " exponent = %" PRIu64 " exponent_adj=%d\n", burst_size, *mantissa, *exponent, *exponent_adj);
}

uint64_t mantissa_exponent_to_burst_size( uint64_t mantissa, uint64_t exponent) {
  double rf = mantissa * pow(2.0, static_cast<double>(exponent) );
  uint64_t r = static_cast<uint64_t>(rf);
  //printf( "burst_size = %" PRIu64 "\n", r );
  return r;
}

  // This is mostly ripped off from one of the MAU tests. It sets up a logical table
  //  and a logical TCAM with a default entry so it always hits so predicated on.
  //  It puts the meter address into the default, so everything gets this address.
  //  The color map ram is in column 9
  //  The meter ram is in column 6
  //  You can choose the row (must have a meter ALU of course) and which meter entry
  //   to use in the meter ram (and corresponding color ram)
void setup_for_meter_test(TestUtil& tu,int pipe,int stage,int row, int entry,bool lpf /*=false*/) {
    // Instantiate whole chip and fish out objmgr
    //tu.chip_init_all();
    RmtObjectManager *om = tu.get_objmgr();
    ASSERT_TRUE(om != NULL);

    int port_number = 16;
    int port_pipe = Port::get_pipe_num(port_number);
    int ipb_num = Port::get_ipb_num(port_number);
    assert( pipe == port_pipe ); // Sanity check
    Ipb *ib = om->ipb_lookup(pipe, ipb_num);
    ib->set_meta_enabled(true);
    ib->set_rx_enabled(true);


    auto& mau_base = RegisterUtils::ref_mau(pipe,stage);

    // Set all PHV ingress/egress threads for our 2 stages (chip9!)
    for (int s = 0; s < 2; s++) {
      tu.set_phv_range_all(s, false);
      // Then set 1st 32 of the 64x32 64x8 to be ingress
      // and the 1st 48 of the 96x16 to be ingress
      tu.set_phv_ranges(s, 32,0, 32,0, 32,16,0, true);
    }

    // Setup ingress dependencies for stages
    tu.set_dependency(0, TestUtil::kDepAction, true);
    tu.set_dependency(1, TestUtil::kDepAction, true);
    // Setup egress dependencies for stages
    tu.set_dependency(0, TestUtil::kDepConcurrent, false);
    tu.set_dependency(1, TestUtil::kDepConcurrent, false);

    int lt0 = 0, lt5 = 5;

    // Setup single logical table for ingress in stage0
    //
    tu.table_config(0, lt0, true);     // stage0  table0  ingress
    tu.table_config(0, lt5, true);     // stage0  table5  ingress
    tu.set_table_default_regs(0, lt0); // stage0 table0
    tu.set_table_default_regs(0, lt5); // stage0 table5

    // Setup stage default regs (need to do this after dependency
    // config so mpr_bus_dep can get setup properly);
    tu.set_stage_default_regs(0); // stage 0
    tu.set_stage_default_regs(1); // stage 1

    // Setup this logical table 0 and physical result bus 6
    // to have certan shift/mask/dflt/miss vals
    //
    tu.physbus_config(0,    lt0,  6,       // stage0  table0  physResultBus6
                      0,    0xFF, 0, lt5,  // nxt_tab shft/mask/dflt/miss (shift ignored)
                      16,   0xFF, 0, 0x6,  // instr   shft/mask/dflt/miss (no pad for instr addr)
                      32+5, 0xFFFFFFFF, 0, 0xABABABAB); // action_addr

    // Now setup a single logical tcam (4) to refer to this table
    tu.ltcam_config(0, 4, lt0, 4, (uint8_t)4); // stage0 ltcam4 table0 physbus4 matchAddrShift

    // Then setup a TCAM(4,0) within this logical TCAM
    // We use vpn now in match_addr NOT tcam_index so for now set vpn to be same (4)
    tu.tcam_config(0, 4,   0,             // stage0 row4 col0,
                   0, lt0, 4, 4,          // inbus0 table0 ltcam4 vpn4
                   true, false, true, 1); // ingress chain output head

    tu.set_debug(false);
    //LOG_VERBOSE("MauTest::Adding TIND 3,3\n");
    // Then setup a TIND SRAM - has to be on row 3 to drive physical result bus 6
    tu.sram_config(0, 3, 3,                          // stage0 row3 col3
                   TestUtil::kUnitramTypeTind, 0, 0, // type tind_addr_bus tind_out_bus
                   lt0, 4, 1, 0);                    // table0 ltcam4 vpn0 vpn1
    uint64_t result = UINT64_C(0x000000FF); // Set bot 8 bits (nxt_tab) to 0xFF
    // Fill all entries with default results - instr=0(0),nxt_tab=0xFF
    for (int i = 0; i < 1024; i++) tu.sram_write(0, 3, 3, i, result, result);
    // Set entry 253 to real results we want - instr=0x4(0x8 post add LSB 0),nxt_tab=7
    // Instr 0x4(0x8) translates to opindex=2,colour=0(,ingress=0)
    result = UINT64_C(0x0000DCBA00040005);
    tu.sram_write(0, 3, 3, 253, result, result);

    // And a catch all entry which should match everything
    // Payload0 has to be 1 now to indicate a boundary
    tu.tcam_write_value_mask(0, 4, 0, 505,   // stage0 row4 col0 index511
                             0x0, 0x0, 1,0); // value mask payload0/1


    // --- START METER SPECIFIC CONFIG ---
    int vpn=5;
    int bus_num=6;
    int log_table=lt0;
    tu.mapram_config(0,0,row,9/*col*/,TestUtil::kMapramTypeColor,
                     vpn, -1, 0/*mflags?*/,
                     log_table, false /*egress*/);
    uint32_t color_value = 0; // 4 x 2bit color values

    tu.mapram_write(stage,row,9/*col*/,entry>>2, color_value, 0); // set color data to zero

    auto& mm_regs = mau_base.rams.match.merge;
    // for color mapram: 4 bits are shifted off the bottom, then 2 bits subword, 10 bits index
    uint32_t i_dflt = (1<<20) | (vpn<<(10+2+4)) | (entry<<4);
    // that will have set the color ram to listen on idletime address, so
    //   here we set up a default for the idletime address too
    auto a_i_dflt = &mm_regs.mau_idletime_adr_default[1/*xm_tm*/][bus_num];
    GLOBAL_MODEL->OutWord(tu.get_chip(),(void*)a_i_dflt, i_dflt);

    int meter_type = (lpf) ?2 :6; // lpf/color blind OR color aware
    int meter_vpn = vpn<<2; // color ram vpn and meter vpn are not the same!
    uint32_t m_dflt = (meter_type<<24) | (1<<23) | (meter_vpn<<(10+7)) | (entry<<7);
    auto a_m_dflt = &mm_regs.mau_meter_adr_default[1/*xm_tm*/][bus_num];
    GLOBAL_MODEL->OutWord(tu.get_chip(),(void*)a_m_dflt, m_dflt);

    // Set a stats address default too to stop endless complaints from get_pfe()
    uint32_t s_dflt = (1<<19);
    auto a_s_dflt = &mm_regs.mau_stats_adr_default[1/*xm_tm*/][bus_num];
    GLOBAL_MODEL->OutWord(tu.get_chip(),(void*)a_s_dflt, s_dflt);

    // Configure mapram with meter VPN - physical write so must invert
    // (also, now that TofinoB0 uses ~bits[21:11] as a write_mask we
    //  must mask the inverted VPN with 0x7FF to keep bits[21:11] == 0
    //  and ~bits[21:11] == 0x7FF thus writing ALL bits)
    uint64_t meter_vpn_write_val = ~(static_cast<uint64_t>(meter_vpn)) & 0x7FF;
    tu.mapram_write(stage,row, 6, entry, meter_vpn_write_val, 0);

    tu.rwram_config(stage, row, 6,                           // stage row col6
                    TestUtil::kUnitramTypeMeter, meter_vpn, meter_vpn, // type vpn0=0 vpn1=0
                    0, log_table, false ,      // s_format=0 log_table egress=F
                    lpf ? false : true); // use deferred rams for normal meters, not for lpf


    if ( !lpf ) {
      auto& adist_regs = mau_base.rams.match.adrdist;
      // set meter_enable bit (used for color) for logical table 0
      GLOBAL_MODEL->OutWord(tu.get_chip(),&adist_regs.meter_enable, 1<<lt0);
    }

    if ( lpf ) {
      // Route DA_LO_32 (Phv::make_word(0,0) == phv word 0) to 4 exact match bytes numbered 24 and up
      //  (ie in the second half of second 128bit group) for use as D by LPF meters (in Tofino takes from the ms 64 bits)
      int start_output_byte = 24; /* start output byte */

      // On JBay D is taken from all 128 bits, so we have to have the start output byte eight bytes earlier
      if ( MauDefs::kStatefulMeterAluDataBits == 128 ) {
        start_output_byte = 16;
      }

      input_xbar_util::set_32_bit_word_src(tu.get_chip() /*chip*/,pipe, stage,
                                           start_output_byte,
                                           true /*enable*/,
                                           0 /*which_phv_word*/ );
      // configure the xbar
      uint32_t w = 0u;
      auto& vh_xbar = mau_base.rams.array.row[row].vh_xbar[1].stateful_meter_alu_data_ctl;
      setp_stateful_meter_alu_data_ctl_stateful_meter_alu_data_xbar_ctl( &w, 0x8 /*enable*/ | 0x1 /*group*/);
      setp_stateful_meter_alu_data_ctl_stateful_meter_alu_data_bytemask (&w, 0x0f ); // take bottom 4 bytes
      GLOBAL_MODEL->OutWord(tu.get_chip(),&vh_xbar, w);
    }
  }

  void run_meter_test( TestUtil& tu, int pipe,int stage,int row, int entry,
                       uint64_t offered_rate, // Mega bits /second
                       double   run_time,   // seconds
                       int packet_size,
                       int   expected_green,
                       int   expected_yellow,
                       int   expected_red,
                       bool logging_on
                       )
  {

    uint64_t cycle_count = 0;

    int port_number = 16;
    Port *port = tu.port_get(port_number);

    std::array<int,4> colors{};
    int packet_count=0;
    while ( cycle_count < ( kClockRate * run_time ) ) { // 100ms
      if (logging_on) printf("---- Packet count = %d\n",packet_count);
      packet_count++;
      // find out what color the packet will get
      int color = tu.color_entry_read(row,9/*col*/,entry);
      assert(color<4);
      colors[color]++;
      // Construct a packet - Ethernet hdr + IP hdr + TCP hdr = 54 bytes
      int payload_size = packet_size - 54;
      assert( payload_size >= 0 );
      char buf[payload_size+1];
      for (int i=0;i<payload_size;++i) buf[i] = 'x';
      buf[payload_size] = 0;
      Packet *p_in = tu.packet_make("08:00:22:AA:BB:CC", "08:00:11:DD:EE:FF",
                                    "10.17.34.51", "10.68.85.102",
                                    TestUtil::kProtoTCP, 0x1188, 0x1199,
                                    buf );
                                    // "_______MIN__PAYLOAD_______");
      // set up rough time info
      p_in->setup_time_info( cycle_count );
      tu.get_objmgr()->rmt_log_packet(p_in);
      int pkt_len = p_in->len();
      assert( pkt_len == packet_size );
      uint64_t packets_per_second = offered_rate / (pkt_len*8);
      uint64_t cycles_between_packets = kClockRate / packets_per_second;
      cycle_count += cycles_between_packets;

      Packet *p_out = tu.port_process_inbound(port, p_in);

      if (logging_on) {
        // try color_write_data API for DS
        RmtObjectManager *om = tu.get_objmgr();
        Mau* mau_p = om->mau_lookup(pipe,stage);
        for (int lrow = 3; lrow <16; lrow=lrow+4) {
          uint8_t color_wr_data=0;
          auto lrowp = mau_p->logical_row_lookup(lrow);
          lrowp->color_write_data(&color_wr_data);
          printf(">>>> Color Write Data Row %d = %d\n",lrow,(int)color_wr_data);
        }
      }


      uint64_t data0,data1;
      tu.sram_read(0 /*pipe*/, 0 /*stage*/, row, 6 /*col*/, 7 /*index*/, &data0, &data1 );
      MeterEntry read_back0( data0, data1 );

      if (logging_on) printf("Cycle=%" PRIu64 " Delta=%" PRIu64 " Color=%d, PacketLen=%d\n",cycle_count, cycles_between_packets,color,pkt_len);
      if (logging_on) printf(" committed_level = %" PRIu64 " peak_level = %" PRIu64 "\n", read_back0.committed_level,read_back0.peak_level );

      // Free packet(s)
      if (p_out != NULL) {
        if  (p_out != p_in) {
          tu.packet_free(p_out);
        }
        tu.packet_free(p_in);
      }
    }
    for (int i=0;i<4;++i) {
      if (logging_on) printf("Color %d = %d\n",i,colors[i]);
    }
    EXPECT_EQ( 0, colors[2] ); // this one never gets set

    double tolerance = 0.011; // 1.1%

    double green_tol = expected_green * tolerance;
    double yellow_tol = expected_yellow * tolerance;
    double red_tol = expected_red * tolerance;

    // at one point I thought this would be a good thing to do. Doesn't
    //  help with my failing test because expected value is small, but not zero
    //int total = expected_red + expected_yellow + expected_green;
    //if (expected_green == 0) expected_green = total * tolerance;
    //if (expected_yellow == 0) expected_yellow = total * tolerance;
    //if (expected_red == 0) expected_red = total * tolerance;

    EXPECT_NEAR( colors[0] , expected_green  , green_tol);
    EXPECT_NEAR( colors[1] , expected_yellow , yellow_tol);
    EXPECT_NEAR( colors[3] , expected_red    , red_tol);

    printf("Green  received = %-6d   expected = %-6d\n",colors[0],expected_green);
    printf("Yellow received = %-6d   expected = %-6d\n",colors[1],expected_yellow);
    printf("Red    received = %-6d   expected = %-6d\n",colors[3],expected_red);

  }

static std::array<int,4> pkt_colors{};

void init_for_pkt_by_pkt_metertest(TestUtil& tu, int pipe,int stage,int row, int entry)
{
    RmtObjectManager *om = tu.get_objmgr();
    int port_number = 16;
    Ipb *ib = om->ipb_lookup(pipe, port_number / RmtDefs::kChannelsPerIpb);
    ib->set_meta_enabled(false);
    ib->set_rx_enabled(true);

    for (int i=0;i<4;++i) {
      pkt_colors[i] = 0;
    }
}

void get_color_count_for_pkt_by_pkt_metertest(uint64_t *green, uint64_t *yel, uint64_t *red)
{
  *green = pkt_colors[0];
  *yel = pkt_colors[1];
  *red = pkt_colors[3];
}


  void run_meter_pkt_by_pkt( TestUtil& tu, int pipe,int stage,int row, int entry,
                             uint64_t cycle, int packet_len,
                             int   expected_color,
                             bool logging_on)
  {

    //uint64_t cycle_count = 0;
    Port *port = tu.port_get(16);

      if (logging_on) printf("---- Packet len = %d\n", packet_len);
      // find out what color the packet will get
      int color = tu.color_entry_read(row,9/*col*/,entry);
      assert(color<4);
      pkt_colors[color]++;
      if (logging_on && color != expected_color)
        printf("---- Color mismatch: expected = %d, got %d\n", expected_color,color);

      // Construct a packet - DA,SA,SIP,DIP,Proto,SPort,DPort
      int payload_size = packet_len - 38; // 26 extra bytes are needed to get to 64 and 64-26=38
      assert( payload_size >= 0 );
      char buf[payload_size+1];
      for (int i=0;i<payload_size;++i) buf[i] = 'x';
      buf[payload_size] = 0;
      Packet *p_in = tu.packet_make("08:00:22:AA:BB:CC", "08:00:11:DD:EE:FF",
                                    "10.17.34.51", "10.68.85.102",
                                    TestUtil::kProtoTCP, 0x1188, 0x1199,
                                    buf );
                                    // "_______MIN__PAYLOAD_______");
      // set up rough time info
      p_in->setup_time_info( cycle );

      int pkt_len = p_in->len() - 16; // 16 bytes is trimmed in pipe.cpp trim_front
      assert( pkt_len == packet_len );

      Packet *p_out = tu.port_process_inbound(port, p_in);

      if (logging_on) {
        // try color_write_data API for DS
        RmtObjectManager *om = tu.get_objmgr();
        Mau* mau_p = om->mau_lookup(pipe,stage);
        for (int lrow = 3; lrow <16; lrow=lrow+4) {
          uint8_t color_wr_data=0;
          auto lrowp = mau_p->logical_row_lookup(lrow);
          lrowp->color_write_data(&color_wr_data);
          printf(">>>> Color Write Data Row %d = %d\n",lrow,(int)color_wr_data);
        }
      }


      uint64_t data0,data1;
      tu.sram_read(0 /*pipe*/, 0 /*stage*/, row, 6 /*col*/, 7 /*index*/, &data0, &data1 );
      MeterEntry read_back0( data0, data1 );

      if (logging_on) printf("Cycle=%" PRIu64  " Color=%d, PacketLen=%d\n",cycle,color,pkt_len);
      if (logging_on) printf("committed_level = %" PRIu64 " peak_level = %" PRIu64 "\n", read_back0.committed_level,read_back0.peak_level );

      // Free packet(s)
      if ((p_out != NULL) && (p_out != p_in)) tu.packet_free(p_out);
      tu.packet_free(p_in);

    for (int i=0;i<4;++i) {
      if (logging_on) printf("Color %d = %d\n",i,pkt_colors[i]);
    }
    EXPECT_EQ( 0, pkt_colors[2] ); // this one never gets set


  }

}
