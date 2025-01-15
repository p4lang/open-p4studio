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


#ifndef _TOFINO_PDFIXED_PD_PORT_MGR_H
#define _TOFINO_PDFIXED_PD_PORT_MGR_H

#include <tofino/pdfixed/pd_common.h>
#include <port_mgr/bf_port_if.h>

/**
 * @brief Sets the maximum TX and RX MTU
 *
 * @param[in] dev_id Device identifier (0..BF_MAX_DEV_COUNT-1)
 * @param[in] port_id Port id
 * @param[in] tx_mtu  Maximum TX MTU
 * @param[in] rx_mtu  Maximum RX MTU
 *
 * @return BF_SUCCESS if success
 */
p4_pd_status_t p4_port_mgr_mtu_set(const bf_dev_id_t dev_id,
                                   const int port_id,
                                   const int tx_mtu,
                                   const int rx_mtu);

#endif
