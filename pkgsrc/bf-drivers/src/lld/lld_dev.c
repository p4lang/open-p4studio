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
#include <string.h>
#else
#include <linux/string.h>
#endif
#include <dvm/bf_drv_intf.h>
#include <lld/bf_dma_if.h>
#include <lld/bf_dev_if.h>
#include <lld/lld_reg_if.h>
#include <lld/lld_spi_if.h>
#include <lld/lld_err.h>
#include <lld/lld_sku.h>
#include <lld/lld_interrupt_if.h>
#include <lld/tofino_defs.h>
#include "lld.h"
#include "lld_dev.h"
#include "lld_dev_lock.h"
#include "lld_dev_tof.h"
#include "lld_dev_tof2.h"
#include "lld_dev_tof3.h"

#include "lld_efuse_tof3.h"
#include "lld_map.h"
#include "lld_log.h"
#include "lld_csr2jtag.h"
#include <tofino_regs/tofino.h>
#include <tof2_regs/tof2_reg_drv.h>
#include <tof3_regs/tof3_reg_drv.h>

// fwd ref
#ifndef BYPASS_LLD
static void lld_dev_un_reset(bf_dev_id_t dev_id, bf_subdev_id_t subdev_id);
#endif

static void lld_dev_tlp_set(bf_dev_id_t dev_id,
                            bf_subdev_id_t subdev_id,
                            bf_dev_family_t dev_family,
                            bool en);

static bf_status_t lld_dev_spi_idcode_get(lld_dev_t *dev_p,
                                          uint32_t *spi_idcode);

/** \brief Quick warm init on the device
 *
 * \param dev_id        : system-assigned identifier (0..BF_MAX_DEV_COUNT-1)
 * \param subdev_id     : sub device within dev_id (0..BF_MAX_SUBDEV_COUNT-1)
 *
 * \return: BF_SUCCESS     : dev_id added successfully
 * \return: BF_INVALID_ARG : invalid dev_id
 */
bf_status_t lld_warm_init_quick(bf_dev_id_t dev_id) {
  lld_dev_t *dev_p;
  bf_subdev_id_t subdev_id = 0;

  dev_p = lld_map_subdev_id_to_dev_p_allow_unassigned(dev_id, subdev_id);
  if (dev_p == NULL) return BF_INVALID_ARG;

  return BF_SUCCESS;
}

/********************************************************************
 * lld_dev_pcie_opt_set
 *
 * set options for link down, L2 state exit and host pcie reset
 *******************************************************************/
static void lld_dev_pcie_opt_set(bf_dev_id_t dev_id,
                                 bf_subdev_id_t subdev_id,
                                 bf_dev_family_t dev_family) {
  uint32_t offset;

  switch (dev_family) {
    case BF_DEV_FAMILY_TOFINO3:
      offset = offsetof(tof3_reg,
                        device_select.misc_all_regs.misc_regs.reset_option);
      lld_subdev_write_register(dev_id, subdev_id, offset, 0x070000A8UL);
      break;
    default:
      break;
  }
}

/** \brief Add a device to the system
 *
 * \param dev_id        : system-assigned identifier (0..BF_MAX_DEV_COUNT-1)
 * \param subdev_id     : subdevice identifier (0..BF_MAX_SUBDEV_COUNT-1),
 *                        within dev_id
 * \param dev_family    : Tofino, ...
 * \param profile       : unused (common param for other modules)
 * \param dma_info      : DMA DR configuration (used, size)
 * \param warm_init_mode: driver start-up mode (cold, warm, fast-recfg)
 *
 * \return: BF_SUCCESS     : dev_id added successfully
 * \return: BF_INVALID_ARG : invalid dev_id
 */
static bf_status_t lld_dev_add_subdev(bf_dev_id_t dev_id,
                                      bf_subdev_id_t subdev_id,
                                      bf_dev_family_t dev_family,
                                      bf_device_profile_t *profile,
                                      struct bf_dma_info_s *dma_info,
                                      bf_dev_init_mode_t warm_init_mode) {
  bf_status_t status;
  lld_dev_t *dev_p;
  uint32_t spi_idcode = 0;

  dev_p = lld_map_subdev_id_to_dev_p_allow_unassigned(dev_id, subdev_id);
  if (dev_p == NULL) return BF_INVALID_ARG;

  if (lld_is_master() && dev_p->assigned) {
    /* In case of master, the device would have been already added
     * with lld_master_dev_add
     */
    return BF_SUCCESS;
  }

  dev_p->dev_family = dev_family;
  dev_p->ready = 0;
  dev_p->assigned = 1;
  dev_p->dev_id = dev_id;
  dev_p->subdev_id = subdev_id;

  /* set pcie link behavior options */
  lld_dev_pcie_opt_set(dev_id, subdev_id, dev_p->dev_family);
  /* disable reporting of EP bit in TLP frame */
  /* Enable replacement of data by 0x0ECC0ECC in case of poison bit set */
  lld_dev_tlp_set(dev_id, subdev_id, dev_p->dev_family, 0);

#ifndef __KERNEL__
  /* Before loading efuse, run any csr2jtag scripts. */
  if (lld_is_master()) {
    lld_csr2jtag_run_pre_efuse(dev_id, subdev_id);
  }
#endif /* __KERNEL__ */

  /* Read the efuse and use that to decide the actual device type. */
  if (lld_efuse_load(dev_id, subdev_id, warm_init_mode)) {
    lld_log("error loading fuse for device %d subdevice %d", dev_id, subdev_id);
    return BF_UNEXPECTED;
  }

  dev_p->dev_type = lld_sku_get_subdev_type(dev_id, subdev_id);
  dev_p->subdev_id = subdev_id;

  /* log the SPI idcode (in case the SPI ROM was not loaded correctly */
  status = lld_dev_spi_idcode_get(dev_p, &spi_idcode);
  lld_log("dev %d subdev %d: SPI idcode: %08x (rc=%d)",
          dev_id,
          subdev_id,
          spi_idcode,
          status);
  /* check for two error cases:
   * 1: the chip is not accessible, in which case we would get 0xffffffff
   * 2: the SPI ROM was not loaded at all, in which case we would get 0x0
   */
  if (spi_idcode == 0xffffffff) {
    lld_log("dev%d:%d : *********************************************",
            dev_id,
            subdev_id);
    lld_log("dev%d:%d : *** WARNING: device may not be accessible ***",
            dev_id,
            subdev_id);
    lld_log("dev%d:%d : *********************************************",
            dev_id,
            subdev_id);
    return BF_HW_COMM_FAIL;
  } else if (spi_idcode == 0x0) {
    lld_log("dev%d:%d : ****************************************************",
            dev_id,
            subdev_id);
    lld_log("dev%d:%d : *** WARNING: SPI ROM may not be loaded correctly ***",
            dev_id,
            subdev_id);
    lld_log("dev%d:%d : ****************************************************",
            dev_id,
            subdev_id);
  }

#ifndef BYPASS_LLD
  /* save the DMA DR configuration */
  if (dma_info) {
    dev_p->dma_info = *dma_info;
  }

  lld_sku_pipe_id_map_get(dev_id,
                          dev_p->pipe_log2phy);  // TBD for TF3 sub devices

  if (lld_is_master()) {
    /* On cold init wait for asic to be ready,
     * For fast-reconfig, skip chip unreset as it happens in
     * lld_reset_core() callback
     */
    if (!lld_dev_is_locked(dev_id, subdev_id)) {
      lld_dev_un_reset(dev_id, subdev_id);

#ifndef __KERNEL__
      /* Immediately after core and mac un-reset run any csr2jtag scripts. */
      lld_csr2jtag_run_post_efuse(dev_id, subdev_id);
#endif /* __KERNEL__ */

      /* mask all top level interrupts */
      lld_int_gbl_en_set(dev_id, subdev_id, false);
    }

    /* Ensure all DRs are disabled and flushed. */
    lld_subdev_dr_run_flush_sequence(dev_id, subdev_id);
  }

  /* Initialize any requested DMA DRs */
  lld_dr_init_dev(dev_id, subdev_id, dma_info);

#ifndef __KERNEL__
  if (lld_spi_init(dev_id, subdev_id) != BF_SUCCESS) {
    return LLD_ERR_INVALID_CFG;
  }
#endif /* __KERNEL__ */

  // finally, mark it ready
  dev_p->ready = 1;

#ifndef __KERNEL__
  // set default handler for interrupts
  lld_register_default_handler_for_all_ints(dev_id, subdev_id);

  /* Mask all interrupts at the shadow regs */
  bf_int_msk_all(dev_id, subdev_id);

  /* Disable all leaf interrupts */
  lld_int_disable_all(dev_id, subdev_id);

  int i;
  /* apply msix_map for Tofino-2 and later */
  for (i = 0; i < LLD_TOFINO_NUM_IRQ; i++) {
    int msix_num = dev_p->irq_to_msix[i];
    status = bf_int_msix_map_set(dev_id, subdev_id, i, msix_num);
    if (status != BF_SUCCESS) {
      lld_log(
          "msix_map configuration failed for device %d subdev %d, irq %d msix "
          "%d sts %s",
          dev_id,
          subdev_id,
          i,
          msix_num,
          bf_err_str(status));
    }
  }
  // Claim the lld (host) interrupts
  status = bf_int_claim_host(dev_id, subdev_id);
  if (status != BF_SUCCESS) {
    lld_log(
        "interrupt registration with LLD failed for device %d subdev %d, sts "
        "%s (%d)",
        dev_id,
        subdev_id,
        bf_err_str(status),
        status);
    return status;
  }

  // Enable the claimed lld (host) inerrupts
  status = bf_int_ena_host(dev_id, subdev_id);
  if (status != BF_SUCCESS) {
    lld_log(
        "interrupt enable with LLD failed for device %d subdev %d, sts %s (%d)",
        dev_id,
        subdev_id,
        bf_err_str(status),
        status);
    return status;
  }
#endif /* __KERNEL__ */
#else  /* BYPASS_LLD */
  dev_p->ready = 1;
  (void)dma_info;
  (void)status;
#endif
  return BF_SUCCESS;
  (void)profile;
  (void)warm_init_mode;
}

/** \brief Add a device to the system
 *
 * \param dev_id        : system-assigned identifier (0..BF_MAX_DEV_COUNT-1)
 * \param dev_family    : Tofino, ...
 * \param profile       : unused (common param for other modules)
 * \param dma_info      : DMA DR configuration (used, size)
 * \param warm_init_mode: driver start-up mode (cold, warm, fast-recfg)
 *
 * \return: BF_SUCCESS     : dev_id added successfully
 * \return: BF_INVALID_ARG : invalid dev_id
 */
bf_status_t lld_dev_add(bf_dev_id_t dev_id,
                        bf_dev_family_t dev_family,
                        bf_device_profile_t *profile,
                        struct bf_dma_info_s *dma_info,
                        bf_dev_init_mode_t warm_init_mode) {
  bf_status_t status = BF_SUCCESS;
  lld_dev_t *dev_p;
  lld_err_t lld_rc;
  uint32_t subdev = 0, num_subdev, subdev_msk;

  /* Look up the first sub-device for this ASIC. */
  dev_p = lld_map_subdev_id_to_dev_p_allow_unassigned(dev_id, 0);
  if (dev_p == NULL) return BF_INVALID_ARG;
  if (lld_is_master() && dev_p->assigned) {
    return BF_SUCCESS;
  }

  dev_p->dev_family = dev_family;
  dev_p->ready = 0;

  /* Add the first subdevice (we always have at least one). */
  status = lld_dev_add_subdev(
      dev_id, subdev, dev_family, profile, dma_info + subdev, warm_init_mode);
  if (BF_SUCCESS != status) {
    lld_log("error adding subdevice %d for device %d", subdev, dev_id);
    return status;
  }

  /* Now that the first subdevice has been added and its EFuse loaded check for
   * additional subdevices and add them if needed. */
  lld_rc = lld_sku_get_num_subdev(dev_id, &num_subdev, &subdev_msk);
  if (lld_rc != LLD_OK) {
    lld_log("error getting number of sub devices for device %d", dev_id);
    return BF_UNEXPECTED;
  }
  dev_p->num_subdev = num_subdev;
  dev_p->subdev_msk = subdev_msk;
  for (subdev = 1; subdev < num_subdev; ++subdev) {
    status = lld_dev_add_subdev(
        dev_id, subdev, dev_family, profile, dma_info + subdev, warm_init_mode);
    if (BF_SUCCESS != status) {
      lld_log("error adding subdevice %d for device %d", subdev, dev_id);
      return status;
    }
  }
  return BF_SUCCESS;
}

/** \brief Add a device to the system
 *
 * \param dev_id        : system-assigned identifier (0..BF_MAX_DEV_COUNT-1)
 * \param dev_family    : Tofino, ...
 * \param dma_info      : DMA DR configuration (used, size)
 * \param warm_init_mode: driver start-up mode (cold, warm, fast-recfg)
 *
 * \return: BF_SUCCESS     : dev_id added successfully
 * \return: BF_INVALID_ARG : invalid dev_id
 *
 */
bf_status_t lld_master_dev_add(bf_dev_id_t dev_id,
                               bf_dev_family_t dev_family,
                               struct bf_dma_info_s *dma_info,
                               bf_dev_init_mode_t warm_init_mode) {
  return lld_dev_add(dev_id, dev_family, NULL, dma_info, warm_init_mode);
}

/** \brief Remove a device from the system
 *
 * \param dev_id: system-assigned identifier (0..BF_MAX_DEV_COUNT-1)
 *
 * \return: BF_SUCCESS     : dev_id removed successfully
 * \return: BF_INVALID_ARG : dev_id invalid
 */
bf_status_t lld_dev_remove(bf_dev_id_t dev_id) {
  bf_subdev_id_t subdev_id;
  lld_dev_t *dev_p;

  for (subdev_id = 0; subdev_id < BF_MAX_SUBDEV_COUNT; subdev_id++) {
    dev_p = lld_map_subdev_id_to_dev_p_allow_unassigned(dev_id, subdev_id);
    if (dev_p != NULL) {
#ifndef __KERNEL__
      for (int i = 0; i < BF_DMA_MAX_DR; ++i) {
        if (dev_p->dr_view[i].dr_buffer) {
          bf_sys_dma_free(dev_p->dma_info.dma_dr_pool_handle,
                          dev_p->dr_view[i].dr_buffer);
        }
      }
#endif
      memset(dev_p, 0, sizeof *dev_p);
    } else {
      return BF_INVALID_ARG;
    }
  }
  return BF_SUCCESS;
}

/*************************************************
 * lld_dev_un_reset
 *
 * Bring all blocks out of reset
 *************************************************/
#ifndef BYPASS_LLD
static void lld_dev_un_reset(bf_dev_id_t dev_id, bf_subdev_id_t subdev_id) {
  lld_dev_t *dev_p = lld_map_subdev_id_to_dev_p(dev_id, subdev_id);
  if (!dev_p) return;
  switch (dev_p->dev_family) {
    case BF_DEV_FAMILY_TOFINO:
      if (!lld_dev_is_locked(dev_id, 0)) {
        /* Put the chip in test mode by issuing the tcu sequence so that
         * the memories are repaired. This needs to be done only during
         * COLD boot */
        lld_dev_tof_tcu_seq(dev_id);
      }
      lld_dev_tof_un_reset(dev_id);
      break;
    case BF_DEV_FAMILY_TOFINO2:
      lld_dev_tof2_un_reset(dev_id);
      break;
    case BF_DEV_FAMILY_TOFINO3:
      lld_dev_tof3_un_reset(dev_id, subdev_id);
      break;



    case BF_DEV_FAMILY_UNKNOWN:
      break;
  }
}
#endif

/********************************************************************
 * lld_tlp_set
 *
 * enables/disables insersion of poison bit in TLP frame sent by Tofino
 *******************************************************************/
static void lld_dev_tlp_set(bf_dev_id_t dev_id,
                            bf_subdev_id_t subdev_id,
                            bf_dev_family_t dev_family,
                            bool en) {
  switch (dev_family) {
    case BF_DEV_FAMILY_TOFINO:
      lld_dev_tof_tlp_poison_set(dev_id, en);
      break;
    case BF_DEV_FAMILY_TOFINO2:
      lld_dev_tof2_tlp_poison_set(dev_id, en);
      break;
    case BF_DEV_FAMILY_TOFINO3:
      lld_dev_tof3_tlp_poison_set(dev_id, subdev_id, en);
      break;



    case BF_DEV_FAMILY_UNKNOWN:
      break;
  }
}

/********************************************************************
 * lld_reset_core
 *
 * Resets core but leave NIF untouched
 *******************************************************************/
bf_status_t lld_reset_core(bf_dev_id_t dev_id) {
  bf_subdev_id_t subdev_id = 0;
  lld_dev_t *dev_p =
      lld_map_subdev_id_to_dev_p_allow_unassigned(dev_id, subdev_id);

  if (dev_p == NULL) return BF_INVALID_ARG;

  // carry out core_reset
  // Initialize DMA rings
  // Reset interrupt registers
  if (lld_dev_is_tofino(dev_id)) {
    lld_dev_tof_reset_core(dev_id);
    lld_dr_init_dev(dev_id, 0, &(dev_p->dma_info));
    lld_int_disable_all(dev_id, 0);
  } else if (lld_dev_is_tof2(dev_id)) {
    lld_dev_tof2_reset_core(dev_id);
    lld_dr_init_dev(dev_id, 0, &(dev_p->dma_info));
    lld_int_disable_all(dev_id, 0);
  } else if (lld_dev_is_tof3(dev_id)) {
    for (subdev_id = 0; subdev_id < (bf_subdev_id_t)dev_p->num_subdev;
         subdev_id++) {
      lld_dev_tof3_reset_core(dev_id, subdev_id);
      lld_dr_init_dev(dev_id, subdev_id, &(dev_p->dma_info));
      lld_int_disable_all(dev_id, subdev_id);
    }
  }

  return BF_SUCCESS;
}

/********************************************************************
 * lld_dev_ready
 * Returns 1 if specified asic has been initialized to the point
 * it is ready for general use.
 *******************************************************************/
bool lld_dev_ready(bf_dev_id_t dev_id, bf_subdev_id_t subdev_id) {
  lld_dev_t *dev_p = lld_map_subdev_id_to_dev_p(dev_id, subdev_id);

  if (dev_p == NULL) {
    return false;
  }
  if (!dev_p->ready) {
    return false;
  }
  return true;
}

/********************************************************************
 * lld_dev_type_get
 * Return device type (tof-full, tof-half-odd, ..)
 *******************************************************************/
bf_dev_type_t lld_dev_type_get(bf_dev_id_t dev_id) {
  lld_dev_t *dev_p = lld_map_dev_id_to_dev_p(dev_id);

  return dev_p ? dev_p->dev_type : BF_DEV_UNKNOWN;
}

/********************************************************************
 * lld_dev_family_get
 * Return device family
 *******************************************************************/
bf_dev_family_t lld_dev_family_get(bf_dev_id_t dev_id) {
  lld_dev_t *dev_p = lld_map_dev_id_to_dev_p_allow_unassigned(dev_id);
  return dev_p ? dev_p->dev_family : BF_DEV_FAMILY_UNKNOWN;
}

/********************************************************************
 * lld_dev_is_tofino
 * Check if dev_id is a Tofino family member
 *******************************************************************/
bool lld_dev_is_tofino(bf_dev_id_t dev_id) {
  return lld_dev_family_get(dev_id) == BF_DEV_FAMILY_TOFINO;
}

/********************************************************************
 * lld_dev_is_tof2
 * Check if dev_id is a Tof2 family member
 *******************************************************************/
bool lld_dev_is_tof2(bf_dev_id_t dev_id) {
  return lld_dev_family_get(dev_id) == BF_DEV_FAMILY_TOFINO2;
}

/********************************************************************
 * lld_dev_is_tof3
 * Check if dev_id is a Tof3 family member
 *******************************************************************/
bool lld_dev_is_tof3(bf_dev_id_t dev_id) {
  return lld_dev_family_get(dev_id) == BF_DEV_FAMILY_TOFINO3;
}











/**
 * @brief  lld_dev_spi_idcode_get
 *  returns the spi idcode read during bootup
 *
 * @param dev_p: lld_dev_t *
 *  Pointer to device struct
 *
 * @param spi_idcode: uint32_t *
 *  spi ID code
 *
 * @return status
 *   BF_SUCCESS on success
 *   BF ERROR code on failure
 *
 */
static bf_status_t lld_dev_spi_idcode_get(lld_dev_t *dev_p,
                                          uint32_t *spi_idcode) {
  uint32_t offset;
  bf_dev_id_t dev_id;
  bf_subdev_id_t subdev_id;

  if (!spi_idcode) return BF_INVALID_ARG;
  if (!dev_p) return BF_INVALID_ARG;
  dev_id = dev_p->dev_id;
  subdev_id = dev_p->subdev_id;

  switch (dev_p->dev_family) {
    case BF_DEV_FAMILY_TOFINO:
      offset = offsetof(Tofino, device_select.misc_regs.spi_idcode);
      break;
    case BF_DEV_FAMILY_TOFINO2:
      offset = offsetof(tof2_reg, device_select.misc_regs.spi_idcode);
      break;
    case BF_DEV_FAMILY_TOFINO3:
      offset =
          offsetof(tof3_reg, device_select.misc_all_regs.misc_regs.spi_idcode);
      break;



    case BF_DEV_FAMILY_UNKNOWN:
    default:
      return BF_INVALID_ARG;
  }
  return lld_subdev_read_register(dev_id, subdev_id, offset, spi_idcode);
}
