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

#ifndef _TOFINOXX_MAU_IGNORED_REGS_
#define _TOFINOXX_MAU_IGNORED_REGS_

#include <string>
#include <cstdint>
#include <rmt-defs.h>
#include <mau-defs.h>
#include <register_adapters.h>

#include <register_includes/idletime_logical_to_physical_sweep_grant_ctl_array_mutable.h>
#include <register_includes/idletime_physical_to_logical_req_inc_ctl_array_mutable.h>
#include <register_includes/mapram_mbe_errlog_array_mutable.h>
#include <register_includes/mapram_sbe_errlog_array_mutable.h>
#include <register_includes/adrmux_row_mem_slow_mode_mutable.h>
#include <register_includes/intr_enable0_mau_adrmux_row_mutable.h>
#include <register_includes/intr_enable0_mau_synth2port_mutable.h>
#include <register_includes/intr_enable1_mau_adrmux_row_mutable.h>
#include <register_includes/intr_enable1_mau_synth2port_mutable.h>
#include <register_includes/intr_freeze_enable_mau_adrmux_row_mutable.h>
#include <register_includes/intr_inject_mau_adrmux_row_mutable.h>
#include <register_includes/intr_inject_mau_synth2port_mutable.h>
#include <register_includes/intr_status_mau_adrmux_row_mutable.h>
#include <register_includes/intr_status_mau_synth2port_mutable.h>
#include <register_includes/mapram_mbe_inj_mutable.h>
#include <register_includes/mapram_sbe_inj_mutable.h>
#include <register_includes/mau_synth2port_errlog_mutable.h>
#include <register_includes/mau_synth2port_error_ctl_mutable.h>
#include <register_includes/intr_enable0_mau_selector_alu_mutable.h>
#include <register_includes/intr_enable1_mau_selector_alu_mutable.h>
#include <register_includes/intr_inject_mau_selector_alu_mutable.h>
#include <register_includes/intr_status_mau_selector_alu_mutable.h>
#include <register_includes/mau_diag_meter_alu_group_mutable.h>
#include <register_includes/mau_selector_alu_errlog_mutable.h>
#include <register_includes/intr_enable0_mau_stats_alu_mutable.h>
#include <register_includes/intr_enable1_mau_stats_alu_mutable.h>
#include <register_includes/intr_inject_mau_stats_alu_mutable.h>
#include <register_includes/intr_status_mau_stats_alu_mutable.h>
#include <register_includes/mau_diag_stats_alu_mutable.h>
#include <register_includes/unit_ram_ecc_mutable.h>
#include <register_includes/unit_ram_mbe_errlog_mutable.h>
#include <register_includes/unit_ram_sbe_errlog_mutable.h>
#include <register_includes/actiondata_error_uram_ctl_array_mutable.h>
#include <register_includes/emm_ecc_error_uram_ctl_array_mutable.h>
#include <register_includes/tind_ecc_error_uram_ctl_array_mutable.h>
#include <register_includes/intr_enable0_mau_unit_ram_row_mutable.h>
#include <register_includes/intr_enable1_mau_unit_ram_row_mutable.h>
#include <register_includes/intr_freeze_enable_mau_unit_ram_row_mutable.h>
#include <register_includes/intr_inject_mau_unit_ram_row_mutable.h>
#include <register_includes/intr_status_mau_unit_ram_row_mutable.h>
#include <register_includes/mau_diag_row_adb_clk_enable_mutable.h>
#include <register_includes/bubble_req_ctl_array_mutable.h>
#include <register_includes/deferred_meter_parity_control_array_mutable.h>
#include <register_includes/deferred_stats_parity_control_array_mutable.h>
#include <register_includes/deferred_stats_parity_errlog_array_mutable.h>
#include <register_includes/def_meter_sbe_errlog_array_mutable.h>
#include <register_includes/emm_ecc_error_ctl_array_mutable.h>
#include <register_includes/err_idata_ovr_ctl_array_mutable.h>
#include <register_includes/err_idata_ovr_fifo_ctl_array_mutable.h>
#include <register_includes/gfm_parity_error_ctl_array_mutable.h>
#include <register_includes/idle_bubble_req_array_mutable.h>
#include <register_includes/idletime_slip_array_mutable.h>
#include <register_includes/idletime_slip_intr_ctl_array_mutable.h>
#include <register_includes/intr_mau_decode_memory_core_array_mutable.h>
#include <register_includes/mau_cfg_mram_thread_array_mutable.h>
#include <register_includes/mau_cfg_uram_thread_array_mutable.h>
#include <register_includes/mau_match_input_xbar_exact_match_enable_array_mutable.h>
#include <register_includes/mau_match_input_xbar_ternary_match_enable_array_mutable.h>
#include <register_includes/mau_snapshot_capture_datapath_error_array_mutable.h>
#include <register_includes/meter_alu_group_error_ctl_array_mutable.h>
#include <register_includes/meter_bubble_req_array_mutable.h>
#include <register_includes/o_error_fifo_ctl_array_mutable.h>
#include <register_includes/pbs_creq_errlog_array_mutable.h>
#include <register_includes/pbs_cresp_errlog_array_mutable.h>
#include <register_includes/pbs_sreq_errlog_array_mutable.h>
#include <register_includes/prev_error_ctl_array_mutable.h>
#include <register_includes/s2p_meter_error_ctl_array_mutable.h>
#include <register_includes/s2p_stats_error_ctl_array_mutable.h>
#include <register_includes/stats_bubble_req_array_mutable.h>
#include <register_includes/stats_lrt_fsm_sweep_offset_array_mutable.h>
#include <register_includes/stats_lrt_sweep_adr_array_mutable.h>
#include <register_includes/tcam_logical_channel_errlog_hi_array_mutable.h>
#include <register_includes/tcam_logical_channel_errlog_lo_array_mutable.h>
#include <register_includes/tcam_match_error_ctl_array_mutable.h>
#include <register_includes/tcam_output_table_thread_array_mutable.h>
#include <register_includes/tcam_parity_control_array_mutable.h>
#include <register_includes/tcam_sbe_errlog_array_mutable.h>
#include <register_includes/tcam_table_prop_array_mutable.h>
#include <register_includes/tind_ecc_error_ctl_array_mutable.h>
#include <register_includes/actiondata_error_ctl_mutable.h>
#include <register_includes/adr_dist_mem_slow_mode_mutable.h>
#include <register_includes/hashout_ctl_mutable.h>
#include <register_includes/idletime_slip_errlog_mutable.h>
#include <register_includes/imem_parity_error_ctl_mutable.h>
#include <register_includes/imem_sbe_errlog_mutable.h>
#include <register_includes/intr_decode_top_mutable.h>
#include <register_includes/intr_enable0_mau_ad_mutable.h>
#include <register_includes/intr_enable0_mau_cfg_mutable.h>
#include <register_includes/intr_enable0_mau_gfm_hash_mutable.h>
#include <register_includes/intr_enable0_mau_imem_mutable.h>
#include <register_includes/intr_enable0_mau_snapshot_mutable.h>
#include <register_includes/intr_enable1_mau_ad_mutable.h>
#include <register_includes/intr_enable1_mau_cfg_mutable.h>
#include <register_includes/intr_enable1_mau_gfm_hash_mutable.h>
#include <register_includes/intr_enable1_mau_imem_mutable.h>
#include <register_includes/intr_enable1_mau_snapshot_mutable.h>
#include <register_includes/intr_freeze_enable_mau_ad_mutable.h>
#include <register_includes/intr_freeze_enable_mau_cfg_mutable.h>
#include <register_includes/intr_freeze_enable_mau_gfm_hash_mutable.h>
#include <register_includes/intr_freeze_enable_mau_imem_mutable.h>
#include <register_includes/intr_freeze_enable_mau_snapshot_mutable.h>
#include <register_includes/intr_inject_mau_ad_mutable.h>
#include <register_includes/intr_inject_mau_cfg_mutable.h>
#include <register_includes/intr_inject_mau_gfm_hash_mutable.h>
#include <register_includes/intr_inject_mau_imem_mutable.h>
#include <register_includes/intr_inject_mau_snapshot_mutable.h>
#include <register_includes/intr_status_mau_cfg_mutable.h>
#include <register_includes/intr_status_mau_gfm_hash_mutable.h>
#include <register_includes/intr_status_mau_imem_mutable.h>
#include <register_includes/mau_cfg_dram_thread_mutable.h>
#include <register_includes/mau_cfg_imem_bubble_req_mutable.h>
#include <register_includes/mau_cfg_lt_thread_mutable.h>
#include <register_includes/mau_cfg_mem_slow_mode_mutable.h>
#include <register_includes/mau_diag_32b_oxbar_ctl_mutable.h>
#include <register_includes/mau_diag_32b_oxbar_premux_ctl_mutable.h>
#include <register_includes/mau_diag_8b_oxbar_ctl_mutable.h>
#include <register_includes/mau_diag_adb_ctl_mutable.h>
#include <register_includes/mau_diag_adb_map_mutable.h>
#include <register_includes/mau_diag_adr_dist_idletime_adr_oxbar_ctl_mutable.h>
#include <register_includes/mau_diag_cfg_ctl_mutable.h>
#include <register_includes/mau_diag_eop_vld_xport_mutable.h>
#include <register_includes/mau_diag_meter_adr_sel_mutable.h>
#include <register_includes/mau_diag_pbus_enable_mutable.h>
#include <register_includes/mau_diag_stats_adr_sel_mutable.h>
#include <register_includes/mau_diag_tcam_clk_en_mutable.h>
#include <register_includes/mau_diag_tcam_hit_xbar_ctl_mutable.h>
#include <register_includes/mau_diag_valid_ctl_mutable.h>
#include <register_includes/meter_sweep_errlog_mutable.h>
#include <register_includes/pbs_creq_ecc_mutable.h>
#include <register_includes/pbs_cresp_ecc_mutable.h>
#include <register_includes/pbs_sreq_ecc_mutable.h>
#include <register_includes/q_hole_acc_errlog_hi_mutable.h>
#include <register_includes/q_hole_acc_errlog_lo_mutable.h>
#include <register_includes/tcam_piped_mutable.h>
#include <register_includes/tcam_scrub_ctl_mutable.h>


namespace MODEL_CHIP_NAMESPACE {

  class MauIgnoredRegs {
 public:
  MauIgnoredRegs(int chipIndex, int pipeIndex, int mauIndex, Mau *mau);

  ~MauIgnoredRegs() {  }

  void not_implemented(const char *clazz) {
    //printf("Register %s not implemented\n", clazz);
  }

  void reset();

private:
      std::array< register_classes::IdletimeLogicalToPhysicalSweepGrantCtlArrayMutable,8 > i_IdletimeLogicalToPhysicalSweepGrantCtlArray_;
      std::array< register_classes::IdletimePhysicalToLogicalReqIncCtlArrayMutable,8 > i_IdletimePhysicalToLogicalReqIncCtlArray_;
      std::array< register_classes::MapramMbeErrlogArrayMutable,8 > i_MapramMbeErrlogArray_;
      std::array< register_classes::MapramSbeErrlogArrayMutable,8 > i_MapramSbeErrlogArray_;
      std::array< register_classes::AdrmuxRowMemSlowModeMutable,8 > i_AdrmuxRowMemSlowMode_;
      std::array< register_classes::IntrEnable0MauAdrmuxRowMutable,8 > i_IntrEnable0MauAdrmuxRow_;
      std::array< register_classes::IntrEnable0MauSynth2portMutable,8 > i_IntrEnable0MauSynth2port_;
      std::array< register_classes::IntrEnable1MauAdrmuxRowMutable,8 > i_IntrEnable1MauAdrmuxRow_;
      std::array< register_classes::IntrEnable1MauSynth2portMutable,8 > i_IntrEnable1MauSynth2port_;
      std::array< register_classes::IntrFreezeEnableMauAdrmuxRowMutable,8 > i_IntrFreezeEnableMauAdrmuxRow_;
      std::array< register_classes::IntrInjectMauAdrmuxRowMutable,8 > i_IntrInjectMauAdrmuxRow_;
      std::array< register_classes::IntrInjectMauSynth2portMutable,8 > i_IntrInjectMauSynth2port_;
      std::array< register_classes::IntrStatusMauAdrmuxRowMutable,8 > i_IntrStatusMauAdrmuxRow_;
      std::array< register_classes::IntrStatusMauSynth2portMutable,8 > i_IntrStatusMauSynth2port_;
      std::array< register_classes::MapramMbeInjMutable,8 > i_MapramMbeInj_;
      std::array< register_classes::MapramSbeInjMutable,8 > i_MapramSbeInj_;
      std::array< register_classes::MauSynth2portErrlogMutable,8 > i_MauSynth2portErrlog_;
      std::array< register_classes::MauSynth2portErrorCtlMutable,8 > i_MauSynth2portErrorCtl_;
      std::array< register_classes::IntrEnable0MauSelectorAluMutable,4 > i_IntrEnable0MauSelectorAlu_;
      std::array< register_classes::IntrEnable1MauSelectorAluMutable,4 > i_IntrEnable1MauSelectorAlu_;
      std::array< register_classes::IntrInjectMauSelectorAluMutable,4 > i_IntrInjectMauSelectorAlu_;
      std::array< register_classes::IntrStatusMauSelectorAluMutable,4 > i_IntrStatusMauSelectorAlu_;
      std::array< register_classes::MauDiagMeterAluGroupMutable,4 > i_MauDiagMeterAluGroup_;
      std::array< register_classes::MauSelectorAluErrlogMutable,4 > i_MauSelectorAluErrlog_;
      std::array< register_classes::IntrEnable0MauStatsAluMutable,4 > i_IntrEnable0MauStatsAlu_;
      std::array< register_classes::IntrEnable1MauStatsAluMutable,4 > i_IntrEnable1MauStatsAlu_;
      std::array< register_classes::IntrInjectMauStatsAluMutable,4 > i_IntrInjectMauStatsAlu_;
      std::array< register_classes::IntrStatusMauStatsAluMutable,4 > i_IntrStatusMauStatsAlu_;
      std::array< register_classes::MauDiagStatsAluMutable,4 > i_MauDiagStatsAlu_;
      std::array< register_classes::UnitRamEccMutable,96 > i_UnitRamEcc_;
      std::array< register_classes::UnitRamMbeErrlogMutable,96 > i_UnitRamMbeErrlog_;
      std::array< register_classes::UnitRamSbeErrlogMutable,96 > i_UnitRamSbeErrlog_;
      std::array< register_classes::ActiondataErrorUramCtlArrayMutable,8 > i_ActiondataErrorUramCtlArray_;
      std::array< register_classes::EmmEccErrorUramCtlArrayMutable,8 > i_EmmEccErrorUramCtlArray_;
      std::array< register_classes::TindEccErrorUramCtlArrayMutable,8 > i_TindEccErrorUramCtlArray_;
      std::array< register_classes::IntrEnable0MauUnitRamRowMutable,8 > i_IntrEnable0MauUnitRamRow_;
      std::array< register_classes::IntrEnable1MauUnitRamRowMutable,8 > i_IntrEnable1MauUnitRamRow_;
      std::array< register_classes::IntrFreezeEnableMauUnitRamRowMutable,8 > i_IntrFreezeEnableMauUnitRamRow_;
      std::array< register_classes::IntrInjectMauUnitRamRowMutable,8 > i_IntrInjectMauUnitRamRow_;
      std::array< register_classes::IntrStatusMauUnitRamRowMutable,8 > i_IntrStatusMauUnitRamRow_;
      std::array< register_classes::MauDiagRowAdbClkEnableMutable,8 > i_MauDiagRowAdbClkEnable_;
      register_classes::BubbleReqCtlArrayMutable i_BubbleReqCtlArray_;
      register_classes::DeferredMeterParityControlArrayMutable i_DeferredMeterParityControlArray_;
      register_classes::DeferredStatsParityControlArrayMutable i_DeferredStatsParityControlArray_;
      register_classes::DeferredStatsParityErrlogArrayMutable i_DeferredStatsParityErrlogArray_;
      register_classes::DefMeterSbeErrlogArrayMutable i_DefMeterSbeErrlogArray_;
      register_classes::EmmEccErrorCtlArrayMutable i_EmmEccErrorCtlArray_;
      register_classes::ErrIdataOvrCtlArrayMutable i_ErrIdataOvrCtlArray_;
      register_classes::ErrIdataOvrFifoCtlArrayMutable i_ErrIdataOvrFifoCtlArray_;
      register_classes::GfmParityErrorCtlArrayMutable i_GfmParityErrorCtlArray_;
      register_classes::IdleBubbleReqArrayMutable i_IdleBubbleReqArray_;
      register_classes::IdletimeSlipArrayMutable i_IdletimeSlipArray_;
      register_classes::IdletimeSlipIntrCtlArrayMutable i_IdletimeSlipIntrCtlArray_;
      register_classes::IntrMauDecodeMemoryCoreArrayMutable i_IntrMauDecodeMemoryCoreArray_;
      register_classes::MauCfgMramThreadArrayMutable i_MauCfgMramThreadArray_;
      register_classes::MauCfgUramThreadArrayMutable i_MauCfgUramThreadArray_;
      register_classes::MauMatchInputXbarExactMatchEnableArrayMutable i_MauMatchInputXbarExactMatchEnableArray_;
      register_classes::MauMatchInputXbarTernaryMatchEnableArrayMutable i_MauMatchInputXbarTernaryMatchEnableArray_;
      register_classes::MauSnapshotCaptureDatapathErrorArrayMutable i_MauSnapshotCaptureDatapathErrorArray_;
      register_classes::MeterAluGroupErrorCtlArrayMutable i_MeterAluGroupErrorCtlArray_;
      register_classes::MeterBubbleReqArrayMutable i_MeterBubbleReqArray_;
      register_classes::OErrorFifoCtlArrayMutable i_OErrorFifoCtlArray_;
      register_classes::PbsCreqErrlogArrayMutable i_PbsCreqErrlogArray_;
      register_classes::PbsCrespErrlogArrayMutable i_PbsCrespErrlogArray_;
      register_classes::PbsSreqErrlogArrayMutable i_PbsSreqErrlogArray_;
      register_classes::PrevErrorCtlArrayMutable i_PrevErrorCtlArray_;
      register_classes::S2pMeterErrorCtlArrayMutable i_S2pMeterErrorCtlArray_;
      register_classes::S2pStatsErrorCtlArrayMutable i_S2pStatsErrorCtlArray_;
      register_classes::StatsBubbleReqArrayMutable i_StatsBubbleReqArray_;
      register_classes::StatsLrtFsmSweepOffsetArrayMutable i_StatsLrtFsmSweepOffsetArray_;
      register_classes::StatsLrtSweepAdrArrayMutable i_StatsLrtSweepAdrArray_;
      register_classes::TcamLogicalChannelErrlogHiArrayMutable i_TcamLogicalChannelErrlogHiArray_;
      register_classes::TcamLogicalChannelErrlogLoArrayMutable i_TcamLogicalChannelErrlogLoArray_;
      register_classes::TcamMatchErrorCtlArrayMutable i_TcamMatchErrorCtlArray_;
      register_classes::TcamOutputTableThreadArrayMutable i_TcamOutputTableThreadArray_;
      register_classes::TcamParityControlArrayMutable i_TcamParityControlArray_;
      register_classes::TcamSbeErrlogArrayMutable i_TcamSbeErrlogArray_;
      register_classes::TcamTablePropArrayMutable i_TcamTablePropArray_;
      register_classes::TindEccErrorCtlArrayMutable i_TindEccErrorCtlArray_;
      register_classes::ActiondataErrorCtlMutable i_ActiondataErrorCtl_;
      register_classes::AdrDistMemSlowModeMutable i_AdrDistMemSlowMode_;
      register_classes::HashoutCtlMutable i_HashoutCtl_;
      register_classes::IdletimeSlipErrlogMutable i_IdletimeSlipErrlog_;
      register_classes::ImemParityErrorCtlMutable i_ImemParityErrorCtl_;
      register_classes::ImemSbeErrlogMutable i_ImemSbeErrlog_;
      register_classes::IntrDecodeTopMutable i_IntrDecodeTop_;
      register_classes::IntrEnable0MauAdMutable i_IntrEnable0MauAd_;
      register_classes::IntrEnable0MauCfgMutable i_IntrEnable0MauCfg_;
      register_classes::IntrEnable0MauGfmHashMutable i_IntrEnable0MauGfmHash_;
      register_classes::IntrEnable0MauImemMutable i_IntrEnable0MauImem_;
      register_classes::IntrEnable0MauSnapshotMutable i_IntrEnable0MauSnapshot_;
      register_classes::IntrEnable1MauAdMutable i_IntrEnable1MauAd_;
      register_classes::IntrEnable1MauCfgMutable i_IntrEnable1MauCfg_;
      register_classes::IntrEnable1MauGfmHashMutable i_IntrEnable1MauGfmHash_;
      register_classes::IntrEnable1MauImemMutable i_IntrEnable1MauImem_;
      register_classes::IntrEnable1MauSnapshotMutable i_IntrEnable1MauSnapshot_;
      register_classes::IntrFreezeEnableMauAdMutable i_IntrFreezeEnableMauAd_;
      register_classes::IntrFreezeEnableMauCfgMutable i_IntrFreezeEnableMauCfg_;
      register_classes::IntrFreezeEnableMauGfmHashMutable i_IntrFreezeEnableMauGfmHash_;
      register_classes::IntrFreezeEnableMauImemMutable i_IntrFreezeEnableMauImem_;
      register_classes::IntrFreezeEnableMauSnapshotMutable i_IntrFreezeEnableMauSnapshot_;
      register_classes::IntrInjectMauAdMutable i_IntrInjectMauAd_;
      register_classes::IntrInjectMauCfgMutable i_IntrInjectMauCfg_;
      register_classes::IntrInjectMauGfmHashMutable i_IntrInjectMauGfmHash_;
      register_classes::IntrInjectMauImemMutable i_IntrInjectMauImem_;
      register_classes::IntrInjectMauSnapshotMutable i_IntrInjectMauSnapshot_;
      register_classes::IntrStatusMauCfgMutable i_IntrStatusMauCfg_;
      register_classes::IntrStatusMauGfmHashMutable i_IntrStatusMauGfmHash_;
      register_classes::IntrStatusMauImemMutable i_IntrStatusMauImem_;
      register_classes::MauCfgDramThreadMutable i_MauCfgDramThread_;
      register_classes::MauCfgImemBubbleReqMutable i_MauCfgImemBubbleReq_;
      register_classes::MauCfgLtThreadMutable i_MauCfgLtThread_;
      register_classes::MauCfgMemSlowModeMutable i_MauCfgMemSlowMode_;
      register_classes::MauDiag_32bOxbarCtlMutable i_MauDiag_32bOxbarCtl_;
      register_classes::MauDiag_32bOxbarPremuxCtlMutable i_MauDiag_32bOxbarPremuxCtl_;
      register_classes::MauDiag_8bOxbarCtlMutable i_MauDiag_8bOxbarCtl_;
      register_classes::MauDiagAdbCtlMutable i_MauDiagAdbCtl_;
      register_classes::MauDiagAdbMapMutable i_MauDiagAdbMap_;
      register_classes::MauDiagAdrDistIdletimeAdrOxbarCtlMutable i_MauDiagAdrDistIdletimeAdrOxbarCtl_;
      register_classes::MauDiagCfgCtlMutable i_MauDiagCfgCtl_;
      register_classes::MauDiagEopVldXportMutable i_MauDiagEopVldXport_;
      register_classes::MauDiagMeterAdrSelMutable i_MauDiagMeterAdrSel_;
      register_classes::MauDiagPbusEnableMutable i_MauDiagPbusEnable_;
      register_classes::MauDiagStatsAdrSelMutable i_MauDiagStatsAdrSel_;
      register_classes::MauDiagTcamClkEnMutable i_MauDiagTcamClkEn_;
      register_classes::MauDiagTcamHitXbarCtlMutable i_MauDiagTcamHitXbarCtl_;
      register_classes::MauDiagValidCtlMutable i_MauDiagValidCtl_;
      register_classes::MeterSweepErrlogMutable i_MeterSweepErrlog_;
      register_classes::PbsCreqEccMutable i_PbsCreqEcc_;
      register_classes::PbsCrespEccMutable i_PbsCrespEcc_;
      register_classes::PbsSreqEccMutable i_PbsSreqEcc_;
      register_classes::QHoleAccErrlogHiMutable i_QHoleAccErrlogHi_;
      register_classes::QHoleAccErrlogLoMutable i_QHoleAccErrlogLo_;
      register_classes::TcamPipedMutable i_TcamPiped_;
      register_classes::TcamScrubCtlMutable i_TcamScrubCtl_;
  };
}
#endif // _TOFINOXX_MAU_IGNORED_REGS_
