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

#ifndef SIMULATOR_INTF_H_INCLUDED
#define SIMULATOR_INTF_H_INCLUDED

typedef enum {
    PKT_FROM_MODEL = 0,
    PKT_FROM_RTL,
} pkt_originator_e;

typedef enum {
    TYP_INVALID = 0,
    // register direct and indirect read/write (from CPU prespective)
    TYP_WR_32,
    TYP_RD_32,
    TYP_WR_128,
    TYP_RD_128,
    // DMA (from chip perspective)
    TYP_DMA_RD,
    TYP_DMA_WR,
    // PKT (from chip perspective)
    TYP_PKT_TX,
    TYP_PKT_RX,
    // Interrupt
    TYP_INT,
} hdr_typ_e;

typedef struct rd_wr_32_t {
    uint32_t chip; // unused by emulator
    uint32_t address;
    uint32_t data_31_0;
} rd_wr_32_t;

typedef struct rd_wr_128_t {
    uint32_t chip; // unused by emulator
    uint64_t address;
    uint64_t data_127_64;
    uint64_t data_63_0;
} rd_wr_128_t;

typedef struct interrupt_t {
    uint32_t chip; // unused by emulator
    uint32_t int_num;
    uint64_t assoc_data; // if any
} interrupt_t;

typedef struct dma_rd_wr_t {
    uint32_t chip; // unused by emulator
    uint32_t direction; //0=host-to-chip (Rd), 1=chip_to_host (Wr)
    uint32_t len_of_data;
    uint64_t remote_address;
} dma_rd_wr_t;

typedef struct pkt_rd_wr_t {
    uint32_t chip; // unused by emulator
    uint32_t direction; //0=ingress (Rx), 1=egress (Tx)
    uint32_t port;
    uint32_t len_of_data;
} pkt_rd_wr_t;

typedef struct bf_hdr_t {
    uint32_t typ; // hdr_typ_e 
    uint32_t len; // length of header = sizeof(bf_hdr_t)
    union {
        rd_wr_32_t  rd_wr_32;
        rd_wr_128_t rd_wr_128;
        interrupt_t interrupt;
        dma_rd_wr_t dma_rd_wr;
        pkt_rd_wr_t pkt_rd_wr;
    } u;
} bf_hdr_t;


#endif //SIMULATOR_INTF_H_INCLUDED

