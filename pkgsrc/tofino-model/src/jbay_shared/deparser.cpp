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

#include <deparser.h>

#include <string>
#include <sstream>
#include <algorithm>
#include <common/rmt-assert.h>
#include <deparser-reg.h>
#include <model_core/log-buffer.h>
#include <packet.h>
#include <port.h>
#include <rmt-log.h>
#include <rmt-object-manager.h>

namespace MODEL_CHIP_NAMESPACE {
  Deparser::Deparser(RmtObjectManager *om, int pipeIndex, int ioIndex,
                     bool egress, DeparserReg &deparser_reg)
    : PipeObject(om,pipeIndex), io_index_(ioIndex), egress_(egress),
      invert_csum_ctl_(om->chip_index(), pipeIndex),
      deparser_reg_(deparser_reg)
  {
    RMT_LOG_VERBOSE("DEPARSER::create: pipe_index:%d ioIndex:%d\n",
                    pipe_index(), io_index_);
  }

  Deparser::~Deparser() {
    RMT_LOG_VERBOSE("DEPARSER::delete: pipe_index:%d io_index:%d\n",
                    pipe_index(), io_index_);
  }

// can't be inline as Port is incomplete type in the .h file
int Deparser::GetPortNumber(const Packet* pkt) {
  RMT_ASSERT(pkt);
  Port* port = pkt->port();
  RMT_ASSERT(port);
  return Port::get_pipe_local_port_index(port->port_index());
}

Packet* Deparser::GetNewPacket(const Phv &phv, Packet *pkt, const BitVector<kPovWidth>& pov) {

    if (!enabled()) {
      RMT_LOG_VERBOSE("DEPARSER::%s: not enabled, returning\n", __func__);
      return NULL;
    }

    int slice  = GetSliceNumber(pkt);

    int slice_channel = GetChannelWithinSlice(pkt);

    bool port_enabled = deparser_reg_.port_enabled(egress_,slice,slice_channel);
    RMT_LOG_VERBOSE("DEPARSER::%s: port=%d local_port=%d slice=%d slice_channel=%d port_enabled=%s\n", __func__,
                    pkt->port()->port_index(),
                    Port::get_pipe_local_port_index(pkt->port()->port_index()),
                    slice, slice_channel, port_enabled?"true":"false");


    // Generate checksum
    auto checksum = GenerateChecksum(phv,pkt,pov,slice);

    // Generate new header bytes
    bool drop_packet_flag = false;
    auto new_header_bytes = GenerateNewHeader(phv, pkt, slice,
                                              pov, checksum,
                                              GetPktVersion(pkt),
                                              &drop_packet_flag);

    RMT_LOG_VERBOSE("DEPARSER::%s: Packet header (len=%d):", __func__,new_header_bytes.size());
    bool logged = false;
    const int kLogLineBytes=64;
    model_core::LogBuffer buf(1 + kLogLineBytes*2);
    for (uint32_t i=0; i<new_header_bytes.size(); i++) {
      buf.Append("%02x",new_header_bytes[i]);
      if ( 0 == ((i+1) % kLogLineBytes) ) {
        // log one line of header bytes
        RMT_LOG_VERBOSE(" %s\n", buf.GetBuf());
        logged = true;
        buf.Reset();
      }
    }
    if (!logged) {
      RMT_LOG_VERBOSE(" %s\n", buf.GetBuf());
    }

    if (drop_packet_flag || !port_enabled) {
      RMT_LOG_VERBOSE("DEPARSER::%s: Dropping packet (drop=%s port_enabled=%s)", __func__,
		      drop_packet_flag?"true":"false",port_enabled?"true":"false");
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

  void Deparser::CheckHeaderByteChunks(std::vector<int>& header_byte_chunks) const {
    // XXX: JbayA0 bug causes incorrect headers when certain bytes are
    // due to certain FD chunks; software should avoid those byte/chunk
    // combinations so report an error if they are detected
    RMT_ASSERT(RmtObject::is_jbayA0());
    int header_len = header_byte_chunks.size();
    if (!kRelaxDeparserChunkChecks && (header_len > 176)) {
      std::vector<std::string> bad_bytes;
      int chunk_index = header_byte_chunks[172];
      if ((chunk_index >= 24) && ((chunk_index % 8) == 0)) {
        bad_bytes.push_back(std::string("173rd byte is due to chunk index ") +
                            std::to_string(chunk_index));
      }
      if (header_len > 352) {
        chunk_index = header_byte_chunks[348];
        if ((chunk_index >= 40) && ((chunk_index % 8) == 0)) {
          bad_bytes.push_back(std::string("349th byte is due to chunk index ") +
                              std::to_string(chunk_index));
        }
      }
      if (!bad_bytes.empty()) {
        std::string msg = "Header of length " + std::to_string(header_len) +":\n";
        for (const auto& it : bad_bytes) msg.append(it);
        RMT_LOG_ERROR("Deparser::%s: %s: %s", __func__,  gress_str(), msg.c_str());
        RMT_ASSERT(kRelaxDeparserByteChecks);
      }
    }
  }

  std::vector<uint8_t>
  Deparser::GenerateNewHeader(const Phv &phv, Packet *pkt, int slice,
                              const BitVector<kPovWidth>& pov,
                              const Checksums &checksum,
                              const uint8_t &version, bool *drop_packet_flag) {
    // TODO: this never sets drop_packet_flag, should it? When header is too big?
    // records each header byte...
    std::vector<uint8_t> header_bytes;
    // records chunk responsible for each header byte...
    std::vector<int> header_byte_chunks;

    constexpr int kCells = 3;
    constexpr int kCellSize = 176;
    constexpr int kStage1OutputChunks = 48;
    constexpr int kTotalStage1OutputChunks = kStage1OutputChunks * kCells;
    // as cell0 last chunk is the same as cell1 first chunk etc. max valid is less
    constexpr int kMaxValidChunks = kTotalStage1OutputChunks - kCells + 1;

    // get all the valid chunks
    struct {
      int current_word = 0;
      int valid_chunks = 0;
      int chunks_in_this_cell = 0;
      int bytes_in_this_cell = 0;
      int cell = 0;
      int last_phv_idx = -1;
      int phv_which_byte = 0;
      int phv_word_width_in_bytes=0;
    } d_state;
    for (int word = 0; word < kWords; ++word) {

      // XXX: a Clot entry must reside entirely within a single word
      int      clot_slice = -1, clot_chunk = -1;
      uint8_t  clot_tag = Clot::kBadTag;
      uint16_t clot_offset = 0, clot_length = 0, clot_checksum = 0;

      for (int chunk = 0; chunk < kChunksPerWord; ++chunk) {

        FdChunk fdc = deparser_reg_.get_chunk( slice,  egress_, word, chunk );
        if ( fdc.valid && is_pov_valid(pov, fdc.pov)) {

          int len = 0;
          int start_of_slice = 0;
          if ( fdc.seg_vld == 0 ) {
            // from phv
            len = fdc.len;

            // XXX: clear last emitted index in pkt clot (if one exists) as interspersing a byte
            // from a PHV word/checksum/constant should prevent subsequent reuse of a clot entry that has
            // a previous adjacent clot entry (adjacent as in immediately preceding in parsed packet)
            if ((len > 0) && (pkt->clot() != nullptr)) pkt->clot()->clear_last_emitted();

            RMT_LOG_VERBOSE("DEPARSER::%s: valid phv chunk (w=%d,ch=%d) len=%d\n", __func__, word,chunk,len);
          }
          else {
            // from clot
            auto clot = pkt->clot();
            RMT_ASSERT(clot && "Clot missing from packet");
            uint8_t segment_tag = deparser_reg_.get_clot_sel_segment_tag(slice,egress_, word, fdc.seg_sel );

            // XXX: Maybe re-use clot info if identical tag as last one used this word
            // (this prevents 2nd/3rd/... calls to get() when a tag is reused in a chunk,
            //  which the Clot::get() code would consider to be an error)
            if ( (segment_tag == clot_tag) ||
                 (clot->get( segment_tag, &clot_length, &clot_offset, &clot_checksum)) ) {

              // XXX: Clot slices must be used in order and appear in adjacent chunks
              if ((segment_tag == clot_tag) && !kRelaxDeparserClotChecks) {
                RMT_ASSERT( (fdc.seg_slice == clot_slice+1) && "Clot slices must be used in order");
                RMT_ASSERT( (chunk == clot_chunk+1) && "Clot slices must be in adjacent chunks");
              }
              clot_tag = segment_tag;
              clot_slice = fdc.seg_slice;
              clot_chunk = chunk;

              start_of_slice = fdc.seg_slice * 8; // TODO kBytesPerSegmentSlice
              len = clot_length - start_of_slice;
              if (len < 0) len = 0;
              if (len > 8) len = 8; // TODO kBytesPerChunk
              RMT_LOG_VERBOSE("DEPARSER::%s: valid clot chunk (w=%d,ch=%d) len=%d (tag=0x%02x,clot_offset=%d,clot_length=%d)\n",
                              __func__, word,chunk,len,segment_tag,clot_offset,clot_length);
            } else if (!clot->err_invalid_tag()) {
              RMT_LOG_ERROR("Deparser::%s: Error processing clot chunk (w=%d,ch=%d) (tag=0x%02x,last_tag=0x%02x,err=%s)\n",
                            __func__, word,chunk,segment_tag,clot_tag,clot->err_str());
              if (!kRelaxDeparserClotChecks) RMT_ASSERT(0 && "Bad clot extract");
            } else {
              // XXX: Missing tag could indicate misconfiguration OR could be deliberate - so can only WARN & skip
              RMT_LOG_WARN("Deparser::%s: Missing clot tag - skipping (w=%d,ch=%d) (tag=0x%02x,last_tag=0x%02x)\n",
                           __func__, word,chunk,segment_tag,clot_tag);
            }
          }

          if (len>0) {
            d_state.valid_chunks++;
            d_state.chunks_in_this_cell++;
            RMT_ASSERT( kRelaxDeparserChunkChecks || (d_state.valid_chunks <= kMaxValidChunks) );
            RMT_ASSERT( kRelaxDeparserChunkChecks || (d_state.chunks_in_this_cell <= kStage1OutputChunks ));
          }

          for (int i=0; i<len; ++i) {
            bool get_from_phv = ( fdc.seg_vld == 0 ) || fdc.is_phv[i];

            int byte = 0;
            int phv_idx = fdc.byte_off[i]; // TODO phv_idx is a bad name as could be checksum or constant too
            if (get_from_phv) { // also covers checksum and constant

              bool new_phv = (phv_idx != d_state.last_phv_idx);

              if ( new_phv ) {
                // check that the previous phv word was fully used    // this check also valid for checksums
                RMT_ASSERT( kRelaxDeparserByteChecks || ((d_state.last_phv_idx == -1) || ( (d_state.phv_which_byte+1) == d_state.phv_word_width_in_bytes ) ));

                if ( phv_idx < Phv::phv_max_d() ) {

                  d_state.current_word = phv.get_ignore_valid_bit_d( phv_idx );
                  d_state.phv_word_width_in_bytes = Phv::which_width_in_bytes_d( phv_idx );

                  RMT_LOG_VERBOSE("DEPARSER::%s: %d phv_idx=%d word=%0*x\n",
                                  __func__, i, phv_idx, d_state.phv_word_width_in_bytes*2, d_state.current_word);

                  if (CheckDeparserPhvGroupConfig(phv_idx,slice) == false) {
                    RMT_LOG_ERROR("Deparser::%s: Deparser group check failed for %u expected %s\n",
                                  __func__, phv_idx, gress_str());
                    RMT_ASSERT(kRelaxDeparserGroupChecks || false);
                  }
                }
                else if (phv_idx >= Phv::phv_max_d() && (phv_idx < (Phv::phv_max_d() + kNumConstants))) {
                  int which = phv_idx - Phv::phv_max_d();
                  d_state.current_word = deparser_reg_.get_constant(egress_,slice,which);
                  d_state.phv_word_width_in_bytes = 1;
                  RMT_LOG_VERBOSE("DEPARSER::%s: %d const_idx=%d word=%08x\n",
                                  __func__, i, which, d_state.current_word);
                }
                else if (phv_idx < (Phv::phv_max_d() + kNumConstants + kNumChecksumEngines)) {
                  uint32_t csum_idx = phv_idx - Phv::phv_max_d() - kNumConstants;
                  d_state.current_word = checksum[csum_idx];
                  d_state.phv_word_width_in_bytes = 2;
                  RMT_LOG_VERBOSE("DEPARSER::%s: %d csum_idx=%03d word=%08x\n",
                                  __func__, i, csum_idx, d_state.current_word);
                }
                else {
                  if (! kRelaxDeparserFdeChecks ) {
                    throw std::out_of_range(std::string("Invalid Field dictionary entry ") + std::to_string(phv_idx));
                  }
                }
                RMT_ASSERT( d_state.phv_word_width_in_bytes > 0  && d_state.phv_word_width_in_bytes <= 4);
                d_state.last_phv_idx = phv_idx;
                d_state.phv_which_byte = 0;
              }
              else {
                // still in the same word

                // for 1 byte words you can use the same byte over and over again,
                //  it just keeps using the byte
                if ( d_state.phv_word_width_in_bytes != 1 ) {
                  d_state.phv_which_byte++;
                }
                // checksums can keep using the same word as well, so wrap around
                if ( (d_state.last_phv_idx >= (Phv::phv_max_d() + kNumConstants)) &&
                     (d_state.last_phv_idx <  (Phv::phv_max_d() + kNumConstants + kNumChecksumEngines)) ) {
                  if ( d_state.phv_which_byte == d_state.phv_word_width_in_bytes ) {
                    d_state.phv_which_byte = 0;
                  }
                }
                RMT_LOG_VERBOSE("DEPARSER::%s: %d    which_byte=%d\n", __func__, i, d_state.phv_which_byte);
                // Check that there are still bytes to get in the phv word
                RMT_ASSERT( kRelaxDeparserByteChecks || ( d_state.phv_which_byte < d_state.phv_word_width_in_bytes));
              }
              // As PHV is big endian have to get most significant byte first
              const int which_byte = d_state.phv_word_width_in_bytes - 1 - d_state.phv_which_byte;
              byte = (d_state.current_word >> (which_byte * 8)) & 0xFF;
            }
            else { // from CLOT
              int byte_offset = clot_offset + start_of_slice + i; // phv_idx is ignored for clots
              uint8_t buf[1];
              int got_len = pkt->get_buf( buf, byte_offset, 1);
              if (got_len != 1) {
                RMT_LOG_ERROR("Deparser::%s: Failed to get clot byte at offset=%d, "
                              "packet length=%d\n",__func__,byte_offset,pkt->len());
              }
              // check getting clot from packet succeeded
              RMT_ASSERT( kRelaxDeparserClotChecks || kRelaxDeparserClotLenChecks || (got_len == 1) );
              byte = buf[0];
              RMT_LOG_VERBOSE("DEPARSER::%s: %d clot_byte_offset=%d byte=%02x\n",
                              __func__, i, byte_offset, byte);
            }
            d_state.bytes_in_this_cell++;

            if ( d_state.bytes_in_this_cell > kCellSize ) {

              int remaining_in_cell = len - i - 1;
              if ( remaining_in_cell < 3 ) {
                // this cell will use the next chunk too
                RMT_ASSERT( kRelaxDeparserChunkChecks || ( (d_state.chunks_in_this_cell+1) <= kStage1OutputChunks ));
              }

              // now move on to preparing for the next cell
              d_state.chunks_in_this_cell = 1; // next cell uses this chunk too
              d_state.bytes_in_this_cell  = 1;
              d_state.cell++;

              if ( i < 3 ) {
                // next cell uses the previous chunk too
                d_state.chunks_in_this_cell++;
              }
            }
            header_bytes.push_back(byte);
            header_byte_chunks.push_back(fdc.index);
          }

        } // ( fdc.valid && is_pov_valid(pov, fdc.pov))
      } // for (int chunk = 0; chunk < kChunksPerWord; ++chunk)
    }  // for (int word = 0; word < kWords; ++word) {

    // check that the final phv word was fully used    // this check also valid for checksums
    RMT_ASSERT( kRelaxDeparserByteChecks || ((d_state.last_phv_idx == -1) || ( (d_state.phv_which_byte+1) == d_state.phv_word_width_in_bytes ) ));

    if (RmtObject::is_jbayA0()) CheckHeaderByteChunks(header_byte_chunks);
    // TODO: check the final cell?

    return header_bytes;
  }

  bool Deparser::is_pov_valid(const BitVector<kPovWidth>& pov, uint8_t pov_position) {
    //RMT_LOG_VERBOSE("DEPARSER::is_pov_valid: pov_[%03d]=%01d\n",pov_position,pov_.get_bit(pov_position));
    return (pov.get_bit(pov_position));
  }

BitVector<Deparser::kPovWidth> Deparser::ExtractPov(const Phv &phv) {
    BitVector<kPovWidth> pov;
    uint32_t phv_byte_idx = 0;
    uint32_t last_phv_idx = 255;

    // For all registers that point to POV byte
    for (int pov_pos_reg_idx=0; pov_pos_reg_idx<16; pov_pos_reg_idx++) {
      uint8_t phv_idx = deparser_reg_.get_pov_pos(egress_,pov_pos_reg_idx); //OD dprsr_reg_rspec_.pov_pos().pov_position(pov_pos_reg_idx);
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
            RMT_LOG_VERBOSE("DEPARSER::ExtractPov: setting pov_@%03d phv_idx:%01d phv_byte_idx:%01d phv_width:%01d orig_phv_word:%08x phv_word:%02x\n",
                        (pov_pos_reg_idx*8+i),phv_idx,phv_byte_idx,phv.which_width_in_bytes_d(phv_idx),phv.get_ignore_valid_bit_d(phv_idx),phv_word);

          pov.set_bit( ((phv_word >> i) & 0x1), (pov_pos_reg_idx*8+i) );
        }

      }
      else if (0xFF == phv_idx) {
        phv_byte_idx = 0;
      }
      else {
        RMT_LOG_WARN("DEPARSER::%s: Unexpected PHV idx %" PRIu8 " in POV position register\n", __func__, phv_idx);
        phv_byte_idx = 0;
      }

      last_phv_idx = phv_idx;
    }

    return pov;
  }

  Deparser::Checksums
  Deparser::GenerateChecksum(const Phv &phv,const Packet* pkt, const BitVector<kPovWidth>& pov,int slice) {
    Checksums phv_checksums;
    Checksums checksums;

    for (int i = 0; i < kNumChecksumEngines; ++i) {
      phv_checksums.at(i) = GetPhvChecksum(phv, pov, i);
    }

    for (int i = 0; i < kNumChecksumEngines; ++i) {
      checksums.at(i) = CombineChecksums(slice,phv,pkt,pov, phv_checksums, i);
    }

    return checksums;
  }

  uint16_t
  Deparser::ApplyChecksumRowEntry(int engine,
                                  const BitVector<kPovWidth>& pov,
                                  const int &csum_idx,
                                  const int &phv_word_width,
                                  const uint32_t phv_word) {
    uint16_t modified_phv_word;
    bool swap_bytes, zero_msb, zero_lsb;
    int csum_pov;
    deparser_reg_.GetChecksumEntry(engine,csum_idx,
                                   &csum_pov,&swap_bytes,&zero_msb,&zero_lsb);

    if ( ! deparser_reg_.test_csum_pov( engine, csum_pov, pov ) ) {
      return 0;
    }
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
  Deparser::GetPhvChecksum(const Phv &phv, const BitVector<kPovWidth>& pov,
                           const int &checksum_engine_idx) {

    // Check this engine is set for this gress
    if ( deparser_reg_.ChecksumEngineThreadIsEgress( checksum_engine_idx) != egress_ ) {
      return 0;
    }

    uint32_t checksum = 0x0;
    for (int csum_idx = 0; csum_idx < kNumPHVChecksumCfgEntries; ++csum_idx) {
      int shift;
      auto phv_idx = deparser_reg_.phv_idx(csum_idx, &shift);
      const uint32_t phv_word = (phv.get_d(phv_idx) >> shift) & 0xFFFF;
      const int phv_word_width = phv.which_width_d(phv_idx);
      if ( phv_word !=0 ) {
        RMT_LOG_VERBOSE("DEPARSER::%s: Got %08x from PHV idx: %u, "
                        "checksum idx: %i checksum engine: %d width: %d\n",
                        __func__, phv_word, phv_idx, csum_idx,
                        checksum_engine_idx, phv_word_width);
      }

      checksum += ApplyChecksumRowEntry(checksum_engine_idx,
                                        pov, csum_idx, phv_word_width,
                                        phv_word);
      if (checksum != 0) {
        RMT_LOG_VERBOSE("DEPARSER::%s: PHV: idx: %d, phv_word_width: %d, "
                        "csum engine idx: %d, phv_word: %u, checksum:%08x\n",
                        __func__, csum_idx, phv_word_width,
                        checksum_engine_idx, phv_word, checksum);
      }
    }
    checksum = ((checksum >> 16) & 0xFFFF) + (checksum & 0xFFFF);
    checksum = ((checksum >> 16) & 0xFFFF) + (checksum & 0xFFFF);
    return checksum;
  }

  uint16_t
  Deparser::CombineChecksums( int slice,
                                 const Phv &phv,const Packet* pkt, const BitVector<kPovWidth>& pov,
                                 const Checksums &phv_checksums,
                                 const int &csum_engine_idx) {
    // Check this engine is set for this gress
    if ( deparser_reg_.ChecksumEngineThreadIsEgress( csum_engine_idx) != egress_ ) {
      return 0;
    }
    uint32_t checksum = 0x0;

    // first eight are checksums from the PHV checksum engines
    for (int index=0; index < 8; ++index) {
      // XXX: initialise phv word in uint16_t so inversion works correctly
      uint16_t word = 0;
      // Copy in phv checksum word if row entry valid
      if ( deparser_reg_.GetFullChecksumPhvEntryValid( slice, csum_engine_idx, index, pov ) )
        word = phv_checksums[index];
      // XXX: Maybe invert phv checksum word (inverted *invalid* entries become 0xFFFF)
      if (get_csum_invert_phv(csum_engine_idx, index)) word = ~word;
      checksum += word;
    }

    // next sixteen are from CLOTs
    bool reset_clot = false;
    for (int index=0; index < 16; ++index) {
      // XXX: initialise clot checksum word in uint16_t so inversion works correctly
      uint16_t word = 0;
      // Copy in clot checksum word if row entry valid *and* clot tag found in Clot
      if ( deparser_reg_.GetFullChecksumClotEntryValid( slice, csum_engine_idx, index, pov ) ) {
        int tag = deparser_reg_.tag( slice, egress_, csum_engine_idx, index );
        uint16_t clot_length;
        uint16_t clot_offset;
        uint16_t clot_checksum = 0;
        auto clot = pkt->clot();
        RMT_ASSERT(clot && "Clot missing from packet");
        reset_clot = true;
        if ( clot->get( tag, &clot_length, &clot_offset, &clot_checksum, false ) ) {
          word = clot_checksum;
        } else if (!clot->err_invalid_tag()) {
          RMT_LOG_ERROR("Deparser::%s: Error processing clot checksum (slice=%d,engine=%d,index=%d) (tag=0x%02x,err=%s)\n",
                       __func__,slice, csum_engine_idx, index, tag, clot->err_str());
          if (!kRelaxDeparserClotChecks) RMT_ASSERT(0 && "Bad clot checksum extract");
        } else {
          // XXX: Missing tag could indicate misconfiguration OR could be deliberate - so can only WARN & use cksum 0
          RMT_LOG_WARN("Deparser::%s: Missing clot tag - using checksum 0x0000 (slice=%d,engine=%d,index=%d) (tag=0x%02x)\n",
                       __func__,slice, csum_engine_idx,index,tag);
        }
      }
      // XXX: Maybe invert clot checksum (invalid/missing tag entries become 0xFFFF)
      if (get_csum_invert_clot(csum_engine_idx, index)) word = ~word;
      checksum += word;
    }
    // XXX: Reset emission as we may need to use Clot again later in GenerateNewHeader
    if (reset_clot) pkt->clot()->reset_emit();

    // add the constant
    checksum += deparser_reg_.get_csum_constant(slice,egress_,csum_engine_idx);

    checksum = ((checksum >> 16) & 0xFFFF) + (checksum & 0xFFFF);
    checksum = ((checksum >> 16) & 0xFFFF) + (checksum & 0xFFFF);


    RMT_ASSERT(0 == (checksum & 0xFFFF0000));

    checksum = (~checksum) & 0xFFFF;
    if (checksum == 0 && deparser_reg_.zeros_as_ones(slice,csum_engine_idx)) {
        checksum = 0xFFFF;
    }

    RMT_LOG_VERBOSE("DEPARSER::%s: Checksum slice: %d, %s, engine index: %d, Checksum:%08x\n",
                    __func__, slice,gress_str(),csum_engine_idx, checksum);
    return checksum;
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

    RMT_LOG_VERBOSE("DEPARSER::GetMetadataHeader: phv_idx_list.size:%d\n",phv_idx_list.size());

    uint8_t prev_phv_idx = 0xFF; // This is an invalid PHV index.
    // This is the offset (in bytes) within the PHV word.
    int offset = 0;
    for (unsigned i = 0; i < phv_idx_list.size(); ++i) {
      uint8_t phv_idx = phv_idx_list.at(i);
      // TODO could remove this when things are stable (getting the phv word will assert in this case
      if (phv_idx >= 224) {
        RMT_LOG_ERROR("Deparser::%s: Phv Index out of range %d\n", __func__, phv_idx);
        RMT_ASSERT(kRelaxDeparserByteChecks);
        break;
      }
      uint32_t phv_word = phv.get_ignore_valid_bit_d(phv_idx);

      // TODO: check whole PHV word is used
      if (prev_phv_idx == phv_idx) {
        // Pick up the next byte from the same PHV word.
        ++offset;
      }
      else {
        offset = 0;
      }
      const int width_bytes = phv.which_width_in_bytes_d(phv_idx);

      // Rollover for 8b PHVs, but flag an error if the same 16 or 32 bit word is used consecutively
      if (offset == width_bytes) {
        if ((width_bytes == 2) || (width_bytes == 4)) {
          RMT_LOG_ERROR("Deparser::%s: Cannot deparse %d consecutively",
                        __func__, width_bytes);
          RMT_ASSERT( kRelaxDeparserByteChecks || false);
        }
        else offset = 0;
      }

      RMT_LOG_VERBOSE("DEPARSER::GetMetadataHeader:   phv_idx:%d byte:0x%02x\n",phv_idx,(phv_word >> ((width_bytes - 1 - offset) * 8)) & 0xFF);
      metadata_header.push_back((phv_word >> ((width_bytes - 1 - offset) * 8)) & 0xFF);
      prev_phv_idx = phv_idx;
    }

    RmtObjectManager *om = get_object_manager();
    return om->pktbuf_create(&metadata_header.front(),
                             metadata_header.size());
  }

Packet *Deparser::GetMirrorPacket(const Phv& phv, const Packet& pkt,const BitVector<kPovWidth>& pov) {
  Packet *mirror_pkt = nullptr;
  uint64_t mirror_table_idx;
  bool valid = egress_ ? deparser_reg_.get_e_mirr_sel_info( phv,pov,&mirror_table_idx ) :
      deparser_reg_.get_i_mirr_sel_info( phv,pov,&mirror_table_idx );
  RMT_LOG_VERBOSE("DEPARSER::GetMirrorPacket: %s valid:%s\n",gress_str(), valid?"true":"false");
  if (valid) {
    int slice  = GetSliceNumber(&pkt);

    uint8_t mirror_id_phv_idx;
    std::vector<uint8_t> phv_idx_list;

    deparser_reg_.get_mirror_table_entry(egress_,slice, mirror_table_idx,
                                         &phv_idx_list, &mirror_id_phv_idx);

    // TODO could remove this when things are stable (getting the phv word will assert in this case)
    if (mirror_id_phv_idx >= 224) {
      RMT_LOG_ERROR("DEPARSER::%s: Mirror Id Phv Index out of range %d\n", __func__, mirror_id_phv_idx);
      RMT_ASSERT(kRelaxDeparserByteChecks);
      return nullptr;
    }

    // create pkt_clone w/o metadata, but keep signature for debug logs
    mirror_pkt = pkt.clone(false/*no-meta*/);
    mirror_pkt->pkt_id_mirr_cpy(pkt.pkt_id(), pipe_index(), 1);  // We only generate a single mirror copy
    mirror_pkt->prepend(GetMetadataHeader(phv, phv_idx_list));

    MirrorMetadata *mirror_metadata = mirror_pkt->mirror_metadata();
    const auto mirror_id = phv.get_d(mirror_id_phv_idx)  & 0xFF;
    mirror_metadata->set_mirror_id(mirror_id);

    // cog data is set up in include/jbay/deparser_metadata.py see include/jbay/deparser-reg.h running for details
    //[[[cog import deparser_metadata as metadata ]]]
    //[[[end]]] (checksum: d41d8cd98f00b204e9800998ecf8427e)

    uint64_t ret_value;
    if (egress_) {
      mirror_metadata->set_version(pkt.egress_info()->version());
      //[[[cog cog.out(metadata.egress_mirr_extract_list) ]]]
      if ( deparser_reg_.get_e_mirr_sel_info(phv, pov, &ret_value) ) mirror_metadata->set_mirr_sel( ret_value );
      if ( deparser_reg_.get_e_mirr_io_sel_info(slice, phv, pov, &ret_value) ) mirror_metadata->set_mirr_io_sel( ret_value );
      if ( deparser_reg_.get_e_mirr_hash_info(slice, phv, pov, &ret_value) ) mirror_metadata->set_mirr_hash( ret_value );
      if ( deparser_reg_.get_e_mirr_epipe_port_info(slice, phv, pov, &ret_value) ) mirror_metadata->set_mirr_epipe_port( ret_value );
      if ( deparser_reg_.get_e_mirr_qid_info(slice, phv, pov, &ret_value) ) mirror_metadata->set_mirr_qid( ret_value );
      if ( deparser_reg_.get_e_mirr_dond_ctrl_info(slice, phv, pov, &ret_value) ) mirror_metadata->set_mirr_dond_ctrl( ret_value );
      if ( deparser_reg_.get_e_mirr_icos_info(slice, phv, pov, &ret_value) ) mirror_metadata->set_mirr_icos( ret_value );
      if ( deparser_reg_.get_e_mirr_mc_ctrl_info(slice, phv, pov, &ret_value) ) mirror_metadata->set_mirr_mc_ctrl( ret_value );
      if ( deparser_reg_.get_e_mirr_c2c_ctrl_info(slice, phv, pov, &ret_value) ) mirror_metadata->set_mirr_c2c_ctrl( ret_value );
      if ( deparser_reg_.get_e_mirr_coal_smpl_len_info(slice, phv, pov, &ret_value) ) mirror_metadata->set_coal_len( ret_value );
      //[[[end]]] (checksum: 7313238bf9953e07366b6cd3884a95ac)
    } else {
      mirror_metadata->set_version(pkt.ingress_info()->version());
      //[[[cog cog.out(metadata.ingress_mirr_extract_list) ]]]
      if ( deparser_reg_.get_i_mirr_sel_info(phv, pov, &ret_value) ) mirror_metadata->set_mirr_sel( ret_value );
      if ( deparser_reg_.get_i_mirr_io_sel_info(slice, phv, pov, &ret_value) ) mirror_metadata->set_mirr_io_sel( ret_value );
      if ( deparser_reg_.get_i_mirr_hash_info(slice, phv, pov, &ret_value) ) mirror_metadata->set_mirr_hash( ret_value );
      if ( deparser_reg_.get_i_mirr_epipe_port_info(slice, phv, pov, &ret_value) ) mirror_metadata->set_mirr_epipe_port( ret_value );
      if ( deparser_reg_.get_i_mirr_qid_info(slice, phv, pov, &ret_value) ) mirror_metadata->set_mirr_qid( ret_value );
      if ( deparser_reg_.get_i_mirr_dond_ctrl_info(slice, phv, pov, &ret_value) ) mirror_metadata->set_mirr_dond_ctrl( ret_value );
      if ( deparser_reg_.get_i_mirr_icos_info(slice, phv, pov, &ret_value) ) mirror_metadata->set_mirr_icos( ret_value );
      if ( deparser_reg_.get_i_mirr_mc_ctrl_info(slice, phv, pov, &ret_value) ) mirror_metadata->set_mirr_mc_ctrl( ret_value );
      if ( deparser_reg_.get_i_mirr_c2c_ctrl_info(slice, phv, pov, &ret_value) ) mirror_metadata->set_mirr_c2c_ctrl( ret_value );
      if ( deparser_reg_.get_i_mirr_coal_smpl_len_info(slice, phv, pov, &ret_value) ) mirror_metadata->set_coal_len( ret_value );
      //[[[end]]] (checksum: 6de51c02a201b9a95d803add6efa2595)
    }
  }

  return mirror_pkt;
}


bool Deparser::MaybeTruncate(Packet *pkt, bool ingress) {
  bool truncated = false;
  if ((nullptr != pkt) && (ingress)) {
    auto mtu_trunc_len = pkt->i2qing_metadata()->mtu_trunc_len();
    if ( mtu_trunc_len && ( pkt->len() > *mtu_trunc_len ) ) {
      RMT_LOG_VERBOSE("DEPARSER::%s: truncating to %d (packet length was %d)\n", __func__, *mtu_trunc_len, pkt->len() );
      pkt->trim_back(pkt->len() - *mtu_trunc_len );
      pkt->set_truncated(true);
      truncated = true;
    }
  }
  return truncated;
}
bool Deparser::ZeroiseCRC(Packet *pkt) {
  if (nullptr == pkt) return false;
  const int fcs_len = Packet::kPktFcsLen; // 4B
  uint8_t buf[fcs_len] = { 0 };
  bool buf_is0 = true;
  int  fcs_pos = pkt->len() - fcs_len;
  int  buf_len = pkt->get_buf(buf, (fcs_pos > 0) ?fcs_pos :0, fcs_len); // Last 4B
  RMT_ASSERT(buf_len <= fcs_len);
  for (int i = 0; i < fcs_len; i++) buf_is0 = (buf_is0 && (buf[i] == 0));
  if (buf_is0) return true; // Already zero
  uint8_t zeros[fcs_len] = { 0 };
  PacketBuffer *zero_pb = get_object_manager()->pktbuf_create(zeros, buf_len);
  pkt->trim_back(buf_len);
  pkt->append(zero_pb);
  return true;
}
bool Deparser::MaybePad(Packet *pkt, bool ingress) {
  bool padded = false;
  if (nullptr != pkt) {
    int slice = GetSliceNumber(pkt);
    // XXX/XXX: Maybe pad to a minimum length (WIP only)
    int min_pkt_len = static_cast<int>( deparser_reg_.chip_reg().get_min_pkt_len(slice, ingress) );
    int act_pkt_len = pkt->len();
    if ((min_pkt_len > 0) && (act_pkt_len < min_pkt_len)) {
      RMT_ASSERT(min_pkt_len < 64);
      static uint8_t pad_buf[64] = { 0 };
      PacketBuffer *pad_pb = get_object_manager()->pktbuf_create(pad_buf, min_pkt_len - act_pkt_len);
      RMT_LOG_VERBOSE("DEPARSER::%s: padding to %d (packet length was %d)\n", __func__, min_pkt_len, act_pkt_len );
      pkt->append(pad_pb);
      padded = true;
    }
  }
  return padded;
}


// For jbay remap logical port  number to correct physical port
uint64_t Deparser::RemapLogicalToPhy(uint64_t log_port, bool ingress) {

  // Nothing to do on JBay
  //
  // uint64_t phy_port=0;
  // if ( log_port <= 63 ) {
  //   phy_port = log_port;
  // }
  // else if ( log_port <= 127 ) {
  //   phy_port = log_port + 64;
  // }
  // else if ( log_port <= 131 ) {
  //   phy_port = log_port - 64; // ethernet cpu/ recirculation
  // }
  // else if ( log_port <= 135 ) {
  //   phy_port = log_port - 64; // Recirculation/Packet Generation
  // }
  // else if ( log_port <= 139 ) {
  //   phy_port = log_port + 56; // PCIe/Recirculation
  // }
  // else if ( log_port <= 143 ) {
  //   phy_port = log_port + 56; // Recirculation/Packet Generation
  // }
  // else {
  //   RMT_ASSERT(0);
  // }
  //
  // RMT_LOG_VERBOSE("Deparser::%s: Remapped logical port %" PRIu64 "to physical"
  //                 " port %" PRIu64 " \n", __func__, log_port, phy_port);
  // return phy_port;

  return log_port;
}

}
