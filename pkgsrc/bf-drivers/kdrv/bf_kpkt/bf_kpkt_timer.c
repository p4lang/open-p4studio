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

#include <linux/types.h>
#include <linux/string.h>
#include <linux/device.h>
#include <linux/timer.h>
#include <linux/version.h>

#include <linux/netdevice.h>

#include <bf_types/bf_types.h>
#include <dvm/bf_drv_intf.h>
#include <dvm/bf_dma_types.h>

#include "bf_kpkt_priv.h"

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 14, 0)
#ifndef from_timer
#define from_timer(var, timer, timer_field) container_of(timer, typeof(*(var)), timer_field)
#endif
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 14, 0)
static void bf_kpkt_timer_fn(struct timer_list *timer) {
  struct bf_kpkt_adapter *adapter = from_timer(adapter, timer, timer);;
  if (!adapter) {
    printk(KERN_ERR "Error: bad parameter in bf_kpkt_timer_fn\n");
    return;
  }
  /* schedule napi even if interrupts are enabled */
  if (adapter->napi_enable) {
    napi_schedule(&adapter->napi);
  }
}
#else
static void bf_kpkt_timer_fn(unsigned long data) {
  struct bf_kpkt_adapter *adapter = (struct bf_kpkt_adapter *)data;
  if (!adapter) {
    printk(KERN_ERR "Error: bad parameter in bf_kpkt_timer_fn\n");
    return;
  }
  /* schedule napi even if interrupts are enabled */
  if (adapter->napi_enable) {
    napi_schedule(&adapter->napi);
  }
}
#endif

void bf_kpkt_timer_init(struct bf_kpkt_adapter *adapter) {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 14, 0)
  timer_setup(&adapter->timer, bf_kpkt_timer_fn, 0);
#else
  struct timer_list *timer = &adapter->timer;
  init_timer(timer);
  timer->function = bf_kpkt_timer_fn;
  timer->data = (unsigned long)adapter;
#endif
}

void bf_kpkt_timer_add(struct bf_kpkt_adapter *adapter, u32 ms) {
  /* setup a timer to run later */
  adapter->timer.expires = jiffies +  msecs_to_jiffies(ms);
  mod_timer(&adapter->timer, adapter->timer.expires);
}

void bf_kpkt_timer_del(struct bf_kpkt_adapter *adapter) {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 6, 0)
  timer_delete_sync(&adapter->timer);
#else
  del_timer_sync(&adapter->timer);
#endif
}
