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

#include <cstdint>
#include <utests/test_util.h>
#include <iostream>
#include <string>
#include <array>
#include <cassert>
#include <random>
#include <map>

#include "gtest.h"

#include <model_core/model.h>
#include <register_includes/reg.h>
#include <bitvector.h>
#include <chip.h>
#include <mau.h>
#include <rmt-object-manager.h>
#include "register_utils.h"
#include "tm-unimplemented.h"

extern std::unique_ptr<model_core::Model> GLOBAL_MODEL;

namespace MODEL_CHIP_TEST_NAMESPACE {

  bool chip_print = false;

  using namespace std;
  using namespace MODEL_CHIP_NAMESPACE;

 class FakeRegisterBlock : public model_core::RegisterBlockBase {
   public:
    using RegisterBlockBase::RegisterBlockBase;
    uint32_t data_ = 0xabcdef01;

    virtual bool read(uint32_t offset, uint32_t* data) const {
      *data = data_;
      return true;
    }
    virtual bool write(uint32_t offset, uint32_t data) {
      data_ = data;
      return true;
    }
    virtual std::string to_string(bool print_zeros = false, std::string indent_string = "") const {
      return std::string("fake register block");
    };
    virtual std::string to_string(uint32_t offset,bool print_zeros = false, std::string indent_string = "") const {
      return to_string(print_zeros, indent_string);
    }
  };
  class FakeRegisterBlockIndirect : public model_core::RegisterBlockIndirectBase {
   public:
    using RegisterBlockIndirectBase::RegisterBlockIndirectBase;
    uint64_t data0_ = 0xabcdef01;
    uint64_t data1_ = 0x23456789;

    virtual bool read(uint64_t offset, uint64_t* data0, uint64_t* data1, uint64_t T) const {
      *data0 = data0_;
      *data1 = data1_;
      return true;
    }
    virtual bool write(uint64_t offset, uint64_t data0, uint64_t data1, uint64_t T) {
      data0_ = data0;
      data1_ = data1;
      return true;
    }
    virtual std::string to_string(bool print_zeros = false, std::string indent_string = "") const {
      return std::string("fake register block");
    };
    virtual std::string to_string(uint64_t offset,bool print_zeros = false, std::string indent_string = "") const {
      return to_string(print_zeros, indent_string);
    }
  };


  class BFN_TEST_NAME(ChipTestBase) : public BaseTest {
  };

#ifndef MODEL_CHIP_JBAY_OR_LATER
TEST_F(BFN_TEST_NAME(ChipTestBase), MagicNum) {
  /**
  * loop through all registers (addresses found in reg.h) for each, run the addr_decode()
  * method in register_utils, this gives us a -1 if the register isn't valid.
  * Then, for each valid reg: outword 0, then inword, inword should = outword:
  * if it doesnt, it may return 0bad = valid. if it returns something else.
  * Mark that down / output it and change said register to return 0bad.
  */
  Chip *chip = om_->chip();
  const uint32_t MIN_REG = 0;
  const uint32_t MAX_REG = 0xFFFFFFF;
  const uint32_t expected = 0;
  const uint32_t zeroBad = 0x0bad0bad;
  int correctCount = 0;
  int zeroBadCount = 0;
  int failCount = 0;
  std::vector<std::pair<uint32_t, uint32_t>> fails;

  for (uint32_t i = MIN_REG; i < MAX_REG; i += 4) {
    /**
    * *EXCEPTIONS*
    * pre_port_down_mutable and pgr_port_down_dis_mutable both seem to return
    * the inverse of whatever we inword. There are also some occassions where
    * we get back 0x00000FF, still unsure as to why.
    * Also, when the file misc_regs_dst_mutable.h has its read method called,
    * it has a callback method that it runs and sets the dbg_rst_ value to 1
    * via a method in chip.cpp (line 126) therefore we always get back 1.
    */
    if ((i != 0x00040128)//MiscRegsDbgRstMutable
        && !(i >= 0x00620100 && i <= 0x00620120)//PrePortDownMutable
        && !(i >= 0x0273d030 && i <= 0x0273d038)//PgrPortDownDisMutable:0
        && !(i >= 0x02f3d030 && i <= 0x02f3d038)//PgrPortDownDisMutable:1
        && !(i >= 0x0373d030 && i <= 0x0373d038)//PgrPortDownDisMutable:2
        && !(i >= 0x03f3d030 && i <= 0x03f3d038)) {//PgrPortDownDisMutable:3

      int ret = RegisterUtils::addr_decode(i, nullptr);
      if ((ret > REG_LO) && (ret < REG_HI) && (ret != REG_DEVSEL_SOFT_RESET)) {
        chip->OutWord(i, expected);
        uint32_t actual = chip->InWord(i);
        if (expected == actual) {
          correctCount++;
        } else if (actual == zeroBad) {
          zeroBadCount++;
        } else {
          fails.push_back(std::make_pair(i, actual));
          failCount++;
        }
      }
    }
  }
  //Output any failed register's names
  if (!(fails.empty())) {
    for (auto item : fails) {
      auto &subscribers = chip->GetSubscribers(item.first);
      for (const auto &subscriber : subscribers) {
        std::cout << "Name: " << subscriber->name_ << " Value: " << item.second
                  << std::endl;
      }
    }
  }
  ASSERT_EQ(failCount, 0)
                << "Correct = " << correctCount << ", ZeroBad = "
                << zeroBadCount << ", failCount = " << failCount;
}

TEST_F(BFN_TEST_NAME(ChipTestBase), TmRegsBadDataWord) {
  // XXX: The model TmRegs and TmRegs2 that *have not been written*
  // return 0x0badd3ff rather than the usual value for an unimplemented
  // register (0x0bad0bad)
  Chip *chip = om_->chip();
  for (uint32_t i = RegisterUtils::kTmStartAddress;
       i < RegisterUtils::kPreStartAddress;
       i += 4) {
    uint32_t actual = chip->InWord(i);
    ASSERT_EQ(0x0badd3ffu, actual);
  }
}
#endif

TEST_F(BFN_TEST_NAME(ChipTestBase), GetSubscribers) {
  Chip *chip = om_->chip();
  uint32_t addr = 56;
  auto &subscribers = chip->GetSubscribers(addr);
  ASSERT_EQ(1u, subscribers.size());
  for (const auto &subscriber : subscribers) {
    ASSERT_EQ("CpuIndAddrLow", subscriber->name_);
  }
  addr = 0;
  auto &subscribers2 = chip->GetSubscribers(addr);
  ASSERT_EQ(0u, subscribers2.size());
}

  TEST_F(BFN_TEST_NAME(ChipTestBase), OutWordInWord) {
    Chip *chip = om_->chip();

    // NB capture chip logger directly since chip may not have an RmtObjectManager
    RmtLoggerCapture log_capture = RmtLoggerCapture(chip->default_logger());
    chip->default_logger()->set_log_flags(0x20);  // verbose only
    const uint32_t addr = 0x0000abc;
    const uint32_t data0 = 0xabab;

    // no subscriber reg; write
    log_capture.start();
    chip->OutWord(addr, data0);
    log_capture.stop();
    int line_count = log_capture.for_each_line_containing(
        "Write to unimplemented address 0x00000abc", nullptr);
    EXPECT_EQ(1, line_count) << log_capture.dump_lines();
    // no subscriber reg; read
    log_capture.clear_and_start();
    (void)chip->InWord(addr);
    log_capture.stop();
    line_count = log_capture.for_each_line_containing(
        "Read from unimplemented address 0x00000abc", nullptr);
    EXPECT_EQ(1, line_count) << log_capture.dump_lines();

    // subscribe to address
    FakeRegisterBlock fake_reg(chip_index(), addr, 4, true, "fake");
    // all ok; write
    log_capture.clear_and_start();
    chip->OutWord(addr, data0);
    log_capture.stop();
    line_count = log_capture.for_each_line(nullptr);
    EXPECT_EQ(0, line_count) << log_capture.dump_lines();
    // all ok; read
    log_capture.clear_and_start();
    (void)chip->InWord(addr);
    log_capture.stop();
    line_count = log_capture.for_each_line(nullptr);
    EXPECT_EQ(0, line_count) << log_capture.dump_lines();
  }

  TEST_F(BFN_TEST_NAME(ChipTestBase), IndirectWriteRead) {
    Chip *chip = om_->chip();

    // NB capture chip logger directly since chip may not have an RmtObjectManager
    RmtLoggerCapture log_capture = RmtLoggerCapture(chip->default_logger());
    chip->default_logger()->set_log_flags(0x20);  // verbose only
    const uint64_t addr = 0x40080000000;
    const uint64_t data0 = 0xabababab;
    const uint64_t data1 = 0x10101010;
    uint64_t read0, read1;
    uint64_t t_base = chip->T_hi();  // base time off current chip T_hi_

    // no subscriber reg; write
    log_capture.start();
    uint64_t t2 = t_base + UINT64_C(102);
    chip->IndirectWrite(addr, data0, data1, t2);
    log_capture.stop();
    int line_count = log_capture.for_each_line_containing(
        "Write to unimplemented address 0x0000040080000000", nullptr);
    EXPECT_EQ(1, line_count) << log_capture.dump_lines();
    // no subscriber reg; read
    log_capture.clear_and_start();
    chip->IndirectRead(addr, &read0, &read1, t2);
    log_capture.stop();
    line_count = log_capture.for_each_line_containing(
        "Read from unimplemented address 0x0000040080000000", nullptr);
    EXPECT_EQ(1, line_count) << log_capture.dump_lines();

    // subscribe to address
    FakeRegisterBlockIndirect fake_reg(chip_index(), addr, 4, true, "fake");
    chip->default_logger()->set_log_flags(0x08);  // warn only
    // time goes backwards; write
    log_capture.clear_and_start();
    uint64_t t1 = t_base + UINT64_C(101);
    chip->IndirectWrite(addr, data0, data1, t1);
    log_capture.stop();
    line_count = log_capture.for_each_line_containing(
        "IndirectRead/Write: Time gone backwards!", nullptr);
    EXPECT_EQ(1, line_count) << log_capture.dump_lines();
    // time goes backwards; read
    log_capture.clear_and_start();
    uint64_t t0 = t_base + UINT64_C(100);
    chip->IndirectRead(addr, &read0, &read1, t0);
    log_capture.stop();
    line_count = log_capture.for_each_line_containing(
        "IndirectRead/Write: Time gone backwards!", nullptr);
    EXPECT_EQ(1, line_count) << log_capture.dump_lines();

    // all ok; write
    chip->default_logger()->set_log_flags(0xFF);  // all levels
    log_capture.clear_and_start();
    uint64_t t3 = t_base + UINT64_C(103);
    chip->IndirectWrite(addr, data0, data1, t3);
    log_capture.stop();
    line_count = log_capture.for_each_line(nullptr);
    EXPECT_EQ(0, line_count) << log_capture.dump_lines();
    // all ok; read
    log_capture.clear_and_start();
    chip->IndirectRead(addr, &read0, &read1, t3);
    log_capture.stop();
    line_count = log_capture.for_each_line(nullptr);
    EXPECT_EQ(0, line_count) << log_capture.dump_lines();
  }

  TEST_F(BFN_TEST_NAME(ChipTestBase), UpdateLogFlags) {
    Chip *chip = om_->chip();

    // NB capture chip logger directly since chip may not have an RmtObjectManager
    RmtLoggerCapture log_capture = RmtLoggerCapture(chip->default_logger());
    chip->default_logger()->set_log_flags(0x20);  // verbose only
    const uint32_t addr = 0x0000abc;
    const uint32_t data0 = 0xabab;

    RmtObjectManager *om = chip->GetObjectManager();
    // no subscriber reg; expect verbose level log on write
    log_capture.start();
    chip->OutWord(addr, data0);
    log_capture.stop();
    int line_count = log_capture.for_each_line_containing(
        "Write to unimplemented address 0x00000abc", nullptr);
    EXPECT_EQ(1, line_count) << log_capture.dump_lines();

    // switch off verbose logging directly on chip logger instance
    chip->default_logger()->set_log_flags(NON);
    log_capture.clear_and_start();
    chip->OutWord(addr, data0);
    log_capture.stop();
    line_count = log_capture.for_each_line_containing(
        "Write to unimplemented address 0x00000abc", nullptr);
    EXPECT_EQ(0, line_count) << log_capture.dump_lines();

    // verify that the chip's logger is registered with RmtObjectManager:
    // switch on verbose logging via RmtObjectManager
    om->update_log_flags(ALL, ALL, ALL, ALL, ALL, 0x20, 0x20);
    log_capture.clear_and_start();
    chip->OutWord(addr, data0);
    log_capture.stop();
    line_count = log_capture.for_each_line_containing(
        "Write to unimplemented address 0x00000abc", nullptr);
    EXPECT_EQ(1, line_count) << log_capture.dump_lines();
  }

  TEST_F(BFN_TEST_NAME(ChipTestBase), SetP4NameLookup) {
    Chip *chip = om_->chip();
    RmtLoggerCapture log_capture = RmtLoggerCapture(om_);
    om_->update_log_flags(ALL, ALL, ALL, ALL, ALL, FEW, FEW);
    log_capture.start();
    chip->SetP4NameLookup(0, nullptr);
    log_capture.stop();
    int line_count = log_capture.for_each_line_containing(
        "No P4 name-lookup file for chip", nullptr);
    EXPECT_EQ(1, line_count) << log_capture.dump_lines();

    // make a temp file into which we'll write some json
    char tmp_file_name[] = "/tmp/p4_context_XXXXXX";
    int err = mkstemp(tmp_file_name);
    ASSERT_GT(err, 0);

    // invalid json
    FILE *file = fopen(tmp_file_name, "w");
    fprintf(file, "invalid json");
    fclose(file);
    log_capture.clear_and_start();
    chip->SetP4NameLookup(0, tmp_file_name);
    log_capture.stop();
    line_count = log_capture.for_each_line_containing(
        "P4 name-lookup file not loaded", nullptr);
    EXPECT_EQ(1, line_count) << log_capture.dump_lines();
    remove(tmp_file_name);

    // missing schema version
    file = fopen(tmp_file_name, "w");
    fprintf(file, "{\"not_schema_version\" : \"0.0.0\"}");
    fclose(file);
    log_capture.clear_and_start();
    chip->SetP4NameLookup(0, tmp_file_name);
    log_capture.stop();
    line_count = log_capture.for_each_line_containing(
        "P4 name-lookup file schema version not found", nullptr);
    EXPECT_EQ(1, line_count) << log_capture.dump_lines();
    remove(tmp_file_name);

    // incorrect schema
    file = fopen(tmp_file_name, "w");
    fprintf(file, "{\"schema_version\" : \"1.5.0\"}");
    fclose(file);
    log_capture.clear_and_start();
    chip->SetP4NameLookup(0, tmp_file_name);
    log_capture.stop();
    line_count = log_capture.for_each_line_containing(
        "P4 name-lookup file has unexpected schema version", nullptr);
    EXPECT_EQ(1, line_count) << log_capture.dump_lines();
    remove(tmp_file_name);

    // ok schema
    file = fopen(tmp_file_name, "w");
    fprintf(file, "{\"schema_version\" : \"1.6.0\"}");
    fclose(file);
    log_capture.clear_and_start();
    chip->SetP4NameLookup(0, tmp_file_name);
    log_capture.stop();
    line_count = log_capture.for_each_line_containing("WARN", nullptr);
    EXPECT_EQ(0, line_count) << log_capture.dump_lines();
    remove(tmp_file_name);
  }

  TEST(BFN_TEST_NAME(ChipTest),SoftReset) {
    if (chip_print) RMT_UT_LOG_INFO("test_chip_soft_reset()\n");

    // Create our TestUtil class
    int chip = 0;
    TestUtil tu(GLOBAL_MODEL.get(), chip);
    auto *misc_regs = RegisterUtils::addr_misc_regs();
    RmtObjectManager *omA = tu.get_objmgr();
    RmtObjectManager *omB = tu.get_objmgr();
    EXPECT_EQ(omA, omB);
    uint64_t rdata0,rdata1,wdata0,wdata1;
    wdata0 = UINT64_C(0x1234567812345678);
    wdata1 = UINT64_C(0x8765432187654321);
    tu.sram_write(0,2,3,4,5, wdata0, wdata1);
    tu.sram_read(0,2,3,4,5, &rdata0, &rdata1);
    EXPECT_EQ(wdata0,rdata0);
    EXPECT_EQ(wdata1,rdata1);

    uint32_t status_val = 0u;
    uint32_t status_mask = RmtDefs::kSoftResetStatusMask;
    uint32_t status_ok = RmtDefs::kSoftResetStatusValOk;
    uint32_t reset_val = RmtDefs::kSoftResetCtrlVal;
    tu.OutWord(&misc_regs->soft_reset, reset_val); // Assert SwRESET
    status_val = tu.InWord(RegisterUtils::addr_misc_regs_dbg_rst());
    EXPECT_NE(status_ok, status_val & status_mask);
    tu.OutWord(&misc_regs->soft_reset, 0u); // Deassert SwRESET
    status_val = tu.InWord(RegisterUtils::addr_misc_regs_dbg_rst());
    EXPECT_EQ(status_ok, status_val & status_mask);

    tu.sram_read(1,2,3,4,5, &rdata0, &rdata1);
    EXPECT_NE(wdata0,rdata0);
    EXPECT_NE(wdata1,rdata1);
    EXPECT_NE(tu.get_objmgr(), nullptr);
  }


  TEST(BFN_TEST_NAME(ChipTest),SoftReset2) {
    if (chip_print) RMT_UT_LOG_INFO("test_chip_soft_reset2()\n");

    // Create our TestUtil class
    int chip = 0;
    TestUtil tu(GLOBAL_MODEL.get(), chip);
    auto *misc_regs = RegisterUtils::addr_misc_regs();

    int pipe_last = RmtDefs::map_mau_pipe(RmtDefs::kPipesMax-1,0xFF);
    int stage_last = (chip > 200) ?chip-200-1 :RmtDefs::kStagesMax-1;
    std::default_random_engine generator;
    std::uniform_int_distribution<uint64_t> data_distribution;
    std::uniform_int_distribution<uint64_t> address_distribution(0,1023);
    std::uniform_int_distribution<uint64_t> pipe_distribution(0,pipe_last);
    std::uniform_int_distribution<uint64_t> stage_distribution(0,stage_last);
    std::uniform_int_distribution<uint64_t> row_distribution(0,7);
    std::uniform_int_distribution<uint64_t> column_distribution(2,11);

    for (int rmt=0; rmt<5; rmt++) { // Loop 5 times reading/writing
      for (int t=0;t<2;++t) { // first time write, second time read back
        generator.seed( unsigned(0xDAB0D1B0D0B0DEB0) );
        std::map<uint64_t,bool> written;
        for (int i=0;i<100;++i) {
          uint64_t wdata0 = data_distribution(generator);
          uint64_t wdata1 = data_distribution(generator);
          int pipe  = pipe_distribution(generator);
          int stage = stage_distribution(generator);
          int r = row_distribution(generator);
          int c = column_distribution(generator);
          int addr = address_distribution(generator);

          // work out address and avoid writing to the same one twice
          uint64_t a = tu.make_sram_addr(pipe, stage, r, c, addr);
          if ( written.count(a) ) continue;
          written[a] = true;

          if (t==0) {
            if (chip_print) printf("  Write %d,%d,%d,%d,%d %" PRIx64 " %" PRIx64 "\n",pipe,stage,
                                   r,c,addr,wdata1,wdata0);
            tu.sram_write(pipe,stage,r,c,addr, wdata0, wdata1);
          } else {
            uint64_t rdata0,rdata1;
            tu.sram_read(pipe,stage,r,c,addr, &rdata0, &rdata1);
            if (chip_print) printf("  Read %d,%d,%d,%d,%d %" PRIx64 " %" PRIx64 "\n",pipe,stage,
                                   r,c,addr,rdata1,rdata0);
            EXPECT_EQ( wdata0, rdata0);
            EXPECT_EQ( wdata1, rdata1);
          }
        }
      }
      // And 5 times reset the model so we get a new objmgr
      uint32_t status_val = 0u;
      uint32_t status_mask = RmtDefs::kSoftResetStatusMask;
      uint32_t status_ok = RmtDefs::kSoftResetStatusValOk;
      uint32_t reset_val = RmtDefs::kSoftResetCtrlVal;
      tu.OutWord(&misc_regs->soft_reset, reset_val); // Assert SwRESET
      status_val = tu.InWord(RegisterUtils::addr_misc_regs_dbg_rst());
      EXPECT_NE(status_ok, status_val & status_mask);
      tu.OutWord(&misc_regs->soft_reset, 0u); // Deassert SwRESET
      status_val = tu.InWord(RegisterUtils::addr_misc_regs_dbg_rst());
      EXPECT_EQ(status_ok, status_val & status_mask);
    }
  }


  TEST(BFN_TEST_NAME(ChipTest),SoftReset3) {
    if (chip_print) RMT_UT_LOG_INFO("test_chip_soft_reset3()\n");

    // Create our TestUtil class
    int chip = 0;
    TestUtil tu(GLOBAL_MODEL.get(), chip);
    auto *misc_regs = RegisterUtils::addr_misc_regs();

    int pipe_last = RmtDefs::map_mau_pipe(RmtDefs::kPipesMax-1,0xFF);
    int stage_last = (chip > 200) ?chip-200-1 :RmtDefs::kStagesMax-1;
    std::default_random_engine generator;
    std::uniform_int_distribution<uint64_t> data_distribution;
    std::uniform_int_distribution<uint64_t> address_distribution(0,1023);
    std::uniform_int_distribution<uint64_t> pipe_distribution(0,pipe_last);
    std::uniform_int_distribution<uint64_t> stage_distribution(0,stage_last);
    std::uniform_int_distribution<uint64_t> row_distribution(0,7);
    std::uniform_int_distribution<uint64_t> column_distribution(2,11);

    for (int rmt=0; rmt<5; rmt++) { // Loop 5 times reading/writing

      // first time write,
      // second time read back,
      // third time reset chip then read back
      // - should be lots mismatches - insist at least 10

      for (int t=0;t<3;++t) {
        int mismatch = 0;
        void *obj_same_size_as_sram = NULL;
        if (t==2) {
          uint32_t reset_val = RmtDefs::kSoftResetCtrlVal;
          tu.OutWord(&misc_regs->soft_reset, reset_val); // Assert SwRESET
          tu.OutWord(&misc_regs->soft_reset, 0u); // Deassert SwRESET
          // Allocate fake SRAM just in case we get
          // exactly same objs as before
          obj_same_size_as_sram = (void*)malloc(sizeof(MauSram));
        }
        generator.seed( unsigned(0xDAB0D1B0D0B0DEB0) );
        std::map<uint64_t,bool> written;
        for (int i=0;i<100;++i) {
          uint64_t wdata0 = data_distribution(generator);
          uint64_t wdata1 = data_distribution(generator);
          int pipe  = pipe_distribution(generator);
          int stage = stage_distribution(generator);
          int r = row_distribution(generator);
          int c = column_distribution(generator);
          int addr = address_distribution(generator);

          // work out address and avoid writing to the same one twice
          uint64_t a = tu.make_sram_addr(pipe, stage, r, c, addr);
          if ( written.count(a) ) continue;
          written[a] = true;

          if (t==0) {
            if (chip_print) printf("  Write %d,%d,%d,%d,%d %" PRIx64 " %" PRIx64 "\n",pipe,stage,
                                   r,c,addr,wdata1,wdata0);
            tu.sram_write(pipe,stage,r,c,addr, wdata0, wdata1);
          } else {
            uint64_t rdata0,rdata1;
            tu.sram_read(pipe,stage,r,c,addr, &rdata0, &rdata1);
            if (chip_print) printf("  Read %d,%d,%d,%d,%d %" PRIx64 " %" PRIx64 "\n",pipe,stage,
                                   r,c,addr,rdata1,rdata0);
            if (t==1) {
              // Check equality 2nd time
              EXPECT_EQ( wdata0, rdata0);
              EXPECT_EQ( wdata1, rdata1);
            }
            if (t==2) {
              // Count errors 3rd time
              if ((wdata0 != rdata0) || (wdata1 != rdata1)) mismatch++;
            }
          }
        }
        if (t==2) {
          if (obj_same_size_as_sram != NULL) free(obj_same_size_as_sram);
          EXPECT_GT(mismatch, 10);
        }
      }
    }
  }

  TEST(BFN_TEST_NAME(ChipTest),SoftReset4) {
    if (chip_print) RMT_UT_LOG_INFO("test_chip_soft_reset4()\n");
    //Test replay for log type level.
    //1. Update a chip's log type level.
    //2. Reset the chip and init again.
    //3. Check if the update log type level has been replayed using save state.

    //We are using MAU at pipe 0 and stage 0 to check if log type level has changed.
    //We trust that if MAU has been updated then all objects in Object Manager has been updated.

    // Create our TestUtil class and obtain chip object
    int chip_n = 0;
    TestUtil tu(GLOBAL_MODEL.get(), chip_n);
    RmtObjectManager *om = tu.get_objmgr();
    Chip *chip = om->chip();

    chip->UpdateLogTypeLevels(
                  model_core::RmtDebug::ALL,  // pipes
                  model_core::RmtDebug::ALL,  // stages
                  0,
                  model_core::RmtDebug::ALL,  // clear
                  UINT64_C(0xF));            // set

    Mau *mau = om->mau_get(0,0);

    uint64_t old_log_type_level = mau->log_type_flags(0);

    chip->ResetChip();
    chip->InitChip();

    om = chip->GetObjectManagerWait();
    mau = om->mau_get(0,0);

    uint64_t new_log_type_level = mau->log_type_flags(0);
    EXPECT_EQ(old_log_type_level, new_log_type_level);
  }


  TEST(BFN_TEST_NAME(ChipTest),NoPhvLeakage) {
    if (chip_print) RMT_UT_LOG_INFO("test_chip_no_phv_leakage()\n");
    bool jbay = RmtObject::is_jbay_or_later();
    int chip = 202;
    int pipe = 0;
    int stage = 0;

    // Create our TestUtil class
    // Instantiating new TestUtil obj should free all existing
    // RmtObjectManagers (if this has not already occurred) then
    // recreate a RmtObjectManager just for chip
    TestUtil tu(GLOBAL_MODEL.get(), chip, pipe, stage);

    // DEBUG setup....................
    uint64_t pipes, stages, types, rows_tabs, cols, flags;
    pipes = ALL; stages = FEW; types = NON; rows_tabs = TOP; cols = HI; flags = ONE;
    pipes = ALL; stages = ALL; types = ALL; rows_tabs = ALL; cols = ALL; flags = ALL;

    tu.set_debug(false);
    tu.set_evaluate_all(true, false); // Don't test evaluateAll
    tu.set_free_on_exit(true);
    // Just to stop compiler complaining about unused vars
    flags = FEW;
    tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);


    // Instantiate whole chip and fish out objmgr
    RmtObjectManager *om = tu.get_objmgr();
    ASSERT_TRUE(om != NULL);
    // Lookup this Pipe/Stage MAU and MAU_ADDR_DIST obj
    Mau *mau = om->mau_lookup(pipe, stage);
    ASSERT_TRUE(mau != NULL);
    // Lookup some port
    Port *port = tu.port_get(16);
    ASSERT_TRUE(port != NULL);


    // Set all PHV ingress/egress threads for our 2 stages (chip9!)
    for (stage = 0; stage < 2; stage++) {
      tu.set_phv_range_all(stage, false);
      // Then set 1st 32 of the 64x32 64x8 to be ingress
      // and the 1st 48 of the 96x16 to be ingress
      tu.set_phv_ranges(stage, 32,0, 32,0, 32,16,0, true);
    }

    int ing_dep = (jbay) ?TestUtil::kDepAction :TestUtil::kDepConcurrent;
    int egr_dep = (jbay) ?TestUtil::kDepAction :TestUtil::kDepAction;
    // Setup ingress dependencies for stages
    tu.set_dependency(0, ing_dep, true);
    tu.set_dependency(1, ing_dep, true);
    // Setup egress dependencies for stages
    tu.set_dependency(0, egr_dep, false);
    tu.set_dependency(1, egr_dep, false);
    // Setup single logical table for ingress in stage0/1
    tu.table_config(0, 0, true);  // stage0  table0  ingress
    tu.table_config(1, 0, true);  // stage1  table0  ingress
    // Setup LT default regs
    tu.set_table_default_regs(0, 0); // stage0 table0
    tu.set_table_default_regs(1, 0); // stage1 table0
    // Setup nxt-tab MISS for table in stage0 to goto stage1
    tu.physbus_config(0, 0, 0,  0,0,0,16, 0,0,0,0, 0u,0u,0u,0u);

    // Setup stage default regs (need to do this after dependency
    // config so mpr_bus_dep can get setup properly);
    tu.set_stage_default_regs(0);
    tu.set_stage_default_regs(1);

    // UP debug now
    //flags = ALL;
    tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);


    // Send in many PHVs using TestUtil::port_process_inbound
    uint32_t n0 = om->dump_stats();

    for (int i = 0; i < 1000; i++) {

      // Dump stats so can check for leaks - print from time to time
      EXPECT_EQ(n0, om->dump_stats( ((i%100)==0) )  );

      Phv *phv_in = tu.phv_alloc();
      phv_in->set_ingress();

      phv_in->set(200, i % 10);
      phv_in->set(201, i);

      Phv *phv_out = tu.port_process_inbound(port, phv_in);

      if (phv_in != phv_out) tu.phv_free(phv_in);
      tu.phv_free(phv_out);
    }


    // Schtum
    tu.finish_test();
    tu.quieten_log_flags();
  }


  TEST(BFN_TEST_NAME(ChipTest),TwoPipesOnly) {
    //chip_print = true;
    if (chip_print) RMT_UT_LOG_INFO("test_chip_two_pipes_only()\n");

    // Create our TestUtil class
    int chip = 0;
    int max_stages = RmtDefs::kStagesMax;
    int num_stages = (chip > 200) ?std::min(chip-200,max_stages) :max_stages;
    int sku = TestUtil::kSku;
    model_core::Model *host = GLOBAL_MODEL.get();
    RMT_ASSERT(host != NULL);
    TestUtil tu(host, chip);
    tu.config_chip(0x5, num_stages, sku, 0u); // Setup to use pipes 0 and 2 only
    host->set_0bad_mode(1);                   // Prevent asserts on writes to non-existent pipes
    tu.Reset();                               // So 2 pipes takes effect

    uint64_t bad_data = UINT64_C(0x0BAD0BAD0BAD0BAD);
    auto *misc_regs = RegisterUtils::addr_misc_regs();

    int pipe_last = RmtDefs::map_mau_pipe(RmtDefs::kPipesMax-1,0x5);
    int stage_last = num_stages-1;
    std::default_random_engine generator;
    std::uniform_int_distribution<uint64_t> data_distribution;
    std::uniform_int_distribution<uint64_t> address_distribution(0,1023);
    std::uniform_int_distribution<uint64_t> pipe_distribution(0,pipe_last);
    std::uniform_int_distribution<uint64_t> stage_distribution(0,stage_last);
    std::uniform_int_distribution<uint64_t> row_distribution(0,7);
    std::uniform_int_distribution<uint64_t> column_distribution(2,11);

    for (int rmt=0; rmt<5; rmt++) { // Loop 5 times reading/writing

      // first time write,
      // second time read back,
      // third time reset chip then read back
      // - should be lots mismatches - insist at least 10

      for (int t=0;t<3;++t) {
        int mismatch = 0;
        void *obj_same_size_as_sram = NULL;
        if (t==2) {
          uint32_t reset_val = RmtDefs::kSoftResetCtrlVal;
          tu.OutWord(&misc_regs->soft_reset, reset_val); // Assert SwRESET
          tu.OutWord(&misc_regs->soft_reset, 0u); // Deassert SwRESET
          // Allocate fake SRAM just in case we get
          // exactly same objs as before
          obj_same_size_as_sram = (void*)malloc(sizeof(MauSram));
        }
        generator.seed( unsigned(0xDAB0D1B0D0B0DEB0) );
        std::map<uint64_t,bool> written;
        for (int i=0;i<100;++i) {
          uint64_t wdata0 = data_distribution(generator);
          uint64_t wdata1 = data_distribution(generator);
          int pipe  = pipe_distribution(generator);
          int stage = stage_distribution(generator);
          int r = row_distribution(generator);
          int c = column_distribution(generator);
          int addr = address_distribution(generator);

          // work out address and avoid writing to the same one twice
          uint64_t a = tu.make_sram_addr(pipe, stage, r, c, addr);
          if ( written.count(a) ) continue;
          written[a] = true;

          if (t==0) {
            if (chip_print) printf("  Write %d,%d,%d,%d,%d %" PRIx64 " %" PRIx64 "\n",pipe,stage,
                                   r,c,addr,wdata1,wdata0);
            tu.sram_write(pipe,stage,r,c,addr, wdata0, wdata1);
          } else {
            uint64_t rdata0,rdata1;
            tu.sram_read(pipe,stage,r,c,addr, &rdata0, &rdata1);
            if (chip_print) printf("  Read %d,%d,%d,%d,%d %" PRIx64 " %" PRIx64 "\n",pipe,stage,
                                   r,c,addr,rdata1,rdata0);
            if (t==1) {
              if ((pipe == 0) || (pipe == 2)) {
                // Check equality 2nd time
                EXPECT_EQ( wdata0, rdata0);
                EXPECT_EQ( wdata1, rdata1);
              } else {
                EXPECT_EQ( bad_data, rdata0);
                EXPECT_EQ( bad_data, rdata1);
              }
            }
            if (t==2) {
              if ((pipe == 0) || (pipe == 2)) {
                // Count errors 3rd time
                if ((wdata0 != rdata0) || (wdata1 != rdata1)) mismatch++;
              }
            }
          }
        }
        if (t==2) {
          if (obj_same_size_as_sram != NULL) free(obj_same_size_as_sram);
          EXPECT_GT(mismatch, 10);
        }
      }
    }
  }


TEST_F(BFN_TEST_NAME(ChipTestBase), DieIds) {
  Chip *chip = om_->chip();

  // defaults
  EXPECT_EQ(0u, chip->GetMyDieId());
  EXPECT_EQ(1u, chip->GetReadDieId());
  EXPECT_EQ(3u, chip->GetWriteDieId());
  EXPECT_EQ(2u, chip->GetDiagonalDieId());

  if (RmtObject::is_chip1()) {
    // non-defaults
    chip->SetMyDieId(1);
    EXPECT_EQ(1u, chip->GetMyDieId());
    EXPECT_EQ(0u, chip->GetReadDieId());
    EXPECT_EQ(2u, chip->GetWriteDieId());
    EXPECT_EQ(3u, chip->GetDiagonalDieId());
    // reset to defaults
    chip->SetMyDieId(0);

    EXPECT_THROW(chip->SetMyDieId(4);, std::runtime_error);
  } else {
    EXPECT_THROW(chip->SetMyDieId(1);, std::runtime_error);
  }
}


#ifdef MODEL_CHIP_JBAY_OR_LATER
//
// Only build these tests for JBay - only JBay has these atomic_mod INSTRs/CSRs

TEST(BFN_TEST_NAME(ChipTest),AtomicModTcamByCsr) {
    GLOBAL_MODEL->Reset();
    if (chip_print) RMT_UT_LOG_INFO("test_chip_atomic_mod_tcam_by_csr()\n");

    // Create our TestUtil class
    uint64_t pipes, stages, types, rows_tabs, cols, flags;
    pipes = ALL; stages = ALL; types = ALL; rows_tabs = TOP; cols = NON; flags = FEW;
    pipes = ALL; stages = ALL; types = ALL; rows_tabs = ALL; cols = ALL; flags = ALL;

    int chip = 202;
    int pipe = 0;
    int stage = 0;
    TestUtil tu(GLOBAL_MODEL.get(), chip, pipe, stage);
    tu.set_debug(false);
    tu.set_evaluate_all(true, false); // Don't test evaluateAll
    tu.set_free_on_exit(true);

    flags = FEW; // Just FatalErrorWarn
    tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);

    uint64_t data0, data1;

    // Configure TCAM
    tu.tcam_config(0, 0, 11, 1,           // pipe0 stage0 row11 col1
                   0, 0, 0, 0,            // inbus0 logtab0 ltcam0 vpn-
                   true, false, true, 0); // ingress chain output head
    // Zeroise TCAM[11,1] index 511
    tu.tcam_zero(0,0,11,1,511);
    data0 = UINT64_C(0); data1 = UINT64_C(0);
    tu.tcam_read_raw(0,0,11,1,511,&data0,&data1);
    EXPECT_EQ(UINT64_C(0), data0);
    EXPECT_EQ(UINT64_C(0), data1);

    // Setup Pending TCAM write in TCAM[11,1] index 511
    // (note don't set bit9 - that causes immediate write)
    int instr_set_tcam_writereg = (0xD << 21) | ( ((11<<1) | (1<<0)) << 10 ) | (511 << 0);
    uint64_t iaddr = TestUtil::make_instruction_address(0,0,0,instr_set_tcam_writereg);
    tu.IndirectWrite(iaddr, UINT64_C(100), UINT64_C(101));

    // Do read back of TCAM to check nothing happened yet
    data0 = UINT64_C(0); data1 = UINT64_C(0);
    tu.tcam_read_raw(0,0,11,1,511,&data0,&data1);
    EXPECT_EQ(UINT64_C(0), data0);
    EXPECT_EQ(UINT64_C(0), data1);

    // Commit write by writing to atomic_mod_tcam_go CSR
    auto& mau_reg_map = RegisterUtils::ref_mau(pipe,stage);
    tu.OutWord(&mau_reg_map.tcams.atomic_mod_tcam_go, 1);

    // Do read back of TCAM to check writes now written
    data0 = UINT64_C(0); data1 = UINT64_C(0);
    tu.tcam_read_raw(0,0,11,1,511,&data0,&data1);
    EXPECT_EQ(UINT64_C(100), data0);
    EXPECT_EQ(UINT64_C(101), data1);


    tu.finish_test();
    tu.quieten_log_flags();
}

TEST(BFN_TEST_NAME(ChipTest),AtomicModSramByCsr) {
    GLOBAL_MODEL->Reset();
    if (chip_print) RMT_UT_LOG_INFO("test_chip_atomic_mod_sram_by_csr()\n");

    // Create our TestUtil class
    uint64_t pipes, stages, types, rows_tabs, cols, flags;
    pipes = ALL; stages = ALL; types = ALL; rows_tabs = TOP; cols = NON; flags = FEW;
    pipes = ALL; stages = ALL; types = ALL; rows_tabs = ALL; cols = ALL; flags = ALL;

    int chip = 202;
    int pipe = 0;
    int stage = 0;
    TestUtil tu(GLOBAL_MODEL.get(), chip, pipe, stage);
    tu.set_debug(false);
    tu.set_evaluate_all(true, false); // Don't test evaluateAll
    tu.set_free_on_exit(true);

    flags = FEW; // Just FatalErrorWarn
    tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);

    tu.sram_config(0, 0, 2, 9,  // pipe0 stage0 row2 col9
                   TestUtil::kUnitramTypeMatch, 0, 0, 1, -1, 2, 2);

    // Initialise SRAM[2,0] index 99 to 0
    uint64_t waddr = TestUtil::make_physical_address(0,0,TestUtil::kPhysMemTypeSRAM,2,9,99);
    tu.IndirectWrite(waddr, UINT64_C(0), UINT64_C(0));
    // Setup Pending SRAM (type 7) write in SRAM[2,9] index 99
    uint64_t paddr = TestUtil::make_physical_address(0,0,7,2,9,99);
    tu.IndirectWrite(paddr, UINT64_C(100), UINT64_C(101));

    // Read back atomic_mod_shadow_ram_status CSR to confirm write pending
    auto& mau_reg_map = RegisterUtils::ref_mau(pipe,stage);
    uint32_t val = tu.InWord(&mau_reg_map.rams.map_alu.row[2].adrmux.atomic_mod_shadow_ram_status_right[3]);
    EXPECT_EQ((1<<10)|99, (int)val);

    // Commit write by writing to atomic_mod_sram_go_pending CSR (0 => ingress)
    tu.OutWord(&mau_reg_map.rams.match.adrdist.atomic_mod_sram_go_pending[0], 1);

    // Read back atomic_mod_shadow_ram_status CSR to confirm write done
    val = tu.InWord(&mau_reg_map.rams.map_alu.row[2].adrmux.atomic_mod_shadow_ram_status_right[3]);
    EXPECT_EQ((0<<10)|99, (int)val);

    // Do physical read back to check write happened
    uint64_t saddr = TestUtil::make_physical_address(0,0,TestUtil::kPhysMemTypeSRAM,2,9,99);
    uint64_t data0 = UINT64_C(0);
    uint64_t data1 = UINT64_C(0);
    tu.IndirectRead(saddr, &data0, &data1);
    EXPECT_EQ(UINT64_C(100), data0);
    EXPECT_EQ(UINT64_C(101), data1);


    tu.finish_test();
    tu.quieten_log_flags();
}

TEST(BFN_TEST_NAME(ChipTest),AtomicModSramByInstr) {
    GLOBAL_MODEL->Reset();
    if (chip_print) RMT_UT_LOG_INFO("test_chip_atomic_mod_sram_by_instr()\n");

    // Create our TestUtil class
    uint64_t pipes, stages, types, rows_tabs, cols, flags;
    pipes = ALL; stages = ALL; types = ALL; rows_tabs = TOP; cols = NON; flags = FEW;
    pipes = ALL; stages = ALL; types = ALL; rows_tabs = ALL; cols = ALL; flags = ALL;

    int chip = 202;
    int pipe = 0;
    int stage = 0;
    TestUtil tu(GLOBAL_MODEL.get(), chip, pipe, stage);
    tu.set_debug(false);
    tu.set_evaluate_all(true, false); // Don't test evaluateAll
    tu.set_free_on_exit(true);

    flags = FEW; // Just FatalErrorWarn
    tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);

    tu.sram_config(0, 0, 2, 9,  // pipe0 stage0 row2 col9
                   TestUtil::kUnitramTypeMatch, 0, 0, 1, -1, 2, 2);

    // Initialise SRAM[2,0] index 99 to 0
    uint64_t waddr = TestUtil::make_physical_address(0,0,TestUtil::kPhysMemTypeSRAM,2,9,99);
    tu.IndirectWrite(waddr, UINT64_C(0), UINT64_C(0));
    // Setup Pending SRAM (type 7) write in SRAM[2,9] index 99
    uint64_t paddr = TestUtil::make_physical_address(0,0,7,2,9,99);
    tu.IndirectWrite(paddr, UINT64_C(100), UINT64_C(101));

    // Read back atomic_mod_shadow_ram_status CSR to confirm write pending
    auto& mau_reg_map = RegisterUtils::ref_mau(pipe,stage);
    uint32_t val = tu.InWord(&mau_reg_map.rams.map_alu.row[2].adrmux.atomic_mod_shadow_ram_status_right[3]);
    EXPECT_EQ((1<<10)|99, (int)val);

    // Commit write using atomic_mod_sram instr
    int instr_atomic_mod_sram = 0x1c80000;
    uint64_t iaddr = TestUtil::make_instruction_address(0,0,0,instr_atomic_mod_sram);
    tu.IndirectWrite(iaddr, UINT64_C(0), UINT64_C(0));

    // Read back atomic_mod_shadow_ram_status CSR to confirm write done
    val = tu.InWord(&mau_reg_map.rams.map_alu.row[2].adrmux.atomic_mod_shadow_ram_status_right[3]);
    EXPECT_EQ((0<<10)|99, (int)val);

    // Do physical read back to check write happened
    uint64_t saddr = TestUtil::make_physical_address(0,0,TestUtil::kPhysMemTypeSRAM,2,9,99);
    uint64_t data0 = UINT64_C(0);
    uint64_t data1 = UINT64_C(0);
    tu.IndirectRead(saddr, &data0, &data1);
    EXPECT_EQ(UINT64_C(100), data0);
    EXPECT_EQ(UINT64_C(101), data1);

    tu.finish_test();
    tu.quieten_log_flags();
}

TEST(BFN_TEST_NAME(ChipTest),AtomicModCsr) {
    GLOBAL_MODEL->Reset();
    if (chip_print) RMT_UT_LOG_INFO("test_chip_atomic_mod_csr()\n");

    // Create our TestUtil class
    uint64_t pipes, stages, types, rows_tabs, cols, flags;
    pipes = ALL; stages = ALL; types = ALL; rows_tabs = TOP; cols = NON; flags = FEW;
    pipes = ALL; stages = ALL; types = ALL; rows_tabs = ALL; cols = ALL; flags = ALL;

    int chip = 202;
    int pipe = 0;
    int stage = 0;
    TestUtil tu(GLOBAL_MODEL.get(), chip, pipe, stage);
    tu.set_debug(false);
    tu.set_evaluate_all(true, false); // Don't test evaluateAll
    tu.set_free_on_exit(true);
    MauOpHandlerCommon::kAllowAtomicWideBubbles = false;


    flags = FEW; // Just FatalErrorWarn
    tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);

    uint64_t waddr, paddr, iaddr, raddr, data0, data1;


    // Configure TCAM
    tu.tcam_config(0, 0, 11, 1,           // pipe0 stage0 row11 col1
                   0, 0, 0, 0,            // inbus0 logtab0 ltcam0 vpn-
                   true, false, true, 0); // ingress chain output head
    // Zeroise TCAM[11,1] index 511
    tu.tcam_zero(0,0,11,1,511);
    data0 = UINT64_C(0); data1 = UINT64_C(0);
    tu.tcam_read_raw(0,0,11,1,511,&data0,&data1);
    EXPECT_EQ(UINT64_C(0), data0);
    EXPECT_EQ(UINT64_C(0), data1);

    // Setup Pending TCAM write in TCAM[11,1] index 511
    // (note don't set bit9 - that causes immediate write)
    int instr_set_tcam_writereg = (0xD << 21) | ( ((11<<1) | (1<<0)) << 10 ) | (511 << 0);
    iaddr = TestUtil::make_instruction_address(0,0,0,instr_set_tcam_writereg);
    tu.IndirectWrite(iaddr, UINT64_C(100), UINT64_C(101));

    // Do read back of TCAM to check nothing happened yet
    data0 = UINT64_C(0); data1 = UINT64_C(0);
    tu.tcam_read_raw(0,0,11,1,511,&data0,&data1);
    EXPECT_EQ(UINT64_C(0), data0);
    EXPECT_EQ(UINT64_C(0), data1);


    // Configure SRAM
    tu.sram_config(0, 0, 2, 9,  // pipe0 stage0 row2 col9
                   TestUtil::kUnitramTypeMatch, 0, 0, 1, -1, 2, 2);

    // Initialise SRAM[2,0] index 99 to 0
    waddr = TestUtil::make_physical_address(0,0,TestUtil::kPhysMemTypeSRAM,2,9,99);
    tu.IndirectWrite(waddr, UINT64_C(0), UINT64_C(0));

    // Setup Pending SRAM (type 7) write in SRAM[2,9] index 99
    paddr = TestUtil::make_physical_address(0,0,7,2,9,99);
    tu.IndirectWrite(paddr, UINT64_C(100), UINT64_C(101));

    // Read back atomic_mod_shadow_ram_status CSR to confirm write not happened yet
    auto& mau_reg_map = RegisterUtils::ref_mau(pipe,stage);
    uint32_t val = tu.InWord(&mau_reg_map.rams.map_alu.row[2].adrmux.atomic_mod_shadow_ram_status_right[3]);
    EXPECT_EQ((1<<10)|99, (int)val);
    // Also do physical read back to check write not happened yet
    raddr = TestUtil::make_physical_address(0,0,TestUtil::kPhysMemTypeSRAM,2,9,99);
    data0 = UINT64_C(0);
    data1 = UINT64_C(0);
    tu.IndirectRead(raddr, &data0, &data1);
    EXPECT_NE(UINT64_C(100), data0);
    EXPECT_NE(UINT64_C(101), data1);


    // ATOMIC COMMIT - START
    flags = ALL; // Up debug
    tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);

    // START AtomicModCSR using atomic_mod_csr BEGIN instruction
    int instr_atomic_mod_csr_begin = 0x1d00000; // Bit1==0 ==> BEGIN
    iaddr = TestUtil::make_instruction_address(0,0,0,instr_atomic_mod_csr_begin);
    tu.IndirectWrite(iaddr, UINT64_C(0), UINT64_C(0));

    // Setup TCAM commit write by writing to atomic_mod_tcam_go CSR
    tu.OutWord(&mau_reg_map.tcams.atomic_mod_tcam_go, 1);

    // Setup pending SRAM write commit by writing to
    // atomic_mod_sram_go_pending CSR (0 => ingress)
    tu.OutWord(&mau_reg_map.rams.match.adrdist.atomic_mod_sram_go_pending[0], 1);

    // END AtomicModCSR using atomic_mod_csr END instruction
    int instr_atomic_mod_csr_end = 0x1d00002; // Bit1==1 ==> END
    iaddr = TestUtil::make_instruction_address(0,0,0,instr_atomic_mod_csr_end);
    tu.IndirectWrite(iaddr, UINT64_C(0), UINT64_C(0));
    // ATOMIC COMMIT - END



    // Now do read back of TCAM to check writes have been written
    data0 = UINT64_C(0); data1 = UINT64_C(0);
    tu.tcam_read_raw(0,0,11,1,511,&data0,&data1);
    EXPECT_EQ(UINT64_C(100), data0);
    EXPECT_EQ(UINT64_C(101), data1);

    // And read back atomic_mod_shadow_ram_status CSR to confirm write now done
    val = tu.InWord(&mau_reg_map.rams.map_alu.row[2].adrmux.atomic_mod_shadow_ram_status_right[3]);
    EXPECT_EQ((0<<10)|99, (int)val);
    // Also do physical read back to check write happened
    raddr = TestUtil::make_physical_address(0,0,TestUtil::kPhysMemTypeSRAM,2,9,99);
    data0 = UINT64_C(0);
    data1 = UINT64_C(0);
    tu.IndirectRead(raddr, &data0, &data1);
    EXPECT_EQ(UINT64_C(100), data0);
    EXPECT_EQ(UINT64_C(101), data1);


    tu.finish_test();
    tu.quieten_log_flags();
}

TEST(BFN_TEST_NAME(ChipTest),AtomicModCsrWide) {
    GLOBAL_MODEL->Reset();
    if (chip_print) RMT_UT_LOG_INFO("test_chip_atomic_mod_csr_wide()\n");

    // Create our TestUtil class
    uint64_t pipes, stages, types, rows_tabs, cols, flags;
    pipes = ALL; stages = ALL; types = ALL; rows_tabs = TOP; cols = NON; flags = FEW;
    pipes = ALL; stages = ALL; types = ALL; rows_tabs = ALL; cols = ALL; flags = ALL;

    int chip = 202;
    int pipe = 0;
    int stage = 0;
    TestUtil tu(GLOBAL_MODEL.get(), chip, pipe, stage);
    tu.set_debug(false);
    tu.set_evaluate_all(true, false); // Don't test evaluateAll
    tu.set_free_on_exit(true);
    MauOpHandlerCommon::kAllowAtomicWideBubbles = true;

    flags = FEW; // Just FatalErrorWarn
    tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);


    auto& mau_reg_map0 = RegisterUtils::ref_mau(pipe,0);
    auto& mau_reg_map1 = RegisterUtils::ref_mau(pipe,1);

    // Configure SRAM[2,0] in Stage0 and Stage1
    uint32_t val;
    uint64_t waddr, paddr, iaddr, raddr, data0, data1;
    tu.sram_config(0, 0, 2, 9,  // pipe0 stage0 row2 col9
                   TestUtil::kUnitramTypeMatch, 0, 0, 1, -1, 2, 2);
    tu.sram_config(0, 1, 2, 9,  // pipe0 stage1 row2 col9
                   TestUtil::kUnitramTypeMatch, 0, 0, 1, -1, 2, 2);

    // Initialise Stage0 SRAM[2,0] index 99 to 0
    waddr = TestUtil::make_physical_address(0,0,TestUtil::kPhysMemTypeSRAM,2,9,99);
    tu.IndirectWrite(waddr, UINT64_C(0), UINT64_C(0));
    // Setup Pending SRAM (type 7) write in Stage0 SRAM[2,9] index 99
    paddr = TestUtil::make_physical_address(0,0,7,2,9,99);
    tu.IndirectWrite(paddr, UINT64_C(100), UINT64_C(101));

    // Initialise Stage1 SRAM[2,0] index 99 to 0
    waddr = TestUtil::make_physical_address(0,1,TestUtil::kPhysMemTypeSRAM,2,9,99);
    tu.IndirectWrite(waddr, UINT64_C(0), UINT64_C(0));
    // Setup Pending SRAM (type 7) write in Stage1 SRAM[2,9] index 99
    paddr = TestUtil::make_physical_address(0,1,7,2,9,99);
    tu.IndirectWrite(paddr, UINT64_C(100), UINT64_C(101));


    // Read back atomic_mod_shadow_ram_status CSR in Stage0 to confirm write not happened yet
    val = tu.InWord(&mau_reg_map0.rams.map_alu.row[2].adrmux.atomic_mod_shadow_ram_status_right[3]);
    EXPECT_EQ((1<<10)|99, (int)val);
    // Also do physical read back to check write not happened yet
    raddr = TestUtil::make_physical_address(0,0,TestUtil::kPhysMemTypeSRAM,2,9,99);
    data0 = UINT64_C(0);
    data1 = UINT64_C(0);
    tu.IndirectRead(raddr, &data0, &data1);
    EXPECT_NE(UINT64_C(100), data0);
    EXPECT_NE(UINT64_C(101), data1);

    // Read back atomic_mod_shadow_ram_status CSR in Stage1 to confirm write not happened yet
    val = tu.InWord(&mau_reg_map1.rams.map_alu.row[2].adrmux.atomic_mod_shadow_ram_status_right[3]);
    EXPECT_EQ((1<<10)|99, (int)val);
    // Also do physical read back to check write not happened yet
    raddr = TestUtil::make_physical_address(0,1,TestUtil::kPhysMemTypeSRAM,2,9,99);
    data0 = UINT64_C(0);
    data1 = UINT64_C(0);
    tu.IndirectRead(raddr, &data0, &data1);
    EXPECT_NE(UINT64_C(100), data0);
    EXPECT_NE(UINT64_C(101), data1);


    flags = ALL; // Up debug
    tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);


    // START AtomicModCSR in Stage0 using atomic_mod_csr BEGIN instruction
    int instr_atomic_mod_csr_begin = 0x1d00000; // Bit1==0 ==> BEGIN
    iaddr = TestUtil::make_instruction_address(0,0,0,instr_atomic_mod_csr_begin);
    tu.IndirectWrite(iaddr, UINT64_C(0), UINT64_C(0));
    // START AtomicModCSR in Stage1 using atomic_mod_csr BEGIN instruction
    iaddr = TestUtil::make_instruction_address(0,1,0,instr_atomic_mod_csr_begin);
    tu.IndirectWrite(iaddr, UINT64_C(0), UINT64_C(0));


    // Setup pending write commit in Stage0 by writing to atomic_mod_sram_go_pending CSR
    tu.OutWord(&mau_reg_map0.rams.match.adrdist.atomic_mod_sram_go_pending[0], 1);
    // Setup pending write commit in Stage1 by writing to atomic_mod_sram_go_pending CSR
    tu.OutWord(&mau_reg_map1.rams.match.adrdist.atomic_mod_sram_go_pending[0], 1);


    // END AtomicModCSR using atomic_mod_csr END instruction in Stage1
    // Just plain END not END WIDE. CSR writes pend till some other MAU does END WIDE
    int instr_atomic_mod_csr_end = 0x1d00002; // Bit1==1 ==> END
    iaddr = TestUtil::make_instruction_address(0,1,0,instr_atomic_mod_csr_end);
    tu.IndirectWrite(iaddr, UINT64_C(0), UINT64_C(0));
    // END AtomicModCSR using atomic_mod_csr END instruction in Stage0
    // But END WIDE - this should trigger flush in *BOTH* Stage0 AND Stage1
    int instr_atomic_mod_csr_end_wide = 0x1d00006; // Bit1==1 ==> END, Bit2==1 ==> WIDE
    iaddr = TestUtil::make_instruction_address(0,0,0,instr_atomic_mod_csr_end_wide);
    tu.IndirectWrite(iaddr, UINT64_C(0), UINT64_C(0));


    // Read back atomic_mod_shadow_ram_status CSR in Stage0 to confirm write now done
    val = tu.InWord(&mau_reg_map0.rams.map_alu.row[2].adrmux.atomic_mod_shadow_ram_status_right[3]);
    EXPECT_EQ((0<<10)|99, (int)val);
    // Also do physical read back in Stage0 SRAM to check write happened
    raddr = TestUtil::make_physical_address(0,0,TestUtil::kPhysMemTypeSRAM,2,9,99);
    data0 = UINT64_C(0);
    data1 = UINT64_C(0);
    tu.IndirectRead(raddr, &data0, &data1);
    EXPECT_EQ(UINT64_C(100), data0);
    EXPECT_EQ(UINT64_C(101), data1);

    // Read back atomic_mod_shadow_ram_status CSR in Stage1 to confirm write now done
    val = tu.InWord(&mau_reg_map1.rams.map_alu.row[2].adrmux.atomic_mod_shadow_ram_status_right[3]);
    EXPECT_EQ((0<<10)|99, (int)val);
    // Also do physical read back in Stage1 SRAM to check write happened
    raddr = TestUtil::make_physical_address(0,1,TestUtil::kPhysMemTypeSRAM,2,9,99);
    data0 = UINT64_C(0);
    data1 = UINT64_C(0);
    tu.IndirectRead(raddr, &data0, &data1);
    EXPECT_EQ(UINT64_C(100), data0);
    EXPECT_EQ(UINT64_C(101), data1);


    tu.finish_test();
    tu.quieten_log_flags();
}
#endif

#if MCN_TEST(MODEL_CHIP_NAMESPACE,rsvd0)

TEST(BFN_TEST_NAME(ChipTest),DISABLED_TmRegUnimplemented) {
  int chip = 0;
  TestUtil tu(GLOBAL_MODEL.get(), chip);

  uint32_t wdata = 0x87654321;
  uint32_t addr, rdata;

  for (addr = BFN_REG_TM_START_CAA; addr< BFN_REG_TM_END_CAA; addr+=4) {
    tu.OutWord(addr, wdata);
    rdata = tu.InWord(addr);
    EXPECT_EQ(wdata, rdata);
  }
  rdata = tu.InWord(BFN_REG_TM_END_CAA + 1);
  EXPECT_NE(wdata, rdata);

  for (addr = BFN_REG_TM_START_QAC; addr< BFN_REG_TM_END_QAC; addr+=4) {
    tu.OutWord(addr, wdata);
    rdata = tu.InWord(addr);
    EXPECT_EQ(wdata, rdata);
  }
  rdata = tu.InWord(BFN_REG_TM_END_QAC + 1);
  EXPECT_NE(wdata, rdata);

  for (addr = BFN_REG_TM_START_SCH; addr< BFN_REG_TM_END_SCH; addr+=4) {
    tu.OutWord(addr, wdata);
    rdata = tu.InWord(addr);
    EXPECT_EQ(wdata, rdata);
  }
  rdata = tu.InWord(BFN_REG_TM_END_SCH + 1);
  EXPECT_NE(wdata, rdata);

  for (addr = BFN_REG_TM_START_QOC; addr< BFN_REG_TM_END_QOC; addr+=4) {
    tu.OutWord(addr, wdata);
    rdata = tu.InWord(addr);
    EXPECT_EQ(wdata, rdata);
  }
  rdata = tu.InWord(BFN_REG_TM_END_QOC + 1);
  EXPECT_NE(wdata, rdata);

  for (addr = BFN_REG_TM_START_CLC; addr< BFN_REG_TM_END_CLC; addr+=4) {
    tu.OutWord(addr, wdata);
    rdata = tu.InWord(addr);
    EXPECT_EQ(wdata, rdata);
  }
  rdata = tu.InWord(BFN_REG_TM_END_CLC + 1);
  EXPECT_NE(wdata, rdata);

  for (addr = BFN_REG_TM_START_PEX; addr< BFN_REG_TM_END_PEX; addr+=4) {
    tu.OutWord(addr, wdata);
    rdata = tu.InWord(addr);
    EXPECT_EQ(wdata, rdata);
  }
  rdata = tu.InWord(BFN_REG_TM_END_PEX + 1);
  EXPECT_NE(wdata, rdata);

  for (addr = BFN_REG_TM_START_QLC; addr< BFN_REG_TM_END_QLC; addr+=4) {
    tu.OutWord(addr, wdata);
    rdata = tu.InWord(addr);
    EXPECT_EQ(wdata, rdata);
  }
  rdata = tu.InWord(BFN_REG_TM_END_QLC + 1);
  EXPECT_NE(wdata, rdata);

  for (addr = BFN_REG_TM_START_PRC; addr< BFN_REG_TM_END_PRC; addr+=4) {
    tu.OutWord(addr, wdata);
    rdata = tu.InWord(addr);
    EXPECT_EQ(wdata, rdata);
  }
  rdata = tu.InWord(BFN_REG_TM_END_PRC + 1);
  EXPECT_NE(wdata, rdata);

  for (addr = BFN_REG_TM_START_PSC; addr< BFN_REG_TM_END_PSC; addr+=4) {
    tu.OutWord(addr, wdata);
    rdata = tu.InWord(addr);
    EXPECT_EQ(wdata, rdata);
  }
  rdata = tu.InWord(BFN_REG_TM_END_PSC + 1);
  EXPECT_NE(wdata, rdata);

  for (addr = BFN_REG_TM_START_MGC; addr< BFN_REG_TM_END_MGC; addr+=4) {
    tu.OutWord(addr, wdata);
    rdata = tu.InWord(addr);
    EXPECT_EQ(wdata, rdata);
  }
  rdata = tu.InWord(BFN_REG_TM_END_MGC + 1);
  EXPECT_NE(wdata, rdata);

  for (addr = BFN_REG_TM_START_DDR; addr< BFN_REG_TM_END_DDR; addr+=4) {
    tu.OutWord(addr, wdata);
    rdata = tu.InWord(addr);
    EXPECT_EQ(wdata, rdata);
  }
  rdata = tu.InWord(BFN_REG_TM_END_DDR + 1);
  EXPECT_NE(wdata, rdata);
}

TEST(BFN_TEST_NAME(ChipTest),TmMemUnimplemented) {
  int chip = 0;
  TestUtil tu(GLOBAL_MODEL.get(), chip);

  uint64_t wdata0 = 0x9987654321;
  uint64_t wdata1 = 0x123456789;
  uint64_t addr, rdata0, rdata1;

  for (addr = BFN_MEM_TM_START_CAA; addr< BFN_MEM_TM_START_CAA; addr+=4) {
    tu.IndirectWrite(addr, wdata0, wdata1);
    tu.IndirectRead(addr, &rdata0, &rdata1);
    EXPECT_EQ(wdata0, rdata0);
    EXPECT_EQ(wdata1, rdata1);
  }
  tu.IndirectRead(BFN_MEM_TM_END_CAA +1, &rdata0, &rdata1);
  EXPECT_NE(wdata0, rdata0);
  EXPECT_NE(wdata1, rdata1);

  for (addr = BFN_MEM_TM_START_QAC; addr< BFN_MEM_TM_START_QAC; addr+=4) {
    tu.IndirectWrite(addr, wdata0, wdata1);
    tu.IndirectRead(addr, &rdata0, &rdata1);
    EXPECT_EQ(wdata0, rdata0);
    EXPECT_EQ(wdata1, rdata1);
  }
  tu.IndirectRead(BFN_MEM_TM_END_QAC +1, &rdata0, &rdata1);
  EXPECT_NE(wdata0, rdata0);
  EXPECT_NE(wdata1, rdata1);

  for (addr = BFN_MEM_TM_START_SCH; addr< BFN_MEM_TM_START_SCH; addr+=4) {
    tu.IndirectWrite(addr, wdata0, wdata1);
    tu.IndirectRead(addr, &rdata0, &rdata1);
    EXPECT_EQ(wdata0, rdata0);
    EXPECT_EQ(wdata1, rdata1);
  }
  tu.IndirectRead(BFN_MEM_TM_END_SCH +1, &rdata0, &rdata1);
  EXPECT_NE(wdata0, rdata0);
  EXPECT_NE(wdata1, rdata1);

  for (addr = BFN_MEM_TM_START_CLC; addr< BFN_MEM_TM_START_CLC; addr+=4) {
    tu.IndirectWrite(addr, wdata0, wdata1);
    tu.IndirectRead(addr, &rdata0, &rdata1);
    EXPECT_EQ(wdata0, rdata0);
    EXPECT_EQ(wdata1, rdata1);
  }
  tu.IndirectRead(BFN_MEM_TM_END_PEX +1, &rdata0, &rdata1);
  EXPECT_NE(wdata0, rdata0);
  EXPECT_NE(wdata1, rdata1);

  for (addr = BFN_MEM_TM_START_PEX; addr< BFN_MEM_TM_START_PEX; addr+=4) {
    tu.IndirectWrite(addr, wdata0, wdata1);
    tu.IndirectRead(addr, &rdata0, &rdata1);
    EXPECT_EQ(wdata0, rdata0);
    EXPECT_EQ(wdata1, rdata1);
  }
  tu.IndirectRead(BFN_MEM_TM_END_QLC +1, &rdata0, &rdata1);
  EXPECT_NE(wdata0, rdata0);
  EXPECT_NE(wdata1, rdata1);

  for (addr = BFN_MEM_TM_START_QLC; addr< BFN_MEM_TM_START_QLC; addr+=4) {
    tu.IndirectWrite(addr, wdata0, wdata1);
    tu.IndirectRead(addr, &rdata0, &rdata1);
    EXPECT_EQ(wdata0, rdata0);
    EXPECT_EQ(wdata1, rdata1);
  }
  tu.IndirectRead(BFN_MEM_TM_END_QLC +1, &rdata0, &rdata1);
  EXPECT_NE(wdata0, rdata0);
  EXPECT_NE(wdata1, rdata1);

  for (addr = BFN_MEM_TM_START_PRC; addr< BFN_MEM_TM_START_PRC; addr+=4) {
    tu.IndirectWrite(addr, wdata0, wdata1);
    tu.IndirectRead(addr, &rdata0, &rdata1);
    EXPECT_EQ(wdata0, rdata0);
    EXPECT_EQ(wdata1, rdata1);
  }
  tu.IndirectRead(BFN_MEM_TM_END_PRC +1, &rdata0, &rdata1);
  EXPECT_NE(wdata0, rdata0);
  EXPECT_NE(wdata1, rdata1);

  for (addr = BFN_MEM_TM_START_PSC; addr< BFN_MEM_TM_START_PSC; addr+=4) {
    tu.IndirectWrite(addr, wdata0, wdata1);
    tu.IndirectRead(addr, &rdata0, &rdata1);
    EXPECT_EQ(wdata0, rdata0);
    EXPECT_EQ(wdata1, rdata1);
  }
  tu.IndirectRead(BFN_MEM_TM_END_PSC +1, &rdata0, &rdata1);
  EXPECT_NE(wdata0, rdata0);
  EXPECT_NE(wdata1, rdata1);
}
#endif


}
