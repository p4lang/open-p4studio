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
#include <asm/io.h>
#include <linux/pci.h>
#include <linux/etherdevice.h>
#include <linux/netdevice.h>
#include <bf_types/bf_types.h>
#include <dvm/bf_drv_intf.h>
#include <dvm/bf_dma_types.h>
#include <lld/bf_dma_if.h>
#include <lld/lld_dr_descriptors.h>
#include <lld/lld_dr_if.h>
#include <lld/lld_reg_if.h>
#include <lld/bf_int_if.h>
#include <lld/lld_err.h>
#include <lld/bf_lld_if.h>
#include "bf_kpkt_priv.h"
#include <linux/version.h>

typedef bf_status_t (*bf_reg_wr_fn)(bf_dev_id_t dev_id, bf_subdev_id_t subdev_id, u32 addr, u32 data);
typedef bf_status_t (*bf_reg_rd_fn)(bf_dev_id_t dev_id, bf_subdev_id_t subdev_id, u32 addr, u32 *data);

static struct bf_kpkt_adapter *chip_to_adapter[BF_MAX_DEV_COUNT]
                                              [BF_MAX_SUBDEV_COUNT];
extern void lld_init(bool is_master, bf_reg_wr_fn wr_fn, bf_reg_rd_fn rd_fn);
extern bf_status_t lld_master_dev_add(bf_dev_id_t dev_id,
                                      bf_dev_family_t dev_family,
                                      struct bf_dma_info_s *dma_info,
                                      bf_dev_init_mode_t warm_init_mode);
extern void lld_dr_init_dev(bf_dev_id_t dev_id, bf_subdev_id_t subdev_id, struct bf_dma_info_s *dma_info);
extern bf_status_t lld_dev_remove(bf_dev_id_t dev_id);
static void bf_kpkt_tbus_setup(u8 dev_id);
extern bool lld_dev_ready(bf_dev_id_t dev_id, bf_subdev_id_t subdev_id);

void bf_kpkt_chip_adapter_set(bf_dev_id_t dev_id, bf_subdev_id_t subdev_id,
                              struct bf_kpkt_adapter *adapter) {
  if (dev_id < 0 || dev_id >= BF_MAX_DEV_COUNT) {
    return;
  }
  if (subdev_id < 0 || subdev_id >= BF_MAX_SUBDEV_COUNT) {
    return;
  }
  chip_to_adapter[dev_id][subdev_id] = adapter;
}
static bf_status_t bf_reg_read(bf_dev_id_t dev_id, bf_subdev_id_t subdev_id, u32 reg, u32 *val) {
  if (!chip_to_adapter[dev_id][subdev_id]) {
    *val = BF_KPKT_FAILED_READ_REG;
    return BF_INVALID_ARG;
  }
  *val = bf_kpkt_read_reg(&chip_to_adapter[dev_id][subdev_id]->hw, reg, true);
  return BF_SUCCESS;
}

static bf_status_t bf_reg_write(bf_dev_id_t dev_id, bf_subdev_id_t subdev_id, u32 reg, u32 val) {
  if (!chip_to_adapter[dev_id][subdev_id]) {
    return BF_INVALID_ARG;
  }
  bf_kpkt_write_reg(&chip_to_adapter[dev_id][subdev_id]->hw, reg, val);
  return BF_SUCCESS;
}

static inline void setp_tbus_flush_flush(u32 *csr, u32 value) {
  *csr = (value & 0x1ul) | (*csr & (~0x1ul));
}

static inline void setp_tbus_ctrl_crcerr_keep(uint32_t *csr, uint32_t value) {
  *csr = ((value & 0x1ul) << 26) | (*csr & (~(0x1ul << 26)));
}//only for tof2

static inline void setp_tbus_ctrl_rx_channel_offset(uint32_t *csr, uint32_t value) {
  *csr = ((value & 0xful) << 22) | (*csr & (~(0xful << 22)));
}//only for tof2

static inline void setp_tbus_ctrl_port_alive(uint32_t *csr, uint32_t value) {
  *csr = ((value & 0x1ul) << 21) | (*csr & (~(0x1ul << 21)));
}

static inline void setp_tbus_ctrl_rx_en(uint32_t *csr, uint32_t value) {
  *csr = ((value & 0x1ul) << 20) | (*csr & (~(0x1ul << 20)));
}

static inline void setp_tbus_ctrl_ecc_dec_dis(uint32_t *csr, uint32_t value) {
  *csr = ((value & 0x1ul) << 16) | (*csr & (~(0x1ul << 16)));
}

static inline void setp_tbus_ctrl_pfc_rx(uint32_t *csr, uint32_t value) {
  *csr = ((value & 0xfful) << 8) | (*csr & (~(0xfful << 8)));
}

static inline void setp_tbus_ctrl_pfc_fm(uint32_t *csr, uint32_t value) {
  *csr = (value & 0xfful) | (*csr & (~0xfful));
}

static inline void setp_tbus_ctrl_crcchk_dis(uint32_t *csr, uint32_t value) {
  *csr = ((value & 0x1ul) << 17) | (*csr & (~(0x1ul << 17)));
}

static inline void setp_tbus_ctrl_crcrmv_dis(uint32_t *csr, uint32_t value) {
  *csr = ((value & 0x1ul) << 18) | (*csr & (~(0x1ul << 18)));
}

/* bf_kpkt_enable_all_dr
 *
 * Enables all the DRs used by the pkt manager
 */
static void bf_kpkt_enable_all_dr(bf_dev_id_t dev_id) {
  bf_dma_dr_id_t dr_id;

  /* Enable lld_dr_fm_pkt_0,1,2,3,4,5,6,7 */
  for (dr_id = lld_dr_fm_pkt_0; dr_id <= lld_dr_fm_pkt_7; dr_id++) {
    lld_dr_enable_set(dev_id, dr_id, true);
  }

  /* Enable lld_dr_tx_pkt_0,1,2,3 */
  for (dr_id = lld_dr_tx_pkt_0; dr_id <= lld_dr_tx_pkt_3; dr_id++) {
    lld_dr_enable_set(dev_id, dr_id, true);
  }

  /* Enable lld_dr_cmp_tx_pkt_0,1,2,3 */
  for (dr_id = lld_dr_cmp_tx_pkt_0; dr_id <= lld_dr_cmp_tx_pkt_3; dr_id++) {
    lld_dr_enable_set(dev_id, dr_id, true);
  }

  /* Enable lld_dr_rx_pkt_0,1,2,3,4,5,6,7 */
  for (dr_id = lld_dr_rx_pkt_0; dr_id <= lld_dr_rx_pkt_7; dr_id++) {
    lld_dr_enable_set(dev_id, dr_id, true);
  }
}

/* bf_kpkt_disable_all_dr
 *
 * Disables all the DRs used by the pkt manager
 */
static void bf_kpkt_disable_all_dr(bf_dev_id_t dev_id) {
  bf_dma_dr_id_t dr_id;

  /* Disable lld_dr_fm_pkt_0,1,2,3,4,5,6,7 */
  for (dr_id = lld_dr_fm_pkt_0; dr_id <= lld_dr_fm_pkt_7; dr_id++) {
    lld_dr_enable_set(dev_id, dr_id, false);
  }

  /* Disable lld_dr_tx_pkt_0,1,2,3 */
  for (dr_id = lld_dr_tx_pkt_0; dr_id <= lld_dr_tx_pkt_3; dr_id++) {
    lld_dr_enable_set(dev_id, dr_id, false);
  }

  /* Disable lld_dr_cmp_tx_pkt_0,1,2,3 */
  for (dr_id = lld_dr_cmp_tx_pkt_0; dr_id <= lld_dr_cmp_tx_pkt_3; dr_id++) {
    lld_dr_enable_set(dev_id, dr_id, false);
  }

  /* Disable lld_dr_rx_pkt_0,1,2,3,4,5,6,7 */
  for (dr_id = lld_dr_rx_pkt_0; dr_id <= lld_dr_rx_pkt_7; dr_id++) {
    lld_dr_enable_set(dev_id, dr_id, false);
  }
}

static int bf_tbus_set_ts_offset(u8 dev_id, u16 offset) {
  u32 tbus_off; /* tbus-ts offset */
  u32 val = 0;
  if (bf_lld_dev_is_tof1(dev_id)) {
    tbus_off = 0x180018ul; /* tbus-ts offset */
  } else if (bf_lld_dev_is_tof1(dev_id)) {
    tbus_off = 0x30001cul; /* tbus-ts offset */
  } else {
    return -1;
  }
  setp_tbus_flush_flush(&val, offset);
  lld_write_register(dev_id, tbus_off, val);
  return 0;
}

#ifdef BF_TBUS_FLUSH_DMA_NEEDED
static int bf_tbus_flush_dma(u8 dev_id) {
  u32 tbus_off; /* tbus-flush offset */
  u32 val = 0;
  if (dev_id >= BF_MAX_DEV_COUNT) {
    return BF_INVALID_ARG;
  }
  if (bf_kpkt_is_locked(chip_to_adapter[dev_id][0])) {
    return -1;
  }
  if (bf_lld_dev_is_tof1(dev_id)) {
    tbus_off = 0x180014ul; /* tbus-flush offset */
  } else if (bf_lld_dev_is_tof2(dev_id)) {
    tbus_off = 0x300014ul; /* tbus-flush offset */
  } else if (bf_lld_dev_is_tof3(dev_id)) {
    tbus_off = 0x300014ul; /* tbus-flush offset */
  } else {
    return -1;
  }
  setp_tbus_flush_flush(&val, 1);
  lld_write_register(dev_id, tbus_off, val);
  return 0;
}
#endif

static int bf_tbus_int_en(u8 dev_id, int int_id, int high_prio, u32 *val,
                          int get) {
  u32 tbus_off;
  if (bf_lld_dev_is_tof1(dev_id)) {
    switch (int_id) {
      case 0:
        if (high_prio) {
          tbus_off = 0x18002cul;
        } else {
          tbus_off = 0x180028ul;
        }
        break;
      case 1:
        if (high_prio) {
          tbus_off = 0x180034ul;
        } else {
          tbus_off = 0x180030ul;
        }
        break;
      case 2:
        if (high_prio) {
          tbus_off = 0x18003cul;
        } else {
          tbus_off = 0x180038ul;
        }
        break;
      default:
        return -1;
    }
  } else if (bf_lld_dev_is_tof2(dev_id)) {
    switch (int_id) {
      case 0:
        if (high_prio) {
          tbus_off = 0x300030ul;
        } else {
          tbus_off = 0x30002cul;
        }
        break;
      case 1:
        if (high_prio) {
          tbus_off = 0x300038ul;
        } else {
          tbus_off = 0x300034ul;
        }
        break;
      case 2:
        if (high_prio) {
          tbus_off = 0x300040ul;
        } else {
          tbus_off = 0x30003cul;
        }
        break;
      default:
        return -1;
    }
  } else if (bf_lld_dev_is_tof3(dev_id)) {
    switch (int_id) {
      case 0:
        if (high_prio) {
          tbus_off = 0x300030ul;
        } else {
          tbus_off = 0x30002cul;
        }
        break;
      case 1:
        if (high_prio) {
          tbus_off = 0x300038ul;
        } else {
          tbus_off = 0x300034ul;
        }
        break;
      case 2:
        if (high_prio) {
          tbus_off = 0x300040ul;
        } else {
          tbus_off = 0x30003cul;
        }
        break;
      default:
        return -1;
    }
  } else {
    return -1;
  }
  if (get) {
    lld_read_register(dev_id, tbus_off, val);
  } else {
    lld_write_register(dev_id, tbus_off, *val);
  }
  return 0;
}

static int bf_tbus_int_st(u8 dev_id, int int_id, u32 *val, int get) {
  u32 tbus_off;
  if (bf_lld_dev_is_tof1(dev_id)) {
    switch (int_id) {
      case 0:
        tbus_off = 0x18001cul;
        break;
      case 1:
        tbus_off = 0x180020ul;
        break;
      case 2:
        tbus_off = 0x180024ul;
        break;
      default:
        return -1;
    }
  } else if (bf_lld_dev_is_tof2(dev_id)) {
    switch (int_id) {
      case 0:
        tbus_off = 0x300020ul;
        break;
      case 1:
        tbus_off = 0x300024ul;
        break;
      case 2:
        tbus_off = 0x300028ul;
        break;
      default:
        return -1;
    }
  } else if (bf_lld_dev_is_tof3(dev_id)) {
    switch (int_id) {
      case 0:
        tbus_off = 0x300020ul;
        break;
      case 1:
        tbus_off = 0x300024ul;
        break;
      case 2:
        tbus_off = 0x300028ul;
        break;
      default:
        return -1;
    }
  } else {
    return -1;
  }
  if (get) {
    lld_read_register(dev_id, tbus_off, val);
  } else {
    lld_write_register(dev_id, tbus_off, *val);
  }
  return 0;
}

int bf_tbus_clr_int(u8 dev_id, int int_id) {
  u32 val = 0xfffffffful;

  /* clear all possible interrupt status, for now */
  bf_tbus_int_st(dev_id, int_id, &val, 0);
  return 0;
}

static int bf_tbus_config(u8 dev_id, bf_kpkt_tbus_cfg_t *tbus_cfg) {
  u32 tmp;
  u32 tbus_off; /* tbus-ctrl */
  u32 val;
  if (bf_lld_dev_is_tof1(dev_id)) {
    tbus_off = 0x180010ul; /* tbus-ctrl */
    lld_read_register(dev_id, tbus_off, &val);
  } else if (bf_lld_dev_is_tof2(dev_id)) {
    tbus_off = 0x300010ul; /* tbus-ctrl */
    lld_read_register(dev_id, tbus_off, &val);
    /* RX channel byte offset */
    tmp = (tbus_cfg->rx_channel_offset)&0x0f;
    setp_tbus_ctrl_rx_channel_offset(&val, tmp);
    /* enable keeping the CRC of input error pkt */
    tmp = (tbus_cfg->crcerr_keep) ? 1 : 0;
    setp_tbus_ctrl_crcerr_keep(&val, tmp);    
  } else if (bf_lld_dev_is_tof3(dev_id)) {
    tbus_off = 0x300010ul; /* tbus-ctrl */
    lld_read_register(dev_id, tbus_off, &val);
    /* RX channel byte offset */
    tmp = (tbus_cfg->rx_channel_offset)&0x0f;
    setp_tbus_ctrl_rx_channel_offset(&val, tmp);
    /* enable keeping the CRC of input error pkt */
    tmp = (tbus_cfg->crcerr_keep) ? 1 : 0;
    setp_tbus_ctrl_crcerr_keep(&val, tmp);    
  } else {
    return -1;
  }
  /* tbus ctrl register */
  /* port aliveness */
  tmp = (tbus_cfg->port_alive) ? 1 : 0;
  setp_tbus_ctrl_port_alive(&val, tmp);
  /* port enable */
  tmp = (tbus_cfg->port_en) ? 1 : 0;
  setp_tbus_ctrl_rx_en(&val, tmp);
  /* leave all crc settings as defaults */
  /* ecc disable */
  tmp = (tbus_cfg->ecc_dis) ? 1 : 0;
  setp_tbus_ctrl_ecc_dec_dis(&val, tmp);
  /* PFX rx threshold */
  tmp = tbus_cfg->pfc_rx_thr;
  setp_tbus_ctrl_pfc_rx(&val, tmp);
  /* PFX fm threshold */
  tmp = tbus_cfg->pfc_fm_thr;
  setp_tbus_ctrl_pfc_fm(&val, tmp);

  setp_tbus_ctrl_crcchk_dis(&val, 1);
  /* Disable CRC removal */
  tmp = (tbus_cfg->crcrmv_dis) ? 1 : 0;
  setp_tbus_ctrl_crcrmv_dis(&val, tmp);
  
  lld_write_register(dev_id, tbus_off, val);
  bf_tbus_set_ts_offset(dev_id, tbus_cfg->ts_offset);

  return 0;
}

void bf_kpkt_dma_service_tx_ring_pkt(u8 dev_id, int ring_index, int count) {
  if (ring_index < BF_PKT_TX_RING_MAX) {
    lld_dr_service(dev_id, 0, lld_dr_cmp_tx_pkt_0 + ring_index, count);
  }
}

void bf_kpkt_dma_service_all_tx_pkt(u8 dev_id, int count) {
  int i;

  for (i = 0; i < BF_PKT_TX_RING_MAX; i++) {
    lld_dr_service(dev_id, 0, lld_dr_cmp_tx_pkt_0 + i, count);
  }
}

void bf_kpkt_dma_service_rx_ring_pkt(u8 dev_id, int ring_index, int count) {
  if (ring_index < BF_PKT_RX_RING_MAX) {
    lld_dr_service(dev_id, 0, lld_dr_rx_pkt_0 + ring_index, count);
  }
}

void bf_kpkt_dma_service_all_rx_pkt(u8 dev_id, int count) {
  int i;

  for (i = 0; i < BF_PKT_RX_RING_MAX; i++) {
    lld_dr_service(dev_id, 0, lld_dr_rx_pkt_0 + i, count);
  }
}

static void bf_kpkt_tbus_setup(u8 dev_id) {
  bf_kpkt_tbus_cfg_t tbus_cfg;
  uint32_t val = 0;

  if (dev_id >= BF_MAX_DEV_COUNT) {
    return;
  }
  if (bf_kpkt_is_locked(chip_to_adapter[dev_id][0])) {
    return;
  }
  /* default initialization of tbus */
  tbus_cfg.pfc_fm_thr = 0x3;
  tbus_cfg.pfc_rx_thr = 0x3;
  tbus_cfg.ecc_dis = 0;
  tbus_cfg.port_en = 1;
  tbus_cfg.port_alive = 1;
  tbus_cfg.ts_offset = 0;
  tbus_cfg.crcrmv_dis = 0;
  /* only for Tof2, set to default valuse*/
  tbus_cfg.rx_channel_offset = 0;
  tbus_cfg.crcerr_keep = 1;
  bf_tbus_config(dev_id, &tbus_cfg);
  /* disable all tbus interrupts */
  bf_tbus_int_en(dev_id, 0, 0, &val, 0);
  bf_tbus_int_en(dev_id, 0, 1, &val, 0);
  bf_tbus_int_en(dev_id, 1, 0, &val, 0);
  bf_tbus_int_en(dev_id, 1, 1, &val, 0);
  bf_tbus_int_en(dev_id, 2, 0, &val, 0);
  bf_tbus_int_en(dev_id, 2, 1, &val, 0);
}

bool bf_kpkt_is_cpu_pkt_en(struct bf_kpkt_adapter *adapter) {
  if (!adapter) {
    printk(KERN_ERR "error in bf_kpkt_is_cpu_pkt_en\n");
    return false;
  }
  return (adapter->en_cpu_pkt);
}

int bf_kpkt_init_tbus(bf_dev_id_t dev_id) {
  struct bf_kpkt_adapter *adapter;
  bf_status_t status;
  int i;

  if (dev_id >= BF_MAX_DEV_COUNT) {
    printk(KERN_ERR "error in bf_kpkt_init_tbus dev_id %d\n", dev_id);
    return -1;
  }
  adapter = chip_to_adapter[dev_id][0];
  if (adapter->tbus_inited) {
    return 0; /* nothing to do */
  }
  bf_kpkt_tbus_setup(dev_id);
  bf_kpkt_enable_all_dr(dev_id);

  /* drain the messages that might have been stuck in tofino prior
   * to a warm restart */
  for (i = 0; i < BF_PKT_TX_RING_MAX; i++) {
    lld_dr_service(dev_id, 0, lld_dr_cmp_tx_pkt_0 + i, 10000);
  }
  if (adapter->pkt_int_enable) {
    /* Claim the pkt_mgr interrupts */
    status = bf_int_claim_tbus(dev_id, 0);
    if (status != BF_SUCCESS) {
      printk(KERN_ERR "interrupt registration with LLD failed for device %d, sts %d", dev_id, status);
      return status;
    }

    /* Enable the pkt_mgr interrupts */
    status = bf_int_ena_tbus(dev_id, 0);
    if (status != BF_SUCCESS) {
      printk(KERN_ERR "interrupt enable with LLD failed for device %d, sts %d",
              dev_id,
              status);
      return status;
    }
  }
  adapter->tbus_inited = true;
  /* Mark cpu pkt path as enabled */
  adapter->en_cpu_pkt = true;
  return 0;
}

static int bf_skb_frag_add(struct sk_buff *skb, struct sk_buff *tail) {
  size_t fragsz;
  void *frag;
  struct page *page;
  int offset;

  if (!skb || !tail) {
    return -1;
  }
  if (skb_shinfo(skb)->nr_frags >= MAX_SKB_FRAGS) {
    printk(KERN_ERR "bf_kpkt error nr_frags maxed out.\n");
    return -1;
  }
  fragsz = tail->len;
  if (fragsz == 0 || fragsz > PAGE_SIZE) {
    printk(KERN_ERR "bf_kpkt error frag size.\n");
    return -1;
  }
#if LINUX_VERSION_CODE > KERNEL_VERSION(3, 19, 0)
  frag = napi_alloc_frag(fragsz);
#else
  frag = netdev_alloc_frag(fragsz);
#endif
  if (!frag) {
    printk(KERN_ERR "bf_kpkt error allocating frag\n");
    return -1;
  }
  page = virt_to_head_page(frag);
  offset = frag - page_address(page);
  memcpy((u8 *)frag, tail->data, fragsz);
  skb_add_rx_frag(skb, skb_shinfo(skb)->nr_frags, page, offset, fragsz, fragsz);
  return 0;
}

/* check if the physical address corresponding to the virtual address
 * is aligned. <align> is 16,32,256 etc., a power of two.
 */
static bool bf_kpkt_is_aligned(u8 *cptr, unsigned long align) {
  /* Least significant 12 bits are same in virtual and physical addresses */
  if ((unsigned long)cptr & (align - 1)) {
    return false;
  } else {
    return true;
  }
}
/* allocates an skb  with head_room and data pointing to the specific alignment.
 * align_ment must be a power of two. copies data to newly allocated skb.
 */
static struct sk_buff *copy_buff_new_skb(u8 *buff,
                                         u16 size,
                                         u16 hd_room,
                                         long align) {
  struct sk_buff *skb;
  u8 *aligned_addr;
  unsigned int len;

  if (align & (align - 1)) {
    /* align is not a power of 2 */
    return NULL;
  }
  skb = dev_alloc_skb(size + hd_room + align);
  if (!skb) {
    return NULL;
  }
  aligned_addr = get_next_aligned_addr(skb->data, hd_room, align, size);
  len = (unsigned int)(aligned_addr - skb->data);
  skb_reserve(skb, len);
  memcpy(skb->data, buff, size);
  skb_put(skb, size);
  return skb;
}

/* checks if the dma_addr at "next_rx_index" is the one is parameter */
static bool dma_addr_is_match(struct bf_kpkt_ring *rx_ring,
                              bf_dma_addr_t dma_addr) {
   if (rx_ring->rx_buffer_info[rx_ring->next_rx_index].dma == dma_addr) {
     return true;
   }
   return false;
}

/* find the next index in rx_ring where stored dma_addr matches */
static int dma_addr_find_next(struct bf_kpkt_ring *rx_ring,
                              bf_dma_addr_t dma_addr) {
   int i;
   for (i = rx_ring->next_rx_index; i < rx_ring->count; i++) {
     if (rx_ring->rx_buffer_info[i].dma == dma_addr) {
       return i;
     }
   }
   for (i = 0; i < rx_ring->next_rx_index; i++) {
     if (rx_ring->rx_buffer_info[i].dma == dma_addr) {
       return i;
     }
   }
   return -1;
}

/**
 *
 * Callback functions to be registered with the LLD Client Library for the
 * notification FIFOs. Allocate a new skb to ring's free meme pool after
 * each error-free callback iteration.
 *
 */
static void pkt_mgr_rx_pkt_cb(
    bf_dev_id_t dev_id, bf_subdev_id_t subdev_id, int data_sz, bf_dma_addr_t dma_addr, int s, int e, int cos) {
  struct sk_buff *skb;
  struct bf_kpkt_adapter *adapter;
  struct bf_kpkt_ring *rx_ring;
  unsigned long magic_wd;
  int buff_index;
  struct bf_kpkt_skb_meta_data *meta_data;
  (void) subdev_id;

  if (dev_id >= BF_MAX_DEV_COUNT || !dma_addr) {
    printk(
        KERN_ERR "error in rx_cb dev_id %d dma_addr %llx\n", dev_id, dma_addr);
    return;
  }

  adapter = chip_to_adapter[dev_id][0];

  if (cos >= MAX_RX_QUEUES) {
    dev_err(adapter->dev, "rx_cb bad cos %d\n", cos);
    cos = cos % (MAX_RX_QUEUES);
  }
  rx_ring = adapter->rx_ring[cos];
  if (!rx_ring) {
    /* this must never happen, but kernel must not crash */
    dev_err(adapter->dev, "error bad rx_ring ptr for cos %d\n", cos);
    adapter->rx_drops++;
    return;
  }

  /* Unmap the DMA address and get the virtual address of the buffer before
   * it is processed by the packet manager
   */
  dma_unmap_single(
      adapter->dev, dma_addr, rx_ring->rx_buf_len, DMA_FROM_DEVICE);
#if 0  /* little faster code that can be used only if iommu is disabled */
  u8 *address = bus_to_virt(dma_addr);
  if (address == NULL) {
    rx_ring->stats.errors++;
    dev_err(adapter->dev, "error invalid virtual addr in rx_cb dr %d\n", cos);
    return;
  }
  /* SKB was previously written just before "address"
   * perform sanity check by checking against the presence of
   * magic word before "address"
   */
  meta_data =
      (struct bf_kpkt_skb_meta_data *)(address -
                                       (sizeof(struct bf_kpkt_skb_meta_data)));
  skb = meta_data->skb;
  magic_wd = meta_data->magic_wd;
  buff_index = meta_data->buff_index;

#else
  /* this code must be used if iommu is enabled
   * and also with little performance hit when iommu is disabled
   */
  if (!dma_addr_is_match(rx_ring, dma_addr)) {
    int temp_index;
    printk(KERN_ERR "BF rx_dma mismatch on dma address ring %d index %d !\n",
           cos, rx_ring->next_rx_index);
    temp_index = dma_addr_find_next(rx_ring, dma_addr);
    if (temp_index < 0 ) {
      printk(KERN_ERR "error finding a matching dma address ring %d index %d\n",
             cos, rx_ring->next_rx_index);
      return;
    }
    rx_ring->next_rx_index = temp_index;
  }
  /* mark dma_unmapped buffer as so */
  rx_ring->rx_buffer_info[rx_ring->next_rx_index].dma = 0;

  skb = rx_ring->rx_buffer_info[rx_ring->next_rx_index].skb;
  /* SKB was previously written just before "address"
   * perform sanity check by checking against the presence of
   * magic word before "address"
   */
  meta_data =
      (struct bf_kpkt_skb_meta_data *)(skb->data -
                                       (sizeof(struct bf_kpkt_skb_meta_data)));
  magic_wd = meta_data->magic_wd;
  buff_index = rx_ring->next_rx_index;

  rx_ring->next_rx_index++;
  if (rx_ring->next_rx_index >= rx_ring->count) {
    rx_ring->next_rx_index = 0;
  }
#endif

  skb_put(skb, data_sz);

  if (magic_wd != ((unsigned long)skb & BF_KPKT_MAGIC_WD_RX)) {
    /* something went bad, free the skb anyway and log error */
    rx_ring->stats.errors++;
    dev_err(adapter->dev, "error bad magic word in rx_cb ring %d\n", cos);
    dev_kfree_skb_any(skb);
    return;
  }

  switch (rx_ring->pkt_status) {
    case BF_PKT_INIT:
      rx_ring->pkt_first = skb;
      if (s != 1) {
        dev_err(adapter->dev,
                "error incorrect start in rx_cb cos %d s:e %d:%d",
                cos,
                s,
                e);
        goto err_and_exit;
      }
      /* mark the rx_buffer free. skb would be freed by kernel stack */
      bf_kpkt_set_rx_ring_buff_free(adapter, rx_ring, buff_index);
      rx_ring->stats.bytes += data_sz;
      adapter->rx_pkt_processed++;
      if (e == 0) {
        /* this is not the end packet, prepare for chaining it */
        rx_ring->pkt_status = BF_PKT_START;
        bf_kpkt_alloc_rx_buffers(adapter, rx_ring, 1, adapter->head_room);
      } else {
        /* reset the packet building state */
        rx_ring->stats.packets++;
        rx_ring->pkt_status = BF_PKT_INIT;
        skb->protocol = eth_type_trans(skb, adapter->netdev);
        skb->priority = cos;
        skb->dev = adapter->netdev;
        skb->ip_summed = CHECKSUM_UNNECESSARY;
        netif_receive_skb(rx_ring->pkt_first);
        bf_kpkt_alloc_rx_buffers(adapter, rx_ring, 1, adapter->head_room);
      }
      return;
    case BF_PKT_START:
      if (s != 0) {
        dev_err(adapter->dev,
                "error incorrect middle rx_cb cos %d s:e %d:%d",
                cos,
                s,
                e);
        goto err_and_exit;
      }
      if(bf_skb_frag_add(rx_ring->pkt_first, skb) == 0) {
        dev_kfree_skb_any(skb);
      } else {
        goto err_and_exit;
      }
      /* mark the irx_buffer free. skb would be freed by kernel stack */
      bf_kpkt_set_rx_ring_buff_free(adapter, rx_ring, buff_index);
      rx_ring->stats.bytes += data_sz;
      adapter->rx_pkt_processed++;
      if (e == 0) {
        /* this is not the end packet, chain it */
        bf_kpkt_alloc_rx_buffers(adapter, rx_ring, 1, adapter->head_room);
      } else {
        /* this is the last fragment in the packet */
        /* reset the packet building state */
        rx_ring->stats.packets++;
        rx_ring->pkt_status = BF_PKT_INIT;
        skb = rx_ring->pkt_first;
        skb->protocol = eth_type_trans(skb, adapter->netdev);
        skb->priority = cos;
        skb->dev = adapter->netdev;
        skb->ip_summed = CHECKSUM_NONE;
        netif_receive_skb(rx_ring->pkt_first);
        bf_kpkt_alloc_rx_buffers(adapter, rx_ring, 1, adapter->head_room);
      }
      return;
    default:
      dev_err(adapter->dev, "bad rx status %x\n", rx_ring->pkt_status);
      goto err_and_exit;
  }

err_and_exit:
  /* free up all pkts in the chain and reset the pkt state to init */
  rx_ring->stats.drops++;
  bf_kpkt_free_mapped_skb(adapter, rx_ring, buff_index);
  rx_ring->pkt_status = BF_PKT_INIT;
  bf_kpkt_alloc_rx_buffers(adapter, rx_ring, 1, adapter->head_room);
  return;
}

/* returns non zero if any of the rx packet DR has an entry to process */
unsigned int bf_kpkt_check_rxpkt_dr_entry(bf_dev_id_t dev_id) {
  int i;
  unsigned int ret;

  for (i = 0; i < BF_PKT_RX_RING_MAX; i++) {
    ret = lld_dr_used_get (dev_id, lld_dr_rx_pkt_0 + i);
    if (ret) {
      return ret;
    }
  }

  return 0;
}

/* returns non zero if any of the tx packet DR has an entry to process */
unsigned int bf_kpkt_check_txpkt_dr_entry(bf_dev_id_t dev_id) {
  int i;
  unsigned int ret;

  for (i = 0; i < BF_PKT_TX_RING_MAX; i++) {
    ret = lld_dr_used_get (dev_id, lld_dr_cmp_tx_pkt_0 + i);
    if (ret) {
      return ret;
    }
  }

  return 0;
}

netdev_tx_t bf_kpkt_xmit_frame_ring(struct sk_buff *skb,
                                    struct bf_kpkt_adapter *adapter,
                                    struct bf_kpkt_ring *tx_ring) {
  struct sk_buff *first_skb;
  dma_addr_t dma_addr;
  unsigned short f;
  u16 count;
  u8 frag_count;
  unsigned int available_space;
  int start, end, i, xmit_error = 0, free_orig_skb = 0;

  /* check if tx DR has space */
  available_space =
      lld_dr_unused_get(adapter->dev_id, lld_dr_tx_pkt_0 + tx_ring->ring_index);
  if (available_space < (skb_shinfo(skb)->nr_frags + 2)) {
    /* the data part in sk_buff may need to split into two descriptors due
     * to 256 bytes alignment requirement.
     */
    return NETDEV_TX_BUSY;
  }
  /*
   * find total data size to xmit
   */
  count = 0;
  for (f = 0; f < skb_shinfo(skb)->nr_frags; f++) {
    count += skb_frag_size(&(skb_shinfo(skb)->frags[f]));
  }
  /* start with sk_buff->len part. Check alignment.
   * if not-aligned, then,
   * strip the beginning part of unaligned data into the first aligned desc.
   * And,
   * aligned part of the data into the 2nd descriptor.
   * if aligned, then,
   * skbuff->data goes to the first descriptor
   */
  start = end = 1;
  if (!bf_kpkt_is_aligned(skb->data, 256)) {
    /* we hit the case of unaligned data in sk_buff */
    u8 *aligned_addr = get_next_aligned_addr(skb->data, 0, 256, 0);
    unsigned int len;
    end = 0;
    if (!aligned_addr) {
      /* bad error. return TX_OK as we cannot return anything better.
       * pkt gets dropped.
       */
      dev_kfree_skb_any(skb);
      xmit_error = 1;
      goto xmit_drop_err;
    } else {
      len = (unsigned int)(aligned_addr - skb->data);
      if (len >= skb_headlen(skb)) {
        len = skb_headlen(skb);
        if (count == 0) {
          end = 1; /* this is the only skb to transmit */
        }
      }
      first_skb = copy_buff_new_skb(skb->data, len, 0, 256);
      if (!first_skb) {
        dev_kfree_skb_any(skb);
        xmit_error = 2;
        goto xmit_drop_err;
      }
      /* make the original skb data to 256 alignment */
      skb_pull(skb, len);
    }
    /* send the the first_skb to tofino while preserving dma_addr in the CB area
     * of skb */
    dma_addr =
        dma_map_single(tx_ring->dev, first_skb->data, len, DMA_TO_DEVICE);
    if (dma_mapping_error(tx_ring->dev, dma_addr)) {
      dev_kfree_skb_any(first_skb);
      dev_kfree_skb_any(skb);
      xmit_error = 3;
      goto xmit_drop_err;
    }
    /* store the dma address in CB area. needed for later clean up */
    BF_KPKT_CB(first_skb)->dma = dma_addr;
    if (lld_push_tx_packet(adapter->dev_id,
                           tx_ring->ring_index,
                           start,
                           end,
                           len,
                           dma_addr,
                           (unsigned long)first_skb) != LLD_OK) {
      dma_unmap_single(tx_ring->dev, dma_addr, len, DMA_TO_DEVICE);
      dev_kfree_skb_any(first_skb);
      dev_kfree_skb_any(skb);
      xmit_error = 4;
      goto xmit_drop_err;
    }
    start = 0;
  }
  /* xmit the aligned (or the original) part of the skb */
  frag_count = skb_shinfo(skb)->nr_frags;
  end = (frag_count ? 0 : 1);

  if (skb_headlen(skb) > 0) {
    dma_addr = dma_map_single(tx_ring->dev, skb->data, skb_headlen(skb), DMA_TO_DEVICE);
    if (dma_mapping_error(tx_ring->dev, dma_addr)) {
      dev_kfree_skb_any(skb);
      xmit_error = 5;
      goto xmit_drop_err;
    }
    BF_KPKT_CB(skb)->dma = dma_addr;
    if (lld_push_tx_packet(adapter->dev_id,
                           tx_ring->ring_index,
                           start,
                           end,
                           skb_headlen(skb),
                           dma_addr,
                           (unsigned long)skb) != LLD_OK) {
      dma_unmap_single(tx_ring->dev, dma_addr, skb_headlen(skb), DMA_TO_DEVICE);
      dev_kfree_skb_any(skb);
      xmit_error = 5;
      goto xmit_drop_err;
    }
    start = 0;
  } else {
    free_orig_skb = 1; /* free  the orig skb after dealing with the frags */
  }
  /* now xmit the frags part of skb. we allocate a new skb and copy the data
   * in fragments into the skb.
   */
  i = 0;
  while (frag_count--) {
    struct sk_buff *frag_skb;
    u8 *frag_addr;
    dma_addr_t frag_dma;
    u16 frag_size;
    skb_frag_t *frag = &(skb_shinfo(skb)->frags[i]);

    end = (frag_count ? 0 : 1);
    frag_size = skb_frag_size(frag);
    /* fragments are not mapped pages. need to map, copy the data to new skb
     * and unmap.
     */
    frag_dma =
        skb_frag_dma_map(tx_ring->dev, frag, 0, frag_size, DMA_TO_DEVICE);
    frag_addr = skb_frag_address_safe(frag);
    if (!frag_addr) {
      dev_err(adapter->dev, "error bad frag address in tx skb\n");
      dma_unmap_page(tx_ring->dev, frag_dma, frag_size, DMA_TO_DEVICE);
      xmit_error = 6;
      goto xmit_drop_err;
    }
    frag_skb = copy_buff_new_skb(frag_addr, frag_size, 0, 256);
    if (!frag_skb) {
      dev_err(adapter->dev, "error allocating skb for frag data in tx\n");
      dma_unmap_page(tx_ring->dev, frag_dma, frag_size, DMA_TO_DEVICE);
      xmit_error = 7;
      goto xmit_drop_err;
    }
    /* frag data is copied on new skb. unmap frag data, now */
    dma_unmap_page(tx_ring->dev, frag_dma, frag_size, DMA_TO_DEVICE);
    /* now map the newly allocated skb */
    frag_dma =
        dma_map_single(tx_ring->dev, frag_skb->data, frag_size, DMA_TO_DEVICE);
    if (dma_mapping_error(tx_ring->dev, frag_dma)) {
      dev_kfree_skb_any(frag_skb);
      xmit_error = 8;
      goto xmit_drop_err;
    }
    BF_KPKT_CB(frag_skb)->dma = frag_dma;
    if (lld_push_tx_packet(adapter->dev_id,
                           tx_ring->ring_index,
                           start,
                           end,
                           frag_size,
                           frag_dma,
                           (unsigned long)frag_skb) != LLD_OK) {
      dma_unmap_single(tx_ring->dev, frag_dma, frag_size, DMA_TO_DEVICE);
      dev_kfree_skb_any(frag_skb);
      xmit_error = 9;
      goto xmit_drop_err;
    }
    i++;
  }
  lld_dr_start(adapter->dev_id, 0, lld_dr_tx_pkt_0 + tx_ring->ring_index);
  if (free_orig_skb) {
    dev_kfree_skb_any(skb);
  }
  return NETDEV_TX_OK;

xmit_drop_err:
  printk(KERN_ERR "bf_kpkt xmit error %d\n", xmit_error);
  lld_dr_start(adapter->dev_id, 0, lld_dr_tx_pkt_0 + tx_ring->ring_index);
  tx_ring->stats.drops++;
  if (free_orig_skb) {
    dev_kfree_skb_any(skb);
  }
  return NETDEV_TX_OK;
}

static void bf_kpkt_tx_completion_cb(bf_dev_id_t dev_id,
                                     bf_subdev_id_t subdev_id,
                                     bf_dma_dr_id_t dr,
                                     uint64_t data_sz_or_ts,
                                     u32 attr,
                                     u32 status,
                                     u32 type,
                                     u64 msg_id,
                                     int s,
                                     int e) {
  int ring_index = dr - lld_dr_cmp_tx_pkt_0;
  struct sk_buff *skb;
  struct bf_kpkt_adapter *adapter;
  struct bf_kpkt_ring *tx_ring;
  dma_addr_t dma_addr;

  (void)attr;
  (void)data_sz_or_ts;
  (void)subdev_id;

  if (dev_id >= BF_MAX_DEV_COUNT) {
    return;
  }
  adapter = chip_to_adapter[dev_id][0];

  /* drop the message if cpu pkt path is disabled. This can happen due a message
   * stuck in Tofino before the warm reboot begins. There is no valid buffer
   * corresponding to the msg_id in this case */
  if (!bf_kpkt_is_cpu_pkt_en(adapter)) {
    return; /* drop */
  }

  if (ring_index < 0 || ring_index >= MAX_TX_QUEUES) {
    dev_err(
        adapter->dev, "error bad ring index %d in tx completion\n", ring_index);
    return;
  }
  tx_ring = adapter->tx_ring[ring_index];
  if (!tx_ring) {
    /* this must never happen, but kernel must not crash */
    return;
  }

  if (type != tx_m_type_pkt) {
    dev_err(adapter->dev,
            "error bad tx completion type dev_id %d dr %d"
            "type %u status %u\n",
            dev_id,
            dr,
            type,
            status);
    tx_ring->stats.errors++;
    return;
  }
  skb = (struct sk_buff *)(uintptr_t)msg_id;
  if (!skb) {
    /* must never happen. log error */
    dev_err(adapter->dev, "error bad skb in tx compl dr %d\n", ring_index);
    tx_ring->stats.errors++;
    return;
  }

  switch (tx_ring->pkt_status) {
    case BF_PKT_INIT:
      if (s != 1) {
        dev_err(adapter->dev,
                "error bad start in tx completion dr %d\n",
                ring_index);
        tx_ring->stats.errors++;
        break;
      }
      tx_ring->stats.bytes += skb_headlen(skb);
      if (e == 0) {
        tx_ring->pkt_status = BF_PKT_START;
      } else {
        /* this packet is a complete packet. */
        tx_ring->stats.packets++;
      }
      break;
      ;

    case BF_PKT_START:
      if (s != 0) {
        dev_err(adapter->dev,
                "error bad start in middle of tx completion dr %d\n",
                ring_index);
        tx_ring->stats.errors++;
        break;
      }
      tx_ring->stats.bytes += skb_headlen(skb);
      if (e == 1) {
        /* this packet is a complete packet. */
        tx_ring->pkt_status = BF_PKT_INIT;
        tx_ring->stats.packets++;
      }
      break;
      ;
    default:
      break;
  }
  dma_addr = BF_KPKT_CB(skb)->dma;
  dma_unmap_single(tx_ring->dev, dma_addr, skb_headlen(skb), DMA_FROM_DEVICE);
  dev_kfree_skb_any(skb);
  return;
}

void bf_kpkt_lld_init(struct bf_kpkt_adapter *adapter) {
  bf_dev_id_t dev_id;

  if (!adapter) {
    printk(KERN_ERR "Bad adapter pointer in lld_init\n");
    return;
  }
  if (adapter->subdev_id != 0 ) {
    return; /* do it only for subdev_id == 0 */
  }
  dev_id = adapter->dev_id;
  if (dev_id < 0 || dev_id >= BF_MAX_DEV_COUNT) {
    printk(KERN_ERR "Bad dev_id in lld_init\n");
    return;
  }
  lld_init(true, bf_reg_write, bf_reg_read);
}

void bf_kpkt_pkt_deinit(bf_dev_id_t dev_id) {
  if (dev_id < 0 || dev_id >= BF_MAX_DEV_COUNT) {
    return;
  }
  if (lld_dev_ready(dev_id, 0) &&
      bf_int_msk_tbus(dev_id, 0) != BF_SUCCESS) {
    printk(KERN_ERR "interrupt disable with LLD failed for device %d", dev_id);
  }
  bf_kpkt_disable_all_dr(dev_id);
  chip_to_adapter[dev_id][0] = NULL;
}

void bf_kpkt_lld_master_deinit(struct bf_kpkt_adapter *adapter) {
  bf_dev_id_t dev_id;
  int i;

  if (!adapter) {
    printk(KERN_ERR "Bad adapter pointer in lld_deinit\n");
    return;
  }
  if (adapter->subdev_id != 0 ) {
    return; /* do it only for subdev_id == 0 */
  }
  dev_id = adapter->dev_id;
  if (dev_id < 0 || dev_id >= BF_MAX_DEV_COUNT) {
    printk(KERN_ERR "Bad dev_id in lld_deinit\n");
    return;
  }
  lld_dr_lock_not_required(dev_id, lld_dr_tx_pkt_0);
  lld_dr_lock_not_required(dev_id, lld_dr_tx_pkt_1);
  lld_dr_lock_not_required(dev_id, lld_dr_tx_pkt_2);
  lld_dr_lock_not_required(dev_id, lld_dr_tx_pkt_3);
  /* Un register callbacks with LLD */
  for (i = 0; i < BF_PKT_TX_RING_MAX; i++) {
    lld_register_completion_callback(dev_id, 0, lld_dr_cmp_tx_pkt_0 + i, NULL);
  }
  for (i = 0; i < BF_PKT_RX_RING_MAX; i++) {
    lld_register_rx_packet_callback(dev_id, 0, i, NULL);
  }
}

int bf_kpkt_lld_master_init(bf_dev_id_t dev_id, struct bf_kpkt_adapter *adapter, const char *buf) {
  int i;
  dr_completion_callback_fn cmplt_cb;
  bf_dev_family_t tof_family = BF_DEV_FAMILY_TOFINO;

  if (dev_id < 0 || dev_id >= BF_MAX_DEV_COUNT) {
    return -1;
  }
  if (adapter->subdev_id != 0 ) {
    return 0; /* do it only for subdev_id == 0 */
  }
  cmplt_cb = (dr_completion_callback_fn)bf_kpkt_tx_completion_cb;
  tof_family = bf_kpkt_dev_family_get(adapter);
  /* lld_master_dev_add */
  /* init the packet DRs in case of warm init; we do not do lld_dev_remove
   * in kernel component, so, no need to redo lld_master_dev_add or lld_init
   * on warn start */
  if (buf[0] == '2') {
    lld_dr_init_dev(dev_id, 0, adapter->bf_dma_info);
  }
  else {
    /* we are force reiniting the ASIC here. So, remove the lld device if it
       already exists */
    if (lld_dev_remove(dev_id) != BF_SUCCESS) {
      dev_err(adapter->dev, "error LLD master dev remove %u\n", dev_id);
      goto kpkt_init_err;
    }
    if (lld_master_dev_add(
      dev_id, tof_family, adapter->bf_dma_info, BF_DEV_INIT_COLD) !=
      BF_SUCCESS) {
      dev_err(adapter->dev, "error LLD master dev add %u\n", dev_id);
      goto kpkt_init_err;
    }
  }
  /* Register callbacks with LLD */
  for (i = 0; i < BF_PKT_TX_RING_MAX; i++) {
    if (lld_register_completion_callback(
            dev_id, 0, lld_dr_cmp_tx_pkt_0 + i, cmplt_cb)) {
      dev_err(
          adapter->dev, "error registering LLD tx callback dev %u\n", dev_id);
      goto kpkt_init_err;
    }
  }
  for (i = 0; i < BF_PKT_RX_RING_MAX; i++) {
    if (lld_register_rx_packet_callback(dev_id, 0, i, pkt_mgr_rx_pkt_cb)) {
      dev_err(
          adapter->dev, "error registering LLD rx callback dev %u\n", dev_id);
      goto kpkt_init_err;
    }
  }
  if ((lld_dr_lock_required(dev_id, lld_dr_tx_pkt_0) != LLD_OK) ||
      (lld_dr_lock_required(dev_id, lld_dr_tx_pkt_1) != LLD_OK) ||
      (lld_dr_lock_required(dev_id, lld_dr_tx_pkt_2) != LLD_OK) ||
      (lld_dr_lock_required(dev_id, lld_dr_tx_pkt_3) != LLD_OK)) {
    dev_err(adapter->dev, "LLD pkt dr lock configuration failed for dev %u\n", dev_id);
    return -1;
  }
  return BF_SUCCESS;

kpkt_init_err:
  return -1;
}

/* PKT DR interrupt handler
 * schedule napi for the particular ring.
 * leave interrupt disabled.
 */
void bf_kpkt_irqhandler(int irq, void *adapter_ptr) {
  struct bf_kpkt_adapter *adapter;
  uint32_t en0 = 0, en1 = 0;

  adapter = (struct bf_kpkt_adapter *)adapter_ptr;
  if (!adapter) {
    printk(KERN_ERR "NULL adapater pointer in bf_kpkt irq hndler\n");
    return;
  }
  /* read the interrupt status register to determine the interrupting Ring */
  /* this can be optimized  by not reading the status and scheduling napi_poll
   * nontheless.
   */
  bf_tbus_int_st(adapter->dev_id, 0, &en0, 1);
  bf_tbus_int_st(adapter->dev_id, 1, &en1, 1);
  
  /* schedule napi for the ring */
  if (en0 || en1) {
    napi_schedule(&adapter->napi);
  }
}

/* enable or disable pkt DR interrupts (all) */
void bf_kpkt_cfg_dr_int(bf_dev_id_t dev_id, int en) {
  uint32_t en0, en1;

  if (dev_id < 0 || dev_id >= BF_MAX_DEV_COUNT) {
    return;
  }
  if (en) {
    en0 = TOFINO_TX_CPL_DR_ALL_NON_EMPTY_INT;
    en1 = TOFINO_RX_DR_ALL_NON_EMPTY_INT;
  } else {
    en0 = en1 = 0;
  }
  bf_tbus_int_en(dev_id, 0, 1, &en0, 0);
  bf_tbus_int_en(dev_id, 1, 1, &en1, 0);
}

