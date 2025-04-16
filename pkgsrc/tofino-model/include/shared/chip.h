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

#ifndef _SHARED_CHIP__
#define _SHARED_CHIP__

#include <array>
#include <cstdint>
#include <cinttypes>
#include <vector>
#include <list>
#include <mutex>
#include <condition_variable>
#include <common/rmt.h>
#include <common/saved-state.h>
#include <common/rmt-assert.h>
#include <model_core/caching_interval_map.h>
#include <model_core/chip_interface.h>
#include <model_core/register_block.h>
#include <model_core/spinlock.h>
#include <model_core/rmt-phv-modification.h>

#include <boost/icl/interval_map.hpp>



namespace MODEL_CHIP_NAMESPACE {

namespace register_classes {
  class MiscRegsSoftResetMutable;
  class MiscRegsDbgRstMutable;
}
class IndirectAccessBlock;
class RmtObjectManager;
class P4NameLookup;
class RmtLogger;

// NB this is located here to ensure it is included for each chip namespace
// when building the model - see the trickery in model.cpp for undef'ing and
// re-including chip.h
/**
 * Placeholder for die id lookup table.
 *
 * Assumes default layout of dies with id's:
 *   0  1
 *   3  2
 * in which read die is east-west peer and write die is north-south peer.
 */
class DieIdTable {
 public:
  DieIdTable() :
    die_id_table_({{
    // rd dg wr
      {{1, 2, 3}},
      {{0, 3, 2}},
      {{3, 0, 1}},
      {{2, 1, 0}}
    }}) { };

  ~DieIdTable() {};

  const uint32_t GetReadDieId(uint32_t my_die_id) const {
    return die_id_table_[my_die_id][0];
  }
  const uint32_t GetWriteDieId(uint32_t my_die_id) const {
    return die_id_table_[my_die_id][2];
  }
  const uint32_t GetDiagonalDieId(uint32_t my_die_id) const {
    return die_id_table_[my_die_id][1];
  }

 private:
  std::array<std::array<uint32_t, 3>, 4> die_id_table_;
};

// Class to maintain per-chip config state
class ChipConfig {

 public:
    ChipConfig();
    virtual ~ChipConfig();

    uint8_t  type()           const { return type_; }
    uint32_t pipes_enabled()  const { return pipes_enabled_; }
    uint8_t  num_stages()     const { return num_stages_; }
    int      sku()            const { return sku_; }
    uint32_t flags()          const { return flags_; }

    void set_type(uint8_t type)     { type_ = type; }
    void set_config(uint32_t pipes_enabled, uint8_t num_stages,
                    int sku, uint32_t flags) {
      pipes_enabled_ = pipes_enabled;
      num_stages_ = num_stages;
      sku_ = sku;
      flags_ = flags;
    }
    bool validate_config(uint32_t pipes_enabled, uint8_t num_stages,
                         int sku, uint32_t flags);
 private:
    uint8_t  type_;
    uint32_t pipes_enabled_;
    uint8_t  num_stages_;
    int      sku_;
    uint32_t flags_;
};


class Chip;

class LogStateInterface {
  protected:
    uint64_t pipes_;
    uint64_t stages_;
    LogStateInterface(uint64_t pipes, uint64_t stages) : pipes_(pipes), stages_(stages) {};
  public:
    virtual void replay(Chip *chip) = 0;
};

class UpdateLogTypeLevelsState : public LogStateInterface {
  private:
    int log_type_;
    uint64_t remove_levels_;
    uint64_t add_levels_;
  public:
    UpdateLogTypeLevelsState(uint64_t pipes, uint64_t stages, int log_type, uint64_t remove_levels, uint64_t add_levels) : 
    LogStateInterface(pipes, stages), log_type_(log_type), remove_levels_(remove_levels), add_levels_(add_levels) {};

    void replay(Chip *chip); 
};

class UpdateLogFlagsState : public LogStateInterface {
  private:
    uint64_t types_;
    uint64_t rows_tabs_;
    uint64_t cols_;
    uint64_t or_log_flags_;
    uint64_t and_log_flags_;
  public:
    UpdateLogFlagsState(uint64_t pipes, uint64_t stages, uint64_t types, uint64_t rows_tabs, uint64_t cols, uint64_t or_log_flags, uint64_t and_log_flags) :
    LogStateInterface(pipes, stages), types_(types), rows_tabs_(rows_tabs), cols_(cols), or_log_flags_(or_log_flags), and_log_flags_(and_log_flags) {};

    void replay(Chip *chip);
};
class SavedStatesLog : public model_common::SavedStateManagerInterface<LogStateInterface *> {
  public:
    SavedStatesLog(Chip *chip) : chip_(chip) {};
    virtual void replay();
  private:
    Chip *chip_;
};

class Chip : public model_core::ChipInterface {

public:
  static bool kChipUseMutex; // Defined in rmt-config.cpp
  static bool kInitOnVeryFirstAccess;
  static bool kUseGlobalTimeIfZero;

  static constexpr int      kMaxNumChipsInPackage       = 4;
  static constexpr uint32_t kSoftResetMask = (1u<<6);
  static constexpr uint8_t  kChipStateOff               = 0;
  static constexpr uint8_t  kChipStateResetPending      = 1;
  static constexpr uint8_t  kChipStateResetInProgress   = 2;
  static constexpr uint8_t  kChipStateReset             = 3;
  static constexpr uint8_t  kChipStateInitPending       = 4;
  static constexpr uint8_t  kChipStateInitInProgress    = 5;
  static constexpr uint8_t  kChipStateInit              = 6;
  static constexpr uint8_t  kChipStateDestroyPending    = 7;
  static constexpr uint8_t  kChipStateDestroyInProgress = 8;


  Chip(int chip_number);
  ~Chip();

  const uint32_t GetMyDieId() const;
  const uint32_t GetReadDieId() const;
  const uint32_t GetWriteDieId() const;
  const uint32_t GetDiagonalDieId() const;
  uint32_t InferMyDieId();
  void SetMyDieId(uint32_t my_die_id);
  void SetMyDieId();

  template < typename T >
  struct subscriber_compare {
    bool operator() (const std::shared_ptr<T>& lhs, const std::shared_ptr<T>& rhs) const {
      // this gives ascending size order for the set
      if ( lhs->size_ != rhs->size_ ) {
        return lhs->size_ > rhs->size_;
      }
      else {
        // if the sizes are the same compare the addresses
        //  so that elements of the same size don't overwrite
        return lhs < rhs;
      }
    }
  };

  struct Subscriber {
    model_core::RegisterBlockBase* block_;
    uint32_t                       start_offset_;
    uint32_t                       size_;
    bool                           is_mutable_;
    std::string                    name_;
  };
  typedef std::set< std::shared_ptr<Subscriber>, subscriber_compare<Subscriber> > Subscribers;
  typedef model_core::CachingIntervalMap<uint32_t,Subscribers> SubscribersMap;

  struct IndirectSubscriber {
    model_core::RegisterBlockIndirectBase* block_;
    uint64_t                               start_offset_;
    uint64_t                               size_;
    bool                                   is_mutable_;
    std::string                            name_;
  };
  typedef std::set< std::shared_ptr<IndirectSubscriber>, subscriber_compare<IndirectSubscriber> > IndirectSubscribers;
  typedef model_core::CachingIntervalMap<uint64_t,IndirectSubscribers> IndirectSubscribersMap;



  // START FUNCS in ChipInterface
  const Subscribers& GetSubscribers(uint32_t addr);
  uint32_t InWord(uint32_t address);
  void OutWord(uint32_t address, uint32_t data);
  int  DecodeAddress(uint32_t address, int *index);
  void IndirectRead(uint64_t address, uint64_t *data0, uint64_t *data1, uint64_t T);
  void IndirectWrite(uint64_t address, uint64_t data0, uint64_t data1, uint64_t T);

  bool IsChipInitialized();
  void InitChip();
  void SignalChip(uint8_t state);
  void WaitChip(uint8_t state);
  void WaitChip(uint8_t state1, uint8_t state2);
  void ResetChip();
  void DestroyChip();
  void MaybeInitChip();
  bool ConfigChip(uint32_t pipes_enabled, uint8_t num_stages, int sku, uint32_t flags);
  bool QueryChip(uint32_t *pipes_enabled, uint8_t *num_stages, int *sku, uint32_t *flags);
  void SetPackage(ChipInterface *chip0, ChipInterface *chip1, ChipInterface *chip2, ChipInterface *chip3);
  void GetPackage(ChipInterface **chip0, ChipInterface **chip1, ChipInterface **chip2, ChipInterface **chip3);
  uint32_t EpochChip();
  uint8_t  GetType()         { return config_.type(); }
  uint32_t GetPipesEnabled() { return config_.pipes_enabled(); }
  uint8_t  GetNumStages()    { return config_.num_stages(); }
  int      GetSku()          { return config_.sku(); }
  uint32_t GetFlags()        { return config_.flags(); }

  void Subscribe(model_core::RegisterBlockBase *block,
                 uint32_t chip, uint32_t offset, uint32_t size,
                 bool isMutable, std::string name) {
    RMT_ASSERT(chip==chip_n_);
    if (size==0) return;
    //printf("Chip Subscribe chip=%d offset=%x size=%x\n",chip,offset,size);
    subscribers_ += make_pair(boost::icl::interval<uint32_t>::right_open(offset,offset+size),
                              Subscribers{std::shared_ptr<Subscriber>{new Subscriber{block,offset,size,isMutable,name}}});
  }
  void IndirectSubscribe(model_core::RegisterBlockIndirectBase *block,
                         uint32_t chip, uint64_t offset, uint32_t size,
                         bool isMutable, std::string name) {
    RMT_ASSERT(chip==chip_n_);
    if (size==0) return;
    // create the indirect_access_blocks_ if it hasn't been done already
    //printf("Chip IndirectSubscribe chip=%d offset=%lx size=%x\n",chip,offset,size);
    // if ( indirect_access_blocks_.size() == 0 ) IndirectInit();

    indirect_subscribers_ += make_pair(boost::icl::interval<uint64_t>::right_open(offset,offset+size),
                                       IndirectSubscribers{std::shared_ptr<IndirectSubscriber>{new IndirectSubscriber{block,offset,size,isMutable,
                                                 name}}});
  }

  // Check that there are no overlapping mutable register.
  //   Must be called after all registers have subscribed
  bool CheckNoOverlaps() {
    bool no_overlaps = true;
    no_overlaps &= CheckNoOverlapsInternal(subscribers_);
    no_overlaps &= CheckNoOverlapsInternal(indirect_subscribers_);
    return no_overlaps;
  }

  void set_0bad_mode(int en);

  void PortDown(uint16_t asic_port);
  void PortUp(uint16_t asic_port);
  void PacketReceivePostBuf(uint16_t asic_port, uint8_t *buf, int len);
  void StartPacketProcessing(void);
  void StopPacketProcessing(void);
  void PacketTransmitRegisterFunc(RmtPacketCoordinatorTxFn tx_fn);
  void LoggerRegisterFunc(rmt_logging_f log_fn);
  void SetLogPktSignature(int offset, int len, bool use_pkt_sig);
  void SetLogPipeStage(uint64_t pipes, uint64_t stages);
  void UpdateLogTypeLevels(uint64_t pipes, uint64_t stages,
                           int log_type, uint64_t remove_levels, uint64_t add_levels);
  void UpdateLogFlags(uint64_t pipes, uint64_t stages,
                      uint64_t types, uint64_t rows_tabs, uint64_t cols,
                      uint64_t or_log_flags, uint64_t and_log_flags);
  void SetP4NameLookup(int pipe, const char *p4_name_lookup);

  void SetTcamWriteReg(int pipe, int stage, int mem, uint32_t address,
                       uint64_t data_0, uint64_t data_1, bool write_tcam);
  void TcamCopyWord(int pipe, int stage,
                    int src_table_id, int dst_table_id, int num_tables,
                    int num_words, int adr_incr_dir,
                    uint32_t src_address, uint32_t dst_address);

  void TimeReset();
  void TimeIncrement(uint64_t pico_increment);
  void TimeSet(uint64_t pico_time);
  void TimeGet(uint64_t& time);

  void GetMauInfo(int pipe, int stage,
                  uint32_t *array, int array_size,
                  const char **name_array, bool reset);

  const char *NameChip();
  const char *RegisterVersionChip();
  void SetPrintWrites(bool v) { print_writes_=v; }

  int SetPhvModification(int pipe, int stage,
      model_core::RmtPhvModification::ModifyEnum which,
      model_core::RmtPhvModification::ActionEnum action,
                        int index, uint32_t value);

  // END FUNCS in ChipInterface


  Chip *GetMyChip()       const { return chips_in_package_[GetMyDieId()]; }
  Chip *GetReadChip()     const { return chips_in_package_[GetReadDieId()]; }
  Chip *GetWriteChip()    const { return chips_in_package_[GetWriteDieId()]; }
  Chip *GetDiagonalChip() const { return chips_in_package_[GetDiagonalDieId()]; }

  void print_the_indirect_subscribers_on_chip();
  RmtObjectManager *GetObjectManager();
  RmtObjectManager *GetObjectManagerWait();
  RmtObjectManager *LookupObjectManager();
  RmtLogger *default_logger() { return default_logger_.get(); }
  uint64_t T_hi() const;


private:
  static constexpr int                                   kPipes = 64;  // Use big val

  model_core::Spinlock                                   spinlock_;
  uint64_t                                               cycles_hi_;
  uint32_t                                               chip_n_;
  uint32_t                                               my_die_id_;
  DieIdTable                                             die_id_table_;
  uint32_t                                               init_chip_cnt_;
  std::mutex                                             init_chip_mutex_;
  std::condition_variable                                init_chip_cv_;
  std::recursive_mutex                                   mutex_;

  SubscribersMap                                         subscribers_;
  IndirectSubscribersMap                                 indirect_subscribers_;
  const Subscribers                                      empty_subscribers_{};
  std::unique_ptr<IndirectAccessBlock >                  indirect_access_block_;
  std::unique_ptr<RmtObjectManager >                     rmt_object_manager_;
  // save the tx and logger function so that we can set it back
  // after Reset in fast-reconfig
  RmtPacketCoordinatorTxFn                               saved_txfn_;
  rmt_logging_f                                          saved_logfn_;
  std::unique_ptr<P4NameLookup>                          saved_logdb_;
  std::array<std::string, kPipes>                        saved_p4namelookup_filename_;
  uint64_t                                               saved_psecs_;
  SavedStatesLog                                         saved_states_log = SavedStatesLog(this);

  uint8_t                                                state_;
  uint32_t                                               prev_ctrl_soft_reset_;

  std::unique_ptr<RmtLogger>                               default_logger_;
  std::unique_ptr<register_classes::MiscRegsSoftResetMutable>  ctrl_soft_reset_;
  std::unique_ptr<register_classes::MiscRegsDbgRstMutable >    status_soft_reset_;

  ChipConfig                                             config_;
  std::array< Chip*, kMaxNumChipsInPackage >             chips_in_package_;

  uint64_t GetT(uint64_t T_cycles); // Figure out time to use

  // default to not assert/assert on unimplemented register access in DV_MODE/SW_MODE
  int  debug_return_0bad_ = DEFAULT_EN_RETURN_0BAD;
  bool print_writes_ = false; // enable this to print all writes in DV_MODE

  RmtLogger* GetLoggerObject( uint32_t address);
  // given an indirect address get the logger object
  RmtLogger* GetIndirectLoggerObject( uint64_t address);

  // Allow model to be initialised/torndown by register writes
  void create_soft_reset(bool recreate);
  void destroy_soft_reset();
  void status_soft_reset_read_cb();
  void ctrl_soft_reset_write_cb();
  void ctrl_soft_reset_write_deferred_cb();
  void ctrl_soft_reset_write(uint32_t val=0u);

  template< typename T > bool CheckNoOverlapsInternal(const T& sub_map ) {
    bool no_overlaps=true;
    for (auto address_interval = sub_map.begin();
         address_interval != sub_map.end();
         ++address_interval ) {

      if (address_interval->second.size() > 1) {
        int n_found = 0;
        for ( auto sub = address_interval->second.begin();
              sub != address_interval->second.end();
              ++sub ) {
          if ( (*sub)->is_mutable_ ) {
            printf("Overlap! %016" PRIx64 " (%d)\n",
                   static_cast<uint64_t>((*sub)->start_offset_ ),
                   n_found);
            n_found++;
            no_overlaps=false;
          }
        }
        if (n_found) {
          // print the names off all in the interval
          for (auto sub = address_interval->second.begin();
               sub != address_interval->second.end();
               ++sub ) {
            printf("  %s%s\n",(*sub)->name_.c_str(),
                   (*sub)->is_mutable_ ? " (mutable)":"" );
          }
        }

      }
    }
    return no_overlaps;
  }

};

}

#endif
