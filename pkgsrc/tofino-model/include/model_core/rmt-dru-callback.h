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


#ifndef _MODEL_CORE_DRU_CALLBACK__
#define _MODEL_CORE_DRU_CALLBACK__

// Types for low level driver callbacks
typedef void (*DruDiagEvent)( int asic, uint8_t *diag_data, int len );
typedef void (*DruIdleUpdate)( int asic, uint8_t *idle_timeout_data, int len );
typedef void (*DruLrtUpdate)( int asic, uint8_t *lrt_stat_data, int len );
typedef void (*DruRxPkt)( int asic, uint8_t *pkt, int len, int cos );
typedef void (*DruLearn)( int asic, uint8_t *learn_filter_data, int len, int pipe_nbr );

#endif // _MODEL_CORE_DRU_CALLBACK__
