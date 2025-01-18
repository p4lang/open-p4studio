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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include <inttypes.h>
#include <stdbool.h>

#include <dru_sim/dru_sim.h>
#include "simulator_intf.h"
#include <model_core/register_categories.h>

extern void model_reg_write(unsigned int asic, unsigned int addr,
                            unsigned int value);
extern unsigned int model_reg_read(unsigned int asic, unsigned int addr);
extern unsigned int model_reg_decode(int asic, unsigned int addr, int *index);
extern void model_ind_write(int asic, uint64_t address, uint64_t data0,
                            uint64_t data1);
extern void model_ind_read(int asic, uint64_t address, uint64_t *data0,
                           uint64_t *data1);
extern void rmt_timer_increment(uint64_t pico_increment);
extern void write_proxy_reg_chnl(uint8_t *buf, int len);
extern int read_proxy_reg_chnl(uint8_t *buf, int len);
extern void write_rx_pkt_chnl(uint8_t *buf, int len);
extern void read_tx_pkt_chnl(uint8_t *buf, int len);

extern int g_rtl_integ;
extern int g_parallel_mode;
extern int g_debug_mode;
extern int g_sku;
extern int g_pkg_size;
extern int g_pipe_mode;
extern int g_phy_pipes_en_bmp;
extern uint8_t g_chip_part_rev_no;
extern int port_status_of_dev_port(int asic_id,
                                   uint32_t asic_port /*bf_dev_port_t*/);
extern uint32_t asic_port_to_global_port(uint8_t asic_id, uint16_t asic_port);
extern void common_packet_handler(uint32_t port_num, uint8_t *pkt,
                                  int len, int orig_len);
extern void write_rx_pkt_chnl(uint8_t *buf, int len);
extern void read_tx_pkt_chnl(uint8_t *buf, int len);
extern void display_pkt(uint8_t *pkt, int len);
extern void packet_emission_control(pkt_originator_e org, int asic_id,
                                    int port_id, uint8_t *pkt, int len);
extern bool rmt_get_type(uint8_t asic_id, uint8_t *chip_type);

/* hard coded efuse values for all SKUs of Tofino */
// BFNT10064Q
static const uint32_t bfn_77110_efuse_regs[8] = {0x0, 0x0, 0x0, 0x0,
                                                 0x0, 0x0, 0x0, 0x0};

//  BFNT10032Q
static const uint32_t bfn_77120_efuse_regs[8] = {0x0,       0x0, 0x0, 0x0,
                                                 0x8000000, 0x0, 0x0, 0x0};

//  BFNT10032D
static const uint32_t bfn_77121_even_efuse_regs[8] = {
    0x55400000, 0x55555555, 0x03155555, 0x0, 0x4000000, 0x0, 0x0, 0x0};

// BFNT10032D
static const uint32_t bfn_77121_odd_efuse_regs[8] = {
    0xAAA00000, 0xAAAAAAAA, 0x030AAAAA, 0x0, 0x4000000, 0x0, 0x0, 0x0};

// BFNT10024D
static const uint32_t bfn_77131_even_efuse_regs[8] = {
    0x55400017, 0x55555555, 0x00255555, 0x40000000, 0xc001815, 0x0, 0x0, 0x0};

// BFNT10024D
static const uint32_t bfn_77131_odd_efuse_regs[8] = {
    0xAAA00017, 0xAAAAAAAA, 0x000AAAAA, 0x40000000, 0xc001815, 0x0, 0x0, 0x0};

// BFNT10018D
static const uint32_t bfn_77140_even_efuse_regs[8] = {
    0x55400017, 0x55555555, 0x00255555, 0x40000000, 0x6001815, 0x0, 0x0, 0x0};

// BFNT10018D
static const uint32_t bfn_77140_odd_efuse_regs[8] = {
    0xAAA00017, 0xAAAAAAAA, 0x000AAAAA, 0x40000000, 0x6001815, 0x0, 0x0, 0x0};

//  BFNT10032D-012
static const uint32_t bfn_32d012_even_efuse_regs[8] = {
    0x55400000, 0x55555555, 0x03155555, 0x0, 0x7000000, 0x0, 0x0, 0x0};

// BFNT10032D-012
static const uint32_t bfn_32d012_odd_efuse_regs[8] = {
    0xAAA00000, 0xAAAAAAAA, 0x030AAAAA, 0x0, 0x7000000, 0x0, 0x0, 0x0};

/* hard coded efuse values for all SKUs of Tofino2 */
// BFNT20128Q
static const uint32_t bfn_t2_0128q_efuse_regs[16] = {
    0x00000110, 0x00000000, 0x00000000, 0x00000000, 0x10000080, 0x00020000,
    0x00000040, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000};

// BFNT20064Q:
static const uint32_t bfn_t2_0064q_efuse_regs[16] = {
    0x00000110, 0x00000000, 0x00000000, 0x00000000, 0x10000080, 0x00020000,
    0x00000040, 0x00000000, 0x00000000, 0x00000007, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000};

// BFNT20080T: Disable pipe 2
static const uint32_t bfn_t2_0080t_efuse_regs[16] = {
    0x00000110, 0x00000000, 0x00000000, 0x00000000, 0x100000A0, 0x00020000,
    0x00000040, 0x00000000, 0x00000000, 0x00000023, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000};

// BFNT20080TR: Disable pipe 0
static const uint32_t bfn_t2_0080tr_efuse_regs[16] = {
    0x00000110, 0x00000000, 0x00000000, 0x00000000, 0x10000088, 0x00020000,
    0x00000040, 0x00000000, 0x00000000, 0x00000023, 0x00000000, 0x00000000,
    0x00010000, 0x00000000, 0x00000000, 0x00000000};

// BFNT20064D: Disable pipes 2 and 3
static const uint32_t bfn_t2_0064d_efuse_regs[16] = {
    0x00000110, 0x00000000, 0x00000000, 0x00000000, 0x100000E0, 0x00020000,
    0x00000040, 0x00000000, 0x00000000, 0x00000067, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000};

// BFNT20064DR: Disable pipes 0 and 1
static const uint32_t bfn_t2_0064dr_efuse_regs[16] = {
    0x00000110, 0x00000000, 0x00000000, 0x00000000, 0x10000098, 0x00020000,
    0x00000040, 0x00000000, 0x00000000, 0x00000067, 0x00000000, 0x00000000,
    0x00010000, 0x00000000, 0x00000000, 0x00000000};

/* hard coded efuse values for all SKUs of Tofino3 */
// BFNT12256Q
static const uint32_t bfn_t3_12256q_efuse_regs[16] = {
    0x00000100, 0x00000000, 0x00000000, 0x00000000, 0x00000080, 0x00020000,
    0x00000040, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000};

// BFNT25256O
static const uint32_t bfn_t3_252560_efuse_regs[16] = {
    0x00000100, 0x00000000, 0x00000000, 0x00000000, 0x10000080, 0x00020000,
    0x00000040, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000800};

/*********************************************************************
* mac_ch_to_ibuf_port_tofino
*
* Mapping of MAC to ibuf port for vanilla tofino
*********************************************************************/
uint32_t mac_ch_to_ibuf_port_tofino[4][16] = {
    {60, 56, 52, 48, 44, 40, 36, 32, 28, 24, 20, 16, 12, 8, 4, 0},
    {0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60},
    {60, 56, 52, 48, 44, 40, 36, 32, 28, 24, 20, 16, 12, 8, 4, 0},
    {0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60},
};

/********************************************************************
* map_mac_block_to_ibuf_port
*
* Map MAC to ibuf port. asic_id should be used to map to correct
* translation table. For now, only one table...tofino
********************************************************************/
unsigned int map_mac_block_to_ibuf_port(int asic_id, unsigned int mac_blk) {
  uint32_t pipe_id = 0, port_id, ibuf_port = 0;
  uint8_t chip_type;
  rmt_get_type(asic_id, &chip_type);
  switch (chip_type) {
  case 1: // Tofino
  case 2: // kTofinoB0
    if (mac_blk == 64) {
      pipe_id = 0;
      ibuf_port = 64;
    } else {
      pipe_id = mac_blk / 16;
      port_id = mac_blk % 16;
      ibuf_port = mac_ch_to_ibuf_port_tofino[pipe_id][port_id];
    }
    break;
  case 4: // Jbay
    if (mac_blk == 0)
      return 0;
    pipe_id = (mac_blk - 1) / 8;
    ibuf_port = (mac_blk - 1) % 8;
    break;
  default:
    break;
  }
  return ((pipe_id << 7) | (ibuf_port));
}

/********************************************************************
* simulated_mac_status_register
*
* Determine whether the passed address corresponds to the Comira
* glb.livelnkstat register on one of the MACs.
********************************************************************/
int simulated_mac_status_register(uint32_t addr) {
  // FIXME for Jbay
  if ((addr & 0xFF00FFFF) == 0x1007c24) { // comira livelnkstate reg
    return 1;
  }
  return 0;
}

/********************************************************************
* simulated_mac_status_register_value
*
* Return a simulated Comira glb.livelnkstate value.
* The value is computed from a logical status maintained by the
* port monitoring code, in main.cpp
* The addr contains the status of up to 4x channels, based on port
* configuration.
* This code sets one of two fields (per-channel) that LLD may look
* at,
*    chlnkup<n>
*    chmacflt<n>
*
* If the port is up the channels' chlnkup bit is set
* If the port is down the channel chmacflt bit is set
*
* LLD will poll this register for each configured and enabled port.
* But, since we don't know which channel is of interest (here) we
* must return the status of all associated channels
********************************************************************/
unsigned int simulated_mac_status_register_value(int asic_id, uint32_t addr) {
  unsigned int val = 0, mac_blk = 0, base_ibuf_port, ch;
  int ch_sts;
  unsigned int max_chl = 0;
  uint8_t chip_type;
  rmt_get_type(asic_id, &chip_type);
  switch (chip_type) {
    case 1://tof
    case 2://tofB0
      // convert addr to MAC block
      mac_blk = (addr >> 17) & 0x7f;
      assert(mac_blk < 65);
      max_chl = 4;
      break;
    case 4://tof2
      // convert addr to MAC block
      mac_blk = (addr >> 18) & 0x3f;
      assert(mac_blk <= 32);
      max_chl = 8;
      break;
    default:
      break;
  }
  // convert MAC block to bf_dev_port_t
  base_ibuf_port = map_mac_block_to_ibuf_port(asic_id, mac_blk);

  // pick up logical statuses for each channel  from port monitoring code
  for (ch = 0; ch < max_chl; ch++) {
    ch_sts = port_status_of_dev_port(asic_id, base_ibuf_port + ch);
    if (ch_sts) { // up
      // if up, set chlinkup<n>
      val |= (1 << (0 + ch)); // set chlnkup<n>
    } else {                  // dn
      // else if dn, chmacflt<n>
      val |= (1 << (4 + ch)); // set chmacflt<n>
      // FIXME for Jbay
    }
  }
  return val;
}

int implemented_by_model(unsigned int asic, uint32_t addr) {
  int reg = model_reg_decode(asic, addr, NULL);
  if (reg == REG_MACS_CNTRS) // MODEL-110 - implement MAC cntrs
    return 1;
  reg &= REG_MASK;
  if ((reg == REG_SERDES) || (reg == REG_MACS) || (reg == REG_ETHS))
    return 0;
  return 1;
}

void simulator_pcie_pkt_in(unsigned int asic, uint8_t *buf, int len) {
  uint16_t asic_port = 0;
  uint16_t pipe;
  uint8_t chip_type;
  rmt_get_type(asic, &chip_type);
  switch (chip_type) {
    case 1:
    case 2:
      /* All PCIe packets come in pipe-2 port 64. */
      pipe = 2;
      /*
         For 2-pipe SKU, it could be pipe-2 port 64 or pipe-3 port 64 depending
         on which one is enabled
      */
      if (!((g_phy_pipes_en_bmp >> 2) & 0x1)) {
        pipe = 3;
      }
      asic_port = (pipe << 7) | 64;
      break;
    case 4:
      asic_port = 0;
      break;
    default:
      break;
  }
  uint32_t global_port = asic_port_to_global_port(asic, asic_port);

  common_packet_handler(global_port, buf, len, len);
}

/***************************************************************************
* simulator_reg_write
***************************************************************************/
void simulator_reg_write(unsigned int asic, unsigned int addr,
                         unsigned int value) {
  if (!g_rtl_integ || g_parallel_mode) {
    if (implemented_by_model(asic, addr)) {
      if (g_debug_mode) {
        printf("MDL: reg_wr: chip=%d : addr=%x : data=%x\n", asic, addr, value);
      }
      model_reg_write(asic, addr, value);
    }
  }
  if (g_rtl_integ) {

    printf("RTL: reg_wr: chip=%d : addr=%x : data=%x\n", asic, addr, value);
    bf_hdr_t hdr;
    hdr.typ = TYP_WR_32;
    hdr.u.rd_wr_32.chip = asic;
    hdr.u.rd_wr_32.address = addr;
    hdr.u.rd_wr_32.data_31_0 = value;
    write_proxy_reg_chnl((uint8_t *)&hdr, sizeof(hdr));
  }
}

/***************************************************************************
* simulator_reg_read
***************************************************************************/
unsigned int simulator_reg_read(unsigned int asic, unsigned int addr) {
  unsigned int model_val;

  /* trap the read access to func_fuse registers
   * and return the hard coded values
   */
  int idx = -1;
  uint8_t asic_type;
  if (!g_rtl_integ && (model_reg_decode(asic, addr, &idx) == REG_DEVSEL_FUSE)) {

    const uint32_t *efuse_reg;

    switch (g_sku) {
    case 0:
      efuse_reg = &bfn_77110_efuse_regs[0];
      break;
    case 1:
      efuse_reg = &bfn_77120_efuse_regs[0];
      break;
    case 2:
    case 3:
    case 4:
    case 5:
      efuse_reg = &bfn_77121_even_efuse_regs[0];
      break;
    case 6:
    case 7:
    case 8:
    case 9:
      efuse_reg = &bfn_77121_odd_efuse_regs[0];
      break;
    case 11:
    case 12:
    case 13:
    case 14:
      efuse_reg = &bfn_77131_even_efuse_regs[0];
      break;
    case 15:
    case 16:
    case 17:
    case 18:
      efuse_reg = &bfn_77131_odd_efuse_regs[0];
      break;
    case 19:
    case 20:
    case 21:
    case 22:
      efuse_reg = &bfn_77140_even_efuse_regs[0];
      break;
    case 23:
    case 24:
    case 25:
    case 26:
      efuse_reg = &bfn_77140_odd_efuse_regs[0];
      break;
    case 27:
    case 28:
    case 29:
    case 30:
      efuse_reg = &bfn_32d012_even_efuse_regs[0];
      break;
    case 31:
    case 32:
    case 33:
    case 34:
      efuse_reg = &bfn_32d012_odd_efuse_regs[0];
      break;
    case 200: // tof2
      efuse_reg = &bfn_t2_0128q_efuse_regs[0];
      break;
    case 201:
      efuse_reg = &bfn_t2_0064q_efuse_regs[0];
      break;
    case 202:
      efuse_reg = &bfn_t2_0080t_efuse_regs[0];
      break;
    case 203:
      efuse_reg = &bfn_t2_0080tr_efuse_regs[0];
      break;
    case 204:
      efuse_reg = &bfn_t2_0064d_efuse_regs[0];
      break;
    case 205:
      efuse_reg = &bfn_t2_0064dr_efuse_regs[0];
      break;
    case 300:  // TOF3
      if (g_pkg_size == 1) {
        // 1 die
        efuse_reg = &bfn_t3_12256q_efuse_regs[0];
      } else {
        // 2 die
        efuse_reg = &bfn_t3_252560_efuse_regs[0];
      }
      break;
    default:
      efuse_reg = &bfn_77110_efuse_regs[0];
      break;
    }

    model_val = efuse_reg[idx];
    rmt_get_type(asic, &asic_type);
    if ((asic_type == 2) || (asic_type == 1)) { // tof
      if (idx == 0) { /* take care of pipe_disable bits */
        /* Only set the disabled pipes in efuse */
        if ((g_pipe_mode >= 0) && (g_pipe_mode <= 4)) {
          unsigned int pipes_en = (unsigned int)g_phy_pipes_en_bmp;
          /* bits 5-8 are pipe bits in efuse */
          /* Zero out the 4 pipes first */
          model_val &= 0xfffffe1f;
          /* Set the disabled pipes */
          model_val |= (~pipes_en & 0xf) << 5;
        } else {
          printf("bad pipe_mode parameters\n");
          assert(0);
        }
        printf(" EFUSE:Idx %d, sku %d, pipe_mode is %d, pipes_en_bmp 0x%x, "
               "model-val 0x%x \n",
               idx, g_sku, g_pipe_mode, g_phy_pipes_en_bmp, model_val);
      }
      /* Chip Part revision no is bits 156-163 */
      /* Set bits 156-159 of part revision no*/
      if (idx == 4) {
        model_val &= 0x0fffffff;
        model_val |= ((g_chip_part_rev_no & 0xf) << 28);
      }
      /* Set bits 160-163 of part revision no*/
      if (idx == 5) {
        model_val &= 0xfffffff0;
        model_val |= ((g_chip_part_rev_no >> 4) & 0xf);
      }
    }
    if (g_debug_mode) {
      printf("MDL: reg_rd: chip=%d : addr=%x : data=%x\n", asic, addr,
             model_val);
    }
    return model_val;
  } else if (!g_rtl_integ && simulated_mac_status_register(addr)) {
    // return corresponding bf_dev_port_t status from port monitoring code
    return (simulated_mac_status_register_value(asic, addr));
  }
  if (!g_rtl_integ || g_parallel_mode) {
    if (g_debug_mode) {
      printf("MDL: reg_rd: chip=%d : addr=%x ...\n", asic, addr);
    }
    model_val = model_reg_read(asic, addr);
    if (g_debug_mode) {
      printf("MDL: reg_rd: chip=%d : addr=%x : data=%x\n", asic, addr,
             model_val);
    }
  }
  if (g_rtl_integ) {

    printf("RTL: reg_rd: chip=%d : addr=%x\n", asic, addr);

    bf_hdr_t hdr;
    hdr.typ = TYP_RD_32;
    hdr.u.rd_wr_32.chip = asic;
    hdr.u.rd_wr_32.address = addr;
    hdr.u.rd_wr_32.data_31_0 = 0xdeadbeef;
    write_proxy_reg_chnl((uint8_t *)&hdr, sizeof(hdr));
    read_proxy_reg_chnl((uint8_t *)&hdr, sizeof(hdr));
    if (g_parallel_mode && (hdr.u.rd_wr_32.data_31_0 != model_val)) {
      printf("Warning: asic=%d : addr=%08x : model read=%08x : rtl read=%08x\n",
             asic, addr, model_val, hdr.u.rd_wr_32.data_31_0);
    }
    model_val = hdr.u.rd_wr_32.data_31_0;

    printf("RTL: reg_rd: chip=%d : addr=%x : data=%x\n", asic, addr, model_val);
  }

  return model_val;
}

/***************************************************************************
* simulator_ind_write
***************************************************************************/
void simulator_ind_write(int asic, uint64_t addr, uint64_t data0,
                         uint64_t data1) {
  if (!g_rtl_integ || g_parallel_mode) {
    if (g_debug_mode) {
      printf("MDL: ind_wr: chip=%d : addr=0x%" PRIx64 " : data0=0x%" PRIx64
             " : data1=0x%" PRIx64 "\n",
             asic, addr, data0, data1);
    }
    model_ind_write(asic, addr, data0, data1);
  }
  if (g_rtl_integ) {

    bf_hdr_t hdr;
    hdr.typ = TYP_WR_128;
    hdr.u.rd_wr_128.chip = asic;
    hdr.u.rd_wr_128.address = addr;
    hdr.u.rd_wr_128.data_127_64 = data1;
    hdr.u.rd_wr_128.data_63_0 = data0;
    write_proxy_reg_chnl((uint8_t *)&hdr, sizeof(hdr));
  }
}

/***************************************************************************
* simulator_ind_read
***************************************************************************/
void simulator_ind_read(int asic, uint64_t addr, uint64_t *data0,
                        uint64_t *data1) {
  if (!g_rtl_integ || g_parallel_mode) {
    if (g_debug_mode) {
      printf("MDL: ind_rd: chip=%d : addr=0x%" PRIx64 " ...\n", asic, addr);
    }
    model_ind_read(asic, addr, data0, data1);
    if (g_debug_mode) {
      printf("MDL: ind_rd: chip=%d : addr=0x%" PRIx64 " : data0=0x%" PRIx64
             " : data1=0x%" PRIx64 "\n",
             asic, addr, *data0, *data1);
    }
  }
  if (g_rtl_integ) {

    bf_hdr_t hdr;
    hdr.typ = TYP_RD_128;
    hdr.u.rd_wr_128.chip = asic;
    hdr.u.rd_wr_128.address = addr;
    hdr.u.rd_wr_128.data_127_64 = 0xdeadbeefull;
    hdr.u.rd_wr_128.data_63_0 = 0xdeadbeefull;
    write_proxy_reg_chnl((uint8_t *)&hdr, sizeof(hdr));

    read_proxy_reg_chnl((uint8_t *)&hdr, sizeof(hdr));
    if ((hdr.u.rd_wr_128.data_127_64 != *data1) ||
        (hdr.u.rd_wr_128.data_63_0 != *data0)) {
      printf("Warning: asic=%d : addr=%016" PRIx64 "\n"
             "       : model data1=%016" PRIx64 " : data0=%016" PRIx64 "\n"
             "       : rtl   data1=%016" PRIx64 " : data0=%016" PRIx64 "\n",
             asic, addr, *data1, *data0, hdr.u.rd_wr_128.data_127_64,
             hdr.u.rd_wr_128.data_63_0);
      *data1 = hdr.u.rd_wr_128.data_127_64;
      *data0 = hdr.u.rd_wr_128.data_63_0;
    }
  }
}

/***************************************************************************
* simulator_incr_time
***************************************************************************/
void simulator_incr_time(uint64_t amount) {
  rmt_timer_increment(amount);
}

extern void write_prxy_int_chnl(uint8_t *buf, int len);
extern void read_prxy_int_chnl(uint8_t *buf, int len);
extern void write_prxy_dma_chnl(uint8_t *buf, int len);
extern void read_prxy_dma_chnl(uint8_t *buf, int len);
extern int push_dma_from_dru(pcie_msg_t *msg, int len, uint8_t *buf);

/***************************************************************************
* simulator_proxy_dma_op
***************************************************************************/
uint8_t dma_buffer[65536];

void service_proxy_dma_requests(void) {
  bf_hdr_t hdr;
  pcie_msg_t proxyd_msg;

  read_prxy_dma_chnl((uint8_t *)&hdr, sizeof(hdr));

  printf("PXY: DMA: op=%s : addr=0x%" PRIx64 " : len=%d\n",
         (hdr.typ == TYP_DMA_WR) ? "Wr" : (hdr.typ == TYP_DMA_RD) ? "Rd" : "??",
         hdr.u.dma_rd_wr.remote_address, hdr.u.dma_rd_wr.len_of_data);

  if (hdr.typ == TYP_DMA_WR) {
    proxyd_msg.typ = pcie_op_dma_wr;
    proxyd_msg.ind_addr = hdr.u.dma_rd_wr.remote_address;
    proxyd_msg.len = hdr.u.dma_rd_wr.len_of_data;
    read_prxy_dma_chnl(dma_buffer, hdr.u.dma_rd_wr.len_of_data);
    push_dma_from_dru(&proxyd_msg, sizeof(proxyd_msg), dma_buffer);
  } else if (hdr.typ == TYP_DMA_RD) {
    proxyd_msg.typ = pcie_op_dma_rd;
    proxyd_msg.ind_addr = hdr.u.dma_rd_wr.remote_address;
    proxyd_msg.len = hdr.u.dma_rd_wr.len_of_data;
    push_dma_from_dru(&proxyd_msg, sizeof(proxyd_msg), dma_buffer);
    printf("PXY: DMA: forward data to emulator\n");
    write_prxy_dma_chnl(dma_buffer, hdr.u.dma_rd_wr.len_of_data);
  } else
    assert(0);
}

void proxy_packet_handler(int asic_id, int asic_port, uint8_t *pkt, int len) {
  bf_hdr_t hdr;

  hdr.typ = TYP_PKT_RX;
  hdr.len = sizeof(hdr);
  hdr.u.pkt_rd_wr.chip = asic_id;
  hdr.u.pkt_rd_wr.direction = 0; // ingress
  hdr.u.pkt_rd_wr.port = asic_port;
  hdr.u.pkt_rd_wr.len_of_data = len;

  write_rx_pkt_chnl((uint8_t *)&hdr, sizeof(hdr));
  write_rx_pkt_chnl((uint8_t *)pkt, len);
}

#if 0
extern void packet_emission_control( int org,
                                     int asic_id, int port_id, uint8_t *pkt, int len);
extern void display_pkt( uint8_t *pkt, int len );
#endif

void *rtl_tx_pkt_service_thread_entry(void *param) {
  (void)param;
  bf_hdr_t hdr;
  uint8_t pkt[9216];

  printf("RTL Proxy Tx Pkt server starting..\n");
  while (1) {
    read_tx_pkt_chnl((uint8_t *)&hdr, sizeof(hdr));

    printf("Proxy: Tx Pkt (from rtl): asic=%d : port=%d : len=%d\n",
           hdr.u.pkt_rd_wr.chip, hdr.u.pkt_rd_wr.port,
           hdr.u.pkt_rd_wr.len_of_data);

    read_tx_pkt_chnl(pkt, hdr.u.pkt_rd_wr.len_of_data);
    display_pkt(pkt, hdr.u.pkt_rd_wr.len_of_data);
    packet_emission_control(1 /*PKT_FROM_RTL*/, hdr.u.pkt_rd_wr.chip,
                            hdr.u.pkt_rd_wr.port, pkt,
                            hdr.u.pkt_rd_wr.len_of_data);
  }
  return NULL;
}
