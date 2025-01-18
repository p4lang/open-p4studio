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

#ifndef _JBAYXX_MAC_FACTORY_
#define _JBAYXX_MAC_FACTORY_

#include <mac-factory-common.h>
#include <register_utils.h>

namespace MODEL_CHIP_NAMESPACE {

class MacFactory : public MacFactoryCommon {
 public:
  // Both ComiraUmac3 and ComiraUmac4 on JBay
  static constexpr uint32_t vmac_c3_base_addr = RegisterUtils::vmac_c3_base_addr;
  static constexpr uint32_t vmac_c3_stride    = RegisterUtils::vmac_c3_stride;
  static constexpr uint32_t vmac_c4_base_addr = RegisterUtils::vmac_c4_base_addr;
  static constexpr uint32_t vmac_c4_stride    = RegisterUtils::vmac_c4_stride;

  static uint32_t vmac_c3_mac_base_addr(int mac_block) {
    return vmac_c3_base_addr + (vmac_c3_stride * mac_block);
  }
  static uint32_t vmac_c4_mac_base_addr(int mac_block) {
    return vmac_c4_base_addr + (vmac_c4_stride * mac_block);
  }

  MacFactory(RmtObjectManager *om);
  virtual ~MacFactory();

  // We calculate mac_index/chan from port/sku...
  bool get_mac_info(int port_index, int sku, int *mac_index, int *mac_chan);

  // ...then we can create/delete mac
  Mac *mac_create(int mac_index);
  void mac_delete(Mac *mac);
};

}

#endif // _JBAYXX_MAC_FACTORY_
