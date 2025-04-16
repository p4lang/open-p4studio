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

#include <cinttypes>
#include <rmt-object-manager.h>
#include <packet-replication-engine.h>
#include <bitvector.h>

/* TODO
 * [ ] - Queueing block queue-to-port tables
 * [ ] - Queueing block CoS to PRE input fifo table
 * [ ] - C2C pipe mask register in Queueing::enqueue
 */
namespace MODEL_CHIP_NAMESPACE {

  extern thread_local rmt_thd_info_t rmt_thd_info; // Required for logging.

  PacketReplicationEngine::PacketReplicationEngine(RmtObjectManager *om,
                                                   int pipeIndex)
    : PipeObject(om, pipeIndex),
      index_(pipeIndex),
      reg_pipe_(om, pipeIndex, this) {
    //for (unsigned ver=0; ver < bpt_.size(); ++ver) {
    //  for (unsigned port=0; port < bpt_[ver].size(); ++port) {
    //    bpt_[ver][port] = make_port(port/kLocalPortWidth, port%kLocalPortWidth);
    //  }
    //}
    reg_pipe_.reset();
    // Pipes 4-7 point to the other die
    if (pipeIndex < RmtDefs::kPipesMax)
      chip_index_ = static_cast<int>(om->chip()->GetMyDieId());
    else
      chip_index_ = static_cast<int>(om->chip()->GetReadDieId());
  }

  PacketReplicationEngine::FifoMode PacketReplicationEngine::fifo_mode(int fifo) {
    RMT_ASSERT(fifo >= 0 && fifo < kRxFifos);
    if (reg_pipe_.arb_mode(fifo)) {
      return FifoMode::kFifoModeSP;
    } else {
      return FifoMode::kFifoModeDRR;
    }
  }

  bool PacketReplicationEngine::has_cpu_port() {
    return reg_pipe_.c2c_enable();
  }
  int PacketReplicationEngine::get_cpu_port() {
    return reg_pipe_.c2c_port();
  }

  unsigned PacketReplicationEngine::copies_per_pass() {
    return reg_pipe_.l1_per_slice();
  }
  int PacketReplicationEngine::get_fifo_quantum(int fifo) {
    RMT_ASSERT(fifo >= 0 && fifo < kRxFifos);
    return reg_pipe_.wrr_weight(fifo);
  }
  int PacketReplicationEngine::max_L1_nodes() {
    return reg_pipe_.max_L1() + 1;
  }
  int PacketReplicationEngine::max_L2_nodes() {
    return reg_pipe_.max_L2() + 1;
  }
  uint16_t PacketReplicationEngine::global_rid() {
    return reg_com()->global_rid();
  }
  void PacketReplicationEngine::global_rid(uint16_t rid) {
    reg_com()->global_rid(rid);
  }
  bool PacketReplicationEngine::use_hw_mask() {
    return reg_com()->use_hw_mask();
  }
  void PacketReplicationEngine::ctl_hw_mask(bool x) {
    reg_com()->use_hw_mask(x);
  }
  bool PacketReplicationEngine::use_bpt() {
    return reg_com()->use_bpt();
  }
  void PacketReplicationEngine::ctl_bpt(bool x) {
    reg_com()->use_bpt(x);
  }
  void PacketReplicationEngine::set_sw_mask_bit(int ver, bool x, int pos) {
    RMT_ASSERT(0 == ver || 1 == ver);
    RMT_ASSERT(pos >= 0 && pos < PacketReplicationEngine::kPortWidth);
    reg_com()->set_sw_mask(ver, x, pos);
  }
  void PacketReplicationEngine::set_hw_mask_bit(bool x, int pos) {
    RMT_ASSERT(pos >= 0 && pos < kPortWidth);
    reg_com()->set_hw_mask(x, pos);
  }
  BitVector<PacketReplicationEngine::kPortWidth>& PacketReplicationEngine::get_sw_mask(int ver) {
    RMT_ASSERT(0 == ver || 1 == ver);
    if (!use_sw_mask()) {
      // Mask is disabled, zero it out before returning.
      sw_mask_.fill_all_zeros();
    } else {
      // Mask is enabled, update the shadow copy with the latest from the
      // registers before returning it.
      for (int i=0; i<kPortWidth; ++i) {
        sw_mask_.set_bit(reg_com()->get_sw_mask(ver,i), i);
      }
    }
    return sw_mask_;
  }
  BitVector<PacketReplicationEngine::kPortWidth>& PacketReplicationEngine::get_hw_mask() {
    if (!use_hw_mask()) {
      // Mask is disabled, zero it out before returning.
      hw_mask_.fill_all_zeros();
    } else {
      // Mask is enabled, update the shadow copy with the latest from the
      // registers before returning it.
      for (int i=0; i<kPortWidth; ++i) {
        hw_mask_.set_bit(reg_com()->get_hw_mask(i), i);
      }
    }
    return hw_mask_;
  }

  void PacketReplicationEngine::reset() {
    std::lock_guard<std::mutex> lock(rx_fifo_mutex_);
    reg_pipe_.reset();
    reg_com()->reset();
    tracking_rdm_change_ = false;
    for (auto &x : fifo_cnts_) x = 0;
  }

  bool PacketReplicationEngine::ph_in(Packet *pkt, int cos) {
    RMT_ASSERT(pkt);
    RMT_ASSERT(cos >= 0 && cos < 8);
    // Must require replication!
    RMT_ASSERT(pkt->i2qing_metadata()->has_mgid1() ||
           pkt->i2qing_metadata()->has_mgid2() ||
           pkt->i2qing_metadata()->cpu_needs_copy());

    // Enqueue the PH in the FIFO.  Use the cos to select the FIFO, since there
    // are 4 FIFOs and 8 CoS values use the upper two bits of CoS as the FIFO.
    return enqueue(cos >> 1, pkt);
  }


  bool PacketReplicationEngine::enqueue(int fifo, Packet *pkt) {
    RMT_ASSERT(fifo >= 0 && fifo < kRxFifos);
    RMT_ASSERT(pkt);
    bool ret = false;

    // Lock FIFO.
    std::unique_lock<std::mutex> lock(rx_fifo_mutex_);
    // Check size.
    int fifo_size = rx_fifo_[fifo].size();
    // TODO - Signal back pressure
    if (fifo_size < reg_com()->fifo_q_size(fifo)) { // Add to FIFO if space
      bool ver = pkt->i2qing_metadata()->use_yid_tbl();
      if (ver) {
        reg_pipe_.inc_tbl1_cnt();
      } else {
        reg_pipe_.inc_tbl0_cnt();
      }
      rx_fifo_[fifo].push(pkt);
      ret = true;
      rx_fifo_cv_.notify_one(); // Wake up engine
    }
    return ret;
  }

  bool PacketReplicationEngine::fifo_ready(int fifo) {
    RMT_ASSERT(fifo >= 0 && fifo < kRxFifos);
    return !rx_fifo_[fifo].empty();
  }

  bool PacketReplicationEngine::fifo_wait(bool wait=true) {
    for (auto &i : rx_fifo_) {
      if (!i.empty()) return true;
    }
    if (!wait) return false;
    std::unique_lock<std::mutex> lock(rx_fifo_mutex_);
    do {
      for (auto &i : rx_fifo_) {
        if (!i.empty()) return true;
      }
      if (!run_ || stopping_) break;
      rx_fifo_cv_.wait(lock);
    } while (run_ && !stopping_);
    return false;
  }

  void PacketReplicationEngine::fifo_head(int fifo, Packet *&pkt) {
    RMT_ASSERT(fifo >= 0 && fifo < kRxFifos);

    // Lock FIFO.
    std::lock_guard<std::mutex> lock(rx_fifo_mutex_);
    if (!rx_fifo_[fifo].empty()) {
      pkt = rx_fifo_[fifo].front();
    } else {
      pkt = nullptr;
    }
  }
  void PacketReplicationEngine::fifo_pop(int fifo) {
    RMT_ASSERT(fifo >= 0 && fifo < kRxFifos);
    std::lock_guard<std::mutex> lock(rx_fifo_mutex_);
    RMT_ASSERT(!rx_fifo_[fifo].empty());
    bool ver = rx_fifo_[fifo].front()->i2qing_metadata()->use_yid_tbl();
    if (ver) {
      reg_pipe_.dec_tbl1_cnt();
    } else {
      reg_pipe_.dec_tbl0_cnt();
    }
    rx_fifo_[fifo].pop();

    // If SW set the RDM change bit track when the counts reach zero.
    if (tracking_rdm_change_) {
      if (0 != fifo_cnts_[fifo]) {
        // This FIFO had a non-zero count, decrement its count and check if all
        // are now zero.
        --fifo_cnts_[fifo];
        bool change_done = true;
        for (auto x : fifo_cnts_) {
          change_done = change_done && !x;
        }

        if (change_done) {
          tracking_rdm_change_ = false;
          reg_pipe_.rdm_change_done();
        }
      }
    }
  }
  size_t PacketReplicationEngine::fifo_size(int fifo) {
    RMT_ASSERT(fifo >= 0 && fifo < kRxFifos);
    return rx_fifo_[fifo].size();
  }
  bool PacketReplicationEngine::fifos_empty() {
    std::lock_guard<std::mutex> lock(rx_fifo_mutex_);
    for (auto &x : rx_fifo_) {
      if (!x.empty()) return false;
    }
    return true;
  }


  void PacketReplicationEngine::set_pmt_bit(int ver, int yid, int bit_pos,
                                            int val) {
    RMT_ASSERT(ver == 0 || ver == 1);
    RMT_ASSERT(yid >= 0 && yid < kPmtSize);
    RMT_ASSERT(bit_pos >= 0 && bit_pos < kPortWidth);
    RMT_ASSERT(val == 0 || val == 1);
    reg_com()->set_pmt_bit(ver, yid, bit_pos, val);
  }
  const BitVector<PacketReplicationEngine::kPortWidth>& PacketReplicationEngine::pmt(int ver, int id) const {
    RMT_ASSERT(0 == ver || 1 == ver);
    RMT_ASSERT(id >= 0 && id < kPmtSize);
    return reg_com()->pmt(ver, id);
  }
  void PacketReplicationEngine::set_lag(int ver, int id,
                                        const BitVector<kPortWidth> &data) {
    RMT_ASSERT(0 == ver || 1 == ver);
    RMT_ASSERT(id >= 0 && id <kLags);
    reg_com()->set_lag(ver, id, data);
  }
  void PacketReplicationEngine::set_lag_mbr(int ver, int id, int mbr, int val) {
    RMT_ASSERT(0 == ver || 1 == ver);
    RMT_ASSERT(id >= 0 && id <kLags);
    RMT_ASSERT(mbr >= 0 && mbr < kPortWidth);
    RMT_ASSERT(0 == val || 1 == val);
    reg_com()->set_lag_mbr(ver, id, mbr, val);
  }
  const BitVector<PacketReplicationEngine::kPortWidth>& PacketReplicationEngine::lag(int ver, int id) const {
    RMT_ASSERT(0 == ver || 1 == ver);
    RMT_ASSERT(id >= 0 && id <kLags);
    return reg_com()->lag(ver, id);
  }

  void PacketReplicationEngine::start_rdm_change_tracking() {
    // Lock FIFOs.
    std::lock_guard<std::mutex> lock(rx_fifo_mutex_);
    // Save current FIFO sizes.
    bool all_empty = true;
    for (int i=0; i<kRxFifos; ++i) {
      fifo_cnts_[i] = rx_fifo_[i].size();
      all_empty = all_empty && (fifo_cnts_[i] == 0);
    }

    if (all_empty) {
      // Update registers.
      reg_pipe_.rdm_change_done();
    } else {
      // Set in-progress flag.
      tracking_rdm_change_ = true;
    }
  }

  void PacketReplicationEngine::one_pass() {
    RmtObjectManager * const om = get_object_manager();
    Queueing * const qing = om->queueing_get();
    // Service FIFOs in priority order, kRxFifos is highest priority, 0 is the
    // lowest.
    for (int fifo = kRxFifos-1; fifo >= 0; --fifo) {
      // The FIFO can run in one of two modes, strict priority or DRR.  In SP
      // mode process the FIFO until it runs out of packets (which may never
      // happen).  In DRR mode only process a configured quantum of packets.
      bool sp_mode = FifoMode::kFifoModeSP == fifo_mode(fifo);
      // If there is no data in the FIFO skip it.
      if (!fifo_ready(fifo)) continue;
      int credits = sp_mode ? 0 : credit_[fifo] + get_fifo_quantum(fifo);
      while (sp_mode || (!sp_mode && credits--)) {
        Packet *pkt = NULL;
        fifo_head(fifo, pkt);
        if (!pkt) break; // FIFO empty, move on to next FIFO.

        rmt_thd_info.ig_packet = pkt;

        // If C2C was requested handle it before going to the multicast tree.
        // If only C2C was requested the original packet can be used but if
        // both C2C and multicast were requested then a copy must be made.
        if (pkt->i2qing_metadata()->cpu_needs_copy() && has_cpu_port() &&
            !pkt->i2qing_metadata()->needs_mc_copy()) {
          // C2C only.
          int cpu_port = make_port(get_cpu_port());
          fifo_pop(fifo);
          RMT_P4_LOG_VERBOSE("Copy-to-CPU: redirecting to CPU port %d\n", cpu_port);
          qing->enqueue_mc_copy(pkt, cpu_port);
          continue;
        } else if (pkt->i2qing_metadata()->cpu_needs_copy() && has_cpu_port()) {
          // C2C and mcast.  Create a copy of the packet to be the CPU copy.
          Packet *cpu_copy = pkt->clone();
          int cpu_port = make_port(get_cpu_port());
          uint64_t c2c_id = cpu_copy->pkt_id_mc_c2c(pkt->pkt_id());
          RMT_P4_LOG_VERBOSE("Copy-to-CPU: replicating to CPU port %d with pkt id 0x%" PRIx64 "\n",
                          cpu_port, c2c_id);
          qing->enqueue_mc_copy(cpu_copy, cpu_port);
          // Clear the C2C flag incase this packet is put back on the PRE's FIFO.
          pkt->i2qing_metadata()->set_copy_to_cpu(false);
        }

        // Walk the multicast tree(s) for this packet.  Only process up to a
        // configured maximum number of L1 nodes.  If there are additional L1
        // nodes enqueue this packet back on the FIFO.
        unsigned max_replications = copies_per_pass();
        bool more_work = walk_table(pkt, max_replications);
        if (more_work) {
          // Remove packet from front of FIFO and place it at the back, only
          // needed if there are multiple packets, otherwise just leave the
          // packet at the front of the FIFO.
          if (1 < fifo_size(fifo)) {
            fifo_pop(fifo);
            enqueue(fifo, pkt);
          }
        } else {
          fifo_pop(fifo);
          om->pkt_delete(pkt);
        }
      }
      // In DRR mode, allow unused credits to carry over.
      if (!sp_mode) credit_[fifo] = credits;
    }
  }


  void PacketReplicationEngine::run() {
    while (run_ && !stopping_) {
      fifo_wait();
      one_pass();
    }
    bool packets_waiting = fifo_wait(false);
    while (stopping_ && packets_waiting) {
      one_pass();
      packets_waiting = fifo_wait(false);
    }
    run_ = false;
  }


  void PacketReplicationEngine::start() {
    stopping_ = false;
    run_ = true;
    worker_ = std::thread(&PacketReplicationEngine::run, this);
  }


  void PacketReplicationEngine::stop(bool right_now=true) {
    stopping_ = true;
    run_ = !right_now;
    if (worker_.joinable()) {
      std::unique_lock<std::mutex> lock(rx_fifo_mutex_);
      rx_fifo_cv_.notify_all();
      lock.unlock();
      worker_.join();
    }
  }


  bool PacketReplicationEngine::is_pruned(int ver, int pkt_rid, int node_rid,
                                          int yid, int bit) {
    RMT_ASSERT(yid >= 0 && yid < kPmtSize);
    RMT_ASSERT(ver == 0 || ver == 1);
    RMT_ASSERT(bit >= 0 && bit < kPortWidth);

    bool pruned = false;
    if (pkt_rid == global_rid() || pkt_rid == node_rid) {
      pruned = pmt(ver, yid).get_bit(bit);
    }
    return pruned;
  }
  bool PacketReplicationEngine::is_live(int ver, int pipe, int port) {
    RMT_ASSERT(0 == ver || 1 == ver);
    RMT_ASSERT(pipe >= 0 && pipe < RmtDefs::kPipesMax);
    RMT_ASSERT(port >= 0 && port < kLocalPortWidth);
    int bit_pos = pipe * kLocalPortWidth + port;

    return !(get_sw_mask(ver).get_bit(bit_pos) | get_hw_mask().get_bit(bit_pos));
  }


  bool PacketReplicationEngine::generate_copy(int ver, int chip, int pipe, int port,
                                              Packet *pkt, int rid, bool rid_first) {
    RMT_ASSERT(pkt);
    RMT_ASSERT(ver == 0 || ver == 1);
    RMT_ASSERT(pipe >= 0 && pipe < RmtDefs::kPresTotal);
    RMT_ASSERT(port >= 0 && port < kLocalPortWidth);
    Queueing *qing = get_object_manager()->queueing_get();
    bool made_copy = false;

    // If using backup table, check liveness.  If down, send to backup.
    int tgt_port = -1;
    if (use_bpt() && !is_live(ver, pipe, port)) {
      // Send to the backup port without checking its liveness.  Since primary
      // is down the backup will be used no matter what.
      int bpt_val = bpt(ver, chip*kDiePortWidth + pipe*kLocalPortWidth + port);
      int bk_port_idx = MemUtils::pre_pbt_mem_entry_port_val(bpt_val);
      int bk_port_pipe = MemUtils::pre_pbt_mem_entry_pipe_val(bpt_val);
      int bk_port_chip = MemUtils::pre_pbt_mem_entry_chip_val(bpt_val);
      int bk_port = make_port(bk_port_chip, bk_port_pipe, bk_port_idx);
      tgt_port = bk_port;
    } else {
      // Not using backups, always send to original port.
      tgt_port = make_port(chip, pipe, port);
    }

    if (is_port_local(tgt_port)) {
      Packet *cpy = pkt->clone();
      uint64_t cpy_id = cpy->pkt_id_mc_cpy(pkt->pkt_id(), get_pre_pipe_index(), pkt->mc_cpy_cnt());
      if (tgt_port == make_port(chip, pipe, port)) {
        RMT_P4_LOG_VERBOSE("MCAST_GRP 0x%04x copy 0x%" PRIx64 " to port %d from node RID 0x%04x\n",
                           pkt->cur_mgid(), cpy_id, tgt_port, rid);
      } else {
        RMT_P4_LOG_VERBOSE("MCAST_GRP 0x%04x copy 0x%" PRIx64 " to bkup port %d (original port %d) from node RID 0x%04x\n",
			   pkt->cur_mgid(), cpy_id, tgt_port, make_port(chip, pipe, port), rid);
      }
      cpy->qing2e_metadata()->set_erid(rid);
      cpy->qing2e_metadata()->set_rid_first(rid_first);
      qing->enqueue_mc_copy(cpy, tgt_port);
      made_copy = true;
    }
    return made_copy;
  }

  void PacketReplicationEngine::get_lag_member(uint16_t hash,
                                               int ver,
                                               int id,
                                               const BitVector<kPortWidth> &lag,
                                               const BitVector<kPortWidth> &sw_mask,
                                               const BitVector<kPortWidth> &hw_mask,
                                               int &mbr_a,
                                               int &mbr_b,
                                               model_core::LogBuffer& lag_buf) {
    mbr_a = -1;
    mbr_b = -1;
    // This function uses the builtin popcount funtion which expects an
    // unsigned long.  Assert that an unsigned long is 64 bits.
    RMT_ASSERT(sizeof(uint64_t) == sizeof(unsigned long long));
    uint64_t mask[kLagArrSize] = {}; // Combined HW and SW mask
    uint64_t mbrs_masked[kLagArrSize] = {}; // LAG membership after mask is applied
    uint64_t mbrs[kLagArrSize] = {}; // LAG membership without liveness mask
    int c_masked[kLagArrSize] = {};
    int c[kLagArrSize] = {};

    // Count the number of active members in the LAG both with and without the
    // liveness mask applied.
    int len = 0, len_masked = 0, local_len = 0;
    int left = reg_com()->lag_npl(ver, id);
    int right = reg_com()->lag_npr(ver, id);
    for (int i=0; i<kLagArrSize; ++i) {
      mask[i] = ~sw_mask.get_word(64*i) & ~hw_mask.get_word(64*i);
      mbrs[i] = lag.get_word(64*i);
      mbrs_masked[i] = mask[i] & mbrs[i];
      c[i] = __builtin_popcountll(mbrs[i]);
      c_masked[i] = __builtin_popcountll(mbrs_masked[i]);
      local_len += c[i];
      len_masked += c_masked[i];
    }
    len = local_len + left + right;
    len &= 0x1FFF;  // Keep it at 13 bits
    len_masked &= kLagLenMask; // Keep it at 9 bits


    // Compute the member index from the hash for both the case where the
    // liveness mask is applied and for the case where it is ignored.
    int idx = len ? hash % len : 0;
    int idx_masked = len_masked ? hash % len_masked : 0;

    // If there are no active members in the LAG return.
    if (0 == local_len) {
      lag_buf.Append("Hash=0x%4x Len=%d LenMasked=%d  LocalLen=%d",
              hash, len, len_masked, local_len);
      return;
    }

    // If the member is in either remote side then return.
    if (idx < right) { // Member is in remote right side
      lag_buf.Append("Hash=0x%4x Len=%d LenMasked=%d  Idx(%d) < Right(%d)",
              hash, len, len_masked, idx, right);
      return;
    } else if (idx >= (local_len + right)) { // Member is in remote left side
      lag_buf.Append("Hash=0x%4x Len=%d LenMasked=%d  "
              "Idx(%d) >= LocalLen(%d)+Right(%d)",
              hash, len, len_masked, idx, local_len, right);
      return;
    }

    // Pick the member without considering liveness first.
    int tgt_port_bit_pos = 0;
    int ports_seen = -1;
    for ( ; tgt_port_bit_pos<kPortWidth; ++tgt_port_bit_pos) {
      if ((mbrs[tgt_port_bit_pos/64] >> (tgt_port_bit_pos%64)) & 1) {
        ++ports_seen;
        if (ports_seen == (idx-right)) {
          break;
        }
      }
    }
    RMT_ASSERT(tgt_port_bit_pos < kPortWidth);
    mbr_a = tgt_port_bit_pos;

    // Pick the member considering liveness next, but only if there are some
    // members to select from.
    if (len_masked) {
      tgt_port_bit_pos = 0;
      ports_seen = -1;
      for ( ; tgt_port_bit_pos<kPortWidth; ++tgt_port_bit_pos) {
        if ((mbrs_masked[tgt_port_bit_pos/64] >> (tgt_port_bit_pos%64)) & 1) {
          ++ports_seen;
          if (ports_seen == (idx_masked)) {
            break;
          }
        }
      }
      RMT_ASSERT(tgt_port_bit_pos < kPortWidth);
      mbr_b = tgt_port_bit_pos;
    } else {
      mbr_b = -1;
    }
    lag_buf.Append("Hash=0x%4x Len=%d LenMasked=%d Idx=%d Right=%d "
            "IdxMasked=%d Mbr=%d MbrMasked=%d",
            hash, len, len_masked, idx, right, idx_masked, mbr_a, mbr_b);
    return;
  }

int PacketReplicationEngine::lag_hash(int ver,
                                      uint16_t hash,
                                      int id,
                                      uint16_t pkt_rid,
                                      uint16_t node_rid,
                                      int yid,
                                      model_core::LogBuffer &log_buf) {
    RMT_ASSERT(ver == 0 || ver == 1);
    RMT_ASSERT(id >= 0 && id < kLags);

    // Check for empty LAGs.
    if (lag(ver, id).is_zero()) {
      log_buf.Append("LAG has no members");
      return -1;
    }

    // In the software mask, bits set to 1 means not available.
    const BitVector<kPortWidth> &sw_mask = get_sw_mask(ver);
    // In the hardware mask, bits set to 1 mean not available.
    const BitVector<kPortWidth> &hw_mask = get_hw_mask();

    // Select the LAG member with and without considering the SW and HW masks.
    model_core::LogBuffer lag_buf(kLags);
    int mbr, mbr_masked;
    get_lag_member(hash, ver, id, lag(ver, id), sw_mask, hw_mask, mbr, mbr_masked, lag_buf);

    // If there is no member return.
    if (-1 == mbr) {
      log_buf.Append("LAG hash did not select member local to device  [%s]", lag_buf.GetBuf());
      return -1;
    }

    // If no up members are available (i.e. mbr_masked isn't valid) then use
    // the original member selected.
    // Else, if the original member selected is down (either HW or SW) then
    // use the member selected after applying the masks.
    // Else use the original member.
    if (mbr_masked == -1) {
      int tgt_chip = mbr / kDiePortWidth;
      int tgt_pipe = (mbr - (tgt_chip * kDiePortWidth)) / kLocalPortWidth;
      int tgt_port = mbr % kLocalPortWidth;
      log_buf.Append("All LAG mbrs are masked/down, using %d  [%s]",
              make_port(tgt_chip, tgt_pipe, tgt_port), lag_buf.GetBuf());
    } else if (sw_mask.get_bit(mbr) || hw_mask.get_bit(mbr)) {
      int tgt_chip = mbr / kDiePortWidth;
      int tgt_pipe = (mbr - (tgt_chip * kDiePortWidth)) / kLocalPortWidth;
      int tgt_port = mbr % kLocalPortWidth;
      int act_chip = mbr_masked / kDiePortWidth;
      int act_pipe = (mbr_masked - (act_chip * kDiePortWidth)) / kLocalPortWidth;
      int act_port = mbr_masked % kLocalPortWidth;
      log_buf.Append("LAG mbr %d is masked/down, using %d [%s]",
              make_port(tgt_chip, tgt_pipe, tgt_port), make_port(act_chip, act_pipe, act_port), lag_buf.GetBuf());
      mbr = mbr_masked;
    } else {
      int tgt_chip = mbr / kDiePortWidth;
      int tgt_pipe = (mbr - (tgt_chip * kDiePortWidth)) / kLocalPortWidth;
      int tgt_port = mbr % kLocalPortWidth;
      log_buf.Append("LAG mbr %d selected  [%s]",
              make_port(tgt_chip, tgt_pipe, tgt_port), lag_buf.GetBuf());
    }

    // If the final member selected is pruned, return.
    if (is_pruned(ver, pkt_rid, node_rid, yid, mbr)) {
      int tgt_chip = mbr / kDiePortWidth;
      int tgt_pipe = (mbr - (tgt_chip * kDiePortWidth)) / kLocalPortWidth;
      int tgt_port = mbr % kLocalPortWidth;
      log_buf.Append(
              "LAG mbr %d is pruned; NodeRID 0x%04x gRID 0x%04x "
              "Pkt RID 0x%04x, L2_XID 0x%04x  [%s]",
              make_port(tgt_chip, tgt_pipe, tgt_port), node_rid, global_rid(),
              pkt_rid, yid, lag_buf.GetBuf());
      return -1;
    }

    return mbr;
  }


  bool PacketReplicationEngine::walk_table(Packet *pkt, int max_copies) {
    RMT_ASSERT(pkt);
    RMT_ASSERT(max_copies > 0);

    // Start with the saved state.
    uint32_t addr_L1 = pkt->next_L1();
    // If that is not valid, then start with the first MGID.
    if (!addr_L1 && pkt->i2qing_metadata()->has_mgid1()) {
      addr_L1 = mit(pkt->i2qing_metadata()->mgid1());
      pkt->set_cur_mgid(pkt->i2qing_metadata()->mgid1());
      pkt->i2qing_metadata()->clr_mgid1();
      pkt->set_L1_cnt(0);
    }
    // If that is not valid, try the other MGID.
    if (!addr_L1 && pkt->i2qing_metadata()->has_mgid2()) {
      addr_L1 = mit(pkt->i2qing_metadata()->mgid2());
      pkt->set_cur_mgid(pkt->i2qing_metadata()->mgid2());
      pkt->i2qing_metadata()->clr_mgid2();
      pkt->set_L1_cnt(0);
    }

    uint16_t pkt_rid = pkt->i2qing_metadata()->irid();
    uint16_t pkt_xid = pkt->i2qing_metadata()->xid();
    int pkt_ver = pkt->ph_ver();
    uint16_t hash1 = pkt->i2qing_metadata()->hash1();
    uint16_t hash2 = pkt->i2qing_metadata()->hash2();
    int yid = pkt->i2qing_metadata()->yid();

    while (addr_L1) {
      // Decode the current L1 node (including any ECMP choices).
      //Rdm::RdmNodeType type_L1 = Rdm::RdmNodeType::kInvalid;
      uint16_t rid = 0;
      uint16_t xid = 0;
      bool     rid_hash = false;
      bool     xid_valid = false;
      uint32_t addr_L2 = 0;
      uint32_t next_L1 = 0;
      bool     rid_first = true;
      uint16_t lag_hash_val = 0;

      // Increment the L1 node visited count and drop if the configured limit
      // is exceeded.
      if (pkt->l1_cnt() >= max_L1_nodes()) {
        // Save MGID and L1 address.
        reg_pipe_.log_max_L1(pkt->cur_mgid(), addr_L1);
        // TODO - Increment packet_drop.
        // TODO - Set interrupt bit.
        break;
      }
      pkt->inc_L1_cnt();

      // Verify that the RDM block we are about to access has been programmed
      // to be assigned to this pipe/PRE.
      RMT_ASSERT(reg_com()->validate_rdm_block_access(get_pre_pipe_index(), addr_L1));

      rdm()->decode_L1(addr_L1, hash1, pkt_ver, next_L1, xid, xid_valid, rid, addr_L2, rid_hash);
//printf("PRE[%d] L1 0x%x has next 0x%x\n", get_pre_pipe_index(), addr_L1, next_L1);
      addr_L1 = next_L1;

      // Include the node's RID value in the LAG hash only if the L1 node says
      // to do so.  Note for Tofino it will never say to include it.
      lag_hash_val = rid_hash ? hash2 ^ rid : hash2;
      // LAG Hash is only 13 bits.
      lag_hash_val &= 0x1FFF;

      // Prune the L2 tree if the XID is valid and it matches the packet's XID.
      if (!xid_valid || (xid != pkt_xid)) {
        // Clear the L2 counter every time a new tree is started.
        pkt->set_L2_cnt(0);
        while (addr_L2) {
          uint32_t next_L2 = 0;
          int chip = 0;
          int pipe = 0;
          uint8_t spv = 0;
          uint64_t npv = 0;
          uint8_t lag_id = 0;
          bool lag_valid = false;

          // Increment the L2 node visited count and drop if the configured
          // limit is exceeded.
          if (pkt->l2_cnt() >= max_L2_nodes()) {
            // Save MGID and L2 address.
            reg_pipe_.log_max_L2(pkt->cur_mgid(), addr_L2);
            // TODO - Increment packet_drop.
            // TODO - Set interrupt bit.
            break;
          }
          pkt->inc_L2_cnt();

          // Verify that the RDM block we are about to access has been programmed
          // to be assigned to this pipe/PRE.
          RMT_ASSERT(reg_com()->validate_rdm_block_access(get_pre_pipe_index(), addr_L2));

          rdm()->decode_L2(addr_L2, next_L2, chip, pipe, spv, npv, lag_id, lag_valid);
//printf("PRE[%d] L2 0x%x has next 0x%x\n", get_pre_pipe_index(), addr_L2, next_L2);
          addr_L2 = next_L2;

          if (lag_valid) {
            // Select a member from the LAG using the hash.  Note that this
            // function will take care of all the masking and pruning.
            model_core::LogBuffer log_buf(2048);
            int lag_mbr = lag_hash(pkt_ver, lag_hash_val, lag_id, pkt_rid, rid, yid, log_buf);
            RMT_P4_LOG_VERBOSE("MCAST_GRP 0x%04x Node RID 0x%04x LAG %d: %s\n",
                            pkt->cur_mgid(), rid, lag_id, log_buf.GetBuf());
            if (-1 == lag_mbr) {
              continue;
            }
	    int tgt_chip = lag_mbr / kDiePortWidth;
            int tgt_pipe = (lag_mbr - (tgt_chip * kDiePortWidth)) / kLocalPortWidth;
            int tgt_port = lag_mbr % kLocalPortWidth;
            bool sent = generate_copy(pkt_ver, tgt_chip, tgt_pipe, tgt_port, pkt, rid, rid_first);
            rid_first = sent ? false : rid_first;
          } else {
            int local_port = 0;
            for (int bitpos = 0; bitpos < 64; bitpos++) {
              uint64_t i = UINT64_C(1) << bitpos;
              if (npv & i) {
                int prune_bit_pos = chip * kDiePortWidth + pipe * kLocalPortWidth + local_port;
                if (!is_pruned(pkt_ver, pkt_rid, rid, yid, prune_bit_pos)) {
                  bool sent = generate_copy(pkt_ver, chip, pipe, local_port, pkt, rid, rid_first);
                  rid_first = sent ? false : rid_first;
                } else {
                  // L2 pruning
                  RMT_P4_LOG_VERBOSE("MCAST_GRP 0x%04x Lvl 2 Prune port %d from Node with RID 0x%04x (gRID 0x%04x) Pkt RID 0x%04x Pkt L2_XID 0x%04x\n",
				     pkt->cur_mgid(), make_port(chip,pipe,local_port), rid,
				     global_rid(), pkt_rid, yid);
                }
              }
              ++local_port;
            }
            for (int bitpos = 0; bitpos < 8; bitpos++) {
              uint8_t i = (uint8_t)1 << bitpos;
              if (spv & i) {
                int prune_bit_pos = chip * kDiePortWidth + pipe * kLocalPortWidth + local_port;
                if (!is_pruned(pkt_ver, pkt_rid, rid, yid, prune_bit_pos)) {
                  bool sent = generate_copy(pkt_ver, chip, pipe, local_port, pkt, rid, rid_first);
                  rid_first = sent ? false : rid_first;
                } else {
                  // L2 pruning
                  RMT_P4_LOG_VERBOSE("MCAST_GRP 0x%04x Lvl 2 Prune port %d from Node with RID 0x%04x (gRID 0x%04x) Pkt RID 0x%04x Pkt L2_XID 0x%04x\n",
				     pkt->cur_mgid(), make_port(chip,pipe,local_port), rid,
				     global_rid(), pkt_rid, yid);
                }
              }
              ++local_port;
            }
          }
        }
      } else {
        RMT_P4_LOG_VERBOSE("MCAST_GRP 0x%04x Lvl 1 Prune Node with RID 0x%04x XID 0x%04x\n",
                        pkt->cur_mgid(), rid, xid);
      }

      if (!--max_copies) goto quantum_reached;
    }

    // Will reach here after fully completing MGID1 or after fully completing
    // MGID2.  If only MGID1 has been completed then the packet must go back on
    // the FIFO for another round of processing (assuming MGID2 is valid).
    pkt->set_next_L1(0);
    return pkt->i2qing_metadata()->has_mgid2();

    quantum_reached:
    pkt->set_next_L1(addr_L1);
    return true;
  }
}
