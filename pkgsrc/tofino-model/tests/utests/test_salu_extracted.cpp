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

// Run all tests in extracted_tests/salu

#include <utests/test_util.h>
#include <iostream>
#include <string>
#include <array>
#include <cassert>

#include "gtest.h"

#include <bitvector.h>
#include <rmt-object-manager.h>
#include <model_core/model.h>
#include <mau.h>
#include <eop.h>

extern std::unique_ptr<model_core::Model> GLOBAL_MODEL;

namespace MODEL_CHIP_TEST_NAMESPACE {

  bool salu_extracted_print = false;

  using namespace std;
  using namespace MODEL_CHIP_NAMESPACE;


TEST(BFN_TEST_NAME(StatefulAluExtractedTest),Packet1) {
    GLOBAL_MODEL->Reset();
    if (salu_extracted_print) RMT_UT_LOG_INFO("test_salu_extracted_packet1()\n");

    // Create our TestUtil class
    uint64_t ALL = UINT64_C(0xFFFFFFFFFFFFFFFF);
    uint64_t FEW = UINT64_C(0xF); // FatalErrorWarn
    uint64_t NON = UINT64_C(0);
    uint64_t TOP = UINT64_C(1) << 63;
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

    // Just to stop compiler complaining about unused vars
    flags = FEW; // Just FatalErrorWarn
    //flags = ALL; // ALL
    tu.update_log_flags(pipes, stages, types, rows_tabs, cols, flags, ALL);

#ifdef MODEL_CHIP_JBAY_OR_LATER
    const char* read_dir="extracted_tests/jbay_salu";

    // This is just for testing pasted in code
    #if 0
    int which_alu = 0;
    RmtObjectManager *om = tu.get_objmgr();
    ASSERT_TRUE(om != NULL);
    int logrow = MauMeterAlu::get_meter_alu_logrow_index(which_alu);
    MauStatefulAlu* salu = om->mau_get(pipe, stage)->logical_row_lookup(logrow)->mau_meter_alu()->get_salu();
    BitVector<MauDefs::kStatefulMeterAluDataBits> phv_data_word{};
    BitVector<MauDefs::kDataBusWidth> data_in_bv;
    BitVector<MauDefs::kDataBusWidth> data_out_bv;
    BitVector<MauDefs::kActionOutputBusWidth> action_out_bv;
    uint32_t addr;
    uint64_t present_time;
    MauStatefulAlu::StatefulBus match_bus{};           // TODO pass this in from DV
    MauStatefulAlu::StatefulBus learn_or_match_bus{}; // TODO pass this in from DV
    #endif

#else
    const char* read_dir="extracted_tests/salu";
#endif

    try {
      bool result = tu.read_test_dir(read_dir);
      EXPECT_TRUE(result);
    } catch (...) {
      EXPECT_FALSE(true) << "Exception thrown!";
    }

    tu.finish_test();
    tu.quieten_log_flags();
}


}

