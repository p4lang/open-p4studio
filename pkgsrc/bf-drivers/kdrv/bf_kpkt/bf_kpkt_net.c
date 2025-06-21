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
#include <linux/pci.h>
#include <linux/dmapool.h>
#include <linux/netdevice.h>
#include <linux/skbuff.h>
#include <linux/etherdevice.h>
#include <linux/vmalloc.h>
#include <linux/string.h>
#include <linux/pkt_sched.h>
#include <linux/version.h>
#ifdef SIOCETHTOOL
#include <linux/ethtool.h>
#endif
#include <linux/version.h>

#include <bf_types/bf_types.h>
#include <dvm/bf_drv_intf.h>
#include <dvm/bf_dma_types.h>
#include <lld/bf_dma_if.h>
#include <lld/lld_dr_if.h>
#include <lld/lld_reg_if.h>
#include <lld/lld_err.h>
#include "bf_kpkt_priv.h"
#include <../../src/dvm/bf_drv_ver.h>

#define DRV_NAME "bf_kpkt"
#define DRV_VERSION BF_DRV_VER
#define DRV_RELDATE "01May2017"
#define DRV_FW_VER "none"

#if LINUX_VERSION_CODE > KERNEL_VERSION(4, 10, 0)
#define HAVE_NETDEVICE_MIN_MAX_MTU 1
#endif

/* FIXME : for multi-device scenario */
static u8 bf_pci_base_mac_addr[ETH_ALEN] = {0x00, 0x02, 0x00, 0x00, 0x03, 0x00};
static bf_dma_info_t bf_dma_info[BF_MAX_DEV_COUNT][BF_MAX_SUBDEV_COUNT] = {0};

static char bf_kpkt_ver[] = DRV_NAME
    ": NAPI Net Driver,"
    "version " DRV_VERSION " (" DRV_RELDATE ")\n";

static bool bf_kpkt_check_cfg_remove(struct bf_kpkt_hw *hw,
                                     struct pci_dev *pdev);

static void bf_kpkt_remove_adapter(struct bf_kpkt_hw *hw) {
  struct bf_kpkt_adapter *adapter = (struct bf_kpkt_adapter *)hw->back;

  if (!hw->hw_addr) {
    return;
  }
  hw->hw_addr = NULL;
  dev_err(adapter->dev, "Adapter removed\n");
}

u32 bf_kpkt_read_reg(struct bf_kpkt_hw *hw, u32 reg, bool quiet) {
  u32 value;
  u8 __iomem *reg_addr;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 18, 13)
  reg_addr = READ_ONCE(hw->hw_addr);
#else
  reg_addr = ACCESS_ONCE(hw->hw_addr);
#endif
  if (BF_KPKT_REMOVED(reg_addr)) {
    return BF_KPKT_FAILED_READ_REG;
  }
  /* do not access the device in case pci error was detected */
  if (hw->pci_error) {
    return BF_KPKT_FAILED_READ_REG;
  }
  value = readl(reg_addr + reg);
  return value;
}

void bf_kpkt_write_reg(struct bf_kpkt_hw *hw, u32 reg, u32 value) {
  u8 __iomem *reg_addr;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 18, 13)
  reg_addr = READ_ONCE(hw->hw_addr);
#else
  reg_addr = ACCESS_ONCE(hw->hw_addr);
#endif
  if (BF_KPKT_REMOVED(reg_addr)) {
    return;
  }
  /* do not access the device in case pci error was detected */
  if (hw->pci_error) {
    return;
  }
  writel(value, reg_addr + reg);
}

static bool bf_kpkt_check_cfg_remove(struct bf_kpkt_hw *hw,
                                     struct pci_dev *pdev) {
  u16 value;

  pci_read_config_word(pdev, PCI_VENDOR_ID, &value);
  if (value == 0xFFFFU) {
    bf_kpkt_remove_adapter(hw);
    return true;
  }
  return false;
}

bool bf_kpkt_is_locked(struct bf_kpkt_adapter *adapter) {
  return adapter->dev_locked;
}

void bf_kpkt_set_pci_error(void *adapter_ptr, u8 pci_error) {
  struct bf_kpkt_adapter *adapter = (struct bf_kpkt_adapter *)adapter_ptr;
  if (!adapter) {
    return;
  }
  adapter->hw.pci_error = pci_error;
}

/**
 * bf_kpkt_tx_timeout - Respond to a Tx Hang
 * @netdev: network interface device structure
 **/
#if defined(RHEL_RELEASE_CODE)
#if defined(RHEL_RELEASE_VERSION)
#if RHEL_RELEASE_CODE > RHEL_RELEASE_VERSION(7, 9)
static void bf_kpkt_tx_timeout(struct net_device *netdev, unsigned int t) {}
#else
static void bf_kpkt_tx_timeout(struct net_device *netdev) {}
#endif
#endif /* RHEL_RELEASE_CODE && RHEL_RELEASE_VERSION */
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(5, 8, 0)
static void bf_kpkt_tx_timeout(struct net_device *netdev, unsigned int t) {}
#else
static void bf_kpkt_tx_timeout(struct net_device *netdev) {}
#endif

void bf_kpkt_set_rx_ring_buff_free(struct bf_kpkt_adapter *adapter,
                                   struct bf_kpkt_ring *rx_ring,
                                   int buff_index) {
  struct bf_kpkt_rx_buffer *bi;

  bi = &rx_ring->rx_buffer_info[buff_index];
  if (!bi) {
    dev_err(adapter->dev,
            "error bad pointer in set buff free ring %u index %d\n",
            rx_ring->ring_index,
            buff_index);
    return;
  }
  bi->skb = NULL;
}

void bf_kpkt_free_mapped_skb(struct bf_kpkt_adapter *adapter,
                             struct bf_kpkt_ring *rx_ring,
                             int buff_index) {
  struct sk_buff *skb;
  struct bf_kpkt_rx_buffer *bi;

  bi = &rx_ring->rx_buffer_info[buff_index];
  if (!bi) {
    dev_err(adapter->dev,
            "error bad pointer in skb free ring %u index %d\n",
            rx_ring->ring_index,
            buff_index);
    return;
  }
  skb = bi->skb;
  if (bi->dma) {
    dma_unmap_single(
        rx_ring->dev, bi->dma, rx_ring->rx_buf_len, DMA_FROM_DEVICE);
    bi->dma = 0;
  }
  if (skb) {
    dev_kfree_skb_any(skb);
    bi->skb = NULL;
  }
}

/* function takes base address and returns the first address which is
 * atleast (base address+head_room) and aligned to "align".
 * <align> is 32, 64, 256 etc, must be power of two
 * the address returned is guaranteed to be < (base_addr + size).
 * returns NULL if such an address cannot be obtained.
 */
u8 *get_next_aligned_addr(u8 *base_addr, long hd_room, long align, long size) {
  u8 *new_addr;

  if (align & (align - 1)) {
    /* align is not a power of 2 */
    return NULL;
  }
  new_addr = base_addr + hd_room;
  if ((long)new_addr & (align - 1)) {
    new_addr = (u8 *)((long)(new_addr + align) & ~(align - 1));
  }
  return new_addr;
}

/**
 * allocate and map an skb for rx_ring, withdata area aligned to 256.
 *
 * Tofino returns the DMA address to s/w after receiving a packet.
 * we preserve the virtual address of skb just below the DMA-able area.
 * we also store the DMA address in SKB's CB area in case we need to clean
 * up the rx buffer before any packet is received in it.
 */
static bool bf_kpkt_alloc_mapped_skb(struct bf_kpkt_adapter *adapter,
                                     struct bf_kpkt_ring *rx_ring,
                                     int buff_index,
                                     u16 hd_room) {
  struct sk_buff *skb;
  u8 *aligned_addr;
  struct bf_kpkt_skb_meta_data *meta_data;
  struct bf_kpkt_rx_buffer *bi;
  dma_addr_t dma;
  unsigned int len;

  /* we need atleast 3 "long" worth of space for maintenance data */
  if (hd_room < (3 * sizeof(void *))) {
    return false;
  }

  bi = &rx_ring->rx_buffer_info[buff_index];
  skb = bi->skb;

  if (likely(!skb)) {
    skb = netdev_alloc_skb(netdev_ring(rx_ring),
                           rx_ring->rx_buf_len + 256 + hd_room);
    if (unlikely(!skb)) {
      dev_err(adapter->dev, "error allocating rx skb\n");
      return false;
    }
    /* data part of SKB must 256 bytes aligned per Tofino specs */
    /* we also leave head room for netdev's processing */
    aligned_addr =
        get_next_aligned_addr(skb->data, hd_room, 256, rx_ring->rx_buf_len);

    len = (unsigned int)(aligned_addr - skb->data);
    skb_reserve(skb, len);
    /* now, skb->data points to a 256 bytes aligned addr */
    /* write the skb virtual address just before skb->data so that
     * we can retrieve skb once we get the received data back
     */
    meta_data =
        (struct bf_kpkt_skb_meta_data *)(skb->data -
                                         (sizeof(
                                             struct bf_kpkt_skb_meta_data)));

    meta_data->skb = skb;
    /* for debug */
    meta_data->magic_wd = (unsigned long)skb & BF_KPKT_MAGIC_WD_RX;
    meta_data->buff_index = buff_index;

    skb_reset_mac_header(skb);
    skb_reset_network_header(skb);
    skb_reset_transport_header(skb);
    bi->skb = skb;
  }
  else {
    printk(KERN_ERR "bf_kpkt skb not inited rx ring %d index %d\n", rx_ring->ring_index, buff_index);
  }
  dma = dma_map_single(
      rx_ring->dev, skb->data, rx_ring->rx_buf_len, DMA_FROM_DEVICE);

  /*
   * if mapping failed free memory back to system since
   * there isn't much point in holding memory we can't use
   */
  if (dma_mapping_error(rx_ring->dev, dma)) {
    dev_kfree_skb_any(skb);
    bi->skb = NULL;
    bi->dma = 0;
    dev_err(adapter->dev, "error dmamap rx skb\n");
    return false;
  }
  /* store the dma adress in skb's CB area as well */
  BF_KPKT_CB(skb)->dma = dma;
  bi->dma = dma;
  return true;
}

/**
 * find first "empty" rx buffer in a ring
 * start by looking from offset <start> and wrap to offset zero when reaching
 * the end of the ring.
 *
 * return the offset in ring of the free buffer.
 */
static int find_free_rx_buff(struct bf_kpkt_ring *rx_ring, u16 start) {
  int i;

  if (start >= rx_ring->count) {
    return -1;
  }
  for (i = start; i < rx_ring->count; i++) {
    if (rx_ring->rx_buffer_info[i].skb == NULL) {
      return i;
    }
  }
  for (i = 0; i < start; i++) {
    if (rx_ring->rx_buffer_info[i].skb == NULL) {
      return i;
    }
  }
  return -1;
}

/**
 * bf_kpkt_alloc_rx_buffers - Replace used receive buffers
 * @rx_ring: ring to place buffers on
 * @count: number of buffers to replace
 * @hd_room: head room to reserve in skbuff
 *
 * return 0 on success and negative int on failure
 **/
int bf_kpkt_alloc_rx_buffers(struct bf_kpkt_adapter *adapter,
                             struct bf_kpkt_ring *rx_ring,
                             u16 count,
                             u16 hd_room) {
  int i;
  struct bf_kpkt_rx_buffer *rxbuff;

  for (i = 0; i < count; i++) {
    int buff_index = find_free_rx_buff(rx_ring, rx_ring->next_to_use);
    if (buff_index == -1) {
      dev_err(adapter->dev,
              "error finding %d-th free buff in Rx Ring %u next exp %u\n",
              i,
              rx_ring->ring_index,
              rx_ring->next_to_use);
      return -1;
    }
    rxbuff = &rx_ring->rx_buffer_info[buff_index];

    if (!bf_kpkt_alloc_mapped_skb(adapter, rx_ring, buff_index, hd_room)) {
      dev_err(adapter->dev, "error allocating skb\n");
      return -1;
    }
    if (lld_push_fm(adapter->dev_id,
                    lld_dr_fm_pkt_0 + rx_ring->ring_index,
                    rxbuff->dma,
                    rx_ring->rx_buf_len) != LLD_OK) {
      bf_kpkt_free_mapped_skb(adapter, rx_ring, buff_index);
      dev_err(adapter->dev, "error pushing to fm %u\n", rx_ring->ring_index);
      return -2;
    }
    rx_ring->next_to_use = buff_index + 1;
    if (rx_ring->next_to_use >= rx_ring->count) {
      rx_ring->next_to_use = 0;
    }
  }
  if (lld_dr_start(adapter->dev_id, 0, lld_dr_fm_pkt_0 + rx_ring->ring_index) !=
      LLD_OK) {
    dev_err(adapter->dev, "error starting dr %u\n", rx_ring->ring_index);
    return -2;
  }
  return 0;
}

static int bf_kpkt_configure_rx_ring(struct bf_kpkt_adapter *adapter,
                                      struct bf_kpkt_ring *ring) {
  return(bf_kpkt_alloc_rx_buffers(adapter, ring, adapter->rx_ring_count, adapter->head_room));
}

/**
 * bf_kpkt_configure_rx - Configure rx buffers for every rx ring
 * @adapter: board private structure
 *
 **/
static void bf_kpkt_configure_rx(struct bf_kpkt_adapter *adapter) {
  int i;

  for (i = 0; i < adapter->num_rx_queues; i++) {
    if(bf_kpkt_configure_rx_ring(adapter, adapter->rx_ring[i]) != 0) {
      dev_err(adapter->dev, "error allocating buffers for rx ring %d\n", i);
      /*** TBD  may need to abort loadng the driver */
    }
  }
}

/**
 * bf_kpkt_clean_rx_ring - Free  all Rx Buffers of a ring
 * @rx_ring: ring to free buffers from
 **/
static void bf_kpkt_clean_rx_ring(struct bf_kpkt_adapter *adapter,
                                  struct bf_kpkt_ring *rx_ring) {
  int i;

  /* ring already cleared, nothing to do */
  if (!rx_ring->rx_buffer_info) {
    return;
  }

  /* Free all the Rx ring sk_buffs */
  for (i = 0; i < rx_ring->count; i++) {
    bf_kpkt_free_mapped_skb(adapter, rx_ring, i);
  }
  rx_ring->next_to_use = 0;
  rx_ring->next_rx_index = 0;
}

#if 0  /* retained for future use */
/**
 * bf_kpkt_clean_all_rx_rings - Free Rx Buffers for all queues
 * @adapter: board private structure
 **/
static void bf_kpkt_clean_all_rx_rings(struct bf_kpkt_adapter *adapter) {
  int i;

  for (i = 0; i < adapter->num_rx_queues; i++)
    bf_kpkt_clean_rx_ring(adapter, adapter->rx_ring[i]);
}
#endif

static void bf_kpkt_napi_sched(struct bf_kpkt_adapter *adapter) {
  if (adapter->bf_kpkt_st == BF_KPKT_ST_DEV_ADD) {
    if (adapter->pkt_int_enable) {
      /* reenable packet DR interrupt by clearing the pending status */
      bf_tbus_clr_int(adapter->dev_id, 0);
      bf_tbus_clr_int(adapter->dev_id, 1);
      /* start a fall back timer in case interrupts were missed */
      bf_kpkt_timer_add(adapter, BF_FALLBACK_POLL_PERIOD);
    } else {
      /* schedule polling using the timer */
      bf_kpkt_timer_add(adapter, adapter->poll_period);
    }
  }
}

/**
 * bf_kpkt_poll - NAPI polling RX/TX cleanup routine
 * @napi: napi struct with our devices info in it
 * @budget: amount of work driver is allowed to do this pass, in packets
 *
 * This function will clean all queues associated with a q_vector.
 **/
static int bf_kpkt_poll(struct napi_struct *napi, int budget) {
  struct bf_kpkt_adapter *adapter =
      container_of(napi, struct bf_kpkt_adapter, napi);
  int temp_budget;

  adapter->in_napi_poll = 1;
  adapter->rx_pkt_processed = 0;
  temp_budget = budget;
  if (adapter->bf_kpkt_st != BF_KPKT_ST_DEV_ADD || !test_bit(BF_NETDEV_ST_UP, &adapter->netdev_run_state)) {
    /* interface state or tofino device state changed on other CPU core, abondon napi_poll() */
    adapter->in_napi_poll = 0;
    napi_complete(napi);
    return 0;
  }
  bf_kpkt_dma_service_all_tx_pkt(adapter->dev_id, 250);
  while(1) {
    bf_kpkt_dma_service_all_rx_pkt(adapter->dev_id, temp_budget);
    if (adapter->rx_pkt_processed < budget) {
      napi_complete(napi);
      spin_lock_bh(&adapter->napi_lock);
      if (adapter->napi_enable) { /* napi is disabled if interface went down */
        /* we clear interrupts here */
        bf_kpkt_napi_sched(adapter);
        if (bf_kpkt_check_rxpkt_dr_entry(adapter->dev_id) != 0) {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(6, 7, 0)
          if (napi_schedule(&adapter->napi)) {
#else
          if (napi_reschedule(&adapter->napi)) {
#endif /* LINUX_VERSION_CODE >= KERNEL_VERSION(6, 7, 0) */

            /* continue: more DR entries to process */
            temp_budget = budget - adapter->rx_pkt_processed;
            spin_unlock_bh(&adapter->napi_lock);
            continue;
          }
        }
      }
      spin_unlock_bh(&adapter->napi_lock);
    }
    break;
  }
  adapter->in_napi_poll = 0;
  return ((adapter->rx_pkt_processed > budget) ? budget : 
                                                 adapter->rx_pkt_processed);
}

static void bf_kpkt_up(struct bf_kpkt_adapter *adapter) {
  spin_lock_bh(&adapter->napi_lock);
  netif_carrier_on(adapter->netdev);
  napi_enable(&adapter->napi);
  /* enable transmits */
  netif_tx_start_all_queues(adapter->netdev);

  adapter->napi_enable = true;
  bf_kpkt_napi_sched(adapter);
  if (adapter->pkt_int_enable) {
    /* enable the interrupt */
    bf_kpkt_cfg_dr_int(adapter->dev_id, 1);
  }
  spin_unlock_bh(&adapter->napi_lock);
}

static void bf_kpkt_down(struct bf_kpkt_adapter *adapter) {
  struct net_device *netdev = adapter->netdev;
  bool enabled = false;

  spin_lock_bh(&adapter->napi_lock);
  if (adapter->napi_enable) {
    enabled = true;
    if (adapter->pkt_int_enable) {
      /* disable the packet DR interrupt */
      bf_kpkt_cfg_dr_int(adapter->dev_id, 0);
    }
    bf_kpkt_timer_del(adapter);
    netif_tx_stop_all_queues(netdev);
    adapter->napi_enable = false;
  }
  spin_unlock_bh(&adapter->napi_lock);
  msleep(50);

  if (enabled) {
    spin_lock_bh(&adapter->napi_lock);
    napi_disable(&adapter->napi);
    /* call carrier off first to avoid false dev_watchdog timeouts */
    netif_carrier_off(netdev);
    netif_tx_disable(netdev);
    spin_unlock_bh(&adapter->napi_lock);
  }
}

/**
 * bf_kpkt_free_rx_resources - Free Rx Resources
 * @rx_ring: ring to clean the resources from
 *
 * Free all receive software resources
 **/
static void bf_kpkt_free_rx_resources(struct bf_kpkt_adapter *adapter,
                                      struct bf_kpkt_ring *rx_ring) {
  bf_kpkt_clean_rx_ring(adapter, rx_ring);

  vfree(rx_ring->rx_buffer_info);
  rx_ring->rx_buffer_info = NULL;
}

/**
 * bf_kpkt_setup_rx_resources - allocate Rx ring resources (Descriptors)
 * @rx_ring:    rx descriptor ring
 *
 * Returns 0 on success, negative on failure
 **/
static int bf_kpkt_setup_rx_resources(struct bf_kpkt_ring *rx_ring) {
  int size;

  size = sizeof(struct bf_kpkt_rx_buffer) * rx_ring->count;
  rx_ring->pkt_status = BF_PKT_INIT;
  rx_ring->pkt_first = NULL;
  rx_ring->rx_buffer_info = vzalloc(size);
  rx_ring->next_to_use = 0;
  rx_ring->next_rx_index = 0;
  if (!rx_ring->rx_buffer_info) {
    return -ENOMEM;
  }
  return 0;
}

/**
 * bf_kpkt_free_all_rx_resources - Free Rx Resources for All Queues
 * @adapter: board private structure
 *
 * Free all receive software resources
 **/
static void bf_kpkt_free_all_rx_resources(struct bf_kpkt_adapter *adapter) {
  int i;

  for (i = 0; i < adapter->num_rx_queues; i++) {
    bf_kpkt_free_rx_resources(adapter, adapter->rx_ring[i]);
  }
}

/**
 * bf_kpkt_setup_all_rx_resources - allocate all queues Rx resources
 * @adapter: board private structure
 *
 * Return 0 on success, negative on failure
 **/
static int bf_kpkt_setup_all_rx_resources(struct bf_kpkt_adapter *adapter) {
  int i, err = 0;

  for (i = 0; i < adapter->num_rx_queues; i++) {
    adapter->rx_ring[i]->count = adapter->rx_ring_count;
    err = bf_kpkt_setup_rx_resources(adapter->rx_ring[i]);
    if (err) {
      dev_err(adapter->dev, "error allocation for Rx Queue %u\n", i);
      goto err_setup_rx;
    }
  }
  return 0;

err_setup_rx:
  /* cleanup part of the allocation before returning */
  while (i--) {
    bf_kpkt_free_rx_resources(adapter, adapter->rx_ring[i]);
  }
  return err;
}

/**
 * bf_kpkt_setup_all_tx_resources - init all tx queue variables
 * @adapter: board private structure
 *
 * Return 0 on success, negative on failure
 **/
static int bf_kpkt_setup_all_tx_resources(struct bf_kpkt_adapter *adapter) {
  int i;

  for (i = 0; i < adapter->num_tx_queues; i++) {
    adapter->tx_ring[i]->pkt_status = BF_PKT_INIT;
    adapter->tx_ring[i]->pkt_first = NULL;
  }
  return 0;
}

/**
 * bf_kpkt_free_all_tx_resources - Free Tx Resources for All Queues
 * @adapter: board private structure
 *
 * Free all transmit software resources
 **/
static void bf_kpkt_free_all_tx_resources(struct bf_kpkt_adapter *adapter) {
  (void)adapter;
}

/**
 * bf_kpkt_soft_init - Initialize module data structures
 * @adapter: initialize adapter (asic-netdev interface) data structures
 *
 **/
static int bf_kpkt_soft_init(struct bf_kpkt_adapter *adapter) {
  struct bf_kpkt_ring *ring_p;
  int i;
  ring_p =
      vzalloc(sizeof(struct bf_kpkt_ring) * (MAX_TX_QUEUES + MAX_RX_QUEUES));
  if (!ring_p) {
    dev_err(adapter->dev, "error allocating ring memory\n");
    return -ENOMEM;
  }
  for (i = 0; i < MAX_TX_QUEUES; i++) {
    adapter->tx_ring[i] = ring_p;
    adapter->tx_ring[i]->netdev = adapter->netdev;
    adapter->tx_ring[i]->dev = adapter->dev;
    adapter->tx_ring[i]->ring_index = i;
    spin_lock_init(&(adapter->tx_ring[i]->xmit_lock));
    ring_p++;
  }
  for (i = 0; i < MAX_RX_QUEUES; i++) {
    adapter->rx_ring[i] = ring_p;
    adapter->rx_ring[i]->netdev = adapter->netdev;
    adapter->rx_ring[i]->dev = adapter->dev;
    adapter->rx_ring[i]->ring_index = i;
    adapter->rx_ring[i]->rx_buf_len = BF_RX_BUF_ALLOC_SIZE;
    ring_p++;
  }

  return 0;
}

static void bf_kpkt_soft_deinit(struct bf_kpkt_adapter *adapter) {
  /* tx_ring[0] is the base of all ring pointers allocated previously */
  vfree(adapter->tx_ring[0]);
}

/**
 * bf_kpkt_change_mtu - Change the Maximum Transfer Unit
 * @netdev: network interface device structure
 * @new_mtu: new value for maximum frame size
 *
 * Returns 0 on success, negative on failure
 **/
static int bf_kpkt_change_mtu(struct net_device *netdev, int new_mtu) {
  struct bf_kpkt_adapter *adapter = netdev_priv(netdev);

  if ((new_mtu < BF_KPKT_MIN_FRAME_SIZE) ||
      (new_mtu > (BF_KPKT_MAX_JUMBO_FRAME_SIZE - (ETH_HLEN + ETH_FCS_LEN)))) {
    return -EINVAL;
  }
  dev_info(adapter->dev, "changing MTU from %d to %d\n", netdev->mtu, new_mtu);

  /* must set new MTU before calling down or up */
  netdev->mtu = new_mtu;
  return 0;
}

/**
 * bf_kpkt_set_mac_addr - Change the Ethernet Address of the  interface
 * @netdev: network interface device structure
 * @p: pointer to an address structure
 *
 * Returns 0 on success, negative on failure
 **/
static int bf_kpkt_set_mac_addr(struct net_device *netdev, void *addr) {
  struct sockaddr *saddr = addr;

  if (!netdev || !saddr) {
    return -EINVAL;
  }
  if (!is_valid_ether_addr(saddr->sa_data)) {
    return -EADDRNOTAVAIL;
  }

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 15, 0)
  dev_addr_mod(netdev, 0, saddr->sa_data, ETH_ALEN);
#else
  memcpy(netdev->dev_addr, saddr->sa_data, ETH_ALEN);
#endif
  printk(KERN_WARNING "bf_kpkt %s: Setting MAC address to %pM\n", netdev->name, netdev->dev_addr);
  return 0;
}

/**
 * bf_kpkt_open - Called when a network interface is made active
 * @netdev: network interface device structure
 *
 * Returns 0 on success, negative value on failure
 *
 * TBD: the watchdog timer is started,
 * and the stack is notified that the interface is ready.
 **/
static int bf_kpkt_open(struct net_device *netdev) {
  struct bf_kpkt_adapter *adapter = netdev_priv(netdev);
  u8 last_byte = adapter->dev_id;

  netif_carrier_off(netdev);
  /* set fake mac address */
  /* dev_id determines the last byte of interface mac address */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 15, 0)
  dev_addr_mod(netdev, 0, bf_pci_base_mac_addr, 1);
  dev_addr_mod(netdev, ETH_ALEN - 1, &last_byte, 1);
#else
  memcpy(netdev->dev_addr, bf_pci_base_mac_addr, ETH_ALEN);
  memcpy(netdev->dev_addr + ETH_ALEN - 1, &last_byte, 1);
#endif
  if (adapter->bf_kpkt_st == BF_KPKT_ST_DEV_ADD) {
    bf_kpkt_up(adapter);
  }
  set_bit(BF_NETDEV_ST_UP, &adapter->netdev_run_state);
  return 0;
}

/**
 * bf_kpkt_close - Disables a network interface
 * @netdev: network interface device structure
 *
 * Returns 0, this is not allowed to fail
 *
 * The close entry point is called when an interface is de-activated
 * by the OS.  The hardware is still under the drivers control, but
 * needs to be disabled.  A global MAC reset is issued to stop the
 * hardware, and all transmit and receive resources are freed.
 **/
static int bf_kpkt_close(struct net_device *netdev) {
  struct bf_kpkt_adapter *adapter = netdev_priv(netdev);

  adapter->netdev_run_state &= ~(1 << BF_NETDEV_ST_UP);
  /* next napi_poll should detect device down and stop napi.
     Interface can be taken down only after that */
  while(adapter->in_napi_poll) {
    msleep(50);
  }
  if (adapter->bf_kpkt_st == BF_KPKT_ST_DEV_ADD) {
    bf_kpkt_down(adapter);
  }
  return 0;
}

int bf_kpkt_dev_add(struct bf_kpkt_adapter *adapter) {
  int err;
  if (adapter->subdev_id != 0 ) {
    return 0; /* do it only for subdev_id == 0 */
  }
  /* allocate transmit descriptors */
  err = bf_kpkt_setup_all_tx_resources(adapter);
  if (err) {
    goto err_open;
  }
  /* allocate receive descriptors */
  err = bf_kpkt_setup_all_rx_resources(adapter);
  if (err) {
    goto err_open;
  }

  bf_kpkt_configure_rx(adapter);

  err = bf_kpkt_init_tbus(adapter->dev_id);
  if (err) {
    goto err_open;
  }

  adapter->bf_kpkt_st = BF_KPKT_ST_DEV_ADD;
  if (test_bit(BF_NETDEV_ST_UP, &adapter->netdev_run_state)) {
    bf_kpkt_up(adapter);
  }
  return 0;

err_open:
  dev_err(adapter->dev, "error pkt_dev_add\n");
  return err;
}

int bf_kpkt_dev_del(struct bf_kpkt_adapter *adapter) {
  if (adapter->subdev_id != 0 ) {
    return 0; /* do it only for subdev_id == 0 */
  }
  adapter->bf_kpkt_st = BF_KPKT_ST_DEV_DEL;
  while(adapter->in_napi_poll) {
    msleep(1);
  }
  msleep(1);
  bf_kpkt_down(adapter);
  adapter->en_cpu_pkt = false;
  adapter->tbus_inited = false;
  bf_kpkt_free_all_rx_resources(adapter);
  bf_kpkt_free_all_tx_resources(adapter);
  bf_kpkt_lld_master_deinit(adapter);
  return 0;
}

/**
 * bf_kpkt_update_stats - Update the board statistics counters.
 * @adapter: board private structure
 **/
static void bf_kpkt_update_stats(struct bf_kpkt_adapter *adapter) {
  struct net_device_stats *net_stats = &adapter->netdev->stats;
  u64 bytes, packets, drops, errors, aborts;
  int i;

  bytes = packets = drops = errors = 0;
  for (i = 0; i < adapter->num_rx_queues; i++) {
    struct bf_kpkt_ring *rx_ring = adapter->rx_ring[i];
    bytes += rx_ring->stats.bytes;
    packets += rx_ring->stats.packets;
    drops += rx_ring->stats.drops;
    errors += rx_ring->stats.errors;
  }
  drops += adapter->rx_drops;
  net_stats->rx_bytes = bytes;
  net_stats->rx_packets = packets;
  net_stats->rx_dropped = drops;
  net_stats->rx_errors = errors;

  bytes = packets = drops = errors = aborts = 0;
  for (i = 0; i < adapter->num_tx_queues; i++) {
    struct bf_kpkt_ring *tx_ring = adapter->tx_ring[i];
    bytes += tx_ring->stats.bytes;
    packets += tx_ring->stats.packets;
    drops += tx_ring->stats.drops;
    errors += tx_ring->stats.errors;
    aborts += tx_ring->stats.aborts;
  }
  net_stats->tx_bytes = bytes;
  net_stats->tx_packets = packets;
  net_stats->tx_dropped = drops;
  net_stats->tx_errors = errors;
  net_stats->tx_aborted_errors = aborts;
}

/**
 * bf_kpkt_get_stats - Get System Network Statistics
 * @netdev: network interface device structure
 *
 * Returns the address of the device statistics structure.
 * The statistics are actually updated from the timer callback.
 **/
static struct net_device_stats *bf_kpkt_get_stats(struct net_device *netdev) {
  struct bf_kpkt_adapter *adapter = netdev_priv(netdev);

  /* update the stats data */
  bf_kpkt_update_stats(adapter);
  return &netdev->stats;
}

static netdev_tx_t bf_kpkt_xmit_frame(struct sk_buff *skb,
                                      struct net_device *netdev) {
  struct bf_kpkt_adapter *adapter = netdev_priv(netdev);
  struct bf_kpkt_ring *tx_ring;
  netdev_tx_t tx_status;
  unsigned int r_idx = skb->queue_mapping;
  u32 min_pkt_size;

  if (!adapter) {
    dev_kfree_skb_any(skb);
    return NETDEV_TX_OK;
  }
  if (r_idx >= adapter->num_tx_queues) {
    r_idx = r_idx % adapter->num_tx_queues;
  }
  tx_ring = adapter->tx_ring[r_idx];
  min_pkt_size = adapter->pci_min_pkt_size;

  if (!netif_carrier_ok(netdev) || !bf_kpkt_is_cpu_pkt_en(adapter) ||
      (adapter->bf_kpkt_st != BF_KPKT_ST_DEV_ADD)) {
    dev_kfree_skb_any(skb);
    tx_ring->stats.drops++;
    return NETDEV_TX_OK;
  }

  /*
   * pad the packet to min packet size
   */
  if (skb->len < min_pkt_size) {
    if (skb_pad(skb, min_pkt_size - skb->len)) {
      /* some error here, but the API does not allow returning errors */
      /* skb is freed in this case */
      tx_ring->stats.drops++;
      return NETDEV_TX_OK;
    }
    skb_put(skb, min_pkt_size - skb->len);
  }
  spin_lock_bh(&(tx_ring->xmit_lock));
  tx_status = bf_kpkt_xmit_frame_ring(skb, adapter, tx_ring);
  spin_unlock_bh(&(tx_ring->xmit_lock));
  return tx_status;
}

static int bf_kpkt_ioctl(struct net_device *netdev,
                         struct ifreq *ifr,
                         int cmd) {
  switch (cmd) {
#ifdef ETHTOOL_OPS_COMPAT
    case:
      return ethtool_ioctl(ifr);
#endif
    default:
      return -EOPNOTSUPP;
  }
}

#if defined(RHEL_RELEASE_CODE)
#if defined(RHEL_RELEASE_VERSION)
#if RHEL_RELEASE_CODE >= RHEL_RELEASE_VERSION(7, 5)
#if RHEL_RELEASE_CODE <= RHEL_RELEASE_VERSION(7, 9)
#define ndo_change_mtu ndo_change_mtu_rh74
#endif
#endif
#endif
#endif

static const struct net_device_ops bf_kpkt_netdev_ops = {
    .ndo_open = bf_kpkt_open,
    .ndo_stop = bf_kpkt_close,
    .ndo_start_xmit = bf_kpkt_xmit_frame,
    .ndo_change_mtu = bf_kpkt_change_mtu,
    .ndo_tx_timeout = bf_kpkt_tx_timeout,
    .ndo_do_ioctl = bf_kpkt_ioctl,
    .ndo_get_stats = bf_kpkt_get_stats,
    .ndo_set_mac_address = bf_kpkt_set_mac_addr,
};

static void bf_kpkt_assign_netdev_ops(struct net_device *dev) {
  dev->netdev_ops = &bf_kpkt_netdev_ops;
  dev->watchdog_timeo = 5 * HZ;
}

static int bf_setup_dma_mem(struct bf_kpkt_adapter *adapter) {
  bf_dma_info_t *dma_info;
  int i;
  unsigned long msk = 7;
  adapter->bf_dma_info = &(bf_dma_info[adapter->dev_id][adapter->subdev_id]);
  dma_info = adapter->bf_dma_info;
  //bf_dev_id_t dev_id = adapter->dev_id;
  //int dma_type_max;
  if (!dma_info) {
    return -ENODEV;
  }
  /*if(lld_dev_is_tofino(dev_id)) {
    dma_type_max = BF_DMA_TYPE_MAX_TOF;
  } else if(lld_dev_is_tof2(dev_id)) {
    dma_type_max = BF_DMA_TYPE_MAX_TOF2;
  } else {
    return -ENODEV;
  }*/
  /* zero out size of count of all DRs not related to packet handling */
  /* the structure is zeroed out at init time , so, no need to
   * explicitly do that here.
   */
  /* free memory DRs */
  for (i = 0; i < MAX_RX_QUEUES; i++) {
    dma_info->dma_buff_info[BF_DMA_CPU_PKT_RECEIVE_0 + i].dma_buf_size = 2048;
    dma_info->dma_buff_info[BF_DMA_CPU_PKT_RECEIVE_0 + i].dma_buf_cnt =
        adapter->rx_ring_count + 16;
  }
  /*  TX pkt DRs */
  for (i = 0; i < MAX_TX_QUEUES; i++) {
    dma_info->dma_buff_info[BF_DMA_CPU_PKT_TRANSMIT_0 + i].dma_buf_size = 2048;
    dma_info->dma_buff_info[BF_DMA_CPU_PKT_TRANSMIT_0 + i].dma_buf_cnt = 512;
  }
  /* configure the DR memory */
  dma_info->dma_dr_buf_size = 0;
  for (i = 0; i < BF_DMA_TYPE_MAX; i++) {
    unsigned int buf_cnt = dma_info->dma_buff_info[i].dma_buf_cnt;
    unsigned int max_tx, max_rx;
    unsigned int guard = 0;
    int x;

    bf_dma_dr_get_max_depth(adapter->dev_id, i, &max_tx, &max_rx);
    if (max_tx < buf_cnt || max_rx < buf_cnt) {
      return -ENOMEM;
    }

    /* Note that free memory DRs have a lower priority than their
     * corresponding completion DR.  Therefore, if completions are being
     * generated at a high rate the free memory DR pointer may not be
     * updated by the hardware (since it is lower priority).  This can
     * result in the CPU seeing a completion while the free memory DR is
     * still full!  Add some guard space to the size of those free memory
     * DRs so that we don't run into this. */
    if (i == BF_DMA_PIPE_LEARN_NOTIFY || i == BF_DMA_PIPE_STAT_NOTIFY ||
        i == BF_DMA_PIPE_IDLE_STATE_NOTIFY || i == BF_DMA_DIAG_ERR_NOTIFY ||
        (i >= BF_DMA_CPU_PKT_RECEIVE_0 && i <= BF_DMA_CPU_PKT_RECEIVE_7)) {
      guard = buf_cnt < 16 ? buf_cnt : 16;
    }
    dma_info->dma_dr_info[i].dma_dr_entry_count[BF_DMA_DR_DIR_CPU2DEV] =
        buf_cnt + guard;
    dma_info->dma_dr_info[i].dma_dr_entry_count[BF_DMA_DR_DIR_DEV2CPU] =
        buf_cnt;

    x = bf_dma_dr_get_mem_requirement(
        adapter->dev_id,
        i,
        dma_info->dma_dr_info[i].dma_dr_entry_count[BF_DMA_DR_DIR_CPU2DEV],
        dma_info->dma_dr_info[i].dma_dr_entry_count[BF_DMA_DR_DIR_DEV2CPU]);
    dma_info->dma_dr_buf_size += x;
  }
  dma_info->dma_dr_buf_size <<=
      2; /* TBD ; some bugif RX DR size is less than 245 */
         /* allocate DR DMA memory */
  /* DR memory must be 128 byte aligned */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 0, 0)
  adapter->dr_cpu_dma_addr = dma_alloc_coherent(adapter->dev,
    dma_info->dma_dr_buf_size + 128,  &adapter->dr_dma_hndl, GFP_KERNEL);
  if (adapter->dr_cpu_dma_addr != NULL) {
    memset(adapter->dr_cpu_dma_addr, 0, dma_info->dma_dr_buf_size + 128);
  }
#else
  adapter->dr_cpu_dma_addr = dma_zalloc_coherent(adapter->dev,
    dma_info->dma_dr_buf_size + 128,  &adapter->dr_dma_hndl, GFP_KERNEL);
#endif
  if (adapter->dr_cpu_dma_addr == NULL) {
    return -ENOMEM;
  }
  /* the address returned should be page aligned, but the specification
   * does not explicitly guarantee that. So, pass on 128 byte aligned addresses
   * to lld DR APIs
   */
  dma_info->dr_mem_vaddr = (void *)((unsigned long)(adapter->dr_cpu_dma_addr) & ~msk);
  dma_info->dr_mem_dma_addr = (uint64_t)((uintptr_t)(adapter->dr_dma_hndl) & ~0x7FULL);
  return 0;
}

static void bf_release_dma_mem(struct bf_kpkt_adapter *adapter) {
  bf_dma_info_t *dma_info = adapter->bf_dma_info;

  if (!dma_info) {
    return;
  }
  if (adapter->dr_cpu_dma_addr) {
    dma_free_coherent(adapter->dev, dma_info->dma_dr_buf_size + 128,
                      adapter->dr_cpu_dma_addr, adapter->dr_dma_hndl);
    adapter->dr_cpu_dma_addr =  NULL;
    adapter->dr_dma_hndl = (dma_addr_t)0;
    dma_info->dr_mem_vaddr = NULL;
    dma_info->dr_mem_dma_addr = 0;
  }
}

static void bf_get_drvinfo(struct net_device *netdev, struct ethtool_drvinfo *drvinfo) {
  struct bf_kpkt_adapter *adapter;

  if (!netdev) {
    return;
  }
  adapter = netdev_priv(netdev);
  if (!adapter) {
    return;
  }
  strscpy(drvinfo->driver, DRV_NAME, sizeof(drvinfo->driver));
  strscpy(drvinfo->version, DRV_VERSION, sizeof(drvinfo->version));
  strscpy(drvinfo->bus_info, pci_name(adapter->pdev), sizeof(drvinfo->bus_info));
  strscpy(drvinfo->fw_version, DRV_FW_VER, sizeof(drvinfo->fw_version));
}

static const struct ethtool_ops bf_ethtool_ops = {
  .get_drvinfo            = bf_get_drvinfo,
};

static void bf_set_ethtool_ops(struct net_device *netdev) {
  netdev->ethtool_ops = &bf_ethtool_ops;
}

bf_dev_family_t bf_kpkt_dev_family_get(struct bf_kpkt_adapter *adapter) {
  if (!adapter) {
    return BF_DEV_FAMILY_UNKNOWN;
  }
  if (adapter->hw.device_id == TOFINO2_DEV_ID_A0 ||
      adapter->hw.device_id == TOFINO2_DEV_ID_A00 ||
      adapter->hw.device_id == TOFINO2_DEV_ID_B0) {
    return BF_DEV_FAMILY_TOFINO2;
  } else if (adapter->hw.device_id == TOFINO_DEV_ID_A0 ||
      adapter->hw.device_id == TOFINO_DEV_ID_B0) {
    return BF_DEV_FAMILY_TOFINO;
  } else if (adapter->hw.device_id == TOFINO3_DEV_ID_A0) {
    return BF_DEV_FAMILY_TOFINO3;





  } else {
    return BF_DEV_FAMILY_UNKNOWN;
  }
}

/**
 * bf_kpkt_init- Device Initialization kernel packet processing
 * @pdev: PCI device information struct
 * @adapter_ptr: pointer to kpkt adapter pointer
 * @dev_id: typically the device minor number
 * @pci_use_highmem: 1 if device takes high mem, 0 otherwiseA
 * @head_room: headroom to reserve in SKBs allocated for rx packets
 * @rx_desc_count: size of each rx ring
 *
 * Returns 0 on success, negative on failure
 *
 **/
int bf_kpkt_init(struct pci_dev *pdev,
                 u8 *bar0_vaddr,
                 void **adapter_ptr,
                 bf_dev_id_t dev_id,
                 bf_dev_id_t subdev_id,
                 int pci_use_highmem,
                 unsigned long head_room,
                 int kpkt_dr_int_en,
                 unsigned long rx_desc_count) {
  struct net_device *netdev;
  struct bf_kpkt_adapter *adapter = NULL;
  int err;

  if (!bar0_vaddr) {
    return -ENOMEM;
  }
  if (dev_id >= BF_MAX_DEV_COUNT || dev_id < 0) {
    return -ENODEV;
  }
  if (subdev_id >= BF_MAX_SUBDEV_COUNT || subdev_id < 0) {
    return -ENODEV;
  }

  printk(KERN_INFO "%s\n", bf_kpkt_ver);

  netdev = alloc_etherdev_mqs(
      sizeof(struct bf_kpkt_adapter), MAX_TX_QUEUES, MAX_RX_QUEUES);
  if (!netdev) {
    return -ENOMEM;
  }

  SET_NETDEV_DEV(netdev, &pdev->dev);

  adapter = netdev_priv(netdev);
  spin_lock_init(&adapter->napi_lock);
  /* alloc_etherdev_mqs() should have zeroed out adapter */
  adapter->num_tx_queues = MAX_TX_QUEUES;
  adapter->num_rx_queues = MAX_RX_QUEUES;
  adapter->rx_ring_count = rx_desc_count;
  adapter->poll_period = BF_POLL_PERIOD;
  adapter->head_room = head_room;
  adapter->netdev = netdev;
  adapter->pdev = pdev;
  adapter->dev = &pdev->dev;
  adapter->dev_id = dev_id;
  adapter->subdev_id = subdev_id;
  adapter->protocol = ETH_P_ALL;
  adapter->dev_locked = false;
  adapter->bf_kpkt_st = BF_KPKT_ST_INIT;
  adapter->napi_enable = false;
  adapter->pkt_int_enable = (kpkt_dr_int_en ? true : false);
  adapter->netdev_run_state = 0; /* state of "napi_enable" */
  clear_bit(BF_NETDEV_ST_UP, &adapter->netdev_run_state);
  /* get the BAR0 kernel virtual address from the base module */
  adapter->hw.back = adapter;
  adapter->hw.hw_addr = bar0_vaddr;
  adapter->hw.pci_error = 0;

  /* PCI config space info */
  adapter->hw.vendor_id = pdev->vendor;
  adapter->hw.device_id = pdev->device;
  pci_read_config_byte(pdev, PCI_REVISION_ID, &(adapter->hw.revision_id));
  if (adapter->hw.revision_id == BF_KPKT_FAILED_READ_CFG_BYTE &&
      bf_kpkt_check_cfg_remove(&(adapter->hw), pdev)) {
    printk(KERN_ERR "read of revision id failed dev_id %d subdev_id %d\n", adapter->dev_id, adapter->subdev_id);
    return -ENODEV;
  }
  adapter->hw.subsystem_vendor_id = pdev->subsystem_vendor;
  adapter->hw.subsystem_device_id = pdev->subsystem_device;
  bf_kpkt_chip_adapter_set(adapter->dev_id, adapter->subdev_id, adapter);

#ifdef HAVE_NETDEVICE_MIN_MAX_MTU
  netdev->min_mtu = BF_KPKT_MIN_FRAME_SIZE;
  netdev->max_mtu = BF_KPKT_MAX_JUMBO_FRAME_SIZE - (ETH_HLEN + ETH_FCS_LEN);
#endif
  if ((pdev->device) == TOFINO_DEV_ID_A0) {
    adapter->pci_min_pkt_size = BF_KPKT_PCI_MIN_PKT_SIZE_TOF_A0;
  } else {
    adapter->pci_min_pkt_size = BF_KPKT_PCI_MIN_PKT_SIZE_DEF;
  }

  if (adapter->subdev_id == 0) {
    bf_kpkt_assign_netdev_ops(netdev);
    bf_kpkt_timer_init(adapter);

#if LINUX_VERSION_CODE < KERNEL_VERSION(3, 10, 0)
    netif_napi_add(netdev, &adapter->napi, bf_kpkt_poll, 64);
#else
#ifdef RHEL_RELEASE_CODE
#if RHEL_RELEASE_CODE >= RHEL_RELEASE_VERSION(9, 2)
    netif_napi_add(netdev, &adapter->napi, bf_kpkt_poll);
#else
    netif_napi_add(netdev, &adapter->napi, bf_kpkt_poll, NAPI_POLL_WEIGHT);
#endif /* RHEL_RELEASE_CODE >= RHEL_RELEASE_VERSION(9, 2) */
#else
#if LINUX_VERSION_CODE < KERNEL_VERSION(6, 1, 0)
    netif_napi_add(netdev, &adapter->napi, bf_kpkt_poll, NAPI_POLL_WEIGHT);
#else
    netif_napi_add(netdev, &adapter->napi, bf_kpkt_poll);
#endif /* LINUX_VERSION_CODE < KERNEL_VERSION(6, 1, 0) */
#endif /* RHEL_RELEASE_CODE */
#endif

  }
  /* setup the private structure */
  err = bf_kpkt_soft_init(adapter);
  if (err) {
    dev_err(adapter->dev, "soft_init failed\n");
    goto err_soft_init;
  }

  if (adapter->subdev_id == 0) {
  netdev->features |= (NETIF_F_SG);
  if (pci_use_highmem) {
    netdev->features |= NETIF_F_HIGHDMA;
  }
  netdev->hw_features = netdev->features;

  netdev->priv_flags |= IFF_NOARP;

  snprintf(netdev->name, IFNAMSIZ, "bf_pci%d", dev_id);
  err = netif_set_real_num_tx_queues(netdev, MAX_TX_QUEUES);
  err |= netif_set_real_num_rx_queues(netdev, MAX_RX_QUEUES);
  if (err) {
    dev_err(adapter->dev, "netdev_set_real_num_queue failed\n");
    goto err_register;
  }

  err = register_netdev(netdev);
  if (err) {
    dev_err(adapter->dev, "register_netdev failed\n");
    goto err_register;
  }

  adapter->netdev = netdev;
  adapter->netdev_registered = true;
  }
  if (bf_setup_dma_mem(adapter)) {
    dev_err(adapter->dev, "error in bf_dma_setup\n");
    err = -ENOMEM;
    goto err_dma_mem;
  }
  if (bf_kpkt_sysfs_add(adapter) != 0) {
    dev_err(adapter->dev, "pkt_sysfs failed\n");
    err = -ENOENT;
    goto err_add_sysfs;
  }

  bf_set_ethtool_ops(netdev);
  printk(KERN_NOTICE "%s BF PCI-MAC is up\n", netdev->name);
  *adapter_ptr = adapter;
  bf_kpkt_lld_init(adapter);
  return 0;

err_add_sysfs:
  bf_release_dma_mem(adapter);
err_dma_mem:
  unregister_netdev(netdev);
err_register:
  bf_kpkt_soft_deinit(adapter);
err_soft_init:
  free_netdev(netdev);
  *adapter_ptr = NULL;
  return err;
}

/**
 * bf_kpkt_remove - Device soft data structure release
 * @adapter_ptr : kpkt adapter pointer
 *
 **/
void bf_kpkt_remove(void *adapter_ptr) {
  struct bf_kpkt_adapter *adapter = (struct bf_kpkt_adapter *)adapter_ptr;
  struct net_device *netdev;

  /* if !adapter then we already cleaned up in probe */
  if (!adapter) {
    return;
  }
  adapter->netdev_run_state &= ~(1 << BF_NETDEV_ST_UP);
  msleep(50);

  netdev = adapter->netdev;
  bf_kpkt_dev_del(adapter);
  bf_kpkt_pkt_deinit(adapter->dev_id);
  if (adapter->netdev_registered) {
    unregister_netdev(adapter->netdev);
    adapter->netdev_registered = false;
  }
  bf_kpkt_sysfs_del(adapter);
  bf_release_dma_mem(adapter);
  bf_kpkt_soft_deinit(adapter);
  free_netdev(netdev);
}
