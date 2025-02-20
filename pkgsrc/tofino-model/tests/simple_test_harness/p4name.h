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

#ifndef _p4name_h_
#define _p4name_h_

#include <phv.h>
#include "json.h"

int LoadP4Json(const char *fname, unsigned pipes);  // load context.json file
int LoadP4Json(std::unique_ptr<json::obj> &&json, unsigned pipes);  // load context.json
void InitP4Tables(unsigned pipes);

json::map *P4TableConfig(const char *, unsigned &pipes /* bitmask of pipes */);

#endif /* _p4name_h_ */
