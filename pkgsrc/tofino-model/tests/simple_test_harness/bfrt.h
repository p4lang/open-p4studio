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

#ifndef _bfrt_h_
#define _bfrt_h_

#include "json.h"

int LoadConf(const char *fname);
inline int LoadConf(std::string fname) { return LoadConf(fname.c_str()); }
int ScanPipelines(json::vector *pipelines);
int ScanPrograms(json::vector *programs);
int ScanDevices(json::vector *devices);

#endif /* _bfrt_h_ */
