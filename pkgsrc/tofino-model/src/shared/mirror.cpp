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
#include <rmt-log.h>
#include <rmt-object-manager.h>
#include <packet.h>
#include <queueing.h>
#include <deparser-block.h>
#include <mirror.h>
#include <chip.h>

namespace MODEL_CHIP_NAMESPACE {

Mirror::Mirror(RmtObjectManager *om, int pipeIndex) :
      PipeObject(om, pipeIndex),
      mirror_regs_(chip_index(), pipeIndex),
      coalescing_timer_([this](uint64_t tid){this->ProcessCoalTimer(tid);}),
      active_coal_sessions_(0), coal_tx_enable_(true), coal_session_mutex_()
{
  for (int slice = 0; slice < kSlices; slice++) {
    for (int coal_num = 0; coal_num < kCoalSessions; coal_num++) {
      coal_sessions_[slice][coal_num].clear();
    }
  }
}


void Mirror::MirrorPktSetI2Qmetadata(uint32_t sess_id, Packet *pkt, uint32_t version)
{
  // Some chips (JBay) allow atomic session cfg update, other chips (Tofino) don't.
  // Abstract away using MirrorSessionReg object which handles things appropriately
  //
  MirrorSessionReg sess(&mirror_regs_, pipe_index(), sess_id); // Create local session
  mirror_regs_.sess_dp_load(&sess, sess_id);                   // ...init with sess_id

  // Some chips (JBay) allow MAU+Deparser to override per-session config by
  // squirrelling away info in MirrorMetadata
  // Then per-session mirror _cfg registers determine whether that dynamic
  // info is used or whether static per-session info is used
  //
  MirrorMetadata *mm = pkt->mirror_metadata();
  I2QueueingMetadata *i2q = pkt->i2qing_metadata();

  i2q->set_version( version );

  // HASH cfg:
  // The cfg bit[0] will either choose the hash from the per-session register or from MAU
  // The cfg bit[1] will select which of hash1 or hash2 will have the hash from MAU.
  //     The other hash will be from the per-session register
  //
  uint16_t hash1 = sess.hash1(), hash2 = sess.hash2();
  if (((sess.hash_cfg() >> 0) & 1) == 1) {
    uint16_t mirrhash = static_cast<uint16_t>(mm->mirr_hash() & 0xFFFF);
    // hash_cfg[0]==1 ==> get from MAU
    // hash_cfg[1]==1 ==> replace hash1, hash_cfg[1]==0 ==> replace hash2
    if (((sess.hash_cfg() >> 1) & 1) == 1) hash1 = mirrhash; else hash2 = mirrhash;
  }
  i2q->set_hash1( hash1 );
  i2q->set_hash2( hash2 );


  // ICOS cfg:
  // The cfg bit will either choose from the per-session register or from MAU
  //
  uint8_t icos = ((sess.icos_cfg() & 1) == 1) ?mm->mirr_icos() :sess.icos();
  i2q->set_icos( icos );


  // DoD cfg:
  // The cfg bit will either choose from the per-session register or from MAU
  // (note Tofino did not use def_on_drop(), so uses_def_on_drop == false)
  //
  uint8_t sessdod = (sess.uses_def_on_drop()) ?sess.def_on_drop() :0;
  uint8_t mirrdod = (mm->mirr_dond_ctrl()) ?1 :0;
  uint8_t dod = ((sess.dod_cfg() & 1) == 1) ?mirrdod :sessdod;
  i2q->set_dod( dod );


  // COPY-TO-CPU cfg:
  // If the cfg bit is set to choose from MAU (1) and c2c_ctrl is 1,
  // then c2c information (c2c_cos and c2c_vld) are from per-session register.
  // If the cfg bit is set to choose from MAU (1) and c2c_ctrl is 0, then
  // c2c_cos and c2c_vld are 0 (not c2c).
  // If the cfg bit is 0, then c2c_cos and c2c_vld are from the per-session register.
  //
  uint8_t c2c_vld = sess.c2c_vld(), c2c_cos = sess.c2c_cos();
  if ((sess.c2c_cfg() & 1) == 1) {
    if (mm->mirr_c2c_ctrl() == 0) { c2c_vld = 0; c2c_cos = 0; }
  }
  i2q->set_copy_to_cpu( c2c_vld );
  i2q->set_copy_to_cpu_cos( c2c_cos );


  // EPIPE CFG:
  // The cfg bit will either choose both of these two fields from the
  //   per-session register or from MAU.
  // If taken from MAU then epipe_port may be valid or invalid on a
  //   per-packet basis.
  // epipe_qid is always valid under all circumstances.
  //
  uint8_t  eport_qid = sess.eport_qid(), epipe_port_vld = sess.epipe_port_vld();
  uint16_t epipe_port = sess.epipe_port();
  if ((sess.epipe_cfg() & 1) == 1) {
    if (mm->mirr_epipe_port()) {
      epipe_port_vld = 1; epipe_port = *mm->mirr_epipe_port();
    } else {
      epipe_port_vld = 0;
    }
    eport_qid = mm->mirr_qid();
  }
  i2q->set_qid( eport_qid );
  if (epipe_port_vld) {
    RmtObjectManager *om = get_object_manager();
    Deparser *dprs = om->deparser_lookup(pipe_index())->ingress();
    int logical_port = epipe_port;
    int phy_port = dprs->RemapLogicalToPhy(logical_port, false);
    i2q->set_egress_unicast_port( phy_port );
  }

  SetI2QmetadataChip(pkt, sess);

  // Always get these fields
  i2q->set_meter_color( sess.color() );
  i2q->set_xid( sess.xid() );
  i2q->set_yid( sess.yid() );
  i2q->set_irid( sess.rid() );

  // No bypass egress mode on Tofino. Yes on JBay
  // No using yid tbl on Tofino. Yes on JBay
  uint8_t eby = (sess.uses_egress_bypass()) ?sess.egress_bypass() :0;
  uint8_t yts = (sess.uses_yid_tbl_sel())   ?sess.yid_tbl_sel()   :0;
  i2q->set_bypass_egr_mode( eby );
  i2q->set_use_yid_tbl( yts );
}

Packet *Mirror::ProcessMirrorPacket(Packet *pkt, bool ingress)
{
  MirrorReg  *M = &mirror_regs_;
  Packet     *mpkt = pkt;
  int         slice = M->get_slice(pkt);

  if (!M->glob_en(ingress)) {
    RMT_P4_LOG_ERROR("Mirroring buffers are not enabled\n");
    return nullptr;
  }

  // get session_id from mirror metadata
  MirrorMetadata  *mirror_metadata = pkt->mirror_metadata();
  uint16_t         sess_id = mirror_metadata->mirror_id();
  uint8_t          version = mirror_metadata->version();
  bool             sess_ok = M->sess_ok(slice, sess_id);

  RMT_P4_LOG_INFO("---- Mirroring session %s%d : %s  %s----\n",
                     M->slice_name(slice), (int)sess_id,
                     ingress ? "ingress" : "egress",
                     sess_ok ? "" : "INVALID");
  if (!sess_ok) return nullptr;

  bool mirr_session = M->sess_en(slice, sess_id, ingress);
  if (!mirr_session) {
    RMT_P4_LOG_VERBOSE("Mirroring session %s%d is not enabled\n",
                    M->slice_name(slice), (int)sess_id);
    return nullptr;
  }

  bool coal_session = M->sess_coal_en(slice, sess_id, ingress);
  if (!coal_session) {
    RMT_P4_LOG_VERBOSE("Mirror coalescing session %s%d is not enabled\n",
                    M->slice_name(slice), (int)sess_id);
  } else if (!M->glob_coal_en(ingress)) {
    // Coal session requested but coalesce not supported
    // for gress (eg ingress on Tofino) - fallback to mirror?
    RMT_P4_LOG_VERBOSE("Mirror coalescing session not supported for %s\n",
                    ingress ?"ingress" :"egress");
    mirr_session = M->glob_mirr_if_not_coal(ingress);
    if (mirr_session) {
      RMT_P4_LOG_VERBOSE("Falling back to mirror session for %s%d\n",
                      M->slice_name(slice), (int)sess_id);
    } else {
      return nullptr;
    }
  }

  if (mirr_session && !coal_session) {
    // for coal packet, truncation happens later
    uint32_t min_pkt_size = M->min_pkt_len();
    uint32_t max_pkt_size = M->trunc_size(slice, sess_id);
    RMT_ASSERT(min_pkt_size <= max_pkt_size);
    uint32_t pkt_len = pkt->len();
    // truncate the packet if it exceeds max allowed size
    if (pkt_len > max_pkt_size) {
      pkt->trim_back(pkt_len - max_pkt_size);
      RMT_P4_LOG_VERBOSE("Truncated mirrored pkt from %d to %d bytes\n", pkt_len, max_pkt_size);
    } else if (pkt_len < min_pkt_size) {
      RMT_ASSERT(min_pkt_size < 64);
      static uint8_t pad_buf[64] = { 0 };
      RmtObjectManager *om = get_object_manager();
      PacketBuffer *pad_pb = om->pktbuf_create(pad_buf, min_pkt_size - pkt_len);
      RMT_P4_LOG_VERBOSE("Padded mirrored pkt to %d bytes\n", min_pkt_size);
      pkt->append(pad_pb);
    }
    if (RmtObject::is_jbayA0()) {
      // XXX: JBayA0 erroneously adds an extra 4B FCS
      pkt->append_zeros(Packet::kPktFcsLen);
    }
    // for all packets i2q meta is needed.
    MirrorPktSetI2Qmetadata(sess_id, pkt, version);
  }
  if (mirr_session && coal_session) {
    // coalescing session:
    //  NOTE: i2q meta added when packet is closed
    //  if packet is not started, allocate new packet, build header..
    //  add specific bytes from egress pkt to the coal_pkt
    //  if coal_pkt.len >= threshold,
    //      close this packet
    //      add session metadata
    //      return pkt
    //  else
    //      return NULL; packet is not ready for tx
    RMT_P4_LOG_VERBOSE("Coalescing Sessions %s%d\n",
                    M->slice_name(slice), (int)sess_id);
    // wait for enough bytes, start timer, add new header etc.. TBD
    int coal_num = M->sess_id_to_coal_id(slice, sess_id);
    RMT_ASSERT(M->coal_ok(slice, coal_num));
    mpkt = CoalPktAdd(slice, coal_num, pkt);
  }

  // XXX - neg mirroring - TBD

  if (mpkt) {
    I2QueueingMetadata *i2q = mpkt->i2qing_metadata();
    if (i2q->is_egress_uc() ||
        i2q->has_mgid1() || i2q->has_mgid2() ||
        i2q->cpu_needs_copy()) {
      RMT_P4_LOG_VERBOSE("Mirrored pkt to egress port %d for session %s%d "
                      "[is_egress_uc=%d mgid1=%d mgid2=%d c2c=%d]\n",
                      i2q->egress_uc_port(), M->slice_name(slice), (int)sess_id,
                      i2q->is_egress_uc()?1:0,
                      (int)i2q->mgid1(), (int)i2q->mgid2(),
                      i2q->cpu_needs_copy()?1:0);
    } else {
      RMT_P4_LOG_VERBOSE("Mirrored pkt for session %s%d has NO unicast egress port and "
                      "NO mgid1 or mgid2 and is NOT CopyToCpu - IGNORING!\n",
                      M->slice_name(slice), (int)sess_id);
      return nullptr;
    }
  }
  return mpkt;
}

void Mirror::ProcessCoalTimer(uint64_t tid)
{
  // RMT_P4_LOG_DEBUG("Mirror Coalescing Timer processing start..\n");
  // go thru' all active packets on coalescing sessions
  for (int slice = 0; slice < kSlices; slice++) {
    for (int coal_num = 0; coal_num < kCoalSessions; coal_num++) {

      // take a lock
      std::lock_guard<std::mutex> lck(coal_session_mutex_);

      if (coal_sessions_[slice][coal_num].pkt == nullptr) {
        continue;
      }
      // XXX check timer ignore bit ?? - Since the flush bit functionality is removed
      // this bit is not valid anymore
      // use post decrement so that timeout of 0 will work too.
      // And we'll get timeout >= specified

      if (coal_sessions_[slice][coal_num].remaining_time--) {
        continue;
      }
      // packet is active for long enough, close it and schedule for tx
      CoalPktTx(slice, coal_num);
    }
  }
}

void Mirror::CoalPktTx(int slice, int coal_num, Packet *pkt)
{
  MirrorReg               *M = &mirror_regs_;
  I2QueueingMetadata      *i2q;
  RmtObjectManager        *om = get_object_manager();
  Queueing                *qing = om->queueing_get();

  // coal_sessions_ lock must be held by the caller
  // We may be directly given a Closed pkt
  //   Otherwise build the coalesced packet for Tx
  if (pkt == nullptr) pkt = CoalPktClose(slice, coal_num);
  i2q = pkt->i2qing_metadata();
  // coal_sessions_[coal_num].clear(); - done by close
  // packet must be set to go somewhere
  if (i2q->is_egress_uc() || i2q->has_mgid1() || i2q->has_mgid1() || i2q->cpu_needs_copy()) {

    // Normally send packet here - but can be configured to just delete it instead
    if (coal_tx_enable_) {
      qing->enqueue_and_resume(pkt);
    } else {
      om->pkt_delete(pkt);
    }
  } else {
    RMT_P4_LOG_VERBOSE("Coalesced mirrored pkt for coal session %s%d has NO unicast egress port "
                    "and NO mgid1 or mgid2 and is NOT CopyToCpu - IGNORING!\n",
                    M->slice_name(slice), coal_num);
    om->pkt_delete(pkt);
  }
}

Packet *Mirror::CoalPktAdd(int slice, int coal_num, Packet *pkt)
{
  // Confusingly Coal Pkts are split up into 'slices' each
  // with a slice hdr - this is *nothing* to do with the Deparser
  // slice parameter passed to this function

  uint32_t pad = 0u;
  uint8_t  buf[kCoalSliceSize];
  MirrorReg *M = &mirror_regs_;

  // Take a lock
  std::lock_guard<std::mutex> lck(coal_session_mutex_);
  uint16_t port = M->get_port(pkt);
  uint16_t pkt_len = pkt->len();
  RMT_ASSERT((pkt_len > 0) && "Zero length pkt in CoalPktAdd");

  Packet *cpkt_last = nullptr;
  Packet *cpkt  = coal_sessions_[slice][coal_num].pkt;
  if (cpkt == nullptr) cpkt = CoalPktStart(slice, coal_num, pkt);
  RMT_ASSERT((cpkt != nullptr) && "Failed cpkt alloc in CoalPktAdd");
  uint16_t cpkt_len = cpkt->len();

  uint16_t coal_len = 0;
  if (M->coal_len_from_input(slice, coal_num)) {
    coal_len = pkt->mirror_metadata()->coal_len() * 4; // in words
  } else {
    coal_len = M->coal_extract_len(slice, coal_num);
  }
  RMT_ASSERT(coal_len <= kCoalMaxSize); // Check [0..1020]

  // Figure out whether we need to expand/limit coal_len
  uint16_t coal_trunc_size = M->coal_trunc_size(slice, coal_num);
  bool trunc_pkt = M->coal_trunc_pkt(coal_len);
  bool whole_pkt = M->coal_whole_pkt(coal_len);
  if (whole_pkt) coal_len = pkt_len;


  if (M->coal_tofino_mode(slice, coal_num)) {
    if (cpkt_len == 0) cpkt->mirror_metadata()->copy_from(*pkt->mirror_metadata());

    // If truncating (JBay) ensure we limit coal_len to coal_trunc_size
    if (trunc_pkt && (coal_len > coal_trunc_size)) coal_len = coal_trunc_size;

    // No slice hdrs on Tofino or if JBay Tofino mode
    // - just do it the way the old code used to
    //
    // XXX what if the cpkt exceeds mtu new pkt is appended to it ?
    // Assume that we can add atleast 1 more pkt (sample)
    // copy(extract) specified number of bytes from pkt to cpkt

    RMT_ASSERT(coal_len > 0);
    if (pkt_len > coal_len) pkt->trim_back(pkt_len - coal_len);
    cpkt->append(pkt);
    coal_sessions_[slice][coal_num].pkt_count++;
    cpkt_len += (coal_len > pkt_len) ?pkt_len :coal_len;

  } else {
    // Loop copying a slice worth of data from passed-in pkt
    // and (on JBay) interspersing slice headers until all
    // required length has been copied
    //
    // Note WRT to when we stop JBay Mirror uArch doc says:
    // "If adding a new slice would exceed the configured size
    //  then the current packet is closed and the pointer is
    //  written out to the Output Ptr, and a new packet is started."
    //
    // Find current cpkt size
    // Loop through passed-in pkt
    //   Copy upto kCoalSliceBodySize bytes from packet[pos] into buf
    //   Pad out with zeros upto 4B boundary
    //   If cpkt.len + hdrsize + bodysize + padsize > max_size_coal_packet then
    //      CoalPktTX/CoalPktClose the last cpkt
    //      CoalPktStart a new cpkt
    //   Setup slice header with actual valid bytes (also port/start/end fields)
    //   Copy buf into new PacketBuffer and append to cpkt
    //

    uint16_t pkt_bytes_to_copy = (coal_len > pkt_len) ?pkt_len :coal_len;
    uint16_t pkt_pos = 0;

    //printf("COAL_PKT_ADD(start): %s%d (cpkt_len=%d pkt_len=%d coal_len=%d bytes_to_copy=%d)\n",
    //       M->slice_name(slice), coal_num, cpkt_len, pkt_len, coal_len, pkt_bytes_to_copy);

    while (pkt_pos < pkt_bytes_to_copy) {

      // Copy pkt mirror_metadata so we have access to all mirror_metadata
      // on CoalPktClose - we reset mirror_metadata when CoalPktClose done.
      // NB *FIRST packet* put into coal packet is source of mirror_metadata
      // so we only do this is cpkt_len == 0
      if (cpkt_len == 0) cpkt->mirror_metadata()->copy_from(*pkt->mirror_metadata());

      // Copy up to 176B into buf - start at pos 4 to leave space for slice hdr
      int hdr_bytes = kCoalSliceHdrSize;
      int lft_bytes = pkt_bytes_to_copy - pkt_pos; // Bytes left to copy
      int get_bytes = (lft_bytes < kCoalSliceBodySize) ?lft_bytes :kCoalSliceBodySize;
      int pkt_bytes = pkt->get_buf(&buf[hdr_bytes], pkt_pos, get_bytes);
      int pad_bytes = ((pkt_bytes % 4) != 0) ?(4 - (pkt_bytes % 4)) :0;
      int new_bytes = hdr_bytes + pkt_bytes + pad_bytes;
      RMT_ASSERT( (pkt_bytes > 0) && "Packet unexpectedly ran out");
      RMT_ASSERT( (new_bytes <= kCoalSliceSize) && "Excessive coal bytes");

      // Copy in pad_bytes at offset=hdr_bytes+pkt_bytes
      (void)model_common::Util::fill_buf(buf, kCoalSliceSize,
                                         hdr_bytes+pkt_bytes, pad_bytes, pad);

      // Setup sliceHdr using port, num sampled bytes(no pad), start/end flags
      bool start = (pkt_pos == 0);
      bool end   = ((pkt_pos + pkt_bytes) == pkt_bytes_to_copy);
      uint32_t sliceHdr = M->coal_slicehdr(port, pkt_bytes, start, end);
      RMT_ASSERT( (sliceHdr != 0u) && "Unexpected zero slice hdr");
      // Copy in sliceHdr bytes at offset=0
      (void)model_common::Util::fill_buf(buf, kCoalSliceSize,
                                         0, kCoalSliceHdrSize, sliceHdr);

      PacketBuffer *pb = get_object_manager()->pktbuf_create(buf, new_bytes);
      cpkt->append(pb);
      coal_sessions_[slice][coal_num].pkt_count++;

      cpkt_len += new_bytes;
      pkt_pos += pkt_bytes;

      // Check if we have exceeded Coal max pkt_len
      if (M->coal_pkthdr_len(slice, coal_num) + cpkt_len > coal_trunc_size) {

        //printf("COAL_PKT_ADD(close): %s%d (cpkt_len=%d trunc_size=%d) %s\n",
        //       M->slice_name(slice), coal_num, cpkt_len, coal_trunc_size,
        //       (cpkt_last != nullptr) ?"WILL TX!" :"");

        // YES. Need to close curr cpkt - TX any pending cpkt_last
        if (cpkt_last != nullptr) CoalPktTx(slice, coal_num, cpkt_last);
        // Now just close the current cpkt. We will either:
        // 1. return it for TX by caller, or
        // 2. TX it ourself the next time we get here
        cpkt_last = CoalPktClose(slice, coal_num);
        cpkt = CoalPktStart(slice, coal_num, pkt); // Start fresh cpkt
        cpkt_len = 0;

        //printf("COAL_PKT_ADD(new): %s%d = %d\n", M->slice_name(slice), coal_num, cpkt_len);
      }


    } // while (pkt_pos < pkt_bytes_to_copy)

    //printf("COAL_PKT_ADD(end): %s%d = %d\n", M->slice_name(slice), coal_num, cpkt_len);


  } // if/ifnot (M->coal_tofino_mode(slice, coal_num))


  uint16_t cpkt_min_size = M->coal_pkt_min_size(slice, coal_num);
  if (cpkt_len >= cpkt_min_size) {
    //printf("COAL_PKT_ADD(close2exit): %s%d (cpkt_len=%d trunc_size=%d) %s\n",
    //       M->slice_name(slice), coal_num, cpkt_len, coal_trunc_size,
    //      (cpkt_last != nullptr) ?"WILL TX!" :"");

    get_object_manager()->pkt_delete(pkt);
    // Need to close curr cpkt - TX any pending cpkt_last
    if (cpkt_last != nullptr) CoalPktTx(slice, coal_num, cpkt_last);
    cpkt_last = CoalPktClose(slice, coal_num);
    return cpkt_last;
  } else if (cpkt_last != nullptr) {
    //printf("COAL_PKT_ADD(exit): %s%d (cpkt_len=%d trunc_size=%d) Got cpkt_last\n",
    //       M->slice_name(slice), coal_num, cpkt_len, coal_trunc_size);

    get_object_manager()->pkt_delete(pkt);
    // Can leave curr cpkt as is - just return cpkt_last
    return cpkt_last;
  } else {
    //printf("COAL_PKT_ADD(null): %s%d (cpkt_len=%d trunc_size=%d)  NO cpkt_last\n",
    //       M->slice_name(slice), coal_num, cpkt_len, coal_trunc_size);
    // Leave curr cpkt to be handled by timer
    // Original pkt needs to be freed by the caller
    return nullptr;
  }
}

Packet *Mirror::CoalPktStart(int slice, int coal_num, Packet *pkt)
{
  RMT_ASSERT(pkt != nullptr);

  // coal_sessions_ lock must be held by the caller
  // create a new packet
  // copy the timeout value
  MirrorReg  *M = &mirror_regs_;
  Packet *cpkt = get_object_manager()->pkt_create();

  RMT_ASSERT(coal_sessions_[slice][coal_num].pkt == nullptr);
  coal_sessions_[slice][coal_num].pkt = cpkt;
  coal_sessions_[slice][coal_num].remaining_time = M->coal_timeout(slice, coal_num);

  if (!coalescing_timer_.is_running() && M->coal_timer_enabled()) {
    uint32_t timeout = M->coal_basetime();
    coalescing_timer_.run(timeout, model_timer::Timer::Recurring);
  }
  ++active_coal_sessions_;
  RMT_ASSERT(active_coal_sessions_ <= kSlices*kCoalSessions);

  return cpkt;
}

Packet *Mirror::CoalPktClose(int slice, int coal_num)
{
  MirrorReg  *M = &mirror_regs_;
  // coal_sessions_ lock must be held by the caller
  Packet *cpkt = coal_sessions_[slice][coal_num].pkt;

  RMT_P4_LOG_DEBUG("Mirror Coalescing Pkt %s%d Closed..\n",
                     M->slice_name(slice), M->coal_id_to_sess_id(slice, coal_num));
  RMT_ASSERT(cpkt != NULL);
  // prepend headers (from registers or from incoming packet)
  uint8_t header_len = M->coal_pkthdr_len(slice, coal_num);

  // hardware can only accept hdr len that is multiple of 4
  RMT_ASSERT((header_len % 4) == 0);
  RMT_ASSERT((header_len >= kCoalHeaderMinSize) && (header_len <= kCoalHeaderMaxSize));
  static_assert( (kCoalHeaderMaxSize >= 16),
                 "Insufficient buffering for 4x32b of COAL headers" );
  int buflen = kCoalHeaderMaxSize, siz = sizeof(uint32_t), pos = 0;
  uint8_t buf[buflen];

  // MSB of various PktHdr fields becomes MSByte in CoalHdr
  uint32_t hdr0 = M->coal_pkthdr0(slice, coal_num);
  uint32_t hdr1 = M->coal_pkthdr1(slice, coal_num);
  uint32_t hdr2 = M->coal_pkthdr2(slice, coal_num);
  uint32_t hdr3 = M->coal_pkthdr3(slice, coal_num);
  //printf("COAL_PKT_CLOSE: %s%d = Headers[0x%08x,0x%08x,0x%08x,0x%08x]\n",
  //       M->slice_name(slice), coal_num, hdr0, hdr1, hdr2, hdr3);

  pos = model_common::Util::fill_buf(buf, buflen, pos, siz, hdr0);
  pos = model_common::Util::fill_buf(buf, buflen, pos, siz, hdr1);
  pos = model_common::Util::fill_buf(buf, buflen, pos, siz, hdr2);
  // XXX: Ignore return result to keep StaticAnalysis happy
  (void)model_common::Util::fill_buf(buf, buflen, pos, siz, hdr3);
  // Replace byte 0 with pkt(sample)_count (this is 'slice' count on JBay)
  // (Byte0 used on Tofino, Byte1 on JBay)
  // Compiler plans to use 2nd/1st byte internally to signal egress parser that
  // this packet is mirrored.
  // On Tofino user should not use bytes 0,1 of the header0 register
  buf[kCoalHeaderPktCntPos] = coal_sessions_[slice][coal_num].pkt_count;

  PacketBuffer *pb = get_object_manager()->pktbuf_create(buf, header_len);
  // add the new header
  cpkt->prepend(pb);

  // Extend packet if shorter than min_bcnt
  // (On TofinoA0 min_bcnt is *always* 0 so this does not happen)
  // And always add on 4B FCS - but just add zeros
  const int fcs_size = Packet::kPktFcsLen; // 4B
  const int pad_size = kCoalMinSize + fcs_size;
  uint8_t   min_bcnt = M->coal_abs_min_size(slice, coal_num);
  uint32_t  cpkt_len = cpkt->len();
  uint32_t  pad_len = fcs_size + ( (min_bcnt > cpkt_len) ?(min_bcnt - cpkt_len) :0 );
  RMT_ASSERT(pad_len <= pad_size);
  uint8_t pad_buf[pad_size] = { 0 }; // Just add zeros
  PacketBuffer *pad_pb = get_object_manager()->pktbuf_create(pad_buf, pad_len);
  cpkt->append(pad_pb);
  // Complain if packet is < 32 bytes long (see TOFECO-191)(JBay complain if < 64B)
  RMT_ASSERT(cpkt->len() >= kCoalMinSize);

  // Set cpkt metadata and version - can override from coal session cfg on Tofino
  uint8_t coal_ver = cpkt->mirror_metadata()->version();
  if (M->coal_has_version(slice, coal_num)) coal_ver = M->coal_ver(slice, coal_num);

  MirrorPktSetI2Qmetadata(M->coal_id_to_sess_id(slice, coal_num), cpkt, coal_ver);

  // Now we've setup i2q_metadata we can reset mirror_metadata to initial state
  cpkt->mirror_metadata()->reset();

  // clear the session
  coal_sessions_[slice][coal_num].clear();
  --active_coal_sessions_;
  RMT_ASSERT(active_coal_sessions_ >= 0);
  if (active_coal_sessions_ == 0) {
    coalescing_timer_.stop();
  }

  return cpkt;
}

}
