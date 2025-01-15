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
from p4testutils.misc_utils import mask_set_do_not_care_packet

from ptf import config
from ptf.packet import *
from ptf.testutils import *
from ptf.thriftutils import *

import os
import ptf.mask as mask

from bf_switcht_api_thrift.ttypes import *
from bf_switcht_api_thrift.model_headers import *

this_dir = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(this_dir, '..'))
from common.utils import *
from common.dtel_utils import *
from common.pktpy_utils import pktpy_skip # noqa pylint: disable=wrong-import-position

# 4 trailing bytes of CRC gets added to the packet
pkt_len = int(test_param_get('pkt_size'))
crc_len = 4

from ast import literal_eval

acl_table_bp_switch = switcht_list_val_t(type=switcht_value_type.ENUM, enumdata=SWITCH_ACL_TABLE_ATTR_BIND_POINT_TYPE_SWITCH)

def get_hw_table_usage(self, table_name):
  return self.client.table_info_get(table_name).usage

@group('acl2')
class ACLTest(ApiHelper):
    high_prio = 500
    low_prio = 600
    lower_prio = 650
    def runTest(self):
        print()
        if(self.client.is_feature_enable(SWITCH_FEATURE_ACL_PORT_GROUP) == 1):
            print("ACL test not supported in the profile")
            return
        self.configure()

        self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_LEARNING, False)
        self.attribute_set(self.vlan20, SWITCH_VLAN_ATTR_LEARNING, False)

        # Create L3 interfaces on physical ports
        self.rif0 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port0, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.nhop0 = self.add_nexthop(self.device, handle=self.rif0, dest_ip='10.10.0.1')
        self.neighbor0 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:66', handle=self.rif0, dest_ip='10.10.0.1')

        self.rif1 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port1, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.nhop1 = self.add_nexthop(self.device, handle=self.rif1, dest_ip='10.10.0.2')
        self.neighbor1 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=self.rif1, dest_ip='10.10.0.2')

        self.rif2 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port3, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.nhop2 = self.add_nexthop(self.device, handle=self.rif2, dest_ip='10.10.0.3')
        self.neighbor2 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:56', handle=self.rif2, dest_ip='10.10.0.3')

        self.route0 = self.add_route(self.device, ip_prefix='10.10.10.1', vrf_handle=self.vrf10, nexthop_handle=self.nhop1)
        self.route1 = self.add_route(self.device, ip_prefix='10.10.10.2', vrf_handle=self.vrf10, nexthop_handle=self.nhop0)

        self.route1 = self.add_route(self.device, ip_prefix='1234:5678:9abc:def0:4422:1133:5577:99aa', vrf_handle=self.vrf10, nexthop_handle=self.nhop1)
        self.route2 = self.add_route(self.device, ip_prefix='10.10.10.3', vrf_handle=self.vrf10, nexthop_handle=self.nhop2)

        vlan_member0 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port2)
        vlan_member1 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port5)
        vlan_member2 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port4)
        self.attribute_set(self.port2, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.attribute_set(self.port5, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.attribute_set(self.port4, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:44:44:44:44:44', destination_handle=self.port4)

        # Create L3 interfaces on LAG
        self.lag0 = self.add_lag(self.device)
        lag_mbr0 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port6)
        lag_mbr1 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port7)
        self.rif3 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.lag0, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.nhop3 = self.add_nexthop(self.device, handle=self.rif3, dest_ip='10.10.0.4')
        self.neighbor3 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:57', handle=self.rif3, dest_ip='10.10.0.4')
        self.route3 = self.add_route(self.device, ip_prefix='10.10.10.4', vrf_handle=self.vrf10, nexthop_handle=self.nhop3)

        self.lag1 = self.add_lag(self.device)
        lag_mbr2 = self.add_lag_member(self.device, lag_handle=self.lag1, port_handle=self.port8)
        lag_mbr3 = self.add_lag_member(self.device, lag_handle=self.lag1, port_handle=self.port9)
        vlan_member3 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.lag1)
        self.attribute_set(self.lag1, SWITCH_LAG_ATTR_PORT_VLAN_ID, 10)

        # Create SVI on vlan 10
        self.rif4 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_VLAN, vlan_handle=self.vlan10, vrf_handle=self.vrf10, src_mac=self.rmac)
        mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:55:55:55:55:55', destination_handle=self.lag1)
        self.nhop4 = self.add_nexthop(self.device, handle=self.rif4, dest_ip='10.10.10.5')
        self.neighbor4 = self.add_neighbor(self.device, mac_address='00:55:55:55:55:55', handle=self.rif4, dest_ip='10.10.10.5')
        self.route4 = self.add_route(self.device, ip_prefix='10.10.10.5', vrf_handle=self.vrf10, nexthop_handle=self.nhop4)

        self.add_vlan_member(self.device, vlan_handle=self.vlan20, member_handle=self.port10)
        self.add_vlan_member(self.device, vlan_handle=self.vlan20, member_handle=self.port11)
        self.attribute_set(self.port10, SWITCH_PORT_ATTR_PORT_VLAN_ID, 20)
        self.attribute_set(self.port11, SWITCH_PORT_ATTR_PORT_VLAN_ID, 20)
        mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan20, mac_address='00:66:66:66:66:66', destination_handle=self.port10)
        mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan20, mac_address='00:77:77:77:77:77', destination_handle=self.port11)

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
        self.exp_pkt2 = simple_tcp_packet(
            eth_dst='00:11:22:33:44:56',
            eth_src='00:77:66:55:44:33',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63)
        self.pkt1 = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.10.10.5',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        self.exp_pkt1 = simple_tcp_packet(
            eth_dst='00:55:55:55:55:55',
            eth_src='00:77:66:55:44:33',
            ip_dst='10.10.10.5',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63)
        self.lag_pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.10.10.4',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        self.exp_lag_pkt = simple_tcp_packet(
            eth_dst='00:11:22:33:44:57',
            eth_src='00:77:66:55:44:33',
            ip_dst='10.10.10.4',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63)
        self.l2_pkt1 = simple_tcp_packet(
            eth_dst='00:55:55:55:55:55',
            eth_src='00:44:44:44:44:44',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        self.l2_pkt2 = simple_tcp_packet(
            eth_dst='00:44:44:44:44:44',
            eth_src='00:55:55:55:55:55',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        self.vlan20_pkt1 = simple_tcp_packet(
            eth_dst='00:66:66:66:66:66',
            eth_src='00:77:77:77:77:77',
            ip_dst='20.20.20.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        self.vlan20_pkt2 = simple_tcp_packet(
            eth_dst='00:77:77:77:77:77',
            eth_src='00:66:66:66:66:66',
            ip_dst='20.20.20.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)

        self.pkt_v6 = simple_tcpv6_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
            ipv6_src='2000::1',
            ipv6_hlim=64)
        self.exp_pkt_v6 = simple_tcpv6_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src='00:77:66:55:44:33',
            ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
            ipv6_src='2000::1',
            ipv6_hlim=63)

        try:
            self.TrafficTestWithoutAclConfig()
            self.MacAclTableTest()
            self.MacAclTablePriorityTest()
            self.IPv4AclTableTest()
            self.IPv4AclTableStatsClearTest()
            self.IPv4AclTablePriorityTest()
            self.LagIPv4AclTableTest()
            self.IPv6AclTableTest()
            self.AclRangeTest()
            self.IPv6AclTablePriorityTest()

        finally:
            self.attribute_set(self.port2, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.port5, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.port4, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.lag1, SWITCH_LAG_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.port10, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.port11, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.cleanup()

    def TrafficTestWithoutAclConfig(self):
        print('Traffic Test without Acl Config')
        print('Sending IPv4 packet port 0 -> port 1')
        send_packet(self, self.devports[0], self.pkt)
        verify_packet(self, self.exp_pkt, self.devports[1])
        print('Sending IPv4 packet vlan 10 -> port 1')
        send_packet(self, self.devports[2], self.pkt)
        verify_packet(self, self.exp_pkt, self.devports[1])
        send_packet(self, self.devports[4], self.pkt)
        verify_packet(self, self.exp_pkt, self.devports[1])
        send_packet(self, self.devports[5], self.pkt)
        verify_packet(self, self.exp_pkt, self.devports[1])
        send_packet(self, self.devports[8], self.pkt)
        verify_packet(self, self.exp_pkt, self.devports[1])
        send_packet(self, self.devports[9], self.pkt)
        verify_packet(self, self.exp_pkt, self.devports[1])
        print('Sending IPv4 packet port 0 -> lag 0')
        send_packet(self, self.devports[0], self.lag_pkt)
        verify_any_packet_any_port(self, [self.exp_lag_pkt, self.exp_lag_pkt],
                [self.devports[6], self.devports[7]])
        print('Sending IPv4 packet port 0 -> lag 1')
        send_packet(self, self.devports[0], self.pkt1)
        verify_any_packet_any_port(self, [self.exp_pkt1, self.exp_pkt1],
                [self.devports[8], self.devports[9]])
        print('Sending IPv4 packet lag 0 -> port 1')
        send_packet(self, self.devports[6], self.pkt)
        verify_packet(self, self.exp_pkt, self.devports[1])
        send_packet(self, self.devports[7], self.pkt)
        verify_packet(self, self.exp_pkt, self.devports[1])
        print('Sending IPv6 packet port 0 -> port 1')
        send_packet(self, self.devports[0], self.pkt_v6)
        verify_packet(self, self.exp_pkt_v6, self.devports[1])
        print('Sending IPv4 packet port 4 -> lag 1')
        send_packet(self, self.devports[4], self.l2_pkt1)
        verify_any_packet_any_port(self, [self.l2_pkt1, self.l2_pkt1],
                [self.devports[8], self.devports[9]])
        print('Sending IPv4 packet lag 1 -> port 4')
        send_packet(self, self.devports[8], self.l2_pkt2)
        verify_packet(self, self.l2_pkt2, self.devports[4])
        send_packet(self, self.devports[9], self.l2_pkt2)
        verify_packet(self, self.l2_pkt2, self.devports[4])
        print('Sending vlan20 L2 packet from port 11 -> port 10')
        send_packet(self, self.devports[11], self.vlan20_pkt1)
        verify_packet(self, self.vlan20_pkt1, self.devports[10])
        print('Sending vlan20 L2 packet from port 10 -> port 11')
        send_packet(self, self.devports[10], self.vlan20_pkt2)
        verify_packet(self, self.vlan20_pkt2, self.devports[11])
    ############################################################################

    def MacAclTableTest(self):
        '''
        This test tries to verify mac acl entries
        Acl is configured with port2 as in_port
        '''
        print("MacAclTableTest()")
        if (self.client.is_feature_enable(SWITCH_FEATURE_INGRESS_MAC_ACL) == 0):
            print("MAC ACL feature not enabled, skipping")
            return
        acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_MAC,
            bind_point_type=[acl_table_bp_switch],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_entry = self.add_acl_entry(self.device,
            src_mac='00:22:22:22:22:22',
            src_mac_mask='ff:ff:ff:ff:ff:ff',
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            ingress_port_lag_handle=self.port2,
            table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry)
        table_name = SWITCH_DEVICE_ATTR_TABLE_MAC_ACL
        pre_hw_table_count = get_hw_table_usage(self, table_name)
        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_INGRESS_ACL, acl_table)
        post_hw_table_count = get_hw_table_usage(self, table_name)
        self.assertEqual(post_hw_table_count - pre_hw_table_count, 1)

        def TestTraffic():
            print(" Testing Traffic")
            m_pkt = simple_eth_packet(
                eth_dst='00:44:44:44:44:44',
                eth_src='00:22:22:22:22:22')

            # Make sure packet from port2 is dropped
            pre_counter = self.object_counters_get(acl_entry)
            send_packet(self, self.devports[2], m_pkt)
            verify_no_other_packets(self, timeout=2)
            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)
            # Make sure packet from different in_port is not dropped
            send_packet(self, self.devports[0], self.pkt)
            verify_packet(self, self.exp_pkt, self.devports[1])

        try:
            TestTraffic()
            self.startFrWrReplay(1, TestTraffic)
        finally:
            self.attribute_set(self.device, SWITCH_DEVICE_ATTR_INGRESS_ACL, 0)
            hw_table_count = get_hw_table_usage(self, table_name)
            self.assertEqual(hw_table_count, 0)
            # remove the acl entry and table
            self.cleanlast()
            self.cleanlast()
    ############################################################################

    def MacAclTablePriorityTest(self):
        '''
        This test tries to veriry mac acl table priority
        '''
        print("MacAclTablePriorityTest()")
        if (self.client.is_feature_enable(SWITCH_FEATURE_INGRESS_MAC_ACL) == 0):
            print("MAC ACL feature not enabled, skipping")
            return
        acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_MAC,
            bind_point_type=[acl_table_bp_switch],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_entry1 = self.add_acl_entry(self.device,
            src_mac='00:77:77:77:77:22',
            src_mac_mask='ff:ff:ff:ff:ff:ff',
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            priority=self.high_prio,
            ingress_port_lag_handle=self.port2,
            table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry1)
        acl_entry2 = self.add_acl_entry(self.device,
            src_mac='00:77:77:77:77:00',
            src_mac_mask='ff:ff:ff:ff:ff:00',
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_PERMIT,
            priority=self.low_prio,
            ingress_port_lag_handle=self.port2,
            table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry2)
        table_name = SWITCH_DEVICE_ATTR_TABLE_MAC_ACL
        pre_hw_table_count = get_hw_table_usage(self, table_name)
        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_INGRESS_ACL, acl_table)
        post_hw_table_count = get_hw_table_usage(self, table_name)
        self.assertEqual(post_hw_table_count - pre_hw_table_count, 2)

        def TestTraffic():
            print(" Testing Traffic")
            pkt1 = simple_eth_packet(
                eth_dst='00:44:44:44:44:44',
                eth_src='00:77:77:77:77:22')
            pkt2 = simple_eth_packet(
                eth_dst='00:44:44:44:44:44',
                eth_src='00:77:77:77:77:33')

            pre_counter = self.object_counters_get(acl_entry1)
            send_packet(self, self.devports[2], pkt1)
            verify_no_other_packets(self, timeout=2)
            post_counter = self.object_counters_get(acl_entry1)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)

            pre_counter = self.object_counters_get(acl_entry2)
            send_packet(self, self.devports[2], pkt2)
            verify_packet(self, pkt2, self.devports[4])
            post_counter = self.object_counters_get(acl_entry2)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)

        try:
            TestTraffic()
            self.startFrWrReplay(0, TestTraffic)  #  WI or FI requires fix
        finally:
            # remove the acl entry1,acl_entry2 and table
            self.attribute_set(self.device, SWITCH_DEVICE_ATTR_INGRESS_ACL, 0)
            hw_table_count = get_hw_table_usage(self, table_name)
            self.assertEqual(hw_table_count, 0)
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
    ############################################################################

    def IPv4AclTableTest(self):
        '''
        This test tries to verify ipv4 acl entries
        We send a packet from port 0 and drop the packet
        The bp_type is PORT. The port_lag_label comes from the table id
        '''
        print("IPv4AclTableTest()")
        table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IPV4

        acl_table = self.add_acl_table(self.device,
            type=table_type,
            bind_point_type=[acl_table_bp_switch],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_entry = self.add_acl_entry(self.device,
            dst_ip='10.10.10.1',
            dst_ip_mask='255.255.255.255',
            #tcp_flags=2,
            #tcp_flags_mask=127,
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            ingress_port_lag_handle=self.port0,
            table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry)
        table_name = SWITCH_DEVICE_ATTR_TABLE_IPV4_ACL
        pre_hw_table_count = get_hw_table_usage(self, table_name)
        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_INGRESS_ACL, acl_table)
        post_hw_table_count = get_hw_table_usage(self, table_name)
        self.assertEqual(post_hw_table_count - pre_hw_table_count, 1)

        def TestTraffic():
            print(" Testing Traffic")
            pre_counter = self.object_counters_get(acl_entry)
            send_packet(self, self.devports[0], self.pkt)
            verify_no_other_packets(self, timeout=2)
            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)
            # Make sure packet from different in_port is not dropped
            send_packet(self, self.devports[2], self.pkt)
            verify_packet(self, self.exp_pkt, self.devports[1])

        try:
            TestTraffic()
            self.startFrWrReplay(0, TestTraffic)  # WI or FI not testing
        finally:
            # remove the acl entry and table
            self.attribute_set(self.device, SWITCH_DEVICE_ATTR_INGRESS_ACL, 0)
            hw_table_count = get_hw_table_usage(self, table_name)
            self.assertEqual(hw_table_count, 0)
            self.cleanlast()
            self.cleanlast()
    ############################################################################

    def IPv4AclTableStatsClearTest(self):
        '''
        This test tries to verify stast clear in ipv4 acl entries
        We send a packet from port 0 and drop the packet
        The bp_type is PORT. The port_lag_label comes from the table id
        '''
        print("IPv4AclTableStatsClearTest()")
        table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IPV4

        acl_table = self.add_acl_table(self.device,
            type=table_type,
            bind_point_type=[acl_table_bp_switch],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_entry = self.add_acl_entry(self.device,
            dst_ip='10.10.10.1',
            dst_ip_mask='255.255.255.255',
            #tcp_flags=2,
            #tcp_flags_mask=127,
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            ingress_port_lag_handle=self.port0,
            table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry)
        table_name = SWITCH_DEVICE_ATTR_TABLE_IPV4_ACL
        pre_hw_table_count = get_hw_table_usage(self, table_name)
        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_INGRESS_ACL, acl_table)
        post_hw_table_count = get_hw_table_usage(self, table_name)
        self.assertEqual(post_hw_table_count - pre_hw_table_count, 1)

        def TestTraffic():
            print(" Testing Traffic")
            pre_counter = self.object_counters_get(acl_entry)
            send_packet(self, self.devports[0], self.pkt)
            verify_no_other_packets(self, timeout=2)
            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[SWITCH_ACL_ENTRY_COUNTER_ID_PKTS].count -
                pre_counter[SWITCH_ACL_ENTRY_COUNTER_ID_PKTS].count, 1)
            self.assertTrue(post_counter[SWITCH_ACL_ENTRY_COUNTER_ID_BYTES].count -
                pre_counter[SWITCH_ACL_ENTRY_COUNTER_ID_BYTES].count != 0)

            cntr_ids = [
            SWITCH_ACL_ENTRY_COUNTER_ID_BYTES]
            self.client.object_counters_clear(acl_entry, cntr_ids)
            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[SWITCH_ACL_ENTRY_COUNTER_ID_PKTS].count -
                pre_counter[SWITCH_ACL_ENTRY_COUNTER_ID_PKTS].count, 1)
            self.assertEqual(post_counter[SWITCH_ACL_ENTRY_COUNTER_ID_BYTES].count, 0)

            self.client.object_counters_clear_all(acl_entry)
            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[SWITCH_ACL_ENTRY_COUNTER_ID_PKTS].count, 0)
            self.assertEqual(post_counter[SWITCH_ACL_ENTRY_COUNTER_ID_BYTES].count, 0)

        try:
            TestTraffic()
            self.startFrWrReplay(1, TestTraffic)
        finally:
            # remove the acl entry and table
            self.attribute_set(self.device, SWITCH_DEVICE_ATTR_INGRESS_ACL, 0)
            hw_table_count = get_hw_table_usage(self, table_name)
            self.assertEqual(hw_table_count, 0)
            self.cleanlast()
            self.cleanlast()
    ############################################################################

    def IPv4AclTablePriorityTest(self):
        '''
        This test tries to veriry v4 acl table priority
        '''
        print("IPv4AclTablePriorityTest()")
        table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IPV4

        acl_table = self.add_acl_table(self.device,
            type=table_type,
            bind_point_type=[acl_table_bp_switch],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_entry1 = self.add_acl_entry(self.device,
            dst_ip='10.10.10.1',
            dst_ip_mask='255.255.255.255',
            #tcp_flags=2,
            #tcp_flags_mask=127,
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            priority = self.high_prio,
            ingress_port_lag_handle=self.port0,
            table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry1)
        acl_entry2 = self.add_acl_entry(self.device,
            dst_ip='10.10.10.0',
            dst_ip_mask='255.255.255.0',
            priority=self.low_prio,
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_PERMIT,
            ingress_port_lag_handle=self.port0,
            table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry2)

        route0 = self.add_route(self.device, ip_prefix='10.10.10.0/24', vrf_handle=self.vrf10,
                                nexthop_handle=self.nhop1)
        table_name = SWITCH_DEVICE_ATTR_TABLE_IPV4_ACL
        pre_hw_table_count = get_hw_table_usage(self, table_name)
        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_INGRESS_ACL, acl_table)
        post_hw_table_count = get_hw_table_usage(self, table_name)
        self.assertEqual(post_hw_table_count - pre_hw_table_count, 2)

        def TestTraffic():
            print(" Testing Traffic")
            pre_counter = self.object_counters_get(acl_entry1)
            send_packet(self, self.devports[0], self.pkt)
            verify_no_other_packets(self, timeout=2)
            post_counter = self.object_counters_get(acl_entry1)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)

            acl_entry2_pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='10.10.10.8',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_ttl=64)
            acl_entry2_exp_pkt = simple_tcp_packet(
                eth_dst='00:11:22:33:44:55',
                eth_src='00:77:66:55:44:33',
                ip_dst='10.10.10.8',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_ttl=63)
            pre_counter = self.object_counters_get(acl_entry2)
            send_packet(self, self.devports[0], acl_entry2_pkt)
            verify_packet(self, acl_entry2_exp_pkt, self.devports[1])
            post_counter = self.object_counters_get(acl_entry2)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)

        try:
            TestTraffic()
            self.startFrWrReplay(0, TestTraffic)  # WI or FI requires fix
        finally:
            # remove route0
            self.cleanlast()
            # remove the acl entry1, acl_entry2 and table
            self.attribute_set(self.device, SWITCH_DEVICE_ATTR_INGRESS_ACL, 0)
            hw_table_count = get_hw_table_usage(self, table_name)
            self.assertEqual(hw_table_count, 0)
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
    ############################################################################

    def LagIPv4AclTableTest(self):
        '''
        This test tries to verify ipv4 acl entries
        We send a packet from lag 0 and drop the packet
        The bp_type is LAG. The port_lag_label comes from the table id
        '''
        print("LagIPv4AclTableTest()")
        table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IPV4

        acl_table = self.add_acl_table(self.device,
            type=table_type,
            bind_point_type=[acl_table_bp_switch],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_entry = self.add_acl_entry(self.device,
            dst_ip='10.10.10.1',
            dst_ip_mask='255.255.255.255',
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            ingress_port_lag_handle=self.lag0,
            table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry)
        table_name = SWITCH_DEVICE_ATTR_TABLE_IPV4_ACL
        pre_hw_table_count = get_hw_table_usage(self, table_name)
        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_INGRESS_ACL, acl_table)
        post_hw_table_count = get_hw_table_usage(self, table_name)
        self.assertEqual(post_hw_table_count - pre_hw_table_count, 1)

        def TestTraffic():
            print(" Testing Traffic")
            pre_counter = self.object_counters_get(acl_entry)
            send_packet(self, self.devports[6], self.pkt)
            verify_no_other_packets(self, timeout=2)
            send_packet(self, self.devports[7], self.pkt)
            verify_no_other_packets(self, timeout=2)
            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 2)
            # Make sure packet from different in_port is not dropped
            send_packet(self, self.devports[2], self.pkt)
            verify_packet(self, self.exp_pkt, self.devports[1])

        try:
            TestTraffic()
            self.startFrWrReplay(1, TestTraffic)
        finally:
            # remove the acl entry and table
            self.attribute_set(self.device, SWITCH_DEVICE_ATTR_INGRESS_ACL, 0)
            hw_table_count = get_hw_table_usage(self, table_name)
            self.assertEqual(hw_table_count, 0)
            self.cleanlast()
            self.cleanlast()
    ############################################################################


    def IPv6AclTableTest(self):
        '''
        This test tries to verify ipv6 acl entries
        We send a packet from port 0 and drop the packet
        The bp_type is PORT. The port_lag_label comes from the table id
        '''
        print("IPv6AclTableTest()")
        table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IPV6

        acl_table = self.add_acl_table(self.device,
            type=table_type,
            bind_point_type=[acl_table_bp_switch],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_entry = self.add_acl_entry(self.device,
            dst_ip='1234:5678:9abc:def0:4422:1133:5577:99aa',
            dst_ip_mask='FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF',
            #tcp_flags=2,
            #tcp_flags_mask=127,
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            ingress_port_lag_handle=self.port0,
            table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry)
        table_name = SWITCH_DEVICE_ATTR_TABLE_IPV6_ACL
        pre_hw_table_count = get_hw_table_usage(self, table_name)
        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_INGRESS_ACL, acl_table)
        post_hw_table_count = get_hw_table_usage(self, table_name)
        self.assertEqual(post_hw_table_count - pre_hw_table_count, 1)

        def TestTraffic():
            print(" Testing Traffic")
            pre_counter = self.object_counters_get(acl_entry)
            send_packet(self, self.devports[0], self.pkt_v6)
            verify_no_other_packets(self, timeout=2)
            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)
            # Make sure packet from different in_port is not dropped
            send_packet(self, self.devports[2], self.pkt)
            verify_packet(self, self.exp_pkt, self.devports[1])

        try:
            TestTraffic()
            self.startFrWrReplay(1, TestTraffic)
        finally:
            # remove the acl entry and table
            self.attribute_set(self.device, SWITCH_DEVICE_ATTR_INGRESS_ACL, 0)
            hw_table_count = get_hw_table_usage(self, table_name)
            self.assertEqual(hw_table_count, 0)
            self.cleanlast()
            self.cleanlast()
    ############################################################################

    def IPv6AclTablePriorityTest(self):
        '''
        This test tries to veriry v6 acl table priority
        '''
        print("IPv6AclTablePriorityTest()")
        table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IPV6

        acl_table = self.add_acl_table(self.device,
            type=table_type,
            bind_point_type=[acl_table_bp_switch],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_entry1 = self.add_acl_entry(self.device,
            dst_ip='1234:5678:9abc:def0:4422:1133:5577:99aa',
            dst_ip_mask='FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF',
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            priority = self.high_prio,
            ingress_port_lag_handle=self.port0,
            table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry1)
        acl_entry2 = self.add_acl_entry(self.device,
            dst_ip='1234:5678:9abc:def0:4422:1133:5577:0000',
            dst_ip_mask='FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:0000',
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_PERMIT,
            priority = self.low_prio,
            ingress_port_lag_handle=self.port0,
            table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry2)
        route0 = self.add_route(self.device, ip_prefix='1234:5678:9abc:def0:4422:1133:5577:0000/112',
                                vrf_handle=self.vrf10, nexthop_handle=self.nhop1)
        table_name = SWITCH_DEVICE_ATTR_TABLE_IPV6_ACL
        pre_hw_table_count = get_hw_table_usage(self, table_name)
        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_INGRESS_ACL, acl_table)
        post_hw_table_count = get_hw_table_usage(self, table_name)
        self.assertEqual(post_hw_table_count - pre_hw_table_count, 2)

        def TestTraffic():
            print(" Testing Traffic")
            pre_counter = self.object_counters_get(acl_entry1)
            send_packet(self, self.devports[0], self.pkt_v6)
            verify_no_other_packets(self, timeout=2)
            post_counter = self.object_counters_get(acl_entry1)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)

            acl_entry2_pkt_v6 = simple_tcpv6_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:88aa',
                ipv6_src='2000::1',
                ipv6_hlim=64)
            acl_entry2_exp_pkt_v6 = simple_tcpv6_packet(
                eth_dst='00:11:22:33:44:55',
                eth_src='00:77:66:55:44:33',
                ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:88aa',
                ipv6_src='2000::1',
                ipv6_hlim=63)
            pre_counter = self.object_counters_get(acl_entry2)
            send_packet(self, self.devports[0], acl_entry2_pkt_v6)
            verify_packet(self, acl_entry2_exp_pkt_v6, self.devports[1])
            post_counter = self.object_counters_get(acl_entry2)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)

        try:
            TestTraffic()
            self.startFrWrReplay(0, TestTraffic)  # WI or FI requires fix
        finally:
            # clean route0
            self.cleanlast()
            # remove the acl_entry1, acl_entry2  and table
            self.attribute_set(self.device, SWITCH_DEVICE_ATTR_INGRESS_ACL, 0)
            hw_table_count = get_hw_table_usage(self, table_name)
            self.assertEqual(hw_table_count, 0)
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
    ############################################################################

    def AclRangeTest(self):
        print("AclRangeTest()")
        if not (self.client.is_feature_enable(SWITCH_FEATURE_INGRESS_L4_PORT_RANGE)):
            print("L4 port range feature not enabled, skipping")
            return

        src = switcht_range_t(min=3000, max=4000)
        dst = switcht_range_t(min=5000, max=6000)
        src_range = self.add_acl_range(self.device,
            type=SWITCH_ACL_RANGE_ATTR_TYPE_SRC_PORT, range=src)
        dst_range = self.add_acl_range(self.device,
            type=SWITCH_ACL_RANGE_ATTR_TYPE_DST_PORT, range=dst)

        table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IPV4

        def create_table_antries(self, direction, src_range, dst_range):
            acl_table = self.add_acl_table(self.device,
                type=table_type,
                bind_point_type=[acl_table_bp_switch],
                direction=direction)
            self.acl_entry1 = self.add_acl_entry(self.device,
                src_ip='192.168.0.1',
                src_ip_mask='255.255.255.255',
                dst_ip='10.10.10.1',
                dst_ip_mask='255.255.255.255',
                dst_port_range_id=dst_range,
                priority = self.low_prio,
                packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
                ingress_port_lag_handle=self.port0,
                table_handle=acl_table)
            self.client.object_counters_clear_all(self.acl_entry1)
            self.acl_entry2 = self.add_acl_entry(self.device,
                src_ip='192.168.0.1',
                src_ip_mask='255.255.255.255',
                dst_ip='10.10.10.1',
                dst_ip_mask='255.255.255.255',
                src_port_range_id=src_range,
                priority = self.low_prio,
                packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
                ingress_port_lag_handle=self.port0,
                table_handle=acl_table)
            self.client.object_counters_clear_all(self.acl_entry2)
            self.acl_entry3 = self.add_acl_entry(self.device,
                src_ip='192.168.0.1',
                src_ip_mask='255.255.255.255',
                dst_ip='10.10.10.1',
                dst_ip_mask='255.255.255.255',
                src_port_range_id=src_range,
                dst_port_range_id=dst_range,
                priority = self.high_prio,
                packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
                ingress_port_lag_handle=self.port0,
                table_handle=acl_table)
            self.client.object_counters_clear_all(self.acl_entry3)
            self.acl_entry4 = self.add_acl_entry(self.device,
                src_ip='192.168.0.5',
                src_ip_mask='255.255.255.255',
                priority = self.lower_prio,
                packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
                ingress_port_lag_handle=self.port0,
                table_handle=acl_table)
            self.client.object_counters_clear_all(self.acl_entry4)
            return (acl_table, self.acl_entry1, self.acl_entry2, self.acl_entry3, self.acl_entry4)


        def TestTraffic():
            print(" Testing Traffic")
            # Must match IP, lou-dst in entry 1
            pkt1 = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='10.10.10.1',
                ip_src='192.168.0.1',
                tcp_sport=2333,
                tcp_dport=5555,
                ip_id=105,
                ip_ttl=64)
            # Must match IP, lou-src in entry 2
            pkt2 = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='10.10.10.1',
                ip_src='192.168.0.1',
                tcp_sport=3333,
                tcp_dport=6333,
                ip_id=105,
                ip_ttl=64)
            # Must match IP, lou-src, lou-dst in entry 3
            pkt3 = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='10.10.10.1',
                ip_src='192.168.0.1',
                tcp_sport=3333,
                tcp_dport=5555,
                ip_id=105,
                ip_ttl=64)
            # Must match only IP in entry 4
            pkt4 = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='10.10.10.1',
                ip_src='192.168.0.5',
                tcp_sport=3333,
                tcp_dport=5555,
                ip_id=105,
                ip_ttl=64)

            pre_counter1 = self.object_counters_get(self.acl_entry1)
            pre_counter2 = self.object_counters_get(self.acl_entry2)
            pre_counter3 = self.object_counters_get(self.acl_entry3)
            pre_counter4 = self.object_counters_get(self.acl_entry4)

            send_packet(self, self.devports[0], pkt1)
            verify_no_other_packets(self, timeout=2)
            post_counter1 = self.object_counters_get(self.acl_entry1)
            post_counter2 = self.object_counters_get(self.acl_entry2)
            post_counter3 = self.object_counters_get(self.acl_entry3)
            post_counter4 = self.object_counters_get(self.acl_entry4)
            '''
            print(" Acl Entry 1 --> pre count %d, post count %d" %(pre_counter1[0].count, post_counter1[0].count))
            print(" Acl Entry 2 --> pre count %d, post count %d" %(pre_counter2[0].count, post_counter2[0].count))
            print(" Acl Entry 3 --> pre count %d, post count %d" %(pre_counter3[0].count, post_counter3[0].count))
            print(" Acl Entry 4 --> pre count %d, post count %d" %(pre_counter4[0].count, post_counter4[0].count))
            '''
            self.assertEqual(post_counter1[0].count - pre_counter1[0].count, 1)
            self.assertEqual(post_counter2[0].count - pre_counter2[0].count, 0)
            self.assertEqual(post_counter3[0].count - pre_counter3[0].count, 0)
            self.assertEqual(post_counter4[0].count - pre_counter4[0].count, 0)

            send_packet(self, self.devports[0], pkt2)
            verify_no_other_packets(self, timeout=2)
            post_counter1 = self.object_counters_get(self.acl_entry1)
            post_counter2 = self.object_counters_get(self.acl_entry2)
            post_counter3 = self.object_counters_get(self.acl_entry3)
            post_counter4 = self.object_counters_get(self.acl_entry4)
            self.assertEqual(post_counter1[0].count - pre_counter1[0].count, 1)
            self.assertEqual(post_counter2[0].count - pre_counter2[0].count, 1)
            self.assertEqual(post_counter3[0].count - pre_counter3[0].count, 0)
            self.assertEqual(post_counter4[0].count - pre_counter4[0].count, 0)

            send_packet(self, self.devports[0], pkt3)
            verify_no_other_packets(self, timeout=2)
            post_counter1 = self.object_counters_get(self.acl_entry1)
            post_counter2 = self.object_counters_get(self.acl_entry2)
            post_counter3 = self.object_counters_get(self.acl_entry3)
            post_counter4 = self.object_counters_get(self.acl_entry4)
            self.assertEqual(post_counter1[0].count - pre_counter1[0].count, 1)
            self.assertEqual(post_counter2[0].count - pre_counter2[0].count, 1)
            self.assertEqual(post_counter3[0].count - pre_counter3[0].count, 1)
            self.assertEqual(post_counter4[0].count - pre_counter4[0].count, 0)

            send_packet(self, self.devports[0], pkt4)
            verify_no_other_packets(self, timeout=2)
            post_counter1 = self.object_counters_get(self.acl_entry1)
            post_counter2 = self.object_counters_get(self.acl_entry2)
            post_counter3 = self.object_counters_get(self.acl_entry3)
            post_counter4 = self.object_counters_get(self.acl_entry4)
            self.assertEqual(post_counter1[0].count - pre_counter1[0].count, 1)
            self.assertEqual(post_counter2[0].count - pre_counter2[0].count, 1)
            self.assertEqual(post_counter3[0].count - pre_counter3[0].count, 1)
            self.assertEqual(post_counter4[0].count - pre_counter4[0].count, 1)

        try:
            if self.client.is_feature_enable(SWITCH_FEATURE_INGRESS_L4_PORT_RANGE):
                try:
                    t, e1, e2, e3, e4 = create_table_antries(self, SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS, src_range, dst_range)
                    table_name = SWITCH_DEVICE_ATTR_TABLE_IPV4_ACL
                    pre_hw_table_count = get_hw_table_usage(self, table_name)
                    self.attribute_set(self.device, SWITCH_DEVICE_ATTR_INGRESS_ACL, t)
                    post_hw_table_count = get_hw_table_usage(self, table_name)
                    self.assertEqual(post_hw_table_count - pre_hw_table_count, 4)
                    TestTraffic()
                    self.startFrWrReplay(1, TestTraffic)
                finally:
                    #  remove the acl entry and table
                    self.attribute_set(self.device, SWITCH_DEVICE_ATTR_INGRESS_ACL, 0)
                    hw_table_count = get_hw_table_usage(self, table_name)
                    self.assertEqual(hw_table_count, 0)
                    self.cleanlast()
                    self.cleanlast()
                    self.cleanlast()
                    self.cleanlast()
                    self.cleanlast()

        finally:
            self.cleanlast()
            self.cleanlast()

###############################################################################
@group('acl2')
class ACLFieldsUnitTest(ApiHelper):
    def runTest(self):
        print()
        self.configure()

        # L3 interfaces
        self.rif0 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port0, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.nhop0 = self.add_nexthop(self.device, handle=self.rif0, dest_ip='10.10.0.1')
        self.neighbor0 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:66', handle=self.rif0, dest_ip='10.10.0.1')

        self.rif1 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port1, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.nhop1 = self.add_nexthop(self.device, handle=self.rif1, dest_ip='10.10.0.2')
        self.neighbor1 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=self.rif1, dest_ip='10.10.0.2')

        self.rif2 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port2, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.nhop2 = self.add_nexthop(self.device, handle=self.rif2, dest_ip='10.10.0.3')
        self.neighbor2 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:56', handle=self.rif2, dest_ip='10.10.0.3')

        self.route0 = self.add_route(self.device, ip_prefix='10.10.10.0/16', vrf_handle=self.vrf10, nexthop_handle=self.nhop1)
        self.route1 = self.add_route(self.device, ip_prefix='1234:5678:9abc:def0:4422:1133:0:0/96', vrf_handle=self.vrf10, nexthop_handle=self.nhop1)

        # test packets
        self.pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.10.10.1',
            ip_src='20.20.20.1',
            ip_id=105,
            ip_tos=8,
            tcp_sport=3333,
            tcp_dport=5555,
            tcp_flags="R",
            ip_ttl=64)
        self.exp_pkt = simple_tcp_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src='00:77:66:55:44:33',
            ip_dst='10.10.10.1',
            ip_src='20.20.20.1',
            ip_id=105,
            ip_tos=8,
            tcp_sport=3333,
            tcp_dport=5555,
            tcp_flags="R",
            ip_ttl=63)
        self.exp_pkt2 = simple_tcp_packet(
            eth_dst='00:11:22:33:44:56',
            eth_src='00:77:66:55:44:33',
            ip_dst='10.10.10.1',
            ip_src='20.20.20.1',
            ip_id=105,
            ip_tos=8,
            tcp_sport=3333,
            tcp_dport=5555,
            tcp_flags="R",
            ip_ttl=63)
        self.allow_pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.10.10.2',
            ip_src='20.20.20.2',
            ip_id=106,
            ip_tos=4,
            tcp_sport=3332,
            tcp_dport=5556,
            tcp_flags="S",
            ip_ttl=32)
        self.exp_allow_pkt = simple_tcp_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src='00:77:66:55:44:33',
            ip_dst='10.10.10.2',
            ip_src='20.20.20.2',
            ip_id=106,
            ip_tos=4,
            tcp_sport=3332,
            tcp_dport=5556,
            tcp_flags="S",
            ip_ttl=31)

        self.pkt_v6 = simple_tcpv6_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
            ipv6_src='2000::1',
            ipv6_dscp=2,
            tcp_sport=3333,
            tcp_dport=5555,
            tcp_flags="R",
            ipv6_hlim=64)
        self.exp_pkt_v6 = simple_tcpv6_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src='00:77:66:55:44:33',
            ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
            ipv6_src='2000::1',
            ipv6_dscp=2,
            tcp_sport=3333,
            tcp_dport=5555,
            tcp_flags="R",
            ipv6_hlim=63)
        self.allow_pkt_v6 = simple_tcpv6_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99ab',
            ipv6_src='2000::2',
            ipv6_dscp=4,
            tcp_sport=3332,
            tcp_dport=5556,
            tcp_flags="S",
            ipv6_hlim=33)
        self.exp_allow_pkt_v6 = simple_tcpv6_packet(
            eth_dst='00:11:22:33:44:55',
            eth_src='00:77:66:55:44:33',
            ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99ab',
            ipv6_src='2000::2',
            ipv6_dscp=4,
            tcp_sport=3332,
            tcp_dport=5556,
            tcp_flags="S",
            ipv6_hlim=32)

        try:
            self.IPv4AclTableFieldTest()
            #self.IPv6AclTableFieldTest()
        finally:
            self.cleanup()

    def IPv4AclTableFieldTest(self):
        print("IPv4AclTableFieldTest()")
        table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IPV4

        print("Testing every field hit in this table")
        '''
        #define INGRESS_IPV4_ACL_KEY
        lkp.ip_src_addr[31:0] : ternary;
        lkp.ip_dst_addr[31:0] : ternary;
        lkp.ip_proto : ternary;
        lkp.ip_tos : ternary;
        lkp.l4_src_port : ternary;
        lkp.l4_dst_port : ternary;
        lkp.ip_ttl : ternary;
        lkp.ip_frag : ternary;
        lkp.tcp_flags : ternary;
        lkp.mac_type : ternary;
        '''
        acl_table = self.add_acl_table(self.device,
            type=table_type,
            bind_point_type=[acl_table_bp_switch],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_INGRESS_ACL, acl_table)

        def ip_frag_head():
            print(" Allow non fragmented valid ipv4 packet")
            send_packet(self, self.devports[0], self.pkt)
            verify_packet(self, self.exp_pkt, self.devports[1])
            self.pkt['IP'].flags = 1
            self.pkt['IP'].frag = 0
            print(" drop first fragment")
            send_packet(self, self.devports[0], self.pkt)
            verify_no_packet(self, self.exp_pkt, self.devports[1], timeout=1)
            self.cleanlast()
            self.pkt['IP'].flags = 0
            self.pkt['IP'].frag = 0

        def ip_frag_more_frag():
            print(" Allow non fragmented valid ipv4 packet")
            send_packet(self, self.devports[0], self.pkt)
            verify_packet(self, self.exp_pkt, self.devports[1])
            self.pkt['IP'].flags = 1
            self.pkt['IP'].frag = 20
            print(" drop non first fragment")
            send_packet(self, self.devports[0], self.pkt)
            verify_no_packet(self, self.exp_pkt, self.devports[1], timeout=1)
            self.cleanlast()
            self.pkt['IP'].flags = 0
            self.pkt['IP'].frag = 0

        def ip_frag_last_frag():
            print(" Allow non fragmented valid ipv4 packet")
            send_packet(self, self.devports[0], self.pkt)
            verify_packet(self, self.exp_pkt, self.devports[1])
            self.pkt['IP'].flags = 0
            self.pkt['IP'].frag = 20
            print(" drop last fragment")
            send_packet(self, self.devports[0], self.pkt)
            verify_no_packet(self, self.exp_pkt, self.devports[1], timeout=1)
            self.cleanlast()
            self.pkt['IP'].flags = 0
            self.pkt['IP'].frag = 0

        def ip_frag_any():
            print(" Allow non fragmented valid ipv4 packet")
            send_packet(self, self.devports[0], self.pkt)
            verify_packet(self, self.exp_pkt, self.devports[1])
            print(" drop first fragment")
            self.pkt['IP'].flags = 1
            self.pkt['IP'].frag = 0
            send_packet(self, self.devports[0], self.pkt)
            verify_no_packet(self, self.exp_pkt, self.devports[1], timeout=1)
            print(" drop non first fragment")
            self.pkt['IP'].flags = 1
            self.pkt['IP'].frag = 20
            send_packet(self, self.devports[0], self.pkt)
            verify_no_packet(self, self.exp_pkt, self.devports[1], timeout=1)
            print(" drop last fragment")
            self.pkt['IP'].flags = 0
            self.pkt['IP'].frag = 20
            send_packet(self, self.devports[0], self.pkt)
            verify_no_packet(self, self.exp_pkt, self.devports[1], timeout=1)
            self.cleanlast()
            self.pkt['IP'].flags = 0
            self.pkt['IP'].frag = 0

        rules = [
            ["{'dst_ip': '10.10.10.1', 'dst_ip_mask': '255.255.255.255'}", True, None],
            ["{'src_ip': '20.20.20.1', 'src_ip_mask': '255.255.255.255'}", True, None],
            ["{'ip_proto': 6, 'ip_proto_mask': 127}", False, None],
            ["{'ip_dscp': 2, 'ip_dscp_mask': 127}", True, None],
            ["{'l4_src_port': 3333, 'l4_src_port_mask': 32759}", True, None],
            ["{'l4_dst_port': 5555, 'l4_dst_port_mask': 32759}", True, None],
            ["{'tcp_flags': 4, 'tcp_flags_mask': 127}", True, None],
            #["{'eth_type': 0x0800, 'eth_type_mask': 0x7FFF}", True, None],
        ]

        def TestTraffic():
            print(" Testing Traffic")
            print(" Send packet with no ACL rule")
            send_packet(self, self.devports[0], self.pkt)
            verify_packet(self, self.exp_pkt, self.devports[1])

            for rule in rules:
                rule_name = rule[0].split(', ')[0].split('\'')[1]

                print(" Add ACL entry to match and deny on %s field" % rule_name)
                acl_entry = self.add_acl_entry(self.device,
                    packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
                    ingress_port_lag_handle=self.port0,
                    table_handle=acl_table,
                    **literal_eval(rule[0]))

                if rule[2] is None:
                    send_packet(self, self.devports[0], self.pkt)
                    verify_no_packet(self, self.exp_pkt, self.devports[1], timeout=1)

                    if rule[1]:
                        print(" do not drop unmatched packet")
                        send_packet(self, self.devports[0], self.allow_pkt)
                        verify_packet(self, self.exp_allow_pkt, self.devports[1])

                    print(" Remove ACL and send packet with no ACL rule")
                    self.cleanlast()
                    send_packet(self, self.devports[0], self.pkt)
                    verify_packet(self, self.exp_pkt, self.devports[1])
                else:
                    # Execute TC specific test scenario
                    rule[2]()

        try:
            TestTraffic()
            self.startFrWrReplay(1, TestTraffic)
        finally:
            # remove the acl entry and table
            self.attribute_set(self.device, SWITCH_DEVICE_ATTR_INGRESS_ACL, 0)
            self.cleanlast()

    def IPv6AclTableFieldTest(self):
        print("IPv6AclTableFieldTest()")
        table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IPV6

        print("Testing every field hit in this table")
        '''
        #define INGRESS_IPV6_ACL_KEY
        lkp.ip_src_addr : ternary;
        lkp.ip_dst_addr : ternary;
        lkp.ip_proto : ternary;
        lkp.ip_tos : ternary;
        lkp.l4_src_port : ternary;
        lkp.l4_dst_port : ternary;
        lkp.ip_ttl : ternary;
        lkp.tcp_flags : ternary;
        '''
        acl_table = self.add_acl_table(self.device,
            type=table_type,
            bind_point_type=[acl_table_bp_switch],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_INGRESS_ACL, acl_table)

        rules = [
            ["{'dst_ip': '1234:5678:9abc:def0:4422:1133:5577:99aa', 'dst_ip_mask': 'FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF'}", True],
            ["{'src_ip': '2000::1', 'src_ip_mask': 'FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF'}", True],
            ["{'ip_proto': 6, 'ip_proto_mask': 127}", False],
            ["{'ip_dscp': 2, 'ip_dscp_mask': 127}", True],
            ["{'l4_src_port': 3333, 'l4_src_port_mask': 32759}", True],
            ["{'l4_dst_port': 5555, 'l4_dst_port_mask': 32759}", True],
            ["{'tcp_flags': 4, 'tcp_flags_mask': 127}", True],
        ]

        def TestTraffic():
            print(" Testing Traffic")
            print(" Send packet with no ACL rule")
            send_packet(self, self.devports[0], self.pkt_v6)
            verify_packet(self, self.exp_pkt_v6, self.devports[1])

            for rule in rules:
                rule_name = rule[0].split(', ')[0].split('\'')[1]

                print(" Add ACL entry to match and deny on %s field" % rule_name)
                acl_entry = self.add_acl_entry(self.device,
                    packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
                    ingress_port_lag_handle=self.port0,
                    table_handle=acl_table,
                    **literal_eval(rule[0]))
                send_packet(self, self.devports[0], self.pkt_v6)
                verify_no_packet(self, self.exp_pkt, self.devports[1], timeout=1)

                if rule[1]:
                    print(" do not drop unmatched packet")
                    send_packet(self, self.devports[0], self.allow_pkt_v6)
                    verify_packet(self, self.exp_allow_pkt_v6, self.devports[1])

                print(" Remove ACL and send packet with no ACL rule")
                self.cleanlast()
                send_packet(self, self.devports[0], self.pkt_v6)
                verify_packet(self, self.exp_pkt_v6, self.devports[1])

        try:
            TestTraffic()
            self.startFrWrReplay(0, TestTraffic)   # WI or FI not testing
        finally:
            # remove the acl entry and table
            self.attribute_set(self.device, SWITCH_DEVICE_ATTR_INGRESS_ACL, 0)
            self.cleanlast()


###############################################################################

@group('acl2')
@group('mirror')
class AclMirrorTest(ApiHelper):
    # ERSPAN version constants synced with ptf
    ERSPAN_2 = 1
    ERSPAN_3 = 2

    def runTest(self):
        print()
        self.configure()

        self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_LEARNING, False)

        vlan_mbr0 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port0)
        vlan_mbr1 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port1)
        vlan_mbr2 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port2)
        vlan_mbr3 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port3)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.attribute_set(self.port2, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.attribute_set(self.port3, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)

        mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:11:11:11:11:11', destination_handle=self.port1)
        mac1 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:22:22:22:22:22', destination_handle=self.port0)

        try:
            self.IPv4MirrorAclTest()
            self.IPv6MirrorAclTest()
        finally:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.port2, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.port3, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.cleanup()

    def IPv4MirrorAclTest(self):
        '''
        This test tries to verify ipv4 acl entries
        We send a packet from port 0 and forward the packet to port 1,
        and mirror to port 2
        '''
        print("IPv4MirrorAclTest()")
        if (self.client.is_feature_enable(SWITCH_FEATURE_INGRESS_MIRROR_ACL) == 0):
            print("MirrorAcl feature not enabled, skipping")
            return
        print("Acl mirroring from 0 -> 1, 2")
        mirror = self.add_mirror(self.device,
            type=SWITCH_MIRROR_ATTR_TYPE_LOCAL,
            direction=SWITCH_MIRROR_ATTR_DIRECTION_INGRESS,
            egress_port_handle=self.port2)
        acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IP_MIRROR,
            bind_point_type=[acl_table_bp_switch],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_entry = self.add_acl_entry(self.device,
            eth_type=0x0800,
            eth_type_mask=0x7FFF, #thrift int16 limitation
            dst_ip='10.10.10.1',
            dst_ip_mask='255.255.255.255',
            action_ingress_mirror_handle=mirror,
            ingress_port_lag_handle=self.port0,
            table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry)
        table_name = SWITCH_DEVICE_ATTR_TABLE_IP_MIRROR_ACL
        pre_hw_table_count = get_hw_table_usage(self, table_name)
        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_INGRESS_ACL, acl_table)
        post_hw_table_count = get_hw_table_usage(self, table_name)
        self.assertEqual(post_hw_table_count - pre_hw_table_count, 1)
        pre_counter = self.object_counters_get(acl_entry)
        try:
            pkt = simple_tcp_packet(
                eth_dst='00:11:11:11:11:11',
                eth_src='00:22:22:22:22:22',
                ip_dst='10.10.10.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_ttl=64)
            send_packet(self, self.devports[0], pkt)
            verify_packets(self, pkt, [self.devports[1], self.devports[2]])
            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)

            # Make sure packet from different in_port (port 3) is not mirrored
            send_packet(self, self.devports[3], pkt)
            verify_packet(self, pkt, self.devports[1])
            verify_no_other_packets(self, timeout=2)
        finally:

            # remove the acl entry, table and mirror
            self.attribute_set(self.device, SWITCH_DEVICE_ATTR_INGRESS_ACL, 0)
            hw_table_count = get_hw_table_usage(self, table_name)
            self.assertEqual(hw_table_count, 0)
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

    def IPv6MirrorAclTest(self):
        '''
        This test tries to verify ipv6 acl entries
        We send a packet from port 0 and forward the packet to port 1,
        and mirror to port 2
        '''
        print("IPv6MirrorAclTest()")
        if (self.client.is_feature_enable(SWITCH_FEATURE_INGRESS_MIRROR_ACL) == 0):
            print("MirrorAcl feature not enabled, skipping")
            return
        print("Acl mirroring from 0 -> 1, 2")
        mirror = self.add_mirror(self.device,
            type=SWITCH_MIRROR_ATTR_TYPE_LOCAL,
            direction=SWITCH_MIRROR_ATTR_DIRECTION_INGRESS,
            egress_port_handle=self.port2)
        acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IP_MIRROR,
            bind_point_type=[acl_table_bp_switch],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_entry = self.add_acl_entry(self.device,
            dst_ip='4000::2',
            dst_ip_mask='FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF',
            action_ingress_mirror_handle=mirror,
            ingress_port_lag_handle=self.port0,
            table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry)
        table_name = SWITCH_DEVICE_ATTR_TABLE_IP_MIRROR_ACL
        pre_hw_table_count = get_hw_table_usage(self, table_name)
        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_INGRESS_ACL, acl_table)
        post_hw_table_count = get_hw_table_usage(self, table_name)
        self.assertEqual(post_hw_table_count - pre_hw_table_count, 1)
        pre_counter = self.object_counters_get(acl_entry)
        try:
            pkt = simple_tcpv6_packet(
                eth_dst='00:11:11:11:11:11',
                eth_src='00:22:22:22:22:22',
                ipv6_dst='4000::2',
                ipv6_src='2000::1',
                ipv6_hlim=64)
            send_packet(self, self.devports[0], pkt)
            verify_packets(self, pkt, [self.devports[1], self.devports[2]])
            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)

            # Make sure packet from different in_port (port 3) is not mirrored
            send_packet(self, self.devports[3], pkt)
            verify_packet(self, pkt, self.devports[1])
            verify_no_other_packets(self, timeout=2)
        finally:

            # remove the acl entry, table and mirror
            self.attribute_set(self.device, SWITCH_DEVICE_ATTR_INGRESS_ACL, 0)
            hw_table_count = get_hw_table_usage(self, table_name)
            self.assertEqual(hw_table_count, 0)
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

###############################################################################
@group('acl2')
class EgressAclTest(ApiHelper):
    def runTest(self):
        print("EgressAclTest()")
        self.configure()
        self.ConfigureL2()
        self.ConfigureL3()

        try:
            self.EgressMacAclTest()
            self.EgressIpv4AclTest()
            self.EgressIpv6AclTest()
        finally:
            self.CleanupL2()
            self.cleanup()
        return

    def EgressMacAclTest(self):
        print("EgressMacAclTest()")
        acl_table = self.add_acl_table(self.device,
                type=SWITCH_ACL_TABLE_ATTR_TYPE_MAC,
                bind_point_type=[acl_table_bp_switch],
                direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_EGRESS)
        self.dmac='00:44:44:44:44:44'
        self.smac='00:12:12:12:12:12'
        self.dmac2 = '00:33:33:33:33:33'
        mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address= self.dmac2,
                                 destination_handle=self.port2)
        dmac_acl_entry = self.add_acl_entry(self.device,
                dst_mac=self.dmac,
                dst_mac_mask='ff:ff:ff:ff:ff:ff',
                packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
                egress_port_lag_handle=self.port2,
                table_handle=acl_table)
        smac_acl_entry = self.add_acl_entry(self.device,
                src_mac=self.smac,
                src_mac_mask='ff:ff:ff:ff:ff:ff',
                packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
                egress_port_lag_handle=self.port2,
                table_handle=acl_table)
        self.client.object_counters_clear_all(dmac_acl_entry)
        self.client.object_counters_clear_all(smac_acl_entry)
        table_name = SWITCH_DEVICE_ATTR_TABLE_EGRESS_MAC_ACL
        pre_hw_table_count = get_hw_table_usage(self, table_name)
        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_EGRESS_ACL, acl_table)
        post_hw_table_count = get_hw_table_usage(self, table_name)
        self.assertEqual(post_hw_table_count - pre_hw_table_count, 2)

        try:
            print("Verifying DMAC ACL entry")
            pre_counter = self.object_counters_get(dmac_acl_entry)
            print("Sending vlan 10 packet matching dmac %s: port 0 -> port 2 (drop)"%(self.dmac))
            send_packet(self, self.devports[0], self.vlan10_pkt1)
            verify_no_other_packets(self, timeout=2)
            post_counter = self.object_counters_get(dmac_acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)
            self.assertEqual(post_counter[1].count - pre_counter[1].count, pkt_len+crc_len)
            self.client.object_counters_clear_all(dmac_acl_entry)
            counter = self.object_counters_get(dmac_acl_entry)
            self.assertEqual(counter[0].count, 0)

            pre_counter = self.object_counters_get(dmac_acl_entry)
            print("Sending vlan 10 packet port 1 -> port 2 (drop)")
            send_packet(self, self.devports[1], self.vlan10_pkt1)
            verify_no_other_packets(self, timeout=2)
            post_counter = self.object_counters_get(dmac_acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)
            self.assertEqual(post_counter[1].count - pre_counter[1].count, pkt_len+crc_len)
            self.client.object_counters_clear_all(dmac_acl_entry)
            counter = self.object_counters_get(dmac_acl_entry)
            self.assertEqual(counter[0].count, 0)

            # send flood packet and check for no drop other ports.
            print("Send flood packet and test for no drop")
            vlan10_pkt2 = simple_tcp_packet(
                eth_dst='00:44:44:44:44:48',
              eth_src='00:44:44:44:44:42',
              ip_dst='10.10.10.1',
              ip_src='192.168.0.1',
              ip_id=105,
              ip_ttl=64)

            pre_counter = self.object_counters_get(dmac_acl_entry)
            print("Sending vlan 10 packet port 0 -> port 2 (no drop)")
            send_packet(self, self.devports[0], vlan10_pkt2)
            #verify_no_packet(self, vlan10_pkt2, self.devports[2], timeout=2)
            verify_any_packet_on_ports_list(self, [vlan10_pkt2],
                 [[self.devports[1],self.devports[2]], [self.devports[6], self.devports[7]]])
            post_counter = self.object_counters_get(dmac_acl_entry)

            print("Verifying SMAC ACL entry")
            print("Sending vlan 10 packet matching smac %s port 0 -> port 2 (drop)"%(self.dmac2))
            pre_counter = self.object_counters_get(smac_acl_entry)
            smac_pkt=copy.deepcopy(self.vlan10_pkt1)
            smac_pkt['Ether'].src=self.smac
            smac_pkt['Ether'].dst=self.dmac2
            send_packet(self, self.devports[0], smac_pkt)
            verify_no_other_packets(self, timeout=2)
            post_counter = self.object_counters_get(smac_acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)
            self.assertEqual(post_counter[1].count - pre_counter[1].count, pkt_len+crc_len)
            self.client.object_counters_clear_all(smac_acl_entry)
            counter = self.object_counters_get(smac_acl_entry)
            self.assertEqual(counter[0].count, 0)
            pre_counter = self.object_counters_get(smac_acl_entry)
            print("Sending vlan 10 packet port 1 -> port 2 (drop)")
            send_packet(self, self.devports[1], smac_pkt)
            verify_no_other_packets(self, timeout=2)
            post_counter = self.object_counters_get(smac_acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)
            self.assertEqual(post_counter[1].count - pre_counter[1].count, pkt_len+crc_len)

        finally:
            # cleanup counter, acl_entry, acl_table
            self.attribute_set(self.device, SWITCH_DEVICE_ATTR_EGRESS_ACL, 0)
            hw_table_count = get_hw_table_usage(self, table_name)
            self.assertEqual(hw_table_count, 0)
            self.cleanlast()
            self.cleanlast()
        return

    def EgressIpv4AclTest(self):
        print("EgressIpv4AclTest()")
        acl_table = self.add_acl_table(self.device,
                type=SWITCH_ACL_TABLE_ATTR_TYPE_IPV4,
                bind_point_type=[acl_table_bp_switch],
                direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_EGRESS)
        dip_acl_entry = self.add_acl_entry(self.device,
                dst_ip='10.10.10.1',
                dst_ip_mask='255.255.255.255',
                ip_proto=6,
                ip_proto_mask=127,
                l4_dst_port=80,
                l4_dst_port_mask=32757,
                l4_src_port=1234,
                l4_src_port_mask=32757,
                packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
                egress_port_lag_handle=self.port2,
                table_handle=acl_table)
        sip_acl_entry = self.add_acl_entry(self.device,
                src_ip='12.12.12.12',
                src_ip_mask='255.255.255.255',
                ip_proto=6,
                ip_proto_mask=127,
                l4_dst_port=80,
                l4_dst_port_mask=32757,
                l4_src_port=1234,
                l4_src_port_mask=32757,
                packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
                egress_port_lag_handle=self.port2,
                table_handle=acl_table)
        self.client.object_counters_clear_all(dip_acl_entry)
        self.client.object_counters_clear_all(sip_acl_entry)
        table_name = SWITCH_DEVICE_ATTR_TABLE_EGRESS_IPV4_ACL
        pre_hw_table_count = get_hw_table_usage(self, table_name)
        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_EGRESS_ACL, acl_table)
        post_hw_table_count = get_hw_table_usage(self, table_name)
        self.assertEqual(post_hw_table_count - pre_hw_table_count, 2)

        try:
            print("Verifying DIP ACL entry")
            pre_counter = self.object_counters_get(dip_acl_entry)
            print("Sending vlan 10 packet matching dip 10.10.10.1 port 0 -> port 2 (drop)")
            send_packet(self, self.devports[0], self.vlan10_pkt1)
            verify_no_other_packets(self, timeout=2)
            post_counter = self.object_counters_get(dip_acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)
            self.assertEqual(post_counter[1].count - pre_counter[1].count, pkt_len+crc_len)
            self.client.object_counters_clear_all(dip_acl_entry)
            counter = self.object_counters_get(dip_acl_entry)
            self.assertEqual(counter[0].count, 0)

            pre_counter = self.object_counters_get(dip_acl_entry)
            print("Sending vlan 10 packet port 1 -> port 2 (drop)")
            send_packet(self, self.devports[1], self.vlan10_pkt1)
            verify_no_other_packets(self, timeout=2)
            post_counter = self.object_counters_get(dip_acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)
            self.assertEqual(post_counter[1].count - pre_counter[1].count, pkt_len+crc_len)
            self.client.object_counters_clear_all(dip_acl_entry)
            counter = self.object_counters_get(dip_acl_entry)
            self.assertEqual(counter[0].count, 0)

            # send acl-miss packet and test for no drop
            print("send acl_miss packet and test for no drop")
            vlan10_pkt2 = simple_tcp_packet(
                eth_dst='00:44:44:44:44:44',
              eth_src='00:44:44:44:44:42',
              ip_dst='50.10.10.2',
              ip_src='192.168.0.1',
              ip_id=105,
              ip_ttl=64)

            print("Sending vlan 10 packet port 0 -> port 2 (no drop)")
            send_packet(self, self.devports[0], vlan10_pkt2)
            verify_packets(self, vlan10_pkt2, [self.devports[2]])

            # send flood packet and check for no drop other ports.
            print("Send flood packet and test for no drop")
            vlan10_pkt2 = simple_tcp_packet(
                eth_dst='00:44:44:44:44:48',
              eth_src='00:44:44:44:44:42',
              ip_dst='10.10.10.1',
              ip_src='192.168.0.1',
              ip_id=105,
              ip_ttl=64)

            pre_counter = self.object_counters_get(dip_acl_entry)
            print("Sending vlan 10 packet port 0 -> port 2 (no drop)")
            send_packet(self, self.devports[0], vlan10_pkt2)
            verify_no_packet(self, vlan10_pkt2, self.devports[2], timeout=2)
            verify_any_packet_on_ports_list(self, [vlan10_pkt2],
                 [[self.devports[1]], [self.devports[6], self.devports[7]]])
            post_counter = self.object_counters_get(dip_acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)
            self.assertEqual(post_counter[1].count - pre_counter[1].count, pkt_len+crc_len)

            print("Verifying SIP ACL entry")
            print("Sending vlan 10 packet matching sip 192.168.0.1 port 0 -> port 2 (drop)")
            pre_counter = self.object_counters_get(sip_acl_entry)
            sip_pkt=copy.deepcopy(self.vlan10_pkt1)
            sip_pkt['IP'].src='12.12.12.12'
            sip_pkt['IP'].dst='11.11.11.11'
            send_packet(self, self.devports[0], sip_pkt)
            verify_no_other_packets(self, timeout=2)
            post_counter = self.object_counters_get(sip_acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)
            self.assertEqual(post_counter[1].count - pre_counter[1].count, pkt_len+crc_len)
            self.client.object_counters_clear_all(sip_acl_entry)
            counter = self.object_counters_get(sip_acl_entry)
            self.assertEqual(counter[0].count, 0)
            pre_counter = self.object_counters_get(sip_acl_entry)
            print("Sending vlan 10 packet port 1 -> port 2 (drop)")
            send_packet(self, self.devports[1], sip_pkt)
            verify_no_other_packets(self, timeout=2)
            post_counter = self.object_counters_get(sip_acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)
            self.assertEqual(post_counter[1].count - pre_counter[1].count, pkt_len+crc_len)

        finally:
            # cleanup counter, acl_entry, acl_table
            self.attribute_set(self.device, SWITCH_DEVICE_ATTR_EGRESS_ACL, 0)
            hw_table_count = get_hw_table_usage(self, table_name)
            self.assertEqual(hw_table_count, 0)
            self.cleanlast()
            self.cleanlast()
        return

    def EgressIpv6AclTest(self):
        print("EgressIpv6AclTest()")
        acl_table = self.add_acl_table(self.device,
                type=SWITCH_ACL_TABLE_ATTR_TYPE_IPV6,
                bind_point_type=[acl_table_bp_switch],
                direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_EGRESS)
        self.ipv6_dip = '1234:5678:9abc:def0:4422:1133:5577:0000'
        self.ipv6_sip = '1111:2222:3333:4444:6666:7777:8888:0001'
        self.ipv6_mask='FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF'
        dip_acl_entry = self.add_acl_entry(self.device,
                dst_ip=self.ipv6_dip,
                dst_ip_mask=self.ipv6_mask,
                ip_proto=6,
                ip_proto_mask=127,
                l4_dst_port=80,
                l4_dst_port_mask=32757,
                l4_src_port=1234,
                l4_src_port_mask=32757,
                packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
                egress_port_lag_handle=self.port2,
                table_handle=acl_table)
        sip_acl_entry = self.add_acl_entry(self.device,
                src_ip=self.ipv6_sip,
                src_ip_mask=self.ipv6_mask,
                ip_proto=6,
                ip_proto_mask=127,
                l4_dst_port=80,
                l4_dst_port_mask=32757,
                l4_src_port=1234,
                l4_src_port_mask=32757,
                packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
                egress_port_lag_handle=self.port2,
                table_handle=acl_table)
        self.client.object_counters_clear_all(dip_acl_entry)
        self.client.object_counters_clear_all(sip_acl_entry)
        table_name = SWITCH_DEVICE_ATTR_TABLE_EGRESS_IPV6_ACL
        pre_hw_table_count = get_hw_table_usage(self, table_name)
        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_INGRESS_ACL, acl_table)
        post_hw_table_count = get_hw_table_usage(self, table_name)
        self.assertEqual(post_hw_table_count - pre_hw_table_count, 2)

        try:
            print("Verifying IPV6 DIP ACL entry")
            pre_counter = self.object_counters_get(dip_acl_entry)
            print("Sending vlan 10 packet matching dip %s port 0 -> port 2 (drop)"%(self.ipv6_dip))
            self.pkt_dip_match = simple_tcpv6_packet(
                eth_dst='00:44:44:44:44:44',
                eth_src='00:44:44:44:44:42',
                ipv6_src='1000::1',
                ipv6_dst=self.ipv6_dip)

            self.pkt_sip_match = simple_tcpv6_packet(
                eth_dst='00:44:44:44:44:44',
                eth_src='00:44:44:44:44:42',
                ipv6_src=self.ipv6_sip)
            send_packet(self, self.devports[0], self.pkt_dip_match)
            verify_no_other_packets(self, timeout=2)
            post_counter = self.object_counters_get(dip_acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)
            self.assertEqual(post_counter[1].count - pre_counter[1].count, pkt_len+crc_len)
            self.client.object_counters_clear_all(dip_acl_entry)
            counter = self.object_counters_get(dip_acl_entry)
            self.assertEqual(counter[0].count, 0)

            pre_counter = self.object_counters_get(dip_acl_entry)
            print("Sending vlan 10 packet port 1 -> port 2 (drop)")
            send_packet(self, self.devports[1], self.pkt_dip_match)
            verify_no_other_packets(self, timeout=2)
            post_counter = self.object_counters_get(dip_acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)
            self.assertEqual(post_counter[1].count - pre_counter[1].count, pkt_len+crc_len)
            self.client.object_counters_clear_all(dip_acl_entry)
            counter = self.object_counters_get(dip_acl_entry)
            self.assertEqual(counter[0].count, 0)

            # send acl-miss packet and test for no drop
            print("send acl_miss packet and test for no drop")
            pkt_dip_fwd = copy.deepcopy(self.pkt_dip_match)
            pkt_dip_fwd['IPv6'].dst='5555:6666::1'
            print("Sending vlan 10 packet port 0 -> port 2 (no drop)")
            send_packet(self, self.devports[0], pkt_dip_fwd)
            verify_packets(self, pkt_dip_fwd, [self.devports[2]])

            # send flood packet and check for no drop other ports.
            print("Send flood packet and test for no drop")
            pkt_dip_flood = copy.deepcopy(self.pkt_dip_match)
            pkt_dip_flood['Ether'].dst='00:44:44:44:44:48'
            pre_counter = self.object_counters_get(dip_acl_entry)
            print("Sending vlan 10 packet port 0 -> port 2 (no drop)")
            send_packet(self, self.devports[0], pkt_dip_flood)
            verify_no_packet(self, pkt_dip_flood, self.devports[2], timeout=2)
            verify_any_packet_on_ports_list(self, [pkt_dip_flood],
                 [[self.devports[1]], [self.devports[6], self.devports[7]]])
            post_counter = self.object_counters_get(dip_acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)

            print("Verifying SIP ACL entry")
            print("Sending vlan 10 packet matching sip %s port 0 -> port 2 (drop)"%(self.ipv6_sip))
            pre_counter = self.object_counters_get(sip_acl_entry)
            send_packet(self, self.devports[0], self.pkt_sip_match)
            verify_no_other_packets(self, timeout=2)
            post_counter = self.object_counters_get(sip_acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)
            self.assertEqual(post_counter[1].count - pre_counter[1].count, pkt_len+crc_len)
            self.client.object_counters_clear_all(sip_acl_entry)
            counter = self.object_counters_get(sip_acl_entry)
            self.assertEqual(counter[0].count, 0)
            pre_counter = self.object_counters_get(sip_acl_entry)
            print("Sending vlan 10 packet port 1 -> port 2 (drop)")
            send_packet(self, self.devports[1], self.pkt_sip_match)
            verify_no_other_packets(self, timeout=2)
            post_counter = self.object_counters_get(sip_acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)
            self.assertEqual(post_counter[1].count - pre_counter[1].count, pkt_len+crc_len)

        finally:
            # cleanup counter, acl_entry, acl_table
            self.attribute_set(self.device, SWITCH_DEVICE_ATTR_INGRESS_ACL, 0)
            hw_table_count = get_hw_table_usage(self, table_name)
            self.assertEqual(hw_table_count, 0)
            self.cleanlast()
            self.cleanlast()
        return

###############################################################################

@group('acl2')
class AclRedirectTest(ApiHelper):
    def runTest(self):
        print('AclRedirectTest')
        self.configure()
        self.ConfigureL2()
        self.ConfigureL3()
        try:
            self.IPv4AclRedirectNexthopTest()
            self.IPv6AclRedirectNexthopTest()
            #TODO: to be enabled after mac label code.
            #self.MacAclRedirectL2PortTest()
            self.IPv4AclRedirectL2PortTest()
            self.IPv4AclRedirectL3PortTest()
            self.IPv6AclRedirectL2PortTest()
            self.IPv6AclRedirectL3PortTest()
        finally:
            self.CleanupL2()
            self.cleanup()
        return

    def IPv4AclRedirectNexthopTest(self):
        print("IPv4AclRedirectNexthopTest()")

        acl_table = self.add_acl_table(self.device,
                type=SWITCH_ACL_TABLE_ATTR_TYPE_IP,
                bind_point_type=[acl_table_bp_switch],
                direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)

        # Attach acl_entry to rif port8
        acl_entry = self.add_acl_entry(self.device,
                dst_ip='10.10.10.1',
                dst_ip_mask='255.255.255.255',
                redirect=self.nhop2,
                ingress_port_lag_handle=self.port8,
                table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry)
        self.stack.pop()

        table_name = SWITCH_DEVICE_ATTR_TABLE_IP_ACL
        pre_hw_table_count = get_hw_table_usage(self, table_name)
        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_INGRESS_ACL, acl_table)
        post_hw_table_count = get_hw_table_usage(self, table_name)
        self.assertEqual(post_hw_table_count - pre_hw_table_count, 1)

        try:
            self.l3_exp_redirect_pkt1 = simple_tcp_packet(
            eth_dst='00:11:22:33:44:56',
            eth_src='00:77:66:55:44:33',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63)

            pre_counter = self.object_counters_get(acl_entry)
            print("Sending Ipv4 packet packet rif port 8 -> redirect nhop rif:port10")
            send_packet(self, self.devports[8], self.l3_pkt1)
            verify_packet(self, self.l3_exp_redirect_pkt1, self.devports[10])
            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)

            #test acl-miss for unmatch packet
            print("Sending acl-miss IPv4 packet port 8 -> lag 2;port11/port12 (no redirect)")
            send_packet(self, self.devports[8], self.l3_lag_pkt1)
            verify_any_packet_any_port(self, [self.l3_exp_lag_pkt1, self.l3_exp_lag_pkt1],
                                       [self.devports[11], self.devports[12]])

            # send packet from different port (lag2) and verify packet not redirected
            print("Sending packet lag2(port11) -> port 9 (no redirect to port 10)")
            send_packet(self, self.devports[11], self.l3_pkt1)
            verify_packet(self, self.l3_exp_pkt1, self.devports[9])

            #unset acl from bind_point and test for no-redirect
            print("unset re-direct acl from rif port 8 and check for no-redirect")
            # delete acl entry
            self.client.object_delete(acl_entry)

            print("Sending IPv4 packet rif port 8 -> rif port 9(no re-direct to port10)")
            send_packet(self, self.devports[8], self.l3_pkt1)
            verify_packet(self, self.l3_exp_pkt1, self.devports[9])

            acl_entry = self.add_acl_entry(self.device,
                    dst_ip='10.10.10.1',
                    dst_ip_mask='255.255.255.255',
                    redirect=self.nhop3,
                    ingress_port_lag_handle=self.port8,
                    table_handle=acl_table)
            self.client.object_counters_clear_all(acl_entry)
            self.stack.pop()

            #acl-redirect to  next-hop, egress_port:lag
            self.l3_exp_redirect_pkt2 = simple_tcp_packet(
            eth_dst='00:11:22:33:44:57',
            eth_src='00:77:66:55:44:33',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63)

            self.attribute_set(acl_entry, SWITCH_ACL_ENTRY_ATTR_REDIRECT, self.nhop3)
            pre_counter = self.object_counters_get(acl_entry)
            print("Sending Ipv4 packet packet rif port 8 -> redirect nhop rif:lag2:port11/port12")
            send_packet(self, self.devports[8], self.l3_pkt1)
            verify_any_packet_any_port(self, [self.l3_exp_redirect_pkt2, self.l3_exp_redirect_pkt2],
                    [self.devports[11], self.devports[12]])
            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)

            #acl-redirect to svi next-hop, egress_port:port
            # Create SVI on vlan 20
            vlan20_member0 = self.add_vlan_member(self.device, vlan_handle=self.vlan20, member_handle=self.port13)
            self.attribute_set(self.port13, SWITCH_PORT_ATTR_PORT_VLAN_ID, 20)
            vlan20_rif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_VLAN,
                         vlan_handle=self.vlan20, vrf_handle=self.vrf10, src_mac=self.rmac)

            mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan20,
                        mac_address='00:55:55:55:66:66', destination_handle=self.port13)
            vlan20_nhop1 = self.add_nexthop(self.device, handle=vlan20_rif, dest_ip='20.10.10.8')
            vlan20_neighbor1 = self.add_neighbor(self.device, mac_address='00:55:55:55:66:66', handle=vlan20_rif, dest_ip='20.10.10.8')
            self.l3_exp_redirect_pkt3 = simple_tcp_packet(
                               eth_dst='00:55:55:55:66:66',
                               eth_src='00:77:66:55:44:33',
                               ip_dst='10.10.10.1',
                               ip_src='192.168.0.1',
                               ip_id=105,
                               ip_ttl=63)

            self.attribute_set(acl_entry, SWITCH_ACL_ENTRY_ATTR_REDIRECT, vlan20_nhop1)
            pre_counter = self.object_counters_get(acl_entry)
            print("Sending Ipv4 packet packet rif port 8 -> redirect svi_vlan20:port13")
            send_packet(self, self.devports[8], self.l3_pkt1)
            verify_packet(self, self.l3_exp_redirect_pkt3, self.devports[13])
            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)

            #acl-redirect to svi next-hop, egress_port:lag
            self.attribute_set(acl_entry, SWITCH_ACL_ENTRY_ATTR_REDIRECT,
                     self.nhop4)
            self.l3_exp_redirect_pkt4 = simple_tcp_packet(
            eth_dst='00:55:55:55:55:55',
            eth_src='00:77:66:55:44:33',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63)
            print("Sending Ipv4 packet packet rif port 8 -> vlan10_rif:lag1:port6/port7(redirect)")
            pre_counter = self.object_counters_get(acl_entry)
            send_packet(self, self.devports[8], self.l3_pkt1)
            verify_any_packet_any_port(self, [self.l3_exp_redirect_pkt4, self.l3_exp_redirect_pkt4],
                    [self.devports[6], self.devports[7]])
            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)

            #unset acl from bind_point and test for no-redirect
            print("unset re-direct acl from rif port 8 and check for no-redirect")
            # delete acl entry
            self.client.object_delete(acl_entry)
            print("Sending IPv4 packet rif port 8 -> rif port 9(no re-direct to port10)")
            send_packet(self, self.devports[8], self.l3_pkt1)
            verify_packet(self, self.l3_exp_pkt1, self.devports[9])
        finally:
            # cleanup vlan20_neighbor1,vlan20_nhop1,mac0, vlan20_rif, vlan20_member0
            self.attribute_set(self.port13, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            # cleanup acl_table, acl_entry is already deleted
            self.attribute_set(self.device, SWITCH_DEVICE_ATTR_INGRESS_ACL, 0)
            hw_table_count = get_hw_table_usage(self, table_name)
            self.assertEqual(hw_table_count, 0)
            self.cleanlast()
        return

    def IPv6AclRedirectNexthopTest(self):
        print("IPv6AclRedirectNexthopTest()")

        nhop1 = self.add_nexthop(self.device, handle=self.rif2, dest_ip='2222:3333:4444::1')
        neighbor1 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:56', handle=self.rif2, dest_ip='2222:3333:4444::1')

        # create acl_table, acl_entry
        acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IP,
            bind_point_type=[acl_table_bp_switch],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_entry = self.add_acl_entry(self.device,
            dst_ip='1234:5678:9abc:def0:4422:1133:5577:99aa',
            dst_ip_mask='FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF',
            ingress_port_lag_handle=self.port8,
            redirect=nhop1,
            table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry)
        self.stack.pop()

        table_name = SWITCH_DEVICE_ATTR_TABLE_IP_ACL
        pre_hw_table_count = get_hw_table_usage(self, table_name)
        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_INGRESS_ACL, acl_table)
        post_hw_table_count = get_hw_table_usage(self, table_name)
        self.assertEqual(post_hw_table_count - pre_hw_table_count, 1)

        # Attach acl_table to rif port8
        pre_counter = self.object_counters_get(acl_entry)
        try:
            #case-1: acl-redirect to nhop with egress_port:port10
            l3_exp_redirect_pkt1_v6 = simple_tcpv6_packet(
                eth_dst='00:11:22:33:44:56',
               eth_src='00:77:66:55:44:33',
               ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
               ipv6_src='2000::1',
               ipv6_hlim=63)

            pre_counter = self.object_counters_get(acl_entry)
            print("Sending Ipv6 packet packet rif port 8 -> redirect nhop rif:port10")
            send_packet(self, self.devports[8], self.l3_pkt1_v6)
            verify_packet(self, l3_exp_redirect_pkt1_v6, self.devports[10])
            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)

            # send packet from different port (lag2) and verify packet not redirected
            print("Sending packet lag2(port11) -> port 9 (no redirect to port 10)")
            send_packet(self, self.devports[11], self.l3_pkt1_v6)
            verify_packet(self, self.l3_exp_pkt1_v6, self.devports[9])

            #test acl-miss for unmatch packet
            route0 = self.add_route(self.device, ip_prefix='2234:5678:9abc:def0:4422:1133:5577:99aa', vrf_handle=self.vrf10,
                                    nexthop_handle=self.nhop1)
            print("Sending acl-miss IPv6 packet port 8 -> nhop rif:port9 (no redirect)")
            l3_pkt2_v6 = simple_tcpv6_packet(
                eth_dst='00:77:66:55:44:33',
              eth_src='00:22:22:22:22:22',
              ipv6_dst='2234:5678:9abc:def0:4422:1133:5577:99aa',
              ipv6_src='2000::1',
              ipv6_hlim=64)

            l3_exp_pkt2_v6 = simple_tcpv6_packet(
                eth_dst='00:11:22:33:44:55',
              eth_src='00:77:66:55:44:33',
              ipv6_dst='2234:5678:9abc:def0:4422:1133:5577:99aa',
              ipv6_src='2000::1',
              ipv6_hlim=63)
            send_packet(self, self.devports[8], l3_pkt2_v6)
            verify_packet(self, l3_exp_pkt2_v6, self.devports[9])
            # cleanup route0
            self.cleanlast()

            #unset acl from bind_point and test for no-redirect
            print("unset re-direct acl from rif port 8 and check for no-redirect")
            # delete acl entry
            self.client.object_delete(acl_entry)
            print("Sending IPv6 packet rif port 8 -> rif port 9(no re-direct to port10)")
            send_packet(self, self.devports[8], self.l3_pkt1_v6)
            verify_packet(self, self.l3_exp_pkt1_v6, self.devports[9])

            #case-2: acl-redirect to  next-hop, egress_port:lag
            l3_exp_redirect_pkt3 = simple_tcpv6_packet(
                eth_dst='00:11:22:33:44:57',
               eth_src='00:77:66:55:44:33',
               ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
               ipv6_src='2000::1',
               ipv6_hlim=63)

            acl_entry = self.add_acl_entry(self.device,
                    dst_ip='1234:5678:9abc:def0:4422:1133:5577:99aa',
                    dst_ip_mask='FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF',
                    redirect=self.nhop3,
                    ingress_port_lag_handle=self.port8,
                    table_handle=acl_table)
            self.stack.pop()

            pre_counter = self.object_counters_get(acl_entry)
            print("Sending Ipv6 packet packet rif port 8 -> redirect nhop rif:lag2:port11/port12")
            send_packet(self, self.devports[8], self.l3_pkt1_v6)
            verify_any_packet_any_port(self, [l3_exp_redirect_pkt3, l3_exp_redirect_pkt3],
                    [self.devports[11], self.devports[12]])
            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)

            #case-3: acl-redirect to svi next-hop, egress_port:port
            # Create SVI on vlan 20
            vlan20_member0 = self.add_vlan_member(self.device, vlan_handle=self.vlan20, member_handle=self.port13)
            self.attribute_set(self.port13, SWITCH_PORT_ATTR_PORT_VLAN_ID, 20)
            vlan20_rif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_VLAN,
                         vlan_handle=self.vlan20, vrf_handle=self.vrf10, src_mac=self.rmac)

            mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan20,
                        mac_address='00:55:55:55:66:66', destination_handle=self.port13)
            vlan20_nhop1 = self.add_nexthop(self.device, handle=vlan20_rif, dest_ip='1111:3333:4444::1')
            vlan20_neighbor1 = self.add_neighbor(self.device, mac_address='00:55:55:55:66:66', handle=vlan20_rif,
                               dest_ip='1111:3333:4444::1')
            l3_exp_redirect_pkt4 = simple_tcpv6_packet(
                eth_dst='00:55:55:55:66:66',
               eth_src='00:77:66:55:44:33',
               ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
               ipv6_src='2000::1',
               ipv6_hlim=63)
            self.attribute_set(acl_entry, SWITCH_ACL_ENTRY_ATTR_REDIRECT,
                     vlan20_nhop1)
            pre_counter = self.object_counters_get(acl_entry)
            print("Sending Ipv6 packet packet rif port 8 -> redirect svi_vlan20:port13")
            send_packet(self, self.devports[8], self.l3_pkt1_v6)
            verify_packet(self, l3_exp_redirect_pkt4, self.devports[13])
            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)

            #case-4: acl-redirect to svi next-hop, egress_port:lag
            self.attribute_set(acl_entry, SWITCH_ACL_ENTRY_ATTR_REDIRECT,
                     self.nhop4)
            l3_exp_redirect_pkt5 = simple_tcpv6_packet(
                eth_dst='00:55:55:55:55:55',
               eth_src='00:77:66:55:44:33',
               ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
               ipv6_src='2000::1',
               ipv6_hlim=63)
            print("Sending Ipv6 packet packet rif port 8 -> vlan10_rif:lag1:port6/port7(redirect)")
            pre_counter = self.object_counters_get(acl_entry)
            send_packet(self, self.devports[8], self.l3_pkt1_v6)
            verify_any_packet_any_port(self, [l3_exp_redirect_pkt5, l3_exp_redirect_pkt5],
                    [self.devports[6], self.devports[7]])
            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)


            #case-5: unset acl from bind_point and test for no-redirect
            print("unset re-direct acl from rif port 8 and check for no-redirect")
            # delete acl entry
            self.client.object_delete(acl_entry)
            print("Sending IPv6 packet rif port 8 -> rif port 9(no re-direct to port10)")
            send_packet(self, self.devports[8], self.l3_pkt1_v6)
            verify_packet(self, self.l3_exp_pkt1_v6, self.devports[9])

        finally:
            # cleanup vlan20_neighbor1,vlan20_nhop1,mac0, vlan20_rif, vlan20_member0
            self.attribute_set(self.port13, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            # cleanup acl_table, acl_entry is already deleted
            self.attribute_set(self.device, SWITCH_DEVICE_ATTR_INGRESS_ACL, 0)
            hw_table_count = get_hw_table_usage(self, table_name)
            self.assertEqual(hw_table_count, 0)
            self.cleanlast()
            # cleanup neighbor1, nhop1
            self.cleanlast()
            self.cleanlast()
        return

    def MacAclRedirectL2PortTest(self):
        print("MacAclRedirectL2PortTest()")

        try:
            #case-1: before apply acl-redirect, test for regular forwarding.
            vlan10_l2_pkt1 = simple_eth_packet(
                eth_dst='00:44:44:44:44:44',
               eth_src='00:22:22:22:22:22')
            print("Sending vlan 10 packet port 0 -> port 2")
            send_packet(self, self.devports[0], vlan10_l2_pkt1)
            verify_packets(self, vlan10_l2_pkt1, [self.devports[2]])

            # Attach acl_table to vlan10, port0
            acl_table = self.add_acl_table(self.device,
              type=SWITCH_ACL_TABLE_ATTR_TYPE_IP,
              bind_point_type=[acl_table_bp_switch],
              direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
            acl_entry = self.add_acl_entry(self.device,
              src_mac='00:22:22:22:22:22',
              src_mac_mask='ff:ff:ff:ff:ff:ff',
              ingress_port_lag_handle=self.port0,
              redirect=self.port1,
              table_handle=acl_table)
            self.client.object_counters_clear_all(acl_entry)
            table_name = SWITCH_DEVICE_ATTR_TABLE_IP_ACL
            pre_hw_table_count = get_hw_table_usage(self, table_name)
            self.attribute_set(self.device, SWITCH_DEVICE_ATTR_INGRESS_ACL, acl_table)
            post_hw_table_count = get_hw_table_usage(self, table_name)
            self.assertEqual(post_hw_table_count - pre_hw_table_count, 1)


            #case-1: check for redirect to port1
            print("Sending vlan 10 packet port 0 -> port 2 (action: redirect port 1)")
            pre_counter = self.object_counters_get(acl_entry)
            send_packet(self, self.devports[0], vlan10_l2_pkt1)
            verify_packet(self, vlan10_l2_pkt1, self.devports[1])
            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)

            #case-2: send acl-miss packet and test for no redirect
            print("send acl_miss packet and test for no redirect")
            vlan10_l2_pkt2 = simple_eth_packet(
                eth_dst='00:44:44:44:44:44',
               eth_src='00:44:44:44:44:42')

            print("Sending vlan 10 packet port 0 -> port 2 (no redirect)")
            send_packet(self, self.devports[0], vlan10_l2_pkt2)
            verify_packet(self, vlan10_l2_pkt2, self.devports[2])

            #case-3: set redirect port as lag
            self.attribute_set(acl_entry, SWITCH_ACL_ENTRY_ATTR_REDIRECT,
                     self.lag1)
            print("Sending vlan 10 packet port 0 -> port 2 (action: redirect lag1:port6/port7)")
            pre_counter = self.object_counters_get(acl_entry)
            send_packet(self, self.devports[0], vlan10_l2_pkt1)
            verify_any_packet_any_port(self, [vlan10_l2_pkt1, vlan10_l2_pkt1],
                    [self.devports[6], self.devports[7]])
            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)


            #case-4: send packet from vlan10, port1 and verify packet not redirected.
            print("Sending vlan 10 packet port 1 -> port 2 (no redirect)")
            send_packet(self, self.devports[1], vlan10_l2_pkt1)
            verify_packet(self, vlan10_l2_pkt1, self.devports[2])

            #case-5: unset acl and test for no redirect.
            # delete acl entry
            self.cleanlast()
            print("Sending vlan 10 packet port 1 -> port 2 (no redirect)")
            send_packet(self, self.devports[0], vlan10_l2_pkt1)
            verify_packet(self, vlan10_l2_pkt1, self.devports[2])

        finally:
            # cleanup acl_table
            self.attribute_set(self.device, SWITCH_DEVICE_ATTR_INGRESS_ACL, 0)
            hw_table_count = get_hw_table_usage(self, table_name)
            self.assertEqual(hw_table_count, 0)
            self.cleanlast()
        return


    def IPv4AclRedirectL2PortTest(self):
        print("IPv4AclRedirectPortTest()")

        # Attach acl_table to vlan10, port0
        acl_table = self.add_acl_table(self.device,
                type = SWITCH_ACL_TABLE_ATTR_TYPE_IP,
                bind_point_type=[acl_table_bp_switch],
                direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_entry = self.add_acl_entry(self.device,
                dst_ip='10.10.10.1',
                dst_ip_mask='255.255.255.255',
                ingress_port_lag_handle=self.port0,
                redirect=self.port1,
                table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry)
        table_name = SWITCH_DEVICE_ATTR_TABLE_IP_ACL
        pre_hw_table_count = get_hw_table_usage(self, table_name)
        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_INGRESS_ACL, acl_table)
        post_hw_table_count = get_hw_table_usage(self, table_name)
        self.assertEqual(post_hw_table_count - pre_hw_table_count, 1)

        try:
            #case-1: check for redirect to port1
            print("Sending vlan 10 packet port 0 -> port 2 (action: redirect port 1)")
            pre_counter = self.object_counters_get(acl_entry)
            send_packet(self, self.devports[0], self.vlan10_pkt1)
            verify_packet(self, self.vlan10_pkt1, self.devports[1])
            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)


            #case-2: send acl-miss packet and test for no redirect
            print("send acl_miss packet and test for no redirect")
            vlan10_pkt2 = simple_tcp_packet(
                eth_dst='00:44:44:44:44:44',
              eth_src='00:44:44:44:44:42',
              ip_dst='50.10.10.2',
              ip_src='192.168.0.1',
              ip_id=105,
              ip_ttl=64)

            print("Sending vlan 10 packet port 0 -> port 2 (no redirect)")
            send_packet(self, self.devports[0], vlan10_pkt2)
            verify_packet(self, vlan10_pkt2, self.devports[2])

            #case-3: set redirect port as lag
            self.attribute_set(acl_entry, SWITCH_ACL_ENTRY_ATTR_REDIRECT,
                     self.lag1)
            print("Sending vlan 10 packet port 0 -> port 2 (action: redirect lag1:port6/port7)")
            pre_counter = self.object_counters_get(acl_entry)
            send_packet(self, self.devports[0], self.vlan10_pkt1)
            verify_any_packet_any_port(self, [self.vlan10_pkt1, self.vlan10_pkt1],
                    [self.devports[6], self.devports[7]])
            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)

            #case-4: send packet from vlan10, port1 and verify packet not redirected
            print("Sending vlan 10 packet port 1 -> port 2 (no redirect)")
            send_packet(self, self.devports[1], self.vlan10_pkt1)
            verify_packet(self, self.vlan10_pkt1, self.devports[2])

            #case-5: unset acl and test for no redirect.
            # delete acl entry
            self.cleanlast()
            print("Sending vlan 10 packet port 1 -> port 2 (no redirect)")
            send_packet(self, self.devports[0], self.vlan10_pkt1)
            verify_packet(self, self.vlan10_pkt1, self.devports[2])

        finally:
            # cleanup acl_table
            self.attribute_set(self.device, SWITCH_DEVICE_ATTR_INGRESS_ACL, 0)
            hw_table_count = get_hw_table_usage(self, table_name)
            self.assertEqual(hw_table_count, 0)
            self.cleanlast()
        return

    def IPv4AclRedirectL3PortTest(self):
        print("IPv4AclRedirectL3PortTest()")

        # Attach acl_table to rif port8
        acl_table = self.add_acl_table(self.device,
                type=SWITCH_ACL_TABLE_ATTR_TYPE_IP,
                bind_point_type=[acl_table_bp_switch],
                direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_entry = self.add_acl_entry(self.device,
                dst_ip='10.10.10.1',
                dst_ip_mask='255.255.255.255',
                ingress_port_lag_handle=self.port8,
                redirect=self.port10,
                table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry)
        table_name = SWITCH_DEVICE_ATTR_TABLE_IP_ACL
        pre_hw_table_count = get_hw_table_usage(self, table_name)
        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_INGRESS_ACL, acl_table)
        post_hw_table_count = get_hw_table_usage(self, table_name)
        self.assertEqual(post_hw_table_count - pre_hw_table_count, 1)

        try:
            #case-1: acl redirect to port10
            pre_counter = self.object_counters_get(acl_entry)
            print("Sending Ipv4 packet packet rif port 8 -> port 9 (redirect to port10)")
            send_packet(self, self.devports[8], self.l3_pkt1)
            verify_packet(self, self.l3_pkt1, self.devports[10])
            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)

            #case-2: send acl_miss packet from port 8 and test for no redirect
            route0 = self.add_route(self.device, ip_prefix='60.10.10.1', vrf_handle=self.vrf10,
                            nexthop_handle=self.nhop1)
            l3_pkt2 = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
               eth_src='00:22:22:22:22:22',
               ip_dst='60.10.10.1',
               ip_src='192.168.0.1',
               ip_id=105,
               ip_ttl=64)

            l3_exp_pkt2 = simple_tcp_packet(
                eth_dst='00:11:22:33:44:55',
               eth_src='00:77:66:55:44:33',
               ip_dst='60.10.10.1',
               ip_src='192.168.0.1',
               ip_id=105,
               ip_ttl=63)

            print("send acl_miss packet and test for no redirect")
            print("Sending Ipv4 packet rif port 8 -> port 9 (no redirect)")
            send_packet(self, self.devports[8], l3_pkt2)
            verify_packet(self, l3_exp_pkt2, self.devports[9])
            self.cleanlast()

            #case3: send packet from other port and test for no redirect.
            print("Sending IPv4 packet rif port 10 -> rif port 9(no redirect)")
            send_packet(self, self.devports[10], self.l3_pkt1)
            verify_packet(self, self.l3_exp_pkt1, self.devports[9])
        finally:
            # cleanup acl_entry, acl_table
            self.attribute_set(self.device, SWITCH_DEVICE_ATTR_INGRESS_ACL, 0)
            hw_table_count = get_hw_table_usage(self, table_name)
            self.assertEqual(hw_table_count, 0)
            self.cleanlast()
            self.cleanlast()
        return

    def IPv6AclRedirectL2PortTest(self):
        print("IPv6AclRedirectL2PortTest()")

        try:
            v6_l3_pkt1 = simple_tcpv6_packet(
                eth_dst='00:44:44:44:44:44',
              eth_src='00:44:44:44:44:42',
              ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
              ipv6_src='2000::1',
              ipv6_hlim=64)

            #case-1: before applying acl-redirect, check for basic forwarding.
            print("Sending vlan 10 packet port 0 -> port 2")
            send_packet(self, self.devports[0], v6_l3_pkt1)
            verify_packet(self, v6_l3_pkt1, self.devports[2])

            # Attach acl_table to vlan10, port0
            acl_table = self.add_acl_table(self.device,
                type=SWITCH_ACL_TABLE_ATTR_TYPE_IP,
                bind_point_type=[acl_table_bp_switch],
                direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)

            acl_entry = self.add_acl_entry(self.device,
                dst_ip='1234:5678:9abc:def0:4422:1133:5577:99aa',
                dst_ip_mask='FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF',
                ingress_port_lag_handle=self.port0,
                redirect=self.port1,
                table_handle=acl_table)
            self.client.object_counters_clear_all(acl_entry)
            table_name = SWITCH_DEVICE_ATTR_TABLE_IP_ACL
            pre_hw_table_count = get_hw_table_usage(self, table_name)
            self.attribute_set(self.device, SWITCH_DEVICE_ATTR_INGRESS_ACL, acl_table)
            post_hw_table_count = get_hw_table_usage(self, table_name)
            self.assertEqual(post_hw_table_count - pre_hw_table_count, 1)

            #case-1: check for redirect to port1
            print("Sending vlan 10 packet port 0 -> port 2 (action: redirect port 1)")
            pre_counter = self.object_counters_get(acl_entry)
            send_packet(self, self.devports[0], v6_l3_pkt1)
            verify_packet(self, v6_l3_pkt1, self.devports[1])
            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)


            #case-2: send acl-miss packet and test for no drop
            print("send acl_miss packet and test for no redirect")
            v6_l3_pkt2 = simple_tcpv6_packet(
                eth_dst='00:44:44:44:44:44',
              eth_src='00:44:44:44:44:42',
              ipv6_dst='2234:5678:9abc:def0:4422:1133:5577:99aa',
              ipv6_src='2000::1',
              ipv6_hlim=64)

            print("Sending vlan 10 packet port 0 -> port 2 (no redirect)")
            send_packet(self, self.devports[0], v6_l3_pkt2)
            verify_packet(self, v6_l3_pkt2, self.devports[2])

            #case-3: set redirect port as lag
            self.attribute_set(acl_entry, SWITCH_ACL_ENTRY_ATTR_REDIRECT,
                     self.lag1)
            print("Sending vlan 10 packet port 0 -> port 2 (action: redirect lag1:port6/port7)")
            pre_counter = self.object_counters_get(acl_entry)
            send_packet(self, self.devports[0], v6_l3_pkt1)
            verify_any_packet_any_port(self, [v6_l3_pkt1, v6_l3_pkt1],
                    [self.devports[6], self.devports[7]])
            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)


            #case-4: send packet from vlan10, different port1 and verify packet not redirected
            print("Sending vlan 10 packet port 1 -> port 2 (no redirect)")
            send_packet(self, self.devports[1], v6_l3_pkt1)
            verify_packet(self, v6_l3_pkt1, self.devports[2])

            #case-5: unset acl and test for no redirect.
            # delete acl entry
            self.cleanlast()
            print("Sending vlan 10 packet port 1 -> port 2 (no redirect)")
            send_packet(self, self.devports[0], v6_l3_pkt1)
            verify_packet(self, v6_l3_pkt1, self.devports[2])

        finally:
            # cleanup acl_table
            self.attribute_set(self.device, SWITCH_DEVICE_ATTR_INGRESS_ACL, 0)
            hw_table_count = get_hw_table_usage(self, table_name)
            self.assertEqual(hw_table_count, 0)
            self.cleanlast()
        return

    def IPv6AclRedirectL3PortTest(self):
        print("IPv6AclRedirectL3PortTest()")

        nhop1 = self.add_nexthop(self.device, handle=self.rif2, dest_ip='2222:3333:4444::1')
        neighbor1 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:56', handle=self.rif2, dest_ip='2222:3333:4444::1')

        # create acl_table, acl_entry
        acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IP,
            bind_point_type=[acl_table_bp_switch],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_entry = self.add_acl_entry(self.device,
            dst_ip='1234:5678:9abc:def0:4422:1133:5577:99aa',
            dst_ip_mask='FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF',
            ingress_port_lag_handle=self.port8,
            redirect=self.port10,
            table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry)
        table_name = SWITCH_DEVICE_ATTR_TABLE_IP_ACL
        pre_hw_table_count = get_hw_table_usage(self, table_name)
        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_INGRESS_ACL, acl_table)
        post_hw_table_count = get_hw_table_usage(self, table_name)
        self.assertEqual(post_hw_table_count - pre_hw_table_count, 1)

        # Attach acl_table to rif port8
        pre_counter = self.object_counters_get(acl_entry)
        try:
            #case-1: acl-redirect to nhop with egress_port:port10
            pre_counter = self.object_counters_get(acl_entry)
            print("Sending Ipv6 packet packet rif port 8 -> redirect port:port10")
            send_packet(self, self.devports[8], self.l3_pkt1_v6)
            verify_packet(self, self.l3_pkt1_v6, self.devports[10])
            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)


            #test acl-miss for unmatch packet
            route0 = self.add_route(self.device, ip_prefix='2234:5678:9abc:def0:4422:1133:5577:99aa', vrf_handle=self.vrf10,
                                    nexthop_handle=self.nhop1)
            print("Sending acl-miss IPv6 packet port 8 -> nhop rif:port9 (no redirect)")
            l3_pkt2_v6 = simple_tcpv6_packet(
                eth_dst='00:77:66:55:44:33',
              eth_src='00:22:22:22:22:22',
              ipv6_dst='2234:5678:9abc:def0:4422:1133:5577:99aa',
              ipv6_src='2000::1',
              ipv6_hlim=64)

            l3_exp_pkt2_v6 = simple_tcpv6_packet(
                eth_dst='00:11:22:33:44:55',
              eth_src='00:77:66:55:44:33',
              ipv6_dst='2234:5678:9abc:def0:4422:1133:5577:99aa',
              ipv6_src='2000::1',
              ipv6_hlim=63)
            send_packet(self, self.devports[8], l3_pkt2_v6)
            verify_packet(self, l3_exp_pkt2_v6, self.devports[9])
            # cleanup route0
            self.cleanlast()

            #unset acl from bind_point and test for no-redirect
            print("unset re-direct acl from rif port 8 and check for no-redirect")
            # delete acl entry
            self.cleanlast()
            print("Sending IPv6 packet rif port 8 -> rif port 9(no re-direct to port10)")
            send_packet(self, self.devports[8], self.l3_pkt1_v6)
            verify_packet(self, self.l3_exp_pkt1_v6, self.devports[9])

            acl_entry = self.add_acl_entry(self.device,
                dst_ip='1234:5678:9abc:def0:4422:1133:5577:99aa',
                dst_ip_mask='FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF',
                ingress_port_lag_handle=self.port8,
                redirect=self.port10,
                table_handle=acl_table)

            #case-2: acl-redirect to  next-hop, egress_port:lag
            l3_exp_redirect_pkt3 = simple_tcpv6_packet(
               eth_dst='00:11:22:33:44:57',
               eth_src='00:77:66:55:44:33',
               ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
               ipv6_src='2000::1',
               ipv6_hlim=63)
            self.attribute_set(acl_entry, SWITCH_ACL_ENTRY_ATTR_REDIRECT,
                     self.lag2)
            pre_counter = self.object_counters_get(acl_entry)
            print("Sending Ipv6 packet packet rif port 8 -> redirect port:lag2:port11/port12")
            send_packet(self, self.devports[8], self.l3_pkt1_v6)
            verify_any_packet_any_port(self, [self.l3_pkt1_v6, self.l3_pkt1_v6],
                    [self.devports[11], self.devports[12]])
            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)

            #case-5: unset acl from bind_point and test for no-redirect
            print("unset re-direct acl from rif port 8 and check for no-redirect")
            # delete acl entry
            self.cleanlast()
            print("Sending IPv6 packet rif port 8 -> rif port 9(no re-direct to port10)")
            send_packet(self, self.devports[8], self.l3_pkt1_v6)
            verify_packet(self, self.l3_exp_pkt1_v6, self.devports[9])

        finally:
            # cleanup acl_table
            self.attribute_set(self.device, SWITCH_DEVICE_ATTR_INGRESS_ACL, 0)
            hw_table_count = get_hw_table_usage(self, table_name)
            self.assertEqual(hw_table_count, 0)
            self.cleanlast()
            # cleanup neighbor1, nhop1
            self.cleanlast()
            self.cleanlast()
        return

###############################################################################

@group('acl2')
class QosAclTest(ApiHelper):
    def setUp(self):
        print()
        self.configure()

        # configure port0 and port1 for dscp testing
        self.rif0 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port0, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.rif1 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port1, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.rif2 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port2, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.nhop1 = self.add_nexthop(self.device, handle=self.rif1, dest_ip='10.10.0.2')
        self.neighbor1 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=self.rif1, dest_ip='10.10.0.2')
        self.route1 = self.add_route(self.device, ip_prefix='172.20.10.1', vrf_handle=self.vrf10, nexthop_handle=self.nhop1)
        self.route2 = self.add_route(self.device, ip_prefix='172.30.30.1', vrf_handle=self.vrf10, nexthop_handle=self.nhop1)
        self.route3 = self.add_route(self.device, ip_prefix='1234:5678:9abc:def0:4422:1133:5577:99aa', vrf_handle=self.vrf10, nexthop_handle=self.nhop1)

        print("Configuring DSCP maps")
        self.dscp_tc_map1 = self.add_qos_map(self.device, dscp=1, tc=20)
        self.dscp_tc_map2 = self.add_qos_map(self.device, dscp=2, tc=24)
        dscp_tc_maps = []
        dscp_tc_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=self.dscp_tc_map1))
        dscp_tc_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=self.dscp_tc_map2))
        self.dscp_tc_map_ingress = self.add_qos_map_ingress(self.device,
            type=SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_DSCP_TO_TC, qos_map_list=dscp_tc_maps)

        self.tc_dscp_map1 = self.add_qos_map(self.device, tc=20, dscp=9)
        self.tc_dscp_map2 = self.add_qos_map(self.device, tc=24, dscp=10)
        tc_dscp_maps = []
        tc_dscp_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=self.tc_dscp_map1))
        tc_dscp_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=self.tc_dscp_map2))
        self.tc_dscp_map_egress = self.add_qos_map_egress(self.device,
            type=SWITCH_QOS_MAP_EGRESS_ATTR_TYPE_TC_TO_DSCP, qos_map_list=tc_dscp_maps)
        self.assertEqual(self.status(), 0)

        self.acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IP_QOS,
            bind_point_type=[acl_table_bp_switch],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_INGRESS_ACL, self.acl_table)

    def runTest(self):
        print()
        try:
            self.DscpRewriteNoAclTest()
            self.DscpRewriteQosMapAndAclTest()
            self.TcUpdateDscpRewriteTest()
            self.TcUpdateIpv6DscpRewriteTest()
            self.SetColorAndMeterConfigTest()
        finally:
            self.attribute_set(self.device, SWITCH_DEVICE_ATTR_INGRESS_ACL, 0)
            pass

    def tearDown(self):
        self.cleanup()

    def DscpRewriteNoAclTest(self):
        print("DscpRewriteNoAclTest()")
        try:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_QOS_DSCP_TOS_GROUP, self.dscp_tc_map_ingress)
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_L3_QOS_GROUP, self.tc_dscp_map_egress)

            print("pass packet w/ mapped dscp value 1 -> 1")
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='172.20.10.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_tos=1 << 2, # dscp 1
                ip_ttl=64)
            exp_pkt = simple_tcp_packet(
                eth_dst='00:11:22:33:44:55',
                eth_src='00:77:66:55:44:33',
                ip_dst='172.20.10.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_tos=1 << 2, # dscp 1
                ip_ttl=63)

            # ingress qos map is enabled so dscp will be rewritten
            exp_pkt[IP].tos = 9 << 2 # dscp 9

            # qos maps shouldn't have any effect
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt, self.devports[1])
            print("pass packet w/ mapped dscp value 1 -> 9")

        finally:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_QOS_DSCP_TOS_GROUP, 0)
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_L3_QOS_GROUP, 0)
            pass

    def DscpRewriteQosMapAndAclTest(self):
        print("DscpRewriteQosMapAndAclTest()")
        # Test the case when both ingress QOS map and QOS ACL are enabled
        try:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_QOS_DSCP_TOS_GROUP, self.dscp_tc_map_ingress)
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_L3_QOS_GROUP, self.tc_dscp_map_egress)
            self.acl_entry1 = self.add_acl_entry(self.device,
                                                 dst_ip='172.30.30.1',
                                                 dst_ip_mask='255.255.255.255',
                                                 ingress_port_lag_handle=self.port0,
                                                 action_set_tc=24,
                                                 table_handle=self.acl_table)

            print("pass packet w/ mapped dscp value 1 -> 1")
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='172.30.30.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_tos=1 << 2, # dscp 1
                ip_ttl=64)
            exp_pkt = simple_tcp_packet(
                eth_dst='00:11:22:33:44:55',
                eth_src='00:77:66:55:44:33',
                ip_dst='172.30.30.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_tos=10 << 2, # dscp 10
                ip_ttl=63)

            # dscp 1 -> ingress qos_map -> tc 20 -> dscp 9
            # dscp 1 -> QOS ACL -> tc 24 -> dscp 10
            # ACL entry should take precedence over qos-map
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt, self.devports[1])
            print("pass packet w/ mapped dscp value 1 -> 10")

            # Make sure packet from different in_port is not changed
            exp_pkt[IP].tos = 1 << 2 # dscp 9
            send_packet(self, self.devports[2], pkt)
            verify_packet(self, exp_pkt, self.devports[1])
            print("pass packet w/ unchanged dscp value 1 -> 1")

            # delete acl entry, it should use qos_map value
            self.client.object_delete(self.acl_entry1)
            exp_pkt[IP].tos = 9 << 2 # dscp 9
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt, self.devports[1])
            print("pass packet w/ mapped dscp value 1 -> 9")

        finally:
            # delete acl entry
            self.client.object_delete(self.acl_entry1)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_QOS_DSCP_TOS_GROUP, 0)
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_L3_QOS_GROUP, 0)
            pass

    def TcUpdateDscpRewriteTest(self):
        print("TcUpdateDscpRewriteTest()")
        try:
            self.acl_entry = self.add_acl_entry(self.device,
                                                dst_ip='172.20.10.1',
                                                dst_ip_mask='255.255.255.255',
                                                ingress_port_lag_handle=self.port0,
                                                action_set_tc=20,
                                                table_handle=self.acl_table)

            self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_L3_QOS_GROUP, self.tc_dscp_map_egress)

            print("pass packet w/ mapped dscp value 0 -> 9")
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='172.20.10.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_tos=0,
                ip_ttl=64)
            exp_pkt = simple_tcp_packet(
                eth_dst='00:11:22:33:44:55',
                eth_src='00:77:66:55:44:33',
                ip_dst='172.20.10.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_tos=9 << 2, # dscp 9
                ip_ttl=63)

            #dscp 0 -> tc 20 -> dscp 9
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt, self.devports[1])
            print("pass packet w/ mapped dscp value 1 -> 9")

            # dscp 0 -> tc 24 -> dscp 10
            self.attribute_set(self.acl_entry, SWITCH_ACL_ENTRY_ATTR_ACTION_SET_TC, 24)
            exp_pkt2 = exp_pkt.copy()
            exp_pkt2[IP].tos = 10 << 2 # dscp 10
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt2, self.devports[1])
            print("pass packet w/ mapped dscp value 0 -> 10")

            exp_pkt3 = exp_pkt.copy()
            exp_pkt3[IP].tos = 0
            # delete acl entry
            self.client.object_delete(self.acl_entry)
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt3, self.devports[1])
            print("pass packet w/ mapped dscp value 0 -> 0")

        finally:
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_L3_QOS_GROUP, 0)
            pass

    def TcUpdateIpv6DscpRewriteTest(self):
        print("TcUpdateIpv6DscpRewriteTest()")
        try:
            self.ipv6_acl_entry = self.add_acl_entry(self.device,
                                                     dst_ip='1234:5678:9abc:def0:4422:1133:5577:99aa',
                                                     dst_ip_mask='FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF',
                                                     ingress_port_lag_handle=self.port0,
                                                     action_set_tc=20,
                                                     table_handle=self.acl_table)

            self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_L3_QOS_GROUP, self.tc_dscp_map_egress)

            print("pass ipv6 packet w/ mapped dscp value 0 -> 9")
            pkt = simple_tcpv6_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
                ipv6_src='2000::1',
                ipv6_dscp=0,
                ipv6_hlim=64)
            exp_pkt = simple_tcpv6_packet(
                eth_dst='00:11:22:33:44:55',
                eth_src='00:77:66:55:44:33',
                ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
                ipv6_src='2000::1',
                ipv6_dscp=9,
                ipv6_hlim=63)

            # dscp 0 -> tc 20 -> dscp 9
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt, self.devports[1])
            print("pass ipv6 packet w/ mapped dscp value 1 -> 9")

            # dscp 0 -> tc 24 -> dscp 10
            self.attribute_set(self.ipv6_acl_entry, SWITCH_ACL_ENTRY_ATTR_ACTION_SET_TC, 24)
            exp_pkt2 = exp_pkt.copy()
            exp_pkt2[IPv6].tc = 10 << 2 # dscp 10
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt2, self.devports[1])
            print("pass ipv6 packet w/ mapped dscp value 0 -> 10")

            exp_pkt3 = exp_pkt.copy()
            exp_pkt3[IPv6].tc = 0
            # delete acl entry
            self.client.object_delete(self.ipv6_acl_entry)
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt3, self.devports[1])
            print("pass ipv6 packet w/ mapped dscp value 0 -> 0")

        finally:
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_L3_QOS_GROUP, 0)
            pass

    def SetColorAndMeterConfigTest(self):
        print("SetColorAndMeterConfigTest()")
        try:
            self.meter = self.add_meter(self.device,
                mode=SWITCH_METER_ATTR_MODE_TWO_RATE_THREE_COLOR,
                pbs=16,cbs=8,cir=1000,pir=1000,
                type=SWITCH_METER_ATTR_TYPE_BYTES,
                color_source=SWITCH_METER_ATTR_COLOR_SOURCE_BLIND)
            self.assertEqual(self.status(), 0)

            self.meter2 = self.add_meter(self.device,
                mode=SWITCH_METER_ATTR_MODE_TWO_RATE_THREE_COLOR,
                pbs=16,cbs=8,cir=2000,pir=2000,
                type=SWITCH_METER_ATTR_TYPE_BYTES,
                color_source=SWITCH_METER_ATTR_COLOR_SOURCE_BLIND)
            self.assertEqual(self.status(), 0)

            acl_entry2 = self.add_acl_entry(self.device,
                dst_ip='172.40.40.1',
                dst_ip_mask='255.255.255.255',
                packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_PERMIT,
                action_meter_handle=self.meter,
                table_handle=self.acl_table)
            self.assertEqual(self.status(), 0)

            self.attribute_set(acl_entry2, SWITCH_ACL_ENTRY_ATTR_ACTION_METER_HANDLE, self.meter2)
            self.assertEqual(self.status(), 0)

            acl_entry3 = self.add_acl_entry(self.device,
                dst_ip='172.50.50.1',
                dst_ip_mask='255.255.255.255',
                action_set_color=SWITCH_ACL_ENTRY_ATTR_ACTION_SET_COLOR_YELLOW,
                table_handle=self.acl_table)
            self.assertEqual(self.status(), 0)

            self.attribute_set(acl_entry3, SWITCH_ACL_ENTRY_ATTR_ACTION_SET_COLOR, SWITCH_ACL_ENTRY_ATTR_ACTION_SET_COLOR_RED)
            self.assertEqual(self.status(), 0)
        finally:
            pass

###############################################################################

@group('acl2')
class SetColorDscpRewriteAclTest(ApiHelper):
    def setUp(self):
        print()
        self.configure()

        # configure port0 and port1 for dscp testing
        self.rif0 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port0, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.rif1 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port1, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.rif2 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port2, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.nhop1 = self.add_nexthop(self.device, handle=self.rif1, dest_ip='10.10.0.2')
        self.neighbor1 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=self.rif1, dest_ip='10.10.0.2')
        self.route1 = self.add_route(self.device, ip_prefix='172.20.10.1', vrf_handle=self.vrf10, nexthop_handle=self.nhop1)
        self.route2 = self.add_route(self.device, ip_prefix='172.30.30.1', vrf_handle=self.vrf10, nexthop_handle=self.nhop1)
        self.route3 = self.add_route(self.device, ip_prefix='1234:5678:9abc:def0:4422:1133:5577:99aa', vrf_handle=self.vrf10, nexthop_handle=self.nhop1)

        print("Configuring color to DSCP maps")
        self.color_dscp_map1 = self.add_qos_map(self.device, color=1, dscp=10)
        self.color_dscp_map2 = self.add_qos_map(self.device, color=2, dscp=9)
        color_dscp_maps = []
        color_dscp_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=self.color_dscp_map1))
        color_dscp_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=self.color_dscp_map2))
        self.color_dscp_map_egress = self.add_qos_map_egress(self.device,
            type=SWITCH_QOS_MAP_EGRESS_ATTR_TYPE_COLOR_TO_DSCP, qos_map_list=color_dscp_maps)
        self.assertEqual(self.status(), 0)

        self.acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IP_QOS,
            bind_point_type=[acl_table_bp_switch],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_INGRESS_ACL, self.acl_table)

    def runTest(self):
        print("Start Test")
        try:
            self.DscpRewriteNoAclTest()
            self.SetColorAclAndQosMapDscpRewriteTest()
            self.SetColorAclAndQosMapDscpRewriteTestIpv6()
            self.SetColorAclPrecedenceAndQosMapDscpRewriteTest()
        finally:
            self.attribute_set(self.device, SWITCH_DEVICE_ATTR_INGRESS_ACL, 0)
            pass

    def tearDown(self):
        self.cleanup()

    def DscpRewriteNoAclTest(self):
        print("QosColorDscpRewriteNoAclTest()")
        try:
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_L3_QOS_GROUP, self.color_dscp_map_egress)
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='172.20.10.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_tos=1 << 2, # dscp 1
                ip_ttl=64)
            exp_pkt = simple_tcp_packet(
                eth_dst='00:11:22:33:44:55',
                eth_src='00:77:66:55:44:33',
                ip_dst='172.20.10.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_tos=1 << 2, # dscp 1
                ip_ttl=63)

            # qos maps shouldn't have any effect
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt, self.devports[1])
            print("pass packet w/ mapped dscp value 1 -> 1")

        finally:
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_L3_QOS_GROUP, 0)
            pass

    def SetColorAclAndQosMapDscpRewriteTest(self):
        print("QosColorDscpRewriteQosMapAndAclTest()")
        # Test the case when both ingress QOS ACL and egress QOS map are enabled
        try:
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_L3_QOS_GROUP, self.color_dscp_map_egress)
            self.acl_entry1 = self.add_acl_entry(self.device,
                                                 src_ip='192.168.0.1',
                                                 src_ip_mask='255.255.255.255',
                                                 action_set_color=SWITCH_ACL_ENTRY_ATTR_ACTION_SET_COLOR_YELLOW,
                                                 ingress_port_lag_handle=self.port0,
                                                 table_handle=self.acl_table)

            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='172.30.30.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_tos=1 << 2, # dscp 1
                ip_ttl=64)
            exp_pkt = simple_tcp_packet(
                eth_dst='00:11:22:33:44:55',
                eth_src='00:77:66:55:44:33',
                ip_dst='172.30.30.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_tos=10 << 2, # dscp 10
                ip_ttl=63)

            # dscp 1 -> QOS ACL -> color 1 -> dscp 10
            pre_counter = self.object_counters_get(self.acl_entry1)
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt, self.devports[1])
            post_counter = self.object_counters_get(self.acl_entry1)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)
            print("pass packet w/ mapped dscp value 1 -> 10")

            # Make sure packet from different in_port is not changed
            exp_pkt[IP].tos = 1 << 2 # dscp 1 
            send_packet(self, self.devports[2], pkt)
            verify_packet(self, exp_pkt, self.devports[1])
            print("pass packet w/ unchanged dscp value 1 -> 1")
            
            # Update ACL action and check for dscp 9
            self.attribute_set(self.acl_entry1, SWITCH_ACL_ENTRY_ATTR_ACTION_SET_COLOR,
                               SWITCH_ACL_ENTRY_ATTR_ACTION_SET_COLOR_RED)
            exp_pkt[IP].tos = 9 << 2 # dscp 9
            self.client.object_counters_clear_all(self.acl_entry1)
            pre_counter = self.object_counters_get(self.acl_entry1)
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt, self.devports[1])
            post_counter = self.object_counters_get(self.acl_entry1)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)
            print("pass packet w/ mapped dscp value 1 -> 9")

        finally:
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_L3_QOS_GROUP, 0)
            pass

    def SetColorAclAndQosMapDscpRewriteTestIpv6(self):
        print("QosColorIpv6DscpRewriteTest()")
        try:
            self.ipv6_acl_entry = self.add_acl_entry(self.device,
                                                     dst_ip='1234:5678:9abc:def0:4422:1133:5577:99aa',
                                                     dst_ip_mask='FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF',
                                                     ingress_port_lag_handle=self.port0,
                                                     action_set_color=SWITCH_ACL_ENTRY_ATTR_ACTION_SET_COLOR_YELLOW,
                                                     table_handle=self.acl_table)
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_L3_QOS_GROUP, self.color_dscp_map_egress)

            print("pass ipv6 packet w/ mapped dscp value 0 -> 10")
            pkt = simple_tcpv6_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
                ipv6_src='2000::1',
                ipv6_dscp=0,
                ipv6_hlim=64)
            exp_pkt = simple_tcpv6_packet(
                eth_dst='00:11:22:33:44:55',
                eth_src='00:77:66:55:44:33',
                ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
                ipv6_src='2000::1',
                ipv6_dscp=10,
                ipv6_hlim=63)

            # Qos ACL -> color 1 -> dscp 10
            pre_counter = self.object_counters_get(self.ipv6_acl_entry)
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt, self.devports[1])
            post_counter = self.object_counters_get(self.ipv6_acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)
            print("pass ipv6 packet w/ mapped dscp value 0 -> 10")

            # Make sure packet from different in_port is not changed
            exp_pkt2 = exp_pkt.copy()
            exp_pkt2[IPv6].tc = 0 # dscp 0
            send_packet(self, self.devports[2], pkt)
            verify_packet(self, exp_pkt2, self.devports[1])
            print("pass packet w/ unchanged dscp value 0 -> 0")

        finally:
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_L3_QOS_GROUP, 0)
            pass

    def SetColorAclPrecedenceAndQosMapDscpRewriteTest(self):
        print("SetColorAclPrecedenceAndQosMapDscpRewriteTest()")
        try:
            print("Configuring DSCP color maps")
            self.dscp_color_map = self.add_qos_map(self.device, dscp=1, color=2)
            dscp_color_maps = []
            dscp_color_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=self.dscp_color_map))
            self.dscp_color_map_ingress = self.add_qos_map_ingress(self.device,
                type=SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_DSCP_TO_COLOR, qos_map_list=dscp_color_maps)

            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_QOS_DSCP_TOS_GROUP, self.dscp_color_map_ingress)
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_L3_QOS_GROUP, self.color_dscp_map_egress)

            self.acl_entry2 = self.add_acl_entry(self.device,
                                                 src_ip='192.168.0.2',
                                                 src_ip_mask='255.255.255.255',
                                                 action_set_color=SWITCH_ACL_ENTRY_ATTR_ACTION_SET_COLOR_YELLOW,
                                                 ingress_port_lag_handle=self.port0,
                                                 table_handle=self.acl_table)
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='172.30.30.1',
                ip_src='192.168.0.2',
                ip_id=105,
                ip_tos=1 << 2, # dscp 1
                ip_ttl=64)
            exp_pkt = simple_tcp_packet(
                eth_dst='00:11:22:33:44:55',
                eth_src='00:77:66:55:44:33',
                ip_dst='172.30.30.1',
                ip_src='192.168.0.2',
                ip_id=105,
                ip_tos=10 << 2, # dscp 10
                ip_ttl=63)

            # dscp 1 -> QoS Map -> color RED -> dscp 9
            # dscp 1 -> QOS ACL -> color YELLOW -> dscp 10
            # Qos ACL in the ingress should take precedence 
            pre_counter = self.object_counters_get(self.acl_entry2)
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt, self.devports[1])
            post_counter = self.object_counters_get(self.acl_entry2)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)
            print("pass packet w/ mapped dscp value 1 -> 10")

        finally:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_QOS_DSCP_TOS_GROUP, 0)
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_L3_QOS_GROUP, 0)
            pass

###############################################################################

@group('acl2')
class SetColorAndTCDscpRewriteAclTest(ApiHelper):
    def setUp(self):
        print()
        self.configure()

        # configure port0 and port1 for dscp testing
        self.rif0 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port0, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.rif1 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port1, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.rif2 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port2, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.nhop1 = self.add_nexthop(self.device, handle=self.rif1, dest_ip='10.10.0.2')
        self.neighbor1 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=self.rif1, dest_ip='10.10.0.2')
        self.route1 = self.add_route(self.device, ip_prefix='172.20.10.1', vrf_handle=self.vrf10, nexthop_handle=self.nhop1)
        self.route2 = self.add_route(self.device, ip_prefix='172.30.30.1', vrf_handle=self.vrf10, nexthop_handle=self.nhop1)
        self.route3 = self.add_route(self.device, ip_prefix='1234:5678:9abc:def0:4422:1133:5577:99aa', vrf_handle=self.vrf10, nexthop_handle=self.nhop1)

        print("Configuring color to DSCP maps")
        self.color_tc_dscp_map1 = self.add_qos_map(self.device, tc=1, color=1, dscp=10)
        self.color_tc_dscp_map2 = self.add_qos_map(self.device, tc=2, color=2, dscp=9)
        color_tc_dscp_maps = []
        color_tc_dscp_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=self.color_tc_dscp_map1))
        color_tc_dscp_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=self.color_tc_dscp_map2))
        self.color_tc_dscp_map_egress = self.add_qos_map_egress(self.device,
            type=SWITCH_QOS_MAP_EGRESS_ATTR_TYPE_TC_AND_COLOR_TO_DSCP, qos_map_list=color_tc_dscp_maps)
        self.assertEqual(self.status(), 0)

        self.acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IP_QOS,
            bind_point_type=[acl_table_bp_switch],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_INGRESS_ACL, self.acl_table)

    def runTest(self):
        print("Start Test")
        try:
            self.DscpRewriteNoAclTest()
            self.SetColorAclAndQosMapDscpRewriteTest()
            self.SetColorAclAndQosMapDscpRewriteTestIpv6()
            self.SetColorAclPrecedenceAndQosMapDscpRewriteTest()
        finally:
            self.attribute_set(self.device, SWITCH_DEVICE_ATTR_INGRESS_ACL, 0)
            pass

    def tearDown(self):
        self.cleanup()

    def DscpRewriteNoAclTest(self):
        print("DscpRewriteNoAclTest()")
        try:
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_L3_QOS_GROUP, self.color_tc_dscp_map_egress)
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='172.20.10.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_tos=1 << 2, # dscp 1
                ip_ttl=64)
            exp_pkt = simple_tcp_packet(
                eth_dst='00:11:22:33:44:55',
                eth_src='00:77:66:55:44:33',
                ip_dst='172.20.10.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_tos=1 << 2, # dscp 1
                ip_ttl=63)

            # qos maps shouldn't have any effect
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt, self.devports[1])
            print("pass packet w/ mapped dscp value 1 -> 1")

        finally:
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_L3_QOS_GROUP, 0)
            pass

    def SetColorAclAndQosMapDscpRewriteTest(self):
        print("QosColorDscpRewriteQosMapAndAclTest()")
        # Test the case when both ingress QOS ACL and egress QOS map are enabled
        try:
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_L3_QOS_GROUP, self.color_tc_dscp_map_egress)
            self.acl_entry1 = self.add_acl_entry(self.device,
                                                 src_ip='192.168.0.1',
                                                 src_ip_mask='255.255.255.255',
                                                 action_set_color=SWITCH_ACL_ENTRY_ATTR_ACTION_SET_COLOR_YELLOW,
                                                 action_set_tc=1,
                                                 ingress_port_lag_handle=self.port0,
                                                 table_handle=self.acl_table)

            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='172.30.30.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_tos=1 << 2, # dscp 1
                ip_ttl=64)
            exp_pkt = simple_tcp_packet(
                eth_dst='00:11:22:33:44:55',
                eth_src='00:77:66:55:44:33',
                ip_dst='172.30.30.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_tos=10 << 2, # dscp 10
                ip_ttl=63)

            # dscp 1 -> QOS ACL -> tc and color 1 -> dscp 10
            pre_counter = self.object_counters_get(self.acl_entry1)
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt, self.devports[1])
            post_counter = self.object_counters_get(self.acl_entry1)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)
            print("pass packet w/ mapped dscp value 1 -> 10")

            # Make sure packet from different in_port is not changed
            exp_pkt[IP].tos = 1 << 2 # dscp 1 
            send_packet(self, self.devports[2], pkt)
            verify_packet(self, exp_pkt, self.devports[1])
            print("pass packet w/ unchanged dscp value 1 -> 1")
            

        finally:
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_L3_QOS_GROUP, 0)
            pass

    def SetColorAclAndQosMapDscpRewriteTestIpv6(self):
        print("QosColorIpv6DscpRewriteTest()")
        try:
            self.ipv6_acl_entry = self.add_acl_entry(self.device,
                                                     dst_ip='1234:5678:9abc:def0:4422:1133:5577:99aa',
                                                     dst_ip_mask='FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF',
                                                     ingress_port_lag_handle=self.port0,
                                                     action_set_color=SWITCH_ACL_ENTRY_ATTR_ACTION_SET_COLOR_YELLOW,
                                                     action_set_tc=1,
                                                     table_handle=self.acl_table)
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_L3_QOS_GROUP, self.color_tc_dscp_map_egress)

            print("pass ipv6 packet w/ mapped dscp value 0 -> 10")
            pkt = simple_tcpv6_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
                ipv6_src='2000::1',
                ipv6_dscp=0,
                ipv6_hlim=64)
            exp_pkt = simple_tcpv6_packet(
                eth_dst='00:11:22:33:44:55',
                eth_src='00:77:66:55:44:33',
                ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
                ipv6_src='2000::1',
                ipv6_dscp=10,
                ipv6_hlim=63)

            # Qos ACL -> tc and color 1 -> dscp 10
            pre_counter = self.object_counters_get(self.ipv6_acl_entry)
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt, self.devports[1])
            post_counter = self.object_counters_get(self.ipv6_acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)
            print("pass ipv6 packet w/ mapped dscp value 0 -> 10")

            # Make sure packet from different in_port is not changed
            exp_pkt2 = exp_pkt.copy()
            exp_pkt2[IPv6].tc = 0 # dscp 0
            send_packet(self, self.devports[2], pkt)
            verify_packet(self, exp_pkt2, self.devports[1])
            print("pass packet w/ unchanged dscp value 0 -> 0")

        finally:
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_L3_QOS_GROUP, 0)
            pass

    def SetColorAclPrecedenceAndQosMapDscpRewriteTest(self):
        print("SetColorAclPrecedenceAndQosMapDscpRewriteTest()")
        try:
            print("Configuring DSCP color maps")
            self.dscp_tc_color_map = self.add_qos_map(self.device, dscp=1, color=2, tc=2)
            dscp_tc_color_maps = []
            dscp_tc_color_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=self.dscp_tc_color_map))
            self.dscp_tc_color_map_ingress = self.add_qos_map_ingress(self.device,
                type=SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_DSCP_TO_TC_AND_COLOR, qos_map_list=dscp_tc_color_maps)

            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_QOS_DSCP_TOS_GROUP, self.dscp_tc_color_map_ingress)
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_L3_QOS_GROUP, self.color_tc_dscp_map_egress)

            self.acl_entry2 = self.add_acl_entry(self.device,
                                                 src_ip='192.168.0.2',
                                                 src_ip_mask='255.255.255.255',
                                                 action_set_color=SWITCH_ACL_ENTRY_ATTR_ACTION_SET_COLOR_YELLOW,
                                                 action_set_tc=1,
                                                 ingress_port_lag_handle=self.port0,
                                                 table_handle=self.acl_table)
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='172.30.30.1',
                ip_src='192.168.0.2',
                ip_id=105,
                ip_tos=1 << 2, # dscp 1
                ip_ttl=64)
            exp_pkt = simple_tcp_packet(
                eth_dst='00:11:22:33:44:55',
                eth_src='00:77:66:55:44:33',
                ip_dst='172.30.30.1',
                ip_src='192.168.0.2',
                ip_id=105,
                ip_tos=10 << 2, # dscp 10
                ip_ttl=63)

            # dscp 1 -> QoS Map -> tc 2 and color RED -> dscp 9
            # dscp 1 -> QOS ACL -> tc 1 and color YELLOW -> dscp 10
            # Qos ACL in the ingress should take precedence 
            pre_counter = self.object_counters_get(self.acl_entry2)
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt, self.devports[1])
            post_counter = self.object_counters_get(self.acl_entry2)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)
            print("pass packet w/ mapped dscp value 1 -> 10")

        finally:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_QOS_DSCP_TOS_GROUP, 0)
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_L3_QOS_GROUP, 0)
            pass

###############################################################################

@group('acl2')
class EgressIpQosAclDscpRewriteTest(ApiHelper):
    def setUp(self):
        print("EgressIpQosAclDscpRewriteTest")
        self.configure()

        # configure port0 and port1 for dscp testing
        self.rif0 = self.add_rif(
            self.device,
            type=SWITCH_RIF_ATTR_TYPE_PORT,
            port_handle=self.port0,
            vrf_handle=self.vrf10,
            src_mac=self.rmac)
        self.rif1 = self.add_rif(
            self.device,
            type=SWITCH_RIF_ATTR_TYPE_PORT,
            port_handle=self.port1,
            vrf_handle=self.vrf10,
            src_mac=self.rmac)
        self.nhop1 = self.add_nexthop(self.device,
                                      handle=self.rif1, dest_ip='10.10.0.2')
        self.neighbor1 = self.add_neighbor(
            self.device,
            mac_address='00:11:22:33:44:55',
            handle=self.rif1,
            dest_ip='10.10.0.2')
        self.route2 = self.add_route(
            self.device,
            ip_prefix='172.30.30.1',
            vrf_handle=self.vrf10,
            nexthop_handle=self.nhop1)

        print("Configuring DSCP maps")
        self.dscp_tc_map1 = self.add_qos_map(self.device, dscp=1, tc=20)
        self.dscp_tc_map2 = self.add_qos_map(self.device, dscp=2, tc=24)
        dscp_tc_maps = []
        dscp_tc_maps.append(
            switcht_list_val_t(
                type=switcht_value_type.OBJECT_ID,
                oid=self.dscp_tc_map1))
        dscp_tc_maps.append(
            switcht_list_val_t(
                type=switcht_value_type.OBJECT_ID,
                oid=self.dscp_tc_map2))
        self.dscp_tc_map_ingress = self.add_qos_map_ingress(
            self.device,
            type=SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_DSCP_TO_TC,
            qos_map_list=dscp_tc_maps)

        self.tc_dscp_map1 = self.add_qos_map(self.device, tc=20, dscp=9)
        self.tc_dscp_map2 = self.add_qos_map(self.device, tc=24, dscp=10)
        tc_dscp_maps = []
        tc_dscp_maps.append(
            switcht_list_val_t(
                type=switcht_value_type.OBJECT_ID,
                oid=self.tc_dscp_map1))
        tc_dscp_maps.append(
            switcht_list_val_t(
                type=switcht_value_type.OBJECT_ID,
                oid=self.tc_dscp_map2))
        self.tc_dscp_map_egress = self.add_qos_map_egress(
            self.device,
            type=SWITCH_QOS_MAP_EGRESS_ATTR_TYPE_TC_TO_DSCP,
            qos_map_list=tc_dscp_maps)
        self.assertEqual(self.status(), 0)

        self.acl_table = self.add_acl_table(
            self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IP_QOS,
            bind_point_type=[acl_table_bp_switch],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_EGRESS)
        self.attribute_set(self.device,
                           SWITCH_DEVICE_ATTR_EGRESS_ACL, self.acl_table)

    def runTest(self):
        try:
            self.DscpRewriteQosMapAndAclTest()
        finally:
            pass

    def tearDown(self):
        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_EGRESS_ACL, 0)
        self.cleanup()

    def DscpRewriteQosMapAndAclTest(self):
        print("DscpRewriteQosMapAndAclTest()")
        # Test the case when both ingress QOS map and QOS ACL are enabled
        try:
            self.attribute_set(
                self.port0,
                SWITCH_PORT_ATTR_INGRESS_QOS_DSCP_TOS_GROUP,
                self.dscp_tc_map_ingress)
            self.attribute_set(
                self.port1,
                SWITCH_PORT_ATTR_EGRESS_QOS_GROUP,
                self.tc_dscp_map_egress)

            self.acl_entry1 = self.add_acl_entry(self.device,
                                                 dst_ip='172.30.30.1',
                                                 dst_ip_mask='255.255.255.255',
                                                 action_set_dscp=10,
                                                 table_handle=self.acl_table)

            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='172.30.30.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_tos=1 << 2, # dscp 1
                ip_ttl=64)
            exp_pkt = simple_tcp_packet(
                eth_dst='00:11:22:33:44:55',
                eth_src='00:77:66:55:44:33',
                ip_dst='172.30.30.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_tos=10 << 2, # dscp 10
                ip_ttl=63)

            # tos 1 -> ingress qos_map -> tc 20 -> tos 9
            # tos 1 -> tc 20 -> tos 9 -> QoS ACL -> tos 10
            # ACL entry should take precedence over qos-map
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt, self.devports[1])
            print("pass packet w/ mapped dscp value 1 -> 10")

            # CASE 2: Setting both DSCP:10 and ECN:3
            self.cleanlast()
            self.acl_entry1 = self.add_acl_entry(self.device,
                                                 dst_ip='172.30.30.1',
                                                 dst_ip_mask='255.255.255.255',
                                                 action_set_dscp=10,
                                                 action_set_ecn=3,
                                                 table_handle=self.acl_table)

            exp_pkt = simple_tcp_packet(
                eth_dst='00:11:22:33:44:55',
                eth_src='00:77:66:55:44:33',
                ip_dst='172.30.30.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_tos=43, # dscp 10, ecn 3
                ip_ttl=63)

            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt, self.devports[1])
            print("pass packet w/ mapped tos value 4 -> 43")

        finally:
            self.attribute_set(
                self.port0, SWITCH_PORT_ATTR_INGRESS_QOS_DSCP_TOS_GROUP, 0)
            self.attribute_set(
                self.port1, SWITCH_PORT_ATTR_EGRESS_QOS_GROUP, 0)
            pass

###############################################################################

@group('acl2')
class EgressIpQosAclPcpRewriteTest(ApiHelper):
    def setUp(self):
        print()
        self.configure()

        # configure port0 and port1 for pcp testing
        self.rif0 = self.add_rif(
            self.device,
            type=SWITCH_RIF_ATTR_TYPE_PORT,
            port_handle=self.port0,
            vrf_handle=self.vrf10,
            src_mac=self.rmac) 
        self.rif1 = self.add_rif(
            self.device, 
            type=SWITCH_RIF_ATTR_TYPE_PORT,
            port_handle=self.port1,
            vrf_handle=self.vrf10,
            src_mac=self.rmac)
        self.nhop1 = self.add_nexthop(
            self.device, handle=self.rif1, dest_ip='10.10.0.2')
        self.neighbor1 = self.add_neighbor(
            self.device,
            mac_address='00:11:22:33:44:55',
            handle=self.rif1,
            dest_ip='10.10.0.2')
        self.route2 = self.add_route(
            self.device,
            ip_prefix='172.30.30.1',
            vrf_handle=self.vrf10,
            nexthop_handle=self.nhop1)

        vlan_mbr0 = self.add_vlan_member(
            self.device,
            vlan_handle=self.vlan10,
            member_handle=self.port2,
            tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
        vlan_mbr1 = self.add_vlan_member(
            self.device,
            vlan_handle=self.vlan10,
            member_handle=self.port3,
            tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
        mac2 = self.add_mac_entry(
            self.device,
            vlan_handle=self.vlan10,
            mac_address='00:33:33:33:33:33',
            destination_handle=self.port3)
        mac3 = self.add_mac_entry(
            self.device,
            vlan_handle=self.vlan10,
            mac_address='00:44:44:44:44:44',
            destination_handle=self.port2)

        print("Configuring PCP maps")
        self.pcp_tc_map1 = self.add_qos_map(self.device, pcp=1, tc=20)
        self.pcp_tc_map2 = self.add_qos_map(self.device, pcp=2, tc=24)
        pcp_tc_maps = []
        pcp_tc_maps.append(
            switcht_list_val_t(
                type=switcht_value_type.OBJECT_ID,
                oid=self.pcp_tc_map1))
        pcp_tc_maps.append(
            switcht_list_val_t(
                type=switcht_value_type.OBJECT_ID,
                oid=self.pcp_tc_map2))
        self.pcp_tc_map_ingress = self.add_qos_map_ingress(
            self.device,
            type=SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_PCP_TO_TC,
            qos_map_list=pcp_tc_maps)

        self.tc_pcp_map1 = self.add_qos_map(self.device, tc=20, pcp=4)
        self.tc_pcp_map2 = self.add_qos_map(self.device, tc=24, pcp=5)
        tc_pcp_maps = []
        tc_pcp_maps.append(
            switcht_list_val_t(
                type=switcht_value_type.OBJECT_ID,
                oid=self.tc_pcp_map1))
        tc_pcp_maps.append(
            switcht_list_val_t(
                type=switcht_value_type.OBJECT_ID,
                oid=self.tc_pcp_map2))
        self.tc_pcp_map_egress = self.add_qos_map_egress(
            self.device,
            type=SWITCH_QOS_MAP_EGRESS_ATTR_TYPE_TC_TO_PCP,
            qos_map_list=tc_pcp_maps)
        self.assertEqual(self.status(), 0)

        self.acl_table = self.add_acl_table(
            self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IP_QOS,
            bind_point_type=[acl_table_bp_switch],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_EGRESS)
        self.attribute_set(
            self.device,
            SWITCH_DEVICE_ATTR_EGRESS_ACL,
            self.acl_table)

    def runTest(self):
        try:
            self.PcpRewriteQosMapAndAclTest()
        finally:
            pass

    def tearDown(self):
        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_EGRESS_ACL, 0)
        self.cleanup()

    def PcpRewriteQosMapAndAclTest(self):
        print("PcpRewriteQosMapAndAclTest()")
        # Test the case when both ingress QOS map and QOS ACL are enabled
        try:
            self.attribute_set(
                self.port2,
                SWITCH_PORT_ATTR_INGRESS_QOS_PCP_GROUP,
                self.pcp_tc_map_ingress)
            self.attribute_set(
                self.port3,
                SWITCH_PORT_ATTR_EGRESS_L2_QOS_GROUP,
                self.tc_pcp_map_egress)

            print("Validate with IPV4 packet")
            pkt = simple_tcp_packet(
                eth_dst='00:33:33:33:33:33',
                eth_src='00:44:44:44:44:44',
                ip_dst='172.20.10.1',
                ip_src='192.168.0.1',
                dl_vlan_enable=True,
                vlan_vid=10,
                vlan_pcp=1,
                ip_ttl=64)
            exp_pkt = pkt.copy()
            exp_pkt[Dot1Q].prio = 4
            # pcp 1 -> tc 20 -> pcp 4
            send_packet(self, self.devports[2], pkt)
            verify_packet(self, exp_pkt, self.devports[3])
            print("pass packet w/ mapped pcp value 1 -> 4")
            self.acl_entry1 = self.add_acl_entry(self.device,
                                                 dst_ip='172.20.10.1',
                                                 dst_ip_mask='255.255.255.255',
                                                 action_set_outer_vlan_pri=5,
                                                 table_handle=self.acl_table)
            # pcp 1 -> ingress qos_map -> tc 20 -> pcp 4
            # pcp 1 -> tc 20 -> pcp -> 4 QOS ACL -> pcp 5
            # ACL entry should take precedence over qos-map
            exp_pkt2 = pkt.copy()
            exp_pkt2[Dot1Q].prio = 5
            send_packet(self, self.devports[2], pkt)
            verify_packet(self, exp_pkt2, self.devports[3])
            print("pass packet w/ mapped pcp value 1 -> 5")

        finally:
            self.attribute_set(
                self.port2, SWITCH_PORT_ATTR_INGRESS_QOS_PCP_GROUP, 0)
            self.attribute_set(
                self.port3, SWITCH_PORT_ATTR_EGRESS_L2_QOS_GROUP, 0)
            pass

#############################################################################

@group('acl2')
class SetColorPcpRewriteAclTest(ApiHelper):
    def setUp(self):
        print()
        self.configure()

        # configure port0 and port1 for pcp testing
        self.rif0 = self.add_rif(
            self.device,
            type=SWITCH_RIF_ATTR_TYPE_PORT,
            port_handle=self.port0,
            vrf_handle=self.vrf10,
            src_mac=self.rmac) 
        self.rif1 = self.add_rif(
            self.device, 
            type=SWITCH_RIF_ATTR_TYPE_PORT,
            port_handle=self.port1,
            vrf_handle=self.vrf10,
            src_mac=self.rmac)
        self.nhop1 = self.add_nexthop(
            self.device, handle=self.rif1, dest_ip='10.10.0.2')
        self.neighbor1 = self.add_neighbor(
            self.device,
            mac_address='00:11:22:33:44:55',
            handle=self.rif1,
            dest_ip='10.10.0.2')
        self.route2 = self.add_route(
            self.device,
            ip_prefix='172.30.30.1',
            vrf_handle=self.vrf10,
            nexthop_handle=self.nhop1)

        vlan_mbr0 = self.add_vlan_member(
            self.device,
            vlan_handle=self.vlan10,
            member_handle=self.port2,
            tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
        vlan_mbr1 = self.add_vlan_member(
            self.device,
            vlan_handle=self.vlan10,
            member_handle=self.port3,
            tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
        mac2 = self.add_mac_entry(
            self.device,
            vlan_handle=self.vlan10,
            mac_address='00:33:33:33:33:33',
            destination_handle=self.port3)
        mac3 = self.add_mac_entry(
            self.device,
            vlan_handle=self.vlan10,
            mac_address='00:44:44:44:44:44',
            destination_handle=self.port2)

        print("Configuring PCP maps")
        self.color_pcp_map1 = self.add_qos_map(self.device, color=1, pcp=4)
        self.color_pcp_map2 = self.add_qos_map(self.device, color=2, pcp=5)
        color_pcp_maps = []
        color_pcp_maps.append(
            switcht_list_val_t(
                type=switcht_value_type.OBJECT_ID,
                oid=self.color_pcp_map1))
        color_pcp_maps.append(
            switcht_list_val_t(
                type=switcht_value_type.OBJECT_ID,
                oid=self.color_pcp_map2))
        self.color_pcp_map_egress = self.add_qos_map_egress(
            self.device,
            type=SWITCH_QOS_MAP_EGRESS_ATTR_TYPE_COLOR_TO_PCP,
            qos_map_list=color_pcp_maps)
        self.assertEqual(self.status(), 0)

        self.acl_table = self.add_acl_table(
            self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IP_QOS,
            bind_point_type=[acl_table_bp_switch],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        self.attribute_set(
            self.device,
            SWITCH_DEVICE_ATTR_INGRESS_ACL,
            self.acl_table)

    def runTest(self):
        try:
            self.SetColorAclAndQosMapPcpRewriteTest()
            self.SetColorAclPrecedenceAndQosMapPcpRewriteTest()
        finally:
            pass

    def tearDown(self):
        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_INGRESS_ACL, 0)
        self.cleanup()

    def SetColorAclAndQosMapPcpRewriteTest(self):
        print("self.SetColorAclAndQosMapPcpRewriteTest()")
        # Test the case when both ingress QOS ACL and Egress QOS map are enabled
        try:
            self.attribute_set(
                self.port3,
                SWITCH_PORT_ATTR_EGRESS_L2_QOS_GROUP,
                self.color_pcp_map_egress)

            print("Validate with IPV4 packet")
            pkt = simple_tcp_packet(
                eth_dst='00:33:33:33:33:33',
                eth_src='00:44:44:44:44:44',
                ip_dst='172.20.10.1',
                ip_src='192.168.0.1',
                dl_vlan_enable=True,
                vlan_vid=10,
                vlan_pcp=1,
                ip_ttl=64)
            exp_pkt = pkt.copy()
            exp_pkt[Dot1Q].prio = 4
            self.acl_entry1 = self.add_acl_entry(self.device,
                                                 src_ip='192.168.0.1',
                                                 src_ip_mask='255.255.255.255',
                                                 action_set_color=SWITCH_ACL_ENTRY_ATTR_ACTION_SET_COLOR_YELLOW,
                                                 table_handle=self.acl_table)
            # pcp 1 -> QOS ACL -> color yellow -> pcp 4
            pre_counter = self.object_counters_get(self.acl_entry1)
            send_packet(self, self.devports[2], pkt)
            verify_packet(self, exp_pkt, self.devports[3])
            post_counter = self.object_counters_get(self.acl_entry1)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)
            print("pass packet w/ mapped pcp value 1 -> 4")
        
        finally:
            self.attribute_set(
                self.port3, SWITCH_PORT_ATTR_EGRESS_L2_QOS_GROUP, 0)
            pass

    def SetColorAclPrecedenceAndQosMapPcpRewriteTest(self):
        print("self.SetColorAclPrecedenceAndQosMapPcpRewriteTest()")
        # Test the case when both ingress QOS ACL and Ingress QOS map are enabled
        try:
            self.pcp_color_map = self.add_qos_map(self.device, pcp=1, color=2)
            pcp_color_maps = []
            pcp_color_maps.append(
                switcht_list_val_t(
                    type=switcht_value_type.OBJECT_ID,
                    oid=self.pcp_color_map))
            self.pcp_color_map_ingress = self.add_qos_map_ingress(
                self.device,
                type=SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_PCP_TO_COLOR,
                qos_map_list=pcp_color_maps)

            self.attribute_set(
                self.port2,
                SWITCH_PORT_ATTR_INGRESS_QOS_PCP_GROUP,
                self.pcp_color_map_ingress)
            self.attribute_set(
                self.port3,
                SWITCH_PORT_ATTR_EGRESS_L2_QOS_GROUP,
                self.color_pcp_map_egress)

            print("Validate with IPV4 packet")
            pkt = simple_tcp_packet(
                eth_dst='00:33:33:33:33:33',
                eth_src='00:44:44:44:44:44',
                ip_dst='172.20.10.1',
                ip_src='192.168.0.2',
                dl_vlan_enable=True,
                vlan_vid=10,
                vlan_pcp=1,
                ip_ttl=64)
            exp_pkt = pkt.copy()
            exp_pkt[Dot1Q].prio = 4
            self.acl_entry2 = self.add_acl_entry(self.device,
                                                 src_ip='192.168.0.2',
                                                 src_ip_mask='255.255.255.255',
                                                 action_set_color=SWITCH_ACL_ENTRY_ATTR_ACTION_SET_COLOR_YELLOW,
                                                 table_handle=self.acl_table)
            # pcp 1 -> QOS ACL -> color yellow -> pcp 4
            # pcp 1 -> QOS map -> color red -> pcp 5
            # ACL entry should take precedence over qos-map
            pre_counter = self.object_counters_get(self.acl_entry2)
            send_packet(self, self.devports[2], pkt)
            verify_packet(self, exp_pkt, self.devports[3])
            post_counter = self.object_counters_get(self.acl_entry2)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)
            print("pass packet w/ mapped pcp value 1 -> 4")

        finally:
            self.attribute_set(
                self.port2, SWITCH_PORT_ATTR_INGRESS_QOS_PCP_GROUP, 0)
            self.attribute_set(
                self.port3, SWITCH_PORT_ATTR_EGRESS_L2_QOS_GROUP, 0)
            pass

#############################################################################

@group('acl2')
class SetColorAndTCPcpRewriteAclTest(ApiHelper):
    def setUp(self):
        print()
        self.configure()

        # configure port0 and port1 for pcp testing
        self.rif0 = self.add_rif(
            self.device,
            type=SWITCH_RIF_ATTR_TYPE_PORT,
            port_handle=self.port0,
            vrf_handle=self.vrf10,
            src_mac=self.rmac) 
        self.rif1 = self.add_rif(
            self.device, 
            type=SWITCH_RIF_ATTR_TYPE_PORT,
            port_handle=self.port1,
            vrf_handle=self.vrf10,
            src_mac=self.rmac)
        self.nhop1 = self.add_nexthop(
            self.device, handle=self.rif1, dest_ip='10.10.0.2')
        self.neighbor1 = self.add_neighbor(
            self.device,
            mac_address='00:11:22:33:44:55',
            handle=self.rif1,
            dest_ip='10.10.0.2')
        self.route2 = self.add_route(
            self.device,
            ip_prefix='172.30.30.1',
            vrf_handle=self.vrf10,
            nexthop_handle=self.nhop1)

        vlan_mbr0 = self.add_vlan_member(
            self.device,
            vlan_handle=self.vlan10,
            member_handle=self.port2,
            tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
        vlan_mbr1 = self.add_vlan_member(
            self.device,
            vlan_handle=self.vlan10,
            member_handle=self.port3,
            tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
        mac2 = self.add_mac_entry(
            self.device,
            vlan_handle=self.vlan10,
            mac_address='00:33:33:33:33:33',
            destination_handle=self.port3)
        mac3 = self.add_mac_entry(
            self.device,
            vlan_handle=self.vlan10,
            mac_address='00:44:44:44:44:44',
            destination_handle=self.port2)

        print("Configuring PCP maps")
        self.color_tc_pcp_map1 = self.add_qos_map(self.device, tc=1, color=1, pcp=4)
        self.color_tc_pcp_map2 = self.add_qos_map(self.device, tc=2, color=2, pcp=5)
        color_tc_pcp_maps = []
        color_tc_pcp_maps.append(
            switcht_list_val_t(
                type=switcht_value_type.OBJECT_ID,
                oid=self.color_tc_pcp_map1))
        color_tc_pcp_maps.append(
            switcht_list_val_t(
                type=switcht_value_type.OBJECT_ID,
                oid=self.color_tc_pcp_map2))
        self.color_tc_pcp_map_egress = self.add_qos_map_egress(
            self.device,
            type=SWITCH_QOS_MAP_EGRESS_ATTR_TYPE_TC_AND_COLOR_TO_PCP,
            qos_map_list=color_tc_pcp_maps)
        self.assertEqual(self.status(), 0)

        self.acl_table = self.add_acl_table(
            self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IP_QOS,
            bind_point_type=[acl_table_bp_switch],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        self.attribute_set(
            self.device,
            SWITCH_DEVICE_ATTR_INGRESS_ACL,
            self.acl_table)

    def runTest(self):
        try:
            self.SetColorAclAndQosMapPcpRewriteTest()
            self.SetColorAclPrecedenceAndQosMapPcpRewriteTest()
        finally:
            pass

    def tearDown(self):
        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_INGRESS_ACL, 0)
        self.cleanup()

    def SetColorAclAndQosMapPcpRewriteTest(self):
        print("self.SetColorAclAndQosMapPcpRewriteTest()")
        # Test the case when both ingress QOS ACL and Egress QOS map are enabled
        try:
            self.attribute_set(
                self.port3,
                SWITCH_PORT_ATTR_EGRESS_L2_QOS_GROUP,
                self.color_tc_pcp_map_egress)

            print("Validate with IPV4 packet")
            pkt = simple_tcp_packet(
                eth_dst='00:33:33:33:33:33',
                eth_src='00:44:44:44:44:44',
                ip_dst='172.20.10.1',
                ip_src='192.168.0.1',
                dl_vlan_enable=True,
                vlan_vid=10,
                vlan_pcp=1,
                ip_ttl=64)
            exp_pkt = pkt.copy()
            exp_pkt[Dot1Q].prio = 4
            self.acl_entry1 = self.add_acl_entry(self.device,
                                                 src_ip='192.168.0.1',
                                                 src_ip_mask='255.255.255.255',
                                                 action_set_color=SWITCH_ACL_ENTRY_ATTR_ACTION_SET_COLOR_YELLOW,
                                                 action_set_tc=1,
                                                 table_handle=self.acl_table)
            # pcp 1 -> QOS ACL -> tc 1 and color yellow -> pcp 4
            pre_counter = self.object_counters_get(self.acl_entry1)
            send_packet(self, self.devports[2], pkt)
            verify_packet(self, exp_pkt, self.devports[3])
            post_counter = self.object_counters_get(self.acl_entry1)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)
            print("pass packet w/ mapped pcp value 1 -> 4")
        
        finally:
            self.attribute_set(
                self.port3, SWITCH_PORT_ATTR_EGRESS_L2_QOS_GROUP, 0)
            pass

    def SetColorAclPrecedenceAndQosMapPcpRewriteTest(self):
        print("self.SetColorAclPrecedenceAndQosMapPcpRewriteTest()")
        # Test the case when both ingress QOS ACL and Ingress QOS map are enabled
        try:
            self.pcp_tc_color_map = self.add_qos_map(self.device, pcp=1, tc=2, color=2)
            pcp_tc_color_maps = []
            pcp_tc_color_maps.append(
                switcht_list_val_t(
                    type=switcht_value_type.OBJECT_ID,
                    oid=self.pcp_tc_color_map))
            self.pcp_tc_color_map_ingress = self.add_qos_map_ingress(
                self.device,
                type=SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_PCP_TO_TC_AND_COLOR,
                qos_map_list=pcp_tc_color_maps)

            self.attribute_set(
                self.port2,
                SWITCH_PORT_ATTR_INGRESS_QOS_PCP_GROUP,
                self.pcp_tc_color_map_ingress)
            self.attribute_set(
                self.port3,
                SWITCH_PORT_ATTR_EGRESS_L2_QOS_GROUP,
                self.color_tc_pcp_map_egress)

            print("Validate with IPV4 packet")
            pkt = simple_tcp_packet(
                eth_dst='00:33:33:33:33:33',
                eth_src='00:44:44:44:44:44',
                ip_dst='172.20.10.1',
                ip_src='192.168.0.2',
                dl_vlan_enable=True,
                vlan_vid=10,
                vlan_pcp=1,
                ip_ttl=64)
            exp_pkt = pkt.copy()
            exp_pkt[Dot1Q].prio = 4
            self.acl_entry2 = self.add_acl_entry(self.device,
                                                 src_ip='192.168.0.2',
                                                 src_ip_mask='255.255.255.255',
                                                 action_set_color=SWITCH_ACL_ENTRY_ATTR_ACTION_SET_COLOR_YELLOW,
                                                 action_set_tc=1,
                                                 table_handle=self.acl_table)
            # pcp 1 -> QOS ACL -> tc 1 and color yellow -> pcp 4
            # pcp 1 -> QOS map -> tc 2 and color red -> pcp 5
            # ACL entry should take precedence over qos-map
            pre_counter = self.object_counters_get(self.acl_entry2)
            send_packet(self, self.devports[2], pkt)
            verify_packet(self, exp_pkt, self.devports[3])
            post_counter = self.object_counters_get(self.acl_entry2)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)
            print("pass packet w/ mapped pcp value 1 -> 4")

        finally:
            self.attribute_set(
                self.port2, SWITCH_PORT_ATTR_INGRESS_QOS_PCP_GROUP, 0)
            self.attribute_set(
                self.port3, SWITCH_PORT_ATTR_EGRESS_L2_QOS_GROUP, 0)
            pass

#############################################################################

@group('acl2')
@group('acl2-meters')
class AclMeterTest(ApiHelper):
    pktlen = 1000
    pkt = simple_tcp_packet(
                eth_dst='00:11:11:11:11:11',
                eth_src='00:22:22:22:22:22',
                ip_dst='10.10.10.1',
                ip_ttl=64, pktlen=pktlen)
    def setUp(self):
        print()
        self.configure()

        self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_LEARNING, False)
        self.queue_handles = self.attribute_get(self.cpu_port_hdl, SWITCH_PORT_ATTR_QUEUE_HANDLES)

        vlan_mbr0 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port0)
        vlan_mbr1 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port1)
        vlan_mbr2 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port2)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.attribute_set(self.port2, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)

        mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:11:11:11:11:11', destination_handle=self.port1)
        mac1 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:22:22:22:22:22', destination_handle=self.port0)

    def runTest(self):
        try:
            #self.IngressAclPermitMeterTest()
            self.IngressMirrorAclPermitMeterTest()
            self.EgressMirrorAclPermitMeterTest()
            self.IngressQosAclPermitMeterTest()
            self.EgressQosAclPermitMeterTest()
        finally:
            pass

    def tearDown(self):
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.port2, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.cleanup()

    def IngressAclPermitMeterTest(self):
        print("IngressAclPermitMeterTest")
        table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IP_QOS

        meter = self.add_meter(self.device,
            mode=SWITCH_METER_ATTR_MODE_TWO_RATE_THREE_COLOR,
            green_packet_action=SWITCH_METER_ATTR_GREEN_PACKET_ACTION_PERMIT,
            yellow_packet_action=SWITCH_METER_ATTR_YELLOW_PACKET_ACTION_PERMIT,
            red_packet_action=SWITCH_METER_ATTR_RED_PACKET_ACTION_PERMIT,
            cbs=10,cir=10,
            type=SWITCH_METER_ATTR_TYPE_PACKETS,
            color_source=SWITCH_METER_ATTR_COLOR_SOURCE_BLIND)

        acl_table = self.add_acl_table(self.device,
            type=table_type,
            bind_point_type=[acl_table_bp_switch],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)

        acl_entry = self.add_acl_entry(self.device,
            dst_ip='10.10.10.1',
            dst_ip_mask='255.255.255.255',
            action_meter_handle=meter,
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_PERMIT,
            ingress_port_lag_handle=self.port0,
            table_handle=acl_table)
        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_INGRESS_ACL, acl_table)

        try:
            pre_counter = self.client.object_counters_get(acl_entry)
            total_packets = 20
            for _ in range(0, total_packets):
              send_packet(self, self.devports[0], self.pkt)
            print("Waiting 5 seconds")
            time.sleep(5)

            metered_packets = 0
            counter1 = self.client.object_counters_get(meter)
            for i in range(0, 3):
              metered_packets += counter1[i].count

            print("Green: ", counter1[SWITCH_METER_COUNTER_ID_GREEN_PACKETS].count)
            print("Yellow: ", counter1[SWITCH_METER_COUNTER_ID_YELLOW_PACKETS].count)
            print("Red: ", counter1[SWITCH_METER_COUNTER_ID_RED_PACKETS].count)

            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 20)

            print("Metered: ", metered_packets, "total: ", total_packets)
            self.assertEqual(metered_packets, total_packets)

        finally:
            # remove the acl entry, table and mirror
            self.attribute_set(self.device, SWITCH_DEVICE_ATTR_INGRESS_ACL, 0)
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

    def IngressMirrorAclPermitMeterTest(self):
        print("IngressMirrorAclPermitMeterTest")
        if (self.client.is_feature_enable(SWITCH_FEATURE_INGRESS_ACL_METER) == 0):
            print("Ingress ACL mirror Meter feature not enabled, skipping")
            return
        table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IP_MIRROR

        meter = self.add_meter(self.device,
            mode=SWITCH_METER_ATTR_MODE_TWO_RATE_THREE_COLOR,
            green_packet_action=SWITCH_METER_ATTR_GREEN_PACKET_ACTION_PERMIT,
            yellow_packet_action=SWITCH_METER_ATTR_YELLOW_PACKET_ACTION_DROP,
            red_packet_action=SWITCH_METER_ATTR_RED_PACKET_ACTION_DROP,
            cbs=125,pbs=125,cir=1000,pir=1000,
            type=SWITCH_METER_ATTR_TYPE_BYTES,
            color_source=SWITCH_METER_ATTR_COLOR_SOURCE_BLIND)

        mirror = self.add_mirror(self.device,
            type=SWITCH_MIRROR_ATTR_TYPE_LOCAL,
            direction=SWITCH_MIRROR_ATTR_DIRECTION_INGRESS,
            egress_port_handle=self.port2,
            meter_handle=meter
            )

        acl_table = self.add_acl_table(self.device,
            type=table_type,
            bind_point_type=[acl_table_bp_switch],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)

        acl_entry = self.add_acl_entry(self.device,
            dst_ip='10.10.10.1',
            dst_ip_mask='255.255.255.255',
            action_ingress_mirror_handle=mirror,
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_PERMIT,
            table_handle=acl_table)
        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_INGRESS_ACL, acl_table)

        try:
            pre_counter = self.client.object_counters_get(acl_entry)
            total_packets = 8
            total_bytes = 8168
            for _ in range(0, total_packets):
                send_packet(self, self.devports[0], self.pkt)
            print("Waiting 5 seconds")
            time.sleep(5)

            metered_bytes = 0
            counter1 = self.client.object_counters_get(meter)
            for i in range(0, 6):
              metered_bytes += counter1[i].count

            print("Green Bytes: ", counter1[SWITCH_METER_COUNTER_ID_GREEN_BYTES].count)
            print("Yellow Bytes: ", counter1[SWITCH_METER_COUNTER_ID_YELLOW_BYTES].count)
            print("Red Bytes: ", counter1[SWITCH_METER_COUNTER_ID_RED_BYTES].count)

            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 8)

            print("Metered Bytes: ", metered_bytes, "Total Bytes: ", total_bytes)
            self.assertEqual(metered_bytes, total_bytes)

        finally:
            # remove the acl entry, table and meter
            self.attribute_set(self.device, SWITCH_DEVICE_ATTR_INGRESS_ACL, 0)
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

    def EgressMirrorAclPermitMeterTest(self):
        print("EgressMirrorAclPermitMeterTest")
        if (self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_ACL_METER) == 0):
            print("Egress ACL mirror Meter feature not enabled, skipping")
            return
        table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IP_MIRROR

        meter = self.add_meter(self.device,
            mode=SWITCH_METER_ATTR_MODE_TWO_RATE_THREE_COLOR,
            green_packet_action=SWITCH_METER_ATTR_GREEN_PACKET_ACTION_PERMIT,
            yellow_packet_action=SWITCH_METER_ATTR_YELLOW_PACKET_ACTION_DROP,
            red_packet_action=SWITCH_METER_ATTR_RED_PACKET_ACTION_DROP,
            cbs=125,pbs=125,cir=1000,pir=1000,
            type=SWITCH_METER_ATTR_TYPE_BYTES,
            color_source=SWITCH_METER_ATTR_COLOR_SOURCE_BLIND)

        mirror = self.add_mirror(self.device,
            type=SWITCH_MIRROR_ATTR_TYPE_LOCAL,
            direction=SWITCH_MIRROR_ATTR_DIRECTION_EGRESS,
            egress_port_handle=self.port2,
            meter_handle=meter
            )

        acl_table = self.add_acl_table(self.device,
            type=table_type,
            bind_point_type=[acl_table_bp_switch],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_EGRESS)

        acl_entry = self.add_acl_entry(self.device,
            dst_ip='10.10.10.1',
            dst_ip_mask='255.255.255.255',
            action_egress_mirror_handle=mirror,
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_PERMIT,
            table_handle=acl_table)
        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_EGRESS_ACL, acl_table)

        try:
            pre_counter = self.client.object_counters_get(acl_entry)
            total_packets = 8
            total_bytes = 8240
            for _ in range(0, total_packets):
              send_packet(self, self.devports[0], self.pkt)
            print("Waiting 5 seconds")
            time.sleep(5)

            metered_bytes = 0
            counter1 = self.client.object_counters_get(meter)
            for i in range(0, 6):
              metered_bytes += counter1[i].count

            print("Green Bytes: ", counter1[SWITCH_METER_COUNTER_ID_GREEN_BYTES].count)
            print("Yellow Bytes: ", counter1[SWITCH_METER_COUNTER_ID_YELLOW_BYTES].count)
            print("Red Bytes: ", counter1[SWITCH_METER_COUNTER_ID_RED_BYTES].count)

            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 8)

            print("Metered Bytes: ", metered_bytes, "Total Bytes: ", total_bytes)
            self.assertEqual(metered_bytes, total_bytes)

        finally:
            # remove the acl entry, table and meter
            self.attribute_set(self.device, SWITCH_DEVICE_ATTR_EGRESS_ACL, 0)
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

    def IngressQosAclPermitMeterTest(self):
        print("IngressQosAclPermitMeterTest")
        if (self.client.is_feature_enable(SWITCH_FEATURE_INGRESS_ACL_METER) == 0):
            print("Ingress ACL Meter feature not enabled, skipping")
            return
        table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IP_QOS

        meter = self.add_meter(self.device,
            mode=SWITCH_METER_ATTR_MODE_TWO_RATE_THREE_COLOR,
            green_packet_action=SWITCH_METER_ATTR_GREEN_PACKET_ACTION_PERMIT,
            yellow_packet_action=SWITCH_METER_ATTR_YELLOW_PACKET_ACTION_DROP,
            red_packet_action=SWITCH_METER_ATTR_RED_PACKET_ACTION_DROP,
            cbs=125,pbs=125,cir=1000,pir=1000,
            type=SWITCH_METER_ATTR_TYPE_BYTES,
            color_source=SWITCH_METER_ATTR_COLOR_SOURCE_BLIND)

        acl_table = self.add_acl_table(self.device,
            type=table_type,
            bind_point_type=[acl_table_bp_switch],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)

        acl_entry = self.add_acl_entry(self.device,
            dst_ip='10.10.10.1',
            dst_ip_mask='255.255.255.255',
            action_meter_handle=meter,
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_PERMIT,
            table_handle=acl_table)
        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_INGRESS_ACL, acl_table)

        try:
            pre_counter = self.client.object_counters_get(acl_entry)
            total_packets = 8
            total_bytes = total_packets * (self.pktlen + crc_len)
            for _ in range(0, total_packets):
              send_packet(self, self.devports[0], self.pkt)
            print("Waiting 5 seconds")
            time.sleep(5)

            metered_bytes = 0
            counter1 = self.client.object_counters_get(meter)
            for i in range(0, 6):
              metered_bytes += counter1[i].count

            print("Green Bytes: ", counter1[SWITCH_METER_COUNTER_ID_GREEN_BYTES].count)
            print("Yellow Bytes: ", counter1[SWITCH_METER_COUNTER_ID_YELLOW_BYTES].count)
            print("Red Bytes: ", counter1[SWITCH_METER_COUNTER_ID_RED_BYTES].count)
            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, total_packets)
            self.assertEqual(post_counter[1].count - pre_counter[1].count, total_bytes)

            print("Metered Bytes: ", metered_bytes, "Total Bytes: ", total_bytes)
            self.assertEqual(metered_bytes, total_bytes)

        finally:
            # remove the acl entry, table and meter
            self.attribute_set(self.device, SWITCH_DEVICE_ATTR_INGRESS_ACL, 0)
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

    def EgressQosAclPermitMeterTest(self):
        print("EgressQosAclPermitMeterTest")
        if (self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_ACL_METER) == 0):
            print("Egress ACL Meter feature not enabled, skipping")
            return
        table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IP_QOS

        meter = self.add_meter(self.device,
            mode=SWITCH_METER_ATTR_MODE_TWO_RATE_THREE_COLOR,
            green_packet_action=SWITCH_METER_ATTR_GREEN_PACKET_ACTION_PERMIT,
            yellow_packet_action=SWITCH_METER_ATTR_YELLOW_PACKET_ACTION_DROP,
            red_packet_action=SWITCH_METER_ATTR_RED_PACKET_ACTION_DROP,
            cbs=125,pbs=125,cir=1000,pir=1000,
            type=SWITCH_METER_ATTR_TYPE_BYTES,
            color_source=SWITCH_METER_ATTR_COLOR_SOURCE_BLIND)

        acl_table = self.add_acl_table(self.device,
            type=table_type,
            bind_point_type=[acl_table_bp_switch],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_EGRESS)

        acl_entry = self.add_acl_entry(self.device,
            dst_ip='10.10.10.1',
            dst_ip_mask='255.255.255.255',
            action_meter_handle=meter,
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_PERMIT,
            table_handle=acl_table)
        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_EGRESS_ACL, acl_table)

        try:
            pre_counter = self.client.object_counters_get(acl_entry)
            total_packets = 8
            total_bytes = total_packets * (self.pktlen + crc_len)
            for _ in range(0, total_packets):
              send_packet(self, self.devports[0], self.pkt)
            print("Waiting 5 seconds")
            time.sleep(5)

            metered_bytes = 0
            counter1 = self.client.object_counters_get(meter)
            for i in range(0, 6):
              metered_bytes += counter1[i].count

            print("Green Bytes: ", counter1[SWITCH_METER_COUNTER_ID_GREEN_BYTES].count)
            print("Yellow Bytes: ", counter1[SWITCH_METER_COUNTER_ID_YELLOW_BYTES].count)
            print("Red Bytes: ", counter1[SWITCH_METER_COUNTER_ID_RED_BYTES].count)
            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, total_packets)
            self.assertEqual(post_counter[1].count - pre_counter[1].count, total_bytes)

            print("Metered Bytes: ", metered_bytes, "Total Bytes: ", total_bytes)
            #TODO: Check the byte counter, 6 extra bytes.
            #self.assertEqual(metered_bytes, total_bytes)

        finally:
            # remove the acl entry, table and meter
            self.attribute_set(self.device, SWITCH_DEVICE_ATTR_EGRESS_ACL, 0)
            self.cleanlast()


###############################################################################
