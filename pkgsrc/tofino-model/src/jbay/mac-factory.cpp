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

// MacFactory - JBay

#include <port.h>
#include <mac-factory.h>
#include <comira-umac3.h>
#include <comira-umac4.h>

namespace MODEL_CHIP_NAMESPACE {

MacFactory::MacFactory(RmtObjectManager *om)
    : MacFactoryCommon(om) {
}
MacFactory::~MacFactory() {
}

bool MacFactory::get_mac_info(int port_index, int sku, int *mac_index, int *mac_chan) {
  RMT_ASSERT((sku >= RmtDefs::kSkuMin) && (sku <= RmtDefs::kSkuMax));
  RMT_ASSERT((mac_index != nullptr) && (mac_chan != nullptr));
  int  N = RmtDefs::kPhysMacs; // Per-pipe
  int  pipe = Port::get_pipe_num(port_index);
  int  per_pipe_mac_unmapped = Port::get_mac_num(port_index);
  int  chan = Port::get_mac_chan(port_index);
  int  mac = -1;

  RMT_ASSERT(per_pipe_mac_unmapped < 72);
  if (per_pipe_mac_unmapped == 0) {
    // Ports 2-5 used for EthCPU - Umac3
    if ((pipe == 0) && (chan >= 2) && (chan <= 5)) {
      mac = 0; chan -= 2; // Umac3 EthCPU chans 0-3 == Ports 2-5
    }
  } else {
    int per_pipe_mac = per_pipe_mac_unmapped - 1;
    mac = ((pipe * N) + per_pipe_mac) + 1; // So in 1-32
  }
  *mac_index = mac; *mac_chan = chan;
  return (mac >= 0);
}

Mac *MacFactory::mac_create(int mac_index) {
  if (mac_index < 0) return nullptr;
  // On JBay some macs are ComiraUmac3, most are ComiraUmac4
  if (mac_index == 0) {
    return new VmacC3(chip_index(), mac_index,
                      vmac_c3_mac_base_addr(mac_index));

  } else if ((mac_index >= 1) && (mac_index <= 32)) {
    // XXX: Index we pass to vmac_c4_mac_base_addr
    // must be ComiraUmac4 index *not* absolute mac index
    // (we need to subtract 1 to get index in [0..31])
    return new VmacC4(chip_index(), mac_index,
                      vmac_c4_mac_base_addr(mac_index-1));
  } else {
    return nullptr;
  }
}
void MacFactory::mac_delete(Mac *mac) {
  RMT_ASSERT(mac != nullptr);
  delete mac;
}


}
