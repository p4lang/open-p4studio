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

#ifndef _TOFINOXX_PKTGEN_REG_H
#define _TOFINOXX_PKTGEN_REG_H

#include <register_includes/pgr_port16_ctrl.h>
#include <register_includes/pgr_port16_ts.h>
#include <register_includes/pgr_port17_ctrl1.h>
#include <register_includes/pgr_port17_ctrl2.h>
#include <register_includes/pgr_port17_ts.h>
#include <register_includes/pgr_port_down_dis_mutable.h>
//#include <tofino/register_includes/pgr_port_down_dis.h>
#include <indirect-addressing.h>
#include <packet.h>
#include <pktgen.h>
#include <pktgen-app-reg.h>
#include <rmt-defs.h>
#include <tofino_lfsr.h>
#include <common/rmt-util.h>
#include <common/bounded_queue.h>
#include <thread>
#include <mutex>
#include <queue>
#include <atomic>
#include <queue>
#include <bitset>

namespace MODEL_CHIP_NAMESPACE {

    class PktGen;
    class PktGenAppReg;
    class PacketEnqueuer;

    class PktGenReg {

        public:

            static constexpr int kChannelModes = 5;
            static constexpr uint8_t ch_mode_[kChannelModes][RmtDefs::kChlsPerIpb] = {
                {0, 0, 0, 0},
                {0, 2, 0, 2},
                {0, 2, 0, 3},
                {0, 2, 1, 2},
                {0, 2, 1, 3}
            };
            static constexpr int CHANNEL_UNDEF = 4;

            virtual PktGen* get_model() { return m_pktgen; }
            virtual ~PktGenReg(){}
            virtual bool recirc(void) = 0;
            virtual bool drop(void) = 0;
            virtual bool mxbar(void) = 0;
            virtual uint8_t get_channel(bool inc) = 0;
            virtual bool is_ch_enabled(uint8_t ch) = 0;
            virtual bool recirc_packet(Packet** packet) = 0;
            virtual void port_down(uint16_t port) = 0;
            virtual void stop(void) = 0;
            inline int get_pipe() { return m_pipe; }

      PktGenReg(int chip, int pipe, RmtObjectManager* om,  PktGen *pktgen, PacketEnqueuer* pe);
            RmtObjectManager* om_;
            PktGen* m_pktgen;
            PacketEnqueuer* packet_enqueuer_;
        protected:
            int m_pipe;
            int m_chip;
            uint8_t ch_tdm[RmtDefs::kChlsPerIpb];
            uint8_t curr_seq_ch_;
            //static uint16_t ch_mode_[kChannelModes][RmtDefs::kChlsPerIpb];
    };



    class P16_regs: public PktGenReg {

        public:

           P16_regs(int chip, int pipe, RmtObjectManager* om, PktGen* pgen, PacketEnqueuer* pe);
            virtual ~P16_regs(void);
            virtual uint8_t get_channel(bool inc);

            virtual bool recirc(void) { return pg_port16ctrl.recir_en(); }
            virtual bool drop(void) {
                return (!pg_port16ctrl.mxbar_en() &&
                        !pg_port16ctrl.recir_en()); }

            virtual bool mxbar(void) { return pg_port16ctrl.mxbar_en(); }

            virtual uint16_t ts_offset(void) {
                return pg_port16ts.offset(); }

            virtual bool recirc_packet(Packet** packet);
            virtual bool is_ch_enabled(uint8_t ch);
            virtual void port_down(uint16_t port) {}
            virtual void stop(void) {}
            register_classes::PgrPort16Ctrl 			pg_port16ctrl;
            register_classes::PgrPort16Ts 				pg_port16ts;

        private:
            P16_regs(const P16_regs& other)=delete;
            P16_regs& operator=(const P16_regs& other)=delete;


            void reset_(void);
            uint8_t get_seq_ch(void);
            uint8_t get_tdm_ch(void);
            void pg_port16ctrl_wb();

    };

    /*
     * Global App Event number generation
     */

    class EvNum {
        public:
            explicit EvNum(void):ev_num_(0){}
            uint64_t next(void) { return ++ev_num_; }
            uint64_t curr(void) { return ev_num_; }
            ~EvNum() =default;
        private:
            std::atomic<uint64_t> ev_num_;
    };

    class EvData {

        public:

            explicit EvData(uint32_t e_data):e_data_(e_data) {}
            ~EvData(void) =default;
            uint32_t data() const { return e_data_; }
            bool is_done(void) const { return ((!is_armed()) || (status_.all())); }
            bool is_sw_done(void) const { return status_[2]; }
            bool is_hw_done(void) const { return status_[1]; }
            void app_done(uint8_t app_id) {
                std::lock_guard<std::mutex> lock(p_mutex);
                app_set.reset(app_id);
                if (app_set.none()) status_.set(1);
            }
            void sw_done(void)  { status_.set(2); }
            void app_add(uint8_t app_id) { app_set.set(app_id); arm(); }
            bool is_app_set(uint8_t app_id) { return app_set.test(app_id); }
            void arm(void) {  status_.set(0); }
            bool is_armed(void) const { return status_[0]; }

        private:
            EvData(const EvData& other)=delete;
            EvData& operator=(const EvData& other)=delete;

            uint32_t e_data_;
            std::bitset<3> status_;

            /*
             * Bit 0 -> There is at least one app present when this event was created
             * Bit 1 -> All Apps are done, then this is set
             * Bit 2 -> Software has reset the port.
             */

            std::bitset<RmtDefs::kPktGenApps> app_set;
            std::mutex p_mutex;


    };

    class PktGenBufferMem : public RmtObject,
        public model_core::RegisterBlockIndirect<RegisterCallback> {
        public:
          static constexpr uint64_t BUF_ST_ADDR = BFN_MEM_PIPE_PKTGEN_ADDR(0);
          static constexpr uint64_t BUF_ADDR_STEP = BFN_MEM_PIPE_ESZ;
          static constexpr uint64_t BUF_ENTRIES = BFN_MEM_PIPE_PKTGEN_BUFMEM_CNT(0);
          static constexpr uint64_t BUF_ELEM_SZ = BFN_MEM_PIPE_PKTGEN_BUFMEM_ESZ(0);
          static constexpr uint64_t BUF_SZ = BUF_ENTRIES * BUF_ELEM_SZ;


          PktGenBufferMem(RmtObjectManager *om,  PktGenReg *com);

          // Return into a linearized array
          void get_val(uint8_t* buf, uint16_t st_offset, uint16_t n_entries);

   private:
          bool write(uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T);
          bool read(uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T) const;
          std::string to_string(bool print_zeros, std::string indent_string) const { return ""; }
          std::string to_string(uint64_t offset, bool print_zeros, std::string indent_string) const { return ""; }
          std::mutex mutex_;
          uint8_t buf_[BUF_ENTRIES][BUF_ELEM_SZ];
    };


    class P17_regs: public PktGenReg {

        public:

            static constexpr uint32_t PGEN_TMO = 1000;
            P17_regs(int chip, int pipe, RmtObjectManager* om, PktGen* pgen, PacketEnqueuer* pe);
            virtual ~P17_regs(void);


            virtual bool recirc(void) { return pg_port17ctrl1.recir_en(); }
            virtual bool drop(void) { return (!pg_port17ctrl1.recir_en());}
            virtual bool mxbar(void) { return false; }
            bool pgen(void) { return (pg_port17ctrl1.pgen_en() && pg_port17ctrl2.pgen_channel_en()); }


            virtual uint8_t get_channel(bool inc);
            virtual bool is_ch_enabled(uint8_t ch);
            virtual uint16_t ts_offset(void) { return pg_port17ts.offset(); }
            virtual bool recirc_packet(Packet** packet);
            virtual void port_down(uint16_t port);
            virtual void stop(void);

            std::array<PktGenAppReg*, RmtDefs::kPktGenApps> pgen_app_;

            model_common::BoundedQueue<Packet*> gen_q_;
            model_common::BoundedQueue<Packet*> recir_q_;

            // Threads
            std::thread* main_thread;

            // Event Numbers, no practical use other than to signal events
            EvNum ev_ids_;

            uint32_t dur_;

            // Flags
            bool g_done;

            // Port Down vector. Stores the port_number, and the apps that are supposed to run for this
            // port down event. If a port down entry is already present, then new entry for that port is not
            // added. Entries that have been processed are cleared by software by writing to pg_portdowndis
            // Once an entry is setup, the port_down thread pushes an event into the listening queues for
            // the interested applications.

            std::vector<EvData*>port_down_vec_;
            std::mutex port_down_mutex_;

            /*
             * Store recirculation matched packets here
             * and queue an event to the individual threads
             */

            std::vector<EvData*>recir_match_;
            std::mutex recir_mutex_;

            // Event mutex. Every app thread has to acquire this mutex to *really* generate packets
            std::mutex ev_mutex_;
            std::condition_variable port_cv_;
            std::atomic<bool> ev_flag_;
            tofino_lfsr lfsr_;


            // PktGen Shared Buffer
            PktGenBufferMem pgen_buffer;
            register_classes::PgrPort17Ctrl1 			pg_port17ctrl1;
            register_classes::PgrPort17Ctrl2 			pg_port17ctrl2;


            // Register programming mutex
            std::mutex reg_mutex;

        private:
            P17_regs(const P17_regs& other)=delete;
            P17_regs& operator=(const P17_regs& other)=delete;

            register_classes::PgrPort17Ts 				pg_port17ts;
            //Model uses the hardware version
            register_classes::PgrPortDownDisMutable 	pg_portdowndis;

            void pg_port17ctrl1_wb();
            void pg_portdowndis_wb();


                void reset_(void);
            void start_(void);

            uint8_t get_seq_ch(void);
            uint8_t get_tdm_ch(void);

            // void tx_helper(Packet *p);
            void main_proc();
            void recir_snoop_handle(Packet* r_packet);
            void queue_evts_(EvData* dat);
            void recirc_reap_(void);
            void port_reap_(bool all, uint32_t port);


    };





};

#endif
