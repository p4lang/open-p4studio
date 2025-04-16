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
#include <packet-body.h>


namespace MODEL_CHIP_NAMESPACE {

void PacketBody::reset() {
  // Free up packet buffers
  for (PacketBuffer* pb : packet_buffers_) delete pb;
  packet_buffers_.clear();
}

uint32_t PacketBody::hash() const {
  uint32_t hashval = 0u;
  for (PacketBuffer* pb : packet_buffers_) hashval ^= pb->hash();
  return hashval;
}
int PacketBody::len() const {
  int len = 0;
  for (PacketBuffer* pb : packet_buffers_) len += pb->len();
  return len;
}
void PacketBody::insert(PacketBuffer *pb) {
  (void)packet_buffers_.insert(packet_buffers_.begin(), pb);
}


// We take ownership of prepended/appended PacketBuffers
void PacketBody::prepend(PacketBuffer* pb, int replace_bytes) {
  DO_ASSERT((replace_bytes >= 0), "PacketBody: bad prepend");
  if (replace_bytes > len()) replace_bytes = len();
  trim_front(replace_bytes);
  (void)packet_buffers_.insert(packet_buffers_.begin(), pb);
}
void PacketBody::prepend_zeros(const int n) {
  prepend(new PacketBuffer(n), 0);
}


void PacketBody::append(PacketBuffer* pb) {
  (void)packet_buffers_.push_back(pb);
}
void PacketBody::append(PacketBody *pbod) {
  // append pkt buffers from one pkt data to the other
  // clone the pkt buffers
  for (PacketBuffer* pb : pbod->packet_buffers_) packet_buffers_.push_back(pb->clone());
}
void PacketBody::append_zeros(const int n) {
  append(new PacketBuffer(n));
}


void PacketBody::pad_front(int bytes_to_pad) {
  prepend_zeros(bytes_to_pad);
}
void PacketBody::pad_back(int bytes_to_pad) {
  append_zeros(bytes_to_pad);
}


void PacketBody::trim_front(int bytes_to_trim) {
  PacketBuffer *pb;
  int bytes_trimmed = 0;
  auto it = packet_buffers_.begin();
  while ((it != packet_buffers_.end()) && (bytes_trimmed < bytes_to_trim)) {
    pb = *it;
    int trim = std::min(bytes_to_trim - bytes_trimmed, pb->len());
    pb->trim_front(trim);
    bytes_trimmed += trim;
    ++it;
  }
  DO_ASSERT(bytes_trimmed == bytes_to_trim, "PacketBody: Unable to trim requested bytes.");
}
void PacketBody::trim_back(int bytes_to_trim) {
  PacketBuffer *pb;
  int trim_start = 0;
  int offset = 0;
  int bytes_trimmed = bytes_to_trim;

  trim_start = len() - bytes_to_trim;
  if (trim_start < 0) {
    trim_start = 0;
    bytes_trimmed = len();
  }
  auto it = packet_buffers_.begin();
  while (it != packet_buffers_.end()) {
    pb = *it;
    if ((offset + pb->len()) <= trim_start) {
      offset += pb->len();
      ++it;
      continue;
    }
    int trim_buf_offset = 0;
    if (offset < trim_start) {
      trim_buf_offset = (trim_start - offset);
    }
    pb->trim_back(pb->len() - trim_buf_offset);
    offset += pb->len();
    ++it;
  }
  DO_ASSERT(bytes_trimmed == bytes_to_trim, "PacketBody: Unable to trim requested bytes.");
}

int PacketBody::get_buf(uint8_t buf[], int start_pos, int len) const {
  int accum_pos = 0;
  int pb_pos = 0;
  PacketBuffer *pb;
  auto it = packet_buffers_.begin();

  // -ve start pos indicates position back from EOP
  if (start_pos < 0) {
    start_pos = this->len() + start_pos;
  }
  if (start_pos < 0) {
    return 0;
  }
  // Iterate through packet_buffers until we find
  // PacketBuffer that contains start position start_pos
  while (it != packet_buffers_.end()) {
    pb = *it;
    int pb_len = pb->len();
    if ((start_pos >= accum_pos) && (start_pos < accum_pos + pb_len)) {
      // Found PacketBuffer holding start_pos
      // Deduce start_pos position wrt this PackerBuffer (pb_pos)
      pb_pos = start_pos - accum_pos;
      break;
    } else {
      accum_pos += pb_len;
      ++it;
    }
  }
  // Iterate through packet_buffers copying bytes
  // till we exhaust len available in passed-in buf
  int got_len = 0;
  while (it != packet_buffers_.end()) {
    pb = *it;
    int copy_len = std::min(len - got_len, pb->len() - pb_pos);
    got_len += pb->get_buf(&buf[got_len], pb_pos, copy_len);
    if (got_len == len) break;
    // Reset pos for remaining PacketBuffers
    pb_pos = 0;
    ++it;
  }
  return got_len;
}


void PacketBody::set_byte(int pos, uint8_t val) {
  DO_ASSERT(((pos >= 0) && (pos < len())), "PacketBody: bad set_byte");
  for (auto *pkt : packet_buffers_) {
    if (pos < pkt->len()) {
      pkt->set_byte(pos, val);
      break;
    }
    pos -= pkt->len();
  }
}

}
