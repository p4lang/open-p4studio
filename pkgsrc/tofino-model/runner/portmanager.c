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

#include "portmanager.h"
#include "bmi_port.h"

#include <assert.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <asm/types.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <linux/sockios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

/* The vector for the packet handler */

static bfm_packet_handler_vector_f packet_handler_vector;

/* Can hard code here or direct to vector above */
#define PACKET_HANDLER_VECTOR packet_handler_vector

void
bfm_packet_handler_vector_set(bfm_packet_handler_vector_f fn)
{
    packet_handler_vector = fn;
}


#define MAX_PKT_LEN   16384     /* Maximum packet length */
#define PORT_COUNT_PER_CHIP_MAX 456
#define MAX_CHIPS 8
#define PORT_COUNT_MAX (MAX_CHIPS*PORT_COUNT_PER_CHIP_MAX)+1

/* Per-port data */
typedef struct port_info_s {
    char     ifname[128];       /* Name of port's VPI or Linux network intf */
    bool     is_port_in_use;    /* Flag to indicate if the port is in use */
    uint64_t cnt_tx_pkts;       /* Transmitted packets counter */
    uint64_t cnt_tx_bytes;      /* Transmitted bytes counter */
    uint64_t cnt_rx_pkts;       /* Received packets counter */
    uint64_t cnt_rx_bytes;      /* Received bytes counter */
} port_info_t;

static port_info_t port_info[PORT_COUNT_MAX];

static bmi_port_mgr_t *port_mgr = NULL;

static int bfm_port_count = 0;

// if bfm_set_pcap_outdir is called the passed path name
// is copied here and pcap_output_dir is set to point to it
char  pcap_output_dir_storage[180] = {0};

char  pcap_output_dir_storage_default[2] = {'.', 0};
char *pcap_output_dir = pcap_output_dir_storage_default;

#include <sys/stat.h>
#include <sys/types.h>
int mkdir(const char *pathname, mode_t mode);

/* bfm_set_pcap_outdir
 *
 */
void bfm_set_pcap_outdir(const char *outdir_name)
{
    sprintf( pcap_output_dir_storage, "%s", outdir_name );
    pcap_output_dir = pcap_output_dir_storage;
    mkdir( pcap_output_dir, 0777 );
}

/*
 * Follow the semantics that 0 is not used as a port number.
 * Subtract 1 from the port num to get the port_info index
 */
static int
port_number_valid(int port_num)
{
    return (port_num >= 0 && port_num < bfm_port_count);
}


/* Return pointer to port structure for given port number */

static port_info_t *
port_num_to_info(uint32_t port_num)
{
    return &port_info[port_num];
}

/* Check if a port is in use */
#define PORT_IN_USE(port_info) ((port_info)->is_port_in_use == true)

#define LOG_ERROR(...) do { \
  fprintf(stderr, __VA_ARGS__); \
} while (0)

#define LOG_INFO(...) do { \
  fprintf(stdout, __VA_ARGS__); \
} while (0)

/* @FIXME Need port stats interface */

/* Process packet received on socket */

static void
pkt_rx(int port_num, const char *buffer, int length)
{
    port_info_t *pi = NULL;
    char small_buffer[64];
    int orig_length = length;

    /* Find corresponding port */
    if (PORT_IN_USE(&(port_info[port_num]))) {
        pi = &port_info[port_num];
    }

    if (pi == NULL) {
        LOG_ERROR("Port %d is not in use\n", port_num);
        return;
    }

    if (length == 0) {
        /* No packet */
        LOG_INFO("No packet data received from %s\n", pi->ifname);
        return;
    }

    /* If packet is smaller than 64 bytes, pad it */
    if (length < 64) {
        memset(small_buffer, 0, sizeof(small_buffer));
        memcpy(small_buffer, buffer, length);
        length = 64;
        buffer = small_buffer;
    }

    /* Update port stats */
    ++pi->cnt_rx_pkts;
    pi->cnt_rx_bytes += (uint64_t) length;

    /* Pass to the packet handler */
    if (PACKET_HANDLER_VECTOR != NULL) {
      PACKET_HANDLER_VECTOR(port_num, (uint8_t *)buffer, length, orig_length);
    } else {
        LOG_INFO("Dropped packet as handler vector is NULL\n");
    }
}

/***************************************************************************/

/* Add the given Linux network interface as the given OF port number */
bfm_error_t
bfm_port_interface_add(const char *ifname, uint32_t port_num,
		       const char *sw_name, int dump_pcap)
{
    port_info_t *pi;

    if (ifname == NULL) {
      LOG_ERROR("Adding interface failed: interface name is NULL\n");
      return BFM_E_PARAM;
    }

    LOG_INFO("Adding interface %s as port %d\n", ifname, port_num);

    if (port_mgr == NULL) {
        LOG_ERROR("PortMgr non-existent\n");
        return BFM_E_NOT_FOUND;
    }

    if (!port_number_valid(port_num)) {
        LOG_ERROR("Invalid port number\n");
        return BFM_E_PARAM;
    }

    if (PORT_IN_USE(pi = port_num_to_info(port_num))) {
        LOG_ERROR("OF port number in use\n");
        return BFM_E_EXISTS;
    }

    /* @fixme Should check if name already exists */

    /*
     * Assume ifname refers to a network adapter unless it has a pipe character
     * in it.
     */
    char pcap_name[1024];
    int interface_add_return_value = 0;
    if(dump_pcap)
    {
        snprintf(pcap_name, sizeof(pcap_name),
                 "%s/bfns.%s-port%.2d.pcap",
                 pcap_output_dir, sw_name, port_num);
        interface_add_return_value = bmi_port_interface_add(port_mgr,
                                                            ifname, port_num,
                                                            pcap_name);
    }
    else {
        interface_add_return_value = bmi_port_interface_add(port_mgr, ifname,
                                                            port_num, NULL);
    }

    if (interface_add_return_value != 0) {
        LOG_ERROR("bmi_port_interface_add failed for port %d.\n", port_num);
    }

    memset(pi, 0, sizeof(*pi));
    strncpy(pi->ifname, ifname, sizeof(pi->ifname) - 1);
    pi->ifname[sizeof(pi->ifname) - 1] = 0;
    pi->is_port_in_use = true;

    return BFM_E_NONE;
}

/* Stop using the given Linux network interface as an OF port */

bfm_error_t
bfm_port_interface_remove(const char *ifname)
{
    port_info_t *pi;
    int idx;

    if (ifname == NULL) {
      LOG_ERROR("Removing interface failed: interface name is NULL\n");
      return BFM_E_PARAM;
    }

    for (idx = 0; idx < PORT_COUNT_MAX; idx++) {
        pi = &port_info[idx];
        if (PORT_IN_USE(pi) && (port_mgr != NULL)) {
            if (strncmp(ifname, pi->ifname, 128) == 0) {
                LOG_INFO("Removing port %d (%s)\n", idx + 1, pi->ifname);
                bmi_port_interface_remove(port_mgr, idx);
            }
        }
    }
    return BFM_E_NONE;
}


/***************************************************************************/

/* Transmit given packet out OF port */

bfm_error_t
bfm_port_packet_emit(uint32_t port_num,
                     uint16_t queue_id,
                     uint8_t *data,
                     int len)
{
    port_info_t *pi;

  //LOG_TRACE("Emit %d bytes to port %d, queue %d",
  //          len, port_num, queue_id);

    if (port_mgr == NULL) {
        LOG_ERROR("PortMgr non-existent\n");
        return BFM_E_NOT_FOUND;
    }

    if (!port_number_valid(port_num)) {
        LOG_ERROR("Invalid OF port number\n");
        return BFM_E_PARAM;
    }

    if (queue_id != 0) {
        LOG_ERROR("Invalid transmit queue\n");
        return BFM_E_PARAM;
    }

    if (!PORT_IN_USE(pi = port_num_to_info(port_num))) {
        LOG_ERROR("Port not in use\n");
        return BFM_E_NOT_FOUND;
    }

    /* Send packet out network interface */
    if (bmi_port_send(port_mgr, port_num, (const char *)data, len) < 0) {
        LOG_ERROR("bmi_port_send() failed\n");
        return BFM_E_UNKNOWN;
    }

    /* Update port stats */
    ++pi->cnt_tx_pkts;
    pi->cnt_tx_bytes += len;

    return BFM_E_NONE;
}


#if 0 /* Emit to group probably won't be necessary due to egress pipe arch */
/**
 * Transmit given packet out a group of ports
 *
 * The only group ID currently supported is "flood".  The value for the
 * flood group is the port-flood id, 0xfffffffb.
 */

bfm_error_t
bfm_port_packet_emit_group(uint32_t group_id,
                           uint32_t ingress_port_num,
                           uint8_t *data,
                           int len)
{
    port_info_t *pi;
    int idx, port_num;

    /* @FIXME Implement groups */

    LOG_TRACE("Send %d bytes to group 0x%x", len, group_id);
    for (idx = 0, port_num = 1; idx < bfm_port_count; ++idx, ++port_num) {
        pi = port_num_to_info(port_num);
        if (!PORT_IN_USE(pi) || port_num == ingress_port_num) {
            continue;
        }
        (void)common_port_packet_emit(port_num, 0, data, len);
    }

    return BFM_E_NONE;  /* @FIXME */
}


/* Transmit given packet out all OF ports, except given one */

bfm_error_t
bfm_port_packet_emit_all(uint32_t skip_port_num,
                         uint8_t *data,
                         int len)
{
    port_info_t *pi;
    int idx, port_num;

    LOG_TRACE("Send %d bytes to all except %d", len, skip_port_num);

    for (idx = 0, port_num = 1; idx < bfm_port_count; ++idx, ++port_num) {
        pi = port_num_to_info(port_num);
        if (!PORT_IN_USE(pi) || port_num == skip_port_num) {
            continue;
        }
        (void)common_port_packet_emit(port_num, 0, data, len);
    }

    return BFM_E_NONE;  /* @FIXME */
}

#endif

/***************************************************************************/

/* Initialize module */

bfm_error_t
bfm_port_init(int port_count)
{
    int i;

    assert(0 == bmi_port_create_mgr(&port_mgr));
    assert(0 == bmi_set_packet_handler(port_mgr, pkt_rx));

    if (port_count >= PORT_COUNT_MAX) {
        LOG_ERROR("Too many ports for port manager: %d > %d\n",
                  port_count, PORT_COUNT_MAX);
        return BFM_E_PARAM;
    }

    bfm_port_count = port_count;

    memset(port_info, 0, sizeof(port_info));
    for (i = 0; i < PORT_COUNT_MAX; ++i) {
        port_info[i].is_port_in_use = false;
    }

    return BFM_E_NONE;
}

bfm_error_t
bfm_port_finish(void)
{
    if (port_mgr != NULL) bmi_port_destroy_mgr(port_mgr);

    port_mgr = NULL;

    return BFM_E_NONE;
}

bfm_error_t
bfm_port_start_pkt_processing(void) {
  assert(0 == bmi_port_start_pkt_processing());
  return BFM_E_NONE;
}

int
bfm_get_port_count(void)
{
    return PORT_COUNT_MAX;
}

bool
bfm_is_if_up(int port)
{
  if (port_mgr == NULL)
      return false;
  if (0 > port || PORT_COUNT_MAX <= port)
      return false;

  port_info_t *pi = port_num_to_info(port);
  if (!PORT_IN_USE(pi))
      return false;

  bool is_up = false;
  bmi_port_interface_is_up(port_mgr, port, &is_up);
  return is_up;
}