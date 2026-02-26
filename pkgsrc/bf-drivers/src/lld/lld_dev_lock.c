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


#ifndef __KERNEL__
#include <string.h>  //for memset
#else
#include <linux/string.h>
#define bf_sys_assert(x) \
  do {                   \
  } while (0)
#endif

#include <dvm/bf_drv_intf.h>
#include "lld_dev_lock.h"
// "locked" indicates the device should not be modified
// Used during upgrade sequences.
static bool lld_dev_locked[BF_MAX_DEV_COUNT][BF_MAX_SUBDEV_COUNT] = {0};

// per device lock for indirect access register
bf_sys_mutex_t lld_reg_access_mtx[BF_MAX_DEV_COUNT][BF_MAX_SUBDEV_COUNT];

/********************************************************************
 * lld_dev_lock
 * Prevent API access to device (used during fast-reconfig)
 *******************************************************************/
bf_status_t lld_subdev_lock(bf_dev_id_t dev_id, bf_subdev_id_t subdev_id) {
  if (dev_id >= BF_MAX_DEV_COUNT || subdev_id >= BF_MAX_SUBDEV_COUNT) {
    return BF_INVALID_ARG;
  }
  lld_dev_locked[dev_id][subdev_id] = true;

  return BF_SUCCESS;
}

/********************************************************************
 * lld_dev_unlock
 * Re-Allow API access to device (used during fast-reconfig)
 *******************************************************************/
bf_status_t lld_subdev_unlock(bf_dev_id_t dev_id, bf_subdev_id_t subdev_id) {
  if (dev_id >= BF_MAX_DEV_COUNT || subdev_id >= BF_MAX_SUBDEV_COUNT) {
    return BF_INVALID_ARG;
  }
  lld_dev_locked[dev_id][subdev_id] = false;
  return BF_SUCCESS;
}

/********************************************************************
 * lld_dev_lock
 * Prevent API access to device (used during fast-reconfig)
 *******************************************************************/
bf_status_t lld_dev_lock(bf_dev_id_t dev_id) {
  bf_subdev_id_t subdev_id;
  bf_status_t status = BF_SUCCESS;

  for (subdev_id = 0; subdev_id < BF_MAX_SUBDEV_COUNT; subdev_id++) {
    status |= lld_subdev_lock(dev_id, subdev_id);
  }
  return status;
}

/********************************************************************
 * lld_dev_unlock
 * Re-Allow API access to device (used during fast-reconfig)
 *******************************************************************/
bf_status_t lld_dev_unlock(bf_dev_id_t dev_id) {
  bf_subdev_id_t subdev_id;
  bf_status_t status = BF_SUCCESS;

  for (subdev_id = 0; subdev_id < BF_MAX_SUBDEV_COUNT; subdev_id++) {
    status |= lld_subdev_unlock(dev_id, subdev_id);
  }
  return BF_SUCCESS;
}

/********************************************************************
 * lld_dev_is_locked
 * Check if API access to device is allowed.
 *******************************************************************/
bool lld_dev_is_locked(bf_dev_id_t dev_id, bf_subdev_id_t subdev_id) {
  if (dev_id >= BF_MAX_DEV_COUNT || subdev_id >= BF_MAX_SUBDEV_COUNT) {
    return false;
  }
  return lld_dev_locked[dev_id][subdev_id];
}

/*****************************************************************
 * lld_dev_lock_ind_reg_mutex_init
 *
 * Initialize mutexes for all devices. Called by lld_init()
 ****************************************************************/
void lld_dev_lock_ind_reg_mutex_init(void) {
  bf_dev_id_t dev_id;
  bf_subdev_id_t subdev_id;

  for (dev_id = 0; dev_id < BF_MAX_DEV_COUNT; dev_id++) {
    for (subdev_id = 0; subdev_id < BF_MAX_SUBDEV_COUNT; subdev_id++) {
      int x;

      x = bf_sys_mutex_init(&lld_reg_access_mtx[dev_id][subdev_id]);
      bf_sys_assert((x == 0));
    }
  }
}

/*****************************************************************
 * lld_dev_lock_ind_reg_lock
 *
 ****************************************************************/
void lld_dev_lock_ind_reg_lock(bf_dev_id_t dev_id, bf_subdev_id_t subdev_id) {
  int x;
  x = bf_sys_mutex_lock(&lld_reg_access_mtx[dev_id][subdev_id]);
  bf_sys_assert((x == 0));
}

/*****************************************************************
 * lld_dev_lock_ind_reg_unlock
 *
 ****************************************************************/
void lld_dev_lock_ind_reg_unlock(bf_dev_id_t dev_id, bf_subdev_id_t subdev_id) {
  int x;
  x = bf_sys_mutex_unlock(&lld_reg_access_mtx[dev_id][subdev_id]);
  bf_sys_assert((x == 0));
}
