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

#ifndef _OS_PRIVS_H_
#define _OS_PRIVS_H_

int os_privs_init(void);
int os_privs_reset(void);
int os_privs_has_cap_net_raw(void);
int os_privs_drop_permanently(void);
int os_privs_for_veth_attach(void);
int os_privs_for_file_access(void);
int os_privs_dropped(void);


#endif
