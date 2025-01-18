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

#ifndef MODEL_COMMON_UTIL_
#define MODEL_COMMON_UTIL_

#include <cassert>
#include <cstring>
#include <string>
#include <cstdint>
#include <common/rmt-assert.h>
#include <common/disallow-copy-and-assign.h> // contains macro that used to be in this file

namespace model_common {

  class Util {
 public:
    static uint8_t  rotr8(uint8_t val, int r)    { return (val >> (r% 8)) | (val << ( 8-(r% 8))); }
    static uint16_t rotr16(uint16_t val, int r)  { return (val >> (r%16)) | (val << (16-(r%16))); }
    static uint32_t rotr32(uint32_t val, int r)  { return (val >> (r%32)) | (val << (32-(r%32))); }
    static uint8_t  rotl8(uint8_t val, int r)    { return (val << (r% 8)) | (val >> ( 8-(r% 8))); }
    static uint16_t rotl16(uint16_t val, int r)  { return (val << (r%16)) | (val >> (16-(r%16))); }
    static uint32_t rotl32(uint32_t val, int r)  { return (val << (r%32)) | (val >> (32-(r%32))); }

    static bool is_little_endian();
    static int  hexchar2int(char in);
    static int  hexmakebuf(const std::string hexstr, int len, uint8_t buf[]);
    static int  hexmakebuf(const char *hexstr, int len, uint8_t buf[]);

    // Fill a uint8_t buf of length buflen at pos with size bytes from arbitrary type T val
    template <typename T> static int fill_buf(uint8_t *buf, int buflen, int pos, int siz, T val,
                                              bool little_endian=false) {
      static_assert(std::is_integral<T>::value, "only integral types allowed" );
      static_assert(!std::is_same< T, bool >::value, "bool not allowed");
      assert((siz >= 0) && (static_cast<size_t>(siz) <= sizeof(T)) && (pos >= 0) && (pos + siz <= buflen));
      for (int i = 0; i < siz; i++) {
        int i2 = (little_endian) ?i :(siz-1-i);
        *(buf + pos + i) = static_cast<uint8_t>((val >> (i2*8)) & static_cast<T>(0xFF));
      }
      return pos+siz;
    }

    // Fill a val of type T with size bytes from pos within a uint8_t buf of length buflen
    template <typename T> static int fill_val(T *val, int siz, uint8_t *buf, int buflen, int pos,
                                              bool little_endian=false) {
      static_assert(std::is_integral<T>::value, "only integral types allowed" );
      static_assert(!std::is_same< T, bool >::value, "bool not allowed");
      assert((val != NULL) && (siz >= 0) && (static_cast<size_t>(siz) <= sizeof(T)));
      assert((pos >= 0) && (pos + siz <= buflen));
      for (int i = 0; i < siz; i++) {
        int i2 = (little_endian) ?i :(siz-1-i);
        T orbyte  =  ( static_cast<T>( *(buf + pos + i) ) << (i2*8) );
        T andmask = ~( static_cast<T>(             0xFF ) << (i2*8) );
        *val = (*val & andmask) | orbyte;
      }
      return pos+siz;
    }

    // increment <val> and wrap to zero if new value exceeds 2^<size> - 1
    template<typename T> static
    T increment_and_wrap(T val, int size, T amount=1) {
      RMT_ASSERT(size <= 64);
      uint64_t mask = UINT64_C(0xffffffffffffffff);
      return (val + amount) & (mask >> (64 - size));
    }


    // Select even bits from in[63:0] to form out[31:0]
    static uint64_t sel_half_even(uint64_t in) {
      uint64_t out = UINT64_C(0);
      for (int byte = 0; byte < 4; byte++) {
        uint16_t in16 = static_cast<uint16_t>( (in >> (byte*16)) & 0xFFFF );
        if (in16 != 0) {
          uint8_t out8 = 0; // See if even bits (bit0 or bit2) set
          if ((in16 & 0x0001) != 0) out8 |= 0x01;
          if ((in16 & 0x0004) != 0) out8 |= 0x02;
          if ((in16 & 0x0010) != 0) out8 |= 0x04;
          if ((in16 & 0x0040) != 0) out8 |= 0x08;
          if ((in16 & 0x0100) != 0) out8 |= 0x10;
          if ((in16 & 0x0400) != 0) out8 |= 0x20;
          if ((in16 & 0x1000) != 0) out8 |= 0x40;
          if ((in16 & 0x4000) != 0) out8 |= 0x80;
          out |= ( static_cast<uint64_t>(out8) << (byte*8) );
        }
      }
      return out;
    }
    // Select odd bits from in[63:0] to form out[31:0]
    static uint64_t sel_half_odd(uint64_t in) {
      return sel_half_even(in >> 1);
    }
    // Select every fourth bit (0,4 etc) from in[63:0] to form out[15:0]
    static uint64_t sel_quarter(uint64_t in) {
      return sel_half_even(sel_half_even(in));
    }
    // Or adjacent bits from in[63:0] to form out[31:0]
    static uint64_t adjacent_or(uint64_t in) {
      return sel_half_even(in) | sel_half_odd(in);
    }
  };


// XXX: klocwork complains about use of memcpy, so wrap its usage here to
// (a) remind the caller to consider the dest buffer size, (b) impose checking
// in a predictable manner and (c) reduce the klocwork complaints to a single
// location
inline void memcpy_uint8(uint8_t* dest, int dest_size, const uint8_t* src, int copy_size) {
  // check that src and dest do not overlap
  RMT_ASSERT(((src + copy_size) <= dest) || ((dest + dest_size) <= src));
  // check there is no overflow
  RMT_ASSERT(copy_size <= dest_size);
  (void)memcpy((void*)dest, (void*)src, (size_t)copy_size);
}

}

#endif // MODEL_COMMON_UTIL_
