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
#include <mau-lookup-result.h>
#include <mau-result-bus.h>


namespace MODEL_CHIP_NAMESPACE {

const char *MauLookupResult::kSelNameTab[] = {
  "IMM_DATA", "INSTR", "ACT_DATA", "STATS", "METER", "IDLE", "NXT_TAB", "SEL_LEN"
};
const uint8_t MauLookupResult::kAddrMaskWidthTab[] = {
  Address::kImmDataWidth, Address::kActionInstrWidth,
  Address::kActionAddrWidth, Address::kStatsAddrWidth,
  Address::kMeterAddrWidth, Address::kIdletimeAddrWidth,
  Address::kNxtTabWidth, Address::kSelectorLenWidth
};
// Note MeterAddrWidth set to be 24 for moment (NOT 27)
const uint8_t MauLookupResult::kAddrWidthTab[] = {
  Address::kImmDataWidth, Address::kActionInstrWidth,
  Address::kActionAddrWidth, Address::kStatsAddrWidth,
  Address::kMeterAddrWidth-Address::kMeterAddrTypeWidth, Address::kIdletimeAddrWidth,
  Address::kNxtTabWidth, Address::kSelectorLenWidth
};
const uint8_t MauLookupResult::kAddrPfePosTab[] = {
  Address::kImmDataPfePos, Address::kActionInstrPfePos,
  Address::kActionAddrPfePos, Address::kStatsAddrPfePos,
  Address::kMeterAddrPfePos, Address::kIdletimeAddrPfePos,
  Address::kNxtTabPfePos, Address::kSelectorLenPfePos
};
const uint8_t MauLookupResult::kAddrOpPosTab[] = {
  Address::kImmDataOpPos, Address::kActionInstrOpPos,
  Address::kActionAddrOpPos, Address::kStatsAddrOpPos,
  Address::kMeterAddrOpPos, Address::kIdletimeAddrOpPos,
  Address::kNxtTabOpPos, Address::kSelectorLenOpPos
};
//                                                       ImmDat Instr   Act Stats Meter  Idle NxtTb SelLn
const uint8_t  MauLookupResult::kPhysBusPadTab[]       = {    0,    0,    5,    7,   23,    4,    0,    0 };
const uint8_t  MauLookupResult::kPhysPerentryBitsTab[] = {    0,    1,    1,    1,    1,    1,    0,    0 }; // currently only used for *copying* PFE bit
const uint8_t  MauLookupResult::kPhysOpBitsTab[]       = {    0,    0,    0,    0, Address::kMeterAddrTypeWidth, 0, 0, 0 }; // currently only used for *copying* MeterType bits
const uint8_t  MauLookupResult::kPhysVpnshiftPosTab[]  = { 0xFF, 0xFF,   15, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }; // currently only used for huffman hole in actions
const uint8_t  MauLookupResult::kPhysSwizzleBitsTab[]  = {    0,    0,    0,    2,    0,    0,    0,    0 }; // currently only used for stats
const uint8_t  MauLookupResult::kPhysSwizzleToPosTab[] = { 0xFF, 0xFF, 0xFF,    0, 0xFF, 0xFF, 0xFF, 0xFF };
const uint32_t MauLookupResult::kPhysDfltMaskTab[]     = {    0,    0,    0,    0,    0,    0,    0, 0xFF }; // 0=>all ones
const bool     MauLookupResult::kLogDfltBeforeMaskTab[]= {false,false,false,false,false,false, true,false }; // Logical space, OR dflt before AND mask?
const uint32_t MauLookupResult::kLogDfltBfMaskMaskTab[]= {    0,    0,    0,    0,    0,    0, 0xFF,    0 }; // 0=>all ones



const char *MauLookupResult::get_sel_name(int sel) {
  RMT_ASSERT((sel >= kSelMin) && (sel <= kSelMax));
  return kSelNameTab[sel];
}
void MauLookupResult::get_tind_offset_nbits(MauResultBus *cnf,
                                            int sel, int busIndex, int xm_tm, int match,
                                            uint32_t matchAddr, uint8_t *off, uint8_t *nbits) {
  RMT_ASSERT ((off != NULL) && (nbits != NULL));
  // default values
  *nbits = kTindOutputBusWidth;
  *off   = 0;
  // Figure out which subword bits to get from TIND bus
  uint8_t tind_datasel = cnf->get_tind_ram_data_size(busIndex);
  if ((tind_datasel < 1) || (tind_datasel > 5)) return;
  int subword = Address::tcam_match_addr_get_subword(matchAddr, tind_datasel-1);
  *nbits = 2<<tind_datasel; // 4,8,16,32,64
  *off = *nbits * subword;
  // the correct 64 bits have already been placed on the tind bus, so
  //  just use the offset within the 64 bits
  if (*off >= 64) *off -= 64;
}
bool MauLookupResult::get_tind_bus(MauResultBus *cnf,
                                   int sel, int busIndex, int xm_tm, int match,
                                   BitVector<kTableResultBusWidth> *bus) {
  RMT_ASSERT(mau_ != NULL);
  MauSramRow *row = mau_->sram_row_lookup(busIndex >> 1);
  RMT_ASSERT(row != NULL);
  RMT_ASSERT((busIndex >= 0) && (busIndex < kTindOutputBuses));
  int bus01 = busIndex & 0x1;
  RMT_ASSERT((bus01 == 0) || (bus01 == 1));

  BitVector<kTindOutputBusWidth> tind_bus(UINT64_C(0));
  uint8_t tind_offset, tind_nbits;
  uint32_t tcam_match_addr;

  tcam_match_addr = row->get_tcam_match_addr(bus01); // From row not result
  bool bus_used = row->get_tind_output_bus(bus01, &tind_bus);
  get_tind_offset_nbits(cnf, sel, busIndex, xm_tm, match,
                        tcam_match_addr, &tind_offset, &tind_nbits);

  bus->set_word(tind_bus.get_word(tind_offset, tind_nbits), 0);
  bus->set_word(static_cast<uint64_t>(tcam_match_addr), kTableResultMatchAddrPos);
  if (ltab_ != NULL)
    RMT_LOG_OBJ(ltab_, RmtDebug::verbose(RmtDebug::kRmtDebugMauLogicalTableFindActions),
                "get_tind_bus(%s) R=%d,bus=%d matchAddr=0x%08x tindBus=%s\n",
                get_sel_name(sel), (busIndex >> 1), bus01, tcam_match_addr, bus->to_string().c_str());
  return bus_used;
}
bool MauLookupResult::get_match_bus(MauResultBus *cnf,
                                    int sel, int busIndex, int xm_tm, int match,
                                    BitVector<kTableResultBusWidth> *bus) {
  RMT_ASSERT(mau_ != NULL);
  MauSramRow *row = mau_->sram_row_lookup(busIndex >> 1);
  RMT_ASSERT(row != NULL);
  RMT_ASSERT((busIndex >= 0) && (busIndex < kMatchOutputBuses));
  int bus01 = busIndex & 0x1;
  RMT_ASSERT((bus01 == 0) || (bus01 == 1));
  return row->get_match_output_bus(bus01, bus);
}
bool MauLookupResult::payload_shifter_enabled(MauResultBus *cnf,
                                              int sel, int busIndex, int xm_tm, int match) {
  if ((match >= 0) && (kRelaxPayloadShifterEnabledCheck)) return true;
  switch (sel) {
    case kSelImmData: return cnf->get_imm_data_payload_shifter_enable(busIndex, xm_tm);
    case kSelInstr:   return cnf->get_act_instr_payload_shifter_enable(busIndex, xm_tm);
    case kSelActData: return cnf->get_act_data_addr_payload_shifter_enable(busIndex, xm_tm);
    case kSelStats:   return cnf->get_stats_addr_payload_shifter_enable(busIndex, xm_tm);
    case kSelMeter:   return cnf->get_meter_addr_payload_shifter_enable(busIndex, xm_tm);
    case kSelIdle:    return cnf->get_idletime_addr_payload_shifter_enable(busIndex, xm_tm);
    case kSelNxtTab:  return true;
    case kSelSelLen:  return true;
    default: RMT_ASSERT(0); break;
  }
}
bool MauLookupResult::get_bus(MauResultBus *cnf,
                              int sel, int busIndex, int xm_tm, int match,
                              BitVector<kTableResultBusWidth> *bus) {
  bool bus_used = false;
  if (payload_shifter_enabled(cnf, sel, busIndex, xm_tm, match)) {
    if (xm_tm == 0)
      bus_used = get_match_bus(cnf, sel, busIndex, xm_tm, match, bus);
    else
      bus_used = get_tind_bus(cnf, sel, busIndex, xm_tm, match, bus);
  } else {
    bus->fill_all_zeros();
  }
  return bus_used;
}
uint8_t MauLookupResult::get_phy_bus_padding(MauResultBus *cnf,
                                             int sel, int busIndex, int xm_tm, int match) {
  RMT_ASSERT((sel >= kSelMin) && (sel <= kSelMax));
  return kPhysBusPadTab[sel];
}
uint32_t MauLookupResult::get_phy_dflt_mask(MauResultBus *cnf,
                                            int sel, int busIndex, int xm_tm, int match) {
  RMT_ASSERT((sel >= kSelMin) && (sel <= kSelMax));
  return kPhysDfltMaskTab[sel] ? kPhysDfltMaskTab[sel] : 0xFFFFFFFF;
}
uint64_t MauLookupResult::get_bus_data(MauResultBus *cnf,
                                       int sel, int busIndex, int xm_tm, int match,
                                       BitVector<kTableResultBusWidth> *bus,
                                       uint8_t padding, uint8_t shift) {
  if ((sel == kSelNxtTab) && (xm_tm == 0)) {
    // Special case for nxt-tab - get from self
    return static_cast<uint64_t>(next_table_orig());
  } else {
    if (shift < padding)
      return bus->get_word(0) << (padding - shift);
    else
      return bus->get_word(shift - padding);
  }
}
uint8_t MauLookupResult::get_addr_mask_width(MauResultBus *cnf,
                                             int sel, int busIndex, int xm_tm, int match) {
  RMT_ASSERT((sel >= kSelMin) && (sel <= kSelMax));
  return kAddrMaskWidthTab[sel];
}
uint32_t MauLookupResult::get_addr_mask(MauResultBus *cnf,
                                        int sel, int busIndex, int xm_tm, int match) {
  return 0xFFFFFFFFu >> (32-get_addr_mask_width(cnf, sel, busIndex, xm_tm, match));
}
uint8_t MauLookupResult::get_addr_width(MauResultBus *cnf,
                                        int sel, int busIndex, int xm_tm, int match) {
  RMT_ASSERT((sel >= kSelMin) && (sel <= kSelMax));
  return kAddrWidthTab[sel];
}
uint8_t MauLookupResult::get_addr_pfe_pos(MauResultBus *cnf,
                                          int sel, int busIndex, int xm_tm, int match) {
  RMT_ASSERT((sel >= kSelMin) && (sel <= kSelMax));
  uint8_t addr_pfe_pos = kAddrPfePosTab[sel];
  if (addr_pfe_pos >= 32) RMT_ASSERT(kPhysPerentryBitsTab[sel] == 0);
  return addr_pfe_pos;
}
uint8_t MauLookupResult::get_addr_op_pos(MauResultBus *cnf,
                                          int sel, int busIndex, int xm_tm, int match) {
  RMT_ASSERT((sel >= kSelMin) && (sel <= kSelMax));
  uint8_t addr_op_pos = kAddrOpPosTab[sel];
  if (addr_op_pos >= 32) RMT_ASSERT(kPhysOpBitsTab[sel] == 0);
  return addr_op_pos;
}
uint8_t MauLookupResult::get_phy_shift(MauResultBus *cnf,
                                       int sel, int busIndex, int xm_tm, int match) {
  switch (sel) {
    case kSelImmData: return cnf->get_imm_data_shift(busIndex, xm_tm, match);
    case kSelInstr:   return cnf->get_act_instr_addr_shift(busIndex, xm_tm, match);
    case kSelActData: return cnf->get_act_data_addr_shift(busIndex, xm_tm, match);
    case kSelStats:   return cnf->get_stats_addr_shift(busIndex, xm_tm, match);
    case kSelMeter:   return cnf->get_meter_addr_shift(busIndex, xm_tm, match);
    case kSelIdle:    return cnf->get_idletime_addr_shift(busIndex, xm_tm, match);
    case kSelNxtTab:  return 0;
    case kSelSelLen:  return cnf->get_selectorlength_shift(busIndex, xm_tm);
    default: RMT_ASSERT(0); break;
  }
}
uint32_t MauLookupResult::get_phy_mask(MauResultBus *cnf,
                                       int sel, int busIndex, int xm_tm, int match) {
  switch (sel) {
    case kSelImmData: return cnf->get_imm_data_mask(busIndex, xm_tm, match);
    case kSelInstr:
      return static_cast<uint32_t>(cnf->get_act_instr_addr_mask(busIndex, xm_tm, match));
    case kSelActData: return cnf->get_act_data_addr_mask(busIndex, xm_tm, match);
    case kSelStats:   return cnf->get_stats_addr_mask(busIndex, xm_tm, match);
    case kSelMeter:   return cnf->get_meter_addr_mask(busIndex, xm_tm, match);
    case kSelIdle:    return cnf->get_idletime_addr_mask(busIndex, xm_tm, match);
    case kSelNxtTab:  return 0xFFFFFFFF;
    case kSelSelLen:
      return static_cast<uint32_t>(cnf->get_selectorlength_mask(busIndex, xm_tm));
    default: RMT_ASSERT(0); break;
  }
}
uint32_t MauLookupResult::get_phy_dflt(MauResultBus *cnf,
                                       int sel, int busIndex, int xm_tm, int match) {
  switch (sel) {
    case kSelImmData: return cnf->get_imm_data_dflt(busIndex, xm_tm, match);
    case kSelInstr:
      return static_cast<uint32_t>(cnf->get_act_instr_addr_dflt(busIndex, xm_tm, match));
    case kSelActData: return cnf->get_act_data_addr_dflt(busIndex, xm_tm, match);
    case kSelStats:   return cnf->get_stats_addr_dflt(busIndex, xm_tm, match);
    case kSelMeter:   return cnf->get_meter_addr_dflt(busIndex, xm_tm, match);
    case kSelIdle:    return cnf->get_idletime_addr_dflt(busIndex, xm_tm, match);
    case kSelNxtTab:  return 0u;
    case kSelSelLen:  return cnf->get_selectorlength_dflt(busIndex, xm_tm);
    default: RMT_ASSERT(0); break;
  }
}
uint8_t MauLookupResult::get_phy_vpnshift_pos(MauResultBus *cnf,
                                               int sel, int busIndex, int xm_tm, int match) {
  RMT_ASSERT((sel >= kSelMin) && (sel <= kSelMax));
  return kPhysVpnshiftPosTab[sel];
}
uint8_t MauLookupResult::get_phy_vpnshift(MauResultBus *cnf,
                                          int sel, int busIndex, int xm_tm, int match) {
  switch (sel) {
    case kSelImmData: return 0;
    case kSelInstr:   return 0;
    case kSelActData: return cnf->get_act_data_addr_vpn_shift(busIndex, xm_tm, match);
    case kSelStats:   return 0;
    case kSelMeter:   return 0;
    case kSelIdle:    return 0;
    case kSelNxtTab:  return 0;
    case kSelSelLen:  return 0;
    default: RMT_ASSERT(0); break;
  }
}
bool MauLookupResult::get_phy_vpnshift_ins_zeros(MauResultBus *cnf,
                                                 int sel, int busIndex, int xm_tm, int match,
                                                 uint8_t *pos, uint8_t *n_bits) {
  RMT_ASSERT((pos != NULL) && (n_bits != NULL));
  uint8_t zero_bits = get_phy_vpnshift(cnf, sel, busIndex, xm_tm, match);
  uint8_t zero_pos = get_phy_vpnshift_pos(cnf, sel, busIndex, xm_tm, match);
  uint8_t addr_width = get_addr_width(cnf, sel, busIndex, xm_tm, match);
  if ((zero_pos >= addr_width) || (zero_bits == 0)) return false;
  *pos = zero_pos;
  *n_bits = zero_bits;
  return true;
}
bool MauLookupResult::get_phy_ins_zeros(MauResultBus *cnf,
                                        int sel, int busIndex, int xm_tm, int match,
                                        uint8_t *pos, uint8_t *n_bits) {
  if (sel == kSelActData)
    return get_phy_vpnshift_ins_zeros(cnf, sel, busIndex, xm_tm, match,
                                      pos, n_bits);
  else
    return false;
}
uint8_t MauLookupResult::get_phy_perentry_bits(MauResultBus *cnf,
                                              int sel, int busIndex, int xm_tm, int match) {
  RMT_ASSERT((sel >= kSelMin) && (sel <= kSelMax));
  return kPhysPerentryBitsTab[sel];
}
uint8_t MauLookupResult::get_phy_perentry_pos(MauResultBus *cnf,
                                              int sel, int busIndex, int xm_tm, int match) {
  uint8_t pos = 0xFF;
  switch (sel) {
    case kSelImmData: pos = 0xFF; break;
    case kSelInstr:   pos = cnf->get_act_instr_addr_perentry_enable(busIndex, xm_tm, match); break;
    case kSelActData: pos = cnf->get_act_data_addr_perentry_enable(busIndex, xm_tm, match);  break;
    case kSelStats:   pos = cnf->get_stats_addr_perentry_enable(busIndex, xm_tm, match);     break;
    case kSelMeter:   pos = cnf->get_meter_addr_perentry_enable(busIndex, xm_tm, match);     break;
    case kSelIdle:    pos = cnf->get_idletime_addr_perentry_enable(busIndex, xm_tm, match);  break;
    case kSelNxtTab:  pos = 0xFF; break;
    case kSelSelLen:  pos = 0xFF; break;
    default: RMT_ASSERT(0); break;
  }
  if ((sel == kSelInstr) && (has_bitmask_ops())) {
    uint8_t pfe_pos_in_addr = get_addr_pfe_pos(cnf, sel, busIndex, xm_tm, match);
    if (pos != pfe_pos_in_addr) {
      RMT_LOG_OBJ(ltab_, RmtDebug::error(RmtDebug::kRmtDebugMauLogicalTableFindActions),
                  "get_phy_per_entry_pos(%s): LT %d using bitmask ops but "
                  "perentry_en_mux_ctl[%d][%d]=%d is NOT default PFE pos=%d\n",
                  get_sel_name(sel), logical_table(), busIndex, xm_tm, pos, pfe_pos_in_addr);
    }
  }
  return pos;
}
uint32_t MauLookupResult::get_pfe(MauResultBus *cnf,
                                  int sel, int busIndex, int xm_tm, int match,
                                  uint8_t padding, uint8_t shift, uint32_t dflt) {
  RMT_ASSERT((padding < 128) && (shift < 128)); // Check sane-ish
  uint8_t perentry_bits = get_phy_perentry_bits(cnf, sel, busIndex, xm_tm, match);
  uint8_t perentry_pos = get_phy_perentry_pos(cnf, sel, busIndex, xm_tm, match);
  uint8_t pfe_pos_in_addr = get_addr_pfe_pos(cnf, sel, busIndex, xm_tm, match);
  RMT_ASSERT(perentry_bits <= 1);
  if ((perentry_bits == 0) || (perentry_pos >= 128)) return 0u;  // No PFE

  uint32_t way1 = 0u, way2 = 0u;
  bool relax = kRelaxLookupShiftPfePosCheck;
  BitVector<kTableResultBusWidth> bus;
  bool bus_used = get_bus(cnf, sel, busIndex, xm_tm, match, &bus);

  // Generically form  : { payload[63-pad:0],payload[63:64-pad] }
  // So for Meters say : { payload[    40:0],payload[63:41]     }
  BitVector<64> barrel(UINT64_C(0));
  //          64b_WORD   SRC              OFFSET        SIZE
  uint64_t barrel_lsbs = bus.get_word(64-padding,    padding);
  uint64_t barrel_msbs = bus.get_word(         0, 64-padding);
  uint8_t  pos1 = (perentry_pos + shift) % 64;
  // DST             64b_WORD   OFFSET        SIZE
  barrel.set_word(barrel_lsbs,       0,    padding);
  barrel.set_word(barrel_msbs, padding, 64-padding);
  way1 = static_cast<uint32_t>(barrel.get_word(pos1,perentry_bits)) << pfe_pos_in_addr;

  uint8_t pos2 = 255, cond = 0;
  if (perentry_pos + shift < padding) {
    pos2 = perentry_pos + shift; cond = 0;
    bool shift_en = payload_shifter_enabled(cnf, sel, busIndex, xm_tm, -1);
    bool dflt_pfe = ((dflt & (1u << pfe_pos_in_addr)) != 0u);
    // Don't complain if payload_shifter disabled, if PFE set in the dflt register or if bus unused
    if (shift_en && !dflt_pfe && bus_used) {
      RMT_LOG_OBJ(ltab_, RmtDebug::error(RmtDebug::kRmtDebugMauLogicalTableFindActions,relax),
                  "get_pfe(%s): PFE pos (%d+%d) within LSB zero-padding %d "
                  "[Pad=%d Shiftcount=%d]\n",
                  get_sel_name(sel), perentry_pos, shift, padding, padding, shift);
      if (!relax) { THROW_ERROR(-2); } // For DV
    }
    way2 = 0u;
  } else if (perentry_pos + shift >= padding + 64) {
    pos2 = perentry_pos + shift - padding - 64; cond = 2;
    bool shift_en = payload_shifter_enabled(cnf, sel, busIndex, xm_tm, -1);
    bool dflt_pfe = ((dflt & (1u << pfe_pos_in_addr)) != 0u);
    // Don't complain if payload_shifter disabled, if PFE set in the dflt register or if bus unused
    if (shift_en && !dflt_pfe && bus_used) {
      RMT_LOG_OBJ(ltab_, RmtDebug::error(RmtDebug::kRmtDebugMauLogicalTableFindActions,relax),
                  "get_pfe(%s): PFE pos (%d+%d) beyond payload(%d)+pad(%d) "
                  "Reduced mod64 to %d  [Pad=%d Shiftcount=%d]\n",
                  get_sel_name(sel), perentry_pos, shift, 64, padding, pos2, padding, shift);
      if (!relax) { THROW_ERROR(-2); } // For DV
    }
    way2 = static_cast<uint32_t>(bus.get_bit(pos2)) << pfe_pos_in_addr;
  } else {
    pos2 = perentry_pos + shift - padding; cond = 1;
    way2 = static_cast<uint32_t>(bus.get_bit(pos2)) << pfe_pos_in_addr;
  }
  if (way1 != way2) {
    const char *where[3] = { "InLSBs", "InPayload", "BeyondPayload" };
    RMT_LOG_OBJ(ltab_, RmtDebug::verbose(RmtDebug::kRmtDebugMauLogicalTableFindActions),
                "get_pfe(%s): DIFF: Way1=0x%08x,Way2=0x%08x PfePos=%d Shift=%d Pad=%d (%s Pos1=%d,Pos2=%d)\n",
                get_sel_name(sel), way1, way2, perentry_pos, shift, padding, where[cond], pos1, pos2);
  }
  return way1;
}
bool MauLookupResult::get_phy_perentry_copy_bits(MauResultBus *cnf,
                                                 int sel, int busIndex, int xm_tm, int match,
                                                 uint8_t *from_pos, uint8_t *to_pos, uint8_t *n_bits,
                                                 bool *use_pre_ins_data) {
  RMT_ASSERT((from_pos != NULL) && (to_pos != NULL) && (n_bits != NULL));
  uint8_t perentry_bits = get_phy_perentry_bits(cnf, sel, busIndex, xm_tm, match);
  uint8_t perentry_pos = get_phy_perentry_pos(cnf, sel, busIndex, xm_tm, match);
  uint8_t addr_width = get_addr_width(cnf, sel, busIndex, xm_tm, match);
  if ((perentry_bits == 0) || (perentry_pos >= addr_width)) return false;
  *from_pos = perentry_pos;
  *to_pos = addr_width-perentry_bits;
  *n_bits = perentry_bits;
  *use_pre_ins_data = true;
  return true;
}
uint8_t MauLookupResult::get_phy_op_bits(MauResultBus *cnf,
                                         int sel, int busIndex, int xm_tm, int match) {
  RMT_ASSERT((sel >= kSelMin) && (sel <= kSelMax));
  return kPhysOpBitsTab[sel];
}
uint8_t MauLookupResult::get_phy_op_pos(MauResultBus *cnf,
                                        int sel, int busIndex, int xm_tm, int match) {
  switch (sel) {
    case kSelImmData: return 0xFF;
    case kSelInstr:   return 0xFF;
    case kSelActData: return 0xFF;
    case kSelStats:   return 0xFF;
    case kSelMeter:   return cnf->get_meter_adr_type_position(busIndex, xm_tm);
    case kSelIdle:    return 0xFF;
    case kSelNxtTab:  return 0xFF;
    case kSelSelLen:  return 0xFF;
    default: RMT_ASSERT(0); break;
  }
}
uint32_t MauLookupResult::get_op(MauResultBus *cnf,
                                 int sel, int busIndex, int xm_tm, int match,
                                 uint8_t padding, uint8_t shift, uint32_t dflt) {
  RMT_ASSERT((padding < 128) && (shift < 128)); // Check sane-ish
  uint8_t op_bits = get_phy_op_bits(cnf, sel, busIndex, xm_tm, match);
  uint8_t op_pos = get_phy_op_pos(cnf, sel, busIndex, xm_tm, match);
  uint8_t op_pos_in_addr = get_addr_op_pos(cnf, sel, busIndex, xm_tm, match);
  if ((op_bits == 0) || (op_pos >= 128)) return 0u;  // No OP for this type
  RMT_ASSERT(op_pos_in_addr + op_bits <= 32);

  uint32_t way1 = 0u, way2 = 0u;
  bool relax = kRelaxLookupShiftOpPosCheck;
  BitVector<kTableResultBusWidth> bus;
  bool bus_used = get_bus(cnf, sel, busIndex, xm_tm, match, &bus);

  // Generically form  : { payload[63-pad:0],payload[63:0],payload[63:64-pad] }
  // So for Meters say : { payload[    40:0],payload[63:0],payload[63:41]     }
  BitVector<128> barrel(UINT64_C(0));
  //          64b_WORD   SRC              OFFSET        SIZE
  uint64_t barrel_lsbs = bus.get_word(64-padding,    padding);
  uint64_t   payload64 = bus.get_word(         0,         64);
  uint64_t barrel_msbs = bus.get_word(         0, 64-padding);
  uint8_t  pos1 = (op_pos + shift) % 64;
  // DST             64b_WORD      OFFSET        SIZE
  barrel.set_word(barrel_lsbs,          0,    padding);
  barrel.set_word(  payload64,    padding,         64);
  barrel.set_word(barrel_msbs, padding+64, 64-padding);
  way1 = static_cast<uint32_t>(barrel.get_word(pos1,op_bits)) << op_pos_in_addr;

  if (op_pos + shift < padding) {
    bool shift_en = payload_shifter_enabled(cnf, sel, busIndex, xm_tm, -1);
    bool dflt_op = ( ( dflt &  ((0xFFFFFFFFu >> (32-op_bits)) << op_pos_in_addr) ) != 0u );
    // Don't complain if payload_shifter disabled, OP set in the dflt register or bus unused
    if (shift_en && !dflt_op && bus_used) {
      RMT_LOG_OBJ(ltab_, RmtDebug::error(RmtDebug::kRmtDebugMauLogicalTableFindActions,relax),
                  "get_op(%s): OP pos (%d+%d) within LSB zero-padding %d "
                  "[Pad=%d Shiftcount=%d]\n",
                  get_sel_name(sel), op_pos, shift, padding, padding, shift);
      if (!relax) { THROW_ERROR(-2); } // For DV
    }
    way2 = 0u; // Always return 0 in this case
  } else if (op_pos + shift >= padding + 64) {
    uint8_t pos = op_pos + shift - padding - 64;
    bool shift_en = payload_shifter_enabled(cnf, sel, busIndex, xm_tm, -1);
    bool dflt_op = ( ( dflt &  ((0xFFFFFFFFu >> (32-op_bits)) << op_pos_in_addr) ) != 0u );
    // Don't complain if payload_shifter disabled, OP set in the dflt register or bus unused
    if (shift_en && !dflt_op && bus_used) {
      RMT_LOG_OBJ(ltab_, RmtDebug::error(RmtDebug::kRmtDebugMauLogicalTableFindActions,relax),
                  "get_op(%s): OP pos (%d+%d) beyond payload(%d)+pad(%d) "
                  "Reduced mod64 to %d  [Pad=%d Shiftcount=%d]\n",
                  get_sel_name(sel), op_pos, shift, 64, padding, pos, padding, shift);
      if (!relax) { THROW_ERROR(-2); } // For DV
    }
    RMT_ASSERT(pos + op_bits <= 64);
    way2 = static_cast<uint32_t>(bus.get_word(pos,op_bits)) << op_pos_in_addr;
  } else {
    uint8_t pos = op_pos + shift - padding;
    // Maybe duplicate bus lo word into hi word to emulate wrap around
    if (pos + op_bits > 64) bus.set_word(bus.get_word(0), 64);
    way2 = static_cast<uint32_t>(bus.get_word(pos,op_bits)) << op_pos_in_addr;
  }
  if (way1 != way2) {
    RMT_LOG_OBJ(ltab_, RmtDebug::verbose(RmtDebug::kRmtDebugMauLogicalTableFindActions),
                "get_op(%s): DIFF: Way1=0x%08x,Way2=0x%08x OpPos=%d Shift=%d Pad=%d\n",
                get_sel_name(sel), way1, way2, op_pos, shift, padding);
  }
  return way1;
}

uint8_t MauLookupResult::get_phy_swizzlemode(MauResultBus *cnf,
                                             int sel, int busIndex, int xm_tm, int match) {
  switch (sel) {
    case kSelImmData: return 0xFF;
    case kSelInstr:   return 0xFF;
    case kSelActData: return 0xFF;
    case kSelStats:   return cnf->get_stats_addr_hole_swizzle(busIndex, xm_tm, match);
    case kSelMeter:   return 0xFF;
    case kSelIdle:    return 0xFF;
    case kSelNxtTab:  return 0xFF;
    case kSelSelLen:  return 0xFF;
    default: RMT_ASSERT(0); break;
  }
}
uint8_t MauLookupResult::get_phy_swizzle_bits(MauResultBus *cnf,
                                             int sel, int busIndex, int xm_tm, int match) {
  RMT_ASSERT((sel >= kSelMin) && (sel <= kSelMax));
  return kPhysSwizzleBitsTab[sel];
}
uint8_t MauLookupResult::get_phy_swizzle_to_pos(MauResultBus *cnf,
                                                int sel, int busIndex, int xm_tm, int match) {
  RMT_ASSERT((sel >= kSelMin) && (sel <= kSelMax));
  return kPhysSwizzleToPosTab[sel];
}
uint8_t MauLookupResult::get_phy_swizzle_from_pos(MauResultBus *cnf,
                                                  int sel, int busIndex, int xm_tm, int match) {
  RMT_ASSERT((sel >= kSelMin) && (sel <= kSelMax));
  if (sel != kSelStats) return 0xFF;
  // Retval = XM_or_TM[7:4] SwizzleMode[3:0], hence mask with 0xF
  uint8_t swizzle_mode = (get_phy_swizzlemode(cnf, sel, busIndex, xm_tm, match) & 0xF);
  if (!((swizzle_mode == 1) || (swizzle_mode == 2))) return 0xFF; // No swizzle
  if (xm_tm == 0) return 16-swizzle_mode; // For XM mode1 => 15,mode2 => 14
  if (xm_tm == 1) return 15-swizzle_mode; // For TM mode1 => 14,mode2 => 13
  return 0xFF;
}

bool MauLookupResult::get_phy_swizzle_move_bits(MauResultBus *cnf,
                                                int sel, int busIndex, int xm_tm, int match,
                                                uint8_t *from_pos, uint8_t *to_pos, uint8_t *n_bits) {
  RMT_ASSERT((from_pos != NULL) && (to_pos != NULL) && (n_bits != NULL));
  if (sel != kSelStats) return false; // Only Stats right now
  uint8_t swiz_bits = get_phy_swizzle_bits(cnf, sel, busIndex, xm_tm, match);
  uint8_t swiz_from_pos = get_phy_swizzle_from_pos(cnf, sel, busIndex, xm_tm, match);
  uint8_t swiz_to_pos = get_phy_swizzle_to_pos(cnf, sel, busIndex, xm_tm, match);
  uint8_t addr_width = get_addr_width(cnf, sel, busIndex, xm_tm, match);
  if ((swiz_from_pos >= addr_width) || (swiz_to_pos >= addr_width) || (swiz_bits == 0))
    return false;
  *from_pos = swiz_from_pos;
  *to_pos = swiz_to_pos;
  *n_bits = swiz_bits;
  return true;
}
bool MauLookupResult::get_phy_move_bits(MauResultBus *cnf,
                                        int sel, int busIndex, int xm_tm, int match,
                                        uint8_t *from_pos, uint8_t *to_pos, uint8_t *n_bits) {
  if (sel == kSelStats)
    return get_phy_swizzle_move_bits(cnf, sel, busIndex, xm_tm, match,
                                     from_pos, to_pos, n_bits);
  else
    return false;
}

uint32_t MauLookupResult::get_selector_action_address(Phv *phv, int logtab) {
  RMT_ASSERT((mau_ != NULL) && (cnf_ != NULL));
  MauHashDistribution *mhd = mau_->mau_hash_dist();
  RMT_ASSERT(mhd != NULL);
  int alu = cnf_->get_meter_alu_for_logical_table(logtab);
  if (alu < 0) return 0u;
  uint32_t sel_len = get_phy_alu_data(cnf_, kSelSelLen, alu);
  return mhd->get_selector_action_address(phv, logtab, alu, sel_len);
}
uint32_t MauLookupResult::get_selector_address(Phv *phv, int logtab) {
  RMT_ASSERT((mau_ != NULL) && (cnf_ != NULL));
  uint32_t sel_addr = 0u;
  MauHashDistribution *mhd = mau_->mau_hash_dist();
  RMT_ASSERT(mhd != NULL);
  for (int alu: cnf_->get_meter_alus_for_logical_table(logtab)) {
    uint32_t sel_len = get_phy_alu_data(cnf_, kSelSelLen, alu);
    uint32_t new_addr = mhd->get_selector_address(phv, logtab, alu, sel_len);
    RMT_ASSERT((sel_addr == 0u) || (sel_addr == new_addr));
    sel_addr = new_addr;
  }
  return sel_addr;
}
uint32_t MauLookupResult::get_hash_distribution_result(MauResultBus *cnf,
                                                       int sel, int logtab, int match,
                                                       bool post_predication) {
  uint32_t data = 0u;
  uint32_t sel_act_data;
  switch (sel) {
    case kSelImmData:
      data = mau_->mau_hash_dist()->get_immediate_data_hash(phv_,logtab);
      break;
    case kSelActData:
      // HashDistribution changes as part of HyperDev transition
      //    Get action_address for logical table (see fig 6-90)
      //    Get SelLen using get_phy_alu_data and then...
      //      Get selector_action_addr for logtab
      //    And OR them together
      data = mau_->mau_hash_dist()->get_action_address(phv_, logtab);
      sel_act_data = get_selector_action_address(phv_, logtab);
      data |= Address::action_addr_get_vaddr(sel_act_data); // 22b so no PFE
      break;
    case kSelMeter:
      // HashDistribution changes as part of HyperDev transition
      //    Get meter_address for logical table (see fig 6-90)
      //    Get SelLen using get_phy_alu_data and then...
      //      Get selector_addr for logtab
      //    But only OR together if PFE, table_active and HIT
      //
      data = mau_->mau_hash_dist()->get_meter_address(phv_, logtab);

      // TODO: remove this old code
      // This is the old Tofino way of doing it. I think it is not ideal as it used the
      //   mau_meter_alu_to_logical_map register "which maps the selector action address
      //   from the meter ALU space to the logical space before OR'ing in the logical action address."
      //   So it is right to use it in the case above, but not here.
      // alu = cnf->get_meter_alu_for_logical_table(logtab);

      // ONLY do this bit if post_predication result requested
      if (post_predication) {
        uint32_t sel_data = get_selector_address(phv_, logtab);
        uint32_t alu_data = 0;

        // Only thing this ALU loop does differently to the one
        // in get_selector_address is check for HIT, pred=ON
        // (but could probably promote those checks to before the call)
        //
        for (int alu: cnf->get_meter_alus_for_logical_table(logtab)) {
          RMT_ASSERT(alu >= 0);

          // Insist that lookup has completed and a call to set_active()
          // has occurred as code here needs to know whether the table
          // is predicated ON or not
          RMT_ASSERT(active() || inactive());

          // TODO: remove this old code
          // This is an old check from Tofino, when it was getting the alu from mau_meter_alu_to_logical_map.
          //  Now the alu comes from mau_logical_to_meter_alu_map, which is where
          //  get_logical_table_for_meter_alu used to get its lt from, so this check no longer makes sense.
          //int logtab2 = cnf->get_logical_table_for_meter_alu(alu);
          // Also barf if config mismatch
          // This is NOT intended as a mechanism to allow LT X to gate selector_addr
          // for some other LT Y (see MikeF response in mau_hyperdev thread 13/9/2015)
          //if (logtab2 >= 0) RMT_ASSERT(logtab2 == logtab);

          // Only OR in if PFE set in orig meter data, table active AND hit
          // (we know we've had a HIT or this func wouldn't be called)
          uint32_t meter_data = get_log_data_hit(cnf, kSelMeter, logtab, match);
          if (Address::meter_addr_enabled(meter_data) && active()) { // && (logtab == logtab2))

            uint32_t new_data = Address::meter_addr_get_vaddr(sel_data); // 23b so no PFE
            // All ALUs must produce the same result
            RMT_ASSERT((alu_data==0) || (new_data==alu_data));
            alu_data = new_data;

            data |= new_data;
          }
        }
      }
      break;
    case kSelStats:
      data = mau_->mau_hash_dist()->get_stats_address(phv_,logtab);
      break;
    case kSelIdle:
    case kSelInstr:
    case kSelNxtTab:
    case kSelSelLen:
      // nothing to do
      break;
    default:
      RMT_ASSERT(0);
      break;
  }
  RMT_LOG_OBJ(ltab_, RmtDebug::verbose(),"get_hash_distribution_result(%s)=0x%08x\n",
              get_sel_name(sel), data);
  return data;
}

uint32_t MauLookupResult::get_phy_data(MauResultBus *cnf,
                                       int sel, int busIndex, int xm_tm, int match) {
  uint8_t pad = get_phy_bus_padding(cnf, sel, busIndex, xm_tm, match);
  uint8_t shift = get_phy_shift(cnf, sel, busIndex, xm_tm, match);
  uint32_t wdth = get_addr_width(cnf, sel, busIndex, xm_tm, match);
  uint32_t mask = get_phy_mask(cnf, sel, busIndex, xm_tm, match);
  uint32_t dflt = get_phy_dflt(cnf, sel, busIndex, xm_tm, match) &
                  get_phy_dflt_mask(cnf, sel, busIndex, xm_tm, match);
  BitVector<kTableResultBusWidth> bus;
  (void)get_bus(cnf, sel, busIndex, xm_tm, match, &bus);
  uint64_t data64 = get_bus_data(cnf, sel, busIndex, xm_tm, match, &bus, pad, shift);
  // Initially mask based on addrtype width - NB Meter only 24b, 3b MeterType ORed later
  uint32_t data32 = static_cast<uint32_t>(data64 &  (UINT64_C(0xFFFFFFFF) >> (32-wdth)));
  uint8_t from, to, nbits;
  RMT_LOG_OBJ(ltab_, RmtDebug::verbose(RmtDebug::kRmtDebugMauLogicalTableFindActions),
              "get_phy_data_bus(%s)=0x%016" PRIx64 " 0x%08x W=%d S=%d M=0x%08x DFLT=0x%08x\n",
              get_sel_name(sel), data64, data32, wdth, shift, mask, dflt);
  if (get_phy_ins_zeros(cnf, sel, busIndex, xm_tm, match, &to, &nbits)) {
    RMT_LOG_OBJ(ltab_, RmtDebug::verbose(RmtDebug::kRmtDebugMauLogicalTableFindActions),
                "get_phy_data_ins_zeros(%s)=0x%08x nbits=%d\n",
                get_sel_name(sel), data32, nbits);
    uint32_t lo_mask = (1u << to) - 1;
    data32 = ((data32 & ~lo_mask) << nbits) | (data32 & lo_mask);
  }
  RMT_LOG_OBJ(ltab_, RmtDebug::verbose(RmtDebug::kRmtDebugMauLogicalTableFindActions),
              "get_phy_data_post_ins(%s)=0x%08x\n", get_sel_name(sel), data32);
  uint32_t pre_mask_copy = 0u, post_mask_copy = 0u;
  // Use get_op to handle shift+pos being outside payload and wrapping around
  pre_mask_copy = get_op(cnf, sel, busIndex, xm_tm, match, pad, shift, dflt);
  uint32_t pfe = get_pfe(cnf, sel, busIndex, xm_tm, match, pad, shift, dflt);
  RMT_LOG_OBJ(ltab_, RmtDebug::verbose(RmtDebug::kRmtDebugMauLogicalTableFindActions),
              "get_phy_data_pre_mask(%s)=0x%08x preMaskOr=0x%08x mask=0x%08x "
              "postMaskOr=0x%08x(pfe=0x%08x copy=0x%08x dflt=0x%08x)\n", get_sel_name(sel), data32,
              pre_mask_copy, mask, (pfe|post_mask_copy|dflt), pfe, post_mask_copy, dflt);
  data32 |= pre_mask_copy;
  data32 &= mask;
  data32 |= pfe; // PFE bit ORd in post-mask
  data32 |= post_mask_copy;
  data32 |= dflt;
  RMT_LOG_OBJ(ltab_, RmtDebug::verbose(RmtDebug::kRmtDebugMauLogicalTableFindActions),
              "get_phy_data_post_mask_dflt(%s)=0x%08x\n", get_sel_name(sel), data32);
  if (get_phy_move_bits(cnf, sel, busIndex, xm_tm, match, &from, &to, &nbits)) {
    RMT_LOG_OBJ(ltab_, RmtDebug::verbose(RmtDebug::kRmtDebugMauLogicalTableFindActions),
                "get_phy_data_move_bits(%s)=0x%08x from=%d to=%d\n",
                get_sel_name(sel), data32, from, to);
    // Extract bits that are moving
    uint32_t move_bits = (data32 >> from) & ((1u << nbits) - 1);
    uint32_t below_mask = (1u << from) -1;
    uint32_t above_mask = ~((1u << (from+nbits)) -1);
    // Close up the gap occupied by the move_bits
    uint32_t unmove_bits = ((data32 & above_mask) >> nbits) | (data32 & below_mask);
    uint32_t lo_mask = (1u << to) - 1;
    // Open up a new gap to accommodate the move_bits
    uint32_t bits_and_gap = ((unmove_bits & ~lo_mask) << nbits) | (unmove_bits & lo_mask);
    data32 = (bits_and_gap) | (move_bits << to);
  }

  RMT_LOG_OBJ(ltab_, RmtDebug::verbose(RmtDebug::kRmtDebugMauLogicalTableFindActions),
              "get_phy_data_end(%s)=0x%08x\n", get_sel_name(sel), data32);
  return data32;
}
uint32_t MauLookupResult::get_phy_data(MauResultBus *cnf,
                                       int sel, int xm_tm, int match) {
  uint32_t data = 0u;
  int n_buses = (xm_tm == 0) ?kMatchOutputBuses :kTindOutputBuses;
  uint16_t buses = (xm_tm == 0) ?match_buses() :tind_buses();
  for (int b = 0; b < n_buses; b++) {
    if ((buses & (1<<b)) != 0) {
      RMT_LOG_OBJ(ltab_, RmtDebug::verbose(RmtDebug::kRmtDebugMauLogicalTableFindActions),
                  "get_phy_data(%s) Examining Bus=%d xm_tm=%d\n", get_sel_name(sel), b, xm_tm);
      data |= get_phy_data(cnf, sel, b, xm_tm, match);
    }
  }
  RMT_LOG_OBJ(ltab_, RmtDebug::verbose(RmtDebug::kRmtDebugMauLogicalTableFindActions),
              "get_phy_data_ALL(%s)=0x%08x\n", get_sel_name(sel), data);
  return data;
}
uint32_t MauLookupResult::get_phy_alu_data(MauResultBus *cnf,
                                           int sel, int xm_tm, int alu, int match) {
  uint32_t data = 0u;
  int n_buses = (xm_tm == 0) ?kMatchOutputBuses :kTindOutputBuses;
  uint16_t buses = cnf->get_meter_alu_buses(xm_tm, alu);
  for (int b = 0; b < n_buses; b++) {
    if ((buses & (1<<b)) != 0) {
      RMT_LOG_OBJ(ltab_, RmtDebug::verbose(RmtDebug::kRmtDebugMauLogicalTableFindActions),
                  "get_phy_alu_data(%s) Examining Bus=%d xm_tm=%d\n", get_sel_name(sel), b, xm_tm);
      data |= get_phy_data(cnf, sel, b, xm_tm, match);
    }
  }
  RMT_LOG_OBJ(ltab_, RmtDebug::verbose(RmtDebug::kRmtDebugMauLogicalTableFindActions),
              "get_phy_alu_data(%s)=0x%08x\n", get_sel_name(sel), data);
  return data;
}
uint32_t MauLookupResult::get_phy_alu_data(MauResultBus *cnf, int sel, int alu, int match) {
  uint32_t xm_data = get_phy_alu_data(cnf, sel, 0, alu, match);
  uint32_t tm_data = get_phy_alu_data(cnf, sel, 1, alu, match);
  RMT_LOG_OBJ(ltab_, RmtDebug::verbose(RmtDebug::kRmtDebugMauLogicalTableFindActions),
              "get_phy_alu_data(%s) XM=0x%08x TM=0x%08x ORed=0x%08x\n",
              get_sel_name(sel), xm_data, tm_data, (xm_data|tm_data));
  return xm_data | tm_data;
}
uint32_t MauLookupResult::get_phy_alu_data(MauResultBus *cnf, int sel, int alu) {
  return get_phy_alu_data(cnf, sel, alu, hitindex());
}



// LOGICAL SPACE funcs

bool MauLookupResult::flow_enabled(MauResultBus *cnf,
                                   int sel, int logtab, int xm_tm, int match,
                                   uint32_t data) {
  if (Address::kGlobalAddrEnable) return true;
  if ((sel == kSelInstr) && (has_bitmask_ops())) return true;
  uint8_t perentry_bits = get_phy_perentry_bits(cnf, sel, 99, xm_tm, match);
  uint8_t pfe_pos = get_addr_pfe_pos(cnf, sel, 99, xm_tm, match);
  return ((perentry_bits == 0) || ((data & (1u<<pfe_pos)) != 0u));
}
bool MauLookupResult::flow_enabled(MauResultBus *cnf, int sel, int logtab,
                                   uint32_t data) {
  return flow_enabled(cnf, sel, logtab, 99, 99, data);
}
bool MauLookupResult::flow_enabled_for_map(MauResultBus *cnf,
                                           int sel, int logtab, int xm_tm, int match,
                                           uint32_t data) {
  // flow_enabled_for_map() now only checked in conjunction with map_enabled_xm|tm()
  // PFE: see discussion XXX: post-hardening, PFE examined only for Instr.
  // PFE: this is part of the HyperDev transition
  switch (sel) {
    case kSelImmData: return true;
    case kSelInstr:   return flow_enabled(cnf, sel, logtab, xm_tm, match, data);
    case kSelActData: return true;
    case kSelStats:   return true;
    case kSelMeter:   return true;
    case kSelIdle:    return true;
    case kSelNxtTab:  return true;
    case kSelSelLen:  return true;
    default: RMT_ASSERT(0); break;
  }
}
bool MauLookupResult::flow_enabled_for_actionbit_map(MauResultBus *cnf,
                                                     int sel, int logtab,
                                                     int xm_tm, int match, uint32_t data) {
  // PFE: see discussion XXX: post-hardening, PFE never
  // PFE: affects actionbit_map so func always returns true
  return true;
}

uint32_t MauLookupResult::get_log_mask(MauResultBus *cnf,
                                       int sel, int logtab, int xm_tm, int match) {
  switch (sel) {
    case kSelImmData: return 0xFFFFFFFF;
    case kSelInstr:   return 0xFFFFFFFF;
    case kSelActData: return 0xFFFFFFFF;
    case kSelStats:   return 0xFFFFFFFF;
    case kSelMeter:   return 0xFFFFFFFF;
    case kSelIdle:    return 0xFFFFFFFF;
    case kSelNxtTab:  return static_cast<uint32_t>(cnf->get_nxt_tab_mask(logtab));
    case kSelSelLen:  return 0xFFFFFFFF;
    default: RMT_ASSERT(0); break;
  }
}
uint32_t MauLookupResult::get_log_mask_map(MauResultBus *cnf,
                                           int sel, int logtab, int xm_tm, int match) {
  // Use normal mask
  return get_log_mask(cnf, sel, logtab, xm_tm, match);
}
uint32_t MauLookupResult::get_log_dflt(MauResultBus *cnf,
                                       int sel, int logtab, int xm_tm, int match) {
  switch (sel) {
    case kSelImmData: return 0u;
    case kSelInstr:   return 0u;
    case kSelActData: return 0u;
    case kSelStats:   return 0u;
    case kSelMeter:   return 0u;
    case kSelIdle:    return 0u;
    case kSelNxtTab:  return static_cast<uint32_t>(cnf->get_nxt_tab_dflt(logtab));
    case kSelSelLen:  return 0u;
    default: RMT_ASSERT(0); break;
  }
}
bool MauLookupResult::get_log_dflt_before_mask(MauResultBus *cnf,
                                               int sel, int logtab, int xm_tm, int match) {
  RMT_ASSERT((sel >= kSelMin) && (sel <= kSelMax));
  return kLogDfltBeforeMaskTab[sel];
}
uint32_t MauLookupResult::get_log_dflt_before_mask_mask(MauResultBus *cnf,
                                                        int sel, int logtab, int xm_tm, int match) {
  RMT_ASSERT((sel >= kSelMin) && (sel <= kSelMax));
  return (kLogDfltBfMaskMaskTab[sel] != 0u) ? kLogDfltBfMaskMaskTab[sel] : 0xFFFFFFFFu;
}
bool MauLookupResult::map_enabled_xm(MauResultBus *cnf,
                                     int sel, int logtab, int xm_tm, int match) {
  RMT_ASSERT(xm_tm == 0);
  return ((sel == kSelInstr) && (cnf->get_act_instr_map_enable(logtab, xm_tm, match)));
}
bool MauLookupResult::map_enabled_tm(MauResultBus *cnf,
                                     int sel, int logtab, int xm_tm, int match) {
  RMT_ASSERT(xm_tm == 1);
  return ((sel == kSelInstr) && (cnf->get_act_instr_map_enable(logtab, xm_tm, match)));
}
bool MauLookupResult::actionbit_map_enabled(MauResultBus *cnf,
                                            int sel, int logtab, int xm_tm, int match) {
  // Bail immediately if not TM - MauResultBus funcs only have actionbit for TM (xm_tm==1)
  if (xm_tm != 1) return false;
  switch (sel) {
    case kSelImmData: return cnf->get_imm_data_actionbit_map_enable(logtab, xm_tm);
    case kSelInstr:   return cnf->get_act_instr_actionbit_map_enable(logtab, xm_tm);
    case kSelActData: return cnf->get_act_data_addr_actionbit_map_enable(logtab, xm_tm);
    case kSelStats:   return cnf->get_stats_addr_actionbit_map_enable(logtab, xm_tm);
    case kSelMeter:   return cnf->get_meter_addr_actionbit_map_enable(logtab, xm_tm);
    case kSelIdle:    return cnf->get_idletime_addr_actionbit_map_enable(logtab, xm_tm);
    case kSelNxtTab:  return cnf->get_nxt_tab_actionbit_map_enable(logtab, xm_tm);
    case kSelSelLen:  return false;
    default: RMT_ASSERT(0); break;
  }
}
bool MauLookupResult::map_enabled_log(MauResultBus *cnf,
                                      int sel, int logtab, int xm_tm, int match) {
  // Only apply logical map for NxtTab and only if NxtTab actionbit map NOT enabled
  // (XXX). These use same LUT but actionbit just uses entries 0 & 1.
  //
  // (XM actionbit check commented out below as there is NEVER an XM actionbit map)
  return ((sel == kSelNxtTab) &&
          //(!actionbit_map_enabled(cnf, sel, logtab, 0, match)) &&
          (!actionbit_map_enabled(cnf, sel, logtab, 1, match)) &&
          (cnf->get_nxt_tab_map_enable(logtab, this->match(), gatewayinhibit())));
}
uint32_t MauLookupResult::apply_log_dflt_mask(MauResultBus *cnf,
                                              int sel, int logtab, int xm_tm, int match,
                                              uint32_t data) {
  uint32_t out_data = data;
  // Logical mask/dflt normally 0xFFFFFFFF/0 here apart from NxtTab
  uint32_t mask = get_log_mask(cnf, sel, logtab, xm_tm, match);
  uint32_t dflt = get_log_dflt(cnf, sel, logtab, xm_tm, match);
  if (get_log_dflt_before_mask(cnf, sel, logtab, xm_tm, match)) {
    if ((dflt & mask) != dflt) {
      RMT_LOG_OBJ(ltab_, RmtDebug::warn(RmtDebug::kRmtDebugMauLogicalTableFindActions),
                  "apply_log_dflt_mask(%s): Some bits in dflt NOT set in mask "
                  "[Dflt=0x%08x Mask=0x%08x]\n", get_sel_name(sel), dflt, mask);
    }
    // Note, get_log_mask calls a PER-CHIP func - see jbay/mau-chip-result-bus.h
    // And on JBay nxt-tab mask now has bit8 set by default (0x100)
    out_data = (data | dflt) & mask;
  } else {
    out_data = (data & mask) | dflt;
  }
  // If nxt_tab, stash intermediate value that has had dflt|mask applied
  if (sel == kSelNxtTab) set_next_table_mask(static_cast<uint16_t>(out_data & 0xFFFF));
  return out_data;
}
uint32_t MauLookupResult::no_log_dflt_mask(MauResultBus *cnf,
                                           int sel, int logtab, int xm_tm, int match,
                                           uint32_t data) {
  // This function is an alternative to apply_log_dflt_mask that does NO dflt|mask...
  // ...but still stashes nxt_tab in next_table_mask
  if (sel == kSelNxtTab) set_next_table_mask(static_cast<uint16_t>(data & 0xFFFF));
  return data;
}

uint32_t MauLookupResult::no_map_data(MauResultBus *cnf,
                                      int sel, int logtab, int xm_tm, int match,
                                      uint32_t data) {
  // XXX - logic that was in this func now moved to  apply_log_dflt_mask()
  // called directly from get_log_data_xm|tm()
  return data;
}
uint32_t MauLookupResult::map_data(MauResultBus *cnf,
                                   int sel, int logtab, int xm_tm, int match,
                                   uint32_t data) {
  uint32_t out_data = data;
  // XXX - masking logic removed - apply_log_dflt_mask() called beforehand instead
  if (sel == kSelInstr) {
    uint8_t in_data8 = static_cast<uint8_t>(data & 0xFF);
    uint8_t out_data8 = cnf->get_act_instr_addr_mapped(in_data8, logtab, xm_tm, match);
    mau_->mau_info_incr(MAU_INSTR_MAP_INDIRECTIONS_USED);
    out_data = static_cast<uint32_t>(out_data8);
  } else if (sel == kSelNxtTab) {
    uint16_t in_data16 = static_cast<uint16_t>(data & 0xFFFF);
    uint16_t out_data16 = cnf->get_nxt_tab_mapped(in_data16, logtab);
    mau_->mau_info_incr(MAU_NXT_TAB_MAP_INDIRECTIONS_USED);
    out_data = static_cast<uint32_t>(out_data16);
  }
  return out_data;
}
uint32_t MauLookupResult::actionbit_map_data(MauResultBus *cnf,
                                             int sel, int logtab, int xm_tm, int match,
                                             uint32_t data) {
  uint8_t pld = payload();
  switch (sel) {
    case kSelImmData: return cnf->get_imm_data_actionbit_map_data(logtab, xm_tm, pld);
    case kSelInstr:
      return static_cast<uint32_t>(cnf->get_act_instr_actionbit_map_data(logtab, xm_tm, pld));
    case kSelActData: return cnf->get_act_data_addr_actionbit_map_data(logtab, xm_tm, pld);
    case kSelStats:   return cnf->get_stats_addr_actionbit_map_data(logtab, xm_tm, pld);
    case kSelMeter:   return cnf->get_meter_addr_actionbit_map_data(logtab, xm_tm, pld);
    case kSelIdle:    return cnf->get_idletime_addr_actionbit_map_data(logtab, xm_tm, pld);
    case kSelNxtTab:
      return static_cast<uint32_t>(cnf->get_nxt_tab_actionbit_map_data(logtab, xm_tm, pld));
    case kSelSelLen:  return data;
    default: RMT_ASSERT(0); break;
  }
}
uint32_t MauLookupResult::get_log_data_xm(MauResultBus *cnf,
                                          int sel, int logtab, int match) {
  if (match_buses() == 0) return 0u;
  uint32_t data_in = get_phy_data(cnf, sel, 0, match);
  uint32_t data_out = data_in;
  if (flow_enabled_for_map(cnf, sel, logtab, 0, match, data_in) &&
      map_enabled_xm(cnf, sel, logtab, 0, match)) {
    // XXX - AND Tofino - must always apply log dflt/mask - only does stuff if NxtTab
    uint32_t data_msk = apply_log_dflt_mask(cnf, sel, logtab, 0, match, data_in);
    data_out = map_data(cnf, sel, logtab, 0, match, data_msk);
    RMT_LOG_OBJ(ltab_, RmtDebug::verbose(RmtDebug::kRmtDebugMauLogicalTableFindActions),
                "get_log_data_xm(%s) InData=0x%08x MskData=0x%08x MapData=0x%08x\n",
                get_sel_name(sel), data_in, data_msk, data_out);
  } else if (!map_enabled_log(cnf, sel, logtab, 0, match)) {
    // XXX - AND Tofino - must always apply log dflt/mask - only does stuff if NxtTab
    uint32_t data_msk = apply_log_dflt_mask(cnf, sel, logtab, 0, match, data_in);
    data_out = no_map_data(cnf, sel, logtab, 0, match, data_msk);
    RMT_LOG_OBJ(ltab_, RmtDebug::verbose(RmtDebug::kRmtDebugMauLogicalTableFindActions),
                "get_log_data_xm(%s) InData=0x%08x MskData=0x%08x OutData=0x%08x NoMAP\n",
                get_sel_name(sel), data_in, data_msk, data_out);
  }
  return data_out;
}
uint32_t MauLookupResult::get_log_data_tm(MauResultBus *cnf,
                                          int sel, int logtab, int match) {
  bool got_tinds = (tind_buses() != 0);
  bool got_ltcams = (ltcams() != 0);
  uint32_t data_in = (got_tinds) ?get_phy_data(cnf, sel, 1, match) :0u;
  uint32_t data_out = data_in;
  // Following conditional used to check flow_enabled(). No more!
  // Add commented out call to flow_enabled_for_actionbit_map()
  // just for symmetry with logic on got_tinds/get_log_data_xm() paths.
  if ((got_ltcams) &&
      //(flow_enabled_for_actionbit_map(cnf, sel, logtab, 1, match, data_in)) &&
      (actionbit_map_enabled(cnf, sel, logtab, 1, match))) {
    // XXX - no log dflt/mask here - data_in ignored, TCAM 0|1 payload() used instead
    uint32_t pld = static_cast<uint32_t>(payload());
    data_out = actionbit_map_data(cnf, sel, logtab, 1, match, pld);
    // XXX - call no_log_dflt_mask to stash payload() as nxt_tab_mask - ignore result
    (void)no_log_dflt_mask(cnf, sel, logtab, 1, match, pld);
    RMT_LOG_OBJ(ltab_, RmtDebug::verbose(RmtDebug::kRmtDebugMauLogicalTableFindActions),
                "get_log_data_tm(%s) InData=0x%08x Payload=0x%x AbitMapData=0x%08x\n",
                get_sel_name(sel), data_in, pld, data_out);
  } else if ((got_tinds) &&
             (flow_enabled_for_map(cnf, sel, logtab, 1, match, data_in)) &&
             (map_enabled_tm(cnf, sel, logtab, 1, match))) {
    // XXX - AND Tofino - must always apply log dflt/mask - only does stuff if NxtTab
    uint32_t data_msk = apply_log_dflt_mask(cnf, sel, logtab, 1, match, data_in);
    data_out = map_data(cnf, sel, logtab, 1, match, data_msk);
    RMT_LOG_OBJ(ltab_, RmtDebug::verbose(RmtDebug::kRmtDebugMauLogicalTableFindActions),
                "get_log_data_tm(%s) InData=0x%08x MskData=0x%08x MapData=0x%08x\n",
                get_sel_name(sel), data_in, data_msk, data_out);
  } else if ((got_tinds) &&
             (!map_enabled_log(cnf, sel, logtab, 1, match))) {
    // XXX - AND Tofino - must always apply log dflt/mask - only does stuff if NxtTab
    uint32_t data_msk = apply_log_dflt_mask(cnf, sel, logtab, 1, match, data_in);
    data_out = no_map_data(cnf, sel, logtab, 1, match, data_msk);
    RMT_LOG_OBJ(ltab_, RmtDebug::verbose(RmtDebug::kRmtDebugMauLogicalTableFindActions),
                "get_log_data_tm(%s) InData=0x%08x MskData=0x%08x OutData=0x%08x NoMAP\n",
                get_sel_name(sel), data_in, data_msk, data_out);
  }
  return data_out;
}
uint32_t MauLookupResult::get_log_data_hit(MauResultBus *cnf,
                                           int sel, int logtab, int match) {
  uint32_t xm_data = get_log_data_xm(cnf, sel, logtab, match);
  uint32_t tm_data = get_log_data_tm(cnf, sel, logtab, match);
  uint32_t or_data = xm_data | tm_data;
  uint32_t msk_data = or_data;
  uint32_t out_data = or_data;
  // Logical map LUT can only ever be enabled for NxtTab and then lookup ONLY
  // occurs if LUT not already enabled for NxtTab actionbit map (XXX)
  if (map_enabled_log(cnf, sel, logtab, 99, match)) {
    // These next two steps only ever do anything in the case of NxtTab
    // XXX - AND Tofino - must always apply log dflt/mask
    msk_data = apply_log_dflt_mask(cnf, sel, logtab, 99, 99, or_data);
    // Apply final logical map to the ORed/MASKed result
    out_data = map_data(cnf, sel, logtab, 99, match, msk_data);
  }
  RMT_LOG_OBJ(ltab_, RmtDebug::verbose(RmtDebug::kRmtDebugMauLogicalTableFindActions),
              "get_log_data_hit(%s) XM=0x%08x TM=0x%08x ORed=0x%08x "
              "(Msk=0x%08x Map=0x%08x)\n", get_sel_name(sel), xm_data, tm_data, or_data,
              msk_data, out_data);
  return out_data;
}
uint32_t MauLookupResult::get_log_data_miss(MauResultBus *cnf, int sel, int logtab) {
  // Miss funcs insist on exact_or_tcam param which is irrelevant (and unused) so pass 99
  uint32_t data = 0u;
  switch (sel) {
    case kSelImmData: data = cnf->get_imm_data_miss(logtab, 99); break;
    case kSelInstr:   data = static_cast<uint32_t>(cnf->get_act_instr_addr_miss(logtab, 99)); break;
    case kSelActData: data = cnf->get_act_data_addr_miss(logtab, 99); break;
    case kSelStats:   data = cnf->get_stats_addr_miss(logtab, 99); break;
    case kSelMeter:   data = cnf->get_meter_addr_miss(logtab, 99); break;
    case kSelIdle:    data = cnf->get_idletime_addr_miss(logtab, 99); break;
    case kSelNxtTab:  data = static_cast<uint32_t>(cnf->get_nxt_tab_miss(logtab)); break;
    case kSelSelLen:  data = 0u; break;
    default: RMT_ASSERT(0); break;
  }
  RMT_LOG_OBJ(ltab_, RmtDebug::verbose(RmtDebug::kRmtDebugMauLogicalTableFindActions),
              "get_log_data_miss(%s) MISS=0x%08x\n", get_sel_name(sel), data);
  return data;
}
void MauLookupResult::tally_hits(MauResultBus *cnf, int sel, int logtab) {
  if ((tallied_ & (1<<sel)) != 0) return; // Only count once for a given type
  tallied_ |= (1<<sel);
  switch (sel) {
    case kSelImmData: mau_->mau_info_incr(MAU_IMM_DATA_ADDRS_DISTRIBUTED); break;
    case kSelInstr:   mau_->mau_info_incr(MAU_INSTR_ADDRS_DISTRIBUTED); break;
    case kSelActData: mau_->mau_info_incr(MAU_ACT_DATA_ADDRS_DISTRIBUTED); break;
    case kSelStats:   mau_->mau_info_incr(MAU_STATS_ADDRS_DISTRIBUTED); break;
    case kSelMeter:   mau_->mau_info_incr(MAU_METER_ADDRS_DISTRIBUTED); break;
    case kSelIdle:    mau_->mau_info_incr(MAU_IDLETIME_ADDRS_DISTRIBUTED); break;
    case kSelNxtTab:  mau_->mau_info_incr(MAU_NXT_TAB_ADDRS_DISTRIBUTED); break;
    case kSelSelLen:  mau_->mau_info_incr(MAU_SEL_LEN_ADDRS_DISTRIBUTED); break;
    default: RMT_ASSERT(0); break;
  }
}
void MauLookupResult::tally_misses(MauResultBus *cnf, int sel, int logtab) {
  if ((tallied_ & (1<<sel)) != 0) return; // Only count once for a given type
  tallied_ |= (1<<sel);
  switch (sel) {
    case kSelImmData: mau_->mau_info_incr(MAU_IMM_DATA_MISS_ADDRS_DISTRIBUTED); break;
    case kSelInstr:   mau_->mau_info_incr(MAU_INSTR_MISS_ADDRS_DISTRIBUTED); break;
    case kSelActData: mau_->mau_info_incr(MAU_ACT_DATA_MISS_ADDRS_DISTRIBUTED); break;
    case kSelStats:   mau_->mau_info_incr(MAU_STATS_MISS_ADDRS_DISTRIBUTED); break;
    case kSelMeter:   mau_->mau_info_incr(MAU_METER_MISS_ADDRS_DISTRIBUTED); break;
    case kSelIdle:    mau_->mau_info_incr(MAU_IDLETIME_MISS_ADDRS_DISTRIBUTED); break;
    case kSelNxtTab:  mau_->mau_info_incr(MAU_NXT_TAB_MISS_ADDRS_DISTRIBUTED); break;
    case kSelSelLen:  mau_->mau_info_incr(MAU_SEL_LEN_MISS_ADDRS_DISTRIBUTED); break;
    default: RMT_ASSERT(0); break;
  }
}
uint32_t MauLookupResult::final_futz(MauResultBus *cnf, int sel, int logtab, uint32_t data) {
  switch (sel) {
    case kSelImmData: return data;
    case kSelInstr:   return data;
    case kSelActData: return data;
    case kSelStats:   return data;
    case kSelMeter:   return data;
    case kSelIdle:    return data;
    case kSelNxtTab:  return data;
    case kSelSelLen:  return data;
    default: RMT_ASSERT(0); break;
  }
}
uint32_t MauLookupResult::get_log_data(MauResultBus *cnf, int sel, int logtab,
                                       bool post_predication) {
  uint32_t data = 0u;
  if (match()) {
    data = get_log_data_hit(cnf, sel, logtab, hitentry());
    data |= get_hash_distribution_result(cnf, sel, logtab, hitentry(), post_predication);
    if (flow_enabled(cnf, sel, logtab, data)) tally_hits(cnf, sel, logtab);
  } else {
    data = get_log_data_miss(cnf, sel, logtab);
    if (flow_enabled(cnf, sel, logtab, data)) tally_misses(cnf, sel, logtab);
  }
  data = final_futz(cnf, sel, logtab, data);
  RMT_LOG_OBJ(ltab_, RmtDebug::verbose(RmtDebug::kRmtDebugMauLogicalTableFindActions),
              "get_log_data(%s)=0x%08x\n", get_sel_name(sel), data);
  return data;
}



void MauLookupResult::reset_addresses() {
  next_table_form_ = NxtTab::inval_next_table();
}
void MauLookupResult::init(Mau *mau, MauResultBus* cnf) {
  RMT_ASSERT(mau != NULL);
  RMT_ASSERT(cnf != NULL);
  mau_ = mau;
  cnf_ = cnf;
  ltab_ = NULL;
}
void MauLookupResult::setup_lookup(Phv *phv, int logtab) {
  RMT_ASSERT(mau_ != NULL);
  RMT_ASSERT(cnf_ != NULL);
  if (!valid_) reset();
  // If already setup, exit
  if ((ltab_ != NULL) && (phv_ != NULL)) return;
  // Check params
  RMT_ASSERT(phv != NULL);
  RMT_ASSERT((logtab >= 0) && (logtab < kLogicalTables));
  MauLogicalTable *ltab = mau_->logical_table_lookup(logtab);
  RMT_ASSERT(ltab != NULL);
  // Stash logtab and configured buses/ltcams
  set_logical_table(logtab);
  set_match_buses(cnf_->get_match_buses(logtab));
  set_tind_buses(cnf_->get_tind_buses(logtab));
  set_ltcams(cnf_->get_ltcams(logtab));
  // Stash ingress/egress, bitmask_ops & LTAB as needed by some low-level funcs
  ingress_ = ltab->is_ingress();
  bitmask_ops_ = ltab->has_bitmask_ops();
  ltab_ = ltab; // Only for RMT_LOGs
  phv_ = phv;
}
uint16_t MauLookupResult::extract_next_table(Mau *mau, int logtab) {
  RMT_ASSERT((mau_ != NULL) && (cnf_ != NULL));
  RMT_ASSERT(ltab_ != NULL);
  // If already extracted/formatted next_table, exit
  if (next_table_form() != NxtTab::inval_next_table()) return next_table_form();
  uint16_t nxt_tab_mask = NxtTab::next_table_mask();
  uint16_t nxt_tab = next_table_orig(); // Might just use this if g/w

  if (gatewayinhibit()) {
    set_next_table_mask(nxt_tab);
    // May still need to perform map - eg on JBay - func below is PER-CHIP
    if (cnf_->get_nxt_tab_map_enable(logtab, match(), gatewayinhibit())) {
      // Map whatever value gateway logic put into next_tab_orig
      nxt_tab = cnf_->get_nxt_tab_mapped(nxt_tab, logtab);
      mau_->mau_info_incr(MAU_NXT_TAB_MAP_INDIRECTIONS_USED);
    }
  } else {
    // Otherwise just do normal bus extract (which does mask/map itself internally)
    nxt_tab = static_cast<uint16_t>(get_log_data(cnf_, kSelNxtTab, logtab, true) &
                                    nxt_tab_mask);
  }
  set_next_table_form(nxt_tab);
  return next_table_form();
}


// This only used by Snapshot code
uint32_t MauLookupResult::get_match_addr(int busIndex, int xm_tm) {
  RMT_ASSERT((mau_ != NULL) && (cnf_ != NULL));
  BitVector<kTableResultBusWidth> bus;
  // Call get_bus but pass kSelSelLen so don't fall foul of payload_shifter_enabled
  (void)get_bus(cnf_, kSelSelLen, busIndex, xm_tm, 99, &bus);
  return static_cast<uint32_t>(bus.get_word(kTableResultMatchAddrPos) &
                               static_cast<uint64_t>(kTableResultMatchAddrMask));
}

// Finally some DV funcs

// This is for DV to get the selector length to pass to the hash distribution functions
//
// Note, get_selector_address and get_selector_action_address BOTH now need to be
// passed the ALU corresponding to the logical_table NOT the logical_table.
//
// Func below no longer used - DV call MauLookupResult
// get_selector_address/get_selector_action_address direct
// (or via Mau shim or via MauHashDistribution shim)
//
uint32_t MauLookupResult::get_selector_length(int logtab) {
  RMT_ASSERT((mau_ != NULL) && (cnf_ != NULL));
  int alu = cnf_->get_meter_alu_for_logical_table(logtab);
  return (alu >= 0) ?get_phy_alu_data(cnf_, kSelSelLen, alu) :0;
}
// this is for DV to get access to the physical buses
// Change to pass kSelSelLen because of payload_shifter_enabled
void MauLookupResult::get_bus(int busIndex, int xm_tm, BitVector<kTableResultBusWidth> *bus) {
  (void)get_bus(cnf_, kSelSelLen, busIndex, xm_tm, 0, bus);
}


}
