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

#ifndef _JBAY_SHARED_DEPARSER_
#define _JBAY_SHARED_DEPARSER_

#include <string>
#include <cstdint>
#include <list>
#include <set>
#include <vector>
#include <bitvector.h>
#include <phv.h>
#include <pipe-object.h>
#include <deparser-invert-csum-ctl.h>

#include <register_includes/dprsr_csum_row_entry_array.h>
#include <register_includes/dprsr_csum_pov_one_engine.h>

namespace MODEL_CHIP_NAMESPACE {

  class DeparserReg;
  class Packet;
  class PacketBuffer;

  class Deparser : public PipeObject {
 public:
    Deparser(RmtObjectManager *om, int pipeIndex, int ioIndex, bool egress, DeparserReg &deparser_reg);
    virtual ~Deparser();

    static constexpr int kNumFieldDictionaryEntries = 128;
    static constexpr int kPovWidth      = 128;
    static constexpr int kWords         = 16;
    static constexpr int kChunksPerWord = 8;
    static constexpr int kBytesPerChunk = 8;
    static constexpr int kSlices        = 4;
    static_assert( kPovWidth == (kWords * kChunksPerWord) , "pov width must be same as number of chunks");

    static constexpr int kNum8BitPHVEntries = 64;
    static constexpr int kNum16BitPHVEntries = 96;
    static constexpr int kNum32BitPHVEntries = 64;

    static bool kRelaxDeparserChunkChecks;     // Defined in rmt-config.cpp
    static bool kRelaxDeparserByteChecks;      // Defined in rmt-config.cpp
    static bool kRelaxDeparserGroupChecks;     // Defined in rmt-config.cpp
    static bool kRelaxDeparserFdeChecks;       // Defined in rmt-config.cpp
    static bool kRelaxDeparserClotChecks;      // Defined in rmt-config.cpp
    static bool kRelaxDeparserClotLenChecks;   // Defined in rmt-config.cpp

    struct FdChunk {
      bool valid     = false;
      int  pov       = 0;
      bool seg_vld   = false;
      int  seg_sel   = 0;
      int  seg_slice = 0;
      int  len       = 0; // only valid if seg_vld == 0
      bool is_phv[8] {};
      int  byte_off[8] {};
      int  index     = 0;
    };


    // Constants for checksum engine.
    static constexpr int kNumChecksumEngines = 8;
    static constexpr int kNumPHVChecksumCfgEntries =
                           kNum8BitPHVEntries + kNum16BitPHVEntries +
                           (kNum32BitPHVEntries * 2);
    typedef std::array<uint16_t,kNumChecksumEngines> Checksums;

    static constexpr int kNumConstants = RmtDefs::kDeparserNumConstants;

    static constexpr int kNumMirrorTableEntries = 8;
    static constexpr int kNumMirrorMetadataBytes = 48;

    inline int  io_index()     const { return io_index_; }

    inline void print() { }

    void CheckHeaderByteChunks(std::vector<int>& header_byte_chunks) const;
    /*
     * The drop_packet_flag tells the caller if the packet must be dropped.
     */
    std::vector<uint8_t>
    GenerateNewHeader(const Phv &phv,Packet *pkt, int slice,
                      const BitVector<kPovWidth>& pov,
                      const Checksums &checksum,
                      const uint8_t &version, bool *drop_packet_flag);

    bool is_pov_valid(const BitVector<kPovWidth>& pov, uint8_t pov_position);
    void SwapBytes(uint16_t *phv_word);
    uint64_t RemapLogicalToPhy(uint64_t value, bool ingress);
    inline const char *gress_str() const { return egress_ ? "egress" : "ingress"; }

    // New JBayB0/WIP funcs to allow checksums to be inverted
    bool get_csum_invert_clot(int csum_eng, int clot_num) const {
      return invert_csum_ctl_.get_csum_invert_clot(csum_eng, clot_num);
    }
    bool get_csum_invert_phv(int csum_eng, int phv_num) const {
      return invert_csum_ctl_.get_csum_invert_phv(csum_eng, phv_num);
    }

 private:
    int                   io_index_;
    const bool            egress_;
    DeparserInvertCsumCtl invert_csum_ctl_;

 protected:
    DeparserReg        &deparser_reg_;

    Packet *GetNewPacket(const Phv &phv, Packet *pkt, const BitVector<kPovWidth>& pov);

    Packet *GetMirrorPacket(const Phv &phv, const Packet &packet,const BitVector<kPovWidth>& pov);

    PacketBuffer* GetMetadataHeader(const Phv &phv, const std::vector<uint8_t> &phv_idx_list);

    BitVector<kPovWidth> ExtractPov(const Phv &phv);

    // Ingress and egress deparsers need to look at different version field in
    // the packet.
    virtual uint8_t GetPktVersion(Packet *pkt) = 0;

    int GetPortNumber(const Packet* pkt);

    int GetSliceNumber(const Packet* pkt) {
      return RmtDefs::get_deparser_slice( GetPortNumber(pkt) );
    }
    int GetChannelWithinSlice(const Packet* pkt) {
      return RmtDefs::get_deparser_channel_within_slice( GetPortNumber(pkt) );
    }

    virtual bool CheckDeparserPhvGroupConfig(const int &phv_idx,int slice) = 0;

    bool MaybeTruncate(Packet *pkt, bool ingress);
    bool ZeroiseCRC(Packet *pkt);
    bool MaybePad(Packet *pkt, bool ingress);


 private:
    Checksums GenerateChecksum(const Phv &phv,const Packet* pkt, const BitVector<kPovWidth>& pov,
                               int slice);

    uint16_t GetPhvChecksum(const Phv &phv, const BitVector<kPovWidth>& pov, const int &checksum_engine_idx);
    uint16_t CombineChecksums( int slice,
                               const Phv &phv,const Packet* pkt, const BitVector<kPovWidth>& pov,
                               const Checksums &phv_checksums,
                               const int &checksum_engine_idx);
    uint16_t ApplyChecksumRowEntry(int engine,
                                   const BitVector<kPovWidth>& pov,
                                   const int &csum_idx,
                                   const int &phv_word_width,
                                   const uint32_t phv_word);

    // This function checks if the FDE version bits match the packet version.
    bool FdeVersionMatch(const int &fde_idx, const uint8_t &version);

  };
}
#endif // _JBAY_SHARED_DEPARSER_
