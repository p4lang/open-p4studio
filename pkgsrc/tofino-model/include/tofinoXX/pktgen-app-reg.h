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

#ifndef _TOFINOXX_PKTGEN_APP_REG_H
#define _TOFINOXX_PKTGEN_APP_REG_H

#include <thread>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <register_includes/pgr_app_ctr48_batch_mutable.h>
#include <register_includes/pgr_app_ctr48_packet_mutable.h>
#include <register_includes/pgr_app_ctr48_trigger_mutable.h>
#include <register_includes/pgr_app_ctrl.h>
#include <register_includes/pgr_app_event_ibg.h>
#include <register_includes/pgr_app_event_ipg.h>
#include <register_includes/pgr_app_event_jitter_mask.h>
#include <register_includes/pgr_app_event_jitter_value.h>
#include <register_includes/pgr_app_event_number.h>
#include <register_includes/pgr_app_event_timer.h>
#include <register_includes/pgr_app_ingr_port_ctrl.h>
#include <register_includes/pgr_app_payload_ctrl.h>
#include <register_includes/pgr_app_recir_match_mask.h>
#include <register_includes/pgr_app_recir_match_value.h>
#include <register_includes/pgr_port_down_dis_mutable.h>
#include <common/rmt-util.h>
#include <model_core/timer-manager.h>
#include <packet.h>
#include <tofino_lfsr.h>


namespace MODEL_CHIP_NAMESPACE {


    class PktGenAppReg;
    enum TRIGGER_TYPE_E {
        ONETIME =  0,
        PERIODIC = 1,
        PORT_DOWN = 2,
        PACKET_RECIRC = 3,
        UNDEF = 4
    } ;

    /*
     * Instance of this structure is created every time
     * an event fires. Contains all the information necessary
     * for packet generation
     */


    class app_timer {

        public:
            app_timer(PktGenAppReg* pgen);
            app_timer(const app_timer& other) = delete;  // XXX
            ~app_timer();

            /* Timing related */
            void pkt_cb(void);
            void batch_cb(void);
            void lfsr_b_cb(void);
            void lfsr_p_cb(void);

            /* Main start/stop */
            void start(void);
            void stop();
            Packet *b_pkt;
        private:
            app_timer& operator=(const app_timer&){ return *this; } // XXX
            PktGenAppReg* pgen_;
            uint16_t ing_p;
            bool ing_p_inc;
            uint32_t o_recir_h;
            uint32_t n_recir_h;
            uint16_t down_port;
            uint8_t ch_;
            model_timer::Timer ibg_t;
            model_timer::Timer ipg_t;
            model_timer::Timer lfsr_b_t;
            model_timer::Timer lfsr_p_t;
            std::atomic<uint16_t> p_count;
            std::atomic<uint16_t> b_count;
            uint32_t lfsr_p_val;
            uint32_t lfsr_b_val;

            std::atomic<bool>b_expired;

            void new_batch_(void);
            void new_packet_(void);

            /*
             * Non timing related, these are functionality related
             */

            void gen_packet_(void);
            void setup_data_(void);
            uint8_t next_channel_(void);

    };

    class app_config {
        public:

            static constexpr uint32_t kJitterDefVal = 0xFFFFFFFF;

            uint8_t app_type;
            uint8_t*  payload_buf;
            uint16_t payload_buf_sz;
            uint16_t app_ingr_port;
            bool	 app_ingr_port_inc;
            uint32_t recir_match_value;
            uint32_t recir_match_mask;
            uint16_t batch_num;
            uint16_t packet_num;
            uint32_t ibg_count;
            uint32_t event_ibg_jitter_value;
            uint32_t event_ibg_jitter_mask;
            uint32_t ipg_count;
            uint32_t event_ipg_jitter_value;
            uint32_t event_ipg_jitter_mask;
            uint32_t event_timer;
            int32_t diff_clks;

            app_config ():app_type (TRIGGER_TYPE_E::UNDEF), payload_buf (nullptr), payload_buf_sz (0),
            app_ingr_port(0), app_ingr_port_inc (0), recir_match_value(0), recir_match_mask (0), batch_num (0),
            packet_num(0), ibg_count(0), event_ibg_jitter_value(0), event_ibg_jitter_mask(kJitterDefVal), ipg_count(0),
            event_ipg_jitter_value(0), event_ipg_jitter_mask(kJitterDefVal), event_timer(0), diff_clks(0){
            }
            app_config(const app_config& other) = delete;  // XXX
            ~app_config () {
                if (payload_buf != nullptr) {
                    delete payload_buf;
                    payload_buf = nullptr;
                }
            }
      private:
        app_config& operator=(const app_config&){ return *this; } // XXX

    };


    class app_counters {

        public:
            void reset_trigger(void) { pg_triggerctr.reset(); }
            void reset_batch(void) { pg_batchctr.reset(); }
            void reset_packet(void) { pg_pktctr.reset(); }
            void reset_all(void) {
                reset_trigger();
                reset_batch();
                reset_packet();
            }

            app_counters(int chip, int pipe, int app_id);
            ~app_counters(void) =default;

            void trigger_inc(void) {
                uint64_t val = pg_triggerctr.ctr48(); ++val; pg_triggerctr.ctr48(val);
            }
            void batch_inc(void) { uint64_t val = pg_batchctr.ctr48(); ++val; pg_batchctr.ctr48(val); }
            void packet_inc(void) { uint64_t val = pg_pktctr.ctr48(); ++val; pg_pktctr.ctr48(val); }

        private:
            app_counters(const app_counters& other) =delete;
            app_counters& operator=(const app_counters& other) =delete;

            register_classes::PgrAppCtr48BatchMutable 			pg_batchctr;
            register_classes::PgrAppCtr48PacketMutable 			pg_pktctr;
            register_classes::PgrAppCtr48TriggerMutable 		pg_triggerctr;

    };

    class PktGenReg;
    class P17_regs;




    class PktGenAppReg {

        public:
            PktGenAppReg(int chip = 0, int pipe = 0, int app_id = 0, P17_regs* pgen = nullptr);

            ~PktGenAppReg();
            inline uint8_t& en(void) { return _en; }
            inline void set_en(uint8_t val) { _en = val; }

            app_config m_app_config;
            app_counters m_app_counters;
            app_timer a_timer;
            std::mutex r_mutex;
            P17_regs* m_pgen;
            int m_pipe;
            uint8_t m_id;
            std::queue<uint64_t> ev_q_;

            void set_packet_payload(uint8_t* buf, uint32_t sz);
            void handle_port_down(uint16_t port);
            void handle_recirc(uint32_t recir_header);
            void start(void);
            void stop(void);
            bool g_done;


        private:
            DISALLOW_COPY_AND_ASSIGN(PktGenAppReg);
            std::thread* app_th;
            uint8_t _en;
            uint32_t dur_;
            std::condition_variable r_cv;
            std::atomic<bool>recir_done_;
            model_timer::Timer main_t;

            register_classes::PgrAppCtrl 				pg_ctrl;
            register_classes::PgrAppEventIbg 			pg_eventibg;
            register_classes::PgrAppEventIpg 			pg_eventipg;
            register_classes::PgrAppEventNumber 		pg_eventnumber;
            register_classes::PgrAppEventTimer 			pg_eventtimer;
            register_classes::PgrAppIngrPortCtrl 		pg_portctrl;
            register_classes::PgrAppPayloadCtrl 		pg_payloadctrl;
            register_classes::PgrAppRecirMatchMask 		pg_recirmatchmask;
            register_classes::PgrAppRecirMatchValue 	pg_recirmatchvalue;
            register_classes::PgrAppEventJitterMask 	pg_ipgjittermask;
            register_classes::PgrAppEventJitterValue 	pg_ipgjittervalue;
            register_classes::PgrAppEventJitterMask 	pg_ibgjittermask;
            register_classes::PgrAppEventJitterValue 	pg_ibgjittervalue;


            void pg_ctrl_wb();
            void reset_(void);
            void _copy_all(void);
            void app_process_(void);
            void port_down(uint16_t port);
            void cpu_down(void);
            bool port_down_ev_(void);
            bool recir_match_ev_(void);
            bool recir_match_packet_(void);
            void process_ev_(void);
            void clear_ports_(void);
            void clear_recir_(void);
            void clear_pending_(void);
            void setup_app_(void);
            void main_cb(uint64_t tid);

    };






};

#endif
