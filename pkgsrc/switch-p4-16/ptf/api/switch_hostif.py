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

"""
Thrift API interface basic tests
"""

import bf_switcht_api_thrift

import time
import sys
import logging

import unittest
import random

import ptf.dataplane as dataplane
import api_base_tests
import pd_base_tests
from switch_helpers import ApiHelper
import model_utils as u
from p4testutils.misc_utils import mask_set_do_not_care_packet

from ptf import config
from ptf.testutils import *
from ptf.packet import *
from ptf.thriftutils import *
from ptf.mask import *

import os
import ptf.mask

this_dir = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(this_dir, '../../../ptf-utils'))

from bf_switcht_api_thrift.ttypes import *
from bf_switcht_api_thrift.model_headers import *
this_dir = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(this_dir, '..'))
from common.utils import *

SWITCH_HOSTIF_REASON_CODE_ARP_REQUEST = 0x0

###############################################################################
@group('hostif')
class HostIfRxTest(ApiHelper):
    def setUp(self):
        print()
        self.configure()

        self.nhop_glean = self.add_nexthop(self.device, type=SWITCH_NEXTHOP_ATTR_TYPE_GLEAN)
        self.route0 = self.add_route(self.device, ip_prefix='30.10.10.1', vrf_handle=self.vrf10, nexthop_handle=self.nhop_glean)

        # create l3 interface and associate with self.port0
        self.rif0 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port0, vrf_handle=self.vrf10,
                                 src_mac=self.rmac)
        self.port0_ingress_ifindex = self.get_port_ifindex(self.port0)
        self.route0 = self.add_route(self.device, ip_prefix='10.10.10.1', vrf_handle=self.vrf10, is_host_myip=True)
        self.route0 = self.add_route(self.device, ip_prefix='2000::3', vrf_handle=self.vrf10, is_host_myip=True)

        self.rif1 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port1, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.nhop1 = self.add_nexthop(self.device, handle=self.rif1, dest_ip='20.10.10.1')
        self.neighbor1 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=self.rif1, dest_ip='20.10.10.1')
        self.route1 = self.add_route(self.device, ip_prefix='20.10.10.1', vrf_handle=self.vrf10, nexthop_handle=self.nhop1)

        self.lag0 = self.add_lag(self.device)
        lag_mbr0 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port3)
        self.lag0_ingress_ifindex = self.get_port_ifindex(self.lag0,port_type=1)

        self.port4_ingress_ifindex = self.get_port_ifindex(self.port4)
        vlan_mbr0 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port4)
        self.attribute_set(self.port4, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)

        vlan_mbr1 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port5)
        self.attribute_set(self.port5, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)

        self.meter = self.add_meter(self.device,
            mode=SWITCH_METER_ATTR_MODE_TWO_RATE_THREE_COLOR,
            pbs=16,cbs=8,cir=1000,pir=1000,
            type=SWITCH_METER_ATTR_TYPE_PACKETS,
            color_source=SWITCH_METER_ATTR_COLOR_SOURCE_BLIND)
        # installing network route for l3 interface network ??. Required for to punt ICMP??
        # create trap group.
        queue_handles = self.attribute_get(self.cpu_port_hdl, SWITCH_PORT_ATTR_QUEUE_HANDLES)
        self.hostif_trap_group0 = self.add_hostif_trap_group(self.device, queue_handle=queue_handles[0].oid, admin_state=True, policer_handle=self.meter)

        # create hostif for port0
        self.hostif_name1 = "test_host_if0"
        self.hostif0 = self.add_hostif(self.device, name=self.hostif_name1, handle=self.port0, oper_status=True)
        self.assertTrue(self.hostif0 != 0)

        # create hostif for port3
        self.hostif_name2 = "test_host_if1"
        self.hostif1 = self.add_hostif(self.device, name=self.hostif_name2, handle=self.port3, oper_status=True)
        self.assertTrue(self.hostif1 != 0)

        # create hostif for port4
        self.hostif_name3 = "test_host_if2"
        self.hostif2 = self.add_hostif(self.device, name=self.hostif_name3, handle=self.port4, oper_status=True)
        self.assertTrue(self.hostif2 != 0)

        print("Sleeping for 5 secs")
        time.sleep(5)

        """
        # open socket to verfiy packet recieved on hostif netd.
        self.sock = open_packet_socket(hostif_name1)
        self.rx_cnt = 0
        self.pre_counter = self.client.object_counters_get(self.hostif0)

        self.sock2 = open_packet_socket(hostif_name2)
        self.rx_cnt2 = 0
        self.pre_counter2 = self.client.object_counters_get(self.hostif1)
        """
        # create hostif_trap entries for following trap types.

        self.trap_list = [
          [SWITCH_HOSTIF_TRAP_ATTR_TYPE_TTL_ERROR, SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_DROP, 10],
          [SWITCH_HOSTIF_TRAP_ATTR_TYPE_ARP_REQUEST, SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_COPY_TO_CPU, 10],
          [SWITCH_HOSTIF_TRAP_ATTR_TYPE_ARP_RESPONSE, SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_REDIRECT_TO_CPU, 10],
          [SWITCH_HOSTIF_TRAP_ATTR_TYPE_OSPF, SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_COPY_TO_CPU, 10],
          [SWITCH_HOSTIF_TRAP_ATTR_TYPE_IGMP_TYPE_QUERY, SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_REDIRECT_TO_CPU, 10],
          [SWITCH_HOSTIF_TRAP_ATTR_TYPE_IGMP_TYPE_LEAVE, SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_REDIRECT_TO_CPU, 10],
          [SWITCH_HOSTIF_TRAP_ATTR_TYPE_IGMP_TYPE_V1_REPORT, SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_REDIRECT_TO_CPU, 10],
          [SWITCH_HOSTIF_TRAP_ATTR_TYPE_IGMP_TYPE_V2_REPORT, SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_REDIRECT_TO_CPU, 10],
          [SWITCH_HOSTIF_TRAP_ATTR_TYPE_IGMP_TYPE_V3_REPORT, SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_REDIRECT_TO_CPU, 10],
          [SWITCH_HOSTIF_TRAP_ATTR_TYPE_PIM, SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_COPY_TO_CPU, 10],
          [SWITCH_HOSTIF_TRAP_ATTR_TYPE_EAPOL, SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_COPY_TO_CPU, 10],
          [SWITCH_HOSTIF_TRAP_ATTR_TYPE_STP, SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_REDIRECT_TO_CPU, 10],
          [SWITCH_HOSTIF_TRAP_ATTR_TYPE_PVRST, SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_REDIRECT_TO_CPU, 10],
          [SWITCH_HOSTIF_TRAP_ATTR_TYPE_UDLD, SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_REDIRECT_TO_CPU, 10],
          [SWITCH_HOSTIF_TRAP_ATTR_TYPE_DHCP, SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_REDIRECT_TO_CPU, 10],
          [SWITCH_HOSTIF_TRAP_ATTR_TYPE_BGP, SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_REDIRECT_TO_CPU, 10],
          [SWITCH_HOSTIF_TRAP_ATTR_TYPE_ISIS, SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_REDIRECT_TO_CPU, 10],
          [SWITCH_HOSTIF_TRAP_ATTR_TYPE_BGPV6, SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_REDIRECT_TO_CPU, 10],
          [SWITCH_HOSTIF_TRAP_ATTR_TYPE_ICCP, SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_REDIRECT_TO_CPU, 10],
          [SWITCH_HOSTIF_TRAP_ATTR_TYPE_ICMP, SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_REDIRECT_TO_CPU, 10],
          [SWITCH_HOSTIF_TRAP_ATTR_TYPE_IPV6_NEIGHBOR_DISCOVERY, SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_REDIRECT_TO_CPU, 10],
          [SWITCH_HOSTIF_TRAP_ATTR_TYPE_ICMPV6, SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_REDIRECT_TO_CPU, 11], # lower priority than ipv6 nd
          [SWITCH_HOSTIF_TRAP_ATTR_TYPE_BFD, SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_REDIRECT_TO_CPU, 10],
          [SWITCH_HOSTIF_TRAP_ATTR_TYPE_BFDV6, SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_REDIRECT_TO_CPU, 10],
          [SWITCH_HOSTIF_TRAP_ATTR_TYPE_LACP, SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_REDIRECT_TO_CPU, 10],
          [SWITCH_HOSTIF_TRAP_ATTR_TYPE_LLDP, SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_REDIRECT_TO_CPU, 10],
          [SWITCH_HOSTIF_TRAP_ATTR_TYPE_DHCPV6, SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_REDIRECT_TO_CPU, 10],
          [SWITCH_HOSTIF_TRAP_ATTR_TYPE_GNMI, SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_REDIRECT_TO_CPU, 10],
          [SWITCH_HOSTIF_TRAP_ATTR_TYPE_P4RT, SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_REDIRECT_TO_CPU, 10],
          [SWITCH_HOSTIF_TRAP_ATTR_TYPE_NTPCLIENT, SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_REDIRECT_TO_CPU, 10],
          [SWITCH_HOSTIF_TRAP_ATTR_TYPE_NTPSERVER, SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_REDIRECT_TO_CPU, 10]
        ]

        # catchall for myip
        self.add_hostif_trap(self.device, type=SWITCH_HOSTIF_TRAP_ATTR_TYPE_MYIP,
                             hostif_trap_group_handle=self.hostif_trap_group0,
                             packet_action=SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_DROP,
                             priority=100)
        for trap in self.trap_list:
            trap.append(self.add_hostif_trap(self.device, type=trap[0],
                                             hostif_trap_group_handle=self.hostif_trap_group0,
                                             packet_action=trap[1],
                                             priority=trap[2]))
    def runTest(self):
        print()
        try:
            if self.test_params['target'] != 'hw':
                self.sock = open_packet_socket(self.hostif_name1)
                self.sock2 = open_packet_socket(self.hostif_name2)
                self.sock3 = open_packet_socket(self.hostif_name3)
            self.rx_cnt = 0
            self.pre_counter = self.client.object_counters_get(self.hostif0)
            self.rx_cnt2 = 0
            self.pre_counter2 = self.client.object_counters_get(self.hostif1)
            self.rx_cnt3 = 0
            self.pre_counter3 = self.client.object_counters_get(self.hostif2)
            #self.TTLTest()
            self.GleanTest()
            self.IsisTest()
            self.ArpTest()
            self.OspfTest()
            self.IgmpTest()
            self.PimTest()
            self.EapolTest()
            self.StpTest()
            self.PvrstTest()
            self.UdldTest()
            self.DhcpTest()
            self.BgpTest()
            self.IccpTest()
            self.IcmpTest()
            self.ICMPv6Test()
            self.IPv6NDTest()
            self.BfdRxTest()
            self.BfdV6RxTest()
            self.LacpTest()
            self.LLDPTest()
            self.DHCPv6Test()
            self.gnmiTest()
            self.p4rtTest()
            self.ntpclientTest()
            self.ntpserverTest()
            print('Deleting hostif trap types: Arp Request/Resp, OSPF, PIM, IGMP, STP, UDLD, DHCP, BGP, BGPV6, ICMPV6, DHCPV6')
            self.cleanupHostIfTrapList(self.trap_list)
            self.NoArpTest()
            self.NoOspfTest()
            self.NoIgmpTest()
            self.NoPimTest()
            self.NoEapolTest()
            self.NoStpTest()
            self.NoPvrstTest()
            self.NoUdldTest()
            self.NoDhcpTest()
            self.NoIccpTest()
            self.NoIcmpTest()
            self.NoICMPv6Test()
            self.NoIPv6NDTest()
            self.NoBfdRxTest()
            self.NoBfdV6RxTest()
            self.NoLacpTest()
            self.NoLLDPTest()
            self.NoDHCPv6Test()
            self.NognmiTest()
            self.Nop4rtTest()
            self.NontpclientTest()
            self.NontpclientTest()
            # self.HostifStatsTest()
        finally:
            pass

    def tearDown(self):
        self.attribute_set(self.port4, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.port5, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.cleanup()

    def cleanupHostIfTrapList(self, trap_list):
        for trap in range(len(trap_list)):
            self.cleanlast()


    def TTLTest(self):
        try:
            print("Sending normal packet with ttl 64, routed")
            pkt = simple_tcp_packet(eth_dst=self.rmac,
                                    ip_dst='20.10.10.1',
                                    ip_ttl=64)
            exp_pkt = simple_tcp_packet(eth_src=self.rmac,
                eth_dst='00:11:22:33:44:55',
                ip_dst='20.10.10.1',
                ip_ttl=63)
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt, self.devports[1])
            print("Sending packet with ttl 0, dropped")
            pkt = simple_tcp_packet(eth_dst=self.rmac,
                                    ip_dst='20.10.10.1',
                                    ip_ttl=0)
            send_packet(self, self.devports[0], pkt)
            verify_no_other_packets(self, timeout=1)
            print("Sending packet with ttl 1, routed and dropped")
            pkt = simple_tcp_packet(eth_dst=self.rmac,
                                    ip_dst='20.10.10.1',
                                    ip_ttl=1)
            send_packet(self, self.devports[0], pkt)
            verify_no_other_packets(self, timeout=1)
        finally:
            pass

    def BgpTest(self):
        try:
            dst_ip = '10.10.10.1'
            pkt = simple_tcp_packet(eth_dst=self.rmac,
                                    ip_dst=dst_ip,
                                    tcp_dport=179)
            exp_pkt = simple_cpu_packet(
                packet_type=0,
                ingress_port=self.devports[0],
                ingress_ifindex=self.port0_ingress_ifindex,
                reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_BGP,
                ingress_bd=2,
                inner_pkt=pkt)
            self.exp_bgpv4_pkt = cpu_packet_mask_ingress_bd(exp_pkt)

            print('Sending BGP v4 dst_port=179 packet')
            send_packet(self, self.devports[0], pkt)
            if self.test_params['target'] != 'hw':
                verify_packet(self, self.exp_bgpv4_pkt, self.cpu_port)
                self.assertTrue(socket_verify_packet(pkt, self.sock))
            self.rx_cnt += 1

            dst_ip = '2000::3'
            pkt = simple_tcpv6_packet(eth_dst=self.rmac,
                                      ipv6_dst=dst_ip,
                                      tcp_dport=179)
            exp_pkt = simple_cpu_packet(
                packet_type=0,
                ingress_port=self.devports[0],
                ingress_ifindex=self.port0_ingress_ifindex,
                reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_BGPV6,
                ingress_bd=2,
                inner_pkt=pkt)
            self.exp_bgpv6_pkt = cpu_packet_mask_ingress_bd(exp_pkt)

            print('Sending BGP v6 dst_port=179 packet')
            send_packet(self, self.devports[0], pkt)
            if self.test_params['target'] != 'hw':
                verify_packet(self, self.exp_bgpv6_pkt, self.cpu_port)
                self.assertTrue(socket_verify_packet(pkt, self.sock))
            self.rx_cnt += 1

            dst_ip = '10.10.10.1'
            pkt = simple_tcp_packet(eth_dst=self.rmac,
                                    ip_dst=dst_ip,
                                    tcp_sport=179)
            exp_pkt = simple_cpu_packet(
                packet_type=0,
                ingress_port=self.devports[0],
                ingress_ifindex=self.port0_ingress_ifindex,
                reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_BGP,
                ingress_bd=2,
                inner_pkt=pkt)
            self.exp_bgpv4_pkt = cpu_packet_mask_ingress_bd(exp_pkt)

            print('Sending BGP v4 src_port=179 packet')
            send_packet(self, self.devports[0], pkt)
            if self.test_params['target'] != 'hw':
                verify_packet(self, self.exp_bgpv4_pkt, self.cpu_port)
                self.assertTrue(socket_verify_packet(pkt, self.sock))
            self.rx_cnt += 1

            dst_ip = '2000::3'
            pkt = simple_tcpv6_packet(eth_dst=self.rmac,
                                      ipv6_dst=dst_ip,
                                      tcp_sport=179)
            exp_pkt = simple_cpu_packet(
                packet_type=0,
                ingress_port=self.devports[0],
                ingress_ifindex=self.port0_ingress_ifindex,
                reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_BGPV6,
                ingress_bd=2,
                inner_pkt=pkt)
            self.exp_bgpv6_pkt = cpu_packet_mask_ingress_bd(exp_pkt)

            print('Sending BGP v6 src_port=179 packet')
            send_packet(self, self.devports[0], pkt)
            if self.test_params['target'] != 'hw':
                verify_packet(self, self.exp_bgpv6_pkt, self.cpu_port)
                self.assertTrue(socket_verify_packet(pkt, self.sock))
            self.rx_cnt += 1
        finally:
            pass


    def GleanTest(self):
        try:
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='30.10.10.1',
                ip_ttl=64)
            cpu_pkt = simple_cpu_packet(
                packet_type=0,
                ingress_port=swport_to_devport(self, self.devports[0]),
                ingress_ifindex=(self.port0 & 0xFFFF),
                reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_MAX + SWITCH_HOSTIF_TRAP_ATTR_INTERNAL_TRAP_GLEAN,
                ingress_bd=0x0,
                inner_pkt=pkt)
            self.exp_glean_pkt = cpu_packet_mask_ingress_bd(cpu_pkt)

            print('Sending glean packet')
            send_packet(self, self.devports[0], pkt)
            if self.test_params['target'] != 'hw':
                verify_packet(self, self.exp_glean_pkt, self.cpu_port)
                self.assertTrue(socket_verify_packet(pkt, self.sock))
            self.rx_cnt += 1
        finally:
            pass

    def IsisTest(self):
        try:
            # ISIS_MAC : 0x09, 0x00, 0x2b, 0x00, 0x00, 0x05
            pkt = simple_eth_packet(eth_dst='09:00:2b:00:00:05', pktlen=100)
            exp_pkt = simple_cpu_packet(
                packet_type=0,
                ingress_port=self.devports[0],
                ingress_ifindex=self.port0_ingress_ifindex,
                reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_ISIS,
                ingress_bd=2,
                inner_pkt=pkt)
            self.exp_isis_pkt = cpu_packet_mask_ingress_bd(exp_pkt)

            print('Sending isis packet')
            send_packet(self, self.devports[0], pkt)
            if self.test_params['target'] != 'hw':
                verify_packet(self, self.exp_isis_pkt, self.cpu_port)
                self.assertTrue(socket_verify_packet(pkt, self.sock))
            self.rx_cnt += 1
        finally:
            pass

    def ArpTest(self):
        try:
            # Broadcast ARP Request
            pkt = simple_arp_packet(arp_op=1, pktlen=100)
            exp_pkt = simple_cpu_packet(
                  packet_type=0,
                  ingress_port=self.devports[0],
                  ingress_ifindex=self.port0_ingress_ifindex,
                  reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_ARP_REQUEST,
                  ingress_bd=2,
                  inner_pkt=pkt)
            self.exp_arpq_bc_pkt = cpu_packet_mask_ingress_bd(exp_pkt)

            print('Sending ARP request broadcast')
            send_packet(self, self.devports[0], pkt)
            if self.test_params['target'] != 'hw':
                verify_packet(self, self.exp_arpq_bc_pkt, self.cpu_port)
                self.assertTrue(socket_verify_packet(pkt, self.sock))
            self.rx_cnt += 1

            # Unicast ARP Request
            pkt = simple_arp_packet(arp_op=1, eth_dst=self.rmac, pktlen=100)
            exp_pkt = simple_cpu_packet(
                  packet_type=0,
                  ingress_port=self.devports[0],
                  ingress_ifindex=self.port0_ingress_ifindex,
                  reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_ARP_REQUEST,
                  ingress_bd=2,
                  inner_pkt=pkt)
            self.exp_arpq_uc_pkt = cpu_packet_mask_ingress_bd(exp_pkt)

            print('Sending unicast ARP request to router MAC')
            send_packet(self, self.devports[0], pkt)
            if self.test_params['target'] != 'hw':
                verify_packet(self, self.exp_arpq_uc_pkt, self.cpu_port)
                self.assertTrue(socket_verify_packet(pkt, self.sock))
            self.rx_cnt += 1

            # Unicast ARP Request to other MAC
            pkt = simple_arp_packet(arp_op=1, eth_dst='00:AA:BB:CC:DD:EE', pktlen=100)
            exp_pkt = simple_cpu_packet(
                  packet_type=0,
                  ingress_port=self.devports[0],
                  ingress_ifindex=self.port0_ingress_ifindex,
                  reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_ARP_REQUEST,
                  ingress_bd=2,
                  inner_pkt=pkt)
            unexp_arpq_uc_pkt = cpu_packet_mask_ingress_bd(exp_pkt)

            print('Sending unicast ARP request to other MAC')
            if self.test_params['target'] != 'hw':
                send_packet(self, self.devports[0], pkt)
                verify_no_other_packets(self, timeout=1)

            # Unicast ARP Response to Router MAC
            pkt = simple_arp_packet(arp_op=2, eth_dst=self.rmac, pktlen=100)
            exp_pkt = simple_cpu_packet(
                  packet_type=0,
                  ingress_port=self.devports[0],
                  ingress_ifindex=self.port0_ingress_ifindex,
                  reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_ARP_RESPONSE,
                  ingress_bd=2,
                  inner_pkt=pkt)
            exp_arpr_pkt = cpu_packet_mask_ingress_bd(exp_pkt)

            print('Sending unicast ARP response to router MAC')
            send_packet(self, self.devports[0], pkt)
            if self.test_params['target'] != 'hw':
                verify_packet(self, exp_arpr_pkt, self.cpu_port)
                self.assertTrue(socket_verify_packet(pkt, self.sock))
            self.rx_cnt += 1

            # Unicast ARP Response to other MAC
            pkt = simple_arp_packet(arp_op=2,
                                    eth_dst='00:11:22:33:44:55', pktlen=100)
            unexp_pkt = simple_cpu_packet(
                  packet_type=0,
                  ingress_port=self.devports[0],
                  ingress_ifindex=self.port0_ingress_ifindex,
                  reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_ARP_RESPONSE,
                  ingress_bd=2,
                  inner_pkt=pkt)
            unexp_arpr_uc_pkt = cpu_packet_mask_ingress_bd(exp_pkt)

            print('Sending unicast ARP response to other MAC')
            if self.test_params['target'] != 'hw':
                send_packet(self, self.devports[0], pkt)
                verify_no_other_packets(self, timeout=1)

            # Broadcast ARP Response
            pkt = simple_arp_packet(arp_op=2, pktlen=100)
            exp_pkt = simple_cpu_packet(
                  packet_type=0,
                  ingress_port=self.devports[0],
                  ingress_ifindex=self.port0_ingress_ifindex,
                  reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_ARP_RESPONSE,
                  ingress_bd=2,
                  inner_pkt=pkt)
            exp_arpr_bc_pkt = cpu_packet_mask_ingress_bd(exp_pkt)

            print('Sending ARP response broadcast')
            send_packet(self, self.devports[0], pkt)
            if self.test_params['target'] != 'hw':
                verify_packet(self, exp_arpr_bc_pkt, self.cpu_port)
                self.assertTrue(socket_verify_packet(pkt, self.sock))
            self.rx_cnt += 1
        finally:
            pass

    def NoArpTest(self):
        try:
            # Broadcast ARP Request
            pkt = simple_arp_packet(arp_op=1, pktlen=100)
            print('Sending ARP request broadcast')
            send_packet(self, self.devports[0], pkt)
            if self.test_params['target'] != 'hw':
                verify_no_packet(self, self.exp_arpq_bc_pkt, self.cpu_port, timeout=1)
        finally:
            pass

    def OspfTest(self):
        try:
            # OSPF Hello
            pkt = simple_ip_packet(ip_proto=89, ip_dst='224.0.0.5')
            exp_pkt = simple_cpu_packet(
                  packet_type=0,
                  ingress_port=self.devports[0],
                  ingress_ifindex=self.port0_ingress_ifindex,
                  reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_OSPF,
                  ingress_bd=2,
                  inner_pkt=pkt)
            self.exp_ospf1_pkt = cpu_packet_mask_ingress_bd(exp_pkt)

            print('Sending OSPF packet destined to 224.0.0.5')
            send_packet(self, self.devports[0], pkt)
            if self.test_params['target'] != 'hw':
                verify_packet(self, self.exp_ospf1_pkt, self.cpu_port)
                self.assertTrue(socket_verify_packet(pkt, self.sock))
            self.rx_cnt += 1

            # OSPF Designated Routers
            pkt = simple_ip_packet(ip_proto=89, ip_dst='224.0.0.6')
            exp_pkt = simple_cpu_packet(
                  packet_type=0,
                  ingress_port=self.devports[0],
                  ingress_ifindex=self.port0_ingress_ifindex,
                  reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_OSPF,
                  ingress_bd=2,
                  inner_pkt=pkt)
            self.exp_ospf2_pkt = cpu_packet_mask_ingress_bd(exp_pkt)

            print('Sending OSPF packet destined to 224.0.0.6')
            send_packet(self, self.devports[0], pkt)
            if self.test_params['target'] != 'hw':
                verify_packet(self, self.exp_ospf2_pkt, self.cpu_port)
                self.assertTrue(socket_verify_packet(pkt, self.sock))
            self.rx_cnt += 1
        finally:
            pass

    def NoOspfTest(self):
        try:
            # OSPF Hello
            pkt = simple_ip_packet(ip_proto=89, ip_dst='224.0.0.5')

            print('Sending OSPF packet destined to 224.0.0.5')
            send_packet(self, self.devports[0], pkt)
            if self.test_params['target'] != 'hw':
                verify_no_packet(self, self.exp_ospf1_pkt, self.cpu_port, timeout=1)

            # OSPF Designated Routers
            pkt = simple_ip_packet(ip_proto=89, ip_dst='224.0.0.6')
            print('Sending OSPF packet destined to 224.0.0.6')
            send_packet(self, self.devports[0], pkt)
            if self.test_params['target'] != 'hw':
                verify_no_packet(self, self.exp_ospf2_pkt, self.cpu_port, timeout=1)
        finally:
            pass

    def IgmpTest(self):
        if (self.client.is_feature_enable(SWITCH_FEATURE_MULTICAST) == 0):
            return

        igmp_info_list = [
            [SWITCH_HOSTIF_TRAP_ATTR_TYPE_IGMP_TYPE_QUERY, 0x11, "Query"],
            [SWITCH_HOSTIF_TRAP_ATTR_TYPE_IGMP_TYPE_LEAVE, 0x17, "Leave"],
            [SWITCH_HOSTIF_TRAP_ATTR_TYPE_IGMP_TYPE_V1_REPORT, 0x12, "V1 Report"],
            [SWITCH_HOSTIF_TRAP_ATTR_TYPE_IGMP_TYPE_V2_REPORT, 0x16, "V2 Report"],
            [SWITCH_HOSTIF_TRAP_ATTR_TYPE_IGMP_TYPE_V3_REPORT, 0x22, "V3 Report"]
        ]

        self.igmp_pkt_list = []

        try:
            print("\nEnabling IGMP snooping on VLAN")
            self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_IGMP_SNOOPING, True)

            for igmp_info in igmp_info_list:
                igmp_pkt = simple_igmp_packet(igmp_type=igmp_info[1])
                exp_pkt = simple_cpu_packet(
                    packet_type=0,
                    ingress_port=self.devports[4],
                    ingress_ifindex=self.port4_ingress_ifindex,
                    reason_code=igmp_info[0],
                    ingress_bd=2,
                    inner_pkt=igmp_pkt)
                exp_igmp_pkt = cpu_packet_mask_ingress_bd(exp_pkt)

                self.igmp_pkt_list.append((igmp_pkt, exp_igmp_pkt, igmp_info[2]))

                print('Sending IGMP {} packet'.format(igmp_info[2]))
                send_packet(self, self.devports[4], igmp_pkt)
                if self.test_params['target'] != 'hw':
                    verify_packet(self, exp_igmp_pkt, self.cpu_port)
                    self.assertTrue(socket_verify_packet(igmp_pkt, self.sock3))
                self.rx_cnt3 += 1

            print("\nDisabling IGMP snooping on VLAN")
            # Disable IGMP snooping on VLAN - packets should be forwarded
            self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_IGMP_SNOOPING, False)

            for (igmp_pkt, exp_igmp_pkt, pkt_type) in self.igmp_pkt_list:
                print('Sending IGMP {} packet'.format(pkt_type))
                send_packet(self, self.devports[4], igmp_pkt)
                verify_packet(self, igmp_pkt, self.devports[5])
                verify_no_other_packets(self)

        finally:
            self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_IGMP_SNOOPING, False)
            pass

    def NoIgmpTest(self):
        if (self.client.is_feature_enable(SWITCH_FEATURE_MULTICAST) == 0):
            return

        try:
            self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_IGMP_SNOOPING, True)

            # IGMP
            for (igmp_pkt, exp_igmp_pkt, pkt_type) in self.igmp_pkt_list:
                print('Sending IGMP {} packet'.format(pkt_type))
                send_packet(self, self.devports[4], igmp_pkt)
                verify_packet(self, igmp_pkt, self.devports[5])
                verify_no_other_packets(self)
        finally:
            self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_IGMP_SNOOPING, False)
            pass

    def PimTest(self):
        try:
            # PIM
            pkt = simple_ip_packet(ip_proto=103)
            exp_pkt = simple_cpu_packet(
                  packet_type=0,
                  ingress_port=self.devports[0],
                  ingress_ifindex=self.port0_ingress_ifindex,
                  reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_PIM,
                  ingress_bd=2,
                  inner_pkt=pkt)
            self.exp_pim_pkt = cpu_packet_mask_ingress_bd(exp_pkt)

            print('Sending PIM packet')
            send_packet(self, self.devports[0], pkt)
            if self.test_params['target'] != 'hw':
                verify_packet(self, self.exp_pim_pkt, self.cpu_port)
                self.assertTrue(socket_verify_packet(pkt, self.sock))
            self.rx_cnt += 1
        finally:
            pass

    def NoPimTest(self):
        try:
            # PIM
            print('Sending PIM packet')
            pkt = simple_ip_packet(ip_proto=103)
            send_packet(self, self.devports[0], pkt)
            if self.test_params['target'] != 'hw':
                verify_no_packet(self, self.exp_pim_pkt, self.cpu_port, timeout=1)
        finally:
            pass

    def EapolTest(self):
        try:
            # Eapol
            self.eap_pkt = simple_eth_packet(eth_type=0x888e)
            exp_pkt = simple_cpu_packet(
                  packet_type=0,
                  ingress_port=self.devports[0],
                  ingress_ifindex=self.port0_ingress_ifindex,
                  reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_EAPOL,
                  ingress_bd=2,
                  inner_pkt=self.eap_pkt)
            self.exp_eap_pkt = cpu_packet_mask_ingress_bd(exp_pkt)

            print('Sending EAP packet')
            send_packet(self, self.devports[0], self.eap_pkt)
            if self.test_params['target'] != 'hw':
                verify_packet(self, self.exp_eap_pkt, self.cpu_port)
                self.assertTrue(socket_verify_packet(self.eap_pkt, self.sock))
            self.rx_cnt += 1
        finally:
            pass

    def NoEapolTest(self):
        try:
            # Eapol
            print('Sending EAP packet')
            pkt = simple_ip_packet(ip_proto=103)
            send_packet(self, self.devports[0], self.eap_pkt)
            if self.test_params['target'] != 'hw':
                verify_no_other_packets(self, timeout=1)
        finally:
            pass

    def StpTest(self):
        try:
            # STP
            pkt = simple_eth_packet(eth_dst='01:80:C2:00:00:00', pktlen=100)
            exp_pkt = simple_cpu_packet(
                  packet_type=0,
                  ingress_port=self.devports[0],
                  ingress_ifindex=self.port0_ingress_ifindex,
                  reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_STP,
                  ingress_bd=2,
                  inner_pkt=pkt)
            self.exp_stp_pkt = cpu_packet_mask_ingress_bd(exp_pkt)

            print('Sending STP packet')
            send_packet(self, self.devports[0], pkt)
            if self.test_params['target'] != 'hw':
                verify_packet(self, self.exp_stp_pkt, self.cpu_port)
                self.assertTrue(socket_verify_packet(pkt, self.sock))
            self.rx_cnt += 1
        finally:
            pass

    def NoStpTest(self):
        try:
            # STP
            pkt = simple_eth_packet(eth_dst='01:80:C2:00:00:00', pktlen=100)
            send_packet(self, self.devports[0], pkt)
            if self.test_params['target'] != 'hw':
                verify_no_packet(self, self.exp_stp_pkt, self.cpu_port, timeout=1)
        finally:
            pass

    def PvrstTest(self):
        try:
            # PVRST
            pkt = simple_eth_packet(eth_dst='01:00:0C:CC:CC:CD', pktlen=100)
            exp_pkt = simple_cpu_packet(
                  packet_type=0,
                  ingress_port=self.devports[0],
                  ingress_ifindex=self.port0_ingress_ifindex,
                  reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_PVRST,
                  ingress_bd=2,
                  inner_pkt=pkt)
            self.exp_pvrst_pkt = cpu_packet_mask_ingress_bd(exp_pkt)

            print('Sending PVRST packet')
            send_packet(self, self.devports[0], pkt)
            if self.test_params['target'] != 'hw':
                verify_packet(self, self.exp_pvrst_pkt, self.cpu_port)
                self.assertTrue(socket_verify_packet(pkt, self.sock))
            self.rx_cnt += 1
        finally:
            pass

    def NoPvrstTest(self):
        try:
            # PVRST
            pkt = simple_eth_packet(eth_dst='01:00:0C:CC:CC:CD', pktlen=100)
            send_packet(self, self.devports[0], pkt)
            if self.test_params['target'] != 'hw':
                verify_no_packet(self, self.exp_pvrst_pkt, self.cpu_port, timeout=1)
        finally:
            pass

    def UdldTest(self):
        try:
            # UDLD
            pkt = simple_eth_packet(eth_dst='01:00:0C:CC:CC:CC', pktlen=100)
            exp_pkt = simple_cpu_packet(
                  packet_type=0,
                  ingress_port=self.devports[0],
                  ingress_ifindex=self.port0_ingress_ifindex,
                  reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_UDLD,
                  ingress_bd=2,
                  inner_pkt=pkt)
            self.exp_udld_pkt = cpu_packet_mask_ingress_bd(exp_pkt)
            print('Sending UDLD packet')
            send_packet(self, self.devports[0], pkt)
            if self.test_params['target'] != 'hw':
                verify_packet(self, self.exp_udld_pkt, self.cpu_port)
                self.assertTrue(socket_verify_packet(pkt, self.sock))
            self.rx_cnt += 1
        finally:
            pass

    def NoUdldTest(self):
        try:
            # UDLD
            print('Sending UDLD packet')
            pkt = simple_eth_packet(eth_dst='01:00:0C:CC:CC:CC', pktlen=100)
            send_packet(self, self.devports[0], pkt)
            if self.test_params['target'] != 'hw':
                verify_no_packet(self, self.exp_udld_pkt, self.cpu_port, timeout=1)
        finally:
            pass

    def NoDhcpTest(self):
        try:
            # DHCP request
            pkt = simple_udp_packet(
                  ip_dst='255.255.255.255', udp_sport=67, udp_dport=68)
            exp_pkt = ptf.mask.Mask(simple_cpu_packet(
                  packet_type=0,
                  ingress_port=self.devports[0],
                  ingress_ifindex=self.port0_ingress_ifindex,
                  reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_DHCP,
                  inner_pkt=pkt))
            mask_set_do_not_care_packet(exp_pkt, FabricCpuHeader, "ingress_bd")

            print('Sending DHCP request packet')
            send_packet(self, self.devports[0], pkt)
            if self.test_params['target'] != 'hw':
                verify_no_packet(self, exp_pkt, self.cpu_port, timeout=1)

            # DHCP Ack
            pkt = simple_udp_packet(eth_dst=self.rmac, ip_dst='10.10.10.1',
                                    udp_sport=68, udp_dport=67)
            exp_pkt = ptf.mask.Mask(simple_cpu_packet(
                  packet_type=0,
                  ingress_port=self.devports[0],
                  ingress_ifindex=self.port0_ingress_ifindex,
                  reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_DHCP,
                  inner_pkt=pkt))
            mask_set_do_not_care_packet(exp_pkt, FabricCpuHeader, "ingress_bd")


            print('Sending DHCP ack packet')
            send_packet(self, self.devports[0], pkt)
            if self.test_params['target'] != 'hw':
                verify_no_packet(self, exp_pkt, self.cpu_port, timeout=1)
        finally:
            pass

    def DhcpTest(self):
        try:
            # DHCP request
            pkt = simple_udp_packet(
                  ip_dst='255.255.255.255', udp_sport=67, udp_dport=68)
            exp_pkt = ptf.mask.Mask(simple_cpu_packet(
                  packet_type=0,
                  ingress_port=self.devports[0],
                  ingress_ifindex=self.port0_ingress_ifindex,
                  reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_DHCP,
                  inner_pkt=pkt))
            mask_set_do_not_care_packet(exp_pkt, FabricCpuHeader, "ingress_bd")

            print('Sending DHCP request packet')
            send_packet(self, self.devports[0], pkt)
            if self.test_params['target'] != 'hw':
                verify_packet(self, exp_pkt, self.cpu_port)
                self.assertTrue(socket_verify_packet(pkt, self.sock))
            self.rx_cnt += 1

            # DHCP Ack
            pkt = simple_udp_packet(eth_dst=self.rmac, ip_dst='10.10.10.1',
                                    udp_sport=68, udp_dport=67)
            exp_pkt = ptf.mask.Mask(simple_cpu_packet(
                  packet_type=0,
                  ingress_port=self.devports[0],
                  ingress_ifindex=self.port0_ingress_ifindex,
                  reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_DHCP,
                  inner_pkt=pkt))
            mask_set_do_not_care_packet(exp_pkt, FabricCpuHeader, "ingress_bd")

            print('Sending DHCP ack packet')
            send_packet(self, self.devports[0], pkt)
            if self.test_params['target'] != 'hw':
                verify_packet(self, exp_pkt, self.cpu_port)
                self.assertTrue(socket_verify_packet(pkt, self.sock))
            self.rx_cnt += 1
        finally:
            pass

    def IccpTest(self):
        try:
            dst_ip = '10.10.10.1'
            self.iccp_pkt = simple_tcp_packet(eth_dst=self.rmac,
                                    ip_dst=dst_ip,
                                    tcp_dport=8888)
            exp_pkt = simple_cpu_packet(
                packet_type=0,
                ingress_port=self.devports[0],
                ingress_ifindex=self.port0_ingress_ifindex,
                reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_ICCP,
                ingress_bd=2,
                inner_pkt=self.iccp_pkt)
            self.exp_iccp_pkt = cpu_packet_mask_ingress_bd(exp_pkt)

            print('Sending ICCP v4 dst_port=8888 packet')
            send_packet(self, self.devports[0], self.iccp_pkt)
            if self.test_params['target'] != 'hw':
                verify_packet(self, self.exp_iccp_pkt, self.cpu_port)
                self.assertTrue(socket_verify_packet(self.iccp_pkt, self.sock))
            self.rx_cnt += 1
        finally:
            pass

    def NoIccpTest(self):
        try:
            print('Sending ICCP v4 dst_port=8888 packet')
            send_packet(self, self.devports[0], self.iccp_pkt)
            if self.test_params['target'] != 'hw':
                verify_no_packet(self, self.exp_iccp_pkt, self.cpu_port, timeout=1)
        finally:
            pass

    def IcmpTest(self):
        try:
            self.icmp_pkt = simple_icmp_packet(
                        eth_dst=self.rmac,
                        ip_dst='10.10.10.1',
                        icmp_type=8,
                        icmp_data='000102030405')
            exp_pkt = simple_cpu_packet(
                packet_type=0,
                ingress_port=self.devports[0],
                ingress_ifindex=self.port0_ingress_ifindex,
                reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_ICMP,
                ingress_bd=1,
                inner_pkt=self.icmp_pkt)
            self.exp_icmp_pkt = cpu_packet_mask_ingress_bd(exp_pkt)
            print('Sending ICMP packet for echo request')
            send_packet(self, self.devports[0], self.icmp_pkt)
            if self.test_params['target'] != 'hw':
                verify_packet(self, self.exp_icmp_pkt, self.cpu_port)
                self.assertTrue(socket_verify_packet(self.icmp_pkt, self.sock))
            self.rx_cnt += 1
        finally:
            pass

    def NoIcmpTest(self):
        try:
            print('Sending ICMP packet for echo request, dropped')
            send_packet(self, self.devports[0], self.icmp_pkt)
            if self.test_params['target'] != 'hw':
                verify_no_packet(self, self.exp_icmp_pkt, self.cpu_port, timeout=1)
        finally:
            pass

    def ICMPv6Test(self):
        try:
            self.icmp6_er_pkt = simple_icmpv6_packet(
                eth_dst='00:77:66:55:44:33',
                ipv6_dst='ff02::2',
                ipv6_src='2000::1',
                icmp_type=128, # echo request
                ipv6_hlim=64)
            exp_pkt = simple_cpu_packet(
                packet_type=0,
                ingress_port=self.devports[0],
                ingress_ifindex=self.port0_ingress_ifindex,
                reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_ICMPV6,
                ingress_bd=1,
                inner_pkt=self.icmp6_er_pkt)
            self.exp_icmp6_er_pkt = cpu_packet_mask_ingress_bd(exp_pkt)
            print('Sending ICMPv6 packet for echo request')
            send_packet(self, self.devports[0], self.icmp6_er_pkt)
            if self.test_params['target'] != 'hw':
                verify_packet(self, self.exp_icmp6_er_pkt, self.cpu_port)
                self.assertTrue(socket_verify_packet(self.icmp6_er_pkt, self.sock))
            self.rx_cnt += 1
        finally:
            pass

    def NoICMPv6Test(self):
        try:
            print('Sending ICMPv6 packet for echo request, dropped')
            send_packet(self, self.devports[0], self.icmp6_er_pkt)
            if self.test_params['target'] != 'hw':
                verify_no_packet(self, self.exp_icmp6_er_pkt, self.cpu_port, timeout=1)
        finally:
            pass

    def IPv6NDTest(self):
        try:
            self.icmp6_rs_pkt = simple_icmpv6_packet(
                eth_dst='00:77:66:55:44:33',
                ipv6_dst='ff02::2',
                ipv6_src='2000::1',
                icmp_type=133,
                ipv6_hlim=64)
            exp_pkt = simple_cpu_packet(
                packet_type=0,
                ingress_port=self.devports[0],
                ingress_ifindex=self.port0_ingress_ifindex,
                reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_IPV6_NEIGHBOR_DISCOVERY,
                ingress_bd=1,
                inner_pkt=self.icmp6_rs_pkt)
            self.exp_icmp6_rs_pkt = cpu_packet_mask_ingress_bd(exp_pkt)
            print('Sending ICMPv6 packet for router solicitation')
            send_packet(self, self.devports[0], self.icmp6_rs_pkt)
            if self.test_params['target'] != 'hw':
                verify_packet(self, self.exp_icmp6_rs_pkt, self.cpu_port)
                self.assertTrue(socket_verify_packet(self.icmp6_rs_pkt, self.sock))
            self.rx_cnt += 1

            self.icmp6_ra_pkt = simple_icmpv6_packet(
                eth_dst='00:77:66:55:44:33',
                ipv6_dst='ff02::1',
                ipv6_src='2000::1',
                icmp_type=134,
                ipv6_hlim=64)
            exp_pkt = simple_cpu_packet(
                packet_type=0,
                ingress_port=self.devports[0],
                ingress_ifindex=self.port0_ingress_ifindex,
                reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_IPV6_NEIGHBOR_DISCOVERY,
                ingress_bd=1,
                inner_pkt=self.icmp6_ra_pkt)
            self.exp_icmp6_ra_pkt = cpu_packet_mask_ingress_bd(exp_pkt)
            print('Sending ICMPv6 packet for router advertisement')
            send_packet(self, self.devports[0], self.icmp6_ra_pkt)
            if self.test_params['target'] != 'hw':
                verify_packet(self, self.exp_icmp6_ra_pkt, self.cpu_port)
                self.assertTrue(socket_verify_packet(self.icmp6_ra_pkt, self.sock))
            self.rx_cnt += 1

            self.icmp6_ns_pkt = simple_icmpv6_packet(
                eth_dst='00:77:66:55:44:33',
                ipv6_dst='ff02::1:ff11:777F',
                ipv6_src='2000::1',
                icmp_type=135,
                ipv6_hlim=64)
            exp_pkt = simple_cpu_packet(
                packet_type=0,
                ingress_port=self.devports[0],
                ingress_ifindex=self.port0_ingress_ifindex,
                reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_IPV6_NEIGHBOR_DISCOVERY,
                ingress_bd=1,
                inner_pkt=self.icmp6_ns_pkt)
            self.exp_icmp6_ns_pkt = cpu_packet_mask_ingress_bd(exp_pkt)
            print('Sending ICMPv6 packet for neighbor solicitation')
            send_packet(self, self.devports[0], self.icmp6_ns_pkt)
            if self.test_params['target'] != 'hw':
                verify_packet(self, self.exp_icmp6_ns_pkt, self.cpu_port)
                self.assertTrue(socket_verify_packet(self.icmp6_ns_pkt, self.sock))
            self.rx_cnt += 1

            self.icmp6_na_pkt = simple_icmpv6_packet(
                eth_dst='00:77:66:55:44:33',
                ipv6_dst='1234::3',
                ipv6_src='2000::1',
                icmp_type=136,
                ipv6_hlim=64)
            exp_pkt = simple_cpu_packet(
                packet_type=0,
                ingress_port=self.devports[0],
                ingress_ifindex=self.port0_ingress_ifindex,
                reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_IPV6_NEIGHBOR_DISCOVERY,
                ingress_bd=1,
                inner_pkt=self.icmp6_na_pkt)
            self.exp_icmp6_na_pkt = cpu_packet_mask_ingress_bd(exp_pkt)
            print('Sending ICMPv6 packet for neighbor advertisement')
            send_packet(self, self.devports[0], self.icmp6_na_pkt)
            if self.test_params['target'] != 'hw':
                verify_packet(self, self.exp_icmp6_na_pkt, self.cpu_port)
                self.assertTrue(socket_verify_packet(self.icmp6_na_pkt, self.sock))
            self.rx_cnt += 1

            self.icmp6_redir_pkt = simple_icmpv6_packet(
                eth_dst='00:77:66:55:44:33',
                ipv6_dst='1234::3',
                ipv6_src='2000::1',
                icmp_type=137,
                ipv6_hlim=64)
            exp_pkt = simple_cpu_packet(
                packet_type=0,
                ingress_port=self.devports[0],
                ingress_ifindex=self.port0_ingress_ifindex,
                reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_IPV6_NEIGHBOR_DISCOVERY,
                ingress_bd=1,
                inner_pkt=self.icmp6_redir_pkt)
            self.exp_icmp6_redir_pkt = cpu_packet_mask_ingress_bd(exp_pkt)
            print('Sending ICMPv6 packet for redirect')
            send_packet(self, self.devports[0], self.icmp6_redir_pkt)
            if self.test_params['target'] != 'hw':
                verify_packet(self, self.exp_icmp6_redir_pkt, self.cpu_port)
                self.assertTrue(socket_verify_packet(self.icmp6_redir_pkt, self.sock))
            self.rx_cnt += 1
        finally:
            pass

    def NoIPv6NDTest(self):
        try:
            print('Sending ICMPv6 packet for router solicitation, dropped')
            send_packet(self, self.devports[0], self.icmp6_rs_pkt)
            if self.test_params['target'] != 'hw':
                verify_no_packet(self, self.exp_icmp6_rs_pkt, self.cpu_port, timeout=1)

            print('Sending ICMPv6 packet for router advertisement, dropped')
            send_packet(self, self.devports[0], self.icmp6_ra_pkt)
            if self.test_params['target'] != 'hw':
                verify_no_packet(self, self.exp_icmp6_ra_pkt, self.cpu_port, timeout=1)

            print('Sending ICMPv6 packet for neighbor solicitation, dropped')
            send_packet(self, self.devports[0], self.icmp6_ns_pkt)
            if self.test_params['target'] != 'hw':
                verify_no_packet(self, self.exp_icmp6_ns_pkt, self.cpu_port, timeout=1)

            print('Sending ICMPv6 packet for neighbor advertisement, dropped')
            send_packet(self, self.devports[0], self.icmp6_na_pkt)
            if self.test_params['target'] != 'hw':
                verify_no_packet(self, self.exp_icmp6_na_pkt, self.cpu_port, timeout=1)

            print('Sending ICMPv6 packet for redirect, dropped')
            send_packet(self, self.devports[0], self.icmp6_redir_pkt)
            if self.test_params['target'] != 'hw':
                verify_no_packet(self, self.exp_icmp6_redir_pkt, self.cpu_port, timeout=1)
        finally:
            pass

    def BfdRxTest(self):
        try:
            dst_ip = '10.10.10.1'
            bfd_udp_dst_port  = 3784

            #  pkt
            self.bfd_pkt = simple_udp_packet(
                eth_dst=self.rmac,
                ip_dst=dst_ip,
                udp_dport=bfd_udp_dst_port)
            exp_bfd_pkt_pkt = simple_cpu_packet(
                packet_type=0,
                ingress_port=self.devports[0],
                ingress_ifindex=self.port0_ingress_ifindex,
                reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_BFD,
                ingress_bd=2,
                inner_pkt=self.bfd_pkt)
            self.exp_bfd_pkt_pkt = cpu_packet_mask_ingress_bd(exp_bfd_pkt_pkt)

            print('Sending BFD v4 dst_port=3784 packet')
            send_packet(self, self.devports[0], self.bfd_pkt)
            if self.test_params['target'] != 'hw':
                verify_packet(self, self.exp_bfd_pkt_pkt, self.cpu_port)
                self.assertTrue(socket_verify_packet(self.bfd_pkt, self.sock))
            self.rx_cnt += 1
        finally:
            pass

    def NoBfdRxTest(self):
        try:
            print('Sending BFD v4 dst_port=3784 packet')
            send_packet(self, self.devports[0], self.bfd_pkt)
            if self.test_params['target'] != 'hw':
                verify_no_other_packets(self, timeout=1)
        finally:
            pass

    def gnmiTest(self):
        try:
            dst_ip = '10.10.10.1'
            gnmi_tcp_dst_port  = 9339

            #  pkt
            self.gnmi_pkt = simple_tcp_packet(
                eth_dst=self.rmac,
                ip_dst=dst_ip,
                tcp_dport=gnmi_tcp_dst_port)
            exp_gnmi_pkt_pkt = simple_cpu_packet(
                packet_type=0,
                ingress_port=self.devports[0],
                ingress_ifindex=self.port0_ingress_ifindex,
                reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_GNMI,
                ingress_bd=2,
                inner_pkt=self.gnmi_pkt)
            self.exp_gnmi_pkt_pkt = cpu_packet_mask_ingress_bd(exp_gnmi_pkt_pkt)

            print('Sending GNMI dst_port=9339 packet')
            send_packet(self, self.devports[0], self.gnmi_pkt)
            if self.test_params['target'] != 'hw':
                verify_packet(self, self.exp_gnmi_pkt_pkt, self.cpu_port)
                self.assertTrue(socket_verify_packet(self.gnmi_pkt, self.sock))
            self.rx_cnt += 1
        finally:
            pass

    def NognmiTest(self):
        try:
            print('Sending GNMI dst_port=9339 packet')
            send_packet(self, self.devports[0], self.gnmi_pkt)
            if self.test_params['target'] != 'hw':
                verify_no_other_packets(self, timeout=1)
        finally:
            pass

    def p4rtTest(self):
        try:
            dst_ip = '10.10.10.1'
            p4rt_tcp_dst_port  = 9559

            #  pkt
            self.p4rt_pkt = simple_tcp_packet(
                eth_dst=self.rmac,
                ip_dst=dst_ip,
                tcp_dport=p4rt_tcp_dst_port)
            exp_p4rt_pkt_pkt = simple_cpu_packet(
                packet_type=0,
                ingress_port=self.devports[0],
                ingress_ifindex=self.port0_ingress_ifindex,
                reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_P4RT,
                ingress_bd=2,
                inner_pkt=self.p4rt_pkt)
            self.exp_p4rt_pkt_pkt = cpu_packet_mask_ingress_bd(exp_p4rt_pkt_pkt)

            print('Sending p4rt dst_port=9339 packet')
            send_packet(self, self.devports[0], self.p4rt_pkt)
            if self.test_params['target'] != 'hw':
                verify_packet(self, self.exp_p4rt_pkt_pkt, self.cpu_port)
                self.assertTrue(socket_verify_packet(self.p4rt_pkt, self.sock))
            self.rx_cnt += 1
        finally:
            pass

    def Nop4rtTest(self):
        try:
            print('Sending p4rt dst_port=9339 packet')
            send_packet(self, self.devports[0], self.p4rt_pkt)
            if self.test_params['target'] != 'hw':
                verify_no_other_packets(self, timeout=1)
        finally:
            pass

    def ntpclientTest(self):
        try:
            dst_ip = '10.10.10.1'
            ntpclient_src_port  = 123

            #  tcp pkt
            self.ntpclient_tcp_pkt = simple_tcp_packet(
                eth_dst=self.rmac,
                ip_dst=dst_ip,
                tcp_sport=ntpclient_src_port)
            exp_ntpclient_pkt_pkt = simple_cpu_packet(
                packet_type=0,
                ingress_port=self.devports[0],
                ingress_ifindex=self.port0_ingress_ifindex,
                reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_NTPCLIENT,
                ingress_bd=2,
                inner_pkt=self.ntpclient_tcp_pkt)
            self.exp_ntpclient_tcp_pkt_pkt = cpu_packet_mask_ingress_bd(exp_ntpclient_pkt_pkt)

            print('Sending ntpclient src_port=123 packet')
            send_packet(self, self.devports[0], self.ntpclient_tcp_pkt)
            if self.test_params['target'] != 'hw':
                verify_packet(self, self.exp_ntpclient_tcp_pkt_pkt, self.cpu_port)
                self.assertTrue(socket_verify_packet(self.ntpclient_tcp_pkt, self.sock))
            self.rx_cnt += 1

            #  udp pkt
            self.ntpclient_udp_pkt = simple_udp_packet(
                eth_dst=self.rmac,
                ip_dst=dst_ip,
                udp_sport=ntpclient_src_port)
            exp_ntpclient_pkt_pkt = simple_cpu_packet(
                packet_type=0,
                ingress_port=self.devports[0],
                ingress_ifindex=self.port0_ingress_ifindex,
                reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_NTPCLIENT,
                ingress_bd=2,
                inner_pkt=self.ntpclient_udp_pkt)
            self.exp_ntpclient_tcp_pkt_pkt = cpu_packet_mask_ingress_bd(exp_ntpclient_pkt_pkt)

            print('Sending ntpclient src_port=123 packet')
            send_packet(self, self.devports[0], self.ntpclient_udp_pkt)
            if self.test_params['target'] != 'hw':
                verify_packet(self, self.exp_ntpclient_tcp_pkt_pkt, self.cpu_port)
                self.assertTrue(socket_verify_packet(self.ntpclient_udp_pkt, self.sock))
            self.rx_cnt += 1
        finally:
            pass

    def NontpclientTest(self):
        try:
            print('Sending ntpclient udp src_port=123 packet')
            send_packet(self, self.devports[0], self.ntpclient_tcp_pkt)
            if self.test_params['target'] != 'hw':
                verify_no_other_packets(self, timeout=1)

            print('Sending ntpclient upd src_port=123 packet')
            send_packet(self, self.devports[0], self.ntpclient_udp_pkt)
            if self.test_params['target'] != 'hw':
                verify_no_other_packets(self, timeout=1)

        finally:
            pass

    def ntpserverTest(self):
        try:
            dst_ip = '10.10.10.1'
            ntpserver_dst_port  = 123

            #  tcp pkt
            self.ntpserver_tcp_pkt = simple_tcp_packet(
                eth_dst=self.rmac,
                ip_dst=dst_ip,
                tcp_dport=ntpserver_dst_port)
            exp_ntpserver_pkt_pkt = simple_cpu_packet(
                packet_type=0,
                ingress_port=self.devports[0],
                ingress_ifindex=self.port0_ingress_ifindex,
                reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_NTPSERVER,
                ingress_bd=2,
                inner_pkt=self.ntpserver_tcp_pkt)
            self.exp_ntpserver_tcp_pkt_pkt = cpu_packet_mask_ingress_bd(exp_ntpserver_pkt_pkt)

            print('Sending ntpserver dst_port=123 tcp packet')
            send_packet(self, self.devports[0], self.ntpserver_tcp_pkt)
            if self.test_params['target'] != 'hw':
                verify_packet(self, self.exp_ntpserver_tcp_pkt_pkt, self.cpu_port)
                self.assertTrue(socket_verify_packet(self.ntpserver_tcp_pkt, self.sock))
            self.rx_cnt += 1

            #  udp pkt
            self.ntpserver_udp_pkt = simple_udp_packet(
                eth_dst=self.rmac,
                ip_dst=dst_ip,
                udp_dport=ntpserver_dst_port)
            exp_ntpserver_pkt_pkt = simple_cpu_packet(
                packet_type=0,
                ingress_port=self.devports[0],
                ingress_ifindex=self.port0_ingress_ifindex,
                reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_NTPSERVER,
                ingress_bd=2,
                inner_pkt=self.ntpserver_udp_pkt)
            self.exp_ntpserver_tcp_pkt_pkt = cpu_packet_mask_ingress_bd(exp_ntpserver_pkt_pkt)

            print('Sending ntpserver dst_port=123 udp packet')
            send_packet(self, self.devports[0], self.ntpserver_udp_pkt)
            if self.test_params['target'] != 'hw':
                verify_packet(self, self.exp_ntpserver_tcp_pkt_pkt, self.cpu_port)
                self.assertTrue(socket_verify_packet(self.ntpserver_udp_pkt, self.sock))
            self.rx_cnt += 1
        finally:
            pass

    def NontpserverTest(self):
        try:
            print('Sending ntpserver udp dst_port=123 tcp packet')
            send_packet(self, self.devports[0], self.ntpserver_tcp_pkt)
            if self.test_params['target'] != 'hw':
                verify_no_other_packets(self, timeout=1)

            print('Sending ntpserver upd dst_port=123 udp packet')
            send_packet(self, self.devports[0], self.ntpserver_udp_pkt)
            if self.test_params['target'] != 'hw':
                verify_no_other_packets(self, timeout=1)

        finally:
            pass

    def BfdV6RxTest(self):
        try:
            dst_ipv6 = "2000::3"
            bfd_udp_dst_port  = 3784

            bfdv6_pkt = simple_udpv6_packet(eth_dst=self.rmac,
                                        ipv6_dst=dst_ipv6,
                                        udp_dport=bfd_udp_dst_port)

            exp_bfdv6_pkt_pkt = simple_cpu_packet(
                packet_type=0,
                ingress_port=self.devports[0],
                ingress_ifindex=self.port0_ingress_ifindex,
                reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_BFDV6,
                ingress_bd=2,
                inner_pkt=bfdv6_pkt)
            self.exp_bfdv6_pkt_pkt = cpu_packet_mask_ingress_bd(exp_bfdv6_pkt_pkt)

            print('Sending BFD v6 dst_port=3784 packet')
            send_packet(self, self.devports[0], bfdv6_pkt)
            if self.test_params['target'] != 'hw':
                verify_packet(self, self.exp_bfdv6_pkt_pkt, self.cpu_port)
                self.assertTrue(socket_verify_packet(bfdv6_pkt, self.sock))
            self.rx_cnt += 1

        finally:
            pass


    def NoBfdV6RxTest(self):
        try:
            dst_ipv6 = "2001:0db8::1:1"
            bfd_udp_dst_port  = 3784

            #  pkt
            bfdv6_pkt = simple_udpv6_packet(eth_dst=self.rmac,
                                        ipv6_dst=dst_ipv6,
                                        udp_dport=bfd_udp_dst_port)


            exp_bfdv6_pkt_pkt = simple_cpu_packet(
                packet_type=0,
                ingress_port=self.devports[0],
                ingress_ifindex=self.port0_ingress_ifindex,
                reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_BFDV6,
                ingress_bd=2,
                inner_pkt=bfdv6_pkt)
            self.exp_bfdv6_pkt_pkt = cpu_packet_mask_ingress_bd(exp_bfdv6_pkt_pkt)
            print('Sending BFD v6 dst_port=3784 packet')
            send_packet(self, self.devports[0], bfdv6_pkt)
            if self.test_params['target'] != 'hw':
                verify_no_packet(self, self.exp_bfdv6_pkt_pkt, self.cpu_port, timeout=2)
        finally:
            pass

    def LacpTest(self):
        try:
            # LACP
            pkt = simple_eth_packet(eth_dst='01:80:C2:00:00:02', pktlen=100)
            exp_pkt = simple_cpu_packet(
                  packet_type=0,
                  ingress_port=self.devports[3],
                  ingress_ifindex=self.lag0_ingress_ifindex,
                  reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_LACP,
                  ingress_bd=2,
                  inner_pkt=pkt)
            self.exp_lacp_pkt = cpu_packet_mask_ingress_bd(exp_pkt)

            print('Sending LACP packet')
            send_packet(self, self.devports[3], pkt)
            if self.test_params['target'] != 'hw':
                verify_packet(self, self.exp_lacp_pkt, self.cpu_port)
                self.assertTrue(socket_verify_packet(pkt, self.sock2))
            self.rx_cnt2 += 1
        finally:
            pass

    def NoLacpTest(self):
        try:
            # LACP
            pkt = simple_eth_packet(eth_dst='01:80:C2:00:00:02', pktlen=100)
            exp_pkt = simple_cpu_packet(
                  packet_type=0,
                  ingress_port=self.devports[3],
                  ingress_ifindex=self.lag0_ingress_ifindex,
                  reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_LACP,
                  ingress_bd=2,
                  inner_pkt=pkt)
            self.exp_lacp_pkt = cpu_packet_mask_ingress_bd(exp_pkt)
            print('Sending LACP packet')
            send_packet(self, self.devports[3], pkt)
            if self.test_params['target'] != 'hw':
                verify_no_packet(self, self.exp_lacp_pkt, self.cpu_port, timeout=1)
        finally:
            pass

    def LLDPTest(self):
        try:
            # LLDP , dmac1: {0x01, 0x80, 0xC2, 0x00, 0x00, 0x0e};
            pkt = simple_eth_packet(eth_dst='01:80:C2:00:00:0e', pktlen=100)
            exp_pkt = simple_cpu_packet(
                  packet_type=0,
                  ingress_port=self.devports[0],
                  ingress_ifindex=self.port0_ingress_ifindex,
                  reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_LLDP,
                  ingress_bd=2,
                  inner_pkt=pkt)
            self.exp_lldp_pkt = cpu_packet_mask_ingress_bd(exp_pkt)
            print('Sending LLDP packet')
            send_packet(self, self.devports[0], pkt)
            if self.test_params['target'] != 'hw':
                verify_packet(self, self.exp_lldp_pkt, self.cpu_port)
                self.assertTrue(socket_verify_packet(pkt, self.sock))
            self.rx_cnt += 1

            # LLDP , dmac2: {0x01, 0x80, 0xC2, 0x00, 0x00, 0x03};
            pkt = simple_eth_packet(eth_dst='01:80:C2:00:00:03', pktlen=100)
            exp_pkt = simple_cpu_packet(
                  packet_type=0,
                  ingress_port=self.devports[0],
                  ingress_ifindex=self.port0_ingress_ifindex,
                  reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_LLDP,
                  ingress_bd=2,
                  inner_pkt=pkt)
            self.exp_lldp_pkt = cpu_packet_mask_ingress_bd(exp_pkt)
            print('Sending LLDP packet')
            send_packet(self, self.devports[0], pkt)
            if self.test_params['target'] != 'hw':
                verify_packet(self, self.exp_lldp_pkt, self.cpu_port)
                self.assertTrue(socket_verify_packet(pkt, self.sock))
            self.rx_cnt += 1
        finally:
            pass


    def NoLLDPTest(self):
        try:
            # LLDP , dmac: {0x01, 0x80, 0xC2, 0x00, 0x00, 0x0e};
            pkt = simple_eth_packet(eth_dst='01:80:C2:00:00:0e', pktlen=100)
            exp_pkt = simple_cpu_packet(
                  packet_type=0,
                  ingress_port=self.devports[0],
                  ingress_ifindex=self.port0_ingress_ifindex,
                  reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_LLDP,
                  ingress_bd=2,
                  inner_pkt=pkt)
            self.exp_lldp_pkt = cpu_packet_mask_ingress_bd(exp_pkt)
            print('Sending LLDP packet')
            send_packet(self, self.devports[0], pkt)
            if self.test_params['target'] != 'hw':
                verify_no_packet(self, self.exp_lldp_pkt, self.cpu_port, timeout=1)

            # LLDP , dmac: {0x01, 0x80, 0xC2, 0x00, 0x00, 0x03};
            pkt = simple_eth_packet(eth_dst='01:80:C2:00:00:03', pktlen=100)
            exp_pkt = simple_cpu_packet(
                  packet_type=0,
                  ingress_port=self.devports[0],
                  ingress_ifindex=self.port0_ingress_ifindex,
                  reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_LLDP,
                  ingress_bd=2,
                  inner_pkt=pkt)
            self.exp_lldp_pkt = cpu_packet_mask_ingress_bd(exp_pkt)
            print('Sending LLDP packet')
            send_packet(self, self.devports[0], pkt)
            if self.test_params['target'] != 'hw':
                verify_no_packet(self, self.exp_lldp_pkt, self.cpu_port, timeout=1)
        finally:
            pass

    def DHCPv6Test(self):
        # DHCPv6
        dhcpv6_client_port = 546
        dhcpv6_srv_port = 547
        dhcpv6_pkt_params_list = [
            ["33:33:00:01:00:02", "FF02::1:2", dhcpv6_client_port,
             dhcpv6_srv_port, "all servers and relay agents"],
            ["33:33:00:01:00:05", "FF05::1:3", dhcpv6_client_port,
             dhcpv6_srv_port, "all servers"],
            [self.rmac, "2000::3", dhcpv6_client_port,
             dhcpv6_srv_port, "ip-to-me server"],
            [self.rmac, "2000::3", dhcpv6_srv_port,
             dhcpv6_client_port, "ip-to-me client"]
        ]

        self.dhcpv6_pkt_list = []

        try:
            for dhcpv6_pkt_params in dhcpv6_pkt_params_list:
                dhcpv6_pkt = simple_udpv6_packet(
                    eth_dst=dhcpv6_pkt_params[0],
                    ipv6_dst=dhcpv6_pkt_params[1],
                    udp_sport=dhcpv6_pkt_params[2],
                    udp_dport=dhcpv6_pkt_params[3])
                dhcpv6_cpu_pkt = simple_cpu_packet(
                    packet_type=0,
                    ingress_port=self.devports[0],
                    ingress_ifindex=self.port0_ingress_ifindex,
                    reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_DHCPV6,
                   inner_pkt=dhcpv6_pkt)
                exp_dhcpv6_pkt = cpu_packet_mask_ingress_bd_and_ifindex(
                    dhcpv6_cpu_pkt)

                self.dhcpv6_pkt_list.append((dhcpv6_pkt, exp_dhcpv6_pkt, dhcpv6_pkt_params[4]))

                print("Sending DHCPv6 \'{}\' packet". format(dhcpv6_pkt_params[4]))
                send_packet(self, self.devports[0], dhcpv6_pkt)
                if self.test_params['target'] != 'hw':
                    verify_packet(self, exp_dhcpv6_pkt, self.cpu_port)
                    self.assertTrue(socket_verify_packet(dhcpv6_pkt, self.sock))
                self.rx_cnt += 1

        finally:
            pass

    def NoDHCPv6Test(self):
        try:
            # DHCPv6
            for (dhcpv6_pkt, exp_dhcpv6_pkt, pkt_type) in self.dhcpv6_pkt_list:
                print('Sending DHCPv6 \'{}\' packet'.format(pkt_type))
                send_packet(self, self.devports[0], dhcpv6_pkt)
                if self.test_params['target'] != 'hw':
                    verify_no_packet(self, exp_dhcpv6_pkt, self.cpu_port, timeout=1)
        finally:
            pass

    def HostifStatsTest(self):
        if self.test_params['target'] == 'hw':
            return
        self.post_counter = self.client.object_counters_get(self.hostif0)
        idx = SWITCH_HOSTIF_COUNTER_ID_RX_PKT
        self.assertEqual(self.post_counter[idx].count - self.pre_counter[idx].count, self.rx_cnt)

        self.post_counter2 = self.client.object_counters_get(self.hostif1)
        idx = SWITCH_HOSTIF_COUNTER_ID_RX_PKT
        self.assertEqual(self.post_counter2[idx].count - self.pre_counter2[idx].count, self.rx_cnt2)

        self.post_counter3 = self.client.object_counters_get(self.hostif2)
        idx = SWITCH_HOSTIF_COUNTER_ID_RX_PKT
        self.assertEqual(self.post_counter3[idx].count - self.pre_counter3[idx].count, self.rx_cnt3)

###############################################################################

@group('l3')
class HostifSVIArpTest(ApiHelper):
    def setUp(self):
        print()
        self.configure()

        self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port0)
        self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port1)
        self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port2)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.attribute_set(self.port2, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_LEARNING, False)

        queue_handles = self.attribute_get(self.cpu_port_hdl, SWITCH_PORT_ATTR_QUEUE_HANDLES)
        hostif_trap_group0 = self.add_hostif_trap_group(self.device, queue_handle=queue_handles[0].oid, admin_state=True)
        self.add_hostif_trap(self.device, type=SWITCH_HOSTIF_TRAP_ATTR_TYPE_ARP_REQUEST,
                             hostif_trap_group_handle=hostif_trap_group0,
                             packet_action=SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_COPY_TO_CPU,
                             priority=100)

    def tearDown(self):
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.port2, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.cleanup()

    def runTest(self):
        arp = simple_arp_packet()
        cpu_pkt = simple_cpu_packet(
            ingress_port=swport_to_devport(self, self.devports[0]),
            ingress_ifindex=(self.port0 & 0xFFFF),
            packet_type=0,
            reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_ARP_REQUEST,
            ingress_bd=0xa,
            inner_pkt=arp)

        try:
            print("No RIF on vlan 10, no copy to cpu")
            send_packet(self, self.devports[0], arp)
            verify_packets(self, arp, [self.devports[1], self.devports[2]])

            print("Create RIF on vlan 10")
            self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_VLAN, vlan_handle=self.vlan10, vrf_handle=self.vrf10, src_mac=self.rmac)

            print("Now copy to cpu")
            send_packet(self, self.devports[0], arp)
            if self.test_params['target'] != 'hw':
                verify_each_packet_on_each_port(self, [arp, arp, cpu_pkt], [self.devports[1], self.devports[2], self.cpu_port])
            else:
                verify_packets(self, arp, [self.devports[1], self.devports[2]])

            if (self.client.is_feature_enable(SWITCH_FEATURE_STP) != 0):
                stp = self.add_stp(self.device)
                self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_STP_HANDLE, stp)
                print("Put port %d in stp forwarding state" % (self.devports[0]))
                self.stp_port0 = self.add_stp_port(self.device, state=SWITCH_STP_PORT_ATTR_STATE_FORWARDING, stp_handle=stp, port_lag_handle=self.port0)
                send_packet(self, self.devports[0], arp)
                if self.test_params['target'] != 'hw':
                    verify_each_packet_on_each_port(self, [arp, arp, cpu_pkt], [self.devports[1], self.devports[2], self.cpu_port])
                else:
                    verify_packets(self, arp, [self.devports[1], self.devports[2]])

                print("Put port %d in stp blocking state" % (self.devports[0]))
                self.attribute_set(self.stp_port0, SWITCH_STP_PORT_ATTR_STATE, SWITCH_STP_PORT_ATTR_STATE_BLOCKING)
                send_packet(self, self.devports[0], arp)
                verify_no_other_packets(self, timeout=1)

                print("Put port %d in stp forwarding state" % (self.devports[0]))
                self.attribute_set(self.stp_port0, SWITCH_STP_PORT_ATTR_STATE, SWITCH_STP_PORT_ATTR_STATE_FORWARDING)
                send_packet(self, self.devports[0], arp)
                if self.test_params['target'] != 'hw':
                    verify_each_packet_on_each_port(self, [arp, arp, cpu_pkt], [self.devports[1], self.devports[2], self.cpu_port])
                else:
                    verify_packets(self, arp, [self.devports[1], self.devports[2]])

                print("Clean stp config")
                self.cleanlast()
                self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_STP_HANDLE, 0)
                self.cleanlast()

            print("Delete RIF on vlan 10")
            self.cleanlast()
            send_packet(self, self.devports[0], arp)
            verify_packets(self, arp, [self.devports[1], self.devports[2]])
        finally:
            pass

###############################################################################
@group('hostif')
class HostIgmpNonRoutableTest(ApiHelper):
    def setUp(self):
        print()
        self.configure()

        self.rif0 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port0, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.nhop0 = self.add_nexthop(self.device, handle=self.rif0, dest_ip='30.30.30.1')
        self.neighbor0 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=self.rif0, dest_ip='30.30.30.1')
        self.route1 = self.add_route(self.device, ip_prefix='30.30.30.1', vrf_handle=self.vrf10, nexthop_handle=self.nhop0)

        self.nhop_glean = self.add_nexthop(self.device, type=SWITCH_NEXTHOP_ATTR_TYPE_GLEAN)
        self.route0 = self.add_route(self.device, ip_prefix='30.10.10.1', vrf_handle=self.vrf10, nexthop_handle=self.nhop_glean)

        self.port4_ingress_ifindex = self.get_port_ifindex(self.port4)
        vlan_mbr0 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port4)
        self.attribute_set(self.port4, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)

        vlan_mbr1 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port5)
        self.attribute_set(self.port5, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)


    def runTest(self):
        print()
        try:
            self.IgmpOnL3Test()
            self.IgmpOnL2Test()
        finally:
            pass

    def tearDown(self):
        self.attribute_set(self.port4, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.port5, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.cleanup()


    def IgmpOnL3Test(self, post_hitless=False):
        igmp_info_list = [
            [SWITCH_HOSTIF_TRAP_ATTR_TYPE_IGMP_TYPE_QUERY, 0x11, "Query"],
            [SWITCH_HOSTIF_TRAP_ATTR_TYPE_IGMP_TYPE_LEAVE, 0x17, "Leave"],
            [SWITCH_HOSTIF_TRAP_ATTR_TYPE_IGMP_TYPE_V1_REPORT, 0x12, "V1 Report"],
            [SWITCH_HOSTIF_TRAP_ATTR_TYPE_IGMP_TYPE_V2_REPORT, 0x16, "V2 Report"],
            [SWITCH_HOSTIF_TRAP_ATTR_TYPE_IGMP_TYPE_V3_REPORT, 0x22, "V3 Report"]
        ]
        self.igmp_pkt_list = []

        try:
            initial_stats = self.client.object_counters_get(self.port0)
            num_drops = 0

            self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_IGMP_SNOOPING, True)

            print('IGMP on pure L3 interface:')
            for igmp_info in igmp_info_list:
                igmp_pkt = simple_igmp_packet(igmp_type=igmp_info[1])
                print('Sending IGMP {} packet'.format(igmp_info[2]))
                send_packet(self, self.devports[0], igmp_pkt)
                verify_no_other_packets(self, timeout=1)
                num_drops += 1

            print("Sleeping for 5 sec before fetching stats")
            time.sleep(5)
            try:
                final_stats = self.client.object_counters_get(self.port0)

                x = SWITCH_PORT_COUNTER_ID_IF_IN_DISCARDS
                final_count = final_stats[x].count - initial_stats[x].count

                x = SWITCH_PORT_COUNTER_ID_IF_IN_NON_ROUTABLE_DISCARDS
                igmp_non_routable_drop = final_stats[x].count - initial_stats[x].count

                print("Expected drop count            : %d" % num_drops)
                print("Final drop count               : %d" % final_count)
                print("IGMP Non Routable drop count   : %d" % igmp_non_routable_drop)

                self.assertEqual(num_drops, final_count)
                self.assertEqual(5, igmp_non_routable_drop)

                if (post_hitless):
                    return final_stats[SWITCH_PORT_COUNTER_ID_IF_IN_DISCARDS].count
                else:
                    return final_count

            finally:
                pass


        finally:
            self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_IGMP_SNOOPING, False)
            pass

    def IgmpOnL2Test(self):
        igmp_info_list = [
            [SWITCH_HOSTIF_TRAP_ATTR_TYPE_IGMP_TYPE_QUERY, 0x11, "Query"],
            [SWITCH_HOSTIF_TRAP_ATTR_TYPE_IGMP_TYPE_LEAVE, 0x17, "Leave"],
            [SWITCH_HOSTIF_TRAP_ATTR_TYPE_IGMP_TYPE_V1_REPORT, 0x12, "V1 Report"],
            [SWITCH_HOSTIF_TRAP_ATTR_TYPE_IGMP_TYPE_V2_REPORT, 0x16, "V2 Report"],
            [SWITCH_HOSTIF_TRAP_ATTR_TYPE_IGMP_TYPE_V3_REPORT, 0x22, "V3 Report"]
        ]
        self.igmp_pkt_list = []

        try:
            self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_IGMP_SNOOPING, True)

            print('\nIGMP on pure L2 interface:')
            for igmp_info in igmp_info_list:
                igmp_pkt = simple_igmp_packet(igmp_type=igmp_info[1])
                print('Sending IGMP {} packet'.format(igmp_info[2]))
                send_packet(self, self.devports[4], igmp_pkt)
                verify_packet(self, igmp_pkt, self.devports[5])

        finally:
            self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_IGMP_SNOOPING, False)
            pass


###############################################################################
@group('hostif')
class HostIfRxVlanActionTest(ApiHelper):
    def setUp(self):
        print()
        self.configure()

        # create l3 interface and associate with self.vlan10
        self.rif0 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_VLAN, vlan_handle=self.vlan10, vrf_handle=self.vrf10, src_mac=self.rmac)
        vlan_mbr0 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port0)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.port0_ingress_ifindex = self.get_port_ifindex(self.port0)
        self.route0 = self.add_route(self.device, ip_prefix='10.10.10.1', vrf_handle=self.vrf10, is_host_myip=True)

        # create trap group.
        queue_handles = self.attribute_get(self.cpu_port_hdl, SWITCH_PORT_ATTR_QUEUE_HANDLES)
        self.hostif_trap_group0 = self.add_hostif_trap_group(self.device, queue_handle=queue_handles[0].oid, admin_state=True)

        # create hostif for port0
        self.hostif_name1 = "test_host_if0"
        self.hostif0 = self.add_hostif(self.device, name=self.hostif_name1, handle=self.port0, oper_status=True)
        self.assertTrue(self.hostif0 != 0)

        print("Sleeping for 5 secs")
        time.sleep(5)

        # create hostif_trap entry for BGP trap types.
        self.add_hostif_trap(self.device, type=SWITCH_HOSTIF_TRAP_ATTR_TYPE_BGP,
                             hostif_trap_group_handle=self.hostif_trap_group0,
                             packet_action=SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_REDIRECT_TO_CPU)

    def runTest(self):
        print()
        try:
            if self.test_params['target'] != 'hw':
                self.sock = open_packet_socket(self.hostif_name1)
            self.VlanStripTest()
            self.VlanKeepTest()
            self.VlanOriginalTest()
        finally:
            pass

    def tearDown(self):
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.cleanup()

    def VlanStripTest(self):
        try:
            self.attribute_set(self.hostif0, SWITCH_HOSTIF_ATTR_VLAN_ACTION, SWITCH_HOSTIF_ATTR_VLAN_ACTION_STRIP)
            dst_ip = '10.10.10.1'
            tagged_pkt = simple_tcp_packet(eth_dst=self.rmac,
                                    dl_vlan_enable=True,
                                    vlan_vid=10,
                                    ip_dst=dst_ip,
                                    tcp_dport=179,
                                    pktlen=104)
            pkt = simple_tcp_packet(eth_dst=self.rmac,
                                    ip_dst=dst_ip,
                                    tcp_dport=179,
                                    pktlen=100)
            exp_pkt = simple_cpu_packet(
                packet_type=0,
                ingress_port=self.devports[0],
                ingress_ifindex=self.port0_ingress_ifindex,
                reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_BGP,
                ingress_bd=2,
                inner_pkt=pkt)
            self.exp_bgpv4_pkt = cpu_packet_mask_ingress_bd(exp_pkt)

            print('Sending BGP v4 dst_port=179 packet')
            send_packet(self, self.devports[0], pkt)
            if self.test_params['target'] != 'hw':
                verify_packet(self, self.exp_bgpv4_pkt, self.cpu_port)
                self.assertTrue(socket_verify_packet(pkt, self.sock))

            exp_pkt = simple_cpu_packet(
                packet_type=0,
                ingress_port=self.devports[0],
                ingress_ifindex=self.port0_ingress_ifindex,
                reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_BGP,
                ingress_bd=2,
                inner_pkt=tagged_pkt)
            self.exp_bgpv4_pkt = cpu_packet_mask_ingress_bd(exp_pkt)

            print('Sending tagged BGP v4 dst_port=179 packet')
            send_packet(self, self.devports[0], tagged_pkt)
            if self.test_params['target'] != 'hw':
                verify_packet(self, self.exp_bgpv4_pkt, self.cpu_port)
                self.assertTrue(socket_verify_packet(pkt, self.sock))

        finally:
            pass

    def VlanKeepTest(self):
        try:
            self.attribute_set(self.hostif0, SWITCH_HOSTIF_ATTR_VLAN_ACTION, SWITCH_HOSTIF_ATTR_VLAN_ACTION_KEEP)
            dst_ip = '10.10.10.1'
            tagged_pkt = simple_tcp_packet(eth_dst=self.rmac,
                                    dl_vlan_enable=True,
                                    vlan_vid=10,
                                    ip_dst=dst_ip,
                                    tcp_dport=179,
                                    pktlen=104)
            pkt = simple_tcp_packet(eth_dst=self.rmac,
                                    ip_dst=dst_ip,
                                    tcp_dport=179,
                                    pktlen=100)
            exp_pkt = simple_cpu_packet(
                packet_type=0,
                ingress_port=self.devports[0],
                ingress_ifindex=self.port0_ingress_ifindex,
                reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_BGP,
                ingress_bd=2,
                inner_pkt=pkt)
            self.exp_bgpv4_pkt = cpu_packet_mask_ingress_bd(exp_pkt)

            print('Sending BGP v4 dst_port=179 packet')
            send_packet(self, self.devports[0], pkt)
            if self.test_params['target'] != 'hw':
                verify_packet(self, self.exp_bgpv4_pkt, self.cpu_port)
            # Uncomment when there is a way to receive tagged packet
            # which currently is stripped by linux
            #self.assertTrue(socket_verify_packet(tagged_pkt, self.sock))

            exp_pkt = simple_cpu_packet(
                packet_type=0,
                ingress_port=self.devports[0],
                ingress_ifindex=self.port0_ingress_ifindex,
                reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_BGP,
                ingress_bd=2,
                inner_pkt=tagged_pkt)
            self.exp_bgpv4_pkt = cpu_packet_mask_ingress_bd(exp_pkt)

            print('Sending tagged BGP v4 dst_port=179 packet')
            send_packet(self, self.devports[0], tagged_pkt)
            if self.test_params['target'] != 'hw':
                verify_packet(self, self.exp_bgpv4_pkt, self.cpu_port)
            # Uncomment when there is a way to receive tagged packet
            # which currently is stripped by linux
            #self.assertTrue(socket_verify_packet(tagged_pkt, self.sock))

        finally:
            pass

    def VlanOriginalTest(self):
        try:
            self.attribute_set(self.hostif0, SWITCH_HOSTIF_ATTR_VLAN_ACTION, SWITCH_HOSTIF_ATTR_VLAN_ACTION_ORIGINAL)
            dst_ip = '10.10.10.1'
            tagged_pkt = simple_tcp_packet(eth_dst=self.rmac,
                                    dl_vlan_enable=True,
                                    vlan_vid=10,
                                    ip_dst=dst_ip,
                                    tcp_dport=179,
                                    pktlen=104)
            pkt = simple_tcp_packet(eth_dst=self.rmac,
                                    ip_dst=dst_ip,
                                    tcp_dport=179,
                                    pktlen=100)
            exp_pkt = simple_cpu_packet(
                packet_type=0,
                ingress_port=self.devports[0],
                ingress_ifindex=self.port0_ingress_ifindex,
                reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_BGP,
                ingress_bd=2,
                inner_pkt=pkt)
            self.exp_bgpv4_pkt = cpu_packet_mask_ingress_bd(exp_pkt)

            print('Sending BGP v4 dst_port=179 packet')
            send_packet(self, self.devports[0], pkt)
            if self.test_params['target'] != 'hw':
                verify_packet(self, self.exp_bgpv4_pkt, self.cpu_port)
                self.assertTrue(socket_verify_packet(pkt, self.sock))

            exp_pkt = simple_cpu_packet(
                packet_type=0,
                ingress_port=self.devports[0],
                ingress_ifindex=self.port0_ingress_ifindex,
                reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_BGP,
                ingress_bd=2,
                inner_pkt=tagged_pkt)
            self.exp_bgpv4_pkt = cpu_packet_mask_ingress_bd(exp_pkt)

            print('Sending tagged BGP v4 dst_port=179 packet')
            send_packet(self, self.devports[0], tagged_pkt)
            if self.test_params['target'] != 'hw':
                verify_packet(self, self.exp_bgpv4_pkt, self.cpu_port)
            # Uncomment when there is a way to receive tagged packet
            # which currently is stripped by linux
            #self.assertTrue(socket_verify_packet(tagged_pkt, self.sock))

        finally:
            pass

###############################################################################
@group('hostif')
class HostLacpBpduTrapAddDeleteTest(ApiHelper):
    def setUp(self):
        print()
        self.configure()

        # create l3 interface and associate with self.vlan10
        vlan_mbr0 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port0)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.port0_ingress_ifindex = self.get_port_ifindex(self.port0)

        vlan_mbr1 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port1)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.port1_ingress_ifindex = self.get_port_ifindex(self.port1)

    def runTest(self):
        print()
        try:
            self.PacketActionUpdateTest()
        finally:
            pass

    def tearDown(self):
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.cleanup()

    def PacketActionUpdateTest(self):
        try:
            ## Test LACP PDUs without TRAP set
            print('Sending LACP packet without TRAP set, pkt shall drop')
            pkt = simple_eth_packet(eth_dst='01:80:C2:00:00:02', pktlen=100)
            send_packet(self, self.devports[0], pkt)
            verify_no_other_packets(self, timeout=1)

            ## Test LACP PDUs with TRAP set
            # create trap group.
            queue_handles = self.attribute_get(self.cpu_port_hdl, SWITCH_PORT_ATTR_QUEUE_HANDLES)
            self.hostif_trap_group0 = self.add_hostif_trap_group(self.device, queue_handle=queue_handles[0].oid, admin_state=True)

            print("Sleeping for 5 secs")
            time.sleep(5)

            # create hostif traps for LACP
            self.lacp_trap = self.add_hostif_trap(self.device, type=SWITCH_HOSTIF_TRAP_ATTR_TYPE_LACP,
                         hostif_trap_group_handle=self.hostif_trap_group0,
                         packet_action=SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_REDIRECT_TO_CPU)

            exp_pkt = simple_cpu_packet(
                  packet_type=0,
                  ingress_port=self.devports[0],
                  ingress_ifindex=self.port0_ingress_ifindex,
                  reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_LACP,
                  ingress_bd=2,
                  inner_pkt=pkt)
            self.exp_lacp_pkt = cpu_packet_mask_ingress_bd(exp_pkt)

            print('Sending LACP packet with TRAP set, pkt shall lift to CPU')
            send_packet(self, self.devports[0], pkt)
            if self.test_params['target'] != 'hw':
                verify_packets(self, self.exp_lacp_pkt, [self.cpu_port])

            self.cleanlast()
            print('Sending LACP packet without TRAP set, pkt shall drop')
            send_packet(self, self.devports[0], pkt)
            verify_no_other_packets(self, timeout=1)


        finally:
            pass

###############################################################################

@group('hostif')
class HostIfTrapAttributesUpdateTest(ApiHelper):
    def setUp(self):
        print()
        self.configure()

        # create l3 interface and associate with self.vlan10
        vlan_mbr0 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port0)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.port0_ingress_ifindex = self.get_port_ifindex(self.port0)

        vlan_mbr1 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port1)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.port1_ingress_ifindex = self.get_port_ifindex(self.port1)

        # create trap group.
        queue_handles = self.attribute_get(self.cpu_port_hdl, SWITCH_PORT_ATTR_QUEUE_HANDLES)
        self.hostif_trap_group0 = self.add_hostif_trap_group(self.device, queue_handle=queue_handles[0].oid, admin_state=True)

        # create hostif for port0
        self.hostif_name1 = "test_host_if0"
        self.hostif0 = self.add_hostif(self.device, name=self.hostif_name1, handle=self.port0, oper_status=True)
        self.assertTrue(self.hostif0 != 0)

        print("Sleeping for 5 secs")
        time.sleep(5)

        # create hostif traps for LLDP nad LACP
        self.redirect_trap = self.add_hostif_trap(self.device, type=SWITCH_HOSTIF_TRAP_ATTR_TYPE_LLDP,
                        hostif_trap_group_handle=self.hostif_trap_group0,
                        packet_action=SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_REDIRECT_TO_CPU, priority=1)

        self.drop_trap = self.add_hostif_trap(self.device, type=SWITCH_HOSTIF_TRAP_ATTR_TYPE_LLDP,
                        hostif_trap_group_handle=self.hostif_trap_group0,
                        packet_action=SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_DROP, priority=2)

        self.lacp_trap = self.add_hostif_trap(self.device, type=SWITCH_HOSTIF_TRAP_ATTR_TYPE_LACP,
                        hostif_trap_group_handle=self.hostif_trap_group0,
                        packet_action=SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_REDIRECT_TO_CPU)

    def runTest(self):
        print()
        try:
            if self.test_params['target'] != 'hw':
                self.sock = open_packet_socket(self.hostif_name1)
            self.PriorityUpdateTest()
            self.PacketActionUpdateTest()
        finally:
            pass

    def tearDown(self):
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.cleanup()

    def PriorityUpdateTest(self):
        try:
            # LLDP , dmac1: {0x01, 0x80, 0xC2, 0x00, 0x00, 0x0e};
            pkt = simple_eth_packet(eth_dst='01:80:C2:00:00:0e', pktlen=100)
            exp_pkt = simple_cpu_packet(
                  packet_type=0,
                  ingress_port=self.devports[0],
                  ingress_ifindex=self.port0_ingress_ifindex,
                  reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_LLDP,
                  ingress_bd=2,
                  inner_pkt=pkt)
            self.exp_lldp_pkt = cpu_packet_mask_ingress_bd(exp_pkt)
            print('Sending LLDP packet')
            send_packet(self, self.devports[0], pkt)
            if self.test_params['target'] != 'hw':
                verify_packet(self, self.exp_lldp_pkt, self.cpu_port)
                self.assertTrue(socket_verify_packet(pkt, self.sock))

            # LLDP , dmac2: {0x01, 0x80, 0xC2, 0x00, 0x00, 0x03};
            pkt2 = simple_eth_packet(eth_dst='01:80:C2:00:00:03', pktlen=100)
            exp_pkt2 = simple_cpu_packet(
                  packet_type=0,
                  ingress_port=self.devports[0],
                  ingress_ifindex=self.port0_ingress_ifindex,
                  reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_LLDP,
                  ingress_bd=2,
                  inner_pkt=pkt2)
            self.exp_lldp_pkt2 = cpu_packet_mask_ingress_bd(exp_pkt2)
            print('Sending LLDP packet')
            send_packet(self, self.devports[0], pkt2)
            if self.test_params['target'] != 'hw':
                verify_packet(self, self.exp_lldp_pkt2, self.cpu_port)
                self.assertTrue(socket_verify_packet(pkt2, self.sock))

            # Set lower priority to redirect entry so drop entry has to be hit now
            self.attribute_set(self.redirect_trap, SWITCH_HOSTIF_TRAP_ATTR_PRIORITY, 3)

            # LLDP , dmac1: {0x01, 0x80, 0xC2, 0x00, 0x00, 0x0e};
            print('Sending LLDP packet')
            if self.test_params['target'] != 'hw':
                send_packet(self, self.devports[0], pkt)
                verify_no_other_packets(self, timeout=1)

            # LLDP , dmac2: {0x01, 0x80, 0xC2, 0x00, 0x00, 0x03};
            print('Sending LLDP packet')
            if self.test_params['target'] != 'hw':
                send_packet(self, self.devports[0], pkt2)
                verify_no_other_packets(self, timeout=1)
        finally:
            pass

    def PacketActionUpdateTest(self):
        try:
            # LACP
            pkt = simple_eth_packet(eth_dst='01:80:C2:00:00:02', pktlen=100)
            exp_pkt = simple_cpu_packet(
                  packet_type=0,
                  ingress_port=self.devports[0],
                  ingress_ifindex=self.port0_ingress_ifindex,
                  reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_LACP,
                  ingress_bd=2,
                  inner_pkt=pkt)
            self.exp_lacp_pkt = cpu_packet_mask_ingress_bd(exp_pkt)

            print('Sending LACP packet')
            send_packet(self, self.devports[0], pkt)
            if self.test_params['target'] != 'hw':
                verify_packets(self, self.exp_lacp_pkt, [self.cpu_port])
                self.assertTrue(socket_verify_packet(pkt, self.sock))

            # Update packet action to drop
            self.attribute_set(self.lacp_trap, SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION,
                              SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_DROP)

            print('Sending LACP packet')
            if self.test_params['target'] != 'hw':
                send_packet(self, self.devports[0], pkt)
                verify_no_other_packets(self, timeout=1)

        finally:
            pass

###############################################################################


@group('hostif')
class HostIfTrapStatsTest(ApiHelper):
    def runTest(self):
        print()
        self.configure()

        # create l3 interface and associate with self.port0
        self.rif0 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port0, vrf_handle=self.vrf10,
                                 src_mac=self.rmac)
        self.route0 = self.add_route(self.device, ip_prefix='10.10.10.1', vrf_handle=self.vrf10, is_host_myip=True)
        self.port0_ingress_ifindex = self.get_port_ifindex(self.port0)

        # installing network route for l3 interface network ??. Required for to punt ICMP??
        # create trap group.
        queue_handles = self.attribute_get(self.cpu_port_hdl, SWITCH_PORT_ATTR_QUEUE_HANDLES)
        self.hostif_trap_group0 = self.add_hostif_trap_group(self.device, queue_handle=queue_handles[0].oid, admin_state=True)

        # create hostif for port0
        hostif_name = "test_host_if"
        self.hostif0 = self.add_hostif(self.device, name=hostif_name, handle=self.port0, oper_status=True)
        self.assertTrue(self.hostif0 != 0)
        print("Sleeping for 5 secs")
        time.sleep(5)

        # open socket to verfiy packet recieved on hostif netd.
        if self.test_params['target'] != 'hw':
            self.sock = open_packet_socket(hostif_name)

        self.arp_trap = self.add_hostif_trap(self.device, type=SWITCH_HOSTIF_TRAP_ATTR_TYPE_ARP_REQUEST,
                        hostif_trap_group_handle=self.hostif_trap_group0,
                        packet_action=SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_COPY_TO_CPU)
        self.dhcp_trap = self.add_hostif_trap(self.device, type=SWITCH_HOSTIF_TRAP_ATTR_TYPE_DHCP,
                         hostif_trap_group_handle=self.hostif_trap_group0,
                         packet_action=SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_REDIRECT_TO_CPU)

        try:
            self.ArpStatsTest()
            self.DhcpStatsTest()
        finally:
            self.cleanup()

    def ArpStatsTest(self):
        try:
            pre_counter = self.client.object_counters_get(self.arp_trap)
            # Broadcast ARP Request
            pkt = simple_arp_packet(arp_op=1, pktlen=100)
            exp_pkt = simple_cpu_packet(
                  packet_type=0,
                  ingress_port=self.devports[0],
                  ingress_ifindex=self.port0_ingress_ifindex,
                  reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_ARP_REQUEST,
                  ingress_bd=2,
                  inner_pkt=pkt)
            self.exp_arpq_bc_pkt = cpu_packet_mask_ingress_bd(exp_pkt)
            print('Sending ARP request broadcast')
            send_packet(self, self.devports[0], pkt)
            if self.test_params['target'] != 'hw':
                verify_packet(self, self.exp_arpq_bc_pkt, self.cpu_port)
                self.assertTrue(socket_verify_packet(pkt, self.sock))

            # Unicast ARP Request
            pkt = simple_arp_packet(arp_op=1, eth_dst=self.rmac, pktlen=100)
            exp_pkt = simple_cpu_packet(
                  packet_type=0,
                  ingress_port=self.devports[0],
                  ingress_ifindex=self.port0_ingress_ifindex,
                  reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_ARP_REQUEST,
                  ingress_bd=2,
                  inner_pkt=pkt)
            self.exp_arpq_uc_pkt = cpu_packet_mask_ingress_bd(exp_pkt)
            print('Sending unicast ARP request to router MAC')
            send_packet(self, self.devports[0], pkt)
            if self.test_params['target'] != 'hw':
                verify_packet(self, self.exp_arpq_uc_pkt, self.cpu_port)
                self.assertTrue(socket_verify_packet(pkt, self.sock))
            if self.test_params['target'] == 'hw':
                time.sleep(2)
            post_counter = self.client.object_counters_get(self.arp_trap)
            idx = SWITCH_HOSTIF_TRAP_COUNTER_ID_RX_PKTS
            self.assertEqual(post_counter[idx].count - pre_counter[idx].count, 2)
        finally:
            pass

    def DhcpStatsTest(self):
        try:
            pre_counter = self.client.object_counters_get(self.dhcp_trap)
            # DHCP request
            pkt = simple_udp_packet(
                  ip_dst='255.255.255.255', udp_sport=67, udp_dport=68)
            exp_pkt = ptf.mask.Mask(simple_cpu_packet(
                  packet_type=0,
                  ingress_port=self.devports[0],
                  ingress_ifindex=self.port0_ingress_ifindex,
                  reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_DHCP,
                  inner_pkt=pkt))
            mask_set_do_not_care_packet(exp_pkt, FabricCpuHeader, "ingress_bd")
            print('Sending DHCP request packet')
            send_packet(self, self.devports[0], pkt)
            if self.test_params['target'] != 'hw':
                verify_packets(self, exp_pkt, [self.cpu_port])
                self.assertTrue(socket_verify_packet(pkt, self.sock))

            # DHCP Ack
            pkt = simple_udp_packet(eth_dst=self.rmac, ip_dst='10.10.10.1',
                                    udp_sport=68, udp_dport=67)
            exp_pkt = ptf.mask.Mask(simple_cpu_packet(
                  packet_type=0,
                  ingress_port=self.devports[0],
                  ingress_ifindex=self.port0_ingress_ifindex,
                  reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_DHCP,
                  inner_pkt=pkt))
            mask_set_do_not_care_packet(exp_pkt, FabricCpuHeader, "ingress_bd")
            print('Sending DHCP ack packet')
            send_packet(self, self.devports[0], pkt)
            if self.test_params['target'] != 'hw':
                verify_packets(self, exp_pkt, [self.cpu_port])
                self.assertTrue(socket_verify_packet(pkt, self.sock))
            if self.test_params['target'] == 'hw':
                time.sleep(2)
            post_counter = self.client.object_counters_get(self.dhcp_trap)
            idx = SWITCH_HOSTIF_TRAP_COUNTER_ID_RX_PKTS
            self.assertEqual(post_counter[idx].count - pre_counter[idx].count, 2)
        finally:
            pass

###############################################################################
@disabled
class HostIfGenlTest(ApiHelper):
    def runTest(self):
        print()
        self.configure()

        try:
            queue_handles = self.attribute_get(self.cpu_port_hdl, SWITCH_PORT_ATTR_QUEUE_HANDLES)
            self.hostif_trap_group0 = self.add_hostif_trap_group(self.device, queue_handle=queue_handles[0].oid, admin_state=1)
            self.genl_trap = self.add_hostif_trap(self.device,
                type=SWITCH_HOSTIF_TRAP_ATTR_TYPE_SAMPLEPACKET,
                hostif_trap_group_handle=self.hostif_trap_group0,
                packet_action=SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_REDIRECT_TO_CPU)

            self.genl_hostif = self.add_hostif(self.device, type=SWITCH_HOSTIF_ATTR_TYPE_GENETLINK,
                name="genl_test", genl_mcgrp_name="genl_mcgrp0")

            self.add_hostif_rx_filter(self.device, type=SWITCH_HOSTIF_RX_FILTER_ATTR_TYPE_TRAP,
                hostif_trap_handle=self.genl_trap, hostif=self.genl_hostif)
        finally:
            self.cleanup()

###############################################################################


@group('hostif')
class HostIfPingTest(ApiHelper):
    def setUp(self):
        print()
        self.configure()
        self.prefix_len = 16

        self.cpu_nexthop = self.add_nexthop(self.device, type=SWITCH_NEXTHOP_ATTR_TYPE_GLEAN)

        self.lag0 = self.add_lag(self.device)
        lag_mbr0 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port1)
        lag_mbr1 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port2)
        lag_mbr2 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port3)
        lag_mbr3 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port4)

        # create l3 interface on self.port0
        self.rif0 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port0,
            vrf_handle=self.vrf10, src_mac=self.rmac)
        # create l3 interface on self.lag0
        self.rif1 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.lag0,
            vrf_handle=self.vrf10, src_mac=self.rmac)

        # bind 30.30.0.3 to rif0
        self.rif0_ip = '30.30.0.3'
        self.route1 = self.add_route(self.device, ip_prefix=self.rif0_ip, vrf_handle=self.vrf10,
                                     nexthop_handle=self.cpu_nexthop, is_host_myip=True)

        # bind 20.20.0.3 to rif1
        self.rif1_ip = '20.20.0.3'
        self.route1 = self.add_route(self.device, ip_prefix=self.rif1_ip, vrf_handle=self.vrf10,
                                     nexthop_handle=self.cpu_nexthop, is_host_myip=True)

        # create trap group.
        queue_handles = self.attribute_get(self.cpu_port_hdl, SWITCH_PORT_ATTR_QUEUE_HANDLES)
        self.hostif_trap_group0 = self.add_hostif_trap_group(self.device, queue_handle=queue_handles[0].oid, admin_state=1)

        # create hostif for rif0
        rif0_hostif_name = "rif0"
        rif0_ip_addr = self.rif0_ip + '/' + str(self.prefix_len)
        self.rif0_hostif0 = self.add_hostif(self.device, name=rif0_hostif_name,
                                       handle=self.rif0, oper_status=True, ip_addr=rif0_ip_addr,
                                       mac=self.rmac)
        self.assertTrue(self.rif0_hostif0 != 0)

        '''
        # create hostif for rif1
        rif1_hostif_name = "rif1"
        rif1_ip_addr = self.rif1_ip + '/' + str(self.prefix_len)
        self.rif1_hostif0 = self.add_hostif(self.device, name=rif1_hostif_name,
                                       handle=self.rif1, oper_status=True, ip_addr=rif1_ip_addr,
                                       mac=self.rmac)
        self.assertTrue(self.rif1_hostif0 != 0)
        '''

        print("Sleeping for 5 secs")
        time.sleep(5)

        # create hostif_trap entries for following trap types.
        trap_list = [
            [SWITCH_HOSTIF_TRAP_ATTR_TYPE_ARP_REQUEST, SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_COPY_TO_CPU],
          [SWITCH_HOSTIF_TRAP_ATTR_TYPE_ARP_RESPONSE, SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_REDIRECT_TO_CPU],
          [SWITCH_HOSTIF_TRAP_ATTR_TYPE_MYIP, SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_COPY_TO_CPU]
        ]

        for trap in trap_list:
            trap.append(self.add_hostif_trap(self.device, type=trap[0],
                        hostif_trap_group_handle=self.hostif_trap_group0,
                        packet_action=trap[1]))

    def runTest(self):
        print()
        try:
            queue_handles = self.attribute_get(self.port0, SWITCH_PORT_ATTR_QUEUE_HANDLES)
            pre_cntr = self.client.object_counters_get(queue_handles[7].oid)

            print("Test ping on rif0")
            self.PingPortTest('30.30.0.1', '30.30.0.3', '00:06:07:08:09:0a', self.devports[0], [self.devports[0]])
            #print("Test ping on rif1")
            #self.PingPortTest('20.20.0.1', '20.20.0.3', '00:06:07:08:09:0b', self.devports[1], [self.devports[1], self.devports[2], self.devports[3], self.devports[4]])

            # set ip_addr rif0 and test ping traffic.
            rif0_ip_addr = "40.40.0.3/16"
            self.route3 = self.add_route(self.device, ip_prefix="40.40.0.3", vrf_handle=self.vrf10,
                                       nexthop_handle=self.cpu_nexthop, is_host_myip=True)
            self.attribute_set(self.rif0_hostif0, SWITCH_HOSTIF_ATTR_IP_ADDR, rif0_ip_addr)

            print("Test ping on rif0 for target_ip: {}".format(rif0_ip_addr))
            self.PingPortTest('40.40.0.2', '40.40.0.3', '00:06:07:08:09:0a', self.devports[0], [self.devports[0]])

            # update mac address of rif0 and test ping traffic
            old_rmac = self.rmac
            self.rmac = '00:77:77:55:44:22'
            self.attribute_set(self.rif0, SWITCH_RIF_ATTR_SRC_MAC, self.rmac)
            self.attribute_set(self.rif0_hostif0, SWITCH_HOSTIF_ATTR_MAC, self.rmac)
            time.sleep(5)
            print("Test ping on rif0 after updating mac to {}".format(self.rmac))
            self.PingPortTest('40.40.0.2', '40.40.0.3', '00:06:07:08:09:0a', self.devports[0], [self.devports[0]])
            self.rmac = old_rmac

            if self.test_params['target'] == 'hw':
                time.sleep(2)
            post_cntr = self.client.object_counters_get(queue_handles[7].oid)
            idx = SWITCH_QUEUE_COUNTER_ID_STAT_PACKETS
            self.assertTrue((post_cntr[idx].count - pre_cntr[idx].count) >= 6)
        finally:
            pass

    def tearDown(self):
        self.cleanup()

    def PingPortTest(self, host_ip, target_ip, host_mac, ingress_port, egress_port_list):
        try:
        # test arp resoultion.
            pkt = simple_arp_packet(
                        arp_op=1,
                        pktlen=100,
                        eth_src=host_mac,
                        hw_snd=host_mac,
                        ip_snd=host_ip,
                        ip_tgt=target_ip)
            exp_pkt = simple_arp_packet(
                        pktlen=42,
                        arp_op=2,
                        eth_src=self.rmac,
                        eth_dst=host_mac,
                        hw_snd=self.rmac,
                        hw_tgt=host_mac,
                        ip_snd=target_ip,
                        ip_tgt=host_ip)
            send_packet(self, ingress_port, pkt)
            verify_packet_any_port(self, exp_pkt, egress_port_list)
            time.sleep(1)

            # test ping
            pkt = simple_icmp_packet(
                        eth_src=host_mac,
                        eth_dst=self.rmac,
                        ip_src=host_ip,
                        ip_dst=target_ip,
                        icmp_type=8,
                        icmp_data='000102030405')
            exp_pkt= simple_icmp_packet(
                        eth_src=self.rmac,
                        eth_dst=host_mac,
                        ip_src=target_ip,
                        ip_dst=host_ip,
                        icmp_type=0,
                        icmp_data='000102030405')
            m = Mask(exp_pkt)
            mask_set_do_not_care_packet(m, IP, "id")
            mask_set_do_not_care_packet(m, IP, "chksum")

            send_packet(self, ingress_port, pkt)
            verify_packet_any_port(self, m, egress_port_list)
            time.sleep(1)
        finally:
            pass

###############################################################################


@group('hostif')
class HostIfTxMirrorTest(ApiHelper):
    def setUp(self):
        print()
        self.configure()
        self.prefix_len = 16

        self.cpu_nexthop = self.add_nexthop(self.device, type=SWITCH_NEXTHOP_ATTR_TYPE_GLEAN)

        # create l3 interface on self.port0
        self.rif0 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port0,
            vrf_handle=self.vrf10, src_mac=self.rmac)

        # bind 30.30.0.3 to rif0
        self.rif0_ip = '30.30.0.3'
        self.route1 = self.add_route(self.device, ip_prefix=self.rif0_ip, vrf_handle=self.vrf10,
                                     nexthop_handle=self.cpu_nexthop, is_host_myip=True)

        # create trap group.
        queue_handles = self.attribute_get(self.cpu_port_hdl, SWITCH_PORT_ATTR_QUEUE_HANDLES)
        self.hostif_trap_group0 = self.add_hostif_trap_group(self.device, queue_handle=queue_handles[0].oid, admin_state=1)

        # create hostif for rif0
        rif0_hostif_name = "rif0"
        rif0_ip_addr = self.rif0_ip + '/' + str(self.prefix_len)
        self.rif0_hostif0 = self.add_hostif(self.device, name=rif0_hostif_name,
                                       handle=self.rif0, oper_status=True, ip_addr=rif0_ip_addr,
                                       mac=self.rmac)
        self.assertTrue(self.rif0_hostif0 != 0)

        print("Sleeping for 5 secs")
        time.sleep(5)

        # create hostif_trap entries for following trap types.
        trap_list = [
            [SWITCH_HOSTIF_TRAP_ATTR_TYPE_ARP_REQUEST, SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_COPY_TO_CPU],
            [SWITCH_HOSTIF_TRAP_ATTR_TYPE_ARP_RESPONSE, SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_REDIRECT_TO_CPU]
        ]

        for trap in trap_list:
            trap.append(self.add_hostif_trap(self.device, type=trap[0],
                        hostif_trap_group_handle=self.hostif_trap_group0,
                        packet_action=trap[1]))

        # add egress mirror session port 2
        self.eg_mirror = self.add_mirror(self.device,
            type=SWITCH_MIRROR_ATTR_TYPE_LOCAL,
            direction=SWITCH_MIRROR_ATTR_DIRECTION_EGRESS,
            egress_port_handle=self.port2)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_EGRESS_MIRROR_HANDLE, self.eg_mirror)

    def tearDown(self):
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_EGRESS_MIRROR_HANDLE, 0)
        self.cleanup()

    def runTest(self):
        print()
        if (self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_PORT_MIRROR) == 0):
            print ("Egress port mirror is disabled.")
            return
        try:
            print("Test ping on rif0")
            self.ArpTest('30.30.0.1', '30.30.0.3', '00:06:07:08:09:0a', self.devports[0], [self.devports[0], self.devports[2]])
        finally:
            pass

    def ArpTest(self, host_ip, target_ip, host_mac, ingress_port, egress_port_list):
        try:
            # test arp resoultion.
            print("Send ARP packet, response is mirrored")
            pkt = simple_arp_packet(
                        arp_op=1,
                        pktlen=100,
                        eth_src=host_mac,
                        hw_snd=host_mac,
                        ip_snd=host_ip,
                        ip_tgt=target_ip)
            exp_pkt = simple_arp_packet(
                        pktlen=42,
                        arp_op=2,
                        eth_src=self.rmac,
                        eth_dst=host_mac,
                        hw_snd=self.rmac,
                        hw_tgt=host_mac,
                        ip_snd=target_ip,
                        ip_tgt=host_ip)
            cpu_pkt = simple_cpu_packet(
                packet_type=0,
                ingress_port=self.devports[0],
                ingress_ifindex=(self.port0 & 0xFFFF),
                reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_ARP_REQUEST,
                ingress_bd=2,
                inner_pkt=pkt)
            exp_cpu_pkt = cpu_packet_mask_ingress_bd(cpu_pkt)
            send_packet(self, ingress_port, pkt)
            verify_packets_any(self, exp_pkt, egress_port_list)
        finally:
            pass

###############################################################################


@group('hostif')
class HostIfCoppTest(ApiHelper):
    def setUp(self):
        print()
        self.configure()

        # create l3 interfaces on port0 and port1
        self.rif0 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port0, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.rif1 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port1, vrf_handle=self.vrf10, src_mac=self.rmac)

        self.nhop0 = self.add_nexthop(self.device, handle=self.rif0, dest_ip='30.30.30.1')
        self.neighbor0 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=self.rif0, dest_ip='30.30.30.1')
        self.route1 = self.add_route(self.device, ip_prefix='30.30.30.1', vrf_handle=self.vrf10, nexthop_handle=self.nhop0)

        # create policer handles
        self.meter1 = self.add_meter(self.device,
            mode=SWITCH_METER_ATTR_MODE_TWO_RATE_THREE_COLOR,
            pbs=16,cbs=8,cir=1000,pir=1000,
            type=SWITCH_METER_ATTR_TYPE_PACKETS,
            color_source=SWITCH_METER_ATTR_COLOR_SOURCE_BLIND)
        self.meter2 = self.add_meter(self.device,
            mode=SWITCH_METER_ATTR_MODE_TWO_RATE_THREE_COLOR,
            pbs=8,cbs=4,cir=500,pir=500,
            type=SWITCH_METER_ATTR_TYPE_PACKETS,
            color_source=SWITCH_METER_ATTR_COLOR_SOURCE_BLIND)
        if (self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_COPP)):
            self.meter3 = self.add_meter(self.device,
                mode=SWITCH_METER_ATTR_MODE_TWO_RATE_THREE_COLOR,
                pbs=12,cbs=6,cir=1000,pir=1000,
                type=SWITCH_METER_ATTR_TYPE_PACKETS,
                color_source=SWITCH_METER_ATTR_COLOR_SOURCE_BLIND)

        # create trap groups
        queue_handles = self.attribute_get(self.cpu_port_hdl, SWITCH_PORT_ATTR_QUEUE_HANDLES)
        self.hostif_trap_group1 = self.add_hostif_trap_group(self.device,
            queue_handle=queue_handles[0].oid,
            admin_state=True,
            policer_handle=self.meter1)
        self.hostif_trap_group2 = self.add_hostif_trap_group(self.device,
            queue_handle=queue_handles[0].oid,
            admin_state=True,
            policer_handle=self.meter2)
        if (self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_COPP)):
            self.hostif_trap_group3 = self.add_hostif_trap_group(self.device,
                queue_handle=queue_handles[0].oid,
                admin_state=True,
                policer_handle=self.meter3)

        self.hostif_trap1 = self.add_hostif_trap(self.device,
            type=SWITCH_HOSTIF_TRAP_ATTR_TYPE_ARP_REQUEST,
            hostif_trap_group_handle=self.hostif_trap_group1,
            packet_action=SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_REDIRECT_TO_CPU)
        self.hostif_trap2 = self.add_hostif_trap(self.device,
            type=SWITCH_HOSTIF_TRAP_ATTR_TYPE_TTL_ERROR,
            hostif_trap_group_handle=self.hostif_trap_group2,
            packet_action=SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_REDIRECT_TO_CPU)
        if (self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_COPP)):
            self.hostif_trap3 = self.add_hostif_trap(self.device,
                type=SWITCH_HOSTIF_TRAP_ATTR_TYPE_L3_MTU_ERROR,
                hostif_trap_group_handle=self.hostif_trap_group3,
                packet_action=SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_REDIRECT_TO_CPU)

        self.arp_pkt = simple_arp_packet(arp_op=1, pktlen=100)
        self.exp_arp_pkt = simple_cpu_packet(
              packet_type=0,
              ingress_port=self.devports[1],
              ingress_ifindex=(self.port1 & 0xFFFF),
              reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_ARP_REQUEST,
              ingress_bd=2,
              inner_pkt=self.arp_pkt)
        self.exp_arpq_bc_pkt = cpu_packet_mask_ingress_bd(self.exp_arp_pkt)
        self.pkt = simple_tcp_packet(
            eth_dst=self.rmac,
            eth_src='00:22:22:22:22:22',
            ip_dst='30.30.30.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=1)
        self.exp_pkt = simple_cpu_packet(
              packet_type=0,
              ingress_port=self.devports[1],
              ingress_ifindex=(self.port1 & 0xFFFF),
              reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_TTL_ERROR,
              ingress_bd=2,
              inner_pkt=self.pkt)
        self.exp_ttl_pkt = cpu_packet_mask_ingress_bd(self.exp_pkt)
        self.mtu_pkt = simple_tcp_packet(
            eth_dst=self.rmac,
            eth_src='00:22:22:22:22:22',
            ip_dst='30.30.30.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64,
            pktlen=2000)
        self.mtu_egress_pkt = simple_tcp_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src=self.rmac,
            ip_dst='30.30.30.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63,
            pktlen=2000)
        self.exp_mtu_pkt = simple_cpu_packet(
              packet_type=0,
              ingress_port=0,
              ingress_ifindex=0,
              reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_L3_MTU_ERROR,
              ingress_bd=2,
              inner_pkt=self.mtu_egress_pkt)
        self.exp_mtu_egress_pkt = cpu_packet_mask_ingress_bd(self.exp_mtu_pkt)

    def runTest(self):
        print()
        try:
            self.HostIfCoppBasicTest(4)
            self.HostIfCoppSetTest()
        finally:
            pass

    def tearDown(self):
        self.cleanup()

    def HostIfCoppBasicTest(self, check_stats_count):
        try:
            print("Use meter 1 for hif group 1, meter 2 for hif group 2 and meter 3 for hif group 3")
            print("Send 4 packets each thru hif group 1, 2 and 3")
            for i in range(0, 4):
                send_packet(self, self.devports[1], self.arp_pkt)
                if self.test_params['target'] != 'hw':
                    verify_packets(self, self.exp_arpq_bc_pkt, [self.cpu_port])

                send_packet(self, self.devports[1], self.pkt)
                if self.test_params['target'] != 'hw':
                    verify_packets(self, self.exp_ttl_pkt, [self.cpu_port])

                if (self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_COPP)):
                    send_packet(self, self.devports[1], self.mtu_pkt)
                    if self.test_params['target'] != 'hw':
                        verify_packets(self, self.exp_mtu_egress_pkt, [self.cpu_port])

            print('Sleeping for 5 sec')
            time.sleep(5)
            counter1_packets = counter2_packets = counter3_packets = 0
            counter1 = self.client.object_counters_get(self.meter1)
            counter2 = self.client.object_counters_get(self.meter2)
            if (self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_COPP)):
                counter3 = self.client.object_counters_get(self.meter3)
            print("Meter 1, 2 and 3 each has 4 green")
            for i in range(0, 3):
                counter1_packets += counter1[i].count
                counter2_packets += counter2[i].count
                if (self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_COPP)):
                    counter3_packets += counter3[i].count
            if (self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_COPP)):
                print(counter1_packets, counter2_packets, counter3_packets)
            else:
                print(counter1_packets, counter2_packets)
            self.assertGreaterEqual(counter1_packets, check_stats_count)
            self.assertGreaterEqual(counter2_packets, check_stats_count)
            if (self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_COPP)):
                self.assertGreaterEqual(counter3_packets, check_stats_count)
        finally:
            pass

    def HostIfCoppSetTest(self):
        print("Set hif group 2 to use meter 1")
        self.attribute_set(self.hostif_trap_group2, SWITCH_HOSTIF_TRAP_GROUP_ATTR_POLICER_HANDLE, self.meter1)
        try:
            counter1 = self.client.object_counters_get(self.meter1)
            for i in range(0, 3):
                counter1_packets = counter1[i].count
            print("check Meter1 to make sure it's stats are cleared")
            print(counter1_packets)
            self.assertEqual(counter1_packets, 0)
            print("Send 4 packets each thru hif group 1, 2 and 3")
            for i in range(0, 4):
                send_packet(self, self.devports[1], self.arp_pkt)
                if self.test_params['target'] != 'hw':
                    verify_packets(self, self.exp_arpq_bc_pkt, [self.cpu_port])
                send_packet(self, self.devports[1], self.pkt)
                if self.test_params['target'] != 'hw':
                    verify_packets(self, self.exp_ttl_pkt, [self.cpu_port])
                if (self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_COPP)):
                    send_packet(self, self.devports[1], self.mtu_pkt)
                    if self.test_params['target'] != 'hw':
                        verify_packets(self, self.exp_mtu_egress_pkt, [self.cpu_port])

            print('Sleeping for 5 sec')
            time.sleep(5)

            counter1_packets = counter2_packets = counter3_packets = 0
            counter1 = self.client.object_counters_get(self.meter1)
            counter2 = self.client.object_counters_get(self.meter2)
            if (self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_COPP)):
                counter3 = self.client.object_counters_get(self.meter3)
            print("Meter 1 has 8 green and meter 3 has 4")
            for i in range(0, 3):
                counter1_packets += counter1[i].count
                counter2_packets += counter2[i].count
                if (self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_COPP)):
                    counter3_packets += counter3[i].count
            if (self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_COPP)):
                print(counter1_packets, counter2_packets, counter3_packets)
            else:
                print(counter1_packets, counter2_packets)
            self.assertEqual(counter1_packets, 0+8)
            self.assertEqual(counter2_packets, 4+0)
            if (self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_COPP)):
                self.assertEqual(counter3_packets, 4+4)
        finally:
            pass

###############################################################################

@group('l3')
@group('ipv6')
class ExceptionPacketsTest(ApiHelper):
    '''
    This test tries to verify packets using default system acl entries
    We send a packet from port 0 and forward/drop depending on packet contents
    @test - IPv4ExceptionTest
    @test - IPv6ExceptionTest
    '''
    def setUp(self):
        print()
        print('Configuring devices for exception packet test cases')
        self.configure()

        devport0 = self.attribute_get(self.port0, SWITCH_PORT_ATTR_DEV_PORT)

        self.rif0 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port0, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.nhop0 = self.add_nexthop(self.device, handle=self.rif0, dest_ip='10.10.10.1')
        self.neighbor0 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=self.rif0, dest_ip='10.10.10.1')
        self.route0 = self.add_route(self.device, ip_prefix='10.10.10.1', vrf_handle=self.vrf10, nexthop_handle=self.nhop0)
        self.route06 = self.add_route(self.device, ip_prefix='3000::3', vrf_handle=self.vrf10, nexthop_handle=self.nhop0)

        self.rif1 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port1, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.nhop1 = self.add_nexthop(self.device, handle=self.rif1, dest_ip='10.20.10.1')
        self.neighbor1 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:66', handle=self.rif1, dest_ip='10.20.10.1')
        self.route1 = self.add_route(self.device, ip_prefix='10.20.10.1', vrf_handle=self.vrf10, nexthop_handle=self.nhop1)
        self.route16 = self.add_route(self.device, ip_prefix='2000::3', vrf_handle=self.vrf10, nexthop_handle=self.nhop1)

        self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_LEARNING, False)
        vlan_member0 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port2)
        vlan_member1 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port5)
        vlan_member2 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port4)
        self.attribute_set(self.port2, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.attribute_set(self.port5, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.attribute_set(self.port4, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:44:44:44:44:44', destination_handle=self.port4)

    def runTest(self):
        try:
            self.IPv4ExceptionTest()
            self.IPv6ExceptionTest()
        finally:
            pass

    def tearDown(self):
        self.attribute_set(self.port2, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.port5, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.port4, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.cleanup()

    def IPv4ExceptionTest(self):
        try:
            print("Valid IPv4 packet from port %d to %d" % (self.devports[0], self.devports[1]))
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='10.20.10.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_ttl=64)
            exp_pkt = simple_tcp_packet(
                eth_dst='00:11:22:33:44:66',
                eth_src='00:77:66:55:44:33',
                ip_dst='10.20.10.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_ttl=63)
            send_packet(self, self.devports[0], pkt)
            verify_packets(self, exp_pkt, [self.devports[1]])

            print("Valid bridge IPv4 packet with TTL=1 from port %d to %d" % (self.devports[0], self.devports[1]))
            pkt = simple_tcp_packet(
                eth_dst='00:44:44:44:44:44',
                eth_src='00:22:22:22:22:22',
                ip_dst='10.20.10.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_ttl=64)
            send_packet(self, self.devports[2], pkt)
            verify_packets(self, pkt, [self.devports[4]])

            print("rmac hit, non ip packet, drop")
            pkt = simple_eth_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22')
            send_packet(self, self.devports[0], pkt)
            verify_no_other_packets(self, timeout=1)

            print("ipv4, routed, ttl = 1, redirect to cpu")
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='10.20.10.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_ttl=1)

            cpu_pkt = simple_cpu_packet(
                packet_type=0,
                ingress_port=swport_to_devport(self, self.devports[0]),
                ingress_ifindex=(self.port0 & 0xFFFF),
                reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_TTL_ERROR,
                ingress_bd=0x1006,
                inner_pkt=pkt)
            cpu_pkt = cpu_packet_mask_ingress_bd(cpu_pkt)
            if self.test_params['target'] != 'hw':
                send_packet(self, self.devports[0], pkt)
                verify_packets(self, cpu_pkt, [self.cpu_port])

            if (self.client.is_feature_enable(SWITCH_FEATURE_L3_UNICAST_SELF_FWD_CHECK) != 0):
                print("ipv4, routed, ingress bd == egress_bd, copy to cpu")
                pkt = simple_tcp_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src='00:22:22:22:22:22',
                    ip_dst='10.10.10.1',
                    ip_src='192.168.0.1',
                    ip_id=105,
                    ip_ttl=63)
                exp_pkt = simple_tcp_packet(
                    eth_dst='00:11:22:33:44:55',
                    eth_src='00:77:66:55:44:33',
                    ip_dst='10.10.10.1',
                    ip_src='192.168.0.1',
                    ip_id=105,
                    ip_ttl=62)
                if self.test_params['target'] != 'hw':
                    send_packet(self, self.devports[0], pkt)
                    verify_packets(self, exp_pkt, [self.devports[0]])
        finally:
            pass

    def IPv6ExceptionTest(self):
        try:
            print("Valid IPv6 packet from port %d to %d" % (self.devports[0], self.devports[1]))
            pkt = simple_tcpv6_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ipv6_dst='2000::3',
                ipv6_src='3000::3',
                ipv6_hlim=64)
            exp_pkt = simple_tcpv6_packet(
                eth_dst='00:11:22:33:44:66',
                eth_src='00:77:66:55:44:33',
                ipv6_dst='2000::3',
                ipv6_src='3000::3',
                ipv6_hlim=63)
            send_packet(self, self.devports[0], pkt)
            verify_packets(self, exp_pkt, [self.devports[1]])

            print("Valid bridge IPv6 packet with TTL = 1 from port %d to %d" % (self.devports[0], self.devports[1]))
            pkt = simple_tcpv6_packet(
                eth_dst='00:44:44:44:44:44',
                eth_src='00:22:22:22:22:22',
                ipv6_dst='2000::3',
                ipv6_src='3000::3',
                ipv6_hlim=64)
            send_packet(self, self.devports[2], pkt)
            verify_packets(self, pkt, [self.devports[4]])

            print("ipv6, routed, ttl = 1, redirect to cpu")
            pkt = simple_tcpv6_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ipv6_dst='2000::3',
                ipv6_src='3000::3',
                ipv6_hlim=1)
            cpu_pkt = simple_cpu_packet(
                packet_type=0,
                ingress_port=swport_to_devport(self, self.devports[0]),
                ingress_ifindex=(self.port0 & 0xFFFF),
                reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_TTL_ERROR,
                ingress_bd=0x1006,
                inner_pkt=pkt)
            cpu_pkt = cpu_packet_mask_ingress_bd(cpu_pkt)
            if self.test_params['target'] != 'hw':
                send_packet(self, self.devports[0], pkt)
                verify_packets(self, cpu_pkt, [self.cpu_port])

            if (self.client.is_feature_enable(SWITCH_FEATURE_L3_UNICAST_SELF_FWD_CHECK) != 0):
                print("ipv6, routed, ingress bd == egress_bd, copy to cpu")
                pkt = simple_tcpv6_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src='00:22:22:22:22:22',
                    ipv6_dst='3000::3',
                    ipv6_src='2000::3',
                    ipv6_hlim=64)
                exp_pkt = simple_tcpv6_packet(
                    eth_dst='00:11:22:33:44:55',
                    eth_src='00:77:66:55:44:33',
                    ipv6_dst='3000::3',
                    ipv6_src='2000::3',
                    ipv6_hlim=63)
                if self.test_params['target'] != 'hw':
                    send_packet(self, self.devports[0], pkt)
                    verify_packets(self, exp_pkt, [self.devports[0]])

            print("ipv6, routed, src is link-local, redirect to cpu")
            pkt = simple_tcpv6_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ipv6_dst='3000::3',
                ipv6_src='fe80::1',
                ipv6_hlim=64)
            cpu_pkt = simple_cpu_packet(
                packet_type=0,
                ingress_port=swport_to_devport(self, self.devports[0]),
                ingress_ifindex=(self.port0 & 0xFFFF),
                reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_MAX +
                    SWITCH_HOSTIF_TRAP_ATTR_INTERNAL_TRAP_SRC_IS_LINK_LOCAL,
                ingress_bd=0x01,
                inner_pkt=pkt)
            cpu_pkt = cpu_packet_mask_ingress_bd(cpu_pkt)
            #send_packet(self, self.devports[0], pkt)
            #verify_packets(self, cpu_pkt, [self.cpu_port])
        finally:
            pass

###############################################################################

@group('l2')
class L2MalformedPacketsTest(ApiHelper):
    '''
    This test tries to verify packets using validate_ethernet table
    We send a packet from port 0 and forward/drop depending on packet contents
    '''
    def setUp(self):
        print()
        print('Configuring devices for ipv4 malformed packet test cases')
        self.configure()

        self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port0)
        self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port1)
        self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port2)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.attribute_set(self.port2, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)

        mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:01:00:00:00:12', destination_handle=self.port0)
        mac1 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:01:00:00:00:22', destination_handle=self.port1)
        mac2 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:01:00:00:00:32', destination_handle=self.port2)

        lag0 = self.add_lag(self.device)
        lag_mbr0 = self.add_lag_member(self.device, lag_handle=lag0, port_handle=self.port3)
        lag_mbr1 = self.add_lag_member(self.device, lag_handle=lag0, port_handle=self.port4)
        self.add_vlan_member(self.device, vlan_handle=self.vlan20, member_handle=lag0, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
        self.add_vlan_member(self.device, vlan_handle=self.vlan20, member_handle=self.port5)
        mac3 = self.add_mac_entry(self.device, vlan_handle=self.vlan20, mac_address='00:01:00:00:00:42', destination_handle=lag0)
        mac4 = self.add_mac_entry(self.device, vlan_handle=self.vlan20, mac_address='00:01:00:00:00:52', destination_handle=self.port5)

    def runTest(self, post_hitless=False):
        try:
            initial_stats = self.client.object_counters_get(self.port0)
            num_drops = 0

            print("Valid packet from port %d to %d" % (self.devports[0], self.devports[1]))
            pkt = simple_tcp_packet(
                eth_dst='00:01:00:00:00:22',
                eth_src='00:01:00:00:00:12',
                ip_dst='10.10.10.1',
                ip_id=108,
                ip_ttl=64)
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, pkt, self.devports[1])

            print("Valid packet from lag 0 to port %d" % self.devports[5])
            tag_pkt = simple_tcp_packet(
                eth_dst='00:01:00:00:00:52',
                eth_src='00:01:00:00:00:42',
                ip_dst='10.10.10.1',
                dl_vlan_enable=True,
                vlan_vid=20,
                ip_ttl=64)
            pkt = simple_tcp_packet(
                eth_dst='00:01:00:00:00:52',
                eth_src='00:01:00:00:00:42',
                ip_dst='10.10.10.1',
                ip_ttl=64,
                pktlen=96)
            send_packet(self, self.devports[3], tag_pkt)
            verify_packet(self, pkt, self.devports[5])
            send_packet(self, self.devports[4], tag_pkt)
            verify_packet(self, pkt, self.devports[5])

            print("Same if check fail, drop")
            mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:02:00:00:00:12', destination_handle=self.port0)
            pkt = simple_tcp_packet(
                eth_dst='00:02:00:00:00:12',
                eth_src='00:01:00:00:00:12',
                ip_dst='10.10.10.1',
                ip_id=108,
                ip_ttl=64)
            send_packet(self, self.devports[0], pkt)
            verify_no_other_packets(self, timeout=1)
            num_drops += 1
            self.cleanlast()

            if (self.client.is_feature_enable(SWITCH_FEATURE_SAME_MAC_CHECK) != 0):
                print("Same mac check fail, drop")
                pkt = simple_tcp_packet(
                    eth_dst='00:01:00:00:00:12',
                    eth_src='00:01:00:00:00:12',
                    ip_dst='10.10.10.1',
                    ip_id=108,
                    ip_ttl=64)
                send_packet(self, self.devports[0], pkt)
                verify_no_other_packets(self, timeout=1)
                num_drops += 1

            print("MAC DA zeros, drop")
            pkt = simple_tcp_packet(
                eth_dst='00:00:00:00:00:00',
                eth_src='00:01:00:00:00:12',
                ip_dst='10.10.10.1',
                ip_id=108,
                ip_ttl=64)
            send_packet(self, self.devports[0], pkt)
            verify_no_other_packets(self, timeout=1)
            num_drops += 1

            print("MAC SA zeros, drop")
            pkt = simple_tcp_packet(
                eth_dst='00:01:00:00:00:11',
                eth_src='00:00:00:00:00:00',
                ip_dst='10.10.10.1',
                ip_id=108,
                ip_ttl=64)
            send_packet(self, self.devports[0], pkt)
            verify_no_other_packets(self, timeout=1)
            num_drops += 1

            print("MAC SA broadcast, drop")
            pkt = simple_tcp_packet(
                eth_dst='00:01:00:00:00:11',
                eth_src='ff:ff:ff:ff:ff:ff',
                ip_dst='10.10.10.1',
                ip_id=108,
                ip_ttl=64)
            send_packet(self, self.devports[0], pkt)
            verify_no_other_packets(self, timeout=1)
            num_drops += 1

            print("MAC SA multicast, drop")
            pkt = simple_tcp_packet(
                eth_dst='00:01:00:00:00:11',
                eth_src='01:00:5e:00:00:01',
                ip_dst='10.10.10.1',
                ip_id=108,
                ip_ttl=64)
            send_packet(self, self.devports[0], pkt)
            verify_no_other_packets(self, timeout=1)
            num_drops += 1

            for ihl in [0,1,2,3,4]:
                print("IPv4 IHL {}, forward".format(ihl))
                pkt = simple_tcp_packet(
                    eth_dst='00:01:00:00:00:22',
                    eth_src='00:01:00:00:00:12',
                    ip_dst='10.10.10.1',
                    ip_ihl=ihl)
                send_packet(self, self.devports[0], pkt)
                verify_packet(self, pkt, self.devports[1])
                flood_pkt = simple_tcp_packet(
                    eth_dst='00:01:00:00:00:62',
                    eth_src='00:01:00:00:00:12',
                    ip_dst='10.10.10.1',
                    ip_ihl=ihl)
                send_packet(self, self.devports[0], flood_pkt)
                verify_packets(self, flood_pkt, [self.devports[1], self.devports[2]])

            print("IPv4 TTL 0, forward")
            pkt = simple_tcp_packet(
                eth_dst='00:01:00:00:00:22',
                eth_src='00:01:00:00:00:11',
                ip_dst='10.10.10.1',
                ip_id=108,
                ip_ttl=0)
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, pkt, self.devports[1])
            flood_pkt = simple_tcp_packet(
                eth_dst='00:01:00:00:00:62',
                eth_src='00:01:00:00:00:12',
                ip_dst='10.10.10.1',
                ip_ttl=0)
            send_packet(self, self.devports[0], flood_pkt)
            verify_packets(self, flood_pkt, [self.devports[1], self.devports[2]])

            '''
            print("IPv4 invalid checksum, drop")
            pkt = simple_tcp_packet(
                eth_dst='00:01:00:00:00:22',
                eth_src='00:01:00:00:00:11',
                ip_dst='172.17.10.1',
                ip_id=108,
                ip_ttl=64)
            pkt[IP].chksum = 0
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, pkt, self.devports[1])
            '''

            print("IPv4 invalid version, forward")
            pkt = simple_tcp_packet(
                eth_dst='00:01:00:00:00:22',
                eth_src='00:01:00:00:00:11',
                ip_dst='10.10.10.1',
                ip_id=108,
                ip_ttl=64)
            pkt[IP].version = 6
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, pkt, self.devports[1])
            flood_pkt = simple_tcp_packet(
                eth_dst='00:01:00:00:00:62',
                eth_src='00:01:00:00:00:12',
                ip_dst='10.10.10.1',
                ip_ttl=64)
            flood_pkt[IP].version = 6
            send_packet(self, self.devports[0], flood_pkt)
            verify_packets(self, flood_pkt, [self.devports[1], self.devports[2]])

            print("IPv4 src is loopback, forward")
            pkt = simple_tcp_packet(
                eth_dst='00:01:00:00:00:22',
                eth_src='00:01:00:00:00:12',
                ip_src='127.10.10.1',
                ip_dst='10.10.10.1',
                ip_id=108,
                ip_ttl=64)
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, pkt, self.devports[1])
            flood_pkt = simple_tcp_packet(
                eth_dst='00:01:00:00:00:62',
                eth_src='00:01:00:00:00:12',
                ip_src='127.10.10.1',
                ip_dst='10.10.10.1',
                ip_ttl=64)
            send_packet(self, self.devports[0], flood_pkt)
            verify_packets(self, flood_pkt, [self.devports[1], self.devports[2]])

            print("IPv4 dst is loopback, forward")
            pkt = simple_tcp_packet(
                eth_dst='00:01:00:00:00:22',
                eth_src='00:01:00:00:00:12',
                ip_src='10.10.10.1',
                ip_dst='127.10.10.1',
                ip_ttl=64)
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, pkt, self.devports[1])
            flood_pkt = simple_tcp_packet(
                eth_dst='00:01:00:00:00:62',
                eth_src='00:01:00:00:00:12',
                ip_src='10.10.10.1',
                ip_dst='127.10.10.1',
                ip_ttl=64)
            send_packet(self, self.devports[0], flood_pkt)
            verify_packets(self, flood_pkt, [self.devports[1], self.devports[2]])

            print("IPv4 src class E, forward")
            pkt = simple_tcp_packet(
                eth_dst='00:01:00:00:00:22',
                eth_src='00:01:00:00:00:12',
                ip_dst='10.10.10.1',
                ip_src='255.0.0.1',
                ip_ttl=64)
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, pkt, self.devports[1])
            flood_pkt = simple_tcp_packet(
                eth_dst='00:01:00:00:00:62',
                eth_src='00:01:00:00:00:12',
                ip_dst='10.10.10.1',
                ip_src='255.0.0.1',
                ip_ttl=64)
            send_packet(self, self.devports[0], flood_pkt)
            verify_packets(self, flood_pkt, [self.devports[1], self.devports[2]])

            print("IPv4 dst unspecified, forward")
            pkt = simple_tcp_packet(
                eth_dst='00:01:00:00:00:22',
                eth_src='00:01:00:00:00:12',
                ip_dst='0.0.0.0',
                ip_ttl=64)
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, pkt, self.devports[1])
            flood_pkt = simple_tcp_packet(
                eth_dst='00:01:00:00:00:62',
                eth_src='00:01:00:00:00:12',
                ip_dst='0.0.0.0',
                ip_ttl=64)
            send_packet(self, self.devports[0], flood_pkt)
            verify_packets(self, flood_pkt, [self.devports[1], self.devports[2]])

            print("IPv4 src unspecified, forward")
            pkt = simple_tcp_packet(
                eth_dst='00:01:00:00:00:22',
                eth_src='00:01:00:00:00:12',
                ip_dst='10.10.10.1',
                ip_src='0.0.0.0',
                ip_ttl=64)
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, pkt, self.devports[1])
            flood_pkt = simple_tcp_packet(
                eth_dst='00:01:00:00:00:62',
                eth_src='00:01:00:00:00:12',
                ip_dst='10.10.10.1',
                ip_src='0.0.0.0',
                ip_ttl=64)
            send_packet(self, self.devports[0], flood_pkt)
            verify_packets(self, flood_pkt, [self.devports[1], self.devports[2]])

            print("IP SA IP multicast, forward")
            pkt = simple_tcp_packet(
                eth_dst='00:01:00:00:00:22',
                eth_src='00:01:00:00:00:11',
                ip_dst='10.10.10.1',
                ip_src='224.0.0.1',
                ip_ttl=64)
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, pkt, self.devports[1])
            flood_pkt = simple_tcp_packet(
                eth_dst='00:01:00:00:00:62',
                eth_src='00:01:00:00:00:11',
                ip_dst='10.10.10.1',
                ip_src='224.0.0.1',
                ip_ttl=64)
            send_packet(self, self.devports[0], flood_pkt)
            verify_packets(self, flood_pkt, [self.devports[1], self.devports[2]])

            print("Port vlan mapping miss, drop")
            pkt = simple_tcp_packet(
                eth_dst='00:01:00:00:00:22',
                eth_src='00:01:00:00:00:11',
                dl_vlan_enable=True,
                vlan_vid=20,
                ip_dst='10.10.10.1',
                ip_id=108,
                ip_ttl=64)
            send_packet(self, self.devports[0], pkt)
            verify_no_other_packets(self, timeout=1)
            num_drops += 1

            print("Port vlan mapping miss, lag, drop")
            pkt = simple_tcp_packet(
                eth_dst='00:01:00:00:00:52',
                eth_src='00:01:00:00:00:42',
                dl_vlan_enable=True,
                vlan_vid=10,
                ip_dst='10.10.10.1',
                ip_id=108,
                ip_ttl=64)
            send_packet(self, self.devports[3], pkt)
            verify_no_other_packets(self, timeout=1)
            send_packet(self, self.devports[4], pkt)
            verify_no_other_packets(self, timeout=1)

            print("MAC DA is reserved, drop")
            pkt = simple_eth_packet(eth_dst='01:80:C2:00:00:02', pktlen=100)
            send_packet(self, self.devports[0], pkt)
            verify_no_other_packets(self, timeout=1)
            num_drops += 1

            print("Sleeping for 5 sec before fetching stats")
            time.sleep(5)
            try:
                final_stats = self.client.object_counters_get(self.port0)

                x = SWITCH_PORT_COUNTER_ID_IF_IN_DISCARDS
                count = final_stats[x].count - initial_stats[x].count

                x = SWITCH_PORT_COUNTER_ID_IF_IN_VLAN_DISCARDS
                vlan_drop = final_stats[x].count - initial_stats[x].count

                x = SWITCH_PORT_COUNTER_ID_IF_IN_OUTER_SMAC_MULTICAST_DISCARDS
                smac_multicast_drop = final_stats[x].count - initial_stats[x].count

                x = SWITCH_PORT_COUNTER_ID_IF_IN_OUTER_SMAC_ZERO_DISCARDS
                smac_zero_drop = final_stats[x].count - initial_stats[x].count

                x = SWITCH_PORT_COUNTER_ID_IF_IN_OUTER_DMAC_ZERO_DISCARDS
                dmac_zero_drop = final_stats[x].count - initial_stats[x].count

                x = SWITCH_PORT_COUNTER_ID_IF_IN_DMAC_RESERVED_DISCARDS
                Reserved_DMAC_drop = final_stats[x].count - initial_stats[x].count

                x = SWITCH_PORT_COUNTER_ID_IF_IN_OUTER_IP_SRC_MULTICAST_DISCARDS
                sa_ip_multicast_drop = final_stats[x].count - initial_stats[x].count

                x = SWITCH_PORT_COUNTER_ID_IF_IN_OUTER_IP_VERSION_INVALID_DISCARDS
                invalid_ip_version_drop = final_stats[x].count - initial_stats[x].count

                x = SWITCH_PORT_COUNTER_ID_IF_IN_OUTER_IP_CHECKSUM_INVALID_DISCARDS
                invalid_ip_checksum_drop = final_stats[x].count - initial_stats[x].count

                x = SWITCH_PORT_COUNTER_ID_IF_IN_IP_TTL_ZERO_DISCARDS
                invalid_ttl_drop = final_stats[x].count - initial_stats[x].count

                x = SWITCH_PORT_COUNTER_ID_IF_IN_SIP_LOOPBACK_DISCARDS
                src_loopback_drop = final_stats[x].count - initial_stats[x].count

                x = SWITCH_PORT_COUNTER_ID_IF_IN_DIP_LOOPBACK_DISCARDS
                dst_loopback_drop = final_stats[x].count - initial_stats[x].count

                x = SWITCH_PORT_COUNTER_ID_IF_IN_OUTER_IP_IHL_DISCARDS
                ihl_drop = final_stats[x].count - initial_stats[x].count

                x = SWITCH_PORT_COUNTER_ID_IF_IN_OUTER_SAME_MAC_CHECK_DISCARDS
                same_mac_drop = final_stats[x].count - initial_stats[x].count

                x = SWITCH_PORT_COUNTER_ID_IF_IN_SAME_IFINDEX_DISCARDS
                same_if_index_drop = final_stats[x].count - initial_stats[x].count

                x = SWITCH_PORT_COUNTER_ID_IF_IN_IP_SRC_CLASS_E_DISCARDS
                sip_class_e_drop = final_stats[x].count - initial_stats[x].count

                x = SWITCH_PORT_COUNTER_ID_IF_IN_DIP_UNSPECIFIED_DISCARDS
                dip_unspecified_drop = final_stats[x].count - initial_stats[x].count

                x = SWITCH_PORT_COUNTER_ID_IF_IN_SIP_UNSPECIFIED_DISCARDS
                sip_unspecified_drop = final_stats[x].count - initial_stats[x].count

                print("Expected drop count : %d" % num_drops)
                print("Final drop count    : %d" % count)
                print("Vlan drop count     : %d" % vlan_drop)
                print("SMAC multicast drop : %d" % smac_multicast_drop)
                print("SMAC zero           : %d" % smac_zero_drop)
                print("DMAC zero           : %d" % dmac_zero_drop)
                print("MAC DA reserved     : %d" % Reserved_DMAC_drop)
                print("SA IP multicast     : %d" % sa_ip_multicast_drop)
                print("SA IP class E drop  : %d" % sip_class_e_drop)
                print("Invalid ip checksum : %d" % invalid_ip_checksum_drop)
                print("Invalid ip version  : %d" % invalid_ip_version_drop)
                print("Invalid TTL         : %d" % invalid_ttl_drop)
                print("IPv4 src is loopback: %d" % src_loopback_drop)
                print("IPv4 dst is loopback: %d" % dst_loopback_drop)
                print("IHL drop            : %d" % ihl_drop)
                print("Same outer MAC      : %d" % same_mac_drop)
                print("Same ifindex drop   : %d" % same_if_index_drop)
                print("SA IP unspecified   : %d" % sip_unspecified_drop)
                print("DA IP unspecified   : %d" % dip_unspecified_drop)

                self.assertEqual(num_drops, count)
                self.assertEqual(1, vlan_drop)
                self.assertEqual(2, smac_multicast_drop)
                self.assertEqual(1, smac_zero_drop)
                self.assertEqual(1, dmac_zero_drop)
                self.assertEqual(1, Reserved_DMAC_drop)
                self.assertEqual(0, sa_ip_multicast_drop)
                self.assertEqual(0, invalid_ip_version_drop)
                self.assertEqual(0, invalid_ip_checksum_drop)
                self.assertEqual(0, invalid_ttl_drop)
                self.assertEqual(0, src_loopback_drop)
                self.assertEqual(0, dst_loopback_drop)
                self.assertEqual(0, ihl_drop)
                self.assertEqual(0, sip_class_e_drop)
                self.assertEqual(0, sip_unspecified_drop)
                self.assertEqual(0, dip_unspecified_drop)
                if (self.client.is_feature_enable(SWITCH_FEATURE_SAME_MAC_CHECK) != 0):
                    self.assertEqual(1, same_mac_drop)
                self.assertEqual(1, same_if_index_drop)
                if (post_hitless):
                    return final_stats[SWITCH_PORT_COUNTER_ID_IF_IN_DISCARDS].count
                else:
                    return count

            finally:
                pass
        finally:
            pass

    def tearDown(self):
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.port2, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.cleanup()

###############################################################################

@group('l3')
class IPv4MalformedPacketsTest(ApiHelper):
    '''
    This test tries to verify routed packets due to switch drop reasons
    We send a packet from port 0 and forward/drop depending on packet contents
    '''
    def setUp(self):
        print()
        print('Configuring devices for ipv4 routed malformed packet test cases')
        self.configure()

        rif1 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT,
           port_handle=self.port0, vrf_handle=self.vrf10, src_mac=self.rmac)
        rif2 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT,
           port_handle=self.port1, vrf_handle=self.vrf10, src_mac=self.rmac)
        rif_subport = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_SUB_PORT,
           port_handle=self.port0, outer_vlan_id=100, vrf_handle=self.vrf10, src_mac=self.rmac)

        nhop2 = self.add_nexthop(self.device, handle=rif2, dest_ip='11.11.11.2')
        neighbor2 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:22', handle=rif2, dest_ip='11.11.11.2')
        route2 = self.add_route(self.device, ip_prefix='11.11.11.0/24', vrf_handle=self.vrf10, nexthop_handle=nhop2)
        route3 = self.add_route(self.device, ip_prefix='13.11.11.0/24', vrf_handle=self.vrf10, packet_action=SWITCH_ROUTE_ATTR_PACKET_ACTION_DROP)

        self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_LEARNING, False)
        vlan10_rif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_VLAN, vlan_handle=self.vlan10, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port4)
        self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port5)
        self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port6)
        self.attribute_set(self.port4, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.attribute_set(self.port5, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.attribute_set(self.port6, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)

        self.attribute_set(self.vlan20, SWITCH_VLAN_ATTR_LEARNING, False)
        vlan20_rif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_VLAN, vlan_handle=self.vlan20, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.add_vlan_member(self.device, vlan_handle=self.vlan20, member_handle=self.port7)
        self.attribute_set(self.port7, SWITCH_PORT_ATTR_PORT_VLAN_ID, 20)

    def runTest(self, post_hitless=False):
        try:
            initial_stats = self.client.object_counters_get(self.port0)
            initial_stats_port1 = self.client.object_counters_get(self.port1)
            num_drops = 0

            print("Valid packet from port %d to %d" % (self.devports[0], self.devports[1]))
            pkt = simple_tcp_packet(
                eth_dst=self.rmac,
                eth_src='00:01:00:00:00:12',
                ip_dst='11.11.11.1',
                ip_ttl=64,
                pktlen=100)
            tagged_pkt = simple_tcp_packet(
                eth_dst=self.rmac,
                eth_src='00:01:00:00:00:12',
                ip_dst='11.11.11.1',
                dl_vlan_enable=True,
                vlan_vid=100,
                ip_ttl=64,
                pktlen=104)
            exp_pkt = simple_tcp_packet(
                eth_dst='00:11:22:33:44:22',
                eth_src=self.rmac,
                ip_dst='11.11.11.1',
                ip_ttl=63,
                pktlen=100)
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt, self.devports[1])
            send_packet(self, self.devports[4], pkt)
            verify_packet(self, exp_pkt, self.devports[1])
            send_packet(self, self.devports[5], pkt)
            verify_packet(self, exp_pkt, self.devports[1])
            send_packet(self, self.devports[6], pkt)
            verify_packet(self, exp_pkt, self.devports[1])
            send_packet(self, self.devports[7], pkt)
            verify_packet(self, exp_pkt, self.devports[1])
            send_packet(self, self.devports[0], tagged_pkt)
            verify_packet(self, exp_pkt, self.devports[1])
            
            print("MTU Exceeds, drop")
            mtu_pkt = simple_tcp_packet(
                eth_dst=self.rmac,
                eth_src='00:01:00:00:00:12',
                ip_dst='11.11.11.1',
                ip_ttl=64,
                pktlen=10000)
            if self.test_params['target'] != 'hw':
                send_packet(self, self.devports[0], mtu_pkt)
                verify_no_other_packets(self, timeout=1)
            
            for ihl in [0,1,2,3,4]:
                print("IPv4 IHL {}, drop".format(ihl))
                pkt = simple_tcp_packet(
                    eth_dst=self.rmac,
                    eth_src='00:01:00:00:00:12',
                    ip_dst='11.11.11.1',
                    ip_ihl=ihl)
                send_packet(self, self.devports[0], pkt)
                verify_no_other_packets(self, timeout=1)
                num_drops += 1

            print("IPv4 TTL 0, drop")
            pkt = simple_tcp_packet(
                eth_dst=self.rmac,
                eth_src='00:01:00:00:00:12',
                ip_dst='11.11.11.1',
                ip_id=108,
                ip_ttl=0)
            send_packet(self, self.devports[0], pkt)
            verify_no_other_packets(self, timeout=1)
            num_drops += 1

            print("IPv4 invalid checksum, drop")
            pkt = simple_tcp_packet(
                eth_dst=self.rmac,
                eth_src='00:01:00:00:00:12',
                ip_dst='11.11.11.1',
                ip_id=108,
                ip_ttl=64)
            pkt[IP].chksum = 0
            send_packet(self, self.devports[0], pkt)
            verify_no_other_packets(self, timeout=1)
            num_drops += 1

            print("IPv4 invalid version, drop")
            pkt = simple_tcp_packet(
                eth_dst=self.rmac,
                eth_src='00:01:00:00:00:12',
                ip_dst='11.11.11.1',
                ip_id=108,
                ip_ttl=64)
            pkt[IP].version = 6
            send_packet(self, self.devports[0], pkt)
            verify_no_other_packets(self, timeout=1)
            num_drops += 1

            print("IPv4 src is loopback, drop")
            pkt = simple_tcp_packet(
                eth_dst=self.rmac,
                eth_src='00:01:00:00:00:12',
                ip_src='127.10.10.1',
                ip_dst='10.10.10.1',
                ip_id=108,
                ip_ttl=64)
            send_packet(self, self.devports[0], pkt)
            verify_no_other_packets(self, timeout=1)
            num_drops += 1

            print("IPv4 dst is loopback, drop")
            pkt = simple_tcp_packet(
                eth_dst=self.rmac,
                eth_src='00:01:00:00:00:12',
                ip_src='10.10.10.1',
                ip_dst='127.10.10.1',
                ip_id=108,
                ip_ttl=64)
            send_packet(self, self.devports[0], pkt)
            verify_no_other_packets(self, timeout=1)
            num_drops += 1

            print("IPv4 src class E, drop")
            pkt = simple_tcp_packet(
                eth_dst=self.rmac,
                eth_src='00:01:00:00:00:12',
                ip_dst='10.10.10.1',
                ip_src='255.0.0.1',
                ip_id=108,
                ip_ttl=64)
            send_packet(self, self.devports[0], pkt)
            verify_no_other_packets(self, timeout=1)
            num_drops += 1

            print("IPv4 dst unspecified, drop")
            pkt = simple_tcp_packet(
                eth_dst=self.rmac,
                eth_src='00:01:00:00:00:12',
                ip_dst='0.0.0.0',
                ip_id=108,
                ip_ttl=64)
            send_packet(self, self.devports[0], pkt)
            verify_no_other_packets(self, timeout=1)
            num_drops += 1

            print("IPv4 src unspecified, drop")
            pkt = simple_tcp_packet(
                eth_dst=self.rmac,
                eth_src='00:01:00:00:00:11',
                ip_dst='11.11.11.1',
                ip_src='0.0.0.0',
                ip_id=108,
                ip_ttl=64)
            send_packet(self, self.devports[0], pkt)
            verify_no_other_packets(self, timeout=1)
            num_drops += 1

            print("IP SA IP multicast, drop")
            pkt = simple_tcp_packet(
                eth_dst=self.rmac,
                eth_src='00:01:00:00:00:12',
                ip_dst='10.10.10.1',
                ip_src='224.0.0.1',
                ip_id=108,
                ip_ttl=64)
            send_packet(self, self.devports[0], pkt)
            verify_no_other_packets(self, timeout=1)
            num_drops += 1

            print("IPv4 LPM miss, drop")
            pkt = simple_tcp_packet(
                eth_dst=self.rmac,
                eth_src='00:01:00:00:00:12',
                ip_dst='12.11.11.1',
                ip_id=108,
                ip_ttl=64)
            send_packet(self, self.devports[0], pkt)
            verify_no_other_packets(self, timeout=1)
            num_drops += 1

            print("IPv6 LPM miss, drop")
            pkt = simple_tcpv6_packet(
                eth_dst=self.rmac,
                eth_src='00:22:22:22:22:22',
                ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
                ipv6_src='2000::1',
                ipv6_hlim=64)
            send_packet(self, self.devports[0], pkt)
            verify_no_other_packets(self, timeout=1)
            num_drops += 1

            print("IPv4 blackhole route, drop")
            pkt = simple_tcp_packet(
                eth_dst=self.rmac,
                eth_src='00:01:00:00:00:12',
                ip_dst='13.11.11.1',
                ip_id=108,
                ip_ttl=64)
            send_packet(self, self.devports[0], pkt)
            verify_no_other_packets(self, timeout=1)
            num_drops += 1

            print("IPv4 L3 port non rmac, drop")
            pkt = simple_tcp_packet(
                eth_dst='00:00:00:22:22:22',
                eth_src='00:01:00:00:00:11',
                ip_dst='11.11.11.1',
                ip_ttl=64)
            send_packet(self, self.devports[0], pkt)
            verify_no_other_packets(self, timeout=1)
            num_drops += 1

            print("IPv4 L3 sub port non rmac, drop")
            pkt = simple_tcp_packet(
                eth_dst='00:00:00:22:22:22',
                ip_dst='11.11.11.1',
                dl_vlan_enable=True,
                vlan_vid=100,
                ip_ttl=64)
            send_packet(self, self.devports[0], pkt)
            verify_no_other_packets(self, timeout=1)
            num_drops += 1

            print("IPv4 SVI vlan10 non rmac, flood")
            pkt = simple_tcp_packet(
                eth_dst='00:00:00:22:22:22',
                ip_dst='11.11.11.1',
                ip_ttl=64)
            send_packet(self, self.devports[4], pkt)
            verify_packets(self, pkt, [self.devports[5], self.devports[6]])

            print("IPv4 SVI vlan20 non rmac, drop")
            pkt = simple_tcp_packet(
                eth_dst='00:00:00:22:22:22',
                eth_src='00:01:00:00:00:11',
                ip_dst='11.11.11.1',
                ip_ttl=64)
            send_packet(self, self.devports[7], pkt)
            verify_no_other_packets(self, timeout=1)
            # not really a drop. just no where to flood the packet.
            # num_drops += 1

            print("IPv4 src limited BC, drop")
            pkt = simple_tcp_packet(
                eth_dst=self.rmac,
                eth_src='00:01:00:00:00:11',
                ip_dst='11.11.11.1',
                ip_src='255.255.255.255',
                ip_id=108,
                ip_ttl=64)
            send_packet(self, self.devports[0], pkt)
            verify_no_other_packets(self, timeout=1)
            num_drops += 1

            print("Sleeping for 5 sec before fetching stats")
            time.sleep(5)
            try:
                final_stats = self.client.object_counters_get(self.port0)
                final_stats_port1 = self.client.object_counters_get(self.port1)
                x = SWITCH_PORT_COUNTER_ID_IF_IN_DISCARDS
                final_count = final_stats[x].count - initial_stats[x].count

                x = SWITCH_PORT_COUNTER_ID_IF_IN_OUTER_IP_SRC_MULTICAST_DISCARDS
                sa_ip_multicast_drop = final_stats[x].count - initial_stats[x].count

                x = SWITCH_PORT_COUNTER_ID_IF_IN_OUTER_IP_VERSION_INVALID_DISCARDS
                invalid_ip_version_drop = final_stats[x].count - initial_stats[x].count

                x = SWITCH_PORT_COUNTER_ID_IF_IN_OUTER_IP_CHECKSUM_INVALID_DISCARDS
                invalid_ip_checksum_drop = final_stats[x].count - initial_stats[x].count

                x = SWITCH_PORT_COUNTER_ID_IF_IN_OUTER_IP_TTL_ZERO_DISCARDS
                invalid_ttl_drop = final_stats[x].count - initial_stats[x].count

                x = SWITCH_PORT_COUNTER_ID_IF_IN_SIP_LOOPBACK_DISCARDS
                src_loopback_drop = final_stats[x].count - initial_stats[x].count

                x = SWITCH_PORT_COUNTER_ID_IF_IN_DIP_LOOPBACK_DISCARDS
                dst_loopback_drop = final_stats[x].count - initial_stats[x].count

                x = SWITCH_PORT_COUNTER_ID_IF_IN_OUTER_IP_IHL_DISCARDS
                ihl_drop = final_stats[x].count - initial_stats[x].count

                x = SWITCH_PORT_COUNTER_ID_IF_IN_IP_SRC_CLASS_E_DISCARDS
                sip_class_e_drop = final_stats[x].count - initial_stats[x].count

                x = SWITCH_PORT_COUNTER_ID_IF_IN_DIP_UNSPECIFIED_DISCARDS
                dip_unspecified_drop = final_stats[x].count - initial_stats[x].count

                x = SWITCH_PORT_COUNTER_ID_IF_IN_SIP_UNSPECIFIED_DISCARDS
                sip_unspecified_drop = final_stats[x].count - initial_stats[x].count

                x = SWITCH_PORT_COUNTER_ID_IF_IN_IP_LPM4_MISS_DISCARDS
                lpm4_miss_drop = final_stats[x].count - initial_stats[x].count

                x = SWITCH_PORT_COUNTER_ID_IF_IN_IP_LPM6_MISS_DISCARDS
                lpm6_miss_drop = final_stats[x].count - initial_stats[x].count

                x = SWITCH_PORT_COUNTER_ID_IF_IN_IP_BLACKHOLE_ROUTE_DISCARDS
                blackhole_route_drop = final_stats[x].count - initial_stats[x].count

                x = SWITCH_PORT_COUNTER_ID_IF_IN_L3_PORT_RMAC_MISS_DISCARDS
                l3_port_rmac_miss_drop = final_stats[x].count - initial_stats[x].count

                x = SWITCH_PORT_COUNTER_ID_IF_IN_FDB_AND_BLACKHOLE_DISCARDS
                fdb_blackhole_route_drop = final_stats[x].count - initial_stats[x].count

                x = SWITCH_PORT_COUNTER_ID_IF_IN_SIP_BC_DISCARDS
                sip_bc_drop = final_stats[x].count - initial_stats[x].count

                x = SWITCH_PORT_COUNTER_ID_IF_OUT_MTU_CHECK_FAIL_DISCARDS
                mtu_check_discards = final_stats_port1[x].count - initial_stats_port1[x].count

                print("Expected drop count : %d" % num_drops)
                print("Final drop count    : %d" % final_count)
                print("SA IP multicast     : %d" % sa_ip_multicast_drop)
                print("SA IP class E drop  : %d" % sip_class_e_drop)
                print("Invalid ip checksum : %d" % invalid_ip_checksum_drop)
                print("Invalid ip version  : %d" % invalid_ip_version_drop)
                print("Invalid TTL         : %d" % invalid_ttl_drop)
                print("IPv4 src is loopback: %d" % src_loopback_drop)
                print("IPv4 dst is loopback: %d" % dst_loopback_drop)
                print("IHL drop            : %d" % ihl_drop)
                print("DA IP unspecified   : %d" % dip_unspecified_drop)
                print("SA IP unspecified   : %d" % sip_unspecified_drop)
                print("IP LPM4 miss        : %d" % lpm4_miss_drop)
                print("IP LPM6 miss        : %d" % lpm4_miss_drop)
                print("Blackhole route     : %d" % blackhole_route_drop)
                print("L3 port rmac miss   : %d" % l3_port_rmac_miss_drop)
                print("FDB Blackhole route : %d" % fdb_blackhole_route_drop)
                print("SIP BC drop         : %d" % sip_bc_drop)
                print("MTU exceeds, drop   : %d" % mtu_check_discards)

                self.assertEqual(num_drops, final_count)
                self.assertEqual(1, sa_ip_multicast_drop)
                self.assertEqual(1, invalid_ip_version_drop)
                self.assertEqual(1, invalid_ip_checksum_drop)
                self.assertEqual(1, invalid_ttl_drop)
                self.assertEqual(1, src_loopback_drop)
                self.assertEqual(1, dst_loopback_drop)
                self.assertEqual(5, ihl_drop)
                self.assertEqual(1, sip_class_e_drop)
                self.assertEqual(1, dip_unspecified_drop)
                self.assertEqual(1, sip_unspecified_drop)
                self.assertEqual(1, lpm4_miss_drop)
                self.assertEqual(1, lpm6_miss_drop)
                self.assertEqual(1, blackhole_route_drop)
                self.assertEqual(2, l3_port_rmac_miss_drop)
                self.assertEqual(1, sip_bc_drop)
                if self.test_params['target'] != 'hw':
                    self.assertEqual(1, mtu_check_discards)

                if (post_hitless):
                    return final_stats[SWITCH_PORT_COUNTER_ID_IF_IN_DISCARDS].count
                else:
                    return final_count

            finally:
                pass
        finally:
            pass

    def tearDown(self):
        self.attribute_set(self.port7, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.port4, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.port5, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.port6, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.cleanup()

###############################################################################

@group('l3')
class IPv6MalformedPacketsTest(ApiHelper):
    '''
    This test tries to verify packets using validate_ipv6 table
    We send a packet from port 0 and forward/drop depending on packet contents
    '''
    def setUp(self):
        print()
        print('Configuring devices for ipv6 malformed packet test cases')
        self.configure()

        rif0 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT,
           port_handle=self.port0, vrf_handle=self.vrf10, src_mac=self.rmac)
        rif1 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT,
           port_handle=self.port1, vrf_handle=self.vrf10, src_mac=self.rmac)

        nhop = self.add_nexthop(self.device, handle=rif1, dest_ip='2000::1')
        self.add_neighbor(self.device, mac_address='00:11:22:33:44:22', handle=rif1, dest_ip='2000::1')
        self.add_route(self.device, ip_prefix='4000::1', vrf_handle=self.vrf10, nexthop_handle=nhop)

    def runTest(self, post_hitless=False):
        try:
            initial_stats = self.client.object_counters_get(self.port0)
            num_drops = 0

            print("Valid packet from port %d to %d" % (self.devports[0], self.devports[1]))
            pkt = simple_tcpv6_packet(
                eth_dst=self.rmac,
                eth_src='00:22:22:22:22:22',
                ipv6_dst='4000::1',
                ipv6_src='4000::2',
                ipv6_hlim=64)
            exp_pkt = simple_tcpv6_packet(
                eth_dst='00:11:22:33:44:22',
                eth_src=self.rmac,
                ipv6_dst='4000::1',
                ipv6_src='4000::2',
                ipv6_hlim=63)
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt, self.devports[1])

            print("IPv6 TTL 0, drop")
            pkt = simple_tcpv6_packet(
                eth_dst=self.rmac,
                eth_src='00:22:22:22:22:22',
                ipv6_dst='4000::1',
                ipv6_src='4000::2',
                ipv6_hlim=0)
            send_packet(self, self.devports[0], pkt)
            verify_no_other_packets(self, timeout=1)
            num_drops += 1

            print("IPv6 invalid version, drop")
            pkt = simple_tcpv6_packet(
                eth_dst=self.rmac,
                eth_src='00:22:22:22:22:22',
                ipv6_dst='4000::1',
                ipv6_src='4000::2',
                ipv6_hlim=64)
            pkt[IPv6].version = 4
            send_packet(self, self.devports[0], pkt)
            verify_no_other_packets(self, timeout=1)
            num_drops += 1

            print("IPv6 src multicast, drop")
            pkt = simple_tcpv6_packet(
                eth_dst=self.rmac,
                eth_src='00:22:22:22:22:22',
                ipv6_dst='4000::1',
                ipv6_src='ff02::1')
            send_packet(self, self.devports[0], pkt)
            verify_no_other_packets(self, timeout=1)
            num_drops += 1

            print("IPv6 dst is loopback 1, drop")
            pkt = simple_tcpv6_packet(
                eth_dst=self.rmac,
                eth_src='00:22:22:22:22:22',
                ipv6_dst='::1',
                ipv6_src='2000::1',
                ipv6_hlim=64)
            send_packet(self, self.devports[0], pkt)
            verify_no_other_packets(self, timeout=1)
            num_drops += 1

            print("IPv6 dst is loopback 2, drop")
            pkt = simple_tcpv6_packet(
                eth_dst=self.rmac,
                eth_src='00:22:22:22:22:22',
                ipv6_dst='::1',
                ipv6_src='0:0:0:0:0:ffff:7f01:0',
                ipv6_hlim=64)
            send_packet(self, self.devports[0], pkt)
            verify_no_other_packets(self, timeout=1)
            num_drops += 1

            print("IPv6 dst is unspecified, drop")
            pkt = simple_tcpv6_packet(
                eth_dst=self.rmac,
                eth_src='00:22:22:22:22:22',
                ipv6_dst='::0',
                ipv6_hlim=64)
            send_packet(self, self.devports[0], pkt)
            verify_no_other_packets(self, timeout=1)
            num_drops += 1

            print("IPv6 src is unspecified, drop")
            pkt = simple_tcpv6_packet(
                eth_dst=self.rmac,
                eth_src='00:22:22:22:22:22',
                ipv6_dst='4000::1',
                ipv6_src='::0')
            send_packet(self, self.devports[0], pkt)
            verify_no_other_packets(self, timeout=1)
            num_drops += 1

            print("IPv6 dst is FF:x0:/16, drop")
            pkt = simple_tcpv6_packet(
                eth_dst=self.rmac,
                eth_src='00:22:22:22:22:22',
                ipv6_dst='FF00:0:0:0:0:0:0:1',
                ipv6_hlim=64)
            send_packet(self, self.devports[0], pkt)
            verify_no_other_packets(self, timeout=1)
            num_drops += 1

            print("IPv6 dst is FF:x1:/16, drop")
            pkt = simple_tcpv6_packet(
                eth_dst=self.rmac,
                eth_src='00:22:22:22:22:22',
                ipv6_dst='FF01:0:0:0:0:0:0:1',
                ipv6_hlim=64)
            send_packet(self, self.devports[0], pkt)
            verify_no_other_packets(self, timeout=1)
            num_drops += 1

            print("Sleeping for 5 sec before fetching stats")
            time.sleep(5)
            try:
                final_stats = self.client.object_counters_get(self.port0)
                x = SWITCH_PORT_COUNTER_ID_IF_IN_DISCARDS
                final_count = final_stats[x].count - initial_stats[x].count
                print("Expected drop count: %d" % num_drops)
                print("Final drop count   : %d" % final_count)
                self.assertEqual(num_drops, final_count)

                x = SWITCH_PORT_COUNTER_ID_IF_IN_OUTER_IP_TTL_ZERO_DISCARDS
                invalid_ttl_drop = final_stats[x].count - initial_stats[x].count

                x = SWITCH_PORT_COUNTER_ID_IF_IN_OUTER_IP_VERSION_INVALID_DISCARDS
                invalid_ip_version_drop = final_stats[x].count - initial_stats[x].count

                x = SWITCH_PORT_COUNTER_ID_IF_IN_OUTER_IP_SRC_MULTICAST_DISCARDS
                sa_ip_multicast_drop = final_stats[x].count - initial_stats[x].count

                x = SWITCH_PORT_COUNTER_ID_IF_IN_DIP_LOOPBACK_DISCARDS
                dst_loopback_drop = final_stats[x].count - initial_stats[x].count

                x = SWITCH_PORT_COUNTER_ID_IF_IN_DIP_UNSPECIFIED_DISCARDS
                dip_unspecified_drop = final_stats[x].count - initial_stats[x].count

                x = SWITCH_PORT_COUNTER_ID_IF_IN_SIP_UNSPECIFIED_DISCARDS
                sip_unspecified_drop = final_stats[x].count - initial_stats[x].count

                x = SWITCH_PORT_COUNTER_ID_IF_IN_IPV6_MC_SCOPE0_DISCARD
                mc_scope0_drop_v6 = final_stats[x].count - initial_stats[x].count

                x = SWITCH_PORT_COUNTER_ID_IF_IN_IPV6_MC_SCOPE1_DISCARD
                mc_scope1_drop_v6 = final_stats[x].count - initial_stats[x].count

                print("Expected drop count : %d" % num_drops)
                print("Final drop count    : %d" % final_count)
                print("Invalid TTL         : %d" % invalid_ttl_drop)
                print("Invalid ip version  : %d" % invalid_ip_version_drop)
                print("SA IP multicast     : %d" % sa_ip_multicast_drop)
                print("IPv6 dst is loopback: %d" % dst_loopback_drop)
                print("DA IP unspecified   : %d" % dip_unspecified_drop)
                print("SA IP unspecified   : %d" % sip_unspecified_drop)
                print("MC scope0_drop_v6   : %d" % mc_scope0_drop_v6)
                print("MC scope1_drop_v6   : %d" % mc_scope1_drop_v6)

                self.assertEqual(num_drops, final_count)
                self.assertEqual(1, invalid_ttl_drop)
                self.assertEqual(1, invalid_ip_version_drop)
                self.assertEqual(1, sa_ip_multicast_drop)
                self.assertEqual(2, dst_loopback_drop)
                self.assertEqual(1, dip_unspecified_drop)
                self.assertEqual(1, sip_unspecified_drop)
                self.assertEqual(1, mc_scope0_drop_v6)
                self.assertEqual(1, mc_scope1_drop_v6)

                if (post_hitless):
                    return final_stats[SWITCH_PORT_COUNTER_ID_IF_IN_DISCARDS].count
                else:
                    return final_count

            finally:
                pass
        finally:
            pass

    def tearDown(self):
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.cleanup()

###############################################################################

@disabled
class SubmitToIngressTest(ApiHelper):

    def setUp(self):
        print()
        print('Configuring devices for CPU Tx test cases')
        self.configure()

        self.rif0 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port0, vrf_handle=self.vrf10, src_mac=self.rmac)

        self.rif1 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port1, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.nhop1 = self.add_nexthop(self.device, handle=self.rif1, dest_ip='33.33.33.1')
        self.neighbor1 = self.add_neighbor(self.device, mac_address='00:22:22:22:22:22', handle=self.rif1, dest_ip='33.33.33.1')

        self.rif2 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port2, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.nhop2 = self.add_nexthop(self.device, handle=self.rif2, dest_ip='33.33.33.2')
        self.neighbor2 = self.add_neighbor(self.device, mac_address='00:22:22:22:22:23', handle=self.rif2, dest_ip='33.33.33.2')

        self.add_route(self.device, ip_prefix='192.17.10.1', vrf_handle=self.default_vrf, nexthop_handle=self.nhop1)
        self.add_route(self.device, ip_prefix='172.17.10.1', vrf_handle=self.default_vrf, nexthop_handle=self.nhop1)
        self.add_route(self.device, ip_prefix='10.10.10.1', vrf_handle=self.default_vrf, nexthop_handle=self.nhop2)
        self.add_route(self.device, ip_prefix='10.10.10.1', vrf_handle=self.vrf10, nexthop_handle=self.nhop1)

        hostif_name = "send_to_ingress"
        self.send_to_ingress_hostif = self.add_hostif(self.device, name=hostif_name, handle=self.cpu_port_hdl, oper_status=True)
        self.assertTrue(self.send_to_ingress_hostif != 0)
        print("Sleeping for 5 secs")
        time.sleep(5)

        # open socket to verfiy packet recieved on hostif netd.
        if self.test_params['target'] != 'hw':
            self.sock = open_packet_socket(hostif_name)

    def ptfSendTest(self):
        print()
        try:
          pkt = simple_tcp_packet(
            eth_dst='00:11:11:11:11:11',
            eth_src='00:00:00:00:00:01',
            ip_dst='192.17.10.1',
            ip_ttl=64)
          cpu_pkt = simple_cpu_packet(
            dst_device=0,
            ingress_ifindex=0,
            ingress_port=0,
            ingress_bd=0,
            tx_bypass=False,
            reason_code=0x400,
            inner_pkt=pkt)
          exp_pkt = simple_tcp_packet(
            eth_dst='00:22:22:22:22:22',
            eth_src=self.rmac,
            ip_dst='192.17.10.1',
            ip_ttl=63)
          print("Sending packet from cpu port %d to port %d" % (self.cpu_port, self.devports[1]))
          send_packet(self, self.cpu_port, cpu_pkt)
          verify_packets(self, exp_pkt, [self.devports[1]])
        finally:
            pass

    def socketSendTest(self):
        print()
        try:
          pkt = simple_tcp_packet(
            eth_dst='00:11:11:11:11:11',
            eth_src='00:00:00:00:00:01',
            ip_dst='172.17.10.1',
            ip_ttl=64)
          exp_pkt = simple_tcp_packet(
            eth_dst='00:22:22:22:22:22',
            eth_src=self.rmac,
            ip_dst='172.17.10.1',
            ip_ttl=63)
          print("Sending packet from cpu port %d to port %d dst_ip 172.17.10.1" % (self.cpu_port, self.devports[1]))
          socket_send_packet(pkt, self.sock)
          verify_packets(self, exp_pkt, [self.devports[1]])

          pkt = simple_tcp_packet(
            eth_dst='00:11:11:11:11:11',
            eth_src='00:00:00:00:00:01',
            ip_dst='10.10.10.1',
            ip_ttl=64)
          exp_pkt = simple_tcp_packet(
            eth_dst='00:22:22:22:22:22',
            eth_src=self.rmac,
            ip_dst='10.10.10.1',
            ip_ttl=63)
          exp_pkt2 = simple_tcp_packet(
            eth_dst='00:22:22:22:22:23',
            eth_src=self.rmac,
            ip_dst='10.10.10.1',
            ip_ttl=63)

          print("Sending packet from cpu port %d to port %d dst_ip 10.10.10.1" % (self.cpu_port, self.devports[2]))
          socket_send_packet(pkt, self.sock)
          verify_packets(self, exp_pkt2, [self.devports[2]])

          print("Add pre ingress acl set vrf vrf10")
          acl_table_bp_switch = switcht_list_val_t(type=switcht_value_type.ENUM, enumdata=SWITCH_ACL_TABLE_ATTR_BIND_POINT_TYPE_SWITCH)
          acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_PREINGRESS,
            bind_point_type=[acl_table_bp_switch],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_PREINGRESS)
          acl_entry = self.add_acl_entry(self.device,
            action_set_vrf_handle=self.vrf10,
            table_handle=acl_table)
          self.attribute_set(self.device, SWITCH_DEVICE_ATTR_PRE_INGRESS_ACL, acl_table)
          print("Sending packet from cpu port %d to port %d dst_ip 10.10.10.1" % (self.cpu_port, self.devports[1]))
          socket_send_packet(pkt, self.sock)
          verify_packets(self, exp_pkt, [self.devports[1]])
        finally:
          self.attribute_set(self.device, SWITCH_DEVICE_ATTR_PRE_INGRESS_ACL, 0)
          self.cleanlast()
          self.cleanlast()
          pass

    def runTest(self):
        self.ptfSendTest()
        self.socketSendTest()

    def tearDown(self):
        self.cleanup()

###############################################################################

@group('cpu')
class CpuTxTest(ApiHelper):

    def qid_to_handle(self, port_handle, qid):
        queue_list = self.attribute_get(port_handle, SWITCH_PORT_ATTR_QUEUE_HANDLES)
        for queue_handle in queue_list:
            queue_id = self.attribute_get(queue_handle.oid, SWITCH_QUEUE_ATTR_QUEUE_ID)
            if qid == queue_id:
                return queue_handle.oid

        print("Invalid queue id: %d, queue_handle does not exists" % qid)
        return 0

    def get_queue_counter(self, queue_handle):
        queue_cntrs = self.client.object_counters_get(queue_handle)
        return queue_cntrs


    def verifyQueueStats(self, before_cntrs, after_cntrs, queue_cntr_type, expected_pkt_count=0, expected_bytes_count=0):
        before = 0
        after = 0
        for cntr in before_cntrs:
            if cntr.counter_id == queue_cntr_type:
                before =  cntr.count
                break
        for cntr in after_cntrs:
            if cntr.counter_id == queue_cntr_type:
                after =  cntr.count
                break
        packet_count = after-before
        print("Received : %d, expected: %d"%(packet_count, expected_pkt_count))
        return (packet_count == expected_pkt_count)

    '''
    Send packet from CPU port to
    '''
    def setUp(self):
        print()
        print('Configuring devices for CPU Tx test cases')
        self.configure()

        self.devport = self.attribute_get(self.port2, SWITCH_PORT_ATTR_DEV_PORT)
        self.rif0 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port0, vrf_handle=self.vrf10, src_mac=self.rmac)

        self.lag0 = self.add_lag(self.device)
        lag_mbr00 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port1)
        lag_mbr01 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port2)
        self.lag1 = self.add_lag(self.device)
        lag_mbr10 = self.add_lag_member(self.device, lag_handle=self.lag1, port_handle=self.port3)
        lag_mbr11 = self.add_lag_member(self.device, lag_handle=self.lag1, port_handle=self.port4)
        self.lag2 = self.add_lag(self.device)
        lag_mbr20 = self.add_lag_member(self.device, lag_handle=self.lag2, port_handle=self.port5)
        lag_mbr21 = self.add_lag_member(self.device, lag_handle=self.lag2, port_handle=self.port6)

        vlan_mbr0 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.lag0, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
        vlan_mbr1 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.lag1)
        vlan_mbr1 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port7)
        self.attribute_set(self.lag1, SWITCH_LAG_ATTR_PORT_VLAN_ID, 10)
        self.attribute_set(self.port7, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        vlan_mbr1 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port8, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)

        self.lag3 = self.add_lag(self.device)
        vlan_mbr30 = self.add_vlan_member(self.device, vlan_handle=self.vlan20, member_handle=self.lag3)
        self.attribute_set(self.lag3, SWITCH_LAG_ATTR_PORT_VLAN_ID, 20)
        mac1 = self.add_mac_entry(self.device, vlan_handle=self.vlan20, mac_address='00:55:52:52:52:52',
               destination_handle=self.lag3)

        mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:11:11:11:11:11', destination_handle=self.lag0)
        mac1 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:22:22:22:22:22', destination_handle=self.lag1)
        mac1 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:33:33:33:33:33', destination_handle=self.port7)
        mac1 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:44:44:44:44:44', destination_handle=self.port8)

        self.rif2 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_VLAN, vlan_handle=self.vlan10, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.nhop2 = self.add_nexthop(self.device, handle=self.rif2, dest_ip='33.33.33.1')
        self.neighbor2 = self.add_neighbor(self.device, mac_address='00:22:22:22:22:22', handle=self.rif2, dest_ip='33.33.33.1')
        self.route2 = self.add_route(self.device, ip_prefix='192.17.10.1', vrf_handle=self.vrf10, nexthop_handle=self.nhop2)
        self.nhop3 = self.add_nexthop(self.device, handle=self.rif2, dest_ip='34.34.34.1')
        self.neighbor3 = self.add_neighbor(self.device, mac_address='00:33:33:33:33:33', handle=self.rif2, dest_ip='34.34.34.1')
        self.route3 = self.add_route(self.device, ip_prefix='192.17.10.2', vrf_handle=self.vrf10, nexthop_handle=self.nhop3)

        self.tx_ports = [self.port9, self.port10, self.port11, self.port12, self.port13]
        self.begin_port_id = 9
        i = self.begin_port_id
        for port in self.tx_ports:
            lrif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=port, vrf_handle=self.vrf10, src_mac=self.rmac)
            lip = '44.44.44.' + str(self.devports[i])
            rip = '192.168.10.' + str(self.devports[i])
            nmac = '00:22:22:22:22:%02d' % self.devports[i]
            lnhop = self.add_nexthop(self.device, handle=lrif, dest_ip=lip)
            lneighbor = self.add_neighbor(self.device, mac_address=nmac, handle=lrif, dest_ip=lip)
            lroute = self.add_route(self.device, ip_prefix=rip, vrf_handle=self.vrf10, nexthop_handle=lnhop)
            i += 1


    def runTest(self):
        print()
        try:
            if self.test_params['target'] == 'hw':
                return
            self.CPUTxBypass()
            self.L2CPUTxAccessPortTest()
            self.L2CPUTxTrunkPortTest()
            self.L2CPUTxAccessLagTest()
            self.L2CPUTxTrunkLagTest()
            self.L2CPUTxFloodTest()
            self.L3CPUTxPortTest()
            self.L3CPUTxLagTest()
        finally:
            pass

    def tearDown(self):
        self.attribute_set(self.lag1, SWITCH_LAG_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.port7, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.lag2, SWITCH_LAG_ATTR_PORT_VLAN_ID, 0)
        self.cleanup()

    def CPUTxBypass(self):
        print("CPUTxBypass()")
        # regression for p4c2100
        i = self.begin_port_id
        for port in self.tx_ports:
            rip = '192.168.10.' + str(self.devports[i])
            nmac = '00:22:22:22:22:%02d' % self.devports[i]
            lpkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:66:66:66:66:66',
                ip_dst=rip,
                ip_src='192.168.0.1',
                ip_id=105,
                ip_ttl=64,
                pktlen=100)
            lexp_pkt = simple_tcp_packet(
                eth_dst=nmac,
                eth_src='00:77:66:55:44:33',
                ip_dst=rip,
                ip_src='192.168.0.1',
                ip_id=105,
                ip_ttl=63,
                pktlen=100)
            send_packet(self, self.devports[0], lpkt)
            verify_packet(self, lexp_pkt, self.devports[i])
            i += 1

        # L3 unicast packet to port
        pkt = simple_tcp_packet(
            eth_dst='00:11:11:11:11:11',
            eth_src='00:00:00:00:00:01',
            ip_dst='172.17.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        cpu_pkt = simple_cpu_packet(
            dst_device=0,
            ingress_ifindex=0xFFFF & self.port2,
            ingress_port=0,
            ingress_bd=0,
            tx_bypass=True,
            reason_code=0xFFFF,
            inner_pkt=pkt)
        try:
            print("Sending packet from cpu port %d to port %d" % (self.cpu_port, self.devports[2]))
            send_packet(self, self.cpu_port, cpu_pkt)
            verify_packets(self, pkt, [self.devports[2]])
        finally:
            pass

        # L3 unicast packet to LAG
        pkt = simple_tcp_packet(
            eth_dst='00:11:11:11:11:11',
            eth_src='00:00:00:00:00:01',
            ip_dst='172.17.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        port_lag_index = (1 << 9) | (0xFFFF & self.lag2)
        cpu_pkt = simple_cpu_packet(
            dst_device=0,
            ingress_ifindex=port_lag_index,
            ingress_port=0,
            ingress_bd=0,
            tx_bypass=True,
            reason_code=0xFFFF,
            inner_pkt=pkt)
        try:
            print("Sending packet from cpu port %d to lag2" % (self.cpu_port))
            send_packet(self, self.cpu_port, cpu_pkt)
            verify_packets_any(self, pkt, [self.devports[5], self.devports[6]])
        finally:
            pass

        # ipv6 cpu tx packet.
        ipv6_pkt = simple_tcpv6_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ipv6_dst='2000::3',
                ipv6_src='3000::3',
                ipv6_hlim=64)
        cpu_pkt = simple_cpu_packet(
            dst_device=0,
            ingress_ifindex=0xFFFF & self.port1,
            ingress_port=0,
            ingress_bd=0,
            tx_bypass=True,
            reason_code=0xFFFF,
            inner_pkt=ipv6_pkt)
        try:
            print("Sending v6 packet from cpu port %d to port %d" % (self.cpu_port, self.devports[1]))
            send_packet(self, self.cpu_port, cpu_pkt)
            verify_packets(self, ipv6_pkt, [self.devports[1]])
        finally:
            pass

        # L3 mcast packet
        pkt = simple_tcp_packet(
            eth_dst='01:11:11:11:11:11',
            eth_src='00:00:00:00:00:01',
            ip_dst='224.0.0.5',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        cpu_pkt = simple_cpu_packet(
            dst_device=0,
            ingress_ifindex=0xFFFF & self.port2,
            ingress_port=0,
            ingress_bd=0,
            tx_bypass=True,
            reason_code=0xFFFF,
            inner_pkt=pkt)
        try:
            print("Sending L3 mc packet from cpu port %d to port %d" % (self.cpu_port, self.devports[2]))
            send_packet(self, self.cpu_port, cpu_pkt)
            verify_packets(self, pkt, [self.devports[2]])
        finally:
            pass

        # L2 mcast packet
        pkt = simple_eth_packet(eth_dst='01:80:C2:00:00:0e', pktlen=100)
        cpu_pkt = simple_cpu_packet(
            dst_device=0,
            ingress_ifindex=0xFFFF & self.port2,
            ingress_port=0,
            ingress_bd=0,
            tx_bypass=True,
            reason_code=0xFFFF,
            inner_pkt=pkt)
        try:
            print("Sending L2 mc packet from cpu port %d to port %d" % (self.cpu_port, self.devports[2]))
            send_packet(self, self.cpu_port, cpu_pkt)
            verify_packets(self, pkt, [self.devports[2]])
        finally:
            pass

    def L2CPUTxAccessPortTest(self):
        print("L2CPUTxAccessPortTest()")
        pkt = simple_tcp_packet(
            eth_dst='00:33:33:33:33:33',
            eth_src='00:00:00:00:00:01',
            ip_dst='172.17.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64,
            pktlen=100)
        if (self.client.is_feature_enable(SWITCH_FEATURE_CPU_BD_MAP) == 0):
            cpu_pkt = simple_cpu_packet(
                dst_device=0,
                ingress_ifindex=0xFFFF & self.port7,
                ingress_port=0,
                ingress_bd=0,
                tx_bypass=True,
                reason_code=0xFFFF,
                inner_pkt=pkt)
        else:
            cpu_pkt = simple_cpu_packet(
                dst_device=0,
                ingress_ifindex=0,
                ingress_port=0,
                ingress_bd=10,  # vlan_id 10 for self.vlan10
                tx_bypass=False,
                egress_queue=7,
                reason_code=0x08,
                inner_pkt=pkt)
        exp_pkt = simple_tcp_packet(
            eth_dst='00:33:33:33:33:33',
            eth_src='00:00:00:00:00:01',
            ip_dst='172.17.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64,
            pktlen=100)


        pkt2 = simple_tcp_packet(
            eth_dst='00:33:33:33:33:33',
            eth_src='00:00:00:00:00:01',
            ip_dst='172.17.10.1',
            ip_src='192.168.0.1',
            dl_vlan_enable=True,
            vlan_vid=10,
            ip_id=105,
            ip_ttl=64,
            pktlen=100)
        if (self.client.is_feature_enable(SWITCH_FEATURE_CPU_BD_MAP) == 0):
            cpu_pkt2 = simple_cpu_packet(
                dst_device=0,
                ingress_ifindex=0xFFFF & self.port7,
                ingress_port=0,
                ingress_bd=0,
                tx_bypass=True,
                reason_code=0xFFFF,
                inner_pkt=pkt2)
        else:
            cpu_pkt2 = simple_cpu_packet(
                dst_device=0,
                ingress_ifindex=0,
                ingress_port=0,
                ingress_bd=10,  # vlan_id 10 for self.vlan10
                tx_bypass=False,
                egress_queue=7,
                reason_code=0x08,
                inner_pkt=pkt2)

        exp_pkt2 = simple_tcp_packet(
            eth_dst='00:33:33:33:33:33',
            eth_src='00:00:00:00:00:01',
            ip_dst='172.17.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64,
            pktlen=96)

        port_7_qid_7_handle = self.qid_to_handle(self.port7,7)
        try:
            if (self.client.is_feature_enable(SWITCH_FEATURE_CPU_BD_MAP) == 0):
                send_packet(self, self.cpu_port, cpu_pkt)
                verify_packets(self, pkt, [self.devports[7]])

                send_packet(self, self.cpu_port, cpu_pkt2)
                verify_packets(self, pkt2, [self.devports[7]])

            else:
                before_q_cntrs = self.get_queue_counter(port_7_qid_7_handle)
                print("Sending packet from cpu port %d to access port %d" % (self.cpu_port, self.devports[7]))
                send_packet(self, self.cpu_port, cpu_pkt)
                verify_packets(self, exp_pkt, [self.devports[7]])
                print("Sleeping for 2 sec before fetching stats")
                time.sleep(2)
                after_q_cntrs = self.get_queue_counter(port_7_qid_7_handle)
                self.assertTrue(self.verifyQueueStats(before_q_cntrs, after_q_cntrs,
                            SWITCH_QUEUE_COUNTER_ID_STAT_PACKETS,
                            expected_pkt_count=1),
                           "Verify QueueStats failed for qid: %d qid_handle: 0x%lx "%(0, port_7_qid_7_handle))

                #send vlan_tagged packet and test for untagged packet
                before_q_cntrs = self.get_queue_counter(port_7_qid_7_handle)
                print("Sending tagged packet from cpu port %d to access port %d" % (self.cpu_port, self.devports[7]))
                send_packet(self, self.cpu_port, cpu_pkt2)
                verify_packets(self, exp_pkt2, [self.devports[7]])
                print("Sleeping for 2 sec before fetching stats")
                time.sleep(2)
                after_q_cntrs = self.get_queue_counter(port_7_qid_7_handle)
                self.assertTrue(self.verifyQueueStats(before_q_cntrs, after_q_cntrs,
                            SWITCH_QUEUE_COUNTER_ID_STAT_PACKETS,
                            expected_pkt_count=1),
                           "Verify QueueStats failed for qid: %d qid_handle: 0x%lx "%(0, port_7_qid_7_handle))

        finally:
            pass

    def L2CPUTxTrunkPortTest(self):
        print("L2CPUTxTrunkPortTest()")
        pkt = simple_tcp_packet(
            eth_dst='00:44:44:44:44:44',
            eth_src='00:00:00:00:00:01',
            ip_dst='172.17.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64,
            pktlen=100)
        if (self.client.is_feature_enable(SWITCH_FEATURE_CPU_BD_MAP) == 0):
            cpu_pkt = simple_cpu_packet(
                dst_device=0,
                ingress_ifindex=0xFFFF & self.port8,
                ingress_port=0,
                ingress_bd=0,
                tx_bypass=True,
                reason_code=0xFFFF,
                inner_pkt=pkt)
        else:
            cpu_pkt = simple_cpu_packet(
                dst_device=0,
                ingress_ifindex=0,
                ingress_port=0,
                ingress_bd=10,  # vlan_id 10 for self.vlan10
                tx_bypass=False,
                egress_queue=7,
                reason_code=0x08,
                inner_pkt=pkt)
        exp_pkt = simple_tcp_packet(
            eth_dst='00:44:44:44:44:44',
            eth_src='00:00:00:00:00:01',
            ip_dst='172.17.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64,
            dl_vlan_enable=True,
            vlan_vid=10,
            pktlen=104)

        pkt2 = simple_tcp_packet(
            eth_dst='00:44:44:44:44:44',
            eth_src='00:00:00:00:00:01',
            ip_dst='172.17.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64,
            dl_vlan_enable=True,
            vlan_vid=10,
            pktlen=104)
        if (self.client.is_feature_enable(SWITCH_FEATURE_CPU_BD_MAP) == 0):
            cpu_pkt2 = simple_cpu_packet(
                dst_device=0,
                ingress_ifindex=0xFFFF & self.port8,
                ingress_port=0,
                ingress_bd=0,
                tx_bypass=True,
                reason_code=0xFFFF,
                inner_pkt=pkt2)
        else:
            cpu_pkt2 = simple_cpu_packet(
                dst_device=0,
                ingress_ifindex=0,
                ingress_port=0,
                ingress_bd=10,  # vlan_id 10 for self.vlan10
                tx_bypass=False,
                egress_queue=7,
                reason_code=0x08,
                inner_pkt=pkt2)
        exp_pkt2 = simple_tcp_packet(
            eth_dst='00:44:44:44:44:44',
            eth_src='00:00:00:00:00:01',
            ip_dst='172.17.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64,
            dl_vlan_enable=True,
            vlan_vid=10,
            pktlen=104)

        port_8_qid_7_handle = self.qid_to_handle(self.port8,7)
        try:
            if (self.client.is_feature_enable(SWITCH_FEATURE_CPU_BD_MAP) == 0):
                send_packet(self, self.cpu_port, cpu_pkt)
                verify_packets(self, pkt, [self.devports[8]])
                send_packet(self, self.cpu_port, cpu_pkt2)
                verify_packets(self, pkt2, [self.devports[8]])

            else:
                before_q_cntrs = self.get_queue_counter(port_8_qid_7_handle)
                print("Sending un-tagged packet from cpu port %d to trunk port %d" % (self.cpu_port, self.devports[8]))
                send_packet(self, self.cpu_port, cpu_pkt)
                verify_packets(self, exp_pkt, [self.devports[8]])
                print("Sleeping for 2 sec before fetching stats")
                time.sleep(2)
                after_q_cntrs = self.get_queue_counter(port_8_qid_7_handle)
                self.assertTrue(self.verifyQueueStats(before_q_cntrs, after_q_cntrs,
                            SWITCH_QUEUE_COUNTER_ID_STAT_PACKETS,
                            expected_pkt_count=1),
                           "Verify QueueStats failed for qid: %d qid_handle: 0x%lx "%(0, port_8_qid_7_handle))

                print("Sending tagged packet from cpu port %d to trunk port %d" % (self.cpu_port, self.devports[8]))
                before_q_cntrs = self.get_queue_counter(port_8_qid_7_handle)
                send_packet(self, self.cpu_port, cpu_pkt2)
                verify_packets(self, exp_pkt2, [self.devports[8]])
                print("Sleeping for 2 sec before fetching stats")
                time.sleep(2)
                after_q_cntrs = self.get_queue_counter(port_8_qid_7_handle)
                self.assertTrue(self.verifyQueueStats(before_q_cntrs, after_q_cntrs,
                            SWITCH_QUEUE_COUNTER_ID_STAT_PACKETS,
                            expected_pkt_count=1),
                           "Verify QueueStats failed for qid: %d qid_handle: 0x%lx "%(0, port_8_qid_7_handle))
        finally:
            pass

    def L2CPUTxAccessLagTest(self):
        print("L2CPUTxAccessLagTest()")
        pkt1 = simple_tcp_packet(
            eth_dst='00:55:52:52:52:52',
            eth_src='00:00:00:00:00:01',
            ip_dst='172.17.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64,
            pktlen=100)
        port_lag_index = (1 << 9) | (0xFFFF & self.lag3);
        if (self.client.is_feature_enable(SWITCH_FEATURE_CPU_BD_MAP) == 0):
            cpu_pkt = simple_cpu_packet(
                dst_device=0,
                ingress_ifindex=port_lag_index,
                ingress_port=0,
                ingress_bd=0,
                tx_bypass=True,
                reason_code=0xFFFF,
                inner_pkt=pkt1)
        else:
            cpu_pkt = simple_cpu_packet(
                dst_device=0,
                ingress_ifindex=0,
                ingress_port=0,
                ingress_bd=20,  # vlan_id 20 for self.vlan20
                tx_bypass=False,
                egress_queue=7,
                reason_code=0x08,
                inner_pkt=pkt1)
        exp_pkt1 = simple_tcp_packet(
            eth_dst='00:55:52:52:52:52',
            eth_src='00:00:00:00:00:01',
            ip_dst='172.17.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64,
            pktlen=100)

        pkt2 = simple_tcp_packet(
            eth_dst='00:55:52:52:52:52',
            eth_src='00:00:00:00:00:01',
            ip_dst='172.17.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64,
            dl_vlan_enable=True,
            vlan_vid=20,
            pktlen=104)
        if (self.client.is_feature_enable(SWITCH_FEATURE_CPU_BD_MAP) == 0):
            cpu_pkt2 = simple_cpu_packet(
                dst_device=0,
                ingress_ifindex=port_lag_index,
                ingress_port=0,
                ingress_bd=0,
                tx_bypass=True,
                reason_code=0xFFFF,
                inner_pkt=pkt2)
        else:
            cpu_pkt2 = simple_cpu_packet(
                dst_device=0,
                ingress_ifindex=0,
                ingress_port=0,
                ingress_bd=20,  # vlan_id 20 for self.vlan20
                tx_bypass=False,
                egress_queue=7,
                reason_code=0x08,
                inner_pkt=pkt2)
        exp_pkt2 = simple_tcp_packet(
            eth_dst='00:55:52:52:52:52',
            eth_src='00:00:00:00:00:01',
            ip_dst='172.17.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64,
            pktlen=100)

        try:
            self.lag_mbr30 = self.add_lag_member(self.device, lag_handle=self.lag3, port_handle=self.port14)
            self.lag_mbr31 = self.add_lag_member(self.device, lag_handle=self.lag3, port_handle=self.port15)
            print("Sending untagged packet from cpu port %d to access lag2" % self.cpu_port)
            if (self.client.is_feature_enable(SWITCH_FEATURE_CPU_BD_MAP) == 0):
                send_packet(self, self.cpu_port, cpu_pkt)
                verify_packets_any(self, pkt1, [self.devports[14], self.devports[15]])

                send_packet(self, self.cpu_port, cpu_pkt2)
                verify_packets_any(self, pkt2, [self.devports[14], self.devports[15]])
                self.cleanlast()

            else:
                send_packet(self, self.cpu_port, cpu_pkt)
                verify_packets_any(self,exp_pkt1, [self.devports[14], self.devports[15]])

                print("Sending tagged packet from cpu port %d to access lag2" % self.cpu_port)
                send_packet(self, self.cpu_port, cpu_pkt2)
                verify_packets_any(self,exp_pkt2, [self.devports[14], self.devports[15]])

                # remove lag port and test for traffic.
                print("remove lag1's memeber port")
                print("Sending packet from cpu port %d to access lag2" % self.cpu_port)
                self.cleanlast()
                send_packet(self, self.cpu_port, cpu_pkt)
                verify_packets_any(self, exp_pkt1, [self.devports[14]])

        finally:
            self.cleanlast()
            pass

    def L2CPUTxTrunkLagTest(self):
        print("L2CPUTxTrunkLagTest()")
        pkt = simple_tcp_packet(
            eth_dst='00:11:11:11:11:11',
            eth_src='00:00:00:00:00:01',
            ip_dst='172.17.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64,
            pktlen=100)
        port_lag_index = (1 << 9) | (0xFFFF & self.lag0);
        if (self.client.is_feature_enable(SWITCH_FEATURE_CPU_BD_MAP) == 0):
            cpu_pkt = simple_cpu_packet(
                dst_device=0,
                ingress_ifindex=port_lag_index,
                ingress_port=0,
                ingress_bd=0,
                tx_bypass=True,
                reason_code=0xFFFF,
                inner_pkt=pkt)
        else:
            cpu_pkt = simple_cpu_packet(
                dst_device=0,
                ingress_ifindex=0,
                ingress_port=0,
                ingress_bd=10,  # vlan_id 10 for self.vlan10
                tx_bypass=False,
                egress_queue=7,
                reason_code=0x08,
                inner_pkt=pkt)
        exp_pkt = simple_tcp_packet(
            eth_dst='00:11:11:11:11:11',
            eth_src='00:00:00:00:00:01',
            ip_dst='172.17.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64,
            dl_vlan_enable=True,
            vlan_vid=10,
            pktlen=104)
        pkt2 = simple_tcp_packet(
            eth_dst='00:11:11:11:11:11',
            eth_src='00:00:00:00:00:01',
            ip_dst='172.17.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64,
            dl_vlan_enable=True,
            vlan_vid=10,
            pktlen=104)
        if (self.client.is_feature_enable(SWITCH_FEATURE_CPU_BD_MAP) == 0):
            cpu_pkt2 = simple_cpu_packet(
                dst_device=0,
                ingress_ifindex=port_lag_index,
                ingress_port=0,
                ingress_bd=0,
                tx_bypass=True,
                reason_code=0xFFFF,
                inner_pkt=pkt2)
        else:
            cpu_pkt2 = simple_cpu_packet(
                dst_device=0,
                ingress_ifindex=0,
                ingress_port=0,
                ingress_bd=10,  # vlan_id 10 for self.vlan10
                tx_bypass=False,
                egress_queue=7,
                reason_code=0x08,
                inner_pkt=pkt2)
        exp_pkt2 = simple_tcp_packet(
            eth_dst='00:11:11:11:11:11',
            eth_src='00:00:00:00:00:01',
            ip_dst='172.17.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64,
            dl_vlan_enable=True,
            vlan_vid=10,
            pktlen=104)
        try:
            print("Sending untagged packet from cpu port %d to trunk lag0" % self.cpu_port)
            if (self.client.is_feature_enable(SWITCH_FEATURE_CPU_BD_MAP) == 0):
                send_packet(self, self.cpu_port, cpu_pkt)
                verify_packets_any(self, pkt, [self.devports[1], self.devports[2]])

                send_packet(self, self.cpu_port, cpu_pkt2)
                verify_packets_any(self, pkt2, [self.devports[1], self.devports[2]])
            else:

                send_packet(self, self.cpu_port, cpu_pkt)
                verify_packets_any(self, exp_pkt, [self.devports[1], self.devports[2]])

                print("Sending tagged packet from cpu port %d to trunk lag0" % self.cpu_port)
                send_packet(self, self.cpu_port, cpu_pkt2)
                verify_packets_any(self, exp_pkt2, [self.devports[1], self.devports[2]])
        finally:
            pass

    def L2CPUTxFloodTest(self):
        print("L2CPUTxFloodTest()")
        if (self.client.is_feature_enable(SWITCH_FEATURE_CPU_BD_MAP) == 0):
            return
        pkt = simple_tcp_packet(
            eth_dst='00:FF:FF:FF:FF:FF',
            eth_src='00:00:00:00:00:01',
            ip_dst='172.17.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64,
            pktlen=100)
        cpu_pkt = simple_cpu_packet(
            dst_device=0,
            ingress_ifindex=0,
            ingress_port=0,
            ingress_bd=10,  # vlan_id 10 for self.vlan10
            tx_bypass=False,
            egress_queue=7,
            reason_code=0x08,
            inner_pkt=pkt)
        exp_pkt = simple_tcp_packet(
            eth_dst='00:FF:FF:FF:FF:FF',
            eth_src='00:00:00:00:00:01',
            ip_dst='172.17.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64,
            pktlen=100)
        exp_tag_pkt = simple_tcp_packet(
            eth_dst='00:FF:FF:FF:FF:FF',
            eth_src='00:00:00:00:00:01',
            ip_dst='172.17.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64,
            dl_vlan_enable=True,
            vlan_vid=10,
            pktlen=104)
        try:
            print("Sending packet from cpu port %d, flood to lag0, lag1, port %d, %d " % (self.cpu_port, self.devports[7], self.devports[8]))
            send_packet(self, self.cpu_port, cpu_pkt)
            verify_any_packet_on_ports_list(self, [exp_pkt, exp_tag_pkt], [
                [self.devports[1], self.devports[2]], [self.devports[3], self.devports[4]], [self.devports[7]], [self.devports[8]]])
        finally:
            pass

    def L3CPUTxPortTest(self):
        print("L3CPUTxPortTest()")
        pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:66:66:66:66:66',
            ip_dst='192.17.10.2',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64,
            pktlen=100)
        if (self.client.is_feature_enable(SWITCH_FEATURE_CPU_BD_MAP) == 0):
            cpu_pkt = simple_cpu_packet(
                dst_device=0,
                ingress_ifindex=0xFFFF & self.port7,
                ingress_port=0,
                ingress_bd=0,
                tx_bypass=True,
                reason_code=0xFFFF,
                inner_pkt=pkt)
        else:
            cpu_pkt = simple_cpu_packet(
                dst_device=0,
                ingress_ifindex=0,
                ingress_port=0,
                ingress_bd=10,  # vlan_id 10 for self.vlan10
                tx_bypass=False,
                egress_queue=7,
                reason_code=0x08,
                inner_pkt=pkt)
        exp_pkt = simple_tcp_packet(
            eth_src='00:77:66:55:44:33',
            eth_dst='00:33:33:33:33:33',
            ip_dst='192.17.10.2',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63,
            pktlen=100)
        try:
            print("Sending packet from cpu port %d to port %d" % (self.cpu_port, self.devports[7]))
            if (self.client.is_feature_enable(SWITCH_FEATURE_CPU_BD_MAP) == 0):
                send_packet(self, self.cpu_port, cpu_pkt)
                verify_packets(self, pkt, [self.devports[7]])
            else:
                send_packet(self, self.cpu_port, cpu_pkt)
                verify_packets(self, exp_pkt, [self.devports[7]])
        finally:
            pass

    def L3CPUTxLagTest(self):
        print("L3CPUTxLagTest()")
        port_lag_index = (1 << 9) | (0xFFFF & self.lag1);
        pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:66:66:66:66:66',
            ip_dst='192.17.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64,
            pktlen=100)
        if (self.client.is_feature_enable(SWITCH_FEATURE_CPU_BD_MAP) == 0):
            cpu_pkt = simple_cpu_packet(
                dst_device=0,
                ingress_ifindex=port_lag_index,
                ingress_port=0,
                ingress_bd=0,
                tx_bypass=True,
                reason_code=0xFFFF,
                inner_pkt=pkt)
        else:
            cpu_pkt = simple_cpu_packet(
                dst_device=0,
                ingress_ifindex=0,
                ingress_port=0,
                ingress_bd=10,  # vlan_id 10 for self.vlan10
                tx_bypass=False,
                egress_queue=7,
                reason_code=0x08,
                inner_pkt=pkt)
        exp_pkt = simple_tcp_packet(
            eth_src='00:77:66:55:44:33',
            eth_dst='00:22:22:22:22:22',
            ip_dst='192.17.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63,
            pktlen=100)
        try:
            print("Sending packet from cpu port %d to lag1" % self.cpu_port)
            if (self.client.is_feature_enable(SWITCH_FEATURE_CPU_BD_MAP) == 0):
                send_packet(self, self.cpu_port, cpu_pkt)
                verify_packets_any(self, pkt, [self.devports[3], self.devports[4]])
            else:
                send_packet(self, self.cpu_port, cpu_pkt)
                verify_packets_any(self, exp_pkt, [self.devports[3], self.devports[4]])
        finally:
            pass

###############################################################################

@group('hostif')
class HostIfRxCallbackTest(ApiHelper):
    def setUp(self):
        print()
        self.configure()

        # create trap group.
        queue_handles = self.attribute_get(self.cpu_port_hdl, SWITCH_PORT_ATTR_QUEUE_HANDLES)
        self.hostif_trap_group0 = self.add_hostif_trap_group(self.device, queue_handle=queue_handles[0].oid, admin_state=1)

        self.hostif_trap_stp_handle = self.add_hostif_trap(self.device, type=SWITCH_HOSTIF_TRAP_ATTR_TYPE_STP,
                        hostif_trap_group_handle=self.hostif_trap_group0,
                        packet_action=SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_REDIRECT_TO_CPU)
        self.add_hostif_rx_filter(self.device,
            type=SWITCH_HOSTIF_RX_FILTER_ATTR_TYPE_TRAP,
            hostif_trap_handle=self.hostif_trap_stp_handle,
            channel_type=SWITCH_HOSTIF_RX_FILTER_ATTR_CHANNEL_TYPE_CB)

    def runTest(self):
        try:
            if self.test_params['target'] == 'hw':
                print('skipping on HW')
                return
            self.client.packet_rx_cb_register(True)
            # STP
            pkt = simple_eth_packet(eth_dst='01:80:C2:00:00:00', pktlen=100)
            exp_pkt = simple_cpu_packet(
                  packet_type=0,
                  ingress_port=self.devports[0],
                  reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_STP,
                  inner_pkt=pkt)
            self.exp_stp_pkt = cpu_packet_mask_ingress_bd_and_ifindex(exp_pkt)

            print('Sending STP packet on port 0')
            send_packet(self, self.devports[0], pkt)
            if self.test_params['target'] != 'hw':
                verify_packet(self, self.exp_stp_pkt, self.cpu_port)
            pkt_size, in_port = verify_packet_from_cb(self, pkt)
            self.assertEqual(self.port0, in_port)
            self.assertEqual(pkt_size, 100)
        finally:
            self.client.packet_rx_cb_register(False)

    def tearDown(self):
        self.cleanup()

###############################################################################

@group('hostif')
class HostIfRxFilterTest(ApiHelper):
    def setUp(self):
        print()
        self.configure()
        self.port2_ingress_ifindex = self.get_port_ifindex(self.port2)
        self.port3_ingress_ifindex = self.get_port_ifindex(self.port3)
        self.prefix_len = 16
        self.cpu_nexthop = self.add_nexthop(self.device, type=SWITCH_NEXTHOP_ATTR_TYPE_GLEAN)
        vlan_member = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port2)
        vlan_member2 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port3)
        self.attribute_set(self.port2, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.attribute_set(self.port3, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.svi_rif0 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_VLAN, vlan_handle=self.vlan10,
                         vrf_handle=self.vrf10, src_mac=self.rmac)
        mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan10,
                  mac_address='00:06:07:08:09:0a', destination_handle=self.port2)

        # bind 30.30.0.3 to self.svi_rif0
        self.rif0_ip = '30.30.0.3'
        self.route1 = self.add_route(self.device, ip_prefix=self.rif0_ip, vrf_handle=self.vrf10,
                                     nexthop_handle=self.cpu_nexthop, is_host_myip=True)

        # create hostif for rif0
        rif0_hostif_name = "svi_rif0"
        rif0_ip_addr = self.rif0_ip + '/' + str(self.prefix_len)
        self.rif0_hostif0 = self.add_hostif(self.device, name=rif0_hostif_name,
                                       handle=self.svi_rif0, oper_status=True, ip_addr=rif0_ip_addr,
                                       mac=self.rmac)
        self.assertTrue(self.rif0_hostif0 != 0)

        # create hostif for port2
        hostif_name = "port2_host_if"
        self.port2_hostif = self.add_hostif(self.device, name=hostif_name, handle=self.port2, oper_status=True)
        self.assertTrue(self.port2_hostif != 0)
        print("Sleeping for 5 secs")
        time.sleep(5)

        # open socket to verfiy packet recieved on hostif netd.
        if self.test_params['target'] != 'hw':
            self.sock = open_packet_socket(hostif_name)

        # create hostif_trap entries for following trap types.
        trap_list = [
            [SWITCH_HOSTIF_TRAP_ATTR_TYPE_STP, SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_REDIRECT_TO_CPU]
        ]

        # create trap group.
        queue_handles = self.attribute_get(self.cpu_port_hdl, SWITCH_PORT_ATTR_QUEUE_HANDLES)
        self.hostif_trap_group0 = self.add_hostif_trap_group(self.device, queue_handle=queue_handles[0].oid, admin_state=1)

        self.hostif_trap_stp_handle = self.add_hostif_trap(self.device, type=SWITCH_HOSTIF_TRAP_ATTR_TYPE_STP,
                        hostif_trap_group_handle=self.hostif_trap_group0,
                        packet_action=SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_REDIRECT_TO_CPU)

        # create hostif_trap entries for following trap types.
        trap_list = [
            [SWITCH_HOSTIF_TRAP_ATTR_TYPE_ARP_REQUEST, SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_COPY_TO_CPU],
            [SWITCH_HOSTIF_TRAP_ATTR_TYPE_ARP_RESPONSE, SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_REDIRECT_TO_CPU],
            [SWITCH_HOSTIF_TRAP_ATTR_TYPE_MYIP, SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_COPY_TO_CPU]
        ]

        for trap in trap_list:
            trap.append(self.add_hostif_trap(self.device, type=trap[0],
                         hostif_trap_group_handle=self.hostif_trap_group0,
                         packet_action=trap[1]))

        # create rx_filter for arp and icmp packet
        for trap in trap_list:
            rx_filter1 = self.add_hostif_rx_filter(self.device,
                              hostif=self.rif0_hostif0,
                              hostif_trap_handle=trap[2],
                              handle=self.vlan10,
                              type=SWITCH_HOSTIF_RX_FILTER_ATTR_TYPE_TRAP)
    def runTest(self):
        print()
        try:
            print("run StpTest()")
            self.StpTest()
            print("run RxCallbackTest()")
            self.RxCallbackTest()
            print("Test ping on svi_rif0")
            self.SviPingTest('30.30.0.1', '30.30.0.3', '00:06:07:08:09:0a', self.devports[2], [self.devports[2]])
        finally:
            pass

    def tearDown(self):
        self.attribute_set(self.port2, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.port3, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.cleanup()

    def StpTest(self):
        try:
            # STP
            pre_counter = self.client.object_counters_get(self.port2_hostif)
            pkt = simple_eth_packet(eth_dst='01:80:C2:00:00:00', pktlen=100)
            exp_pkt = simple_cpu_packet(
                  packet_type=0,
                  ingress_port=self.devports[2],
                  ingress_ifindex=self.port2_ingress_ifindex,
                  reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_STP,
                  ingress_bd=2,
                  inner_pkt=pkt)
            self.exp_stp_pkt = cpu_packet_mask_ingress_bd(exp_pkt)

            print('Sending STP packet on port 2')
            send_packet(self, self.devports[2], pkt)
            if self.test_params['target'] != 'hw':
                verify_packet(self, self.exp_stp_pkt, self.cpu_port)
                self.assertTrue(socket_verify_packet(pkt, self.sock))
            time.sleep(5)
            post_counter = self.client.object_counters_get(self.port2_hostif)
            idx = SWITCH_HOSTIF_COUNTER_ID_RX_PKT
            self.assertEqual(post_counter[idx].count - pre_counter[idx].count, 1)
        finally:
            pass

    def RxCallbackTest(self):
        '''
        callback is higher priority than default hostif rx filter
        '''
        try:
            self.client.packet_rx_cb_register(True)
            rx_filter1 = self.add_hostif_rx_filter(self.device,
                              hostif=self.port2_hostif,
                              type=SWITCH_HOSTIF_RX_FILTER_ATTR_TYPE_TRAP,
                              hostif_trap_handle=self.hostif_trap_stp_handle,
                              channel_type=SWITCH_HOSTIF_RX_FILTER_ATTR_CHANNEL_TYPE_CB)
            # STP
            pkt = simple_eth_packet(eth_dst='01:80:C2:00:00:00', pktlen=100)
            exp_pkt = simple_cpu_packet(
                  packet_type=0,
                  ingress_port=self.devports[2],
                  ingress_ifindex=self.port2_ingress_ifindex,
                  reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_STP,
                  ingress_bd=2,
                  inner_pkt=pkt)
            self.exp_stp_pkt = cpu_packet_mask_ingress_bd(exp_pkt)

            print('Sending STP packet on port 2')
            send_packet(self, self.devports[2], pkt)
            if self.test_params['target'] != 'hw':
                verify_packet(self, self.exp_stp_pkt, self.cpu_port)
        finally:
            self.client.packet_rx_cb_register(False)
            self.cleanlast()

    def SviPingTest(self, host_ip, target_ip, host_mac, ingress_port, egress_port_list):
        try:
            if (self.client.is_feature_enable(SWITCH_FEATURE_CPU_BD_MAP) == 0):
                print("not supported")
                return
            # test arp resoultion.
            pkt = simple_arp_packet(
                        arp_op=1,
                        pktlen=100,
                        eth_src=host_mac,
                        hw_snd=host_mac,
                        ip_snd=host_ip,
                        ip_tgt=target_ip)
            exp_pkt = simple_arp_packet(
                        pktlen=42,
                        arp_op=2,
                        eth_src=self.rmac,
                        eth_dst=host_mac,
                        hw_snd=self.rmac,
                        hw_tgt=host_mac,
                        ip_snd=target_ip,
                        ip_tgt=host_ip)
            print("Sending ARP request for target_ip: {} from port2".format(target_ip))
            send_packet(self, self.devports[2], pkt)
            verify_packet_any_port(self, exp_pkt, egress_port_list)
            time.sleep(1)
            print("Received ARP responce for target_ip: {} from port2".format(target_ip))
            # test ping
            pkt = simple_icmp_packet(
                        eth_src=host_mac,
                        eth_dst=self.rmac,
                        ip_src=host_ip,
                        ip_dst=target_ip,
                        icmp_type=8,
                        icmp_data='000102030405')
            exp_pkt= simple_icmp_packet(
                        eth_src=self.rmac,
                        eth_dst=host_mac,
                        ip_src=target_ip,
                        ip_dst=host_ip,
                        icmp_type=0,
                        icmp_data='000102030405')
            m = Mask(exp_pkt)
            mask_set_do_not_care_packet(m, IP, "id")
            mask_set_do_not_care_packet(m, IP, "chksum")

            print("Sending ICMP Echo request from host_ip: {} to target_ip: {} from port2".format(host_ip,target_ip))
            send_packet(self, self.devports[2], pkt)
            verify_packet_any_port(self, m, egress_port_list)
            time.sleep(1)
            print("Received ICMP Echo reply from target_ip: {} from port2".format(target_ip))

        finally:
            pass

###############################################################################

@group('hostif')
class HostIfPVMissTest(ApiHelper):
    def setUp(self):
        print()
        self.configure()
        self.port1_ingress_ifindex = self.get_port_ifindex(self.port1)
        self.port2_ingress_ifindex = self.get_port_ifindex(self.port2)
        self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port1)
        self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port2, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.svi_rif0 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_VLAN, vlan_handle=self.vlan10,
                         vrf_handle=self.vrf10, src_mac=self.rmac)
        mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan10,
                  mac_address='00:06:07:08:09:0a', destination_handle=self.port2)

        queue_handles = self.attribute_get(self.cpu_port_hdl, SWITCH_PORT_ATTR_QUEUE_HANDLES)
        self.hostif_trap_group0 = self.add_hostif_trap_group(self.device, queue_handle=queue_handles[0].oid, admin_state=True)
        self.trap_list = [
          [SWITCH_HOSTIF_TRAP_ATTR_TYPE_ARP_REQUEST, SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_COPY_TO_CPU, 10],
          [SWITCH_HOSTIF_TRAP_ATTR_TYPE_ARP_RESPONSE, SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_REDIRECT_TO_CPU, 10],
          [SWITCH_HOSTIF_TRAP_ATTR_TYPE_IPV6_NEIGHBOR_DISCOVERY, SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_REDIRECT_TO_CPU, 10],
          [SWITCH_HOSTIF_TRAP_ATTR_TYPE_LACP, SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_REDIRECT_TO_CPU, 10],
          [SWITCH_HOSTIF_TRAP_ATTR_TYPE_ARP_SUPPRESS, SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_REDIRECT_TO_CPU, 1],
          [SWITCH_HOSTIF_TRAP_ATTR_TYPE_ND_SUPPRESS, SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_REDIRECT_TO_CPU, 1]
        ]

        for trap in self.trap_list:
            trap.append(self.add_hostif_trap(self.device, type=trap[0],
                                             hostif_trap_group_handle=self.hostif_trap_group0,
                                             packet_action=trap[1],
                                             priority=trap[2]))
        self.pre_p1 = self.client.object_counters_get(self.port1)
        self.pre_p2 = self.client.object_counters_get(self.port2)

    def DropStatsTest(self):
        time.sleep(4)
        self.post_p1 = self.client.object_counters_get(self.port1)
        self.post_p2 = self.client.object_counters_get(self.port2)
        id = SWITCH_PORT_COUNTER_ID_IF_IN_VLAN_DISCARDS
        if self.test_params['target'] != 'hw':
            self.assertEqual(self.post_p1[id].count - self.pre_p1[id].count, 3)
            self.assertEqual(self.post_p2[id].count - self.pre_p2[id].count, 3)

    def ArpTest(self):
        try:
            untagged_pkt = simple_arp_packet(arp_op=1, pktlen=100)
            tagged_10_pkt = simple_arp_packet(arp_op=1, vlan_vid=10, pktlen=104)
            # ARP Request
            print ("ARP req packet on untagged port, send to CPU")
            exp_pkt = simple_cpu_packet(
                  packet_type=0,
                  ingress_port=self.devports[1],
                  ingress_ifindex=self.port1_ingress_ifindex,
                  reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_ARP_REQUEST,
                  ingress_bd=2,
                  inner_pkt=untagged_pkt)
            self.exp_untagged_pkt = cpu_packet_mask_ingress_bd(exp_pkt)
            send_packet(self, self.devports[1], untagged_pkt)
            if self.test_params['target'] != 'hw':
                verify_each_packet_on_each_port(self, [self.exp_untagged_pkt, tagged_10_pkt], [self.cpu_port, self.devports[2]])

            print ("ARP req packet on tagged port vlan 10, send to CPU")
            exp_pkt = simple_cpu_packet(
                  packet_type=0,
                  ingress_port=self.devports[2],
                  ingress_ifindex=self.port2_ingress_ifindex,
                  reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_ARP_REQUEST,
                  ingress_bd=2,
                  inner_pkt=tagged_10_pkt)
            self.exp_tagged_pkt = cpu_packet_mask_ingress_bd(exp_pkt)
            send_packet(self, self.devports[2], tagged_10_pkt)
            if self.test_params['target'] != 'hw':
                verify_each_packet_on_each_port(self, [self.exp_tagged_pkt, untagged_pkt], [self.cpu_port, self.devports[1]])

            print ("ARP req packet on untagged port vlan 20, drop")
            tagged_20_pkt = simple_arp_packet(arp_op=1, vlan_vid=20, pktlen=100)
            if self.test_params['target'] != 'hw':
                send_packet(self, self.devports[1], tagged_20_pkt)
                verify_no_other_packets(self, timeout=2)

            print ("ARP req packet on tagged port vlan 20, drop")
            tagged_20_pkt = simple_arp_packet(arp_op=1, vlan_vid=20, pktlen=100)
            if self.test_params['target'] != 'hw':
                send_packet(self, self.devports[2], tagged_20_pkt)
                verify_no_other_packets(self, timeout=2)

            # ARP Response to Router MAC
            print ("ARP resp packet on untagged port, send to CPU")
            untagged_pkt = simple_arp_packet(arp_op=2, eth_dst=self.rmac, pktlen=100)
            exp_pkt = simple_cpu_packet(
                  packet_type=0,
                  ingress_port=self.devports[1],
                  ingress_ifindex=self.port1_ingress_ifindex,
                  reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_ARP_RESPONSE,
                  ingress_bd=2,
                  inner_pkt=untagged_pkt)
            self.exp_untagged_pkt = cpu_packet_mask_ingress_bd(exp_pkt)
            send_packet(self, self.devports[1], untagged_pkt)
            if self.test_params['target'] != 'hw':
                verify_packet(self, self.exp_untagged_pkt, self.cpu_port)

            print ("ARP resp packet on tagged port vlan 10, send to CPU")
            tagged_10_pkt = simple_arp_packet(arp_op=2, eth_dst=self.rmac, vlan_vid=10, pktlen=100)
            exp_pkt = simple_cpu_packet(
                  packet_type=0,
                  ingress_port=self.devports[2],
                  ingress_ifindex=self.port2_ingress_ifindex,
                  reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_ARP_RESPONSE,
                  ingress_bd=2,
                  inner_pkt=tagged_10_pkt)
            self.exp_tagged_pkt = cpu_packet_mask_ingress_bd(exp_pkt)
            send_packet(self, self.devports[2], tagged_10_pkt)
            if self.test_params['target'] != 'hw':
                verify_packet(self, self.exp_tagged_pkt, self.cpu_port)

            print ("ARP resp packet on untagged port vlan 20, drop")
            tagged_20_pkt = simple_arp_packet(arp_op=2, eth_dst=self.rmac, vlan_vid=20, pktlen=100)
            if self.test_params['target'] != 'hw':
                send_packet(self, self.devports[1], tagged_20_pkt)
                verify_no_other_packets(self, timeout=2)

            print ("ARP resp packet on tagged port vlan 20, drop")
            tagged_20_pkt = simple_arp_packet(arp_op=2, eth_dst=self.rmac, vlan_vid=20, pktlen=100)
            if self.test_params['target'] != 'hw':
                send_packet(self, self.devports[2], tagged_20_pkt)
                verify_no_other_packets(self, timeout=2)

            # ARP suppress test
            self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_ARP_SUPPRESS_ENABLE, 1)
            tagged_10_pkt = simple_arp_packet(arp_op=1, vlan_vid=10, pktlen=104)
            print ("ARP req packet on tagged port vlan 10, send to CPU")
            exp_pkt = simple_cpu_packet(
                  packet_type=0,
                  ingress_port=self.devports[2],
                  ingress_ifindex=self.port2_ingress_ifindex,
                  reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_ARP_REQUEST,
                  ingress_bd=2,
                  inner_pkt=tagged_10_pkt)
            self.exp_tagged_pkt = cpu_packet_mask_ingress_bd(exp_pkt)
            send_packet(self, self.devports[2], tagged_10_pkt)
            if self.test_params['target'] != 'hw':
                verify_packet(self, self.exp_tagged_pkt, self.cpu_port)
            verify_no_other_packets(self, timeout=1)

            # arp cpu tx packet.
            cpu_pkt = simple_cpu_packet(
                dst_device=0,
                ingress_ifindex=0xFFFF & self.port1,
                ingress_port=0,
                ingress_bd=0,
                tx_bypass=True,
                reason_code=0xFFFF,
                inner_pkt=tagged_10_pkt)

            print("Sending v6 packet from cpu port %d to port %d" % (self.cpu_port, self.devports[1]))
            if self.test_params['target'] != 'hw':
                send_packet(self, self.cpu_port, cpu_pkt)
                verify_packets(self, tagged_10_pkt, [self.devports[1]])
            self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_ARP_SUPPRESS_ENABLE, 0)

        finally:
            pass

    def ICMPv6Test(self):
        try:
            self.icmp6_rs_pkt = simple_icmpv6_packet(
                eth_dst='00:77:66:55:44:33',
                ipv6_dst='ff02::2',
                ipv6_src='2000::1',
                icmp_type=133,
                ipv6_hlim=64)
            exp_pkt = simple_cpu_packet(
                packet_type=0,
                ingress_port=self.devports[1],
                ingress_ifindex=self.port1_ingress_ifindex,
                reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_IPV6_NEIGHBOR_DISCOVERY,
                ingress_bd=1,
                inner_pkt=self.icmp6_rs_pkt)
            self.exp_icmp6_rs_pkt = cpu_packet_mask_ingress_bd(exp_pkt)
            print('Sending ICMPv6 packet for router solicitation on untagged port, send to CPU')
            send_packet(self, self.devports[1], self.icmp6_rs_pkt)
            if self.test_params['target'] != 'hw':
                verify_packet(self, self.exp_icmp6_rs_pkt, self.cpu_port)

            self.icmp6_rs_pkt = simple_icmpv6_packet(
                eth_dst='00:77:66:55:44:33',
                ipv6_dst='ff02::2',
                ipv6_src='2000::1',
                icmp_type=133,
                ipv6_hlim=64,
                dl_vlan_enable=True,
                vlan_vid=10)
            exp_pkt = simple_cpu_packet(
                packet_type=0,
                ingress_port=self.devports[2],
                ingress_ifindex=self.port2_ingress_ifindex,
                reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_IPV6_NEIGHBOR_DISCOVERY,
                ingress_bd=1,
                inner_pkt=self.icmp6_rs_pkt)
            self.exp_icmp6_rs_pkt = cpu_packet_mask_ingress_bd(exp_pkt)
            print('Sending ICMPv6 packet for router solicitation on tagged port, send to CPU')
            send_packet(self, self.devports[2], self.icmp6_rs_pkt)
            if self.test_params['target'] != 'hw':
                verify_packet(self, self.exp_icmp6_rs_pkt, self.cpu_port)

            self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_ARP_SUPPRESS_ENABLE, 1)
            self.icmp6_rs_pkt = simple_icmpv6_packet(
                eth_dst='00:77:66:55:44:33',
                ipv6_dst='ff02::2',
                ipv6_src='2000::1',
                icmp_type=133,
                ipv6_hlim=64,
                dl_vlan_enable=True,
                vlan_vid=10)
            exp_pkt = simple_cpu_packet(
                packet_type=0,
                ingress_port=self.devports[2],
                ingress_ifindex=self.port2_ingress_ifindex,
                reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_IPV6_NEIGHBOR_DISCOVERY,
                ingress_bd=1,
                inner_pkt=self.icmp6_rs_pkt)
            self.exp_icmp6_rs_pkt = cpu_packet_mask_ingress_bd(exp_pkt)
            print('Enable Nd suppression and Sending ICMPv6 packet for router solicitation on tagged port, send to CPU')
            send_packet(self, self.devports[2], self.icmp6_rs_pkt)
            if self.test_params['target'] != 'hw':
                verify_packet(self, self.exp_icmp6_rs_pkt, self.cpu_port)
            verify_no_other_packets(self, timeout=1)

            # nd cpu tx packet.
            cpu_pkt = simple_cpu_packet(
                dst_device=0,
                ingress_ifindex=0xFFFF & self.port1,
                ingress_port=0,
                ingress_bd=0,
                tx_bypass=True,
                reason_code=0xFFFF,
                inner_pkt=self.icmp6_rs_pkt)

            print("Sending v6 packet from cpu port %d to port %d" % (self.cpu_port, self.devports[1]))
            if self.test_params['target'] != 'hw':
                send_packet(self, self.cpu_port, cpu_pkt)
                verify_packets(self, self.icmp6_rs_pkt, [self.devports[1]])

            self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_ARP_SUPPRESS_ENABLE, 0)

            self.icmp6_rs_pkt = simple_icmpv6_packet(
                eth_dst='00:77:66:55:44:33',
                ipv6_dst='ff02::2',
                ipv6_src='2000::1',
                icmp_type=133,
                ipv6_hlim=64,
                dl_vlan_enable=True,
                vlan_vid=20)
            print('Sending ICMPv6 packet for router solicitation with vlan 20 tag, drop')
            send_packet(self, self.devports[1], self.icmp6_rs_pkt)
            verify_no_other_packets(self, timeout=2)
            send_packet(self, self.devports[2], self.icmp6_rs_pkt)
            verify_no_other_packets(self, timeout=2)
        finally:
            pass

    def LacpTest(self):
        try:
            # LACP
            pkt = simple_eth_packet(eth_dst='01:80:C2:00:00:02', pktlen=100)
            exp_pkt = simple_cpu_packet(
                  packet_type=0,
                  ingress_port=self.devports[1],
                  ingress_ifindex=self.port1_ingress_ifindex,
                  reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_LACP,
                  ingress_bd=2,
                  inner_pkt=pkt)
            self.exp_lacp_pkt1 = cpu_packet_mask_ingress_bd(exp_pkt)
            exp_pkt = simple_cpu_packet(
                  packet_type=0,
                  ingress_port=self.devports[2],
                  ingress_ifindex=self.port2_ingress_ifindex,
                  reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_LACP,
                  ingress_bd=2,
                  inner_pkt=pkt)
            self.exp_lacp_pkt2 = cpu_packet_mask_ingress_bd(exp_pkt)

            print('Sending LACP packet')
            send_packet(self, self.devports[1], pkt)
            if self.test_params['target'] != 'hw':
                verify_packets(self, self.exp_lacp_pkt1, [self.cpu_port])
            send_packet(self, self.devports[2], pkt)
            if self.test_params['target'] != 'hw':
                verify_packets(self, self.exp_lacp_pkt2, [self.cpu_port])
        finally:
            pass

    def runTest(self):
        try:
            self.ArpTest()
            self.ICMPv6Test()
            self.LacpTest()
            self.DropStatsTest()
        finally:
            pass

    def tearDown(self):
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.cleanup()

###############################################################################

@group('hostif')
class HostIfUserDefinedTrapTest(ApiHelper):
    SWITCH_UDT_REASON_CODE = 0x2000
    def setUp(self):
        print()
        self.configure()
        self.vlan_mbr0 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port0)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.rif1 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT,
            port_handle=self.port1, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.rif2 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT,
            port_handle=self.port2, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.rif3 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT,
            port_handle=self.port3, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.queue_handles = self.attribute_get(self.cpu_port_hdl, SWITCH_PORT_ATTR_QUEUE_HANDLES)
        self.hostif_trap_group0 = self.add_hostif_trap_group(self.device,
            queue_handle=self.queue_handles[0].oid,
            admin_state=True)
        self.hostif_trap_group1 = self.add_hostif_trap_group(self.device,
            queue_handle=self.queue_handles[2].oid,
            admin_state=True)

        self.hostif1_name = "port1_hostif"
        self.port1_hostif = self.add_hostif(self.device, name=self.hostif1_name, handle=self.port1, oper_status=True)
        self.assertTrue(self.port1_hostif != 0)
        self.hostif2_name = "port2_hostif"
        self.port2_hostif = self.add_hostif(self.device, name=self.hostif2_name, handle=self.port2, oper_status=True)
        self.assertTrue(self.port2_hostif != 0)
        print("Sleeping for 5 secs")
        time.sleep(5)
        self.port1_ingress_ifindex = self.get_port_ifindex(self.port1)

        # open socket to verfiy packet recieved on hostif netd.
        if self.test_params['target'] != 'hw':
            self.sock1 = open_packet_socket(self.hostif1_name)
            self.sock2 = open_packet_socket(self.hostif2_name)

    def runTest(self):
        try:
            self.aclIpSrcTrap()
            self.neighborUdtTrap()
            if self.test_params['target'] != 'hw':
                self.ecmpNeighborUdtTrap()
        finally:
            pass

    def tearDown(self):
        if self.test_params['target'] != 'hw':
            self.sock1.close()
            self.sock2.close()
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.cleanup()

    def neighborUdtTrap(self):
        print("Testing neighborUdtTrap()")
        try:
            neighbor_mac = '00:11:11:11:11:02'
            neighbor_ip = '10.2.0.2'
            dest_ip = '10.3.0.2'
            route_prefix = '10.3.0.0/24'
            nhop = self.add_nexthop(self.device, handle=self.rif2, dest_ip=neighbor_ip)
            route = self.add_route(self.device, ip_prefix=route_prefix, vrf_handle=self.vrf10, nexthop_handle=nhop)
            nbor = self.add_neighbor(self.device, mac_address=neighbor_mac, handle=self.rif2, dest_ip=neighbor_ip)

            pkt = simple_tcp_packet(eth_dst=self.rmac,
                                    ip_dst=dest_ip,
                                    ip_ttl=64)

            exp_pkt = simple_tcp_packet(eth_dst=neighbor_mac,
                                    eth_src=self.rmac,
                                    ip_dst=dest_ip,
                                    ip_ttl=63)
            print('Sending tcp packet, from Port:{} to Port:{}'.format(self.devports[1], self.devports[2]))
            send_packet(self, self.devports[1], pkt)
            verify_packet(self, exp_pkt, self.devports[2])

            # Delete the neighbor entry
            self.cleanlast()
            udt_trap = self.add_hostif_user_defined_trap(self.device,
              type=SWITCH_HOSTIF_USER_DEFINED_TRAP_ATTR_TYPE_NEIGHBOR,
              hostif_trap_group_handle=self.hostif_trap_group1,
              priority=1)
            # Add neighbor entry to trap packets
            nbor_trap = self.add_neighbor(self.device, dest_ip=neighbor_ip, handle=self.rif2,
            packet_action=SWITCH_NEIGHBOR_ATTR_PACKET_ACTION_TRAP, user_defined_trap_handle=udt_trap)
            self.add_hostif_rx_filter(self.device, type=SWITCH_HOSTIF_RX_FILTER_ATTR_TYPE_TRAP,
                hostif_trap_handle=udt_trap, hostif=self.port1_hostif)
            reason_code = self.SWITCH_UDT_REASON_CODE + (udt_trap & 0xFFFF)

            cpu_pkt = simple_cpu_packet(
                packet_type=0,
                ingress_port=self.devports[1],
                ingress_ifindex=self.port1_ingress_ifindex,
                reason_code=reason_code,
                ingress_bd=2,
                inner_pkt=pkt)
            exp_cpu_pkt = cpu_packet_mask_ingress_bd(cpu_pkt)
            print('Sending tcp packet, from Port:{} to hostif:{}'.format(self.devports[0], self.hostif1_name))
            send_packet(self, self.devports[1], pkt)
            if self.test_params['target'] != 'hw':
                verify_packet(self, exp_cpu_pkt, self.cpu_port)
                self.assertTrue(socket_verify_packet(pkt, self.sock1))

        finally:
            if nhop:
                self.clean_to_object(nhop)

    def ecmpNeighborUdtTrap(self):
        print("Testing ecmpNeighborUdtTrap()")
        try:
            neighbor1_mac = '00:11:11:11:11:02'
            neighbor1_ip = '10.2.0.2'
            neighbor2_mac = '00:11:11:11:12:02'
            neighbor2_ip = '10.2.1.2'
            dest_ip = '10.3.0.2'
            route_prefix = '10.3.0.0/24'
            ecmp = self.add_ecmp(self.device)
            nbor1 = self.add_neighbor(self.device, mac_address=neighbor1_mac, handle=self.rif2, dest_ip=neighbor1_ip)
            nbor2 = self.add_neighbor(self.device, mac_address=neighbor2_mac, handle=self.rif3, dest_ip=neighbor2_ip)
            nhop1 = self.add_nexthop(self.device, handle=self.rif2, dest_ip=neighbor1_ip)
            nhop2 = self.add_nexthop(self.device, handle=self.rif3, dest_ip=neighbor2_ip)
            ecmp_member1 = self.add_ecmp_member(self.device, ecmp_handle=ecmp, nexthop_handle=nhop1)
            ecmp_member2 = self.add_ecmp_member(self.device, ecmp_handle=ecmp, nexthop_handle=nhop2)
            route = self.add_route(self.device, ip_prefix=route_prefix, vrf_handle=self.vrf10, nexthop_handle=ecmp)

            pkt = simple_tcp_packet(eth_dst=self.rmac,
                                    ip_dst=dest_ip,
                                    ip_ttl=64)

            exp_pkt1 = simple_tcp_packet(eth_dst=neighbor1_mac,
                                    eth_src=self.rmac,
                                    ip_dst=dest_ip,
                                    ip_ttl=63)
            exp_pkt2 = simple_tcp_packet(eth_dst=neighbor2_mac,
                                    eth_src=self.rmac,
                                    ip_dst=dest_ip,
                                    ip_ttl=63)
            print('Sending tcp packet, from Port:{} to Port:{}'.format(self.devports[1], [self.devports[2],
            self.devports[3]]))
            send_packet(self, self.devports[1], pkt)
            verify_any_packet_any_port(self, [exp_pkt1, exp_pkt2], [self.devports[2], self.devports[3]])

            # Delete the neighbor entries
            self.clean_to_object(nbor1)
            udt1_trap = self.add_hostif_user_defined_trap(self.device,
              type=SWITCH_HOSTIF_USER_DEFINED_TRAP_ATTR_TYPE_NEIGHBOR,
              hostif_trap_group_handle=self.hostif_trap_group1,
              priority=1)
            udt2_trap = self.add_hostif_user_defined_trap(self.device,
              type=SWITCH_HOSTIF_USER_DEFINED_TRAP_ATTR_TYPE_NEIGHBOR,
              hostif_trap_group_handle=self.hostif_trap_group1,
              priority=1)
            # Add neighbor entry to trap packets
            nbor1_trap = self.add_neighbor(self.device, dest_ip=neighbor1_ip, handle=self.rif2,
            packet_action=SWITCH_NEIGHBOR_ATTR_PACKET_ACTION_TRAP, user_defined_trap_handle=udt1_trap)
            nbor2_trap = self.add_neighbor(self.device, dest_ip=neighbor2_ip, handle=self.rif3,
            packet_action=SWITCH_NEIGHBOR_ATTR_PACKET_ACTION_TRAP, user_defined_trap_handle=udt2_trap)
            nhop1 = self.add_nexthop(self.device, handle=self.rif2, dest_ip=neighbor1_ip)
            nhop2 = self.add_nexthop(self.device, handle=self.rif3, dest_ip=neighbor2_ip)
            ecmp_member1 = self.add_ecmp_member(self.device, ecmp_handle=ecmp, nexthop_handle=nhop1)
            ecmp_member2 = self.add_ecmp_member(self.device, ecmp_handle=ecmp, nexthop_handle=nhop2)
            route = self.add_route(self.device, ip_prefix=route_prefix, vrf_handle=self.vrf10, nexthop_handle=ecmp)
            self.add_hostif_rx_filter(self.device, type=SWITCH_HOSTIF_RX_FILTER_ATTR_TYPE_TRAP,
                hostif_trap_handle=udt1_trap, hostif=self.port1_hostif)
            self.add_hostif_rx_filter(self.device, type=SWITCH_HOSTIF_RX_FILTER_ATTR_TYPE_TRAP,
                hostif_trap_handle=udt2_trap, hostif=self.port2_hostif)
            reason_code1 = self.SWITCH_UDT_REASON_CODE + (udt1_trap & 0xFFFF)
            reason_code2 = self.SWITCH_UDT_REASON_CODE + (udt2_trap & 0xFFFF)

            src_ip = int(binascii.hexlify(socket.inet_aton('172.168.8.1')), 16)
            max_itrs = 20
            lb_count = {self.hostif1_name:0, self.hostif2_name:0}
            print('Sending tcp packets, from Port:{} to hostif:{}'.format(self.devports[1],
                  [self.hostif1_name,self.hostif2_name]))
            for i in range(0, max_itrs):
                src_ip_addr = socket.inet_ntoa(
                    binascii.unhexlify(format(src_ip, 'x').zfill(8)))
                pkt[IP].src=src_ip_addr
                cpu_pkt1 = simple_cpu_packet(
                    packet_type=0,
                    ingress_port=self.devports[1],
                    ingress_ifindex=self.port1_ingress_ifindex,
                    reason_code=reason_code1,
                    ingress_bd=2,
                    inner_pkt=pkt)
                cpu_pkt2 = simple_cpu_packet(
                    packet_type=0,
                    ingress_port=self.devports[1],
                    ingress_ifindex=self.port1_ingress_ifindex,
                    reason_code=reason_code2,
                    ingress_bd=2,
                    inner_pkt=pkt)
                src_ip+=1
                exp_cpu_pkt1 = cpu_packet_mask_ingress_bd(cpu_pkt1)
                exp_cpu_pkt2 = cpu_packet_mask_ingress_bd(cpu_pkt2)
                send_packet(self, self.devports[1], pkt)
                if self.test_params['target'] != 'hw':
                    verify_any_packet_any_port(self, [exp_cpu_pkt1, exp_cpu_pkt2], [self.cpu_port, self.cpu_port])
                    rcv_pkt_port1_hostif = socket_verify_packet(pkt, self.sock1)
                    rcv_pkt_port2_hostif = socket_verify_packet(pkt, self.sock2)
                    self.assertTrue(rcv_pkt_port1_hostif or rcv_pkt_port2_hostif)
                if rcv_pkt_port1_hostif:
                    lb_count[self.hostif1_name]+=1
                else:
                    lb_count[self.hostif2_name]+=1
            print("Host interface Packet Distribution:{}".format(lb_count))
            for _,v in lb_count.items():
                self.assertTrue(v != 0)

        finally:
            if ecmp:
                self.clean_to_object(ecmp)

    def aclIpSrcTrap(self):
        print("Testing aclIpSrcTrap()")
        if self.client.is_feature_enable(SWITCH_FEATURE_INGRESS_ACL_METER) == 0:
            print("IP acl meter not enabled, skipping")
            return
        if self.client.is_feature_enable(SWITCH_FEATURE_FOLDED_SWITCH_PIPELINE):
            print("Feature not supported on Folded Switch Pipeline")
            return
        # Requires meter based packet actions
        print("Test currently disabled - skipping")
        return

        src_ip = '10.0.0.1'
        table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IP
        try:
            self.trap_list = [
                [SWITCH_HOSTIF_TRAP_ATTR_TYPE_ARP_REQUEST, SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_REDIRECT_TO_CPU, 10],
              [SWITCH_HOSTIF_TRAP_ATTR_TYPE_LACP, SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_REDIRECT_TO_CPU, 30]
            ]

            udt_trap = self.add_hostif_user_defined_trap(self.device,
              type=SWITCH_HOSTIF_USER_DEFINED_TRAP_ATTR_TYPE_ACL,
              hostif_trap_group_handle=self.hostif_trap_group1,
              priority=20)

            for trap in self.trap_list:
                trap.append(self.add_hostif_trap(self.device, type=trap[0],
                                                 hostif_trap_group_handle=self.hostif_trap_group0,
                                                 packet_action=trap[1],
                                                 priority=trap[2]))

            acl_table_bp_switch = switcht_list_val_t(type=switcht_value_type.ENUM, enumdata=SWITCH_ACL_TABLE_ATTR_BIND_POINT_TYPE_SWITCH)
            acl_table = self.add_acl_table(self.device,
              type=table_type,
              bind_point_type=[acl_table_bp_switch],
              direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)

            self.add_acl_entry(self.device,
                  src_ip=src_ip,
                  src_ip_mask='255.255.255.255',
                  packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_REDIRECT_TO_CPU,
                  action_hostif_user_defined_trap_handle=udt_trap,
                  table_handle=acl_table)

            self.add_acl_entry(self.device,
                  eth_type=0x08cc,
                  eth_type_mask=0x0FFF,
                  dst_mac="01:80:C2:00:00:02",
                  dst_mac_mask="ff:ff:ff:ff:ff:ff",
                  packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_REDIRECT_TO_CPU,
                  action_hostif_user_defined_trap_handle=udt_trap,
                  table_handle=acl_table)

            self.add_hostif_rx_filter(self.device, type=SWITCH_HOSTIF_RX_FILTER_ATTR_TYPE_TRAP,
                hostif_trap_handle=udt_trap, hostif=self.port1_hostif)
            reason_code = self.SWITCH_UDT_REASON_CODE + (udt_trap & 0xFFFF)

            lacp_pkt = simple_eth_packet(eth_dst='01:80:C2:00:00:02', pktlen=100)
            exp_pkt = simple_cpu_packet(
                  packet_type=0,
                  ingress_port=self.devports[1],
                  ingress_ifindex=self.port1_ingress_ifindex,
                  reason_code=reason_code,
                  ingress_bd=2,
                  inner_pkt=lacp_pkt)
            self.exp_lacp_pkt = cpu_packet_mask_ingress_bd_port_and_ifindex(exp_pkt)

            tcp_pkt = simple_ip_packet(ip_src=src_ip)
            cpu_pkt = simple_cpu_packet(
                packet_type=0,
                ingress_port=self.devports[1],
                ingress_ifindex=self.port1_ingress_ifindex,
                reason_code=reason_code,
                ingress_bd=2,
                inner_pkt=tcp_pkt)
            self.exp_tcp_pkt = cpu_packet_mask_ingress_bd_port_and_ifindex(cpu_pkt)

            arp_pkt = simple_arp_packet(arp_op=1, pktlen=100)
            exp_pkt = simple_cpu_packet(
                  packet_type=0,
                  ingress_port=self.devports[1],
                  ingress_ifindex=self.port1_ingress_ifindex,
                  reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_ARP_REQUEST,
                  ingress_bd=2,
                  inner_pkt=arp_pkt)
            self.exp_arpq_bc_pkt = cpu_packet_mask_ingress_bd_port_and_ifindex(exp_pkt)

            pre_q0_counter = self.object_counters_get(self.queue_handles[0].oid)
            pre_q2_counter = self.object_counters_get(self.queue_handles[2].oid)

            print("ARP > udt > LACP")
            print('Sending tcp packet, send to udt trap')
            send_packet(self, self.devports[0], tcp_pkt)
            if self.test_params['target'] != 'hw':
                verify_packet(self, self.exp_tcp_pkt, self.cpu_port)
                self.assertTrue(socket_verify_packet(tcp_pkt, self.sock1))
            send_packet(self, self.devports[1], tcp_pkt)
            if self.test_params['target'] != 'hw':
                verify_packet(self, self.exp_tcp_pkt, self.cpu_port)

            print('Sending ARP request broadcast, send to arp trap since priority higher')
            send_packet(self, self.devports[0], arp_pkt)
            if self.test_params['target'] != 'hw':
                verify_packet(self, self.exp_arpq_bc_pkt, self.cpu_port)
                self.assertTrue(socket_verify_packet(arp_pkt, self.sock1))
            send_packet(self, self.devports[1], arp_pkt)
            if self.test_params['target'] != 'hw':
                verify_packet(self, self.exp_arpq_bc_pkt, self.cpu_port)

            print('Sending LACP request broadcast, send to udp trap since priority lower')
            send_packet(self, self.devports[0], lacp_pkt)
            if self.test_params['target'] != 'hw':
                verify_packet(self, self.exp_lacp_pkt, self.cpu_port)
                self.assertTrue(socket_verify_packet(lacp_pkt, self.sock1))
            send_packet(self, self.devports[1], lacp_pkt)
            if self.test_params['target'] != 'hw':
                verify_packet(self, self.exp_lacp_pkt, self.cpu_port)
                self.assertTrue(socket_verify_packet(lacp_pkt, self.sock1))

            if self.test_params['target'] == 'hw':
                time.sleep(2)
            post_q0_counter = self.object_counters_get(self.queue_handles[0].oid)
            post_q2_counter = self.object_counters_get(self.queue_handles[2].oid)
            x = SWITCH_QUEUE_COUNTER_ID_STAT_PACKETS
            self.assertEqual(post_q0_counter[x].count - pre_q0_counter[x].count, 2)
            self.assertEqual(post_q2_counter[x].count - pre_q2_counter[x].count, 4)
        finally:
            if udt_trap:
                self.clean_to_object(udt_trap)
