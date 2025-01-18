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

#ifndef _SHARED_BITVECTOR_
#define _SHARED_BITVECTOR_

#include <iostream>
#include <array>
#include <cinttypes>
#include <common/rmt-assert.h>
#include <rmt-log.h>
#include <common/rmt-util.h>
#include <boost/format.hpp>

// TODO: this class is pretty efficient in time because of the templating,
//  but not in space for small bit vectors because the unit of allocation
//  is a 64 bit word. Could fix this so smaller words were used when possible.

namespace MODEL_CHIP_NAMESPACE {

  template <int BV_SIZE>
      class BitVector {
 public:
    static const int kWords = ((BV_SIZE-1) / 64) + 1;

    static uint64_t tcam_compare_func(uint64_t w0, uint64_t w1,
                                      uint64_t s0, uint64_t s1) {
      //uint64_t val = ~(s0|s1) | (w0&w1) | (s0&w0) | (s1&w1);
      //uint64_t msk = ~(s0&s1) | (w0&w1);
      //return ~(val & msk);
    // Dimitri says this does the same as the truth table in the uArch document
      return (((~w0) & s0 ) | ((~w1) & s1 ));
    }
    static bool tcam_compare(const BitVector<BV_SIZE>& bv_w0,
                             const BitVector<BV_SIZE>& bv_w1,
                             const BitVector<BV_SIZE>& bv_s0,
                             const BitVector<BV_SIZE>& bv_s1) {
      for (int i = 0; i < kWords; i++) {
        if (tcam_compare_func(bv_w0.bits_[i], bv_w1.bits_[i],
                              bv_s0.bits_[i], bv_s1.bits_[i]) != UINT64_C(0))
          return false;
      }
      return true;
    }

 public:
    BitVector() {}
    ~BitVector() {}

    // constructor from a correctly-sized array of any standard integer
    template<typename T>
        explicit BitVector(const std::array<T,((BV_SIZE-1)/(8*sizeof(T)) + 1)>& in) {
      static_assert( std::is_integral<T>::value, "only integral types allowed" );
      set_all(in);
    }
    // specialization for 64 bit
    explicit BitVector(const std::array<uint64_t,kWords>& in) : bits_(in) {
      mask_last_word();
    }
    // specialization for single 64 bit arg
    explicit BitVector(const uint64_t repeat) {
      for ( uint64_t& b : bits_ ) b = repeat;
      mask_last_word();
    }


    void copy_from(const BitVector<BV_SIZE>& from) {
      bits_ = from.bits_;
    }

    // This template lets you do this:
    //    BitVector<128> bv{ 0xfffffffffffffffful , 0xfffffffffffffffful };
    //  but too dangerous? if the values are not 64 bit they will be converted
    //    and then there will gaps.
    //template<typename... T> explicit BitVector(T... ts ) : bits_{ts...} {}

    // something might be possible like this and then copy the initializer_list
    //  into bits_
    //explicit BitVector( std::initializer_list<uint64_t> il ) { }

    template<typename T>
        void set_all(const std::array<T,((BV_SIZE-1)/(8*sizeof(T)) + 1)>& in) {
      static_assert( std::is_integral<T>::value, "only integral types allowed" );
      static_assert(! std::is_same< T, bool >::value,"doesn't work for bool");
      constexpr int inWidth   = 8*sizeof(T);
      constexpr int inPerWord = 64 / inWidth;
      constexpr int inTotal = ((BV_SIZE-1)/(8*sizeof(T)) + 1);
      int i=0;
      for ( uint64_t& b : bits_ ) {
        b=0;
        for (int j=0; j<inPerWord; ++j,++i) {
          if (i<inTotal) b |= (static_cast<uint64_t>(in[i])) << (j*inWidth);
        }
      }
      mask_last_word();
    }
    // specialization for 64 bit
    void set_all(const std::array<uint64_t,kWords>& in) {
      bits_ = in;
      mask_last_word();
    }
    void set_all(const char *hexstr) {
      int len = (BV_SIZE / 8) * 2;
      std::array<uint8_t,((BV_SIZE-1)/8)+1> tmpbuf;
      if (model_common::Util::hexmakebuf(hexstr, len, tmpbuf.data()) == len)
        set_all(tmpbuf);
    }

    const BitVector& fill_all(const uint8_t in) {
      uint64_t word=0;
      for (int i=0;i<8;++i) {
        word |= static_cast<uint64_t>(in) << (i*8);
      }
      bits_.fill(word);
      mask_last_word();
      return *this;
    }
    const BitVector& fill_all_ones() {
      bits_.fill( UINT64_C(0xFFFFFFFFFFFFFFFF) );
      mask_last_word();
      return *this;
    }
    const BitVector& fill_all_zeros() {
      bits_.fill( UINT64_C(0x0) );
      mask_last_word();
      return *this;
    }

    bool bit_set(int bit_offset) const {
      return ((get_bit(bit_offset) & 0x1) == 0x1);
    }
    uint8_t get_bit(int bit_offset) const {
      return static_cast<uint8_t>(get_word(bit_offset, 1));
    }
    uint8_t get_byte(int byte_offset) const {
      return static_cast<uint8_t>(get_word(byte_offset * 8, 8));
    }
    void set32(int word_offset, uint32_t word) {
      RMT_ASSERT( word_offset>=0 && word_offset*32 < BV_SIZE );
      uint64_t bv_word = bits_[word_offset/2];
      if ((word_offset % 2) == 0) {
        bv_word = (bv_word & UINT64_C(0xFFFFFFFF00000000)) |
            ((static_cast<uint64_t>(word)) << 0);
      } else {
        bv_word = (bv_word & UINT64_C(0x00000000FFFFFFFF)) |
            ((static_cast<uint64_t>(word)) << 32);
      }
      bits_[word_offset/2] = bv_word;
      mask_last_word();
    }
    // TODO: make byte_offset the first parameter (and rename?)
    void set_byte(uint8_t byte, int byte_offset) {
      RMT_ASSERT( byte_offset*8 < BV_SIZE );
      int w = byte_offset / 8;
      int b = (byte_offset % 8) * 8;
      bits_[w] &= ~( UINT64_C( 0xff ) << b );
      bits_[w] |= static_cast<uint64_t>(byte) << b;
      mask_last_word();
    }
    uint64_t get_word(int bit_offset, int n_bits=64) const {
      uint64_t word = 0;
      if ((bit_offset>=0) && (bit_offset < BV_SIZE) && (n_bits > 0) && (n_bits <= 64)) {
        n_bits = std::min(n_bits, BV_SIZE - bit_offset);
        word = bits_[bit_offset / 64];
        int mod_bit_offset = bit_offset % 64;
        if (mod_bit_offset != 0) {
          word >>= mod_bit_offset;
          int spill = mod_bit_offset + n_bits - 64;
          int word_offset = (bit_offset / 64) + 1;
          if ((spill > 0) && (word_offset < kWords)) { // Might not spill into next uint64_t
            uint64_t word2 = bits_[word_offset];
            word |= (word2 << (n_bits - spill));
          }
        }
        if (n_bits < 64) {
          word &= (UINT64_C(1) << n_bits) - 1;
        }
      }
      return word;
    }
    // TODO: make bit_offset the first parameter
    void set_word(uint64_t word, int bit_offset, int n_bits=64) {
      if ((bit_offset>=0)&&(bit_offset < BV_SIZE) && (n_bits > 0) && (n_bits <= 64)) {
        n_bits = std::min(n_bits, BV_SIZE - bit_offset);
        int mod_bit_offset = bit_offset % 64;
        if (n_bits < 64) {
          word &= ((UINT64_C(1) << n_bits) - 1);
	  bits_[bit_offset / 64] &= ~(((UINT64_C(1) << n_bits) - 1) << mod_bit_offset);
        }
	else {
	  bits_[bit_offset / 64] &= ~(UINT64_C(0xFFFFFFFFFFFFFFFF) << mod_bit_offset);
	}
        bits_[bit_offset / 64] |= (word << mod_bit_offset);
        int spill = mod_bit_offset + n_bits - 64;
        int word_offset = (bit_offset / 64) + 1;
        if ((spill > 0) && (word_offset < kWords)) { // Might not spill into next uint64_t
          bits_[word_offset] &= ~((UINT64_C(1) << spill) - 1);
          bits_[word_offset] |= (word >> (n_bits - spill));
        }
      }
    }
    // TODO: make bit_offset the first parameter
    void set_bit(bool v, int bit_offset) {
      set_word( v, bit_offset, 1 );
    }
    void set_bit(int bit_offset) {
      set_word( 1, bit_offset, 1 );
    }
    void clear_bit(int bit_offset) {
      set_word( 0, bit_offset, 1 );
    }

    void set_range(uint64_t word, int bit_offset, int n_bits) {
      int bits_written = 0;
      while (bits_written < n_bits) {
        int bits_to_write = std::min(n_bits - bits_written, 64);
        set_word(word, bit_offset + bits_written, bits_to_write);
        bits_written += bits_to_write;
      }
    }
    void set_ones(int bit_offset, int n_bits) {
      set_range(UINT64_C(0xFFFFFFFFFFFFFFFF), bit_offset, n_bits);
    }

    // TODO: make this use BV_SIZE instead of OTHER_SIZE
    //   surely it's better if it doesn't compile than just always returns false?
    template<int OTHER_SIZE>
        bool equals(const BitVector<OTHER_SIZE>& other) const {
      if (BV_SIZE != OTHER_SIZE) return false;
      int cmpWholeWords = BV_SIZE / 64;
      int bitsLeft = BV_SIZE % 64;
      for (int i = 0; i < cmpWholeWords; ++i) {
        if (bits_[i] != other.bits_[i]) return false;
      }
      if (bitsLeft > 0) {
        int i = cmpWholeWords;
        uint64_t mask = ((UINT64_C(1) << bitsLeft) - 1) ;
        if ((bits_[i] & mask) != (other.bits_[i] & mask)) return false;
      }
      return true;
    }
    template<int OTHER_SIZE>
        bool masked_equals(const BitVector<OTHER_SIZE>& other,
                           const BitVector<OTHER_SIZE>& mask) const {
      if (BV_SIZE < OTHER_SIZE) return false;
      int maskWords = ((OTHER_SIZE - 1) / 64) + 1;
      for (int i = 0; i < maskWords; ++i) {
        uint64_t maskTmp = mask.bits_[i];
        if ((bits_[i] & maskTmp) != (other.bits_[i] & maskTmp)) return false;
      }
      return true;
    }
    // Test if q_bv == this (s) using a mask and s0q1_enable
    //   (sets which bits should detect mismatch when s=0 and q=1)
    //   and s1q0_enable (mismatch when s=1 and q=0).
    //   If both s0q1_enable and s1q0_enable are all ones then
    //   is just a normal masked compare
    template<int Q_SIZE>
        bool masked_s0q1_s1q0_equals(const BitVector<Q_SIZE>& q_bv,
                                     const BitVector<Q_SIZE>& mask_bv,
                                     const BitVector<Q_SIZE>& s0q1_enable_bv,
                                     const BitVector<Q_SIZE>& s1q0_enable_bv
                                     ) const {
      RMT_ASSERT(BV_SIZE >= Q_SIZE);
      int maskWords = ((Q_SIZE - 1) / 64) + 1;
      for (int i = 0; i < maskWords; ++i) {
        uint64_t mask   = mask_bv.bits_[i];
        uint64_t s      = bits_[i];
        uint64_t q      = q_bv.bits_[i];
        uint64_t s0q1   = s0q1_enable_bv.bits_[i];
        uint64_t s1q0   = s1q0_enable_bv.bits_[i];
        uint64_t mismatch_s0_q1 = s0q1 & (~s) &   q ;
        uint64_t mismatch_s1_q0 = s1q0 &   s  & (~q);
        uint64_t mismatch = (mismatch_s0_q1 | mismatch_s1_q0) & mask;
        if (mismatch != 0)
          return false;
      }
      return true;
    }
    // Fills other with bits from this bitvector starting at bit_offset.
    // Typically use this to copy from big bitvectors into smaller bitvectors.
    // Amount to copy MUST meet or exceed capacity of other
    template<int EXTRACT_SIZE>
        void extract_into(int bit_offset,
                          BitVector<EXTRACT_SIZE> *other) const {
      RMT_ASSERT ((BV_SIZE - bit_offset >= EXTRACT_SIZE) && (other != NULL));
      int offset = 0;
      while (offset + bit_offset < BV_SIZE) {
        uint64_t word = get_word(offset + bit_offset);
        other->set_word(word, offset);
        offset += 64;
      }
    }
    // Set this bit vector starting at offset bit_offset from bit vector "in"
    // Typically use this to copy from smaller bitvectors into bigger bitvectors
    // Amount to copy MUST NOT exceed capacity of this
    template<int IN_SIZE>
        void set_from(int bit_offset, const BitVector<IN_SIZE>& in) {
      RMT_ASSERT (BV_SIZE - bit_offset >= IN_SIZE);
      int offset = 0;
      while (offset < IN_SIZE) {
        uint64_t word = in.get_word(offset);
        set_word(word, offset + bit_offset, std::min(64,IN_SIZE-offset));
        offset += 64;
      }
    }
    // Find the first bit set in BitVector after position after_pos
    int get_first_bit_set(int after_pos=-1) const {
      if (after_pos > BV_SIZE) return -1;
      int which_word = 0;
      uint64_t word;
      if (after_pos >= 0) {
        // Mask-off bits upto and including after_pos
        which_word = after_pos / 64;
        int word_pos = after_pos % 64;
        if (word_pos == 63) {
          which_word++;
          word_pos = -1;
          if (which_word >= kWords) return -1;
        }
        uint64_t mask = UINT64_C(0xFFFFFFFFFFFFFFFF) << (word_pos+1);
        word = bits_[which_word ] & mask;
      } else {
        which_word = 0;
        word = bits_[0];
      }
      // Find a non-zero word
      while ((word == UINT64_C(0)) && (++which_word < kWords)) {
        word = bits_[which_word ];
      }
      // Find lowest bit set within it using ffsll builtin
      if (which_word < kWords) {
        return __builtin_ffsll(word) - 1 + which_word*64;
      } else {
        return -1;
      }
    }
    // Find the LAST bit set in BitVector after position after_pos
    int get_last_bit_set() const {
      for (int i=kWords-1; i>=0; i--) {
        if (bits_[i] != UINT64_C(0))
          return (i*64) + __builtin_clzll(UINT64_C(1)) - __builtin_clzll(bits_[i]);
      }
      return -1;
    }

    void invert() {
      for (int i=0; i<kWords ; ++i) {
        bits_[i] = ~bits_[i];
      }
      mask_last_word();
    }
    void mask(const BitVector<BV_SIZE>& mask) {
      for (int i=0; i<kWords ; ++i) {
        bits_[i] &= mask.bits_[i];
      }
      mask_last_word();
    }
    void or_with(const BitVector<BV_SIZE>& orval) {
      for (int i=0; i<kWords ; ++i) {
        bits_[i] |= orval.bits_[i];
      }
      mask_last_word();
    }
    bool intersects_with(const BitVector<BV_SIZE>& other) const {
      for (int i=0; i<kWords ; ++i) {
        if ((bits_[i] & other.bits_[i]) != 0) return true;
      }
      return false;
    }
    bool is_zero() const {
      for (int i=0; i<kWords ; ++i) {
        if (bits_[i] != 0) return false;
      }
      return true;
    }
    int popcount() const {
      int cnt = 0;
      for (int i=0; i<kWords ; ++i) {
        cnt += __builtin_popcountll(bits_[i]);
      }
      return cnt;
    }
    void swap_byte_order() {
      RMT_ASSERT((BV_SIZE % 8) == 0);
      int n_bytes = BV_SIZE / 8;
      for (int i=0; i < n_bytes/2; ++i) {
        uint8_t bx = get_byte(i);
        uint8_t by = get_byte(n_bytes-1-i);
        set_byte(by, i);
        set_byte(bx, n_bytes-1-i);
      }
    }

    void byte_shift_left(int shift) {
      RMT_ASSERT((BV_SIZE % 8) == 0);
      RMT_ASSERT(shift >= 0);
      if (shift==0) return;
      constexpr int n_bytes = BV_SIZE / 8;
      for (int i=n_bytes-1; i >=0; --i) {
        uint8_t bx = ( i >= shift ) ? get_byte(i-shift) : 0;
        set_byte(bx, i);
      }
    }

    // this templated version is 10% faster than non-templated version with constexpr
    //  args, and it forces the parameters to be constexpr (which is 2x faster than
    //  variable args), so best use this one.
    template< const int BIT_OFFSET, const int N_BITS >
      uint8_t parity() const {
        return parity( BIT_OFFSET, N_BITS );
    }

    template< const int BIT_OFFSET, const int N_BITS >
        uint8_t masked_parity(const BitVector<BV_SIZE>& mask) const {
      return masked_parity( mask, BIT_OFFSET, N_BITS );
    }


    // More masked_x funcs to apply various 64b funcs to selected words in BV

    void masked_set_ones(uint64_t selector) {
      for (int i = 0; i < kWords; i++) {
        if (((selector >> i) & 1) == 1) bits_[i] = UINT64_C(0xFFFFFFFFFFFFFFFF);
      }
      mask_last_word();
    }
    void masked_quarter(uint64_t selector) {
      for (int i = 0; i < kWords; i++) {
        if (((selector >> i) & 1) == 1)
          bits_[i] = model_common::Util::sel_quarter(bits_[i]);
      }
    }
    void masked_adjacent_or(uint64_t selector) {
      for (int i = 0; i < kWords; i++) {
        if (((selector >> i) & 1) == 1)
          bits_[i] = model_common::Util::adjacent_or(bits_[i]);
      }
    }
    uint64_t masked_concat(uint64_t selector, uint8_t off, uint8_t width,
                           bool zero_upper, bool update_selector) {
      RMT_ASSERT((width <= 32) && (off > 0));
      uint64_t mask = UINT64_C(0xFFFFFFFFFFFFFFFF) >> (64-width);
      for (int i = 0; i < kWords; i++) {
        if ( (((selector >> i) & 1) == 1) && (i + off < kWords) ) {
          bits_[i] = ((bits_[i + off] & mask) << width) | ((bits_[i] & mask) << 0);
          if (zero_upper) bits_[i + off] = UINT64_C(0);
          if (update_selector) selector &= ~( UINT64_C(1) << (i + off) );
        }
      }
      return selector;
    }


    void print(RmtLogger *logger=nullptr) const {
      for (uint64_t b : bits_ ) {
        if (nullptr == logger) {
          printf("%016" PRIx64 "\n",b);
        }
        else {
          RMT_LOG_OBJ(logger, RmtDebug::verbose(), "%016" PRIx64 "\n",b);
        }
      }
    }
    void print_with_printf() const {
      for (uint64_t b : bits_ ) {
        printf("%016" PRIx64 "\n",b);
      }
    }

    std::string to_string() const {
      std::string r = "";
      constexpr int last_width = (BV_SIZE%64) ? (BV_SIZE%64) : 64;
      constexpr int last_nibble_width = (last_width + 4 - 1)/4;
      static const std::string last_format = (boost::format("%%0%dx") %
                                              last_nibble_width ).str();
      for (int i=0; i < (kWords-1); ++i) {
        r =  (boost::format("%016x") % bits_[i]).str() + r;
      }
      r =  (boost::format(last_format) % bits_[kWords-1]).str() + r;
      return r;
    }

 private:
    std::array<uint64_t,kWords> bits_ { };

    // from http://www-graphics.stanford.edu/~seander/bithacks.html#ParityParallel
    inline uint8_t parity( uint64_t v ) const {
      v ^= v >> 32;
      v ^= v >> 16;
      v ^= v >> 8;
      v ^= v >> 4;
      v &= 0xf;
      return (0x6996u >> v) & 1u;
    }

    inline void mask_last_word() {
      int last_word_bits = BV_SIZE % 64;
      if (last_word_bits > 0) {
        int last_word = (BV_SIZE-1)/64;
        bits_[last_word] &= ((UINT64_C(1) << last_word_bits) - 1);
      }
    }

    inline uint8_t parity(const int bit_offset,const int n_bits) const {
      RMT_ASSERT( (bit_offset + n_bits) <= BV_SIZE );
      uint8_t p=0;
      int first_word = bit_offset / 64;
      int last_bit   = bit_offset + n_bits - 1;
      int last_word  = last_bit / 64;
      int first_bit_pos = bit_offset % 64;
      uint64_t first_mask = UINT64_C(0xFFFFFFFFFFFFFFFF) << first_bit_pos;
      uint64_t last_bit_pos = last_bit % 64;
      uint64_t last_mask = UINT64_C(0xFFFFFFFFFFFFFFFF) >> (63-last_bit_pos);
      if (first_word == last_word) {
        first_mask &= last_mask;
      }
      //printf("first_word = %d, last_word = %d, last_bit = %d, first_bit_pos = %d, first_mask = %016" PRIx64 ",last_mask = %016" PRIx64 "\n",
      //       first_word,last_word,last_bit,first_bit_pos,first_mask,last_mask);
      // mask and do first word
      p ^= static_cast<uint8_t>(parity( bits_[first_word] & first_mask ));
      // do all middle words
      for (int i=first_word+1; i<last_word; ++i) {
        p ^= static_cast<uint8_t>(parity( bits_[i] ));
      }
      // mask and do last word
      if (last_word != first_word) {
        p ^= static_cast<uint8_t>(parity( bits_[last_word] & last_mask ));
      }
      return p;
    }

    inline uint8_t masked_parity(const BitVector<BV_SIZE>& mask,const int bit_offset,const int n_bits) const {
      RMT_ASSERT( (bit_offset + n_bits) <= BV_SIZE );
      uint8_t p=0;
      int first_word = bit_offset / 64;
      int last_bit   = bit_offset + n_bits - 1;
      int last_word  = last_bit / 64;
      int first_bit_pos = bit_offset % 64;
      uint64_t first_mask = UINT64_C(0xFFFFFFFFFFFFFFFF) << first_bit_pos;
      uint64_t last_bit_pos = last_bit % 64;
      uint64_t last_mask = UINT64_C(0xFFFFFFFFFFFFFFFF) >> (63-last_bit_pos);
      if (first_word == last_word) {
        first_mask &= last_mask;
      }
      //printf("first_word = %d, last_word = %d, last_bit = %d, first_bit_pos = %d, first_mask = %016" PRIx64 ",last_mask = %016" PRIx64 "\n",
      //       first_word,last_word,last_bit,first_bit_pos,first_mask,last_mask);
      // mask and do first word
      p ^= static_cast<uint8_t>(parity( bits_[first_word] & first_mask & mask.bits_[first_word] ));
      // do all middle words
      for (int i=first_word+1; i<last_word; ++i) {
        p ^= static_cast<uint8_t>(parity( bits_[i] & mask.bits_[i] ));
      }
      // mask and do last word
      if (last_word != first_word) {
        p ^= static_cast<uint8_t>(parity( bits_[last_word] & last_mask & mask.bits_[last_word]));
      }
      return p;
    }


    // TODO: want this to be non-copyable, but some parser code relies on this
    //   at the moment
    //DISALLOW_COPY_AND_ASSIGN(BitVector);
  };
}

#endif
