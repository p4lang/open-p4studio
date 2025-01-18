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

#ifndef _SHARED_PIPE_
#define _SHARED_PIPE_

#include <vector>
#include <array>
#include <string>
#include <cstdint>
#include <rmt-log.h>
#include <rmt-defs.h>
#include <pipe-object.h>
#include <eop.h>
#include <teop.h>
#include <phv.h>
#include <parser-arbiter.h>
#include <parse-merge-reg-chip.h>
#include <register_adapters.h>


namespace MODEL_CHIP_NAMESPACE {

  class Packet;
  class PacketGenMetadata;

  class Pipe : public PipeObject {

 private:
    static constexpr int  kType = RmtTypes::kRmtTypePipe;
    static constexpr bool kEvaluateAllDefault = RmtDefs::kEvaluateAllDefault;
    static constexpr bool kEvaluateAllTestDefault = false;

 public:
    Pipe(RmtObjectManager *om, int pipeIndex, const PipeConfig &config);
    virtual ~Pipe();

    // These no longer used
    inline int ipipe_table_id_0()  const { return ipipe_table_id_0_; }
    inline int epipe_table_id_0()  const { return epipe_table_id_0_; }

    inline void set_ipipe_table_id_0(int tab) {
      parse_merge_reg_.phv_set_start_tab_ingress(tab);
    }
    inline void set_epipe_table_id_0(int tab) {
      parse_merge_reg_.phv_set_start_tab_egress(tab);
    }
    inline void set_ghost_table_id_0(int tab) {
      parse_merge_reg_.phv_set_start_tab_ghost(tab);
    }
    inline void set_ipipe_table_id_0(int port, int tab) {
      parse_merge_reg_.phv_set_start_tab_ingress(port, tab);
    }
    inline void set_epipe_table_id_0(int port, int tab) {
      parse_merge_reg_.phv_set_start_tab_egress(port, tab);
    }
    int  ipipe_table_id_0(Phv *phv);
    int  epipe_table_id_0(Phv *phv);
    int  ghost_table_id_0(Phv *phv);
    int  ghost_table_id_0();

    Phv *run_maus(Phv *phv);
    Phv *run_maus2(Phv *phv, Phv *ophv);
    void handle_eop(const Eop &eop);
    void handle_dp_teop(const Teop &teop);
    void handle_dp_teop(Teop *teop);
    void process(Packet *inpkt, Packet *outpkt, Packet **queued_pkt, Packet **sent_pkt,
                 Packet **resubmit_pkt,
                 Packet **ing_mirror_pkt,
                 Packet **egr_mirror_pkt,
                 PacketGenMetadata **packet_gen_metadata
                 );
    void update_dependencies();
    void dependencies_changed();

    // Allow evaluate_all to be set at pipe level
    bool evaluate_all(int mauIndex);
    void set_evaluate_all(int mauIndex, bool tf);
    bool evaluate_all();
    void set_evaluate_all(bool tf, bool dotest=false);

    // Hand out addresses of per-pipe objects
    ParseMergeReg *prs_merge_reg() {
      return &parse_merge_reg_;
    }

    TimeInfoType& get_time_info(bool eop) {
      return eop ? eop_time_info_ : hdr_time_info_;
    }
    void setup_time_info(uint64_t first_tick_time) {
      hdr_time_info_.set_all(first_tick_time);
      eop_time_info_.set_all(first_tick_time+2);
    }

 private:
    int  get_ipipe_delay();
    int  get_epipe_delay();
    void set_pktlen_eopinfo(Phv *phv, Eop *eop);
    void set_time_info(Phv *phv, Eop *eop);
    Phv *run_maus_internal(Phv *phv, Phv *ophv=NULL);

    ParserArbiter                     *parser_arbiter_;
    ParseMergeRegType                  parse_merge_reg_;
    int                                curr_dependency_seq_;
    int                                pending_dependency_seq_;
    int                                ipipe_table_id_0_;
    int                                epipe_table_id_0_;
    bool                               evaluate_all_;
    bool                               evaluate_all_test_;
    BitVector<Phv::kWordsMaxExtended>  extended_word_selector_;
    TimeInfoType                       hdr_time_info_{};
    TimeInfoType                       eop_time_info_{};
  };
}
#endif // _SHARED_PIPE_
