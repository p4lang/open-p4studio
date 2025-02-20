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

#ifndef _SHARED_MAC_FACTORY_COMMON_
#define _SHARED_MAC_FACTORY_COMMON_

#include <mac.h>

namespace MODEL_CHIP_NAMESPACE {

class MacFactoryCommon : public RmtObject {
 public:

  MacFactoryCommon(RmtObjectManager *om) : RmtObject(om) { }
  virtual ~MacFactoryCommon() { }

  // PER-CHIP implementation
  virtual bool get_mac_info(int port_index, int sku, int *mac_index, int *mac_chan) = 0;

  virtual Mac *mac_create(int mac_index) = 0;
  virtual void mac_delete(Mac *mac) = 0;
};

}

#endif // _SHARED_MAC_FACTORY_COMMON_
