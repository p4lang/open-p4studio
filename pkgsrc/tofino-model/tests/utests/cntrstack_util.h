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

#ifndef _CNTRSTACK_UTIL_
#define _CNTRSTACK_UTIL_

#include <utests/test_util.h>
#include <rmt-defs.h>
#include <parser.h>
#include <parser-counter-stack.h>



namespace MODEL_CHIP_TEST_NAMESPACE {

using namespace MODEL_CHIP_NAMESPACE;


class CntrstackUtil : public Parser {
 public:
  CntrstackUtil(RmtObjectManager *om, int pipe, int parser_index);
  ~CntrstackUtil();

  void   counter_reset() override;
  bool   counter_handle(int index,
                        uint8_t f8_3, uint8_t f8_2,
                        uint8_t f8_1, uint8_t f8_0, int *shift) override;
 private:
  bool  _do_printf_ = false;
};


}

#endif /*  _CNTRSTACK_UTIL_ */
