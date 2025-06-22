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


#ifndef BF_DMA_IF_H_INCLUDED
#define BF_DMA_IF_H_INCLUDED

/* Allow the use in C++ code.  */
#ifdef __cplusplus
extern "C" {
#endif

#include <bf_types/bf_types.h>
#include <dvm/bf_dma_types.h> /* for bf_dma_type_t */
#include <lld/bf_dma_dr_id.h>

/**
 * @file bf_dma_if.h
 * \brief Details DMA APIs.
 *
 */

typedef enum {
#define LLD_DR_ID(x) lld_dr_##x
  BF_DMA_DR_IDS,
  BF_DMA_MAX_DR,
  BF_DMA_MAX_TOF_DR = lld_dr_cmp_tx_pkt_3,
  BF_DMA_MAX_TOF2_DR = lld_dr_cmp_que_read_block_1,
  BF_DMA_MAX_TOF3_DR = lld_dr_cmp_que_read_block_1,

  BF_DMA_NO_DR = 0xff,
#undef LLD_DR_ID
} bf_dma_dr_id_t;

typedef enum {
  BF_DMA_DR_MODE_PUSHED_PTR = 0,
  BF_DMA_DR_MODE_DEV_REG,
} bf_dma_ptr_mode_t;

#define BF_DMA_MAX_DR_LEN (1024 * 1024)

bf_status_t bf_dma_start(bf_dev_id_t dev_id,
                         bf_subdev_id_t subdev_id,
                         bf_dma_dr_id_t dr);
bf_status_t bf_dma_service(bf_dev_id_t dev_id,
                           bf_subdev_id_t subdev_id,
                           bf_dma_dr_id_t dr_id,
                           int max_work,
                           int *work_done);

bf_status_t bf_dma_service_mac_stats(bf_dev_id_t devId, int max_work);
bf_status_t bf_dma_service_mac_write_block_completion(bf_dev_id_t devId,
                                                      bf_subdev_id_t subdev_id,
                                                      int max_work);
bf_status_t bf_dma_service_diag(bf_dev_id_t devId,
                                bf_subdev_id_t subdev_id,
                                int max_work);

bf_status_t bf_dma_dr_ptr_mode_set(bf_dev_id_t dev_id,
                                   bf_subdev_id_t subdev_id,
                                   bf_dma_dr_id_t dr_id,
                                   bf_dma_ptr_mode_t mode);

bf_status_t bf_dma_dr_get_max_depth(bf_dev_id_t devId,
                                    bf_dma_type_t type,
                                    unsigned int *tx_depth,
                                    unsigned int *rx_depth);

int bf_dma_dr_get_mem_requirement(bf_dev_family_t dev_family,
                                  bf_dma_type_t type,
                                  int tx_depth,
                                  int rx_depth);
#ifdef __cplusplus
}
#endif /* C++ */

#endif  // BF_DMA_IF_H_INCLUDED
