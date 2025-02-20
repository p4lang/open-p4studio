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

#include <utests/test_util.h>
#include <utests/filesystem_helper.h>
#include <utests/reader_actions.h>
#include <model_core/model.h>
#include <chip.h>
#include <mau.h>
#include <string>
#include <crafter/Crafter.h>
#include <crafter/Utils/CrafterUtils.h>
#include <rmt-log.h>
#include <parser-static-config.h>
#include <parser.h>
#include <deparser.h>
#include <port.h>
#include <rmt-defs.h>
#include <phv-factory.h>
#include <utests/gtest.h>

extern std::unique_ptr<model_core::Model> GLOBAL_MODEL;
extern "C" void rmt_set_stdout(FILE *file_ptr);  // rmt-global.cpp
extern "C" void rmt_set_stderr(FILE *file_ptr);  // rmt-global.cpp

namespace MODEL_CHIP_TEST_NAMESPACE {

using namespace MODEL_CHIP_NAMESPACE;


// RmtStreamCaptureBase
RmtStreamCaptureBase::RmtStreamCaptureBase() :
    tmpfile_(nullptr) { }

RmtStreamCaptureBase::~RmtStreamCaptureBase() { }

FILE* RmtStreamCaptureBase::start() {
  if (nullptr == tmpfile_) tmpfile_ = tmpfile();
  if (nullptr != tmpfile_) set_stream(tmpfile_);
  return tmpfile_;
}

void RmtStreamCaptureBase::stop() {
  if (nullptr == tmpfile_) return;
  set_stream(nullptr);
}

void RmtStreamCaptureBase::clear() {
  if (nullptr == tmpfile_) return;
  stop();
  (void)fclose(tmpfile_);  // tmpfile is automatically deleted
  tmpfile_ = nullptr;
}

void RmtStreamCaptureBase::clear_and_start() {
  clear();
  start();
}

int RmtStreamCaptureBase::for_each_line(
    const std::function<void(int line_num, std::string line)> callback_fn) {
  if (tmpfile_ == nullptr) return -1;
  rewind(tmpfile_);
  size_t line_len = 0;
  char *line = nullptr;
  int line_count = 0;
  while (getline(&line, &line_len, tmpfile_) != -1) {
    if (nullptr != callback_fn) callback_fn(line_count, std::string(line));
    line_count++;
  }
  free(line);
  return line_count;
}

int RmtStreamCaptureBase::for_each_line_containing(
    const std::string match,
    const std::function<void(
        int line_num,
        size_t pos,
        std::string line)> callback_fn,
    const size_t match_pos) {
  if (tmpfile_ == nullptr) return -1;
  int line_count = 0;
  auto line_matcher = [match, match_pos, &line_count, callback_fn](
      int line_num,
      std::string line)->void {
    size_t pos = line.find(match);
    // note: default match_pos=string::npos is treated as 'don't care'
    if ((pos != std::string::npos) &&
        ((match_pos == std::string::npos) || (pos == match_pos))) {
      line_count++;
      if (nullptr != callback_fn) callback_fn(line_num, pos, line);
    }
  };
  (void)for_each_line(line_matcher);
  return line_count;
}
int RmtStreamCaptureBase::for_each_line_starts_with(
    const std::string match,
    const std::function<void(
        int line_num,
        size_t pos,
        std::string line)> callback_fn) {
  return for_each_line_containing(match, callback_fn, 0u);
}

std::string RmtStreamCaptureBase::dump_lines(
    const size_t max_line_length,
    const bool with_line_numbers) {
  std::string output;
  for_each_line([&](int line_num, std::string line)->void{
    std::string prefix;
    if (with_line_numbers) {
      prefix.append("[" + std::to_string(line_num) + "] ");
    }
    std::string trunc_line = line.substr(0u, max_line_length);
    // add ellipsis plus newline if line has been truncated
    if (trunc_line.length() < line.length()) trunc_line.append("...\n");
    output.append(prefix + trunc_line);
  });
  return output;
}

// RmtStreamCapture
RmtStreamCapture::RmtStreamCapture(RmtStreamCaptureEnum stream_num) :
    RmtStreamCaptureBase(),
    stream_num_(stream_num)
    { }

RmtStreamCapture::~RmtStreamCapture() {
  // stop redirecting stream and close tmpfile before destructing self
  clear();
}

void RmtStreamCapture::set_stream(FILE *file_ptr) {
  // make call to appropriate C linkage global func
  switch (stream_num_) {
    case RmtStreamCaptureEnum::stdout :
      rmt_set_stdout(file_ptr);
      break;
    case RmtStreamCaptureEnum::stderr :
      rmt_set_stderr(file_ptr);
      break;
    default :
      break;
  }
}

//RmtLoggerCapture
RmtLoggerCapture::RmtLoggerCapture(RmtLogger *rmt_logger) :
    RmtStreamCaptureBase(),
    rmt_logger_(rmt_logger)
    { }

RmtLoggerCapture::~RmtLoggerCapture() {
  // stop redirecting stream and close tmpfile before destructing self
  clear();
}

void RmtLoggerCapture::set_stream(FILE *file_ptr) {
  if (nullptr == rmt_logger_) return;
  rmt_logger_->set_log_file(file_ptr);
}

// BaseTest
void BaseTest::SetUp() {
  GLOBAL_MODEL->Reset();
  tu_ = new TestUtil(GLOBAL_MODEL.get(), chip_index(), pipe_index(), stage_index());
  om_ = tu_->get_objmgr();
  ASSERT_TRUE(nullptr != om_);
}

void BaseTest::TearDown() {
  if (nullptr != tu_) delete tu_;
  if (nullptr != rmt_stdout_capture_) delete rmt_stdout_capture_;
  if (nullptr != rmt_stderr_capture_) delete rmt_stderr_capture_;
  if (nullptr != rmt_logger_capture_) delete rmt_logger_capture_;
}

RmtStreamCapture* BaseTest::rmt_stdout_capture() {
  if (nullptr == rmt_stdout_capture_) {
    rmt_stdout_capture_ = new RmtStreamCapture(
      RmtStreamCapture::RmtStreamCaptureEnum::stdout);
  }
  return rmt_stdout_capture_;
}

RmtStreamCapture* BaseTest::rmt_stderr_capture() {
  if (nullptr == rmt_stderr_capture_) {
    rmt_stderr_capture_ = new RmtStreamCapture(
      RmtStreamCapture::RmtStreamCaptureEnum::stderr);
  }
  return rmt_stderr_capture_;
}

RmtLoggerCapture* BaseTest::rmt_logger_capture() {
  // note: this method returns an RmtLoggerCapture for the RmtObjectManager
  // which will capture logging from all registered objects; RmtLoggerCapture
  // instances targetted at specific objects may also be separately
  // constructed if desired.
  if (nullptr == rmt_logger_capture_) {
    rmt_logger_capture_ = new RmtLoggerCapture(om_);
  }
  return rmt_logger_capture_;
}

void BaseTest::load_context_json_file(int pipe_index) const {
  std::string path;
  if (RmtObject::is_jbay_or_later()) {
   path = get_resource_file_path("parser-static-config-jbay.json");
  } else {
    path = get_resource_file_path("parser-static-config-tofinoXX.json");
  }
  om_->ConfigureP4NameLookup(pipe_index, path);
}

// FakeRegister
FakeRegister::FakeRegister(TestUtil *tu,
                           uint32_t addr,
                           int width)
    : tu_(tu),
      width_(width),
      addr_(addr) {
  RMT_ASSERT(width_ <= 64);
  uint64_t mask = 0xffffffffffffffffu;
  max_ = mask >> (64 - width_);
}

uint64_t FakeRegister::read() {
  uint64_t val = tu_->InWord(addr_);
  if (width_ > 32) {
    uint64_t val_1 = tu_->InWord(addr_ + 4);
    val = val | (val_1 << 32);
  }
  return val & max_;
}

void FakeRegister::write(uint64_t data) {
  RMT_ASSERT(data <= max_);
  tu_->OutWord(addr_, (uint32_t) (data & 0xffffffffu));
  if (width_ > 32) {
    tu_->OutWord(addr_ + 4, (uint32_t) (data >> 32));
  }
}

void FakeRegister::write_max() {
  write(max_);
}

  // TestUtil

  int TestUtil::dru_learn_callback_count_ = 0;


  TestUtil::TestUtil(model_core::Model *model, int chip, int pipe, int stage)
      : model_(model), chip_(chip), pipe_(pipe), stage_(stage),
        pipes_en_(kAllPipesEn), num_stages_(kMaxStages), sku_(kSku), flags_(0u),
        use_ind_regs_(false), debug_(false),
        evaluate_all_(false), evaluate_all_test_(false),
        free_on_exit_(true),
        dv_test_(999), dv_vintage_(kVintageLatest) {
      assert(model != NULL);
      assert((chip >= 0) && (chip <= 255));
      // Chips >200 are special and have chip-200 MAU stages
      if (chip > 200) num_stages_ = chip - 200;
      Reset();
      model_->register_callback_dru_learn( DummyDruLearn );
      test_reader_.get_action()->set_test_util(this);
      set_dv_mode();
  }
  TestUtil::~TestUtil() { }

  // Initialize etc
  void TestUtil::Reset() {
    assert(model_ != NULL);

    // Reset global error
    GLOBAL_RMT_ERROR = 0;

    // Figure out current config
    uint32_t curr_pipes_en = 0u, curr_flags = 0u;
    uint8_t  curr_num_stages = 0;
    int      curr_sku = -1;
    uint8_t  curr_type = model_->GetType(chip_);
    model_->QueryChip(chip_, &curr_pipes_en, &curr_num_stages, &curr_sku, &curr_flags);

    if ((curr_type == RmtDefs::kChipType) &&
        (curr_pipes_en == pipes_en_) &&
        (curr_num_stages == num_stages_) &&
        (curr_sku == sku_) && (curr_flags == flags_)) {
      // Chip config unchanged - just Reset
      model_->ResetChip(chip_);
      // ... also reset time (to 0) - utests assume this
      model_->TimeReset(chip_);
    } else {
      // Chip config changed - Destroy then Create afresh with new type/config
      //
      // Destroy chip freeing resources (RmtObjectManager, Subscribers etc)
      (void)model_->DestroyChip(chip_);
      // Create chip of appropriate type configured with
      // specified pipes_en, num_stages, flags
      bool create_ok = model_->CreateChip(chip_, RmtDefs::kChipType, pipes_en_, num_stages_, sku_, flags_);
      RMT_ASSERT(create_ok && "TestUtil::Reset: FAILED to CreateChip !!!!!!!!!!");
    }
    // Then initialise it
    model_->InitChip(chip_);

    // Fetch new RmtObjectManager
    RmtObjectManager *objmgr = get_objmgr();
    if (objmgr != NULL) {
      // Setup whether it does a chip_free_all when Model destroys it
      objmgr->set_chip_free_all_on_exit(free_on_exit_);
      // And whether it is in evaluateAll mode
      objmgr->set_evaluate_all(evaluate_all_, evaluate_all_test_);
    }

    // Switch off pfe_mode (ie use kGlobalAddrEnable) by default
    set_pfe_mode(false);

    // Schtummificate
    quieten_log_flags();
    quieten_p4_log_flags();
  }
  // This func now redundant
  void TestUtil::chip_init_all() {
    //if (debug_) printf("TestUtil<chip=%d> ChipInitAll\n",chip_);
    //RmtObjectManager *objmgr = get_objmgr();
    //if (objmgr != NULL) objmgr->chip_init_all();
  }

  void TestUtil::quieten_p4_log_flags(uint64_t pipes) {
    // restrict P4 type logging to FATAL, ERROR, WARN
    RmtObjectManager *objmgr = get_objmgr();
    if (objmgr != NULL) {
      objmgr->update_log_type_levels(pipes, ALL, RMT_LOG_TYPE_P4, ALL, UINT64_C(0xF));
    }
  }

  void TestUtil::set_dv_mode() {
#ifndef DV_MODE
    // Setup all default values that apply in DV_MODE (see rmt-features.h)
    // Allows utests to run OK without -DDV_MODE on build
    GLOBAL_ABORT_ON_ERROR = 1;
    GLOBAL_THROW_ON_ERROR = 1;
    GLOBAL_THROW_ON_ASSERT = 1;
    Mau::kMauFeatures[0] = 0u;
    //Chip::kChipUseMutex = false;
    Packet::kPktInitTimeRandOnAlloc = false;
    PhvFactory::kPhvInitTimeRandOnAlloc = false;
    MauOpHandlerCommon::kZeroisePushPopData = false;
    MauMapram::kMapramWriteData1HasTime = true;
    MauStatsAlu::kKeepFullResStats = true;
    MauMemory::kKeepFullResStats = true;
    MauMemory::kAccessFullResStats = true;
    MauLookupResult::kRelaxLookupShiftPfePosCheck = true;
    MauLookupResult::kRelaxLookupShiftOpPosCheck = true;
    MauSramRowReg::kRelaxOfloRdMuxCheck = true;
    MauSramRowReg::kRelaxSynth2PortFabricCheck = true;
    MauAddrDist::kRelaxActionAddrsConsumedCheck = true;
    MauAddrDist::kRelaxStatsAddrsConsumedCheck = true;
    MauAddrDist::kRelaxMeterAddrsConsumedCheck = true;
    MauAddrDist::kRelaxIdletimeAddrsConsumedCheck = true;
    MauDependencies::kRelaxDelayCheck = true;
    MauLogicalTable::kRelaxPairedLtcamErrors = true;
    Instr::kRelaxInstrFormatCheck = true;
    Instr::kRelaxEqNeqConfigCheck = false;
    MauSnapshotCommon::kSnapshotUsePhvTime = true;
    MauSnapshotCommon::kSnapshotMaskThreadFields = false;
    MauAddrDist::kRelaxPrePfeAddrCheck = true;
    MauStatefulAlu::kRelaxSaluPredRsiCheck = false;
    MauDependencies::kRelaxDependencyCheck = true;
    MauStatefulCounters::kSynchronousStatefulCounterClear = false;
    MauStatefulCounters::kStatefulCounterTickCheckTime = true;
    Mau::kResetUnusedLookupResults = true;
    Mau::kSetNextTablePred = true;
    MauMeter::kRelaxExponentCheck = true;
    MauAddrDist::kMeterSweepOnDemand = false;
    Chip::kUseGlobalTimeIfZero = false;
    Clot::kRelaxOverlapCheck = false;
    Clot::kAllowDuplicate = true;
    Clot::kAllowRepeatEmit = true;
#endif /* !DV_MODE */
 }

 void TestUtil::set_dv_test(int XX) {
    bool aje = ((XX % 2) == 0);
    bool cc  = ((XX % 2) == 1);
    bool post_pfe      = (((aje) && (XX >  72)) || ((cc) && (XX > 41)));
    bool post_hyperdev = (((aje) && (XX > 136)) || ((cc) && (XX > 67)));
    bool do_setup_counters = false;
    bool do_setup_snapshot = false;

    // Stash test number and 'vintage'
    dv_test_ = XX;
    dv_vintage_ = kVintageOriginal;
    if (post_pfe)  dv_vintage_ = kVintagePostPfe;
    if (post_hyperdev)  dv_vintage_ = kVintagePostHyperdev;

    // Always disable/relax certain things. In particular:
    // 'special' MAU[0] - (get this anyway ifdef DV_MODE)
    //   - without this tests 30,49,66,67,84,94,108 fail
    Mau::kMauFeatures[0] = 0u;

    if (!post_hyperdev) {
      // OLD DV tests did not setup counters/snapshot - do it unprompted
      do_setup_counters = true; do_setup_snapshot = true;

      // OLD DV tests often wrote to invalid SRAMs/TCAMs
      MauMemory::kAllowBadSramWrite = true;
      MauMemory::kAllowBadTcamWrite = true;
      MauMemory::kAllowBadMapramWrite = true;
      // OLD DV tests programmed mapram_ctl - this no longer exists
      // so we need to relax the SRAM VPN checks
      MauSram::kRelaxSramVpnCheck = true;
      // OLD DV tests often had shift+pfe >= 64 so relax check
      MauLookupResult::kRelaxLookupShiftPfePosCheck = true;
      // Payload shifter didn't exist initially
      MauLookupResult::kRelaxPayloadShifterEnabledCheck = true;
      // OLD DV tests didn't program deferred_ram_ctl
      MauAddrDist::kRelaxPacketActionAtHdrCheck = true;
      // OLD DV tests didn't know about DinPower features
      Mau::kMauDinPowerMode = false;
      // OLD DV tests didn't setup logical_table_thread or adr_dist_table_thread
      MauDependencies::kRelaxThreadCheck = true;
      // or meter_alu_thread
      MauMeterAlu::kRelaxThreadCheck = true;
      // Also relax Gateway replication checks
      MauGatewayPayload::kRelaxGatewayReplicationCheck = true;
    }
    if (!post_pfe) {
      // OLD DV tests didn't set pfe in addresses so enable addresses globally
      Address::kGlobalAddrEnable = true;
      // OLD DV tests had OP==NOP in globally-enabled meter addresses
      MauLogicalTable::kRelaxHdrtimeMeterAddrNopCheck = true;
    }

    if (do_setup_counters) {
      // Maybe setup counters for old tests
      auto& mau_base = RegisterUtils::ref_mau(pipe_,stage_);
      auto& mm_regs = mau_base.rams.match.merge;
      //uint32_t all_hit  = (2<<21)|(2<<18)|(2<<15)|(2<<12)|(2<<9)|(2<<6)|(2<<3)|(2<<0);
      uint32_t all_miss = (1<<21)|(1<<18)|(1<<15)|(1<<12)|(1<<9)|(1<<6)|(1<<3)|(1<<0);
      OutWord((void*)&mm_regs.mau_table_counter_ctl[0], all_miss);
      OutWord((void*)&mm_regs.mau_table_counter_ctl[1], all_miss);
    }
    if (do_setup_snapshot || (XX==146)) {
      setup_snapshot(true,  1); // Ingress Armed
      setup_snapshot(false, 1); // Egress Armed
    }
  }

  void TestUtil::finish_test(int arg) {
    // Reset global error
    GLOBAL_RMT_ERROR = 0;

    const char *ctlstr[8] = { "disabled", "miss", "hit",
                              "gw_miss", "gw_hit", "gw_inhibit",
                              "unused", "unused" };
    int nStages = kMaxStages;
    RmtObjectManager *om = get_objmgr();
    if (om == NULL) return;

    // Go through all pipes/stages/tables looking for
    // non-zero table counters and if we find one print
    // out its value and what it was counting
    for (int pipe = 0; pipe < kMaxPipes; pipe++) {
      for (int stage = 0; stage < nStages; stage++) {
        Mau *mau = om->mau_lookup(pipe,stage);
        if (mau != NULL) {
          auto& mau_base = RegisterUtils::ref_mau(pipe,stage);
          auto& mm_regs = mau_base.rams.match.merge;

          for (int table = 0; table < kMaxLogTabs; table++) {
            auto a_tab_ctr = &mm_regs.mau_table_counter[table][0];
            uint32_t v_tab_ctr = InWord((void*)a_tab_ctr);
            if (v_tab_ctr != 0u) {
              auto a_tab_ctr_ctl = &mm_regs.mau_table_counter_ctl[table / 8];
              uint32_t v_tab_ctr_ctl = InWord((void*)a_tab_ctr_ctl);
              uint32_t ctl = (v_tab_ctr_ctl >> ((table % 8) * 3)) & 0x7;
              printf("ENDTEST[%d]<%d,%d,%d,%d> COUNTER N_%s = %d\n",
                     dv_test_, chip_, pipe, stage, table,
                     ctlstr[ctl], v_tab_ctr);
            }
          }
        }
      }
    }
    // Go through all pipes/stages/tables looking for
    // snapshotted info and if we find any print out a
    // what was on the various result buses
    for (int pipe = 0; pipe < kMaxPipes; pipe++) {
      for (int stage = 0; stage < nStages; stage++) {
        Mau *mau = om->mau_lookup(pipe,stage);
        if (mau != NULL) {
          auto& mau_base = RegisterUtils::ref_mau(pipe,stage);
          auto& mm_regs = mau_base.rams.match.merge;
          auto& snapshot = mau_base.dp.snapshot_ctl;
          auto a_snap_fsm_ing  = &snapshot.mau_fsm_snapshot_cur_stateq[0];
          auto a_snap_fsm_egr  = &snapshot.mau_fsm_snapshot_cur_stateq[1];
          uint32_t v_fsm_ing = InWord((void*)a_snap_fsm_ing);
          uint32_t v_fsm_egr = InWord((void*)a_snap_fsm_egr);
          if ((v_fsm_ing == 3) || (v_fsm_egr == 3)) {
            for (int xm = 0; xm < 16; xm++) {
              auto a_snap_xm = &mm_regs.mau_snapshot_physical_exact_match_hit_address[xm];
              uint32_t v_snap_xm = InWord((void*)a_snap_xm);
              if (v_snap_xm != 0u) {
                printf("ENDTEST[%d]<%d,%d,%d> XM_HIT_ADDR[%d] = 0x%06x\n",
                       dv_test_, chip_, pipe, stage, xm, v_snap_xm);
              }
            }
            for (int tm = 0; tm < 16; tm++) {
              auto a_snap_tm = &mm_regs.mau_snapshot_physical_tcam_hit_address[tm];
              uint32_t v_snap_tm = InWord((void*)a_snap_tm);
              if (v_snap_tm != 0u) {
                printf("ENDTEST[%d]<%d,%d,%d> TM_HIT_ADDR[%d] = 0x%06x\n",
                       dv_test_, chip_, pipe, stage, tm, v_snap_tm);
              }
            }
          }
        }
      }
    }
    // Go through each MAU dumping out info it there's been a PHV
    for (int pipe = 0; pipe < kMaxPipes; pipe++) {
      for (int stage = 0; stage < nStages; stage++) {
        Mau *mau = om->mau_lookup(pipe,stage);
        if ((mau != NULL) && (mau->mau_info_read("MAU_N_PHVS") > 0u)) {
          printf("ENDTEST[%d]<%d,%d,%d> DUMP INFO:\n",
                 dv_test_, chip_, pipe, stage);
          mau->mau_info()->dump();
        }
      }
    }
  }

  MODEL_CHIP_NAMESPACE::RmtObjectManager *TestUtil::get_objmgr() const {
    model_core::Model *model = get_model();
    if (model == NULL) return NULL;
    model_core::ChipInterface *chip_if = model->GetChip(chip_);
    if (chip_if == NULL) return NULL;
    MODEL_CHIP_NAMESPACE::Chip *chip_impl = dynamic_cast<MODEL_CHIP_NAMESPACE::Chip*>(chip_if);
    if (chip_impl == NULL) return NULL;
    return chip_impl->GetObjectManager();
  }

  // Some random number helper funcs
  //
  // Knuth's MMIX 64b linear congruence
  uint64_t TestUtil::mmix_rand64(uint64_t v) {
    return (v * UINT64_C(6364136223846793005)) + UINT64_C(1442695040888963407);
  }
  // Given some input v and some index i, deterministically generate an i'th value
  uint64_t TestUtil::xrand64(uint64_t v, uint64_t i, uint8_t width) {
    static uint64_t primes[16] = {
      UINT64_C(1111111111111111111), UINT64_C(1122334455667788991),
      UINT64_C(1799999999999999999), UINT64_C(2015121110987654321),
      UINT64_C(2223243435546756677), UINT64_C(2327074306453592351),
      UINT64_C(3203000719597029781), UINT64_C(3319393725273939133),
      UINT64_C(3842148274728412483), UINT64_C(5555555555555555533),
      UINT64_C(6082394749206781697), UINT64_C(6787988999657777797),
      UINT64_C(8093914354023690019), UINT64_C(8888888897888888899),
      UINT64_C(9876534021204356789), UINT64_C(9999999992999999999) };
    RMT_ASSERT(width <= 64);
    uint64_t mask  = UINT64_C(0xFFFFFFFFFFFFFFFF)  >>  (64 - width);
    uint8_t  shift = (width <= 40) ?((((i + 5) * 13) +  7) % 16) :0;
    uint64_t p1    = primes[         (((i + 3) * 13) + 11) % 16   ];
    uint64_t p2    = primes[         (((i + 2) * 11) +  7) % 16   ];
    return ((((v + i + UINT64_C(12356789)) * p1) + p2) >> shift) & mask;
  }
  uint32_t TestUtil::xrand32(uint64_t v, uint64_t i, uint8_t width) {
    RMT_ASSERT(width <= 32);
    return static_cast<uint32_t>(xrand64(v, i, width));
  }
  uint16_t TestUtil::xrand16(uint64_t v, uint64_t i, uint8_t width) {
    RMT_ASSERT(width <= 16);
    return static_cast<uint16_t>(xrand64(v, i, width));
  }
  uint8_t TestUtil::xrand8(uint64_t v, uint64_t i, uint8_t width) {
    RMT_ASSERT(width <= 8);
    return static_cast<uint8_t>(xrand64(v, i, width));
  }
  bool TestUtil::xrandbool(uint64_t v, uint64_t i) {
    return ((xrand64(v, i, 1) & 1) == 1);
  }
  int TestUtil::xrandrange(uint64_t v, uint64_t i, int min, int max) {
    RMT_ASSERT(min <= max);
    return min + static_cast<int>(xrand32(v, i) % (1 + max - min));
  }


  void TestUtil::PipeBusRead(uint64_t addr, uint64_t *data0, uint64_t *data1) {
    if (use_ind_regs_) {
      auto pcie_regs = RegisterUtils::addr_pcie_regs();
      uint32_t addr_lo = static_cast<uint32_t>(addr & UINT64_C(0xFFFFFFFF));
      uint32_t addr_hi = static_cast<uint32_t>(addr >> 32);
      OutWord((void*)&pcie_regs->cpu_ind_addr_low,  addr_lo);
      OutWord((void*)&pcie_regs->cpu_ind_addr_high, addr_hi);
      uint64_t d0 = UINT64_C(0);
      d0 |= (static_cast<uint64_t>(InWord((void*)&pcie_regs->cpu_ind_data00)));
      d0 |= (static_cast<uint64_t>(InWord((void*)&pcie_regs->cpu_ind_data01)) << 32);
      if (data0 != NULL) *data0 = d0;
      uint64_t d1 = UINT64_C(0);
      d1 |= (static_cast<uint64_t>(InWord((void*)&pcie_regs->cpu_ind_data10)));
      d1 |= (static_cast<uint64_t>(InWord((void*)&pcie_regs->cpu_ind_data11)) << 32);
      if (data1 != NULL) *data1 = d1;
    } else {
      if (get_debug()) printf("TestUtil::PipeBusRead(%d,0x%016" PRIx64 ")\n",
                              chip_, addr);
      model_->IndirectRead(chip_, addr, data0, data1);
    }
  }
  void TestUtil::PipeBusWrite(uint64_t addr, uint64_t data0, uint64_t data1) {
    if (use_ind_regs_) {
      auto pcie_regs = RegisterUtils::addr_pcie_regs();
      uint32_t addr_lo = static_cast<uint32_t>(addr & UINT64_C(0xFFFFFFFF));
      uint32_t addr_hi = static_cast<uint32_t>(addr >> 32);
      OutWord((void*)&pcie_regs->cpu_ind_addr_low,  addr_lo);
      OutWord((void*)&pcie_regs->cpu_ind_addr_high, addr_hi);
      uint32_t d11 = static_cast<uint32_t>(data1 >> 32);
      uint32_t d10 = static_cast<uint32_t>(data1 & UINT64_C(0xFFFFFFFF));
      uint32_t d01 = static_cast<uint32_t>(data0 >> 32);
      uint32_t d00 = static_cast<uint32_t>(data0 & UINT64_C(0xFFFFFFFF));
      OutWord((void*)&pcie_regs->cpu_ind_data11, d11);
      OutWord((void*)&pcie_regs->cpu_ind_data10, d10);
      OutWord((void*)&pcie_regs->cpu_ind_data01, d01);
      OutWord((void*)&pcie_regs->cpu_ind_data00, d00);
    } else {
      if (get_debug()) printf("TestUtil::PipeBusWrite(%d,0x%016" PRIx64 ") = 0x%016" PRIx64 "\n",
                              chip_, addr, data0);
      model_->IndirectWrite(chip_, addr, data0, data1);
    }
  }
  // Call IndirectWrite but remap field in addr from val_old to val_new
  void TestUtil::RemapIndirectWrite(int field_pos, int field_width,
                                    uint32_t val_old, uint32_t val_new,
                                    uint64_t addr, uint64_t data0, uint64_t data1) {
    if ((field_width > 0) && (field_width <= 32) &&
        (field_pos >= 0) && (field_pos + field_width <= 64)) {
      uint32_t mask32 = (0xFFFFFFFFu >> (32-field_width));
      uint64_t mask64 = static_cast<uint64_t>(mask32);
      uint32_t val_in_addr = static_cast<uint32_t>((addr >> field_pos) & mask64);
      if (val_in_addr == (val_old & mask32)) {
        uint64_t addr_old = addr;
        addr &= ~(mask64 << field_pos);
        addr |= static_cast<uint64_t>(val_new & mask32) << field_pos;
        printf("RemapIndirectWrite(%d,%d,%d,%d 0x%016" PRIx64 " -> 0x%016" PRIx64 " %s\n",
               field_pos, field_width, val_old & mask32, val_new & mask32,
               addr_old, addr, (addr_old == addr) ?"same" :"");
      }
    }
    IndirectWrite(addr, data0, data1);
  }

  void TestUtil::set_phv_range(int pipe, int stage, int phv_start, int phv_end, bool ingress) {
    if (phv_start > phv_end) return;
    for (int phv = phv_start; phv <= phv_end; phv++)
      set_phv_ingress_egress(pipe, stage, phv, ingress);
  }
  void TestUtil::set_phv_range_all(int pipe, int stage, bool ingress) {
    set_phv_range(pipe, stage,
                  Phv::make_word_mapped(0,0),
                  Phv::make_word_mapped(6,kPhvsPerGrp-1), ingress);
  }
  void TestUtil::set_phv_ranges(int pipe, int stage,
                                int n0, int n1, int n2, int n3, int n4, int n5, int n6,
                                bool ingress) {
    if ((n0 > 0) || (n0 <= kPhvsPerGrp))
      set_phv_range(pipe, stage,
                    Phv::make_word_mapped(0,0), Phv::make_word_mapped(0,0)+n0-1,
                    ingress);
    if ((n1 > 0) || (n1 <= kPhvsPerGrp))
      set_phv_range(pipe, stage,
                    Phv::make_word_mapped(1,0), Phv::make_word_mapped(1,0)+n1-1,
                    ingress);
    if ((n2 > 0) || (n2 <= kPhvsPerGrp))
      set_phv_range(pipe, stage,
                    Phv::make_word_mapped(2,0), Phv::make_word_mapped(2,0)+n2-1,
                    ingress);
    if ((n3 > 0) || (n3 <= kPhvsPerGrp))
      set_phv_range(pipe, stage,
                    Phv::make_word_mapped(3,0), Phv::make_word_mapped(3,0)+n3-1,
                    ingress);
    if ((n4 > 0) || (n4 <= kPhvsPerGrp))
      set_phv_range(pipe, stage,
                    Phv::make_word_mapped(4,0), Phv::make_word_mapped(4,0)+n4-1,
                    ingress);
    if ((n5 > 0) || (n5 <= kPhvsPerGrp))
      set_phv_range(pipe, stage,
                    Phv::make_word_mapped(5,0), Phv::make_word_mapped(5,0)+n5-1,
                    ingress);
    if ((n6 > 0) || (n6 <= kPhvsPerGrp))
      set_phv_range(pipe, stage,
                    Phv::make_word_mapped(6,0), Phv::make_word_mapped(6,0)+n6-1,
                    ingress);
  }


  void TestUtil::set_phv_match(int pipe, int stage, int phv_word, bool ingress,
                               uint32_t value, uint32_t mask) {
    // Setup a snapshot match (for value/mask) on a particular PHV word
    // Note, this also calls funcs above to setup ingress/egress thread
    if ((pipe < 0) || (pipe >= kMaxPipes)) return;
    if ((stage < 0) || (stage >= kMaxStages)) return;
    if ((phv_word < 0) || (phv_word >= 224)) return;
    // We DO NOT MAP the passed in phv_word for match, only for capture
    int which_word  = phv_word % 32;
    int which_group = phv_word / 32;
    if (which_group > 6) return;
    int sizes[7]   = { 32, 32, 8,  8, 16, 16, 16 };
    int offsets[7] = {  0, 32, 0, 32,  0, 32, 64 };
    int size = sizes[which_group];
    int off  = offsets[which_group] + which_word;
    uint32_t size_mask = 0xFFFFFFFFu >> (32-size);
    uint64_t val64 = static_cast<uint64_t>(value & size_mask);
    uint64_t msk64 = static_cast<uint64_t>(mask & size_mask);
    if (mask != 0u) {
      // If we care about any bits set valid bit in val/mask
      val64 |= UINT64_C(1) << size;
      msk64 |= UINT64_C(1) << size;
    }
    uint64_t w0 = (~val64 & msk64) | ~msk64;
    uint64_t w1 =  (val64 & msk64) | ~msk64;
    auto& mau_base = RegisterUtils::ref_mau(pipe,stage);
    auto& snap_regs = mau_base.dp.snapshot_dp.snapshot_match;
    if (sizes[which_group] == 32) {
      auto snaphi0 = &snap_regs.mau_snapshot_match_subword32b_hi[off][0];
      auto snaphi1 = &snap_regs.mau_snapshot_match_subword32b_hi[off][1];
      auto snaplo0 = &snap_regs.mau_snapshot_match_subword32b_lo[off][0];
      auto snaplo1 = &snap_regs.mau_snapshot_match_subword32b_lo[off][1];
      OutWord((void*)snaphi0, static_cast<uint32_t>(w0 >> 16) & 0x1FFFFu);
      OutWord((void*)snaphi1, static_cast<uint32_t>(w1 >> 16) & 0x1FFFFu);
      OutWord((void*)snaplo0, static_cast<uint32_t>(w0 >>  0) &  0xFFFFu);
      OutWord((void*)snaplo1, static_cast<uint32_t>(w1 >>  0) &  0xFFFFu);
    } else if (sizes[which_group] == 16) {
      auto snap0 = &snap_regs.mau_snapshot_match_subword16b[off][0];
      auto snap1 = &snap_regs.mau_snapshot_match_subword16b[off][1];
      OutWord((void*)snap0, static_cast<uint32_t>(w0 >>  0) &  0x1FFFFu);
      OutWord((void*)snap1, static_cast<uint32_t>(w1 >>  0) &  0x1FFFFu);
    } else if (sizes[which_group] == 8) {
      auto snap0 = &snap_regs.mau_snapshot_match_subword8b[off][0];
      auto snap1 = &snap_regs.mau_snapshot_match_subword8b[off][1];
      OutWord((void*)snap0, static_cast<uint32_t>(w0 >>  0) &  0x1FFu);
      OutWord((void*)snap1, static_cast<uint32_t>(w1 >>  0) &  0x1FFu);
    }
    // However we DO MAP the passed in phv_word for thread programming
    phv_word = RmtDefs::map_mausnap_phv_index(phv_word);
    set_phv_ingress_egress(pipe, stage, phv_word, ingress);
  }
  void TestUtil::set_phv_capture(int pipe, int stage, int phv_word, bool ingress) {
    // Capture a word by matching on any value of it (ie mask=0u)
    set_phv_match(pipe, stage, phv_word, ingress, 0u, 0u);
  }
  uint64_t TestUtil::get_phv_capture_word(int pipe, int stage, int phv_word) {
    // Fish out a captured PHV word
    if ((pipe < 0) || (pipe >= kMaxPipes)) return UINT64_C(0xFFFFFFFFFFFFFFFF);
    if ((stage < 0) || (stage >= kMaxStages)) return UINT64_C(0xFFFFFFFFFFFFFFFF);
    if ((phv_word < 0) || (phv_word >= 224)) return UINT64_C(0xFFFFFFFFFFFFFFFF);
    int which_group = phv_word / 16;
    if (which_group > 13) return UINT64_C(0xFFFFFFFFFFFFFFFF);
    // MAP the passed in phv_word so we get the correct captured word
    phv_word = RmtDefs::map_mausnap_phv_index(phv_word);
    if ((phv_word < 0) || (phv_word >= kMaxPhvNum)) return UINT64_C(0xFFFFFFFFFFFFFFFF);
    int sizes[14]    = { 32, 32, 32, 32,  8,  8,  8,   8,  16,  16,  16,  16,  16,  16 };
    //int base[14]   = {  0,  0, 32, 32, 64, 64, 96,  96, 128, 128, 128, 176, 176, 176 };
    //int halves[14] = {  0,  0,  1,  1,  0,  0,  1,   1,   0,   0,   0,   1,   1,   1 };
    //int off  = phv_word - base[which_group];
    //int half = halves[which_group];
    if (sizes[which_group] == 32) {
      int off = Phv::map_word_abs_to_rel32(phv_word);
      auto a_caphi = RegisterUtils::addr_snapshot_capture32_hi(pipe, stage, off);
      auto a_caplo = RegisterUtils::addr_snapshot_capture32_lo(pipe, stage, off);
      uint32_t v_caphi = InWord((void*)a_caphi) & 0x1FFFFu;
      uint32_t v_caplo = InWord((void*)a_caplo) & 0x0FFFFu;
      return (static_cast<uint64_t>(v_caphi) << 16) | (static_cast<uint64_t>(v_caplo) << 0);
    } else if (sizes[which_group] == 16) {
      int off = Phv::map_word_abs_to_rel16(phv_word);
      auto a_cap = RegisterUtils::addr_snapshot_capture16(pipe, stage, off);
      uint32_t v_cap = InWord((void*)a_cap) & 0x1FFFFu;
      return (static_cast<uint64_t>(v_cap) << 0);
    } else if (sizes[which_group] == 8) {
      int off = Phv::map_word_abs_to_rel8(phv_word);
      auto a_cap = RegisterUtils::addr_snapshot_capture8(pipe, stage, off);
      uint32_t v_cap = InWord((void*)a_cap) & 0x1FFu;
      return (static_cast<uint64_t>(v_cap) << 0);
    } else {
      RMT_ASSERT(0);
    }
  }
  void TestUtil::set_capture_timestamp(int pipe, int stage, bool ingress, uint64_t timestamp) {
    // Setup for timestamp-based snapshot - next PHV with thread_active will be captured
    if ((pipe < 0) || (pipe >= kMaxPipes)) return;
    if ((stage < 0) || (stage >= kMaxStages)) return;
    auto& mau_base = RegisterUtils::ref_mau(pipe,stage);
    auto& snap_regs = mau_base.dp.snapshot_ctl;
    auto trigger_hi = &snap_regs.mau_snapshot_timestamp_trigger_hi;
    auto trigger_lo = &snap_regs.mau_snapshot_timestamp_trigger_lo;
    OutWord((void*)trigger_lo, static_cast<uint32_t>((timestamp >>  0) & 0xFFFFFFFFu));
    OutWord((void*)trigger_hi, static_cast<uint32_t>((timestamp >> 32) & 0xFFFFu));
    int ie = (ingress) ?0 :1;
    auto a_trigger_config = &snap_regs.mau_snapshot_config;
    uint32_t v_trigger_config = InWord((void*)a_trigger_config);
    OutWord((void*)a_trigger_config, v_trigger_config | (1u<<ie));
  }
  uint64_t TestUtil::get_capture_timestamp_now(int pipe, int stage, bool ingress) {
    // Retrieve value of free-running timer - single one - not separate ingress/egress
    if ((pipe < 0) || (pipe >= kMaxPipes)) return UINT64_C(0xFFFFFFFFFFFFFFFF);
    if ((stage < 0) || (stage >= kMaxStages)) return UINT64_C(0xFFFFFFFFFFFFFFFF);
    auto& mau_base = RegisterUtils::ref_mau(pipe,stage);
    auto& snap_regs = mau_base.dp.snapshot_ctl;
    auto ts_hi = &snap_regs.mau_snapshot_timestamp_hi;
    auto ts_lo = &snap_regs.mau_snapshot_timestamp_lo;
    uint32_t hi = InWord((void*)ts_hi) & 0xFFFF;
    uint32_t lo = InWord((void*)ts_lo);
    return (static_cast<uint64_t>(hi) << 32) | (static_cast<uint64_t>(lo) << 0);
  }
  int TestUtil::setup_snapshot(int pipe, int stage, bool ingress, int fsm_state) {
    // Setup the snapshot FSM
    if ((pipe < 0) || (pipe >= kMaxPipes)) return -1;
    if ((stage < 0) || (stage >= kMaxStages)) return -1;
    auto& mau_base = RegisterUtils::ref_mau(pipe,stage);
    auto& snapshot = mau_base.dp.snapshot_ctl;
    auto a_snap_fsm = &snapshot.mau_fsm_snapshot_cur_stateq[ingress ?0 :1];
    uint32_t v_snap_fsm = InWord((void*)a_snap_fsm);
    if (fsm_state >= 0) OutWord((void*)a_snap_fsm, static_cast<uint32_t>(fsm_state) & 0x3);
    return static_cast<int>(v_snap_fsm & 0x3); // Return prev setting
  }




  void TestUtil::set_dependency(int pipe, int stage, int dep, bool ingress) {
    if ((pipe < 0) || (pipe >= kMaxPipes)) return;
    if ((stage < 0) || (stage >= kMaxStages)) return;
    if (get_debug()) printf("TestUtil<%d,%d>::set_dependency(%d{0=conc,1=act,2=match},%d)\n",
                            pipe, stage, dep, ingress);
    int ing_eg = (ingress) ?0 :1;
    uint32_t ing_eg_bit = 1u << ing_eg;

    auto& mau_base = RegisterUtils::ref_mau(pipe,stage);
    auto& dp_regs = mau_base.dp;
    auto& mm_regs = mau_base.rams.match.merge;
    auto a_stage_conc = &dp_regs.stage_concurrent_with_prev;
    auto a_match_inp_muxsel = &dp_regs.match_ie_input_mux_sel;
    auto a_phv_fifo_en = &dp_regs.phv_fifo_enable;
    auto a_pred_ctl = &mm_regs.predication_ctl[ing_eg];
    auto a_act_out_delay = &dp_regs.action_output_delay[ing_eg];
    uint32_t v_stage_conc = InWord((void*)a_stage_conc);
    uint32_t v_match_inp_muxsel = InWord((void*)a_match_inp_muxsel);
    uint32_t v_phv_fifo_en = InWord((void*)a_phv_fifo_en);
    uint32_t v_pred_ctl = InWord((void*)a_pred_ctl);
    uint32_t v_act_out_delay = InWord((void*)a_act_out_delay);
    bool en_prev_stage_fin_out = false;
    int v_prev_stage_fin_out_delay = -1;

    if (dep == kDepConcurrent) {
      // Action output should be in 13-19
      v_act_out_delay = 15;

      v_stage_conc |= ing_eg_bit;
      // Clear ing_eg_bit => iPHV
      v_match_inp_muxsel &= ~ing_eg_bit;
      // Enable action output - bits 3 & 2 so shift by 2
      v_phv_fifo_en |= ((0x1 << 2) << ing_eg);
      setp_predication_ctl_start_table_fifo_delay0(&v_pred_ctl, 3);

    } else if (dep == kDepAction) {
      en_prev_stage_fin_out = true;
      v_prev_stage_fin_out_delay = 2;
      v_act_out_delay = -1;

      v_stage_conc &= ~ing_eg_bit;
      // Clear ing_eg_bit => iPHV
      v_match_inp_muxsel &= ~ing_eg_bit;
      // Disable action output
      v_phv_fifo_en &= ~((0x1 << 2) << ing_eg);
      // start_table_fifo_enable programming is different on JBay!
      if (RmtObject::is_jbay_or_later()) {
        setp_predication_ctl_start_table_fifo_enable(&v_pred_ctl, 0x0);
      } else {
        setp_predication_ctl_start_table_fifo_enable(&v_pred_ctl, 0x1);
      }
      setp_predication_ctl_start_table_fifo_delay0(&v_pred_ctl, 3);

    } else if (dep == kDepMatch) {
      // Action output should be in 13-19
      v_act_out_delay = 15;

      v_stage_conc &= ~ing_eg_bit;
      // Set ing_eg_bit => oPHV
      v_match_inp_muxsel |= ing_eg_bit;
      // Unless we are MAU0 in which case we clear ing_eg_bit => iPhv
      if (Mau::must_use_iphv(stage)) v_match_inp_muxsel &= ~ing_eg_bit;
      // Enable action output - bits 3 & 2 so shift by 2
      v_phv_fifo_en |= ((0x1 << 2) << ing_eg);
      setp_predication_ctl_start_table_fifo_enable(&v_pred_ctl, 0x1);
      setp_predication_ctl_start_table_fifo_delay0(&v_pred_ctl, 16);
    }

    if (stage > 0) {
      // In certain cases we also fiddle with the
      // config of the previous stage
      auto& mau_base_prev = RegisterUtils::ref_mau(pipe,stage-1);
      auto& dp_regs_prev = mau_base_prev.dp;
      auto a_phv_fifo_en_prev = &dp_regs_prev.phv_fifo_enable;
      uint32_t v_phv_fifo_en_prev = InWord((void*)a_phv_fifo_en_prev);
      // Enable/disable final_output - bits 1 & 0 so shift by 0
      if (en_prev_stage_fin_out)
        v_phv_fifo_en_prev |= ((0x1 << 0) << ing_eg);
      else
        v_phv_fifo_en_prev &= ~((0x1 << 0) << ing_eg);
      OutWord((void*)a_phv_fifo_en_prev, v_phv_fifo_en_prev);
      auto a_next_stage_dep = &dp_regs_prev.next_stage_dependency_on_cur[ing_eg];
      OutWord((void*)a_next_stage_dep, dep);

      if (v_prev_stage_fin_out_delay >= 0) {
        //Removed in bfnregs 20150107_182406_7982_mau_dev
        //auto a_fin_out_delay_prev = &dp_regs_prev.final_output_delay[ing_eg][0];
        //uint32_t v_fin_out_delay_prev = InWord((void*)a_fin_out_delay_prev);
        //OutWord((void*)a_fin_out_delay_prev, v_prev_stage_fin_out_delay);
      }
    }

    // Finally we configure the specified stage
    OutWord((void*)a_stage_conc, v_stage_conc);
    OutWord((void*)a_match_inp_muxsel, v_match_inp_muxsel);
    OutWord((void*)a_phv_fifo_en, v_phv_fifo_en);
    OutWord((void*)a_pred_ctl, v_pred_ctl);
    auto a_cur_stage_dep = &dp_regs.cur_stage_dependency_on_prev[ing_eg];
    OutWord((void*)a_cur_stage_dep, dep);
    OutWord((void*)a_act_out_delay, v_act_out_delay);
  }


Lfltr_rspec *
TestUtil::lfltr_reg_get(int pipe)
{
  auto *a = RegisterUtils::addr_lfltr(pipe);
  RMT_ASSERT(a != NULL);
  return a;
}

void TestUtil::lfltr_config(int pipe, bool disable, uint32_t timeout)
{
  Lfltr_rspec *lfreg_p = lfltr_reg_get(pipe);
  uint32_t  csr = 0u;
  setp_lfltr_common_ctrl_learn_dis(&csr, (disable ? 1 : 0));
  OutWord(&lfreg_p->ctrl.common_ctrl, csr);

  // timeout is used as packet count to push LQs for utest
  setp_lfltr_lqt_timeout_timeout(&csr, timeout); // * (RmtDefs::kRmtClocksPerSec/1000000));
  OutWord(&lfreg_p->ctrl.lqt_timeout, csr);

  // initialize hash seed and GF matrix

  for (int i=0; i < RmtDefs::kLearnFilterNumHash; i++) {
    //uint32_t  rand_seed = random (); // XXX seed the generator...
    OutWord(&lfreg_p->ctrl.hash_seed[i], (uint32_t)random());
  }

  for (int i=0; i < RmtDefs::kLearnFilterNumHash; i++) {
    for (int j=0; j < RmtDefs::kLearnFilterHashIndexBits; j++) {
      for (int k=0; k < RmtDefs::kLearnQuantumWidth/32; k++) { // 12*32 = 384
        switch (k)
        {
          case 0: OutWord (&lfreg_p->hash[i].hash_array[j].hash_array_0_12, (uint32_t)random()); break;
          case 1: OutWord (&lfreg_p->hash[i].hash_array[j].hash_array_1_12, (uint32_t)random()); break;
          case 2: OutWord (&lfreg_p->hash[i].hash_array[j].hash_array_2_12, (uint32_t)random()); break;
          case 3: OutWord (&lfreg_p->hash[i].hash_array[j].hash_array_3_12, (uint32_t)random()); break;
          case 4: OutWord (&lfreg_p->hash[i].hash_array[j].hash_array_4_12, (uint32_t)random()); break;
          case 5: OutWord (&lfreg_p->hash[i].hash_array[j].hash_array_5_12, (uint32_t)random()); break;
          case 6: OutWord (&lfreg_p->hash[i].hash_array[j].hash_array_6_12, (uint32_t)random()); break;
          case 7: OutWord (&lfreg_p->hash[i].hash_array[j].hash_array_7_12, (uint32_t)random()); break;
          case 8: OutWord (&lfreg_p->hash[i].hash_array[j].hash_array_8_12, (uint32_t)random()); break;
          case 9: OutWord (&lfreg_p->hash[i].hash_array[j].hash_array_9_12, (uint32_t)random()); break;
          case 10: OutWord (&lfreg_p->hash[i].hash_array[j].hash_array_10_12,(uint32_t)random()); break;
          case 11: OutWord (&lfreg_p->hash[i].hash_array[j].hash_array_11_12,(uint32_t)random()); break;
          default: assert(0); break;
        }
      }
    }
  }
}

void TestUtil::lfltr_clear(int pipe)
{
  Lfltr_rspec *lfreg_p = lfltr_reg_get(pipe);
  uint32_t csr = 0u;
  setp_lfltr_bft_ctrl_clear(&csr, 1);
  OutWord(&lfreg_p->ctrl.bft_ctrl, csr);
}


// Mirroring - helper functions for bit manipulations
// since metadata is spread across various registers and it was moved between regiters,
// just get all the meta and program all meta registers in a single function
// (XXX too many parameters to this function) - but used only once
void
TestUtil::mir_session_set_metadata(uint32_t& meta0, uint32_t& meta1,
                                   uint32_t& meta2, uint32_t& meta3, uint32_t& meta4,
                                   uint32_t egr_port, uint32_t icos, uint32_t pipe_mask,
                                   uint32_t color, uint32_t hash1, uint32_t hash2,
                                   uint32_t mgid1, uint32_t mgid1_v,
                                   uint32_t mgid2, uint32_t mgid2_v,
                                   uint32_t c2c_v, uint32_t c2c_cos, uint32_t deflect
                                  )
{
    // using new definitions from Alain
    meta0 = ((icos & 0x7) |
             (1<<3) | /* egress valid */
             ((egr_port & 0x1FF) << 4) |
             /* XXX shat is eport qid ?? [17:13] */
             ((color & 0x3) << 18) |
             ((pipe_mask & 0xF) << 20) |
             ((hash1 & 0xFF) << 24));

    meta1 = (((hash1 >> 8) & 0x1F) |
             ((hash2 & 0x1FFF) << 5) |
             ((mgid1 & 0x3FFF) << 18));

    meta2 = (((mgid1 >> 14) & 0x3) |
             ((mgid1_v & 0x1) << 2) |
             ((mgid2 & 0xFFFF) << 3) |
             ((mgid2_v & 0x1) << 19));

    meta3 = ((c2c_cos & 0x3) << 30);

    meta4 = (((c2c_cos >> 2) & 0x01) |
             ((c2c_v & 0x1) << 1) |
             ((deflect & 0x1) << 2));

}

  void TestUtil::imem_config(int pipe, int stage, int phvNum,
                             int opNum, int opColour, uint32_t opInstr) {
    if (get_debug()) printf("TestUtil<%d,%d>::imem_config(opNum=%d,phvNum=%d,op=%d(0x%08x))\n",
                            pipe, stage, opNum, phvNum, opInstr, opInstr);
    if ((pipe < 0) || (pipe >= kMaxPipes)) return;
    if ((stage < 0) || (stage >= kMaxStages)) return;
    if ((phvNum < 0) || (phvNum >= kMaxPhvNum)) return;
    if ((opNum < 0) || (opNum >= kMaxOpNum)) return;
    if ((opColour < 0) || (opColour > 1)) return;

    int which_word = phvNum % kPhvsPerGrp;
    int which_group = phvNum / kPhvsPerGrp;
    if (which_group > 6) return;
    // Barf noisily if asking for (posssible) Mocha/Dark word
    assert((which_word >= 0) && (which_word < 12));

    //auto& mau_base = RegisterUtils::ref_mau(pipe,stage);
    //auto& imem_regs = mau_base.dp.imem;
    volatile uint32_t *a_imem;
    uint32_t  v_imem;
    switch (Phv::which_width(phvNum)) {
      case 32:
        which_word = Phv::map_word_abs_to_rel32(phvNum);
        a_imem = RegisterUtils::addr_imem32(pipe,stage,opNum, which_word);
        //a_imem = &imem_regs.imem_subword32[which_word][opNum];
        v_imem = InWord((void*)a_imem);
        RegisterUtils::do_setp_imem_subword32_instr(&v_imem, opInstr);
        RegisterUtils::do_setp_imem_subword32_color(&v_imem, (uint32_t)opColour);
        RegisterUtils::do_setp_imem_subword32_parity(&v_imem, parity32(v_imem));
        OutWord((void*)a_imem, v_imem);
        break;
      case 8:
        which_word = Phv::map_word_abs_to_rel8(phvNum);
        a_imem = RegisterUtils::addr_imem8(pipe,stage,opNum, which_word);
        //a_imem = &imem_regs.imem_subword8[which_word][opNum];
        v_imem = InWord((void*)a_imem);
        RegisterUtils::do_setp_imem_subword8_instr(&v_imem, opInstr);
        RegisterUtils::do_setp_imem_subword8_color(&v_imem, (uint32_t)opColour);
        RegisterUtils::do_setp_imem_subword8_parity(&v_imem, parity32(v_imem));
        OutWord((void*)a_imem, v_imem);
        break;
      case 16:
        which_word = Phv::map_word_abs_to_rel16(phvNum);
        a_imem = RegisterUtils::addr_imem16(pipe,stage,opNum, which_word);
        //a_imem = &imem_regs.imem_subword16[which_word][opNum];
        v_imem = InWord((void*)a_imem);
        RegisterUtils::do_setp_imem_subword16_instr(&v_imem, opInstr);
        RegisterUtils::do_setp_imem_subword16_color(&v_imem, (uint32_t)opColour);
        RegisterUtils::do_setp_imem_subword16_parity(&v_imem, parity32(v_imem));
        OutWord((void*)a_imem, v_imem);
        break;
      default: return;
    }
  }



  void TestUtil::table_config(int pipe, int stage, int log_table, bool ingress) {

    if (get_debug()) printf("TestUtil<%d,%d>::table_config(%d)\n",
                            pipe, stage, log_table);

    if ((pipe < 0) || (pipe >= kMaxPipes)) return;
    if ((stage < 0) || (stage >= kMaxStages)) return;
    if ((log_table < 0) || (log_table >= kMaxLogTabs)) return;

    int ing_eg = (ingress) ?0 :1;
    uint32_t ing_eg_bit = 1u << log_table;

    auto& mau_base = RegisterUtils::ref_mau(pipe,stage);
    auto& dp_regs = mau_base.dp;
    auto& mm_regs = mau_base.rams.match.merge;
    auto& adist_regs = mau_base.rams.match.adrdist;

    // Setup predication_ctl regs
    auto a_my_pred_ctl = &mm_regs.predication_ctl[ing_eg];
    auto a_oth_pred_ctl = &mm_regs.predication_ctl[1-ing_eg];
    uint32_t v_my_pred_ctl = InWord((void*)a_my_pred_ctl );
    uint32_t v_oth_pred_ctl = InWord((void*)a_oth_pred_ctl );
    // NB. Assuming table thread config in bottom 16 LSBs of word
    v_my_pred_ctl |= (ing_eg_bit << 0);
    v_oth_pred_ctl &= ~(ing_eg_bit << 0);
    OutWord((void*)a_my_pred_ctl, v_my_pred_ctl, "my predication");
    OutWord((void*)a_oth_pred_ctl, v_oth_pred_ctl, "oth predication");

    // Then imem_table_addr - controls LSB instr addr
    auto a_imem_taddr_egress = &dp_regs.imem_table_addr_egress;
    uint32_t v_imem_taddr_egress = InWord((void*)a_imem_taddr_egress);
    // Assume in bottom 16 LSBs again
    if (ingress)
      v_imem_taddr_egress &= ~(ing_eg_bit << 0);
    else
      v_imem_taddr_egress |= (ing_eg_bit << 0);
    OutWord((void*)a_imem_taddr_egress, v_imem_taddr_egress);

    // Then logical_table_thread - replicated x3
    auto a_lt_thread = &mm_regs.logical_table_thread[0];
    uint32_t v_lt_thread = InWord((void*)a_lt_thread);
    if (ingress) {
      v_lt_thread |= (ing_eg_bit << 0);
      v_lt_thread &= ~(ing_eg_bit << 16);
    } else {
      v_lt_thread &= ~(ing_eg_bit << 0);
      v_lt_thread |= (ing_eg_bit << 16);
    }
    for (int i = 0; i < 3; i++) {
      OutWord((void*)&mm_regs.logical_table_thread[i], v_lt_thread);
    }

    // Then adr_dist_table_thread - replicated x2
    auto a_ad_ing_thread = &adist_regs.adr_dist_table_thread[0][0];
    auto a_ad_egr_thread = &adist_regs.adr_dist_table_thread[1][0];
    uint32_t v_ad_ing_thread = InWord((void*)a_ad_ing_thread);
    uint32_t v_ad_egr_thread = InWord((void*)a_ad_egr_thread);
    if (ingress) {
      v_ad_ing_thread |= (ing_eg_bit << 0);
      v_ad_egr_thread &= ~(ing_eg_bit << 0);
    } else {
      v_ad_ing_thread &= ~(ing_eg_bit << 0);
      v_ad_egr_thread |= (ing_eg_bit << 0);
    }
    for (int i = 0; i < 2; i++) {
      OutWord((void*)&adist_regs.adr_dist_table_thread[0][i], v_ad_ing_thread);
      OutWord((void*)&adist_regs.adr_dist_table_thread[1][i], v_ad_egr_thread);
    }

    // No such register since mau_dev_07092015
    // Then exact_match_delay_config
    // Disable this - for now all Tables can be TernaryMatch
    //auto a_xm_delay_cnf = &mm_regs.exact_match_delay_config;
    //uint32_t v_xm_delay_cnf = InWord((void*)a_xm_delay_cnf);
    //v_xm_delay_cnf &= ~ing_eg_bit;
    //OutWord((void*)a_xm_delay_cnf, v_xm_delay_cnf);

    // Do this one instead - might come in handy
    //auto a_logical_table_thread = &mm_regs.logical_table_thread;
    uint32_t v_logical_table_thread = 0u;
    //logical_table_thread = InWord((void*)a_logical_table_thread);
    if (ingress) {
      v_logical_table_thread |=  ing_eg_bit;
      v_logical_table_thread &= ~(1u << (log_table+16));
    } else {
      v_logical_table_thread &= ~ing_eg_bit;
      v_logical_table_thread |=  (1u << (log_table+16));
    }
    //OutWord((void*)a_logical_table_thread, v_logical_table_thread);

    // Setup table counter to count miss/hit - even_tables=hit odd=miss
    uint32_t ctl = ((log_table % 2) == 0) ?2 :1; // hit=2 miss=1
    auto a_tab_ctr_ctl = &mm_regs.mau_table_counter_ctl[log_table / 8];
    uint32_t v_tab_ctr_ctl = InWord((void*)a_tab_ctr_ctl);
    v_tab_ctr_ctl &= ~(0x7 << ((log_table % 8) * 3)); // Clear old cfg
    v_tab_ctr_ctl |=   ctl << ((log_table % 8) * 3);  // Set new cfg
    OutWord((void*)a_tab_ctr_ctl, v_tab_ctr_ctl);
  }



  void TestUtil::physbus_config(int pipe, int stage, int log_table, int bus_num,
                                uint8_t nxtab_shift, uint16_t nxtab_mask,
                                uint16_t nxtab_dflt, uint16_t nxtab_miss,
                                uint8_t instr_shift, uint8_t instr_mask,
                                uint8_t instr_dflt, uint8_t instr_miss,
                                uint32_t data_shift, uint32_t data_mask,
                                uint32_t data_dflt, uint32_t data_miss) {

    auto& mau_base = RegisterUtils::ref_mau(pipe,stage);
    auto& mm_regs = mau_base.rams.match.merge;

    // Setup all PhysicalResultBus/LogicalTable shift/mask/dflt/miss values
    //
    // First NEXT TABLE values
    auto a_nxt_tab_fmt = &mm_regs.next_table_format_data[log_table];
    uint32_t v_nxt_tab_fmt = InWord((void*)a_nxt_tab_fmt);
    // As of regs 20140930 there is no longer a nxtab_shift field
    //setp_next_table_format_data_match_next_table_adr_shiftcount(&v_nxt_tab_fmt, nxtab_shift);
    setp_next_table_format_data_match_next_table_adr_mask(&v_nxt_tab_fmt, nxtab_mask);
    setp_next_table_format_data_match_next_table_adr_default(&v_nxt_tab_fmt, nxtab_dflt);
    setp_next_table_format_data_match_next_table_adr_miss_value(&v_nxt_tab_fmt, nxtab_miss);
    OutWord((void*)a_nxt_tab_fmt, v_nxt_tab_fmt);


    // Then ACTION INSTRUCTION ADDRs
    // Initially make shift vals same for TernaryMatch and every ExactMatch entry (5)
    //
    auto a_i_tm_shft = &mm_regs.mau_action_instruction_adr_tcam_shiftcount[bus_num];
    OutWord((void*)a_i_tm_shft, instr_shift);
    for (int mtch = 0; mtch < 5; mtch++) {
      auto a_i_xm_shft = &mm_regs.mau_action_instruction_adr_exact_shiftcount[bus_num][mtch];
      OutWord((void*)a_i_xm_shft, instr_shift);
    }
    // Make mask/default vals same for TernaryMatch ExactMatch
    for (int xm_tm = 0; xm_tm <= 1; xm_tm++) {
      auto a_i_mask = &mm_regs.mau_action_instruction_adr_mask[xm_tm][bus_num];
      OutWord((void*)a_i_mask, instr_mask);
      auto a_i_dflt = &mm_regs.mau_action_instruction_adr_default[xm_tm][bus_num];
      OutWord((void*)a_i_dflt, instr_dflt);
    }
    // Miss val
    auto a_i_miss = &mm_regs.mau_action_instruction_adr_miss_value[log_table];
    OutWord((void*)a_i_miss, instr_miss);


    // ACTION DATA ADDRs
    // Initially make shift vals same for TernaryMatch and every ExactMatch entry (5)
    //
    auto a_d_tm_shft = &mm_regs.mau_actiondata_adr_tcam_shiftcount[bus_num];
    OutWord((void*)a_d_tm_shft, data_shift);
    for (int mtch = 0; mtch < 5; mtch++) {
      auto a_d_xm_shft = &mm_regs.mau_actiondata_adr_exact_shiftcount[bus_num][mtch];
      OutWord((void*)a_d_xm_shft, data_shift);
    }
    // Make mask/default vals same for TernaryMatch ExactMatch
    for (int xm_tm = 0; xm_tm <= 1; xm_tm++) {
      auto a_d_mask = &mm_regs.mau_actiondata_adr_mask[xm_tm][bus_num];
      OutWord((void*)a_d_mask, data_mask);
      auto a_d_dflt = &mm_regs.mau_actiondata_adr_default[xm_tm][bus_num];
      OutWord((void*)a_d_dflt, data_dflt);
    }
    // Miss val
    auto a_d_miss = &mm_regs.mau_actiondata_adr_miss_value[log_table];
    OutWord((void*)a_d_miss, data_miss);

    // Payload shifter - activate everything for given bus
    for (int xm_tm = 0; xm_tm <= 1; xm_tm++) {
      auto a_payload_shifter = &mm_regs.mau_payload_shifter_enable[xm_tm][bus_num];
      OutWord((void*)a_payload_shifter, 0x3F);
    }

    // TIND_RAM_DATA_SIZE
    auto a_tind_ram_data_size = &mm_regs.tind_ram_data_size[bus_num];
    OutWord((void*)a_tind_ram_data_size, 5); // Which means 64 (2<<(5+1))
  }


  void TestUtil::ltcam_config(int pipe, int stage, int log_tcam,
                              int log_table, int phys_bus, uint8_t match_addr_shift) {

    if (get_debug()) printf("TestUtil<%d,%d>::ltcam_config(%d)\n",
                            pipe, stage, log_tcam);

    if ((pipe < 0) || (pipe >= kMaxPipes)) return;
    if ((stage < 0) || (stage >= kMaxStages)) return;
    if ((log_tcam < 0) || (log_tcam >= kMaxLogTcams)) return;
    if (phys_bus >= kMaxPhysBuses) return;
    if (log_table >= kMaxLogTabs) return;
    if (match_addr_shift < 0) return;

    auto& mau_base = RegisterUtils::ref_mau(pipe,stage);
    auto& tcam_regs = mau_base.tcams;
    auto a_tcam_match_addr_shift = &tcam_regs.tcam_match_adr_shift[log_tcam];
    OutWord((void*)a_tcam_match_addr_shift, match_addr_shift);

    auto& mm_regs = mau_base.rams.match.merge;
    if (log_table >= 0) {
      auto a_tcam_ltab_ixbar = &mm_regs.tcam_hit_to_logical_table_ixbar_outputmap[log_tcam];
      uint32_t v_tcam2l = InWord((void*)a_tcam_ltab_ixbar);
      setp_tcam_hit_to_logical_table_ixbar_outputmap_enabled_4bit_muxctl_enable(&v_tcam2l,
                                                                                0x1);
      setp_tcam_hit_to_logical_table_ixbar_outputmap_enabled_4bit_muxctl_select(&v_tcam2l,
                                                                                log_table);
      OutWord((void*)a_tcam_ltab_ixbar, v_tcam2l);
    }
    if (phys_bus >= 0) {
      auto a_tcam_phys_oxbar = &mm_regs.tcam_match_adr_to_physical_oxbar_outputmap[phys_bus];
      uint32_t v_tcam2p = InWord((void*)a_tcam_phys_oxbar);
      setp_tcam_match_adr_to_physical_oxbar_outputmap_enabled_3bit_muxctl_enable(&v_tcam2p,
                                                                                  0x1);
      setp_tcam_match_adr_to_physical_oxbar_outputmap_enabled_3bit_muxctl_select(&v_tcam2p,
                                                                                  log_tcam);
      OutWord((void*)a_tcam_phys_oxbar, v_tcam2p);

      // Update tind_ram_data_size to be 5
      auto a_tind_ram_data_size = &mm_regs.tind_ram_data_size[phys_bus];
      OutWord((void*)a_tind_ram_data_size, 5); // Which means 64 (2<<(5+1))
    }
#ifdef MODEL_CHIP_CB_OR_LATER
    auto a_mpr_tcam_table_oxbar_ctl = &tcam_regs.mpr_tcam_table_oxbar_ctl[log_tcam];
    if (log_table >= 0) OutWord((void*)a_mpr_tcam_table_oxbar_ctl, (1<<4)|log_table);
#endif
  }


  void TestUtil::sram_config(int pipe, int stage, int row, int col,
                             int type, int in_bus, int out_bus,
                             int log_table, int log_tcam,
                             int vpn0, int vpn1, bool egress) {

    assert(col > 1);
    if ((pipe < 0) || (pipe >= kMaxPipes)) return;
    if ((stage < 0) || (stage >= kMaxStages)) return;
    if ((row < 0) || (row >= kSramMaxRows)) return;
    if ((col < 0) || (col >= kSramMaxCols)) return;
    if ((type < 0) || (type > kMaxUnitramType)) return;
    if ((in_bus < 0) || (in_bus > 1)) return;
    if ((out_bus < 0) || (out_bus > 1)) return;
    if ((log_table >= kMaxLogTabs) || (log_tcam >= kMaxLogTcams)) return;

    auto& mau_base = RegisterUtils::ref_mau(pipe,stage);
    auto& ram_regs = mau_base.rams.map_alu.row[row].adrmux;
    auto& m_array_regs = mau_base.rams.array.row[row].ram[col];

    // Setup RAM ADDR MUX and UNIT RAM CTL
    int left_right = (col < 6) ? 0 : 1;
    int muxcol = col % 6;
    auto a_ramaddr_mux = &ram_regs.ram_address_mux_ctl[left_right][muxcol];
    auto a_unitram_ctl = &m_array_regs.unit_ram_ctl;
    uint32_t v_unitram_ctl = InWord((void*)a_unitram_ctl);
    uint32_t v_ramaddr_mux = InWord((void*)a_ramaddr_mux);


    if (type == kUnitramTypeMatch) {
      // Set RAM ADDR MUX to take hash input
      // NB. NO unitram_adr_mux_enable since registers 190914
      //setp_ram_address_mux_ctl_ram_unitram_adr_mux_enable(&v_ramaddr_mux, (uint32_t)0x1);

      setp_mau_adrmux_row_addrmap_ram_address_mux_ctl_ram_unitram_adr_mux_select (&v_ramaddr_mux, (uint32_t)0x1);
      // Set UNIT RAM CTL to select specified input bus and drive specified output bus
      setp_unit_ram_ctl_match_ram_matchdata_bus1_sel(&v_unitram_ctl, (uint32_t)in_bus);
      setp_unit_ram_ctl_match_result_bus_select(&v_unitram_ctl, (uint32_t)(1 << out_bus));

    } else if (type == kUnitramTypeTind) {
      // Set RAM ADDR MUX to take tind input
      // MUX must configured with val 2 if Tind is using in_bus 0, val 3 if in_bus 1
      setp_mau_adrmux_row_addrmap_ram_address_mux_ctl_ram_unitram_adr_mux_select(&v_ramaddr_mux, (uint32_t)(in_bus + 2));
      // Set UNIT RAM CTL to drive specified output bus
      setp_unit_ram_ctl_tind_result_bus_select(&v_unitram_ctl, (uint32_t)(1 << out_bus));

    } else if (type == kUnitramTypeAction) {
      // Set RAM ADDR MUX to select action row input for moment (no oflow/oflow2)
      setp_mau_adrmux_row_addrmap_ram_address_mux_ctl_ram_unitram_adr_mux_select(&v_ramaddr_mux, (uint32_t)5);
    }
    OutWord((void*)a_ramaddr_mux, v_ramaddr_mux);
    OutWord((void*)a_unitram_ctl, v_unitram_ctl, "unitram_ctl");


    // Set UNITRAM CONFIG to say SRAM type and possibly logical table
    auto a_unitram_cnf = &ram_regs.unitram_config[left_right][muxcol];
    uint32_t v_unitram_cnf = InWord((void*)a_unitram_cnf);
    // NB. To unitram_enable since registers 190914
    //setp_mau_adrmux_row_addrmap_unitram_config_unitram_enable(&v_unitram_cnf, (uint32_t)0x1);

    setp_mau_adrmux_row_addrmap_unitram_config_unitram_type(&v_unitram_cnf, (uint32_t)type);
    setp_mau_adrmux_row_addrmap_unitram_config_unitram_enable(&v_unitram_cnf, (uint32_t)0x1);
    if (egress) {
      setp_mau_adrmux_row_addrmap_unitram_config_unitram_egress(&v_unitram_cnf, (uint32_t)0x1);
    } else {
      setp_mau_adrmux_row_addrmap_unitram_config_unitram_ingress(&v_unitram_cnf, (uint32_t)0x1);
    }
    if (log_table >= 0) {
      setp_mau_adrmux_row_addrmap_unitram_config_unitram_logical_table(&v_unitram_cnf, (uint32_t)log_table);
    }
    OutWord((void*)a_unitram_cnf, v_unitram_cnf);


    // Set UNITRAM VPN to use specified VPN vals
    if ((vpn0 >= 0) || (vpn1 >= 0)) {
      if (type == kUnitramTypeMatch) {
        // VPNs now split depending on type (since regs 190914)
        auto a_matchram_vpn = &m_array_regs.match_ram_vpn;
        uint32_t v_matchram_vpn = InWord((void*)a_matchram_vpn);
        uint32_t vpn_lsbs = getp_match_ram_vpn_match_ram_vpn_lsbs(&v_matchram_vpn);
        if (vpn0 >= 0) {
          setp_match_ram_vpn_match_ram_vpn0(&v_matchram_vpn, (uint32_t)(vpn0 >> 2));
          // Clear vpn field 0 then setup to use vpn0
          vpn_lsbs &= ~(0x7 << (0 * 3));
          vpn_lsbs |= (((0 << 2) | (vpn0 & 0x3))  <<  (0 * 3));
        }
        if (vpn1 >= 0) {
          setp_match_ram_vpn_match_ram_vpn1(&v_matchram_vpn, (uint32_t)(vpn1 >> 2));
          // Clear vpn field 1 then setup to use vpn1
          vpn_lsbs &= ~(0x7 << (1 * 3));
          vpn_lsbs |= (((1 << 2) | (vpn1 & 0x3))  <<  (1 * 3));
        }
        setp_match_ram_vpn_match_ram_vpn_lsbs(&v_matchram_vpn, vpn_lsbs);
        OutWord((void*)a_matchram_vpn, v_matchram_vpn);
      } else {
        int vpn = -1;
        if (vpn1 >= 0) vpn = vpn1;
        if (vpn0 >= 0) vpn = vpn0;
        if (vpn >= 0) {
          setp_mau_adrmux_row_addrmap_unitram_config_unitram_vpn(&v_unitram_cnf, (uint32_t)(vpn));
          OutWord((void*)a_unitram_cnf, v_unitram_cnf, "unitram_vpn");
        }
      }
    }

    // Now program up row_action_nxtab and match_to_logical_table_ixbar
    if ((log_table >= 0)  &&  ((type == kUnitramTypeMatch) || (type == kUnitramTypeTind))) {
      auto& mm_regs = mau_base.rams.match.merge;
      auto& mm_col_regs = mau_base.rams.match.merge.col[col];

      auto a_row_act_nxtab = &mm_col_regs.row_action_nxtable_bus_drive[row];
      uint32_t v_row_act_nxtab = InWord((void*)a_row_act_nxtab);
      v_row_act_nxtab = 1u << out_bus; // bit0=>bus0 bit1=>bus1
      int inp = (row << 1) | (out_bus & 0x1);
      int xm_tm = (type == kUnitramTypeMatch) ?0 :1;
      auto a_match_to_ltab   = &mm_regs.match_to_logical_table_ixbar_outputmap[xm_tm][inp];
      auto a_match_to_ltab_2 = &mm_regs.match_to_logical_table_ixbar_outputmap[xm_tm+2][inp];
      uint32_t v_m2l   = InWord((void*)a_match_to_ltab);
      uint32_t v_m2l_2 = InWord((void*)a_match_to_ltab_2);
      setp_match_to_logical_table_ixbar_outputmap_enabled_4bit_muxctl_enable(&v_m2l, 0x1);
      setp_match_to_logical_table_ixbar_outputmap_enabled_4bit_muxctl_select(&v_m2l, log_table);
      setp_match_to_logical_table_ixbar_outputmap_enabled_4bit_muxctl_enable(&v_m2l_2, 0x1);
      setp_match_to_logical_table_ixbar_outputmap_enabled_4bit_muxctl_select(&v_m2l_2, log_table);
      OutWord((void*)a_row_act_nxtab, v_row_act_nxtab);
      OutWord((void*)a_match_to_ltab, v_m2l);
      OutWord((void*)a_match_to_ltab_2, v_m2l_2);
    }

    // Now program up tind_xbar
    if ((log_tcam >= 0) && (type == kUnitramTypeTind) && (col < kSramMaxCols/2)) {
      // TODO check: adr_vh_xbar has moved a lot!
      // USED to be in 2 places below but now moved again!
      //auto& vh_xbars = mau_base.rams.map_alu.adr_vh_xbars;
      //auto& vh_xbars = mau_base.rams.xcore.adr_vh_xbars;
      auto& vh_xbars = mau_base.rams.map_alu.row[row].vh_xbars;

      auto a_tind_ctl = &vh_xbars.adr_dist_tind_adr_xbar_ctl[in_bus];
      uint32_t v_ltc = InWord((void*)a_tind_ctl);
      uint32_t v_ltc_en = getp_adr_dist_tind_adr_xbar_ctl_enabled_3bit_muxctl_enable(&v_ltc);
      if (v_ltc_en == 0) {
        // Not yet enabled - in this case configure it just for us - first SRAM wins
        // TODO: row_config to setup this separately from any SRAM config
        setp_adr_dist_tind_adr_xbar_ctl_enabled_3bit_muxctl_enable(&v_ltc, 0x1);
        setp_adr_dist_tind_adr_xbar_ctl_enabled_3bit_muxctl_select(&v_ltc, log_tcam);
        OutWord((void*)a_tind_ctl, v_ltc);
      }
    }
  }

  void TestUtil::meter_config(int pipe, int stage, int row,
                              bool egress,bool red, bool lpf,
                              bool byte,int byte_count_adjust ) {
    auto& mau_base = RegisterUtils::ref_mau(pipe,stage);
    auto& mm_regs = mau_base.rams.match.merge;
    auto& meter_regs = mau_base.rams.map_alu.meter_group[row/2].meter;
    // Setup METER CTL for logrow to use specified format
    uint32_t m_format=0;
    setp_meter_ctl_meter_alu_egress (&m_format, egress?1:0 );
    setp_meter_ctl_red_enable( &m_format, red?1:0 );
    setp_meter_ctl_lpf_enable( &m_format, lpf?1:0 );
    setp_meter_ctl_meter_enable( &m_format, lpf?0:1 );
    setp_meter_ctl_meter_byte( &m_format, byte?1:0 );
    setp_meter_ctl_meter_bytecount_adjust(  &m_format, byte_count_adjust );
    auto a_meter_ctl = &meter_regs.meter_ctl;
    OutWord((void*)a_meter_ctl, m_format);

    // Setup meter_alu_thread - replicated x2
    uint32_t v_meter_alu_thread = InWord((void*)&mm_regs.meter_alu_thread[0]);
    if (egress) {
      v_meter_alu_thread &= ~(1 << ((row/2)+0));
      v_meter_alu_thread |=  (1 << ((row/2)+4));
    } else {
      v_meter_alu_thread |=  (1 << ((row/2)+0));
      v_meter_alu_thread &= ~(1 << ((row/2)+4));
    }
    for (int i = 0; i < 2; i++) {
      OutWord((void*)&mm_regs.meter_alu_thread[i], v_meter_alu_thread);
    }
  }

  void TestUtil::rwram_config(int pipe, int stage, int row, int col,
                              int type, int vpn0, int vpn1,
                              int s_format, int log_table, bool egress,
                              bool use_deferred_rams /*=true*/) {

    // See table in mau-stats-alu for s_format - value in [0..31] for ingress
    // or in [64..95] for egress

    if ((pipe < 0) || (pipe >= kMaxPipes)) return;
    if ((stage < 0) || (stage >= kMaxStages)) return;
    if ((row < 0) || (row >= kSramMaxRows)) return;
    if ((col < 0) || (col >= kSramMaxCols)) return;
    if ((type < 0) || (type > kMaxUnitramType)) return;
    if (s_format < 0) return;
    if (log_table >= kMaxLogTabs) return;

    int left_right = (col < 6) ? 0 : 1;
    int logrow = row + row + left_right;
    int muxcol = col % 6;
    auto& mau_base = RegisterUtils::ref_mau(pipe,stage);
    auto& cfg_regs = mau_base.cfg_regs;
    auto& ram_regs = mau_base.rams.map_alu.row[row].adrmux;
    auto& i2pctl_regs = mau_base.rams.map_alu.row[row].i2portctl;
    auto& m_array_regs = mau_base.rams.array.row[row].ram[col];
    auto& stats_regs = mau_base.rams.map_alu.stats_wrap[row/2].stats;
    auto& meter_regs = mau_base.rams.map_alu.meter_group[row/2].meter;
    auto& selector_regs = mau_base.rams.map_alu.meter_group[row/2].selector;
    auto& stateful_regs = mau_base.rams.map_alu.meter_group[row/2].stateful;
    auto& fabric_regs = mau_base.rams.array.switchbox.row[row].ctl;
    auto& adist_regs = mau_base.rams.match.adrdist;

    if (type == kUnitramTypeStats) {
      assert(log_table >= 0);
      assert(col > 5); // No maprams in 0,1,2,3,4,5 so barf
      uint32_t stats_format = (s_format & 0x1F);
      // LRT thresh in bits[30:4] - but we always set bit4 zero
      uint32_t lrt_thresh = (s_format >> 4) & ~0x1;

      // TODO_1_8 - is there anything that replaces these two registers?
      // Setup MauCfgLtHasStats
      //auto a_cfg_has_stats = &cfg_regs.mau_cfg_lt_has_stats;
      //uint32_t v_cfg_has_stats = InWord((void*)a_cfg_has_stats);
      //OutWord(a_cfg_has_stats, v_cfg_has_stats | (1<<log_table));
      // Setup STATS DUMP CTL for logical table
      auto a_stats_dump_ctl = &cfg_regs.stats_dump_ctl[log_table];
      uint32_t v_stats_dump_ctl = stats_format;
      if ((vpn0 >= 0) && (vpn0 <= 62) && (vpn1 >= 0) && (vpn1 <= 62) && (vpn0 < vpn1))
        v_stats_dump_ctl |= ((vpn0 << 5) | (vpn1 << 11));
      OutWord((void*)a_stats_dump_ctl, v_stats_dump_ctl);

      // Setup STATISTICS CTL for logrow to use specified format
      if (lrt_thresh != 0u) stats_format |= 0x20; // Set LRT enable bit maybe
      if (egress)           stats_format |= 0x40; // Set stats_alu_egress bit
      auto a_stats_ctl = &stats_regs.statistics_ctl;
      OutWord((void*)a_stats_ctl, stats_format);

      // Note Stats LRT code will right-shift by 4 to recreate original [30:4]
      auto a_lrt_thresh = &stats_regs.lrt_threshold[0];
      OutWord((void*)a_lrt_thresh, lrt_thresh);
      // For LRT callback also need to setup mau_cfg_stats_alu_lt
      auto a_cfg_stats_alu_lt = &cfg_regs.mau_cfg_stats_alu_lt[row/2];
      OutWord((void*)a_cfg_stats_alu_lt, log_table);

      // Setup trivial RAM FABRIC to let StatsALU on logrow see SRAM on logrow
      if (left_right == 0) {
        auto a_mux  = &fabric_regs.l_stats_alu_o_mux_select;
        uint32_t v_mux = InWord((void*)a_mux);
        OutWord((void*)a_mux, v_mux|0x10); // l_stats_rd
      } else {
        auto a_mux = &fabric_regs.r_stats_alu_o_mux_select;
        uint32_t v_mux = InWord((void*)a_mux);
        OutWord((void*)a_mux, v_mux|0x20); // r_stats_rd
      }

      // Setup trivial ADDRESS DIST XBAR to push stats address to sram logical row
      auto a_stats_icxbar = &adist_regs.adr_dist_stats_adr_icxbar_ctl[log_table];
      uint32_t v_stats_icxbar = InWord((void*)a_stats_icxbar);
      int alu = (logrow-1)/4;
      OutWord(a_stats_icxbar, v_stats_icxbar | (1<<alu));
      //Before HyperDev was: OutWord(a_stats_icxbar, v_stats_icxbar | (1<<logrow));
      auto a_stats_virt = &adist_regs.mau_ad_stats_virt_lt[alu];
      uint32_t v_stats_virt = InWord((void*)a_stats_virt);
      OutWord(a_stats_virt, v_stats_virt | (1<<log_table)); // Allow virt access

      // Setup maximal mapram_vpn_limit in mapram_ctl
      //assert(left_right == 1);
      //auto a_mapram_ctl = &ram_regs.mapram_ctl[muxcol];
      //uint32_t v_mapram_ctl = InWord((void*)a_mapram_ctl);
      //setp_mapram_ctl_mapram_vpn_limit (&v_mapram_ctl, (uint32_t)0x3F);
      //OutWord((void*)a_mapram_ctl, v_mapram_ctl);
      auto a_synth2port_vpn_ctl = &i2pctl_regs.synth2port_vpn_ctl;
      OutWord((void*)a_synth2port_vpn_ctl, (uint32_t)((0x3F << 6) | (0x0 << 0)));

      if ((logrow == 13) || (logrow == 9) || (logrow == 5) || (logrow == 1)) {
        // Logrow 13,9,5 or 1 so maybe setup DeferredRAMs to allow EOP stats
        auto a_deferred_ram = &adist_regs.deferred_ram_ctl[0][logrow/4];
        auto a_paah = &adist_regs.packet_action_at_headertime[0][logrow/4];

        //uint32_t v_deferred_ram = InWord((void*)a_deferred_ram);
        //OutWord(a_deferred_ram, v_deferred_ram | ((egress) ?0x3 :0x2));
        uint32_t v_deferred_ram = InWord((void*)a_deferred_ram) & ~0x3;
        v_deferred_ram |= use_deferred_rams ? ((egress) ?0x3 :0x2) : 0;
        OutWord(a_deferred_ram, v_deferred_ram);
        OutWord(a_paah, use_deferred_rams ?0 :1);
      }


    } else if ((type == kUnitramTypeMeter) ||
               (type == kUnitramTypeStateful) ||
               (type == kUnitramTypeSelector)) {
      assert(col > 5); // No maprams in 0,1,2,3,4,5 so barf

      if (type == kUnitramTypeMeter) {
        // Setup METER CTL for logrow to use specified format
        uint32_t m_format=0;
        setp_meter_ctl_meter_alu_egress (&m_format, egress?1:0 );
        setp_meter_ctl_red_enable( &m_format, 0 );
        setp_meter_ctl_lpf_enable( &m_format, 0 );
        setp_meter_ctl_meter_enable( &m_format, 1 );
        setp_meter_ctl_meter_byte( &m_format, 0 );
        setp_meter_ctl_meter_bytecount_adjust(  &m_format, 0 );
        auto a_meter_ctl = &meter_regs.meter_ctl;
        OutWord((void*)a_meter_ctl, m_format);
      } else if (type == kUnitramTypeStateful) {
        uint32_t sta_format = s_format;
        setp_stateful_ctl_salu_enable( &sta_format, 1 );
        auto a_stateful_ctl = &stateful_regs.stateful_ctl;
        OutWord((void*)a_stateful_ctl, sta_format);
      } else if (type == kUnitramTypeSelector) {
        uint32_t sel_format = s_format;
        setp_selector_alu_ctl_selector_enable( &sel_format, 1 );
        auto a_selector_ctl = &selector_regs.selector_alu_ctl;
        OutWord((void*)a_selector_ctl, sel_format);
      }

      // Setup trivial RAM FABRIC to let MeterALU on logrow see SRAM on logrow
      if (left_right == 0) {
        auto a_mux  = &fabric_regs.l_meter_alu_o_mux_select;
        uint32_t v_mux = InWord((void*)a_mux);
        OutWord((void*)a_mux, v_mux|0x10); // l_meter_rd
      } else {
        // The meter bus is not used any more, it just uses the stats
        //auto a_mux = &fabric_regs.r_meter_alu_o_mux_select;
        //uint32_t v_mux = InWord((void*)a_mux);
        //OutWord((void*)a_mux, v_mux|0x20); // r_meter_rd
      }


      // Setup trivial ADDRESS DIST XBAR to push meter address to sram logical row
      auto a_meter_icxbar = &adist_regs.adr_dist_meter_adr_icxbar_ctl[log_table];
      uint32_t v_meter_icxbar = InWord((void*)a_meter_icxbar);
      int alu = (logrow-3)/4;
      OutWord(a_meter_icxbar, v_meter_icxbar | (1<<alu));
      // Before HyperDev was: OutWord(a_meter_icxbar, v_meter_icxbar | (1<<logrow));
      auto a_meter_virt = &adist_regs.mau_ad_meter_virt_lt[alu];
      uint32_t v_meter_virt = InWord((void*)a_meter_virt);
      OutWord(a_meter_virt, v_meter_virt | (1<<log_table)); // Allow virt access


      if (type == kUnitramTypeMeter) {
        // In case of meters also setup color_xbar to link LT->ALU
#ifdef MODEL_CHIP_JBAY_OR_LATER // TODO: move somewhere and get rid of ifdef
        auto a_meter_color_icxbar = &adist_regs.meter_color_logical_to_phys_icxbar_ctl[log_table];
        OutWord(a_meter_color_icxbar, (1<<alu));
#else
        auto a_meter_color_ixbar = &adist_regs.meter_color_logical_to_phys_ixbar_ctl[log_table];
        OutWord(a_meter_color_ixbar, (0x4 | alu));
#endif
      }

      // Setup maximal mapram_vpn_limit in mapram_ctl
      //assert(left_right == 1);
      //auto a_mapram_ctl = &ram_regs.mapram_ctl[muxcol];
      //uint32_t v_mapram_ctl = InWord((void*)a_mapram_ctl);
      //setp_mapram_ctl_mapram_vpn_limit (&v_mapram_ctl, (uint32_t)0x3F);
      //OutWord((void*)a_mapram_ctl, v_mapram_ctl);
      auto a_synth2port_vpn_ctl = &i2pctl_regs.synth2port_vpn_ctl;
      OutWord((void*)a_synth2port_vpn_ctl, (uint32_t)((0x3F << 6) | (0x0 << 0)));

      // Setup trivial RAM FABRIC to let ALU on logrow see SRAM on logrow
      if (left_right == 0) {
        auto a_mux  = &fabric_regs.l_stats_alu_o_mux_select;
        uint32_t v_mux = InWord((void*)a_mux);
        OutWord((void*)a_mux, v_mux|0x10); // l_stats_rd
      } else {
        auto a_mux = &fabric_regs.r_stats_alu_o_mux_select;
        uint32_t v_mux = InWord((void*)a_mux);
        OutWord((void*)a_mux, v_mux|0x20); // r_stats_rd
      }

      if (type == kUnitramTypeMeter) {
        if ((logrow == 15) || (logrow == 11) || (logrow == 7) || (logrow == 3)) {
          // Logrow 15,11,7 or 3 so maybe setup DeferredRAMs to allow EOP meters
          auto a_deferred_ram = &adist_regs.deferred_ram_ctl[1][logrow/4];
          auto a_paah = &adist_regs.packet_action_at_headertime[1][logrow/4];

          //uint32_t v_deferred_ram = InWord((void*)a_deferred_ram);
          //OutWord(a_deferred_ram, v_deferred_ram | ((egress) ?0x3 :0x2));
          uint32_t v_deferred_ram = InWord((void*)a_deferred_ram) & ~0x3;
          v_deferred_ram |= use_deferred_rams ? ((egress) ?0x3 :0x2) : 0;
          OutWord(a_deferred_ram, v_deferred_ram);
          OutWord(a_paah, use_deferred_rams ?0 :1);
        }
      }
      if ((type == kUnitramTypeSelector) || (type == kUnitramTypeStateful)) {
        if ((logrow == 15) || (logrow == 11) || (logrow == 7) || (logrow == 3)) {
          // Make sure p_a_a_h set to HDR time
          OutWord(&adist_regs.packet_action_at_headertime[1][logrow/4], 1);
        }
      }
    }


    // Setup RAM ADDR MUX and UNIT RAM CTL
    auto a_ramaddr_mux = &ram_regs.ram_address_mux_ctl[left_right][muxcol];
    auto a_unitram_ctl = &m_array_regs.unit_ram_ctl;
    uint32_t v_unitram_ctl = InWord((void*)a_unitram_ctl);
    uint32_t v_ramaddr_mux = InWord((void*)a_ramaddr_mux);
    if (type == kUnitramTypeStats) {
      setp_mau_adrmux_row_addrmap_ram_address_mux_ctl_ram_unitram_adr_mux_select (&v_ramaddr_mux, (uint32_t)0x5);
      setp_mau_adrmux_row_addrmap_ram_address_mux_ctl_ram_ofo_stats_mux_select_statsmeter (&v_ramaddr_mux, (uint32_t)0x1);
      setp_mau_adrmux_row_addrmap_ram_address_mux_ctl_ram_stats_meter_adr_mux_select_stats (&v_ramaddr_mux, (uint32_t)0x1);
      setp_mau_adrmux_row_addrmap_ram_address_mux_ctl_map_ram_wadr_mux_enable (&v_ramaddr_mux, (uint32_t)0x1);
      setp_mau_adrmux_row_addrmap_ram_address_mux_ctl_map_ram_wadr_mux_select (&v_ramaddr_mux, (uint32_t)0x1);
      setp_mau_adrmux_row_addrmap_ram_address_mux_ctl_synth2port_radr_mux_select_home_row (&v_ramaddr_mux, (uint32_t)0x1);
    } else if ((type == kUnitramTypeMeter) ||
               (type == kUnitramTypeStateful) ||
               (type == kUnitramTypeSelector)) {
      setp_mau_adrmux_row_addrmap_ram_address_mux_ctl_ram_unitram_adr_mux_select (&v_ramaddr_mux, (uint32_t)0x5);
      setp_mau_adrmux_row_addrmap_ram_address_mux_ctl_ram_ofo_stats_mux_select_statsmeter (&v_ramaddr_mux, (uint32_t)0x1);
      setp_mau_adrmux_row_addrmap_ram_address_mux_ctl_ram_stats_meter_adr_mux_select_meter (&v_ramaddr_mux, (uint32_t)0x1);
      setp_mau_adrmux_row_addrmap_ram_address_mux_ctl_map_ram_wadr_mux_enable (&v_ramaddr_mux, (uint32_t)0x1);
      setp_mau_adrmux_row_addrmap_ram_address_mux_ctl_map_ram_wadr_mux_select (&v_ramaddr_mux, (uint32_t)0x1);
      setp_mau_adrmux_row_addrmap_ram_address_mux_ctl_synth2port_radr_mux_select_home_row(&v_ramaddr_mux, (uint32_t)0x1);
    }
    OutWord((void*)a_ramaddr_mux, v_ramaddr_mux);
    OutWord((void*)a_unitram_ctl, v_unitram_ctl, "unitram_ctl");

    // Set UNITRAM CONFIG to say SRAM type and possibly stats/meter table
    auto a_unitram_cnf = &ram_regs.unitram_config[left_right][muxcol];
    uint32_t v_unitram_cnf = InWord((void*)a_unitram_cnf);

    setp_mau_adrmux_row_addrmap_unitram_config_unitram_type(&v_unitram_cnf, (uint32_t)type);
    setp_mau_adrmux_row_addrmap_unitram_config_unitram_enable(&v_unitram_cnf, (uint32_t)0x1);
    if (egress) {
      setp_mau_adrmux_row_addrmap_unitram_config_unitram_egress(&v_unitram_cnf, (uint32_t)0x1);
    } else {
      setp_mau_adrmux_row_addrmap_unitram_config_unitram_ingress(&v_unitram_cnf, (uint32_t)0x1);
    }
    if (log_table >= 0) {
      setp_mau_adrmux_row_addrmap_unitram_config_unitram_logical_table(&v_unitram_cnf, (uint32_t)log_table);
    }
    OutWord((void*)a_unitram_cnf, v_unitram_cnf);

    // If setting up Stats/Meter/Stateful/Selector SRAM also set
    // MAPRAM CONFIG to have matching type etc.
    uint8_t mapram_type = 0;
    switch (type) {
      case kUnitramTypeStats:    mapram_type = kMapramTypeStats;    break;
      case kUnitramTypeMeter:    mapram_type = kMapramTypeMeter;    break;
      case kUnitramTypeStateful: mapram_type = kMapramTypeStateful; break;
      case kUnitramTypeSelector: mapram_type = kMapramTypeSelector; break;
    }
    if (mapram_type != 0) {
      assert(col > 5); // No maprams in 0,1,2,3,4,5 so barf
      assert(left_right == 1);
      auto a_mapram_cnf = &ram_regs.mapram_config[muxcol];
      uint32_t v_mapram_cnf = InWord((void*)a_mapram_cnf);

      setp_mapram_config_mapram_type(&v_mapram_cnf, (uint32_t)mapram_type);
      setp_mapram_config_mapram_enable(&v_mapram_cnf, (uint32_t)0x1);
      if (egress) {
        setp_mapram_config_mapram_egress(&v_mapram_cnf, (uint32_t)0x1);
      } else {
        setp_mapram_config_mapram_ingress(&v_mapram_cnf, (uint32_t)0x1);
      }
      if (log_table >= 0) {
        setp_mapram_config_mapram_logical_table(&v_mapram_cnf, (uint32_t)log_table);
      }
      OutWord((void*)a_mapram_cnf, v_mapram_cnf);

      // setup the hbus members for synth2port
      auto a_synth2port_hbus_members = &i2pctl_regs.synth2port_hbus_members[0][1];
      uint32_t v_hbus_members = InWord((void*)a_synth2port_hbus_members);
      OutWord(a_synth2port_hbus_members, v_hbus_members | (1<<(col-6)));

      // setup synth2port_ctl.synth2port_enable=1 given we're using synth2port
      auto a_synth2port_ctl = &i2pctl_regs.synth2port_ctl;
      uint32_t v_synth2port_ctl = InWord((void*)a_synth2port_ctl);
      OutWord(a_synth2port_ctl, v_synth2port_ctl | (1u<<6)); // Set synth2port_enable
    }


    if (type == kUnitramTypeMeter) {
      // Set UNITRAM VPN to use specified VPN vals
      if ((vpn0 >= 0) || (vpn1 >= 0)) {
        int vpn = -1;
        if (vpn1 >= 0) vpn = vpn1;
        if (vpn0 >= 0) vpn = vpn0;
        if (vpn >= 0) {
          setp_mau_adrmux_row_addrmap_unitram_config_unitram_vpn(&v_unitram_cnf, (uint32_t)(vpn));
          OutWord((void*)a_unitram_cnf, v_unitram_cnf, "unitram_vpn");
        }
      }
    }
  }

  void TestUtil::mux_config(int pipe, int stage, int row, int col,
                            int mux_inp, int rd_bus, int wr_bus, int log_table,
                            bool oflo, int src_idx, int src_sel,
                            bool oflo2) {

    if ((pipe < 0) || (pipe >= kMaxPipes)) return;
    if ((stage < 0) || (stage >= kMaxStages)) return;
    if ((row < 0) || (row >= kSramMaxRows)) return;
    if ((col < 0) || (col >= kSramMaxCols)) return;
    if ((rd_bus < -1) || (rd_bus > 4)) return;
    if ((wr_bus < -1) || (wr_bus > 3)) return;
    if (log_table >= kMaxLogTabs) return;

    auto& mau_base = RegisterUtils::ref_mau(pipe,stage);
    auto& ram_regs = mau_base.rams.map_alu.row[row].adrmux;
    auto& m_array_regs = mau_base.rams.array.row[row].ram[col];
    auto& vh_xbars = mau_base.rams.map_alu.row[row].vh_xbars;
    auto& adist_regs = mau_base.rams.match.adrdist;
    auto& fabric_regs = mau_base.rams.array.switchbox.row[row].ctl;

    // Setup RAM ADDR MUX
    int left_right = (col < 6) ? 0 : 1;
    int logrow = row + row + left_right;
    int muxcol = col % 6;
    auto a_ramaddr_mux = &ram_regs.ram_address_mux_ctl[left_right][muxcol];
    auto a_unitram_ctl = &m_array_regs.unit_ram_ctl;
    uint32_t v_unitram_ctl = InWord((void*)a_unitram_ctl);
    uint32_t v_mux = InWord((void*)a_ramaddr_mux);

    if (log_table >= 0) {
      // Setup trivial ADDRESS DIST XBAR to push ACTION addresses
      // to given sram logical row and to ALL oflow buses
      auto a_action_icxbar = &adist_regs.adr_dist_action_data_adr_icxbar_ctl[log_table];
      uint32_t v_action_icxbar = InWord((void*)a_action_icxbar);
      v_action_icxbar |= 0x70000;         // Push to ALL oflow buses
      v_action_icxbar |= (1<<logrow);     // Push to logical row of sram/mux
      //if (oflo || oflo2) v_action_icxbar |= (1<<(logrow+1)); // Push to logical row above
      OutWord(a_action_icxbar, v_action_icxbar);

      // Setup trivial RAM FABRIC to push ACTION DATA L->R
      // Configure r_action r_l_action appropriately based on
      // left/right and rd_bus we're using
      auto a_r_mux = &fabric_regs.r_action_o_mux_select;
      auto a_rl_mux = &fabric_regs.r_l_action_o_mux_select;
      uint32_t v_r_mux = InWord((void*)a_r_mux);
      uint32_t v_rl_mux = InWord((void*)a_rl_mux);
      v_r_mux |= 0x1000;
      v_rl_mux |= 0x1000;
      if (left_right == 0) {
        // LHS MUX
        if (rd_bus == 2) v_r_mux  |= 0x20; // Oflo
        if (rd_bus == 3) v_r_mux  |= 0x10; // Oflo2
        if (rd_bus == 4) v_rl_mux |= 0x10; // Action
      } else {
        // RHS MUX
        if (rd_bus == 3) v_rl_mux |= 0x80; // Oflo2
        if (rd_bus == 4) v_r_mux  |= 0x01; // Action
      }
      OutWord(a_r_mux, v_r_mux);
      OutWord(a_rl_mux, v_rl_mux);
    }

    // mux_inp == ACTION(1) OFLO(4) DELAYED(5)
    setp_mau_adrmux_row_addrmap_ram_address_mux_ctl_ram_unitram_adr_mux_select(&v_mux,
                                                                               (uint32_t)mux_inp);
    if (((mux_inp == 4) || (mux_inp == 5)) && ((oflo) || (oflo2))) {
      // Selecting oflo(4) or delayed input(5) so program other muxes too
      if (oflo) {
        setp_mau_adrmux_row_addrmap_ram_address_mux_ctl_ram_oflo_adr_mux_select_oflo(&v_mux, 1u);
      } else if (oflo2) {
        setp_mau_adrmux_row_addrmap_ram_address_mux_ctl_ram_oflo_adr_mux_select_oflo2(&v_mux, 1u);
      }
      if (mux_inp == 5) {
        setp_mau_adrmux_row_addrmap_ram_address_mux_ctl_ram_ofo_stats_mux_select_oflo(&v_mux, 1u);
      }
      if (oflo) {
        // And program row
        auto a_rowdist = &vh_xbars.adr_dist_oflo_adr_xbar_ctl[left_right];
        uint32_t v_row = InWord((void*)a_rowdist);
        setp_adr_dist_oflo_adr_xbar_ctl_adr_dist_oflo_adr_xbar_enable(&v_row, 1u);
        setp_adr_dist_oflo_adr_xbar_ctl_adr_dist_oflo_adr_xbar_source_sel(&v_row, src_sel);
        setp_adr_dist_oflo_adr_xbar_ctl_adr_dist_oflo_adr_xbar_source_index(&v_row, src_idx);
        OutWord((void*)a_rowdist, v_row);
      } else if (oflo2) {
        printf("oflo2 mux setup NOT YET SUPPORTED\n");
      }
    }
    OutWord((void*)a_ramaddr_mux, v_mux);

    // Setup rd/wr
    if (wr_bus < 0) wr_bus = 7;
    setp_unit_ram_ctl_match_ram_write_data_mux_select(&v_unitram_ctl, (uint32_t)wr_bus);
    if (rd_bus < 0) rd_bus = 7;
    setp_unit_ram_ctl_match_ram_read_data_mux_select(&v_unitram_ctl, (uint32_t)rd_bus);
    OutWord((void*)a_unitram_ctl, v_unitram_ctl);

  }



  void TestUtil::mapram_config(int pipe, int stage, int row, int col,
                               int type, int vpn0, int vpn1,
                               int m_flags, int log_table, bool egress) {

    if ((pipe < 0) || (pipe >= kMaxPipes)) return;
    if ((stage < 0) || (stage >= kMaxStages)) return;
    if ((row < 0) || (row >= kSramMaxRows)) return;
    if ((col < 0) || (col >= kSramMaxCols)) return;
    if ((type < 0) || (type >= kMaxMapramType)) return;
    if (log_table >= kMaxLogTabs) return;
    assert(col >= kSramMaxCols/2); // Must be on RHS
    uint32_t idlebus = idle_flags_idlebus(m_flags);
    if (row < kSramMaxRows/2)
      assert((idlebus <= 7) || (idlebus == 8));
    else
      assert( ((idlebus >= 10) && (idlebus <= 17))  ||  (idlebus == 8) );

    auto& mau_base = RegisterUtils::ref_mau(pipe,stage);
    auto& cfg_regs = mau_base.cfg_regs;
    auto& adist_regs = mau_base.rams.match.adrdist;
    auto& alu_row_regs = mau_base.rams.map_alu.row[row];
    auto& i2pctl_regs = mau_base.rams.map_alu.row[row].i2portctl;
    int left_right = (col < 6) ? 0 : 1;
    int rhscol = col % 6;
    auto a_idle_oxbar = &adist_regs.adr_dist_idletime_adr_oxbar_ctl[idlebus/4];
    auto a_idletime_dump_ctl = &cfg_regs.idle_dump_ctl[log_table];
    auto a_idletime_sweep_ctl = &adist_regs.idletime_sweep_ctl[log_table];
    auto a_ramaddr_mux = &alu_row_regs.adrmux.ram_address_mux_ctl[left_right][rhscol];
    auto a_mapram_cnf = &alu_row_regs.adrmux.mapram_config[rhscol];
    auto a_idle_ctl = &alu_row_regs.vh_xbars.adr_dist_idletime_adr_xbar_ctl[rhscol];
    auto a_synth2port_ctl = &i2pctl_regs.synth2port_ctl;

    uint32_t v_ramaddr_mux = InWord((void*)a_ramaddr_mux);
    uint32_t v_mapram_cnf = InWord((void*)a_mapram_cnf);
    uint32_t v_idle_ctl = InWord((void*)a_idle_ctl);

    // ONLY does idletime maprams and color maprams at moment!!
    //  this can only program color maprams to get their read address from
    //   the idletime bus (the hardware can use the stats bus too)
    if (type == kMapramTypeIdletime || type == kMapramTypeColor ) {
      setp_mapram_config_mapram_type(&v_mapram_cnf, (uint32_t)type);
      // Set RAM ADDR MUX to take idletime input
      if ( type == kMapramTypeIdletime ) {
        setp_mau_adrmux_row_addrmap_ram_address_mux_ctl_map_ram_radr_mux_select_smoflo(
            &v_ramaddr_mux, (uint32_t)0x1);
        // Setup for write???
      }
      else if ( type == kMapramTypeColor ) {
        setp_mau_adrmux_row_addrmap_ram_address_mux_ctl_map_ram_radr_mux_select_color(
            &v_ramaddr_mux, (uint32_t)0x1);
        setp_mapram_config_mapram_color_bus_select(&v_mapram_cnf, 1 /*color bus*/);

        // Setup for write too - tell synth2port to use home row addr (ie meter_rd_addr)
        setp_mau_adrmux_row_addrmap_ram_address_mux_ctl_synth2port_radr_mux_select_home_row (&v_ramaddr_mux, (uint32_t)0x1);
        setp_mau_adrmux_row_addrmap_ram_address_mux_ctl_map_ram_wadr_mux_enable (&v_ramaddr_mux, (uint32_t)0x1);
        setp_mau_adrmux_row_addrmap_ram_address_mux_ctl_map_ram_wadr_mux_select (&v_ramaddr_mux, (uint32_t)0x3);

        // Don't need to set synth2port_enable but DO need to set bit
        // for mapram column in synth2port_ctl.synth2port_mapram_color
        uint32_t v_synth2port_ctl = InWord((void*)a_synth2port_ctl);
        v_synth2port_ctl |= (1u << (col - (kSramMaxCols/2))); // Col6->Bit0 Col7->Bit1)
        OutWord((void*)a_synth2port_ctl, v_synth2port_ctl);
      }
      else {
        assert(0);
      }
      setp_mau_adrmux_row_addrmap_ram_address_mux_ctl_ram_ofo_stats_mux_select_statsmeter(
          &v_ramaddr_mux, (uint32_t)0x1);
      setp_mau_adrmux_row_addrmap_ram_address_mux_ctl_ram_stats_meter_adr_mux_select_idlet(
          &v_ramaddr_mux, (uint32_t)0x1);

      // Set per-row idletime address XBAR (if not yet enabled)
      if (getp_adr_dist_idletime_adr_xbar_ctl_enabled_4bit_muxctl_enable(&v_idle_ctl) == 0) {
        setp_adr_dist_idletime_adr_xbar_ctl_enabled_4bit_muxctl_enable(&v_idle_ctl, 0x1);
        setp_adr_dist_idletime_adr_xbar_ctl_enabled_4bit_muxctl_select(&v_idle_ctl,
                                                                       (idlebus % 10));
        OutWord((void*)a_idle_ctl, v_idle_ctl);
      }
    }
    if (type == kMapramTypeIdletime) {
      uint32_t idleflags = idle_flags_flags(m_flags);
      uint32_t bitwidth = idle_flags_bitwidth(m_flags);
      //uint32_t interval = idle_flags_interval(m_flags);

      // Set MAPRAM config based on idletime flags
      if ((idleflags & kIdleFlagDisableNotify) != 0)
        setp_mapram_config_idletime_disable_notification(&v_mapram_cnf, 0x1);
      if ((idleflags & kIdleFlagTwoWay) != 0)
        setp_mapram_config_two_way_idletime_notification(&v_mapram_cnf, 0x1);
      if ((idleflags & kIdleFlagPerFlow) != 0)
        setp_mapram_config_per_flow_idletime(&v_mapram_cnf, 0x1);

      setp_mapram_config_idletime_bitwidth(&v_mapram_cnf, bitwidth);

    }

    // GENERIC mapram config
    // Set MAPRAM CONFIG to say logical table/VPN/ingress etc
    uint32_t min_vpn = 0u;
    uint32_t max_vpn = 0u;
    if (vpn0 >= 0) {
      min_vpn = vpn0;
      max_vpn = vpn0;
      if (vpn1 >= 0) assert(vpn1 == vpn0);
      setp_mapram_config_mapram_vpn(&v_mapram_cnf, vpn0);
      // mapram_vpn_members no longer exists! 23/4/15
      //setp_mapram_config_mapram_vpn(&v_mapram_cnf, (vpn0 >> 2));
      //uint32_t vpn_members = (1u << (vpn0 & 0x3));
      //if ((vpn1 >= 0) && ((vpn0 >> 2) == (vpn1 >> 2))) {
      //  vpn_members |= (1u << (vpn1 & 0x3));
      //  if (vpn1 > vpn0) max_vpn = vpn1;
      //}
      //setp_mapram_config_mapram_vpn_members(&v_mapram_cnf, vpn_members);
    }
    uint32_t v_idox_in = 0u, v_idox_out = 0u;
    uint32_t v_isc_in = 0u, v_isc_out = 0u;
    uint32_t v_idc_in = 0u, v_idc_out = 0u;
    if (log_table >= 0) {
      setp_mapram_config_mapram_logical_table(&v_mapram_cnf, log_table);

      if (type == kMapramTypeIdletime || type == kMapramTypeColor ) {
        // If we have logical table program up idletime oxbar
        v_idox_in = InWord((void*)a_idle_oxbar);
        v_idox_out = v_idox_in;
        v_idox_out &= ~(0x1F << (5*(idlebus%4))); // Zeroise (disable) entry for ibus
        v_idox_out |= ((0x10 | log_table) << (5*(idlebus%4))); // Re-enable for log_table

      }
      if (type == kMapramTypeIdletime) {
        // TODO_1_8 - is there anything that replaces this register?
        // Setup up MauCfgLtHasIdle
        //auto a_cfg_has_idle = &cfg_regs.mau_cfg_lt_has_idle;
        //uint32_t v_cfg_has_idle = InWord((void*)a_cfg_has_idle);
        //OutWord(a_cfg_has_idle, v_cfg_has_idle | (1<<log_table));

        // If we have logical table program up sweep in address distribution
        v_isc_in = InWord((void*)a_idletime_sweep_ctl);
        v_isc_out = v_isc_in;
        // And dump at top-level
        v_idc_in = InWord((void*)a_idletime_dump_ctl);
        v_idc_out = v_idc_in;
        uint32_t interval = idle_flags_interval(m_flags);
        uint32_t curr_sweep_min_vpn, curr_sweep_max_vpn, curr_interval;
        bool enabled;
        if (getp_idletime_sweep_ctl_idletime_sweep_en(&v_isc_out) == 1) {
          // Already enabled - find curr vals max_vpn, interval
          enabled = true;
          curr_sweep_min_vpn = getp_idletime_sweep_ctl_idletime_sweep_offset(&v_isc_out);
          curr_sweep_max_vpn = getp_idletime_sweep_ctl_idletime_sweep_size(&v_isc_out);
          curr_interval = getp_idletime_sweep_ctl_idletime_sweep_interval(&v_isc_out);
        } else {
          // Not yet enabled so enable now
          enabled = false;
          curr_sweep_min_vpn = 99; curr_sweep_max_vpn = 99;
          setp_idletime_sweep_ctl_idletime_sweep_en(&v_isc_out, 0x1);
        }
        // For sweep/dump, keep MIN min_vpn, MAX max_vpn, MIN interval
        if ((curr_sweep_min_vpn > min_vpn) || (!enabled)) {
          setp_idletime_sweep_ctl_idletime_sweep_offset(&v_isc_out, min_vpn);
          setp_idle_dump_ctl_idletime_dump_offset(&v_idc_out, min_vpn);
          //printf("mapram_config[%d,%d,%d,%d]: Logtab=%d MinVPN=%d [%d,%d]\n"
          //pipe, stage, row, col, log_table, min_vpn, curr_sweep_min_vpn, enabled);
        }
        if ((curr_sweep_max_vpn < max_vpn) || (!enabled)) {
          setp_idletime_sweep_ctl_idletime_sweep_size(&v_isc_out, max_vpn);
          setp_idle_dump_ctl_idletime_dump_size(&v_idc_out, max_vpn);
          //printf("mapram_config[%d,%d,%d,%d]: Logtab=%d MaxVPN=%d [%d,%d]\n",
          //pipe, stage, row, col, log_table, max_vpn, curr_sweep_max_vpn, enabled);
        }
        if ((curr_interval > interval) || (!enabled)) {
          setp_idletime_sweep_ctl_idletime_sweep_interval(&v_isc_out, interval);
        }
      }
      // program the mapping from logical table to meter index for color
      if (type == kMapramTypeColor) {
        auto& mm_regs = mau_base.rams.match.merge;
        bool jbay = RmtObject::is_jbay_or_later();
        int log_row = (row*2) + 1; // meter alus always on right, so odd logical row
        int bus = MauMeterAlu::get_meter_alu_regs_index(log_row);

        // On JBay index by bus, otherwise use 0,1 if LTs0-7,8-15
        int reg = (jbay) ?bus :(log_table/8);
        auto& cmtlc = mm_regs.mau_mapram_color_map_to_logical_ctl[reg];
        uint32_t v = InWord( &cmtlc );
        if (jbay) {
          v = 1<<log_table; // overwrite with LT
        } else {
          int shift = (log_table % 8)*3;
          v &= ~( 0x7<<shift ); // clear our field
          v |= 0x4 << shift; // set enable
          v |= (bus & 0x3) << shift; // set bus
        }
        OutWord( &cmtlc, v );

        uint32_t mux_cfg = 0;
        setp_r_color0_mux_select_r_color0_sel_color_r_i (&mux_cfg, 1);
        auto& mux_reg = mau_base.rams.map_alu.mapram_color_switchbox.row[row].ctl.r_color0_mux_select;
        OutWord( &mux_reg, mux_cfg );
      }
    }
    if (egress) {
      setp_mapram_config_mapram_egress(&v_mapram_cnf, 0x1);
    } else {
      setp_mapram_config_mapram_ingress(&v_mapram_cnf, 0x1);
    }

    OutWord((void*)a_ramaddr_mux, v_ramaddr_mux);
    OutWord((void*)a_mapram_cnf, v_mapram_cnf);
    if (v_idox_in != v_idox_out) {
      OutWord((void*)a_idle_oxbar, v_idox_out);
      //printf("Mapram[%d,%d,%d,%d] Logtab=%d iBus=%d idoxIn=0x%08x idoxOut=0x%08x\n",
      //pipe, stage, row, col, log_table, idlebus, v_idox_in, v_idox_out);
    }
    if (v_isc_in != v_isc_out) {
      OutWord((void*)a_idletime_sweep_ctl, v_isc_out);
    }
    if (v_idc_in != v_idc_out) {
      OutWord((void*)a_idletime_dump_ctl, v_idc_out);
    }
  }




  void TestUtil::tcam_config(int pipe, int stage, int row, int col,
                             int in_bus, int log_table, int log_tcam, int vpn,
                             bool ingress, bool chain, bool output, int head) {
    if (get_debug()) printf("TestUtil<%d,%d>::tcam_config(%d,%d)\n",
                            pipe, stage, row, col);

    if ((pipe < 0) || (pipe >= kMaxPipes)) return;
    if ((stage < 0) || (stage >= kMaxStages)) return;
    if ((row < 0) || (row >= kTcamMaxRows)) return;
    if ((col < 0) || (col >= kTcamMaxCols)) return;
    if ((in_bus < 0) || (in_bus > 1)) return;
    if ((head < 0) || (head >= kTcamMaxEntries)) return;
    if (log_tcam >= kMaxLogTcams) return;

    auto& mau_base = RegisterUtils::ref_mau(pipe,stage);
    auto& tcam_regs = mau_base.tcams.col[col];
    auto a_mode = &tcam_regs.tcam_mode[row];
    uint32_t v_mode = InWord((void*)a_mode);
    //Removed in bfnregs 20150107_182406_7982_mau_dev
    //auto a_tailptr = &tcam_regs.tcam_priority_tailptr[row];
    //uint32_t v_tailptr = InWord((void*)a_tailptr, "tcam_pri_tailptr");

    if (log_table >= 0) {
      setp_tcam_mode_tcam_logical_table(&v_mode, log_table);
    }
    // This is actually used to indicate TCAM priority
    setp_tcam_mode_tcam_vpn(&v_mode, vpn);
    setp_tcam_mode_tcam_match_output_enable(&v_mode, (output) ?1 :0);
    setp_tcam_mode_tcam_ingress(&v_mode, (ingress) ?1 :0);
    setp_tcam_mode_tcam_egress(&v_mode, (ingress) ?0 :1);
    setp_tcam_mode_tcam_chain_out_enable(&v_mode, (chain) ?1 :0);
    setp_tcam_mode_tcam_data1_select(&v_mode, in_bus);
    // Don't setup any DirtCAM modes for now
    setp_tcam_mode_tcam_vbit_dirtcam_mode(&v_mode, 0);
    setp_tcam_mode_tcam_data_dirtcam_mode(&v_mode, 0);

    OutWord((void*)a_mode, v_mode);
    //OutWord((void*)a_tailptr, head);

    // Set this TCAM as active for the input logicalTCAM (if output)
    // and inactive for any other logicalTCAM
#if MCN_TEST(MODEL_CHIP_NAMESPACE,rsvd0)
    auto a_tcam_map_oxbar_ctl = &tcam_regs.tcam_map_oxbar_ctl[row];
    uint32_t v_tcam_map_oxbar_ctl = InWord((void*)a_tcam_map_oxbar_ctl);
    uint32_t all_ltc_enables = 0u; // Work out LTCAM enable mask
    for (int ltc = 0; ltc < kMaxLogTcams; ltc++) all_ltc_enables |= (1u<<((ltc*3)+2));
    v_tcam_map_oxbar_ctl &= ~all_ltc_enables; // Clear enables; leave result selects
    if (output) v_tcam_map_oxbar_ctl |= (1u<<((log_tcam*3)+2)); // Maybe enable ours
    OutWord((void*)a_tcam_map_oxbar_ctl, v_tcam_map_oxbar_ctl);
#else
    for (int ltc = 0; ltc < kMaxLogTcams; ltc++) {
      auto a_tablemap = &tcam_regs.tcam_table_map[ltc];
      uint32_t v_tablemap = InWord((void*)a_tablemap);
      if ((ltc == log_tcam) && (output)) {
        v_tablemap |= (1 << row);
        OutWord((void*)a_tablemap, v_tablemap);
      } else if ((v_tablemap & (1 << row)) != 0) {
        v_tablemap &= ~(1 << row);
        OutWord((void*)a_tablemap, v_tablemap);
      }
    }
#endif
  }



  void TestUtil::table_cntr_config(int pipe, int stage, int log_table, int cnt_what) {
    if ((pipe < 0) || (pipe >= kMaxPipes)) return;
    if ((stage < 0) || (stage >= kMaxStages)) return;
    if ((log_table < 0) || (log_table >= kMaxLogTabs)) return;
    if ((cnt_what < 0) || (cnt_what > 7)) return;

    auto& mau_base = RegisterUtils::ref_mau(pipe,stage);
    //auto& cfg_regs = mau_base.cfg_regs;
    //auto& adist_regs = mau_base.rams.match.adrdist;
    auto& mm_regs = mau_base.rams.match.merge;

    // Setup ctrl reg to specify what we're counting
    // (Note table need not be predicated ON)
    // cnt_what 0   => Disabled
    // cnt_what 1   => Count TableMiss
    // cnt_what 2   => Count TableHit
    // cnt_what 3   => Count GW Miss
    // cnt_what 4   => Count GW Hit
    // cnt_what 5   => Count GW Inhibit
    // cnt_what 6-7 => Reserved
    auto a_tablectr_ctl = &mm_regs.mau_table_counter_ctl[log_table / 8];
    uint32_t v_tablectr_ctl = InWord((void*)a_tablectr_ctl);
    v_tablectr_ctl &= ~(0x7 << ((log_table % 8) * 3));            // Clear
    v_tablectr_ctl |=  (cnt_what & 0x7) << ((log_table % 8) * 3); // Set
    OutWord((void*)a_tablectr_ctl, v_tablectr_ctl);
  }




  Packet *TestUtil::packet_make(const char *da, const char *sa,
                                const char *sip, const char *dip,
                                int proto, int sport, int dport,
                                const char *payload) {
    RmtObjectManager *om = get_objmgr();
    if (om == NULL) return NULL;
    if (get_debug()) printf("TestUtil::packet_make(%s %s)\n", da, sa);

    // Create an Ethernet header
    Crafter::Ethernet eth_header;
    eth_header.SetDestinationMAC(da);
    eth_header.SetSourceMAC(sa);
    // Create an IP header
    Crafter::IP ip_header;
    //ip_header.SetSourceIP(std::string("10.17.34.51"));
    //ip_header.SetDestinationIP(std::string("10.68.85.102"));
    ip_header.SetSourceIP(sip);
    ip_header.SetDestinationIP(dip);
    // Now TCP or UDP headers
    Crafter::TCP tcp_header;
    Crafter::UDP udp_header;
    if (proto == kProtoTCP) {
      // Create a TCP header
      tcp_header.SetSrcPort(sport);
      tcp_header.SetDstPort(dport);
    } else if (proto == kProtoUDP) {
      // Create a UDP header
      udp_header.SetSrcPort(sport);
      udp_header.SetDstPort(dport);
    } else {
    }
    // The body - this could be any array of bytes or chars
    Crafter::RawLayer body((payload != NULL) ?payload :"EMPTY");

    // Create a packet
    Crafter::Packet the_packet;
    if (proto == kProtoTCP) {
      the_packet = eth_header / ip_header / tcp_header / body;
    } else if (proto == kProtoUDP) {
      the_packet = eth_header / ip_header / udp_header / body;
    } else {
      the_packet = eth_header / ip_header / body;
    }
    // Now create a RMT flavour packet
    uint8_t pkt_data[9999];
    size_t  pkt_len = the_packet.GetData(pkt_data);
    Packet *p = om->pkt_create(pkt_data, pkt_len);
    if (get_debug()) printf("TestUtil::packet_make(%s %s) = %p\n", da, sa, p);
    return p;
  }

  Packet *TestUtil::packet_make(uint64_t seed, uint16_t len) {
    constexpr int pkt_buf_sz = 16384;
    RmtObjectManager *om = get_objmgr();
    if ((om == NULL) || (len > pkt_buf_sz)) return NULL;
    if (get_debug()) printf("TestUtil::packet_make(%016" PRIx64 " %d)\n", seed, len);
    uint8_t  pkt_buf[pkt_buf_sz];
    uint64_t w64 = seed;
    uint16_t pos = 0;
    while (pos < len) {
      pos = model_common::Util::fill_buf(pkt_buf, pkt_buf_sz, pos, 8, w64);
      w64 = mmix_rand64(w64);
    }
    Packet *p = om->pkt_create(pkt_buf, len);
    if (get_debug()) printf("TestUtil::packet_make(%016" PRIx64 " %d) = %p\n",
                            seed, len, p);
    return p;
  }

  Packet *TestUtil::packet_make(uint64_t seed, uint16_t pktlen,
                                uint8_t *hdr, uint16_t hdrlen, uint16_t tailzeros) {
    constexpr int pkt_buf_sz = 16384;
    RmtObjectManager *om = get_objmgr();
    if ((om == NULL) || (pktlen > pkt_buf_sz) || ((hdrlen + tailzeros) > pktlen)) return NULL;
    if (get_debug()) printf("TestUtil::packet_make(%016" PRIx64 " %d)\n", seed, pktlen);
    uint8_t pkt_buf[pkt_buf_sz];
    if (hdrlen > 0)
      (void)std::memcpy((void*)pkt_buf, (void*)hdr, hdrlen);
    uint64_t w64 = seed;
    uint16_t pos = hdrlen;
    while (pos < pktlen) {
      pos = model_common::Util::fill_buf(pkt_buf, pkt_buf_sz, pos, 8, w64);
      w64 = mmix_rand64(w64);
    }
    if (tailzeros > 0) for (int i = 0; i < tailzeros; i++) pkt_buf[pktlen-tailzeros+i] = 0;
    Packet *p = om->pkt_create(pkt_buf, pktlen);
    if (get_debug()) printf("TestUtil::packet_make(%016" PRIx64 " %d[%d]<%d>) = %p\n",
                            seed, pktlen, hdrlen, tailzeros, p);
    return p;
  }

  void TestUtil::packet_free(Packet *p) {
    if (p == NULL) return;
    RmtObjectManager *om = get_objmgr();
    if (om == NULL) return;
    //if (om->pkt_last_deleted() == p) return;
    om->pkt_delete(p);
  }
  void TestUtil::packet_free(Packet *pin, Packet *pout) {
    if ((pin == NULL) || (pout == NULL)) return;
    RmtObjectManager *om = get_objmgr();
    if (om == NULL) return;
    if (pin != pout) om->pkt_delete(pout);
    om->pkt_delete(pin);
  }

  Phv *TestUtil::phv_alloc() {
    RmtObjectManager *om = get_objmgr();
    if (om == NULL) return NULL;
    return om->phv_create();
  }

  void TestUtil::phv_free(Phv *phv) {
    if (phv == NULL) return;
    RmtObjectManager *om = get_objmgr();
    if (om == NULL) return;
    om->phv_delete(phv);
  }

  Port *TestUtil::port_get(int portIndex) {
    if ((portIndex < 0) || (portIndex > 999)) return NULL;
    if (get_debug()) printf("TestUtil<port=%d>::port_get().....\n", portIndex);
    RmtObjectManager *om = get_objmgr();
    if (om == NULL) return NULL;

    Port *port = om->port_get(portIndex);
    if (port == NULL) return NULL;
    Parser *inprs = port->parser()->ingress();
    Deparser *indprs = port->deparser()->ingress();
    if ((inprs == NULL) || (indprs == NULL)) return NULL;

    // Initialize ingress parser with basic config
    parser_config_basic_eth_ip_tcp(inprs);
    inprs->set_parser_mode(RmtDefs::kParserChannels,
                            RmtDefs::kParserMaxStates);
    inprs->set_channel(0, true, 0);


    // Initialize ingress deparser with basic config
    deparser_init(indprs);

    if (get_debug()) printf("TestUtil<port=%d>::port_get() = %p\n", portIndex, port);
    return port;
  }

  Packet *TestUtil::port_process_inbound(Port *port, Packet *p) {
    if ((port == NULL) || (p == NULL)) return NULL;
    if (get_debug()) printf("TestUtil<%d,0,port=%d>::port_process_inbound(%p,%d).....\n",
                            port->pipe_index(), port->port_index(), p, p->len());
    Packet *out = port->process_inbound(p);
    if (get_debug()) printf("TestUtil<%d,0,port=%d>::port_process_inbound(%p,%d) = %p\n",
                            port->pipe_index(), port->port_index(), p, p->len(), out);
    return out;
  }

  Packet *TestUtil::port_process_outbound(Port *port, Packet *p) {
    if ((port == NULL) || (p == NULL)) return NULL;
    if (get_debug()) printf("TestUtil<%d,0,port=%d>::port_process_outbound(%p,%d).....\n",
                            port->pipe_index(), port->port_index(), p, p->len());
    Packet *tx = port->process_outbound(p);
    if (get_debug()) printf("TestUtil<%d,0,port=%d>::port_process_outbound(%p,%d) = %p\n",
                            port->pipe_index(), port->port_index(), p, p->len(), tx);
    return tx;
  }

  Phv *TestUtil::port_process_inbound(Port *port, Phv *phv) {
    if ((port == NULL) || (phv == NULL)) return NULL;
    if (get_debug()) printf("TestUtil<%d,0,port=%d>::port_process_inbound(%p).....\n",
                            port->pipe_index(), port->port_index(), phv);
    Phv *out;
    if ((dv_test_ == 999) || (phv->has_pipe_data())) {
      // NOT a DV test or PHV already has pipe data - don't meddle!
      out = port->matchaction(phv);
    } else {
      // DV test and PHV does NOT have pipe data
      // Do repeated lookups to unit-test phv pipe data mechanism

      uint32_t match0[8] = { 0u }, match1[8] = { 0u }, match2[8] = { 0u };
      uint8_t  calcOnly = PhvDataCtrl::kCalcOnly, loadFromPhv = PhvDataCtrl::kLoadFromPhv;
      EXPECT_EQ(calcOnly, phv->get_pipe_data_ctrl(0, PhvData::kIxbarData));
      EXPECT_EQ(calcOnly, phv->get_pipe_data_ctrl(0, PhvData::kTcamMatchAddr));

      // Run PHV twice.
      // Both times stash TCAM results in phv PhvPipeData
      phv->set_pipe_data_ctrl(0, PhvData::kTcamMatchAddr, PhvDataCtrl::kCalcAndStore);

      // 1. First time calculate input IxBar and store in phv PhvPipeData (whole IxBar)
      //    then calculate and store TCAM results too
      //
      phv->set_pipe_data_ctrl(0, PhvData::kIxbarData, PhvDataCtrl::kCalcAndStore);
      out = port->matchaction(phv); // Phv 'out' still the value we return
      for (int i = 0; i < 8; i++)
        phv->get_pipe_data_tcam_match_addr(0, i, &match0[i], nullptr, nullptr);

      // 2. second time DON'T calculate IxBar, just load from phv PhvPipeData
      //    then calculate and store TCAM results in phv PhvPipeData again
      //
      phv->set_pipe_data_ctrl(0, PhvData::kIxbarData, PhvDataCtrl::kLoadFromPhv);
      // Clear down any stored TCAM results in phv PhvPipeData
      for (int i = 0; i < 8; i++) phv->set_pipe_data_tcam_match_addr(0, i, 0u, 0, 0);
      Phv *out1 = port->matchaction(phv);
      // out1 should be equal to out
      EXPECT_TRUE(out->equals(out1));
      for (int i = 0; i < 8; i++)
        phv->get_pipe_data_tcam_match_addr(0, i, &match1[i], nullptr, nullptr);
      if (out1 != phv) phv_free(out1);

      // Compare LTCAM match addrs - should be same!
      for (int i = 0; i < 8; i++) {
        EXPECT_EQ(match0[i], match1[i]);
      }

      // Now create a new phv, cloned from our initial passed-in phv
      // - it should refer to *same* PhvPipeData (still marked LoadFromPhv)
      //
      Phv *phv2 = phv->clone(false);
      for (int i = 0; i < Phv::kWordsMaxExtended; i++) {
        // Zeroise valid words only - want to keep valid bits as they were
        if (Phv::is_valid_phv_x(i) && phv->is_valid_x(i)) phv2->clobber_x(i, 0u);
      }

      EXPECT_TRUE(phv2->has_pipe_data());
      EXPECT_EQ(loadFromPhv, phv2->get_pipe_data_ctrl(0, PhvData::kIxbarData));
      uint8_t calcAndStore = PhvDataCtrl::kCalcAndStore;
      EXPECT_EQ(calcAndStore, phv2->get_pipe_data_ctrl(0, PhvData::kTcamMatchAddr));

      // Clear down any stored TCAM results in phv/phv2 PhvPipeData
      for (int i = 0; i < 8; i++)
        phv2->set_pipe_data_tcam_match_addr(0, i, 0u, 0, 0);

      // Then lookup again
      // - This zero PHV should get *same* TCAM match addrs despite diff PHV words
      //   as IxBar data will have been used from phv2 PhvPipeData instead
      //
      Phv *out2 = port->matchaction(phv2);
      for (int i = 0; i < 8; i++)
        phv2->get_pipe_data_tcam_match_addr(0, i, &match2[i], nullptr, nullptr);
      if (out2 != phv2) phv_free(out2);

      // Compare LTCAM match addrs again
      for (int i = 0; i < 8; i++) {
        EXPECT_EQ(match1[i], match2[i]);
      }

      // Free up
      phv_free(phv2); // Does not free PhvPipeData

      // Free pipe data from passed-in phv
      phv->free_pipe_data();
    }
    if (get_debug()) printf("TestUtil<%d,0,port=%d>::port_process_inbound(%p) = %p\n",
                            port->pipe_index(), port->port_index(), phv, out);
    return out;
  }

  Phv *TestUtil::port_process_inbound(Port *port, Phv *phv, Phv *ophv) {
    if ((port == NULL) || (phv == NULL) || (ophv == NULL)) return NULL;
    if (get_debug()) printf("TestUtil<%d,0,port=%d>::port_process_inbound(%p).....\n",
                            port->pipe_index(), port->port_index(), phv);
    Phv *out = port->matchaction2(phv, ophv);
    if (get_debug()) printf("TestUtil<%d,0,port=%d>::port_process_inbound(%p) = %p\n",
                            port->pipe_index(), port->port_index(), phv, out);
    return out;
  }

  Phv *TestUtil::port_parse(Port *port, Packet *p) {
    if ((port == NULL) || (p == NULL)) return NULL;
    if (get_debug()) printf("TestUtil<%d,0,port=%d>::port_parse(%p,%d).....\n",
                            port->pipe_index(), port->port_index(), p, p->len());
    Phv *out = port->parse(p);
    if (get_debug()) printf("TestUtil<%d,0,port=%d>::port_parse(%p,%d) = %p\n",
                            port->pipe_index(), port->port_index(), p, p->len(), out);
    return out;
  }

  void TestUtil::handle_eop(Port *port, const Eop &eop) {
    if (port == NULL) return;
    if (get_debug()) printf("TestUtil<%d,0,port=%d>::port_handle_eop().....\n",
                            port->pipe_index(), port->port_index());
    port->handle_eop(eop);
  }


  // Read value from a Comira UMAC3 MacCounter
  uint64_t TestUtil::mac_cntr_read_vmac_c3(uint32_t mac_cntr_base_addr,
                                                int chan, int cntr_index, bool clear) {
    RMT_ASSERT((chan >= 0) && (chan < 4));
    RMT_ASSERT((cntr_index >= 0) && (cntr_index <= 88));

    // Need to convert word-addressed CSR (0x1) to byte address
    uint32_t ctl_addr = mac_cntr_base_addr + (0x1 * 4);
    uint32_t ctl = 0u;
    int      cnt;

    // Loop till not busy
    for (cnt = 0; cnt < 10; cnt++) {
      if ( ( (InWord(ctl_addr) >> 15) & 1) == 0) break;
    }
    if (cnt >= 10) return UINT64_C(0xFFFFFFFFFFFFFFFF);

    // Write setting busy (bit15) and chan/cntr we want to read
    ctl = (1<<15) | (((clear)?1:0) << 13) | ((chan & 0x3) << 7) | ((cntr_index & 0x7F) << 0);
    OutWord(ctl_addr, ctl);

    // Loop till not busy again
    for (cnt = 0; cnt < 10; cnt++) {
      if ( ( (InWord(ctl_addr) >> 15) & 1) == 0) break;
    }
    if (cnt >= 10) return UINT64_C(0xFFFFFFFFFFFFFFFF);

    // Now we can read counter data
    uint32_t data_addr[4] = { mac_cntr_base_addr + (0x2 * 4), mac_cntr_base_addr + (0x3 * 4),
                              mac_cntr_base_addr + (0x4 * 4), mac_cntr_base_addr + (0x5 * 4) };
    uint64_t data[4] = { static_cast<uint64_t>( InWord( data_addr[0] ) & 0xFFFF ),
                         static_cast<uint64_t>( InWord( data_addr[1] ) & 0xFFFF ),
                         static_cast<uint64_t>( InWord( data_addr[2] ) & 0xFFFF ),
                         static_cast<uint64_t>( InWord( data_addr[3] ) & 0xFFFF ) };
    return (data[3] << 48) | (data[2] << 32) | (data[1] << 16) | (data[0] << 0);
  }
  // Read value from a Comira UMAC4 MacCounter
  uint64_t TestUtil::mac_cntr_read_vmac_c4(uint32_t mac_cntr_base_addr,
                                                int chan, int cntr_index, bool clear) {
    RMT_ASSERT((chan >= 0) && (chan < 8));
    RMT_ASSERT((cntr_index >= 0) && (cntr_index <= 95));

    uint32_t data_addr[2] = { mac_cntr_base_addr + (chan << 10) + (cntr_index << 3) + 0,
                              mac_cntr_base_addr + (chan << 10) + (cntr_index << 3) + 4 };
    uint64_t data[2] = { static_cast<uint64_t>( InWord( data_addr[0] ) ),
                         static_cast<uint64_t>( InWord( data_addr[1] ) ) };
    if (clear) {
      OutWord(data_addr[0], 0u); OutWord(data_addr[1], 0u);
    }
    return (data[1] << 32) | (data[0] << 0);
  }
  // Read value from a Tamba MAC MacCounter
  uint64_t TestUtil::mac_cntr_read_vmac_t1(uint32_t mac_cntr_base_addr,
                                           int chan, int cntr_index, bool clear) {
    RMT_ASSERT((chan >= 0) && (chan < 8));
    RMT_ASSERT((cntr_index >= 0) && (cntr_index <= 95));

    // Need to convert word-addressed CSR (0x1) to byte address
    uint32_t ctl_addr = mac_cntr_base_addr + 0x8;
    uint32_t ctl = 0u;
    int      cnt;

    // Loop till not busy
    for (cnt = 0; cnt < 10; cnt++) {
      if ( ( (InWord(ctl_addr) >> 16) & 1) == 0) break;
    }
    if (cnt >= 10) return UINT64_C(0xFFFFFFFFFFFFFFFF);

    // Write setting access request (bit16) and chan/cntr we want to read
    ctl = (1<<16) | (((chan & 0x7) * 96) + (cntr_index % 96));
    OutWord(ctl_addr, ctl);

    // Loop till not busy again
    for (cnt = 0; cnt < 10; cnt++) {
      if ( ( (InWord(ctl_addr) >> 16) & 1) == 0) break;
    }
    if (cnt >= 10) return UINT64_C(0xFFFFFFFFFFFFFFFF);

    // Now we can read counter data
    uint32_t rd_data_addr[2] = { mac_cntr_base_addr + 0x0C, mac_cntr_base_addr + 0x10 };
    uint64_t rd_data[2] = { static_cast<uint64_t>( InWord( rd_data_addr[0] ) & 0xFFFFFFFF ),
                            static_cast<uint64_t>( InWord( rd_data_addr[1] ) & 0xFFFFFFFF ) };

    if (clear) {
      uint32_t wr_data_addr[2] = { mac_cntr_base_addr + 0x00, mac_cntr_base_addr + 0x04 };
      OutWord(wr_data_addr[0], 0u); OutWord(wr_data_addr[1], 0u);

      // Write setting access request (bit16) and write indication (bit12) chan/cntr we want to read
      ctl = (1<<16) | (1<<12) | (((chan & 0x7) * 96) + (cntr_index % 96));
      OutWord(ctl_addr, ctl);
    }

    return (rd_data[1] << 32) | (rd_data[0] << 0);
  }

  // Read value from a Comira UMAC3 MacCounter based on Port
  uint64_t TestUtil::mac_cntr_read_vmac_c3(int port_index, int cntr_index, bool clear) {
    RmtObjectManager *om = get_objmgr();
    Port *port = om->port_lookup(port_index);
    if (port == nullptr) return UINT64_C(0xFFFFFFFFFFFFFFFF);
    Mac *mac = port->mac();
    if (mac == nullptr) return UINT64_C(0xFFFFFFFFFFFFFFFF);
    if (mac->mac_type() != MacType::kVmacC3) return UINT64_C(0xFFFFFFFFFFFFFFFF);
    return mac_cntr_read_vmac_c3(mac->mac_counter_base_addr(), port->mac_chan(),
                                 cntr_index, clear);
  }
  // Read value from a Comira UMAC4 MacCounter based on Port
  uint64_t TestUtil::mac_cntr_read_vmac_c4(int port_index, int cntr_index, bool clear) {
    RmtObjectManager *om = get_objmgr();
    Port *port = om->port_lookup(port_index);
    if (port == nullptr) return UINT64_C(0xFFFFFFFFFFFFFFFF);
    Mac *mac = port->mac();
    if (mac == nullptr) return UINT64_C(0xFFFFFFFFFFFFFFFF);
    if (mac->mac_type() != MacType::kVmacC4) return UINT64_C(0xFFFFFFFFFFFFFFFF);
    return mac_cntr_read_vmac_c4(mac->mac_counter_base_addr(), port->mac_chan(),
                                 cntr_index, clear);
  }
  // Read value from a Tamba MAC MacCounter based on Port
  uint64_t TestUtil::mac_cntr_read_vmac_t1(int port_index, int cntr_index, bool clear) {
    RmtObjectManager *om = get_objmgr();
    Port *port = om->port_lookup(port_index);
    if (port == nullptr) return UINT64_C(0xFFFFFFFFFFFFFFFF);
    Mac *mac = port->mac();
    if (mac == nullptr) return UINT64_C(0xFFFFFFFFFFFFFFFF);
    if (mac->mac_type() != MacType::kVmacT1) return UINT64_C(0xFFFFFFFFFFFFFFFF);
    return mac_cntr_read_vmac_t1(mac->mac_counter_base_addr(), port->mac_chan(),
                                 cntr_index, clear);
  }

  // Read value from arbitrary MacCounter based on Port
  uint64_t TestUtil::mac_cntr_read(int port_index, int cntr_index, bool clear) {
    RmtObjectManager *om = get_objmgr();
    Port *port = om->port_lookup(port_index);
    if (port == nullptr) return UINT64_C(0xFFFFFFFFFFFFFFFF);
    Mac *mac = port->mac();
    if (mac == nullptr) return UINT64_C(0xFFFFFFFFFFFFFFFF);
    if (mac->mac_type() == MacType::kVmacC3) {
      return mac_cntr_read_vmac_c3(port_index, cntr_index, clear);
    } else if (mac->mac_type() == MacType::kVmacC4) {
      return mac_cntr_read_vmac_c4(port_index, cntr_index, clear);
    } else if (mac->mac_type() == MacType::kVmacT1) {
      return mac_cntr_read_vmac_t1(port_index, cntr_index, clear);
    } else {
      return UINT64_C(0xFFFFFFFFFFFFFFFF);
    }
  }




  Phv* TestUtil::get_read_phv( const char c, const std::string s, const int n, int e ) {
    return test_reader_.get_action()->get_phv(c,s,n,e);
  }
  Eop* TestUtil::get_read_eop( int n, int e ) {
    return test_reader_.get_action()->get_eop(n,e);
  }
  void TestUtil::process_read_phv_match_and_action( const int n, int e, int *ingress_start_table, int *egress_start_table ) {
    test_reader_.get_action()->process_match(n,e,ingress_start_table,egress_start_table);
    test_reader_.get_action()->process_action(n,e);
  }

  void TestUtil::check_counter(volatile void *addr,
                               int width,
                               const std::function<void()> &incrementer,
                               std::string counter_name) {
    auto *test_counter = new_fake_register(addr, width);
    EXPECT_EQ(UINT64_C(0), test_counter->read()) << counter_name;  // sanity check

    test_counter->write(UINT64_C(0xabba));
    EXPECT_EQ(UINT64_C(0xabba), test_counter->read()) << counter_name;

    if (NULL != incrementer) {
      incrementer();
      EXPECT_EQ(UINT64_C(0xabbb), test_counter->read()) << counter_name;

      test_counter->write_max();
      incrementer();
      EXPECT_EQ(UINT64_C(0), test_counter->read()) << counter_name;

      incrementer();
      EXPECT_EQ(UINT64_C(1), test_counter->read()) << counter_name;
    }
    delete test_counter;
  }

}
