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

#include <common/rmt-assert.h>
#include <rmt-object-manager.h>
#include <comira-umac4.h>


namespace MODEL_CHIP_NAMESPACE {

// Mapping table to map Comira4 specific MAC counter indices to generic counter indices
//
// For each vendor_index this table contains:
// 1. Whether the counter is a RX counter (true => RX, false => TX)
// 2. Upto 2 32b masks of counters to sum to form the result for the vendor_index
// 3. A flag to indicate whether the sum corresponding to cntr_mask1 should be subtracted or added to the sum from cntr_mask0
//
const MacCounterMapperConfig VmacC4::kMacCounterMapperTab[] = {
  // vendor_index                          is_rx  cntr_flavor0           cntr_mask0                        cntr_flavor1        cntr_mask1            do_subtract
  {  VmacC4Cntr::FramesXmitOK,             false, CntrFlavor::Frames,    1u<<Cntr::OK,                     CntrFlavor::None,   0u,                   false    },  //  0
  {  VmacC4Cntr::FramesXmitAll,            false, CntrFlavor::Frames,    1u<<Cntr::All,                    CntrFlavor::None,   0u,                   false    },  //  1
  {  VmacC4Cntr::FramesXmitError,          false, CntrFlavor::Frames,    1u<<Cntr::Error,                  CntrFlavor::None,   0u,                   false    },  //  2
  {  VmacC4Cntr::OctetsXmitOK,             false, CntrFlavor::Octets,    1u<<Cntr::OK,                     CntrFlavor::None,   0u,                   false    },  //  3
  {  VmacC4Cntr::OctetsXmitAll,            false, CntrFlavor::Octets,    1u<<Cntr::All,                    CntrFlavor::None,   0u,                   false    },  //  4
  {  VmacC4Cntr::FramesXmitUnicast,        false, CntrFlavor::Frames,    1u<<Cntr::Unicast,                CntrFlavor::None,   0u,                   false    },  //  5
  {  VmacC4Cntr::FramesXmitMulticast,      false, CntrFlavor::Frames,    1u<<Cntr::Multicast,              CntrFlavor::None,   0u,                   false    },  //  6
  {  VmacC4Cntr::FramesXmitBroadcast,      false, CntrFlavor::Frames,    1u<<Cntr::Broadcast,              CntrFlavor::None,   0u,                   false    },  //  7
  {  VmacC4Cntr::FramesXmitPause,          false, CntrFlavor::Frames,    1u<<Cntr::Pause,                  CntrFlavor::None,   0u,                   false    },  //  8
  {  VmacC4Cntr::FramesXmitPriPause,       false, CntrFlavor::Frames,    1u<<Cntr::PriPause,               CntrFlavor::None,   0u,                   false    },  //  9
  {  VmacC4Cntr::FramesXmitVLAN,           false, CntrFlavor::Frames,    1u<<Cntr::Vlan,                   CntrFlavor::None,   0u,                   false    },  // 10
  {  VmacC4Cntr::FramesXmitSizeLT64,       false, CntrFlavor::Bucket64,  1u<<0,                            CntrFlavor::None,   0u,                   false    },  // 11
  {  VmacC4Cntr::FramesXmitSizeEQ64,       false, CntrFlavor::Frames,    1u<<Cntr::SizeEq64,               CntrFlavor::None,   0u,                   false    },  // 12
  {  VmacC4Cntr::FramesXmitSize65to127,    false, CntrFlavor::Bucket64,  1u<<1,                            CntrFlavor::Frames, 1u<<Cntr::SizeEq64_B, true     },  // 13
  {  VmacC4Cntr::FramesXmitSize128to255,   false, CntrFlavor::Bucket64,  MacCounters::make_range(2,3),     CntrFlavor::None,   0u,                   false    },  // 14
  {  VmacC4Cntr::FramesXmitSize256to511,   false, CntrFlavor::Bucket64,  MacCounters::make_range(4,7),     CntrFlavor::None,   0u,                   false    },  // 15
  {  VmacC4Cntr::FramesXmitSize512to1023,  false, CntrFlavor::Bucket512, 1u<<1,                            CntrFlavor::None,   0u,                   false    },  // 16
  {  VmacC4Cntr::FramesXmitSize1024to1518, false, CntrFlavor::Bucket512, 1u<<2,                            CntrFlavor::Frames, 1u<<Cntr::Size15xx_A, true     },  // 17
  {  VmacC4Cntr::FramesXmitSize1519to2047, false, CntrFlavor::Bucket512, 1u<<3,                            CntrFlavor::Frames, 1u<<Cntr::Size15xx_B, false    },  // 18
  {  VmacC4Cntr::FramesXmitSize2048to4095, false, CntrFlavor::Bucket512, MacCounters::make_range(4,7),     CntrFlavor::None,   0u,                   false    },  // 19
  {  VmacC4Cntr::FramesXmitSize4096to8191, false, CntrFlavor::Bucket512, MacCounters::make_range(8,15),    CntrFlavor::None,   0u,                   false    },  // 20
  {  VmacC4Cntr::FramesXmitSize8192to9215, false, CntrFlavor::Bucket512, MacCounters::make_range(16,17),   CntrFlavor::None,   0u,                   false    },  // 21
  {  VmacC4Cntr::FramesXmitSizeGT9216,     false, CntrFlavor::Bucket512, MacCounters::make_range(18,31),   CntrFlavor::None,   0u,                   false    },  // 22
  {  VmacC4Cntr::FramesXmitPri0,           false, CntrFlavor::Pri,       1u<<0,                            CntrFlavor::None,   0u,                   false    },  // 23
  {  VmacC4Cntr::FramesXmitPri1,           false, CntrFlavor::Pri,       1u<<1,                            CntrFlavor::None,   0u,                   false    },  // 24
  {  VmacC4Cntr::FramesXmitPri2,           false, CntrFlavor::Pri,       1u<<2,                            CntrFlavor::None,   0u,                   false    },  // 25
  {  VmacC4Cntr::FramesXmitPri3,           false, CntrFlavor::Pri,       1u<<3,                            CntrFlavor::None,   0u,                   false    },  // 26
  {  VmacC4Cntr::FramesXmitPri4,           false, CntrFlavor::Pri,       1u<<4,                            CntrFlavor::None,   0u,                   false    },  // 27
  {  VmacC4Cntr::FramesXmitPri5,           false, CntrFlavor::Pri,       1u<<5,                            CntrFlavor::None,   0u,                   false    },  // 28
  {  VmacC4Cntr::FramesXmitPri6,           false, CntrFlavor::Pri,       1u<<6,                            CntrFlavor::None,   0u,                   false    },  // 29
  {  VmacC4Cntr::FramesXmitPri7,           false, CntrFlavor::Pri,       1u<<7,                            CntrFlavor::None,   0u,                   false    },  // 30
  {  VmacC4Cntr::XmitPri0Pause1US,         false, CntrFlavor::PriPause,  1u<<0,                            CntrFlavor::None,   0u,                   false    },  // 31
  {  VmacC4Cntr::XmitPri1Pause1US,         false, CntrFlavor::PriPause,  1u<<1,                            CntrFlavor::None,   0u,                   false    },  // 32
  {  VmacC4Cntr::XmitPri2Pause1US,         false, CntrFlavor::PriPause,  1u<<2,                            CntrFlavor::None,   0u,                   false    },  // 33
  {  VmacC4Cntr::XmitPri3Pause1US,         false, CntrFlavor::PriPause,  1u<<3,                            CntrFlavor::None,   0u,                   false    },  // 34
  {  VmacC4Cntr::XmitPri4Pause1US,         false, CntrFlavor::PriPause,  1u<<4,                            CntrFlavor::None,   0u,                   false    },  // 35
  {  VmacC4Cntr::XmitPri5Pause1US,         false, CntrFlavor::PriPause,  1u<<5,                            CntrFlavor::None,   0u,                   false    },  // 36
  {  VmacC4Cntr::XmitPri6Pause1US,         false, CntrFlavor::PriPause,  1u<<6,                            CntrFlavor::None,   0u,                   false    },  // 37
  {  VmacC4Cntr::XmitPri7Pause1US,         false, CntrFlavor::PriPause,  1u<<7,                            CntrFlavor::None,   0u,                   false    },  // 38
  {  VmacC4Cntr::FramesXmitDrained,        false, CntrFlavor::Frames,    1u<<Cntr::Drained,                CntrFlavor::None,   0u,                   false    },  // 39
  {  VmacC4Cntr::FramesXmitJabbered,       false, CntrFlavor::Frames,    1u<<Cntr::Jabber,                 CntrFlavor::None,   0u,                   false    },  // 40
  {  VmacC4Cntr::FramesXmitPadded,         false, CntrFlavor::Frames,    1u<<Cntr::Padded,                 CntrFlavor::None,   0u,                   false    },  // 41
  {  VmacC4Cntr::FramesXmitTruncated,      false, CntrFlavor::Frames,    1u<<Cntr::Truncated,              CntrFlavor::None,   0u,                   false    },  // 42
  {  VmacC4Cntr::FramesRcvdOK,             true,  CntrFlavor::Frames,    1u<<Cntr::OK,                     CntrFlavor::None,   0u,                   false    },  // 43
  {  VmacC4Cntr::OctetsRcvdOK,             true,  CntrFlavor::Octets,    1u<<Cntr::OK,                     CntrFlavor::None,   0u,                   false    },  // 44
  {  VmacC4Cntr::FramesRcvdAll,            true,  CntrFlavor::Frames,    1u<<Cntr::All,                    CntrFlavor::None,   0u,                   false    },  // 45
  {  VmacC4Cntr::OctetsRcvdAll,            true,  CntrFlavor::Octets,    1u<<Cntr::All,                    CntrFlavor::None,   0u,                   false    },  // 46
  {  VmacC4Cntr::FramesRcvdCRCError,       true,  CntrFlavor::Frames,    1u<<Cntr::CrcError,               CntrFlavor::None,   0u,                   false    },  // 47
  {  VmacC4Cntr::FramesRcvdError,          true,  CntrFlavor::Frames,    1u<<Cntr::Error,                  CntrFlavor::None,   0u,                   false    },  // 48
  {  VmacC4Cntr::FramesRcvdUnicast,        true,  CntrFlavor::Frames,    1u<<Cntr::Unicast,                CntrFlavor::None,   0u,                   false    },  // 49
  {  VmacC4Cntr::FramesRcvdMulticast,      true,  CntrFlavor::Frames,    1u<<Cntr::Multicast,              CntrFlavor::None,   0u,                   false    },  // 50
  {  VmacC4Cntr::FramesRcvdBroadcast,      true,  CntrFlavor::Frames,    1u<<Cntr::Broadcast,              CntrFlavor::None,   0u,                   false    },  // 51
  {  VmacC4Cntr::FramesRcvdPause,          true,  CntrFlavor::Frames,    1u<<Cntr::Pause,                  CntrFlavor::None,   0u,                   false    },  // 52
  {  VmacC4Cntr::FramesRcvdLenError,       true,  CntrFlavor::Frames,    1u<<Cntr::LenErr,                 CntrFlavor::None,   0u,                   false    },  // 53
  {  VmacC4Cntr::Reserved,                 true,  CntrFlavor::None,      0u,                               CntrFlavor::None,   0u,                   false    },  // 54
  {  VmacC4Cntr::FramesRcvdOversized,      true,  CntrFlavor::Frames,    1u<<Cntr::Oversized,              CntrFlavor::None,   0u,                   false    },  // 55
  {  VmacC4Cntr::FramesRcvdFragments,      true,  CntrFlavor::Frames,    1u<<Cntr::Fragment,               CntrFlavor::None,   0u,                   false    },  // 56
  {  VmacC4Cntr::FramesRcvdJabber,         true,  CntrFlavor::Frames,    1u<<Cntr::Jabber,                 CntrFlavor::None,   0u,                   false    },  // 57
  {  VmacC4Cntr::FramesRcvdPriPause,       true,  CntrFlavor::Frames,    1u<<Cntr::PriPause,               CntrFlavor::None,   0u,                   false    },  // 58
  {  VmacC4Cntr::FramesRcvdCrcErrStomp,    true,  CntrFlavor::Frames,    1u<<Cntr::CrcErrStomp,            CntrFlavor::None,   0u,                   false    },  // 59
  {  VmacC4Cntr::FramesRcvdMaxFrmSizeVio,  true,  CntrFlavor::Frames,    1u<<Cntr::MaxFrmSizeViolErr,      CntrFlavor::None,   0u,                   false    },  // 60
  {  VmacC4Cntr::FramesRcvdVLAN,           true,  CntrFlavor::Frames,    1u<<Cntr::Vlan,                   CntrFlavor::None,   0u,                   false    },  // 61
  {  VmacC4Cntr::FramesDropped,            true,  CntrFlavor::Frames,    1u<<Cntr::Dropped,                CntrFlavor::None,   0u,                   false    },  // 62
  {  VmacC4Cntr::FramesRcvdSizeLT64,       true,  CntrFlavor::Bucket64,  1u<<0,                            CntrFlavor::None,   0u,                   false    },  // 63
  {  VmacC4Cntr::FramesRcvdSizeEQ64,       true,  CntrFlavor::Frames,    1u<<Cntr::SizeEq64,               CntrFlavor::None,   0u,                   false    },  // 64
  {  VmacC4Cntr::FramesRcvdSize65to127,    true,  CntrFlavor::Bucket64,  1u<<1,                            CntrFlavor::Frames, 1u<<Cntr::SizeEq64_B, true     },  // 65
  {  VmacC4Cntr::FramesRcvdSize128to255,   true,  CntrFlavor::Bucket64,  MacCounters::make_range(2,3),     CntrFlavor::None,   0u,                   false    },  // 66
  {  VmacC4Cntr::FramesRcvdSize256to511,   true,  CntrFlavor::Bucket64,  MacCounters::make_range(4,7),     CntrFlavor::None,   0u,                   false    },  // 67
  {  VmacC4Cntr::FramesRcvdSize512to1023,  true,  CntrFlavor::Bucket512, 1u<<1,                            CntrFlavor::None,   0u,                   false    },  // 68
  {  VmacC4Cntr::FramesRcvdSize1024to1518, true,  CntrFlavor::Bucket512, 1u<<2,                            CntrFlavor::Frames, 1u<<Cntr::Size15xx_A, true     },  // 69
  {  VmacC4Cntr::FramesRcvdSize1519to2047, true,  CntrFlavor::Bucket512, 1u<<3,                            CntrFlavor::Frames, 1u<<Cntr::Size15xx_B, false    },  // 70
  {  VmacC4Cntr::FramesRcvdSize2048to4095, true,  CntrFlavor::Bucket512, MacCounters::make_range(4,7),     CntrFlavor::None,   0u,                   false    },  // 71
  {  VmacC4Cntr::FramesRcvdSize4096to8191, true,  CntrFlavor::Bucket512, MacCounters::make_range(8,15),    CntrFlavor::None,   0u,                   false    },  // 72
  {  VmacC4Cntr::FramesRcvdSize8192to9215, true,  CntrFlavor::Bucket512, MacCounters::make_range(16,17),   CntrFlavor::None,   0u,                   false    },  // 73
  {  VmacC4Cntr::FramesRcvdSizeGT9216,     true,  CntrFlavor::Bucket512, MacCounters::make_range(18,31),   CntrFlavor::None,   0u,                   false    },  // 74
  {  VmacC4Cntr::FramesRcvdPri0,           true,  CntrFlavor::Pri,       1u<<0,                            CntrFlavor::None,   0u,                   false    },  // 75
  {  VmacC4Cntr::FramesRcvdPri1,           true,  CntrFlavor::Pri,       1u<<1,                            CntrFlavor::None,   0u,                   false    },  // 76
  {  VmacC4Cntr::FramesRcvdPri2,           true,  CntrFlavor::Pri,       1u<<2,                            CntrFlavor::None,   0u,                   false    },  // 77
  {  VmacC4Cntr::FramesRcvdPri3,           true,  CntrFlavor::Pri,       1u<<3,                            CntrFlavor::None,   0u,                   false    },  // 78
  {  VmacC4Cntr::FramesRcvdPri4,           true,  CntrFlavor::Pri,       1u<<4,                            CntrFlavor::None,   0u,                   false    },  // 79
  {  VmacC4Cntr::FramesRcvdPri5,           true,  CntrFlavor::Pri,       1u<<5,                            CntrFlavor::None,   0u,                   false    },  // 80
  {  VmacC4Cntr::FramesRcvdPri6,           true,  CntrFlavor::Pri,       1u<<6,                            CntrFlavor::None,   0u,                   false    },  // 81
  {  VmacC4Cntr::FramesRcvdPri7,           true,  CntrFlavor::Pri,       1u<<7,                            CntrFlavor::None,   0u,                   false    },  // 82
  {  VmacC4Cntr::RcvdPri0Pause1US,         true,  CntrFlavor::PriPause,  1u<<0,                            CntrFlavor::None,   0u,                   false    },  // 83
  {  VmacC4Cntr::RcvdPri1Pause1US,         true,  CntrFlavor::PriPause,  1u<<1,                            CntrFlavor::None,   0u,                   false    },  // 84
  {  VmacC4Cntr::RcvdPri2Pause1US,         true,  CntrFlavor::PriPause,  1u<<2,                            CntrFlavor::None,   0u,                   false    },  // 85
  {  VmacC4Cntr::RcvdPri3Pause1US,         true,  CntrFlavor::PriPause,  1u<<3,                            CntrFlavor::None,   0u,                   false    },  // 86
  {  VmacC4Cntr::RcvdPri4Pause1US,         true,  CntrFlavor::PriPause,  1u<<4,                            CntrFlavor::None,   0u,                   false    },  // 87
  {  VmacC4Cntr::RcvdPri5Pause1US,         true,  CntrFlavor::PriPause,  1u<<5,                            CntrFlavor::None,   0u,                   false    },  // 88
  {  VmacC4Cntr::RcvdPri6Pause1US,         true,  CntrFlavor::PriPause,  1u<<6,                            CntrFlavor::None,   0u,                   false    },  // 89
  {  VmacC4Cntr::RcvdPri7Pause1US,         true,  CntrFlavor::PriPause,  1u<<7,                            CntrFlavor::None,   0u,                   false    },  // 90
  {  VmacC4Cntr::RcvdStdPause1US,          true,  CntrFlavor::PriPause,  1u<<8,                            CntrFlavor::None,   0u,                   false    },  // 91
  {  VmacC4Cntr::FramesRcvdTrunc,          true,  CntrFlavor::Frames,    1u<<Cntr::Truncated,              CntrFlavor::None,   0u,                   false    },  // 92
  {  VmacC4Cntr::Reserved2,                true,  CntrFlavor::None,      0u,                               CntrFlavor::None,   0u,                   false    },  // 93
  {  VmacC4Cntr::InvalidPreamble,          true,  CntrFlavor::Frames,    1u<<Cntr::InvalidPreambleErr,     CntrFlavor::None,   0u,                   false    },  // 94
  {  VmacC4Cntr::NormalLenInvalidCRC,      true,  CntrFlavor::Frames,    1u<<Cntr::NormalLenInvalidCrcErr, CntrFlavor::None,   0u,                   false    }   // 95
};


VmacC4::VmacC4(int chipIndex, int macBlock, uint32_t baseAddr)
  : RegisterBlock(chipIndex, counter_base_addr(baseAddr), kCounterSpaceSizeBytes, false, nullptr, nullptr, "VmacC4"),
    Mac(kNumChans), base_addr_(baseAddr), mac_block_(macBlock) {
  static_assert((sizeof(kMacCounterMapperTab)/sizeof(kMacCounterMapperTab[0]) == kMaxMapEntries),
                "Should be exactly kMaxMapEntries defined in VmacC4::kMacCounterMapperTab");
  // Check consecutive entries in kMacCounterMapperTab have consecutive vendor_index values
  for (int i = 0; i < kMaxMapEntries; i++) {
    RMT_ASSERT( (i == kMacCounterMapperTab[i].vendor_index) && "Bad VmacC4::kMacCounterMapperTab");
  }
}
VmacC4::~VmacC4() { }


uint64_t VmacC4::mac_counter_read(int chan, int umac4_index, bool clear) {
  RMT_ASSERT((chan >= 0) && (chan < kNumChans));
  if ((umac4_index < 0) || (umac4_index >= kMaxMapEntries)) return UINT64_C(0);
  const MacCounterMapperConfig& e = kMacCounterMapperTab[umac4_index];
  return read_counter_mask(e.is_rx, chan,
                           MacCounters::get_base(e.cntr_flavor0), e.cntr_mask0,
                           MacCounters::get_base(e.cntr_flavor1), e.cntr_mask1,
                           clear, e.do_subtract);
}


bool VmacC4::read(uint32_t offset, uint32_t* data) const {
  RMT_ASSERT(data != nullptr);
  RMT_ASSERT( offset < kCounterSpaceSizeBytes );
  RMT_ASSERT( offset_get_zeros(offset) == 0 );
  int chan = offset_get_chan(offset);
  int cntr = offset_get_cntr(offset);
  int hilo = offset_get_hilo(offset);
  // This const_cast is yuck but mac_counter_read() calls read_counter_mask()
  // which may have to clear counters and so can't be const.
  // In this case we invoke func with clear=false so no clear would happen anyway
  uint64_t d = const_cast<VmacC4*>(this)->mac_counter_read(chan, cntr, false);
  *data = (hilo == 0) ?static_cast<uint32_t>(d & 0xFFFFFFFFu) :static_cast<uint32_t>(d >> 32);
  return true;
}
bool VmacC4::write(uint32_t offset, uint32_t data) {
  RMT_ASSERT(offset < kCounterSpaceSizeBytes );
  RMT_ASSERT( offset_get_zeros(offset) == 0 );
  RMT_ASSERT( data == 0u ); // TODO: allow arbitrary values to be written
  int chan = offset_get_chan(offset);
  int cntr = offset_get_cntr(offset);
  (void)mac_counter_read(chan, cntr, true);
  return true;
}



}
