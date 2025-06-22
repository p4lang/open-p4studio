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
#include "lld_subdev_reg_if.h"
#include "lld.h"
#include "lld_map.h"
#include "lld_dev.h"

/** \brief lld_subdev_wr
 *         Write a 32b device register via the PCIe interface
 *
 * \param dev_id: dev_id #
 * \param subdev_id: sub device # within dev_id
 * \param reg   : Tofino register offset
 * \param data  : 32-bit value to write to reg
 *
 * \return LLD_OK (0)
 * \return LLD_ERR_BAD_PARM : bad chip
 * \return LLD_ERR_BAD_PARM : bad sub device id
 * \return LLD_ERR_BAD_PARM : register not 32-bit aligned
 *
 */
int lld_subdev_wr(bf_dev_id_t dev_id,
                  bf_subdev_id_t subdev_id,
                  uint32_t reg,
                  uint32_t data)

{
  lld_dev_t *dev_p = lld_map_subdev_id_to_dev_p(dev_id, subdev_id);

  if (dev_p == NULL) return LLD_ERR_BAD_PARM;
  if (reg & 0x3) return LLD_ERR_BAD_PARM;

  lld_ctx->wr_fn(dev_id, subdev_id, reg, data);
  return LLD_OK;
}

/** \brief lld_subdev_rd
 *         Read a 32b device register via the PCIe interface
 *
 * \param dev_id: dev_id #
 * \param subdev_id: sub device # within dev_id
 * \param reg   : Tofino register offset
 * \param val   : Pointer where register contents will be written
 *
 * \return LLD_OK (0)
 * \return LLD_ERR_BAD_PARM : bad dev_id
 * \return LLD_ERR_BAD_PARM : bad sub device id
 * \return LLD_ERR_BAD_PARM : register not 32-bit aligned
 * \return LLD_ERR_BAD_PARM : return value ptr NULL
 *
 */
int lld_subdev_rd(bf_dev_id_t dev_id,
                  bf_subdev_id_t subdev_id,
                  uint32_t reg,
                  uint32_t *val) {
  lld_dev_t *dev_p = lld_map_subdev_id_to_dev_p(dev_id, subdev_id);

  if (dev_p == NULL) return LLD_ERR_BAD_PARM;
  if (reg & 0x3) return LLD_ERR_BAD_PARM;
  if (!val) return LLD_ERR_BAD_PARM;

  lld_ctx->rd_fn(dev_id, subdev_id, reg, val);

  return LLD_OK;
}
