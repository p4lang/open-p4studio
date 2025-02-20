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

#ifndef _RMT_PHV_MODIFICATION_
#define _RMT_PHV_MODIFICATION_

#include <cinttypes>
#include <string>

namespace model_core {

class RmtPhvModification {
  public:
    enum class ModifyEnum {
      kMatch,
      kAction,
      kOutput,
      kErr
    };
    enum class ActionEnum {
      kXor,
      kOr,
      kClr,
      kAnd,
      kErr
    };

    // String => token mapping funcs
    static ActionEnum action_for_string(const std::string action);
    static ModifyEnum phv_for_string(const std::string stage);

};

}

#endif //_RMT_PHV_MODIFICATION_