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

#ifndef _SHARED_LEARNING_FILTER_REG_
#define _SHARED_LEARNING_FILTER_REG_

#include <cstdint>
#include <array>
#include <vector>
#include <rmt-log.h>

#include <register_includes/lfltr_bft_ctrl_mutable.h>
#include <register_includes/lfltr_common_ctrl_mutable.h>
#include <register_includes/lfltr_ctr48_lq_dropped_state_mutable.h>
#include <register_includes/lfltr_ctrl_rspec.h>
#include <register_includes/lfltr_hash_array_array.h>
#include <register_includes/lfltr_hash_array.h>
#include <register_includes/lfltr_hash_rspec_array.h>
#include <register_includes/lfltr_hash_rspec.h>
#include <register_includes/lfltr_hash_seed_array.h>
#include <register_includes/lfltr_hash_seed.h>
// Not implemented - and now differently named in Tofino and JBay, so
//  would require splitting out if we wanted to implement them
//#include <register_includes/lfltr_intr_en0.h>
//#include <register_includes/lfltr_intr_en1.h>
//#include <register_includes/lfltr_intr_inj.h>
//#include <register_includes/lfltr_intr_stat.h>
#include <register_includes/lfltr_lqt_timeout.h>
#include <register_includes/lfltr_mbe_log.h>
#include <register_includes/lfltr_pbe_log.h>
#include <register_includes/lfltr_rspec.h>
#include <register_includes/lfltr_sbe_log.h>

namespace MODEL_CHIP_NAMESPACE {

// forward declarations
class LearningFilter;

class LfRegs {
  public:
    LfRegs(int chip=0, int pipe=0, LearningFilter *lf = 0);

    // keep them all public.. no special abstraction, other than just collecting,
    // them in one place
    // There is a auto generated class for all LearningFilter Related resgister in tofino_model.h, 
    // but it is not used here, just pull in the specific registers that are needed.
    register_classes::LfltrCommonCtrlMutable            common_ctrl_;
    register_classes::LfltrLqtTimeout                   lqt_timeout_;
    register_classes::LfltrBftCtrlMutable               bft_ctrl_;
    register_classes::LfltrCtr48LqDroppedStateMutable   lq_dropped_;
    register_classes::LfltrHashSeedArray                hash_seed_;

    register_classes::LfltrHashRspecArray               hash_;

    // currently not implemented/used in the model
  //register_classes::LfltrIntrStat         int_stat_;
  //register_classes::LfltrIntrEn0          int_en0_;
  //register_classes::LfltrIntrEn1          int_en1_;
  //register_classes::LfltrIntrInj          int_inj_;
    register_classes::LfltrPbeLog           pbe_log_;
    register_classes::LfltrSbeLog           sbe_log_;
    register_classes::LfltrMbeLog           mbe_log_;

private:
    // back ref to parent class
    LearningFilter                    *lf_;

    // callback for write to bft_ctrl register
    void                              bft_cltr_write_cb();

  };
} // namespace tofino

#endif // _SHARED_LEARNING_FILTER_REG_
