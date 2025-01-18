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

#ifndef _UTESTS_TEST_UTIL_
#define _UTESTS_TEST_UTIL_

#include <functional>
#include <memory>
#include <atomic>
#include <model_core/model.h>
#include <indirect-addressing.h>
#include <register_utils.h>
#include <rmt-defs.h>
#include <rmt-op-handler.h>
#include <rmt-object-manager.h>
#include <deparser.h>
#include <phv.h>
#include <eop.h>
#include <pktgen-app-reg.h>
#include <address.h>
#include <bitvector.h>
#include "test_namespace.h"
#include "test_config.h"
#include "test_reader.h"
#include "action_hv_translator.h"
#include <utests/gtest.h>
#include <utests/register_mapper.h>

#define RMT_UT_LOG_INFO         printf
#define RMT_UT_LOG_ERROR(...) do {   \
    fprintf(stderr, __VA_ARGS__);    \
  } while(0)


// Some macros to fix up instructions written to IMEM
// Original formats in test_dvXX.cpp files are before loss of Bit4

#ifdef FIRST_TRANSFORMATION_TO_LOSE_BIT4
// Mostly just lose bit4.
// But in LoadConst16|32 cases need to shift bits about - bit11 moves up to bit16 (so shift left by 5)
// In all LoadConst cases we shift the 5b at [16:12] down by 1 and shift colour/parity (all hi bits) down by 1
// Input is old format so OPcode at pos 12
#define RM_B4(x)    ((((x)&~0x1Fu)>>1) | ((x)&0xFu))

#define RM_B4_32(x) ( ((((x)>>12)&0xFu)==0x8u) ?( (((x)&0x800u)<<5) | (((x)&0x1F000u)>>1) | ((x)&0x07FE07FFu) | (((x)&0xF0000000u)>>1) )  :((((x)&~0x1Fu)>>1) | ((x)&0xFu)) )

#define RM_B4_16(x) ( ((((x)>>12)&0xFu)==0x8u) ?( (((x)&0x800u)<<5) | (((x)&0x1F000u)>>1) | ((x)&0x00FE07FFu) | (((x)&0xFE000000u)>>1) )  :((((x)&~0x1Fu)>>1) | ((x)&0xFu)) )

#define RM_B4_8(x)  ( ((((x)>>12)&0xFu)==0x8u) ?(                     (((x)&0x1F000u)>>1) | ((x)&0x001E07FFu) | (((x)&0xFFC00000u)>>1) )  :((((x)&~0x1Fu)>>1) | ((x)&0xFu)) )
#endif

#define SECOND_TRANSFORMATION_TO_LOSE_BIT4_AND_BIT11
#ifdef SECOND_TRANSFORMATION_TO_LOSE_BIT4_AND_BIT11
// Remember source format is original *before* loss of Bit4
// Here need to lose bit4 and bit11
// But in LoadConst16|32 cases need to shift bits about - bits[11:10] move up to bits[16:15] (so shift left by 5)
// In all LoadConst cases we shift the 5b at [16:12] down by 2 and shift colour/parity (all hi bits) down by 2
// Input is old format so OPcode at pos 12
#define RM_B4(x)    ( (((x)&~0xFFFu)>>2) | (((x)&0x7E0)>>1) | ((x)&0xFu) )

#define RM_B4_32(x) ( ((((x)>>12)&0xFu)==0x8u) ?( (((x)&0xC00u)<<5) | (((x)&0x1F000u)>>2) | ((x)&0x07FE03FFu) | (((x)&0xF0000000u)>>2) )  :( (((x)&~0xFFFu)>>2) | (((x)&0x7E0)>>1) | ((x)&0xFu) )  )

#define RM_B4_16(x) ( ((((x)>>12)&0xFu)==0x8u) ?( (((x)&0xC00u)<<5) | (((x)&0x1F000u)>>2) | ((x)&0x00FE03FFu) | (((x)&0xFE000000u)>>2) )  :( (((x)&~0xFFFu)>>2) | (((x)&0x7E0)>>1) | ((x)&0xFu) )  )

#define RM_B4_8(x)  ( ((((x)>>12)&0xFu)==0x8u) ?(                     (((x)&0x1F000u)>>2) | ((x)&0x001E03FFu) | (((x)&0xFFC00000u)>>2) )  :( (((x)&~0xFFFu)>>2) | (((x)&0x7E0)>>1) | ((x)&0xFu) )  )
#endif


namespace MODEL_CHIP_NAMESPACE {
  class Parser;
}

namespace MODEL_CHIP_TEST_NAMESPACE {

  // Log level constants for use with TestUtil::update_log_flags().
  static constexpr uint64_t ONE = UINT64_C(1);
  static constexpr uint64_t HI  = UINT64_C(0xFFFFFFFF00000000);
  static constexpr uint64_t ALL = UINT64_C(0xFFFFFFFFFFFFFFFF);
  static constexpr uint64_t FEW = UINT64_C(0xF); // FatalErrorWarn
  static constexpr uint64_t NON = UINT64_C(0);
  static constexpr uint64_t TOP = UINT64_C(1) << 63;
  static constexpr uint64_t TYP_DEPARSER =
      (ONE << MODEL_CHIP_NAMESPACE::RmtTypes::kRmtTypeDeparser);
  static constexpr uint64_t TYP_PARSER =
      (ONE << MODEL_CHIP_NAMESPACE::RmtTypes::kRmtTypeParser);
  static constexpr uint64_t TYP_PARSER_CHECKSUM =
      (ONE << MODEL_CHIP_NAMESPACE::RmtTypes::kRmtTypeParser) |
      (ONE << MODEL_CHIP_NAMESPACE::RmtTypes::kRmtTypeChecksumEngine);


/**
 * Provides mechanism to capture rmt assert and error logging in a tmpfile.
 */
class RmtStreamCaptureBase {
 public:
  RmtStreamCaptureBase();
  virtual ~RmtStreamCaptureBase();

  /**
   * Start redirecting the configured stream to a tmpfile. The first call to
   * this method, or the first call after a call to clear(), will cause a
   * tmpfile to be created. Subsequent calls will start redirection to the
   * existing tmpfile.
   *
   * @return pointer to the tmpfile, or nullptr if a tmpfile could not be
   * created.
   */
  FILE* start();

  /**
   * Stop redirecting the configured stream. This method does not close or
   * remove any existing tmpfile.
   */
  void stop();

  /**
   * Stop redirecting the configured stream and remove any existing tmpfile.
   * This method is called during destruction, but can be called before if a
   * new tmpfile is desired on a subsequent call to start().
   */
  void clear();

  /**
   * Same as calling clear() followed by start().
   */
  void clear_and_start();

  /**
   * Reads from the tmpfile line-by-line and calls callback function with each
   * line and its line number.
   *
   * @param callback_fn a function to call with each line number and line.
   * @return the number of lines read; -1 if the file_ptr is null.
   */
  int for_each_line(
    const std::function<void(int line_num, std::string line)> callback_fn);

  /**
   * Reads from the tmpfile line-by-line and calls callback function with each
   * line and its line number if the line contains the given match string at
   * the given position.
   *
   * @param match A string that must occur in lines that are matched.
   * @param callback_fn a function to call with each line number, position at
   *     which match string was found, and line.
   * @param match_pos The position at which the match string must be found in a
   *     line; this default to std::string::npos which is interpreted as 'any
   *     position'.
   * @return the number of lines read; -1 if the file_ptr is null.
   */
  int for_each_line_containing(
    const std::string match,
    const std::function<void(
        int line_num,
        size_t pos,
        std::string line)> callback_fn = nullptr,
    const size_t match_pos=std::string::npos);

  /**
   * Reads from the tmpfile line-by-line and calls callback function with each
   * line and its line number if the line starts with the given match string.
   *
   * @param match A string that must occur at the start of lines that are
   *     matched.
   * @param callback_fn a function to call with each line number, position at
   *     which match string was found, and line.
   * @return the number of lines read; -1 if the file_ptr is null.
   */
  int for_each_line_starts_with(
    const std::string match,
    const std::function<void(
        int line_num,
        size_t pos,
        std::string line)> callback_fn = nullptr);

  /**
   * Dump contents of tmpfile to a string.
   * @param max_line_length Specifies length to which each printed line should
   *     be truncated; defaults to string::npos which is interpreted as no
   *     limit.
   * @param with_line_numbers If true (default) then each line will be prefixed
   *     with its line number; if false then no prefix will be printed.
   * @return A std::string
   */
  std::string dump_lines(
      size_t max_line_length=std::string::npos,
      bool with_line_numbers=true);

 private:
  virtual void set_stream(FILE *file_ptr) = 0;
  FILE *tmpfile_;
};

/**
 * Provides mechanism to capture rmt assert and error logging in a tmpfile.
 */
class RmtStreamCapture : public RmtStreamCaptureBase {
 public:
  enum RmtStreamCaptureEnum {
    stdout,
    stderr
  };

  RmtStreamCapture(RmtStreamCaptureEnum stream_num);
  ~RmtStreamCapture();

 private:
  virtual void set_stream(FILE *file_ptr);
  RmtStreamCaptureEnum stream_num_;
};

/**
 * Provides mechanism to capture rmt logging in a tmpfile.
 */
class RmtLoggerCapture : public RmtStreamCaptureBase {
 public:
  RmtLoggerCapture(MODEL_CHIP_NAMESPACE::RmtLogger *rmt_logger);
  ~RmtLoggerCapture();

 private:
  virtual void set_stream(FILE *file_ptr);
  MODEL_CHIP_NAMESPACE::RmtLogger *rmt_logger_;
};

/**
 * Test fixture class to provide basic test setup and teardown.
 */
class BaseTest : public testing::Test {
 public:
  virtual void SetUp() override;
  virtual void TearDown() override;
  virtual int chip_index() { return chip_index_; }
  void set_chip_index(int val) { chip_index_ = val; }
  int pipe_index() { return pipe_index_; }
  void set_pipe_index(int val) { pipe_index_ = val; }
  uint64_t pipes_mask() { return UINT64_C(1) << pipe_index(); };
  int stage_index() { return stage_index_; }
  void set_stage_index(int val) { stage_index_ = val; }
  //                         <   DA           SA      ET I4 C LN  ID  OF  T P CK   SRC     DST    SP  DP>
  //                         <   DA     ><    SA    ><ET><><><LN><ID><OF><><><CK>< SRC  >< DST  ><SP><DP>
  const char *udp_pktstr_ = "080022AABBCC080011DDEEFF080045000100123400001011FFFF0A1122330A4455661188119900000000000000000000000000000000";
  const char *tcp_pktstr_ = "080022AABBCC080011DDEEFF080045000100123400001006FFFF0A1122330A4455660688069900000000000000000000000000000000";

 protected:
  TestUtil *tu_;
  MODEL_CHIP_NAMESPACE::RmtObjectManager *om_;

  RmtStreamCapture *rmt_stdout_capture();
  RmtStreamCapture *rmt_stderr_capture();
  RmtLoggerCapture *rmt_logger_capture();
  void load_context_json_file(int pipe_index) const;

 private:
  int chip_index_ = 202;
  int pipe_index_ = 1;
  int stage_index_ = 0;
  RmtStreamCapture *rmt_stdout_capture_ = nullptr;
  RmtStreamCapture *rmt_stderr_capture_ = nullptr;
  RmtLoggerCapture *rmt_logger_capture_ = nullptr;
};

class FakeRegister {
  // Provides accessor methods to underlying register of given width.
 public:
  // Construct a register of given width at given addresses.
  FakeRegister(TestUtil *tu,
               uint32_t addr_,
               int width);
  // Read value of register.
  uint64_t read();
  // Write given data to register.
  void write(uint64_t data);
  // Write the register with the maximum value for its given width.
  void write_max();

 private:
  TestUtil *tu_;
  int width_;
  uint64_t max_;
  uint32_t addr_;
};

class TestUtil {

 public:
    // What 'vintage' DV test are we?
    static constexpr int      kVintageOriginal = 0;
    static constexpr int      kVintagePostPfe = 1;
    static constexpr int      kVintagePostHyperdev = 2;
    static constexpr int      kVintageLatest = 999;

    // Chip index >8 means only 2 MAUs
    static constexpr int      kChipDefault = 9;
    static constexpr int      kTcamDataBits = 44;
    static constexpr int      kTcamPayloadBits = 1;
    static constexpr int      kTcamBits = kTcamDataBits + kTcamPayloadBits;
    static constexpr uint64_t kTcamPayloadMask = (UINT64_C(1) << kTcamPayloadBits) - 1;
    static constexpr uint64_t kTcamDataMask = (UINT64_C(1) << kTcamDataBits) - 1;
    static constexpr uint64_t kTcamMask = (UINT64_C(1) << kTcamBits) - 1;
    static constexpr int      kTcamMaxEntries = 512;
    static constexpr int      kSramMaxEntries = 1024;
    static constexpr int      kMapramMaxEntries = 1024;

    static constexpr int      kPipesMin = MODEL_CHIP_NAMESPACE::RmtDefs::kPipesMin;
    static constexpr int      kPipesMax = MODEL_CHIP_NAMESPACE::RmtDefs::kPipesMax;
    static constexpr int      kPipeBits = MODEL_CHIP_NAMESPACE::RmtDefs::kPipeBits;
    static constexpr int      kPipeMask = MODEL_CHIP_NAMESPACE::RmtDefs::kPipeMask;
    static constexpr int      kStagesMin = MODEL_CHIP_NAMESPACE::RmtDefs::kStagesMin;
    static constexpr int      kStagesMax = MODEL_CHIP_NAMESPACE::RmtDefs::kStagesMax;
    static constexpr int      kStageBits = MODEL_CHIP_NAMESPACE::RmtDefs::kStageBits;
    static constexpr int      kStageMask = MODEL_CHIP_NAMESPACE::RmtDefs::kStageMask;
    static constexpr int      kStageShift = MODEL_CHIP_NAMESPACE::kIndStageShift;
    static constexpr int      kSku = MODEL_CHIP_NAMESPACE::RmtDefs::kSkuDefault;
    static constexpr int      kMaxPipes = kPipesMax;
    static constexpr uint8_t  kAllPipesEn = (1<<kMaxPipes)-1;
    static constexpr int      kMaxStages = kStagesMax;

    static constexpr int      kSramMaxRows = 8;
    static constexpr int      kSramMaxCols = 12;
    static constexpr int      kTcamMaxRows = 12;
    static constexpr int      kTcamMaxCols = 2;
    static constexpr int      kAbsMaxRows = kTcamMaxRows;
    static constexpr int      kAbsMaxCols = kSramMaxCols;
    static constexpr int      kMaxPhysBuses = 16;
    static constexpr int      kMaxLogTcams = 8;
    static constexpr int      kMaxLogTabs = 16;
    static constexpr int      kMaxAlus = 4;
    static constexpr int      kMaxPhvNum = MODEL_CHIP_NAMESPACE::RmtDefs::kPhvWordsMax;
    static constexpr int      kPhvsPerGrp = MODEL_CHIP_NAMESPACE::RmtDefs::kPhvWordsPerGroup;
    static constexpr int      kMaxOpNum = 32;

    static constexpr int      kPhysMemTypeSRAM = 0;
    static constexpr int      kPhysMemTypeMapRAM = 1;
    static constexpr int      kPhysMemTypeDeferStatsRAM  = 2;
    static constexpr int      kPhysMemTypeDeferMeterRAM = 3;
    static constexpr int      kPhysMemTypeTCAM = 4;

    static constexpr int      kVirtMemTypeStats = 0;
    static constexpr int      kVirtMemTypeMeter = 1;
    static constexpr int      kVirtMemTypeSelectorStateful = 2;
    static constexpr int      kVirtMemTypeSelector = 2;
    static constexpr int      kVirtMemTypeStateful = 2;
    static constexpr int      kVirtMemTypeIdletime = 3;

    static constexpr int      kUnitramTypeMatch = 1;
    static constexpr int      kUnitramTypeAction = 2;
    static constexpr int      kUnitramTypeStats = 3;
    static constexpr int      kUnitramTypeMeter = 4;
    static constexpr int      kUnitramTypeStateful = 5;
    static constexpr int      kUnitramTypeTind = 6;
    static constexpr int      kUnitramTypeSelector = 7;
    static constexpr int      kMaxUnitramType = 7;

    static constexpr int      kMapramTypeStats = 1;
    static constexpr int      kMapramTypeMeter = 2;
    static constexpr int      kMapramTypeStateful = 3;
    static constexpr int      kMapramTypeIdletime = 4;
    static constexpr int      kMapramTypeColor = 5;
    static constexpr int      kMapramTypeSelector = 6;
    static constexpr int      kMaxMapramType = 6;

    // Byte0 = general flags, Byte1 = idle bitwidth, Byte2 = idle bus
    static constexpr int      kIdleFlagDisableNotify = 0x0000001;
    static constexpr int      kIdleFlagTwoWay        = 0x0000002;
    static constexpr int      kIdleFlagPerFlow       = 0x0000004;
    static constexpr int      kIdleFlagUnused        = 0x0000080;
    static constexpr int      kIdleFlagFlagMask      = 0x00000FF;
    static constexpr int      kIdleFlagFlagShift     = 0;
    static constexpr int      kIdleFlagIntervalShift = 8;
    static constexpr int      kIdleFlagIdleBusShift  = 16;
    static constexpr int      kIdleFlagBitWidthShift = 24;
    static constexpr int      kIdleFlagIntervalMask  = 0xF;
    static constexpr int      kIdleFlagIdleBusMask   = 0x1F;
    static constexpr int      kIdleFlagBitWidthMask  = 0x3;
    static inline int idle_flags_flags(int f) {
      return ((f >> kIdleFlagFlagShift) & kIdleFlagFlagMask);
    }
    static inline int idle_flags_interval(int f) {
      return ((f >> kIdleFlagIntervalShift) & kIdleFlagIntervalMask);
    }
    static inline int idle_flags_idlebus(int f) {
      return ((f >> kIdleFlagIdleBusShift) & kIdleFlagIdleBusMask);
    }
    static inline int idle_flags_bitwidth(int f) {
      return ((f >> kIdleFlagBitWidthShift) & kIdleFlagBitWidthMask);
    }
    static inline int idle_flags_make(int flags, int intvl, int idlebus, int bitwidth) {
      int f = 0;
      f |= (flags & kIdleFlagFlagMask) << kIdleFlagFlagShift;
      f |= (intvl & kIdleFlagIntervalMask) << kIdleFlagIntervalShift;
      f |= (idlebus & kIdleFlagIdleBusMask) << kIdleFlagIdleBusShift;
      f |= (bitwidth & kIdleFlagBitWidthMask) << TestUtil::kIdleFlagBitWidthShift;
      return f;
    }


    // from http://www-graphics.stanford.edu/~seander/bithacks.html#ParityParallel
    static uint8_t parity32( uint32_t v ) {
      v ^= v >> 16;
      v ^= v >> 8;
      v ^= v >> 4;
      v &= 0xf;
      return (0x6996 >> v) & 1;
    }
    static bool compare_phvs(MODEL_CHIP_NAMESPACE::Phv *phv_in,
                             MODEL_CHIP_NAMESPACE::Phv *phv_out,bool print=false) {
      if ((phv_in == NULL) || (phv_out == NULL)) return false;
      bool match=true;
      for (int i = 0; i < MODEL_CHIP_NAMESPACE::RmtDefs::kPhvWordsMax; i++) {
        uint32_t vi  = phv_in->get(i);
        uint32_t vo  = phv_out->get(i);
        if ( vi != vo ) {
          match=false;
          if (print) {
            // print the command that will check for this change in the phv
            printf("    EXPECT_EQ(0x%x, phv_out2->get(%d)); // was 0x%x in input phv\n", vo, i, vi);
          }
        }
      }
      return match;
    }
    static uint64_t make_physical_address(int pipe, int stage, int memType,
                                          int row, int col, int index) {
      if ((pipe < 0) || (pipe >= kMaxPipes) ||
          (stage < 0) || (stage >= kMaxStages) ||
          (row < 0) || (row >= kAbsMaxRows) || (col < 0) || (col >= kAbsMaxCols))
        return UINT64_C(0xFFFFFFFFFFFFFFFF);
      int phys_virt = 0; // =>physical addr
      uint64_t addr = UINT64_C(0x20080000000); // Base for phys/virt addresses
      addr |= static_cast<uint64_t>(pipe & ((1<<3)-1)) << (kStageShift+kStageBits);
      addr |= static_cast<uint64_t>(stage & kStageMask) << kStageShift;
      addr |= static_cast<uint64_t>(phys_virt & 1) << 30;
      addr |= static_cast<uint64_t>(memType & ((1<<3)-1)) << 18;
      addr |= static_cast<uint64_t>(row & ((1<<4)-1)) << 14;
      addr |= static_cast<uint64_t>(col & ((1<<4)-1)) << 10;
      addr |= static_cast<uint64_t>(index & ((1<<10)-1)) << 0;
      return addr;
    }
    static uint64_t make_virtual_address(int pipe, int stage, int memType,
                                         int logtab, uint32_t vaddr) {
      if ((pipe < 0) || (pipe >= kMaxPipes) ||
          (stage < 0) || (stage >= kMaxStages) ||
          (logtab < 0) || (logtab >= kMaxLogTabs))
        return UINT64_C(0xFFFFFFFFFFFFFFFF);
      int phys_virt = 1; // =>virtual addr
      uint64_t addr = UINT64_C(0x20080000000); // Base for phys/virt addresses
      addr |= static_cast<uint64_t>(pipe & ((1<<3)-1)) << (kStageShift+kStageBits);
      addr |= static_cast<uint64_t>(stage & kStageMask) << kStageShift;
      addr |= static_cast<uint64_t>(phys_virt & 1) << 30;
      addr |= static_cast<uint64_t>(memType & ((1<<2)-1)) << 25;
      addr |= static_cast<uint64_t>(logtab & ((1<<4)-1)) << 21;
      addr |= static_cast<uint64_t>(vaddr & ((1<<21)-1)) << 0;
      return addr;
    }
    static uint64_t make_instruction_address(int pipe, int stage, int datasize, int instr) {
      if ((pipe < 0) || (pipe >= kMaxPipes) ||
          (stage < 0) || (stage >= kMaxStages))
        return UINT64_C(0xFFFFFFFFFFFFFFFF);
      uint64_t addr = UINT64_C(0x20040000000); // Base for instruction addresses
      addr |= static_cast<uint64_t>(pipe & ((1<<3)-1)) << (kStageShift+kStageBits);
      addr |= static_cast<uint64_t>(stage & kStageMask) << kStageShift;
      addr |= static_cast<uint64_t>(datasize & ((1<<2)-1)) << 28;
      addr |= static_cast<uint64_t>(instr & ((1<<28)-1)) << 0;
      return addr;
    }

    static void DummyDruLearn( int asic, uint8_t *learn_filter_data, int len, int pipe_nbr )
    {
      // XXX: static definition of this callback is making it very convoluted to access model
      // registers... (is there a global host interface available in test util infra ???
      // TODO: make print conditional
      if (1) printf("TestUtil DruLearn<chip=%d pipe=%d> total_len=%d, learn quantum:\n",asic,pipe_nbr, len);
      if (0) {
        for (int i=0;i<len;i++) {
          printf(" %02x",learn_filter_data[i]);
        }
        printf("\n");
      }
      dru_learn_callback_count_++;
      if (1) printf("Callback count = %d\n", dru_learn_callback_count_);
    }

    TestUtil(model_core::Model *model, int chip, int pipe, int stage);
    TestUtil(model_core::Model *model, int chip, int pipe)
        : TestUtil(model, chip, pipe,0 /*stage*/)   {}
    TestUtil(model_core::Model *model, int chip)
        : TestUtil(model, chip, 0 /*pipe*/)  {}
    TestUtil(model_core::Model *model)
        : TestUtil(model, 0 /*chip*/)  {}
    virtual ~TestUtil();


    inline model_core::Model                       *get_model()  const { return model_; }
    MODEL_CHIP_NAMESPACE::RmtObjectManager  *get_objmgr() const;
    MODEL_CHIP_NAMESPACE::Mau *get_mau() { return get_objmgr()->mau_lookup(pipe_, stage_); }

    inline int               get_chip()           const { return chip_; }
    inline int               get_pipe()           const { return pipe_; }
    inline int               get_stage()          const { return stage_; }
    inline bool              get_use_ind_regs()   const { return use_ind_regs_; }
    inline bool              get_debug()          const { return debug_; }
    inline void              set_pipe(int p)            { pipe_ = p; }
    inline void              set_stage(int s)           { stage_ = s; }
    inline void              set_use_ind_regs(bool tf)  { use_ind_regs_ = tf; }
    inline void              set_debug(bool tf)         { debug_ = tf; }
    inline void set_evaluate_all(bool tf, bool dotest=true) {
      evaluate_all_ = tf;
      evaluate_all_test_ = dotest;
      // Tell objmgr if it is in evaluateAll mode
      MODEL_CHIP_NAMESPACE::RmtObjectManager *objmgr = get_objmgr();
      if (objmgr != NULL) objmgr->set_evaluate_all(evaluate_all_, evaluate_all_test_);
    }
    inline void set_free_on_exit(bool tf)  {
      free_on_exit_ = tf;
      // Tell objmgr whether it does a chip_free_all when Model destroys it
      MODEL_CHIP_NAMESPACE::RmtObjectManager *objmgr = get_objmgr();
      if (objmgr != NULL) objmgr->set_chip_free_all_on_exit(free_on_exit_);
    }
    inline void config_chip(uint8_t pipes_en, uint8_t num_stages,
                            int sku, uint32_t flags) {
      // Takes effect on next Reset()
      RMT_ASSERT(pipes_en != 0);
      pipes_en_ = pipes_en;
      RMT_ASSERT(num_stages > 0);
      num_stages_ = num_stages;
      sku_ = sku;
      flags_ = flags;
    }
    void set_dv_mode();
    void set_dv_test(int XX);

    inline void set_pfe_mode(bool tf) {
      // This func now redundant - use set_dv_test instead
    }

    void finish_test(int arg=0);


    // Random number helpers
    uint64_t mmix_rand64(uint64_t v);
    uint64_t xrand64(uint64_t v, uint64_t i, uint8_t width=64);
    uint32_t xrand32(uint64_t v, uint64_t i, uint8_t width=32);
    uint16_t xrand16(uint64_t v, uint64_t i, uint8_t width=16);
    uint8_t  xrand8 (uint64_t v, uint64_t i, uint8_t width=8);
    bool     xrandbool(uint64_t v, uint64_t i);
    int      xrandrange(uint64_t v, uint64_t i, int min, int max);

    // Debug
    inline void update_log_flags(uint64_t pipes, uint64_t stages,
                                 uint64_t types, uint64_t rows_tabs, uint64_t cols,
                                 uint64_t or_log_flags, uint64_t and_log_flags) {
      MODEL_CHIP_NAMESPACE::RmtObjectManager *objmgr = get_objmgr();
      if (objmgr == NULL) return;
      objmgr->update_log_flags(pipes, stages, types, rows_tabs, cols,
                                or_log_flags, and_log_flags);
    }
    inline void update_log_flags(uint64_t stages,
                                 uint64_t types, uint64_t rows_tabs, uint64_t cols,
                                 uint64_t or_log_flags, uint64_t and_log_flags) {
      update_log_flags((1 << pipe_), stages, types, rows_tabs, cols,
                       or_log_flags, and_log_flags);
    }
    inline void update_log_flags(uint64_t types, uint64_t rows_tabs, uint64_t cols,
                                 uint64_t or_log_flags, uint64_t and_log_flags) {
      update_log_flags((1 << pipe_), (1 << stage_), types, rows_tabs, cols,
                       or_log_flags, and_log_flags);
    }
    inline void quieten_log_flags() {
      uint64_t ALL = UINT64_C(0xFFFFFFFFFFFFFFFF);
      uint64_t FEW = UINT64_C(0x7); // FatalErrorWarn
      update_log_flags(ALL, ALL, ALL, ALL, ALL, FEW, FEW);
    }
    void quieten_p4_log_flags(uint64_t pipes=ALL);


    // Initialize etc
    void Reset();
    // This func now redundant
    void chip_init_all();


    // Basic read/write reg funcs
    //
    inline uint32_t InWord(uint32_t addr, const char *s=NULL) {
      if (debug_) {
        if (s != NULL)
          printf("TestUtil:::InWord(0x%08X<%s>)=", addr, s);
        else
          printf("TestUtil:::InWord(0x%08X)=", addr);
        fflush(stdout);
      }
      uint32_t val = model_->InWord(chip_, addr);
      if (debug_) printf("%d[0x%08x]\n", val, val);
      return val;
    }
    inline void OutWord(uint32_t addr, uint32_t data, const char *s=NULL) {
      if (debug_) {
        if (s != NULL)
          printf("TestUtil::OutWord(0x%08X<%s>,%d[0x%08x])", addr, s, data, data);
        else
          printf("TestUtil::OutWord(0x%08X,%d[0x%08x])", addr, data, data);
        fflush(stdout);
      }
      model_->OutWord(chip_, addr, data);
      if (debug_) printf(" OK\n");
    }
    virtual uint32_t InWord(volatile void *addr, const char *s=NULL) {
      uint64_t addr64 = reinterpret_cast<uint64_t>(addr);
      assert(addr64 <= UINT64_C(0xFFFFFFFF));
      return InWord(static_cast<uint32_t>(addr64 & UINT64_C(0xFFFFFFFF)), s);
    }
    virtual void OutWord(volatile void *addr, uint32_t data, const char *s=NULL) {
      uint64_t addr64 = reinterpret_cast<uint64_t>(addr);
      assert(addr64 <= UINT64_C(0xFFFFFFFF));
      OutWord(static_cast<uint32_t>(addr64 & UINT64_C(0xFFFFFFFF)), data, s);
    }


    // Special OutWords to handle OutWord to phv_ingress|egress_thread
    // Need to do divert these via set_phv_ingress_egress func such that
    // new flavour <hi|lo,14 groups of 8 phvs> thread programming can occur
    inline void OutWordPieT(int repl, int grp, volatile void *addr, uint32_t data, bool ing) {
      if (repl != 0) return; // If not replica 0 ignore
      for (int i = 0; i < 32; i++) {
        if ((data & (1u<<i)) != 0u) {
          set_phv_ingress_egress(pipe_, stage_, (grp*32)+i, ing);
        }
      }
    }
    inline void OutWordPiT(int repl, int grp, volatile void *addr, uint32_t data) {
      OutWordPieT(repl, grp, addr, data, true);
    }
    inline void OutWordPeT(int repl, int grp, volatile void *addr, uint32_t data) {
      OutWordPieT(repl, grp, addr, data, false);
    }




    // These next two can configurably (use_ind_regs_) use PipeAddressBus
    // OR CPU_IND regs to perform read/write
    virtual void PipeBusRead(uint64_t addr, uint64_t *data0, uint64_t *data1);
    virtual void PipeBusWrite(uint64_t addr, uint64_t data0, uint64_t data1);

    inline void IndirectRead(uint64_t addr, uint64_t *data0, uint64_t *data1) {
      PipeBusRead(addr, data0, data1);
    }
    inline void IndirectWrite(uint64_t addr, uint64_t data0, uint64_t data1) {
      PipeBusWrite(addr, data0, data1);
    }
    inline void IndirectWrite(uint64_t addr, uint64_t data0, uint64_t data1, uint64_t T) {
      // this version can't do use_ind_regs_
      model_->IndirectWrite(chip_, addr, data0, data1, T);
    }
    // Call IndirectWrite but remap field in addr from val_old to val_new
    void RemapIndirectWrite(int field_pos, int field_width,
                            uint32_t val_old, uint32_t val_new,
                            uint64_t addr, uint64_t data0, uint64_t data1);



    // Create addresses for SRAMs, read/write SRAMs
    //
    inline uint64_t make_phys_addr(int pipe, int stage, int memType,
                                   int row, int col, int index) {
      if (pipe < 0) pipe = pipe_;
      if (stage < 0) stage = stage_;
      return make_physical_address(pipe, stage, memType, row, col, index);
    }

    inline uint64_t make_sram_addr(int pipe, int stage, int row, int col, int index) {
      return make_phys_addr(pipe, stage, kPhysMemTypeSRAM, row, col, index);
    }
    inline uint64_t make_mapram_addr(int pipe, int stage, int row, int col, int index) {
      return make_phys_addr(pipe, stage, kPhysMemTypeMapRAM, row, col, index);
    }
    inline uint64_t make_tcam_addr(int pipe, int stage, int row, int col, int index) {
      return make_phys_addr(pipe, stage, kPhysMemTypeTCAM, row, col, index);
    }


    // Read/write SRAMs
    //
    inline void sram_read(int pipe, int stage, int row, int col, int index,
                          uint64_t *data0, uint64_t *data1) {
      PipeBusRead(make_sram_addr(pipe,stage,row,col,index), data0, data1);
    }
    inline void sram_write(int pipe, int stage, int row, int col, int index,
                           uint64_t data0, uint64_t data1) {
      PipeBusWrite(make_sram_addr(pipe,stage,row,col,index), data0, data1);
    }
    inline void sram_read(int stage, int row, int col, int index,
                          uint64_t *data0, uint64_t *data1) {
      sram_read(pipe_, stage, row, col, index, data0, data1);
    }
    inline void sram_write(int stage, int row, int col, int index,
                           uint64_t data0, uint64_t data1) {
      sram_write(pipe_, stage, row, col, index, data0, data1);
    }
    inline void sram_read(int row, int col, int index,
                          uint64_t *data0, uint64_t *data1) {
      sram_read(pipe_, stage_, row, col, index, data0, data1);
    }
    inline void sram_write(int row, int col, int index,
                           uint64_t data0, uint64_t data1) {
      sram_write(pipe_, stage_, row, col, index, data0, data1);
    }

    // Read/write MapRAMs
    //
    inline void mapram_read(int pipe, int stage, int row, int col, int index,
                            uint64_t *data0, uint64_t *data1) {
      PipeBusRead(make_mapram_addr(pipe,stage,row,col,index), data0, data1);
    }
    inline void mapram_write(int pipe, int stage, int row, int col, int index,
                             uint64_t data0, uint64_t data1) {
      PipeBusWrite(make_mapram_addr(pipe,stage,row,col,index), data0, data1);
    }
    inline void mapram_read(int stage, int row, int col, int index,
                            uint64_t *data0, uint64_t *data1) {
      mapram_read(pipe_, stage, row, col, index, data0, data1);
    }
    inline void mapram_write(int stage, int row, int col, int index,
                             uint64_t data0, uint64_t data1) {
      mapram_write(pipe_, stage, row, col, index, data0, data1);
    }
    inline void mapram_read(int row, int col, int index,
                            uint64_t *data0, uint64_t *data1) {
      mapram_read(pipe_, stage_, row, col, index, data0, data1);
    }
    inline void mapram_write(int row, int col, int index,
                             uint64_t data0, uint64_t data1) {
      mapram_write(pipe_, stage_, row, col, index, data0, data1);
    }
    int color_entry_read(int row, int col, int entry) {
      uint64_t data0, data1;
      mapram_read(pipe_, stage_, row, col, (entry>>2), &data0, &data1);
      return (data0>>((entry%4)*2)) & 3;
    }


    // Create addresses for TCAMs, read/write TCAMs
    //
    // RAW read/write - no MASKING done here at all
    inline void tcam_read_raw(int pipe, int stage, int row, int col, int index,
                              uint64_t *data0, uint64_t *data1) {
      PipeBusRead(make_tcam_addr(pipe,stage,row,col,index), data0, data1);
    }
    inline void tcam_write_raw(int pipe, int stage, int row, int col, int index,
                               uint64_t data0, uint64_t data1) {
      PipeBusWrite(make_tcam_addr(pipe,stage,row,col,index), data0, data1);
    }
    inline void tcam_write_with_writereg(int pipe, int stage, int row, int col, int index,
                                         uint64_t data0, uint64_t data1) {
      MODEL_CHIP_NAMESPACE::RmtObjectManager *objmgr = get_objmgr();
      assert(objmgr != NULL);
      MODEL_CHIP_NAMESPACE::RmtOpHandler *oph = objmgr->op_handler_get();
      int mem = row + (col * kTcamMaxRows);
      oph->set_tcam_writereg(pipe, stage, mem, index, data0, data1);
    }

    // Next two funcs assume tcam is split into data/payload (so you get extra bit)
    inline void tcam_read(int pipe, int stage, int row, int col, int index,
                          uint64_t *data0, uint64_t *data1,
                          uint8_t *payload0, uint8_t *payload1) {
      uint64_t tmp_data0, tmp_data1;
      tcam_read_raw(pipe, stage, row, col, index, &tmp_data0, &tmp_data1);
      if (data0 != NULL) *data0 = (tmp_data0 >> kTcamPayloadBits) & kTcamDataMask;
      if (data1 != NULL) *data1 = (tmp_data1 >> kTcamPayloadBits) & kTcamDataMask;
      if (payload0 != NULL) *payload0 = static_cast<uint8_t>(tmp_data0 & kTcamPayloadMask);
      if (payload1 != NULL) *payload1 = static_cast<uint8_t>(tmp_data1 & kTcamPayloadMask);
    }
    inline void tcam_write(int pipe, int stage, int row, int col, int index,
                           uint64_t data0, uint64_t data1,
                           uint8_t payload0, uint8_t payload1) {
      uint64_t tmp_data0 = ((data0 & kTcamDataMask) << kTcamPayloadBits) |
          (static_cast<uint64_t>(payload0) & kTcamPayloadMask);
      uint64_t tmp_data1 = ((data1 & kTcamDataMask) << kTcamPayloadBits) |
          (static_cast<uint64_t>(payload1) & kTcamPayloadMask);
      tcam_write_raw(pipe, stage, row, col, index, tmp_data0, tmp_data1);
    }

    // Convenience funcs that don't get payload bit and always set it to 0
    inline void tcam_read(int pipe, int stage, int row, int col, int index,
                          uint64_t *data0, uint64_t *data1) {
      tcam_read(pipe, stage, row, col, index, data0, data1, NULL, NULL);
    }
    inline void tcam_write(int pipe, int stage, int row, int col, int index,
                           uint64_t data0, uint64_t data1) {
      tcam_write(pipe, stage, row, col, index, data0, data1, 0, 0);
    }
    // Convenience func that zeroes a particular index
    // NB. data0=0,data1=0 => MISMATCH (unless srch0=0 AND srch1=0)
    inline void tcam_zero(int pipe, int stage, int row, int col, int index) {
      tcam_write(pipe, stage, row, col, index, UINT64_C(0), UINT64_C(0));
    }


    // These 2 assume you're writing value mask so calc data0/data1 appropriately
    inline void tcam_write_value_mask(int pipe, int stage, int row, int col, int index,
                                      uint64_t value, uint64_t mask) {
      uint64_t data1 = (value & mask) | ~mask;
      uint64_t data0 = (~value & mask) | ~mask;
      tcam_write(pipe, stage, row, col, index, data0, data1);
    }
    inline void tcam_write_value_mask(int pipe, int stage, int row, int col, int index,
                                      uint64_t value, uint64_t mask,
                                      uint8_t payload0, uint8_t payload1) {
      uint64_t data1 = (value & mask) | ~mask;
      uint64_t data0 = (~value & mask) | ~mask;
      tcam_write(pipe, stage, row, col, index, data0, data1, payload0, payload1);
    }

    // Use stored val for pipe
    inline void tcam_read(int stage, int row, int col, int index,
                          uint64_t *data0, uint64_t *data1) {
      tcam_read(pipe_, stage, row, col, index, data0, data1);
    }
    inline void tcam_read(int stage, int row, int col, int index,
                          uint64_t *data0, uint64_t *data1, uint8_t *pld0, uint8_t *pld1) {
      tcam_read(pipe_, stage, row, col, index, data0, data1, pld0, pld1);
    }
    inline void tcam_write(int stage, int row, int col, int index,
                           uint64_t data0, uint64_t data1) {
      tcam_write(pipe_, stage, row, col, index, data0, data1);
    }
    inline void tcam_zero(int stage, int row, int col, int index) {
      tcam_zero(pipe_, stage, row, col, index);
    }
    inline void tcam_write(int stage, int row, int col, int index,
                           uint64_t data0, uint64_t data1, uint8_t pld0, uint8_t pld1) {
      tcam_write(pipe_, stage, row, col, index, data0, data1, pld0, pld1);
    }
    inline void tcam_write_value_mask(int stage, int row, int col, int index,
                                      uint64_t value, uint64_t mask) {
      tcam_write_value_mask(pipe_, stage, row, col, index, value, mask);
    }
    inline void tcam_write_value_mask(int stage, int row, int col, int index,
                                      uint64_t value, uint64_t mask,
                                      uint8_t pld0, uint8_t pld1) {
      tcam_write_value_mask(pipe_, stage, row, col, index, value, mask, pld0, pld1);
    }

    // Use stored val for both pipe and stage
    inline void tcam_read(int row, int col, int index,
                          uint64_t *data0, uint64_t *data1) {
      tcam_read(pipe_, stage_, row, col, index, data0, data1);
    }
    inline void tcam_read(int row, int col, int index,
                          uint64_t *data0, uint64_t *data1, uint8_t *pld0, uint8_t *pld1) {
      tcam_read(pipe_, stage_, row, col, index, data0, data1, pld0, pld1);
    }
    inline void tcam_write(int row, int col, int index,
                           uint64_t data0, uint64_t data1) {
      tcam_write(pipe_, stage_, row, col, index, data0, data1);
    }
    inline void tcam_zero(int row, int col, int index) {
      tcam_zero(pipe_, stage_, row, col, index);
    }
    inline void tcam_write(int row, int col, int index,
                           uint64_t data0, uint64_t data1, uint8_t pld0, uint8_t pld1) {
      tcam_write(pipe_, stage_, row, col, index, data0, data1, pld0, pld1);
    }
    inline void tcam_write_value_mask(int row, int col, int index,
                                      uint64_t value, uint64_t mask) {
      tcam_write_value_mask(pipe_, stage_, row, col, index, value, mask);
    }
    inline void tcam_write_value_mask(int row, int col, int index,
                                      uint64_t value, uint64_t mask,
                                      uint8_t pld0, uint8_t pld1) {
      tcam_write_value_mask(pipe_, stage_, row, col, index, value, mask, pld0, pld1);
    }



    // Create virtual addresses for read/write SRAMs (eg STATS rams)
    //
    inline uint64_t make_virt_addr(int pipe, int stage, int memtype,
                                   int logtab, uint32_t vaddr) {
      if (pipe < 0) pipe = pipe_;
      if (stage < 0) stage = stage_;
      return make_virtual_address(pipe, stage, memtype, logtab, vaddr);
    }


    inline void rwram_read(int pipe, int stage, int memtype,
                           int logtab, uint32_t vaddr,
                           uint64_t *data0, uint64_t *data1) {
      PipeBusRead(make_virt_addr(pipe,stage,memtype,logtab,vaddr), data0, data1);
    }
    inline void rwram_write(int pipe, int stage, int memtype,
                            int logtab, uint32_t vaddr,
                            uint64_t data0, uint64_t data1) {
      PipeBusWrite(make_virt_addr(pipe,stage,memtype,logtab,vaddr), data0, data1);
    }
    inline void rwram_read(int stage, int memtype, int logtab, uint32_t vaddr,
                           uint64_t *data0, uint64_t *data1) {
      rwram_read(pipe_, stage, memtype, logtab, vaddr, data0, data1);
    }
    inline void rwram_write(int stage, int memtype, int logtab, uint32_t vaddr,
                            uint64_t data0, uint64_t data1) {
      rwram_write(pipe_, stage, memtype, logtab, vaddr, data0, data1);
    }
    inline void rwram_read(int memtype, int logtab, uint32_t vaddr,
                           uint64_t *data0, uint64_t *data1) {
      rwram_read(pipe_, stage_, memtype, logtab, vaddr, data0, data1);
    }
    inline void rwram_write(int memtype, int logtab, uint32_t vaddr,
                            uint64_t data0, uint64_t data1) {
      rwram_write(pipe_, stage_, memtype, logtab, vaddr, data0, data1);
    }




    // Set phv_ingress_thread/phv_egress_thread
    //
    void set_phv_ingress_egress(int pipe, int stage, int phv_word, bool ingress);

    inline void set_phv_ingress_egress(int stage, int phv_word, bool ingress) {
      set_phv_ingress_egress(pipe_, stage, phv_word, ingress);
    }
    inline void set_phv_ingress_egress(int phv_word, bool ingress) {
      set_phv_ingress_egress(pipe_, stage_, phv_word, ingress);
    }


    // Set a range of PHVs to ingress/egress
    //
    void set_phv_range(int pipe, int stage, int phv_start, int phv_end, bool ingress);

    inline void set_phv_range(int stage, int phv_start, int phv_end, bool ingress) {
      set_phv_range(pipe_, stage, phv_start, phv_end, ingress);
    }
    inline void set_phv_range(int phv_start, int phv_end, bool ingress) {
      set_phv_range(pipe_, stage_, phv_start, phv_end, ingress);
    }

    void set_phv_range_all(int pipe, int stage, bool ingress);

    inline void set_phv_range_all(int stage, bool ingress) {
      set_phv_range_all(pipe_, stage, ingress);
    }
    inline void set_phv_range_all(bool ingress) {
      set_phv_range_all(pipe_, stage_, ingress);
    }

    void set_phv_ranges(int pipe, int stage,
                        int n0, int n1, int n2, int n3, int n4, int n5, int n6,
                        bool ingress);
    inline void set_phv_ranges(int stage,
                               int n0, int n1, int n2, int n3, int n4, int n5, int n6,
                               bool ingress) {
      set_phv_ranges(pipe_, stage, n0, n1, n2, n3, n4, n5, n6, ingress);
    }
    inline void set_phv_ranges(int n0, int n1, int n2, int n3, int n4, int n5, int n6,
                               bool ingress) {
      set_phv_ranges(pipe_, stage_, n0, n1, n2, n3, n4, n5, n6, ingress);
    }


    // Setup PHVs to match/capture
    void set_phv_match(int pipe, int stage, int phv_word, bool ingress,
                       uint32_t value, uint32_t mask);

    inline void set_phv_match(int stage, int phv_word, bool ingress,
                              uint32_t value, uint32_t mask) {
      set_phv_match(pipe_, stage, phv_word, ingress, value, mask);
    }
    inline void set_phv_match(int phv_word, bool ingress,
                              uint32_t value, uint32_t mask) {
      set_phv_match(pipe_, stage_, phv_word, ingress, value, mask);
    }

    void set_phv_capture(int pipe, int stage, int phv_word, bool ingress);

    inline void set_phv_capture(int stage, int phv_word, bool ingress) {
      set_phv_capture(pipe_, stage, phv_word, ingress);
    }
    inline void set_phv_capture(int phv_word, bool ingress) {
      set_phv_capture(pipe_, stage_, phv_word, ingress);
    }

    // Fish out a captured PHV word
    uint64_t get_phv_capture_word(int pipe, int stage, int phv_word);

    inline uint64_t get_phv_capture_word(int stage, int phv_word) {
      return get_phv_capture_word(pipe_, stage, phv_word);
    }
    inline uint64_t get_phv_capture_word(int phv_word) {
      return get_phv_capture_word(pipe_, stage_, phv_word);
    }

    // Setup for timestamp-based snapshot - next PHV with thread_active will be captured
    void set_capture_timestamp(int pipe, int stage, bool ingress, uint64_t timestamp=UINT64_C(0));

    inline void set_capture_timestamp(int stage, bool ingress, uint64_t timestamp=UINT64_C(0)) {
      set_capture_timestamp(pipe_, stage, ingress, timestamp);
    }
    inline void set_capture_timestamp(bool ingress, uint64_t timestamp=UINT64_C(0)) {
      set_capture_timestamp(pipe_, stage_, ingress, timestamp);
    }

    // Retrieve value of free-running timer
    uint64_t get_capture_timestamp_now(int pipe, int stage, bool ingress);

    inline uint64_t get_capture_timestamp_now(int stage, bool ingress) {
      return get_capture_timestamp_now(pipe_, stage, ingress);
    }
    inline uint64_t get_capture_timestamp_now(bool ingress) {
      return get_capture_timestamp_now(pipe_, stage_, ingress);
    }

    // Setup snapshot FSM - without this there is no match/capture
    int setup_snapshot(int pipe, int stage, bool ingress, int fsm_state);

    inline int setup_snapshot(int stage, bool ingress, int fsm_state) {
      return setup_snapshot(pipe_, stage, ingress, fsm_state);
    }
    inline int setup_snapshot(bool ingress, int fsm_state) {
      return setup_snapshot(pipe_, stage_, ingress, fsm_state);
    }



    // Configure a MAU stage to be concurrent/action-dependent/match-dependent
    //
    static constexpr int kDepConcurrent = 0;
    static constexpr int kDepAction = 1;
    static constexpr int kDepMatch = 2;

    void set_dependency(int pipe, int stage, int dep, bool ingress);

    inline void set_dependency(int stage, int dep, bool ingress) {
      set_dependency(pipe_, stage, dep, ingress);
    }
    inline void set_dependency(int dep, bool ingress) {
      set_dependency(pipe_, stage_, dep, ingress);
    }



    // Configure instruction op-codes
    //
    void imem_config(int pipe, int stage, int phvNum,
                     int opNum, int opColour, uint32_t opInstr);
    inline void imem_config(int stage, int phvNum,
                            int opNum, int opColour, uint32_t opInstr) {
      imem_config(pipe_, stage, phvNum, opNum, opColour, opInstr);
    }
    inline void imem_config(int phvNum,
                            int opNum, int opColour, uint32_t opInstr) {
      imem_config(pipe_, stage_, phvNum, opNum, opColour, opInstr);
    }



    // Configure a Logical Table to be active
    //
    void table_config(int pipe, int stage, int log_table, bool ingress);
    inline void table_config(int stage, int log_table, bool ingress) {
      table_config(pipe_, stage, log_table, ingress);
    }
    inline void table_config(int log_table, bool ingress) {
      table_config(pipe_, stage_, log_table, ingress);
    }


    // Configure a Physical Result Bus OR Logical Table
    // to have certain shift/mask/dflt/miss settings
    //
    void physbus_config(int pipe, int stage, int log_table, int bus_num,
                        uint8_t nxtab_shift, uint16_t nxtab_mask,
                        uint16_t nxtab_dflt, uint16_t nxtab_miss,
                        uint8_t instr_shift, uint8_t instr_mask,
                        uint8_t instr_dflt, uint8_t instr_miss,
                        uint32_t data_shift, uint32_t data_mask,
                        uint32_t data_dflt, uint32_t data_miss);

    inline void physbus_config(int stage, int log_table, int bus_num,
                             uint8_t nxtab_shift, uint8_t nxtab_mask,
                             uint8_t nxtab_dflt, uint8_t nxtab_miss,
                             uint8_t instr_shift, uint8_t instr_mask,
                             uint8_t instr_dflt, uint8_t instr_miss,
                             uint32_t data_shift, uint32_t data_mask,
                             uint32_t data_dflt, uint32_t data_miss) {
      physbus_config(pipe_, stage, log_table, bus_num,
                     nxtab_shift, nxtab_mask, nxtab_dflt, nxtab_miss,
                     instr_shift, instr_mask, instr_dflt, instr_miss,
                     data_shift, data_mask, data_dflt, data_miss);
    }
    inline void physbus_config(int log_table, int bus_num,
                             uint8_t nxtab_shift, uint8_t nxtab_mask,
                             uint8_t nxtab_dflt, uint8_t nxtab_miss,
                             uint8_t instr_shift, uint8_t instr_mask,
                             uint8_t instr_dflt, uint8_t instr_miss,
                             uint32_t data_shift, uint32_t data_mask,
                             uint32_t data_dflt, uint32_t data_miss) {
      physbus_config(pipe_, stage_, log_table, bus_num,
                     nxtab_shift, nxtab_mask, nxtab_dflt, nxtab_miss,
                     instr_shift, instr_mask, instr_dflt, instr_miss,
                     data_shift, data_mask, data_dflt, data_miss);
    }


    // Configure a Logical TCAM to refer to a specific
    // LogicalTable and PhysicalResultBus
    //
    void ltcam_config(int pipe, int stage, int log_tcam,
                      int log_table, int phys_bus, uint8_t match_addr_shift);

    inline void ltcam_config(int stage, int log_tcam,
                             int log_table, int phys_bus, uint8_t match_addr_shift) {
      ltcam_config(pipe_, stage, log_tcam, log_table, phys_bus, match_addr_shift);
    }
    inline void ltcam_config(int log_tcam,
                             int log_table, int phys_bus, uint8_t match_addr_shift) {
      ltcam_config(pipe_, stage_, log_tcam, log_table, phys_bus, match_addr_shift);
    }


    // Configure an SRAM to have a certain type, be attached to a particular
    // bus, have a certain VPN, attach to a specific LogicalTCAM/LogicalTable
    //
    void sram_config(int pipe, int stage, int row, int col,
                     int type, int in_bus, int out_bus,
                     int log_table, int log_tcam, int vpn0, int vpn1, bool egress=false);

    inline void sram_config(int stage, int row, int col, int type, int in_bus, int out_bus,
                            int log_table, int log_tcam, int vpn0, int vpn1, bool egress=false) {
      sram_config(pipe_, stage, row, col, type, in_bus, out_bus,
                  log_table, log_tcam, vpn0, vpn1, egress);
    }
    inline void sram_config(int row, int col, int type, int in_bus, int out_bus,
                            int log_table, int log_tcam, int vpn0, int vpn1, bool egress=false) {
      sram_config(pipe_, stage_, row, col, type, in_bus, out_bus,
                  log_table, log_tcam, vpn0, vpn1, egress);
    }


    // Configure a Stats/Meter RAM - includes basic AdrDist/RamFabric config
    //
    void meter_config(int pipe, int stage, int row,
                      bool egress,bool red, bool lpf,
                      bool byte,int byte_count_adjust );
    void rwram_config(int pipe, int stage, int row, int col,
                      int type, int vpn0, int vpn1,
                      int s_format, int log_table, bool egress,
                      bool use_deferred_rams);

    inline void rwram_config(int stage, int row, int col,
                             int type, int vpn0, int vpn1,
                             int s_format, int log_table, bool egress,
                             bool use_deferred_rams) {
      rwram_config(pipe_, stage, row, col, type, vpn0, vpn1,
                   s_format, log_table, egress,use_deferred_rams);
    }
    inline void rwram_config(int row, int col,
                             int type, int vpn0, int vpn1,
                             int s_format, int log_table, bool egress,
                             bool use_deferred_rams) {
      rwram_config(pipe_, stage_, row, col, type, vpn0, vpn1,
                   s_format, log_table, egress,use_deferred_rams);
    }

    // Configure UnitRam address muxes
    void mux_config(int pipe, int stage, int row, int col,
                    int mux_inp, int rd_bus, int wr_bus, int log_table,
                    bool oflo, int src_idx, int src_sel,
                    bool oflo2);

    inline void mux_config(int stage, int row, int col,
                           int mux_inp, int rd_bus, int wr_bus, int log_table,
                           bool oflo, int src_idx, int src_sel,
                           bool oflo2) {
      mux_config(pipe_, stage, row, col, mux_inp, rd_bus, wr_bus, log_table,
                 oflo, src_idx, src_sel, oflo2);
    }
    inline void mux_config(int row, int col,
                           int mux_inp, int rd_bus, int wr_bus, int log_table,
                           bool oflo, int src_idx, int src_sel,
                           bool oflo2) {
      mux_config(pipe_, stage_, row, col, mux_inp, rd_bus, wr_bus, log_table,
                 oflo, src_idx, src_sel, oflo2);
    }



    // Configure a MapRAM
    //
    void mapram_config(int pipe, int stage, int row, int col,
                       int type, int vpn0, int vpn1,
                       int m_flags, int log_table, bool egress=false);

    inline void mapram_config(int stage, int row, int col,
                              int type, int vpn0, int vpn1,
                              int m_flags, int log_table, bool egress=false) {
      mapram_config(pipe_, stage, row, col, type, vpn0, vpn1,
                   m_flags, log_table, egress);
    }
    inline void mapram_config(int row, int col,
                              int type, int vpn0, int vpn1,
                              int m_flags, int log_table, bool egress=false) {
      mapram_config(pipe_, stage_, row, col, type, vpn0, vpn1,
                   m_flags, log_table, egress);
    }



    // Configure a TCAM to use particular bus, have certain priority (vpn)
    // be ingress/egress, use chain_out, output results etc
    //
    void tcam_config(int pipe, int stage, int row, int col,
                     int in_bus, int log_table, int log_tcam, int vpn,
                     bool ingress, bool chain, bool output, int head);

    inline void tcam_config(int stage, int row, int col,
                            int in_bus, int log_table, int log_tcam, int vpn,
                            bool ingress, bool chain, bool output, int head) {
      tcam_config(pipe_, stage, row, col, in_bus, log_table, log_tcam,
                  vpn, ingress, chain, output, head);
    }

    inline void tcam_config(int row, int col,
                            int in_bus, int log_table, int log_tcam, int vpn,
                            bool ingress, bool chain, bool output, int head) {
      tcam_config(pipe_, stage_, row, col, in_bus, log_table, log_tcam,
                  vpn, ingress, chain, output, head);
    }


    // Configure TableCounter for a particular LogicalTable
    void table_cntr_config(int pipe, int stage, int log_table, int cnt_what);


    inline void table_cntr_config(int stage, int log_table, int cnt_what) {
      table_cntr_config(pipe_, stage, log_table, cnt_what);
    }

    inline void table_cntr_config(int log_table, int cnt_what) {
      table_cntr_config(pipe_, stage_, log_table, cnt_what);
    }


    // Configure StatefulLogCounter for a particular LogicalTable with a
    // given shift (to apply to counter before ORing with address) and
    // a given VPN range
    //
    void stateful_cntr_config(int pipe, int stage, int log_table, int alu,
                              int cnt_what, int cntr_shift, int vpn_min, int vpn_max);


    // functions to hide register differences between chips
    void set_stateful_log_instruction_regs( Mau_match_merge_addrmap& mm_regs, int alu,
                                            int width, int vpn_offset, int vpn_limit );
    void set_stateful_clear_instruction_regs( Mau_match_merge_addrmap& mm_regs, int alu,
                                              int width, int vpn_offset, int vpn_limit );

    inline void stateful_cntr_config(int stage, int log_table, int alu,
                                     int cnt_what, int cntr_shift, int vpn_min, int vpn_max) {
      stateful_cntr_config(pipe_, stage, log_table, alu,
                           cnt_what, cntr_shift, vpn_min, vpn_max);
    }

    inline void stateful_cntr_config(int log_table, int alu,
                                     int cnt_what, int cntr_shift, int vpn_min, int vpn_max) {
      stateful_cntr_config(pipe_, stage_, log_table, alu,
                           cnt_what, cntr_shift, vpn_min, vpn_max);
    }


    // Setup MAU dump ctl regs - PER-CHIP
    void set_dump_ctl_regs(int pipe, int stage);

    inline void set_dump_ctl_regs(int stage) {
      set_dump_ctl_regs(pipe_, stage);
    }
    inline void set_dump_ctl_regs() {
      set_dump_ctl_regs(pipe_, stage_);
    }


    // Setup MAU default regs (eg pred_stage_id) - PER-CHIP
    void set_stage_default_regs(int pipe, int stage);

    inline void set_stage_default_regs(int stage) {
      set_stage_default_regs(pipe_, stage);
    }
    inline void set_stage_default_regs() {
      set_stage_default_regs(pipe_, stage_);
    }


    // Setup LT default regs (eg pred_is_a_brch) - PER-CHIP
    void set_table_default_regs(int pipe, int stage, int table);

    inline void set_table_default_regs(int stage, int table) {
      set_table_default_regs(pipe_, stage, table);
    }
    inline void set_table_default_regs(int table) {
      set_table_default_regs(pipe_, stage_, table);
    }


    // Setup TEOP regs - PER-CHIP
    void set_teop_regs(int pipe, int stage, int table, int alu, int bus);

    inline void set_teop_regs(int stage, int table, int alu, int bus) {
      set_teop_regs(pipe_, stage, table, alu, bus);
    }
    inline void set_teop_regs(int table, int alu, int bus) {
      set_teop_regs(pipe_, stage_, table, alu, bus);
    }


    // Setup MIRROR regs - PER-CHIP
    void set_mirror_global(int pipe,
                           bool ing_en, bool egr_en,
                           bool coal_ing_en, bool coal_egr_en,
                           uint16_t coal_baseid, uint32_t coal_basetime);
    void set_mirror_meta(int pipe, int sess,
                         uint32_t egr_port, uint32_t egr_port_v,
                         uint32_t eport_qid, uint32_t icos,
                         uint32_t pipe_mask, uint32_t color,
                         uint32_t hash1, uint32_t hash2,
                         uint32_t mgid1, uint32_t mgid1_v,
                         uint32_t mgid2, uint32_t mgid2_v,
                         uint32_t c2c_v, uint32_t c2c_cos,
                         uint32_t xid, uint32_t yid, uint32_t rid,
                         uint32_t egress_bypass, uint32_t yid_tbl_sel,
                         uint32_t deflect);
    void set_mirror_meta_cfg(int pipe, int sess,
                             bool hash_cfg, bool icos_cfg, bool dod_cfg,
                             bool c2c_cfg, bool mc_cfg, bool epipe_cfg);
    void set_mirror_norm_session(int pipe, int slice, int sess,
                                 bool ing_en, bool egr_en,
                                 bool coal_en, uint8_t coal_num,
                                 uint8_t pri, uint8_t max_n,
                                 uint16_t trunc_size);
    void set_mirror_coal_session(int pipe, int slice, int coal_sess,
                                 bool en, uint8_t ver, uint8_t pri,
                                 uint8_t pkt_hdr_len, uint16_t min_pkt_size,
                                 uint16_t extract_len,
                                 bool len_from_inp, bool tofino_mode,
                                 uint32_t hdr0, uint32_t hdr1,
                                 uint32_t hdr2, uint32_t hdr3,
                                 uint32_t coal_timeout);


    // Setup PACKET-GEN regs - PER-CHIP

    static constexpr int kPgenAppTypeTimer    = 0;
    static constexpr int kPgenAppTypePeriodic = 1;
    static constexpr int kPgenAppTypeLinkdown = 2;
    static constexpr int kPgenAppTypeRecirc   = 3;
    static constexpr int kPgenAppTypeDprsr    = 4;
    static constexpr int kPgenAppTypePfc      = 5;
    static constexpr int kPgenAppTypeMin      = kPgenAppTypeTimer;
    static constexpr int kPgenAppTypeMax      = kPgenAppTypePfc;
    static constexpr int kPgenAppTypeMask     = 0x3F;

    static constexpr int kPgenOutPortTbc      = -2;
    static constexpr int kPgenOutPortEthCpu   = -1;
    static constexpr int kPgenOutPortEbuf0    =  0;
    static constexpr int kPgenOutPortEbuf1    =  1;
    static constexpr int kPgenOutPortEbuf2    =  2;
    static constexpr int kPgenOutPortEbuf3    =  3;
    static constexpr int kPgenOutPortMin      = kPgenOutPortTbc;
    static constexpr int kPgenOutPortMax      = kPgenOutPortEbuf3;

    static constexpr uint8_t kPgenOutPortFlagsTbc         = 0x01;
    static constexpr uint8_t kPgenOutPortFlagsEthCpu      = 0x02;
    static constexpr uint8_t kPgenOutPortFlagsEbuf        = 0x04;
    static constexpr uint8_t kPgenOutPortFlagsAllowReuse  = 0x10;

    static constexpr uint8_t kPgenAppFlagNoKey            = 0x01;
    static constexpr uint8_t kPgenAppFlagStopAtPktBndry   = 0x02;
    static constexpr uint8_t kPgenAppFlagUsePortDownMask1 = 0x04;
    static constexpr uint8_t kPgenAppFlagUseCurrTs        = 0x08;
    static constexpr uint8_t kPgenAppFlagAddrSizeFromPkt  = 0x10;
    static constexpr uint8_t kPgenAppFlagLimitPktsBatches = 0x20;
    static constexpr uint8_t kPgenAppFlagLimitPktsPackets = 0x40;
    static constexpr uint8_t kPgenAppFlagMask             = 0x7F;

    static constexpr uint8_t  kPgenAppChanMin      = 0;
    static constexpr uint8_t  kPgenAppChanMax      = 7;
    static constexpr uint32_t kPgenAppIbgJitterMax = 255u*31;    // 7905
    static constexpr uint32_t kPgenAppIpgJitterMax = 255u*31/5;  // 1580
    static constexpr uint32_t kPgenAppIpgMinLo     = 10;

    void pktgen_app_ctrl_set(int pipe, int app,
                             bool en, uint8_t type, uint8_t chan, uint8_t prio, uint8_t flg);
    void pktgen_app_payload_ctrl_set(int pipe, int app,
                                     uint16_t addr_16B, uint16_t size_B, bool from_recirc);
    void pktgen_app_ingr_port_ctrl_set(int pipe, int app,
                                       uint8_t ing_port, bool ing_inc, uint8_t ing_wrap,
                                       uint8_t ing_port_pipe_id);
    void pktgen_app_recirc_set(int pipe, int app, int outport,
                               const MODEL_CHIP_NAMESPACE::BitVector<128> &value,
                               const MODEL_CHIP_NAMESPACE::BitVector<128> &mask);
    void pktgen_app_event_set(int pipe, int app, uint16_t batchnum, uint16_t pktnum,
                              uint32_t ibg_jit_base, uint8_t ibg_jit_max, uint8_t ibg_jit_scl,
                              uint32_t ipg_jit_base, uint8_t ipg_jit_max, uint8_t ipg_jit_scl,
                              uint32_t timer_cycles);
    void pktgen_app_counters(int pipe, int app);

    // NB Here output is WRT pipe - so from EBUF (but also from TBC/PCIe and EthCPU Rx)
    void pktgen_cmn_output_port_ctrl_set(int pipe, int outport,
                                         bool en, uint8_t chan_en, uint8_t chan_mode,
                                         uint64_t chan_seq=UINT64_C(0));
    // NB Here input is WRT pipe - so to IPB (but also to TBC/PCIe and EthCPU Tx)
    void pktgen_cmn_input_port_ctrl_set(int pipe,
                                        uint32_t recirc_chan_en, uint32_t pgen_chan_en,
                                        uint64_t chan_seq=UINT64_C(0));
    void pktgen_cmn_timestamp_set(int pipe, int outport,
                                  uint32_t recirc_ts_off, uint32_t csr_ts_off);
    void pktgen_cmn_portdown_ctrl_set(int pipe,
                                      const MODEL_CHIP_NAMESPACE::BitVector<72> &mask0,
                                      const MODEL_CHIP_NAMESPACE::BitVector<72> &mask1);
    MODEL_CHIP_NAMESPACE::BitVector<72> pktgen_cmn_portdown_ctrl_get(int pipe, int mask);

    void pktgen_cmn_portdown_disable_set(int pipe,
                                         const MODEL_CHIP_NAMESPACE::BitVector<72> &disable);
    bool pktgen_cmn_portdown_disable_get(int pipe,int port);

  void pktgen_port_down_vec_clr_set(int pipe,
                                    bool en, bool app_disable, bool set_sent);

    void pktgen_cmn_pfc_xoff_ctrl_set(int pipe);
    void pktgen_cmn_credit_dwrr_ctrl_set(int pipe);
    // These to setup PACKET-GEN memories
    void pktgen_mem_pkt_set(int pipe, int i, uint64_t data0, uint64_t data1);
    void pktgen_mem_meta1_set(int pipe, int i, uint64_t data0, uint64_t data1);




    // Create/free a packet (uses Crafter lib)
    //
    static constexpr int kProtoTCP = 0;
    static constexpr int kProtoUDP = 1;
    static constexpr int kProtoIP  = 2;

    MODEL_CHIP_NAMESPACE::Packet *packet_make(const char *da, const char *sa,
                                              const char *sip, const char *dip,
                                              int proto, int sport, int dport,
                                              const char *payload=NULL);
    MODEL_CHIP_NAMESPACE::Packet *packet_make(uint64_t seed, uint16_t len);
    MODEL_CHIP_NAMESPACE::Packet *packet_make(uint64_t seed, uint16_t pktlen,
                                              uint8_t *hdr, uint16_t hdrlen, uint16_t tailzeros=0);
    void packet_free(MODEL_CHIP_NAMESPACE::Packet *p);
    void packet_free(MODEL_CHIP_NAMESPACE::Packet *pin, MODEL_CHIP_NAMESPACE::Packet *pout);
    MODEL_CHIP_NAMESPACE::Phv *phv_alloc();
    void phv_free(MODEL_CHIP_NAMESPACE::Phv *phv);


    // Get and configure a port, and send a packet on it
    //
    MODEL_CHIP_NAMESPACE::Port   *port_get(int index);
    MODEL_CHIP_NAMESPACE::Packet *port_process_inbound(MODEL_CHIP_NAMESPACE::Port *port,
                                                       MODEL_CHIP_NAMESPACE::Packet *p);
    MODEL_CHIP_NAMESPACE::Packet *port_process_outbound(MODEL_CHIP_NAMESPACE::Port *port,
                                                        MODEL_CHIP_NAMESPACE::Packet *p);
    MODEL_CHIP_NAMESPACE::Phv    *port_process_inbound(MODEL_CHIP_NAMESPACE::Port *port,
                                                       MODEL_CHIP_NAMESPACE::Phv *phv);
    MODEL_CHIP_NAMESPACE::Phv    *port_process_inbound(MODEL_CHIP_NAMESPACE::Port *port,
                                                       MODEL_CHIP_NAMESPACE::Phv *phv,
                                                       MODEL_CHIP_NAMESPACE::Phv *ophv);
    MODEL_CHIP_NAMESPACE::Phv    *port_parse(MODEL_CHIP_NAMESPACE::Port *port,
                                             MODEL_CHIP_NAMESPACE::Packet *p);
    void handle_eop(MODEL_CHIP_NAMESPACE::Port *port, const MODEL_CHIP_NAMESPACE::Eop &eop);



    // Read value from specific types of MacCounter using counter address within MAC
    uint64_t mac_cntr_read_vmac_c3(uint32_t mac_cntr_base_addr, int chan, int cntr_index, bool clear);
    uint64_t mac_cntr_read_vmac_c4(uint32_t mac_cntr_base_addr, int chan, int cntr_index, bool clear);
    uint64_t mac_cntr_read_vmac_t1(uint32_t mac_cntr_base_addr, int chan, int cntr_index, bool clear);
    // Read value from specific types of MacCounter based on Port
    uint64_t mac_cntr_read_vmac_c3(int port_index, int cntr_index, bool clear=false);
    uint64_t mac_cntr_read_vmac_c4(int port_index, int cntr_index, bool clear=false);
    uint64_t mac_cntr_read_vmac_t1(int port_index, int cntr_index, bool clear=false);
    uint64_t mac_cntr_read(int port_index, int cntr_index, bool clear);



#ifdef MODEL_CHIP_JBAY_OR_LATER
    // TODO:JBAY: Fix
    void deparser_init(MODEL_CHIP_NAMESPACE::Deparser *dp);
    // backwards compatible to get Tofino tests working
    void deparser_set_field_dictionary_entry(int pipe, int index, int valid,
                                             int phv0, int phv1, int phv2, int phv3,
                                             int pov_sel, int num_bytes,int version);
    void deparser_set_field_dictionary_entry(int pipe, int index, int valid, int pov, bool seg_vld,
                                             int seg_sel, int seg_slice,
                                             int phvs[8], int num_bytes, int is_phv);

    void deparser_set_egress_unicast_port_info(int pipe, int phv, int pov, bool disable);
    void deparser_set_copy_to_cpu_info( int pipe, int phv, int pov, bool disable, int shift );
    void deparser_set_ct_disable_info( int pipe, int phv, int pov, bool disable, int shift );
    void deparser_set_ct_mcast_info( int pipe, int phv, int pov, bool disable, int shift );
    void deparser_set_deflect_on_drop_info(int pipe, int phv, int pov, bool disable, int shift );
    void deparser_set_drop_ctl_info(int pipe, int phv, int pov, bool disable, int shift );

    void deparser_set_capture_tx_ts_info(int pipe, int phv, int pov, bool disable, int shift);
    void deparser_set_force_tx_error_info(int pipe, int phv, int pov, bool disable, int shift);
    void deparser_set_tx_pkt_has_offsets_info(int pipe, int phv, int pov, bool disable, int shift);

    void deparser_set_mtu_trunc_len_info( int pipe, int phv, int pov, bool disable, int shift );
    void deparser_set_mtu_trunc_err_f_info( int pipe, int phv, int pov, bool disable, int shift );

    uint32_t deparser_create_info_word(int phv, int pov, bool disable,int shift=0);
    void deparser_set_csum_row_entry(int pipe, int csum_idx, int csum_entry_idx,
                                     bool swap, bool zero_m_s_b, bool zero_l_s_b,
                                     int pov);
    void set_multicast_pipe_vector(int pipe_index,
                                   int table_num,
                                   int mgid,
                                   uint16_t val);
    void set_dprsr_csum_invert(int pipe, int csum_eng, int bit);
    void set_dprsr_clot_csum_invert(int pipe, int csum_eng, int clot_num);
    void set_dprsr_phv_csum_invert(int pipe, int csum_eng, int phv_num);

#else
    // Deparser config
    //
    uint32_t deparser_create_info_word(int phv, bool valid,
                                       uint16_t default_value,
                                       uint32_t default_value_offset=16,
                                       const uint8_t &shift = 0,
                                       const uint8_t &shift_offset=0);
    uint32_t deparser_create_info_word(int phv, bool valid, bool default_valid,
                                       uint16_t default_value,
                                       uint32_t default_value_offset=16,
                                       const uint8_t &shift = 0,
                                       const uint8_t &shift_offset=0);
    void deparser_set_capture_tx_ts_info(int pipe, int phv, bool valid, bool default_value);
    void deparser_set_copy_to_cpu_info(int pipe, int phv, bool valid,
                                       uint16_t default_value,
                                       const uint8_t &shift=0);
    void deparser_set_copy_to_cpu_cos_info(int pipe, int phv, bool valid, uint16_t default_value);
    void deparser_set_ct_disable_info(int pipe, int phv, bool valid, uint16_t default_value);
    void deparser_set_ct_mcast_info(int pipe, int phv, bool valid, uint16_t default_value);
    void deparser_set_deflect_on_drop_info(int pipe, int phv, bool valid, uint16_t default_value);
    void deparser_set_drop_ctl_info(int pipe, int phv, bool valid, uint16_t default_value, bool is_ingress=true);
    void deparser_set_ecos_info(int pipe, int phv, bool valid, uint16_t default_value);
    void deparser_set_egress_unicast_port_info(int pipe, int phv, bool valid, bool default_valid, uint16_t default_value);
    void deparser_set_force_tx_error_info(int pipe, int phv, bool valid, bool default_value);
    void deparser_set_hash1_info(int pipe, int phv, bool valid, uint16_t default_value);
    void deparser_set_hash2_info(int pipe, int phv, bool valid, uint16_t default_value);
    void deparser_set_icos_info(int pipe, int phv, bool valid, uint16_t default_value);
    void deparser_set_meter_color_info(int pipe, int phv, bool valid, uint16_t default_value);
    void deparser_set_mgid1_info(int pipe, int phv, bool valid, bool default_valid, uint16_t default_value);
    void deparser_set_mgid2_info(int pipe, int phv, bool valid, bool default_valid, uint16_t default_value);
    void deparser_set_mirror_cfg(int pipe, bool is_ingress, uint8_t phv, bool valid);
    void deparser_set_mirror_metadata(int pipe, int table_entry_idx, bool is_ingress, uint8_t metadata_phv_idx[48]);
    void deparser_set_mirror_table_entry(int pipe, int table_entry_idx, bool is_ingress, uint8_t id_phv, uint8_t len, bool valid);
    void deparser_set_physical_ingress_port_info(int pipe, uint8_t phv, bool sel);
    void deparser_set_pipe_vector_table(int pipe, int table_num, uint16_t entry, uint8_t pipe_vector);
    void deparser_set_qid_info(int pipe, int phv, bool valid, uint16_t default_value);
    void deparser_set_resubmit_cfg(int pipe, bool valid, uint8_t phv);
    void deparser_set_resubmit_table_entry(int pipe, uint8_t table_idx, bool valid, int len, const std::array<uint8_t, 8> metadata_phv_idx);
    void deparser_set_rid_info(int pipe, int phv, bool valid, uint16_t default_value);
    void deparser_set_tx_pkt_has_offsets_info(int pipe, int phv, bool valid, bool default_value);
    void deparser_set_use_yid_tbl_info(int pipe, uint32_t value);
    void deparser_set_bypass_egr_mode_info(int pipe, int phv, bool valid, uint16_t default_value);
    void deparser_set_xid_info(int pipe, int phv, bool valid, uint16_t default_value);
    void deparser_set_yid_info(int pipe, int phv, bool valid, uint16_t default_value);
    void
    deparser_set_phv_group_config(volatile uint32_t *group_reg,
                                  const int &group, const int &valid);
    void
    deparser_set_phv_split_config(volatile uint32_t *split_reg,
                                  const int &split_offset, const int &valid);
    void deparser_set_field_dictionary_entry(int pipe, int index, int valid,
                                             int phv0, int phv1, int phv2, int phv3,
                                             int pov_sel, int num_bytes,int version);
    void
    deparser_set_csum_cfg_entry(int pipe, int csum_idx, int csum_entry_idx,
                                bool swap, bool zero_m_s_b, bool zero_l_s_b);
    uint32_t
    deparser_get_csum_cfg_entry(int pipe, int csum_idx, int csum_entry_idx);
    void
    deparser_set_tphv_csum_cfg_entry(int pipe, int csum_engine_idx,
                                     int csum_entry_idx, bool swap,
                                     bool zero_msb, bool zero_lsb);
    void deparser_init(MODEL_CHIP_NAMESPACE::Deparser *dp);


    void pktgen_recirc_set(int pipe, bool valid, uint16_t port);
    void pktgen_pgen_set(int pipe, uint16_t port, MODEL_CHIP_NAMESPACE::app_config* a_config,
                         bool* valid, uint8_t sz, bool recir_en);
    void pktgen_mxbar_set(int pipe, bool valid, uint16_t port);
    void pktgen_portdown_en(int pipe, uint16_t port);
    struct pgr_app_counter {
     public:
      uint64_t trig_cnt;
      uint64_t batch_cnt;
      uint64_t pkt_cnt;
     pgr_app_counter():trig_cnt(0), batch_cnt(0), pkt_cnt(0){}
    };
    // Populates these across all applications in this pipe
    void pktgen_get_counters(int pipe, bool* en, pgr_app_counter* cntr);

#endif
    void deparser_set_copy_to_cpu_pipe_vector(int pipe, uint8_t pipe_vector);

    // Program up the learning configuration
    //
    void learn_config(int pipe, bool valid, int phv_for_table_index,
                      // these parameters currently ignored, all entries are
                      //  programmed to pick up the SA
                      int table_entry_index,int length,int phvs[48]);


    uint64_t deparser_get_learn_counter(int pipe);
    int  dru_learn_callback_count() { return dru_learn_callback_count_; }
    void zero_dru_learn_callback_count() { dru_learn_callback_count_=0; }
    void lfltr_clear(int pipe);
    void lfltr_config(int pipe, bool disable, uint32_t timeout);
    Lfltr_rspec *lfltr_reg_get(int pipe);

    auto ing_buf_reg_get(int pipe, int port) -> decltype( MODEL_CHIP_NAMESPACE::RegisterUtils::addr_ingbuf(pipe, port) ) {
      return MODEL_CHIP_NAMESPACE::RegisterUtils::addr_ingbuf(pipe, port);
    }
    auto mirror_buf_reg_get(int pipe) -> decltype( MODEL_CHIP_NAMESPACE::RegisterUtils::addr_mirbuf(pipe) ) {
      return MODEL_CHIP_NAMESPACE::RegisterUtils::addr_mirbuf(pipe);
    }
    //auto ing_buf_reg_get(int pipe, int port);
    //auto mirror_buf_reg_get(int pipe);
    // Mirroring - helper functions for bit manipulations
    void mir_session_set_metadata(uint32_t& meta0, uint32_t& meta1,
                                  uint32_t& meta2, uint32_t& meta3, uint32_t& meta4,
                                  uint32_t egr_port, uint32_t icos, uint32_t pipe_mask,
                                  uint32_t color, uint32_t hash1, uint32_t hash2,
                                  uint32_t mgid1, uint32_t mgid1_v,
                                  uint32_t mgid2, uint32_t mgid2_v,
                                  uint32_t c2c_v, uint32_t c2c_cos, uint32_t deflect
                                  );
    bool read_test_file(const char* file_name) {
      test_reader_.set_pipe(pipe_);
      test_reader_.set_stage(stage_);
      return test_reader_.read_file(file_name);
    }
    bool read_test_dir(const char* dir_name) {
      test_reader_.set_pipe(pipe_);
      test_reader_.set_stage(stage_);
      return test_reader_.read_dir(dir_name);
    }
    //// call these after reading the test file if you want to do your own phv processing
    //
    // get phv eg [io]phv_[in|out]_[n]e[01]
    MODEL_CHIP_NAMESPACE::Phv* get_read_phv( const char c, const std::string s, const int n, int e );
    // get eop _[n]e[01]
    MODEL_CHIP_NAMESPACE::Eop* get_read_eop( int n, int e );
    // process the phv  _[n]e[01] and check results match expected values read from test file
    void process_read_phv_match_and_action( const int n, int e, int *ingress_start_table, int *egress_start_table );

    uint32_t reg_ptr_to_addr(volatile void *addr) {
      RMT_ASSERT(addr != nullptr);
      uint64_t addr64 = reinterpret_cast<uint64_t>(addr);
      assert(addr64 <= UINT64_C(0xFFFFFFFF));
      return static_cast<uint32_t>(addr64 & UINT64_C(0xFFFFFFFF));
    }

    FakeRegister* new_fake_register(volatile void *addr, int width) {
      return new FakeRegister(this, reg_ptr_to_addr(addr), width);
    }

    /**
     * Test that a counter can be read and written, and increments and wraps
     * when applicable.
     * @param addr The counter register's address.
     * @param width The width in bits of the counter.
     * @param incrementer An optional function to increment the counter; if
     *    NULL then the counter's increment and wrap behaviour is not tested.
     * @param counter_name A string used to annotate any test failure messages.
     */
    void check_counter(volatile void *addr,
                       int width,
                       const std::function<void()> &incrementer,
                       std::string counter_name);

    /**
     * Lookup address for given path in register_map
     * @param path A vector of elements of path to a register
     * @return ptr to register
     */
    RegPtr lookup_register_map(std::vector<PathElement> path);

    void parser_update_extract16_type_cnt(MODEL_CHIP_NAMESPACE::Parser *p,
                                          int pi,
                                          uint8_t extract_type,
                                          uint8_t val);
 private:
    TestConfig         config_;
    model_core::Model *model_;
    int                chip_;
    int                pipe_;
    int                stage_;
    uint8_t            pipes_en_;
    uint8_t            num_stages_;
    int                sku_;
    uint32_t           flags_;
    bool               use_ind_regs_;
    bool               debug_;
    bool               evaluate_all_;
    bool               evaluate_all_test_;
    bool               free_on_exit_;
    int                dv_test_;
    int                dv_vintage_;
    static int         dru_learn_callback_count_;

    TestReader test_reader_;


    void set_learn_tbl_entry( volatile Dprsr_learn_table_entry_r& entry,
                              bool valid, int len, int phvs[48]);


};


}
#endif // _UTESTS_TEST_UTIL_
