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

#include <learning-filter-reg.h>
#include <learning-filter.h>
#include <register_adapters.h>


namespace MODEL_CHIP_NAMESPACE {

LfRegs::LfRegs(int chip, int pipe, LearningFilter *lf) :
    common_ctrl_(learning_filter_adapter(common_ctrl_,chip, pipe)),
    lqt_timeout_(learning_filter_adapter(lqt_timeout_,chip, pipe)),
    bft_ctrl_(learning_filter_adapter(bft_ctrl_,chip, pipe, [this](){this->bft_cltr_write_cb();})),
    lq_dropped_(learning_filter_adapter(lq_dropped_,chip, pipe)),
    hash_seed_(learning_filter_adapter(hash_seed_,chip, pipe)),
    hash_(learning_filter_adapter(hash_,chip, pipe))
{
    lf_ = NULL; // avoid any callbacks.. not ready yet

    // initialize the registers to RESET values
    common_ctrl_.reset();
    lqt_timeout_.reset();
    bft_ctrl_.reset();
    lq_dropped_.reset();
    hash_seed_.reset();
    hash_.reset();

    lf_ = lf;
}

void LfRegs::bft_cltr_write_cb()
{
  if (lf_) {
    lf_->clear();
  }
  bft_ctrl_.clear(0);
}

}

