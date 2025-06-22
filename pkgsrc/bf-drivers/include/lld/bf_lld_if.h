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


#ifndef BF_LLD_IF_H_INCLUDED
#define BF_LLD_IF_H_INCLUDED

/* Allow the use in C++ code.  */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file bf_lld_if.h
 * \brief Details Device-level APIs.
 *
 */

/**
 * @addtogroup lld-api
 * @{
 * This is a description of some APIs.
 */

typedef bf_status_t (*bf_reg_wr_fn)(bf_dev_id_t dev_id,
                                    bf_subdev_id_t subdev_id,
                                    uint32_t addr,
                                    uint32_t data);
typedef bf_status_t (*bf_reg_rd_fn)(bf_dev_id_t dev_id,
                                    bf_subdev_id_t subdev_id,
                                    uint32_t addr,
                                    uint32_t *data);

bf_status_t bf_lld_bind_wr_fn(bf_reg_wr_fn fn);
bf_status_t bf_lld_bind_rd_fn(bf_reg_rd_fn fn);
bf_status_t bf_lld_init(bool is_master, bf_reg_wr_fn wr_fn, bf_reg_rd_fn rd_fn);

// legacy APIs
bf_status_t bf_bind_wr_fn(bf_reg_wr_fn fn);
bf_status_t bf_bind_rd_fn(bf_reg_rd_fn fn);
char *bf_lld_dbg_get_full_reg_path_name(bf_dev_family_t dev_family,
                                        uint32_t offset);
char *bf_dbg_get_full_reg_path_name(uint32_t offset);
bool bf_lld_dev_is_tof1(bf_dev_id_t dev_id);
bool bf_lld_dev_is_tof2(bf_dev_id_t dev_id);
bool bf_lld_dev_is_tof3(bf_dev_id_t dev_id);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif /* C++ */

#endif  // BF_LLD_IF_H_INCLUDED
