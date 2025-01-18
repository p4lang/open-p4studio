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

#ifndef _JBAYXX_DEPARSER_PERF_COUNT_ADAPTER_H_
#define _JBAYXX_DEPARSER_PERF_COUNT_ADAPTER_H_

#include <register_includes/dprsr_perf_count_r_mutable.h>
#include <register_includes/dprsr_perf_byt_count_r_mutable.h>
#include <deparser-perf-count-adapter-shared.h>

namespace MODEL_CHIP_NAMESPACE {

class DeparserPerfCountTimeAdapter : public DeparserPerfTimeAdapter<
    register_classes::DprsrPerfCountRMutable> {
  using DeparserPerfTimeAdapter<register_classes::DprsrPerfCountRMutable>::DeparserPerfTimeAdapter;
};

class DeparserPerfBytCountTimeAdapter : public DeparserPerfTimeAdapter<
    register_classes::DprsrPerfBytCountRMutable> {
  using DeparserPerfTimeAdapter<register_classes::DprsrPerfBytCountRMutable>::DeparserPerfTimeAdapter;
};

}

#endif //_JBAYXX_DEPARSER_PERF_COUNT_ADAPTER_H_
