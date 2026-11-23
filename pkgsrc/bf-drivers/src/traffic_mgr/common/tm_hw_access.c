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


#include <stddef.h>
#include <sched.h>
#include <inttypes.h>
#include <unistd.h>

#include <traffic_mgr/traffic_mgr.h>
#include "tm_error.h"
#include <dvm/bf_drv_intf.h>
#include "tm_ctx.h"
#include "traffic_mgr/hw_intf/tm_tofino_hw_intf.h"
#include "tm_hw_access.h"

#include <tofino_regs/tofino.h>
#include <target-sys/bf_sal/bf_sys_intf.h>
#include <lld/bf_dma_if.h>
#include <lld/lld_err.h>
#include <lld/lld_reg_if.h>
#include <lld/lld_dr_if.h>
#include <lld/lld_subdev_dr_if.h>
#include <lld/lld_tof_addr_conversion.h>
#include <lld/lld_tof2_addr_conversion.h>
#include <lld/lld_tof3_addr_conversion.h>
#include <lld/lld_diag_ext.h>
#include <mc_mgr/mc_mgr_shared_dr.h>

/* Move following code to utils */
#define BF_TM_DMA_BUF_ALLOC_FAIL_TOLERATE (500)

typedef enum {
  BF_TM_DESCR_UNUSED =
      1, /* Descriptor free to use. May or may not point to dma able buffer */
  BF_TM_DESCR_INUSE =
      2, /* buffer pointed by the descriptor is used for filling up data to dma
          */
  BF_TM_DESCR_DMAREADY =
      4, /* buffer pointed by the descriptor is full and ready for dma */
  BF_TM_DESCR_UNDERDMA = 8, /* Descriptor/Buffer handed to DMA engine */
  BF_TM_DESCR_DMADONE =
      0x10 /* buffer pointed to by descriptor is now DMA complete */
} bf_tm_descr_state_en;

typedef struct bf_tm_dma_buffer_descriptor_t bf_tm_dma_buffer_descriptor_t;
struct bf_tm_dma_buffer_descriptor_t {
  void *buf_v_addr;
  bf_phys_addr_t buf_phys_addr;
  uint64_t v_addr_compare;  // This is used to search for
                            // dma buffer using 64bit
                            // message id in the DMA callback
  uint32_t buf_size;
  uint32_t data_size;
  uint8_t wr_list_size;
  uint8_t descr_state;  // UNUSED, INUSE, DMADONE
  bf_tm_dma_buffer_descriptor_t *next;
  bf_tm_dma_buffer_descriptor_t *prev;
};

typedef struct bf_tm_dma_ctx_t bf_tm_dma_ctx_t;
struct bf_tm_dma_ctx_t {
  bf_sys_dma_pool_handle_t dma_pool_hndl;
  tm_mutex_t mutex;  // This mutex is used to synchronize access to descritor
                     // ring. Multiple threads (DMA Engine thread that does
                     // callback when DMA is complete and TM driver thread that
                     // produces data to DMA access descriptor ring.
  uint32_t inuse_buffer_count;
  uint32_t max_buffer_count;
  uint32_t buffer_size;
  uint32_t pool_size;
  bf_tm_dma_buffer_descriptor_t *front;
  bf_tm_dma_buffer_descriptor_t *end;
};

static bf_tm_dma_ctx_t g_tm_dma_ctx[BF_TM_NUM_ASIC][BF_TM_NUM_SUBDEV];

#define BF_Q_WRITELIST_ADDR_SIZE (8)  // 64bit addr
#define BF_TM_TOFINO_WR_SIZE (4)  // 32bit writes in tofino, variable in tof2

// Only call this function under a lock.
static bf_tm_dma_buffer_descriptor_t *bf_tm_get_descriptor(
    bf_dev_id_t dev, bf_subdev_id_t subdev_id, uint8_t wr_sz) {
  bf_tm_dma_buffer_descriptor_t *descr;

  // if buffer exists, size matches, buffer isn't full, and buffer isn't closed
  // for DMA, give the buffer at the end of the list
  if (g_tm_dma_ctx[dev][subdev_id].end &&
      g_tm_dma_ctx[dev][subdev_id].end->wr_list_size == wr_sz &&
      g_tm_dma_ctx[dev][subdev_id].end->data_size + wr_sz +
              BF_Q_WRITELIST_ADDR_SIZE <=
          g_tm_dma_ctx[dev][subdev_id].end->buf_size &&
      g_tm_dma_ctx[dev][subdev_id].end->descr_state == BF_TM_DESCR_INUSE) {
    descr = g_tm_dma_ctx[dev][subdev_id].end;
    descr->data_size += wr_sz + BF_Q_WRITELIST_ADDR_SIZE;
    return descr;
  }

  // if we skipped end because the buffer is full or because of a size mismatch,
  // set it to DMA_READY
  if (g_tm_dma_ctx[dev][subdev_id].end &&
      g_tm_dma_ctx[dev][subdev_id].end->descr_state == BF_TM_DESCR_INUSE)
    g_tm_dma_ctx[dev][subdev_id].end->descr_state = BF_TM_DESCR_DMAREADY;

  // if we've reached the max # buffers, return null.
  if (g_tm_dma_ctx[dev][subdev_id].inuse_buffer_count >=
      g_tm_dma_ctx[dev][subdev_id].max_buffer_count) {
    return (NULL);
  }

  // otherwise, append new buffer to end of list
  void *v_addr;
  bf_phys_addr_t p_addr;
  bf_tm_status_t rc = bf_sys_dma_alloc(
      g_tm_dma_ctx[dev][subdev_id].dma_pool_hndl, 1, &v_addr, &p_addr);
  if (BF_TM_IS_NOTOK(rc)) {
    return (NULL);
  }

  descr = bf_sys_calloc(1, sizeof(bf_tm_dma_buffer_descriptor_t));
  if (descr == NULL) {
    bf_sys_dma_free(g_tm_dma_ctx[dev][subdev_id].dma_pool_hndl, v_addr);
    LOG_ERROR("TM: Could not allocate memory descriptor for device %d", dev);
    return (NULL);
  }
  // LOG_TRACE("Thread %u TMDMA new DMA buffer %-30" PRIx64, id, v_addr);
  descr->descr_state = BF_TM_DESCR_INUSE;
  descr->next = g_tm_dma_ctx[dev][subdev_id].end;
  if (g_tm_dma_ctx[dev][subdev_id].end)
    g_tm_dma_ctx[dev][subdev_id].end->prev = descr;
  g_tm_dma_ctx[dev][subdev_id].end = descr;
  if (g_tm_dma_ctx[dev][subdev_id].front == NULL)
    g_tm_dma_ctx[dev][subdev_id].front = descr;
  g_tm_dma_ctx[dev][subdev_id].inuse_buffer_count += 1;
  descr->buf_v_addr = v_addr;
  descr->buf_phys_addr = p_addr;
  descr->buf_size = g_tm_dma_ctx[dev][subdev_id].buffer_size;
  descr->data_size = wr_sz + BF_Q_WRITELIST_ADDR_SIZE;
  descr->wr_list_size = wr_sz;
  // I am not happy with using uintptr_t here. We cannot move to 64b without
  // modifying code.
  descr->v_addr_compare = (uintptr_t)((uint8_t *)v_addr + BF_TM_DMA_MSG_ID);

  return descr;
}

static bf_tm_dma_buffer_descriptor_t *bf_tm_subdev_find_descriptor_addr(
    bf_dev_id_t dev, bf_subdev_id_t subdev_id, uint64_t v_addr_compare) {
  // Walk the descriptor list and find slot/descriptor that matches with virtual
  // address
  TM_MUTEX_LOCK(&(g_tm_dma_ctx[dev][subdev_id].mutex));
  bf_tm_dma_buffer_descriptor_t *cur = g_tm_dma_ctx[dev][subdev_id].front;
  while (cur) {
    if (cur->v_addr_compare == v_addr_compare) {
      TM_MUTEX_UNLOCK(&(g_tm_dma_ctx[dev][subdev_id].mutex));
      return (cur);
    }
    cur = cur->prev;
  }
  TM_MUTEX_UNLOCK(&(g_tm_dma_ctx[dev][subdev_id].mutex));
  return (NULL);
}

static bf_tm_dma_buffer_descriptor_t *bf_tm_find_descriptor_state(
    bf_dev_id_t dev,
    bf_subdev_id_t subdev_id,
    bf_tm_descr_state_en descr_state) {
  // Walk the descriptor list and find slot/descriptor that matches with virtual
  // address
  TM_MUTEX_LOCK(&(g_tm_dma_ctx[dev][subdev_id].mutex));
  bf_tm_dma_buffer_descriptor_t *cur = g_tm_dma_ctx[dev][subdev_id].front;
  while (cur) {
    if (cur->descr_state & descr_state) {
      TM_MUTEX_UNLOCK(&(g_tm_dma_ctx[dev][subdev_id].mutex));
      return (cur);
    }
    cur = cur->prev;
  }
  TM_MUTEX_UNLOCK(&(g_tm_dma_ctx[dev][subdev_id].mutex));
  return (NULL);
}

static void bf_tm_dma_complete_cb(bf_dev_id_t dev,
                                  bf_subdev_id_t subdev_id,
                                  bf_dma_dr_id_t dr,
                                  uint64_t data_sz_or_ts,
                                  uint32_t attr,
                                  uint32_t status,
                                  uint32_t type,
                                  uint64_t msg_id,
                                  int s,
                                  int e) {
  (void)s;
  (void)e;
  // pthread_t                      id = pthread_self();
  bf_tm_dma_buffer_descriptor_t *descr;

  if ((msg_id & 0xf) != BF_TM_DMA_MSG_ID) {
    // Not my DMA completion
    if ((msg_id & 0xf) == BF_DIAG_DMA_MSG_ID) {
      // Diag reg or mem test DMA completion
      lld_reg_mem_test_completion_cb(
          dev, subdev_id, dr, data_sz_or_ts, attr, status, type, msg_id, s, e);
    }
    return;
  }
  // msg_id >>= 4; // bottom 4 bits identify user of DR

  if (status) {
    LOG_ERROR("Dev %d/%d DMA write to 0x%" PRIx64 " failed, status=%d",
              dev,
              subdev_id,
              msg_id,
              status);
  } else {
    descr = bf_tm_subdev_find_descriptor_addr(dev, subdev_id, msg_id);
    if (descr) {
      if (descr->descr_state != BF_TM_DESCR_UNDERDMA && descr->data_size) {
        LOG_ERROR(
            "DMA Done Callback occurred on buffer "
            "0x%" PRIx64 "whose state was not under dma",
            msg_id);
      } else {
        TM_MUTEX_LOCK(&(g_tm_dma_ctx[dev][subdev_id].mutex));
        if (descr->next) descr->next->prev = descr->prev;
        if (descr->prev) descr->prev->next = descr->next;
        if (g_tm_dma_ctx[dev][subdev_id].front == descr)
          g_tm_dma_ctx[dev][subdev_id].front = descr->prev;
        if (g_tm_dma_ctx[dev][subdev_id].end == descr)
          g_tm_dma_ctx[dev][subdev_id].end = descr->next;
        if (g_tm_dma_ctx[dev][subdev_id].inuse_buffer_count)
          g_tm_dma_ctx[dev][subdev_id].inuse_buffer_count--;
        TM_MUTEX_UNLOCK(&(g_tm_dma_ctx[dev][subdev_id].mutex));
        /* we can unlock here because we should have the only reference to
           this memory/descriptor */
        /* Unmap the DMA address of the buffer so that it is reclaimed by the
           device */
        if ((bf_sys_dma_unmap(g_tm_dma_ctx[dev][subdev_id].dma_pool_hndl,
                              descr->buf_v_addr,
                              g_tm_dma_ctx[dev][subdev_id].buffer_size,
                              BF_DMA_FROM_CPU)) != 0) {
          LOG_ERROR("Unable to unmap DMA buffer %p at %s:%d",
                    descr->buf_v_addr,
                    __func__,
                    __LINE__);
        }
        if (descr->buf_v_addr) {
          bf_sys_dma_free(g_tm_dma_ctx[dev][subdev_id].dma_pool_hndl,
                          descr->buf_v_addr);
        }
        bf_sys_free(descr);
        // used for Unit Testing purposes to check DMA buffers are freed.
        // LOG_TRACE("Thread %u TMDMA Freeing DMA buffer %-30" PRIx64, id,
        // msg_id);
      }
    } else {
      LOG_TRACE(
          "DMA Done callback occurred on unknown buffer "
          "0x%" PRIx64 "which might be already cleaned up",
          msg_id);
    }
  }
  // For testing purposes only. To check if invoking complete_operations in
  // DMA done callback causes any issues.
  // bf_tm_complete_operations(dev);
  // No deadlock or any issue observed.
  (void)data_sz_or_ts;
  (void)attr;
  (void)type;
  (void)dr;
}

static bf_tm_status_t bf_tm_update_wlist(bf_dev_id_t dev,
                                         bf_subdev_id_t subdev_id,
                                         uint64_t indir_addr,
                                         uint8_t wr_sz,
                                         uint64_t hi,
                                         uint64_t lo) {
  uint8_t *v_addr;
  bf_tm_status_t rc = BF_SUCCESS;
  int alloc_fail_count = 0;
  uint32_t data_idx;

  // pthread_t         id = pthread_self();
  bf_tm_dma_buffer_descriptor_t *descr;

  if (wr_sz > 16 || ((uint32_t)wr_sz + BF_Q_WRITELIST_ADDR_SIZE >
                     g_tm_dma_ctx[dev][subdev_id].buffer_size)) {
    LOG_ERROR("Can't write %d bytes at %" PRIx64 ", DMA buffer size = %d",
              wr_sz + BF_Q_WRITELIST_ADDR_SIZE,
              indir_addr,
              g_tm_dma_ctx[dev][subdev_id].buffer_size);
    return (BF_INVALID_ARG);
  }

bf_tm_dma_alloc_buf:
  TM_MUTEX_LOCK(&(g_tm_dma_ctx[dev][subdev_id].mutex));
  descr = bf_tm_get_descriptor(dev, subdev_id, wr_sz);
  if (!descr) {
    TM_MUTEX_UNLOCK(&(g_tm_dma_ctx[dev][subdev_id].mutex));
    if (alloc_fail_count &&
        (alloc_fail_count % (BF_TM_DMA_BUF_ALLOC_FAIL_TOLERATE / 10)) == 0) {
      int dr_tx_count_sw = lld_dr_get_used_count(
          dev, subdev_id, lld_dr_tx_que_write_list, false);
      int dr_cmp_count_sw = lld_dr_get_used_count(
          dev, subdev_id, lld_dr_cmp_que_write_list, false);
      LOG_TRACE(
          "Waiting for Tx DR dev=%d/%d try:%d with tm:%d tx:%d cmp:%d buffers",
          dev,
          subdev_id,
          alloc_fail_count + 1,
          g_tm_dma_ctx[dev][subdev_id].inuse_buffer_count,
          dr_tx_count_sw,
          dr_cmp_count_sw);
    }
    sched_yield();
    lld_dr_service(dev, subdev_id, lld_dr_cmp_que_write_list, 5);
    alloc_fail_count++;
    if (alloc_fail_count < BF_TM_DMA_BUF_ALLOC_FAIL_TOLERATE) {
      usleep(200);
      goto bf_tm_dma_alloc_buf;
    } else {
      LOG_ERROR("Could not get DMA descriptor device %d/%d, buffer_in_use %d",
                dev,
                subdev_id,
                g_tm_dma_ctx[dev][subdev_id].inuse_buffer_count);
      return (BF_NO_SYS_RESOURCES);
    }
  }

  // Fillup DMA memory with 64bit address, wr_sz byte data
  v_addr = (uint8_t *)descr->buf_v_addr;
  data_idx = descr->data_size - (wr_sz + BF_Q_WRITELIST_ADDR_SIZE);
  for (int i = 0; i < BF_Q_WRITELIST_ADDR_SIZE; i++) {
    v_addr[data_idx + i] = (indir_addr >> (8 * i)) & 0xFF;
  }
  for (int i = 0; i < wr_sz; i++) {
    if (i < 8) {
      v_addr[data_idx + BF_Q_WRITELIST_ADDR_SIZE + i] = (lo >> (8 * i)) & 0xFF;
    } else {
      v_addr[data_idx + BF_Q_WRITELIST_ADDR_SIZE + i] =
          (hi >> (8 * (i - 8))) & 0xFF;
    }
  }
  TM_MUTEX_UNLOCK(&(g_tm_dma_ctx[dev][subdev_id].mutex));
  return (rc);
}

static bf_tm_status_t bf_tm_flush_buffer(
    bf_dev_id_t dev,
    bf_subdev_id_t subdev_id,
    bf_tm_dma_buffer_descriptor_t *descriptor) {
  bf_tm_status_t rc = 0;
  bf_dma_addr_t dma_addr;

  /* Map the virtual address of the buffer to the DMA address before it is
     pushed into the DR */
  if (bf_sys_dma_map(g_tm_dma_ctx[dev][subdev_id].dma_pool_hndl,
                     descriptor->buf_v_addr,
                     descriptor->buf_phys_addr,
                     g_tm_dma_ctx[dev][subdev_id].buffer_size,
                     &dma_addr,
                     BF_DMA_FROM_CPU) != 0) {
    LOG_ERROR("Unable to map DMA buffer %p at %s:%d",
              descriptor->buf_v_addr,
              __func__,
              __LINE__);
    return BF_HW_COMM_FAIL;
  }

  TM_MUTEX_LOCK(&(g_tm_dma_ctx[dev][subdev_id].mutex));
  descriptor->descr_state = BF_TM_DESCR_UNDERDMA;
  TM_MUTEX_UNLOCK(&(g_tm_dma_ctx[dev][subdev_id].mutex));

  rc = lld_subdev_push_wl(dev,
                          subdev_id,
                          0,
                          descriptor->wr_list_size,
                          descriptor->data_size / (BF_Q_WRITELIST_ADDR_SIZE +
                                                   descriptor->wr_list_size),
                          dma_addr,
                          descriptor->v_addr_compare);
  if (LLD_OK == rc) {
    // LOG_TRACE("Write List flush/push dev(%d), rc = %d", dev, rc);
    return BF_SUCCESS;
  }
  /*
   * As WL push for this DR has failed, revert the DR state so it will be
   * either freed by the caller function, or flushed again if possible.
   *
   * Note: this rollback assumes that the push was failed in a way the current
   * descriptior didn't pass through the completion callback and is not freed
   * for sure, otherwise the tm_dma_ctx should be changed to keep some pool
   * of TM DMA reusable descriptors detached from DMA handles and with
   * BF_TM_DESCR_DMADONE state set at the completion callback.
   */
  TM_MUTEX_LOCK(&(g_tm_dma_ctx[dev][subdev_id].mutex));
  descriptor->descr_state = BF_TM_DESCR_DMAREADY;
  TM_MUTEX_UNLOCK(&(g_tm_dma_ctx[dev][subdev_id].mutex));

  /* Unmap the buffer */
  if (bf_sys_dma_unmap(g_tm_dma_ctx[dev][subdev_id].dma_pool_hndl,
                       descriptor->buf_v_addr,
                       g_tm_dma_ctx[dev][subdev_id].buffer_size,
                       BF_DMA_FROM_CPU) != 0) {
    LOG_ERROR("Unable to unmap DMA buffer %p at %s:%d",
              descriptor->buf_v_addr,
              __func__,
              __LINE__);
    // Don't repeat descriptor failed unmapping.
  } else if (LLD_ERR_DR_FULL == rc) {
    /* It is possible that the completion DR is full and if so that
     * will stop the Tx DR from processing data.
     */
    return BF_EAGAIN;
  }

  LOG_ERROR(
      "Write List push fails dev=%d/%d, rc=%d, sz 0x%x cnt %d addr 0x%" PRIx64
      " id 0x%" PRIx64,
      dev,
      subdev_id,
      rc,
      descriptor->wr_list_size,
      descriptor->data_size /
          (BF_Q_WRITELIST_ADDR_SIZE + descriptor->wr_list_size),
      dma_addr,
      descriptor->v_addr_compare);

  return BF_HW_COMM_FAIL;
}

void bf_tm_flush_wlist(bf_dev_id_t dev) {
  // The TM context lock here is needed only for some callers
  // others do it recursively.
  TM_LOCK(dev, g_tm_ctx[dev]->lock);

  bf_tm_status_t rc = BF_SUCCESS;
  bf_tm_dma_buffer_descriptor_t *descriptor;

  // IF Fast reconfig is in progress do not flush DMA buffers.
  if (tm_is_device_locked(dev)) {
    TM_UNLOCK(dev, g_tm_ctx[dev]->lock);
    return;
  }

  if (g_tm_ctx[dev]->current_init_mode == BF_DEV_WARM_INIT_HITLESS) {
    // During API replay after hitless restart, do not write to device
    // until warm init done is completed. When warm-init-done callback
    // is excuted, device init mode is reset and dma flush is done.
    TM_UNLOCK(dev, g_tm_ctx[dev]->lock);
    return;
  }

  for (uint32_t i = 0; i < g_tm_ctx[dev]->subdev_count; i++) {
    if (!g_tm_dma_ctx[dev][i].inuse_buffer_count) {
      // No outstanding buffers to flush. Skip and continue
      continue;
    }

    descriptor = bf_tm_find_descriptor_state(dev, i, BF_TM_DESCR_INUSE);
    if (descriptor) {
      TM_MUTEX_LOCK(&(g_tm_dma_ctx[dev][i].mutex));
      // Force outstanding buffer (that is not completely full) to DMAable.
      descriptor->descr_state = BF_TM_DESCR_DMAREADY;
      TM_MUTEX_UNLOCK(&(g_tm_dma_ctx[dev][i].mutex));
    }

    unsigned int dr_fail_count = 0;
    unsigned int dr_count = 0;

    while (BF_SUCCESS == rc) {
      descriptor = bf_tm_find_descriptor_state(dev, i, BF_TM_DESCR_DMAREADY);
      if (descriptor) {
        rc = bf_tm_flush_buffer(dev, i, descriptor);
        if (BF_EAGAIN == rc) {
          // If the Tx DR becomes full let's try to process it.
          int dr_tx_count_sw =
              lld_dr_get_used_count(dev, i, lld_dr_tx_que_write_list, false);
          int dr_cmp_count_sw =
              lld_dr_get_used_count(dev, i, lld_dr_cmp_que_write_list, false);
          LOG_TRACE(
              "Start Tx DR dev=%d/%d try:%d with tm:%d/%d tx:%d cmp:%d buffers",
              dev,
              i,
              dr_fail_count + 1,
              dr_count,
              g_tm_dma_ctx[dev][i].inuse_buffer_count,
              dr_tx_count_sw,
              dr_cmp_count_sw);
          lld_dr_start(dev, i, lld_dr_tx_que_write_list);
          dr_count = 0;

          // And let's service few completions, if any.
          lld_dr_service(dev, i, lld_dr_cmp_que_write_list, 5);

          if (dr_fail_count < BF_TM_DMA_BUF_ALLOC_FAIL_TOLERATE) {
            dr_fail_count++;
            rc = BF_SUCCESS;
            continue;
          } else {
            LOG_TRACE("Unable to flush dev %d/%d with %d active DMA buffers.",
                      dev,
                      i,
                      g_tm_dma_ctx[dev][i].inuse_buffer_count);
            break;
          }
        } else if (BF_SUCCESS == rc) {
          dr_count++;
          dr_fail_count = 0;
        }
      } else {
        // No more dma ready descriptors.
        break;
      }
    }

    if (dr_count) {
      // Process DR once for all UNDERDMA descriptors pushed so far.
      // LOG_DBG("Dev %d start DMA DR for %d descriptors", dev, dr_count);
      lld_dr_start(dev, i, lld_dr_tx_que_write_list);
    }

    // Purge the DMA buffers that are NOT flushed
    if (BF_SUCCESS != rc) {
      unsigned int dr_purged_count = 0;
      while (1) {
        descriptor = bf_tm_find_descriptor_state(dev, i, BF_TM_DESCR_DMAREADY);
        // return DMA buffers that descriptors point to back to pool.
        // Data contained in these buffers is NOT pushed to HW.
        // DMA has failed.
        if (descriptor) {
          TM_MUTEX_LOCK(&(g_tm_dma_ctx[dev][i].mutex));
          if (rc != BF_HW_COMM_FAIL) {
            /* Unmap the DMA address of the buffer so that it is reclaimed by
             * the
             * device */
            if ((bf_sys_dma_unmap(g_tm_dma_ctx[dev][i].dma_pool_hndl,
                                  (void *)descriptor->buf_v_addr,
                                  descriptor->buf_size,
                                  BF_DMA_FROM_CPU)) != 0) {
              LOG_ERROR("Unable to unmap DMA buffer %p at %s:%d",
                        (void *)descriptor->buf_v_addr,
                        __func__,
                        __LINE__);
            }
          }
          bf_sys_dma_free(g_tm_dma_ctx[dev][i].dma_pool_hndl,
                          (void *)descriptor->buf_v_addr);
          LOG_TRACE("DMA buffer %p freed due to write list error",
                    descriptor->buf_v_addr);
          if (descriptor->next) descriptor->next->prev = descriptor->prev;
          if (descriptor->prev) descriptor->prev->next = descriptor->next;
          if (g_tm_dma_ctx[dev][i].front == descriptor)
            g_tm_dma_ctx[dev][i].front = descriptor->prev;
          if (g_tm_dma_ctx[dev][i].end == descriptor)
            g_tm_dma_ctx[dev][i].end = descriptor->next;
          if (g_tm_dma_ctx[dev][i].inuse_buffer_count)
            g_tm_dma_ctx[dev][i].inuse_buffer_count--;
          TM_MUTEX_UNLOCK(&(g_tm_dma_ctx[dev][i].mutex));
          bf_sys_free(descriptor);
          dr_purged_count++;
        } else {
          // No more dma ready descriptors.
          break;
        }
      }
      LOG_TRACE("Dev %d/%d purged %d DMA DR descriptors after error=%d",
                dev,
                i,
                dr_purged_count,
                rc);
      rc = BF_SUCCESS;  // Clean to process the next subdev.
    } else {
      // Used for unit-testing purposes to check how many buffers were built
      // before entire
      // config was pushed to HW.
      // LOG_TRACE("TMDMA %d buffers pushed to device.",
      // g_tm_dma_ctx[dev].inuse_buffer_count);
    }
  }  // for subdev
  TM_UNLOCK(dev, g_tm_ctx[dev]->lock);
}

void bf_tm_complete_ops(bf_dev_id_t dev) {
  bf_tm_flush_wlist(dev);

  for (uint32_t i = 0; i < g_tm_ctx[dev]->subdev_count; i++) {
    if (!g_tm_dma_ctx[dev][i].inuse_buffer_count) {
      // No outstanding buffers to flush.
      continue;
    }

    while (bf_tm_find_descriptor_state(dev, i, BF_TM_DESCR_UNDERDMA) &&
           mcmgr_tm_get_dr_state(dev)) {
      int serviced = lld_dr_service(dev,
                                    i,
                                    lld_dr_cmp_que_write_list,
                                    g_tm_dma_ctx[dev][i].max_buffer_count);
      if (serviced == 0 && g_tm_ctx[dev]->target == BF_TM_TARGET_MODEL) {
        // The model needs CPU time to actually start.
        // Don't busy poll to avoid starving it.
        bf_sys_usleep(100);
      }
    }
  }
}

bf_tm_status_t bf_tm_subdev_write_register(bf_dev_id_t dev,
                                           bf_subdev_id_t subdev_id,
                                           uint32_t offset,
                                           uint32_t data) {
  uint64_t indir_addr = 0;

  if (BF_TM_IS_TOF3(g_tm_ctx[dev]->asic_type)) {
    indir_addr = tof3_dir_to_indir_dev_sel(offset);
  } else if (BF_TM_IS_TOF2(g_tm_ctx[dev]->asic_type)) {
    indir_addr = tof2_dir_to_indir_dev_sel(offset);
  } else {
    indir_addr = tof_dir_to_indir_dev_sel(offset);
  }

  if ((g_tm_ctx[dev]->batch_mode) || g_tm_ctx[dev]->api_batch_mode) {
    // Either first time TM init is happening (during cold boot)
    // or TM is under fast reconfig or TM App has put TM writes
    // in batch mode (across TM APIs).
    // In all the cases, TM writes are done in the context of
    // single thread. Hence no locking done here.
    return (bf_tm_update_wlist(
        dev, subdev_id, indir_addr, BF_TM_TOFINO_WR_SIZE, 0, data));
  } else if (tm_is_device_locked(dev) &&
             !(g_tm_ctx[dev]->fast_reconfig_init_seq)) {
    // In fast reconfig mode; populate write list
    return (bf_tm_update_wlist(
        dev, subdev_id, indir_addr, BF_TM_TOFINO_WR_SIZE, 0, data));
  } else {
    return (lld_subdev_write_register(dev, subdev_id, offset, data));
  }
  return (BF_SUCCESS);
}

bf_tm_status_t bf_tm_write_register(bf_dev_id_t dev,
                                    uint32_t offset,
                                    uint32_t data) {
  return bf_tm_subdev_write_register(dev, 0, offset, data);
}

bf_tm_status_t bf_tm_subdev_write_memory(bf_dev_id_t dev,
                                         bf_subdev_id_t subdev_id,
                                         uint64_t ind_addr,
                                         uint8_t wr_sz,
                                         uint64_t hi,
                                         uint64_t lo) {
  if ((g_tm_ctx[dev]->batch_mode) || g_tm_ctx[dev]->api_batch_mode) {
    // Either first time TM init is happening (during cold boot)
    // or TM is under fast reconfig or TM App has put TM writes
    // in batch mode (across TM APIs).
    // In all the cases, TM writes are done in the context of
    // single thread. Hence no locking done here.
    return (bf_tm_update_wlist(dev, subdev_id, ind_addr >> 4, wr_sz, hi, lo));
  } else if (tm_is_device_locked(dev) &&
             !(g_tm_ctx[dev]->fast_reconfig_init_seq)) {
    // In fast reconfig mode; populate write list
    return (bf_tm_update_wlist(dev, subdev_id, ind_addr >> 4, wr_sz, hi, lo));
  } else {
    return (lld_subdev_ind_write(dev, subdev_id, ind_addr >> 4, hi, lo));
  }
  return (BF_SUCCESS);
}

bf_tm_status_t bf_tm_write_memory(bf_dev_id_t dev,
                                  uint64_t ind_addr,
                                  uint8_t wr_sz,
                                  uint64_t hi,
                                  uint64_t lo) {
  return bf_tm_subdev_write_memory(dev, 0, ind_addr, wr_sz, hi, lo);
}
bf_tm_status_t bf_tm_subdev_read_register(bf_dev_id_t dev,
                                          bf_subdev_id_t subdev_id,
                                          uint32_t offset,
                                          uint32_t *data) {
  // Inorder to avoid read back stale data, if any DMA
  // buffers are pending to be pushed to hardware, wait
  // until those DMA buffers are pushed and DMA completion
  // callback is received for the buffers.
  bf_tm_complete_operations(dev);
  return (lld_subdev_read_register(dev, subdev_id, offset, data));
}

bf_tm_status_t bf_tm_read_register(bf_dev_id_t dev,
                                   uint32_t offset,
                                   uint32_t *data) {
  return bf_tm_subdev_read_register(dev, 0, offset, data);
}
bf_tm_status_t bf_tm_subdev_read_memory(bf_dev_id_t dev,
                                        bf_subdev_id_t subdev_id,
                                        uint64_t ind_addr,
                                        uint64_t *hi,
                                        uint64_t *lo) {
  // Inorder to avoid read back stale data, if any DMA
  // buffers are pending to be pushed to hardware, wait
  // until those DMA buffers are pushed and DMA completion
  // callback is received for the buffers.
  bf_tm_complete_operations(dev);
  return lld_subdev_ind_read(dev, subdev_id, ind_addr >> 4, hi, lo);
}

bf_tm_status_t bf_tm_read_memory(bf_dev_id_t dev,
                                 uint64_t ind_addr,
                                 uint64_t *hi,
                                 uint64_t *lo) {
  return bf_tm_subdev_read_memory(dev, 0, ind_addr, hi, lo);
}

bf_tm_status_t bf_tm_setup_dma_sizes(bf_dev_id_t dev,
                                     bf_subdev_id_t subdev_id,
                                     uint32_t poolsize,
                                     uint32_t buffersize) {
  g_tm_dma_ctx[dev][subdev_id].buffer_size = buffersize;
  g_tm_dma_ctx[dev][subdev_id].pool_size = poolsize;
  g_tm_dma_ctx[dev][subdev_id].max_buffer_count = poolsize / buffersize;
  return (BF_TM_EOK);
}

bf_tm_status_t bf_tm_setup_dma(bf_dev_id_t dev,
                               bf_subdev_id_t subdev_id,
                               bf_sys_dma_pool_handle_t hdl) {
  dr_completion_callback_fn dma_cb;
  bf_tm_status_t rc = BF_SUCCESS;

  // DMA is setup only when chip is cold inited or after fast
  // reconfig. Any previous DMA buffers are released as part
  // of fast reconfig.
  g_tm_dma_ctx[dev][subdev_id].dma_pool_hndl = hdl;
  g_tm_dma_ctx[dev][subdev_id].inuse_buffer_count = 0;
  g_tm_dma_ctx[dev][subdev_id].front = NULL;
  g_tm_dma_ctx[dev][subdev_id].end = NULL;
  TM_LOCK_INIT(g_tm_dma_ctx[dev][subdev_id].mutex);

  dma_cb = (dr_completion_callback_fn)bf_tm_dma_complete_cb;

  if (g_tm_ctx[dev]->asic_type == BF_TM_ASIC_TOF3) {
    if (lld_register_completion_callback(
            dev, subdev_id, lld_dr_cmp_que_write_list, dma_cb)) {
      LOG_ERROR("DMA callback registration failed. Device %d Subdev %d",
                dev,
                subdev_id);
      rc = BF_NOT_READY;
    }
  } else {
    if (mcmgr_tm_register_completion_cb(
            dev, lld_dr_cmp_que_write_list, dma_cb, BF_TM_DMA_MSG_ID)) {
      LOG_ERROR("DMA callback registration failed. Device %d", dev);
      rc = BF_NOT_READY;
    }
  }

  return (rc);
}

void bf_tm_cleanup_wlist(bf_dev_id_t dev) {
  TM_LOCK(dev, g_tm_ctx[dev]->lock);

  bf_tm_dma_buffer_descriptor_t *descriptor;

  for (uint32_t i = 0; i < g_tm_ctx[dev]->subdev_count; i++) {
    if (!g_tm_dma_ctx[dev][i].inuse_buffer_count) {
      // No outstanding buffers to flush.
      continue;
    }

    while (1) {
      descriptor = bf_tm_find_descriptor_state(
          dev,
          i,
          (BF_TM_DESCR_INUSE | BF_TM_DESCR_UNDERDMA | BF_TM_DESCR_DMAREADY));
      // return DMA buffers that descriptors point to back to pool.
      // Data contained in these buffers is NOT pushed to HW.
      if (descriptor) {
        TM_MUTEX_LOCK(&(g_tm_dma_ctx[dev][i].mutex));
        /* Unmap the DMA address of the buffer so that it is reclaimed by the
         * device */
        if ((bf_sys_dma_unmap(g_tm_dma_ctx[dev][i].dma_pool_hndl,
                              (void *)descriptor->buf_v_addr,
                              descriptor->buf_size,
                              BF_DMA_FROM_CPU)) != 0) {
          LOG_ERROR("Unable to unmap DMA buffer %p at %s:%d",
                    (void *)descriptor->buf_v_addr,
                    __func__,
                    __LINE__);
        }
        bf_sys_dma_free(g_tm_dma_ctx[dev][i].dma_pool_hndl,
                        (void *)descriptor->buf_v_addr);
        LOG_TRACE("Dev %d/%d TMDMA buffers freed due to write list cleanup %p",
                  dev,
                  i,
                  descriptor->buf_v_addr);
        if (descriptor->next) descriptor->next->prev = descriptor->prev;
        if (descriptor->prev) descriptor->prev->next = descriptor->next;
        if (g_tm_dma_ctx[dev][i].front == descriptor)
          g_tm_dma_ctx[dev][i].front = descriptor->prev;
        if (g_tm_dma_ctx[dev][i].end == descriptor)
          g_tm_dma_ctx[dev][i].end = descriptor->next;
        if (g_tm_dma_ctx[dev][i].inuse_buffer_count)
          g_tm_dma_ctx[dev][i].inuse_buffer_count--;
        TM_MUTEX_UNLOCK(&(g_tm_dma_ctx[dev][i].mutex));
        bf_sys_free(descriptor);
      } else {
        // No more dma ready descriptors.
        break;
      }
    }
    if (bf_sys_rmutex_del(&g_tm_dma_ctx[dev][i].mutex)) {
      LOG_ERROR("%s:%d Unable to destroy TM DMA mutex for dev %d:%d",
                __func__,
                __LINE__,
                dev,
                i);
    }
  }
  TM_UNLOCK(dev, g_tm_ctx[dev]->lock);
}
