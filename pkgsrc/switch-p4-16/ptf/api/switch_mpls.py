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

from ptf import config
from ptf.testutils import *
from ptf.thriftutils import *

import os
import ptf.mask as mask

from bf_switcht_api_thrift.ttypes import *
from bf_switcht_api_thrift.model_headers import *

this_dir = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(this_dir, '..'))
from common.utils import *
from common.pktpy_utils import pktpy_skip, pktpy_skip_test # noqa pylint: disable=wrong-import-position


class MplsCpuTrapTest(ApiHelper):
    def setUp(self):
        self.configure()
        if (self.client.is_feature_enable(SWITCH_FEATURE_MPLS) == 0):
            print("MPLS feature not enabled, skipping")
            return
        self.ingress_rif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port0, vrf_handle=self.vrf10, src_mac=self.rmac, mpls_state=True)
        queue_handles = self.attribute_get(self.cpu_port_hdl, SWITCH_PORT_ATTR_QUEUE_HANDLES)
        self.hostif_trap_group0 = self.add_hostif_trap_group(self.device, queue_handle=queue_handles[0].oid, admin_state=1)
        self.mpls_ttl_trap = self.add_hostif_trap(self.device, type=SWITCH_HOSTIF_TRAP_ATTR_TYPE_MPLS_TTL_ERROR, packet_action=SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_DROP, hostif_trap_group_handle=self.hostif_trap_group0)
        self.mpls_null_lbl = self.add_mpls(self.device, label=3,packet_action=SWITCH_MPLS_ATTR_PACKET_ACTION_TRAP)
        self.add_hostif_trap(self.device, type=SWITCH_HOSTIF_TRAP_ATTR_TYPE_MPLS_ROUTER_ALERT, packet_action=SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_REDIRECT_TO_CPU, hostif_trap_group_handle=self.hostif_trap_group0)

        # MPLS Egress PHP configs
        self.egress_rif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port3, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.ip_nhop = self.add_nexthop(self.device, type=SWITCH_NEXTHOP_ATTR_TYPE_IP, handle=self.egress_rif, dest_ip='40.40.40.1')
        self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=self.egress_rif, dest_ip='40.40.40.1')
        self.mpls_object1000 = self.add_mpls(self.device, label=1000, nexthop_rif_handle=self.ip_nhop, num_pop=1, pop_ttl_mode=SWITCH_MPLS_ATTR_POP_TTL_MODE_PIPE_MODEL, pop_qos_mode=SWITCH_MPLS_ATTR_POP_QOS_MODE_PIPE_MODEL)

    def runTest(self):
        if (self.client.is_feature_enable(SWITCH_FEATURE_MPLS) == 0):
            print("MPLS feature not enabled, skipping")
            return
        try:
            self.MplsTTLErrorDrop()
            self.MplsRouterAlertToCpu()
            self.MplsImplicitNullLabelTrapDrop()
        finally:
            self.cleanup

    def MplsTTLErrorDrop(self):
        print(" Testing packet with TTL=0, TTL=1 ")
        send_pkt = simple_tcp_packet(
            eth_dst=self.rmac,
            ip_dst='40.40.40.1',
            ip_ttl=64)
        recv_pkt = simple_tcp_packet(
            eth_dst="00:11:22:33:44:55",
            eth_src=self.rmac,
            ip_dst='40.40.40.1',
            ip_ttl=63)
        mpls_tag ={'label':1000, 'ttl':64, 'tc': 0, 's': 1}
        mpls_tag_list = []
        mpls_tag_list.append(mpls_tag)
        mpls_ttl_64_pkt = simple_mpls_packet(eth_dst=self.rmac, mpls_tags=mpls_tag_list, inner_frame=send_pkt['IP'])
        mpls_tag['ttl']=1
        mpls_tag_list = []
        mpls_tag_list.append(mpls_tag)
        mpls_ttl_1_pkt = simple_mpls_packet(eth_dst=self.rmac, mpls_tags=mpls_tag_list, inner_frame=send_pkt['IP'])
        mpls_tag['ttl']=0
        mpls_tag_list = []
        mpls_tag_list.append(mpls_tag)
        mpls_ttl_0_pkt = simple_mpls_packet(eth_dst=self.rmac, mpls_tags=mpls_tag_list, inner_frame=send_pkt['IP'])
        self.port0_ingress_ifindex = self.get_port_ifindex(self.port0)
        mpls_e_ttl_1_pkt = simple_cpu_packet(
            packet_type=0,
            ingress_port=self.devports[0],
            ingress_ifindex=self.port0_ingress_ifindex,
            reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_MPLS_TTL_ERROR,
            ingress_bd=2,
            inner_pkt=mpls_ttl_1_pkt)
        self.mpls_cpu_packet = cpu_packet_mask_ingress_bd(mpls_e_ttl_1_pkt)
        try:
            print("  Tx mpls tag packet with TTL 64 --> Forw ")
            send_packet(self, self.devports[0], mpls_ttl_64_pkt)
            verify_packet(self, recv_pkt, self.devports[3])
            print("  Tx mpls tag packet with TTL 1 --> drop ")
            send_packet(self, self.devports[0], mpls_ttl_1_pkt)
            verify_no_other_packets(self)
            print("  Tx mpls tag packet with TTL 0 --> drop ")
            send_packet(self, self.devports[0], mpls_ttl_0_pkt)
            verify_no_other_packets(self)
            print("  Update TTL error hostif_trap to redirect to cpu, Tx mpls tag packet with TTL 1 --> cpu ")
            self.attribute_set(self.mpls_ttl_trap, SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION, SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_REDIRECT_TO_CPU)
            send_packet(self, self.devports[0], mpls_ttl_1_pkt)
            verify_packet(self, self.mpls_cpu_packet, self.cpu_port)
            self.attribute_set(self.mpls_ttl_trap, SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION, SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_DROP)
        finally:
            pass

    def MplsImplicitNullLabelTrapDrop(self):
        print(" Testing packet with implicit null label 3 to cpu ")
        send_pkt = simple_tcp_packet(
            eth_dst=self.rmac,
            ip_dst='40.40.40.1',
            ip_ttl=64)
        mpls_tag ={'label':3, 'ttl':63, 'tc': 0, 's': 1}
        mpls_tag_list = []
        mpls_tag_list.append(mpls_tag)
        mpls_pkt = simple_mpls_packet(eth_dst=self.rmac, mpls_tags=mpls_tag_list, inner_frame=send_pkt['IP'])
        self.port0_ingress_ifindex = self.get_port_ifindex(self.port0)
        mpls_e_pkt = simple_cpu_packet(
            packet_type=0,
            ingress_port=self.devports[0],
            ingress_ifindex=self.port0_ingress_ifindex,
            reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_MAX + SWITCH_HOSTIF_TRAP_ATTR_INTERNAL_TRAP_MPLS_TRAP,
            ingress_bd=2,
            inner_pkt=mpls_pkt)
        self.mpls_cpu_packet = cpu_packet_mask_ingress_bd(mpls_e_pkt)
        try:
            print("  Tx mpls implicit null label 3 --> cpu ")
            send_packet(self, self.devports[0], mpls_pkt)
            verify_packet(self, self.mpls_cpu_packet, self.cpu_port)
            print("  Set implicit null label to drop action ")
            self.attribute_set(self.mpls_null_lbl, SWITCH_MPLS_ATTR_PACKET_ACTION, SWITCH_MPLS_ATTR_PACKET_ACTION_DROP)
            print("  Tx mpls implicit null label 3 --> drop ")
            send_packet(self, self.devports[0], mpls_pkt)
            verify_no_other_packets(self)
            self.attribute_set(self.mpls_null_lbl, SWITCH_MPLS_ATTR_PACKET_ACTION, SWITCH_MPLS_ATTR_PACKET_ACTION_TRAP)
        finally:
            pass

    def MplsRouterAlertToCpu(self):
        print(" Testing packet with Router Alert label 1 to cpu ")
        send_pkt = simple_tcp_packet(
            eth_dst=self.rmac,
            ip_dst='40.40.40.1',
            ip_ttl=64)
        mpls_tag ={'label':1, 'ttl':63, 'tc': 0, 's': 0}
        mpls_tag_2 ={'label':5555, 'ttl':63, 'tc': 0, 's': 1}
        mpls_tag_list = []
        mpls_tag_list.append(mpls_tag)
        mpls_tag_list.append(mpls_tag_2)
        mpls_pkt = simple_mpls_packet(eth_dst=self.rmac, mpls_tags=mpls_tag_list, inner_frame=send_pkt['IP'])
        self.port0_ingress_ifindex = self.get_port_ifindex(self.port0)
        mpls_e_pkt = simple_cpu_packet(
            packet_type=0,
            ingress_port=self.devports[0],
            ingress_ifindex=self.port0_ingress_ifindex,
            reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_MPLS_ROUTER_ALERT,
            ingress_bd=2,
            inner_pkt=mpls_pkt)
        self.mpls_cpu_packet = cpu_packet_mask_ingress_bd(mpls_e_pkt)
        try:
            print("  Tx mpls packet with router-alert label 1 to cpu ")
            send_packet(self, self.devports[0], mpls_pkt)
            verify_packet(self, self.mpls_cpu_packet, self.cpu_port)
        finally:
            pass

    def tearDown(self):
        self.cleanup()

class MplsIpv6Test(ApiHelper):
    def setUp(self):
        self.configure()
        if (self.client.is_feature_enable(SWITCH_FEATURE_MPLS) == 0):
            print("MPLS feature not enabled, skipping")
            return
        #Add TTL_ERROR trap
        self.ttl_error=self.add_hostif_trap(self.device, type=SWITCH_HOSTIF_TRAP_ATTR_TYPE_TTL_ERROR, packet_action=SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_DROP)
        #MPLS Ingress LER configs
        self.ingress_rif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port0, vrf_handle=self.vrf10, src_mac=self.rmac, mpls_state=True)
        self.egress_rif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port1, vrf_handle=self.vrf10, src_mac=self.rmac)
        label_list = []
        label_list.append(switcht_list_val_t(type=switcht_value_type.UINT32, u32data=1000))
        self.nhop_label_1 = self.add_nexthop(self.device, type=SWITCH_NEXTHOP_ATTR_TYPE_MPLS, handle=self.egress_rif, dest_ip='100::1', labelstack=label_list, mpls_encap_ttl_mode=SWITCH_NEXTHOP_ATTR_MPLS_ENCAP_TTL_MODE_PIPE_MODEL, mpls_encap_ttl=63, mpls_encap_qos_mode=SWITCH_NEXTHOP_ATTR_MPLS_ENCAP_QOS_MODE_PIPE_MODEL, mpls_encap_exp=5, labelop=SWITCH_NEXTHOP_ATTR_LABELOP_PUSH)

        label_list.append(switcht_list_val_t(type=switcht_value_type.UINT32, u32data=2000))
        self.nhop_label_2 = self.add_nexthop(self.device, type=SWITCH_NEXTHOP_ATTR_TYPE_MPLS, handle=self.egress_rif, dest_ip='100::2', labelstack=label_list, mpls_encap_ttl_mode=SWITCH_NEXTHOP_ATTR_MPLS_ENCAP_TTL_MODE_PIPE_MODEL, mpls_encap_ttl=63, mpls_encap_qos_mode=SWITCH_NEXTHOP_ATTR_MPLS_ENCAP_QOS_MODE_PIPE_MODEL, mpls_encap_exp=5, labelop=SWITCH_NEXTHOP_ATTR_LABELOP_PUSH)

        label_list.append(switcht_list_val_t(type=switcht_value_type.UINT32, u32data=3000))
        self.nhop_label_3 = self.add_nexthop(self.device, type=SWITCH_NEXTHOP_ATTR_TYPE_MPLS, handle=self.egress_rif, dest_ip='100::3', labelstack=label_list, mpls_encap_ttl_mode=SWITCH_NEXTHOP_ATTR_MPLS_ENCAP_TTL_MODE_PIPE_MODEL, mpls_encap_ttl=63, mpls_encap_qos_mode=SWITCH_NEXTHOP_ATTR_MPLS_ENCAP_QOS_MODE_PIPE_MODEL, mpls_encap_exp=5, labelop=SWITCH_NEXTHOP_ATTR_LABELOP_PUSH)

        self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=self.egress_rif, dest_ip='100::1')
        self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=self.egress_rif, dest_ip='100::2')
        self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=self.egress_rif, dest_ip='100::3')

        self.mpls_encap_route_1 = self.add_route(self.device, ip_prefix='110::1', vrf_handle=self.vrf10, nexthop_handle=self.nhop_label_1)
        self.mpls_encap_route_2 = self.add_route(self.device, ip_prefix='120::1', vrf_handle=self.vrf10, nexthop_handle=self.nhop_label_2)
        self.mpls_encap_route_3 = self.add_route(self.device, ip_prefix='130::1', vrf_handle=self.vrf10, nexthop_handle=self.nhop_label_3)

        #MPLS Egress LER Term configs
        self.egress_rif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port2, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.mpls_rif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_MPLS, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.nhop = self.add_nexthop(self.device, type=SWITCH_NEXTHOP_ATTR_TYPE_IP, handle=self.egress_rif, dest_ip='220::1')
        self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=self.egress_rif, dest_ip='220::1')
        self.route = self.add_route(self.device, ip_prefix='200::1', vrf_handle=self.vrf10, nexthop_handle=self.nhop)
        self.mpls_object1000 = self.add_mpls(self.device, label=1000, nexthop_rif_handle=self.mpls_rif, num_pop=1, pop_ttl_mode=SWITCH_MPLS_ATTR_POP_TTL_MODE_PIPE_MODEL, pop_qos_mode=SWITCH_MPLS_ATTR_POP_QOS_MODE_PIPE_MODEL)
        #MPLS Egress LER NULL Term configs
        self.mpls_object0 = self.add_mpls(self.device, label=0, nexthop_rif_handle=self.mpls_rif, num_pop=1, pop_ttl_mode=SWITCH_MPLS_ATTR_POP_TTL_MODE_PIPE_MODEL, pop_qos_mode=SWITCH_MPLS_ATTR_POP_QOS_MODE_PIPE_MODEL)
        #MPLS EgressPHP configs
        self.egress_rif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port3, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.ip_nhop = self.add_nexthop(self.device, type=SWITCH_NEXTHOP_ATTR_TYPE_IP, handle=self.egress_rif, dest_ip='220::1')
        self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=self.egress_rif, dest_ip='220::1')
        self.mpls_object2000 = self.add_mpls(self.device, label=2000, nexthop_rif_handle=self.ip_nhop, num_pop=1, pop_ttl_mode=SWITCH_MPLS_ATTR_POP_TTL_MODE_PIPE_MODEL, pop_qos_mode=SWITCH_MPLS_ATTR_POP_QOS_MODE_PIPE_MODEL)
        self.mpls_object2111 = self.add_mpls(self.device, label=2111, nexthop_rif_handle=self.ip_nhop, num_pop=2, pop_ttl_mode=SWITCH_MPLS_ATTR_POP_TTL_MODE_PIPE_MODEL, pop_qos_mode=SWITCH_MPLS_ATTR_POP_QOS_MODE_PIPE_MODEL)
        self.mpls_object2222 = self.add_mpls(self.device, label=2222, nexthop_rif_handle=self.ip_nhop, num_pop=3, pop_ttl_mode=SWITCH_MPLS_ATTR_POP_TTL_MODE_PIPE_MODEL, pop_qos_mode=SWITCH_MPLS_ATTR_POP_QOS_MODE_PIPE_MODEL)

        #MPLS Egress PHP Swap with NULL configs
        self.egress_rif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port4, vrf_handle=self.vrf10, src_mac=self.rmac)
        label_list = []
        label_list.append(switcht_list_val_t(type=switcht_value_type.UINT32, u32data=0))
        self.mpls_null_nhop = self.add_nexthop(self.device, type=SWITCH_NEXTHOP_ATTR_TYPE_MPLS, handle=self.egress_rif, dest_ip='330::1', labelstack=label_list, mpls_encap_ttl=63, labelop=SWITCH_NEXTHOP_ATTR_LABELOP_SWAP)
        self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=self.egress_rif, dest_ip='330::1')
        self.mpls_object3000 = self.add_mpls(self.device, label=3000, nexthop_rif_handle=self.mpls_null_nhop)
        #MPLS Egress Transit Swap configs
        self.egress_rif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port6, vrf_handle=self.vrf10, src_mac=self.rmac)
        label_list = []
        label_list.append(switcht_list_val_t(type=switcht_value_type.UINT32, u32data=0))
        self.mpls_transit_nhop = self.add_nexthop(self.device, type=SWITCH_NEXTHOP_ATTR_TYPE_MPLS, handle=self.egress_rif, dest_ip='550::1', labelstack=label_list,labelop=SWITCH_NEXTHOP_ATTR_LABELOP_SWAP)
        self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=self.egress_rif, dest_ip='550::1')
        self.mpls_object5000 = self.add_mpls(self.device, label=5000, nexthop_rif_handle=self.mpls_transit_nhop)
        #MPLS Transit swap ECMP hash configs
        self.egress_rif_port7 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port7, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.egress_rif_port8 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port8, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.egress_rif_port9 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port9, vrf_handle=self.vrf10, src_mac=self.rmac)
        label_list = []
        label_list.append(switcht_list_val_t(type=switcht_value_type.UINT32, u32data=7070))
        self.mpls_transit_nhop1 = self.add_nexthop(self.device, type=SWITCH_NEXTHOP_ATTR_TYPE_MPLS, handle=self.egress_rif_port7, dest_ip='700::1', labelstack=label_list,labelop=SWITCH_NEXTHOP_ATTR_LABELOP_SWAP)
        label_list = []
        label_list.append(switcht_list_val_t(type=switcht_value_type.UINT32, u32data=7171))
        self.mpls_transit_nhop2 = self.add_nexthop(self.device, type=SWITCH_NEXTHOP_ATTR_TYPE_MPLS, handle=self.egress_rif_port8, dest_ip='710::1', labelstack=label_list,labelop=SWITCH_NEXTHOP_ATTR_LABELOP_SWAP)
        label_list = []
        label_list.append(switcht_list_val_t(type=switcht_value_type.UINT32, u32data=7272))
        self.mpls_transit_nhop3 = self.add_nexthop(self.device, type=SWITCH_NEXTHOP_ATTR_TYPE_MPLS, handle=self.egress_rif_port9, dest_ip='720::1', labelstack=label_list,labelop=SWITCH_NEXTHOP_ATTR_LABELOP_SWAP)

        self.ecmp = self.add_ecmp(self.device)
        self.ecmp_mbr1 = self.add_ecmp_member(self.device, ecmp_handle=self.ecmp, nexthop_handle=self.mpls_transit_nhop1)
        self.ecmp_mbr2 = self.add_ecmp_member(self.device, ecmp_handle=self.ecmp, nexthop_handle=self.mpls_transit_nhop2)
        self.ecmp_mbr3 = self.add_ecmp_member(self.device, ecmp_handle=self.ecmp, nexthop_handle=self.mpls_transit_nhop3)

        self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=self.egress_rif_port7, dest_ip='700::1')
        self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=self.egress_rif_port8, dest_ip='710::1')
        self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=self.egress_rif_port9, dest_ip='720::1')

        self.mpls_object7000 = self.add_mpls(self.device, label=7000, nexthop_rif_handle=self.ecmp)

    def runTest(self):
        if (self.client.is_feature_enable(SWITCH_FEATURE_MPLS) == 0):
            print("MPLS feature not enabled, skipping")
            return
        try:
            self.MplsIngressLER()
            self.MplsEgressLERTerm()
            self.MplsEgressLERNullTerm()
            self.MplsEgressPhp()
            self.MplsEgressPhpSwapNull()
            self.MplsTransitSwap()
            self.MplsTransitSwapEcmpHash()
            self.MplsAdminStateTest()
        finally:
            self.cleanup()

    def tearDown(self):
        self.cleanup()

    def MplsIngressLER(self):
        send_pkt_1 = simple_tcpv6_packet(
            eth_dst=self.rmac,
            ipv6_dst='110::1',
            ipv6_hlim=64)

        send_pkt_2 = simple_tcpv6_packet(
            eth_dst=self.rmac,
            ipv6_dst='120::1',
            ipv6_hlim=64)

        send_pkt_3 = simple_tcpv6_packet(
            eth_dst=self.rmac,
            ipv6_dst='130::1',
            ipv6_hlim=64)

        exact_pkt_1 = simple_tcpv6_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src=self.rmac,
            ipv6_dst='110::1',
            ipv6_hlim=63)
        exact_pkt_2 = simple_tcpv6_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src=self.rmac,
            ipv6_dst='120::1',
            ipv6_hlim=63)
        exact_pkt_3 = simple_tcpv6_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src=self.rmac,
            ipv6_dst='130::1',
            ipv6_hlim=63)
        mpls_tag_list = []
        mpls_tag_1={'label':1000, 'ttl':63, 'tc': 5, 's': 0}
        mpls_tag_2={'label':2000, 'ttl':63, 'tc': 5, 's': 0}
        mpls_tag_3={'label':3000, 'ttl':63, 'tc': 5, 's' : 0}
        mpls_tag_1['s'] = 1
        mpls_tag_list.append(mpls_tag_1)
        mpls_pkt_label_1 = simple_mpls_packet(eth_src=self.rmac, eth_dst='00:11:22:33:44:55', mpls_tags=mpls_tag_list, inner_frame=exact_pkt_1['IPv6'])
        mpls_tag_1['s'] = 0
        mpls_tag_2['s'] = 1
        del mpls_tag_list [:]
        mpls_tag_list.append(mpls_tag_1)
        mpls_tag_list.append(mpls_tag_2)
        mpls_pkt_label_2 = simple_mpls_packet(eth_src=self.rmac, eth_dst='00:11:22:33:44:55', mpls_tags=mpls_tag_list, inner_frame=exact_pkt_2['IPv6'])
        mpls_tag_1['s'] = 0
        mpls_tag_2['s'] = 0
        mpls_tag_3['s'] = 1
        del mpls_tag_list [:]
        mpls_tag_list.append(mpls_tag_1)
        mpls_tag_list.append(mpls_tag_2)
        mpls_tag_list.append(mpls_tag_3)
        mpls_pkt_label_3 = simple_mpls_packet(eth_src=self.rmac, eth_dst='00:11:22:33:44:55', mpls_tags=mpls_tag_list, inner_frame=exact_pkt_3['IPv6'])

        try:
            print("Send ip packet to add one MPLS label 1000")
            send_packet(self, self.devports[0], send_pkt_1)
            verify_packet(self, mpls_pkt_label_1, self.devports[1])

            print("Send ip packet to add two MPLS label stack - 1000, 2000")
            send_packet(self, self.devports[0], send_pkt_2)
            verify_packet(self, mpls_pkt_label_2, self.devports[1])

            print("Send ip packet to add three MPLS label stack - 1000, 2000, 3000")
            send_packet(self, self.devports[0], send_pkt_3)
            verify_packet(self, mpls_pkt_label_3, self.devports[1])

        finally:
            pass

    def MplsEgressLERTerm(self):
        send_pkt = simple_tcpv6_packet(
            eth_dst=self.rmac,
            ipv6_dst='200::1',
            ipv6_hlim=64)
        mpls_tag ={'label':1000, 'ttl':63, 'tc': 0, 's': 1}
        mpls_tag_list = []
        mpls_tag_list.append(mpls_tag)
        mpls_pkt = simple_mpls_packet(eth_dst=self.rmac, mpls_tags=mpls_tag_list, inner_frame=send_pkt['IPv6'])
        recv_pkt = simple_tcpv6_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src=self.rmac,
            ipv6_dst='200::1',
            ipv6_hlim=63)
        try:
            print("Send MPLS tag packet with label 1000 - term and IP lookup")
            send_packet(self, self.devports[0], mpls_pkt)
            verify_packet(self, recv_pkt, self.devports[2])

            self._MplsCounterTest(self.mpls_object1000)
        finally:
            pass

    def MplsEgressLERNullTerm(self):
        send_pkt = simple_tcpv6_packet(
            eth_dst=self.rmac,
            ipv6_dst='200::1',
            ipv6_hlim=64)
        mpls_tag ={'label':0, 'ttl':63, 'tc': 0, 's': 1}
        mpls_tag_list = []
        mpls_tag_list.append(mpls_tag)
        mpls_pkt = simple_mpls_packet(eth_dst=self.rmac, mpls_tags=mpls_tag_list, inner_frame=send_pkt['IPv6'])
        recv_pkt = simple_tcpv6_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src=self.rmac,
            ipv6_dst='200::1',
            ipv6_hlim=63)
        try:
            print("Send MPLS tag packet with label 0 - term and IP lookup")
            send_packet(self, self.devports[0], mpls_pkt)
            verify_packet(self, recv_pkt, self.devports[2])

            self._MplsCounterTest(self.mpls_object0)
        finally:
            pass

    def MplsEgressPhp(self):
        send_pkt = simple_tcpv6_packet(
            eth_dst=self.rmac,
            ipv6_dst='300::1',
            ipv6_hlim=64)
        mpls_tag ={'label':2000, 'ttl':63, 'tc': 0, 's': 1}
        mpls_tag_list = []
        mpls_tag_list.append(mpls_tag)
        mpls_pkt = simple_mpls_packet(eth_dst=self.rmac, mpls_tags=mpls_tag_list, inner_frame=send_pkt['IPv6'])

        mpls_tag_2_list = []
        mpls_tag_2 = {'label':2111, 'ttl':63, 'tc':0, 's':0}

        mpls_tag_2_list.append(mpls_tag_2)
        mpls_tag_2_list.append(mpls_tag)
        mpls_tag_2_pkt = simple_mpls_packet(eth_dst=self.rmac, mpls_tags=mpls_tag_2_list, inner_frame=send_pkt['IPv6'])

        mpls_tag_3_list = []
        mpls_tag_3 = {'label':2222, 'ttl':63, 'tc':0, 's':0}
        mpls_tag_3_list.append(mpls_tag_3)
        mpls_tag_3_list.append(mpls_tag_2)
        mpls_tag_3_list.append(mpls_tag)
        mpls_tag_3_pkt = simple_mpls_packet(eth_dst=self.rmac, mpls_tags=mpls_tag_3_list, inner_frame=send_pkt['IPv6'])

        recv_pkt = simple_tcpv6_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src=self.rmac,
            ipv6_dst='300::1',
            ipv6_hlim=63)
        recv_mpls_tag_list = []
        recv_mpls_tag_list.append(mpls_tag)
        recv_mpls_pkt = simple_mpls_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src=self.rmac,
            mpls_tags=recv_mpls_tag_list,
            inner_frame=send_pkt['IPv6'])

        try:
            print("Send MPLS tag packet with label 2000 - PHP and forward IP packet")
            send_packet(self, self.devports[0], mpls_pkt)
            verify_packet(self, recv_pkt, self.devports[3])

            self._MplsCounterTest(self.mpls_object2000)

            print("Send MPLS tag packet with labels - 2111, 2000 - PHP and forward IP packet")
            send_packet(self, self.devports[0], mpls_tag_2_pkt)
            verify_packet(self, recv_pkt, self.devports[3])

            self._MplsCounterTest(self.mpls_object2111)

            print("Send MPLS tag packet with labels - 2222, 2111, 2000 - PHP and forward IP packet")
            send_packet(self, self.devports[0], mpls_tag_3_pkt)
            verify_packet(self, recv_pkt, self.devports[3])

            self._MplsCounterTest(self.mpls_object2222)

            print("Set MPLS object label 2222 to pop only 2 labels")
            self.attribute_set(self.mpls_object2222, SWITCH_MPLS_ATTR_NUM_POP, 2)
            print("Send MPLS tag packet with labels - 2222, 2111, 2000 - Pop 2 labels and forward MPLS packet with 2000")
            send_packet(self, self.devports[0], mpls_tag_3_pkt)
            verify_packet(self, recv_mpls_pkt, self.devports[3])
            self.attribute_set(self.mpls_object2222, SWITCH_MPLS_ATTR_NUM_POP, 3)
        finally:
            pass

    def MplsEgressPhpSwapNull(self):
        send_pkt = simple_tcpv6_packet(
            eth_dst=self.rmac,
            ipv6_dst='400::1',
            ipv6_hlim=64)
        mpls_tag ={'label':3000, 'ttl':64, 'tc': 0, 's': 1}
        mpls_tag_list = []
        mpls_tag_list.append(mpls_tag)
        mpls_pkt = simple_mpls_packet(eth_dst=self.rmac, mpls_tags=mpls_tag_list, inner_frame=send_pkt['IPv6'])
        recv_pkt = simple_tcpv6_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src=self.rmac,
            ipv6_dst='400::1',
            ipv6_hlim=64)
        null_mpls_tag ={'label':0, 'ttl':63, 'tc': 0, 's': 1}
        null_mpls_tag_list = []
        null_mpls_tag_list.append(null_mpls_tag)
        recv_mpls_pkt = simple_mpls_packet(eth_src=self.rmac, eth_dst='00:11:22:33:44:55', mpls_tags=null_mpls_tag_list, inner_frame=send_pkt['IPv6'])
        try:
            print("Send MPLS tag packet with label 3000 - PHP and forward packet with explicit NULL label")
            send_packet(self, self.devports[0], mpls_pkt)
            verify_packet(self, recv_mpls_pkt, self.devports[4])

            self._MplsCounterTest(self.mpls_object3000)
        finally:
            pass

    def MplsTransitSwap(self):
        send_pkt = simple_tcpv6_packet(
            eth_dst=self.rmac,
            ipv6_dst='660::1',
            ipv6_hlim=64)
        mpls_tag ={'label':5000, 'ttl':64, 'tc': 3, 's': 1}
        mpls_tag_list = []
        mpls_tag_list.append(mpls_tag)
        mpls_pkt = simple_mpls_packet(eth_dst=self.rmac, mpls_tags=mpls_tag_list, inner_frame=send_pkt['IPv6'])
        recv_pkt = simple_tcpv6_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src=self.rmac,
            ipv6_dst='660::1',
            ipv6_hlim=64)
        mpls_tag ={'label':0, 'ttl':63, 'tc': 3, 's': 1}
        mpls_tag_list = []
        mpls_tag_list.append(mpls_tag)
        recv_mpls_pkt = simple_mpls_packet(eth_src=self.rmac, eth_dst='00:11:22:33:44:55', mpls_tags=mpls_tag_list, inner_frame=send_pkt['IPv6'])
        mpls_tag['label'] = 6000
        new_mpls_tag_list = []
        new_mpls_tag_list.append(mpls_tag)
        recv_mpls_pkt_6000 = simple_mpls_packet(eth_src=self.rmac, eth_dst='00:11:22:33:44:55', mpls_tags=new_mpls_tag_list, inner_frame=send_pkt['IPv6'])
        mpls_tag_1={'label':5000, 'ttl':64, 'tc': 5, 's': 0}
        mpls_tag_2={'label':4000, 'ttl':64, 'tc': 5, 's': 0}
        mpls_tag_3={'label':3000, 'ttl':64, 'tc': 5, 's': 1}
        mpls_3_tag_list = []
        mpls_3_tag_list.append(mpls_tag_1)
        mpls_3_tag_list.append(mpls_tag_2)
        mpls_3_tag_list.append(mpls_tag_3)
        mpls_3_tag_pkt = simple_mpls_packet(eth_dst=self.rmac, mpls_tags=mpls_3_tag_list, inner_frame=send_pkt['IPv6'])
        mpls_swap_tag={'label':6000, 'ttl':63, 'tc': 5, 's': 0}
        recv_mpls_3_tag = []
        recv_mpls_3_tag.append(mpls_swap_tag)
        recv_mpls_3_tag.append(mpls_tag_2)
        recv_mpls_3_tag.append(mpls_tag_3)
        recv_mpls_3_tag_pkt = simple_mpls_packet(eth_src=self.rmac, eth_dst='00:11:22:33:44:55', mpls_tags=recv_mpls_3_tag, inner_frame=send_pkt['IPv6'])

        try:
            print("Send MPLS tag 5000 packet and swap with explicit NULL")
            send_packet(self, self.devports[0], mpls_pkt)
            verify_packet(self, recv_mpls_pkt, self.devports[6])
            new_label = []
            print("Set MPLS nexthop to swap with label 6000")
            new_label.append(switcht_list_val_t(type=switcht_value_type.UINT32, u32data=6000))
            self.attribute_set(self.mpls_transit_nhop, SWITCH_NEXTHOP_ATTR_LABELSTACK, new_label)
            send_packet(self, self.devports[0], mpls_pkt)
            verify_packet(self, recv_mpls_pkt_6000, self.devports[6])
            print("Send 3-tagged packet and swap top label 5000 with 6000")
            send_packet(self, self.devports[0], mpls_3_tag_pkt)
            verify_packet(self, recv_mpls_3_tag_pkt, self.devports[6])
            nullnew_label = []
            print("Set MPLS nexthop to swap with label 6000")
            nullnew_label.append(switcht_list_val_t(type=switcht_value_type.UINT32, u32data=0))
            self.attribute_set(self.mpls_transit_nhop, SWITCH_NEXTHOP_ATTR_LABELSTACK, nullnew_label)

            self._MplsCounterTest(self.mpls_object5000, pkt_no=3)
        finally:
            pass

    def MplsTransitSwapEcmpHash(self):
        send_pkt = simple_tcpv6_packet(
            eth_dst=self.rmac,
            ipv6_dst='660::1',
            ipv6_hlim=64)
        mpls_tag ={'label':7000, 'ttl':64, 'tc': 3, 's': 1}
        mpls_tag_list = []
        mpls_tag_list.append(mpls_tag)
        mpls_send_pkt = simple_mpls_packet(eth_dst=self.rmac, mpls_tags=mpls_tag_list, inner_frame=send_pkt['IPv6'])

        recv_pkt = simple_tcpv6_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src=self.rmac,
            ipv6_dst='660::1',
            ipv6_hlim=64)
        mpls_swap_tag_7070={'label':7070, 'ttl':63, 'tc': 3, 's': 1}
        mpls_tag_7070 = []
        mpls_tag_7070.append(mpls_swap_tag_7070)
        mpls_swap_tag_7171={'label':7171, 'ttl':63, 'tc': 3, 's': 1}
        mpls_tag_7171 = []
        mpls_tag_7171.append(mpls_swap_tag_7171)
        mpls_swap_tag_7272={'label':7272, 'ttl':63, 'tc': 3, 's': 1}
        mpls_tag_7272 = []
        mpls_tag_7272.append(mpls_swap_tag_7272)

        recv_mpls_7070 = simple_mpls_packet(eth_src=self.rmac, eth_dst='00:11:22:33:44:55', mpls_tags=mpls_tag_7070, inner_frame=send_pkt['IPv6'])
        recv_mpls_7171 = simple_mpls_packet(eth_src=self.rmac, eth_dst='00:11:22:33:44:55', mpls_tags=mpls_tag_7171, inner_frame=send_pkt['IPv6'])
        recv_mpls_7272 = simple_mpls_packet(eth_src=self.rmac, eth_dst='00:11:22:33:44:55', mpls_tags=mpls_tag_7272, inner_frame=send_pkt['IPv6'])

        self.rif7_start_count = self.client.object_counters_get(self.egress_rif_port7)
        self.rif8_start_count = self.client.object_counters_get(self.egress_rif_port8)
        self.rif9_start_count = self.client.object_counters_get(self.egress_rif_port9)

        try:
            print("ECMP - Send MPLS tag packet 7000")
            send_packet(self, self.devports[0], mpls_send_pkt)
            verify_any_packet_any_port(self, [recv_mpls_7070, recv_mpls_7171, recv_mpls_7272], [self.devports[7], self.devports[8], self.devports[9]])
            self.rif7_end_count = self.client.object_counters_get(self.egress_rif_port7)
            self.rif8_end_count = self.client.object_counters_get(self.egress_rif_port8)
            self.rif9_end_count = self.client.object_counters_get(self.egress_rif_port9)
            rif_cntr_id = SWITCH_RIF_COUNTER_ID_OUT_UCAST_PKTS
            if(self.rif7_end_count[rif_cntr_id].count - self.rif7_start_count[rif_cntr_id].count == 1):
                self.final_egress_rif = self.egress_rif_port7
            elif(self.rif8_end_count[rif_cntr_id].count - self.rif8_start_count[rif_cntr_id].count == 1):
                self.final_egress_rif = self.egress_rif_port8
            elif(self.rif9_end_count[rif_cntr_id].count - self.rif9_start_count[rif_cntr_id].count == 1):
                self.final_egress_rif = self.egress_rif_port9
            else:
                self.final_egress_rif = 0

            iter_count = 10
            self.rif_lb_start_count = self.client.object_counters_get(self.final_egress_rif)
            print("Start RIF count %d"%(self.rif_lb_start_count[rif_cntr_id].count))

            for i in range(0, iter_count):
                l4_sport = random.randint(5000, 65535)
                l4_dport = random.randint(5000, 65535)
                mpls_send_pkt['TCP'].sport = l4_sport
                mpls_send_pkt['TCP'].dport = l4_dport
                send_packet(self, self.devports[0], mpls_send_pkt)

            time.sleep(2)
            self.rif_lb_end_count = self.client.object_counters_get(self.final_egress_rif)
            diff = self.rif_lb_end_count[rif_cntr_id].count - self.rif_lb_start_count[rif_cntr_id].count
            self.assertTrue(self.rif_lb_end_count[rif_cntr_id].count - self.rif_lb_start_count[rif_cntr_id].count == iter_count)

        finally:
            self.dataplane.flush()

    def _MplsCounterTest(self, mpls_id, pkt_no=1):
        counters = self.client.object_counters_get(mpls_id)
        for cntr in counters:
            if cntr.counter_id == SWITCH_MPLS_COUNTER_ID_PKTS:
                self.assertEqual(cntr.count, pkt_no, "Improper packet counter value")
            elif cntr.counter_id == SWITCH_MPLS_COUNTER_ID_BYTES:
                self.assertNotEqual(cntr.count, 0, "Bytes counter value should be non-zero")
            else:
                self.fail("Unknown MPLS packet counter ID")

        self.client.object_counters_clear(mpls_id, [SWITCH_MPLS_COUNTER_ID_BYTES])
        counters = self.client.object_counters_get(mpls_id)
        for cntr in counters:
            if cntr.counter_id == SWITCH_MPLS_COUNTER_ID_PKTS:
                self.assertEqual(pkt_no, cntr.count, "Improper packet counter value")
            elif cntr.counter_id == SWITCH_MPLS_COUNTER_ID_BYTES:
                self.assertEqual(cntr.count, 0, "Bytes counter value should be zero")
            else:
                self.fail("Unknown sidlist packet counter ID")

        self.client.object_counters_clear_all(mpls_id)
        counters = self.client.object_counters_get(mpls_id)
        for cntr in counters:
            self.assertEqual(cntr.count, 0, "sidlist counters not cleared")

    def MplsAdminStateTest(self):
        print("Setting ingress RIF MPLS state to Down")
        self.attribute_set(self.ingress_rif, SWITCH_RIF_ATTR_MPLS_STATE, False)

        send_pkt = simple_tcpv6_packet(
            eth_dst=self.rmac,
            ipv6_dst='200::1',
            ipv6_hlim=64)
        mpls_tag ={'label':1000, 'ttl':63, 'tc': 0, 's': 1}
        mpls_tag_list = []
        mpls_tag_list.append(mpls_tag)
        mpls_pkt = simple_mpls_packet(eth_dst=self.rmac, mpls_tags=mpls_tag_list, inner_frame=send_pkt['IPv6'])
        recv_pkt = simple_tcpv6_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src=self.rmac,
            ipv6_dst='200::1',
            ipv6_hlim=63)

        self.client.object_counters_clear_all(self.port0)

        try:
            print("Send MPLS tag packet - drop")
            send_packet(self, self.devports[0], mpls_pkt)
            verify_no_other_packets(self)

            print("Setting ingress RIF MPLS state to Up")
            self.attribute_set(self.ingress_rif, SWITCH_RIF_ATTR_MPLS_STATE, True)

            print("Send MPLS tag packet - forward")
            send_packet(self, self.devports[0], mpls_pkt)
            verify_packet(self, recv_pkt, self.devports[2])

            print("Verify port stats")
            port_stats = self.object_counters_get(self.port0)
            x = SWITCH_PORT_COUNTER_ID_IF_IN_MPLS_DISABLE_DISCARDS
            self.assertEqual(port_stats[x].count, 1)

        finally:
            pass

class MplsIpv4Test(ApiHelper):
    def setUp(self):
        self.configure()
        if (self.client.is_feature_enable(SWITCH_FEATURE_MPLS) == 0):
            print("MPLS feature not enabled, skipping")
            return
        #Add TTL_ERROR trap
        self.ttl_error=self.add_hostif_trap(self.device, type=SWITCH_HOSTIF_TRAP_ATTR_TYPE_TTL_ERROR, packet_action=SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_DROP)
        #Ingress LER configs
        self.ingress_rif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port0, vrf_handle=self.vrf10, src_mac=self.rmac, mpls_state=True)
        self.egress_rif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port1, vrf_handle=self.vrf10, src_mac=self.rmac)
        label_list = []
        label_list.append(switcht_list_val_t(type=switcht_value_type.UINT32, u32data=1000))
        self.nhop_label_1 = self.add_nexthop(self.device, type=SWITCH_NEXTHOP_ATTR_TYPE_MPLS, handle=self.egress_rif, dest_ip='10.10.0.1', labelstack=label_list, mpls_encap_ttl_mode=SWITCH_NEXTHOP_ATTR_MPLS_ENCAP_TTL_MODE_PIPE_MODEL, mpls_encap_ttl=63, mpls_encap_qos_mode=SWITCH_NEXTHOP_ATTR_MPLS_ENCAP_QOS_MODE_PIPE_MODEL, mpls_encap_exp=5, labelop=SWITCH_NEXTHOP_ATTR_LABELOP_PUSH)

        label_list.append(switcht_list_val_t(type=switcht_value_type.UINT32, u32data=2000))
        self.nhop_label_2 = self.add_nexthop(self.device, type=SWITCH_NEXTHOP_ATTR_TYPE_MPLS, handle=self.egress_rif, dest_ip='10.10.0.2', labelstack=label_list, mpls_encap_ttl_mode=SWITCH_NEXTHOP_ATTR_MPLS_ENCAP_TTL_MODE_PIPE_MODEL, mpls_encap_ttl=63, mpls_encap_qos_mode=SWITCH_NEXTHOP_ATTR_MPLS_ENCAP_QOS_MODE_PIPE_MODEL, mpls_encap_exp=5, labelop=SWITCH_NEXTHOP_ATTR_LABELOP_PUSH)

        label_list.append(switcht_list_val_t(type=switcht_value_type.UINT32, u32data=3000))
        self.nhop_label_3 = self.add_nexthop(self.device, type=SWITCH_NEXTHOP_ATTR_TYPE_MPLS, handle=self.egress_rif, dest_ip='10.10.0.3', labelstack=label_list, mpls_encap_ttl_mode=SWITCH_NEXTHOP_ATTR_MPLS_ENCAP_TTL_MODE_PIPE_MODEL, mpls_encap_ttl=63, mpls_encap_qos_mode=SWITCH_NEXTHOP_ATTR_MPLS_ENCAP_QOS_MODE_PIPE_MODEL, mpls_encap_exp=5, labelop=SWITCH_NEXTHOP_ATTR_LABELOP_PUSH)

        self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=self.egress_rif, dest_ip='10.10.0.1')
        self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=self.egress_rif, dest_ip='10.10.0.2')
        self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=self.egress_rif, dest_ip='10.10.0.3')

        self.mpls_encap_route_1 = self.add_route(self.device, ip_prefix='10.10.10.1', vrf_handle=self.vrf10, nexthop_handle=self.nhop_label_1)
        self.mpls_encap_route_2 = self.add_route(self.device, ip_prefix='10.10.10.2', vrf_handle=self.vrf10, nexthop_handle=self.nhop_label_2)
        self.mpls_encap_route_3 = self.add_route(self.device, ip_prefix='10.10.10.3', vrf_handle=self.vrf10, nexthop_handle=self.nhop_label_3)

        #Mpls Ingress LER Term configs
        self.egress_rif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port2, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.mpls_rif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_MPLS, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.nhop = self.add_nexthop(self.device, type=SWITCH_NEXTHOP_ATTR_TYPE_IP, handle=self.egress_rif, dest_ip='20.20.0.1')
        self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=self.egress_rif, dest_ip='20.20.0.1')
        self.route = self.add_route(self.device, ip_prefix='20.20.20.1', vrf_handle=self.vrf10, nexthop_handle=self.nhop)
        self.mpls_object1000 = self.add_mpls(self.device, label=1000, nexthop_rif_handle=self.mpls_rif, num_pop=1, pop_ttl_mode=SWITCH_MPLS_ATTR_POP_TTL_MODE_PIPE_MODEL, pop_qos_mode=SWITCH_MPLS_ATTR_POP_QOS_MODE_PIPE_MODEL)

        self.vrf20 = self.add_vrf(self.device, id=20, src_mac=self.rmac)
        self.egress_rif_vrf20 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port5, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.nhop_vrf20 = self.add_nexthop(self.device, type=SWITCH_NEXTHOP_ATTR_TYPE_IP, handle=self.egress_rif_vrf20, dest_ip='20.20.0.1')
        self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=self.egress_rif_vrf20, dest_ip='20.20.0.1')
        self.route_vrf20 = self.add_route(self.device, ip_prefix='20.20.20.1', vrf_handle=self.vrf20, nexthop_handle=self.nhop_vrf20)

        # MPLS Egress LER Null Term configs
        self.mpls_object0 = self.add_mpls(self.device, label=0, nexthop_rif_handle=self.mpls_rif, num_pop=1, pop_ttl_mode=SWITCH_MPLS_ATTR_POP_TTL_MODE_PIPE_MODEL, pop_qos_mode=SWITCH_MPLS_ATTR_POP_QOS_MODE_PIPE_MODEL)

        # MPLS Egress PHP configs
        self.egress_rif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port3, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.ip_nhop = self.add_nexthop(self.device, type=SWITCH_NEXTHOP_ATTR_TYPE_IP, handle=self.egress_rif, dest_ip='20.20.20.1')
        self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=self.egress_rif, dest_ip='20.20.20.1')
        self.mpls_object2000 = self.add_mpls(self.device, label=2000, nexthop_rif_handle=self.ip_nhop, num_pop=1, pop_ttl_mode=SWITCH_MPLS_ATTR_POP_TTL_MODE_PIPE_MODEL, pop_qos_mode=SWITCH_MPLS_ATTR_POP_QOS_MODE_PIPE_MODEL)
        self.mpls_object2111 = self.add_mpls(self.device, label=2111, nexthop_rif_handle=self.ip_nhop, num_pop=2, pop_ttl_mode=SWITCH_MPLS_ATTR_POP_TTL_MODE_PIPE_MODEL, pop_qos_mode=SWITCH_MPLS_ATTR_POP_QOS_MODE_PIPE_MODEL)
        self.mpls_object2222 = self.add_mpls(self.device, label=2222, nexthop_rif_handle=self.ip_nhop, num_pop=3, pop_ttl_mode=SWITCH_MPLS_ATTR_POP_TTL_MODE_PIPE_MODEL, pop_qos_mode=SWITCH_MPLS_ATTR_POP_QOS_MODE_PIPE_MODEL)
        # MPLS Egress PHP Swap Null configs
        self.egress_rif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port4, vrf_handle=self.vrf10, src_mac=self.rmac)
        label_list = []
        label_list.append(switcht_list_val_t(type=switcht_value_type.UINT32, u32data=0))
        self.mpls_null_nhop = self.add_nexthop(self.device, type=SWITCH_NEXTHOP_ATTR_TYPE_MPLS, handle=self.egress_rif, dest_ip='30.30.30.1', labelstack=label_list, mpls_encap_ttl=63, labelop=SWITCH_NEXTHOP_ATTR_LABELOP_SWAP)
        self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=self.egress_rif, dest_ip='30.30.30.1')
        self.mpls_object3000 = self.add_mpls(self.device, label=3000, nexthop_rif_handle=self.mpls_null_nhop)

        #MPLS TRansit Swap configs
        self.egress_rif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port6, vrf_handle=self.vrf10, src_mac=self.rmac)
        label_list = []
        label_list.append(switcht_list_val_t(type=switcht_value_type.UINT32, u32data=0))
        self.mpls_transit_nhop = self.add_nexthop(self.device, type=SWITCH_NEXTHOP_ATTR_TYPE_MPLS, handle=self.egress_rif, dest_ip='50.50.50.1', labelstack=label_list,labelop=SWITCH_NEXTHOP_ATTR_LABELOP_SWAP)
        self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=self.egress_rif, dest_ip='50.50.50.1')
        self.mpls_object5000 = self.add_mpls(self.device, label=5000, nexthop_rif_handle=self.mpls_transit_nhop)

        #MPLS ECMP Swap configs
        self.egress_rif_port7 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port7, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.egress_rif_port8 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port8, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.egress_rif_port9 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port9, vrf_handle=self.vrf10, src_mac=self.rmac)
        label_list = []
        label_list.append(switcht_list_val_t(type=switcht_value_type.UINT32, u32data=7070))
        self.mpls_transit_nhop1 = self.add_nexthop(self.device, type=SWITCH_NEXTHOP_ATTR_TYPE_MPLS, handle=self.egress_rif_port7, dest_ip='70.70.70.1', labelstack=label_list,labelop=SWITCH_NEXTHOP_ATTR_LABELOP_SWAP)
        label_list = []
        label_list.append(switcht_list_val_t(type=switcht_value_type.UINT32, u32data=7171))
        self.mpls_transit_nhop2 = self.add_nexthop(self.device, type=SWITCH_NEXTHOP_ATTR_TYPE_MPLS, handle=self.egress_rif_port8, dest_ip='71.71.71.1', labelstack=label_list,labelop=SWITCH_NEXTHOP_ATTR_LABELOP_SWAP)
        label_list = []
        label_list.append(switcht_list_val_t(type=switcht_value_type.UINT32, u32data=7272))
        self.mpls_transit_nhop3 = self.add_nexthop(self.device, type=SWITCH_NEXTHOP_ATTR_TYPE_MPLS, handle=self.egress_rif_port9, dest_ip='72.72.72.1', labelstack=label_list,labelop=SWITCH_NEXTHOP_ATTR_LABELOP_SWAP)

        self.ecmp = self.add_ecmp(self.device)
        self.ecmp_mbr1 = self.add_ecmp_member(self.device, ecmp_handle=self.ecmp, nexthop_handle=self.mpls_transit_nhop1)
        self.ecmp_mbr2 = self.add_ecmp_member(self.device, ecmp_handle=self.ecmp, nexthop_handle=self.mpls_transit_nhop2)
        self.ecmp_mbr3 = self.add_ecmp_member(self.device, ecmp_handle=self.ecmp, nexthop_handle=self.mpls_transit_nhop3)

        self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=self.egress_rif_port7, dest_ip='70.70.70.1')
        self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=self.egress_rif_port8, dest_ip='71.71.71.1')
        self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=self.egress_rif_port9, dest_ip='72.72.72.1')

        self.mpls_object7000 = self.add_mpls(self.device, label=7000, nexthop_rif_handle=self.ecmp)

    def runTest(self):
        if (self.client.is_feature_enable(SWITCH_FEATURE_MPLS) == 0):
            print("MPLS feature not enabled, skipping")
            return
        try:
            self.MplsIngressLER()
            self.MplsEgressLERTerm()
            self.MplsEgressLERTermUpdateMplsRifVrf()
            self.MplsEgressLERNullTerm()
            self.MplsEgressPhp()
            self.MplsEgressPhpSwapNull()
            self.MplsTransitSwap()
            self.MplsTransitSwapEcmpHash()
        finally:
            self.cleanup()

    def MplsIngressLER(self):
        send_pkt_1 = simple_tcp_packet(
            eth_dst=self.rmac,
            ip_dst='10.10.10.1',
            ip_ttl=64)

        send_pkt_2 = simple_tcp_packet(
            eth_dst=self.rmac,
            ip_dst='10.10.10.2',
            ip_ttl=64)

        send_pkt_3 = simple_tcp_packet(
            eth_dst=self.rmac,
            ip_dst='10.10.10.3',
            ip_ttl=64)

        exact_pkt_1 = simple_tcp_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src=self.rmac,
            ip_dst='10.10.10.1',
            ip_ttl=63)
        exact_pkt_2 = simple_tcp_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src=self.rmac,
            ip_dst='10.10.10.2',
            ip_ttl=63)
        exact_pkt_3 = simple_tcp_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src=self.rmac,
            ip_dst='10.10.10.3',
            ip_ttl=63)
        mpls_tag_list = []
        mpls_tag_1={'label':1000, 'ttl':63, 'tc': 5, 's': 0}
        mpls_tag_2={'label':2000, 'ttl':63, 'tc': 5, 's': 0}
        mpls_tag_3={'label':3000, 'ttl':63, 'tc': 5, 's' : 0}
        mpls_tag_1['s'] = 1
        mpls_tag_list.append(mpls_tag_1)
        mpls_pkt_label_1 = simple_mpls_packet(eth_src=self.rmac, eth_dst='00:11:22:33:44:55', mpls_tags=mpls_tag_list, inner_frame=exact_pkt_1['IP'])
        mpls_tag_1['s'] = 0
        mpls_tag_2['s'] = 1
        del mpls_tag_list [:]
        mpls_tag_list.append(mpls_tag_1)
        mpls_tag_list.append(mpls_tag_2)
        mpls_pkt_label_2 = simple_mpls_packet(eth_src=self.rmac, eth_dst='00:11:22:33:44:55', mpls_tags=mpls_tag_list, inner_frame=exact_pkt_2['IP'])
        mpls_tag_1['s'] = 0
        mpls_tag_2['s'] = 0
        mpls_tag_3['s'] = 1
        del mpls_tag_list [:]
        mpls_tag_list.append(mpls_tag_1)
        mpls_tag_list.append(mpls_tag_2)
        mpls_tag_list.append(mpls_tag_3)
        mpls_pkt_label_3 = simple_mpls_packet(eth_src=self.rmac, eth_dst='00:11:22:33:44:55', mpls_tags=mpls_tag_list, inner_frame=exact_pkt_3['IP'])

        try:
            print("Send ip packet to add one MPLS label 1000")
            send_packet(self, self.devports[0], send_pkt_1)
            verify_packet(self, mpls_pkt_label_1, self.devports[1])

            print("Send ip packet to add two MPLS label stack - 1000, 2000")
            send_packet(self, self.devports[0], send_pkt_2)
            verify_packet(self, mpls_pkt_label_2, self.devports[1])

            print("Send ip packet to add three MPLS label stack - 1000, 2000, 3000")
            send_packet(self, self.devports[0], send_pkt_3)
            verify_packet(self, mpls_pkt_label_3, self.devports[1])

        finally:
            pass

    def MplsEgressLERTerm(self):
        send_pkt = simple_tcp_packet(
            eth_dst=self.rmac,
            ip_dst='20.20.20.1',
            ip_ttl=64)
        mpls_tag ={'label':1000, 'ttl':63, 'tc': 0, 's': 1}
        mpls_tag_list = []
        mpls_tag_list.append(mpls_tag)
        mpls_pkt = simple_mpls_packet(eth_dst=self.rmac, mpls_tags=mpls_tag_list, inner_frame=send_pkt['IP'])
        recv_pkt = simple_tcp_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src=self.rmac,
            ip_dst='20.20.20.1',
            ip_ttl=63)
        try:
            print("Send MPLS tag packet with label 1000 - term and IP lookup")
            send_packet(self, self.devports[0], mpls_pkt)
            verify_packet(self, recv_pkt, self.devports[2])

            self._MplsCounterTest(self.mpls_object1000)
        finally:
            pass

    def MplsEgressLERTermUpdateMplsRifVrf(self):
        send_pkt = simple_tcp_packet(
            eth_dst=self.rmac,
            ip_dst='20.20.20.1',
            ip_ttl=64)
        mpls_tag ={'label':1000, 'ttl':63, 'tc': 0, 's': 1}
        mpls_tag_list = []
        mpls_tag_list.append(mpls_tag)
        mpls_pkt = simple_mpls_packet(eth_dst=self.rmac, mpls_tags=mpls_tag_list, inner_frame=send_pkt['IP'])
        recv_pkt = simple_tcp_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src=self.rmac,
            ip_dst='20.20.20.1',
            ip_ttl=63)
        try:
            print("Send MPLS tag packet with label 1000 - term and IP lookup")
            send_packet(self, self.devports[0], mpls_pkt)
            verify_packet(self, recv_pkt, self.devports[2])
            print("Update VRF on MPLS RIF to vrf20, term and forwrd to port5, vrf20")
            self.attribute_set(self.mpls_rif, SWITCH_RIF_ATTR_VRF_HANDLE, self.vrf20)
            send_packet(self, self.devports[0], mpls_pkt)
            verify_packet(self, recv_pkt, self.devports[5])

            self._MplsCounterTest(self.mpls_object1000)
        finally:
            self.attribute_set(self.mpls_rif, SWITCH_RIF_ATTR_VRF_HANDLE, self.vrf10)
            pass

    def MplsEgressLERNullTerm(self):
        send_pkt = simple_tcp_packet(
            eth_dst=self.rmac,
            ip_dst='20.20.20.1',
            ip_ttl=64)
        mpls_tag ={'label':0, 'ttl':63, 'tc': 0, 's': 1}
        mpls_tag_list = []
        mpls_tag_list.append(mpls_tag)
        mpls_pkt = simple_mpls_packet(eth_dst=self.rmac, mpls_tags=mpls_tag_list, inner_frame=send_pkt['IP'])
        recv_pkt = simple_tcp_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src=self.rmac,
            ip_dst='20.20.20.1',
            ip_ttl=63)
        try:
            print("Send MPLS tag packet with label 0 - term and IP lookup")
            send_packet(self, self.devports[0], mpls_pkt)
            verify_packet(self, recv_pkt, self.devports[2])

            self._MplsCounterTest(self.mpls_object0)
        finally:
            pass

    def MplsEgressPhp(self):
        send_pkt = simple_tcp_packet(
            eth_dst=self.rmac,
            ip_dst='30.30.30.1',
            ip_ttl=64)
        mpls_tag ={'label':2000, 'ttl':63, 'tc': 0, 's': 1}
        mpls_tag_list = []
        mpls_tag_list.append(mpls_tag)
        mpls_pkt = simple_mpls_packet(eth_dst=self.rmac, mpls_tags=mpls_tag_list, inner_frame=send_pkt['IP'])

        mpls_tag_2_list = []
        mpls_tag_2 = {'label':2111, 'ttl':63, 'tc':0, 's':0}

        mpls_tag_2_list.append(mpls_tag_2)
        mpls_tag_2_list.append(mpls_tag)
        mpls_tag_2_pkt = simple_mpls_packet(eth_dst=self.rmac, mpls_tags=mpls_tag_2_list, inner_frame=send_pkt['IP'])

        mpls_tag_3_list = []
        mpls_tag_3 = {'label':2222, 'ttl':63, 'tc':0, 's':0}
        mpls_tag_3_list.append(mpls_tag_3)
        mpls_tag_3_list.append(mpls_tag_2)
        mpls_tag_3_list.append(mpls_tag)
        mpls_tag_3_pkt = simple_mpls_packet(eth_dst=self.rmac, mpls_tags=mpls_tag_3_list, inner_frame=send_pkt['IP'])

        recv_pkt = simple_tcp_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src=self.rmac,
            ip_dst='30.30.30.1',
            ip_ttl=63)
        recv_mpls_tag_list = []
        recv_mpls_tag_list.append(mpls_tag)
        recv_mpls_pkt = simple_mpls_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src=self.rmac,
            mpls_tags=recv_mpls_tag_list,
            inner_frame=send_pkt['IP'])

        try:
            print("Send MPLS tag packet with label 2000 - PHP and forward IP packet")
            send_packet(self, self.devports[0], mpls_pkt)
            verify_packet(self, recv_pkt, self.devports[3])

            self._MplsCounterTest(self.mpls_object2000)

            print("Send MPLS tag packet with labels - 2111, 2000 - PHP and forward IP packet")
            send_packet(self, self.devports[0], mpls_tag_2_pkt)
            verify_packet(self, recv_pkt, self.devports[3])

            self._MplsCounterTest(self.mpls_object2111)

            print("Send MPLS tag packet with labels - 2222, 2111, 2000 - PHP and forward IP packet")
            send_packet(self, self.devports[0], mpls_tag_3_pkt)
            verify_packet(self, recv_pkt, self.devports[3])

            self._MplsCounterTest(self.mpls_object2222)

            print("Set MPLS object label 2222 to pop only 2 labels")
            self.attribute_set(self.mpls_object2222, SWITCH_MPLS_ATTR_NUM_POP, 2)
            print("Send MPLS tag packet with labels - 2222, 2111, 2000 - Pop 2 labels and forward MPLS packet with 2000")
            send_packet(self, self.devports[0], mpls_tag_3_pkt)
            verify_packet(self, recv_mpls_pkt, self.devports[3])

            self._MplsCounterTest(self.mpls_object2222)

            self.attribute_set(self.mpls_object2222, SWITCH_MPLS_ATTR_NUM_POP, 3)
        finally:
            pass

    def MplsEgressPhpSwapNull(self):
        send_pkt = simple_tcp_packet(
            eth_dst=self.rmac,
            ip_dst='40.40.40.1',
            ip_ttl=64)
        mpls_tag ={'label':3000, 'ttl':64, 'tc': 0, 's': 1}
        mpls_tag_list = []
        mpls_tag_list.append(mpls_tag)
        mpls_pkt = simple_mpls_packet(eth_dst=self.rmac, mpls_tags=mpls_tag_list, inner_frame=send_pkt['IP'])
        recv_pkt = simple_tcp_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src=self.rmac,
            ip_dst='40.40.40.1',
            ip_ttl=64)
        null_mpls_tag ={'label':0, 'ttl':63, 'tc': 0, 's': 1}
        null_mpls_tag_list = []
        null_mpls_tag_list.append(null_mpls_tag)
        recv_mpls_pkt = simple_mpls_packet(eth_src=self.rmac, eth_dst='00:11:22:33:44:55', mpls_tags=null_mpls_tag_list, inner_frame=send_pkt['IP'])
        try:
            print("Send MPLS tag packet with label 3000 - PHP and forward packet with explicit NULL label")
            send_packet(self, self.devports[0], mpls_pkt)
            verify_packet(self, recv_mpls_pkt, self.devports[4])

            self._MplsCounterTest(self.mpls_object3000)
        finally:
            pass

    def MplsTransitSwap(self):
        send_pkt = simple_tcp_packet(
            eth_dst=self.rmac,
            ip_dst='60.60.60.1',
            ip_ttl=64)
        mpls_tag ={'label':5000, 'ttl':64, 'tc': 3, 's': 1}
        mpls_tag_list = []
        mpls_tag_list.append(mpls_tag)
        mpls_pkt = simple_mpls_packet(eth_dst=self.rmac, mpls_tags=mpls_tag_list, inner_frame=send_pkt['IP'])
        recv_pkt = simple_tcp_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src=self.rmac,
            ip_dst='60.60.60.1',
            ip_ttl=64)
        mpls_tag ={'label':0, 'ttl':63, 'tc': 3, 's': 1}
        mpls_tag_list = []
        mpls_tag_list.append(mpls_tag)
        recv_mpls_pkt = simple_mpls_packet(eth_src=self.rmac, eth_dst='00:11:22:33:44:55', mpls_tags=mpls_tag_list, inner_frame=send_pkt['IP'])
        mpls_tag['label'] = 6000
        new_mpls_tag_list = []
        new_mpls_tag_list.append(mpls_tag)
        recv_mpls_pkt_6000 = simple_mpls_packet(eth_src=self.rmac, eth_dst='00:11:22:33:44:55', mpls_tags=new_mpls_tag_list, inner_frame=send_pkt['IP'])
        mpls_tag_1={'label':5000, 'ttl':64, 'tc': 5, 's': 0}
        mpls_tag_2={'label':4000, 'ttl':63, 'tc': 5, 's': 0}
        mpls_tag_3={'label':3000, 'ttl':63, 'tc': 5, 's': 1}
        mpls_3_tag_list = []
        mpls_3_tag_list.append(mpls_tag_1)
        mpls_3_tag_list.append(mpls_tag_2)
        mpls_3_tag_list.append(mpls_tag_3)
        mpls_3_tag_pkt = simple_mpls_packet(eth_dst=self.rmac, mpls_tags=mpls_3_tag_list, inner_frame=send_pkt['IP'])
        mpls_swap_tag={'label':6000, 'ttl':63, 'tc': 5, 's': 0}
        recv_mpls_3_tag = []
        recv_mpls_3_tag.append(mpls_swap_tag)
        recv_mpls_3_tag.append(mpls_tag_2)
        recv_mpls_3_tag.append(mpls_tag_3)
        recv_mpls_3_tag_pkt = simple_mpls_packet(eth_src=self.rmac, eth_dst='00:11:22:33:44:55', mpls_tags=recv_mpls_3_tag, inner_frame=send_pkt['IP'])

        try:
            print("Send MPLS tag 5000 packet and swap with explicit NULL")
            send_packet(self, self.devports[0], mpls_pkt)
            verify_packet(self, recv_mpls_pkt, self.devports[6])
            new_label = []
            print("Set MPLS nexthop to swap with label 6000")
            new_label.append(switcht_list_val_t(type=switcht_value_type.UINT32, u32data=6000))
            self.attribute_set(self.mpls_transit_nhop, SWITCH_NEXTHOP_ATTR_LABELSTACK, new_label)
            send_packet(self, self.devports[0], mpls_pkt)
            verify_packet(self, recv_mpls_pkt_6000, self.devports[6])
            print("Send 3-tagged packet and swap top label 5000 with 6000")
            send_packet(self, self.devports[0], mpls_3_tag_pkt)
            verify_packet(self, recv_mpls_3_tag_pkt, self.devports[6])
            new_label_null = []
            new_label_null.append(switcht_list_val_t(type=switcht_value_type.UINT32, u32data=0))
            self.attribute_set(self.mpls_transit_nhop, SWITCH_NEXTHOP_ATTR_LABELSTACK, new_label_null)

            self._MplsCounterTest(self.mpls_object5000, pkt_no=3)
        finally:
            pass

    def MplsTransitSwapEcmpHash(self):
        send_pkt = simple_tcp_packet(
            eth_dst=self.rmac,
            ip_dst='60.60.60.1',
            ip_ttl=64)
        mpls_tag ={'label':7000, 'ttl':64, 'tc': 3, 's': 1}
        mpls_tag_list = []
        mpls_tag_list.append(mpls_tag)
        mpls_send_pkt = simple_mpls_packet(eth_dst=self.rmac, mpls_tags=mpls_tag_list, inner_frame=send_pkt['IP'])

        recv_pkt = simple_tcp_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src=self.rmac,
            ip_dst='60.60.60.1',
            ip_ttl=64)
        mpls_swap_tag_7070={'label':7070, 'ttl':63, 'tc': 3, 's': 1}
        mpls_tag_7070 = []
        mpls_tag_7070.append(mpls_swap_tag_7070)
        mpls_swap_tag_7171={'label':7171, 'ttl':63, 'tc': 3, 's': 1}
        mpls_tag_7171 = []
        mpls_tag_7171.append(mpls_swap_tag_7171)
        mpls_swap_tag_7272={'label':7272, 'ttl':63, 'tc': 3, 's': 1}
        mpls_tag_7272 = []
        mpls_tag_7272.append(mpls_swap_tag_7272)

        recv_mpls_7070 = simple_mpls_packet(eth_src=self.rmac, eth_dst='00:11:22:33:44:55', mpls_tags=mpls_tag_7070, inner_frame=send_pkt['IP'])
        recv_mpls_7171 = simple_mpls_packet(eth_src=self.rmac, eth_dst='00:11:22:33:44:55', mpls_tags=mpls_tag_7171, inner_frame=send_pkt['IP'])
        recv_mpls_7272 = simple_mpls_packet(eth_src=self.rmac, eth_dst='00:11:22:33:44:55', mpls_tags=mpls_tag_7272, inner_frame=send_pkt['IP'])

        self.rif7_start_count = self.client.object_counters_get(self.egress_rif_port7)
        self.rif8_start_count = self.client.object_counters_get(self.egress_rif_port8)
        self.rif9_start_count = self.client.object_counters_get(self.egress_rif_port9)

        try:
            print("ECMP - Send MPLS tag packet 7000")
            send_packet(self, self.devports[0], mpls_send_pkt)
            verify_any_packet_any_port(self, [recv_mpls_7070, recv_mpls_7171, recv_mpls_7272], [self.devports[7], self.devports[8], self.devports[9]])
            self.rif7_end_count = self.client.object_counters_get(self.egress_rif_port7)
            self.rif8_end_count = self.client.object_counters_get(self.egress_rif_port8)
            self.rif9_end_count = self.client.object_counters_get(self.egress_rif_port9)
            rif_cntr_id = SWITCH_RIF_COUNTER_ID_OUT_UCAST_PKTS
            if(self.rif7_end_count[rif_cntr_id].count - self.rif7_start_count[rif_cntr_id].count == 1):
                self.final_egress_rif = self.egress_rif_port7
            elif(self.rif8_end_count[rif_cntr_id].count - self.rif8_start_count[rif_cntr_id].count == 1):
                self.final_egress_rif = self.egress_rif_port8
            elif(self.rif9_end_count[rif_cntr_id].count - self.rif9_start_count[rif_cntr_id].count == 1):
                self.final_egress_rif = self.egress_rif_port9
            else:
                self.final_egress_rif = 0

            iter_count = 10
            self.rif_lb_start_count = self.client.object_counters_get(self.final_egress_rif)
            print("Start RIF count %d"%(self.rif_lb_start_count[rif_cntr_id].count))

            for i in range(0, iter_count):
                l4_sport = random.randint(5000, 65535)
                l4_dport = random.randint(5000, 65535)
                mpls_send_pkt['TCP'].sport = l4_sport
                mpls_send_pkt['TCP'].dport = l4_dport
                send_packet(self, self.devports[0], mpls_send_pkt)

            time.sleep(2)
            self.rif_lb_end_count = self.client.object_counters_get(self.final_egress_rif)
            diff = self.rif_lb_end_count[rif_cntr_id].count - self.rif_lb_start_count[rif_cntr_id].count
            self.assertTrue(self.rif_lb_end_count[rif_cntr_id].count - self.rif_lb_start_count[rif_cntr_id].count == iter_count)

        finally:
            pass

    def _MplsCounterTest(self, mpls_id, pkt_no=1):
        counters = self.client.object_counters_get(mpls_id)
        for cntr in counters:
            if cntr.counter_id == SWITCH_MPLS_COUNTER_ID_PKTS:
                self.assertEqual(cntr.count, pkt_no, "Improper packet counter value")
            elif cntr.counter_id == SWITCH_MPLS_COUNTER_ID_BYTES:
                self.assertNotEqual(cntr.count, 0, "Bytes counter value should be non-zero")
            else:
                self.fail("Unknown MPLS packet counter ID")

        self.client.object_counters_clear(mpls_id, [SWITCH_MPLS_COUNTER_ID_BYTES])
        counters = self.client.object_counters_get(mpls_id)
        for cntr in counters:
            if cntr.counter_id == SWITCH_MPLS_COUNTER_ID_PKTS:
                self.assertEqual(pkt_no, cntr.count, "Improper packet counter value")
            elif cntr.counter_id == SWITCH_MPLS_COUNTER_ID_BYTES:
                self.assertEqual(cntr.count, 0, "Bytes counter value should be zero")
            else:
                self.fail("Unknown sidlist packet counter ID")

        self.client.object_counters_clear_all(mpls_id)
        counters = self.client.object_counters_get(mpls_id)
        for cntr in counters:
            self.assertEqual(cntr.count, 0, "sidlist counters not cleared")


    def tearDown(self):
        self.cleanup()
