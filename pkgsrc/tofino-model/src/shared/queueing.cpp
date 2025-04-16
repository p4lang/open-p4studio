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

#include <rmt-log.h>
#include <common/rmt-flags.h>
#include <chip.h>
#include <rmt-object-manager.h>
#include <queueing.h>
#include <packet-replication-engine.h>
#include <rmt-packet-coordinator.h>
#include <port.h>

namespace MODEL_CHIP_NAMESPACE {

  void Queueing::enqueue(Packet *pkt) {
    RMT_ASSERT (pkt);
    RmtObjectManager *om = get_object_manager();

    // If replication is required if either MGID is valid or C2C is set.
    bool replicate = pkt->i2qing_metadata()->cpu_needs_copy() ||
                     pkt->i2qing_metadata()->has_mgid1() ||
                     pkt->i2qing_metadata()->has_mgid2();

    // Pack is unicast if the egress unicast port is valid.  Note that a packet
    // can be both unicast and require replication also!
    bool unicast = pkt->i2qing_metadata()->is_egress_uc();

    RMT_ASSERT(unicast || replicate);

    // If the packet is to be unicasted get the destination port and cos.
    Packet *uc_pkt = nullptr;
    int     uc_port = 0;
    if (unicast) {
      uc_pkt = pkt;
      uc_port = pkt->i2qing_metadata()->egress_uc_port();
    }

    // If replication is required, check the pipe_mask and the C2C pipe_mask
    // to know which PREs to pass the packet's handle to.
    // The pipe mask for the multicast groups is passed from the deparser and
    // the pipe mask for C2C is stored in a register in the deparser.
    // The two pipe masks are OR'ed together in the deparser and determine
    // which PREs need to work on the packet.
    Packet *mc_pkt = nullptr;
    uint8_t mc_cos = 0, pipe_mask = 0;
    if (replicate) {
      // If the packet is both UC and MC then create a copy for the PRE blocks
      // to work on.  If the packet is only MC then just pass the original to
      // the PRE blocks.
      if (uc_pkt) {
        uc_pkt = pkt->clone();
        uc_pkt->set_phv(nullptr);
        mc_pkt = pkt;
      } else {
        mc_pkt = pkt;
      }
      mc_cos = pkt->i2qing_metadata()->icos();

      pipe_mask = pkt->i2qing_metadata()->pipe_mask();
      if (!pipe_mask) {
        // The two lines below are commented out because the control plane may
        // reserve some MGIDs as "invalid MGIDs" in which case the pipeline may
        // set the mgid to these values making the packet a valid multicast
        // packet from the ASIC's perspective, but the pipemask will be zero
        // because the control plane doesn't add any members to the mgid.
        //RMT_ASSERT(pipe_mask);
      }
    }

    if (uc_pkt) {
      // Valid port numbers per pipe are 0-71, anything outside of this range
      // should be dropped.
      if (Port::get_port_num(uc_port) >= Port::kPortsPerPipe) {
        om->pkt_delete(uc_pkt);
        uc_pkt = nullptr;
      } else {
        // This func can queue on Local or Read die
        enqueue_local_read(uc_pkt, uc_port, false /*resume*/);
      }
    }
    if (mc_pkt) {
      // Count the number of bits set in the pipe mask.
      unsigned pre_cnt = 0;
      for (int i=0; i<RmtDefs::kPresTotal; ++i) {
        if (pipe_mask & (1 << i)) {
          ++pre_cnt;
        }
      }
      // For each PRE that needs to work on the packet, make a fresh copy of
      // the packet and pass it to the PRE.  Note that for the last PRE, pass
      // the original packet rather than a copy.
      for (int i=0; i<RmtDefs::kPresTotal; ++i) {
        if (pipe_mask & (1 << i)) {
          --pre_cnt;
          if (!pre_cnt) { // Last PRE, don't copy.
            bool enqueued = om->pre_get(i)->ph_in(mc_pkt, mc_cos);
            if (!enqueued) {
              // Was not able to enqueue the packet, just delete it.
              om->pkt_delete(mc_pkt);
              mc_pkt = NULL;
            }
            break;
          } else {
            Packet *pre_copy = mc_pkt->clone();
            bool enqueued = om->pre_get(i)->ph_in(pre_copy, mc_cos);
            if (!enqueued) {
              // Was not able to enqueue the packet, just delete it.
              om->pkt_delete(pre_copy);
            }
          }
        }
      }
      RMT_ASSERT(!pre_cnt);
    }
    return;
  }

  void Queueing::enqueue_and_resume(Packet *pkt) {
    enqueue(pkt); // only called from mirror*.cpp
  }


  void Queueing::enqueue_mc_copy(Packet *pkt, int port) {
    // This function can queue on Local or Read die
    enqueue_local_read(pkt, port, true /*resume*/);
  }

  void Queueing::dequeue(int pipe, int portGrp, int qid, Packet *&pkt_out) {
    RMT_ASSERT (RmtDefs::kTmPipesMax > pipe && pipe >= 0);
    RMT_ASSERT (RmtDefs::kPortGroupsPerPipe > portGrp && portGrp >= 0);
    RMT_ASSERT (RmtDefs::kQidsPerPortGroup > qid && qid >= 0);
    int pipe_qid;

    // Check if the specified Qid in the selected pipe & mac has
    // a packet. If non-empty queue, pop the first packet off.
    Packet *pkt = NULL;
    pipe_qid = (portGrp * RmtDefs::kQidsPerPortGroup) + qid;
    std::lock_guard<std::mutex> l(queue_mtxs_[pipe][pipe_qid]);
    if (!port_queues_[pipe][pipe_qid].empty()) {
      ++ts_cntr_;
      pkt = port_queues_[pipe][pipe_qid].front();
      port_queues_[pipe][pipe_qid].pop();
      pkt->qing2e_metadata()->set_egr_q_depth(port_queues_[pipe][pipe_qid].size());
      int jitter = ts_cntr_.load() % 7 ? 0 : 1;
      pkt->qing2e_metadata()->set_delay(ts_cntr_.load() - pkt->qing2e_metadata()->ing_q_ts() + jitter); // FIXME - wrap around?
    }
    pkt_out = pkt;
    return;
  }

  void Queueing::dequeue(int pipe, int portGrp, Packet *&pkt_out) {
    RMT_ASSERT (RmtDefs::kTmPipesMax > pipe && pipe >= 0);
    RMT_ASSERT (RmtDefs::kPortGroupsPerPipe > portGrp && portGrp >= 0);

    // Check if the specified port group in the selected pipe has
    // a packet. If there is a non-empty queue, pop the first packet off.
    Packet *pkt = NULL;
    for (int qid=0; !pkt && qid<RmtDefs::kQidsPerPortGroup; ++qid) {
      dequeue(pipe, portGrp, qid, pkt);
    }
    pkt_out = pkt;
    return;
  }

  void Queueing::dequeue(int pipe, Packet *&pkt_out) {
    RMT_ASSERT (RmtDefs::kTmPipesMax > pipe && pipe >= 0);

    // Search the port groups in the requested pipe for a non-empty queue and pop
    // the first packet off.
    Packet *pkt = NULL;
    for (int portGrp=0; !pkt && portGrp<RmtDefs::kPortGroupsPerPipe; portGrp++) {
      dequeue(pipe, portGrp, pkt);
    }
    pkt_out = pkt;
    return;
  }

  void Queueing::dequeue(Packet *&pkt_out) {
    Packet *pkt = NULL;
    // Check if any pipe has a packet. If there is a non-empty queue,
    // pop the first packet off.
    for (int p=0; !pkt && p<kQGroups; ++p) {
      dequeue(p, pkt);
    }
    pkt_out = pkt;
    return;
  }

  bool Queueing::queueHasPackets(int pipe, int portGrp, int qid) {
    RMT_ASSERT (RmtDefs::kTmPipesMax > pipe && pipe >= 0);
    RMT_ASSERT (RmtDefs::kPortGroupsPerPipe > portGrp && portGrp >= 0);
    RMT_ASSERT (RmtDefs::kQidsPerPortGroup > qid && qid >= 0);

    int pipe_qid = (portGrp * RmtDefs::kQidsPerPortGroup) + qid;
    return !port_queues_[pipe][pipe_qid].empty();
  }

  bool Queueing::portGrpHasPackets(int pipe, int portGrp) {
    RMT_ASSERT (RmtDefs::kTmPipesMax > pipe && pipe >= 0);
    RMT_ASSERT (RmtDefs::kPortGroupsPerPipe > portGrp && portGrp >= 0);

    for (int q=0; q<RmtDefs::kQidsPerPortGroup; ++q) {
      if (queueHasPackets(pipe, portGrp, q)) return true;
    }
    return false;
  }

  bool Queueing::pipeHasPackets(int pipe) {
    RMT_ASSERT (RmtDefs::kTmPipesMax > pipe && pipe >= 0);

    for (int portGrp=0; portGrp<RmtDefs::kPortGroupsPerPipe; portGrp++) {
      if (portGrpHasPackets(pipe, portGrp)) return true;
    }
    return false;
  }

  bool Queueing::hasPackets() {
    for (int g=0; g<kQGroups; ++g) {
      if (pipeHasPackets(g)) return true;
    }
    return false;
  }

  void Queueing::add_to_queue(Packet *pkt, int port) {
    RMT_ASSERT(pkt);
    RMT_ASSERT(port >= 0 && port < RmtDefs::kPortsTotal);
    RmtObjectManager * const om = get_object_manager();
    bool dod_test_mode = ((om->chip()->GetFlags() & RMT_FLAGS_DOD_TEST_MODE) != 0u);

    // Mask off pipe id to get port number.
    uint8_t masked_port = Port::get_port_num(port);
    uint8_t pipe = Port::get_pipe_num(port);
    RMT_ASSERT(pipe < RmtDefs::kTmPipesMax);
    RMT_ASSERT(masked_port < Port::kPortsPerPipeMax); // Allow 72 for mirrored pkt

    // Save the length of the packet as it is stored in the TM.  This will be
    // used to set the egress packet length used on EOP processing.
    pkt->qing2e_metadata()->set_len( pkt->len() );

    int queue = queue_id(pipe, masked_port, pkt->i2qing_metadata()->qid());

    // Do we have a dod_pkt? - count for dod-test-mode
    bool dod_pkt = pkt->i2qing_metadata()->dod();
    if  (dod_pkt) ++dod_pkt_counter_;

    // XXX need to simulate drop-condition in the model for testing
    // For now every 10th pkt that indicates dod is dropped in the model for
    // testing, but only if the model is run with cmdline flag --dod-test-mode
    bool     qfull       = (dod_test_mode) ?((dod_pkt_counter_ % 10) == 0) :false;
    uint32_t qfull_qsize = (dod_test_mode) ?(dod_pkt_counter_ * 100u) :0;

    if (dod_pkt && qfull)
    {
        // defd_quid and defd_port are configured in
        // TM register - qac_reg.pipe_config - since these registers are
        // not modelled/instantiated.. just use qid = 0  and port = 0
        // for functional model

        pkt->qing2e_metadata()->set_ing_q_depth( qfull_qsize );

        queue = 0;
        pipe  = 0;
        pkt->qing2e_metadata()->set_ing_q_ts(ts_cntr_.load());
        pkt->qing2e_metadata()->set_egress_port(0);
        // set deflection flag in q2egress and remove dod flag in ingress2q
        // meta
        pkt->qing2e_metadata()->set_redc(1);
        pkt->i2qing_metadata()->set_dod(0);
        pkt->i2qing_metadata()->set_qid(queue);

        RMT_LOG_OBJ(om, RmtDebug::warn(), "QFull & DoD - deflecting packet to port 0\n");
    } else {
        pkt->qing2e_metadata()->set_ing_q_depth(port_queues_[pipe][queue].size());
        pkt->qing2e_metadata()->set_ing_q_ts(ts_cntr_.load());
        pkt->qing2e_metadata()->set_egress_port(port);
    }
    // RmtPacketCoordinator multi-threaded packet processing changes.
    // - tell RPC that a packet has been queued in TM
    // XXX: tell RPC what pipe packet will egress on
    om->packet_coordinator_get()->enqueue_from_tm(pkt, pipe);

    std::lock_guard<std::mutex> l(queue_mtxs_[pipe][queue]);
    port_queues_[pipe][queue].push(pkt);
    return;
  }


  // Following funcs to handle multi-die functionality on WIP
  //
  void Queueing::enqueue_local(Packet *pkt, int port, bool resume) {
    RmtObjectManager * const om = get_object_manager();
    RMT_ASSERT(om != nullptr);
    RMT_ASSERT(Port::get_port_num(port) < Port::kPortsPerPipe);
    RMT_ASSERT(Port::get_die_num(port) == static_cast<int>(om->chip()->GetMyDieId()));
    Port *p = om->port_lookup(Port::get_die_local_port_index(port));
    RMT_ASSERT((p != nullptr) && "No Port object for egress port number");
    pkt->set_port(p);
    add_to_queue(pkt, Port::get_die_local_port_index(port));
  }
  // void Queueing::enqueue_read(Packet *pkt, int port)
  // is a CHIP_SPECIFIC function in src/tofinoXX_jbay/ or src/rsvd0_shared/
  // Tofino/JBay versions just assert. WIP dispatches to ReadChip.
  //
  void Queueing::enqueue_local_read(Packet *pkt, int port, bool resume) {
    RmtObjectManager * const om = get_object_manager();
    RMT_ASSERT(om != nullptr);
    int my_die_id = static_cast<int>(om->chip()->GetMyDieId());
    int rd_die_id = static_cast<int>(om->chip()->GetReadDieId());
    int port_die_id = Port::get_die_num(port);
    if      (port_die_id == my_die_id) enqueue_local(pkt, port, resume);
    else if (port_die_id == rd_die_id) enqueue_read(pkt, port);
  }


}
