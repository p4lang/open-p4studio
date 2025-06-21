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

#include <linux/delay.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <bf_types/bf_types.h>
#include <target-sys/bf_sal/bf_sys_intf.h>
#include <dvm/bf_drv_intf.h>
#include <lld/bf_dma_if.h>
#include <lld/bf_dev_if.h>
#include <lld/lld_spi_if.h>
#include <lld/lld_fault.h>
#include <lld/bf_lld_if.h>

bf_status_t lld_spi_init(bf_dev_id_t dev_id, bf_subdev_id_t subdev_id) {
  (void)dev_id;
  (void)subdev_id;
  return BF_SUCCESS;
}

bf_status_t bf_drv_client_register_callbacks(
    bf_drv_client_handle_t client_handle,
    bf_drv_client_callbacks_t *callbacks,
    bf_drv_client_prio_t add_priority) {
  (void)client_handle;
  (void)callbacks;
  (void)add_priority;
  return BF_SUCCESS;
}

bf_status_t bf_drv_register(const char *client_name,
                            bf_drv_client_handle_t *client_handle) {
  (void)client_name;
  (void)client_handle;
  return BF_SUCCESS;
}

bf_status_t bf_drv_device_type_get(bf_dev_id_t dev_id, bool *is_sw_model) {
  *is_sw_model = false;
  return BF_SUCCESS;
}

int bf_sys_usleep(int micro_seconds) {
  /* use udelay for very short durations only */
  if (micro_seconds <= 10) {
    udelay(micro_seconds);
  } else {
    usleep_range(micro_seconds, micro_seconds + 1);
  }
  return 0;
}

/* IMP *** we define bf_sys_mutex using spin locks assuming that all the
 * lld code that bf_kpkt is linking to and that needs by_sys_mutex
 * run in soft_irq context.
 * care should be taken while linking more functionalities into bf_kpkt
 * to ensure that code does not sleep while holding the bf_sys_mutex ****/
int bf_sys_mutex_del(bf_sys_mutex_t *mtx) {
  if (mtx) {
    if (mtx->bf_mutex) {
      kfree(mtx->bf_mutex);
      mtx->bf_mutex = NULL;
    }
  }
  return 0;
}

int bf_sys_mutex_init(bf_sys_mutex_t *mtx) {
  spinlock_t *sl;

  if (mtx) {
    sl = kmalloc(sizeof(spinlock_t), GFP_KERNEL);
    mtx->bf_mutex = sl;
    if (!sl) {
      printk(KERN_ERR "bf_kpkt failed to maloc spinlock\n");
      return -1;
    }
    spin_lock_init(sl);
    return 0;
  } else {
    return -1;
  }
}

int bf_sys_mutex_lock(bf_sys_mutex_t *mtx) {
  spin_lock_bh((spinlock_t *)(mtx->bf_mutex));
  return 0;
}

int bf_sys_mutex_unlock(bf_sys_mutex_t *mtx) {
  spin_unlock_bh((spinlock_t *)(mtx->bf_mutex));
  return 0;
}

void lld_fault_dma_error(bf_dev_id_t dev_id, uint64_t *completion_desc) {
  (void)dev_id;
  (void)completion_desc;
}

int bf_sys_log_and_trace(int module, int bf_level, const char *format, ...) {
  (void)module;
  (void)bf_level;
  (void)format;
  return 0;
}

extern char *get_full_reg_path_name(bf_dev_id_t dev_id, uint32_t offset);

char *get_full_reg_path_name(bf_dev_id_t dev_id, uint32_t offset) {
  (void)offset;
  (void)dev_id;
  return NULL;
}

extern char *lld_reg_parse_get_full_reg_path_name(bf_dev_family_t dev_family,
                                                  uint32_t offset);

char *lld_reg_parse_get_full_reg_path_name(bf_dev_family_t dev_family,
                                           uint32_t offset) {
  (void)dev_family;
  (void)offset;
  return NULL;
}

extern void lld_debug_init(void);
void lld_debug_init(void) {}

extern void lld_log_dma(int dir,
                        bf_dev_id_t dev_id,
                        bf_subdev_id_t subdev_id,
                        uint32_t dr,
                        uint64_t *data,
                        int n_wds,
                        uint64_t head,
                        uint64_t tail);
void lld_log_dma(int dir,
                 bf_dev_id_t dev_id,
                 bf_subdev_id_t subdev_id,
                 uint32_t dr,
                 uint64_t *data,
                 int n_wds,
                 uint64_t head,
                 uint64_t tail) {
  (void)dev_id;
  (void)subdev_id;
  (void)dr;
  (void)data;
  (void)n_wds;
  (void)head;
  (void)tail;
}

void lld_debug_bus_init(bf_dev_id_t dev_id) { (void)dev_id; }

#if 0
bf_status_t lld_int_poll(bf_dev_id_t dev_id, bool all_ints)
{
  (void)dev_id;
  (void)all_ints;
  return BF_SUCCESS;
}

bf_status_t lld_disable_all_ints(bf_dev_id_t input_dev)
{
  (void)input_dev;
  return BF_SUCCESS;
}

void lld_register_default_handler_for_all_ints(void)
{
}

bf_status_t lld_int_ena(bf_dev_id_t dev_id, bf_subdev_id_t subdev_id, bf_int_nbr_t int_nbr)
{
  (void)dev_id;
  (void)int_nbr;
  return BF_SUCCESS;
}

bf_status_t lld_int_claim(bf_dev_id_t dev_id, bf_subdev_id_t subdev_id, bf_int_nbr_t int_nbr) {
  (void)dev_id;
  (void)int_nbr;
  return BF_SUCCESS;
}
#endif
