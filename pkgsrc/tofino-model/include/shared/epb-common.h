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

#ifndef _SHARED_EPB_COMMON_
#define _SHARED_EPB_COMMON_

#include <string>
#include <cstdint>
#include <vector>
#include <pipe-object.h>
#include <shared/packet.h>


namespace MODEL_CHIP_NAMESPACE {

  class EpbCommon : public PipeObject {

 public:
    static constexpr int      kType = RmtTypes::kRmtTypeEpb;

    static constexpr int      kChannelsPerEpb             = RmtDefs::kChannelsPerEpb;
    static constexpr int      kMetadataMaxHdrBytes        = 64;
    static constexpr uint16_t kFlagIngressQueueDepth      = 1 << 0;
    static constexpr uint16_t kFlagIngressCongestion      = 1 << 1;
    static constexpr uint16_t kFlagIngressQueueTimestamp  = 1 << 2;
    static constexpr uint16_t kFlagEgressQueueDepth       = 1 << 3;
    static constexpr uint16_t kFlagEgressCongestion       = 1 << 4;
    static constexpr uint16_t kFlagAppPoolCongestion      = 1 << 5;
    static constexpr uint16_t kFlagQueueDelay             = 1 << 6;
    static constexpr uint16_t kFlagMulticastReplicationID = 1 << 7;
    static constexpr uint16_t kFlagReplicationIDFirst     = 1 << 8;
    static constexpr uint16_t kFlagQID                    = 1 << 9;
    static constexpr uint16_t kFlageCos                   = 1 << 10;
    static constexpr uint16_t kFlagRedirOrRecirc          = 1 << 11;
    static constexpr uint16_t kFlagLength                 = 1 << 12;
    static constexpr uint16_t kFlagTBD1                   = 1 << 13;
    static constexpr uint16_t kFlagTBD2                   = 1 << 14;
    static constexpr uint16_t kFlagTBD3                   = 1 << 15;

    EpbCommon(RmtObjectManager *om, int pipeIndex, int parseIndex);
    virtual ~EpbCommon();

    // CHIP-SPECIFIC funcs

    /* Remap physical port to logical port */
    virtual uint16_t RemapPhyToLogical(uint16_t phy_port, int chan) = 0;
    // Some convenience funcs in case callers want a DIY header
    virtual bool is_chan_valid(int chan) = 0;
    virtual bool is_chan_enabled(int chan) = 0;
    virtual void set_chan_enabled(int chan, bool enabled) = 0;
    virtual uint16_t get_ctrl_flags(int chan) = 0;
    virtual void set_ctrl_flags(int chan, uint16_t flags) = 0;
    virtual uint32_t get_global_version(int chan) = 0;
    virtual bool is_ctrl_flag_set(int chan, uint16_t flag) = 0;
    virtual uint16_t get_packet_len(Packet *pkt) = 0;
    // Maybe add padding (only JBay for now)
    virtual uint16_t get_padding(int metadata_size) { return 0; }
    virtual uint16_t get_max_byte() = 0;


    // Fill a uint8_t buffer at pos with size bytes from arbitrary type val
    template <typename T> int fill_hdr(uint8_t *hdr, int pos, unsigned int siz, T val,
                                       bool little_endian=false) {
      static_assert(std::is_integral<T>::value, "only integral types allowed" );
      static_assert(!std::is_same< T, bool >::value, "bool not allowed");
      RMT_ASSERT ((siz <= sizeof(T)) && (pos >= 0) && (pos + siz <= kMetadataMaxHdrBytes));
      for (unsigned int i = 0; i < siz; i++) {
        int i2 = (little_endian) ?i :(siz-1-i);
        *(hdr + pos + i) = static_cast<uint8_t>((val >> (i2*8)) & static_cast<T>(0xFF));
      }
      return pos+siz;
    }


    // GENERIC funcs (make virtual in case we ever need to override PER-CHIP)

    virtual uint64_t get_global_timestamp(int chan=0);

    // Tack a metadata hdr onto front of packet using enables defined
    // in chnl_ctrl reg and using data associated with packet
    //
    // NB. system timestamp is used for EPBGlobalTimestamp
    //     glb_ver_ register is used for GlobalVersion
    //
    virtual Packet *prepend_metadata_hdr(Packet *packet, int chan=-1);
    virtual Packet *add_metadata_hdr(Packet *packet, int chan=-1);


 private:

  };
}

#endif // _SHARED_EPB_COMMON_
