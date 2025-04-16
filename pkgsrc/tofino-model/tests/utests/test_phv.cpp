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
#include <iostream>
#include <string>
#include <array>
#include <cassert>

#include "gtest.h"

#include <model_core/model.h>
#include <bitvector.h>
#include <tcam.h>
#include <rmt-object-manager.h>
#include <packet.h>
#include <phv-factory.h>
#include <phv.h>
#include <parser-static-config.h>
#include <p4-name-lookup.h>
#include <phv-modification.h>
#include <mau-object.h>
#include <model_core/rmt-phv-modification.h>

extern std::unique_ptr<model_core::Model> GLOBAL_MODEL;

namespace MODEL_CHIP_TEST_NAMESPACE {

  bool phv_print = false;

  using namespace std;
  using namespace MODEL_CHIP_NAMESPACE;


  TEST(BFN_TEST_NAME(PhvTest),Basic) {
    GLOBAL_MODEL->Reset();
    if (phv_print) RMT_UT_LOG_INFO("test_phv_basic()\n");
    TestUtil tu(GLOBAL_MODEL.get());
    RmtObjectManager *om = tu.get_objmgr();
    if (phv_print) RMT_UT_LOG_INFO("Create PHV\n");
    Phv *phv = om->phv_create();
    phv->set(Phv::make_word_mapped(0,0), 0x99999990u);
    ASSERT_TRUE((phv->get(Phv::make_word_mapped(0,0)) == 0x99999990u));
    phv->set(Phv::make_word_mapped(1,1), 0x99999991u);
    ASSERT_TRUE((phv->get(Phv::make_word_mapped(1,1)) == 0x99999991u));
    phv->set(Phv::make_word_mapped(2,2), 0x99999992u);
    ASSERT_TRUE((phv->get(Phv::make_word_mapped(2,2)) == 0x92u));
    phv->set(Phv::make_word_mapped(3,3), 0x99999993u);
    ASSERT_TRUE((phv->get(Phv::make_word_mapped(3,3)) == 0x93u));
    phv->set(Phv::make_word_mapped(4,4), 0x99999994u);
    ASSERT_TRUE((phv->get(Phv::make_word_mapped(4,4)) == 0x9994u));
    phv->set(Phv::make_word_mapped(5,5), 0x99999995u);
    ASSERT_TRUE((phv->get(Phv::make_word_mapped(5,5)) == 0x9995u));
    phv->set(Phv::make_word_mapped(6,6), 0x99999996u);
    ASSERT_TRUE((phv->get(Phv::make_word_mapped(6,6)) == 0x9996u));
    if (phv_print) phv->print_d("PHV DUMP D", true);
    if (phv_print) phv->print_p("PHV DUMP P", true);
    if (phv_print) phv->print_x("PHV DUMP X", true);
  }

  TEST(BFN_TEST_NAME(PhvTest),ErrorFlag) {
    GLOBAL_MODEL->Reset();
    if (phv_print) RMT_UT_LOG_INFO("test_phv_error_flag()\n");
    TestUtil tu(GLOBAL_MODEL.get());
    RmtObjectManager *om = tu.get_objmgr();
    Phv *phv = om->phv_create();
    phv->set(Phv::make_word_mapped(0,0), 0x99999990u);
    ASSERT_TRUE((phv->get(Phv::make_word_mapped(0,0)) == 0x99999990u));
    ASSERT_TRUE(phv->is_valid(Phv::make_word_mapped(0,0)));
    ASSERT_FALSE(phv->has_error(Phv::make_word_mapped(0,0)));
    phv->set_error(Phv::make_word_mapped(0,0));
    ASSERT_FALSE(phv->is_valid(Phv::make_word_mapped(0,0)));
    ASSERT_TRUE(phv->has_error(Phv::make_word_mapped(0,0)));

    phv->set(Phv::make_word_mapped(2,2), 0x92u);
    ASSERT_TRUE((phv->get(Phv::make_word_mapped(2,2)) == 0x92u));
    ASSERT_TRUE(phv->is_valid(Phv::make_word_mapped(2,2)));
    ASSERT_FALSE(phv->has_error(Phv::make_word_mapped(2,2)));
    phv->set_error(Phv::make_word_mapped(2,2));
    ASSERT_FALSE(phv->is_valid(Phv::make_word_mapped(2,2)));
    ASSERT_TRUE(phv->has_error(Phv::make_word_mapped(2,2)));

    phv->set(Phv::make_word_mapped(4,4), 0x9994u);
    ASSERT_TRUE((phv->get(Phv::make_word_mapped(4,4)) == 0x9994u));
    ASSERT_TRUE(phv->is_valid(Phv::make_word_mapped(4,4)));
    ASSERT_FALSE(phv->has_error(Phv::make_word_mapped(4,4)));
    phv->set_error(Phv::make_word_mapped(4,4));
    ASSERT_FALSE(phv->is_valid(Phv::make_word_mapped(4,4)));
    ASSERT_TRUE(phv->has_error(Phv::make_word_mapped(4,4)));
  }

  TEST(BFN_TEST_NAME(PhvTest),MultiWriteDisableFlag) {
    // Removed phv multi-write flag - keep test code for moment
    return;
    GLOBAL_MODEL->Reset();
    if (phv_print) RMT_UT_LOG_INFO("test_phv_multi_write_disable_flag()\n");
    TestUtil tu(GLOBAL_MODEL.get());
    RmtObjectManager *om = tu.get_objmgr();
    Phv *phv = om->phv_create();
    //phv->set_multi_write_disabled(Phv::make_word_mapped(0,0));
    phv->set(Phv::make_word_mapped(0,0), 0x99999990u);
    ASSERT_TRUE(phv->is_valid(Phv::make_word_mapped(0,0)));
    ASSERT_FALSE(phv->has_error(Phv::make_word_mapped(0,0)));
    ASSERT_EQ(0x99999990u, phv->get(Phv::make_word_mapped(0,0)));
    phv->set(Phv::make_word_mapped(0,0), 0x99999991u);
    ASSERT_TRUE(phv->is_valid(Phv::make_word_mapped(0,0)));
    ASSERT_TRUE(phv->has_error(Phv::make_word_mapped(0,0)));
    ASSERT_EQ(0x99999990u, phv->get(Phv::make_word_mapped(0,0)));

    //phv->set_multi_write_disabled(Phv::make_word_mapped(2,2));
    phv->set(Phv::make_word_mapped(2,2), 0x92u);
    ASSERT_TRUE(phv->is_valid(Phv::make_word_mapped(2,2)));
    ASSERT_FALSE(phv->has_error(Phv::make_word_mapped(2,2)));
    ASSERT_EQ(0x92u, phv->get(Phv::make_word_mapped(2,2)));
    phv->set(Phv::make_word_mapped(2,2), 0x93u);
    ASSERT_TRUE(phv->is_valid(Phv::make_word_mapped(2,2)));
    ASSERT_TRUE(phv->has_error(Phv::make_word_mapped(2,2)));
    ASSERT_EQ(0x92u, phv->get(Phv::make_word_mapped(2,2)));

    //phv->set_multi_write_disabled(Phv::make_word_mapped(4,4));
    phv->set(Phv::make_word_mapped(4,4), 0x9994u);
    ASSERT_TRUE(phv->is_valid(Phv::make_word_mapped(4,4)));
    ASSERT_FALSE(phv->has_error(Phv::make_word_mapped(4,4)));
    ASSERT_EQ(0x9994u, phv->get(Phv::make_word_mapped(4,4)));
    phv->set(Phv::make_word_mapped(4,4), 0x9995u);
    ASSERT_TRUE(phv->is_valid(Phv::make_word_mapped(4,4)));
    ASSERT_TRUE(phv->has_error(Phv::make_word_mapped(4,4)));
    ASSERT_EQ(0x9994u, phv->get(Phv::make_word_mapped(4,4)));
  }

  TEST(BFN_TEST_NAME(PhvTest),Init) {
    GLOBAL_MODEL->Reset();
    if (phv_print) RMT_UT_LOG_INFO("test_phv_init()\n");
    TestUtil tu(GLOBAL_MODEL.get());
    RmtObjectManager *om = tu.get_objmgr();
    PhvFactory::kPhvInitAllValid = false;

    Phv *phv = om->phv_create();
    for (unsigned int i = 0; i < Phv::kWordsMax; i++) {
      EXPECT_TRUE(phv->is_valid_phv(i));
    }
    EXPECT_FALSE(phv->is_valid_phv(-1));
    EXPECT_FALSE(phv->is_valid_phv(Phv::kWordsMax));
    for (unsigned int i = 0; i < Phv::kWordsMax; i++) {
      EXPECT_FALSE(phv->is_valid(i));
      EXPECT_EQ(0u, phv->get(i));
    }
    for (unsigned int i = 0; i < Phv::kWordsMax; i++) {
      phv->set(i, i);
    }
    for (unsigned int i = 0; i < Phv::kWordsMax; i++) {
      EXPECT_TRUE(phv->is_valid(i));
      EXPECT_EQ(i, phv->get(i));
    }

    // Should get same with a clone which doesn't
    // copy data by default
    Phv *clone = phv->clone();
    for (unsigned int i = 0; i < Phv::kWordsMax; i++) {
      EXPECT_TRUE(clone->is_valid_phv(i));
    }
    EXPECT_FALSE(clone->is_valid_phv(-1));
    EXPECT_FALSE(clone->is_valid_phv(Phv::kWordsMax));
    for (unsigned int i = 0; i < Phv::kWordsMax; i++) {
      EXPECT_FALSE(clone->is_valid(i));
      EXPECT_EQ(0u, clone->get(i));
    }
    for (unsigned int i = 0; i < Phv::kWordsMax; i++) {
      clone->set(i, i);
    }
    for (unsigned int i = 0; i < Phv::kWordsMax; i++) {
      EXPECT_TRUE(clone->is_valid(i));
      EXPECT_EQ(i, clone->get(i));
    }

    // But if we clone and select data too
    // should find all words already populated
    Phv *clone2 = phv->clone(true);
    for (unsigned int i = 0; i < Phv::kWordsMax; i++) {
      EXPECT_TRUE(clone2->is_valid_phv(i));
    }
    EXPECT_FALSE(clone2->is_valid_phv(-1));
    EXPECT_FALSE(clone2->is_valid_phv(Phv::kWordsMax));
    for (unsigned int i = 0; i < Phv::kWordsMax; i++) {
      EXPECT_TRUE(clone2->is_valid(i));
      EXPECT_EQ(i, clone2->get(i));
    }

    // Create fresh PHV, and then populate by
    // selecting every other word from initial phv
    Phv *copy_odd = om->phv_create();
    for (unsigned int i = 0; i < Phv::kWordsMax; i++) {
      EXPECT_FALSE(copy_odd->is_valid(i));
      EXPECT_EQ(0u, copy_odd->get(i));
    }
    BitVector<Phv::kWordsMax> sel(UINT64_C(0xAAAAAAAAAAAAAAAA));
    copy_odd->copydata(phv, &sel);
    // Check PHV we copied from still OK
    for (unsigned int i = 0; i < Phv::kWordsMax; i++) {
      EXPECT_TRUE(phv->is_valid(i));
      EXPECT_EQ(i, phv->get(i));
    }
    // Check only odd words valid (with copied data)
    // Even words should all be invalid (with val 0)
    for (unsigned int i = 0; i < Phv::kWordsMax; i++) {
      if ((i % 2) == 0) {
        EXPECT_FALSE(copy_odd->is_valid(i));
        EXPECT_EQ(0u, copy_odd->get(i));
      } else {
        EXPECT_TRUE(copy_odd->is_valid(i));
        EXPECT_EQ(i, copy_odd->get(i));
      }
    }
  }

  TEST(BFN_TEST_NAME(PhvTest),InitExtended) {
    GLOBAL_MODEL->Reset();
    if (phv_print) RMT_UT_LOG_INFO("test_phv_init_extended()\n");
    TestUtil tu(GLOBAL_MODEL.get());
    RmtObjectManager *om = tu.get_objmgr();
    PhvFactory::kPhvInitAllValid = false;

    // A. Create fresh PHV then test 'normal' words
    //    then test extended words
    //
    Phv *phv = om->phv_create();

    for (unsigned int i = 0; i < Phv::kWordsMax; i++) {
      EXPECT_TRUE(phv->is_valid_phv(i));
      EXPECT_TRUE(phv->is_valid_phv_x(i));
      EXPECT_FALSE(phv->is_valid(i));
    }
    for (unsigned int i = 0; i < Phv::kWordsMaxExtended; i++) {
      if (Phv::which_width_x(i) == 0) { // Skip 'hole' [224,256]
        EXPECT_FALSE(phv->is_valid_phv_x(i));
        EXPECT_FALSE(phv->is_valid_phv(i));
      } else {
        EXPECT_TRUE(phv->is_valid_phv_x(i));
        if (i < Phv::kWordsMax) {
          EXPECT_TRUE(phv->is_valid_phv(i));
        } else {
          EXPECT_FALSE(phv->is_valid_phv(i));
        }
      }
      EXPECT_FALSE(phv->is_valid_x(i));
    }

    // Negative words should not be valid
    for (unsigned int i = 1; i < 1000; i++) {
      EXPECT_FALSE(phv->is_valid_phv(-i));
      EXPECT_FALSE(phv->is_valid_phv_x(-i));
    }
    // Words beyond Max/MaxExtended should not be valid
    for (unsigned int i = 0; i < 1000; i++) {
      EXPECT_FALSE(phv->is_valid_phv(Phv::kWordsMax + i));
      EXPECT_FALSE(phv->is_valid_phv_x(Phv::kWordsMaxExtended + i));
    }

    // Prior to any set operations no words are
    // valid and all gets return 0
    for (unsigned int i = 0; i < Phv::kWordsMax; i++) {
      EXPECT_FALSE(phv->is_valid(i));
      EXPECT_FALSE(phv->is_valid_x(i));
      EXPECT_EQ(0u, phv->get(i));
    }
    for (unsigned int i = 0; i < Phv::kWordsMaxExtended; i++) {
      EXPECT_FALSE(phv->is_valid(i));
      EXPECT_FALSE(phv->is_valid_x(i));
      // Skip 'hole' [224,256]
      if (Phv::which_width_x(i) > 0) {
        EXPECT_EQ(0u, phv->get_x(i));
      }
    }

    // Initialise PHV normal words - they should
    // all become valid and we should get what we set
    for (unsigned int i = 0; i < Phv::kWordsMax; i++) {
      phv->set(i, (i % 256));
    }
    for (unsigned int i = 0; i < Phv::kWordsMax; i++) {
      EXPECT_TRUE(phv->is_valid(i));
      EXPECT_EQ((i % 256), phv->get(i));
    }

    // Initialise whole PHV including extended words
    // (but skip 'hole' from [224,255]) - all words
    // (apart from ones in 'hole') should be valid
    // and we should get what we set
       for (unsigned int i = 0; i < Phv::kWordsMaxExtended; i++) {
      // Skip 'hole' [224,256]
      if (Phv::which_width_x(i) > 0) phv->set_x(i, (i % 256));
    }
    for (unsigned int i = 0; i < Phv::kWordsMaxExtended; i++) {
      if (Phv::which_width_x(i) > 0) {
        EXPECT_TRUE(phv->is_valid_x(i));
        EXPECT_EQ((i % 256), phv->get_x(i));
      }
    }





    // B. Clone PHV = Should get same results as above
    //    with a clone PHV which doesn't copy data by default
    //
    Phv *clone = phv->clone();

    for (unsigned int i = 0; i < Phv::kWordsMax; i++) {
      EXPECT_TRUE(clone->is_valid_phv(i));
      EXPECT_TRUE(clone->is_valid_phv_x(i));
      EXPECT_FALSE(clone->is_valid(i));
    }
    for (unsigned int i = 0; i < Phv::kWordsMaxExtended; i++) {
      if (Phv::which_width_x(i) == 0) { // Skip 'hole' [224,256]
        EXPECT_FALSE(clone->is_valid_phv_x(i));
        EXPECT_FALSE(clone->is_valid_phv(i));
      } else {
        EXPECT_TRUE(clone->is_valid_phv_x(i));
        if (i < Phv::kWordsMax) {
          EXPECT_TRUE(clone->is_valid_phv(i));
        } else {
          EXPECT_FALSE(clone->is_valid_phv(i));
        }
      }
      EXPECT_FALSE(clone->is_valid_x(i));
    }

    // Negative words should not be valid
    for (unsigned int i = 1; i < 1000; i++) {
      EXPECT_FALSE(clone->is_valid_phv(-i));
      EXPECT_FALSE(clone->is_valid_phv_x(-i));
    }
    // Words beyond Max/MaxExtended should not be valid
    for (unsigned int i = 0; i < 1000; i++) {
      EXPECT_FALSE(clone->is_valid_phv(Phv::kWordsMax + i));
      EXPECT_FALSE(clone->is_valid_phv_x(Phv::kWordsMaxExtended + i));
    }

    // Prior to any set operations no words are
    // valid and all gets return 0
    for (unsigned int i = 0; i < Phv::kWordsMax; i++) {
      EXPECT_FALSE(clone->is_valid(i));
      EXPECT_FALSE(clone->is_valid_x(i));
      EXPECT_EQ(0u, clone->get(i));
    }
    for (unsigned int i = 0; i < Phv::kWordsMaxExtended; i++) {
      EXPECT_FALSE(clone->is_valid(i));
      EXPECT_FALSE(clone->is_valid_x(i));
      // Skip 'hole' [224,256]
      if (Phv::which_width_x(i) > 0) {
        EXPECT_EQ(0u, clone->get_x(i));
      }
    }

    // Initialise PHV normal words - they should
    // all become valid and we should get what we set
    for (unsigned int i = 0; i < Phv::kWordsMax; i++) {
      clone->set(i, (i % 256));
    }
    for (unsigned int i = 0; i < Phv::kWordsMax; i++) {
      EXPECT_TRUE(clone->is_valid(i));
      EXPECT_TRUE(clone->is_valid_x(i));
      EXPECT_EQ((i % 256), clone->get(i));
      EXPECT_EQ((i % 256), clone->get_x(i));
    }

    // Initialise whole PHV including extended words
    // (but skip 'hole' from [224,255]) - all words
    // (apart from ones in 'hole') should be valid
    // and we should get what we set

    for (unsigned int i = 0; i < Phv::kWordsMaxExtended; i++) {
      // Skip 'hole' [224,256]
      if (Phv::which_width_x(i) > 0) clone->set_x(i, (i % 256));
    }
    for (unsigned int i = 0; i < Phv::kWordsMaxExtended; i++) {
      if (Phv::which_width_x(i) > 0) {
        EXPECT_TRUE(clone->is_valid_x(i));
        EXPECT_EQ((i % 256), clone->get_x(i));
      }
    }




    // C. CloneX PHV = Should get same results as above
    //    with a clone_x PHV which doesn't copy data by default
    //
    Phv *clone_x = phv->clone_x();

    for (unsigned int i = 0; i < Phv::kWordsMax; i++) {
      EXPECT_TRUE(clone_x->is_valid_phv(i));
      EXPECT_TRUE(clone_x->is_valid_phv_x(i));
      EXPECT_FALSE(clone_x->is_valid(i));
    }
    for (unsigned int i = 0; i < Phv::kWordsMaxExtended; i++) {
      if (Phv::which_width_x(i) == 0) { // Skip 'hole' [224,256]
        EXPECT_FALSE(clone_x->is_valid_phv_x(i));
        EXPECT_FALSE(clone_x->is_valid_phv(i));
      } else {
        EXPECT_TRUE(clone_x->is_valid_phv_x(i));
        if (i < Phv::kWordsMax) {
          EXPECT_TRUE(clone_x->is_valid_phv(i));
        } else {
          EXPECT_FALSE(clone_x->is_valid_phv(i));
        }
      }
      EXPECT_FALSE(clone_x->is_valid_x(i));
    }

    // Negative words should not be valid
    for (unsigned int i = 1; i < 1000; i++) {
      EXPECT_FALSE(clone_x->is_valid_phv(-i));
      EXPECT_FALSE(clone_x->is_valid_phv_x(-i));
    }
    // Words beyond Max/MaxExtended should not be valid
    for (unsigned int i = 0; i < 1000; i++) {
      EXPECT_FALSE(clone_x->is_valid_phv(Phv::kWordsMax + i));
      EXPECT_FALSE(clone_x->is_valid_phv_x(Phv::kWordsMaxExtended + i));
    }

    // Prior to any set operations no words are
    // valid and all gets return 0
    for (unsigned int i = 0; i < Phv::kWordsMax; i++) {
      EXPECT_FALSE(clone_x->is_valid(i));
      EXPECT_FALSE(clone_x->is_valid_x(i));
      EXPECT_EQ(0u, clone_x->get(i));
    }
    for (unsigned int i = 0; i < Phv::kWordsMaxExtended; i++) {
      EXPECT_FALSE(clone_x->is_valid(i));
      EXPECT_FALSE(clone_x->is_valid_x(i));
      // Skip 'hole' [224,256]
      if (Phv::which_width_x(i) > 0) {
        EXPECT_EQ(0u, clone_x->get_x(i));
      }
    }

    // Initialise PHV normal words - they should
    // all become valid and we should get what we set
    for (unsigned int i = 0; i < Phv::kWordsMax; i++) {
      clone_x->set(i, (i % 256));
    }
    for (unsigned int i = 0; i < Phv::kWordsMax; i++) {
      EXPECT_TRUE(clone_x->is_valid(i));
      EXPECT_TRUE(clone_x->is_valid_x(i));
      EXPECT_EQ((i % 256), clone_x->get(i));
      EXPECT_EQ((i % 256), clone_x->get_x(i));
    }

    // Initialise whole PHV including extended words
    // (but skip 'hole' from [224,255]) - all words
    // (apart from ones in 'hole') should be valid
    // and we should get what we set
    for (unsigned int i = 0; i < Phv::kWordsMaxExtended; i++) {
      // Skip 'hole' [224,256]
      if (Phv::which_width_x(i) > 0) clone_x->set_x(i, (i % 256));
    }
    for (unsigned int i = 0; i < Phv::kWordsMaxExtended; i++) {
      // Skip 'hole' [224,256]
      if (Phv::which_width_x(i) > 0) {
        EXPECT_TRUE(clone_x->is_valid_x(i));
        EXPECT_EQ((i % 256), clone_x->get_x(i));
      }
    }




    // D. But if we clone and select data too
    //    should find normal words already populated
    //    but extended words should be 0
    //
    Phv *clone2 = phv->clone(true);

    for (unsigned int i = 0; i < Phv::kWordsMaxExtended; i++) {
      if (i < Phv::kWordsMax) {
        EXPECT_TRUE(clone2->is_valid(i));
        EXPECT_TRUE(clone2->is_valid_x(i));
        EXPECT_EQ((i % 256), clone2->get(i));
        EXPECT_EQ((i % 256), clone2->get_x(i));
      } else {
        // In extended word range
        if (Phv::which_width_x(i) > 0) {
          EXPECT_FALSE(clone2->is_valid_x(i));
          EXPECT_EQ(0u, clone2->get_x(i));
        }
      }
    }




    // E. But if we clone_x and select data too
    //    should find normal words AND extended words
    //    already populated
    //
    Phv *clone2_x = phv->clone_x(true);

    for (unsigned int i = 0; i < Phv::kWordsMaxExtended; i++) {
      if (i < Phv::kWordsMax) {
        EXPECT_TRUE(clone2_x->is_valid(i));
        EXPECT_TRUE(clone2_x->is_valid_x(i));
        EXPECT_EQ((i % 256), clone2_x->get(i));
        EXPECT_EQ((i % 256), clone2_x->get_x(i));
      } else {
        // In extended word range
        if (Phv::which_width_x(i) > 0) {
          EXPECT_TRUE(clone2_x->is_valid_x(i));
          EXPECT_EQ((i % 256), clone2_x->get_x(i));
        }
      }
    }




    // F. Create fresh PHV, and then populate by
    //    selecting every other word from initial phv
    ///
    Phv *copy_odd = om->phv_create();
    for (unsigned int i = 0; i < Phv::kWordsMax; i++) {
      EXPECT_FALSE(copy_odd->is_valid(i));
      EXPECT_EQ(0u, copy_odd->get(i));
    }
    BitVector<Phv::kWordsMax> sel(UINT64_C(0xAAAAAAAAAAAAAAAA));
    copy_odd->copydata(phv, &sel);
    // Check PHV we copied from still OK
    for (unsigned int i = 0; i < Phv::kWordsMax; i++) {
      EXPECT_TRUE(phv->is_valid(i));
      EXPECT_EQ((i % 256), phv->get(i));
    }
    // Check only odd words valid (with copied data)
    // Even words should all be invalid (with val 0)
    for (unsigned int i = 0; i < Phv::kWordsMax; i++) {
      if ((i % 2) == 0) {
        EXPECT_FALSE(copy_odd->is_valid(i));
        EXPECT_EQ(0u, copy_odd->get(i));
      } else {
        EXPECT_TRUE(copy_odd->is_valid(i));
        EXPECT_EQ((i % 256), copy_odd->get(i));
      }
    }




    // G. Create fresh PHV, and then populate by
    //    selecting every other word from initial phv
    //    this time using copydata_x
    //
    Phv *copy_odd_x = om->phv_create();

    // Check initial state
    for (unsigned int i = 0; i < Phv::kWordsMaxExtended; i++) {
      if (Phv::which_width_x(i) == 0) { // Skip 'hole' [224,256]
        EXPECT_FALSE(copy_odd_x->is_valid_phv_x(i));
        EXPECT_FALSE(copy_odd_x->is_valid_phv(i));
      } else if (i < Phv::kWordsMax) {
        EXPECT_TRUE(copy_odd_x->is_valid_phv_x(i));
        EXPECT_TRUE(copy_odd_x->is_valid_phv(i));
        EXPECT_FALSE(copy_odd_x->is_valid_x(i));
        EXPECT_FALSE(copy_odd_x->is_valid(i));
        EXPECT_EQ(0u, copy_odd_x->get_x(i));
        EXPECT_EQ(0u, copy_odd_x->get(i));
      } else {
        EXPECT_TRUE(copy_odd_x->is_valid_phv_x(i));
        EXPECT_FALSE(copy_odd_x->is_valid_phv(i));
        EXPECT_FALSE(copy_odd_x->is_valid_x(i));
        EXPECT_FALSE(copy_odd_x->is_valid(i));
        EXPECT_EQ(0u, copy_odd_x->get_x(i));
      }
    }

    // Copy over every other word
    BitVector<Phv::kWordsMaxExtended> selx(UINT64_C(0xAAAAAAAAAAAAAAAA));
    copy_odd_x->copydata_x(phv, &selx);

    // Check PHV we copied *from* still OK
    for (unsigned int i = 0; i < Phv::kWordsMax; i++) {
      EXPECT_TRUE(phv->is_valid(i));
      EXPECT_EQ((i % 256), phv->get(i));
    }
    for (unsigned int i = 0; i < Phv::kWordsMaxExtended; i++) {
      // Skip 'hole' [224,256]
      if (Phv::which_width_x(i) > 0) {
        EXPECT_TRUE(phv->is_valid_x(i));
        EXPECT_EQ((i % 256), phv->get_x(i));
      }
    }

    // In copy_odd_x check only odd words valid (with copied data)
    // Even words should all be invalid (with val 0)
    //
    for (unsigned int i = 0; i < Phv::kWordsMaxExtended; i++) {
      if (Phv::which_width_x(i) == 0) { // Skip 'hole' [224,256]
        EXPECT_FALSE(copy_odd_x->is_valid_phv_x(i));
        EXPECT_FALSE(copy_odd_x->is_valid_phv(i));
      } else if (i < Phv::kWordsMax) {
          EXPECT_TRUE(copy_odd_x->is_valid_phv_x(i));
          EXPECT_TRUE(copy_odd_x->is_valid_phv(i));
          if ((i % 2) == 0) {
            EXPECT_FALSE(copy_odd_x->is_valid_x(i));
            EXPECT_FALSE(copy_odd_x->is_valid(i));
            EXPECT_EQ(0u, copy_odd_x->get_x(i));
            EXPECT_EQ(0u, copy_odd_x->get(i));
          } else {
            EXPECT_TRUE(copy_odd_x->is_valid_x(i));
            EXPECT_TRUE(copy_odd_x->is_valid(i));
            EXPECT_EQ((i % 256), copy_odd_x->get_x(i));
            EXPECT_EQ((i % 256), copy_odd_x->get(i));
          }
      } else {
        EXPECT_TRUE(copy_odd_x->is_valid_phv_x(i));
        EXPECT_FALSE(copy_odd_x->is_valid_phv(i));
        if ((i % 2) == 0) {
          EXPECT_FALSE(copy_odd_x->is_valid_x(i));
          EXPECT_EQ(0u, copy_odd_x->get_x(i));
        } else {
          EXPECT_TRUE(copy_odd_x->is_valid_x(i));
          EXPECT_EQ((i % 256), copy_odd_x->get_x(i));
        }
      }
    }



    // H. Check widths of normal words and extended words
    Phv *phv2 = om->phv_create();

    phv2->set(Phv::make_word_mapped(0,0), 0x99999990u);
    EXPECT_EQ(0x99999990u, phv2->get(Phv::make_word_mapped(0,0)));
    phv2->set(Phv::make_word_mapped(1,1), 0x99999991u);
    EXPECT_EQ(0x99999991u, phv2->get(Phv::make_word_mapped(1,1)));

    phv2->set(Phv::make_word_mapped(2,2), 0x99999992u);
    EXPECT_EQ(0x92u, phv2->get(Phv::make_word_mapped(2,2)));
    phv2->set(Phv::make_word_mapped(3,3), 0x99999993u);
    EXPECT_EQ(0x93u, phv2->get(Phv::make_word_mapped(3,3)));

    phv2->set(Phv::make_word_mapped(4,4), 0x99999994u);
    EXPECT_EQ(0x9994u, phv2->get(Phv::make_word_mapped(4,4)));
    phv2->set(Phv::make_word_mapped(5,5), 0x99999995u);
    EXPECT_EQ(0x9995u, phv2->get(Phv::make_word_mapped(5,5)));
    phv2->set(Phv::make_word_mapped(6,6), 0x99999996u);
    EXPECT_EQ(0x9996u, phv2->get(Phv::make_word_mapped(6,6)));

    // Make an extended phvWord by hand and if it's valid
    // check other extended words
    int phv_word = Phv::kWordsPerGroup * 8;
    if (Phv::is_valid_phv_x(phv_word)) {
      phv2->set_x(Phv::make_word_mapped(8,8), 0x99999998u);
      EXPECT_EQ(0x99999998u, phv2->get_x(Phv::make_word_mapped(8,8)));

      phv2->set_x(Phv::make_word_mapped(9,9), 0x99999999u);
      EXPECT_EQ(0x99u, phv2->get_x(Phv::make_word_mapped(9,9)));

      phv2->set_x(Phv::make_word_mapped(10,10), 0x99999910u);
      EXPECT_EQ(0x9910u, phv2->get_x(Phv::make_word_mapped(10,10)));
      phv2->set_x(Phv::make_word_mapped(11,11), 0x99999911u);
      EXPECT_EQ(0x9911u, phv2->get_x(Phv::make_word_mapped(11,11)));
    }

    if (phv_print) {
      for (unsigned int i = 0; i < Phv::kWordsMaxExtended; i++) {
        if (((i % 32) == ((i / 32) % 32)) && (Phv::is_valid_phv_x(i))) {
          printf("Phv::make_word_mapped(%d,%d) = 0x%08x\n",
                 ((i/32)%32), (i%32), phv2->get_x(i));
        }
      }
    }
  }


  TEST(BFN_TEST_NAME(PhvTest),IndexToOff16) {
    GLOBAL_MODEL->Reset();
    if (phv_print) RMT_UT_LOG_INFO("test_phv_index_to_off16()\n");
    TestUtil tu(GLOBAL_MODEL.get());
    int phvWordA, phvWordB, size, off16A, off16B, sz8_01, sz32_01;

    // First of all confirm some know mappings
    size = off16A = off16B = sz8_01 = sz32_01 = -1;
    Phv::phv_index_to_off16_p(0, &size, &off16A, &off16B, &sz8_01);
    EXPECT_EQ(0, off16A);
    EXPECT_EQ(32, size);
    EXPECT_EQ(-99999, sz8_01);

    size = off16A = off16B = sz8_01 = sz32_01 = -1;
    Phv::phv_index_to_off16_p(223, &size, &off16A, &off16B, &sz8_01);
    EXPECT_EQ(255, off16A);
    EXPECT_EQ(16, size);
    EXPECT_EQ(99999, sz8_01);

    size = off16A = off16B = sz8_01 = sz32_01 = -1;
    Phv::phv_index_to_off16_p(64, &size, &off16A, &off16B, &sz8_01);
    EXPECT_EQ(128, off16A);
    EXPECT_EQ(8, size);
    EXPECT_EQ(0, sz8_01);

    size = off16A = off16B = sz8_01 = sz32_01 = -1;
    Phv::phv_index_to_off16_p(128, &size, &off16A, &off16B, &sz8_01);
    EXPECT_EQ(160, off16A);
    EXPECT_EQ(16, size);
    EXPECT_EQ(99999, sz8_01);

    // And back
    phvWordA = phvWordB = size = off16A = off16B = sz32_01 = -1;
    Phv::phv_off16_to_index_p(0, &size, &phvWordA, &phvWordB, &sz32_01);
    EXPECT_EQ(0, phvWordA);
    EXPECT_EQ(-1, phvWordB);
    EXPECT_EQ(32, size);
    EXPECT_EQ(0, sz32_01);

    phvWordA = phvWordB = size = off16A = off16B = sz32_01 = -1;
    Phv::phv_off16_to_index_p(255, &size, &phvWordA, &phvWordB, &sz32_01);
    EXPECT_EQ(223, phvWordA);
    EXPECT_EQ(-1, phvWordB);
    EXPECT_EQ(16, size);
    EXPECT_EQ(99999, sz32_01);

    phvWordA = phvWordB = size = off16A = off16B = sz32_01 = -1;
    Phv::phv_off16_to_index_p(128, &size, &phvWordA, &phvWordB, &sz32_01);
    EXPECT_EQ(64, phvWordA);
    EXPECT_EQ(65, phvWordB);
    EXPECT_EQ(8, size);
    EXPECT_EQ(-99999, sz32_01);

    phvWordA = phvWordB = size = off16A = off16B = sz32_01 = -1;
    Phv::phv_off16_to_index_p(160, &size, &phvWordA, &phvWordB, &sz32_01);
    EXPECT_EQ(128, phvWordA);
    EXPECT_EQ(-1, phvWordB);
    EXPECT_EQ(16, size);
    EXPECT_EQ(99999, sz32_01);

    // And finally with an odd 8b PHV
    int size1 = -1, size2 = -1;
    phvWordA = phvWordB = size1 = size2 = off16A = off16B = sz8_01 = sz32_01 = -1;
    Phv::phv_index_to_off16_p(65, &size1, &off16A, &off16B, &sz8_01);
    Phv::phv_off16_to_index_p(off16A, &size2, &phvWordA, &phvWordB, &sz32_01);
    EXPECT_EQ(1, sz8_01);
    EXPECT_EQ(64, phvWordA);
    EXPECT_EQ(65, phvWordB);
    EXPECT_EQ(8, size1);
    EXPECT_EQ(8, size2);
    // And the next even
    phvWordA = phvWordB = size1 = size2 = off16A = off16B = sz8_01 = sz32_01 = -1;
    Phv::phv_index_to_off16_p(66, &size1, &off16A, &off16B, &sz8_01);
    Phv::phv_off16_to_index_p(off16A, &size2, &phvWordA, &phvWordB, &sz32_01);
    EXPECT_EQ(0, sz8_01);
    EXPECT_EQ(66, phvWordA);
    EXPECT_EQ(67, phvWordB);
    EXPECT_EQ(8, size1);
    EXPECT_EQ(8, size2);

    // Now exhaustively test all possible phvWords
    for (int p = 0; p < 224; p++) {
      phvWordA = phvWordB = size1 = size2 = off16A = off16B = sz8_01 = sz32_01 = -1;
      Phv::phv_index_to_off16_p(p, &size1, &off16A, &off16B, &sz8_01);
      Phv::phv_off16_to_index_p(off16A, &size2, &phvWordA, &phvWordB, &sz32_01);

      EXPECT_EQ(size1, size2);
      if (size1 == 8) {
        EXPECT_EQ(phvWordA+1, phvWordB);
        if      (sz8_01 == 0) EXPECT_EQ(p, phvWordA);
        else if (sz8_01 == 1) EXPECT_EQ(p, phvWordB);
        else RMT_ASSERT(0);
      } else {
        EXPECT_EQ(-1, phvWordB);
        EXPECT_EQ(p, phvWordA);
      }
    }
    // And then exhaustively test all possible offsets
    for (int off = 0; off < 256; off++) {
      int off16A = -1;
      phvWordA = phvWordB = size1 = size2 = off16A = off16B = sz8_01 = sz32_01 = -1;
      Phv::phv_off16_to_index_p(off, &size1, &phvWordA, &phvWordB, &sz32_01);

      if (size1 == 8) {
        EXPECT_EQ(phvWordA+1, phvWordB);

        off16A = off16B = size2 = sz8_01 = -1;
        Phv::phv_index_to_off16_p(phvWordA, &size2, &off16A, &off16B, &sz8_01);
        EXPECT_EQ(size1, size2);
        EXPECT_EQ(off, off16A);
        EXPECT_EQ(0, sz8_01);

        off16A = off16B = size2 = sz8_01 = -1;
        Phv::phv_index_to_off16_p(phvWordB, &size2, &off16A, &off16B, &sz8_01);
        EXPECT_EQ(size1, size2);
        EXPECT_EQ(off, off16A);
        EXPECT_EQ(-1, off16B);
        EXPECT_EQ(1, sz8_01);

      } else if (size1 == 16) {
        EXPECT_EQ(-1, phvWordB);

        off16A = off16B = size2 = sz8_01 = -1;
        Phv::phv_index_to_off16_p(phvWordA, &size2, &off16A, &off16B, &sz8_01);
        EXPECT_EQ(size1, size2);
        EXPECT_EQ(off, off16A);
        EXPECT_EQ(-1, off16B);
        EXPECT_EQ(99999, sz8_01);

      } else {
        EXPECT_EQ(32, size1);
        EXPECT_EQ(-1, phvWordB);

        off16A = off16B = size2 = sz8_01 = -1;
        Phv::phv_index_to_off16_p(phvWordA, &size2, &off16A, &off16B, &sz8_01);
        EXPECT_EQ(size1, size2);
        if      (sz32_01 == 0) EXPECT_EQ(off, off16A);
        else if (sz32_01 == 1) EXPECT_EQ(off, off16B);
        else RMT_ASSERT(0);
        EXPECT_EQ(-99999, sz8_01);
      }
    }
  }

  class BFN_TEST_NAME(BasePhvTest) : public BaseTest {
   public:
    Phv *phv_;

    void SetUp() {
      BaseTest::SetUp();
      //init a phv
      phv_ = om_->phv_create();
      phv_->set_pipe(pipe_index());
      phv_->set(Phv::make_word_mapped(0,0), 0x99999990u);
      ASSERT_EQ(phv_->get(Phv::make_word_mapped(0,0)), 0x99999990u);
      phv_->set(Phv::make_word_mapped(0,14), 0xabcdef01u);
      ASSERT_EQ(phv_->get(Phv::make_word_mapped(0,14)), 0xabcdef01u);
      phv_->set(Phv::make_word_mapped(0,15), 0x55555555u);  // jbay -> POV
      ASSERT_EQ(phv_->get(Phv::make_word_mapped(0,15)), 0x55555555u);
      phv_->set(Phv::make_word_mapped(2,2), 0x99999991u);
      ASSERT_EQ(phv_->get(Phv::make_word_mapped(2,2)), 0x91u);
      phv_->set(Phv::make_word_mapped(3,3), 0x99999992u);
      ASSERT_EQ(phv_->get(Phv::make_word_mapped(3,3)), 0x92u);
      phv_->set(Phv::make_word_mapped(4,4), 0x99999993u);
      ASSERT_EQ(phv_->get(Phv::make_word_mapped(4,4)), 0x9993u);
    }

    void TearDown() {
      if(nullptr != phv_) om_->phv_delete(phv_);
      BaseTest::TearDown();
    }
  };


  TEST_F(BFN_TEST_NAME(BasePhvTest),PrintPhvWithoutP4Names) {
    EXPECT_FALSE(om_->p4_name_lookup(pipe_index()).IsLoaded());
    // enable logging
    om_->update_log_flags(ALL,ALL,ALL,ALL,ALL,RmtDebug::kRmtDebugPhv1,RmtDebug::kRmtDebugPhv1);
    RmtLoggerCapture *log_capture = rmt_logger_capture();
    log_capture->start();
    phv_->print_p("PHV DUMP P", false);
    log_capture->stop();
    int word = 0;
    if (RmtObject::is_jbay_or_later()) {
      auto line_checker = [&word](int line_num, size_t pos, std::string line)->void {
        switch (word) {
          case 0 :
            EXPECT_EQ("<202,1>    0(mapped=  0) [ 0, 0]<32> = 2576980368(0x99999990) JSON object not found.\n", line);
            break;
          case 14 :
            EXPECT_EQ("<202,1>   14(mapped= 14) [ 0,14]<32> = 2882400001(0xabcdef01) JSON object not found.\n", line);
            break;
          case 15 :
            EXPECT_EQ("<202,1>   15(mapped= 15) [ 0,15]<32> = 1431655765(0x55555555) JSON object not found.\n", line);
            break;
          case 66 :
            EXPECT_EQ("<202,1>   66(mapped= 82) [ 2, 2]<8> = 145(0x91) JSON object not found.\n", line);
            break;
          case 99 :
            EXPECT_EQ("<202,1>   99(mapped=123) [ 3, 3]<8> = 146(0x92) JSON object not found.\n", line);
            break;
          case 132 :
            EXPECT_EQ("<202,1>  132(mapped=164) [ 4, 4]<16> = 39315(0x9993) JSON object not found.\n", line);
            break;
          default:
            if (Phv::which_width_p(word) == 8) {
              EXPECT_TRUE(line.find("0(0x00)") != std::string::npos) << word << " " << line;
            } else if (Phv::which_width_p(word) == 16) {
              EXPECT_TRUE(line.find("0(0x0000)") != std::string::npos) << word << " " << line;
            } else {
              EXPECT_TRUE(line.find("0(0x00000000)") != std::string::npos) << word << " " << line;
            }
            break;
        }
        word++;
      };
      // filter out POV details...
      int line_count = log_capture->for_each_line_containing("mapped=", line_checker);
      EXPECT_EQ(224, line_count);
    } else {
      auto line_checker = [&word](int line_num, size_t pos, std::string line)->void {
        switch (word) {
          case 0 :
            EXPECT_EQ("<202,1>    0(mapped=  0) [ 0, 0]<32> = 2576980368(0x99999990) JSON object not found.\n", line);
            break;
          case 14 :
            EXPECT_EQ("<202,1>   14(mapped= 14) [ 0,14]<32> = 2882400001(0xabcdef01) JSON object not found.\n", line);
            break;
          case 15 :
            EXPECT_EQ("<202,1>   15(mapped= 15) [ 0,15]<32> = 1431655765(0x55555555) JSON object not found.\n", line);
            break;
          case 66 :
            EXPECT_EQ("<202,1>   66(mapped= 66) [ 2, 2]<8> = 145(0x91) JSON object not found.\n", line);
            break;
          case 99 :
            EXPECT_EQ("<202,1>   99(mapped= 99) [ 3, 3]<8> = 146(0x92) JSON object not found.\n", line);
            break;
          case 132 :
            EXPECT_EQ("<202,1>  132(mapped=132) [ 4, 4]<16> = 39315(0x9993) JSON object not found.\n", line);
            break;
          default:
            if (Phv::which_width_p(word) == 8) {
              EXPECT_TRUE(line.find("0(0x00)") != std::string::npos) << word << " " << line;
            } else if (Phv::which_width_p(word) == 16) {
              EXPECT_TRUE(line.find("0(0x0000)") != std::string::npos) << word << " " << line;
            } else {
              EXPECT_TRUE(line.find("0(0x00000000)") != std::string::npos) << word << " " << line;
            }
            break;
        }
        word++;
        if (word == 224) word = 256;
      };
      // filter out POV details...
      int line_count = log_capture->for_each_line_containing("mapped=", line_checker);
      EXPECT_EQ(336, line_count);
    }
    if (HasNonfatalFailure()) {
      std::cout << "Non-fatal error occurred, captured log lines:\n" << log_capture->dump_lines();
      phv_->print_p("repeated PHV DUMP P", false);
    }
  }

  TEST_F(BFN_TEST_NAME(BasePhvTest),PrintPhvWithP4Names) {
    load_context_json_file(pipe_index());
    EXPECT_TRUE(om_->p4_name_lookup(pipe_index()).IsLoaded());
    om_->update_log_flags(ALL,ALL,ALL,ALL,ALL,RmtDebug::kRmtDebugPhv1,RmtDebug::kRmtDebugPhv1);
    RmtLoggerCapture *log_capture = rmt_logger_capture();
    log_capture->start();
    phv_->print_p("PHV DUMP P", false);
    log_capture->stop();
    int word = 0;
    if (RmtObject::is_jbay_or_later()) {
      auto line_checker_p4 = [&word](int line_num, size_t pos, std::string line)->void {
        switch (word) {
          case 0 :
            EXPECT_EQ("<202,1>    0(mapped=  0) [ 0, 0]<32> = 2576980368(0x99999990) I [my_metadata.da_lo_32[31:0]]\n", line);
            break;
          case 14 :
            EXPECT_EQ("<202,1>   14(mapped= 14) [ 0,14]<32> = 2882400001(0xabcdef01) I [junk_word_14_a[7:0], junk_word_14_b[7:0]]\n", line);
            break;
          case 15 :
            EXPECT_EQ("<202,1>   15(mapped= 15) [ 0,15]<32> = 1431655765(0x55555555) I [other_mixed_with_POV[2:0], POV]\n", line);
            break;
          case 66 :
            EXPECT_EQ("<202,1>   66(mapped= 82) [ 2, 2]<8> = 145(0x91) I [my_metadata.ip4_ttl[7:0]]\n", line);
            break;
          case 99 :
            EXPECT_EQ("<202,1>   99(mapped=123) [ 3, 3]<8> = 146(0x92) I [phv8_3[7:0]]\n", line);
            break;
          case 132 :
            EXPECT_EQ("<202,1>  132(mapped=164) [ 4, 4]<16> = 39315(0x9993) I [my_metadata.ip4_len[7:0]]\n", line);
            break;
          default:
            if (Phv::which_width_p(word) == 8) {
              EXPECT_TRUE(line.find("0(0x00)") != std::string::npos) << word << " " << line;
            } else if (Phv::which_width_p(word) == 16) {
              EXPECT_TRUE(line.find("0(0x0000)") != std::string::npos) << word << " " << line;
            } else {
              EXPECT_TRUE(line.find("0(0x00000000)") != std::string::npos) << word << " " << line;
            }
            break;
        }
        word++;
      };
      // filter out POV details...
      int line_count = log_capture->for_each_line_containing("mapped=", line_checker_p4);
      EXPECT_EQ(224, line_count);
    } else {
      auto line_checker_p4 = [&word](int line_num, size_t pos, std::string line)->void {
        switch (word) {
          case 0 :
            EXPECT_EQ("<202,1>    0(mapped=  0) [ 0, 0]<32> = 2576980368(0x99999990) I [my_metadata.da_lo_32[31:0]]\n", line);
            break;
          case 14 :
            EXPECT_EQ("<202,1>   14(mapped= 14) [ 0,14]<32> = 2882400001(0xabcdef01) I [junk_word_14_a[7:0], junk_word_14_b[7:0]]\n", line);
            break;
          case 15 :
            EXPECT_EQ("<202,1>   15(mapped= 15) [ 0,15]<32> = 1431655765(0x55555555) I [other_mixed_with_POV[2:0], POV]\n", line);
            break;
          case 66 :
            EXPECT_EQ("<202,1>   66(mapped= 66) [ 2, 2]<8> = 145(0x91) I [my_metadata.ip4_ttl[7:0]]\n", line);
            break;
          case 99 :
            EXPECT_EQ("<202,1>   99(mapped= 99) [ 3, 3]<8> = 146(0x92) I [phv8_3[7:0]]\n", line);
            break;
          case 132 :
            EXPECT_EQ("<202,1>  132(mapped=132) [ 4, 4]<16> = 39315(0x9993) I [my_metadata.ip4_len[7:0]]\n", line);
            break;
          default:
            if (Phv::which_width_p(word) == 8) {
              EXPECT_TRUE(line.find("0(0x00)") != std::string::npos) << word << " " << line;
            } else if (Phv::which_width_p(word) == 16) {
              EXPECT_TRUE(line.find("0(0x0000)") != std::string::npos) << word << " " << line;
            } else {
              EXPECT_TRUE(line.find("0(0x00000000)") != std::string::npos) << word << " " << line;
            }
            break;
        }
        word++;
        if (word == 224) word = 256;
      };
      // filter out POV details...
      int line_count = log_capture->for_each_line_containing("mapped=", line_checker_p4);
      EXPECT_EQ(336, line_count);
    }
    if (HasNonfatalFailure()) {
      std::cout << "Non-fatal error occurred, captured log lines:\n" << log_capture->dump_lines();
      phv_->print_p("repeated PHV DUMP P", false);
    }
  }

  TEST_F(BFN_TEST_NAME(BasePhvTest),PrintPhvPOVFieldsWithP4Names) {
    load_context_json_file(pipe_index());
    EXPECT_TRUE(om_->p4_name_lookup(pipe_index()).IsLoaded());
    om_->update_log_flags(ALL,ALL,ALL,ALL,ALL,RmtDebug::kRmtDebugPhv1,RmtDebug::kRmtDebugPhv1);
    RmtLoggerCapture *log_capture = rmt_logger_capture();
    log_capture->start();
    phv_->print_p("PHV DUMP P", false);
    log_capture->stop();
    int word = 0;
    auto line_checker_p4 = [&word](int line_num, size_t pos, std::string line)->void {
      switch (word) {
        case 0 :
          EXPECT_EQ("<202,1>       15  POV bit  0 = 1  my_metadata.da_lo_32\n", line);
          break;
        case 1 :
          EXPECT_EQ("<202,1>       15  POV bit  1 = 0  my_metadata.ip4_src\n", line);
          break;
        case 2 :
          EXPECT_EQ("<202,1>       15  POV bit  2 = 1  my_metadata.sa_lo_32\n", line);
          break;
        case 3 :
          EXPECT_EQ("<202,1>       15  POV bit  3 = 0  my_metadata.ip4_dst\n", line);
          break;
        case 4 :
          EXPECT_EQ("<202,1>       15  POV bit  4 = 1  ipv4_option_32b\n", line);
          break;
        case 5 :
          EXPECT_EQ("<202,1>       15  POV bit  5 = 0  ipv6\n", line);
          break;
        case 6 :
          // checks that gaps in POV bits are ok
          EXPECT_EQ("<202,1>       15  POV bit 27 = 0  skipped_some_pov_bits__test\n", line);
          break;
        case 7 :
          // checks that multiple POV records in single container are found
          EXPECT_EQ("<202,1>       15  POV bit 28 = 1  multiple_pov_records\n", line);
          break;
        default:
          break;
      }
      word++;
    };
    int line_count = log_capture->for_each_line_containing("POV bit", line_checker_p4);
    EXPECT_EQ(8, line_count);
    if (HasNonfatalFailure()) {
      std::cout << "Non-fatal error occurred, captured log lines:\n" << log_capture->dump_lines();
      phv_->print_p("repeated PHV DUMP P", false);
    }
  }

  TEST_F(BFN_TEST_NAME(BasePhvTest),PrintPhvNoLogging) {
    // repeat with logging disabled
    om_->update_log_flags(ALL,ALL,ALL,ALL,ALL,FEW,FEW);
    RmtLoggerCapture *log_capture = rmt_logger_capture();
    log_capture->start();
    phv_->print_p("PHV DUMP P", false);
    log_capture->stop();
    int line_count = log_capture->for_each_line(nullptr);
    EXPECT_EQ(0, line_count);
  }

  TEST(BFN_TEST_NAME(PhvTest),PhvModification){
    GLOBAL_MODEL->Reset();
    if (phv_print) RMT_UT_LOG_INFO("test_phv_modification()\n");
    TestUtil tu(GLOBAL_MODEL.get());
    RmtObjectManager *om = tu.get_objmgr();
    if (phv_print) RMT_UT_LOG_INFO("Create PhvModification Object\n");

    uint64_t seed = 0x43154190u; int iters = 10000; bool verbose = false;
    bool xor_flag, or_flag, clr_flag;
    std::cout << "<seed=" << seed << "> <iters=" << iters << "> <verbose=" << verbose << ">\n";
    for (int k = 0; k < iters; k++) {
      xor_flag = (k % 2 == 0);
      or_flag  = (k % 3 == 0);
      clr_flag = (k % 5 == 0);
      
      // create phvs
      Phv* phv_t = om->phv_create(); 
      Phv* phv_m = om->phv_create(); 
      uint32_t t; 
      for (int i = 0; i < Phv::kWordsMaxExtended; i++) {
        t = tu.xrand32(seed, (iters * k) - i, 32);
        phv_t->set_x(i, t);
        phv_m->set_x(i, t);
      }
      
      // specify number of indices to change
      int xor_i = tu.xrandrange(seed, seed - k, 0, Phv::kWordsMaxExtended);
      int or_i  = tu.xrandrange(seed, seed + k, 0, Phv::kWordsMaxExtended);
      int clr_i = tu.xrandrange(seed, seed - k + 11u, 0, Phv::kWordsMaxExtended);
      if (verbose) {
        std::cout << "[Iteration: " << k << "] Actions: " << ((xor_flag) ? "XOR " : "") << ((or_flag) ? "OR " : "") << ((clr_flag) ? "CLR " : "") << "\n";
        if (xor_flag) std::cout << "\tXOR addresses: " << xor_i << "\n";
        if (or_flag)  std::cout << "\tOR addresses: "  << or_i  << "\n";
        if (clr_flag) std::cout << "\tCLR addresses: " << clr_i << "\n";
      }
      // MODEL_CORE PATH (phv_t)
      PhvModification* p_mod = new PhvModification(om, 0, 0, om->mau_lookup(0, 0));
      // XOR PHV
      if (xor_flag)
        for (int i = 0; i < xor_i; i++)
          p_mod->set_modification(model_core::RmtPhvModification::ActionEnum::kXor, tu.xrandrange(seed, k * i, 0, Phv::kWordsMaxExtended), tu.xrand32(seed, 41u + k + i, 32));
      // OR PHV
      if (or_flag)
        for (int i = 0; i < or_i; i++)
          p_mod->set_modification(model_core::RmtPhvModification::ActionEnum::kOr, tu.xrandrange(seed, k + 3 * i, 0, Phv::kWordsMaxExtended), tu.xrand32(seed, k * i, 32));
      // CLR PHV
      if (clr_flag) 
        for (int i = 0; i < clr_i; i++)
          p_mod->set_modification(model_core::RmtPhvModification::ActionEnum::kClr, tu.xrandrange(seed, k + i, 0, Phv::kWordsMaxExtended), tu.xrand32(seed, k + i, 32));
      
      p_mod->apply_modification(phv_t, false);

      // MANUAL PATH (phv_m)
      Phv* phv_xor = om->phv_create(); 
      Phv* phv_or = om->phv_create();
      Phv* phv_clr = om->phv_create(); 
      for (int i = 0; i < xor_i; i++) // XOR PHV
        phv_xor->clobber_x(tu.xrandrange(seed, k * i, 0, Phv::kWordsMaxExtended), tu.xrand32(seed, 41u + k + i, 32));
      for (int i = 0; i < or_i; i++) // OR PHV
        phv_or->clobber_x(tu.xrandrange(seed, k + 3 * i, 0, Phv::kWordsMaxExtended), tu.xrand32(seed, k * i, 32));
      for (int i = 0; i < clr_i; i++) // CLR PHV
        phv_clr->clobber_x(tu.xrandrange(seed, k + i, 0, Phv::kWordsMaxExtended), tu.xrand32(seed, k + i, 32));

      for (int i = 0; i < Phv::kWordsMaxExtended; i++) {
        if (clr_flag) phv_m->clobber_x(i, phv_m->get_x(i) & ~phv_clr->get_x(i));
        if (or_flag)  phv_m->set_x(i, phv_or->get_x(i));
        if (xor_flag) phv_m->clobber_x(i, phv_m->get_x(i) ^ phv_xor->get_x(i));
      }
      delete phv_xor; delete phv_or; delete phv_clr; // clean up manual

      ASSERT_TRUE(phv_t->equals(phv_m)); // check phvs are equal

      delete phv_t; delete phv_m; delete p_mod;
    }
  }
}
