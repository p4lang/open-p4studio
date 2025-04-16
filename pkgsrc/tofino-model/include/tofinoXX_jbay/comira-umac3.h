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

#ifndef _SHARED_COMIRA_UMAC3_H_
#define _SHARED_COMIRA_UMAC3_H_

#include <unordered_map>
#include <model_core/register_block.h>
#include <mac.h>
#include <register_utils.h>

namespace MODEL_CHIP_NAMESPACE {

// Counter numbering derived from document on Google Drive
// Hardware Documents/Vendors/Comira/Tofino-IP-Docs/u1244_14_4_b1_registers_v1_3.pdf
struct VmacC3Cntr {
  static constexpr int FramesRcvdOK             =  0;
  static constexpr int FramesRcvdAll            =  1;
  static constexpr int FramesRcvdCRCError       =  2;
  static constexpr int FramesRcvdError          =  3;
  static constexpr int OctetsRcvdOK             =  4;
  static constexpr int OctetsRcvdAll            =  5;
  static constexpr int FramesRcvdUnicast        =  6;
  static constexpr int FramesRcvdMulticast      =  7;
  static constexpr int FramesRcvdBroadcast      =  8;
  static constexpr int FramesRcvdPause          =  9;
  static constexpr int FramesRcvdLenError       = 10;
  static constexpr int FramesRcvdUndersized     = 11;
  static constexpr int FramesRcvdOversized      = 12;
  static constexpr int FramesRcvdFragments      = 13;
  static constexpr int FramesRcvdJabber         = 14;
  static constexpr int FramesRcvdPriPause       = 15;
  static constexpr int FramesRcvdCrcErrStomp    = 16;
  static constexpr int FramesRcvdFrameTooLong   = 17;
  static constexpr int FramesRcvdVLAN           = 18;
  static constexpr int FramesDropped            = 19;
  static constexpr int FramesRcvdSizeLT64       = 20;
  static constexpr int FramesRcvdSizeEQ64       = 21;
  static constexpr int FramesRcvdSize65to127    = 22;
  static constexpr int FramesRcvdSize128to255   = 23;
  static constexpr int FramesRcvdSize256to511   = 24;
  static constexpr int FramesRcvdSize512to1023  = 25;
  static constexpr int FramesRcvdSize1024to1518 = 26;
  static constexpr int FramesRcvdSize1519to2047 = 27;
  static constexpr int FramesRcvdSize2048to4095 = 28;
  static constexpr int FramesRcvdSize4096to8191 = 29;
  static constexpr int FramesRcvdSize8192to9215 = 30;
  static constexpr int FramesRcvdSizeGT9216     = 31;
  static constexpr int FramesXmitOK             = 32;
  static constexpr int FramesXmitAll            = 33;
  static constexpr int FramesXmitError          = 34;
  static constexpr int OctetsXmitOK             = 35;
  static constexpr int OctetsXmitAll            = 36;
  static constexpr int FramesXmitUnicast        = 37;
  static constexpr int FramesXmitMulticast      = 38;
  static constexpr int FramesXmitBroadcast      = 39;
  static constexpr int FramesXmitPause          = 40;
  static constexpr int FramesXmitPriPause       = 41;
  static constexpr int FramesXmitVLAN           = 42;
  static constexpr int FramesXmitSizeLT64       = 43;
  static constexpr int FramesXmitSizeEQ64       = 44;
  static constexpr int FramesXmitSize65to127    = 45;
  static constexpr int FramesXmitSize128to255   = 46;
  static constexpr int FramesXmitSize256to511   = 47;
  static constexpr int FramesXmitSize512to1023  = 48;
  static constexpr int FramesXmitSize1024to1518 = 49;
  static constexpr int FramesXmitSize1519to2047 = 50;
  static constexpr int FramesXmitSize2048to4095 = 51;
  static constexpr int FramesXmitSize4096to8191 = 52;
  static constexpr int FramesXmitSize8192to9215 = 53;
  static constexpr int FramesXmitSizeGT9216     = 54;
  static constexpr int FramesXmitPri0           = 55;
  static constexpr int FramesXmitPri1           = 56;
  static constexpr int FramesXmitPri2           = 57;
  static constexpr int FramesXmitPri3           = 58;
  static constexpr int FramesXmitPri4           = 59;
  static constexpr int FramesXmitPri5           = 60;
  static constexpr int FramesXmitPri6           = 61;
  static constexpr int FramesXmitPri7           = 62;
  static constexpr int FramesRcvdPri0           = 63;
  static constexpr int FramesRcvdPri1           = 64;
  static constexpr int FramesRcvdPri2           = 65;
  static constexpr int FramesRcvdPri3           = 66;
  static constexpr int FramesRcvdPri4           = 67;
  static constexpr int FramesRcvdPri5           = 68;
  static constexpr int FramesRcvdPri6           = 69;
  static constexpr int FramesRcvdPri7           = 70;
  static constexpr int XmitPri0Pause1US         = 71;
  static constexpr int XmitPri1Pause1US         = 72;
  static constexpr int XmitPri2Pause1US         = 73;
  static constexpr int XmitPri3Pause1US         = 74;
  static constexpr int XmitPri4Pause1US         = 75;
  static constexpr int XmitPri5Pause1US         = 76;
  static constexpr int XmitPri6Pause1US         = 77;
  static constexpr int XmitPri7Pause1US         = 78;
  static constexpr int RcvdPri0Pause1US         = 79;
  static constexpr int RcvdPri1Pause1US         = 80;
  static constexpr int RcvdPri2Pause1US         = 81;
  static constexpr int RcvdPri3Pause1US         = 82;
  static constexpr int RcvdPri4Pause1US         = 83;
  static constexpr int RcvdPri5Pause1US         = 84;
  static constexpr int RcvdPri6Pause1US         = 85;
  static constexpr int RcvdPri7Pause1US         = 86;
  static constexpr int RcvdStdPause1US          = 87;
  static constexpr int FramesRcvdTrunc          = 88;
  static constexpr int MaxCounter               = 88; // Same as preceding entry!
};



class VmacC3 : public model_core::RegisterBlock<RegisterCallback>, public Mac {

  static constexpr int      kMaxMapEntries         = VmacC3Cntr::MaxCounter+1;
  static constexpr int      kNumChans              = 4;
  static constexpr int      kAddrSpaceWordSize     = 4;
  static constexpr uint32_t kAddrSpaceStrideBytes  = RegisterUtils::vmac_c3_stride;
  static constexpr uint32_t kCounterSpaceOffset    = RegisterUtils::vmac_c3_cntr_offset;
  static constexpr uint32_t kCounterSpaceSizeBytes = RegisterUtils::vmac_c3_cntr_size;

  static uint32_t counter_base_addr(uint32_t base) { return base + kCounterSpaceOffset; }

  // These are CSR word offsets - need to take byte address and divide by 4 to derive
  static constexpr uint32_t kOffsetBase            = 0x0u;
  static constexpr uint32_t kOffsetSwReset         = kOffsetBase + 0u;
  static constexpr uint32_t kOffsetRdCtrl          = kOffsetBase + 1u;
  static constexpr uint32_t kOffsetRData0          = kOffsetBase + 2u;
  static constexpr uint32_t kOffsetRData1          = kOffsetBase + 3u;
  static constexpr uint32_t kOffsetRData2          = kOffsetBase + 4u;
  static constexpr uint32_t kOffsetRData3          = kOffsetBase + 5u;
  static constexpr uint32_t kOffsetStatsRst        = kOffsetBase + 6u;

  // Map defined in comira-umac3.cpp
  static const MacCounterMapperConfig kMacCounterMapperTab[kMaxMapEntries];

 public:
  VmacC3(int chipIndex, int macBlock, uint32_t baseAddr);
  virtual ~VmacC3();


  // Implement virtual funcs from Mac - all counter funcs
  uint64_t mac_counter_read(int chan, int umac3_index, bool clear) override;

  uint32_t mac_counter_base_addr() const override { return counter_base_addr(base_addr_); }
  uint32_t mac_stride()            const override { return kAddrSpaceStrideBytes; }
  int      mac_block()             const override { return mac_block_; }
  int      mac_type()              const override { return MacType::kVmacC3; }


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
  void do_read_reg(uint32_t offset, uint32_t *data) const;
  void do_write_reg(uint32_t offset, uint32_t data);
  void do_clear_all(uint32_t chans);


 private:
  uint32_t                                  base_addr_;
  int                                       mac_block_;
  uint16_t                                  sw_reset_;
  uint16_t                                  rd_ctrl_;
  uint16_t                                  stats_rst_;
  uint64_t                                  data_;
  std::unordered_map< uint16_t, uint16_t >  other_reg_vals_;

};

}

#endif // _SHARED_COMIRA_UMAC3_H_
