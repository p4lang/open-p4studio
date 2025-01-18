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

#include <algorithm>
#include <string>
#include <common/rmt-util.h>
#include <model_core/log-buffer.h>
#include <rmt-log.h>
#include <rmt-object-manager.h>
#include <packet-pipe-data.h>
#include <clot.h>
#include <packet.h>
#include <port.h>


namespace MODEL_CHIP_NAMESPACE {

  Packet::Packet(RmtObjectManager *om)
      : RmtObject(om) {
    reset();
  }
  Packet::Packet(RmtObjectManager *om, size_t len)
      : RmtObject(om) {
    reset();
    (void)packet_body_.insert(new PacketBuffer(len));
  }
  Packet::Packet(RmtObjectManager *om, PacketBuffer *pb)
      : RmtObject(om) {
    reset();
    (void)packet_body_.insert(pb);
  }
  Packet::Packet(RmtObjectManager *om, const uint8_t buf[], size_t len)
      : RmtObject(om) {
    reset();
    (void)packet_body_.insert(new PacketBuffer(buf, len));
  }
  Packet::Packet(RmtObjectManager *om, const std::shared_ptr<uint8_t>& buf, size_t len)
        : RmtObject(om) {
    reset();
    (void)packet_body_.insert(new PacketBuffer(buf, len));
  }
  Packet::Packet(RmtObjectManager *om, const std::string hexstr)
      : RmtObject(om) {
    reset();
    int len = hexstr.length();
    uint8_t buf[len/2];
    int i = model_common::Util::hexmakebuf(hexstr, len, buf);
    if (i == len)
      (void)packet_body_.insert(new PacketBuffer(buf, len/2));
    else
      (void)packet_body_.insert(new PacketBuffer());
  }
  Packet::Packet(RmtObjectManager *om, const char* hexstr)
      : RmtObject(om) {
    reset();
    int len = std::strlen(hexstr);
    uint8_t buf[len/2];
    int i = model_common::Util::hexmakebuf(hexstr, len, buf);
    if (i == len)
      (void)packet_body_.insert(new PacketBuffer(buf, len/2));
    else
      (void)packet_body_.insert(new PacketBuffer());
  }
  Packet::~Packet() {
    reset();
  }

  void Packet::reset_meta() {
    // Switch packet to egress then reset all fields
    set_egress();
    set_phv(NULL);
    set_port(NULL);
    set_version(kPacketInitialVersion);
    set_metadata_added(false);
    set_orig_hdr_len(0);
    set_teop(NULL);
    // Then ingress and reset all fields
    set_ingress();
    set_phv(NULL);
    set_port(NULL);
    set_version(kPacketInitialVersion);
    set_metadata_added(false);
    set_orig_hdr_len(0);
    set_orig_ingress_pkt_len(0);
    set_priority(0);
    // So resets to being an ingress packet
    set_generated(false);
    set_generated_T(UINT64_C(0));
    set_metadata_len(0);
    set_resubmit_header(nullptr);
    set_resubmit(false);
    set_truncated(false);
  }
  void Packet::reset_i2q() {
    i2qing_metadata()->reset();
  }
  void Packet::reset_q2e() {
    qing2e_metadata()->reset();
  }
  void Packet::reset_br() {
    bridge_metadata()->reset();
  }
  void Packet::reset_qing_data() {
    set_next_L1(0);
    set_ph_ver(0);
    set_L1_cnt(0);
    set_L2_cnt(0);
    set_cur_mgid(0);
  }
  void Packet::reset_clots() {
    Clot *ing_clot = ingress_info_.clot();
    Clot *egr_clot = egress_info_.clot();
    ingress_info_.set_clot(NULL);
    egress_info_.set_clot(NULL);
    if (ing_clot != NULL) delete ing_clot;
    if (egr_clot != NULL) delete egr_clot;
  }
  void Packet::reset_time_info() {
    if (kPktInitTimeRandOnAlloc) {
      // If no sweep yet happened T could still be 0.
      // In that case make it 1
      uint64_t T = get_object_manager()->time_get_cycles();
      if (T == UINT64_C(0)) T = UINT64_C(1);
      setup_time_info(T);
    }
  }
  void Packet::reset_bufs() {
    packet_body_.reset();
  }
  void Packet::reset_ids() {
    // NOT called on recirc - so this state preserved
    recirc_cnt_ = 0u;
    pkt_id_ = UINT64_C(0);
  }
  void Packet::reset_except_bufs() {
    // Called on recirc so bufs/ids preserved
    reset_meta();
    reset_i2q();
    reset_q2e();
    reset_br();
    reset_qing_data();
    reset_clots();
    reset_time_info();
  }
  void Packet::reset() {
    packet_body_.set_packet(this);
    reset_except_bufs();
    reset_bufs();
    reset_ids();
  }
  void Packet::destroy() {
    // Stuff that needs to be freed up
    reset_bufs();
    reset_clots();
    free_pipe_data();
    packet_body_.set_packet(nullptr);
  }

  Packet *Packet::clone_internal(bool cp_meta) const {
    Packet *clone_p = get_object_manager()->pkt_create();
    clone_p->packet_body_ = packet_body_; // Deep copy

    if (cp_meta) {
        // Copy the packet metadata as well.
        clone_p->i2qing_metadata()->copy_from(i2qing_metadata_);
        clone_p->qing2e_metadata()->copy_from(qing2e_metadata_);
        clone_p->bridge_metadata()->copy_from(bridge_metadata_);
        clone_p->egress_ = egress_;
        clone_p->ingress_info_ = ingress_info_;
        clone_p->egress_info_ = egress_info_;
        clone_p->qing_data_ = qing_data_;
        // NULL out CLOTs just in case
        clone_p->ingress_info_.set_clot(NULL);
        clone_p->egress_info_.set_clot(NULL);
        clone_p->generated_pkt_ = generated_pkt_;
        if (generated_pkt_) clone_p->generated_T_ = generated_T_;
        clone_p->metadata_len_ = metadata_len_;
        clone_p->resubmit_ = resubmit_;
        clone_p->truncated_ = truncated_;
        // We don't copy PacketPipeData
    }
    clone_p->orig_ingress_pkt_len_ = orig_ingress_pkt_len_;
    clone_p->priority_ = priority_;
    clone_p->pkt_id_ = pkt_id_;
    clone_p->pkt_sig_set(pkt_signature_);
    return clone_p;
  }


  // We take ownership of prepended/appended PacketBuffers
  // Shim through to PacketData object
  void Packet::prepend(PacketBuffer* pb, int replace_bytes) {
    packet_body_.prepend(pb, replace_bytes);
  }
  void Packet::prepend(const BitVector<128> &bv, int replace_bytes) {
    packet_body_.prepend(new PacketBuffer(bv), replace_bytes);
  }
  void Packet::prepend_metadata_hdr(const uint8_t metadata[], size_t size) {
    PacketBuffer *pb = new PacketBuffer(metadata, size);
    prepend(pb);
    // stash len of metadata so that it is available for trim_metadata_hdr
    metadata_len_ = size;
  }
  void Packet::trim_metadata_hdr() {
    packet_body_.trim_front(metadata_len_);
    metadata_len_ = 0;
  }
  void Packet::append(PacketBuffer* pb) {
    packet_body_.append(pb);
  }
  void Packet::append(Packet *pkt) {
    packet_body_.append(&pkt->packet_body_);
  }
  void Packet::append_zeros(const int n) {
    packet_body_.append_zeros(n);
  }
  int Packet::trim_front(int bytes_to_trim) {
    packet_body_.trim_front(bytes_to_trim);  // Asserts `bytes_to_trim == bytes_trimmed`
    return bytes_to_trim;
  }
  int Packet::trim_back(int bytes_to_trim) {
    packet_body_.trim_back(bytes_to_trim);  // Asserts `bytes_to_trim == bytes_trimmed`
    return bytes_to_trim;
  }
  int Packet::get_buf(uint8_t buf[], int start_pos, int len) const {
    return packet_body_.get_buf(buf, start_pos, len);
  }
  void Packet::set_byte(int pos, uint8_t val) {
    packet_body_.set_byte(pos, val);
  }


  void Packet::pkt_sig_set(const std::string& pkt_sig) {
    pkt_signature_ = pkt_sig;
  }
  std::string Packet::pkt_sig_get() {
    return pkt_signature_;
  }
  void Packet::extract_signature() {
    RmtObjectManager *om = get_object_manager();
    if (om->log_use_pkt_sig()) {
      int off=0, buf_len=0;
      om->log_pkt_sig_location(off, buf_len);
      buf_len = std::min(buf_len, MAX_LOG_PKT_SIG_LEN);
      uint8_t buf[buf_len];
      model_core::LogBuffer pkt_sig(2 + (2 * buf_len));
      if (get_buf(buf, off, buf_len)) {
        pkt_sig.Append("0x");
        int str_len = 2;
        for (int i = 0; ((i < buf_len )&& (str_len < (MAX_LOG_PKT_SIG_LEN - 2))); i++) {
          pkt_sig.Append("%02x", (int)buf[i]);
          str_len += 2;
        }
      }
      pkt_signature_ = pkt_sig.GetBuf();
    }
  }

  // Funcs to allow DV to get/set cached PacketPipeData
  // This data can be used as to capture or replace calculated data
  //  at various points in Pipe processing
  //  (so far only Parser information - Parser Counters)
  //
  PacketPipeData *Packet::pipe_data() {
    if (pipe_data_ == nullptr) pipe_data_ = new PacketPipeData(); // Alloc on demand
    return pipe_data_;
  }
  void Packet::set_pipe_data_pipe(int pipe) {
    if (pipe_data_ != nullptr) pipe_data_->set_pipe_data_pipe(pipe);
  }
  void Packet::set_pipe_data(bool ing, int parser, int what_data,
                             int bit_offset, uint64_t data, int width) {
    pipe_data()->set_pipe_data_p(ing, parser, what_data, bit_offset, data, width);
  }
  uint64_t Packet::get_pipe_data(bool ing, int parser, int what_data,
                                 int bit_offset, int width) {
    if (pipe_data_ == nullptr) return UINT64_C(0);
    return pipe_data()->get_pipe_data_p(ing, parser, what_data, bit_offset, width);
  }
  void Packet::set_pipe_data_ctrl(bool ing, int parser, int what_data, uint8_t ctrl) {
    pipe_data()->set_pipe_data_ctrl_p(ing, parser, what_data, ctrl);
  }
  uint8_t Packet::get_pipe_data_ctrl(bool ing, int parser, int what_data) {
    if (pipe_data_ == nullptr) return PacketDataCtrl::kCalcOnly; // Default is calculate data
    return pipe_data()->get_pipe_data_ctrl_p(ing, parser, what_data);
  }
  void Packet::free_pipe_data() {
    if (pipe_data_ != nullptr) delete pipe_data_;
    pipe_data_ = nullptr;
  }
  bool Packet::has_pipe_data() {
    return (pipe_data_ != nullptr);
  }


}
