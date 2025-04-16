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

#ifndef _MODEL_CORE_MODEL__
#define _MODEL_CORE_MODEL__

#include <cstdint>
#include <cinttypes>
#include <utility>
#include <ostream>
#include <vector>
#include <list>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <cstring>
#include <memory>
#include <unordered_set>

#include <common/rmt.h>
#include <common/rmt-assert.h>
#include <common/rmt-util.h>
#include <model_core/model-interface.h>
#include <model_core/rmt-dru-callback.h>
#include <model_core/capture.h>
#include <model_core/chip_interface.h>
#include <model_core/event-log.h>
#include <model_core/event-stream.h>
#include <model_core/event-writer.h>
#include <model_core/spinlock.h>
#include <model_core/file-logger.h>
#include <model_core/rmt-phv-modification.h>


namespace tofino {
  class RmtObjectManager;
}
namespace tofinoB0 {
  class RmtObjectManager;
}
namespace jbay {
  class RmtObjectManager;
}
namespace jbayB0 {
  class RmtObjectManager;
}
namespace rsvd0 {
  class RmtObjectManager;
}
namespace rsvd1 {
  class RmtObjectManager;
}


namespace model_core {

using EventLog_t = EventLog<EventStream, EventWriter>;

  struct ChipType {
    static constexpr uint8_t  kNone     = 0;
    static constexpr uint8_t  kDefault  = 0;
    static constexpr uint8_t  kTofino   = 1;
    static constexpr uint8_t  kTofinoA0 = 1;
    static constexpr uint8_t  kTofinoB0 = 2;
    // XXX: trestles support removed
    // static constexpr uint8_t  kTrestles = 3;
    static constexpr uint8_t  kJbay     = 4;
    static constexpr uint8_t  kJbayA0   = 4;
    static constexpr uint8_t  kJbayB0   = 5;
    static constexpr uint8_t  kRsvd0       = 6;
    static constexpr uint8_t  kRsvd1     = 6;
    static constexpr uint8_t  kRsvd2     = 7;
    static constexpr uint8_t  kRsvd3 = 8;
  };


  // Callback OP object that stores args for asynchronous callback
  class CallbackOp {
 public:
    static constexpr int kCallbackOpNop = 0;
    static constexpr int kCallbackOpDruDiagEvent = 1;
    static constexpr int kCallbackOpDruIdleUpdate = 2;
    static constexpr int kCallbackOpDruLrtUpdate = 3;
    static constexpr int kCallbackOpDruRxPkt = 4;
    static constexpr int kCallbackOpDruLearn = 5;

    CallbackOp(int op, int asic, uint32_t epoch, uint8_t *data, int len, int arg=0) {
      op_ = op;
      asic_ = asic;
      epoch_ = epoch;
      len_ = len;
      arg_ = arg;
      data_ = nullptr;
      if ((data != nullptr) && (len > 0)) {
        void* data_mem = malloc((size_t)len);
        RMT_ASSERT_NOT_NULL(data_mem);
        data_ = static_cast<uint8_t*>(data_mem);
        model_common::memcpy_uint8(data_, len, data, len);
      }
    }
    ~CallbackOp()  { op_ = kCallbackOpNop; data_ = nullptr;}
    int      op()    const { return op_; }
    int      asic()  const { return asic_; }
    uint32_t epoch() const { return epoch_; }
    uint8_t *data()  const { return data_; }
    int      len()   const { return len_; }
    int      arg()   const { return arg_; }
    // data_ normally extracted with swapdata() and freed by caller
    uint8_t *swapdata(uint8_t *new_data=nullptr) {
      uint8_t *ret_data = data_;
      data_ = new_data;
      return ret_data;
    }

 private:
    int       op_;
    int       asic_;
    uint32_t  epoch_;
    uint8_t  *data_;
    int       len_;
    int       arg_;
  }; // CallbackOp



class Model : public ModelInterface {

public:
  static bool kAllowCb50;   // Defined in model.cpp
  // static bool kAllowFtr100; // FTR now always 100T

  Model(int chips, const char* event_log);
  ~Model();

  bool IsChipIndexValid(int chip) { return ((chip >= 0) && (chip < chips_)); }
  bool IsChipValid(int chip)      { return ((IsChipIndexValid(chip)) && (chip_p_[chip] != NULL)); }

  void StartCapture();
  void StopCapture();

  uint32_t InWord(int chip, uint32_t address) {
    if (en_0bad_mode_ == 0) RMT_ASSERT(IsChipValid(chip));
    return (IsChipValid(chip)) ?chip_p_[chip]->InWord(address) :BAD_DATA_WORD;
  }
  void OutWord(int chip, uint32_t address, uint32_t data) {
    if (IsChipValid(chip)) {
      if (capture_ != nullptr) capture_->log_outword(address, data);
      chip_p_[chip]->OutWord(address, data);
    }
  }
  int DecodeAddress(int chip, uint32_t address, int *index) {
    return (IsChipValid(chip)) ?chip_p_[chip]->DecodeAddress(address, index) :0;
  }

  // for convenience allow to be called with a volatile pointer
  //  (eg from the semifore structures)
  uint32_t InWord(int chip, volatile void* address) {
    return InWord(chip, reinterpret_cast<size_t>(address) );
  }
  void OutWord(int chip, volatile void* address, uint32_t data) {
    OutWord(chip, reinterpret_cast<size_t>(address), data );
  }

  void IndirectRead(int chip, uint64_t address, uint64_t *data0, uint64_t *data1, uint64_t T=UINT64_C(0)) {
    if (en_0bad_mode_ == 0) RMT_ASSERT(IsChipValid(chip));
    if (IsChipValid(chip)) chip_p_[chip]->IndirectRead(address, data0, data1, T);
  }
  void IndirectWrite(int chip, uint64_t address, uint64_t data0, uint64_t data1, uint64_t T=UINT64_C(0)) {
    if (IsChipValid(chip)) {
      if (capture_ != nullptr) capture_->log_indirect_write(address, data0, data1);
      chip_p_[chip]->IndirectWrite(address, data0, data1, T);
    }
  }


  void Log(int chip, int pipe, const char *buffer);
  void ProcessConfigFlags(uint32_t flags);

  void SetLogDir(const char *log_dir) override;
  const char *GetLogDir() const override;

  void SetTrace(unsigned chip, unsigned pipe, bool enable) override;
  bool TraceEnabled(unsigned chip, unsigned pipe) const override;

 private:
  void ContinueOnConfigErrorsImpl(bool enabled, std::ostream* o) override {
    continue_on_config_errors_ = enabled;
    config_msg_ = o;
  }
 public:
  using ModelInterface::ContinueOnConfigErrors; //Unhides ContinueOnConfigErrors(bool,std::ostream*)
  std::pair<bool, std::ostream*> ContinueOnConfigErrors() const override {
    return {continue_on_config_errors_, config_msg_};
  };

  uint8_t GetDefaultType();
  bool CreateChip(int chip, uint8_t type);
  bool DestroyChip(int chip);

  int  GetPackage(int *chip0, int *chip1, int *chip2, int *chip3);
  bool IsPackaged(int chip);
  bool SetPackage(int chip0, int chip1, int chip2, int chip3);
  bool UnPackage(int chip0, int chip1, int chip2, int chip3);
  bool UnPackage(int chip);
  void UnPackageAll();

  bool IsChipInitialized(int chip) {
    return (IsChipValid(chip)) ?chip_p_[chip]->IsChipInitialized() :false;
  }
  ChipInterface *GetChip(int chip) {
    return (IsChipValid(chip)) ?chip_p_[chip].get() :NULL;
  }
  void InitChip(int chip) {
    if (IsChipValid(chip)) chip_p_[chip]->InitChip();
  }
  void ResetChip(int chip) {
    if (IsChipValid(chip)) chip_p_[chip]->ResetChip();
  }
  bool ConfigChip(int chip, uint32_t pipes_enabled, uint8_t num_stages, int sku, uint32_t flags) {
    ProcessConfigFlags(flags);
    if ((flags & RMT_FLAGS_CAPTURE_INPUTS) != 0u) {
      StartCapture();
    }
    return (IsChipValid(chip)) ?chip_p_[chip]->ConfigChip(pipes_enabled,num_stages,sku,flags) :false;
  }
  bool QueryChip(int chip, uint32_t *pipes_enabled, uint8_t *num_stages, int *sku, uint32_t *flags) {
    return (IsChipValid(chip)) ?chip_p_[chip]->QueryChip(pipes_enabled,num_stages,sku,flags) :false;
  }
  uint32_t EpochChip(int chip) {
    return (IsChipValid(chip)) ?chip_p_[chip]->EpochChip() :0;
  }
  uint8_t  GetType(int chip) {
    return (IsChipValid(chip)) ?chip_p_[chip]->GetType() :ChipType::kNone;
  }
  uint32_t GetPipesEnabled(int chip) {
    return (IsChipValid(chip)) ?chip_p_[chip]->GetPipesEnabled() :0;
  }
  uint8_t  GetNumStages(int chip) {
    return (IsChipValid(chip)) ?chip_p_[chip]->GetNumStages() :0;
  }
  uint32_t GetFlags(int chip) {
    return (IsChipValid(chip)) ?chip_p_[chip]->GetFlags() :0;
  }

  bool CreateChip(int chip, uint8_t type,
                  uint32_t pipes_enabled, uint8_t num_stages, int sku, uint32_t flags) {
    bool ok = CreateChip(chip, type);
    if (ok) ConfigChip(chip, pipes_enabled, num_stages, sku, flags);
    return ok;
  }



  void Subscribe(RegisterBlockBase *block, uint32_t chip,
                 uint32_t offset, uint32_t size,
                 bool isMutable, std::string name=nullptr) {
    if (IsChipValid(chip))
      chip_p_[chip]->Subscribe(block,chip,offset,size,isMutable,name);
  }
  void IndirectSubscribe(RegisterBlockIndirectBase *block, uint32_t chip,
                         uint64_t offset, uint32_t size,
                         bool isMutable, std::string name=nullptr) {
    if (IsChipValid(chip))
      chip_p_[chip]->IndirectSubscribe(block,chip,offset,size,isMutable,name);
  }



  // Check that there are no overlapping mutable register.
  //   Must be called after all registers have subscribed
  bool CheckNoOverlaps() {
    bool no_overlaps = true;
    for (int i=0;i<chips_;i++) {
      if ( chip_p_[i] )
        no_overlaps = no_overlaps && chip_p_[i]->CheckNoOverlaps();
    }
    return no_overlaps;
  }

  void set_0bad_mode(int en);


  void PortDown(int chip, uint16_t asic_port) {
    if (IsChipValid(chip))
      chip_p_[chip]->PortDown(asic_port);
  }
  void PortUp(int chip, uint16_t asic_port) {
    if (IsChipValid(chip))
      chip_p_[chip]->PortUp(asic_port);
  }
  void PacketReceivePostBuf(int chip, uint16_t asic_port, uint8_t *buf, int len) {
    if (IsChipValid(chip)) {
      if (capture_ != nullptr) capture_->log_packet(asic_port, buf, len);
      chip_p_[chip]->PacketReceivePostBuf(asic_port, buf, len);
    }
  }
  void StartPacketProcessing(int chip) {
    if (IsChipValid(chip))
      chip_p_[chip]->StartPacketProcessing();
  }
  void StopPacketProcessing(int chip) {
    if (IsChipValid(chip))
      chip_p_[chip]->StopPacketProcessing();
  }
  void PacketTransmitRegisterFunc(int chip, RmtPacketCoordinatorTxFn tx_fn) {
    if (IsChipValid(chip))
      chip_p_[chip]->PacketTransmitRegisterFunc(tx_fn);
  }
  void LoggerRegisterFunc(int chip, rmt_logging_f log_fn) {
    if (IsChipValid(chip))
      chip_p_[chip]->LoggerRegisterFunc(log_fn);
  }
  void SetLogPktSignature(int chip, int offset, int len, bool use_pkt_sig) {
    if (IsChipValid(chip))
      chip_p_[chip]->SetLogPktSignature(offset, len, use_pkt_sig);
  }
  void SetLogPipeStage(int chip, uint64_t pipes, uint64_t stages) {
    if (IsChipValid(chip))
      chip_p_[chip]->SetLogPipeStage(pipes, stages);
  }
  void UpdateLogTypeLevels(int chip, uint64_t pipes, uint64_t stages,
                           int log_type, uint64_t remove_levels, uint64_t add_levels) {
    if (IsChipValid(chip))
      chip_p_[chip]->UpdateLogTypeLevels(pipes, stages, log_type, remove_levels, add_levels);
  }
  void UpdateLogFlags(int chip, uint64_t pipes, uint64_t stages,
                      uint64_t types, uint64_t rows_tabs, uint64_t cols,
                      uint64_t or_log_flags, uint64_t and_log_flags) {
    if (IsChipValid(chip))
      chip_p_[chip]->UpdateLogFlags(pipes, stages, types, rows_tabs,cols,
                                    or_log_flags, and_log_flags);
  }
  void SetP4NameLookup(int chip, int pipe, const char *p4_name_lookup) {
    if (IsChipValid(chip))
      chip_p_[chip]->SetP4NameLookup(pipe, p4_name_lookup);
  }



  void SetTcamWritereg(int chip, int pipe, int stage, int mem, uint32_t address,
                       uint64_t data_0, uint64_t data_1, bool write_tcam) {
    if (IsChipInitialized(chip))
      chip_p_[chip]->SetTcamWriteReg(pipe, stage, mem, address, data_0, data_1, write_tcam);
  }
  void TcamCopyWord(int chip, int pipe, int stage,
                    int src_table_id, int dst_table_id, int num_tables,
                    int num_words, int adr_incr_dir,
                    uint32_t src_address, uint32_t dst_address) {
    if (IsChipInitialized(chip))
      chip_p_[chip]->TcamCopyWord(pipe, stage, src_table_id, dst_table_id,
                                  num_tables, num_words, adr_incr_dir,
                                  src_address, dst_address);
  }

  void TimeReset(int chip) {
    // Only allow when chip in Reset
    if (IsChipValid(chip) && !IsChipInitialized(chip))
      chip_p_[chip]->TimeReset();
  }
  void TimeIncrement(int chip, uint64_t pico_increment) {
    if (IsChipInitialized(chip))
      chip_p_[chip]->TimeIncrement(pico_increment);
  }
  void TimeSet(int chip, uint64_t pico_time) {
    if (IsChipInitialized(chip))
      chip_p_[chip]->TimeSet(pico_time);
  }
  void TimeGet(int chip, uint64_t& time) {
    if (IsChipInitialized(chip))
      chip_p_[chip]->TimeGet(time);
  }

  void GetMauInfo(int chip, int pipe, int stage,
                  uint32_t *array, int array_size,
                  const char **name_array, bool reset) {
    if (IsChipInitialized(chip))
      chip_p_[chip]->GetMauInfo(pipe, stage, array, array_size, name_array, reset);
  }

  const char *NameChip(int chip) {
    return (IsChipValid(chip)) ?chip_p_[chip]->NameChip() :"";
  }
  const char *RegisterVersionChip(int chip) {
    return (IsChipValid(chip)) ?chip_p_[chip]->RegisterVersionChip() :"";
  }



  // PER-CHIP RmtObjectManager accessors
  tofinoB0::RmtObjectManager *GetTofinoB0ObjectManager(int chip);
  void GetObjectManager(int chip, tofinoB0::RmtObjectManager **objmgr);

  tofino::RmtObjectManager *GetTofinoObjectManager(int chip);
  void GetObjectManager(int chip, tofino::RmtObjectManager **objmgr);

  jbay::RmtObjectManager *GetJbayObjectManager(int chip);
  void GetObjectManager(int chip, jbay::RmtObjectManager **objmgr);

  jbayB0::RmtObjectManager *GetJbayB0ObjectManager(int chip);
  void GetObjectManager(int chip, jbayB0::RmtObjectManager **objmgr);

  rsvd0::RmtObjectManager *GetCbObjectManager(int chip);
  void GetObjectManager(int chip, rsvd0::RmtObjectManager **objmgr);

  rsvd1::RmtObjectManager *GetFtrObjectManager(int chip);
  void GetObjectManager(int chip, rsvd1::RmtObjectManager **objmgr);


  // TODO: Should explictly create each chip as and when it's needed
  // TODO: Leave this in for now so things continue to work
  void Reset();
  void DestroyAllChips();

  // TODO: Temp accessor just for ref_model_wrapper
  // TODO: Get DV to get hold of RmtObjectManager properly via GetChip
  tofino::RmtObjectManager *GetObjectManager(int chip);

  void SetPrintWrites(int chip,bool v) {
    if (IsChipValid(chip))
      chip_p_[chip]->SetPrintWrites(v);
  }

  // Register functions for Low Level Driver callbacks
  void register_callback_dru_diag_event( DruDiagEvent  cb ) {
    lld_callbacks_.dru_diag_event_ = cb;
  }
  void register_callback_dru_idle_update( DruIdleUpdate cb ) {
    lld_callbacks_.dru_idle_update_ = cb;
  }
  void register_callback_dru_lrt_update ( DruLrtUpdate  cb ) {
    lld_callbacks_.dru_lrt_update_ = cb;
  }
  void register_callback_dru_rx_pkt     ( DruRxPkt      cb ) {
    lld_callbacks_.dru_rx_pkt_ = cb;
  }
  void register_callback_dru_learn      ( DruLearn      cb ) {
    lld_callbacks_.dru_learn_ = cb;
  }

  // Funcs to callback the registered Low Level Driver functions
  void dru_diag_event_callback( int asic, uint8_t *diag_data, int len );
  void dru_idle_update_callback( int asic, uint8_t *idle_timeout_data, int len );
  void dru_lrt_update_callback( int asic, uint8_t *lrt_stat_data, int len );
  void dru_rx_pkt_callback( int asic, uint8_t *pkt, int len, int cos );
  void dru_learn_callback( int asic, uint8_t *learn_filter_data, int len, int pipe_nbr );

  // Event logging
  void log_event(const Event<EventWriter>& event);
  void log_message(const uint64_t t, const Severity severity, const std::string& message);
  void flush_event_log();

  int SetPhvModification(int chip, int pipe, int stage,
                    RmtPhvModification::ModifyEnum which,
                    RmtPhvModification::ActionEnum action,
                                int index, uint32_t value);

private:
  int                                            chips_;
  std::vector< std::unique_ptr<ChipInterface> >  chip_p_;
  int                                            en_0bad_mode_;

  model_core::ModelLogger                        model_logger_;
  model_core::Spinlock                           spinlock_;
  bool                                           cb_in_progress_;
  std::list<CallbackOp>                          cb_q_;
  std::unique_ptr<EventLog_t>                    events_;

  std::unique_ptr<Capture>                       capture_;

  struct pair_hash {
    std::size_t operator () (std::pair<unsigned, unsigned> const &pair) const {
      return std::hash<unsigned>()(pair.first) ^ std::hash<unsigned>()(pair.second);
    }
  };
  std::unordered_set<std::pair<unsigned, unsigned>, pair_hash> trace_{};
  bool continue_on_config_errors_{};
  std::ostream* config_msg_ = nullptr;

private:

  // Funcs that actually perform callback of Low Level Driver func
  void do_dru_diag_event_callback(int asic, uint8_t *diag_data, int len);
  void do_dru_idle_update_callback(int asic, uint8_t *idle_timeout_data, int len);
  void do_dru_lrt_update_callback(int asic, uint8_t *lrt_stat_data, int len);
  void do_dru_rx_pkt_callback(int asic, uint8_t *pkt, int len, int cos);
  void do_dru_learn_callback(int asic, uint8_t *learn_filter_data, int len, int pipe_nb);

  void do_callback(int which_cb, int asic, uint8_t *data, int len, int arg=0);
  void q_or_callback(int which_cb, int asic, uint8_t *data, int len, int arg=0);

  // For Low Level Driver callbacks
  struct LldCallbacks {
    DruDiagEvent  dru_diag_event_ = 0;
    DruIdleUpdate dru_idle_update_ = 0;
    DruLrtUpdate  dru_lrt_update_ = 0;
    DruRxPkt      dru_rx_pkt_ = 0;
    DruLearn      dru_learn_ = 0;
    void reset() {
      dru_diag_event_ = 0;
      dru_idle_update_ = 0;
      dru_lrt_update_ = 0;
      dru_rx_pkt_ = 0;
      dru_learn_ = 0;
    }
  } lld_callbacks_;

};

}

#endif
