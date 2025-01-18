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

#ifndef _TOFINOXX_DEPARSER_LEARNING_
#define _TOFINOXX_DEPARSER_LEARNING_

#include <string>
#include <cstdint>
#include <vector>
#include <pipe-object.h>
#include <packet.h>
#include <bitvector.h>
#include <deparser-metadata.h>

#include <register_includes/dprsr_learn_table_entry_r_array.h>
#include <register_includes/dprsr_learn_cfg_r.h>

namespace MODEL_CHIP_NAMESPACE {

  class Phv;

  class DeparserLearning : public PipeObject {

 public:
    DeparserLearning(RmtObjectManager *om, int pipeIndex);

    virtual ~DeparserLearning();

    void CalculateLearningQuantum(const Phv &phv, LearnQuantumType* lq);

 private:
    register_classes::DprsrLearnTableEntryRArray learn_table_entry_array_;
    register_classes::DprsrLearnCfgR             learn_cfg_;
  };
}
#endif // _TOFINOXX_DEPARSER_LEARNING_
