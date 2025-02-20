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

#ifndef _SHARED_IPB_COMMON_H_
#define _SHARED_IPB_COMMON_H_

#include <string>
#include <cstdint>
#include <vector>
#include <common/rmt-assert.h>
#include <rmt-defs.h>
#include <pipe-object.h>
#include <packet.h>

#include <register_includes/chnl_deparser_drop_pkt_mutable.h>
#include <register_includes/chnl_wsch_discard_pkt.h>
#include <register_includes/chnl_wsch_trunc_pkt.h>
#include <register_includes/chnl_parser_discard_pkt.h>
#include <register_includes/chnl_parser_send_pkt_mutable.h>
#include <register_includes/chnl_deparser_send_pkt_mutable.h>
#include <register_includes/chnl_macs_received_pkt_mutable.h>

namespace MODEL_CHIP_NAMESPACE {


class IpbCountersCommon {
 public:
  // constructor
  IpbCountersCommon(int chipIndex, int pipeIndex, int ipbIndex, int chanIndex);
  // reset all counters
  virtual void reset();

  // contained counters are public to allow tests to access them
  register_classes::ChnlDeparserDropPktMutable  chnl_deparser_drop_pkt_;
  register_classes::ChnlWschDiscardPkt          chnl_wsch_discard_pkt_;
  register_classes::ChnlWschTruncPkt            chnl_wsch_trunc_pkt_;
  register_classes::ChnlParserDiscardPkt        chnl_parser_discard_pkt_;
  register_classes::ChnlParserSendPktMutable    chnl_parser_send_pkt_;
  register_classes::ChnlDeparserSendPktMutable  chnl_deparser_send_pkt_;
  register_classes::ChnlMacsReceivedPktMutable  chnl_macs_received_pkt_;

  virtual void increment_chnl_deparser_drop_pkt();
  virtual void increment_chnl_deparser_send_pkt();
  virtual void increment_chnl_parser_send_pkt();
  virtual void increment_chnl_parser_send_pkt_err();
  virtual void increment_chnl_resubmit_received_pkt() = 0;
  virtual void increment_chnl_macs_received_pkt();

 protected:
  int wrap_counter_size_;
};

class IpbCommon : public PipeObject {

  public:
    static constexpr int      kType = RmtTypes::kRmtTypeIpb;

    static constexpr int      kChannelsPerIpb = RmtDefs::kChannelsPerIpb;
    static constexpr int      kHeaderSizeBytes = RmtDefs::kIpbHeaderSizeBytes;
    static constexpr int      kMeta0SizeBytes = RmtDefs::kIpbMeta0SizeBytes;
    static constexpr int      kMeta1SizeBytes = RmtDefs::kIpbMeta1SizeBytes;
    static constexpr int      kMeta1ResubSizeBytes = RmtDefs::kIpbMeta1ResubSizeBytes;
    static constexpr int      kMeta0Meta1SizeBytes = kMeta0SizeBytes + kMeta1SizeBytes;
    static constexpr int      kHeaderWordSizeBytes = sizeof(uint64_t);
    static constexpr int      kHeaderWordSizeBits = kHeaderWordSizeBytes * 8;
    static_assert( (kHeaderWordSizeBytes == 8),
                   "IpbCommon code assumes kHeaderWordSize is 8");
    static_assert( ((kHeaderSizeBytes % kHeaderWordSizeBytes) == 0),
                   "IpbCommon assumes IPB Header is multiple of kHeaderWordSizeBytes(8B)");
    static_assert( ((kMeta0SizeBytes % kHeaderWordSizeBytes) == 0),
                   "IpbCommon assumes Meta0 is multiple of kHeaderWordSizeBytes(8B)");
    static_assert( ((kMeta1SizeBytes % kHeaderWordSizeBytes) == 0),
                   "IpbCommon assumes Meta1 is multiple of kHeaderWordSizeBytes(8B)");
    static_assert( (kMeta1ResubSizeBytes <= kMeta1SizeBytes),
                   "IPB Meta1 resub size must be <= IPB Meta1 size");
    static_assert( (kMeta0Meta1SizeBytes <= kHeaderSizeBytes),
                   "IPB Meta0 + IPB Meta1 size must be <= IPB Header size");
    static constexpr int      kMeta0SizeWords = kMeta0SizeBytes / kHeaderWordSizeBytes;
    static constexpr int      kMeta1SizeWords = kMeta1SizeBytes / kHeaderWordSizeBytes;
    static constexpr int      kMeta1ResubSizeWords = kMeta1ResubSizeBytes / kHeaderWordSizeBytes;

    // Some flags to control format of IPB header
    static constexpr uint32_t kFlagsPktHasLeadingPadZero8B   = 0x00000001;
    static constexpr uint32_t kFlagsPktHasMeta0              = 0x00000002;
    static constexpr uint32_t kFlagsPktHasMeta1              = 0x00000004;
    static constexpr uint32_t kFlagsPktHasTrailingPadZero8B  = 0x00000008;
    static constexpr uint32_t kFlagsAddLeadingPadZero8B      = 0x00000010;
    static constexpr uint32_t kFlagsMeta1IsByteSwapped       = 0x00000100;

    // Meta0 defns
    static constexpr int kMeta0TimestampWidth    = 48;
    static constexpr int kMeta0LogicalPortWidth  = RmtDefs::kIpbMeta0LogicalPortWidth; // WIP 11 else 9
    static constexpr int kMeta0PadAWidth         = 12 - kMeta0LogicalPortWidth;
    static constexpr int kMeta0VersionWidth      =  2;
    static constexpr int kMeta0PadBWidth         =  1;
    static constexpr int kMeta0ResubmitFlagWidth =  1;
    static constexpr int kMeta0HighFieldWidth    = kMeta0ResubmitFlagWidth +
        kMeta0PadBWidth + kMeta0VersionWidth + kMeta0PadAWidth + kMeta0LogicalPortWidth;
    static constexpr int kMeta0TotalWidth = kMeta0HighFieldWidth + kMeta0TimestampWidth;
    static_assert( (kMeta0HighFieldWidth == 16), "Bad width IPB Resub/Ver/Port fields" );
    static_assert( (kMeta0TotalWidth == 64), "Bad width IPB Meta0 fields" );

    static constexpr uint64_t kAllOnes                = UINT64_C(0xFFFFFFFFFFFFFFFF);
    static constexpr uint64_t kMeta0TimestampMask     = kAllOnes >> (64-kMeta0TimestampWidth);
    static constexpr uint64_t kMeta0LogicalPortMask   = kAllOnes >> (64-kMeta0LogicalPortWidth);
    static constexpr uint64_t kMeta0VersionMask       = kAllOnes >> (64-kMeta0VersionWidth);
    static constexpr uint64_t kMeta0ResubmitFlagMask  = kAllOnes >> (64-kMeta0ResubmitFlagWidth);

    static constexpr int kMeta0TimestampShift    = 0;
    static constexpr int kMeta0LogicalPortShift  = kMeta0TimestampShift + kMeta0TimestampWidth;
    static constexpr int kMeta0PadAShift         = kMeta0LogicalPortShift + kMeta0LogicalPortWidth;
    static constexpr int kMeta0VersionShift      = kMeta0PadAShift + kMeta0PadAWidth;
    static constexpr int kMeta0PadBShift         = kMeta0VersionShift + kMeta0VersionWidth;
    static constexpr int kMeta0ResubmitFlagShift = kMeta0PadBShift + kMeta0PadBWidth;

    static inline uint8_t meta0_get_resubmit_flag(uint64_t meta0) {
      return static_cast<uint8_t>((meta0 >> kMeta0ResubmitFlagShift) & kMeta0ResubmitFlagMask);
    }
    static inline uint8_t meta0_get_version(uint64_t meta0) {
      return static_cast<uint8_t>((meta0 >> kMeta0VersionShift) & kMeta0VersionMask);
    }
    static inline uint16_t meta0_get_logical_port(uint64_t meta0) {
      return static_cast<uint16_t>((meta0 >> kMeta0LogicalPortShift) & kMeta0LogicalPortMask);
    }
    static inline uint64_t meta0_get_timestamp(uint64_t meta0) {
      return ((meta0 >> kMeta0TimestampShift) & kMeta0TimestampMask);
    }
    static inline uint64_t meta0_make(uint8_t resubmit_flag, uint8_t version,
                                      uint16_t logical_port, uint64_t timestamp) {
      uint64_t meta0 = UINT64_C(0);
      meta0 |= (resubmit_flag & kMeta0ResubmitFlagMask) << kMeta0ResubmitFlagShift;
      meta0 |= (version & kMeta0VersionMask) << kMeta0VersionShift;
      meta0 |= (logical_port & kMeta0LogicalPortMask) << kMeta0LogicalPortShift;
      meta0 |= (timestamp & kMeta0TimestampMask) << kMeta0TimestampShift;
      return meta0;
    }


    IpbCommon(RmtObjectManager *om, int pipeIndex, int ipbIndex);
    virtual ~IpbCommon();


    inline bool is_meta_enabled()         { return meta_enabled_; }
    inline bool is_rx_enabled()           { return rx_enabled_; }
    inline void set_meta_enabled(bool tf) { override_ = true; meta_enabled_ = tf; }
    inline void set_rx_enabled(bool tf)   { override_ = true; rx_enabled_ = tf; }
    inline void set_dflt_enabled()        { override_ = false; }
    inline bool is_chan_valid(int chan)   {
      return ((chan >= 0) && (chan < kChannelsPerIpb));
    }
    inline bool is_chan_meta_enabled(int chan) {
      // Determined by IPB register config unless overriden in s/w
      return (override_) ?is_meta_enabled() :is_chan_valid(chan) && is_chan_enabled(chan);
    }
    inline bool is_chan_rx_enabled(int chan) {
      // Determined by IPB register config unless overriden in s/w
      return (override_) ?is_rx_enabled() :is_chan_valid(chan) && is_chan_enabled(chan);
    }

    uint64_t get_global_timestamp(Packet *packet, int chan);

    Packet *prepend_metadata_hdr(Packet *packet, int chan, uint32_t flags,
                                 uint8_t resubmit_flag, uint8_t version,
                                 uint16_t logical_port, uint64_t timestamp,
                                 uint64_t meta1[]);
    Packet *prepend_metadata_hdr(Packet *packet, int chan, uint32_t flags);

    // Channel(group) is used when 100G port is split into 4x10G or 2x40G etc
    void insert_metadata(Packet *pkt, uint32_t channel,
                         bool resubmit=false, bool gen=false, bool recirc=false);


    // Generic funcs to extract metadata from Registers,
    // Packets, Packet objects and ResubmitHeaders
    void get_register_metadata(Packet *packet, int chan, uint32_t flags,
                               uint8_t *resubmit_flag, uint8_t *version,
                               uint16_t *logical_port, uint64_t *timestamp,
                               uint64_t meta1[]);
    void get_packet(Packet *packet, int chan, uint32_t flags,
                    uint8_t *resubmit_flag, uint8_t *version,
                    uint16_t *logical_port, uint64_t *timestamp,
                    uint64_t meta1[], bool trim);
    void get_packet_metadata(Packet *packet, int chan, uint32_t flags,
                             uint8_t *resubmit_flag, uint8_t *version,
                             uint16_t *logical_port, uint64_t *timestamp);
    void get_resubmit_metadata(Packet *packet, int chan, uint32_t flags,
                               uint64_t meta1[]);

    // Virtual funcs to handle chip-specific ways of getting metadata from registers
    virtual bool     overwrite_meta1(Packet *packet, int chan) = 0;
    virtual bool     is_chan_enabled(int chan) = 0;
    virtual uint8_t  get_version(Packet *packet, int chan) = 0;
    virtual uint16_t get_logical_port(Packet *packet, int chan) = 0;
    virtual uint64_t get_meta1(Packet *packet, int chan, int word) = 0;
    virtual uint32_t get_packet_flags(Packet *packet) = 0;
    virtual uint16_t get_max_byte(Packet *packet) = 0;

    virtual IpbCountersCommon* get_ipb_counters(int ipbChan) = 0;

  private:
    bool  override_;
    bool  meta_enabled_;
    bool  rx_enabled_;

};
}
#endif // _SHARED_IPB_COMMON_H_
