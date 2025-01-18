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

#include <sched.h>
#include <pthread.h>
#include <linux/tcp.h>
#include <assert.h>
#include <semaphore.h>

#include<stdio.h>
#include<stdlib.h>
#include<string.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include <errno.h>
#include <inttypes.h>

#include<bf_types/bf_types.h>
#include<dru_sim/dru_sim.h>
#include<dru_sim/dru_mti.h>
#include<lld/bf_dma_dr_id.h>

#define TOFINO_MODEL_DEBUG_MODE       0

// Default TCP ports used for DRU communication
// Can be overridden on the command line
//
#define TCP_PORT_BASE_DEFAULT         8001
#define REG_CHNL_PORT_DEFAULT         8001
#define DMA_CHNL_PORT_DEFAULT         8002
#define PRXY_REG_CHNL_PORT_DEFAULT    8003
#define PRXY_DMA_CHNL_PORT_DEFAULT    8004
#define PRXY_PKT_TX_CHNL_PORT_DEFAULT 8005
#define PRXY_PKT_RX_CHNL_PORT_DEFAULT 8006
#define PRXY_INT_CHNL_PORT_DEFAULT    8007
#define DV_REG_CHNL_PORT_DEFAULT      8008

// Offset from g_tcp_port_base
#define REG_CHNL_PORT         0
#define DMA_CHNL_PORT         1
#define PRXY_REG_CHNL_PORT    2
#define PRXY_DMA_CHNL_PORT    3
#define PRXY_PKT_TX_CHNL_PORT 4
#define PRXY_PKT_RX_CHNL_PORT 5
#define PRXY_INT_CHNL_PORT    6
#define DV_REG_CHNL_PORT      7

/************************************************************
* TCP port base for a run. Can be modified on the command
* line.
************************************************************/
int g_tcp_port_base = TCP_PORT_BASE_DEFAULT;

int reg_chnl;
int dma_chnl;
int dv_reg_chnl;

// proxy connections for RTL integration
int proxy_reg_chnl;
int proxy_pkt_tx_chnl;
int proxy_pkt_rx_chnl;
int proxy_int_chnl;
int proxy_dma_chnl;

int g_debug_mode = TOFINO_MODEL_DEBUG_MODE;
static pthread_mutex_t emu_socket_lock = PTHREAD_MUTEX_INITIALIZER;

// interfaces to the model code, in model_intf.c
//
extern         void simulator_reg_write( int asic, unsigned int addr, unsigned int value );
extern unsigned int simulator_reg_read(  int asic, unsigned int addr );
extern void         simulator_ind_write( int asic, uint64_t address, uint64_t data0, uint64_t data1 );
extern void         simulator_ind_read(  int asic, uint64_t address, uint64_t *data0, uint64_t *data1 );
extern int   g_rtl_integ;
extern char *g_rtl_ip_address;
extern int   g_include_chip_dv_socket;

// interfaces to the DRU simulation code in lldif/module/src/lldif/dru_sim.c
//
extern void dru_hdl_pcie_fifo(void);

//lld_dr_id type to support lld_validate_dr_id() in model lib
typedef enum {
#define LLD_DR_ID(x) lld_dr_##x
  BF_DMA_DR_IDS,
  BF_DMA_MAX_DR,
  BF_DMA_MAX_TOF_DR = lld_dr_cmp_tx_pkt_3,
  BF_DMA_MAX_TOF2_DR = lld_dr_cmp_que_read_block_1,
  BF_DMA_MAX_TOF3_DR = lld_dr_cmp_que_read_block_1,
  BF_DMA_MAX_TF5_DR = lld_dr_cmp_que_read_block_1,  // FTR
  BF_DMA_NO_DR = 0xff,
#undef LLD_DR_ID
} bf_dma_dr_id_t;

extern bool rmt_get_type(uint8_t asic_id, uint8_t *chip_type);

pthread_t dru_service_thread;
extern void *dru_service_thread_entry(void *param);
void *null_parm;

int socket_desc;

#ifndef __cplusplus
#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif
#endif

/*********************************************************************
* create_server
*********************************************************************/
int create_server( int listen_port )
{
    int client_sock , c;
    struct sockaddr_in server , client;
    int so_reuseaddr = 1;

    close( socket_desc ); // close any previous version

    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
        return 1;
    }
    printf("Listen socket created\n");

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( listen_port );

    setsockopt(socket_desc,SOL_SOCKET,SO_REUSEADDR, &so_reuseaddr, sizeof so_reuseaddr);

    do {
        //Bind
        if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
        {
            //print the error message
            perror("bind failed. Error");
//            return 1;
            sleep(1);
        }
        else {
            break;
        }
    } while (1);

    printf("bind done on port %d. Listening..\n", listen_port);
    //Listen
    listen(socket_desc , 1);

    //Accept an incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);

    //accept connection from an incoming client
    client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
    if (client_sock < 0)
    {
        perror("accept failed");
        return 1;
    }
    printf("Connection accepted on port %d\n", listen_port);

    return client_sock;
}

/*********************************************************************
* create_client
*********************************************************************/
int create_client( char *ip_addr_str, int server_port )
{
    int sock;
    struct sockaddr_in server;

    //Create socket
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1) {
        printf("Could not create socket");
        exit(1);
    }
    printf("Client socket created\n");

retry:
    server.sin_addr.s_addr = inet_addr( ip_addr_str );
    server.sin_family = AF_INET;
    server.sin_port = htons( server_port );

    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        sleep(1);
        goto retry;
    }

    printf("Connected on port %d\n", server_port);
    return sock;
}

/*********************************************************************
* init_comms
*********************************************************************/
void init_comms( int dru_mode /*0=cpu, 1=dru*/)
{
    if (dru_mode) {
        reg_chnl = create_server( g_tcp_port_base + REG_CHNL_PORT );
        dma_chnl = create_client( (char*)"127.0.0.1", g_tcp_port_base + DMA_CHNL_PORT );

        if (g_include_chip_dv_socket) {
            dv_reg_chnl = create_server( g_tcp_port_base + DV_REG_CHNL_PORT );
            printf("Chip DV interface created.\n");
        }

        if (g_rtl_integ) { // create proxy connections
            printf("RTL_INTEG: Creating proxy connections to RTL model\n");
            proxy_reg_chnl    = create_client( (char*)g_rtl_ip_address, g_tcp_port_base + PRXY_REG_CHNL_PORT );
            proxy_dma_chnl    = create_server( g_tcp_port_base + PRXY_DMA_CHNL_PORT );
            proxy_pkt_rx_chnl = create_client( (char*)g_rtl_ip_address, g_tcp_port_base + PRXY_PKT_RX_CHNL_PORT );
            proxy_pkt_tx_chnl = create_server( g_tcp_port_base + PRXY_PKT_TX_CHNL_PORT );
            proxy_int_chnl    = create_server( g_tcp_port_base + PRXY_INT_CHNL_PORT );
            printf("TCP ports:\n");
            printf(": PCIe register access   : %d (server)\n", g_tcp_port_base + REG_CHNL_PORT);
            printf(": DMA (harlyn-bfd)       : %d (client)\n", g_tcp_port_base + DMA_CHNL_PORT );
            printf(": Proxy PCIe access      : %d (client)\n", g_tcp_port_base + PRXY_REG_CHNL_PORT );
            printf(": Proxy DMA (rtl-harlyn) : %d (server)\n", g_tcp_port_base + PRXY_DMA_CHNL_PORT );
            printf(": Proxy Rx packets       : %d (client)\n", g_tcp_port_base + PRXY_PKT_RX_CHNL_PORT );
            printf(": Proxy Tx packets       : %d (server)\n", g_tcp_port_base + PRXY_PKT_TX_CHNL_PORT );
            printf(": Proxy Interrupts       : %d (server)\n", g_tcp_port_base + PRXY_INT_CHNL_PORT );
        }
    }
    else {
        reg_chnl = create_client( (char*)"127.0.0.1", g_tcp_port_base + REG_CHNL_PORT );
        dma_chnl = create_server( g_tcp_port_base + DMA_CHNL_PORT );
    }

}

extern void post_possible_dru_work(void);
int terminate_dru_thread = 0;

/*********************************************************************
* read_from_socket
*********************************************************************/
int read_from_socket( int sock, uint8_t *buf, int len )
{
    int n_read= 0, n_read_this_time = 0, i=1;

    //Receive a message from client
    do {
        n_read_this_time = recv( sock, (buf + n_read), (len - n_read), 0/*MSG_WAITALL*/ );
        if (n_read_this_time > 0) {
            n_read += n_read_this_time;
            if (n_read < len) {
                printf("Partial recv: %d of %d so far..\n", n_read, len );
            }
        }
        else if (n_read_this_time == 0) {
            close( sock );
            // also tell dru thread to terminate
            terminate_dru_thread = 1;
            post_possible_dru_work();
            return -1;
        }
        else {
            int error = errno;
            if (error == EWOULDBLOCK) {
                printf("read_from_socket: EWOULDBLOCK\n");
            }
            else if (error == EINTR) {
                printf("read_from_socket: EINTR\n");
            }
            else {
                close( sock );
                return -1;
            }
        }
        setsockopt(sock, IPPROTO_TCP, TCP_QUICKACK, (void *)&i, sizeof(i));

    } while( n_read < len );
    return n_read;
}

/*********************************************************************
* write_to_socket
*********************************************************************/
void write_to_socket( int sock, uint8_t *buf, int len )
{
    if( send(sock, buf, len, 0) < 0) {
        puts("Send failed");
        exit(3);
    }
}

/*********************************************************************
* write_dv_reg_chnl
*********************************************************************/
void write_dv_reg_chnl( uint8_t *buf, int len )
{
    write_to_socket( dv_reg_chnl, buf, len );
}

/*********************************************************************
* write_reg_chnl
*********************************************************************/
static void write_reg_chnl( uint8_t *buf, int len )
{
    write_to_socket( reg_chnl, buf, len );
}

/*********************************************************************
* mod_write_dma_chnl
*********************************************************************/
void mod_write_dma_chnl( uint8_t *buf, int len )
{
    write_to_socket( dma_chnl, buf, len );
}

/*********************************************************************
* read_dv_reg_chnl
*********************************************************************/
int read_dv_reg_chnl( uint8_t *buf, int len )
{
    return read_from_socket( dv_reg_chnl, buf, len );
}

/*********************************************************************
* read_reg_chnl
*********************************************************************/
static int read_reg_chnl( uint8_t *buf, int len )
{
    return read_from_socket( reg_chnl, buf, len );
}

/*********************************************************************
* mod_read_dma_chnl
*********************************************************************/
int mod_read_dma_chnl( uint8_t *buf, int len )
{
    return read_from_socket( dma_chnl, buf, len );
}

/*********************************************************************
* write_proxy_reg_chnl
*********************************************************************/
void write_proxy_reg_chnl( uint8_t *buf, int len )
{
    pthread_mutex_lock(&emu_socket_lock);
    write_to_socket( proxy_reg_chnl, buf, len );
    pthread_mutex_unlock(&emu_socket_lock);
}

/*********************************************************************
* read_proxy_reg_chnl
*********************************************************************/
int read_proxy_reg_chnl( uint8_t *buf, int len )
{
    return read_from_socket( proxy_reg_chnl, buf, len );
}

/*********************************************************************
* write_rx_pkt_chnl
*********************************************************************/
void write_rx_pkt_chnl( uint8_t *buf, int len )
{
    write_to_socket( proxy_pkt_rx_chnl, buf, len );
}

/*********************************************************************
* read_tx_pkt_chnl
*********************************************************************/
void read_tx_pkt_chnl( uint8_t *buf, int len )
{
    read_from_socket( proxy_pkt_tx_chnl, buf, len );
}

/*********************************************************************
* write_prxy_int_chnl
*********************************************************************/
void write_prxy_int_chnl( uint8_t *buf, int len )
{
    write_to_socket( proxy_int_chnl, buf, len );
}

/*********************************************************************
* read_prxy_int_chnl
*********************************************************************/
void read_prxy_int_chnl( uint8_t *buf, int len )
{
    read_from_socket( proxy_int_chnl, buf, len );
}

/*********************************************************************
* write_prxy_dma_chnl
*********************************************************************/
void write_prxy_dma_chnl( uint8_t *buf, int len )
{
    write_to_socket( proxy_dma_chnl, buf, len );
}

/*********************************************************************
* read_prxy_dma_chnl
*********************************************************************/
void read_prxy_dma_chnl( uint8_t *buf, int len )
{
    read_from_socket( proxy_dma_chnl, buf, len );
}

/*********************************************************************
* push_to_model
*
* Service a simulated PCIe register access from the LLD to/from a
* model register.
*********************************************************************/
uint32_t mod_push_to_model( int typ, unsigned int asic, uint64_t reg, uint64_t data )
{
    if (typ == pcie_op_wr) {
        simulator_reg_write( asic, (unsigned int)reg, (unsigned int)data );
    }
    else if (typ == pcie_op_rd) {
        return( simulator_reg_read( asic, (unsigned int)reg ) );
    }
    return 0;
}

/********************************************************************
* push_dma_from_dru
*
* This function is used when the DRU simulator is linked
* with the model (iinstead of the LLD).
*
* It is called by the DRU to push a DMA request
* to the LLD
********************************************************************/
int push_dma_from_dru( pcie_msg_t *msg, int len, uint8_t *buf )
{
    if (g_debug_mode) {

        printf("push_dma_from_dru: (%s) addr=%"PRIx64", dma-len=%d\n",
               (msg->typ == pcie_op_dma_rd) ? "Rd" :
               (msg->typ == pcie_op_dma_wr) ? "Wr" : "???",
               msg->ind_addr, msg->len );
    }

    mod_write_dma_chnl( (unsigned char*)msg, sizeof(pcie_msg_t));

    if (msg->typ == pcie_op_dma_rd) { // read
        int n_read = 0;

        n_read = mod_read_dma_chnl( buf, msg->len );

        if (g_debug_mode) {
            int i;

            printf("Host-to-DRU: %d: %08"PRIx64" : len=%d\n",
                       msg->asic, msg->ind_addr, msg->len);
            printf("Data:");
            for (i=0; i < (int)min(msg->len,16);i++) {
                printf("%s%08x ", (i % 8) == 0 ? "\n":"",
                *((uint32_t*)(&buf[4*i])));
            }
            printf("\n");
        }
        return( n_read );
    }
    else if (msg->typ == pcie_op_dma_wr) { // write
        if (g_debug_mode) {
            int i;

            printf("DRU-to-Host: %d: %08"PRIx64" : len=%d\n",
                       msg->asic, msg->ind_addr, msg->len);
            printf("Data:");
            for (i=0; i < (int)min(msg->len/4,16);i++) {
                printf("%s%08x ", (i % 8) == 0 ? "\n":"",
                *((uint32_t*)(buf + 4*i)));
            }
            printf("\n");
        }
        mod_write_dma_chnl( (unsigned char*)buf, msg->len );
    }
    return 0;
}

/*********************************************************************
* write_vpi_link
*********************************************************************/
void mod_write_vpi_link( pcie_msg_t *msg )
{
    write_reg_chnl( (uint8_t *)msg, sizeof(pcie_msg_t) );
}

/*********************************************************************
* service_chip_dv_reg_requests
*********************************************************************/
void service_chip_dv_reg_requests(void)
{
    pcie_msg_t msg;

    read_dv_reg_chnl( (uint8_t *)&msg, sizeof(pcie_msg_t) );
    if (msg.typ == pcie_op_wr) {
        simulator_reg_write( msg.asic, msg.addr, msg.value );
    }
    else if (msg.typ == pcie_op_rd) {
        msg.value = simulator_reg_read( msg.asic, msg.addr );
        write_dv_reg_chnl( (uint8_t *)&msg, sizeof(pcie_msg_t) );
    }
    else {
        printf("service_chip_dv_reg_requests: unknown msg type: %d\n", msg.typ );
    }
}

/*********************************************************************
* check_dru_ctrl_chnl
*********************************************************************/
void *check_dru_ctrl_chnl( unsigned char*buf, int len )
{
    pcie_msg_t *msg = (pcie_msg_t*)buf;
    int n;

    n = read_reg_chnl( buf, len );

    if (n == -1) {
        // socket closed. Presumably bf-drivers terminated
        printf("Reg channel closed. Restart reg connection...\n");
        return NULL;
    }

    if (0 /*g_debug_mode*/) {
        if ((msg->typ == pcie_op_wr) || (msg->typ == pcie_op_rd)) {
            printf("DRU rcvd: typ=%d (%s), asic=%d: addr=%08"PRIx64" = %08x\n",
                   msg->typ,
                   (msg->typ == pcie_op_rd) ? "pcie_op_rd" :
                   (msg->typ == pcie_op_wr) ? "pcie_op_wr" : "unexpected",
                   msg->asic, msg->addr, msg->value);
        }
    }
    return( buf );
}

/*********************************************************************
* check_vpi_link_cpu
*********************************************************************/
void *check_vpi_link_cpu( unsigned char*buf, int len )
{
    printf("Hmm, check_vpi_link_cpu shouldnt be getting called for this target\n");
    return NULL;
}

uint8_t *get_lld_dma_memory( int chip, int min_sz, int *rtnd_sz )
{
    uint8_t *mem = (uint8_t *)malloc( min_sz );

    if (rtnd_sz) *rtnd_sz = min_sz;
    memset(mem, 0, min_sz);

    return( mem );
}

uint8_t *get_pipemgr_dma_memory( int chip, int min_sz, int *rtnd_sz )
{
    if (rtnd_sz) *rtnd_sz = min_sz;
    return( (uint8_t *)malloc( min_sz ) );
}

bool lld_dev_is_tofino(bf_dev_id_t dev_id) {
  uint8_t t = 0;
  rmt_get_type(dev_id, &t);
  if (t == 1 || t == 2) return true;
  return false;
}

bool lld_dev_is_tof2(bf_dev_id_t dev_id) {
  uint8_t t = 0;
  rmt_get_type(dev_id, &t);
  if (t == 4 || t == 5) return true;
  return false;
}

bool lld_dev_is_tof3(bf_dev_id_t dev_id) {
  uint8_t t = 0;
  rmt_get_type(dev_id, &t);
  if (t == 6) return true;
  return false;
}

bool lld_dev_is_tf5(bf_dev_id_t dev_id) {
  uint8_t t = 0;
  rmt_get_type(dev_id, &t);
  if (t == 8) return true;
  return false;
}

bf_dev_family_t lld_dev_family_get(bf_dev_id_t dev_id) {
    if(lld_dev_is_tofino(dev_id)) return BF_DEV_FAMILY_TOFINO;
    else if(lld_dev_is_tof2(dev_id)) return BF_DEV_FAMILY_TOFINO2;
    else if(lld_dev_is_tof3(dev_id)) return BF_DEV_FAMILY_TOFINO3;
    else if(lld_dev_is_tf5(dev_id)) return BF_DEV_FAMILY_TOFINO5; //FTR
    else return BF_DEV_FAMILY_UNKNOWN;
}

int lld_validate_dr_id(bf_dev_id_t dev_id, bf_dma_dr_id_t dr_id) {
  bf_dev_family_t dev_family = lld_dev_family_get(dev_id);
  if (dev_family == BF_DEV_FAMILY_UNKNOWN) return -1;
  switch (dev_family) {
    case BF_DEV_FAMILY_TOFINO:
      if (dr_id > BF_DMA_MAX_TOF_DR) return -1;
      break;
    case BF_DEV_FAMILY_TOFINO2:
      if (dr_id > BF_DMA_MAX_TOF2_DR) return -1;
      break;
    case BF_DEV_FAMILY_TOFINO3:
      if (dr_id > BF_DMA_MAX_TOF3_DR) return -1;
      break;
    case BF_DEV_FAMILY_TOFINO5: /* FTR_S */
      if (dr_id > BF_DMA_MAX_TF5_DR) return -1;
      break;
      /* FTR_E */
    default:
      return -1;
  }
  return 0;
}
