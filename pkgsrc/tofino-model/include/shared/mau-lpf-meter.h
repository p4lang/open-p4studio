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

#ifndef _SHARED_MAU_LPF_METER_
#define _SHARED_MAU_LPF_METER_

#include <string>
#include <cstdint>
#include <rmt-defs.h>
#include <mau-defs.h>
#include <mau-object.h>
#include <bitvector.h>
#include <address.h>

namespace MODEL_CHIP_NAMESPACE {

class MauLpfMeter : public MauObject {
  
 public:
  MauLpfMeter(RmtObjectManager *om, int pipeIndex, int mauIndex, int logicalRowIndex,
              Mau *mau);
  ~MauLpfMeter();

  static constexpr int  kType = RmtTypes::kRmtTypeMauLpfMeter;

  static constexpr int  kDataBusWidth = MauDefs::kDataBusWidth;

  // Next func gets final input post any wacky forwarding behaviour
  void get_input_data(BitVector<kDataBusWidth> *data);

  void calculate_output(uint64_t present_time,
                        uint64_t relative_time, // just used to work out if forwarding
                        uint32_t addr,
                        bool meter_sweep,
                        bool red_enable,
                        bool red_only_mode, // JBay only
                        uint32_t red_nodrop_value,
                        uint32_t red_drop_value,
                        uint32_t D, // from meter hash group FIFO
                        uint64_t random_number,  // from LFSR
                        int meter_time_scale,
                        BitVector<kDataBusWidth>* data_in,
                        bool meter_lpf_sat_ctl,
                        BitVector<kDataBusWidth>* data_out,
                        uint32_t *action_data_out
                        );

  void update_cache(int old_addr, int new_addr) {
    if ((old_addr < 0) || (new_addr < 0)) return;
    if (Address::meter_addr_get_vaddr(old_addr) ==
        Address::meter_addr_get_vaddr(addr_)        ) {
      addr_ = new_addr;
    }
  }
  void update_cache_V(uint64_t T, uint32_t addr, uint32_t V) {
    cache_V(T, addr, V);
  }
  void update_cache_config_write(uint64_t T, uint32_t addr, BitVector<kDataBusWidth> *read_data, BitVector<kDataBusWidth> *write_data) {
    // update the cache with V set from the data as it was *before* being overwritten by the config write
    update_cache_V(T, addr, get_vold(read_data));
    // save a copy of the value read from the ram during the config write (ie the ram contents before the write happens), as
    //  this is the data the RTL sees if there is a phv access in the next cycle
    config_write_read_data_.copy_from(*read_data);
    // also need to save a copy of the write data, as this is what is replayed in the next cycle to prevent the write being
    //  dropped
    config_write_write_data_.copy_from(*write_data);
    config_write_T_ = T;
  }
  

 private:
  // generatated by script (below)
  static constexpr int kTimestampBitPos = 100;
  static constexpr int kTimestampWidth  = 28;
  static constexpr int kRateEnableBitPos = 99;
  static constexpr int kRateEnableWidth  = 1;
  static constexpr int kVoldBitPos = 64;
  static constexpr int kVoldWidth  = 32;
  static constexpr int kLpfActionScaleBitPos = 57;
  static constexpr int kLpfActionScaleWidth  = 5;
  static constexpr int kTimeConstantMantissaBitPos = 43;
  static constexpr int kTimeConstantMantissaWidth  = 9;
  static constexpr int kRiseTimeConstantExponentBitPos = 38;
  static constexpr int kRiseTimeConstantExponentWidth  = 5;
  static constexpr int kFallTimeConstantExponentBitPos = 33;
  static constexpr int kFallTimeConstantExponentWidth  = 5;
  static constexpr int kRedProbabilityScaleBitPos = 29;
  static constexpr int kRedProbabilityScaleWidth  = 3;
  static constexpr int kRedLevelExponentBitPos = 24;
  static constexpr int kRedLevelExponentWidth  = 5;
  static constexpr int kRedLevelMaxBitPos = 16;
  static constexpr int kRedLevelMaxWidth  = 8;
  static constexpr int kReddLevel100BitPos = 8;
  static constexpr int kReddLevel100Width  = 8;
  static constexpr int kRedLevel0BitPos = 0;
  static constexpr int kRedLevel0Width  = 8;

  uint32_t get_timestamp( BitVector<kDataBusWidth> *d ){
      return d->get_word(kTimestampBitPos, kTimestampWidth);
  }
  void set_timestamp( uint32_t w, BitVector<kDataBusWidth> *d ){
      d->set_word(w, kTimestampBitPos, kTimestampWidth);  }
  uint32_t get_rate_enable( BitVector<kDataBusWidth> *d ){
      return d->get_word(kRateEnableBitPos, kRateEnableWidth);
  }
  void set_rate_enable( uint32_t w, BitVector<kDataBusWidth> *d ){
      d->set_word(w, kRateEnableBitPos, kRateEnableWidth);  }
  uint32_t get_vold( BitVector<kDataBusWidth> *d ){
      return d->get_word(kVoldBitPos, kVoldWidth);
  }
  void set_vold( uint32_t w, BitVector<kDataBusWidth> *d ){
      d->set_word(w, kVoldBitPos, kVoldWidth);  }
  uint32_t get_lpf_action_scale( BitVector<kDataBusWidth> *d ){
      return d->get_word(kLpfActionScaleBitPos, kLpfActionScaleWidth);
  }
  void set_lpf_action_scale( uint32_t w, BitVector<kDataBusWidth> *d ){
      d->set_word(w, kLpfActionScaleBitPos, kLpfActionScaleWidth);  }
  uint32_t get_time_constant_mantissa( BitVector<kDataBusWidth> *d ){
      return d->get_word(kTimeConstantMantissaBitPos, kTimeConstantMantissaWidth);
  }
  void set_time_constant_mantissa( uint32_t w, BitVector<kDataBusWidth> *d ){
      d->set_word(w, kTimeConstantMantissaBitPos, kTimeConstantMantissaWidth);  }
  uint32_t get_rise_time_constant_exponent( BitVector<kDataBusWidth> *d ){
      return d->get_word(kRiseTimeConstantExponentBitPos, kRiseTimeConstantExponentWidth);
  }
  void set_rise_time_constant_exponent( uint32_t w, BitVector<kDataBusWidth> *d ){
      d->set_word(w, kRiseTimeConstantExponentBitPos, kRiseTimeConstantExponentWidth);  }
  uint32_t get_fall_time_constant_exponent( BitVector<kDataBusWidth> *d ){
      return d->get_word(kFallTimeConstantExponentBitPos, kFallTimeConstantExponentWidth);
  }
  void set_fall_time_constant_exponent( uint32_t w, BitVector<kDataBusWidth> *d ){
      d->set_word(w, kFallTimeConstantExponentBitPos, kFallTimeConstantExponentWidth);  }
  uint32_t get_red_probability_scale( BitVector<kDataBusWidth> *d ){
      return d->get_word(kRedProbabilityScaleBitPos, kRedProbabilityScaleWidth);
  }
  void set_red_probability_scale( uint32_t w, BitVector<kDataBusWidth> *d ){
      d->set_word(w, kRedProbabilityScaleBitPos, kRedProbabilityScaleWidth);  }
  uint32_t get_red_level_exponent( BitVector<kDataBusWidth> *d ){
      return d->get_word(kRedLevelExponentBitPos, kRedLevelExponentWidth);
  }
  void set_red_level_exponent( uint32_t w, BitVector<kDataBusWidth> *d ){
      d->set_word(w, kRedLevelExponentBitPos, kRedLevelExponentWidth);  }
  uint32_t get_red_level_max( BitVector<kDataBusWidth> *d ){
      return d->get_word(kRedLevelMaxBitPos, kRedLevelMaxWidth);
  }
  void set_red_level_max( uint32_t w, BitVector<kDataBusWidth> *d ){
      d->set_word(w, kRedLevelMaxBitPos, kRedLevelMaxWidth);  }
  uint32_t get_redd_level100( BitVector<kDataBusWidth> *d ){
      return d->get_word(kReddLevel100BitPos, kReddLevel100Width);
  }
  void set_redd_level100( uint32_t w, BitVector<kDataBusWidth> *d ){
      d->set_word(w, kReddLevel100BitPos, kReddLevel100Width);  }
  uint32_t get_red_level0( BitVector<kDataBusWidth> *d ){
      return d->get_word(kRedLevel0BitPos, kRedLevel0Width);
  }
  void set_red_level0( uint32_t w, BitVector<kDataBusWidth> *d ){
      d->set_word(w, kRedLevel0BitPos, kRedLevel0Width);  }
  void unpack( BitVector<kDataBusWidth> *d,
            uint32_t* timestamp,
            bool* rate_enable,
            uint32_t* vold,
            int32_t* lpf_action_scale,
            int32_t* time_constant_mantissa,
            int32_t* rise_time_constant_exponent,
            int32_t* fall_time_constant_exponent,
            int32_t* red_probability_scale,
            int32_t* red_level_exponent,
            int32_t* red_level_max,
            int32_t* redd_level100,
            int32_t* red_level0 ) {
      *timestamp = d->get_word(kTimestampBitPos, kTimestampWidth);
      *rate_enable = (0 != d->get_word(kRateEnableBitPos, kRateEnableWidth));
      *vold = d->get_word(kVoldBitPos, kVoldWidth);
      *lpf_action_scale = d->get_word(kLpfActionScaleBitPos, kLpfActionScaleWidth);
      *time_constant_mantissa = d->get_word(kTimeConstantMantissaBitPos, kTimeConstantMantissaWidth);
      *rise_time_constant_exponent = d->get_word(kRiseTimeConstantExponentBitPos, kRiseTimeConstantExponentWidth);
      *fall_time_constant_exponent = d->get_word(kFallTimeConstantExponentBitPos, kFallTimeConstantExponentWidth);
      *red_probability_scale = d->get_word(kRedProbabilityScaleBitPos, kRedProbabilityScaleWidth);
      *red_level_exponent = d->get_word(kRedLevelExponentBitPos, kRedLevelExponentWidth);
      *red_level_max = d->get_word(kRedLevelMaxBitPos, kRedLevelMaxWidth);
      *redd_level100 = d->get_word(kReddLevel100BitPos, kReddLevel100Width);
      *red_level0 = d->get_word(kRedLevel0BitPos, kRedLevel0Width);
  }
  std::string to_string(BitVector<kDataBusWidth> *d, std::string indent_string = "") const {
    std::string r("");
    r += indent_string + std::string("timestamp") + ": 0x" + boost::str( boost::format("%x") %  
        static_cast<uint32_t>(d->get_word(kTimestampBitPos, kTimestampWidth)) ) + "\n";
    r += indent_string + std::string("rate_enable") + ": 0x" + boost::str( boost::format("%x") %  
        static_cast<uint32_t>(d->get_word(kRateEnableBitPos, kRateEnableWidth)) ) + "\n";
    r += indent_string + std::string("vold") + ": 0x" + boost::str( boost::format("%x") %  
        static_cast<uint32_t>(d->get_word(kVoldBitPos, kVoldWidth)) ) + "\n";
    r += indent_string + std::string("lpf_action_scale") + ": 0x" + boost::str( boost::format("%x") %  
        static_cast<uint32_t>(d->get_word(kLpfActionScaleBitPos, kLpfActionScaleWidth)) ) + "\n";
    r += indent_string + std::string("time_constant_mantissa") + ": 0x" + boost::str( boost::format("%x") %  
        static_cast<uint32_t>(d->get_word(kTimeConstantMantissaBitPos, kTimeConstantMantissaWidth)) ) + "\n";
    r += indent_string + std::string("rise_time_constant_exponent") + ": 0x" + boost::str( boost::format("%x") %  
        static_cast<uint32_t>(d->get_word(kRiseTimeConstantExponentBitPos, kRiseTimeConstantExponentWidth)) ) + "\n";
    r += indent_string + std::string("fall_time_constant_exponent") + ": 0x" + boost::str( boost::format("%x") %  
        static_cast<uint32_t>(d->get_word(kFallTimeConstantExponentBitPos, kFallTimeConstantExponentWidth)) ) + "\n";
    r += indent_string + std::string("red_probability_scale") + ": 0x" + boost::str( boost::format("%x") %  
        static_cast<uint32_t>(d->get_word(kRedProbabilityScaleBitPos, kRedProbabilityScaleWidth)) ) + "\n";
    r += indent_string + std::string("red_level_exponent") + ": 0x" + boost::str( boost::format("%x") %  
        static_cast<uint32_t>(d->get_word(kRedLevelExponentBitPos, kRedLevelExponentWidth)) ) + "\n";
    r += indent_string + std::string("red_level_max") + ": 0x" + boost::str( boost::format("%x") %  
        static_cast<uint32_t>(d->get_word(kRedLevelMaxBitPos, kRedLevelMaxWidth)) ) + "\n";
    r += indent_string + std::string("redd_level100") + ": 0x" + boost::str( boost::format("%x") %  
        static_cast<uint32_t>(d->get_word(kReddLevel100BitPos, kReddLevel100Width)) ) + "\n";
    r += indent_string + std::string("red_level0") + ": 0x" + boost::str( boost::format("%x") %  
        static_cast<uint32_t>(d->get_word(kRedLevel0BitPos, kRedLevel0Width)) ) + "\n";
      return r;
  }
  // end generatated by script (below)

  static_assert( (kTimestampBitPos == MauDefs::kTimestampBitPos), "Timestamp bitpos must match one in MauDefs" );
  static_assert( (kTimestampWidth  == MauDefs::kTimestampWidth),  "Timestamp width must match one in MauDefs" );
  static_assert( kTimestampWidth < 31, "Timestamp width must be less than 31 bits" );
  static constexpr uint32_t kTimestampMask = (1u<<kTimestampWidth)-1u;

  // work out leading 1's position using g++ builtin cout leading zeros
  //  int is probably 32 bits, but this should work for any size
  int leading_1s_position( int x ) {
    RMT_ASSERT( x != 0); // undefined for x==0
    constexpr int kBitsInInt = __builtin_clz(1) + 1;
    return kBitsInInt - __builtin_clz(x) - 1;
  }

 private:

  // Single element cache of LPF V values (see comments in MauLpfMeter::calculate_output)
  uint64_t  T_;
  uint32_t  addr_;
  uint32_t  V_;
  bool      snapped_=false;
  uint32_t  snapped_timestamp_{0};
  uint64_t                 config_write_T_{ UINT64_C(0) };
  BitVector<kDataBusWidth> config_write_read_data_{ UINT64_C(0) };
  BitVector<kDataBusWidth> config_write_write_data_{ UINT64_C(0) };
  BitVector<kDataBusWidth> data_in_local_{ UINT64_C(0) };
  
  // https://graphics.stanford.edu/~seander/bithacks.html#ReverseByteWith64Bits
  inline uint8_t reverse_bits( uint8_t b ) {
    return ((b * UINT64_C(0x80200802)) & UINT64_C(0x0884422110)) * UINT64_C(0x0101010101) >> 32;
  }

  inline void cache_V(uint64_t T, uint32_t addr, uint32_t V) {
    T_ = T; addr_ = addr; V_ = V;
  }
  inline uint32_t maybe_use_cache_V(uint64_t T, uint32_t addr, uint32_t V) {
    return ((UINT64_C(1) + T_ == T) &&
            (Address::meter_addr_get_vaddr(addr_) ==
             Address::meter_addr_get_vaddr(addr)     )) ?  V_ :V;
  }
  
  void calculate_red_action_data(
        uint64_t Vnew,
        uint32_t red_nodrop_value,
        uint32_t red_drop_value,
        uint32_t red_probability_scale,
        uint32_t red_level_exponent,
        uint32_t red_level_max,
        uint32_t red_d_level100,
        uint32_t red_level0,
        uint64_t random_number,
        bool     red_disable,
        uint32_t *action_data_out );
  
};
}

#endif // _SHARED_MAU_METER_

/*
#!/usr/bin/perl -w
# generate constants and pack/unpack
my $a = [
    [ "Timestamp" ,                100, 28 , ""], # name, start pos, width
    [ "RateEnable",                 99,  1 , "bool"],  
    [ "Vold" ,                      64, 32 , ""],
    [ "LpfActionScale",             57,  5 , "int"],
    [ "TimeConstantMantissa",       43,  9 , "int"],
    [ "RiseTimeConstantExponent",   38,  5 , "int"],
    [ "FallTimeConstantExponent",   33,  5 , "int"],
    [ "RedProbabilityScale",        29,  3 , "int"],
    [ "RedLevelExponent",           24,  5 , "int"],
    [ "RedLevelMax",                16,  8 , "int"],
    [ "ReddLevel100",                8,  8 , "int"],
    [ "RedLevel0",                   0,  8 , "int"],
];

foreach my $v (@$a) {
    my ($n,$b,$w,$type) = @$v;
    print "  static constexpr int k${n}BitPos = $b;\n";
    print "  static constexpr int k${n}Width  = $w;\n";
    if ($type =~ "float_([0-9]+)_([0-9])+") {
        my $m_width = $1;
        my $e_width = $2;
        die unless ($m_width + $e_width) == $w;
        print "  static constexpr int k${n}ExponentWidth = $e_width;\n";
        print "  static constexpr int k${n}MantissaWidth = $m_width;\n";
    }
}    
print "\n";
foreach my $v (@$a) {
    my ($n,$b,$w,$type) = @$v;
    my $lc_n = $n;
    $lc_n =~ s/^([A-Z])/lc($1)/e;
    $lc_n =~ s/([A-Z])/"_".lc($1)/ge;
    push( @$v, $lc_n );
}
foreach my $v (@$a) {
    my ($n,$b,$w,$type,$lc_n) = @$v;
    print "  uint32_t get_${lc_n}( BitVector<kDataBusWidth> *d ){\n";
    print "      return d->get_word(k${n}BitPos, k${n}Width);\n  }\n";
    print "  void set_${lc_n}( uint32_t w, BitVector<kDataBusWidth> *d ){\n";
    print "      d->set_word(w, k${n}BitPos, k${n}Width);  }\n";
}    

my ($params,$body) = ("","");
foreach my $v (@$a) {
    my ($n,$b,$w,$type,$lc_n) = @$v;
    if ($type eq "") {
        $params .= ",\n            uint32_t* $lc_n";
        $body   .= "      *$lc_n = d->get_word(k${n}BitPos, k${n}Width);\n";
    }
    elsif ($type eq "bool") {
        $params .= ",\n            bool* $lc_n";
        $body   .= "      *$lc_n = (0 != d->get_word(k${n}BitPos, k${n}Width));\n";
    }
    elsif ($type eq "int") {
        $params .= ",\n            int32_t* $lc_n";
        $body .= "      *$lc_n = d->get_word(k${n}BitPos, k${n}Width);\n";
    }
    elsif ($type eq "signed") {
        $params .= ",\n            int32_t* $lc_n";
        $body .= "      *$lc_n = signextend<int32_t, k${n}Width> (\n".
                 "                    d->get_word(k${n}BitPos, k${n}Width));\n";
    }
    elsif ($type =~ "float_([0-9]+)_([0-9])+") {
        $params .= ",\n            uint32_t* ${lc_n}_mantissa";
        $params .= ",\n            uint32_t* ${lc_n}_exponent";
        $body   .= "       *${lc_n}_mantissa = d->get_word(k${n}BitPos + k${n}ExponentWidth, k${n}MantissaWidth );\n";
        $body   .= "       *${lc_n}_exponent = d->get_word(k${n}BitPos, k${n}ExponentWidth );\n";
    }
    else {
        die $type;
    }
}
print "  void unpack( BitVector<kDataBusWidth> *d";
print $params;
print " ) {\n";
print $body;
print "  }\n";

# print "  void pack( BitVector<kDataBusWidth> *d";
# foreach my $v (@$a) {
#     my ($n,$b,$w,$type,$lc_n) = @$v;
#     print ",\n            uint32_t $lc_n";
# }
# print " ) {\n";
# foreach my $v (@$a) {
#     my ($n,$b,$w,$type,$lc_n) = @$v;
#     print "      d->set_word($lc_n, k${n}BitPos, k${n}Width);\n";
# }
# print "  }\n";

print "  std::string to_string(BitVector<kDataBusWidth> *d, std::string indent_string = \"\") const {\n";
print "    std::string r(\"\");\n";
foreach my $v (@$a) {
    my ($n,$b,$w,$type,$lc_n) = @$v;
    print "    r += indent_string + std::string(\"$lc_n\") + ".
        "\": 0x\" + boost::str( boost::format(\"%x\") %  \n". 
        "        static_cast<uint32_t>(d->get_word(k${n}BitPos, k${n}Width)) ) + \"\\n\";\n";
}
print "      return r;\n";
print "  }\n";

*/
