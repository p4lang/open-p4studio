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

#include <mau.h>
// TODO:JBAY: Just a copy of Tofino code at moment

#include <rmt-object-manager.h>
#include <parser-block.h>
#include <deparser-block.h>
#include <chip.h>
#include <rmt-log.h>
#include <register_includes/reg.h>
#include <register_utils.h>

namespace MODEL_CHIP_NAMESPACE {

RmtLogger* Chip::GetLoggerObject(uint32_t address) {
  // TODO: impelement parser etc for jbay

  RmtObjectManager *om = LookupObjectManager();
  if (om == NULL) return default_logger_.get();

  RmtLogger* logger_obj = nullptr;
  if (RegisterUtils::is_pipe_address(address)) {
    int mau = RegisterUtils::mau_index_for_address(address);
    if (RegisterUtils::is_mau_index(mau)) {
        logger_obj = om->mau_lookup(RegisterUtils::pipe_index_for_address(address), mau);
    } else {
  //     uint32_t i_prsr_base = (pipe * jbay_reg_pipes_pmarb_ibp18_reg_ibp_reg_array_element_size) +
  //         jbay_reg_pipes_pmarb_ibp18_reg_ibp_reg_address;
  //     uint i_prsr = ( address - i_prsr_base ) /
  //         jbay_reg_pipes_pmarb_ibp18_reg_ibp_reg_array_element_size;
  //     if ( i_prsr < jbay_reg_pipes_pmarb_ibp18_reg_ibp_reg_array_count ) {
  //       return om->parser_lookup(pipe,i_prsr);
  //     }
  //     else {
  //       uint32_t e_prsr_base = (pipe * jbay_reg_pipes_pmarb_ebp18_reg_ebp_reg_array_element_size) +
  //           jbay_reg_pipes_pmarb_ebp18_reg_ebp_reg_address;
  //       uint e_prsr = ( address - e_prsr_base ) /
  //           jbay_reg_pipes_pmarb_ebp18_reg_ebp_reg_array_element_size;
  //       if ( e_prsr < jbay_reg_pipes_pmarb_ebp18_reg_ebp_reg_array_count ) {
  //         return om->parser_lookup(pipe,e_prsr);
  //       }
  //       else {
  //         // only one deparser block for both deparsers
  //         return om->deparser_lookup(pipe);
  //       }
  //     }
    }
  }
  // if we can't work out the pipe/state etc, then the logging setting
  //  comes from the rmt object manager
  return logger_obj ? logger_obj : default_logger_.get();
}

}
