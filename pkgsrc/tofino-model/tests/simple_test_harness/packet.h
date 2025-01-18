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

#ifndef _packet_h_
#define _packet_h_

extern bool raw_mode;

void set_port_phase0(int port, uint64_t data0, uint64_t data1);
void reinit_all_ports();
void packet_output(int asic_id, int port, uint8_t *buf, int len);
int check_missing_expected();
void wait_for_idle(void);
bool set_observationLog(char *);
void update_packet_trace();

#endif /* _packet_h_ */
