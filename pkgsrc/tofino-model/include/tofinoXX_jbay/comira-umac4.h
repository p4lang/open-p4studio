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

#ifndef _SHARED_COMIRA_UMAC4_H_
#define _SHARED_COMIRA_UMAC4_H_

#include <model_core/register_block.h>
#include <mac.h>
#include <register_utils.h>

namespace MODEL_CHIP_NAMESPACE {

// Counter numbering derived from bf-drivers/src/port_mgr/port_mgr_tof2/umac4_rmon.h
struct VmacC4Cntr {
  static constexpr int FramesXmitOK             =  0;
  static constexpr int FramesXmitAll            =  1;
  static constexpr int FramesXmitError          =  2;
  static constexpr int OctetsXmitOK             =  3;
  static constexpr int OctetsXmitAll            =  4;
  static constexpr int FramesXmitUnicast        =  5;
  static constexpr int FramesXmitMulticast      =  6;
  static constexpr int FramesXmitBroadcast      =  7;
  static constexpr int FramesXmitPause          =  8;
  static constexpr int FramesXmitPriPause       =  9;
  static constexpr int FramesXmitVLAN           = 10;
  static constexpr int FramesXmitSizeLT64       = 11;
  static constexpr int FramesXmitSizeEQ64       = 12;
  static constexpr int FramesXmitSize65to127    = 13;
  static constexpr int FramesXmitSize128to255   = 14;
  static constexpr int FramesXmitSize256to511   = 15;
  static constexpr int FramesXmitSize512to1023  = 16;
  static constexpr int FramesXmitSize1024to1518 = 17;
  static constexpr int FramesXmitSize1519to2047 = 18;
  static constexpr int FramesXmitSize2048to4095 = 19;
  static constexpr int FramesXmitSize4096to8191 = 20;
  static constexpr int FramesXmitSize8192to9215 = 21;
  static constexpr int FramesXmitSizeGT9216     = 22;
  static constexpr int FramesXmitPri0           = 23;
  static constexpr int FramesXmitPri1           = 24;
  static constexpr int FramesXmitPri2           = 25;
  static constexpr int FramesXmitPri3           = 26;
  static constexpr int FramesXmitPri4           = 27;
  static constexpr int FramesXmitPri5           = 28;
  static constexpr int FramesXmitPri6           = 29;
  static constexpr int FramesXmitPri7           = 30;
  static constexpr int XmitPri0Pause1US         = 31;
  static constexpr int XmitPri1Pause1US         = 32;
  static constexpr int XmitPri2Pause1US         = 33;
  static constexpr int XmitPri3Pause1US         = 34;
  static constexpr int XmitPri4Pause1US         = 35;
  static constexpr int XmitPri5Pause1US         = 36;
  static constexpr int XmitPri6Pause1US         = 37;
  static constexpr int XmitPri7Pause1US         = 38;
  static constexpr int FramesXmitDrained        = 39;
  static constexpr int FramesXmitJabbered       = 40;
  static constexpr int FramesXmitPadded         = 41;
  static constexpr int FramesXmitTruncated      = 42;
  static constexpr int FramesRcvdOK             = 43;
  static constexpr int OctetsRcvdOK             = 44;
  static constexpr int FramesRcvdAll            = 45;
  static constexpr int OctetsRcvdAll            = 46;
  static constexpr int FramesRcvdCRCError       = 47;
  static constexpr int FramesRcvdError          = 48;
  static constexpr int FramesRcvdUnicast        = 49;
  static constexpr int FramesRcvdMulticast      = 50;
  static constexpr int FramesRcvdBroadcast      = 51;
  static constexpr int FramesRcvdPause          = 52;
  static constexpr int FramesRcvdLenError       = 53;
  static constexpr int Reserved                 = 54;
  static constexpr int FramesRcvdOversized      = 55;
  static constexpr int FramesRcvdFragments      = 56;
  static constexpr int FramesRcvdJabber         = 57;
  static constexpr int FramesRcvdPriPause       = 58;
  static constexpr int FramesRcvdCrcErrStomp    = 59;
  static constexpr int FramesRcvdMaxFrmSizeVio  = 60;
  static constexpr int FramesRcvdVLAN           = 61;
  static constexpr int FramesDropped            = 62;
  static constexpr int FramesRcvdSizeLT64       = 63;
  static constexpr int FramesRcvdSizeEQ64       = 64;
  static constexpr int FramesRcvdSize65to127    = 65;
  static constexpr int FramesRcvdSize128to255   = 66;
  static constexpr int FramesRcvdSize256to511   = 67;
  static constexpr int FramesRcvdSize512to1023  = 68;
  static constexpr int FramesRcvdSize1024to1518 = 69;
  static constexpr int FramesRcvdSize1519to2047 = 70;
  static constexpr int FramesRcvdSize2048to4095 = 71;
  static constexpr int FramesRcvdSize4096to8191 = 72;
  static constexpr int FramesRcvdSize8192to9215 = 73;
  static constexpr int FramesRcvdSizeGT9216     = 74;
  static constexpr int FramesRcvdPri0           = 75;
  static constexpr int FramesRcvdPri1           = 76;
  static constexpr int FramesRcvdPri2           = 77;
  static constexpr int FramesRcvdPri3           = 78;
  static constexpr int FramesRcvdPri4           = 79;
  static constexpr int FramesRcvdPri5           = 80;
  static constexpr int FramesRcvdPri6           = 81;
  static constexpr int FramesRcvdPri7           = 82;
  static constexpr int RcvdPri0Pause1US         = 83;
  static constexpr int RcvdPri1Pause1US         = 84;
  static constexpr int RcvdPri2Pause1US         = 85;
  static constexpr int RcvdPri3Pause1US         = 86;
  static constexpr int RcvdPri4Pause1US         = 87;
  static constexpr int RcvdPri5Pause1US         = 88;
  static constexpr int RcvdPri6Pause1US         = 89;
  static constexpr int RcvdPri7Pause1US         = 90;
  static constexpr int RcvdStdPause1US          = 91;
  static constexpr int FramesRcvdTrunc          = 92;
  static constexpr int Reserved2                = 93;
  static constexpr int InvalidPreamble          = 94;
  static constexpr int NormalLenInvalidCRC      = 95;
  static constexpr int MaxCounter               = 95; // Same as preceding entry!
};


class VmacC4 : public model_core::RegisterBlock<RegisterCallback>, public Mac {

  static constexpr int      kMaxMapEntries         = VmacC4Cntr::MaxCounter + 1;
  static constexpr int      kNumChans              = 8;
  static constexpr uint32_t kAddrSpaceStrideBytes  = RegisterUtils::vmac_c4_stride;
  static constexpr uint32_t kCounterSpaceOffset    = RegisterUtils::vmac_c4_cntr_offset;
  static constexpr uint32_t kCounterSpaceSizeBytes = RegisterUtils::vmac_c4_cntr_size;

  static uint32_t counter_base_addr(uint32_t base) { return base + kCounterSpaceOffset; }

  // Umac4 counter offsets are chan[12:10] cntr[9:3] hilo[2] zeros[1:0]
  static int      offset_get_chan(uint32_t off)    { return (off >> 10) &  0x7; }
  static int      offset_get_cntr(uint32_t off)    { return (off >>  3) & 0x7F; }
  static int      offset_get_hilo(uint32_t off)    { return (off >>  2) &  0x1; }
  static int      offset_get_zeros(uint32_t off)   { return (off >>  0) &  0x3; }

  // Map defined in comira-umac4.cpp
  static const MacCounterMapperConfig kMacCounterMapperTab[kMaxMapEntries];

 public:
  VmacC4(int chipIndex, int macBlock, uint32_t baseAddr);
  virtual ~VmacC4();

  // Implement virtual funcs from Mac - all counter funcs
  uint64_t mac_counter_read(int chan, int umac4_index, bool clear) override;

  uint32_t mac_counter_base_addr() const override { return counter_base_addr(base_addr_); }
  uint32_t mac_stride()            const override { return kAddrSpaceStrideBytes; }
  int      mac_block()             const override { return mac_block_; }
  int      mac_type()              const override { return MacType::kVmacC4; }

  // Implement virtual funcs from RegisterBlock
  bool read(uint32_t offset, uint32_t* data) const override;
  bool write(uint32_t offset, uint32_t data) override;

  std::string to_string(bool print_zeros = false,
                        std::string indent_string = "") const override {
    return "";
  }
  std::string to_string(uint32_t offset, bool print_zeros = false,
                        std::string indent_string = "") const override {
    return "";
  }


 private:
  uint32_t  base_addr_;
  int       mac_block_;

};

}

#endif // _SHARED_COMIRA_UMAC4_H_
