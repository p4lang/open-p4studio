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

#include <string>
#include <sstream>
#include <algorithm>
#include <tuple>

#include <common/rmt-assert.h>
#include <deparser.h>
#include <deparser-reg.h>
#include <packet.h>
#include <port.h>
#include <rmt-log.h>
#include <rmt-object-manager.h>

namespace MODEL_CHIP_NAMESPACE {
  Deparser::Deparser(RmtObjectManager *om, int pipeIndex, int ioIndex,
                     bool egress, DeparserReg &deparser_reg)
      : PipeObject(om,pipeIndex,0x3F,RmtTypes::kRmtTypeDeparser), io_index_(ioIndex), egress_(egress),
      deparser_reg_(deparser_reg)
  {
    RMT_LOG_VERBOSE("DEPARSER::create pipe_index:%d ioIndex:%d\n",
                    pipe_index(), io_index_);
  }

  Deparser::~Deparser() {
    RMT_LOG_VERBOSE("DEPARSER::delete pipe_index:%d io_index:%d\n",
                    pipe_index(), io_index_);
  }

  Packet *
  Deparser::GetNewPacket(const Phv &phv, Packet *pkt) {

    if (!enabled()) return NULL;

    // Extract POV from incoming PHV
    const BitVector<256> pov = ExtractPov(phv);

    // Generate checksum
    auto checksum = GenerateChecksum(phv);

    // Generate new header bytes
    bool drop_packet_flag = false;
    auto new_header_bytes = GenerateNewHeader(phv, pov, checksum,
                                              GetPktVersion(pkt),
                                              &drop_packet_flag);

    RMT_LOG_VERBOSE("DEPARSER::%s", __func__);
    for (uint32_t i=0; i<new_header_bytes.size(); i++) {
      RMT_LOG_VERBOSE(" %02x",new_header_bytes[i]);
    }
    RMT_LOG_VERBOSE("\n");

    if (drop_packet_flag) {
      RMT_LOG_VERBOSE("DEPARSER::%s: Dropping packet", __FUNCTION__);
      get_object_manager()->pkt_delete(pkt);
      pkt = nullptr;
    }
    else {
      // Create packet buffer with new header
      PacketBuffer *new_header = get_object_manager()->pktbuf_create(
                                                         &new_header_bytes[0],
                                                         new_header_bytes.size());
      // Update packet with new header
      pkt->prepend(new_header, pkt->orig_hdr_len());
    }

    return pkt;
  }

  bool
  Deparser::FdeVersionMatch(const int &fde_idx, const uint8_t &version) {
    const uint32_t fde_version = (deparser_reg_.get_fde_version(egress_, fde_idx) & 0x0F);
    switch (fde_version) {
      case 0b1100: return (version == 0b00);
      case 0b1001: return (version == 0b01);
      case 0b0110: return (version == 0b10);
      case 0b0011: return (version == 0b11);
      case 0b1110: return ((version & 0b01) == 0b00);
      case 0b1011: return ((version & 0b01) == 0b01);
      case 0b1101: return ((version & 0b10) == 0b00);
      case 0b0111: return ((version & 0b10) == 0b10);
      case 0b1111: return true;
      default : return false;
    }
  }

  std::vector<int>
  Deparser::Get18QfdeIndices(const int &start_byte,
                             const int &end_byte,
                             const BitVector<256> &pov,
                             const uint8_t &version,
                             int *real_start_byte) {
    RMT_ASSERT(real_start_byte != nullptr);
    (*real_start_byte) = start_byte;
    std::vector<int> fde_indices;
    int current_byte = 0;

    // Iterate through all field dictionary entries.
    for (int fde_idx = 0; fde_idx < kNumFieldDictionaryEntries; ++fde_idx) {
      // If POV bit says this field dictionary entry is valid.
      if (deparser_reg_.get_fde_valid(egress_, fde_idx) &&
          is_pov_valid(pov, deparser_reg_.get_fde_pov_sel(egress_, fde_idx)) &&
          FdeVersionMatch(fde_idx, version)) {
        bool is_consistent;
        auto num_fde_bytes = deparser_reg_.get_fde_num_bytes(egress_, fde_idx,
                                                             &is_consistent);
        if (!is_consistent) {
          RMT_LOG_WARN("POV/PHV num_bytes is different for %d", fde_idx);
        }
        if ((current_byte + num_fde_bytes) > start_byte && current_byte < end_byte) {
          fde_indices.push_back(fde_idx);
          // The if-statement below checks if this is the first FDE index.
          if (current_byte <= start_byte) {
            RMT_ASSERT(fde_indices.size() == 1);
            (*real_start_byte) = current_byte;
          }
        }

        // Always update current_byte.
        current_byte += num_fde_bytes;
      }
    }

    RMT_ASSERT(fde_indices.size() <= (18 * 4));
    RMT_ASSERT((*real_start_byte) >= start_byte - 3);
    RMT_ASSERT((*real_start_byte) <= start_byte);
    return fde_indices;
  }

  std::vector<uint16_t>
  Deparser::GetValidPhvIndices(const std::vector<int> &fde_indices) {
    std::vector<uint16_t> phv_indices;
    uint32_t next_fde_num = 0;
    int next_byte_num = 0;

    for (int qfde_num = 0; (qfde_num < 16) && (next_fde_num < fde_indices.size()); ++qfde_num) {
      const uint32_t min_fde_num = qfde_num * 4;
      const uint32_t max_fde_num = min_fde_num + 16;
      if (min_fde_num > next_fde_num) {
        RMT_LOG_ERROR("Min FDE number %u exceeds %u in QFDE %d\n", min_fde_num,
                      next_fde_num, qfde_num);
        throw std::out_of_range(std::string("Overflow in QFDE ") +
          std::to_string(qfde_num));
      }

      for (int i = 0; (i < 16) && (next_fde_num < fde_indices.size()); ++i) {
        if (max_fde_num <= next_fde_num) {
          throw std::out_of_range(std::string("Underflow in QFDE ") +
                                  std::to_string(qfde_num) +
                                  std::string(" byte ") + std::to_string(i));
        }

        int fde_idx = fde_indices.at(next_fde_num);
        bool is_consistent;
        RMT_ASSERT(deparser_reg_.get_fde_valid(egress_, fde_idx));
        RMT_ASSERT(deparser_reg_.get_fde_num_bytes(egress_, fde_idx, &is_consistent) >= next_byte_num);
        auto phv_idx = deparser_reg_.get_fde_phv_pointer(egress_, fde_idx,
                                                         next_byte_num);
        const auto invalid_phvs = GetInvalidPhvs();
        if (invalid_phvs.find(phv_idx) != invalid_phvs.end()) {
          RMT_LOG_ERROR("Deparser::%s:: Accessing %d in %s",
                        __func__, phv_idx, egress_ ? "egress" : "ingress");
          RMT_ASSERT(false);
        }
        phv_indices.push_back(phv_idx);

        // Find the next valid header byte.
        ++next_byte_num;
        if (deparser_reg_.get_fde_num_bytes(egress_, fde_idx,
                                            &is_consistent) == next_byte_num) {
          ++next_fde_num;
          next_byte_num = 0;
        }
      }
    }

    RMT_ASSERT(phv_indices.size() <= 243);
    return phv_indices;
  }

  std::list<uint8_t>
  Deparser::GetHeaderBytes(const Phv &phv,
                           const Checksums &checksum,
                           const std::vector<uint16_t> &phv_indices,
                           uint16_t * const prev_phv_idx,
                           int * const header_byte_idx) {
    RMT_ASSERT (nullptr != header_byte_idx);
    std::list<uint8_t> header_bytes;

    for (auto phv_idx : phv_indices) {
      // header_byte_idx for multiple byte PHV words
      if ((*prev_phv_idx) == phv_idx) {
        if ( Phv::is_valid_phv_d(phv_idx) &&
            (phv.which_width_in_bytes_d(phv_idx) == 1)) {
          // For 8 bit PHV words we do not need to increment header_byte_idx
          //  because (a) there is only one byte (b) an 8 bit PHV can be deparsed
          //  consecutively, so we don't need header_byte_idx to spot consecutive
          //  word deparsing. So, do nothing.
        }
        else {
          // In all other cases increment the header byte count
          ++(*header_byte_idx);
        }
      } else {
        (*header_byte_idx) = 0;
      }

      uint32_t phv_word = 0x0;
      int phv_word_width = -1;

      if (phv_idx < Phv::phv_max_d()) {
        if (CheckDeparserPhvGroupConfig(phv_idx) == false) {
          RMT_LOG_ERROR("Deparser::%s::Deparser group check failed for %u\n",
                       __FUNCTION__, phv_idx);
          RMT_ASSERT(false);
        }
        phv_word = phv.get_ignore_valid_bit_d(phv_idx);
        phv_word_width = phv.which_width_in_bytes_d(phv_idx);
      }
      else if (phv_idx < (Phv::phv_max_d() + kNumChecksumEngines)) {
        uint32_t csum_idx = phv_idx - Phv::phv_max_d();
        bool is_valid = false;
        std::tie(phv_word, is_valid) = checksum[csum_idx];
        phv_word_width = 2;
        RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugDeparserCsumSummary),
                "DEPARSER::%s::checksum phv_idx=%03d phv_word=%08x\n",
                __func__, phv_idx, phv_word);
        RMT_ASSERT(is_valid);
      }
      else if (0 == phv.which_width_d(phv_idx)) {
        throw std::out_of_range(std::string("Invalid PHV index ") + std::to_string(phv_idx));
      }
      else {
        phv_word = phv.get_ignore_valid_bit_dx(phv_idx);
        phv_word_width = phv.which_width_in_bytes_dx(phv_idx);
        RMT_ASSERT(phv_idx <= (Phv::phv_max_extended_dx()-1));
        RMT_LOG_VERBOSE("DEPARSER::%s:: phv_idx=%03d orig_phv_word=%08x header_byte_idx=%01d phv_word=%08x\n",
                    __FUNCTION__, phv_idx,phv.get_dx(phv_idx),(*header_byte_idx),phv_word);
      }
      RMT_ASSERT(phv_word_width > 0);
      // We want to extract exactly 1 byte from PHV word. phv_word_width
      // contains the width of the data in the PHV word. byte_offset is the
      // offset (in bytes) of the byte that we want to extract from
      // phv_word. The least significant byte has offset 0.
      const int byte_offset = phv_word_width - 1 - (*header_byte_idx);
      RMT_ASSERT(byte_offset < phv_word_width);
      // This assert ensures that the same PHV word cannot appear at consecutive
      // locations in the deparsed header.
      if (byte_offset < 0) {
         std::ostringstream phv_idx_str;
         for (auto x : phv_indices) {
             if (phv_idx_str.str().length() > 0) phv_idx_str << " ";
             phv_idx_str << "PHV" << x;
         }
         RMT_LOG_ERROR("DEPARSER::%s:: Error extracting %s\n", __FUNCTION__,
                       phv_idx_str.str().c_str());
         RMT_LOG_ERROR("DEPARSER::%s:: Writing PHV%hu to packet header\n",
                       __FUNCTION__, phv_idx);
      }
      RMT_ASSERT(byte_offset >= 0);
      phv_word = (phv_word >> (byte_offset * 8)) & 0xFF;

      header_bytes.push_back(phv_word);

      (*prev_phv_idx) = phv_idx;
    }

    RMT_ASSERT(header_bytes.size() <= 243);
    return header_bytes;
  }

  template<class T>
  void
  Deparser::AdjustPhvOffset(const Phv &phv,
                            T phv_idx_begin, T phv_idx_end,
                            int * const phv_offset,
                            uint16_t * const prev_phv_idx) {
    for (auto i = phv_idx_begin; i != phv_idx_end; std::advance(i, 1)) {
      if ((*phv_offset) == 0) {
        (*prev_phv_idx) = 0xFFFF;
        if (std::next(i) != phv_idx_end) (*prev_phv_idx) = *std::next(i);
        (*phv_offset) = phv.which_width_in_bytes_d(*prev_phv_idx);
      }
      --(*phv_offset);
    }
  }

  std::vector<uint8_t>
  Deparser::GenerateNewHeader(const Phv &phv,
                              const BitVector<256> &pov,
                              const Checksums &checksum,
                              const uint8_t &version, bool *drop_packet_flag) {
    std::vector<uint8_t> header_bytes;
    decltype(header_bytes.size()) max_new_header_bytes_size(240);
    uint16_t prev_phv_idx = 0xFFFF;
    int header_byte_idx = 0;

    // Pass 1.
    {
      int real_start_byte;
      auto fde_indices = Get18QfdeIndices(0, 240, pov, version,
                                          &real_start_byte);
      RMT_ASSERT(real_start_byte == 0);

      auto phv_indices = GetValidPhvIndices(fde_indices);
      auto new_header_bytes = GetHeaderBytes(phv, checksum, phv_indices,
                                             &prev_phv_idx, &header_byte_idx);
      const auto offset = new_header_bytes.size() -
                            std::min(max_new_header_bytes_size,
                                     new_header_bytes.size());
      auto end = phv_indices.rbegin();
      std::advance(end, offset);
      AdjustPhvOffset(phv, phv_indices.rbegin(), end, &header_byte_idx,
                      &prev_phv_idx);
      header_bytes.insert(header_bytes.end(), new_header_bytes.begin(),
                          new_header_bytes.end());
      header_bytes.resize(std::min(max_new_header_bytes_size,
                                   header_bytes.size()));
    }

    // Pass 2.
    {
      int real_start_byte;
      auto fde_indices = Get18QfdeIndices(240, 480, pov, version,
                                          &real_start_byte);
      auto phv_indices = GetValidPhvIndices(fde_indices);
      RMT_LOG_VERBOSE("DEPARSER::%s:: Pass 2 starts at %d\n", __FUNCTION__,
                      real_start_byte);
      auto begin = phv_indices.rbegin();
      std::advance(begin, phv_indices.size() - (240 - real_start_byte));
      AdjustPhvOffset(phv, begin, phv_indices.rend(), &header_byte_idx,
                      &prev_phv_idx);
      auto new_header_bytes = GetHeaderBytes(phv, checksum, phv_indices,
                                             &prev_phv_idx, &header_byte_idx);

      // Pop-off the first 3 bytes since they were consumed in the pass 1.
      while (real_start_byte != 240) {
        ++real_start_byte;
        new_header_bytes.pop_front();
        // If we are popping off bytes, the new header must not be empty.
        RMT_ASSERT(new_header_bytes.size() > 0);
      }

      // Put the remaining bytes in the header.
      header_bytes.insert(header_bytes.end(), new_header_bytes.begin(),
                          new_header_bytes.end());
    }

    // If the extracted header size > 480B, drop the packet.
    (*drop_packet_flag) = false;
    if (header_bytes.size() > 480) {
      (*drop_packet_flag) = true;
      deparser_reg_.increment_hdr_too_long(egress_);
    }
    else {
      int real_start_byte;
      if (Get18QfdeIndices(480, 481, pov, version,
                           &real_start_byte).size() > 0) {
        (*drop_packet_flag) = true;
      }
    }
    return header_bytes;
  }

  bool Deparser::is_pov_valid(const BitVector<256> &pov, uint8_t pov_position) {
    //RMT_LOG_VERBOSE("DEPARSER::is_pov_valid:: pov_[%03d]=%01d\n",pov_position,pov_.get_bit(pov_position));
    return (pov.get_bit(pov_position));
  }

  BitVector<256> Deparser::ExtractPov(const Phv &phv) {
    BitVector<256> pov;
    uint32_t phv_byte_idx = 0;
    uint32_t last_phv_idx = 255;

    // For all registers that point to POV byte
    for (int pov_pos_reg_idx=0; pov_pos_reg_idx<32; pov_pos_reg_idx++) {
      uint8_t phv_idx = deparser_reg_.get_pov_pos(egress_,pov_pos_reg_idx); //OD dprsr_reg_rspec_.pov_pos().pov_position(pov_pos_reg_idx);
      // POV bits can be extracted only from PHV registers, not T-PHV.
      if (phv_idx < Phv::phv_max_d()) {
        uint32_t phv_word = phv.get_ignore_valid_bit_d(phv_idx);

        if (last_phv_idx == phv_idx) {
          phv_byte_idx = (phv_byte_idx+1) % phv.which_width_in_bytes_d(phv_idx);
          phv_word = ((phv_word >> (phv_byte_idx*8)) & 0xFF);
        } else {
          phv_byte_idx = 0;
          phv_word = (phv_word & 0xFF);
        }

        for (int i=0; i<8; i++) {
          if (((phv_word >> i) & 0x1))
            RMT_LOG_VERBOSE("DEPARSER::ExtractPov:: setting pov_@%03d phv_idx:%01d phv_byte_idx:%01d phv_width:%01d orig_phv_word:%08x phv_word:%02x\n",
                        (pov_pos_reg_idx*8+i),phv_idx,phv_byte_idx,phv.which_width_in_bytes_d(phv_idx),phv.get_ignore_valid_bit_d(phv_idx),phv_word);

          pov.set_bit( ((phv_word >> i) & 0x1), (pov_pos_reg_idx*8+i) );
        }

      }
      else if (0xFF == phv_idx) {
        phv_byte_idx = 0;
      }
      else {
        RMT_LOG_WARN("DEPARSER::%s:: Unexpected PHV idx %" PRIu8 " in POV position register\n", __FUNCTION__, phv_idx);
        phv_byte_idx = 0;
      }

      last_phv_idx = phv_idx;
    } // end iterating through all 32 POV position registers

    return pov;
  }

  Deparser::Checksums
  Deparser::GenerateChecksum(const Phv &phv) {
    std::array<uint16_t, kNumChecksumEngines> phv_checksums;
    Checksums checksums;

    for (int i = 0; i < kNumChecksumEngines; ++i) {
      phv_checksums.at(i) = GetPhvChecksum(phv, i);
    }

    for (int i = 0; i < kNumChecksumEngines; ++i) {
      checksums.at(i) = GetChecksum(phv, phv_checksums, i);
    }

    return checksums;
  }

  uint16_t
  Deparser::ApplyChecksumRowEntry(const int &csum_idx,
                                  const int &phv_word_width,
                                  const bool &is_tagalong_phv,
                                  const int &checksum_engine_idx,
                                  const uint32_t phv_word) {
    uint16_t modified_phv_word;
    bool swap_bytes = GLOBAL_FALSE;
    bool zero_msb = GLOBAL_FALSE;
    bool zero_lsb = GLOBAL_FALSE;
    std::tie(swap_bytes, zero_msb, zero_lsb) =
      deparser_reg_.GetChecksumEntry(is_tagalong_phv, checksum_engine_idx,
                                     csum_idx);
    if (phv_word_width==8) {
      if (swap_bytes) {
        modified_phv_word = (phv_word << 8) & 0xFF00;
      }
      else {
        modified_phv_word = phv_word & 0x00FF;
      }
    }
    else {
      modified_phv_word = (phv_word & 0xFFFF);
      if (swap_bytes)
        SwapBytes(&modified_phv_word);
    }
    if (zero_msb) {
      modified_phv_word &= 0x00FF;
    }
    if (zero_lsb) {
      modified_phv_word &= 0xFF00;
    }

    return modified_phv_word;
  }

  uint16_t
  Deparser::GetPhvChecksum(const Phv &phv,
                           const int &checksum_engine_idx) {
    uint32_t prev_checksum = 0x0, checksum = 0x0;
    for (int csum_idx = 0; csum_idx < kNumPHVChecksumCfgEntries; ++csum_idx) {
      int shift;
      auto phv_idx = deparser_reg_.phv_idx(csum_idx, &shift);

      const uint32_t phv_word = (phv.get_d(phv_idx) >> shift) & 0xFFFF;
      const int phv_word_width = phv.which_width_d(phv_idx);
      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugDeparserCsumVerbose),
              "DEPARSER::%s: PHV: Got 0x%x(%u) from PHV idx: %u, "
              "checksum idx: %i, checksum engine: %d, width: %d\n",
              __func__, phv_word, phv_word, phv_idx, csum_idx,
              checksum_engine_idx, phv_word_width);

      // If valid bit is not set, checksum should treat PHV word as 0.
      RMT_ASSERT((phv_word == 0) || phv.is_valid_d(phv_idx));

      checksum += ApplyChecksumRowEntry(csum_idx, phv_word_width, false,
                                        checksum_engine_idx, phv_word);
      if (checksum != 0) {
        RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugDeparserCsumNonZero),
                "DEPARSER::%s: PHV: phv idx: %u, "
                "checksum idx: %i, checksum engine: %d, width: %d, "
                "phv_word: 0x%x(%u), checksum:%08x, shift: %d\n",
                __func__, phv_idx, csum_idx, checksum_engine_idx,
                phv_word_width, phv_word, phv_word, checksum, shift);
      }
      if (checksum != prev_checksum) {
        RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugDeparserCsumChange),
                "DEPARSER::%s: PHV: phv idx: %u, "
                "checksum idx: %i checksum engine: %d, width: %d, "
                "phv_word: 0x%x(%u), checksum:%08x, shift: %d\n",
                __func__, phv_idx, csum_idx, checksum_engine_idx,
                phv_word_width, phv_word, phv_word, checksum, shift);
      }
      prev_checksum = checksum;
    }
    checksum = ((checksum >> 16) & 0xFFFF) + (checksum & 0xFFFF);
    checksum = ((checksum >> 16) & 0xFFFF) + (checksum & 0xFFFF);
    return checksum;
  }

  std::tuple<uint16_t, bool>
  Deparser::GetChecksum(
    const Phv &phv,
    const std::array<uint16_t, kNumChecksumEngines> &phv_checksums,
    const int &checksum_engine_idx) {
    uint32_t prev_checksum = 0x0, checksum = 0x0;

    for (int index = 0; index < (kNumTagalongPHVChecksumCfgEntries -
                                 kNumChecksumEngines); ++index) {
      uint32_t phv_idx;

      if (index < (kNum32BitTagalongPHVEntries*2)) {
        phv_idx = (index >> 1) + k_phv::kTagalongStart;
      } else {
        phv_idx = (index-kNum32BitTagalongPHVEntries+k_phv::kTagalongStart);
      }
      uint32_t phv_word = phv.get_ignore_valid_bit_dx(phv_idx);
      uint8_t phv_word_width = 0;
      if (index < (kNum32BitTagalongPHVEntries*2)) {
        phv_word_width = 32;
      }
      else if (index < ((kNum32BitTagalongPHVEntries * 2) +
                        kNum8BitTagalongPHVEntries)) {
        phv_word_width = 8;
      }
      else {
        phv_word_width = 16;
      }

      // For T-PHVs there is a linear mapping from checksum index to T-PHV
      // container. This may change later.
      if (phv_word_width == 32) {
        if (index % 2) {
          phv_word = (phv_word & 0xFFFF);
        } else {
          phv_word = ((phv_word >> 16) & 0xFFFF);
        }
      }
      checksum += ApplyChecksumRowEntry(index, phv_word_width, true,
                                        checksum_engine_idx, phv_word);
      if (checksum != prev_checksum) {
        RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugDeparserCsumChange),
                "DEPARSER::%s: TPHV: phv_idx: %u, "
                "checksum idx: %i checksum engine: %d, "
                "width: %d, phv_word: 0x%x(%u), checksum:%08x\n",
                __func__, phv_idx, index, checksum_engine_idx,
                phv_word_width, phv_word, phv_word, checksum);
      }
      prev_checksum = checksum;
    }

    // The last kNumChecksumEngines determine which checksum output from PHV is
    // used by T-PHV.
    auto phv_checksum = phv_checksums.begin();
    bool is_valid = true;
    for (int index = 0; index < kNumChecksumEngines; ++index) {
      RMT_ASSERT(phv_checksum != phv_checksums.end());
      const auto csum_idx = (index + kNumTagalongPHVChecksumCfgEntries -
                             kNumChecksumEngines);
      auto modified_phv_checksum = ApplyChecksumRowEntry(csum_idx, 16, true,
                                                         checksum_engine_idx,
                                                         *phv_checksum);
      if (modified_phv_checksum != 0) {
        int phv_checksum_pipe = -1, checksum_pipe = -2, engine_idx_offset;
        deparser_reg_.get_csum_array_index(checksum_engine_idx, &checksum_pipe,
                                           &engine_idx_offset);
        deparser_reg_.get_csum_array_index(index, &phv_checksum_pipe,
                                           &engine_idx_offset);
        if (phv_checksum_pipe != checksum_pipe) {
          is_valid = false;
        }
      }
      checksum += modified_phv_checksum;

      if (checksum != prev_checksum) {
        RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugDeparserCsumChange),
                "DEPARSER::%s: TPHV+PHV: checksum idx: %i checksum engine: %d, "
                "PHV checksum[%d]:%08x, checksum:%08x, is_valid=%d\n",
                __func__, csum_idx, checksum_engine_idx,
                index, *phv_checksum, checksum, is_valid);
      }
      prev_checksum = checksum;

      ++phv_checksum;
    }

    checksum = ((checksum >> 16) & 0xFFFF) + (checksum & 0xFFFF);
    checksum = ((checksum >> 16) & 0xFFFF) + (checksum & 0xFFFF);
    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugDeparserCsumVerbose),
            "DEPARSER::%s: Checksum engine index: %d, Checksum:%08x\n",
            __func__, checksum_engine_idx, checksum);
    RMT_ASSERT(0 == (checksum & 0xFFFF0000));

    checksum = (~checksum) & 0xFFFF;
    if (checksum == 0 && deparser_reg_.zeros_as_ones(checksum_engine_idx)) {
        checksum = 0xFFFF;
    }

    return std::make_tuple((uint16_t)checksum, is_valid);
  }

  void Deparser::SwapBytes(uint16_t *phv_word) {
    const uint16_t lsb = (*phv_word) & 0xFF;
    const uint16_t msb = ((*phv_word) & 0xFF00) >> 8;
    (*phv_word) = (lsb << 8) | msb;
  }

  PacketBuffer*
  Deparser::GetMetadataHeader(const Phv &phv,
                              const std::vector<uint8_t> &phv_idx_list) {
    if ( phv_idx_list.size() == 0 ) {
      return get_object_manager()->pktbuf_create(nullptr,0);
    }

    std::vector<uint8_t> metadata_header;
    metadata_header.reserve(phv_idx_list.size());

    uint8_t prev_phv_idx = 0xFF; // This is an invalid PHV index.
    // This is the offset (in bytes) within the PHV word.
    int offset = 0;
    for (unsigned i = 0; i < phv_idx_list.size(); ++i) {
      uint8_t phv_idx = phv_idx_list.at(i);
      uint32_t phv_word = phv.get_ignore_valid_bit_d(phv_idx);

      if (prev_phv_idx == phv_idx) {
        // Pick up the next byte from the same PHV word.
        ++offset;
      }
      else {
        offset = 0;
      }
      // Rollover to the first byte for 8b and 32b PHVs. Assert for 16b PHVs.
      const int width_bytes = phv.which_width_in_bytes_d(phv_idx);
      if (offset == width_bytes) {
        if (width_bytes == 2) {
          RMT_LOG_ERROR("Deparser::%s: Cannot deparse %d consecutively",
                        __func__, width_bytes);
          RMT_ASSERT(false);
        }
        else offset = 0;
      }

      metadata_header.push_back((phv_word >> ((width_bytes - 1 - offset) * 8)) & 0xFF);
      prev_phv_idx = phv_idx;
    }

    RmtObjectManager *om = get_object_manager();
    return om->pktbuf_create(&metadata_header.front(),
                             metadata_header.size());
  }

  Packet *Deparser::GetMirrorPacket(const Phv &phv, const Packet &pkt) {
    Packet *mirror_pkt = nullptr;
    uint8_t phv_idx, shift;
    bool valid = false;
    deparser_reg_.get_mirror_cfg(egress_, &phv_idx, &valid, &shift);
    if (valid && phv.is_valid_d(phv_idx)) {

      // Pick up just the 3 LSBs from the PHV word.
      const uint8_t mirror_table_idx = phv.get_d(phv_idx) & 0x07;
      uint8_t mirror_id_phv_idx;
      std::vector<uint8_t> phv_idx_list;

      deparser_reg_.get_mirror_table_entry(egress_, mirror_table_idx, &valid,
                                           &phv_idx_list, &mirror_id_phv_idx);
      if (valid && phv.is_valid_d(mirror_id_phv_idx)) {
        // create pkt_clone w/o metadata, but keep signature for debug logs
        mirror_pkt = pkt.clone(false/*no-meta*/);
        mirror_pkt->pkt_id_mirr_cpy(pkt.pkt_id(), pipe_index(), 1);  // We only generate a single mirror copy
        mirror_pkt->prepend(GetMetadataHeader(phv, phv_idx_list));

        MirrorMetadata *mirror_metadata = mirror_pkt->mirror_metadata();
        const auto mirror_id = ((phv.get_d(mirror_id_phv_idx) & 0xFFFF) >> shift) & 0x3FF;
        mirror_metadata->set_mirror_id(mirror_id);
        if (egress_) {
          mirror_metadata->set_version(pkt.egress_info()->version());
          // add coal_len... flush bit is not supported anymore but pass it along
          uint8_t coal_len;
          if (deparser_reg_.e_coal_phv_valid()) {
            coal_len = phv.get_d(deparser_reg_.e_coal_phv());
          } else {
            coal_len = deparser_reg_.e_coal_len();
          }
          mirror_metadata->set_coal_len(coal_len);
        } else {
          mirror_metadata->set_version(pkt.ingress_info()->version());
          mirror_metadata->set_coal_len(0);
        }
      }
    }

    return mirror_pkt;
  }

  uint64_t
  Deparser::GetMetadataFromPhv(const Phv &phv, const uint8_t &phv_idx,
                               const uint8_t &shift, const bool &shift_valid,
                               const int &metadata_width) const {
    uint32_t mask;
    if (shift_valid) {
      mask = 0xFF;
      if (metadata_width > 8) {
        mask = 0xFFFF;
      }
    }
    else {
      mask = (1 << metadata_width) - 1;
      RMT_ASSERT (shift == 0);
    }
    // Making phv_word 64b wide since we want to return a 64b value.
    const uint64_t phv_word = (uint64_t)((phv.get_d(phv_idx) & mask) >> shift);
    const int width = phv.which_width_d(phv_idx);
    int current_offset = 0;
    uint64_t value = 0;

    if (metadata_width > width) {
      RMT_LOG_WARN("Metadata width (%d bits) exceeds PHV width (%d bits)\n",
                   metadata_width, width);
    }

    // We will repeat the PHV word until we get a 64b value. Hardware does it
    // this way if the metadata width is larger than the PHV width.
    do {
      value |= (phv_word << current_offset);
      current_offset += width;
    } while ((8 * sizeof(value)) != current_offset);

    // mask off to metadata width
    value &= (( UINT64_C(1) << metadata_width ) - 1 );

    return value;
  }

  // For TofinoXX remap to correct physical pipe
  uint64_t Deparser::RemapLogicalToPhy(uint64_t log_port, bool ingress) {
    uint8_t  ipipe_map = deparser_reg_.get_ipipe_remap();
    uint8_t  epipe_map = deparser_reg_.get_epipe_remap();
    uint8_t  log_phy_map = (ingress) ?ipipe_map :epipe_map;
    uint32_t pipe = Port::get_pipe_num(log_port);
    /* 2 bits for each logical pipe */
    uint32_t phy_pipe = (log_phy_map >> (pipe*2)) & 0x3;

    if (pipe == phy_pipe) {
      return log_port;
    }
    uint64_t phy_port = Port::make_port_index(phy_pipe, log_port);

    RMT_LOG_VERBOSE("Deparser: Remapped logical port %" PRIu64 "to physical"
                    " port %" PRIu64 " \n", (log_port & 0x1FF), phy_port);
    return phy_port;
  }
}
