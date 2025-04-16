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

#ifndef _SHARED_RMT_OBJECT_MANAGER_
#define _SHARED_RMT_OBJECT_MANAGER_

#include <list>
#include <vector>
#include <set>
#include <memory>
#include <atomic>
#include <mutex>
#include <common/rmt.h>
#include <rmt-typedefs.h>
#include <rdm.h>
#include <rmt-defs.h>
#include <rmt-string-map.h>
#include <tbus-regs.h>
#include <model_core/rmt-phv-modification.h>

namespace model_core {
enum class Severity;
enum class Context;
}

namespace MODEL_CHIP_NAMESPACE {

  class Chip;
  class DeparserBlock;
  class I2QueueingMetadata;
  class E2MacMetadata;
  class Instr;
  class LearningFilter;
  class Mau;
  class MauIO;
  class Packet;
  class PacketBuffer;
  class PacketReplicationEngine;
  class PacketReplicationEngineRegCom;
  class ParserBlock;
  class Ipb;
  class IpbCounters;
  class Epb;
  class EpbCounters;
  class Mac;
  class MacFactory;
  class Mirror;
  class P4NameLookup;
  class Phv;
  class PhvFactory;
  class Pipe;
  class Port;
  class Queueing;
  class RmtOpHandler;
  class RmtPacketCoordinator;
  class RmtSweeper;
  class PktGen;
  class EgressBuf;
  class S2p;
  class P2s;
  class ParserArbiter;
  class TmRegs;
  class TmRegs2;
  class TmSchModelWrapper;
  class OtherChipObjects;
  template<typename T> class RmtSingletonFactory;
  enum class Gress;
  // thread local struct - used to store context for debug log
  typedef struct rmt_thd_info_ {
      Packet  *ig_packet;  // ingress pkt - used for packet_id or signature
      Packet  *eg_packet;  // egress pkt - used for packet_id or signature
      int      pipe;       // used by DefaultLogger to log to correct pipe
  } rmt_thd_info_t;

  extern thread_local rmt_thd_info_t rmt_thd_info;


  class RmtObjectManager : public DefaultLogger {

 public:
    static constexpr int kMausPerPipe = RmtDefs::kStagesMax;
    static constexpr int kLogPrefixMaxLen = 32;

 private:
    static constexpr int     kPortsTotal = RmtDefs::kPortsTotal;
    static constexpr int     kPipes = RmtDefs::kPipesMax;
    static constexpr uint32_t kPipesEn = (1<<kPipes)-1;
    static constexpr uint8_t kNumStages = RmtDefs::kStagesMax;
    static constexpr int     kMacsTotal = (kPipes * RmtDefs::kPhysMacs) + 1;
    static constexpr int     kParsersPerPipe = RmtDefs::kParsers;
    static constexpr int     kIpbsPerPipe = RmtDefs::kIpbs;
    static constexpr int     kEpbsPerPipe = RmtDefs::kEpbs;
    static constexpr int     kPktGenPortsPerPipe = RmtDefs::kPktGenPortsPerPipe;
    static constexpr int     kParsersTotal = kPipes * kParsersPerPipe;
    static constexpr int     kIpbsTotal = kPipes * kIpbsPerPipe;
    static constexpr int     kEpbsTotal = kPipes * kEpbsPerPipe;
    static constexpr int     kMausTotal = kPipes * kMausPerPipe;
    static constexpr int     kMauIOsTotal = kPipes * (kMausPerPipe+1);
    static constexpr int     kPktGenTotal = kPipes;
    static constexpr int     kPresTotal = RmtDefs::kPresTotal;

 public:
    RmtObjectManager(int chipIndex, bool initAll,
                     uint32_t pipes_en, uint8_t num_stages, Chip *chip);
    RmtObjectManager(int chipIndex, bool initAll);
    RmtObjectManager(int chipIndex);
    RmtObjectManager();
    ~RmtObjectManager();
    RmtObjectManager(const RmtObjectManager& other) = delete; // XXX

    void reset(bool initAll);
    bool is_multi_chip();
    bool is_thread_per_pipe();
    int  num_threads();
    bool is_multi_thread();

    // Return num pipes/stages, whether pipe/stage is valid etc
    int  num_pipes()           const { return kPipes; }
    int  num_stages()          const { return num_stages_; }
    bool is_pipe_valid(int p)  const {
      return ((p >= 0) && (p < num_pipes()) && ((pipes_en_ & (1<<p)) != 0));
    }
    bool is_stage_valid(int s) const { return ((s >= 0) && (s < num_stages())); }
    // Number of stages to use - may be restricted for test purposes
    int num_stages_to_use(int chipIndex);

    // Return pointer to Chip
    Chip *chip()  const { return chip_; }

    PacketBuffer *pktbuf_create(const uint8_t buf[], size_t size);
    PacketBuffer *pktbuf_create(const std::shared_ptr<uint8_t>& buf, size_t size);
    void pktbuf_delete(PacketBuffer *pb);

    Packet *pkt_create();
    Packet *pkt_create(PacketBuffer *pb);
    Packet *pkt_create(const uint8_t buf[], size_t len);
    Packet *pkt_create(const std::shared_ptr<uint8_t>& buf, size_t len);
    Packet *pkt_create(const std::string hexstr);
    Packet *pkt_create(const char* hexstr);
    Packet *pkt_last_deleted();
    void pkt_delete(Packet *p);

    Phv *phv_create();
    void phv_delete(Phv *phv);

    Instr *instr_create();
    void instr_delete(Instr *instr);

    OtherChipObjects *other_chip_objects_get();
    void other_chip_objects_delete();

    Queueing *queueing_get();
    void queueing_delete();

    Rdm *rdm_get();
    void rdm_delete();

    PacketReplicationEngineRegCom* pre_reg_com_get();
    void pre_reg_com_delete();
    PacketReplicationEngine *pre_get(int pre);
    void pre_delete(int pre);
    bool pre_fifos_empty();
    void pre_start();
    void pre_stop();

    TmSchModelWrapper* sch_model_wrap_get(int sch);
    void sch_model_wrap_delete(int sch);
    void sch_model_wrap_run(int sch);

    RmtPacketCoordinator* packet_coordinator_get();
    void packet_coordinator_delete();

    RmtSweeper *sweeper_get();
    void sweeper_delete();


    inline void     version_set(uint32_t v) { version_ = v; }
    inline uint32_t version_get()           { return version_; }
    inline uint64_t timestamp_get()         { return time_get_cycles(); }

    inline int  chip_index()       const { return chip_index_; }
    inline int  ipipe_table_id_0() const { return ipipe_table_id_0_; }
    inline int  epipe_table_id_0() const { return epipe_table_id_0_; }


    inline Mac *mac_lookup(int mi) {
      return macs_[mi];
    }
    inline void mac_set(int mi, Mac *mac) {
      macs_[mi] = mac;
    }

    inline const PortConfig *portconfig_lookup(int pi) {
      return portconfigs_[pi];
    }
    inline void portconfig_set(int pi, const PortConfig *portconfig) {
      portconfigs_[pi] = portconfig;
    }

    inline Port   *port_lookup(int pi)          { return ports_[pi]; }
    inline void    port_set(int pi, Port *port) { ports_[pi] = port; }
    inline Pipe   *pipe_lookup(int pi)          { return pipes_[pi]; }
    inline void    pipe_set(int pi, Pipe *pipe) { pipes_[pi] = pipe; }

    inline ParserBlock *parser_lookup(int pipeIndex, int parserIndex) {
      if (!is_pipe_valid(pipeIndex)) return NULL;
      if ((parserIndex < 0) || (parserIndex >= kParsersPerPipe)) return NULL;
      return parsers_[parser_array_index(pipeIndex,parserIndex)];
    }
    inline void parser_set(int pipeIndex, int parserIndex, ParserBlock *parser) {
      parsers_[parser_array_index(pipeIndex,parserIndex)] = parser;
    }

    inline void ipb_set(int pipeIndex, int ipbIndex, Ipb *ib) {
      ipbs_[ipb_array_index(pipeIndex,ipbIndex)] = ib;
    }
    Ipb *ipb_lookup(int pipeIndex,
                    int ipbIndex);
    IpbCounters *ipb_counters_lookup(int pipeIndex,
                                     int ipbIndex,
                                     int ipbChanIndex);

    inline Epb *epb_lookup(int pipeIndex, int epbIndex) {
      if (!is_pipe_valid(pipeIndex)) return NULL;
      if ((epbIndex < 0) || (epbIndex >= kEpbsPerPipe)) return NULL;
      return epbs_[epb_array_index(pipeIndex,epbIndex)];
    }
    inline void epb_set(int pipeIndex, int epbIndex, Epb *eb) {
      epbs_[epb_array_index(pipeIndex,epbIndex)] = eb;
    }
    EpbCounters *epb_counters_lookup(int pipeIndex,
                                     int epbIndex,
                                     int epbChanIndex);

    inline Mirror *mirror_lookup(int pipeIndex) {
      if (!is_pipe_valid(pipeIndex)) return NULL;
      return mirrors_[pipeIndex];
    }
    inline void mirror_set(int pipeIndex, Mirror *mr) {
      if (!is_pipe_valid(pipeIndex)) return;
      mirrors_[pipeIndex] = mr;
    }

    inline Mau *mau_lookup(int pipeIndex, int mauIndex) {
      pipeIndex = RmtDefs::map_mau_pipe(pipeIndex, pipes_en_);
      if (!is_pipe_valid(pipeIndex)) return NULL;
      if ((mauIndex < 0) || (mauIndex >= kMausPerPipe)) return NULL;
      return maus_[mau_array_index(pipeIndex,mauIndex)];
    }
    inline void mau_set(int pipeIndex, int mauIndex, Mau *mau) {
      pipeIndex = RmtDefs::map_mau_pipe(pipeIndex, pipes_en_);
      maus_[mau_array_index(pipeIndex,mauIndex)] = mau;
    }

    inline MauIO *mau_io_lookup(int pipeIndex, int mauIOIndex) {
      pipeIndex = RmtDefs::map_mau_pipe(pipeIndex, pipes_en_);
      if (!is_pipe_valid(pipeIndex)) return NULL;
      if ((mauIOIndex < 0) || (mauIOIndex >= (kMausPerPipe+1))) return NULL;
      return mau_ios_[mau_io_array_index(pipeIndex,mauIOIndex)];
    }
    inline void mau_io_set(int pipeIndex, int mauIOIndex, MauIO *mau_io) {
      pipeIndex = RmtDefs::map_mau_pipe(pipeIndex, pipes_en_);
      mau_ios_[mau_io_array_index(pipeIndex,mauIOIndex)] = mau_io;
    }

    inline DeparserBlock *deparser_lookup(int pi) {
      if (!is_pipe_valid(pi)) return NULL;
      return deparsers_[pi];
    }
    inline void deparser_set(int pi, DeparserBlock *deparser) {
      deparsers_[pi] = deparser;
    }

    inline LearningFilter *learning_filter_lookup(int pi) {
      pi = RmtDefs::map_lfltr_pipe(pi, pipes_en_);
      if (!is_pipe_valid(pi)) return NULL;
      return learning_filters_[pi];
    }
    inline void learning_filter_set(int pi, LearningFilter *learning_filter) {
      pi = RmtDefs::map_lfltr_pipe(pi, pipes_en_);
      learning_filters_[pi] = learning_filter ;
    }

    inline PktGen *pktgen_lookup(int pi) {
      if (!is_pipe_valid(pi)) return NULL;
      return pktgen_[pi];
    }
    inline void pktgen_set(int pipeIndex, PktGen *pg) {
      pktgen_[pipeIndex] = pg;
    }

    inline EgressBuf *egress_buf_lookup(int pipeIndex) {
      if (!is_pipe_valid(pipeIndex)) return NULL;
      return egress_bufs_[pipeIndex];
    }
    inline void egress_buf_set(int pipeIndex, EgressBuf *egress_buf) {
      egress_bufs_[pipeIndex] = egress_buf;
    }

    inline S2p *s2p_lookup(int pipeIndex) {
      if (!is_pipe_valid(pipeIndex)) return NULL;
      return s2p_[pipeIndex];
    }
    inline void s2p_set(int pipeIndex, S2p *s2p) {
      s2p_[pipeIndex] = s2p;
    }

    inline P2s *p2s_lookup(int pipeIndex) {
      if (!is_pipe_valid(pipeIndex)) return NULL;
      return p2s_[pipeIndex];
    }
    inline void p2s_set(int pipeIndex, P2s *p2s) {
      p2s_[pipeIndex] = p2s;
    }

    inline ParserArbiter *parser_arbiter_lookup(int pipeIndex) {
      if (!is_pipe_valid(pipeIndex)) return NULL;
      return parser_arbiter_[pipeIndex];
    }
    inline void parser_arbiter_set(int pipeIndex, ParserArbiter *ptr) {
      parser_arbiter_[pipeIndex] = ptr;
    }
    // Combined lookup/create/set funcs
    bool                get_mac_info(int portIndex, int sku, int *mac_index, int *mac_chan);
    Mac                *mac_get(int macIndex);
    const PortConfig   *portconfig_get(int portIndex);
    Port               *port_get(int portIndex);
    ParserBlock        *parser_get(int pipeIndex, int parserIndex);
    Ipb                *ipb_get(int pipeIndex, int ipbIndex);
    Epb                *epb_get(int pipeIndex, int epbIndex);
    Pipe               *pipe_get(int pipeIndex, int nMaus=-1);
    Mau                *mau_get(int pipeIndex, int mauIndex);
    MauIO              *mau_io_get(int pipeIndex, int mauIOIndex);
    DeparserBlock      *deparser_get(int pipeIndex);
    LearningFilter     *learning_filter_get(int pipeIndex);
    MacFactory         *mac_factory_get();
    PhvFactory         *phv_factory_get();
    RmtOpHandler       *op_handler_get();
    Mirror             *mirror_get(int pipeIndex);
    PktGen             *pktgen_get(int pipeIndex);
    EgressBuf          *egress_buf_get(int pipeIndex);
    S2p                *s2p_get(int pipeIndex);
    P2s                *p2s_get(int pipeIndex);
    ParserArbiter      *parser_arbiter_get(int pipeIndex);
    TMType             *tm_get();

    void set_ipipe_table_id_0(int table_id_0);
    void set_epipe_table_id_0(int table_id_0);

    // Allow evaluate_all to be set per-pipe
    bool evaluate_all(int pipeIndex);
    void set_evaluate_all(int pipeIndex, bool tf, bool dotest=false);
    bool evaluate_all();
    void set_evaluate_all(bool tf, bool dotest=false);


    void chip_init_all();
    void chip_free_all();
    inline void set_chip_free_all_on_exit(bool tf=true) {
      chip_free_all_on_exit_ = tf;
    }

    // Some funcs to facilitate logging/debugging
    uint64_t register_object(RmtLogger *obj);
    void unregister_object(RmtLogger *obj);
    void set_initial_log_flags(uint64_t log_flags_0);

    void update_log_flags(uint64_t pipes, uint64_t stages,
                          uint64_t types, uint64_t rows_tabs, uint64_t cols,
                          uint64_t or_log_flags, uint64_t and_log_flags);
    void set_log_flags(uint64_t f) override { RmtLogger::set_log_flags(f); }
    inline void set_log_flags(uint64_t pipes, uint64_t stages,
                              uint64_t types, uint64_t rows_tabs, uint64_t cols,
                              uint64_t log_flags) {
      update_log_flags(pipes, stages, types, rows_tabs, cols,
                       log_flags, UINT64_C(0xFFFFFFFFFFFFFFFF));
    }
    inline void clear_log_flags(uint64_t pipes, uint64_t stages,
                              uint64_t types, uint64_t rows_tabs, uint64_t cols,
                              uint64_t log_flags) {
      update_log_flags(pipes, stages, types, rows_tabs, cols,
                       UINT64_C(0), ~log_flags);
    }
    uint64_t log_flags(RmtLogger *obj);
    bool log_enabled(const RmtLogger *obj);
    uint32_t dump_stats(bool do_print=true);
    void get_mau_info(int pipe, int stage, uint32_t *array, int array_size,
                      const char **name_array, bool reset=false);

    inline unsigned n_packets() { return n_packets_.load(); }

    const std::string get_table_name(int pipe, int stage, int table_index);
    const std::string get_phv_name(int pipe, int stage, int container);
    const std::string get_gateway_condition_name(int pipe, int stage, int table_index);
    const bool get_gateway_has_attached_table(int pipe, int stage, int table_index);

    // All time in picoseconds (apart from time_get_cycles)
    void     time_increment(uint64_t increment);
    void     time_increment_cycles(uint64_t increment);
    void     time_set(uint64_t time);
    uint64_t time_get();
    uint64_t time_get_cycles();

    void     set_log_fn(rmt_logging_f log_fn);
    rmt_logging_f log_fn() { return log_fn_;}

    void set_log_prefix(const char *str) {
      char *newstr = (str == nullptr) ?nullptr: strndup(str, kLogPrefixMaxLen);
      char *oldstr = log_prefix_;
      log_prefix_ = newstr;
      if (oldstr != nullptr) free(oldstr);
    }
    const char *log_prefix() { return (log_prefix_ == nullptr) ?"" :log_prefix_; }


    void set_log_pipe_stage(uint64_t pipes, uint64_t stages) {
        log_pipes_ = pipes; // bitmap
        log_stages_ = stages; // bitmap
    }
    void update_log_type_levels(uint64_t pipes, uint64_t stages,
                                int log_type, uint64_t remove_levels, uint64_t add_levels);
    void set_log_pkt_signature(int offset, int len, bool use_pkt_sig) {
        pkt_sig_off_ = offset;
        pkt_sig_len_ = len;
        use_pkt_sig_ = use_pkt_sig;
    }
    bool log_use_pkt_sig() {
        return use_pkt_sig_;
    }
    void log_pkt_sig_location(int &off, int &len) {
        off = pkt_sig_off_; len = pkt_sig_len_;
    }

    // JSON-based name-lookup information.
    const P4NameLookup& p4_name_lookup(int pipe) const;
    void p4_name_lookup_swap(int pipe, std::unique_ptr<P4NameLookup> *x);
    void ConfigureP4NameLookup(int pipe, const std::string &file_name);

    // Lookup/Destroy/Initialise string->int map
    int  string_map_lookup(const char *key, const char *value);
    std::vector<std::string> string_map_get_all_keys();
    void string_map_delete();
    void string_map_init();

    void log_message(const model_core::Severity severity, const std::string& message);
    void log_message(const uint64_t pkt_id, const int pipe_index, const Gress gress, const int rmt_type, const model_core::Severity severity, const std::string& message);
    void log_info(const std::string& message);
    void log_warning(const std::string& message);
    void log_error(const std::string& message);
    void log_phv(const int rmt_type, const Phv& phv);
    void log_mau_phv(const int stage, const Phv& phv);
    void log_packet(const int rmt_type, const Packet& packet);
    void log_parser_state(const uint64_t pkt_id, const int pipe, const Gress gress, const int state);
    void log_parser_extract(const uint64_t pkt_id, const int pipe, const Gress gress, const int phv_word, const int data, const bool tag_along);
    void log_parser_tcam_match(const uint64_t pkt_id, const int pipe, const Gress gress, const int index, const std::string& lookup);
    void log_deparser_metadata(const uint64_t pkt_id, const int pipe, const I2QueueingMetadata& metadata);
    void log_deparser_metadata(const uint64_t pkt_id, const int pipe, const E2MacMetadata& metadata);
    void log_deparser_pkt_drop(const uint64_t pkt_id, const int pipe, const Gress gress);
    void log_mau_gateway(const uint64_t pkt_id, const int pipe, const Gress gress, const int stage, const int table, const bool enabled, const bool match, const int next_table_stage, const int next_table_table);
    void log_mau_gateway(const uint64_t pkt_id, const int pipe, const Gress gress, const int stage, const int table, const uint32_t action_instr_addr, const int next_table_stage, const int next_table_table);
    void log_mau_table_hit(const uint64_t pkt_id, const int pipe, const Gress gress, const int stage, const int table, const int next_table_stage, const int next_table_stable, const uint32_t action_instr_addr, const uint32_t stats_addr, const bool stats_addr_consumed);
    void log_mau_stateful_alu(const uint64_t pkt_id, const int pipe, const Gress gress, const int stage, const int table, const int meter_alu, const int stateful_instr);
    void flush_event_log();

    int set_phv_modification(int pipe, int stage, model_core::RmtPhvModification::ModifyEnum which, model_core::RmtPhvModification::ActionEnum action, int index, uint32_t value);

 private:
    RmtObjectManager& operator=(const RmtObjectManager&){ return *this; } // XXX
    Packet *pkt_create_internal(Packet *p);
    void insert_log_object(RmtLogger *obj);

    inline int parser_array_index(int pipe, int parser) {
      return (pipe * kParsersPerPipe) + parser;
    }
    inline int ipb_array_index(int pipe, int ipb) {
      return (pipe * kIpbsPerPipe) + ipb;
    }
    inline int epb_array_index(int pipe, int epb) {
      return (pipe * kEpbsPerPipe) + epb;
    }
    inline int mau_array_index(int pipe, int mau) {
      return (pipe * kMausPerPipe) + mau;
    }
    inline int mau_io_array_index(int pipe, int mau_io) {
      return (pipe * (kMausPerPipe+1)) + mau_io;
    }

    int                                        chip_index_ = 0;
    int                                        ipipe_table_id_0_ = 0;
    int                                        epipe_table_id_0_ = 0;
    uint32_t                                   pipes_en_ = 0u;
    uint8_t                                    num_stages_ = 0;
    Chip                                      *chip_ = nullptr;
    bool                                       evaluate_all_ = RmtDefs::kEvaluateAllDefault;
    std::atomic<unsigned>                      n_phvs_ = {};
    std::atomic<unsigned>                      n_packets_ = {};
    std::atomic<unsigned>                      n_instrs_ = {};
    uint32_t                                   n_objects_ = 0u;
    uint32_t                                   version_ = 0u;
    uint64_t                                   log_flags_0_ = RmtDebug::kRmtDebugBadStuff;
    uint64_t                                   log_flags_ = 0;
    uint64_t                                   log_pipes_ = 0;
    uint64_t                                   log_stages_ = 0;
    uint64_t                                   log_types_ = 0;
    uint64_t                                   log_rows_tabs_ = 0;
    uint64_t                                   log_cols_ = 0;
    PhvFactory                                *phv_factory_ = nullptr;
    MacFactory                                *mac_factory_ = nullptr;
    RmtOpHandler                              *rmt_op_handler_ = nullptr;
    bool                                       chip_free_all_on_exit_ = true;
    bool                                       multi_threaded_ = false;
    std::mutex                                      mutex_;
    std::set< RmtLogger* >                          objects_ = { };
    std::array<Mac*,kMacsTotal>                     macs_ = { };
    std::array<const PortConfig*,kPortsTotal>       portconfigs_ = { };
    std::array<Port*,kPortsTotal>                   ports_ = { };
    std::array<ParserBlock*,kParsersTotal>          parsers_ = { };
    std::array<Pipe*,kPipes>                        pipes_ = { };
    std::array<Mau*,kMausTotal>                     maus_ = { };
    std::array<MauIO*,kMauIOsTotal>                 mau_ios_ = { };
    std::array<DeparserBlock*,kPipes>               deparsers_ = { };
    std::array<LearningFilter*,kPipes>              learning_filters_ = { };
    std::array<Ipb*,kIpbsTotal> ipbs_ = { };
    std::array<Epb*,kEpbsTotal>                     epbs_ = { };
    std::array<PktGen*, kPktGenTotal>               pktgen_ = { };
    std::array<EgressBuf*, kPipes>                  egress_bufs_ = { };
    std::array<S2p*, kPipes>                        s2p_ = { };
    std::array<P2s*, kPipes>                        p2s_ = { };
    std::array<ParserArbiter*, kPipes>              parser_arbiter_ = { };
    std::array<Mirror*,kPipes>                      mirrors_ = { };
    std::unique_ptr<Queueing>                       queueing_;
    std::unique_ptr<PacketReplicationEngineRegCom>  pre_reg_com_;
    std::array<PacketReplicationEngine*,kPresTotal> pres_ = { };
    std::unique_ptr<Rdm>                            rdm_;
    std::array<TmSchModelWrapper*, kPipes> sch_model_wraps_ { };
    std::array<std::unique_ptr<P4NameLookup>,kPipes> p4_name_lookup_;
    RmtPacketCoordinator                      *packet_coordinator_ = nullptr;
    RmtSweeper                                *sweeper_ = nullptr;
    rmt_logging_f                              log_fn_ = nullptr;
    char                                      *log_prefix_ = nullptr;
    Packet                                    *last_pkt_deleted_ = nullptr;
    int                                        pkt_sig_off_ = -8;
    int                                        pkt_sig_len_ = 8;
    bool                                       use_pkt_sig_ = false;
    TmRegs                                    *tm_regs_ = nullptr;
    TmRegs2                                   *tm_regs2_ = nullptr;
    TbusRegs                                  *tbus_regs_ = nullptr;
    OtherChipObjects                          *other_chip_objects_ = nullptr;
    RmtStringMap                               string_map_;
    RmtSingletonFactory<TMType>               *tm_factory_;
  };
}

#endif // _SHARED_RMT_OBJECT_MANAGER_
