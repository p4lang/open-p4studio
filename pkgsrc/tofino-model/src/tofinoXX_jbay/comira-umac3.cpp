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
#include <comira-umac3.h>

namespace MODEL_CHIP_NAMESPACE {

// Mapping table to map Comira3 specific MAC counter indices to generic counter indices
//
// For each vendor_index this table contains:
// 1. Whether the counter is a RX counter (true => RX, false => TX)
// 2. Upto 2 32b masks of counters to sum to form the result for the vendor_index
// 3. A flag to indicate whether the sum corresponding to cntr_mask1 should be subtracted or added to the sum from cntr_mask0
//
const MacCounterMapperConfig VmacC3::kMacCounterMapperTab[] = {
  // vendor_index                          is_rx  cntr_flavor0           cntr_mask0                       cntr_flavor1        cntr_mask1            do_subtract
  {  VmacC3Cntr::FramesRcvdOK,             true,  CntrFlavor::Frames,    1u<<Cntr::OK,                    CntrFlavor::None,   0u,                   false    },  //  0
  {  VmacC3Cntr::FramesRcvdAll,            true,  CntrFlavor::Frames,    1u<<Cntr::All,                   CntrFlavor::None,   0u,                   false    },  //  1
  {  VmacC3Cntr::FramesRcvdCRCError,       true,  CntrFlavor::Frames,    1u<<Cntr::CrcError,              CntrFlavor::None,   0u,                   false    },  //  2
  {  VmacC3Cntr::FramesRcvdError,          true,  CntrFlavor::Frames,    1u<<Cntr::Error,                 CntrFlavor::None,   0u,                   false    },  //  3
  {  VmacC3Cntr::OctetsRcvdOK,             true,  CntrFlavor::Octets,    1u<<Cntr::OK,                    CntrFlavor::None,   0u,                   false    },  //  4
  {  VmacC3Cntr::OctetsRcvdAll,            true,  CntrFlavor::Octets,    1u<<Cntr::All,                   CntrFlavor::None,   0u,                   false    },  //  5
  {  VmacC3Cntr::FramesRcvdUnicast,        true,  CntrFlavor::Frames,    1u<<Cntr::Unicast,               CntrFlavor::None,   0u,                   false    },  //  6
  {  VmacC3Cntr::FramesRcvdMulticast,      true,  CntrFlavor::Frames,    1u<<Cntr::Multicast,             CntrFlavor::None,   0u,                   false    },  //  7
  {  VmacC3Cntr::FramesRcvdBroadcast,      true,  CntrFlavor::Frames,    1u<<Cntr::Broadcast,             CntrFlavor::None,   0u,                   false    },  //  8
  {  VmacC3Cntr::FramesRcvdPause,          true,  CntrFlavor::Frames,    1u<<Cntr::Pause,                 CntrFlavor::None,   0u,                   false    },  //  9
  {  VmacC3Cntr::FramesRcvdLenError,       true,  CntrFlavor::Frames,    1u<<Cntr::LenErr,                CntrFlavor::None,   0u,                   false    },  // 10
  {  VmacC3Cntr::FramesRcvdUndersized,     true,  CntrFlavor::Frames,    1u<<Cntr::Undersized,            CntrFlavor::None,   0u,                   false    },  // 11
  {  VmacC3Cntr::FramesRcvdOversized,      true,  CntrFlavor::Frames,    1u<<Cntr::Oversized,             CntrFlavor::None,   0u,                   false    },  // 12
  {  VmacC3Cntr::FramesRcvdFragments,      true,  CntrFlavor::Frames,    1u<<Cntr::Fragment,              CntrFlavor::None,   0u,                   false    },  // 13
  {  VmacC3Cntr::FramesRcvdJabber,         true,  CntrFlavor::Frames,    1u<<Cntr::Jabber,                CntrFlavor::None,   0u,                   false    },  // 14
  {  VmacC3Cntr::FramesRcvdPriPause,       true,  CntrFlavor::Frames,    1u<<Cntr::PriPause,              CntrFlavor::None,   0u,                   false    },  // 15
  {  VmacC3Cntr::FramesRcvdCrcErrStomp,    true,  CntrFlavor::Frames,    1u<<Cntr::CrcErrStomp,           CntrFlavor::None,   0u,                   false    },  // 16
  {  VmacC3Cntr::FramesRcvdFrameTooLong,   true,  CntrFlavor::Frames,    1u<<Cntr::TooLongErr,            CntrFlavor::None,   0u,                   false    },  // 17
  {  VmacC3Cntr::FramesRcvdVLAN,           true,  CntrFlavor::Frames,    1u<<Cntr::Vlan,                  CntrFlavor::None,   0u,                   false    },  // 18
  {  VmacC3Cntr::FramesDropped,            true,  CntrFlavor::Frames,    1u<<Cntr::Dropped,               CntrFlavor::None,   0u,                   false    },  // 19
  {  VmacC3Cntr::FramesRcvdSizeLT64,       true,  CntrFlavor::Bucket64,  1u<<0,                           CntrFlavor::None,   0u,                   false    },  // 20
  {  VmacC3Cntr::FramesRcvdSizeEQ64,       true,  CntrFlavor::Frames,    1u<<Cntr::SizeEq64,              CntrFlavor::None,   0u,                   false    },  // 21
  {  VmacC3Cntr::FramesRcvdSize65to127,    true,  CntrFlavor::Bucket64,  1u<<1,                           CntrFlavor::Frames, 1u<<Cntr::SizeEq64_B, true     },  // 22
  {  VmacC3Cntr::FramesRcvdSize128to255,   true,  CntrFlavor::Bucket64,  MacCounters::make_range(2,3),    CntrFlavor::None,   0u,                   false    },  // 23
  {  VmacC3Cntr::FramesRcvdSize256to511,   true,  CntrFlavor::Bucket64,  MacCounters::make_range(4,7),    CntrFlavor::None,   0u,                   false    },  // 24
  {  VmacC3Cntr::FramesRcvdSize512to1023,  true,  CntrFlavor::Bucket512, 1u<<1,                           CntrFlavor::None,   0u,                   false    },  // 25
  {  VmacC3Cntr::FramesRcvdSize1024to1518, true,  CntrFlavor::Bucket512, 1u<<2,                           CntrFlavor::Frames, 1u<<Cntr::Size15xx_A, true     },  // 26
  {  VmacC3Cntr::FramesRcvdSize1519to2047, true,  CntrFlavor::Bucket512, 1u<<3,                           CntrFlavor::Frames, 1u<<Cntr::Size15xx_B, false    },  // 27
  {  VmacC3Cntr::FramesRcvdSize2048to4095, true,  CntrFlavor::Bucket512, MacCounters::make_range(4,7),    CntrFlavor::None,   0u,                   false    },  // 28
  {  VmacC3Cntr::FramesRcvdSize4096to8191, true,  CntrFlavor::Bucket512, MacCounters::make_range(8,15),   CntrFlavor::None,   0u,                   false    },  // 29
  {  VmacC3Cntr::FramesRcvdSize8192to9215, true,  CntrFlavor::Bucket512, MacCounters::make_range(16,17),  CntrFlavor::None,   0u,                   false    },  // 30
  {  VmacC3Cntr::FramesRcvdSizeGT9216,     true,  CntrFlavor::Bucket512, MacCounters::make_range(18,31),  CntrFlavor::None,   0u,                   false    },  // 31
  {  VmacC3Cntr::FramesXmitOK,             false, CntrFlavor::Frames,    1u<<Cntr::OK,                    CntrFlavor::None,   0u,                   false    },  // 32
  {  VmacC3Cntr::FramesXmitAll,            false, CntrFlavor::Frames,    1u<<Cntr::All,                   CntrFlavor::None,   0u,                   false    },  // 33
  {  VmacC3Cntr::FramesXmitError,          false, CntrFlavor::Frames,    1u<<Cntr::Error,                 CntrFlavor::None,   0u,                   false    },  // 34
  {  VmacC3Cntr::OctetsXmitOK,             false, CntrFlavor::Octets,    1u<<Cntr::OK,                    CntrFlavor::None,   0u,                   false    },  // 35
  {  VmacC3Cntr::OctetsXmitAll,            false, CntrFlavor::Octets,    1u<<Cntr::All,                   CntrFlavor::None,   0u,                   false    },  // 36
  {  VmacC3Cntr::FramesXmitUnicast,        false, CntrFlavor::Frames,    1u<<Cntr::Unicast,               CntrFlavor::None,   0u,                   false    },  // 37
  {  VmacC3Cntr::FramesXmitMulticast,      false, CntrFlavor::Frames,    1u<<Cntr::Multicast,             CntrFlavor::None,   0u,                   false    },  // 38
  {  VmacC3Cntr::FramesXmitBroadcast,      false, CntrFlavor::Frames,    1u<<Cntr::Broadcast,             CntrFlavor::None,   0u,                   false    },  // 39
  {  VmacC3Cntr::FramesXmitPause,          false, CntrFlavor::Frames,    1u<<Cntr::Pause,                 CntrFlavor::None,   0u,                   false    },  // 40
  {  VmacC3Cntr::FramesXmitPriPause,       false, CntrFlavor::Frames,    1u<<Cntr::PriPause,              CntrFlavor::None,   0u,                   false    },  // 41
  {  VmacC3Cntr::FramesXmitVLAN,           false, CntrFlavor::Frames,    1u<<Cntr::Vlan,                  CntrFlavor::None,   0u,                   false    },  // 42
  {  VmacC3Cntr::FramesXmitSizeLT64,       false, CntrFlavor::Bucket64,  1u<<0,                           CntrFlavor::None,   0u,                   false    },  // 43
  {  VmacC3Cntr::FramesXmitSizeEQ64,       false, CntrFlavor::Frames,    1u<<Cntr::SizeEq64,              CntrFlavor::None,   0u,                   false    },  // 44
  {  VmacC3Cntr::FramesXmitSize65to127,    false, CntrFlavor::Bucket64,  1u<<1,                           CntrFlavor::Frames, 1u<<Cntr::SizeEq64_B, true     },  // 45
  {  VmacC3Cntr::FramesXmitSize128to255,   false, CntrFlavor::Bucket64,  MacCounters::make_range(2,3),    CntrFlavor::None,   0u,                   false    },  // 46
  {  VmacC3Cntr::FramesXmitSize256to511,   false, CntrFlavor::Bucket64,  MacCounters::make_range(4,7),    CntrFlavor::None,   0u,                   false    },  // 47
  {  VmacC3Cntr::FramesXmitSize512to1023,  false, CntrFlavor::Bucket512, 1u<<1,                           CntrFlavor::None,   0u,                   false    },  // 48
  {  VmacC3Cntr::FramesXmitSize1024to1518, false, CntrFlavor::Bucket512, 1u<<2,                           CntrFlavor::Frames, 1u<<Cntr::Size15xx_A, true     },  // 49
  {  VmacC3Cntr::FramesXmitSize1519to2047, false, CntrFlavor::Bucket512, 1u<<3,                           CntrFlavor::Frames, 1u<<Cntr::Size15xx_B, false    },  // 50
  {  VmacC3Cntr::FramesXmitSize2048to4095, false, CntrFlavor::Bucket512, MacCounters::make_range(4,7),    CntrFlavor::None,   0u,                   false    },  // 51
  {  VmacC3Cntr::FramesXmitSize4096to8191, false, CntrFlavor::Bucket512, MacCounters::make_range(8,15),   CntrFlavor::None,   0u,                   false    },  // 52
  {  VmacC3Cntr::FramesXmitSize8192to9215, false, CntrFlavor::Bucket512, MacCounters::make_range(16,17),  CntrFlavor::None,   0u,                   false    },  // 53
  {  VmacC3Cntr::FramesXmitSizeGT9216,     false, CntrFlavor::Bucket512, MacCounters::make_range(18,31),  CntrFlavor::None,   0u,                   false    },  // 54
  {  VmacC3Cntr::FramesXmitPri0,           false, CntrFlavor::Pri,       1u<<0,                           CntrFlavor::None,   0u,                   false    },  // 55
  {  VmacC3Cntr::FramesXmitPri1,           false, CntrFlavor::Pri,       1u<<1,                           CntrFlavor::None,   0u,                   false    },  // 56
  {  VmacC3Cntr::FramesXmitPri2,           false, CntrFlavor::Pri,       1u<<2,                           CntrFlavor::None,   0u,                   false    },  // 57
  {  VmacC3Cntr::FramesXmitPri3,           false, CntrFlavor::Pri,       1u<<3,                           CntrFlavor::None,   0u,                   false    },  // 58
  {  VmacC3Cntr::FramesXmitPri4,           false, CntrFlavor::Pri,       1u<<4,                           CntrFlavor::None,   0u,                   false    },  // 59
  {  VmacC3Cntr::FramesXmitPri5,           false, CntrFlavor::Pri,       1u<<5,                           CntrFlavor::None,   0u,                   false    },  // 60
  {  VmacC3Cntr::FramesXmitPri6,           false, CntrFlavor::Pri,       1u<<6,                           CntrFlavor::None,   0u,                   false    },  // 61
  {  VmacC3Cntr::FramesXmitPri7,           false, CntrFlavor::Pri,       1u<<7,                           CntrFlavor::None,   0u,                   false    },  // 62
  {  VmacC3Cntr::FramesRcvdPri0,           true,  CntrFlavor::Pri,       1u<<0,                           CntrFlavor::None,   0u,                   false    },  // 63
  {  VmacC3Cntr::FramesRcvdPri1,           true,  CntrFlavor::Pri,       1u<<1,                           CntrFlavor::None,   0u,                   false    },  // 64
  {  VmacC3Cntr::FramesRcvdPri2,           true,  CntrFlavor::Pri,       1u<<2,                           CntrFlavor::None,   0u,                   false    },  // 65
  {  VmacC3Cntr::FramesRcvdPri3,           true,  CntrFlavor::Pri,       1u<<3,                           CntrFlavor::None,   0u,                   false    },  // 66
  {  VmacC3Cntr::FramesRcvdPri4,           true,  CntrFlavor::Pri,       1u<<4,                           CntrFlavor::None,   0u,                   false    },  // 67
  {  VmacC3Cntr::FramesRcvdPri5,           true,  CntrFlavor::Pri,       1u<<5,                           CntrFlavor::None,   0u,                   false    },  // 68
  {  VmacC3Cntr::FramesRcvdPri6,           true,  CntrFlavor::Pri,       1u<<6,                           CntrFlavor::None,   0u,                   false    },  // 69
  {  VmacC3Cntr::FramesRcvdPri7,           true,  CntrFlavor::Pri,       1u<<7,                           CntrFlavor::None,   0u,                   false    },  // 70
  {  VmacC3Cntr::XmitPri0Pause1US,         false, CntrFlavor::PriPause,  1u<<0,                           CntrFlavor::None,   0u,                   false    },  // 71
  {  VmacC3Cntr::XmitPri1Pause1US,         false, CntrFlavor::PriPause,  1u<<1,                           CntrFlavor::None,   0u,                   false    },  // 72
  {  VmacC3Cntr::XmitPri2Pause1US,         false, CntrFlavor::PriPause,  1u<<2,                           CntrFlavor::None,   0u,                   false    },  // 73
  {  VmacC3Cntr::XmitPri3Pause1US,         false, CntrFlavor::PriPause,  1u<<3,                           CntrFlavor::None,   0u,                   false    },  // 74
  {  VmacC3Cntr::XmitPri4Pause1US,         false, CntrFlavor::PriPause,  1u<<4,                           CntrFlavor::None,   0u,                   false    },  // 75
  {  VmacC3Cntr::XmitPri5Pause1US,         false, CntrFlavor::PriPause,  1u<<5,                           CntrFlavor::None,   0u,                   false    },  // 76
  {  VmacC3Cntr::XmitPri6Pause1US,         false, CntrFlavor::PriPause,  1u<<6,                           CntrFlavor::None,   0u,                   false    },  // 77
  {  VmacC3Cntr::XmitPri7Pause1US,         false, CntrFlavor::PriPause,  1u<<7,                           CntrFlavor::None,   0u,                   false    },  // 78
  {  VmacC3Cntr::RcvdPri0Pause1US,         true,  CntrFlavor::PriPause,  1u<<0,                           CntrFlavor::None,   0u,                   false    },  // 79
  {  VmacC3Cntr::RcvdPri1Pause1US,         true,  CntrFlavor::PriPause,  1u<<1,                           CntrFlavor::None,   0u,                   false    },  // 80
  {  VmacC3Cntr::RcvdPri2Pause1US,         true,  CntrFlavor::PriPause,  1u<<2,                           CntrFlavor::None,   0u,                   false    },  // 81
  {  VmacC3Cntr::RcvdPri3Pause1US,         true,  CntrFlavor::PriPause,  1u<<3,                           CntrFlavor::None,   0u,                   false    },  // 82
  {  VmacC3Cntr::RcvdPri4Pause1US,         true,  CntrFlavor::PriPause,  1u<<4,                           CntrFlavor::None,   0u,                   false    },  // 83
  {  VmacC3Cntr::RcvdPri5Pause1US,         true,  CntrFlavor::PriPause,  1u<<5,                           CntrFlavor::None,   0u,                   false    },  // 84
  {  VmacC3Cntr::RcvdPri6Pause1US,         true,  CntrFlavor::PriPause,  1u<<6,                           CntrFlavor::None,   0u,                   false    },  // 85
  {  VmacC3Cntr::RcvdPri7Pause1US,         true,  CntrFlavor::PriPause,  1u<<7,                           CntrFlavor::None,   0u,                   false    },  // 86
  {  VmacC3Cntr::RcvdStdPause1US,          true,  CntrFlavor::PriPause,  1u<<8,                           CntrFlavor::None,   0u,                   false    },  // 87
  {  VmacC3Cntr::FramesRcvdTrunc,          true,  CntrFlavor::Frames,    1u<<Cntr::Truncated,             CntrFlavor::None,   0u,                   false    }   // 88
};

VmacC3::VmacC3(int chipIndex, int macBlock, uint32_t baseAddr)
  : RegisterBlock(chipIndex, counter_base_addr(baseAddr), kCounterSpaceSizeBytes, false, nullptr, nullptr, "VmacC3"),
    Mac(kNumChans), base_addr_(baseAddr), mac_block_(macBlock),
    sw_reset_(0), rd_ctrl_(0), stats_rst_(0), data_(UINT64_C(0)), other_reg_vals_() {
  static_assert((sizeof(kMacCounterMapperTab)/sizeof(kMacCounterMapperTab[0]) == kMaxMapEntries),
                "Should be exactly kMaxMapEntries defined in VmacC3::kMacCounterMapperTab");
  // Check consecutive entries in kMacCounterMapperTab have consecutive vendor_index values
  for (int i = 0; i < kMaxMapEntries; i++) {
    RMT_ASSERT( (i == kMacCounterMapperTab[i].vendor_index) && "Bad VmacC3::kMacCounterMapperTab");
  }
}
VmacC3::~VmacC3() {
  other_reg_vals_.clear();
}



uint64_t VmacC3::mac_counter_read(int chan, int umac3_index, bool clear) {
  RMT_ASSERT((chan >= 0) && (chan < kNumChans));
  if ((umac3_index < 0) || (umac3_index >= kMaxMapEntries)) return UINT64_C(0);
  const MacCounterMapperConfig& e = kMacCounterMapperTab[umac3_index];
  return read_counter_mask(e.is_rx, chan,
                           MacCounters::get_base(e.cntr_flavor0), e.cntr_mask0,
                           MacCounters::get_base(e.cntr_flavor1), e.cntr_mask1,
                           clear, e.do_subtract);
}


void VmacC3::do_read_reg(uint32_t offset, uint32_t *data) const {
  uint16_t addr = static_cast<uint16_t>(offset & 0xFFFF);
  try { // Read other register values from map
    *data = static_cast<uint32_t>(other_reg_vals_.at(addr));
  } catch (const std::exception&) {
    *data = 0u;
  }
}
void VmacC3::do_write_reg(uint32_t offset, uint32_t data) {
  // Note this does not and can not take account of RO CSRs
  uint16_t addr = static_cast<uint16_t>(offset & 0xFFFF);
  uint16_t val  = static_cast<uint16_t>(data & 0xFFFF);
  try { // Write other register values to map
    other_reg_vals_.emplace(addr, val);
  } catch (const std::exception&) { }
}
void VmacC3::do_clear_all(uint32_t chans) {
  // Clear down counters for selected chans
  for (int ch = 0; ch < kNumChans; ch++) {
    if (((chans >> ch) & 1) == 1) clear_counters(ch);
  }
}

bool VmacC3::read(uint32_t offset, uint32_t* data) const {
  RMT_ASSERT(data != nullptr);
  // Offset is byte address of a word-addressed CSR
  RMT_ASSERT( offset < kCounterSpaceSizeBytes);
  RMT_ASSERT((offset % kAddrSpaceWordSize) == 0);
  offset /= kAddrSpaceWordSize;

  // Use cached copies of rd_ctrl/stats_rst/sw_reset
  if (offset == kOffsetRdCtrl) {
    *data = static_cast<uint32_t>( rd_ctrl_ );

  } else if (offset == kOffsetRData0) {
    *data = static_cast<uint32_t>( (data_ >>  0) & 0xFFFF );
  } else if (offset == kOffsetRData1) {
    *data = static_cast<uint32_t>( (data_ >> 16) & 0xFFFF );
  } else if (offset == kOffsetRData2) {
    *data = static_cast<uint32_t>( (data_ >> 32) & 0xFFFF );
  } else if (offset == kOffsetRData3) {
    *data = static_cast<uint32_t>( (data_ >> 48) & 0xFFFF );

  } else if (offset == kOffsetStatsRst) {
    *data = static_cast<uint32_t>( stats_rst_ );
  } else if (offset == kOffsetSwReset) {
    *data = static_cast<uint32_t>( sw_reset_ );
  } else {
    // This currently unreachable.
    // ComiraUmac3 registers for 28B of address space exactly
    // covering RdCtrl, RData0-3, StatsRst and SwReset CSRs.
    RMT_ASSERT(0 && "Should not be reached");
    //do_read_reg(offset, data);
  }
  return true;
}
bool VmacC3::write(uint32_t offset, uint32_t data) {
  // Offset is byte address of a word-addressed CSR
  RMT_ASSERT(offset < kCounterSpaceSizeBytes);
  RMT_ASSERT((offset % kAddrSpaceWordSize) == 0);
  offset /= kAddrSpaceWordSize;

  // Use cached copies of rd_ctrl/stats_rst/sw_reset
  if (offset == kOffsetRdCtrl) {
    rd_ctrl_ = static_cast<uint16_t>( data & 0xFFFF );
    if (((data >> 15) & 1) == 1) {
      bool pri = (((data >> 14) & 1) == 1);
      bool clr = (((data >> 13) & 1) == 1);
      bool clr_cntr = (clr && !pri); // Clear only ok for non-pri reads
      int  chan = (data >> 7) & 0x03;
      int  cntr = (data >> 0) & 0x7F;
      // Load selected counter into data_ for reading
      data_ = mac_counter_read(chan, cntr, clr_cntr);
      // Clear top bit of rd_ctrl data to indicate read done
      rd_ctrl_ = static_cast<uint16_t>( data & 0x7FFF );
    }

  } else if ((offset == kOffsetRData0) || (offset == kOffsetRData1) ||
             (offset == kOffsetRData2) || (offset == kOffsetRData3)) {
    // rdata0..rdata3 are read-only so ignore write

  } else if (offset == kOffsetStatsRst) {
    stats_rst_ = static_cast<uint16_t>( data & 0xFFFF );
    do_clear_all(data & 0xF);
  } else if (offset == kOffsetSwReset) {
    sw_reset_ = static_cast<uint16_t>( data & 0xFFFF );
    do_clear_all(data & 0xF);
  } else {
    // This currently unreachable.
    // ComiraUmac3 registers for 28B of address space exactly
    // covering RdCtrl, RData0-3, StatsRst and SwReset CSRs.
    RMT_ASSERT(0 && "Should not be reached");
    //do_write_reg(offset, data);
  }
  return true;
}



}
