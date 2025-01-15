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

import binascii

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
from ptf.packet import *
from ptf.thriftutils import *

import os
import copy
import ptf.mask as mask

from bf_switcht_api_thrift.ttypes import *
from bf_switcht_api_thrift.model_headers import *
from bf_pktpy.packets import BFD

from bfruntime_client_base_tests import BfRuntimeTest
import bfrt_grpc.bfruntime_pb2 as bfruntime_pb2
import bfrt_grpc.client as client

from p4testutils.misc_utils import mask_set_do_not_care_packet
from ptf.mask import *
import ptf.mask

this_dir = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(this_dir, '..'))
from common.utils import *
import json
import model_utils as u

acl_table_bp_port = switcht_list_val_t(type=switcht_value_type.ENUM, enumdata=SWITCH_ACL_TABLE_ATTR_BIND_POINT_TYPE_PORT)
acl_table_bp_rif = switcht_list_val_t(type=switcht_value_type.ENUM, enumdata=SWITCH_ACL_TABLE_ATTR_BIND_POINT_TYPE_RIF)

###############################################################################
@group('l3')
class L3TableFillTest(ApiHelper):
    def setUp(self):
        self.configure()

        # fill all fib tables upto 80% capacity
        info = self.client.table_info_get(SWITCH_DEVICE_ATTR_TABLE_IPV4_HOST)
        self.client.fill_table(self.device, switcht_perf_table.HOST_ROUTE_IPV4, int(info.size * 0.8))

        info = self.client.table_info_get(SWITCH_DEVICE_ATTR_TABLE_IPV6_HOST)
        self.client.fill_table(self.device, switcht_perf_table.HOST_ROUTE_IPV6, int(info.size * 0.8))

        info = self.client.table_info_get(SWITCH_DEVICE_ATTR_TABLE_IPV4_LPM)
        self.client.fill_table(self.device, switcht_perf_table.LPM_ROUTE_IPV4, int(info.size * 0.8))

        info = self.client.table_info_get(SWITCH_DEVICE_ATTR_TABLE_IPV6_LPM64)
        self.client.fill_table(self.device, switcht_perf_table.LPM_ROUTE_IPV6, int(info.size * 0.8))

        rif0 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port0, vrf_handle=self.vrf10, src_mac=self.rmac)
        rif1 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port1, vrf_handle=self.vrf10, src_mac=self.rmac)
        nhop1 = self.add_nexthop(self.device, handle=rif1, dest_ip='30.30.30.1')
        self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=rif1, dest_ip='30.30.30.1')
        self.add_route(self.device, ip_prefix='192.168.0.0/16', vrf_handle=self.vrf10, nexthop_handle=nhop1)

    def runTest(self):
        pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            ip_dst='192.168.1.1',
            ip_ttl=64)
        exp_pkt = simple_tcp_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src='00:77:66:55:44:33',
            ip_dst='192.168.1.1',
            ip_ttl=63)
        send_packet(self, self.devports[0], pkt)
        verify_packet(self, exp_pkt, self.devports[1])
        pass
    def tearDown(self):
        self.cleanlast()
        self.cleanlast()
        self.cleanlast()
        self.cleanlast()
        self.cleanlast()
        self.client.object_flush_all(SWITCH_OBJECT_TYPE_ROUTE)
        # fill_table creates 10 VRFs per fib type, they have to be cleaned up
        objects = self.client.object_get_all_handles(SWITCH_OBJECT_TYPE_VRF)
        for handle in objects:
          if handle == self.default_vrf:
            continue
          self.client.object_delete(handle)
        self.cleanup()

###############################################################################
@group('l3')
class L3FibLabelTest(ApiHelper):
    def setUp(self):
        print()

        self.configure()

        # ingress port
        self.rif0 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port0, vrf_handle=self.vrf10, src_mac=self.rmac)

        # nhop1
        rif1 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port1, vrf_handle=self.vrf10, src_mac=self.rmac)
        nhop4 = self.add_nexthop(self.device, handle=rif1, dest_ip='9.9.9.1')
        neighbor4 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:54', handle=rif1, dest_ip='9.9.9.1')

        # nhop2
        rif2 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port2, vrf_handle=self.vrf10, src_mac=self.rmac)
        nhop1 = self.add_nexthop(self.device, handle=rif2, dest_ip='10.10.10.1')
        neighbor1 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=rif2, dest_ip='10.10.10.1')

        # nhop3
        rif3 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port3, vrf_handle=self.vrf10, src_mac=self.rmac)
        nhop2 = self.add_nexthop(self.device, handle=rif3, dest_ip='20.20.20.1')
        neighbor2 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:56', handle=rif3, dest_ip='20.20.20.1')

        # nhop4
        rif4 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port4, vrf_handle=self.vrf10, src_mac=self.rmac)
        nhop3 = self.add_nexthop(self.device, handle=rif4, dest_ip='30.30.30.1')
        neighbor3 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:57', handle=rif4, dest_ip='30.30.30.1')

        self.route1 = self.add_route(self.device, ip_prefix='9.9.9.1', vrf_handle=self.vrf10, nexthop_handle=nhop4)
        self.route2 = self.add_route(self.device, ip_prefix='10.10.10.1', vrf_handle=self.vrf10, nexthop_handle=nhop1)
        self.route3 = self.add_route(self.device, ip_prefix='20.20.20.1', vrf_handle=self.vrf10, nexthop_handle=nhop2)
        self.route4 = self.add_route(self.device, ip_prefix='30.30.30.1', vrf_handle=self.vrf10, nexthop_handle=nhop3)


        table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IPV4
        if self.client.is_feature_enable(SWITCH_FEATURE_SHARED_INGRESS_IP_ACL):
            table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IP

        self.acl_table = self.add_acl_table(self.device,
            type=table_type,
            bind_point_type=[acl_table_bp_port],
            #bind_point_type=[],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        self.acl_entry = self.add_acl_entry(self.device,
            fib_label=10,
            fib_label_mask=0xff,
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            table_handle=self.acl_table)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, self.acl_table)

    def runTest(self):
        try:
            if (self.client.is_feature_enable(SWITCH_FEATURE_FIB_ACL_LABEL) == 0):
                print("Fib ACL Label feature not enabled, skipping")
                return

            self.Test1()
        finally:
            pass

    def tearDown(self):
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
        self.cleanup()

    def Test1(self):
        try:
            pkt1 = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                ip_dst='9.9.9.1',
                ip_ttl=64)
            exp_pkt1 = simple_tcp_packet(
                eth_dst='00:11:22:33:44:54',
                eth_src='00:77:66:55:44:33',
                ip_dst='9.9.9.1',
                ip_ttl=63)
            pkt2 = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                ip_dst='10.10.10.1',
                ip_ttl=64)
            exp_pkt2 = simple_tcp_packet(
                eth_dst='00:11:22:33:44:55',
                eth_src='00:77:66:55:44:33',
                ip_dst='10.10.10.1',
                ip_ttl=63)
            pkt3 = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                ip_dst='20.20.20.1',
                ip_ttl=64)
            exp_pkt3 = simple_tcp_packet(
                eth_dst='00:11:22:33:44:56',
                eth_src='00:77:66:55:44:33',
                ip_dst='20.20.20.1',
                ip_ttl=63)
            pkt4 = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                ip_dst='30.30.30.1',
                ip_ttl=64)
            exp_pkt4 = simple_tcp_packet(
                eth_dst='00:11:22:33:44:57',
                eth_src='00:77:66:55:44:33',
                ip_dst='30.30.30.1',
                ip_ttl=63)

            print("Without fib_label set")
            pre_counter = self.client.object_counters_get(self.acl_entry)
            send_packet(self, self.devports[0], pkt1)
            verify_packet(self, exp_pkt1, self.devports[1])
            send_packet(self, self.devports[0], pkt2)
            verify_packet(self, exp_pkt2, self.devports[2])
            send_packet(self, self.devports[0], pkt3)
            verify_packet(self, exp_pkt3, self.devports[3])
            send_packet(self, self.devports[0], pkt4)
            verify_packet(self, exp_pkt4, self.devports[4])
            time.sleep(3)
            post_counter = self.client.object_counters_get(self.acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 0)

            print("With fib_label set")
            self.attribute_set(self.route1, SWITCH_ROUTE_ATTR_FIB_LABEL, 10)
            self.attribute_set(self.route2, SWITCH_ROUTE_ATTR_FIB_LABEL, 10)
            self.attribute_set(self.route3, SWITCH_ROUTE_ATTR_FIB_LABEL, 10)
            self.attribute_set(self.route4, SWITCH_ROUTE_ATTR_FIB_LABEL, 10)
            pre_counter = self.client.object_counters_get(self.acl_entry)
            send_packet(self, self.devports[0], pkt1)
            verify_no_other_packets(self, timeout=1)
            send_packet(self, self.devports[0], pkt2)
            verify_no_other_packets(self, timeout=1)
            send_packet(self, self.devports[0], pkt3)
            verify_no_other_packets(self, timeout=1)
            send_packet(self, self.devports[0], pkt4)
            verify_no_other_packets(self, timeout=1)
            time.sleep(3)
            post_counter = self.object_counters_get(self.acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 4)

            print("With fib_label unset for 2 routes")
            self.attribute_set(self.route3, SWITCH_ROUTE_ATTR_FIB_LABEL, 0)
            self.attribute_set(self.route4, SWITCH_ROUTE_ATTR_FIB_LABEL, 0)
            pre_counter = self.client.object_counters_get(self.acl_entry)
            send_packet(self, self.devports[0], pkt1)
            verify_no_other_packets(self, timeout=1)
            send_packet(self, self.devports[0], pkt2)
            verify_no_other_packets(self, timeout=1)
            send_packet(self, self.devports[0], pkt3)
            verify_packet(self, exp_pkt3, self.devports[3])
            send_packet(self, self.devports[0], pkt4)
            verify_packet(self, exp_pkt4, self.devports[4])
            time.sleep(3)
            post_counter = self.object_counters_get(self.acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 2)
        finally:
            pass

###############################################################################
@group('l3')
class L3RIFMiscTest(ApiHelper):
    def setUp(self):
        print()

        self.configure()
        self.rif0 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port0, vrf_handle=self.vrf10, src_mac=self.rmac)

        self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port1)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.rif1 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_VLAN, vlan_handle=self.vlan10, vrf_handle=self.vrf10, src_mac=self.rmac)

        nhop1 = self.add_nexthop(self.device, handle=self.rif0, dest_ip='10.10.0.1')
        neighbor1 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=self.rif0, dest_ip='10.10.0.1')
        route1 = self.add_route(self.device, ip_prefix='10.10.10.1', vrf_handle=self.vrf10, nexthop_handle=nhop1)

        nhop2 = self.add_nexthop(self.device, handle=self.rif1, dest_ip='10.10.0.2')
        neighbor2 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:56', handle=self.rif1, dest_ip='10.10.0.2')
        route2 = self.add_route(self.device, ip_prefix='10.10.10.2', vrf_handle=self.vrf10, nexthop_handle=nhop2)
        mac = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:11:22:33:44:56', destination_handle=self.port1)
        self.pkt1 = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='10.10.10.2',
                ip_ttl=64)
        self.exp_pkt1 = simple_tcp_packet(
                eth_dst='00:11:22:33:44:56',
                eth_src='00:77:66:55:44:33',
                ip_dst='10.10.10.2',
                ip_ttl=63)
        self.pkt2 = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='10.10.10.1',
                ip_ttl=64)
        self.exp_pkt2 = simple_tcp_packet(
                eth_dst='00:11:22:33:44:55',
                eth_src='00:77:66:55:44:33',
                ip_dst='10.10.10.1',
                ip_ttl=63)

    def DuplicateRifTest(self):
        try:
            send_packet(self, self.devports[0], self.pkt1)
            verify_packet(self, self.exp_pkt1, self.devports[1])
            # try to create a duplicate RIF on port0, it will fail but packet forwarding should still function
            self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port0, vrf_handle=self.vrf10, src_mac=self.rmac)
            self.assertEqual(self.status(), SWITCH_STATUS_ITEM_ALREADY_EXISTS)
            send_packet(self, self.devports[0], self.pkt1)
            verify_packet(self, self.exp_pkt1, self.devports[1])

            send_packet(self, self.devports[1], self.pkt2)
            verify_packet(self, self.exp_pkt2, self.devports[0])
        finally:
            pass

    def InvalidAttrTest(self):
        try:
            send_packet(self, self.devports[0], self.pkt1)
            verify_packet(self, self.exp_pkt1, self.devports[1])
            send_packet(self, self.devports[1], self.pkt2)
            verify_packet(self, self.exp_pkt2, self.devports[0])
            self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, vrf_handle=self.vrf10, src_mac=self.rmac)
            self.assertEqual(self.status(), SWITCH_STATUS_INVALID_PARAMETER)
            self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_SUB_PORT, vrf_handle=self.vrf10, src_mac=self.rmac)
            self.assertEqual(self.status(), SWITCH_STATUS_INVALID_PARAMETER)
            self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_SUB_PORT, port_handle=self.port1, vrf_handle=self.vrf10, src_mac=self.rmac)
            self.assertEqual(self.status(), SWITCH_STATUS_INVALID_PARAMETER)
            send_packet(self, self.devports[0], self.pkt1)
            verify_packet(self, self.exp_pkt1, self.devports[1])
            send_packet(self, self.devports[1], self.pkt2)
            verify_packet(self, self.exp_pkt2, self.devports[0])
        finally:
            pass

    def runTest(self):
        try:
            self.DuplicateRifTest()
            self.InvalidAttrTest()
        finally:
            pass

    def tearDown(self):
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.cleanup()

###############################################################################
@group('l3')
@group('ecmp')
class L3NhopResolutionTest(ApiHelper):
    def runTest(self):
        print()

        self.configure()
        self.rif0 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port0, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.rif1 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port1, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.subport1_100 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_SUB_PORT, port_handle=self.port1, outer_vlan_id=100, vrf_handle=self.vrf10, src_mac=self.rmac)

        vlan_member1 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port2)
        vlan_member2 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port3)
        self.attribute_set(self.port2, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.attribute_set(self.port3, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.rif2 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_VLAN, vlan_handle=self.vlan10, vrf_handle=self.vrf10, src_mac=self.rmac)

        self.ecmp = self.add_ecmp(self.device)

        # send the test packet(s)
        self.pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        self.exp_pkt = simple_tcp_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src='00:77:66:55:44:33',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63)

        try:
            self.L3InterfaceResolutionTest()
            self.L3SubPortResolutionTest()
            self.SVIResolutionTest()
            self.SVIResolutionActionTest()
            self.L3EcmpResolutionTest()
            self.SVIEcmpResolutionTest()
        finally:
            self.attribute_set(self.port2, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.port3, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.cleanup()

    def L3InterfaceResolutionTest(self):
        print("For L3 interface")
        print("Test nhop resolution with various order of nhop, neighbor, route updates")
        print("Nhop -> Neighbor -> Route")
        nhop1 = self.add_nexthop(self.device, handle=self.rif1, dest_ip='10.10.0.2')
        neighbor1 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=self.rif1, dest_ip='10.10.0.2')
        route0 = self.add_route(self.device, ip_prefix='10.10.10.1', vrf_handle=self.vrf10, nexthop_handle=nhop1)

        try:
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                1], " (192.168.0.1 -> 10.10.10.1 [id = 105])")
            send_packet(self, self.devports[0], self.pkt)
            verify_packet(self, self.exp_pkt, self.devports[1])

            print("Remove and re-create route with same nexthop")
            self.cleanlast()
            route0 = self.add_route(self.device, ip_prefix='10.10.10.1', vrf_handle=self.vrf10, nexthop_handle=nhop1)
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                1], " (192.168.0.1 -> 10.10.10.1 [id = 105])")
            send_packet(self, self.devports[0], self.pkt)
            verify_packet(self, self.exp_pkt, self.devports[1])

            print("Create nhop2 on same RIF and update route to nhop2")
            nhop2 = self.add_nexthop(self.device, handle=self.rif1, dest_ip='20.10.0.2')
            neighbor2 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:66', handle=self.rif1, dest_ip='20.10.0.2')
            self.attribute_set(route0, SWITCH_ROUTE_ATTR_NEXTHOP_HANDLE, nhop2)
            new_exp_pkt = simple_tcp_packet(
                eth_dst='00:11:22:33:44:66',
              eth_src='00:77:66:55:44:33',
              ip_dst='10.10.10.1',
              ip_src='192.168.0.1',
              ip_id=105,
              ip_ttl=63)
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                1], " (192.168.0.1 -> 10.10.10.1 [id = 105])")
            send_packet(self, self.devports[0], self.pkt)
            verify_packet(self, new_exp_pkt, self.devports[1])
        finally:
            # Remove route, neighbor, nhop
            self.attribute_set(route0, SWITCH_ROUTE_ATTR_NEXTHOP_HANDLE, nhop1)
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

        print("Neighbor -> Nhop -> Route")
        neighbor1 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=self.rif1, dest_ip='10.10.0.2')
        nhop1 = self.add_nexthop(self.device, handle=self.rif1, dest_ip='10.10.0.2')
        route0 = self.add_route(self.device, ip_prefix='10.10.10.1', vrf_handle=self.vrf10, nexthop_handle=nhop1)

        try:
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                1], " (192.168.0.1 -> 10.10.10.1 [id = 105])")
            send_packet(self, self.devports[0], self.pkt)
            verify_packet(self, self.exp_pkt, self.devports[1])
        finally:
            # Remove route, nhop, neighbor
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

        print("Nhop -> Route -> Neighbor")
        nhop1 = self.add_nexthop(self.device, handle=self.rif1, dest_ip='10.10.0.2')
        route0 = self.add_route(self.device, ip_prefix='10.10.10.1', vrf_handle=self.vrf10, nexthop_handle=nhop1)
        neighbor1 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=self.rif1, dest_ip='10.10.0.2')

        try:
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                1], " (192.168.0.1 -> 10.10.10.1 [id = 105])")
            send_packet(self, self.devports[0], self.pkt)
            verify_packet(self, self.exp_pkt, self.devports[1])
        finally:
            # Remove route, nhop, neighbor
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

        print("Set rif as route nexthop_handle")
        nhop1 = self.add_nexthop(self.device, handle=self.rif1, dest_ip='10.10.10.1')
        neighbor1 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=self.rif1, dest_ip='10.10.10.1')
        route0 = self.add_route(self.device, ip_prefix='10.10.10.1', vrf_handle=self.vrf10, nexthop_handle=self.rif1)

        try:
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                1], " (192.168.0.1 -> 10.10.10.1 [id = 105])")
            send_packet(self, self.devports[0], self.pkt)
            verify_packet(self, self.exp_pkt, self.devports[1])

        finally:
            # Remove route, nhop, neighbor
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

    def L3SubPortResolutionTest(self):
        print("For L3 subport")
        print("Test nhop resolution with various order of nhop, neighbor, route updates")
        print("Nhop -> Neighbor -> Route")
        nhop1 = self.add_nexthop(self.device, handle=self.subport1_100, dest_ip='10.10.0.2')
        neighbor1 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=self.subport1_100, dest_ip='10.10.0.2')
        route0 = self.add_route(self.device, ip_prefix='10.10.10.1', vrf_handle=self.vrf10, nexthop_handle=nhop1)
        sp_pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.10.10.1',
            ip_ttl=64,
            pktlen=100)
        sp_exp_pkt = simple_tcp_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src='00:77:66:55:44:33',
            ip_dst='10.10.10.1',
            dl_vlan_enable=True,
            vlan_vid=100,
            ip_ttl=63,
            pktlen=104)

        try:
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                1], " (192.168.0.1 -> 10.10.10.1 [id = 105])")
            send_packet(self, self.devports[0], sp_pkt)
            verify_packet(self, sp_exp_pkt, self.devports[1])

            print("Remove and re-create route with same nexthop")
            self.cleanlast()
            route0 = self.add_route(self.device, ip_prefix='10.10.10.1', vrf_handle=self.vrf10, nexthop_handle=nhop1)
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                1], " (192.168.0.1 -> 10.10.10.1 [id = 105])")
            send_packet(self, self.devports[0], sp_pkt)
            verify_packet(self, sp_exp_pkt, self.devports[1])

            print("Create nhop2 on same RIF and update route to nhop2")
            nhop2 = self.add_nexthop(self.device, handle=self.subport1_100, dest_ip='20.10.0.2')
            neighbor2 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:66', handle=self.subport1_100, dest_ip='20.10.0.2')
            self.attribute_set(route0, SWITCH_ROUTE_ATTR_NEXTHOP_HANDLE, nhop2)
            new_exp_pkt = simple_tcp_packet(
                eth_dst='00:11:22:33:44:66',
              eth_src='00:77:66:55:44:33',
              ip_dst='10.10.10.1',
              dl_vlan_enable=True,
              vlan_vid=100,
              ip_ttl=63,
              pktlen=104)
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                1], " (192.168.0.1 -> 10.10.10.1 [id = 105])")
            send_packet(self, self.devports[0], sp_pkt)
            verify_packet(self, new_exp_pkt, self.devports[1])
        finally:
            # Remove route, neighbor, nhop
            self.attribute_set(route0, SWITCH_ROUTE_ATTR_NEXTHOP_HANDLE, nhop1)
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

        print("Neighbor -> Nhop -> Route")
        neighbor1 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=self.subport1_100, dest_ip='10.10.0.2')
        nhop1 = self.add_nexthop(self.device, handle=self.subport1_100, dest_ip='10.10.0.2')
        route0 = self.add_route(self.device, ip_prefix='10.10.10.1', vrf_handle=self.vrf10, nexthop_handle=nhop1)

        try:
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                1], " (192.168.0.1 -> 10.10.10.1 [id = 105])")
            send_packet(self, self.devports[0], sp_pkt)
            verify_packet(self, sp_exp_pkt, self.devports[1])
        finally:
            # Remove route, nhop, neighbor
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

        print("Nhop -> Route -> Neighbor")
        nhop1 = self.add_nexthop(self.device, handle=self.subport1_100, dest_ip='10.10.0.2')
        route0 = self.add_route(self.device, ip_prefix='10.10.10.1', vrf_handle=self.vrf10, nexthop_handle=nhop1)
        neighbor1 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=self.subport1_100, dest_ip='10.10.0.2')

        try:
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                1], " (192.168.0.1 -> 10.10.10.1 [id = 105])")
            send_packet(self, self.devports[0], sp_pkt)
            verify_packet(self, sp_exp_pkt, self.devports[1])
        finally:
            # Remove route, nhop, neighbor
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

    def SVIResolutionTest(self):
        print("For SVI interface")
        print("Test nhop resolution with various order of nhop, neighbor, route, mac updates")
        print("Nhop -> Neighbor -> Route -> MAC")
        nhop2 = self.add_nexthop(self.device, handle=self.rif2, dest_ip='10.10.0.2')
        neighbor2 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=self.rif2, dest_ip='10.10.0.2')
        route0 = self.add_route(self.device, ip_prefix='10.10.10.1', vrf_handle=self.vrf10, nexthop_handle=nhop2)
        mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:11:22:33:44:55', destination_handle=self.port2)

        try:
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 10.10.10.1 [id = 105])")
            send_packet(self, self.devports[0], self.pkt)
            verify_packets(self, self.exp_pkt, [self.devports[2]])

            print("Remove MAC, packet is gleaned")
            self.cleanlast()
            print("Sending packet port %d" % self.devports[0], " (192.168.0.1 -> 10.10.10.1 [id = 105])")
            cpu_pkt = simple_cpu_packet(
                packet_type=0,
                ingress_port=swport_to_devport(self, self.devports[0]),
                ingress_ifindex=(self.port0 & 0xFFFF),
                reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_MAX + SWITCH_HOSTIF_TRAP_ATTR_INTERNAL_TRAP_GLEAN,
                ingress_bd=0x0,
                inner_pkt=self.pkt)
            cpu_pkt = cpu_packet_mask_ingress_bd(cpu_pkt)
            send_packet(self, self.devports[0], self.pkt)
            verify_packets(self, cpu_pkt, [self.cpu_port])

            mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:11:22:33:44:55', destination_handle=self.port2)
            print("Re-add MAC and packet should not be gleaned anymore")
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 10.10.10.1 [id = 105])")
            send_packet(self, self.devports[0], self.pkt)
            verify_packets(self, self.exp_pkt, [self.devports[2]])

            print("Update MAC port_lag_handle to port %d from port %d", (self.devports[3], self.devports[2]))
            self.attribute_set(mac0, SWITCH_MAC_ENTRY_ATTR_DESTINATION_HANDLE, self.port3)
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                3], " (192.168.0.1 -> 10.10.10.1 [id = 105])")
            send_packet(self, self.devports[0], self.pkt)
            verify_packets(self, self.exp_pkt, [self.devports[3]])
        finally:
            # Remove route, neighbor, nhop, mac
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

        print("MAC -> Nhop -> Neighbor -> Route")
        mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:11:22:33:44:55', destination_handle=self.port2)
        nhop2 = self.add_nexthop(self.device, handle=self.rif2, dest_ip='10.10.0.2')
        neighbor2 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=self.rif2, dest_ip='10.10.0.2')
        route0 = self.add_route(self.device, ip_prefix='10.10.10.1', vrf_handle=self.vrf10, nexthop_handle=nhop2)

        try:
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 10.10.10.1 [id = 105])")
            send_packet(self, self.devports[0], self.pkt)
            verify_packets(self, self.exp_pkt, [self.devports[2]])
        finally:
            # Remove route, neighbor, nhop, mac
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

        print("Nhop -> MAC -> Neighbor -> Route")
        nhop2 = self.add_nexthop(self.device, handle=self.rif2, dest_ip='10.10.0.2')
        mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:11:22:33:44:55', destination_handle=self.port2)
        neighbor2 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=self.rif2, dest_ip='10.10.0.2')
        route0 = self.add_route(self.device, ip_prefix='10.10.10.1', vrf_handle=self.vrf10, nexthop_handle=nhop2)

        try:
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 10.10.10.1 [id = 105])")
            send_packet(self, self.devports[0], self.pkt)
            verify_packets(self, self.exp_pkt, [self.devports[2]])
        finally:
            # Remove route, neighbor, nhop, mac
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

        print("Nhop -> Neighbor -> MAC -> Route")
        nhop2 = self.add_nexthop(self.device, handle=self.rif2, dest_ip='10.10.0.2')
        neighbor2 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=self.rif2, dest_ip='10.10.0.2')
        mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:11:22:33:44:55', destination_handle=self.port2)
        route0 = self.add_route(self.device, ip_prefix='10.10.10.1', vrf_handle=self.vrf10, nexthop_handle=nhop2)

        try:
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 10.10.10.1 [id = 105])")
            send_packet(self, self.devports[0], self.pkt)
            verify_packets(self, self.exp_pkt, [self.devports[2]])
        finally:
            # Remove route, neighbor, nhop, mac
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

    def SVIResolutionActionTest(self):
        print("For SVI interface")
        print("Test if the nexthop action specs are getting updated correctly")
        print("Nhop -> Neighbor -> Route")
        cpu_pkt = simple_cpu_packet(
                packet_type=0,
                ingress_port=swport_to_devport(self, self.devports[0]),
                ingress_ifindex=(self.port0 & 0xFFFF),
                reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_MAX + SWITCH_HOSTIF_TRAP_ATTR_INTERNAL_TRAP_GLEAN,
                ingress_bd=0x0,
                inner_pkt=self.pkt)
        cpu_pkt = cpu_packet_mask_ingress_bd(cpu_pkt)

        nhop2 = self.add_nexthop(self.device, handle=self.rif2, dest_ip='10.10.0.2')
        neighbor2 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=self.rif2, dest_ip='10.10.0.2')
        route0 = self.add_route(self.device, ip_prefix='10.10.10.1', vrf_handle=self.vrf10, nexthop_handle=nhop2)
        try:
            print("Sending packet port 0 (192.168.0.1 -> 10.10.10.1 [id = 105]), send to CPU")
            send_packet(self, self.devports[0], self.pkt)
            verify_packets(self, cpu_pkt, [self.cpu_port])
        finally:
            pass

        print("Now add MAC")
        mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:11:22:33:44:55', destination_handle=self.port2)

        try:
            print("Sending packet port 0 (192.168.0.1 -> 10.10.10.1 [id = 105]), route to port %d only" % self.devports[2])
            send_packet(self, self.devports[0], self.pkt)
            verify_packets(self, self.exp_pkt, [self.devports[2]])
        finally:
            pass

        print("Now remove MAC")
        self.cleanlast()

        try:
            print("Sending packet port 0 (192.168.0.1 -> 10.10.10.1 [id = 105]), send to CPU")
            send_packet(self, self.devports[0], self.pkt)
            verify_packets(self, cpu_pkt, [self.cpu_port])
        finally:
            pass

        print("Set nhop2 action to drop")
        self.attribute_set(nhop2, SWITCH_NEXTHOP_ATTR_TYPE, SWITCH_NEXTHOP_ATTR_TYPE_DROP)
        try:
            print("Sending packet port 0 (192.168.0.1 -> 10.10.10.1 [id = 105]), drop")
            send_packet(self, self.devports[0], self.pkt)
            verify_no_other_packets(self, timeout=2)
        finally:
            pass

        print("Update nhop2 action to IP forwarding")
        self.attribute_set(nhop2, SWITCH_NEXTHOP_ATTR_TYPE, SWITCH_NEXTHOP_ATTR_TYPE_IP)
        try:
            print("Sending packet port 0 (192.168.0.1 -> 10.10.10.1 [id = 105]), send to CPU")
            send_packet(self, self.devports[0], self.pkt)
            verify_packets(self, cpu_pkt, [self.cpu_port])
        finally:
            pass

        print("Now add MAC on different port")
        mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:11:22:33:44:55', destination_handle=self.port3)
        try:
            print("Sending packet port 0 (192.168.0.1 -> 10.10.10.1 [id = 105]), route to port %d only" % self.devports[3])
            send_packet(self, self.devports[0], self.pkt)
            verify_packets(self, self.exp_pkt, [self.devports[3]])
        finally:
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

    def L3EcmpResolutionTest(self):
        print("For ECMP nhop resolution")
        print("Test ecmp nhop resolution with various order of nhop, neighbor, ecmp_member")

        print("2 ECMP member, same RIF negative test")
        route0 = self.add_route(self.device, ip_prefix='10.10.10.1', vrf_handle=self.vrf10, nexthop_handle=self.ecmp)
        nhop1 = self.add_nexthop(self.device, handle=self.rif1, dest_ip='10.10.0.2')
        neighbor1 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=self.rif1, dest_ip='10.10.0.2')
        ecmp_member1 = self.add_ecmp_member(self.device, ecmp_handle=self.ecmp, nexthop_handle=nhop1)
        nhop2 = self.add_nexthop(self.device, handle=self.rif1, dest_ip='20.10.0.2')
        neighbor2 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:66', handle=self.rif1, dest_ip='20.10.0.2')
        ecmp_member2 = self.add_ecmp_member(self.device, ecmp_handle=self.ecmp, nexthop_handle=nhop2)
        new_exp_pkt = simple_tcp_packet(
            eth_dst='00:11:22:33:44:66',
          eth_src='00:77:66:55:44:33',
          ip_dst='10.10.10.1',
          ip_src='192.168.0.1',
          ip_id=105,
          ip_ttl=63)

        try:
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                1], " (192.168.0.1 -> 10.10.10.1 [id = 105])")
            send_packet(self, self.devports[0], self.pkt)
            verify_any_packet_any_port(self, [self.exp_pkt, new_exp_pkt], [
                        self.devports[1], self.devports[1]])

            print("Remove ecmp member 2")
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                1], " (192.168.0.1 -> 10.10.10.1 [id = 105])")
            send_packet(self, self.devports[0], self.pkt)
            verify_packet(self, self.exp_pkt, self.devports[1])
        finally:
            # Remove ecmp_member, neighbor, nhop
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

        print("Nhop -> Neighbor -> ecmp_member")
        nhop1 = self.add_nexthop(self.device, handle=self.rif1, dest_ip='10.10.0.2')
        neighbor1 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=self.rif1, dest_ip='10.10.0.2')
        ecmp_member1 = self.add_ecmp_member(self.device, ecmp_handle=self.ecmp, nexthop_handle=nhop1)
        route0 = self.add_route(self.device, ip_prefix='10.10.10.1', vrf_handle=self.vrf10, nexthop_handle=self.ecmp)

        try:
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                1], " (192.168.0.1 -> 10.10.10.1 [id = 105])")
            send_packet(self, self.devports[0], self.pkt)
            verify_packet(self, self.exp_pkt, self.devports[1])
        finally:
            # Remove ecmp_member, neighbor, nhop
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

        print("Nhop -> ecmp_member -> Neighbor")
        nhop1 = self.add_nexthop(self.device, handle=self.rif1, dest_ip='10.10.0.2')
        ecmp_member1 = self.add_ecmp_member(self.device, ecmp_handle=self.ecmp, nexthop_handle=nhop1)
        neighbor1 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=self.rif1, dest_ip='10.10.0.2')
        route0 = self.add_route(self.device, ip_prefix='10.10.10.1', vrf_handle=self.vrf10, nexthop_handle=self.ecmp)

        try:
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                1], " (192.168.0.1 -> 10.10.10.1 [id = 105])")
            send_packet(self, self.devports[0], self.pkt)
            verify_packet(self, self.exp_pkt, self.devports[1])
        finally:
            # Remove ecmp_member, neighbor, nhop
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

        print("Neighbor -> Nhop -> ecmp_member")
        neighbor1 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=self.rif1, dest_ip='10.10.0.2')
        nhop1 = self.add_nexthop(self.device, handle=self.rif1, dest_ip='10.10.0.2')
        ecmp_member1 = self.add_ecmp_member(self.device, ecmp_handle=self.ecmp, nexthop_handle=nhop1)
        route0 = self.add_route(self.device, ip_prefix='10.10.10.1', vrf_handle=self.vrf10, nexthop_handle=self.ecmp)

        try:
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                1], " (192.168.0.1 -> 10.10.10.1 [id = 105])")
            send_packet(self, self.devports[0], self.pkt)
            verify_packet(self, self.exp_pkt, self.devports[1])
        finally:
            # Remove ecmp_member, neighbor, nhop
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

    def SVIEcmpResolutionTest(self):
        print("For ECMP with SVI interface")
        print("Test nhop resolution with various order of nhop, neighbor, route, mac updates")

        print("Nhop -> Neighbor -> MAC -> ecmp_member")
        nhop2 = self.add_nexthop(self.device, handle=self.rif2, dest_ip='10.10.0.2')
        neighbor2 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=self.rif2, dest_ip='10.10.0.2')
        mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:11:22:33:44:55', destination_handle=self.port2)
        ecmp_member2 = self.add_ecmp_member(self.device, ecmp_handle=self.ecmp, nexthop_handle=nhop2)
        route0 = self.add_route(self.device, ip_prefix='10.10.10.1', vrf_handle=self.vrf10, nexthop_handle=self.ecmp)

        try:
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 10.10.10.1 [id = 105])")
            send_packet(self, self.devports[0], self.pkt)
            verify_packets(self, self.exp_pkt, [self.devports[2]])
        finally:
            # Remove route, neighbor, nhop, mac, ecmp_member
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

        print("Nhop -> Neighbor -> ecmp_member -> MAC")
        route0 = self.add_route(self.device, ip_prefix='10.10.10.1', vrf_handle=self.vrf10, nexthop_handle=self.ecmp)
        nhop2 = self.add_nexthop(self.device, handle=self.rif2, dest_ip='10.10.0.2')
        neighbor2 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=self.rif2, dest_ip='10.10.0.2')
        ecmp_member2 = self.add_ecmp_member(self.device, ecmp_handle=self.ecmp, nexthop_handle=nhop2)

        try:
            # this should actually flood since neighbor is known. Need driver support to switch actions
            print("ARP not yet received, drop packet")
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 10.10.10.1 [id = 105])")
            send_packet(self, self.devports[0], self.pkt)
            verify_no_other_packets(self, timeout=2)

            print("ARP is now resolved")
            mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:11:22:33:44:55', destination_handle=self.port2)

            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 10.10.10.1 [id = 105])")
            send_packet(self, self.devports[0], self.pkt)
            verify_packets(self, self.exp_pkt, [self.devports[2]])

            print("Update MAC port_lag_handle to port %d from port %d" % (self.devports[3], self.devports[2]))
            self.attribute_set(mac0, SWITCH_MAC_ENTRY_ATTR_DESTINATION_HANDLE, self.port3)
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                3], " (192.168.0.1 -> 10.10.10.1 [id = 105])")
            send_packet(self, self.devports[0], self.pkt)
            verify_packets(self, self.exp_pkt, [self.devports[3]])
        finally:
            # Remove route, neighbor, nhop, mac, ecmp_member
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

        print("Nhop -> Neighbor -> ecmp_member -> MAC")
        route0 = self.add_route(self.device, ip_prefix='10.10.10.1', vrf_handle=self.vrf10, nexthop_handle=self.ecmp)
        nhop2 = self.add_nexthop(self.device, handle=self.rif2, dest_ip='10.10.0.2')
        neighbor2 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=self.rif2, dest_ip='10.10.0.2')
        mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:11:22:33:44:55', destination_handle=self.port2)
        ecmp_member1 = self.add_ecmp_member(self.device, ecmp_handle=self.ecmp, nexthop_handle=nhop2)
        ecmp_member2 = self.add_ecmp_member(self.device, ecmp_handle=self.ecmp, nexthop_handle=nhop2)

        try:
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 10.10.10.1 [id = 105])")
            send_packet(self, self.devports[0], self.pkt)
            verify_packets(self, self.exp_pkt, [self.devports[2]])

            self.cleanlast()
            print("Remove 1 ecmp member")
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 10.10.10.1 [id = 105])")
            send_packet(self, self.devports[0], self.pkt)
            verify_packets(self, self.exp_pkt, [self.devports[2]])

            print("Add back 1 ecmp member")
            ecmp_member2 = self.add_ecmp_member(self.device, ecmp_handle=self.ecmp, nexthop_handle=nhop2)
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 10.10.10.1 [id = 105])")
            send_packet(self, self.devports[0], self.pkt)
            verify_packets(self, self.exp_pkt, [self.devports[2]])

            print("Update MAC port_lag_handle to port %d from port %d" % (self.devports[3], self.devports[2]))
            self.attribute_set(mac0, SWITCH_MAC_ENTRY_ATTR_DESTINATION_HANDLE, self.port3)
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                3], " (192.168.0.1 -> 10.10.10.1 [id = 105])")
            send_packet(self, self.devports[0], self.pkt)
            verify_packets(self, self.exp_pkt, [self.devports[3]])

            self.cleanlast()
            self.cleanlast()
            print("Remove both ecmp members")
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 10.10.10.1 [id = 105])")
            send_packet(self, self.devports[0], self.pkt)
            verify_no_other_packets(self, timeout=2)
            #verify_packets(self, self.exp_pkt, [self.devports[2]])

            print("Add back 1 ecmp member")
            ecmp_member1 = self.add_ecmp_member(self.device, ecmp_handle=self.ecmp, nexthop_handle=nhop2)
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                3], " (192.168.0.1 -> 10.10.10.1 [id = 105])")
            send_packet(self, self.devports[0], self.pkt)
            verify_packets(self, self.exp_pkt, [self.devports[3]])

            print("Update MAC port_lag_handle to port %d from port %d" % (self.devports[2], self.devports[3]))
            self.attribute_set(mac0, SWITCH_MAC_ENTRY_ATTR_DESTINATION_HANDLE, self.port2)
            print("Add back 2nd ecmp member")
            ecmp_member2 = self.add_ecmp_member(self.device, ecmp_handle=self.ecmp, nexthop_handle=nhop2)
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 10.10.10.1 [id = 105])")
            send_packet(self, self.devports[0], self.pkt)
            verify_packets(self, self.exp_pkt, [self.devports[2]])
        finally:
            # Remove route, neighbor, nhop, mac, ecmp_member
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

        print("Nhop -> ecmp_member -> Neighbor -> MAC")
        nhop2 = self.add_nexthop(self.device, handle=self.rif2, dest_ip='10.10.0.2')
        ecmp_member2 = self.add_ecmp_member(self.device, ecmp_handle=self.ecmp, nexthop_handle=nhop2)
        neighbor2 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=self.rif2, dest_ip='10.10.0.2')
        mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:11:22:33:44:55', destination_handle=self.port2)
        route0 = self.add_route(self.device, ip_prefix='10.10.10.1', vrf_handle=self.vrf10, nexthop_handle=self.ecmp)

        try:
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 10.10.10.1 [id = 105])")
            send_packet(self, self.devports[0], self.pkt)
            verify_packets(self, self.exp_pkt, [self.devports[2]])
        finally:
            # Remove route, neighbor, nhop, mac, ecmp_member
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

        print("Neighbor -> MAC -> Nhop -> ecmp_member")
        neighbor2 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=self.rif2, dest_ip='10.10.0.2')
        mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:11:22:33:44:55', destination_handle=self.port2)
        nhop2 = self.add_nexthop(self.device, handle=self.rif2, dest_ip='10.10.0.2')
        ecmp_member2 = self.add_ecmp_member(self.device, ecmp_handle=self.ecmp, nexthop_handle=nhop2)
        route0 = self.add_route(self.device, ip_prefix='10.10.10.1', vrf_handle=self.vrf10, nexthop_handle=self.ecmp)

        try:
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 10.10.10.1 [id = 105])")
            send_packet(self, self.devports[0], self.pkt)
            verify_packets(self, self.exp_pkt, [self.devports[2]])
        finally:
            # Remove route, neighbor, nhop, mac, ecmp_member
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

        print("Neighbor -> Nhop -> ecmp_member -> MAC")
        neighbor2 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=self.rif2, dest_ip='10.10.0.2')
        nhop2 = self.add_nexthop(self.device, handle=self.rif2, dest_ip='10.10.0.2')
        ecmp_member2 = self.add_ecmp_member(self.device, ecmp_handle=self.ecmp, nexthop_handle=nhop2)
        mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:11:22:33:44:55', destination_handle=self.port2)
        route0 = self.add_route(self.device, ip_prefix='10.10.10.1', vrf_handle=self.vrf10, nexthop_handle=self.ecmp)

        try:
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 10.10.10.1 [id = 105])")
            send_packet(self, self.devports[0], self.pkt)
            verify_packets(self, self.exp_pkt, [self.devports[2]])
        finally:
            # Remove route, neighbor, nhop, mac, ecmp_member
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

        print("MAC -> Neighbor -> Nhop -> ecmp_member")
        mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:11:22:33:44:55', destination_handle=self.port2)
        neighbor2 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=self.rif2, dest_ip='10.10.0.2')
        nhop2 = self.add_nexthop(self.device, handle=self.rif2, dest_ip='10.10.0.2')
        ecmp_member2 = self.add_ecmp_member(self.device, ecmp_handle=self.ecmp, nexthop_handle=nhop2)
        route0 = self.add_route(self.device, ip_prefix='10.10.10.1', vrf_handle=self.vrf10, nexthop_handle=self.ecmp)

        try:
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 10.10.10.1 [id = 105])")
            send_packet(self, self.devports[0], self.pkt)
            verify_packets(self, self.exp_pkt, [self.devports[2]])
        finally:
            # Remove route, neighbor, nhop, mac, ecmp_member
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

        print("MAC -> Nhop -> Neighbor -> ecmp_member")
        mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:11:22:33:44:55', destination_handle=self.port2)
        nhop2 = self.add_nexthop(self.device, handle=self.rif2, dest_ip='10.10.0.2')
        neighbor2 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=self.rif2, dest_ip='10.10.0.2')
        ecmp_member2 = self.add_ecmp_member(self.device, ecmp_handle=self.ecmp, nexthop_handle=nhop2)
        route0 = self.add_route(self.device, ip_prefix='10.10.10.1', vrf_handle=self.vrf10, nexthop_handle=self.ecmp)

        try:
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 10.10.10.1 [id = 105])")
            send_packet(self, self.devports[0], self.pkt)
            verify_packets(self, self.exp_pkt, [self.devports[2]])
        finally:
            # Remove route, neighbor, nhop, mac, ecmp_member
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

###############################################################################

@group('l3')
class L3FibLpmTest(ApiHelper):
    def runTest(self):
        print()
        self.configure()

        try:
            self.ExactOverLpmTest()
            self.LpmSubnetTest()
        finally:
            self.cleanup()

    def ExactOverLpmTest(self):
        print("ExactOverLpmTest")
        self.ingress_rif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port0, vrf_handle=self.vrf10, src_mac=self.rmac)

        self.exact_rif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port1, vrf_handle=self.vrf10, src_mac=self.rmac)
        exact_nhop = self.add_nexthop(self.device, handle=self.exact_rif, dest_ip='10.10.0.1')
        self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=self.exact_rif, dest_ip='10.10.0.1')

        self.lpm_rif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port2, vrf_handle=self.vrf10, src_mac=self.rmac)
        lpm_nhop = self.add_nexthop(self.device, handle=self.lpm_rif, dest_ip='10.10.0.2')
        self.add_neighbor(self.device, mac_address='00:11:22:33:44:66', handle=self.lpm_rif, dest_ip='10.10.0.2')

        send_pkt = simple_tcp_packet(
            eth_dst=self.rmac,
            ip_dst='10.10.10.1',
            ip_ttl=64)
        exact_pkt = simple_tcp_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src=self.rmac,
            ip_dst='10.10.10.1',
            ip_ttl=63)
        lpm_pkt = simple_tcp_packet(
            eth_dst='00:11:22:33:44:66',
            eth_src=self.rmac,
            ip_dst='10.10.10.1',
            ip_ttl=63)
        send_v6_pkt = simple_tcpv6_packet(
            eth_dst='00:77:66:55:44:33',
            ipv6_dst='4000::1',
            ipv6_hlim=64)
        exact_v6_pkt = simple_tcpv6_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src='00:77:66:55:44:33',
            ipv6_dst='4000::1',
            ipv6_hlim=63)
        lpm_v6_pkt = simple_tcpv6_packet(
            eth_dst='00:11:22:33:44:66',
            eth_src='00:77:66:55:44:33',
            ipv6_dst='4000::1',
            ipv6_hlim=63)

        try:
            lpm_route = self.add_route(self.device, ip_prefix='10.10.10.0/24', vrf_handle=self.vrf10, nexthop_handle=lpm_nhop)
            lpm_route_v6 = self.add_route(self.device, ip_prefix='4000::0/96', vrf_handle=self.vrf10, nexthop_handle=lpm_nhop)
            print("Sending packet from %d to port %d, lpm route hit, v4" % (self.devports[0], self.devports[2]))
            send_packet(self, self.devports[0], send_pkt)
            verify_packet(self, lpm_pkt, self.devports[2])
            print("Sending packet from %d to port %d, lpm route hit, v6" % (self.devports[0], self.devports[2]))
            send_packet(self, self.devports[0], send_v6_pkt)
            verify_packet(self, lpm_v6_pkt, self.devports[2])

            exact_route = self.add_route(self.device, ip_prefix='10.10.10.1', vrf_handle=self.vrf10, nexthop_handle=exact_nhop)
            exact_route_v6 = self.add_route(self.device, ip_prefix='4000::1', vrf_handle=self.vrf10, nexthop_handle=exact_nhop)
            print("Add exact route, lpm will not be hit")
            print("Sending packet from %d to port %d, exact route hit, v4" % (self.devports[0], self.devports[1]))
            send_packet(self, self.devports[0], send_pkt)
            verify_packet(self, exact_pkt, self.devports[1])
            print("Sending packet from %d to port %d, exact route hit, v6" % (self.devports[0], self.devports[1]))
            send_packet(self, self.devports[0], send_v6_pkt)
            verify_packet(self, exact_v6_pkt, self.devports[1])

            self.cleanlast()
            self.cleanlast()
            print("Remove exact route, lpm will be hit")
            print("Sending packet from %d to port %d, lpm route hit, v4" % (self.devports[0], self.devports[2]))
            send_packet(self, self.devports[0], send_pkt)
            verify_packet(self, lpm_pkt, self.devports[2])
            print("Sending packet from %d to port %d, lpm route hit, v6" % (self.devports[0], self.devports[2]))
            send_packet(self, self.devports[0], send_v6_pkt)
            verify_packet(self, lpm_v6_pkt, self.devports[2])
        finally:
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

    def LpmSubnetTest(self):
        print("LpmSubnetTest")
        handle_list = []

        handle_list = [{} for i in range(0,8)]
        for i in range(0,8):
            handle_list[i]['vrf'] = 0
            handle_list[i]['neighbor'] = 0
            handle_list[i]['nexthop'] = 0
            handle_list[i]['rif'] = 0
            handle_list[i]['route'] = 0

        for i in range(0, 8):
            handle_list[i]['vrf'] = self.add_vrf(self.device, id=((i+1)*2), src_mac=self.rmac)
            handle_list[i]['rif'] = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port_list[i],
                vrf_handle=handle_list[i]['vrf'], src_mac=self.rmac)

        ip_addr = [{} for i in range(0,8)]
        ip_addr[0] = '172.17.0.0/16'
        ip_addr[1] = '172.17.64.0/18'
        ip_addr[2] = '172.17.16.0/20'
        ip_addr[3] = '172.17.4.0/24'
        ip_addr[4] = '172.17.0.64/26'
        ip_addr[5] = '172.17.0.32/28'
        ip_addr[6] = '172.17.0.4/30'
        for i in range(1, 8):
            handle_list[i]['nexthop']  = self.add_nexthop(self.device, handle=handle_list[i]['rif'], dest_ip='10.20.10.1')
            handle_list[i]['neighbor']  = self.add_neighbor(self.device, mac_address=self.rmac,
                handle=handle_list[i]['rif'], dest_ip='10.20.10.1')
            handle_list[i]['route'] = self.add_route(self.device, ip_prefix=ip_addr[i-1],
                vrf_handle=handle_list[0]['vrf'], nexthop_handle=handle_list[i]['nexthop'])

        ip_addr = [{} for i in range(0,8)]
        ip_addr[0] = '172.17.0.1'
        ip_addr[1] = '172.17.64.1'
        ip_addr[2] = '172.17.16.1'
        ip_addr[3] = '172.17.4.1'
        ip_addr[4] = '172.17.0.65'
        ip_addr[5] = '172.17.0.33'
        ip_addr[6] = '172.17.0.5'
        try:
            for i in range(1, 8):
                send_pkt = simple_tcp_packet(
                    eth_dst=self.rmac,
                  eth_src='00:22:22:22:22:22',
                  ip_dst=ip_addr[i-1],
                  ip_src='192.168.0.1',
                  ip_id=105,
                  ip_ttl=64)

                exp_pkt = simple_tcp_packet(
                    eth_dst=self.rmac,
                    eth_src=self.rmac,
                    ip_dst=ip_addr[i-1],
                    ip_src='192.168.0.1',
                    ip_id=105,
                    ip_ttl=63)
                print("Sending packet from %d to port %d" % (self.devports[0], self.devports[i]))
                send_packet(self, self.devports[0], send_pkt)
                verify_packet(self, exp_pkt, self.devports[i])
        finally:
            self.cleanup()

###############################################################################

@group('l3')
class L3SnakeTest(ApiHelper):
    def runTest(self):
        print()
        self.configure()
        handle_list = []

        SWITCH_MAX_PORTS = len(self.port_list)
        print("Max ports %d"%(SWITCH_MAX_PORTS))

        handle_list = [{} for i in range(SWITCH_MAX_PORTS)]
        for i in range(0, SWITCH_MAX_PORTS):
            handle_list[i]['vrf'] = 0
            handle_list[i]['neighbor'] = 0
            handle_list[i]['nexthop'] = 0
            handle_list[i]['rif'] = 0
            handle_list[i]['route'] = 0

        for i in range(0, SWITCH_MAX_PORTS):
            handle_list[i]['vrf'] = self.add_vrf(self.device, id=((i+1)*2), src_mac=self.rmac)
            handle_list[i]['rif'] = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port_list[i],
                vrf_handle=handle_list[i]['vrf'], src_mac=self.rmac)

        # Add a static route 172.17.10.1/24
        ip_addr = '172.17.10.1'
        for i in range(0, SWITCH_MAX_PORTS - 1):
            handle_list[i]['nexthop']  = self.add_nexthop(self.device, handle=handle_list[i+1]['rif'], dest_ip='10.20.10.1')
            handle_list[i]['neighbor']  = self.add_neighbor(self.device, mac_address=self.rmac,
                handle=handle_list[i+1]['rif'], dest_ip='10.20.10.1')
            handle_list[i]['route'] = self.add_route(self.device, ip_prefix='172.17.10.0/24',
                vrf_handle=handle_list[i]['vrf'], nexthop_handle=handle_list[i]['nexthop'])

        '''
      for i in range(0, SWITCH_MAX_PORTS - 1):
        print("Index %d"%(i))
        print("Port handle 0x%lx"%(handle_list[i]['port']))
        print("VRF handle 0x%lx"%(handle_list[i]['vrf']))
        print("RIF handle 0x%lx"%(handle_list[i]['rif']))
        print("Nexthop handle 0x%lx"%(handle_list[i]['nexthop']))
        print("Neighbor handle 0x%lx"%(handle_list[i]['neighbor']))
      '''

        try:
            for i in range(0, SWITCH_MAX_PORTS - 1):

                if i == 0:
                    send_pkt = simple_tcp_packet(
                        eth_dst=self.rmac,
                      eth_src='00:22:22:22:22:22',
                      ip_dst=ip_addr,
                      ip_src='192.168.0.1',
                      ip_id=105,
                      ip_ttl=64)

                    exp_pkt = simple_tcp_packet(
                        eth_dst=self.rmac,
                        eth_src=self.rmac,
                        ip_dst=ip_addr,
                        ip_src='192.168.0.1',
                        ip_id=105,
                        ip_ttl=63)
                else:
                    send_pkt_ttl = 64 - i
                    recv_pkt_ttl = send_pkt_ttl - 1
                    send_pkt = simple_tcp_packet(
                        eth_dst=self.rmac,
                      eth_src=self.rmac,
                      ip_dst=ip_addr,
                      ip_src='192.168.0.1',
                      ip_id=105,
                      ip_ttl=send_pkt_ttl)
                    exp_pkt = simple_tcp_packet(
                        eth_dst=self.rmac,
                      eth_src=self.rmac,
                      ip_dst=ip_addr,
                      ip_src='192.168.0.1',
                      ip_id=105,
                      ip_ttl=recv_pkt_ttl)

                print("Sending packet from %d to port %d" % (self.devports[i], self.devports[i+1]))
                send_packet(self, self.devports[i], send_pkt)
                verify_packet(self, exp_pkt, self.devports[i+1])
        finally:
            self.cleanup()

###############################################################################
@group('l3')
class L3RmacTest(ApiHelper):
    def setUp(self):
        print()

        self.configure()

        self.new_rmac = '00:77:66:55:44:44'
        # port 0 is egress interface
        self.rif0 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port0, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.nhop0 = self.add_nexthop(self.device, handle=self.rif0, dest_ip='10.10.0.2')
        self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=self.rif0, dest_ip='10.10.0.2')
        self.add_route(self.device, ip_prefix='10.10.10.1', vrf_handle=self.vrf10, nexthop_handle=self.nhop0)

        self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port2)
        self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port3)

        self.add_vlan_member(self.device, vlan_handle=self.vlan20, member_handle=self.port5)
        self.add_vlan_member(self.device, vlan_handle=self.vlan20, member_handle=self.port6)

        self.pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            ip_dst='10.10.10.1',
            ip_ttl=64)
        self.pkt1 = simple_tcp_packet(
            eth_dst='00:77:66:55:44:44',
            ip_dst='10.10.10.1',
            ip_ttl=64)
        self.exp_pkt = simple_tcp_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src='00:77:66:55:44:33',
            ip_dst='10.10.10.1',
            ip_ttl=63)

    def runTest(self):
        try:
            self.SwitchRmacUpdateTest()
            self.ResolutionTest1()
            self.ResolutionTest2()
            self.ResolutionTest3()
            self.ResolutionTest4()
            self.ResolutionTest5()
        finally:
            pass

    def SwitchRmacUpdateTest(self):
        print("SwitchRmacUpdateTest()")
        try:
          self.rif1 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port1, vrf_handle=self.vrf10, src_mac=self.rmac)

          send_packet(self, self.devports[1], self.pkt)
          verify_packet(self, self.exp_pkt, self.devports[0])

          # send packet with new RMAC, will be dropped
          send_packet(self, self.devports[1], self.pkt1)
          verify_no_other_packets(self, timeout=1)

          # update switch rmac to new RMAC
          self.attribute_set(self.device, SWITCH_DEVICE_ATTR_SRC_MAC, self.new_rmac)
          send_packet(self, self.devports[1], self.pkt1)
          verify_packet(self, self.exp_pkt, self.devports[0])

          # revert to old RMAC
          self.attribute_set(self.device, SWITCH_DEVICE_ATTR_SRC_MAC, self.rmac)
          send_packet(self, self.devports[1], self.pkt)
          verify_packet(self, self.exp_pkt, self.devports[0])
        finally:
          self.attribute_set(self.device, SWITCH_DEVICE_ATTR_SRC_MAC, self.default_rmac)
          self.cleanlast()

    def ResolutionTest1(self):
        print("ResolutionTest1()")
        try:
          # add vlan member -> set attribute -> add rif
          self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port1)
          self.attribute_set(self.port1, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
          self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_VLAN, vlan_handle=self.vlan10, vrf_handle=self.vrf10, src_mac=self.rmac)

          send_packet(self, self.devports[1], self.pkt)
          verify_packet(self, self.exp_pkt, self.devports[0])
        finally:
          self.cleanlast()
          self.attribute_set(self.port1, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
          self.cleanlast()

    def ResolutionTest2(self):
        print("ResolutionTest2()")
        try:
          # add rif -> add vlan member -> set attribute
          self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_VLAN, vlan_handle=self.vlan10, vrf_handle=self.vrf10, src_mac=self.rmac)
          self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port1)
          self.attribute_set(self.port1, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)

          send_packet(self, self.devports[1], self.pkt)
          verify_packet(self, self.exp_pkt, self.devports[0])
        finally:
          self.attribute_set(self.port1, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
          self.cleanlast()
          self.cleanlast()

    def ResolutionTest3(self):
        print("ResolutionTest3()")
        try:
          # add vlan member -> add rif -> set attribute
          self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port1)
          self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_VLAN, vlan_handle=self.vlan10, vrf_handle=self.vrf10, src_mac=self.rmac)
          self.attribute_set(self.port1, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)

          send_packet(self, self.devports[1], self.pkt)
          verify_packet(self, self.exp_pkt, self.devports[0])
        finally:
          self.attribute_set(self.port1, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
          self.cleanlast()
          self.cleanlast()

    def ResolutionTest4(self):
        print("ResolutionTest4()")
        try:
          self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port1)
          self.attribute_set(self.port1, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)

          # no SVI, packet flooded since rmac is unknown
          send_packet(self, self.devports[1], self.pkt)
          verify_packets(self, self.pkt, [self.devports[2], self.devports[3]])

          rif10 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_VLAN, vlan_handle=self.vlan10, vrf_handle=self.vrf10, src_mac=self.rmac)
          # SVI added, packet is routed
          send_packet(self, self.devports[1], self.pkt)
          verify_packet(self, self.exp_pkt, self.devports[0])

          # update SVI src mac
          self.attribute_set(rif10, SWITCH_RIF_ATTR_SRC_MAC, self.new_rmac)
          send_packet(self, self.devports[1], self.pkt1)
          verify_packet(self, self.exp_pkt, self.devports[0])

          # remove SVI, packet will be flooded
          self.cleanlast()
          send_packet(self, self.devports[1], self.pkt)
          verify_packets(self, self.pkt, [self.devports[2], self.devports[3]])
        finally:
          self.attribute_set(self.port1, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
          self.cleanlast()

    def ResolutionTest5(self):
        print("ResolutionTest5()")
        try:
          self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port4)
          self.add_vlan_member(self.device, vlan_handle=self.vlan20, member_handle=self.port4)

          # SVI added on vlan10, port4 is untagged on vlan 10
          self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_VLAN, vlan_handle=self.vlan10, vrf_handle=self.vrf10, src_mac=self.rmac)
          self.attribute_set(self.port4, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
          send_packet(self, self.devports[4], self.pkt)
          verify_packet(self, self.exp_pkt, self.devports[0])

          # SVI added on vlan20, port4 is untagged on vlan 20
          self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_VLAN, vlan_handle=self.vlan20, vrf_handle=self.vrf10, src_mac=self.rmac)
          self.attribute_set(self.port4, SWITCH_PORT_ATTR_PORT_VLAN_ID, 20)
          send_packet(self, self.devports[4], self.pkt)
          verify_packet(self, self.exp_pkt, self.devports[0])

          # delete SVI on vlan20, flood on vlan 20
          self.cleanlast()
          send_packet(self, self.devports[4], self.pkt)
          verify_packets(self, self.pkt, [self.devports[5], self.devports[6]])

          # go back to vlan 10
          self.attribute_set(self.port4, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
          send_packet(self, self.devports[4], self.pkt)
          verify_packet(self, self.exp_pkt, self.devports[0])

          # delete SVI on vlan10, flood on vlan 10
          self.cleanlast()
          send_packet(self, self.devports[4], self.pkt)
          verify_packets(self, self.pkt, [self.devports[2], self.devports[3]])
        finally:
          self.attribute_set(self.port4, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
          self.cleanlast()
          self.cleanlast()

    def tearDown(self):
        self.cleanup()


###############################################################################
@group('l3')
class L3MymacTest(ApiHelper):
    def setUp(self):
        print()

        self.configure()

        self.mymac = '00:77:66:55:55:55'
        self.mymac_mask = 'ff:ff:ff:ff:ff:ff'
        # port 0 is egress interface
        self.rif0 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port0, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.nhop0 = self.add_nexthop(self.device, handle=self.rif0, dest_ip='10.10.0.2')
        self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=self.rif0, dest_ip='10.10.0.2')
        self.add_route(self.device, ip_prefix='10.10.10.1', vrf_handle=self.vrf10, nexthop_handle=self.nhop0)


        self.pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            ip_dst='10.10.10.1',
            ip_ttl=64)
        self.pkt1 = simple_tcp_packet(
            eth_dst='00:77:66:55:55:55',
            ip_dst='10.10.10.1',
            ip_ttl=64)
        self.tagged_pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            ip_dst='10.10.10.1',
            ip_ttl=64,
            dl_vlan_enable=True,
            vlan_vid=10,
            pktlen=104)
        self.tagged_pkt1 = simple_tcp_packet(
            eth_dst='00:77:66:55:55:55',
            ip_dst='10.10.10.1',
            ip_ttl=64,
            dl_vlan_enable=True,
            vlan_vid=10,
            pktlen=104)
        self.exp_pkt = simple_tcp_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src='00:77:66:55:44:33',
            ip_dst='10.10.10.1',
            ip_ttl=63)

    def runTest(self):
        try:
            self.MymacUpdateTest()
            self.MymacAnyRifTest()
            self.MymacSVITest()
            self.MymacDropL2Test()
            self.MymacSubPortTest()
        finally:
            pass

    def MymacUpdateTest(self):
        print("MymacUpdateTest()")
        try:
          self.rif1 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port1, vrf_handle=self.vrf10, src_mac=self.rmac)

          print("send pkt with rmac, will be forwarded");
          send_packet(self, self.devports[1], self.pkt)
          verify_packet(self, self.exp_pkt, self.devports[0])

          # send packet with mymac, will be dropped
          print("send pkt with mymac, will be dropped");
          send_packet(self, self.devports[1], self.pkt1)
          verify_no_other_packets(self, timeout=1)

          # update mymac to interface
          print("update mymac");
          self.add_my_mac(self.device, port_lag_index=self.port1, mac_address=self.mymac,mac_address_mask=self.mymac_mask);

          # send pkts with mymac
          print("send pkts with mymac, will be forwarded")
          send_packet(self, self.devports[1], self.pkt1)
          verify_packet(self, self.exp_pkt, self.devports[0])

          # send pkts with rmac
          print("send pkt with rmac, will be forwarded");
          send_packet(self, self.devports[1], self.pkt)
          verify_packet(self, self.exp_pkt, self.devports[0])
        finally:
          self.cleanlast()
          self.cleanlast()

    def MymacAnyRifTest(self):
        print("MymacAnyRifTest()")
        try:
          self.rif1 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port1, vrf_handle=self.vrf10, src_mac=self.rmac)
          self.rif2 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port2, vrf_handle=self.vrf10, src_mac=self.rmac)
          self.rif3 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port3, vrf_handle=self.vrf10, src_mac=self.rmac)
          self.add_my_mac(self.device, mac_address=self.mymac,mac_address_mask=self.mymac_mask);

          print("send pkt with mymac on rif1, will be forwarded");
          send_packet(self, self.devports[1], self.pkt1)
          verify_packet(self, self.exp_pkt, self.devports[0])

          print("send pkt with mymac on rif2, will be forwarded");
          send_packet(self, self.devports[2], self.pkt1)
          verify_packet(self, self.exp_pkt, self.devports[0])

          print("send pkt with mymac on rif3, will be forwarded");
          send_packet(self, self.devports[3], self.pkt1)
          verify_packet(self, self.exp_pkt, self.devports[0])

        finally:
          self.cleanlast()
          self.cleanlast()
          self.cleanlast()
          self.cleanlast()

    def MymacSVITest(self):
        print("MymacSVITest()")
        try:
          # SVI config
          self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port1)
          self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_VLAN, vlan_handle=self.vlan10, vrf_handle=self.vrf10, src_mac=self.rmac)
          self.attribute_set(self.port1, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)

          self.add_my_mac(self.device, mac_address=self.mymac,mac_address_mask=self.mymac_mask);

          print("send untagged pkt with mymac on SVI")
          send_packet(self, self.devports[1], self.pkt1)
          verify_packet(self, self.exp_pkt, self.devports[0])

          print("send tagged pkt with mymac on SVI")
          send_packet(self, self.devports[1], self.tagged_pkt1)
          verify_packet(self, self.exp_pkt, self.devports[0])

        finally:
          self.attribute_set(self.port1, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
          self.cleanlast()
          self.cleanlast()
          self.cleanlast()

    def MymacDropL2Test(self):
        print("MymacDropL2Test()")
        try:
          self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port1)
          self.attribute_set(self.port1, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)

          self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port2)
          self.attribute_set(self.port2, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)

          self.add_my_mac(self.device, mac_address=self.mymac,mac_address_mask=self.mymac_mask);

          print("send pkt with mymac on L2, will be dropped")
          send_packet(self, self.devports[1], self.pkt1)
          verify_no_other_packets(self, timeout=1)

        finally:
          self.attribute_set(self.port1, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
          self.attribute_set(self.port2, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
          self.cleanlast()
          self.cleanlast()
          self.cleanlast()

    def MymacSubPortTest(self):
        print("MymacSubPortTest()")
        try:
          self.rif1 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_SUB_PORT, port_handle=self.port1, outer_vlan_id=10,
                         vrf_handle=self.vrf10, src_mac=self.rmac)

          print("send pkt with rmac, will be forwarded");
          send_packet(self, self.devports[1], self.tagged_pkt)
          verify_packet(self, self.exp_pkt, self.devports[0])

          # send packet with mymac, will be dropped
          print("send pkt with mymac, will be dropped");
          send_packet(self, self.devports[1], self.tagged_pkt1)
          verify_no_other_packets(self, timeout=1)

          # update mymac to interface
          print("update mymac");
          self.add_my_mac(self.device, port_lag_index=self.port1, mac_address=self.mymac,mac_address_mask=self.mymac_mask);

          # send pkts with mymac
          print("send pkts with mymac, will be forwarded")
          send_packet(self, self.devports[1], self.tagged_pkt1)
          verify_packet(self, self.exp_pkt, self.devports[0])

          # send pkts with rmac
          print("send pkt with rmac, will be forwarded");
          send_packet(self, self.devports[1], self.tagged_pkt)
          verify_packet(self, self.exp_pkt, self.devports[0])
        finally:
          self.cleanlast()
          self.cleanlast()

    def tearDown(self):
        self.cleanup()

###############################################################################

@group('l3')
class L3RouteTest(ApiHelper):
    def setUp(self):
        print()

        self.configure()
        self.nhop_drop = self.add_nexthop(self.device, type=SWITCH_NEXTHOP_ATTR_TYPE_DROP)
        self.rif0 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port0, vrf_handle=self.vrf10, src_mac=self.rmac)

        self.rif1 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port1, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.nhop1 = self.add_nexthop(self.device, handle=self.rif1, dest_ip='10.10.0.2')
        self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=self.rif1, dest_ip='10.10.0.2')

        self.route0 = self.add_route(self.device, ip_prefix='10.10.10.1', vrf_handle=self.vrf10, nexthop_handle=self.nhop1)
        self.add_hostif_trap(self.device, type=SWITCH_HOSTIF_TRAP_ATTR_TYPE_MYIP,
                             packet_action=SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_REDIRECT_TO_CPU)

    def runTest(self):
        self.routeActionsTest()

    def routeActionsTest(self):
        print("self.routeActionsTest")
        print("Modify route actions and verify behavior")

        pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            ip_dst='10.10.10.1',
            ip_ttl=64)
        exp_pkt = simple_tcp_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src='00:77:66:55:44:33',
            ip_dst='10.10.10.1',
            ip_ttl=63)
        cpu_pkt = simple_cpu_packet(
                packet_type=0,
                ingress_port=swport_to_devport(self, self.devports[0]),
                ingress_ifindex=(self.port0 & 0xFFFF),
                reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_MYIP,
                ingress_bd=0x0,
                inner_pkt=pkt)
        myip_cpu_pkt = cpu_packet_mask_ingress_bd(cpu_pkt)
        cpu_pkt = simple_cpu_packet(
                packet_type=0,
                ingress_port=swport_to_devport(self, self.devports[0]),
                ingress_ifindex=(self.port0 & 0xFFFF),
                reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_MAX + SWITCH_HOSTIF_TRAP_ATTR_INTERNAL_TRAP_GLEAN,
                ingress_bd=0x0,
                inner_pkt=pkt)
        trap_cpu_pkt = cpu_packet_mask_ingress_bd(cpu_pkt)
        try:
            self.attribute_set(self.route0, SWITCH_ROUTE_ATTR_NEXTHOP_HANDLE, self.nhop1)
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                1], " (192.168.0.1 -> 10.10.10.1)")
            send_packet(self, self.devports[0], pkt)
            verify_packets(self, exp_pkt, [self.devports[1]])

            print("Set route to myip")
            self.attribute_set(self.route0, SWITCH_ROUTE_ATTR_IS_HOST_MYIP, True)
            send_packet(self, self.devports[0], pkt)
            verify_packets(self, myip_cpu_pkt, [self.cpu_port])
            print("Set route back to port %d" % (self.devports[1]))
            self.attribute_set(self.route0, SWITCH_ROUTE_ATTR_IS_HOST_MYIP, False)
            send_packet(self, self.devports[0], pkt)
            verify_packets(self, exp_pkt, [self.devports[1]])

            print("Set route to drop via nexthop")
            self.attribute_set(self.route0, SWITCH_ROUTE_ATTR_NEXTHOP_HANDLE, self.nhop_drop)
            send_packet(self, self.devports[0], pkt)
            verify_no_other_packets(self, timeout=2)
            print("Set route back to port %d" % (self.devports[1]))
            self.attribute_set(self.route0, SWITCH_ROUTE_ATTR_NEXTHOP_HANDLE, self.nhop1)
            send_packet(self, self.devports[0], pkt)
            verify_packets(self, exp_pkt, [self.devports[1]])

            print("Set route to drop via packet action")
            self.attribute_set(self.route0, SWITCH_ROUTE_ATTR_PACKET_ACTION, SWITCH_ROUTE_ATTR_PACKET_ACTION_DROP)
            send_packet(self, self.devports[0], pkt)
            verify_no_other_packets(self, timeout=2)
            self.attribute_set(self.route0, SWITCH_ROUTE_ATTR_NEXTHOP_HANDLE, 0)
            send_packet(self, self.devports[0], pkt)
            verify_no_other_packets(self, timeout=2)
            print("Set route to back to forward via packet action")
            self.attribute_set(self.route0, SWITCH_ROUTE_ATTR_PACKET_ACTION, SWITCH_ROUTE_ATTR_PACKET_ACTION_FORWARD)
            self.attribute_set(self.route0, SWITCH_ROUTE_ATTR_NEXTHOP_HANDLE, self.nhop1)
            send_packet(self, self.devports[0], pkt)
            verify_packets(self, exp_pkt, [self.devports[1]])

            print("Set route to back to forward via packet action but set nhop to 0")
            self.attribute_set(self.route0, SWITCH_ROUTE_ATTR_PACKET_ACTION, SWITCH_ROUTE_ATTR_PACKET_ACTION_FORWARD)
            self.attribute_set(self.route0, SWITCH_ROUTE_ATTR_NEXTHOP_HANDLE, 0)
            send_packet(self, self.devports[0], pkt)
            verify_no_other_packets(self, timeout=2)
            print("Set route to back to forward via packet action but set valid nhop")
            self.attribute_set(self.route0, SWITCH_ROUTE_ATTR_NEXTHOP_HANDLE, self.nhop1)
            send_packet(self, self.devports[0], pkt)
            verify_packets(self, exp_pkt, [self.devports[1]])

            print("Set route to trap via packet action")
            self.attribute_set(self.route0, SWITCH_ROUTE_ATTR_PACKET_ACTION, SWITCH_ROUTE_ATTR_PACKET_ACTION_TRAP)
            send_packet(self, self.devports[0], pkt)
            verify_packets(self, trap_cpu_pkt, [self.cpu_port])
            self.attribute_set(self.route0, SWITCH_ROUTE_ATTR_NEXTHOP_HANDLE, 0)
            send_packet(self, self.devports[0], pkt)
            verify_packets(self, trap_cpu_pkt, [self.cpu_port])
            print("Transition from trap to drop")
            self.attribute_set(self.route0, SWITCH_ROUTE_ATTR_PACKET_ACTION, SWITCH_ROUTE_ATTR_PACKET_ACTION_DROP)
            send_packet(self, self.devports[0], pkt)
            verify_no_other_packets(self, timeout=2)
            print("Transition from drop to forward")
            self.attribute_set(self.route0, SWITCH_ROUTE_ATTR_PACKET_ACTION, SWITCH_ROUTE_ATTR_PACKET_ACTION_FORWARD)
            self.attribute_set(self.route0, SWITCH_ROUTE_ATTR_NEXTHOP_HANDLE, self.nhop1)
            send_packet(self, self.devports[0], pkt)
            verify_packets(self, exp_pkt, [self.devports[1]])
        finally:
            self.attribute_set(self.route0, SWITCH_ROUTE_ATTR_NEXTHOP_HANDLE, self.nhop1)

    def tearDown(self):
        self.cleanup()

###############################################################################

@group('l3')
class L3MultipleRifTest(ApiHelper):
    nmac0 = "00:11:22:33:44:05"
    nmac1 = "00:11:22:33:44:15"
    mac01 = "00:11:22:33:44:01"
    nmac01 = "00:11:22:33:01:55"
    mac02 = "00:11:22:33:44:02"
    nmac02 = "00:11:22:33:02:55"
    mac11 = "00:11:22:33:44:11"
    nmac11 = "00:11:22:33:11:55"
    mac12 = "00:11:22:33:44:12"
    nmac12 = "00:11:22:33:12:55"
    def setUp(self):
        print()
        self.configure()
        if (self.client.is_feature_enable(SWITCH_FEATURE_MULTIPLE_RIFS_PER_PORT) == 0):
          print("Multiple RIFs not supported")
          return

        # ing/eg RIFs on port0
        self.ingress_rif0 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port0, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.nhop0 = self.add_nexthop(self.device, handle=self.ingress_rif0, dest_ip='11.11.0.1')
        self.add_neighbor(self.device, mac_address=self.nmac0, handle=self.ingress_rif0, dest_ip='11.11.0.1')
        self.egress_rif01 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port0, vrf_handle=self.vrf10, src_mac=self.mac01)
        self.nhop01 = self.add_nexthop(self.device, handle=self.egress_rif01, dest_ip='10.10.0.1')
        self.add_neighbor(self.device, mac_address=self.nmac01, handle=self.egress_rif01, dest_ip='10.10.0.1')
        self.egress_rif02 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port0, vrf_handle=self.vrf10, src_mac=self.mac02)
        self.nhop02 = self.add_nexthop(self.device, handle=self.egress_rif02, dest_ip='10.10.0.2')
        self.add_neighbor(self.device, mac_address=self.nmac02, handle=self.egress_rif02, dest_ip='10.10.0.2')

        # ing/eg RIFs on port1
        self.ingress_rif1 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port1, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.nhop1 = self.add_nexthop(self.device, handle=self.ingress_rif1, dest_ip='11.11.1.1')
        self.add_neighbor(self.device, mac_address=self.nmac1, handle=self.ingress_rif1, dest_ip='11.11.1.1')
        self.egress_rif11 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port1, vrf_handle=self.vrf10, src_mac=self.mac11)
        self.nhop11 = self.add_nexthop(self.device, handle=self.egress_rif11, dest_ip='10.10.1.1')
        self.add_neighbor(self.device, mac_address=self.nmac11, handle=self.egress_rif11, dest_ip='10.10.1.1')
        self.egress_rif12 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port1, vrf_handle=self.vrf10, src_mac=self.mac12)
        self.nhop12 = self.add_nexthop(self.device, handle=self.egress_rif12, dest_ip='10.10.1.2')
        self.add_neighbor(self.device, mac_address=self.nmac12, handle=self.egress_rif12, dest_ip='10.10.1.2')

        self.add_route(self.device, ip_prefix='12.0.0.1', vrf_handle=self.vrf10, nexthop_handle=self.nhop0)
        self.add_route(self.device, ip_prefix='12.12.0.1', vrf_handle=self.vrf10, nexthop_handle=self.nhop01)
        self.add_route(self.device, ip_prefix='12.12.0.2', vrf_handle=self.vrf10, nexthop_handle=self.nhop02)
        self.add_route(self.device, ip_prefix='12.0.1.1', vrf_handle=self.vrf10, nexthop_handle=self.nhop1)
        self.add_route(self.device, ip_prefix='12.12.1.1', vrf_handle=self.vrf10, nexthop_handle=self.nhop11)
        self.add_route(self.device, ip_prefix='12.12.1.2', vrf_handle=self.vrf10, nexthop_handle=self.nhop12)

    def runTest(self):
        if (self.client.is_feature_enable(SWITCH_FEATURE_MULTIPLE_RIFS_PER_PORT) == 0):
          return
        try:
            # irif0 -> erif1
            pkt = simple_tcp_packet(
                eth_dst=self.rmac,
                ip_dst='12.0.1.1',
                ip_ttl=64)
            exp_pkt = simple_tcp_packet(
                eth_dst=self.nmac1,
                eth_src=self.rmac,
                ip_dst='12.0.1.1',
                ip_ttl=63)
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt, self.devports[1])
            # irif01 -> erif11
            pkt = simple_tcp_packet(
                eth_dst=self.mac01,
                ip_dst='12.12.1.1',
                ip_ttl=64)
            exp_pkt = simple_tcp_packet(
                eth_dst=self.nmac11,
                eth_src=self.mac11,
                ip_dst='12.12.1.1',
                ip_ttl=63)
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt, self.devports[1])
            # irif02 -> erif12
            pkt = simple_tcp_packet(
                eth_dst=self.mac02,
                ip_dst='12.12.1.2',
                ip_ttl=64)
            exp_pkt = simple_tcp_packet(
                eth_dst=self.nmac12,
                eth_src=self.mac12,
                ip_dst='12.12.1.2',
                ip_ttl=63)
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt, self.devports[1])
            # erif11 -> irif02
            pkt = simple_tcp_packet(
                eth_dst=self.mac11,
                ip_dst='12.12.0.2',
                ip_ttl=64)
            exp_pkt = simple_tcp_packet(
                eth_dst=self.nmac02,
                eth_src=self.mac02,
                ip_dst='12.12.0.2',
                ip_ttl=63)
            send_packet(self, self.devports[1], pkt)
            verify_packet(self, exp_pkt, self.devports[0])
            # erif12 -> irif01
            pkt = simple_tcp_packet(
                eth_dst=self.mac12,
                ip_dst='12.12.0.1',
                ip_ttl=64)
            exp_pkt = simple_tcp_packet(
                eth_dst=self.nmac01,
                eth_src=self.mac01,
                ip_dst='12.12.0.1',
                ip_ttl=63)
            send_packet(self, self.devports[1], pkt)
            verify_packet(self, exp_pkt, self.devports[0])
        finally:
            pass

    def tearDown(self):
        self.cleanup()

###############################################################################

@disabled
class L3BfdTest(ApiHelper):
    def setUp(self):
        print()

        self.configure()
        self.mac = '8e:fd:6b:20:ab:ba'
        self.src_ip = '192.168.67.101'
        self.dst_ip = '192.168.67.100'
        self.udp_src_port=5012
        self.version = 0x1
        self.sta = 0x1
        self.detect_mult = 0x3
        self.local_discriminator = 1234
        self.remote_discriminator = 5678
        self.min_tx = 1000000
        self.min_rx = 1000000
        self.echo_rx = 0
        
        self.rif1 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port0,
                                 vrf_handle=self.vrf10, src_mac=self.rmac)
        self.nhop1 = self.add_nexthop(self.device, handle=self.rif1, dest_ip=self.dst_ip)
        self.neighbor1 = self.add_neighbor(self.device, mac_address=self.mac,
                                           handle=self.rif1, dest_ip=self.dst_ip)
        self.add_route(self.device, ip_prefix=self.dst_ip, vrf_handle=self.default_vrf,
                        nexthop_handle=self.nhop1)
        self.add_route(self.device, ip_prefix=self.src_ip, vrf_handle=self.vrf10,
                       is_host_myip=True)
        queue_handles = self.attribute_get(self.cpu_port_hdl, SWITCH_PORT_ATTR_QUEUE_HANDLES)
        self.hostif_trap_group0 = self.add_hostif_trap_group(self.device, 
                                                             queue_handle=queue_handles[0].oid,
                                                             admin_state=1)
        # create hostif_trap entries for following trap types.
        trap_list = [
          [SWITCH_HOSTIF_TRAP_ATTR_TYPE_BFD, SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_REDIRECT_TO_CPU, 4],
          [SWITCH_HOSTIF_TRAP_ATTR_TYPE_MYIP, SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_COPY_TO_CPU, 8]
        ]
        for trap in trap_list:
            trap.append(self.add_hostif_trap(self.device, type=trap[0],
                        hostif_trap_group_handle=self.hostif_trap_group0,
                        packet_action=trap[1], priority=trap[2]))

    def runTest(self):
        try:
           self.bfdTestDownInitUp()
           self.bfdTestDownUp()
           self.bfdTestDownInitTimeoutDown()
           self.bfdTestDownUpTimeoutDown()
           self.bfdTestDownUpDown()
           self.bfdTestDownDown()
           self.bfdTestDownInitInit()
           self.bfdTestDownUpUp()
           self.bfdTestwithDifferentLocalAndRemoteTimers()
           #self.bfdTestDownInitAdminDown()
           #self.bfdTestDownupAdminDown()
           self.bfdTestDownInitUpWithPassive()
           self.bfdTestDownInitTimeoutWithPassive()
           self.bfdTestDownInitUpTimeoutWithPassive()

        finally:
            pass

    def tearDown(self):
        self.cleanup()
 
    def bfdTestDownInitUp(self):
        print("bfdTestDownInitUp")
        try:
            self.add_bfd_session(self.device,vrf_handle=self.default_vrf,
                             local_discriminator=self.local_discriminator,
                             src_ip_address=self.src_ip, dst_ip_address=self.dst_ip,
                             udp_src_port=self.udp_src_port,min_rx=self.min_rx,min_tx=self.min_tx,
                             multiplier=self.detect_mult)
            bfd_pkt = self.bfd_ipv4_packet(
                         eth_dst=self.rmac,
                         eth_src=self.mac,
                         ip_src=self.dst_ip,
                         ip_dst=self.src_ip,
                         sta=0x1,
                         my_discriminator=self.remote_discriminator)
            exp_bfd_pkt = self.bfd_ipv4_packet(
                         eth_dst=self.mac,
                         eth_src=self.rmac,
                         ip_src=self.src_ip,
                         ip_dst=self.dst_ip,
                         sta=0x2,
                         my_discriminator=self.local_discriminator,
                         your_discriminator=self.remote_discriminator)

            m = Mask(exp_bfd_pkt)
            mask_set_do_not_care_packet(m, IP, "id")
            mask_set_do_not_care_packet(m, IP, "chksum")
            mask_set_do_not_care_packet(m, UDP, "sport")
            mask_set_do_not_care_packet(m, UDP, "chksum")

            #send bfd pkt with DOWN state
            send_packet(self, self.devports[0], bfd_pkt)

            #verify if bfd pkt is INIT state
            verify_packet(self, m, self.devports[0],timeout=2)
                
            bfd_pkt[BFD].sta=0x3
            bfd_pkt[BFD].your_discriminator=self.local_discriminator
            exp_bfd_pkt[BFD].sta=0x3
            m = Mask(exp_bfd_pkt)
            mask_set_do_not_care_packet(m, IP, "id")
            mask_set_do_not_care_packet(m, IP, "chksum")
            mask_set_do_not_care_packet(m, UDP, "sport")
            mask_set_do_not_care_packet(m, UDP, "chksum")

            #send bfd pkt with UP state
            send_packet(self, self.devports[0], bfd_pkt)

            #verify if bfd pkt is UP state
            verify_packet(self, m, self.devports[0],timeout=5)

        finally:
            self.cleanlast()
            pass

    def bfdTestDownUp(self):
        print("bfdTestDownUp")
        try:
            self.add_bfd_session(self.device,vrf_handle=self.default_vrf,
                             local_discriminator=self.local_discriminator,
                             src_ip_address=self.src_ip, dst_ip_address=self.dst_ip,
                             udp_src_port=self.udp_src_port,min_rx=self.min_rx,min_tx=self.min_tx,
                             multiplier=self.detect_mult)
            bfd_pkt = self.bfd_ipv4_packet(
                         eth_dst=self.rmac,
                         eth_src=self.mac,
                         ip_src=self.dst_ip,
                         ip_dst=self.src_ip,
                         sta=0x2,
                         my_discriminator=self.remote_discriminator,
                         your_discriminator=self.local_discriminator)
            exp_bfd_pkt = self.bfd_ipv4_packet(
                         eth_dst=self.mac,
                         eth_src=self.rmac,
                         ip_src=self.src_ip,
                         ip_dst=self.dst_ip,
                         sta=0x3,
                         my_discriminator=self.local_discriminator,
                         your_discriminator=self.remote_discriminator)

            m = Mask(exp_bfd_pkt)
            mask_set_do_not_care_packet(m, IP, "id")
            mask_set_do_not_care_packet(m, IP, "chksum")
            mask_set_do_not_care_packet(m, UDP, "sport")
            mask_set_do_not_care_packet(m, UDP, "chksum")

            #send bfd pkt with INIT state
            send_packet(self, self.devports[0], bfd_pkt)

            #verify if bfd pkt is UP state
            verify_packet(self, m, self.devports[0],timeout=5)

        finally:
            self.cleanlast()
            pass

    def bfdTestDownInitTimeoutDown(self):
        print("bfdTestDownInitTimeoutDown")
        try:
            self.add_bfd_session(self.device,vrf_handle=self.default_vrf,
                             local_discriminator=self.local_discriminator,
                             src_ip_address=self.src_ip, dst_ip_address=self.dst_ip,
                             udp_src_port=self.udp_src_port,min_rx=self.min_rx,min_tx=self.min_tx,
                             multiplier=self.detect_mult)
            bfd_pkt = self.bfd_ipv4_packet(
                         eth_dst=self.rmac,
                         eth_src=self.mac,
                         ip_src=self.dst_ip,
                         ip_dst=self.src_ip,
                         sta=0x1,
                         my_discriminator=self.remote_discriminator)
            exp_bfd_pkt = self.bfd_ipv4_packet(
                         eth_dst=self.mac,
                         eth_src=self.rmac,
                         ip_src=self.src_ip,
                         ip_dst=self.dst_ip,
                         sta=0x2,
                         my_discriminator=self.local_discriminator,
                         your_discriminator=self.remote_discriminator)

            m = Mask(exp_bfd_pkt)
            mask_set_do_not_care_packet(m, IP, "id")
            mask_set_do_not_care_packet(m, IP, "chksum")
            mask_set_do_not_care_packet(m, UDP, "sport")
            mask_set_do_not_care_packet(m, UDP, "chksum")

            #send bfd pkt with DOWN state
            send_packet(self, self.devports[0], bfd_pkt)
            time.sleep(2)

            #verify if bfd pkt is INIT state
            verify_packet(self, m, self.devports[0])
                
            exp_bfd_pkt[BFD].sta=0x1
            exp_bfd_pkt[BFD].diag=0x1
            m = Mask(exp_bfd_pkt)
            mask_set_do_not_care_packet(m, IP, "id")
            mask_set_do_not_care_packet(m, IP, "chksum")
            mask_set_do_not_care_packet(m, UDP, "sport")
            mask_set_do_not_care_packet(m, UDP, "chksum")
            

            #verify if bfd pkt is DOWN state
            verify_packet(self, m, self.devports[0],timeout=3)

        finally:
            self.cleanlast()
            pass

    def bfdTestDownUpTimeoutDown(self):
        print("bfdTestDownUpTimeoutDown")
        try:
            self.add_bfd_session(self.device,vrf_handle=self.default_vrf,
                             local_discriminator=self.local_discriminator,
                             src_ip_address=self.src_ip, dst_ip_address=self.dst_ip,
                             udp_src_port=self.udp_src_port,min_rx=self.min_rx,min_tx=self.min_tx,
                             multiplier=self.detect_mult)
            bfd_pkt = self.bfd_ipv4_packet(
                         eth_dst=self.rmac,
                         eth_src=self.mac,
                         ip_src=self.dst_ip,
                         ip_dst=self.src_ip,
                         sta=0x2,
                         my_discriminator=self.remote_discriminator,
                         your_discriminator=self.local_discriminator)
            exp_bfd_pkt = self.bfd_ipv4_packet(
                         eth_dst=self.mac,
                         eth_src=self.rmac,
                         ip_src=self.src_ip,
                         ip_dst=self.dst_ip,
                         sta=0x3,
                         my_discriminator=self.local_discriminator,
                         your_discriminator=self.remote_discriminator)

            m = Mask(exp_bfd_pkt)
            mask_set_do_not_care_packet(m, IP, "id")
            mask_set_do_not_care_packet(m, IP, "chksum")
            mask_set_do_not_care_packet(m, UDP, "sport")
            mask_set_do_not_care_packet(m, UDP, "chksum")

            #send bfd pkt with INIT state
            send_packet(self, self.devports[0], bfd_pkt)
            time.sleep(5)

            #verify if bfd pkt is UP state
            verify_packet(self, m, self.devports[0])

            exp_bfd_pkt[BFD].sta=0x1
            exp_bfd_pkt[BFD].diag=0x3
            m = Mask(exp_bfd_pkt)
            mask_set_do_not_care_packet(m, IP, "id")
            mask_set_do_not_care_packet(m, IP, "chksum")
            mask_set_do_not_care_packet(m, UDP, "sport")
            mask_set_do_not_care_packet(m, UDP, "chksum")
            
            #wait for UP timeout
            #verify if bfd pkt is DOWN state
            verify_packet(self, m, self.devports[0],timeout=3)

        finally:
            self.cleanlast()
            pass

    def bfdTestDownUpDown(self):
        print("bfdTestDownUpDown")
        try:
            self.add_bfd_session(self.device,vrf_handle=self.default_vrf,
                             local_discriminator=self.local_discriminator,
                             src_ip_address=self.src_ip, dst_ip_address=self.dst_ip,
                             udp_src_port=self.udp_src_port,min_rx=self.min_rx,min_tx=self.min_tx,
                             multiplier=self.detect_mult)
            bfd_pkt = self.bfd_ipv4_packet(
                         eth_dst=self.rmac,
                         eth_src=self.mac,
                         ip_src=self.dst_ip,
                         ip_dst=self.src_ip,
                         sta=0x2,
                         my_discriminator=self.remote_discriminator,
                         your_discriminator=self.local_discriminator)
            exp_bfd_pkt = self.bfd_ipv4_packet(
                         eth_dst=self.mac,
                         eth_src=self.rmac,
                         ip_src=self.src_ip,
                         ip_dst=self.dst_ip,
                         sta=0x3,
                         my_discriminator=self.local_discriminator,
                         your_discriminator=self.remote_discriminator)

            m = Mask(exp_bfd_pkt)
            mask_set_do_not_care_packet(m, IP, "id")
            mask_set_do_not_care_packet(m, IP, "chksum")
            mask_set_do_not_care_packet(m, UDP, "sport")
            mask_set_do_not_care_packet(m, UDP, "chksum")

            #send bfd pkt with INIT state
            send_packet(self, self.devports[0], bfd_pkt)

            #verify if bfd pkt is UP state
            verify_packet(self, m, self.devports[0],timeout=5)

            bfd_pkt[BFD].sta=0x1
            exp_bfd_pkt[BFD].sta=0x1
            exp_bfd_pkt[BFD].diag=0x3
            m = Mask(exp_bfd_pkt)
            mask_set_do_not_care_packet(m, IP, "id")
            mask_set_do_not_care_packet(m, IP, "chksum")
            mask_set_do_not_care_packet(m, UDP, "sport")
            mask_set_do_not_care_packet(m, UDP, "chksum")
            
            #send bfd pkt with DOWN state
            send_packet(self, self.devports[0], bfd_pkt)

            #veirfy if bfd pkt is DOWN state
            verify_packet(self, m, self.devports[0])
        finally:
            self.cleanlast()
            pass

    def bfdTestDownDown(self):
        print("bfdTestDownDown")
        try:
            self.add_bfd_session(self.device,vrf_handle=self.default_vrf,
                             local_discriminator=self.local_discriminator,
                             src_ip_address=self.src_ip, dst_ip_address=self.dst_ip,
                             udp_src_port=self.udp_src_port,min_rx=self.min_rx,min_tx=self.min_tx,
                             multiplier=self.detect_mult)
            bfd_pkt = self.bfd_ipv4_packet(
                         eth_dst=self.rmac,
                         eth_src=self.mac,
                         ip_src=self.dst_ip,
                         ip_dst=self.src_ip,
                         sta=0x3,
                         my_discriminator=self.remote_discriminator,
                         your_discriminator=self.local_discriminator)
            exp_bfd_pkt = self.bfd_ipv4_packet(
                         eth_dst=self.mac,
                         eth_src=self.rmac,
                         ip_src=self.src_ip,
                         ip_dst=self.dst_ip,
                         sta=0x1,
                         my_discriminator=self.local_discriminator,
                         your_discriminator=0)

            m = Mask(exp_bfd_pkt)
            mask_set_do_not_care_packet(m, IP, "id")
            mask_set_do_not_care_packet(m, IP, "chksum")
            mask_set_do_not_care_packet(m, UDP, "sport")
            mask_set_do_not_care_packet(m, UDP, "chksum")

            #verify if bfd pkt is DOWN state
            verify_packet(self, m, self.devports[0])

            #send bfd pkt with UP state
            send_packet(self, self.devports[0], bfd_pkt)

            exp_bfd_pkt[BFD].your_discriminator=self.remote_discriminator
            m = Mask(exp_bfd_pkt)
            mask_set_do_not_care_packet(m, IP, "id")
            mask_set_do_not_care_packet(m, IP, "chksum")
            mask_set_do_not_care_packet(m, UDP, "sport")
            mask_set_do_not_care_packet(m, UDP, "chksum")

            #verify if bfd pkt is still DOWN state
            verify_packet(self, m, self.devports[0])

        finally:
            self.cleanlast()
            pass

    def bfdTestDownInitInit(self):
        print("bfdTestDownInitInit")
        try:
            self.add_bfd_session(self.device,vrf_handle=self.default_vrf,
                             local_discriminator=self.local_discriminator,
                             src_ip_address=self.src_ip, dst_ip_address=self.dst_ip,
                             udp_src_port=self.udp_src_port,min_rx=self.min_rx,min_tx=self.min_tx,
                             multiplier=self.detect_mult)
            bfd_pkt = self.bfd_ipv4_packet(
                         eth_dst=self.rmac,
                         eth_src=self.mac,
                         ip_src=self.dst_ip,
                         ip_dst=self.src_ip,
                         sta=0x1,
                         my_discriminator=self.remote_discriminator)
            exp_bfd_pkt = self.bfd_ipv4_packet(
                         eth_dst=self.mac,
                         eth_src=self.rmac,
                         ip_src=self.src_ip,
                         ip_dst=self.dst_ip,
                         sta=0x2,
                         my_discriminator=self.local_discriminator,
                         your_discriminator=self.remote_discriminator)

            m = Mask(exp_bfd_pkt)
            mask_set_do_not_care_packet(m, IP, "id")
            mask_set_do_not_care_packet(m, IP, "chksum")
            mask_set_do_not_care_packet(m, UDP, "sport")
            mask_set_do_not_care_packet(m, UDP, "chksum")

            #send bfd pkt with DOWN state
            send_packet(self, self.devports[0], bfd_pkt)

            #verify if bfd pkt is INIT state
            verify_packet(self, m, self.devports[0],timeout=2)
                
            exp_bfd_pkt[BFD].sta=0x2
            m = Mask(exp_bfd_pkt)
            mask_set_do_not_care_packet(m, IP, "id")
            mask_set_do_not_care_packet(m, IP, "chksum")
            mask_set_do_not_care_packet(m, UDP, "sport")
            mask_set_do_not_care_packet(m, UDP, "chksum")
            
            #send again bfd pkt with DOWN state
            send_packet(self, self.devports[0], bfd_pkt)

            #verify if bfd pkt is INIT state
            verify_packet(self, m, self.devports[0])

        finally:
            self.cleanlast()
            pass

    def bfdTestDownUpUp(self):
        print("bfdTestDownUpUp")
        try:
            self.add_bfd_session(self.device,vrf_handle=self.default_vrf,
                             local_discriminator=self.local_discriminator,
                             src_ip_address=self.src_ip, dst_ip_address=self.dst_ip,
                             udp_src_port=self.udp_src_port,min_rx=self.min_rx,min_tx=self.min_tx,
                             multiplier=self.detect_mult)
            bfd_pkt = self.bfd_ipv4_packet(
                         eth_dst=self.rmac,
                         eth_src=self.mac,
                         ip_src=self.dst_ip,
                         ip_dst=self.src_ip,
                         sta=0x2,
                         my_discriminator=self.remote_discriminator,
                         your_discriminator=self.local_discriminator)
            exp_bfd_pkt = self.bfd_ipv4_packet(
                         eth_dst=self.mac,
                         eth_src=self.rmac,
                         ip_src=self.src_ip,
                         ip_dst=self.dst_ip,
                         sta=0x3,
                         my_discriminator=self.local_discriminator,
                         your_discriminator=self.remote_discriminator)

            m = Mask(exp_bfd_pkt)
            mask_set_do_not_care_packet(m, IP, "id")
            mask_set_do_not_care_packet(m, IP, "chksum")
            mask_set_do_not_care_packet(m, UDP, "sport")
            mask_set_do_not_care_packet(m, UDP, "chksum")

            #send bfd pkt with INIT state
            send_packet(self, self.devports[0], bfd_pkt)

            #verify if bfd pkt is UP state
            verify_packet(self, m, self.devports[0],timeout=5)

            #send bfd pkt with INIT state
            send_packet(self, self.devports[0], bfd_pkt)

            #verify if bfd pkt is UP state
            verify_packet(self, m, self.devports[0],timeout=2)

            bfd_pkt[BFD].sta=0x3
            #send bfd pkt with UP state
            send_packet(self, self.devports[0], bfd_pkt)

            #verify if bfd pkt is UP state
            verify_packet(self, m, self.devports[0],timeout=2)

        finally:
            self.cleanlast()
            pass

    def bfdTestwithDifferentLocalAndRemoteTimers(self):
        print("bfdTestwithDifferentLocalAndRemoteTimers")
        try:
            self.add_bfd_session(self.device,vrf_handle=self.default_vrf,
                             local_discriminator=self.local_discriminator,
                             src_ip_address=self.src_ip, dst_ip_address=self.dst_ip,
                             udp_src_port=self.udp_src_port,min_rx=self.min_rx,min_tx=self.min_tx,
                             multiplier=self.detect_mult)
            bfd_pkt = self.bfd_ipv4_packet(
                         eth_dst=self.rmac,
                         eth_src=self.mac,
                         ip_src=self.dst_ip,
                         ip_dst=self.src_ip,
                         sta=0x1,
                         my_discriminator=self.remote_discriminator,
                         min_tx_interval=2000000,
                         min_rx_interval=2000000,
                         detect_mult=0x4)
            exp_bfd_pkt = self.bfd_ipv4_packet(
                         eth_dst=self.mac,
                         eth_src=self.rmac,
                         ip_src=self.src_ip,
                         ip_dst=self.dst_ip,
                         sta=0x2,
                         my_discriminator=self.local_discriminator,
                         your_discriminator=self.remote_discriminator)

            m = Mask(exp_bfd_pkt)
            mask_set_do_not_care_packet(m, IP, "id")
            mask_set_do_not_care_packet(m, IP, "chksum")
            mask_set_do_not_care_packet(m, UDP, "sport")
            mask_set_do_not_care_packet(m, UDP, "chksum")

            #send bfd pkt with DOWN state
            send_packet(self, self.devports[0], bfd_pkt)

            #verify if bfd pkt is INIT state
            verify_packet(self, m, self.devports[0],timeout=2)

            exp_bfd_pkt[BFD].sta=0x2
            m = Mask(exp_bfd_pkt)
            mask_set_do_not_care_packet(m, IP, "id")
            mask_set_do_not_care_packet(m, IP, "chksum")
            mask_set_do_not_care_packet(m, UDP, "sport")
            mask_set_do_not_care_packet(m, UDP, "chksum")
            
            #wait for INIT timeout of wrong interval
            time.sleep(3)
            #verify if bfd pkt is INIT state
            verify_packet(self, m, self.devports[0])
               
            exp_bfd_pkt[BFD].sta=0x1
            exp_bfd_pkt[BFD].diag=0x1
            m = Mask(exp_bfd_pkt)
            mask_set_do_not_care_packet(m, IP, "id")
            mask_set_do_not_care_packet(m, IP, "chksum")
            mask_set_do_not_care_packet(m, UDP, "sport")
            mask_set_do_not_care_packet(m, UDP, "chksum")
            
            #wait for INIT timeout
            time.sleep(5)
            #verify if bfd pkt is DOWN state
            verify_packet(self, m, self.devports[0])

        finally:
            self.cleanlast()
            pass

    def bfdTestDownInitUpWithPassive(self):
        print("bfdTestDownInitUpWithPassive")
        try:
            self.add_bfd_session(self.device,vrf_handle=self.default_vrf,
                             type=SWITCH_BFD_SESSION_ATTR_TYPE_ASYNC_PASSIVE,
                             local_discriminator=self.local_discriminator,
                             src_ip_address=self.src_ip, dst_ip_address=self.dst_ip,
                             udp_src_port=self.udp_src_port,min_rx=self.min_rx,min_tx=self.min_tx,
                             multiplier=self.detect_mult)
            bfd_pkt = self.bfd_ipv4_packet(
                         eth_dst=self.rmac,
                         eth_src=self.mac,
                         ip_src=self.dst_ip,
                         ip_dst=self.src_ip,
                         sta=0x1,
                         my_discriminator=self.remote_discriminator)
            exp_bfd_pkt = self.bfd_ipv4_packet(
                         eth_dst=self.mac,
                         eth_src=self.rmac,
                         ip_src=self.src_ip,
                         ip_dst=self.dst_ip,
                         sta=0x2,
                         my_discriminator=self.local_discriminator,
                         your_discriminator=self.remote_discriminator)

            m = Mask(exp_bfd_pkt)
            mask_set_do_not_care_packet(m, IP, "id")
            mask_set_do_not_care_packet(m, IP, "chksum")
            mask_set_do_not_care_packet(m, UDP, "sport")
            mask_set_do_not_care_packet(m, UDP, "chksum")

            #send bfd pkt with DOWN state
            send_packet(self, self.devports[0], bfd_pkt)

            #verify if bfd pkt is INIT state
            verify_packet(self, m, self.devports[0],timeout=2)
                
            bfd_pkt[BFD].sta=0x3
            bfd_pkt[BFD].your_discriminator=self.local_discriminator
            exp_bfd_pkt[BFD].sta=0x3
            m = Mask(exp_bfd_pkt)
            mask_set_do_not_care_packet(m, IP, "id")
            mask_set_do_not_care_packet(m, IP, "chksum")
            mask_set_do_not_care_packet(m, UDP, "sport")
            mask_set_do_not_care_packet(m, UDP, "chksum")

            #send bfd pkt with UP state
            send_packet(self, self.devports[0], bfd_pkt)

            #verify if bfd pkt is UP state
            verify_packet(self, m, self.devports[0],timeout=5)

        finally:
            self.cleanlast()
            pass

    def bfdTestDownInitTimeoutWithPassive(self):
        print("bfdTestDownInitTimeoutWithPassive")
        try:
            self.add_bfd_session(self.device,vrf_handle=self.default_vrf,
                             type=SWITCH_BFD_SESSION_ATTR_TYPE_ASYNC_PASSIVE,
                             local_discriminator=self.local_discriminator,
                             src_ip_address=self.src_ip, dst_ip_address=self.dst_ip,
                             udp_src_port=self.udp_src_port,min_rx=self.min_rx,min_tx=self.min_tx,
                             multiplier=self.detect_mult)
            bfd_pkt = self.bfd_ipv4_packet(
                         eth_dst=self.rmac,
                         eth_src=self.mac,
                         ip_src=self.dst_ip,
                         ip_dst=self.src_ip,
                         sta=0x1,
                         my_discriminator=self.remote_discriminator)
            exp_bfd_pkt = self.bfd_ipv4_packet(
                         eth_dst=self.mac,
                         eth_src=self.rmac,
                         ip_src=self.src_ip,
                         ip_dst=self.dst_ip,
                         sta=0x2,
                         my_discriminator=self.local_discriminator,
                         your_discriminator=self.remote_discriminator)

            m = Mask(exp_bfd_pkt)
            mask_set_do_not_care_packet(m, IP, "id")
            mask_set_do_not_care_packet(m, IP, "chksum")
            mask_set_do_not_care_packet(m, UDP, "sport")
            mask_set_do_not_care_packet(m, UDP, "chksum")

            #send bfd pkt with DOWN state
            send_packet(self, self.devports[0], bfd_pkt)

            #verify if bfd pkt is INIT state
            verify_packet(self, m, self.devports[0],timeout=2)
                
            exp_bfd_pkt[BFD].sta=0x1
            exp_bfd_pkt[BFD].diag=0x1
            m = Mask(exp_bfd_pkt)
            mask_set_do_not_care_packet(m, IP, "id")
            mask_set_do_not_care_packet(m, IP, "chksum")
            mask_set_do_not_care_packet(m, UDP, "sport")
            mask_set_do_not_care_packet(m, UDP, "chksum")
            

            #verify if bfd pkt is not recevied with DOWN state
            verify_no_packet(self, m, self.devports[0],timeout=3)

        finally:
            self.cleanlast()
            pass

    def bfdTestDownInitUpTimeoutWithPassive(self):
        print("bfdTestDownInitUpTimeoutWithPassive")
        try:
            self.add_bfd_session(self.device,vrf_handle=self.default_vrf,
                             type=SWITCH_BFD_SESSION_ATTR_TYPE_ASYNC_PASSIVE,
                             local_discriminator=self.local_discriminator,
                             src_ip_address=self.src_ip, dst_ip_address=self.dst_ip,
                             udp_src_port=self.udp_src_port,min_rx=self.min_rx,min_tx=self.min_tx,
                             multiplier=self.detect_mult)
            bfd_pkt = self.bfd_ipv4_packet(
                         eth_dst=self.rmac,
                         eth_src=self.mac,
                         ip_src=self.dst_ip,
                         ip_dst=self.src_ip,
                         sta=0x1,
                         my_discriminator=self.remote_discriminator)
            exp_bfd_pkt = self.bfd_ipv4_packet(
                         eth_dst=self.mac,
                         eth_src=self.rmac,
                         ip_src=self.src_ip,
                         ip_dst=self.dst_ip,
                         sta=0x2,
                         my_discriminator=self.local_discriminator,
                         your_discriminator=self.remote_discriminator)

            m = Mask(exp_bfd_pkt)
            mask_set_do_not_care_packet(m, IP, "id")
            mask_set_do_not_care_packet(m, IP, "chksum")
            mask_set_do_not_care_packet(m, UDP, "sport")
            mask_set_do_not_care_packet(m, UDP, "chksum")

            #send bfd pkt with DOWN state
            send_packet(self, self.devports[0], bfd_pkt)

            #verify if bfd pkt is INIT state
            verify_packet(self, m, self.devports[0],timeout=2)
                
            bfd_pkt[BFD].sta=0x3
            bfd_pkt[BFD].your_discriminator=self.local_discriminator
            exp_bfd_pkt[BFD].sta=0x3
            m = Mask(exp_bfd_pkt)
            mask_set_do_not_care_packet(m, IP, "id")
            mask_set_do_not_care_packet(m, IP, "chksum")
            mask_set_do_not_care_packet(m, UDP, "sport")
            mask_set_do_not_care_packet(m, UDP, "chksum")

            #send bfd pkt with UP state
            send_packet(self, self.devports[0], bfd_pkt)

            #verify if bfd pkt is UP state
            verify_packet(self, m, self.devports[0],timeout=5)

            exp_bfd_pkt[BFD].sta=0x1
            exp_bfd_pkt[BFD].diag=0x3
            m = Mask(exp_bfd_pkt)
            mask_set_do_not_care_packet(m, IP, "id")
            mask_set_do_not_care_packet(m, IP, "chksum")
            mask_set_do_not_care_packet(m, UDP, "sport")
            mask_set_do_not_care_packet(m, UDP, "chksum")
            
            #wait for UP timeout
            #verify if bfd pkt is not recevied with DOWN state
            verify_no_packet(self, m, self.devports[0],timeout=5)



        finally:
            self.cleanlast()
            pass

    def bfd_ipv4_packet(self,                                    
                        eth_dst='00:01:02:03:04:05',             
                        eth_src='00:06:07:08:09:0a',             
                        ip_src='192.168.0.1',                    
                        ip_dst='192.168.0.2',                    
                        udp_sport=1234,
                        udp_dport=3784,                          
                        version=0x1,                             
                        diag=0x0,                                
                        sta=0x0,                                 
                        flags=0x0,                               
                        detect_mult=0x3,                         
                        my_discriminator=0x11111111,             
                        your_discriminator=0,                    
                        min_tx_interval=1000000,                 
                        min_rx_interval=1000000,                        
                        echo_rx_interval=0):

        bfd_hdr = BFD(version = version,
                      diag = diag,
                      sta = sta,
                      flags = flags,
                      detect_mult = detect_mult,
                      len = 24,
                      my_discriminator = my_discriminator,
                      your_discriminator = your_discriminator,
                      min_tx_interval = min_tx_interval,
                      min_rx_interval = min_rx_interval,
                      echo_rx_interval = 0)

        bfd_pkt = simple_udp_packet(pktlen=66,
                                    eth_dst=eth_dst,
                                    eth_src=eth_src,
                                    ip_dst=ip_dst,
                                    ip_src=ip_src,
                                    ip_ttl=255,
                                    udp_sport=udp_sport,
                                    udp_dport=udp_dport,
                                    with_udp_chksum=True,
                                    udp_payload=bfd_hdr)
        return bfd_pkt


@group('l3')
class L3InterfaceTest(ApiHelper):
    def setUp(self):
        print()

        self.configure()
        self.nhop_drop = self.add_nexthop(self.device, type=SWITCH_NEXTHOP_ATTR_TYPE_DROP)
        self.rif0 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port0, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.rif0_counter = 0
        self.rif0_byte_counter = 0

        self.rif1 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port1, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.rif1_counter = 0
        self.rif1_byte_counter = 0
        self.nhop1 = self.add_nexthop(self.device, handle=self.rif1, dest_ip='10.10.0.2')
        self.neighbor1 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=self.rif1, dest_ip='10.10.0.2')

        self.rif2 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port2, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.rif2_counter = 0
        self.rif2_byte_counter = 0
        self.nhop2 = self.add_nexthop(self.device, handle=self.rif2, dest_ip='10.10.0.2')
        self.neighbor2 = self.add_neighbor(self.device, mac_address='00:22:22:33:44:55', handle=self.rif2, dest_ip='10.10.0.2')

        self.lag0 = self.add_lag(self.device)
        lag_mbr0 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port3)
        lag_mbr1 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port4)
        self.rif3 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.lag0, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.rif3_counter = 0
        self.rif3_byte_counter = 0
        self.nhop3 = self.add_nexthop(self.device, handle=self.rif3, dest_ip='10.10.0.2')
        self.neighbor3 = self.add_neighbor(self.device, mac_address='00:33:22:33:44:55', handle=self.rif3, dest_ip='10.10.0.2')
        # Set v4 route to nhop1
        self.route0 = self.add_route(self.device, ip_prefix='10.10.10.1', vrf_handle=self.vrf10, nexthop_handle=self.nhop1)
        self.route0_lpm = self.add_route(self.device, ip_prefix='11.11.11.0/24', vrf_handle=self.vrf10, nexthop_handle=self.nhop1)

        # Set v6 route 0/0 i.e. catch all --> to not forw the packets
        self.route_catch_all_lpm = self.add_route(self.device, ip_prefix='0::0/0', vrf_handle=self.vrf10, nexthop_handle=self.nhop_drop)
        # Set v6 routes to nhop1 - with prefix=128, prefix > 64
        self.route1 = self.add_route(self.device, ip_prefix='1234:5678:9abc:def0:4422:1133:5577:99aa', vrf_handle=self.vrf10, nexthop_handle=self.nhop1)
        self.route1_lpm = self.add_route(self.device, ip_prefix='4000::0/65', vrf_handle=self.vrf10, nexthop_handle=self.nhop1)

        # Set routes with prefix <= 64
        self.route1_lpm64 = self.add_route(self.device, ip_prefix='5000::0/64', vrf_handle=self.vrf10, nexthop_handle=self.nhop1)
        self.route2_lpm64 = self.add_route(self.device, ip_prefix='8500::0/10', vrf_handle=self.vrf10, nexthop_handle=self.nhop1)

        # Set route to lag
        self.route_lag0 = self.add_route(self.device, ip_prefix='12.10.10.1', vrf_handle=self.vrf10, nexthop_handle=self.nhop3)
        self.route_lag1 = self.add_route(self.device, ip_prefix='1234:5678:9abc:def0:1122:3344:5566:7788', vrf_handle=self.vrf10, nexthop_handle=self.nhop3)

        # Set route to glean
        self.nhop_glean = self.add_nexthop(self.device, type=SWITCH_NEXTHOP_ATTR_TYPE_GLEAN)
        self.route_glean = self.add_route(self.device, ip_prefix='13.20.10.1', vrf_handle=self.vrf10, nexthop_handle=self.nhop_glean)

        self.rif0_start = self.client.object_counters_get(self.rif0)
        self.rif1_start = self.client.object_counters_get(self.rif1)
        self.rif2_start = self.client.object_counters_get(self.rif2)
        self.rif3_start = self.client.object_counters_get(self.rif3)

        if (self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_COPP) == 1):
            queue_handles = self.attribute_get(self.cpu_port_hdl, SWITCH_PORT_ATTR_QUEUE_HANDLES)
            self.hostif_trap_group = self.add_hostif_trap_group(self.device,
                queue_handle=queue_handles[0].oid,
                admin_state=True)
            self.add_hostif_trap(self.device,
                type=SWITCH_HOSTIF_TRAP_ATTR_TYPE_L3_MTU_ERROR,
                hostif_trap_group_handle=self.hostif_trap_group,
                packet_action=SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_REDIRECT_TO_CPU)

        # For v4 FIB resolution - Program route entry with prefix <= 16 ---> nhop 1
        self.route_16 = self.add_route(self.device, ip_prefix='55.55.0.0/16', vrf_handle=self.vrf10, nexthop_handle=self.nhop1)
        # For v6 FIB resolution - Program route entry with prefix <= 64 ---> nhop 1
        self.route_64 = self.add_route(self.device, ip_prefix='AB15::0/64', vrf_handle=self.vrf10, nexthop_handle=self.nhop1)

    def runTest(self):
        try:
            self.RifVrfUpdateTest()
            self.RifIPv4DisableTest()
            self.RifIPv6DisableTest()
            self.RifRmacUpdateTest()
            self.IPv4FibTest()
            self.IPv4FibLocalHostTest()
            self.IPv4FibJumboTest()
            self.IPv4FibLPMTest()
            self.IPv4MtuTest()
            self.IPv6MtuTest()
            self.IPv4FibModifyTest()
            self.IPv4FibResolutionTest()
            self.IPv6FibTest()
            self.IPv6FibLPMTest()
            self.IPv6FibLPM64Test()
            self.IPv6FibModifyTest()
            self.IPv6FibResolutionTest()
            self.IPv4FibLagTest()
            self.IPv6FibLagTest()
            self.IPv4FibGleanTest()
            self.IPv4FibIngressVlanZeroTest()
            self.L3RifStatsTest()
        finally:
            pass

    def tearDown(self):
        self.cleanup()

    def RifVrfUpdateTest(self):
        print("RiVrfUpdateeTest")
        pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        exp_pkt = simple_tcp_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src='00:77:66:55:44:33',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63)
        exp_pkt2 = simple_tcp_packet(
            eth_dst='00:44:22:33:44:55',
            eth_src='00:77:66:55:44:33',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63)

        self.vrf20 = self.add_vrf(self.device, id=20, src_mac=self.rmac)
        self.rif4 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port5, vrf_handle=self.vrf20, src_mac=self.rmac)
        self.nhop4 = self.add_nexthop(self.device, handle=self.rif4, dest_ip='10.10.0.4')
        self.neighbor4 = self.add_neighbor(self.device, mac_address='00:44:22:33:44:55', handle=self.rif4, dest_ip='10.10.0.4')
        self.add_route(self.device, ip_prefix='10.10.10.1', vrf_handle=self.vrf20, nexthop_handle=self.nhop4)
        try:
            print("Sending packet on port %d, forward to port %d" % (self.devports[0], self.devports[1]))
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt, self.devports[1])
            self.rif0_counter += 1
            self.rif0_byte_counter += len(pkt)
            self.rif1_counter += 1
            self.rif1_byte_counter += len(exp_pkt)

            print("Update VRF on ingress RIF to vrf20")
            self.attribute_set(self.rif0, SWITCH_RIF_ATTR_VRF_HANDLE, self.vrf20)
            print("Sending packet on port %d, forward to port %d" % (self.devports[0], self.devports[5]))
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt2, self.devports[5])
            self.rif0_counter += 1
            self.rif0_byte_counter += len(pkt)

            print("Update VRF on ingress RIF to vrf10")
            self.attribute_set(self.rif0, SWITCH_RIF_ATTR_VRF_HANDLE, self.vrf10)
            print("Sending packet on port %d, forward to port %d" % (self.devports[0], self.devports[1]))
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt, self.devports[1])
            self.rif0_counter += 1
            self.rif0_byte_counter += len(pkt)
            self.rif1_counter += 1
            self.rif1_byte_counter += len(exp_pkt)
        finally:
            self.attribute_set(self.rif0, SWITCH_RIF_ATTR_VRF_HANDLE, self.vrf10)
            # clean temp objects
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

    def RifIPv4DisableTest(self):
        print("RifIPv4DisableTest")
        pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        exp_pkt = simple_tcp_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src='00:77:66:55:44:33',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63)
        try:
            print("Sending packet on port %d, forward" % self.devports[0])
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt, self.devports[1])
            self.rif0_counter += 1
            self.rif0_byte_counter += len(pkt)
            self.rif1_counter += 1
            self.rif1_byte_counter += len(exp_pkt)

            print("Disable IPv4 on ingress RIF")
            self.attribute_set(self.rif0, SWITCH_RIF_ATTR_IPV4_UNICAST, False)
            initial_stats = self.client.object_counters_get(self.port0)
            print("Sending packet on port %d, dropped" % self.devports[0])
            send_packet(self, self.devports[0], pkt)
            verify_no_other_packets(self, timeout=1)
            self.rif0_counter += 1
            self.rif0_byte_counter += len(pkt)
            time.sleep(3)
            final_stats = self.client.object_counters_get(self.port0)
            x = SWITCH_PORT_COUNTER_ID_IF_IN_DISCARDS
            self.assertEqual(final_stats[x].count - initial_stats[x].count, 1)

            print("Enable IPv4 on ingress RIF")
            self.attribute_set(self.rif0, SWITCH_RIF_ATTR_IPV4_UNICAST, True)
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt, self.devports[1])
            self.rif0_counter += 1
            self.rif0_byte_counter += len(pkt)
            self.rif1_counter += 1
            self.rif1_byte_counter += len(exp_pkt)
        finally:
            self.attribute_set(self.rif0, SWITCH_RIF_ATTR_IPV4_UNICAST, True)

    def RifIPv6DisableTest(self):
        print("RifIPv6DisableTest")
        pkt = simple_tcpv6_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
            ipv6_src='2000::1',
            ipv6_hlim=64)
        exp_pkt = simple_tcpv6_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src='00:77:66:55:44:33',
            ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
            ipv6_src='2000::1',
            ipv6_hlim=63)
        try:
            print("Sending packet on port %d, forward" % self.devports[0])
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt, self.devports[1])
            self.rif0_counter += 1
            self.rif0_byte_counter += len(pkt)
            self.rif1_counter += 1
            self.rif1_byte_counter += len(exp_pkt)

            print("Disable IPv6 on ingress RIF")
            self.attribute_set(self.rif0, SWITCH_RIF_ATTR_IPV6_UNICAST, False)
            initial_stats = self.client.object_counters_get(self.port0)
            print("Sending packet on port %d, dropped" % self.devports[0])
            send_packet(self, self.devports[0], pkt)
            verify_no_other_packets(self, timeout=1)
            self.rif0_counter += 1
            self.rif0_byte_counter += len(pkt)
            time.sleep(3)
            final_stats = self.client.object_counters_get(self.port0)
            x = SWITCH_PORT_COUNTER_ID_IF_IN_DISCARDS
            self.assertEqual(final_stats[x].count - initial_stats[x].count, 1)

            print("Enable IPv6 on ingress RIF")
            self.attribute_set(self.rif0, SWITCH_RIF_ATTR_IPV6_UNICAST, True)
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt, self.devports[1])
            self.rif0_counter += 1
            self.rif0_byte_counter += len(pkt)
            self.rif1_counter += 1
            self.rif1_byte_counter += len(exp_pkt)
        finally:
            self.attribute_set(self.rif0, SWITCH_RIF_ATTR_IPV6_UNICAST, True)

    def RifRmacUpdateTest(self):
        print("RifRmacUpdateTest")
        pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        exp_pkt = simple_tcp_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src='00:77:66:55:44:33',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63)
        pkt1 = simple_tcp_packet(
            eth_dst='00:77:66:55:44:44',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        exp_pkt1 = simple_tcp_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src='00:77:66:55:44:44',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63)
        try:
            print("Sending packet on port %d with rmac 00:77:66:55:44:33, forward" % self.devports[0])
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt, self.devports[1])
            self.rif0_counter += 1
            self.rif0_byte_counter += len(pkt)
            self.rif1_counter += 1
            self.rif1_byte_counter += len(exp_pkt)

            self.new_rmac = "00:77:66:55:44:44"
            print("Update ingress rif rmac")
            self.attribute_set(self.rif0, SWITCH_RIF_ATTR_SRC_MAC, self.new_rmac)
            print("Sending packet on port %d with rmac 00:77:66:55:44:33, dropped" % self.devports[0])
            send_packet(self, self.devports[0], pkt)
            verify_no_other_packets(self, timeout=1)
            self.rif0_counter += 1
            self.rif0_byte_counter += len(pkt)

            print("Sending packet on port %d with rmac 00:77:66:55:44:44, forward" % self.devports[0])
            send_packet(self, self.devports[0], pkt1)
            verify_packet(self, exp_pkt, self.devports[1])
            self.rif0_counter += 1
            self.rif0_byte_counter += len(pkt1)
            self.rif1_counter += 1
            self.rif1_byte_counter += len(exp_pkt)

            print("Update egress rif rmac")
            self.attribute_set(self.rif1, SWITCH_RIF_ATTR_SRC_MAC, self.new_rmac)
            print("Sending packet on port %d with rmac 00:77:66:55:44:44, forward" % self.devports[0])
            send_packet(self, self.devports[0], pkt1)
            verify_packet(self, exp_pkt1, self.devports[1])
            self.rif0_counter += 1
            self.rif0_byte_counter += len(pkt1)
            self.rif1_counter += 1
            self.rif1_byte_counter += len(exp_pkt1)

            print("Revert ingress rif rmac")
            self.attribute_set(self.rif0, SWITCH_RIF_ATTR_SRC_MAC, self.rmac)
            print("Sending packet on port %d with rmac 00:77:66:55:44:44, dropped" % self.devports[0])
            send_packet(self, self.devports[0], pkt1)
            verify_no_other_packets(self, timeout=1)
            self.rif0_counter += 1
            self.rif0_byte_counter += len(pkt1)

            print("Sending packet on port %d with rmac 00:77:66:55:44:33, forward" % self.devports[0])
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt1, self.devports[1])
            self.rif0_counter += 1
            self.rif0_byte_counter += len(pkt)
            self.rif1_counter += 1
            self.rif1_byte_counter += len(exp_pkt1)

            print("Revert egress rif rmac")
            self.attribute_set(self.rif1, SWITCH_RIF_ATTR_SRC_MAC, self.rmac)
            print("Sending packet on port %d with rmac 00:77:66:55:44:33, forward" % self.devports[0])
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt, self.devports[1])
            self.rif0_counter += 1
            self.rif0_byte_counter += len(pkt)
            self.rif1_counter += 1
            self.rif1_byte_counter += len(exp_pkt)
        finally:
            self.attribute_set(self.rif0, SWITCH_RIF_ATTR_SRC_MAC, self.rmac)
            self.attribute_set(self.rif1, SWITCH_RIF_ATTR_SRC_MAC, self.rmac)

    def IPv4FibTest(self):
        print("IPv4FibTest")
        pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        exp_pkt = simple_tcp_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src='00:77:66:55:44:33',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63)
        gre_pkt = ipv4_erspan_pkt(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=0,
            ip_ttl=64,
            ip_flags=0x0,
            version=2, #ERSPAN_III
            mirror_id=1,
            sgt_other=4,
            inner_frame=pkt)
        exp_gre_pkt = ipv4_erspan_pkt(
            eth_dst='00:11:22:33:44:55',
            eth_src='00:77:66:55:44:33',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=0,
            ip_ttl=63,
            ip_flags=0x0,
            version=2, #ERSPAN_III
            mirror_id=1,
            sgt_other=4,
            inner_frame=pkt)
        try:
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                1], " (192.168.0.1 -> 10.10.10.1)")
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt, self.devports[1])
            self.rif0_counter += 1
            self.rif0_byte_counter += len(pkt)
            self.rif1_counter += 1
            self.rif1_byte_counter += len(exp_pkt)
            print("Sending gre encapsulated packet port %d" % self.devports[0], " -> port %d" % self.devports[
                1], " (192.168.0.1 -> 10.10.10.1)")
            send_packet(self, self.devports[0], gre_pkt)
            verify_packet(self, exp_gre_pkt, self.devports[1])
            self.rif0_counter += 1
            self.rif0_byte_counter += len(gre_pkt)
            self.rif1_counter += 1
            self.rif1_byte_counter += len(exp_gre_pkt)
        finally:
            pass

    def IPv4FibLocalHostTest(self):
        print("IPv4FibLocalTest")
        if (self.client.is_feature_enable(SWITCH_FEATURE_IPV4_LOCAL_HOST) == 0):
            print("IPv4 local_host_table feature not enabled, skipping")
            return

        rif1 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port5,
                                 vrf_handle=self.vrf10, src_mac=self.rmac)
        nhop1 = self.add_nexthop(self.device, handle=rif1, dest_ip='20.20.0.2')
        neighbor1 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=rif1, dest_ip='20.20.0.2')
        route0 = self.add_route(self.device, ip_prefix='30.30.30.1', vrf_handle=self.vrf10, nexthop_handle=nhop1)

        nhop2 = self.add_nexthop(self.device, handle=rif1, dest_ip='30.30.30.1')
        neighbor2 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=rif1, dest_ip='30.30.30.1')
        route2 = self.add_route(self.device, ip_prefix='30.30.30.1', vrf_handle=self.vrf10, nexthop_handle=nhop2,
                                is_nbr_sourced=True)

        pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='30.30.30.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        exp_pkt = simple_tcp_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src='00:77:66:55:44:33',
            ip_dst='30.30.30.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63)
        try:
            # test if packet can be forwarded using nbr installed route
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                5], " (192.168.0.1 -> 30.30.30.1)")
            send_packet(self, self.devports[0], pkt)
            self.rif0_counter += 1
            self.rif0_byte_counter += len(pkt)
            verify_packet(self, exp_pkt, self.devports[5])

            # delete neighbor and test if packet can be forwarded using /32 route installed not via neighbor
            print("Delete local host route")
            self.cleanlast()
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                5], " (192.168.0.1 -> 30.30.30.1)")
            send_packet(self, self.devports[0], pkt)
            self.rif0_counter += 1
            self.rif0_byte_counter += len(pkt)
            verify_packet(self, exp_pkt, self.devports[5])

        finally:
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            pass


    def IPv4FibJumboTest(self):
        print("IPv4FibJumboTest")
        if self.test_params['target'] == 'hw':
            print("Skipping on HW")
            return
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_RX_MTU, 9220)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_TX_MTU, 9220)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_RX_MTU, 9220)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_TX_MTU, 9220)
        self.attribute_set(self.rif1, SWITCH_RIF_ATTR_MTU, 9220)
        pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            pktlen=6000,
            ip_ttl=64)
        exp_pkt = simple_tcp_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src='00:77:66:55:44:33',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            pktlen=6000,
            ip_ttl=63)
        try:
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                1], " (192.168.0.1 -> 10.10.10.1)")
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt, self.devports[1])
            self.rif0_counter += 1
            self.rif0_byte_counter += len(pkt)
            self.rif1_counter += 1
            self.rif1_byte_counter += len(exp_pkt)
        finally:
            self.attribute_set(self.rif1, SWITCH_RIF_ATTR_MTU, 1514)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_RX_MTU, 1500)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_TX_MTU, 1500)
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_RX_MTU, 1500)
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_TX_MTU, 1500)
            pass

    def IPv4FibLPMTest(self):
        print("IPv4FibLPMTest")
        pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='11.11.11.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        exp_pkt = simple_tcp_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src='00:77:66:55:44:33',
            ip_dst='11.11.11.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63)
        try:
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                1], " (192.168.0.1 -> 11.11.11.0/24) LPM")
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt, self.devports[1])
            self.rif0_counter += 1
            self.rif0_byte_counter += len(pkt)
            self.rif1_counter += 1
            self.rif1_byte_counter += len(exp_pkt)
        finally:
            pass

    def IPv4FibModifyTest(self):
        print("IPv4FibModifyTest")
        print("Modify nexhop of route0 from nhop1 to nhop2")

        pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        exp_pkt = simple_tcp_packet(
            eth_dst='00:22:22:33:44:55',
            eth_src='00:77:66:55:44:33',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63)
        lag_exp_pkt = simple_tcp_packet(
            eth_dst='00:33:22:33:44:55',
            eth_src='00:77:66:55:44:33',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63)
        cpu_pkt = simple_cpu_packet(
                packet_type=0,
                ingress_port=swport_to_devport(self, self.devports[0]),
                ingress_ifindex=(self.port0 & 0xFFFF),
                reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_MYIP,
                ingress_bd=0x0,
                inner_pkt=pkt)
        cpu_pkt = cpu_packet_mask_ingress_bd(cpu_pkt)
        try:
            self.attribute_set(self.route0, SWITCH_ROUTE_ATTR_NEXTHOP_HANDLE, self.nhop2)
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 10.10.10.1)")
            send_packet(self, self.devports[0], pkt)
            verify_packets(self, exp_pkt, [self.devports[2]])
            self.rif0_counter += 1
            self.rif0_byte_counter += len(pkt)
            self.rif2_counter += 1
            self.rif2_byte_counter += len(exp_pkt)
            self.add_hostif_trap(self.device, type=SWITCH_HOSTIF_TRAP_ATTR_TYPE_MYIP,
                                 packet_action=SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_REDIRECT_TO_CPU)
            print("Set route to myip")
            self.attribute_set(self.route0, SWITCH_ROUTE_ATTR_IS_HOST_MYIP, True)
            send_packet(self, self.devports[0], pkt)
            verify_packets(self, cpu_pkt, [self.cpu_port])
            self.rif0_counter += 1
            self.rif0_byte_counter += len(pkt)
            print("Set route back to port %d" % (self.devports[2]))
            self.attribute_set(self.route0, SWITCH_ROUTE_ATTR_IS_HOST_MYIP, False)
            send_packet(self, self.devports[0], pkt)
            verify_packets(self, exp_pkt, [self.devports[2]])
            self.rif0_counter += 1
            self.rif0_byte_counter += len(pkt)
            self.rif2_counter += 1
            self.rif2_byte_counter += len(exp_pkt)
            self.cleanlast()
            print("Set route to drop via nexthop")
            self.attribute_set(self.route0, SWITCH_ROUTE_ATTR_NEXTHOP_HANDLE, self.nhop_drop)
            send_packet(self, self.devports[0], pkt)
            verify_no_other_packets(self, timeout=2)
            self.rif0_counter += 1
            self.rif0_byte_counter += len(pkt)
            print("Set route back to port %d" % (self.devports[2]))
            self.attribute_set(self.route0, SWITCH_ROUTE_ATTR_NEXTHOP_HANDLE, self.nhop2)
            send_packet(self, self.devports[0], pkt)
            verify_packets(self, exp_pkt, [self.devports[2]])
            self.rif0_counter += 1
            self.rif0_byte_counter += len(pkt)
            self.rif2_counter += 1
            self.rif2_byte_counter += len(exp_pkt)
            print("Set route to drop via packet action")
            self.attribute_set(self.route0, SWITCH_ROUTE_ATTR_PACKET_ACTION, SWITCH_ROUTE_ATTR_PACKET_ACTION_DROP)
            send_packet(self, self.devports[0], pkt)
            verify_no_other_packets(self, timeout=2)
            self.rif0_counter += 1
            self.rif0_byte_counter += len(pkt)
            print("Set route to back to forward via packet action")
            self.attribute_set(self.route0, SWITCH_ROUTE_ATTR_PACKET_ACTION, SWITCH_ROUTE_ATTR_PACKET_ACTION_FORWARD)
            send_packet(self, self.devports[0], pkt)
            verify_packets(self, exp_pkt, [self.devports[2]])
            self.rif0_counter += 1
            self.rif0_byte_counter += len(pkt)
            self.rif2_counter += 1
            self.rif2_byte_counter += len(exp_pkt)
            print("Set route to lag")
            self.attribute_set(self.route0, SWITCH_ROUTE_ATTR_NEXTHOP_HANDLE, self.nhop3)
            send_packet(self, self.devports[0], pkt)
            verify_any_packet_any_port(self, [lag_exp_pkt] * 2, [self.devports[3], self.devports[4]])
            self.rif0_counter += 1
            self.rif0_byte_counter += len(pkt)
            self.rif3_counter += 1
            self.rif3_byte_counter += len(lag_exp_pkt)
            print("Set route back to port %d" % (self.devports[2]))
            self.attribute_set(self.route0, SWITCH_ROUTE_ATTR_NEXTHOP_HANDLE, self.nhop2)
            send_packet(self, self.devports[0], pkt)
            verify_packets(self, exp_pkt, [self.devports[2]])
            self.rif0_counter += 1
            self.rif0_byte_counter += len(pkt)
            self.rif2_counter += 1
            self.rif2_byte_counter += len(exp_pkt)
        finally:
            self.attribute_set(self.route0, SWITCH_ROUTE_ATTR_NEXTHOP_HANDLE, self.nhop1)
            pass

    def IPv4FibResolutionTest(self):
        print("IPv4FibResolutionTest")
        pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='55.55.55.100',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        e_pkt = simple_tcp_packet(
            eth_dst='00:33:22:33:44:55',    # nhop3
            eth_src='00:77:66:55:44:33',
            ip_dst='55.55.55.100',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63)
        lpm16_pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='55.55.55.100',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        lpm16_e_pkt = simple_tcp_packet(
            eth_dst='00:11:22:33:44:55',    # nhop1
            eth_src='00:77:66:55:44:33',
            ip_dst='55.55.55.100',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63)
        lpm25_pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='55.55.55.100',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        lpm25_e_pkt = simple_tcp_packet(
            eth_dst='00:22:22:33:44:55',    # nhop2
            eth_src='00:77:66:55:44:33',
            ip_dst='55.55.55.100',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63)

        try:
            print(" Route exists with prefix <= 16 i.e. LPM entry 55.55.0.0/16")
            print (" TX packet port %d" % self.devports[0], " -> port %d" % self.devports[
                1], " [192.168.0.1 -> 55.55.55.100], match LPM entry 55.55.0.0/16")
            send_packet(self, self.devports[0], lpm16_pkt)
            verify_packet(self, lpm16_e_pkt, self.devports[1])
            self.rif0_counter += 1
            self.rif0_byte_counter += len(lpm16_pkt)
            self.rif1_counter += 1
            self.rif1_byte_counter += len(lpm16_e_pkt)

            # Program route entry with prefix <= 25 ---> nhop 2
            print(" Program route with prefix <= 25 i.e. LPM entry 55.55.55.0/25")
            self.route_25 = self.add_route(self.device, ip_prefix='55.55.55.0/25', vrf_handle=self.vrf10, nexthop_handle=self.nhop2)

            print (" TX packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " [192.168.0.1 -> 55.55.55.100], match LPM entry 55.55.55.0/25")
            send_packet(self, self.devports[0], lpm25_pkt)
            verify_packet(self, lpm25_e_pkt, self.devports[2])
            self.rif0_counter += 1
            self.rif0_byte_counter += len(lpm25_pkt)
            self.rif2_counter += 1
            self.rif2_byte_counter += len(lpm25_e_pkt)

            # Program route with prefix == 32 ---> nhop 3
            print(" Program route with prefix == 32 i.e. /32 entry 55.55.55.100")
            self.route_32 = self.add_route(self.device, ip_prefix='55.55.55.100', vrf_handle=self.vrf10, nexthop_handle=self.nhop3)

            print (" TX packet port %d" % self.devports[0], " -> lag [port %d, port %d]" % (self.devports[
                3], self.devports[4]), " [192.168.0.1 -> 55.55.55.100], match /32 entry 55.55.55.100")
            send_packet(self, self.devports[0], pkt)
            verify_any_packet_any_port(self, [e_pkt] * 2, [self.devports[3], self.devports[4]])
            self.rif0_counter += 1
            self.rif0_byte_counter += len(pkt)
            self.rif3_counter += 1
            self.rif3_byte_counter += len(e_pkt)

            print(" Remove route with prefix == 32 i.e. /32 entry 55.55.55.100")
            self.cleanlast()    # Remove route with prefix == 32

            print (" TX packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " [192.168.0.1 -> 55.55.55.100], match LPM entry 55.55.55.0/25")
            send_packet(self, self.devports[0], lpm25_pkt)
            verify_packet(self, lpm25_e_pkt, self.devports[2])
            self.rif0_counter += 1
            self.rif0_byte_counter += len(lpm25_pkt)
            self.rif2_counter += 1
            self.rif2_byte_counter += len(lpm25_e_pkt)

            print(" Remove route with prefix <= 25 i.e. LPM entry 55.55.55.0/25")
            self.cleanlast()    # Remove route with prefix <= 25

            print (" TX packet port %d" % self.devports[0], " -> port %d" % self.devports[
                1], " [192.168.0.1 -> 55.55.55.100], match LPM entry 55.55.0.0/16")
            send_packet(self, self.devports[0], lpm16_pkt)
            verify_packet(self, lpm16_e_pkt, self.devports[1])
            self.rif0_counter += 1
            self.rif0_byte_counter += len(lpm16_pkt)
            self.rif1_counter += 1
            self.rif1_byte_counter += len(lpm16_e_pkt)
        finally:
            pass

    def IPv4MtuTest(self):
        print("IPv4MtuTest")
        # set MTU to 200 for port 1
        max_pkts = 30
        min_pkt_size = 200
        mtu_size = 1200
        self.attribute_set(self.rif1, SWITCH_RIF_ATTR_MTU, mtu_size)
        self.attribute_set(self.rif3, SWITCH_RIF_ATTR_MTU, mtu_size)
        try:
            for i in range(0, max_pkts):
              j = random.randint(min_pkt_size, mtu_size)
              print("Max MTU is %d, send pkt size %d, send to port/lag"%(mtu_size,j))
              pkt = simple_tcp_packet(
                  eth_dst='00:77:66:55:44:33',
                  eth_src='00:22:22:22:22:22',
                  ip_dst='10.10.10.1',
                  ip_src='192.168.0.1',
                  ip_id=105,
                  ip_ttl=64,
                  pktlen=j+14-1)
              exp_pkt = simple_tcp_packet(
                  eth_dst='00:11:22:33:44:55',
                  eth_src='00:77:66:55:44:33',
                  ip_dst='10.10.10.1',
                  ip_src='192.168.0.1',
                  ip_id=105,
                  ip_ttl=63,
                  pktlen=j+14-1)
              print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                    1], " (192.168.0.1 -> 10.10.10.1)")
              send_packet(self, self.devports[0], pkt)
              verify_packets(self, exp_pkt, [self.devports[1]])
              self.rif0_counter += 1
              self.rif0_byte_counter += len(pkt)
              self.rif1_counter += 1
              self.rif1_byte_counter += len(exp_pkt)
              pkt['IP'].dst = '12.10.10.1'
              exp_pkt['IP'].dst = '12.10.10.1'
              exp_pkt['Ethernet'].dst = '00:33:22:33:44:55'
              print("Sending packet port %d" % self.devports[0], " -> lag 0 (192.168.0.1 -> 12.10.10.1)")
              send_packet(self, self.devports[0], pkt)
              verify_any_packet_any_port(self, [exp_pkt],
                        [self.devports[3], self.devports[4]])
              self.rif0_counter += 1
              self.rif0_byte_counter += len(pkt)
              self.rif3_counter += 1
              self.rif3_byte_counter += len(exp_pkt)

            print("Max MTU is %d, send pkt size %d, send to port/lag"%(mtu_size,mtu_size))
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
              eth_src='00:22:22:22:22:22',
              ip_dst='10.10.10.1',
              ip_src='192.168.0.1',
              ip_id=105,
              ip_ttl=64,
              pktlen=mtu_size+14)
            exp_pkt = simple_tcp_packet(
                eth_dst='00:11:22:33:44:55',
              eth_src='00:77:66:55:44:33',
              ip_dst='10.10.10.1',
              ip_src='192.168.0.1',
              ip_id=105,
              ip_ttl=63,
              pktlen=mtu_size+14)
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                  1], " (192.168.0.1 -> 10.10.10.1)")
            send_packet(self, self.devports[0], pkt)
            verify_packets(self, exp_pkt, [self.devports[1]])
            self.rif0_counter += 1
            self.rif0_byte_counter += len(pkt)
            self.rif1_counter += 1
            self.rif1_byte_counter += len(exp_pkt)
            pkt['IP'].dst = '12.10.10.1'
            exp_pkt['IP'].dst = '12.10.10.1'
            exp_pkt['Ethernet'].dst = '00:33:22:33:44:55'
            print("Sending packet port %d" % self.devports[0], " -> lag 0 (192.168.0.1 -> 12.10.10.1)")
            send_packet(self, self.devports[0], pkt)
            verify_any_packet_any_port(self, [exp_pkt],
                      [self.devports[3], self.devports[4]])
            self.rif0_counter += 1
            self.rif0_byte_counter += len(pkt)
            self.rif3_counter += 1
            self.rif3_byte_counter += len(exp_pkt)

            print("Max MTU is %d, send pkt size %d, send to cpu"%(mtu_size, mtu_size+1))
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
              eth_src='00:22:22:22:22:22',
              ip_dst='10.10.10.1',
              ip_src='192.168.0.1',
              ip_id=105,
              ip_ttl=64,
              pktlen=mtu_size+1+14)
            exp_pkt = simple_tcp_packet(
                eth_dst='00:11:22:33:44:55',
              eth_src='00:77:66:55:44:33',
              ip_dst='10.10.10.1',
              ip_src='192.168.0.1',
              ip_id=105,
              ip_ttl=63,
              pktlen=mtu_size+1+14)
            cpu_pkt = simple_cpu_packet(
                  ingress_port=0,
                  packet_type=0,
                  ingress_ifindex=0,
                  reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_L3_MTU_ERROR,
                  ingress_bd=0x02,
                  inner_pkt=exp_pkt)
            cpu_pkt_len = len(cpu_pkt)
            cpu_pkt = cpu_packet_mask_ingress_bd(cpu_pkt)
            print("Sending packet port %d" % self.devports[0], " -> cpu (192.168.0.1 -> 10.10.10.1)")
            if (self.test_params['target'] != 'hw' and
                self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_COPP) == 1):
                send_packet(self, self.devports[0], pkt)
                verify_packets(self, cpu_pkt, [self.cpu_port])
                self.rif0_counter += 1
                self.rif0_byte_counter += len(pkt)
                self.rif1_counter += 1
                self.rif1_byte_counter += len(exp_pkt)

            pkt['IP'].dst = '12.10.10.1'
            exp_pkt['IP'].dst = '12.10.10.1'
            exp_pkt['Ethernet'].dst = '00:33:22:33:44:55'
            cpu_pkt = simple_cpu_packet(
                  ingress_port=0,
                  packet_type=0,
                  ingress_ifindex=0,
                  reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_L3_MTU_ERROR,
                  ingress_bd=0x02,
                  inner_pkt=exp_pkt)
            cpu_pkt = cpu_packet_mask_ingress_bd(cpu_pkt)
            print("Sending packet port %d" % self.devports[0], " -> cpu (192.168.0.1 -> 12.10.10.1)")
            if (self.test_params['target'] != 'hw' and
                self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_COPP) == 1):
                send_packet(self, self.devports[0], pkt)
                verify_packets(self, cpu_pkt, [self.cpu_port])
                self.rif0_counter += 1
                self.rif0_byte_counter += len(pkt)
                self.rif3_counter += 1
                self.rif3_byte_counter += len(exp_pkt)
        finally:
            self.attribute_set(self.rif3, SWITCH_RIF_ATTR_MTU, 1514)
            self.attribute_set(self.rif1, SWITCH_RIF_ATTR_MTU, 1514)
            pass

    def IPv6FibTest(self):
        print("IPv6FibTest")
        pkt = simple_tcpv6_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
            ipv6_src='2000::1',
            ipv6_hlim=64)
        exp_pkt = simple_tcpv6_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src='00:77:66:55:44:33',
            ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
            ipv6_src='2000::1',
            ipv6_hlim=63)

        try:
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                1], " (2000::1 -> 1234:5678:9abc:def0:4422:1133:5577:99aa)")
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt, self.devports[1])
            self.rif0_counter += 1
            self.rif0_byte_counter += len(pkt)
            self.rif1_counter += 1
            self.rif1_byte_counter += len(exp_pkt)
        finally:
            pass

    def IPv6FibLPMTest(self):
        print("IPv6FibLPMTest")
        pkt = simple_tcpv6_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ipv6_dst='4000::1',
            ipv6_src='2000::1',
            ipv6_hlim=64)
        exp_pkt = simple_tcpv6_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src='00:77:66:55:44:33',
            ipv6_dst='4000::1',
            ipv6_src='2000::1',
            ipv6_hlim=63)

        try:
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                1], " (2000::1 -> 4000::1) LPM entry 4000::0/65")
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt, self.devports[1])
            self.rif0_counter += 1
            self.rif0_byte_counter += len(pkt)
            self.rif1_counter += 1
            self.rif1_byte_counter += len(exp_pkt)
        finally:
            pass

    def IPv6FibLPM64Test(self):
        print("IPv6FibLPM64Test")
        pkt = simple_tcpv6_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ipv6_dst='5000::5',
            ipv6_src='2000::1',
            ipv6_hlim=64)
        exp_pkt = simple_tcpv6_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src='00:77:66:55:44:33',
            ipv6_dst='5000::5',
            ipv6_src='2000::1',
            ipv6_hlim=63)
        pkt5 = simple_tcpv6_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ipv6_dst='8500::5555',
            ipv6_src='2000::1',
            ipv6_hlim=64)
        exp_pkt5 = simple_tcpv6_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src='00:77:66:55:44:33',
            ipv6_dst='8500::5555',
            ipv6_src='2000::1',
            ipv6_hlim=63)

        try:
            print("TX packet port %d" % self.devports[0], " -> port %d" % self.devports[
                1], " [2000::1 -> 5000::5] LPM-64 entry 5000::0/64")
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt, self.devports[1])
            self.rif0_counter += 1
            self.rif0_byte_counter += len(pkt)
            self.rif1_counter += 1
            self.rif1_byte_counter += len(exp_pkt)

            print("TX packet port %d" % self.devports[0], " -> port %d" % self.devports[
                1], " [2000::1 -> 8500::5555] LPM-64 entry 8500::0/10")
            send_packet(self, self.devports[0], pkt5)
            verify_packet(self, exp_pkt5, self.devports[1])
            self.rif0_counter += 1
            self.rif0_byte_counter += len(pkt5)
            self.rif1_counter += 1
            self.rif1_byte_counter += len(exp_pkt5)
        finally:
            pass

    def IPv6FibModifyTest(self):
        print("IPv6FibModifyTest")
        print("Modify nexhop of route1 from nhop1 to nhop2")

        pkt = simple_tcpv6_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
            ipv6_src='2000::1',
            ipv6_hlim=64)
        exp_pkt = simple_tcpv6_packet(
            eth_dst='00:22:22:33:44:55',
            eth_src='00:77:66:55:44:33',
            ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
            ipv6_src='2000::1',
            ipv6_hlim=63)
        lag_exp_pkt = simple_tcpv6_packet(
            eth_dst='00:33:22:33:44:55',
            eth_src='00:77:66:55:44:33',
            ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
            ipv6_src='2000::1',
            ipv6_hlim=63)

        try:
            self.attribute_set(self.route1, SWITCH_ROUTE_ATTR_NEXTHOP_HANDLE, self.nhop2)
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (2000::1 -> 1234:5678:9abc:def0:4422:1133:5577:99aa)")
            send_packet(self, self.devports[0], pkt)
            verify_packets(self, exp_pkt, [self.devports[2]])
            self.rif0_counter += 1
            self.rif0_byte_counter += len(pkt)
            self.rif2_counter += 1
            self.rif2_byte_counter += len(exp_pkt)
            print("Set route to drop")
            self.attribute_set(self.route1, SWITCH_ROUTE_ATTR_NEXTHOP_HANDLE, self.nhop_drop)
            send_packet(self, self.devports[0], pkt)
            verify_no_other_packets(self, timeout=2)
            self.rif0_counter += 1
            self.rif0_byte_counter += len(pkt)
            print("Set route back to port %d" % (self.devports[2]))
            self.attribute_set(self.route1, SWITCH_ROUTE_ATTR_NEXTHOP_HANDLE, self.nhop2)
            send_packet(self, self.devports[0], pkt)
            verify_packets(self, exp_pkt, [self.devports[2]])
            self.rif0_counter += 1
            self.rif0_byte_counter += len(pkt)
            self.rif2_counter += 1
            self.rif2_byte_counter += len(exp_pkt)
            print("Set route to lag")
            self.attribute_set(self.route1, SWITCH_ROUTE_ATTR_NEXTHOP_HANDLE, self.nhop3)
            send_packet(self, self.devports[0], pkt)
            verify_any_packet_any_port(self, [lag_exp_pkt] * 2, [self.devports[3], self.devports[4]])
            self.rif0_counter += 1
            self.rif0_byte_counter += len(pkt)
            self.rif3_counter += 1
            self.rif3_byte_counter += len(lag_exp_pkt)
            print("Set route back to port %d" % (self.devports[2]))
            self.attribute_set(self.route1, SWITCH_ROUTE_ATTR_NEXTHOP_HANDLE, self.nhop2)
            send_packet(self, self.devports[0], pkt)
            verify_packets(self, exp_pkt, [self.devports[2]])
            self.rif0_counter += 1
            self.rif0_byte_counter += len(pkt)
            self.rif2_counter += 1
            self.rif2_byte_counter += len(exp_pkt)
        finally:
            self.attribute_set(self.route1, SWITCH_ROUTE_ATTR_NEXTHOP_HANDLE, self.nhop1)
            pass

    def IPv6FibResolutionTest(self):
        print("IPv6FibResolutionTest")
        pkt = simple_tcpv6_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ipv6_dst='AB15::5555',
            ipv6_src='2000::1',
            ipv6_hlim=64)
        e_pkt = simple_tcpv6_packet(
            eth_dst='00:33:22:33:44:55',    # nhop3
            eth_src='00:77:66:55:44:33',
            ipv6_dst='AB15::5555',
            ipv6_src='2000::1',
            ipv6_hlim=63)
        lpm64_pkt = simple_tcpv6_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ipv6_dst='AB15::5555',
            ipv6_src='2000::1',
            ipv6_hlim=64)
        lpm64_e_pkt = simple_tcpv6_packet(
            eth_dst='00:11:22:33:44:55',    # nhop1
            eth_src='00:77:66:55:44:33',
            ipv6_dst='AB15::5555',
            ipv6_src='2000::1',
            ipv6_hlim=63)
        lpm_pkt = simple_tcpv6_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ipv6_dst='AB15::5555',
            ipv6_src='2000::1',
            ipv6_hlim=64)
        lpm_e_pkt = simple_tcpv6_packet(
            eth_dst='00:22:22:33:44:55',    #nhop2
            eth_src='00:77:66:55:44:33',
            ipv6_dst='AB15::5555',
            ipv6_src='2000::1',
            ipv6_hlim=63)

        try:
            print(" Route exists with prefix <= 64 i.e. LPM-64 entry AB15::0/64")
            print (" TX packet port %d" % self.devports[0], " -> port %d" % self.devports[
                1], " [2000::1 -> AB15::5555], match LPM-64 entry AB15::0/64")
            send_packet(self, self.devports[0], lpm64_pkt)
            verify_packet(self, lpm64_e_pkt, self.devports[1])
            self.rif0_counter += 1
            self.rif0_byte_counter += len(lpm64_pkt)
            self.rif1_counter += 1
            self.rif1_byte_counter += len(lpm64_e_pkt)

            # Program route with prefix > 64 ---> nhop 2
            print(" Program route with prefix > 64 i.e. LPM entry AB15::0/65")
            self.route_g_64 = self.add_route(self.device, ip_prefix='AB15::0/65', vrf_handle=self.vrf10, nexthop_handle=self.nhop2)

            print (" TX packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " [2000::1 -> AB15::5555], match LPM entry AB15::0/65")
            send_packet(self, self.devports[0], lpm_pkt)
            verify_packet(self, lpm_e_pkt, self.devports[2])
            self.rif0_counter += 1
            self.rif0_byte_counter += len(lpm_pkt)
            self.rif2_counter += 1
            self.rif2_byte_counter += len(lpm_e_pkt)

            # Program route with prefix == 128 ---> nhop 3
            print(" Program route with prefix == 128 i.e. /128 entry AB15::5555")
            self.route_128 = self.add_route(self.device, ip_prefix='AB15::5555', vrf_handle=self.vrf10, nexthop_handle=self.nhop3)

            print (" TX packet port %d" % self.devports[0], " -> lag [port %d, port %d]" % (self.devports[
                3], self.devports[4]), " [2000::1 -> AB15::5555], match /128 entry AB15::5555")
            send_packet(self, self.devports[0], pkt)
            verify_any_packet_any_port(self, [e_pkt] * 2, [self.devports[3], self.devports[4]])
            self.rif0_counter += 1
            self.rif0_byte_counter += len(pkt)
            self.rif3_counter += 1
            self.rif3_byte_counter += len(e_pkt)

            print(" Remove route with prefix == 128 i.e. /128 entry AB15::5555")
            self.cleanlast()    # Remove route with prefix == 128

            print (" TX packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " [2000::1 -> AB15::5555], match LPM entry AB15::0/65")
            send_packet(self, self.devports[0], lpm_pkt)
            verify_packet(self, lpm_e_pkt, self.devports[2])
            self.rif0_counter += 1
            self.rif0_byte_counter += len(lpm_pkt)
            self.rif2_counter += 1
            self.rif2_byte_counter += len(lpm_e_pkt)

            print(" Remove route with prefix > 64 i.e. LPM entry AB15::0/65")
            self.cleanlast()    # Remove route with prefix == 128

            print (" TX packet port %d" % self.devports[0], " -> port %d" % self.devports[
                1], " [2000::1 -> AB15::5555], match LPM-64 entry AB15::0/64")
            send_packet(self, self.devports[0], lpm64_pkt)
            verify_packet(self, lpm64_e_pkt, self.devports[1])
            self.rif0_counter += 1
            self.rif0_byte_counter += len(lpm64_pkt)
            self.rif1_counter += 1
            self.rif1_byte_counter += len(lpm64_e_pkt)

        finally:
            pass

    def IPv4FibLagTest(self):
        print("IPv4FibLagTest")
        pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='12.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        exp_pkt = simple_tcp_packet(
            eth_dst='00:33:22:33:44:55',
            eth_src='00:77:66:55:44:33',
            ip_dst='12.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63)
        try:
            print("Sending packet port 0 -> lag 0 (192.168.0.1 -> 12.10.10.1)")
            send_packet(self, self.devports[0], pkt)
            verify_any_packet_any_port(self, [exp_pkt] * 2, [self.devports[3], self.devports[4]])
            self.rif0_counter += 1
            self.rif0_byte_counter += len(pkt)
            self.rif3_counter += 1
            self.rif3_byte_counter += len(exp_pkt)
        finally:
            pass

    def IPv6FibLagTest(self):
        print("IPv6FibLagTest")
        pkt = simple_tcpv6_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ipv6_dst='1234:5678:9abc:def0:1122:3344:5566:7788',
            ipv6_src='2000::1',
            ipv6_hlim=64)
        exp_pkt = simple_tcpv6_packet(
            eth_dst='00:33:22:33:44:55',
            eth_src='00:77:66:55:44:33',
            ipv6_dst='1234:5678:9abc:def0:1122:3344:5566:7788',
            ipv6_src='2000::1',
            ipv6_hlim=63)

        try:
            print("Sending packet port 0 -> lag 0 (2000::1 -> 1234:5678:9abc:def0:1122:3344:5566:7788)")
            send_packet(self, self.devports[0], pkt)
            verify_any_packet_any_port(self, [exp_pkt] * 2, [self.devports[3], self.devports[4]])
            self.rif0_counter += 1
            self.rif0_byte_counter += len(pkt)
            self.rif3_counter += 1
            self.rif3_byte_counter += len(exp_pkt)
        finally:
            pass

    def IPv4FibGleanTest(self):
        print("IPv4FibGleanTest")
        pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='13.20.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        cpu_pkt = simple_cpu_packet(
                packet_type=0,
                ingress_port=swport_to_devport(self, self.devports[0]),
                ingress_ifindex=(self.port0 & 0xFFFF),
                reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_MAX + SWITCH_HOSTIF_TRAP_ATTR_INTERNAL_TRAP_GLEAN,
                ingress_bd=0x0,
                inner_pkt=pkt)
        cpu_pkt = cpu_packet_mask_ingress_bd(cpu_pkt)
        try:
            if self.test_params['target'] != 'hw':
                print("Sending packet port %d" % self.devports[0], " -> port %d" % self.cpu_port, " (192.168.0.1 -> 10.10.10.1)")
                send_packet(self, self.devports[0], pkt)
                verify_packets(self, cpu_pkt, [self.cpu_port])
                self.rif0_counter += 1
                self.rif0_byte_counter += len(pkt)
        finally:
            pass

    def IPv6MtuTest(self):
        print("IPv6MtuTest")
        # set MTU to 200 for port 1
        self.attribute_set(self.rif1, SWITCH_RIF_ATTR_MTU, 200)
        self.attribute_set(self.rif3, SWITCH_RIF_ATTR_MTU, 200)
        try:
            print("Max MTU is 200, send pkt size 199, send to port/lag")
            pkt = simple_tcpv6_packet(
                eth_dst='00:77:66:55:44:33',
              eth_src='00:22:22:22:22:22',
              ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
              ipv6_src='2000::1',
              ipv6_hlim=64,
              pktlen=199+14+40)
            exp_pkt = simple_tcpv6_packet(
                eth_dst='00:11:22:33:44:55',
              eth_src='00:77:66:55:44:33',
              ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
              ipv6_src='2000::1',
              ipv6_hlim=63,
              pktlen=199+14+40)
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                  1], " (2000::1 -> 1234:5678:9abc:def0:4422:1133:5577:99aa)")
            send_packet(self, self.devports[0], pkt)
            verify_packets(self, exp_pkt, [self.devports[1]])
            self.rif0_counter += 1
            self.rif0_byte_counter += len(pkt)
            self.rif1_counter += 1
            self.rif1_byte_counter += len(exp_pkt)
            pkt['IPv6'].dst = '1234:5678:9abc:def0:1122:3344:5566:7788'
            exp_pkt['IPv6'].dst = '1234:5678:9abc:def0:1122:3344:5566:7788'
            exp_pkt['Ethernet'].dst = '00:33:22:33:44:55'
            print("Sending packet port %d" % self.devports[0], " -> lag 0 (2000::1 -> 1234:5678:9abc:def0:1122:3344:5566:7788)")
            send_packet(self, self.devports[0], pkt)
            verify_any_packet_any_port(self, [exp_pkt],
                      [self.devports[3], self.devports[4]])
            self.rif0_counter += 1
            self.rif0_byte_counter += len(pkt)
            self.rif3_counter += 1
            self.rif3_byte_counter += len(exp_pkt)
            print("Max MTU is 200, send pkt size 200, send to port/lag")
            pkt = simple_tcpv6_packet(
                eth_dst='00:77:66:55:44:33',
              eth_src='00:22:22:22:22:22',
              ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
              ipv6_src='2000::1',
              ipv6_hlim=64,
              pktlen=200+14+40)
            exp_pkt = simple_tcpv6_packet(
                eth_dst='00:11:22:33:44:55',
              eth_src='00:77:66:55:44:33',
              ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
              ipv6_src='2000::1',
              ipv6_hlim=63,
              pktlen=200+14+40)
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                  1], " (2000::1 -> 1234:5678:9abc:def0:4422:1133:5577:99aa)")
            send_packet(self, self.devports[0], pkt)
            verify_packets(self, exp_pkt, [self.devports[1]])
            self.rif0_counter += 1
            self.rif0_byte_counter += len(pkt)
            self.rif1_counter += 1
            self.rif1_byte_counter += len(exp_pkt)
            pkt['IPv6'].dst = '1234:5678:9abc:def0:1122:3344:5566:7788'
            exp_pkt['IPv6'].dst = '1234:5678:9abc:def0:1122:3344:5566:7788'
            exp_pkt['Ethernet'].dst = '00:33:22:33:44:55'
            print("Sending packet port %d" % self.devports[0], " -> lag 0 (2000::1 -> 1234:5678:9abc:def0:1122:3344:5566:7788)")
            send_packet(self, self.devports[0], pkt)
            verify_any_packet_any_port(self, [exp_pkt],
                      [self.devports[3], self.devports[4]])
            self.rif0_counter += 1
            self.rif0_byte_counter += len(pkt)
            self.rif3_counter += 1
            self.rif3_byte_counter += len(exp_pkt)

            print("Max MTU is 200, send pkt size 201, send to cpu")
            pkt = simple_tcpv6_packet(
                eth_dst='00:77:66:55:44:33',
              eth_src='00:22:22:22:22:22',
              ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
              ipv6_src='2000::1',
              ipv6_hlim=64,
              pktlen=201+14+40)
            exp_pkt = simple_tcpv6_packet(
                eth_dst='00:11:22:33:44:55',
              eth_src='00:77:66:55:44:33',
              ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
              ipv6_src='2000::1',
              ipv6_hlim=63,
              pktlen=201+14+40)
            cpu_pkt = simple_cpu_packet(
                  packet_type=0,
                  ingress_port=0,
                  ingress_ifindex=0,
                  reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_L3_MTU_ERROR,
                  ingress_bd=0x02,
                  inner_pkt=exp_pkt)
            cpu_pkt = cpu_packet_mask_ingress_bd(cpu_pkt)
            print("Sending packet port %d" % self.devports[0], " -> cpu (2000::1 -> 1234:5678:9abc:def0:4422:1133:5577:99aa)")
            if (self.test_params['target'] != 'hw' and
                self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_COPP) == 1):
                send_packet(self, self.devports[0], pkt)
                verify_packets(self, cpu_pkt, [self.cpu_port])
                self.rif0_counter += 1
                self.rif0_byte_counter += len(pkt)
                self.rif1_counter += 1
                self.rif1_byte_counter += len(exp_pkt)

            pkt['IPv6'].dst = '1234:5678:9abc:def0:1122:3344:5566:7788'
            exp_pkt['IPv6'].dst = '1234:5678:9abc:def0:1122:3344:5566:7788'
            exp_pkt['Ethernet'].dst = '00:33:22:33:44:55'
            cpu_pkt = simple_cpu_packet(
                  packet_type=0,
                  ingress_port=0,
                  ingress_ifindex=0,
                  reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_L3_MTU_ERROR,
                  ingress_bd=0x02,
                  inner_pkt=exp_pkt)
            cpu_pkt = cpu_packet_mask_ingress_bd(cpu_pkt)
            print("Sending packet port %d" % self.devports[0], " -> cpu (2000::1 -> 1234:5678:9abc:def0:1122:3344:5566:7788)")
            if (self.test_params['target'] != 'hw' and
                self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_COPP) == 1):
                send_packet(self, self.devports[0], pkt)
                verify_packets(self, cpu_pkt, [self.cpu_port])
                self.rif0_counter += 1
                self.rif0_byte_counter += len(pkt)
                self.rif3_counter += 1
                self.rif3_byte_counter += len(exp_pkt)

        finally:
            self.attribute_set(self.rif3, SWITCH_RIF_ATTR_MTU, 1514)
            self.attribute_set(self.rif1, SWITCH_RIF_ATTR_MTU, 1514)
            pass

    def IPv4FibIngressVlanZeroTest(self):
        print("IPv4FibIngressVlanZeroTest")
        return
        pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            dl_vlan_enable=True,
            vlan_vid=0,
            ip_ttl=64,
            pktlen=104)
        exp_pkt = simple_tcp_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src='00:77:66:55:44:33',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63,
            pktlen=100)
        try:
            print("Sending vlan 0 packet port %d" % self.devports[0], " -> port %d" % self.devports[
                1], " (192.168.0.1 -> 10.10.10.1)")
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt, self.devports[1])
            self.rif0_counter += 1
            self.rif0_byte_counter += len(pkt)
            self.rif1_counter += 1
            self.rif1_byte_counter += len(exp_pkt)
        finally:
            pass

    def L3RifStatsTest(self):
        print("L3RifStatsTest")
        time.sleep(4)
        self.rif0_end = self.client.object_counters_get(self.rif0)
        self.rif1_end = self.client.object_counters_get(self.rif1)
        self.rif2_end = self.client.object_counters_get(self.rif2)
        self.rif3_end = self.client.object_counters_get(self.rif3)
        # 0 is SWITCH_RIF_COUNTER_ID_IN_UCAST_PKTS
        # 6 is SWITCH_RIF_COUNTER_ID_OUT_UCAST_PKTS
        # 3 is SWITCH_RIF_COUNTER_ID_IN_UCAST_BYTES
        # 9 is SWITCH_RIF_COUNTER_ID_OUT_UCAST_BYTES
        print("IN_UCAST_PKTS  RIF0 counted: %d expected: %d" % (self.rif0_end[0].count, self.rif0_counter))
        print("OUT_UCAST_PKTS RIF1 counted: %d expected: %d" % (self.rif1_end[6].count, self.rif1_counter))
        print("OUT_UCAST_PKTS RIF2 counted: %d expected: %d" % (self.rif2_end[6].count, self.rif2_counter))
        print("OUT_UCAST_PKTS RIF3 counted: %d expected: %d" % (self.rif3_end[6].count, self.rif3_counter))
        print("IN_UCAST_BYTES  RIF0 counted: %d expected: %d" % (self.rif0_end[3].count, self.rif0_byte_counter))
        print("OUT_UCAST_BYTES RIF1 counted: %d expected: %d" % (self.rif1_end[9].count, self.rif1_byte_counter))
        print("OUT_UCAST_BYTES RIF2 counted: %d expected: %d" % (self.rif2_end[9].count, self.rif2_byte_counter))
        print("OUT_UCAST_BYTES RIF3 counted: %d expected: %d" % (self.rif3_end[9].count, self.rif3_byte_counter))
        self.assertTrue((self.rif0_end[0].count - self.rif0_start[0].count) == self.rif0_counter)
        self.assertTrue((self.rif1_end[6].count - self.rif1_start[6].count) == self.rif1_counter)
        self.assertTrue((self.rif2_end[6].count - self.rif2_start[6].count) == self.rif2_counter)
        self.assertTrue((self.rif3_end[6].count - self.rif3_start[6].count) == self.rif3_counter)
        # add ethernet fcs to the expected size
        self.assertTrue((self.rif0_end[3].count - self.rif0_start[3].count) == (self.rif0_byte_counter + (4*self.rif0_counter)))
        self.assertTrue((self.rif1_end[9].count - self.rif1_start[9].count) == (self.rif1_byte_counter + (4*self.rif1_counter)))
        self.assertTrue((self.rif2_end[9].count - self.rif2_start[9].count) == (self.rif2_byte_counter + (4*self.rif2_counter)))
        self.assertTrue((self.rif3_end[9].count - self.rif3_start[9].count) == (self.rif3_byte_counter + (4*self.rif3_counter)))

###############################################################################

@group('l3')
class L3SubPortTest(ApiHelper):
    '''
    Configuration:
      port0        -> 100, 200
      port1        -> 200, 300
      lag0/port2   -> 400, 500
      svi600/port3 -> 600
      svi700/port4 -> 400, 500
    '''
    def setUp(self):
        print()

        self.configure()
        self.nhop_drop = self.add_nexthop(self.device, type=SWITCH_NEXTHOP_ATTR_TYPE_DROP)
        self.nhop_glean = self.add_nexthop(self.device, type=SWITCH_NEXTHOP_ATTR_TYPE_GLEAN)

        # add L3 RIF on port0, port1 and lag0
        self.rif0 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port0, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.rif0_icntr = 0
        self.rif0_ecntr = 0
        self.nhop0 = self.add_nexthop(self.device, handle=self.rif0, dest_ip='10.10.0.1')
        self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=self.rif0, dest_ip='10.10.0.1')

        self.rif1 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port1, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.rif1_icntr = 0
        self.rif1_ecntr = 0
        self.nhop1 = self.add_nexthop(self.device, handle=self.rif1, dest_ip='10.10.0.2')
        self.add_neighbor(self.device, mac_address='00:22:22:33:44:55', handle=self.rif1, dest_ip='10.10.0.2')

        self.lag0 = self.add_lag(self.device)
        lag_mbr0 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port2)
        self.rif2 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.lag0, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.rif2_icntr = 0
        self.rif2_ecntr = 0
        self.nhop2 = self.add_nexthop(self.device, handle=self.rif2, dest_ip='10.10.0.3')
        self.add_neighbor(self.device, mac_address='00:33:22:33:44:55', handle=self.rif2, dest_ip='10.10.0.3')

        # add SVI on vlan600, port3 and vlan700, port4
        self.vlan600 = self.add_vlan(self.device, vlan_id=600)
        self.add_vlan_member(self.device, vlan_handle=self.vlan600, member_handle=self.port3)
        self.attribute_set(self.port3, SWITCH_PORT_ATTR_PORT_VLAN_ID, 600)
        self.rif3 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_VLAN, vlan_handle=self.vlan600, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.rif3_icntr = 0
        self.rif3_ecntr = 0
        self.nhop3 = self.add_nexthop(self.device, handle=self.rif3, dest_ip='10.10.0.4')
        self.add_neighbor(self.device, mac_address='00:44:22:33:44:55', handle=self.rif3, dest_ip='10.10.0.4')
        self.add_mac_entry(self.device, vlan_handle=self.vlan600, mac_address='00:44:22:33:44:55', destination_handle=self.port3)

        self.vlan700 = self.add_vlan(self.device, vlan_id=700)
        self.add_vlan_member(self.device, vlan_handle=self.vlan700, member_handle=self.port4, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
        self.rif4 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_VLAN, vlan_handle=self.vlan700, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.rif4_icntr = 0
        self.rif4_ecntr = 0
        self.nhop4 = self.add_nexthop(self.device, handle=self.rif4, dest_ip='10.10.0.5')
        self.add_neighbor(self.device, mac_address='00:55:22:33:44:55', handle=self.rif4, dest_ip='10.10.0.5')
        self.add_mac_entry(self.device, vlan_handle=self.vlan700, mac_address='00:55:22:33:44:55', destination_handle=self.port4)

        # add subport 100, 200 on port0
        self.subport0_100 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_SUB_PORT, port_handle=self.port0, outer_vlan_id=100, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.subport0_100_icntr = 0
        self.subport0_100_ecntr = 0
        self.nhop_sp0_100 = self.add_nexthop(self.device, handle=self.subport0_100, dest_ip='20.20.0.10')
        self.add_neighbor(self.device, mac_address='00:33:33:33:01:00', handle=self.subport0_100, dest_ip='20.20.0.10')
        self.subport0_200 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_SUB_PORT, port_handle=self.port0, outer_vlan_id=200, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.subport0_200_icntr = 0
        self.subport0_200_ecntr = 0
        self.nhop_sp0_200 = self.add_nexthop(self.device, handle=self.subport0_200, dest_ip='20.20.0.20')
        self.add_neighbor(self.device, mac_address='00:33:33:33:02:00', handle=self.subport0_200, dest_ip='20.20.0.20')

        # add subport 200, 300 on port1
        self.subport1_200 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_SUB_PORT, port_handle=self.port1, outer_vlan_id=200, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.subport1_200_icntr = 0
        self.subport1_200_ecntr = 0
        self.nhop_sp1_200 = self.add_nexthop(self.device, handle=self.subport1_200, dest_ip='20.20.1.20')
        self.add_neighbor(self.device, mac_address='00:33:33:33:12:00', handle=self.subport1_200, dest_ip='20.20.1.20')
        self.subport1_300 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_SUB_PORT, port_handle=self.port1, outer_vlan_id=300, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.subport1_300_icntr = 0
        self.subport1_300_ecntr = 0
        self.nhop_sp1_300 = self.add_nexthop(self.device, handle=self.subport1_300, dest_ip='20.20.1.30')
        self.add_neighbor(self.device, mac_address='00:33:33:33:13:00', handle=self.subport1_300, dest_ip='20.20.1.30')

        # add subport 400, 500 on lag0
        self.sublag0_400 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_SUB_PORT, port_handle=self.lag0, outer_vlan_id=400, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.sublag0_400_icntr = 0
        self.sublag0_400_ecntr = 0
        self.nhop_sl0_400 = self.add_nexthop(self.device, handle=self.sublag0_400, dest_ip='20.20.0.40')
        self.add_neighbor(self.device, mac_address='00:33:33:33:04:00', handle=self.sublag0_400, dest_ip='20.20.0.40')
        self.sublag0_500 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_SUB_PORT, port_handle=self.lag0, outer_vlan_id=500, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.sublag0_500_icntr = 0
        self.sublag0_500_ecntr = 0
        self.nhop_sl0_500 = self.add_nexthop(self.device, handle=self.sublag0_500, dest_ip='20.20.0.50')
        self.add_neighbor(self.device, mac_address='00:33:33:33:05:00', handle=self.sublag0_500, dest_ip='20.20.0.50')

        # add subport 600 on port3, this port is untagged on vlan600
        self.subport3_600 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_SUB_PORT, port_handle=self.port3, outer_vlan_id=600, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.subport3_600_icntr = 0
        self.subport3_600_ecntr = 0
        self.nhop_sp3_600 = self.add_nexthop(self.device, handle=self.subport3_600, dest_ip='20.20.3.60')
        self.add_neighbor(self.device, mac_address='00:33:33:33:36:00', handle=self.subport3_600, dest_ip='20.20.3.60')

        # add subport 400, 500 on port4, this port is tagged on vlan700
        self.subport4_400 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_SUB_PORT, port_handle=self.port4, outer_vlan_id=400, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.subport4_400_icntr = 0
        self.subport4_400_ecntr = 0
        self.nhop_sp4_400 = self.add_nexthop(self.device, handle=self.subport4_400, dest_ip='20.20.4.40')
        self.add_neighbor(self.device, mac_address='00:33:33:33:44:00', handle=self.subport4_400, dest_ip='20.20.4.40')
        self.subport4_500 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_SUB_PORT, port_handle=self.port4, outer_vlan_id=500, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.subport4_500_icntr = 0
        self.subport4_500_ecntr = 0
        self.nhop_sp4_500 = self.add_nexthop(self.device, handle=self.subport4_500, dest_ip='20.20.4.50')
        self.add_neighbor(self.device, mac_address='00:33:33:33:45:00', handle=self.subport4_500, dest_ip='20.20.4.50')

        # add myip routes for all subports
        self.add_route(self.device, ip_prefix='40.40.0.11', vrf_handle=self.vrf10, nexthop_handle=self.nhop_glean)
        self.add_route(self.device, ip_prefix='40.40.0.21', vrf_handle=self.vrf10, nexthop_handle=self.nhop_glean)
        self.add_route(self.device, ip_prefix='40.40.1.21', vrf_handle=self.vrf10, nexthop_handle=self.nhop_glean)
        self.add_route(self.device, ip_prefix='40.40.1.31', vrf_handle=self.vrf10, nexthop_handle=self.nhop_glean)
        self.add_route(self.device, ip_prefix='40.40.0.41', vrf_handle=self.vrf10, nexthop_handle=self.nhop_glean)
        self.add_route(self.device, ip_prefix='40.40.0.51', vrf_handle=self.vrf10, nexthop_handle=self.nhop_glean)
        self.add_route(self.device, ip_prefix='40.40.3.61', vrf_handle=self.vrf10, nexthop_handle=self.nhop_glean)
        self.add_route(self.device, ip_prefix='40.40.4.41', vrf_handle=self.vrf10, nexthop_handle=self.nhop_glean)
        self.add_route(self.device, ip_prefix='40.40.4.51', vrf_handle=self.vrf10, nexthop_handle=self.nhop_glean)
        # add routes to all nhop
        self.add_route(self.device, ip_prefix='30.30.0.1', vrf_handle=self.vrf10, nexthop_handle=self.nhop0)
        self.add_route(self.device, ip_prefix='30.30.1.1', vrf_handle=self.vrf10, nexthop_handle=self.nhop1)
        self.add_route(self.device, ip_prefix='30.30.2.1', vrf_handle=self.vrf10, nexthop_handle=self.nhop2)
        self.add_route(self.device, ip_prefix='30.30.3.1', vrf_handle=self.vrf10, nexthop_handle=self.nhop3)
        self.add_route(self.device, ip_prefix='30.30.4.1', vrf_handle=self.vrf10, nexthop_handle=self.nhop4)
        self.add_route(self.device, ip_prefix='40.40.0.10', vrf_handle=self.vrf10, nexthop_handle=self.nhop_sp0_100)
        self.add_route(self.device, ip_prefix='40.40.0.20', vrf_handle=self.vrf10, nexthop_handle=self.nhop_sp0_200)
        self.add_route(self.device, ip_prefix='40.40.1.20', vrf_handle=self.vrf10, nexthop_handle=self.nhop_sp1_200)
        self.add_route(self.device, ip_prefix='40.40.1.30', vrf_handle=self.vrf10, nexthop_handle=self.nhop_sp1_300)
        self.add_route(self.device, ip_prefix='40.40.0.40', vrf_handle=self.vrf10, nexthop_handle=self.nhop_sl0_400)
        self.add_route(self.device, ip_prefix='40.40.0.50', vrf_handle=self.vrf10, nexthop_handle=self.nhop_sl0_500)
        self.add_route(self.device, ip_prefix='40.40.3.60', vrf_handle=self.vrf10, nexthop_handle=self.nhop_sp3_600)
        self.add_route(self.device, ip_prefix='40.40.4.40', vrf_handle=self.vrf10, nexthop_handle=self.nhop_sp4_400)
        self.add_route(self.device, ip_prefix='40.40.4.50', vrf_handle=self.vrf10, nexthop_handle=self.nhop_sp4_500)

        self.ecmp0 = self.add_ecmp(self.device)
        route1 = self.add_route(self.device, ip_prefix='60.60.60.0/16', vrf_handle=self.vrf10, nexthop_handle=self.ecmp0)
        self.add_ecmp_member(self.device, nexthop_handle=self.nhop_sp0_200, ecmp_handle=self.ecmp0)
        self.add_ecmp_member(self.device, nexthop_handle=self.nhop_sp1_200, ecmp_handle=self.ecmp0)
        self.add_ecmp_member(self.device, nexthop_handle=self.nhop_sl0_400, ecmp_handle=self.ecmp0)
        self.add_ecmp_member(self.device, nexthop_handle=self.nhop_sp3_600, ecmp_handle=self.ecmp0)
        self.add_ecmp_member(self.device, nexthop_handle=self.nhop_sp4_500, ecmp_handle=self.ecmp0)

        self.rif0_start = self.client.object_counters_get(self.rif0)
        self.rif1_start = self.client.object_counters_get(self.rif1)
        self.rif2_start = self.client.object_counters_get(self.rif2)
        self.rif3_start = self.client.object_counters_get(self.rif3)
        self.rif4_start = self.client.object_counters_get(self.rif4)
        self.sp0_100_start = self.client.object_counters_get(self.subport0_100)
        self.sp0_200_start = self.client.object_counters_get(self.subport0_200)
        self.sp1_200_start = self.client.object_counters_get(self.subport1_200)
        self.sp1_300_start = self.client.object_counters_get(self.subport1_300)
        self.sl0_400_start = self.client.object_counters_get(self.sublag0_400)
        self.sl0_500_start = self.client.object_counters_get(self.sublag0_500)
        self.sp3_600_start = self.client.object_counters_get(self.subport3_600)
        self.sp4_400_start = self.client.object_counters_get(self.subport4_400)
        self.sp4_500_start = self.client.object_counters_get(self.subport4_500)

    def runTest(self):
        try:
            self.RifVrfUpdateTest()
            self.RifToSubPortTest()
            self.SubPortToRifTest()
            self.SubPortToSubPortTest()
            self.SubPortIP2METest()
            self.SubPortECMPTest()
            self.LagSubPortUnitTest()
            self.PVMissTest()
            self.NoFloodTest()
            self.VlanConflictTest()
            self.SubPortAdminStatusTest()
            self.SubportIngressAclTest()
            self.SubportEgressAclTest()
            self.SubportQoSGroupTest()
            self.RifStatsTest()
            self.SubPortScaleTest()
        finally:
            pass

    def tearDown(self):
        self.attribute_set(self.port3, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.cleanup()

    def RifVrfUpdateTest(self):
        print("RifVrfUpdateTest")
        pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='30.30.1.1',
            ip_id=105,
            ip_ttl=64,
            dl_vlan_enable=True,
            vlan_vid=100,
            pktlen=104)
        exp_pkt1 = simple_tcp_packet(
            eth_dst='00:22:22:33:44:55',
            eth_src='00:77:66:55:44:33',
            ip_dst='30.30.1.1',
            ip_id=105,
            ip_ttl=63,
            pktlen=100)
        exp_pkt0 = simple_tcp_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src='00:77:66:55:44:33',
            ip_dst='30.30.1.1',
            ip_id=105,
            ip_ttl=63,
            pktlen=100)
        self.vrf20 = self.add_vrf(self.device, id=20, src_mac=self.rmac)
        self.add_route(self.device, ip_prefix='30.30.1.1', vrf_handle=self.vrf20, nexthop_handle=self.nhop0)
        try:
            print("Sending packet on port %d, forward to port %d" % (self.devports[0], self.devports[1]))
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt1, self.devports[1])
            self.subport0_100_icntr += 1
            self.rif1_ecntr += 1

            print("Update VRF on ingress subport RIF to vrf20")
            self.attribute_set(self.subport0_100, SWITCH_RIF_ATTR_VRF_HANDLE, self.vrf20)
            print("Sending packet on port %d, forward to port %d" % (self.devports[0], self.devports[0]))
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt0, self.devports[0])
            self.subport0_100_icntr += 1
            self.rif0_ecntr += 1

            print("Update VRF on ingress subport RIF to vrf10")
            self.attribute_set(self.subport0_100, SWITCH_RIF_ATTR_VRF_HANDLE, self.vrf10)
            print("Sending packet on port %d, forward to port %d" % (self.devports[0], self.devports[1]))
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt1, self.devports[1])
            self.subport0_100_icntr += 1
            self.rif1_ecntr += 1
        finally:
            self.attribute_set(self.subport0_100, SWITCH_RIF_ATTR_VRF_HANDLE, self.vrf10)
            self.cleanlast()
            self.cleanlast()

    def RifToSubPortTest(self):
        print("RifToSubPortTest()")
        pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='40.40.0.10',
            ip_src='30.30.0.1',
            ip_id=105,
            ip_ttl=64,
            pktlen=100)
        tagged_pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='40.40.0.10',
            ip_src='30.30.0.1',
            ip_id=105,
            ip_ttl=64,
            dl_vlan_enable=True,
            vlan_vid=700,
            pktlen=104)
        exp_pkt = simple_tcp_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src='00:77:66:55:44:33',
            ip_dst='40.40.0.10',
            ip_src='30.30.0.1',
            ip_id=105,
            ip_ttl=63,
            dl_vlan_enable=True,
            vlan_vid=100,
            pktlen=104)
        try:
            pkt_data = [
                ['40.40.0.10', '00:33:33:33:01:00', 100, 0, 'subport0.100'],
                ['40.40.0.20', '00:33:33:33:02:00', 200, 0, 'subport0.200'],
                ['40.40.1.20', '00:33:33:33:12:00', 200, 1, 'subport1.200'],
                ['40.40.1.30', '00:33:33:33:13:00', 300, 1, 'subport1.300'],
                ['40.40.0.40', '00:33:33:33:04:00', 400, 2, 'sublag0.400'],
                ['40.40.0.50', '00:33:33:33:05:00', 500, 2, 'sublag0.500'],
                ['40.40.3.60', '00:33:33:33:36:00', 600, 3, 'subport3.600'],
                ['40.40.4.40', '00:33:33:33:44:00', 400, 4, 'subport4.400'],
                ['40.40.4.50', '00:33:33:33:45:00', 500, 4, 'subport4.500'],
            ]
            ingress_rifs = [0,1,2,3]
            # untagged packets
            for port in ingress_rifs:
                for content in pkt_data:
                    pkt[IP].dst = content[0]
                    exp_pkt[IP].dst = content[0]
                    exp_pkt[Ether].dst = content[1]
                    exp_pkt[Dot1Q].vlan = content[2]
                    #print("Sending packet rif%d -> %s" % (port, content[4]))
                    send_packet(self, self.devports[port], pkt)
                    verify_packet(self, exp_pkt, self.devports[content[3]])
            # tagged packets
            for content in pkt_data:
                tagged_pkt[IP].dst = content[0]
                exp_pkt[IP].dst = content[0]
                exp_pkt[Ether].dst = content[1]
                exp_pkt[Dot1Q].vlan = content[2]
                #print("Sending packet rif%d -> %s" % (4, content[4]))
                send_packet(self, self.devports[4], tagged_pkt)
                verify_packet(self, exp_pkt, self.devports[content[3]])
            self.rif0_icntr += 9
            self.rif1_icntr += 9
            self.rif2_icntr += 9
            self.rif3_icntr += 9
            self.rif4_icntr += 9
            self.subport0_100_ecntr += 5
            self.subport0_200_ecntr += 5
            self.subport1_200_ecntr += 5
            self.subport1_300_ecntr += 5
            self.sublag0_400_ecntr += 5
            self.sublag0_500_ecntr += 5
            self.subport3_600_ecntr += 5
            self.subport4_400_ecntr += 5
            self.subport4_500_ecntr += 5
        finally:
            pass

    def SubPortToRifTest(self):
        print("SubPortToRifTest()")
        pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='30.30.0.1',
            ip_id=105,
            ip_ttl=64,
            dl_vlan_enable=True,
            vlan_vid=100,
            pktlen=104)
        exp_pkt = simple_tcp_packet(
            eth_dst='00:33:33:33:01:00',
            eth_src='00:77:66:55:44:33',
            ip_dst='30.30.0.1',
            ip_id=105,
            ip_ttl=63,
            pktlen=100)
        exp_tagged_pkt = simple_tcp_packet(
            eth_dst='00:33:33:33:01:00',
            eth_src='00:77:66:55:44:33',
            ip_dst='30.30.0.1',
            ip_id=105,
            dl_vlan_enable=True,
            vlan_vid=700,
            ip_ttl=63,
            pktlen=104)
        try:
            pkt_data = [
                ['30.30.0.1', '00:11:22:33:44:55', 0],
                ['30.30.1.1', '00:22:22:33:44:55', 1],
                ['30.30.2.1', '00:33:22:33:44:55', 2],
                ['30.30.3.1', '00:44:22:33:44:55', 3],
            ]
            ingress_rifs = [[0, 100], [0, 200], [1, 200], [1, 300],
                            [2, 400], [2, 500],
                            [3, 600], [4, 400], [4, 500]]
            for item in ingress_rifs:
                for content in pkt_data:
                    pkt[IP].dst = content[0]
                    pkt[Dot1Q].vlan = item[1]
                    exp_pkt[IP].dst = content[0]
                    exp_pkt[Ether].dst = content[1]
                    #print("Sending packet subport%d.%d -> rif%d" % (item[0], item[1], content[2]))
                    send_packet(self, self.devports[item[0]], pkt)
                    verify_packet(self, exp_pkt, self.devports[content[2]])
            pkt_data = [
                ['30.30.4.1', '00:55:22:33:44:55', 4],
            ]
            for item in ingress_rifs:
                for content in pkt_data:
                    pkt[IP].dst = content[0]
                    pkt[Dot1Q].vlan = item[1]
                    exp_tagged_pkt[IP].dst = content[0]
                    exp_tagged_pkt[Ether].dst = content[1]
                #print("Sending packet subport%d.%d -> rif%d" % (item[0], item[1], content[2]))
                send_packet(self, self.devports[item[0]], pkt)
                verify_packet(self, exp_tagged_pkt, self.devports[content[2]])
            self.rif0_ecntr += 9
            self.rif1_ecntr += 9
            self.rif2_ecntr += 9
            self.rif3_ecntr += 9
            self.rif4_ecntr += 9
            self.subport0_100_icntr += 5
            self.subport0_200_icntr += 5
            self.subport1_200_icntr += 5
            self.subport1_300_icntr += 5
            self.sublag0_400_icntr += 5
            self.sublag0_500_icntr += 5
            self.subport3_600_icntr += 5
            self.subport4_400_icntr += 5
            self.subport4_500_icntr += 5
        finally:
            pass

    def SubPortToSubPortTest(self):
        print("SubPortToSubPortTest()")
        pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='40.40.0.10',
            ip_src='30.30.0.1',
            ip_id=105,
            ip_ttl=64,
            dl_vlan_enable=True,
            vlan_vid=100,
            pktlen=104)
        exp_pkt = simple_tcp_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src='00:77:66:55:44:33',
            ip_dst='40.40.0.10',
            ip_src='30.30.0.1',
            ip_id=105,
            ip_ttl=63,
            dl_vlan_enable=True,
            vlan_vid=100,
            pktlen=104)
        try:
            pkt_data = [
                ['40.40.0.10', '00:33:33:33:01:00', 100, 0, self.subport0_100],
                ['40.40.0.20', '00:33:33:33:02:00', 200, 0, self.subport0_200],
                ['40.40.1.20', '00:33:33:33:12:00', 200, 1, self.subport1_200],
                ['40.40.1.30', '00:33:33:33:13:00', 300, 1, self.subport1_300],
                ['40.40.0.40', '00:33:33:33:04:00', 400, 2, self.sublag0_400],
                ['40.40.0.50', '00:33:33:33:05:00', 500, 2, self.sublag0_500],
                ['40.40.3.60', '00:33:33:33:36:00', 600, 3, self.subport3_600],
                ['40.40.4.40', '00:33:33:33:44:00', 400, 4, self.subport4_400],
                ['40.40.4.50', '00:33:33:33:45:00', 500, 4, self.subport4_500],
            ]
            # untagged packets
            for irif in pkt_data:
                for erif in pkt_data:
                    if irif[4] == erif[4]:
                        continue
                    pkt[IP].dst = erif[0]
                    pkt[Dot1Q].vlan = irif[2]
                    exp_pkt[IP].dst = erif[0]
                    exp_pkt[Ether].dst = erif[1]
                    exp_pkt[Dot1Q].vlan = erif[2]
                    #print("Sending packet rif%d -> %s" % (port, content[4]))
                    send_packet(self, self.devports[irif[3]], pkt)
                    verify_packet(self, exp_pkt, self.devports[erif[3]])
            self.subport0_100_icntr += 8
            self.subport0_200_icntr += 8
            self.subport1_200_icntr += 8
            self.subport1_300_icntr += 8
            self.sublag0_400_icntr += 8
            self.sublag0_500_icntr += 8
            self.subport3_600_icntr += 8
            self.subport4_400_icntr += 8
            self.subport4_500_icntr += 8
            self.subport0_100_ecntr += 8
            self.subport0_200_ecntr += 8
            self.subport1_200_ecntr += 8
            self.subport1_300_ecntr += 8
            self.sublag0_400_ecntr += 8
            self.sublag0_500_ecntr += 8
            self.subport3_600_ecntr += 8
            self.subport4_400_ecntr += 8
            self.subport4_500_ecntr += 8
        finally:
            pass

    def SubPortIP2METest(self):
        print("SubPortIP2METest()")
        if self.test_params['target'] == 'hw':
            print("Skipping on HW")
            return
        pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='40.40.0.10',
            ip_src='30.30.0.1',
            ip_id=105,
            ip_ttl=64,
            dl_vlan_enable=True,
            vlan_vid=100,
            pktlen=104)
        try:
            pkt_data = [
                ['40.40.0.11', 100, 0, self.get_port_ifindex(self.port0)],
                ['40.40.0.21', 200, 0, self.get_port_ifindex(self.port0)],
                ['40.40.1.21', 200, 1, self.get_port_ifindex(self.port1)],
                ['40.40.1.31', 300, 1, self.get_port_ifindex(self.port1)],
                ['40.40.0.41', 400, 2, self.get_port_ifindex(self.lag0, port_type=1)],
                ['40.40.0.51', 500, 2, self.get_port_ifindex(self.lag0, port_type=1)],
                ['40.40.3.61', 600, 3, self.get_port_ifindex(self.port3)],
                ['40.40.4.41', 400, 4, self.get_port_ifindex(self.port4)],
                ['40.40.4.51', 500, 4, self.get_port_ifindex(self.port4)],
            ]
            for data in pkt_data:
                pkt[IP].dst = data[0]
                pkt[Dot1Q].vlan = data[1]
                #print("Sending packet rif%d -> %s" % (port, content[4]))
                cpu_pkt = simple_cpu_packet(
                    packet_type=0,
                    ingress_port=swport_to_devport(self, self.devports[data[2]]),
                    ingress_ifindex=data[3],
                    reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_MAX + SWITCH_HOSTIF_TRAP_ATTR_INTERNAL_TRAP_GLEAN,
                    ingress_bd=0x0,
                    inner_pkt=pkt)
                cpu_pkt = cpu_packet_mask_ingress_bd(cpu_pkt)
                send_packet(self, self.devports[data[2]], pkt)
                verify_packet(self, cpu_pkt, self.cpu_port)
            self.subport0_100_icntr += 1
            self.subport0_200_icntr += 1
            self.subport1_200_icntr += 1
            self.subport1_300_icntr += 1
            self.sublag0_400_icntr += 1
            self.sublag0_500_icntr += 1
            self.subport3_600_icntr += 1
            self.subport4_400_icntr += 1
            self.subport4_500_icntr += 1
            self.subport0_100_ecntr += 1
            self.subport0_200_ecntr += 1
            self.subport1_200_ecntr += 1
            self.subport1_300_ecntr += 1
            self.sublag0_400_ecntr += 1
            self.sublag0_500_ecntr += 1
            self.subport3_600_ecntr += 1
            self.subport4_400_ecntr += 1
            self.subport4_500_ecntr += 1
        finally:
            pass

    def SubPortECMPTest(self):
        print("SubPortECMPTest()")
        try:
            count = [0, 0, 0, 0, 0]
            dst_ip = int(binascii.hexlify(socket.inet_aton('60.60.60.1')), 16)
            src_ip = int(binascii.hexlify(socket.inet_aton('192.168.8.1')), 16)
            max_itrs = 200
            for i in range(0, max_itrs):
                dst_ip_addr = socket.inet_ntoa(
                    binascii.unhexlify(format(dst_ip, 'x').zfill(8)))
                src_ip_addr = socket.inet_ntoa(
                    binascii.unhexlify(format(src_ip, 'x').zfill(8)))
                pkt = simple_tcp_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src='00:22:22:22:22:22',
                    ip_dst=dst_ip_addr,
                    ip_src=src_ip_addr,
                    ip_ttl=64,
                    pktlen=100)

                exp_pkt1 = simple_tcp_packet(
                    eth_dst='00:33:33:33:02:00',
                    eth_src='00:77:66:55:44:33',
                    ip_dst=dst_ip_addr,
                    ip_src=src_ip_addr,
                    dl_vlan_enable=True,
                    vlan_vid=200,
                    ip_ttl=63,
                    pktlen=104)
                exp_pkt2 = simple_tcp_packet(
                    eth_dst='00:33:33:33:12:00',
                    eth_src='00:77:66:55:44:33',
                    ip_dst=dst_ip_addr,
                    ip_src=src_ip_addr,
                    dl_vlan_enable=True,
                    vlan_vid=200,
                    ip_ttl=63,
                    pktlen=104)
                exp_pkt3 = simple_tcp_packet(
                    eth_dst='00:33:33:33:04:00',
                    eth_src='00:77:66:55:44:33',
                    ip_dst=dst_ip_addr,
                    ip_src=src_ip_addr,
                    dl_vlan_enable=True,
                    vlan_vid=400,
                    ip_ttl=63,
                    pktlen=104)
                exp_pkt4 = simple_tcp_packet(
                    eth_dst='00:33:33:33:36:00',
                    eth_src='00:77:66:55:44:33',
                    ip_dst=dst_ip_addr,
                    ip_src=src_ip_addr,
                    dl_vlan_enable=True,
                    vlan_vid=600,
                    ip_ttl=63,
                    pktlen=104)
                exp_pkt5 = simple_tcp_packet(
                    eth_dst='00:33:33:33:45:00',
                    eth_src='00:77:66:55:44:33',
                    ip_dst=dst_ip_addr,
                    ip_src=src_ip_addr,
                    dl_vlan_enable=True,
                    vlan_vid=500,
                    ip_ttl=63,
                    pktlen=104)

                send_packet(self, self.devports[0], pkt)
                rcv_idx = verify_any_packet_any_port(
                    self, [exp_pkt1, exp_pkt2, exp_pkt3, exp_pkt4, exp_pkt5], [
                        self.devports[0],
                        self.devports[1],
                        self.devports[2],
                        self.devports[3],
                        self.devports[4]
                    ], timeout=2)
                count[rcv_idx] += 1
                dst_ip += 1
                src_ip += 1

            print('count:', count)
            for i in range(0, 5):
                self.assertTrue((count[i] >= ((max_itrs / 5) * 0.7)),
                                "Ecmp paths are not equally balanced")
            self.rif0_icntr += 200
            self.subport0_200_ecntr += count[0]
            self.subport1_200_ecntr += count[1]
            self.sublag0_400_ecntr += count[2]
            self.subport3_600_ecntr += count[3]
            self.subport4_500_ecntr += count[4]

        finally:
            pass

    def LagSubPortUnitTest(self):
        print("LagSubPortUnitTest()")
        pkt_400 = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            ip_dst='30.30.0.1',
            ip_ttl=64,
            dl_vlan_enable=True,
            vlan_vid=400,
            pktlen=104)
        pkt_500 = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            ip_dst='30.30.0.1',
            ip_ttl=64,
            dl_vlan_enable=True,
            vlan_vid=500,
            pktlen=104)
        pkt_800 = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            ip_dst='30.30.0.1',
            ip_ttl=64,
            dl_vlan_enable=True,
            vlan_vid=800,
            pktlen=104)
        exp_pkt = simple_tcp_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src='00:77:66:55:44:33',
            ip_dst='30.30.0.1',
            ip_ttl=63,
            pktlen=100)

        try:
            print("Send from port%d/400 of lag0" % self.devports[2])
            send_packet(self, self.devports[2], pkt_400)
            verify_packet(self, exp_pkt, self.devports[0])
            self.sublag0_400_icntr += 1
            self.rif0_ecntr += 1
            print("Send from port%d/500 of lag0" % self.devports[2])
            send_packet(self, self.devports[2], pkt_500)
            verify_packet(self, exp_pkt, self.devports[0])
            self.sublag0_500_icntr += 1
            self.rif0_ecntr += 1
            print("Send from port%d/400 not added to lag0, drop" % self.devports[6])
            send_packet(self, self.devports[6], pkt_400)
            verify_no_other_packets(self, timeout=2)
            print("Send from port%d/500 not added to lag0, drop" % self.devports[6])
            send_packet(self, self.devports[6], pkt_500)
            verify_no_other_packets(self, timeout=2)
            print("Add lag member on port %d" % self.devports[6])
            lag_mem = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port6)
            self.stack.pop()
            print("Send from port%d/400 after add to lag0, fwd" % self.devports[6])
            send_packet(self, self.devports[6], pkt_400)
            verify_packet(self, exp_pkt, self.devports[0])
            self.sublag0_400_icntr += 1
            self.rif0_ecntr += 1
            print("Send from port%d/500 after add to lag0, fwd" % self.devports[6])
            send_packet(self, self.devports[6], pkt_500)
            verify_packet(self, exp_pkt, self.devports[0])
            self.sublag0_500_icntr += 1
            self.rif0_ecntr += 1

            print("Send from port%d/800 on lag0 with no subport800, drop" % self.devports[2])
            send_packet(self, self.devports[2], pkt_800)
            verify_no_other_packets(self, timeout=2)
            print("Send from port%d/800 on lag0 with no subport800, drop" % self.devports[6])
            send_packet(self, self.devports[6], pkt_800)
            verify_no_other_packets(self, timeout=2)
            print("Add subport vlan 800 on lag 0")
            self.sublag0_800 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_SUB_PORT, port_handle=self.lag0, outer_vlan_id=800, vrf_handle=self.vrf10, src_mac=self.rmac)
            print("Send from port%d/800 on lag0 after subport800 add, fwd" % self.devports[2])
            send_packet(self, self.devports[2], pkt_800)
            verify_packet(self, exp_pkt, self.devports[0])
            self.rif0_ecntr += 1
            print("Send from port%d/800 on lag0 after subport800 add, fwd" % self.devports[6])
            send_packet(self, self.devports[6], pkt_800)
            verify_packet(self, exp_pkt, self.devports[0])
            self.rif0_ecntr += 1

            print("Remove lag member on port %d" % self.devports[6])
            self.client.object_delete(lag_mem)
            print("Send from port%d/400 after removing from lag0, drop" % self.devports[6])
            send_packet(self, self.devports[6], pkt_400)
            verify_no_other_packets(self, timeout=2)
            print("Send from port%d/500 after removing from lag0, drop" % self.devports[6])
            send_packet(self, self.devports[6], pkt_500)
            verify_no_other_packets(self, timeout=2)
            print("Send from port%d/800 after removing from lag0, drop" % self.devports[6])
            send_packet(self, self.devports[6], pkt_800)
            verify_no_other_packets(self, timeout=2)
            print("Send from port%d/400 of lag0" % self.devports[2])
            send_packet(self, self.devports[2], pkt_400)
            verify_packet(self, exp_pkt, self.devports[0])
            self.sublag0_400_icntr += 1
            self.rif0_ecntr += 1
            print("Send from port%d/500 of lag0" % self.devports[2])
            send_packet(self, self.devports[2], pkt_500)
            verify_packet(self, exp_pkt, self.devports[0])
            self.sublag0_500_icntr += 1
            self.rif0_ecntr += 1
            print("Send from port%d/800 of lag0" % self.devports[2])
            send_packet(self, self.devports[2], pkt_800)
            verify_packet(self, exp_pkt, self.devports[0])
            self.rif0_ecntr += 1

            print("Remove subport 800 on lag0")
            self.cleanlast()
            print("Send from port%d/800 on lag0 with no subport800, drop" % self.devports[2])
            send_packet(self, self.devports[2], pkt_800)
            verify_no_other_packets(self, timeout=2)
            print("Send from port%d/400 of lag0" % self.devports[2])
            send_packet(self, self.devports[2], pkt_400)
            verify_packet(self, exp_pkt, self.devports[0])
            self.sublag0_400_icntr += 1
            self.rif0_ecntr += 1
            print("Send from port%d/500 of lag0" % self.devports[2])
            send_packet(self, self.devports[2], pkt_500)
            verify_packet(self, exp_pkt, self.devports[0])
            self.sublag0_500_icntr += 1
            self.rif0_ecntr += 1
        finally:
            pass

    def PVMissTest(self):
        print("PVMissTest()")
        pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='30.30.0.1',
            ip_ttl=64,
            dl_vlan_enable=True,
            vlan_vid=100,
            pktlen=104)
        exp_pkt = simple_tcp_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src='00:77:66:55:44:33',
            ip_dst='30.30.0.1',
            ip_ttl=63,
            pktlen=100)
        try:
            pkt[Dot1Q].vlan = 400
            send_packet(self, self.devports[4], pkt)
            verify_packet(self, exp_pkt, self.devports[0])
            self.subport4_400_icntr += 1
            pkt[Dot1Q].vlan = 500
            send_packet(self, self.devports[4], pkt)
            verify_packet(self, exp_pkt, self.devports[0])
            self.subport4_500_icntr += 1
            pkt[Dot1Q].vlan = 700
            send_packet(self, self.devports[4], pkt)
            verify_packet(self, exp_pkt, self.devports[0])
            self.rif4_icntr += 1
            self.rif0_ecntr += 3
            pkt[Dot1Q].vlan = 40
            send_packet(self, self.devports[4], pkt)
            verify_no_other_packets(self, timeout=1)
            pkt[Dot1Q].vlan = 100
            send_packet(self, self.devports[4], pkt)
            verify_no_other_packets(self, timeout=1)
            pkt[Dot1Q].vlan = 200
            send_packet(self, self.devports[4], pkt)
            verify_no_other_packets(self, timeout=1)
        finally:
            pass

    def NoFloodTest(self):
        '''
        This test tries to verify packet not flooded on the tagged VLAN when no route hit
        using vlan 600 and port 3 for this test
        '''
        print("NoFloodTest()")
        pkt = simple_tcp_packet(
            eth_dst='00:66:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='50.50.0.1',
            ip_ttl=64)
        tagged_pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='50.50.0.1',
            ip_ttl=64,
            dl_vlan_enable=True,
            vlan_vid=600,
            pktlen=104)
        self.add_vlan_member(self.device, vlan_handle=self.vlan600, member_handle=self.port8)
        self.add_vlan_member(self.device, vlan_handle=self.vlan600, member_handle=self.port9)
        try:
            # ingress on vlan rif 600
            send_packet(self, self.devports[3], pkt)
            verify_packets(self, pkt, [self.devports[8], self.devports[9]])
            self.rif3_icntr += 1
            self.rif3_ecntr += 2
            # ingress on subport rif 600 with rmac
            send_packet(self, self.devports[3], tagged_pkt)
            verify_no_other_packets(self, timeout=1)
            self.subport3_600_icntr += 1
            # ingress on subport rif 600 with uknown unicast
            tagged_pkt[Ether].dst = '00:66:66:55:44:33'
            send_packet(self, self.devports[3], tagged_pkt)
            verify_no_other_packets(self, timeout=1)
            self.subport3_600_icntr += 1
        finally:
            self.cleanlast()
            self.cleanlast()

    def VlanConflictTest(self):
        '''
        Create a VLAN RIF and sub-port RIF with same vlan number and make sure
        two separate RIFs are created. Now delete VLAN and make sure sub-port
        RIF is not impacted. Repeat but this time delete the sub-port RIF
        '''
        print("VlanConflictTest()")
        pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='30.30.0.1',
            ip_ttl=64,
            pktlen=100)
        tagged_pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='30.30.0.1',
            ip_ttl=64,
            dl_vlan_enable=True,
            vlan_vid=800,
            pktlen=104)
        exp_pkt = simple_tcp_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src='00:77:66:55:44:33',
            ip_dst='30.30.0.1',
            ip_ttl=63,
            pktlen=100)

        try:
            print("Add vlan 800 on port 9")
            self.vlan800 = self.add_vlan(self.device, vlan_id=800, learning=False)
            self.add_vlan_member(self.device, vlan_handle=self.vlan800, member_handle=self.port9)
            self.attribute_set(self.port9, SWITCH_PORT_ATTR_PORT_VLAN_ID, 800)
            self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_VLAN, vlan_handle=self.vlan800, vrf_handle=self.vrf10, src_mac=self.rmac)
            print("Send untagged pkt on vlan 800 to port %d" % self.devports[0])
            send_packet(self, self.devports[9], pkt)
            verify_packet(self, exp_pkt, self.devports[0])
            print("Add subport 800 on port 8")
            self.subport8_800 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_SUB_PORT, port_handle=self.port8, outer_vlan_id=800, vrf_handle=self.vrf10, src_mac=self.rmac)
            print("Send untagged pkt on vlan 800 to port %d after subport add" % self.devports[0])
            send_packet(self, self.devports[9], pkt)
            verify_packet(self, exp_pkt, self.devports[0])
            print("Send tagged pkt on subport vlan 800 to port %d" % self.devports[0])
            send_packet(self, self.devports[8], tagged_pkt)
            verify_packet(self, exp_pkt, self.devports[0])
            self.cleanlast()
            print("Send untagged pkt on vlan 800 to port %d after subport removal" % self.devports[0])
            send_packet(self, self.devports[9], pkt)
            verify_packet(self, exp_pkt, self.devports[0])
            self.rif0_ecntr += 4
        finally:
            self.attribute_set(self.port9, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

        try:
            print("Add subport 800 on port 8")
            self.subport8_800 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_SUB_PORT, port_handle=self.port8, outer_vlan_id=800, vrf_handle=self.vrf10, src_mac=self.rmac)
            print("Send tagged pkt on subport vlan 800 to port %d" % self.devports[0])
            send_packet(self, self.devports[8], tagged_pkt)
            verify_packet(self, exp_pkt, self.devports[0])
            print("Add vlan 800 on port 9")
            self.vlan800 = self.add_vlan(self.device, vlan_id=800, learning=False)
            self.add_vlan_member(self.device, vlan_handle=self.vlan800, member_handle=self.port9)
            self.attribute_set(self.port9, SWITCH_PORT_ATTR_PORT_VLAN_ID, 800)
            self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_VLAN, vlan_handle=self.vlan800, vrf_handle=self.vrf10, src_mac=self.rmac)
            print("Send tagged pkt on subport vlan 800 to port %d after vlan add" % self.devports[0])
            send_packet(self, self.devports[8], tagged_pkt)
            verify_packet(self, exp_pkt, self.devports[0])
            print("Send untagged pkt on vlan 800 to port %d" % self.devports[0])
            send_packet(self, self.devports[9], pkt)
            verify_packet(self, exp_pkt, self.devports[0])
            self.attribute_set(self.port9, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            print("Send tagged pkt on vlan 800 to port %d after vlan removal" % self.devports[0])
            send_packet(self, self.devports[8], tagged_pkt)
            verify_packet(self, exp_pkt, self.devports[0])
            self.rif0_ecntr += 4
        finally:
            self.cleanlast()

    def SubPortAdminStatusTest(self):
        print("SubPortAdminStatusTest()")
        pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='30.30.0.1',
            ip_ttl=64,
            dl_vlan_enable=True,
            vlan_vid=200,
            pktlen=104)
        exp_pkt = simple_tcp_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src='00:77:66:55:44:33',
            ip_dst='30.30.0.1',
            ip_ttl=63,
            pktlen=100)
        try:
            print("Sending packet on sub port %d vlan 200, forward" % self.devports[1])
            send_packet(self, self.devports[1], pkt)
            verify_packet(self, exp_pkt, self.devports[0])
            self.subport1_200_icntr += 1
            self.rif0_ecntr += 1

            print("Disable IPv4 on subport")
            self.attribute_set(self.subport1_200, SWITCH_RIF_ATTR_IPV4_UNICAST, False)
            print("Sending packet on sub port %d vlan 200, drop" % self.devports[1])
            send_packet(self, self.devports[1], pkt)
            verify_no_other_packets(self, timeout=1)
            self.subport1_200_icntr += 1

            print("Enable IPv4 on subport")
            self.attribute_set(self.subport1_200, SWITCH_RIF_ATTR_IPV4_UNICAST, True)
            print("Sending packet on sub port %d vlan 200, forward" % self.devports[1])
            send_packet(self, self.devports[1], pkt)
            verify_packet(self, exp_pkt, self.devports[0])
            self.subport1_200_icntr += 1
            self.rif0_ecntr += 1
        finally:
            self.attribute_set(self.subport1_200, SWITCH_RIF_ATTR_IPV4_UNICAST, True)

    def SubportIngressAclTest(self):
        print("SubportIngressAclTest()")
        if(self.client.is_feature_enable(SWITCH_FEATURE_ACL_PORT_GROUP) == 1):
            print ("Subport ingress acl is not supported with ACL portgroup")
            return
        if (self.client.is_feature_enable(SWITCH_FEATURE_BD_LABEL_IN_INGRESS_ACL) == 0):
            print("IngressIPv4Acl RIF/VLAN binding feature not enabled, skipping")
            return

        if(self.client.is_feature_enable(SWITCH_FEATURE_INGRESS_IPV4_ACL) == 1):
            acl_table = self.add_acl_table(self.device,
                                           type=SWITCH_ACL_TABLE_ATTR_TYPE_IPV4,
                                           bind_point_type=[acl_table_bp_rif],
                                           direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        else:
            acl_table = self.add_acl_table(self.device,
                                           type=SWITCH_ACL_TABLE_ATTR_TYPE_IP,
                                           bind_point_type=[acl_table_bp_rif],
                                           direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_entry = self.add_acl_entry(self.device,
            dst_ip='40.40.1.20',
            dst_ip_mask='255.255.255.255',
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            table_handle=acl_table)
        pre_counter = self.client.object_counters_get(acl_entry)

        try:
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='40.40.1.20',
                ip_ttl=64,
                dl_vlan_enable=True,
                vlan_vid=100)
            exp_pkt = simple_tcp_packet(
                eth_dst='00:33:33:33:12:00',
                eth_src='00:77:66:55:44:33',
                ip_dst='40.40.1.20',
                ip_ttl=63,
                dl_vlan_enable=True,
                vlan_vid=200)
            print("Allow packet before binding ingress ACL to rif")
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt, self.devports[1])
            self.subport0_100_icntr += 1
            self.subport1_200_ecntr += 1

            print("Bind ACL to subport0_100")
            self.attribute_set(self.subport0_100, SWITCH_RIF_ATTR_INGRESS_ACL_HANDLE, acl_table)
            send_packet(self, self.devports[0], pkt)
            verify_no_other_packets(self, timeout=2)
            self.subport0_100_icntr += 1
            print("Sleeping for 2 sec before fetching stats")
            time.sleep(2)
            post_counter = self.client.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)

            print("Remove binding, packet should be forwarded")
            self.attribute_set(self.subport0_100, SWITCH_RIF_ATTR_INGRESS_ACL_HANDLE, 0)
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt, self.devports[1])
            self.subport0_100_icntr += 1
            self.subport1_200_ecntr += 1
        finally:
            self.attribute_set(self.subport0_100, SWITCH_RIF_ATTR_INGRESS_ACL_HANDLE, 0)
            self.cleanlast()
            self.cleanlast()

    def SubportEgressAclTest(self):
        print("SubportEgressAclTest()")
        if (self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_IP_ACL) == 0):
            print("EgressIPv4Acl feature not enabled, skipping")
            return

        if (self.client.is_feature_enable(SWITCH_FEATURE_BD_LABEL_IN_EGRESS_ACL) == 0):
            print("EgressIPv4Acl RIF/VLAN binding feature not enabled, skipping")
            return

        acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IPV4,
            bind_point_type=[acl_table_bp_rif],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_EGRESS)
        acl_entry = self.add_acl_entry(self.device,
            dst_ip='40.40.1.20',
            dst_ip_mask='255.255.255.255',
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            table_handle=acl_table)
        pre_counter = self.client.object_counters_get(acl_entry)

        try:
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='40.40.1.20',
                ip_ttl=64,
                dl_vlan_enable=True,
                vlan_vid=100)
            exp_pkt = simple_tcp_packet(
                eth_dst='00:33:33:33:12:00',
                eth_src='00:77:66:55:44:33',
                ip_dst='40.40.1.20',
                ip_ttl=63,
                dl_vlan_enable=True,
                vlan_vid=200)
            print("Allow packet before binding egress ACL to subport")
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt, self.devports[1])
            self.subport0_100_icntr += 1
            self.subport1_200_ecntr += 1

            print("Bind ACL to subport1_200")
            self.attribute_set(self.subport1_200, SWITCH_RIF_ATTR_EGRESS_ACL_HANDLE, acl_table)
            send_packet(self, self.devports[0], pkt)
            verify_no_other_packets(self, timeout=2)
            self.subport0_100_icntr += 1
            self.subport1_200_ecntr += 1
            print("Sleeping for 2 sec before fetching stats")
            time.sleep(2)
            post_counter = self.client.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)

            print("Remove binding, packet should be forwarded")
            self.attribute_set(self.subport1_200, SWITCH_RIF_ATTR_EGRESS_ACL_HANDLE, 0)
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt, self.devports[1])
            self.subport0_100_icntr += 1
            self.subport1_200_ecntr += 1
        finally:
            self.attribute_set(self.subport1_200, SWITCH_RIF_ATTR_EGRESS_ACL_HANDLE, 0)
            self.cleanlast()
            self.cleanlast()

    def SubportQoSGroupTest(self):
        print("SubportQoSGroupTest()")
        if (self.client.is_feature_enable(SWITCH_FEATURE_INGRESS_QOS_MAP) == 0):
            print("Ingress qos mapping feature not enabled, skipping")
            return

        qos_map1 = self.add_qos_map(self.device, dscp=1, tc=20)
        dscp_tc_maps = []
        dscp_tc_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=qos_map1))
        self.dscp_tc_map_ingress = self.add_qos_map_ingress(self.device,
            type=SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_DSCP_TO_TC, qos_map_list=dscp_tc_maps)
        qos_map5 = self.add_qos_map(self.device, tc=20, dscp=9)
        tc_dscp_maps = []
        tc_dscp_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=qos_map5))
        self.tc_dscp_map_egress = self.add_qos_map_egress(self.device,
            type=SWITCH_QOS_MAP_EGRESS_ATTR_TYPE_TC_TO_DSCP, qos_map_list=tc_dscp_maps)

        try:
            print("pass packet w/ mapped dscp value 1 -> 9")
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='40.40.1.20',
                ip_id=105,
                ip_tos=4, # dscp 1
                ip_ttl=64,
                dl_vlan_enable=True,
                vlan_vid=100)
            exp_pkt_no_qos = simple_tcp_packet(
                eth_dst='00:33:33:33:12:00',
                eth_src='00:77:66:55:44:33',
                ip_dst='40.40.1.20',
                ip_id=105,
                ip_tos=4, # dscp 1
                ip_ttl=63,
                dl_vlan_enable=True,
                vlan_vid=200)
            exp_pkt = simple_tcp_packet(
                eth_dst='00:33:33:33:12:00',
                eth_src='00:77:66:55:44:33',
                ip_dst='40.40.1.20',
                ip_id=105,
                ip_tos=36, # dscp 9
                ip_ttl=63,
                dl_vlan_enable=True,
                vlan_vid=200)

            print("No binding, packet should be forwarded unmodified")
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt_no_qos, self.devports[1])
            self.subport0_100_icntr += 1
            self.subport1_200_ecntr += 1

            print("Bind ingress qos group to port0 and egress qos group to port1")
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_QOS_DSCP_TOS_GROUP, self.dscp_tc_map_ingress)
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_L3_QOS_GROUP, self.tc_dscp_map_egress)
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt, self.devports[1])
            self.subport0_100_icntr += 1
            self.subport1_200_ecntr += 1

            print("Remove binding, packet should be forwarded")
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_QOS_DSCP_TOS_GROUP, 0)
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_L3_QOS_GROUP, 0)
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt_no_qos, self.devports[1])
            self.subport0_100_icntr += 1
            self.subport1_200_ecntr += 1
        finally:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_QOS_DSCP_TOS_GROUP, 0)
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_L3_QOS_GROUP, 0)
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

    def SubPortScaleTest(self):
        print("SubPortScaleTest()")
        vlan_id = 100
        cnt = 256
        lag1 = self.add_lag(self.device)
        self.add_lag_member(self.device, lag_handle=lag1, port_handle=self.port9)
        if (self.client.is_feature_enable(SWITCH_FEATURE_MULTIPLE_RIFS_PER_PORT) == 1):
            cnt = 128
        try:
            for i in range(0, cnt):
                self.add_rif(self.device,
                    type=SWITCH_RIF_ATTR_TYPE_SUB_PORT,
                    port_handle=self.port7,
                    outer_vlan_id=(vlan_id+i),
                    vrf_handle=self.vrf10,
                    src_mac=self.rmac)
                assert(self.status() == 0)
            for i in range(0, cnt):
                self.add_rif(self.device,
                    type=SWITCH_RIF_ATTR_TYPE_SUB_PORT,
                    port_handle=lag1,
                    outer_vlan_id=(vlan_id+i),
                    vrf_handle=self.vrf10,
                    src_mac=self.rmac)
                assert(self.status() == 0)
        finally:
            for _ in range(0, cnt*2):
                self.cleanlast()
            self.cleanlast()
            self.cleanlast()

    def RifStatsTest(self):
        print("RifStatsTest()")
        time.sleep(2)
        self.rif0_end = self.client.object_counters_get(self.rif0)
        self.rif1_end = self.client.object_counters_get(self.rif1)
        self.rif2_end = self.client.object_counters_get(self.rif2)
        self.rif3_end = self.client.object_counters_get(self.rif3)
        self.rif4_end = self.client.object_counters_get(self.rif4)
        self.sp0_100_end = self.client.object_counters_get(self.subport0_100)
        self.sp0_200_end = self.client.object_counters_get(self.subport0_200)
        self.sp1_200_end = self.client.object_counters_get(self.subport1_200)
        self.sp1_300_end = self.client.object_counters_get(self.subport1_300)
        self.sl0_400_end = self.client.object_counters_get(self.sublag0_400)
        self.sl0_500_end = self.client.object_counters_get(self.sublag0_500)
        self.sp3_600_end = self.client.object_counters_get(self.subport3_600)
        self.sp4_400_end = self.client.object_counters_get(self.subport4_400)
        self.sp4_500_end = self.client.object_counters_get(self.subport4_500)
        # 0 is SWITCH_RIF_COUNTER_ID_IN_UCAST_PKTS
        # 6 is SWITCH_RIF_COUNTER_ID_OUT_UCAST_PKTS
        print("IN_UCAST  RIF0 expected: %d counted: %d" % (self.rif0_end[0].count, self.rif0_icntr))
        print("OUT_UCAST RIF0 expected: %d counted: %d" % (self.rif0_end[6].count, self.rif0_ecntr))
        print("IN_UCAST  RIF1 expected: %d counted: %d" % (self.rif1_end[0].count, self.rif1_icntr))
        print("OUT_UCAST RIF1 expected: %d counted: %d" % (self.rif1_end[6].count, self.rif1_ecntr))
        print("IN_UCAST  RIF2 expected: %d counted: %d" % (self.rif2_end[0].count, self.rif2_icntr))
        print("OUT_UCAST RIF2 expected: %d counted: %d" % (self.rif2_end[6].count, self.rif2_ecntr))
        print("IN_UCAST  RIF3 expected: %d counted: %d" % (self.rif3_end[0].count, self.rif3_icntr))
        print("OUT_UCAST RIF3 expected: %d counted: %d" % (self.rif3_end[6].count, self.rif3_ecntr))
        print("IN_UCAST  RIF4 expected: %d counted: %d" % (self.rif4_end[0].count, self.rif4_icntr))
        print("OUT_UCAST RIF4 expected: %d counted: %d" % (self.rif4_end[6].count, self.rif4_ecntr))
        print("IN_UCAST  subport0.100 expected: %d counted: %d" % (self.sp0_100_end[0].count, self.subport0_100_icntr))
        print("OUT_UCAST subport0.100 expected: %d counted: %d" % (self.sp0_100_end[6].count, self.subport0_100_ecntr))
        print("IN_UCAST  subport0.200 expected: %d counted: %d" % (self.sp0_200_end[0].count, self.subport0_200_icntr))
        print("OUT_UCAST subport0.200 expected: %d counted: %d" % (self.sp0_200_end[6].count, self.subport0_200_ecntr))
        print("IN_UCAST  subport1.200 expected: %d counted: %d" % (self.sp1_200_end[0].count, self.subport1_200_icntr))
        print("OUT_UCAST subport1.200 expected: %d counted: %d" % (self.sp1_200_end[6].count, self.subport1_200_ecntr))
        print("IN_UCAST  subport1.300 expected: %d counted: %d" % (self.sp1_300_end[0].count, self.subport1_300_icntr))
        print("OUT_UCAST subport1.300 expected: %d counted: %d" % (self.sp1_300_end[6].count, self.subport1_300_ecntr))
        print("IN_UCAST   sublag0.400 expected: %d counted: %d" % (self.sl0_400_end[0].count, self.sublag0_400_icntr))
        print("OUT_UCAST  sublag0.400 expected: %d counted: %d" % (self.sl0_400_end[6].count, self.sublag0_400_ecntr))
        print("IN_UCAST   sublag0.500 expected: %d counted: %d" % (self.sl0_500_end[0].count, self.sublag0_500_icntr))
        print("OUT_UCAST  sublag0.500 expected: %d counted: %d" % (self.sl0_500_end[6].count, self.sublag0_500_ecntr))
        print("IN_UCAST  subport3.600 expected: %d counted: %d" % (self.sp3_600_end[0].count, self.subport3_600_icntr))
        print("OUT_UCAST subport3.600 expected: %d counted: %d" % (self.sp3_600_end[6].count, self.subport3_600_ecntr))
        print("IN_UCAST  subport4.400 expected: %d counted: %d" % (self.sp4_400_end[0].count, self.subport4_400_icntr))
        print("OUT_UCAST subport4.400 expected: %d counted: %d" % (self.sp4_400_end[6].count, self.subport4_400_ecntr))
        print("IN_UCAST  subport4.500 expected: %d counted: %d" % (self.sp4_500_end[0].count, self.subport4_500_icntr))
        print("OUT_UCAST subport4.500 expected: %d counted: %d" % (self.sp4_500_end[6].count, self.subport4_500_ecntr))
        self.assertTrue((self.rif0_end[0].count - self.rif0_start[0].count) == self.rif0_icntr)
        self.assertTrue((self.rif0_end[6].count - self.rif0_start[6].count) == self.rif0_ecntr)
        self.assertTrue((self.rif1_end[0].count - self.rif1_start[0].count) == self.rif1_icntr)
        self.assertTrue((self.rif1_end[6].count - self.rif1_start[6].count) == self.rif1_ecntr)
        self.assertTrue((self.rif2_end[0].count - self.rif2_start[0].count) == self.rif2_icntr)
        self.assertTrue((self.rif2_end[6].count - self.rif2_start[6].count) == self.rif2_ecntr)
        self.assertTrue((self.rif3_end[0].count - self.rif3_start[0].count) == self.rif3_icntr)
        self.assertTrue((self.rif3_end[6].count - self.rif3_start[6].count) == self.rif3_ecntr)
        self.assertTrue((self.rif4_end[0].count - self.rif4_start[0].count) == self.rif4_icntr)
        self.assertTrue((self.rif4_end[6].count - self.rif4_start[6].count) == self.rif4_ecntr)
        self.assertTrue((self.sp0_100_end[0].count - self.sp0_100_start[0].count) == self.subport0_100_icntr)
        self.assertTrue((self.sp0_100_end[6].count - self.sp0_100_start[6].count) == self.subport0_100_ecntr)
        self.assertTrue((self.sp0_200_end[0].count - self.sp0_200_start[0].count) == self.subport0_200_icntr)
        self.assertTrue((self.sp0_200_end[6].count - self.sp0_200_start[6].count) == self.subport0_200_ecntr)
        self.assertTrue((self.sp1_200_end[0].count - self.sp1_200_start[0].count) == self.subport1_200_icntr)
        self.assertTrue((self.sp1_200_end[6].count - self.sp1_200_start[6].count) == self.subport1_200_ecntr)
        self.assertTrue((self.sp1_300_end[0].count - self.sp1_300_start[0].count) == self.subport1_300_icntr)
        self.assertTrue((self.sp1_300_end[6].count - self.sp1_300_start[6].count) == self.subport1_300_ecntr)
        self.assertTrue((self.sl0_400_end[0].count - self.sl0_400_start[0].count) == self.sublag0_400_icntr)
        self.assertTrue((self.sl0_400_end[6].count - self.sl0_400_start[6].count) == self.sublag0_400_ecntr)
        self.assertTrue((self.sl0_500_end[0].count - self.sl0_500_start[0].count) == self.sublag0_500_icntr)
        self.assertTrue((self.sl0_500_end[6].count - self.sl0_500_start[6].count) == self.sublag0_500_ecntr)
        self.assertTrue((self.sp3_600_end[0].count - self.sp3_600_start[0].count) == self.subport3_600_icntr)
        self.assertTrue((self.sp3_600_end[6].count - self.sp3_600_start[6].count) == self.subport3_600_ecntr)
        self.assertTrue((self.sp4_400_end[0].count - self.sp4_400_start[0].count) == self.subport4_400_icntr)
        self.assertTrue((self.sp4_400_end[6].count - self.sp4_400_start[6].count) == self.subport4_400_ecntr)
        self.assertTrue((self.sp4_500_end[0].count - self.sp4_500_start[0].count) == self.subport4_500_icntr)
        self.assertTrue((self.sp4_500_end[6].count - self.sp4_500_start[6].count) == self.subport4_500_ecntr)

###############################################################################

@group('l3')
class L3MultipleSVITest(ApiHelper):
    mac01 = "00:11:22:33:44:01"
    mac02 = "00:11:22:33:44:02"
    mac03 = "00:11:22:33:44:03"
    def setUp(self):
        print()
        self.configure()
        self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_LEARNING, False)

        #### PORT ####
        # Create rif0 with port0
        rif0 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port0, vrf_handle=self.vrf10, src_mac=self.rmac)
        nhop0 = self.add_nexthop(self.device, handle=rif0, dest_ip='11.11.0.2')
        self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=rif0, dest_ip='11.11.0.2')
        self.add_route(self.device, ip_prefix='11.11.11.1', vrf_handle=self.vrf10, nexthop_handle=nhop0)

        self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port1)
        self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port2)
        self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port3)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.attribute_set(self.port2, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.attribute_set(self.port3, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)

        # Create rif1 with vlan10
        self.svi1 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_VLAN, vlan_handle=self.vlan10, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.svi2 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_VLAN, vlan_handle=self.vlan10, vrf_handle=self.vrf10, src_mac=self.mac01)
        self.svi3 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_VLAN, vlan_handle=self.vlan10, vrf_handle=self.vrf10, src_mac=self.mac02)

        nhop1 = self.add_nexthop(self.device, handle=self.svi1, dest_ip='10.10.0.1')
        self.add_neighbor(self.device, mac_address='00:22:22:33:44:55', handle=self.svi1, dest_ip='10.10.0.1')
        self.add_route(self.device, ip_prefix='10.10.10.1', vrf_handle=self.vrf10, nexthop_handle=nhop1)
        self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:22:22:33:44:55', destination_handle=self.port1)

    def runTest(self):
        try:
            self.BasicTest()
            self.SequencingTest()
        finally:
            pass

    def BasicTest(self):
        print("BasicTest")
        try:
            for dp in [self.devports[1], self.devports[2], self.devports[3]]:
                for dmac in [self.rmac, self.mac01, self.mac02]:
                    pkt = simple_tcp_packet(
                        eth_dst=dmac,
                        ip_dst='11.11.11.1',
                        ip_ttl=64)
                    exp_pkt = simple_tcp_packet(
                        eth_dst='00:11:22:33:44:55',
                        eth_src='00:77:66:55:44:33',
                        ip_dst='11.11.11.1',
                        ip_ttl=63)
                    print("Sending %s packet on port %d, forward to port %d" % (dmac, dp, self.devports[0]))
                    send_packet(self, dp, pkt)
                    verify_packet(self, exp_pkt, self.devports[0])

            from_pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                ip_dst='10.10.10.1',
                ip_ttl=64)
            exp_from_pkt = simple_tcp_packet(
                eth_dst='00:22:22:33:44:55',
                eth_src='00:77:66:55:44:33',
                ip_dst='10.10.10.1',
                ip_ttl=63)
            print("Sending packet on port %d, forward to port %d" % (self.devports[0], self.devports[1]))
            send_packet(self, self.devports[0], from_pkt)
            verify_packet(self, exp_from_pkt, self.devports[1])
        finally:
            pass

    def SequencingTest(self):
        print("BasicTest")
        pkt = simple_tcp_packet(
            eth_dst=self.mac03,
            ip_dst='11.11.11.1',
            ip_ttl=64)
        exp_pkt = simple_tcp_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src='00:77:66:55:44:33',
            ip_dst='11.11.11.1',
            ip_ttl=63)
        try:
            self.BasicTest()
            print("svi created before vlan_member")
            self.svi4 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_VLAN, vlan_handle=self.vlan10, vrf_handle=self.vrf10, src_mac=self.mac03)
            print("Add new svi, test existing ports")
            self.BasicTest()
            self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port4)
            self.attribute_set(self.port4, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
            print("Add new vlan_member, test existing ports")
            self.BasicTest()

            print("Sending %s packet on port %d, forward to port %d" % (self.mac03, self.devports[4], self.devports[0]))
            send_packet(self, self.devports[4], pkt)
            verify_packet(self, exp_pkt, self.devports[0])

            # cleanup
            self.attribute_set(self.port4, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.cleanlast()
            print("Delete vlan member, test existing ports")
            self.BasicTest()
            print("Delete svi, test existing ports")
            self.cleanlast()
            self.BasicTest()

            print("vlan_member created before svi")
            self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port4)
            self.attribute_set(self.port4, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
            print("Add new vlan_member, test existing ports")
            self.BasicTest()
            self.svi4 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_VLAN, vlan_handle=self.vlan10, vrf_handle=self.vrf10, src_mac=self.mac03)
            print("Add new svi, test existing ports")
            self.BasicTest()

            print("Sending %s packet on port %d, forward to port %d" % (self.mac03, self.devports[4], self.devports[0]))
            send_packet(self, self.devports[4], pkt)
            verify_packet(self, exp_pkt, self.devports[0])

            # cleanup
            self.attribute_set(self.port4, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.cleanlast()
            print("Delete svi, test existing ports")
            self.BasicTest()
            self.cleanlast()
            print("Delete vlan member, test existing ports")
            self.BasicTest()
        finally:
            pass

    def tearDown(self):
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.port2, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.port3, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.cleanup()

###############################################################################

@group('l3')
class L3SVITest(ApiHelper):
    def setUp(self):
        print()
        self.configure()

        #### PORT ####
        # Create rif0 with port0
        rif0 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port0, vrf_handle=self.vrf10, src_mac=self.rmac)

        vlan_mbr1 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port1)
        vlan_mbr2 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port2)
        vlan_mbr4 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port4)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.attribute_set(self.port2, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.attribute_set(self.port4, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)

        # Create rif1 with vlan10
        self.rif1 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_VLAN, vlan_handle=self.vlan10, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.rif10_ig_ucast = 0
        self.rif10_eg_ucast = 0
        self.rif10_ig_bcast = 0
        self.rif10_eg_bcast = 0

        # Create nhop1, nhop2 & nhop3 on SVI
        nhop1 = self.add_nexthop(self.device, handle=self.rif1, dest_ip='10.10.0.1')
        self.neighbor1 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=self.rif1, dest_ip='10.10.0.1')
        route1 = self.add_route(self.device, ip_prefix='10.10.10.1', vrf_handle=self.vrf10, nexthop_handle=nhop1)

        nhop2 = self.add_nexthop(self.device, handle=self.rif1, dest_ip='10.10.0.2')
        self.neighbor2 = self.add_neighbor(self.device, mac_address='00:22:22:33:44:55', handle=self.rif1, dest_ip='10.10.0.2')
        route2 = self.add_route(self.device, ip_prefix='10.10.10.2', vrf_handle=self.vrf10, nexthop_handle=nhop2)

        nhop3 = self.add_nexthop(self.device, handle=self.rif1, dest_ip='10.10.0.3')
        self.neighbor3 = self.add_neighbor(self.device, mac_address='00:33:22:33:44:55', handle=self.rif1, dest_ip='10.10.0.3')
        route3 = self.add_route(self.device, ip_prefix='10.10.10.3', vrf_handle=self.vrf10, nexthop_handle=nhop3)

        # add directed broadcast IP on vlan 10
        nhop_bcast = self.add_nexthop(self.device, handle=self.rif1, dest_ip='10.10.1.255')
        self.add_neighbor(self.device, mac_address='FF:FF:FF:FF:FF:FF', handle=self.rif1, dest_ip='10.10.1.255')
        route3 = self.add_route(self.device, ip_prefix='10.10.1.255', vrf_handle=self.vrf10, nexthop_handle=nhop_bcast)

        # Create nhop and route to L3 intf
        nhop4 = self.add_nexthop(self.device, handle=rif0, dest_ip='11.11.0.2')
        neighbor4 = self.add_neighbor(self.device, mac_address='00:44:22:33:44:55', handle=rif0, dest_ip='11.11.0.2')
        route4 = self.add_route(self.device, ip_prefix='11.11.11.1', vrf_handle=self.vrf10, nexthop_handle=nhop4)
        v6route5 = self.add_route(self.device, ip_prefix='1234:5678:9abc:def0:4422:1133:5577:1111', vrf_handle=self.vrf10, nexthop_handle=nhop4)

        #### LAG ####
        # Create rif5 with port5
        rif5 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port5, vrf_handle=self.vrf10, src_mac=self.rmac)

        # Create lag0 with port6 and port7
        self.lag0 = self.add_lag(self.device)
        lag_mbr0 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port6)
        lag_mbr1 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port7)

        # Create lag1 with port8 and port9
        self.lag1 = self.add_lag(self.device)
        lag_mbr4 = self.add_lag_member(self.device, lag_handle=self.lag1, port_handle=self.port8)
        lag_mbr5 = self.add_lag_member(self.device, lag_handle=self.lag1, port_handle=self.port9)

        # Create rif_vlan20 with vlan20
        self.rif_vlan20 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_VLAN, vlan_handle=self.vlan20, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.rif20_ig_ucast = 0
        self.rif20_eg_ucast = 0
        self.rif20_ig_bcast = 0
        self.rif20_eg_bcast = 0
        vlan_mbr_lag0 = self.add_vlan_member(self.device, vlan_handle=self.vlan20, member_handle=self.lag0)
        vlan_mbr_lag1 = self.add_vlan_member(self.device, vlan_handle=self.vlan20, member_handle=self.lag1, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
        self.attribute_set(self.lag0, SWITCH_LAG_ATTR_PORT_VLAN_ID, 20)
        self.attribute_set(self.lag1, SWITCH_LAG_ATTR_PORT_VLAN_ID, 20)

        # Create nhop5, nhop6 & on SVI
        nhop5 = self.add_nexthop(self.device, handle=self.rif_vlan20, dest_ip='20.10.0.1')
        neighbor5 = self.add_neighbor(self.device, mac_address='00:11:33:33:44:55', handle=self.rif_vlan20, dest_ip='20.10.0.1')
        route5 = self.add_route(self.device, ip_prefix='20.10.10.1', vrf_handle=self.vrf10, nexthop_handle=nhop5)
        v6route5 = self.add_route(self.device, ip_prefix='1234:5678:9abc:def0:4422:1133:5577:99aa', vrf_handle=self.vrf10, nexthop_handle=nhop5)

        # Set route to lag
        nhop6 = self.add_nexthop(self.device, handle=self.rif_vlan20, dest_ip='20.10.0.2')
        neighbor6 = self.add_neighbor(self.device, mac_address='00:22:33:33:44:55', handle=self.rif_vlan20, dest_ip='20.10.0.2')
        route6 = self.add_route(self.device, ip_prefix='20.10.10.2', vrf_handle=self.vrf10, nexthop_handle=nhop6)
        v6route6 = self.add_route(self.device, ip_prefix='1234:5678:9abc:def0:1122:3344:5566:7788', vrf_handle=self.vrf10, nexthop_handle=nhop6)

        # Create nhop and route to L3 intf
        nhop7 = self.add_nexthop(self.device, handle=rif5, dest_ip='21.11.0.2')
        neighbor7 = self.add_neighbor(self.device, mac_address='00:44:33:33:44:55', handle=rif5, dest_ip='21.11.0.2')
        route7 = self.add_route(self.device, ip_prefix='21.11.11.1', vrf_handle=self.vrf10, nexthop_handle=nhop7)
        v6route7 = self.add_route(self.device, ip_prefix='1234:5678:9abc:def0:1122:3344:5566:6677', vrf_handle=self.vrf10, nexthop_handle=nhop7)

        if (self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_COPP) == 1):
            queue_handles = self.attribute_get(self.cpu_port_hdl, SWITCH_PORT_ATTR_QUEUE_HANDLES)
            self.hostif_trap_group = self.add_hostif_trap_group(self.device,
                queue_handle=queue_handles[0].oid,
                admin_state=True)
            self.add_hostif_trap(self.device,
                type=SWITCH_HOSTIF_TRAP_ATTR_TYPE_L3_MTU_ERROR,
                hostif_trap_group_handle=self.hostif_trap_group,
                packet_action=SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_REDIRECT_TO_CPU)

        self.rif10_start = self.client.object_counters_get(self.rif1)
        self.rif20_start = self.client.object_counters_get(self.rif_vlan20)

    def runTest(self):
        try:
            self.SVIRifVrfUpdateTest()
            self.SVIRifIPv4DisableTest()
            self.SVIRifIPv6DisableTest()
            self.SVIRifRmacUpdateTest()
            self.SVIHostIngressVlanZeroTest()
            self.SVIBridgingTest()
            self.SVIHostTest()
            self.SVILpmMissTest()
            self.SVIHostVlanTaggingTest()
            self.SVIGleanTest()
            self.SVIHostPostRoutedGleanTest()
            self.SVIDirBcastTest()
            self.SVIHostStaticMacMoveTest()
            self.SVIRouteDynamicMacTest()
            self.SVIArpMoveTest()
            self.SVIRouteDynamicMacMoveTest()
            self.SVIRouteNeighborMacTest()
            self.SVIHostIngressVlanMemberTest()
            self.IPv4SVILagHostTest()
            self.IPv4SVILagHostDynamicMacTest()
            self.IPv6SVILagHostTest()
            self.SVIMtuTest()
            self.SVIStpTest()
            self.SVIRifStatsTest()
        finally:
            pass

    def tearDown(self):
        self.client.object_flush_all(SWITCH_OBJECT_TYPE_MAC_ENTRY)
        self.attribute_set(self.lag0, SWITCH_LAG_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.lag1, SWITCH_LAG_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.port2, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.port4, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.cleanup()

    def SVIRifVrfUpdateTest(self):
        print("RiVrfUpdateeTest")
        pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='11.11.11.1',
            ip_ttl=64)
        exp_pkt = simple_tcp_packet(
            eth_dst='00:44:22:33:44:55',
            eth_src='00:77:66:55:44:33',
            ip_dst='11.11.11.1',
            ip_ttl=63)
        exp_pkt2 = simple_tcp_packet(
            eth_dst='00:44:44:33:44:55',
            eth_src='00:77:66:55:44:33',
            ip_dst='11.11.11.1',
            ip_ttl=63)

        vrf20 = self.add_vrf(self.device, id=20, src_mac=self.rmac)
        rif2 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port10, vrf_handle=vrf20, src_mac=self.rmac)
        nhop10 = self.add_nexthop(self.device, handle=rif2, dest_ip='10.10.0.4')
        self.add_neighbor(self.device, mac_address='00:44:44:33:44:55', handle=rif2, dest_ip='10.10.0.4')
        self.add_route(self.device, ip_prefix='11.11.11.1', vrf_handle=vrf20, nexthop_handle=nhop10)
        try:
            print("Sending packet on port %d, forward to port %d" % (self.devports[1], self.devports[0]))
            send_packet(self, self.devports[1], pkt)
            verify_packet(self, exp_pkt, self.devports[0])
            self.rif10_ig_ucast += 1

            print("Update VRF on ingress RIF to vrf20")
            self.attribute_set(self.rif1, SWITCH_RIF_ATTR_VRF_HANDLE, vrf20)
            print("Sending packet on port %d, forward to port %d" % (self.devports[0], self.devports[10]))
            send_packet(self, self.devports[1], pkt)
            verify_packet(self, exp_pkt2, self.devports[10])
            self.rif10_ig_ucast += 1

            print("Update VRF on ingress RIF to vrf10")
            self.attribute_set(self.rif1, SWITCH_RIF_ATTR_VRF_HANDLE, self.vrf10)
            print("Sending packet on port %d, forward to port %d" % (self.devports[0], self.devports[0]))
            send_packet(self, self.devports[1], pkt)
            verify_packet(self, exp_pkt, self.devports[0])
            self.rif10_ig_ucast += 1
        finally:
            self.attribute_set(self.rif1, SWITCH_RIF_ATTR_VRF_HANDLE, self.vrf10)
            # clean temp objects
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

    def SVIHostIngressVlanZeroTest(self):
        print()
        print("SVIHostIngressVlanZeroTest()")
        print("Test routing after NHOP resolved via static MAC entry")
        # Add static MAC on if1 and if2
        mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:11:22:33:44:55', destination_handle=self.port1)
        mac1 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:22:22:33:44:55', destination_handle=self.port2)
        pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:33:44:55',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            dl_vlan_enable=True,
            vlan_vid=0,
            ip_ttl=64,
            pktlen=104)
        exp_pkt = simple_tcp_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src='00:77:66:55:44:33',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63,
            pktlen=100)
        try:
            print("Sending packet port %d to port %d, (192.168.0.1 -> 10.10.10.1) Routed" % (self.devports[2], self.devports[1]))
            send_packet(self, self.devports[2], pkt)
            verify_packet(self, exp_pkt, self.devports[1])
            self.rif10_ig_ucast += 1
            self.rif10_eg_ucast += 1
        finally:
            self.cleanlast()
            self.cleanlast()
            pass

    def SVIRifIPv4DisableTest(self):
        print()
        print("SVIRifIPv4DisableTest()")
        pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='11.11.11.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64,
            pktlen=100)
        exp_pkt = simple_tcp_packet(
            eth_dst='00:44:22:33:44:55',
            eth_src='00:77:66:55:44:33',
            ip_dst='11.11.11.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63,
            pktlen=100)
        vlan_pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='11.11.11.1',
            ip_src='192.168.0.1',
            dl_vlan_enable=True,
            vlan_vid=10,
            ip_id=105,
            ip_ttl=64,
            pktlen=104)

        try:
            print("Sending packet on port %d, Routed" % self.devports[1])
            send_packet(self, self.devports[1], pkt)
            verify_packet(self, exp_pkt, self.devports[0])
            self.rif10_ig_ucast += 1
            send_packet(self, self.devports[2], pkt)
            verify_packet(self, exp_pkt, self.devports[0])
            self.rif10_ig_ucast += 1
            send_packet(self, self.devports[4], pkt)
            verify_packet(self, exp_pkt, self.devports[0])
            self.rif10_ig_ucast += 1

            print("Disable IPv4 on ingress RIF")
            self.attribute_set(self.rif1, SWITCH_RIF_ATTR_IPV4_UNICAST, False)
            print("Sending packet on port %d, dropped" % self.devports[1])
            send_packet(self, self.devports[1], pkt)
            verify_no_other_packets(self, timeout=1)
            self.rif10_ig_ucast += 1
            send_packet(self, self.devports[2], pkt)
            verify_no_other_packets(self, timeout=1)
            self.rif10_ig_ucast += 1
            send_packet(self, self.devports[4], pkt)
            verify_no_other_packets(self, timeout=1)
            self.rif10_ig_ucast += 1
            print("Verify setting for new vlan member")
            vlan_mbr3 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port3, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
            send_packet(self, self.devports[3], vlan_pkt)
            verify_no_other_packets(self, timeout=1)
            self.rif10_ig_ucast += 1
            self.cleanlast()

            print("Enable IPv4 on ingress RIF")
            self.attribute_set(self.rif1, SWITCH_RIF_ATTR_IPV4_UNICAST, True)
            print("Sending packet on port %d, Routed" % self.devports[1])
            send_packet(self, self.devports[1], pkt)
            verify_packet(self, exp_pkt, self.devports[0])
            self.rif10_ig_ucast += 1
            send_packet(self, self.devports[2], pkt)
            verify_packet(self, exp_pkt, self.devports[0])
            self.rif10_ig_ucast += 1
            send_packet(self, self.devports[4], pkt)
            verify_packet(self, exp_pkt, self.devports[0])
            self.rif10_ig_ucast += 1
            print("Verify setting for new vlan member")
            vlan_mbr3 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port3, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
            send_packet(self, self.devports[3], vlan_pkt)
            verify_packet(self, exp_pkt, self.devports[0])
            self.rif10_ig_ucast += 1
            self.cleanlast()
        finally:
            self.attribute_set(self.rif1, SWITCH_RIF_ATTR_IPV4_UNICAST, True)
            pass

    def SVIRifIPv6DisableTest(self):
        print()
        print("SVIRifIPv6DisableTest()")
        pkt = simple_tcpv6_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:1111',
            ipv6_src='2000::1',
            ipv6_hlim=64,
            pktlen=100)
        vlan_pkt = simple_tcpv6_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:1111',
            ipv6_src='2000::1',
            dl_vlan_enable=True,
            vlan_vid=10,
            ipv6_hlim=64,
            pktlen=104)
        exp_pkt = simple_tcpv6_packet(
            eth_dst='00:44:22:33:44:55',
            eth_src='00:77:66:55:44:33',
            ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:1111',
            ipv6_src='2000::1',
            ipv6_hlim=63,
            pktlen=100)

        try:
            print("Sending packet on port %d, Routed" % self.devports[1])
            send_packet(self, self.devports[1], pkt)
            verify_packet(self, exp_pkt, self.devports[0])
            self.rif10_ig_ucast += 1

            print("Disable IPv6 on ingress RIF")
            self.attribute_set(self.rif1, SWITCH_RIF_ATTR_IPV6_UNICAST, False)
            print("Sending packet on port %d, dropped" % self.devports[1])
            send_packet(self, self.devports[1], pkt)
            verify_no_other_packets(self, timeout=1)
            self.rif10_ig_ucast += 1
            print("Verify setting for new vlan member")
            vlan_mbr3 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port3, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
            send_packet(self, self.devports[3], vlan_pkt)
            verify_no_other_packets(self, timeout=1)
            self.rif10_ig_ucast += 1
            self.cleanlast()

            print("Enable IPv6 on ingress RIF")
            self.attribute_set(self.rif1, SWITCH_RIF_ATTR_IPV6_UNICAST, True)
            print("Sending packet on port %d, Routed" % self.devports[1])
            send_packet(self, self.devports[1], pkt)
            verify_packet(self, exp_pkt, self.devports[0])
            self.rif10_ig_ucast += 1
            print("Verify setting for new vlan member")
            vlan_mbr3 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port3, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
            send_packet(self, self.devports[3], vlan_pkt)
            verify_packet(self, exp_pkt, self.devports[0])
            self.rif10_ig_ucast += 1
            self.cleanlast()
        finally:
            self.attribute_set(self.rif1, SWITCH_RIF_ATTR_IPV6_UNICAST, True)
            pass

    def SVIRifRmacUpdateTest(self):
        print()
        print("SVIRifRmacUpdateTest")
        self.new_rmac = '00:77:66:55:44:44'
        pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            ip_dst='11.11.11.1',
            ip_ttl=64)
        pkt1 = simple_tcp_packet(
            eth_dst=self.new_rmac,
            ip_dst='11.11.11.1',
            ip_ttl=64)
        exp_pkt = simple_tcp_packet(
            eth_dst='00:44:22:33:44:55',
            eth_src='00:77:66:55:44:33',
            ip_dst='11.11.11.1',
            ip_ttl=63)
        try:
            print("Sending packet on port %d with rmac 00:77:66:55:44:33, forward" % self.devports[1])
            send_packet(self, self.devports[1], pkt)
            verify_packet(self, exp_pkt, self.devports[0])
            self.rif10_ig_ucast += 1

            print("Update ingress rif rmac")
            self.attribute_set(self.rif1, SWITCH_RIF_ATTR_SRC_MAC, self.new_rmac)
            print("Sending packet on port %d with rmac 00:77:66:55:44:33, flood" % self.devports[1])
            send_packet(self, self.devports[1], pkt)
            verify_packets(self, pkt, [self.devports[2], self.devports[4]])
            self.rif10_ig_ucast += 1
            self.rif10_eg_ucast += 2

            print("Sending packet on port %d with rmac 00:77:66:55:44:44, forward" % self.devports[2])
            send_packet(self, self.devports[1], pkt1)
            verify_packet(self, exp_pkt, self.devports[0])
            self.rif10_ig_ucast += 1

            print("Revert ingress rif rmac")
            self.attribute_set(self.rif1, SWITCH_RIF_ATTR_SRC_MAC, self.rmac)
            print("Sending packet on port %d with rmac 00:77:66:55:44:44, flood" % self.devports[1])
            send_packet(self, self.devports[1], pkt1)
            verify_packets(self, pkt1, [self.devports[2], self.devports[4]])
            self.rif10_ig_ucast += 1
            self.rif10_eg_ucast += 2

            print("Sending packet on port %d with rmac 00:77:66:55:44:33, forward" % self.devports[1])
            send_packet(self, self.devports[1], pkt)
            verify_packet(self, exp_pkt, self.devports[0])
            self.rif10_ig_ucast += 1

        finally:
            self.attribute_set(self.rif1, SWITCH_RIF_ATTR_SRC_MAC, self.rmac)

    def SVIBridgingTest(self):
        print()
        print("SVIBridgingTest()")
        print("Test L2 bridging on an SVI port if rmac miss")
        self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_LEARNING, False)
        pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='11.11.11.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        exp_pkt = simple_tcp_packet(
            eth_dst='00:44:22:33:44:55',
            eth_src='00:77:66:55:44:33',
            ip_dst='11.11.11.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63)
        pkt2 = simple_tcp_packet(
            eth_dst='00:77:66:55:44:44',
            eth_src='00:22:22:22:22:22',
            ip_dst='11.11.11.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)

        try:
            print("Sending packet port %d to port %d, (192.168.0.1 -> 11.11.11.1) Routed" % (self.devports[1], self.devports[0]))
            send_packet(self, self.devports[1], pkt)
            verify_packet(self, exp_pkt, self.devports[0])
            self.rif10_ig_ucast += 1
            print("Sending packet port %d to port %d, (192.168.0.1 -> 11.11.11.1) Routed" % (self.devports[2], self.devports[0]))
            send_packet(self, self.devports[2], pkt)
            verify_packet(self, exp_pkt, self.devports[0])
            self.rif10_ig_ucast += 1
            print("Sending packet port %d to port %d, (192.168.0.1 -> 11.11.11.1) Routed" % (self.devports[4], self.devports[0]))
            send_packet(self, self.devports[4], pkt)
            verify_packet(self, exp_pkt, self.devports[0])
            self.rif10_ig_ucast += 1
            print("Sending packet port %d, (192.168.0.1 -> 11.11.11.1) flood" % (self.devports[1]))
            send_packet(self, self.devports[1], pkt2)
            verify_packets(self, pkt2, [self.devports[2], self.devports[4]])
            self.rif10_ig_ucast += 1
            self.rif10_eg_ucast += 2
            print("Sending packet port %d, (192.168.0.1 -> 11.11.11.1) flood" % (self.devports[2]))
            send_packet(self, self.devports[2], pkt2)
            verify_packets(self, pkt2, [self.devports[1], self.devports[4]])
            self.rif10_ig_ucast += 1
            self.rif10_eg_ucast += 2
            print("Sending packet port %d, (192.168.0.1 -> 11.11.11.1) flood" % (self.devports[4]))
            send_packet(self, self.devports[4], pkt2)
            verify_packets(self, pkt2, [self.devports[1], self.devports[2]])
            self.rif10_ig_ucast += 1
            self.rif10_eg_ucast += 2
            mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:77:66:55:44:44', destination_handle=self.port1)
            print("Sending packet port %d to port %d, (192.168.0.1 -> 11.11.11.1), bridged" % (self.devports[2], self.devports[1]))
            send_packet(self, self.devports[2], pkt2)
            verify_packet(self, pkt2, self.devports[1])
            self.rif10_ig_ucast += 1
            self.rif10_eg_ucast += 1
            print("Sending packet port %d to port %d, (192.168.0.1 -> 11.11.11.1), bridged" % (self.devports[4], self.devports[1]))
            send_packet(self, self.devports[4], pkt2)
            verify_packet(self, pkt2, self.devports[1])
            self.rif10_ig_ucast += 1
            self.rif10_eg_ucast += 1
        finally:
            self.cleanlast()
            self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_LEARNING, True)
            pass

    def SVIHostTest(self):
        print()
        print("SVIHostTest()")
        print("Test routing after NHOP resolved via static MAC entry")
        # Add static MAC on if1 and if2
        mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:11:22:33:44:55', destination_handle=self.port1)
        mac1 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:22:22:33:44:55', destination_handle=self.port2)
        pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        exp_pkt = simple_tcp_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src='00:77:66:55:44:33',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63)
        pkt1 = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.10.10.2',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        exp_pkt1 = simple_tcp_packet(
            eth_dst='00:22:22:33:44:55',
            eth_src='00:77:66:55:44:33',
            ip_dst='10.10.10.2',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63)
        pkt2 = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='11.11.11.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        exp_pkt2 = simple_tcp_packet(
            eth_dst='00:44:22:33:44:55',
            eth_src='00:77:66:55:44:33',
            ip_dst='11.11.11.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63)

        try:
            print("Sending packet port %d to port %d, (192.168.0.1 -> 10.10.10.1) Routed" % (self.devports[0], self.devports[1]))
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt, self.devports[1])
            self.rif10_eg_ucast += 1
            print("Sending packet port %d to port %d, (192.168.0.1 -> 10.10.10.2) Routed" % (self.devports[0], self.devports[2]))
            send_packet(self, self.devports[0], pkt1)
            verify_packet(self, exp_pkt1, self.devports[2])
            self.rif10_eg_ucast += 1
            print("Sending packet port %d to port %d, (192.168.0.1 -> 11.11.11.1) Routed" % (self.devports[1], self.devports[0]))
            send_packet(self, self.devports[1], pkt2)
            verify_packet(self, exp_pkt2, self.devports[0])
            self.rif10_ig_ucast += 1
        finally:
            self.cleanlast()
            self.cleanlast()
            pass


    def SVILpmMissTest(self):
        print()
        print("SVILpmMissTest()")
        print("Test lpm drop")
        pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            ip_dst='11.11.11.1',
            ip_ttl=64)
        exp_pkt = simple_tcp_packet(
            eth_dst='00:44:22:33:44:55',
            eth_src='00:77:66:55:44:33',
            ip_dst='11.11.11.1',
            ip_ttl=63)
        pkt1 = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            ip_dst='21.21.21.1',
            ip_ttl=64)
        try:
            print("Sending packet port %d to port %d, (192.168.0.1 -> 11.11.11.1) Routed" % (self.devports[1], self.devports[0]))
            send_packet(self, self.devports[1], pkt)
            verify_packet(self, exp_pkt, self.devports[0])
            self.rif10_ig_ucast += 1
            print("Sending packet port %d, drop (192.168.0.1 -> 21.21.21.1) Routed" % (self.devports[1]))
            send_packet(self, self.devports[1], pkt1)
            verify_no_other_packets(self, timeout=1)
            self.rif10_ig_ucast += 1
        finally:
            pass

    def SVIHostVlanTaggingTest(self):
        print()
        print("SVIHostVlanTaggingTest()")
        print("Test routing after NHOP resolved via static MAC entry")
        vlan_mbr3 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port3, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
        mac3 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:33:22:33:44:55', destination_handle=self.port3)
        pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.10.10.3',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        exp_pkt = simple_tcp_packet(
            eth_dst='00:33:22:33:44:55',
            eth_src='00:77:66:55:44:33',
            ip_dst='10.10.10.3',
            ip_src='192.168.0.1',
            dl_vlan_enable=True,
            vlan_vid=10,
            ip_id=105,
            ip_ttl=63,
            pktlen=104)

        try:
            print("Sending packet port %d to port %d, (192.168.0.1 -> 10.10.10.1) Routed, tagged" % (self.devports[0], self.devports[3]))
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt, self.devports[3])
            self.rif10_eg_ucast += 1
        finally:
            self.cleanlast()
            self.cleanlast()
            pass

    def SVIGleanTest(self):
        print()
        print("SVIGleanTest()")
        print("Test glean when route miss")
        vlan_mbr3 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port3, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
        mac3 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:33:22:33:44:55', destination_handle=self.port3)
        pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.10.10.4',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        tag_pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.10.10.5',
            ip_src='192.168.0.1',
            dl_vlan_enable=True,
            vlan_vid=10,
            ip_id=105,
            ip_ttl=64)

        try:
            print("Sending packet from port %d, (192.168.0.1 -> 10.10.10.4) route miss, drop no glean" % (self.devports[4]))
            send_packet(self, self.devports[4], pkt)
            verify_no_other_packets(self, timeout=1)
            self.rif10_ig_ucast += 1
            print("Sending packet from port %d, (192.168.0.1 -> 10.10.10.5) route miss, drop no glean" % (self.devports[3]))
            send_packet(self, self.devports[3], tag_pkt)
            verify_no_other_packets(self, timeout=1)
            self.rif10_ig_ucast += 1

            print("Add glean entry 10.10.0.0/16")
            self.nhop_glean = self.add_nexthop(self.device, type=SWITCH_NEXTHOP_ATTR_TYPE_GLEAN)
            self.route_glean = self.add_route(self.device, ip_prefix='10.10.0.0/16', vrf_handle=self.vrf10, nexthop_handle=self.nhop_glean)

            cpu_pkt = simple_cpu_packet(
                packet_type=0,
                ingress_port=swport_to_devport(self, self.devports[4]),
                ingress_ifindex=(self.port4 & 0xFFFF),
                reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_MAX + SWITCH_HOSTIF_TRAP_ATTR_INTERNAL_TRAP_GLEAN,
                ingress_bd=0xa,
                inner_pkt=pkt)
            cpu_pkt = cpu_packet_mask_ingress_bd(cpu_pkt)
            print("Sending packet from port %d, (192.168.0.1 -> 10.10.10.4) route, glean" % (self.devports[4]))
            if self.test_params['target'] != 'hw':
                send_packet(self, self.devports[4], pkt)
                verify_packet(self, cpu_pkt, self.cpu_port)
                self.rif10_ig_ucast += 1
                self.rif10_eg_ucast += 1
            cpu_pkt = simple_cpu_packet(
                packet_type=0,
                ingress_port=swport_to_devport(self, self.devports[3]),
                ingress_ifindex=(self.port3 & 0xFFFF),
                reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_MAX + SWITCH_HOSTIF_TRAP_ATTR_INTERNAL_TRAP_GLEAN,
                ingress_bd=0xa,
                inner_pkt=tag_pkt)
            cpu_pkt = cpu_packet_mask_ingress_bd(cpu_pkt)
            print("Sending tag packet from port %d, (192.168.0.1 -> 10.10.10.5) route, glean" % (self.devports[3]))
            if self.test_params['target'] != 'hw':
                send_packet(self, self.devports[3], tag_pkt)
                verify_packet(self, cpu_pkt, self.cpu_port)
                self.rif10_ig_ucast += 1
                self.rif10_eg_ucast += 1
        finally:
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            pass

    def SVIHostPostRoutedGleanTest(self):
        print()
        print("SVIHostPostRoutedGleanTest()")
        print("Test glean when static mac entry is missing")
        pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        exp_pkt = simple_tcp_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src='00:77:66:55:44:33',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63)
        cpu_pkt = simple_cpu_packet(
            packet_type=0,
            ingress_port=swport_to_devport(self, self.devports[0]),
            ingress_ifindex=(self.port0 & 0xFFFF),
            reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_MAX + SWITCH_HOSTIF_TRAP_ATTR_INTERNAL_TRAP_GLEAN,
            ingress_bd=0x0,
            inner_pkt=pkt)
        cpu_pkt = cpu_packet_mask_ingress_bd(cpu_pkt)

        try:
            print("Sending packet from port %d, (192.168.0.1 -> 10.10.10.1) glean" % (self.devports[0]))
            send_packet(self, self.devports[0], pkt)
            verify_packets(self, cpu_pkt, [self.cpu_port])
            mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:11:22:33:44:55', destination_handle=self.port1)
            print("Install static mac on port %d, should not glean now" % self.devports[1])
            print("Sending packet port %d to port %d, (192.168.0.1 -> 10.10.10.1) Routed, tagged" % (self.devports[0], self.devports[1]))
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt, self.devports[1])
            self.rif10_eg_ucast += 1
            self.cleanlast()
            print("Removed static mac and not packet will be gleaned after routed")
            print("Sending packet from port %d, (192.168.0.1 -> 10.10.10.1) glean" % (self.devports[0]))
            send_packet(self, self.devports[0], pkt)
            verify_packets(self, cpu_pkt, [self.cpu_port])
        finally:
            pass

    def SVIDirBcastTest(self):
        print()
        print("SVIDirBcastTest()")
        print("Test flood for dir bcast")
        pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.10.1.255',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        exp_pkt = simple_tcp_packet(
            eth_dst='FF:FF:FF:FF:FF:FF',
            eth_src='00:77:66:55:44:33',
            ip_dst='10.10.1.255',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63)

        try:
            print("Sending packet from port %d, (192.168.0.1 -> 10.10.10.1) glean" % (self.devports[0]))
            send_packet(self, self.devports[0], pkt)
            verify_packets(self, exp_pkt, [self.devports[1], self.devports[2], self.devports[4]])
            self.rif10_eg_ucast += 3
        finally:
            pass

    def SVIHostStaticMacMoveTest(self):
        print()
        print("SVIHostStaticMacMoveTest()")
        print("Test routing after NHOP resolved via static MAC move")
        # Add static MAC on if1 and if2
        mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:11:22:33:44:55', destination_handle=self.port1)
        pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        exp_pkt = simple_tcp_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src='00:77:66:55:44:33',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63)
        try:
            print("MAC installed on port%d" % (self.devports[1]))
            print("Sending packet port %d to port %d, (192.168.0.1 -> 10.10.10.1) Routed" % (self.devports[0], self.devports[1]))
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt, self.devports[1])
            self.rif10_eg_ucast += 1
            print("Move MAC to port%d from port%d" % (self.devports[2], self.devports[1]))
            self.attribute_set(mac0, SWITCH_MAC_ENTRY_ATTR_DESTINATION_HANDLE, self.port2)
            print("Sending packet port %d to port %d, (192.168.0.1 -> 10.10.10.1) Routed" % (self.devports[0], self.devports[2]))
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt, self.devports[2])
            self.rif10_eg_ucast += 1
        finally:
            self.cleanlast()
            pass

    def SVIRouteDynamicMacTest(self):
        print()
        print("SVIRouteDynamicMacTest()")
        print("Test routing after NHOP resolved via dynamically learn MAC entry")
        self.attribute_set(self.port2, SWITCH_PORT_ATTR_LEARNING, True)
        self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_LEARNING, True)
        arp_pkt = simple_arp_packet(
            eth_dst='FF:FF:FF:FF:FF:FF',
            eth_src='00:22:22:33:44:55',
            arp_op=1, #ARP request
            ip_tgt='10.10.10.1',
            ip_snd='10.10.10.2',
            hw_snd='00:22:22:33:44:55',
            pktlen=100)
        pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.10.10.2',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        exp_pkt = simple_tcp_packet(
            eth_dst='00:22:22:33:44:55',
            eth_src='00:77:66:55:44:33',
            ip_dst='10.10.10.2',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63)

        try:
            # Src MAC learnt on port 2 resulting in nhop2 resolution
            print("Sending ARP request on port %d" % self.devports[2], " (10.10.10.2 -> 10.10.10.1) Flood on %d %d " % (self.devports[1], self.devports[4]))
            send_packet(self, self.devports[2], arp_pkt)
            verify_packets(self, arp_pkt, [self.devports[1], self.devports[4]])
            self.rif10_ig_bcast += 1
            self.rif10_eg_bcast += 2
            print("Nexthop 2 resolved on port %d " % self.devports[2], "00:22:22:33:44:55")
            time.sleep(3)

            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 10.10.10.2) Routed")
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt, self.devports[2])
            self.rif10_eg_ucast += 1
        finally:
            self.client.object_flush_all(SWITCH_OBJECT_TYPE_MAC_ENTRY)
            self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_LEARNING, False)
            self.attribute_set(self.port2, SWITCH_PORT_ATTR_LEARNING, False)
            pass

    def SVIRouteDynamicMacMoveTest(self):
        print()
        print("SVIRouteDynamicMacMoveTest()")
        print("Test routing after NHOP resolved via dynamically learn MAC entry")
        self.attribute_set(self.port2, SWITCH_PORT_ATTR_LEARNING, True)
        self.attribute_set(self.port4, SWITCH_PORT_ATTR_LEARNING, True)
        self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_LEARNING, True)
        arp_pkt = simple_arp_packet(
            eth_dst='FF:FF:FF:FF:FF:FF',
            eth_src='00:22:22:33:44:55',
            arp_op=1, #ARP request
            ip_tgt='10.10.10.1',
            ip_snd='10.10.10.2',
            hw_snd='00:22:22:33:44:55',
            pktlen=100)
        pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.10.10.2',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        exp_pkt = simple_tcp_packet(
            eth_dst='00:22:22:33:44:55',
            eth_src='00:77:66:55:44:33',
            ip_dst='10.10.10.2',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63)

        try:
            # Src MAC learnt on port 2 resulting in nhop2 resolution
            print("Sending ARP request on port %d" % self.devports[2], " (10.10.10.2 -> 10.10.10.1) Flood on %d %d " % (self.devports[1], self.devports[4]))
            send_packet(self, self.devports[2], arp_pkt)
            verify_packets(self, arp_pkt, [self.devports[1], self.devports[4]])
            self.rif10_ig_bcast += 1
            self.rif10_eg_bcast += 2
            print("Nexthop 2 resolved on port %d " % self.devports[2], "00:22:22:33:44:55")
            time.sleep(3)

            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 10.10.10.2) Routed")
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt, self.devports[2])
            self.rif10_eg_ucast += 1

            # Src MAC move to port 4
            print("Mac move, Sending ARP request on port %d" % self.devports[4])
            send_packet(self, self.devports[4], arp_pkt)
            verify_packets(self, arp_pkt, [self.devports[1], self.devports[2]])
            self.rif10_ig_bcast += 1
            self.rif10_eg_bcast += 2

            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                4], " (192.168.0.1 -> 10.10.10.2) Routed")
            pkt[IP].dst = '10.10.10.2'
            exp_pkt[IP].dst = '10.10.10.2'
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt, self.devports[4])
            self.rif10_eg_ucast += 1
        finally:
            self.client.object_flush_all(SWITCH_OBJECT_TYPE_MAC_ENTRY)
            self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_LEARNING, False)
            self.attribute_set(self.port2, SWITCH_PORT_ATTR_LEARNING, False)
            self.attribute_set(self.port4, SWITCH_PORT_ATTR_LEARNING, False)
            pass


    def SVIArpMoveTest(self):
        print()
        print("SVIArpMoveTest()")
        print("Test routing after NHOP resolved via dynamically learn MAC entry")
        self.attribute_set(self.port2, SWITCH_PORT_ATTR_LEARNING, True)
        self.attribute_set(self.port4, SWITCH_PORT_ATTR_LEARNING, True)
        self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_LEARNING, True)
        self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_AGING_INTERVAL, 30000)
        arp1_pkt = simple_arp_packet(
            eth_dst='FF:FF:FF:FF:FF:FF',
            eth_src='00:22:22:33:44:55',
            arp_op=1, #ARP request
            ip_tgt='10.10.10.1',
            ip_snd='10.10.10.2',
            hw_snd='00:22:22:33:44:55',
            pktlen=100)
        arp2_pkt = simple_arp_packet(
            eth_dst='FF:FF:FF:FF:FF:FF',
            eth_src='00:22:22:33:44:66',
            arp_op=1, #ARP request
            ip_tgt='10.10.10.1',
            ip_snd='10.10.10.2',
            hw_snd='00:22:22:33:44:66',
            pktlen=100)
        pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.10.10.2',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        exp1_pkt = simple_tcp_packet(
            eth_dst='00:22:22:33:44:55',
            eth_src='00:77:66:55:44:33',
            ip_dst='10.10.10.2',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63)
        exp2_pkt = simple_tcp_packet(
            eth_dst='00:22:22:33:44:66',
            eth_src='00:77:66:55:44:33',
            ip_dst='10.10.10.2',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63)

        try:
            # Learn 00:22:22:33:44:55
            print("Sending ARP request on port %d" % self.devports[2], " (10.10.10.2 -> 10.10.10.1) Flood on %d %d " % (self.devports[1], self.devports[4]))
            send_packet(self, self.devports[2], arp1_pkt)
            verify_packets(self, arp1_pkt, [self.devports[1], self.devports[4]])
            self.rif10_ig_bcast += 1
            self.rif10_eg_bcast += 2
            time.sleep(3)

            # Learn 00:22:22:33:44:66
            print("Sending ARP request on port %d" % self.devports[4], " (10.10.10.2 -> 10.10.10.1) Flood on %d %d " % (self.devports[1], self.devports[2]))
            send_packet(self, self.devports[4], arp2_pkt)
            verify_packets(self, arp2_pkt, [self.devports[1], self.devports[2]])
            self.rif10_ig_bcast += 1
            self.rif10_eg_bcast += 2
            time.sleep(3)

            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 10.10.10.2) Routed")
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp1_pkt, self.devports[2])
            self.rif10_eg_ucast += 1

            print("Update neighbor to 00:22:22:33:44:66")
            self.attribute_set(self.neighbor2, SWITCH_NEIGHBOR_ATTR_MAC_ADDRESS, "00:22:22:33:44:66")
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                4], " (192.168.0.1 -> 10.10.10.2) Routed")
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp2_pkt, self.devports[4])
            self.rif10_eg_ucast += 1

            print("Update neighbor back to 00:22:22:33:44:55")
            self.attribute_set(self.neighbor2, SWITCH_NEIGHBOR_ATTR_MAC_ADDRESS, "00:22:22:33:44:55")
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                2], " (192.168.0.1 -> 10.10.10.2) Routed")
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp1_pkt, self.devports[2])
            self.rif10_eg_ucast += 1
        finally:
            self.attribute_set(self.neighbor2, SWITCH_NEIGHBOR_ATTR_MAC_ADDRESS, "00:22:22:33:44:55")
            self.client.object_flush_all(SWITCH_OBJECT_TYPE_MAC_ENTRY)
            self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_LEARNING, False)
            self.attribute_set(self.port2, SWITCH_PORT_ATTR_LEARNING, False)
            self.attribute_set(self.port4, SWITCH_PORT_ATTR_LEARNING, False)
            pass

    def SVIRouteNeighborMacTest(self):
        print()
        print("SVIRouteNeighborMacTest()")
        return
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_LEARNING, True)
        self.attribute_set(self.port2, SWITCH_PORT_ATTR_LEARNING, True)
        self.attribute_set(self.port4, SWITCH_PORT_ATTR_LEARNING, True)
        self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_LEARNING, True)
        self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_AGING_INTERVAL, 40000)
        ip_pre = '20.20.'
        mac_pre = '00:22:22:22:'
        count = 11
        # create nexthops
        for port in [self.devports[1], self.devports[2], self.devports[4]]:
            for mac_offset in range(1, count):
                nhop_ip = ip_pre + str(port) + '.' + str(mac_offset)
                nh = self.add_nexthop(self.device, handle=self.rif1, dest_ip=nhop_ip)
                self.add_route(self.device, ip_prefix=nhop_ip, vrf_handle=self.vrf10, nexthop_handle=nh)
        # learn macs
        for port in [self.devports[1], self.devports[2], self.devports[4]]:
            for mac_offset in range(1, count):
                src_mac = mac_pre + hex(port) + ':' + hex(mac_offset)
                arp_pkt = simple_arp_packet(
                    eth_dst='FF:FF:FF:FF:FF:FF',
                    eth_src=src_mac,
                    arp_op=1, #ARP request
                    ip_tgt='12.12.12.1',
                    ip_snd='12.12.12.2',
                    hw_snd='00:22:22:33:44:55',
                    pktlen=120)
                send_packet(self, port, arp_pkt)
                self.rif10_ig_bcast += 1
                self.rif10_eg_bcast += 2
        # give it a couple of seconds before the model starts learning
        time.sleep(2)
        # add neighbors
        # if there were to be a deadlock, it would be in this loop
        for port in [self.devports[1], self.devports[2], self.devports[4]]:
            for mac_offset in range(1, count):
                src_mac = mac_pre + hex(port) + ':' + hex(mac_offset)
                neigh_ip = ip_pre + str(port) + '.' + str(mac_offset)
                self.add_neighbor(self.device, mac_address=src_mac, handle=self.rif1, dest_ip=neigh_ip)

        print("Sleep for 20")
        time.sleep(20)
        try:
            for port in [self.devports[1], self.devports[2], self.devports[4]]:
                for mac_offset in range(1, 5):
                    neigh_mac = mac_pre + hex(port) + ':' + hex(mac_offset)
                    nhop_ip = ip_pre + str(port) + '.' + str(mac_offset)
                    pkt = simple_tcp_packet(
                        eth_dst='00:77:66:55:44:33',
                        eth_src='00:22:22:22:22:22',
                        ip_dst=nhop_ip,
                        ip_src='192.168.0.1',
                        ip_id=105,
                        ip_ttl=64)
                    exp_pkt = simple_tcp_packet(
                        eth_src='00:77:66:55:44:33',
                        eth_dst=neigh_mac,
                        ip_dst=nhop_ip,
                        ip_src='192.168.0.1',
                        ip_id=105,
                        ip_ttl=63)
                    print("Sending pkt DIP %s from %d -> %d" % (nhop_ip, self.devports[0], port))
                    send_packet(self, self.devports[0], pkt)
                    verify_packets(self, exp_pkt, [port])
                    self.rif10_eg_ucast += 1
        finally:
            self.client.object_flush_all(SWITCH_OBJECT_TYPE_MAC_ENTRY)
            for port in [self.devports[1], self.devports[2], self.devports[4]]:
                for mac_offset in range(1, count):
                    self.cleanlast()
                    self.cleanlast()
                    self.cleanlast()
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_LEARNING, False)
            self.attribute_set(self.port2, SWITCH_PORT_ATTR_LEARNING, False)
            self.attribute_set(self.port4, SWITCH_PORT_ATTR_LEARNING, False)
            self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_LEARNING, False)

    def SVIHostIngressVlanMemberTest(self):
        print()
        print("SVIHostIngressVlanMemberTest()")
        print("Test routing when packet ingress on member of SVI vlan")
        pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:33:44:55',
            ip_dst='10.10.10.1',
            ip_src='10.10.10.2',
            ip_id=105,
            ip_ttl=64)
        exp_pkt = simple_tcp_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src='00:77:66:55:44:33',
            ip_dst='10.10.10.1',
            ip_src='10.10.10.2',
            ip_id=105,
            ip_ttl=63)

        try:
            # Packet routed to nhop1 already resolved
            print("Sending known dst IP packet on port %d" % self.devports[2], " -> port %d" % self.devports[
                1], " (10.10.10.2 -> 10.10.10.1) Routed")
            #send_packet(self, self.devports[2], pkt)
            #verify_packet(self, exp_pkt, self.devports[1])
        finally:
            pass

    def IPv4SVILagHostTest(self):
        print()
        print("IPv4SVILagHostTest()")
        print("Test routing after NHOP resolved via static MAC entry")

        # Add static MAC on lag0 and lag1, port1
        mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan20, mac_address='00:11:33:33:44:55', destination_handle=self.lag0)
        mac1 = self.add_mac_entry(self.device, vlan_handle=self.vlan20, mac_address='00:22:33:33:44:55', destination_handle=self.lag1)
        mac1 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:11:22:33:44:55', destination_handle=self.port1)

        pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='20.10.10.1',
            ip_src='192.168.0.1',
            ip_ttl=64)
        exp_pkt = simple_tcp_packet(
            eth_dst='00:11:33:33:44:55',
            eth_src='00:77:66:55:44:33',
            ip_dst='20.10.10.1',
            ip_src='192.168.0.1',
            ip_ttl=63)
        pkt1 = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='20.10.10.2',
            ip_src='192.168.0.1',
            ip_ttl=64,
            pktlen=100)
        exp_pkt1 = simple_tcp_packet(
            eth_dst='00:22:33:33:44:55',
            eth_src='00:77:66:55:44:33',
            ip_dst='20.10.10.2',
            ip_src='192.168.0.1',
            dl_vlan_enable=True,
            vlan_vid=20,
            ip_ttl=63,
            pktlen=104)
        pkt2 = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='21.11.11.1',
            ip_src='192.168.0.1',
            ip_ttl=64)
        exp_pkt2 = simple_tcp_packet(
            eth_dst='00:44:33:33:44:55',
            eth_src='00:77:66:55:44:33',
            ip_dst='21.11.11.1',
            ip_src='192.168.0.1',
            ip_ttl=63)
        pkt3 = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_ttl=64,
            pktlen=100)
        tagged_pkt3 = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_ttl=64,
            dl_vlan_enable=True,
            vlan_vid=20,
            pktlen=104)
        exp_pkt3 = simple_tcp_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src='00:77:66:55:44:33',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_ttl=63,
            pktlen=100)

        try:
            print("Sending packet port %d to lag 0, (192.168.0.1 -> 20.10.10.1) Routed" % (self.devports[5]))
            send_packet(self, self.devports[5], pkt)
            verify_any_packet_any_port(self, [exp_pkt] * 2, [self.devports[6], self.devports[7]])
            self.rif20_eg_ucast += 1
            print("Sending packet port %d to lag 1, (192.168.0.1 -> 20.10.10.2) Routed" % (self.devports[5]))
            send_packet(self, self.devports[5], pkt1)
            verify_any_packet_any_port(self, [exp_pkt1] * 2, [self.devports[8], self.devports[9]])
            self.rif20_eg_ucast += 1
            print("Sending packet port %d to port %d, (192.168.0.1 -> 21.11.11.1) Routed" % (self.devports[6], self.devports[5]))
            send_packet(self, self.devports[6], pkt2)
            verify_packet(self, exp_pkt2, self.devports[5])
            self.rif20_ig_ucast += 1
            print("Sending packet port %d to port %d, (192.168.0.1 -> 21.11.11.1) Routed" % (self.devports[7], self.devports[5]))
            send_packet(self, self.devports[7], pkt2)
            verify_packet(self, exp_pkt2, self.devports[5])
            self.rif20_ig_ucast += 1

            # inter SVI routing
            print("Sending packet vlan10/port %d to vlan20/lag0, (192.168.0.1 -> 20.10.10.1) Routed" % (self.devports[1]))
            send_packet(self, self.devports[1], pkt)
            verify_any_packet_any_port(self, [exp_pkt] * 2, [self.devports[6], self.devports[7]])
            self.rif10_ig_ucast += 1
            self.rif20_eg_ucast += 1
            print("Sending packet vlan10/port %d to vlan20/lag1, (192.168.0.1 -> 20.10.10.2) Routed" % (self.devports[2]))
            send_packet(self, self.devports[2], pkt1)
            verify_any_packet_any_port(self, [exp_pkt1] * 2, [self.devports[8], self.devports[9]])
            self.rif10_ig_ucast += 1
            self.rif20_eg_ucast += 1
            print("Sending packet vlan20/port %d to vlan10/port %d, (192.168.0.1 -> 10.10.10.1) Routed" % (self.devports[6], self.devports[1]))
            send_packet(self, self.devports[6], pkt3)
            verify_packet(self, exp_pkt3, self.devports[1])
            self.rif20_ig_ucast += 1
            self.rif10_eg_ucast += 1
            print("Sending packet vlan20/port %d to vlan10/port %d, (192.168.0.1 -> 10.10.10.1) Routed" % (self.devports[7], self.devports[1]))
            send_packet(self, self.devports[7], pkt3)
            verify_packet(self, exp_pkt3, self.devports[1])
            self.rif20_ig_ucast += 1
            self.rif10_eg_ucast += 1
            print("Sending packet vlan20/port %d to vlan10/port %d, (192.168.0.1 -> 10.10.10.1) Routed" % (self.devports[8], self.devports[1]))
            send_packet(self, self.devports[8], tagged_pkt3)
            verify_packet(self, exp_pkt3, self.devports[1])
            self.rif20_ig_ucast += 1
            self.rif10_eg_ucast += 1
            print("Sending packet vlan20/port %d to vlan10/port %d, (192.168.0.1 -> 10.10.10.1) Routed" % (self.devports[9], self.devports[1]))
            send_packet(self, self.devports[9], tagged_pkt3)
            verify_packet(self, exp_pkt3, self.devports[1])
            self.rif20_ig_ucast += 1
            self.rif10_eg_ucast += 1
        finally:
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            pass

    def IPv4SVILagHostDynamicMacTest(self):
        print()
        print("IPv4SVILagHostDynamicMacTest()")
        print("Test routing after NHOP resolved via dynamically learn MAC entry")
        self.attribute_set(self.lag0, SWITCH_LAG_ATTR_LEARNING, True)
        self.attribute_set(self.vlan20, SWITCH_VLAN_ATTR_LEARNING, True)
        arp_pkt = simple_arp_packet(
            eth_dst='FF:FF:FF:FF:FF:FF',
            eth_src='00:22:33:33:44:55',
            arp_op=1, #ARP request
            ip_tgt='20.10.10.1',
            ip_snd='20.10.10.2',
            hw_snd='00:22:22:33:44:55',
            pktlen=100)
        tagged_arp_pkt = simple_arp_packet(
            eth_dst='FF:FF:FF:FF:FF:FF',
            eth_src='00:22:33:33:44:55',
            arp_op=1, #ARP request
            vlan_vid=20,
            ip_tgt='20.10.10.1',
            ip_snd='20.10.10.2',
            hw_snd='00:22:22:33:44:55',
            pktlen=104)
        pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='20.10.10.2',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        exp_pkt = simple_tcp_packet(
            eth_dst='00:22:33:33:44:55',
            eth_src='00:77:66:55:44:33',
            ip_dst='20.10.10.2',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63)

        try:
            # Src MAC learnt on port 6 resulting in nhop2 resolution
            print("Sending ARP request on port %d" % self.devports[6],
            " (10.10.10.2 -> 20.10.10.1) Flood on lag1(member ports: %d, %d )" % (self.devports[8], self.devports[9]))
            send_packet(self, self.devports[6], arp_pkt)
            verify_any_packet_any_port(self, [tagged_arp_pkt] * 2, [self.devports[8], self.devports[9]])
            self.rif20_ig_bcast += 1
            self.rif20_eg_bcast += 1
            print("Nexthop 2 resolved on lag 0 ", "00:22:33:33:44:55")
            time.sleep(3)

            print("Sending packet port %d" % self.devports[5], " -> lag 0(member ports: %d, %d)" % (self.devports[6], self.devports[7]),
            " (192.168.0.1 -> 20.10.10.2) Routed")
            send_packet(self, self.devports[5], pkt)
            verify_any_packet_any_port(self, [exp_pkt] * 2, [self.devports[6], self.devports[7]])
            self.rif20_eg_ucast += 1
        finally:
            self.client.object_flush_all(SWITCH_OBJECT_TYPE_MAC_ENTRY)
            self.attribute_set(self.vlan20, SWITCH_VLAN_ATTR_LEARNING, False)
            self.attribute_set(self.lag0, SWITCH_LAG_ATTR_LEARNING, False)
            pass

    def IPv6SVILagHostTest(self):
        print()
        print("IPv6SVILagHostTest()")
        print("Test routing after NHOP resolved via static MAC entry")

        # Add static MAC on lag0 and lag1
        mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan20, mac_address='00:11:33:33:44:55', destination_handle=self.lag0)
        mac1 = self.add_mac_entry(self.device, vlan_handle=self.vlan20, mac_address='00:22:33:33:44:55', destination_handle=self.lag1)

        pkt = simple_tcpv6_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
            ipv6_src='2000::1',
            ipv6_hlim=64)
        exp_pkt = simple_tcpv6_packet(
            eth_dst='00:11:33:33:44:55',
            eth_src='00:77:66:55:44:33',
            ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
            ipv6_src='2000::1',
            ipv6_hlim=63)
        pkt1 = simple_tcpv6_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ipv6_dst='1234:5678:9abc:def0:1122:3344:5566:7788',
            ipv6_src='2000::1',
            ipv6_hlim=64,
            pktlen=100)
        exp_pkt1 = simple_tcpv6_packet(
            eth_dst='00:22:33:33:44:55',
            eth_src='00:77:66:55:44:33',
            ipv6_dst='1234:5678:9abc:def0:1122:3344:5566:7788',
            ipv6_src='2000::1',
            dl_vlan_enable=True,
            vlan_vid=20,
            ipv6_hlim=63,
            pktlen=104)
        pkt2 = simple_tcpv6_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ipv6_dst='1234:5678:9abc:def0:1122:3344:5566:6677',
            ipv6_src='2000::1',
            ipv6_hlim=64)
        exp_pkt2 = simple_tcpv6_packet(
            eth_dst='00:44:33:33:44:55',
            eth_src='00:77:66:55:44:33',
            ipv6_dst='1234:5678:9abc:def0:1122:3344:5566:6677',
            ipv6_src='2000::1',
            ipv6_hlim=63)

        try:
            print("Sending packet port %d to lag 0, (2000::1 -> 1234:5678:9abc:def0:4422:1133:5577:99aa) Routed" % (self.devports[5]))
            send_packet(self, self.devports[5], pkt)
            verify_any_packet_any_port(self, [exp_pkt] * 2, [self.devports[6], self.devports[7]])
            self.rif20_eg_ucast += 1
            print("Sending packet port %d to lag 1, (2000::1 -> 1234:5678:9abc:def0:1122:3344:5566:7788) Routed" % (self.devports[5]))
            send_packet(self, self.devports[5], pkt1)
            verify_any_packet_any_port(self, [exp_pkt1] * 2, [self.devports[8], self.devports[9]])
            self.rif20_eg_ucast += 1
            print("Sending packet port %d to port %d, (2000::1 -> 1234:5678:9abc:def0:1122:3344:5566:6677) Routed" % (self.devports[6], self.devports[5]))
            send_packet(self, self.devports[6], pkt2)
            verify_packet(self, exp_pkt2, self.devports[5])
            self.rif20_ig_ucast += 1
            print("Sending packet port %d to port %d, (2000::1 -> 1234:5678:9abc:def0:1122:3344:5566:6677) Routed" % (self.devports[7], self.devports[5]))
            send_packet(self, self.devports[7], pkt2)
            verify_packet(self, exp_pkt2, self.devports[5])
            self.rif20_ig_ucast += 1
        finally:
            self.cleanlast()
            self.cleanlast()

    def SVIMtuTest(self):
        print("SVIMtuTest()")
        self.attribute_set(self.rif1, SWITCH_RIF_ATTR_MTU, 200)
        self.attribute_set(self.rif_vlan20, SWITCH_RIF_ATTR_MTU, 200)
        try:
            print("Max MTU is 200, send pkt size 200, send to SVI port/lag")
            mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:11:22:33:44:55', destination_handle=self.port1)
            mac1 = self.add_mac_entry(self.device, vlan_handle=self.vlan20, mac_address='00:11:33:33:44:55', destination_handle=self.lag0)
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
              eth_src='00:22:22:22:22:22',
              ip_dst='10.10.10.1',
              ip_src='192.168.0.1',
              ip_id=105,
              ip_ttl=64,
              pktlen=200+14)
            exp_pkt = simple_tcp_packet(
                eth_dst='00:11:22:33:44:55',
              eth_src='00:77:66:55:44:33',
              ip_dst='10.10.10.1',
              ip_src='192.168.0.1',
              ip_id=105,
              ip_ttl=63,
              pktlen=200+14)
            print("Sending packet port %d" % self.devports[0], " -> port %d" % self.devports[
                  1], " (192.168.0.1 -> 10.10.10.1)")
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt, self.devports[1])
            self.rif10_eg_ucast += 1
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
              eth_src='00:22:22:22:22:22',
              ip_dst='20.10.10.1',
              ip_src='192.168.0.1',
              ip_id=105,
              ip_ttl=64,
              pktlen=200+14)
            exp_pkt = simple_tcp_packet(
                eth_dst='00:11:33:33:44:55',
              eth_src='00:77:66:55:44:33',
              ip_dst='20.10.10.1',
              ip_src='192.168.0.1',
              ip_id=105,
              ip_ttl=63,
              pktlen=200+14)
            print("Sending packet port %d -> lag 0, (192.168.0.1 -> 20.10.10.1) " % (self.devports[0]))
            send_packet(self, self.devports[0], pkt)
            verify_any_packet_any_port(self, [exp_pkt] * 2, [self.devports[6], self.devports[7]])
            self.rif20_eg_ucast += 1

            print("Max MTU is 200, send pkt size 201, send to cpu")
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
              eth_src='00:22:22:22:22:22',
              ip_dst='10.10.10.1',
              ip_src='192.168.0.1',
              ip_id=105,
              ip_ttl=64,
              pktlen=201+14)
            exp_pkt = simple_tcp_packet(
                eth_dst='00:11:22:33:44:55',
              eth_src='00:77:66:55:44:33',
              ip_dst='10.10.10.1',
              ip_src='192.168.0.1',
              ip_id=105,
              ip_ttl=63,
              pktlen=201+14)
            cpu_pkt = simple_cpu_packet(
                  ingress_port=0,
                  packet_type=0,
                  ingress_ifindex=0,
                  reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_L3_MTU_ERROR,
                  ingress_bd=0x02,
                  inner_pkt=exp_pkt)
            cpu_pkt = cpu_packet_mask_ingress_bd(cpu_pkt)
            print("Sending packet port %d" % self.devports[0], " -> cpu (192.168.0.1 -> 10.10.10.1)")
            if (self.test_params['target'] != 'hw' and
                self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_COPP) == 1):
                send_packet(self, self.devports[0], pkt)
                verify_packet(self, cpu_pkt, self.cpu_port)
                self.rif10_eg_ucast += 1

            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
              eth_src='00:22:22:22:22:22',
              ip_dst='20.10.10.1',
              ip_src='192.168.0.1',
              ip_id=105,
              ip_ttl=64,
              pktlen=201+14)
            exp_pkt = simple_tcp_packet(
                eth_dst='00:11:33:33:44:55',
              eth_src='00:77:66:55:44:33',
              ip_dst='20.10.10.1',
              ip_src='192.168.0.1',
              ip_id=105,
              ip_ttl=63,
              pktlen=201+14)
            cpu_pkt = simple_cpu_packet(
                  ingress_port=0,
                  packet_type=0,
                  ingress_ifindex=0,
                  reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_L3_MTU_ERROR,
                  ingress_bd=0x02,
                  inner_pkt=exp_pkt)
            cpu_pkt = cpu_packet_mask_ingress_bd(cpu_pkt)
            print("Sending packet port %d" % self.devports[0], " -> cpu (192.168.0.1 -> 20.10.10.1)")
            if (self.test_params['target'] != 'hw' and
                self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_COPP) == 1):
                send_packet(self, self.devports[0], pkt)
                verify_packet(self, cpu_pkt, self.cpu_port)
                self.rif20_eg_ucast += 1
        finally:
            self.attribute_set(self.rif_vlan20, SWITCH_RIF_ATTR_MTU, 1514)
            self.attribute_set(self.rif1, SWITCH_RIF_ATTR_MTU, 1514)
            self.cleanlast()
            self.cleanlast()

    def SVIStpTest(self):
        print()
        print("SVIStpTest()")
        print("Test packet behavior with STP enabled")
        if (self.client.is_feature_enable(SWITCH_FEATURE_STP) == 0):
            print("STP feature not enabled, skipping")
            return

        stp = self.add_stp(self.device)
        self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_STP_HANDLE, stp)
        self.stp_port2 = self.add_stp_port(self.device, state=SWITCH_STP_PORT_ATTR_STATE_FORWARDING, stp_handle=stp, port_lag_handle=self.port2)

        queue_handles = self.attribute_get(self.cpu_port_hdl, SWITCH_PORT_ATTR_QUEUE_HANDLES)
        hostif_trap_group0 = self.add_hostif_trap_group(self.device, queue_handle=queue_handles[0].oid, admin_state=True)
        self.add_hostif_trap(self.device, type=SWITCH_HOSTIF_TRAP_ATTR_TYPE_ARP_REQUEST,
                             hostif_trap_group_handle=hostif_trap_group0,
                             packet_action=SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_COPY_TO_CPU,
                             priority=100)

        arp_pkt = simple_arp_packet(
            eth_dst='FF:FF:FF:FF:FF:FF',
            eth_src='00:22:22:33:44:55',
            arp_op=1, #ARP request
            pktlen=100)
        cpu_pkt = simple_cpu_packet(
            ingress_port=swport_to_devport(self, self.devports[2]),
            ingress_ifindex=(self.port2 & 0xFFFF),
            packet_type=0,
            reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_ARP_REQUEST,
            ingress_bd=0xa,
            inner_pkt=arp_pkt)

        try:
            print("Sending ARP request on port %d, Flood on %d %d " % (self.devports[2], self.devports[1], self.devports[4]))
            send_packet(self, self.devports[2], arp_pkt)
            if self.test_params['target'] != 'hw':
                verify_each_packet_on_each_port(self, [arp_pkt, arp_pkt, cpu_pkt], [self.devports[1], self.devports[4], self.cpu_port])
            else:
                verify_packets(self, arp_pkt, [self.devports[1], self.devports[4]])
            self.rif10_ig_bcast += 1
            self.rif10_eg_bcast += 3

            print("Port %d in stp blocking state" % (self.devports[2]))
            self.attribute_set(self.stp_port2, SWITCH_STP_PORT_ATTR_STATE, SWITCH_STP_PORT_ATTR_STATE_BLOCKING)
            print("Sending ARP request on port %d, drop " % (self.devports[2]))
            send_packet(self, self.devports[2], arp_pkt)
            verify_no_other_packets(self, timeout=1)
            self.rif10_ig_bcast += 1

            print("Port %d in stp forwarding state" % (self.devports[2]))
            self.attribute_set(self.stp_port2, SWITCH_STP_PORT_ATTR_STATE, SWITCH_STP_PORT_ATTR_STATE_FORWARDING)
            print("Sending ARP request on port %d, Flood on %d %d " % (self.devports[2], self.devports[1], self.devports[4]))
            send_packet(self, self.devports[2], arp_pkt)
            if self.test_params['target'] != 'hw':
                verify_each_packet_on_each_port(self, [arp_pkt, arp_pkt, cpu_pkt], [self.devports[1], self.devports[4], self.cpu_port])
            else:
                verify_packets(self, arp_pkt, [self.devports[1], self.devports[4]])
            self.rif10_ig_bcast += 1
            self.rif10_eg_bcast += 3
        finally:
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_STP_HANDLE, 0)
            self.cleanlast()

    def SVIRifStatsTest(self):
        print("SVIRifStatsTest()")
        time.sleep(4)
        self.rif10_end = self.client.object_counters_get(self.rif1)
        self.rif20_end = self.client.object_counters_get(self.rif_vlan20)
        # 0 is SWITCH_RIF_COUNTER_ID_IN_UCAST_PKTS
        # 2 is SWITCH_RIF_COUNTER_ID_IN_BCAST_PKTS
        # 6 is SWITCH_RIF_COUNTER_ID_OUT_UCAST_PKTS
        # 8 is SWITCH_RIF_COUNTER_ID_OUT_BCAST_PKTS
        print("IN_UCAST  vlan10 counted: %d expected: %d" % (self.rif10_end[0].count, self.rif10_ig_ucast))
        print("IN_BCAST  vlan10 counted: %d expected: %d" % (self.rif10_end[2].count, self.rif10_ig_bcast))
        print("OUT_UCAST vlan10 counted: %d expected: %d" % (self.rif10_end[6].count, self.rif10_eg_ucast))
        print("OUT_BCAST vlan10 counted: %d expected: %d" % (self.rif10_end[8].count, self.rif10_eg_bcast))
        print("IN_UCAST  vlan20 counted: %d expected: %d" % (self.rif20_end[0].count, self.rif20_ig_ucast))
        print("IN_BCAST  vlan20 counted: %d expected: %d" % (self.rif20_end[2].count, self.rif20_ig_bcast))
        print("OUT_UCAST vlan20 counted: %d expected: %d" % (self.rif20_end[6].count, self.rif20_eg_ucast))
        print("OUT_BCAST vlan20 counted: %d expected: %d" % (self.rif20_end[8].count, self.rif20_eg_bcast))
        self.assertTrue((self.rif10_end[0].count - self.rif10_start[0].count) == self.rif10_ig_ucast)
        self.assertTrue((self.rif10_end[2].count - self.rif10_start[2].count) == self.rif10_ig_bcast)
        self.assertTrue((self.rif10_end[6].count - self.rif10_start[6].count) == self.rif10_eg_ucast)
        self.assertTrue((self.rif10_end[8].count - self.rif10_start[8].count) == self.rif10_eg_bcast)
        self.assertTrue((self.rif20_end[0].count - self.rif20_start[0].count) == self.rif20_ig_ucast)
        self.assertTrue((self.rif20_end[2].count - self.rif20_start[2].count) == self.rif20_ig_bcast)
        self.assertTrue((self.rif20_end[6].count - self.rif20_start[6].count) == self.rif20_eg_ucast)
        self.assertTrue((self.rif20_end[8].count - self.rif20_start[8].count) == self.rif20_eg_bcast)

###############################################################################

@group('l3')
class L3SharedNeighborTest(ApiHelper):
    def setUp(self):
        print()
        self.configure()

        #### PORT ####
        # Create rif0 with port0
        rif0 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port0, vrf_handle=self.vrf10, src_mac=self.rmac)

        vlan_mbr1 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port1)
        vlan_mbr1 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port2)

        # Create rif with vlan10
        self.rif_vlan10 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_VLAN, vlan_handle=self.vlan10, vrf_handle=self.vrf10, src_mac=self.rmac)

        # Create nhop, nhopv6 on SVI
        nhop = self.add_nexthop(self.device, handle=self.rif_vlan10, dest_ip='10.10.0.1')
        neighbor = self.add_neighbor(self.device, mac_address='00:11:33:33:44:55', handle=self.rif_vlan10, dest_ip='10.10.0.1')
        nhopv6 = self.add_nexthop(self.device, handle=self.rif_vlan10, dest_ip='1234:5678:9abc:def0:4422:1133:5577:99aa')
        neighborv6 = self.add_neighbor(self.device, mac_address='00:11:33:33:44:55', handle=self.rif_vlan10, dest_ip='1234:5678:9abc:def0:4422:1133:5577:99aa')

        route = self.add_route(self.device, ip_prefix='11.11.0.1', vrf_handle=self.vrf10, nexthop_handle=nhop)
        routev6 = self.add_route(self.device, ip_prefix='1234:5678:9abc:def0:1122:3344:5566:6677', vrf_handle=self.vrf10, nexthop_handle=nhopv6)

    def runTest(self):
        pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            ip_dst='11.11.0.1',
            ip_ttl=64)
        exp_pkt = simple_tcp_packet(
            eth_dst='00:11:33:33:44:55',
            eth_src='00:77:66:55:44:33',
            ip_dst='11.11.0.1',
            ip_ttl=63)
        cpu_pkt = simple_cpu_packet(
            ingress_port=swport_to_devport(self, self.devports[0]),
            ingress_ifindex=(self.port0 & 0xFFFF),
            packet_type=0,
            reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_MAX + SWITCH_HOSTIF_TRAP_ATTR_INTERNAL_TRAP_GLEAN,
            ingress_bd=0x0,
            inner_pkt=pkt)
        cpu_pktv4 = cpu_packet_mask_ingress_bd(cpu_pkt)
        pktv6 = simple_tcpv6_packet(
            eth_dst='00:77:66:55:44:33',
            ipv6_dst='1234:5678:9abc:def0:1122:3344:5566:6677',
            ipv6_hlim=64)
        exp_pktv6 = simple_tcpv6_packet(
            eth_dst='00:11:33:33:44:55',
            eth_src='00:77:66:55:44:33',
            ipv6_dst='1234:5678:9abc:def0:1122:3344:5566:6677',
            ipv6_hlim=63)
        cpu_pkt = simple_cpu_packet(
            ingress_port=swport_to_devport(self, self.devports[0]),
            ingress_ifindex=(self.port0 & 0xFFFF),
            packet_type=0,
            reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_MAX + SWITCH_HOSTIF_TRAP_ATTR_INTERNAL_TRAP_GLEAN,
            ingress_bd=0x0,
            inner_pkt=pktv6)
        cpu_pktv6 = cpu_packet_mask_ingress_bd(cpu_pkt)

        try:
            print('no fdb, post route flood')
            if self.test_params['target'] != 'hw':
                send_packet(self, self.devports[0], pkt)
                verify_packet(self, cpu_pktv4, self.cpu_port)
                send_packet(self, self.devports[0], pktv6)
                verify_packet(self, cpu_pktv6, self.cpu_port)

            print('fdb, route to port 1')
            mac = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:11:33:33:44:55', destination_handle=self.port1)
            send_packet(self, self.devports[0], pkt)
            verify_packets(self, exp_pkt, [self.devports[1]])
            send_packet(self, self.devports[0], pktv6)
            verify_packets(self, exp_pktv6, [self.devports[1]])

            print('remove fdb, post route flood again')
            self.cleanlast()
            if self.test_params['target'] != 'hw':
                send_packet(self, self.devports[0], pkt)
                verify_packet(self, cpu_pktv4, self.cpu_port)
                send_packet(self, self.devports[0], pktv6)
                verify_packet(self, cpu_pktv6, self.cpu_port)
        finally:
            pass

    def tearDown(self):
        self.cleanup()

@group('l3')
class L3SVIVirtualTest(ApiHelper):
    def setUp(self):
        print()
        self.configure()

        #### PORT ####
        rif0 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port0, vrf_handle=self.vrf10, src_mac=self.rmac)

        vlan_mbr1 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port1)
        vlan_mbr4 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port4)
        vlan_mbr2 = self.add_vlan_member(self.device, member_handle=self.port2, vlan_handle=self.vlan10,
                tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.attribute_set(self.port2, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.attribute_set(self.port4, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)

        # Create rif1 with vlan10
        self.rif1 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_VLAN, vlan_handle=self.vlan10, vrf_handle=self.vrf10, src_mac=self.rmac)

        # Create nhop1, nhop2 & nhop3 on SVI
        nhop1 = self.add_nexthop(self.device, handle=self.rif1, dest_ip='10.10.0.1')
        self.neighbor1 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=self.rif1, dest_ip='10.10.0.1')
        route1 = self.add_route(self.device, ip_prefix='10.10.10.1', vrf_handle=self.vrf10, nexthop_handle=nhop1)

        nhop2 = self.add_nexthop(self.device, handle=self.rif1, dest_ip='10.10.0.2')
        self.neighbor2 = self.add_neighbor(self.device, mac_address='00:22:22:33:44:55', handle=self.rif1, dest_ip='10.10.0.2')
        route2 = self.add_route(self.device, ip_prefix='10.10.10.2', vrf_handle=self.vrf10, nexthop_handle=nhop2)

        nhop3 = self.add_nexthop(self.device, handle=self.rif1, dest_ip='10.10.0.3')
        self.neighbor3 = self.add_neighbor(self.device, mac_address='00:33:22:33:44:55', handle=self.rif1, dest_ip='10.10.0.3')
        route3 = self.add_route(self.device, ip_prefix='10.10.10.3', vrf_handle=self.vrf10, nexthop_handle=nhop3)

        # add directed broadcast IP on vlan 10
        nhop_bcast = self.add_nexthop(self.device, handle=self.rif1, dest_ip='10.10.1.255')
        self.add_neighbor(self.device, mac_address='FF:FF:FF:FF:FF:FF', handle=self.rif1, dest_ip='10.10.1.255')
        route3 = self.add_route(self.device, ip_prefix='10.10.1.255', vrf_handle=self.vrf10, nexthop_handle=nhop_bcast)

        # Create nhop and route to L3 intf
        nhop4 = self.add_nexthop(self.device, handle=rif0, dest_ip='11.11.0.2')
        neighbor4 = self.add_neighbor(self.device, mac_address='00:44:22:33:44:55', handle=rif0, dest_ip='11.11.0.2')
        route4 = self.add_route(self.device, ip_prefix='11.11.11.1', vrf_handle=self.vrf10, nexthop_handle=nhop4)

    def sviVirtualTest(self):
        print()
        print("sviVirtualTest")
        self.new_rmac = '00:77:66:55:44:44'
        pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            ip_dst='11.11.11.1',
            ip_ttl=64)
        pkt1 = simple_tcp_packet(
            eth_dst=self.new_rmac,
            ip_dst='11.11.11.1',
            ip_ttl=64)
        exp_pkt = simple_tcp_packet(
            eth_dst='00:44:22:33:44:55',
            eth_src='00:77:66:55:44:33',
            ip_dst='11.11.11.1',
            ip_ttl=63)
        tagged_pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            ip_dst='11.11.11.1',
            ip_ttl=64,
            dl_vlan_enable=True,
            vlan_vid=10,
            pktlen=104)
        try:
            print("Sending packet on port %d with rmac 00:77:66:55:44:33, forward" % self.devports[1])
            send_packet(self, self.devports[1], pkt)
            verify_packet(self, exp_pkt, self.devports[0])


            print("Adding new virtual SVI rif with new ingress rif rmac")
            rif2=self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_VLAN, vlan_handle=self.vlan10, vrf_handle=self.vrf10, src_mac=self.new_rmac, is_virtual=True)
            print("Sending tagged packet on port %d with rmac 00:77:66:55:44:44" % self.devports[2])
            send_packet(self, self.devports[2], tagged_pkt)
            verify_packets(self, exp_pkt, [self.devports[0]])
            print("Sending packet on port %d with rmac 00:77:66:55:44:44" % self.devports[1])
            send_packet(self, self.devports[1], pkt1)
            verify_packets(self, exp_pkt, [self.devports[0]])
            print("Sending packet on port %d with rmac 00:77:66:55:44:44" % self.devports[4])
            send_packet(self, self.devports[4], pkt1)
            verify_packets(self, exp_pkt, [self.devports[0]])

        finally:
            pass

    def runTest(self):
        try:
            self.sviVirtualTest()
        finally:
            pass

    def tearDown(self):
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.port2, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.port4, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.cleanup()

###############################################################################
@group('l3')
@group('lag')
class L3LagTest(ApiHelper):
    def setUp(self):
        print()
        self.configure()

        self.rif0 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port0, vrf_handle=self.vrf10, src_mac=self.rmac)
        nhop = self.add_nexthop(self.device, handle=self.rif0, dest_ip='11.11.11.1')
        neighbor = self.add_neighbor(self.device, mac_address='00:11:11:11:11:11', handle=self.rif0, dest_ip='11.11.11.1')
        self.add_route(self.device, ip_prefix='11.11.11.1', vrf_handle=self.vrf10, nexthop_handle=nhop)

        self.lag0 = self.add_lag(self.device)
        self.lag0_mbr1 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port1)
        self.lag0_mbr2 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port2)
        self.lag0_mbr3 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port3)
        self.lag0_mbr4 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port4, egress_disable=True)
        self.lag0_mbr5 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port5)
        self.lag0_mbr6 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port6, egress_disable=True)

        self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port7)
        self.attribute_set(self.port7, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.lag0)
        self.attribute_set(self.lag0, SWITCH_LAG_ATTR_PORT_VLAN_ID, 10)
        self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:55:55:55:55:55', destination_handle=self.port7)
        self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:22:22:22:22:22', destination_handle=self.lag0)

        self.rif1 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_VLAN, vlan_handle=self.vlan10, vrf_handle=self.vrf10, src_mac=self.rmac)
        nhop = self.add_nexthop(self.device, handle=self.rif1, dest_ip='22.22.22.1')
        neighbor = self.add_neighbor(self.device, mac_address='00:22:22:22:22:22', handle=self.rif1, dest_ip='22.22.22.1')
        self.add_route(self.device, ip_prefix='22.22.22.1', vrf_handle=self.vrf10, nexthop_handle=nhop)

        self.add_route(self.device, ip_prefix='22.22.0.0/16', vrf_handle=self.vrf10, nexthop_handle=nhop)

    def packetTest(self):
        try:
            plist = [self.devports[1], self.devports[2], self.devports[3], self.devports[4], self.devports[5], self.devports[6]]
            print('L2: Unicast')
            uc = [0, 0, 0, 0, 0, 0]
            dst_ip = int(binascii.hexlify(socket.inet_aton('22.22.22.2')), 16)
            src_ip = int(binascii.hexlify(socket.inet_aton('192.168.1.2')), 16)
            max_itrs = 48
            for i in range(0, max_itrs):
                dst_ip_addr = socket.inet_ntoa(binascii.unhexlify(format(dst_ip, 'x').zfill(8)))
                src_ip_addr = socket.inet_ntoa(binascii.unhexlify(format(src_ip, 'x').zfill(8)))
                pkt = simple_tcp_packet(
                    eth_dst='00:22:22:22:22:22',
                    eth_src='00:55:55:55:55:55',
                    ip_dst=dst_ip_addr,
                    ip_src=src_ip_addr)
                send_packet(self, self.devports[7], pkt)
                rcv_idx = verify_any_packet_any_port(
                    self, [pkt, pkt, pkt, pkt, pkt, pkt], plist)
                uc[rcv_idx] += 1
                dst_ip += 1
                src_ip += 1
            print(uc)

            print('L2: Broadcast')
            bc = [0, 0, 0, 0, 0, 0]
            mac_pre = '00:22:22:22:22:'
            dst_ip = int(binascii.hexlify(socket.inet_aton('22.22.22.2')), 16)
            for i in range(0, max_itrs):
                dst_ip_addr = socket.inet_ntoa(binascii.unhexlify(format(dst_ip, 'x').zfill(8)))
                mac = mac_pre + hex(i)
                pkt = simple_arp_packet(
                    arp_op=1,
                    pktlen=100,
                    eth_src=mac,
                    hw_snd=mac,
                    ip_snd=dst_ip_addr,
                    ip_tgt='22.22.22.1')
                send_packet(self, self.devports[7], pkt)
                rcv_idx = verify_any_packet_any_port(
                    self, [pkt, pkt, pkt, pkt, pkt, pkt], plist)
                bc[rcv_idx] += 1
                dst_ip += 1
            print(bc)
            pkt = simple_arp_packet(
                arp_op=1,
                pktlen=100,
                eth_src='00:22:22:22:22:22',
                hw_snd='00:22:22:22:22:22',
                ip_snd='22.22.22.1',
                ip_tgt='22.22.22.2')
            send_packet(self, self.devports[1], pkt)
            verify_packets(self, pkt, [self.devports[7]])
            send_packet(self, self.devports[2], pkt)
            verify_packets(self, pkt, [self.devports[7]])
            send_packet(self, self.devports[3], pkt)
            verify_packets(self, pkt, [self.devports[7]])
            send_packet(self, self.devports[5], pkt)
            verify_packets(self, pkt, [self.devports[7]])

            print('L3: Unicast')
            l3 = [0, 0, 0, 0, 0, 0]
            dst_ip = int(binascii.hexlify(socket.inet_aton('22.22.22.2')), 16)
            for i in range(0, max_itrs):
                dst_ip_addr = socket.inet_ntoa(binascii.unhexlify(format(dst_ip, 'x').zfill(8)))
                pkt = simple_tcp_packet(
                    eth_dst=self.rmac,
                    eth_src='00:11:11:11:11:11',
                    ip_dst=dst_ip_addr,
                    ip_src='11.11.11.1',
                    ip_ttl=64)
                exp_pkt = simple_tcp_packet(
                    eth_dst='00:22:22:22:22:22',
                    eth_src=self.rmac,
                    ip_dst=dst_ip_addr,
                    ip_src='11.11.11.1',
                    ip_ttl=63)
                send_packet(self, self.devports[0], pkt)
                rcv_idx = verify_any_packet_any_port(
                    self, [exp_pkt, exp_pkt, exp_pkt, exp_pkt, exp_pkt, exp_pkt], plist)
                l3[rcv_idx] += 1
                dst_ip += 1
            print(l3)
            return uc, bc, l3
        finally:
            pass

    def runTest(self):
        uc, bc, l3 = self.packetTest()
        for count in [uc, bc, l3]:
            for i in [0,1,2,4]:
                self.assertTrue(count[i] != 0)
            self.assertEqual(count[3], 0)
            self.assertEqual(count[5], 0)
        self.attribute_set(self.lag0_mbr4, SWITCH_LAG_MEMBER_ATTR_EGRESS_DISABLE, False)
        self.attribute_set(self.lag0_mbr6, SWITCH_LAG_MEMBER_ATTR_EGRESS_DISABLE, False)
        uc, bc, l3 = self.packetTest()
        for count in [uc, bc, l3]:
            for i in [0,1,2,3,4,5]:
                self.assertTrue(count[i] != 0)
        self.attribute_set(self.lag0_mbr2, SWITCH_LAG_MEMBER_ATTR_EGRESS_DISABLE, True)
        self.attribute_set(self.lag0_mbr3, SWITCH_LAG_MEMBER_ATTR_EGRESS_DISABLE, True)
        self.attribute_set(self.lag0_mbr4, SWITCH_LAG_MEMBER_ATTR_EGRESS_DISABLE, True)
        self.attribute_set(self.lag0_mbr5, SWITCH_LAG_MEMBER_ATTR_EGRESS_DISABLE, True)
        self.attribute_set(self.lag0_mbr6, SWITCH_LAG_MEMBER_ATTR_EGRESS_DISABLE, True)
        uc, bc, l3 = self.packetTest()
        for count in [uc, bc, l3]:
            for i in [1,2,3,4,5]:
                self.assertTrue(count[i] == 0)
            self.assertTrue(count[0] != 0)
        self.attribute_set(self.lag0_mbr2, SWITCH_LAG_MEMBER_ATTR_EGRESS_DISABLE, False)
        self.attribute_set(self.lag0_mbr3, SWITCH_LAG_MEMBER_ATTR_EGRESS_DISABLE, False)
        self.attribute_set(self.lag0_mbr5, SWITCH_LAG_MEMBER_ATTR_EGRESS_DISABLE, False)
        uc, bc, l3 = self.packetTest()
        for count in [uc, bc, l3]:
            for i in [0,1,2,4]:
                self.assertTrue(count[i] != 0)
            self.assertEqual(count[3], 0)
            self.assertEqual(count[5], 0)

    def tearDown(self):
        self.attribute_set(self.port7, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.lag0, SWITCH_LAG_ATTR_PORT_VLAN_ID, 0)
        self.cleanup()

###############################################################################
@group('l3')
@group('lag')
class L3LagAddDeleteTest(ApiHelper):
    '''
    add a combination of LAGs with various member states
     - all enabled
     - few enabled
     - all disabled
    Post warm-init update the states and add/del new members
    '''
    def setUp(self):
        print()
        self.configure()
        self.max_itrs = 50

        self.rif0 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port0, vrf_handle=self.vrf10, src_mac=self.rmac)
        nhop = self.add_nexthop(self.device, handle=self.rif0, dest_ip='11.11.11.1')
        neighbor = self.add_neighbor(self.device, mac_address='00:11:11:11:11:11', handle=self.rif0, dest_ip='11.11.11.1')
        self.add_route(self.device, ip_prefix='11.11.11.1', vrf_handle=self.vrf10, nexthop_handle=nhop)

        self.lag0 = self.add_lag(self.device)
        self.lag0_mbr1 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port1)
        self.lag0_mbr2 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port2)

        self.lag1 = self.add_lag(self.device)
        self.lag1_mbr1 = self.add_lag_member(self.device, lag_handle=self.lag1, port_handle=self.port3)
        self.lag1_mbr2 = self.add_lag_member(self.device, lag_handle=self.lag1, port_handle=self.port4, egress_disable=True)

        self.lag2 = self.add_lag(self.device)
        self.lag2_mbr1 = self.add_lag_member(self.device, lag_handle=self.lag2, port_handle=self.port5, egress_disable=True)
        self.lag2_mbr2 = self.add_lag_member(self.device, lag_handle=self.lag2, port_handle=self.port6, egress_disable=True)

        self.lag3 = self.add_lag(self.device)
        self.lag3_mbr1 = self.add_lag_member(self.device, lag_handle=self.lag3, port_handle=self.port13)
        self.stack.pop()
        self.lag3_mbr2 = self.add_lag_member(self.device, lag_handle=self.lag3, port_handle=self.port14)
        self.stack.pop()
        self.lag3_mbr3 = self.add_lag_member(self.device, lag_handle=self.lag3, port_handle=self.port15)

        self.rif0 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.lag0, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.nhop0 = self.add_nexthop(self.device, handle=self.rif0, dest_ip='22.22.22.1')
        self.add_neighbor(self.device, mac_address='00:22:22:22:22:22', handle=self.rif0, dest_ip='22.22.22.1')
        self.add_route(self.device, ip_prefix='22.22.0.0/16', vrf_handle=self.vrf10, nexthop_handle=self.nhop0)

        self.rif1 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.lag1, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.nhop1 = self.add_nexthop(self.device, handle=self.rif1, dest_ip='33.33.33.1')
        self.add_neighbor(self.device, mac_address='00:33:33:33:33:33', handle=self.rif1, dest_ip='33.33.33.1')
        self.add_route(self.device, ip_prefix='33.33.0.0/16', vrf_handle=self.vrf10, nexthop_handle=self.nhop1)

        self.rif2 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.lag2, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.nhop2 = self.add_nexthop(self.device, handle=self.rif2, dest_ip='44.44.44.1')
        self.add_neighbor(self.device, mac_address='00:44:44:44:44:44', handle=self.rif2, dest_ip='44.44.44.1')
        self.add_route(self.device, ip_prefix='44.44.0.0/16', vrf_handle=self.vrf10, nexthop_handle=self.nhop2)

        self.rif3 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.lag3, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.nhop3 = self.add_nexthop(self.device, handle=self.rif3, dest_ip='55.55.55.1')
        self.add_neighbor(self.device, mac_address='00:55:55:55:55:55', handle=self.rif3, dest_ip='55.55.55.1')
        self.add_route(self.device, ip_prefix='55.55.0.0/16', vrf_handle=self.vrf10, nexthop_handle=self.nhop3)

    def packetTest(self, plist, nip, nmac):
        try:
            count = [0] * len(plist)
            dst_ip = int(binascii.hexlify(socket.inet_aton(nip)), 16)
            pkt = simple_tcp_packet(
                    eth_dst=self.rmac,
                    eth_src='00:11:11:11:11:11',
                    ip_dst=nip,
                    ip_src='11.11.11.1',
                    ip_ttl=64)
            exp_pkt = simple_tcp_packet(
                    eth_dst=nmac,
                    eth_src=self.rmac,
                    ip_dst=nip,
                    ip_src='11.11.11.1',
                    ip_ttl=63)
            exp_pkt_list = [exp_pkt] * len(plist)
            for i in range(0, self.max_itrs):
                dst_ip_addr = socket.inet_ntoa(binascii.unhexlify(format(dst_ip, 'x').zfill(8)))
                pkt['IP'].dst = dst_ip_addr
                for i, _ in enumerate(plist):
                    exp_pkt_list[i]['IP'].dst = dst_ip_addr
                send_packet(self, self.devports[0], pkt)
                rcv_idx = verify_any_packet_any_port(self, exp_pkt_list, plist)
                count[rcv_idx] += 1
                dst_ip += 1
            return count
        finally:
            pass

    def runPreTest(self):
        print('Pre warm reboot tests')
        plist = [self.devports[1], self.devports[2]]
        count = self.packetTest(plist, "22.22.22.2", "00:22:22:22:22:22")
        print (plist, count)
        self.assertEqual(sum(count), self.max_itrs)
        plist = [self.devports[3]]
        count = self.packetTest(plist, "33.33.33.2", "00:33:33:33:33:33")
        print (plist, count)
        self.assertEqual(sum(count), self.max_itrs)
        plist = [self.devports[13], self.devports[14], self.devports[15]]
        count = self.packetTest(plist, "55.55.55.2", "00:55:55:55:55:55")
        print (plist, count)
        self.assertEqual(sum(count), self.max_itrs)

    def runPostTest(self):
        print('Post warm reboot tests')
        plist = [self.devports[13], self.devports[14], self.devports[15]]
        count = self.packetTest(plist, "55.55.55.2", "00:55:55:55:55:55")
        print (plist, count)
        for i in range(0, len(count)):
            self.assertTrue(count[i] >= (self.max_itrs * 0.2), "LAG paths are not equally balanced")
        self.assertEqual(sum(count), self.max_itrs)
        self.client.object_delete(self.lag3_mbr1)
        plist = [self.devports[14], self.devports[15]]
        count = self.packetTest(plist, "55.55.55.2", "00:55:55:55:55:55")
        print (plist, count)
        for i in range(0, len(count)):
            self.assertTrue(count[i] >= (self.max_itrs * 0.2), "LAG paths are not equally balanced")
        self.assertEqual(sum(count), self.max_itrs)
        self.client.object_delete(self.lag3_mbr2)
        plist = [self.devports[15]]
        count = self.packetTest(plist, "55.55.55.2", "00:55:55:55:55:55")
        print (plist, count)
        for i in range(0, len(count)):
            self.assertTrue(count[i] >= (self.max_itrs * 0.2), "LAG paths are not equally balanced")
        self.assertEqual(sum(count), self.max_itrs)

        self.lag0_mbr3 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port7)
        self.lag0_mbr4 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port8, egress_disable=True)
        plist = [self.devports[1], self.devports[2], self.devports[7]]
        count = self.packetTest(plist, "22.22.22.2", "00:22:22:22:22:22")
        print (plist, count)
        for i in range(0, len(count)):
            self.assertTrue(count[i] >= (self.max_itrs * 0.2), "LAG paths are not equally balanced")
        self.assertEqual(sum(count), self.max_itrs)

        self.attribute_set(self.lag1_mbr2, SWITCH_LAG_MEMBER_ATTR_EGRESS_DISABLE, False)
        plist = [self.devports[3], self.devports[4]]
        count = self.packetTest(plist, "33.33.33.2", "00:33:33:33:33:33")
        print (plist, count)
        self.assertEqual(sum(count), self.max_itrs)
        for i in range(0, len(count)):
            self.assertTrue(count[i] >= (self.max_itrs * 0.2), "LAG paths are not equally balanced")
        self.lag1_mbr3 = self.add_lag_member(self.device, lag_handle=self.lag1, port_handle=self.port9, egress_disable=True)
        self.lag1_mbr4 = self.add_lag_member(self.device, lag_handle=self.lag1, port_handle=self.port10)
        plist = [self.devports[3], self.devports[4], self.devports[10]]
        count = self.packetTest(plist, "33.33.33.2", "00:33:33:33:33:33")
        print (plist, count)
        self.assertEqual(sum(count), self.max_itrs)
        for i in range(0, len(count)):
            self.assertTrue(count[i] >= (self.max_itrs * 0.2), "LAG paths are not equally balanced")

        self.attribute_set(self.lag2_mbr1, SWITCH_LAG_MEMBER_ATTR_EGRESS_DISABLE, False)
        self.attribute_set(self.lag2_mbr2, SWITCH_LAG_MEMBER_ATTR_EGRESS_DISABLE, False)
        plist = [self.devports[5], self.devports[6]]
        count = self.packetTest(plist, "44.44.44.2", "00:44:44:44:44:44")
        print (plist, count)
        self.assertEqual(sum(count), self.max_itrs)
        for i in range(0, len(count)):
            self.assertTrue(count[i] >= (self.max_itrs * 0.2), "LAG paths are not equally balanced")
        self.lag2_mbr3 = self.add_lag_member(self.device, lag_handle=self.lag2, port_handle=self.port11)
        self.lag2_mbr4 = self.add_lag_member(self.device, lag_handle=self.lag2, port_handle=self.port12)
        plist = [self.devports[5], self.devports[6], self.devports[11], self.devports[12]]
        count = self.packetTest(plist, "44.44.44.2", "00:44:44:44:44:44")
        print (plist, count)
        self.assertEqual(sum(count), self.max_itrs)
        for i in range(0, len(count)):
            self.assertTrue(count[i] >= (self.max_itrs * 0.1), "LAG paths are not equally balanced")

    def runTest(self):
        self.runPreTest()
        self.runPostTest()

    def tearDown(self):
        self.cleanup()

###############################################################################

@disabled
class L3BridgeTest(ApiHelper):
    '''
    This performs basic L3 bridge sub port testing
    Port 0 is L3 RIF
    Bridge 1 - port1/vlan60
    Bridge 2 - port2/vlan70
    '''
    def runTest(self):
        print()
        self.configure()

        # L3 interface on port 0
        if0 = self.add_interface(self.device, port_lag_handle=self.port0, type=SWITCH_INTERFACE_ATTR_TYPE_ACCESS)
        rif0 = self.add_rif(self.device, handle=if0, vrf_handle=self.vrf10, src_mac=self.rmac)

        # Create bridge sub port RIF on port 1
        br1 = self.add_bridge(self.device, type=SWITCH_BRIDGE_ATTR_TYPE_DOT1D)
        if1 = self.add_interface(self.device, port_lag_handle=self.port1, type=SWITCH_INTERFACE_ATTR_TYPE_PORT_VLAN,
            vlan_id=60, bridge_handle=br1)
        rif1 = self.add_rif(self.device, handle=br1, vrf_handle=self.vrf10, src_mac=self.rmac)

        # Create bridge sub port RIF on port 2
        br2 = self.add_bridge(self.device, type=SWITCH_BRIDGE_ATTR_TYPE_DOT1D)
        if2 = self.add_interface(self.device, port_lag_handle=self.port2, type=SWITCH_INTERFACE_ATTR_TYPE_PORT_VLAN,
            vlan_id=70, bridge_handle=br2)
        rif2 = self.add_rif(self.device, handle=br2, vrf_handle=self.vrf10, src_mac=self.rmac)

        nhop0 = self.add_nexthop(self.device, handle=rif0, dest_ip='12.12.0.1')
        neighbor0 = self.add_neighbor(self.device, mac_address='00:00:22:33:44:55', handle=rif0, dest_ip='12.12.0.1')
        route0 = self.add_route(self.device, ip_prefix='12.12.12.1', vrf_handle=self.vrf10, nexthop_handle=nhop0)

        nhop1 = self.add_nexthop(self.device, handle=rif1, dest_ip='10.10.0.1')
        neighbor1 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=rif1, dest_ip='10.10.0.1')
        route1 = self.add_route(self.device, ip_prefix='10.10.10.1', vrf_handle=self.vrf10, nexthop_handle=nhop1)
        mac1 = self.add_mac_entry(self.device, vlan_handle=br1, mac_address='00:11:22:33:44:55', interface_handle=if1)

        nhop2 = self.add_nexthop(self.device, handle=rif2, dest_ip='11.11.0.1')
        neighbor2 = self.add_neighbor(self.device, mac_address='00:22:22:33:44:55', handle=rif2, dest_ip='11.11.0.1')
        route2 = self.add_route(self.device, ip_prefix='11.11.11.1', vrf_handle=self.vrf10, nexthop_handle=nhop2)
        mac2 = self.add_mac_entry(self.device, vlan_handle=br2, mac_address='00:22:22:33:44:55', interface_handle=if2)

        try:
            self.L3RifToBridgeTest()
            self.L3BridgeToRifTest()
            self.L3BridgeToBridgeTest()
        finally:
            self.cleanup()

    def L3RifToBridgeTest(self):
        print("L3RifToBridgeTest")
        pkt_60 = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        exp_pkt_60 = simple_tcp_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src='00:77:66:55:44:33',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            dl_vlan_enable=True,
            vlan_vid=60,
            ip_ttl=63,
            pktlen=104)
        pkt_70 = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='11.11.11.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        exp_pkt_70 = simple_tcp_packet(
            eth_dst='00:22:22:33:44:55',
            eth_src='00:77:66:55:44:33',
            ip_dst='11.11.11.1',
            ip_src='192.168.0.1',
            ip_id=105,
            dl_vlan_enable=True,
            vlan_vid=70,
            ip_ttl=63,
            pktlen=104)

        try:
            print("Sending L3 packet from 0 -> 1 : port0 to port1/vlan60")
            send_packet(self, self.devports[0], pkt_60)
            verify_packet(self, exp_pkt_60, self.devports[1])
            print("Sending L3 packet from 0 -> 1 : port0 to port2/vlan70")
            send_packet(self, self.devports[0], pkt_70)
            verify_packet(self, exp_pkt_70, self.devports[2])
        finally:
            pass

    def L3BridgeToRifTest(self):
        print("L3BridgeToRifTest")
        pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='12.12.12.1',
            ip_src='192.168.0.1',
            ip_id=105,
            dl_vlan_enable=True,
            vlan_vid=70,
            ip_ttl=64)
        exp_pkt = simple_tcp_packet(
            eth_dst='00:00:22:33:44:55',
            eth_src='00:77:66:55:44:33',
            ip_dst='12.12.12.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63,
            pktlen=96)

        try:
            print("Sending L3 packet from 2 -> 0 : port2/vlan70 to port0")
            send_packet(self, self.devports[2], pkt)
            verify_packet(self, exp_pkt, self.devports[0])
        finally:
            pass

    def L3BridgeToBridgeTest(self):
        print("L3BridgeToBridgeTest")
        pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='11.11.11.1',
            ip_src='192.168.0.1',
            ip_id=105,
            dl_vlan_enable=True,
            vlan_vid=60,
            ip_ttl=64)
        exp_pkt = simple_tcp_packet(
            eth_dst='00:22:22:33:44:55',
            eth_src='00:77:66:55:44:33',
            ip_dst='11.11.11.1',
            ip_src='192.168.0.1',
            dl_vlan_enable=True,
            vlan_vid=70,
            ip_id=105,
            ip_ttl=63)

        try:
            print("Sending L3 packet from 1 -> 2 : port1/vlan60 to port2/vlan70")
            send_packet(self, self.devports[1], pkt)
            verify_packet(self, exp_pkt, self.devports[2])
        finally:
            pass

###############################################################################

@group('l3')
@group('ecmp')
class L3ECMPAddDeleteTest(ApiHelper):
    def setUp(self):
        print()

        self.configure()

#ingress port
        rif0 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port0, vrf_handle=self.vrf10, src_mac=self.rmac)

#nhop1
        rif1 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port1, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.nhop1 = self.add_nexthop(self.device, handle=rif1, dest_ip='10.10.0.2')
        neighbor1 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=rif1, dest_ip='10.10.0.2')
        mac1 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:11:22:33:44:55', destination_handle=self.port1)
#nhop2
        rif2 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port2, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.nhop2 = self.add_nexthop(self.device, handle=rif2, dest_ip='20.20.0.2')
        neighbor2 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:56', handle=rif2, dest_ip='20.20.0.2')
        mac2 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:11:22:33:44:56', destination_handle=self.port2)
#nhop3
        rif3 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port3, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.nhop3 = self.add_nexthop(self.device, handle=rif3, dest_ip='20.20.0.3')
        neighbor3 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:57', handle=rif3, dest_ip='20.20.0.3')
        mac3 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:11:22:33:44:57', destination_handle=self.port3)

        # ecmp with same nexthops repeated
        ecmpX = self.add_ecmp(self.device)
        self.ecmpX_member01 = self.add_ecmp_member(self.device, nexthop_handle=self.nhop1, ecmp_handle=ecmpX)
        self.ecmpX_member02 = self.add_ecmp_member(self.device, nexthop_handle=self.nhop2, ecmp_handle=ecmpX)
        self.ecmpX_member03 = self.add_ecmp_member(self.device, nexthop_handle=self.nhop3, ecmp_handle=ecmpX)
        self.ecmpX_member04 = self.add_ecmp_member(self.device, nexthop_handle=self.nhop1, ecmp_handle=ecmpX)
        self.ecmpX_member05 = self.add_ecmp_member(self.device, nexthop_handle=self.nhop2, ecmp_handle=ecmpX)
        self.ecmpX_member06 = self.add_ecmp_member(self.device, nexthop_handle=self.nhop3, ecmp_handle=ecmpX)
        self.add_route(self.device, ip_prefix='20.20.0.0/16', vrf_handle=self.vrf10, nexthop_handle=ecmpX)

        self.ecmp0 = self.add_ecmp(self.device)
        route1 = self.add_route(self.device, ip_prefix='10.10.10.1', vrf_handle=self.vrf10, nexthop_handle=self.ecmp0)
        ecmp_member01 = self.add_ecmp_member(self.device, nexthop_handle=self.nhop1, ecmp_handle=self.ecmp0)
        ecmp_member02 = self.add_ecmp_member(self.device, nexthop_handle=self.nhop2, ecmp_handle=self.ecmp0)
        ecmp_member03 = self.add_ecmp_member(self.device, nexthop_handle=self.nhop3, ecmp_handle=self.ecmp0)

        ecmp1 = self.add_ecmp(self.device)
        ecmp_member01 = self.add_ecmp_member(self.device, nexthop_handle=self.nhop1, ecmp_handle=ecmp1)
        ecmp_member03 = self.add_ecmp_member(self.device, nexthop_handle=self.nhop3, ecmp_handle=ecmp1)

        ecmp2 = self.add_ecmp(self.device)
        ecmp_member01 = self.add_ecmp_member(self.device, nexthop_handle=self.nhop1, ecmp_handle=ecmp2)
        ecmp_member02 = self.add_ecmp_member(self.device, nexthop_handle=self.nhop2, ecmp_handle=ecmp2)

        ecmp3 = self.add_ecmp(self.device)
        ecmp_member01 = self.add_ecmp_member(self.device, nexthop_handle=self.nhop1, ecmp_handle=ecmp3)
        self.pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='10.10.10.1',
                ip_src='192.168.0.1',
                ip_id=106,
                ip_ttl=64)
        self.exp_pkt1 = simple_tcp_packet(
                eth_dst='00:11:22:33:44:55',
                eth_src='00:77:66:55:44:33',
                ip_dst='10.10.10.1',
                ip_src='192.168.0.1',
                ip_id=106,
                ip_ttl=63)
        self.exp_pkt2 = simple_tcp_packet(
                eth_dst='00:11:22:33:44:56',
                eth_src='00:77:66:55:44:33',
                ip_dst='10.10.10.1',
                ip_src='192.168.0.1',
                ip_id=106,
                ip_ttl=63)
        self.exp_pkt3 = simple_tcp_packet(
                eth_dst='00:11:22:33:44:57',
                eth_src='00:77:66:55:44:33',
                ip_dst='10.10.10.1',
                ip_src='192.168.0.1',
                ip_id=106,
                ip_ttl=63)

    def runTest(self):
        try:
            self.AddDeleteTest_1()
            self.AddDeleteTest_2()
        finally:
            pass

    def tearDown(self):
        self.cleanup()

    def AddDeleteTest_2(self):
        try:
            max_itrs = 30
            def packetTest(plist):
                pkt = copy.deepcopy(self.pkt)
                exp_pkt1 = copy.deepcopy(self.exp_pkt1)
                exp_pkt2 = copy.deepcopy(self.exp_pkt2)
                exp_pkt3 = copy.deepcopy(self.exp_pkt3)
                count = [0, 0, 0]
                dst_ip = int(binascii.hexlify(socket.inet_aton('20.20.20.1')), 16)
                for i in range(0, max_itrs):
                    dst_ip_addr = socket.inet_ntoa(binascii.unhexlify(format(dst_ip, 'x').zfill(8)))
                    pkt[IP].dst = dst_ip_addr
                    exp_pkt1[IP].dst = dst_ip_addr
                    exp_pkt2[IP].dst = dst_ip_addr
                    exp_pkt3[IP].dst = dst_ip_addr
                    send_packet(self, self.devports[0], pkt)
                    rcv_idx = verify_any_packet_any_port(
                        self, [exp_pkt1, exp_pkt2, exp_pkt3], plist)
                    count[rcv_idx] += 1
                    dst_ip += 1
                return count

            print("LB with 6 members and 3 unique")
            plist = [self.devports[1], self.devports[2], self.devports[3]]
            count = packetTest(plist)
            print(count)
            self.assertTrue(count[0] > 0)
            self.assertTrue(count[1] > 0)
            self.assertTrue(count[2] > 0)
            self.assertEqual(sum(count), max_itrs)

            print("LB with 6 members and 2 unique")
            self.attribute_set(self.ecmpX_member03, SWITCH_ECMP_MEMBER_ATTR_NEXTHOP_HANDLE, self.nhop1)
            self.attribute_set(self.ecmpX_member06, SWITCH_ECMP_MEMBER_ATTR_NEXTHOP_HANDLE, self.nhop1)
            count = packetTest(plist)
            print(count)
            self.assertTrue(count[0] > 0)
            self.assertTrue(count[1] > 0)
            self.assertTrue(count[0] > count[1])
            self.assertEqual(count[2], 0)
            self.assertEqual(sum(count), max_itrs)

            print("LB with 6 members and 1 unique on nhop1")
            self.attribute_set(self.ecmpX_member02, SWITCH_ECMP_MEMBER_ATTR_NEXTHOP_HANDLE, self.nhop1)
            self.attribute_set(self.ecmpX_member05, SWITCH_ECMP_MEMBER_ATTR_NEXTHOP_HANDLE, self.nhop1)
            count = packetTest(plist)
            print(count)
            self.assertEqual(count[0], max_itrs)
            self.assertEqual(count[1], 0)
            self.assertEqual(count[2], 0)
            self.assertEqual(sum(count), max_itrs)

            print("LB with 6 members and 1 unique moved from nhop1 to nhop2")
            self.attribute_set(self.ecmpX_member01, SWITCH_ECMP_MEMBER_ATTR_NEXTHOP_HANDLE, self.nhop2)
            self.attribute_set(self.ecmpX_member02, SWITCH_ECMP_MEMBER_ATTR_NEXTHOP_HANDLE, self.nhop2)
            self.attribute_set(self.ecmpX_member03, SWITCH_ECMP_MEMBER_ATTR_NEXTHOP_HANDLE, self.nhop2)
            self.attribute_set(self.ecmpX_member04, SWITCH_ECMP_MEMBER_ATTR_NEXTHOP_HANDLE, self.nhop2)
            self.attribute_set(self.ecmpX_member05, SWITCH_ECMP_MEMBER_ATTR_NEXTHOP_HANDLE, self.nhop2)
            self.attribute_set(self.ecmpX_member06, SWITCH_ECMP_MEMBER_ATTR_NEXTHOP_HANDLE, self.nhop2)
            count = packetTest(plist)
            print(count)
            self.assertEqual(count[0], 0)
            self.assertEqual(count[1], max_itrs)
            self.assertEqual(count[2], 0)
            self.assertEqual(sum(count), max_itrs)

            print("LB with 6 members and 3 unique, reset to original")
            self.attribute_set(self.ecmpX_member01, SWITCH_ECMP_MEMBER_ATTR_NEXTHOP_HANDLE, self.nhop1)
            self.attribute_set(self.ecmpX_member02, SWITCH_ECMP_MEMBER_ATTR_NEXTHOP_HANDLE, self.nhop2)
            self.attribute_set(self.ecmpX_member03, SWITCH_ECMP_MEMBER_ATTR_NEXTHOP_HANDLE, self.nhop3)
            self.attribute_set(self.ecmpX_member04, SWITCH_ECMP_MEMBER_ATTR_NEXTHOP_HANDLE, self.nhop1)
            self.attribute_set(self.ecmpX_member05, SWITCH_ECMP_MEMBER_ATTR_NEXTHOP_HANDLE, self.nhop2)
            self.attribute_set(self.ecmpX_member06, SWITCH_ECMP_MEMBER_ATTR_NEXTHOP_HANDLE, self.nhop3)
            count = packetTest(plist)
            print(count)
            self.assertTrue(count[0] > 0)
            self.assertTrue(count[1] > 0)
            self.assertTrue(count[2] > 0)
            self.assertEqual(sum(count), max_itrs)
        finally:
            pass

    def AddDeleteTest_1(self):
        try:

            print("Send packet with all 4 ecmp present")
            print("Sending packet from port %d" % self.devports[0], " (192.168.0.1 -> 10.10.10.1) Host")
            send_packet(self, self.devports[0], self.pkt)
            verify_any_packet_any_port(self, [self.exp_pkt1, self.exp_pkt2, self.exp_pkt3],
                                       [self.devports[1], self.devports[2], self.devports[3]], timeout=2)

            print("Remove ecmp3")
            self.cleanlast()
            self.cleanlast()
            print("Sending packet from port %d" % self.devports[0], " (192.168.0.1 -> 10.10.10.1) Host")
            send_packet(self, self.devports[0], self.pkt)
            verify_any_packet_any_port(self, [self.exp_pkt1, self.exp_pkt2, self.exp_pkt3],
                                       [self.devports[1], self.devports[2], self.devports[3]], timeout=2)

            print("Remove ecmp2")
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            print("Sending packet from port %d" % self.devports[0], " (192.168.0.1 -> 10.10.10.1) Host")
            send_packet(self, self.devports[0], self.pkt)
            verify_any_packet_any_port(self, [self.exp_pkt1, self.exp_pkt2, self.exp_pkt3],
                                       [self.devports[1], self.devports[2], self.devports[3]], timeout=2)

            print("Remove ecmp1")
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            print("Sending packet from port %d" % self.devports[0], " (192.168.0.1 -> 10.10.10.1) Host")
            send_packet(self, self.devports[0], self.pkt)
            verify_any_packet_any_port(self, [self.exp_pkt1, self.exp_pkt2, self.exp_pkt3],
                                       [self.devports[1], self.devports[2], self.devports[3]], timeout=2)

            print("Remove member 3 from ecmp0")
            self.cleanlast()
            print("Sending packet from port %d" % self.devports[0], " (192.168.0.1 -> 10.10.10.1) Host")
            send_packet(self, self.devports[0], self.pkt)
            verify_any_packet_any_port(self, [self.exp_pkt1, self.exp_pkt2],
                                       [self.devports[1], self.devports[2]], timeout=2)

            print("Remove member 2 from ecmp0")
            self.cleanlast()
            print("Sending packet from port %d" % self.devports[0], " (192.168.0.1 -> 10.10.10.1) Host")
            send_packet(self, self.devports[0], self.pkt)
            verify_any_packet_any_port(self, [self.exp_pkt1],
                                       [self.devports[1]], timeout=2)

            print("Remove member 1 from ecmp0, packet should be dropped")
            self.cleanlast()
            print("Sending packet from port %d" % self.devports[0], " (192.168.0.1 -> 10.10.10.1) Host")
            send_packet(self, self.devports[0], self.pkt)
            verify_no_other_packets(self, timeout=2)

            print("Add member 1 to ecmp0")
            ecmp_member01 = self.add_ecmp_member(self.device, nexthop_handle=self.nhop1, ecmp_handle=self.ecmp0)
            print("Sending packet from port %d" % self.devports[0], " (192.168.0.1 -> 10.10.10.1) Host")
            send_packet(self, self.devports[0], self.pkt)
            verify_packets(self, self.exp_pkt1, [self.devports[1]])

            print("Add member 2 to ecmp0")
            ecmp_member02 = self.add_ecmp_member(self.device, nexthop_handle=self.nhop2, ecmp_handle=self.ecmp0)
            print("Sending packet from port %d" % self.devports[0], " (192.168.0.1 -> 10.10.10.1) Host")
            send_packet(self, self.devports[0], self.pkt)
            verify_any_packet_any_port(self, [self.exp_pkt1, self.exp_pkt2],
                                       [self.devports[1], self.devports[2]], timeout=2)

            print("Add member 3 to ecmp0")
            ecmp_member03 = self.add_ecmp_member(self.device, nexthop_handle=self.nhop3, ecmp_handle=self.ecmp0)
            print("Sending packet from port %d" % self.devports[0], " (192.168.0.1 -> 10.10.10.1) Host")
            send_packet(self, self.devports[0], self.pkt)
            verify_any_packet_any_port(self, [self.exp_pkt1, self.exp_pkt2, self.exp_pkt3],
                                       [self.devports[1], self.devports[2], self.devports[3]], timeout=2)

            print("Add back ecmp 1, 2, 3")
            ecmp1 = self.add_ecmp(self.device)
            ecmp_member01 = self.add_ecmp_member(self.device, nexthop_handle=self.nhop1, ecmp_handle=ecmp1)
            ecmp_member03 = self.add_ecmp_member(self.device, nexthop_handle=self.nhop3, ecmp_handle=ecmp1)

            ecmp2 = self.add_ecmp(self.device)
            ecmp_member01 = self.add_ecmp_member(self.device, nexthop_handle=self.nhop1, ecmp_handle=ecmp2)
            ecmp_member02 = self.add_ecmp_member(self.device, nexthop_handle=self.nhop2, ecmp_handle=ecmp2)

            ecmp3 = self.add_ecmp(self.device)
            ecmp_member01 = self.add_ecmp_member(self.device, nexthop_handle=self.nhop1, ecmp_handle=ecmp3)
            print("Sending packet from port %d" % self.devports[0], " (192.168.0.1 -> 10.10.10.1) Host")
            send_packet(self, self.devports[0], self.pkt)
            verify_any_packet_any_port(self, [self.exp_pkt1, self.exp_pkt2, self.exp_pkt3],
                                       [self.devports[1], self.devports[2], self.devports[3]], timeout=2)
        finally:
            pass

###############################################################################
@disabled
class IPv4ResilientECMPTest(ApiHelper):
    def runTest(self):
        print()

        self.configure()

#ingress port
        rif0 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port0, vrf_handle=self.vrf10, src_mac=self.rmac)

#ecmp group
        self.ecmp_group = self.add_ecmp(self.device)
        route1 = self.add_route(self.device, ip_prefix='20.20.20.1/16', vrf_handle=self.vrf10, nexthop_handle=self.ecmp_group)
#nhop1
        rif = [None]*7
        neighbor = [None]*7
        mac = [None]*7
        self.ecmp_member = [None]*7
        self.nhop = [None]*7
        self.exp_pkt = [None]*7

        self.ip_pre = '10.10.10.'
        self.mac_pre = '00:22:22:22:22:'
        self.num_paths = 6

        for port in [self.port1, self.port2, self.port3, self.port4, self.port5, self.port6]:
            pnum = self.attribute_get(port, SWITCH_PORT_ATTR_DEV_PORT)
            nhop_ip = self.ip_pre + str(pnum)
            nhop_mac = self.mac_pre + str(pnum)
            rif[pnum] = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=port, vrf_handle=self.vrf10, src_mac=self.rmac)
            self.nhop[pnum] = self.add_nexthop(self.device, handle=rif[pnum], dest_ip=nhop_ip)
            neighbor[pnum] = self.add_neighbor(self.device, mac_address=nhop_mac, handle=rif[pnum], dest_ip=nhop_ip)
            mac[pnum] = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address=nhop_mac, destination_handle=port)
            self.ecmp_member[pnum] = self.add_ecmp_member(self.device, nexthop_handle=self.nhop[pnum], ecmp_handle=self.ecmp_group)
            self.exp_pkt[pnum] = simple_tcp_packet(
                                                  eth_dst=nhop_mac,
                                                  eth_src='00:77:66:55:44:33',
                                                  ip_dst='20.20.20.1',
                                                  ip_src='192.168.0.1',
                                                  ip_id=106,
                                                  ip_ttl=63)
        try:
            self.AddDeleteTest()
        finally:
            self.cleanup()

    def get_exp_pkts(self, src_port, dst_port):
        for i in range (0, self.num_paths):
            nhop_mac = self.mac_pre + str(i+1)
            self.exp_pkt[i] = simple_tcp_packet(
                eth_dst=nhop_mac,
                eth_src='00:77:66:55:44:33',
                ip_dst='20.20.20.1',
                ip_src='192.168.0.1',
                tcp_sport=src_port,
                tcp_dport=dst_port,
                ip_id=106,
                ip_ttl=63)

    def AddDeleteTest(self):
        try:
            pkt_to_idx = {}
            num_paths = 6
            count = [0] * 6
            num_sample_pkts = 120
            print ("Send packets with all 6 members present")
            seed = time.time()
            random.seed(seed)
            for i in range(0, num_sample_pkts):
                src_port = random.randint(0, 65535)
                dst_port = random.randint(0, 65535)
                pkt = simple_tcp_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src='00:22:22:22:22:22',
                    ip_dst='20.20.20.1',
                    ip_src='192.168.0.1',
                    tcp_sport=src_port,
                    tcp_dport=dst_port,
                    ip_id=106,
                    ip_ttl=64)
                send_packet(self, self.devports[0], pkt)
                self.get_exp_pkts(src_port, dst_port)
                rcv_idx = verify_any_packet_any_port(self, self.exp_pkt,
                                                     [self.devports[1], self.devports[2], self.devports[3],
                                                      self.devports[4], self.devports[5], self.devports[6],
                                                     ], timeout=2)
                pkt_to_idx[(src_port, dst_port)] = rcv_idx
                count[rcv_idx] += 1
            print("Initial ECMP load balancing result", count)
            prev_count = count

            active_member_list = [0,1,2,3,4,5]
            inactive_member_list = []
            activate = 0
            # Activate/Deactivate one member at a time and send the same set of packets again
            for i in range(0,9):
                print("iterartion %d" % i)
                if (len(active_member_list)==6):
                    activate = 0;
                elif (len(active_member_list)==1):
                    activate = 1;
                else:
                    activate = random.randint(0,1)
                if (activate):
                    act_member = random.choice(inactive_member_list)
                    inactive_member_list.remove(act_member)
                    active_member_list.append(act_member)
                    self.attribute_set(self.ecmp_member[act_member+1], SWITCH_ECMP_MEMBER_ATTR_ENABLE, True)
                    print("        Activate member %d" %changed_member, "and send the same set of packets")
                else:
                    changed_member = random.choice(active_member_list)
                    inactive_member_list.append(changed_member)
                    active_member_list.remove(changed_member)
                    self.attribute_set(self.ecmp_member[changed_member+1], SWITCH_ECMP_MEMBER_ATTR_ENABLE, False)
                    print("        Deactivate member %d" %changed_member, "and send the same set of packets")

                count = [0]*6
                random.seed(seed)
                for i in range(0, num_sample_pkts):
                    src_port = random.randint(0, 65535)
                    dst_port = random.randint(0, 65535)
                    pkt = simple_tcp_packet(
                        eth_dst='00:77:66:55:44:33',
                        eth_src='00:22:22:22:22:22',
                        ip_dst='20.20.20.1',
                        ip_src='192.168.0.1',
                        tcp_sport=src_port,
                        tcp_dport=dst_port,
                        ip_id=106,
                        ip_ttl=64)
                    send_packet(self, self.devports[0], pkt)
                    self.get_exp_pkts(src_port, dst_port)
                    rcv_idx = verify_any_packet_any_port(self, self.exp_pkt,
                                                         [self.devports[1], self.devports[2], self.devports[3],
                                                          self.devports[4], self.devports[5], self.devports[6],
                                                         ], timeout=2)
                    # Make sure that no packets are received on deactivated links
                    self.assertNotIn(rcv_idx, inactive_member_list, "received packet on a deactivated member")
                    # Make sure the unaffected packets are received on the same path
                    if (i < num_sample_pkts and
                        pkt_to_idx[(src_port, dst_port)] in active_member_list):
                        if (rcv_idx != pkt_to_idx[(src_port, dst_port)]):
                            print("An unaffected flow was moved to a new member. Expected member %d" % pkt_to_idx[(src_port, dst_port)], " actual member %d" % rcv_idx)
                    count[rcv_idx] += 1

                # Make sure the live paths are equally balanced after member deactivated
                affected_flow_count = [0]*6
                print("        ECMP load balancing result after member status is changed", count)
                for i in range(0, 6):
                    if i in active_member_list:
                        affected_flow_count[i] = (count[i] - prev_count[i])
                    else:
                        affected_flow_count[i] = '-'
                print("        Load distribution of affected flows amongst active paths =", affected_flow_count)
                for i in active_member_list:
                    self.assertTrue((count[i] >= ((num_sample_pkts / len(active_member_list)) * 0.5)),
                                    "Not all paths are equally balanced")



        finally:
            pass


###############################################################################

@group('l3')
@group('ecmp')
class L3ECMPTest(ApiHelper):
    def setUp(self):
        print()

        self.configure()

        # ingress port
        self.rif0 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port0, vrf_handle=self.vrf10, src_mac=self.rmac)

        # ecmp0 nhop
        rif1 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port1, vrf_handle=self.vrf10, src_mac=self.rmac)
        nhop1 = self.add_nexthop(self.device, handle=rif1, dest_ip='10.10.0.2')
        neighbor1 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=rif1, dest_ip='10.10.0.2')
        mac1 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:11:22:33:44:55', destination_handle=self.port1)

        # ecmp0 nhop
        rif2 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port2, vrf_handle=self.vrf10, src_mac=self.rmac)
        nhop2 = self.add_nexthop(self.device, handle=rif2, dest_ip='20.20.0.2')
        neighbor2 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:56', handle=rif2, dest_ip='20.20.0.2')
        mac2 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:11:22:33:44:56', destination_handle=self.port2)

        # ecmp1 nhop
        self.lag0 = self.add_lag(self.device)
        lag_mbr00 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port3)
        lag_mbr01 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port4)
        lag_mbr02 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port5)
        rif3 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.lag0, vrf_handle=self.vrf10, src_mac=self.rmac)
        nhop3 = self.add_nexthop(self.device, handle=rif3, dest_ip='30.30.0.2')
        neighbor3 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:57', handle=rif3, dest_ip='30.30.0.2')
        mac3 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:11:22:33:44:57', destination_handle=self.lag0)

        # ecmp1 nhop
        self.lag1 = self.add_lag(self.device)
        lag_mbr10 = self.add_lag_member(self.device, lag_handle=self.lag1, port_handle=self.port6)
        lag_mbr11 = self.add_lag_member(self.device, lag_handle=self.lag1, port_handle=self.port7)
        lag_mbr12 = self.add_lag_member(self.device, lag_handle=self.lag1, port_handle=self.port8)
        rif4 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.lag1, vrf_handle=self.vrf10, src_mac=self.rmac)
        nhop4 = self.add_nexthop(self.device, handle=rif4, dest_ip='40.40.0.2')
        neighbor4 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:58', handle=rif4, dest_ip='40.40.0.2')
        mac4 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:11:22:33:44:58', destination_handle=self.lag1)

        ecmp0 = self.add_ecmp(self.device)
        ecmp_member01 = self.add_ecmp_member(self.device, nexthop_handle=nhop1, ecmp_handle=ecmp0)
        ecmp_member02 = self.add_ecmp_member(self.device, nexthop_handle=nhop2, ecmp_handle=ecmp0)

        ecmp1 = self.add_ecmp(self.device)
        ecmp_member11 = self.add_ecmp_member(self.device, nexthop_handle=nhop3, ecmp_handle=ecmp1)
        ecmp_member12 = self.add_ecmp_member(self.device, nexthop_handle=nhop4, ecmp_handle=ecmp1)

        route1 = self.add_route(self.device, ip_prefix='10.10.10.1', vrf_handle=self.vrf10, nexthop_handle=ecmp0)
        route2 = self.add_route(self.device, ip_prefix='10.100.0.0/16', vrf_handle=self.vrf10, nexthop_handle=ecmp1)
        route0_lpm = self.add_route(self.device, ip_prefix='11.11.11.0/24', vrf_handle=self.vrf10, nexthop_handle=ecmp0)
        routev6 = self.add_route(self.device, ip_prefix='1234:5678:9abc:def0:4422:1133:5577:99aa', vrf_handle=self.vrf10, nexthop_handle=ecmp0)
        routev6_lpm = self.add_route(self.device, ip_prefix='4000::0/124', vrf_handle=self.vrf10, nexthop_handle=ecmp0)

    def runTest(self):
        try:
            self.EcmpIPv4HostTest()
            self.EcmpIPv4LpmTest()
            self.EcmpIPv6HostTest()
            self.EcmpIPv6LpmTest()
            self.EcmpIPv4LagTest()
            self.EcmpRebalanceTest()
        finally:
            pass

    def tearDown(self):
        self.cleanup()

    def EcmpIPv4HostTest(self):
        print("EcmpIPv4HostTest()")
        try:
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='10.10.10.1',
                ip_src='192.168.0.1',
                ip_id=106,
                ip_ttl=64)
            exp_pkt1 = simple_tcp_packet(
                eth_dst='00:11:22:33:44:55',
                eth_src='00:77:66:55:44:33',
                ip_dst='10.10.10.1',
                ip_src='192.168.0.1',
                ip_id=106,
                ip_ttl=63)
            exp_pkt2 = simple_tcp_packet(
                eth_dst='00:11:22:33:44:56',
                eth_src='00:77:66:55:44:33',
                ip_dst='10.10.10.1',
                ip_src='192.168.0.1',
                ip_id=106,
                ip_ttl=63)

            print("Sending packet from port %d" % self.devports[0], " (192.168.0.1 -> 10.10.10.1) Host")
            send_packet(self, self.devports[0], pkt)
            verify_any_packet_any_port(self, [exp_pkt1, exp_pkt2],
                                       [self.devports[1], self.devports[2]], timeout=2)
        finally:
            pass

    def EcmpIPv4LpmTest(self):
        print("EcmpIPv4LpmTest()")
        try:
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='11.11.11.4',
                ip_src='192.168.0.1',
                ip_id=106,
                ip_ttl=64)
            exp_pkt1 = simple_tcp_packet(
                eth_dst='00:11:22:33:44:55',
                eth_src='00:77:66:55:44:33',
                ip_dst='11.11.11.4',
                ip_src='192.168.0.1',
                ip_id=106,
                ip_ttl=63)
            exp_pkt2 = simple_tcp_packet(
                eth_dst='00:11:22:33:44:56',
                eth_src='00:77:66:55:44:33',
                ip_dst='11.11.11.4',
                ip_src='192.168.0.1',
                ip_id=106,
                ip_ttl=63)

            print("Sending packet from port %d" % self.devports[0], " (192.168.0.1 -> 11.11.11.4) LPM")
            send_packet(self, self.devports[0], pkt)
            verify_any_packet_any_port(self, [exp_pkt1, exp_pkt2],
                                       [self.devports[1], self.devports[2]], timeout=2)
        finally:
            pass

    def EcmpIPv6HostTest(self):
        print("EcmpIPv6HostTest()")
        pkt = simple_tcpv6_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
            ipv6_src='2000::1',
            ipv6_hlim=64)
        exp_pkt1 = simple_tcpv6_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src='00:77:66:55:44:33',
            ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
            ipv6_src='2000::1',
            ipv6_hlim=63)
        exp_pkt2 = simple_tcpv6_packet(
            eth_dst='00:11:22:33:44:56',
            eth_src='00:77:66:55:44:33',
            ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
            ipv6_src='2000::1',
            ipv6_hlim=63)

        try:
            print("Sending packet from port %d" % self.devports[0], " (2000::1 -> 1234:5678:9abc:def0:4422:1133:5577:99aa) Host")
            send_packet(self, self.devports[0], pkt)
            verify_any_packet_any_port(self, [exp_pkt1, exp_pkt2],
                                       [self.devports[1], self.devports[2]], timeout=2)
        finally:
            pass

    def EcmpIPv6LpmTest(self):
        print("EcmpIPv6LpmTest()")
        pkt = simple_tcpv6_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ipv6_dst='4000::1',
            ipv6_src='2000::1',
            ipv6_hlim=64)
        exp_pkt1 = simple_tcpv6_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src='00:77:66:55:44:33',
            ipv6_dst='4000::1',
            ipv6_src='2000::1',
            ipv6_hlim=63)
        exp_pkt2 = simple_tcpv6_packet(
            eth_dst='00:11:22:33:44:56',
            eth_src='00:77:66:55:44:33',
            ipv6_dst='4000::1',
            ipv6_src='2000::1',
            ipv6_hlim=63)

        try:
            print("Sending packet from port %d" % self.devports[0], " (2000::1 -> 4000::1/124) LPM")
            send_packet(self, self.devports[0], pkt)
            verify_any_packet_any_port(self, [exp_pkt1, exp_pkt2],
                                       [self.devports[1], self.devports[2]], timeout=2)
        finally:
            pass

    def EcmpIPv4LagTest(self):
        print("EcmpIPv4LagTest()")
        try:
            max_itrs = 200
            def packetTest():
                count = [0, 0, 0, 0, 0, 0, 0]
                dst_ip = int(binascii.hexlify(socket.inet_aton('10.100.10.1')), 16)
                src_ip = int(binascii.hexlify(socket.inet_aton('192.168.8.1')), 16)
                for i in range(0, max_itrs):
                    dst_ip_addr = socket.inet_ntoa(
                        binascii.unhexlify(format(dst_ip, 'x').zfill(8)))
                    src_ip_addr = socket.inet_ntoa(
                        binascii.unhexlify(format(src_ip, 'x').zfill(8)))
                    pkt = simple_tcp_packet(
                        eth_dst='00:77:66:55:44:33',
                        eth_src='00:22:22:22:22:22',
                        ip_dst=dst_ip_addr,
                        ip_src=src_ip_addr,
                        ip_id=106,
                        ip_ttl=64)

                    exp_pkt1 = simple_tcp_packet(
                        eth_dst='00:11:22:33:44:57',
                        eth_src='00:77:66:55:44:33',
                        ip_dst=dst_ip_addr,
                        ip_src=src_ip_addr,
                        ip_id=106,
                        ip_ttl=63)
                    exp_pkt2 = simple_tcp_packet(
                        eth_dst='00:11:22:33:44:58',
                        eth_src='00:77:66:55:44:33',
                        ip_dst=dst_ip_addr,
                        ip_src=src_ip_addr,
                        ip_id=106,
                        ip_ttl=63)

                    send_packet(self, self.devports[0], pkt)
                    rcv_idx = verify_any_packet_any_port(
                        self, [exp_pkt1, exp_pkt2], [
                            self.devports[3], self.devports[4], self.devports[5],
                            self.devports[6], self.devports[7], self.devports[8], self.devports[9]
                        ], timeout=2)
                    count[rcv_idx] += 1
                    dst_ip += 1
                    src_ip += 1
                return count

            count = packetTest()
            print('Base: ecmp-count:', count)
            ecmp_count = [
                (count[0] + count[1] + count[2]),
                (count[3] + count[4] + count[5] + count[6])
            ]
            for i in range(0, 2):
                self.assertTrue((ecmp_count[i] >= ((max_itrs / 2) * 0.5)),
                                "Ecmp paths are not equally balanced")
            for i in range(0, 3):
                self.assertTrue((count[i] >= ((max_itrs / 6) * 0.5)),
                                "Lag path1 is not equally balanced")
            for i in range(3, 6):
                self.assertTrue((count[i] >= ((max_itrs / 6) * 0.5)),
                                "Lag path2 is not equally balanced")
            self.assertTrue(count[6] == 0)
            lag_mbr13 = self.add_lag_member(self.device, lag_handle=self.lag1, port_handle=self.port9)
            count = packetTest()
            print('Add port %d to lag 1, ecmp-count:' % (self.devports[9]), count)
            ecmp_count = [
                (count[0] + count[1] + count[2]),
                (count[3] + count[4] + count[5] + count[6])
            ]
            for i in range(0, 2):
                self.assertTrue((ecmp_count[i] >= ((max_itrs / 2) * 0.5)),
                                "Ecmp paths are not equally balanced")
            for i in range(0, 3):
                self.assertTrue((count[i] >= ((max_itrs / 7) * 0.5)),
                                "Lag path1 is not equally balanced")
            for i in range(3, 7):
                self.assertTrue((count[i] >= ((max_itrs / 7) * 0.5)),
                                "Lag path2 is not equally balanced")
            self.cleanlast()
            lag_mbr03 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port9)
            count = packetTest()
            print('Remove port %d from lag 1 and add to lag 0, ecmp-count:' % (self.devports[9]), count)
            ecmp_count = [
                (count[0] + count[1] + count[2] + count[6]),
                (count[3] + count[4] + count[5])
            ]
            for i in range(0, 2):
                self.assertTrue((ecmp_count[i] >= ((max_itrs / 2) * 0.5)),
                                "Ecmp paths are not equally balanced")
            for i in [0, 1, 2, 6]:
                self.assertTrue((count[i] >= ((max_itrs / 7) * 0.5)),
                                "Lag path1 is not equally balanced")
            for i in range(3, 6):
                self.assertTrue((count[i] >= ((max_itrs / 7) * 0.5)),
                                "Lag path2 is not equally balanced")
            self.cleanlast()
            count = packetTest()
            print('Remove port %d from lag 0, ecmp-count:' % (self.devports[9]), count)
            ecmp_count = [
                (count[0] + count[1] + count[2]),
                (count[3] + count[4] + count[5] + count[6])
            ]
            for i in range(0, 2):
                self.assertTrue((ecmp_count[i] >= ((max_itrs / 2) * 0.5)),
                                "Ecmp paths are not equally balanced")
            for i in range(0, 3):
                self.assertTrue((count[i] >= ((max_itrs / 6) * 0.5)),
                                "Lag path1 is not equally balanced")
            for i in range(3, 6):
                self.assertTrue((count[i] >= ((max_itrs / 6) * 0.5)),
                                "Lag path2 is not equally balanced")
            self.assertTrue(count[6] == 0)
        finally:
            pass

    def EcmpRebalanceTest(self):
        print("EcmpRebalanceTest()")
        print("Test to verify ECMP load balancing after adding/deleting members")

        ecmp2 = self.add_ecmp(self.device)
        route_rb = self.add_route(self.device, ip_prefix='13.1.1.0/24', vrf_handle=self.vrf10, nexthop_handle=ecmp2)
        for l_port in [self.port9, self.port10, self.port11, self.port12]:
            l_rif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=l_port, vrf_handle=self.vrf10, src_mac=self.rmac)
            l_ip = '13.0.0.' + str(l_port & 256)
            l_mac = '00:22:22:33:44:' + str(l_port & 0xFF)
            l_nhop = self.add_nexthop(self.device, handle=l_rif, dest_ip=l_ip)
            l_neighbor = self.add_neighbor(self.device, mac_address=l_mac, handle=l_rif, dest_ip=l_ip)
            ecmp_member = self.add_ecmp_member(self.device, nexthop_handle=l_nhop, ecmp_handle=ecmp2)

        pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='13.1.1.1',
            ip_src='192.168.0.1',
            ip_id=106,
            ip_ttl=64)
        exp_pkt1 = simple_tcp_packet(
            eth_dst='00:22:22:33:44:' + str(self.port9 & 0xFF),
            eth_src='00:77:66:55:44:33',
            ip_dst='13.1.1.1',
            ip_src='192.168.0.1',
            ip_id=106,
            ip_ttl=63)
        exp_pkt2 = simple_tcp_packet(
            eth_dst='00:22:22:33:44:' + str(self.port10 & 0xFF),
            eth_src='00:77:66:55:44:33',
            ip_dst='13.1.1.1',
            ip_src='192.168.0.1',
            ip_id=106,
            ip_ttl=63)
        exp_pkt3 = simple_tcp_packet(
            eth_dst='00:22:22:33:44:' + str(self.port11 & 0xFF),
            eth_src='00:77:66:55:44:33',
            ip_dst='13.1.1.1',
            ip_src='192.168.0.1',
            ip_id=106,
            ip_ttl=63)
        exp_pkt4 = simple_tcp_packet(
            eth_dst='00:22:22:33:44:' + str(self.port12 & 0xFF),
            eth_src='00:77:66:55:44:33',
            ip_dst='13.1.1.1',
            ip_src='192.168.0.1',
            ip_id=106,
            ip_ttl=63)
        try:
            max_itrs = 120
            def packetTest():
                count = [0, 0, 0, 0]
                dst_ip = int(binascii.hexlify(socket.inet_aton('13.1.1.1')), 16)
                src_ip = int(binascii.hexlify(socket.inet_aton('192.168.8.1')), 16)
                for i in range(0, max_itrs):
                    dst_ip_addr = socket.inet_ntoa(
                        binascii.unhexlify(format(dst_ip, 'x').zfill(8)))
                    src_ip_addr = socket.inet_ntoa(
                        binascii.unhexlify(format(src_ip, 'x').zfill(8)))
                    pkt['IP'].dst = dst_ip_addr
                    pkt['IP'].src = src_ip_addr
                    exp_pkt1['IP'].dst = dst_ip_addr
                    exp_pkt1['IP'].src = src_ip_addr
                    exp_pkt2['IP'].dst = dst_ip_addr
                    exp_pkt2['IP'].src = src_ip_addr
                    exp_pkt3['IP'].dst = dst_ip_addr
                    exp_pkt3['IP'].src = src_ip_addr
                    exp_pkt4['IP'].dst = dst_ip_addr
                    exp_pkt4['IP'].src = src_ip_addr

                    send_packet(self, self.devports[0], pkt)
                    rcv_idx = verify_any_packet_any_port(
                        self, [exp_pkt1, exp_pkt2, exp_pkt3, exp_pkt4], [
                            self.devports[9], self.devports[10], self.devports[11], self.devports[12]
                        ], timeout=2)
                    count[rcv_idx] += 1
                    dst_ip += 1
                    src_ip += 1
                return count

            lb_count = packetTest()
            print('ecmp-count:', lb_count)
            for i in range(0, 4):
                self.assertTrue((lb_count[i] >= ((max_itrs / 4) * 0.7)),
                                "Ecmp paths are not equally balanced")

            print("Remove last ecmp member")
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

            lb_count = packetTest()
            print('ecmp-count:', lb_count)
            for i in range(0, 3):
                self.assertTrue((lb_count[i] >= ((max_itrs / 3) * 0.7)),
                                "Ecmp paths are not equally balanced")
            self.assertEqual(lb_count[3], 0)

            print("Remove last ecmp member")
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

            lb_count = packetTest()
            print('ecmp-count:', lb_count)
            for i in range(0, 2):
                self.assertTrue((lb_count[i] >= ((max_itrs / 2) * 0.7)),
                                "Ecmp paths are not equally balanced")
            self.assertEqual(lb_count[2], 0)
            self.assertEqual(lb_count[3], 0)

            print("Remove last ecmp member")
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

            lb_count = packetTest()
            print('ecmp-count:', lb_count)
            for i in range(0, 1):
                self.assertTrue((lb_count[i] >= ((max_itrs / 1) * 0.7)),
                                "Ecmp paths are not equally balanced")
            self.assertEqual(lb_count[1], 0)
            self.assertEqual(lb_count[2], 0)
            self.assertEqual(lb_count[3], 0)

            print("Add back 3 ecmp members")
            for l_port in [self.port10, self.port11, self.port12]:
                l_rif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=l_port, vrf_handle=self.vrf10, src_mac=self.rmac)
                l_ip = '13.0.0.' + str(l_port & 256)
                l_mac = '00:22:22:33:44:' + str(l_port & 0xFF)
                l_nhop = self.add_nexthop(self.device, handle=l_rif, dest_ip=l_ip)
                l_neighbor = self.add_neighbor(self.device, mac_address=l_mac, handle=l_rif, dest_ip=l_ip)
                ecmp_member = self.add_ecmp_member(self.device, nexthop_handle=l_nhop, ecmp_handle=ecmp2)
            lb_count = packetTest()
            print('ecmp-count:', lb_count)
            for i in range(0, 4):
                self.assertTrue((lb_count[i] >= ((max_itrs / 4) * 0.7)),
                                "Ecmp paths are not equally balanced")
        finally:
            pass

###############################################################################

@group('l3')
@group('ecmp')
class L3ECMPLBTest(ApiHelper):
    def setUp(self):
        print()
        print("ECMP load balancing test")

        self.configure()

        rif0 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port0, vrf_handle=self.vrf10, src_mac=self.rmac)

        # L3 RIF on access port
        rif1 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port1, vrf_handle=self.vrf10, src_mac=self.rmac)
        nhop1 = self.add_nexthop(self.device, handle=rif1, dest_ip='10.10.0.1')
        neighbor1 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=rif1, dest_ip='10.10.0.1')

        # L3 RIF on access lag
        self.lag0 = self.add_lag(self.device)
        lag_mbr00 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port2)
        lag_mbr01 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port3)
        lag_mbr02 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port4)
        rif2 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.lag0, vrf_handle=self.vrf10, src_mac=self.rmac)
        nhop2 = self.add_nexthop(self.device, handle=rif2, dest_ip='10.10.0.2')
        neighbor2 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=rif2, dest_ip='10.10.0.2')

        # SVI vlan10 RIF on access port
        rif3 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_VLAN, vlan_handle=self.vlan10, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port5)
        self.attribute_set(self.port5, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        nhop3 = self.add_nexthop(self.device, handle=rif3, dest_ip='20.20.0.2')
        neighbor3 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:57', handle=rif3, dest_ip='20.20.0.2')
        mac3 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:11:22:33:44:57', destination_handle=self.port5)

        # SVI vlan20 RIF on trunk lag
        rif4 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_VLAN, vlan_handle=self.vlan20, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.lag1 = self.add_lag(self.device)
        lag_mbr10 = self.add_lag_member(self.device, lag_handle=self.lag1, port_handle=self.port6)
        lag_mbr11 = self.add_lag_member(self.device, lag_handle=self.lag1, port_handle=self.port7)
        lag_mbr12 = self.add_lag_member(self.device, lag_handle=self.lag1, port_handle=self.port8)
        self.add_vlan_member(self.device, vlan_handle=self.vlan20, member_handle=self.lag1, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
        nhop4 = self.add_nexthop(self.device, handle=rif4, dest_ip='40.40.0.2')
        neighbor4 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:58', handle=rif4, dest_ip='40.40.0.2')
        mac4 = self.add_mac_entry(self.device, vlan_handle=self.vlan20, mac_address='00:11:22:33:44:58', destination_handle=self.lag1)

        # SVI vlan30 RIF on trunk port
        rif5 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_VLAN, vlan_handle=self.vlan30, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.add_vlan_member(self.device, vlan_handle=self.vlan30, member_handle=self.port9, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
        nhop5 = self.add_nexthop(self.device, handle=rif5, dest_ip='30.30.0.2')
        neighbor5 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:59', handle=rif5, dest_ip='30.30.0.2')
        mac5 = self.add_mac_entry(self.device, vlan_handle=self.vlan30, mac_address='00:11:22:33:44:59', destination_handle=self.port9)

        ecmp0 = self.add_ecmp(self.device)
        ecmp_member01 = self.add_ecmp_member(self.device, nexthop_handle=nhop1, ecmp_handle=ecmp0)
        ecmp_member02 = self.add_ecmp_member(self.device, nexthop_handle=nhop2, ecmp_handle=ecmp0)
        ecmp_member03 = self.add_ecmp_member(self.device, nexthop_handle=nhop3, ecmp_handle=ecmp0)
        ecmp_member04 = self.add_ecmp_member(self.device, nexthop_handle=nhop4, ecmp_handle=ecmp0)
        ecmp_member05 = self.add_ecmp_member(self.device, nexthop_handle=nhop5, ecmp_handle=ecmp0)

        route0 = self.add_route(self.device, ip_prefix='11.11.11.0/24', vrf_handle=self.vrf10, nexthop_handle=ecmp0)

    def runTest(self):
        try:
            self.LBTest()
        finally:
            pass
    def tearDown(self):
        self.attribute_set(self.port5, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.cleanup()

    def LBTest(self):
        print("LBTest()")
        try:
            count = [0, 0, 0, 0, 0, 0, 0, 0, 0]
            dst_ip = int(binascii.hexlify(socket.inet_aton('11.11.11.1')), 16)
            src_ip = int(binascii.hexlify(socket.inet_aton('192.168.8.1')), 16)
            max_itrs = 200
            for i in range(0, max_itrs):
                dst_ip_addr = socket.inet_ntoa(
                    binascii.unhexlify(format(dst_ip, 'x').zfill(8)))
                src_ip_addr = socket.inet_ntoa(
                    binascii.unhexlify(format(src_ip, 'x').zfill(8)))
                pkt = simple_tcp_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src='00:22:22:22:22:22',
                    ip_dst=dst_ip_addr,
                    ip_src=src_ip_addr,
                    ip_id=106,
                    ip_ttl=64,
                    pktlen=100)

                exp_pkt1 = simple_tcp_packet(
                    eth_dst='00:11:22:33:44:55',
                    eth_src='00:77:66:55:44:33',
                    ip_dst=dst_ip_addr,
                    ip_src=src_ip_addr,
                    ip_id=106,
                    ip_ttl=63,
                    pktlen=100)
                exp_pkt2 = simple_tcp_packet(
                    eth_dst='00:11:22:33:44:56',
                    eth_src='00:77:66:55:44:33',
                    ip_dst=dst_ip_addr,
                    ip_src=src_ip_addr,
                    ip_id=106,
                    ip_ttl=63,
                    pktlen=100)
                exp_pkt3 = simple_tcp_packet(
                    eth_dst='00:11:22:33:44:57',
                    eth_src='00:77:66:55:44:33',
                    ip_dst=dst_ip_addr,
                    ip_src=src_ip_addr,
                    ip_id=106,
                    ip_ttl=63,
                    pktlen=100)
                exp_pkt4 = simple_tcp_packet(
                    eth_dst='00:11:22:33:44:58',
                    eth_src='00:77:66:55:44:33',
                    ip_dst=dst_ip_addr,
                    ip_src=src_ip_addr,
                    dl_vlan_enable=True,
                    vlan_vid=20,
                    ip_id=106,
                    ip_ttl=63,
                    pktlen=104)
                exp_pkt5 = simple_tcp_packet(
                    eth_dst='00:11:22:33:44:59',
                    eth_src='00:77:66:55:44:33',
                    ip_dst=dst_ip_addr,
                    ip_src=src_ip_addr,
                    dl_vlan_enable=True,
                    vlan_vid=30,
                    ip_id=106,
                    ip_ttl=63,
                    pktlen=104)

                send_packet(self, self.devports[0], pkt)
                rcv_idx = verify_any_packet_any_port(
                    self, [exp_pkt1, exp_pkt2, exp_pkt3, exp_pkt4, exp_pkt5], [
                        self.devports[1],
                        self.devports[2], self.devports[3], self.devports[4],
                        self.devports[5],
                        self.devports[6], self.devports[7], self.devports[8],
                        self.devports[9]
                    ], timeout=2)
                count[rcv_idx] += 1
                dst_ip += 1
                src_ip += 1

            ecmp_count = [
                (count[0]), (count[1] + count[2] + count[3]),
                (count[4]), (count[5] + count[6] + count[7]),
                (count[8])
            ]
            print('count:', count)
            print('ecmp-count:', ecmp_count)
            for i in range(0, 5):
                self.assertTrue((ecmp_count[i] >= ((max_itrs / 5) * 0.7)),
                                "Ecmp paths are not equally balanced")
        finally:
            pass

###############################################################################

@group('l3')
@group('ecmp')
class L3ECMPSharedNhopTest(ApiHelper):
    def setUp(self):
        print()
        print("ECMP nexthop shared test")

        self.configure()
        rif0 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT,
            port_handle=self.port0, vrf_handle=self.vrf10, src_mac=self.rmac)

        self.nhop = []
        self.vlans = [self.vlan10, self.vlan20, self.vlan30, self.vlan40]
        self.rifs = []
        self.vlan_ids = list(range(10,160,10))
        for i in range(50, 160, 10):
            self.vlans.append(self.add_vlan(self.device, vlan_id=i))
        for dp, vlan, port, vlan_id in zip(self.devports[1:], self.vlans, self.port_list[1:], self.vlan_ids):
            mac = '00:11:22:33:44:' + hex(dp & 0xff)[2:]
            ip = '20.20.0.' + str(dp & 0xff)
            rif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_VLAN,
                vlan_handle=vlan, vrf_handle=self.vrf10, src_mac=self.rmac)
            self.add_vlan_member(self.device, vlan_handle=vlan, member_handle=port)
            self.attribute_set(port, SWITCH_PORT_ATTR_PORT_VLAN_ID, vlan_id)
            self.add_mac_entry(self.device, vlan_handle=vlan, destination_handle=port, mac_address=mac)
            nhop = self.add_nexthop(self.device, handle=rif, dest_ip=ip)
            self.nhop.append(nhop)
            self.add_neighbor(self.device, mac_address=mac, handle=rif, dest_ip=ip)
        ecmp0 = self.add_ecmp(self.device)
        ecmp1 = self.add_ecmp(self.device)
        ecmp2 = self.add_ecmp(self.device)
        self.route0 = self.add_route(self.device, ip_prefix='40.40.0.0/16', vrf_handle=self.vrf10, nexthop_handle=ecmp0)
        self.route0 = self.add_route(self.device, ip_prefix='50.50.0.0/16', vrf_handle=self.vrf10, nexthop_handle=ecmp1)
        self.route0 = self.add_route(self.device, ip_prefix='60.60.0.0/16', vrf_handle=self.vrf10, nexthop_handle=ecmp2)
        for i in range(0,15,1):
            self.add_ecmp_member(self.device, nexthop_handle=self.nhop[i], ecmp_handle=ecmp0)
            self.add_ecmp_member(self.device, nexthop_handle=self.nhop[i], ecmp_handle=ecmp1)
            # use same nexthop twice in ecmp2
            self.add_ecmp_member(self.device, nexthop_handle=self.nhop[i], ecmp_handle=ecmp2)
            self.add_ecmp_member(self.device, nexthop_handle=self.nhop[i], ecmp_handle=ecmp2)

    def runTest(self):
        self.LBTest('40.40.0.1')
        self.LBTest('50.50.0.1')
        self.LBTest('60.60.0.1')

    def LBTest(self, dip):
        try:
            count = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
            dst_ip = int(binascii.hexlify(socket.inet_aton(dip)), 16)
            src_ip = int(binascii.hexlify(socket.inet_aton('192.168.8.1')), 16)
            max_itrs = 600
            pkt = simple_tcp_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src='00:22:22:22:22:22',
                    ip_dst='40.40.0.1',
                    ip_src='192.168.1.1',
                    ip_ttl=64)
            exp_pkts = []
            for dp in self.devports[1:]:
                mac = '00:11:22:33:44:' + hex(dp & 0xff)[2:]
                exp_pkt = simple_tcp_packet(
                    eth_dst=mac,
                    eth_src='00:77:66:55:44:33',
                    ip_dst='40.40.0.1',
                    ip_src='192.168.1.1',
                    ip_ttl=63)
                exp_pkts.append(exp_pkt)
            for i in range(0, max_itrs):
                dst_ip_addr = socket.inet_ntoa(
                    binascii.unhexlify(format(dst_ip, 'x').zfill(8)))
                src_ip_addr = socket.inet_ntoa(
                    binascii.unhexlify(format(src_ip, 'x').zfill(8)))
                pkt['IP'].dst = dst_ip_addr
                pkt['IP'].src = src_ip_addr
                for idx, dp in enumerate(self.devports[1:]):
                    exp_pkts[idx]['IP'].dst = dst_ip_addr
                    exp_pkts[idx]['IP'].src = src_ip_addr

                send_packet(self, self.devports[0], pkt)
                rcv_idx = verify_any_packet_any_port(
                    self, exp_pkts, self.devports[1:], timeout=4)
                count[rcv_idx] += 1
                dst_ip += 1
                src_ip += 1

            print('count:', count)
            for i in range(0,15):
                self.assertTrue((count[i] >= ((max_itrs / 15) * 0.4)),
                                "Ecmp paths are not equally balanced")
        finally:
            pass

    def tearDown(self):
        for port in self.port_list[1:]:
            self.attribute_set(port, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.cleanup()

###############################################################################


@group('l3')
@group('multi-vrf')
class L3MultiVrfTest(ApiHelper):
    def setUp(self):
        print()
        self.configure()
        self._BaseVrfConfig()

    def get_nexthop_ip(self, vrf, port):
        return '192.168.' + str(vrf) + "." + str(port+1)

    def get_neighbor_mac(self, port):
        return '00:11:11:11:11:' + str(hex(port+1)[2:]).zfill(2)

    def _BaseVrfConfig(self):
        self.rmac1 = '00:77:77:77:77:11'
        self.rmac2 = '00:77:77:77:77:22'
        self.vrf1 = self.add_vrf(self.device, id=2, src_mac=self.rmac1)
        self.vrf2 = self.add_vrf(self.device, id=3, src_mac=self.rmac2)

        #VRF1 Base config
        self.rif10 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port0,
                vrf_handle=self.vrf1, src_mac=self.rmac1)
        self.vlan1 = self.add_vlan(self.device, vlan_id=2)
        self.add_vlan_member(self.device, member_handle=self.port1, vlan_handle=self.vlan1)
        self.add_vlan_member(self.device, member_handle=self.port2, vlan_handle=self.vlan1,
                tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_PORT_VLAN_ID, 2)
        self.rif11 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_VLAN, vlan_handle=self.vlan1,
                vrf_handle=self.vrf1, src_mac=self.rmac1)
        self.lag10 = self.add_lag(self.device)
        self.lag10_mbr0 = self.add_lag_member(self.device, lag_handle=self.lag10, port_handle=self.port3)
        self.lag10_mbr1 = self.add_lag_member(self.device, lag_handle=self.lag10, port_handle=self.port4)
        self.rif12 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.lag10,
                vrf_handle=self.vrf1, src_mac=self.rmac1)
        self.rif13=self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_SUB_PORT, port_handle=self.port5,
                outer_vlan_id=20, vrf_handle=self.vrf1, src_mac=self.rmac1)
        self.rif14 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port6,
                vrf_handle=self.vrf1, src_mac=self.rmac1)
        #Configure routes for VRF1
        ip_addr = self.get_nexthop_ip(1, self.devports[0])
        mac_addr = self.get_neighbor_mac(self.devports[0])
        self.nhop10 = self.add_nexthop(self.device, handle=self.rif10, dest_ip=ip_addr)
        self.route10 = self.add_route(self.device, ip_prefix=ip_addr, vrf_handle=self.vrf1,
                nexthop_handle=self.nhop10)
        self.nbr10 = self.add_neighbor(self.device, handle=self.rif10, mac_address=mac_addr,
                dest_ip=ip_addr)

        ip_addr = self.get_nexthop_ip(1, self.devports[1])
        mac_addr = self.get_neighbor_mac(self.devports[1])
        self.nhop11 = self.add_nexthop(self.device, handle=self.rif11, dest_ip=ip_addr)
        self.route11 = self.add_route(self.device, ip_prefix=ip_addr, vrf_handle=self.vrf1,
                nexthop_handle=self.nhop11)
        self.nbr11 = self.add_neighbor(self.device, handle=self.rif11, mac_address=mac_addr,
                dest_ip=ip_addr)
        self.add_mac_entry(self.device, vlan_handle=self.vlan1, destination_handle=self.port1,
                mac_address=mac_addr)

        ip_addr = self.get_nexthop_ip(1, self.devports[2])
        mac_addr = self.get_neighbor_mac(self.devports[2])
        self.nhop12 = self.add_nexthop(self.device, handle=self.rif11, dest_ip=ip_addr)
        self.route12 = self.add_route(self.device, ip_prefix=ip_addr, vrf_handle=self.vrf1,
                nexthop_handle=self.nhop12)
        self.nbr12 = self.add_neighbor(self.device, handle=self.rif11, mac_address=mac_addr,
                dest_ip=ip_addr)
        self.add_mac_entry(self.device, vlan_handle=self.vlan1, destination_handle=self.port2,
                mac_address=mac_addr)

        ip_addr = self.get_nexthop_ip(1, self.devports[3])
        mac_addr = self.get_neighbor_mac(self.devports[3])
        self.nhop13 = self.add_nexthop(self.device, handle=self.rif12, dest_ip=ip_addr)
        self.route13 = self.add_route(self.device, ip_prefix=ip_addr, vrf_handle=self.vrf1,
                nexthop_handle=self.nhop13)
        self.nbr13 = self.add_neighbor(self.device, handle=self.rif12, mac_address=mac_addr,
                dest_ip=ip_addr)

        ip_addr = self.get_nexthop_ip(1, self.devports[5])
        mac_addr = self.get_neighbor_mac(self.devports[5])
        self.nhop14 = self.add_nexthop(self.device, handle=self.rif13, dest_ip=ip_addr)
        self.route14 = self.add_route(self.device, ip_prefix=ip_addr, vrf_handle=self.vrf1,
                nexthop_handle=self.nhop14)
        self.nbr14 = self.add_neighbor(self.device, handle=self.rif13, mac_address=mac_addr,
                dest_ip=ip_addr)

        self.nhop15 = self.add_nexthop(self.device, handle=self.rif14, dest_ip='1.1.1.1')
        self.route15 = self.add_route(self.device, ip_prefix='1.1.1.1', vrf_handle=self.vrf1,
                nexthop_handle=self.nhop15)
        self.nbr15 = self.add_neighbor(self.device, handle=self.rif14, mac_address='00:11:11:11:11:11', dest_ip='1.1.1.1')

        #VRF2 Base config
        self.rif20 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port7,
                vrf_handle=self.vrf2, src_mac=self.rmac2)
        self.vlan2 = self.add_vlan(self.device, vlan_id=3)
        self.add_vlan_member(self.device, member_handle=self.port8, vlan_handle=self.vlan2)
        self.add_vlan_member(self.device, member_handle=self.port9, vlan_handle=self.vlan2,
                tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
        self.attribute_set(self.port8, SWITCH_PORT_ATTR_PORT_VLAN_ID, 3)
        self.rif21 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_VLAN, vlan_handle=self.vlan2,
                vrf_handle=self.vrf2,src_mac=self.rmac2)
        self.lag20 = self.add_lag(self.device)
        self.lag20_mbr0 = self.add_lag_member(self.device, lag_handle=self.lag20, port_handle=self.port10)
        self.lag20_mbr1 = self.add_lag_member(self.device, lag_handle=self.lag20, port_handle=self.port11)
        self.rif22 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.lag20,
                vrf_handle=self.vrf2, src_mac=self.rmac2)
        self.rif23=self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_SUB_PORT, port_handle=self.port12,
                outer_vlan_id=30, vrf_handle=self.vrf2, src_mac=self.rmac2)
        self.rif24 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port13,
                vrf_handle=self.vrf2, src_mac=self.rmac2)
        #Configure routes for VRF2
        ip_addr = self.get_nexthop_ip(2, self.devports[7])
        mac_addr = self.get_neighbor_mac(self.devports[7])
        self.nhop20 = self.add_nexthop(self.device, handle=self.rif20, dest_ip=ip_addr)
        self.route20 = self.add_route(self.device, ip_prefix=ip_addr, vrf_handle=self.vrf2,
                nexthop_handle=self.nhop20)
        self.nbr20 = self.add_neighbor(self.device, handle=self.rif20, mac_address=mac_addr,
                dest_ip=ip_addr)

        ip_addr = self.get_nexthop_ip(2, self.devports[8])
        mac_addr = self.get_neighbor_mac(self.devports[8])
        self.nhop21 = self.add_nexthop(self.device, handle=self.rif21, dest_ip=ip_addr)
        self.route21 = self.add_route(self.device, ip_prefix=ip_addr, vrf_handle=self.vrf2,
                nexthop_handle=self.nhop21)
        self.nbr21 = self.add_neighbor(self.device, handle=self.rif21, mac_address=mac_addr,
                dest_ip=ip_addr)
        self.add_mac_entry(self.device, vlan_handle=self.vlan2, destination_handle=self.port8,
                mac_address=mac_addr)

        ip_addr = self.get_nexthop_ip(2, self.devports[9])
        mac_addr = self.get_neighbor_mac(self.devports[9])
        self.nhop22 = self.add_nexthop(self.device, handle=self.rif21, dest_ip=ip_addr)
        self.route22 = self.add_route(self.device, ip_prefix=ip_addr, vrf_handle=self.vrf2,
                nexthop_handle=self.nhop22)
        self.nbr22 = self.add_neighbor(self.device, handle=self.rif21, mac_address=mac_addr,
                dest_ip=ip_addr)
        self.add_mac_entry(self.device, vlan_handle=self.vlan2, destination_handle=self.port9,
                mac_address=mac_addr)

        ip_addr = self.get_nexthop_ip(2, self.devports[10])
        mac_addr = self.get_neighbor_mac(self.devports[10])
        self.nhop23 = self.add_nexthop(self.device, handle=self.rif22, dest_ip=ip_addr)
        self.route23 = self.add_route(self.device, ip_prefix=ip_addr, vrf_handle=self.vrf2,
                nexthop_handle=self.nhop23)
        self.nbr23 = self.add_neighbor(self.device, handle=self.rif22, mac_address=mac_addr,
                dest_ip=ip_addr)

        ip_addr = self.get_nexthop_ip(2, self.devports[12])
        mac_addr = self.get_neighbor_mac(self.devports[12])
        self.nhop24 = self.add_nexthop(self.device, handle=self.rif23, dest_ip=ip_addr)
        self.route24 = self.add_route(self.device, ip_prefix=ip_addr, vrf_handle=self.vrf2,
                nexthop_handle=self.nhop24)
        self.nbr24 = self.add_neighbor(self.device, handle=self.rif23, mac_address=mac_addr,
                dest_ip=ip_addr)

        self.nhop25 = self.add_nexthop(self.device, handle=self.rif24, dest_ip='1.1.1.1')
        self.route25 = self.add_route(self.device, ip_prefix='1.1.1.1', vrf_handle=self.vrf2,
                nexthop_handle=self.nhop25)
        self.nbr25 = self.add_neighbor(self.device, handle=self.rif24, mac_address='00:11:11:11:11:11', dest_ip='1.1.1.1')

        #Configure overlapping routrs
        self.nhop16 = self.add_nexthop(self.device, handle=self.rif14, dest_ip='172.31.1.1')
        self.nbr16 = self.add_neighbor(self.device, handle=self.rif14, mac_address='00:11:11:11:11:11', dest_ip='172.31.1.1')
        self.nhop17 = self.add_nexthop(self.device, handle=self.rif14, dest_ip='fd00::1000:1')
        self.nbr17 = self.add_neighbor(self.device, handle=self.rif14, mac_address='00:11:11:11:11:11',
                dest_ip='fd00::1000:1')
        self.nhop26 = self.add_nexthop(self.device, handle=self.rif24, dest_ip='172.31.2.1')
        self.nbr26 = self.add_neighbor(self.device, handle=self.rif24, mac_address='00:11:11:11:11:22',
                dest_ip='172.31.2.1')
        self.nhop27 = self.add_nexthop(self.device, handle=self.rif24, dest_ip='fd00::2000:1')
        self.nbr27 = self.add_neighbor(self.device, handle=self.rif24, mac_address='00:11:11:11:11:22',
                dest_ip='fd00::2000:1')
        self.overlapping_route_config = {
                'vrf1_v4_host_route':{'ip_prefix':'172.16.1.1', 'vrf_handle':self.vrf1, 'nexthop_handle':self.nhop16},
                'vrf2_v4_host_route':{'ip_prefix':'172.16.1.1', 'vrf_handle':self.vrf2, 'nexthop_handle':self.nhop26},
                'vrf1_v4_lpm_route':{'ip_prefix':'172.16.2.0/25', 'vrf_handle':self.vrf1, 'nexthop_handle':self.nhop16},
                'vrf2_v4_lpm_host_route':{'ip_prefix':'172.16.2.0/24', 'vrf_handle':self.vrf2, 'nexthop_handle':self.nhop26},
                'vrf1_v6_host_route':{'ip_prefix':'fd00::1', 'vrf_handle':self.vrf1, 'nexthop_handle':self.nhop17},
                'vrf2_v6_host_route':{'ip_prefix':'fd00::1', 'vrf_handle':self.vrf2, 'nexthop_handle':self.nhop27},
                'vrf1_v6_lpm_route':{'ip_prefix':'fd00::0010:0/112', 'vrf_handle':self.vrf1, 'nexthop_handle':self.nhop17},
                'vrf2_v6_lpm_route':{'ip_prefix':'fd00::0010:0/108', 'vrf_handle':self.vrf2, 'nexthop_handle':self.nhop27},
                }
        for route in self.overlapping_route_config:
            r = self.add_route(self.device, **self.overlapping_route_config[route])

    def runTest(self):
        try:
            self.L3VrfIPv4UnicastStatusTest()
            self.L3VrfIPv4UnicastStatusTest(status = False)
            self.L3VrfRmacTest()
            self.L3VrfIsolationTests()
            self.L3InterVrfAclRedirectTest()
            self.L3VrfInterfacesTest()
            self.L3VrfScaleTest()
        finally:
            pass

    def L3VrfInterfacesTest(self):
        print("L3VrfInterfacesTest()")
        try:
            for vrf in [self.vrf1, self.vrf2]:
                if vrf == self.vrf1:
                    vrf=1
                    rmac = self.rmac1
                    vports = self.devports[:7]
                    vlan_id=2
                    subport_vlan_id=20
                else:
                    vrf=2
                    rmac = self.rmac2
                    vports = self.devports[7:14]
                    vlan_id=3
                    subport_vlan_id=30
                vrf_pkt = simple_tcp_packet(
                    eth_dst=rmac,
                    eth_src='00:11:11:11:11:11',
                    ip_src='1.1.1.1',
                    ip_dst='192.168.1.1',
                    ip_id=105,
                    ip_ttl=64)
                vrf_exp_pkt = simple_tcp_packet(
                    eth_dst='00:11:11:11:11:01',
                    eth_src=rmac,
                    ip_src='1.1.1.1',
                    ip_dst='192.168.1.1',
                    ip_id=105,
                    ip_ttl=63)
                vrf_exp_vlan_pkt = simple_tcp_packet(
                    eth_dst='00:11:11:11:11:01',
                    eth_src=rmac,
                    ip_src='1.1.1.1',
                    ip_dst='192.168.1.1',
                    dl_vlan_enable=True,
                    vlan_vid=vlan_id,
                    ip_id=105,
                    ip_ttl=63,
                    pktlen=104)

                vrf_pkt[IP].dst=self.get_nexthop_ip(vrf, vports[0])
                vrf_exp_pkt[IP].dst=vrf_pkt[IP].dst
                vrf_exp_pkt[Ether].dst=self.get_neighbor_mac(vports[0])
                print("VRF{} Sending packet DST IP:{} from L3 RIF port{} to L3 RIF port{}".format(vrf, vrf_pkt[IP].dst, vports[6], vports[0]))
                send_packet(self, vports[6], vrf_pkt)
                verify_packet(self, vrf_exp_pkt, vports[0])

                vrf_pkt[IP].dst=self.get_nexthop_ip(vrf, vports[1])
                vrf_exp_pkt[IP].dst=vrf_pkt[IP].dst
                vrf_exp_pkt[Ether].dst=self.get_neighbor_mac(vports[1])
                print("VRF{}Sending packet DST IP:{} from L3 RIF port{} to L3 SVI RIF port{}(Untagged)".format(vrf, vrf_pkt[IP].dst, vports[6], vports[1]))
                send_packet(self, vports[6], vrf_pkt)
                verify_packet(self, vrf_exp_pkt, vports[1])

                vrf_pkt[IP].dst=self.get_nexthop_ip(vrf, vports[2])
                vrf_exp_vlan_pkt[IP].dst=vrf_pkt[IP].dst
                vrf_exp_vlan_pkt[Ether].dst=self.get_neighbor_mac(vports[2])
                print("VRF{} Sending packet DST IP:{} from L3 RIF port{} to L3 SVI RIF port{}(Tagged)".format(vrf, vrf_pkt[IP].dst, vports[6], vports[2]))
                send_packet(self, vports[6], vrf_pkt)
                verify_packet(self, vrf_exp_vlan_pkt, vports[2])

                vrf_pkt[IP].dst=self.get_nexthop_ip(vrf, vports[3])
                vrf_exp_pkt[IP].dst=vrf_pkt[IP].dst
                vrf_exp_pkt[Ether].dst=self.get_neighbor_mac(vports[3])
                print("VRF{} Sending packet DST IP:{} from L3 RIF port{} to L3 LAG RIF ports[{},{}]".format(vrf, vrf_pkt[IP].dst, vports[6], vports[3], vports[4]))
                send_packet(self, vports[6], vrf_pkt)
                verify_packets_any(self, vrf_exp_pkt, [vports[3], vports[4]])

                vrf_pkt[IP].dst=self.get_nexthop_ip(vrf, vports[5])
                vrf_exp_vlan_pkt[IP].dst=vrf_pkt[IP].dst
                vrf_exp_vlan_pkt[Ether].dst=self.get_neighbor_mac(vports[5])
                vrf_exp_vlan_pkt[Ether][Dot1Q].vlan = subport_vlan_id
                print("VRF{} ending packet DST IP:{} from L3 RIF port{} to L3 SubportRIF port{}".format(vrf, vrf_pkt[IP].dst, vports[6], vports[5]))
                send_packet(self, vports[6], vrf_pkt)
                verify_packet(self, vrf_exp_vlan_pkt, vports[5])
        finally:
            pass

    def L3VrfIsolationTests(self):
        print("L3VrfIsolationTests()")
        self._L3VrfRouteIsolationTests()

    def L3VrfRmacTest(self):
        print ("L3VrfRmacTest - L3 Vrf packet forward/drop test for rmac match/nomatch")

        vrf1_rmac_miss_pkt = simple_tcp_packet(
            eth_dst=self.rmac2,
            eth_src='00:11:11:11:11:11',
            ip_src='1.1.1.1',
            ip_dst=self.get_nexthop_ip(1, self.devports[0]),
            ip_id=105,
            ip_ttl=64)
        vrf2_rmac_miss_pkt = simple_tcp_packet(
            eth_dst=self.rmac1,
            eth_src='00:11:11:11:11:11',
            ip_src='1.1.1.1',
            ip_dst=self.get_nexthop_ip(2, self.devports[7]),
            ip_id=105,
            ip_ttl=64)
        vrf1_rmac_hit_pkt = simple_tcp_packet(
            eth_dst=self.rmac1,
            eth_src='00:11:11:11:11:11',
            ip_src='1.1.1.1',
            ip_dst=self.get_nexthop_ip(1, self.devports[0]),
            ip_id=105,
            ip_ttl=64)
        vrf2_rmac_hit_pkt = simple_tcp_packet(
            eth_dst=self.rmac2,
            eth_src='00:11:11:11:11:11',
            ip_src='1.1.1.1',
            ip_dst=self.get_nexthop_ip(2, self.devports[7]),
            ip_id=105,
            ip_ttl=64)
        vrf1_exp_pkt = simple_tcp_packet(
            eth_dst=self.get_neighbor_mac(self.devports[0]),
            eth_src=self.rmac1,
            ip_src='1.1.1.1',
            ip_dst=self.get_nexthop_ip(1, self.devports[0]),
            ip_id=105,
            ip_ttl=63)
        vrf2_exp_pkt = simple_tcp_packet(
            eth_dst=self.get_neighbor_mac(self.devports[7]),
            eth_src=self.rmac2,
            ip_src='1.1.1.1',
            ip_dst=self.get_nexthop_ip(2, self.devports[7]),
            ip_id=105,
            ip_ttl=63)

        try:
            print("VRF1(RMAC Miss): Sending packet (DST IP: 192.168.1.1) from port6 ")
            send_packet(self, self.devports[6], vrf1_rmac_miss_pkt)
            verify_no_other_packets(self, timeout=1)

            print("VRF2(RMAC Miss): Sending packet (DST IP: 192.168.2.8) from port13 ")
            send_packet(self, self.devports[13], vrf2_rmac_miss_pkt)
            verify_no_other_packets(self, timeout=1)

            print("VRF1(RMAC Hit): Sending packet (DST IP: 192.168.1.1) from port6 to port0 ")
            send_packet(self, self.devports[6], vrf1_rmac_hit_pkt)
            verify_packet(self, vrf1_exp_pkt, self.devports[0])

            print("VRF2(RMAC Hit): Sending packet (DST IP: 192.168.2.8) from port13 to port7 ")
            send_packet(self, self.devports[13], vrf2_rmac_hit_pkt)
            verify_packet(self, vrf2_exp_pkt, self.devports[7])
        finally:
            pass


    def _L3VrfRouteIsolationTests(self):
        print("L3VrfRouteIsolationTests()")
        #self.overlapping_route_config = {
        #        'vrf1_v4_host_route':{'ip_prefix':'172.16.1.1', 'vrf_handle':self.vrf1, 'nexthop_handle':self.nhop16},
        #        'vrf2_v4_host_route':{'ip_prefix':'172.16.1.1', 'vrf_handle':self.vrf2, 'nexthop_handle':self.nhop26},
        #        'vrf1_v4_lpm_route':{'ip_prefix':'172.16.2.0/25', 'vrf_handle':self.vrf1, 'nexthop_handle':self.nhop16},
        #        'vrf2_v4_lpm_host_route':{'ip_prefix':'172.16.2.0/24', 'vrf_handle':self.vrf2, 'nexthop_handle':self.nhop26},
        #        'vrf1_v6_host_route':{'ip_prefix':'fd00::1', 'vrf_handle':self.vrf1, 'nexthop_handle':self.nhop17},
        #        'vrf2_v6_host_route':{'ip_prefix':'fd00::1', 'vrf_handle':self.vrf2, 'nexthop_handle':self.nhop27},
        #        'vrf1_v6_lpm_route':{'ip_prefix':'fd00::0010:0/112', 'vrf_handle':self.vrf1, 'nexthop_handle':self.nhop17},
        #        'vrf2_v6_lpm_route':{'ip_prefix':'fd00::0010:0/108', 'vrf_handle':self.vrf2, 'nexthop_handle':self.nhop27},
        #        }
        vrf1_v4_pkt = simple_tcp_packet(
            eth_dst=self.rmac1,
            eth_src='00:22:22:22:22:22',
            ip_dst='172.16.1.1',
            ip_src='192.168.1.1',
            ip_id=105,
            ip_ttl=64)
        vrf2_v4_pkt = simple_tcp_packet(
            eth_dst=self.rmac2,
            eth_src='00:22:22:22:22:22',
            ip_dst='172.16.1.1',
            ip_src='192.168.2.8',
            ip_id=105,
            ip_ttl=64)
        vrf1_v4_exp_pkt = simple_tcp_packet(
            eth_dst='00:11:11:11:11:11',
            eth_src=self.rmac1,
            ip_dst='172.16.1.1',
            ip_src='192.168.1.1',
            ip_id=105,
            ip_ttl=63)
        vrf2_v4_exp_pkt = simple_tcp_packet(
            eth_dst='00:11:11:11:11:22',
            eth_src=self.rmac2,
            ip_dst='172.16.1.1',
            ip_src='192.168.2.8',
            ip_id=105,
            ip_ttl=63)
        vrf1_v6_pkt = simple_tcpv6_packet(
            eth_dst=self.rmac1,
            eth_src='00:22:22:22:22:22',
            ipv6_dst='fd00::1',
            ipv6_src='2000::1',
            ipv6_hlim=64)
        vrf2_v6_pkt = simple_tcpv6_packet(
            eth_dst=self.rmac2,
            eth_src='00:22:22:22:22:22',
            ipv6_dst='fd00::1',
            ipv6_src='3000::1',
            ipv6_hlim=64)
        vrf1_v6_exp_pkt = simple_tcpv6_packet(
            eth_dst='00:11:11:11:11:11',
            eth_src=self.rmac1,
            ipv6_dst='fd00::1',
            ipv6_src='2000::1',
            ipv6_hlim=63)
        vrf2_v6_exp_pkt = simple_tcpv6_packet(
            eth_dst='00:11:11:11:11:22',
            eth_src=self.rmac2,
            ipv6_dst='fd00::1',
            ipv6_src='3000::1',
            ipv6_hlim=63)
        try:
            print("VRF1(overlapping v4 host route): Sending packet (DST IP: 172.16.1.1) from port0 to port6")
            send_packet(self, self.devports[0], vrf1_v4_pkt)
            verify_packet(self, vrf1_v4_exp_pkt, self.devports[6])

            print("VRF2(overlapping v4 host route): Sending packet (DST IP: 172.16.1.1) from port7 to port13")
            send_packet(self, self.devports[7], vrf2_v4_pkt)
            verify_packet(self, vrf2_v4_exp_pkt, self.devports[13])

            print("VRF1(overlapping v4 subnet[172.16.2.0/25]): Sending packet (DST IP: 172.16.2.1) from port0 to port6")
            vrf1_v4_pkt[IP].dst = '172.16.2.1'
            vrf1_v4_exp_pkt[IP].dst = vrf1_v4_pkt[IP].dst
            send_packet(self, self.devports[0], vrf1_v4_pkt)
            verify_packet(self, vrf1_v4_exp_pkt, self.devports[6])

            print("VRF2(overlapping v4 subnet[172.16.2.0/24]): Sending packet (DST IP: 172.16.2.1) from port7 to port13")
            vrf2_v4_pkt[IP].dst = '172.16.2.1'
            vrf2_v4_exp_pkt[IP].dst = vrf2_v4_pkt[IP].dst
            send_packet(self, self.devports[7], vrf2_v4_pkt)
            verify_packet(self, vrf2_v4_exp_pkt, self.devports[13])

            print("VRF1(overlapping v4 subnet[172.16.2.0/25]): Sending packet (DST IP:172.16.2.129) from port0")
            vrf1_v4_pkt[IP].dst = '172.16.2.129'
            vrf1_v4_exp_pkt[IP].dst = vrf1_v4_pkt[IP].dst
            send_packet(self, self.devports[0], vrf1_v4_pkt)
            verify_no_other_packets(self, timeout=2)

            print("VRF2(overlapping v4 subnet[172.16.2.0/24]): Sending packet (DST IP:172.16.2.129) from port7 to port13")
            vrf2_v4_pkt[IP].dst = '172.16.2.129'
            vrf2_v4_exp_pkt[IP].dst = vrf2_v4_pkt[IP].dst
            send_packet(self, self.devports[7], vrf2_v4_pkt)
            verify_packet(self, vrf2_v4_exp_pkt, self.devports[13])

            print("VRF1(overlapping v6 host route): Sending packet (DST IP: fd00::1000:1) from port0 to port6")
            send_packet(self, self.devports[0], vrf1_v6_pkt)
            verify_packet(self, vrf1_v6_exp_pkt, self.devports[6])

            print("VRF2(overlapping v6 host route): Sending packet (DST IP: fd00::1000:1) from port7 to port13")
            send_packet(self, self.devports[7], vrf2_v6_pkt)
            verify_packet(self, vrf2_v6_exp_pkt, self.devports[13])

            print("VRF1(overlapping v6 subnet[fd00::0010:0/112]): Sending packet (DST IP: fd00::0010:1) from port0 to port6")
            vrf1_v6_pkt[IPv6].dst = 'fd00::0010:1'
            vrf1_v6_exp_pkt[IPv6].dst = vrf1_v6_pkt[IPv6].dst
            send_packet(self, self.devports[0], vrf1_v6_pkt)
            verify_packet(self, vrf1_v6_exp_pkt, self.devports[6])

            print("VRF2(overlapping v6 subnet[fd00::0010:0/108]): Sending packet (DST IP: fd00::0010:1) from port7 to port13")
            vrf2_v6_pkt[IPv6].dst = 'fd00::0010:1'
            vrf2_v6_exp_pkt[IPv6].dst = vrf2_v6_pkt[IPv6].dst
            send_packet(self, self.devports[7], vrf2_v6_pkt)
            verify_packet(self, vrf2_v6_exp_pkt, self.devports[13])

            print("VRF1(overlapping v6 subnet[fd00::0010:0/112]): Sending packet (DST IP: fd00::0011:1) from port0")
            vrf1_v6_pkt[IPv6].dst = 'fd00::0011:1'
            vrf1_v6_exp_pkt[IPv6].dst = vrf1_v6_pkt[IPv6].dst
            send_packet(self, self.devports[0], vrf1_v6_pkt)
            verify_no_other_packets(self, timeout=2)

            print("VRF2(overlapping v6 subnet[fd00::0010:0/108]): Sending packet (DST IP: fd00::0011:1) from port7 to port13")
            vrf2_v6_pkt[IPv6].dst = 'fd00::0011:1'
            vrf2_v6_exp_pkt[IPv6].dst = vrf2_v6_pkt[IPv6].dst
            send_packet(self, self.devports[7], vrf2_v6_pkt)
            verify_packet(self, vrf2_v6_exp_pkt, self.devports[13])
        finally:
            pass

    def L3InterVrfAclRedirectTest(self):
        if (self.client.is_feature_enable(SWITCH_FEATURE_ACL_REDIRECT_NEXTHOP) == 0):
            print("ACL redirect nexthop feature not enabled, skipping")
            return
        print("L3InterVrfAclRedirectTest()")
        vrf1_pkt = simple_tcp_packet(
            eth_dst=self.rmac1,
            eth_src='00:11:11:11:11:11',
            ip_src='1.1.1.1',
            ip_dst=self.get_nexthop_ip(1, self.devports[0]),
            ip_id=105,
            ip_ttl=64)
        vrf1_exp_pkt = simple_tcp_packet(
            eth_dst=self.get_neighbor_mac(self.devports[0]),
            eth_src=self.rmac1,
            ip_src='1.1.1.1',
            ip_dst=self.get_nexthop_ip(1, self.devports[0]),
            ip_id=105,
            ip_ttl=63)
        vrf2_exp_pkt = simple_tcp_packet(
            eth_dst=self.get_neighbor_mac(self.devports[7]),
            eth_src=self.rmac2,
            ip_src='1.1.1.1',
            ip_dst=self.get_nexthop_ip(1, self.devports[0]),
            ip_id=105,
            ip_ttl=63)
        try:
            print("Sending Packet DST IP:{} from port6 to port1 on VRF1".format(vrf1_pkt[IP].dst))
            send_packet(self, self.devports[6], vrf1_pkt)
            verify_packet(self, vrf1_exp_pkt, self.devports[0])
            if self.client.is_feature_enable(SWITCH_FEATURE_SHARED_INGRESS_IP_ACL):
                    table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IP
            else:
                    table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IPV4

            acl_table =  self.add_acl_table(self.device, type=table_type,
                    bind_point_type=[acl_table_bp_port],
                    direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
            acl_entry=self.add_acl_entry(self.device, dst_ip=vrf1_pkt[IP].dst, dst_ip_mask='255.255.255.255',
                    redirect=self.nhop20,
                    table_handle=acl_table)
            self.attribute_set(self.port6, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, acl_table)
            try:
                print("Sending Packet DST IP:{} from RIF(port6) on VRF1 -> ACL REDRIECT -> RIF(port7) on"
                " VRF2".format(vrf1_pkt[IP].dst))
                send_packet(self, self.devports[6], vrf1_pkt)
                verify_packet(self, vrf2_exp_pkt, self.devports[7])
            finally:
                self.attribute_set(self.port6, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
                self.cleanlast()
                self.cleanlast()
        finally:
            pass
    def L3VrfScaleTest(self):
        max_vrf = self.attribute_get(self.device, SWITCH_DEVICE_ATTR_MAX_VRF)
        #Exclude 5 vrfs that have already been added
        total_vrfs = max_vrf - 5
        print("L3VrfScaleTest() - Adding {} Vrfs".format(total_vrfs))
        try:
            vrfs = []
            for id in range(total_vrfs):
                vrf = self.add_vrf(self.device, id=1000+id,src_mac=self.rmac)
                self.assertTrue((self.status()==0) and vrf != 0, "Failed to add VRF# {} with id:{}".format(id, 1000+id))
                vrfs.append(vrf)
        finally:
            if vrfs:
                self.clean_to_object(vrfs[0])
            pass

    def L3VrfIPv4UnicastStatusTest(self, status = True):
        print("L3VrfIPv4UnicastStatusTest(status = {})".format(status))
        vrf_handle = self.vrf1
        vrf=1
        rmac = self.rmac1
        vports = self.devports[:7]
        vlan_id=2
        subport_vlan_id=20
        def mk_pkt():
            return simple_tcp_packet(
                eth_dst=rmac,
                eth_src='00:11:11:11:11:11',
                ip_src='1.1.1.1',
                ip_dst=self.get_nexthop_ip(1, self.devports[0]),
                ip_id=105,
                ip_ttl=64)
        def mk_exp_pkt():
            return simple_tcp_packet(
                eth_dst='00:11:11:11:11:01',
                eth_src=rmac,
                ip_src='1.1.1.1',
                ip_dst=self.get_nexthop_ip(1, self.devports[0]),
                ip_id=105,
                ip_ttl=63)
        def mk_exp_vlan_pkt():
            return simple_tcp_packet(
                eth_dst='00:11:11:11:11:01',
                eth_src=rmac,
                ip_src='1.1.1.1',
                ip_dst=self.get_nexthop_ip(1, self.devports[0]),
                dl_vlan_enable=True,
                vlan_vid=vlan_id,
                ip_id=105,
                ip_ttl=63,
                pktlen=104)
        orig_status = self.attribute_get(vrf_handle, SWITCH_VRF_ATTR_IPV4_UNICAST)
        print("Changing VRF IPv4 unicast status from {} to {}".format(orig_status, status))
        self.attribute_set(vrf_handle, SWITCH_VRF_ATTR_IPV4_UNICAST, status)
        assert(status == self.attribute_get(vrf_handle, SWITCH_VRF_ATTR_IPV4_UNICAST))
        try:
            vrf_pkt, vrf_exp_pkt = mk_pkt(), mk_exp_pkt()
            vrf_pkt[IP].dst=self.get_nexthop_ip(vrf, vports[0])
            vrf_exp_pkt[IP].dst=vrf_pkt[IP].dst
            vrf_exp_pkt[Ether].dst=self.get_neighbor_mac(vports[0])
            print("VRF{} Sending packet DST IP:{} from L3 RIF port{} to L3 RIF port{}".format(vrf, vrf_pkt[IP].dst, vports[6], vports[0]))
            send_packet(self, vports[6], vrf_pkt)
            if status:
                verify_packet(self, vrf_exp_pkt, vports[0])
            else:
                verify_no_packet(self, vrf_exp_pkt, vports[0])

            vrf_pkt, vrf_exp_pkt = mk_pkt(), mk_exp_pkt()
            vrf_pkt[IP].dst=self.get_nexthop_ip(vrf, vports[1])
            vrf_exp_pkt[IP].dst=vrf_pkt[IP].dst
            vrf_exp_pkt[Ether].dst=self.get_neighbor_mac(vports[1])
            print("VRF{}Sending packet DST IP:{} from L3 RIF port{} to L3 SVI RIF port{}(Untagged)".format(vrf, vrf_pkt[IP].dst, vports[6], vports[1]))
            send_packet(self, vports[6], vrf_pkt)
            if status:
                verify_packet(self, vrf_exp_pkt, vports[1])
            else:
                verify_no_packet(self, vrf_exp_pkt, vports[1])

            vrf_pkt, vrf_exp_vlan_pkt = mk_pkt(), mk_exp_vlan_pkt()
            vrf_pkt[IP].dst=self.get_nexthop_ip(vrf, vports[2])
            vrf_exp_vlan_pkt[IP].dst=vrf_pkt[IP].dst
            vrf_exp_vlan_pkt[Ether].dst = self.get_neighbor_mac(vports[2])
            print("VRF{} Sending packet DST IP:{} from L3 RIF port{} to L3 SVI RIF port{}(Tagged)".format(vrf, vrf_pkt[IP].dst, vports[6], vports[2]))
            send_packet(self, vports[6], vrf_pkt)
            if status:
                verify_packet(self, vrf_exp_vlan_pkt, vports[2])
            else:
                verify_no_packet(self, vrf_exp_vlan_pkt, vports[2])

            vrf_pkt, vrf_exp_pkt = mk_pkt(), mk_exp_pkt()
            vrf_pkt[IP].dst=self.get_nexthop_ip(vrf, self.devports[3])
            vrf_exp_pkt[IP].dst=vrf_pkt[IP].dst
            vrf_exp_pkt[Ether].dst=self.get_neighbor_mac(vports[3])
            print("VRF{} Sending packet DST IP:{} from L3 RIF port{} to L3 LAG RIF ports[{},{}]".format(vrf, vrf_pkt[IP].dst, vports[6], vports[3], vports[4]))
            send_packet(self, vports[6], vrf_pkt)
            if status:
                verify_packets_any(self, vrf_exp_pkt, [vports[3], vports[4]])
            else:
                verify_no_packet_any(self, vrf_exp_pkt, [vports[3], vports[4]])

            vrf_pkt, vrf_exp_vlan_pkt = mk_pkt(), mk_exp_vlan_pkt()
            vrf_pkt[IP].dst=self.get_nexthop_ip(vrf, self.devports[5])
            vrf_exp_vlan_pkt[IP].dst=vrf_pkt[IP].dst
            vrf_exp_vlan_pkt[Ether].dst = self.get_neighbor_mac(vports[5])
            vrf_exp_vlan_pkt[Ether][Dot1Q].vlan = subport_vlan_id
            print("VRF{} ending packet DST IP:{} from L3 RIF port{} to L3 SubportRIF port{}".format(vrf, vrf_pkt[IP].dst, vports[6], vports[5]))
            send_packet(self, vports[6], vrf_pkt)
            if status:
                verify_packet(self, vrf_exp_vlan_pkt, vports[5])
            else:
                verify_no_packet(self, vrf_exp_vlan_pkt, vports[5])
        finally:
            print("Clean up: changing VRF IPv4 unicast status from {} to {}".format(status, orig_status))
            self.attribute_set(vrf_handle, SWITCH_VRF_ATTR_IPV4_UNICAST, orig_status)

    def tearDown(self):
        self.attribute_set(self.port8, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.cleanup()

###############################################################################

@group('port_isolation')
class PortIsolationTest(ApiHelper):
    def setUp(self):
        print()

        self.configure()

        # ingress ports
        rif0 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port0, vrf_handle=self.vrf10, src_mac=self.rmac)
        rif1 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port1, vrf_handle=self.vrf10, src_mac=self.rmac)

        # nhop1
        rif2 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port2, vrf_handle=self.vrf10, src_mac=self.rmac)
        nhop1 = self.add_nexthop(self.device, handle=rif2, dest_ip='10.10.0.2')
        neighbor1 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=rif2, dest_ip='10.10.0.2')

        # nhop2
        rif3 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port3, vrf_handle=self.vrf10, src_mac=self.rmac)
        nhop2 = self.add_nexthop(self.device, handle=rif3, dest_ip='20.20.0.2')
        neighbor2 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:56', handle=rif3, dest_ip='20.20.0.2')

        # nhop3
        rif4 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port4, vrf_handle=self.vrf10, src_mac=self.rmac)
        nhop3 = self.add_nexthop(self.device, handle=rif4, dest_ip='30.30.0.2')
        neighbor3 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:57', handle=rif4, dest_ip='30.30.0.2')

        self.add_route(self.device, ip_prefix='10.10.10.1', vrf_handle=self.vrf10, nexthop_handle=nhop1)
        self.add_route(self.device, ip_prefix='20.20.20.1', vrf_handle=self.vrf10, nexthop_handle=nhop2)
        self.add_route(self.device, ip_prefix='30.30.30.1', vrf_handle=self.vrf10, nexthop_handle=nhop3)

        # port 3 shared between group 1 and 2
        self.ig1 = self.add_isolation_group(self.device)
        self.ig2 = self.add_isolation_group(self.device)

        self.igm1 = self.add_isolation_group_member(self.device, handle=self.port2, isolation_group_handle=self.ig1)
        self.igm4 = self.add_isolation_group_member(self.device, handle=self.port4, isolation_group_handle=self.ig2)

        self.igm2 = self.add_isolation_group_member(self.device, handle=self.port3, isolation_group_handle=self.ig1)
        self.igm3 = self.add_isolation_group_member(self.device, handle=self.port3, isolation_group_handle=self.ig2)


    def runTest(self):
        if (self.client.is_feature_enable(SWITCH_FEATURE_PORT_ISOLATION) == 0):
            print("Port Isolation feature not enabled, skipping")
            return
        try:
            self.Test1()
        finally:
            pass

    def tearDown(self):
        self.cleanup()

    def Test1(self):
        try:
            pkt1 = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                ip_dst='10.10.10.1',
                ip_ttl=64)
            exp_pkt1 = simple_tcp_packet(
                eth_dst='00:11:22:33:44:55',
                eth_src='00:77:66:55:44:33',
                ip_dst='10.10.10.1',
                ip_ttl=63)
            pkt2 = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                ip_dst='20.20.20.1',
                ip_ttl=64)
            exp_pkt2 = simple_tcp_packet(
                eth_dst='00:11:22:33:44:56',
                eth_src='00:77:66:55:44:33',
                ip_dst='20.20.20.1',
                ip_ttl=63)
            pkt3 = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                ip_dst='30.30.30.1',
                ip_ttl=64)
            exp_pkt3 = simple_tcp_packet(
                eth_dst='00:11:22:33:44:57',
                eth_src='00:77:66:55:44:33',
                ip_dst='30.30.30.1',
                ip_ttl=63)

            print("Sending packet from port %d" % self.devports[0])
            send_packet(self, self.devports[0], pkt1)
            verify_packet(self, exp_pkt1, self.devports[2])
            send_packet(self, self.devports[0], pkt2)
            verify_packet(self, exp_pkt2, self.devports[3])
            send_packet(self, self.devports[0], pkt3)
            verify_packet(self, exp_pkt3, self.devports[4])

            print("Sending packet from port %d" % self.devports[1])
            send_packet(self, self.devports[1], pkt1)
            verify_packet(self, exp_pkt1, self.devports[2])
            send_packet(self, self.devports[1], pkt2)
            verify_packet(self, exp_pkt2, self.devports[3])
            send_packet(self, self.devports[1], pkt3)
            verify_packet(self, exp_pkt3, self.devports[4])

            # attach isolation groups
            print("Attach isolation groups to ingress ports")
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_ISOLATION_GROUP_HANDLE, self.ig1)
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_ISOLATION_GROUP_HANDLE, self.ig2)
            print("Sending packet from port %d" % self.devports[0])
            send_packet(self, self.devports[0], pkt1)
            verify_no_other_packets(self, timeout=1)
            send_packet(self, self.devports[0], pkt2)
            verify_no_other_packets(self, timeout=1)
            send_packet(self, self.devports[0], pkt3)
            verify_packet(self, exp_pkt3, self.devports[4])

            print("Sending packet from port %d" % self.devports[1])
            send_packet(self, self.devports[1], pkt1)
            verify_packet(self, exp_pkt1, self.devports[2])
            send_packet(self, self.devports[1], pkt2)
            verify_no_other_packets(self, timeout=1)
            send_packet(self, self.devports[1], pkt3)
            verify_no_other_packets(self, timeout=1)

            print("Remove port3 from isolation groups")
            self.cleanlast(2)

            print("Sending packet from port %d" % self.devports[0])
            send_packet(self, self.devports[0], pkt1)
            verify_no_other_packets(self, timeout=1)
            send_packet(self, self.devports[0], pkt2)
            verify_packet(self, exp_pkt2, self.devports[3])
            send_packet(self, self.devports[0], pkt3)
            verify_packet(self, exp_pkt3, self.devports[4])

            print("Sending packet from port %d" % self.devports[1])
            send_packet(self, self.devports[1], pkt1)
            verify_packet(self, exp_pkt1, self.devports[2])
            send_packet(self, self.devports[1], pkt2)
            verify_packet(self, exp_pkt2, self.devports[3])
            send_packet(self, self.devports[1], pkt3)
            verify_no_other_packets(self, timeout=1)

            # detach isolation groups
            print("Detach isolation groups from ingress ports")
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_ISOLATION_GROUP_HANDLE, 0)
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_ISOLATION_GROUP_HANDLE, 0)
            print("Sending packet from port %d" % self.devports[0])
            send_packet(self, self.devports[0], pkt1)
            verify_packet(self, exp_pkt1, self.devports[2])
            send_packet(self, self.devports[0], pkt2)
            verify_packet(self, exp_pkt2, self.devports[3])
            send_packet(self, self.devports[0], pkt3)
            verify_packet(self, exp_pkt3, self.devports[4])

            print("Sending packet from port %d" % self.devports[1])
            send_packet(self, self.devports[1], pkt1)
            verify_packet(self, exp_pkt1, self.devports[2])
            send_packet(self, self.devports[1], pkt2)
            verify_packet(self, exp_pkt2, self.devports[3])
            send_packet(self, self.devports[1], pkt3)
            verify_packet(self, exp_pkt3, self.devports[4])
        finally:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_ISOLATION_GROUP_HANDLE, 0)
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_ISOLATION_GROUP_HANDLE, 0)
            pass

@group('port_isolation')
class CombinedIsolationTest(ApiHelper):
    def setUp(self):
        print()

        self.configure()

        # self.vlan10 = self.add_vlan(self.device, vlan_id=10)
        # self.rmac = '00:77:66:55:44:33'
        # self.vrf10 = self.add_vrf(self.device, id=10, src_mac=self.rmac)

        self.br = self.add_bridge(self.device, type=SWITCH_BRIDGE_ATTR_TYPE_DOT1Q)
        self.port0_bp = self.add_bridge_port(self.device,
                                        port_lag_handle=self.port0,
                                        bridge_handle=self.br,
                                        admin_state=1)
        # self.port1_bp = add_bridge_port(self.device,
        #                                 port_lag_handle=port1,
        #                                 bridge_handle=br,
        #                                 admin_state=1)
        self.port2_bp = self.add_bridge_port(self.device,
                                        port_lag_handle=self.port2,
                                        bridge_handle=self.br,
                                        admin_state=1)
        # self.port3_bp = add_bridge_port(self.device,
        #                                 port_lag_handle=port3,
        #                                 bridge_handle=br,
        #                                 admin_state=1)

        self.vlan10_mem0 = self.add_vlan_member(self.device,
                                           member_handle=self.port0,
                                           vlan_handle=self.vlan10)
        self.vlan10_mem1 = self.add_vlan_member(self.device,
                                           member_handle=self.port1,
                                           vlan_handle=self.vlan10)
        self.vlan10_mem2 = self.add_vlan_member(self.device,
                                           member_handle=self.port2,
                                           vlan_handle=self.vlan10)
        self.vlan10_mem3 = self.add_vlan_member(self.device,
                                           member_handle=self.port3,
                                           vlan_handle=self.vlan10)

        self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.attribute_set(self.port2, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.attribute_set(self.port3, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)

        self.rif0 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_VLAN, vlan_handle=self.vlan10, vrf_handle=self.vrf10, src_mac=self.rmac)

        self.port0_mac = '00:11:11:11:11:11'
        self.port1_mac = '00:22:22:22:22:22'
        self.port2_mac = '00:33:33:33:33:33'
        self.port3_mac = '00:44:44:44:44:44'

        self.rif_ip = '1.1.1.100'
        self.port0_ip = '1.1.1.10'
        self.port1_ip = '1.1.1.20'
        self.port2_ip = '1.1.1.30'
        self.port3_ip = '1.1.1.40'

        nhop0 = self.add_nexthop(self.device,
                                 handle=self.rif0,
                                 dest_ip=self.rif_ip)
        nhop1 = self.add_nexthop(self.device,
                                 handle=self.rif0,
                                 dest_ip=self.port0_ip)
        nhop2 = self.add_nexthop(self.device,
                                 handle=self.rif0,
                                 dest_ip=self.port1_ip)
        nhop3 = self.add_nexthop(self.device,
                                 handle=self.rif0,
                                 dest_ip=self.port2_ip)
        nhop4 = self.add_nexthop(self.device,
                                 handle=self.rif0,
                                 dest_ip=self.port3_ip)

        neighbor100 = self.add_neighbor(self.device,
                                        mac_address=self.rmac,
                                        handle=self.rif0,
                                        dest_ip=self.rif_ip,
                                        nexthop_handle=nhop0)
        neighbor0 = self.add_neighbor(self.device,
                                      mac_address=self.port0_mac,
                                      handle=self.rif0,
                                      dest_ip=self.port0_ip,
                                      nexthop_handle=nhop1)
        neighbor1 = self.add_neighbor(self.device,
                                      mac_address=self.port1_mac,
                                      handle=self.rif0,
                                      dest_ip=self.port1_ip,
                                      nexthop_handle=nhop2)
        neighbor2 = self.add_neighbor(self.device,
                                      mac_address=self.port2_mac,
                                      handle=self.rif0,
                                      dest_ip=self.port2_ip,
                                      nexthop_handle=nhop3)
        neighbor3 = self.add_neighbor(self.device,
                                      mac_address=self.port3_mac,
                                      handle=self.rif0,
                                      dest_ip=self.port3_ip,
                                      nexthop_handle=nhop4)

        route1 = self.add_route(self.device,
                                ip_prefix='1.1.1.0/24',
                                vrf_handle=self.vrf10,
                                nexthop_handle=nhop0)
        route1 = self.add_route(self.device,
                                ip_prefix='1.1.1.100/32',
                                vrf_handle=self.vrf10,
                                nexthop_handle=nhop0)
        route1 = self.add_route(self.device,
                                ip_prefix='1.1.1.10/32',
                                vrf_handle=self.vrf10,
                                nexthop_handle=nhop1)
        route1 = self.add_route(self.device,
                                ip_prefix='1.1.1.20/32',
                                vrf_handle=self.vrf10,
                                nexthop_handle=nhop2)
        route1 = self.add_route(self.device,
                                ip_prefix='1.1.1.30/32',
                                vrf_handle=self.vrf10,
                                nexthop_handle=nhop3)
        route1 = self.add_route(self.device,
                                ip_prefix='1.1.1.40/32',
                                vrf_handle=self.vrf10,
                                nexthop_handle=nhop4)

        self.add_mac_entry(self.device,
                      vlan_handle=self.vlan10,
                      mac_address=self.port0_mac,
                      destination_handle=self.port0)
        self.add_mac_entry(self.device,
                      vlan_handle=self.vlan10,
                      mac_address=self.port1_mac,
                      destination_handle=self.port1)
        self.add_mac_entry(self.device,
                      vlan_handle=self.vlan10,
                      mac_address=self.port2_mac,
                      destination_handle=self.port2)
        self.add_mac_entry(self.device,
                      vlan_handle=self.vlan10,
                      mac_address=self.port3_mac,
                      destination_handle=self.port3)

        self.ig1 = self.add_isolation_group(
            self.device,
            type=SWITCH_ISOLATION_GROUP_ATTR_TYPE_BRIDGE_PORT)
        self.ig2 = self.add_isolation_group(self.device)

        self.igm1 = self.add_isolation_group_member(
            self.device,
            handle=self.port2_bp,
            isolation_group_handle=self.ig1)
        self.igm2 = self.add_isolation_group_member(
            self.device,
            handle=self.port1,
            isolation_group_handle=self.ig2)

    def runTest(self):
        if (self.client.is_feature_enable(SWITCH_FEATURE_PORT_ISOLATION) == 0):
            print("Port Isolation feature not enabled, skipping")
            return
        try:
            self.Test1()
        finally:
            pass

    def tearDown(self):
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.port2, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.port3, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.cleanup()

    def Test1(self):
        try:
            pkt_brcast = simple_udp_packet(eth_dst="ff:ff:ff:ff:ff:ff",
                                           eth_src=self.port0_mac,
                                           pktlen=100)

            pkt1_b = simple_udp_packet(eth_dst=self.port1_mac,
                                       eth_src=self.port0_mac,
                                       pktlen=100)
            pkt2_b = simple_udp_packet(eth_dst=self.port2_mac,
                                       eth_src=self.port0_mac,
                                       pktlen=100)
            pkt3_b = simple_udp_packet(eth_dst=self.port3_mac,
                                       eth_src=self.port0_mac,
                                       pktlen=100)

            pkt1_r = simple_udp_packet(eth_dst=self.rmac,
                                       eth_src=self.port0_mac,
                                       ip_dst=self.port1_ip,
                                       ip_src=self.port0_ip,
                                       ip_ttl=63)
            pkt1_r_exp = simple_udp_packet(eth_src=self.rmac,
                                           eth_dst=self.port1_mac,
                                           ip_dst=self.port1_ip,
                                           ip_src=self.port0_ip,
                                           ip_ttl=62)

            pkt2_r = simple_udp_packet(eth_dst=self.rmac,
                                       eth_src=self.port0_mac,
                                       ip_dst=self.port2_ip,
                                       ip_src=self.port0_ip,
                                       ip_ttl=63)
            pkt2_r_exp = simple_udp_packet(eth_src=self.rmac,
                                           eth_dst=self.port2_mac,
                                           ip_dst=self.port2_ip,
                                           ip_src=self.port0_ip,
                                           ip_ttl=62)

            pkt3_r = simple_udp_packet(eth_dst=self.rmac,
                                       eth_src=self.port0_mac,
                                       ip_dst=self.port3_ip,
                                       ip_src=self.port0_ip,
                                       ip_ttl=63)
            pkt3_r_exp = simple_udp_packet(eth_src=self.rmac,
                                           eth_dst=self.port3_mac,
                                           ip_dst=self.port3_ip,
                                           ip_src=self.port0_ip,
                                           ip_ttl=62)

            pkt4_r = simple_udp_packet(eth_dst=self.rmac,
                                       eth_src=self.port1_mac,
                                       ip_dst=self.port0_ip,
                                       ip_src=self.port1_ip,
                                       ip_ttl=63)
            pkt4_r_exp = simple_udp_packet(eth_src=self.rmac,
                                           eth_dst=self.port0_mac,
                                           ip_dst=self.port0_ip,
                                           ip_src=self.port1_ip,
                                           ip_ttl=62)

            def get_drop_count(port):
                ctrs = self.client.object_counters_get(port)
                return ctrs[SWITCH_PORT_COUNTER_ID_IF_OUT_DISCARDS].count

            print("Sending bridged packet (brcast) from port %d" %
                  self.devports[0])
            send_packet(self, self.devports[0], pkt_brcast)
            verify_packets(self,
                           pkt_brcast,
                           [self.devports[1],
                            self.devports[2],
                            self.devports[3]])

            print("Sending bridged packet from port %d -> to port %d" %
                  (self.devports[0], self.devports[1]))
            send_packet(self, self.devports[0], pkt1_b)
            verify_packets(self, pkt1_b, [self.devports[1]])

            print("Sending bridged packet from port %d -> to port %d" %
                  (self.devports[0], self.devports[2]))
            send_packet(self, self.devports[0], pkt2_b)
            verify_packets(self, pkt2_b, [self.devports[2]])

            print("Sending bridged packet from port %d -> to port %d" %
                  (self.devports[0], self.devports[3]))
            send_packet(self, self.devports[0], pkt3_b)
            verify_packets(self, pkt3_b, [self.devports[3]])

            print("Sending routed packet from port %d -> to port %d" %
                  (self.devports[0], self.devports[1]))
            send_packet(self, self.devports[0], pkt1_r)
            verify_packets(self, pkt1_r_exp, [self.devports[1]])

            print("Sending routed packet from port %d -> to port %d" %
                  (self.devports[0], self.devports[2]))
            send_packet(self, self.devports[0], pkt2_r)
            verify_packets(self, pkt2_r_exp, [self.devports[2]])

            print("Sending routed packet from port %d -> to port %d" %
                  (self.devports[0], self.devports[3]))
            send_packet(self, self.devports[0], pkt3_r)
            verify_packets(self, pkt3_r_exp, [self.devports[3]])

            ###########################################
            print("Attach bridge port isolation group "
                  "with member port%d to port%d" %
                  (self.devports[2], self.devports[0]))
            self.attribute_set(
                self.port0_bp,
                SWITCH_BRIDGE_PORT_ATTR_ISOLATION_GROUP_HANDLE,
                self.ig1)

            print("Sending bridged packet (brcast) from port %d" %
                  (self.devports[0]))
            send_packet(self, self.devports[0], pkt_brcast)
            verify_packets(self,
                           pkt_brcast,
                           [self.devports[1],
                            self.devports[3]])

            print("Sending bridged packet (brcast) from port %d" %
                  (self.devports[2]))
            send_packet(self, self.devports[2], pkt_brcast)
            verify_packets(self,
                           pkt_brcast,
                           [self.devports[0],
                            self.devports[1],
                            self.devports[3]])

            print("Sending bridged packet from port %d -> to port %d" %
                  (self.devports[0], self.devports[1]))
            send_packet(self, self.devports[0], pkt1_b)
            verify_packets(self, pkt1_b, [self.devports[1]])

            print("Sending bridged packet from port %d -> to port %d, "
                  "Should be dropped" %
                  (self.devports[0], self.devports[2]))
            drop_count = get_drop_count(self.port2)
            send_packet(self, self.devports[0], pkt2_b)
            verify_no_other_packets(self)
            assert get_drop_count(self.port2) - drop_count == 1

            print("Sending bridged packet from port %d -> to port %d" %
                  (self.devports[0], self.devports[3]))
            send_packet(self, self.devports[0], pkt3_b)
            verify_packets(self, pkt3_b, [self.devports[3]])

            print("Sending routed packet from port %d -> to port %d" %
                  (self.devports[0], self.devports[1]))
            send_packet(self, self.devports[0], pkt1_r)
            verify_packets(self, pkt1_r_exp, [self.devports[1]])

            print("Sending routed packet from port %d -> to port %d" %
                  (self.devports[0], self.devports[2]))
            send_packet(self, self.devports[0], pkt2_r)
            verify_packets(self, pkt2_r_exp, [self.devports[2]])

            print("Sending routed packet from port %d -> to port %d" %
                  (self.devports[0], self.devports[3]))
            send_packet(self, self.devports[0], pkt3_r)
            verify_packets(self, pkt3_r_exp, [self.devports[3]])

            ###########################################
            print("Attach  port isolation group "
                  "with member port%d to port%d" %
                  (self.devports[1], self.devports[0]))
            self.attribute_set(
                self.port0,
                SWITCH_PORT_ATTR_ISOLATION_GROUP_HANDLE,
                self.ig2)

            print("Sending bridged packet (brcast) from port %d" %
                  (self.devports[0]))
            send_packet(self, self.devports[0], pkt_brcast)
            verify_packets(self, pkt_brcast, [self.devports[3]])

            print("Sending bridged packet (brcast) from port %d" %
                  (self.devports[1]))
            send_packet(self, self.devports[1], pkt_brcast)
            verify_packets(self, pkt_brcast, [self.devports[0],
                                              self.devports[2],
                                              self.devports[3]])

            print("Sending bridged packet from port %d -> to port %d, "
                  "Should be dropped" %
                  (self.devports[0], self.devports[1]))
            drop_count = get_drop_count(self.port1)
            send_packet(self, self.devports[0], pkt1_b)
            verify_no_other_packets(self)
            assert get_drop_count(self.port1) - drop_count == 1

            print("Sending bridged packet from port %d -> to port %d, "
                  "Should be dropped" %
                  (self.devports[0], self.devports[2]))
            drop_count = get_drop_count(self.port2)
            send_packet(self, self.devports[0], pkt2_b)
            verify_no_other_packets(self)
            assert get_drop_count(self.port2) - drop_count == 1

            print("Sending bridged packet from port %d -> to port %d" %
                  (self.devports[0], self.devports[3]))
            send_packet(self, self.devports[0], pkt3_b)
            verify_packets(self, pkt3_b, [self.devports[3]])

            print("Sending routed packet from port %d -> to port %d, "
                  "Should be dropped" %
                  (self.devports[0], self.devports[1]))
            drop_count = get_drop_count(self.port1)
            send_packet(self, self.devports[0], pkt1_r)
            verify_no_other_packets(self)
            assert get_drop_count(self.port1) - drop_count == 1

            print("Sending routed packet from port %d -> to port %d" %
                  (self.devports[0], self.devports[2]))
            send_packet(self, self.devports[0], pkt2_r)
            verify_packets(self, pkt2_r_exp, [self.devports[2]])

            print("Sending routed packet from port %d -> to port %d" %
                  (self.devports[0], self.devports[3]))
            send_packet(self, self.devports[0], pkt3_r)
            verify_packets(self, pkt3_r_exp, [self.devports[3]])

            ###########################################
            print("Detach bridge_port isolation group "
                  "with member port%d to port%d" %
                  (self.devports[1], self.devports[0]))
            self.attribute_set(
                self.port0_bp,
                SWITCH_BRIDGE_PORT_ATTR_ISOLATION_GROUP_HANDLE,
                0)

            print("Sending bridged packet (brcast) from port %d" %
                  (self.devports[0]))
            send_packet(self, self.devports[0], pkt_brcast)
            verify_packets(self, pkt_brcast, [self.devports[2],
                                              self.devports[3]])

            print("Sending bridged packet from port %d -> to port %d, "
                  "Should be dropped" %
                  (self.devports[0], self.devports[1]))
            drop_count = get_drop_count(self.port1)
            send_packet(self, self.devports[0], pkt1_b)
            verify_no_other_packets(self)
            assert get_drop_count(self.port1) - drop_count == 1

            print("Sending bridged packet from port %d -> to port %d" %
                  (self.devports[0], self.devports[2]))
            send_packet(self, self.devports[0], pkt2_b)
            verify_packets(self, pkt2_b, [self.devports[2]])

            print("Sending bridged packet from port %d -> to port %d" %
                  (self.devports[0], self.devports[3]))
            send_packet(self, self.devports[0], pkt3_b)
            verify_packets(self, pkt3_b, [self.devports[3]])

            print("Sending routed packet from port %d -> to port %d, "
                  "Should be dropped" %
                  (self.devports[0], self.devports[1]))
            drop_count = get_drop_count(self.port1)
            send_packet(self, self.devports[0], pkt1_r)
            verify_no_other_packets(self)
            assert get_drop_count(self.port1) - drop_count == 1

            print("Sending routed packet from port %d -> to port %d" %
                  (self.devports[0], self.devports[2]))
            send_packet(self, self.devports[0], pkt2_r)
            verify_packets(self, pkt2_r_exp, [self.devports[2]])

            print("Sending routed packet from port %d -> to port %d" %
                  (self.devports[0], self.devports[3]))
            send_packet(self, self.devports[0], pkt3_r)
            verify_packets(self, pkt3_r_exp, [self.devports[3]])

            print("Sending routed packet from port %d -> to port %d" %
                  (self.devports[1], self.devports[0]))
            send_packet(self, self.devports[1], pkt4_r)
            verify_packets(self, pkt4_r_exp, [self.devports[0]])

            ###########################################
            print("Detach bridge_port isolation group "
                  "with member port%d to port%d" %
                  (self.devports[1], self.devports[0]))
            self.attribute_set(
                self.port0,
                SWITCH_PORT_ATTR_ISOLATION_GROUP_HANDLE,
                0)

            print("Sending bridged packet (brcast) from port %d" %
                  (self.devports[0]))
            send_packet(self, self.devports[0], pkt_brcast)
            verify_packets(self,
                           pkt_brcast,
                           [self.devports[1],
                            self.devports[2],
                            self.devports[3]])

            print("Sending bridged packet from port %d -> to port %d" %
                  (self.devports[0], self.devports[1]))
            send_packet(self, self.devports[0], pkt1_b)
            verify_packets(self, pkt1_b, [self.devports[1]])

            print("Sending bridged packet from port %d -> to port %d" %
                  (self.devports[0], self.devports[2]))
            send_packet(self, self.devports[0], pkt2_b)
            verify_packets(self, pkt2_b, [self.devports[2]])

            print("Sending bridged packet from port %d -> to port %d" %
                  (self.devports[0], self.devports[3]))
            send_packet(self, self.devports[0], pkt3_b)
            verify_packets(self, pkt3_b, [self.devports[3]])

            print("Sending routed packet from port %d -> to port %d" %
                  (self.devports[0], self.devports[1]))
            send_packet(self, self.devports[0], pkt1_r)
            verify_packets(self, pkt1_r_exp, [self.devports[1]])

            print("Sending routed packet from port %d -> to port %d" %
                  (self.devports[0], self.devports[2]))
            send_packet(self, self.devports[0], pkt2_r)
            verify_packets(self, pkt2_r_exp, [self.devports[2]])

            print("Sending routed packet from port %d -> to port %d" %
                  (self.devports[0], self.devports[3]))
            send_packet(self, self.devports[0], pkt3_r)
            verify_packets(self, pkt3_r_exp, [self.devports[3]])

            print("Verification complete")
        finally:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_ISOLATION_GROUP_HANDLE, 0)
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_ISOLATION_GROUP_HANDLE, 0)
            pass

###############################################################################

def GenerateNRandomWeights(n, total_weight):
  weights = [0] * n

  for i in range(0, total_weight):
    x = random.randint(0, n - 1)
    weights[x] += 1

  return weights;

@group('l3')
@group('ecmp')
class L3WCMPTest(ApiHelper):
    N = 3
    TOTAL_WEIGHT = 30
    SCALING = 0.8
    max_itrs = 240

    def setUp(self):
        print()
        print("WCMP load balancing test")

        self.configure()

        rif0 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port0, vrf_handle=self.vrf10, src_mac=self.rmac)

        # L3 RIF on access port
        rif1 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port1, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.nhop1 = self.add_nexthop(self.device, handle=rif1, dest_ip='10.10.0.1')
        neighbor1 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=rif1, dest_ip='10.10.0.1')

        rif2 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port2, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.nhop2 = self.add_nexthop(self.device, handle=rif2, dest_ip='10.10.0.2')
        neighbor2 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:56', handle=rif2, dest_ip='10.10.0.2')

        rif3 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port3, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.nhop3 = self.add_nexthop(self.device, handle=rif3, dest_ip='10.10.0.3')
        neighbor1 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:57', handle=rif3, dest_ip='10.10.0.3')

        self.weights = GenerateNRandomWeights(self.N, self.TOTAL_WEIGHT)
        # too many odd combinations causing random failures in MTS and CI. Fix the weights for now
        self.weights = [7, 10, 13]
        print("Weights: {}".format(self.weights))
        ecmp0 = self.add_ecmp(self.device)
        ecmp_member01 = self.add_ecmp_member(self.device, nexthop_handle=self.nhop1, ecmp_handle=ecmp0, weight=self.weights[0])
        ecmp_member02 = self.add_ecmp_member(self.device, nexthop_handle=self.nhop2, ecmp_handle=ecmp0, weight=self.weights[1])
        ecmp_member03 = self.add_ecmp_member(self.device, nexthop_handle=self.nhop3, ecmp_handle=ecmp0, weight=self.weights[2])

        self.add_route(self.device, ip_prefix='11.11.0.0/16', vrf_handle=self.vrf10, nexthop_handle=ecmp0)
        self.add_route(self.device, ip_prefix='4000:5678:9abc:def0:4422:1133:0000:0000/96', vrf_handle=self.vrf10, nexthop_handle=ecmp0)

    def runTest(self):
        try:
            pass
            self.IPv4Test()
            self.IPv6Test()
            self.AddDeleteTest()
        finally:
            pass
    def tearDown(self):
        self.cleanup()

    def packetTest(self, base_ip='11.11.11.1'):
        count = [0, 0, 0]
        try:
            dst_ip = int(binascii.hexlify(socket.inet_aton(base_ip)), 16)
            src_ip = int(binascii.hexlify(socket.inet_aton('192.168.8.1')), 16)
            for i in range(0, self.max_itrs):
                dst_ip_addr = socket.inet_ntoa(
                    binascii.unhexlify(format(dst_ip, 'x').zfill(8)))
                src_ip_addr = socket.inet_ntoa(
                    binascii.unhexlify(format(src_ip, 'x').zfill(8)))
                pkt = simple_tcp_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src='00:22:22:22:22:22',
                    ip_dst=dst_ip_addr,
                    ip_src=src_ip_addr,
                    ip_ttl=64)
                exp_pkt1 = simple_tcp_packet(
                    eth_dst='00:11:22:33:44:55',
                    eth_src='00:77:66:55:44:33',
                    ip_dst=dst_ip_addr,
                    ip_src=src_ip_addr,
                    ip_ttl=63)
                exp_pkt2 = simple_tcp_packet(
                    eth_dst='00:11:22:33:44:56',
                    eth_src='00:77:66:55:44:33',
                    ip_dst=dst_ip_addr,
                    ip_src=src_ip_addr,
                    ip_ttl=63)
                exp_pkt3 = simple_tcp_packet(
                    eth_dst='00:11:22:33:44:57',
                    eth_src='00:77:66:55:44:33',
                    ip_dst=dst_ip_addr,
                    ip_src=src_ip_addr,
                    ip_ttl=63)

                send_packet(self, self.devports[0], pkt)
                rcv_idx = verify_any_packet_any_port(
                    self, [exp_pkt1, exp_pkt2, exp_pkt3], [
                        self.devports[1], self.devports[2], self.devports[3],
                    ], timeout=2)
                count[rcv_idx] += 1
                dst_ip += 100
                src_ip += 100
        finally:
            pass
        return count

    def AddDeleteTest(self):
        print("AddDeleteTest()")
        try:
            ecmp1 = self.add_ecmp(self.device)
            self.add_route(self.device, ip_prefix='12.12.0.0/16', vrf_handle=self.vrf10, nexthop_handle=ecmp1)
            total = 5 + 10 + 15
            em1 = self.add_ecmp_member(self.device, nexthop_handle=self.nhop1, ecmp_handle=ecmp1, weight=5)
            em2 = self.add_ecmp_member(self.device, nexthop_handle=self.nhop2, ecmp_handle=ecmp1, weight=10)
            em3 = self.add_ecmp_member(self.device, nexthop_handle=self.nhop3, ecmp_handle=ecmp1, weight=15)
            print("WCMP with 3 members")
            count = self.packetTest(base_ip='12.12.12.1')
            ideal = [30, 60, 80]
            print('Ideal distribution :', ideal)
            print('Packet distribution :', count)
            self.assertTrue(count[0] >= ideal[0], "Wcmp paths are not equally balanced for 5")
            self.assertTrue(count[1] >= ideal[1], "Wcmp paths are not equally balanced for 10")
            self.assertTrue(count[2] >= ideal[2], "Wcmp paths are not equally balanced for 15")

            # remove weight=15 member
            self.cleanlast()
            print("WCMP with 2 members")
            count = self.packetTest(base_ip='12.12.12.1')
            ideal = [60, 120, 0]
            print('Ideal distribution :', ideal)
            print('Packet distribution :', count)
            self.assertTrue(count[0] >= ideal[0], "Wcmp paths are not equally balanced for 5")
            self.assertTrue(count[1] >= ideal[1], "Wcmp paths are not equally balanced for 10")
            self.assertTrue(count[2] == 0, "Wcmp paths are not equally balanced for 15")

            # remove weight=10 member
            self.cleanlast()
            print("WCMP with 1 members")
            count = self.packetTest(base_ip='12.12.12.1')
            print('Packet distribution :', count)
            self.assertTrue(count[0] == self.max_itrs, "Wcmp paths are not equally balanced for 5")
            self.assertTrue(count[1] == 0, "Wcmp paths are not equally balanced for 10")
            self.assertTrue(count[2] == 0, "Wcmp paths are not equally balanced for 15")

            # re-add weight=10,15 to original state
            em2 = self.add_ecmp_member(self.device, nexthop_handle=self.nhop2, ecmp_handle=ecmp1, weight=10)
            em3 = self.add_ecmp_member(self.device, nexthop_handle=self.nhop3, ecmp_handle=ecmp1, weight=15)
            print("WCMP with 3 members")
            count = self.packetTest(base_ip='12.12.12.1')
            ideal = [30, 60, 80]
            print('Ideal distribution :', ideal)
            print('Packet distribution :', count)
            self.assertTrue(count[0] >= ideal[0], "Wcmp paths are not equally balanced for 5")
            self.assertTrue(count[1] >= ideal[1], "Wcmp paths are not equally balanced for 10")
            self.assertTrue(count[2] >= ideal[2], "Wcmp paths are not equally balanced for 15")

            self.attribute_set(em1, SWITCH_ECMP_MEMBER_ATTR_ENABLE, False)
            print("WCMP with 2 members, 1st member disabled")
            count = self.packetTest(base_ip='12.12.12.1')
            ideal = [0, 80, 140]
            print('Ideal distribution :', ideal)
            print('Packet distribution :', count)
            self.assertTrue(count[0] == 0, "Wcmp paths are not equally balanced for 5")
            self.assertTrue(count[1] >= ideal[1], "Wcmp paths are not equally balanced for 10")
            self.assertTrue(count[2] >= ideal[2], "Wcmp paths are not equally balanced for 15")

            self.attribute_set(em1, SWITCH_ECMP_MEMBER_ATTR_ENABLE, True)
            self.attribute_set(em3, SWITCH_ECMP_MEMBER_ATTR_ENABLE, False)
            print("WCMP with 2 members, 3rd member disabled")
            count = self.packetTest(base_ip='12.12.12.1')
            ideal = [60, 120, 0]
            print('Ideal distribution :', ideal)
            print('Packet distribution :', count)
            self.assertTrue(count[0] >= ideal[0], "Wcmp paths are not equally balanced for 5")
            self.assertTrue(count[1] >= ideal[1], "Wcmp paths are not equally balanced for 10")
            self.assertTrue(count[2] == 0, "Wcmp paths are not equally balanced for 15")

            self.attribute_set(em3, SWITCH_ECMP_MEMBER_ATTR_ENABLE, True)
            self.attribute_set(em2, SWITCH_ECMP_MEMBER_ATTR_ENABLE, False)
            print("WCMP with 2 members, 2nd member disabled")
            count = self.packetTest(base_ip='12.12.12.1')
            ideal = [40, 0, 160]
            print('Ideal distribution :', ideal)
            print('Packet distribution :', count)
            self.assertTrue(count[0] >= ideal[0], "Wcmp paths are not equally balanced for 5")
            self.assertTrue(count[1] == 0, "Wcmp paths are not equally balanced for 10")
            self.assertTrue(count[2] >= ideal[2], "Wcmp paths are not equally balanced for 15")

            self.attribute_set(em2, SWITCH_ECMP_MEMBER_ATTR_ENABLE, True)
            print("WCMP with 3 members")
            count = self.packetTest(base_ip='12.12.12.1')
            ideal = [30, 60, 80]
            print('Ideal distribution :', ideal)
            print('Packet distribution :', count)
            self.assertTrue(count[0] >= ideal[0], "Wcmp paths are not equally balanced for 5")
            self.assertTrue(count[1] >= ideal[1], "Wcmp paths are not equally balanced for 10")
            self.assertTrue(count[2] >= ideal[2], "Wcmp paths are not equally balanced for 15")
        finally:
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            pass

    def IPv4Test(self):
        print("IPv4Test()")
        try:
            count = self.packetTest()
            ideal = [0] * self.N
            for i in range(0, self.N):
              ideal[i] = int((self.weights[i] / self.TOTAL_WEIGHT) * self.SCALING * self.max_itrs)
            print('Packet distribution :', count)
            print('Ideal distribution  :', ideal)
            for i in range(0, self.N):
              self.assertTrue(count[i] >= ideal[i], "Wcmp paths are not equally balanced")
        finally:
            pass

    def IPv6Test(self):
        print("IPv6Test()")
        try:
            count = [0, 0, 0]
            dst_ip = int(binascii.hexlify(socket.inet_pton(socket.AF_INET6, '4000:5678:9abc:def0:4422:1133:5577:1111')), 16)
            src_ip = int(binascii.hexlify(socket.inet_pton(socket.AF_INET6, '2000:5678:9abc:def0:4422:1133:5577:1111')), 16)
            max_itrs = 600
            for i in range(0, max_itrs):
                dst_ip_addr = socket.inet_ntop(socket.AF_INET6,
                    binascii.unhexlify(format(dst_ip, 'x').zfill(8)))
                src_ip_addr = socket.inet_ntop(socket.AF_INET6,
                    binascii.unhexlify(format(src_ip, 'x').zfill(8)))
                pkt = simple_tcpv6_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src='00:22:22:22:22:22',
                    ipv6_dst=dst_ip_addr,
                    ipv6_src=src_ip_addr,
                    ipv6_hlim=64)
                exp_pkt1 = simple_tcpv6_packet(
                    eth_dst='00:11:22:33:44:55',
                    eth_src='00:77:66:55:44:33',
                    ipv6_dst=dst_ip_addr,
                    ipv6_src=src_ip_addr,
                    ipv6_hlim=63)
                exp_pkt2 = simple_tcpv6_packet(
                    eth_dst='00:11:22:33:44:56',
                    eth_src='00:77:66:55:44:33',
                    ipv6_dst=dst_ip_addr,
                    ipv6_src=src_ip_addr,
                    ipv6_hlim=63)
                exp_pkt3 = simple_tcpv6_packet(
                    eth_dst='00:11:22:33:44:57',
                    eth_src='00:77:66:55:44:33',
                    ipv6_dst=dst_ip_addr,
                    ipv6_src=src_ip_addr,
                    ipv6_hlim=63)

                send_packet(self, self.devports[0], pkt)
                rcv_idx = verify_any_packet_any_port(
                    self, [exp_pkt1, exp_pkt2, exp_pkt3], [
                        self.devports[1], self.devports[2], self.devports[3],
                    ], timeout=2)
                count[rcv_idx] += 1
                dst_ip += 1
                src_ip += 1

            ideal = [0] * self.N
            for i in range(0, self.N):
              ideal[i] = int((self.weights[i] / self.TOTAL_WEIGHT) * self.SCALING * max_itrs)
            print('Packet distribution :', count)
            print('Ideal distribution  :', ideal)
            for i in range(0, self.N):
              self.assertTrue(count[i] >= ideal[i], "Wcmp paths are not equally balanced")
        finally:
            pass

###############################################################################
from threading import Thread
@disabled
class SWI4287Test(ApiHelper):
    def thread_function(self, name):
        for i in range(300):
          nhop = self.add_nexthop(self.device, handle=self.rif1, dest_ip='10.10.0.1')
          if self.status() != SWITCH_STATUS_SUCCESS:
            os._exit(1)
          nhop = self.add_nexthop(self.device, handle=self.rif1, dest_ip='10.10.0.2')
          if self.status() != SWITCH_STATUS_SUCCESS:
            os._exit(1)
          nhop = self.add_nexthop(self.device, handle=self.rif1, dest_ip='10.10.0.3')
          if self.status() != SWITCH_STATUS_SUCCESS:
            os._exit(1)
          time.sleep(0.2)
          self.cleanlast()
          time.sleep(0.1)
          self.cleanlast()
          time.sleep(0.1)
          self.cleanlast()

    def runTest(self):
        print()
        self.configure()
        self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port1)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_LEARNING, True)
        self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_LEARNING, True)

        #Create rif1 with vlan10
        self.rif1 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_VLAN, vlan_handle=self.vlan10, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=self.rif1, dest_ip='10.10.0.1')
        self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=self.rif1, dest_ip='10.10.0.2')
        self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=self.rif1, dest_ip='10.10.0.3')

        pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:11:22:33:44:55',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63,
            pktlen=100)
        try:
            x = Thread(target=self.thread_function, args=(1,))
            x.start()
            for i in range(200):
                send_packet(self, self.devports[1], pkt)
                time.sleep(0.5)
                self.client.object_flush_all(SWITCH_OBJECT_TYPE_MAC_ENTRY)
            x.join()
        finally:
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.client.object_flush_all(SWITCH_OBJECT_TYPE_MAC_ENTRY)
            self.cleanup()

@disabled
class SWI5971Test(ApiHelper):
    def thread_function(self, name):
        for i in range(300):
          nhop = self.add_nexthop(self.device, handle=self.rif1, dest_ip='10.10.0.1')
          if self.status() != SWITCH_STATUS_SUCCESS:
            os._exit(1)

    def runTest(self):
        print()
        self.configure()
        self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port1)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_LEARNING, True)
        self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_LEARNING, True)

        #Create rif1 with vlan10
        self.rif1 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_VLAN, vlan_handle=self.vlan10, vrf_handle=self.vrf10, src_mac=self.rmac)

        arp_pkt = simple_arp_packet(
            eth_dst='FF:FF:FF:FF:FF:FF',
            eth_src='00:11:22:33:44:55',
            arp_op=1, #ARP request
            ip_tgt='10.10.10.1',
            ip_snd='192.168.0.1',
            hw_snd='00:11:22:33:44:55',
            pktlen=100)
        try:
            for i in range(100):
                send_packet(self, self.devports[1], arp_pkt)
                time.sleep(0.51)
                self.add_nexthop(self.device, handle=self.rif1, dest_ip='10.10.0.1')
                self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=self.rif1, dest_ip='10.10.0.1')
                time.sleep(1)
                self.client.object_flush_all(SWITCH_OBJECT_TYPE_MAC_ENTRY)
                self.cleanlast()
                self.cleanlast()
        finally:
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.client.object_flush_all(SWITCH_OBJECT_TYPE_MAC_ENTRY)
            self.cleanup()

class L3PortIPStats(ApiHelper):
    def setUp(self):
        print()
        self.configure()
        #### PORT ####
        # Create rif0 with port0
        self.rif0 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port0, vrf_handle=self.vrf10, src_mac=self.rmac)

        vlan_mbr1 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port1)
        vlan_mbr2 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port2)
        vlan_mbr4 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port4)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.attribute_set(self.port2, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.attribute_set(self.port4, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)

        # Create rif1 with vlan10
        self.rif1 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_VLAN, vlan_handle=self.vlan10, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.rif10_ig_ucast = 0
        self.rif10_eg_ucast = 0
        self.rif10_ig_bcast = 0
        self.rif10_eg_bcast = 0

        # Create nhop1, nhop2 & nhop3 on SVI
        nhop1 = self.add_nexthop(self.device, handle=self.rif1, dest_ip='10.10.0.1')
        self.neighbor1 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=self.rif1, dest_ip='10.10.0.1')
        route1 = self.add_route(self.device, ip_prefix='10.10.10.1', vrf_handle=self.vrf10, nexthop_handle=nhop1)

        nhop2 = self.add_nexthop(self.device, handle=self.rif1, dest_ip='10.10.0.2')
        self.neighbor2 = self.add_neighbor(self.device, mac_address='00:22:22:33:44:55', handle=self.rif1, dest_ip='10.10.0.2')
        route2 = self.add_route(self.device, ip_prefix='10.10.10.2', vrf_handle=self.vrf10, nexthop_handle=nhop2)

        nhop3 = self.add_nexthop(self.device, handle=self.rif1, dest_ip='10.10.0.3')
        self.neighbor3 = self.add_neighbor(self.device, mac_address='00:33:22:33:44:55', handle=self.rif1, dest_ip='10.10.0.3')
        route3 = self.add_route(self.device, ip_prefix='10.10.10.3', vrf_handle=self.vrf10, nexthop_handle=nhop3)

        # Create nhop and route to L3 intf
        nhop4 = self.add_nexthop(self.device, handle=self.rif0, dest_ip='11.11.0.2')
        neighbor4 = self.add_neighbor(self.device, mac_address='00:44:22:33:44:55', handle=self.rif0, dest_ip='11.11.0.2')
        route4 = self.add_route(self.device, ip_prefix='11.11.11.1', vrf_handle=self.vrf10, nexthop_handle=nhop4)
        v6route5 = self.add_route(self.device, ip_prefix='1234:5678:9abc:def0:4422:1133:5577:1111', vrf_handle=self.vrf10, nexthop_handle=nhop4)

        self.initial_stats = self.client.object_counters_get(self.port1)
        self.initial_stats_egress = self.client.object_counters_get(self.port0)

        self.ipv4_in_unicast = 0
        self.ipv6_in_unicast = 0
        self.ipv4_in_packets = 0
        self.drop_in_ipv4_pkt = 0
        self.drop_in_ipv6_pkt = 0
        self.ipv6_in_packets = 0
        self.ipv4_out_unicast = 0
        self.ipv6_out_unicast = 0
        self.drop_out_ipv4_pkt = 0
        self.drop_out_ipv6_pkt = 0


    def runTest(self):
        try:
            if (self.client.is_feature_enable(SWITCH_FEATURE_IP_STATS) == 0):
                print("Ip Stats feature not enabled, skipping")
                return
            self.sendIpv4Unicastpkt()
            self.Ipv4DropPktTest()
            self.sendIpv6Unicastpkt()
            self.egressIpv4DropStats()
            self.egressIPv6Dropstats()
            self.cleanlast()
            print("Sleeping for 5 sec before fetching stats")
            time.sleep(5)
            try:
                final_stats = self.client.object_counters_get(self.port1)
                final_stats_egress = self.client.object_counters_get(self.port0)

                x = SWITCH_PORT_COUNTER_ID_IP_IN_DISCARDS
                ip_in_discards = final_stats[x].count - self.initial_stats[x].count

                x = SWITCH_PORT_COUNTER_ID_IP_IN_RECEIVES
                ip_in_receives = final_stats[x].count - self.initial_stats[x].count

                x = SWITCH_PORT_COUNTER_ID_IP_IN_OCTETS
                ip_in_octets = final_stats[x].count - self.initial_stats[x].count

                x = SWITCH_PORT_COUNTER_ID_IP_IN_NON_UCAST_PKTS
                ip_in_non_ucast_pkts = final_stats[x].count - self.initial_stats[x].count

                x =  SWITCH_PORT_COUNTER_ID_IP_IN_UCAST_PKTS
                ip_in_ucast_pkts = final_stats[x].count - self.initial_stats[x].count

                x = SWITCH_PORT_COUNTER_ID_IPV6_IN_UCAST_PKTS
                ip_in_ucast_v6_pkts = final_stats[x].count - self.initial_stats[x].count

                x = SWITCH_PORT_COUNTER_ID_IPV6_IN_DISCARDS
                ip_in_v6_discards = final_stats[x].count - self.initial_stats[x].count

                x = SWITCH_PORT_COUNTER_ID_IPV6_IN_RECEIVES
                ip_in_v6_receives = final_stats[x].count - self.initial_stats[x].count

                x = SWITCH_PORT_COUNTER_ID_IPV6_IN_OCTETS
                ip_in_v6_octets = final_stats[x].count - self.initial_stats[x].count

                x = SWITCH_PORT_COUNTER_ID_IP_OUT_UCAST_PKTS
                ip_out_ucast_pkts = final_stats_egress[x].count - self.initial_stats_egress[x].count

                x = SWITCH_PORT_COUNTER_ID_IP_OUT_DISCARDS
                ip_out_discard_pkts = final_stats_egress[x].count - self.initial_stats_egress[x].count

                x = SWITCH_PORT_COUNTER_ID_IP_OUT_OCTETS
                ip_out_octets = final_stats_egress[x].count - self.initial_stats_egress[x].count

                x = SWITCH_PORT_COUNTER_ID_IPV6_OUT_OCTETS
                ip_out_v6_octets = final_stats_egress[x].count - self.initial_stats_egress[x].count

                x = SWITCH_PORT_COUNTER_ID_IPV6_OUT_DISCARDS
                ip_out_v6_discards = final_stats_egress[x].count - self.initial_stats_egress[x].count

                x = SWITCH_PORT_COUNTER_ID_IPV6_OUT_UCAST_PKTS
                ip_out_v6_unicast = final_stats_egress[x].count - self.initial_stats_egress[x].count

                print("Expected v4 drop count    : %d" % ip_in_discards)
                print("In v4 packets received    : %d" % ip_in_receives)
                print("In v4 Octets received     : %d" % ip_in_octets)
                print("In v4 non unicast packets : %d" % ip_in_non_ucast_pkts)
                print("In v4 Unicast packets     : %d" % ip_in_ucast_pkts)
                print("In v6 Unicast packets     : %d" % ip_in_ucast_v6_pkts)
                print("In v6 packet drop         : %d" % ip_in_v6_discards)
                print("In v6 packets received    : %d" % ip_in_v6_receives)
                print("In v6 Octets received     : %d" % ip_in_v6_octets)
                print("Out v4 unicast pkts       : %d" % ip_out_ucast_pkts)
                print("Out v4 discard pkts       : %d" % ip_out_discard_pkts)
                print("Out v4 Octets received    : %d" % ip_out_octets)
                print("Out v6 octets             : %d" %ip_out_v6_octets)
                print("Out v6 unicast            : %d" %ip_out_v6_unicast)
                print("Out v6 discards           : %d" %ip_out_v6_discards)

                self.assertEqual(self.drop_in_ipv4_pkt, ip_in_discards)
                self.assertEqual(self.ipv4_in_packets, ip_in_receives)
                self.assertEqual(self.ipv4_in_unicast, ip_in_ucast_pkts)
                self.assertEqual(self.ipv6_in_unicast, ip_in_ucast_v6_pkts)
                self.assertEqual(self.drop_in_ipv6_pkt, ip_in_v6_discards)
                self.assertEqual(self.ipv4_out_unicast, ip_out_ucast_pkts)
                self.assertEqual(self.drop_out_ipv4_pkt, ip_out_discard_pkts)
                self.assertEqual(self.ipv6_out_unicast, ip_out_v6_unicast)
                self.assertEqual(self.drop_out_ipv6_pkt, ip_out_v6_discards)
                self.assertEqual(self.ipv6_in_packets, ip_in_v6_receives)
            finally:
                pass
        finally:
            pass

    def sendIpv4Unicastpkt(self):
        # Testing unicast packets
        print("Send Ipv4 Unicast packet")
        num_drops = 0
        pkt = simple_tcp_packet(
        eth_dst='00:77:66:55:44:33',
        eth_src='00:22:22:22:22:22',
        ip_dst='11.11.11.1',
        ip_ttl=64)

        exp_pkt = simple_tcp_packet(
        eth_dst='00:44:22:33:44:55',
        eth_src='00:77:66:55:44:33',
        ip_dst='11.11.11.1',
        ip_ttl=63)

        print("Sending packet on port %d, forward to port %d" % (self.devports[1], self.devports[0]))
        send_packet(self, self.devports[1], pkt)
        verify_packet(self, exp_pkt, self.devports[0])
        self.ipv4_in_unicast += 1
        self.ipv4_in_packets += 1
        self.ipv4_out_unicast += 1

    def Ipv4DropPktTest(self):
        '''
        This test tries to verify ip acl entries
        We send a packet from port 0 and drop the packet
        The bp_type is PORT. The port_lag_label comes from the table id
        '''
        pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='11.11.11.1',
            ip_ttl=64)

        print("IPV4DropPktTest()")
        if (self.client.is_feature_enable(SWITCH_FEATURE_SHARED_INGRESS_IP_ACL) == 0):
            print("Shared IP ingress ACL feature not enabled, skipping")
            return
        acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IP,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_entry = self.add_acl_entry(self.device,
            dst_ip='11.11.11.1',
            dst_ip_mask='255.255.255.255',
            #tcp_flags=2,
            #tcp_flags_mask=127,
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, acl_table)

        def TestTraffic():
            print(" Testing Traffic")
            pre_counter = self.object_counters_get(acl_entry)
            send_packet(self, self.devports[1], pkt)
            verify_no_other_packets(self, timeout=2)
            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)
            self.ipv4_in_packets += 1
            self.drop_in_ipv4_pkt += 1

        try:
            TestTraffic()
        finally:
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            # remove the acl entry and table
            self.cleanlast()
            self.cleanlast()
    def sendIpv6Unicastpkt(self):
        print()
        print("send Ipv6 Unicast packet()")
        pkt = simple_tcpv6_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:1111',
            ipv6_src='2000::1',
            ipv6_hlim=64,
            pktlen=100)

        exp_pkt = simple_tcpv6_packet(
            eth_dst='00:44:22:33:44:55',
            eth_src='00:77:66:55:44:33',
            ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:1111',
            ipv6_src='2000::1',
            ipv6_hlim=63,
            pktlen=100)

        try:
            print("Sending packet on port %d, Routed" % self.devports[1])
            send_packet(self, self.devports[1], pkt)
            verify_packet(self, exp_pkt, self.devports[0])
            self.ipv6_in_unicast += 1
            self.ipv6_in_packets += 1
            self.ipv6_out_unicast += 1

            print("Disable IPv6 on ingress RIF")
            self.attribute_set(self.rif1, SWITCH_RIF_ATTR_IPV6_UNICAST, False)
            print("Sending packet on port %d, dropped" % self.devports[1])
            send_packet(self, self.devports[1], pkt)
            verify_no_other_packets(self, timeout=1)
            self.drop_in_ipv6_pkt += 1
            self.ipv6_in_packets += 1

        finally:
            self.attribute_set(self.rif1, SWITCH_RIF_ATTR_IPV6_UNICAST, True)
            pass

    def egressIpv4DropStats(self):
        print("Testing Egress Ipv4 Drop Stats")
        if (self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_IP_ACL) == 0):
            print("EgressIPv4Acl feature not enabled, skipping")
            return

        acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IPV4,
            bind_point_type=[acl_table_bp_rif],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_EGRESS)
        acl_entry = self.add_acl_entry(self.device,
            dst_ip='11.11.11.1',
            dst_ip_mask='255.255.255.255',
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            table_handle=acl_table)
        pre_counter = self.client.object_counters_get(acl_entry)
        try:
            pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='11.11.11.1',
            ip_ttl=64)

            print("Allow packet before binding egress ACL to port")
            print("Bind ACL to %d" %self.devports[0])
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, acl_table)
            send_packet(self, self.devports[1], pkt)
            verify_no_other_packets(self, timeout=2)
            self.drop_out_ipv4_pkt += 1
            self.ipv4_in_packets += 1
            self.ipv4_in_unicast += 1

        finally:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, 0)
            pass
    def egressIPv6Dropstats(self):
        print("Egress IPv6 Drop Stats Testing()")
        if (self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_IP_ACL) == 0):
            print("EgressIPv6Acl  feature not enabled, skipping")
            return

        pkt = simple_tcpv6_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:1111',
            ipv6_src='2000::1',
            ipv6_hlim=64,
            pktlen=100)

        acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IPV6,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_EGRESS)
        acl_entry = self.add_acl_entry(self.device,
            dst_ip='1234:5678:9abc:def0:4422:1133:5577:1111',
            dst_ip_mask='FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF',
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, acl_table)

        def TestTraffic():
            print("Testing Traffic")
            send_packet(self, self.devports[1], pkt)
            verify_no_other_packets(self, timeout=2)
        try:
            TestTraffic()
            self.drop_out_ipv6_pkt += 1
            self.ipv6_in_packets += 1
            self.ipv6_in_unicast += 1
        finally:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, 0)

    def tearDown(self):
        self.client.object_flush_all(SWITCH_OBJECT_TYPE_MAC_ENTRY)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.port2, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.port4, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.cleanup()

class L3ECMPHashRotate(ApiHelper):
    def setUp(self):
        print()
        self.configure()

        # ingress port
        self.rif0 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port0,
            vrf_handle=self.vrf10, src_mac=self.rmac)
        self.ecmp = self.add_ecmp(self.device)
        self.route4 = self.add_route(self.device, ip_prefix='13.1.1.0/24', vrf_handle=self.vrf10, nexthop_handle=self.ecmp)
        for l_port in [self.port1, self.port2, self.port3, self.port4]:
            l_rif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=l_port, vrf_handle=self.vrf10, src_mac=self.rmac)
            l_ip = '13.0.0.' + str(l_port & 256)
            l_mac = '00:22:22:33:44:' + str(l_port & 0xFF)
            l_nhop = self.add_nexthop(self.device, handle=l_rif, dest_ip=l_ip)
            l_neighbor = self.add_neighbor(self.device, mac_address=l_mac, handle=l_rif, dest_ip=l_ip)
            ecmp_member = self.add_ecmp_member(self.device, nexthop_handle=l_nhop, ecmp_handle=self.ecmp)

    def runTest(self):
        try:
            if (self.client.is_feature_enable(SWITCH_FEATURE_ECMP_DEFAULT_HASH_OFFSET) == 0):
                print("ECMP default hash offset feature not enabled, skipping")
                return
            self.EcmpIPv4HashRotateTest()
        finally:
            pass

    def tearDown(self):
        self.cleanup()

    def EcmpIPv4HashRotateTest(self):
        print("EcmpIPv4HostTest()")
        pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='13.1.1.1',
            ip_src='192.168.0.1',
            ip_id=106,
            ip_ttl=64)
        exp_pkt1 = simple_tcp_packet(
            eth_dst='00:22:22:33:44:' + str(self.port1 & 0xFF),
            eth_src='00:77:66:55:44:33',
            ip_dst='13.1.1.1',
            ip_src='192.168.0.1',
            ip_id=106,
            ip_ttl=63)
        exp_pkt2 = simple_tcp_packet(
            eth_dst='00:22:22:33:44:' + str(self.port2 & 0xFF),
            eth_src='00:77:66:55:44:33',
            ip_dst='13.1.1.1',
            ip_src='192.168.0.1',
            ip_id=106,
            ip_ttl=63)
        exp_pkt3 = simple_tcp_packet(
            eth_dst='00:22:22:33:44:' + str(self.port3 & 0xFF),
            eth_src='00:77:66:55:44:33',
            ip_dst='13.1.1.1',
            ip_src='192.168.0.1',
            ip_id=106,
            ip_ttl=63)
        exp_pkt4 = simple_tcp_packet(
            eth_dst='00:22:22:33:44:' + str(self.port4 & 0xFF),
            eth_src='00:77:66:55:44:33',
            ip_dst='13.1.1.1',
            ip_src='192.168.0.1',
            ip_id=106,
            ip_ttl=63)

        try:
            max_itrs = 100
            def packetTest():
                count = [0, 0, 0, 0]
                dst_ip = int(binascii.hexlify(socket.inet_aton('13.1.1.1')), 16)
                src_ip = int(binascii.hexlify(socket.inet_aton('192.168.8.1')), 16)
                for i in range(0, max_itrs):
                    dst_ip_addr = socket.inet_ntoa(
                        binascii.unhexlify(format(dst_ip, 'x').zfill(8)))
                    src_ip_addr = socket.inet_ntoa(
                        binascii.unhexlify(format(src_ip, 'x').zfill(8)))
                    pkt['IP'].dst = dst_ip_addr
                    pkt['IP'].src = src_ip_addr
                    exp_pkt1['IP'].dst = dst_ip_addr
                    exp_pkt1['IP'].src = src_ip_addr
                    exp_pkt2['IP'].dst = dst_ip_addr
                    exp_pkt2['IP'].src = src_ip_addr
                    exp_pkt3['IP'].dst = dst_ip_addr
                    exp_pkt3['IP'].src = src_ip_addr
                    exp_pkt4['IP'].dst = dst_ip_addr
                    exp_pkt4['IP'].src = src_ip_addr

                    send_packet(self, self.devports[0], pkt)
                    rcv_idx = verify_any_packet_any_port(
                        self, [exp_pkt1, exp_pkt2, exp_pkt3, exp_pkt4], [
                            self.devports[1], self.devports[2], self.devports[3], self.devports[4]
                        ], timeout=2)
                    count[rcv_idx] += 1
                    dst_ip += 1
                    src_ip += 1
                return count


            lb_count = packetTest()
            print(lb_count)
            for i in range(0, 4):
                self.assertTrue((lb_count[i] >= ((max_itrs / 4) * 0.7)),
                                "Ecmp paths are not equally balanced")
            self.attribute_set(self.device, SWITCH_DEVICE_ATTR_ECMP_DEFAULT_HASH_OFFSET, 11)

            lb_count1 = packetTest()
            print(lb_count1)
            for i in range(0, 4):
                self.assertTrue((lb_count[i] >= ((max_itrs / 4) * 0.7)),
                                "Ecmp paths are not equally balanced")

            self.attribute_set(self.device, SWITCH_DEVICE_ATTR_ECMP_DEFAULT_HASH_OFFSET, 3)

            lb_count2 = packetTest()
            print(lb_count2)
            for i in range(0, 4):
                self.assertTrue((lb_count[i] >= ((max_itrs / 4) * 0.7)),
                                "Ecmp paths are not equally balanced")
        finally:
            pass

###############################################################################

@group('l3')
class SWI6059Test(ApiHelper):
    def setUp(self):
        print()

        self.configure()

        self.rif0 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port0, vrf_handle=self.vrf10, src_mac=self.rmac)

        self.rif2 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port2, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.nhop2 = self.add_nexthop(self.device, handle=self.rif2, dest_ip='11.11.0.2')
        self.neighbor2 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:56', handle=self.rif2, dest_ip='11.11.0.2')

        self.rif1 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port1, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.nhop1 = self.add_nexthop(self.device, handle=self.rif1, dest_ip='10.10.0.2')
        self.neighbor1 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=self.rif1, dest_ip='10.10.0.2')

    def runTest(self):
        try:
            self.r1 = self.add_route(self.device, ip_prefix='11.11.0.0/16', vrf_handle=self.vrf10, nexthop_handle=self.rif1)
            self.attribute_set(self.r1, SWITCH_ROUTE_ATTR_NEXTHOP_HANDLE, self.nhop2)

            # delete rif1
            self.stack.pop()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
        finally:
            self.client.object_delete(self.r1)
            self.cleanlast()

    def tearDown(self):
        self.cleanup()

###############################################################################

from threading import Thread
from datetime import datetime
import random
@disabled
class SWI5310Test(ApiHelper):
    '''
    To test continous MAC learning and MAC aging with scale of 254
    '''
    #lock = threading.Lock()
    neighbors = {}

    def runTest(self):
        print()
        self.configure()
        self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port1)
        self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port2)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_LEARNING, True)
        self.attribute_set(self.port2, SWITCH_PORT_ATTR_LEARNING, True)
        self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_LEARNING, True)

        x = Thread(target=self.port_thread_mac_learn_aging, args=(1,))
        x.start()
        x.join()

    def port_thread_mac_learn_aging(self, port):
        try:
          while True:
            for i in range(1,255):
                sip = "192.168.20."+str(i)
                smac = "04:f8:f8:a2:14:"+hex(i)
                pkt = simple_tcp_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src=smac,
                    ip_dst=sip,
                    pktlen=100)

                send_packet(self, self.devports[1], pkt)
                time.sleep(0.5)

        finally:
            pass

@disabled
@group('l3')
@group('ecmp')
class L3ECMPDynamicOrderingTest(ApiHelper):

    def setUp(self):
        print()
        print("ECMP Dynamic Ordering test")

        self.configure()

        rif0 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port0, vrf_handle=self.vrf10, src_mac=self.rmac)

        # L3 RIF on access port
        rif1 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port1, vrf_handle=self.vrf10, src_mac=self.rmac)
        nhop1 = self.add_nexthop(self.device, handle=rif1, dest_ip='10.10.0.1')
        neighbor1 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=rif1, dest_ip='10.10.0.1')

        rif2 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port2, vrf_handle=self.vrf10, src_mac=self.rmac)
        nhop2 = self.add_nexthop(self.device, handle=rif2, dest_ip='10.10.0.2')
        neighbor2 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:56', handle=rif2, dest_ip='10.10.0.2')

        rif3 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port3, vrf_handle=self.vrf10, src_mac=self.rmac)
        nhop3 = self.add_nexthop(self.device, handle=rif3, dest_ip='10.10.0.3')
        neighbor1 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:57', handle=rif3, dest_ip='10.10.0.3')

        ecmp0 = self.add_ecmp(self.device, type=SWITCH_ECMP_ATTR_TYPE_ORDERED_ECMP)
        ecmp_member01 = self.add_ecmp_member(self.device, nexthop_handle=nhop1, ecmp_handle=ecmp0)
        ecmp_member02 = self.add_ecmp_member(self.device, nexthop_handle=nhop2, ecmp_handle=ecmp0)
        ecmp_member03 = self.add_ecmp_member(self.device, nexthop_handle=nhop3, ecmp_handle=ecmp0)
        self.ecmp_members = [ecmp_member01, ecmp_member02, ecmp_member03]
        self.add_route(self.device, ip_prefix='11.11.0.0/16', vrf_handle=self.vrf10, nexthop_handle=ecmp0)

    def runTest(self):
        try:
            # Testing with Continuous Priorities
            self.ECMPSequencingTest([4,5,6], [6,5,4])
            # Testing with Non Continuous Priorities
            self.ECMPSequencingTest([5, 12, 17], [12, 17, 5])
        finally:
            pass

    def ECMPSequencingTest(self, priorities1, priorities2):
        print("ECMP Dynamic Ordering Test for Priorties {} -> {}".format(priorities1, priorities2))
        print("Evaluating ECMP distrubition for Priorities {}".format(priorities1))
        for mbr, priority in zip(self.ecmp_members, priorities1):
            self.attribute_set(mbr, SWITCH_ECMP_MEMBER_ATTR_SEQUENCE_ID, priority)
        lb_count1 = self.hashDistributionTest()
        dist_dict1 = {}
        for count, priority in zip(lb_count1, priorities1):
            dist_dict1[priority] = count
        print("Evaluating ECMP distrubition for Priorities {}".format(priorities2))
        for mbr, priority in zip(self.ecmp_members, priorities2):
            self.attribute_set(mbr, SWITCH_ECMP_MEMBER_ATTR_SEQUENCE_ID, priority)
        lb_count2 = self.hashDistributionTest()
        dist_dict2 = {}
        for count, priority in zip(lb_count2, priorities2):
            dist_dict2[priority] = count
        self.assertTrue(dist_dict1 == dist_dict2, "ECMP distribution {} with Priorities {} and distribution {} with"
                        "Priorities {} are dissimilar".format(lb_count1, priorities1, lb_count2, priorities2))
    def tearDown(self):
        self.cleanup()

    def hashDistributionTest(self):
        try:
            count = [0, 0, 0]
            dst_ip = int(binascii.hexlify(socket.inet_aton('11.11.11.1')), 16)
            src_ip = int(binascii.hexlify(socket.inet_aton('192.168.8.1')), 16)
            max_itrs = 150
            for i in range(0, max_itrs):
                dst_ip_addr = socket.inet_ntoa(
                    binascii.unhexlify(format(dst_ip, 'x').zfill(8)))
                src_ip_addr = socket.inet_ntoa(
                    binascii.unhexlify(format(src_ip, 'x').zfill(8)))
                pkt = simple_tcp_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src='00:22:22:22:22:22',
                    ip_dst=dst_ip_addr,
                    ip_src=src_ip_addr,
                    ip_ttl=64)
                exp_pkt1 = simple_tcp_packet(
                    eth_dst='00:11:22:33:44:55',
                    eth_src='00:77:66:55:44:33',
                    ip_dst=dst_ip_addr,
                    ip_src=src_ip_addr,
                    ip_ttl=63)
                exp_pkt2 = simple_tcp_packet(
                    eth_dst='00:11:22:33:44:56',
                    eth_src='00:77:66:55:44:33',
                    ip_dst=dst_ip_addr,
                    ip_src=src_ip_addr,
                    ip_ttl=63)
                exp_pkt3 = simple_tcp_packet(
                    eth_dst='00:11:22:33:44:57',
                    eth_src='00:77:66:55:44:33',
                    ip_dst=dst_ip_addr,
                    ip_src=src_ip_addr,
                    ip_ttl=63)

                send_packet(self, self.devports[0], pkt)
                rcv_idx = verify_any_packet_any_port(
                    self, [exp_pkt1, exp_pkt2, exp_pkt3], [
                        self.devports[1], self.devports[2], self.devports[3],
                    ], timeout=2)
                count[rcv_idx] += 1
                dst_ip += 1
                src_ip += 1

            print('ecmp-distribution:', count)
            for i in range(0, 3):
                self.assertTrue((count[i] >= ((max_itrs / 3) * 0.7)),
                                "Ecmp paths are not equally balanced")
            return count

        finally:
            pass

class L3VrfTTLActionTest(ApiHelper):
    def setUp(self):
        print()
        self.configure()
        cpu_queue_handles = self.attribute_get(self.cpu_port_hdl, SWITCH_PORT_ATTR_QUEUE_HANDLES)
        self.hostif_trap_group = self.add_hostif_trap_group(self.device,
            queue_handle=cpu_queue_handles[0].oid,
            admin_state=True)
        self.ttl_trap = self.add_hostif_trap(self.device,
            type=SWITCH_HOSTIF_TRAP_ATTR_TYPE_TTL_ERROR,
            hostif_trap_group_handle=self.hostif_trap_group,
            packet_action=SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_REDIRECT_TO_CPU)
        self.vrf11 = self.add_vrf(self.device, id=11, src_mac=self.rmac,
            ttl_action=SWITCH_VRF_ATTR_TTL_ACTION_TRAP)
        self.rif0 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT,
            port_handle=self.port0, vrf_handle=self.vrf11, src_mac=self.rmac)
        self.rif1 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT,
            port_handle=self.port1, vrf_handle=self.vrf11, src_mac=self.rmac)
        self.nhop1 = self.add_nexthop(self.device,
                handle=self.rif1,
                dest_ip='192.168.67.1')
        self.neighbor1 = self.add_neighbor(self.device,
                mac_address='00:11:22:33:44:66',
                handle=self.rif1,
                dest_ip='192.168.67.1')
        self.route = self.add_route(self.device,
                    ip_prefix='192.168.69.0/24',
                    vrf_handle=self.vrf11,
                    nexthop_handle=self.nhop1)
        self.ttl_1_pkt = simple_udp_packet(
                            eth_src='00:77:22:33:88:11',
                            eth_dst=self.rmac,
                            ip_src='192.168.67.3',
                            ip_dst='192.168.69.1',
                            ip_ttl=1)
        cpu_pkt = simple_cpu_packet(
            packet_type=0,
            ingress_bd=0x00,
            ingress_port=swport_to_devport(self, self.devports[0]),
            ingress_ifindex=(self.port0 & 0xFFFF),
            reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_TTL_ERROR,
            inner_pkt=self.ttl_1_pkt)
        self.ttl_1_pkt_cpu = cpu_packet_mask_ingress_bd(cpu_pkt)
        self.ttl_1_pkt_routed = simple_udp_packet(
                            eth_src=self.rmac,
                            eth_dst='00:11:22:33:44:66',
                            ip_src='192.168.67.3',
                            ip_dst='192.168.69.1',
                            ip_ttl=0)

    def runTest(self):
        try:
            self.VrfTTLActionsTest()
        finally:
            pass

    def tearDown(self):
        self.cleanup()

    def VrfTTLActionsTest(self):
        if self.test_params['target'] == 'hw':
            print("Skipping on HW")
            return
        ingress_port = self.devports[0]
        route_egress_port = self.devports[1]
        def set_actions(vrf_ttl_action, trap_ttl_action):
            self.attribute_set(self.vrf11, SWITCH_VRF_ATTR_TTL_ACTION,
                vrf_ttl_action)
            self.attribute_set(self.ttl_trap,
                SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION, trap_ttl_action)
        def verify_copy(vrf_ttl_action, trap_ttl_action):
            set_actions(vrf_ttl_action, trap_ttl_action)
            send_packet(self, ingress_port, self.ttl_1_pkt)
            verify_multiple_packets_on_ports(self, [
                (self.cpu_port, [self.ttl_1_pkt_cpu]),
                (route_egress_port, [self.ttl_1_pkt_routed])
            ])
        def verify_trap(vrf_ttl_action, trap_ttl_action):
            set_actions(vrf_ttl_action, trap_ttl_action)
            send_packet(self, ingress_port, self.ttl_1_pkt)
            verify_multiple_packets_on_ports(self, [
                (self.cpu_port, [self.ttl_1_pkt_cpu])
            ])
        def verify_permit(vrf_ttl_action, trap_ttl_action):
            set_actions(vrf_ttl_action, trap_ttl_action)
            send_packet(self, ingress_port, self.ttl_1_pkt)
            verify_multiple_packets_on_ports(self, [
                (route_egress_port, [self.ttl_1_pkt_routed])
            ])
        def verify_drop(vrf_ttl_action, trap_ttl_action):
            set_actions(vrf_ttl_action, trap_ttl_action)
            send_packet(self, ingress_port, self.ttl_1_pkt)
            verify_no_other_packets(self, timeout=2)

        print("verifying behaviour for TTL=1 packets with combinations of" +
               " hostif trap and vrf TTL actions applied:")
        print("===============================================================")
        print("VRF TTL action    Hostif trap TTL action      Effective action ")
        print("===============================================================")

        print("    COPY               COPY                        COPY        ")
        verify_copy(SWITCH_VRF_ATTR_TTL_ACTION_COPY,
                    SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_COPY_TO_CPU)
        print("    COPY               TRAP                        COPY        ")
        verify_copy(SWITCH_VRF_ATTR_TTL_ACTION_COPY,
                    SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_REDIRECT_TO_CPU)
        print("    COPY               DROP                        COPY        ")
        verify_copy(SWITCH_VRF_ATTR_TTL_ACTION_COPY,
                    SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_DROP)
        print("    COPY               PERMIT                      COPY        ")
        verify_copy(SWITCH_VRF_ATTR_TTL_ACTION_COPY,
                    SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_FORWARD)

        print("    TRAP               COPY                        TRAP        ")
        verify_trap(SWITCH_VRF_ATTR_TTL_ACTION_TRAP,
                    SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_COPY_TO_CPU)
        print("    TRAP               TRAP                        TRAP        ")
        verify_trap(SWITCH_VRF_ATTR_TTL_ACTION_TRAP,
                    SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_REDIRECT_TO_CPU)
        print("    TRAP               DROP                        TRAP        ")
        verify_trap(SWITCH_VRF_ATTR_TTL_ACTION_TRAP,
                    SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_DROP)
        print("    TRAP               PERMIT                      TRAP        ")
        verify_trap(SWITCH_VRF_ATTR_TTL_ACTION_TRAP,
                    SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_FORWARD)

        print("    DROP               COPY                        DROP        ")
        verify_drop(SWITCH_VRF_ATTR_TTL_ACTION_DROP,
                    SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_COPY_TO_CPU)
        print("    DROP               TRAP                        DROP        ")
        verify_drop(SWITCH_VRF_ATTR_TTL_ACTION_DROP,
                    SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_REDIRECT_TO_CPU)
        print("    DROP               DROP                        DROP        ")
        verify_drop(SWITCH_VRF_ATTR_TTL_ACTION_DROP,
                    SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_DROP)
        print("    DROP               PERMIT                      DROP        ")
        verify_drop(SWITCH_VRF_ATTR_TTL_ACTION_DROP,
                    SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_FORWARD)

        print("    PERMIT             COPY                        PERMIT      ")
        verify_permit(SWITCH_VRF_ATTR_TTL_ACTION_FORWARD,
                      SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_COPY_TO_CPU)
        print("    PERMIT             TRAP                        PERMIT      ")
        verify_permit(SWITCH_VRF_ATTR_TTL_ACTION_FORWARD,
                      SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_REDIRECT_TO_CPU)
        print("    PERMIT             DROP                        PERMIT      ")
        verify_permit(SWITCH_VRF_ATTR_TTL_ACTION_FORWARD,
                      SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_DROP)
        print("    PERMIT             PERMIT                      PERMIT      ")
        verify_permit(SWITCH_VRF_ATTR_TTL_ACTION_FORWARD,
                      SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_FORWARD)

        print("    NONE               COPY                        COPY        ")
        verify_copy(SWITCH_VRF_ATTR_TTL_ACTION_NONE,
                      SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_COPY_TO_CPU)
        print("    NONE               TRAP                        TRAP        ")
        verify_trap(SWITCH_VRF_ATTR_TTL_ACTION_NONE,
                    SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_REDIRECT_TO_CPU)
        print("    NONE               DROP                        DROP        ")
        verify_drop(SWITCH_VRF_ATTR_TTL_ACTION_NONE,
                    SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_DROP)
        print("    NONE               PERMIT                      PERMIT      ")
        verify_permit(SWITCH_VRF_ATTR_TTL_ACTION_NONE,
                    SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_FORWARD)
        print("===============================================================")

class L3VrfIPOptionsActionTest(ApiHelper):
    def setUp(self):
        print()
        self.configure()
        cpu_queue_handles = self.attribute_get(self.cpu_port_hdl, SWITCH_PORT_ATTR_QUEUE_HANDLES)
        self.hostif_trap_group = self.add_hostif_trap_group(self.device,
            queue_handle=cpu_queue_handles[0].oid,
            admin_state=True)
        self.vrf11 = self.add_vrf(self.device, id=11, src_mac=self.rmac,
            ip_options_action=SWITCH_VRF_ATTR_IP_OPTIONS_ACTION_TRAP)
        self.rif0 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT,
            port_handle=self.port0, vrf_handle=self.vrf11, src_mac=self.rmac)
        self.rif1 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT,
            port_handle=self.port1, vrf_handle=self.vrf11, src_mac=self.rmac)
        self.nhop1 = self.add_nexthop(self.device,
                handle=self.rif1,
                dest_ip='192.168.67.1')
        self.neighbor1 = self.add_neighbor(self.device,
                mac_address='00:11:22:33:44:66',
                handle=self.rif1,
                dest_ip='192.168.67.1')
        self.route = self.add_route(self.device,
                    ip_prefix='192.168.69.0/24',
                    vrf_handle=self.vrf11,
                    nexthop_handle=self.nhop1)

        router_alert_option = IPOption(b'\x94\x04\x00\x00') # supported option

        # not passed to the forwarder
        self.unreachable_dest_pkt = simple_udp_packet(
                            eth_src='00:77:22:33:88:11',
                            eth_dst=self.rmac,
                            ip_src='192.168.67.3',
                            ip_dst='192.168.70.1',
                            ip_ttl=64,
                            ip_options=router_alert_option)
        cpu_pkt = simple_cpu_packet(
            packet_type=0,
            ingress_bd=0x00,
            ingress_port=swport_to_devport(self, self.devports[0]),
            ingress_ifindex=(self.port0 & 0xFFFF),
            reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_MAX + SWITCH_HOSTIF_TRAP_ATTR_INTERNAL_TRAP_IP_OPTIONS,
            inner_pkt=self.unreachable_dest_pkt)
        self.unreachable_dest_pkt_cpu = cpu_packet_mask_ingress_bd(cpu_pkt)

        self.pkt = simple_udp_packet(
                            eth_src='00:77:22:33:88:11',
                            eth_dst=self.rmac,
                            ip_src='192.168.67.3',
                            ip_dst='192.168.69.1',
                            ip_ttl=64,
                            ip_options=router_alert_option)
        cpu_pkt = simple_cpu_packet(
            packet_type=0,
            ingress_bd=0x00,
            ingress_port=swport_to_devport(self, self.devports[0]),
            ingress_ifindex=(self.port0 & 0xFFFF),
            reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_MAX + SWITCH_HOSTIF_TRAP_ATTR_INTERNAL_TRAP_IP_OPTIONS,
            inner_pkt=self.pkt)
        self.pkt_cpu = cpu_packet_mask_ingress_bd(cpu_pkt)
        self.pkt_routed = simple_udp_packet(
                            eth_src=self.rmac,
                            eth_dst='00:11:22:33:44:66',
                            ip_src='192.168.67.3',
                            ip_dst='192.168.69.1',
                            ip_ttl=63,
                            ip_options=router_alert_option)

    def runTest(self):
        try:
            self.VrfIPOptionsTest()
        finally:
            pass

    def tearDown(self):
        self.cleanup()

    def VrfIPOptionsTest(self):
        if self.test_params['target'] == 'hw':
            print("Skipping on HW")
            return
        ingress_port = self.devports[0]
        route_egress_port = self.devports[1]

        def set_actions(vrf_ip_options_action):
            self.attribute_set(self.vrf11, SWITCH_VRF_ATTR_IP_OPTIONS_ACTION,
                vrf_ip_options_action)

        print("verify VRF IP options action COPY")
        set_actions(SWITCH_VRF_ATTR_IP_OPTIONS_ACTION_COPY)
        send_packet(self, ingress_port, self.pkt)
        verify_multiple_packets_on_ports(self, [
            (self.cpu_port, [self.pkt_cpu]),
            (route_egress_port, [self.pkt_routed])
        ])
        send_packet(self, ingress_port, self.unreachable_dest_pkt)
        verify_multiple_packets_on_ports(self, [
            (self.cpu_port, [self.unreachable_dest_pkt_cpu]),
        ])

        print("verify VRF IP options action TRAP")
        set_actions(SWITCH_VRF_ATTR_IP_OPTIONS_ACTION_TRAP)
        send_packet(self, ingress_port, self.pkt)
        verify_multiple_packets_on_ports(self, [
            (self.cpu_port, [self.pkt_cpu])
        ])
        send_packet(self, ingress_port, self.unreachable_dest_pkt)
        verify_multiple_packets_on_ports(self, [
            (self.cpu_port, [self.unreachable_dest_pkt_cpu]),
        ])

        print("verify VRF IP options action FORWARD")
        set_actions(SWITCH_VRF_ATTR_IP_OPTIONS_ACTION_FORWARD)
        send_packet(self, ingress_port, self.pkt)
        verify_multiple_packets_on_ports(self, [
            (route_egress_port, [self.pkt_routed])
        ])
        send_packet(self, ingress_port, self.unreachable_dest_pkt)
        verify_no_other_packets(self)

        print("verify VRF IP options action DROP")
        set_actions(SWITCH_VRF_ATTR_IP_OPTIONS_ACTION_DROP)
        send_packet(self, ingress_port, self.pkt)
        verify_no_other_packets(self)
        send_packet(self, ingress_port, self.unreachable_dest_pkt)
        verify_no_other_packets(self)
@group('l3')
class VirtualRIFTest(ApiHelper):
    def setUp(self):
        print()

        self.configure()
        self.new_rmac = '00:77:66:55:44:44'
        self.new_rmac1 = '00:77:66:55:44:55'
        # ingress port
        self.rif0 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port0, vrf_handle=self.vrf10, src_mac=self.rmac)
        # nhop1
        rif1 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port1, vrf_handle=self.vrf10, src_mac=self.rmac)

        nhop4 = self.add_nexthop(self.device, handle=rif1, dest_ip='9.9.9.1')
        neighbor4 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:54', handle=rif1, dest_ip='9.9.9.1')
        self.route1 = self.add_route(self.device, ip_prefix='9.9.9.1', vrf_handle=self.vrf10, nexthop_handle=nhop4)

    def rifVirtualTest(self):
        try:
            pkt1 = simple_tcp_packet(
                eth_dst='00:77:66:55:44:44',
                ip_dst='9.9.9.1',
                ip_ttl=64)
            exp_pkt1 = simple_tcp_packet(
                eth_dst='00:11:22:33:44:54',
                eth_src='00:77:66:55:44:33',
                ip_dst='9.9.9.1',
                ip_ttl=63)
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                ip_dst='9.9.9.1',
                ip_ttl=64)
            exp_pkt = simple_tcp_packet(
                eth_dst='00:11:22:33:44:54',
                eth_src='00:77:66:55:44:33',
                ip_dst='9.9.9.1',
                ip_ttl=63)


            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt, self.devports[1])
            #sending packet with new rmac before virtual rif is added
            send_packet(self, self.devports[0], pkt1)
            verify_no_other_packets(self, timeout=1)

            rif2 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port0, vrf_handle=self.vrf10, src_mac=self.new_rmac, is_virtual=True)
            send_packet(self, self.devports[0], pkt1)
            verify_packet(self, exp_pkt1, self.devports[1])
            #remove the rif and send packets with new rmac
            self.cleanlast()
            send_packet(self, self.devports[0], pkt1)
            verify_no_other_packets(self, timeout=1)

            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt, self.devports[1])

        finally:
            pass
    def runTest(self):
        try:
            self.rifVirtualTest()
        finally:
            pass

    def tearDown(self):
        self.cleanup()

@disabled
@group('l3')
class L3SnakeLoopbackTest(ApiHelper, BfRuntimeTest):

    config = {}
    devport_port_hdl_dict = {}
    port_hdl_devport_dict = {}
    table_entries = {}
    switch_speed_to_speed_dict = {
            10000:     "10G",
            25000:     "25G",
            40000:     "40G",
            50000:     "50G",
            100000:    "100G",
            200000:    "200G",
            400000:    "400G",
                                }
    fec_to_switch_fec_dict = {
            "NONE":     SWITCH_PORT_ATTR_FEC_TYPE_NONE,
            "RS":       SWITCH_PORT_ATTR_FEC_TYPE_RS,
            "FC":       SWITCH_PORT_ATTR_FEC_TYPE_FC
                                }
    def convert_fec_to_switch_fec(self, fec):
        return self.fec_to_switch_fec_dict[fec.upper()]

    def init_config(self):
        arch = test_param_get('arch')
        port_config_file = test_param_get('port_config')
        if port_config_file==None:
            ports = []
            if arch == 'tofino':
               ports = [port for port in range(0, self.max_ports, 4)]
            elif arch == 'tofino2':
               ports = [port for port in range(0, self.max_ports, 8)]
            self.config[100000] = L3SnakeTest2.Config(ports=ports)
        else:
            f = open(port_config_file)
            port_config_dict = json.load(f)
            for alias in port_config_dict:
                port_config = port_config_dict[alias]
                if "speed" not in port_config or "fec" not in port_config or "lane_list" not in port_config:
                    raise Exception("Port config should specify speed, fec and lane list attributes for all ports. All attributes were not specified for {}:{}".format(alias, port_config))
                speed = port_config["speed"]
                fec = port_config["fec"]
                lane_list = port_config["lane_list"]
                port_speed=speed
                port_fec=self.convert_fec_to_switch_fec(fec)
                if port_speed not in self.config:
                    self.config[port_speed] =  {
                            "ports": [],
                            "fecs" : [],
                            "port_handles": [],
                            "devports": [],
                            "hostifs": [],
                            "hostif_sockets": [],
                            "macs":  [],
                            "vrfs":  [],
                            "rifs":  [],
                            "nexthops": [],
                            "neighbors": [],
                            "routes": []
                        }
                self.config[port_speed]["ports"].append(lane_list)
                self.config[port_speed]["fecs"].append(port_fec)
                self.config[port_speed]["hostifs"].append(alias)

    def test_configure(self):
        self.device = self.get_device_handle(0)
        self.max_ports = self.attribute_get(self.device, SWITCH_DEVICE_ATTR_MAX_PORTS)
        self.init_config()

        # Default fec if not specified for a given speed
        speed_fec_dict = {
             400000: SWITCH_PORT_ATTR_FEC_TYPE_RS,
             200000: SWITCH_PORT_ATTR_FEC_TYPE_RS,
             100000: SWITCH_PORT_ATTR_FEC_TYPE_RS,
             40000: SWITCH_PORT_ATTR_FEC_TYPE_NONE,
             25000: SWITCH_PORT_ATTR_FEC_TYPE_NONE,
             10000: SWITCH_PORT_ATTR_FEC_TYPE_NONE
        }

        mac_prefix = "00:01:02:03:04:"
        for kSpeed, vConfig in self.config.items():
            fecs = vConfig["fecs"]
            hostifs = vConfig["hostifs"]
            for idx, port in enumerate(vConfig["ports"]):
                if not fecs:
                        fec=speed_fec_dict[kSpeed]
                else:
                        fec=fecs[idx]
                port_hdl = self.add_port(self.device, lane_list=u.lane_list_t(port), speed=kSpeed, fec_type=fec, loopback_mode=SWITCH_PORT_ATTR_LOOPBACK_MODE_MAC_NEAR)
                vConfig["port_handles"].append(port_hdl)
                if not hostifs:
                        hostif_name ="hosti_if{}".format(port)
                else:
                        hostif_name=hostifs[idx]
                self.add_hostif(self.device, name=hostif_name, handle=port_hdl, oper_status=True)
                vConfig["hostifs"].append(hostif_name)
                vConfig["hostif_sockets"].append(open_packet_socket(hostif_name))

                mac_address = mac_prefix + hex(port[0])[2:]
                vConfig["macs"].append(mac_address)

                dev_port = self.attribute_get(port_hdl, SWITCH_PORT_ATTR_DEV_PORT)
                self.devport_port_hdl_dict[dev_port] = port_hdl
                self.port_hdl_devport_dict[port_hdl] = dev_port
                vConfig["devports"].append(dev_port)

                vrf_hdl = self.add_vrf(self.device, id=((port[0]+1)*2), src_mac=mac_address)
                vConfig["vrfs"].append(vrf_hdl)

                rif_hdl = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=port_hdl,  vrf_handle=vrf_hdl,
                src_mac=mac_address)
                vConfig["rifs"].append(rif_hdl)

                nexthop_hdl  = self.add_nexthop(self.device, handle=rif_hdl, dest_ip='10.20.10.1')
                vConfig["nexthops"].append(nexthop_hdl)

                neighbor_hdl  = self.add_neighbor(self.device, mac_address=mac_address,
                    handle=rif_hdl, dest_ip='10.20.10.1')
                vConfig["neighbors"].append(neighbor_hdl)
                print("Added port %3d (devport %3d), with speed %s" % (port[0], dev_port, self.switch_speed_to_speed_dict[kSpeed]))

        # Add static route 172.17.10.1/24
        self.ip_addr = '172.17.10.1'
        for kSpeed, vConfig in self.config.items():
            vrfs = vConfig["vrfs"]
            nexthops = vConfig["nexthops"]
            for idx, vrf in enumerate(vrfs):
                route_hdl = self.add_route(self.device, ip_prefix='172.17.10.0/24',
                    vrf_handle=vrf, nexthop_handle=nexthops[(idx+1)%(len(nexthops))])

    def clearTable(self, table, target):
        entries = {}
        resp = table.entry_get(target)
        for data, key in resp:
            entries[key]=data
        for k in entries:
            table.entry_del(target, [k])
        self.table_entries[table] = entries

    def restoreTable(self, table, target):
        for key,data in self.table_entries[table].items():
            table.entry_add(target, [key], [data])

    def runTest(self):
        self.test_configure()
        BfRuntimeTest.setUp(self, 0, "switch", notifications=client.Notifications(enable_learn=False))
        bfrt_info = self.interface.bfrt_info_get("switch")
        ingress_system_acl = bfrt_info.table_get("ingress_system_acl")
        target = client.Target(device_id=0)
        self.clearTable(ingress_system_acl, target)
        try:
            self.SnakeTest()
        finally:
            self.restoreTable(ingress_system_acl, target)
            self.cleanup()
            pass

    def SnakeTest(self):
        try:
            #Packets to Saturate the Pipe and get line rate
            counts ={   400000: 10392,
                        200000: 5196,
                        100000: 2048,
                        40000: 696,
                        25000: 325,
                        10000: 110
                    }
            for kSpeed, vConfig in self.config.items():
                if vConfig["port_handles"] and vConfig["macs"] and vConfig["hostifs"] and vConfig["hostif_sockets"]:
                    send_pkt = simple_tcp_packet(
                                eth_dst=vConfig["macs"][0],
                                eth_src='00:22:22:22:22:22',
                                ip_dst=self.ip_addr,
                                ip_src='192.168.0.1',
                                ip_id=105,
                                ip_ttl=64)
                    print("Sending packet from host interface:%s to port %d" % (vConfig["hostifs"][0], vConfig["port_handles"][0]))
                    for i in range(counts[kSpeed]):
                        socket_send_packet(send_pkt, vConfig["hostif_sockets"][0])
        finally:
            for kSpeed, vConfig in self.config.items():
                for sock in vConfig["hostif_sockets"]:
                    sock.close()
