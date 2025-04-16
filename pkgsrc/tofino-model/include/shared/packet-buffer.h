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

#ifndef _SHARED_PACKET_BUFFER_
#define _SHARED_PACKET_BUFFER_

#include <string>
#include <memory>
#include <cstring>

#include <bitvector.h>
#include <common/rmt-util.h>

#ifndef DO_ASSERT
#include <common/rmt-assert.h>
#define DO_ASSERT(cond,str) RMT_ASSERT((cond) && (str))
#endif


namespace MODEL_CHIP_NAMESPACE {

  class PacketBuffer {

 public:
    PacketBuffer() {}
    PacketBuffer(const uint8_t buf[], size_t size)
        : buf_(std::shared_ptr<uint8_t>(new uint8_t[size],std::default_delete<uint8_t[]>())),
        size_(size), end_(size) {
      DO_ASSERT(buf || (size == 0), "PacketBuffer: Bad CTOR");
      if ( size != 0 ) {
        model_common::memcpy_uint8(buf_.get(), size, buf, size);
      }
    }
    PacketBuffer(const std::shared_ptr<uint8_t>& buf, size_t size)
        : buf_(buf), size_(size), end_(size) {
    }
    PacketBuffer(const BitVector<128> &bv)
        : buf_(std::shared_ptr<uint8_t>(new uint8_t[16],std::default_delete<uint8_t[]>())),
        size_(16), end_(16) {
      uint8_t *tmpbuf = buf_.get();
      // Assume BV is in Network Byte Order
      for (int i=0; i<16; i++) tmpbuf[i] = bv.get_byte(i);
    }
    // create a packet buffer filled with zeros
    PacketBuffer(const size_t size)
        : buf_(std::shared_ptr<uint8_t>(new uint8_t[size](),std::default_delete<uint8_t[]>())),
        size_(size), end_(size) {
    }
    ~PacketBuffer() { }

    uint32_t hash() {
      uint32_t hashval = 0u;
      uint8_t *tmpbuf = buf_.get();
      int pos = start_;
      while (pos < end_) {
        if (pos + (int)sizeof(uint32_t) <= end_) {
          hashval ^= *(uint32_t*)(tmpbuf + pos);
        } else {
          uint32_t c = (uint32_t) *(tmpbuf + pos + 0);
          if (pos + 1 < end_) c = (c << 8) | (uint32_t) *(tmpbuf + pos + 1);
          if (pos + 2 < end_) c = (c << 8) | (uint32_t) *(tmpbuf + pos + 2);
          hashval ^= c;
        }
        pos += sizeof(uint32_t);
      }
      return hashval;
    }
    int get_buf(uint8_t buf[], int pos, int len) {
      //printf("PacketBuffer::get_buf(%d,%d) start/end=(%d,%d)\n",
      //       pos,len,start_,end_);
      if (start_ + pos + len <= end_) {
        uint8_t *tmpbuf = buf_.get();
        model_common::memcpy_uint8(buf, len, &tmpbuf[start_ + pos], len);
      } else {
        len = 0;
      }
      return len;
    }
    inline int size() const {
      return size_;
    }
    inline int len() const {
      return end_ - start_;
    }
    inline int start() const {
      return start_;
    }
    inline int end() const {
      return end_;
    }
    inline void set_start(int start) {
      DO_ASSERT(((start >= 0) && (start <= size_) && (start <= end_)), "PacketBuffer: Bad set_start");
      start_ = start;
    }
    inline void set_end(int end) {
      DO_ASSERT(((end >= 0) && (end <= size_) && (end >= start_)), "PacketBuffer: Bad set_end");
      end_ = end;
    }
    inline void trim_front(int bytes_to_trim) {
      DO_ASSERT(((start_ + bytes_to_trim <= end_)), "PacketBuffer: Bad trim_front");
      start_ += bytes_to_trim;
    }
    inline void trim_back(int bytes_to_trim) {
      DO_ASSERT((start_ + bytes_to_trim <= end_), "PacketBuffer: Bad trim_back");
      end_ -= bytes_to_trim;
    }
    inline uint8_t get_byte(int pos) {
      DO_ASSERT(((pos >= 0) && (start_ + pos < end_)), "PacketBuffer: Bad get_byte");
      return *(buf_.get() + start_ + pos);
    }
    inline void set_byte(int pos, uint8_t val) {
      // DO_ASSERT(((pos >= 0) && (start_ + pos < end_)), "PacketBuffer: Bad set_byte");
      assert((pos >= 0) && (start_ + pos < end_));
      *(buf_.get() + start_ + pos) = val;
    }
    inline void swap_byte_order() {
      // XXX: Allow odd length buffer
      int ll = len();
      for (int i = 0; i < ll/2; ++i) {
        uint8_t bx = get_byte(i);
        uint8_t by = get_byte(ll-1-i);
        set_byte(i, by);
        set_byte(ll-1-i, bx);
      }
    }

    PacketBuffer *clone_reset() {
      return clone_internal(0, size_);
    }
    PacketBuffer *clone() {
      return clone_internal(start_, end_);
    }
    PacketBuffer *clone_head() {
      return clone_internal(0, start_);
    }
    PacketBuffer *clone_tail() {
      return clone_internal(end_, size_);
    }

    std::string to_string() {
      std::string r("");
      uint8_t *tmpbuf = buf_.get();
      for (int pos = start_; pos < end_; ++pos) {
        r += boost::str( boost::format("%02x") % static_cast<uint>(*(tmpbuf+pos)) );
      }
      return r;
    }
    void print(const char *s) {
      if (s != nullptr) printf("%s:", s);
      printf("Data=%s,%d [%d,%d])\n", buf_.get(),size_,start_,end_);
    }
    void print() { print(nullptr); }

 private:
    DISALLOW_COPY_AND_ASSIGN(PacketBuffer);

    inline PacketBuffer* clone_internal(int start, int end) const {
      PacketBuffer *clone_pb = new PacketBuffer();
      clone_pb->buf_ = buf_;
      clone_pb->size_ = size_;
      clone_pb->start_ = start;
      clone_pb->end_ = end;
      return clone_pb;
    }

    std::shared_ptr<uint8_t> buf_ = NULL;
    int size_ = 0;
    int start_ = 0;
    int end_ = 0;
  };
}
#endif // _SHARED_PACKET_BUFFER_
