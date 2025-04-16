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

#include <mau.h>
#include <pipe.h>
#include <string>
#include <ctime>
#include <learning-filter.h>
#include <mirror.h>
#include <packet.h>
#include <packet-gen-metadata.h>
#include <port.h>
#include <rmt-log.h>
#include <ipb.h>
#include <rmt-object-manager.h>

namespace MODEL_CHIP_NAMESPACE {

  Pipe::Pipe(RmtObjectManager *om, int pipeIndex, const PipeConfig& config)
      : PipeObject(om,pipeIndex,0x3F,kType),
        parser_arbiter_(om->parser_arbiter_get(pipeIndex)),
        parse_merge_reg_(chip_index(),pipeIndex),
        curr_dependency_seq_(0), pending_dependency_seq_(1),
        ipipe_table_id_0_(0), epipe_table_id_0_(0),
        evaluate_all_(kEvaluateAllDefault),
        evaluate_all_test_(kEvaluateAllTestDefault),
        extended_word_selector_(UINT64_C(0)) {

    // Setup bitvector to select just tagalong/extended words
    for (int i = 0; i < Phv::kWordsMaxExtended; i++) {
      if (Phv::is_valid_taga_phv_x(i)) extended_word_selector_.set_bit(true, i);
    }
    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugCreate),
            "Pipe::create\n");

  }
  Pipe::~Pipe() {
    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugDelete),
            "Pipe::delete\n");
  }


  int Pipe::ipipe_table_id_0(Phv *phv) {
    if (phv == NULL) return 0;
    Packet *pkt = phv->ingress_packet();
    if (pkt == NULL) return 0;
    Port *port = pkt->port();
    if (port == NULL) return 0;
    int port_num = Port::get_pipe_local_port_index(port->port_index());
    return parse_merge_reg_.phv_start_tab_ingress(port_num);
  }
  int Pipe::epipe_table_id_0(Phv *phv) {
    if (phv == NULL) return 0;
    Packet *pkt = phv->egress_packet();
    if (pkt == NULL) return 0;
    Port *port = pkt->port();
    if (port == NULL) return 0;
    int port_num = Port::get_pipe_local_port_index(port->port_index());
    return parse_merge_reg_.phv_start_tab_egress(port_num);
  }
  int Pipe::ghost_table_id_0(Phv *phv) {
    if (phv == NULL) return 0;
    return parse_merge_reg_.phv_start_tab_ghost();
  }
  int Pipe::ghost_table_id_0() {
    return parse_merge_reg_.phv_start_tab_ghost();
  }


  Phv *Pipe::run_maus_internal(Phv *phv, Phv *ophv) {
    RmtObjectManager *om = get_object_manager();
    Phv *iphv = phv;
    //Phv *ophv = phv;
    Phv *next_iphv = NULL;
    Phv *next_ophv = NULL;
    int ipipe_table_id = ipipe_table_id_0(phv);
    int epipe_table_id = epipe_table_id_0(phv);
    int ghost_table_id = ghost_table_id_0(phv);
    RMT_ASSERT((ipipe_table_id >= 0) && (epipe_table_id >= 0));

    // Bail if no MAUs setup
    Mau *mau0 = om->mau_lookup(pipe_index(), 0);
    if (mau0 == NULL) return NULL;

    // Check dependencies are sane
    if (curr_dependency_seq_ < pending_dependency_seq_) update_dependencies();

    // Used to hunt for first ingress/egress table if none configured
    //if (ipipe_table_id < 0) ipipe_table_id = 0; //mau0->find_first_table(true);
    //if (epipe_table_id < 0) epipe_table_id = 0; //mau0->find_first_table(false);

    // Clone input PHV (if ophv NULL) and use per-MAU ingress/egress
    // PHV word select to copy PHV data words into cloned phv.
    // This ingress/egress select can NEVER copy extended words
    // into the cloned phv so the MAUs just get the non-extended
    // words from input phv to work on.
    //
    // The extended words remain untouched in input phv until
    // run_maus is done. At that point we explicitly copy them
    // into output phv.
    //
    if (ophv == NULL) {
      ophv = phv->clone();
      ophv->copydata(phv, mau0->ingress_selector());
      ophv->copydata(phv, mau0->egress_selector());
    }

    phv->set_source(Phv::kFlagsSourceMau);
    ophv->set_source(Phv::kFlagsSourceMau);

    // Now loop through all MAUs
    for (int stage = 0; stage < om->num_stages(); stage++) {

      Mau *mau = om->mau_lookup(pipe_index(), stage);
      if (mau != NULL) {
        RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugPipeRunSingleMau),
                "Pipe::run_maus(chip=%d pipe=%d stage=%d)\n",
                chip_index(), pipe_index(), stage);

        RMT_P4_LOG_INFO("------------ Stage %d ------------\n", stage);
        mau->execute(iphv, ophv,
                     &ipipe_table_id, &epipe_table_id, &ghost_table_id,
                     &next_iphv, &next_ophv);

        RMT_ASSERT(next_iphv != NULL);
        RMT_ASSERT(next_ophv != NULL);

        om->log_mau_phv(stage, *next_iphv);

        // Move on to next stage with new iphv/ophv - free old ones
        if ((next_iphv != iphv) && (next_ophv != iphv) && (phv != iphv))
          om->phv_delete(iphv);
        if ((next_iphv != ophv) && (next_ophv != ophv) && (phv != ophv))
          om->phv_delete(ophv);
        iphv = next_iphv;
        ophv = next_ophv;
      }
    }
    if ((iphv != ophv) && (iphv != phv)) om->phv_delete(iphv);
    RMT_ASSERT(ophv != NULL);

    phv->set_source(0);
    ophv->set_source(0);

    // Copy tagalong/extended words from input -> output
    ophv->copydata_x(phv, &extended_word_selector_);

    return ophv;
  }


  // Just a wrapper round real func run_maus_internal
  Phv *Pipe::run_maus(Phv *phv) {
    if (nullptr == phv) return nullptr;

    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugPipeRunMaus|RmtDebug::kRmtDebugEnter),
            "Pipe::run_maus(START) T=%p inPHV=0x%08X ..........\n",
            std::time(nullptr), (phv != NULL) ?phv->hash() :0u);
    // Prserve local copy to avoid confusion on later calls to set_evaluate_all()
    bool eval_all_test = evaluate_all_test_;

    // Call run_maus_internal as normal
    Phv *ophv = run_maus_internal(phv);

    if (eval_all_test) {
      // Do run_maus_internal twice more - first with evaluate_all
      // off, then second with evaluate_all on. Results should be
      // same. Finish off by restoring intial value.
      // Reset cacheId on cloned phvs so we don't get any cacheing
      // effects.
      RmtObjectManager *om = get_object_manager();
      Phv *ophv2 = NULL;
      Phv *phvA = phv->clone_x(true);
      Phv *phvB = phv->clone_x(true);
      phvA->set_cache_id();
      RMT_ASSERT(phv->identical(phvA));
      RMT_ASSERT(!phv->cache_id().Equals(phvA->cache_id()));
      phvB->set_cache_id();
      RMT_ASSERT(phv->identical(phvB));
      RMT_ASSERT(!phv->cache_id().Equals(phvB->cache_id()));
      bool curr_eval_all = evaluate_all(); // Save

      // Do first run...
      set_evaluate_all(false, eval_all_test);
      Phv *ophvA = run_maus_internal(phvA);
      set_evaluate_all(true, eval_all_test);
      Phv *ophvB = run_maus_internal(phvB);

      // Compare output hashes
      uint32_t ophv_hash = (ophv != NULL) ?ophv->hash() :0u;
      uint32_t ophvA_hash = (ophvA != NULL) ?ophvA->hash() :0u;
      uint32_t ophvB_hash = (ophvB != NULL) ?ophvB->hash() :0u;

      if ((ophv_hash != ophvA_hash) || (ophv_hash != ophvB_hash)) {
        // If we got mismatch above repeat run but with more debug...
        RMT_LOG(RmtDebug::error(RmtDebug::kRmtDebugPipeRunMaus),
                "Pipe::run_maus() EvaluateAll MISMATCH "
                "ophv=0x%08X ophvA=0x%08X ophvB=0x%08X  re-running.....\n",
                ophv_hash, ophvA_hash, ophvB_hash);
        uint64_t dbgflg = RmtDebug::verbose(RmtDebug::kRmtDebugPipeRunMaus);
        uint64_t ONE = UINT64_C(1);
        uint64_t ALL = UINT64_C(0xFFFFFFFFFFFFFFFF);
        uint64_t FEW = UINT64_C(0xF);
        uint64_t pipes = ONE<<pipe_index();
        om->update_log_flags(pipes,ALL,ALL,ALL,ALL,ALL,ALL); // Loud
        // Restore original setting
        set_evaluate_all(curr_eval_all, eval_all_test);

        // Re-run original phv producing ophv2
        RMT_LOG(dbgflg, "Pipe::run_maus(START orig phv) EvaluateAll=%d .....\n",
                curr_eval_all);
        ophv2 = run_maus_internal(phv);
        RMT_LOG(dbgflg, "Pipe::run_maus(END orig phv) EvaluateAll=%d ophv2=0x%08X\n",
                curr_eval_all, ophv2->hash());
        set_evaluate_all(false, eval_all_test);
        // Then free prev ophvA and rerun with phvA
        RMT_LOG(dbgflg, "Pipe::run_maus(START phvA) EvaluateAll=false .....\n");
        om->phv_delete(ophvA);
        ophvA = run_maus_internal(phvA);
        RMT_LOG(dbgflg, "Pipe::run_maus(END phvA) EvaluateAll=false ophvA=0x%08X\n",
                ophvA->hash());
        set_evaluate_all(true, eval_all_test);
        // Then free prev ophvB and rerun with phvB
        RMT_LOG(dbgflg, "Pipe::run_maus(START phvB) EvaluateAll=true .....\n");
        om->phv_delete(ophvB);
        ophvB = run_maus_internal(phvB);
        RMT_LOG(dbgflg, "Pipe::run_maus(END phvB) EvaluateAll=true ophvB=0x%08X\n",
                ophvB->hash());

        if ((ophv2->hash() != ophvA->hash()) || (ophv2->hash() != ophvB->hash())) {
          RMT_LOG(RmtDebug::error(RmtDebug::kRmtDebugPipeRunMaus),
                  "Pipe::run_maus() EvaluateAll MISMATCH REPEAT "
                  "ophv=0x%08X ophvA=0x%08X ophvB=0x%08X\n",
                  ophv2->hash(), ophvA->hash(), ophvB->hash());
        }
        om->update_log_flags(pipes,ALL,ALL,ALL,ALL,FEW,FEW); // Quiet
      } else {
        RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugPipeRunMaus),
                "Pipe::run_maus() EvaluateAll/ShortCircuit AGREE "
                "ophv=0x%08X ophvA=0x%08X ophvB=0x%08X\n",
                ophv_hash, ophvA_hash, ophvB_hash);
      }

      set_evaluate_all(curr_eval_all, eval_all_test); // Restore again

      if (ophv2 != NULL) om->phv_delete(ophv2);
      om->phv_delete(phvA);
      om->phv_delete(ophvA);
      om->phv_delete(phvB);
      om->phv_delete(ophvB);
    }
    if (ophv != NULL) {
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugPipeRunMaus|RmtDebug::kRmtDebugExit),
              "Pipe::run_maus(END) T=%d inPHV=0x%08X outPHV=0x%08X\n",
              std::time(nullptr),
              (phv != NULL) ?phv->hash() :0u, (ophv != NULL) ?ophv->hash() :0u);
    }
    return ophv;
  }


  // Another wrapper (just for DV)
  Phv *Pipe::run_maus2(Phv *phv, Phv *ophv) {
    return run_maus_internal(phv, ophv);
  }

  // Handle EOP - push deferred stats/meter addresses
  void Pipe::handle_eop(const Eop &eop) {
    if (!eop.valid()) return;
    RmtObjectManager *om = get_object_manager();
    for (int stage = 0; stage < om->num_stages(); stage++) {
      Mau *mau = om->mau_lookup(pipe_index(), stage);
      if (mau != NULL) {
        RMT_P4_LOG_INFO("------------ Stage %d EOP processing ------------\n", stage);
        mau->handle_eop(eop);
      }
    }
  }

  // Handle TEOP - true pktlen/errors off TEOP bus
  void Pipe::handle_dp_teop(const Teop &teop) {
    if (!teop.teop_time() || !teop.in_use()) return;
    RmtObjectManager *om = get_object_manager();
    for (int stage = 0; stage < om->num_stages(); stage++) {
      Mau *mau = om->mau_lookup(pipe_index(), stage);
      if (mau != NULL) {
        RMT_P4_LOG_INFO("------------ Stage %d TEOP processing ------------\n", stage);
        mau->handle_dp_teop(teop);
      }
    }
  }
  void Pipe::handle_dp_teop(Teop *teop) {
    if (teop == NULL) return;
    handle_dp_teop(*teop);
    delete teop;
  }



  int Pipe::get_ipipe_delay() {
    // TODO: TIME: Lots of stuff needs to change to
    // TODO: TIME: make model time handling better
    return 2;
  }
  int Pipe::get_epipe_delay() {
    return 0;
  }

  // Set pktlen and eopnum into Phv and Eop
  void Pipe::set_pktlen_eopinfo(Phv *phv, Eop *eop) {
    RMT_ASSERT((phv != NULL) && (eop != NULL));
    for (int i = 0; i <= 1; i++) {
      bool ingress = (i==0);
      Packet *p = (ingress) ?phv->ingress_packet() :phv->egress_packet();
      if (p != NULL) {
        // NB. No attempt made to detect CutThrough mode and set egress EOP pktlen to 0
        uint16_t pktlen = ingress ? p->len() : p->qing2e_metadata()->len();
        uint8_t port = p->port()->port_index();
        bool resubmit = p->is_resubmit();
        uint8_t eopnum = Eop::make_eopnum(port, resubmit);
        eop->set_eopinfo(pktlen, eopnum, ingress);
        phv->set_eopnum(eopnum, ingress);
      }
    }
  }
  // Set time into Phv and Eop
  void Pipe::set_time_info(Phv *phv, Eop *eop) {
    RMT_ASSERT((phv != NULL) && (eop != NULL));
    // Copy the time info from (one of) the packets.
    // This could match the RTL for normal packets if the time is set carefully
    // But there's no attempt to do resubmit times properly.

    Packet *pkt_in  = phv->ingress_packet(), *pkt_out = phv->egress_packet();
    RMT_ASSERT((pkt_in != NULL) || (pkt_out != NULL));

    // Fish out hdr/eop relative time of both packets - apply ipipe_delay delta to egress
    // NB. No attempt made yet to add delay to reflect buffering in TM
    uint64_t T_phv_in = UINT64_C(0), T_phv_out = UINT64_C(0), T_phv_delta = UINT64_C(0);
    uint64_t T_eop_in = UINT64_C(0), T_eop_out = UINT64_C(0), T_eop_delta = UINT64_C(0);
    if (pkt_out != NULL) {
      T_phv_delta = get_ipipe_delay();
      if (pkt_out->get_time_info(false).relative_time_valid())
        T_phv_out = pkt_out->get_time_info(false).get_relative_time();
      T_phv_out += T_phv_delta;
      T_eop_delta = get_ipipe_delay();
      if (pkt_out->get_time_info(true).relative_time_valid())
        T_eop_out = pkt_out->get_time_info(true).get_relative_time();
      T_eop_out += T_eop_delta;
    }
    if (pkt_in != NULL) {
      if (pkt_in->get_time_info(false).relative_time_valid())
        T_phv_in = pkt_in->get_time_info(false).get_relative_time();
      if (T_phv_in >= T_phv_out) T_phv_delta = UINT64_C(0);
      if (pkt_in->get_time_info(true).relative_time_valid())
        T_eop_in = pkt_in->get_time_info(true).get_relative_time();
      if (T_eop_in >= T_eop_out) T_eop_delta = UINT64_C(0);
    }

    // Use maximal relative time to decide which time_info to copy
    uint64_t             T_phv = (T_phv_out > T_phv_in) ?T_phv_out :T_phv_in;
    uint64_t             T_eop = (T_eop_out > T_eop_in) ?T_eop_out :T_eop_in;
    Packet            *phv_pkt = (T_phv_out > T_phv_in) ?pkt_out :pkt_in;
    Packet            *eop_pkt = (T_eop_out > T_eop_in) ?pkt_out :pkt_in;
    TimeInfoType phv_time_info = phv_pkt->get_time_info(false /*hdr*/ );
    TimeInfoType eop_time_info = eop_pkt->get_time_info(true /*eop*/ );

    // Find curr time and apply further delta if T_now > T_phv/T_eop
    uint64_t T_now = get_object_manager()->time_get_cycles();
    T_phv_delta += (T_now > T_phv) ?(T_now - T_phv) :UINT64_C(0);
    T_eop_delta += (T_now > T_eop) ?(T_now - T_eop) :UINT64_C(0);
    phv_time_info.time_incr(T_phv_delta);
    eop_time_info.time_incr(T_eop_delta);

    // Then update time_info in Phv/Eop
    phv->set_time_info_from(phv_time_info);
    eop->set_time_info_from(eop_time_info);

    // But maybe override from pipe
#ifdef SET_METER_TIME_FROM_PIPE
    phv->set_time_info_from( get_time_info(true) );
    eop->set_time_info_from( get_time_info(true) );
#endif
  }


  void Pipe::process(Packet *inpkt, Packet *outpkt,
                     Packet **queued_pkt, Packet **sent_pkt,
                     Packet **resubmit_pkt,
                     Packet **ing_mirror_pkt,
                     Packet **egr_mirror_pkt,
                     PacketGenMetadata **packet_gen_metadata) {
    if (!enabled()) return;

    RmtObjectManager *om = get_object_manager();
    int inpindex = -1;
    Port *inport = NULL, *outport = NULL;
    Parser *inprs = NULL, *outprs = NULL;
    Phv *inphv = NULL, *inphv_i = NULL, *inphv_e = NULL, *outphv = NULL;
    bool is_resubmit = false;
    bool has_ing_input = false;
    bool has_egr_input = false;
    int in_pchan, in_ipbchan;
    uint16_t orig_ingress_pkt_len = 0, orig_egress_pkt_len = 0;

    is_resubmit = inpkt && inpkt->is_resubmit();

    if ((inpkt != NULL) || (outpkt != NULL)) {
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugPipeProcess|RmtDebug::kRmtDebugEnter),
              "Pipe::process(START) T=%d  %d>>> I=0x%08x:%d O=0x%08x:%d ..........\n",
              std::time(nullptr),
              ((inpkt != NULL) && (inpkt->port() != NULL)) ?inpkt->port()->port_index() :0,
              (inpkt != NULL) ?inpkt->hash() :0u, (inpkt != NULL) ?inpkt->len() :-1,
              (outpkt != NULL) ?outpkt->hash() :0u, (outpkt != NULL) ?outpkt->len() :-1);
    }

    if (inpkt != NULL) {
      if (!inpkt->is_ingress()) {
        RMT_LOG(RmtDebug::warn(), "inpkt NOT flagged as ingress pkt\n");
      } else {
        // Stash away original ingress pkt_len before IPB adds metadata hdrs
        orig_ingress_pkt_len = inpkt->len();
      }
      inport = inpkt->port();
      inpindex = inport->pipe_index();
      in_ipbchan = inport->ipb_chan();
      in_pchan = inport->parser_chan();

      inprs = inport->parser()->ingress();

      if (inport->ipb()->is_chan_meta_enabled(in_ipbchan)) {
        inport->ipb()->insert_metadata(inpkt, in_ipbchan, is_resubmit);
      }
      if (inport->ipb()->is_chan_rx_enabled(in_ipbchan)) {
        RMT_P4_LOG_INFO(
            "========== Packet to input parser: from port %d (%d bytes)  ==========\n",
            inport->port_index(), inpkt->len());
        rmt_log_packet(inpkt);
        auto *ipb_counters = inport->ipb()->get_ipb_counters(
            in_ipbchan);
        ipb_counters->increment_chnl_macs_received_pkt();

        // inphv may have tagalong/extended words set
        inphv_i = inprs->parse(inpkt, in_pchan);
        if (inphv_i) {
          if ((!is_chip1_or_later()) && (inprs->hdr_len_adj() != RmtDefs::kIpbHeaderSizeBytes)) {
            RMT_LOG(RmtDebug::warn(), "IngressParser HdrLenAdj(%d) != IpbHeaderSize(%d)\n",
                    inprs->hdr_len_adj(), RmtDefs::kIpbHeaderSizeBytes);
          }
          has_ing_input = true;
          inpkt->trim_metadata_hdr();
        }
      } else {
        RMT_P4_LOG_INFO(
            "========== Packet to input parser: from port %d (%d bytes)  ==========\n"
            "========== Packet DROPPED: channel disabled in IPB          ==========\n",
            inport->port_index(), inpkt->len());
      }
    }
    if (outpkt != NULL) {
      if (!outpkt->is_egress()) {
        RMT_LOG(RmtDebug::warn(), "outpkt NOT flagged as egress pkt\n");
      } else {
        // Stash away original egress pkt_len before EPB adds metadata hdrs
        orig_egress_pkt_len = outpkt->len();
      }

      outport = outpkt->port();

      if ((inpindex < 0) || (inpindex == outport->pipe_index())) {
        int out_epbchan = outport->epb_chan();
        int out_pchan = outport->parser_chan();

        // Call EPB to add configurable egress metadata hdr
        outpkt = outport->epb()->add_metadata_hdr(outpkt, out_epbchan);
        RMT_P4_LOG_INFO(
            "========== Packet to egress parser: to port %d (%d bytes)  ==========\n",
            outport->port_index(), outpkt->len());
        rmt_log_packet(outpkt);

        // Parse output packet
        outprs = outport->parser()->egress();
        inphv_e = outprs->parse(outpkt, out_pchan);
        if (inphv_e) {
          uint16_t epb_hdr_bytes = outpkt->len() - orig_egress_pkt_len;
          // XXX: complain if egress parser hdr_len_adj doesn't equal EPB hdr bytes
          if ((!is_chip1_or_later()) && (outprs->hdr_len_adj() != epb_hdr_bytes)) {
            RMT_LOG(RmtDebug::warn(), "EgressParser HdrLenAdj(%d) != EpbHeaderSize(%d)\n",
                    outprs->hdr_len_adj(), epb_hdr_bytes);
          }
          has_egr_input = true;
          outpkt->trim_metadata_hdr();
        }
      } else {
        RMT_LOG(RmtDebug::warn(), "inpkt/outpkt destined for diff pipes");
      }
    }

    if (inphv_i) {
        // log parser results - valid headers
        RMT_P4_LOG_VERBOSE("Ingress Headers:\n");
        rmt_log_pov(false, inphv_i);
        parser_arbiter_->handle_phv(inphv_i, false, is_resubmit);
    }
    if (inphv_e) {
        // log parser results - valid headers
        RMT_P4_LOG_VERBOSE("Egress Headers:\n");
        rmt_log_pov(true, inphv_e);
        parser_arbiter_->handle_phv(inphv_e, true);
    }
    if (inphv_i && inphv_e) {
        inphv = get_object_manager()->phv_create();
        inphv->merge_phvs(inphv_i, inphv_e);
        inphv->set_cache_id();
        inpkt->set_phv(inphv);
        outpkt->set_phv(inphv);
        get_object_manager()->phv_delete(inphv_i);
        inphv_i = nullptr;
        get_object_manager()->phv_delete(inphv_e);
        inphv_e = nullptr;
    } else {
        inphv = inphv_i ? inphv_i : inphv_e;
    }

    if (inphv != NULL) {
      Eop eop; // Install pktlen/eopnum  time_info in inphv/eop
      set_pktlen_eopinfo(inphv, &eop);
      set_time_info(inphv, &eop);

      // Call all MAUs on composite PHV
      outphv = run_maus(inphv);
      RMT_ASSERT(outphv != NULL);
      // Handle EOP right now - TODO: EOP: allow this to be deferred?
      parser_arbiter_->handle_eop(&eop, is_resubmit);
      handle_eop(eop);

      LearnQuantumType lq;
      if (queued_pkt && has_ing_input && inport && inport->deparser() != NULL) {

        // Now that we've run inpkt through ingress Parser/MAU
        // it's time to setup orig_ingress_pkt_len metadata
        // (this is used by EPB to set eg_intr_md.pkt_length metadata)
        // Could set earlier but that risks it being used inadvertently
        inpkt->set_orig_ingress_pkt_len(orig_ingress_pkt_len);

        Packet *mirror_pkt = nullptr;
        RMT_P4_LOG_VERBOSE("Ingress Deparser Headers:\n");
        rmt_log_pov(false, outphv);
        // Call ingress deparser with output PHV
        *queued_pkt = inport->deparser()->DeparseIngress(*outphv, &lq, &mirror_pkt, resubmit_pkt, packet_gen_metadata);
        // XXX: Set incoming packet to nullptr if the DeparseIngress deletes the packet.
        if (*queued_pkt == nullptr) inpkt = nullptr;
        // We cannot have a regular packet and a resubmitted packet.
        RMT_ASSERT((nullptr == (*resubmit_pkt)) || (nullptr == (*queued_pkt)));
        if (nullptr != (*queued_pkt)) {
          (*queued_pkt)->set_phv(NULL);
        }
        if (mirror_pkt && ing_mirror_pkt) {
          Packet *tx_mirror_pkt = nullptr;
          if (inport && inport->mirror()) {
            mirror_pkt->set_port(inport);
            tx_mirror_pkt = inport->mirror()->ProcessMirrorPacket(mirror_pkt,
                                                                  true/*ingr*/);
          }
          if (tx_mirror_pkt != nullptr) {
            // Mirror packets just egress so setup orig_ingress_pkt_len now
            tx_mirror_pkt->set_orig_ingress_pkt_len(tx_mirror_pkt->len());
          } else {
            // packet is not mirrored (config) or stored in mirr buf for coalescing etc
            // coal not supported on ingress
            om->pkt_delete(mirror_pkt);
          }
          *ing_mirror_pkt = tx_mirror_pkt;
        }
      }
      // TODO: call learning block here (even if there is no packet, so timer is run)
      LearningFilter* lf = om->learning_filter_get( pipe_index() );
      if (lf && lf->learn(lq) && inport) {
         inport->deparser()->deparser_reg()->increment_learn_counter();
      }

      if (sent_pkt && has_egr_input && outport && outport->deparser()) {
        RMT_P4_LOG_VERBOSE("Egress Deparser Headers:\n");
        Packet *mirror_pkt = nullptr;
        rmt_log_pov(true, outphv);
        // Call egress deparser with output PHV
        *sent_pkt = outport->deparser()->DeparseEgress(*outphv, &mirror_pkt);
        // XXX: Set outgoing packet to nullptr if the DeparseEgress deletes the packet.
        if (*sent_pkt == nullptr) outpkt = nullptr;
        if (mirror_pkt) {
          mirror_pkt->set_port(outport);
        }
        // Maybe call MAUs again to handle TEOP (non-NULL on JBay only)
        Teop *teop = outphv->teop();
        if (((*sent_pkt) != nullptr) && (teop != NULL)) {
          // Only full in len if Deparser hasn't done so
          if (teop->hdr_time()) teop->set_byte_len((*sent_pkt)->len());
        }
        handle_dp_teop(teop);
        outphv->set_teop(NULL);

        if (nullptr != (*sent_pkt)) {
          (*sent_pkt)->set_phv(NULL);
        }
        if (mirror_pkt && egr_mirror_pkt) {
          Packet *tx_mirror_pkt = nullptr;
          if (outport && outport->mirror()) {
            tx_mirror_pkt = outport->mirror()->ProcessMirrorPacket(mirror_pkt,
                                                                   false/*egr*/);
          }
          if (tx_mirror_pkt != nullptr) {
            // Mirror packets just egress so setup orig_ingress_pkt_len now
            tx_mirror_pkt->set_orig_ingress_pkt_len(tx_mirror_pkt->len());
          } else {
            // packet is not mirrored or stored in mirr buf for coalescing
            om->pkt_delete(mirror_pkt);
          }
          *egr_mirror_pkt = tx_mirror_pkt;
        }
      }
    }

    if ((inpkt != NULL) || (outpkt != NULL)) {
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugPipeProcess|RmtDebug::kRmtDebugExit),
              "Pipe::process(END) T=%p  %d>>> I=0x%08x:%d O=0x%08x:%d  "
              "inPHV=0x%08X outPHV=0x%08X  Q=0x%08x:%d TX=0x%08x:%d >>>%d\n",
              std::time(nullptr),
              ((inpkt != NULL) && (inpkt->port() != NULL)) ?inpkt->port()->port_index() :0,
              (inpkt != NULL) ?inpkt->hash() :0u, (inpkt != NULL) ?inpkt->len() :-1,
              (outpkt != NULL) ?outpkt->hash() :0u, (outpkt != NULL) ?outpkt->len() :-1,
              (inphv != NULL) ?inphv->hash() :0u, (outphv != NULL) ?outphv->hash() :0u,
              ((queued_pkt != NULL) && (*queued_pkt != NULL)) ?(*queued_pkt)->hash() :0u,
              ((queued_pkt != NULL) && (*queued_pkt != NULL)) ?(*queued_pkt)->len() :-1,
              ((sent_pkt != NULL) && (*sent_pkt != NULL)) ?(*sent_pkt)->hash() :0u,
              ((sent_pkt != NULL) && (*sent_pkt != NULL)) ?(*sent_pkt)->len() :-1,
              ((sent_pkt != NULL) && (*sent_pkt != NULL)) ?(*sent_pkt)->i2qing_metadata()->egress_uc_port() :0);
    }

    // Free up PHVs
    if (inphv != NULL) om->phv_delete(inphv);
    if (outphv != NULL) om->phv_delete(outphv);
    return;
  }


  void Pipe::update_dependencies() {
    while (curr_dependency_seq_ < pending_dependency_seq_) {
      curr_dependency_seq_ = pending_dependency_seq_;

      // This code subsumed by better per-MAU checking
      // which examines prev/next MAU too - see Mau::check_dependency_config

      // RmtObjectManager *om = get_object_manager();
      // for (int stage = 1; stage < om->num_stages(); stage++) {
      //    Mau *prev_mau = om->mau_lookup(pipe_index(), stage-1);
      //    Mau *curr_mau = om->mau_lookup(pipe_index(), stage);
      //    if ((prev_mau != NULL) && (curr_mau != NULL)) {
      //       MauDependencies *pd = prev_mau->mau_dependencies();
      //       MauDependencies *cd = curr_mau->mau_dependencies();
      //       RMT_ASSERT ((pd != NULL) && (cd != NULL));
      //       // Sanity check dependencies
      //       if ((cd->ingress_prev_conc() && !pd->ingress_next_maybe_conc()) ||
      //           (cd->ingress_prev_action_dependent() && !pd->ingress_next_maybe_action_dependent()) ||
      //           (cd->ingress_prev_match_dependent() && !pd->ingress_next_maybe_match_dependent()))
      //         RMT_LOG(RmtDebug::warn(), "Stage dependency mismatch - ingress\n");
      //       if ((cd->egress_prev_conc() && !pd->egress_next_maybe_conc()) ||
      //           (cd->egress_prev_action_dependent() && !pd->egress_next_maybe_action_dependent()) ||
      //           (cd->egress_prev_match_dependent() && !pd->egress_next_maybe_match_dependent()))
      //         RMT_LOG(RmtDebug::warn(), "Stage dependency mismatch - egress\n");
      //
      //    }
      // }
    }
  }
  void Pipe::dependencies_changed() {
    pending_dependency_seq_++;
  }


  bool Pipe::evaluate_all(int mauIndex) {
    RmtObjectManager *om = get_object_manager();
    RMT_ASSERT (om != NULL);
    if ((mauIndex < 0) || (mauIndex >= om->num_stages())) return false;
    Mau *mau = om->mau_lookup(pipe_index(), mauIndex);
    return (mau != NULL) ?mau->evaluate_all() :false;
  }
  void Pipe::set_evaluate_all(int mauIndex, bool tf) {
    RmtObjectManager *om = get_object_manager();
    RMT_ASSERT (om != NULL);
    if ((mauIndex < 0) || (mauIndex >= om->num_stages())) return;
    Mau *mau = om->mau_lookup(pipe_index(), mauIndex);
    if (mau != NULL) mau->set_evaluate_all(tf);
  }
  bool Pipe::evaluate_all() {
    return evaluate_all_;
  }
  void Pipe::set_evaluate_all(bool tf, bool dotest) {
    RmtObjectManager *om = get_object_manager();
    RMT_ASSERT (om != NULL);
    for (int mauIndex = 0; mauIndex < om->num_stages(); mauIndex++) {
      set_evaluate_all(mauIndex, tf);
    }
    evaluate_all_ = tf;
    evaluate_all_test_ = dotest;
    RMT_LOG(RmtDebug::verbose(),
            "Pipe::set_evaluate_all(chip=%d pipe=%d stage=*) "
            "EvalAll=%d EvalAllTest=%d\n",
            chip_index(), pipe_index(), tf, dotest);
  }


}
