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

extern std::unique_ptr<model_core::Model> GLOBAL_MODEL;


namespace MODEL_CHIP_TEST_NAMESPACE {

    using namespace std;
    using namespace MODEL_CHIP_NAMESPACE;

    static constexpr uint32_t DEF_TMO = 500;
    static constexpr uint32_t MAX_CLK = 1500;
    static constexpr uint8_t NAPPS = 8;
    static constexpr uint8_t MAX_BATCHES = 5;
    static constexpr uint8_t MAX_PKTS = 6;
    static constexpr uint8_t MAX_IBG = 20;
    static constexpr uint8_t MAX_IPG = 5;
    static constexpr uint8_t EV_DELAY = 5;
    static constexpr uint16_t APP_INGR_PORT = 22;
    static constexpr uint16_t APP_INGR_PORT_INC = true;
    static constexpr uint32_t RECIR_MATCH_VAL = 0x12345678;
    static constexpr uint32_t RECIR_MATCH_MASK = 0x0;
    static constexpr uint8_t RECIR_PKT_DATA[4] = {0x12, 0x34, 0x56, 0x78};
    static constexpr uint32_t IBG_JITTER_VAL = 0xFFFFFFFC;
    static constexpr uint32_t IBG_JITTER_MASK = ~0xF;
    static constexpr uint32_t IPG_JITTER_VAL = 0xFFFFFFFD;
    static constexpr uint32_t IPG_JITTER_MASK = ~0xF;
    static constexpr uint64_t IGN_VAL = 1000;

    void model_tick(uint64_t n_clocks, uint64_t& a, const uint64_t b = IGN_VAL) {
		uint64_t clock = 0;
		while ((clock < n_clocks) && (a < b)) {
			++clock;
			model_timer::ModelTimerIncrement(1);
			std::this_thread::sleep_for(std::chrono::microseconds(DEF_TMO));
		}
        if (b != IGN_VAL) {
            cout << "CLKS_DONE: " << clock << ":COUNTER_VAL: " << a << ":EXPECTED_VAL: " << b << endl;
        }
    }

    void en_app(bool* en, int app_id, bool val) {
        if (app_id == NAPPS) {
            for (int i = 0; i < NAPPS; i ++) {
                en[i] = val;
            }
        } else if (app_id < NAPPS) {
            en[app_id] = val;
        }

    }

    uint64_t pgen_app_setup(bool* en,
                            app_config* p,
                            uint32_t* n_clks,
                            uint8_t app_type,
                            bool do_jitter=false,
                            uint16_t app_ingr_port = APP_INGR_PORT,
                            bool ingr_port_inc = APP_INGR_PORT_INC,
                            uint32_t recir_match_val = RECIR_MATCH_VAL,
                            uint32_t recir_match_mask = RECIR_MATCH_MASK) {
        uint64_t n_pkts = 0;
        *n_clks = 0;
        uint32_t prev_pkt_time = 0;
        uint32_t pkt_time = 0;
        srand(time(NULL));
		for (uint8_t i = 0; i < NAPPS; i ++) {
			p[i].app_type = app_type;
			p[i].app_ingr_port = app_ingr_port;
			p[i].app_ingr_port_inc = ingr_port_inc;
			p[i].batch_num = rand()%MAX_BATCHES;
			p[i].packet_num = rand()%MAX_PKTS;
			p[i].ibg_count = rand()%MAX_IBG;
			p[i].ipg_count = rand()%MAX_IPG;
			p[i].event_timer = EV_DELAY;
			p[i].recir_match_value = recir_match_val;
			p[i].recir_match_mask = recir_match_mask;
            if (do_jitter) {
			    p[i].event_ibg_jitter_value = IBG_JITTER_VAL;
    			p[i].event_ibg_jitter_mask = IBG_JITTER_MASK;
	    		p[i].event_ipg_jitter_value = IPG_JITTER_VAL;
		    	p[i].event_ipg_jitter_mask = IPG_JITTER_MASK;


            }
			if (en[i]) {
                n_pkts += ((p[i].batch_num + 1)*(p[i].packet_num + 1));
                pkt_time = (p[i].packet_num * p[i].ipg_count);
                if (pkt_time < p[i].ibg_count) pkt_time = p[i].ibg_count;
                if (pkt_time > prev_pkt_time) {
                    prev_pkt_time = pkt_time;
                    *n_clks = pkt_time;
                    if (do_jitter) *n_clks += MAX_CLK;
                }
            }

		}
        *n_clks += MAX_CLK;
        return n_pkts;
    }

    void verify_counters(app_config* p, bool* en, TestUtil::pgr_app_counter* ctr, uint16_t trig_cnt) {
        uint64_t n_triggers = 0;
        uint64_t n_batches = 0;
        uint32_t n_pkts = 0;

        for (uint8_t i = 0; i < NAPPS; i ++) {
            if (en[i]) {
                EXPECT_EQ(ctr[i].trig_cnt, static_cast<uint64_t>(trig_cnt));
                EXPECT_EQ(ctr[i].batch_cnt, static_cast<uint64_t>(p[i].batch_num + 1));
                EXPECT_EQ(ctr[i].pkt_cnt, static_cast<uint64_t>((p[i].packet_num + 1)*(p[i].batch_num + 1)));

            }
            n_triggers += ctr[i].trig_cnt;
            n_batches += ctr[i].batch_cnt;
            n_pkts += ctr[i].pkt_cnt;
        }
        cout << "NTRIGGERS: " << n_triggers << ":NBATCHES:" << n_batches << ":NPKTS:" << n_pkts << endl;

    }


	void add_pkt_meta(TestUtil* tu, Packet*& pkt, int ingress_port, int egress_port) {

		assert(pkt != nullptr);
		assert(tu != NULL);

        // working out the parser number won't work for any other value! TODO: fix!
        RMT_ASSERT( tu->get_pipe() == 0 );
		int prsr = ((egress_port)/(RmtDefs::kChlsPerIpb))/((tu->get_pipe() + 1));
		bool is_recir = (prsr == RmtDefs::kPktGen_P16 || prsr == RmtDefs::kPktGen_P17);

		RmtObjectManager* rmt = tu->get_objmgr();
		PktGen* pgen = nullptr;

		if (is_recir) {
			pgen = tu->get_objmgr()->pktgen_lookup( tu->get_pipe() );
			assert(pgen != NULL);
		}

		Port* ig_port = rmt->port_get(ingress_port);

		RmtPacketCoordinator* rc = rmt->packet_coordinator_get();
		rc->set_tx_fn(tr_packet);
		install_fake_pipe_process_fn(rmt, rc);
		pkt->set_ingress();
		pkt->set_port(ig_port);
		pkt->i2qing_metadata()->set_physical_ingress_port(ingress_port);
		pkt->i2qing_metadata()->set_egress_unicast_port(egress_port);

		if (pgen) {
			pgen->set_test(true);
			pkt->i2qing_metadata()->set_egress_unicast_port(egress_port);
		}
		pkt->set_metadata_added(true);
	}


	RmtPacketCoordinator* pgen_setup(TestUtil* tu,
									 int pgen_port,
									 int channel,
									 uint32_t n_pkts,
									 PktGen*& pgen) {

		RmtObjectManager *rmt = tu->get_objmgr();
		RmtPacketCoordinator* rc = rmt->packet_coordinator_get();

		int egress_port = pgen_port*RmtDefs::kParserChannels + channel;
		int ingress_port = 0;

        pgen = tu->get_objmgr()->pktgen_lookup( tu->get_pipe() );
		assert(pgen != NULL);
		pgen->set_test(true);
        install_fake_pipe_process_fn(rmt, rc);

		Port* ing_port = rmt->port_get(ingress_port);

		while (n_pkts) {
			Packet* pkt = rmt->pkt_create();
			rc->set_tx_fn(tr_packet);
			pkt->set_ingress();
			pkt->set_port(ing_port);
			pkt->i2qing_metadata()->set_physical_ingress_port(0);
			pkt->i2qing_metadata()->set_egress_unicast_port(egress_port);
			pkt->set_metadata_added(true);
			rc->enqueue(ingress_port, pkt, false);
			--n_pkts;
		}
		return rc;
	}



	TEST(BFN_TEST_NAME(PktGen), P16_Drop) {
		TestUtil *tu = new TestUtil(GLOBAL_MODEL.get(), 202, 0, 0);
		PktGen* pgen = nullptr;
		uint32_t n_pkts = 2;
		uint32_t iter = 0;
		RmtPacketCoordinator* rc = nullptr;
		assert(tu != NULL);
		rc = pgen_setup(tu, RmtDefs::kPktGen_P16, 0, n_pkts, pgen);
		rc->start();
		while ((pgen->g_drop_cnt_ != n_pkts) && (iter < MAX_CLK)) {
			std::this_thread::sleep_for(std::chrono::microseconds(DEF_TMO * 4));
			iter ++;
		}
		EXPECT_EQ(n_pkts, pgen->g_drop_cnt_);
		rc->stop();

	}
	TEST(BFN_TEST_NAME(PktGen), P16_mxbar) {

		TestUtil *tu = new TestUtil(GLOBAL_MODEL.get(), 202, 0, 0);
		PktGen* pgen = nullptr;
		uint32_t n_pkts = 2;
		uint32_t iter = 0;
		RmtPacketCoordinator* rc = nullptr;
		assert(tu != NULL);
		rc = pgen_setup(tu, RmtDefs::kPktGen_P16, 0, n_pkts, pgen);
 		tu->pktgen_mxbar_set(0, true, (RmtDefs::kPktGen_P16 * RmtDefs::kParserChannels));

		rc->start();
		while ((pgen->g_mxbar_cnt_ != n_pkts) && (iter < MAX_CLK)) {
			std::this_thread::sleep_for(std::chrono::microseconds(DEF_TMO * 4));
			++iter;
		}
		EXPECT_EQ(n_pkts, pgen->g_mxbar_cnt_);
		rc->stop();
	}
	TEST(BFN_TEST_NAME(PktGen), P16_recirc) {
		TestUtil *tu = new TestUtil(GLOBAL_MODEL.get(), 202, 0, 0);
		PktGen* pgen = nullptr;
		RmtPacketCoordinator* rc = nullptr;
		uint32_t n_pkts = 2;
		uint32_t iter = 0;

		assert(tu != NULL);
		rc = pgen_setup(tu, RmtDefs::kPktGen_P16, 0, n_pkts, pgen);

 		tu->pktgen_recirc_set(0, true, RmtDefs::kPktGen_P16 * RmtDefs::kParserChannels);
		rc->start();
		while ( (pgen->g_recirc_cnt_ != (n_pkts * pgen->get_def_recir_count())) &&
				( iter < MAX_CLK )) {
			std::this_thread::sleep_for(std::chrono::microseconds(DEF_TMO * 4));
			iter ++;
		}

		tu->pktgen_recirc_set(0, false, RmtDefs::kPktGen_P16 * RmtDefs::kParserChannels);
		EXPECT_EQ(pgen->get_def_recir_count() * n_pkts, pgen->g_recirc_cnt_);
		rc->stop();
	}
	TEST(BFN_TEST_NAME(PktGen), P17_Drop) {

		TestUtil *tu = new TestUtil(GLOBAL_MODEL.get(), 202, 0, 0);
		PktGen* pgen = nullptr;
		uint32_t n_pkts = 2;
		uint32_t iter = 0;
		RmtPacketCoordinator* rc = nullptr;
		assert(tu != NULL);
		rc = pgen_setup(tu, RmtDefs::kPktGen_P17, 0, n_pkts, pgen);
		rc->start();
		while ((pgen->g_drop_cnt_ != n_pkts) &&
			    (iter < MAX_CLK)) {
			std::this_thread::sleep_for(std::chrono::microseconds(DEF_TMO * 4));
			iter ++;
		}

		EXPECT_EQ(n_pkts, pgen->g_drop_cnt_);
		rc->stop();

	}
	TEST(BFN_TEST_NAME(PktGen), P17_recirc) {
		TestUtil *tu = new TestUtil(GLOBAL_MODEL.get(), 202, 0, 0);
		PktGen* pgen = nullptr;
		RmtPacketCoordinator* rc = nullptr;
		uint32_t n_pkts = 2;
		uint32_t iter = 0;

		assert(tu != NULL);
		rc = pgen_setup(tu, RmtDefs::kPktGen_P17, 0, n_pkts, pgen);

 		tu->pktgen_recirc_set(0, true, RmtDefs::kPktGen_P17 * RmtDefs::kParserChannels);
		rc->start();
		while ((pgen->g_recirc_cnt_ != (n_pkts * pgen->get_def_recir_count())) &&
			   (iter < MAX_CLK)) {
			std::this_thread::sleep_for(std::chrono::microseconds(DEF_TMO * 4));
			iter ++;
		}

		tu->pktgen_recirc_set(0, false, RmtDefs::kPktGen_P17 * RmtDefs::kParserChannels);
		EXPECT_EQ(pgen->get_def_recir_count() * n_pkts, pgen->g_recirc_cnt_);
		rc->stop();
	}
	TEST(BFN_TEST_NAME(PktGen), P17_pgen_onetime_1) {
		TestUtil *tu = new TestUtil(GLOBAL_MODEL.get(), 202, 0, 0);
		PktGen* pgen = nullptr;
		RmtPacketCoordinator* rc = nullptr;
		assert(tu != NULL);
        uint32_t n_pkts = 0;
        uint32_t n_clks = 0;
		rc = pgen_setup(tu, RmtDefs::kPktGen_P17, 0, n_pkts, pgen);

		bool en[NAPPS];
		app_config p[NAPPS];
        en_app(en, NAPPS, false);
        en_app(en, 0, true);

        n_pkts = pgen_app_setup(en, p, &n_clks, TRIGGER_TYPE_E::ONETIME);
 		tu->pktgen_pgen_set(0, RmtDefs::kPktGen_P17 * RmtDefs::kParserChannels, p, en, NAPPS, false);
		rc->start();

        n_clks *= 2;
        model_tick(n_clks, pgen->g_pgen_cnt_, n_pkts);

        //TestUtil::pgr_app_counter ctr[NAPPS];
        //tu->pktgen_get_counters(0, en, ctr);

        en_app(en, 0, false);
 		tu->pktgen_pgen_set(0, RmtDefs::kPktGen_P17 * RmtDefs::kParserChannels, p, en, NAPPS, false);


        en_app(en, 0, true);
        //verify_counters(en, ctr, 1);

        EXPECT_EQ(pgen->g_triggers_, 1u);
        EXPECT_EQ(pgen->g_pgen_cnt_, n_pkts);
        rc->stop();
	}

	TEST(BFN_TEST_NAME(PktGen), P17_pgen_onetime_8) {
		TestUtil *tu = new TestUtil(GLOBAL_MODEL.get(), 202, 0, 0);
		PktGen* pgen = nullptr;
		RmtPacketCoordinator* rc = nullptr;
		uint32_t n_pkts = 0;
        uint32_t n_clks = 0;
        //uint32_t curr_clks = 0;
		assert(tu != NULL);
		rc = pgen_setup(tu, RmtDefs::kPktGen_P17, 0, n_pkts, pgen);

		app_config p[NAPPS];
		bool en[NAPPS];

        en_app(en, NAPPS, true);
        n_pkts = pgen_app_setup(en, p, &n_clks, TRIGGER_TYPE_E::ONETIME);
 		tu->pktgen_pgen_set(0, RmtDefs::kPktGen_P17 * RmtDefs::kParserChannels, p, en, NAPPS, false);
		rc->start();

        n_clks *= NAPPS;
        model_tick(n_clks, pgen->g_pgen_cnt_, n_pkts);

        en_app(en, NAPPS, false);
 		tu->pktgen_pgen_set(0, RmtDefs::kPktGen_P17 * RmtDefs::kParserChannels, p, en, NAPPS, false);

        //TestUtil::pgr_app_counter ctr[NAPPS];
        //tu->pktgen_get_counters(0, en, ctr);

        en_app(en, NAPPS, true);
        //verify_counters(p, en, ctr, NAPPS);

		EXPECT_EQ(pgen->g_triggers_, NAPPS);
		EXPECT_EQ(pgen->g_pgen_cnt_, n_pkts);
		rc->stop();
	}
	TEST(BFN_TEST_NAME(PktGen), P17_pgen_periodic_1) {
		TestUtil *tu = new TestUtil(GLOBAL_MODEL.get(), 202, 0, 0);
		PktGen* pgen = nullptr;
		RmtPacketCoordinator* rc = nullptr;
		uint32_t n_pkts = 0;
        uint32_t n_clks = 0;
		assert(tu != NULL);
		rc = pgen_setup(tu, RmtDefs::kPktGen_P17, 0, n_pkts, pgen);

		bool en[NAPPS];
		app_config p[NAPPS];
        en_app(en, NAPPS, false);
        en_app(en, 5, true);
        n_pkts = pgen_app_setup(en, p, &n_clks, TRIGGER_TYPE_E::PERIODIC);
 		tu->pktgen_pgen_set(0, RmtDefs::kPktGen_P17 * RmtDefs::kParserChannels, p, en, NAPPS, false);
		rc->start();
        model_tick(n_clks, pgen->g_pgen_cnt_);
	//uint64_t pgen_count = pgen->g_pgen_cnt_;
        en_app(en, 5, false);
 		tu->pktgen_pgen_set(0, RmtDefs::kPktGen_P17 * RmtDefs::kParserChannels, p, en, NAPPS, false);

        model_tick(MAX_CLK, pgen->g_pgen_cnt_);

        //TestUtil::pgr_app_counter ctr[NAPPS];
        //tu->pktgen_get_counters(0, en, ctr);

        //verify_counters(en, ctr, 0);
		cout << "NTRIGGERS: " << pgen->g_triggers_ << endl;
		cout << "PGEN_ACTUAL_COUNT:" << pgen->g_pgen_cnt_ << endl;

		rc->stop();
	}
	TEST(BFN_TEST_NAME(PktGen), P17_pgen_periodic_8) {
		TestUtil *tu = new TestUtil(GLOBAL_MODEL.get(), 202, 0, 0);
		PktGen* pgen = nullptr;
		RmtPacketCoordinator* rc = nullptr;
		uint32_t n_pkts = 0;
        uint32_t n_clks = 0;
		assert(tu != NULL);
		rc = pgen_setup(tu, RmtDefs::kPktGen_P17, 0, n_pkts, pgen);

		bool en[NAPPS];
		app_config p[NAPPS];
        en_app(en, NAPPS, true);
        n_pkts = pgen_app_setup(en, p, &n_clks, TRIGGER_TYPE_E::PERIODIC);
 		tu->pktgen_pgen_set(0, RmtDefs::kPktGen_P17 * RmtDefs::kParserChannels, p, en, NAPPS, false);
		rc->start();
        model_tick(n_clks, pgen->g_pgen_cnt_);

	    en_app(en, NAPPS, false);
 		tu->pktgen_pgen_set(0, RmtDefs::kPktGen_P17 * RmtDefs::kParserChannels, p, en, NAPPS, false);
        //uint64_t pgen_count = pgen->g_pgen_cnt_;
        model_tick(MAX_CLK, pgen->g_pgen_cnt_);

        //TestUtil::pgr_app_counter ctr[NAPPS];
        //tu->pktgen_get_counters(0, en, ctr);

        //verify_counters(en, ctr, 0);

		cout << "NTRIGGERS: " << pgen->g_triggers_ << endl;
		cout << "PGEN_COUNT:" << pgen->g_pgen_cnt_ << endl;

		rc->stop();
	}
	TEST(BFN_TEST_NAME(PktGen), P17_pgen_pdown_1) {
		TestUtil *tu = new TestUtil(GLOBAL_MODEL.get(), 202, 0, 0);
		PktGen* pgen = nullptr;
		RmtPacketCoordinator* rc = nullptr;
		uint32_t n_pkts = 0;
		uint32_t n_clks = 0;
		//uint32_t curr_clks = 0;
        uint8_t lst_port[] = { 2, 11, 22, 25 };
        uint32_t n_ports = sizeof(lst_port)/sizeof(uint8_t);

		assert(tu != NULL);
		rc = pgen_setup(tu, RmtDefs::kPktGen_P17, 0, n_pkts, pgen);

		bool en[NAPPS];
		app_config p[NAPPS];
        en_app(en, NAPPS, false);
        en_app(en, 4, true);

        n_pkts = pgen_app_setup(en, p, &n_clks, TRIGGER_TYPE_E::PORT_DOWN);

        tu->pktgen_pgen_set(0, RmtDefs::kPktGen_P17 * RmtDefs::kParserChannels, p, en, NAPPS, false);
		rc->start();
		// Set a port down event
        // This will not trigger
        for (uint32_t i = 0; i < n_ports; i ++) {
		    pgen->handle_port_down(lst_port[i]);
        }


	    model_tick(n_clks, pgen->g_pgen_cnt_);

        EXPECT_EQ(pgen->g_triggers_, 0u);
        EXPECT_EQ(pgen->g_pgen_cnt_, 0u);

        // Enable port downs on the list of ports
        for (uint32_t i = 0; i < n_ports; i ++) {
		    tu->pktgen_portdown_en(0, lst_port[i]);
        }
        // Now actually do the port downs
        for (uint32_t i = 0; i < n_ports; i ++) {
		    pgen->handle_port_down(lst_port[i]);
        }
        n_clks *= n_ports;
        model_tick(n_clks, pgen->g_pgen_cnt_, n_pkts*n_ports);

        //TestUtil::pgr_app_counter ctr[NAPPS];
        //tu->pktgen_get_counters(0, en, ctr);

        //verify_counters(en, ctr, n_ports);

		EXPECT_EQ(pgen->g_triggers_, n_ports);
		EXPECT_EQ(pgen->g_pgen_cnt_, n_pkts*n_ports);
		rc->stop();
	}
	TEST(BFN_TEST_NAME(PktGen), P17_pgen_pdown_8) {
		TestUtil *tu = new TestUtil(GLOBAL_MODEL.get(), 202, 0, 0);
		PktGen* pgen = nullptr;
		RmtPacketCoordinator* rc = nullptr;
		uint32_t n_pkts = 0;
        uint32_t n_clks = 0;
        //uint32_t curr_clks = 0;
        uint8_t lst_port[] = { 2, 9, 11, 13, 22 };
        uint32_t n_ports = sizeof(lst_port)/sizeof(uint8_t);

        assert(tu != NULL);
		rc = pgen_setup(tu, RmtDefs::kPktGen_P17, 0, n_pkts, pgen);

		bool en[NAPPS];
		app_config p[NAPPS];
        en_app(en, NAPPS, true);
        n_pkts = pgen_app_setup(en, p, &n_clks, TRIGGER_TYPE_E::PORT_DOWN);

 		tu->pktgen_pgen_set(0, RmtDefs::kPktGen_P17 * RmtDefs::kParserChannels, p, en, NAPPS, false);
		rc->start();

        // Will not trigger, portdown is not enabled on these ports
        for (uint32_t i = 0; i < n_ports; i ++) {
		    pgen->handle_port_down(lst_port[i]);
        }
        model_tick(n_clks, pgen->g_pgen_cnt_);

        EXPECT_EQ(pgen->g_triggers_, 0u);
        EXPECT_EQ(pgen->g_pgen_cnt_, 0u);

        // Enable port downs on the list of ports
        for (uint32_t i = 0; i < n_ports; i ++) {
		    tu->pktgen_portdown_en(0, lst_port[i]);
        }
        // Now actually do the port downs
        for (uint32_t i = 0; i < n_ports; i ++) {
		    pgen->handle_port_down(lst_port[i]);
        }

        n_clks *= n_ports;
        n_clks *= NAPPS;
        model_tick(n_clks, pgen->g_pgen_cnt_, n_ports*n_pkts);

        //TestUtil::pgr_app_counter ctr[NAPPS];
        //tu->pktgen_get_counters(0, en, ctr);
        //verify_counters(en, ctr, n_ports);
		EXPECT_EQ(pgen->g_triggers_, n_ports*NAPPS);
        EXPECT_EQ(pgen->g_pgen_cnt_, n_ports*n_pkts);


		rc->stop();
	}
	TEST(BFN_TEST_NAME(PktGen), P17_pgen_pdown_nohandle) {
		TestUtil *tu = new TestUtil(GLOBAL_MODEL.get(), 202, 0, 0);
		PktGen* pgen = nullptr;
		RmtPacketCoordinator* rc = nullptr;
		uint32_t n_pkts = 0;
        uint32_t n_clks = 0;
		assert(tu != NULL);
		rc = pgen_setup(tu, RmtDefs::kPktGen_P17, 0, n_pkts, pgen);

		bool en[NAPPS];
		app_config p[NAPPS];
        en_app(en, NAPPS, true);
        n_pkts = pgen_app_setup(en, p, &n_clks, TRIGGER_TYPE_E::PORT_DOWN);

 		tu->pktgen_pgen_set(0, RmtDefs::kPktGen_P17 * RmtDefs::kParserChannels, p, en, NAPPS, false);
		rc->start();

		// Set a port down event that is not on my pipe
		pgen->handle_port_down(155);

        model_tick(n_clks, pgen->g_pgen_cnt_);

		EXPECT_EQ(pgen->g_pgen_cnt_, 0u);

        en_app(en, NAPPS, false);
 		tu->pktgen_pgen_set(0, RmtDefs::kPktGen_P17 * RmtDefs::kParserChannels, p, en, NAPPS, false);

        en_app(en, NAPPS, true);
        //TestUtil::pgr_app_counter ctr[NAPPS];
        //tu->pktgen_get_counters(0, en, ctr);
        //verify_counters(en, ctr, 0);

		rc->stop();
	}

	TEST(BFN_TEST_NAME(PktGen), P17_pgen_recirc_1) {
		TestUtil *tu = new TestUtil(GLOBAL_MODEL.get(), 202, 0, 0);
		PktGen* pgen = nullptr;
		RmtPacketCoordinator* rc = nullptr;
		uint32_t n_pkts = 0;
		uint32_t n_clks = 0;
		assert(tu != NULL);

		rc = pgen_setup(tu, RmtDefs::kPktGen_P17, 0, n_pkts, pgen);


		Packet* pkt1 = tu->get_objmgr()->pkt_create(RECIR_PKT_DATA, sizeof(RECIR_PKT_DATA));

		ASSERT_TRUE(pkt1 != NULL);
		add_pkt_meta(tu, pkt1, 0, RmtDefs::kPktGen_P17*RmtDefs::kChlsPerIpb);

		bool en[NAPPS];
		app_config p[NAPPS];
        en_app(en, NAPPS, false);
		en_app(en, 2, true);
        n_pkts = pgen_app_setup(en, p, &n_clks, TRIGGER_TYPE_E::PACKET_RECIRC);
		n_pkts *= pgen->get_def_recir_count();

 		tu->pktgen_recirc_set(0, true, RmtDefs::kPktGen_P17 * RmtDefs::kParserChannels);
 		tu->pktgen_pgen_set(0, RmtDefs::kPktGen_P17 * RmtDefs::kParserChannels, p, en, NAPPS, true);

		rc->enqueue(0, pkt1, false);
		rc->start();

        n_clks *= pgen->get_def_recir_count();

        model_tick(n_clks, pgen->g_pgen_cnt_, n_pkts);
        model_tick(n_clks, pgen->g_recirc_cnt_, pgen->get_def_recir_count());


        en_app(en, 2, false);
 		tu->pktgen_recirc_set(0, false, RmtDefs::kPktGen_P17 * RmtDefs::kParserChannels);
 		tu->pktgen_pgen_set(0, RmtDefs::kPktGen_P17 * RmtDefs::kParserChannels, p, en, NAPPS, false);

        en_app(en, 2, true);
        //TestUtil::pgr_app_counter ctr[NAPPS];
        //tu->pktgen_get_counters(0, en, ctr);
        //verify_counters(en, ctr, pgen->get_def_recir_count());


		EXPECT_EQ(pgen->g_recirc_cnt_, pgen->get_def_recir_count());
		EXPECT_EQ(pgen->g_pgen_cnt_, n_pkts);

		rc->stop();
	}
	TEST(BFN_TEST_NAME(PktGen), P17_pgen_recirc_8) {
		TestUtil *tu = new TestUtil(GLOBAL_MODEL.get(), 202, 0, 0);
		PktGen* pgen = nullptr;
		RmtPacketCoordinator* rc = nullptr;
		uint32_t n_pkts = 0;
		uint32_t n_clks = 0;
		assert(tu != NULL);
		rc = pgen_setup(tu, RmtDefs::kPktGen_P17, 0, n_pkts, pgen);


		Packet* pkt1 = tu->get_objmgr()->pkt_create(RECIR_PKT_DATA, sizeof(RECIR_PKT_DATA));

		ASSERT_TRUE(pkt1 != NULL);

		add_pkt_meta(tu, pkt1, 0, RmtDefs::kPktGen_P17*RmtDefs::kChlsPerIpb);


		bool en[NAPPS];
		app_config p[NAPPS];
        en_app(en, NAPPS, true);
        n_pkts = pgen_app_setup(en, p, &n_clks, TRIGGER_TYPE_E::PACKET_RECIRC);
		n_pkts *= pgen->get_def_recir_count();

 		tu->pktgen_recirc_set(0, true, RmtDefs::kPktGen_P17 * RmtDefs::kParserChannels);
 		tu->pktgen_pgen_set(0, RmtDefs::kPktGen_P17 * RmtDefs::kParserChannels, p, en, NAPPS, true);

		rc->enqueue(0, pkt1, false);
		rc->start();

        n_clks *= NAPPS;

        model_tick(n_clks, pgen->g_pgen_cnt_, n_pkts);
        model_tick(n_clks, pgen->g_recirc_cnt_, pgen->get_def_recir_count());

        en_app(en, NAPPS, false);
 		tu->pktgen_pgen_set(0, RmtDefs::kPktGen_P17 * RmtDefs::kParserChannels, p, en, NAPPS, false);
 		tu->pktgen_recirc_set(0, false, RmtDefs::kPktGen_P17 * RmtDefs::kParserChannels);

        en_app(en, NAPPS, true);
        //TestUtil::pgr_app_counter ctr[NAPPS];
        //tu->pktgen_get_counters(0, en, ctr);
        //verify_counters(en, ctr, pgen->get_def_recir_count());

		EXPECT_EQ(pgen->g_pgen_cnt_, n_pkts);
		EXPECT_EQ(pgen->get_def_recir_count(), pgen->g_recirc_cnt_);
		EXPECT_EQ(pgen->get_def_recir_count()*NAPPS, pgen->g_triggers_);
		rc->stop();
	}
	TEST(BFN_TEST_NAME(PktGen), P17_pgen_recirc_lfsr_8) {
		TestUtil *tu = new TestUtil(GLOBAL_MODEL.get(), 202, 0, 0);
		PktGen* pgen = nullptr;
		RmtPacketCoordinator* rc = nullptr;
		uint32_t n_pkts = 0;
		uint32_t n_clks = 0;
		assert(tu != NULL);
		rc = pgen_setup(tu, RmtDefs::kPktGen_P17, 0, n_pkts, pgen);


		Packet* pkt1 = tu->get_objmgr()->pkt_create(RECIR_PKT_DATA, sizeof(RECIR_PKT_DATA));

		ASSERT_TRUE(pkt1 != NULL);

		add_pkt_meta(tu, pkt1, 0, RmtDefs::kPktGen_P17*RmtDefs::kChlsPerIpb);


		bool en[NAPPS];
		app_config p[NAPPS];
        en_app(en, NAPPS, true);
        n_pkts = pgen_app_setup(en, p, &n_clks, TRIGGER_TYPE_E::PACKET_RECIRC, true);
		n_pkts *= pgen->get_def_recir_count();

 		tu->pktgen_recirc_set(0, true, RmtDefs::kPktGen_P17 * RmtDefs::kParserChannels);
 		tu->pktgen_pgen_set(0, RmtDefs::kPktGen_P17 * RmtDefs::kParserChannels, p, en, NAPPS, true);

		rc->enqueue(0, pkt1, false);
		rc->start();

        n_clks *= NAPPS;
        n_clks *= 2;

        model_tick(n_clks, pgen->g_pgen_cnt_, n_pkts);
        model_tick(n_clks, pgen->g_recirc_cnt_, pgen->get_def_recir_count());
        en_app(en, NAPPS, false);

 		tu->pktgen_pgen_set(0, RmtDefs::kPktGen_P17 * RmtDefs::kParserChannels, p, en, NAPPS, false);
 		tu->pktgen_recirc_set(0, false, RmtDefs::kPktGen_P17 * RmtDefs::kParserChannels);


        en_app(en, NAPPS, true);
        //TestUtil::pgr_app_counter ctr[NAPPS];
        //tu->pktgen_get_counters(0, en, ctr);
        //verify_counters(en, ctr, pgen->get_def_recir_count());

		EXPECT_EQ(pgen->g_pgen_cnt_, n_pkts);
		EXPECT_EQ(pgen->get_def_recir_count(), pgen->g_recirc_cnt_);
		EXPECT_EQ(pgen->get_def_recir_count()*NAPPS, pgen->g_triggers_);
		rc->stop();
	}

}
