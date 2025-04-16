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

#include <bitvector.h>
#include <rmt-object-manager.h>
#include <model_core/model.h>
#include <mau.h>

extern std::unique_ptr<model_core::Model> GLOBAL_MODEL;


namespace MODEL_CHIP_TEST_NAMESPACE {

  bool rcp_print = false;

  using namespace std;
  using namespace MODEL_CHIP_NAMESPACE;


  TEST(BFN_TEST_NAME(RecipeTest),AccessMauTcam) {
    GLOBAL_MODEL->Reset();
    GLOBAL_MODEL->InitChip(0);
    if (rcp_print) RMT_UT_LOG_INFO("test_recipe_access_mau_tcam()\n");

    // NB. This stuff only works if you've programmed up the TCAM
    // with data and programmed up the TCAM regs/TCAM input xbar regs etc


    // Get an ObjectManager from the Host object you call InWord/OutWord
    // on - you need to specify which chip (normally 0)
    //
    int chip = 0;
    RmtObjectManager *objmgr = GLOBAL_MODEL->GetObjectManager(chip);

    // Once you have an objmgr you can get an MAU using mau_lookup
    // You pass a pipe (0-3) and an stage/mauIndex (0-11) and you'll
    // get back the MAU object
    int pipeIndex = 0;
    int mauIndex = 0;
    Mau *mau = objmgr->mau_lookup(pipeIndex, mauIndex);
    
    // To get a physical TCAM you then call tcam_lookup on the MAU
    // passing row/col - row in (0-15) col in (0-1)
    int rowIndex = 0;
    int colIndex = 0;
    MauTcam *tcam = mau->tcam_lookup(rowIndex, colIndex);
    
    // Finally to lookup a PHV in a TCAM call lookup(phv) - that will
    // return you the priority of the highest pri match NOT the index
    Phv *phv = NULL;
    int hit_pri = tcam->lookup(phv);
  }
}

