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


#ifndef LLD_INTERRUPT_IF_INCLUDED
#define LLD_INTERRUPT_IF_INCLUDED
#include <lld/lld_interrupt.h>

void lld_int_gbl_en_set(bf_dev_id_t dev_id, bf_subdev_id_t subdev_id, bool en);

bf_status_t lld_int_claim(bf_dev_id_t dev_id,
                          bf_subdev_id_t subdev_id,
                          bf_int_nbr_t int_nbr);
void lld_int_svc(bf_dev_id_t dev_id,
                 bf_subdev_id_t subdev_id,
                 uint32_t sh_int_val,
                 uint16_t sh_int_reg);
bool lld_int_nbr_claimed(bf_dev_id_t dev_id,
                         bf_subdev_id_t subdev_id,
                         bf_int_nbr_t int_nbr);
bf_status_t lld_int_msk(bf_dev_id_t dev_id,
                        bf_subdev_id_t subdev_id,
                        bf_int_nbr_t int_nbr);
bf_status_t lld_int_ena(bf_dev_id_t dev_id,
                        bf_subdev_id_t subdev_id,
                        bf_int_nbr_t int_nbr);
int lld_int_register_cb(bf_dev_id_t dev_id,
                        bf_subdev_id_t subdev_id,
                        uint32_t offset,
                        lld_int_cb cb_fn,
                        void *userdata);
uint32_t lld_default_handler(bf_dev_id_t dev_id,
                             bf_subdev_id_t subdev_id,
                             uint32_t status_reg,
                             uint32_t status_value,
                             uint32_t enable_hi_reg,
                             uint32_t enable_lo_reg,
                             void *userdata);
bf_status_t lld_register_default_handler_all_ints_cb(bf_dev_id_t dev_id,
                                                     bf_subdev_id_t subdev_id,
                                                     void *blk_lvl_int);
void lld_register_default_handler_for_all_ints(bf_dev_id_t dev_id,
                                               bf_subdev_id_t subdev_id);
lld_int_cb lld_get_int_cb(bf_dev_id_t dev_id,
                          bf_subdev_id_t subdev_id,
                          uint32_t offset,
                          void **userdata);
bf_status_t lld_inject_all_ints(bf_dev_id_t dev_id, bf_subdev_id_t subdev_id);
bf_status_t lld_inject_reg_with_offset(bf_dev_id_t dev_id,
                                       bf_subdev_id_t subdev_id,
                                       uint32_t offset);
int lld_register_mac_int_poll_cb(lld_mac_int_poll_cb fn);
int lld_register_mac_int_dump_cb(lld_mac_int_dump_cb fn);
int lld_register_mac_int_bh_wakeup_cb(lld_mac_int_bh_wakeup_cb fn);
bf_status_t lld_int_poll(bf_dev_id_t dev_id,
                         bf_subdev_id_t subdev_id,
                         bool all_ints);
uint32_t lld_int_get_glb_status(bf_dev_id_t dev_id, bf_subdev_id_t subdev_id);
uint32_t lld_int_get_shadow_int_status(bf_dev_id_t dev_id,
                                       bf_subdev_id_t subdev_id,
                                       uint16_t sh_int_reg);
uint32_t lld_int_get_shadow_msk_status(bf_dev_id_t dev_id,
                                       bf_subdev_id_t subdev_id,
                                       uint16_t sh_msk_reg);
void lld_int_set_shadow_msk_status(bf_dev_id_t dev_id,
                                   bf_subdev_id_t subdev_id,
                                   uint16_t sh_msk_reg,
                                   uint32_t value);
bf_status_t lld_clear_all_ints(bf_dev_id_t dev_id, bf_subdev_id_t subdev_id);
bf_status_t lld_clear_ints(bf_dev_id_t dev_id, bf_subdev_id_t subdev_id);
bf_status_t lld_enable_all_ints(bf_dev_id_t dev_id, bf_subdev_id_t subdev_id);
uint32_t lld_map_int_nbr_to_sh_int_reg(bf_int_nbr_t int_nbr);
uint32_t lld_map_int_nbr_to_sh_int_bit(bf_int_nbr_t int_nbr);
bf_status_t lld_int_disable_all(bf_dev_id_t dev_id, bf_subdev_id_t subdev_id);

void lld_int_host_leaf_enable_set(bf_dev_id_t dev_id,
                                  bf_subdev_id_t subdev_id,
                                  bool en);
void lld_int_mbus_leaf_enable_set(bf_dev_id_t dev_id,
                                  bf_subdev_id_t subdev_id,
                                  bool en);
bf_status_t lld_int_msix_map_set(bf_dev_id_t dev_id,
                                 bf_subdev_id_t subdev_id,
                                 bf_int_nbr_t int_nbr,
                                 int msix_num);

#endif  // LLD_INTERRUPT_IF_INCUDED
