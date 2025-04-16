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

#ifndef _JBAY_SHARED_PARSER_ARBITER_
#define _JBAY_SHARED_PARSER_ARBITER_

#include <parser-arbiter-common.h>
#include <register_includes/parb_phv_count_mutable.h>

namespace MODEL_CHIP_NAMESPACE {

/**
 * Concrete subclass of ParserArbiterCommon template for JBay chip-specific
* register class.
 */
class ParserArbiter
    : public ParserArbiterCommon<register_classes::ParbPhvCountMutable> {

 public:
  typedef typename register_classes::ParbPhvCountMutable counter_reg_t;

  ParserArbiter(RmtObjectManager *om, int pipeIndex);

  /**
   * Create a chip-specific phv counter register for a given generic counter
   * name.
   * @param chipIndex Chip index
   * @param pipeIndex Pipe index
   * @param generic_reg_enum Generic counter enum
   * @return An instance of register_classes::ParbPhvCountMutable
   */
  static counter_reg_t create_counter(int chipIndex,
                                      int pipeIndex,
                                      CtrEnum generic_reg_enum);
};

}

#endif // _JBAY_SHARED_PARSER_ARBITER_
