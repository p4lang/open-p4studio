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

#include <deparser-ingress.h>

#include <inttypes.h>
#include <deparser-reg.h>
#include <packet.h>
#include <phv.h>
#include <port.h>

namespace tofino {

Packet *
DeparserIngress::HandleResubmit(Packet* pkt, const Phv &phv, uint8_t drop_ctl,
                                Packet **resubmit_pkt) {
  uint8_t phv_idx, shift;
  bool valid = false;

  deparser_reg_.get_resubmit_cfg(&phv_idx, &valid, &shift);
  if (valid && phv.is_valid_d(phv_idx) && ((drop_ctl & 0x01) == 0) && ! phv.ingress_pkterr() ) {
    uint8_t resubmit_table_idx = ((phv.get_d(phv_idx) & 0xFF) >> shift) & 0x07;
    std::vector<uint8_t> phv_idx_list;
    bool table_entry_valid;
    deparser_reg_.get_resubmit_table_entry(resubmit_table_idx, &table_entry_valid,
                                           &phv_idx_list);
    if (table_entry_valid) {
      (*resubmit_pkt) = pkt;
      (*resubmit_pkt)->mark_for_resubmit();
      // Do not add resubmit header to the packet. Just stash it somewhere
      // so ingress buffer module can add to the packet correctly.
      (*resubmit_pkt)->set_resubmit_header(GetMetadataHeader(phv,
                                                             phv_idx_list));
      pkt = nullptr;
    }
  }
  return pkt;
}


}
