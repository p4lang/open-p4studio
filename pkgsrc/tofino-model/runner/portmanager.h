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

#ifndef _PORTMANAGER_H_
#define _PORTMANAGER_H_

/* Common port manager header file. */

#include <common/common_types.h>
#include <stdbool.h>
#include <stdint.h>

typedef void (*bfm_packet_handler_vector_f)(uint32_t port_num,
                                            uint8_t *buffer,
                                            int length,
                                            int orig_length);

extern void
bfm_packet_handler_vector_set(bfm_packet_handler_vector_f fn);

extern bfm_error_t bfm_port_init(int port_count);
extern bfm_error_t bfm_port_start_pkt_processing(void);
extern bfm_error_t bfm_port_finish(void);

extern bfm_error_t
bfm_port_interface_add(const char *ifname, uint32_t port_num,
                       const char *sw_name, int dump_pcap);
extern bfm_error_t
bfm_port_interface_remove(const char *ifname);

extern bfm_error_t bfm_port_packet_emit(uint32_t port_num,
                                        uint16_t queue_id,
                                        uint8_t *data, int len);

extern void bfm_set_pcap_outdir(const char *outdir_name);

extern int bfm_get_port_count(void);

bool bfm_is_if_up(int port);

#endif /* _PORTMANAGER_H_ */
