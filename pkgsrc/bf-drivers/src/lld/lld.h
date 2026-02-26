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


#ifndef lld_h_included
#define lld_h_included

#include <bf_types/bf_types.h>
#include <dvm/bf_dma_types.h>

#include <lld/bf_dma_if.h>
#include <lld/lld_err.h>
#include "lld_dr.h"
#include <lld/bf_dev_if.h>
#include <lld/lld_int_cb.h>
#include "lld_efuse.h"

/* Allow the use in C++ code.  */
#ifdef __cplusplus
extern "C" {
#endif

#define LLD_MAX_I2C_MASTERS (1)
#define LLD_MAX_GPIO_PINS (1)
#define LLD_MAX_MDIO_MASTERS (1)
#define LLD_TOFINO_NUM_IRQ 512
#define LLD_TOF2_MSIX_MAX 32

#define LLD_MAX_DMA_SZ (32768)
#define LLD_MAX_DMA_SZ_SF (4096)

#define LLD_TOF2_ROTATED_PHY_CPU_MAC_ID (39)
#define LLD_TOF3_ROTATED_PHY_CPU_MAC_ID (72)  // no rotated die in TF3
                                              /** \typedef lld_dev_t:
                                               *
                                               */
typedef struct lld_int_subset_t {
  uint32_t global_mask;
  uint32_t shadow_mask[16];
} lld_int_subset_t;

/** \typedef lld_dev_t:
 *
 */
typedef struct lld_dev_t {
  bf_dev_family_t dev_family;  // tofino, ..
  bf_dev_type_t dev_type;      // tofino full, tofino small, ...
  int assigned;                // 1=in-use, 0=available for assignment
  int ready;  // 1=chip ready to use, 0=lld_chip_add not finished
  bf_dev_id_t dev_id;
  bf_subdev_id_t subdev_id;
  uint16_t num_subdev;  // populated for first subdev only
  uint16_t subdev_msk;  // populated for first subdev only

  bf_dev_pipe_t pipe_log2phy[BF_PIPE_COUNT];
  int irq_to_msix[LLD_TOFINO_NUM_IRQ];  // map each irq to an MSIX (tofino-2)

  lld_dr_view_t dr_view[BF_DMA_MAX_DR];  // DR views

  lld_int_subset_t module_int;    // subset of interrupt hierarchy controlled by
                                  // this lld instance
  lld_efuse_data_t efuse_data;    // parsed efuse data
  struct bf_dma_info_s dma_info;  // from device add
} lld_dev_t;

/** \typedef lld_context_t:
 *
 */
typedef struct lld_context_t {
  lld_dev_t asic[BF_MAX_DEV_COUNT][BF_MAX_SUBDEV_COUNT];

  bool is_master;      // true=LLD instance is "master"
  bf_reg_wr_fn wr_fn;  // register write fn
  bf_reg_rd_fn rd_fn;  // register read fn
  lld_mac_int_poll_cb mac_int_poll_cb;
  lld_mac_int_dump_cb mac_int_dump_cb;
  lld_mac_int_bh_wakeup_cb mac_int_bh_wakeup_cb;
} lld_context_t;

extern lld_context_t *lld_ctx;
void lld_init(bool is_master, bf_reg_wr_fn wr_fn, bf_reg_rd_fn rd_fn);
void lld_set_wr_fn(bf_reg_wr_fn fn);
void lld_set_rd_fn(bf_reg_rd_fn fn);
void lld_set_is_master(bool is_master);
bool lld_is_master(void);
void lld_print_dr_stats(void);
void lld_print_dr_contents(bf_dev_id_t dev_id,
                           bf_subdev_id_t subdev_id,
                           bf_dma_dr_id_t dr_id);
bool lld_is_pipe_addr(bf_dev_id_t dev_id, uint32_t addr);
int lld_get_pipe_from_addr(bf_dev_id_t dev_id, uint32_t addr);
int lld_get_stage_from_addr(bf_dev_id_t dev_id, uint32_t addr);
uint32_t lld_subdev_efuse_get_serdes_dis_odd(bf_dev_id_t dev_id,
                                             bf_subdev_id_t subdev_id);
uint32_t lld_subdev_efuse_get_serdes_dis_even(bf_dev_id_t dev_id,
                                              bf_subdev_id_t subdev_id);
#ifndef __KERNEL__
ucli_status_t bf_drv_show_tech_ucli_lld__(ucli_context_t *uc);
#endif
#ifdef __cplusplus
}
#endif /* C++ */

#endif
