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
#include <deparser-block.h>

namespace MODEL_CHIP_TEST_NAMESPACE {

using namespace MODEL_CHIP_NAMESPACE;

void TestUtil::set_multicast_pipe_vector(int pipe_index,
                                         int table_num,
                                         int mgid,
                                         uint16_t val) {
  auto *deparser_reg = get_objmgr()->deparser_get(pipe_index)->deparser_reg();
  deparser_reg->set_multicast_pipe_vector(table_num, mgid, val);
}

void TestUtil::set_dprsr_csum_invert(int pipe, int csum_eng, int bit) {
  assert(RmtObject::is_jbayB0());
  assert((pipe >= 0) && (pipe < 4));
  assert((csum_eng >= 0) && (csum_eng < 8));
  assert((bit >= 0) && (bit < 24));
  auto& a_icr = RegisterUtils::addr_dprsr(pipe)->inp.icr;
  auto& a_ipp = RegisterUtils::addr_dprsr(pipe)->inp.ipp;
  uint32_t v_icr  = InWord((void*)&a_icr.scratch);
  uint32_t v_icr2 = InWord((void*)&a_icr.scratch2);
  uint32_t v_ipp  = InWord((void*)&a_ipp.scratch);
  uint32_t v_invert = 0u;
  switch (csum_eng) {
    case 0: v_invert = v_icr & 0xFFFFFFu; break;
    case 1: v_invert = ((v_icr2 & 0xFFFFu) << 8) | (v_icr >> 24); break;
    case 2: v_invert = ((v_ipp & 0xFFu) << 16) | (v_icr2 >> 16); break;
    case 3: v_invert = v_ipp >> 8; break;
  }
  v_invert |= (1u << bit); // Set clot/phv csum to be inverted
  switch (csum_eng) {
    case 0:
      v_icr =  (v_icr  & 0xFF000000u) |  (v_invert & 0x00FFFFFFu);
      break;
    case 1:
      v_icr  = (v_icr  & 0x00FFFFFFu) | ((v_invert & 0x000000FFu) << 24);
      v_icr2 = (v_icr2 & 0xFFFF0000u) | ((v_invert & 0x00FFFF00u) >>  8);
      break;
    case 2:
      v_icr2 = (v_icr2 & 0x0000FFFFu) | ((v_invert & 0x0000FFFFu) << 16);
      v_ipp =  (v_ipp  & 0xFFFFFF00u) | ((v_invert & 0x00FF0000u) >> 16);
      break;
    case 3:
      v_ipp =  (v_ipp  & 0x000000FFu) | ((v_invert & 0x00FFFFFFu) <<  8);
      break;
  }
  OutWord((void*)&a_icr.scratch,  v_icr);
  OutWord((void*)&a_icr.scratch2, v_icr2);
  OutWord((void*)&a_ipp.scratch,  v_ipp);
}

}
