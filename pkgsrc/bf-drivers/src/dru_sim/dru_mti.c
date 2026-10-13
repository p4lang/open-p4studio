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


//
//  dru_mti.c
//
//  DRU simulator Message Transfer Interface (MTI)
//
//
#include <errno.h>
#include <sys/mman.h>
#include <time.h>
#ifdef TARGET_IS_MODEL
#include <assert.h>
#include <semaphore.h>
#define bf_sys_malloc malloc
#define bf_sys_free free
#define assert_macro assert
#else
#define assert_macro bf_sys_assert
#include <target-sys/bf_sal/bf_sys_intf.h>
#endif
#include <dru_sim/dru_mti.h>
#include <dvm/bf_drv_intf.h>
#include <lld/bf_dma_if.h>
#include <lld/lld_dr_descriptors.h>
#include <lld/lld_dr_regs.h>

#include <lld/lld_dr_regs_tof.h>
#include <lld/lld_dr_regs_tof2.h>
#include <lld/lld_dr_regs_tof3.h>

extern bool lld_dev_is_tofino(dru_dev_id_t dev_id);
extern bool lld_dev_is_tof2(dru_dev_id_t dev_id);
extern bool lld_dev_is_tof3(dru_dev_id_t dev_id);

extern bf_dev_family_t lld_dev_family_get(bf_dev_id_t dev_id);
extern int dru_process_inst_list(dru_dev_id_t asic, int dru, uint64_t *desc);
extern void dru_process_mac_stat(uint64_t *desc);
extern void dru_process_wr_blk(dru_dev_id_t asic, uint64_t *desc);
extern void dru_process_rd_blk(dru_dev_id_t asic, uint64_t *desc);
extern void dru_process_que_write_list(dru_dev_id_t asic, uint64_t *desc);
extern void dru_process_tx_pkt(dru_dev_id_t asic, uint64_t *desc);

extern void dru_to_pcie_dma_wr(uint32_t asic,
                               uint64_t addr,
                               uint8_t *buf,
                               uint32_t n);
extern void dru_to_pcie_dma_rd(uint32_t asic,
                               uint64_t addr,
                               uint8_t *buf,
                               uint32_t n);

extern int svc_cnt[BF_MAX_DEV_COUNT][BF_DMA_MAX_DR];

extern bf_dma_dr_id_t path_get_dr_info(bf_dma_dr_id_t dr, uint32_t dr_max);
extern dr_type_e dr_get_type(bf_dma_dr_id_t dr);

void post_possible_dru_work(void);

#define MIN(x, y) (((int)x) < ((int)y) ? ((int)x) : ((int)y))

#define MTI_FIFO_DEPTH (65536 * 8 /*max DMA sz/ min rx data sz*/)

extern void get_dru_regs(dru_dev_id_t asic, int dr, Dru_rspec_all **dru_regs);
extern int lld_validate_dr_id(bf_dev_id_t dev_id, bf_dma_dr_id_t dr_id);

typedef struct mti_msg_t {
  void *buffer;
  int len;
  /* Following field is used to implement
   * a simple state-machine for splitting Rx
   * data across multiple FM buffers
   */
  int bytes_processed;
  /*
   * "pipe" applies only to learn filter data
   */
  int pipe;
} mti_msg_t;

typedef struct mti_fifo_t {
  int hd;
  int tl;
  mti_msg_t msg[MTI_FIFO_DEPTH];
} mti_fifo_t;

int mti_fifo_empty(mti_fifo_t *f) {
  if (f->hd == f->tl) {
    return 1;
  }
  return 0;
}

int mti_fifo_full(mti_fifo_t *f) {
  if (((f->hd + 1) % MTI_FIFO_DEPTH) == f->tl) {
    return 1;
  }
  return 0;
}

void mti_fifo_push(mti_fifo_t *f, void *buffer, int len, int pipe) {
  f->msg[f->hd].buffer = buffer;
  f->msg[f->hd].len = len;
  f->msg[f->hd].bytes_processed = 0;
  f->msg[f->hd].pipe = pipe;
  f->hd = (f->hd + 1) % MTI_FIFO_DEPTH;
}

mti_msg_t *mti_fifo_peek(mti_fifo_t *f, void **buffer, int *len, int *pipe) {
  *buffer = f->msg[f->tl].buffer;
  *len = f->msg[f->tl].len;
  *pipe = f->msg[f->tl].pipe;
  return (&f->msg[f->tl]);
}

void mti_fifo_pull(mti_fifo_t *f, void **buffer, int *len, int *pipe) {
  mti_fifo_peek(f, buffer, len, pipe);
  f->tl = (f->tl + 1) % MTI_FIFO_DEPTH;
  return;
}

int mti_typ_to_dr_e[] = {
    lld_dr_rx_pkt_0,
    lld_dr_rx_pkt_1,
    lld_dr_rx_pkt_2,
    lld_dr_rx_pkt_3,
    lld_dr_rx_pkt_4,
    lld_dr_rx_pkt_5,
    lld_dr_rx_pkt_6,
    lld_dr_rx_pkt_7,
    lld_dr_rx_lrt,
    lld_dr_rx_idle,
    lld_dr_rx_learn,
    lld_dr_rx_learn,
    lld_dr_rx_learn,
    lld_dr_rx_learn,
    lld_dr_rx_diag,
};

mti_fifo_t **mti = NULL;

int dru_init_mti() {
  dru_dev_id_t dev_id;

  if (mti != NULL) {
    printf("INFO: DRU MTI already initialized\n");
    return 0;
  }

  /*
   * Allocate memory for the following 2D array of structs and
   * initialize it to all 0's.
   *     mti_fifo_t mti[BF_MAX_DEV_COUNT][MTI_TYP_NUM];
   */
  mti = (mti_fifo_t **)bf_sys_malloc(sizeof(mti_fifo_t *) * BF_MAX_DEV_COUNT);
  if (mti == NULL) {
    printf("ERROR: Memory allocation failed for DRU MTI\n");
    return -1;
  }

  memset(mti, 0, sizeof(mti_fifo_t *) * BF_MAX_DEV_COUNT);

  for (dev_id = 0; dev_id < BF_MAX_DEV_COUNT; dev_id++) {
    /*
     * Each device needs MTI_TYP_NUM fifos of MTI_FIFO_DEPTH entries
     * (~180 MB per device, ~1.4 GB for BF_MAX_DEV_COUNT devices).
     * malloc + memset made all of it resident at startup even though a
     * model setup normally uses a single device. Anonymous mmap memory is
     * zero-filled by the kernel and only backed by RAM when touched, so
     * fifos of unused devices cost (almost) nothing.
     */
    void *p = mmap(NULL,
                   sizeof(mti_fifo_t) * MTI_TYP_NUM,
                   PROT_READ | PROT_WRITE,
                   MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE,
                   -1,
                   0);
    if (p == MAP_FAILED) {
      printf("ERROR: Memory allocation failed for DRU MTI, dev_id =%d\n",
             dev_id);
      return -1;
    }
    mti[dev_id] = (mti_fifo_t *)p;
  }

  printf("INFO: DRU sim MTI initialized successfully\n");

  return 0;
}

mti_fifo_t *mti_fifo(dru_dev_id_t asic, int dr_e) {
  int i;

  if (!(asic < BF_MAX_DEV_COUNT)) return NULL;

  for (i = 0; i < MTI_TYP_NUM; i++) {
    if (mti_typ_to_dr_e[i] == dr_e) {
      return &mti[asic][i];
    }
  }
  return NULL;
}

void dru_mti_tx(dru_dev_id_t asic, mti_typ_e data_type, void *data, int len) {
  uint8_t *mti_data;
  mti_fifo_t *f;
  int pipe = -1;  // only matters for learn filter data

  // validate asic #
  assert_macro((asic < BF_MAX_DEV_COUNT));

  // validate data-type
  assert_macro((data_type < MTI_TYP_NUM));

  if ((data_type >= MTI_TYP_LEARN_PIPE0) &&
      (data_type <= MTI_TYP_LEARN_PIPE3)) {
    /*
     * All learn filter data should go thru the same
     * MTI fifo to maintain sequence.
     */
    pipe = data_type - MTI_TYP_LEARN_PIPE0;
    data_type = MTI_TYP_LEARN_PIPE0;
  }
  f = &mti[asic][data_type];

  mti_data = bf_sys_malloc(len);
  // validate mallocd buffer
  assert_macro(mti_data);

  // validate MTI storage availability
  // assert_macro( !mti_fifo_full(f) );
  if (mti_fifo_full(f)) {
    printf("MTI: Error: Fifo full <typ=%d>\n", data_type);
    bf_sys_free(mti_data);
    post_possible_dru_work();  // try to wake him up anyway
    return;
  }

  memcpy((char *)mti_data, (char *)data, len);

  mti_fifo_push(f, mti_data, len, pipe);

  post_possible_dru_work();
}

/**************************************************************
 * dru_learn
 *
 * Wrapper for dru_mti_tx() for learn filter data
 *
 * Accepts up to 4K 48 byte learn quanta
 **************************************************************/
void dru_learn(dru_dev_id_t asic,
               uint8_t *learn_filter_data,
               int len,
               int pipe_nbr) {
  assert_macro((len <= 4096 * 48));

  dru_mti_tx(asic, MTI_TYP_LEARN_PIPE0 + pipe_nbr, learn_filter_data, len);
}

/**************************************************************
 * dru_rx_pkt
 *
 * Wrapper for dru_mti_tx() for Rx packets
 **************************************************************/
void dru_rx_pkt(dru_dev_id_t asic, uint8_t *pkt, int len, int cos) {
  dru_mti_tx(asic, MTI_TYP_RX_PKT_0 + cos, pkt, len);
}

/**************************************************************
 * dru_lrt_update
 *
 * Wrapper for dru_mti_tx() for LRT stat updates
 **************************************************************/
void dru_lrt_update(dru_dev_id_t asic, uint8_t *lrt_stat_data, int len) {
  uint8_t lrt_update[16];

  assert_macro((len <= 16));
  memset((char *)&lrt_update[0], 0, sizeof(lrt_update));

  memcpy((char *)&lrt_update[0], lrt_stat_data, MIN(len, sizeof(lrt_update)));
  dru_mti_tx(asic, MTI_TYP_LRT, lrt_update, sizeof(lrt_update));
}

/**************************************************************
 * dru_idle_update
 *
 * Wrapper for dru_mti_tx() for Idle timeout updates
 **************************************************************/
void dru_idle_update(dru_dev_id_t asic, uint8_t *idle_timeout_data, int len) {
  uint8_t idle_update[8];

  assert_macro((len <= 8));
  memset((char *)&idle_update[0], 0, sizeof(idle_update));

  memcpy((char *)&idle_update[0],
         idle_timeout_data,
         MIN(len, sizeof(idle_update)));
  dru_mti_tx(asic, MTI_TYP_IDLE, idle_update, sizeof(idle_update));
}

/**************************************************************
 * dru_diag_event
 *
 * Wrapper for dru_mti_tx() for Diagnostic events
 **************************************************************/
void dru_diag_event(dru_dev_id_t asic, uint8_t *diag_data, int len) {
  uint8_t diag_event[16];

  assert_macro((len <= 16));
  memset((char *)&diag_event[0], 0, sizeof(diag_event));

  memcpy((char *)&diag_event[0], diag_data, MIN(len, sizeof(diag_event)));
  dru_mti_tx(asic, MTI_TYP_DIAG, diag_event, sizeof(diag_event));
}

int wds_per_dr_typ(bf_dma_dr_id_t dr_e) {
  int wds_per_desc[4] = {1, 2, 4, 2};
  dr_type_e dr_typ = dr_get_type(dr_e);

  return wds_per_desc[dr_typ];
}

static bool is_dru_full(dru_dev_id_t asic, bf_dma_dr_id_t dr_e) {
  uint32_t head_ptr, tail_ptr;
  uint32_t head_wrap_bit, tail_wrap_bit;
  Dru_rspec_all *dr_regs;
  get_dru_regs(asic, dr_e, &dr_regs);
  if (dr_regs == NULL) return true;
  head_ptr = TOF_DR_PTR_PART(dr_regs->head_ptr);
  tail_ptr = TOF_DR_PTR_PART(dr_regs->tail_ptr);
  head_wrap_bit = TOF_DR_WRP_PART(dr_regs->head_ptr);
  tail_wrap_bit = TOF_DR_WRP_PART(dr_regs->tail_ptr);
  // full is defined as,
  //
  // dr_full = (dr_head_ptr[15:3] == dr_tail_ptr[15:3]) &
  //           ~(dr_head_ptr[16] == hr_tail_ptr[16])
  //
  if ((head_ptr == tail_ptr) && (head_wrap_bit != tail_wrap_bit)) {
    // DR full
    return true;
  }
  return false;
}
static bool is_dru_empty(dru_dev_id_t asic, bf_dma_dr_id_t dr_e) {
  Dru_rspec_all *dr_regs;
  get_dru_regs(asic, dr_e, &dr_regs);
  if (dr_regs == NULL) return true;
  return (dr_regs->tail_ptr == dr_regs->head_ptr);
}
static bool is_dru_enabled(dru_dev_id_t asic, bf_dma_dr_id_t dr_e) {
  Dru_rspec_all *dr_regs;
  get_dru_regs(asic, dr_e, &dr_regs);
  if (dr_regs == NULL) return false;
  return (dr_regs->ctrl & 1);
}

/***************************************************************
 * get_dr_part
 *
 * Helper function to get values of
 * DR_WRAP_BIT_POSITION, TOF_DR_PTR_PART, TOF_DR_WRP_PART.
 * NOTE: The function uses common macros for all chips as they
 * are similar when this function is written. This function should
 * be updated if logic changes in future.
 ***************************************************************/
static void get_dr_part(dru_dev_id_t asic,
                        int *dr_wrap_bit_pos,
                        uint32_t *out_ptr,
                        uint32_t *wrap_bit,
                        Dru_rspec_all *dru_regs,
                        bool type_push_desc) {
  uint32_t addr;
  if (type_push_desc) {
    addr = dru_regs->tail_ptr;
  } else {
    addr = dru_regs->head_ptr;
  }
  switch (lld_dev_family_get(asic)) {
    case BF_DEV_FAMILY_TOFINO:
    case BF_DEV_FAMILY_TOFINO2:
    case BF_DEV_FAMILY_TOFINO3:

      *dr_wrap_bit_pos = TOF_DR_WRAP_BIT_POSITION;
      *out_ptr = TOF_DR_PTR_PART(addr);
      *wrap_bit = TOF_DR_WRP_PART(addr);
      break;
    default:
      *dr_wrap_bit_pos = -1;
      return;
  }
}

static bool dru_push_desc(dru_dev_id_t asic,
                          bf_dma_dr_id_t dr_e,
                          uint64_t *desc_p) {
  uint64_t bus_addr;
  uint64_t pushed_ptr_val;
  uint32_t tail_ptr, wrap_bit, desc_sz = wds_per_dr_typ(dr_e);
  int dr_wrap_bit_pos;
  Dru_rspec_all *dr_regs;
  get_dru_regs(asic, dr_e, &dr_regs);
  if (dr_regs == NULL) return false;
  // compute descriptor address
  get_dr_part(asic, &dr_wrap_bit_pos, &tail_ptr, &wrap_bit, dr_regs, true);
  if (dr_wrap_bit_pos == -1) return false;
  bus_addr = ((((uint64_t)dr_regs->base_addr_high) << 32ULL) |
              ((uint64_t)dr_regs->base_addr_low)) +
             (uint64_t)tail_ptr;
  dru_to_pcie_dma_wr(asic, bus_addr, (uint8_t *)desc_p, desc_sz * 8);
  tail_ptr = (tail_ptr + (desc_sz * 8));
  if (tail_ptr == dr_regs->size) {
    tail_ptr = 0;
    wrap_bit ^= 0x1;  // toggle wrap bit too
  }
  bus_addr = ((((uint64_t)dr_regs->base_addr_high) << 32ULL) |
              ((uint64_t)dr_regs->base_addr_low)) +
             (uint64_t)dr_regs->size;

  // DMA our tail_ptr into the DR (in CPU address space)
  dr_regs->tail_ptr = tail_ptr | (wrap_bit << dr_wrap_bit_pos);
  pushed_ptr_val = dr_regs->tail_ptr;
  dru_to_pcie_dma_wr(asic, bus_addr, (uint8_t *)&pushed_ptr_val, 8);
  return true;
}

static bool dru_pull_desc(dru_dev_id_t asic,
                          bf_dma_dr_id_t dr_e,
                          uint64_t *desc_p) {
  uint64_t bus_addr;
  uint64_t pushed_ptr_val;
  uint32_t head_ptr, wrap_bit, desc_sz = wds_per_dr_typ(dr_e);
  int dr_wrap_bit_pos;
  Dru_rspec_all *dr_regs;
  get_dru_regs(asic, dr_e, &dr_regs);
  if (dr_regs == NULL) return false;
  get_dr_part(asic, &dr_wrap_bit_pos, &head_ptr, &wrap_bit, dr_regs, false);
  if (dr_wrap_bit_pos == -1) return false;
  bus_addr = ((((uint64_t)dr_regs->base_addr_high) << 32ULL) |
              ((uint64_t)dr_regs->base_addr_low)) +
             (uint64_t)head_ptr;
  dru_to_pcie_dma_rd(asic, bus_addr, (uint8_t *)desc_p, desc_sz * 8);
  // remove from FM DR
  head_ptr = head_ptr + (desc_sz * 8);
  if (head_ptr == dr_regs->size) {
    head_ptr = 0;
    wrap_bit ^= 0x1;  // toggle wrap bit too
  }
  bus_addr = ((((uint64_t)dr_regs->base_addr_high) << 32ULL) |
              ((uint64_t)dr_regs->base_addr_low)) +
             (uint64_t)dr_regs->size;
  // DMA our head_ptr into the DR (in CPU address space)
  dr_regs->head_ptr = head_ptr | (wrap_bit << dr_wrap_bit_pos);
  pushed_ptr_val = dr_regs->head_ptr;
  dru_to_pcie_dma_wr(asic, bus_addr, (uint8_t *)&pushed_ptr_val, 8);
  return true;
}

/************************************************************************
 * process_lrt_stats_push
 *
 * data format:  bit(s)  description
 *                   0   0=stats, 1=barrier
 *               15: 1   rsvd
 *               63:16   stat/barrier address
 *              127:64   counter value or 0 (if barrier)
 ************************************************************************/
static void process_lrt_stats_push(dru_dev_id_t asic,
                                   mti_fifo_t *f,
                                   bf_dma_dr_id_t dr_e,
                                   bf_dma_dr_id_t other_dr_e) {
  int i, total_len = 0, dma_len = 0, max = 0, buf_len = 0;
  uint64_t dma_addr;
  uint64_t addr;
  void *buffer;
  int pipe_na;
  int sz_fld;
  uint64_t desc[4];
  if (lld_dev_family_get(asic) == BF_DEV_FAMILY_UNKNOWN) return;
  if (lld_validate_dr_id(asic, dr_e) || lld_validate_dr_id(asic, other_dr_e))
    return;
  /* For LRT (dump + evict + barrier/lock) DR, drain more aggressively. The
   * regular drain of other DRs happens at the rate of one DR at a time, i,e..
   * one "buffer" worth of data is shipped off to the driver. LRT DRs were
   * re-sized from 32kB to 512 bytes, and thus draining 512/16 = 32 messages
   * at a time, which would cause the MTI FIFO to overflow. The logic below
   * drains until there is no message to be shipped out, subject to the
   * availability of free memory DR and space to stick the message into the
   * LRT Rx DR.
   */
  while (f && !mti_fifo_empty(f) && !is_dru_full(asic, other_dr_e) &&
         !is_dru_empty(asic, dr_e)) {
    if (!dru_pull_desc(asic, dr_e, desc)) return;
    svc_cnt[asic][dr_e]++;
    extract_dr_msg_fm_wd0(desc[0], addr, sz_fld);
    buf_len = convert_free_memory_sz(sz_fld);
    max = buf_len / 16;
    dma_addr = addr;
    total_len = 0;
    for (i = 0; i < max; i++) {
      if (mti_fifo_empty(f)) break;
      mti_fifo_pull(f, &buffer, &dma_len, &pipe_na);
      assert_macro((dma_len == 16));
      dru_to_pcie_dma_wr(asic, dma_addr, (uint8_t *)buffer, dma_len);
      dma_addr += 16;
      total_len += 16;
      bf_sys_free(buffer);
    }
    desc[1] = desc[0] & ~0xF;  // clear "size" bits
    desc[0] = 0x3ULL | ((uint64_t)total_len << 32ULL);
    desc[0] |= (uint64_t)(rx_m_type_lrt << 2);
    if (!dru_push_desc(asic, other_dr_e, desc)) {
      return;
    }
    svc_cnt[asic][other_dr_e]++;
  }
}

/************************************************************************
 * process_idle_timeout_push
 *
 * data format:  bit(s)  description
 *                   0   0=stats, 1=barrier
 *               15: 1   rsvd
 *               63:16   idle time/barrier address
 ************************************************************************/
static void process_idle_timeout_push(dru_dev_id_t asic,
                                      uint64_t *desc,
                                      mti_fifo_t *f,
                                      uint64_t addr,
                                      int buf_len) {
  int i, total_len = 0, dma_len, max = buf_len / 8;
  uint64_t dma_addr = addr;
  void *buffer;
  int pipe_na;

  if (max == 0) return;  // make sure there's room for at least one
  for (i = 0; i < max; i++) {
    if (mti_fifo_empty(f)) break;
    mti_fifo_pull(f, &buffer, &dma_len, &pipe_na);
    assert_macro((dma_len == 8));
    dru_to_pcie_dma_wr(asic, dma_addr, (uint8_t *)buffer, dma_len);
    dma_addr += 8;
    total_len += 8;
    bf_sys_free(buffer);
  }
  desc[1] = desc[0] & ~0xF;  // clear "size" bits
  desc[0] = 0x3ULL | ((uint64_t)total_len << 32ULL);
  desc[0] |= (rx_m_type_idle << 2);
}

/************************************************************************
 * process_learn_quanta_push
 *
 * data format:  bit(s)  description
 *              383:0    learn filter entry data
 ************************************************************************/
static void process_learn_quanta_push(dru_dev_id_t asic,
                                      uint64_t *desc,
                                      mti_fifo_t *f,
                                      uint64_t dma_addr,
                                      int buf_len) {
  int dma_len, start = 0, end = 0;
  void *buffer;
  int pipe_na;
  int pipe, pushable_entries, leftover_entries;
  mti_msg_t *in_progress_msg;

  pushable_entries = buf_len / 48;

  in_progress_msg = mti_fifo_peek(f, &buffer, &dma_len, &pipe);
  leftover_entries =
      (in_progress_msg->len - in_progress_msg->bytes_processed) / 48;

  if (in_progress_msg->bytes_processed == 0) {  // not in progress
    start = 1;
  }

  // determine # of 48 byte entries that fit
  if (leftover_entries <= pushable_entries) {
    mti_fifo_pull(f, &buffer, &dma_len, &pipe_na);
    dma_len = leftover_entries * 48;
    end = 1;
  } else {
    dma_len = pushable_entries * 48;
    end = 0;
  }
  dru_to_pcie_dma_wr(asic,
                     dma_addr,
                     (uint8_t *)buffer + in_progress_msg->bytes_processed,
                     dma_len);
  if (end) {
    bf_sys_free(buffer);
  } else {
    in_progress_msg->bytes_processed += dma_len;
  }

  desc[1] = desc[0] & ~0xF;  // clear "size" bits
  desc[0] = (start << 0ull) | (end << 1ull) |
            (rx_m_type_learn << 2) |  // 3'b011:Learn Filter
            ((pipe & 7) << 7) |       // [9:7] are pipe ID
            ((uint64_t)dma_len << 32ULL);
}

/************************************************************************
 * process_rx_pkt_push
 *
 * data format:  bit(s)  description
 *              <packet data>
 *
 * This case is different than the above. Instead of packing multiple
 * entries in a single FM buffer, we may need to split the packet across
 * multiple FM buffers, using "start"/"end" flags as needed.
 ************************************************************************/
static void process_rx_pkt_push(dru_dev_id_t asic,
                                uint64_t *desc,
                                mti_fifo_t *f,
                                uint64_t addr,
                                int buf_len) {
  int dma_len, len, start = 1, end = 1;
  uint8_t *buf_addr;
  void *buffer;
  mti_msg_t *in_progress_msg;
  int free_needed = 0;
  int pipe_na;

  in_progress_msg = mti_fifo_peek(f, &buffer, &len, &pipe_na);
  if (in_progress_msg->bytes_processed == 0) {
    if (len <= buf_len) {
      mti_fifo_pull(f, &buffer, &len, &pipe_na);
      dma_len = len;
      buf_addr = (uint8_t *)buffer;
      free_needed = 1;
    } else {  // leave on fifo, set-up for "in-progress"
      end = 0;
      dma_len = buf_len;
      buf_addr = (uint8_t *)buffer;
      in_progress_msg->bytes_processed = buf_len;
    }
  } else {
    start = 0;
    if ((len - in_progress_msg->bytes_processed) <= buf_len) {
      mti_fifo_pull(f, &buffer, &len, &pipe_na);
      dma_len = (len - in_progress_msg->bytes_processed);
      buf_addr = (uint8_t *)buffer + in_progress_msg->bytes_processed;
      free_needed = 1;
    } else {
      end = 0;
      dma_len = buf_len;
      buf_addr = (uint8_t *)buffer + in_progress_msg->bytes_processed;
      in_progress_msg->bytes_processed += buf_len;
    }
  }
  dru_to_pcie_dma_wr(asic, addr, (uint8_t *)buf_addr, dma_len);

  if (free_needed) {
    bf_sys_free(buffer);
  }
  desc[1] = desc[0] & ~0xF;  // clear "size" bits
  desc[0] = (start << 0ull) | (end << 1ull) | ((uint64_t)dma_len << 32ULL);
  desc[0] |= (rx_m_type_pkt << 2);
}

dru_sim_dma2virt_dbg_callback_fn_mti bf_mem_dma2virt_dbg;

void dru_mti_register_dma2virt_cb(
    dru_sim_dma2virt_dbg_callback_fn_mti dma2virt_fn) {
  bf_mem_dma2virt_dbg = dma2virt_fn;
}

/************************************************************************
 * process_diag_evt_push
 *
 * data format:  bit(s)  description
 *              127: 0   diag data
 ************************************************************************/
static void process_diag_evt_push(dru_dev_id_t asic,
                                  uint64_t *desc,
                                  mti_fifo_t *f,
                                  uint64_t addr,
                                  int buf_len) {
  int i, dma_len, len, max = buf_len / 16;
  uint64_t *wd_ptr = (uint64_t *)bf_mem_dma2virt_dbg(asic, addr);
  uint64_t *mti_wd_ptr;
  void *buffer;
  int pipe_na;

  if (max == 0) return;  // make sure there's room for at least one
  dma_len = 0;
  for (i = 0; i < max; i++) {
    if (mti_fifo_empty(f)) break;
    mti_fifo_pull(f, &buffer, &len, &pipe_na);
    assert_macro((len == 16));
    mti_wd_ptr = (uint64_t *)buffer;
    *wd_ptr++ = *mti_wd_ptr++;
    *wd_ptr++ = *mti_wd_ptr;
    dma_len += 16;
    bf_sys_free(buffer);
  }
  dru_to_pcie_dma_wr(asic,
                     desc[0] & ~0xf,
                     (uint8_t *)bf_mem_dma2virt_dbg(asic, addr),
                     dma_len);
  desc[1] = desc[0] & ~0xF;  // clear "size" bits
  desc[0] = 0x3ULL | ((uint64_t)dma_len << 32ULL);
  desc[0] |= (rx_m_type_diag << 2);
}

static void dr_service_fm(dru_dev_id_t asic,
                          bf_dma_dr_id_t dr_e,
                          bf_dma_dr_id_t other_dr_e,
                          mti_fifo_t *f) {
  int sz_fld;
  uint64_t addr;
  int buf_len;
  uint64_t desc[4];

  if (lld_validate_dr_id(asic, dr_e) || lld_validate_dr_id(asic, other_dr_e))
    return;
  if (dr_e != lld_dr_fm_lrt) {
    if (!dru_pull_desc(asic, dr_e, desc)) return;
    svc_cnt[asic][dr_e]++;
    extract_dr_msg_fm_wd0(desc[0], addr, sz_fld);
    buf_len = convert_free_memory_sz(sz_fld);
  }
  switch (dr_e) {
    case lld_dr_fm_lrt:
      process_lrt_stats_push(asic, f, dr_e, other_dr_e);
      break;
    case lld_dr_fm_idle:
      process_idle_timeout_push(asic, desc, f, addr, buf_len);
      break;
    case lld_dr_fm_learn:
      process_learn_quanta_push(asic, desc, f, addr, buf_len);
      break;
    case lld_dr_fm_pkt_0:
    case lld_dr_fm_pkt_1:
    case lld_dr_fm_pkt_2:
    case lld_dr_fm_pkt_3:
    case lld_dr_fm_pkt_4:
    case lld_dr_fm_pkt_5:
    case lld_dr_fm_pkt_6:
    case lld_dr_fm_pkt_7:
      process_rx_pkt_push(asic, desc, f, addr, buf_len);
      break;
    case lld_dr_fm_diag:
      process_diag_evt_push(asic, desc, f, addr, buf_len);
      break;
    default:
      assert_macro(0);
  }

  if (dr_e != lld_dr_fm_lrt) {
    if (!dru_push_desc(asic, other_dr_e, desc)) {
      return;
    }
    svc_cnt[asic][other_dr_e]++;
  }
}
static void dr_service_tx(dru_dev_id_t asic,
                          bf_dma_dr_id_t dr_e,
                          bf_dma_dr_id_t other_dr_e) {
  uint64_t desc[4];
  int dru, il_data_sz;
  if (lld_validate_dr_id(asic, dr_e) || lld_validate_dr_id(asic, other_dr_e))
    return;
  switch (lld_dev_family_get(asic)) {
    case BF_DEV_FAMILY_TOFINO:
      svc_cnt[asic][dr_e]++;
      if (!dru_pull_desc(asic, dr_e, desc)) return;
      // handle TX desc based on type
      switch (BITS64(desc[0], 4, 2)) {
        case tx_m_type_mac_stat:
          dru_process_mac_stat(desc);
          // build completion (CP)
          {
            int data_sz_in_bytes = 89 * 8;    // fixed data length
            desc[0] = desc[0] & ~(0xf << 5);  // mask off error status
            desc[0] = desc[0] & ~(0xffffffffull << 32ull);  // mask off data_sz
            desc[0] = desc[0] | ((uint64_t)data_sz_in_bytes << 32ull);
          }
          desc[1] = desc[3];  // message-id
          break;
        case tx_m_type_il:
          dru = (dr_e - lld_dr_tx_pipe_inst_list_0);
          il_data_sz = dru_process_inst_list(asic, dru, desc);
          // build completion (CP)
          desc[0] = (desc[0] & 0x00000000ffffffffull) |
                    ((uint64_t)il_data_sz << 32ull);
          desc[0] = desc[0] & ~(0xf << 5);  // mask off error status
          desc[1] = desc[3];                // message-id
          break;
        case tx_m_type_wr_blk:
          dru_process_wr_blk(asic, desc);
          // build completion (CP)
          {  // int entry_typ = BITS64( desc[0],  7, 5 );
            int data_sz = BITS64(desc[0], 63, 32);
            int data_sz_in_bytes = data_sz;  // * ((entry_typ == 0) ? 4:
                                             //    (entry_typ == 1) ? 8:16);
            desc[0] = desc[0] & ~(0xffffffe0ull);  // mask off error status
            desc[0] = desc[0] & ~(0xffffffffull << 32ull);  // mask off data_sz
            desc[0] = desc[0] | ((uint64_t)data_sz_in_bytes << 32ull);
          }
          desc[1] = desc[3];  // message-id
          break;
        case tx_m_type_rd_blk:
          dru_process_rd_blk(asic, desc);
          // build completion (CP)
          {
            int entry_typ = BITS64(desc[0], 7, 5);
            int data_sz = BITS64(desc[0], 63, 32);
            int data_sz_in_bytes =
                data_sz * ((entry_typ == 0) ? 4 : (entry_typ == 1) ? 8 : 16);
            desc[0] = desc[0] & ~(0xf << 5);  // mask off error status
            desc[0] = desc[0] & ~(0xffffffffull << 32ull);  // mask off data_sz
            desc[0] = desc[0] | ((uint64_t)data_sz_in_bytes << 32ull);
          }
          desc[1] = desc[3];  // message-id
          break;
        case tx_m_type_que_wr_list:
          dru_process_que_write_list(asic, desc);
          // build completion (CP)
          {
            int data_sz = BITS64(desc[0], 63, 32);
            desc[0] = desc[0] & ~(0xf << 5);  // mask off error status
            desc[0] = desc[0] & ~(0xffffffffull << 32ull);  // mask off data_sz
            desc[0] = desc[0] | ((uint64_t)data_sz << 32ull);
          }
          desc[1] = desc[3];  // message-id
          break;
        case tx_m_type_pkt:
          dru_process_tx_pkt(asic, desc);
          // build completion (CP)
          desc[0] = desc[0] & ~(0xf << 5);  // mask off error status
          desc[1] = desc[3];                // message-id
          break;
        default:
          assert_macro(0);
      }

      // send back completion (CP)
      if (!dru_push_desc(asic, other_dr_e, desc)) return;

      svc_cnt[asic][other_dr_e]++;
      break;
    case BF_DEV_FAMILY_TOFINO2:
    case BF_DEV_FAMILY_TOFINO3:

      if (!dru_pull_desc(asic, dr_e, desc)) return;
      time_t t_stamp;
      uint64_t timestamp;
      svc_cnt[asic][dr_e]++;
      // handle TX desc based on type
      switch (BITS64(desc[0], 4, 2)) {
        case tx_m_type_mac_stat:
          dru_process_mac_stat(desc);
          break;
        case tx_m_type_il:
          dru = (dr_e - lld_dr_tx_pipe_inst_list_0);
          dru_process_inst_list(asic, dru, desc);
          break;
        case tx_m_type_mac_wr_blk:
        // tof2_dru_process_mac_wr_blk(asic, desc);
        // break;
        case tx_m_type_wr_blk:
          dru_process_wr_blk(asic, desc);
          break;
        // case tx_m_type_que_rd_blk:
        // tof2_dru_process_que_rd_blk(asic, desc);
        // break;
        case tx_m_type_rd_blk:
          dru_process_rd_blk(asic, desc);
          break;
        case tx_m_type_que_wr_list:
          dru_process_que_write_list(asic, desc);
          break;
        case tx_m_type_pkt:
          dru_process_tx_pkt(asic, desc);
          break;
        default:
          assert_macro(0);
      }
      // build completion (CP)
      t_stamp = time(0);
      timestamp = t_stamp & 0x000000ffffffffffull;      // only take 40 bits.
      desc[0] = desc[0] & ~(0xf << 5);                  // mask off error status
      desc[0] = desc[0] & ~(0xffffffffffull << 24ull);  // mask off timestamp
      desc[0] = desc[0] | (timestamp << 24ull);
      desc[1] = desc[3];  // message-id

      // send back completion (CP)
      if (!dru_push_desc(asic, other_dr_e, desc)) return;

      svc_cnt[asic][other_dr_e]++;
      break;
    default:
      return;
  }
}
bf_dma_dr_id_t trigger_drs[] = {
    lld_dr_fm_pkt_0,
    lld_dr_fm_pkt_1,
    lld_dr_fm_pkt_2,
    lld_dr_fm_pkt_3,
    lld_dr_fm_pkt_4,
    lld_dr_fm_pkt_5,
    lld_dr_fm_pkt_6,
    lld_dr_fm_pkt_7,
    lld_dr_fm_lrt,
    lld_dr_fm_idle,
    lld_dr_fm_learn,
    lld_dr_fm_diag,
    lld_dr_tx_pipe_inst_list_0,
    lld_dr_tx_pipe_inst_list_1,
    lld_dr_tx_pipe_inst_list_2,
    lld_dr_tx_pipe_inst_list_3,
    lld_dr_tx_pipe_write_block,
    lld_dr_tx_pipe_read_block,
    lld_dr_tx_que_write_list,
    lld_dr_tx_pkt_0,
    lld_dr_tx_pkt_1,
    lld_dr_tx_pkt_2,
    lld_dr_tx_pkt_3,
    lld_dr_tx_mac_stat,
    lld_dr_tx_mac_write_block,
    lld_dr_tx_que_write_list_1,
    lld_dr_tx_que_read_block_0,
    lld_dr_tx_que_read_block_1,
};

void dru_run_on(dru_dev_id_t asic) {
  uint32_t dr;
  uint32_t dr_max = (sizeof(trigger_drs) / sizeof(trigger_drs[0]));
  // for exclude the last four tof2 specific DRs.
  if (lld_dev_is_tofino(asic)) dr_max -= 4;
  for (dr = 0; dr < dr_max; dr++) {
    bf_dma_dr_id_t dr_e = trigger_drs[dr];
    // skip disabled channels
    if (!is_dru_enabled(asic, dr_e)) continue;
    if (!is_dru_empty(asic, dr_e)) {
      dr_type_e dr_typ = dr_get_type(dr_e);
      bf_dma_dr_id_t other_dr_e = path_get_dr_info(dr_e, dr_max);

      if (!is_dru_full(asic, other_dr_e)) {
        if (dr_typ == FM) {
          mti_fifo_t *f = mti_fifo(asic, other_dr_e);
          if (f && !mti_fifo_empty(f)) {
            dr_service_fm(asic, dr_e, other_dr_e, f);
          }
        } else if (dr_typ == TX) {
          dr_service_tx(asic, dr_e, other_dr_e);
        } else {
          assert_macro(0);
        }
      }
    }
  }
}

#ifdef TARGET_IS_MODEL
static int sem_initd = 0;
static sem_t possible_work;

extern int terminate_dru_thread;

void post_possible_dru_work(void) {
  int s;

  if (!sem_initd) {
    sem_init(&possible_work, 0, 1);
    sem_initd = 1;
  }
  s = sem_post(&possible_work);
  if (s == -1) {
    perror("sem_post");
  }
}
void dru_run(void) {
  dru_dev_id_t asic;
  int s = 0;
  int ret;

  if (!sem_initd) {
    s = sem_init(&possible_work, 0, 1);
    if (s == -1) {
      perror("sem_init");
    }
    sem_initd = 1;
  }

  ret = dru_init_mti();
  if (ret != 0) {
    perror("DRU sim mti initialization failed");
    return;
  }

  for (;;) {
    s = sem_wait(&possible_work);
    if (s == -1) {
      perror("sem_wait");
    }

    if (terminate_dru_thread) {
      terminate_dru_thread = 0;
      printf("DRU thread terminating..\n");
      return;
    }

    for (asic = 0; asic < BF_MAX_DEV_COUNT; asic++) {
      dru_run_on(asic);
      sched_yield();
    }
  }
}
#else
static int sem_initd = 0;
static unsigned long work = 0;
bf_sys_mutex_t possible_work;

void post_possible_dru_work(void) {
  int s;

  if (!sem_initd) {
    s = bf_sys_mutex_init(&possible_work);
    if (s == -1) {
      perror("sem_init");
      assert_macro(!s);
    }
    sem_initd = 1;
  }
  s = bf_sys_mutex_lock(&possible_work);
  if (s == -1) {
    perror("sem_lock");
    assert_macro(!s);
  }
  ++work;
  s = bf_sys_mutex_unlock(&possible_work);
  if (s == -1) {
    perror("sem_unlock");
    assert_macro(!s);
  }
}

void dru_run(void) {
  dru_dev_id_t asic;
  int s = 0;
  int ret;

  if (!sem_initd) {
    s = bf_sys_mutex_init(&possible_work);
    if (s == -1) {
      perror("sem_init");
      assert_macro(!s);
    }
    sem_initd = 1;
  }

  ret = dru_init_mti();
  if (ret != 0) {
    perror("DRU sim mti initialization failed");
    return;
  }

  for (;;) {
    s = bf_sys_mutex_lock(&possible_work);
    if (s == -1) {
      perror("sem_lock");
      assert_macro(!s);
    }
    unsigned long my_work = work;
    s = bf_sys_mutex_unlock(&possible_work);
    if (s == -1) {
      perror("sem_unlock");
      assert_macro(!s);
    }
    while (my_work) {
      for (asic = 0; asic < BF_MAX_DEV_COUNT; asic++) {
        dru_run_on(asic);
        sched_yield();
      }
      --my_work;
    }
  }
}
#endif
