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

#include "pktgen_common.h"
#include "pktgen_util.h"
#include "test_pkt_q.h"

extern std::unique_ptr<model_core::Model> GLOBAL_MODEL;


namespace MODEL_CHIP_TEST_NAMESPACE {

static constexpr uint32_t DEF_TMO = 500;
static constexpr uint32_t MAX_CLK = 1500;

using namespace MODEL_CHIP_NAMESPACE;

void send_packets(TestUtil* tu,
                  int ingress_port,
                  int egress_port,
                  uint32_t n_pkts ) {

  RmtObjectManager *rmt = tu->get_objmgr();
  RmtPacketCoordinator* rc = rmt->packet_coordinator_get();

  while (n_pkts) {
    Packet* pkt = rmt->pkt_create();
    rc->set_tx_fn(tr_packet);
    pkt->set_ingress();
    pkt->set_port( rmt->port_get(ingress_port) );
    pkt->i2qing_metadata()->set_physical_ingress_port(ingress_port);
    pkt->i2qing_metadata()->set_egress_unicast_port(egress_port);
    pkt->set_metadata_added(true);
    rc->enqueue(ingress_port, pkt, false);
    --n_pkts;
  }
}

RmtPacketCoordinator* pgen_setup(TestUtil* tu, PktGen*& pgen) {

  RmtObjectManager *rmt = tu->get_objmgr();
  RmtPacketCoordinator* rc = rmt->packet_coordinator_get();
  pgen = tu->get_objmgr()->pktgen_lookup( tu->get_pipe() );
  assert(pgen != NULL);
  pgen->set_test(true);
  install_fake_pipe_process_fn(rmt, rc);
  return rc;
}



TEST(BFN_TEST_NAME(PktGen), Recirculation) {

  TestUtil *tu = new TestUtil(GLOBAL_MODEL.get(), 202, 0, 0);
  PktGen* pgen = nullptr;

  uint32_t iter = 0;
  RmtPacketCoordinator* rc = nullptr;
  assert(tu != NULL);
  rc = pgen_setup(tu, pgen);

  int pipe=0;
  auto& pgr_common_reg = RegisterUtils::addr_pmarb(pipe)->pgrreg.pgr_common;

  // enable the ethernet port so we can check this doesn't recirculate
  uint32_t csr = 0;
  setp_pgr_eth_cpu_ctrl_port_en(&csr, 1);
  setp_pgr_eth_cpu_ctrl_channel_en(&csr, 0x1); // 1st channel only
  setp_pgr_eth_cpu_ctrl_channel_mode(&csr, 0);  // 1 channel mode
  setp_pgr_eth_cpu_ctrl_channel_seq(&csr, 0); // TDM sequence: 0,0,0,0
  tu->OutWord(&pgr_common_reg.eth_cpu_port_ctrl, csr);

  // enable recirculation on EBuf0 both channels
  uint32_t eb_csr = 0;
  setp_pgr_ebuf_port_ctrl_port_en(&eb_csr, 1);
  setp_pgr_ebuf_port_ctrl_channel_en(&eb_csr, 0x3);  // both channels
  setp_pgr_ebuf_port_ctrl_channel_mode(&eb_csr, 1);  // 2 channel mode
  tu->OutWord(&pgr_common_reg.ebuf_port_ctrl[0], eb_csr);

  // port 2 is the first ethernet CPU port
  send_packets(tu, 0 /*ing port*/, 2 /*egr port*/, 2);
  rc->start();
  while ((pgen->g_mxbar_cnt_ != 2) && (iter < MAX_CLK)) {
    std::this_thread::sleep_for(std::chrono::microseconds(DEF_TMO * 4));
    ++iter;
  }
  EXPECT_EQ(2u, pgen->g_mxbar_cnt_);
  rc->stop();

  // port 0 is ebuf0,ch0 so should recirculate
  send_packets(tu, 0 /*ing port*/, 0 /*egr port*/, 2);
  rc->start();
  while ( (pgen->g_recirc_cnt_ != 2) &&
          ( iter < MAX_CLK )) {
    std::this_thread::sleep_for(std::chrono::microseconds(DEF_TMO * 4));
    iter ++;
  }

  EXPECT_EQ(2u, pgen->g_recirc_cnt_);
  EXPECT_EQ(2u, pgen->g_mxbar_cnt_);
  rc->stop();



}

TEST(BFN_TEST_NAME(PktGen), Stress) {

    // Purpose of this test is to stress test the JBay
    // PktGen logic. Strategy is:
    //
    // Create TestPktQ object
    // Fish out per-pipe TestPerPipePktQ
    // Instantiate per-pipe PktGen obj passing TestPerPipePktQ
    //
    // Loop for many iterations
    //    Setup random PktgenRandAppConfig
    //      Occasionally A. allow >1 PFC
    //                   B. don't limit batches/pkts for Linkdown
    //      See if logic spots it
    //
    //    Create PktgenRandAppMgr object
    //    Call PktgenRandAppMgr::setup_apps()
    //    Setup new random contents of PktBuf Phase0Buf
    //
    //    Randomly enable Pipe0 for TBC/EthCPUx4 (always *logical pipe* 0)
    //      Keep masks of TBC/EthCPU/Ebuf for all pipes
    //
    //    EVENT LOOP START
    //    Loop for lots of packets/events (linkdown,PFC etc)
    //      Mark down ports as UP again - LATER
    //      Move on time
    //      Synthesize event (packet/recirc/linkdown/PFC etc)
    //      Call PktGen::maybe_recirculate()  // Random Pipe/Port/Ebuf
    //           PktGen::maybe_trigger()      // Random Pipe/Port/Ebuf
    //           PktGen::handle_port_down()   // Random Pipe/Port
    //    EVENT LOOP END
    //
    //    If recirc check packet was recirculated
    //       If Port >= 8 expect false+pkt (expect unmodified pkt)
    //       If Port < 8
    //          If Pipe0 and Port setup for TBC/EthCPU
    //            expect false+pkt (expect unmodified pkt)
    //          If Port setup for IPB
    //            expect true+null
    //
    //    If packet/recirc, check if packet/recirc would match
    //      any of the 16 apps - if so increment n_trigger_exp
    //    If linkdown check if event would match
    //      any of the 16 apps - if so increment n_trigger_exp
    //
    //    Otherwise harvest output packets generated by apps from TestPktQ
    //      NB. Will need to tie up packet with ***source*** Pipe/PktGen/AppMgr
    //      Look in packet hdr for APP
    //      Check APP is enabled etc
    //      If TimeoutAPP (oneoff/periodic) increment n_trigger_act
    //      Check currT in packet is expected - given [minT,maxT] - write back lastT
    //      Check IngrPort in packet is expected - write back lastPort
    //      Check BatchNum/PktNum as expected - write back lastBatch|PktNum
    //         On very LAST pkt of very LAST batch increment n_trigger_act
    //      Check APP specific hdrs as expected
    //      Check data from random buf as expected
    //      HOW DO I CHECK buf off selected by inbound pkt???????
    //         Well I know up front if pkt will match
    //         - BUT MUST ensure mask ignores the 24b in question
    //         --- and so those 24b can contain our cycling value
    //         Maybe just cycle through buffer at 8 byte granularity
    //           So first time should see pktbuf[0] then [8] then [16] etc
    //           More state to store per-app - write back lastOff
    //
    //    Move on time until no more packets HARVESTED
    //
    //    Timed APP checks - iff enabled - at time currT
    //      n_trigger_exp can be determined from startT/currT
    //        oneoff app should have 0/1 events
    //        periodic app should have N events
    //        check these match the n_trigger_act
    //
    //    Ultimately for all apps n_trigger_act should equal n_trigger_exp
    //
    using BV72  = MODEL_CHIP_NAMESPACE::BitVector< 72>;

    ASSERT_TRUE(RmtObject::is_jbay_or_later());
    int chip = 0, pipe = 0, stage = 0;
    model_timer::ModelTimerClear(); // make sure no events are left over
    GLOBAL_MODEL->Reset();
    TestUtil tu(GLOBAL_MODEL.get(), chip, pipe, stage);
    RmtObjectManager *om = tu.get_objmgr();
    ASSERT_TRUE(om != NULL);

    uint64_t pipes, stages, types, rows_tabs, cols, flags;
    pipes = ALL; stages = ALL; types = ALL; rows_tabs = ALL; cols = ALL; flags = ALL;
    // Just to stop compiler complaining about unused vars
    flags = FEW; // Just FatalErrorWarn
    //flags = ALL;
    tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);

    // Define vars to maintain config etc
    //
    const bool ITER_DEBUG                  =    false;
    const bool APP_DEBUG                   =    false;
    const bool QUEUE_DEBUG                 =    false;
    const bool MATCH_DEBUG                 =    false;
    const bool CHECK_DEBUG                 =    false;
    const bool CHECK_MISMATCH_DEBUG        =    false; // Immediate complaint if bad generated pkt
    const bool PKT_COUNT_DEBUG             =    false;
    const bool PKT_IN_DEBUG                =    false;
    const bool MEM_DEBUG                   =    false;
    const int  RECIRC_PENDING              =        1;
    const int  num_pipes                   = MODEL_CHIP_NAMESPACE::RmtDefs::kPipesMax;
    const int  num_ports_per_pipe          =       72;
    const int  min_packet_size             =       64;
    const int  max_packet_size             =     9104;
    const int  one_tick_micros             =      500;
    const int  pkt_wait_micros             =     2000; //  1 millis
    const int  long_wait_micros            =  1000000; //  1 sec wait after all pkts sent
    int        num_iterations              =       10;
    //int        num_iterations              =       3;
    int        num_packets_per_iteration   =     1999;
    int        num_portdowns_per_iteration =       50;
    //int        tot_packets               = num_iterations * num_packets_per_iteration;
    //int        print_modulus             = (tot_packets <= 100000000) ?100 :1000;
    //uint64_t   debug_iteration           = UINT64_C(0x9436b2775525209d);
    uint64_t   debug_iteration             = UINT64_C(0);  // 0=>rand seed each iteration
    //uint64_t   debug_on_packet           = UINT64_C(0);  // 0=>none
    int        port_inc                    = RmtObject::is_chip1() ?2 :1; // XXX

    // Derive packet_portdown_mod from n_packets/n_portdowns per iteration
    int packet_portdown_mod = 0;
    if (num_portdowns_per_iteration > 0)
      packet_portdown_mod = num_packets_per_iteration/num_portdowns_per_iteration;

    // Put iteration number into packet number to ease debug
    int iteration_mult = 1;
    while (iteration_mult < num_packets_per_iteration) iteration_mult *= 10;

    // 1. Table of PktGen objects, 1 per-pipe
    // 2. Table of PktgenRandAppConfig objects, 1 per-pipe
    // 3. Table of PktgenRandAppMgr objects, 1 per-pipe
    //
    std::array< PktGen*, num_pipes >                    PktGenTab;
    std::array<struct PktgenRandAppConfig, num_pipes >  ConfigTab;
    std::array< PktgenRandAppMgr*, num_pipes >          AppMgrTab;


    // Setup TestPktQ object and fish out per-pipe TestPerPipePktQ
    // Instantiate PktGen obj passing in TestPerPipePktQ
    //
    TestPktQ  Test_Pkt_Q_ObjectOnStack{ };
    TestPktQ *Test_Pkt_Q = &Test_Pkt_Q_ObjectOnStack; // new TestPktQ{ };
    for (int p = 0; p < num_pipes; p++) {
      PktGenTab[p] = om->pktgen_get(p);
      TestPerPipePktQ *per_pipe_q = Test_Pkt_Q->get_per_pipe_q(p);
      if (RmtObject::is_chip1()) {
        // On WIP install IPB0 into per_pipe_q to get Meta0/Meta1 headers added correctly
        // (PktGen no longer does this on WIP - it lets IPB do it)
        Ipb *ipb = om->ipb_get(p, 0);
        ipb->set_meta_enabled(true);
        per_pipe_q->set_ipb( ipb );
      }
      PktGenTab[p]->set_enqueuer( per_pipe_q );
    }
    Test_Pkt_Q->set_micros_per_tick(static_cast<uint64_t>(one_tick_micros));
    Test_Pkt_Q->set_debug(QUEUE_DEBUG);


    // Setup main random number generation
    //
    std::default_random_engine main_generator;
    std::uniform_int_distribution<uint64_t> loop_seed;
    main_generator.seed( unsigned(0xDAB0D1B0D0B0DEB0) );


    // LOOP FOR LOTS OF ITERATIONS
    //
    for (int iteration = 0; iteration < num_iterations; iteration++) {

      if (ITER_DEBUG) printf("STARTING iteration %d\n", iteration);

      for (int p = 0; p < num_pipes; p++) {
        // clear port down vectors by toggling en
        tu.pktgen_port_down_vec_clr_set(p,false /*en*/, false /*app_disable*/, false /*set_sent*/);
        tu.pktgen_port_down_vec_clr_set(p,true /*en*/, false /*app_disable*/, false /*set_sent*/);
      }

      model_timer::ModelTimerClear(); // make sure no events are left over

      // Setup random number generation for this iteration
      //
      std::default_random_engine loop_generator;
      std::uniform_int_distribution<uint64_t> config_seed;
      std::uniform_int_distribution<uint64_t> packet_seed;
      std::uniform_int_distribution<uint64_t> buffer_seed;
      uint64_t iteration_seed = loop_seed(main_generator);
      if (debug_iteration != UINT64_C(0)) iteration_seed = debug_iteration;
      loop_generator.seed( iteration_seed );

      uint64_t cfg_seeds[num_pipes] = { UINT64_C(0) };
      //config_seed(loop_generator), config_seed(loop_generator),
      //  config_seed(loop_generator), config_seed(loop_generator)
      //};
      for (int p = 0; p < num_pipes; p++) cfg_seeds[p] = config_seed(loop_generator);


      // linkdown: by default set the mask/set so packet bursts are generated (ie
      //  kPgenAppFlagLimitPktsPackets not set), this means that it takes time to
      //  deal with the event and subsequent events are dropped.
      //  But see try_clearing_port_down_bits below...
      uint8_t ld_mask = 0x2E;
      uint8_t ld_set  = 0x20;
      int try_clearing_port_down_bits = tu.xrandrange(iteration_seed, 7435, 0, 1);
      if ( try_clearing_port_down_bits ) {
        // if we are clearing port down bits then the counting in this test can't cope
        //  with events being dropped, so we need to limit the packet to 1 so no
        //  events are dropped
        ld_mask |= TestUtil::kPgenAppFlagLimitPktsPackets;
        ld_set  |= TestUtil::kPgenAppFlagLimitPktsPackets;
      }

      // Now setup a random PktgenRandAppConfig per-pipe
      //
      for (int p = 0; p < num_pipes; p++) {
        ConfigTab[p] = {
          0x3F,  // maskTypes - will be overwritten to get numEachType apps
          0x3F,  // maskFlags - will be overwritten from flagsMaskEachType
          0x00,  // setFlags  - will be overwritten from flagsSetEachType
          1,     // maskBits - 1 packet in 16 will match on average if maskBits=4
          2,     // maxBatches
          2,     // maxPackets per batch
          36,    // maxBatchesPackets (max product of 2 above)
          500u,  // minPeriod
          3000u, // maxPeriod
          // Apps are { Timer, Periodic, Linkdown, Recirc, Dprsr, Pfc }
          //{ 0, 0, 0, 0, 0, 0 },                   // number each type of app
          //{ 1, 3, 1, 3, 1, 3 },                   // number each type of app
          { 1, 5, 1, 5, 5, 1 },                     // number each type of app
          // Flags are:
          //   static constexpr uint8_t kPgenAppFlagNoKey            = 0x01;
          //   static constexpr uint8_t kPgenAppFlagStopAtPktBndry   = 0x02;
          //   static constexpr uint8_t kPgenAppFlagUsePortDownMask1 = 0x04;
          //   static constexpr uint8_t kPgenAppFlagUseCurrTs        = 0x08;
          //   static constexpr uint8_t kPgenAppFlagAddrSizeFromPkt  = 0x10;
          //   static constexpr uint8_t kPgenAppFlagLimitPktsBatches = 0x20;
          //   static constexpr uint8_t kPgenAppFlagLimitPktsPackets = 0x40;
          //
          //{ 0x11, 0x11, 0x21, 0x11, 0x11, 0x11 }, // flags to mask for each type of app
          //{ 0x01, 0x01, 0x21, 0x11, 0x01, 0x01 }, // flags to mask for each type of app
          // linkdown mask/set is determined above
          { 0x0F, 0x0F, ld_mask, 0x1F, 0x0F, 0x0F },   // flags to mask for each type of app
          { 0x00, 0x00, ld_set , 0x00, 0x00, 0x00 },   // flags to set for each type of app
          { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }    // flags to set for *first* app of type
        };
      }

      // Keep track of what ports are down
      std::array< BV72, num_pipes> PortsDownTab;

      // Instantiate PktgenRandAppMgr objs (1 per-pipe) passing
      // in Config and then get each to setup apps/bufs/ports per-pipe
      //
      for (int p = 0; p < num_pipes; p++) {
        PortsDownTab[p].fill_all_zeros();
        // bring all the ports up
        for (int i = 0; i < num_ports_per_pipe; i += port_inc) {
          PktGenTab[p]->handle_port_up( Port::make_port_index(p,i) );
        }
        BV72 all_ports(0);
        all_ports.fill_all_ones();
        tu.pktgen_cmn_portdown_disable_set(p, all_ports);

        AppMgrTab[p] = new PktgenRandAppMgr{ &tu, p, cfg_seeds[p], &ConfigTab[p] };
        AppMgrTab[p]->setup_bufs();
        // Get flags in [001 010 100 101 110 111] based on iteration#
        // (In pipe0 determines whether we use tbc/ethcpu/ebuf else always just ebuf)
        uint8_t flags = (p == 0) ?((iteration_seed % 7) + 1) :TestUtil::kPgenOutPortFlagsEbuf;
        AppMgrTab[p]->setup_ports(flags); // Primarily for recirc

        AppMgrTab[p]->setup_apps(); // Setup apps last
        if (APP_DEBUG) AppMgrTab[p]->dump_apps();

      }


      // How long to wait for a packet to be enqueued
      uint64_t long_wait = static_cast<uint64_t>(long_wait_micros/one_tick_micros);
      uint64_t pkt_wait = static_cast<uint64_t>(pkt_wait_micros/one_tick_micros);
      int      pkts_cnt = 0, pkts_exp_trigger = 0, pkts_exp_portdown = 0;
      int      pkts_in_trigger = 0, pkts_out_trigger = 0;
      int      pkts_ok_trigger = 0, pkts_err_trigger = 0;
      int      pkts_in_recirc = 0, pkts_out_recirc = 0;
      int      last_trigger_err = 0, packet = 0;
      int      pkts_drained_zero_cnt = 0;

      while (true) {
        uint64_t pkt_rand = packet_seed(loop_generator);

        // Every so often maybe take a port down - but only ports in [8..71]
        if ((packet > 0) && (packet_portdown_mod > 0) &&
            ((packet % packet_portdown_mod) == 0)) {

          int portdown_pipe = tu.xrandrange(pkt_rand, 5000, 0, num_pipes-1);
          int portdown_port = -1;

          if (try_clearing_port_down_bits) {
            // first randomly clear some bits of the disable mask (write 1 to clear)
            BV72 dis_clear(UINT64_C(0));
            for (int i=0;i<num_ports_per_pipe;++i) {
              // set about 20% of the bits
              if ( 0 != tu.xrandrange(pkt_rand, 2900 + i,  0, 4) ) {
                // don't clear bits for ports where we are currently waiting for a trigger
                //  because this can cause 2 triggers to happen when the test only
                //  expects one.
                bool expected_port_down = false;
                for (int app = 0; app < PktgenRandAppMgr::kNumAppsPerPipe; app++) {
                  PktgenRandApp *papp = AppMgrTab[portdown_pipe]->get_app(app);
                  if (papp->get_expected_port_down(i))
                    expected_port_down = true;
                }
                if ( !expected_port_down ) {
                  dis_clear.set_bit( i );
                }
              }
            }
            tu.pktgen_cmn_portdown_disable_set(portdown_pipe, dis_clear);
          }



          portdown_port = tu.xrandrange(pkt_rand, 5005, 8, num_ports_per_pipe-1);

          RMT_ASSERT(portdown_port >= 8);

          // sometimes try setting up to 5 ports down to provoke dropping of events
          int n_ports_down = tu.xrandrange(pkt_rand, 9800,  1, 10);
          n_ports_down = n_ports_down<5 ? 1 : n_ports_down-5; // more at one

          // Figure out whether we have an APP that'll be triggered by this
          // XXX: WIP: apps only ever configured to care about EVEN ports
          for (int app = 0; app < PktgenRandAppMgr::kNumAppsPerPipe; app++) {
            PktgenRandApp *papp = AppMgrTab[portdown_pipe]->get_app(app);

            for (int i=0;i<n_ports_down && (i+portdown_port)<num_ports_per_pipe; ++i) {
              int port_n = portdown_port + i;
              if ( papp->portdown_triggered(port_n) && ! papp->get_expected_port_down(port_n)) {
                //printf("EXPECTED TRIGGER %d_%d port %d\n",portdown_pipe,app,i+portdown_port);
                // XXX: WIP: should only ever trigger on portdown of EVEN ports
                if (RmtObject::is_chip1()) RMT_ASSERT((port_n % 2) == 0);
                pkts_exp_portdown++;
                papp->inc_n_triggers_exp();
                papp->set_expected_port_down(port_n);
                PortsDownTab[portdown_pipe].set_bit(port_n);
              }
            }
          }

          for (int i=0;i<n_ports_down && (i+portdown_port)<num_ports_per_pipe; ++i) {
            int port_n = portdown_port + i;
            // Notify PktGen of port down
            // XXX: WIP: only EVEN ports; otherwise any port
            if (!RmtObject::is_chip1() || ((port_n % 2) == 0)) {
              PktGenTab[portdown_pipe]->handle_port_down( Port::make_port_index(portdown_pipe,port_n) );
            }
          }
        }

        Packet *pkt_in = nullptr;

        if (packet < num_packets_per_iteration) {
          // Make a random egress packet (on Pipe0 could be a pkt from tbc/ethcpu)

          // Setup up first 16B of packet to never use bit 7 or bit 1
          // This means a randomly chosen addr_size from packet will never
          // exceed size of the PGEN PktBuf
          uint8_t hdrbuf[16];
          char    hdrbufstr[16+16+1];
          char   *hdrbufstrp = &hdrbufstr[0];
          for (int i = 0; i < 16; i++) {
            hdrbuf[i] = tu.xrand8(pkt_rand,5050+i) & 0x7D;
            hdrbufstrp += sprintf(hdrbufstrp, "%02x", hdrbuf[i]);
          }

          int pkt_in_size = tu.xrandrange(pkt_rand, 5101, min_packet_size, max_packet_size);
          uint16_t pkt_in_len = static_cast<uint16_t>(pkt_in_size);
          bool     ingress = tu.xrandbool(pkt_rand, 5102);
          pkt_in = tu.packet_make(pkt_rand, pkt_in_len, hdrbuf, 16);  // CREATE PACKET
          RMT_ASSERT(pkt_in != nullptr);
          if (pkt_in != nullptr)  { pkts_cnt++; }
          if (ingress) pkt_in->set_ingress(); else pkt_in->set_egress();
          uint32_t pkt_in_hash = pkt_in->hash();

          // To a random pipe and port (but not a port that's down)
          int pipe_in = tu.xrandrange(pkt_rand, 5103, 0, num_pipes-1);
          int perpipe_port_in = -1;
          for (int i = 1; i <= 100; i++) {
            perpipe_port_in = tu.xrandrange(pkt_rand, 5200+i, 0, num_ports_per_pipe-1);
            if (!PortsDownTab[pipe_in].bit_set(perpipe_port_in)) break;
          }
          RMT_ASSERT(perpipe_port_in >= 0);
          uint16_t port_in = Port::make_port_index(pipe_in, perpipe_port_in);
          Port *port_obj = om->port_lookup(port_in);
          RMT_ASSERT(port_obj != nullptr);
          pkt_in->set_port(port_obj);

          // Work out whether we expect recirc - only on egress - so port_out really
          bool expect_recirc = (ingress) ?false :AppMgrTab[pipe_in]->port_for_recirc(port_in);
          // Work out whether we may get deparse app match
          bool maybe_dprsr = ingress;

          bool found_full_fifo = false;
          uint16_t matches = 0;
          // Figure out whether we have an APP that'll trigger
          int ebuf = AppMgrTab[pipe_in]->port_get_ebuf(port_in);
          for (int app = 0; app < PktgenRandAppMgr::kNumAppsPerPipe; app++) {
            PktgenRandApp *papp = AppMgrTab[pipe_in]->get_app(app);
            if ( ( expect_recirc && papp->recirc_triggered(ebuf) ) ||
                 ( maybe_dprsr   && papp->dprsr_triggered() ) ) {
              if (papp->pkt_matches(pkt_in)) {
                matches |= ( 1 << app );
                // See if this app has a full input fifo
                if (papp->fifo_full())
                  found_full_fifo = true;
              }
            }
          }

          // DONT inject packet if some app has a full input fifo
          if (!found_full_fifo) {

            for (int app = 0; app < PktgenRandAppMgr::kNumAppsPerPipe; app++) {
              PktgenRandApp *papp = AppMgrTab[pipe_in]->get_app(app);
              if (((matches >> app) & 1) == 1) {
                if (MATCH_DEBUG)
                  printf("Pipe=%d App=%d Hdr16=0x%s Expecting %s trigger!!!!!!!!!!\n",
                         pipe_in, app, hdrbufstr, maybe_dprsr?"dprsr":"recirc");
                pkts_exp_trigger++;
                papp->inc_n_triggers_exp();
              }
            }

            // Build a PktInfo for debug purposes
            PktInfo pi_in{ pkt_in, port_in, !expect_recirc,
                  static_cast<uint8_t>(pipe_in), matches };

            // Now process packet using PktGen for pipe_in
            if (maybe_dprsr) {
              PacketGenMetadata packet_gen_metadata{};
              // Copy the trigger from the start of the packet, this would normally come
              //  from the deparser. This means the test harness can treat this case in
              //  a similar way to recirc triggers which really do trigger on the start
              //  of the packet.
              uint8_t buf[16]{};
              pkt_in->get_buf( buf, 0, 16 );
              auto pb = new PacketBuffer( buf, 16 );
              packet_gen_metadata.set_trigger( pb );
              // Deparser triggered, this info has to match the values in pktgen_util.cpp
              //  it would be better if these were randomised there and retrieved here
              packet_gen_metadata.set_address( 17 ); // TODO retrieve randomized value
              packet_gen_metadata.set_length( 67 );  // TODO retrieve randomized value
              PktGenTab[pipe_in]->maybe_trigger(&packet_gen_metadata);
            }
            //if (maybe_dprsr) pkts_in_trigger++;
            bool recirc = (ingress) ?false :PktGenTab[pipe_in]->maybe_recirculate(&pkt_in);
            if (recirc) pkts_in_recirc++;
            if (pkt_in == nullptr)  { pkts_cnt--; } // PGEN taking care of packet now

            pi_in.set_num((iteration * iteration_mult) + packet);
            pi_in.set_pgen(!recirc);
            if (PKT_IN_DEBUG) pi_in.print(recirc?"RCR":"PGN");
            EXPECT_EQ(expect_recirc, recirc);

            // If recirc add a RECIRC_PENDING pending packet
            if (recirc) Test_Pkt_Q->add_pending_packet(RECIRC_PENDING,
                                                       port_in, pkt_in_len, pkt_in_hash);
            packet++;

          } // if (!found_full_fifo)


        } // if (packet < num_packets_per_iteration)
        bool all_pkts_input = (packet >= num_packets_per_iteration);


        // Loop dequeueing packets - just drain packet Q - no wait
        int pkts_drained = 0;
        PktInfo *pi = Test_Pkt_Q->get_pktinfo();

        while (pi != nullptr) {
          Packet *pkt_rx = pi->pkt();
          bool recirc = !pi->pgen();

          if (recirc) {
            if (CHECK_DEBUG)
              printf("Pipe=%d Checking recirculated packet !!!!!!!!!!\n", pi->src_pipe());
            // Packet SHOULD NOT be an egress packet
            EXPECT_FALSE(pi->pkt_is_egress());
            // Port of packet SHOULD be a recirc port
            EXPECT_TRUE( AppMgrTab[pi->src_pipe()]->port_for_recirc(pi->port()) );
            // If we dequeued a recirc packet remove it from RECIRC_PENDING
            Test_Pkt_Q->remove_pending_packet(RECIRC_PENDING, pi);
            pkts_out_recirc++;
            // Increment drained
            pkts_drained++;

          } else {
            if (CHECK_DEBUG)
              printf("Pipe=%d Checking generated packet !!!!!!!!!!\n", pi->src_pipe());
            bool periodic = false;
            int result = AppMgrTab[pi->src_pipe()]->check_app_generated_pkt(pkt_rx, pi,
                                                                            &periodic);
            if ((result != 0) && (CHECK_MISMATCH_DEBUG))
              printf("Pipe=%d Error %d checking generated packet !!!!!!!!!!\n",
                     pi->src_pipe(), result);

            if (result == 0) { pkts_ok_trigger++;                              }
            if (result != 0) { pkts_err_trigger++;  last_trigger_err = result; }
            // Increment drained but only if not timer-generated packet
            // (don't count those as they just go on for ever)
            if (!periodic) pkts_drained++;

            pkts_out_trigger++;
            //EXPECT_EQ(0, result);
          }
          if (PKT_COUNT_DEBUG) {
            printf("PKT_COUNTS: Cnt=%d Trigger(In=%d,Out=%d,OK=%d,%s=%d <%d>) "
                   "Recirc=(In=%d,Out=%d)\n",
                   pkts_cnt, pkts_in_trigger, pkts_out_trigger, pkts_ok_trigger,
                   (pkts_err_trigger > 0) ?"ERR":"Err", pkts_err_trigger,
                   last_trigger_err, pkts_in_recirc, pkts_out_recirc);
          }

          // Free up
          Test_Pkt_Q->free_pktinfo(pi);
          if (pkt_rx != nullptr) tu.packet_free(pkt_rx);

          // Just drain packet Q - no wait!
          pi = Test_Pkt_Q->get_pktinfo();

        } // while (pi != nullptr)


        // Free input packet if there was one and we still have ref to it
        if (pkt_in != nullptr)  { tu.packet_free(pkt_in); pkts_cnt--; }

        if (all_pkts_input) {
          // We exit loop if all pkts done and we didn't drain any packets on last 9 goes
          if (pkts_drained == 0) pkts_drained_zero_cnt++; else pkts_drained_zero_cnt = 0;
          if (ITER_DEBUG) printf("DRAINING final packets......%d\n",pkts_drained_zero_cnt);
          // If still draining we'll loop again
          if (pkts_drained_zero_cnt >= 9) break;
          // Do long waits whilst still draining
          Test_Pkt_Q->wait(long_wait);
        } else {
          // Still inputting packets - just a short wait
          Test_Pkt_Q->wait(pkt_wait);
        }

      } // while (true)


      if (ITER_DEBUG) printf("Exited loop\n");

      // First off disable all apps so no more packets generated
      for (int p = 0; p < num_pipes; p++) AppMgrTab[p]->disable_apps();

      // Short wait then drain any packets still hanging around
      Test_Pkt_Q->wait(pkt_wait);
      PktInfo *pi = Test_Pkt_Q->get_pktinfo();
      while (pi != nullptr) {
        Packet *pkt_tmp = pi->pkt();
        if (pkt_tmp != nullptr) tu.packet_free(pkt_tmp);
        Test_Pkt_Q->free_pktinfo(pi);
        pi = Test_Pkt_Q->get_pktinfo();
      }


      // ITERATION END checks

      // 1. Should be 0 RECIRC_PENDING at end
      EXPECT_EQ(0, Test_Pkt_Q->count_pending_packets(RECIRC_PENDING));

      // 2. Should be 0 pkts_err_trigger at end
      EXPECT_EQ(0, pkts_err_trigger);

      // 3. Number of actual triggers of each app in each pipe should match expected
      int mismatches = 0;;
      for (int p = 0; p < num_pipes; p++) {
        for (int app = 0; app < PktgenRandAppMgr::kNumAppsPerPipe; app++) {
          PktgenRandApp *papp = AppMgrTab[p]->get_app(app);
          uint8_t  typ = papp->type();
          uint64_t act = papp->n_triggers_act();
          uint64_t exp = papp->n_triggers_exp();
          // In case of timers calculate expected triggers using current time
          if ((typ == 0) || (typ == 1))
            exp = papp->n_timeouts_expected();

          //EXPECT_EQ(act, exp);
          if (papp->enabled()) {
            if (act != exp) {
              mismatches++;
              const char *types[6] = { "Timer", "Periodic", "Linkdown",
                                       "Recirc", "Dprsr", "PFC" };
              printf("Pipe=%d App=%2d Type=%8s  "
                     "TrigExp=%3" PRId64 " TrigAct=%3" PRId64 " "
                     "MISMATCH_ERR!!  (%d,%d)\n",
                     p, app, types[typ], exp, act, p, app);
            }
          }
        }
      }

      // 99. Finally free up all AppMgr objects
      for (int p = 0; p < num_pipes; p++) {
        if (AppMgrTab[p] != nullptr) delete AppMgrTab[p];
        AppMgrTab[p] = nullptr;
      }

      // Print summary status for iteration
      printf("Iteration[%d]: Seed=%16" PRIx64 " Trigger(In=%d,Out=%d,OK=%d,%s=%d <%d>)"
             " Recirc=(In=%d,Out=%d) Mismatch=%d PortDown=%d Cnts(Pkt=%d,PI=%d)\n", iteration, iteration_seed,
             pkts_in_trigger, pkts_out_trigger, pkts_ok_trigger,
             (pkts_err_trigger > 0) ?"ERR":"Err", pkts_err_trigger,
             last_trigger_err, pkts_in_recirc, pkts_out_recirc, mismatches, pkts_exp_portdown,
             pkts_cnt, Test_Pkt_Q->cnt_pktinfo());
      if (MEM_DEBUG) om->dump_stats();

      EXPECT_EQ(0, mismatches);
      //if (mismatches > 0) break;


    } // for (int iteration = 0; iteration < num_iterations; iteration++)


    tu.quieten_log_flags();

  } // TEST(BFN_TEST_NAME(PktGen), Stress)

}
