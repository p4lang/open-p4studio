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

#ifndef _BMI_INTERFACE_
#define _BMI_INTERFACE_

typedef struct bmi_interface_s bmi_interface_t;

int bmi_interface_create(bmi_interface_t **bmi, const char *device);

int bmi_interface_add_dumper(bmi_interface_t *bmi, const char *filename);

int bmi_interface_destroy(bmi_interface_t *bmi);

int bmi_interface_send(bmi_interface_t *bmi, const char *data, int len);

int bmi_interface_recv(bmi_interface_t *bmi, const char **data, int *n_drops_since_last_rx);

int bmi_interface_recv_with_copy(bmi_interface_t *bmi, char *data, int max_len);

int bmi_interface_get_fd(bmi_interface_t *bmi);

const struct timeval *bmi_interface_get_select_timeout(bmi_interface_t *bmi);

int bmi_interface_get_stats(bmi_interface_t *bmi, int *recv, int *drop, int *ifdrop);

#endif