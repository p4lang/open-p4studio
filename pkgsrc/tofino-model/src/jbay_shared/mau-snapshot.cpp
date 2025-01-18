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

// MauSnapshot - Jbay specific code

#include <mau.h>
#include <register_adapters.h>
#include <mau-snapshot.h>

namespace MODEL_CHIP_NAMESPACE {

MauSnapshot::MauSnapshot(RmtObjectManager *om, int pipeIndex, int mauIndex, Mau *mau)
    : MauSnapshotCommon(om, pipeIndex, mauIndex, mau),
      mau_snapshot_config_(default_adapter(mau_snapshot_config_,chip_index(),pipeIndex,mauIndex)),
      mau_snapshot_capture_subword32b_hi_ { {
        { default_adapter(mau_snapshot_capture_subword32b_hi_[0], chip_index(),pipeIndex,mauIndex,0 )},
        { default_adapter(mau_snapshot_capture_subword32b_hi_[1], chip_index(),pipeIndex,mauIndex,1 )} } },
      mau_snapshot_capture_subword32b_lo_ { {
        { default_adapter(mau_snapshot_capture_subword32b_lo_[0], chip_index(),pipeIndex,mauIndex,0 )},
        { default_adapter(mau_snapshot_capture_subword32b_lo_[1], chip_index(),pipeIndex,mauIndex,1 )} } },
      mau_snapshot_capture_subword16b_    { {
        { default_adapter(mau_snapshot_capture_subword16b_[0], chip_index(),pipeIndex,mauIndex,0 )},
        { default_adapter(mau_snapshot_capture_subword16b_[1], chip_index(),pipeIndex,mauIndex,1 )} } },
      mau_snapshot_capture_subword8b_     { {
        { default_adapter(mau_snapshot_capture_subword8b_[0], chip_index(),pipeIndex,mauIndex,0 )},
        { default_adapter(mau_snapshot_capture_subword8b_[1], chip_index(),pipeIndex,mauIndex,1 )} } },
      mau_snapshot_datapath_capture_(default_adapter(mau_snapshot_datapath_capture_,chip_index(),pipeIndex,mauIndex)),
      mau_snapshot_table_active_(default_adapter(mau_snapshot_table_active_,chip_index(),pipeIndex,mauIndex)),
      mau_snapshot_next_table_out_(default_adapter(mau_snapshot_next_table_out_,chip_index(),pipeIndex,mauIndex)),
      mau_snapshot_global_exec_out_(default_adapter(mau_snapshot_global_exec_out_,chip_index(),pipeIndex,mauIndex)),
      mau_snapshot_long_branch_out_(default_adapter(mau_snapshot_long_branch_out_,chip_index(),pipeIndex,mauIndex)),
      mau_snapshot_mpr_next_table_out_(default_adapter(mau_snapshot_mpr_next_table_out_,chip_index(),pipeIndex,mauIndex)),
      mau_snapshot_mpr_global_exec_out_(default_adapter(mau_snapshot_mpr_global_exec_out_,chip_index(),pipeIndex,mauIndex)),
      mau_snapshot_mpr_long_branch_out_(default_adapter(mau_snapshot_mpr_long_branch_out_,chip_index(),pipeIndex,mauIndex)),
      mau_snapshot_meter_adr_(default_adapter(mau_snapshot_meter_adr_,chip_index(),pipeIndex,mauIndex)),
      mau_snapshot_imem_logical_read_adr_(default_adapter(mau_snapshot_imem_logical_read_adr_,chip_index(),pipeIndex,mauIndex)),
      mau_snapshot_imem_logical_selector_fallback_(default_adapter(mau_snapshot_imem_logical_selector_fallback_,chip_index(),pipeIndex,mauIndex))
{
  mau_snapshot_config_.reset();
  for (int i = 0; i <= 1; i++) {
    mau_snapshot_capture_subword32b_hi_[i].reset();
    mau_snapshot_capture_subword32b_lo_[i].reset();
    mau_snapshot_capture_subword16b_[i].reset();
    mau_snapshot_capture_subword8b_[i].reset();
  }
  mau_snapshot_table_active_.reset();
  mau_snapshot_next_table_out_.reset();
  mau_snapshot_global_exec_out_.reset();
  mau_snapshot_long_branch_out_.reset();
  mau_snapshot_mpr_next_table_out_.reset();
  mau_snapshot_mpr_global_exec_out_.reset();
  mau_snapshot_mpr_long_branch_out_.reset();
  mau_snapshot_meter_adr_.reset();
  mau_snapshot_imem_logical_read_adr_.reset();
  mau_snapshot_imem_logical_selector_fallback_.reset();

  // Check mapping funcs are working as we expect
  RMT_ASSERT(RmtDefs::map_mausnap_phv_index(kPhvWordsUnmapped) == kPhvWords);
  RMT_ASSERT(RmtDefs::unmap_mausnap_phv_index(kPhvWords) == kPhvWordsUnmapped);
}
MauSnapshot::~MauSnapshot() {
}

bool MauSnapshot::is_thread_active(bool ingress, Phv *phv) {
  bool ing_en = (mau_snapshot_config_.snapshot_match_ingress_disable() == 0);
  bool ght_en = (mau_snapshot_config_.snapshot_match_ghost_disable() == 0);
  bool both   = (mau_snapshot_config_.snapshot_match_require_ingress_ghost() == 1);
  bool ing_active = (phv->ingress() && ing_en);
  bool ght_active = (phv->ghost()   && ght_en);
  bool ing_thrd_active = (both) ?(ing_active && ght_active) :(ing_active || ght_active);
  bool egr_thrd_active = phv->egress();
  return (ingress) ?ing_thrd_active :egr_thrd_active;
}
void MauSnapshot::datapath_capture(bool ingress,
                                   uint8_t from_prev, uint8_t timed, uint8_t here,
                                   uint8_t error, uint8_t ing_pktver, uint8_t eg_pktver,
                                   uint8_t thread_active, uint8_t trigger_thread,
                                   uint8_t ghost_thread_active) {
  uint8_t ie = (ingress) ?0 :1;
  mau_snapshot_datapath_capture_.snapshot_from_prev_stage(ie, from_prev);
  mau_snapshot_datapath_capture_.timebased_snapshot_trigger(ie, timed);
  mau_snapshot_datapath_capture_.snapshot_from_this_stage(ie, here);
  // TODO: disappeared in regs_28059_mau_dev: where did it go??
  //mau_snapshot_datapath_capture_.snapshot_error(ie, error);
  mau_snapshot_datapath_capture_.snapshot_ingress_pktversion(ie, ing_pktver);
  mau_snapshot_datapath_capture_.snapshot_egress_pktversion(ie, eg_pktver);
  mau_snapshot_datapath_capture_.snapshot_thread_active(ie, thread_active);
  mau_snapshot_datapath_capture_.snapshot_trigger_thread(ie, trigger_thread);
  mau_snapshot_datapath_capture_.snapshot_thread_active_ghost(ie, ghost_thread_active);
}
void MauSnapshot::next_table_capture(bool ingress) {
  // Get handle on the output I/O object for this MAU
  // XXX - capture OUTPUT info
  MauIO *outIO = mau()->mau_io_output();
  if (ingress) {
    mau_snapshot_next_table_out_.
        mau_snapshot_next_table_out(kIngress, outIO->ingress_nxt_tab());
    mau_snapshot_next_table_out_.
        mau_snapshot_next_table_out(kGhost, outIO->ghost_nxt_tab());
  } else {
    mau_snapshot_next_table_out_.
        mau_snapshot_next_table_out(kEgress, outIO->egress_nxt_tab());
  }
}
void MauSnapshot::per_chip_capture(bool ingress, Phv *phv) {
  table_active_capture(ingress);
  global_exec_capture(ingress);
  long_branch_capture(ingress);
  mpr_next_table_capture(ingress);
  mpr_global_exec_capture(ingress);
  mpr_long_branch_capture(ingress);
  meter_adr_capture(ingress);
  imem_logical_read_adr_capture(ingress);
  imem_logical_selector_fallback_capture(ingress);
}

void MauSnapshot::table_active_capture(bool ingress) {
  uint16_t actives = 0;
  for (int lt = 0; lt < kLogicalTables; lt++) {
    MauLookupResult *res = mau()->mau_lookup_result(lt);
    if (res->active()) actives |= (1<<lt);
  }
  RMT_ASSERT(actives == mau()->pred_lt_info(Pred::kActive));
  // Put ALL actives in - 61306_mau_dev_jbay
  int index = (ingress) ?kIngress :kEgress;
  mau_snapshot_table_active_.mau_snapshot_table_active(index, actives);
}
void MauSnapshot::global_exec_capture(bool ingress) {
  MauIO *outIO = mau()->mau_io_output();
  MauPredication *pred = mau()->mau_predication();
  uint16_t gex = outIO->global_exec();
  uint8_t  dummy;
  if (ingress) {
    uint16_t ing_gex = 0, ght_gex = 0;
    // Figure out what global_exec bits are ingress/ghost LTs
    pred->thread_output_info(kIngress, &ing_gex, &dummy);
    pred->thread_output_info(kGhost, &ght_gex, &dummy);
    uint16_t mask = (kSnapshotMaskThreadFields) ?ing_gex|ght_gex :0xFFFF;
    mau_snapshot_global_exec_out_.
        mau_snapshot_global_exec_out(kIngress, gex & mask);
  } else {
    uint16_t egr_gex = 0;
    // Figure out what global_exec bits are egress LTs
    pred->thread_output_info(kEgress, &egr_gex, &dummy);
    uint16_t mask = (kSnapshotMaskThreadFields) ?egr_gex :0xFFFF;
    mau_snapshot_global_exec_out_.
        mau_snapshot_global_exec_out(kEgress, gex & mask);
  }
}
void MauSnapshot::long_branch_capture(bool ingress) {
  MauIO *outIO = mau()->mau_io_output();
  MauPredication *pred = mau()->mau_predication();
  uint8_t  lbr = outIO->long_branch();
  uint16_t dummy;
  if (ingress) {
    uint8_t ing_lbr = 0, ght_lbr = 0;
    // Figure out what long_branch bits are ingress/ghost bits
    pred->thread_output_info(kIngress, &dummy, &ing_lbr);
    pred->thread_output_info(kGhost, &dummy, &ght_lbr);
    uint8_t mask = (kSnapshotMaskThreadFields) ?ing_lbr|ght_lbr :0xFF;
    mau_snapshot_long_branch_out_.
        mau_snapshot_long_branch_out(kIngress, lbr & mask);
  } else {
    uint8_t egr_lbr = 0;
    // Figure out what long_branch bits are egress bits
    pred->thread_output_info(kEgress, &dummy, &egr_lbr);
    uint8_t mask = (kSnapshotMaskThreadFields) ?egr_lbr :0xFF;
    mau_snapshot_long_branch_out_.
        mau_snapshot_long_branch_out(kEgress, lbr & mask);
  }
}
void MauSnapshot::mpr_next_table_capture(bool ingress) {
  MauIO *outIO = mau()->mau_io_output();
  if (ingress) {
    mau_snapshot_mpr_next_table_out_.
        mau_snapshot_mpr_next_table_out(kIngress, outIO->ingress_mpr_nxt_tab());
    mau_snapshot_mpr_next_table_out_.
        mau_snapshot_mpr_next_table_out(kGhost, outIO->ghost_mpr_nxt_tab());
  } else {
    mau_snapshot_mpr_next_table_out_.
        mau_snapshot_mpr_next_table_out(kEgress, outIO->egress_mpr_nxt_tab());
  }
}
void MauSnapshot::mpr_global_exec_capture(bool ingress) {
  MauIO *outIO = mau()->mau_io_output();
  MauPredication *pred = mau()->mau_predication();
  uint16_t mpr_gex = outIO->mpr_global_exec();
  uint8_t  dummy;
  if (ingress) {
    uint16_t ing_gex = 0, ght_gex = 0;
    // Figure out what global_exec bits are ingress/ghost LTs
    pred->thread_output_info(kIngress, &ing_gex, &dummy);
    pred->thread_output_info(kGhost, &ght_gex, &dummy);
    uint16_t mask = (kSnapshotMaskThreadFields) ?ing_gex|ght_gex :0xFFFF;
    mau_snapshot_mpr_global_exec_out_.
        mau_snapshot_mpr_global_exec_out(kIngress, mpr_gex & mask);
  } else {
    uint16_t egr_gex = 0;
    // Figure out what global_exec bits are egress LTs
    pred->thread_output_info(kEgress, &egr_gex, &dummy);
    uint16_t mask = (kSnapshotMaskThreadFields) ?egr_gex :0xFFFF;
    mau_snapshot_mpr_global_exec_out_.
        mau_snapshot_mpr_global_exec_out(kEgress, mpr_gex & mask);
  }
}
void MauSnapshot::mpr_long_branch_capture(bool ingress) {
  MauIO *outIO = mau()->mau_io_output();
  MauPredication *pred = mau()->mau_predication();
  uint8_t  mpr_lbr = outIO->mpr_long_branch();
  uint16_t dummy;
  if (ingress) {
    uint8_t ing_lbr = 0, ght_lbr = 0;
    // Figure out what long_branch bits are ingress/ghost bits
    pred->thread_output_info(kIngress, &dummy, &ing_lbr);
    pred->thread_output_info(kGhost, &dummy, &ght_lbr);
    uint8_t mask = (kSnapshotMaskThreadFields) ?ing_lbr|ght_lbr :0xFF;
    mau_snapshot_mpr_long_branch_out_.
        mau_snapshot_mpr_long_branch_out(kIngress, mpr_lbr & mask);
  } else {
    uint8_t egr_lbr = 0;
    // Figure out what long_branch bits are egress bits
    pred->thread_output_info(kEgress, &dummy, &egr_lbr);
    uint8_t mask = (kSnapshotMaskThreadFields) ?egr_lbr :0xFF;
    mau_snapshot_mpr_long_branch_out_.
        mau_snapshot_mpr_long_branch_out(kEgress, mpr_lbr & mask);
  }
}
void MauSnapshot::meter_adr_capture(bool ingress) {
  MauAddrDist *mad = mau()->mau_addr_dist();
  for (int i = 0; i < MauDefs::kLogicalRowsPerMau; i++) {
    if (((MauDefs::kMeterAluLogicalRows >> i) & 1) == 1) {
      uint32_t alu = MauMeterAlu::get_meter_alu_regs_index(i);
      RMT_ASSERT(alu < MauDefs::kNumMeterAlus);
      uint32_t meter_addr = mad->meter_addr_nocheck(i);
      bool alu_egr = mau()->is_meter_alu_egress(alu);
      if ((ingress && !alu_egr) || (!ingress && alu_egr)) {
        mau_snapshot_meter_adr_.mau_snapshot_meter_adr(alu, meter_addr);
      }
    }
  }
}
void MauSnapshot::imem_logical_read_adr_capture(bool ingress) {
  MauInstrStore *instore = mau()->mau_instr_store();
  uint16_t lts = mau()->pred_lt_info((ingress) ?Pred::kIngThread :Pred::kEgrThread);
  for (int i = 0; i < kLogicalTables; i++) {
    if (((lts >> i) & 1) == 1) {
      uint8_t op = instore->get_op_for_lt(i);
      mau_snapshot_imem_logical_read_adr_.mau_snapshot_imem_logical_read_adr(i, op);
    }
  }
}
void MauSnapshot::imem_logical_selector_fallback_capture(bool ingress) {
  uint16_t fallback_lts = mau()->mau_instr_store()->get_fallback_using_lts();
  uint16_t lts = mau()->pred_lt_info((ingress) ?Pred::kIngThread :Pred::kEgrThread);
  uint16_t v = mau_snapshot_imem_logical_selector_fallback_.mau_snapshot_imem_logical_selector_fallback();
  v = (v & ~lts) | (fallback_lts & lts);
  mau_snapshot_imem_logical_selector_fallback_.mau_snapshot_imem_logical_selector_fallback(v);
}




}
