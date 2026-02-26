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


#ifndef lld_dr_h
#define lld_dr_h

#include "dvm/bf_drv_intf.h"
#include "lld/lld_err.h"
#include "lld/lld_dr_if.h"
#include "lld/lld_dr_descriptors.h"

// un-comment to read head/tail ptrs directly from hw
// and not configure pushed ptr mode
//
// Note: NOT using pushed ptr mode on the harlyn model
//       introduces a race condition due to the use of
//       separate sockets for register access and DMA.
//       the head/tail ptr update may be "seen" before
//       a completions (or RX desc) DMA has been processed
//       by the local DMA server thread.
//
//       Use with caution.
//
//#define DONT_USE_PUSHED_PTR_MODE

typedef enum {
  FM = 0, /* free memory */
  RX,     /* dev to host */
  TX,     /* host to dev */
  CP,     /* completion  */
} dr_type_t;

/* bus on which DRs reside. This required for the fast-recfg
 * flush sequence as the flushing HW is per-bus, not per DR */
typedef enum {
  lld_dr_mbus,
  lld_dr_cbus,
  lld_dr_pbus,
  lld_dr_tbus,
} lld_dr_bus_t;

/* bit map of (above) lld_dr_bus_t bits. Map is constructed
 * as (1 << bus), so the only valid values are 0x0 - 0xF */
typedef uint32_t lld_dr_bus_map_t;

#define bus_in_map(bus, bus_map) ((bus_map) & (1 << (bus)))
#define mbus_in_map(m) ((m) & (1 << lld_dr_mbus))
#define cbus_in_map(m) ((m) & (1 << lld_dr_cbus))
#define pbus_in_map(m) ((m) & (1 << lld_dr_pbus))
#define tbus_in_map(m) ((m) & (1 << lld_dr_tbus))

lld_dr_bus_map_t lld_dr_construct_bus_map(bf_dev_id_t dev_id,
                                          bf_subdev_id_t subdev_id,
                                          struct bf_dma_info_s *dma_info);

typedef struct lld_dr_basic_cfg_s {
  dr_type_t type;
  int depth;
  int producer;  // 1=producer, 0=consumer
} lld_dr_basic_cfg_t;

/**
 * Structure used during DMA ring (DR) processing to reduce PCIe transactions
 * to Tofino head/tail pointer registers
 */
struct lld_dr_view_s;

typedef int (*lld_dr_push_fn)(struct lld_dr_view_s *dr, uint64_t *desc);

/** \typedef lld_dr_view_t:
 * Structure used during DMA ring (DR) processing to reduce PCIe transactions
 * to Tofino head/tail pointer registers
 */
typedef struct lld_dr_view_s {
  // cache line 1:
  uint32_t head; /* 0: cached head ptr */
  uint32_t tail; /* 4: cached tail ptr */
  uint64_t base; /* 8: base of DR descriptor area, virtual address space */
  int n_words_per_desc; /* 16: # of u64 words per descriptor */
  int n_entries;        /* 20: # of possible descriptor entries in the ring */
  uint64_t n_descs;     /* 24: # of descriptors pushed or pulled (stat) */
  uint64_t n_bytes; /* 32: # of data bytes read/written via this ring (stat) */
  bool lock_reqd; /* 40: whether or not DR requires use of a lock (app defined)
                   */
  lld_dr_push_fn push; /* 44: one of dr_push_1(), dr_push_2(), or dr_push_4() */
  bf_dev_id_t dev_id;  /* 52: back-ref to associated dev_id */
  bf_dma_dr_id_t dr_id; /* 60: back-ref too associated DR enum */

  // cache line 2:
  uint64_t volatile *dev_pushed_ptr; /* 0: virtual address of u64
                                                   written by device */
  uint64_t last_pointer_written;     /* 8: used to identify the wrap case */
  uint32_t dr_head_reg;              /* 16: offset of DR head register */
  uint32_t dr_tail_reg;              /* 20: offset of DR tail register */
  bool producer; /*24:  1=host pushes descriptors, 0=host pulls descriptors */
  bool use_pushed_ptr_mode; /* 28: 1=device pushed ptr to host mem, 0 = host
                               reads device registers */
  void *callback_fn;     /* 32: client callback function, registered via API */
  bf_sys_mutex_t mtx[2]; /* 40: locks used if lock_reqd=true */
  void *dr_buffer;       /* 120: Virtual address of the DR's DMA buffer. */
  bf_subdev_id_t subdev_id; /* 128: back-ref to associated subdev_id */
} lld_dr_view_t;

lld_err_t lld_subdev_dr_run_flush_sequence(bf_dev_id_t dev_id,
                                           bf_subdev_id_t subdev_id);
void lld_dr_init_dev(bf_dev_id_t dev_id,
                     bf_subdev_id_t subdev_id,
                     struct bf_dma_info_s *dma_info);
void dr_init(bf_dev_id_t dev_id,
             bf_dma_dr_id_t d,
             lld_dr_view_t *view,
             uint64_t desc_area_v,
             uint64_t desc_area_p);
int lld_dr_start(bf_dev_id_t dev_id,
                 bf_subdev_id_t subdev_id,
                 bf_dma_dr_id_t dr_id);
int lld_dr_service(bf_dev_id_t dev_id,
                   bf_subdev_id_t subdev_id,
                   bf_dma_dr_id_t dr_id,
                   int n);

int dr_evaluate(lld_dr_view_t *view);
int dr_full(lld_dr_view_t *dr);
int dr_space(lld_dr_view_t *dr);
int dr_used(lld_dr_view_t *dr);
int dr_depth(lld_dr_view_t *dr);

void lld_dr_enable_set(bf_dev_id_t dev_id, bf_dma_dr_id_t dr, bool en);
void lld_subdev_dr_enable_set(bf_dev_id_t dev_id,
                              bf_subdev_id_t subdev_id,
                              bf_dma_dr_id_t dr,
                              bool en);
bf_status_t lld_dr_set_write_time_mode(bf_dev_id_t dev_id,
                                       bf_subdev_id_t subdev_id,
                                       bf_dma_dr_id_t dr_id,
                                       bool en);
bf_status_t lld_dr_pushed_ptr_mode_set(bf_dev_id_t dev_id,
                                       bf_subdev_id_t subdev_id,
                                       bf_dma_dr_id_t dr_id,
                                       bool en);
void lld_dr_set_dr_ring_timeout(bf_dev_id_t dev_id,
                                bf_dma_dr_id_t dr_id,
                                uint16_t timeout);
void lld_dr_data_timeout_set(bf_dev_id_t dev_id,
                             bf_dma_dr_id_t dr_id,
                             uint32_t timeout);
void lld_subdev_dr_data_timeout_set(bf_dev_id_t dev_id,
                                    bf_subdev_id_t subdev_id,
                                    bf_dma_dr_id_t dr_id,
                                    uint32_t timeout);

void dr_update_view(lld_dr_view_t *view);
void dr_publish_view(lld_dr_view_t *view);
int lld_dr_words_per_desc(bf_dev_family_t dev_fam, int dr_e);
// int lld_dr_cfg_this(bf_dev_id_t dev_id, bf_dma_dr_id_t dr, int depth);
lld_dr_basic_cfg_t *lld_dr_basic_cfg_get(bf_dev_id_t dev_id,
                                         bf_subdev_id_t subdev_id,
                                         bf_dma_dr_id_t dr_id);
int lld_dr_cfg_depth_get(bf_dev_id_t dev_id,
                         bf_subdev_id_t subdev_id,
                         bf_dma_dr_id_t dr_id,
                         int *depth);
int lld_validate_dr_id(bf_dev_id_t dev_id, bf_dma_dr_id_t dr_id);

bf_dma_dr_id_t lld_dr_get_max_dr(bf_dev_id_t dev_id, bf_subdev_id_t subdev_id);
lld_dr_bus_t lld_dr_host_bus(bf_dev_family_t dev_family, bf_dma_dr_id_t dr_id);
int lld_dr_desc_area_sz(bf_dev_id_t dev_id, bf_subdev_id_t subdev_id);
int dr_push_1(struct lld_dr_view_s *view, uint64_t *desc);
int dr_push_2(struct lld_dr_view_s *view, uint64_t *desc);
int dr_push_4(struct lld_dr_view_s *view, uint64_t *desc);
int dr_pull(struct lld_dr_view_s *view, dr_descr_value_t *desc_val);
void lld_dr_ring_timeout_set(bf_dev_id_t dev_id,
                             bf_subdev_id_t subdev_id,
                             bf_dma_dr_id_t dr_id,
                             uint16_t timeout);
int lld_subdev_register_dr_callback(bf_dev_id_t dev_id,
                                    bf_subdev_id_t subdev_id,
                                    bf_dma_dr_id_t dr_id,
                                    lld_cb_fn_u cb);
void *lld_subdev_get_dr_callback(bf_dev_id_t dev_id,
                                 bf_subdev_id_t subdev_id,
                                 bf_dma_dr_id_t dr_id);
void dr_process_completion(lld_dr_view_t *view, dr_descr_value_t *descr_value);
void dr_process_rx_lrt(lld_dr_view_t *view, dr_descr_value_t *descr_value);
void dr_process_rx_idle(lld_dr_view_t *view, dr_descr_value_t *descr_value);
void dr_process_rx_learn(lld_dr_view_t *view, dr_descr_value_t *descr_value);
void dr_process_rx_diag(lld_dr_view_t *view, dr_descr_value_t *descr_value);
void dr_process_rx_pkt(lld_dr_view_t *view, dr_descr_value_t *descr_value);
#endif
