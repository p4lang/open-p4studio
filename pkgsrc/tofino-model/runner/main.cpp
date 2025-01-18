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

/****************************************************************
 *
 * main for thrift based protocol dependent layer based on
 * P4 program from p4-params.mk at top level of repository
 *
 ***************************************************************/

#include <cstdio>
#include <fstream>
#include <iostream>
#include <list>
#include <thread>
#include <mutex>
#include <string>
#include <sstream>

#include <sys/types.h>
#include <pthread.h>
#include <getopt.h>
#include <signal.h>
#include <string.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <assert.h>
#include <map>

extern "C" {
#include <dru_sim/dru_sim.h>
#include <dru_sim/dru_mti.h>

#include "portmanager.h"
#include "simulator_intf.h"
}

#include "os_privs.h"
#include "cli.h"
#include <common/rmt.h>
#include <model_core/model.h>
#include <model_core/p4_target_conf.h>
#include <model_core/port-json-loader.h>
#include <model_core/rmt-debug.h>

//#define COVERAGE
#ifdef COVERAGE
extern "C" {
void __gcov_flush();
}
static void term_handler(int signum) {
    __gcov_flush();
    exit(1);
}
#else
static void term_handler(int signum) {
  // Do nothing - pause should exit
}
#endif
static void int_handler(int signum) {
  // Do nothing - pause should exit
}

extern "C" {
/* Functions to be passed to dru_intf_tcp */
void mod_write_dma_chnl( uint8_t *buf, int len );
int mod_read_dma_chnl( uint8_t *buf, int len );
void mod_write_vpi_link( pcie_msg_t *msg );
uint32_t mod_push_to_model( int typ, unsigned int asic, uint64_t reg, uint64_t data );
}

// Find correct include file for this
typedef struct rmt_logging_context_ {
  // client can store their per device info here - such as log files..
  bool  init_done;
  bool  log_to_file;
  bool  log_to_console;
  char  *log_file_name;
  FILE  *log_file;
  pthread_mutex_t log_lock;
} rmt_logging_ctx_t;

/************************************************************
* Global, set by command line to indicate the simulation
* includes BOTH the asic model AND the RTL model.
************************************************************/
int   g_parallel_mode = 0;

/************************************************************
* Global, set by command line to indicate the simulation
* includes the RTL model.
************************************************************/
int   g_rtl_integ = 0;

/************************************************************
* Global, set by command line to indicate the simulation
* includes the Mentor emulator.
************************************************************/
int   g_emu_integ = 0;
/************************************************************
* Global, set by command line to indicate the IP address of
* the machine running the RTL model.
************************************************************/
const char *g_rtl_ip_address = "127.0.0.1";

/************************************************************
* Global, set by command line to indicate the Tofino SKU
* takes values
* 77110 = 0, 77120 = 1, 77121_even = 2-5, 77121_odd =6-9
* 77131_even = 11-14, 77131_odd = 15-18, 77140 = 19
* includes BOTH the asic model AND the RTL model.
************************************************************/
int   g_sku = -1;

/************************************************************
* Global, set by command line to indicate even or odd pipe Tofino SKU
* takes three values, full = 0, and 1, 2, 3 and 4 for _02,
* _03, _12 and _13 pipe modes respectively
* includes BOTH the asic model AND the RTL model.
************************************************************/
int   g_pipe_mode = -1;
/************************************************************
* Global,indicates mapping from logical pipe to physical pipe
************************************************************/
int   g_log2phy_pipe[8];
/************************************************************
* Global,bitmap of physical pipes that are enabled
* (*** SHOULD BE CONFIGURABLE PER-CHIP ***)
************************************************************/
int   g_phy_pipes_en_bmp = 0;
/************************************************************
* Global,number of stages active per pipe
* (*** SHOULD BE CONFIGURABLE PER-CHIP ***)
************************************************************/
int   g_num_stages = 0;
/************************************************************
* Global, set by command line to control if the chip-dv
* socket should be created. This is needed by the DV team
* for their tcl interface
************************************************************/
int g_include_chip_dv_socket = 0;

/************************************************************
* Global, set by command line to indicate the simulation
* should assert on reads/writes of unimplemented registers.
* Default is to return 0x0bad0bad and silently drop writes.
************************************************************/
int   g_return_0bad = 1;
/************************************************************
* Global, set by command line to put ports in internal loopback
* mode. useful for emulating mac or phy lopback mode.
  Stores pipe bitmap
************************************************************/
static int internal_port_loopback = 0;
/************************************************************
* Global, set the chip part revision number
************************************************************/
uint8_t g_chip_part_rev_no = 0;

/************************************************************
* Global, set the package size to indicate number of die's
* For TOFINO, JBAY, FLATROCK Package size is always 1 for CB
* it could be 1 or 2.
* Set the pipes per die to the maximum number of pipes possible on a die.
************************************************************/
int g_pkg_size = 1;
int g_pipes_per_die = 4;

/************************************************************
* Global, set by command line to control if the model time needs to increment
************************************************************/
bool time_disable = false;

extern int g_debug_mode;
extern int g_tcp_port_base;
static int cli_port=8000;
//extern ucli_module_t* harlyn_ucli_module_create(void);

/**
 * The maximum number of ports to support:
 * @fixme should be runtime parameter
 */
#define PORT_COUNT 512

int g_port_count = 17;

/* @fixme This is for pcap dumps only right now */
static const char *switch_name = "harlyn";
static int do_pcap_dump = 0;

/**
 * Check an operation and return if there's an error.
 */
#define CHECK(op)                                                       \
    do {                                                                \
        int _rv;                                                        \
        if ((_rv = (op)) < 0) {                                         \
            fprintf(stderr, "%s: ERROR %d at %s:%d",                    \
                    #op, _rv, __FILE__, __LINE__);                      \
            return _rv;                                                 \
        }                                                               \
    } while (0)

extern int start_bfn_api_rpc_server(void);

extern "C" {
extern void write_rx_pkt_chnl( uint8_t *buf, int len );
extern void write_tx_pkt_chnl( uint8_t *buf, int len );
extern void read_tx_pkt_chnl( uint8_t *buf, int len );
extern void *rtl_tx_pkt_service_thread_entry(void *param);
void common_packet_handler(uint32_t port_num, uint8_t *pkt, int len, int orig_len);
}

bool port_status_monitor_thread_started = false;
pthread_t port_status_monitor_thread;

#if 0
/*************************************************************
* pkt_msg_t
*
* This structure is exchanged with the RTL-side shim-layer and
* must remain consistent with it.
**************************************************************/
typedef struct pkt_msg_t {
    int chip;
    int ingress_port;
    int pkt_len;
} pkt_msg_t;

typedef enum {
    PKT_FROM_MODEL = 0,
    PKT_FROM_RTL,
} pkt_originator_e;
#endif

//fwd ref
void nq_pkt( pkt_originator_e org,
             int chip, int port, uint8_t *pkt, int len );
int match_pkt( pkt_originator_e org,
             int chip, int port, uint8_t *pkt, int len );

extern "C" {


static void print_model_version(void)
{
    printf("tofino-model %s\n", tmodel_get_sw_version());
}

static void print_model_int_version(void)
{
    printf("tofino-model %s\n", tmodel_get_internal_version());
}


static bool        chip_types_initialised = false;
static const char *chip_type_names[32] = { NULL };
static char        chip_types_available_buf[256];

static void chip_type_names_set(int type, const char *name) {
  assert((type >= 0) && (type < 32));
  chip_type_names[type] = name;
}
static void chip_type_names_init(void) {
  chip_type_names_set(model_core::ChipType::kTofino, "Tofino");
  chip_type_names_set(model_core::ChipType::kTofinoB0, "TofinoB0");
  chip_type_names_set(model_core::ChipType::kJbay, "JBay");
  chip_type_names_set(model_core::ChipType::kJbayB0, "JBayB0");
  chip_type_names_set(model_core::ChipType::kRsvd0, "CloudBreak");
  chip_type_names_set(model_core::ChipType::kRsvd2, "CloudBreak"); // No B0 hopefully!
  chip_type_names_set(model_core::ChipType::kRsvd3, "FlatRock");
}
static uint32_t chip_types_available(void) {
  uint32_t chip_types = 0u;
#ifdef MODEL_TOFINO
  chip_types |= 1u << model_core::ChipType::kTofino;
#endif
#ifdef MODEL_TOFINOB0
  chip_types |= 1u << model_core::ChipType::kTofinoB0;
#endif
#ifdef MODEL_JBAY
  chip_types |= 1u << model_core::ChipType::kJbay;
#endif
#ifdef MODEL_JBAYB0
  chip_types |= 1u << model_core::ChipType::kJbayB0;
#endif
  return chip_types;
}
static void chip_types_available_init(void) {
  uint32_t types = chip_types_available();
  char sep = ' ';
  std::stringstream ss;
  for (int typ = 0; typ < 32; typ++) {
    if (types & (1u << typ)) {
      ss << sep << chip_type_names[typ] << '=' << typ;
      sep = ',';
    }
  }
  auto len = ss.str().length() + 1; // The c_str will have a trailing \0.
  assert(len <= sizeof(chip_types_available_buf));
  memcpy (chip_types_available_buf, ss.str().c_str(), len);
}
static void chip_types_init(void) {
  chip_type_names_init();
  chip_types_available_init();
  chip_types_initialised = true;
}
static bool is_chip_type_available(int type) {
  return ((type >= 0) && (type < 32) &&
          (chip_types_available() & (1u << type)) != 0u);
}
static char *chip_types_available_str(void) {
  if (!chip_types_initialised) chip_types_init();
  return chip_types_available_buf;
}
static const char* chip_type_name(int type) {
  const char *name = ((type >= 0) && (type < 32)) ?chip_type_names[type] :NULL;
  return (name != NULL) ?name :"unknown";
}


// unused functions
//static bool is_tofinoA0(int type) {
//  return (type == model_core::ChipType::kTofinoA0);
//}
//static bool is_tofino(int type) {
//  return is_tofinoA0(type);
//}
//static bool is_tofinoB0(int type) {
//  return (type == model_core::ChipType::kTofinoB0);
//}
//static bool is_tofinoXX(int type) {
//  return is_tofinoA0(type) || is_tofinoB0(type);
//}
static bool is_jbayA0(int type) {
  return (type == model_core::ChipType::kJbayA0);
};
static bool is_jbay(int type) {
  return is_jbayA0(type);
};
static bool is_jbayB0(int type) {
  return (type == model_core::ChipType::kJbayB0);
};
static bool is_jbayXX(int type) {
  return is_jbay(type) || is_jbayB0(type);
}
static bool is_chip1(int type) {
  return (type == model_core::ChipType::kRsvd0);
};
static bool is_chip1XX(int type) {
  return is_chip1(type);
};
static bool is_chip2(int type) {
  return (type == model_core::ChipType::kRsvd3);
};
static bool is_chip2XX(int type) {
  return is_chip2(type);
};
// unused functions
//static bool is_jbay_or_later(int type) {
// return is_jbayXX(type) || is_chip1(type);
//};
//static bool is_tofino_or_later(int type) {
// return is_tofinoXX(type) || is_jbay_or_later(type);
//};
static bool is_dual_die(int type) {
  return is_chip1XX(type);
}


int rtl_port_map(int port_from_rtl)
{
    if (port_from_rtl < 16) {
        return ((15 - port_from_rtl)  * 4); // works only for 100G mode
    }
    return port_from_rtl;
}

extern "C" {
uint32_t
asic_port_to_global_port(uint8_t asic_id, uint16_t asic_port)
{
    return asic_id * PORT_COUNT + asic_port;
}
}

static void
global_port_to_asic_port(uint32_t port_num, uint8_t *asic_id, uint16_t *asic_port)
{
    *asic_id = port_num / PORT_COUNT;
    *asic_port = port_num % PORT_COUNT;
}

void setup_log2phy_pipe_map() {

    for (int i = 0; (i < (4*g_pkg_size)) && (i<8); i++) {
        g_log2phy_pipe[i] = i;
        g_phy_pipes_en_bmp |= 0x1<<i;
    }
    switch (g_pipe_mode) {
        case 1 :    /* pipes 0_2 enabled */
            g_log2phy_pipe[0] = 0;
            g_log2phy_pipe[1] = 2;
            g_log2phy_pipe[2] = 0xff;
            g_log2phy_pipe[3] = 0xff;
            g_phy_pipes_en_bmp &= ~(0x1<<1);
            g_phy_pipes_en_bmp &= ~(0x1<<3);
            break;
        case 2 : /* pipes 0_3 enabled */
            g_log2phy_pipe[0] = 0;
            g_log2phy_pipe[1] = 3;
            g_log2phy_pipe[2] = 0xff;
            g_log2phy_pipe[3] = 0xff;
            g_phy_pipes_en_bmp &= ~(0x1<<1);
            g_phy_pipes_en_bmp &= ~(0x1<<2);
            break;
        case 3 : /* pipes 1_2 enabled */
            g_log2phy_pipe[0] = 1;
            g_log2phy_pipe[1] = 2;
            g_log2phy_pipe[2] = 0xff;
            g_log2phy_pipe[3] = 0xff;
            g_phy_pipes_en_bmp &= ~(0x1<<0);
            g_phy_pipes_en_bmp &= ~(0x1<<3);
            break;
        case 4 : /* pipes 1_3 enabled */
            g_log2phy_pipe[0] = 1;
            g_log2phy_pipe[1] = 3;
            g_log2phy_pipe[2] = 0xff;
            g_log2phy_pipe[3] = 0xff;
            g_phy_pipes_en_bmp &= ~(0x1<<0);
            g_phy_pipes_en_bmp &= ~(0x1<<2);
            break;
        case 5: /* pipes 0_1 enabled */
            g_log2phy_pipe[0] = 0;
            g_log2phy_pipe[1] = 1;
            g_log2phy_pipe[2] = 0xff;
            g_log2phy_pipe[3] = 0xff;
            g_phy_pipes_en_bmp &= ~(0x1<<2);
            g_phy_pipes_en_bmp &= ~(0x1<<3);
            break;
        case 6: /* pipes 0_1_3 enabled */
            g_log2phy_pipe[0] = 0;
            g_log2phy_pipe[1] = 1;
            g_log2phy_pipe[2] = 3;
            g_log2phy_pipe[3] = 0xff;
            g_phy_pipes_en_bmp &= ~(0x1<<2);
            break;
        case 7: /* pipes 2_3_1 enabled */
            g_log2phy_pipe[0] = 2;
            g_log2phy_pipe[1] = 3;
            g_log2phy_pipe[2] = 1;
            g_log2phy_pipe[3] = 0xff;
            g_phy_pipes_en_bmp &= ~(0x1<<0);
            break;
        case 8: /* pipes 2_3 enabled */
            g_log2phy_pipe[0] = 2;
            g_log2phy_pipe[1] = 3;
            g_log2phy_pipe[2] = 0xff;
            g_log2phy_pipe[3] = 0xff;
            g_phy_pipes_en_bmp &= ~(0x1<<0);
            g_phy_pipes_en_bmp &= ~(0x1<<1);
            break;
        default:
            break;
    }
}

#define DEV_PORT_TO_PIPE(x) (((x) >> 7) & 0x7)
#define DEV_PORT_TO_LOCAL_PORT(x) ((x) & 0x7F)
#define MAKE_DEV_PORT(pipe, port) (((pipe) << 7) | (port))
/* Convert global physical port_num to global logical port-num */
int physical_to_logical(int port_num)
{
    int i, log_port = -1;
    uint16_t phy_port = 0;
    uint8_t asic_id = 0;

    /* Get the asic-id and asic-physical port */
    global_port_to_asic_port(port_num, &asic_id, &phy_port);

    int pipe = DEV_PORT_TO_PIPE(phy_port);
    int port = DEV_PORT_TO_LOCAL_PORT(phy_port);

    /* For Cloudbreak, MAC has 4 channels (2 bit) after channel reduction.
       Logical dev-port used by ptf is still 3 bit.
       Convert 2 bit physical dev-port to 3 bit logical dev-port
    */
    uint8_t chip_type = model_core::ChipType::kDefault;
    rmt_get_type(asic_id, &chip_type);
    if (chip_type == model_core::ChipType::kRsvd0) {
        port = port << 1;
    }
    for (i=0; (i< (4*g_pkg_size)) && (i<8); i++) {
        if (g_log2phy_pipe[i] == pipe) {
            log_port = MAKE_DEV_PORT(i, port);
        }
    }
    if (log_port == -1) {
        printf("Unable to map phy pipe %d to logical pipe \n", pipe);
    } else {
        log_port = asic_port_to_global_port(asic_id, log_port);
    }
    return log_port;
}

/* In PTF space, port numbers are logical, Device 0 is between 0-455,
   Device1 is between 456 and 911.
   Convert global logical port-num to global physical port-num
 */
int logical_to_physical(int port_num)
{
    int phy_port = -1;
    uint16_t log_port = 0;
    uint8_t asic_id = 0;

    /* Get the asic-id and asic-logical port */
    global_port_to_asic_port(port_num, &asic_id, &log_port);

    int pipe = DEV_PORT_TO_PIPE(log_port);
    int port = DEV_PORT_TO_LOCAL_PORT(log_port);
    if (g_log2phy_pipe[pipe] >= (g_pkg_size*4)) {
        return -1;
    }
    if (port > 71) {
        return -1;
    }
    /* For Cloudbreak, MAC has 4 channels (2 bit) after channel reduction.
       Logical dev-port used by ptf is still 3 bit.
       Convert 3 bit logical dev-port to 2 bit physical dev-port
    */
    uint8_t chip_type = model_core::ChipType::kDefault;
    rmt_get_type(asic_id, &chip_type);
    if (chip_type == model_core::ChipType::kRsvd0) {
        port = port >> 1;
    }
    phy_port = MAKE_DEV_PORT(g_log2phy_pipe[pipe], port);
    /* Use asic-id and asic-physical port to get global phy port */
    phy_port = asic_port_to_global_port(asic_id, phy_port);
    return phy_port;
}


/* Tx packet coordination. Packets emitted from both model and RTL
 * are passed here. Packet is transmitted back thru veths only
 * after all participating simulators (model and RTL, if g_rtl_integ)
 * have generated matching packets.
 * If they generate different packets they will be "stuck" here for
 * debugging.
 */
void
packet_emission_control( pkt_originator_e org,
                         int asic_id, int port_id, uint8_t *pkt, int len)
{
    int ret;
    int orig_len = len;
    // Convert from zero based port numbers to one based port numbers.
    // Assume all 100g ports so there is a single port per MAC block, therefor
    // divide the model port number by 4 to convert it to an external port
    // number.

    // Emulator adds CRC. Strip it.
    len = (org == PKT_FROM_RTL) ? len - 4: len;
    int remapped_port = (org == PKT_FROM_RTL) ? rtl_port_map(port_id) : port_id;

    int port = (asic_id * PORT_COUNT) + remapped_port;

    /* If port's pipe is in internal loopback, inject the packet back on
       the same port
    */
    if (internal_port_loopback & (1u << DEV_PORT_TO_PIPE(port))) {
        common_packet_handler(port, pkt, len, orig_len);
        return;
    }

    if (port_status_monitor_thread_started) {
        if (!bfm_is_if_up(port)) {
            printf("WARN: asic %d, asic-port %d transmitting to port %d which is down!\n",
                   asic_id, port_id, port);
            return;//assert(0);
        }
    }

    if (!g_parallel_mode) { // send now
        ret = bfm_port_packet_emit(port, 0, pkt, len);
        if (ret < 0) {
            printf("Error sending packet\n");
        }
    }
    else {
        /* Try to match this packet against those queued by
         * the other simulator. If a match is found, dequeue
         * from the other simulator and send it on the veth.
         * If no match is found, enqueue this packet on this
         * simulators queue for a subsequent match.
         */
        if (match_pkt( org, asic_id, port_id, pkt, len )) {
           printf("RTL-Model packets match: asic=%d : port=%d : len=%d\n",
                  asic_id, port_id, len );
           ret = bfm_port_packet_emit(port, 0, pkt, len);
           if (ret < 0) {
               printf("Error sending packet\n");
           }
        }
        else {
            printf("No match found. Enqueue to %s egress-queue: asic=%d : port=%d : len=%d\n",
                   (org == PKT_FROM_RTL) ? "RTL":"Model",
                   asic_id, port_id, len );
            nq_pkt( org, asic_id, port_id, pkt, len );
        }
    }
}

void display_pkt( uint8_t *pkt, int len) {
    printf("%02X%02X%02X%02X %02X%02X%02X%02X %02X%02X%02X%02X %02X%02X%02X%02X\n",
            len >  0 ? pkt[ 0] : 0x00, len >  1 ? pkt[ 1] : 0x00,
            len >  2 ? pkt[ 2] : 0x00, len >  3 ? pkt[ 3] : 0x00,
            len >  4 ? pkt[ 4] : 0x00, len >  5 ? pkt[ 5] : 0x00,
            len >  6 ? pkt[ 6] : 0x00, len >  7 ? pkt[ 7] : 0x00,
            len >  8 ? pkt[ 8] : 0x00, len >  9 ? pkt[ 9] : 0x00,
            len > 10 ? pkt[10] : 0x00, len > 11 ? pkt[11] : 0x00,
            len > 12 ? pkt[12] : 0x00, len > 13 ? pkt[13] : 0x00,
            len > 14 ? pkt[14] : 0x00, len > 15 ? pkt[15] : 0x00);
    printf("%02X%02X%02X%02X %02X%02X%02X%02X %02X%02X%02X%02X %02X%02X%02X%02X\n",
            len > 16 ? pkt[16] : 0x00, len > 17 ? pkt[17] : 0x00,
            len > 18 ? pkt[18] : 0x00, len > 19 ? pkt[19] : 0x00,
            len > 20 ? pkt[20] : 0x00, len > 21 ? pkt[21] : 0x00,
            len > 22 ? pkt[22] : 0x00, len > 23 ? pkt[23] : 0x00,
            len > 24 ? pkt[24] : 0x00, len > 25 ? pkt[25] : 0x00,
            len > 26 ? pkt[26] : 0x00, len > 27 ? pkt[27] : 0x00,
            len > 28 ? pkt[28] : 0x00, len > 29 ? pkt[29] : 0x00,
            len > 30 ? pkt[30] : 0x00, len > 31 ? pkt[31] : 0x00);
    printf("%02X%02X%02X%02X %02X%02X%02X%02X %02X%02X%02X%02X %02X%02X%02X%02X\n",
            len > 32 ? pkt[32] : 0x00, len > 33 ? pkt[33] : 0x00,
            len > 34 ? pkt[34] : 0x00, len > 35 ? pkt[35] : 0x00,
            len > 36 ? pkt[36] : 0x00, len > 37 ? pkt[37] : 0x00,
            len > 38 ? pkt[38] : 0x00, len > 39 ? pkt[39] : 0x00,
            len > 40 ? pkt[40] : 0x00, len > 41 ? pkt[41] : 0x00,
            len > 42 ? pkt[42] : 0x00, len > 43 ? pkt[43] : 0x00,
            len > 44 ? pkt[44] : 0x00, len > 45 ? pkt[45] : 0x00,
            len > 46 ? pkt[46] : 0x00, len > 47 ? pkt[47] : 0x00);
    printf("%02X%02X%02X%02X %02X%02X%02X%02X %02X%02X%02X%02X %02X%02X%02X%02X\n",
            len > 48 ? pkt[48] : 0x00, len > 49 ? pkt[49] : 0x00,
            len > 50 ? pkt[50] : 0x00, len > 51 ? pkt[51] : 0x00,
            len > 52 ? pkt[52] : 0x00, len > 53 ? pkt[53] : 0x00,
            len > 54 ? pkt[54] : 0x00, len > 55 ? pkt[55] : 0x00,
            len > 56 ? pkt[56] : 0x00, len > 57 ? pkt[57] : 0x00,
            len > 58 ? pkt[58] : 0x00, len > 59 ? pkt[59] : 0x00,
            len > 60 ? pkt[60] : 0x00, len > 61 ? pkt[61] : 0x00,
            len > 62 ? pkt[62] : 0x00, len > 63 ? pkt[63] : 0x00);
    printf("%02X%02X%02X%02X %02X%02X%02X%02X %02X%02X%02X%02X %02X%02X%02X%02X\n",
            len > 64 ? pkt[64] : 0x00, len > 65 ? pkt[65] : 0x00,
            len > 66 ? pkt[66] : 0x00, len > 67 ? pkt[67] : 0x00,
            len > 68 ? pkt[68] : 0x00, len > 69 ? pkt[69] : 0x00,
            len > 70 ? pkt[70] : 0x00, len > 71 ? pkt[71] : 0x00,
            len > 72 ? pkt[72] : 0x00, len > 73 ? pkt[73] : 0x00,
            len > 74 ? pkt[74] : 0x00, len > 75 ? pkt[75] : 0x00,
            len > 76 ? pkt[76] : 0x00, len > 77 ? pkt[77] : 0x00,
            len > 78 ? pkt[78] : 0x00, len > 79 ? pkt[79] : 0x00);
    printf("%02X%02X%02X%02X %02X%02X%02X%02X %02X%02X%02X%02X %02X%02X%02X%02X\n",
            len > 80 ? pkt[80] : 0x00, len > 81 ? pkt[81] : 0x00,
            len > 82 ? pkt[82] : 0x00, len > 83 ? pkt[83] : 0x00,
            len > 84 ? pkt[84] : 0x00, len > 85 ? pkt[85] : 0x00,
            len > 86 ? pkt[86] : 0x00, len > 87 ? pkt[87] : 0x00,
            len > 88 ? pkt[88] : 0x00, len > 89 ? pkt[89] : 0x00,
            len > 90 ? pkt[90] : 0x00, len > 91 ? pkt[91] : 0x00,
            len > 92 ? pkt[92] : 0x00, len > 93 ? pkt[93] : 0x00,
            len > 94 ? pkt[94] : 0x00, len > 95 ? pkt[95] : 0x00);
    printf("%02X%02X%02X%02X %02X%02X%02X%02X %02X%02X%02X%02X %02X%02X%02X%02X\n",
            len > 96  ? pkt[96]  : 0x00, len > 97  ? pkt[97]  : 0x00,
            len > 98  ? pkt[98]  : 0x00, len > 99  ? pkt[99]  : 0x00,
            len > 100 ? pkt[100] : 0x00, len > 101 ? pkt[101] : 0x00,
            len > 102 ? pkt[102] : 0x00, len > 103 ? pkt[103] : 0x00,
            len > 104 ? pkt[104] : 0x00, len > 105 ? pkt[105] : 0x00,
            len > 106 ? pkt[106] : 0x00, len > 107 ? pkt[107] : 0x00,
            len > 108 ? pkt[108] : 0x00, len > 109 ? pkt[109] : 0x00,
            len > 110 ? pkt[110] : 0x00, len > 111 ? pkt[111] : 0x00);
}

} //extern "C"

#define FCS_LEN 4
/* Wrapper for transmit operation */
static void
transmit_wrapper(int asic_id, int port_id, uint8_t *pkt, int len) {
    /* Convert from an asic_id and port_id local to that asic to the
     * transmit port number.
     * For now assume one asic and a direct 1:1 port map. */
    (void)asic_id;

#if 0
    printf("Model Tx Packet %d bytes from chip %d port %d out port %d:\n",
           len, asic_id, port_id, (asic_id * PORT_COUNT) + port_id );

    display_pkt( pkt, len );
#endif
    /* Remove fcs bytes before sending packet out */
    packet_emission_control( PKT_FROM_MODEL, asic_id, port_id, pkt, (len>FCS_LEN)? (len - FCS_LEN):len);
}

#ifdef USE_RUNNER_CLIENT_LOGGING
// Call to client_logging uninit() was at end of rmt_client_uninit
// Call to client_logging_init(chip_cnt, log_dir) was just before rmt_set_log_dir
// Call to rmt_set_log_fn(client_logger) was just after rmt_set_log_dir

#define MAX_LOG_CTX 32
rmt_logging_ctx_t log_ctx[MAX_LOG_CTX];

void
client_logging_init(int chip_cnt, char *log_dir)
{
    // allocate the context for logging
    // id represents thread/chip as implemented in the model
    rmt_logging_ctx_t   *ctx;
    char file_name[512];
    int id;

    assert(chip_cnt < MAX_LOG_CTX);
    for (id=0; id < chip_cnt; id++) {
        if (log_dir) {
            snprintf(file_name, 512, "%s/model_%d.log", log_dir, id);
        } else {
            snprintf(file_name, 512, "model_%d.log", id);
        }
        ctx = &log_ctx[id];
        ctx->log_to_file = true;    // XXX provide CLI
        ctx->log_to_console = true; // XXX provide CLI
        ctx->log_file_name = strdup(file_name);
        if (ctx->log_file_name == NULL) {
            printf("Error memory allocation for log file name: %s\n", file_name);
            exit(1);
	}
        ctx->log_file = fopen(ctx->log_file_name, "w");
        if (pthread_mutex_init(&ctx->log_lock, NULL) != 0) {
            assert(0);
        }
        ctx->init_done = true;
    }
    return;
}

void
client_logging_uninit(void)
{
    rmt_logging_ctx_t   *ctx;
    int                 id;

    for (id=0; id < MAX_LOG_CTX; id++)
    {
        ctx = &log_ctx[id];
        ctx->log_to_file = false;
        ctx->log_to_console = false;

        if (ctx->log_file) {
            fflush(ctx->log_file);
            fclose(ctx->log_file);
            free(ctx->log_file_name);
            ctx->log_file_name = NULL;
            pthread_mutex_destroy(&ctx->log_lock);
            ctx->init_done = false;
        }
    }
}

void
client_logging_reset(void)
{
    rmt_logging_ctx_t   *ctx;
    int                 id;

    for (id=0; id < MAX_LOG_CTX; id++)
    {
        ctx = &log_ctx[id];
        ctx->log_to_file = false;
        ctx->log_to_console = false;
        ctx->log_file = NULL;
        ctx->log_file_name = NULL;
        ctx->init_done = false;
    }
}

int
client_logger (int chip, int pipe, const char *buffer)
{
    rmt_logging_ctx_t     *ctx;
    struct timeval cur_time = {};
    struct tm *loctime;
    char time_str[128]{0};


    if (!buffer) {
        return 0;
    }
    if (chip >= MAX_LOG_CTX) {
        printf("%s\n", buffer);
        return 0;
    }
    ctx = &log_ctx[chip];
    if (!ctx->init_done) {
        printf("%s\n", buffer);
        return 0;
    }
    gettimeofday(&cur_time, NULL);
    loctime = localtime(&cur_time.tv_sec);
    if ( loctime ) {
        strftime(time_str, sizeof(time_str), "%m-%d %T", loctime);
    }

    pthread_mutex_lock(&ctx->log_lock);
    if (ctx->log_to_console) {
        printf(":%s.%.06d:    %s", time_str, (int32_t)cur_time.tv_usec, buffer);
        if (isatty(fileno(stdout)) == 0) fflush(stdout);
    }
    if (ctx->log_file) {
        fprintf(ctx->log_file, ":%s.%.06d:    %s", time_str,
                            (int32_t)cur_time.tv_usec, buffer);
        fflush(ctx->log_file);
    }
    pthread_mutex_unlock(&ctx->log_lock);
    return 0;
}
#endif // USE_RUNNER_CLIENT_LOGGING

extern "C" {

int modelled_port_status[ MAX_RMT_CHIPS ][ PORT_COUNT ] = {{0}};

int port_status_of_dev_port( int asic_id, uint32_t asic_port /*bf_dev_port_t*/ )
{
    return( modelled_port_status[ asic_id ][ asic_port ] );
}
}

static void
packet_handler(uint32_t port_num, uint8_t *pkt, int len, int orig_len)
{
    /* Convert port_num to an asic_id and model port number.
     * For now assume one asic and a direct 1:1 port map. */
    // Convert from one based port numbers to zero based port numbers.
    // Assume all 100g ports so there is a single port per MAC block, therefor
    // multiple the external port number by 4 to convert it to a model port
    // number.
    uint8_t asic_id = ~0;
    uint16_t asic_port = ~0;
    global_port_to_asic_port(port_num, &asic_id, &asic_port);

#if 0
    /* @fixme log vector */
    printf("Packet Rx port %d length %d; chip %d port %d:\n",
           port_num, len, asic_id, asic_port);

    display_pkt( pkt, len );
#endif
    /* MODEL-274
     * Figure out if pkt has alredy been padded
     * (eg if from veth via pkt_rx and size was < 64B)
     * NB. NO optimisation for case where len < orig_len
     * (eg if from emulator via packet_emission_control and looped back)
     */
    if ((len > orig_len) && ((len - orig_len) >= FCS_LEN)) {
      /* Padded by at least 4B - so just zeroise final FCS bytes */
      for (int i = len - FCS_LEN; i < len; i++) pkt[i] = 0;
      rmt_packet_receive(asic_id, asic_port, pkt, len);
    } else {
      /* Otherwise allocate a new packet and add FCS bytes to it */
      int len_w_fcs = ((len > orig_len) ?orig_len :len) + FCS_LEN;
      uint8_t *pkt_w_fcs = (uint8_t*)malloc(len_w_fcs);
      assert(pkt_w_fcs != NULL);
      memcpy(pkt_w_fcs, pkt, len);
      /* Setting FCS to zero */
      for (int i = len; i < len_w_fcs; i++) pkt_w_fcs[i] = 0;
      rmt_packet_receive(asic_id, asic_port, pkt_w_fcs, len_w_fcs);
      /* Model makes a copy of the packet, free the allocated packet here */
      free(pkt_w_fcs);
    }
}

std::mutex pkt_chnl_mutex;

extern "C" {
void
common_packet_handler(uint32_t port_num, uint8_t *pkt, int len, int orig_len)
{
    /* Convert port_num to an asic_id and model port number.
     * For now assume one asic and a direct 1:1 port map. */
    uint8_t asic_id = ~0;
    uint16_t asic_port = ~0;
    global_port_to_asic_port(port_num, &asic_id, &asic_port);

    if (!g_rtl_integ || g_parallel_mode) {
      // pass original packet to original handler
      packet_handler( port_num, pkt, len, orig_len );
    }

    if (g_rtl_integ) {
        std::lock_guard<std::mutex> lock(pkt_chnl_mutex);
        extern void proxy_packet_handler( int asic_id, int asic_port, uint8_t *pkt, int len );
        printf("\nModel Received from port traffic injector%d\n", port_num);
        printf("\nModel Translated to port %d\n", 15 - port_num/4);
        printf("\nFWD PKT to emulator\n");
        proxy_packet_handler( asic_id, (15 - (port_num/4)), pkt, len ); // This again only works in 100G mode.. need to fix this.
                                                                        // along with fix in rtl_port_map() function in this file.
    }
}
}


static void
port_up_handler(uint32_t port_num)
{
    uint8_t asic_id = ~0;
    uint16_t asic_port = ~0;
    global_port_to_asic_port(port_num, &asic_id, &asic_port);

    modelled_port_status[ asic_id ][ asic_port ] = 1; // Up

    rmt_port_up(asic_id, asic_port);
}

static void
port_down_handler(uint32_t port_num)
{
    uint8_t asic_id = ~0;
    uint16_t asic_port = ~0;
    global_port_to_asic_port(port_num, &asic_id, &asic_port);

    modelled_port_status[ asic_id ][ asic_port ] = 0; // Dn

    rmt_port_down(asic_id, asic_port);
}

void *port_status_monitor(void *arg)
{
    int i;
    int sz = bfm_get_port_count();
    int *status = (int*)malloc(sizeof(int) * sz);
    assert(status);
    memset(status, 0, sizeof(int) * sz);

    while (1) {
        for (i=0; i<sz; ++i) {
            bool x = bfm_is_if_up(i);
            // Detect up to down transitions.
            if (!x && status[i]) {
                port_down_handler(i);
            }
            // Detect down to up transitions.
            if (x && !status[i]) {
                // Nothing to do for this right now.
                port_up_handler(i);
            }
            status[i] = x;
        }
        usleep(100000);
    }

    free(status);
    return NULL;
}

#if 0
/********************************************************************
* rtl_tx_pkt_server
*
* Fields packets emitted by the RTL.
********************************************************************/
void *rtl_tx_pkt_service_thread_entry(void *param)
{
    rtl_tx_pkt_service();

    (void)param;

    return NULL;
}
#endif

/********************************************************************
* rtl_int_server
*
* Fields interrupts emitted by the RTL.
********************************************************************/
void *rtl_int_service_thread_entry(void *param)
{
    printf("RTL INT server starting..and ending\n");
    return NULL;
}

/********************************************************************
* rtl_dma_server
*
* Fields DMA operations emitted by the RTL.
********************************************************************/
extern "C" {
void service_proxy_dma_requests(void);
extern void service_chip_dv_reg_requests(void);
extern void init_comms( int dru_mode /*0=cpu, 1=dru*/);
extern void rmt_install_dru_callbacks( DruLearn      dru_learn,
                                       DruDiagEvent  dru_diag_event,
                                       DruIdleUpdate dru_idle_update,
                                       DruLrtUpdate  dru_lrt_update,
                                       DruRxPkt      dru_rx_pkt );
}

void *rtl_dma_service_thread_entry(void *param)
{
    printf("RTL DMA server starting..\n");
    while (1) {
        service_proxy_dma_requests();
        sleep(0);
    }
    return NULL;
}

/********************************************************************
* chip_dv tcl server thread
*
********************************************************************/
void *chip_dv_reg_service_thread_entry(void *param)
{
    printf("Chip DV REG server starting..\n");
    while (1) {
        service_chip_dv_reg_requests();
    }
    return NULL;
}

void *null_parm;
void *null_thread_parm;
pthread_t rtl_tx_pkt_service_thread;
pthread_t rtl_int_service_thread;
pthread_t rtl_dma_service_thread;
pthread_t chip_dv_reg_service_thread;


extern "C" {
void harlyn_lld_re_init(void)
{
    printf("Re-Init link to bf-drivers...\n");

    init_comms( 1 ); // init socket interface for PCIe and DMA
    dru_create_service_thread();

    printf("Re-Init link to bf-drivers...done\n");
    return;
}
}

static bfm_error_t
harlyn_lld_init(void)
{
    if (g_parallel_mode) {
        printf("Simulation target: RTL + Asic model (parallel mode)\n");
    }
    else if (g_rtl_integ) {
        if (g_emu_integ) {
            printf("Simulation target: RTL (Emulator)\n");
        }
        else {
            printf("Simulation target: RTL (Simulator)\n");
        }
    }
    else {
        printf("Simulation target: Asic Model\n");
    }
    if (g_rtl_integ) {
        printf("RTL IP address: %s\n", g_rtl_ip_address );
    }
    printf("Using TCP port range: %d-%d\n",
           g_tcp_port_base, g_tcp_port_base+3 );

    init_comms( 1 ); // init socket interface for PCIe and DMA
    dru_init_tcp(
            g_debug_mode,
            g_emu_integ,
            g_parallel_mode,
            mod_write_dma_chnl,
            mod_read_dma_chnl,
            mod_write_vpi_link,
            NULL,
            mod_push_to_model,
            NULL
            ); // config DRU interface. Start PCIe and DMA simulation threads
    dru_create_service_thread();
    rmt_install_dru_callbacks( dru_learn,
                               dru_diag_event,
                               dru_idle_update,
                               dru_lrt_update,
                               dru_rx_pkt );
    if (g_rtl_integ) {
        /* create the RTL Tx packet proxy thread */
        pthread_create(&rtl_tx_pkt_service_thread, NULL, rtl_tx_pkt_service_thread_entry, &null_parm);
        pthread_create(&rtl_int_service_thread, NULL, rtl_int_service_thread_entry, &null_parm);
        pthread_create(&rtl_dma_service_thread, NULL, rtl_dma_service_thread_entry, &null_parm);
    }
    if (g_include_chip_dv_socket) {
        pthread_create(&chip_dv_reg_service_thread, NULL, chip_dv_reg_service_thread_entry, &null_parm);
    }
    return BFM_E_NONE;
}


static bfm_error_t
harlyn_init(int chip_cnt)
{
    bfm_error_t rv = BFM_E_NONE;

    bfm_packet_handler_vector_set(common_packet_handler);

    bfm_port_init(PORT_COUNT*chip_cnt);

    return rv;
}

// Example code to use model timer
// rmt_timer_increment(units) - advances the model time by specified units
// All active model timers that expire due to this increment will fire
typedef void gTimerCb_t (uint64_t units);

typedef struct gTimer {
  uint64_t    granularity;  // in terms of usec
  gTimerCb_t  *cb;
} gTimer_t;

gTimer_t gModelTimer;

pthread_t rmt_model_timer_thread;
void *
rmt_model_timer_entry (void *arg)
{
    // setup the gTimer
    gModelTimer.granularity = 1000; // min is 1000 uS => 1 ms.
    gModelTimer.cb = &rmt_timer_increment;

    struct timeval cur_time = {};
    struct timeval next_expiry = {};
    struct timeval period = {};

    period.tv_sec = 0;
    period.tv_usec = gModelTimer.granularity;

    gettimeofday(&cur_time, NULL);
    timeradd(&cur_time, &period, &next_expiry);
    while (1) {

      if (timercmp(&next_expiry, &cur_time, >)) {
        struct timeval sleep_time = {};
        timersub(&next_expiry, &cur_time, &sleep_time);
        select(0, 0, 0, 0, &sleep_time);
      }

      gettimeofday(&cur_time, NULL);

      if (gModelTimer.cb) {
          if (time_disable == true) {
              gModelTimer.cb(0);
          } else {
              gModelTimer.cb(gModelTimer.granularity*1000000); // in pico-sec
          }
      }
      timeradd(&next_expiry, &period, &next_expiry);
  }

  return nullptr;
}

void
rmt_timer_thread_start()
{
    pthread_create(&rmt_model_timer_thread, NULL, rmt_model_timer_entry, (void *) &null_thread_parm);
}


extern int harlyn_ucli_create(char *prompt);
extern int harlyn_ucli_thread_spawn(void);

int bfm_port_interface_update(bool add, std::string name, int ofport) {
  int phy_ofport = logical_to_physical(ofport);
  if (phy_ofport == -1) {
    return -2;
  }
  if (add) {
    if (bfm_port_interface_add(name.c_str(), phy_ofport,
                               switch_name, do_pcap_dump) != 0) {
      return -1;
    }
  } else {
    if (bfm_port_interface_remove(name.c_str()) != 0) {
      return -1;
    }
  }
  return 0;
}

/* Add/remove interfaces from the info given in json file */
static int
addRemIntfsUsingJsonFile(char *ofPortInfo_file, bool add, int chip_type, int chip_cnt) {
  try {
    model_core::PortJSONLoader loader(ofPortInfo_file, ofPortInfo_file);
    std::vector<model_core::VethMapping>&
        json_array_veth = loader.getVethMap();
    bool is_chip1 = is_chip1XX(chip_type);
    for (const model_core::VethMapping& item : json_array_veth) {
      const int ofport = item.getDevicePort();
      const int veth1 = item.getVeth1();
      const int veth2 = item.getVeth2();

      if (is_chip1) {
        if ((ofport %2) != 0) {
            printf("Tofino3 only supports even port numbers. "
                    "Port %d is invalid\n", ofport);
            return -1;
        }
        /* Skip ports greater than 512 for Tofino3 single die */
        if ((ofport >= PORT_COUNT) && (chip_cnt == 1)){
            continue;
        }
      }

      std::string veth_name = "veth" + std::to_string(veth1);
      int ret = bfm_port_interface_update(add, veth_name, ofport);
      if (ret == 0) {
        printf("Mapping phyofport %d (logofport %d) to veth%d and veth%d\n",
               logical_to_physical(ofport), ofport, veth1, veth2);
      } else if (ret == -1) {
        printf("bfm_port_interface failed\n");
        return -1;
      } else if (ret == -2) {
        printf("Invalid port %d in PortToVeth mapping\n", ofport);
        return -1;
      } else {
        printf(
            "Error in bfm_port_interface_update - Unexpected return value: %d\n",
            ret);
        return -1;
      }
    }
    //IF JSON Object Parsing:
    std::vector<model_core::IFMapping>& json_array_if = loader.getIFMap();
    for (const model_core::IFMapping& item : json_array_if) {
      const int dev_port = item.getDevicePort();
      const std::string if_name = item.getIFName();

      int ret = bfm_port_interface_update(add, if_name, dev_port);
      if (ret == 0) {
        printf("Mapping phy_dev_port %d (log_dev_port %d) to %s\n",
               logical_to_physical(dev_port), dev_port, if_name.c_str());
      } else if (ret == -1) {
        printf("bfm_port_interface failed\n");
        return -1;
      } else if (ret == -2) {
        printf("Invalid port %d in PortToIf mapping\n", dev_port);
        return -1;
      } else {
        printf(
            "Error in bfm_port_interface_update - Unexpected return value: %d\n",
            ret);
        return -1;
      }
    }
  } catch (std::invalid_argument& e) {
    printf("%s\n", e.what());
    return -1;
  }
  return 0;
}

void
rmt_client_uninit()
{
    // stop all the threads
    if (g_rtl_integ) {
        pthread_cancel(rtl_tx_pkt_service_thread);
        pthread_join(rtl_tx_pkt_service_thread, NULL);
    }

    pthread_cancel(rmt_model_timer_thread);
    pthread_join(rmt_model_timer_thread, NULL);

    if (port_status_monitor_thread_started) {
        pthread_cancel(port_status_monitor_thread);
        pthread_join(port_status_monitor_thread, NULL);
    }

    rmt_stop_packet_processing(); // move to rmt_uninit() ?
}

void
get_username_password(const char* filename, std::string &username, std::string &password)
{
  if ( 0 == strcmp(filename,"-") ) {
    printf("Reading cli username:password from stdin\n");
    if (!( std::getline(std::cin,username,':') &&
           std::getline(std::cin,password,'\n') ) ) {
      printf("Could not read cli username:password from stdin\n");
      exit(1);
    }
  }
  else {
    std::ifstream ifs(filename);
    if ( ifs ) {
      if (!( std::getline(ifs,username,':') &&
             std::getline(ifs,password,'\n') ) ) {
        printf("Could not find cli username:password in %s\n",filename);
        exit(1);
      }
    }
    else {
      printf("Could not open %s to read username:password\n",filename);
      exit(1);
    }
  }
}
SaveTracePairs extract_trace_args(char const* optarg) {
    SaveTracePairs save_trace;
    char const* start = optarg;
    while (*start) {
        char* end;
        auto chip = static_cast<unsigned>(strtol(start, &end, 10));
        if (start == end) break;
        start = end;
        if (*start != ':') break;
        ++start;
        auto pipe = static_cast<unsigned>(strtol(start, &end, 10));
        if (start == end) break;
        save_trace.emplace_back(chip, pipe);
        start = end;
        if (*start != ',') break;
        ++start;
    }
    if (*start != 0) {
        fprintf(stderr, "Bad argument `--save-trace %s'. Discarding argument.\n", optarg);
        return SaveTracePairs{};
    }
    return save_trace;
}


int
main(int argc, char* argv[])
{
    int opret1 = os_privs_init(); // Remove any privs currently in effect
    int opret2 = os_privs_for_veth_attach(); // Setup CAP_NET_RAW
    (void)os_privs_for_file_access(); // Setup CAP_DAC_* but allow fail
    if ((opret1 != 0) || (opret2 != 0)) {
      fprintf(stderr, "%s: Unable to drop privileges to purely CAP_NET_RAW\n", argv[0]);
      exit(1);
    }

    int rv = 0, n_veth;
    std::list<std::string> interfaces;
    bool no_cli = false;
    bool no_veth = false;
    bool multi_program = false;
    bool start_port_monitor = false;
    bool logs_disable = false;
    bool json_logs_enable = false;
    int chip_cnt = 1;
    int max_pipes = 4;
    char ofPortInfo_file[250]{0};
    bool ofPortInfoFileExists = false;
    const char *p4_name_lookup = NULL;
    const char *p4_target_conf_file = NULL;
    const char *install_dir = NULL;
    std::string cli_username{};
    std::string cli_password{};
    int chip_id = 0;
    int chip_type = model_core::ChipType::kDefault;
    bool  disable_meter_sweep = false;
    char *log_dir = NULL;
    int pkt_log_len = -1;
    bool use_pcie_veth = false;
    bool dod_test_mode = false;
    bool thread_per_pipe = false;
    bool capture_inputs = false;
    SaveTracePairs save_trace{};
    bool continue_on_config_errors = false;

    /*************************************************************
     * Parse command line options
     ************************************************************/
    while (1) {
        int option_index = 0;
        /* Options without short equivalents */
        enum long_opts {
            OPT_START = 256,
            OPT_NOCLI,
            OPT_NOVETH,
            OPT_PORTMON,
            OPT_TIME_DISABLE,
            OPT_LOGS_DISABLE,
            OPT_JSON_LOGS_ENABLE,
            OPT_VERSION,
            OPT_INTERNAL_VERSION,
            OPT_SKU,
            OPT_CLIPORT,
            OPT_CLI_USER_PASS_FILE,
            OPT_CHIP_TYPE,
            OPT_INT_PORT_LOOP,
            OPT_LOG_DIR,
            OPT_PKT_LOG_LEN,
            OPT_PCIE_VETH,
            OPT_P4_TARGET_CONFIG,
            OPT_INSTALL_DIR,
            OPT_DOD_TEST_MODE,
            OPT_THREAD_PER_PIPE,
            OPT_CAPTURE_INPUTS,
            OPT_CONTINUE_ON_CONFIG_ERRORS,
            OPT_GENERATE_PCAP,
        };
        static struct option long_options[] = {
            {"interface", required_argument, 0, 'i' },
            {"num-of-chips", required_argument, 0, 'd' },
            {"pkg-size", required_argument, 0, 'k' },
            {"of-port-info", required_argument, 0, 'f' },
            {"help", no_argument, 0, 'h' },
            {"no-cli", no_argument, 0, OPT_NOCLI },
            {"no-veth", no_argument, 0, OPT_NOVETH },
            {"tcp-port-base", required_argument, 0, 't' },
            {"cli-port", required_argument, 0, OPT_CLIPORT },
            {"cli-credentials", required_argument, 0, OPT_CLI_USER_PASS_FILE },
            {"parallel-mode", required_argument, 0, 'p' },
            {"rtl-ip-addr", required_argument, 0, 'r' },
            {"emu-ip-addr", required_argument, 0, 'e' },
            {"chip-dv", required_argument, 0, 'c' },
            {"unimplemented-regs", required_argument, 0, 'u' },
            {"port-monitor", no_argument, 0, OPT_PORTMON },
            {"p4-log-helper", required_argument, 0, 'l' },          // TO BE DEPRECATED (use --p4-target-config instead)
            {"p4-target-config", required_argument, 0, OPT_P4_TARGET_CONFIG},
            {"install-dir", required_argument, 0, OPT_INSTALL_DIR },
            {"sku", required_argument, 0, OPT_SKU },
            {"time-disable", no_argument, 0, OPT_TIME_DISABLE},
            {"logs-disable", no_argument, 0, OPT_LOGS_DISABLE},
            {"json-logs-enable", no_argument, 0, OPT_JSON_LOGS_ENABLE},
            {"version", no_argument, 0, OPT_VERSION},
            {"iversion", no_argument, 0, OPT_INTERNAL_VERSION},
            {"chip-type", required_argument, 0, OPT_CHIP_TYPE },
            {"int-port-loop", required_argument, 0, OPT_INT_PORT_LOOP },
            {"log-dir", required_argument, 0, OPT_LOG_DIR },
            {"pkt-log-len", required_argument, 0, OPT_PKT_LOG_LEN },
            {"use-pcie-veth", required_argument, 0, OPT_PCIE_VETH },
            {"dod-test-mode", no_argument, 0, OPT_DOD_TEST_MODE },
            {"thread-per-pipe", no_argument, 0, OPT_THREAD_PER_PIPE },
            {"capture-inputs", no_argument, 0, OPT_CAPTURE_INPUTS },
            {"save-trace", required_argument, 0, 's' },
            {"continue-on-config-errors", no_argument, 0, OPT_CONTINUE_ON_CONFIG_ERRORS },
            {"generate-pcap", no_argument, 0, OPT_GENERATE_PCAP},
            {0, 0, 0, 0 }
        };

        int c = getopt_long(argc, argv, "c:u:i:f:d:k:e:h:p:r:t:ml:s:",
                            long_options, &option_index);
        if (c == -1) {
            break;
        }

        bool int_port_loop_requested = false;
        switch (c) {
            case 'u':
                g_return_0bad = 0; // Assert on read/write unimpl reg
                break;
            case 'c':
                g_include_chip_dv_socket = 1;
                break;
            case 'i':
                interfaces.push_back(std::string(optarg));
                no_veth = true;
                break;
            case 'l':
                p4_name_lookup = optarg;
                break;
            case OPT_P4_TARGET_CONFIG:
                p4_target_conf_file = optarg;
                break;
            case OPT_INSTALL_DIR:
                install_dir = optarg;
                break;
            case 'd':
                chip_cnt = atoi(optarg);
                break;
            case 'k':
                g_pkg_size = atoi(optarg);
                break;
            case 'p': // both model and RTL active
                g_rtl_ip_address = optarg;
                g_rtl_integ = 1;
                g_parallel_mode = 1;
                break;
            case 'r': // only RTL active
                g_rtl_ip_address = optarg;
                g_rtl_integ = 1;
                g_parallel_mode = 0;
                break;
            case 'e': // only RTL active
                g_rtl_ip_address = optarg;
                g_rtl_integ = 1;
                g_emu_integ = 1;
                g_parallel_mode = 0;
                break;
            case 't':
                g_tcp_port_base = atoi(optarg);
                break;
            case 'f':
                ofPortInfoFileExists = true;
                strncpy(ofPortInfo_file, optarg, sizeof(ofPortInfo_file)-1 );
                break;
            case OPT_SKU:
                if (strncmp(optarg, "77110", 5) == 0) {
                    g_sku = 0;
                    g_pipe_mode = 0;
                } else if (strncmp(optarg, "77120", 5) == 0) {
                    g_sku = 1;
                    g_pipe_mode = 0;
                    if (!int_port_loop_requested) {
                      /* If no explicit loopback was requested default to pipes
                       * 1 and 3 which are the internal pipes. */
                      internal_port_loopback = 0xA;
                    }
                } else if (strncmp(optarg, "77121_even_02", 13) == 0) {
                    g_sku = 2;
                    g_pipe_mode = 1;
                    max_pipes = 2;
                } else if (strncmp(optarg, "77121_even_03", 13) == 0) {
                    g_sku = 3;
                    g_pipe_mode = 2;
                    max_pipes = 2;
                } else if (strncmp(optarg, "77121_even_12", 13) == 0) {
                    g_sku = 4;
                    g_pipe_mode = 3;
                    max_pipes = 2;
                } else if (strncmp(optarg, "77121_even_13", 13) == 0) {
                    g_sku = 5;
                    g_pipe_mode = 4;
                    max_pipes = 2;
                } else if (strncmp(optarg, "77121_odd_02", 12) == 0) {
                    g_sku = 6;
                    g_pipe_mode = 1;
                    max_pipes = 2;
                } else if (strncmp(optarg, "77121_odd_03", 12) == 0) {
                    g_sku = 7;
                    g_pipe_mode = 2;
                    max_pipes = 2;
                } else if (strncmp(optarg, "77121_odd_12", 12) == 0) {
                    g_sku = 8;
                    g_pipe_mode = 3;
                    max_pipes = 2;
                } else if (strncmp(optarg, "77121_odd_13", 12) == 0) {
                    g_sku = 9;
                    g_pipe_mode = 4;
                    max_pipes = 2;
                } else if (strncmp(optarg, "77131_even_02", 13) == 0) {
                    g_sku = 11;
                    g_pipe_mode = 1;
                    max_pipes = 2;
                } else if (strncmp(optarg, "77131_even_03", 13) == 0) {
                    g_sku = 12;
                    g_pipe_mode = 2;
                    max_pipes = 2;
                } else if (strncmp(optarg, "77131_even_12", 13) == 0) {
                    g_sku = 13;
                    g_pipe_mode = 3;
                    max_pipes = 2;
                } else if (strncmp(optarg, "77131_even_13", 13) == 0) {
                    g_sku = 14;
                    g_pipe_mode = 4;
                    max_pipes = 2;
                } else if (strncmp(optarg, "77131_odd_02", 12) == 0) {
                    g_sku = 15;
                    g_pipe_mode = 1;
                    max_pipes = 2;
                } else if (strncmp(optarg, "77131_odd_03", 12) == 0) {
                    g_sku = 16;
                    g_pipe_mode = 2;
                    max_pipes = 2;
                } else if (strncmp(optarg, "77131_odd_12", 12) == 0) {
                    g_sku = 17;
                    g_pipe_mode = 3;
                    max_pipes = 2;
                } else if (strncmp(optarg, "77131_odd_13", 12) == 0) {
                    g_sku = 18;
                    g_pipe_mode = 4;
                    max_pipes = 2;
                } else if (strncmp(optarg, "77140_even_02", 13) == 0) {
                    g_sku = 19;
                    g_pipe_mode = 1;
                    max_pipes = 2;
                } else if (strncmp(optarg, "77140_even_03", 13) == 0) {
                    g_sku = 20;
                    g_pipe_mode = 2;
                    max_pipes = 2;
                } else if (strncmp(optarg, "77140_even_12", 13) == 0) {
                    g_sku = 21;
                    g_pipe_mode = 3;
                    max_pipes = 2;
                } else if (strncmp(optarg, "77140_even_13", 13) == 0) {
                    g_sku = 22;
                    g_pipe_mode = 4;
                    max_pipes = 2;
                } else if (strncmp(optarg, "77140_odd_02", 12) == 0) {
                    g_sku = 23;
                    g_pipe_mode = 1;
                    max_pipes = 2;
                } else if (strncmp(optarg, "77140_odd_03", 12) == 0) {
                    g_sku = 24;
                    g_pipe_mode = 2;
                    max_pipes = 2;
                } else if (strncmp(optarg, "77140_odd_12", 12) == 0) {
                    g_sku = 25;
                    g_pipe_mode = 3;
                    max_pipes = 2;
                } else if (strncmp(optarg, "77140_odd_13", 12) == 0) {
                    g_sku = 26;
                    g_pipe_mode = 4;
                    max_pipes = 2;
                } else if (strncmp(optarg, "32d12_even_02", 13) == 0) {
                    g_sku = 27;
                    g_pipe_mode = 1;
                    max_pipes = 2;
                    if (!int_port_loop_requested) {
                      /* If no explicit loopback was requested default to
                       * logical pipe 1 which is the internal pipe. */
                      internal_port_loopback = 0x2;
                    }
                } else if (strncmp(optarg, "32d12_even_03", 13) == 0) {
                    g_sku = 28;
                    g_pipe_mode = 2;
                    max_pipes = 2;
                    if (!int_port_loop_requested) {
                      /* If no explicit loopback was requested default to
                       * logical pipe 1 which is the internal pipe. */
                      internal_port_loopback = 0x2;
                    }
                } else if (strncmp(optarg, "32d12_even_12", 13) == 0) {
                    g_sku = 29;
                    g_pipe_mode = 3;
                    max_pipes = 2;
                    if (!int_port_loop_requested) {
                      /* If no explicit loopback was requested default to
                       * logical pipe 1 which is the internal pipe. */
                      internal_port_loopback = 0x2;
                    }
                } else if (strncmp(optarg, "32d12_even_13", 13) == 0) {
                    g_sku = 30;
                    g_pipe_mode = 4;
                    max_pipes = 2;
                    if (!int_port_loop_requested) {
                      /* If no explicit loopback was requested default to
                       * logical pipe 1 which is the internal pipe. */
                      internal_port_loopback = 0x2;
                    }
                } else if (strncmp(optarg, "32d12_odd_02", 12) == 0) {
                    g_sku = 31;
                    g_pipe_mode = 1;
                    max_pipes = 2;
                    if (!int_port_loop_requested) {
                      /* If no explicit loopback was requested default to
                       * logical pipe 1 which is the internal pipe. */
                      internal_port_loopback = 0x2;
                    }
                } else if (strncmp(optarg, "32d12_odd_03", 12) == 0) {
                    g_sku = 32;
                    g_pipe_mode = 2;
                    max_pipes = 2;
                    if (!int_port_loop_requested) {
                      /* If no explicit loopback was requested default to
                       * logical pipe 1 which is the internal pipe. */
                      internal_port_loopback = 0x2;
                    }
                } else if (strncmp(optarg, "32d12_odd_12", 12) == 0) {
                    g_sku = 33;
                    g_pipe_mode = 3;
                    max_pipes = 2;
                    if (!int_port_loop_requested) {
                      /* If no explicit loopback was requested default to
                       * logical pipe 1 which is the internal pipe. */
                      internal_port_loopback = 0x2;
                    }
                } else if (strncmp(optarg, "32d12_odd_13", 12) == 0 ||
                           strncmp(optarg, "32d12", 5) == 0) {
                    g_sku = 34;
                    g_pipe_mode = 4;
                    max_pipes = 2;
                    if (!int_port_loop_requested) {
                      /* If no explicit loopback was requested default to
                       * logical pipe 1 which is the internal pipe. */
                      internal_port_loopback = 0x2;
                    }
                } else if (strncmp(optarg, "t2_0128q", 8) == 0) {
                    g_sku = 200;
                    g_pipe_mode = 0;
                }  else if (strncmp(optarg, "t2_0064q", 8) == 0) {
                    g_sku = 201;
                    g_pipe_mode = 0;
                    if (!int_port_loop_requested) {
                      /* If no explicit loopback was requested default to pipes
                       * 2 and 3 which are the internal pipes. */
                      internal_port_loopback = 0xC;
                    }
                }  else if (strncmp(optarg, "t2_0080tr", 9) == 0) {
                    g_sku = 203;
                    g_pipe_mode = 7;
                    max_pipes = 3;
                }  else if (strncmp(optarg, "t2_0080t", 8) == 0) {
                    g_sku = 202;
                    g_pipe_mode = 6;
                    max_pipes = 3;
                }  else if (strncmp(optarg, "t2_0064dr", 9) == 0) {
                    g_sku = 205;
                    g_pipe_mode = 8;
                    max_pipes = 2;
                }  else if (strncmp(optarg, "t2_0064d", 8) == 0) {
                    g_sku = 204;
                    g_pipe_mode = 5;
                    max_pipes = 2;
                }
                break;
            case OPT_NOCLI:
                no_cli = true;
                break;
            case OPT_CLIPORT:
                cli_port = atoi(optarg);
                break;
            case OPT_CLI_USER_PASS_FILE :
                get_username_password( optarg, cli_username, cli_password );
                break;
            case OPT_NOVETH:
                no_veth = true;
                break;
            case 'm':
                multi_program = true;
                break;
            case OPT_PORTMON:
                start_port_monitor = true;
                break;
            case OPT_TIME_DISABLE:
                time_disable = true;
                break;
            case OPT_LOGS_DISABLE:
                logs_disable = true;
                break;
            case OPT_JSON_LOGS_ENABLE:
                json_logs_enable = true;
                break;
            case OPT_VERSION:
                print_model_version();
                exit(0);
                break;
            case OPT_INTERNAL_VERSION:
                print_model_int_version();
                exit(0);
                break;
            case OPT_CHIP_TYPE:
                chip_type = atoi(optarg);
                if (!is_chip_type_available(chip_type)) {
                  printf("chip_type %d unavailable (Available types:%s)\n",
                         chip_type, chip_types_available_str());
                  exit(1);
                }
                break;
            case OPT_INT_PORT_LOOP:
                internal_port_loopback = strtoul(optarg, NULL, 0);
                int_port_loop_requested = true;
                break;
            case OPT_LOG_DIR:
                log_dir = optarg;
                break;
            case OPT_PKT_LOG_LEN:
                pkt_log_len = atoi(optarg);
                break;
            case OPT_PCIE_VETH:
                use_pcie_veth = true;
                break;
            case OPT_DOD_TEST_MODE:
                dod_test_mode = true;
                break;
            case OPT_THREAD_PER_PIPE:
                thread_per_pipe = true;
                break;
            case OPT_CAPTURE_INPUTS:
                capture_inputs = true;
                break;
            case 's':
                // Overwrite any previous settings.
                save_trace = extract_trace_args(optarg);
                break;
            case OPT_CONTINUE_ON_CONFIG_ERRORS:
                continue_on_config_errors = true;
                break;
            case OPT_GENERATE_PCAP:
                do_pcap_dump = 1;
                break;
            case 'h':
            case '?':
                printf("Usage: %s [OPTION]\n", argv[0]);
                printf("\n");
                printf(" --no-cli Do not start the CLI thread\n");
                printf(" --cli-port <TCP port for model shell>\n");
                printf(" --cli-credentials <file (or - for stdin) containing username:password >\n");
                printf(" --log-dir <Specify log file directory>\n");
                printf(" --json-logs-enable Enable JSON log stream\n");
                printf(" --pkt-log-len <Specify packet log length in bytes>\n");
                printf(" --int-port-loop <Specify pipe bitmap>\n");
#if 0
                printf(" --no-veth No veth interfaces\n");
#endif
                printf(" --version Print the version of the model\n");
#if 0
                printf(" -i <intf>, --interface=<intf>");
                printf(" Attach a network interface at startup\n");
#endif
                printf(" -d <no-of-devices>\n");
                printf(" -k <no-of-chips-per-device (package-size)>\n");
                printf(" --chip-type=<type_index>  (Available types:%s)\n",
                       chip_types_available_str());
                printf(" -f <port to veth i/f mapping file>\n");
                printf(" -t <TCP port base for dru_sim>\n");
                printf(" -l <HW object to P4 object mapping file> DEPRECATED, use --p4-target-config instead\n");
                printf(" --p4-target-config <P4 target conf file>\n");
                printf(" --install-dir <P4 programs install directory>\n");
                printf(" -s --save-trace <chip>:<pipe>[,<chip>:<pipe>]  Save the traces (in the --log-dir) for post processing\n");
                printf(" --continue-on-config-errors  Continue best effort with executing under invalid input data - undefined behaviour\n");
                printf(" --logs-disable  Turns off all logging for every chip\n");
                printf(" --time-disable  Stop the model incrementing time, so rmt_time_increment() can be used\n");
                printf(" --thread-per-pipe  Enables multi-threading of pipes\n");
                printf(" --generate-pcap  Enables pcap files to be generated to pcap_output directory\n");
                printf(" -h,--help Display this help message and exit\n");
                exit(c == 'h' ? 0 : 1);
                break;
        }
    }

    /*************************************************************
     * Check for root access as uses veth interfaces
     * (also allow if process had CAP_NET_RAW privilege)
     * @fixme allow specifying vpi names as runtime config
     ************************************************************/
    if ((geteuid() != 0) && (os_privs_has_cap_net_raw() == 0)) {
      fprintf(stderr, "%s: This program uses veth interfaces so requires CAP_NET_RAW privilege\n", argv[0]);
      fprintf(stderr, "  (use sudo setcap cap_net_raw+p <path tofino-model binary>)\n");
      exit(1);
    }

    if (logs_disable) {
        printf("Logging disabled for all devices\n");
    } else {
      // Do an early check on the validity of config file (check for existence,
      // ability to open and validity of JSON syntax, but not schema). This is
      // done here so that the user gets early feedback if there are any
      // problems. However, the p4 name lookup files specified by the config
      // file cannot be installed in the chips until they have been
      // initialised, so those files are loaded and checked later after chips
      // have been initialised by the driver.
      printf("Performing preliminary checks on config file\n");
      if (nullptr != p4_target_conf_file) {
        P4TargetConf p4_target_conf(p4_target_conf_file);
        if (!p4_target_conf.IsLoaded()) {
          printf("ERROR: Invalid P4 target config file: %s\n", p4_target_conf_file);
          // XXX: exit if JSON file was specified but failed to load
          exit(1);
        }
      }
    }

    // Only supports max 2 Die Package for WIP. -d <<NUM>> gives the number of
    // WIP Packages that needs to be created.
    // if -k <<PKG_SIZE>> is not specified, WIP will use 1 as default.
    // if specified, PKG_SIZE for WIP can be 1 or 2 only.
    // For TOFINO, JBAY and FLATROCK g_pkg_size is always 1
    // Total Chip_cnt will be PKG_SIZE * <<NUM>>

    if (is_dual_die(chip_type)) {
      if (g_pkg_size <= 0 || g_pkg_size > 2) {
        printf("%s: Invalid -k << %d >> package size param. Valid values [1-2] \n",
               chip_type_name(chip_type), g_pkg_size);
        exit(1);
      }
    } else if (g_pkg_size <= 0 || g_pkg_size > 1) {
      printf("%s: Invalid -k << %d >> package size param. Valid value 1 \n",
             chip_type_name(chip_type), g_pkg_size);
      exit(1);
    }
    printf(" Package size %d\n", g_pkg_size);

    chip_cnt *= g_pkg_size;
    if (chip_cnt > MAX_RMT_CHIPS) {
      int cap_chip_cnt = (MAX_RMT_CHIPS / g_pkg_size) * g_pkg_size;;

      printf("%s: Chip Count %d exceeded. Max Chips supported %d. "
             "Capped Chip Count is %d\n",
             chip_type_name(chip_type), chip_cnt, MAX_RMT_CHIPS, cap_chip_cnt);
      chip_cnt = cap_chip_cnt;
    }

    printf(" No of Chips is %d\n", chip_cnt);
    if (ofPortInfoFileExists) {
        if (strstr(ofPortInfo_file, "None")) {
            ofPortInfoFileExists = false;
            ofPortInfo_file[0] = '\0';
        } else {
            printf(" Of-Port info file is %s\n", ofPortInfo_file);
        }
    }

    std::string json_log_file_name;
    if (log_dir) {
      // TODO timestamp file
      json_log_file_name = std::string(log_dir) + std::string("/model.ldjson");
    } else {
      json_log_file_name = "model.ldjson";
    }

    if (chip_type == model_core::ChipType::kTofinoB0) {
        g_chip_part_rev_no = 1;
        printf("Chip part revision number is %d\n", g_chip_part_rev_no);
    }

    /* Do a max of 16 ports per die on Tofino3 */
    if (is_chip1XX(chip_type)) {
        g_port_count = 16;
    }

    /*************************************************************
     * Initialize Modules.
     ************************************************************/
    if (!g_rtl_integ && // NOT RTL SIM MODE
        !g_emu_integ && // NOT EMULATOR MODE or either of those modes
        !g_parallel_mode) {
      // Disable Meter Sweeps.
      disable_meter_sweep = true;
    }

    if (json_logs_enable) {
      rmt_init(chip_cnt, json_log_file_name.c_str());
    } else {
      rmt_init(chip_cnt);
    }
    // This initializes logging for C code in the model.
    rmt_set_log_dir(log_dir);
    rmt_set_log_fn(nullptr);

    rmt_set_save_trace(save_trace);
    rmt_set_continue_on_config_errors(continue_on_config_errors);

    printf(" No of Packages is %d\n", chip_cnt / g_pkg_size);
    for (chip_id = 0; chip_id < chip_cnt; chip_id += g_pkg_size) {
      bool ok = rmt_create_package(chip_id, static_cast<uint8_t>(chip_type), g_pkg_size);
      if (!ok) {
        fprintf(stderr, "%s: Package create failed for package num %d\n", argv[0],
                chip_id / g_pkg_size);
        exit(1);
      }
    }

    // Configure chips to only use enabled pipes/stages
    // ***  TYPE/CONFIG SHOULD BE CONFIGURABLE PER-CHIP        ***
    // ***  (for the moment always use default chip)           ***
    // ***  (this is whatever chip was enabled on ./configure) ***
    for (chip_id=0; chip_id<chip_cnt; chip_id++) {
      /* Now that the chip is created we can determine the type.  The type
       * passed to rmt_create_chip above could be none/default so we need to
       * query the type after creating it. */
      uint8_t actual_type = static_cast<uint8_t>(chip_type);
      rmt_get_type(chip_id, &actual_type);
      chip_type = actual_type;

      /* Query the chip to determine the default stage count, pipe enable
       * mask and sku. */
      uint32_t pipes_en = 0u;
      uint8_t num_stages = 0;
      int sku = 0;
      bool query_ok = rmt_query(chip_id, &pipes_en, &num_stages, &sku, NULL);
      assert(query_ok);

      // If no sku specified default it based on value returned above
      if (g_sku == -1) {
        g_sku = sku;
        g_pipe_mode = 0; /* default to pipe-full mode */
        printf("Chip %d SKU: defaulting to %s\n", chip_id,
               (is_jbayXX(chip_type)) ?"t2_0128q" :
               ((is_chip1XX(chip_type))? "t3_12256q": "BFN77110"));
      }

      if (g_sku == 10) {
        g_pipe_mode = 0; /* default to pipe-full mode */
      }
      printf("Chip %d SKU setting: sku %d pipe mode %d\n",
          chip_id, g_sku, g_pipe_mode);
      setup_log2phy_pipe_map();
      int chip_phy_pipes_en_bmp = (g_phy_pipes_en_bmp>>(4*chip_id))&0xf;
      printf("Chip %d Physical pipes enabled bitmap 0x%x \n",
          chip_id, chip_phy_pipes_en_bmp);

      // Get per-chip default values for pipe bitmap, num stages
      // if these are setup with zero/negative values
      if (chip_phy_pipes_en_bmp <= 0) chip_phy_pipes_en_bmp = pipes_en;
      if (g_num_stages <= 0) g_num_stages = num_stages;

      uint32_t flags = 0u;
      if (g_return_0bad)   flags |= RMT_FLAGS_0BAD_MODE;
      if (dod_test_mode)   flags |= RMT_FLAGS_DOD_TEST_MODE;
      if (thread_per_pipe) flags |= RMT_FLAGS_THREAD_PER_PIPE;
      if (chip_cnt > 1)    flags |= RMT_FLAGS_MULTI_CHIP;
      if (capture_inputs)  flags |= RMT_FLAGS_CAPTURE_INPUTS;

      if (!rmt_config(chip_id, chip_phy_pipes_en_bmp, g_num_stages, g_sku, flags)) {
        printf("Config chip pipes=0x%x,nStages=%d failed. "
               "Using default vals (pipes=0x%x,nStages=%d)\n",
               chip_phy_pipes_en_bmp, g_num_stages, pipes_en, num_stages);
      }
    }
    rmt_meter_sweep_init(disable_meter_sweep);
    if (pkt_log_len != -1) {
        printf("Setting Packet log length to %d\n", pkt_log_len);
        rmt_set_pkt_log_len(pkt_log_len);
    }
    CHECK(harlyn_init(chip_cnt));
    rmt_timer_thread_start();
    rmt_0bad_mode_set( g_return_0bad );

    if (internal_port_loopback != 0) {
        printf("Internal port loopback for pipes: 0x%x\n", internal_port_loopback);
    }

    if (do_pcap_dump) {
        /* change where pcap files are stored */
        bfm_set_pcap_outdir("pcap_output");
    }

    /*************************************************************
     * Add ports;
     ************************************************************/
    if (use_pcie_veth) {
        printf("Setting Pcie to use veth\n");
        rmt_pcie_veth_init(use_pcie_veth);
    }

    if (no_veth) {
        g_port_count = 0;
        int base_port = 0;
        bool is_chip1 = false;
        if (is_jbayXX(chip_type)) {
          base_port = 8;
        } else if (is_chip1XX(chip_type)) {
          base_port = 8;
          is_chip1 = true;
        } else if (is_chip2XX(chip_type)) {
          base_port = 8;
        }
        for (auto element : interfaces) {
            printf("Adding interface %s (port %d)\n", element.c_str(), base_port + g_port_count);
            CHECK(bfm_port_interface_add(element.c_str(), base_port + g_port_count,
                                         switch_name, do_pcap_dump));
            if (is_chip1) {
                g_port_count += 2;
            } else {
                g_port_count++;
            }
        }
    } else {
        if (ofPortInfoFileExists) {
            int res = addRemIntfsUsingJsonFile(ofPortInfo_file, true, chip_type, chip_cnt);
            if (res != 0) {
                return res;
            }
        } else {
            int base_chip_veth = 0;
            for (chip_id =0; chip_id<chip_cnt; chip_id++) {
                int base_port = 0;
                bool is_chip1 = false;
                int port_num = 0;
                if (is_jbayXX(chip_type)) {
                  base_port = 8;
                } else if (is_chip1XX(chip_type)) {
                  base_port = 8;
                  is_chip1 = true;
                } else if (is_chip2XX(chip_type)) {
                  base_port = 8;
                }
                base_chip_veth = chip_id*PORT_COUNT;
                for (n_veth = 0; n_veth < g_port_count; n_veth++) {
                    port_num = is_chip1? (2*n_veth): n_veth;
                    int log_port = base_chip_veth + base_port + port_num;
                    std::stringstream veth_name;
                    veth_name << "veth" << (n_veth + g_port_count*chip_id)*2;
                    int phy_port = logical_to_physical(log_port);
                    if (phy_port == -1) {
                        continue;
                    }
                    CHECK(bfm_port_interface_add(veth_name.str().c_str(), phy_port,
                                                 switch_name, do_pcap_dump));
                }
            }

            // Add the CPU port for the first chip based on chip type
            int cpu_port;
            if (is_jbayXX(chip_type)) {
                cpu_port = 2;
            } else if (is_chip1XX(chip_type)) {
                cpu_port = 2;
            } else if (is_chip1XX(chip_type)) {
                cpu_port = 2;
            } else if (is_chip2XX(chip_type)) {
                cpu_port = 2;
            } else {
                cpu_port = 64;
            }
            int phy_port = logical_to_physical(cpu_port);
            if (phy_port != -1) {
                CHECK(bfm_port_interface_add("veth250", phy_port,
                                              switch_name, do_pcap_dump));
            }
        }
    }
    CHECK(bfm_port_start_pkt_processing());

    if (start_port_monitor) {
        port_status_monitor_thread_started = true;
        void *not_used = NULL;
        pthread_create(&port_status_monitor_thread, NULL, port_status_monitor, &not_used);
    }

    /*************************************************************
     * Enable All Modules.
     ************************************************************/
    Cli cli;
    if (no_cli) {
      signal (SIGTERM, term_handler);
    } else {
      cli.StartCli(cli_port,cli_username,cli_password);
      // could scrub the username and password here, see MODEL 721
    }

    harlyn_lld_init();

    rmt_set_model_log_fn(nullptr);
    rmt_transmit_register(transmit_wrapper);

    /*************************************************************
     * Setup logging.
     ************************************************************/
    if (logs_disable) {
        printf("Logging disabled for all devices\n");
        for (int chip = 0; chip < chip_cnt; chip++) {
            // Disabling all the 4 log types found in rmt-log.h
            // Note: model logging applies some fixed flags despite this
            for (int log_type=0; log_type < MAX_RMT_LOG_TYPE; log_type++) {
              rmt_update_log_type_levels(
                  chip,
                  model_core::RmtDebug::ALL,    // pipes
                  model_core::RmtDebug::ALL,    // stages
                  log_type,
                  model_core::RmtDebug::ALL,    // clear
                  model_core::RmtDebug::NONE);  // set
            }
        }
    } else {
        /* The p4_target_conf file is used by the driver to program the chips.
         * We use it here to load the correct context file for p4-name-lookup
         * in each chip.
         */
        P4TargetConf p4_target_conf(p4_target_conf_file);
        if (!p4_target_conf.IsLoaded()) {
            printf("WARNING: P4 target config file has not been loaded\n");
            if (nullptr == p4_name_lookup) {
                printf("WARNING: P4 name_lookup file has not been specified "
                       "by --p4-log-helper or -l option\n");
                printf("WARNING: Unable to include P4 names in log messages\n");
            } else {
                printf("WARNING: Using P4 name-lookup file specified by deprecated "
                       "--p4-log-helper or -l option: '%s'\n", p4_name_lookup);
            }
        }
        for (int chip = 0; chip < chip_cnt; chip += g_pkg_size) {
            for (int pipe =0 ; pipe < max_pipes * g_pkg_size; pipe++) {
                std::string context_path;
                // the p4_target_conf_file condition is superfluous but keeps
                // klocwork happy
                if ((p4_target_conf_file != nullptr) && p4_target_conf.IsLoaded()) {
                    /* If pkg_size is more than 1, then there is more than 1 subdevice.
                       In this case, get the real chip-id.
                    */
                    context_path = p4_target_conf.GetContext(chip/g_pkg_size, pipe);
                    if (!context_path.empty()) {
                        printf("Device %d: Pipe %d: loading P4 name lookup file %s found in %s\n",
                               chip, pipe, context_path.c_str(), p4_target_conf_file);
                        // if context_path does not start with '/' assume it is
                        // relative to install dir
                        if ((context_path.front() != '/') && install_dir) {
                          context_path = std::string(install_dir) + '/'  + context_path;
                        }
                        if (nullptr != p4_name_lookup) {
                            printf("WARNING: Device %d: Pipe %d: Ignoring --p4-log-helper or "
                                   "-l option (%s)\n",
                                   chip, pipe, p4_name_lookup);
                        }
                    } else {
                        printf("WARNING: Device %d: Pipe %d: missing P4 name lookup file in %s\n",
                               chip, pipe, p4_target_conf_file);
                    }
                }
                if (context_path.empty() && (nullptr != p4_name_lookup)) {
                    // If not using p4_target_conf we use the deprecated method
                    // to load a single context file for chip 0.
                    if (multi_program && (chip != 0)) {
                        printf("WARNING: Device %d: Pipe %d: Cannot use --p4-log-helper or "
                               "-l option (%s) with multi-program on this device\n",
                               chip, pipe, p4_name_lookup);
                    } else {
                        printf("Device %d: Pipe %d: loading P4 name lookup file %s given by "
                               "--p4-log-helper or -l option\n",
                               chip, pipe, p4_name_lookup);
                        context_path = p4_name_lookup;
                        if (chip % 2) { // just for testing
                            rmt_set_log_pkt_signature(chip, -4, 4, true);
                        }
                    }
                }
                if (!context_path.empty()) {
                    int phy_pipe = g_log2phy_pipe[pipe] % g_pipes_per_die;
                    rmt_set_p4_name_lookup(chip + pipe/g_pipes_per_die,
                                           phy_pipe, context_path.c_str());
                } else {
                    printf("WARNING: Device %d: Pipe %d: P4 name lookup file not loaded, "
                           "unable to include P4 names in log messages\n",
                           chip, pipe);
                }
            }
            // enable Warn and more severe for model logging
            constexpr uint64_t DEFAULT_MODEL_LOG_LEVELS = UINT64_C(0xF);
            // enable Verbose and more severe for P4 logging
            constexpr uint64_t DEFAULT_P4_LOG_LEVELS = UINT64_C(0x7F);
            for (int log_type=0; log_type < MAX_RMT_LOG_TYPE; log_type++) {
              uint64_t levels = DEFAULT_P4_LOG_LEVELS;
              if (log_type == RMT_LOG_TYPE_MODEL) {
                levels = DEFAULT_MODEL_LOG_LEVELS;
              }
              rmt_update_log_type_levels(
                  chip,
                  model_core::RmtDebug::ALL,  // pipes
                  model_core::RmtDebug::ALL,  // stages
                  log_type,
                  model_core::RmtDebug::ALL,  // clear
                  levels);                    // set
            }
        }
    }

    // Permanently drop privileges now we've opened all files/veths
    // This applies to all threads (despite what the man page says)
    //
    printf("Dropping excess privileges...\n");
    int opret3 = os_privs_drop_permanently();
    if (opret3 != 0) {
      fprintf(stderr, "%s: Unable to permanently drop all privileges\n", argv[0]);
      exit(1);
    }


    /*************************************************************
     * Wait for a signal - SIGINT
     ************************************************************/
    signal (SIGINT, int_handler);

    pause();


    /*************************************************************
     * Clean up
     ************************************************************/
    bfm_port_finish();
    rmt_client_uninit();

    if (no_veth) {
        for (auto element : interfaces) {
            CHECK(bfm_port_interface_remove(element.c_str()));
        }
    } else {
        if (ofPortInfoFileExists) {
            addRemIntfsUsingJsonFile(ofPortInfo_file, false, chip_type, chip_cnt);
        } else {
            for (chip_id =0; chip_id<chip_cnt; chip_id++) {
                for (n_veth = 0; n_veth < g_port_count; n_veth++) {
                    std::stringstream veth_name;
                    veth_name << "veth" << (n_veth + g_port_count*chip_id)*2;
                    CHECK(bfm_port_interface_remove(veth_name.str().c_str()));
                }
            }
        }
    }

    rmt_uninit();

    printf("done\n");
    return rv;
}

typedef struct pkt_q_t {
    struct pkt_q_t *next;
    int      pkt_len;
    uint8_t *pkt_data;
} pkt_q_t;

// indexed by [asic][port]
//hack: could use a better max for number of asics
pkt_q_t *pkt_q_rtl[ 16 ][PORT_COUNT] = {{0}};
pkt_q_t *pkt_q_mdl[ 16 ][PORT_COUNT] = {{0}};

void nq_pkt( pkt_originator_e org,
             int chip, int port, uint8_t *pkt, int len )
{
    pkt_q_t *last = NULL;
    pkt_q_t *to_q = (pkt_q_t *)malloc( sizeof(pkt_q_t) );
    if (to_q == NULL) {
        printf("Memory allocation failed at %s:%d\n", __FILE__, __LINE__);
        assert(0);
        return;
    }

    pkt_q_t **pkt_q = (org == PKT_FROM_MODEL) ? &pkt_q_mdl[chip][port] :
                                                &pkt_q_rtl[chip][port];
    uint8_t *pkt_data = (uint8_t *)malloc(len);
    if (pkt_data == NULL) {
        free(to_q);
        printf("Memory allocation failed at %s:%d\n", __FILE__, __LINE__);
        assert(0);
        return;
    }

    memcpy((char*)pkt_data, (char *)pkt, len );

    to_q->pkt_len = len;
    to_q->pkt_data= pkt_data;
    to_q->next    = NULL;

    // append to queue
    last = *pkt_q;
    if (last == NULL) {
        *pkt_q = to_q;
        return;
    }
    while (last->next != NULL) {
        last = last->next;
    }
    last->next = to_q;
}

/********************************************************************
* match_pkt
*
* Look for a packet matching the len and byte-sequence in the passed
* values (pkt,len). If found, dequeue it. Caller will use other
* packet to transmit.
********************************************************************/
int match_pkt( pkt_originator_e org,
             int chip, int port, uint8_t *pkt, int len )
{
    pkt_q_t *last = NULL, *prev = NULL;
    pkt_q_t **pkt_q = (org == PKT_FROM_RTL) ? &pkt_q_mdl[chip][port] :
                                              &pkt_q_rtl[chip][port];
    last = *pkt_q;
    if (last == NULL) {
        return 0; // no packets queued
    }
    do {
        if ((len == last->pkt_len) &&
            (memcmp(pkt, last->pkt_data, len) == 0)) {
            // dequeue the pkt_q_t
            if (prev == NULL) {
                *pkt_q = (*pkt_q)->next;
            }
            else {
                prev->next = last->next;
            }
            free(last);
            return 1; // matched
        }
        prev = last;
        last = last->next;
    } while (last != NULL);
    return 0; // no match found
}
