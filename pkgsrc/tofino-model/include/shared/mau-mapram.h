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

#ifndef _SHARED_MAU_MAPRAM_
#define _SHARED_MAU_MAPRAM_

#include <string>
#include <deque>
#include <cstdint>
#include <rmt-defs.h>
#include <mau-defs.h>
#include <mau-object.h>
#include <mau-logical-row.h>
#include <mau-mapram-reg.h>
#include <sram.h>


namespace MODEL_CHIP_NAMESPACE {

  class Mau;
  class MauSram;

  class MauMapram : public MauObject {
 public:
    // Defined in rmt-config.cpp
    static bool     kMapramWriteData1HasTime;
    static uint64_t kMapramColorWriteLatencyTEOP;

    static constexpr int      kType = RmtTypes::kRmtTypeMauMapram;
    static constexpr int      kExactMatchInputBits = MauDefs::kExactMatchInputBits;
    static constexpr int      kExactMatchValidBits = MauDefs::kExactMatchValidBits;
    static constexpr int      kNextTableWidth = MauDefs::kNextTableWidth;

    static constexpr int      kSramWidth = MauDefs::kSramWidth;
    static constexpr int      kSramAddressWidth  = MauDefs::kSramAddressWidth;
    static constexpr int      kSramEntries = 1<<kSramAddressWidth;
    static constexpr int      kMapramColumns = MauDefs::kMapramColumnsPerMau;
    static constexpr int      kMapramWidth = MauDefs::kMapramWidth;
    static constexpr int      kMapramAddressWidth  = MauDefs::kMapramAddressWidth;
    static constexpr int      kMapramEntries = 1<<kMapramAddressWidth;
    static constexpr int      kMapramVpnBits = MauDefs::kMapramVpnBits;
    static constexpr uint8_t  kMapramVpnMask = MauDefs::kMapramVpnMask;
    static constexpr uint8_t  kMapramVpnUnoccupied = MauDefs::kMapramVpnUnoccupied;
    static constexpr uint64_t kMapramMask = UINT64_C(0xFFFFFFFFFFFFFFFF) >> (64-kMapramWidth);

    static constexpr uint64_t kMapramColorWriteLatency = MauDefs::kMapramColorWriteLatency;

    static constexpr int      kMapramColorEntryWidth = 2;
    static constexpr uint8_t  kMapramColorEntryMask  = (1<<kMapramColorEntryWidth)-1;


    static constexpr uint64_t kIdletimeFsmMsgType = UINT64_C(0);
    static constexpr int      kIdletimeFsmMsg_IdleActiveOffset = 4;
    static constexpr int      kIdletimeFsmMsg_AddressOffset = 36;
    static constexpr int      kIdletimeFsmMsg_PipeStageTableOffset = 52;
    static constexpr uint64_t kIdletimeLockAckMsgType = UINT64_C(1);
    static constexpr int      kIdletimeLockAckMsg_LockIdOffset = 36;
    static constexpr int      kIdletimeLockAckMsg_PipeStageTableOffset = 52;
    static constexpr uint64_t kIdletimeDumpMsgType = UINT64_C(2);
    static constexpr int      kIdletimeDumpMsg_DataOffset = 4;
    static constexpr int      kIdletimeDumpMsg_AddressOffset = 36;
    static constexpr int      kIdletimeDumpMsg_PipeStageTableOffset = 52;



    MauMapram(RmtObjectManager *om, int pipeIndex, int mauIndex,
              int rowIndex, int colIndex, int mapramIndex, Mau *mau);
    virtual ~MauMapram();


    inline int col_index()                     const { return col_index_; }
    inline int row_index()                     const { return row_index_; }
    inline MauSram *sram()                     const { return sram_; }
    inline void set_sram(MauSram *sram)              { sram_ = sram; }
    inline MauLogicalRow *logical_row()        const { return logrow_; }
    inline void set_logical_row(MauLogicalRow *row)  {
      logrow_ = row;
      physical_row_ = logrow_ ? logrow_->physical_row() : nullptr;
    }
    inline MauSramRow *physical_row()          const { return physical_row_; }
    inline int mapram_index()                  const { return mapram_index_; }
    inline int logcol_index()                  const { return logcol_index_; }

    inline bool is_ingress()                { return mau_mapram_reg_.is_ingress(); }
    inline bool is_egress()                 { return mau_mapram_reg_.is_egress(); }
    inline bool is_active()                 { return mau_mapram_reg_.is_active(); }
    inline bool is_synth2port_mapram()      { return mau_mapram_reg_.is_synth2port_mapram(); }
    inline bool is_synth2port_home_mapram() { return mau_mapram_reg_.is_synth2port_home_mapram(); }
    inline bool is_synth2port_oflo_mapram() { return mau_mapram_reg_.is_synth2port_oflo_mapram(); }
    inline bool is_statistics_mapram()      { return mau_mapram_reg_.is_statistics_mapram(); }
    inline bool is_meter_mapram()           { return mau_mapram_reg_.is_meter_mapram(); }
    inline bool is_stateful_mapram()        { return mau_mapram_reg_.is_stateful_mapram(); }
    inline bool is_idletime_mapram()        { return mau_mapram_reg_.is_idletime_mapram(); }
    inline bool is_color_mapram()           { return mau_mapram_reg_.is_color_mapram(); }
    inline bool is_color_home_mapram()      { return mau_mapram_reg_.is_color_home_mapram(); }
    inline bool is_color_oflo_mapram()      { return mau_mapram_reg_.is_color_oflo_mapram(); }
    inline bool is_selector_mapram()        { return mau_mapram_reg_.is_selector_mapram(); }
    inline bool vpn_match(int vpn)          { return mau_mapram_reg_.vpn_match(vpn); }
    inline bool has_parity()                { return mau_mapram_reg_.has_parity(); }
    inline bool has_ecc()                   { return mau_mapram_reg_.has_ecc(); }
    inline int vpn_limit()                  { return mau_mapram_reg_.vpn_limit(); }
    inline int get_logical_table()          { return mau_mapram_reg_.get_logical_table(); }
    inline int get_type()                   { return mau_mapram_reg_.get_type(); }
    inline int get_vpn()                    { return mau_mapram_reg_.get_vpn(); }
    inline int get_vpn_min()                { return mau_mapram_reg_.get_vpn_min(); }
    inline int get_vpn_max()                { return mau_mapram_reg_.get_vpn_max(); }
    inline uint8_t get_order()              { return mau_mapram_reg_.get_order(); }
    inline void set_order(uint8_t ord)      { mau_mapram_reg_.set_order(ord); }
    inline uint32_t get_priority()          { return mau_mapram_reg_.get_priority(); }
    inline int get_color_alu_index()        { return mau_mapram_reg_.get_color_alu_index(); }
    inline int get_alu_index()              { return mau_mapram_reg_.get_alu_index(); }
    inline int get_meter_alu_index()        { return mau_mapram_reg_.get_meter_alu_index(); }
    inline bool check_ingress_egress(bool ingress) {
      return mau_mapram_reg_.check_ingress_egress(ingress);
    }

    // IDLETIME info
    inline uint32_t idletime_addr()          { return mau_mapram_reg_.idletime_addr(); }
    inline bool idletime_enabled()           { return true; }
    inline bool idletime_notify_enabled()    { return mau_mapram_reg_.idletime_notify_enabled(); }
    inline int idletime_bus()                { return mau_mapram_reg_.idletime_bus(); }
    inline int idletime_bitwidth()           { return mau_mapram_reg_.idletime_bitwidth(); }
    inline bool is_valid_idletime_mode()     { return mau_mapram_reg_.is_valid_idletime_mode(); }
    inline int idletime_type()               { return mau_mapram_reg_.idletime_type(); }
    inline int idletime_width()              { return mau_mapram_reg_.idletime_width(); }
    inline int idletime_nentries()           { return mau_mapram_reg_.idletime_nentries(); }
    inline int idletime_dump_width()         { return mau_mapram_reg_.idletime_dump_width(); }
    inline bool idletime_2way()              { return mau_mapram_reg_.idletime_2way(); }
    inline bool idletime_perflow()           { return mau_mapram_reg_.idletime_perflow(); }
    inline int idletime_offset(int idx)      { return mau_mapram_reg_.idletime_offset(idx); }
    inline int idletime_dump_offset(int idx) { return mau_mapram_reg_.idletime_dump_offset(idx); }
    inline uint8_t idletime_rd_clear_val()   { return mau_mapram_reg_.idletime_rd_clear_val(); }

    inline uint32_t color_read_addr(uint8_t* addrtype)    { return mau_mapram_reg_.color_read_addr(addrtype); }

    inline void read(const int index, uint64_t *data0, uint64_t *data1, uint64_t T) {
      if (is_color_mapram()) {
        if (T > UINT64_C(0))
          process_pending_color_writes(T);
        else
          process_pending_color_writes();
      }
      BitVector<kMapramWidth> val;
      if (get(index, &val)) {
        if (data0 != NULL) *data0 = val.get_word(0);
        if (data1 != NULL) *data1 = UINT64_C(0);
      }
    }
    inline void write(const int index, uint64_t data0, uint64_t data1, uint64_t T) {
      // TODO:T: Temporary logic till DV swaps over to using explicit T value
      uint64_t Tdata = (kMapramWriteData1HasTime) ?data1 :UINT64_C(0);
      uint64_t Twrite = (T > Tdata) ?T :Tdata;
      if (is_color_mapram()) {
        if (Twrite > UINT64_C(0))
          process_pending_color_writes(Twrite);
        else
          process_pending_color_writes();
      }
      BitVector<kMapramWidth> val;
      val.set_word(data0, 0);
      //val.set_word(data1, 64);
      // Some chips (eg TofinoB0) allows mapram physical write to be a masked write
      uint64_t write_mask = get_write_mask(data0, data1);
      if (write_mask == kMapramMask) {
        set(index, val);
      } else {
        BitVector<kMapramWidth> mask;
        mask.set_word(write_mask, 0);
        set_masked(index, val, mask);
      }
    }
    inline bool get(const int index, BitVector<kMapramWidth> *val) const {
      RMT_ASSERT(val != NULL);
      RMT_ASSERT((index >= 0) && (index < kMapramEntries));
      return mapram_.get(index, val);
    }
    inline void set(const int index, const BitVector<kMapramWidth> &val) {
      RMT_ASSERT((index >= 0) && (index < kMapramEntries));
      mapram_.set(index, val);
    }
    inline void set_masked(const int index, const BitVector<kMapramWidth> &val,
                           const BitVector<kMapramWidth> &mask) {
      RMT_ASSERT((index >= 0) && (index < kMapramEntries));
      mapram_.set_masked(index, val, mask);
    }
    inline uint64_t get_word(const int index, int off, int nbits) const {
      RMT_ASSERT((index >= 0) && (index < kMapramEntries));
      return mapram_.get_word(index, off, nbits);
    }
    inline void set_word(const int index, int off, int nbits, uint64_t word) {
      RMT_ASSERT((index >= 0) && (index < kMapramEntries));
      mapram_.set_word(index, off, nbits, word);
    }


    inline uint8_t get_synth2port_vpn(const int index) const {
      static_assert( (kMapramVpnBits < 8), "VPN must fit in uint8_t");
      int off = 0; // Maprams are 1024x10 since regs_13957_mau_dev so contain single VPN
      // In the case of Synth2Port access we return INVERTED MASKED mapram word as VPN
      uint64_t word = mapram_.get_word(index, off, kMapramWidth);
      return static_cast<uint8_t>(~word & static_cast<uint64_t>(kMapramVpnMask));
    }
    inline void set_synth2port_vpn(const int index, const uint8_t vpn) {
      int off = 0; // Maprams are 1024x10 since regs_13957_mau_dev so contain single VPN
      // In the case of Synth2Port access we store INVERTED MASKED VPN as mapram word
      uint64_t word = static_cast<uint64_t>(~vpn & kMapramVpnMask);
      mapram_.set_word(index, off, kMapramWidth, word);
    }


    inline uint32_t make_match_address(const int match_index, const int match_entry) {
      uint16_t vpn = 0;
      // match addr is 19 bits - vpn<9>index<10>
      return static_cast<uint32_t>(((vpn & 0x1FF) << 10) | ((match_index & 0x3FF)) << 0);
    }

    uint64_t get_write_mask(uint64_t data0, uint64_t data1); // Specialised PER-CHIP
    void reset_resources();
    void idletime_tally(bool read, bool written);
    void idletime_notify(int index, uint64_t msg, int addr_off);
    void idletime_fsm_notify(int index, int subword, bool active, bool idle);
    void idletime_dump_notify(int index, uint8_t data);
    bool idletime_sweep(int index, int subword, int off, int nbits, bool twoway,
                        uint64_t max_val, uint64_t incr_eq0, uint64_t incr_ne0);
    bool idletime_sweep(int index, int subword);
    bool idletime_sweep_one(int index, int subword);
    bool idletime_sweep_word(int index);
    bool idletime_sweep_all();
    void idletime_active_one(int index, int subword);
    uint64_t idletime_read_word(int index, bool clear);
    bool idletime_check_width(uint32_t addr, int *width);
    void idletime_write_one(int index, int rd_subword, int wr_subword,
                            int width, uint64_t data);

    void idletime_dump_one(int index, bool clear);
    void idletime_dump_all(bool clear);
    bool idletime_handles_addr(uint32_t addr);
    void idletime_handle(MauExecuteState *state, uint32_t addr);

    void run_read_idletime(MauExecuteState *state);
    void run_write_idletime(MauExecuteState *state);
    void run_read_color(MauExecuteState *state);
    void run_write_color(MauExecuteState *state);
    void run_read(MauExecuteState *state);
    void run_write(MauExecuteState *state);

    void deferred_update_color(uint32_t addr, uint8_t color,
                               uint64_t time_now, uint64_t time_of_write);
    void process_pending_color_writes();


 private:
    void update_color(uint32_t addr, uint8_t color);
    void process_pending_color_writes(uint64_t relative_time);
    void process_pending_color_writes(MauExecuteState* state);


 private:
    int                                row_index_;
    int                                col_index_;
    int                                mapram_index_;
    int                                logcol_index_;
    bool                               lhs_;
    int                                sweep_index_;
    int                                sweep_subword_;
    bool                               notify_error_;
    bool                               idle_;
    MauLogicalRow                     *logrow_;
    MauSramRow                        *physical_row_;
    MauSram                           *sram_;
    Sram<kMapramEntries,kMapramWidth>  mapram_;
    MauMapramReg                       mau_mapram_reg_;
  };
}
#endif // _SHARED_MAU_MAPRAM_
