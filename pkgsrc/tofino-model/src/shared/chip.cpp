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
#include <cinttypes>
#include <common/rmt-assert.h>
#include <model_core/model.h>
#include <model_core/version-string.h>
#include <chip.h>
#include <rmt-types.h>
#include <rmt-debug.h>
#include <rmt-defs.h>
#include <rmt-log.h>
#include <rmt-reg-version.h>
#include <indirect-addressing.h>
#include <indirect_access_block.h>
#include <rmt-object-manager.h>
#include <rmt-packet-coordinator.h>
#include <rmt-op-handler.h>
#include <packet-replication-engine.h>
#include <pktgen.h>
#include <address.h>
#include <port.h>
#include <register_adapters.h>
#include <register_utils.h>
#include <p4-name-lookup.h>

#include <register_includes/reg.h>
#include <register_includes/misc_regs_soft_reset_mutable.h>
#include <register_includes/misc_regs_dbg_rst_mutable.h>
#include <model_core/rmt-phv-modification.h>

#if WITH_DEBUGGER
#include <libP4Debugger.h>
#include <phv.h>
#endif

namespace MODEL_CHIP_NAMESPACE {

ChipConfig::ChipConfig()  {
  uint32_t kPipesEn = (1<<RmtDefs::kPipesMax)-1;
  // Initialise to default config
  set_type(RmtDefs::kChipType);
  set_config(kPipesEn, RmtDefs::kStagesMax, RmtDefs::kSkuDefault, 0u);
}
ChipConfig::~ChipConfig() {
}
bool ChipConfig::validate_config(uint32_t pipes_enabled, uint8_t num_stages,
                                 int sku, uint32_t flags) {
  uint32_t kPipesEn = (1<<RmtDefs::kPipesMax)-1;
  return ((pipes_enabled != 0u) && ((pipes_enabled & kPipesEn) == pipes_enabled) &&
          (num_stages > 0) && (num_stages <= RmtDefs::kStagesMax) &&
          (sku >= RmtDefs::kSkuMin) && (sku <= RmtDefs::kSkuMax));
}

void UpdateLogTypeLevelsState::replay(Chip *chip) {
  printf("replaying chip->UpdateLogTypeLevels(%016" PRIx64 ", %016" PRIx64 ", %X, %016" PRIx64 ", %016" PRIx64 ")\n", 
          pipes_, stages_, log_type_, remove_levels_, add_levels_);

  chip->UpdateLogTypeLevels(pipes_, stages_, log_type_, remove_levels_, add_levels_);
}

void UpdateLogFlagsState::replay(Chip *chip) {
  printf("replaying chip->UpdateLogFlags(%016" PRIx64 ", %016" PRIx64 ", %016" PRIx64 ", %016" PRIx64 ", %016" PRIx64 ", %016" PRIx64 ", %016" PRIx64 ")\n", 
          pipes_, stages_, types_, rows_tabs_, cols_, or_log_flags_, and_log_flags_);

  chip->UpdateLogFlags(pipes_, stages_, types_, rows_tabs_, cols_, or_log_flags_, and_log_flags_);
}

void SavedStatesLog::replay() {
  printf("Replaying %zu Log/Flag Updates.\n", size());
  bool was_saving = is_saving();
  disable_saving();
  for(auto it = states_->begin(); it != states_->end(); ++it) {
    (*it)->replay(chip_);
  }
  if(was_saving) {
    enable_saving();
  }
  printf("Finished Replaying Log/Flag Updates.\n");
}

Chip::Chip(int chip_number) :
    spinlock_(), cycles_hi_(UINT64_C(0)), chip_n_(chip_number),
    my_die_id_(0), die_id_table_(),
    init_chip_cnt_(), init_chip_mutex_(), init_chip_cv_(),
    mutex_(),
    subscribers_(),
    indirect_subscribers_(),
    indirect_access_block_(), rmt_object_manager_(),
    saved_txfn_(), saved_logfn_(), saved_logdb_(),
    saved_p4namelookup_filename_(), saved_psecs_(UINT64_C(0)),
    state_(), prev_ctrl_soft_reset_(),
    default_logger_( new DefaultLogger( nullptr, RmtTypes::kRmtTypeChip )),
    ctrl_soft_reset_(), status_soft_reset_(), config_(),
    chips_in_package_()
{
  static_assert( (kPipes >= RmtDefs::kPipesMax), "chip.h kPipes too small" );
  default_logger_.get()->set_log_flags(FIXED_LOG_FLAGS);
#if WITH_DEBUGGER
  // Register a handle with the debugger for each PHV on each chip, in each
  // pipe, in each stage.
  for (int p=0; p<RmtDefs::kPipesMax; ++p) {
    for (int s=0; s<RmtDefs::kStagesMax; ++s) {
      for (int i=0; i<Phv::kWordsMax; ++i) {
        register_handle_t hdl = RmtDefs::p4dHandle_phv(chip_n_, p, s, i);
        char name[1024] = {};
        sprintf(name, "chip[%d].pipe[%d].stage[%d].phv[%d]", chip_n_, p, s, i);
        p4d_declareRegister(name, hdl, Phv::which_width(i));
      }
    }
  }
#endif
}

Chip::~Chip() {
  DestroyChip();
}

const uint32_t Chip::GetMyDieId() const {
  return my_die_id_;
}
const uint32_t Chip::GetReadDieId() const {
  return die_id_table_.GetReadDieId(my_die_id_);
}
const uint32_t Chip::GetWriteDieId() const {
  return die_id_table_.GetWriteDieId(my_die_id_);
}
const uint32_t Chip::GetDiagonalDieId() const {
  return die_id_table_.GetDiagonalDieId(my_die_id_);
}

uint32_t Chip::InferMyDieId() {
  for (uint32_t i = 0; i < 4; i++) {
    if (chips_in_package_[i] == this) return i;
  }
  return 0u;
}
void Chip::SetMyDieId(uint32_t my_die_id) {
  RMT_ASSERT((my_die_id == 0u) || (RmtObject::is_chip1()));
  RMT_ASSERT(my_die_id < 4u);
  my_die_id_ = my_die_id;
}
void Chip::SetMyDieId() {
  SetMyDieId( InferMyDieId() );
}

void Chip::create_soft_reset(bool recreate) {

  // If ctrl register already exists only recreate if told to
  if ((ctrl_soft_reset_.get() != NULL) && (!recreate)) return;

  // Create status reg with read callback and reset it immediately
  register_classes::MiscRegsDbgRstMutable *s = NULL;
  s = misc_regs_dbg_rst_adapter_new(s, chip_n_, 1,
                                    nullptr, [this](){this->status_soft_reset_read_cb();} );
  status_soft_reset_.reset( s ); // Stash new status reg
  s->reset(); // Then reset it

  // Create ctrl reg with write callback
  register_classes::MiscRegsSoftResetMutable *r =
      new register_classes::MiscRegsSoftResetMutable(
          chip_n_, [this](){this->ctrl_soft_reset_write_cb();} );
  ctrl_soft_reset_.reset( r ); // Stash new ctrl reg

  // Save value that was in old incarnation SoftReset reg
  uint32_t prev_reg_val = prev_ctrl_soft_reset_;

  // We've been called from ResetChip or InitChip - we don't
  // want to trigger a further call to ResetChip/InitChip
  // when we reset() the SoftReset register, so save/restore
  // the value of state_ before/after.
  uint8_t prev_state = state_;
  prev_ctrl_soft_reset_ = 0u; r->reset();
  state_ = prev_state;

  // And restore value into new incarnation SoftReset reg
  prev_ctrl_soft_reset_ = prev_reg_val;
}

void Chip::destroy_soft_reset() {
  status_soft_reset_.reset(NULL);
  ctrl_soft_reset_.reset(NULL);
}

void Chip::status_soft_reset_read_cb() {
  uint32_t val = (state_ == kChipStateInit) ?RmtDefs::kSoftResetStatusValOk :RmtDefs::kSoftResetStatusValErr;
  // Update val into reg prior to read and return
  status_soft_reset_->dbg_rst(val);
}
void Chip::ctrl_soft_reset_write_cb() {
  uint32_t old_val = prev_ctrl_soft_reset_;
  uint32_t new_val = ctrl_soft_reset_->swrst();
  if (old_val == new_val) return;
  //prev_ctrl_soft_reset_ = new_val;
  bool old_reset_asserted = ((old_val & RmtDefs::kSoftResetCtrlMask) == RmtDefs::kSoftResetCtrlVal);
  bool new_reset_asserted = ((new_val & RmtDefs::kSoftResetCtrlMask) == RmtDefs::kSoftResetCtrlVal);
  if (!old_reset_asserted && new_reset_asserted)
    state_ = kChipStateResetPending;
  else if (old_reset_asserted && !new_reset_asserted)
    state_ = kChipStateInitPending;
}
void Chip::ctrl_soft_reset_write_deferred_cb() {
  if      (state_ == kChipStateResetPending) ResetChip();
  else if (state_ == kChipStateInitPending)  InitChip();
}
void Chip::ctrl_soft_reset_write( uint32_t val) {
  if (ctrl_soft_reset_.get() == NULL) return;
  ctrl_soft_reset_->swrst(val);
  // No callback so do it ourself
  ctrl_soft_reset_write_cb();
  ctrl_soft_reset_write_deferred_cb();
}



bool Chip::IsChipInitialized() {
  return (state_ == kChipStateInit);
}
void Chip::InitChip() {
  // Could be called direct from GetObjectManager so lock
  // (but could already hold lock if we came through OutWord)
  mutex_.lock();
  if (state_ != kChipStateInit) {
    state_ = kChipStateInitInProgress;

    prev_ctrl_soft_reset_ &= ~RmtDefs::kSoftResetCtrlVal; // Ensure SoftReset deasserted

    indirect_access_block_.reset( new IndirectAccessBlock(chip_n_) );
    uint32_t pipes_en = config_.pipes_enabled();
    uint8_t num_stages = config_.num_stages();
    default_logger_->set_object_manager(nullptr);
    rmt_object_manager_.reset( new RmtObjectManager(chip_n_,true,pipes_en,num_stages,this) );
    default_logger_->set_object_manager(rmt_object_manager_.get());
    // Create soft_reset reg - but don't recreate if already exists
    create_soft_reset(false);

    // XXX: restore time into RmtObjectManager
    if (saved_psecs_ > UINT64_C(0)) {
      rmt_object_manager_->time_set(saved_psecs_);
      saved_psecs_ = UINT64_C(0);
    }
    if (saved_txfn_ != NULL) {
      rmt_object_manager_->packet_coordinator_get()->set_tx_fn(saved_txfn_);
      saved_txfn_ = NULL;
    }
    if (saved_logfn_ != NULL) {
      rmt_object_manager_->set_log_fn(saved_logfn_);
      saved_logfn_ = NULL;
    }

    // We reload from file these days
    // This allows it to be changed prior to FastReconfig
    //
    //if (saved_logdb_.get()) {
    //  rmt_object_manager_->p4_name_lookup_swap(&saved_logdb_);
    //}
    int p = 0;
    while ((pipes_en >> p) != 0) {
      if (((pipes_en >> p) & 1) != 0) {
        rmt_object_manager_->ConfigureP4NameLookup(p, saved_p4namelookup_filename_[p]);
      }
      p++;
    }

    rmt_object_manager_->pre_start();
    rmt_object_manager_->packet_coordinator_get()->start();

    init_chip_cnt_++;
    SignalChip(kChipStateInit);

    if(saved_states_log.size() > 0) {
      saved_states_log.replay();
    }
  }
  mutex_.unlock();
}
void Chip::SignalChip(uint8_t state) {
  std::unique_lock<std::mutex> lock(init_chip_mutex_);
  state_ = state;
  // Tell anyone waiting chip has gone to state
  init_chip_cv_.notify_all();
}
void Chip::WaitChip(uint8_t state) {
  std::unique_lock<std::mutex> lock(init_chip_mutex_);
  // Wait for chip to enter state
  int cnt = 0;
  while ((state_ != state) && (cnt++ < 999999)) {
    init_chip_cv_.wait(lock);
  }
  RMT_ASSERT((cnt < 999999) && "Too long waiting for init_chip_cv");
}
void Chip::WaitChip(uint8_t state1, uint8_t state2) {
  std::unique_lock<std::mutex> lock(init_chip_mutex_);
  // Wait for chip to enter state1 or state2
  int cnt = 0;
  while ( ( ! ((state_ == state1) || (state_ == state2)) ) && (cnt++ < 999999) ) {
    init_chip_cv_.wait(lock);
  }
  RMT_ASSERT((cnt < 999999) && "Too long waiting for init_chip_cv 2");
}
void Chip::ResetChip() {
  mutex_.lock();
  if (state_ != kChipStateReset) {
    uint8_t start_state = state_;
    state_ = kChipStateResetInProgress;

    prev_ctrl_soft_reset_ |= RmtDefs::kSoftResetCtrlVal; // Ensure SoftReset asserted

    if (rmt_object_manager_ != NULL) {
      saved_txfn_ = rmt_object_manager_->packet_coordinator_get()->get_tx_fn();
      saved_logfn_ = rmt_object_manager_->log_fn();
      saved_psecs_ = rmt_object_manager_->time_get();
      // We reload from file now - which may have been updated
      //rmt_object_manager_->p4_name_lookup_swap(&saved_logdb_);
    }
    // Remove all subscriptions
    subscribers_.clear();
    indirect_subscribers_.clear();

    // Delete rmt_object_manager (and hence all subscribers)
    indirect_access_block_.reset( NULL );
    default_logger_->set_object_manager(nullptr);
    rmt_object_manager_.reset( NULL );
    // Teardown/Recreate soft_reset reg (unless destroying)
    if (start_state != kChipStateDestroyInProgress) create_soft_reset(true);

    SignalChip(kChipStateReset);
  }
  mutex_.unlock();
}
void Chip::DestroyChip() {
  state_ = kChipStateDestroyInProgress;
  ResetChip();
  state_ = kChipStateDestroyInProgress;
  destroy_soft_reset();
  state_ = kChipStateOff;
}
void Chip::MaybeInitChip() {
  if ((kInitOnVeryFirstAccess) &&
      (init_chip_cnt_ == 0) &&
      (state_ == kChipStateReset)) InitChip();
}

bool Chip::ConfigChip(uint32_t pipes_enabled, uint8_t num_stages, int sku, uint32_t flags) {
  bool ok = config_.validate_config(pipes_enabled, num_stages, sku, flags);
  if (ok) config_.set_config(pipes_enabled, num_stages, sku, flags);
  return ok;
}
bool Chip::QueryChip(uint32_t *pipes_enabled, uint8_t *num_stages, int *sku, uint32_t *flags) {
  if (pipes_enabled != NULL) *pipes_enabled = GetPipesEnabled();
  if (num_stages != NULL)    *num_stages = GetNumStages();
  if (sku != NULL)           *sku = GetSku();
  if (flags != NULL)         *flags = GetFlags();
  return true;
}
void Chip::SetPackage(ChipInterface *chip0, ChipInterface *chip1,
                      ChipInterface *chip2, ChipInterface *chip3) {
  static_assert( (kMaxNumChipsInPackage >= 4), "Package too small!");
  chips_in_package_[0] = dynamic_cast<Chip*>(chip0);
  chips_in_package_[1] = dynamic_cast<Chip*>(chip1);
  chips_in_package_[2] = dynamic_cast<Chip*>(chip2);
  chips_in_package_[3] = dynamic_cast<Chip*>(chip3);
  SetMyDieId();
}
void Chip::GetPackage(ChipInterface **chip0, ChipInterface **chip1,
                      ChipInterface **chip2, ChipInterface **chip3) {
  static_assert( (kMaxNumChipsInPackage >= 4), "Package too small!");
  *chip0 = chips_in_package_[0];
  *chip1 = chips_in_package_[1];
  *chip2 = chips_in_package_[2];
  *chip3 = chips_in_package_[3];
}
uint32_t Chip::EpochChip() {
  return IsChipInitialized() ?init_chip_cnt_ :0u;
}


void Chip::set_0bad_mode( int en ) {
    debug_return_0bad_ = en;
}


void Chip::PortDown(uint16_t asic_port) {
  RmtObjectManager *om = GetObjectManagerWait();
  rmt_log_fn_va(chip_n_, "PORT DOWN asic %u port %u\n", chip_n_, asic_port);
  om->pre_reg_com_get()->hw_mask_port_down(asic_port);
  // XXX: WIP: map external port view (36) to internal port view (72)
  uint16_t internal_port = Port::port_map_inbound(asic_port);
  // Port down notification to my Packet Generator in my pipe
  PktGen* pgen = om->pktgen_get(om->port_lookup(internal_port)->pipe_index());
  RMT_ASSERT(pgen != nullptr);
  pgen->handle_port_down(internal_port);

  if (RmtObject::is_chip1()) {
    Chip *r_chip = om->chip()->GetReadChip();
    // If the other chip isn't created yet then return
    if (r_chip == nullptr) return;
    RmtObjectManager *r_om = r_chip->LookupObjectManager();
    if (r_om == nullptr) return;

    r_om->pre_reg_com_get()->hw_mask_port_down(asic_port);
    PktGen* r_pgen = r_om->pktgen_get(r_om->port_lookup(internal_port)->pipe_index());
    if (r_pgen == nullptr) return;
    r_pgen->handle_port_down(internal_port);
  }
}
void Chip::PortUp(uint16_t asic_port) {
  RmtObjectManager *om = GetObjectManagerWait();
  rmt_log_fn_va(chip_n_, "PORT UP asic %u port %u\n", chip_n_, asic_port);
  // XXX: WIP: map external port view (36) to internal port view (72)
  uint16_t internal_port = Port::port_map_inbound(asic_port);
  // Port up notification to my Packet Generator in my pipe
  PktGen* pgen = om->pktgen_get(om->port_lookup(internal_port)->pipe_index());
  RMT_ASSERT(pgen != nullptr);
  pgen->handle_port_up(internal_port);

  if (RmtObject::is_chip1()) {
    Chip *r_chip = om->chip()->GetReadChip();
    // If the other chip isn't created yet then return
    if (r_chip == nullptr) return;
    RmtObjectManager *r_om = r_chip->LookupObjectManager();
    if (r_om == nullptr) return;

    PktGen* r_pgen = r_om->pktgen_get(r_om->port_lookup(internal_port)->pipe_index());
    if (r_pgen == nullptr) return;
    r_pgen->handle_port_up(internal_port);
  }
}
void Chip::PacketReceivePostBuf(uint16_t asic_port, uint8_t *buf, int len) {
  RmtObjectManager *om = GetObjectManagerWait();
  RmtPacketCoordinator *pc = om->packet_coordinator_get();
  // XXX: WIP: map external port view (36) to internal port view (72)
  uint16_t internal_port = Port::port_map_inbound(asic_port);
  pc->enqueue(internal_port, buf, len);
}
void Chip::StartPacketProcessing(void) {
  RmtObjectManager *om = GetObjectManagerWait();
  RmtPacketCoordinator *pc = om->packet_coordinator_get();
  pc->start();
  om->pre_start();
}
void Chip::StopPacketProcessing(void) {
  RmtObjectManager *om = GetObjectManagerWait();
  RmtPacketCoordinator *pc = om->packet_coordinator_get();
  om->pre_stop();
  pc->stop();
}
void Chip::PacketTransmitRegisterFunc(RmtPacketCoordinatorTxFn tx_fn) {
  RmtObjectManager *om = GetObjectManagerWait();
  rmt_log_fn_va(chip_n_, "Registering handler for tx\n");
  RmtPacketCoordinator *pc = om->packet_coordinator_get();
  pc->set_tx_fn(tx_fn);
}
void Chip::LoggerRegisterFunc(rmt_logging_f log_fn) {
  RmtObjectManager *om = GetObjectManagerWait();
  rmt_log_fn_va(chip_n_, "Setting logging fn\n");
  om->set_log_fn(log_fn);
}
void Chip::SetLogPktSignature(int offset, int len, bool use_pkt_sig) {
  RmtObjectManager *om = GetObjectManagerWait();
  om->set_log_pkt_signature(offset, len, use_pkt_sig);
}
void Chip::SetLogPipeStage(uint64_t pipes, uint64_t stages) {
  RmtObjectManager *om = GetObjectManagerWait();
  om->set_log_pipe_stage(pipes, stages);
}
void Chip::UpdateLogTypeLevels(uint64_t pipes, uint64_t stages,
                               int log_type, uint64_t remove_levels, uint64_t add_levels) {
  RmtObjectManager *om = GetObjectManagerWait();
  om->update_log_type_levels(pipes, stages, log_type, remove_levels, add_levels);
  saved_states_log.add_state(new UpdateLogTypeLevelsState(pipes, stages, log_type, remove_levels, add_levels));
}
void Chip::UpdateLogFlags(uint64_t pipes, uint64_t stages,
                          uint64_t types, uint64_t rows_tabs, uint64_t cols,
                          uint64_t or_log_flags, uint64_t and_log_flags) {
  RmtObjectManager *om = GetObjectManagerWait();
  om->update_log_flags(pipes, stages, types, rows_tabs, cols,
                       or_log_flags, and_log_flags);
  saved_states_log.add_state(new UpdateLogFlagsState(pipes, stages, types, rows_tabs,
                       cols, or_log_flags, and_log_flags));
}
void Chip::SetP4NameLookup(int pipe, const char *p4_name_lookup) {
  if (nullptr == p4_name_lookup) {
    RMT_LOG_OBJ(
        default_logger_.get(), RmtDebug::warn(),
        "No P4 name-lookup file for chip %d, pipe %d\n", chip_n_, pipe);
    return;
  }
  RmtObjectManager *om = GetObjectManagerWait();
  saved_p4namelookup_filename_[pipe] = std::string(p4_name_lookup);
  om->ConfigureP4NameLookup(pipe, saved_p4namelookup_filename_[pipe]);
  if (om->p4_name_lookup(pipe).IsLoaded()) {
    std::string ver_str("");
    try {
      ver_str = om->p4_name_lookup(pipe).GetSchemaVersion();
    } catch (const std::invalid_argument&) { }
    if (ver_str.empty()) {
      RMT_LOG_OBJ(
          default_logger_.get(), RmtDebug::warn(),
          "P4 name-lookup file schema version not found");
    } else {
      model_core::VersionString ver(ver_str);
      model_core::VersionString expected_ver("1.6.0");
      if (ver < expected_ver) {
        RMT_LOG_OBJ(
            default_logger_.get(), RmtDebug::warn(),
            "P4 name-lookup file has unexpected schema version '%s' "
            "(expected >= '%s')",
            ver_str.c_str(), expected_ver.str().c_str());
      }
    }
  } else {
    RMT_LOG_OBJ(
      default_logger_.get(), RmtDebug::warn(),
      "P4 name-lookup file not loaded for chip %d, pipe %d\n", chip_n_, pipe);
  }
}

void Chip::SetTcamWriteReg(int pipe, int stage, int mem, uint32_t address,
                     uint64_t data_0, uint64_t data_1, bool write_tcam) {
  RmtObjectManager *om = GetObjectManagerWait();
  RmtOpHandler *op = om->op_handler_get();
  op->set_tcam_writereg(pipe, stage, mem, address,
                        data_0, data_1, write_tcam );
}
void Chip::TcamCopyWord(int pipe, int stage,
                  int src_table_id, int dst_table_id, int num_tables,
                  int num_words, int adr_incr_dir,
                  uint32_t src_address, uint32_t dst_address) {
  RmtObjectManager *om = GetObjectManagerWait();
  RmtOpHandler *op = om->op_handler_get();
  op->tcam_copy_word(pipe, stage, src_table_id, dst_table_id, num_tables,
                     num_words, adr_incr_dir, src_address, dst_address );
}

void Chip::TimeReset() {
  RMT_ASSERT(state_ == kChipStateReset);
  spinlock_.lock();
  cycles_hi_ = saved_psecs_ = UINT64_C(0);
  spinlock_.unlock();
}
void Chip::TimeIncrement(uint64_t pico_increment) {
  RmtObjectManager *om = GetObjectManagerWait();
  om->time_increment(pico_increment); //Picoseconds
}
void Chip::TimeSet(uint64_t pico_time) {
  RmtObjectManager *om = GetObjectManagerWait();
  om->time_set(pico_time); //Picoseconds
}
void Chip::TimeGet(uint64_t& time) {
  RmtObjectManager *om = LookupObjectManager();
  time = (om != nullptr) ?om->time_get() :UINT64_C(0);
}

void Chip::GetMauInfo(int pipe, int stage,
                      uint32_t *array, int array_size,
                      const char **name_array, bool reset) {
  RmtObjectManager *om = GetObjectManagerWait();
  om->get_mau_info(pipe, stage, array, array_size, name_array, reset);
}

#define STRINGIFY(s) #s

const char *Chip::NameChip() {
  return STRINGIFY(MODEL_CHIP_NAMESPACE);
}
const char *Chip::RegisterVersionChip() {
  return RMT_REG_VERSION;
}

#undef STRINGIFY


const Chip::Subscribers& Chip::GetSubscribers(uint32_t address) {
  auto address_interval = subscribers_.find(address);
  if ( address_interval != subscribers_.end() ) {
    return address_interval->second;
  }
  return empty_subscribers_;
}

uint32_t Chip::InWord(uint32_t address)  {

  if (kChipUseMutex) mutex_.lock();
  auto address_interval = subscribers_.find(address);
  // This just returns the first one. Mutable registers (that
  //  the model can write) can only have one instance, and
  //  non-mutable register copies are kept in sync by OutWord
  if ( address_interval != subscribers_.end() ) {
    for ( auto sub = address_interval->second.begin(); sub != address_interval->second.end(); ++sub ) {
      uint32_t data = 0;
      const std::shared_ptr<Subscriber>& b = *sub;
      uint32_t offset = address - b->start_offset_;
      bool read_done = b->block_->read( offset, &data);
      RMT_LOG_OBJ(GetLoggerObject(address),
                RmtDebug::kRmtDebugRead &
                  ((data==0) ? 0 : UINT64_C(0xFFFFFFFFFFFFFFFF)),
                  "InWord 0x%08x from %s (0x%08x) offset 0x%x%s\n%s\n",
                  read_done?data:0,
                  b->name_.empty()?"?":b->name_.c_str(),
                  address,
                  offset,
                  read_done?"":" no read",
                  b->block_->to_string(offset,false).c_str());

      if (read_done) {
        if (kChipUseMutex) mutex_.unlock();
        return data;
      }
    }
  }
  if (kChipUseMutex) mutex_.unlock();
  RMT_LOG_OBJ(default_logger_.get(), RmtDebug::verbose(),
              "Read from unimplemented address 0x%08x\n", address);
  if (debug_return_0bad_) {
    return static_cast<uint32_t>(MauDefs::kBadDataWord & UINT64_C(0xFFFFFFFF));;
  }
  RMT_ASSERT(0); // read from an unimplemented register
}
void Chip::OutWord(uint32_t address, uint32_t data) {
  if (print_writes_) printf("tu.OutWord(0x%08x, 0x%08x); // chip=%d\n", address, data, chip_n_);

  if (kChipUseMutex) mutex_.lock();
  bool found=false;
  auto address_interval = subscribers_.find(address);
  if ( address_interval != subscribers_.end() ) {
    for ( auto sub = address_interval->second.begin(); sub != address_interval->second.end(); ++sub ) {
      const std::shared_ptr<Subscriber>& b = *sub;
      uint32_t offset = address - b->start_offset_;
      // write can return false to prevent further writes (to smaller blocks that are
      //  also registered for this address)
      bool keep_writing = b->block_->write( offset , data );
      if (!found || !keep_writing) { // only print for first or if this stops the writing
        RMT_LOG_OBJ(GetLoggerObject(address),
                    RmtDebug::kRmtDebugWrite &
                    ((data==0) ? 0 : UINT64_C(0xFFFFFFFFFFFFFFFF)),
                    "OutWord 0x%08x to %s (0x%08x) offset 0x%x%s\n%s\n",
                    data,
                    b->name_.empty()?"?":b->name_.c_str(),
                    address,offset,keep_writing?"":" stop writing",
                    b->block_->to_string(offset,false).c_str() );
      }
      found=true;
      if (!keep_writing) {
        break;
      }
    }
  }
  // Maybe need to handle reset now OutWord complete
  if (found) ctrl_soft_reset_write_deferred_cb();

  if (kChipUseMutex) mutex_.unlock();
  if (!found) {
    RMT_LOG_OBJ(default_logger_.get(), RmtDebug::verbose(),
                "Write to unimplemented address 0x%08x\n", address);
    if (debug_return_0bad_) return;
    //RMT_ASSERT(0); // write to an unimplemented register
  }
}

int Chip::DecodeAddress(uint32_t address, int *index) {
  return RegisterUtils::addr_decode(address, index);
}


uint64_t Chip::GetT(uint64_t cycles_T) {
  RmtObjectManager *om = LookupObjectManager();

  spinlock_.lock();

  uint64_t local_hi = cycles_hi_; // Keep local copy in case we print later
  if ((cycles_T == UINT64_C(0)) && (kUseGlobalTimeIfZero) && (om != nullptr))
    cycles_T = om->time_get_cycles();
  bool t_gone_backwards = ((cycles_T > UINT64_C(0)) && (cycles_T < cycles_hi_));
  if (cycles_T > cycles_hi_) cycles_hi_ = cycles_T;

  spinlock_.unlock();

  if (t_gone_backwards)
    RMT_LOG_OBJ(default_logger_.get(), RmtDebug::warn(), "IndirectRead/Write: "
                "Time gone backwards! T_hi=%" PRIu64 " T_this=%" PRIu64 "\n", local_hi, cycles_T);
  return cycles_T;
}

uint64_t Chip::T_hi() const { return cycles_hi_; }


// should only be used by the indirect access block
void Chip::IndirectRead( uint64_t address, uint64_t *data0, uint64_t *data1, uint64_t T )  {
  T = GetT(T);

  // Detect register reads and pass them over to InWord instead.
  if (Address::ind_addr_is_reg(address)) {
    uint32_t reg_addr = Address::ind_to_dir_reg_addr(address);
    // Register addresses must address a word.
    RMT_ASSERT( !(reg_addr & 3) );
    uint32_t data = InWord(reg_addr);
    *data0 = data;
    *data1 = 0;
    return;
  }

  if (kChipUseMutex) mutex_.lock();
  auto address_interval = indirect_subscribers_.find(address);
  // This just returns the first one. Mutable registers (that
  //  the model can write) can only have one instance, and
  //  non-mutable register copies are kept in sync by OutWord
  if ( address_interval != indirect_subscribers_.end() ) {
    auto& b = *(address_interval->second.begin());
    uint64_t offset = address - b->start_offset_;
    b->block_->read( address - b->start_offset_, data0, data1, T );
    RMT_LOG_OBJ(GetIndirectLoggerObject(address),
                RmtDebug::kRmtDebugRead &
                 (((*data0)==0 && (*data1)==0) ? 0 : UINT64_C(0xFFFFFFFFFFFFFFFF)),
                "IndirectRead 0x%016" PRIx64 " %016" PRIx64
                " from %s (0x%016" PRIx64 ") offset 0x%" PRIx64 "\n%s",
                *data1,*data0,
                b->name_.empty()?"?":b->name_.c_str(),
                address,
                offset,
                b->block_->to_string(offset,false).c_str());
    if (kChipUseMutex) mutex_.unlock();
    return;
  }
  if (kChipUseMutex) mutex_.unlock();
  RMT_LOG_OBJ(default_logger_.get(), RmtDebug::verbose(),
              "Read from unimplemented address 0x%016" PRIx64 "\n", address);
  if (debug_return_0bad_) {
    if (data0 != NULL) *data0 = MauDefs::kBadDataWord;
    if (data1 != NULL) *data1 = MauDefs::kBadDataWord;
    return;
  }
  RMT_ASSERT(0); // read from an unimplemented register
}


void Chip::IndirectWrite( uint64_t address, uint64_t data0, uint64_t data1, uint64_t T )  {
  T = GetT(T);
  if (print_writes_) printf("tu.IndirectWrite(0x%016" PRIx64 ", 0x%016" PRIx64 ", 0x%016" PRIx64 ", 0x%" PRIx64 ");\n",address,data0,data1,T);

  // Detect register writes and pass them over to OutWord instead.
  if (Address::ind_addr_is_reg(address)) {
    uint32_t reg_addr = Address::ind_to_dir_reg_addr(address);
    // Register addresses must address a word.
    RMT_ASSERT( !(reg_addr & 3) );
    // Register writes can only be done with 32 bits of data.
    RMT_ASSERT( !(data0 >>32) && !data1);
    uint32_t reg_data = data0 & 0xFFFFFFFF;
    OutWord(reg_addr, reg_data);
    return;
  }

  if (kChipUseMutex) mutex_.lock();
  bool found=false;
  auto address_interval = indirect_subscribers_.find(address);
  if ( address_interval != indirect_subscribers_.end() ) {
    // write to all the subscribers in an address interval
    for ( auto sub = address_interval->second.begin(); sub != address_interval->second.end(); ++sub ) {
      auto& b = *sub;
      uint64_t offset = address - b->start_offset_;
      if (!found) { // only print for first
        RMT_LOG_OBJ(GetIndirectLoggerObject(address),
                    RmtDebug::kRmtDebugWrite &
                    ((data0==0 && data1==0) ? 0 : UINT64_C(0xFFFFFFFFFFFFFFFF)),
                    "IndirectWrite 0x%016" PRIx64 " %016" PRIx64
                    " to %s (0x%016" PRIx64 ") offset 0x%" PRIx64 "\n%s",
                    data1,data0,
                    b->name_.empty()?"?":b->name_.c_str(),
                    address,
                    offset,
                    b->block_->to_string(offset,false).c_str());
      }
      b->block_->write( address - b->start_offset_, data0, data1, T );
      found=true;
    }
  }
  if (kChipUseMutex) mutex_.unlock();
  if (!found) {
    RMT_LOG_OBJ(default_logger_.get(), RmtDebug::verbose(),
                "Write to unimplemented address 0x%016" PRIx64 "\n",
                address);
    if (debug_return_0bad_) return;
    RMT_ASSERT(0); // write to an unimplemented register
  }
}


void Chip::print_the_indirect_subscribers_on_chip() {
  auto a = indirect_subscribers_.begin();
  for ( ; a != indirect_subscribers_.end(); ++a) {
    for (auto b = a->second.begin(); b != a->second.end(); ++b) {
      auto c = *b;
      printf("%p 0x%" PRIx64 ", %d, %s\n", c->block_, c->start_offset_,
             c->is_mutable_, c->name_.c_str());
    }
  }
}

int Chip::SetPhvModification(int pipe, int stage,
      model_core::RmtPhvModification::ModifyEnum which,
      model_core::RmtPhvModification::ActionEnum action,
      int index, uint32_t value) {
  RmtObjectManager *om = GetObjectManagerWait();
  return om->set_phv_modification(pipe, stage, which, action, index, value);
}

RmtObjectManager* Chip::GetObjectManager() {
  RMT_ASSERT (rmt_object_manager_ != NULL);
  return rmt_object_manager_.get();
}
RmtObjectManager* Chip::GetObjectManagerWait() {
  WaitChip(kChipStateInit);
  RMT_ASSERT (rmt_object_manager_ != NULL);
  return rmt_object_manager_.get();
}
RmtObjectManager* Chip::LookupObjectManager() {
  if (!IsChipInitialized()) return NULL;
  return rmt_object_manager_.get();
}


RmtLogger* Chip::GetIndirectLoggerObject(uint64_t address) {
  RmtObjectManager *om = LookupObjectManager();
  if (om == NULL) return default_logger_.get();
  RmtLogger* logger_obj = nullptr;
  if (indirect_is_in_pipe_space(address)) {
    int pipe = indirect_get_pipe( address );
    int stage = indirect_get_stage( address );
    logger_obj = om->mau_lookup(pipe,stage);
  }
  return logger_obj ? logger_obj : default_logger_.get();
}




}
