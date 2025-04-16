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

#include <mau.h>
#include <string>
#include <rmt-log.h>
#include <rmt-object-manager.h>
#include <mau-meter.h>
#include <mau-stats-alu.h>
#include <phv.h>
#include <address.h>
#include <register_adapters.h>



namespace MODEL_CHIP_NAMESPACE {

  // See section 6.2.10 of MAU uArch doc and table 6.27 Stats RAM Entry Formats
  //
  // Form index into this table using <msb> PKT[1] BYTE[1] #Ents[3] <lsb>
  //
  // #Ents 0 (OR pkt_fld_width 0 AND byte_fld_width 0) => INVALID format
  // OTHERWISE we have a valid format but in that case:
  // pkt_offset, byte_offset -1 => INVALID entry for format
  // 
  // idx, PKT, BYTE, #Ents, pkt_fld_width, byte_fld_width, flags, {pkt_offsets}, {byte_offsets}
  const MauStatsAluConfig MauStatsAlu::kStatsAlu_Config[] = {
    {  0, 0, 0, 0, 0, 0,         0, { -1, -1, -1, -1, -1, -1, -1 }, { -1, -1, -1, -1, -1, -1, -1} },
    {  1, 0, 0, 0, 0, 0,         0, { -1, -1, -1, -1, -1, -1, -1 }, { -1, -1, -1, -1, -1, -1, -1} },
    {  2, 0, 0, 0, 0, 0,         0, { -1, -1, -1, -1, -1, -1, -1 }, { -1, -1, -1, -1, -1, -1, -1} },
    {  3, 0, 0, 0, 0, 0,         0, { -1, -1, -1, -1, -1, -1, -1 }, { -1, -1, -1, -1, -1, -1, -1} },
    {  4, 0, 0, 0, 0, 0,         0, { -1, -1, -1, -1, -1, -1, -1 }, { -1, -1, -1, -1, -1, -1, -1} },
    {  5, 0, 0, 0, 0, 0,         0, { -1, -1, -1, -1, -1, -1, -1 }, { -1, -1, -1, -1, -1, -1, -1} },
    {  6, 0, 0, 0, 0, 0,         0, { -1, -1, -1, -1, -1, -1, -1 }, { -1, -1, -1, -1, -1, -1, -1} },
    {  7, 0, 0, 0, 0, 0,         0, { -1, -1, -1, -1, -1, -1, -1 }, { -1, -1, -1, -1, -1, -1, -1} },
    // Byte configs
    {  8, 0, 1, 0, 0, 0,         0, { -1, -1, -1, -1, -1, -1, -1 }, { -1, -1, -1, -1, -1, -1, -1} },
    {  9, 0, 1, 0, 0, 0,         0, { -1, -1, -1, -1, -1, -1, -1 }, { -1, -1, -1, -1, -1, -1, -1} },
    { 10, 0, 1, 2, 0,64, kFlgRsvd0, { -1, -1, -1, -1, -1, -1, -1 }, {  0, -1, -1, -1, 64, -1, -1} },
    { 11, 0, 1, 0, 0, 0,         0, { -1, -1, -1, -1, -1, -1, -1 }, { -1, -1, -1, -1, -1, -1, -1} },
    { 12, 0, 1, 4, 0,32,  kFlgDflt, { -1, -1, -1, -1, -1, -1, -1 }, {  0, -1, 32, -1, 64, -1, 96} },
    { 13, 0, 1, 0, 0, 0,         0, { -1, -1, -1, -1, -1, -1, -1 }, { -1, -1, -1, -1, -1, -1, -1} },
    { 14, 0, 1, 0, 0, 0,         0, { -1, -1, -1, -1, -1, -1, -1 }, { -1, -1, -1, -1, -1, -1, -1} },
    { 15, 0, 1, 0, 0, 0,         0, { -1, -1, -1, -1, -1, -1, -1 }, { -1, -1, -1, -1, -1, -1, -1} },
    // Packet configs
    { 16, 1, 0, 0, 0, 0,         0, { -1, -1, -1, -1, -1, -1, -1 }, { -1, -1, -1, -1, -1, -1, -1} },
    { 17, 1, 0, 0, 0, 0,         0, { -1, -1, -1, -1, -1, -1, -1 }, { -1, -1, -1, -1, -1, -1, -1} },
    { 18, 1, 0, 2,64, 0, kFlgRsvd0, {  0, -1, -1, -1, 64, -1, -1 }, { -1, -1, -1, -1, -1, -1, -1} },
    { 19, 1, 0, 0, 0, 0,         0, { -1, -1, -1, -1, -1, -1, -1 }, { -1, -1, -1, -1, -1, -1, -1} },
    { 20, 1, 0, 4,32, 0,  kFlgDflt, {  0, -1, 32, -1, 64, -1, 96 }, { -1, -1, -1, -1, -1, -1, -1} },
    { 21, 1, 0, 0, 0, 0,         0, { -1, -1, -1, -1, -1, -1, -1 }, { -1, -1, -1, -1, -1, -1, -1} },
    //
    // XXX 
    // - was using incorrect subword->entry mappings for 6-entry SRAMs
    // - NOT a simple sequential mapping
    // - see uArch 1.9.1 fig 6-40 page 116
    //
    // OLD sub0=<ent0> sub1=<ent1> sub2=<ent2> sub3=<ent3> sub4=<ent4> sub5=<ent5> sub6=-1
    // OLD sub0=0      sub1=21     sub2=42     sub3=64     sub4=85     sub5=106    sub6=-1
    // OLD { 22,1,0,6,21,0, kFlgDflt, { 0,21,42,64,85,106,-1 },  { -1,-1,-1,-1,-1,-1,-1 } },
    //
    // NEW sub0=<ent0> sub1=<ent2> sub2=<ent4> sub3=-1     sub4=<ent1> sub5=<ent3> sub6=<ent5>
    // SO  sub0=0      sub1=42     sub2=85     sub3=-1     sub4=21     sub5=64     sub6=106
    // NEW { 22,1,0,6,21,0, kFlgDflt, { 0,42,85,-1,21,64,106 },  { -1,-1,-1,-1,-1,-1,-1} },
    //
    { 22, 1, 0, 6,21, 0,  kFlgDflt, {  0, 42, 85, -1, 21, 64,106 }, { -1, -1, -1, -1, -1, -1, -1} },
    { 23, 1, 0, 0, 0, 0,         0, { -1, -1, -1, -1, -1, -1, -1 }, { -1, -1, -1, -1, -1, -1, -1} },
    // Packet+Byte configs
    { 24, 1, 1, 0, 0, 0,         0, { -1, -1, -1, -1, -1, -1, -1 }, { -1, -1, -1, -1, -1, -1, -1} },
    { 25, 1, 1, 1,64,64, kFlgRsvd0, {  0, -1, -1, -1, -1, -1, -1 }, { 64, -1, -1, -1, -1, -1, -1} },
    { 26, 1, 1, 2,28,36,  kFlgDflt, {  0, -1, -1, -1, 64, -1, -1 }, { 28, -1, -1, -1, 92, -1, -1} },
    { 27, 1, 1, 3,17,25,  kFlgDflt, {  0, 42, 85, -1, -1, -1, -1 }, { 17, 60,102, -1, -1, -1, -1} },
    { 28, 1, 1, 0, 0, 0,         0, { -1, -1, -1, -1, -1, -1, -1 }, { -1, -1, -1, -1, -1, -1, -1} },
    { 29, 1, 1, 0, 0, 0,         0, { -1, -1, -1, -1, -1, -1, -1 }, { -1, -1, -1, -1, -1, -1, -1} },
    { 30, 1, 1, 0, 0, 0,         0, { -1, -1, -1, -1, -1, -1, -1 }, { -1, -1, -1, -1, -1, -1, -1} },
    { 31, 1, 1, 0, 0, 0,         0, { -1, -1, -1, -1, -1, -1, -1 }, { -1, -1, -1, -1, -1, -1, -1} },
  };


  // STATIC funcs to access format array above

  // Get pkt/byte off/nbits for a subword in some particular stats format
  bool MauStatsAlu::get_stats_format_info(int which_format, int subword,
                                          int *pkt_nbits, int *pkt_off,
                                          int *byte_nbits, int *byte_off, int *flags) {
    if ((which_format < 0) || (which_format >= kMaxStatsFormats)) return false;
    if (!is_valid_stats_mode(which_format)) return false;
    const MauStatsAluConfig &config = kStatsAlu_Config[which_format];
    if ((subword < 0) || (subword >= kMaxEntriesPerWord)) return false;
    
    // Return false if config has 0 entries or 0 pkt/byte field width
    // or if the specified subword is invalid for the config
    if (config.n_entries == 0) return false;
    if ((config.pkt_field_width == 0) && (config.byte_field_width == 0)) return false;
    if ((config.pkt_offsets[subword] < 0) && (config.byte_offsets[subword] < 0)) return false;

    // Sanity check pkt_fld_width <= byte_fld_width
    if ((config.pkt_field_width != 0) && (config.byte_field_width != 0)) {
      RMT_ASSERT(config.pkt_field_width <= config.byte_field_width);
      if (config.pkt_field_width < config.byte_field_width) { 
        // If pkt_fld_width < byte_fld_width should be 8b smaller
        RMT_ASSERT(config.pkt_field_width + kStatsPktByteWidthDelta == config.byte_field_width);
      }
    }    
    // So a valid config has been selected and we want info about a valid subword    
    if (pkt_nbits != NULL)  *pkt_nbits = static_cast<int>(config.pkt_field_width);
    if (pkt_off != NULL)    *pkt_off = static_cast<int>(config.pkt_offsets[subword]);
    if (byte_nbits != NULL) *byte_nbits = static_cast<int>(config.byte_field_width);
    if (byte_off != NULL)   *byte_off = static_cast<int>(config.byte_offsets[subword]);
    if (flags != NULL)      *flags = static_cast<int>(config.flags);
    return true;
  }
  // See if all-ones is a RSVD value for format
  bool MauStatsAlu::is_ones_rsvd(int which_format) {
    if ((which_format < 0) || (which_format >= kMaxStatsFormats)) return false;
    const MauStatsAluConfig &config = kStatsAlu_Config[which_format];
    return ones_rsvd(config.flags);
  }
  // See if subword is valid for format
  bool MauStatsAlu::is_subword_valid(int which_format, int subword) {
    return get_stats_format_info(which_format, subword, NULL, NULL, NULL, NULL, NULL);
  }
  // Get mask corresponding to subword given some particular stats format
  bool MauStatsAlu::get_subword_mask(int which_format, int subword,
                                     BitVector<kDataBusWidth> *mask) {
    uint64_t ONES = UINT64_C(0xFFFFFFFFFFFFFFFF);
    int pkt_nbits, pkt_off, byte_nbits, byte_off;
    if (!get_stats_format_info(which_format, subword,
                               &pkt_nbits, &pkt_off, &byte_nbits, &byte_off, NULL))
      return false;
    if ((pkt_nbits > 0) && (pkt_off >= 0))
      mask->set_word(ONES, pkt_off, pkt_nbits);
    if ((byte_nbits > 0) && (byte_off >= 0))
      mask->set_word(ONES, byte_off, byte_nbits);
    return true;
  }
  // Extract subword bits from input_data and return them in dump1 dump2
  // using this particular stats format
  // Return value is how many dump words we need to send - 0, 1 or 2
  int MauStatsAlu::get_dump_data_word(int which_format, int subword,
                                      const BitVector<kDataBusWidth> &input_data,
                                      uint64_t *dump1, uint64_t *dump2,
                                      bool no_dump_if_rsvd) {
    
    int pkt_nbits, pkt_off, byte_nbits, byte_off, flags;
    if (!get_stats_format_info(which_format, subword,
                               &pkt_nbits, &pkt_off, &byte_nbits, &byte_off, &flags)) 
      return 0;

    uint64_t ones = UINT64_C(0xFFFFFFFFFFFFFFFF), zeros = UINT64_C(0);  
    uint64_t pkt_cnt, pkt_max, byte_cnt, byte_max;

    bool rsvd = false;
    bool pkt  = ((pkt_nbits > 0)  && (pkt_off >= 0));
    bool byte = ((byte_nbits > 0) && (byte_off >= 0));
    RMT_ASSERT(pkt || byte);
    
    if (pkt && byte) {
      pkt_cnt = input_data.get_word(pkt_off, pkt_nbits);
      byte_cnt = input_data.get_word(byte_off, byte_nbits);
      pkt_max = ones >> (64-pkt_nbits);
      byte_max = ones >> (64-byte_nbits);
      rsvd = ((zero_rsvd(flags) && (pkt_cnt == zeros) && (byte_cnt == zeros)) ||
              (ones_rsvd(flags) && (pkt_cnt == pkt_max) && (byte_cnt == byte_max)));
    } else if (pkt) {
      pkt_cnt = input_data.get_word(pkt_off, pkt_nbits);
      byte_cnt = zeros;
      pkt_max = ones >> (64-pkt_nbits);
      rsvd = ((zero_rsvd(flags) && (pkt_cnt == zeros)) ||
              (ones_rsvd(flags) && (pkt_cnt == pkt_max)));
    } else if (byte) {
      pkt_cnt = zeros;
      byte_cnt = input_data.get_word(byte_off, byte_nbits);
      byte_max = ones >> (64-byte_nbits);
      rsvd = ((zero_rsvd(flags) && (byte_cnt == zeros)) ||
              (ones_rsvd(flags) && (byte_cnt == byte_max)));
    } else {
      pkt_cnt = zeros; byte_cnt = zeros; // Stop compiler whingeing
    }
    
    // If caller has specified not to dump reserved vals return 0
    if (no_dump_if_rsvd && rsvd) return 0;

    if ((pkt_nbits == 64) && (byte_nbits == 64)) {
      if ((dump1 == NULL) || (dump2 == NULL)) return 0;
      *dump1 = pkt_cnt;
      *dump2 = byte_cnt;
      return 2;
    } else {
      RMT_ASSERT((pkt_nbits + byte_nbits) <= 64);
      if (dump1 == NULL) return 0;
      *dump1 = pkt_cnt;
      if (pkt_nbits < 64) { 
        *dump1 |= (byte_cnt << pkt_nbits);
      }
      return 1;
    }
  }
  // As above but input_data in 2 64b params
  int MauStatsAlu::get_dump_data_word(int which_format, int subword,
                                      uint64_t input_data0, uint64_t input_data1,
                                      uint64_t *dump1, uint64_t *dump2,
                                      bool no_dump_if_rsvd) {
    BitVector<kDataBusWidth> input_data(UINT64_C(0));    
    input_data.set_word(input_data0,  0, 64);
    input_data.set_word(input_data1, 64, 64);
    return get_dump_data_word(which_format, subword, input_data,
                              dump1, dump2, no_dump_if_rsvd);
  }
  // Format pkt_cnt/byte_cnt into LRT evict data word
  uint64_t MauStatsAlu::get_lrt_evict_data_word(int pkt_nbits, int pkt_off,
                                                int byte_nbits, int byte_off,
                                                uint64_t pkt_cnt, uint64_t byte_cnt) {
    uint64_t data = UINT64_C(0);
    uint64_t byte_cnt_mask = UINT64_C(0x0FFFFFFFFF); // Is 36b always
    uint64_t pkt_cnt_mask  = UINT64_C(0x00FFFFFFFF); // Is 32b ...
    if ((byte_nbits > 0) && (byte_off >= 0)) {    
      pkt_cnt_mask = UINT64_C(0x000FFFFFFF);    // ...tho 28b if byte_cnt
      data |= (byte_cnt & byte_cnt_mask) << 28; // Always 36b at off 28
    }
    if ((pkt_nbits > 0) && (pkt_off >= 0)) {
      data |= (pkt_cnt  & pkt_cnt_mask)  <<  0; // 32b OR 28b at off 0
    }
    return data;
  }



  MauStatsAlu::MauStatsAlu(RmtObjectManager *om,
                           int pipeIndex, int mauIndex, int logicalRowIndex,
                           Mau *mau, MauLogicalRow *mau_log_row,
                           int physicalRowIndex, int physicalRowWhich)
      : MauObject(om, pipeIndex, mauIndex, kType, logicalRowIndex, mau),
        mau_log_row_(mau_log_row), mau_memory_(mau->mau_memory()),
        logical_row_(logicalRowIndex),
        alu_index_(get_stats_alu_regs_index(logicalRowIndex)),
        statistics_ctl_(default_adapter(statistics_ctl_,chip_index(), pipeIndex, mauIndex, alu_index_,
                                        [this](){this->stats_ctl_change_callback();})),
        lrt_threshold_(default_adapter(lrt_threshold_,chip_index(), pipeIndex, mauIndex, alu_index_)),
        lrt_update_interval_(default_adapter(lrt_update_interval_,chip_index(), pipeIndex, mauIndex, alu_index_)),
        stats_alu_lt_(default_adapter(stats_alu_lt_,chip_index(), pipeIndex, mauIndex, alu_index_)) {

    static_assert((sizeof(kStatsAlu_Config)/sizeof(kStatsAlu_Config[0]) == kMaxStatsFormats),
                  "Should be exactly kMaxStatsFormats defined in kStatsAlu_Config table");
    RMT_ASSERT(physicalRowWhich == 1);       // Only RHS since regs_12544_mau_dev
    RMT_ASSERT((physicalRowIndex % 2) == 0); // Only even rows since regs_13957_mau_dev
    statistics_ctl_.reset();
    lrt_threshold_.reset();
    lrt_update_interval_.reset();
    stats_alu_lt_.reset();
  }
  MauStatsAlu::~MauStatsAlu() { }


  void MauStatsAlu::stats_ctl_change_callback() {
    int which_format = get_stats_format_this();
    if (which_format == 0) return; // Allow zero reset value too
    if (!is_valid_stats_mode(which_format)) {
      RMT_LOG(RmtDebug::error(),
              "STATS_ALU<%d> Invalid stats format %d\n", logical_row_, which_format);
    }
  }
  int MauStatsAlu::get_stats_format_this() {
    bool pkts = (statistics_ctl_.stats_process_packets() == 0x1);
    bool bytes = (statistics_ctl_.stats_process_bytes() == 0x1);
    int  nentries = (statistics_ctl_.stats_entries_per_word() & 0x7);
    return get_stats_format(pkts, bytes, nentries);
  }
  bool MauStatsAlu::is_lrt_enabled() {
    return (statistics_ctl_.lrt_enable() == 1);
  }
  uint64_t MauStatsAlu::get_lrt_threshold() {
    if (!is_lrt_enabled()) return UINT64_C(0xFFFFFFFFFFFFFFFF);
    return static_cast<uint64_t>(lrt_threshold_.lrt_threshold(0)) << kStatsLrtThresholdShift;
  }
  bool MauStatsAlu::get_subentry_info(int subentry,
                                      int *pkt_nbits, int *pkt_off,
                                      int *byte_nbits, int *byte_off) {
    return get_stats_format_info(get_stats_format_this(), subentry,
                                 pkt_nbits, pkt_off, byte_nbits, byte_off, NULL);
  }
  int MauStatsAlu::get_packet_len(MauExecuteState *state) {
    int pktlen = 0;
    int adj = signextend<signed int, kStatsBytecountAdjustWidth>(
        statistics_ctl_.stats_bytecount_adjust());
    if (state->op_ == kStateOpHandleEop) {
      bool egress = (statistics_ctl_.stats_alu_egress() == 1);
      if (!egress && state->eop_.ingress_valid()) {
        pktlen = static_cast<int>(state->eop_.ingress_pktlen());
      } else if (egress && state->eop_.egress_valid()) {
        pktlen = static_cast<int>(state->eop_.egress_pktlen());
      }
    } else if (state->op_ == kStateOpHandleTeop) {
      pktlen = state->teop_.byte_len();
    } else {
      adj = 0;
    }
    return (pktlen + adj >= 0) ?(pktlen + adj) :0;
  }
  void MauStatsAlu::get_input(BitVector<kDataBusWidth> *data, uint32_t *addr) {
    *addr = 0;
    data->fill_all_zeros();
    mau_log_row_->stats_rd_addr( addr );
    mau_log_row_->stats_alu_rd_data( data );
  }
  void MauStatsAlu::get_output(BitVector<kDataBusWidth> *data, uint32_t *addr) {
    if (has_run_) {
      mau_log_row_->stats_wr_data(data);
      mau_log_row_->stats_wr_addr(addr);
    }
    else {
      data->fill_all_zeros();
      *addr = 0;
    }
  }
  void MauStatsAlu::reset_resources() {
    has_run_=false;
  }
  void MauStatsAlu::set_output_data(BitVector<kDataBusWidth> *data) {
    mau_log_row_->set_stats_wr_data(data);
  }
  void MauStatsAlu::set_output_addr(uint32_t *addr) {
    mau_log_row_->set_stats_wr_addr(addr);
  }
  void MauStatsAlu::set_output(BitVector<kDataBusWidth> *data, uint32_t *addr) {
    has_run_=true;
    set_output_data(data);
    set_output_addr(addr);
  }

  void MauStatsAlu::keep_full_res_stats(uint32_t addr, int pkt_inc, int byte_inc) {
    int logtab = mau_log_row_->stats_logical_table();
    int logtab2 = stats_alu_lt_.mau_cfg_stats_alu_lt();
    if (logtab < 0) logtab = logtab2; // Could happen if no Stats SRAM on row
    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauStatsAluIncr),
            "STATS_ALU_FULL_START<%d> Tab=%d VPN=%d Addr=0x%08x pktInc=%d byteInc=%d\n",
            logical_row_, logtab, Address::stats_addr_get_vpn(addr), addr, pkt_inc, byte_inc);

    if (!kKeepFullResStats || (logtab < 0)) return;
    RMT_ASSERT(mau_memory_ != NULL);
    if ((pkt_inc > 0) || (byte_inc > 0)) {
      uint32_t vaddr = Address::stats_addr_get_vaddr(addr);
      uint32_t pbus_vaddr = Address::addrShift(vaddr, -MauMemory::kStatsFullVAddrPbusShift);
      uint64_t full_pkt_cnt = UINT64_C(0);
      uint64_t full_byte_cnt = UINT64_C(0);
      
      // Since we're abusing stats_virt_read/write_full here (normally called from PBus
      // indirectRead/indirectWrite) we use a VAddr, pbus_vaddr, that has the opposite
      // shift to cancel out the effect of the one that's about to occur
      mau_memory_->stats_virt_read_full(logtab, pbus_vaddr, &full_pkt_cnt, &full_byte_cnt);
      if (pkt_inc > 0)  full_pkt_cnt += static_cast<uint64_t>(pkt_inc);
      if (byte_inc > 0) full_byte_cnt += static_cast<uint64_t>(byte_inc);

      RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauStatsAluIncr),
              "STATS_ALU_FULL<%d> Logtab=%d VPN=%d Index=%d VAddr=0x%08x "
              "FullPktCnt=%" PRId64 " FullByteCount=%" PRId64 "\n",
              logical_row_, logtab, Address::stats_addr_get_vpn(vaddr),
              Address::stats_addr_get_index(vaddr), vaddr,
              full_pkt_cnt, full_byte_cnt);
      
      mau_memory_->stats_virt_write_full(logtab, pbus_vaddr, full_pkt_cnt, full_byte_cnt);
    }
  }

  uint64_t MauStatsAlu::get_wr_cnt(MauExecuteState *state, bool pkt,
                                   int format, int nbits, int off) {
    // If we find a non-zero format and address stashed in the passed-in state,
    // then figure out off/bits from that rather than using input address
    int rd_format = state->rw_format_;
    int rd_addr = state->rw_raddr_;
    int rd_subword = Address::stats_addr_get_subword(rd_addr);
    int rd_pkt_nbits, rd_pkt_off, rd_byte_nbits, rd_byte_off;
    bool got_rd_info = ((rd_format > 0) && (Address::stats_addr_op_enabled(rd_addr)) &&
                        (get_stats_format_info(rd_format, rd_subword,
                                               &rd_pkt_nbits, &rd_pkt_off,
                                               &rd_byte_nbits, &rd_byte_off, NULL)));
    if (got_rd_info && pkt) {
      RMT_ASSERT((rd_format == format) && (nbits == rd_pkt_nbits));
      return state->data_.get_word(rd_pkt_off, rd_pkt_nbits);
    } else if (got_rd_info && !pkt) {
      RMT_ASSERT((rd_format == format) && (nbits == rd_byte_nbits));
      return state->data_.get_word(rd_byte_off, rd_byte_nbits);
    } else {
      return state->data_.get_word(off, nbits);
    }
  }


  bool MauStatsAlu::run_alu_with_state(MauExecuteState *state) {
    int pkt_nbits, pkt_off, byte_nbits, byte_off, flags;
    BitVector<kDataBusWidth> data(UINT64_C(0));
    uint32_t addr = 0u;

    get_input(&data, &addr);
    if (!Address::stats_addr_op_enabled(addr)) return false;
    
    mau_log_row_->mau_addr_dist()->stats_synth2port_fabric_check(alu_index_,logical_row_);
    
    uint32_t waddr = addr;    
    int op = Address::stats_addr_op(addr);
    int subword = Address::stats_addr_get_subword(addr);
    int format = get_stats_format_this();
    
    if (!get_stats_format_info(format, subword,
                               &pkt_nbits, &pkt_off, &byte_nbits, &byte_off, &flags))
      return false;

    // Copy out ALL data if PbusRd|PbusRdClr
    if ((op == Address::kStatsOpCfgRd) || (op == Address::kStatsOpCfgRdClr)) {
      state->data_.copy_from(data);
      state->ret_ = 1;
      // Stash format in case we're doing a movereg read/write
      state->rw_format_ = format;
    } else if (op != Address::kStatsOpCfgWr) {
      // Count how many times ALU had non-cfgRd/Wr stuff to do
      mau()->mau_info_incr(MAU_STATS_ALU_INVOCATIONS);
    }

    // Are we counting pkts, bytes or both
    bool pkt  = ((pkt_nbits > 0)  && (pkt_off >= 0));
    bool byte = ((byte_nbits > 0) && (byte_off >= 0));
    RMT_ASSERT(pkt || byte);
    uint64_t ones = UINT64_C(0xFFFFFFFFFFFFFFFF);    
    uint64_t pkt_cnt = UINT64_C(0);
    uint64_t byte_cnt = UINT64_C(0);
    int      pkt_toff = -1,  pkt_inc = 0;
    int      byte_toff = -1, byte_inc = 0;

    // Work out if both pkt/byte cnt vals at max value
    bool both_at_max = true;
    if (pkt) {
      uint64_t pkt_cnt_max = ones >> (64-pkt_nbits);
      pkt_toff = pkt_off+pkt_nbits-1;
      pkt_cnt = data.get_word(pkt_off, pkt_nbits);
      if (pkt_cnt != pkt_cnt_max) both_at_max = false;
    }
    if (byte) {
      uint64_t byte_cnt_max = ones >> (64-byte_nbits);
      byte_toff = byte_off+byte_nbits-1;
      byte_cnt = data.get_word(byte_off, byte_nbits);
      if (byte_cnt != byte_cnt_max) both_at_max = false;
    }

    // Work out if we increment/do LRT
    // With most formats the max (all-ones) value is RSVD *and* LRT is allowed
    // However 64b format neither reserves all-ones nor allows LRT
    uint64_t lrt_thresh = get_lrt_threshold(); // MAX_U64 if not enabled
    bool     max_and_rsvd = both_at_max && ones_rsvd(flags);
    bool     lrt_possible = !max_and_rsvd && is_lrt_enabled() && lrt_avail(flags);
    bool     do_lrt = false, do_clr = false;
    
    // Now maybe do increment, clear, LRT
    if (pkt) {
      switch (op) {
        case Address::kStatsOpCfgWr:
          pkt_cnt = get_wr_cnt(state, true, format, pkt_nbits, pkt_off);
          break;
        case Address::kStatsOpStats: 
          pkt_inc = 1;
          if (!max_and_rsvd) pkt_cnt += static_cast<uint64_t>(pkt_inc);
          if (lrt_possible && (pkt_cnt*kStatsPktMult > lrt_thresh)) do_lrt = true;
          break;
        case Address::kStatsOpCfgRd:
          break;
        case Address::kStatsOpCfgRdClr:
          if (!max_and_rsvd) do_clr = true;
          break;
      }
    }
    if (byte) {
      switch (op) {
        case Address::kStatsOpCfgWr:
          byte_cnt = get_wr_cnt(state, false, format, byte_nbits, byte_off);
          break;
        case Address::kStatsOpStats: 
          byte_inc = get_packet_len(state);
          if (!max_and_rsvd) byte_cnt += static_cast<uint64_t>(byte_inc);
          if (lrt_possible && (byte_cnt*kStatsByteMult > lrt_thresh)) do_lrt = true;
          break;
        case Address::kStatsOpCfgRd:
          break;
        case Address::kStatsOpCfgRdClr:
          if (!max_and_rsvd) do_clr = true;
          break;
      }
    }
    
    RMT_LOG(RmtDebug::verbose(RmtDebug::kRmtDebugMauStatsAluIncr),
            "STATS_ALU<%d> Pkt[%d:%d]=%" PRId64 " Bytes[%d:%d]=%" PRId64 " %s %s\n",
            logical_row_, pkt_toff, pkt_off, pkt_cnt, byte_toff, byte_off, byte_cnt,
            (do_lrt) ?"EVICT" :"", (do_clr) ?"CLEAR" :"");

    // Do we do LRT evict
    if (do_lrt) {
      uint64_t evict_data = get_lrt_evict_data_word(pkt_nbits, pkt_off,
                                                    byte_nbits, byte_off,
                                                    pkt_cnt, byte_cnt);
      // Stash addr/data in state evict info for later handling
      state->evict_info_.set_evictinfo(alu_index_, addr, evict_data);
      mau()->mau_info_incr(MAU_STATS_ALU_EVICTIONS);
    }
    if (do_clr) {
      mau()->mau_info_incr(MAU_STATS_ALU_DUMPS);
    }
    // Zeroise counts in stats entry - LRT or Dump&Clear
    if (do_lrt || do_clr) pkt_cnt = byte_cnt = UINT64_C(0);


    // Update vals in data word
    if (pkt)  data.set_word(pkt_cnt, pkt_off, pkt_nbits);
    if (byte) data.set_word(byte_cnt, byte_off, byte_nbits);

    // Setup output regardless
    set_output(&data, &waddr);
    
    if (((pkt_inc > 0) || (byte_inc > 0))  &&  !max_and_rsvd) {
      // NB. Neither ordinary PbusWr nor Dump affects full_res_stats.
      // Need to do 'special' full_res_stats virt PbusWr for that.

      // NB. Increment *both* pkt and byte stats irrespective
      // of format unless they were at max val
      keep_full_res_stats(addr, 1, get_packet_len(state));
    }
    return true;
  }


}
