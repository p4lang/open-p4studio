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

#include <packet-replication-engine-reg.h>
#include <packet-replication-engine.h>
#include <rmt-object-manager.h>
#include <indirect-addressing.h>
#include <register_adapters.h>
#include <model_core/log-buffer.h>



namespace MODEL_CHIP_NAMESPACE {
  PacketReplicationEngineRegPipe::PacketReplicationEngineRegPipe(RmtObjectManager *om,
                                                                 int pipeIndex,
                                                                 PacketReplicationEngine *pre) :
                              PipeObject(om, pipeIndex),
                              pre_(pre),
                              ctrl_(packet_replication_engine_adapter(ctrl_,chip_index(), pipeIndex)),
                              arb_ctrl_(packet_replication_engine_adapter(arb_ctrl_,chip_index(), pipeIndex)),
                              wrr_ctrl_(packet_replication_engine_adapter(wrr_ctrl_,chip_index(), pipeIndex)),
                              fifo0_depth_(packet_replication_engine_adapter(fifo0_depth_,chip_index(), pipeIndex, 0)),
                              fifo1_depth_(packet_replication_engine_adapter(fifo1_depth_,chip_index(), pipeIndex, 1)),
                              fifo2_depth_(packet_replication_engine_adapter(fifo2_depth_,chip_index(), pipeIndex, 2)),
                              fifo3_depth_(packet_replication_engine_adapter(fifo3_depth_,chip_index(), pipeIndex, 3)),
                              max_l1_node_ctrl_(packet_replication_engine_adapter(max_l1_node_ctrl_,chip_index(), pipeIndex)),
                              max_l2_node_ctrl_(packet_replication_engine_adapter(max_l2_node_ctrl_,chip_index(), pipeIndex)),
                              rdm_ctrl_(packet_replication_engine_adapter(rdm_ctrl_,chip_index(), pipeIndex, [this](){this->pipe_wcb();}, [this](){this->pipe_rcb();})),
                              filter_ctrl_(packet_replication_engine_adapter(filter_ctrl_,chip_index(), pipeIndex)),
                              filter_mask_(packet_replication_engine_adapter(filter_mask_,chip_index(), pipeIndex)),
                              rdm_addr_ctrl_(packet_replication_engine_adapter(rdm_addr_ctrl_,chip_index(), pipeIndex)),
                              int_stat_(packet_replication_engine_adapter(int_stat_,chip_index(), pipeIndex)),
                              int_en0_(packet_replication_engine_adapter(int_en0_,chip_index(), pipeIndex)),
                              int_en1_(packet_replication_engine_adapter(int_en1_,chip_index(), pipeIndex)),
                              int_inj_(packet_replication_engine_adapter(int_inj_,chip_index(), pipeIndex)),
                              fifo0_ph_count_(packet_replication_engine_adapter(fifo0_ph_count_,chip_index(), pipeIndex, 0)),
                              fifo1_ph_count_(packet_replication_engine_adapter(fifo1_ph_count_,chip_index(), pipeIndex, 1)),
                              fifo2_ph_count_(packet_replication_engine_adapter(fifo2_ph_count_,chip_index(), pipeIndex, 2)),
                              fifo3_ph_count_(packet_replication_engine_adapter(fifo3_ph_count_,chip_index(), pipeIndex, 3)),
                              table0_ph_count_(packet_replication_engine_adapter(table0_ph_count_,chip_index(), pipeIndex, 0)),
                              table1_ph_count_(packet_replication_engine_adapter(table1_ph_count_,chip_index(), pipeIndex, 1)),
                              cpu_copies_(packet_replication_engine_adapter(cpu_copies_,chip_index(), pipeIndex)),
                              ph_processed_(packet_replication_engine_adapter(ph_processed_,chip_index(), pipeIndex, register_classes::PreCtr48PhProcessedMutable::kPhProcessed)),
                              total_copies_(packet_replication_engine_adapter(total_copies_,chip_index(), pipeIndex, register_classes::PreCtr48TotalCopiesMutable::kTotalCopies)),
                              xid_prunes_(packet_replication_engine_adapter(xid_prunes_,chip_index(), pipeIndex, register_classes::PreCtr48XidPrunesMutable::kXidPrunes)),
                              yid_prunes_(packet_replication_engine_adapter(yid_prunes_,chip_index(), pipeIndex, register_classes::PreCtr48YidPrunesMutable::kYidPrunes)),
                              filtered_ph_processed_(packet_replication_engine_adapter(filtered_ph_processed_,chip_index(), pipeIndex, register_classes::PreCtr48PhProcessedMutable::kFilteredPhProcessed)),
                              filtered_total_copies_(packet_replication_engine_adapter(filtered_total_copies_,chip_index(), pipeIndex, register_classes::PreCtr48TotalCopiesMutable::kFilteredTotalCopies)),
                              filtered_xid_prunes_(packet_replication_engine_adapter(filtered_xid_prunes_,chip_index(), pipeIndex, register_classes::PreCtr48XidPrunesMutable::kFilteredXidPrunes)),
                              filtered_yid_prunes_(packet_replication_engine_adapter(filtered_yid_prunes_,chip_index(), pipeIndex, register_classes::PreCtr48YidPrunesMutable::kFilteredYidPrunes)),
                              filtered_port_vector_(packet_replication_engine_adapter(filtered_port_vector_,chip_index(), pipeIndex)),
                              rdm_ph_log0_(packet_replication_engine_adapter(rdm_ph_log0_,chip_index(), pipeIndex, 0)),
                              rdm_ph_log1_(packet_replication_engine_adapter(rdm_ph_log1_,chip_index(), pipeIndex, 1)),
                              rdm_ph_log2_(packet_replication_engine_adapter(rdm_ph_log2_,chip_index(), pipeIndex, 2)),
                              rdm_ph_log3_(packet_replication_engine_adapter(rdm_ph_log3_,chip_index(), pipeIndex, 3)),
                              rdm_ph_log4_(packet_replication_engine_adapter(rdm_ph_log4_,chip_index(), pipeIndex, 4)),
                              ph_lost_(packet_replication_engine_adapter(ph_lost_,chip_index(), pipeIndex)),
                              packet_drop_(packet_replication_engine_adapter(packet_drop_,chip_index(), pipeIndex)),
                              max_l1_node_log_(packet_replication_engine_adapter(max_l1_node_log_,chip_index(), pipeIndex)),
                              max_l2_node_log_(packet_replication_engine_adapter(max_l2_node_log_,chip_index(), pipeIndex)),
                              illegal_l1_node_log_(packet_replication_engine_adapter(illegal_l1_node_log_,chip_index(), pipeIndex)),
                              illegal_l2_node_log_(packet_replication_engine_adapter(illegal_l2_node_log_,chip_index(), pipeIndex)),
                              sbe_log_(packet_replication_engine_adapter(sbe_log_,chip_index(), pipeIndex)),
                              mbe_log_(packet_replication_engine_adapter(mbe_log_,chip_index(), pipeIndex))

  {
    RMT_ASSERT(pre);
    RMT_ASSERT(pipeIndex == pre->get_pre_pipe_index());
  }

  void PacketReplicationEngineRegPipe::reset() {
    rdm_change_pending_ = false;

    ctrl_.reset();
    arb_ctrl_.reset();
    wrr_ctrl_.reset();
    fifo0_depth_.reset();
    fifo1_depth_.reset();
    fifo2_depth_.reset();
    fifo3_depth_.reset();
    max_l1_node_ctrl_.reset();
    max_l2_node_ctrl_.reset();
    rdm_ctrl_.reset();
    filter_ctrl_.reset();
    filter_mask_.reset();
    rdm_addr_ctrl_.reset();
    int_stat_.reset();
    int_en0_.reset();
    int_en1_.reset();
    int_inj_.reset();
    fifo0_ph_count_.reset();
    fifo1_ph_count_.reset();
    fifo2_ph_count_.reset();
    fifo3_ph_count_.reset();
    table0_ph_count_.reset();
    table1_ph_count_.reset();
    cpu_copies_.reset();
    ph_processed_.reset();
    total_copies_.reset();
    xid_prunes_.reset();
    yid_prunes_.reset();
    filtered_ph_processed_.reset();
    filtered_total_copies_.reset();
    filtered_xid_prunes_.reset();
    filtered_yid_prunes_.reset();
    filtered_port_vector_.reset();
    ph_lost_.reset();
    rdm_ph_log0_.reset();
    rdm_ph_log1_.reset();
    rdm_ph_log2_.reset();
    rdm_ph_log3_.reset();
    rdm_ph_log4_.reset();
    packet_drop_.reset();
    max_l1_node_log_.reset();
    max_l2_node_log_.reset();
    illegal_l1_node_log_.reset();
    illegal_l2_node_log_.reset();
    sbe_log_.reset();
    mbe_log_.reset();
  }

  void PacketReplicationEngineRegPipe::pipe_wcb() {
    if (!rdm_change_pending_ && rdm_ctrl_.rdm_change()) {
      // Assume host interface has written rdm_change from 0 to 1.
      rdm_change_pending_ = true;
      pre_->start_rdm_change_tracking();
    }
  }

  void PacketReplicationEngineRegPipe::rdm_change_done() {
    rdm_change_pending_ = false;
    rdm_ctrl_.rdm_change(0);
  }




  //
  // Memories
  //

  // RDM
  PacketReplicationEngineMemRdm::PacketReplicationEngineMemRdm(RmtObjectManager *om, PacketReplicationEngineRegCom *com) :
                              RmtObject(om),
                              RegisterBlockIndirect(chip_index(),
                                                    BFN_MEM_TM_PRE(rdm_mem_word_address) / 16,
                                                    BFN_MEM_TM_PRE_RDM_SZ,
                                                    false,
                                                    nullptr,
                                                    nullptr,
                                                    "PRE RDM Memory"),
                              com_(com)
  {
  }

  bool PacketReplicationEngineMemRdm::write(uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T) {
    std::array<uint64_t, 2> temp = {{data0, data1}};
    BitVector<128> data(temp);
    int index = offset;
    com_->write_rdm(index, data);
    return true;
  }
  bool PacketReplicationEngineMemRdm::read(uint64_t offset, uint64_t *data0, uint64_t *data1, uint64_t T) const {
    int index = offset;
    uint64_t hi=0, lo=0;
    com_->rdm()->to_bits(index, hi, lo);
    *data1 = hi;
    *data0 = lo;
    return true;
  }
  // MIT
  PacketReplicationEngineMemMit::PacketReplicationEngineMemMit(RmtObjectManager *om, int pre_idx, PacketReplicationEngineRegCom *com) :
                              RmtObject(om),
                              RegisterBlockIndirect(chip_index(), MemUtils::pre_mit_mem_address(pre_idx) / 16,
                                                    MemUtils::pre_mit_mem_size(),
                                                    false,
                                                    nullptr,
                                                    nullptr,
                                                    "PRE MIT Memory"),
                              idx_(pre_idx),
                              com_(com)
  {
  }

  bool PacketReplicationEngineMemMit::write(uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T) {
    int index = 4 * offset;
    com_->write_mit_row(idx_, index, data0 & 0x000FFFFF,
                        (data0 >> 20) & 0x000FFFFF,
                        (data0 >> 40) & 0x000FFFFF,
                        ((data0 >> 60) & 0xF) | ((data1 << 4) & 0x000FFFF0));
    return true;
  }
  bool PacketReplicationEngineMemMit::read(uint64_t offset, uint64_t *data0, uint64_t *data1, uint64_t T) const {
    int index = 4 * offset;
    uint64_t hi=0, lo=0, x=0;
    x = com_->mit(idx_, index);
    lo = x;
    x = com_->mit(idx_, index+1);
    lo |= (x << 20);
    x = com_->mit(idx_, index+2);
    lo |= (x << 40);
    x = com_->mit(idx_, index+3);
    lo |= (x << 60);
    hi = x >> 4;
    *data1 = hi;
    *data0 = lo;
    return true;
  }
  // PBT
  PacketReplicationEngineMemPbt::PacketReplicationEngineMemPbt(RmtObjectManager *om, int ver, PacketReplicationEngineRegCom *com) :
                              RmtObject(om),
                              RegisterBlockIndirect(chip_index(),
                                                    ((0 == ver) ? BFN_MEM_TM_PRE(pbt0_mem_word_address) :
                                                                  BFN_MEM_TM_PRE(pbt1_mem_word_address)) / 16,
                                                    MemUtils::pre_pbt_mem_size(),
                                                    false,
                                                    nullptr,
                                                    nullptr,
                                                    "PRE PBT Memory"),
                              ver_(ver),
                              com_(com)
  {
  }

  bool PacketReplicationEngineMemPbt::write(uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T) {
    int index = offset;
    com_->write_bpt(ver_, index, data0 & MemUtils::pre_pbt_entry_mask());
    return true;
  }
  bool PacketReplicationEngineMemPbt::read(uint64_t offset, uint64_t *data0, uint64_t *data1, uint64_t T) const {
    int index = offset;
    uint64_t hi=0, lo=0;
    lo = com_->bpt(ver_, index);
    *data1 = hi;
    *data0 = lo;
    return true;
  }
  // LAG NP
  PacketReplicationEngineMemLagNp::PacketReplicationEngineMemLagNp(RmtObjectManager *om, int ver, PacketReplicationEngineRegCom *com) :
                                   RmtObject(om),
                                   RegisterBlockIndirect(chip_index(),
                                                         ((0 == ver) ? BFN_MEM_TM_PRE(lit0_np_mem_word_address) :
                                                                       BFN_MEM_TM_PRE(lit1_np_mem_word_address)) / 16,
                                                         MemUtils::pre_lit_np_mem_size(),
							 false,
							 nullptr,
							 nullptr,
							 "PRE LAG NP Memory"),
                              ver_(ver),
                              com_(com)
  {
  }

  bool PacketReplicationEngineMemLagNp::write(uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T) {
    int index = offset;
    com_->set_lag_np(ver_, index, data0 & 0x3FFFFFF);
    return true;
  }
  bool PacketReplicationEngineMemLagNp::read(uint64_t offset, uint64_t *data0, uint64_t *data1, uint64_t T) const {
    int index = offset;
    uint64_t hi=0, lo=0, x=0;
    x = com_->lag_npr(ver_, index);
    lo = x;
    x = com_->lag_npl(ver_, index);
    lo |= x << 13;
    *data1 = hi;
    *data0 = lo;
    return true;
  }
  // LAG Membership
  PacketReplicationEngineMemLag::PacketReplicationEngineMemLag(RmtObjectManager *om, int ver, int seg, PacketReplicationEngineRegCom *com) :
                                 RmtObject(om),
                                 RegisterBlockIndirect(chip_index(), MemUtils::pre_lit_mem_address(ver,seg) / 16,
                                                       MemUtils::pre_lit_mem_size(),
                                                       false,
                                                       nullptr,
                                                       nullptr,
                                                       "PRE LAG Memory"),
                                 ver_(ver),
                                 seg_(seg),
                                 com_(com)
  {
  }

  bool PacketReplicationEngineMemLag::write(uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T) {
    int index = offset;
    com_->write_lag(ver_, index, seg_, data1 & 0xFF, data0);
    return true;
  }
  bool PacketReplicationEngineMemLag::read(uint64_t offset, uint64_t *data0, uint64_t *data1, uint64_t T) const {
    int index = offset;
    if (RmtDefs::kTmPortsPerPipe > 64) {
      *data0 = com_->lag(ver_, index).get_word(seg_ * RmtDefs::kTmPortsPerPipe, 64);
      *data1 = com_->lag(ver_, index).get_word(seg_ * RmtDefs::kTmPortsPerPipe + 64, 8);
    } else {
      *data0 = com_->lag(ver_, index).get_word(seg_ * RmtDefs::kTmPortsPerPipe, RmtDefs::kTmPortsPerPipe);
      *data1 = 0;
    }
    return true;
  }
  // PMT
  PacketReplicationEngineMemPmt::PacketReplicationEngineMemPmt(RmtObjectManager *om, int ver, int seg, PacketReplicationEngineRegCom *com) :
                                 RmtObject(om),
                                 RegisterBlockIndirect(chip_index(), MemUtils::pre_pmt_mem_address(ver,seg) / 16,
                                                       MemUtils::pre_pmt_mem_size(),
                                                       false,
                                                       nullptr,
                                                       nullptr,
                                                       "PRE PMT Memory"),
                                 ver_(ver),
                                 seg_(seg),
                                 com_(com)
  {
  }

  bool PacketReplicationEngineMemPmt::write(uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T) {
    int index = offset;
    com_->write_pmt(ver_, index, seg_, data1 & 0xFF, data0);
    return true;
  }
  bool PacketReplicationEngineMemPmt::read(uint64_t offset, uint64_t *data0, uint64_t *data1, uint64_t T) const {
    int index = offset;
    if (RmtDefs::kTmPortsPerPipe > 64) {
      *data0 = com_->pmt(ver_, index).get_word(seg_ * RmtDefs::kTmPortsPerPipe, 64);
      *data1 = com_->pmt(ver_, index).get_word(seg_ * RmtDefs::kTmPortsPerPipe + 64, 8);
    } else {
      *data0 = com_->pmt(ver_, index).get_word(seg_ * RmtDefs::kTmPortsPerPipe, RmtDefs::kTmPortsPerPipe);
      *data1 = 0;
    }
    return true;
  }
  // FIFO
  PacketReplicationEngineMemFifo::PacketReplicationEngineMemFifo(RmtObjectManager *om, PacketReplicationEngineRegCom *com) :
                                 RmtObject(om),
                                 RegisterBlockIndirect(chip_index(),
                                                       BFN_MEM_TM_PRE_FIFO_ADDR / 16,
                                                       BFN_MEM_TM_PRE_FIFO_ESZ * BFN_MEM_TM_PRE_FIFO_CNT / 16,
                                                       false,
                                                       nullptr,
                                                       nullptr,
                                                       "PRE FIFO Memory"),
                                 com_(com)
  {
  }

  bool PacketReplicationEngineMemFifo::write(uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T) {
    int index = offset;
    com_->fifo_base(index, (data0 >> 37) & 0x3FF );
    com_->fifo_rows(index, (data0 >> 26) & 0x7FF );
    return true;
  }
  bool PacketReplicationEngineMemFifo::read(uint64_t offset, uint64_t *data0, uint64_t *data1, uint64_t T) const {
    //int index = offset;
    uint64_t base = com_->fifo_base(offset);
    uint64_t rows = com_->fifo_rows(offset);
    *data0 = (base << 37) | (rows << 26);
    *data1 = 0;
    return true;
  }




  PacketReplicationEngineRegCom::PacketReplicationEngineRegCom(RmtObjectManager *om) :
                              RmtObject(om),
                              mem_rdm_(om, this),
                              mem_fifo_(om, this),
                              common_ctrl_(chip_index()),
                              prune_rid_(chip_index()),
                              port_mask0_(chip_index(), 0),
                              port_mask1_(chip_index(), 1),
                              port_down_(chip_index()),
                              pipe_int_status_(chip_index()),
                              rdm_blk_id_(chip_index())
  {
    // MIT
    for (uint32_t mit_cnt=0; mit_cnt < MemUtils::pre_mit_mem_cnt(); mit_cnt++) {
      mem_mit_[mit_cnt] = new PacketReplicationEngineMemMit(om, mit_cnt, this);
    }
    // PBT
    for (uint32_t pbt_cnt=0; pbt_cnt < MemUtils::pre_pbt_mem_cnt(); pbt_cnt++) {
      mem_pbt_[pbt_cnt] = new PacketReplicationEngineMemPbt(om, pbt_cnt, this);
    }
    // LAG_NP
    for (uint32_t lag_np_cnt=0; lag_np_cnt < MemUtils::pre_lag_np_mem_cnt(); lag_np_cnt++) {
      mem_lag_np_[lag_np_cnt] = new PacketReplicationEngineMemLagNp(om, lag_np_cnt, this);
    }
    // LAG
    for (uint32_t lag_cnt=0; lag_cnt < MemUtils::pre_lag_mem_cnt(); lag_cnt++) {
      mem_lag0_[lag_cnt] = new PacketReplicationEngineMemLag(om, 0, lag_cnt, this);
      mem_lag1_[lag_cnt] = new PacketReplicationEngineMemLag(om, 1, lag_cnt, this);
    }
    // PMT
    for (uint32_t pmt_cnt=0; pmt_cnt < MemUtils::pre_pmt_mem_cnt(); pmt_cnt++) {
      mem_pmt0_[pmt_cnt] = new PacketReplicationEngineMemPmt(om, 0, pmt_cnt, this);
      mem_pmt1_[pmt_cnt] = new PacketReplicationEngineMemPmt(om, 1, pmt_cnt, this);
    }
  }

  PacketReplicationEngineRegCom::~PacketReplicationEngineRegCom() {
    // Free MIT Objects
    for (uint32_t mit_cnt=0; mit_cnt < MemUtils::pre_mit_mem_cnt(); mit_cnt++) {
      delete mem_mit_[mit_cnt];
      mem_mit_[mit_cnt] = NULL;
    }

    // Free PBT Objects
    for (uint32_t pbt_cnt=0; pbt_cnt < MemUtils::pre_pbt_mem_cnt(); pbt_cnt++) {
      delete mem_pbt_[pbt_cnt];
      mem_pbt_[pbt_cnt] = NULL;
    }

    // Free LAG_NP Objects
    for (uint32_t lag_np_cnt=0; lag_np_cnt < MemUtils::pre_lag_np_mem_cnt(); lag_np_cnt++) {
      delete mem_lag_np_[lag_np_cnt];
      mem_lag_np_[lag_np_cnt] = NULL;
    }

    // Free LAG Objects
    for (uint32_t lag_cnt=0; lag_cnt < MemUtils::pre_lag_mem_cnt(); lag_cnt++) {
      delete mem_lag0_[lag_cnt];
      mem_lag0_[lag_cnt] = NULL;
      delete mem_lag1_[lag_cnt];
      mem_lag1_[lag_cnt] = NULL;
    }

    // Free PMT Objects
    for (uint32_t pmt_cnt=0; pmt_cnt < MemUtils::pre_pmt_mem_cnt(); pmt_cnt++) {
      delete mem_pmt0_[pmt_cnt];
      mem_pmt0_[pmt_cnt] = NULL;
      delete mem_pmt1_[pmt_cnt];
      mem_pmt1_[pmt_cnt] = NULL;
    }
  }

  void PacketReplicationEngineRegCom::reset() {
    for (auto &x : mit_)
      for (auto &y : x)
        y = 0;
    for (auto &x : bpt_)
      for (auto &y : x)
        y = 0;
    for (auto &x : lag_)
      for (auto &y : x)
        y.fill_all_zeros();
    for (auto &x : pmt_)
      for (auto &y : x)
        y.fill_all_zeros();

    common_ctrl_.reset();
    prune_rid_.reset();
    port_mask0_.reset();
    port_mask1_.reset();
    port_down_.reset();
    pipe_int_status_.reset();
    rdm_blk_id_.reset();
  }

  bool PacketReplicationEngineRegCom::validate_rdm_block_access(int pipe, int addr) {
    int blk = rdm()->addr_to_blk(addr);
    int id = rdm_blk_id_.id(blk);
    return pipe == id;
  }
  void PacketReplicationEngineRegCom::set_pmt_bit(uint32_t ver, uint32_t yid, uint32_t bit, uint32_t val) {
    RMT_ASSERT(0 == ver || 1 == ver);
    RMT_ASSERT(yid < MemUtils::pre_yids());
    RMT_ASSERT(bit < MemUtils::pre_pmt_mem_size());
    pmt_[ver][yid].set_bit(val, bit);
  }
  void PacketReplicationEngineRegCom::write_pmt(uint32_t ver, uint32_t yid, uint32_t segment, uint8_t upper, uint64_t lower) {
    RMT_ASSERT(0 == ver || 1 == ver);
    RMT_ASSERT(yid < MemUtils::pre_yids());
    RMT_ASSERT(segment <= (MemUtils::pre_pmt_mem_cnt() - 1));
    if (kLocalPortWidth > 64) {
      pmt_[ver][yid].set_word(lower, kLocalPortWidth*segment, 64);
      pmt_[ver][yid].set_word(upper, kLocalPortWidth*segment+64, 8);
    } else {
      pmt_[ver][yid].set_word(lower, kLocalPortWidth*segment, kLocalPortWidth);
    }
  }
  const BitVector<MemUtils::pre_pmt_mem_size()>& PacketReplicationEngineRegCom::pmt(uint32_t ver, uint32_t yid) const {
    RMT_ASSERT(0 == ver || 1 == ver);
    RMT_ASSERT(yid < MemUtils::pre_yids());
    return pmt_[ver][yid];
  }
  void PacketReplicationEngineRegCom::set_lag_mbr(uint32_t ver, uint32_t id, uint32_t mbr, uint32_t val) {
    RMT_ASSERT(0 == ver || 1 == ver);
    RMT_ASSERT(id <MemUtils::pre_lag_mem_size());
    RMT_ASSERT(mbr < MemUtils::pre_lag_mem_entry_size());
    RMT_ASSERT(0 == val || 1 == val);
    lag_[ver][id].set_bit(val, mbr);
  }
  void PacketReplicationEngineRegCom::set_lag(uint32_t ver, uint32_t id,
                                              const BitVector<MemUtils::pre_lag_mem_entry_size()> &data) {
    RMT_ASSERT(0 == ver || 1 == ver);
    RMT_ASSERT(id <MemUtils::pre_lag_mem_size());
    lag_[ver][id].set_from(0, data);
  }
  void PacketReplicationEngineRegCom::write_lag(uint32_t ver, uint32_t index, uint32_t segment, uint8_t upper, uint64_t lower) {
    RMT_ASSERT(0 == ver || 1 == ver);
    RMT_ASSERT(index < MemUtils::pre_lag_mem_size());
    RMT_ASSERT(segment <= (MemUtils::pre_lag_mem_cnt() - 1));
    if (kLocalPortWidth > 64) {
      lag_[ver][index].set_word(lower, kLocalPortWidth*segment, 64);
      lag_[ver][index].set_word(upper, kLocalPortWidth*segment+64, 8);
    } else {
      lag_[ver][index].set_word(lower, kLocalPortWidth*segment, kLocalPortWidth);
    }
  }
  const BitVector<MemUtils::pre_lag_mem_entry_size()>& PacketReplicationEngineRegCom::lag(uint32_t ver, uint32_t index) const {
    RMT_ASSERT(0 == ver || 1 == ver);
    RMT_ASSERT(index < MemUtils::pre_lag_mem_size());
    return lag_[ver][index];
  }
  void PacketReplicationEngineRegCom::set_lag_np(uint32_t ver, uint32_t id, uint32_t val) {
    RMT_ASSERT(0 == ver || 1 == ver);
    RMT_ASSERT(id < MemUtils::pre_lit_np_mem_size());
    RMT_ASSERT(0 == (val >> 26));
    int right = val & 0x1FFF;
    int left  = val >> 13;
    lag_npl_[ver][id] = left;
    lag_npr_[ver][id] = right;
  }
  int PacketReplicationEngineRegCom::lag_npl(uint32_t ver, uint32_t id) const {
    RMT_ASSERT(0 == ver || 1 == ver);
    RMT_ASSERT(id < MemUtils::pre_lit_np_mem_size());
    return lag_npl_[ver][id];
  }
  int PacketReplicationEngineRegCom::lag_npr(uint32_t ver, uint32_t id) const {
    RMT_ASSERT(0 == ver || 1 == ver);
    RMT_ASSERT(id < MemUtils::pre_lit_np_mem_size());
    return lag_npr_[ver][id];
  }
  Rdm* PacketReplicationEngineRegCom::rdm() const {
    return get_object_manager()->rdm_get();
  }
  void PacketReplicationEngineRegCom::write_rdm(int row, const BitVector<128> &data) {
    rdm()->encode(row, data);
  }
  int PacketReplicationEngineRegCom::bpt(uint32_t ver, uint32_t index) const {
    RMT_ASSERT(0 == ver || 1 == ver);
    RMT_ASSERT(MemUtils::pre_pbt_mem_size() > index);
    return bpt_[ver][index];
  }
  void PacketReplicationEngineRegCom::write_bpt(uint32_t ver, uint32_t index, uint32_t val) {
    RMT_ASSERT(0 == ver || 1 == ver);
    RMT_ASSERT(MemUtils::pre_pbt_mem_size() > index);
    RMT_ASSERT(val <= MemUtils::pre_pbt_mem_max_val());
    bpt_[ver][index] = val;
  }
  int PacketReplicationEngineRegCom::mit(uint32_t index, uint32_t mgid) const {
    RMT_ASSERT(MemUtils::pre_mit_mem_cnt() > index);
    RMT_ASSERT(kMitSize > mgid);
    std::lock_guard<std::mutex> lock(mit_mutex_[index]);
    return mit_[index][mgid];
  }
  void PacketReplicationEngineRegCom::write_mit(uint32_t index, uint32_t mgid, uint32_t val) {
    RMT_ASSERT(MemUtils::pre_mit_mem_cnt() > index);
    RMT_ASSERT(kMitSize > mgid);
    RMT_ASSERT(0 == (val & 0xFFF00000)); // FIXME - Use Rdm Min/Max addresses
    std::lock_guard<std::mutex> lock(mit_mutex_[index]);
    mit_[index][mgid] = val;
  }
  void PacketReplicationEngineRegCom::write_mit_row(uint32_t index, uint32_t mgid, uint32_t val0,
                                                    uint32_t val1, uint32_t val2, uint32_t val3) {
    RMT_ASSERT(MemUtils::pre_mit_mem_cnt() > index);
    RMT_ASSERT(kMitSize > mgid);
    RMT_ASSERT(0 == (mgid & 0x3)); // Must be a multiple of 4.
    RMT_ASSERT(0 == (val0 & 0xFFF00000)); // FIXME - Use Rdm Min/Max addresses
    RMT_ASSERT(0 == (val1 & 0xFFF00000)); // FIXME - Use Rdm Min/Max addresses
    RMT_ASSERT(0 == (val2 & 0xFFF00000)); // FIXME - Use Rdm Min/Max addresses
    RMT_ASSERT(0 == (val3 & 0xFFF00000)); // FIXME - Use Rdm Min/Max addresses
    std::lock_guard<std::mutex> lock(mit_mutex_[index]);
    mit_[index][mgid+0] = val0;
    mit_[index][mgid+1] = val1;
    mit_[index][mgid+2] = val2;
    mit_[index][mgid+3] = val3;
  }

}
