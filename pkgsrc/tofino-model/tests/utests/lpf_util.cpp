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
#include "lpf_util.h"

namespace MODEL_CHIP_TEST_NAMESPACE {

using namespace MODEL_CHIP_NAMESPACE;

void lpf_time_constant_to_mantissa_exponent( double tc, int *mantissa, int *exponent ) {
  int debug=0;
  assert( tc >= 1 );
  *exponent = 0;

  double t = tc;
  while ( t >= 2.0 ) {
    (*exponent)++;
    t = t/2;
    if (debug) printf( "t = %f exponent = %d\n", t, *exponent );
  }

  double m = (t - 1.0) * 512.0;
  *mantissa = ( static_cast<int>(m) ) & 0x1ff;
  if (debug) printf( "mantissa = %d\n", *mantissa );
  if (debug) printf( "exponent = %d\n", *exponent );
}

double mantissa_exponent_to_lpf_time_constant( int mantissa, int exponent) {

  return ( 1 + ( static_cast<double>(mantissa) / 512 ) ) * pow( 2.0, exponent );

}

bool result_within_tolerance_limit(uint64_t result, uint64_t expected_result,
                                   int tolerance_limit_pcent)
{
  uint64_t new_val = expected_result * tolerance_limit_pcent / 100;
  uint64_t dev = 0;

  if (result > expected_result) {
    dev = result - expected_result;
  } else {
    dev = expected_result - result;
  }

  if (dev <= new_val) {
    return (true);
  }
  return (false);
}

void run_lpf_test( TestUtil& tu, int pipe,int stage,int row, int entry,
                   std::vector<LpfEvent> const& events,
                   bool logging_on
                   )
{
  RmtObjectManager *om = tu.get_objmgr();

  // Get a port - one associated with pipe 0
  // This also sets up basic config ingress parser and deparser
  int port_number = 16;
  int port_pipe = Port::get_pipe_num(port_number);
  int ipb_num = Port::get_ipb_num(port_number);
  assert( pipe == port_pipe ); // Sanity check
  Port *port = tu.port_get(port_number);

  // XXX disabling the ingress buffer logic - this is causing some problem
  // temp disable ingress buffer for this test
  Ipb *ib = om->ipb_lookup(pipe, ipb_num);
  ib->set_meta_enabled(false);
  ib->set_rx_enabled(true);
  int prsr_num = Port::get_parser_num(port_number);
  Parser *prsr = om->parser_lookup(pipe, prsr_num)->ingress();
  prsr->set_hdr_len_adj(16);


  //std::array<int,4> colors{};
  int packet_count=0;
  int pass = 0;
  int fail = 0;

  for (LpfEvent e : events) {
    uint64_t cycle_time = kClockRate * e.time;
    if (logging_on) printf("---- Packet count = %d | time = %f (%" PRIi64
                           ") | in = %f | expected out = %f\n",packet_count,
                           e.time,cycle_time,e.input,e.output);
    packet_count++;
    // Construct a packet - DA,SA,SIP,DIP,Proto,SPort,DPort
    int packet_size = 64;
    int payload_size = packet_size - 38; // 26 extra bytes are needed to get to 64 and 64-26=38
    assert( payload_size >= 0 );
    char buf[payload_size+1];
    for (int i=0;i<payload_size;++i) buf[i] = 'x';
    buf[payload_size] = 0;

    uint64_t da_mac = UINT64_C(0x080100000000);
    // put the input into the bottom 32 bits of the MAC DA - this is where it is extracted
    //  and sent to the D input of the meter from
    uint32_t in_int = e.input;
    da_mac |= in_int;
    // convert the MAC DA to the string libcrafter needs
    char da_str[20];
    sprintf(da_str,"%02x:%02x:%02x:%02x:%02x:%02x",
            (int)((da_mac>>40)&0xff),
            (int)((da_mac>>32)&0xff),
            (int)((da_mac>>24)&0xff),
            (int)((da_mac>>16)&0xff),
            (int)((da_mac>> 8)&0xff),
            (int)((da_mac>> 0)&0xff));
    Packet *p_in = tu.packet_make(da_str, "08:00:11:DD:EE:FF",
                                  "10.17.34.51", "10.68.85.102",
                                  TestUtil::kProtoTCP, 0x1188, 0x1199,
                                  buf );
    // set up rough time info
    p_in->setup_time_info( cycle_time );

    int pkt_len = p_in->len() - 16; // 16 bytes is trimmed in pipe.cpp trim_front
    assert( pkt_len == packet_size );

    Packet *p_out = tu.port_process_inbound(port, p_in);

    uint64_t data0,data1;
    tu.sram_read(0 /*pipe*/, 0 /*stage*/, row, 6 /*col*/, 7 /*index*/, &data0, &data1 );
    LpfEntry read_back0( data0, data1 );

    if (logging_on) printf("Cycle=%" PRIu64 " PacketLen=%d\n",cycle_time, pkt_len);
    if (logging_on) printf(" timestamp = %" PRIi64 " v_old = %" PRIi64 " (0x%" PRIx64 ")\n",
                           read_back0.timestamp,read_back0.v_old, read_back0.v_old );

    double tolerance = 0.10; // 10% - maybe 6%
    EXPECT_NEAR( read_back0.v_old , e.output  , e.output * tolerance);

    if (result_within_tolerance_limit(read_back0.v_old, e.output, LPF_TOLERANCE_LIMIT)) {
      pass++;
    }  else {
     fail++;
    }

    if (logging_on) printf("Data,%f,%f,%" PRIi64 "\n",e.time,e.output,read_back0.v_old);

    // Free packet(s)
    if (p_out != NULL) {
      if  (p_out != p_in) {
        tu.packet_free(p_out);
      }
      tu.packet_free(p_in);
    }
  }
  if (logging_on) printf(" %d Events failed out of %d \n", fail, (fail+pass));
}

}
