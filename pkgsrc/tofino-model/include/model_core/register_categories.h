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

#ifndef _MODEL_CORE_REGISTER_CATEGORIES_H_
#define _MODEL_CORE_REGISTER_CATEGORIES_H_

#define REG_LO                 0x010000
#define REG_DEVSEL             0x020000
#define REG_DEVSEL_SOFT_RESET  0x020001
#define REG_DEVSEL_FUSE        0x020002
#define REG_SERDES             0x030000
#define REG_MACS               0x040000
#define REG_MACS_CNTRS         0x040001
#define REG_MACS_OTHER         0x040002
#define REG_ETHS               0x050000
#define REG_PIPES              0x070000
#define REG_HI                 0x990000
#define REG_MASK               0xFF0000

#endif // _MODEL_CORE_REGISTER_CATEGORIES_H_
