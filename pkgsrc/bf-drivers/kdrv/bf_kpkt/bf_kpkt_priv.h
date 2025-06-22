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

#ifndef _BF_KPKT_H_
#define _BF_KPKT_H_

#include <linux/mutex.h>

#define BF_KPKT_MAGIC_WD_RX 0xfd619c0b
#define BF_KPKT_MAGIC_WD_TX 0xec508bfa

#define BF_KPKT_FAILED_READ_REG 0xffffffffU
#define BF_KPKT_FAILED_READ_CFG_DWORD 0xffffffffU
#define BF_KPKT_FAILED_READ_CFG_WORD 0xffffU
#define BF_KPKT_FAILED_READ_CFG_BYTE 0xffU

static inline bool bf_kpkt_removed(void __iomem *addr) {
  return unlikely(!addr);
}
#define BF_KPKT_REMOVED(a) bf_kpkt_removed(a)

/* Error  */
#define BF_KPKT_SUCCESS 0
#define BF_KPKT_ERR_FEATURE_NOT_SUPPORTED -1

#define BF_KPKT_READ_PCIE_WORD bf_kpkt_read_pci_cfg_word
#define BFNEt_WRITE_PCIE_WORD bf_kpkt_write_pci_cfg_word

#define BF_KPKT_VENDOR_ID 0x1D1C
/* following macros duplicated in bf_kdrv.c as well. Needs to unify! */
#define TOFINO_DEV_ID_A0 0x01
#define TOFINO_DEV_ID_B0 0x10
#define TOFINO2_DEV_ID_A0 0x0100
#define TOFINO2_DEV_ID_A00 0x0000
#define TOFINO2_DEV_ID_B0 0x0110
#define TOFINO3_DEV_ID_A0 0x0DA2





#define BF_KPKT_PCI_LINK_STATUS 0xB2 /* FIXME */
#define BF_KPKT_PCI_LINK_WIDTH 0x3F0
#define BF_KPKT_PCI_LINK_WIDTH_1 0x10
#define BF_KPKT_PCI_LINK_WIDTH_2 0x20
#define BF_KPKT_PCI_LINK_WIDTH_4 0x40
#define BF_KPKT_PCI_LINK_WIDTH_8 0x80
#define BF_KPKT_PCI_LINK_SPEED 0xF
#define BF_KPKT_PCI_LINK_SPEED_2500 0x1
#define BF_KPKT_PCI_LINK_SPEED_5000 0x2
#define BF_KPKT_PCI_LINK_SPEED_8000 0x3

#define MAX_TX_QUEUES 4
#define MAX_RX_QUEUES 8
#define BF_RX_BUF_ALLOC_SIZE 1536 /* should be multiple of cache line size */

/* TX/RX descriptor defines */
#define BF_KPKT_DEFAULT_TXD 512

#define BF_KPKT_DEFAULT_RXD 512

/* Supported Rx Buffer Sizes */
#define BF_KPKT_RXBUFFER_256 256 /* Used for skb receive header */
#define BF_KPKT_RXBUFFER_2K 2048
#define BF_KPKT_RXBUFFER_3K 3072
#define BF_KPKT_RXBUFFER_4K 4096
#define BF_KPKT_MAX_RXBUFFER 16384 /* largest size for single descriptor */

#define BF_KPKT_MAX_JUMBO_FRAME_SIZE 9728
#define BF_KPKT_MIN_FRAME_SIZE 68 /* kernel has problems with lesser value */
#define BF_KPKT_PCI_MIN_PKT_SIZE_TOF_A0 80
#define BF_KPKT_PCI_MIN_PKT_SIZE_DEF	60
#define netdev_ring(ring) (ring->netdev)
#define napi_ring(ring) (ring->q_vector->napi)

#define BF_POLL_PERIOD 10 /* msec */
#define BF_FALLBACK_POLL_PERIOD 1 /* msec */

/* definitions of pkt_tx_completion and rx_pkt DR interrupts */
#define TOFINO_TX_CPL_DR_ALL_NON_EMPTY_INT 0x1FE00UL //the same for both Tof and Tof2
#define TOFINO_RX_DR_ALL_NON_EMPTY_INT 0xFFFF0000UL //the same for both Tof and Tof2

/* wrapper around a pointer to a socket buffer,
 * so a DMA handle can be stored along with the buffer */
struct bf_kpkt_tx_buffer {
  unsigned long time_stamp;
  struct sk_buff *skb;
  unsigned int bytecount;
  __be16 protocol;
  DEFINE_DMA_UNMAP_ADDR(dma);
  DEFINE_DMA_UNMAP_LEN(len);
  u32 tx_flags;
};

struct bf_kpkt_rx_buffer {
  struct sk_buff *skb;
  dma_addr_t dma;
};

struct bf_kpkt_queue_stats {
  u64 packets;
  u64 bytes;
  u64 drops;
  u64 errors;
  u64 aborts;
};

typedef enum {
  BF_PKT_INIT = 0,
  BF_PKT_START,
  BF_PKT_MIDDLE,
  PF_PKT_END,
} bf_pkt_st;

/*
 * Enum to define PKT TX ring
 */
typedef enum bf_pkt_tx_ring_e {
  BF_PKT_TX_RING_0,
  BF_PKT_TX_RING_1,
  BF_PKT_TX_RING_2,
  BF_PKT_TX_RING_3,
  BF_PKT_TX_RING_MAX
} bf_pkt_tx_ring_t;

/*
 * Enum to define PKT RX ring
 */
typedef enum bf_pkt_rx_ring_e {
  BF_PKT_RX_RING_0,
  BF_PKT_RX_RING_1,
  BF_PKT_RX_RING_2,
  BF_PKT_RX_RING_3,
  BF_PKT_RX_RING_4,
  BF_PKT_RX_RING_5,
  BF_PKT_RX_RING_6,
  BF_PKT_RX_RING_7,
  BF_PKT_RX_RING_MAX
} bf_pkt_rx_ring_t;

struct bf_kpkt_ring {
  struct net_device *netdev; /* netdev ring belongs to */
  struct device *dev;        /* device for DMA mapping */
  void *desc;                /* descriptor ring memory */
  union {
    struct bf_kpkt_tx_buffer *tx_buffer_info;
    struct bf_kpkt_rx_buffer *rx_buffer_info;
  };
  struct bf_kpkt_queue_stats stats;
  int ring_index;
  unsigned long state;
  bf_pkt_st pkt_status;
  struct sk_buff *pkt_first;
  struct sk_buff *pkt_last;
  dma_addr_t dma; /* phys. address of descriptor ring */
  u16 rx_buf_len;
  unsigned int size; /* length in bytes */

  u16 count;       /* amount of descriptors */
  u16 next_to_use; /* next likely free descriptor to use */
  u16 next_rx_index; /* next likely ndex of rx packet */
  int poll_weight; /* napi budget, for rx pkt ring only */
  spinlock_t xmit_lock; /* per tx-queue lock */
};

struct bf_kpkt_hw {
  u8 *hw_addr;
  void *back;
  u16 device_id;
  u16 vendor_id;
  u16 subsystem_device_id;
  u16 subsystem_vendor_id;
  u8 revision_id;
  u8 pci_error;
};

struct bf_kpkt_cb {
  dma_addr_t dma;
};

#define BF_KPKT_CB(skb) ((struct bf_kpkt_cb *)(skb)->cb)

struct bf_kpkt_skb_meta_data {
  struct sk_buff *skb;
  u32 magic_wd;
  int buff_index;
};

#define BF_KPKT_SYSFS_NAME_SIZE 32

/* sysfs related structs */
struct kpkt_sysfs_buff_s {
  struct device_attribute dev_attr;
  struct device *device;
  char name[BF_KPKT_SYSFS_NAME_SIZE];
  int evt_no;
  struct bf_kpkt_adapter *adapter; /* back pointer */
};

typedef enum {
  BF_KPKT_ST_UNINIT = 0,
  BF_KPKT_ST_INIT = 1,
  BF_KPKT_ST_MASTER_INIT = 2,
  BF_KPKT_ST_PKT_DEV_ADD = 3,
  BF_KPKT_ST_DEV_ADD = 4,
  BF_KPKT_ST_DEV_DEL = 5,
  BF_KPKT_ST_DEV_LOCK = 6,
  BF_KPKT_ST_DEV_UNLOCK = 7,

  BF_KPKT_ST_MAX = 8 /* keep this the last */
} bf_kpkt_state;

typedef enum {
  BF_KPKT_EVT_INIT = 0,
  BF_KPKT_EVT_MST_INIT = 1,
  BF_KPKT_EVT_PKTDEV_INIT = 2,
  BF_KPKT_EVT_DEV_ADD = 3,
  BF_KPKT_EVT_DEV_DEL = 4,
  BF_KPKT_EVT_DEV_LOCK = 5,
  BF_KPKT_EVT_DEV_UNLOCK = 6,

  BF_KPKT_EVT_MAX = 7 /* keep this the last event */
} bf_kpkt_evt_t;

#define BF_NETDEV_ST_INITED 1
#define BF_NETDEV_ST_UP 2

/* board specific private data structure */
struct bf_kpkt_adapter {
  bf_dev_id_t dev_id;
  bf_subdev_id_t subdev_id;
  struct net_device *netdev;
  struct timer_list timer;
  struct pci_dev *pdev;
  struct device *dev;
  struct napi_struct napi;
  unsigned long state;
  unsigned long poll_period;
  bool netdev_registered;
  struct bf_kpkt_hw hw;
  bf_dma_info_t *bf_dma_info;
  dma_addr_t dr_dma_hndl; /* original DR dma handle */
  void *dr_cpu_dma_addr;  /* original DR address */
  struct kpkt_sysfs_buff_s kpkt_sysfs_buff[BF_KPKT_EVT_MAX];

  /* TX */
  struct bf_kpkt_ring *tx_ring[MAX_TX_QUEUES];
  int num_tx_queues;
  u32 pci_min_pkt_size; /* Tofino A0 takes minimum 80 bytes */

  /* RX */
  struct bf_kpkt_ring *rx_ring[MAX_RX_QUEUES];
  int num_rx_queues;
  unsigned int rx_ring_count;
  u16 head_room;        /* head room to allocate for rx pkts */
  int rx_pkt_processed; /* accounting for NAPI budgeting */

  u32 link_speed;
  bool link_up;
  bool dev_locked;
  bool tbus_inited;
  bool en_cpu_pkt; /* flag to mark cpu pkt path as up or down */
  bf_kpkt_state bf_kpkt_st;
  volatile long netdev_run_state; /* bitwise OR  */
  u64 rx_drops;                   /* drop due to bad rx_ringi */
  __be16 protocol;
  spinlock_t napi_lock;
  bool napi_enable;
  volatile int in_napi_poll;
  bool pkt_int_enable; /* if tx/rx packet DR interrupt is enabled */
};

typedef struct bf_kpkt_tbus_cfg_s {
  u8 pfc_fm_thr; /* num of free mem messages to turn on PFC */
  u8 pfc_rx_thr; /* num of rx messages to turn on PFC */
  u8 ecc_dis;    /* disable ECC decoder/checker */
  u8 crcrmv_dis; /* control bit to keep/remove crc32 */
  u8 port_en;    /* enable MAC port for PCIE */
  u8 port_alive; /* port aliveness */
  u16 ts_offset; /* timestamp offset */
  u8 rx_channel_offset ;// 4bits, RX channel byte offset
  u8 crcerr_keep;  // enable keeping the CRC of input error pkt
} bf_kpkt_tbus_cfg_t;

u32 bf_kpkt_read_reg(struct bf_kpkt_hw *hw, u32 reg, bool quiet);
void bf_kpkt_write_reg(struct bf_kpkt_hw *hw, u32 reg, u32 value);
bf_dev_family_t bf_kpkt_dev_family_get(struct bf_kpkt_adapter *adapter);
void bf_kpkt_chip_adapter_set(bf_dev_id_t dev_id, bf_subdev_id_t subdev_id,
                              struct bf_kpkt_adapter *adapter);
int bf_kpkt_lld_master_init(bf_dev_id_t dev_id, struct bf_kpkt_adapter *adapter, const char *buf);
void bf_kpkt_lld_master_deinit(struct bf_kpkt_adapter *adapter);
void bf_kpkt_pkt_deinit(bf_dev_id_t dev_id);
int bf_kpkt_init_tbus(bf_dev_id_t dev_id);
int bf_tbus_clr_int(u8 dev_id, int int_id);
void bf_kpkt_cfg_dr_int(bf_dev_id_t dev_id, int en);
void bf_kpkt_irqhandler(int irq, void *adapter_ptr);
bool bf_kpkt_is_locked(struct bf_kpkt_adapter *);
bool bf_kpkt_is_cpu_pkt_en(struct bf_kpkt_adapter *);
void bf_kpkt_dma_service_tx_ring_pkt(u8 dev_id, int ring_index, int count);
void bf_kpkt_dma_service_all_tx_pkt(u8 dev_id, int count);
void bf_kpkt_dma_service_rx_ring_pkt(u8 dev_id, int ring_index, int count);
void bf_kpkt_dma_service_all_rx_pkt(u8 dev_id, int count);
void bf_kpkt_set_rx_ring_buff_free(struct bf_kpkt_adapter *adapter,
                                   struct bf_kpkt_ring *rx_ring,
                                   int buff_index);
void bf_kpkt_free_mapped_skb(struct bf_kpkt_adapter *adapter,
                             struct bf_kpkt_ring *rx_ring,
                             int buff_index);
int bf_kpkt_dev_add(struct bf_kpkt_adapter *adapter);
int bf_kpkt_dev_del(struct bf_kpkt_adapter *adapter);
void bf_kpkt_sysfs_del(struct bf_kpkt_adapter *adapter);
int bf_kpkt_sysfs_add(struct bf_kpkt_adapter *adapter);
unsigned int bf_kpkt_check_rxpkt_dr_entry(bf_dev_id_t dev_id);
unsigned int bf_kpkt_check_txpkt_dr_entry(bf_dev_id_t dev_id);

netdev_tx_t bf_kpkt_xmit_frame_ring(struct sk_buff *skb,
                                    struct bf_kpkt_adapter *adapter,
                                    struct bf_kpkt_ring *tx_ring);
u8 *get_next_aligned_addr(u8 *base_addr, long hd_room, long align, long size);
int bf_kpkt_alloc_rx_buffers(struct bf_kpkt_adapter *adapter,
                             struct bf_kpkt_ring *rx_ring,
                             u16 count,
                             u16 hd_room);
void bf_kpkt_timer_init(struct bf_kpkt_adapter *adapter);
void bf_kpkt_timer_add(struct bf_kpkt_adapter *adapter, u32 ms);
void bf_kpkt_timer_del(struct bf_kpkt_adapter *adapter);
void bf_kpkt_lld_init(struct bf_kpkt_adapter *adapter);
void bf_kpkt_set_pci_error(void *adapter_ptr, u8 pci_error);
int bf_kpkt_init(struct pci_dev *pdev,
                 u8 *bar0_vaddr,
                 void **adapter_ptr,
                 bf_dev_id_t dev_id,
                 bf_dev_id_t subdev_id,
                 int pci_use_highmem,
                 unsigned long head_room,
                 int kpkt_dr_int_en,
                 unsigned long rx_desc_count);
void bf_kpkt_remove(void *adapter_ptr);
#endif /* _BF_KPKT_H_ */
