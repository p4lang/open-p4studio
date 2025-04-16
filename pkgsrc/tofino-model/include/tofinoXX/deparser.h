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

#ifndef _TOFINOXX_DEPARSER_
#define _TOFINOXX_DEPARSER_

#include <string>
#include <cstdint>
#include <list>
#include <set>
#include <vector>
#include <bitvector.h>
#include <deparser-metadata.h>
#include <phv.h>
#include <pipe-object.h>

//OD #include <tofino/register_includes/dprsr_reg_rspec_mutable.h>

namespace MODEL_CHIP_NAMESPACE {

  class DeparserReg;
  class Packet;
  class PacketBuffer;

  class Deparser : public PipeObject {
 public:
    Deparser(RmtObjectManager *om, int pipeIndex, int ioIndex, bool egress, DeparserReg &deparser_reg);
    virtual ~Deparser();

    static constexpr int kNumFieldDictionaryEntries = 192;

    static constexpr int kNum8BitPHVEntries = 64;
    static constexpr int kNum16BitPHVEntries = 96;
    static constexpr int kNum32BitPHVEntries = 64;

    static constexpr int kNum8BitTagalongPHVEntries = 32;
    static constexpr int kNum16BitTagalongPHVEntries = 48;
    static constexpr int kNum32BitTagalongPHVEntries = 32;

    // Constants for checksum engine.
    static constexpr int kNumChecksumEngines = 12;
    static constexpr int kNumTagalongPHVChecksumCfgEntries =
                           kNum8BitTagalongPHVEntries +
                           kNum16BitTagalongPHVEntries +
                           (kNum32BitTagalongPHVEntries * 2) +
                           kNumChecksumEngines;
    static constexpr int kNumPHVChecksumCfgEntries =
                           kNum8BitPHVEntries + kNum16BitPHVEntries +
                           (kNum32BitPHVEntries * 2);
    typedef std::array<std::tuple<uint16_t, bool>,
                       kNumChecksumEngines> Checksums;


    static constexpr int kNumMirrorTableEntries = 8;
    static constexpr int kNumMirrorMetadataBytes = 48;

    // Currently not used in Tofino deparser
    static bool kRelaxDeparserChunkChecks;     // Defined in rmt-config.cpp
    static bool kRelaxDeparserByteChecks;      // Defined in rmt-config.cpp
    static bool kRelaxDeparserGroupChecks;     // Defined in rmt-config.cpp
    static bool kRelaxDeparserFdeChecks;       // Defined in rmt-config.cpp
    static bool kRelaxDeparserClotChecks;      // Defined in rmt-config.cpp
    static bool kRelaxDeparserClotLenChecks;   // Defined in rmt-config.cpp

    inline int   io_index()     const { return io_index_; }

    inline void print() {
    }

    /*
     * The drop_packet_flag tells the caller if the packet must be dropped.
     */
    std::vector<uint8_t>
    GenerateNewHeader(const Phv &phv, const BitVector<256> &pov,
                      const Checksums &checksum,
                      const uint8_t &version, bool *drop_packet_flag);

    bool is_pov_valid(const BitVector<256> &pov, uint8_t pov_position);
    void SwapBytes(uint16_t *phv_word);
    uint64_t RemapLogicalToPhy(uint64_t value, bool ingress);


 private:
    int                io_index_;
    bool               egress_ = false;

 protected:
    DeparserReg        &deparser_reg_;

    Packet *GetNewPacket(const Phv &phv, Packet *pkt);

    Packet *GetMirrorPacket(const Phv &phv, const Packet &packet);

    PacketBuffer*
    GetMetadataHeader(const Phv &phv, const std::vector<uint8_t> &phv_idx_list);

    template<int METADATA_WIDTH>
    std::tuple<bool, uint64_t> ExtractMetadata(const Phv &phv, DeparserMetadataInfo<METADATA_WIDTH> info) {

      static_assert(METADATA_WIDTH <= 64, "Metadata width exceeds 64b");

      bool valid = false;
      uint64_t value = 0;
      auto metadata_value = info.default_value_;

      if (info.valid_ && phv.is_valid_d(info.phv_idx_)) {
        valid = true;
        value = GetMetadataFromPhv(phv, info.phv_idx_, info.shift_,
                                   info.shift_valid_, METADATA_WIDTH);
      }
      else if (info.default_valid_) {
        valid = true;
        value = metadata_value.data.get_word(0);
      }

      return std::make_tuple(valid, value);
    }

    // Ingress and egress deparsers need to look at different version field in
    // the packet.
    virtual uint8_t GetPktVersion(Packet *pkt) = 0;

    virtual bool
    CheckDeparserPhvGroupConfig(const int &phv_idx) = 0;

    // This function returns the PHV indices which cannot appear in the field
    // dictionary for this thread.
    virtual std::set<int> GetInvalidPhvs() = 0;
 private:
    BitVector<256> ExtractPov(const Phv &phv);

    uint64_t
    GetMetadataFromPhv(const Phv &phv, const uint8_t &phv_idx,
                       const uint8_t &shift, const bool &shift_valid,
                       const int &metadata_width) const;

    Checksums GenerateChecksum(const Phv &phv);

    uint16_t
    GetPhvChecksum(const Phv &phv, const int &checksum_engine_idx);
    std::tuple<uint16_t, bool>
    GetChecksum(const Phv &phv,
                const std::array<uint16_t, kNumChecksumEngines> &phv_checksums,
                const int &checksum_engine_idx);

    uint16_t ApplyChecksumRowEntry(const int &index,
                                   const int &phv_word_width,
                                   const bool &is_tagalong_phv,
                                   const int &checksum_engine_idx,
                                   const uint32_t phv_word);

    // This function checks if the FDE version bits match the packet version.
    bool FdeVersionMatch(const int &fde_idx, const uint8_t &version);

    // This function implementes stage 1 of FDE compression algorithm. It
    // returns a list of FDE indices that contain PHV indices of [start_byte,
    // end_byte) bytes of deparsed header.
    // An FDE may straddle the start byte. For example, FDE 80 may contain 4
    // bytes which appear at bytes (start_byte - 2) to (start_byte + 1). For
    // such cases, real_start_byte is set to (start_byte - 2). real_start_byte
    // indicates the position in the header of the first byte of the first FDE
    // index in the returned vector.
    std::vector<int> Get18QfdeIndices(const int &start_byte,
                                      const int &end_byte,
                                      const BitVector<256> &pov,
                                      const uint8_t &version,
                                      int *real_start_byte);

    // This function implementes stage 2 of FDE compression algorithm. It
    // returns a list of valid PHV indices.
    std::vector<uint16_t> GetValidPhvIndices(const std::vector<int> &fde_indices);

    std::list<uint8_t>
    GetHeaderBytes(const Phv &phv,
                   const Checksums &checksum,
                   const std::vector<uint16_t> &phv_indices,
                   uint16_t * const prev_phv_idx,
                   int * const header_byte_idx);
    template<class T>
    void
    AdjustPhvOffset(const Phv &phv,
                    T phv_idx_begin, T phv_idx_end,
                    int * const phv_offset, uint16_t * const prev_phv_idx);
  };
}
#endif // _TOFINOXX_DEPARSER_
