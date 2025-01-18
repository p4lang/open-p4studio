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
#include <memory>
#include <common/rmt-assert.h>
#include <common/rmt-flags.h>
#include <common/rmt-util.h>
#include <chip.h>
#include <rmt-reg-version.h>
#include <rmt-object-manager.h>
#include <deparser-block.h>
#include <learning-filter.h>
#include <mac-factory.h>
#include <mirror.h>
#include <p4-name-lookup.h>
#include <packet-buffer.h>
#include <packet-replication-engine.h>
#include <pktgen.h>
#include <port.h>
#include <phv-factory.h>
#include <rmt-log.h>
#include <rmt-op-handler.h>
#include <rmt-packet-coordinator.h>
#include <rmt-sweeper.h>
#include <tm-regs.h>
#include <tbus-regs.h>
#include <model_core/event.h>
#include <event.h>
#include <model_core/event-writer.h>
#include <other-pipe-objects.h>
#include <parser-arbiter.h>
#include <other-tm-objects.h>
#include <rmt-factory.h>
#include <tm.h>
#include <model_core/log-buffer.h>
#include <model_core/rmt-phv-modification.h>

extern std::unique_ptr<model_core::Model> GLOBAL_MODEL;

namespace MODEL_CHIP_NAMESPACE {

  using W = model_core::EventWriter;

  RmtObjectManager::RmtObjectManager(int chipIndex, bool initAll,
                                     uint32_t pipes_en, uint8_t num_stages, Chip *chip)
      : DefaultLogger(nullptr, RmtTypes::kRmtTypeRmtObjectManager),
        chip_index_(chipIndex), pipes_en_(pipes_en), num_stages_(num_stages),
        chip_(chip), queueing_(nullptr), pre_reg_com_(nullptr), rdm_(nullptr),
        string_map_(this),
        tm_factory_(new RmtSingletonFactory<TMType>(this)) {
    // We can't call the DefaultLogger constructor with the objectmanager
    //  above because the objectmanager is not fully formed yet, so call
    //  with nullptr and then call set_object_manager(this) here.
    set_object_manager(this);
    for (int p = 0; p < num_pipes(); p++) {
      p4_name_lookup_[p] = std::unique_ptr<P4NameLookup>(new P4NameLookup(""));
    }
    reset(initAll);
  }

  RmtObjectManager::RmtObjectManager(int chipIndex, bool initAll) :
    RmtObjectManager(chipIndex, initAll, kPipesEn, kNumStages, nullptr) {}

  RmtObjectManager::RmtObjectManager(int chipIndex) :
    RmtObjectManager(chipIndex, false, kPipesEn, kNumStages, nullptr) {}

  RmtObjectManager::RmtObjectManager() :
    RmtObjectManager(0, false, kPipesEn, kNumStages, nullptr) {}

  RmtObjectManager::~RmtObjectManager() {
    RMT_LOG(RmtDebug::verbose(), "RmtObjectManager DELETE\n");
    chip_free_all();
    delete tm_factory_;
  }

  void RmtObjectManager::reset(bool initAll) {
    RMT_ASSERT(model_common::Util::is_little_endian()); // Move this check somewhere better?
    RMT_ASSERT((pipes_en_ != 0) && ((pipes_en_ & kPipesEn) == pipes_en_));
    RMT_ASSERT((num_stages_ > 0) && (num_stages_ <= RmtDefs::kStagesMax));
    // Allow num_stages_ to be less than MIN for high chip vals (as used by utests)
    RMT_ASSERT((num_stages_ >= RmtDefs::kStagesMin) || (chip_index_ > 200));

    printf("%s\n", RMT_CHIP_VERSION_LONG);
    RMT_LOG(RmtDebug::verbose(), "RmtObjectManager CREATE\n");
    log_flags_0_ = static_cast<uint64_t>(INITIAL_LOG_FLAGS|FIXED_LOG_FLAGS);
    if (initAll) chip_init_all();
    insert_log_object(this);
  }


  bool RmtObjectManager::is_multi_chip() {
    uint32_t flags = (chip_ != nullptr) ?chip_->GetFlags() :0u;
    return ((flags & RMT_FLAGS_MULTI_CHIP) != 0u);
  }
  bool RmtObjectManager::is_thread_per_pipe() {
    uint32_t flags = (chip_ != nullptr) ?chip_->GetFlags() :0u;
    return ((flags & RMT_FLAGS_THREAD_PER_PIPE) != 0u);
  }
  int RmtObjectManager::num_threads() {
    int kDefaultNumThreads = 1;
    int num_threads = 0;
    if (is_thread_per_pipe()) {
      // Limit number threads used to number of enabled pipes
      for (int p = 0; p < num_pipes(); p++) {
        if (is_pipe_valid(p)) num_threads++;
      }
    } else {
      // Only report >1 if we have a chip_
      num_threads = (chip_ != nullptr) ?kDefaultNumThreads :1;
    }
    RMT_ASSERT((num_threads > 0) && "No valid pipes");
    return num_threads;
  }
  bool RmtObjectManager::is_multi_thread() {
    return (num_threads() > 1);
  }


  int RmtObjectManager::num_stages_to_use(int chipIndex) {
    // For 'special' chips used to return value derived from chipIndex - but no more!
    // Now handled by test wrapper explicitly setting num_stages - see TestUtil CTOR
    RMT_ASSERT(chipIndex >= 0);
    return num_stages();
  }

  TMType *RmtObjectManager::tm_get() {
    return tm_factory_->get();
  }
  OtherChipObjects *RmtObjectManager::other_chip_objects_get() {
    return nullptr;
  }

  PacketBuffer *RmtObjectManager::pktbuf_create(const uint8_t buf[], size_t size) {
    PacketBuffer *pb = new PacketBuffer(buf, size);
    RMT_LOG(RmtDebug::verbose(), "Allocating packet buffer %p\n", pb);
    return pb;
  }
  PacketBuffer *RmtObjectManager::pktbuf_create(const std::shared_ptr<uint8_t>& buf, size_t size) {
    PacketBuffer *pb = new PacketBuffer(buf, size);
    RMT_LOG(RmtDebug::verbose(), "Allocating packet buffer %p\n", pb);
    return pb;
  }
  void RmtObjectManager::pktbuf_delete(PacketBuffer *pb) {
    RMT_LOG(RmtDebug::verbose(), "Deleting packet buffer %p\n", pb);
    delete pb;
  }

  Packet *RmtObjectManager::pkt_create_internal(Packet *p) {
    RMT_ASSERT_NOT_NULL(p);
    // XXX: Maybe clear last_pkt_deleted_ cache
    if (last_pkt_deleted_ == p) last_pkt_deleted_ = nullptr;
    n_packets_++;
    RMT_LOG(RmtDebug::verbose(), "Allocating packet %p\n", p);
    return p;
  }
  Packet *RmtObjectManager::pkt_create() {
    return pkt_create_internal(new Packet(this));;
  }
  Packet *RmtObjectManager::pkt_create(PacketBuffer *pb) {
    return pkt_create_internal(new Packet(this, pb));
  }
  Packet *RmtObjectManager::pkt_create(const uint8_t buf[], size_t len) {
    return pkt_create_internal(new Packet(this, buf, len));
  }
  Packet *RmtObjectManager::pkt_create(const std::shared_ptr<uint8_t>& buf, size_t len) {
    return pkt_create_internal(new Packet(this, buf, len));
  }
  Packet *RmtObjectManager::pkt_create(const std::string hexstr) {
    return pkt_create_internal(new Packet(this, hexstr));
  }
  Packet *RmtObjectManager::pkt_create(const char* hexstr) {
    return pkt_create_internal(new Packet(this, hexstr));
  }
  Packet *RmtObjectManager::pkt_last_deleted() {
    return last_pkt_deleted_;
  }
  void RmtObjectManager::pkt_delete(Packet *p) {
    if (nullptr != p) {
      last_pkt_deleted_ = p;
      n_packets_--;
      RMT_LOG(RmtDebug::verbose(), "Deleting packet %p\n", p);
      if (packet_coordinator_ != NULL) packet_coordinator_->pkt_freed(p);
      p->destroy();
      delete p;
    }
  }

  Phv *RmtObjectManager::phv_create() {
    PhvFactory *pf = phv_factory_get();
    Phv *phv =  pf->phv_create();
    n_phvs_++;
    RMT_LOG(RmtDebug::verbose(), "Allocating phv %p <%d>\n", phv, n_phvs_.load());
    return phv;
  }
  void RmtObjectManager::phv_delete(Phv *phv) {
    if (nullptr != phv) {
      n_phvs_--;
      RMT_LOG(RmtDebug::verbose(), "Deleting phv %p <%d>\n", phv, n_phvs_.load());
      PhvFactory *pfac = phv->pf();
      if (pfac != NULL) {
        pfac->phv_delete(phv);
      } else {
        phv->destroy();
        delete phv;
      }
    }
  }

  Instr *RmtObjectManager::instr_create() {
    Instr *instr = new Instr();
    RMT_LOG(RmtDebug::verbose(), "Allocating instr %p\n", instr);
    n_instrs_++;
    return instr;
  }
  void RmtObjectManager::instr_delete(Instr *instr) {
    RMT_LOG(RmtDebug::verbose(), "Deleting instr %p\n", instr);
    n_instrs_--;
    delete instr;
  }

  Queueing *RmtObjectManager::queueing_get() {
    if (!queueing_) {
      queueing_.reset(new Queueing(this));
    }
    return queueing_.get();
  }
  void RmtObjectManager::queueing_delete() {
    if (queueing_) {
      queueing_.reset();
    }
  }

  RmtPacketCoordinator* RmtObjectManager::packet_coordinator_get() {
    if (!packet_coordinator_) {
      int n_threads = num_threads();
      multi_threaded_ = (n_threads > 1);
      packet_coordinator_ = new RmtPacketCoordinator(this);
      packet_coordinator_->set_num_threads(n_threads);
    }
    return packet_coordinator_;
  }
  void RmtObjectManager::packet_coordinator_delete() {
    if (packet_coordinator_) {
      delete packet_coordinator_;
      packet_coordinator_ = NULL;
      multi_threaded_ = false;
    }
  }

  RmtSweeper* RmtObjectManager::sweeper_get() {
    if (!sweeper_) {
      sweeper_ = new RmtSweeper(this);
    }
    return sweeper_;
  }
  void RmtObjectManager::sweeper_delete() {
    if (sweeper_) {
      delete sweeper_;
      sweeper_ = NULL;
    }
  }

  Rdm* RmtObjectManager::rdm_get() {
    if (!rdm_) {
      rdm_.reset(new Rdm());
    }
    return rdm_.get();
  }
  void RmtObjectManager::rdm_delete() {
    rdm_.reset();
  }

  PacketReplicationEngineRegCom* RmtObjectManager::pre_reg_com_get() {
    if (nullptr == pre_reg_com_) {
      //Rdm *rdm = rdm_get();
      pre_reg_com_.reset(new PacketReplicationEngineRegCom(this));
      pre_reg_com_.get()->reset();
    }
    return pre_reg_com_.get();
  }
  void RmtObjectManager::pre_reg_com_delete() {
    pre_reg_com_.reset();
  }

  PacketReplicationEngine* RmtObjectManager::pre_get(int pre) {
    RMT_ASSERT(pre >= 0 && pre < kPresTotal);
    if (!pres_[pre]) {
      //PacketReplicationEngineRegCom *reg_com = pre_reg_com_get();
      pres_[pre] = new PacketReplicationEngine(this, pre);
    }
    return pres_[pre];
  }
  void RmtObjectManager::pre_delete(int pre) {
    RMT_ASSERT(pre >= 0 && pre < kPresTotal);
    if (pres_[pre]) {
      delete pres_[pre];
      pres_[pre] = NULL;
    }
  }
  bool RmtObjectManager::pre_fifos_empty() {
    for (int i=0; i < kPresTotal; ++i) {
      if (!pre_get(i)->fifos_empty())
        return false;
    }
    return true;
  }
  void RmtObjectManager::pre_start() {
    for (int i=0; i < kPresTotal; ++i) {
      pre_get(i)->start();
    }
  }
  void RmtObjectManager::pre_stop() {
    for (int i=0; i < kPresTotal; ++i) {
      pre_get(i)->stop(true);
    }
  }

  TmSchModelWrapper* RmtObjectManager::sch_model_wrap_get(int sch) {
    RMT_ASSERT(sch >= 0 && sch < kPipes);
    if (!sch_model_wraps_[sch]) {
      sch_model_wraps_[sch] = new TmSchModelWrapper(this, sch);
    }
    return (sch_model_wraps_[sch]);
  }

  void RmtObjectManager::sch_model_wrap_delete(int sch) {
    RMT_ASSERT(sch >= 0 && sch < kPipes);
    if (sch_model_wraps_[sch]) {
      delete sch_model_wraps_[sch];
      sch_model_wraps_[sch] = NULL;
    }
  }

  void RmtObjectManager::sch_model_wrap_run(int sch) {
    RMT_ASSERT(sch >= 0 && sch < kPipes);
    sch_model_wrap_get(sch)->run_sch_model();
  }


  bool RmtObjectManager::get_mac_info(int port_index, int sku, int *mac_index, int *mac_chan) {
    RMT_ASSERT((mac_index != nullptr) && (mac_chan != nullptr));
    if ((port_index < 0) || (port_index >= kPortsTotal)) return false;

    // Note we must pass *port* index to MacFactory as it
    // understands mapping from Ports to MacBlocks
    MacFactory *macf = mac_factory_get();
    return macf->get_mac_info(port_index, sku, mac_index, mac_chan);
  }

  Mac *RmtObjectManager::mac_get(int mac_index) {
    if ((mac_index < 0) || (mac_index >= kMacsTotal)) return NULL;
    Mac *mac = mac_lookup(mac_index);
    if (mac == NULL) {
      MacFactory *macf = mac_factory_get();
      mac = macf->mac_create(mac_index);
      mac_set(mac_index, mac);
    }
    return mac;
  }

  const PortConfig *RmtObjectManager::portconfig_get(int port_index) {
    if ((port_index < 0) || (port_index >= kPortsTotal)) return NULL;
    const PortConfig *portconfig = portconfig_lookup(port_index);
    if (portconfig == NULL) {
      portconfig = &RmtDefs::kPort_Config_Default[port_index];
      portconfig_set(port_index, portconfig);
    }
    return portconfig;
  }

  Port *RmtObjectManager::port_get(int port_index) {
    if ((port_index < 0) || (port_index >= kPortsTotal)) return NULL;
    Port *port = port_lookup(port_index);
    if (port == NULL) {
      int mac_index = -1, mac_chan = -1;
      (void)get_mac_info(port_index, chip_->GetSku(), &mac_index, &mac_chan);

      // port = new Port(this, port_index, *portconfig_get(port_index));
      port = new Port(this, port_index, mac_index, mac_chan);
      port_set(port_index, port);
      port->set_mac(mac_get(mac_index));
      port->set_pipe(pipe_get(port->pipe_index()));
      port->set_parser(parser_get(port->pipe_index(),port->parser_index()));
      port->set_deparser(deparser_get(port->pipe_index()));
      port->set_ipb(ipb_get(port->pipe_index(), port->ipb_index()));
      port->set_epb(epb_get(port->pipe_index(), port->epb_index()));
      port->set_mirror(mirror_get(port->pipe_index()));
    }
    return port;
  }

  Pipe *RmtObjectManager::pipe_get(int pipe_index, int nMaus) {
    // If we're using a high chip index, limit number MAUs to speed things up
    if (nMaus == -1) nMaus = num_stages_to_use(chip_index_);
    if (!is_pipe_valid(pipe_index)) return NULL;
    if ((nMaus < 0) || (nMaus > num_stages())) return NULL;
    Pipe *pipe = pipe_lookup(pipe_index);
    if (pipe == NULL) {
      pipe = new Pipe(this, pipe_index, RmtDefs::kPipe_Config[pipe_index]);
      pipe->set_ipipe_table_id_0(ipipe_table_id_0());
      pipe->set_epipe_table_id_0(epipe_table_id_0());
      pipe_set(pipe_index, pipe);
      learning_filter_get(pipe_index);
    }
    if (pipe != NULL) {
      for (int i = 0; i < nMaus; i++) (void)mau_get(pipe_index, i);
    }
    return pipe;
  }

  ParserBlock *RmtObjectManager::parser_get(int pipe_index, int parser_index) {
    if (!is_pipe_valid(pipe_index)) return NULL;
    if ((parser_index < 0) || (parser_index >= kParsersPerPipe)) return NULL;
    ParserBlock *parser = parser_lookup(pipe_index,parser_index);
    if (parser == NULL) {
      parser = new ParserBlock(this, pipe_index, parser_index, RmtDefs::kParser_Config[parser_index]);
      parser_set(pipe_index, parser_index, parser);
      parser->set_pipe(pipe_get(pipe_index));
    }
    return parser;
  }

  Ipb *RmtObjectManager::ipb_get(int pipe_index, int ipb_index) {
    if (!is_pipe_valid(pipe_index)) return NULL;
    if ((ipb_index < 0) || (ipb_index >= kIpbsPerPipe)) return NULL;
    Ipb *ipb = ipb_lookup(pipe_index,ipb_index);
    if (ipb == NULL) {
      ipb = new Ipb(this, pipe_index, ipb_index);
      ipb_set(pipe_index, ipb_index, ipb);
      ipb->set_pipe(pipe_get(pipe_index));
    }
    return ipb;
  }

  Ipb *RmtObjectManager::ipb_lookup(int pipeIndex, int ipbIndex) {
    Ipb *ipb = nullptr;
    if (is_pipe_valid(pipeIndex) && (ipbIndex >= 0) && (ipbIndex < kIpbsPerPipe)) {
      ipb = ipbs_[ipb_array_index(pipeIndex,ipbIndex)];
    }
    if (nullptr == ipb) {
      // XXX: check in case of invalid indices
      RMT_LOG_WARN(
          "IPB not found for pipe index %d, ipb index %d\n",
          pipeIndex, ipbIndex);
    }
    return ipb;
  }

  IpbCounters *RmtObjectManager::ipb_counters_lookup(int pipeIndex,
                                                     int ipbIndex,
                                                     int ipbChanIndex) {
    IpbCounters *ipb_counters = nullptr;
    Ipb*ipb = ipb_lookup(pipeIndex, ipbIndex);
    if (nullptr != ipb) ipb_counters = ipb->get_ipb_counters(ipbChanIndex);
    if (nullptr == ipb_counters) {
      // XXX: check in case of invalid indices
      RMT_LOG_WARN(
          "IPB counters not found for pipe index %d, ipb index %d, ipb chan %d\n",
          pipeIndex, ipbIndex, ipbChanIndex);
    }
    return ipb_counters;
  }

  Epb *RmtObjectManager::epb_get(int pipe_index, int epb_index) {
    if (!is_pipe_valid(pipe_index)) return NULL;
    if ((epb_index < 0) || (epb_index >= kEpbsPerPipe)) return NULL;
    Epb *epb = epb_lookup(pipe_index,epb_index);
    if (epb == NULL) {
      epb = new Epb(this, pipe_index, epb_index);
      epb_set(pipe_index, epb_index, epb);
      epb->set_pipe(pipe_get(pipe_index));
    }
    return epb;
  }

  EpbCounters *RmtObjectManager::epb_counters_lookup(int pipeIndex,
                                                     int epbIndex,
                                                     int epbChanIndex) {
    EpbCounters *epb_counters = nullptr;
    Epb *epb = epb_lookup(pipeIndex, epbIndex);
    if (nullptr != epb) epb_counters = epb->get_epb_counters(epbChanIndex);
    if (nullptr == epb_counters) {
      // XXX: check in case of invalid indices
      RMT_LOG_WARN(
          "EPB counters not found for pipe index %d, epb index %d, epb chan %d\n",
          pipeIndex, epbIndex, epbChanIndex);
    }
    return epb_counters;
  }


  Mirror *RmtObjectManager::mirror_get(int pipe_index) {
    if (!is_pipe_valid(pipe_index)) return NULL;
    Mirror *mirror = mirror_lookup(pipe_index);
    if (mirror == NULL) {
      mirror = new Mirror(this, pipe_index);
      mirror_set(pipe_index, mirror);
      mirror->set_pipe(pipe_get(pipe_index));
    }
    return mirror;
  }

  Mau *RmtObjectManager::mau_get(int pipe_index, int mau_index) {
    pipe_index = RmtDefs::map_mau_pipe(pipe_index, pipes_en_);
    if (!is_pipe_valid(pipe_index)) return NULL;
    if (!is_stage_valid(mau_index)) return NULL;
    Mau *mau = mau_lookup(pipe_index, mau_index);
    if (mau == NULL) {
      mau = new Mau(this, pipe_index, mau_index, RmtDefs::kMau_Config[mau_index]);
      mau_set(pipe_index, mau_index, mau);
      mau->set_pipe(pipe_get(pipe_index,0)); // Pass 0 so we don't make more MAUs
      // Link ourself to previous MAU if there is one (and to next MAU)
      if (mau_index > 0) {
        Mau *prev_mau = mau_lookup(pipe_index, mau_index-1);
        if (prev_mau != NULL) mau->set_mau_previous(prev_mau);
      }
      if (mau_index < num_stages()-1) {
        Mau *next_mau = mau_lookup(pipe_index, mau_index+1);
        if (next_mau != NULL) next_mau->set_mau_previous(mau);
      }
    }
    return mau;
  }

  MauIO *RmtObjectManager::mau_io_get(int pipe_index, int mau_io_index) {
    pipe_index = RmtDefs::map_mau_pipe(pipe_index, pipes_en_);
    if (!is_pipe_valid(pipe_index)) return NULL;
    if ((mau_io_index < 0) || (mau_io_index >= (num_stages()+1))) return NULL;
    MauIO *mau_io = mau_io_lookup(pipe_index, mau_io_index);
    if (mau_io == NULL) {
      Mau *mau = mau_lookup(pipe_index, mau_io_index); // Find associated MAU
      mau_io = new MauIO(this, pipe_index, mau_io_index, mau);
      mau_io_set(pipe_index, mau_io_index, mau_io);
    }
    return mau_io;
  }

  DeparserBlock *RmtObjectManager::deparser_get(int pipe_index) {
    if (!is_pipe_valid(pipe_index)) return NULL;
    DeparserBlock *deparser = deparser_lookup(pipe_index);
    if (deparser == NULL) {
      deparser = new DeparserBlock(this, pipe_index);
      deparser_set(pipe_index, deparser);
      deparser->set_pipe(pipe_get(pipe_index));
    }
    return deparser;
  }

  PktGen *RmtObjectManager::pktgen_get(int pipe_index) {
    if (!is_pipe_valid(pipe_index)) return NULL;
    PktGen *pkt_gen = pktgen_lookup(pipe_index);
    if (pkt_gen == NULL) {
      pkt_gen = new PktGen(this, packet_coordinator_get(), pipe_index);
      pktgen_set(pipe_index,pkt_gen);
      pkt_gen->set_pipe(pipe_get(pipe_index));
    }
    return pkt_gen;
  }

  EgressBuf *RmtObjectManager::egress_buf_get(int pipe_index) {
    if (!is_pipe_valid(pipe_index)) return NULL;
    EgressBuf *ptr = egress_buf_lookup(pipe_index);
    if (ptr == NULL) {
      ptr = new EgressBuf(this, pipe_index);
      egress_buf_set(pipe_index, ptr);
    }
    return ptr;
  }

  S2p *RmtObjectManager::s2p_get(int pipe_index) {
    if (!is_pipe_valid(pipe_index)) return NULL;
    S2p *ptr = s2p_lookup(pipe_index);
    if (ptr == NULL) {
      ptr = new S2p(this, pipe_index);
      s2p_set(pipe_index, ptr);
    }
    return ptr;
  }

  P2s *RmtObjectManager::p2s_get(int pipe_index) {
    if (!is_pipe_valid(pipe_index)) return NULL;
    P2s *ptr = p2s_lookup(pipe_index);
    if (ptr == NULL) {
      ptr = new P2s(this, pipe_index);
      p2s_set(pipe_index, ptr);
    }
    return ptr;
  }

  ParserArbiter *RmtObjectManager::parser_arbiter_get(int pipe_index) {
    if (!is_pipe_valid(pipe_index)) return NULL;
    ParserArbiter *ptr = parser_arbiter_lookup(pipe_index);
    if (ptr == NULL) {
      ptr = new ParserArbiter(this, pipe_index);
      parser_arbiter_set(pipe_index, ptr);
    }
    return ptr;
  }

  LearningFilter *RmtObjectManager::learning_filter_get(int pipe_index) {
    pipe_index = RmtDefs::map_lfltr_pipe(pipe_index, pipes_en_);
    if (!is_pipe_valid(pipe_index)) return NULL;
    LearningFilter *learning_filter = learning_filter_lookup(pipe_index);
    if (learning_filter == NULL) {
      learning_filter = new LearningFilter(this, pipe_index);
      learning_filter_set(pipe_index, learning_filter);
      learning_filter->set_pipe(pipe_get(pipe_index));
    }
    return learning_filter;
  }

  PhvFactory *RmtObjectManager::phv_factory_get() {
    if (phv_factory_ == NULL)
      phv_factory_ = new PhvFactory(this);
    RMT_ASSERT(phv_factory_ != NULL);
    return phv_factory_;
  }

  MacFactory *RmtObjectManager::mac_factory_get() {
    if (mac_factory_ == NULL)
      mac_factory_ = new MacFactory(this);
    RMT_ASSERT(mac_factory_ != NULL);
    return mac_factory_;
  }

  RmtOpHandler *RmtObjectManager::op_handler_get() {
    if (rmt_op_handler_ == NULL)
      rmt_op_handler_ = new RmtOpHandler(this);
    RMT_ASSERT(rmt_op_handler_ != NULL);
    return rmt_op_handler_;
  }

  void RmtObjectManager::set_ipipe_table_id_0(int table_id_0) {
    ipipe_table_id_0_ = table_id_0;
    for (int i = 0; i < num_pipes(); i++) {
      if (is_pipe_valid(i)) {
        Pipe *p = pipe_lookup(i);
        if (p != NULL) p->set_ipipe_table_id_0(table_id_0);
      }
    }
  }

  void RmtObjectManager::set_epipe_table_id_0(int table_id_0) {
    epipe_table_id_0_ = table_id_0;
    for (int i = 0; i < num_pipes(); i++) {
      if (is_pipe_valid(i)) {
        Pipe *p = pipe_lookup(i);
        if (p != NULL) p->set_epipe_table_id_0(table_id_0);
      }
    }
  }


  bool RmtObjectManager::evaluate_all(int pipeIndex) {
    if (!is_pipe_valid(pipeIndex)) return false;
    Pipe *p = pipe_lookup(pipeIndex);
    return (p != NULL) ?p->evaluate_all() :false;
  }
  void RmtObjectManager::set_evaluate_all(int pipeIndex, bool tf, bool dotest) {
    if (!is_pipe_valid(pipeIndex)) return;
    Pipe *p = pipe_lookup(pipeIndex);
    if (p != NULL) p->set_evaluate_all(tf, dotest);
  }
  bool RmtObjectManager::evaluate_all() {
    return evaluate_all_;
  }
  void RmtObjectManager::set_evaluate_all(bool tf, bool dotest) {
    for (int pipeIndex = 0; pipeIndex < num_pipes(); pipeIndex++) {
      if (is_pipe_valid(pipeIndex)) {
        set_evaluate_all(pipeIndex, tf, dotest);
      }
    }
    evaluate_all_ = tf;
  }


  void RmtObjectManager::chip_init_all() {
    int stagesPerPipe = num_stages_to_use(chip_index());

    tm_regs_ = new TmRegs(chip_index());
    tm_regs2_ = new TmRegs2(chip_index());
    tbus_regs_ = new TbusRegs(chip_index());
    (void)tm_get(); // XXX: create TM at init
    (void)queueing_get();
    (void)packet_coordinator_get();
    (void)sweeper_get();
    (void)rdm_get();
    (void)pre_reg_com_get();
    for (int i=0; i < kPresTotal; ++i) {
      (void)pre_get(i);
    }

    // Create a port obj for all enabled pipes/macs/mac_chans
    // Note, initially we create 4x (JBay 8x) mac_chans per MAC
    //   ie all 10G ports - this can be changed later
    // This creates all necessary pipes etc
    for (int i = 0; i < num_pipes(); i++) {
      if (is_pipe_valid(i)) {
        for (int j = Port::kPortGroupMin; j <= Port::kPortGroupMax; j++) {
          for (int k = Port::kPortChanMin; k <= Port::kPortChanMax; k++) {
            (void)port_get(Port::make_port_index(i,j,k));
          }
        }
      }
    }
    for (int j = 0; j < num_pipes(); j++) {
      if (is_pipe_valid(j)) {
        for (int k = 0; k < stagesPerPipe; k++) {
          Mau *mau = mau_get(j, k);
          if (mau != NULL) mau->mau_init_all();
          (void)mau_io_get(j, k);
        }
        (void)mau_io_get(j, stagesPerPipe);
      }
    }
    // create the packet generators, egress_bufs and s2p/p2s converters
    for (int j = 0; j < num_pipes(); j++) {
      if (is_pipe_valid(j)) {
        (void)pktgen_get(j);
        (void)egress_buf_get(j);
        (void)s2p_get(j);
        (void)p2s_get(j);
      }
    }
  }


  // XXX: insert object into std::set, maybe taking a mutex first
  void RmtObjectManager::insert_log_object(RmtLogger *obj) {
    if (multi_threaded_) {
      std::unique_lock<std::mutex> lock(mutex_);
      objects_.insert(obj);
    } else {
      objects_.insert(obj);
    }
  }

  // All RmtLogger objects call this in their constructor
  // which allows update_log_flags to set logging on them.
  // The return value sets the initial value of the object log flags
  //
  uint64_t RmtObjectManager::register_object(RmtLogger *obj) {
    if (obj == this) return UINT64_C(0);
    if (RmtDebug::is_rmt_debug_enabled()) {
      n_objects_++;
      insert_log_object(obj);
      //printf("RmtObjectManager::register_object(%d)\n", n_objects_);
    }
#ifdef USE_PRINTF
    return UINT64_C(0xFFFFFFFFFFFFFFFF);
#else
    return log_flags_0_;
#endif
  }

  void RmtObjectManager::unregister_object(RmtLogger *obj) {
    if (RmtDebug::is_rmt_debug_enabled()) {
      if (multi_threaded_) {
        std::unique_lock<std::mutex> lock(mutex_);
        n_objects_--;
        objects_.erase(obj);
      } else {
        n_objects_--;
        objects_.erase(obj);
      }
    }
  }

  uint64_t RmtObjectManager::log_flags(RmtLogger *obj) {
#ifdef USE_PRINTF
    return UINT64_C(0xFFFFFFFFFFFFFFFF);
#else
    uint64_t NON = UINT64_C(0);
    uint64_t ONE = UINT64_C(1);
    uint64_t ALL = UINT64_C(0xFFFFFFFFFFFFFFFF);
    uint64_t chk_pipe = obj->pipe_is_valid(obj->pipe_index()) ?
                                ONE << obj->pipe_index() : ALL;
    uint64_t chk_stg = obj->stage_is_valid(obj->s_index()) ?
                                ONE << obj->s_index() : ALL;


    if (((log_pipes_ & chk_pipe) != NON) &&
        ((log_stages_ & chk_stg) != NON) &&
        ((log_types_ & (ONE << (obj->t_index() % 64))) != NON) &&
        ((log_rows_tabs_ & (ONE << obj->rt_index())) != NON) &&
        ((log_cols_ & (ONE << obj->c_index())) != NON)) {
      uint64_t flags = obj->log_flags();
      flags |= log_flags_;
      return flags;
    }
    return log_flags_0_;
#endif
  }
  bool RmtObjectManager::log_enabled(const RmtLogger *obj) {
    uint64_t NON = UINT64_C(0);
    uint64_t ONE = UINT64_C(1);
    uint64_t ALL = UINT64_C(0xFFFFFFFFFFFFFFFF);
    uint64_t chk_pipe = obj->pipe_is_valid(obj->pipe_index()) ?
                                ONE << obj->pipe_index() : ALL;
    uint64_t chk_stg = obj->stage_is_valid(obj->s_index()) ?
                                ONE << obj->s_index() : ALL;


    if (((log_pipes_ & chk_pipe) != NON) &&
        ((log_stages_ & chk_stg) != NON)) {
        return true;
    }
    return false;
  }
  // Setup the initial log flags returned to objects that
  // register with the RmtObjectManager. By default this
  // specifies logging of Fatal,Error and Warn messages
  void RmtObjectManager::set_initial_log_flags(uint64_t log_flags_0) {
    log_flags_0 |= static_cast<uint64_t>(FIXED_LOG_FLAGS);
    log_flags_0_ = log_flags_0;
    log_flags_ = log_flags_0;
  }

  // Configure logging for system RmtLogger objects
  // Pass in a mask of pipes/stages/rows_or_tables/columns/types
  // and any objects whose pipe/stage/row/col/type is set in the
  // passed masks gets its log_flags updated.
  //
  // NB. Object must satisfy all checks - if you don't care
  // what pipe/stage an object is in use 0xFFFFFFFFFFFFFFFF
  //
  void RmtObjectManager::update_log_flags(uint64_t pipes, uint64_t stages,
                                          uint64_t types, uint64_t rows_tabs, uint64_t cols,
                                          uint64_t or_log_flags, uint64_t and_log_flags) {
    or_log_flags  |= static_cast<uint64_t>(FIXED_LOG_FLAGS);
    and_log_flags |= static_cast<uint64_t>(FIXED_LOG_FLAGS);

    // Treat 0 as meaning ALL pipes/stages etc
    uint64_t ALL = UINT64_C(0xFFFFFFFFFFFFFFFF);
    uint64_t LOW = UINT64_C(0xFFFFFFFF);
    uint64_t ONE = UINT64_C(1);
    uint64_t NON = UINT64_C(0);

    if (pipes == NON) pipes = ALL;
    if (stages == NON) stages = ALL;
    if (types == NON) types = ALL;
    if (rows_tabs == NON) rows_tabs = ALL;
    if (cols == NON) cols = ALL;
    printf("RmtObjectManager::update_log_flags("
           "pipes=0x%016" PRIx64 " stages=0x%016" PRIx64 " types=0x%016" PRIx64
           " rows_tabs=0x%016" PRIx64 " cols=0x%016" PRIx64
           " OR=0x%08x 0x%08x  AND=0x%08x 0x%08x  VER=%s)\n",
           pipes, stages, types, rows_tabs, cols,
           static_cast<uint32_t>(or_log_flags >> 32),
           static_cast<uint32_t>(or_log_flags & LOW),
           static_cast<uint32_t>(and_log_flags >> 32),
           static_cast<uint32_t>(and_log_flags & LOW), RMT_CHIP_VERSION);

    auto it = objects_.begin();
    while (it != objects_.end()) {
      RmtLogger *obj = *it;
      if (((pipes & (ONE << obj->pipe_index())) != NON) &&
          ((stages & (ONE << obj->s_index())) != NON) &&
          ((types & (ONE << (obj->t_index() % 64))) != NON) &&
          ((rows_tabs & (ONE << obj->rt_index())) != NON) &&
          ((cols & (ONE << obj->c_index())) != NON)) {
        uint64_t flags = obj->log_flags();
        flags |= or_log_flags;
        flags &= and_log_flags;
        obj->set_log_flags(flags);
      }
      ++it;
    }
    log_pipes_ = pipes;
    log_stages_ = stages;
    log_types_ = types;
    log_rows_tabs_ = rows_tabs;
    log_cols_ = cols;
    log_flags_ |= or_log_flags;
    log_flags_ &= and_log_flags;
  }
  void RmtObjectManager::set_log_fn(rmt_logging_f log_fn) {
    log_fn_ = log_fn;
  }

  void RmtObjectManager::update_log_type_levels(uint64_t pipes,
                                                uint64_t stages,
                                                int log_type,
                                                uint64_t remove_levels,
                                                uint64_t add_levels) {
    uint64_t ONE = UINT64_C(1);
    uint64_t NON = UINT64_C(0);
    uint64_t ALL = UINT64_C(0xFFFFFFFFFFFFFFFF);

    for (int pipe = 0; pipe < 64; pipe++) {
      if (RMT_LOG_TYPE_P4 == log_type &&
          pipe_is_valid(pipe) &&
          (pipes & (ONE << pipe)) &&
          !p4_name_lookup(pipe).IsLoaded()) {
        RMT_P4_LOG(
            RmtDebug::kRmtDebugForce,
            "WARNING: updating P4 logging flags for pipe %d but P4 names have "
            "not been loaded.", pipe)
      }
    }

    RMT_TYPE_LOG(
        log_type, RmtDebug::kRmtDebugForce,
        "Updating %s log flags: clearing 0x%08x %08x, setting 0x%08x %08x\n",
        RmtDebug::log_string_for_type(log_type).c_str(),
        static_cast<uint32_t>(remove_levels >> 32),
        static_cast<uint32_t>(remove_levels & UINT64_C(0xFFFFFFFF)),
        static_cast<uint32_t>(add_levels >> 32),
        static_cast<uint32_t>(add_levels & UINT64_C(0xFFFFFFFF))
    );

    auto it = objects_.begin();
    while (it != objects_.end()) {
      RmtLogger *obj = *it;

      uint64_t chk_pipe = 0;
      if (obj->pipe_is_valid(obj->pipe_index())) {
        chk_pipe = ONE << obj->pipe_index();
      } else if (pipes == ALL) {
        chk_pipe = ALL;
      }
      uint64_t chk_stg = 0;
      if (obj->stage_is_valid(obj->s_index())) {
        chk_stg = ONE << obj->s_index();
      } else if (stages == ALL) {
        chk_stg = ALL;
      }
      // modify flags on objects that do not belong to a pipe/stage only
      // if operation is specified to use all 'ALL' pipes/stages respectively
      if (((pipes & chk_pipe) != NON) &&
          ((stages & chk_stg) != NON)) {
          obj->update_log_type_flags(log_type, remove_levels, add_levels);
      }
      ++it;
    }
  }

  // Dump out some stats so we can spot leaks
  uint32_t RmtObjectManager::dump_stats(bool do_print) {
    uint32_t phvs   = n_phvs_.load();
    uint32_t pkts   = n_packets_.load();
    uint32_t instrs = n_instrs_.load();
    uint32_t objs   = n_objects_;
    if (do_print) {
      printf("RmtObjectManager<%d> STATUS:\n", chip_index_);
      printf("n_phvs = %d\n", phvs);
      printf("n_packets = %d\n", pkts);
      printf("n_objects = %d\n", objs);
      printf("n_instrs = %d\n", instrs);
    }
    return phvs+pkts+instrs+objs;
  }


  // Get per-MAU informational counters - see mau-info-defs.h for list
  void RmtObjectManager::get_mau_info(int pipe, int stage,
                                      uint32_t *array, int array_size,
                                      const char **name_array, bool reset) {
    Mau *mau = mau_lookup(pipe, stage);
    if (mau != NULL) mau->mau_info_read(array, array_size, name_array, reset);
  }



  // At the minute just call sweeper - picoseconds
  void RmtObjectManager::time_increment(uint64_t increment) {
    RMT_ASSERT(sweeper_ != NULL);
    sweeper_->sweep_increment(increment);
  }
  void RmtObjectManager::time_increment_cycles(uint64_t increment) {
    RMT_ASSERT(sweeper_ != NULL);
    sweeper_->sweep_increment_cycles(increment);
  }

  void RmtObjectManager::time_set(uint64_t time) {
    RMT_ASSERT(sweeper_ != NULL);
    sweeper_->sweep(time);
  }
  uint64_t RmtObjectManager::time_get() {
    return (sweeper_ != NULL) ?sweeper_->get_time_now() :UINT64_C(0);
  }
  uint64_t RmtObjectManager::time_get_cycles() {
    return (sweeper_ != NULL) ?sweeper_->get_cycles_now() :UINT64_C(0);
  }




  const P4NameLookup&
  RmtObjectManager::p4_name_lookup(int pipe) const {
    RMT_ASSERT(nullptr != p4_name_lookup_[pipe].get());
    return *p4_name_lookup_[pipe].get();
  }
  void RmtObjectManager::p4_name_lookup_swap(int pipe, std::unique_ptr<P4NameLookup> *x) {
    x->swap(p4_name_lookup_[pipe]);
  }

  void
  RmtObjectManager::ConfigureP4NameLookup(int pipe, const std::string &file_name) {
    p4_name_lookup_[pipe].reset(new P4NameLookup(file_name));
  }

  const std::string
  RmtObjectManager::get_table_name(int pipe, int stage, int table_index) {
    return p4_name_lookup_[pipe]->GetTableName(stage, table_index);
  }

  const std::string
  RmtObjectManager::get_phv_name(int pipe, int stage, int container) {
    return p4_name_lookup_[pipe]->GetFieldName(stage, container);
  }

  const std::string
  RmtObjectManager::get_gateway_condition_name(int pipe, int stage, int table_index) {
    return p4_name_lookup_[pipe]->GetGatewayConditionName(stage, table_index);
  }

  const bool
  RmtObjectManager::get_gateway_has_attached_table(int pipe, int stage, int table_index) {
    return p4_name_lookup_[pipe]->GetGatewayHasAttachedTable(stage, table_index);
  }


  void RmtObjectManager::chip_free_all() {

    if (!chip_free_all_on_exit_) return;

    // Clear down the object list
    objects_.clear();

    // stop the packet gens before deleting anything else
    for (int p = 0; p < num_pipes(); p++) {
      if (is_pipe_valid(p)) {
        PktGen *pg = pktgen_lookup(p);
        if (pg != NULL) pg->stop();
      }
    }

    int stagesPerPipe = num_stages_to_use(chip_index());

    for (int i = 0; i < kPortsTotal; i++) {
      Port *port = port_lookup(i);
      if (port != NULL) delete port;
      port_set(i, NULL);
    }
    for (int p = 0; p < num_pipes(); p++) {
      if (is_pipe_valid(p)) {
        for (int m = 0; m < stagesPerPipe+1; m++) {
          MauIO *mauIO = mau_io_lookup(p, m);
          if (mauIO != NULL) {
            delete mauIO;
            mau_io_set(p, m, NULL);
          }
        }
      }
    }
    for (int p = 0; p < num_pipes(); p++) {
      if (is_pipe_valid(p)) {
        for (int m = 0; m < stagesPerPipe; m++) {
          Mau *mau = mau_lookup(p, m);
          if (mau != NULL) {
            mau->mau_free_all();
            delete mau;
            mau_set(p, m, NULL);
          }
        }
      }
    }
    for (int p = 0; p < num_pipes(); p++) {
      if (is_pipe_valid(p)) {
        for (int pa = 0; pa < kParsersPerPipe; pa++) {
          ParserBlock *prs = parser_lookup(p, pa);
          if (prs != NULL) delete prs;
          parser_set(p, pa, NULL);
        }
      }
    }
    for (int p = 0; p < num_pipes(); p++) {
      if (is_pipe_valid(p)) {
        DeparserBlock *dprs = deparser_lookup(p);
        if (dprs != NULL) delete dprs;
        deparser_set(p, NULL);
      }
    }
    for (int p = 0; p < num_pipes(); p++) {
      if (is_pipe_valid(p)) {
        for (int ib = 0; ib < kIpbsPerPipe; ib++) {
          Ipb *ipb = ipb_lookup(p, ib);
          if (ipb != NULL) delete ipb;
          ipb_set(p, ib, NULL);
        }
      }
    }
    for (int p = 0; p < num_pipes(); p++) {
      if (is_pipe_valid(p)) {
        for (int epb_index = 0; epb_index < kEpbsPerPipe; epb_index++) {
          Epb *epb = epb_lookup(p, epb_index);
          if (epb != NULL) delete epb;
          epb_set(p, epb_index, NULL);
        }
      }
    }
    for (int p = 0; p < num_pipes(); p++) {
      if (is_pipe_valid(p)) {
        Mirror *mirror = mirror_lookup(p);
        if (mirror != NULL) delete mirror;
        mirror_set(p, NULL);
      }
    }
    for (int p = 0; p < num_pipes(); p++) {
      if (is_pipe_valid(p)) {
        LearningFilter *lf = learning_filter_lookup(p);
        if (lf != NULL) delete lf;
        learning_filter_set(p, NULL);
      }
    }

    for (int p = 0; p < num_pipes(); p++) {
      if (is_pipe_valid(p)) {
        Pipe *pipe = pipe_lookup(p);
        if (pipe != NULL) delete pipe;
        pipe_set(p, NULL);
      }
    }

    queueing_delete();

    rdm_delete();
    pre_reg_com_delete();
    for (int i=0; i < kPresTotal; ++i) {
      pre_delete(i);
    }

    // And finally phv_factory, mac_factory and op_handler
    if (phv_factory_ != NULL) {
      delete phv_factory_;
      phv_factory_ = NULL;
    }
    if (mac_factory_ != NULL) {
      delete mac_factory_;
      mac_factory_ = NULL;
    }
    if (rmt_op_handler_ != NULL) {
      delete rmt_op_handler_;
      rmt_op_handler_ = NULL;
    }

    packet_coordinator_delete();
    sweeper_delete();

    rdm_delete();

    delete tm_regs_;
    tm_regs_=nullptr;
    delete tm_regs2_;
    tm_regs2_=nullptr;
    delete tbus_regs_;
    tbus_regs_=nullptr;

    for (int p = 0; p < num_pipes(); p++) {
      if (is_pipe_valid(p)) {
        PktGen *pg = pktgen_lookup(p);
        if (pg != NULL) delete pg;
        pktgen_set(p, NULL);
      }
    }
    for (int p = 0; p < num_pipes(); p++) {
      if (is_pipe_valid(p)) {
        EgressBuf *egress_buf = egress_buf_lookup(p);
        if (egress_buf != nullptr) delete egress_buf;
        egress_buf_set(p, nullptr);
        S2p *s2p = s2p_lookup(p);
        if (s2p != nullptr) delete s2p;
        s2p_set(p, nullptr);
        P2s *p2s = p2s_lookup(p);
        if (p2s != nullptr) delete p2s;
        p2s_set(p, nullptr);
        ParserArbiter *parser_arbiter = parser_arbiter_lookup(p);
        if (parser_arbiter != nullptr) delete parser_arbiter;
        parser_arbiter_set(p, nullptr);
      }
    }
    for (int p = 0; p < num_pipes(); p++) {
      sch_model_wrap_delete(p);
    }

    tm_factory_->free_all();
  }


int RmtObjectManager::string_map_lookup(const char *key, const char *value) {
  return string_map_.lookup(key,value);
}

std::vector<std::string> RmtObjectManager::string_map_get_all_keys() {
  return string_map_.get_all_keys();
}

void RmtObjectManager::log_message(const model_core::Severity severity, const std::string& message) {
  GLOBAL_MODEL->log_message(time_get(), severity, message);
}

void RmtObjectManager::log_message(const uint64_t pkt_id, const int pipe_index, const Gress gress, const int rmt_type, const model_core::Severity severity, const std::string& message) {
  GLOBAL_MODEL->log_event(EventMessage<W>(time_get(), pkt_id, chip_index_, pipe_index, gress, rmt_type, severity, message));
}

void RmtObjectManager::log_info(const std::string& message) {
  log_message(model_core::Severity::kInfo, message);
}

void RmtObjectManager::log_warning(const std::string& message) {
  log_message(model_core::Severity::kWarn, message);
}

void RmtObjectManager::log_error(const std::string& message) {
  log_message(model_core::Severity::kError, message);
}

void RmtObjectManager::log_phv(const int rmt_type, const Phv& phv) {
  GLOBAL_MODEL->log_event(EventPhv<W>(time_get(), chip_index_, rmt_type, phv));
}

void RmtObjectManager::log_mau_phv(const int stage, const Phv& phv) {
  GLOBAL_MODEL->log_event(EventMauPhv<W>(time_get(), chip_index_, stage, phv));
}

void RmtObjectManager::log_packet(const int rmt_type, const Packet& packet) {
  GLOBAL_MODEL->log_event(EventPacket<W>(time_get(), chip_index_, rmt_type, packet));
}

void RmtObjectManager::log_parser_state(const uint64_t pkt_id, const int pipe_index, const Gress gress, const int state) {
  GLOBAL_MODEL->log_event(EventParserState<W>(time_get(), pkt_id, chip_index_, pipe_index, gress, state));
}

void RmtObjectManager::log_parser_extract(const uint64_t pkt_id, const int pipe, const Gress gress, const int phv_word, const int data, const bool tag_along) {
  GLOBAL_MODEL->log_event(EventParserExtract<W>(time_get(), pkt_id, chip_index_, pipe, gress, phv_word, data, tag_along));
}

void RmtObjectManager::log_parser_tcam_match(const uint64_t pkt_id, const int pipe, const Gress gress, const int index, const std::string& lookup) {
  GLOBAL_MODEL->log_event(EventParserTcamMatch<W>(time_get(), pkt_id, chip_index_, pipe, gress, index, lookup));

}

void RmtObjectManager::log_deparser_metadata(const uint64_t pkt_id, const int pipe, const I2QueueingMetadata& metadata) {
  GLOBAL_MODEL->log_event(
      EventDeparserMetadataToTm<W>(time_get(), pkt_id, chip_index_, pipe, metadata));
}

void RmtObjectManager::log_deparser_metadata(const uint64_t pkt_id, const int pipe, const E2MacMetadata& metadata) {
  GLOBAL_MODEL->log_event(
      EventDeparserMetadataToMac<W>(time_get(), pkt_id, chip_index_, pipe, metadata));
}

void RmtObjectManager::log_deparser_pkt_drop(const uint64_t pkt_id, const int pipe, const Gress gress) {
  GLOBAL_MODEL->log_event(
      EventMessage<W>(time_get(), pkt_id, chip_index_, pipe, gress, RmtTypes::kRmtTypeDeparser, model_core::Severity::kWarn, "Packet dropped"));
}

void RmtObjectManager::log_mau_gateway(const uint64_t pkt_id, const int pipe, const Gress gress, const int stage, const int table, const bool enabled, const bool match, const int next_table_stage, const int next_table_table) {
  GLOBAL_MODEL->log_event(
      EventMauGateway<W>(time_get(), pkt_id, chip_index_, pipe, gress, stage,  table, enabled, match, next_table_stage, next_table_table));
}

void RmtObjectManager::log_mau_gateway(const uint64_t pkt_id, const int pipe, const Gress gress, const int stage, const int table, const uint32_t action_instr_addr, const int next_table_stage, const int next_table_table) {
  GLOBAL_MODEL->log_event(
      EventMauGateway<W>(time_get(), pkt_id, chip_index_, pipe, gress, stage, table, action_instr_addr, next_table_stage, next_table_table));
}

void RmtObjectManager::log_mau_table_hit(const uint64_t pkt_id, const int pipe, const Gress gress, const int stage, const int table, const int next_table_stage, const int next_table_table, const uint32_t action_instr_addr, const uint32_t stats_addr, const bool stats_addr_consumed) {
  GLOBAL_MODEL->log_event(
      EventMauTableHit<W>(time_get(), pkt_id, chip_index_, pipe, gress, stage, table, next_table_stage, next_table_table, action_instr_addr, stats_addr, stats_addr_consumed));
}

void RmtObjectManager::log_mau_stateful_alu(const uint64_t pkt_id, const int pipe, const Gress gress, const int stage, const int table, const int meter_alu, const int stateful_instr) {
  GLOBAL_MODEL->log_event(
      EventMauStatefulAlu<W>(time_get(), pkt_id, chip_index_, pipe, gress, stage, table, meter_alu, stateful_instr));
}

void RmtObjectManager::flush_event_log() {
  GLOBAL_MODEL->flush_event_log();
}

int RmtObjectManager::set_phv_modification(int pipe, int stage, model_core::RmtPhvModification::ModifyEnum which, model_core::RmtPhvModification::ActionEnum action, int index, uint32_t value) {
  Mau *mau = mau_lookup(pipe, stage);
  return (mau != nullptr) ? mau->set_phv_modification(which, action, index, value) : -3;
}

} // namespace MODEL_CHIP_NAMESPACE
