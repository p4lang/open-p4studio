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


/**
 * @file lld_reg_if.c
 * \brief Details Register access APIs.
 *
 */

/**
 * @addtogroup lld-reg-api
 * @{
 * This is a description of some APIs.
 */

#ifndef __KERNEL__
#include <inttypes.h>
#else
#define bf_sys_assert()
#endif

#include <dvm/bf_drv_intf.h>
#include <lld/bf_dev_if.h>
#include "lld.h"
#include "lld_map.h"
#include "lld_dev.h"
#include "lld/lld_reg_if.h"
#include "lld_subdev_reg_if.h"

#ifdef DEVICE_IS_EMULATOR
#include <lld/lld_sku.h>

static inline bool is_phy_pipe_valid(bf_dev_id_t dev_id, uint32_t phy_pipe_id) {
  lld_err_t r = LLD_OK;
  uint32_t log_pipe_id = 0;
  uint32_t num_pipes = 0;
  r = lld_sku_map_phy_pipe_id_to_pipe_id(dev_id, phy_pipe_id, &log_pipe_id);
  if (r != LLD_OK) return false;
  r = lld_sku_get_num_active_pipes(dev_id, &num_pipes);
  if (r != LLD_OK) return false;
  if (log_pipe_id >= num_pipes) return false;
  return true;
}
int lld_check_addr_valid(bf_dev_id_t dev_id, uint32_t reg) {
  uint32_t phy_pipe_id = 0, stage_id = 0;
  uint32_t num_mau_stages = 0, prsr_stage = 0;
  uint32_t dev_type = 0;
  if (lld_dev_is_tof2(dev_id) || lld_dev_is_tof3(dev_id)) {
    if (reg & 0x4000000) {
      /* Pipe addresses (PBUS) - Validate the stage id and pipe. */
      /* Check the pipe id. */
      phy_pipe_id = (reg >> 24) & 0x3;
      if (!is_phy_pipe_valid(dev_id, phy_pipe_id)) return LLD_ERR_BAD_PARM;
      stage_id = (reg & 0xf80000) >> 19;
      /* Get stage info. */
      lld_sku_get_num_active_mau_stages(dev_id, &num_mau_stages, phy_pipe_id);
      lld_sku_get_prsr_stage(dev_id, &prsr_stage);
      if (stage_id >= num_mau_stages && stage_id < prsr_stage) {
        return LLD_ERR_BAD_PARM;
      }
    } else if (((reg >> 24) & 0x7) == 0) {
      /* Device Select address (CBUS) - Validate learn filter id. */
      dev_type = (reg >> 19) & 0x1F;
      if (dev_type >= 0x8 && dev_type <= 0xB) {
        /* Learn Filters */
        uint32_t which_filter = dev_type - 0x8;
        /* Filter maps one-to-one with pipe so validate it like a pipe. */
        if (!is_phy_pipe_valid(dev_id, which_filter)) return LLD_ERR_BAD_PARM;
      }
    }
  } else if (lld_dev_is_tofino(dev_id)) {
  } else {
    bf_sys_assert(0);
  }
  return LLD_OK;
}
#endif

/** \brief lld_subdev_write_register
 *         Write a 32b device register via the PCIe interface
 *
 * \param dev_id: dev_id #
 * \param subdev_id: subdev_id #
 * \param reg   : Tofino register offset
 * \param data  : 32-bit value to write to reg
 *
 * \return LLD_OK (0)
 * \return LLD_ERR_BAD_PARM : bad dev_id
 * \return LLD_ERR_BAD_PARM : bad subdev_id
 * \return LLD_ERR_BAD_PARM : register not 32-bit aligned
 *
 */
int lld_subdev_write_register(bf_dev_id_t dev_id,
                              bf_dev_id_t subdev_id,
                              uint32_t reg,
                              uint32_t data) {
  lld_dev_t *dev_p = lld_map_subdev_id_to_dev_p(dev_id, subdev_id);

  if (dev_p == NULL) return LLD_ERR_BAD_PARM;
  if (reg & 0x3) return LLD_ERR_BAD_PARM;

#ifdef DEVICE_IS_EMULATOR
  if (lld_check_addr_valid(dev_id, reg) != LLD_OK) return LLD_OK;
#endif

  lld_subdev_wr(dev_id, subdev_id, reg, data);
  return LLD_OK;
}

/** \brief lld_write_register
 *         Write a 32b device register via the PCIe interface
 *
 * \param dev_id: dev_id #
 * \param reg   : Tofino register offset
 * \param data  : 32-bit value to write to reg
 *
 * \return LLD_OK (0)
 * \return LLD_ERR_BAD_PARM : bad chip
 * \return LLD_ERR_BAD_PARM : register not 32-bit aligned
 *
 */
int lld_write_register(bf_dev_id_t dev_id, uint32_t reg, uint32_t data) {
  return lld_subdev_write_register(dev_id, 0, reg, data);
}

/** \brief lld_subdev_read_register
 *         Read a 32b device register via the PCIe interface
 *
 * \param dev_id: dev_id #
 * \param subdev_id: subdev_id #
 * \param reg   : Tofino register offset
 * \param val   : Pointer where register contents will be written
 *
 * \return LLD_OK (0)
 * \return LLD_ERR_BAD_PARM : bad dev_id
 * \return LLD_ERR_BAD_PARM : bad subdev_id
 * \return LLD_ERR_BAD_PARM : register not 32-bit aligned
 * \return LLD_ERR_BAD_PARM : return value ptr NULL
 *
 */
int lld_subdev_read_register(bf_dev_id_t dev_id,
                             bf_subdev_id_t subdev_id,
                             uint32_t reg,
                             uint32_t *val) {
  lld_dev_t *dev_p = lld_map_subdev_id_to_dev_p(dev_id, subdev_id);

  if (dev_p == NULL) return LLD_ERR_BAD_PARM;
  if (reg & 0x3) return LLD_ERR_BAD_PARM;
  if (!val) return LLD_ERR_BAD_PARM;

#ifdef DEVICE_IS_EMULATOR
  if (lld_check_addr_valid(dev_id, reg) != LLD_OK) {
    *val = 0;
    return LLD_OK;
  }
#endif

  lld_subdev_rd(dev_id, subdev_id, reg, val);

  return LLD_OK;
}

/** \brief lld_read_register
 *         Read a 32b device register via the PCIe interface
 *
 * \param dev_id: dev_id #
 * \param reg   : Tofino register offset
 * \param val   : Pointer where register contents will be written
 *
 * \return LLD_OK (0)
 * \return LLD_ERR_BAD_PARM : bad dev_id
 * \return LLD_ERR_BAD_PARM : register not 32-bit aligned
 * \return LLD_ERR_BAD_PARM : return value ptr NULL
 *
 */
int lld_read_register(bf_dev_id_t dev_id, uint32_t reg, uint32_t *val) {
  return lld_subdev_read_register(dev_id, 0, reg, val);
}
