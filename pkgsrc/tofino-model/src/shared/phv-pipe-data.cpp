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

#include <string>
#include <common/rmt-assert.h>
#include <mau-input.h>
#include <phv-pipe-data.h>


namespace MODEL_CHIP_NAMESPACE {

PhvMauData::PhvMauData(int mau) : mau_(mau), ctrl_(), ixbar_data_(), tcam_match_() {
}
PhvMauData::~PhvMauData() {
}

void PhvMauData::set_ixbar_data(int bit_offset, uint64_t data, int width) {
  RMT_ASSERT((bit_offset >= 0) && (bit_offset < kTotalMatchInputBits));
  //printf("set_ixbar_data(%d,%d,0x%" PRIx64 ")\n", bit_offset, width, data);
  ixbar_data_.set_word(data, bit_offset, width);
}
void PhvMauData::set_ixbar_xm_data(int bit_offset, uint64_t data, int width) {
  RMT_ASSERT((bit_offset >= 0) && (bit_offset < kExactMatchInputBits));
  static_assert( (kExactFirstBit < kTernaryFirstBit),
                 "XM IxBar bits expected to precede TM IxBar bits" );
  // XM/TM share single IxBar with XM bits *preceding* TM bits.
  // So allow offset + width to exceed XM bits - but reduce width to remain in XM range
  if (bit_offset + width > kExactMatchInputBits) width = kExactMatchInputBits - bit_offset;
  set_ixbar_data(kExactFirstBit + bit_offset, data, width);
}
void PhvMauData::set_ixbar_tm_data(int bit_offset, uint64_t data, int width) {
  RMT_ASSERT((bit_offset >= 0) && (bit_offset < kTernaryMatchInputBits));
  set_ixbar_data(kTernaryFirstBit + bit_offset, data, width);
}
void PhvMauData::set_tcam_match_data(int bit_offset, uint64_t data, int width) {
  RMT_ASSERT((bit_offset >= 0) && (bit_offset < kTcamMatchTotalWidth));
  tcam_match_.set_word(data, bit_offset, width);
}
void PhvMauData::set_tcam_match_addr(int ltcam, uint32_t match_addr, uint8_t hit, uint8_t action_bit) {
  RMT_ASSERT((ltcam >= 0) && (ltcam < kLogicalTcams));
  // Write result into correct location in tcam_match_ BV based on OutputWidth/LTCAM index
  int offset = kTcamMatchOutputWidth * ltcam;
  RMT_ASSERT(offset < kTcamMatchTotalWidth);
  uint64_t data = make_tcam_match(match_addr, hit, action_bit);
  set_tcam_match_data(offset, data, kTcamMatchOutputWidth);
}


uint64_t PhvMauData::get_ixbar_data(int bit_offset, int width) {
  RMT_ASSERT((bit_offset >= 0) && (bit_offset < kTotalMatchInputBits));
  uint64_t data = ixbar_data_.get_word(bit_offset, width);
  //printf("get_ixbar_data(%d,%d) = 0x%" PRIx64 "\n", bit_offset, width, data);
  return data;
}
uint64_t PhvMauData::get_ixbar_xm_data(int bit_offset, int width) {
  RMT_ASSERT((bit_offset >= 0) && (bit_offset < kExactMatchInputBits));
  return get_ixbar_data(kExactFirstBit + bit_offset, width);
}
uint64_t PhvMauData::get_ixbar_tm_data(int bit_offset, int width) {
  RMT_ASSERT((bit_offset >= 0) && (bit_offset < kTernaryMatchInputBits));
  return get_ixbar_data(kTernaryFirstBit + bit_offset, width);
}
uint64_t PhvMauData::get_tcam_match_data(int bit_offset, int width) {
  RMT_ASSERT((bit_offset >= 0) && (bit_offset < kTcamMatchTotalWidth));
  return tcam_match_.get_word(bit_offset, width);
}
void PhvMauData::get_tcam_match_addr(int ltcam, uint32_t *match_addr, uint8_t *hit, uint8_t *action_bit) {
  RMT_ASSERT((ltcam >= 0) && (ltcam < kLogicalTcams) && (match_addr != nullptr));
  // Read result from correct location in tcam_match_ BV based on OutputWidth/LTCAM index
  int offset = kTcamMatchOutputWidth * ltcam;
  RMT_ASSERT(offset < kTcamMatchTotalWidth);
  uint64_t data = get_tcam_match_data(offset, kTcamMatchOutputWidth);
  *match_addr = tcam_match_addr(data);
  if (hit != nullptr) *hit = tcam_match_hit(data);
  if (action_bit != nullptr) *action_bit = tcam_match_abit(data);
}


void PhvMauData::set_mau_data(int what_data, int bit_offset, uint64_t data, int width) {
  switch (what_data) {
    case PhvData::kIxbarData:     set_ixbar_data(bit_offset, data, width);      break;
    case PhvData::kIxbarXmData:   set_ixbar_xm_data(bit_offset, data, width);   break;
    case PhvData::kIxbarTmData:   set_ixbar_tm_data(bit_offset, data, width);   break;
    case PhvData::kTcamMatchAddr: set_tcam_match_data(bit_offset, data, width); break;
    default: RMT_ASSERT(0 && "set_mau_data bad argument");
  }
}
uint64_t PhvMauData::get_mau_data(int what_data, int bit_offset, int width) {
  switch (what_data) {
    case PhvData::kIxbarData:     return get_ixbar_data(bit_offset, width);
    case PhvData::kIxbarXmData:   return get_ixbar_xm_data(bit_offset, width);
    case PhvData::kIxbarTmData:   return get_ixbar_tm_data(bit_offset, width);
    case PhvData::kTcamMatchAddr: return get_tcam_match_data(bit_offset, width);
    default: return UINT64_C(0);
  }
}

void PhvMauData::set_mau_data_ctrl(int what_data, uint8_t ctrl) {
  RMT_ASSERT((what_data >= PhvData::kMin) && (what_data <= PhvData::kMax));
  RMT_ASSERT((ctrl >= PhvDataCtrl::kCalcOnly) && (ctrl <= PhvDataCtrl::kLoadFromPhv));
  if (what_data == PhvData::kIxbarData) {
    // Setting ctrl on IxbarData should affect IxbarXmData and IxbarTmData too
    ctrl_[what_data] = ctrl_[PhvData::kIxbarXmData]  = ctrl_[PhvData::kIxbarTmData] = ctrl;
  } else {
    ctrl_[what_data] = ctrl;
  }
}
uint8_t PhvMauData::get_mau_data_ctrl(int what_data) {
  RMT_ASSERT((what_data >= PhvData::kMin) && (what_data <= PhvData::kMax));
  return ctrl_[what_data];
}




PhvPipeData::PhvPipeData(int pipe) : pipe_(pipe) {
  for (int i = 0; i < kStages; i++) {
    maus_[i] = nullptr;
  }
}
PhvPipeData::~PhvPipeData() {
  for (int i = 0; i < kStages; i++) {
    if (maus_[i] != nullptr) delete maus_[i];
    maus_[i] = nullptr;
  }
}

PhvMauData *PhvPipeData::phv_mau_data_create(int mau) {
  RMT_ASSERT((mau >= 0) && (mau < kStages));
  return new PhvMauData(mau);
}
PhvMauData *PhvPipeData::phv_mau_data_lookup(int mau) {
  RMT_ASSERT((mau >= 0) && (mau < kStages));
  return maus_[mau];
}
PhvMauData *PhvPipeData::phv_mau_data_set(int mau, PhvMauData *mau_data) {
  RMT_ASSERT((mau >= 0) && (mau < kStages) && (mau_data != nullptr));
  maus_[mau] = mau_data;
  return mau_data;
}
PhvMauData *PhvPipeData::phv_mau_data_get(int mau) {
  RMT_ASSERT((mau >= 0) && (mau < kStages));
  PhvMauData *mau_data = phv_mau_data_lookup(mau);
  if (mau_data != nullptr) return mau_data;
  return phv_mau_data_set(mau, phv_mau_data_create(mau));
}


void PhvPipeData::set_pipe_data(int mau, int what_data,
                                int bit_offset, uint64_t data, int width) {
  RMT_ASSERT((mau >= 0) && (mau < kStages));
  RMT_ASSERT((what_data >= PhvData::kMin) && (what_data <= PhvData::kMax));
  PhvMauData *mau_data = phv_mau_data_get(mau);
  RMT_ASSERT(mau_data != nullptr);
  mau_data->set_mau_data(what_data, bit_offset, data, width);
}
uint64_t PhvPipeData::get_pipe_data(int mau, int what_data,
                                    int bit_offset, int width) {
  RMT_ASSERT((what_data >= PhvData::kMin) && (what_data <= PhvData::kMax));
  PhvMauData *mau_data = phv_mau_data_lookup(mau);
  RMT_ASSERT(mau_data != nullptr);
  return mau_data->get_mau_data(what_data, bit_offset, width);
}

// Convenience funcs for tcam outputs
void PhvPipeData::set_pipe_data_tcam_match_addr(int mau, int ltcam,
                                                uint32_t match_addr, uint8_t hit, uint8_t action_bit) {
  RMT_ASSERT((mau >= 0) && (mau < kStages));
  PhvMauData *mau_data = phv_mau_data_get(mau);
  RMT_ASSERT(mau_data != nullptr);
  mau_data->set_tcam_match_addr(ltcam, match_addr, hit, action_bit);
}
void PhvPipeData::get_pipe_data_tcam_match_addr(int mau, int ltcam,
                                                uint32_t *match_addr, uint8_t *hit, uint8_t *action_bit) {
  RMT_ASSERT((mau >= 0) && (mau < kStages));
  PhvMauData *mau_data = phv_mau_data_get(mau);
  RMT_ASSERT_NOT_NULL(mau_data);
  mau_data->get_tcam_match_addr(ltcam, match_addr, hit, action_bit);
}


void PhvPipeData::set_pipe_data_ctrl(int mau, int what_data, uint8_t ctrl) {
  RMT_ASSERT((what_data >= PhvData::kMin) && (what_data <= PhvData::kMax));
  RMT_ASSERT((ctrl >= PhvDataCtrl::kCalcOnly) && (ctrl <= PhvDataCtrl::kLoadFromPhv));
  // Ignore set of Calc if no PhvMauData object yet exists as Calc is default
  if ((ctrl == PhvDataCtrl::kCalcOnly) && (phv_mau_data_lookup(mau) == nullptr)) return;
  PhvMauData *mau_data = phv_mau_data_get(mau);
  RMT_ASSERT(mau_data != nullptr);
  mau_data->set_mau_data_ctrl(what_data, ctrl);
}
uint8_t PhvPipeData::get_pipe_data_ctrl(int mau, int what_data) {
  RMT_ASSERT((what_data >= PhvData::kMin) && (what_data <= PhvData::kMax));
  PhvMauData *mau_data = phv_mau_data_lookup(mau);
  // If no PhvMauData object created yet, just say we want to CalcOnly
  if (mau_data == nullptr) return PhvDataCtrl::kCalcOnly;
  return mau_data->get_mau_data_ctrl(what_data);
}



}
