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

#ifndef _SHARED_MAU_METER_
#define _SHARED_MAU_METER_

#include <string>
#include <cstdint>
#include <mau-defs.h>
#include <mau-object.h>
#include <bitvector.h>
#include <address.h>

namespace MODEL_CHIP_NAMESPACE {

// http://graphics.stanford.edu/~seander/bithacks.html#FixedSignExtend
template <typename T, unsigned B>
inline T signextend(const T x)
{
  struct {T x:B;} s;
  return s.x = x;
}
//int r = signextend<signed int,5>(x);  // sign extend 5 bit number x to r

class MauMeter : public MauObject {

 public:
  MauMeter(RmtObjectManager *om, int pipeIndex, int mauIndex, int logicalRowIndex,
           Mau *mau);
  ~MauMeter();

  static constexpr int  kType = RmtTypes::kRmtTypeMauMeter;
  static constexpr uint64_t kProbabalistMask = INT64_C(0x1FFFF); // 17 bits
  static constexpr uint64_t kFractionalBitsMask = INT64_C(0x1FFFF); // 17 bits

  static constexpr int  kDataBusWidth = MauDefs::kDataBusWidth;
  static constexpr uint8_t kMeterColorInhibit = 4; // used as a mask
  static constexpr uint8_t kMeterColorGreen   = 0;
  static constexpr uint8_t kMeterColorYellow  = 1;
  static constexpr uint8_t kMeterColorYellow2 = 2;
  static constexpr uint8_t kMeterColorRed     = 3;
  static constexpr uint8_t kMeterColorMask    = 3;

  static bool kRelaxExponentCheck; // Defined in rmt-config.cpp

  static uint8_t color_extract(uint8_t color) { return color & kMeterColorMask; }
  static bool color_inhibit(uint8_t color) { return color & kMeterColorInhibit; }

  // Next func gets final input post any wacky forwarding behaviour
  void get_input_data(BitVector<kDataBusWidth> *data);

  void calculate_output(uint64_t present_time,
                        uint64_t relative_time, // just used to work out if forwarding
                        uint32_t addr,
                        bool is_byte_counter,
                        bool rng_enable,
                        uint64_t random_number,
                        int meter_time_scale,
                        bool meter_sweep,
                        BitVector<kDataBusWidth>* data_in,
                        uint8_t color_in,
                        int decrement,
                        bool meter_lpf_sat_ctl,
                        BitVector<kDataBusWidth>* data_out,
                        uint8_t* color_out);

  void get_curr_color(BitVector<kDataBusWidth>* data_in, uint8_t* color_out);



  void update_cache(int old_addr, int new_addr) {
    if ((old_addr < 0) || (new_addr < 0)) return;
    if (Address::meter_addr_get_vaddr(old_addr) ==
        Address::meter_addr_get_vaddr(addr_)        ) {
      addr_ = new_addr;
    }
  }
  void update_cache_config_write(uint64_t T, uint32_t addr, BitVector<kDataBusWidth> *read_data, BitVector<kDataBusWidth> *write_data) {
    // update the cache
    T_ = T;
    addr_ = addr;
    // save a copy of the value read from the ram during the config write (ie the ram contents before the write happens), as
    //  this is the data the RTL sees if there is a phv access in the next cycle
    config_write_read_data_.copy_from(*read_data);
    // also need to save a copy of the write data, as this is what is replayed in the next cycle to prevent the write being
    //  dropped
    config_write_write_data_.copy_from(*write_data);
    config_write_T_ = T;
  }

 private:
  uint64_t T_{0};
  uint32_t addr_{0};
  bool     snapped_=false;
  uint32_t snapped_timestamp_{0};
  uint64_t                 config_write_T_{ UINT64_C(0) };
  BitVector<kDataBusWidth> config_write_read_data_{ UINT64_C(0) };
  BitVector<kDataBusWidth> config_write_write_data_{ UINT64_C(0) };
  BitVector<kDataBusWidth> data_in_local_{ UINT64_C(0) };

  // generatated by script (below)
  static constexpr int kTimestampBitPos = 100;
  static constexpr int kTimestampWidth  = 28;
  static constexpr int kPeakLevelBitPos = 77;
  static constexpr int kPeakLevelWidth  = 23;
  static constexpr int kCommittedLevelBitPos = 54;
  static constexpr int kCommittedLevelWidth  = 23;
  static constexpr int kPeakBurstsizeBitPos = 41;
  static constexpr int kPeakBurstsizeWidth  = 13;
  static constexpr int kPeakBurstsizeExponentWidth = 5;
  static constexpr int kPeakBurstsizeMantissaWidth = 8;
  static constexpr int kCommittedBurstsizeBitPos = 28;
  static constexpr int kCommittedBurstsizeWidth  = 13;
  static constexpr int kCommittedBurstsizeExponentWidth = 5;
  static constexpr int kCommittedBurstsizeMantissaWidth = 8;
  static constexpr int kPeakRateBitPos = 14;
  static constexpr int kPeakRateWidth  = 14;
  static constexpr int kPeakRateRelativeExponentWidth = 5;
  static constexpr int kPeakRateMantissaWidth = 9;
  static constexpr int kCommittedRateBitPos = 0;
  static constexpr int kCommittedRateWidth  = 14;
  static constexpr int kCommittedRateRelativeExponentWidth = 5;
  static constexpr int kCommittedRateMantissaWidth = 9;

  uint32_t get_timestamp( BitVector<kDataBusWidth> *d ){
      return d->get_word(kTimestampBitPos, kTimestampWidth);
  }
  void set_timestamp( uint32_t w, BitVector<kDataBusWidth> *d ){
      d->set_word(w, kTimestampBitPos, kTimestampWidth);  }
  uint32_t get_peak_level( BitVector<kDataBusWidth> *d ){
      return d->get_word(kPeakLevelBitPos, kPeakLevelWidth);
  }
  void set_peak_level( uint32_t w, BitVector<kDataBusWidth> *d ){
      d->set_word(w, kPeakLevelBitPos, kPeakLevelWidth);  }
  uint32_t get_committed_level( BitVector<kDataBusWidth> *d ){
      return d->get_word(kCommittedLevelBitPos, kCommittedLevelWidth);
  }
  void set_committed_level( uint32_t w, BitVector<kDataBusWidth> *d ){
      d->set_word(w, kCommittedLevelBitPos, kCommittedLevelWidth);  }
  uint32_t get_peak_burstsize( BitVector<kDataBusWidth> *d ){
      return d->get_word(kPeakBurstsizeBitPos, kPeakBurstsizeWidth);
  }
  void set_peak_burstsize( uint32_t w, BitVector<kDataBusWidth> *d ){
      d->set_word(w, kPeakBurstsizeBitPos, kPeakBurstsizeWidth);  }
  uint32_t get_committed_burstsize( BitVector<kDataBusWidth> *d ){
      return d->get_word(kCommittedBurstsizeBitPos, kCommittedBurstsizeWidth);
  }
  void set_committed_burstsize( uint32_t w, BitVector<kDataBusWidth> *d ){
      d->set_word(w, kCommittedBurstsizeBitPos, kCommittedBurstsizeWidth);  }
  uint32_t get_peak_rate( BitVector<kDataBusWidth> *d ){
      return d->get_word(kPeakRateBitPos, kPeakRateWidth);
  }
  void set_peak_rate( uint32_t w, BitVector<kDataBusWidth> *d ){
      d->set_word(w, kPeakRateBitPos, kPeakRateWidth);  }
  uint32_t get_committed_rate( BitVector<kDataBusWidth> *d ){
      return d->get_word(kCommittedRateBitPos, kCommittedRateWidth);
  }
  void set_committed_rate( uint32_t w, BitVector<kDataBusWidth> *d ){
      d->set_word(w, kCommittedRateBitPos, kCommittedRateWidth);  }
  void unpack( BitVector<kDataBusWidth> *d,
            uint32_t* timestamp,
            int32_t* peak_level,
            int32_t* committed_level,
            uint32_t* peak_burstsize_mantissa,
            uint32_t* peak_burstsize_exponent,
            uint32_t* committed_burstsize_mantissa,
            uint32_t* committed_burstsize_exponent,
            uint32_t* peak_rate_mantissa,
            uint32_t* peak_rate_relative_exponent,
            uint32_t* committed_rate_mantissa,
            uint32_t* committed_rate_relative_exponent ) {
      *timestamp = d->get_word(kTimestampBitPos, kTimestampWidth);
      *peak_level = signextend<int32_t, kPeakLevelWidth> (
                    d->get_word(kPeakLevelBitPos, kPeakLevelWidth));
      *committed_level = signextend<int32_t, kCommittedLevelWidth> (
                    d->get_word(kCommittedLevelBitPos, kCommittedLevelWidth));
       *peak_burstsize_mantissa = d->get_word(kPeakBurstsizeBitPos, kPeakBurstsizeMantissaWidth );
       *peak_burstsize_exponent = d->get_word(kPeakBurstsizeBitPos + kPeakBurstsizeMantissaWidth, kPeakBurstsizeExponentWidth );
       *committed_burstsize_mantissa = d->get_word(kCommittedBurstsizeBitPos, kCommittedBurstsizeMantissaWidth );
       *committed_burstsize_exponent = d->get_word(kCommittedBurstsizeBitPos + kCommittedBurstsizeMantissaWidth, kCommittedBurstsizeExponentWidth );
       *peak_rate_mantissa = d->get_word(kPeakRateBitPos, kPeakRateMantissaWidth );
       *peak_rate_relative_exponent = d->get_word(kPeakRateBitPos + kPeakRateMantissaWidth, kPeakRateRelativeExponentWidth );
       *committed_rate_mantissa = d->get_word(kCommittedRateBitPos, kCommittedRateMantissaWidth );
       *committed_rate_relative_exponent = d->get_word(kCommittedRateBitPos + kCommittedRateMantissaWidth, kCommittedRateRelativeExponentWidth );
  }
  std::string to_string(BitVector<kDataBusWidth> *d, std::string indent_string = "") const {
    std::string r("");
    r += indent_string + std::string("timestamp") + ": 0x" + boost::str( boost::format("%x") %
        static_cast<uint32_t>(d->get_word(kTimestampBitPos, kTimestampWidth)) ) + "\n";
    r += indent_string + std::string("peak_level") + ": 0x" + boost::str( boost::format("%x") %
        static_cast<uint32_t>(d->get_word(kPeakLevelBitPos, kPeakLevelWidth)) ) + "\n";
    r += indent_string + std::string("committed_level") + ": 0x" + boost::str( boost::format("%x") %
        static_cast<uint32_t>(d->get_word(kCommittedLevelBitPos, kCommittedLevelWidth)) ) + "\n";
    r += indent_string + std::string("peak_burstsize") + ": 0x" + boost::str( boost::format("%x") %
        static_cast<uint32_t>(d->get_word(kPeakBurstsizeBitPos, kPeakBurstsizeWidth)) ) + "\n";
    r += indent_string + std::string("committed_burstsize") + ": 0x" + boost::str( boost::format("%x") %
        static_cast<uint32_t>(d->get_word(kCommittedBurstsizeBitPos, kCommittedBurstsizeWidth)) ) + "\n";
    r += indent_string + std::string("peak_rate") + ": 0x" + boost::str( boost::format("%x") %
        static_cast<uint32_t>(d->get_word(kPeakRateBitPos, kPeakRateWidth)) ) + "\n";
    r += indent_string + std::string("committed_rate") + ": 0x" + boost::str( boost::format("%x") %
        static_cast<uint32_t>(d->get_word(kCommittedRateBitPos, kCommittedRateWidth)) ) + "\n";
      return r;
  }
  // end generatated by script (below)


  static_assert( (kTimestampBitPos == MauDefs::kTimestampBitPos), "Timestamp bitpos must match one in MauDefs" );
  static_assert( (kTimestampWidth  == MauDefs::kTimestampWidth),  "Timestamp width must match one in MauDefs" );
  static_assert( kTimestampWidth < 31, "Timestamp width must be less than 31 bits" );
  static constexpr uint32_t kTimestampMask = (1u<<kTimestampWidth)-1u;

  static_assert( kPeakLevelWidth == kCommittedLevelWidth, "Assumes levels are same width" );
  static_assert( kCommittedLevelWidth < 31 , "Level width must be less than 31 bits" );
  static constexpr uint32_t kLevelMask = (1u<<kCommittedLevelWidth)-1u;

 private:
  int32_t calculate_new_bucket_level(
      bool     rng_enable,
      uint64_t random_number,
      uint64_t timestamp_diff,
      int32_t  level,
      uint32_t burstsize_mantissa,
      uint32_t burstsize_exponent,
      uint32_t rate_mantissa,
      uint32_t rate_exponent,
      uint32_t bits_to_clear,
      uint32_t decrement,
      bool    *saturated);

  int calculate_burstsize_exponent_adj(int burstsize_exponent);
  int calculate_rate_exponent(int rate_relative_exponent, int burstsize_exponent);


};

}
#endif // _SHARED_MAU_METER_

/*
#!/usr/bin/perl -w
# generate constants and pack/unpack
my $a = [
#    [ "ColorAwareEnable", 126,1 , "bool"],
    [ "Timestamp" ,         100, 28 , ""], # name, start pos, width
    [ "PeakLevel" ,          77, 23 , "signed"],
    [ "CommittedLevel",      54, 23 , "signed"],
    [ "PeakBurstsize",       41, 13 , "float_8_5"],  # _mantissa_exponent
    [ "CommittedBurstsize",  28, 13 , "float_8_5"],
    [ "PeakRate",            14, 14 , "float_9_5_relative"],
    [ "CommittedRate",        0, 14 , "float_9_5_relative"],
];

foreach my $v (@$a) {
    my ($n,$b,$w,$type) = @$v;
    print "  static constexpr int k${n}BitPos = $b;\n";
    print "  static constexpr int k${n}Width  = $w;\n";
    if ($type =~ "float_([0-9]+)_([0-9])+(_relative)?") {
        my $m_width = $1;
        my $e_width = $2;
        my $relative = defined($3);
        die unless ($m_width + $e_width) == $w;
        if ( $relative ) {
            print "  static constexpr int k${n}RelativeExponentWidth = $e_width;\n";
        }
        else {
            print "  static constexpr int k${n}ExponentWidth = $e_width;\n";
        }
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
    elsif ($type eq "signed") {
        $params .= ",\n            int32_t* $lc_n";
        $body .= "      *$lc_n = signextend<int32_t, k${n}Width> (\n".
                 "                    d->get_word(k${n}BitPos, k${n}Width));\n";
    }
    elsif ($type =~ "float_([0-9]+)_([0-9])+(_relative)?") {
        my $relative = defined($3);
        $params .= ",\n            uint32_t* ${lc_n}_mantissa";
        if ( $relative ) {
            $params .= ",\n            uint32_t* ${lc_n}_relative_exponent";
            $body   .= "       *${lc_n}_mantissa = d->get_word(k${n}BitPos, k${n}MantissaWidth );\n";
            $body   .= "       *${lc_n}_relative_exponent = d->get_word(k${n}BitPos + k${n}MantissaWidth, k${n}RelativeExponentWidth );\n";
        }
        else {
            $params .= ",\n            uint32_t* ${lc_n}_exponent";
            $body   .= "       *${lc_n}_mantissa = d->get_word(k${n}BitPos, k${n}MantissaWidth );\n";
            $body   .= "       *${lc_n}_exponent = d->get_word(k${n}BitPos + k${n}MantissaWidth, k${n}ExponentWidth );\n";
        }
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
