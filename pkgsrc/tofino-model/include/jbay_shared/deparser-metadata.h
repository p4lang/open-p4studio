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

#ifndef _JBAY_SHARED_DEPARSER_METADATA_
#define _JBAY_SHARED_DEPARSER_METADATA_

#include <bitvector.h>
#include <rmt-defs.h>

namespace MODEL_CHIP_NAMESPACE {

// this is only in this file to remain compatible with shared Tofino code
struct LearnQuantumType {
  LearnQuantumType() {};
  ~LearnQuantumType() {};
  int length=0;
  bool valid=false;
  BitVector<RmtDefs::kLearnQuantumWidth> data{};
};

}

#endif // _JBAY_SHARED_DEPARSER_METADATA_
