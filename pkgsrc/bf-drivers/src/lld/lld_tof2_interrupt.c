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


#include "lld.h"
#include "lld_log.h"
#include "lld_map.h"
#include "lld_dev.h"
#include <lld/lld_reg_if.h>
#include <lld/lld_dr_if.h>
#include <lld/lld_sku.h>
#include "lld_memory_mapping.h"
#include "lld_tof2_interrupt.h"
#include <lld/tof2_reg_drv_defs.h>
#include "lld_tof2_interrupt_struct.h"
#ifndef __KERNEL__
#include <diag/bf_dev_diag.h>
#include <lld/lld_diag_ext.h>
#endif

// for telnet access
extern void *rl_outstream;

extern int get_reg_num_fields(bf_dev_id_t dev_id, uint32_t offset);

void process_host_ints(bf_dev_id_t dev_id, uint32_t sh_ints);
void process_tbus_ints(bf_dev_id_t dev_id, uint32_t sh_ints);
void process_cbus_ints(bf_dev_id_t dev_id, uint32_t sh_ints, int n);
void process_pbus_ints(bf_dev_id_t dev_id,
                       uint32_t sh_ints,
                       int n,
                       int all_ints);
void process_mbus_ints(bf_dev_id_t dev_id, uint32_t sh_ints, int n);
void port_mgr_tof2_port_default_handler_for_mac_ints(bf_dev_id_t dev_id,
                                                     int ch,
                                                     uint32_t reg,
                                                     uint32_t set_int_bits,
                                                     void *userdata);

typedef enum {
  lld_tof2_shadow_reg_host_bus_0 = 0,
  lld_tof2_shadow_reg_tbus_1,
  lld_tof2_shadow_reg_cbus_2,
  lld_tof2_shadow_reg_cbus_3,
  lld_tof2_shadow_reg_mbus_4,
  lld_tof2_shadow_reg_mbus_5,
  lld_tof2_shadow_reg_mbus_6,
  lld_tof2_shadow_reg_mbus_7,
  lld_tof2_shadow_reg_pbus_8,
  lld_tof2_shadow_reg_pbus_9,
  lld_tof2_shadow_reg_pbus_10,
  lld_tof2_shadow_reg_pbus_11,
  lld_tof2_shadow_reg_pbus_12,
  lld_tof2_shadow_reg_pbus_13,
  lld_tof2_shadow_reg_pbus_14,
  lld_tof2_shadow_reg_pbus_15,
} lld_tof2_shadow_reg_bus_t;

static bf_status_t lld_tof2_int_traverse_blk_list(
    bf_dev_id_t dev_id,
    lld_tof2_blk_lvl_int_list_t *int_list,
    lld_blk_int_traverse_cb cb_fn) {
  bf_status_t status = BF_SUCCESS;
  int i, j;
  if (!int_list->is_leaf) {
    lld_tof2_blk_lvl_int_list_t *temp_list = int_list->u.blk_lvl_list;
    for (j = 0; ((j < (int)int_list->n) && (temp_list[j].reg_top != 0)); j++) {
      lld_tof2_int_traverse_blk_list(dev_id, &temp_list[j], cb_fn);
    }
  } else {
    for (i = 0;
         ((i < (int)int_list->n) && (int_list->u.blk_lvl_int[i].status_reg));
         i++) {
      status |= cb_fn(dev_id, 0, (void *)&(int_list->u.blk_lvl_int[i]));
    }
  }
  return status;
}

bf_status_t lld_tof2_int_traverse_blk_lvl_int(bf_dev_id_t dev_id,
                                              lld_blk_int_traverse_cb cb_fn) {
  int i, j;
  bf_status_t status = BF_SUCCESS;
  int array_len;
  for (i = 0; i < 16; i++) {
    j = 0;
    array_len = (int)(sizeof(Tof2_int_top[i]) / sizeof(Tof2_int_top[i][0]));
    while ((j < array_len) && (Tof2_int_top[i][j].reg_top != 0)) {
      // the valid element
      status |=
          lld_tof2_int_traverse_blk_list(dev_id, &Tof2_int_top[i][j], cb_fn);
      j++;
    }
  }
  return status;
}

bf_status_t lld_tof2_clear_all_ints_cb(bf_dev_id_t dev_id,
                                       bf_subdev_id_t subdev_id,
                                       void *blk_lvl_int_vd) {
  int i;
  lld_tof2_blk_lvl_int_t *blk_lvl_int =
      (lld_tof2_blk_lvl_int_t *)blk_lvl_int_vd;
  (void)subdev_id;
  for (i = 0; i < LLD_TOF2_COUNT_NUMB; i++) {
    if (blk_lvl_int->count[dev_id][i] != blk_lvl_int->count_shown[dev_id][i]) {
      blk_lvl_int->count_shown[dev_id][i] = blk_lvl_int->count[dev_id][i];
    }
  }
  return BF_SUCCESS;
}

bf_status_t lld_tof2_clear_ints_cb(bf_dev_id_t dev_id,
                                   bf_subdev_id_t subdev_id,
                                   void *blk_lvl_int_vd) {
  int i;
  lld_tof2_blk_lvl_int_t *blk_lvl_int =
      (lld_tof2_blk_lvl_int_t *)blk_lvl_int_vd;
  (void)subdev_id;
  for (i = 0; i < LLD_TOF2_COUNT_NUMB; i++) {
    blk_lvl_int->count[dev_id][i] = 0;
  }
  return BF_SUCCESS;
}

static bool lld_tof2_reg_security_check(bf_dev_id_t dev_id, uint32_t reg) {
  uint32_t num_pipes, num_stages, stage;
  bf_dev_pipe_t pipe;
  bf_dev_pipe_t phy_pipe;
  lld_err_t rc = LLD_OK;
  if (lld_is_pipe_addr(dev_id, reg)) {
    lld_sku_get_num_active_pipes(dev_id, &num_pipes);
    phy_pipe = lld_get_pipe_from_addr(dev_id, reg);
    stage = lld_get_stage_from_addr(dev_id, reg);
    rc = lld_sku_map_phy_pipe_id_to_pipe_id(dev_id, phy_pipe, &pipe);
    if (rc != LLD_OK || pipe >= num_pipes) return false;
    rc = lld_sku_get_num_active_mau_stages(dev_id, &num_stages, phy_pipe);
    if (rc != LLD_OK) return false;
    if (stage >= num_stages && stage < 20) return false;
  } else if ((reg & 0x7000000) == 0x2000000) {
    /* MAC register.  Check for accesses to eth400_p1...P32. */
    int mac = (reg >> 18) & 0x3F;
    if (mac >= 1 && mac <= 32) {
      /* Ensure the MAC is on an enabled pipe. */
      phy_pipe = (mac - 1) / 8;
      lld_sku_get_num_active_pipes(dev_id, &num_pipes);
      rc = lld_sku_map_phy_pipe_id_to_pipe_id(dev_id, phy_pipe, &pipe);
      if (rc != LLD_OK || pipe >= num_pipes) return false;
    }
  }
  return true;
}

bf_status_t lld_tof2_enable_all_pipe_ints_cb(bf_dev_id_t dev_id,
                                             bf_subdev_id_t subdev_id,
                                             void *blk_lvl_int_vd) {
  lld_tof2_blk_lvl_int_t *blk_lvl_int =
      (lld_tof2_blk_lvl_int_t *)blk_lvl_int_vd;
  (void)subdev_id;
  if (!lld_tof2_reg_security_check(dev_id, blk_lvl_int->enable_hi_reg))
    return BF_SUCCESS;

  if (blk_lvl_int->enable_hi_reg != 0xffffffff) {
    lld_write_register(dev_id, blk_lvl_int->enable_hi_reg, 0xffffffff);
  }
  return BF_SUCCESS;
}

static void lld_tof2_int_handling(bf_dev_id_t dev_id,
                                  lld_tof2_blk_lvl_int_t *temp_int,
                                  int array_len);
static bf_status_t lld_tof2_read_leaf_status_cb(bf_dev_id_t dev_id,
                                         bf_subdev_id_t subdev_id,
                                         void *blk_lvl_int_vd) {
  lld_tof2_blk_lvl_int_t *blk_lvl_int =
      (lld_tof2_blk_lvl_int_t *)blk_lvl_int_vd;
  (void)subdev_id;
  if (!lld_tof2_reg_security_check(dev_id, blk_lvl_int->status_reg))
    return BF_SUCCESS;

  if (blk_lvl_int->status_reg != 0xffffffff && blk_lvl_int->status_reg != 0) {
    lld_tof2_int_handling(dev_id, blk_lvl_int, 1);
  }
  return BF_SUCCESS;
}

void lld_tof2_int_gbl_en_set(bf_dev_id_t dev_id, bool en) {
  uint32_t i;
  for (i = 0; i < tof2_reg_device_select_pcie_bar01_regs_shadow_msk_array_count;
       i++) {
    lld_write_register(
        dev_id,
        tof2_reg_device_select_pcie_bar01_regs_shadow_msk_address + (4 * i),
        en ? 0 : 0xffffffff);
  }
}

void lld_tof2_int_msk(bf_dev_id_t dev_id,
                      uint32_t sh_int_reg,
                      uint32_t sh_int_bit) {
  uint32_t bit_fld = (1u << sh_int_bit);
  uint32_t shadow_msk, shadow_msk_reg;

  shadow_msk_reg = (4 * sh_int_reg) +
                   tof2_reg_device_select_pcie_bar01_regs_shadow_msk_address;

  // mask off the interrupt
  lld_read_register(dev_id, shadow_msk_reg, &shadow_msk);
  lld_write_register(dev_id, shadow_msk_reg, (shadow_msk | bit_fld));
  return;
}
void lld_tof2_int_ena(bf_dev_id_t dev_id,
                      uint32_t sh_int_reg,
                      uint32_t sh_int_bit) {
  uint32_t bit_fld = (1u << sh_int_bit);
  uint32_t shadow_msk, shadow_msk_reg;

  shadow_msk_reg = (4 * sh_int_reg) +
                   tof2_reg_device_select_pcie_bar01_regs_shadow_msk_address;

  // un-mask the interrupt
  lld_read_register(dev_id, shadow_msk_reg, &shadow_msk);
  lld_write_register(dev_id, shadow_msk_reg, (shadow_msk & ~bit_fld));
  return;
}

static bf_status_t lld_tof2_int_recursive_find_blk(
    lld_tof2_blk_lvl_int_list_t *int_list,
    lld_tof2_blk_lvl_int_t **array,
    uint8_t *pairs,
    uint32_t offset) {
  int i, j;
  if (!int_list->is_leaf) {
    lld_tof2_blk_lvl_int_list_t *temp_list = int_list->u.blk_lvl_list;
    for (j = 0; ((j < (int)int_list->n) && (temp_list[j].reg_top != 0)); j++) {
      lld_tof2_int_recursive_find_blk(&temp_list[j], array, pairs, offset);
    }
  } else {
    for (i = 0; ((i < (int)int_list->n) &&
                 (int_list->u.blk_lvl_int[i].status_reg != 0));
         i++) {
      if (int_list->u.blk_lvl_int[i].status_reg == offset) {
        *(array + (*pairs)) = (int_list->u.blk_lvl_int + i);
        (*pairs) += 1;
        if (*pairs > 4) return BF_INVALID_ARG;
      }
    }
  }
  return BF_SUCCESS;
}

static bf_status_t lld_tof2_int_find_blk_lvl_int(lld_tof2_blk_lvl_int_t **array,
                                          uint8_t *pairs,
                                          uint32_t offset) {
  int i, j;
  int array_len;
  bf_status_t sts = BF_SUCCESS;
  if (pairs == NULL) return BF_INVALID_ARG;
  *pairs = 0;
  for (i = 0; i < 16; i++) {
    j = 0;
    array_len = (int)(sizeof(Tof2_int_top[i]) / sizeof(Tof2_int_top[i][0]));
    while ((j < array_len) && (Tof2_int_top[i][j].reg_top != 0)) {
      sts |= lld_tof2_int_recursive_find_blk(
          &Tof2_int_top[i][j], array, pairs, offset);
      j++;
    }
  }
  return sts;
}

int lld_tof2_int_register_cb(bf_dev_id_t dev_id,
                             uint32_t offset,
                             lld_int_cb cb_fn,
                             void *userdata) {
  lld_tof2_blk_lvl_int_t *blk_lvl_int[4] = {NULL, NULL, NULL, NULL};
  int overwrite = 0;
  bool set = false;
  uint8_t pairs = 0;
  uint8_t i;
  bf_status_t sts =
      lld_tof2_int_find_blk_lvl_int(&(blk_lvl_int[0]), &pairs, offset);
  if (sts != BF_SUCCESS) return -1;
  for (i = 0; i < pairs; i++) {
    if (blk_lvl_int[i] != NULL) {
      if (blk_lvl_int[i]->cb_fn[dev_id] != NULL) {
        overwrite = 1;
      }
      blk_lvl_int[i]->cb_fn[dev_id] = cb_fn;
      blk_lvl_int[i]->userdata[dev_id] = userdata;
      set = true;
    }
  }
  if (set == false) return -1;
  return overwrite;
}

/* This function only for getting the data back by configuration function
 * lld_tof2_int_register_cb.
 * For Tof2, one status register may have multiple pairs of enable registers.
 * Here only return the data for the first struct of the status register.
 */
lld_int_cb lld_tof2_get_int_cb(bf_dev_id_t dev_id,
                               uint32_t offset,
                               void **userdata) {
  lld_tof2_blk_lvl_int_t *blk_lvl_int[4] = {NULL, NULL, NULL, NULL};
  uint8_t pairs = 0;
  bf_status_t sts =
      lld_tof2_int_find_blk_lvl_int(&(blk_lvl_int[0]), &pairs, offset);
  if (sts != BF_SUCCESS) return NULL;
  if (blk_lvl_int[0] != NULL) {
    *userdata = blk_lvl_int[0]->userdata[dev_id];
    return blk_lvl_int[0]->cb_fn[dev_id];
  }
  return NULL;
}

/*int lld_tof2_get_cbus_cbc_status_offset(uint32_t *offset_p, void *userdata,
int len) {
  int actual_len =
(int)(sizeof(CBUS_CBC_0_blk_lvl_int)/sizeof(CBUS_CBC_0_blk_lvl_int[0]));
  if (len < actual_len) return -1;
  for (int i = 0; i < actual_len; i++) {
    *(offset_p + i) = CBUS_CBC_0_blk_lvl_int[i]->status_reg;
    (userdata + i) = (void *)(&CBUS_CBC_0_blk_lvl_int[i]);
  }
  return actual_len;
}
*/
static void lld_tof2_int_handling(bf_dev_id_t dev_id,
                                  lld_tof2_blk_lvl_int_t *temp_int,
                                  int array_len) {
  uint32_t reg_value;
  int k;
  int b;
  if (temp_int == NULL) return;
  for (k = 0; (k < array_len) && (temp_int[k].status_reg != 0); k++) {
    lld_read_register(dev_id, temp_int[k].status_reg, &reg_value);
    if (!reg_value) continue;

#ifndef __KERNEL__
    // check for UMAC interrupts
    if ((temp_int[k].status_reg & 0xff0300ff) == 0x020000b0) {
      // umac4 interrupt
      reg_value &= 0xFFFF;  // lower 16b are "status"
      port_mgr_tof2_port_default_handler_for_mac_ints(
          dev_id, k, temp_int[k].status_reg, reg_value, NULL);
      return;
    } else if (temp_int[k].status_reg >= 0x2000080 &&
               temp_int[k].status_reg <= 0x20000F8) {
      // umac3 interrupt (CPU port)
      port_mgr_tof2_port_default_handler_for_mac_ints(
          dev_id, -1, temp_int[k].status_reg, reg_value, NULL);
      return;
    } else if (temp_int[k].status_reg >= 0x2900080 &&
               temp_int[k].status_reg <= 0x29000F8) {
      // umac3 interrupt (rotated CPU port)
      port_mgr_tof2_port_default_handler_for_mac_ints(
          dev_id, -1, temp_int[k].status_reg, reg_value, NULL);
      return;
    }
#endif
    for (b = 0; b < 32; b++) {
      if ((1u << b) & reg_value) temp_int[k].count[dev_id][b]++;
    }
    if (temp_int[k].cb_fn[dev_id] != NULL) {
      temp_int[k].cb_fn[dev_id](dev_id,
                                0,
                                temp_int[k].status_reg,
                                reg_value,
                                temp_int[k].enable_hi_reg,
                                temp_int[k].enable_lo_reg,
                                temp_int[k].userdata[dev_id]);
    }
  }
}

static void lld_tof2_int_find_leaf(bf_dev_id_t dev_id,
                                   lld_tof2_blk_lvl_int_list_t *blk_lvl_list) {
  if (blk_lvl_list == NULL) return;

  if (!blk_lvl_list->is_leaf) {
    /* We have reached an intermediate interrupt status register, process each
     * element in its list by reading the status register and applying its mask.
     * If any bits in the mask are set then recurse to process its list. */
    int j;
    uint32_t last_reg = 0xffffffff;
    uint32_t reg_value = 0;
    lld_tof2_blk_lvl_int_list_t *element = blk_lvl_list->u.blk_lvl_list;
    for (j = 0; j < (int)blk_lvl_list->n; j++) {
      /* Skip over dummy entries. */
      if (element[j].reg_top == 0 || element[j].reg_top == 0xFFFFFFFF) continue;
      /* Multiple entries in the list may use the same status register and apply
       * different masks to it.  These are generally structured such that a
       * consecutive run of entries in the list will use the same status
       * register with different masks so here we check if this entry is using
       * the same status register as the previous and reuse the value from the
       * previous if they are.  If they use different status registers then the
       * new status register is read and the value saved. */
      if (last_reg != element[j].reg_top) {
        if (!lld_tof2_reg_security_check(dev_id, element[j].reg_top)) continue;
        lld_read_register(dev_id, element[j].reg_top, &reg_value);
        last_reg = element[j].reg_top;
      }
      if (reg_value & element[j].mask) {
        lld_tof2_int_find_leaf(dev_id, &element[j]);
      }
    }
  } else {
    /* We have reached a list of leaf interrupt status registers which must be
     * checked, process each of them. */
    lld_tof2_int_handling(
        dev_id, blk_lvl_list->u.blk_lvl_int, (int)blk_lvl_list->n);
  }
}

void lld_tof2_int_svc(bf_dev_id_t dev_id,
                      uint32_t sh_int_val,
                      uint16_t sh_int_reg) {
  lld_tof2_blk_lvl_int_list_t *temp_list;
  // only handle Tof2_int_top[sh_int_reg]
  int i = 0;
  int array_len = (int)(sizeof(Tof2_int_top[sh_int_reg]) /
                        sizeof(Tof2_int_top[sh_int_reg][0]));
  if (Tof2_int_top[sh_int_reg][0].reg_top == 0) return;
  while ((i < array_len) && (Tof2_int_top[sh_int_reg][i].reg_top != 0)) {
    if ((sh_int_val & (Tof2_int_top[sh_int_reg][i].mask)) == 0) {
      i++;
      continue;
    }
    temp_list = &Tof2_int_top[sh_int_reg][i];
    lld_tof2_int_find_leaf(dev_id, temp_list);
    i++;
  }
  return;
}

bf_status_t lld_tof2_int_poll(bf_dev_id_t dev_id, bool all_ints) {
  uint32_t glb_ints;
  uint32_t shadow_int_reg, shadow_msk_reg, sh_ints, n;
  lld_read_register(
      dev_id,
      DEF_tof2_reg_device_select_pcie_bar01_regs_glb_shadow_int_address,
      &glb_ints);

  if ((glb_ints == 0) && (all_ints == false)) return BF_SUCCESS;

  if (all_ints) {
    return lld_tof2_int_traverse_blk_lvl_int(dev_id,
                                             lld_tof2_read_leaf_status_cb);
  }

  shadow_int_reg =
      DEF_tof2_reg_device_select_pcie_bar01_regs_shadow_int_address;
  shadow_msk_reg =
      DEF_tof2_reg_device_select_pcie_bar01_regs_shadow_msk_address;

  for (n = 0; n < 16; n++) {
    if ((glb_ints & (1u << n)) == 0) continue;

    lld_write_register(dev_id,
                       shadow_msk_reg + (4 * n),
                       0xffffffff);  // mask all at this level

    lld_read_register(dev_id, shadow_int_reg + (4 * n), &sh_ints);
    lld_tof2_int_svc(dev_id, sh_ints, n);
    lld_write_register(
        dev_id, shadow_msk_reg + (4 * n), 0x0);  // un-mask all at this level
  }

  return BF_SUCCESS;
}

uint32_t lld_tof2_int_get_glb_status(bf_dev_id_t dev_id) {
  uint32_t glb_ints;

  lld_read_register(
      dev_id,
      DEF_tof2_reg_device_select_pcie_bar01_regs_glb_shadow_int_address,
      &glb_ints);

  return glb_ints;
}
uint32_t lld_tof2_int_get_shadow_int_status(bf_dev_id_t dev_id,
                                            uint16_t sh_int_reg) {
  uint32_t shadow_int, shadow_int_reg;

  shadow_int_reg =
      (4 * sh_int_reg) +
      DEF_tof2_reg_device_select_pcie_bar01_regs_shadow_int_address;

  lld_read_register(dev_id, shadow_int_reg, &shadow_int);
  return shadow_int;
}
uint32_t lld_tof2_int_get_shadow_msk_status(bf_dev_id_t dev_id,
                                            uint16_t sh_msk_reg) {
  uint32_t shadow_msk, shadow_msk_reg;

  shadow_msk_reg =
      (4 * sh_msk_reg) +
      DEF_tof2_reg_device_select_pcie_bar01_regs_shadow_msk_address;

  lld_read_register(dev_id, shadow_msk_reg, &shadow_msk);
  return shadow_msk;
}
void lld_tof2_int_set_shadow_msk_status(bf_dev_id_t dev_id,
                                        uint16_t sh_msk_reg,
                                        uint32_t value) {
  uint32_t shadow_msk_reg;

  shadow_msk_reg =
      (4 * sh_msk_reg) +
      DEF_tof2_reg_device_select_pcie_bar01_regs_shadow_msk_address;

  lld_write_register(dev_id, shadow_msk_reg, value);
  return;
}

static void lld_tof2_int_leaf_enable_set_cb(bf_dev_id_t dev_id,
                                     lld_tof2_blk_lvl_int_t *blk_lvl_int,
                                     bool en) {
  if (en == true) {
    if (blk_lvl_int->enable_hi_reg != 0xffffffff) {
      lld_write_register(dev_id, blk_lvl_int->enable_hi_reg, 0xffffffff);
    } else if (blk_lvl_int->enable_lo_reg != 0xffffffff) {
      lld_write_register(dev_id, blk_lvl_int->enable_lo_reg, 0xffffffff);
    }
  } else {
    if (blk_lvl_int->enable_hi_reg != 0xffffffff) {
      lld_write_register(dev_id, blk_lvl_int->enable_hi_reg, 0x0);
    }
    if (blk_lvl_int->enable_lo_reg != 0xffffffff) {
      lld_write_register(dev_id, blk_lvl_int->enable_lo_reg, 0x0);
    }
  }
}

static void lld_tof2_int_leaf_enable_set(bf_dev_id_t dev_id,
                                  lld_tof2_blk_lvl_int_list_t *int_list,
                                  bool en) {
  if (!int_list->is_leaf) {
    lld_tof2_blk_lvl_int_list_t *temp_list = int_list->u.blk_lvl_list;
    int j;
    for (j = 0; ((j < (int)int_list->n) && (temp_list[j].reg_top != 0)); j++) {
      lld_tof2_int_leaf_enable_set(dev_id, &temp_list[j], en);
    }
  } else {
    int i;
    for (i = 0; ((i < (int)int_list->n) &&
                 (int_list->u.blk_lvl_int[i].status_reg != 0));
         i++) {
      lld_tof2_int_leaf_enable_set_cb(
          dev_id, &(int_list->u.blk_lvl_int[i]), en);
    }
  }
}
void lld_tof2_int_host_leaf_enable_set(bf_dev_id_t dev_id, bool en) {
  int i;
  if (Tof2_int_top[0][0].reg_top == 0) return;
  for (i = 0;
       (i < (int)(sizeof(Tof2_int_top[0]) / sizeof(Tof2_int_top[0][0]))) &&
       (Tof2_int_top[0][i].reg_top != 0);
       i++) {
    lld_tof2_int_leaf_enable_set(dev_id, &(Tof2_int_top[0][i]), en);
  }
}

void lld_tof2_int_mbus_leaf_enable_set(bf_dev_id_t dev_id, bool en) {
  int array_len, i, j;
  for (j = 4; j < 8; j++) {
    if (Tof2_int_top[j][0].reg_top == 0) continue;
    i = 0;
    array_len = (int)(sizeof(Tof2_int_top[j]) / sizeof(Tof2_int_top[j][0]));
    while ((i < array_len) && (Tof2_int_top[j][i].reg_top != 0)) {
      lld_tof2_int_leaf_enable_set(dev_id, &(Tof2_int_top[j][i]), en);
    }
  }
}

bf_status_t lld_tof2_int_disable_all_cb(bf_dev_id_t dev_id,
                                        bf_subdev_id_t subdev_id,
                                        void *blk_lvl_int_vd) {
  lld_tof2_blk_lvl_int_t *blk_lvl_int =
      (lld_tof2_blk_lvl_int_t *)blk_lvl_int_vd;
  (void)subdev_id;
  if (!lld_tof2_reg_security_check(dev_id, blk_lvl_int->status_reg))
    return BF_SUCCESS;
  if (blk_lvl_int->enable_hi_reg != 0xffffffff) {
    lld_write_register(dev_id, blk_lvl_int->enable_hi_reg, 0x0);
  }
  if (blk_lvl_int->enable_lo_reg != 0xffffffff) {
    lld_write_register(dev_id, blk_lvl_int->enable_lo_reg, 0x0);
  }
  return BF_SUCCESS;
}

bf_status_t lld_tof2_inject_all_ints_cb(bf_dev_id_t dev_id,
                                        bf_subdev_id_t subdev_id,
                                        void *blk_lvl_int_vd) {
  lld_tof2_blk_lvl_int_t *blk_lvl_int =
      (lld_tof2_blk_lvl_int_t *)blk_lvl_int_vd;
  (void)subdev_id;
  if (!lld_tof2_reg_security_check(dev_id, blk_lvl_int->status_reg))
    return BF_SUCCESS;

  if (blk_lvl_int->status_inject_reg != 0xffffffff) {
    lld_write_register(dev_id, blk_lvl_int->status_inject_reg, 0xffffffff);
  }
  /*if (blk_lvl_int->ecc_inject_reg != 0xffffffff) {
    lld_write_register(dev_id, blk_lvl_int->ecc_inject_reg, 0xffffffff);
  }*/

  /*
  if (blk_lvl_int->inject_reg ==
      DEF_tof2_reg_device_select_mbc_mbc_mbus_intr_inj_address) {
    lld_write_register(dev_id, blk_lvl_int->inject_reg, 0xffff);
  } else if (blk_lvl_int->inject_reg ==
             DEF_tof2_reg_device_select_pbc_pbc_pbus_intr_inj_address) {
    lld_write_register(dev_id, blk_lvl_int->inject_reg, 0x3ffffff);
  } else if (blk_lvl_int->inject_reg ==
             DEF_tof2_reg_device_select_cbc_cbc_cbus_intr_inj_address) {
    lld_write_register(dev_id, blk_lvl_int->inject_reg, 0x3ff);
  } else if (blk_lvl_int->inject_reg ==
             DEF_tof2_reg_device_select_lfltr_ctrl_intr_inj_address) {
    lld_write_register(dev_id, blk_lvl_int->inject_reg, 0xffffc00);  // fix me
  } else {
    lld_write_register(dev_id, blk_lvl_int->inject_reg, 0xffffffff);
  }*/
  return BF_SUCCESS;
}

bf_status_t lld_tof2_inject_ints_with_offset(bf_dev_id_t dev_id,
                                             uint32_t offset) {
  int i;
  lld_tof2_blk_lvl_int_t *blk_lvl_int[4] = {NULL, NULL, NULL, NULL};
  uint8_t pairs = 0;
  bf_subdev_id_t subdev_id = 0;
  bf_status_t sts =
      lld_tof2_int_find_blk_lvl_int(&(blk_lvl_int[0]), &pairs, offset);
  if (sts != BF_SUCCESS) return sts;
  for (i = 0; i < pairs; i++) {
    if (blk_lvl_int[i] != NULL) {
      sts |= lld_tof2_inject_all_ints_cb(dev_id, subdev_id, blk_lvl_int[i]);
    }
  }
  return sts;
}

uint32_t lld_tof2_get_status_reg(void *blk_lvl_int) {
  if (blk_lvl_int != NULL) {
    return (((lld_tof2_blk_lvl_int_t *)blk_lvl_int)->status_reg);
  }
  return -1;
}

static uint32_t lld_tof2_msix_map_entry_get(bf_dev_id_t dev_id, int irq) {
  uint32_t offset, val, cur_msix = 0;

  offset = DEF_tof2_reg_device_select_pcie_bar01_regs_msix_map_address;
  if (lld_read_register(dev_id, offset + ((irq / 4) * 4), &val) != BF_SUCCESS) {
    lld_log_error("error in getting msix map entry irq %d\n", irq);
    return 0;
  }
  switch (irq % 4) {
    case 0:
      cur_msix = getp_tof2_msix_map_int_4np0(&val);
      break;
    case 1:
      cur_msix = getp_tof2_msix_map_int_4np1(&val);
      break;
    case 2:
      cur_msix = getp_tof2_msix_map_int_4np2(&val);
      break;
    case 3:
      cur_msix = getp_tof2_msix_map_int_4np3(&val);
      break;
  }
  return cur_msix;
}

static bf_status_t lld_tof2_msix_map_entry_set(bf_dev_id_t dev_id,
                                               int irq,
                                               uint32_t msix_val) {
  uint32_t offset, val = 0;
  bf_status_t status;

  offset = DEF_tof2_reg_device_select_pcie_bar01_regs_msix_map_address;
  status = lld_read_register(dev_id, offset + ((irq / 4) * 4), &val);
  if (status != BF_SUCCESS) {
    lld_log_error("error in getting msix map entry_set irq %d\n", irq);
    return status;
  }
  switch (irq % 4) {
    case 0:
      setp_tof2_msix_map_int_4np0(&val, msix_val);
      break;
    case 1:
      setp_tof2_msix_map_int_4np1(&val, msix_val);
      break;
    case 2:
      setp_tof2_msix_map_int_4np2(&val, msix_val);
      break;
    case 3:
      setp_tof2_msix_map_int_4np3(&val, msix_val);
      break;
  }
  status = lld_write_register(dev_id, offset + ((irq / 4) * 4), val);
  return status;
}

/* it is assumed that irq is disabled when calling this API */
bf_status_t lld_tof2_msix_map_set(bf_dev_id_t dev_id, int irq, int msix_num) {
  /* The irq might get mapped to msix_num to which other irq(s) might already be
     mapped. So, following steps are followed.
     1: hanlde and clear the currently mapped msix num (to irq), if any.
     2: process any pending new MSIX
     3: set msix map for irq and
  */
  uint32_t cur_msix;

  if (irq >= LLD_TOFINO_NUM_IRQ || msix_num >= LLD_TOF2_MSIX_MAX) {
    return BF_INVALID_ARG;
  }
  cur_msix = lld_tof2_msix_map_entry_get(dev_id, irq);
  if (cur_msix) {  // do not process unused  default msix index
    bf_int_msi_x_svc(dev_id, 0, (bf_msi_x_int_nbr_t)cur_msix, false);
  }
  bf_int_msi_x_svc(dev_id, 0, (bf_msi_x_int_nbr_t)msix_num, false);
  return (lld_tof2_msix_map_entry_set(dev_id, irq, msix_num));
}

#ifndef __KERNEL__
extern int dump_field_name_by_offset(bf_dev_id_t dev_id,
                                     uint32_t target_offset,
                                     int bit);
bf_status_t lld_tof2_dump_int_list_cb(bf_dev_id_t dev_id,
                                      bf_subdev_id_t subdev_id,
                                      void *blk_lvl_int_vd) {
  lld_tof2_blk_lvl_int_t *blk_lvl_int =
      (lld_tof2_blk_lvl_int_t *)blk_lvl_int_vd;
  int i;
  (void)subdev_id;
  char *path = get_full_reg_path_name(dev_id, blk_lvl_int->status_reg);
  fprintf(rl_outstream,
          "%08x : %08x : %08x : %08x : %p : %s\n",
          blk_lvl_int->status_reg,
          blk_lvl_int->enable_hi_reg,
          blk_lvl_int->enable_lo_reg,
          blk_lvl_int->shadow_mask,
          blk_lvl_int->userdata[dev_id],
          (path) ? path : "NONE");

  for (i = 0; i < LLD_TOF2_COUNT_NUMB; i++) {
    if (blk_lvl_int->count[dev_id][i] != blk_lvl_int->count_shown[dev_id][i]) {
      int rc;

      fprintf(rl_outstream,
              ": [%2d:%2d] : %d : ",
              i,
              i,
              blk_lvl_int->count[dev_id][i]);
      rc = dump_field_name_by_offset((int)dev_id, blk_lvl_int->status_reg, i);
      fprintf(rl_outstream, "\n");
      // update "shown"
      blk_lvl_int->count_shown[dev_id][i] = blk_lvl_int->count[dev_id][i];
      if (rc != 0) break;
    }
  }
  return BF_SUCCESS;
}

bf_status_t lld_tof2_dump_new_ints_cb(bf_dev_id_t dev_id,
                                      bf_subdev_id_t subdev_id,
                                      void *blk_lvl_int_vd) {
  int i, dump = 0;
  lld_tof2_blk_lvl_int_t *blk_lvl_int =
      (lld_tof2_blk_lvl_int_t *)blk_lvl_int_vd;
  (void)subdev_id;
  for (i = 0; i < LLD_TOF2_COUNT_NUMB; i++) {
    if (blk_lvl_int->count[dev_id][i] != blk_lvl_int->count_shown[dev_id][i]) {
      dump = 1;
    }
  }
  if (dump == 0) return BF_SUCCESS;

  char *path = get_full_reg_path_name(dev_id, blk_lvl_int->status_reg);
  fprintf(rl_outstream,
          "%08x : %08x : %08x : %08x : %p : %s\n",
          blk_lvl_int->status_reg,
          blk_lvl_int->enable_hi_reg,
          blk_lvl_int->enable_lo_reg,
          blk_lvl_int->shadow_mask,
          blk_lvl_int->userdata[dev_id],
          (path) ? path : "NONE");

  for (i = 0; i < LLD_TOF2_COUNT_NUMB; i++) {
    if (blk_lvl_int->count[dev_id][i] != blk_lvl_int->count_shown[dev_id][i]) {
      fprintf(
          rl_outstream,
          ": [%2d:%2d] : %d : ",
          i,
          i,
          blk_lvl_int->count[dev_id][i] - blk_lvl_int->count_shown[dev_id][i]);
      dump_field_name_by_offset((int)dev_id, blk_lvl_int->status_reg, i);
      fprintf(rl_outstream, "\n");
      // update "shown"
      blk_lvl_int->count_shown[dev_id][i] = blk_lvl_int->count[dev_id][i];
    }
  }
  return BF_INVALID_ARG;
}
bf_status_t lld_tof2_clear_new_ints_cb(bf_dev_id_t dev_id,
                                       bf_subdev_id_t subdev_id,
                                       void *blk_lvl_int_vd) {
  int i;
  lld_tof2_blk_lvl_int_t *blk_lvl_int =
      (lld_tof2_blk_lvl_int_t *)blk_lvl_int_vd;
  (void)subdev_id;

  for (i = 0; i < LLD_TOF2_COUNT_NUMB; i++) {
    blk_lvl_int->count_shown[dev_id][i] = blk_lvl_int->count[dev_id][i];
  }
  return BF_INVALID_ARG;
}

bf_status_t lld_tof2_dump_all_ints_cb(bf_dev_id_t dev_id,
                                      bf_subdev_id_t subdev_id,
                                      void *blk_lvl_int_vd) {
  int i, dump = 0;
  lld_tof2_blk_lvl_int_t *blk_lvl_int =
      (lld_tof2_blk_lvl_int_t *)blk_lvl_int_vd;
  (void)subdev_id;
  for (i = 0; i < LLD_TOF2_COUNT_NUMB; i++) {
    if (blk_lvl_int->count[dev_id][i] != 0) {
      dump = 1;
    }
  }
  if (dump == 0) return BF_SUCCESS;

  char *path = get_full_reg_path_name(dev_id, blk_lvl_int->status_reg);
  fprintf(rl_outstream,
          "%08x : %08x : %08x : %08x : %p : %s\n",
          blk_lvl_int->status_reg,
          blk_lvl_int->enable_hi_reg,
          blk_lvl_int->enable_lo_reg,
          blk_lvl_int->shadow_mask,
          blk_lvl_int->userdata[dev_id],
          (path) ? path : "NONE");

  for (i = 0; i < LLD_TOF2_COUNT_NUMB; i++) {
    if (blk_lvl_int->count[dev_id][i] != 0) {
      fprintf(rl_outstream,
              ": [%2d:%2d] : %d : ",
              i,
              i,
              blk_lvl_int->count[dev_id][i]);
      dump_field_name_by_offset((int)dev_id, blk_lvl_int->status_reg, i);
      fprintf(rl_outstream, "\n");
      // update "shown"
      blk_lvl_int->count_shown[dev_id][i] = blk_lvl_int->count[dev_id][i];
    }
  }
  return BF_INVALID_ARG;
}
bf_status_t lld_tof2_dump_unfired_ints_cb(bf_dev_id_t dev_id,
                                          bf_subdev_id_t subdev_id,
                                          void *blk_lvl_int_vd) {
  int i, dump = 0;
  lld_tof2_blk_lvl_int_t *blk_lvl_int =
      (lld_tof2_blk_lvl_int_t *)blk_lvl_int_vd;
  int n_flds = get_reg_num_fields(dev_id, blk_lvl_int->status_reg);
  (void)subdev_id;
  for (i = 0; i < n_flds; i++) {
    if (blk_lvl_int->count[dev_id][i] == 0) {
      dump = 1;
      break;
    }
  }
  if (dump == 0) return BF_SUCCESS;

  char *path = get_full_reg_path_name(dev_id, blk_lvl_int->status_reg);
  fprintf(rl_outstream,
          "%08x : %08x : %08x : %08x : %p : %s\n",
          blk_lvl_int->status_reg,
          blk_lvl_int->enable_hi_reg,
          blk_lvl_int->enable_lo_reg,
          blk_lvl_int->shadow_mask,
          blk_lvl_int->userdata[dev_id],
          (path) ? path : "NONE");
  lld_inttest_error_set(get_full_reg_path_name(dev_id, blk_lvl_int->status_reg),
                        blk_lvl_int->status_reg,
                        0xffffffff,
                        blk_lvl_int->status_reg);

  for (i = 0; i < n_flds; i++) {
    if (blk_lvl_int->count[dev_id][i] == 0) {
      fprintf(rl_outstream,
              ": [%2d:%2d] : %d : ",
              i,
              i,
              blk_lvl_int->count[dev_id][i]);
      dump_field_name_by_offset((int)dev_id, blk_lvl_int->status_reg, i);
      fprintf(rl_outstream, "\n");
    }
  }
  return BF_INVALID_ARG;
}
#endif /* __KERNEL__ */
