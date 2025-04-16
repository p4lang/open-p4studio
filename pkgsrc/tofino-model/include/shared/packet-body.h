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

#ifndef _SHARED_PACKET_BODY_
#define _SHARED_PACKET_BODY_

#include <string>
#include <vector>
#include <packet-buffer.h>


namespace MODEL_CHIP_NAMESPACE {

class Packet;

class PacketBody {
 public:
  PacketBody() = default;

  // Implement copy CTOR and copy assignment op...
  PacketBody(const PacketBody& other) {
    for (PacketBuffer* pb: other.packet_buffers_) packet_buffers_.push_back(pb->clone());
  }
  PacketBody& operator=(const PacketBody& other) {
    for (PacketBuffer* pb: other.packet_buffers_) packet_buffers_.push_back(pb->clone());
    return *this;
  }
  // ...but remove move CTOR and move assignment op
  PacketBody(PacketBody&& other) = delete;
  PacketBody& operator=(PacketBody&& other) = delete;

  ~PacketBody() { reset(); }


  void    set_packet(Packet *p) { packet_ = p; }
  Packet *get_packet() const { return packet_; }

  void reset();
  uint32_t hash() const;
  int  len() const;
  void insert(PacketBuffer *pb);
  void prepend(PacketBuffer* pb, int replace_bytes);
  void prepend_zeros(const int n);
  void append(PacketBuffer* pb);
  void append(PacketBody *pbod);
  void append_zeros(const int n);
  void pad_front(int bytes_to_pad);
  void pad_back(int bytes_to_pad);
  void trim_front(int bytes_to_trim);
  void trim_back(int bytes_to_trim);
  int  get_buf(uint8_t buf[], int start_pos, int len) const;
  void set_byte(int pos, uint8_t val);

 private:
  Packet                     *packet_{nullptr};
  std::vector<PacketBuffer*>  packet_buffers_{};
};

}
#endif // _SHARED_PACKET_DATA_
