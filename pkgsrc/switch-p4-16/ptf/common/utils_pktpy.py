################################################################################
 #  Copyright (C) 2024 Intel Corporation
 #
 #  Licensed under the Apache License, Version 2.0 (the "License");
 #  you may not use this file except in compliance with the License.
 #  You may obtain a copy of the License at
 #
 #  http://www.apache.org/licenses/LICENSE-2.0
 #
 #  Unless required by applicable law or agreed to in writing,
 #  software distributed under the License is distributed on an "AS IS" BASIS,
 #  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 #  See the License for the specific language governing permissions
 #  and limitations under the License.
 #
 #
 #  SPDX-License-Identifier: Apache-2.0
################################################################################

import copy
import random
import six
import socket
import sys 
import time

import binascii

from ptf.testutils import *
from ptf.packet import *
from ptf.dataplane import DataPlane
import ptf.mask
from bf_pktpy.library.specs.validate import *
from bf_pktpy.packets import Ether, IP, Dot1Q
from bf_pktpy.packets import FabricHeader
from bf_pktpy.packets import FabricCpuHeader
from bf_pktpy.packets import FabricPayloadHeader
from bf_pktpy.packets import FabricCpuTimestampHeader

from p4testutils.misc_utils import mask_set_do_not_care_packet

import logging
try:
    from pal_rpc.ttypes import *
except ImportError:
    pass

ETH_P_ALL = 3

###############################################################################
# Helper functions                                                            #
###############################################################################
def verify_any_packet_on_ports_list(test, pkts=[], ports=[], device_number=0, timeout=2, no_flood=False):
    """
    Ports is list of port lists
    Check that _any_ packet is received atleast once in every sublist in
    ports belonging to the given device (default device_number is 0).

    Also verifies that the packet is ot received on any other ports for this
    device, and that no other packets are received on the device
    (unless --relax is in effect).
    """
    rcv_idx = []
    failures = {}
    pkt_cnt = 0
    for port_list in ports:
        rcv_ports = set()
        remaining_timeout = timeout
        port_sub_list_failures = []
        port_sub_list_poll_success = False
        if remaining_timeout > 0:
            port_idx = 0
            port_list_len = len(port_list)
            while remaining_timeout > 0 or port_idx > 0:
                port = port_list[port_idx]
                port_idx = (port_idx + 1) % port_list_len
                remaining_timeout = remaining_timeout - 0.1
                (rcv_device, rcv_port, rcv_pkt, pkt_time) = test.dataplane.poll(
                    port_number=port, timeout=0.1, filters=get_filters())
                if rcv_device != device_number:
                    continue
                for pkt in pkts:
                    logging.debug("Checking for pkt on device %d, port %d",
                                  device_number, port)
                    if ptf.dataplane.match_exp_pkt(pkt, rcv_pkt):
                        pkt_cnt += 1
                        rcv_ports.add(port_list.index(rcv_port))
                        port_sub_list_failures = []
                        port_sub_list_poll_success = True
                        break
                    else:
                        port_sub_list_failures.append((port, DataPlane.PollFailure(pkt, [rcv_pkt], 1)))
                if port_sub_list_poll_success and no_flood:
                    break;
        # Either no expected packets received or unexpected packets recieved
        if not port_sub_list_poll_success or port_sub_list_failures:
            port_tuple = tuple(port_list)
            failures.setdefault(port_tuple, [])
            failures[port_tuple] = failures[port_tuple] + port_sub_list_failures
        rcv_idx.append(rcv_ports)

    verify_no_other_packets(test)
    if failures:
        def format_per_port_failure(port, failure):
            return "On port {}\n{}".format(port, failure.format())
        def format_per_port_failures(fail_list):
            return "\n".join([format_per_port_failure(port, failure) for (port, failure) in fail_list])
        def format_port_list_failures(port_list, fail_list):
            return "None of the expected packets received for port list {}: \n{}".format(port_list, format_per_port_failures(fail_list))
        failure_report = "\n".join([format_port_list_failures(port_list, fail_list) for port_list, fail_list in
            list(failures.items())])
        test.fail("Did not receive expected packets on any of ports_list {} for device {}. \n{}".format(ports,
            device_number, failure_report))

    test.assertTrue(pkt_cnt >= len(ports),
                    "Did not receive pkt on one of ports %r for device %d" %
                    (ports, device_number))
    return rcv_idx

def verify_packets_on_multiple_port_lists(test, pkts=[], ports=[], device_number=0, timeout=2):
    """
    Ports is list of port lists
    Check that given packets are received once in every sublist in
    ports belonging to the given device (default device_number is 0).

    Also verifies that the packet is not received on any other ports for this
    device, and that no other packets are received on the device
    (unless --relax is in effect).
    """
    test.assertTrue(len(pkts) == len(ports), "packet list count does not match port list count")

    pkt_cnt = 0
    rcv_idx = []
    for port_list, pkt in zip(ports, pkts):
        rcv_ports = set()
        for port in port_list:
            (rcv_device, rcv_port, rcv_pkt, pkt_time) = test.dataplane.poll(
                port_number=port, timeout=timeout, filters=get_filters())
            if rcv_device != device_number:
                continue
            logging.debug("Checking for pkt on device %d, port %d",
                          device_number, port)
            if ptf.dataplane.match_exp_pkt(pkt, rcv_pkt):
                pkt_cnt += 1
                rcv_ports.add(port_list.index(rcv_port))
                break
        rcv_idx.append(rcv_ports)

    verify_no_other_packets(test)
    test.assertTrue(pkt_cnt == len(pkts),
                    "Did not receive pkt on one of ports %r for device %d" %
                    (ports, device_number))
    return rcv_idx

def verify_any_packet_on_port(test, pkts=[], port_id=0, device_number=0):
    """
    Check that a packet received on the specified port is one of _any_ packets
    Useful to check packets arriving in non-deterministic order on a port
    Does not check any other ports (relaxed check)
    """
    device, port = port_to_tuple(port_id)
    logging.debug("Checking for pkt on device %d, port %d", device, port)
    (rcv_device, rcv_port, rcv_pkt, pkt_time) = dp_poll(
        test, device_number=device, port_number=port, timeout=2)
    test.assertTrue(rcv_pkt != None,
                    "Did not receive expected pkt on device %d, port %r" %
                    (device, port))
    for pkt in pkts:
        if str(pkt) == str(rcv_pkt):
            return True
    return False

def verify_multiple_packets_on_ports(test, plist=[], device=0, timeout=3):
    for port, pkts in plist:
        for n in range(0, len(pkts)):
            (rcv_device, rcv_port, rcv_pkt, pkt_time) = test.dataplane.poll(
                port_number=port, timeout=timeout, filters=get_filters())
            if rcv_port is None:
                test.assertTrue(False,
                                'Failed to receive packet(s) on %d' % port)
            for pkt in pkts[:]:
                if ptf.dataplane.match_exp_pkt(pkt, rcv_pkt):
                    pkts.remove(pkt)
                    logging.debug("received expected packet on port %d", rcv_port)
                else:
                    logging.debug("packet mismatch on port %d", rcv_port)
        test.assertTrue(
            len(pkts) == 0, "Not all packets for port %d were received" % port)
    verify_no_other_packets(test)

def verify_any_packet_any_port_v2(test, pkts=[], ports=[], device_number=0, timeout=2):
    """
    Check that any of given packets is received on any of the specified ports.
    Also verifies that the packet is not received on any other ports and that
    no other packets are received.
    In contrast to standard verify_any_packet_any_port() function this one
    allows to verify masked packets.
    """
    received = False
    match_index = 0

    result = dp_poll(test, device_number=device_number, timeout=timeout)

    if isinstance(result, test.dataplane.PollSuccess) and result.port in ports:
        received_packet = str(result.packet)
        for pkt in pkts:
            if ptf.dataplane.match_exp_pkt(pkt, received_packet):
                match_index = ports.index(result.port)
                received = True
    verify_no_other_packets(test, device_number=device_number)

    if isinstance(result, test.dataplane.PollFailure):
        test.fail("Did not receive any expected packet on any of ports %r for "
                  "device %d.\n%s" % (ports, device_number, result.format()))

    if result.port not in ports:
        test.fail("One of the expected packets was received on device %d on an "
                  "unexpected port: %d\n%s" % (device_number, result.port, result.format()))

    if not received:
        test.fail("Did not receive expected packet on any of ports for device %d.\n%s"
                  % (device_number, result.format()))

    return match_index

def verify_packet_from_cb(test, pkt):
    received = False
    attempts = 0
    in_port = 0
    pkt_size = 0
    while(attempts < 5):
        result = test.client.get_last_packet()
        if result.valid:
          received = True
          #if ptf.dataplane.match_exp_pkt(pkt, result.pkt):
          #    received = True
          in_port = result.port_handle
          pkt_size = result.pkt_size
          break
        time.sleep(1)
        attempts += 1

    if not received:
        test.fail("Did not receive expected packet via callback")

    return pkt_size, in_port

def flush_pool(test):
    """
    Wrapper function around dataplane.flush
    """
    test.dataplane.flush()

def get_swtype(client):
    board_type = client.pltfm_pm.pltfm_pm_board_type_get()
    swtype = ""
    if re.search("0x0234|0x1234|0x4234|0x5234", hex(board_type)):
        swtype = "mavericks"
    elif re.search("0x2234|0x3234", hex(board_type)):
        swtype = "montara"
    return swtype

def devport_to_swport(client, devport):
    if (test_param_get('arch') == 'Tofino') and \
       (test_param_get('target') == 'hw'):
        swport = devport
        try:
            # if built with platform then get the swtype
            swtype = get_swtype(client)
            fpport = getFrontPanelPort(swtype, devport)
            pgrp, chnl = fpport.split("/")
            swport = (int(pgrp) * 4 - 4) + int(chnl)
        except:
            # not built for platform return the devport
            return devport
        return swport
    else:
        return devport

def swport_to_devport(client, swport):
    if (test_param_get('arch') == 'Tofino') and \
       (test_param_get('target') == 'hw'):
        pgrp = swport/4 + 1
        chnl = swport%4
        try:
            # if built with platform then get the swtype
            return client.pal.pal_port_front_panel_port_to_dev_port(0, pgrp,
                                                                    chnl)
        except:
            # not built for platform return the swport
            return swport
    else:
        return swport

def get_cpu_port(client):
    cpu_port = 64
    if (test_param_get('arch') == 'Tofino') and \
       (test_param_get('target') == 'hw'):
        try:
            swtype = get_swtype(client)
            if swtype == "mavericks":
                cpu_port = 65 * 4 - 4
            elif swtype == "montara":
                cpu_port = 33 * 4 - 4
        except:
            return cpu_port

    return cpu_port

def get_pipeid(devport):
    if (test_param_get('arch') == 'Tofino') and \
       (test_param_get('target') == 'hw'):
        return (devport >> 7)
    else:
        return 0

###############################################################################
# CPU Header                                                                  #
###############################################################################



def simple_cpu_packet(header_version=0,
                      packet_version=0,
                      packet_type=5,
                      fabric_color=0,
                      fabric_qos=0,
                      dst_device=0,
                      dst_port_or_group=0,
                      ingress_ifindex=1,
                      ingress_bd=0,
                      egress_queue=0,
                      tx_bypass=False,
                      ingress_port=1,
                      reason_code=0,
                      sflow_sid=0,
                      bfd_sid=0,
                      arrival_time_0=0,
                      arrival_time_1=0,
                      inner_pkt=None):

    ether = inner_pkt.copy() if inner_pkt else Ether()
    eth_type = ether.type
    ether.type = 0x9000
    ether._body = None

    fabric_header = FabricHeader(
        packet_type=packet_type,
        header_version=header_version,
        packet_version=packet_version,
        pad1=0,
        fabric_color=fabric_color,
        fabric_qos=fabric_qos,
        dst_device=dst_device)

    fabric_cpu_header = FabricCpuHeader(
        tx_bypass=tx_bypass,
        reserved1=0,
        egress_queue=egress_queue,
        ingress_port=ingress_port,
        port_lag_index=ingress_ifindex,
        ingress_bd=ingress_bd,
        reason_code=reason_code)

    fabric_payload_header = FabricPayloadHeader(ether_type=eth_type)

    fabric_timestamp_header = FabricCpuTimestampHeader(
        arrival_time_0=arrival_time_0, arrival_time_1=arrival_time_1)

    pkt = ether / fabric_header / fabric_cpu_header

    if sflow_sid:
        pkt = pkt / FabricCpuSflowHeader(sflow_sid=sflow_sid)
    elif bfd_sid:
        pkt = pkt / FabricCpuBfdEventHeader(bfd_sid=bfd_sid)
    elif arrival_time_0:
        pkt = pkt / fabric_timestamp_header

    pkt = pkt / fabric_payload_header

    if inner_pkt and inner_pkt.body is not None:
        ether_payload = inner_pkt.body \
            if isinstance(inner_pkt.body, (six.binary_type, six.string_types)) \
            else inner_pkt.body.copy()
        pkt = pkt / ether_payload
    else:
        ip_pkt = simple_ip_only_packet()
        pkt = pkt / ip_pkt

    return pkt


def mpls_udp_inner_packet(pktlen=300, mpls_tags=[], inner_frame=None):
    """
    A mpls udp packet contains mpls labels and a payload that would be
    used inside of a mpls udp packet. The key differential between it and a
    simple mpls packet is that it does not have an ethernet header.

    Supports a few parameters:
    @param len Length of packet in bytes w/o CRC
    @param mpls_tags mpls tag stack
    @param inner_frame The inner frame

    """

    if MINSIZE > pktlen:
        pktlen = MINSIZE
    pkt = None
    mpls_tags = list(mpls_tags)
    while len(mpls_tags):
        tag = mpls_tags.pop(0)
        mpls = MPLS()
        if 'label' in tag:
            mpls.label = tag['label']
        if 'tc' in tag:
            mpls.cos = tag['tc']
        if 'ttl' in tag:
            mpls.ttl = tag['ttl']
        if 's' in tag:
            mpls.s = tag['s']
        if pkt == None:
            pkt = mpls
        else:
            pkt = pkt / mpls
    if inner_frame:
        pkt = pkt / inner_frame
    else:
        pkt = pkt / simple_tcp_packet(pktlen=pktlen - len(pkt))
    return pkt


def simple_unicast_fabric_packet(header_version=0,
                                 packet_version=0,
                                 fabric_color=0,
                                 fabric_qos=0,
                                 dst_device=0,
                                 dst_port_or_group=0,
                                 routed=0,
                                 outer_routed=0,
                                 tunnel_terminate=0,
                                 ingress_tunnel_type=0,
                                 nexthop_index=0,
                                 inner_pkt=None):

    ether = Ether(str(inner_pkt))
    eth_type = ether.type
    ether.type = 0x9000

    fabric_header = FabricHeader(
        packet_type=0x1,
        header_version=header_version,
        packet_version=packet_version,
        pad1=0,
        fabric_color=fabric_color,
        fabric_qos=fabric_qos,
        dst_device=dst_device)

    fabric_unicast_header = FabricUnicastHeader(
        routed=0,
        outerRouted=0,
        tunnelTerminate=0,
        ingressTunnelType=0,
        nexthopIndex=0)

    fabric_payload_header = FabricPayloadHeader(ether_type=eth_type)

    if inner_pkt:
        pkt = (
            str(ether)[:14]
        ) / fabric_header / fabric_unicast_header / fabric_payload_header / (
            str(inner_pkt)[14:])
    else:
        ip_pkt = simple_ip_only_packet()
        pkt = (
            str(ether)[:14]
        ) / fabric_header / fabric_unicast_header / fabric_payload_header / ip_pkt

    return pkt


def simple_multicast_fabric_packet(header_version=0,
                                   packet_version=0,
                                   fabric_color=0,
                                   fabric_qos=0,
                                   dst_device=0,
                                   dst_port_or_group=0,
                                   routed=0,
                                   outer_routed=0,
                                   tunnel_terminate=0,
                                   ingress_tunnel_type=0,
                                   ingress_ifindex=1,
                                   ingress_bd=0,
                                   mcast_grp_A=0,
                                   mcast_grp_B=0,
                                   ingress_rid=0,
                                   l1_exclusion_id=0,
                                   inner_pkt=None):

    ether = Ether(str(inner_pkt))
    eth_type = ether.type
    ether.type = 0x9000

    fabric_header = FabricHeader(
        packet_type=0x2,
        header_version=header_version,
        packet_version=packet_version,
        pad1=0,
        fabric_color=fabric_color,
        fabric_qos=fabric_qos,
        dst_device=dst_device)

    fabric_multicast_header = FabricMulticastHeader(
        routed=routed,
        outerRouted=outer_routed,
        tunnelTerminate=tunnel_terminate,
        ingressTunnelType=ingress_tunnel_type,
        ingressIfindex=ingress_ifindex,
        ingressBd=ingress_bd,
        mcastGrpA=mcast_grp_A,
        mcastGrpB=mcast_grp_B,
        ingressRid=ingress_rid,
        l1ExclusionId=l1_exclusion_id)

    fabric_payload_header = FabricPayloadHeader(ether_type=eth_type)

    if inner_pkt:
        pkt = (
            str(ether)[:14]
        ) / fabric_header / fabric_multicast_header / fabric_payload_header / (
            str(inner_pkt)[14:])
    else:
        ip_pkt = simple_ip_only_packet()
        pkt = (
            str(ether)[:14]
        ) / fabric_header / fabric_multicast_header / fabric_payload_header / ip_pkt

    return pkt


def simple_eth_dot1q_packet(eth_dst='00:01:02:03:04:05',
                            eth_src='00:06:07:08:09:0a',
                            dl_vlan_enable=False,
                            vlan_vid=0,
                            vlan_pcp=0,
                            dl_vlan_cfi=0,
                            pktlen=60,
                            eth_type=0x88cc):

    if dl_vlan_enable:
        pkt = Ether(dst=eth_dst, src=eth_src, type=eth_type)/ \
              Dot1Q(prio=vlan_pcp, id=dl_vlan_cfi, vlan=vlan_vid)
    else:
        pkt = Ether(dst=eth_dst, src=eth_src, type=eth_type)

    pkt = pkt/("0" * (pktlen - len(pkt)))

    return pkt

def simple_igmp_packet(pktlen=100,
                      eth_dst='01:00:5e:00:00:01',
                      eth_src='00:06:07:08:09:0a',
                      dl_vlan_enable=False,
                      vlan_vid=0,
                      vlan_pcp=0,
                      dl_vlan_cfi=0,
                      ip_src='192.168.0.1',
                      ip_dst='224.0.0.1',
                      ip_tos=0,
                      ip_ttl=64,
                      ip_ihl=None,
                      ip_options=False,
                      igmp_type=0x11,
                      igmp_mrtime=20,
                      igmp_gaddr='0.0.0.0'
                      ):

    igmp_hdr = IGMP(type=igmp_type, mrcode=igmp_mrtime, gaddr=igmp_gaddr)

    # Note Dot1Q.id is really CFI
    if (dl_vlan_enable):
        pkt = Ether(dst=eth_dst, src=eth_src)/ \
            Dot1Q(prio=vlan_pcp, id=dl_vlan_cfi, vlan=vlan_vid)/ \
            IP(src=ip_src, dst=ip_dst, tos=ip_tos, ttl=ip_ttl, ihl=ip_ihl)/ \
            igmp_hdr
    else:
        if not ip_options:
            pkt = Ether(dst=eth_dst, src=eth_src)/ \
                IP(src=ip_src, dst=ip_dst, tos=ip_tos, ttl=ip_ttl, ihl=ip_ihl)/ \
                igmp_hdr
        else:
            pkt = Ether(dst=eth_dst, src=eth_src)/ \
                IP(src=ip_src, dst=ip_dst, tos=ip_tos, ttl=ip_ttl, ihl=ip_ihl, options=ip_options)/ \
                igmp_hdr

    # if udp_payload:
    #     pkt = pkt/udp_payload

    pkt = pkt/("".join([chr(x % 256) for x in range(pktlen - len(pkt))]))

    return pkt


######
# Pktgen header
######
def ifindex_from_pipe_port(pipe, port):
    return (pipe * 72) + port + 1


def bfd_event_cpu_packet(mac_da='00:00:01:00:00:00',
                         ingress_port=128,
                         ingress_ifindex=69,
                         rcode=0x217,
                         bfd_sid=1,
                         bfd_event=0):
    pktgen_bfd_pkt = bfd_ipv4_packet(
        pktlen=66,
        eth_dst=mac_da,
        eth_src='00:00:00:00:00:00',
        dl_vlan_enable=False,
        vlan_vid=0,
        vlan_pcp=0,
        dl_vlan_cfi=0,
        ip_src='0.0.0.0',
        ip_dst='0.0.0.0',
        ip_tos=48,
        ip_ttl=255,
        ip_id=0x0001,
        udp_sport=0,
        udp_dport=0,
        with_udp_chksum=False,
        ip_ihl=None,
        ip_options=False,
        version=1,
        diag=0,
        sta=3,
        flags=0x00,
        detect_mult=0x00,
        bfdlen=24,
        my_discriminator=0,
        your_discriminator=0,
        min_tx_interval=00000,
        min_rx_interval=0000,
        echo_rx_interval=0000)
    pkt = simple_cpu_packet(
        header_version=0,
        packet_version=0,
        fabric_color=0,
        fabric_qos=0,
        dst_device=0,
        dst_port_or_group=0,
        ingress_ifindex=ingress_ifindex,
        ingress_bd=0,
        egress_queue=0,
        tx_bypass=False,
        ingress_port=ingress_port,
        reason_code=rcode,
        sflow_sid=0,
        bfd_sid=bfd_sid,
        inner_pkt=pktgen_bfd_pkt)

    fab_pyld_hdr = pkt.getlayer(FabricPayloadHeader)
    fab_pyld_hdr.ether_type = 0x9001
    return pkt


###############################################################################
# CRC16 and Entropy hash calculation                                          #
###############################################################################
import crc16


def crc16_regular(buff, crc=0, poly=0xa001):
    l = len(buff)
    i = 0
    while i < l:
        ch = ord(buff[i])
        uc = 0
        while uc < 8:
            if (crc & 1) ^ (ch & 1):
                crc = (crc >> 1) ^ poly
            else:
                crc >>= 1
            ch >>= 1
            uc += 1
        i += 1
    return crc


def entropy_hash(pkt, layer='ipv4', ifindex=0):
    buff = ''
    if layer == 'ether':
        buff += str(format(ifindex, '02x')).zfill(4)
        buff += pkt[Ether].src.translate(None, ':')
        buff += pkt[Ether].dst.translate(None, ':')
        buff += str(hex(pkt[Ether].type)[2:]).zfill(4)
    elif layer == 'ipv4':
        buff += binascii.hexlify(socket.inet_aton(pkt[IP].src))
        buff += binascii.hexlify(socket.inet_aton(pkt[IP].dst))
        buff += str(hex(pkt[IP].proto)[2:]).zfill(2)
        if pkt[IP].proto == 6:
            buff += str(hex(pkt[TCP].sport)[2:]).zfill(4)
            buff += str(hex(pkt[TCP].dport)[2:]).zfill(4)
        elif pkt[IP].proto == 17:
            buff += str(hex(pkt[UDP].sport)[2:]).zfill(4)
            buff += str(hex(pkt[UDP].dport)[2:]).zfill(4)
    elif layer == 'ipv6':
        buff += binascii.hexlify(socket.inet_pton(socket.AF_INET6, pkt[IPv6].src))
        buff += binascii.hexlify(socket.inet_pton(socket.AF_INET6, pkt[IPv6].dst))
        buff += str(hex(pkt[IPv6].nh)[2:]).zfill(2)
        if pkt[IPv6].nh == 6:
            buff += str(hex(pkt[TCP].sport)[2:]).zfill(4)
            buff += str(hex(pkt[TCP].dport)[2:]).zfill(4)
        elif pkt[IPv6].nh == 17:
            buff += str(hex(pkt[UDP].sport)[2:]).zfill(4)
            buff += str(hex(pkt[UDP].dport)[2:]).zfill(4)
    else:
        buff = ''
    h = crc16_regular(buff.decode('hex'))
    return h


def open_packet_socket(hostif_name):
    s = socket.socket(socket.AF_PACKET, socket.SOCK_RAW,
                      socket.htons(ETH_P_ALL))
    s.bind((hostif_name, ETH_P_ALL))
    s.setblocking(0)
    return s


def socket_verify_packet(pkt, s, timeout=2):
    MAX_PKT_SIZE = 9100
    timeout = time.time() + timeout
    match = False

    if isinstance(pkt, ptf.mask.Mask):
        if not pkt.is_valid():
            return False

    while time.time() < timeout:
        try:
            packet_from_tap_device = s.recv(MAX_PKT_SIZE)

            if isinstance(pkt, ptf.mask.Mask):
                match = pkt.pkt_match(packet_from_tap_device)
            else:
                match = (str(packet_from_tap_device) == str(pkt))

            if match:
                break

        except:
            pass

    return match


def socket_send_packet(pkt, s):
    s.send(bytes(pkt))

def cpu_packet_mask_ingress_bd(pkt):
    pkt = ptf.mask.Mask(pkt)
    mask_set_do_not_care_packet(pkt, FabricCpuHeader, "ingress_bd")
    return pkt

def cpu_packet_mask_ingress_bd_and_timestamp(pkt):
    pkt = ptf.mask.Mask(pkt)
    mask_set_do_not_care_packet(pkt, FabricCpuHeader, "ingress_bd")
    mask_set_do_not_care_packet(pkt, FabricCpuTimestampHeader, "arrival_time_0")
    mask_set_do_not_care_packet(pkt, FabricCpuTimestampHeader, "arrival_time_1")
    return pkt

def cpu_packet_mask_ingress_bd_and_ifindex(pkt):
    pkt = ptf.mask.Mask(pkt)
    mask_set_do_not_care_packet(pkt, FabricCpuHeader, "ingress_bd")
    mask_set_do_not_care_packet(pkt, FabricCpuHeader, "port_lag_index")
    return pkt

def cpu_packet_mask_ingress_bd_port_and_ifindex(pkt):
    pkt = ptf.mask.Mask(pkt)
    mask_set_do_not_care_packet(pkt, FabricCpuHeader, "ingress_bd")
    mask_set_do_not_care_packet(pkt, FabricCpuHeader, "ingress_port")
    mask_set_do_not_care_packet(pkt, FabricCpuHeader, "port_lag_index")

def ether_packet_mask_type(pkt):
    pkt = ptf.mask.Mask(pkt)
    mask_set_do_not_care_packet(pkt, Ether, "type")
    return pkt

def dot1q_packet_mask_types(pkt):
    pkt = ptf.mask.Mask(pkt)
    mask_set_do_not_care_packet(pkt, Dot1Q, "type")
    mask_set_do_not_care_packet(pkt, Ether, "type")
    return pkt

def truncate_packet(full_pkt, truncate_size):
    truncated_packet = full_pkt.load_bytes(bytes(full_pkt)[:-truncate_size])
    truncated_packet["IP"].reset_chksum()
    return truncated_packet
