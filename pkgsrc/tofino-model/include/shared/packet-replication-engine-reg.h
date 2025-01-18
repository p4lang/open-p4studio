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

#ifndef _SHARED_PACKET_REPLICATION_ENGINE_REG_
#define _SHARED_PACKET_REPLICATION_ENGINE_REG_

#include <array>
#include <mutex>

#include <bitvector.h>
#include <indirect-addressing.h>
#include <pipe-object.h>
#include <mem_utils.h>
// Reg defs auto-generated from Semifore
#include <register_includes/pre_port_mask_mutable.h>
#include <register_includes/pre_pipe_int_status_mutable.h>
#include <register_includes/pre_port_down_mutable.h>
#include <register_includes/pre_common_ctrl_mutable.h>
#include <register_includes/pre_prune_rid_mutable.h>
#include <register_includes/pre_rdm_blk_id.h>

// Per-Pipe Registers
#include <register_includes/pre_ctr48_packet_drop_mutable.h>
#include <register_includes/pre_max_l1_node_log_mutable.h>
#include <register_includes/pre_max_l1_node_ctrl_mutable.h>
#include <register_includes/pre_filter_mask_mutable.h>
#include <register_includes/pre_int_en1_mutable.h>
#include <register_includes/pre_ctr48_yid_prunes_mutable.h>
#include <register_includes/pre_ctr48_cpu_copies_mutable.h>
#include <register_includes/pre_ctr48_ph_lost_mutable.h>
#include <register_includes/pre_ctr48_total_copies_mutable.h>
#include <register_includes/pre_ctr48_ph_processed_mutable.h>
#include <register_includes/pre_max_l2_node_log_mutable.h>
#include <register_includes/pre_rdm_ph_log_mutable.h>
#include <register_includes/pre_port_vector_mutable.h>
#include <register_includes/pre_rdm_ctrl_mutable.h>
#include <register_includes/pre_mbe_log_mutable.h>
#include <register_includes/pre_int_stat_mutable.h>
#include <register_includes/pre_max_l2_node_ctrl_mutable.h>
#include <register_includes/pre_illegal_l1_node_log_mutable.h>
#include <register_includes/pre_fifo_ph_count_mutable.h>
#include <register_includes/pre_illegal_l2_node_log_mutable.h>
#include <register_includes/pre_table_ph_count_mutable.h>
#include <register_includes/pre_sbe_log_mutable.h>
#include <register_includes/pre_int_en0_mutable.h>
#include <register_includes/pre_wrr_ctrl_mutable.h>
#include <register_includes/pre_fifo_depth_ctrl_mutable.h>
#include <register_includes/pre_filter_ctrl_mutable.h>
#include <register_includes/pre_ctr48_xid_prunes_mutable.h>
#include <register_includes/pre_int_inj_mutable.h>
#include <register_includes/pre_rdm_addr_ctrl_mutable.h>
#include <register_includes/pre_ctrl_mutable.h>
#include <register_includes/pre_arb_ctrl_mutable.h>


namespace MODEL_CHIP_NAMESPACE {

  class Rdm;
  class PacketReplicationEngine;
  class PacketReplicationEngineRegCom;

  //
  // Contains registers specific to a single PRE.
  //
  class PacketReplicationEngineRegPipe : public PipeObject {
  public:
    PacketReplicationEngineRegPipe(RmtObjectManager *om, int pipeIndex, PacketReplicationEngine *pre);

    void reset();
    void rdm_change_done();

    inline bool     arb_mode(int fifo) { return (arb_ctrl_.arbitration_mode() >> fifo) & 1; }
    inline bool     c2c_enable() { return ctrl_.c2c_enable(); }
    inline uint8_t  c2c_port()   { return ctrl_.c2c_port(); }
    inline uint32_t l1_per_slice() {return ctrl_.l1_per_slice(); }
    inline uint8_t  wrr_weight(int fifo) { return 0==fifo ? wrr_ctrl_.wrr_pri0() :
                                                  1==fifo ? wrr_ctrl_.wrr_pri1() :
                                                  2==fifo ? wrr_ctrl_.wrr_pri2() :
                                                  3==fifo ? wrr_ctrl_.wrr_pri3() :
                                                  0; }
    inline int      max_L1() { return max_l1_node_ctrl_.l1_max_node(); }
    inline int      max_L2() { return max_l2_node_ctrl_.l2_max_node(); }


    inline void     log_max_L1(uint16_t mgid, uint32_t addr) {
                      max_l1_node_log_.mgid(mgid);
                      max_l1_node_log_.l1_addr(addr); }
    inline void     log_max_L2(uint16_t mgid, uint32_t addr) {
                      max_l2_node_log_.mgid(mgid);
                      max_l2_node_log_.l2_addr(addr); }
    inline void inc_tbl0_cnt() { table0_ph_count_.table_ph_count(table0_ph_count_.table_ph_count() + 1); }
    inline void inc_tbl1_cnt() { table1_ph_count_.table_ph_count(table1_ph_count_.table_ph_count() + 1); }
    inline void dec_tbl0_cnt() { RMT_ASSERT(table0_ph_count_.table_ph_count());
                                 table0_ph_count_.table_ph_count(table0_ph_count_.table_ph_count() - 1); }
    inline void dec_tbl1_cnt() { RMT_ASSERT(table1_ph_count_.table_ph_count());
                                 table1_ph_count_.table_ph_count(table1_ph_count_.table_ph_count() - 1); }
  private:
    void pipe_wcb();
    inline void pipe_rcb() {}

    bool rdm_change_pending_ = false;
    PacketReplicationEngine *pre_;

    register_classes::PreCtrlMutable ctrl_;
    register_classes::PreArbCtrlMutable arb_ctrl_;
    register_classes::PreWrrCtrlMutable wrr_ctrl_;
    register_classes::PreFifoDepthCtrlMutable fifo0_depth_;
    register_classes::PreFifoDepthCtrlMutable fifo1_depth_;
    register_classes::PreFifoDepthCtrlMutable fifo2_depth_;
    register_classes::PreFifoDepthCtrlMutable fifo3_depth_;
    register_classes::PreMaxL1NodeCtrlMutable max_l1_node_ctrl_;
    register_classes::PreMaxL2NodeCtrlMutable max_l2_node_ctrl_;
    register_classes::PreRdmCtrlMutable rdm_ctrl_;
    register_classes::PreFilterCtrlMutable filter_ctrl_;
    register_classes::PreFilterMaskMutable filter_mask_;
    register_classes::PreRdmAddrCtrlMutable rdm_addr_ctrl_;
    register_classes::PreIntStatMutable int_stat_;
    register_classes::PreIntEn0Mutable int_en0_;
    register_classes::PreIntEn1Mutable int_en1_;
    register_classes::PreIntInjMutable int_inj_;
    register_classes::PreFifoPhCountMutable fifo0_ph_count_;
    register_classes::PreFifoPhCountMutable fifo1_ph_count_;
    register_classes::PreFifoPhCountMutable fifo2_ph_count_;
    register_classes::PreFifoPhCountMutable fifo3_ph_count_;
    register_classes::PreTablePhCountMutable table0_ph_count_;
    register_classes::PreTablePhCountMutable table1_ph_count_;
    register_classes::PreCtr48CpuCopiesMutable cpu_copies_;
    register_classes::PreCtr48PhProcessedMutable ph_processed_;
    register_classes::PreCtr48TotalCopiesMutable total_copies_;
    register_classes::PreCtr48XidPrunesMutable xid_prunes_;
    register_classes::PreCtr48YidPrunesMutable yid_prunes_;
    register_classes::PreCtr48PhProcessedMutable filtered_ph_processed_;
    register_classes::PreCtr48TotalCopiesMutable filtered_total_copies_;
    register_classes::PreCtr48XidPrunesMutable filtered_xid_prunes_;
    register_classes::PreCtr48YidPrunesMutable filtered_yid_prunes_;
    register_classes::PrePortVectorMutable filtered_port_vector_;
    register_classes::PreRdmPhLogMutable rdm_ph_log0_;
    register_classes::PreRdmPhLogMutable rdm_ph_log1_;
    register_classes::PreRdmPhLogMutable rdm_ph_log2_;
    register_classes::PreRdmPhLogMutable rdm_ph_log3_;
    register_classes::PreRdmPhLogMutable rdm_ph_log4_;
    register_classes::PreCtr48PhLostMutable ph_lost_;
    register_classes::PreCtr48PacketDropMutable packet_drop_;
    register_classes::PreMaxL1NodeLogMutable max_l1_node_log_;
    register_classes::PreMaxL2NodeLogMutable max_l2_node_log_;
    register_classes::PreIllegalL1NodeLogMutable illegal_l1_node_log_;
    register_classes::PreIllegalL2NodeLogMutable illegal_l2_node_log_;
    register_classes::PreSbeLogMutable sbe_log_;
    register_classes::PreMbeLogMutable mbe_log_;
  };


  //
  // RDM
  class PacketReplicationEngineMemRdm : public RmtObject,
                                        public model_core::RegisterBlockIndirect<RegisterCallback> {
  public:
    PacketReplicationEngineMemRdm(RmtObjectManager *om, PacketReplicationEngineRegCom *com);
  private:
    bool write(uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T);
    bool read(uint64_t offset,uint64_t* data0,uint64_t* data1, uint64_t T) const;
    std::string to_string(bool print_zeros, std::string indent_string) const {return "";}
    std::string to_string(uint64_t offset, bool print_zeros, std::string indent_string) const {return "";}
    PacketReplicationEngineRegCom *com_ = nullptr;
  };

  //
  // MIT
  class PacketReplicationEngineMemMit : public RmtObject,
                                        public model_core::RegisterBlockIndirect<RegisterCallback> {
  public:
    PacketReplicationEngineMemMit(RmtObjectManager *om, int pre_idx, PacketReplicationEngineRegCom *com);
  private:
    bool write(uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T);
    bool read(uint64_t offset,uint64_t* data0,uint64_t* data1, uint64_t T) const;
    std::string to_string(bool print_zeros, std::string indent_string) const {return "";}
    std::string to_string(uint64_t offset, bool print_zeros, std::string indent_string) const {return "";}
    int idx_ = -1;
    PacketReplicationEngineRegCom *com_ = nullptr;
  };

  //
  // PBT
  class PacketReplicationEngineMemPbt : public RmtObject,
                                        public model_core::RegisterBlockIndirect<RegisterCallback> {
  public:
    PacketReplicationEngineMemPbt(RmtObjectManager *om, int ver, PacketReplicationEngineRegCom *com);
  private:
    bool write(uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T);
    bool read(uint64_t offset,uint64_t* data0,uint64_t* data1, uint64_t T) const;
    std::string to_string(bool print_zeros, std::string indent_string) const {return "";}
    std::string to_string(uint64_t offset, bool print_zeros, std::string indent_string) const {return "";}
    int ver_ = -1;
    PacketReplicationEngineRegCom *com_ = nullptr;
  };

  //
  // LAG - NP
  class PacketReplicationEngineMemLagNp : public RmtObject,
                                          public model_core::RegisterBlockIndirect<RegisterCallback> {
  public:
    PacketReplicationEngineMemLagNp(RmtObjectManager *om, int ver, PacketReplicationEngineRegCom *com);
  private:
    bool write(uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T);
    bool read(uint64_t offset,uint64_t* data0,uint64_t* data1, uint64_t T) const;
    std::string to_string(bool print_zeros, std::string indent_string) const {return "";}
    std::string to_string(uint64_t offset, bool print_zeros, std::string indent_string) const {return "";}
    int ver_ = -1;
    PacketReplicationEngineRegCom *com_ = nullptr;
  };

  //
  // LAG - Membership
  class PacketReplicationEngineMemLag : public RmtObject,
                                        public model_core::RegisterBlockIndirect<RegisterCallback> {
  public:
    PacketReplicationEngineMemLag(RmtObjectManager *om, int ver, int seg, PacketReplicationEngineRegCom *com);
  private:
    bool write(uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T);
    bool read(uint64_t offset,uint64_t* data0,uint64_t* data1, uint64_t T) const;
    std::string to_string(bool print_zeros, std::string indent_string) const {return "";}
    std::string to_string(uint64_t offset, bool print_zeros, std::string indent_string) const {return "";}
    int ver_ = -1;
    int seg_ = -1;
    PacketReplicationEngineRegCom *com_ = nullptr;
  };

  //
  // PMT
  class PacketReplicationEngineMemPmt : public RmtObject,
                                        public model_core::RegisterBlockIndirect<RegisterCallback> {
  public:
    PacketReplicationEngineMemPmt(RmtObjectManager *om, int ver, int seg, PacketReplicationEngineRegCom *com);
  private:
    bool write(uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T);
    bool read(uint64_t offset,uint64_t* data0,uint64_t* data1, uint64_t T) const;
    std::string to_string(bool print_zeros, std::string indent_string) const {return "";}
    std::string to_string(uint64_t offset, bool print_zeros, std::string indent_string) const {return "";}
    int ver_ = -1;
    int seg_ = -1;
    PacketReplicationEngineRegCom *com_ = nullptr;
  };

  //
  // FIFO
  class PacketReplicationEngineMemFifo : public RmtObject,
                                         public model_core::RegisterBlockIndirect<RegisterCallback> {
  public:
    PacketReplicationEngineMemFifo(RmtObjectManager *om, PacketReplicationEngineRegCom *com);
  private:
    bool write(uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T);
    bool read(uint64_t offset,uint64_t* data0,uint64_t* data1, uint64_t T) const;
    std::string to_string(bool print_zeros, std::string indent_string) const {return "";}
    std::string to_string(uint64_t offset, bool print_zeros, std::string indent_string) const {return "";}
    PacketReplicationEngineRegCom *com_ = nullptr;
  };

  //
  // Contains registers shared by all PREs
  //
  class PacketReplicationEngineRegCom : public RmtObject {
  public:

    static constexpr int kMitSize = 0xFFFF+1;
    static constexpr int kFifoCnt = 4;
    static constexpr int kLocalPortWidth = RmtDefs::kTmPortsPerPipe;
    PacketReplicationEngineRegCom(RmtObjectManager *om);
    ~PacketReplicationEngineRegCom();

    void reset();

    bool validate_rdm_block_access(int pipe, int addr);
    void set_pmt_bit(uint32_t ver, uint32_t yid, uint32_t bit_pos, uint32_t val);
    void write_pmt(uint32_t ver, uint32_t yid, uint32_t segment, uint8_t upper, uint64_t lower);
    const BitVector<MemUtils::pre_pmt_mem_size()>& pmt(uint32_t ver, uint32_t yid) const;

    void set_lag_mbr(uint32_t ver, uint32_t id, uint32_t mbr, uint32_t val);
    void set_lag(uint32_t ver, uint32_t id, const BitVector<MemUtils::pre_lag_mem_entry_size()> &data);
    void write_lag(uint32_t ver, uint32_t index, uint32_t segment, uint8_t upper, uint64_t lower);
    const BitVector<MemUtils::pre_lag_mem_entry_size()>& lag(uint32_t ver, uint32_t index) const;
    void set_lag_np(uint32_t ver, uint32_t id, uint32_t val);
    int  lag_npl(uint32_t ver, uint32_t id) const;
    int  lag_npr(uint32_t ver, uint32_t id) const;

    int  mit(uint32_t index, uint32_t mgid) const;
    void write_mit(uint32_t index, uint32_t mgid, uint32_t val);
    void write_mit_row(uint32_t index, uint32_t mgid, uint32_t val0, uint32_t val1, uint32_t val2, uint32_t val3);

    int  bpt(uint32_t ver, uint32_t index) const;
    void write_bpt(uint32_t ver, uint32_t index, uint32_t val);

    Rdm* rdm() const;
    void write_rdm(int row, const BitVector<128> &data);

    inline int      fifo_q_size(int q) const {return fifo_rows_[q] * 8; }
    inline int      fifo_base(int q) const { return fifo_base_[q]; }
    inline int      fifo_rows(int q) const { return fifo_rows_[q]; }
    inline void     fifo_base(int q, int x) { fifo_base_[q] = x; }
    inline void     fifo_rows(int q, int x) { fifo_rows_[q] = x; }
    inline uint16_t global_rid() { return prune_rid_.rid(); }
    inline void     global_rid(uint16_t x) { prune_rid_.rid(x); }
    inline bool     use_hw_mask() { return common_ctrl_.hw_port_liveness_en(); }
    inline void     use_hw_mask(bool x) { common_ctrl_.hw_port_liveness_en(x); }
    inline bool     use_bpt() { return common_ctrl_.backup_port_en(); }
    inline void     use_bpt(bool x) { common_ctrl_.backup_port_en(x); }
    inline void     set_sw_mask(int ver, uint8_t val, int pos) { ver ? port_mask1_.mask(pos, val) : port_mask0_.mask(pos, val); }
    inline void     set_hw_mask(uint8_t val, int pos) { port_down_.port_down(pos, val); }
    inline bool     get_sw_mask(int ver, int pos) { return ver ? port_mask1_.mask(pos) : port_mask0_.mask(pos); }
    inline bool     get_hw_mask(int pos) { return port_down_.port_down(pos); }
    inline void     hw_mask_port_down(int port) { set_hw_mask(1, (port >> 7)*kLocalPortWidth + (port & 0x7F)); }
  private:

    std::array<int, kFifoCnt>                            fifo_base_ = {{0x000,0x100,0x200,0x300}};
    std::array<int, kFifoCnt>                            fifo_rows_ = {{0x100,0x100,0x100,0x100}};
    std::array<std::array<uint32_t, kMitSize>, MemUtils::pre_mit_mem_cnt()>  mit_ = {};
    mutable std::array<std::mutex, MemUtils::pre_mit_mem_cnt()>              mit_mutex_ = {};
    std::array<std::array<int, MemUtils::pre_pbt_mem_size()>, 2>               bpt_ = {};
    // No mutex for the LAG and Prune Mask tables as they are written in pieces
    // anyways through the host interface.
    std::array<std::array<BitVector<MemUtils::pre_lag_mem_entry_size()>, MemUtils::pre_lag_mem_size()>, 2>  lag_ = {};
    std::array<std::array<int, MemUtils::pre_lit_np_mem_size()>, 2>                lag_npl_ = {};
    std::array<std::array<int, MemUtils::pre_lit_np_mem_size()>, 2>                lag_npr_ = {};
    std::array<std::array<BitVector<MemUtils::pre_pmt_mem_size()>, MemUtils::pre_yids()>, 2>  pmt_ = {};

    PacketReplicationEngineMemRdm           mem_rdm_;
    std::array<PacketReplicationEngineMemMit*, MemUtils::pre_mit_mem_cnt()> mem_mit_;
    std::array<PacketReplicationEngineMemPbt*, MemUtils::pre_pbt_mem_cnt()> mem_pbt_;
    std::array<PacketReplicationEngineMemLagNp*, MemUtils::pre_lag_np_mem_cnt()> mem_lag_np_;
    std::array<PacketReplicationEngineMemLag*, MemUtils::pre_lag_mem_cnt()> mem_lag0_;
    std::array<PacketReplicationEngineMemLag*, MemUtils::pre_lag_mem_cnt()> mem_lag1_;
    std::array<PacketReplicationEngineMemPmt*, MemUtils::pre_pmt_mem_cnt()> mem_pmt0_;
    std::array<PacketReplicationEngineMemPmt*, MemUtils::pre_pmt_mem_cnt()> mem_pmt1_;
    PacketReplicationEngineMemFifo          mem_fifo_;
    register_classes::PreCommonCtrlMutable      common_ctrl_;
    register_classes::PrePruneRidMutable        prune_rid_;
    register_classes::PrePortMaskMutable        port_mask0_;
    register_classes::PrePortMaskMutable        port_mask1_;
    register_classes::PrePortDownMutable        port_down_;
    register_classes::PrePipeIntStatusMutable   pipe_int_status_;
    register_classes::PreRdmBlkId               rdm_blk_id_;
  };
}
#endif
