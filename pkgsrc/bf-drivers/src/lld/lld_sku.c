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
#include <stdbool.h>
#else
#define bf_sys_assert(x) \
  do {                   \
  } while (0)
#endif
#include <target-sys/bf_sal/bf_sys_intf.h>
#include "lld.h"
#include <lld/lld_reg_if.h>
#include "lld_map.h"
#include <lld/lld_sku.h>
#include <lld/lld_err.h>
#include <tofino_regs/tofino.h>
#include "lld_efuse.h"
#include "lld_log.h"
#include "lld_dev.h"

#define LLD_SKU_TOF3_DEVPORT_3BIT_TO_2BIT_CH(_val) (_val >> 1)
#define LLD_SKU_TOF3_DEVPORT_2BIT_TO_3BIT_CH(_val) (_val << 1)

typedef struct lld_mac2pipe_port_s {
  bf_dev_pipe_t pipe_log;
  uint8_t ibuf_phy;
} lld_mac2pipe_port_t;

/* clang-format off */
/* tables derived from MXBAR tables */
/* mac_blk to pipe_id/port_id for  Tofino-Full SKU */
static const lld_mac2pipe_port_t lld_64full_mac2parse[BF_PIPE_PORT_COUNT] = {
    {0, 15},
    {0, 14},
    {0, 13},
    {0, 12},
    {0, 11},
    {0, 10},
    {0, 9},
    {0, 8},
    {0, 7},
    {0, 6},
    {0, 5},
    {0, 4},
    {0, 3},
    {0, 2},
    {0, 1},
    {0, 0},

    {1, 0},
    {1, 1},
    {1, 2},
    {1, 3},
    {1, 4},
    {1, 5},
    {1, 6},
    {1, 7},
    {1, 8},
    {1, 9},
    {1, 10},
    {1, 11},
    {1, 12},
    {1, 13},
    {1, 14},
    {1, 15},

    {2, 15},
    {2, 14},
    {2, 13},
    {2, 12},
    {2, 11},
    {2, 10},
    {2, 9},
    {2, 8},
    {2, 7},
    {2, 6},
    {2, 5},
    {2, 4},
    {2, 3},
    {2, 2},
    {2, 1},
    {2, 0},

    {3, 0},
    {3, 1},
    {3, 2},
    {3, 3},
    {3, 4},
    {3, 5},
    {3, 6},
    {3, 7},
    {3, 8},
    {3, 9},
    {3, 10},
    {3, 11},
    {3, 12},
    {3, 13},
    {3, 14},
    {3, 15},

    {0, 16}, /* MAC 64 */
    {2, 16}  /* MAC 65 */
};

/* pipe_id/port_id to mac_blk for  Tofino-Full SKU */
static const uint8_t lld_64full_pipe_port2mac[BF_SUBDEV_PIPE_COUNT][16] = {
    {15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0},
    {16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31},
    {47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32},
    {48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63}};

/* mac_blk to pipe_id/port_id for Tofino 32Q SKU */
static const lld_mac2pipe_port_t lld_32q_mac2parse[BF_PIPE_PORT_COUNT] = {
    {0, 15},
    {1, 14},
    {0, 13},
    {1, 12},
    {0, 11},
    {1, 10},
    {0, 9},
    {1, 8},
    {0, 7},
    {1, 6},
    {0, 5},
    {1, 4},
    {0, 3},
    {1, 2},
    {0, 1},
    {1, 0},

    {0, 0},
    {1, 1},
    {0, 2},
    {1, 3},
    {0, 4},
    {1, 5},
    {0, 6},
    {1, 7},
    {0, 8},
    {1, 9},
    {0, 10},
    {1, 11},
    {0, 12},
    {1, 13},
    {0, 14},
    {1, 15},

    {2, 15},
    {3, 14},
    {2, 13},
    {3, 12},
    {2, 11},
    {3, 10},
    {2, 9},
    {3, 8},
    {2, 7},
    {3, 6},
    {2, 5},
    {3, 4},
    {2, 3},
    {3, 2},
    {2, 1},
    {3, 0},

    {2, 0},
    {3, 1},
    {2, 2},
    {3, 3},
    {2, 4},
    {3, 5},
    {2, 6},
    {3, 7},
    {2, 8},
    {3, 9},
    {2, 10},
    {3, 11},
    {2, 12},
    {3, 13},
    {2, 14},
    {3, 15},

    {0, 16}, /* MAC 64 */
    {2, 16}  /* MAC 65 */
};

/* pipe_id/port_id to mac_blk for  Tofino-32Q SKU */
static const uint8_t lld_32q_pipe_port2mac[BF_SUBDEV_PIPE_COUNT][16] = {
    {16, 14, 18, 12, 20, 10, 22, 8, 24, 6, 26, 4, 28, 2, 30, 0},
    {15, 17, 13, 19, 11, 21, 9, 23, 7, 25, 5, 27, 3, 29, 1, 31},
    {48, 46, 50, 44, 52, 42, 54, 40, 56, 38, 58, 36, 60, 34, 62, 32},
    {47, 49, 45, 51, 43, 53, 41, 55, 39, 57, 37, 59, 35, 61, 33, 63}};

/* mac_blk to pipe_id/port_id for  Tofino-Half Even SKU */
static const lld_mac2pipe_port_t lld_32half_even_mac2parse[BF_PIPE_PORT_COUNT] =
    {
     {0, 15},
     {0xff, 0xff},
     {0, 13},
     {0xff, 0xff},
     {0, 11},
     {0xff, 0xff},
     {0, 9},
     {0xff, 0xff},
     {0, 7},
     {0xff, 0xff},
     {0, 5},
     {0xff, 0xff},
     {0, 3},
     {0xff, 0xff},
     {0, 1},
     {0xff, 0xff},

     {0, 0},
     {0xff, 0xff},
     {0, 2},
     {0xff, 0xff},
     {0, 4},
     {0xff, 0xff},
     {0, 6},
     {0xff, 0xff},
     {0, 8},
     {0xff, 0xff},
     {0, 10},
     {0xff, 0xff},
     {0, 12},
     {0xff, 0xff},
     {0, 14},
     {0xff, 0xff},

     {1, 15},
     {0xff, 0xff},
     {1, 13},
     {0xff, 0xff},
     {1, 11},
     {0xff, 0xff},
     {1, 9},
     {0xff, 0xff},
     {1, 7},
     {0xff, 0xff},
     {1, 5},
     {0xff, 0xff},
     {1, 3},
     {0xff, 0xff},
     {1, 1},
     {0xff, 0xff},

     {1, 0},
     {0xff, 0xff},
     {1, 2},
     {0xff, 0xff},
     {1, 4},
     {0xff, 0xff},
     {1, 6},
     {0xff, 0xff},
     {1, 8},
     {0xff, 0xff},
     {1, 10},
     {0xff, 0xff},
     {1, 12},
     {0xff, 0xff},
     {1, 14},
     {0xff, 0xff},

     {0, 16},     /* MAC 64 */
     {0xff, 0xff} /* MAC 65 */
};

/* mac_blk to pipe_id/port_id for  Tofino-Half Odd SKU */
static const lld_mac2pipe_port_t lld_32half_odd_mac2parse[BF_PIPE_PORT_COUNT] =
    {
     {0xff, 0xff},
     {0, 14},
     {0xff, 0xff},
     {0, 12},
     {0xff, 0xff},
     {0, 10},
     {0xff, 0xff},
     {0, 8},
     {0xff, 0xff},
     {0, 6},
     {0xff, 0xff},
     {0, 4},
     {0xff, 0xff},
     {0, 2},
     {0xff, 0xff},
     {0, 0},

     {0xff, 0xff},
     {0, 1},
     {0xff, 0xff},
     {0, 3},
     {0xff, 0xff},
     {0, 5},
     {0xff, 0xff},
     {0, 7},
     {0xff, 0xff},
     {0, 9},
     {0xff, 0xff},
     {0, 11},
     {0xff, 0xff},
     {0, 13},
     {0xff, 0xff},
     {0, 15},

     {0xff, 0xff},
     {1, 14},
     {0xff, 0xff},
     {1, 12},
     {0xff, 0xff},
     {1, 10},
     {0xff, 0xff},
     {1, 8},
     {0xff, 0xff},
     {1, 6},
     {0xff, 0xff},
     {1, 4},
     {0xff, 0xff},
     {1, 2},
     {0xff, 0xff},
     {1, 0},

     {0xff, 0xff},
     {1, 1},
     {0xff, 0xff},
     {1, 3},
     {0xff, 0xff},
     {1, 5},
     {0xff, 0xff},
     {1, 7},
     {0xff, 0xff},
     {1, 9},
     {0xff, 0xff},
     {1, 11},
     {0xff, 0xff},
     {1, 13},
     {0xff, 0xff},
     {1, 15},

     {0, 16},     /* MAC 64 */
     {0xff, 0xff} /* MAC 65 */
};

/* pipe_id/port_id to mac_blk for  Tofino-Half Even SKU */
static const uint8_t lld_32half_even_pipe_port2mac[BF_SUBDEV_PIPE_COUNT][16] = {
    // mapping is the same regardless which pipe 0/1 is present
    {16, 14, 18, 12, 20, 10, 22, 8, 24, 6, 26, 4, 28, 2, 30, 0},
    {16, 14, 18, 12, 20, 10, 22, 8, 24, 6, 26, 4, 28, 2, 30, 0},
    // mapping is the same regardless which pipe 2/3 is present
    {48, 46, 50, 44, 52, 42, 54, 40, 56, 38, 58, 36, 60, 34, 62, 32},
    {48, 46, 50, 44, 52, 42, 54, 40, 56, 38, 58, 36, 60, 34, 62, 32},
};

/* pipe_id/port_id to mac_blk for  Tofino-Half Odd SKU */
static const uint8_t lld_32half_odd_pipe_port2mac[BF_SUBDEV_PIPE_COUNT][16] = {
    // mapping is the same regardless which pipe 0/1 is present
    {15, 17, 13, 19, 11, 21, 9, 23, 7, 25, 5, 27, 3, 29, 1, 31},
    {15, 17, 13, 19, 11, 21, 9, 23, 7, 25, 5, 27, 3, 29, 1, 31},
    // mapping is the same regardless which pipe 2/3 is present
    {47, 49, 45, 51, 43, 53, 41, 55, 39, 57, 37, 59, 35, 61, 33, 63},
    {47, 49, 45, 51, 43, 53, 41, 55, 39, 57, 37, 59, 35, 61, 33, 63},
};

/* mac_blk to pipe_id/port_id for Tofino2 32x400G port SKU */
/*static const lld_mac2pipe_port_t lld_tof2_mac2parse[BF_PIPE_PORT_COUNT] = {
    {0, 0},  // cpu port

    // p1-8
    {0, 1},
    {0, 2},
    {0, 3},
    {0, 4},
    {0, 5},
    {0, 6},
    {0, 7},
    {0, 8},

    // p9-16
    {1, 1},
    {1, 2},
    {1, 3},
    {1, 4},
    {1, 5},
    {1, 6},
    {1, 7},
    {1, 8},

    // p17-24
    {2, 1},
    {2, 2},
    {2, 3},
    {2, 4},
    {2, 5},
    {2, 6},
    {2, 7},
    {2, 8},

    // p25-32
    {3, 1},
    {3, 2},
    {3, 3},
    {3, 4},
    {3, 5},
    {3, 6},
    {3, 7},
    {3, 8},

    {0xff, 0xff},
    {0xff, 0xff},
    {0xff, 0xff},
    {0xff, 0xff},
    {0xff, 0xff},
    {0xff, 0xff},
    {0xff, 0xff},
    {0xff, 0xff},
    {0xff, 0xff},
    {0xff, 0xff},
    {0xff, 0xff},
    {0xff, 0xff},
    {0xff, 0xff},
    {0xff, 0xff},
    {0xff, 0xff},
    {0xff, 0xff},

    {0xff, 0xff},
    {0xff, 0xff},
    {0xff, 0xff},
    {0xff, 0xff},
    {0xff, 0xff},
    {0xff, 0xff},
    {0xff, 0xff},
    {0xff, 0xff},
    {0xff, 0xff},
    {0xff, 0xff},
    {0xff, 0xff},
    {0xff, 0xff},
    {0xff, 0xff},
    {0xff, 0xff},
    {0xff, 0xff},
    {0xff, 0xff},

    // MAC64/65, unused
    {0xff, 0xff},
    {0xff, 0xff},
};*/
/* clang-format on */

/*******************************************************************
 * lld_sku_get_sku
 *
 * Return SKU. The return value is based on the chip part number
 * read from efuse during asic discovery.
 *******************************************************************/
uint32_t lld_sku_get_sku(bf_dev_id_t dev_id) {
  uint32_t part_nbr;
  uint32_t die_config = 0;
  lld_dev_t *dev_p = lld_map_dev_id_to_dev_p_allow_unassigned(dev_id);
  if (!dev_p) return 0xFFFFFFFF;

  part_nbr = lld_efuse_get_chip_part_number(dev_id);
  die_config = lld_efuse_get_die_config(dev_id);

  switch (dev_p->dev_family) {
    case BF_DEV_FAMILY_TOFINO:
      switch (part_nbr) {
        case BFN_PART_NBR_BFNT10064Q:
          return BFN_SKU_BFN_T1_64Q;  // T-6.4-Full
        case BFN_PART_NBR_BFNT10032Q:
          return BFN_SKU_BFN_T1_32Q;  // T-3.2-Full
        case BFN_PART_NBR_BFNT10032D:
          return BFN_SKU_BFN_T1_32D;  // T-3.2-Half
        case BFN_PART_NBR_BFNT10032D_018:
          return BFN_SKU_BFN_T1_32D_018;  // T-3.2-18-Half
        case BFN_PART_NBR_BFNT10032D_020:
          return BFN_SKU_BFN_T1_32D_020;  // T-3.2-20-Half
        default:
          return BFN_SKU_BFN_T1_64Q;
      }
    case BF_DEV_FAMILY_TOFINO2:
      switch (part_nbr) {
        case BFN_PART_NBR_BFNT20080T:
          return BFN_SKU_BFN0080T;
        case BFN_PART_NBR_BFNT20080TM:
          return BFN_SKU_BFN0080TM;
        case BFN_PART_NBR_BFNT20064Q:
          return BFN_SKU_BFN0064Q;
        case BFN_PART_NBR_BFNT20064D:
          return BFN_SKU_BFN0064D;
        case BFN_PART_NBR_BFNT20128Q:
          return BFN_SKU_BFN0128Q;  // T2-12.8-Full
        case BFN_PART_NBR_BFNT20128QM:
          return BFN_SKU_BFN0128QM;  // T2-12.8-Full
        default:
          return BFN_SKU_BFN0128Q;
      }
    case BF_DEV_FAMILY_TOFINO3:
      switch (part_nbr) {
        case BFN_PART_NBR_BFNT3_56G:
          /* Single die or multi-die */
          if (die_config == 0) {
            return BFN_SKU_BFNT31_12Q;
          } else {
            return BFN_SKU_BFNT32_25Q;
          }
        case BFN_PART_NBR_BFNT3_112G:
          if (die_config == 0) {
            return BFN_SKU_BFNT31_112_12Q;
          } else {
            return BFN_SKU_BFNT32_112_25Q;
          }
        case BFN_PART_NBR_BFNT3_HYBRID:
          if (die_config == 0) {
            return BFN_SKU_BFNT31_12QH;
          } else {
            return BFN_SKU_BFNT32_25QH;
          }
        default:
          if (die_config == 0) {
            return BFN_SKU_BFNT31_12Q;
          } else {
            return BFN_SKU_BFNT32_25Q;
          }
      }









    default:
      // Unknown type What to return?
      return 0xFF;
  }
  return BFN_SKU_BFN_T1_64Q;  // T-6.4-Full
}

/*******************************************************************
 * lld_sku_get_sku_name
 *
 * Return SKU. The return value is based on the chip part number
 * read from efuse during asic discovery.
 *******************************************************************/
char *lld_sku_get_sku_name(bf_dev_id_t dev_id) {
  uint32_t part_nbr;
  uint32_t die_config = 0;

  part_nbr = lld_efuse_get_chip_part_number(dev_id);
  die_config = lld_efuse_get_die_config(dev_id);

  if (lld_dev_is_tofino(dev_id)) {
    // TOF
    switch (part_nbr) {
      case BFN_PART_NBR_BFNT10064Q:
        return "64Q";  // T-6.4-Full
      case BFN_PART_NBR_BFNT10032Q:
        return "32Q";  // T-3.2-Full
      case BFN_PART_NBR_BFNT10032D:
        return "32D";  // T-3.2-Half
      case BFN_PART_NBR_BFNT10032D_018:
        return "32D-018";  // T-3.2-18-Half
      case BFN_PART_NBR_BFNT10032D_020:
        return "32D-020";  // T-3.2-20-Half
      default:
        return "64T";  // ???
    }
  } else if (lld_dev_is_tof2(dev_id)) {
    // TOF2
    switch (part_nbr) {
      case BFN_PART_NBR_BFNT20080T:
        return "80T";
      case BFN_PART_NBR_BFNT20080TM:
        return "80TM";
      case BFN_PART_NBR_BFNT20064Q:
        return "64Q";
      case BFN_PART_NBR_BFNT20064D:
        return "64D";
      case BFN_PART_NBR_BFNT20128Q:
        return "128Q";  // T2-12.8-Full
      case BFN_PART_NBR_BFNT20128QM:
        return "128QM";  // T2-12.8-Full
      default:
        return "128Q";
    }
  } else if (lld_dev_is_tof3(dev_id)) {
    // TOF3
    switch (part_nbr) {
      case BFN_PART_NBR_BFNT3_56G:
        /* Single die or multi-die */
        if (die_config == 0) {
          return "31_12Q";
        } else {
          return "32_25";
        }
      case BFN_PART_NBR_BFNT3_112G:
        if (die_config == 0) {
          return "31_112_12Q";
        } else {
          return "32_112_25Q";
        }
      case BFN_PART_NBR_BFNT3_HYBRID:
        if (die_config == 0) {
          return "31_12QH";
        } else {
          return "32_25QH";
        }
      default:
        if (die_config == 0) {
          return "31_12Q";
        } else {
          return "32_25Q";
        }
    }
  } else {
    // Unknown type What to return?
    return "unknown";
  }
}

bf_dev_type_t lld_sku_get_subdev_type(bf_dev_id_t dev_id,
                                      bf_subdev_id_t subdev_id) {
  uint32_t part_nbr;
  uint32_t die_config = 0;
  lld_dev_t *dev_p = lld_map_dev_id_to_dev_p_allow_unassigned(dev_id);
  if (!dev_p) return 0xFFFFFFFF;

  switch (dev_p->dev_family) {
    case BF_DEV_FAMILY_TOFINO:
      part_nbr = lld_efuse_get_chip_part_number(dev_id);
      switch (part_nbr) {
        case BFN_PART_NBR_BFNT10064Q:
          return BF_DEV_BFNT10064Q;
        case BFN_PART_NBR_BFNT10032Q:
          return BF_DEV_BFNT10032Q;
        case BFN_PART_NBR_BFNT10032D:
          return BF_DEV_BFNT10032D;
        case BFN_PART_NBR_BFNT10032D_018:
          return BF_DEV_BFNT10032D018;
        case BFN_PART_NBR_BFNT10032D_020:
          return BF_DEV_BFNT10032D020;
        default:
          break;
      }
      break;
    case BF_DEV_FAMILY_TOFINO2:
      part_nbr = lld_efuse_get_chip_part_number(dev_id);
      switch (part_nbr) {
        case BFN_PART_NBR_BFNT20080T:
          return BF_DEV_BFNT20080T;
        case BFN_PART_NBR_BFNT20080TM:
          return BF_DEV_BFNT20080TM;
        case BFN_PART_NBR_BFNT20064Q:
          return BF_DEV_BFNT20064Q;
        case BFN_PART_NBR_BFNT20064D:
          return BF_DEV_BFNT20064D;
        case BFN_PART_NBR_BFNT20128Q:
          return BF_DEV_BFNT20128Q;
        case BFN_PART_NBR_BFNT20128QM:
          return BF_DEV_BFNT20128QM;
        default:
          break;
      }
      break;
    case BF_DEV_FAMILY_TOFINO3:
      die_config = lld_subdev_efuse_get_die_config(dev_id, subdev_id);
      part_nbr = lld_subdev_efuse_get_chip_part_number(dev_id, subdev_id);

      switch (part_nbr) {
        case BFN_PART_NBR_BFNT3_56G:
          /* Single die or multi-die */
          if (die_config == 0) {
            return BF_DEV_BFNT31_12Q;
          } else {
            return BF_DEV_BFNT32_25Q;
          }
        case BFN_PART_NBR_BFNT3_112G:
          if (die_config == 0) {
            return BF_DEV_BFNT31_112_12Q;
          } else {
            return BF_DEV_BFNT32_112_25Q;
          }
        case BFN_PART_NBR_BFNT3_HYBRID:
          if (die_config == 0) {
            return BF_DEV_BFNT31_12QH;
          } else {
            return BF_DEV_BFNT32_25QH;
          }
        default:
          break;
      }
      break;













    default:
      break;
  }
  return BF_DEV_UNKNOWN;
}

bf_dev_type_t lld_sku_get_dev_type(bf_dev_id_t dev_id) {
  return (lld_sku_get_subdev_type(dev_id, 0));
}

static inline bool tof2_mac_blk_is_cpu(bf_dev_id_t dev_id,
                                       uint32_t phy_mac_blk) {
  bool rotated = lld_efuse_get_die_rotated(dev_id);
  return ((phy_mac_blk == LLD_TOF2_ROTATED_PHY_CPU_MAC_ID) && rotated) ||
         ((phy_mac_blk == 0) && !rotated);
}
static inline bf_dev_port_t tof2_cpu_port(bf_dev_id_t dev_id) {
  uint32_t num_pipes = 0, cpu_pipe = 0;
  lld_err_t rc = lld_sku_get_num_active_pipes(dev_id, &num_pipes);
  if (rc != LLD_OK) return BF_INVALID_PIPE;
  if (num_pipes == 3) cpu_pipe = 1;
  return MAKE_DEV_PORT(cpu_pipe, 2);
}
static inline bf_dev_pipe_t tof2_cpu_pipe(bf_dev_id_t dev_id) {
  return DEV_PORT_TO_PIPE(tof2_cpu_port(dev_id));
}

static lld_err_t lld_tof2_sku_pipe_id_map_get(
    bf_dev_id_t dev_id, bf_dev_pipe_t log_pipe[BF_PIPE_COUNT]) {
  lld_err_t rc = LLD_OK;
  uint32_t pipe_count;
  uint32_t pipe_disable;
  uint32_t l_pipe, p_pipe, i;
  uint32_t p_pipes[4] = {0, 1, 2, 3};
  uint32_t p_pipes_rot[4] = {2, 3, 0, 1};
  bool rot;
  lld_dev_t *dev_p = lld_map_dev_id_to_dev_p_allow_unassigned(dev_id);
  if ((dev_p == NULL) || (dev_p->dev_family != BF_DEV_FAMILY_TOFINO2))
    return LLD_ERR_BAD_PARM;

  rc = lld_sku_get_num_active_pipes(dev_id, &pipe_count);
  if (rc != LLD_OK) return LLD_OK;
  pipe_disable = lld_efuse_get_pipe_disable(dev_id);
  rot = lld_efuse_get_die_rotated(dev_id);

  for (l_pipe = 0, i = 0; l_pipe < pipe_count; ++l_pipe, ++i) {
    /* Find the next enabled physical pipe. */
    for (; i < 4; ++i) {
      p_pipe = rot ? p_pipes_rot[i] : p_pipes[i];
      if (~pipe_disable & (1u << p_pipe)) break;
    }
    if (i >= 4) return LLD_ERR_BAD_PARM;
    log_pipe[l_pipe] = p_pipe;
  }
  for (; l_pipe < BF_PIPE_COUNT; ++l_pipe) log_pipe[l_pipe] = BF_INVALID_PIPE;

  if (pipe_count == 3) {
    if (rot) {
      log_pipe[0] = 3;
      log_pipe[1] = 2;
      log_pipe[2] = 1;
      log_pipe[3] = BF_INVALID_PIPE;
    } else {
      log_pipe[0] = 1;
      log_pipe[1] = 0;
      log_pipe[2] = 3;
      log_pipe[3] = BF_INVALID_PIPE;
    }
  }

  return LLD_OK;
}

static lld_err_t lld_tof3_sku_pipe_id_map_get(
    bf_dev_id_t dev_id, uint32_t log_pipe[BF_PIPE_COUNT]) {
  uint32_t pipe_disable;
  uint64_t port_lo64;
  uint32_t i, pipe_numb = 0;
  uint32_t num_subdev;
  uint32_t subdev;
  /* phy_pipe1 tracks whether all ports on a pipe are disabled. */
  bool phy_pipe1[BF_PIPE_COUNT] = {false};
  /* phy_pipe2 tracks whether individual pipes are disabled. */
  bool phy_pipe2[BF_PIPE_COUNT] = {false};
  lld_dev_t *dev_p = lld_map_dev_id_to_dev_p_allow_unassigned(dev_id);
  if ((dev_p == NULL) || (dev_p->dev_family != BF_DEV_FAMILY_TOFINO3))
    return LLD_ERR_BAD_PARM;
  if (lld_sku_get_num_subdev(dev_id, &num_subdev, NULL) != LLD_OK) {
    return LLD_ERR_BAD_PARM;
  }
  for (subdev = 0; subdev < num_subdev; subdev++) {
    lld_subdev_efuse_get_port_disable(dev_id, subdev, NULL, &port_lo64);
    pipe_disable = lld_subdev_efuse_get_pipe_disable(dev_id, subdev);
    for (i = 0; i < BF_SUBDEV_PIPE_COUNT; i++) {
      if (((port_lo64 >> (8 * i + 1)) & 0xFF) != 0xFF) {
        phy_pipe1[i + BF_SUBDEV_PIPE_COUNT * subdev] = true;
      }
      if (((pipe_disable >> i) & 0x1) == 0) {
        phy_pipe2[i + BF_SUBDEV_PIPE_COUNT * subdev] = true;
      }
    }
  }
  for (i = 0; i < num_subdev * BF_SUBDEV_PIPE_COUNT; i++) {
    if (phy_pipe1[i] && phy_pipe2[i]) {
      log_pipe[pipe_numb++] = i;
    } else if (phy_pipe2[i]) {
      lld_log("Error: no enabled ports in physical pipe %d, dev %d", i, dev_id);
    }
  }
  while (pipe_numb < num_subdev * BF_SUBDEV_PIPE_COUNT) {
    log_pipe[pipe_numb++] = 0xFFFFFFFF;
  }
  return LLD_OK;
}

/*******************************************************************
 * lld_sku_get_num_subdev
 *
 * return number of subdevices and sundevice mask (1 meaning present)
 *******************************************************************/
lld_err_t lld_sku_get_num_subdev(bf_dev_id_t dev_id,
                                 uint32_t *num_subdev,
                                 uint32_t *subdev_msk) {
  uint32_t sku_id;
  lld_dev_t *dev_p = lld_map_dev_id_to_dev_p_allow_unassigned(dev_id);
  if (dev_p == NULL) return LLD_ERR_BAD_PARM;
  if (!num_subdev) return LLD_ERR_BAD_PARM;
  switch (dev_p->dev_family) {
    case BF_DEV_FAMILY_TOFINO:
    case BF_DEV_FAMILY_TOFINO2:
      *num_subdev = 1;
      if (subdev_msk) *subdev_msk = 0x1;
      return LLD_OK;
    case BF_DEV_FAMILY_TOFINO3:
      sku_id = lld_sku_get_sku(dev_id);
      if ((sku_id == BFN_SKU_BFNT32_25Q) ||
          (sku_id == BFN_SKU_BFNT32_112_25Q) ||
          (sku_id == BFN_SKU_BFNT32_25QH)) {
        *num_subdev = 2;
        if (subdev_msk) *subdev_msk = 0x3;
      } else {
        *num_subdev = 1;
        if (subdev_msk) *subdev_msk = 0x1;
      }
      return LLD_OK;










    case BF_DEV_FAMILY_UNKNOWN:
      break;
  }
  return LLD_ERR_BAD_PARM;
}

/*******************************************************************
 * lld_sku_map_pipe_id_to_phy_pipe_id
 *
 * Map a logical pipe-id (0-n) to the physical pipe number based
 * on the SKU. Logical pipe-id's are numbered from 0-n. These may
 * be mapped to arbitrary physical pipes due to faults in the part
 * (which partially determines SKU).
 *******************************************************************/
lld_err_t lld_sku_map_pipe_id_to_phy_pipe_id(bf_dev_id_t dev_id,
                                             bf_dev_pipe_t pipe_id,
                                             bf_dev_pipe_t *phy_pipe_id) {
  lld_dev_t *dev_p = lld_map_dev_id_to_dev_p_allow_unassigned(dev_id);
  if (dev_p == NULL) return LLD_ERR_BAD_PARM;
  if (phy_pipe_id == NULL) return LLD_ERR_BAD_PARM;
  if (pipe_id >= BF_PIPE_COUNT) return LLD_ERR_BAD_PARM;
  switch (dev_p->dev_family) {
    case BF_DEV_FAMILY_TOFINO:
    case BF_DEV_FAMILY_TOFINO2:
    case BF_DEV_FAMILY_TOFINO3:

      if (dev_p->pipe_log2phy[pipe_id] != BF_INVALID_PIPE) {
        *phy_pipe_id = dev_p->pipe_log2phy[pipe_id];
        return LLD_OK;
      }
      break;
    default:
      break;
  }
  return LLD_ERR_BAD_PARM;
}

/*******************************************************************
 * lld_sku_map_phy_pipe_id_to_pipe_id
 *
 * Map a physical pipe number to the logical pipe number based
 * on the SKU. Logical pipe-id's are numbered from 0-n. These may
 * be mapped to arbitrary physical pipes due to faults in the part
 * (which partially determines SKU).
 *******************************************************************/
lld_err_t lld_sku_map_phy_pipe_id_to_pipe_id(bf_dev_id_t dev_id,
                                             bf_dev_pipe_t phy_pipe_id,
                                             bf_dev_pipe_t *pipe_id) {
  uint32_t i;
  lld_dev_t *dev_p = lld_map_dev_id_to_dev_p_allow_unassigned(dev_id);
  if (dev_p == NULL) return LLD_ERR_BAD_PARM;
  if (pipe_id == NULL) return LLD_ERR_BAD_PARM;
  if (phy_pipe_id >= BF_PIPE_COUNT) return LLD_ERR_BAD_PARM;
  *pipe_id = BF_INVALID_PIPE; /* initialize to an invalid value */
  switch (dev_p->dev_family) {
    case BF_DEV_FAMILY_TOFINO:
    case BF_DEV_FAMILY_TOFINO2:
    case BF_DEV_FAMILY_TOFINO3:
      for (i = 0; i < BF_PIPE_COUNT; i++) {
        if (dev_p->pipe_log2phy[i] == phy_pipe_id) {
          *pipe_id = i;
          return LLD_OK;
        }
      }
      break;
    default:
      break;
  }
  return LLD_ERR_BAD_PARM;
}

/*******************************************************************
 * lld_sku_map_mac_to_pipe_and_ibuf
 *
 * Return the physical pipe-id and ibuf/prsr to which the given MAC block
 * is connected.
 *******************************************************************/
static lld_err_t lld_tof_sku_map_mac_to_pipe_and_ibuf(bf_dev_id_t dev_id,
                                                      uint32_t mac_blk,
                                                      bf_dev_pipe_t *pipe_id,
                                                      uint32_t *ibuf) {
  uint32_t part_sku;
  const lld_mac2pipe_port_t *mac_pipe_port_map;
  uint64_t port_lo64, port_hi64;
  lld_dev_t *dev_p = lld_map_dev_id_to_dev_p_allow_unassigned(dev_id);
  if ((dev_p == NULL) || (dev_p->dev_family != BF_DEV_FAMILY_TOFINO))
    return LLD_ERR_BAD_PARM;
  if (pipe_id == NULL) return LLD_ERR_BAD_PARM;
  if (ibuf == NULL) return LLD_ERR_BAD_PARM;
  if (mac_blk > 65) return LLD_ERR_BAD_PARM;
  lld_efuse_get_port_disable(dev_id, &port_hi64, &port_lo64);
  part_sku = lld_sku_get_sku(dev_id);
  switch (part_sku) {
    case BFN_SKU_BFN_T1_64Q:
      mac_pipe_port_map = &lld_64full_mac2parse[mac_blk];
      break;
    case BFN_SKU_BFN_T1_32Q:
      mac_pipe_port_map = &lld_32q_mac2parse[mac_blk];
      break;
    case BFN_SKU_BFN_T1_32D:
    case BFN_SKU_BFN_T1_32D_018:
    case BFN_SKU_BFN_T1_32D_020:
      if (port_lo64 & 0x1ULL) {
        /* this is "half-odd" configuration */
        mac_pipe_port_map = &lld_32half_odd_mac2parse[mac_blk];
      } else {
        /* this is "half-even" configuration */
        mac_pipe_port_map = &lld_32half_even_mac2parse[mac_blk];
      }
      break;
    default:
      lld_log("Bad chip %d Sku %d", dev_id, part_sku);
      return LLD_ERR_BAD_PARM;
  }
  if (mac_pipe_port_map->pipe_log == 0xff) return LLD_ERR_BAD_PARM;

  /* For chips smaller than 32half's need to make sure the
   * specific port has not been fused out. */
  if (mac_blk < 64) {
    if (((port_lo64 >> mac_blk) & 1ull) == 1ull) {
      return LLD_ERR_BAD_PARM;
    }
  } else {
    if (((port_hi64 >> (mac_blk - 64)) & 1ull) == 1ull) {
      return LLD_ERR_BAD_PARM;
    }
  }

  /* IMPORTANT: logical pipe ID is returned */
  *pipe_id = mac_pipe_port_map->pipe_log;
  *ibuf = mac_pipe_port_map->ibuf_phy;
  return LLD_OK;
}
/*******************************************************************
 * lld_tof2_sku_map_mac_to_pipe_and_ibuf
 *
 * Maps a physical MAC block to a logical pipe and ibuf within that pipe.
 *******************************************************************/
static lld_err_t lld_tof2_sku_map_mac_to_pipe_and_ibuf(bf_dev_id_t dev_id,
                                                       uint32_t mac_blk,
                                                       bf_dev_pipe_t *pipe_id,
                                                       uint32_t *ibuf) {
  bf_dev_pipe_t phy_pipe;
  uint32_t sku;
  lld_err_t rc;
  lld_dev_t *dev_p = lld_map_dev_id_to_dev_p_allow_unassigned(dev_id);
  if ((dev_p == NULL) || (dev_p->dev_family != BF_DEV_FAMILY_TOFINO2))
    return LLD_ERR_BAD_PARM;
  /* Special case the CPU port. */
  if (tof2_mac_blk_is_cpu(dev_id, mac_blk)) {
    *pipe_id = tof2_cpu_pipe(dev_id);
    *ibuf = 0;
    return LLD_OK;
  }
  /* Non-CPU MAC blocks are in the range 1..32. */
  if (mac_blk == 0 || mac_blk > 32) return LLD_ERR_BAD_PARM;

  /* The mac_blk is physical so it converts to a physical pipe id which then
   * maps back to the logical pipe id. */
  phy_pipe = (mac_blk - 1) / 8;
  rc = lld_sku_map_phy_pipe_id_to_pipe_id(dev_id, phy_pipe, pipe_id);
  if (rc != LLD_OK) return rc;
  /* The 8 MACs per pipe map to ibufs 1-8; ibuf 0 is CPU/Recirc. */
  *ibuf = 1 + ((mac_blk - 1) % 8);

  /* We've mapped the mac_blk back to a logical pipe and ibuf in that pipe
   * however the mac_blk may be disabled by the SKU.  Ports are disabled based
   * on SKU so check per SKU if the logical pipe and ibuf are enabled or
   * disabled. */
  sku = lld_sku_get_sku(dev_id);
  switch (sku) {
    case BFN_SKU_BFN0080T:
    case BFN_SKU_BFN0080TM:
      /* 80T disables the following ports:
       *   Logical/Physical Pipe 0: local port 32-39, MAC 4,  IBuf 4
       *   Logical/Physical Pipe 1: local port 32-39, MAC 12, IBuf 4
       *   Logical Pipe 2/Physical Pipe 3: local port 32-39, MAC 20, IBuf 4
       *   Logical Pipe 2/Physical Pipe 3: local port 64-71, MAC 24, IBuf 8
       * For a rotated die this would be:
       *   Physical Pipe 2: local port 32-39, MAC 20, IBuf 4
       *   Physical Pipe 3: local port 32-39, MAC 28, IBuf 4
       *   Physical Pipe 1: local port 32-39, MAC 4,  IBuf 4
       *   Physical Pipe 1: local port 64-71, MAC 8,  IBuf 8
       * */
      if (*ibuf == 4) return LLD_ERR_BAD_PARM;
      if (*pipe_id == 2 && *ibuf == 8) return LLD_ERR_BAD_PARM;
      break;
    default:
      break;
  }
  return LLD_OK;
}
static lld_err_t lld_tof3_sku_map_mac_to_pipe_and_ibuf(bf_dev_id_t dev_id,
                                                       uint32_t mac_blk,
                                                       uint32_t *pipe_id,
                                                       uint32_t *ibuf) {
  uint32_t pipe_flag[BF_PIPE_COUNT];
  uint32_t i;
  lld_dev_t *dev_p = lld_map_dev_id_to_dev_p_allow_unassigned(dev_id);

  if ((dev_p == NULL) || (dev_p->dev_family != BF_DEV_FAMILY_TOFINO3))
    return LLD_ERR_BAD_PARM;

  if (((mac_blk == LLD_TOF3_ROTATED_PHY_CPU_MAC_ID) &&
       lld_efuse_get_die_rotated(dev_id)) ||
      ((mac_blk == 0) && !lld_efuse_get_die_rotated(dev_id))) {
    *pipe_id = 0;
    *ibuf = 0;
    return LLD_OK;
  }

  if (mac_blk > 65) return LLD_ERR_BAD_PARM;

  if (lld_tof3_sku_pipe_id_map_get(dev_id, pipe_flag) != LLD_OK)
    return LLD_ERR_BAD_PARM;
  for (i = 0; i < BF_PIPE_COUNT; i++) {
    if (pipe_flag[i] == ((mac_blk - 1) / 8)) {
      *pipe_id = i;
      /*  The 8 MACs per pipe map to ibufs 1-8; ibuf 0 is CPU/Recirc. */
      *ibuf = 1 + ((mac_blk - 1) % 8);

      return LLD_OK;
    }
  }
  return LLD_ERR_BAD_PARM;
}

/*******************************************************************
 * lld_sku_map_mac_ch_to_dev_port_id
 *
 * Return the dev_port_id_t value associated with the given physical MAC block
 * and channel, both in physical values dev_port_id contains logical pipe_id
 *bits into it.
 *******************************************************************/
lld_err_t lld_sku_map_mac_ch_to_dev_port_id(bf_dev_id_t dev_id,
                                            uint32_t mac_blk,
                                            uint32_t ch,
                                            bf_dev_port_t *dev_port_id) {
  lld_dev_t *dev_p = lld_map_dev_id_to_dev_p_allow_unassigned(dev_id);
  uint32_t ibuf;
  bf_dev_pipe_t pipe_log;
  lld_err_t rc;

  if (dev_p == NULL) {
    lld_log("Error in getting lld_dev, dev %d", dev_id);
    return LLD_ERR_BAD_PARM;
  }
  if (dev_port_id == NULL) {
    lld_log("Invalid dev_port_id, dev %d", dev_id);
    return LLD_ERR_BAD_PARM;
  }
  switch (dev_p->dev_family) {
    case BF_DEV_FAMILY_TOFINO:
      if (mac_blk > 64) return LLD_ERR_BAD_PARM;
      if (ch > 3) return LLD_ERR_BAD_PARM;
      // special-case for CPU MAC
      if (mac_blk == 64) {
        *dev_port_id = 64 + ch;
        return LLD_OK;
      }
      rc = lld_tof_sku_map_mac_to_pipe_and_ibuf(
          dev_id, mac_blk, &pipe_log, &ibuf);
      if (rc != LLD_OK) {
        return rc;
      }
      *dev_port_id = (pipe_log << 7) | (ibuf * 4 + ch);
      break;
    case BF_DEV_FAMILY_TOFINO2:
      // special-case for CPU MAC
      if (tof2_mac_blk_is_cpu(dev_id, mac_blk)) {
        if (ch > 3) {
          lld_log("Invalid channel id %d for cpu mac 0, should be 0-3. dev %d",
                  ch,
                  dev_id);
          return LLD_ERR_BAD_PARM;
        }
        *dev_port_id = tof2_cpu_port(dev_id) + ch;
        return LLD_OK;
      }
      if (mac_blk == 0 || mac_blk > 32) {
        lld_log("Invalid mac_blk %d, should be 1-32. dev %d", mac_blk, dev_id);
        return LLD_ERR_BAD_PARM;
      }
      if (ch > 7) {
        lld_log("Invalid channel id %d, should be 0-7. dev %d", ch, dev_id);
        return LLD_ERR_BAD_PARM;
      }
      rc = lld_tof2_sku_map_mac_to_pipe_and_ibuf(
          dev_id, mac_blk, &pipe_log, &ibuf);
      if (rc != LLD_OK) {
        lld_log("Fail to get pipe log and ibuf, dev %d", dev_id);
        return rc;
      }
      *dev_port_id = (pipe_log << 7) | (ibuf * 8 + ch);
      break;
    case BF_DEV_FAMILY_TOFINO3:
      // special-case for CPU MAC
      if (((mac_blk == LLD_TOF3_ROTATED_PHY_CPU_MAC_ID) &&
           lld_efuse_get_die_rotated(dev_id)) ||
          ((mac_blk == 0) && !lld_efuse_get_die_rotated(dev_id))) {
        if (ch > 1) {
          lld_log("Invalid channel id %d for cpu mac 0, should be 0-1. dev %d",
                  ch,
                  dev_id);
          return LLD_ERR_BAD_PARM;
        }
        *dev_port_id = (ch << 1) + 2;
        return LLD_OK;
      }
      // add subdev-id check and max mac_blk
      if (mac_blk > 65) {
        lld_log("Invalid mac_blk %d, should be 0-65. dev %d", mac_blk, dev_id);
        return LLD_ERR_BAD_PARM;
      }
      if (ch > 3) {
        lld_log("Invalid channel id %d, should be 0-3. dev %d", ch, dev_id);
        return LLD_ERR_BAD_PARM;
      }
      rc = lld_tof3_sku_map_mac_to_pipe_and_ibuf(
          dev_id, mac_blk, &pipe_log, &ibuf);
      if (rc != LLD_OK) {
        lld_log("Fail to get pipe log and ibuf, dev %d", dev_id);
        return rc;
      }
      *dev_port_id = (pipe_log << 7) | ((ibuf * 8) + (ch << 1));
#if 0
      lld_log("--- dev %d mac_blk %d ch %d pipe %d ibuf %d dev-port %d",
              dev_id,
              mac_blk,
              ch,
              pipe_log,
              ibuf,
              *dev_port_id);
#endif
      break;
    default:
      lld_log("Invalid dev family %d, dev %d", dev_p->dev_family, dev_id);
      return LLD_ERR_BAD_PARM;
  }
  return LLD_OK;
}

static lld_err_t lld_tof_sku_map_dev_port_id_to_mac_ch(bf_dev_id_t dev_id,
                                                       uint32_t dev_port_id,
                                                       uint32_t *mac_blk,
                                                       uint32_t *ch) {
  uint64_t port_lo64, port_hi64;
  uint32_t port;
  bf_dev_pipe_t pipe_phy;
  bf_dev_pipe_t pipe_log;
  lld_err_t rc;
  uint32_t part_sku;
  uint32_t num_subdev = 0;

  pipe_log = (dev_port_id >> 7);
  lld_sku_get_num_subdev(dev_id, &num_subdev, NULL);
  if (pipe_log > (BF_SUBDEV_PIPE_COUNT * num_subdev)) {
    return LLD_ERR_BAD_PARM;
  }

  rc = lld_sku_map_pipe_id_to_phy_pipe_id(dev_id, pipe_log, &pipe_phy);
  if (rc != LLD_OK) {
    return rc;
  }

  port = dev_port_id & 0x7F;
  if ((port / 4) > 16) { /* ports 64,65,66 and 67 are CPU MAC ports */
    return LLD_ERR_BAD_PARM;
  }

  if ((port / 4) == 16) {
    if (pipe_log == 0) {
      // special-case of CPU MAC
      *mac_blk = 64;
      *ch = port % 4;
      return LLD_OK;
    } else {
      return LLD_ERR_BAD_PARM;
    }
  }

  *ch = port % 4;
  port = port / 4;

  lld_efuse_get_port_disable(dev_id, &port_hi64, &port_lo64);
  part_sku = lld_sku_get_sku(dev_id);
  switch (part_sku) {
    case BFN_SKU_BFN_T1_64Q:
      *mac_blk = lld_64full_pipe_port2mac[pipe_phy][port];
      break;
    case BFN_SKU_BFN_T1_32Q:
      *mac_blk = lld_32q_pipe_port2mac[pipe_phy][port];
      break;
    case BFN_SKU_BFN_T1_32D:
    case BFN_SKU_BFN_T1_32D_018:
    case BFN_SKU_BFN_T1_32D_020:
      if (port_lo64 & 0x1ULL) {
        /* this is "half-odd" configuration */
        *mac_blk = lld_32half_odd_pipe_port2mac[pipe_phy][port];
      } else {
        /* this is "half-even" configuration */
        *mac_blk = lld_32half_even_pipe_port2mac[pipe_phy][port];
      }
      break;
    default:
      lld_log("Bad chip %d Sku %d", dev_id, part_sku);
      return LLD_ERR_BAD_PARM;
  }

  /* For chips smaller than 32half's need to make sure the
   * specific port has not been fused out. */
  if (*mac_blk < 64) {
    if (((port_lo64 >> *mac_blk) & 1ull) == 1ull) {
      return LLD_ERR_BAD_PARM;
    }
  } else {
    if (((port_hi64 >> (*mac_blk - 64)) & 1ull) == 1ull) {
      return LLD_ERR_BAD_PARM;
    }
  }

  return LLD_OK;
}
static lld_err_t lld_tof2_sku_map_dev_port_id_to_mac_ch(bf_dev_id_t dev_id,
                                                        uint32_t dev_port_id,
                                                        uint32_t *phy_mac_blk,
                                                        uint32_t *ch) {
  bf_dev_pipe_t pipe_log = DEV_PORT_TO_PIPE(dev_port_id);
  bf_dev_pipe_t pipe_phy = pipe_log;
  uint32_t port = DEV_PORT_TO_LOCAL_PORT(dev_port_id);
  uint32_t cpu_pipe = tof2_cpu_pipe(dev_id);
  /* The devport has a logical pipe-id, make sure it is a legal value. */
  uint32_t num_pipes = 0;
  lld_err_t rc = lld_sku_get_num_active_pipes(dev_id, &num_pipes);
  if (LLD_OK != rc) return rc;
  if (pipe_log >= num_pipes) return LLD_ERR_BAD_PARM;
  if (port > 71) return LLD_ERR_BAD_PARM;
  if ((port >= 2) && (port <= 5) && (pipe_log == cpu_pipe)) {
    bool rotated = lld_efuse_get_die_rotated(dev_id);
    if (rotated)
      *phy_mac_blk = LLD_TOF2_ROTATED_PHY_CPU_MAC_ID;
    else
      *phy_mac_blk = 0;
    *ch = port - 2;
    return LLD_OK;
  }
  if (port < 8) return LLD_ERR_BAD_PARM;  // recir port

  rc = lld_sku_map_pipe_id_to_phy_pipe_id(dev_id, pipe_log, &pipe_phy);
  if (LLD_OK != rc) return rc;

  *phy_mac_blk = (port - 8) / 8 + (pipe_phy * 8) + 1;
  *ch = (port - 8) % 8;
  return LLD_OK;
}
static lld_err_t lld_tof3_sku_map_dev_port_id_to_mac_ch(bf_dev_id_t dev_id,
                                                        uint32_t dev_port_id,
                                                        uint32_t *mac_blk,
                                                        uint32_t *ch) {
  uint32_t pipe_log = DEV_PORT_TO_PIPE(dev_port_id);
  uint32_t port = DEV_PORT_TO_LOCAL_PORT(dev_port_id);
  if ((!mac_blk) || (!ch) || (pipe_log > 7)) return LLD_ERR_BAD_PARM;
  if (port > 71) return LLD_ERR_BAD_PARM;

  if (!(port % 2 == 0)) return LLD_ERR_BAD_PARM;

  if ((port >= 2) && (port <= 4) && (pipe_log == 0)) {
    // no rotated die.
    *mac_blk = 0;
    *ch = (port >> 1) / 2;
    return LLD_OK;
  }
  if (port < 8) return LLD_ERR_BAD_PARM;  // recir port

  *mac_blk = (port - 8) / 8 + (pipe_log * 8) + 1;
  *ch = ((port - 8) % 8) >> 1;
  (void)dev_id;
  return LLD_OK;
}
/*******************************************************************
 * lld_sku_map_dev_port_id_to_mac_ch
 *
 * Return the MAC block and channel associated with the given
 * dev_port_id_t value.
 * dev_port_id_t contains logicalpipe_id into it.
 * mac_blk and ch are physical.
 *******************************************************************/
lld_err_t lld_sku_map_dev_port_id_to_mac_ch(bf_dev_id_t dev_id,
                                            uint32_t dev_port_id,
                                            uint32_t *phy_mac_blk,
                                            uint32_t *ch) {
  lld_dev_t *dev_p = lld_map_dev_id_to_dev_p_allow_unassigned(dev_id);
  lld_err_t sts;
  if (dev_p == NULL) return LLD_ERR_BAD_PARM;
  if (phy_mac_blk == NULL) return LLD_ERR_BAD_PARM;
  if (ch == NULL) return LLD_ERR_BAD_PARM;
  switch (dev_p->dev_family) {
    case BF_DEV_FAMILY_TOFINO:
      sts = lld_tof_sku_map_dev_port_id_to_mac_ch(
          dev_id, dev_port_id, phy_mac_blk, ch);
      break;
    case BF_DEV_FAMILY_TOFINO2:
      sts = lld_tof2_sku_map_dev_port_id_to_mac_ch(
          dev_id, dev_port_id, phy_mac_blk, ch);
      break;
    case BF_DEV_FAMILY_TOFINO3:
      sts = lld_tof3_sku_map_dev_port_id_to_mac_ch(
          dev_id, dev_port_id, phy_mac_blk, ch);
      break;
    default:
      return LLD_ERR_BAD_PARM;
  }
  return sts;
}

static lld_err_t lld_tof_sku_map_log2phy_mac_block(bf_dev_id_t dev_id,
                                                   uint32_t log_mac_block,
                                                   uint32_t *phy_mac_block) {
  uint64_t port_lo64, port_hi64;
  uint32_t part_sku;

  lld_efuse_get_port_disable(dev_id, &port_hi64, &port_lo64);
  part_sku = lld_sku_get_sku(dev_id);
  switch (part_sku) {
    case BFN_SKU_BFN_T1_64Q:
    case BFN_SKU_BFN_T1_32Q:
      *phy_mac_block = log_mac_block;  // identity mapping
      break;
    case BFN_SKU_BFN_T1_32D:
    case BFN_SKU_BFN_T1_32D_018:
    case BFN_SKU_BFN_T1_32D_020:
      if (port_lo64 & 0x1ULL) {
        /* this is "half-odd" configuration */
        if (log_mac_block == 32) {
          *phy_mac_block = 64;
        } else {
          *phy_mac_block = 2 * log_mac_block + 1;
        }
      } else {
        /* this is "half-even" configuration */
        *phy_mac_block = 2 * log_mac_block;
      }
      break;
    default:
      lld_log("Bad chip %d Sku %d", dev_id, part_sku);
      return LLD_ERR_BAD_PARM;
  }
  return LLD_OK;
}
static lld_err_t lld_tof2_sku_map_log2phy_mac_block(bf_dev_id_t dev_id,
                                                    uint32_t log_mac_block,
                                                    uint32_t *phy_mac_block) {
  bool rotated = lld_efuse_get_die_rotated(dev_id);
  if (log_mac_block > 32) {
    lld_log("Error logical mac block %d, valid value:0-32. dev %d",
            log_mac_block,
            dev_id);
    return LLD_ERR_BAD_PARM;
  }

  /* Special case the CPU MAC block. */
  if (log_mac_block == 0) {
    *phy_mac_block = rotated ? LLD_TOF2_ROTATED_PHY_CPU_MAC_ID : log_mac_block;
    return LLD_OK;
  }

  /* The logical to physical MAC mapping depends only upon die rotation.  In the
   * non-rotated case the two are the same.  In the rotated case there is a two
   * pipe offset. */
  if (rotated)
    *phy_mac_block = ((log_mac_block - 1 + 16) % 32) + 1;
  else
    *phy_mac_block = log_mac_block;

  return LLD_OK;
}

static lld_err_t lld_tof3_sku_map_log2phy_mac_block(bf_dev_id_t dev_id,
                                                    uint32_t log_mac_block,
                                                    uint32_t *phy_mac_block) {
  uint32_t pipe_log[BF_PIPE_COUNT];
  uint32_t pipe;

  if (log_mac_block == 0) {
    // CPU MAC
    if (lld_efuse_get_die_rotated(dev_id))
      *phy_mac_block = LLD_TOF3_ROTATED_PHY_CPU_MAC_ID;
    else
      *phy_mac_block = 0;
    return LLD_OK;
  }
  pipe = (log_mac_block - 1) / 8;
  if (lld_tof3_sku_pipe_id_map_get(dev_id, pipe_log) != LLD_OK) {
    lld_log("Cannot get the pipe_id map from sku, dev %d", dev_id);
    return LLD_ERR_BAD_PARM;
  }
  if (pipe_log[pipe] == 0xFFFFFFFF) {
    lld_log("Log pipe id %d does not have a phy pipe, dev %d", pipe, dev_id);
    return LLD_ERR_BAD_PARM;
  }
  *phy_mac_block = log_mac_block;
  return LLD_OK;
}
/*******************************************************************
 * lld_sku_map_log2phy_mac_block
 *
 * Return the physical mac_block associated with the given
 * logical mac_block.
 *******************************************************************/
static lld_err_t lld_tof_sku_map_phy2log_mac_block(bf_dev_id_t dev_id,
                                                   uint32_t phy_mac_block,
                                                   uint32_t *log_mac_block) {
  uint64_t port_lo64, port_hi64;
  uint32_t part_sku;

  lld_efuse_get_port_disable(dev_id, &port_hi64, &port_lo64);
  part_sku = lld_sku_get_sku(dev_id);
  switch (part_sku) {
    case BFN_SKU_BFN_T1_64Q:
    case BFN_SKU_BFN_T1_32Q:
      *log_mac_block = phy_mac_block;  // identity mapping
      break;
    case BFN_SKU_BFN_T1_32D:
    case BFN_SKU_BFN_T1_32D_018:
    case BFN_SKU_BFN_T1_32D_020:
      if (port_lo64 & 0x1ULL) {
        /* this is "half-odd" configuration */
        if (phy_mac_block == 64) {
          *log_mac_block = 32;
        } else {
          *log_mac_block = (phy_mac_block - 1) / 2;
        }
      } else {
        /* this is "half-even" configuration */
        *log_mac_block = phy_mac_block / 2;
      }
      break;
    default:
      lld_log("Bad chip %d Sku %d", dev_id, part_sku);
      return LLD_ERR_BAD_PARM;
  }
  return LLD_OK;
}

static lld_err_t lld_tof2_sku_map_phy2log_mac_block(bf_dev_id_t dev_id,
                                                    uint32_t phy_mac_block,
                                                    uint32_t *log_mac_block) {
  bf_dev_pipe_t log_pipe, phy_pipe;
  lld_err_t rc;
  uint32_t mac_offset_in_pipe;

  /* Special case the CPU MAC block. */
  if (phy_mac_block == 0) {
    if (lld_efuse_get_die_rotated(dev_id)) {
      return LLD_ERR_BAD_PARM;
    }
    *log_mac_block = 0;
    return LLD_OK;
  }
  if (phy_mac_block == LLD_TOF2_ROTATED_PHY_CPU_MAC_ID) {
    if (!lld_efuse_get_die_rotated(dev_id)) {
      return LLD_ERR_BAD_PARM;
    }
    *log_mac_block = 0;
    return LLD_OK;
  }

  if (phy_mac_block > 32) return LLD_ERR_BAD_PARM;

  phy_pipe = (phy_mac_block - 1) / 8;
  rc = lld_sku_map_phy_pipe_id_to_pipe_id(dev_id, phy_pipe, &log_pipe);
  if (rc != LLD_OK) return rc;

  mac_offset_in_pipe = (phy_mac_block - 1) % 8;
  *log_mac_block = (log_pipe * 8) + mac_offset_in_pipe + 1;

  return LLD_OK;
}

static lld_err_t lld_tof3_sku_map_phy2log_mac_block(bf_dev_id_t dev_id,
                                                    uint32_t phy_mac_block,
                                                    uint32_t *log_mac_block) {
  uint32_t pipe_log[BF_PIPE_COUNT];
  uint32_t pipe = (phy_mac_block - 1) / 8;
  uint32_t i;
  if (((phy_mac_block == 0) && !lld_efuse_get_die_rotated(dev_id)) ||
      ((phy_mac_block == LLD_TOF3_ROTATED_PHY_CPU_MAC_ID) &&
       lld_efuse_get_die_rotated(dev_id))) {
    *log_mac_block = 0;
    return LLD_OK;
  }

  if (phy_mac_block > 32) return LLD_ERR_BAD_PARM;

  if (lld_tof3_sku_pipe_id_map_get(dev_id, pipe_log) != LLD_OK) {
    return LLD_ERR_BAD_PARM;
  }
  for (i = 0; i < BF_PIPE_COUNT; i++) {
    if (pipe_log[i] == pipe) {
      // *log_mac_block = (8 * i + 1) + ((phy_mac_block - 1) % 4) + 4;
      // Revisit later
      *log_mac_block = phy_mac_block;
      return LLD_OK;
    }
  }
  return LLD_ERR_BAD_PARM;
}

lld_err_t lld_sku_map_log2phy_mac_block(bf_dev_id_t dev_id,
                                        uint32_t log_mac_block,
                                        uint32_t *phy_mac_block) {
  lld_dev_t *dev_p = lld_map_dev_id_to_dev_p_allow_unassigned(dev_id);

  if (dev_p == NULL) {
    lld_log("Invalid dev_p, dev %d", dev_id);
    return LLD_ERR_BAD_PARM;
  }
  if (phy_mac_block == NULL) {
    lld_log("Invalid phy_mac_block, dev %d", dev_id);
    return LLD_ERR_BAD_PARM;
  }

  switch (dev_p->dev_family) {
    case BF_DEV_FAMILY_TOFINO:
      return lld_tof_sku_map_log2phy_mac_block(
          dev_id, log_mac_block, phy_mac_block);
    case BF_DEV_FAMILY_TOFINO2:
      return lld_tof2_sku_map_log2phy_mac_block(
          dev_id, log_mac_block, phy_mac_block);
    case BF_DEV_FAMILY_TOFINO3:
      return lld_tof3_sku_map_log2phy_mac_block(
          dev_id, log_mac_block, phy_mac_block);
    default:
      lld_log("Invalid dev family, dev %d", dev_id);
      return LLD_ERR_BAD_PARM;
  }
}

/*******************************************************************
 * lld_sku_map_phy2log_mac_block
 *
 * Return the logical mac_block associated with the given
 * physical mac_block.
 *******************************************************************/
lld_err_t lld_sku_map_phy2log_mac_block(bf_dev_id_t dev_id,
                                        uint32_t phy_mac_block,
                                        uint32_t *log_mac_block) {
  lld_dev_t *dev_p = lld_map_dev_id_to_dev_p_allow_unassigned(dev_id);

  if (dev_p == NULL) return LLD_ERR_BAD_PARM;
  if (log_mac_block == NULL) return LLD_ERR_BAD_PARM;

  switch (dev_p->dev_family) {
    case BF_DEV_FAMILY_TOFINO:
      return lld_tof_sku_map_phy2log_mac_block(
          dev_id, phy_mac_block, log_mac_block);
    case BF_DEV_FAMILY_TOFINO2:
      return lld_tof2_sku_map_phy2log_mac_block(
          dev_id, phy_mac_block, log_mac_block);
    case BF_DEV_FAMILY_TOFINO3:
      return lld_tof3_sku_map_phy2log_mac_block(
          dev_id, phy_mac_block, log_mac_block);
    default:
      return LLD_ERR_BAD_PARM;
  }
}

/*******************************************************************
 * lld_sku_get_num_active_pipes
 *
 * Get num of active pipes on this dev_id based on SKU
 *******************************************************************/
lld_err_t lld_sku_get_num_active_pipes(bf_dev_id_t dev_id,
                                       uint32_t *num_pipes) {
  uint32_t pipe_disable, i;
  uint32_t subdev_cnt = 0;
  lld_dev_t *dev_p = lld_map_dev_id_to_dev_p_allow_unassigned(dev_id);

  if (dev_p == NULL) {
    return LLD_ERR_BAD_PARM;
  }
  *num_pipes = 0;
  switch (dev_p->dev_family) {
    case BF_DEV_FAMILY_TOFINO3:

      lld_sku_get_num_subdev(dev_id, &subdev_cnt, NULL);
      pipe_disable = lld_subdev_efuse_get_pipe_disable(dev_id, 0);
      for (i = 0; i < BF_SUBDEV_PIPE_COUNT; i++) {
        if (!(pipe_disable & 0x1)) {
          (*num_pipes)++;
        }
        pipe_disable >>= 1;
      }
      if (subdev_cnt > 1) {  // assume maximum two subdev
        pipe_disable = lld_subdev_efuse_get_pipe_disable(dev_id, 1);
        for (i = 0; i < BF_SUBDEV_PIPE_COUNT; i++) {
          if (!(pipe_disable & 0x1)) {
            (*num_pipes)++;
          }
          pipe_disable >>= 1;
        }
      }
      break;
    case BF_DEV_FAMILY_TOFINO:
    case BF_DEV_FAMILY_TOFINO2:
      pipe_disable = lld_efuse_get_pipe_disable(dev_id);
      for (i = 0; i < BF_SUBDEV_PIPE_COUNT; i++) {
        if (!(pipe_disable & 0x1)) {
          (*num_pipes)++;
        }
        pipe_disable >>= 1;
      }
      break;
    default:
      return LLD_ERR_BAD_PARM;
  }

#ifdef EMU_OVERRIDE_PIPE_COUNT
  *num_pipes = EMU_OVERRIDE_PIPE_COUNT;
#endif

  return LLD_OK;
}

/*******************************************************************
 * lld_sku_get_num_active_mau_stages
 *
 * Get num of active stages on this dev_id based on SKU
 *******************************************************************/
lld_err_t lld_sku_get_num_active_mau_stages(bf_dev_id_t dev_id,
                                            uint32_t *num_stages,
                                            uint32_t phy_pipe_id) {
  uint32_t stage_disable = lld_efuse_get_mau_stage_disable(dev_id, phy_pipe_id);
  unsigned i = 0;
  lld_dev_t *dev_p = lld_map_dev_id_to_dev_p_allow_unassigned(dev_id);
  uint32_t stage_count = 0;
  if (dev_p == NULL) {
    return LLD_ERR_BAD_PARM;
  }
  switch (dev_p->dev_family) {
    case BF_DEV_FAMILY_TOFINO:
#ifdef EMU_OVERRIDE_STAGE_COUNT
      *num_stages = EMU_OVERRIDE_STAGE_COUNT;
      (void)stage_count;
      (void)i;
      (void)stage_disable;
#else
      stage_count = 12;
      *num_stages = 12;
      for (i = 0; i < stage_count; i++) {
        if ((stage_disable & 0x1)) {
          (*num_stages)--;
        }
        stage_disable >>= 1;
      }
#endif
      return LLD_OK;
    case BF_DEV_FAMILY_TOFINO2:
#ifdef EMU_OVERRIDE_STAGE_COUNT
      *num_stages = EMU_OVERRIDE_STAGE_COUNT;
#else
      *num_stages = 21;
      stage_count = 21;  // an extra stage
      for (i = 0; i < stage_count; i++) {
        if ((stage_disable & 0x1)) {
          (*num_stages)--;
        }
        stage_disable >>= 1;
      }
      if (*num_stages > 20) {
        *num_stages = 20;
        // return LLD_ERR_BAD_PARM;
      }
#endif
      return LLD_OK;
    case BF_DEV_FAMILY_TOFINO3:
#ifdef EMU_OVERRIDE_STAGE_COUNT
      *num_stages = EMU_OVERRIDE_STAGE_COUNT;
#else
      *num_stages = 21;
      stage_count = 21;  // an extra stage
      for (i = 0; i < stage_count; i++) {
        if ((stage_disable & 0x1)) {
          (*num_stages)--;
        }
        stage_disable >>= 1;
      }
      if (*num_stages > 20) {
        *num_stages = 20;
        // return LLD_ERR_BAD_PARM;
      }
#endif
      return LLD_OK;



















    default:
      return LLD_ERR_BAD_PARM;
  }
}

/*******************************************************************
 * lld_sku_get_prsr_stage
 *
 * Get the "stage" associated with the parser on this dev_id based on SKU
 *******************************************************************/
lld_err_t lld_sku_get_prsr_stage(bf_dev_id_t dev_id, uint32_t *stage) {
  lld_dev_t *dev_p = lld_map_dev_id_to_dev_p_allow_unassigned(dev_id);

  if (dev_p == NULL) {
    return LLD_ERR_BAD_PARM;
  }
  // They are only used for addressing, fixed values, not configurable, not
  // changeable
  switch (dev_p->dev_family) {
    case BF_DEV_FAMILY_TOFINO:
      *stage = 0xe;
      break;
    case BF_DEV_FAMILY_TOFINO2:
      *stage = 0x18;
      break;
    case BF_DEV_FAMILY_TOFINO3:

      *stage = 0x18;
      break;
    default:
      *stage = ~0;
      return LLD_ERR_BAD_PARM;
  }

  return LLD_OK;
}

/*******************************************************************
 * lld_sku_get_dprsr_stage
 *
 * Get the "stage" associated with the deparser on this dev_id based on SKU
 *******************************************************************/
lld_err_t lld_sku_get_dprsr_stage(bf_dev_id_t dev_id, uint32_t *stage) {
  lld_dev_t *dev_p = lld_map_dev_id_to_dev_p_allow_unassigned(dev_id);

  if (dev_p == NULL) {
    return LLD_ERR_BAD_PARM;
  }
  // They are only used for addressing, fixed values, not configurable, not
  // changeable
  switch (dev_p->dev_family) {
    case BF_DEV_FAMILY_TOFINO:
      *stage = 0xf;
      break;
    case BF_DEV_FAMILY_TOFINO2:
      *stage = 0x1a;
      break;
    case BF_DEV_FAMILY_TOFINO3:

      *stage = 0x1a;
      break;
    default:
      *stage = ~0;
      return LLD_ERR_BAD_PARM;
  }

  return LLD_OK;
}

/*************************************************
 * lld_sku_pipe_id_map_get
 *
 * Map logical pipe ID to physicalpipe id for all SKUs
 * structure in the LLD context. Returns LLD error
 * if "chip" exceeds SDK configured max or bad efuse settings.
 *************************************************/
lld_err_t lld_sku_pipe_id_map_get(bf_dev_id_t dev_id,
                                  bf_dev_pipe_t pipe_log2phy[BF_PIPE_COUNT]) {
  uint32_t i;
  uint32_t pipe_disable;
  uint32_t pipe_cnt = 0;
  lld_dev_t *dev_p = lld_map_dev_id_to_dev_p_allow_unassigned(dev_id);
  if (dev_p == NULL) {
    for (i = 0; i < BF_PIPE_COUNT; i++) {
      pipe_log2phy[i] = BF_INVALID_PIPE;
    }
    return LLD_ERR_BAD_PARM;
  }

  pipe_disable = lld_efuse_get_pipe_disable(dev_id);
  pipe_cnt = BF_PIPE_COUNT;

  /* If none of the pipes are disabled and the die is not rotated then it's a
   * 1:1 log->phy mapping */
  /* It is assumened that pipe_disable is same for both sub devices for
   * Tofino3 */
  if (pipe_disable == 0 && !lld_efuse_get_die_rotated(dev_id)) {
    for (i = 0; i < pipe_cnt; i++) {
      pipe_log2phy[i] = i;
    }
    return LLD_OK;
  }
  switch (dev_p->dev_family) {
    case BF_DEV_FAMILY_TOFINO:
      /* Logical pipe 0 */
      if (pipe_disable & 0x1) {
        if (pipe_disable & 0x2) { /* must not be */
          lld_log("Wrong pipe_disable info(%d) in efuse, dev_id: %d\n",
                  pipe_disable,
                  dev_id);
          return LLD_ERR_BAD_PARM;
        }
        pipe_log2phy[0] = 1;
      } else if (pipe_disable & 0x2) {
        pipe_log2phy[0] = 0;
      } else { /* must not be */
        lld_log("Wrong pipe_disable info(%d) in efuse, dev_id: %d\n",
                pipe_disable,
                dev_id);
        return LLD_ERR_BAD_PARM;
      }
      /* Logical pipe 1 */
      if (pipe_disable & 0x4) {
        if (pipe_disable & 0x8) { /* must not be */
          lld_log("Wrong pipe_disable info(%d) in efuse, dev_id: %d\n",
                  pipe_disable,
                  dev_id);
          return LLD_ERR_BAD_PARM;
        }
        pipe_log2phy[1] = 3;
      } else if (pipe_disable & 0x8) {
        pipe_log2phy[1] = 2;
      } else { /* must not be */
        lld_log("Wrong pipe_disable info(%d) in efuse, dev_id: %d\n",
                pipe_disable,
                dev_id);
        return LLD_ERR_BAD_PARM;
        ;
      }
      /* invalidate the rest of the pipe ids */
      pipe_log2phy[2] = BF_INVALID_PIPE;
      pipe_log2phy[3] = BF_INVALID_PIPE;
      break;
    case BF_DEV_FAMILY_TOFINO2:
      if (lld_tof2_sku_pipe_id_map_get(dev_id, pipe_log2phy) != LLD_OK)
        return LLD_ERR_BAD_PARM;
      break;
    case BF_DEV_FAMILY_TOFINO3:
      if (lld_tof3_sku_pipe_id_map_get(dev_id, pipe_log2phy) != LLD_OK)
        return LLD_ERR_BAD_PARM;
      break;



    default:
      for (i = 0; i < BF_PIPE_COUNT; i++) {
        pipe_log2phy[i] = BF_INVALID_PIPE;
      }
      lld_log(
          "Wrong dev_family: %d, in dev_id: %d\n", dev_p->dev_family, dev_id);
      return LLD_ERR_BAD_PARM;
  }
  return LLD_OK;
}

/*************************************************
 * lld_sku_get_pkt_generator_en
 *
 * Returns if packet generator is enabled on this SKU
 * Returns LLD_OK
 *************************************************/
lld_err_t lld_sku_get_pkt_generator_en(bf_dev_id_t dev_id,
                                       bf_subdev_id_t subdev_id,
                                       int *en) {
  if (lld_subdev_efuse_get_packet_generator_disable(dev_id, subdev_id) == 1) {
    *en = 0;
  } else {
    *en = 1;
  }
  return LLD_OK;
}

/*************************************************
 * lld_sku_get_resubmit_en
 *
 * Returns if resubmit feature is enabled on this SKU
 * Returns LLD_OK
 *************************************************/
lld_err_t lld_sku_get_resubmit_en(bf_dev_id_t dev_id,
                                  bf_subdev_id_t subdev_id,
                                  int *en) {
  if (lld_subdev_efuse_get_resubmit_disable(dev_id, subdev_id) == 1) {
    *en = 0;
  } else {
    *en = 1;
  }
  return LLD_OK;
}

/*************************************************
 * lld_sku_get_baresync_en
 *
 * Returns if Baresync is enabled on this SKU
 * Returns LLD_OK
 *************************************************/
lld_err_t lld_sku_get_baresync_en(bf_dev_id_t dev_id,
                                  bf_subdev_id_t subdev_id,
                                  int *en) {
  if (lld_subdev_efuse_get_baresync_disable(dev_id, subdev_id) == 1) {
    *en = 0;
  } else {
    *en = 1;
  }
  return LLD_OK;
}

/*************************************************
 * lld_sku_get_port_speed
 *
 * Returns the port speed reduction value of the device
 * Returns LLD_OK on success and LLD_ERR_BAD_PARM on error
 *************************************************/
lld_err_t lld_sku_get_port_speed(bf_dev_id_t dev_id, bf_sku_port_speed_t *sp) {
  uint32_t port_speed = lld_efuse_get_port_speed_reduction(dev_id);

  if (port_speed == 0xFFFFFFFF) {
    return LLD_ERR_BAD_PARM;
  }
  switch ((lld_efuse_port_speed_t)port_speed) {
    case LLD_PORT_SPEED_RED_100G:
      *sp = BF_SKU_PORT_SPEED_100G;
      break;
    case LLD_PORT_SPEED_RED_50G:
      *sp = BF_SKU_PORT_SPEED_50G;
      break;
    case LLD_PORT_SPEED_RED_25G:
      *sp = BF_SKU_PORT_SPEED_25G;
      break;
    default:
      *sp = BF_SKU_PORT_SPEED_INVALID;
      return LLD_ERR_BAD_PARM;
  }
  return LLD_OK;
}

/*************************************************
 * lld_sku_get_pcie_lanes
 *
 * Returns number of pcie lanes supported by the device
 * Returns LLD_OK on success and LLD_ERR_BAD_PARM on error
 *************************************************/
lld_err_t lld_sku_get_pcie_lanes(bf_dev_id_t dev_id,
                                 bf_subdev_id_t subdev_id,
                                 int *num_lanes) {
  uint32_t lanes = lld_subdev_efuse_get_pcie_lane_reduction(dev_id, subdev_id);

  if (lanes == 0xFFFFFFFF) {
    return LLD_ERR_BAD_PARM;
  }
  switch ((lld_efuse_pcie_lane_t)lanes) {
    case LLD_PCIE_LANES_4:
      *num_lanes = 4;
      break;
    case LLD_PCIE_LANES_2:
      *num_lanes = 2;
      break;
    case LLD_PCIE_LANES_1:
      *num_lanes = 1;
      break;
    default:
      *num_lanes = 0;
      return LLD_ERR_BAD_PARM;
  }
  return LLD_OK;
}

static lld_err_t lld_tof_sku_get_core_clk_freq(
    bf_dev_id_t dev_id,
    uint32_t frequency_reduction,
    bf_sku_core_clk_freq_t *bps_freq,
    bf_sku_core_clk_freq_t *pps_freq) {
  bool is_sw_model;
  bf_status_t sts;
  sts = bf_drv_device_type_get(dev_id, &is_sw_model);
  if (sts != BF_SUCCESS) {
    lld_log("ERROR: device type get failed for device %d, status %s (%d)",
            dev_id,
            bf_err_str(sts),
            sts);
    return LLD_ERR_BAD_PARM;
  }

  if (!is_sw_model) {
    /* for A0: if the freq_reduction is set to "0", set the
     * core clock to 1.1Ghz, not 1.25Ghz.
     * For now, we are pretending "A0" parts are indicated by
     * a value of 0 for part_revision_number.
     */
    if (lld_efuse_get_part_revision_number(dev_id) == 0) {
      if (frequency_reduction == 0) {
        *bps_freq = BF_SKU_CORE_CLK_1_1_GHZ;
        *pps_freq = BF_SKU_CORE_CLK_1_1_GHZ;
        lld_log("** Note: Core clk restricted to 1.1Ghz for A0 **");
        return LLD_OK;
      }
    }
  }

  /*
   * fuse_freq[1:0]
   * 2'b00 - no constraint
   * 2'b01 - 1.1G
   * 2'b11 - 1G
   */
  switch (frequency_reduction) {
    case 0:
      *bps_freq = BF_SKU_CORE_CLK_1_25_GHZ;
      *pps_freq = BF_SKU_CORE_CLK_1_25_GHZ;
      break;
    case 1:
      *bps_freq = BF_SKU_CORE_CLK_1_1_GHZ;
      *pps_freq = BF_SKU_CORE_CLK_1_1_GHZ;
      break;
    case 3:
      *bps_freq = BF_SKU_CORE_CLK_1_0_GHZ;
      *pps_freq = BF_SKU_CORE_CLK_1_0_GHZ;
      break;
    default:
      lld_log(
          "Error: EFUSE frequency reduction mis-programmed: dev=%d : "
          "efuse:%d",
          dev_id,
          frequency_reduction);
      *bps_freq = BF_SKU_CORE_CLK_UNDEF_GHZ;
      *pps_freq = BF_SKU_CORE_CLK_UNDEF_GHZ;
      return LLD_ERR_INVALID_CFG;
  }
  return LLD_OK;
}

static lld_err_t lld_tof2_sku_get_core_clk_freq(
    bf_dev_id_t dev_id,
    bf_sku_core_clk_freq_t *bps_freq,
    bf_sku_core_clk_freq_t *pps_freq) {
  bf_sku_chip_part_rev_t rev = BF_SKU_CHIP_PART_REV_A0;
  uint32_t pps_freq_reduction = lld_efuse_get_frequency_reduction_pps(dev_id);
  uint32_t bps_freq_reduction = lld_efuse_get_frequency_reduction_bps(dev_id);
  lld_sku_get_chip_part_revision_number(dev_id, &rev);
  switch (pps_freq_reduction) {
    case 0:
      *pps_freq = BF_SKU_CORE_CLK_1_5_GHZ;
      break;
    case 1:
      *pps_freq = BF_SKU_CORE_CLK_1_5_GHZ;
      break;
    case 2:
      *pps_freq = BF_SKU_CORE_CLK_1_3_GHZ;
      break;
    case 3:
      *pps_freq = BF_SKU_CORE_CLK_1_1_GHZ;
      break;
    default:
      lld_log(
          "Error: EFUSE frequency reduction mis-programmed: dev=%d : "
          "efuse:%d",
          dev_id,
          pps_freq_reduction);
      *pps_freq = BF_SKU_CORE_CLK_UNDEF_GHZ;
      return LLD_ERR_INVALID_CFG;
  }
  switch (bps_freq_reduction) {
    case 0:
      *bps_freq = BF_SKU_CORE_CLK_1_3_GHZ;
      break;
    case 1:
      *bps_freq = BF_SKU_CORE_CLK_1_3_GHZ;
      break;
    case 2:
      *bps_freq = BF_SKU_CORE_CLK_1_3_GHZ;
      break;
    case 3:
      *bps_freq = BF_SKU_CORE_CLK_1_25_GHZ;
      break;
    default:
      lld_log(
          "Error: EFUSE frequency reduction mis-programmed: dev=%d : "
          "efuse:%d",
          dev_id,
          bps_freq_reduction);
      *bps_freq = BF_SKU_CORE_CLK_UNDEF_GHZ;
      return LLD_ERR_INVALID_CFG;
  }

  if (rev == BF_SKU_CHIP_PART_REV_A0) {
    lld_log("Note: Core clock restricted to 1.25GHz for A0");
    *bps_freq = BF_SKU_CORE_CLK_1_25_GHZ;
  }
  return LLD_OK;
}

/* information based on sub_device-0 is returned */
static lld_err_t lld_tof3_sku_get_core_clk_freq(
    bf_dev_id_t dev_id,
    bf_sku_core_clk_freq_t *bps_freq,
    bf_sku_core_clk_freq_t *pps_freq) {
  bf_sku_chip_part_rev_t rev = lld_efuse_get_part_revision_number(dev_id);
  uint32_t pps_freq_reduction = lld_efuse_get_frequency_reduction_pps(dev_id);
  uint32_t bps_freq_reduction = lld_efuse_get_frequency_reduction_bps(dev_id);
  lld_sku_get_chip_part_revision_number(dev_id, &rev);
  switch (pps_freq_reduction) {
    default:
      *pps_freq = BF_SKU_CORE_CLK_1_2625_GHZ;
      break;
  }
  switch (bps_freq_reduction) {
    default:
      *bps_freq = BF_SKU_CORE_CLK_1_3_GHZ;
      break;
  }

  return LLD_OK;
}

/*************************************************
 * lld_sku_get_core_clk_freq
 *
 * Returns LLD_OK on success and LLD_ERR_LOCK_FAILED on failure
 *************************************************/
lld_err_t lld_sku_get_core_clk_freq(bf_dev_id_t dev_id,
                                    bf_sku_core_clk_freq_t *bps_freq,
                                    bf_sku_core_clk_freq_t *pps_freq) {
  uint32_t frequency_reduction;
  lld_dev_t *dev_p = lld_map_dev_id_to_dev_p_allow_unassigned(dev_id);
  if (dev_p == NULL) return LLD_ERR_BAD_PARM;

  frequency_reduction = lld_efuse_get_frequency_reduction(dev_id);
  if (frequency_reduction == 0xFFFFFFFF) return LLD_ERR_INVALID_CFG;

  switch (dev_p->dev_family) {
    case BF_DEV_FAMILY_TOFINO:
      return lld_tof_sku_get_core_clk_freq(
          dev_id, frequency_reduction, bps_freq, pps_freq);
    case BF_DEV_FAMILY_TOFINO2:
      return lld_tof2_sku_get_core_clk_freq(dev_id, bps_freq, pps_freq);
    case BF_DEV_FAMILY_TOFINO3:
      return lld_tof3_sku_get_core_clk_freq(dev_id, bps_freq, pps_freq);





    default:
      return LLD_ERR_BAD_PARM;
  }
}

static lld_err_t lld_tof_sku_check_efuse_consistency(bf_dev_id_t dev_id) {
  uint64_t port_lo64, port_hi64;
  uint32_t port_speed, pcie_lanes, resubmit, baresync, pipe_disable;
  uint32_t part_sku;

  lld_efuse_get_port_disable(dev_id, &port_hi64, &port_lo64);
  port_speed = lld_efuse_get_port_speed_reduction(dev_id);
  pcie_lanes = lld_subdev_efuse_get_pcie_lane_reduction(dev_id, 0);
  resubmit = lld_subdev_efuse_get_resubmit_disable(dev_id, 0);
  baresync = lld_subdev_efuse_get_baresync_disable(dev_id, 0);
  pipe_disable = lld_efuse_get_pipe_disable(dev_id);

  part_sku = lld_sku_get_sku(dev_id);
  switch (part_sku) {
    case BFN_SKU_BFN_T1_64Q:
      bf_sys_assert(pipe_disable == 0);
      bf_sys_assert(port_speed == LLD_PORT_SPEED_RED_100G);
      bf_sys_assert(pcie_lanes == LLD_PCIE_LANES_4);
      bf_sys_assert(resubmit == 0);
      bf_sys_assert(baresync == 0);
      bf_sys_assert(port_lo64 == 0x0);
      bf_sys_assert(port_hi64 == 0x0);
      break;
    case BFN_SKU_BFN_T1_32Q:
      bf_sys_assert(pipe_disable == 0);
      bf_sys_assert(port_speed == LLD_PORT_SPEED_RED_100G);
      bf_sys_assert(pcie_lanes == LLD_PCIE_LANES_4);
      bf_sys_assert(resubmit == 0);
      bf_sys_assert(baresync == 0);
      bf_sys_assert(port_lo64 == 0x0);
      bf_sys_assert(port_hi64 == 0x0);
      break;
    case BFN_SKU_BFN_T1_32D:
      bf_sys_assert((pipe_disable & 0x3) != 0x3);
      bf_sys_assert((pipe_disable & 0x3) != 0);
      bf_sys_assert((pipe_disable & 0xc) != 0xc);
      bf_sys_assert((pipe_disable & 0xc) != 0);
      bf_sys_assert(pcie_lanes == LLD_PCIE_LANES_2);
      bf_sys_assert(resubmit == 1);
      bf_sys_assert(baresync == 1);
      bf_sys_assert(port_lo64 == 0xAAAAAAAAAAAAAAAAULL ||
                    port_lo64 == 0x5555555555555555ULL);
      bf_sys_assert(port_hi64 == 0x0);
      break;
    case BFN_SKU_BFN_T1_32D_018:
      bf_sys_assert((pipe_disable & 0x3) != 0x3);
      bf_sys_assert((pipe_disable & 0x3) != 0);
      bf_sys_assert((pipe_disable & 0xc) != 0xc);
      bf_sys_assert((pipe_disable & 0xc) != 0);
      bf_sys_assert(pcie_lanes == LLD_PCIE_LANES_2);
      bf_sys_assert(resubmit == 1);
      bf_sys_assert(baresync == 1);
      bf_sys_assert(port_lo64 == 0xEEEEAEEEEEEEAEEEULL ||
                    port_lo64 == 0xDDDD5DDDDDDD5DDDULL);
      bf_sys_assert(port_hi64 == 0x0);
      break;
    case BFN_SKU_BFN_T1_32D_020:
      bf_sys_assert((pipe_disable & 0x3) != 0x3);
      bf_sys_assert((pipe_disable & 0x3) != 0);
      bf_sys_assert((pipe_disable & 0xc) != 0xc);
      bf_sys_assert((pipe_disable & 0xc) != 0);
      bf_sys_assert(pcie_lanes == LLD_PCIE_LANES_2);
      bf_sys_assert(resubmit == 1);
      bf_sys_assert(baresync == 1);
      bf_sys_assert(port_lo64 == 0xEEEAAEEEEEEAAEEEULL ||
                    port_lo64 == 0xDDD55DDDDDD55DDDULL);
      bf_sys_assert(port_hi64 == 0x0);
      break;
    default:
      lld_log("Bad chip %d Sku %d", dev_id, part_sku);
      return LLD_ERR_BAD_PARM;
  }
  return LLD_OK;
}
static lld_err_t lld_tof2_sku_check_efuse_consistency(bf_dev_id_t dev_id) {
  uint32_t i;
  bool phy_pipe1[BF_SUBDEV_PIPE_COUNT] = {false, false, false, false};
  uint64_t port_lo64;
  uint32_t pipe_disable, pipe_numb = 0, stage_disable;
  // check port disable
  lld_efuse_get_port_disable(dev_id, NULL, &port_lo64);
  if (((port_lo64 & 0x1) ^ ((port_lo64 >> 39) & 0x1)) == 0)
    return LLD_ERR_BAD_PARM;
  if (((port_lo64 >> 1) & 0xFFFFFFFF) == 0xFFFFFFFF) return LLD_ERR_BAD_PARM;
  for (i = 0; i < BF_SUBDEV_PIPE_COUNT; i++) {
    if (((port_lo64 >> (8 * i + 1)) & 0xFF) != 0xFF) {
      phy_pipe1[i] = true;
    }
  }
  // check pipe disable
  pipe_disable = lld_efuse_get_pipe_disable(dev_id);
  for (i = 0; i < BF_SUBDEV_PIPE_COUNT; i++) {
    if (((pipe_disable >> i) & 0x1) == 0) {
      pipe_numb++;
      // check port resource is available
      if (!phy_pipe1[i]) return LLD_ERR_BAD_PARM;
      // check mau stage disable
      stage_disable = lld_efuse_get_mau_stage_disable(dev_id, i);
      if (!stage_disable || stage_disable == 0x1FFFFF) return LLD_ERR_BAD_PARM;
    }
  }
  if (pipe_numb == 0) return LLD_ERR_BAD_PARM;
  return LLD_OK;
}
static lld_err_t lld_tof3_sku_check_efuse_consistency(
    bf_dev_id_t dev_id, bf_subdev_id_t subdev_id) {
  uint32_t i;
  bool phy_pipe1[BF_PIPE_COUNT] = {
      false, false, false, false, false, false, false, false};
  uint64_t port_lo64;
  uint32_t pipe_disable, pipe_numb = 0, stage_disable;
  // check port disable
  lld_efuse_get_port_disable(dev_id, NULL, &port_lo64);
  if (((port_lo64 & 0x1) ^ ((port_lo64 >> 39) & 0x1)) == 0)
    return LLD_ERR_BAD_PARM;
  if (((port_lo64 >> 1) & 0xFFFFFFFF) == 0xFFFFFFFF) return LLD_ERR_BAD_PARM;
  for (i = 0; i < BF_PIPE_COUNT; i++) {
    if (((port_lo64 >> (8 * i + 1)) & 0xFF) != 0xFF) {
      phy_pipe1[i] = true;
    }
  }
  // check pipe disable
  pipe_disable = lld_efuse_get_pipe_disable(dev_id);
  for (i = 0; i < BF_PIPE_COUNT; i++) {
    if (((pipe_disable >> i) & 0x1) == 0) {
      pipe_numb++;
      // check port resource is available
      if (!phy_pipe1[i]) return LLD_ERR_BAD_PARM;
      // check mau stage disable
      stage_disable =
          lld_subdev_efuse_get_mau_stage_disable(dev_id, subdev_id, i);
      if ((stage_disable & 0x1FFFFF) == 0x1FFFFF) return LLD_ERR_BAD_PARM;
    }
  }
  if (pipe_numb == 0) return LLD_ERR_BAD_PARM;
  return LLD_OK;
}

/*************************************************
 * lld_sku_check_efuse_consistency
 *
 * checks if efuse bits are consistent on this SKU
 * Returns LLD error
 * if the part_id is not valid, LLD_OK, otherwise.
 * asserts on th efirst inconsistecy found
 *************************************************/
lld_err_t lld_sku_check_efuse_consistency(bf_dev_id_t dev_id) {
  uint32_t num_subdev, i;
  lld_err_t err;
  lld_dev_t *dev_p = lld_map_dev_id_to_dev_p_allow_unassigned(dev_id);
  if (dev_p == NULL) return LLD_ERR_BAD_PARM;
  switch (dev_p->dev_family) {
    case BF_DEV_FAMILY_TOFINO:
      return lld_tof_sku_check_efuse_consistency(dev_id);
    case BF_DEV_FAMILY_TOFINO2:
      return lld_tof2_sku_check_efuse_consistency(dev_id);
    case BF_DEV_FAMILY_TOFINO3:
      err = lld_sku_get_num_subdev(dev_id, &num_subdev, NULL);
      if (err != LLD_OK) {
        return err;
      }
      for (i = 0; i < num_subdev; i++) {
        err |= lld_tof3_sku_check_efuse_consistency(dev_id, i);
      }
      return err;
    default:
      return LLD_ERR_BAD_PARM;
  }
}

/*************************************************
 * lld_sku_get_chip_part_revision_number
 *
 * Returns chup part revision number
 * Returns LLD_OK on success and LLD_ERR_BAD_PARM on error
 *************************************************/
lld_err_t lld_sku_get_chip_part_revision_number(
    bf_dev_id_t dev_id, bf_sku_chip_part_rev_t *rev_no) {
  /* Tofino-2 B0 and B1 both have device-id 0x0110 while A0 uses 0x0100. */
  if (lld_efuse_get_device_id(dev_id) == 0x0110) {
    /* This is a B0 or B1 part, use the part_revision_number to identify which.
     * B0 has a value of 0 while B1 has a value of 2. */
    uint32_t rev = lld_efuse_get_part_revision_number(dev_id);
    *rev_no = rev == 0 ? BF_SKU_CHIP_PART_REV_B0 : BF_SKU_CHIP_PART_REV_B1;
    return LLD_OK;
  }
  *rev_no = lld_efuse_get_part_revision_number(dev_id);

  return LLD_OK;
}

/*************************************************
 * lld_sku_get_chip_id
 *
 * Returns chup id (giving wafer position)
 * Returns LLD_OK on success and LLD_ERR_BAD_PARM on error
 *************************************************/
lld_err_t lld_sku_get_chip_id(bf_dev_id_t dev_id, uint64_t *chip_id) {
  *chip_id = lld_efuse_get_chip_id(dev_id);

  return LLD_OK;
}

/*************************************************
 * lld_sku_get_vmin
 *
 * Returns VMIN for the chip
 * Returns LLD_OK
 *************************************************/
lld_err_t lld_sku_get_vmin(bf_dev_id_t dev_id, int *vmin) {
  if (!vmin) return LLD_ERR_BAD_PARM;
  *vmin = lld_efuse_get_vmin(dev_id);
  return LLD_OK;
}

/*************************************************
 * lld_sku_is_dev_port_internal
 *
 * Returns if the device port is internal (doesn't have serdes)
 * Returns LLD_OK on success and LLD_ERR_BAD_PARM on error
 *************************************************/
lld_err_t lld_sku_is_dev_port_internal(bf_dev_id_t dev_id,
                                       uint32_t dev_port_id,
                                       bool *is_internal) {
  lld_dev_t *dev_p = NULL;
  lld_err_t rc;
  uint32_t part_sku, mac_blk, ch;

  if (is_internal == NULL) return LLD_ERR_BAD_PARM;

  dev_p = lld_map_dev_id_to_dev_p_allow_unassigned(dev_id);
  if (dev_p == NULL) return LLD_ERR_BAD_PARM;

  part_sku = lld_sku_get_sku(dev_id);

  rc = lld_sku_map_dev_port_id_to_mac_ch(dev_id, dev_port_id, &mac_blk, &ch);
  if (rc != LLD_OK) {
    return rc;
  }

  if (part_sku == BFN_SKU_BFN_T1_32Q) {
    // For 32Q, odd MACs are internal
    if ((mac_blk & 0x01)) {
      // Indicates that this is an internal port
      *is_internal = true;
      return LLD_OK;
    }
  } else if (part_sku == BFN_SKU_BFN0064Q) {
    // tofino2 6.4Q, any ports in pipe2&3 are internal
    if (mac_blk > 16) {
      *is_internal = true;
      return LLD_OK;
    }
  }

  // For all other SKUs, there is no concept of an internal port. So
  // just return false
  *is_internal = false;
  return LLD_OK;
}

#define BF_TOF3_PCIE_PORT (0)
#define BF_TOF3_ETH_CPU_PORT (2)
#define BF_TOF3_MAC0_DIE1_PIPE 4
/*************************************************
 * lld_get_pcie_cpu_port
 *
 *
 *************************************************/
bf_dev_port_t lld_get_pcie_cpu_port(bf_dev_id_t dev_id) {
  lld_dev_t *dev_p = lld_map_dev_id_to_dev_p_allow_unassigned(dev_id);
  if (dev_p == NULL) return -1;
  switch (dev_p->dev_family) {
    case BF_DEV_FAMILY_TOFINO: {
      uint32_t num_pipes = 0;
      lld_err_t rc = lld_sku_get_num_active_pipes(dev_id, &num_pipes);
      if (rc != LLD_OK) return -1;
      if (num_pipes > 2) {
        return 320;
      } else {
        return 192;
      }
    }
    case BF_DEV_FAMILY_TOFINO2:
      return MAKE_DEV_PORT(tof2_cpu_pipe(dev_id), 0);
    case BF_DEV_FAMILY_TOFINO3:
      return (BF_TOF3_PCIE_PORT);
    default:
      return -1;
  }
}

bf_dev_port_t lld_get_pcie_cpu_port2(bf_dev_id_t dev_id) {
  uint32_t num_subdev = 0;
  lld_dev_t *dev_p = lld_map_dev_id_to_dev_p_allow_unassigned(dev_id);
  if (dev_p == NULL) return -1;
  switch (dev_p->dev_family) {
    case BF_DEV_FAMILY_TOFINO3:
      lld_sku_get_num_subdev(dev_id, &num_subdev, NULL);
      if (num_subdev < 2) {
        return -1;
      }
      return MAKE_DEV_PORT(BF_TOF3_MAC0_DIE1_PIPE, BF_TOF3_PCIE_PORT);
    case BF_DEV_FAMILY_TOFINO:
    case BF_DEV_FAMILY_TOFINO2:
    default:
      return -1;
  }
}

/*************************************************
 * lld_get_min_cpu_port
 *
 *
 *************************************************/
bf_dev_port_t lld_get_min_cpu_port(bf_dev_id_t dev_id) {
  lld_dev_t *dev_p = lld_map_dev_id_to_dev_p_allow_unassigned(dev_id);
  if (dev_p == NULL) return -1;
  switch (dev_p->dev_family) {
    case BF_DEV_FAMILY_TOFINO:
      return 64;
    case BF_DEV_FAMILY_TOFINO2:
      return tof2_cpu_port(dev_id);
    case BF_DEV_FAMILY_TOFINO3:
      return (BF_TOF3_ETH_CPU_PORT);
    default:
      return -1;
  }
}
bf_dev_port_t lld_get_min_cpu_port2(bf_dev_id_t dev_id) {
  lld_dev_t *dev_p = lld_map_dev_id_to_dev_p_allow_unassigned(dev_id);
  if (dev_p == NULL) return -1;
  switch (dev_p->dev_family) {
    case BF_DEV_FAMILY_TOFINO3:
      return MAKE_DEV_PORT(BF_TOF3_MAC0_DIE1_PIPE, BF_TOF3_ETH_CPU_PORT);
    case BF_DEV_FAMILY_TOFINO:
    case BF_DEV_FAMILY_TOFINO2:
    default:
      return -1;
  }
}
/*************************************************
 * lld_get_max_cpu_port
 *
 *
 *************************************************/
bf_dev_port_t lld_get_max_cpu_port(bf_dev_id_t dev_id) {
  lld_dev_t *dev_p = lld_map_dev_id_to_dev_p_allow_unassigned(dev_id);
  if (dev_p == NULL) return -1;
  switch (dev_p->dev_family) {
    case BF_DEV_FAMILY_TOFINO:
      return 67;
    case BF_DEV_FAMILY_TOFINO2:
      return tof2_cpu_port(dev_id) + 3;
    case BF_DEV_FAMILY_TOFINO3:
      return (4);
    default:
      return -1;
  }
}
/*************************************************
 * lld_get_next_cpu_port
 *
 *
 *************************************************/
bf_status_t lld_get_next_cpu_port(bf_dev_id_t dev_id, bf_dev_port_t *port) {
  int offset = 1;
  const bf_dev_port_t min = lld_get_min_cpu_port(dev_id);
  const bf_dev_port_t max = lld_get_max_cpu_port(dev_id);
  lld_dev_t *dev_p = lld_map_dev_id_to_dev_p_allow_unassigned(dev_id);
  if (dev_p == NULL || port == NULL) return BF_INVALID_ARG;
  if (dev_p->dev_family == BF_DEV_FAMILY_TOFINO3) {
    if (*port & 1) return BF_INVALID_ARG;
    offset = 2;
  }
  if (*port < min || *port > max) return BF_INVALID_ARG;
  if (*port == max) return BF_OBJECT_NOT_FOUND;
  *port += offset;
  return BF_SUCCESS;
}

/*************************************************
 * lld_get_min_front_panel_port
 *
 *
 *************************************************/
int lld_get_min_fp_port(bf_dev_id_t dev_id) {
  lld_dev_t *dev_p = lld_map_dev_id_to_dev_p_allow_unassigned(dev_id);
  if (dev_p == NULL) return (-1);
  switch (dev_p->dev_family) {
    case BF_DEV_FAMILY_TOFINO:
      return (0);
    case BF_DEV_FAMILY_TOFINO2:
      return (8);
    case BF_DEV_FAMILY_TOFINO3:
      return (8);
    default:
      return (-1);
  }
}

/*************************************************
 * lld_get_max_front_panel_port
 *
 *
 *************************************************/
int lld_get_max_fp_port(bf_dev_id_t dev_id) {
  lld_dev_t *dev_p = lld_map_dev_id_to_dev_p_allow_unassigned(dev_id);
  if (dev_p == NULL) return (-1);
  switch (dev_p->dev_family) {
    case BF_DEV_FAMILY_TOFINO:
      return (63);
    case BF_DEV_FAMILY_TOFINO2:
      return (71);
    case BF_DEV_FAMILY_TOFINO3:
      return (71);
    default:
      return (-1);
  }
}

/*************************************************
 * lld_get_max_pront_port_mac_per_pipe
 * not including CPU MAC
 *
 *************************************************/
int lld_get_max_frontport_mac_per_pipe(bf_dev_id_t dev_id) {
  lld_dev_t *dev_p = lld_map_dev_id_to_dev_p_allow_unassigned(dev_id);
  if (dev_p == NULL) return (-1);
  switch (dev_p->dev_family) {
    case BF_DEV_FAMILY_TOFINO:
      return (16);
    case BF_DEV_FAMILY_TOFINO2:
      return (8);
    case BF_DEV_FAMILY_TOFINO3:
      return (8);
    default:
      return (-1);
  }
}

/*************************************************
 * lld_get_max_mac_blocks
 * including CPU MAC
 *
 *************************************************/
int lld_get_max_mac_blocks(bf_dev_id_t dev_id) {
  uint32_t num_subdev = 0;
  lld_sku_get_num_subdev(dev_id, &num_subdev, NULL);
  return (lld_get_max_frontport_mac_per_pipe(dev_id) *
              (BF_SUBDEV_PIPE_COUNT * num_subdev) +
          1);
}

/*************************************************
 * lld_get_chnls_per_mac
 * front panel mac and recirc ports
 *
 *************************************************/
int lld_get_chnls_per_mac(bf_dev_id_t dev_id) {
  lld_dev_t *dev_p = lld_map_dev_id_to_dev_p_allow_unassigned(dev_id);
  if (dev_p == NULL) return (-1);
  switch (dev_p->dev_family) {
    case BF_DEV_FAMILY_TOFINO:
      return (4);
    case BF_DEV_FAMILY_TOFINO2:
      return (8);
    case BF_DEV_FAMILY_TOFINO3:
      return (4);
    default:
      return (-1);
  }
}

/*************************************************
 * lld_get_chnls_per_mac
 * all ports
 *
 *************************************************/
int lld_get_chnls_dev_port(bf_dev_id_t dev_id, bf_dev_port_t dev_port_id) {
  lld_dev_t *dev_p = lld_map_dev_id_to_dev_p_allow_unassigned(dev_id);
  bf_dev_port_t min_cpu = lld_get_min_cpu_port(dev_id);
  bf_dev_port_t max_cpu = lld_get_max_cpu_port(dev_id);
  if (dev_p == NULL) return (-1);
  switch (dev_p->dev_family) {
    case BF_DEV_FAMILY_TOFINO:
      return 4;
    case BF_DEV_FAMILY_TOFINO2: {
      if (dev_port_id >= min_cpu && dev_port_id <= max_cpu) {
        return 4;  // eth cpu ports
      }
      return 8;  // front panel ports
    }
    case BF_DEV_FAMILY_TOFINO3:
      if (dev_port_id >= min_cpu && dev_port_id <= max_cpu) {
        return (2);  // eth cpu ports
      } else {
        return (4);  // front panel ports
      }
    default:
      return -1;
  }
}

/*************************************************
 * lld_get_internal_pipe_numb
 *
 *
 *************************************************/
int lld_get_internal_pipe_numb(bf_dev_id_t dev_id) {
  uint32_t part_sku;
  part_sku = lld_sku_get_sku(dev_id);
  switch (part_sku) {
    case BFN_SKU_BFN_T1_32Q:
    case BFN_SKU_BFN0064Q:
      return 2;
    default:
      return 0;
  }
}

static uint32_t tiles[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1,
    2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3};  //  indexed by UMAC
static uint32_t grps[] = {
    8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 1, 2, 3, 4, 5, 6, 7,
    7, 6, 5, 4, 3, 2, 1, 0, 0, 1, 2, 3, 4, 5, 6, 7};  // mac

static uint32_t grp_9_mac_stn_id[4] = {35, 36, 37, 38};

/*******************************************************************
 * lld_sku_map_mac_stn_id_to_tile_and_group
 *
 * Return the tile and group associated with the given MAC stn-id
 *******************************************************************/
lld_err_t lld_sku_map_mac_stn_id_to_tile_and_group(bf_dev_id_t dev_id,
                                                   uint32_t mac_stn_id,
                                                   uint32_t *tile,
                                                   uint32_t *grp) {
  bool rotated;
  lld_dev_t *dev_p = lld_map_dev_id_to_dev_p_allow_unassigned(dev_id);
  if (dev_p == NULL) return LLD_ERR_BAD_PARM;

  // not valid on Tof1 (not tile architecture)
  if (dev_p->dev_family != BF_DEV_FAMILY_TOFINO2) {
    return LLD_ERR_INVALID_CFG;
  }

  if (!tile) return LLD_ERR_BAD_PARM;
  if (!grp) return LLD_ERR_BAD_PARM;

  rotated = lld_efuse_get_die_rotated(dev_id);

  if ((mac_stn_id == 0 && !rotated) ||
      (mac_stn_id == LLD_TOF2_ROTATED_PHY_CPU_MAC_ID && rotated)) {
    *tile = 0;  // cpu port
    *grp = 8;
  } else if (mac_stn_id == 35) {
    *tile = rotated ? 2 : 0;
    *grp = 9;
  } else if (mac_stn_id == 36) {
    *tile = rotated ? 3 : 1;
    *grp = 9;
  } else if (mac_stn_id == 37) {
    *tile = rotated ? 0 : 2;
    *grp = 9;
  } else if (mac_stn_id == 38) {
    *tile = rotated ? 1 : 3;
    *grp = 9;
  } else if (mac_stn_id < 33) {
    *tile = tiles[mac_stn_id];
    if (rotated) {
      *tile = (*tile + 2) & 3;
    }
    *grp = grps[mac_stn_id];
  } else {
    *tile = 0xffffffff;
    *grp = 0xffffffff;
    return LLD_ERR_BAD_PARM;
  }
  return LLD_OK;
}

/*******************************************************************
 * lld_sku_map_tile_and_group_to_mac_stn_id
 *
 * Return the MAC stn-id associated with the given tile and group
 *******************************************************************/
lld_err_t lld_sku_map_tile_and_group_to_mac_stn_id(bf_dev_id_t dev_id,
                                                   uint32_t tile,
                                                   uint32_t grp,
                                                   uint32_t *mac_stn_id) {
  uint32_t umac;

  lld_dev_t *dev_p = lld_map_dev_id_to_dev_p_allow_unassigned(dev_id);
  if (dev_p == NULL) return LLD_ERR_BAD_PARM;

  // not valid on Tof1 (not tile architecture)
  if (dev_p->dev_family != BF_DEV_FAMILY_TOFINO2) {
    return LLD_ERR_INVALID_CFG;
  }

  if (!mac_stn_id) return LLD_ERR_BAD_PARM;
  if (tile >= 4) return LLD_ERR_BAD_PARM;
  if (grp > 9) return LLD_ERR_BAD_PARM;

  if (grp == 8) {
    *mac_stn_id = 0;
    return LLD_OK;
  }
  if (grp == 9) {
    *mac_stn_id = grp_9_mac_stn_id[tile];
    return LLD_OK;
  }
  for (umac = 0; umac < 33; umac++) {
    if ((tiles[umac] == tile) && (grps[umac] == grp)) {
      *mac_stn_id = umac;
      return LLD_OK;
    }
  }
  return LLD_ERR_BAD_PARM;
}

/*******************************************************************
 * lld_sku_map_devport_from_user_to_device_safe
 *
 * Map a user specified devport to device specific devport
 * with check the mapping is possible on the device.
 * Returns LLD_ERR_BAD_PARM if mapping is not possible.
 *******************************************************************/
lld_err_t lld_sku_map_devport_from_user_to_device_safe(
    bf_dev_id_t dev_id, bf_dev_port_t user_port, bf_dev_port_t *dev_port) {
  lld_dev_t *dev_p = lld_map_dev_id_to_dev_p_allow_unassigned(dev_id);
  if (NULL == dev_port) return LLD_ERR_BAD_PARM;
  if (dev_p == NULL) return LLD_ERR_BAD_PARM;
  if (dev_p->dev_family == BF_DEV_FAMILY_TOFINO ||
      dev_p->dev_family == BF_DEV_FAMILY_TOFINO2) {
    *dev_port = user_port;
    return LLD_OK;
  }
  if (dev_p->dev_family == BF_DEV_FAMILY_TOFINO3) {
    uint32_t pipe = DEV_PORT_TO_PIPE(user_port);
    bf_dev_port_t local_port = DEV_PORT_TO_LOCAL_PORT(user_port);
    if (LLD_SKU_TOF3_DEVPORT_2BIT_TO_3BIT_CH(
            LLD_SKU_TOF3_DEVPORT_3BIT_TO_2BIT_CH(local_port)) == local_port) {
      *dev_port =
          MAKE_DEV_PORT(pipe, LLD_SKU_TOF3_DEVPORT_3BIT_TO_2BIT_CH(local_port));
      return LLD_OK;
    }
  }
  return LLD_ERR_BAD_PARM;
}

/*******************************************************************
 * lld_sku_map_devport_from_user_to_device
 *
 * Map a user specified devport to device specific devport
 *******************************************************************/
bf_dev_port_t lld_sku_map_devport_from_user_to_device(bf_dev_id_t dev_id,
                                                      bf_dev_port_t dev_port) {
  lld_dev_t *dev_p = lld_map_dev_id_to_dev_p_allow_unassigned(dev_id);
  if (dev_p == NULL) return dev_port;
  if (dev_p->dev_family == BF_DEV_FAMILY_TOFINO3) {
    uint32_t pipe = DEV_PORT_TO_PIPE(dev_port);
    bf_dev_port_t local_port = DEV_PORT_TO_LOCAL_PORT(dev_port);
    bf_dev_port_t new_dev_port =
        MAKE_DEV_PORT(pipe, LLD_SKU_TOF3_DEVPORT_3BIT_TO_2BIT_CH(local_port));
    return new_dev_port;
  }
  return dev_port;
}

/*******************************************************************
 * lld_sku_map_devport_from_device_to_user
 *
 * Map a device specific devport to user devport
 *******************************************************************/
bf_dev_port_t lld_sku_map_devport_from_device_to_user(bf_dev_id_t dev_id,
                                                      bf_dev_port_t dev_port) {
  lld_dev_t *dev_p = lld_map_dev_id_to_dev_p_allow_unassigned(dev_id);
  if (dev_p == NULL) return dev_port;
  if (dev_p->dev_family == BF_DEV_FAMILY_TOFINO3) {
    uint32_t pipe = DEV_PORT_TO_PIPE(dev_port);
    bf_dev_port_t local_port = DEV_PORT_TO_LOCAL_PORT(dev_port);
    bf_dev_port_t new_dev_port =
        MAKE_DEV_PORT(pipe, LLD_SKU_TOF3_DEVPORT_2BIT_TO_3BIT_CH(local_port));
    return new_dev_port;
  }
  return dev_port;
}

#if defined(DEVICE_IS_EMULATOR)
bool lld_serdes_56g_mode = false;
#endif

/*************************************************
 * num of serdes lanes per lane supported
 *
 *************************************************/
int lld_get_num_serdes_per_mac(bf_dev_id_t dev_id, bf_dev_port_t dev_port) {
  lld_dev_t *dev_p = lld_map_dev_id_to_dev_p_allow_unassigned(dev_id);
  bf_dev_port_t min_cpu = lld_get_min_cpu_port(dev_id);
  bf_dev_port_t max_cpu = lld_get_max_cpu_port(dev_id);
  uint32_t mac = 0, ch;
  bf_subdev_id_t subdev_id = 0;

  if (dev_p == NULL) return 1;
  if (dev_p->dev_family != BF_DEV_FAMILY_TOFINO3) {
    return 1;
  }

#if defined(DEVICE_IS_EMULATOR)
#if defined(DEVICE_IS_EMULATOR_SERDES_56G)
  lld_serdes_56g_mode = true;
#endif
  if (lld_serdes_56g_mode) {
    return 8;
  } else {
    return 4;
  }
#endif

  if (dev_port >= min_cpu && dev_port <= max_cpu) {
    return 4;
  }

  if (DEV_PORT_TO_PIPE(dev_port) > 3) {
    subdev_id = 1;
  }
  if (lld_tof3_sku_map_dev_port_id_to_mac_ch(dev_id, dev_port, &mac, &ch) !=
      LLD_OK) {
    return 0;
  }

  // skip mac-0 on die-1
  if (mac > 32) mac = (mac % 33) + 1;

  return ((lld_efuse_get_serdes_dis_even(dev_id, subdev_id) |
           lld_efuse_get_serdes_dis_odd(dev_id, subdev_id)) &
          (1UL << (mac - 1)))
             ? 4
             : 8;
}
