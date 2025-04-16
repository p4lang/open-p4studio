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

#include <ipb-common.h>
#include <register_adapters.h>
#include <rmt-object-manager.h>
#include <common/rmt-util.h>


namespace MODEL_CHIP_NAMESPACE {

using namespace model_common;

IpbCountersCommon::IpbCountersCommon(int chipIndex, int pipeIndex, int ipbIndex, int chanIndex) :
  chnl_deparser_drop_pkt_(
      ipb_adapter(chnl_deparser_drop_pkt_, chipIndex, pipeIndex, ipbIndex, chanIndex)),
  chnl_wsch_discard_pkt_(
      ipb_adapter(chnl_wsch_discard_pkt_, chipIndex, pipeIndex, ipbIndex, chanIndex)),
  chnl_wsch_trunc_pkt_(
      ipb_adapter(chnl_wsch_trunc_pkt_, chipIndex, pipeIndex, ipbIndex, chanIndex)),
  chnl_parser_discard_pkt_(
      ipb_adapter(chnl_parser_discard_pkt_, chipIndex, pipeIndex, ipbIndex, chanIndex)),
  chnl_parser_send_pkt_(
      ipb_adapter(chnl_parser_send_pkt_, chipIndex, pipeIndex, ipbIndex, chanIndex)),
  chnl_deparser_send_pkt_(
      ipb_adapter(chnl_deparser_send_pkt_, chipIndex, pipeIndex, ipbIndex, chanIndex)),
  chnl_macs_received_pkt_(
      ipb_adapter(chnl_macs_received_pkt_, chipIndex, pipeIndex, ipbIndex, chanIndex)),
  // 64 bit wide counters by default
  wrap_counter_size_(64) {
}

void IpbCountersCommon::reset() {
  chnl_deparser_drop_pkt_.reset();
  chnl_wsch_discard_pkt_.reset();
  chnl_wsch_trunc_pkt_.reset();
  chnl_parser_discard_pkt_.reset();
  chnl_parser_send_pkt_.reset();
  chnl_deparser_send_pkt_.reset();
  chnl_macs_received_pkt_.reset();
}


void IpbCountersCommon::increment_chnl_deparser_drop_pkt() {
  chnl_deparser_drop_pkt_.chnl_dprs_drop_pkt(
      Util::increment_and_wrap(chnl_deparser_drop_pkt_.chnl_dprs_drop_pkt(),
                               wrap_counter_size_));
}

void IpbCountersCommon::increment_chnl_deparser_send_pkt() {
  chnl_deparser_send_pkt_.chnl_deparser_send_pkt(
      Util::increment_and_wrap(chnl_deparser_send_pkt_.chnl_deparser_send_pkt(),
                               wrap_counter_size_));
}

void IpbCountersCommon::increment_chnl_parser_send_pkt() {
  chnl_parser_send_pkt_.chnl_parser_send_pkt(
      Util::increment_and_wrap(chnl_parser_send_pkt_.chnl_parser_send_pkt(),
                               wrap_counter_size_));
}

void IpbCountersCommon::increment_chnl_parser_send_pkt_err() { /* not implemented for tofino */ }

void IpbCountersCommon::increment_chnl_macs_received_pkt() {
  chnl_macs_received_pkt_.chnl_macs_received_pkt(
      Util::increment_and_wrap(chnl_macs_received_pkt_.chnl_macs_received_pkt(),
                               wrap_counter_size_));
}


IpbCommon::IpbCommon(RmtObjectManager *om, int pipeIndex, int ipbIndex)
    : PipeObject(om, pipeIndex, ipbIndex, kType),
      override_(false), meta_enabled_(true), rx_enabled_(false) {
}
IpbCommon::~IpbCommon() {
}


// Get stashed timestamp if PktGen else timestamp from RmtObjectManager
uint64_t IpbCommon::get_global_timestamp(Packet *packet, int chan) {
  if (packet->is_generated()) return packet->generated_T();
  return (get_object_manager() != NULL) ? get_object_manager()->timestamp_get() : UINT64_C(0);
}


// This is the lowest level function - just writes what it's given
// Only examines flags to determine:
//   a) whether to byteswap meta1
//   b) whether to add leading zeros
//
Packet *IpbCommon::prepend_metadata_hdr(Packet *packet, int chan, uint32_t flags,
                                        uint8_t resubmit_flag, uint8_t version,
                                        uint16_t logical_port, uint64_t timestamp,
                                        uint64_t meta1[]) {
  static_assert( (kHeaderWordSizeBytes == 8),
                 "IpbCommon code assumes kHeaderWordSize is 8 - uses uint64_t");
  RMT_ASSERT(packet != NULL);

  uint64_t meta0 = meta0_make(resubmit_flag, version, logical_port, timestamp);
  uint64_t zeros = UINT64_C(0);
  uint8_t  hdr[kHeaderSizeBytes] = {0};
  int hdrlen = kHeaderSizeBytes, sz = kHeaderWordSizeBytes, pos = 0;

  if ((flags & kFlagsAddLeadingPadZero8B) != 0u) {
    pos = model_common::Util::fill_buf(hdr, hdrlen, pos, sz, zeros);
  }
  if (kMeta0SizeWords > 0) {
    // fill_buf puts in most significant bytes first
    pos = model_common::Util::fill_buf(hdr, hdrlen, pos, sz, meta0);
    if (kMeta0SizeWords > 1) {
      for (int i = 1; i < kMeta0SizeWords; i++) {
        pos = model_common::Util::fill_buf(hdr, hdrlen, pos, sz, zeros);
      }
    }
  }
  if (kMeta1SizeWords > 0) {
    // meta1 may need byte-swapping
    bool byteswap = ((flags & kFlagsMeta1IsByteSwapped) != 0u);
    for (int i = 0; i < kMeta1SizeWords; i++) {
      int i2 = (byteswap) ?i :(kMeta1SizeWords-1-i);
      pos = model_common::Util::fill_buf(hdr, hdrlen, pos, sz, meta1[i2], byteswap);
    }
  }
  while (pos < kHeaderSizeBytes) {
    int n = std::min(kHeaderSizeBytes - pos, sz);
    pos = model_common::Util::fill_buf(hdr, hdrlen, pos, n, zeros);
  }
  RMT_ASSERT(pos == kHeaderSizeBytes);

  packet->prepend_metadata_hdr(hdr, pos);
  return packet;
}


// Higher level function.
// Figures out what to do from Packet and flags
// May use IPB registers for version/logical_port/meta1
// Can derive meta1 from resubmit_header
// Can also use values from original packet if there were any
// Honours meta_enabled setting - sometimes disabled by unit tests
//
Packet *IpbCommon::prepend_metadata_hdr(Packet *packet, int chan, uint32_t flags) {
  RMT_ASSERT(packet != NULL);
  if (!is_chan_meta_enabled(chan)) return packet;

  // META0 - initialize
  //
  uint8_t  resubmit_flag = 0;
  uint8_t  version = 0;
  uint16_t logical_port = 0;
  uint64_t timestamp = UINT64_C(0);
  uint64_t meta1[kMeta1SizeWords] = { UINT64_C(0) };

  // META0 - get values from registers
  //
  get_register_metadata(packet, chan, flags,
                        &resubmit_flag, &version, &logical_port, &timestamp, meta1);

  // META0
  // Now override by getting values from Packet *data buffer*
  //
  // NB We derive resubmit from Packet *object* - so pass NULL
  // NB We never allow timestamp to be overwritten - pass NULL
  // NB If registers are overwriting meta1 we temporarily clear the
  //    kFlagsPktHasMeta1 flag to disable getting meta1 from Packet hdr
  //
  // Call twice - second time pass true to trim metadata from Packet
  //
  // XXX: overwrite_meta1 ignored for PktGen packets on JBay (so false)
  bool ovwrt_meta1 = overwrite_meta1(packet, chan);

  uint32_t tmp_flags = (ovwrt_meta1) ?(flags & ~kFlagsPktHasMeta1) :flags;
  if (tmp_flags != 0u) {
    get_packet(packet, chan, tmp_flags,
               NULL, &version, &logical_port, NULL, meta1, false);
  }
  if (flags != 0u) {
    uint64_t dummy[kMeta1SizeWords];
    get_packet(packet, chan, flags, NULL, NULL, NULL, NULL, dummy, true);
  }

  // META0
  // And then override by getting values from Packet *object*
  //
  get_packet_metadata(packet, chan, flags,
                      &resubmit_flag, &version, &logical_port, NULL);

  // META1
  // Now override meta1 if packet is a resubmit
  //
  if (resubmit_flag != 0) {
    get_resubmit_metadata(packet, chan, flags, meta1);
    get_ipb_counters(chan)->increment_chnl_resubmit_received_pkt();
  }

  // XXX:
  // Find max packet *data* byte that will be given to Parser.
  // This determined by the sun of IBP prsr_dph_max CSR.
  // Add on the IBP header_size to determine parsable len.
  // Store this inside the packet for the Parser to access.
  packet->set_parsable_len(get_max_byte(packet) + kHeaderSizeBytes);

  // XXX
  // Install version in packet - this is what is used for Parser
  // TCAM lookup, and is the value installed in the Phv for MAU use.
  // (On JBay/WIP it can also be updated using Parser ActionRAM config)
  packet->set_version(version);

  // Now have values for all fields and have trimmed any existing header
  // from Packet - can call low-level prepend_metadata_header
  //
  return prepend_metadata_hdr(packet, chan, flags,
                              resubmit_flag, version, logical_port, timestamp, meta1);
}


// Main entrypoint func from pipe.cpp
void IpbCommon::insert_metadata(Packet *packet, uint32_t channel,
                                bool resubmit, bool gen, bool recirc) {
  RMT_ASSERT(resubmit == packet->is_resubmit());
  (void)prepend_metadata_hdr( packet, channel, get_packet_flags(packet) );
}




// Extract metadata from registers (uses per-chip funcs)
void IpbCommon::get_register_metadata(Packet *packet, int chan, uint32_t flags,
                                      uint8_t *resubmit_flag, uint8_t *version,
                                      uint16_t *logical_port, uint64_t *timestamp,
                                      uint64_t meta1[]) {
  RMT_ASSERT(packet != NULL);

  if (resubmit_flag != NULL) *resubmit_flag = 0;
  if (version != NULL)       *version = get_version(packet, chan);
  if (logical_port != NULL)  *logical_port = get_logical_port(packet, chan);
  if (timestamp != NULL)     *timestamp = get_global_timestamp(packet, chan);

  // If the overwrite_meta1 field is set then we construct the meta1 data from
  // the chnl_meta registers but if overwrite_meta1 is not set then we leave it
  // at zero.
  // XXX: overwrite_meta1 ignored for PktGen packets on JBay (so false)
  //            however not on Tofino/WIP (so true)
  if (overwrite_meta1(packet, chan))
    for (int i = 0; i < kMeta1SizeWords; i++) meta1[i] = get_meta1(packet, chan, i);
  else
    for (int i = 0; i < kMeta1SizeWords; i++) meta1[i] = 0;
}


// Extract information from packet *data buffer*
// NB This is explicitly conveyed by setting certain flags on
// call to prepend_metadata_hdr
void IpbCommon::get_packet(Packet *packet, int chan, uint32_t flags,
                           uint8_t *resubmit_flag, uint8_t *version,
                           uint16_t *logical_port, uint64_t *timestamp,
                           uint64_t meta1[], bool trim) {
  static_assert( (kHeaderWordSizeBytes == 8),
                 "IpbCommon code assumes kHeaderWordSize is 8 - uses uint64_t");
  RMT_ASSERT(packet != NULL);

  int sz = kHeaderWordSizeBytes;
  int pos = 0;

  // Depending on flags maybe fill stuff in from headers
  // already on packet. We will trim these off it told to
  //
  if ((flags & kFlagsPktHasLeadingPadZero8B) != 0u) pos += 8;

  if ((flags & kFlagsPktHasMeta0) != 0u) {
    uint64_t meta0 = UINT64_C(0);

    for (int i = 0; i < kMeta0SizeWords; i++) {
      uint8_t meta0_buf[kHeaderWordSizeBytes];
      int get0 = packet->get_buf(meta0_buf, pos, sz);
      RMT_ASSERT(get0 == sz);
      if (i == 0) {
        // We assume the metadata we want is in the first 64b
        int fill0 = model_common::Util::fill_val(&meta0, sz, meta0_buf, sz, 0);
        RMT_ASSERT(fill0 == sz);
      }
      pos += get0;
    }
    if (resubmit_flag != NULL) *resubmit_flag = meta0_get_resubmit_flag(meta0);
    if (version != NULL)       *version = meta0_get_version(meta0);
    if (logical_port != NULL)  *logical_port = meta0_get_logical_port(meta0);
    if (timestamp != NULL)     *timestamp = meta0_get_timestamp(meta0);
  }
  if ((flags & kFlagsPktHasMeta1) != 0u) {
    for (int i = 0; i < kMeta1SizeWords; i++) {
      // First bytes off wire are more significant than later bytes.
      // And meta1[1] is more significant than meta1[0].
      // Which is why we fill up meta1 'bottom to top' like this.
      uint8_t meta1_buf[kHeaderWordSizeBytes];
      int get1  = packet->get_buf(meta1_buf, pos, sz);
      int fill1 = model_common::Util::fill_val(&meta1[kMeta1SizeWords-1-i], sz, meta1_buf, sz, 0);
      RMT_ASSERT((get1 == sz) && (fill1 == sz));
      pos += get1;
    }
  }

  if ((flags & kFlagsPktHasTrailingPadZero8B) != 0u) pos += 8;

  if (trim) {
    int trim = packet->trim_front(pos);
    RMT_ASSERT(trim == pos);
  }
}


// Extract metadata from Packet *object*
void IpbCommon::get_packet_metadata(Packet *packet, int chan, uint32_t flags,
                                    uint8_t *resubmit_flag, uint8_t *version,
                                    uint16_t *logical_port, uint64_t *timestamp) {
  // Always override values based on state of Packet *object*
  if (resubmit_flag != NULL) {
    if (packet->is_resubmit())
      *resubmit_flag = 1;
  }
  if (logical_port != NULL) {
    if (packet->is_generated())
      *logical_port = packet->i2qing_metadata()->physical_ingress_port();
  }
}


// Extract metadata (just meta1) from resubmit PacketBuffer within Packet object
void IpbCommon::get_resubmit_metadata(Packet *packet, int chan, uint32_t flags,
                                      uint64_t meta1[]) {
  static_assert( (kHeaderWordSizeBytes == 8),
                 "IpbCommon code assumes kHeaderWordSize is 8 - uses uint64_t");
  RMT_ASSERT(packet != NULL);

  PacketBuffer *resub = packet->get_resubmit_header();
  if (resub != NULL) {

    // Fish out resub hdr into fixed-size buf with trailing-zero pad
    // Note, on WIP Meta1 bytes from resub may be less than normal Meta1 bytes
    uint8_t meta1_resub_buf[kMeta1ResubSizeBytes] = { 0 };
    int resublen = resub->len();
    int getbytes = (resublen <= kMeta1ResubSizeBytes) ?resublen :kMeta1ResubSizeBytes;
    int gotbytes = (getbytes > 0) ?resub->get_buf(meta1_resub_buf, 0, getbytes) :0;
    RMT_ASSERT(gotbytes == getbytes);

    // Then copy into meta1 words and return
    // Note1: this logic assumes meta1[N] more signif than meta1[N-1]
    // Note2: in case (WIP) where kMeta1ResubSizeWords < kMeta1SizeWords
    //        then meta1[0] remains untouched
    int sz = kHeaderWordSizeBytes;
    int pos = 0;
    for (int i = 0; i < kMeta1ResubSizeWords; i++) {
      model_common::Util::fill_val(&meta1[kMeta1SizeWords - 1 - i], sz,
                                   meta1_resub_buf, kMeta1ResubSizeBytes, pos);
      pos += sz;
    }
    RMT_ASSERT(pos == kMeta1ResubSizeBytes);
  }
}


}
