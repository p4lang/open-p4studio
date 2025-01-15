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
from switch_l2 import L2LagTest


pkt_len = int(test_param_get('pkt_size'))

from ast import literal_eval


acl_table_bp_port = switcht_list_val_t(type=switcht_value_type.ENUM, enumdata=SWITCH_ACL_TABLE_ATTR_BIND_POINT_TYPE_PORT)
acl_table_bp_lag = switcht_list_val_t(type=switcht_value_type.ENUM, enumdata=SWITCH_ACL_TABLE_ATTR_BIND_POINT_TYPE_LAG)
acl_table_bp_vlan = switcht_list_val_t(type=switcht_value_type.ENUM, enumdata=SWITCH_ACL_TABLE_ATTR_BIND_POINT_TYPE_VLAN)
acl_table_bp_rif = switcht_list_val_t(type=switcht_value_type.ENUM, enumdata=SWITCH_ACL_TABLE_ATTR_BIND_POINT_TYPE_RIF)
acl_table_bp_switch = switcht_list_val_t(type=switcht_value_type.ENUM, enumdata=SWITCH_ACL_TABLE_ATTR_BIND_POINT_TYPE_SWITCH)
acl_group_bp_port = switcht_list_val_t(type=switcht_value_type.ENUM, enumdata=SWITCH_ACL_GROUP_ATTR_BIND_POINT_TYPE_PORT)
acl_group_bp_vlan = switcht_list_val_t(type=switcht_value_type.ENUM, enumdata=SWITCH_ACL_GROUP_ATTR_BIND_POINT_TYPE_VLAN)
acl_group_bp_rif = switcht_list_val_t(type=switcht_value_type.ENUM, enumdata=SWITCH_ACL_GROUP_ATTR_BIND_POINT_TYPE_RIF)

def get_hw_table_usage(self, table_name):
  return self.client.table_info_get(table_name).usage

@group('acl')
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
        self.pkt = simple_tcp_packet(pktlen=pkt_len,
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        self.exp_pkt = simple_tcp_packet(pktlen=pkt_len,
            eth_dst='00:11:22:33:44:55',
            eth_src='00:77:66:55:44:33',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63)
        self.exp_pkt2 = simple_tcp_packet(pktlen=pkt_len,
            eth_dst='00:11:22:33:44:56',
            eth_src='00:77:66:55:44:33',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63)
        self.pkt1 = simple_tcp_packet(pktlen=pkt_len,
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.10.10.5',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        self.exp_pkt1 = simple_tcp_packet(pktlen=pkt_len,
            eth_dst='00:55:55:55:55:55',
            eth_src='00:77:66:55:44:33',
            ip_dst='10.10.10.5',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63)
        self.lag_pkt = simple_tcp_packet(pktlen=pkt_len,
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.10.10.4',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        self.exp_lag_pkt = simple_tcp_packet(pktlen=pkt_len,
            eth_dst='00:11:22:33:44:57',
            eth_src='00:77:66:55:44:33',
            ip_dst='10.10.10.4',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63)
        self.l2_pkt1 = simple_tcp_packet(pktlen=pkt_len,
            eth_dst='00:55:55:55:55:55',
            eth_src='00:44:44:44:44:44',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        self.l2_pkt2 = simple_tcp_packet(pktlen=pkt_len,
            eth_dst='00:44:44:44:44:44',
            eth_src='00:55:55:55:55:55',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        self.vlan20_pkt1 = simple_tcp_packet(pktlen=pkt_len,
            eth_dst='00:66:66:66:66:66',
            eth_src='00:77:77:77:77:77',
            ip_dst='20.20.20.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        self.vlan20_pkt2 = simple_tcp_packet(pktlen=pkt_len,
            eth_dst='00:77:77:77:77:77',
            eth_src='00:66:66:66:66:66',
            ip_dst='20.20.20.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)

        self.pkt_v6 = simple_tcpv6_packet(pktlen=pkt_len,
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
            ipv6_src='2000::1',
            ipv6_hlim=64)
        self.exp_pkt_v6 = simple_tcpv6_packet(pktlen=pkt_len,
            eth_dst='00:11:22:33:44:55',
            eth_src='00:77:66:55:44:33',
            ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
            ipv6_src='2000::1',
            ipv6_hlim=63)

        try:
            self.TrafficTestWithoutAclConfig()
            self.MacAclTableTest()
            self.MacAclTablePriorityTest()
            self.PreIngressAclTableTest()
            self.IPAclTableTest()
            self.IPv4AclTableTest()
            self.IngressAdminStateAclTest()
            self.EgressAdminStateAclTest()
            self.IPv4AclTableStatsClearTest()
            self.IPv4AclTablePriorityTest()
            self.LagIPv4AclTableTest()
            self.IPv6AclTableTest()
            self.AclRangeTest()
            self.AclPriorityRangeVlanBPointTest()
            self.IPv4AclRedirectTest()
            self.IPv4RaclTableTest()
            self.IPv4RaclRedirectTest()
            self.IPv6RaclTableTest()
            self.IPv6AclTablePriorityTest()
            self.IPv4RaclTablePriorityTest()
            self.IPv6RaclTablePriorityTest()
            self.L3BdLabelTest()
            self.SVIAclTest()
            self.SVIBdLabelTest()
            self.VlanAclTest()
            self.VlanBdLabelTest()
            self.IngressOuterVlanIdv4AclTest()
            self.IngressOuterVlanIdv6AclTest()
            self.IngressOuterVlanIdMirrorAclTest()
            self.EgressOuterVlanIdv4AclTest()
            #self.EgressOuterVlanIdv6AclTest()
            self.EgressOuterVlanIdMirrorAclTest()
            self.SimpleAclGroupTest()
            self.SharedAclTableTest()
            self.SharedAclGroupTest()
            self.SharedAclGroupTest2()
            self.EgressMacAclTableTest()
            self.EgressIPv4AclTableTest()
            self.LagEgressIPv4AclTableTest()
            self.EgressDscpDenyMirrorTableTest()
            self.EgressIPv6AclTableTest()
            self.EgressPortLabelTest()
            self.EgressLagLabelTest()
            self.IPv4AclTableUserMetaTest()
            self.EgressMacAclTablePrioTest()
            self.EgressIPv4AclTablePrioTest()
            self.EgressIPv6AclTablePrioTest()
            self.EgressSharedAclGroupTest()
            self.SharedAclGroupMirrorTest()
            self.SharedAclGroupDscpMirrorTest()
            if(self.client.is_feature_enable(SWITCH_FEATURE_BD_LABEL_IN_EGRESS_ACL) == 0):
                print("Egress ACL BD_LABEL feature is not supported. VLAN and RIF BindPoint/Label Tests")
                return
            else:
                self.EgressL3BdLabelTest()
                self.EgressSVIAclTest()
                self.EgressSVIBdLabelTest()
                self.EgressVlanAclTest()
                self.EgressVlanBdLabelTest()

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
        We send a packet from port 0 and drop the packet
        The bp_type is PORT. The port_lag_label comes from the table id
        '''
        print("MacAclTableTest()")
        if (self.client.is_feature_enable(SWITCH_FEATURE_INGRESS_MAC_ACL) == 0):
            print("MAC ACL feature not enabled, skipping")
            return
        acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_MAC,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_entry = self.add_acl_entry(self.device,
            src_mac='00:22:22:22:22:22',
            src_mac_mask='ff:ff:ff:ff:ff:ff',
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry)
        table_name = SWITCH_DEVICE_ATTR_TABLE_MAC_ACL
        pre_hw_table_count = get_hw_table_usage(self, table_name)
        self.attribute_set(self.port2, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, acl_table)
        post_hw_table_count = get_hw_table_usage(self, table_name)
        self.assertEqual(post_hw_table_count - pre_hw_table_count, 1)

        def TestTraffic():
            print(" Testing Traffic")
            m_pkt = simple_eth_packet(
                eth_dst='00:44:44:44:44:44',
                eth_src='00:22:22:22:22:22')

            pre_counter = self.object_counters_get(acl_entry)
            send_packet(self, self.devports[2], m_pkt)
            verify_no_other_packets(self, timeout=2)
            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)

        try:
            TestTraffic()
            self.startFrWrReplay(1, TestTraffic)
        finally:
            self.attribute_set(self.port2, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
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
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_entry1 = self.add_acl_entry(self.device,
            src_mac='00:77:77:77:77:22',
            src_mac_mask='ff:ff:ff:ff:ff:ff',
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            priority=self.high_prio,
            table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry1)
        acl_entry2 = self.add_acl_entry(self.device,
            src_mac='00:77:77:77:77:00',
            src_mac_mask='ff:ff:ff:ff:ff:00',
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_PERMIT,
            priority=self.low_prio,
            table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry2)
        table_name = SWITCH_DEVICE_ATTR_TABLE_MAC_ACL
        pre_hw_table_count = get_hw_table_usage(self, table_name)
        self.attribute_set(self.port2, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, acl_table)
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
            self.attribute_set(self.port2, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            # remove the acl entry1,acl_entry2 and table
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
    ############################################################################

    def PreIngressAclTableTest(self):
        '''
        This test tries to verify pre ingress acl entries
        We send a packet from port 0 and forward on vrf10
        Add acl which sets to vrf20 and hit different route entry
        '''
        print("PreIngressAclTableTest()")
        if (self.client.is_feature_enable(SWITCH_FEATURE_PRE_INGRESS_ACL) == 0):
            print("Pre ingress ACL feature not enabled, skipping")
            return
        vrf20 = self.add_vrf(self.device, id=20, src_mac=self.rmac)
        self.add_route(self.device, ip_prefix='10.10.10.1', vrf_handle=vrf20, nexthop_handle=self.nhop2)
        acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_PREINGRESS,
            bind_point_type=[acl_table_bp_switch],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_PREINGRESS)


        def TestTraffic():
            print(" Testing Traffic")
            print(" Send packet without pre-ingress ACL, forward to %d" % self.devports[1])
            send_packet(self, self.devports[0], self.pkt)
            verify_packet(self, self.exp_pkt, self.devports[1])

            print(" Send packet with pre-ingress ACL, forward to %d" % self.devports[3])
            acl_entry = self.add_acl_entry(self.device,
                src_mac='00:22:22:22:22:22',
                src_mac_mask='ff:ff:ff:ff:ff:ff',
                action_set_vrf_handle=vrf20,
                table_handle=acl_table)
            self.client.object_counters_clear_all(acl_entry)
            table_name = SWITCH_DEVICE_ATTR_TABLE_PRE_INGRESS_ACL
            pre_hw_table_count = get_hw_table_usage(self, table_name)
            self.attribute_set(self.device, SWITCH_DEVICE_ATTR_PRE_INGRESS_ACL, acl_table)
            post_hw_table_count = get_hw_table_usage(self, table_name)
            self.assertEqual(post_hw_table_count - pre_hw_table_count, 1)
            pre_counter = self.object_counters_get(acl_entry)
            send_packet(self, self.devports[0], self.pkt)
            verify_packet(self, self.exp_pkt2, self.devports[3])
            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)

        try:
            TestTraffic()
            self.startFrWrReplay(1, TestTraffic)
        finally:
            self.attribute_set(self.device, SWITCH_DEVICE_ATTR_PRE_INGRESS_ACL, 0)
            # remove the acl entry and table
            self.cleanlast()
            self.cleanlast()
            # remove route and vrf
            self.cleanlast()
            self.cleanlast()
    ############################################################################

    def IPAclTableTest(self):
        '''
        This test tries to verify ip acl entries
        We send a packet from port 0 and drop the packet
        The bp_type is PORT. The port_lag_label comes from the table id
        '''
        print("IPAclTableTest()")
        if (self.client.is_feature_enable(SWITCH_FEATURE_SHARED_INGRESS_IP_ACL) == 0):
            print("Shared IP ingress ACL feature not enabled, skipping")
            return
        acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IP,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_entry = self.add_acl_entry(self.device,
            dst_ip='10.10.10.1',
            dst_ip_mask='255.255.255.255',
            #tcp_flags=2,
            #tcp_flags_mask=127,
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry)
        table_name = SWITCH_DEVICE_ATTR_TABLE_IP_ACL
        pre_hw_table_count = get_hw_table_usage(self, table_name)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, acl_table)
        post_hw_table_count = get_hw_table_usage(self, table_name)
        self.assertEqual(post_hw_table_count - pre_hw_table_count, 1)

        def TestTraffic():
            print(" Testing Traffic")
            pre_counter = self.object_counters_get(acl_entry)
            send_packet(self, self.devports[0], self.pkt)
            verify_no_other_packets(self, timeout=2)
            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)

        try:
            TestTraffic()
            self.startFrWrReplay(0, TestTraffic)  # WI or FI not testing
        finally:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            # remove the acl entry and table
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
        if self.client.is_feature_enable(SWITCH_FEATURE_SHARED_INGRESS_IP_ACL):
            table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IP

        acl_table = self.add_acl_table(self.device,
            type=table_type,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_entry = self.add_acl_entry(self.device,
            dst_ip='10.10.10.1',
            dst_ip_mask='255.255.255.255',
            #tcp_flags=2,
            #tcp_flags_mask=127,
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry)
        table_name = SWITCH_DEVICE_ATTR_TABLE_IPV4_ACL
        if self.client.is_feature_enable(SWITCH_FEATURE_SHARED_INGRESS_IP_ACL):
            table_name = SWITCH_DEVICE_ATTR_TABLE_IP_ACL
        pre_hw_table_count = get_hw_table_usage(self, table_name)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, acl_table)
        post_hw_table_count = get_hw_table_usage(self, table_name)
        self.assertEqual(post_hw_table_count - pre_hw_table_count, 1)

        def TestTraffic():
            print(" Testing Traffic")
            pre_counter = self.object_counters_get(acl_entry)
            send_packet(self, self.devports[0], self.pkt)
            verify_no_other_packets(self, timeout=2)
            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)

        try:
            TestTraffic()
            self.startFrWrReplay(0, TestTraffic)  # WI or FI not testing
        finally:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            # remove the acl entry and table
            self.cleanlast()
            self.cleanlast()
    ############################################################################

    def IngressAdminStateAclTest(self):
        '''
        This test tries to verify acl admin state
        We send a packet from port 0 and drop the packet in ingress
        The bp_type is PORT. The port_lag_label comes from the table id
        '''
        print("IngressAdminStateAclTest()")
        table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IPV4
        if self.client.is_feature_enable(SWITCH_FEATURE_SHARED_INGRESS_IP_ACL):
            table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IP

        acl_table = self.add_acl_table(self.device,
            type=table_type,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_entry = self.add_acl_entry(self.device,
            dst_ip='10.10.10.1',
            dst_ip_mask='255.255.255.255',
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            table_handle=acl_table,
            admin_state=True)
        admin_state_attr = self.attribute_get(acl_entry, SWITCH_ACL_ENTRY_ATTR_ADMIN_STATE)
        self.assertEqual(admin_state_attr, True)

        self.attribute_set(acl_entry, SWITCH_ACL_ENTRY_ATTR_ADMIN_STATE, False)
        admin_state_attr = self.attribute_get(acl_entry, SWITCH_ACL_ENTRY_ATTR_ADMIN_STATE)
        self.assertEqual(admin_state_attr, False)

        self.client.object_counters_clear_all(acl_entry)
        table_name = SWITCH_DEVICE_ATTR_TABLE_IPV4_ACL
        if self.client.is_feature_enable(SWITCH_FEATURE_SHARED_INGRESS_IP_ACL):
            table_name = SWITCH_DEVICE_ATTR_TABLE_IP_ACL
        pre_hw_table_count = get_hw_table_usage(self, table_name)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, acl_table)
        post_hw_table_count = get_hw_table_usage(self, table_name)
        self.assertEqual(post_hw_table_count - pre_hw_table_count, 1)

        def TestTraffic():
            print(" Testing Traffic")
            pre_counter = self.object_counters_get(acl_entry)
            send_packet(self, self.devports[0], self.pkt)
            verify_no_other_packets(self, timeout=2)
            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)

        try:
            TestTraffic()
            self.startFrWrReplay(0, TestTraffic)  # WI or FI not testing
        finally:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            # remove the acl entry and table
            self.cleanlast()
            self.cleanlast()
    ############################################################################

    def EgressAdminStateAclTest(self):
        '''
        This test tries to verify acl admin state
        We send a packet from port 0 and drop the packet in egress
        The bp_type is PORT. The port_lag_label comes from the table id
        '''
        print("EgressAdminStateAclTest()")
        table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IPV4
        if (self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_IP_ACL) == 0):
            print("EgressIPv4Acl  feature not enabled, skipping")
            return

        acl_table = self.add_acl_table(self.device,
            type=table_type,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_EGRESS)
        acl_entry = self.add_acl_entry(self.device,
            dst_ip='10.10.10.1',
            dst_ip_mask='255.255.255.255',
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            table_handle=acl_table,
            admin_state=True)
        admin_state_attr = self.attribute_get(acl_entry, SWITCH_ACL_ENTRY_ATTR_ADMIN_STATE)
        self.assertEqual(admin_state_attr, True)

        self.attribute_set(acl_entry, SWITCH_ACL_ENTRY_ATTR_ADMIN_STATE, False)
        admin_state_attr = self.attribute_get(acl_entry, SWITCH_ACL_ENTRY_ATTR_ADMIN_STATE)
        self.assertEqual(admin_state_attr, False)

        self.client.object_counters_clear_all(acl_entry)
        table_name = SWITCH_DEVICE_ATTR_TABLE_IPV4_ACL
        pre_hw_table_count = get_hw_table_usage(self, table_name)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, acl_table)

        def TestTraffic():
            print(" Testing Traffic")
            pre_counter = self.object_counters_get(acl_entry)
            send_packet(self, self.devports[0], self.pkt)
            verify_no_other_packets(self, timeout=2)
            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)

        try:
            TestTraffic()
            self.startFrWrReplay(0, TestTraffic)  # WI or FI not testing
        finally:
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, 0)
            # remove the acl entry and table
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
        if self.client.is_feature_enable(SWITCH_FEATURE_SHARED_INGRESS_IP_ACL):
            table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IP

        acl_table = self.add_acl_table(self.device,
            type=table_type,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_entry = self.add_acl_entry(self.device,
            dst_ip='10.10.10.1',
            dst_ip_mask='255.255.255.255',
            #tcp_flags=2,
            #tcp_flags_mask=127,
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry)
        table_name = SWITCH_DEVICE_ATTR_TABLE_IPV4_ACL
        if self.client.is_feature_enable(SWITCH_FEATURE_SHARED_INGRESS_IP_ACL):
            table_name = SWITCH_DEVICE_ATTR_TABLE_IP_ACL
        pre_hw_table_count = get_hw_table_usage(self, table_name)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, acl_table)
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
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            # remove the acl entry and table
            self.cleanlast()
            self.cleanlast()
    ############################################################################

    def IPv4AclTablePriorityTest(self):
        '''
        This test tries to veriry v4 acl table priority
        '''
        print("IPv4AclTablePriorityTest()")
        table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IPV4
        if self.client.is_feature_enable(SWITCH_FEATURE_SHARED_INGRESS_IP_ACL):
            table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IP

        acl_table = self.add_acl_table(self.device,
            type=table_type,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_entry1 = self.add_acl_entry(self.device,
            dst_ip='10.10.10.1',
            dst_ip_mask='255.255.255.255',
            #tcp_flags=2,
            #tcp_flags_mask=127,
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            priority = self.high_prio,
            table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry1)
        acl_entry2 = self.add_acl_entry(self.device,
            dst_ip='10.10.10.0',
            dst_ip_mask='255.255.255.0',
            priority=self.low_prio,
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_PERMIT,
            table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry2)

        table_name = SWITCH_DEVICE_ATTR_TABLE_IPV4_ACL
        if self.client.is_feature_enable(SWITCH_FEATURE_SHARED_INGRESS_IP_ACL):
            table_name = SWITCH_DEVICE_ATTR_TABLE_IP_ACL
        pre_hw_table_count = get_hw_table_usage(self, table_name)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, acl_table)
        post_hw_table_count = get_hw_table_usage(self, table_name)
        self.assertEqual(post_hw_table_count - pre_hw_table_count, 2)
        route0 = self.add_route(self.device, ip_prefix='10.10.10.0/24', vrf_handle=self.vrf10,
                                nexthop_handle=self.nhop1)

        def TestTraffic():
            print(" Testing Traffic")
            pre_counter = self.object_counters_get(acl_entry1)
            send_packet(self, self.devports[0], self.pkt)
            verify_no_other_packets(self, timeout=2)
            post_counter = self.object_counters_get(acl_entry1)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)

            acl_entry2_pkt = simple_tcp_packet(pktlen=pkt_len,
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='10.10.10.8',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_ttl=64)
            acl_entry2_exp_pkt = simple_tcp_packet(pktlen=pkt_len,
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
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            # remove the acl entry1, acl_entry2 and table
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
        if self.client.is_feature_enable(SWITCH_FEATURE_SHARED_INGRESS_IP_ACL):
            table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IP

        acl_table = self.add_acl_table(self.device,
            type=table_type,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_entry = self.add_acl_entry(self.device,
            dst_ip='10.10.10.1',
            dst_ip_mask='255.255.255.255',
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry)
        table_name = SWITCH_DEVICE_ATTR_TABLE_IPV4_ACL
        if self.client.is_feature_enable(SWITCH_FEATURE_SHARED_INGRESS_IP_ACL):
            table_name = SWITCH_DEVICE_ATTR_TABLE_IP_ACL
        pre_hw_table_count = get_hw_table_usage(self, table_name)
        self.attribute_set(self.lag0, SWITCH_LAG_ATTR_INGRESS_ACL_HANDLE, acl_table)
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

        try:
            TestTraffic()
            self.startFrWrReplay(1, TestTraffic)
        finally:
            self.attribute_set(self.lag0, SWITCH_LAG_ATTR_INGRESS_ACL_HANDLE, 0)
            # remove the acl entry and table
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
        if self.client.is_feature_enable(SWITCH_FEATURE_SHARED_INGRESS_IP_ACL):
            table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IP

        acl_table = self.add_acl_table(self.device,
            type=table_type,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_entry = self.add_acl_entry(self.device,
            dst_ip='1234:5678:9abc:def0:4422:1133:5577:99aa',
            dst_ip_mask='FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF',
            #tcp_flags=2,
            #tcp_flags_mask=127,
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry)
        table_name = SWITCH_DEVICE_ATTR_TABLE_IPV6_ACL
        if self.client.is_feature_enable(SWITCH_FEATURE_SHARED_INGRESS_IP_ACL):
            table_name = SWITCH_DEVICE_ATTR_TABLE_IP_ACL
        pre_hw_table_count = get_hw_table_usage(self, table_name)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, acl_table)
        post_hw_table_count = get_hw_table_usage(self, table_name)
        self.assertEqual(post_hw_table_count - pre_hw_table_count, 1)

        def TestTraffic():
            print(" Testing Traffic")
            pre_counter = self.object_counters_get(acl_entry)
            send_packet(self, self.devports[0], self.pkt_v6)
            verify_no_other_packets(self, timeout=2)
            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)

        try:
            TestTraffic()
            self.startFrWrReplay(1, TestTraffic)
        finally:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            # remove the acl entry and table
            self.cleanlast()
            self.cleanlast()
    ############################################################################

    def IPv6AclTablePriorityTest(self):
        '''
        This test tries to veriry v6 acl table priority
        '''
        print("IPv6AclTablePriorityTest()")
        table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IPV6
        if self.client.is_feature_enable(SWITCH_FEATURE_SHARED_INGRESS_IP_ACL):
            table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IP

        acl_table = self.add_acl_table(self.device,
            type=table_type,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_entry1 = self.add_acl_entry(self.device,
            dst_ip='1234:5678:9abc:def0:4422:1133:5577:99aa',
            dst_ip_mask='FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF',
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            priority = self.high_prio,
            table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry1)
        acl_entry2 = self.add_acl_entry(self.device,
            dst_ip='1234:5678:9abc:def0:4422:1133:5577:0000',
            dst_ip_mask='FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:0000',
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_PERMIT,
            priority = self.low_prio,
            table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry2)
        table_name = SWITCH_DEVICE_ATTR_TABLE_IPV6_ACL
        if self.client.is_feature_enable(SWITCH_FEATURE_SHARED_INGRESS_IP_ACL):
            table_name = SWITCH_DEVICE_ATTR_TABLE_IP_ACL
        pre_hw_table_count = get_hw_table_usage(self, table_name)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, acl_table)
        post_hw_table_count = get_hw_table_usage(self, table_name)
        self.assertEqual(post_hw_table_count - pre_hw_table_count, 2)
        route0 = self.add_route(self.device, ip_prefix='1234:5678:9abc:def0:4422:1133:5577:0000/112',
                                vrf_handle=self.vrf10, nexthop_handle=self.nhop1)

        def TestTraffic():
            print(" Testing Traffic")
            pre_counter = self.object_counters_get(acl_entry1)
            send_packet(self, self.devports[0], self.pkt_v6)
            verify_no_other_packets(self, timeout=2)
            post_counter = self.object_counters_get(acl_entry1)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)

            acl_entry2_pkt_v6 = simple_tcpv6_packet(pktlen=pkt_len,
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:88aa',
                ipv6_src='2000::1',
                ipv6_hlim=64)
            acl_entry2_exp_pkt_v6 = simple_tcpv6_packet(pktlen=pkt_len,
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
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            # remove the acl_entry1, acl_entry2  and table
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
    ############################################################################

    def AclRangeTest(self):
        print("AclRangeTest()")
        if not (self.client.is_feature_enable(SWITCH_FEATURE_INGRESS_L4_PORT_RANGE) and self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_L4_PORT_RANGE)):
            print("L4 port range feature not enabled, skipping")
            return

        src = switcht_range_t(min=3000, max=4000)
        dst = switcht_range_t(min=5000, max=6000)
        src_range = self.add_acl_range(self.device,
            type=SWITCH_ACL_RANGE_ATTR_TYPE_SRC_PORT, range=src)
        dst_range = self.add_acl_range(self.device,
            type=SWITCH_ACL_RANGE_ATTR_TYPE_DST_PORT, range=dst)

        table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IPV4
        if self.client.is_feature_enable(SWITCH_FEATURE_SHARED_INGRESS_IP_ACL):
            table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IP


        def create_table_antries(self, direction, src_range, dst_range):
            acl_table = self.add_acl_table(self.device,
                type=table_type,
                bind_point_type=[acl_table_bp_port],
                direction=direction)
            self.acl_entry1 = self.add_acl_entry(self.device,
                src_ip='192.168.0.1',
                src_ip_mask='255.255.255.255',
                dst_ip='10.10.10.1',
                dst_ip_mask='255.255.255.255',
                dst_port_range_id=dst_range,
                priority = self.low_prio,
                packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
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
                table_handle=acl_table)
            self.client.object_counters_clear_all(self.acl_entry3)
            self.acl_entry4 = self.add_acl_entry(self.device,
                src_ip='192.168.0.5',
                src_ip_mask='255.255.255.255',
                priority = self.lower_prio,
                packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
                table_handle=acl_table)
            self.client.object_counters_clear_all(self.acl_entry4)
            return (acl_table, self.acl_entry1, self.acl_entry2, self.acl_entry3, self.acl_entry4)


        def TestTraffic():
            print(" Testing Traffic")
            # Must match IP, lou-dst in entry 1
            pkt1 = simple_tcp_packet(pktlen=pkt_len,
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='10.10.10.1',
                ip_src='192.168.0.1',
                tcp_sport=2333,
                tcp_dport=5555,
                ip_id=105,
                ip_ttl=64)
            # Must match IP, lou-src in entry 2
            pkt2 = simple_tcp_packet(pktlen=pkt_len,
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='10.10.10.1',
                ip_src='192.168.0.1',
                tcp_sport=3333,
                tcp_dport=6333,
                ip_id=105,
                ip_ttl=64)
            # Must match IP, lou-src, lou-dst in entry 3
            pkt3 = simple_tcp_packet(pktlen=pkt_len,
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='10.10.10.1',
                ip_src='192.168.0.1',
                tcp_sport=3333,
                tcp_dport=5555,
                ip_id=105,
                ip_ttl=64)
            # Must match only IP in entry 4
            pkt4 = simple_tcp_packet(pktlen=pkt_len,
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
                    if self.client.is_feature_enable(SWITCH_FEATURE_SHARED_INGRESS_IP_ACL):
                        table_name = SWITCH_DEVICE_ATTR_TABLE_IP_ACL
                    pre_hw_table_count = get_hw_table_usage(self, table_name)
                    self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, t)
                    post_hw_table_count = get_hw_table_usage(self, table_name)
                    self.assertEqual(post_hw_table_count - pre_hw_table_count, 4)
                    TestTraffic()
                    self.startFrWrReplay(1, TestTraffic)
                finally:
                    self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
                    #  remove the acl entry and table
                    self.cleanlast()
                    self.cleanlast()
                    self.cleanlast()
                    self.cleanlast()
                    self.cleanlast()

            if self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_L4_PORT_RANGE) and self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_IP_ACL):
                try:
                    t, e1, e2, e3, e4 = create_table_antries(self, SWITCH_ACL_TABLE_ATTR_DIRECTION_EGRESS, src_range, dst_range)
                    table_name = SWITCH_DEVICE_ATTR_TABLE_EGRESS_IPV4_ACL
                    pre_hw_table_count = get_hw_table_usage(self, table_name)
                    self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, t)
                    post_hw_table_count = get_hw_table_usage(self, table_name)
                    self.assertEqual(post_hw_table_count - pre_hw_table_count, 4)
                    TestTraffic()
                    self.startFrWrReplay(1, TestTraffic)
                finally:
                    self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, 0)
                    # remove the acl entry and table
                    self.cleanlast()
                    self.cleanlast()
                    self.cleanlast()
                    self.cleanlast()
                    self.cleanlast()
        finally:
            self.cleanlast()
            self.cleanlast()
    ############################################################################

    def AclPriorityRangeVlanBPointTest(self):
        print("AclPriorityRangeVlanBPointTest()")

        dst = switcht_range_t(min=50000, max=60000)
        dst_range = self.add_acl_range(self.device,
            type=SWITCH_ACL_RANGE_ATTR_TYPE_DST_PORT, range=dst)

        table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IPV4
        if self.client.is_feature_enable(SWITCH_FEATURE_SHARED_INGRESS_IP_ACL):
            table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IP

        self.vlan50 = self.add_vlan(self.device, vlan_id=50)
        self.attribute_set(self.vlan50, SWITCH_VLAN_ATTR_LEARNING, False)
        self.add_vlan_member(self.device, vlan_handle=self.vlan50, member_handle=self.port12)
        self.add_vlan_member(self.device, vlan_handle=self.vlan50, member_handle=self.port13)
        self.attribute_set(self.port12, SWITCH_PORT_ATTR_PORT_VLAN_ID, 50)
        self.attribute_set(self.port13, SWITCH_PORT_ATTR_PORT_VLAN_ID, 50)


        def create_table_antries(self, direction, dst_range):
            '''
            [SWI-5004] Testing the functionality of below ACL entries using the existing bd_label approach
            Program 3 egress acl entries [using acl range] with decreasing priority [same acl table 1]
            Attach Acl table 1 to both vlans 50, 20
                Match VLAN=50, IP_PROTOCOL: 17,  L4_DST_PORT_RANGE: 50000-60000, Action Forw
                Match VLAN=20, SRC_IP: 1.1.1.1, Action Forw
                Match VLAN=20, Action drop
            '''
            acl_table1 = self.add_acl_table(self.device,
                type=table_type,
                bind_point_type=[acl_table_bp_vlan],
                direction=direction)
            acl_table2 = self.add_acl_table(self.device,
                type=table_type,
                bind_point_type=[acl_table_bp_vlan],
                direction=direction)

            self.acl_entry1 = self.add_acl_entry(self.device,
                ip_proto=17,
                ip_proto_mask=127,
                dst_port_range_id=dst_range,
                priority = self.high_prio,
                packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_PERMIT,
                table_handle=acl_table1)
            self.client.object_counters_clear_all(self.acl_entry1)
            self.acl_entry2 = self.add_acl_entry(self.device,
                dst_ip='1.1.1.1',
                dst_ip_mask='255.255.255.255',
                priority = self.low_prio,
                packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_PERMIT,
                table_handle=acl_table1)
            self.client.object_counters_clear_all(self.acl_entry2)
            self.acl_entry3 = self.add_acl_entry(self.device,
                priority = self.lower_prio,
                packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
                table_handle=acl_table1)
            self.client.object_counters_clear_all(self.acl_entry3)
            return (acl_table1, acl_table2)


        def TestTraffic():
            print(" Testing Traffic")
            vlan50_permit_pkt = simple_udp_packet(pktlen=pkt_len,
                eth_dst='00:44:44:44:44:44',
                eth_src='00:44:44:44:44:48',
                ip_dst='5.5.5.5',
                ip_src='192.168.0.1',
                udp_dport=55000,
                ip_ttl=64)
            vlan20_permit_pkt = simple_tcp_packet(pktlen=pkt_len,
                eth_dst='00:66:66:66:66:66',
                eth_src='00:77:77:77:77:77',
                ip_dst='1.1.1.1',
                ip_src='192.168.0.1',
                tcp_dport=60500,
                ip_ttl=64)
            vlan20_drop_pkt = simple_tcp_packet(pktlen=pkt_len,
                eth_dst='00:66:66:66:66:66',
                eth_src='00:77:77:77:77:77',
                ip_dst='2.2.2.2',
                ip_src='192.168.0.1',
                tcp_dport=55000,
                ip_ttl=64)

            pre_counter1 = self.object_counters_get(self.acl_entry1)
            pre_counter2 = self.object_counters_get(self.acl_entry2)
            pre_counter3 = self.object_counters_get(self.acl_entry3)

            send_packet(self, self.devports[12], vlan50_permit_pkt)
            verify_packets(self, vlan50_permit_pkt, [self.devports[13]])
            post_counter1 = self.object_counters_get(self.acl_entry1)
            post_counter2 = self.object_counters_get(self.acl_entry2)
            post_counter3 = self.object_counters_get(self.acl_entry3)
            self.assertEqual(post_counter1[0].count - pre_counter1[0].count, 1)
            self.assertEqual(post_counter2[0].count - pre_counter2[0].count, 0)
            self.assertEqual(post_counter3[0].count - pre_counter3[0].count, 0)

            send_packet(self, self.devports[11], vlan20_permit_pkt)
            verify_packets(self, vlan20_permit_pkt, [self.devports[10]])
            post_counter1 = self.object_counters_get(self.acl_entry1)
            post_counter2 = self.object_counters_get(self.acl_entry2)
            post_counter3 = self.object_counters_get(self.acl_entry3)
            self.assertEqual(post_counter1[0].count - pre_counter1[0].count, 1)
            self.assertEqual(post_counter2[0].count - pre_counter2[0].count, 1)
            self.assertEqual(post_counter3[0].count - pre_counter3[0].count, 0)

            send_packet(self, self.devports[11], vlan20_drop_pkt)
            verify_no_other_packets(self, timeout=2)
            post_counter1 = self.object_counters_get(self.acl_entry1)
            post_counter2 = self.object_counters_get(self.acl_entry2)
            post_counter3 = self.object_counters_get(self.acl_entry3)
            self.assertEqual(post_counter1[0].count - pre_counter1[0].count, 1)
            self.assertEqual(post_counter2[0].count - pre_counter2[0].count, 1)
            self.assertEqual(post_counter3[0].count - pre_counter3[0].count, 1)

        try:
            if (self.client.is_feature_enable(SWITCH_FEATURE_INGRESS_L4_PORT_RANGE) and
                self.client.is_feature_enable(SWITCH_FEATURE_BD_LABEL_IN_INGRESS_ACL)):
                try:
                    t1, t2 = create_table_antries(self, SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS, dst_range)
                    table_name = SWITCH_DEVICE_ATTR_TABLE_IPV4_ACL
                    if self.client.is_feature_enable(SWITCH_FEATURE_SHARED_INGRESS_IP_ACL):
                        table_name = SWITCH_ACL_TABLE_ATTR_TYPE_IP
                    pre_hw_table_count = get_hw_table_usage(self, table_name)
                    self.attribute_set(self.vlan50, SWITCH_VLAN_ATTR_INGRESS_ACL_HANDLE, t1)
                    self.attribute_set(self.vlan20, SWITCH_VLAN_ATTR_INGRESS_ACL_HANDLE, t1)
                    post_hw_table_count = get_hw_table_usage(self, table_name)
                    self.assertEqual(post_hw_table_count - pre_hw_table_count, 3)
                    TestTraffic()
                    self.startFrWrReplay(1, TestTraffic)
                finally:
                    self.attribute_set(self.vlan50, SWITCH_VLAN_ATTR_INGRESS_ACL_HANDLE, 0)
                    self.attribute_set(self.vlan20, SWITCH_VLAN_ATTR_INGRESS_ACL_HANDLE, 0)
                    # remove the acl entry, table
                    self.cleanlast()
                    self.cleanlast()
                    self.cleanlast()
                    self.cleanlast()
                    self.cleanlast()

            '''
            Enable this PTF once SWI-5185 is fixed
            if (self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_L4_PORT_RANGE) and
                self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_IP_ACL) and
                self.client.is_feature_enable(SWITCH_FEATURE_BD_LABEL_IN_EGRESS_ACL)):
                try:
                    t1, t2 = create_table_antries(self, SWITCH_ACL_TABLE_ATTR_DIRECTION_EGRESS, dst_range)
                    self.attribute_set(self.vlan50, SWITCH_VLAN_ATTR_EGRESS_ACL_HANDLE, t1)
                    self.attribute_set(self.vlan20, SWITCH_VLAN_ATTR_EGRESS_ACL_HANDLE, t1)
                    TestTraffic()
                    self.startFrWrReplay(1, TestTraffic)
                finally:
                    self.attribute_set(self.vlan50, SWITCH_VLAN_ATTR_EGRESS_ACL_HANDLE, 0)
                    self.attribute_set(self.vlan20, SWITCH_VLAN_ATTR_EGRESS_ACL_HANDLE, 0)
                    # remove the acl entry, table
                    self.cleanlast()
                    self.cleanlast()
                    self.cleanlast()
                    self.cleanlast()
                    self.cleanlast()
            '''

        finally:
            self.attribute_set(self.port12, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.port13, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

    ############################################################################

    def IPv4AclRedirectTest(self):
        '''
        This test tries to verify ipv4 racl entries
        We send a packet from port 0 and redirect to port 3
        The bp_type is PORT. The port_lag_label comes from the table id
        '''
        print("IPv4AclRedirectTest()")
        if (self.client.is_feature_enable(SWITCH_FEATURE_ACL_REDIRECT_NEXTHOP) == 0):
            print("ACL redirect feature not enabled, skipping")
            return
        table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IPV4
        if self.client.is_feature_enable(SWITCH_FEATURE_SHARED_INGRESS_IP_ACL):
            table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IP

        acl_table = self.add_acl_table(self.device,
            type=table_type,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_entry = self.add_acl_entry(self.device,
            dst_ip='10.10.10.1',
            dst_ip_mask='255.255.255.255',
            redirect=self.nhop2,
            table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry)
        table_name = SWITCH_DEVICE_ATTR_TABLE_IPV4_ACL
        if self.client.is_feature_enable(SWITCH_FEATURE_SHARED_INGRESS_IP_ACL):
            table_name = SWITCH_DEVICE_ATTR_TABLE_IP_ACL
        pre_hw_table_count = get_hw_table_usage(self, table_name)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, acl_table)
        post_hw_table_count = get_hw_table_usage(self, table_name)
        self.assertEqual(post_hw_table_count - pre_hw_table_count, 1)

        def TestTraffic():
            print(" Testing Traffic")
            self.local_exp_pkt = simple_tcp_packet(pktlen=pkt_len,
                eth_dst='00:11:22:33:44:56',
                eth_src='00:77:66:55:44:33',
                ip_dst='10.10.10.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_ttl=63)
            pre_counter = self.object_counters_get(acl_entry)
            send_packet(self, self.devports[0], self.pkt)
            verify_packet(self, self.local_exp_pkt, self.devports[3])
            verify_no_other_packets(self, timeout=2)
            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)

        try:
            TestTraffic()
            self.startFrWrReplay(1, TestTraffic)
        finally:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            # remove the acl entry and table
            self.cleanlast()
            self.cleanlast()
    ############################################################################

    def IPv4RaclTableTest(self):
        '''
        This test tries to verify ipv4 racl entries
        We send a packet from port 0 and drop the packet
        The bp_type is PORT. The port_lag_label comes from the table id
        '''
        print("IPv4RAclTableTest()")
        if (self.client.is_feature_enable(SWITCH_FEATURE_INGRESS_IPV4_RACL) == 0):
            print("IPV4 RACL feature not enabled, skipping")
            return
        acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IPV4_RACL,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_entry = self.add_acl_entry(self.device,
            dst_ip='10.10.10.1',
            dst_ip_mask='255.255.255.255',
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, acl_table)

        def TestTraffic():
            print(" Testing Traffic")
            pre_counter = self.object_counters_get(acl_entry)
            send_packet(self, self.devports[0], self.pkt)
            verify_no_other_packets(self, timeout=2)
            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)

        try:
            TestTraffic()
            self.startFrWrReplay(1, TestTraffic)
        finally:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            # remove the acl entry and table
            self.cleanlast()
            self.cleanlast()
    ############################################################################

    def IPv4RaclTablePriorityTest(self):
        '''
        This test tries to veriry v4 racl table priority
        '''
        print("IPv4RaclTablePriorityTest()")
        if (self.client.is_feature_enable(SWITCH_FEATURE_INGRESS_IPV4_RACL) == 0):
            print("IPV4 RACL feature not enabled, skipping")
            return
        acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IPV4_RACL,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_entry1= self.add_acl_entry(self.device,
            dst_ip='10.10.10.1',
            dst_ip_mask='255.255.255.255',
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
             priority = self.high_prio,
            table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry1)
        acl_entry2 = self.add_acl_entry(self.device,
            dst_ip='10.10.10.0',
            dst_ip_mask='255.255.255.0',
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_PERMIT,
            priority = self.low_prio,
            table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry2)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, acl_table)
        route0 = self.add_route(self.device, ip_prefix='10.10.10.0/24', vrf_handle=self.vrf10, nexthop_handle=self.nhop1)

        def TestTraffic():
            print(" Testing Traffic")
            pre_counter = self.object_counters_get(acl_entry1)
            send_packet(self, self.devports[0], self.pkt)
            verify_no_other_packets(self, timeout=2)
            post_counter = self.object_counters_get(acl_entry1)

            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)
            acl_entry2_pkt = simple_tcp_packet(pktlen=pkt_len,
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='10.10.10.24',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_ttl=64)
            acl_entry2_exp_pkt = simple_tcp_packet(pktlen=pkt_len,
                eth_dst='00:11:22:33:44:55',
                eth_src='00:77:66:55:44:33',
                ip_dst='10.10.10.24',
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
            self.startFrWrReplay(0, TestTraffic) # WI or FI requires fix
        finally:
            # cleanup route0
            self.cleanlast()
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            # remove the acl_entry1, acl_entry2 and table
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
    ############################################################################

    def IPv4RaclRedirectTest(self):
        '''
        This test tries to verify ipv4 racl entries
        We send a packet from port 0 and redirect to port 3
        The bp_type is PORT. The port_lag_label comes from the table id
        '''
        print("IPv4RAclRedirectTest()")
        if ((self.client.is_feature_enable(SWITCH_FEATURE_ACL_REDIRECT_NEXTHOP) == 0) or (self.client.is_feature_enable(SWITCH_FEATURE_INGRESS_IPV4_RACL) == 0)):
            print("Either RACL or ACL redirect feature not enabled, skipping")
            return
        acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IPV4_RACL,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_entry = self.add_acl_entry(self.device,
            dst_ip='10.10.10.1',
            dst_ip_mask='255.255.255.255',
            redirect=self.nhop2,
            table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, acl_table)

        def TestTraffic():
            print(" Testing Traffic")
            self.local_exp_pkt = simple_tcp_packet(pktlen=pkt_len,
                eth_dst='00:11:22:33:44:56',
                eth_src='00:77:66:55:44:33',
                ip_dst='10.10.10.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_ttl=63)
            pre_counter = self.object_counters_get(acl_entry)
            send_packet(self, self.devports[0], self.pkt)
            verify_packet(self, self.local_exp_pkt, self.devports[3])
            verify_no_other_packets(self, timeout=2)
            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)

        try:
            TestTraffic()
            self.startFrWrReplay(1, TestTraffic)
        finally:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            # remove the acl entry and table
            self.cleanlast()
            self.cleanlast()
    ############################################################################

    def IPv6RaclTableTest(self):
        '''
        This test tries to verify ipv6 racl entries
        We send a packet from port 0 and drop the packet
        The bp_type is PORT. The port_lag_label comes from the table id
        '''
        print("IPv6RAclTableTest()")
        if (self.client.is_feature_enable(SWITCH_FEATURE_INGRESS_IPV6_RACL) == 0):
            print("IPV6 RACL feature not enabled, skipping")
            return
        acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IPV6_RACL,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_entry = self.add_acl_entry(self.device,
            dst_ip='1234:5678:9abc:def0:4422:1133:5577:99aa',
            dst_ip_mask='FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF',
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, acl_table)

        def TestTraffic():
            print(" Testing Traffic")
            pre_counter = self.object_counters_get(acl_entry)
            send_packet(self, self.devports[0], self.pkt_v6)
            verify_no_other_packets(self, timeout=2)
            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)

        try:
            TestTraffic()
            self.startFrWrReplay(1, TestTraffic)
        finally:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            # remove the acl entry and table
            self.cleanlast()
            self.cleanlast()
    ############################################################################

    def IPv6RaclTablePriorityTest(self):
        '''
        This test tries to veriry v6 racl table priority
        '''
        print("IPv6RaclTablePriorityTest()")
        if (self.client.is_feature_enable(SWITCH_FEATURE_INGRESS_IPV6_RACL) == 0):
            print("IPV6 RACL feature not enabled, skipping")
            return
        acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IPV6_RACL,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_entry1 = self.add_acl_entry(self.device,
            dst_ip='1234:5678:9abc:def0:4422:1133:5577:99aa',
            dst_ip_mask='FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF',
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            priority = self.high_prio,
            table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry1)
        acl_entry2 = self.add_acl_entry(self.device,
            dst_ip='1234:5678:9abc:def0:4422:1133:5577:0000',
            dst_ip_mask='FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:0000',
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_PERMIT,
            priority = self.low_prio,
            table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry2)
        route0 = self.add_route(self.device, ip_prefix='1234:5678:9abc:def0:4422:1133:5577:0000/112',
                      vrf_handle=self.vrf10, nexthop_handle=self.nhop1)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, acl_table)

        def TestTraffic():
            print(" Testing Traffic")
            pre_counter = self.object_counters_get(acl_entry1)
            send_packet(self, self.devports[0], self.pkt_v6)
            verify_no_other_packets(self, timeout=2)
            post_counter = self.object_counters_get(acl_entry1)

            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)
            acl_entry2_pkt_v6 = simple_tcpv6_packet(pktlen=pkt_len,
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:88aa',
                ipv6_src='2000::1',
                ipv6_hlim=64)
            acl_entry2_exp_pkt_v6 = simple_tcpv6_packet(pktlen=pkt_len,
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
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            # remove the acl_entry1, acl_entry2 and table
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
    ############################################################################

    def L3BdLabelTest(self):
        print("L3BdLabelTest()")
        if(self.client.is_feature_enable(SWITCH_FEATURE_BD_LABEL_IN_INGRESS_ACL) == 0):
            print("Ingress ACL BD_LABEL feature is not supported.")
            return
        table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IPV4
        if self.client.is_feature_enable(SWITCH_FEATURE_SHARED_INGRESS_IP_ACL):
            table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IP

        acl_table = self.add_acl_table(self.device,
            type=table_type,
            bind_point_type=[acl_table_bp_rif],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_entry = self.add_acl_entry(self.device,
            bd_label=2,
            bd_label_mask=0xFF,
            dst_ip='10.10.10.1',
            dst_ip_mask='255.255.255.255',
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry)
        table_name = SWITCH_DEVICE_ATTR_TABLE_IPV4_ACL
        if self.client.is_feature_enable(SWITCH_FEATURE_SHARED_INGRESS_IP_ACL):
            table_name = SWITCH_DEVICE_ATTR_TABLE_IP_ACL
        pre_hw_table_count = get_hw_table_usage(self, table_name)
        self.attribute_set(self.rif0, SWITCH_RIF_ATTR_INGRESS_VLAN_RIF_LABEL, 2)
        self.attribute_set(self.rif0, SWITCH_RIF_ATTR_INGRESS_ACL_HANDLE, acl_table)
        post_hw_table_count = get_hw_table_usage(self, table_name)
        self.assertEqual(post_hw_table_count - pre_hw_table_count, 1)

        def TestTraffic():
            print(" Testing Traffic")
            pre_counter = self.object_counters_get(acl_entry)
            send_packet(self, self.devports[0], self.pkt)
            verify_no_other_packets(self, timeout=2)
            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)

        try:
            TestTraffic()
            self.startFrWrReplay(1, TestTraffic)
        finally:
            self.attribute_set(self.rif0, SWITCH_RIF_ATTR_INGRESS_VLAN_RIF_LABEL, 0)
            self.attribute_set(self.rif0, SWITCH_RIF_ATTR_INGRESS_ACL_HANDLE, 0)
            # remove the acl entry and table
            self.cleanlast()
            self.cleanlast()
    ############################################################################

    def SVIAclTest(self):
        print("SVIAclTest()")
        if(self.client.is_feature_enable(SWITCH_FEATURE_BD_LABEL_IN_INGRESS_ACL) == 0):
            print("Ingress ACL BD_LABEL feature is not supported.")
            return
        table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IPV4
        if self.client.is_feature_enable(SWITCH_FEATURE_SHARED_INGRESS_IP_ACL):
            table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IP

        acl_table = self.add_acl_table(self.device,
            type=table_type,
            bind_point_type=[acl_table_bp_rif],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_entry = self.add_acl_entry(self.device,
            dst_ip='10.10.10.1',
            dst_ip_mask='255.255.255.255',
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry)
        table_name = SWITCH_DEVICE_ATTR_TABLE_IPV4_ACL
        if self.client.is_feature_enable(SWITCH_FEATURE_SHARED_INGRESS_IP_ACL):
            table_name = SWITCH_DEVICE_ATTR_TABLE_IP_ACL
        pre_hw_table_count = get_hw_table_usage(self, table_name)
        self.attribute_set(self.rif4, SWITCH_RIF_ATTR_INGRESS_ACL_HANDLE, acl_table)
        post_hw_table_count = get_hw_table_usage(self, table_name)
        self.assertEqual(post_hw_table_count - pre_hw_table_count, 1)

        def TestTraffic():
            print(" Testing Traffic")
            pre_counter = self.object_counters_get(acl_entry)
            send_packet(self, self.devports[2], self.pkt)
            verify_no_other_packets(self, timeout=2)
            send_packet(self, self.devports[4], self.pkt)
            verify_no_other_packets(self, timeout=2)
            send_packet(self, self.devports[5], self.pkt)
            verify_no_other_packets(self, timeout=2)
            send_packet(self, self.devports[8], self.pkt)
            verify_no_other_packets(self, timeout=2)
            send_packet(self, self.devports[9], self.pkt)
            verify_no_other_packets(self, timeout=2)
            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 5)

        try:
            TestTraffic()
            self.startFrWrReplay(1, TestTraffic)
        finally:
            self.attribute_set(self.rif4, SWITCH_RIF_ATTR_INGRESS_ACL_HANDLE, 0)
            # remove the acl entry and table
            self.cleanlast()
            self.cleanlast()
    ############################################################################

    def SVIBdLabelTest(self):
        print("SVIBdLabelTest()")
        table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IPV4
        if self.client.is_feature_enable(SWITCH_FEATURE_SHARED_INGRESS_IP_ACL):
            table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IP

        acl_table = self.add_acl_table(self.device,
            type=table_type,
            bind_point_type=[acl_table_bp_rif],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_entry = self.add_acl_entry(self.device,
            bd_label=2,
            bd_label_mask=0xFF,
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry)
        table_name = SWITCH_DEVICE_ATTR_TABLE_IPV4_ACL
        if self.client.is_feature_enable(SWITCH_FEATURE_SHARED_INGRESS_IP_ACL):
            table_name = SWITCH_DEVICE_ATTR_TABLE_IP_ACL
        pre_hw_table_count = get_hw_table_usage(self, table_name)
        self.attribute_set(self.rif4, SWITCH_RIF_ATTR_INGRESS_VLAN_RIF_LABEL, 2)
        self.attribute_set(self.rif4, SWITCH_RIF_ATTR_INGRESS_ACL_HANDLE, acl_table)
        post_hw_table_count = get_hw_table_usage(self, table_name)
        self.assertEqual(post_hw_table_count - pre_hw_table_count, 1)

        def TestTraffic():
            print(" Testing Traffic")
            pre_counter = self.object_counters_get(acl_entry)
            send_packet(self, self.devports[8], self.l2_pkt1)
            verify_no_other_packets(self, timeout=2)
            send_packet(self, self.devports[9], self.l2_pkt1)
            verify_no_other_packets(self, timeout=2)
            send_packet(self, self.devports[4], self.l2_pkt1)
            verify_no_other_packets(self, timeout=2)
            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 3)

        try:
            TestTraffic()
            self.startFrWrReplay(1, TestTraffic)
        finally:
            self.attribute_set(self.rif4, SWITCH_RIF_ATTR_INGRESS_VLAN_RIF_LABEL, 0)
            self.attribute_set(self.rif4, SWITCH_RIF_ATTR_INGRESS_ACL_HANDLE, 0)
            # remove the acl entry and table
            self.cleanlast()
            self.cleanlast()
    ############################################################################

    def VlanAclTest(self):
        print("VlanAclTest()")
        table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IPV4
        if self.client.is_feature_enable(SWITCH_FEATURE_SHARED_INGRESS_IP_ACL):
            table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IP

        acl_table = self.add_acl_table(self.device,
            type=table_type,
            bind_point_type=[acl_table_bp_vlan],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_entry = self.add_acl_entry(self.device,
            dst_ip='20.20.20.1',
            dst_ip_mask='255.255.255.255',
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry)
        table_name = SWITCH_DEVICE_ATTR_TABLE_IPV4_ACL
        if self.client.is_feature_enable(SWITCH_FEATURE_SHARED_INGRESS_IP_ACL):
            table_name = SWITCH_DEVICE_ATTR_TABLE_IP_ACL
        pre_hw_table_count = get_hw_table_usage(self, table_name)
        self.attribute_set(self.vlan20, SWITCH_VLAN_ATTR_INGRESS_ACL_HANDLE, acl_table)
        post_hw_table_count = get_hw_table_usage(self, table_name)
        self.assertEqual(post_hw_table_count - pre_hw_table_count, 1)

        def TestTraffic():
            print(" Testing Traffic")
            pre_counter = self.object_counters_get(acl_entry)
            send_packet(self, self.devports[11], self.vlan20_pkt1)
            verify_no_other_packets(self, timeout=2)
            send_packet(self, self.devports[10], self.vlan20_pkt2)
            verify_no_other_packets(self, timeout=2)
            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 2)

        try:
            TestTraffic()
            self.startFrWrReplay(0, TestTraffic)  # WI or FI not testing
        finally:
            self.attribute_set(self.vlan20, SWITCH_VLAN_ATTR_INGRESS_ACL_HANDLE, 0)
            # remove the acl entry and table
            self.cleanlast()
            self.cleanlast()
    ############################################################################

    def VlanBdLabelTest(self):
        print("VlanBdLabelTest()")
        if(self.client.is_feature_enable(SWITCH_FEATURE_BD_LABEL_IN_INGRESS_ACL) == 0):
            print("Ingress ACL BD_LABEL feature is not supported.")
            return
        table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IPV4
        if self.client.is_feature_enable(SWITCH_FEATURE_SHARED_INGRESS_IP_ACL):
            table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IP

        acl_table = self.add_acl_table(self.device,
            type=table_type,
            bind_point_type=[acl_table_bp_vlan],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_entry = self.add_acl_entry(self.device,
            bd_label=2,
            bd_label_mask=0xFF,
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry)
        table_name = SWITCH_DEVICE_ATTR_TABLE_IPV4_ACL
        if self.client.is_feature_enable(SWITCH_FEATURE_SHARED_INGRESS_IP_ACL):
            table_name = SWITCH_DEVICE_ATTR_TABLE_IP_ACL
        pre_hw_table_count = get_hw_table_usage(self, table_name)
        self.attribute_set(self.vlan20, SWITCH_VLAN_ATTR_INGRESS_VLAN_RIF_LABEL, 2)
        self.attribute_set(self.vlan20, SWITCH_VLAN_ATTR_INGRESS_ACL_HANDLE, acl_table)
        post_hw_table_count = get_hw_table_usage(self, table_name)
        self.assertEqual(post_hw_table_count - pre_hw_table_count, 1)

        def TestTraffic():
            print(" Testing Traffic")
            pre_counter = self.object_counters_get(acl_entry)
            send_packet(self, self.devports[10], self.vlan20_pkt1)
            verify_no_other_packets(self, timeout=2)
            send_packet(self, self.devports[11], self.vlan20_pkt1)
            verify_no_other_packets(self, timeout=2)
            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 2)

        try:
            TestTraffic()
            self.startFrWrReplay(1, TestTraffic)
        finally:
            self.attribute_set(self.vlan20, SWITCH_VLAN_ATTR_INGRESS_VLAN_RIF_LABEL, 0)
            self.attribute_set(self.vlan20, SWITCH_VLAN_ATTR_INGRESS_ACL_HANDLE, 0)
            # remove the acl entry and table
            self.cleanlast()
            self.cleanlast()
    ############################################################################

    def IngressOuterVlanIdv4AclTest(self):
        print("IngressOuterVlanIdv4AclTest()")
        if (self.client.is_feature_enable(SWITCH_FEATURE_BD_IN_INGRESS_ACL) == 0):
            print("Internal-Vlan/BD matching feature not enabled, skipping")
            return
        acl_table = self.add_acl_table(self.device,
                type=SWITCH_ACL_TABLE_ATTR_TYPE_IPV4,
                bind_point_type=[acl_table_bp_port],
                direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_entry = self.add_acl_entry(self.device,
                internal_vlan=20,
                internal_vlan_mask=0xFFF,
                packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
                table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry)
        table_name = SWITCH_DEVICE_ATTR_TABLE_IPV4_ACL
        pre_hw_table_count = get_hw_table_usage(self, table_name)
        self.attribute_set(self.port10, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, acl_table)
        self.attribute_set(self.port11, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, acl_table)
        post_hw_table_count = get_hw_table_usage(self, table_name)
        self.assertEqual(post_hw_table_count - pre_hw_table_count, 1)

        def TestTraffic():
            print(" Testing Traffic")
            pre_counter = self.client.object_counters_get(acl_entry)
            send_packet(self, self.devports[11], self.vlan20_pkt1)
            verify_no_other_packets(self, timeout=2)
            send_packet(self, self.devports[10], self.vlan20_pkt2)
            verify_no_other_packets(self, timeout=2)
            post_counter = self.client.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 2)

        try:
            TestTraffic()
            self.startFrWrReplay(0, TestTraffic)  # WI or FI not testing
        finally:
            self.attribute_set(self.port10, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            self.attribute_set(self.port11, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            # remove the acl entry and table
            self.cleanlast()
            self.cleanlast()
    ############################################################################

    def IngressOuterVlanIdv6AclTest(self):
        print("IngressOuterVlanIdv6AclTest()")
        if (self.client.is_feature_enable(SWITCH_FEATURE_BD_IN_INGRESS_ACL) == 0):
            print("Internal-Vlan/BD matching feature not enabled, skipping")
            return
        acl_table = self.add_acl_table(self.device,
                type=SWITCH_ACL_TABLE_ATTR_TYPE_IPV6,
                bind_point_type=[acl_table_bp_port],
                direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_entry = self.add_acl_entry(self.device,
            dst_ip='1234:5678:9abc:def0:4422:1133:5577:99aa',
            dst_ip_mask='FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF',
            internal_vlan=20,
            internal_vlan_mask=0xFFF,
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry)
        table_name = SWITCH_DEVICE_ATTR_TABLE_IPV6_ACL
        pre_hw_table_count = get_hw_table_usage(self, table_name)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, acl_table)
        post_hw_table_count = get_hw_table_usage(self, table_name)
        self.assertEqual(post_hw_table_count - pre_hw_table_count, 1)

        def TestTraffic():
            print(" Testing Traffic")
            outer_vlan_pkt_v6 = simple_tcpv6_packet(pktlen=pkt_len,
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                dl_vlan_enable=True,
                vlan_vid=20,
                ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
                ipv6_src='2000::1',
                ipv6_hlim=64)
            pre_counter = self.client.object_counters_get(acl_entry)
            send_packet(self, self.devports[0], outer_vlan_pkt_v6)
            verify_no_other_packets(self, timeout=2)
            post_counter = self.client.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)

        try:
            TestTraffic()
            self.startFrWrReplay(0, TestTraffic)  # WI or FI not testing
        finally:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            # remove the acl entry and table
            self.cleanlast()
            self.cleanlast()
    ############################################################################

    def IngressOuterVlanIdMirrorAclTest(self):
        print("IngressOuterVlanIdMirrorAclTest()")
        if (self.client.is_feature_enable(SWITCH_FEATURE_BD_IN_INGRESS_ACL) == 0):
            print("Internal-Vlan/BD matching feature not enabled, skipping")
            return
        acl_table = self.add_acl_table(self.device,
                type=SWITCH_ACL_TABLE_ATTR_TYPE_IP_MIRROR,
                bind_point_type=[acl_table_bp_port],
                direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_entry = self.add_acl_entry(self.device,
                internal_vlan=20,
                internal_vlan_mask=0xFFF,
                packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
                table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry)
        table_name = SWITCH_DEVICE_ATTR_TABLE_IP_MIRROR_ACL
        pre_hw_table_count = get_hw_table_usage(self, table_name)
        self.attribute_set(self.port10, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, acl_table)
        self.attribute_set(self.port11, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, acl_table)
        post_hw_table_count = get_hw_table_usage(self, table_name)
        self.assertEqual(post_hw_table_count - pre_hw_table_count, 1)

        def TestTraffic():
            print(" Testing Traffic")
            pre_counter = self.client.object_counters_get(acl_entry)
            send_packet(self, self.devports[11], self.vlan20_pkt1)
            verify_no_other_packets(self, timeout=2)
            send_packet(self, self.devports[10], self.vlan20_pkt2)
            verify_no_other_packets(self, timeout=2)
            post_counter = self.client.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 2)

        try:
            TestTraffic()
            self.startFrWrReplay(0, TestTraffic)  # WI or FI not testing
        finally:
            self.attribute_set(self.port10, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            self.attribute_set(self.port11, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            # remove the acl entry and table
            self.cleanlast()
            self.cleanlast()
    ############################################################################

    def EgressOuterVlanIdv4AclTest(self):
        print("EgressOuterVlanIdv4AclTest()")
        if (self.client.is_feature_enable(SWITCH_FEATURE_BD_IN_EGRESS_ACL) == 0):
            print("Internal-Vlan/BD matching feature not enabled, skipping")
            return
        acl_table = self.add_acl_table(self.device,
                type=SWITCH_ACL_TABLE_ATTR_TYPE_IPV4,
                bind_point_type=[acl_table_bp_port],
                direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_EGRESS)
        acl_entry = self.add_acl_entry(self.device,
                internal_vlan=20,
                internal_vlan_mask=0xFFF,
                packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
                table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry)
        table_name = SWITCH_DEVICE_ATTR_TABLE_IPV4_ACL
        pre_hw_table_count = get_hw_table_usage(self, table_name)
        self.attribute_set(self.port10, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, acl_table)
        self.attribute_set(self.port11, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, acl_table)

        def TestTraffic():
            print(" Testing Traffic")
            pre_counter = self.client.object_counters_get(acl_entry)
            send_packet(self, self.devports[11], self.vlan20_pkt1)
            verify_no_other_packets(self, timeout=2)
            send_packet(self, self.devports[10], self.vlan20_pkt2)
            verify_no_other_packets(self, timeout=2)
            post_counter = self.client.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 2)

        try:
            TestTraffic()
            self.startFrWrReplay(0, TestTraffic)  # WI or FI not testing
        finally:
            self.attribute_set(self.port10, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, 0)
            self.attribute_set(self.port11, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, 0)
            # remove the acl entry and table
            self.cleanlast()
            self.cleanlast()
    ############################################################################

    def EgressOuterVlanIdv6AclTest(self):
        print("EgressOuterVlanIdv6AclTest()")
        if (self.client.is_feature_enable(SWITCH_FEATURE_BD_IN_EGRESS_ACL) == 0):
            print("Internal-Vlan/BD matching feature not enabled, skipping")
            return
        acl_table = self.add_acl_table(self.device,
                type=SWITCH_ACL_TABLE_ATTR_TYPE_IPV6,
                bind_point_type=[acl_table_bp_port],
                direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_EGRESS)
        acl_entry = self.add_acl_entry(self.device,
            dst_ip='1234:5678:9abc:def0:4422:1133:5577:99aa',
            dst_ip_mask='FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF',
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry)
        table_name = SWITCH_DEVICE_ATTR_TABLE_IPV6_ACL
        pre_hw_table_count = get_hw_table_usage(self, table_name)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, acl_table)

        def TestTraffic():
            print(" Testing Traffic")
            outer_vlan_pkt_v6 = simple_tcpv6_packet(pktlen=pkt_len,
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                vlan_vid=20,
                dl_vlan_enable=True,
                ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
                ipv6_src='2000::1',
                ipv6_hlim=64)
            pre_counter = self.client.object_counters_get(acl_entry)
            send_packet(self, self.devports[0], outer_vlan_pkt_v6)
            verify_no_other_packets(self, timeout=2)
            post_counter = self.client.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)

        try:
            TestTraffic()
            self.startFrWrReplay(0, TestTraffic)  # WI or FI not testing
        finally:
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, 0)
            # remove the acl entry and table
            self.cleanlast()
            self.cleanlast()
    ############################################################################

    def EgressOuterVlanIdMirrorAclTest(self):
        print("EgressOuterVlanIdMirrorAclTest()")
        if (self.client.is_feature_enable(SWITCH_FEATURE_BD_IN_EGRESS_ACL) == 0):
            print("Internal-Vlan/BD matching feature not enabled, skipping")
            return
        acl_table = self.add_acl_table(self.device,
                type=SWITCH_ACL_TABLE_ATTR_TYPE_IP_MIRROR,
                bind_point_type=[acl_table_bp_port],
                direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_EGRESS)
        acl_entry = self.add_acl_entry(self.device,
                internal_vlan=20,
                internal_vlan_mask=0xFFF,
                packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
                table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry)
        table_name = SWITCH_DEVICE_ATTR_TABLE_IP_MIRROR_ACL
        pre_hw_table_count = get_hw_table_usage(self, table_name)
        self.attribute_set(self.port10, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, acl_table)
        self.attribute_set(self.port11, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, acl_table)

        def TestTraffic():
            print(" Testing Traffic")
            pre_counter = self.client.object_counters_get(acl_entry)
            send_packet(self, self.devports[11], self.vlan20_pkt1)
            verify_no_other_packets(self, timeout=2)
            send_packet(self, self.devports[10], self.vlan20_pkt2)
            verify_no_other_packets(self, timeout=2)
            post_counter = self.client.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 2)

        try:
            TestTraffic()
            self.startFrWrReplay(0, TestTraffic)  # WI or FI not testing
        finally:
            self.attribute_set(self.port10, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, 0)
            self.attribute_set(self.port11, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, 0)
            # remove the acl entry and table
            self.cleanlast()
            self.cleanlast()
    ############################################################################

    def SimpleAclGroupTest(self):
        '''
        This test tries to verify ACL groups
        We send a packet from port 0 and port 1 and drop in both cases
        Create a group/table of bp_type PORT
        Create another group/table of bp_type VLAN
        '''
        print("SimpleAclGroupTest()")
        if (self.client.is_feature_enable(SWITCH_FEATURE_INGRESS_MAC_ACL) == 0):
            print("MAC ACL feature not enabled, skipping")
            return
        table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IPV4
        if self.client.is_feature_enable(SWITCH_FEATURE_SHARED_INGRESS_IP_ACL):
            table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IP

        # Create ACL groups
        acl_group1 = self.add_acl_group(self.device, bind_point_type=[acl_group_bp_vlan])
        acl_group2 = self.add_acl_group(self.device, bind_point_type=[acl_group_bp_port])

        # Create ACL tables and entries
        acl_table1 = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_MAC,
            bind_point_type=[acl_table_bp_vlan],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_entry11 = self.add_acl_entry(self.device,
            src_mac='00:22:22:22:22:25',
            src_mac_mask='ff:ff:ff:ff:ff:ff',
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            table_handle=acl_table1)
        self.client.object_counters_clear_all(acl_entry11)
        acl_table2 = self.add_acl_table(self.device,
            type=table_type,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_entry21 = self.add_acl_entry(self.device,
            dst_ip='10.10.10.1',
            dst_ip_mask='255.255.255.255',
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            table_handle=acl_table2)
        self.client.object_counters_clear_all(acl_entry21)

        table_name = SWITCH_DEVICE_ATTR_TABLE_IPV4_ACL
        if self.client.is_feature_enable(SWITCH_FEATURE_SHARED_INGRESS_IP_ACL):
            table_name = SWITCH_DEVICE_ATTR_TABLE_IP_ACL
        pre_hw_table_count = get_hw_table_usage(self, table_name)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, acl_group2)
        # Create ACL group members
        acl_grp_member1 = self.add_acl_group_member(self.device,
            acl_table_handle=acl_table1,
            acl_group_handle=acl_group1)
        acl_grp_member2 = self.add_acl_group_member(self.device,
            acl_table_handle=acl_table2,
            acl_group_handle=acl_group2)

        self.attribute_set(self.vlan20, SWITCH_VLAN_ATTR_INGRESS_ACL_HANDLE, acl_group1)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, acl_group2)
        post_hw_table_count = get_hw_table_usage(self, table_name)
        self.assertEqual(post_hw_table_count - pre_hw_table_count, 1)

        def TestTraffic():
            print(" Testing Traffic")
            pkt1 = simple_eth_packet(
                eth_dst='00:44:44:44:44:45',
                eth_src='00:22:22:22:22:25')
            pkt2 = simple_tcp_packet(pktlen=pkt_len,
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='10.10.10.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_ttl=64)
            send_packet(self, self.devports[10], pkt1)
            verify_no_other_packets(self, timeout=2)
            send_packet(self, self.devports[0], pkt2)
            verify_no_other_packets(self, timeout=2)

        try:
            TestTraffic()
            self.startFrWrReplay(1, TestTraffic)
        finally:
            self.attribute_set(self.vlan20, SWITCH_VLAN_ATTR_INGRESS_ACL_HANDLE, 0)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            # remove the acl entry and table
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
    ############################################################################

    def SharedAclTableTest(self):
        '''
        This test tries to verify ACL groups
        We send a packet from port 0 and port 1 and drop in both cases
        Create an table for ipv4
        Create 2 groups of bp_type RIF and use the same acl table
        '''
        print("SharedAclTableTest()")
        table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IPV4
        if self.client.is_feature_enable(SWITCH_FEATURE_SHARED_INGRESS_IP_ACL):
            table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IP

        # Create ACL groups
        acl_group1 = self.add_acl_group(self.device, bind_point_type=[acl_group_bp_rif])
        acl_group2 = self.add_acl_group(self.device, bind_point_type=[acl_group_bp_rif])

        # Create ACL tables and entries
        acl_table = self.add_acl_table(self.device,
            type=table_type,
            bind_point_type=[acl_table_bp_rif],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_entry = self.add_acl_entry(self.device,
            dst_ip='10.10.10.0',
            dst_ip_mask='255.255.255.0',
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry)

        # Create ACL group members
        acl_grp_member1 = self.add_acl_group_member(self.device,
            acl_table_handle=acl_table,
            acl_group_handle=acl_group1)
        acl_grp_member2 = self.add_acl_group_member(self.device,
            acl_table_handle=acl_table,
            acl_group_handle=acl_group2)

        self.attribute_set(self.rif0, SWITCH_RIF_ATTR_INGRESS_ACL_HANDLE, acl_group1)
        self.attribute_set(self.rif1, SWITCH_RIF_ATTR_INGRESS_ACL_HANDLE, acl_group2)

        def TestTraffic():
            print(" Testing Traffic")
            pkt1 = simple_tcp_packet(pktlen=pkt_len,
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='10.10.10.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_ttl=64)
            pkt2 = simple_tcp_packet(pktlen=pkt_len,
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='10.10.10.2',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_ttl=64)
            # tx from port0
            send_packet(self, self.devports[0], pkt1)
            verify_no_other_packets(self, timeout=2)
            # tx from port0
            send_packet(self, self.devports[1], pkt2)
            verify_no_other_packets(self, timeout=2)

        try:
            TestTraffic()
            self.startFrWrReplay(1, TestTraffic)
        finally:
            self.attribute_set(self.rif0, SWITCH_RIF_ATTR_INGRESS_ACL_HANDLE, 0)
            self.attribute_set(self.rif1, SWITCH_RIF_ATTR_INGRESS_ACL_HANDLE, 0)
            # remove the acl entry and table
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
    ############################################################################

    def SharedAclGroupTest(self):
        '''
        This test tries to verify ACL groups with deny/permit
        ACL-group of bp_type PORT, have 2 tables of type IP v4, IP mirror
        Add entries to only 1 table
        We will verify if the correct label is being used
        '''
        print("SharedAclGroupTest()")
        if (self.client.is_feature_enable(SWITCH_FEATURE_INGRESS_MIRROR_ACL) == 0):
            print("MirrorAcl feature not enabled, skipping")
            return
        table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IPV4
        if self.client.is_feature_enable(SWITCH_FEATURE_SHARED_INGRESS_IP_ACL):
            table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IP

        # Create ACL groups
        acl_group1 = self.add_acl_group(self.device, bind_point_type=[acl_group_bp_port])
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, acl_group1)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, acl_group1)

        # Create ACL tables and entries
        acl_table1 = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IP_MIRROR,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)

        acl_table2 = self.add_acl_table(self.device,
            type=table_type,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_entry22 = self.add_acl_entry(self.device,
            src_ip='11.11.11.1',
            src_ip_mask='255.255.255.255',
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            table_handle=acl_table2)
        self.client.object_counters_clear_all(acl_entry22)

        # Create ACL group members
        acl_grp_member1 = self.add_acl_group_member(self.device,
            acl_table_handle=acl_table1,
            acl_group_handle=acl_group1)
        acl_grp_member2 = self.add_acl_group_member(self.device,
            acl_table_handle=acl_table2,
            acl_group_handle=acl_group1)

        def TestTraffic():
            print(" Testing Traffic")
            pkt1 = simple_tcp_packet(pktlen=pkt_len,
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='10.10.10.1',
                ip_src='11.11.11.1',
                ip_id=105,
                ip_ttl=64)
            pkt2 = simple_tcp_packet(pktlen=pkt_len,
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='10.10.10.1',
                ip_src='12.12.12.1',
                ip_id=105,
                ip_ttl=64)

            # tx from port0, verify ACL pkt-drop
            send_packet(self, self.devports[0], pkt1)
            verify_no_other_packets(self, timeout=2)
            # tx from port1, verify acl pkt-drop
            send_packet(self, self.devports[1], pkt1)
            verify_no_other_packets(self, timeout=2)

        try:
            TestTraffic()
            self.startFrWrReplay(1, TestTraffic)
        finally:
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            self.cleanlast()
    ############################################################################

    def SharedAclGroupMirrorTest(self):
        '''
        This test tries to verify ACL groups with deny/permit and Mirror ACLs
        Acl-group/table of bp_type PORT
        Have 2 acl tables of type [v4, mirror] from seperate label space
        TX packet from port 0 - check for dropped packet
        TX packet with '20.20.20.1' - check for mirror packet
        '''
        print("SharedAclGroupMirrorTest()")
        if (self.client.is_feature_enable(SWITCH_FEATURE_INGRESS_MIRROR_ACL) == 0):
            print("MirrorAcl feature not enabled, skipping")
            return
        table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IPV4
        if self.client.is_feature_enable(SWITCH_FEATURE_SHARED_INGRESS_IP_ACL):
            table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IP

        # Create ACL groups
        acl_group1 = self.add_acl_group(self.device, bind_point_type=[acl_group_bp_port])

        # Create ACL tables and entries
        mirror = self.add_mirror(self.device,
            type=SWITCH_MIRROR_ATTR_TYPE_LOCAL,
            direction=SWITCH_MIRROR_ATTR_DIRECTION_INGRESS,
            egress_port_handle=self.port2)
        acl_table1 = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IP_MIRROR,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_entry11 = self.add_acl_entry(self.device,
            dst_ip='20.20.20.1',
            dst_ip_mask='255.255.255.255',
            action_ingress_mirror_handle=mirror,
            table_handle=acl_table1)
        self.client.object_counters_clear_all(acl_entry11)

        acl_table2 = self.add_acl_table(self.device,
            type=table_type,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_entry21 = self.add_acl_entry(self.device,
            dst_ip='10.10.10.1',
            dst_ip_mask='255.255.255.255',
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            table_handle=acl_table2)
        self.client.object_counters_clear_all(acl_entry21)

        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, acl_group1)
        # Create ACL group members
        acl_grp_member1 = self.add_acl_group_member(self.device,
            acl_table_handle=acl_table1,
            acl_group_handle=acl_group1)
        acl_grp_member2 = self.add_acl_group_member(self.device,
            acl_table_handle=acl_table2,
            acl_group_handle=acl_group1)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, acl_group1)

        def TestTraffic():
            print(" Testing Traffic")
            pkt2 = simple_tcp_packet(pktlen=pkt_len,
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='10.10.10.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_ttl=64)
            pkt3 = simple_tcp_packet(pktlen=pkt_len,
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='20.20.20.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_ttl=64)
            exp_pkt = simple_tcp_packet(pktlen=pkt_len,
                eth_dst='00:11:22:33:44:55',
                eth_src='00:77:66:55:44:33',
                ip_dst='10.10.10.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_ttl=63)
            # tx from port0, verify the ACL pkt-drop
            send_packet(self, self.devports[0], pkt2)
            verify_no_other_packets(self, timeout=2)
            # tx from port0, verify the Mirror ACL
            send_packet(self, self.devports[0], pkt3)
            verify_packets(self, pkt3, [self.devports[2]])

            #remove ACL group member 2, packet must be forw
            if (self.client.is_feature_enable(SWITCH_FEATURE_ACL_PORT_GROUP) == 0):
                self.cleanlast()
                send_packet(self, self.devports[0], pkt2)
                verify_packet(self, self.exp_pkt, self.devports[1])

        try:
            TestTraffic()
            acl_grp_member2 = self.add_acl_group_member(self.device,
                acl_table_handle=acl_table2,
                acl_group_handle=acl_group1)
            self.startFrWrReplay(1, TestTraffic)
        finally:
            self.cleanlast()
            self.cleanlast()
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            # remove the acl entry and table
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
    ############################################################################

    def SharedAclGroupDscpMirrorTest(self):
        '''
        This test tries to verify ACL groups with IP Mirror ACLs and 
        DSCP mirror ACL.
        Acl-group/table of bp_type PORT
        Have 2 acl tables of type [mirror, dscp_mirror] from seperate label space
        TX packet with '20.20.20.1' - check for mirror packet on port2
        TX packet with DSCP 6 - check for mirror packet on port3
        '''
        print("SharedAclGroupDscpMirrorTest()")
        if (self.client.is_feature_enable(SWITCH_FEATURE_INGRESS_TOS_MIRROR_ACL) == 0):
            print("DscpMirrorAcl feature not enabled, skipping")
            return

        if self.client.is_feature_enable(SWITCH_FEATURE_SHARED_INGRESS_IP_ACL):
            table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IP

        #Create ACL groups
        acl_group1 = self.add_acl_group(self.device, bind_point_type=[acl_group_bp_port])

        #Create ACL tables and entries
        meter = self.add_meter(self.device,
            mode=SWITCH_METER_ATTR_MODE_TWO_RATE_THREE_COLOR,
            cbs=10,cir=10,
            type=SWITCH_METER_ATTR_TYPE_PACKETS,
            color_source=SWITCH_METER_ATTR_COLOR_SOURCE_BLIND)

        mirror = self.add_mirror(self.device,
            type=SWITCH_MIRROR_ATTR_TYPE_LOCAL,
            direction=SWITCH_MIRROR_ATTR_DIRECTION_INGRESS,
            egress_port_handle=self.port2, meter_handle=meter)

        dscp_mirror_meter = self.add_meter(self.device,
            mode=SWITCH_METER_ATTR_MODE_TWO_RATE_THREE_COLOR,
            cbs=10,cir=10,
            type=SWITCH_METER_ATTR_TYPE_PACKETS,
            color_source=SWITCH_METER_ATTR_COLOR_SOURCE_BLIND)

        dscp_mirror = self.add_mirror(self.device,
            type=SWITCH_MIRROR_ATTR_TYPE_LOCAL,
            direction=SWITCH_MIRROR_ATTR_DIRECTION_INGRESS,
            egress_port_handle=self.port3, meter_handle=dscp_mirror_meter)

        acl_table1 = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IP_MIRROR,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_entry11 = self.add_acl_entry(self.device,
            dst_ip='20.20.20.1',
            dst_ip_mask='255.255.255.255',
            action_ingress_mirror_handle=mirror,
            table_handle=acl_table1)
        self.client.object_counters_clear_all(acl_entry11)

        acl_table2 = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_TOS_MIRROR,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)

        acl_entry21 = self.add_acl_entry(self.device,
            ip_dscp=6,
            ip_dscp_mask=127,
            action_ingress_mirror_handle=dscp_mirror,
            table_handle=acl_table2)
        self.client.object_counters_clear_all(acl_entry21)

        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, acl_group1)
        #Create ACL group members
        acl_grp_member1 = self.add_acl_group_member(self.device,
            acl_table_handle=acl_table1,
            acl_group_handle=acl_group1)
        acl_grp_member2 = self.add_acl_group_member(self.device,
            acl_table_handle=acl_table2,
            acl_group_handle=acl_group1)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, acl_group1)

        def TestTraffic():
            print(" Testing Traffic")
            pkt3 = simple_tcp_packet(pktlen=pkt_len,
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='20.20.20.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_ttl=64)
            pkt4 = simple_tcp_packet(pktlen=pkt_len,
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='30.30.30.1',
                ip_src='192.168.0.1',
                ip_dscp=6,
                ip_id=105,
                ip_ttl=64)
            #tx from port0, verify the Mirror ACL
            pre_counter = self.object_counters_get(acl_entry11)
            send_packet(self, self.devports[0], pkt3)
            verify_packets(self, pkt3, [self.devports[2]])
            post_counter = self.object_counters_get(acl_entry11)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)

            #tx from port0, verify the DSCP Mirror ACL
            pre_counter = self.object_counters_get(acl_entry21)
            send_packet(self, self.devports[0], pkt4)
            verify_packets(self, pkt4, [self.devports[3]])
            post_counter = self.object_counters_get(acl_entry21)
            print("Counter %d"%(post_counter[0].count))
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)

        try:
            TestTraffic()
            acl_grp_member2 = self.add_acl_group_member(self.device,
                acl_table_handle=acl_table2,
                acl_group_handle=acl_group1)
            self.startFrWrReplay(1, TestTraffic)
        finally:
            self.cleanlast()
            self.cleanlast()
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
#remove the acl entry and table
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
    ############################################################################

    def SharedAclGroupTest2(self):
        '''
        This test tries to verify ACL groups with deny/permit
        Acl-group/table of bp_type PORT,
        have 2 tables of type [v6, mirror] from seperate label space
        We will verify if the correct label is being used
        '''
        print("SharedAclGroupTest2()")
        if (self.client.is_feature_enable(SWITCH_FEATURE_INGRESS_MIRROR_ACL) == 0):
            print("MirrorAcl feature not enabled, skipping")
            return
        table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IPV6
        if self.client.is_feature_enable(SWITCH_FEATURE_SHARED_INGRESS_IP_ACL):
            table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IP

        route0 = self.add_route(self.device, ip_prefix='20.20.20.1', vrf_handle=self.vrf10, nexthop_handle=self.nhop1)
        acl_group1 = self.add_acl_group(self.device, bind_point_type=[acl_group_bp_port])
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, acl_group1)

        def TestSetup():
            # program ACL tables, entries
            acl_table2 = self.add_acl_table(self.device,
                type=table_type,
                bind_point_type=[acl_table_bp_port],
                direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
            acl_entry21 = self.add_acl_entry(self.device,
                dst_ip='1234:5678:9abc:def0:4422:1133:5577:99aa',
                dst_ip_mask='FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF',
                packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
                table_handle=acl_table2)
            self.client.object_counters_clear_all(acl_entry21)

            mirror = self.add_mirror(self.device,
                type=SWITCH_MIRROR_ATTR_TYPE_LOCAL,
                direction=SWITCH_MIRROR_ATTR_DIRECTION_INGRESS,
                egress_port_handle=self.port2)
            acl_table3 = self.add_acl_table(self.device,
                type=SWITCH_ACL_TABLE_ATTR_TYPE_IP_MIRROR,
                bind_point_type=[acl_table_bp_port],
                direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
            acl_entry31 = self.add_acl_entry(self.device,
                dst_ip='20.20.20.1',
                dst_ip_mask='255.255.255.255',
                action_ingress_mirror_handle=mirror,
                table_handle=acl_table3)
            self.client.object_counters_clear_all(acl_entry31)

            # program ACL group members
            acl_grp_member2 = self.add_acl_group_member(self.device,
                acl_table_handle=acl_table2,
                acl_group_handle=acl_group1)
            acl_grp_member3 = self.add_acl_group_member(self.device,
                acl_table_handle=acl_table3,
                acl_group_handle=acl_group1)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, acl_group1)

        def TestTraffic():
            print(" Testing Traffic")
            # tx from port0, verify the v6 acl pkt-drop
            print(" Sending IPv6 packet port 0 -> port 1, dp-pkt")
            send_packet(self, self.devports[0], self.pkt_v6)
            verify_no_other_packets(self, timeout=2)

            mirror_pkt1 = simple_tcp_packet(pktlen=pkt_len,
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='20.20.20.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_ttl=64)
            exp_no_mirror_pkt1 = simple_tcp_packet(pktlen=pkt_len,
                eth_dst='00:11:22:33:44:55',
                eth_src='00:77:66:55:44:33',
                ip_dst='20.20.20.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_ttl=63)
            # tx from port0, verify the Mirror ACL
            print(" Sending IPv4 packet port 0 -> port 1, mirror to port2")
            send_packet(self, self.devports[0], mirror_pkt1)
            verify_each_packet_on_each_port(self, [exp_no_mirror_pkt1, mirror_pkt1],
                                [self.devports[1], self.devports[2]])
            if(self.client.is_feature_enable(SWITCH_FEATURE_ACL_PORT_GROUP) == 0):
                # cleanp acl_group member
                self.cleanlast()
                self.cleanlast()

                # cleanup acl_entry31, acl_table3, mirror
                self.cleanlast()
                self.cleanlast()
                self.cleanlast()

                # send from port0, verify the Mirror ACL
                print(" Sending IPv4 packet port 0 -> port 1, no mirror")
                send_packet(self, self.devports[0], mirror_pkt1)
                verify_packets(self, exp_no_mirror_pkt1, [self.devports[1]])

                # cleanup acl_entry21, acl_table2, test for forw
                self.cleanlast()
                self.cleanlast()

                print(" Sending IPv6 packet port 0 -> port 1")
                send_packet(self, self.devports[0], self.pkt_v6)
                verify_packets(self, self.exp_pkt_v6, [self.devports[1]])
            else:
                self.cleanup()

        def TestCleanup():
            print(" Cleaning up")
            # cleanp acl_group member
            self.cleanlast()
            self.cleanlast()

            # cleanup acl_entry31, acl_table3, mirror
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

            # cleanup acl_entry21, acl_table2, test for forw
            self.cleanlast()
            self.cleanlast()

        try:
            TestSetup()
            TestTraffic()
            TestSetup()
            if (self.startFrWrReplay(1, TestTraffic) == False):
                TestCleanup()
        finally:
            # unset acl_group from port0
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            # cleanup acl_group
            self.cleanlast()
            # clean route0
            self.cleanlast()
    ############################################################################

    def EgressMacAclTableTest(self):
        '''
        This test tries to verify egress mac acl entries
        We send a packet from port 5 and drop the packet on egress port 4
        The bp_type is PORT. The port_lag_label comes from the table id
        '''
        print("EgressMacAclTableTest()")
        if (self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_MAC_ACL) == 0):
            print("EgressMacAcl  feature not enabled, skipping")
            return

        acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_MAC,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_EGRESS)
        acl_entry = self.add_acl_entry(self.device,
            src_mac='00:22:22:22:22:22',
            src_mac_mask='ff:ff:ff:ff:ff:ff',
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry)
        table_name = SWITCH_DEVICE_ATTR_TABLE_EGRESS_MAC_ACL
        pre_hw_table_count = get_hw_table_usage(self, table_name)
        self.attribute_set(self.port4, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, acl_table)
        post_hw_table_count = get_hw_table_usage(self, table_name)
        self.assertEqual(post_hw_table_count - pre_hw_table_count, 1)

        def TestTraffic():
            print(" Testing Traffic")
            pkt = simple_eth_packet(
                eth_dst='00:44:44:44:44:44',
                eth_src='00:22:22:22:22:22')

            pre_counter = self.object_counters_get(acl_entry)
            send_packet(self, self.devports[5], pkt)
            verify_no_other_packets(self, timeout=2)
            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)

        try:
            TestTraffic()
            self.startFrWrReplay(1, TestTraffic)
        finally:
            self.attribute_set(self.port4, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, 0)
            # remove the acl entry and table
            self.cleanlast()
            self.cleanlast()
    ############################################################################

    def EgressMacAclTablePrioTest(self):
        '''
        This test tries to veriry egress mac acl table priority
        '''
        print("EgressMacAclTablePrioTest()")
        if (self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_MAC_ACL) == 0):
            print("EgressMacAcl  feature not enabled, skipping")
            return

        acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_MAC,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_EGRESS)
        acl_entry1 = self.add_acl_entry(self.device,
            src_mac='00:22:22:22:22:22',
            src_mac_mask='ff:ff:ff:ff:ff:ff',
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            prority=self.high_prio,
            table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry1)
        acl_entry2 = self.add_acl_entry(self.device,
            src_mac='00:22:22:22:22:00',
            src_mac_mask='ff:ff:ff:ff:ff:00',
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_PERMIT,
            priority=self.low_prio,
            table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry2)
        table_name = SWITCH_DEVICE_ATTR_TABLE_EGRESS_MAC_ACL
        pre_hw_table_count = get_hw_table_usage(self, table_name)
        self.attribute_set(self.port4, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, acl_table)
        post_hw_table_count = get_hw_table_usage(self, table_name)
        self.assertEqual(post_hw_table_count - pre_hw_table_count, 2)

        def TestTraffic():
            print(" Testing Traffic")
            pkt1 = simple_eth_packet(
                eth_dst='00:44:44:44:44:44',
                eth_src='00:22:22:22:22:22')
            pkt2 = simple_eth_packet(
                eth_dst='00:44:44:44:44:44',
                eth_src='00:22:22:22:22:33')

            # case-1: tx packet to high prio acl_entry1 and test for drop
            pre_counter = self.object_counters_get(acl_entry1)
            send_packet(self, self.devports[5], pkt1)
            verify_no_other_packets(self, timeout=2)
            post_counter = self.object_counters_get(acl_entry1)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)

            # case-2: tx packet for low_prio acl_entry2 and test for no drop
            pre_counter = self.object_counters_get(acl_entry2)
            send_packet(self, self.devports[5], pkt2)
            verify_packet(self, pkt2, self.devports[4])
            post_counter = self.object_counters_get(acl_entry2)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)

        try:
            TestTraffic()
            self.startFrWrReplay(0, TestTraffic)  # WI or FI requires fix
        finally:
            self.attribute_set(self.port4, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, 0)
            # remove the acl_entry1, acl_entry2 and table
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
    ############################################################################

    def EgressIPv4AclTableTest(self):
        '''
        This test tries to verify ipv4 acl entries
        We send a packet from port 0 and drop the packet on port 1
        The bp_type is PORT. The port_lag_label comes from the table id
        '''
        print("EgressIPv4AclTableTest()")
        if (self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_IP_ACL) == 0):
            print("EgressIPv4Acl  feature not enabled, skipping")
            return
        acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IPV4,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_EGRESS)
        acl_entry = self.add_acl_entry(self.device,
            dst_ip='10.10.10.1',
            dst_ip_mask='255.255.255.255',
            #tcp_flags=2,
            #tcp_flags_mask=127,
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry)
        table_name = SWITCH_DEVICE_ATTR_TABLE_EGRESS_IPV4_ACL
        pre_hw_table_count = get_hw_table_usage(self, table_name)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, acl_table)
        post_hw_table_count = get_hw_table_usage(self, table_name)
        self.assertEqual(post_hw_table_count - pre_hw_table_count, 1)

        def TestTraffic():
            print(" Testing Traffic")
            pre_counter = self.object_counters_get(acl_entry)
            send_packet(self, self.devports[0], self.pkt)
            verify_no_other_packets(self, timeout=2)
            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)

            """
            #change acl action to PERMIT and test for no drop
            self.attribute_set(acl_entry, SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION,
                        SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_PERMIT)
            pre_counter = self.object_counters_get(acl_entry)
            send_packet(self, self.devports[0], self.pkt)
            verify_packets(self, self.exp_pkt, [self.devports[1]])
            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)
            """

        try:
            TestTraffic()
            self.startFrWrReplay(1, TestTraffic)
        finally:
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, 0)
            # remove the acl entry and table
            self.cleanlast()
            self.cleanlast()
    ############################################################################

    def EgressIPv4AclTablePrioTest(self):
        '''
        This test tries to verify ipv4 acl entries
        We send a packet from port 0 and drop the packet on port 1
        The bp_type is PORT. The port_lag_label comes from the table id
        '''
        print("EgressIPv4AclTablePrioTest()")
        if (self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_IP_ACL) == 0):
            print("EgressIPv4Acl  feature not enabled, skipping")
            return
        acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IPV4,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_EGRESS)
        acl_entry1 = self.add_acl_entry(self.device,
            dst_ip='10.10.10.1',
            dst_ip_mask='255.255.255.255',
            priority = self.high_prio,
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry1)
        acl_entry2 = self.add_acl_entry(self.device,
            dst_ip='10.10.10.0',
            dst_ip_mask='255.255.255.0',
            priority=self.low_prio,
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_PERMIT,
            table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry2)

        table_name = SWITCH_DEVICE_ATTR_TABLE_EGRESS_IPV4_ACL
        pre_hw_table_count = get_hw_table_usage(self, table_name)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, acl_table)
        post_hw_table_count = get_hw_table_usage(self, table_name)
        self.assertEqual(post_hw_table_count - pre_hw_table_count, 2)
        route0 = self.add_route(self.device, ip_prefix='10.10.10.0/24', vrf_handle=self.vrf10,
                       nexthop_handle=self.nhop1)

        def TestTraffic():
            print(" Testing Traffic")
            # case-1: tx packet to high prio acl_entry1 and test for dp-pkt
            pre_counter = self.object_counters_get(acl_entry1)
            send_packet(self, self.devports[0], self.pkt)
            verify_no_other_packets(self, timeout=2)
            post_counter = self.object_counters_get(acl_entry1)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)

            acl_entry2_pkt = simple_tcp_packet(pktlen=pkt_len,
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='10.10.10.8',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_ttl=64)
            acl_entry2_exp_pkt = simple_tcp_packet(pktlen=pkt_len,
                eth_dst='00:11:22:33:44:55',
                eth_src='00:77:66:55:44:33',
                ip_dst='10.10.10.8',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_ttl=63)

            # case-2: tx packet to low prio acl_entry2 and test for no  dp-pkt
            pre_counter = self.object_counters_get(acl_entry2)
            send_packet(self, self.devports[0], acl_entry2_pkt)
            verify_packet(self, acl_entry2_exp_pkt, self.devports[1])
            post_counter = self.object_counters_get(acl_entry2)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)

        try:
            TestTraffic()
            self.startFrWrReplay(0, TestTraffic) # WI or FI requires fix
        finally:
            # remove route0
            self.cleanlast()
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, 0)
            # remove the acl_entry1, acl_entry2 and table
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
    ############################################################################

    def LagEgressIPv4AclTableTest(self):
        print("LagEgressIPv4AclTableTest()")
        if (self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_IP_ACL) == 0):
            print("EgressIPv4Acl  feature not enabled, skipping")
            return
        acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IPV4,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_EGRESS)
        acl_entry = self.add_acl_entry(self.device,
            dst_ip='10.10.10.4',
            dst_ip_mask='255.255.255.255',
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry)
        table_name = SWITCH_DEVICE_ATTR_TABLE_EGRESS_IPV4_ACL
        pre_hw_table_count = get_hw_table_usage(self, table_name)
        self.attribute_set(self.lag0, SWITCH_LAG_ATTR_EGRESS_ACL_HANDLE, acl_table)
        post_hw_table_count = get_hw_table_usage(self, table_name)
        self.assertEqual(post_hw_table_count - pre_hw_table_count, 1)

        def TestTraffic():
            print(" Testing Traffic")
            pre_counter = self.object_counters_get(acl_entry)
            send_packet(self, self.devports[0], self.lag_pkt)
            verify_no_other_packets(self, timeout=2)
            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)

        try:
            TestTraffic()
            self.startFrWrReplay(1, TestTraffic)
        finally:
            self.attribute_set(self.lag0, SWITCH_LAG_ATTR_EGRESS_ACL_HANDLE, 0)
            # remove the acl entry and table
            self.cleanlast()
            self.cleanlast()
    ############################################################################

    def EgressDscpDenyMirrorTableTest(self):
        '''
        This test tries to verify DSCP mirror ACL and DSCP deny ACL entry.
        Acl table of bp_type PORT
        TX packet with DSCP 6 - check for mirror packet on port1
        and mirror on port5
        TX packet with DSCP 16 - Drop
        TX V6 packet with DSCP 16 - Drop
        '''
        print("EgressDscpMirrorTest()")
        if (self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_TOS_MIRROR_ACL) == 0):
            print("DscpMirrorAcl feature not enabled, skipping")
            return

        route0 = self.add_route(self.device, ip_prefix='10.10.0.5',
            vrf_handle=self.vrf10, nexthop_handle=self.nhop1)

        #Create ACL tables and entries
        dscp_mirror_meter = self.add_meter(self.device,
            mode=SWITCH_METER_ATTR_MODE_TWO_RATE_THREE_COLOR,
            cbs=10,cir=10,
            type=SWITCH_METER_ATTR_TYPE_PACKETS,
            color_source=SWITCH_METER_ATTR_COLOR_SOURCE_BLIND)

        dscp_mirror = self.add_mirror(self.device,
            type=SWITCH_MIRROR_ATTR_TYPE_LOCAL,
            direction=SWITCH_MIRROR_ATTR_DIRECTION_EGRESS,
            egress_port_handle=self.port5, meter_handle=dscp_mirror_meter)

        acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_TOS_MIRROR,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_EGRESS)

        acl_entry = self.add_acl_entry(self.device,
            ip_dscp=6,
            ip_dscp_mask=127,
            action_egress_mirror_handle=dscp_mirror,
            table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry)

        # ACL tables, entries
        dscp_acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IPV4,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_EGRESS)
        dscp_acl_entry = self.add_acl_entry(self.device,
            ip_dscp=16,
            ip_dscp_mask=127,
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            table_handle=dscp_acl_table)

        dscp_v6_acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IPV6,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_EGRESS)
        dscp_v6_acl_entry = self.add_acl_entry(self.device,
            ip_dscp=16,
            ip_dscp_mask=127,
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            table_handle=dscp_v6_acl_table)

        # ACL groups
        acl_group = self.add_acl_group(self.device, bind_point_type=[acl_group_bp_port])
        acl_grp_member1 = self.add_acl_group_member(self.device,
            acl_table_handle=acl_table,
            acl_group_handle=acl_group)
        acl_grp_member2 = self.add_acl_group_member(self.device,
            acl_table_handle=dscp_acl_table,
            acl_group_handle=acl_group)

        acl_grp_member3 = self.add_acl_group_member(self.device,
            acl_table_handle=dscp_v6_acl_table,
            acl_group_handle=acl_group)

        self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, acl_group)

        route0 = self.add_route(self.device, ip_prefix='1234:5678:9abc:def0:4422:1133:5577:0000/112',
                      vrf_handle=self.vrf10, nexthop_handle=self.nhop1)

        def TestTraffic():
            print(" Testing Traffic")
            dscp_pkt= simple_udp_packet(pktlen=pkt_len,
                eth_dst=self.rmac,
                eth_src='00:11:22:33:44:66',
                ip_dst='10.10.0.5',
                ip_src='1.1.1.10',
                ip_dscp=6,
                ip_ttl=63)
            dscp_pkt_exp = simple_udp_packet(pktlen=pkt_len,
                eth_src=self.rmac,
                eth_dst='00:11:22:33:44:55',
                ip_dst='10.10.0.5',
                ip_src='1.1.1.10',
                ip_dscp=6,
                ip_ttl=62)

            dscp_v6_pkt = simple_tcpv6_packet(pktlen=pkt_len,
                eth_dst=self.rmac,
                eth_src='00:11:22:33:44:66',
                ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:88aa',
                ipv6_src='2000::1',
                ipv6_dscp=16,
                ipv6_hlim=64)
            #verify the DSCP Mirror ACL
            print(" Sending IPv4 packet,DSCP=6 port 0 -> port 1 -> mirror port 5")
            pre_counter = self.object_counters_get(acl_entry)
            send_packet(self, self.devports[0], dscp_pkt)
            verify_packets(self, dscp_pkt_exp, [self.devports[1], self.devports[5]])
            post_counter = self.object_counters_get(acl_entry)
            print("Counter %d"%(post_counter[0].count))
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)
            print(" Verify DSCP deny ACL entry. Sending IPv4 packet,DSCP=16,TOS=64 port 0, drop packet")
            dscp_pkt['IP'].tos=64
            pre_counter = self.object_counters_get(dscp_acl_entry)
            send_packet(self, self.devports[0], dscp_pkt)
            verify_no_other_packets(self, timeout=2)
            post_counter = self.object_counters_get(dscp_acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)

            print(" Verify V6 DSCP deny ACL entry. Sending IPv6 packet TOS=64 port 0, drop packet")
            pre_counter = self.object_counters_get(dscp_v6_acl_entry)
            send_packet(self, self.devports[0], dscp_v6_pkt)
            verify_no_other_packets(self, timeout=2)
            post_counter = self.object_counters_get(dscp_v6_acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)
        try:
            TestTraffic()

        finally:
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, 0)
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
    ############################################################################

    def EgressIPv6AclTableTest(self):
        '''
        This test tries to verify ipv6 acl entries
        We send a packet from port 0 and drop the packet on port 1
        The bp_type is PORT. The port_lag_label comes from the table id
        '''
        print("EgressIPv6AclTableTest()")
        if (self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_IP_ACL) == 0):
            print("EgressIPv6Acl  feature not enabled, skipping")
            return
        acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IPV6,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_EGRESS)
        acl_entry = self.add_acl_entry(self.device,
            dst_ip='1234:5678:9abc:def0:4422:1133:5577:99aa',
            dst_ip_mask='FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF',
            #tcp_flags=2,
            #tcp_flags_mask=127,
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry)
        table_name = SWITCH_DEVICE_ATTR_TABLE_EGRESS_IPV6_ACL
        pre_hw_table_count = get_hw_table_usage(self, table_name)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, acl_table)
        post_hw_table_count = get_hw_table_usage(self, table_name)
        self.assertEqual(post_hw_table_count - pre_hw_table_count, 1)

        def TestTraffic():
            print(" Testing Traffic")
            pre_counter = self.object_counters_get(acl_entry)
            send_packet(self, self.devports[0], self.pkt_v6)
            verify_no_other_packets(self, timeout=2)
            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)

            """
            #change acl action to PERMIT and test for no drop
            self.attribute_set(acl_entry, SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION,
                        SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_PERMIT)
            pre_counter = self.object_counters_get(acl_entry)
            send_packet(self, self.devports[0], self.pkt_v6)
            verify_packets(self, self.exp_pkt_v6, [self.devports[1]])
            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)
            """

        try:
            TestTraffic()
            self.startFrWrReplay(1, TestTraffic)
        finally:
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, 0)
            # remove the acl entry and table
            self.cleanlast()
            self.cleanlast()
    ############################################################################

    def EgressIPv6AclTablePrioTest(self):
        '''
        This test tries to verify ipv6 acl entries
        We send a packet from port 0 and drop the packet on port 1
        The bp_type is PORT. The port_lag_label comes from the table id
        '''
        print("EgressIPv6AclTablePrioTest()")
        if (self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_IP_ACL) == 0):
            print("EgressIPv6Acl  feature not enabled, skipping")
            return
        acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IPV6,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_EGRESS)
        acl_entry1 = self.add_acl_entry(self.device,
            dst_ip='1234:5678:9abc:def0:4422:1133:5577:99aa',
            dst_ip_mask='FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF',
            priority = self.high_prio,
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry1)
        acl_entry2 = self.add_acl_entry(self.device,
            dst_ip='1234:5678:9abc:def0:4422:1133:5577:0000',
            dst_ip_mask='FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:0000',
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_PERMIT,
            priority = self.low_prio,
            table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry2)

        table_name = SWITCH_DEVICE_ATTR_TABLE_EGRESS_IPV6_ACL
        pre_hw_table_count = get_hw_table_usage(self, table_name)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, acl_table)
        post_hw_table_count = get_hw_table_usage(self, table_name)
        self.assertEqual(post_hw_table_count - pre_hw_table_count, 2)
        route0 = self.add_route(self.device, ip_prefix='1234:5678:9abc:def0:4422:1133:5577:0000/112',
                      vrf_handle=self.vrf10, nexthop_handle=self.nhop1)

        def TestTraffic():
            print(" Testing Traffic")
            # case1: tx packet to high_prio acl_entry1 and test for dp-pkt
            pre_counter = self.object_counters_get(acl_entry1)
            send_packet(self, self.devports[0], self.pkt_v6)
            verify_no_other_packets(self, timeout=2)
            post_counter = self.object_counters_get(acl_entry1)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)

            acl_entry2_pkt_v6 = simple_tcpv6_packet(pktlen=pkt_len,
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:88aa',
                ipv6_src='2000::1',
                ipv6_hlim=64)
            acl_entry2_exp_pkt_v6 = simple_tcpv6_packet(pktlen=pkt_len,
                eth_dst='00:11:22:33:44:55',
                eth_src='00:77:66:55:44:33',
                ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:88aa',
                ipv6_src='2000::1',
                ipv6_hlim=63)
            # case-2: tx packet to low_prio acl_entry2 and test for no  dp-pkt
            pre_counter = self.object_counters_get(acl_entry2)
            send_packet(self, self.devports[0], acl_entry2_pkt_v6)
            verify_packet(self, acl_entry2_exp_pkt_v6, self.devports[1])
            post_counter = self.object_counters_get(acl_entry2)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)

        try:
            TestTraffic()
            self.startFrWrReplay(0, TestTraffic)  # WI or FI requires fix
        finally:
            #clean route0
            self.cleanlast()
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, 0)
            # remove the acl_entry1, acl_entry2 and table
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
    ############################################################################

    def EgressPortLabelTest(self):
        '''
        This test tries to verify ipv4 acl entries
        We send a packet from port 0 and drop the packet
        The bp_type is NONE. The port_lag_label is chosen by the test
        '''
        print("EgressPortLabelTest()")
        if (self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_IP_ACL) == 0):
            print("EgressAcl feature not enabled, skipping")
            return
        acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IPV4,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_EGRESS)
        acl_entry = self.add_acl_entry(self.device,
            port_lag_label=2,
            port_lag_label_mask=0xFF,
            dst_ip='10.10.10.1',
            dst_ip_mask='255.255.255.255',
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry)
        table_name = SWITCH_DEVICE_ATTR_TABLE_EGRESS_IPV4_ACL
        pre_hw_table_count = get_hw_table_usage(self, table_name)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_PORT_LAG_LABEL, 2)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, acl_table)
        post_hw_table_count = get_hw_table_usage(self, table_name)
        self.assertEqual(post_hw_table_count - pre_hw_table_count, 1)

        def TestTraffic():
            print(" Testing Traffic")
            pre_counter = self.object_counters_get(acl_entry)
            send_packet(self, self.devports[0], self.pkt)
            verify_no_other_packets(self, timeout=2)
            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)

        try:
            TestTraffic()
            self.startFrWrReplay(1, TestTraffic)
        finally:
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_PORT_LAG_LABEL, 0)
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, 0)
            # remove the acl entry and table
            self.cleanlast()
            self.cleanlast()
    ############################################################################

    def EgressLagLabelTest(self):
        print("EgressLagLabelTest()")
        if (self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_IP_ACL) == 0):
            print("EgressAcl feature not enabled, skipping")
            return
        acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IPV4,
            bind_point_type=[acl_table_bp_lag],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_EGRESS)
        acl_entry = self.add_acl_entry(self.device,
            port_lag_label=2,
            port_lag_label_mask=0xFF,
            dst_ip='10.10.10.5',
            dst_ip_mask='255.255.255.255',
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry)
        table_name = SWITCH_DEVICE_ATTR_TABLE_EGRESS_IPV4_ACL
        pre_hw_table_count = get_hw_table_usage(self, table_name)
        self.attribute_set(self.lag1, SWITCH_LAG_ATTR_EGRESS_PORT_LAG_LABEL, 2)
        self.attribute_set(self.lag1, SWITCH_LAG_ATTR_EGRESS_ACL_HANDLE, acl_table)
        post_hw_table_count = get_hw_table_usage(self, table_name)
        self.assertEqual(post_hw_table_count - pre_hw_table_count, 1)

        def TestTraffic():
            print(" Testing Traffic")
            pre_counter = self.object_counters_get(acl_entry)
            send_packet(self, self.devports[0], self.pkt1)
            verify_no_other_packets(self, timeout=2)
            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)

        try:
            TestTraffic()
            self.startFrWrReplay(1, TestTraffic)
        finally:
            self.attribute_set(self.lag1, SWITCH_LAG_ATTR_EGRESS_PORT_LAG_LABEL, 0)
            self.attribute_set(self.lag1, SWITCH_LAG_ATTR_EGRESS_ACL_HANDLE, 0)
            # remove the acl entry and table
            self.cleanlast()
            self.cleanlast()
    ############################################################################

    def EgressL3BdLabelTest(self):
        print("EgressL3BdLabelTest()")
        if (self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_IP_ACL) == 0):
            print("EgressAcl feature not enabled, skipping")
            return
        acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IPV4,
            bind_point_type=[acl_table_bp_rif],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_EGRESS)
        acl_entry = self.add_acl_entry(self.device,
            bd_label=2,
            bd_label_mask=0xFF,
            dst_ip='10.10.10.1',
            dst_ip_mask='255.255.255.255',
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry)
        self.client.object_counters_clear_all(acl_entry)
        table_name = SWITCH_DEVICE_ATTR_TABLE_EGRESS_IPV4_ACL
        pre_hw_table_count = get_hw_table_usage(self, table_name)
        self.attribute_set(self.rif1, SWITCH_RIF_ATTR_EGRESS_VLAN_RIF_LABEL, 2)
        self.attribute_set(self.rif1, SWITCH_RIF_ATTR_EGRESS_ACL_HANDLE, acl_table)
        post_hw_table_count = get_hw_table_usage(self, table_name)
        self.assertEqual(post_hw_table_count - pre_hw_table_count, 1)

        def TestTraffic():
            print(" Testing Traffic")
            pre_counter = self.object_counters_get(acl_entry)
            send_packet(self, self.devports[0], self.pkt)
            verify_no_other_packets(self, timeout=2)
            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)

        try:
            TestTraffic()
            self.startFrWrReplay(1, TestTraffic)
        finally:
            self.attribute_set(self.rif1, SWITCH_RIF_ATTR_EGRESS_VLAN_RIF_LABEL, 0)
            self.attribute_set(self.rif1, SWITCH_RIF_ATTR_EGRESS_ACL_HANDLE, 0)
            # remove the acl entry and table
            self.cleanlast()
            self.cleanlast()
    ############################################################################

    def EgressSVIAclTest(self):
        print("EgressSVIAclTest()")
        if (self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_IP_ACL) == 0):
            print("EgressAcl feature not enabled, skipping")
            return
        acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IPV4,
            bind_point_type=[acl_table_bp_rif],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_EGRESS)
        acl_entry = self.add_acl_entry(self.device,
            dst_ip='10.10.10.5',
            dst_ip_mask='255.255.255.255',
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry)
        table_name = SWITCH_DEVICE_ATTR_TABLE_EGRESS_IPV4_ACL
        pre_hw_table_count = get_hw_table_usage(self, table_name)
        self.attribute_set(self.rif4, SWITCH_RIF_ATTR_EGRESS_ACL_HANDLE, acl_table)
        post_hw_table_count = get_hw_table_usage(self, table_name)
        self.assertEqual(post_hw_table_count - pre_hw_table_count, 1)

        def TestTraffic():
            print(" Testing Traffic")
            pre_counter = self.object_counters_get(acl_entry)
            send_packet(self, self.devports[0], self.pkt1)
            verify_no_other_packets(self, timeout=2)
            send_packet(self, self.devports[1], self.pkt1)
            verify_no_other_packets(self, timeout=2)
            send_packet(self, self.devports[3], self.pkt1)
            verify_no_other_packets(self, timeout=2)
            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 3)

        try:
            TestTraffic()
            self.startFrWrReplay(1, TestTraffic)
        finally:
            self.attribute_set(self.rif4, SWITCH_RIF_ATTR_EGRESS_ACL_HANDLE, 0)
            # remove the acl entry and table
            self.cleanlast()
            self.cleanlast()
    ############################################################################

    def EgressSVIBdLabelTest(self):
        print("EgressSVIBdLabelTest()")
        if (self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_IP_ACL) == 0):
            print("EgressAcl feature not enabled, skipping")
            return
        acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IPV4,
            bind_point_type=[acl_table_bp_rif],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_EGRESS)
        acl_entry = self.add_acl_entry(self.device,
            dst_ip='10.10.10.5',
            dst_ip_mask='255.255.255.255',
            bd_label=2,
            bd_label_mask=0xFF,
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry)
        table_name = SWITCH_DEVICE_ATTR_TABLE_EGRESS_IPV4_ACL
        pre_hw_table_count = get_hw_table_usage(self, table_name)
        self.attribute_set(self.rif4, SWITCH_RIF_ATTR_EGRESS_VLAN_RIF_LABEL, 2)
        self.attribute_set(self.rif4, SWITCH_RIF_ATTR_EGRESS_ACL_HANDLE, acl_table)
        post_hw_table_count = get_hw_table_usage(self, table_name)
        self.assertEqual(post_hw_table_count - pre_hw_table_count, 1)

        def TestTraffic():
            print(" Testing Traffic")
            pre_counter = self.object_counters_get(acl_entry)
            send_packet(self, self.devports[0], self.pkt1)
            verify_no_other_packets(self, timeout=2)
            send_packet(self, self.devports[1], self.pkt1)
            verify_no_other_packets(self, timeout=2)
            send_packet(self, self.devports[3], self.pkt1)
            verify_no_other_packets(self, timeout=2)
            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 3)

        try:
            TestTraffic()
            self.startFrWrReplay(1, TestTraffic)
        finally:
            self.attribute_set(self.rif4, SWITCH_RIF_ATTR_EGRESS_VLAN_RIF_LABEL, 0)
            self.attribute_set(self.rif4, SWITCH_RIF_ATTR_EGRESS_ACL_HANDLE, 0)
            # remove the acl entry and table
            self.cleanlast()
            self.cleanlast()
    ############################################################################

    def EgressVlanAclTest(self):
        print("EgressVlanAclTest()")
        if (self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_IP_ACL) == 0):
            print("EgressAcl feature not enabled, skipping")
            return
        acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IPV4,
            bind_point_type=[acl_table_bp_vlan],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_EGRESS)
        acl_entry = self.add_acl_entry(self.device,
            dst_ip='20.20.20.1',
            dst_ip_mask='255.255.255.255',
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry)
        table_name = SWITCH_DEVICE_ATTR_TABLE_EGRESS_IPV4_ACL
        pre_hw_table_count = get_hw_table_usage(self, table_name)
        self.attribute_set(self.vlan20, SWITCH_VLAN_ATTR_EGRESS_ACL_HANDLE, acl_table)
        post_hw_table_count = get_hw_table_usage(self, table_name)
        self.assertEqual(post_hw_table_count - pre_hw_table_count, 1)

        def TestTraffic():
            print(" Testing Traffic")
            pre_counter = self.object_counters_get(acl_entry)
            send_packet(self, self.devports[11], self.vlan20_pkt1)
            verify_no_other_packets(self, timeout=2)
            send_packet(self, self.devports[10], self.vlan20_pkt2)
            verify_no_other_packets(self, timeout=2)
            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 2)

        try:
            TestTraffic()
            self.startFrWrReplay(0, TestTraffic)    # WI or FI not testing
        finally:
            self.attribute_set(self.vlan20, SWITCH_VLAN_ATTR_EGRESS_ACL_HANDLE, 0)
            # remove the acl entry and table
            self.cleanlast()
            self.cleanlast()
    ############################################################################

    def EgressVlanBdLabelTest(self):
        print("EgressVlanBdLabelTest()")
        if (self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_IP_ACL) == 0):
            print("EgressAcl feature not enabled, skipping")
            return
        acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IPV4,
            bind_point_type=[acl_table_bp_vlan],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_EGRESS)
        acl_entry = self.add_acl_entry(self.device,
            bd_label=2,
            bd_label_mask=0xFF,
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry)
        table_name = SWITCH_DEVICE_ATTR_TABLE_EGRESS_IPV4_ACL
        pre_hw_table_count = get_hw_table_usage(self, table_name)
        self.attribute_set(self.vlan20, SWITCH_VLAN_ATTR_EGRESS_VLAN_RIF_LABEL, 2)
        self.attribute_set(self.vlan20, SWITCH_VLAN_ATTR_EGRESS_ACL_HANDLE, acl_table)
        post_hw_table_count = get_hw_table_usage(self, table_name)
        self.assertEqual(post_hw_table_count - pre_hw_table_count, 1)

        def TestTraffic():
            print(" Testing Traffic")
            pre_counter = self.object_counters_get(acl_entry)
            send_packet(self, self.devports[11], self.vlan20_pkt1)
            verify_no_other_packets(self, timeout=2)
            send_packet(self, self.devports[10], self.vlan20_pkt2)
            verify_no_other_packets(self, timeout=2)
            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 2)

        try:
            TestTraffic()
            self.startFrWrReplay(1, TestTraffic)
        finally:
            self.attribute_set(self.vlan20, SWITCH_VLAN_ATTR_EGRESS_VLAN_RIF_LABEL, 0)
            self.attribute_set(self.vlan20, SWITCH_VLAN_ATTR_EGRESS_ACL_HANDLE, 0)
            # remove the acl entry and table
            self.cleanlast()
            self.cleanlast()
    ############################################################################

    def EgressSharedAclGroupTest(self):
        '''
        This test tries to verify ACL groups with deny/permit
        Acl-group of bp_type PORT, prpgram 2 tables from seperate label space [v4, v6]
        We will verify if the correct label is being used
        Currently IP v4, v6 tables are not sharing the label space only in M1 profile
        So, using some acl table feature checks to run only on M1 profile
        '''
        print("EgressSharedAclGroupTest()")
        # Currently IP v4, v6 tables are not sharing the label space only in M1 profile
        # So, using some of the acl table feature checks for this test case
        if ((self.client.is_feature_enable(SWITCH_FEATURE_INGRESS_MAC_ACL)) or
            (self.client.is_feature_enable(SWITCH_FEATURE_INGRESS_IP_QOS_ACL)) or
            (self.client.is_feature_enable(SWITCH_FEATURE_INGRESS_IPV4_RACL)) or
            (self.client.is_feature_enable(SWITCH_FEATURE_INGRESS_IPV6_RACL)) or
            (self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_MAC_ACL)) or
            (self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_MIRROR_ACL))):
            print("IP v4, v6 tables are sharing the same label space in this profile, skipping")
            return

        if (self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_IP_ACL) == 0):
            print("EgressIPv4Acl feature not enabled, skipping")
            return
        # ACL groups
        acl_group1 = self.add_acl_group(self.device, bind_point_type=[acl_group_bp_port])

        # ACL tables, entries
        acl_table1 = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IPV4,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_EGRESS)
        acl_entry11 = self.add_acl_entry(self.device,
            dst_ip='10.10.10.1',
            dst_ip_mask='255.255.255.255',
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            table_handle=acl_table1)
        self.client.object_counters_clear_all(acl_entry11)

        acl_table2 = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IPV6,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_EGRESS)
        acl_entry21 = self.add_acl_entry(self.device,
            dst_ip='1234:5678:9abc:def0:4422:1133:5577:99aa',
            dst_ip_mask='FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF',
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            table_handle=acl_table2)
        self.client.object_counters_clear_all(acl_entry21)

        # ACL group members
        acl_grp_member1 = self.add_acl_group_member(self.device,
            acl_table_handle=acl_table1,
            acl_group_handle=acl_group1)
        acl_grp_member2 = self.add_acl_group_member(self.device,
            acl_table_handle=acl_table2,
            acl_group_handle=acl_group1)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, acl_group1)

        def TestTraffic():
            print(" Testing Traffic")
            # send from port0, verify v4 acl pkt-dp
            print(" Sending IPv4 packet port 0 -> port 1, pkt-dp")
            send_packet(self, self.devports[0], self.pkt)
            verify_no_other_packets(self, timeout=2)

            # send from port0, verify the v6 acl pkt-dp
            print(" Sending IPv6 packet port 0 -> port 1, pkt-dp")
            send_packet(self, self.devports[0], self.pkt_v6)
            verify_no_other_packets(self, timeout=2)

            # clear v6 acl_group member. test forw of port 1
            self.cleanlast()
            print(" Sending IPv6 packet port 0 -> port 1")
            send_packet(self, self.devports[0], self.pkt_v6)
            verify_packets(self, self.exp_pkt_v6, [self.devports[1]])

            # clear v4 acl_group member, test forw of port 1
            self.cleanlast()
            print(" Sending IPv4 packet port 0 -> port 1")
            send_packet(self, self.devports[0], self.pkt)
            verify_packets(self, self.exp_pkt, [self.devports[1]])

        def TestCleanup():
            # clear v6 acl_group member. test forw of port 1
            self.cleanlast()
            # clear v4 acl_group member, test forw of port 1
            self.cleanlast()

        try:
            TestTraffic()
            #  reprogram acl group members for retesting after FR or WI
            acl_grp_member1 = self.add_acl_group_member(self.device,
                acl_table_handle=acl_table1,
                acl_group_handle=acl_group1)
            acl_grp_member2 = self.add_acl_group_member(self.device,
                acl_table_handle=acl_table2,
                acl_group_handle=acl_group1)
            if (self.startFrWrReplay(1, TestTraffic) == False):
                TestCleanup()
        finally:
            # unset acl_group from port0
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, 0)

            # cleanup acl_entry21, acl_table2
            self.cleanlast()
            self.cleanlast()

            # cleanup acl_entry11, acl_table1
            self.cleanlast()
            self.cleanlast()

            # cleanup acl_group
            self.cleanlast()
    ############################################################################

    def IPv4AclTableUserMetaTest(self):
        '''
        This test tries to verify ipv4 acl ingress/egress acl table for set/drop user meta feature
        We send a packet from port 0 and set user metadata in ingress and match on user metadata
        and drop the packet in the egress. The bp_type is PORT. The port_lag_label comes from the table id
        '''
        if (self.client.is_feature_enable(SWITCH_FEATURE_ACL_USER_META) == 0):
            print("ACL user metadata feature not enabled, skipping")
            return
        table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IPV4
        if self.client.is_feature_enable(SWITCH_FEATURE_SHARED_INGRESS_IP_ACL):
            table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IP

        meta_range  = self.attribute_get(self.device, SWITCH_DEVICE_ATTR_ACL_USER_METADATA_RANGE);
        print("IPv4AclTableUserMetaTest()")
        if(meta_range.max == 0):
            print("ACL user metadata range=0, skipping")
            return
        else:
            print("ACL user metadata range - min={}, max={}".format(meta_range.min, meta_range.max))

        ig_acl_table = self.add_acl_table(self.device,
            type=table_type,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)

        # Add ACL entry (Match - IP SRC:192.168.0.0/22  IP DST:10.10.10.1/24 Action - Set User Meta {Min Value}) to IPv4 ACL Table
        ig_acl_min_meta_entry = self.add_acl_entry(self.device,
            dst_ip='10.10.10.1',
            dst_ip_mask='255.255.255.255',
            src_ip='192.168.0.0',
            src_ip_mask='255.255.255.252',
            set_user_metadata=meta_range.min+1,
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_PERMIT,
            table_handle=ig_acl_table)

        # Add ACL entry (Match - IP SRC:192.168.0.4/22  IP DST:10.10.10.1/24 Action - Set User Meta {Max Value}) to IPv4 ACL Table
        ig_acl_max_meta_entry = self.add_acl_entry(self.device,
            dst_ip='10.10.10.1',
            dst_ip_mask='255.255.255.255',
            src_ip='192.168.0.4',
            src_ip_mask='255.255.255.252',
            set_user_metadata=meta_range.max,
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_PERMIT,
            table_handle=ig_acl_table)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, ig_acl_table)


        eg_acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IPV4,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_EGRESS)

        # Add ACL entry (Match - IP SRC:192.168.0.1/24 USER META:Min Value Action - DROP to Egress IPv4 ACL Table
        eg_acl_min_meta_entry = self.add_acl_entry(self.device,
            src_ip='192.168.0.1',
            src_ip_mask='255.255.255.255',
            user_metadata=meta_range.min+1,
            user_metadata_mask=meta_range.max,
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            table_handle=eg_acl_table)

        # Add ACL entry (Match - IP SRC:192.168.0.4/24 USER META:Max Value Action - DROP to Egerss IPv4 ACL Table
        eg_acl_max_meta_entry = self.add_acl_entry(self.device,
            src_ip='192.168.0.4',
            src_ip_mask='255.255.255.255',
            user_metadata=meta_range.max,
            user_metadata_mask=meta_range.max,
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            table_handle=eg_acl_table)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, eg_acl_table)

        def TestTraffic():
            print(" Testing Traffic")
            # Test packets
            # Packet Ingress Match-> Set Meta Min -> Egress Match -> Egress Drop
            pkt_meta_min_drop = simple_tcp_packet(pktlen=pkt_len,
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='10.10.10.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_ttl=64)
            exp_pkt_meta_min_drop = simple_tcp_packet(pktlen=pkt_len,
                eth_dst='00:11:22:33:44:55',
                eth_src='00:77:66:55:44:33',
                ip_dst='10.10.10.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_ttl=63)

            # Packet Ingress Match-> Set Meta Min -> Egress Miss -> Forward
            pkt_meta_min_fwd = simple_tcp_packet(pktlen=pkt_len,
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='10.10.10.1',
                ip_src='192.168.0.2',
                ip_id=105,
                ip_ttl=64)
            exp_pkt_meta_min_fwd = simple_tcp_packet(pktlen=pkt_len,
                eth_dst='00:11:22:33:44:55',
                eth_src='00:77:66:55:44:33',
                ip_dst='10.10.10.1',
                ip_src='192.168.0.2',
                ip_id=105,
                ip_ttl=63)

            # Packet Ingress Match-> Set Meta Max -> Egress Match-> Drop
            pkt_meta_max_drop = simple_tcp_packet(pktlen=pkt_len,
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='10.10.10.1',
                ip_src='192.168.0.4',
                ip_id=105,
                ip_ttl=64)
            exp_pkt_meta_max_drop = simple_tcp_packet(pktlen=pkt_len,
                eth_dst='00:11:22:33:44:55',
                eth_src='00:77:66:55:44:33',
                ip_dst='10.10.10.1',
                ip_src='192.168.0.4',
                ip_id=105,
                ip_ttl=63)

            # Packet Ingress Miss-> Forward -> Egress Miss-> Forward
            pkt_meta_max_fwd = simple_tcp_packet(pktlen=pkt_len,
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='10.10.10.1',
                ip_src='192.168.0.7',
                ip_id=105,
                ip_ttl=64)
            exp_pkt_meta_max_fwd = simple_tcp_packet(pktlen=pkt_len,
                eth_dst='00:11:22:33:44:55',
                eth_src='00:77:66:55:44:33',
                ip_dst='10.10.10.1',
                ip_src='192.168.0.7',
                ip_id=105,
                ip_ttl=63)

            self.client.object_counters_clear_all(ig_acl_min_meta_entry)
            self.client.object_counters_clear_all(ig_acl_max_meta_entry)
            self.client.object_counters_clear_all(eg_acl_min_meta_entry)
            self.client.object_counters_clear_all(eg_acl_max_meta_entry)
            ig_meta_min_pre_counter = self.object_counters_get(ig_acl_min_meta_entry)
            ig_meta_max_pre_counter = self.object_counters_get(ig_acl_max_meta_entry)
            eg_meta_min_pre_counter = self.object_counters_get(eg_acl_min_meta_entry)
            eg_meta_max_pre_counter = self.object_counters_get(eg_acl_max_meta_entry)

            #Both Ingress and Egress ACL hit
            send_packet(self, self.devports[0], pkt_meta_min_drop)
            verify_no_other_packets(self, timeout=2)

            #Ingress ACL hit and Egress ACL miss
            send_packet(self, self.devports[0], pkt_meta_min_fwd)
            verify_packet(self, exp_pkt_meta_min_fwd, self.devports[1])

            #Both Ingress and Egress ACL hit
            send_packet(self, self.devports[0], pkt_meta_max_drop)
            verify_no_other_packets(self, timeout=2)

            #Ingress ACL miss and Egress ACL miss
            send_packet(self, self.devports[0], pkt_meta_max_fwd)
            verify_packet(self, exp_pkt_meta_max_fwd, self.devports[1])

            ig_meta_min_post_counter = self.object_counters_get(ig_acl_min_meta_entry)
            ig_meta_max_post_counter = self.object_counters_get(ig_acl_max_meta_entry)
            eg_meta_min_post_counter = self.object_counters_get(eg_acl_min_meta_entry)
            eg_meta_max_post_counter = self.object_counters_get(eg_acl_max_meta_entry)

            self.assertEqual(ig_meta_min_post_counter[0].count - ig_meta_min_pre_counter[0].count, 2)
            self.assertEqual(ig_meta_max_post_counter[0].count - ig_meta_max_pre_counter[0].count, 2)
            self.assertEqual(eg_meta_min_post_counter[0].count - eg_meta_min_pre_counter[0].count, 1)
            self.assertEqual(eg_meta_max_post_counter[0].count - eg_meta_max_pre_counter[0].count, 1)

        try:
            TestTraffic()
            self.startFrWrReplay(1, TestTraffic)
        finally:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, 0)
            #subtest cleanup to allow other subtests to be run before the final cleanup happens
            # remove the acl entries
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            # remove the acl tables
            self.cleanlast()
            self.cleanlast()

###############################################################################

@group('acl')
class ACLPortLagLabelTest(ApiHelper):
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
        self.pkt = simple_tcp_packet(pktlen=pkt_len,
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        self.exp_pkt = simple_tcp_packet(pktlen=pkt_len,
            eth_dst='00:11:22:33:44:55',
            eth_src='00:77:66:55:44:33',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63)
        self.exp_pkt2 = simple_tcp_packet(pktlen=pkt_len,
            eth_dst='00:11:22:33:44:56',
            eth_src='00:77:66:55:44:33',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63)
        self.pkt1 = simple_tcp_packet(pktlen=pkt_len,
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.10.10.5',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        self.exp_pkt1 = simple_tcp_packet(pktlen=pkt_len,
            eth_dst='00:55:55:55:55:55',
            eth_src='00:77:66:55:44:33',
            ip_dst='10.10.10.5',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63)
        self.lag_pkt = simple_tcp_packet(pktlen=pkt_len,
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.10.10.4',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        self.exp_lag_pkt = simple_tcp_packet(pktlen=pkt_len,
            eth_dst='00:11:22:33:44:57',
            eth_src='00:77:66:55:44:33',
            ip_dst='10.10.10.4',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63)
        self.l2_pkt1 = simple_tcp_packet(pktlen=pkt_len,
            eth_dst='00:55:55:55:55:55',
            eth_src='00:44:44:44:44:44',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        self.l2_pkt2 = simple_tcp_packet(pktlen=pkt_len,
            eth_dst='00:44:44:44:44:44',
            eth_src='00:55:55:55:55:55',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        self.vlan20_pkt1 = simple_tcp_packet(pktlen=pkt_len,
            eth_dst='00:66:66:66:66:66',
            eth_src='00:77:77:77:77:77',
            ip_dst='20.20.20.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        self.vlan20_pkt2 = simple_tcp_packet(pktlen=pkt_len,
            eth_dst='00:77:77:77:77:77',
            eth_src='00:66:66:66:66:66',
            ip_dst='20.20.20.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)

        self.pkt_v6 = simple_tcpv6_packet(pktlen=pkt_len,
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
            ipv6_src='2000::1',
            ipv6_hlim=64)
        self.exp_pkt_v6 = simple_tcpv6_packet(pktlen=pkt_len,
            eth_dst='00:11:22:33:44:55',
            eth_src='00:77:66:55:44:33',
            ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
            ipv6_src='2000::1',
            ipv6_hlim=63)

        try:
            self.PortLabelTest()
            self.LagLabelTest()

        finally:
            self.attribute_set(self.port2, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.port5, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.port4, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.lag1, SWITCH_LAG_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.port10, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.port11, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.cleanup()

    def PortLabelTest(self):
        '''
        This test tries to verify ipv4 acl entries
        We send a packet from port 0 and drop the packet
        The bp_type is NONE. The port_lag_label is chosen by the test
        '''
        print("PortLabelTest()")
        table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IPV4
        if self.client.is_feature_enable(SWITCH_FEATURE_SHARED_INGRESS_IP_ACL):
            table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IP

        acl_table = self.add_acl_table(self.device,
            type=table_type,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_entry = self.add_acl_entry(self.device,
            port_lag_label=2,
            port_lag_label_mask=0xFF,
            dst_ip='10.10.10.1',
            dst_ip_mask='255.255.255.255',
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry)
        table_name = SWITCH_DEVICE_ATTR_TABLE_IPV4_ACL
        if self.client.is_feature_enable(SWITCH_FEATURE_SHARED_INGRESS_IP_ACL):
            table_name = SWITCH_DEVICE_ATTR_TABLE_IP_ACL
        pre_hw_table_count = get_hw_table_usage(self, table_name)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_PORT_LAG_LABEL, 2)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, acl_table)
        post_hw_table_count = get_hw_table_usage(self, table_name)
        self.assertEqual(post_hw_table_count - pre_hw_table_count, 1)

        def TestTraffic():
            print(" Testing Traffic")
            pre_counter = self.object_counters_get(acl_entry)
            send_packet(self, self.devports[0], self.pkt)
            verify_no_other_packets(self, timeout=2)
            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)

        try:
            TestTraffic()
            self.startFrWrReplay(1, TestTraffic)
        finally:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_PORT_LAG_LABEL, 0)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            # remove the acl entry and table
            self.cleanlast()
            self.cleanlast()
    ############################################################################

    def LagLabelTest(self):
        print("LagLabelTest()")
        table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IPV4
        if self.client.is_feature_enable(SWITCH_FEATURE_SHARED_INGRESS_IP_ACL):
            table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IP

        acl_table = self.add_acl_table(self.device,
            type=table_type,
            bind_point_type=[acl_table_bp_lag],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_entry = self.add_acl_entry(self.device,
            port_lag_label=2,
            port_lag_label_mask=0xFF,
            dst_ip='10.10.10.1',
            dst_ip_mask='255.255.255.255',
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry)
        table_name = SWITCH_DEVICE_ATTR_TABLE_IPV4_ACL
        if self.client.is_feature_enable(SWITCH_FEATURE_SHARED_INGRESS_IP_ACL):
            table_name = SWITCH_DEVICE_ATTR_TABLE_IP_ACL
        pre_hw_table_count = get_hw_table_usage(self, table_name)
        self.attribute_set(self.lag0, SWITCH_LAG_ATTR_INGRESS_PORT_LAG_LABEL, 2)
        self.attribute_set(self.lag0, SWITCH_LAG_ATTR_INGRESS_ACL_HANDLE, acl_table)
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

        try:
            TestTraffic()
            self.startFrWrReplay(1, TestTraffic)
        finally:
            self.attribute_set(self.lag0, SWITCH_LAG_ATTR_INGRESS_PORT_LAG_LABEL, 0)
            self.attribute_set(self.lag0, SWITCH_LAG_ATTR_INGRESS_ACL_HANDLE, 0)
            # remove the acl entry and table
            self.cleanlast()
            self.cleanlast()
    ############################################################################


###############################################################################
@group('acl')
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
        self.pkt = simple_tcp_packet(pktlen=pkt_len,
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
        self.exp_pkt = simple_tcp_packet(pktlen=pkt_len,
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
        self.exp_pkt2 = simple_tcp_packet(pktlen=pkt_len,
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
        self.allow_pkt = simple_tcp_packet(pktlen=pkt_len,
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
        self.exp_allow_pkt = simple_tcp_packet(pktlen=pkt_len,
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

        self.pkt_v6 = simple_tcpv6_packet(pktlen=pkt_len,
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
            ipv6_src='2000::1',
            ipv6_dscp=2,
            tcp_sport=3333,
            tcp_dport=5555,
            tcp_flags="R",
            ipv6_hlim=64)
        self.exp_pkt_v6 = simple_tcpv6_packet(pktlen=pkt_len,
            eth_dst='00:11:22:33:44:55',
            eth_src='00:77:66:55:44:33',
            ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
            ipv6_src='2000::1',
            ipv6_dscp=2,
            tcp_sport=3333,
            tcp_dport=5555,
            tcp_flags="R",
            ipv6_hlim=63)
        self.allow_pkt_v6 = simple_tcpv6_packet(pktlen=pkt_len,
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99ab',
            ipv6_src='2000::2',
            ipv6_dscp=4,
            tcp_sport=3332,
            tcp_dport=5556,
            tcp_flags="S",
            ipv6_hlim=33)
        self.exp_allow_pkt_v6 = simple_tcpv6_packet(pktlen=pkt_len,
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
            self.PreIngressAclTableFieldTest()
            self.IPv4AclTableFieldTest()
            self.IPv6AclTableFieldTest()
            self.IPv4MirrorAclTableFieldTest()
            self.IPv6MirrorAclTableFieldTest()
            self.EgressIPv4AclTableFieldTest()
            self.EgressIPv6AclTableFieldTest()
            self.EgressIPv4MirrorAclTableFieldTest()
            self.EgressIPv6MirrorAclTableFieldTest()
            self.EgressIPMirrorAclTableFieldTest()
        finally:
            self.cleanup()

    def PreIngressAclTableFieldTest(self):
        print("PreIngressAclTableFieldTest()")
        if (self.client.is_feature_enable(SWITCH_FEATURE_PRE_INGRESS_ACL) == 0):
            print("PreIngressAcl feature not enabled, skipping")
            return
        print("Testing every field hit in this table")
        '''
        lkp.mac_src_addr : ternary;
        lkp.mac_dst_addr : ternary;
        lkp.mac_type : ternary;
        lkp.ip_src_addr : ternary;
        lkp.ip_dst_addr : ternary;
        lkp.ip_tos : ternary;
        ingress_port : ternary
        '''
        acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_PREINGRESS,
            bind_point_type=[acl_table_bp_switch],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_PREINGRESS)
        vrf20 = self.add_vrf(self.device, id=20, src_mac=self.rmac)
        self.add_route(self.device, ip_prefix='10.10.10.1', vrf_handle=vrf20, nexthop_handle=self.nhop2)

        rules = [
            ["{'src_mac': '00:22:22:22:22:22', 'src_mac_mask': 'ff:ff:ff:ff:ff:ff'}", True],
            ["{'dst_ip': '10.10.10.1', 'dst_ip_mask': '255.255.255.255'}", True],
            ["{'src_ip': '20.20.20.1', 'src_ip_mask': '255.255.255.255'}", True],
            ["{'ip_proto': 6, 'ip_proto_mask': 127}", False],
            ["{'ip_dscp': 2, 'ip_dscp_mask': 127}", True],
        ]

        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_PRE_INGRESS_ACL, acl_table)

        def TestTraffic():
            print(" Testing Traffic")
            print(" Send packet with no ACL rule")
            send_packet(self, self.devports[0], self.pkt)
            verify_packet(self, self.exp_pkt, self.devports[1])

            for rule in rules:
                rule_name = rule[0].split(', ')[0].split('\'')[1]

                print(" Add ACL entry to match and set vrf on %s field" % rule_name)
                acl_entry = self.add_acl_entry(self.device,
                    table_handle=acl_table,
                    action_set_vrf_handle=vrf20,
                    **literal_eval(rule[0]))
                send_packet(self, self.devports[0], self.pkt)
                verify_packet(self, self.exp_pkt2, self.devports[2])

                print(" Remove ACL and send packet with no ACL rule")
                self.cleanlast()
                send_packet(self, self.devports[0], self.pkt)
                verify_packet(self, self.exp_pkt, self.devports[1])

            print(" Add ACL entry to match and set vrf on field ingress port handle")
            acl_entry = self.add_acl_entry(self.device,
                table_handle=acl_table,
                action_set_vrf_handle=vrf20,
                ingress_port_lag_handle=self.port0)
            send_packet(self, self.devports[0], self.pkt)
            verify_packet(self, self.exp_pkt2, self.devports[2])

            print(" Remove ACL and send packet with no ACL rule")
            self.cleanlast()
            send_packet(self, self.devports[0], self.pkt)
            verify_packet(self, self.exp_pkt, self.devports[1])

        try:
            TestTraffic()
            self.startFrWrReplay(0, TestTraffic)   # WI or FI not testing
        finally:
            self.attribute_set(self.device, SWITCH_DEVICE_ATTR_PRE_INGRESS_ACL, 0)
            # remove the acl entry and table
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

    def IPAclTableFieldTest(self):
        print("IPAclTableFieldTest()")
        table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IP
        if self.client.is_feature_enable(SWITCH_FEATURE_SHARED_INGRESS_IP_ACL):
            print("This profile does not have shared IP ACL")
            return

        print("Testing every field hit in this table")
        '''
        #define INGRESS_IP_ACL_KEY
        ether_type : ternary;
        ipv4_src_addr : ternary;
        ipv4_dst_addr : ternary;
        ipv6_src_addr : ternary;
        ipv6_dst_addr : ternary;
        dmac : ternary;
        ip_proto : ternary;
        l4_src_port : ternary;
        l4_dst_port : ternary;
        ttl : ternary;
        ip_frag : ternary;
        tcp_flags : ternary;
        dstp : ternary;
        ecn : ternary;
        icmpv6_type : ternary;
        arp_tpa : ternary;
        in_port : ternary;
        '''
        acl_table = self.add_acl_table(self.device,
            type=table_type,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, acl_table)

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
            ["{'dst_ip': '1234:5678:9abc:def0:4422:1133:5577:99aa', 'dst_ip_mask': 'FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF'}", True, None],
            ["{'src_ip': '2000::1', 'src_ip_mask': 'FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF'}", True, None],
            ["{'ip_proto': 6, 'ip_proto_mask': 127}", False, None],
            ["{'ip_dscp': 2, 'ip_dscp_mask': 127}", True, None],
            ["{'l4_src_port': 3333, 'l4_src_port_mask': 32759}", True, None],
            ["{'l4_dst_port': 5555, 'l4_dst_port_mask': 32759}", True, None],
            ["{'ttl': 64, 'ttl_mask': 127}", True, None],
            ["{"+ "'ip_frag': {}".format(SWITCH_ACL_ENTRY_ATTR_IP_FRAG_HEAD) +"}", False, ip_frag_head],
            ["{"+ "'ip_frag': {}".format(SWITCH_ACL_ENTRY_ATTR_IP_FRAG_NON_HEAD) +"}", False, ip_frag_more_frag],
            ["{"+ "'ip_frag': {}".format(SWITCH_ACL_ENTRY_ATTR_IP_FRAG_NON_HEAD) +"}", False, ip_frag_last_frag],
            ["{"+ "'ip_frag': {}".format(SWITCH_ACL_ENTRY_ATTR_IP_FRAG_ANY) +"}", False, ip_frag_any],
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
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            # remove the acl entry and table
            self.cleanlast()

    def IPv4AclTableFieldTest(self):
        print("IPv4AclTableFieldTest()")
        table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IPV4
        if self.client.is_feature_enable(SWITCH_FEATURE_SHARED_INGRESS_IP_ACL):
            table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IP

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
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, acl_table)

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
            ["{'ttl': 64, 'ttl_mask': 127}", True, None],
            ["{"+ "'ip_frag': {}".format(SWITCH_ACL_ENTRY_ATTR_IP_FRAG_HEAD) +"}", False, ip_frag_head],
            ["{"+ "'ip_frag': {}".format(SWITCH_ACL_ENTRY_ATTR_IP_FRAG_NON_HEAD) +"}", False, ip_frag_more_frag],
            ["{"+ "'ip_frag': {}".format(SWITCH_ACL_ENTRY_ATTR_IP_FRAG_NON_HEAD) +"}", False, ip_frag_last_frag],
            ["{"+ "'ip_frag': {}".format(SWITCH_ACL_ENTRY_ATTR_IP_FRAG_ANY) +"}", False, ip_frag_any],
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
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            # remove the acl entry and table
            self.cleanlast()

    def IPv6AclTableFieldTest(self):
        print("IPv6AclTableFieldTest()")
        table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IPV6
        if self.client.is_feature_enable(SWITCH_FEATURE_SHARED_INGRESS_IP_ACL):
            table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IP

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
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, acl_table)

        rules = [
            ["{'dst_ip': '1234:5678:9abc:def0:4422:1133:5577:99aa', 'dst_ip_mask': 'FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF'}", True],
            ["{'src_ip': '2000::1', 'src_ip_mask': 'FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF'}", True],
            ["{'ip_proto': 6, 'ip_proto_mask': 127}", False],
            ["{'ip_dscp': 2, 'ip_dscp_mask': 127}", True],
            ["{'l4_src_port': 3333, 'l4_src_port_mask': 32759}", True],
            ["{'l4_dst_port': 5555, 'l4_dst_port_mask': 32759}", True],
            ["{'ttl': 64, 'ttl_mask': 127}", True],
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
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            # remove the acl entry and table
            self.cleanlast()

    def IPv4MirrorAclTableFieldTest(self):
        print("IPv4MirrorAclTableFieldTest()")
        if (self.client.is_feature_enable(SWITCH_FEATURE_INGRESS_MIRROR_ACL) == 0):
            print("MirrorAcl feature not enabled, skipping")
            return
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
        lkp.tcp_flags : ternary;
        lkp.mac_type : ternary;
        '''
        acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IP_MIRROR,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, acl_table)
        mirror = self.add_mirror(self.device,
            type=SWITCH_MIRROR_ATTR_TYPE_LOCAL,
            direction=SWITCH_MIRROR_ATTR_DIRECTION_INGRESS,
            egress_port_handle=self.port2)

        rules = [
            ["{'dst_ip': '10.10.10.1', 'dst_ip_mask': '255.255.255.255'}", True],
            ["{'src_ip': '20.20.20.1', 'src_ip_mask': '255.255.255.255'}", True],
            ["{'ip_proto': 6, 'ip_proto_mask': 127}", False],
            ["{'ip_dscp': 2, 'ip_dscp_mask': 127}", True],
            ["{'l4_src_port': 3333, 'l4_src_port_mask': 32759}", True],
            ["{'l4_dst_port': 5555, 'l4_dst_port_mask': 32759}", True],
            ["{'ttl': 64, 'ttl_mask': 127}", True],
            ["{'tcp_flags': 4, 'tcp_flags_mask': 127}", True],
            #["{'eth_type': 0x0800, 'eth_type_mask': 0x7FFF}", True],
        ]

        def TestTraffic():
            print(" Testing Traffic")
            print(" Send packet with no ACL rule")
            send_packet(self, self.devports[0], self.pkt)
            verify_packet(self, self.exp_pkt, self.devports[1])

            for rule in rules:
                rule_name = rule[0].split(', ')[0].split('\'')[1]

                print(" Add ACL entry to match and mirror on %s field" % rule_name)
                acl_entry = self.add_acl_entry(self.device,
                    action_ingress_mirror_handle=mirror,
                    table_handle=acl_table,
                    **literal_eval(rule[0]))

                send_packet(self, self.devports[0], self.pkt)
                verify_each_packet_on_each_port(self, [self.exp_pkt, self.pkt], [self.devports[1], self.devports[2]])

                if rule[1]:
                    print(" do not mirror unmatched packet")
                    send_packet(self, self.devports[0], self.allow_pkt)
                    verify_packet(self, self.exp_allow_pkt, self.devports[1])
                    verify_no_packet(self, self.allow_pkt, self.devports[2], timeout=2)

                print(" Remove ACL and send packet with no ACL rule")
                self.cleanlast()
                send_packet(self, self.devports[0], self.pkt)
                verify_packet(self, self.exp_pkt, self.devports[1])

        try:
            TestTraffic()
            self.startFrWrReplay(0, TestTraffic)   # WI or FI not testing
        finally:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            # remove the mirror entry and table
            self.cleanlast()
            self.cleanlast()

    def IPv6MirrorAclTableFieldTest(self):
        print("IPv6MirrorAclTableFieldTest()")
        if (self.client.is_feature_enable(SWITCH_FEATURE_INGRESS_MIRROR_ACL) == 0):
            print("MirrorAcl feature not enabled, skipping")
            return
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
        mirror = self.add_mirror(self.device,
            type=SWITCH_MIRROR_ATTR_TYPE_LOCAL,
            direction=SWITCH_MIRROR_ATTR_DIRECTION_INGRESS,
            egress_port_handle=self.port2)
        acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IP_MIRROR,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, acl_table)

        rules = [
            ["{'dst_ip': '1234:5678:9abc:def0:4422:1133:5577:99aa', 'dst_ip_mask': 'FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF'}", True],
            ["{'src_ip': '2000::1', 'src_ip_mask': 'FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF'}", True],
            ["{'ip_proto': 6, 'ip_proto_mask': 127}", False],
            ["{'ip_dscp': 2, 'ip_dscp_mask': 127}", True],
            ["{'l4_src_port': 3333, 'l4_src_port_mask': 32759}", True],
            ["{'l4_dst_port': 5555, 'l4_dst_port_mask': 32759}", True],
            ["{'ttl': 64, 'ttl_mask': 127}", True],
            ["{'tcp_flags': 4, 'tcp_flags_mask': 127}", True],
        ]

        def TestTraffic():
            print(" Testing Traffic")
            print(" Send packet with no ACL rule")
            send_packet(self, self.devports[0], self.pkt_v6)
            verify_packet(self, self.exp_pkt_v6, self.devports[1])

            for rule in rules:
                rule_name = rule[0].split(', ')[0].split('\'')[1]

                print(" Add ACL entry to match and mirror on %s field" % rule_name)
                acl_entry = self.add_acl_entry(self.device,
                    action_ingress_mirror_handle=mirror,
                    table_handle=acl_table,
                    **literal_eval(rule[0]))
                send_packet(self, self.devports[0], self.pkt_v6)
                verify_each_packet_on_each_port(self, [self.exp_pkt_v6, self.pkt_v6], [self.devports[1], self.devports[2]])

                if rule[1]:
                    print(" do not mirror unmatched packet")
                    send_packet(self, self.devports[0], self.allow_pkt_v6)
                    verify_packet(self, self.exp_allow_pkt_v6, self.devports[1])
                    verify_no_packet(self, self.allow_pkt_v6, self.devports[2], timeout=2)

                print(" Remove ACL and send packet with no ACL rule")
                self.cleanlast()
                send_packet(self, self.devports[0], self.pkt_v6)
                verify_packet(self, self.exp_pkt_v6, self.devports[1])

        try:
            TestTraffic()
            self.startFrWrReplay(1, TestTraffic)
        finally:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            # remove the mirror entry and ACL table
            self.cleanlast()
            self.cleanlast()

    def EgressIPv4AclTableFieldTest(self):
        print("EgressIPv4AclTableFieldTest()")
        print("Testing every field hit in this table")
        '''
        #define EGRESS_IPV4_ACL_KEY
        hdr.ipv4.src_addr : ternary;
        hdr.ipv4.dst_addr : ternary;
        hdr.ipv4.protocol : ternary;
        hdr.ipv4.diffserv : ternary;
        lkp.tcp_flags : ternary;
        lkp.l4_src_port : ternary;
        lkp.l4_dst_port : ternary;
        hdr.ethernet.ether_type : ternary;
        '''
        if (self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_IP_ACL) == 0):
            print("EgressIPv4Acl feature not enabled, skipping")
            return
        acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IPV4,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_EGRESS)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, acl_table)

        rules = [
            ["{'dst_ip': '10.10.10.1', 'dst_ip_mask': '255.255.255.255'}", True],
            ["{'src_ip': '20.20.20.1', 'src_ip_mask': '255.255.255.255'}", True],
            ["{'ip_proto': 6, 'ip_proto_mask': 127}", False],
            #["{'ip_dscp': 2, 'ip_dscp_mask': 127}", True],
            ["{'l4_src_port': 3333, 'l4_src_port_mask': 32759}", True],
            ["{'l4_dst_port': 5555, 'l4_dst_port_mask': 32759}", True],
            ["{'tcp_flags': 4, 'tcp_flags_mask': 127}", True],
            #["{'eth_type': 0x0800, 'eth_type_mask': 0x7FFF}", True],
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
                    table_handle=acl_table,
                    **literal_eval(rule[0]))
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

        try:
            TestTraffic()
            self.startFrWrReplay(0, TestTraffic)   # WI or FI not testing
        finally:
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, 0)
            # remove the acl entry and table
            self.cleanlast()

    def EgressIPv6AclTableFieldTest(self):
        print("EgressIPv6AclTableFieldTest()")
        print("Testing every field hit in this table")
        '''
        #define EGRESS_IPV6_ACL_KEY
        hdr.ipv6.src_addr : ternary;
        hdr.ipv6.dst_addr : ternary;
        hdr.ipv6.next_hdr : ternary;
        hdr.ipv6.traffic_class : ternary;
        lkp.tcp_flags : ternary;
        lkp.l4_src_port : ternary;
        lkp.l4_dst_port : ternary;
        '''
        if (self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_IP_ACL) == 0):
            print("EgressIPv6Acl feature not enabled, skipping")
            return
        acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IPV6,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_EGRESS)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, acl_table)

        rules = [
            ["{'dst_ip': '1234:5678:9abc:def0:4422:1133:5577:99aa', 'dst_ip_mask': 'FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF'}", True],
            ["{'src_ip': '2000::1', 'src_ip_mask': 'FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF'}", True],
            ["{'ip_proto': 6, 'ip_proto_mask': 127}", False],
            #["{'ip_dscp': 2, 'ip_dscp_mask': 127}", True],
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
            self.startFrWrReplay(1, TestTraffic)
        finally:
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, 0)
            # remove the acl entry and table
            self.cleanlast()

    def EgressIPv4MirrorAclTableFieldTest(self):
        print("EgressIPv4MirrorAclTableFieldTest()")
        if (self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_IP_ACL) == 0):
            print("EgressIPv4 mirror acl feature not enabled, skipping")
            return
        print("Testing every field hit in this table")
        '''
        #define EGRESS_IPV4_ACL_KEY
        hdr.ipv4.src_addr : ternary;
        hdr.ipv4.dst_addr : ternary;
        hdr.ipv4.protocol : ternary;
        hdr.ipv4.diffserv : ternary;
        lkp.tcp_flags : ternary;
        lkp.l4_src_port : ternary;
        lkp.l4_dst_port : ternary;
        hdr.ethernet.ether_type : ternary;
        '''
        acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IPV4,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_EGRESS)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, acl_table)
        mirror = self.add_mirror(self.device,
            type=SWITCH_MIRROR_ATTR_TYPE_LOCAL,
            direction=SWITCH_MIRROR_ATTR_DIRECTION_EGRESS,
            egress_port_handle=self.port2)

        rules = [
            ["{'dst_ip': '10.10.10.1', 'dst_ip_mask': '255.255.255.255'}", True],
            ["{'src_ip': '20.20.20.1', 'src_ip_mask': '255.255.255.255'}", True],
            ["{'ip_proto': 6, 'ip_proto_mask': 127}", False],
            #["{'ip_dscp': 2, 'ip_dscp_mask': 127}", True],
            ["{'l4_src_port': 3333, 'l4_src_port_mask': 32759}", True],
            ["{'l4_dst_port': 5555, 'l4_dst_port_mask': 32759}", True],
            ["{'tcp_flags': 4, 'tcp_flags_mask': 127}", True],
            #["{'eth_type': 0x0800, 'eth_type_mask': 0x7FFF}", True],
        ]

        def TestTraffic():
            print(" Testing Traffic")
            print(" Send packet with no ACL rule")
            send_packet(self, self.devports[0], self.pkt)
            verify_packet(self, self.exp_pkt, self.devports[1])

            for rule in rules:
                rule_name = rule[0].split(', ')[0].split('\'')[1]

                print(" Add ACL entry to match and mirror on %s field" % rule_name)
                acl_entry = self.add_acl_entry(self.device,
                    action_egress_mirror_handle=mirror,
                    table_handle=acl_table,
                    **literal_eval(rule[0]))
                send_packet(self, self.devports[0], self.pkt)
                verify_each_packet_on_each_port(self, [self.exp_pkt, self.exp_pkt], [self.devports[1], self.devports[2]])

                if rule[1]:
                    print(" do not mirror unmatched packet")
                    send_packet(self, self.devports[0], self.allow_pkt)
                    verify_packet(self, self.exp_allow_pkt, self.devports[1])
                    verify_no_packet(self, self.exp_allow_pkt, self.devports[2], timeout=2)

                print(" Remove ACL and send packet with no ACL rule")
                self.cleanlast()
                send_packet(self, self.devports[0], self.pkt)
                verify_packet(self, self.exp_pkt, self.devports[1])

        try:
            TestTraffic()
            self.startFrWrReplay(1, TestTraffic)
        finally:
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, 0)
            # remove the mirror entry and ACL table
            self.cleanlast()
            self.cleanlast()

    def EgressIPv6MirrorAclTableFieldTest(self):
        print("EgressIPv6MirrorAclTableFieldTest()")
        if (self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_IP_ACL) == 0):
            print("EgressIPv6 mirror acl feature not enabled, skipping")
            return
        print("Testing every field hit in this table")
        '''
        #define EGRESS_IPV6_ACL_KEY
        hdr.ipv6.src_addr : ternary;
        hdr.ipv6.dst_addr : ternary;
        hdr.ipv6.next_hdr : ternary;
        hdr.ipv6.traffic_class : ternary;
        lkp.tcp_flags : ternary;
        lkp.l4_src_port : ternary;
        lkp.l4_dst_port : ternary;
        '''
        acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IPV6,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_EGRESS)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, acl_table)
        mirror = self.add_mirror(self.device,
            type=SWITCH_MIRROR_ATTR_TYPE_LOCAL,
            direction=SWITCH_MIRROR_ATTR_DIRECTION_EGRESS,
            egress_port_handle=self.port2)

        rules = [
            ["{'dst_ip': '1234:5678:9abc:def0:4422:1133:5577:99aa', 'dst_ip_mask': 'FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF'}", True],
            ["{'src_ip': '2000::1', 'src_ip_mask': 'FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF'}", True],
            ["{'ip_proto': 6, 'ip_proto_mask': 127}", False],
            #["{'ip_dscp': 2, 'ip_dscp_mask': 127}", True],
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

                print(" Add ACL entry to match and mirror on %s field" % rule_name)
                acl_entry = self.add_acl_entry(self.device,
                    action_egress_mirror_handle=mirror,
                    table_handle=acl_table,
                    **literal_eval(rule[0]))
                send_packet(self, self.devports[0], self.pkt_v6)
                verify_each_packet_on_each_port(self, [self.exp_pkt_v6, self.exp_pkt_v6], [self.devports[1], self.devports[2]])

                if rule[1]:
                    print(" do not mirror unmatched packet")
                    send_packet(self, self.devports[0], self.allow_pkt_v6)
                    verify_packet(self, self.exp_allow_pkt_v6, self.devports[1])
                    verify_no_packet(self, self.exp_allow_pkt_v6, self.devports[2], timeout=2)

                print(" Remove ACL and send packet with no ACL rule")
                self.cleanlast()
                send_packet(self, self.devports[0], self.pkt_v6)
                verify_packet(self, self.exp_pkt_v6, self.devports[1])

        try:
            TestTraffic()
            self.startFrWrReplay(0, TestTraffic)   # WI or FI not testing
        finally:
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, 0)
            # remove the mirror entry and ACL table
            self.cleanlast()
            self.cleanlast()

    def EgressIPMirrorAclTableFieldTest(self):
        print("EgressIPMirrorAclTableFieldTest()")
        if (self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_IP_ACL) == 0):
            print("Egress mirror acl feature not enabled, skipping")
            return
        print("Testing every field hit in this table")
        '''
        #define EGRESS_IPV6_ACL_KEY
        hdr.ipv6.src_addr : ternary;
        hdr.ipv6.dst_addr : ternary;
        hdr.ipv6.next_hdr : ternary;
        hdr.ipv6.traffic_class : ternary;
        lkp.tcp_flags : ternary;
        lkp.l4_src_port : ternary;
        lkp.l4_dst_port : ternary;

        #define EGRESS_IPV4_ACL_KEY
        hdr.ipv4.src_addr : ternary;
        hdr.ipv4.dst_addr : ternary;
        hdr.ipv4.protocol : ternary;
        hdr.ipv4.diffserv : ternary;
        lkp.tcp_flags : ternary;
        lkp.l4_src_port : ternary;
        lkp.l4_dst_port : ternary;
        '''

        acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IP_MIRROR,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_EGRESS)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, acl_table)
        mirror = self.add_mirror(self.device,
            type=SWITCH_MIRROR_ATTR_TYPE_LOCAL,
            direction=SWITCH_MIRROR_ATTR_DIRECTION_EGRESS,
            egress_port_handle=self.port2)

        rules = [
            [
                ["{'dst_ip': '10.10.10.1', 'dst_ip_mask': '255.255.255.255'}", True],
                ["{'src_ip': '20.20.20.1', 'src_ip_mask': '255.255.255.255'}", True],
                ["{'ip_proto': 6, 'ip_proto_mask': 127}", False],
                #["{'ip_dscp': 2, 'ip_dscp_mask': 127}", True],
                ["{'l4_src_port': 3333, 'l4_src_port_mask': 32759}", True],
                ["{'l4_dst_port': 5555, 'l4_dst_port_mask': 32759}", True],
                ["{'tcp_flags': 4, 'tcp_flags_mask': 127}", True],
            ],
            [
                ["{'dst_ip': '1234:5678:9abc:def0:4422:1133:5577:99aa', 'dst_ip_mask': 'FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF'}", True],
                ["{'src_ip': '2000::1', 'src_ip_mask': 'FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF'}", True],
                ["{'ip_proto': 6, 'ip_proto_mask': 127, 'src_ip': '2000::1', 'src_ip_mask': 'FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF'}", False],
                #["{'ip_dscp': 2, 'ip_dscp_mask': 127, 'src_ip': '2000::1', 'src_ip_mask': 'FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF'}", True],
                ["{'l4_src_port': 3333, 'l4_src_port_mask': 32759, 'src_ip': '2000::1', 'src_ip_mask': 'FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF'}", True],
                ["{'l4_dst_port': 5555, 'l4_dst_port_mask': 32759, 'src_ip': '2000::1', 'src_ip_mask': 'FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF'}", True],
                ["{'tcp_flags': 4, 'tcp_flags_mask': 127, 'src_ip': '2000::1', 'src_ip_mask': 'FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF'}", True],
            ]
        ]

        def TestTraffic():
            print(" Testing Traffic")
            print(" Send packet with no ACL rule")
            send_packet(self, self.devports[0], self.pkt)
            verify_packet(self, self.exp_pkt, self.devports[1])
            send_packet(self, self.devports[0], self.pkt_v6)
            verify_packet(self, self.exp_pkt_v6, self.devports[1])

            for rule4, rule6 in zip(rules[0], rules[1]):
                for rule in [rule4, rule6]:
                    rule_name = rule[0].split(', ')[0].split('\'')[1]

                    print(" Add %s ACL entry to match and mirror on %s field" % ("IPv4" if rule[0] == rule4[0] else "IPv6", rule_name))
                    acl_entry = self.add_acl_entry(self.device,
                        action_egress_mirror_handle=mirror,
                        table_handle=acl_table,
                        **literal_eval(rule[0]))

                for rule in [rule4, rule6]:
                    self.curr_pkt = self.pkt if rule[0] == rule4[0] else self.pkt_v6
                    self.curr_exp_pkt = self.exp_pkt if rule[0] == rule4[0] else self.exp_pkt_v6
                    self.curr_allow_pkt = self.allow_pkt if rule[0] == rule4[0] else self.allow_pkt_v6
                    self.curr_exp_allow_pkt = self.exp_allow_pkt if rule[0] == rule4[0] else self.exp_allow_pkt_v6

                    send_packet(self, self.devports[0], self.curr_pkt)
                    verify_each_packet_on_each_port(self, [self.curr_exp_pkt, self.curr_exp_pkt], [self.devports[1], self.devports[2]])

                    if rule[1]:
                        print(" do not mirror unmatched %s packet" % ("IPv4" if rule[0] == rule4[0] else "IPv6"))
                        send_packet(self, self.devports[0], self.curr_allow_pkt)
                        verify_packet(self, self.curr_exp_allow_pkt, self.devports[1])
                        verify_no_packet(self, self.curr_exp_allow_pkt, self.devports[2], timeout=2)

                for rule in [rule4, rule6]:
                    print(" Remove ACL and send %s packet with no ACL rule" % ("IPv4" if rule[0] == rule4[0] else "IPv6"))
                    self.cleanlast()
                    send_packet(self, self.devports[0], self.curr_pkt)
                    verify_packet(self, self.curr_exp_pkt, self.devports[1])

        try:
            TestTraffic()
            self.startFrWrReplay(0, TestTraffic)   # WI or FI not testing
        finally:
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, 0)
            # remove the mirror entry and ACL table
            self.cleanlast()
            self.cleanlast()

###############################################################################


@group('acl')
class AclMeterTest(ApiHelper):
    pkt = simple_tcp_packet(pktlen=pkt_len,
                eth_dst='00:11:11:11:11:11',
                eth_src='00:22:22:22:22:22',
                ip_dst='10.10.10.1',
                ip_ttl=64)
    def setUp(self):
        print()
        self.configure()

        self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_LEARNING, False)
        self.queue_handles = self.attribute_get(self.cpu_port_hdl, SWITCH_PORT_ATTR_QUEUE_HANDLES)

        vlan_mbr0 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port0)
        vlan_mbr1 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port1)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)

        mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:11:11:11:11:11', destination_handle=self.port1)
        mac1 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:22:22:22:22:22', destination_handle=self.port0)

        self.hostif_trap_group = self.add_hostif_trap_group(self.device,
            queue_handle=self.queue_handles[2].oid,
            admin_state=True)
        self.udt_trap = self.add_hostif_user_defined_trap(self.device,
          type=SWITCH_HOSTIF_USER_DEFINED_TRAP_ATTR_TYPE_ACL,
          hostif_trap_group_handle=self.hostif_trap_group,
          priority=20)

    def runTest(self):
        if (self.client.is_feature_enable(SWITCH_FEATURE_INGRESS_ACL_METER) == 0):
            print("Ingress ACL meter feature not enabled, skipping")
            return
        try:
            self.IngressAclPermitMeterTest()
            self.IngressAclTrapMeterTest()
            self.IngressAclCopyMeterTest()
            self.IngressAclMeterUpdateTest()
        finally:
            pass

    def tearDown(self):
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.cleanup()

    def IngressAclPermitMeterTest(self):
        print("IngressAclPermitMeterTest")
        table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IPV4
        if self.client.is_feature_enable(SWITCH_FEATURE_SHARED_INGRESS_IP_ACL):
            table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IP

        meter = self.add_meter(self.device,
            mode=SWITCH_METER_ATTR_MODE_TWO_RATE_THREE_COLOR,
            green_packet_action=SWITCH_METER_ATTR_RED_PACKET_ACTION_PERMIT,
            yellow_packet_action=SWITCH_METER_ATTR_RED_PACKET_ACTION_DROP,
            red_packet_action=SWITCH_METER_ATTR_RED_PACKET_ACTION_DROP,
            cbs=10,cir=10,
            type=SWITCH_METER_ATTR_TYPE_PACKETS,
            color_source=SWITCH_METER_ATTR_COLOR_SOURCE_BLIND)

        acl_table = self.add_acl_table(self.device,
            type=table_type,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)

        acl_entry = self.add_acl_entry(self.device,
            dst_ip='10.10.10.1',
            dst_ip_mask='255.255.255.255',
            action_meter_handle=meter,
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_PERMIT,
            table_handle=acl_table)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, acl_table)

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

            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 20)

            print("Metered: ", metered_packets, "total: ", total_packets)
            self.assertEqual(metered_packets, total_packets)

        finally:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            # remove the acl entry, table and mirror
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

    def IngressAclTrapMeterTest(self):
        print("IngressAclTrapMeterTest")
        table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IPV4
        if self.client.is_feature_enable(SWITCH_FEATURE_SHARED_INGRESS_IP_ACL):
            table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IP

        meter = self.add_meter(self.device,
            mode=SWITCH_METER_ATTR_MODE_TWO_RATE_THREE_COLOR,
            green_packet_action=SWITCH_METER_ATTR_RED_PACKET_ACTION_TRAP,
            yellow_packet_action=SWITCH_METER_ATTR_RED_PACKET_ACTION_DROP,
            red_packet_action=SWITCH_METER_ATTR_RED_PACKET_ACTION_DROP,
            cbs=10,cir=10,
            type=SWITCH_METER_ATTR_TYPE_PACKETS,
            color_source=SWITCH_METER_ATTR_COLOR_SOURCE_BLIND)

        acl_table = self.add_acl_table(self.device,
            type=table_type,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)

        acl_entry = self.add_acl_entry(self.device,
            dst_ip='10.10.10.1',
            dst_ip_mask='255.255.255.255',
            action_meter_handle=meter,
            action_hostif_user_defined_trap_handle=self.udt_trap,
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_PERMIT,
            table_handle=acl_table)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, acl_table)

        try:
            pre_counter = self.client.object_counters_get(acl_entry)
            pre_q2_counter = self.object_counters_get(self.queue_handles[2].oid)
            total_packets = 20
            for _ in range(0, total_packets):
              send_packet(self, self.devports[0], self.pkt)
            print("Waiting 5 seconds")
            time.sleep(5)

            metered_packets = 0
            counter1 = self.client.object_counters_get(meter)
            g = SWITCH_METER_COUNTER_ID_GREEN_PACKETS
            for i in range(0, 3):
              metered_packets += counter1[i].count

            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 20)

            print("Metered: ", metered_packets, "total: ", total_packets)
            self.assertEqual(metered_packets, total_packets)

            post_q2_counter = self.object_counters_get(self.queue_handles[2].oid)
            x = SWITCH_QUEUE_COUNTER_ID_STAT_PACKETS
            q_count = post_q2_counter[x].count - pre_q2_counter[x].count
            print("Metered green: ", counter1[g].count, "Q2 count: ", q_count)
            self.assertEqual(counter1[g].count, q_count)

        finally:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            # remove the acl entry, table and mirror
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

    def IngressAclCopyMeterTest(self):
        print("IngressAclCopyMeterTest")
        table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IPV4
        if self.client.is_feature_enable(SWITCH_FEATURE_SHARED_INGRESS_IP_ACL):
            table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IP

        meter = self.add_meter(self.device,
            mode=SWITCH_METER_ATTR_MODE_TWO_RATE_THREE_COLOR,
            green_packet_action=SWITCH_METER_ATTR_RED_PACKET_ACTION_COPY,
            yellow_packet_action=SWITCH_METER_ATTR_RED_PACKET_ACTION_PERMIT,
            red_packet_action=SWITCH_METER_ATTR_RED_PACKET_ACTION_PERMIT,
            cbs=10,cir=10,
            type=SWITCH_METER_ATTR_TYPE_PACKETS,
            color_source=SWITCH_METER_ATTR_COLOR_SOURCE_BLIND)

        acl_table = self.add_acl_table(self.device,
            type=table_type,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)

        acl_entry = self.add_acl_entry(self.device,
            dst_ip='10.10.10.1',
            dst_ip_mask='255.255.255.255',
            action_meter_handle=meter,
            action_hostif_user_defined_trap_handle=self.udt_trap,
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_PERMIT,
            table_handle=acl_table)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, acl_table)

        try:
            pre_counter = self.client.object_counters_get(acl_entry)
            pre_q2_counter = self.object_counters_get(self.queue_handles[2].oid)
            total_packets = 20
            for _ in range(0, total_packets):
              send_packet(self, self.devports[0], self.pkt)
              verify_packet(self, self.pkt, self.devports[1])
            print("Waiting 5 seconds")
            time.sleep(5)

            metered_packets = 0
            counter1 = self.client.object_counters_get(meter)
            g = SWITCH_METER_COUNTER_ID_GREEN_PACKETS
            for i in range(0, 3):
              metered_packets += counter1[i].count

            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 20)

            print("Metered: ", metered_packets, "total: ", total_packets)
            self.assertEqual(metered_packets, total_packets)

            post_q2_counter = self.object_counters_get(self.queue_handles[2].oid)
            x = SWITCH_QUEUE_COUNTER_ID_STAT_PACKETS
            q_count = post_q2_counter[x].count - pre_q2_counter[x].count
            print("Metered green: ", counter1[g].count, "Q2 count: ", q_count)
            self.assertEqual(counter1[g].count, q_count)

        finally:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            # remove the acl entry, table and mirror
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

    def IngressAclMeterUpdateTest(self):
        print("IngressAclMeterUpdateTest")
        table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IPV4
        if self.client.is_feature_enable(SWITCH_FEATURE_SHARED_INGRESS_IP_ACL):
            table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IP

        meter = self.add_meter(self.device,
            mode=SWITCH_METER_ATTR_MODE_TWO_RATE_THREE_COLOR,
            red_packet_action=SWITCH_METER_ATTR_RED_PACKET_ACTION_DROP,
            cbs=10,cir=10,
            type=SWITCH_METER_ATTR_TYPE_PACKETS,
            color_source=SWITCH_METER_ATTR_COLOR_SOURCE_BLIND)

        acl_table = self.add_acl_table(self.device,
            type=table_type,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)

        acl_entry = self.add_acl_entry(self.device,
            dst_ip='10.10.10.1',
            dst_ip_mask='255.255.255.255',
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_PERMIT,
            table_handle=acl_table)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, acl_table)

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

            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 20)

            print("Metered: ", metered_packets, "total: ", total_packets)
            self.assertEqual(metered_packets, 0)

            print("Update ACL entry meter handle")
            self.attribute_set(acl_entry, SWITCH_ACL_ENTRY_ATTR_ACTION_METER_HANDLE, meter)
            pre_counter = self.client.object_counters_get(acl_entry)
            for _ in range(0, total_packets):
              send_packet(self, self.devports[0], self.pkt)
            print("Waiting 5 seconds")
            time.sleep(5)

            metered_packets = 0
            counter1 = self.client.object_counters_get(meter)
            for i in range(0, 3):
              metered_packets += counter1[i].count

            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 20)

            print("Metered: ", metered_packets, "total: ", total_packets)
            self.assertEqual(metered_packets, total_packets)

        finally:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            # remove the acl entry, table and mirror
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

###############################################################################

@group('acl')
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
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.attribute_set(self.port2, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)

        mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:11:11:11:11:11', destination_handle=self.port1)
        mac1 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:22:22:22:22:22', destination_handle=self.port0)

        try:
            self.IngressMirrorMeterAclTest()
            self.EgressMirrorMeterAclTest()
            self.EgressEnhancedRemote3IPv4MirrorAclTest()
            self.IPv4MirrorAclTest()
            self.IPv6MirrorAclTest()
            self.IPv4MirrorAclTruncateTest()
        finally:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.port2, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.cleanup()

    def IngressMirrorMeterAclTest(self):
        '''
        This test tries to verify rate limiting for Ingress mirror.
        '''
        if ((self.client.is_feature_enable(SWITCH_FEATURE_MIRROR_METER) == 0) or
            (self.client.is_feature_enable(SWITCH_FEATURE_INGRESS_MIRROR_ACL) == 0)):
            print("Ingress ACL mirror or meter feature not enabled, skipping")
            return
        print("Ingress Acl mirroring from 0 -> 1, 2 and Apply Rate Limiter")

        meter = self.add_meter(self.device,
            mode=SWITCH_METER_ATTR_MODE_TWO_RATE_THREE_COLOR,
            red_packet_action=SWITCH_METER_ATTR_RED_PACKET_ACTION_DROP,
            cbs=10,cir=10,
            type=SWITCH_METER_ATTR_TYPE_PACKETS,
            color_source=SWITCH_METER_ATTR_COLOR_SOURCE_BLIND)

        mirror = self.add_mirror(self.device,
            type=SWITCH_MIRROR_ATTR_TYPE_LOCAL,
            direction=SWITCH_MIRROR_ATTR_DIRECTION_INGRESS,
            egress_port_handle=self.port2,
            meter_handle=meter
            )

        acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IP_MIRROR,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)

        acl_entry = self.add_acl_entry(self.device,
            dst_ip='10.10.10.1',
            dst_ip_mask='255.255.255.255',
            action_ingress_mirror_handle=mirror,
            table_handle=acl_table)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, acl_table)
        pkt = simple_tcp_packet(pktlen=pkt_len,
                eth_dst='00:11:11:11:11:11',
                eth_src='00:22:22:22:22:22',
                ip_dst='10.10.10.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_ttl=64)

        try:
            total_packets = 40
            send_packet(self, self.devports[0], pkt, total_packets)
            print("Waiting 10 seconds")
            time.sleep(10)

            original_packets = count_matched_packets(self, pkt, self.devports[1])
            mirrored_packets = count_matched_packets(self, pkt, self.devports[2])

            print("{} > {}".format(original_packets, mirrored_packets))
            self.assertTrue(original_packets > mirrored_packets)

        finally:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            # remove the acl entry, table and mirror
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

    def EgressMirrorMeterAclTest(self):
        '''
        This test tries to verify rate limiting for Egress mirror.
        '''
        if ((self.client.is_feature_enable(SWITCH_FEATURE_MIRROR_METER) == 0) or
            (self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_IP_ACL) == 0)):
            print("Egress ACL mirror or meter feature not enabled, skipping")
            return
        print("Egress Acl mirroring from 0 -> 1, 2 and Apply Rate Limiter")

        meter = self.add_meter(self.device,
            mode=SWITCH_METER_ATTR_MODE_TWO_RATE_THREE_COLOR,
            red_packet_action=SWITCH_METER_ATTR_RED_PACKET_ACTION_DROP,
            cbs=10,cir=10,
            type=SWITCH_METER_ATTR_TYPE_PACKETS,
            color_source=SWITCH_METER_ATTR_COLOR_SOURCE_BLIND)

        mirror = self.add_mirror(self.device,
            type=SWITCH_MIRROR_ATTR_TYPE_LOCAL,
            direction=SWITCH_MIRROR_ATTR_DIRECTION_EGRESS,
            egress_port_handle=self.port2,
            meter_handle=meter
            )

        acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IPV4,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_EGRESS)

        acl_entry = self.add_acl_entry(self.device,
            dst_ip='10.10.10.1',
            dst_ip_mask='255.255.255.255',
            action_egress_mirror_handle=mirror,
            table_handle=acl_table)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, acl_table)
        pkt = simple_tcp_packet(pktlen=pkt_len,
                eth_dst='00:11:11:11:11:11',
                eth_src='00:22:22:22:22:22',
                ip_dst='10.10.10.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_ttl=64)
        try:
            total_packets = 40
            send_packet(self, self.devports[0], pkt, total_packets)
            print("Waiting 10 seconds")
            time.sleep(10)

            original_packets = count_matched_packets(self, pkt, self.devports[1])
            mirrored_packets = count_matched_packets(self, pkt, self.devports[2])

            print("{} > {}".format(original_packets, mirrored_packets))
            self.assertTrue(original_packets > mirrored_packets)

        finally:
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, 0)
            # remove the acl entry, table and mirror
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

    def EgressEnhancedRemote3IPv4MirrorAclTest(self):
        '''
        This test tries to verify ipv4 acl entries
        We send a packet from port 0 and drop the packet
        The bp_type is PORT. The port_lag_label comes from the table id
        '''
        print("EgressEnhancedRemote3IPv4MirrorAclTest()")
        if (self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_IP_ACL) == 0):
            print("EgressIPv4 mirror acl feature not enabled, skipping")
            return

        print("Erspan egress ACL mirror packet from 0 -> 1, mirror to 3")

        mirror = self.add_mirror(self.device,
            type=SWITCH_MIRROR_ATTR_TYPE_ENHANCED_REMOTE,
            direction=SWITCH_MIRROR_ATTR_DIRECTION_EGRESS,
            rspan_type=SWITCH_MIRROR_ATTR_RSPAN_TYPE_NONE,
            erspan_type=SWITCH_MIRROR_ATTR_ERSPAN_TYPE_ERSPAN_3,
            ttl=64,
            platform_info=False,
            egress_port_handle=self.port3,
            src_mac_address='00:33:33:33:33:33',
            dest_mac_address='00:44:44:44:44:44',
            src_ip='10.10.10.1',
            dest_ip='20.20.20.1')
        session_id = self.attribute_get(mirror, SWITCH_MIRROR_ATTR_SESSION_ID)
        acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IPV4,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_EGRESS)
        acl_entry = self.add_acl_entry(self.device,
            dst_ip='20.0.0.1',
            dst_ip_mask='255.255.255.255',
            action_egress_mirror_handle=mirror,
            table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, acl_table)
        pre_counter = self.object_counters_get(acl_entry)

        pkt = simple_udp_packet(pktlen=pkt_len,
            eth_dst='00:11:11:11:11:11',
            eth_src='00:22:22:22:22:22',
            ip_src='10.0.0.1',
            ip_dst='20.0.0.1',
            ip_ttl=64)

        exp_mirrored_pkt = ptf.mask.Mask(ipv4_erspan_pkt(pktlen=pkt_len,
            eth_dst='00:44:44:44:44:44',
            eth_src='00:33:33:33:33:33',
            ip_src='10.10.10.1',
            ip_dst='20.20.20.1',
            ip_id=0,
            ip_ttl=64,
            ip_flags=0x0,
            version=self.ERSPAN_3,
            mirror_id=session_id,
            erspan_gra=2,
            inner_frame=pkt))
        # IPv4.total_length is different on Model and Harware because of 4B CRC.
        mask_set_do_not_care_packet(exp_mirrored_pkt, IP, "len")
        mask_set_do_not_care_packet(exp_mirrored_pkt, IP, "chksum")
        mask_set_do_not_care_packet(exp_mirrored_pkt, ERSPAN_III, "timestamp")

        try:
            send_packet(self, self.devports[0], pkt)
            p1 = [self.devports[1], [pkt]]
            p2 = [self.devports[3], [exp_mirrored_pkt]]
            verify_each_packet_on_each_port(self, [pkt, exp_mirrored_pkt], [self.devports[1], self.devports[3]])
            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)
        finally:
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, 0)
            # remove the acl entry, table and mirror
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

    def IPv4MirrorAclTest(self):
        '''
        This test tries to verify ipv4 acl entries
        We send a packet from port 0 and drop the packet
        The bp_type is PORT. The port_lag_label comes from the table id
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
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_entry = self.add_acl_entry(self.device,
            dst_ip='10.10.10.1',
            dst_ip_mask='255.255.255.255',
            action_ingress_mirror_handle=mirror,
            table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, acl_table)
        pre_counter = self.object_counters_get(acl_entry)
        try:
            pkt = simple_tcp_packet(pktlen=pkt_len,
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
        finally:

            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            # remove the acl entry, table and mirror
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

    def IPv6MirrorAclTest(self):
        '''
        This test tries to verify ipv4 acl entries
        We send a packet from port 0 and drop the packet
        The bp_type is PORT. The port_lag_label comes from the table id
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
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_entry = self.add_acl_entry(self.device,
            dst_ip='4000::2',
            dst_ip_mask='FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF',
            action_ingress_mirror_handle=mirror,
            table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, acl_table)
        pre_counter = self.object_counters_get(acl_entry)
        try:
            pkt = simple_tcpv6_packet(pktlen=pkt_len,
                eth_dst='00:11:11:11:11:11',
                eth_src='00:22:22:22:22:22',
                ipv6_dst='4000::2',
                ipv6_src='2000::1',
                ipv6_hlim=64)
            send_packet(self, self.devports[0], pkt)
            verify_packets(self, pkt, [self.devports[1], self.devports[2]])
            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)
        finally:

            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            # remove the acl entry, table and mirror
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

    def IPv4MirrorAclTruncateTest(self):
        '''
        This test verifies ipv4 acl entries that specify truncation.
        We send a packet from port 0 and drop the packet
        The bp_type is PORT. The port_lag_label comes from the table id
        '''
        print("IPv4MirrorAclTruncateTest()")
        if (self.client.is_feature_enable(SWITCH_FEATURE_INGRESS_MIRROR_ACL) == 0):
            print("MirrorAcl feature not enabled, skipping")
            return
        print("Acl mirroring from 0 -> 1, 2")
        mirror = self.add_mirror(self.device,
            max_pkt_len=100,
            type=SWITCH_MIRROR_ATTR_TYPE_LOCAL,
            direction=SWITCH_MIRROR_ATTR_DIRECTION_INGRESS,
            session_type=SWITCH_MIRROR_ATTR_SESSION_TYPE_TRUNCATE,
            egress_port_handle=self.port2)
        acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IP_MIRROR,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_entry = self.add_acl_entry(self.device,
            dst_ip='10.10.10.1',
            dst_ip_mask='255.255.255.255',
            action_ingress_mirror_handle=mirror,
            table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, acl_table)
        pre_counter = self.object_counters_get(acl_entry)
        try:
            pkt = simple_tcp_packet(
                pktlen=200,
                eth_dst='00:11:11:11:11:11',
                eth_src='00:22:22:22:22:22',
                ip_dst='10.10.10.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_ttl=64)
            exp_pkt = []
            for i in range(0, 4):
                truncate_amount = -100 + i
                exp_pkt.append(truncate_packet(pkt, -truncate_amount))
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, pkt, self.devports[1])
            verify_any_packet_on_ports_list(
                self,
                [exp_pkt[0], exp_pkt[1], exp_pkt[2], exp_pkt[3]],
                [[self.devports[2]]])
            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)
        finally:

            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            # remove the acl entry, table and mirror
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

###############################################################################


@group('acl')
@group('mirror')
class AclMirrorInOutTest(ApiHelper):
    ERSPAN_2 = 1
    ERSPAN_3 = 2

    def runTest(self):
        print()
        self.configure()
        rif0 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port0, vrf_handle=self.vrf10, src_mac=self.rmac)
        rif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port1, vrf_handle=self.vrf10, src_mac=self.rmac)
        nhop = self.add_nexthop(self.device, handle=rif, dest_ip='10.10.10.1')
        neighbor = self.add_neighbor(self.device, mac_address='00:11:22:33:44:57', handle=rif, dest_ip='10.10.10.1')
        route = self.add_route(self.device, ip_prefix='10.10.0.0/16', vrf_handle=self.vrf10, nexthop_handle=nhop)

        try:
            self.IngressAclMirrorInMeterTest()
            self.IngressAclMirrorOutMeterTest()
            self.EgressAclMirrorInMeterTest()
            self.EgressAclERSpanMirrorInTest()
            self.EgressAclMirrorOutMeterTest()

        finally:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.port2, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.cleanup()

    def IngressAclMirrorInMeterTest(self):
        '''
        This test tries to verify rate limiting for Mirror in-pkt using Ingress Acl
        '''
        if ((self.client.is_feature_enable(SWITCH_FEATURE_MIRROR_METER) == 0) or
            (self.client.is_feature_enable(SWITCH_FEATURE_INGRESS_MIRROR_ACL) == 0)):
            print("Ingress IP mirror ACL, mirror in or meter feature not enabled, skipping")
            return
        print("Ingress Acl on port 0 -> Egress port 1, Mirror in-pkt on port 2, Apply Rate Limit")

        meter = self.add_meter(self.device,
            mode=SWITCH_METER_ATTR_MODE_TWO_RATE_THREE_COLOR,
            red_packet_action=SWITCH_METER_ATTR_RED_PACKET_ACTION_DROP,
            cbs=10, cir=10,
            type=SWITCH_METER_ATTR_TYPE_PACKETS,
            color_source=SWITCH_METER_ATTR_COLOR_SOURCE_BLIND)

        mirror = self.add_mirror(self.device,
            type=SWITCH_MIRROR_ATTR_TYPE_LOCAL,
            direction=SWITCH_MIRROR_ATTR_DIRECTION_INGRESS,
            egress_port_handle=self.port2,
            meter_handle=meter)

        acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IP_MIRROR,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)

        acl_entry = self.add_acl_entry(self.device,
            dst_ip='10.10.10.1',
            dst_ip_mask='255.255.255.255',
            action_ingress_mirror_handle=mirror,
            table_handle=acl_table)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, acl_table)

        pkt = simple_tcp_packet(pktlen=pkt_len,
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='10.10.10.1',
                ip_src='192.168.0.1',
                ip_id=106,
                ip_ttl=64)
        e_pkt = simple_tcp_packet(pktlen=pkt_len,
                eth_dst='00:11:22:33:44:57',
                eth_src='00:77:66:55:44:33',
                ip_dst='10.10.10.1',
                ip_src='192.168.0.1',
                ip_id=106,
                ip_ttl=63)

        try:
            total_packets = 10
            send_packet(self, self.devports[0], pkt, total_packets)
            print("  Waiting 10 secs")
            time.sleep(10)

            outgoing_packets = count_matched_packets(self, e_pkt, self.devports[1])
            mirror_packets = count_matched_packets(self, pkt, self.devports[2])

            print("  {}[Total Pkt] = {}[Outgoing Pkt] > {}[Mirror in-Pkt Rate-Lim]".format(total_packets, outgoing_packets, mirror_packets))
            self.assertTrue(total_packets == outgoing_packets)
            self.assertTrue(total_packets > mirror_packets)
            self.assertTrue(outgoing_packets > mirror_packets)

        finally:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            # remove the acl entry, table, mirror, meter
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

    def IngressAclMirrorOutMeterTest(self):
        '''
        This test tries to verify rate limiting for Mirror out-pkt using Ingress Acl
        '''
        if ((self.client.is_feature_enable(SWITCH_FEATURE_MIRROR_METER) == 0) or
            (self.client.is_feature_enable(SWITCH_FEATURE_INGRESS_MIRROR_ACL) == 0) or
            (self.client.is_feature_enable(SWITCH_FEATURE_INGRESS_MIRROR_ACL_MIRROR_IN_OUT) == 0)):
            print("Ingress IP mirror ACL, mirror out or meter feature not enabled, skipping")
            return
        print("Ingress Acl on port 0 -> Egress port 1, Mirror out-pkt on port 2, Apply Rate Limit")

        meter = self.add_meter(self.device,
            mode=SWITCH_METER_ATTR_MODE_TWO_RATE_THREE_COLOR,
            red_packet_action=SWITCH_METER_ATTR_RED_PACKET_ACTION_DROP,
            cbs=10, cir=10,
            type=SWITCH_METER_ATTR_TYPE_PACKETS,
            color_source=SWITCH_METER_ATTR_COLOR_SOURCE_BLIND)

        mirror = self.add_mirror(self.device,
            type=SWITCH_MIRROR_ATTR_TYPE_LOCAL,
            direction=SWITCH_MIRROR_ATTR_DIRECTION_EGRESS,
            egress_port_handle=self.port2,
            meter_handle=meter)

        acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IP_MIRROR,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)

        acl_entry = self.add_acl_entry(self.device,
            dst_ip='10.10.10.1',
            dst_ip_mask='255.255.255.255',
            action_egress_mirror_handle=mirror,
            table_handle=acl_table)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, acl_table)

        pkt = simple_tcp_packet(pktlen=pkt_len,
                 eth_dst='00:77:66:55:44:33',
                 eth_src='00:22:22:22:22:22',
                 ip_dst='10.10.10.1',
                 ip_src='192.168.0.1',
                 ip_id=106,
                 ip_ttl=64)
        e_pkt = simple_tcp_packet(pktlen=pkt_len,
                eth_dst='00:11:22:33:44:57',
                eth_src='00:77:66:55:44:33',
                ip_dst='10.10.10.1',
                ip_src='192.168.0.1',
                ip_id=106,
                ip_ttl=63)

        try:
            total_packets = 10
            send_packet(self, self.devports[0], pkt, total_packets)
            print("  Waiting 10 secs")
            time.sleep(10)

            outgoing_packets = count_matched_packets(self, e_pkt, self.devports[1])
            mirror_packets = count_matched_packets(self, e_pkt, self.devports[2])

            print("  {}[Total Pkt] = {}[Outgoing Pkt] > {}[Mirror out-Pkt Rate-Lim]".format(total_packets, outgoing_packets, mirror_packets))
            self.assertTrue(total_packets == outgoing_packets)
            self.assertTrue(total_packets > mirror_packets)
            self.assertTrue(outgoing_packets > mirror_packets)

        finally:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            # remove the acl entry, table, mirror, meter
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

    def EgressAclMirrorInMeterTest(self):
        '''
        This test tries to verify rate limiting for Mirror in-pkt using Egress Acl
        '''
        if ((self.client.is_feature_enable(SWITCH_FEATURE_MIRROR_METER) == 0) or
            (self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_MIRROR_ACL) == 0) or
            (self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_MIRROR_ACL_MIRROR_IN_OUT) == 0)):
            print("Egress Ipv4/Ipv6 mirror ACL, mirror in or meter feature not enabled, skipping")
            return
        print("Egress Acl on port 1 ->  Ingress port 0, Mirror in-pkt on port 2, Apply Rate Limit")

        meter = self.add_meter(self.device,
            mode=SWITCH_METER_ATTR_MODE_TWO_RATE_THREE_COLOR,
            red_packet_action=SWITCH_METER_ATTR_RED_PACKET_ACTION_DROP,
            cbs=10, cir=10,
            type=SWITCH_METER_ATTR_TYPE_PACKETS,
            color_source=SWITCH_METER_ATTR_COLOR_SOURCE_BLIND)

        mirror = self.add_mirror(self.device,
            type=SWITCH_MIRROR_ATTR_TYPE_LOCAL,
            direction=SWITCH_MIRROR_ATTR_DIRECTION_EGRESS,
            egress_port_handle=self.port2,
            meter_handle=meter)

        acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IPV4_MIRROR,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_EGRESS)

        acl_entry = self.add_acl_entry(self.device,
            dst_ip='10.10.10.1',
            dst_ip_mask='255.255.255.255',
            action_ingress_mirror_handle=mirror,
            table_handle=acl_table)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, acl_table)

        pkt = simple_tcp_packet(pktlen=pkt_len,
                 eth_dst='00:77:66:55:44:33',
                 eth_src='00:22:22:22:22:22',
                 ip_dst='10.10.10.1',
                 ip_src='192.168.0.1',
                 ip_id=106,
                 ip_ttl=64)
        e_pkt = simple_tcp_packet(pktlen=pkt_len,
                eth_dst='00:11:22:33:44:57',
                eth_src='00:77:66:55:44:33',
                ip_dst='10.10.10.1',
                ip_src='192.168.0.1',
                ip_id=106,
                ip_ttl=63)
        try:
            total_packets = 10
            send_packet(self, self.devports[0], pkt, total_packets)
            print("  Waiting 10 secs")
            time.sleep(10)

            outgoing_packets = count_matched_packets(self, e_pkt, self.devports[1])
            mirror_packets = count_matched_packets(self, pkt, self.devports[2])

            print("  {}[Total Pkt] = {}[Outgoing Pkt] > {}[Mirror in-pkt Rate-Lim]".format(total_packets, outgoing_packets, mirror_packets))
            self.assertTrue(total_packets == outgoing_packets)
            self.assertTrue(total_packets > mirror_packets)
            self.assertTrue(outgoing_packets > mirror_packets)

        finally:
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, 0)
            # remove the acl entry, table, mirror, meter
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

    def EgressAclERSpanMirrorInTest(self):
        '''
        This test tries to verify ERSpan Mirror in-pkt using Egress Acl
        '''
        if ((self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_MIRROR_ACL_MIRROR_IN_OUT) == 0) or
            (self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_MIRROR_ACL) == 0)):
            print("Egress Ipv4/Ipv6 mirror ACL, mirror in feature not enabled, skipping")
            return
        print("Egress Acl on port 1 ->  Ingress port 0, Mirror in-pkt on port 2, Apply Rate Limit")

        mirror = self.add_mirror(self.device,
            type=SWITCH_MIRROR_ATTR_TYPE_ENHANCED_REMOTE,
            direction=SWITCH_MIRROR_ATTR_DIRECTION_EGRESS,
            rspan_type=SWITCH_MIRROR_ATTR_RSPAN_TYPE_NONE,
            erspan_type=SWITCH_MIRROR_ATTR_ERSPAN_TYPE_ERSPAN_3,
            ttl=64,
            platform_info=False,
            egress_port_handle=self.port2,
            src_mac_address='00:33:33:33:33:33',
            dest_mac_address='00:44:44:44:44:44',
            src_ip='10.0.0.1',
            dest_ip='20.0.0.1')
        session = self.attribute_get(mirror, SWITCH_MIRROR_ATTR_SESSION_ID)

        acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IPV4_MIRROR,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_EGRESS)

        acl_entry = self.add_acl_entry(self.device,
            dst_ip='10.10.10.1',
            dst_ip_mask='255.255.255.255',
            action_ingress_mirror_handle=mirror,
            table_handle=acl_table)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, acl_table)

        pkt = simple_tcp_packet(pktlen=pkt_len,
                 eth_dst='00:77:66:55:44:33',
                 eth_src='00:22:22:22:22:22',
                 ip_dst='10.10.10.1',
                 ip_src='192.168.0.1',
                 ip_id=106,
                 ip_ttl=64)
        e_pkt = simple_tcp_packet(pktlen=pkt_len,
                eth_dst='00:11:22:33:44:57',
                eth_src='00:77:66:55:44:33',
                ip_dst='10.10.10.1',
                ip_src='192.168.0.1',
                ip_id=106,
                ip_ttl=63)
        ERSPAN_3 = 2
        e_mirror_pkt = ptf.mask.Mask(ipv4_erspan_pkt(pktlen=pkt_len,
            eth_dst='00:44:44:44:44:44',
            eth_src='00:33:33:33:33:33',
            ip_src='10.0.0.1',
            ip_dst='20.0.0.1',
            ip_id=0,
            ip_ttl=64,
            ip_flags=0x0,
            version=ERSPAN_3,
            mirror_id=session,
            erspan_gra=2,
            inner_frame=pkt))
        mask_set_do_not_care_packet(e_mirror_pkt, IP, "chksum")
        mask_set_do_not_care_packet(e_mirror_pkt, ERSPAN_III, "timestamp")

        try:
            print("  Tx Ipv4 packet on port 0 -> port 1 [ERSpan mirror -> port 2]")
            send_packet(self, self.devports[0], pkt)
            print("  Waiting 10 secs")
            time.sleep(10)
            verify_each_packet_on_each_port(self, [e_pkt, e_mirror_pkt], [self.devports[1], self.devports[2]])
            verify_no_other_packets(self, timeout=2)

        finally:
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, 0)
            # remove the acl entry, table, mirror
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

    def EgressAclMirrorOutMeterTest(self):
        '''
        This test tries to verify rate limiting for Mirror out-pkt using Egress Acl
        '''
        if ((self.client.is_feature_enable(SWITCH_FEATURE_MIRROR_METER) == 0) or
            (self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_MIRROR_ACL) == 0)):
            print("Egress Ipv4/Ipv6 mirror ACL, mirror out or meter feature not enabled, skipping")
            return
        print("Egress Acl on port 1 ->  Ingress port 0, Mirror out-pkt on port 2, Apply Rate Limit")

        meter = self.add_meter(self.device,
            mode=SWITCH_METER_ATTR_MODE_TWO_RATE_THREE_COLOR,
            red_packet_action=SWITCH_METER_ATTR_RED_PACKET_ACTION_DROP,
            cbs=10, cir=10,
            type=SWITCH_METER_ATTR_TYPE_PACKETS,
            color_source=SWITCH_METER_ATTR_COLOR_SOURCE_BLIND)

        mirror = self.add_mirror(self.device,
            type=SWITCH_MIRROR_ATTR_TYPE_LOCAL,
            direction=SWITCH_MIRROR_ATTR_DIRECTION_EGRESS,
            egress_port_handle=self.port2,
            meter_handle=meter)

        acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IPV4_MIRROR,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_EGRESS)

        acl_entry = self.add_acl_entry(self.device,
            dst_ip='10.10.10.1',
            dst_ip_mask='255.255.255.255',
            action_egress_mirror_handle=mirror,
            table_handle=acl_table)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, acl_table)

        pkt = simple_tcp_packet(pktlen=pkt_len,
                 eth_dst='00:77:66:55:44:33',
                 eth_src='00:22:22:22:22:22',
                 ip_dst='10.10.10.1',
                 ip_src='192.168.0.1',
                 ip_id=106,
                 ip_ttl=64)
        e_pkt = simple_tcp_packet(pktlen=pkt_len,
                eth_dst='00:11:22:33:44:57',
                eth_src='00:77:66:55:44:33',
                ip_dst='10.10.10.1',
                ip_src='192.168.0.1',
                ip_id=106,
                ip_ttl=63)
        try:
            total_packets = 10
            send_packet(self, self.devports[0], pkt, total_packets)
            print("  Waiting 10 secs")
            time.sleep(10)

            outgoing_packets = count_matched_packets(self, e_pkt, self.devports[1])
            mirror_packets = count_matched_packets(self, e_pkt, self.devports[2])

            print("  {}[Total Pkt] = {}[Outgoing Pkt] > {}[Mirror out-pkt Rate-Lim]".format(total_packets, outgoing_packets, mirror_packets))
            self.assertTrue(total_packets == outgoing_packets)
            self.assertTrue(total_packets > mirror_packets)
            self.assertTrue(outgoing_packets > mirror_packets)

        finally:
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, 0)
            # remove the acl entry, table, mirror, meter
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

###############################################################################


@group('acl')
class AclRedirectTest(ApiHelper):
    def runTest(self):
        print()
        self.configure()
        self.ConfigureL2()
        self.ConfigureL3()
        try:
            self.IPv4AclRedirectNexthopTest()
            self.IPv6AclRedirectNexthopTest()
            self.MacAclRedirectL2PortTest()
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
        if (self.client.is_feature_enable(SWITCH_FEATURE_ACL_REDIRECT_NEXTHOP) == 0):
            print("AclRedirect nexthop feature not enabled, skipping")
            return
        table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IPV4
        if self.client.is_feature_enable(SWITCH_FEATURE_SHARED_INGRESS_IP_ACL):
            table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IP

        acl_table = self.add_acl_table(self.device,
                type=table_type,
                bind_point_type=[acl_table_bp_port],
                direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)

        acl_entry = self.add_acl_entry(self.device,
                dst_ip='10.10.10.1',
                dst_ip_mask='255.255.255.255',
                redirect=self.nhop2,
                table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry)

        # Attach acl_table to rif port8
        self.attribute_set(self.port8, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, acl_table)
        try:
            self.l3_exp_redirect_pkt1 = simple_tcp_packet(pktlen=pkt_len,
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

            #unset acl from bind_point and test for no-redirect
            print("unset re-direct acl from rif port 8 and check for no-redirect")
            self.attribute_set(self.port8, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            print("Sending IPv4 packet rif port 8 -> rif port 9(no re-direct to port10)")
            send_packet(self, self.devports[8], self.l3_pkt1)
            verify_packet(self, self.l3_exp_pkt1, self.devports[9])
            self.attribute_set(self.port8, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, acl_table)

            #acl-redirect to  next-hop, egress_port:lag
            self.l3_exp_redirect_pkt2 = simple_tcp_packet(pktlen=pkt_len,
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
            self.l3_exp_redirect_pkt3 = simple_tcp_packet(pktlen=pkt_len,
                               eth_dst='00:55:55:55:66:66',
                               eth_src='00:77:66:55:44:33',
                               ip_dst='10.10.10.1',
                               ip_src='192.168.0.1',
                               ip_id=105,
                               ip_ttl=63)

            self.attribute_set(acl_entry, SWITCH_ACL_ENTRY_ATTR_REDIRECT,
                     vlan20_nhop1)
            pre_counter = self.object_counters_get(acl_entry)
            print("Sending Ipv4 packet packet rif port 8 -> redirect svi_vlan20:port13")
            send_packet(self, self.devports[8], self.l3_pkt1)
            verify_packet(self, self.l3_exp_redirect_pkt3, self.devports[13])
            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)

            #acl-redirect to svi next-hop, egress_port:lag
            self.attribute_set(acl_entry, SWITCH_ACL_ENTRY_ATTR_REDIRECT,
                     self.nhop4)
            self.l3_exp_redirect_pkt4 = simple_tcp_packet(pktlen=pkt_len,
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
            self.attribute_set(self.port8, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
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
            # cleanup acl_entry, acl_table
            self.attribute_set(acl_entry, SWITCH_ACL_ENTRY_ATTR_ACTION_COUNTER_HANDLE, 0)
            self.cleanlast()
            self.cleanlast()
        return

    def IPv6AclRedirectNexthopTest(self):
        print("IPv6AclRedirectNexthopTest()")
        if (self.client.is_feature_enable(SWITCH_FEATURE_ACL_REDIRECT_NEXTHOP) == 0):
            print("AclRedirect nexthop feature not enabled, skipping")
            return
        table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IPV6
        if self.client.is_feature_enable(SWITCH_FEATURE_SHARED_INGRESS_IP_ACL):
            table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IP

        nhop1 = self.add_nexthop(self.device, handle=self.rif2, dest_ip='2222:3333:4444::1')
        neighbor1 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:56', handle=self.rif2, dest_ip='2222:3333:4444::1')

        # create acl_table, acl_entry
        acl_table = self.add_acl_table(self.device,
            type=table_type,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_entry = self.add_acl_entry(self.device,
            dst_ip='1234:5678:9abc:def0:4422:1133:5577:99aa',
            dst_ip_mask='FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF',
            redirect=nhop1,
            table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry)

        # Attach acl_table to rif port8
        self.attribute_set(self.port8, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, acl_table)
        pre_counter = self.object_counters_get(acl_entry)
        try:
            #case-1: acl-redirect to nhop with egress_port:port10
            l3_exp_redirect_pkt1_v6 = simple_tcpv6_packet(pktlen=pkt_len,
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


            #test acl-miss for unmatch packet
            route0 = self.add_route(self.device, ip_prefix='2234:5678:9abc:def0:4422:1133:5577:99aa', vrf_handle=self.vrf10,
                                    nexthop_handle=self.nhop1)
            print("Sending acl-miss IPv6 packet port 8 -> nhop rif:port9 (no redirect)")
            l3_pkt2_v6 = simple_tcpv6_packet(pktlen=pkt_len,
                eth_dst='00:77:66:55:44:33',
              eth_src='00:22:22:22:22:22',
              ipv6_dst='2234:5678:9abc:def0:4422:1133:5577:99aa',
              ipv6_src='2000::1',
              ipv6_hlim=64)

            l3_exp_pkt2_v6 = simple_tcpv6_packet(pktlen=pkt_len,
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
            self.attribute_set(self.port8, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            print("Sending IPv6 packet rif port 8 -> rif port 9(no re-direct to port10)")
            send_packet(self, self.devports[8], self.l3_pkt1_v6)
            verify_packet(self, self.l3_exp_pkt1_v6, self.devports[9])
            self.attribute_set(self.port8, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, acl_table)

            #case-2: acl-redirect to  next-hop, egress_port:lag
            l3_exp_redirect_pkt3 = simple_tcpv6_packet(pktlen=pkt_len,
                eth_dst='00:11:22:33:44:57',
               eth_src='00:77:66:55:44:33',
               ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
               ipv6_src='2000::1',
               ipv6_hlim=63)
            self.attribute_set(acl_entry, SWITCH_ACL_ENTRY_ATTR_REDIRECT,
                     self.nhop3)
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
            l3_exp_redirect_pkt4 = simple_tcpv6_packet(pktlen=pkt_len,
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
            l3_exp_redirect_pkt5 = simple_tcpv6_packet(pktlen=pkt_len,
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
            self.attribute_set(self.port8, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
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
            # cleanup acl_entry, acl_table
            self.attribute_set(acl_entry, SWITCH_ACL_ENTRY_ATTR_ACTION_COUNTER_HANDLE, 0)
            self.cleanlast()
            self.cleanlast()
            # cleanup neighbor1, nhop1
            self.cleanlast()
            self.cleanlast()
        return

    def MacAclRedirectL2PortTest(self):
        print("MacAclRedirectL2PortTest()")
        if (self.client.is_feature_enable(SWITCH_FEATURE_INGRESS_MAC_ACL) == 0):
            print("MacACL feature not enabled, skipping")
            return
        if (self.client.is_feature_enable(SWITCH_FEATURE_ACL_REDIRECT_PORT) == 0):
            print("AclRedirect port feature not enabled, skipping")
            return

        try:
            #case-1: before apply acl-redirect, test for regular forwarding.
            vlan10_l2_pkt1 = simple_eth_packet(
                eth_dst='00:44:44:44:44:44',
               eth_src='00:22:22:22:22:22')
            print("Sending vlan 10 packet port 0 -> port 2")
            send_packet(self, self.devports[0], vlan10_l2_pkt1)
            verify_packets(self, vlan10_l2_pkt1, [self.devports[2]])

            acl_table = self.add_acl_table(self.device,
              type=SWITCH_ACL_TABLE_ATTR_TYPE_MAC,
              bind_point_type=[acl_table_bp_port],
              direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
            acl_entry = self.add_acl_entry(self.device,
              src_mac='00:22:22:22:22:22',
              src_mac_mask='ff:ff:ff:ff:ff:ff',
              redirect=self.port1,
              table_handle=acl_table)
            self.client.object_counters_clear_all(acl_entry)

            # Attach acl_table to vlan10, port0
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, acl_table)

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
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            print("Sending vlan 10 packet port 1 -> port 2 (no redirect)")
            send_packet(self, self.devports[0], vlan10_l2_pkt1)
            verify_packet(self, vlan10_l2_pkt1, self.devports[2])

        finally:
            # cleanup acl_entry, acl_table
            self.cleanlast()
            self.cleanlast()
        return


    def IPv4AclRedirectL2PortTest(self):
        print("IPv4AclRedirectPortTest()")
        if (self.client.is_feature_enable(SWITCH_FEATURE_ACL_REDIRECT_PORT) == 0):
            print("AclRedirect port feature not enabled, skipping")
            return
        table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IPV4
        if self.client.is_feature_enable(SWITCH_FEATURE_SHARED_INGRESS_IP_ACL):
            table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IP

        acl_table = self.add_acl_table(self.device,
                type=table_type,
                bind_point_type=[acl_table_bp_port],
                direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_entry = self.add_acl_entry(self.device,
                dst_ip='10.10.10.1',
                dst_ip_mask='255.255.255.255',
                redirect=self.port1,
                table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry)

        # Attach acl_table to vlan10, port0
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, acl_table)
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
            vlan10_pkt2 = simple_tcp_packet(pktlen=pkt_len,
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
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            print("Sending vlan 10 packet port 1 -> port 2 (no redirect)")
            send_packet(self, self.devports[0], self.vlan10_pkt1)
            verify_packet(self, self.vlan10_pkt1, self.devports[2])

        finally:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            # cleanup acl_entry, acl_table
            self.cleanlast()
            self.cleanlast()
        return

    def IPv4AclRedirectL3PortTest(self):
        print("IPv4AclRedirectL3PortTest()")
        if (self.client.is_feature_enable(SWITCH_FEATURE_ACL_REDIRECT_PORT) == 0):
            print("AclRedirect port feature not enabled, skipping")
            return
        table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IPV4
        if self.client.is_feature_enable(SWITCH_FEATURE_SHARED_INGRESS_IP_ACL):
            table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IP

        acl_table = self.add_acl_table(self.device,
                type=table_type,
                bind_point_type=[acl_table_bp_port],
                direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_entry = self.add_acl_entry(self.device,
                dst_ip='10.10.10.1',
                dst_ip_mask='255.255.255.255',
                redirect=self.port10,
                table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry)

        # Attach acl_table to rif port8
        self.attribute_set(self.port8, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, acl_table)
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
            l3_pkt2 = simple_tcp_packet(pktlen=pkt_len,
                eth_dst='00:77:66:55:44:33',
               eth_src='00:22:22:22:22:22',
               ip_dst='60.10.10.1',
               ip_src='192.168.0.1',
               ip_id=105,
               ip_ttl=64)

            l3_exp_pkt2 = simple_tcp_packet(pktlen=pkt_len,
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
            self.attribute_set(self.port8, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            # cleanup acl_entry, acl_table
            self.cleanlast()
            self.cleanlast()
        return

    def IPv6AclRedirectL2PortTest(self):
        print("IPv6AclRedirectL2PortTest()")
        if (self.client.is_feature_enable(SWITCH_FEATURE_ACL_REDIRECT_PORT) == 0):
            print("AclRedirect port feature not enabled, skipping")
            return
        table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IPV6
        if self.client.is_feature_enable(SWITCH_FEATURE_SHARED_INGRESS_IP_ACL):
            table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IP

        try:
            v6_l3_pkt1 = simple_tcpv6_packet(pktlen=pkt_len,
                eth_dst='00:44:44:44:44:44',
              eth_src='00:44:44:44:44:42',
              ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
              ipv6_src='2000::1',
              ipv6_hlim=64)

            #case-1: before applying acl-redirect, check for basic forwarding.
            print("Sending vlan 10 packet port 0 -> port 2")
            send_packet(self, self.devports[0], v6_l3_pkt1)
            verify_packet(self, v6_l3_pkt1, self.devports[2])

            acl_table = self.add_acl_table(self.device,
                type=table_type,
                bind_point_type=[acl_table_bp_port],
                direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)

            acl_entry = self.add_acl_entry(self.device,
                dst_ip='1234:5678:9abc:def0:4422:1133:5577:99aa',
                dst_ip_mask='FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF',
                redirect=self.port1,
                table_handle=acl_table)
            self.client.object_counters_clear_all(acl_entry)

            # Attach acl_table to vlan10, port0
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, acl_table)

            #case-1: check for redirect to port1
            print("Sending vlan 10 packet port 0 -> port 2 (action: redirect port 1)")
            pre_counter = self.object_counters_get(acl_entry)
            send_packet(self, self.devports[0], v6_l3_pkt1)
            verify_packet(self, v6_l3_pkt1, self.devports[1])
            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)


            #case-2: send acl-miss packet and test for no drop
            print("send acl_miss packet and test for no redirect")
            v6_l3_pkt2 = simple_tcpv6_packet(pktlen=pkt_len,
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
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            print("Sending vlan 10 packet port 1 -> port 2 (no redirect)")
            send_packet(self, self.devports[0], v6_l3_pkt1)
            verify_packet(self, v6_l3_pkt1, self.devports[2])

        finally:
            # cleanup acl_entry, acl_table
            self.cleanlast()
            self.cleanlast()
        return

    def IPv6AclRedirectL3PortTest(self):
        print("IPv6AclRedirectL3PortTest()")
        if (self.client.is_feature_enable(SWITCH_FEATURE_ACL_REDIRECT_PORT) == 0):
            print("AclRedirect nexthop feature not enabled, skipping")
            return
        table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IPV6
        if self.client.is_feature_enable(SWITCH_FEATURE_SHARED_INGRESS_IP_ACL):
            table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IP

        nhop1 = self.add_nexthop(self.device, handle=self.rif2, dest_ip='2222:3333:4444::1')
        neighbor1 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:56', handle=self.rif2, dest_ip='2222:3333:4444::1')

        # create acl_table, acl_entry
        acl_table = self.add_acl_table(self.device,
            type=table_type,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_entry = self.add_acl_entry(self.device,
            dst_ip='1234:5678:9abc:def0:4422:1133:5577:99aa',
            dst_ip_mask='FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF',
            redirect=self.port10,
            table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry)

        # Attach acl_table to rif port8
        self.attribute_set(self.port8, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, acl_table)
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
            l3_pkt2_v6 = simple_tcpv6_packet(pktlen=pkt_len,
                eth_dst='00:77:66:55:44:33',
              eth_src='00:22:22:22:22:22',
              ipv6_dst='2234:5678:9abc:def0:4422:1133:5577:99aa',
              ipv6_src='2000::1',
              ipv6_hlim=64)

            l3_exp_pkt2_v6 = simple_tcpv6_packet(pktlen=pkt_len,
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
            self.attribute_set(self.port8, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            print("Sending IPv6 packet rif port 8 -> rif port 9(no re-direct to port10)")
            send_packet(self, self.devports[8], self.l3_pkt1_v6)
            verify_packet(self, self.l3_exp_pkt1_v6, self.devports[9])
            self.attribute_set(self.port8, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, acl_table)

            #case-2: acl-redirect to  next-hop, egress_port:lag
            l3_exp_redirect_pkt3 = simple_tcpv6_packet(pktlen=pkt_len,
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
            self.attribute_set(self.port8, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            print("Sending IPv6 packet rif port 8 -> rif port 9(no re-direct to port10)")
            send_packet(self, self.devports[8], self.l3_pkt1_v6)
            verify_packet(self, self.l3_exp_pkt1_v6, self.devports[9])

        finally:
            # cleanup acl_entry, acl_table
            self.attribute_set(acl_entry, SWITCH_ACL_ENTRY_ATTR_ACTION_COUNTER_HANDLE, 0)
            self.cleanlast()
            self.cleanlast()
            # cleanup neighbor1, nhop1
            self.cleanlast()
            self.cleanlast()
        return

@group('acl')
class AclRedirectEcmpNextHopTest(ApiHelper):
    def runTest(self):
        print("AclRedirectEcmpNextHopTest()")
        if (self.client.is_feature_enable(SWITCH_FEATURE_ACL_REDIRECT_NEXTHOP) == 0):
            print("AclRedirect nexthop feature not enabled, skipping")
            return
        table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IPV4
        if self.client.is_feature_enable(SWITCH_FEATURE_SHARED_INGRESS_IP_ACL):
            table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IP

        self.configure()

        # ingress port
        rif0 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port0, vrf_handle=self.vrf10, src_mac=self.rmac)

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


        # non-ecmp nhop3
        rif3 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port3, vrf_handle=self.vrf10, src_mac=self.rmac)
        nhop3 = self.add_nexthop(self.device, handle=rif3, dest_ip='30.30.0.2')
        neighbor3 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:57', handle=rif3, dest_ip='30.30.0.2')

        ecmp0 = self.add_ecmp(self.device)
        ecmp_member01 = self.add_ecmp_member(self.device, nexthop_handle=nhop1, ecmp_handle=ecmp0)
        ecmp_member02 = self.add_ecmp_member(self.device, nexthop_handle=nhop2, ecmp_handle=ecmp0)
        route1 = self.add_route(self.device, ip_prefix='10.10.0.0/16', vrf_handle=self.vrf10, nexthop_handle=nhop3)

        # before apply acl redirect, test for packet
        try:
            pkt1 = simple_tcp_packet(pktlen=pkt_len,
                 eth_dst='00:77:66:55:44:33',
                 eth_src='00:22:22:22:22:22',
                 ip_dst='10.10.10.1',
                 ip_src='192.168.0.1',
                 ip_id=106,
                 ip_ttl=64)
            exp_pkt1 = simple_tcp_packet(pktlen=pkt_len,
                 eth_dst='00:11:22:33:44:57',
                 eth_src='00:77:66:55:44:33',
                 ip_dst='10.10.10.1',
                 ip_src='192.168.0.1',
                 ip_id=106,
                 ip_ttl=63)
            print("Sending packet from port %d" % self.devports[0], " (192.168.0.1 -> 10.10.10.1) Host")
            send_packet(self, self.devports[0], pkt1)
            verify_packet(self, exp_pkt1, self.devports[3])
        finally:
            pass

        # install acl-redirect ecmp nexthop
        acl_table = self.add_acl_table(self.device,
                type=table_type,
                bind_point_type=[acl_table_bp_port],
                direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)

        acl_entry = self.add_acl_entry(self.device,
                dst_ip='10.10.0.0',
                dst_ip_mask='255.255.0.0',
                redirect=ecmp0,
                table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry)

        # Attach acl_table to rif port0
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, acl_table)

        # add route for acl-miss test
        ip_prefix2 = '40.40.40.1'
        acl_miss_route2 = self.add_route(self.device,
                                ip_prefix=ip_prefix2,
                                vrf_handle=self.vrf10,
                                nexthop_handle=nhop3)

        try:
            count = [0, 0]
            dst_ip = int(binascii.hexlify(socket.inet_aton('10.10.10.1')), 16)
            src_ip = int(binascii.hexlify(socket.inet_aton('192.168.0.1')), 16)
            max_itrs = 200
            pre_counter = self.object_counters_get(acl_entry)
            pkt2 = simple_tcp_packet(pktlen=pkt_len,
                    eth_dst='00:77:66:55:44:33',
                    eth_src='00:22:22:22:22:22',
                    ip_dst='10.10.10.1',
                    ip_src='192.168.0.1',
                    ip_ttl=64)
            exp_pkt2 = simple_tcp_packet(pktlen=pkt_len,
                    eth_dst='00:11:22:33:44:55',
                    eth_src='00:77:66:55:44:33',
                    ip_dst='10.10.10.1',
                    ip_src='192.168.0.1',
                    ip_ttl=63)
            exp_pkt3 = simple_tcp_packet(pktlen=pkt_len,
                    eth_dst='00:11:22:33:44:56',
                    eth_src='00:77:66:55:44:33',
                    ip_dst='10.10.10.1',
                    ip_src='192.168.0.1',
                    ip_ttl=63)
            print("Sending packet from port %d, redirect to ecmp_nexthop:port0/port1" % self.devports[0], " (192.168.0.1 -> 10.10.10.1) Host")
            for i in range(0, max_itrs):
                dst_ip_addr = socket.inet_ntoa(
                    binascii.unhexlify(format(dst_ip, 'x').zfill(8)))
                src_ip_addr = socket.inet_ntoa(
                    binascii.unhexlify(format(src_ip, 'x').zfill(8)))
                pkt2['IP'].dst = dst_ip_addr
                pkt2['IP'].src = src_ip_addr
                exp_pkt2['IP'].dst = dst_ip_addr
                exp_pkt2['IP'].src = src_ip_addr
                exp_pkt3['IP'].dst = dst_ip_addr
                exp_pkt3['IP'].src = src_ip_addr
                send_packet(self, self.devports[0], pkt2)
                rcv_idx = verify_any_packet_any_port(self, [exp_pkt2, exp_pkt3],
                                       [self.devports[1], self.devports[2]], timeout=2)

                count[rcv_idx] += 1
                dst_ip += 1
                src_ip += 1
            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, max_itrs)
            print('ecmp-count:', count)
            for i in range(0, 2):
                self.assertTrue((count[i] >= ((max_itrs / 2) * 0.8)),
                                "Ecmp paths are not equally balanced")

            #case-2: send acl-mismatch packet and test for no redirect
            pkt2 = simple_tcp_packet(pktlen=pkt_len,
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='40.40.40.1',
                ip_src='192.168.0.1',
                ip_id=106,
                ip_ttl=64)
            exp_pkt4 = simple_tcp_packet(pktlen=pkt_len,
                eth_dst='00:11:22:33:44:57',
                eth_src='00:77:66:55:44:33',
                ip_dst='40.40.40.1',
                ip_src='192.168.0.1',
                ip_id=106,
                ip_ttl=63)
            print("Sending packet from port %d" % self.devports[0], " (192.168.0.1 -> 10.10.10.1) Host")
            send_packet(self, self.devports[0], pkt2)
            verify_packet(self, exp_pkt4, self.devports[3])

            #case-3: unset acl from bind_point and test for no-redirect
            print("unset re-direct acl from rif port 0 and check for no-redirect")
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            print("Sending IPv4 packet rif port 0 -> rif port 3(no re-direct to port1/port2)")
            send_packet(self, self.devports[0], pkt1)
            verify_packet(self, exp_pkt1, self.devports[3])
        finally:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            self.cleanup()
        return


@group('acl')
class AclRedirectTunnelNextHopTest(ApiHelper):
    def runTest(self):
        print("AclRedirectTunnelNextHopTest()")
        if (self.client.is_feature_enable(SWITCH_FEATURE_ACL_REDIRECT_NEXTHOP) == 0):
            print("AclRedirect feature not enabled, skipping")
            return
        if (self.client.is_feature_enable(SWITCH_FEATURE_IPV4_TUNNEL) == 0):
            print("IPv4 tunnel feature not enabled, skipping")
            return
        if (self.client.is_feature_enable(SWITCH_FEATURE_VXLAN) == 0):
            print("VXLAN feature not enabled, skipping")
            return
        table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IPV4
        if self.client.is_feature_enable(SWITCH_FEATURE_SHARED_INGRESS_IP_ACL):
            table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IP

        self.configure()
        # underlay config input
        self.uvrf = self.add_vrf(self.device, id=100, src_mac=self.rmac)
        self.vm_ip = '100.100.1.1'
        self.vm_ip2 = '100.100.2.1'
        self.customer_ip = '100.100.3.1'
        self.vni = 2000
        self.my_lb_ip = '10.10.10.10'
        self.tunnel_ip = '10.10.10.1'
        self.tunnel_ip2 = '10.10.10.3'
        self.inner_dmac = "00:33:33:33:33:33"
        self.inner_dmac2 = "00:33:33:33:33:44"
        self.underlay_neighbor_mac = '00:11:11:11:11:11'
        self.underlay_neighbor_mac2 = '00:11:11:11:11:22'

        self.default_rmac = "00:BA:7E:F0:00:00"  # from bf_switch_device_add



        # create underlay loopback rif for tunnel
        self.urif_lb = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_LOOPBACK, vrf_handle=self.uvrf, src_mac=self.rmac)

        # overlay configuration
        self.ovrf = self.add_vrf(self.device, id = 200, src_mac=self.rmac)
        self.tunnel_type = SWITCH_TUNNEL_ATTR_TYPE_VXLAN
        self.term_type = SWITCH_TUNNEL_TERM_ATTR_TERMINATION_TYPE_P2MP
        self.encap_ttl_mode = SWITCH_TUNNEL_ATTR_ENCAP_TTL_MODE_PIPE_MODEL
        self.decap_ttl_mode = SWITCH_TUNNEL_ATTR_DECAP_TTL_MODE_PIPE_MODEL
        self.orif1 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port0,
                     vrf_handle=self.ovrf, src_mac=self.rmac)

        # create overlay nexthop
        self.overlay_nhop_ip = '10.10.10.2'
        self.overlay_neighbor_mac3 = '00:11:11:11:11:33'
        self.orif2 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port4,
                         vrf_handle=self.ovrf, src_mac=self.rmac)
        self.overlay_neighbor = self.add_neighbor(self.device,
            mac_address=self.overlay_neighbor_mac3,
            handle=self.orif2,
            dest_ip=self.overlay_nhop_ip)  # 10.10.10.2
        self.overlay_nhop = self.add_nexthop(self.device, handle=self.orif2,
                            dest_ip=self.overlay_nhop_ip)
        # add route to overlay_nexthop
        self.customer_route = self.add_route(self.device,
                                ip_prefix=self.vm_ip,
                                vrf_handle=self.ovrf,
                                nexthop_handle=self.overlay_nhop)
        try:
            print("Sending packet from Access port 0 to port 4")
            pkt1 = simple_tcp_packet(pktlen=pkt_len,
                eth_dst=self.rmac,
                eth_src='00:22:22:22:22:22',
                ip_dst=self.vm_ip,
                ip_src=self.customer_ip,
                ip_id=108,
                ip_ttl=64)
            exp_pkt1 = simple_tcp_packet(pktlen=pkt_len,
                eth_dst=self.overlay_neighbor_mac3,
                eth_src=self.rmac,
                ip_dst=self.vm_ip,
                ip_src=self.customer_ip,
                ip_id=108,
                ip_ttl=63)
            send_packet(self,self.devports[0],pkt1)
            verify_packets(self, exp_pkt1, [self.devports[4]])
        finally:
            pass


        # create vxlan tunnel nexthop.

        # Encap configuration follows

        # create Encap/Decap mappers
        self.encap_tunnel_map = self.add_tunnel_mapper(self.device,
            type=SWITCH_TUNNEL_MAPPER_ATTR_TYPE_VRF_HANDLE_TO_VNI)
        self.decap_tunnel_map = self.add_tunnel_mapper(self.device,
            type=SWITCH_TUNNEL_MAPPER_ATTR_TYPE_VNI_TO_VRF_HANDLE)

        # create Encap/Decap mapper entries for ovrf
        self.encap_tunnel_map_entry = self.add_tunnel_mapper_entry(self.device,
            type=SWITCH_TUNNEL_MAPPER_ENTRY_ATTR_TYPE_VRF_HANDLE_TO_VNI,
            tunnel_mapper_handle=self.encap_tunnel_map,
            network_handle=self.ovrf,
            tunnel_vni=self.vni)
        self.decap_tunnel_map_entry = self.add_tunnel_mapper_entry(self.device,
            type=SWITCH_TUNNEL_MAPPER_ENTRY_ATTR_TYPE_VNI_TO_VRF_HANDLE,
            tunnel_mapper_handle=self.decap_tunnel_map,
            network_handle=self.ovrf,
            tunnel_vni=self.vni)

        ingress_mapper_list = [switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=self.encap_tunnel_map)]
        egress_mapper_list = [switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=self.decap_tunnel_map)]

        # create Tunnel
        self.tunnel = self.add_tunnel(self.device,
            type=self.tunnel_type,
            src_ip=self.my_lb_ip,
            ingress_mapper_handles=ingress_mapper_list,
            egress_mapper_handles=egress_mapper_list,
            encap_ttl_mode=self.encap_ttl_mode,
            decap_ttl_mode=self.decap_ttl_mode,
            underlay_rif_handle=self.urif_lb)

        # Create route to tunnel ip
        self.urif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port1, vrf_handle=self.uvrf, src_mac=self.rmac)
        self.uneighbor = self.add_neighbor(self.device,
            mac_address=self.underlay_neighbor_mac,
            handle=self.urif,
            dest_ip=self.tunnel_ip)  # 10.10.10.1
        self.unhop = self.add_nexthop(self.device, handle=self.urif, dest_ip='10.10.10.1')

        # add route to tunnel ip
        self.tunnel_route = self.add_route(self.device, ip_prefix="10.10.10.1",
            vrf_handle=self.uvrf, nexthop_handle=self.unhop)

        # create tunnel nexthop for VM
        self.tunnel_nexthop = self.add_nexthop(self.device,
            type=SWITCH_NEXTHOP_ATTR_TYPE_TUNNEL,
            rw_type=SWITCH_NEXTHOP_ATTR_RW_TYPE_L3_VNI,
            handle=self.tunnel,
            dest_ip=self.tunnel_ip,  # 10.10.10.1
            mac_address=self.inner_dmac,
            tunnel_vni=2000)

        # install acl-redirect nexthop
        acl_table = self.add_acl_table(self.device,
                type=table_type,
                bind_point_type=[acl_table_bp_port],
                direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)

        acl_entry = self.add_acl_entry(self.device,
                dst_ip=self.vm_ip,
                dst_ip_mask='255.255.255.255',
                redirect=self.tunnel_nexthop,
                table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry)

        # Attach acl_table to rif port0
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, acl_table)

        # add route for acl-miss test
        self.vm_ip10 = '103.100.1.1'
        self.customer_route2 = self.add_route(self.device,
                                ip_prefix=self.vm_ip10,
                                vrf_handle=self.ovrf,
                                nexthop_handle=self.overlay_nhop)
        try:
            print("Sending packet from Access port 0 to VxLan port 1")
            inner_pkt = simple_tcp_packet(pktlen=pkt_len,
                eth_dst=self.inner_dmac,  # 00:33:33:33:33:33
                eth_src=self.default_rmac,
                ip_dst=self.vm_ip,
                ip_src=self.customer_ip,
                ip_id=108,
                ip_ttl=63)
            vxlan_pkt = mask.Mask(simple_vxlan_packet(
                pktlen=pkt_len,
                eth_src='00:77:66:55:44:33',
                eth_dst=self.underlay_neighbor_mac,  # 00:11:11:11:11:11
                ip_id=0,
                ip_src=self.my_lb_ip,
                ip_dst=self.tunnel_ip,
                ip_ttl=64,
                ip_flags=0x2,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=inner_pkt))
            mask_set_do_not_care_packet(vxlan_pkt, UDP, "sport")
            pre_counter = self.object_counters_get(acl_entry)
            print("Sending Ipv4 packet rif port 0 -> redirect tunnel nhop rif:port1")
            send_packet(self, self.devports[0], pkt1)
            verify_packets(self, vxlan_pkt, [self.devports[1]])
            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)

            # send acl-mismatch packet and test for no redirect
            print("Sending packet from Access port 0 to port 4")
            pkt21 = simple_tcp_packet(pktlen=pkt_len,
                eth_dst=self.rmac,
                eth_src='00:22:22:22:22:22',
                ip_dst=self.vm_ip10,
                ip_src=self.customer_ip,
                ip_id=108,
                ip_ttl=64)
            exp_pkt22 = simple_tcp_packet(pktlen=pkt_len,
                eth_dst=self.overlay_neighbor_mac3,
                eth_src=self.rmac,
                ip_dst=self.vm_ip10,
                ip_src=self.customer_ip,
                ip_id=108,
                ip_ttl=63)
            send_packet(self, self.devports[0], pkt21)
            verify_packets(self, exp_pkt22, [self.devports[4]])

            #unset acl from bind_point and test for no-redirect
            print("unset re-direct acl from rif port 0 and check for no-redirect")
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            print("Sending IPv4 packet rif port 0 -> rif port 4(no re-direct to vxlan port1)")
            send_packet(self, self.devports[0], pkt1)
            verify_packets(self, exp_pkt1, [self.devports[4]])
        finally:
            self.cleanup()
        return

###############################################################################

@group('acl')
class IngressAclBindPointTest(ApiHelper):
    def runTest(self):
        if(self.client.is_feature_enable(SWITCH_FEATURE_ACL_PORT_GROUP) == 1):
            print("ACL bindpoint test not supported in the profile")
            return
        self.table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IPV4
        if self.client.is_feature_enable(SWITCH_FEATURE_SHARED_INGRESS_IP_ACL):
            self.table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IP

        print()
        self.configure()
        self.ConfigureL2()
        self.ConfigureL3()
        try:
            self.BindPointSwitchTest()
            self.BindPointL2PortTest()
            self.BindPointL3PortTest()
            self.BindPointL2LagTest()
            self.BindPointL3LagTest()
            self.BindPointVlanTest()
            self.BindPointL3SVIRifTest()
            self.BindPointL3PortRifTest()
        finally:
            self.CleanupL2()
            self.cleanup()
        return

    def BindPointSwitchTest(self):
        print("BindPointSwitchTest()")
        acl_table = self.add_acl_table(self.device,
                type=self.table_type,
                bind_point_type=[acl_table_bp_switch],
                direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_entry = self.add_acl_entry(self.device,
                dst_ip='10.10.10.1',
                dst_ip_mask='255.255.255.255',
                packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
                table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry)
        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_INGRESS_ACL, acl_table)
        try:
            pre_counter = self.object_counters_get(acl_entry)
            print("Sending vlan 10 packet port 0 (drop)")
            send_packet(self, self.devports[0], self.vlan10_pkt1)
            verify_no_other_packets(self, timeout=2)
            print("Sending vlan 10 packet port 1 (drop)")
            send_packet(self, self.devports[1], self.vlan10_pkt1)
            verify_no_other_packets(self, timeout=2)
            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 2)

            # send acl-miss packet and test for no drop
            print("send acl_miss packet and test for no drop")
            vlan10_pkt2 = simple_tcp_packet(pktlen=pkt_len,
                eth_dst='00:44:44:44:44:44',
              eth_src='00:44:44:44:44:42',
              ip_dst='50.10.10.2',
              ip_src='192.168.0.1',
              ip_id=105,
              ip_ttl=64)

            print("Sending vlan 10 packet port 0 -> port 2 (no drop)")
            send_packet(self, self.devports[0], vlan10_pkt2)
            verify_packets(self, vlan10_pkt2, [self.devports[2]])
        finally:
            self.attribute_set(self.device, SWITCH_DEVICE_ATTR_INGRESS_ACL, 0)
            self.cleanlast()
            self.cleanlast()
        return

    def BindPointL2PortTest(self):
        print("BindPointL2PortTest()")
        acl_table = self.add_acl_table(self.device,
                type=self.table_type,
                bind_point_type=[acl_table_bp_port],
                direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_entry = self.add_acl_entry(self.device,
                dst_ip='10.10.10.1',
                dst_ip_mask='255.255.255.255',
                packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
                table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry)

        # Attach acl_table to vlan10, port0
        self.acl_group1 = self.add_acl_group(self.device, bind_point_type=[acl_group_bp_port])
        self.acl_group_member1 = self.add_acl_group_member(self.device,
            acl_table_handle=acl_table,
            acl_group_handle=self.acl_group1)
        table_name = SWITCH_DEVICE_ATTR_TABLE_IPV4_ACL
        pre_hw_table_count = get_hw_table_usage(self, table_name)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, self.acl_group1)
        post_hw_table_count = get_hw_table_usage(self, table_name)
        # Only one h/w entry should be used
        self.assertEqual(post_hw_table_count - pre_hw_table_count, 1)
        self.attribute_set(self.port2, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, self.acl_group1)
        self.attribute_set(self.port3, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, self.acl_group1)
        self.attribute_set(self.port4, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, self.acl_group1)
        self.attribute_set(self.port5, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, self.acl_group1)
        self.attribute_set(self.port6, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, self.acl_group1)
        post_hw_table_count = get_hw_table_usage(self, table_name)
        # Only one h/w entry should be used for all 6 bind points of the same
        # ACL table
        self.assertEqual(post_hw_table_count - pre_hw_table_count, 1)
        try:
            pre_counter = self.object_counters_get(acl_entry)
            send_packet(self, self.devports[0], self.vlan10_pkt1)
            verify_no_other_packets(self, timeout=2)
            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)

            # send acl-miss packet and test for no drop
            print("send acl_miss packet and test for no drop")
            vlan10_pkt2 = simple_tcp_packet(pktlen=pkt_len,
                eth_dst='00:44:44:44:44:44',
              eth_src='00:44:44:44:44:42',
              ip_dst='50.10.10.2',
              ip_src='192.168.0.1',
              ip_id=105,
              ip_ttl=64)

            print("Sending vlan 10 packet port 0 -> port 2 (no drop)")
            send_packet(self, self.devports[0], vlan10_pkt2)
            verify_packets(self, vlan10_pkt2, [self.devports[2]])


            #send packet from vlan10, port1 and verify packet not dropped
            print("Sending vlan 10 packet port 1 -> port 2 (no drop)")
            send_packet(self, self.devports[1], self.vlan10_pkt1)
            verify_packets(self, self.vlan10_pkt1, [self.devports[2]])
        finally:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            post_hw_table_count = get_hw_table_usage(self, table_name)
            self.assertEqual(post_hw_table_count - pre_hw_table_count, 1)
            self.attribute_set(self.port2, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            self.attribute_set(self.port3, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            self.attribute_set(self.port4, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            post_hw_table_count = get_hw_table_usage(self, table_name)
            self.assertEqual(post_hw_table_count - pre_hw_table_count, 1)
            self.attribute_set(self.port5, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            self.attribute_set(self.port6, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            # All h/w acl entries should be removed after detaching
            # ACL from all ports
            post_hw_table_count = get_hw_table_usage(self, table_name)
            self.assertEqual(post_hw_table_count - pre_hw_table_count, 0)
            # cleanup counter, acl_entry, acl_table

            self.cleanlast()
            self.cleanlast()
        return

    def BindPointL2LagTest(self):
        print("BindPointL2LagTest()")
        acl_table = self.add_acl_table(self.device,
                type=self.table_type,
                bind_point_type=[acl_table_bp_lag],
                direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_entry = self.add_acl_entry(self.device,
                dst_ip='10.10.10.1',
                dst_ip_mask='255.255.255.255',
                packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
                table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry)

        lag1 = self.add_lag(self.device)
        vlan_member1 = self.add_vlan_member(self.device, vlan_handle=self.vlan10,
                       member_handle=lag1)
        self.attribute_set(lag1, SWITCH_LAG_ATTR_PORT_VLAN_ID, 10)
        self.attribute_set(lag1, SWITCH_LAG_ATTR_INGRESS_ACL_HANDLE, acl_table)
        try:
            print("check for no drop before adding lag member")
            print("Sending vlan 10 packet port 1 -> port 2 (no drop)")
            send_packet(self, self.devports[1], self.vlan10_pkt1)
            verify_packets(self, self.vlan10_pkt1, [self.devports[2]])

            # add lag member
            print("add lag member and check for drop")
            lag_mbr1 = self.add_lag_member(self.device, lag_handle=lag1, port_handle=self.port13)
            pre_counter = self.object_counters_get(acl_entry)
            print("Sending vlan 10 packet port 13 -> port 2 (drop)")
            send_packet(self, self.devports[13], self.vlan10_pkt1)
            verify_no_other_packets(self, timeout=2)
            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)

            # send acl-miss packet and test for no drop
            print("Send acl_miss packet and test for no drop")
            vlan10_lag_pkt1= simple_tcp_packet(pktlen=pkt_len,
                    eth_dst='00:44:44:44:44:44',
                    eth_src='00:44:44:44:44:42',
                    ip_dst='50.10.10.1',
                    ip_src='192.168.0.1',
                    ip_id=105,
                    ip_ttl=64)

            print("Sending vlan 10 packet port 13 -> port 2 (no drop)")
            send_packet(self, self.devports[13], vlan10_lag_pkt1)
            verify_packets(self, vlan10_lag_pkt1, [self.devports[2]])

            # remove lag member and add back as normal l2 port.
            print("remove lag_member, add back port as normal l2 port and check for no drop")
            self.cleanlast()
            vlan_member2 = self.add_vlan_member(self.device, vlan_handle=self.vlan10,
                              member_handle=self.port13)
            self.attribute_set(self.port13, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
            print("Sending vlan 10 packet port 13 -> port 2 (no drop)")
            send_packet(self, self.devports[13], self.vlan10_pkt1)
            verify_packets(self, self.vlan10_pkt1, [self.devports[2]])
        finally:
            #cleanup vlan_member2,vlan_member1
            self.attribute_set(lag1, SWITCH_LAG_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.port13, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.cleanlast()
            self.cleanlast()
            self.attribute_set(lag1, SWITCH_LAG_ATTR_INGRESS_ACL_HANDLE, 0)
            #cleanup lag1
            self.cleanlast()
            # cleanup counter, acl_entry, acl_table

            self.cleanlast()
            self.cleanlast()
        return


    def BindPointL3PortTest(self):
        print("BindPointL3PortTest()")
        acl_table = self.add_acl_table(self.device,
                type=self.table_type,
                bind_point_type=[acl_table_bp_port],
                direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_entry = self.add_acl_entry(self.device,
                dst_ip='10.10.10.1',
                dst_ip_mask='255.255.255.255',
                packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
                table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry)

        # Attach acl_table to rif port8
        self.attribute_set(self.port8, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, acl_table)
        try:
            pre_counter = self.object_counters_get(acl_entry)
            print("Sending Ipv4 packet packet rif port 8 -> port 9 (drop)")
            send_packet(self, self.devports[8], self.l3_pkt1)
            verify_no_other_packets(self, timeout=2)
            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)

            #send acl_miss packet from port 8 and test for no drop
            route0 = self.add_route(self.device, ip_prefix='60.10.10.1', vrf_handle=self.vrf10,
                            nexthop_handle=self.nhop1)
            l3_pkt2 = simple_tcp_packet(pktlen=pkt_len,
                eth_dst='00:77:66:55:44:33',
               eth_src='00:22:22:22:22:22',
               ip_dst='60.10.10.1',
               ip_src='192.168.0.1',
               ip_id=105,
               ip_ttl=64)

            l3_exp_pkt2 = simple_tcp_packet(pktlen=pkt_len,
                eth_dst='00:11:22:33:44:55',
               eth_src='00:77:66:55:44:33',
               ip_dst='60.10.10.1',
               ip_src='192.168.0.1',
               ip_id=105,
               ip_ttl=63)
            print("send acl_miss packet and test for no drop")
            print("Sending Ipv4 packet rif port 8 -> port 9 (no drop)")
            send_packet(self, self.devports[8], l3_pkt2)
            verify_packets(self, l3_exp_pkt2, [self.devports[9]])
            self.cleanlast()

            print("Sending IPv4 packet rif port 10 -> rif port 9(no drop)")
            send_packet(self, self.devports[10], self.l3_pkt1)
            verify_packets(self, self.l3_exp_pkt1, [self.devports[9]])
        finally:
            self.attribute_set(self.port8, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            # cleanup counter, acl_entry, acl_table

            self.cleanlast()
            self.cleanlast()
        return

    def BindPointL3PortRifTest(self):
        print("BindPointL3PortRifTest()")
        if(self.client.is_feature_enable(SWITCH_FEATURE_BD_LABEL_IN_INGRESS_ACL) == 0):
            print("Ingress ACL BD_LABEL feature is not supported. VLAN and RIF BindPoint/Label Tests")
            return
        acl_table = self.add_acl_table(self.device,
                type=self.table_type,
                bind_point_type=[acl_table_bp_rif],
                direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_entry = self.add_acl_entry(self.device,
                dst_ip='10.10.10.1',
                dst_ip_mask='255.255.255.255',
                packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
                table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry)

        # Attach acl_table to rif port8
        self.attribute_set(self.rif0, SWITCH_RIF_ATTR_INGRESS_ACL_HANDLE, acl_table)
        try:
            pre_counter = self.object_counters_get(acl_entry)
            print("Sending Ipv4 packet packet rif port 8 -> port 9 (drop)")
            send_packet(self, self.devports[8], self.l3_pkt1)
            verify_no_other_packets(self, timeout=2)
            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)

            #send acl_miss packet from port 8 and test for no drop
            route0 = self.add_route(self.device, ip_prefix='60.10.10.1', vrf_handle=self.vrf10,
                            nexthop_handle=self.nhop1)
            l3_pkt2 = simple_tcp_packet(pktlen=pkt_len,
                eth_dst='00:77:66:55:44:33',
               eth_src='00:22:22:22:22:22',
               ip_dst='60.10.10.1',
               ip_src='192.168.0.1',
               ip_id=105,
               ip_ttl=64)

            l3_exp_pkt2 = simple_tcp_packet(pktlen=pkt_len,
                eth_dst='00:11:22:33:44:55',
               eth_src='00:77:66:55:44:33',
               ip_dst='60.10.10.1',
               ip_src='192.168.0.1',
               ip_id=105,
               ip_ttl=63)
            print("send acl_miss packet and test for no drop")
            print("Sending Ipv4 packet rif port 8 -> port 9 (no drop)")
            send_packet(self, self.devports[8], l3_pkt2)
            verify_packets(self, l3_exp_pkt2, [self.devports[9]])
            self.cleanlast()

            print("Sending IPv4 packet rif port 10 -> rif port 9(no drop)")
            send_packet(self, self.devports[10], self.l3_pkt1)
            verify_packets(self, self.l3_exp_pkt1, [self.devports[9]])

            # remove acl from rif and test for no drop
            self.attribute_set(self.rif0, SWITCH_RIF_ATTR_INGRESS_ACL_HANDLE, 0)
            print("Sending Ipv4 packet packet rif port 8 -> port 9 (no drop)")
            send_packet(self, self.devports[8], self.l3_pkt1)
            verify_packets(self, self.l3_exp_pkt1, [self.devports[9]])

        finally:
            self.attribute_set(acl_entry, SWITCH_ACL_ENTRY_ATTR_ACTION_COUNTER_HANDLE, 0)
            self.cleanlast()
            self.cleanlast()
        return



    def BindPointL3SVIRifTest(self):
        print("BindPointL3RifTest()")
        acl_table = self.add_acl_table(self.device,
                type=self.table_type,
                bind_point_type=[acl_table_bp_rif],
                direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_entry = self.add_acl_entry(self.device,
                dst_ip='10.10.10.5',
                dst_ip_mask='255.255.255.255',
                #tcp_flags=2,
                #tcp_flags_mask=127,
                packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
                table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry)

        # Attach acl_table to rif vlan10
        print("Apply ACL to rif VLAN10")
        self.attribute_set(self.vlan10_rif, SWITCH_RIF_ATTR_INGRESS_ACL_HANDLE, acl_table)
        try:
            pre_counter = self.object_counters_get(acl_entry)
            print("Sending IPv4 packet port 0,SVI_VLAN10 -> lag 1 (drop)")
            send_packet(self, self.devports[0], self.l3_pkt2)
            verify_no_other_packets(self, timeout=2)
            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)

            # send acl_miss packet and test for no drop
            print("Send acl_miss packet and test for no drop")
            print("Sending IPv4 packet rif port 0 -> rif port 9(no drop)")
            send_packet(self, self.devports[0], self.l3_pkt1)
            verify_packets(self, self.l3_exp_pkt1, [self.devports[9]])

            # send bridge packet and test for no drop
            print("Sending vlan 10 bridge packet port 0 -> lag1 (no drop)")
            send_packet(self, self.devports[0], self.vlan10_lag_pkt2)
            verify_any_packet_any_port(self, [self.vlan10_lag_pkt2, self.vlan10_lag_pkt2],
                    [self.devports[6], self.devports[7]])

            print("unset ACL from rif VLAN10")
            self.attribute_set(self.vlan10_rif, SWITCH_RIF_ATTR_INGRESS_ACL_HANDLE, 0)
            print("Sending IPv4 packet rif port 0 -> rif port 9(no drop)")
            send_packet(self, self.devports[0], self.l3_pkt1)
            verify_packets(self, self.l3_exp_pkt1, [self.devports[9]])
        finally:
            self.cleanlast()
            self.cleanlast()
        return


    def BindPointL3LagTest(self):
        print("BindPointL3LagTest()")
        acl_table = self.add_acl_table(self.device,
                type=self.table_type,
                bind_point_type=[acl_table_bp_lag],
                direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_entry = self.add_acl_entry(self.device,
                dst_ip='10.10.10.1',
                dst_ip_mask='255.255.255.255',
                packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
                table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry)
        lag1 = self.add_lag(self.device)
        rif1 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=lag1,
                             vrf_handle=self.vrf10, src_mac=self.rmac)

        # Attach acl_table to rif lag1
        self.attribute_set(lag1, SWITCH_LAG_ATTR_INGRESS_ACL_HANDLE, acl_table)
        try:
            print("check for no drop before adding lag member")
            print("Sending IPv4 packet rif lag1 -> rif port 9(no drop)")
            send_packet(self, self.devports[10], self.l3_pkt1)
            verify_packets(self, self.l3_exp_pkt1, [self.devports[9]])

            #add lag member to lag1
            print("add lag member and check for drop")
            lag_mbr1 = self.add_lag_member(self.device, lag_handle=lag1, port_handle=self.port13)
            pre_counter = self.object_counters_get(acl_entry)
            print("Sending Ipv4 packet packet rif lag1 -> rif port 9 (drop)")
            send_packet(self, self.devports[13], self.l3_pkt1)
            verify_no_other_packets(self, timeout=2)
            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)


            # send acl_miss packet and test for no drop
            route1 = self.add_route(self.device, ip_prefix='30.10.10.5', vrf_handle=self.vrf10,
                     nexthop_handle=self.nhop1)
            l3_pkt1 = simple_tcp_packet(pktlen=pkt_len,
                eth_dst='00:77:66:55:44:33',
               eth_src='00:22:22:22:22:22',
               ip_dst='30.10.10.5',
               ip_src='192.168.0.1',
               ip_id=105,
               ip_ttl=64)
            l3_exp_pkt1 = simple_tcp_packet(pktlen=pkt_len,
                eth_dst='00:11:22:33:44:55',
               eth_src='00:77:66:55:44:33',
               ip_dst='30.10.10.5',
               ip_src='192.168.0.1',
               ip_id=105,
               ip_ttl=63)

            print("Send acl_miss packet and test for no drop")
            print("Sending Ipv4 packet packet rif port13 -> rif 9(no drop)")
            send_packet(self, self.devports[13], l3_pkt1)
            verify_packets(self, l3_exp_pkt1, [self.devports[9]])
            self.cleanlast()


            print("unset acl from lag1 and check for no drop")
            self.attribute_set(lag1, SWITCH_LAG_ATTR_INGRESS_ACL_HANDLE, 0)
            send_packet(self, self.devports[13], self.l3_pkt1)
            verify_packets(self, self.l3_exp_pkt1, [self.devports[9]])

            # remove lag member and add back as normal l3 port.
            print("remove lag_member, add back port as normal rif port and check for no drop")
            self.cleanlast()
            rif2 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT,
                             port_handle=self.port13, vrf_handle=self.vrf10, src_mac=self.rmac)
            print("Sending IPv4 packet rif port port13 -> rif port 9(no drop)")
            send_packet(self, self.devports[13], self.l3_pkt1)
            verify_packets(self, self.l3_exp_pkt1, [self.devports[9]])
        finally:
            #cleanup rif2,rif1s
            self.cleanlast()
            self.cleanlast()
            #deattach acl from lag1
            #self.attribute_set(lag1, SWITCH_LAG_ATTR_INGRESS_ACL_HANDLE, 0)
            #remove lags
            self.cleanlast()
            # cleanup counter, acl_entry, acl_table
            self.cleanlast()
            self.cleanlast()
        return

    def BindPointVlanTest(self):
        print("BindPointVlanTest()")
        vlan_acl_table = self.add_acl_table(self.device,
                type=self.table_type,
                bind_point_type=[acl_table_bp_vlan],
                direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        vlan_acl_entry = self.add_acl_entry(self.device,
                dst_ip='20.20.20.1',
                dst_ip_mask='255.255.255.255',
                #tcp_flags=2,
                #tcp_flags_mask=127,
                packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
                table_handle=vlan_acl_table)
        self.client.object_counters_clear_all(vlan_acl_entry)

        acl_counter_handle = self.add_acl_counter(self.device)
        self.attribute_set(vlan_acl_entry, SWITCH_ACL_ENTRY_ATTR_ACTION_COUNTER_HANDLE, acl_counter_handle)
        # Attach acl_table to vlan10
        self.attribute_set(self.vlan20, SWITCH_VLAN_ATTR_INGRESS_ACL_HANDLE, vlan_acl_table)
        try:
            # add vlan member
            vlan_member0 = self.add_vlan_member(self.device, vlan_handle=self.vlan20,
                                member_handle=self.port13)
            self.attribute_set(self.port13, SWITCH_PORT_ATTR_PORT_VLAN_ID, 20)
            pre_counter = self.object_counters_get(vlan_acl_entry)
            send_packet(self, self.devports[13], self.vlan20_pkt1)
            verify_no_other_packets(self, timeout=2)
            post_counter = self.object_counters_get(vlan_acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)

            # send acl_miss packet and test for no drop
            vlan20_pkt2 = simple_tcp_packet(pktlen=pkt_len,
                eth_dst='00:66:66:66:66:66',
               eth_src='00:77:77:77:77:77',
               ip_dst='20.20.20.2',
               ip_src='192.168.0.1',
               ip_id=105,
               ip_ttl=64)

            print("Sending acl_miss packet and check for no drop")
            print("Sending vlan20 L2 packet from port 13 -> port 4")
            send_packet(self, self.devports[13], vlan20_pkt2)
            verify_packets(self, vlan20_pkt2, [self.devports[4]])

        finally:
            # cleanup vlan member
            self.attribute_set(self.port13, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.vlan20, SWITCH_VLAN_ATTR_INGRESS_ACL_HANDLE, 0)
            self.cleanlast()
            if vlan_acl_entry:
                self.attribute_set(vlan_acl_entry, SWITCH_ACL_ENTRY_ATTR_ACTION_COUNTER_HANDLE, 0)
            # cleanup acl counter, acl_entry, acl_table
            self.cleanlast()
            self.cleanlast()
        return

###############################################################################
@group('acl')
class EgressAclBindPointTest(ApiHelper):
    def runTest(self):
        print("EgressAclBindPointTest()")
        if (self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_IP_ACL) == 0):
            print("EgressAcl  feature not enabled, skipping")
            return
        self.configure()
        self.ConfigureL2()
        self.ConfigureL3()

        try:
            self.BindPointL2PortTest()
            self.BindPointL2LagTest()
            self.BindPointL3PortTest()
            self.BindPointL3PortIPv4MirrorAclTest()
            self.BindPointL3LagTest()
            if(self.client.is_feature_enable(SWITCH_FEATURE_BD_LABEL_IN_EGRESS_ACL) == 0):
                print("Egress ACL BD_LABEL feature is not supported. VLAN and RIF BindPoint/Label Tests")
                return
            else:
                self.BindPointVlanTest()
                self.BindPointL3RifTest()
        finally:
            self.CleanupL2()
            self.cleanup()
        return

    def BindPointL2PortTest(self):
        print("BindPointL2PortTest()")
        acl_table = self.add_acl_table(self.device,
                type=SWITCH_ACL_TABLE_ATTR_TYPE_IPV4,
                bind_point_type=[acl_table_bp_port],
                direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_EGRESS)
        acl_entry = self.add_acl_entry(self.device,
                dst_ip='10.10.10.1',
                dst_ip_mask='255.255.255.255',
                packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
                table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry)

        # Attach acl_table to vlan10, port2
        self.attribute_set(self.port2, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, acl_table)
        try:
            pre_counter = self.object_counters_get(acl_entry)
            print("Sending vlan 10 packet port 0 -> port 2 (drop)")
            send_packet(self, self.devports[0], self.vlan10_pkt1)
            verify_no_other_packets(self, timeout=2)
            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)

            pre_counter = self.object_counters_get(acl_entry)
            print("Sending vlan 10 packet port 1 -> port 2 (drop)")
            send_packet(self, self.devports[1], self.vlan10_pkt1)
            verify_no_other_packets(self, timeout=2)
            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)

            # send acl-miss packet and test for no drop
            print("send acl_miss packet and test for no drop")
            vlan10_pkt2 = simple_tcp_packet(pktlen=pkt_len,
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
            vlan10_pkt2 = simple_tcp_packet(pktlen=pkt_len,
                eth_dst='00:44:44:44:44:48',
              eth_src='00:44:44:44:44:42',
              ip_dst='10.10.10.1',
              ip_src='192.168.0.1',
              ip_id=105,
              ip_ttl=64)

            pre_counter = self.object_counters_get(acl_entry)
            print("Sending vlan 10 packet port 0 -> port 2 (no drop)")
            send_packet(self, self.devports[0], vlan10_pkt2)
            verify_no_packet(self, vlan10_pkt2, self.devports[2], timeout=2)
            verify_any_packet_on_ports_list(self, [vlan10_pkt2],
                 [[self.devports[1]], [self.devports[6], self.devports[7]]])
            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)

            print("unset egress-acl from port2")
            self.attribute_set(self.port2, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, 0)
            #send packet from vlan10, port1 and verify packet not dropped
            print("Sending vlan 10 packet port 1 -> port 2 (no drop)")
            send_packet(self, self.devports[1], self.vlan10_pkt1)
            verify_packets(self, self.vlan10_pkt1, [self.devports[2]])
        finally:
            # cleanup counter, acl_entry, acl_table
            self.cleanlast()
            self.cleanlast()
        return

    def BindPointL2LagTest(self):
        print("BindPointL2LagTest()")
        acl_table = self.add_acl_table(self.device,
                type=SWITCH_ACL_TABLE_ATTR_TYPE_IPV4,
                bind_point_type=[acl_table_bp_lag],
                direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_EGRESS)
        acl_entry = self.add_acl_entry(self.device,
                dst_ip='10.10.10.1',
                dst_ip_mask='255.255.255.255',
                packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
                table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry)

        lag1 = self.add_lag(self.device)
        vlan_member1 = self.add_vlan_member(self.device, vlan_handle=self.vlan10,
                       member_handle=lag1)
        self.attribute_set(lag1, SWITCH_LAG_ATTR_PORT_VLAN_ID, 10)
        self.attribute_set(lag1, SWITCH_LAG_ATTR_EGRESS_ACL_HANDLE, acl_table)
        try:
            print("check for no drop before adding lag member")
            print("Sending vlan 10 packet port 1 -> port 2 (no drop)")
            send_packet(self, self.devports[1], self.vlan10_pkt1)
            verify_packets(self, self.vlan10_pkt1, [self.devports[2]])

            # add lag member
            print("add lag member and check for drop")
            lag_mbr1 = self.add_lag_member(self.device, lag_handle=lag1, port_handle=self.port13)

            mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan10,
                                      mac_address='00:44:44:44:44:50',
                                      destination_handle=lag1)

            # send packet from port0 to lag1
            vlan10_lag_pkt = simple_tcp_packet(pktlen=pkt_len,
            eth_dst='00:44:44:44:44:50',
            eth_src='00:44:44:44:44:42',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)

            pre_counter = self.object_counters_get(acl_entry)
            print("Sending vlan 10 packet port 0 -> lag1(port13) (drop)")
            send_packet(self, self.devports[0], vlan10_lag_pkt)
            verify_no_other_packets(self, timeout=2)
            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)

            # send acl-miss packet and test for no drop
            print("Send acl_miss packet and test for no drop")
            vlan10_lag_pkt2 = simple_tcp_packet(pktlen=pkt_len,
                eth_dst='00:44:44:44:44:50',
               eth_src='00:44:44:44:44:42',
               ip_dst='50.10.10.1',
               ip_src='192.168.0.1',
               ip_id=105,
               ip_ttl=64)

            print("Sending vlan 10 packet port 0 -> lag1(port13) (no drop)")
            send_packet(self, self.devports[0], vlan10_lag_pkt2)
            verify_packets(self, vlan10_lag_pkt2, [self.devports[13]])

            print("unset egress-acl from lag1")
            self.attribute_set(lag1, SWITCH_LAG_ATTR_EGRESS_ACL_HANDLE, 0)
            print("Sending vlan 10 packet port 0 -> lag1(port13) (no drop)")
            send_packet(self, self.devports[0], vlan10_lag_pkt)
            verify_packets(self, vlan10_lag_pkt, [self.devports[13]])

            # remove lag member and add back as normal l2 port.
            print("remove lag_member,mac_entry add back port as normal l2 port and check for no drop")
            #clean up mac0, lag_mbr1
            self.cleanlast()
            self.cleanlast()
            mac1 = self.add_mac_entry(self.device, vlan_handle=self.vlan10,
                                      mac_address='00:44:44:44:44:50',
                                      destination_handle=self.port13)
            vlan_member2 = self.add_vlan_member(self.device, vlan_handle=self.vlan10,
                              member_handle=self.port13)
            self.attribute_set(self.port13, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)

            print("Sending vlan 10 packet port 0 -> lag1(port13) (no drop)")
            send_packet(self, self.devports[0], vlan10_lag_pkt)
            verify_packets(self, vlan10_lag_pkt, [self.devports[13]])

        finally:
            #cleanup vlan_member2,mac1, vlan_member1
            self.attribute_set(self.port13, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(lag1, SWITCH_LAG_ATTR_PORT_VLAN_ID, 0)
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            #cleanup lag1
            self.cleanlast()
            # cleanup counter, acl_entry, acl_table
            self.cleanlast()
            self.cleanlast()
        return

    def BindPointL3PortTest(self):
        print("BindPointL3PortTest()")
        acl_table = self.add_acl_table(self.device,
                type=SWITCH_ACL_TABLE_ATTR_TYPE_IPV4,
                bind_point_type=[acl_table_bp_port],
                direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_EGRESS)
        acl_entry = self.add_acl_entry(self.device,
                dst_ip='10.10.10.1',
                dst_ip_mask='255.255.255.255',
                packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
                table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry)

        # Attach acl_table to rif port9
        print("Attach acl_table to rif port 9")
        self.attribute_set(self.port9, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, acl_table)
        try:
            pre_counter = self.object_counters_get(acl_entry)
            print("Sending Ipv4 packet rif port 8 -> port 9 (drop)")
            send_packet(self, self.devports[8], self.l3_pkt1)
            verify_no_other_packets(self, timeout=2)
            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)

            # send acl_miss packet and test for no drop
            route0 = self.add_route(self.device, ip_prefix='60.10.10.1', vrf_handle=self.vrf10,
                            nexthop_handle=self.nhop1)
            l3_pkt2 = simple_tcp_packet(pktlen=pkt_len,
                eth_dst='00:77:66:55:44:33',
               eth_src='00:22:22:22:22:22',
               ip_dst='60.10.10.1',
               ip_src='192.168.0.1',
               ip_id=105,
               ip_ttl=64)

            l3_exp_pkt2 = simple_tcp_packet(pktlen=pkt_len,
                eth_dst='00:11:22:33:44:55',
               eth_src='00:77:66:55:44:33',
               ip_dst='60.10.10.1',
               ip_src='192.168.0.1',
               ip_id=105,
               ip_ttl=63)
            print("send acl_miss packet and test for no drop")
            print("Sending Ipv4 packet rif port 8 -> port 9 (no drop)")
            send_packet(self, self.devports[8], l3_pkt2)
            verify_packets(self, l3_exp_pkt2, [self.devports[9]])
            self.cleanlast()

            # unset acl_table from rif port9
            print("Unset acl table from rif port9")
            self.attribute_set(self.port9, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, 0)
            print("Sending IPv4 packet rif port 8 -> rif port 9(no drop)")
            send_packet(self, self.devports[8], self.l3_pkt1)
            verify_packets(self, self.l3_exp_pkt1, [self.devports[9]])

            print("Sending IPv4 packet rif port 10 -> rif port 9(no drop)")
            send_packet(self, self.devports[10], self.l3_pkt1)
            verify_packets(self, self.l3_exp_pkt1, [self.devports[9]])
        finally:
            # cleanup counter, acl_entry, acl_table
            self.cleanlast()
            self.cleanlast()
        return


    def BindPointL3PortIPv4MirrorAclTest(self):
        print("BindPointL3PortAclMirrorTest()")
        if (self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_IP_ACL) == 0):
            print("EgressIPv4 mirror acl feature not enabled, skipping")
            return
        print("Erspan egress ACL mirror routed packet")# from 0 -> 1, mirror to 3")

        mirror = self.add_mirror(self.device,
            type=SWITCH_MIRROR_ATTR_TYPE_ENHANCED_REMOTE,
            direction=SWITCH_MIRROR_ATTR_DIRECTION_EGRESS,
            rspan_type=SWITCH_MIRROR_ATTR_RSPAN_TYPE_NONE,
            erspan_type=SWITCH_MIRROR_ATTR_ERSPAN_TYPE_ERSPAN_3,
            ttl=64,
            platform_info=False,
            egress_port_handle=self.port10,
            src_mac_address='00:33:33:33:33:33',
            dest_mac_address='00:44:44:44:44:44',
            src_ip='10.0.0.1',
            dest_ip='20.0.0.1')
        session_id = self.attribute_get(mirror, SWITCH_MIRROR_ATTR_SESSION_ID)
        acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IPV4,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_EGRESS)
        acl_entry = self.add_acl_entry(self.device,
            dst_ip='10.10.10.1',
            dst_ip_mask='255.255.255.255',
            action_egress_mirror_handle=mirror,
            table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry)

        # Attach acl_table to rif port9
        print("Attach acl_table to rif port 9")
        self.attribute_set(self.port9, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, acl_table)

        ERSPAN_3 = 2
        exp_mirrored_pkt = ptf.mask.Mask(ipv4_erspan_pkt(pktlen=pkt_len,
            eth_dst='00:44:44:44:44:44',
            eth_src='00:33:33:33:33:33',
            ip_src='10.0.0.1',
            ip_dst='20.0.0.1',
            ip_id=0,
            ip_ttl=64,
            ip_flags=0x0,
            version=ERSPAN_3,
            mirror_id=session_id,
            erspan_gra=2,
            inner_frame=self.l3_exp_pkt1))

        # IPv4.total_length is different on Model and Harware because of 4B CRC.
        mask_set_do_not_care_packet(exp_mirrored_pkt, IP, "len")
        mask_set_do_not_care_packet(exp_mirrored_pkt, IP, "chksum")
        mask_set_do_not_care_packet(exp_mirrored_pkt, ERSPAN_III, "timestamp")

        try:
            pre_counter = self.object_counters_get(acl_entry)
            print("Sending Ipv4 packet rif port 8 -> port 9 (mirror -> port 10)")
            send_packet(self, self.devports[8], self.l3_pkt1)
            p1 = [self.devports[9], [self.l3_exp_pkt1]]
            p2 = [self.devports[10], [exp_mirrored_pkt]]
            verify_each_packet_on_each_port(self, [self.l3_exp_pkt1, exp_mirrored_pkt], [self.devports[9], self.devports[10]])
            verify_no_other_packets(self, timeout=2)
            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)

            # send acl_miss packet and test for no drop
            route0 = self.add_route(self.device, ip_prefix='60.10.10.1', vrf_handle=self.vrf10,
                            nexthop_handle=self.nhop1)
            l3_pkt2 = simple_tcp_packet(pktlen=pkt_len,
                eth_dst='00:77:66:55:44:33',
               eth_src='00:22:22:22:22:22',
               ip_dst='60.10.10.1',
               ip_src='192.168.0.1',
               ip_id=105,
               ip_ttl=64)

            l3_exp_pkt2 = simple_tcp_packet(pktlen=pkt_len,
                eth_dst='00:11:22:33:44:55',
               eth_src='00:77:66:55:44:33',
               ip_dst='60.10.10.1',
               ip_src='192.168.0.1',
               ip_id=105,
               ip_ttl=63)
            print("send acl_miss packet and test for no mirror")
            print("Sending Ipv4 packet rif port 8 -> port 9 (no mirror)")
            send_packet(self, self.devports[8], l3_pkt2)
            verify_packet(self, l3_exp_pkt2, self.devports[9])
            verify_no_other_packets(self, timeout=2)
            self.cleanlast()

            # unset acl_table from rif port9
            print("Unset acl table from rif port9")
            self.attribute_set(self.port9, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, 0)
            print("Sending IPv4 packet rif port 8 -> port 9 (no mirror)")
            send_packet(self, self.devports[8], self.l3_pkt1)
            verify_packet(self, self.l3_exp_pkt1, self.devports[9])
            verify_no_other_packets(self, timeout=2)

        finally:
            self.attribute_set(self.port9, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, 0)
            # remove the acl entry, table and mirror
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
        return


    def BindPointL3LagTest(self):
        print("BindPointL3LagTest()")
        acl_table = self.add_acl_table(self.device,
                type=SWITCH_ACL_TABLE_ATTR_TYPE_IPV4,
                bind_point_type=[acl_table_bp_lag],
                direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_EGRESS)
        acl_entry = self.add_acl_entry(self.device,
                dst_ip='30.10.10.4',
                dst_ip_mask='255.255.255.255',
                packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
                table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry)
        lag1 = self.add_lag(self.device)

        # Attach acl_table to rif lag1
        print("Attach egress acl table to rif lag1")
        self.attribute_set(lag1, SWITCH_LAG_ATTR_EGRESS_ACL_HANDLE, acl_table)
        try:
            print("check for no drop before adding lag member")
            print("Sending IPv4 packet rif lag1 -> rif port 9(no drop)")
            send_packet(self, self.devports[10], self.l3_pkt1)
            verify_packets(self, self.l3_exp_pkt1, [self.devports[9]])

            #add lag member to lag1
            print("add lag member and check for drop")
            lag_mbr1 = self.add_lag_member(self.device, lag_handle=lag1, port_handle=self.port13)
            #self.attribute_set(lag1, SWITCH_LAG_ATTR_EGRESS_ACL_HANDLE, acl_table)

            rif1 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=lag1,
                           vrf_handle=self.vrf10, src_mac=self.rmac)
            nhop1 = self.add_nexthop(self.device, handle=rif1, dest_ip='10.10.0.7')
            neighbor1 = self.add_neighbor(self.device, mac_address='00:11:22:34:45:57',
                             handle=rif1, dest_ip='10.10.0.7')
            route1 = self.add_route(self.device, ip_prefix='30.10.10.4', vrf_handle=self.vrf10,
                     nexthop_handle=nhop1)

            l3_lag_pkt1 = simple_tcp_packet(pktlen=pkt_len,
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='30.10.10.4',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_ttl=64)
            l3_exp_lag_pkt1 = simple_tcp_packet(pktlen=pkt_len,
                eth_dst='00:11:22:34:45:57',
                eth_src='00:77:66:55:44:33',
                ip_dst='30.10.10.4',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_ttl=63)

            pre_counter = self.object_counters_get(acl_entry)
            print("Sending Ipv4 packet rif port10 -> rif lag1(drop)")
            send_packet(self, self.devports[10], l3_lag_pkt1)
            verify_no_other_packets(self, timeout=2)
            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)

            # send acl_miss packet and test for no drop
            route2 = self.add_route(self.device, ip_prefix='30.10.10.5', vrf_handle=self.vrf10,
                     nexthop_handle=nhop1)
            l3_lag_pkt2 = simple_tcp_packet(pktlen=pkt_len,
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='30.10.10.5',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_ttl=64)
            l3_exp_lag_pkt2 = simple_tcp_packet(pktlen=pkt_len,
                eth_dst='00:11:22:34:45:57',
                eth_src='00:77:66:55:44:33',
                ip_dst='30.10.10.5',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_ttl=63)

            print("Send acl_miss packet and test for no drop")
            print("Sending Ipv4 packet packet rif port10 -> rif lag1(no drop)")
            send_packet(self, self.devports[10], l3_lag_pkt2)
            verify_packets(self, l3_exp_lag_pkt2, [self.devports[13]])
            self.cleanlast()

            # reset acl and test for no drop
            print("unset egress acl_table from lag1 and check for no drop")
            self.attribute_set(lag1, SWITCH_LAG_ATTR_EGRESS_ACL_HANDLE, 0)
            send_packet(self, self.devports[10], l3_lag_pkt1)
            verify_packets(self, l3_exp_lag_pkt1, [self.devports[13]])

            # remove lag member, route and add back as normal l3 port.
            print("remove lag_member,route, add back port as normal rif port and check for no drop")
            # remove route1, nbr1, nhop1 , rif1 and lag_mbr1
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            rif2 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT,
                             port_handle=self.port13, vrf_handle=self.vrf10,
                             src_mac=self.rmac)

            nhop2 = self.add_nexthop(self.device, handle=rif2, dest_ip='10.10.0.8')
            neighbor2 = self.add_neighbor(self.device, mac_address='00:11:22:34:45:58',
                             handle=rif2, dest_ip='10.10.0.8')
            route2 = self.add_route(self.device, ip_prefix='30.10.10.5', vrf_handle=self.vrf10,
                     nexthop_handle=nhop2)

            l3_pkt2 = simple_tcp_packet(pktlen=pkt_len,
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='30.10.10.5',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_ttl=64)
            l3_exp_pkt2 = simple_tcp_packet(pktlen=pkt_len,
                eth_dst='00:11:22:34:45:58',
                eth_src='00:77:66:55:44:33',
                ip_dst='30.10.10.5',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_ttl=63)
            print("Sending IPv4 packet rif port port10 -> rif port 13(no drop)")
            send_packet(self, self.devports[10], l3_pkt2)
            verify_packets(self, l3_exp_pkt2, [self.devports[13]])
        finally:
            #cleanup route2, nhopr, neighbor2, rif2
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            #remove lag1
            self.cleanlast()
            # cleanup counter, acl_entry, acl_table
            self.cleanlast()
            self.cleanlast()
        return

    def BindPointVlanTest(self):
        print("BindPointVlanTest()")
        vlan_acl_table = self.add_acl_table(self.device,
                type=SWITCH_ACL_TABLE_ATTR_TYPE_IPV4,
                bind_point_type=[acl_table_bp_vlan],
                direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_EGRESS)
        vlan_acl_entry = self.add_acl_entry(self.device,
                dst_ip='20.20.20.1',
                dst_ip_mask='255.255.255.255',
                #tcp_flags=2,
                #tcp_flags_mask=127,
                packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
                table_handle=vlan_acl_table)
        self.client.object_counters_clear_all(vlan_acl_entry)

        # Attach acl_table to vlan20
        self.attribute_set(self.vlan20, SWITCH_VLAN_ATTR_EGRESS_ACL_HANDLE, vlan_acl_table)
        try:
            # add vlan member
            vlan_member0 = self.add_vlan_member(self.device, vlan_handle=self.vlan20,
                                member_handle=self.port13)
            self.attribute_set(self.port13, SWITCH_PORT_ATTR_PORT_VLAN_ID, 20)
            pre_counter = self.object_counters_get(vlan_acl_entry)
            send_packet(self, self.devports[13], self.vlan20_pkt1)
            verify_no_other_packets(self, timeout=2)
            post_counter = self.object_counters_get(vlan_acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)

            # send acl_miss packeet and test for no drop
            vlan20_pkt2 = simple_tcp_packet(pktlen=pkt_len,
                eth_dst='00:66:66:66:66:66',
               eth_src='00:77:77:77:77:77',
               ip_dst='20.20.20.2',
               ip_src='192.168.0.1',
               ip_id=105,
               ip_ttl=64)

            print("test sending acl_miss packet and check for no drop")
            print("Sending vlan20 L2 packet from port 5 -> port 4")
            send_packet(self, self.devports[5], vlan20_pkt2)
            verify_packets(self, vlan20_pkt2, [self.devports[4]])

        finally:
            # cleanup vlan member
            self.attribute_set(self.port13, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.vlan20, SWITCH_VLAN_ATTR_EGRESS_ACL_HANDLE, 0)
            self.cleanlast()
            # cleanup acl counter, acl_entry, acl_table
            self.cleanlast()
            self.cleanlast()
        return


    def BindPointL3RifTest(self):
        print("BindPointL3RifTest()")
        acl_table = self.add_acl_table(self.device,
                type=SWITCH_ACL_TABLE_ATTR_TYPE_IPV4,
                bind_point_type=[acl_table_bp_rif],
                direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_EGRESS)
        acl_entry = self.add_acl_entry(self.device,
                dst_ip='10.10.10.5',
                dst_ip_mask='255.255.255.255',
                #tcp_flags=2,
                #tcp_flags_mask=127,
                packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
                table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry)
        # Attach acl_table to rif vlan10
        print("Apply ACL to rif VLAN10")
        self.attribute_set(self.vlan10_rif, SWITCH_RIF_ATTR_EGRESS_ACL_HANDLE, acl_table)
        try:
            pre_counter = self.object_counters_get(acl_entry)
            print("Sending IPv4 packet port 8-> SVI_VLAN10 (drop)")
            send_packet(self, self.devports[8], self.l3_pkt2)
            verify_no_other_packets(self, timeout=2)
            post_counter = self.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 1)

            # send acl_miss packet and test for no drop
            route0 = self.add_route(self.device, ip_prefix='31.10.10.5', vrf_handle=self.vrf10,
                     nexthop_handle=self.nhop4)

            svi_l3_pkt1 = simple_tcp_packet(pktlen=pkt_len,
                eth_dst='00:77:66:55:44:33',
               eth_src='00:22:22:22:22:22',
               ip_dst='31.10.10.5',
               ip_src='192.168.0.1',
               ip_id=105,
               ip_ttl=64)
            svi_l3_exp_pkt2 = simple_tcp_packet(pktlen=pkt_len,
                eth_dst='00:55:55:55:55:55',
               eth_src='00:77:66:55:44:33',
               ip_dst='31.10.10.5',
               ip_src='192.168.0.1',
               ip_id=105,
               ip_ttl=63)

            print("send acl_miss packet and test for no drop")
            print("Sending IPv4 packet port 8-> SVI_VLAN10:lag1:port6/port7 (no drop)")
            send_packet(self, self.devports[8], svi_l3_pkt1)
            verify_any_packet_any_port(self, [svi_l3_exp_pkt2, svi_l3_exp_pkt2],
                    [self.devports[6], self.devports[7]])
            self.cleanlast()

            #send bridge packet in vlan10 and test for no drop
            print("Sending vlan 10 bridge packet port 2 -> lag1 (no drop)")
            send_packet(self, self.devports[2], self.vlan10_lag_pkt2)
            verify_any_packet_any_port(self, [self.vlan10_lag_pkt2, self.vlan10_lag_pkt2],
                    [self.devports[6], self.devports[7]])

            print("unset ACL from rif VLAN10")
            self.attribute_set(self.vlan10_rif, SWITCH_RIF_ATTR_EGRESS_ACL_HANDLE, 0)
            print("Sending IPv4 packet port 8-> SVI_VLAN10 (no drop)")
            send_packet(self, self.devports[8], self.l3_pkt2)
            verify_any_packet_any_port(self, [self.l3_exp_pkt2, self.l3_exp_pkt2],
                    [self.devports[6], self.devports[7]])
        finally:
            # cleanup counter, acl_entry, acl_table
            self.cleanlast()
            self.cleanlast()
        return

###############################################################################

@group('acl')
class ACLMultipleTablesTest(ApiHelper):
    def setUp(self):
        print()
        self.configure()

        self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_LEARNING, False)

        vlan_member1 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port5, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
        vlan_member2 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port4)
        self.attribute_set(self.port5, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.attribute_set(self.port4, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)

        mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:44:44:44:44:44', destination_handle=self.port5)

        # Create ACL groups
        # MAC, IPv4 RACL, Mirror, IP QOS
        self.acl_group1 = self.add_acl_group(self.device, bind_point_type=[acl_group_bp_port])
        # IPv4, IPv4 RACL, Mirror, IP QOS (instead of Dtel)
        self.acl_group2 = self.add_acl_group(self.device, bind_point_type=[acl_group_bp_port])
        # IPv6, IPv6 RACL, Mirror, IP QOS
        self.acl_group3 = self.add_acl_group(self.device, bind_point_type=[acl_group_bp_port])

        # send the test packet(s)
        template_v4pkt = simple_tcp_packet(pktlen=pkt_len,
            eth_dst='00:44:44:44:44:44',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            dl_vlan_enable=True,
            vlan_vid=10,
            ip_id=105,
            ip_ttl=64)

        self.mac_acl_pkt = template_v4pkt.copy()
        self.mac_acl_pkt[Ether].src = '00:22:22:22:22:25'
        self.exp_mac_acl_pkt = self.mac_acl_pkt.copy()

        self.v4_acl_pkt = template_v4pkt.copy()
        self.v4_acl_pkt[IP].dst = '10.10.10.2'
        self.exp_v4_acl_pkt = self.v4_acl_pkt.copy()

        self.v4_racl_pkt = template_v4pkt.copy()
        self.v4_racl_pkt[IP].dst = '10.10.10.3'
        self.exp_v4_racl_pkt = self.v4_racl_pkt.copy()

        self.mirror_acl_pkt = template_v4pkt.copy()
        self.mirror_acl_pkt[IP].dst = '10.10.10.4'
        self.exp_mirror_acl_pkt = self.mirror_acl_pkt.copy()

        self.qos_acl_pkt = template_v4pkt.copy()
        self.qos_acl_pkt[Dot1Q].prio = 1
        self.qos_acl_pkt[IP].dst = '10.10.10.5'
        self.exp_qos_acl_pkt = self.qos_acl_pkt.copy()

        self.dtel_acl_pkt = template_v4pkt.copy()
        self.dtel_acl_pkt[IP].dst = '10.10.10.6'
        self.exp_dtel_acl_pkt = self.dtel_acl_pkt.copy()

        template_v6pkt = simple_tcpv6_packet(pktlen=pkt_len,
            eth_dst='00:44:44:44:44:44',
            eth_src='00:22:22:22:22:22',
            ipv6_dst='1234:5678:9abc:def0:4422:1133:5577:99aa',
            ipv6_src='2000::1',
            dl_vlan_enable=True,
            vlan_vid=10,
            ipv6_hlim=64)

        self.v6_acl_pkt = template_v6pkt.copy()
        self.exp_v6_acl_pkt = self.v6_acl_pkt.copy()

        self.v6_racl_pkt = template_v6pkt.copy()
        self.v6_racl_pkt[IPv6].dst = '1234:5678:9abc:def0:4422:1133:5577:99bb'
        self.exp_v6_racl_pkt = self.v6_racl_pkt.copy()

        self.MacAclConfigure([self.acl_group1])
        self.IPv4AclConfigure([self.acl_group2])
        self.IPv6AclConfigure([self.acl_group3])
        self.IPv4RaclConfigure([self.acl_group1, self.acl_group2])
        self.IPv6RaclConfigure([self.acl_group3])
        self.MirrorAclConfigure([self.acl_group1, self.acl_group2, self.acl_group3])
        self.IPQosAclConfigure([self.acl_group1, self.acl_group2, self.acl_group3])
        # Use IP QOS instead of DTEL until there is another way to figure out whether
        # this is M1 profile or not for ACL labels allocations. See label_supp_for_racl_mac_qos_acl
        #self.DtelConfigure([self.acl_group2])

    def runTest(self):
        if(self.client.is_feature_enable(SWITCH_FEATURE_ACL_PORT_GROUP) == 1):
            print("This ACL test is not supported with port_group")
            return
        try:
            self.NoAclBoundTest()
            self.AclGroup1Test()
            self.AclGroup2Test()
            self.AclGroup3Test()
        finally:
            pass

    def tearDown(self):
        self.attribute_set(self.port4, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
        self.attribute_set(self.port4, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.port5, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.port5, SWITCH_PORT_ATTR_EGRESS_L2_QOS_GROUP, 0)
        self.cleanup()

    def NoAclBoundTest(self):
        print("NoAclBoundTest")
        try:
            print("Sending IPv4 packets port 4 -> port 5")
            send_packet(self, self.devports[4], self.mac_acl_pkt)
            verify_packets(self, self.exp_mac_acl_pkt, [self.devports[5]])
            send_packet(self, self.devports[4], self.v4_acl_pkt)
            verify_packets(self, self.exp_v4_acl_pkt, [self.devports[5]])
            send_packet(self, self.devports[4], self.v6_acl_pkt)
            verify_packets(self, self.exp_v6_acl_pkt, [self.devports[5]])
            send_packet(self, self.devports[4], self.v4_racl_pkt)
            verify_packets(self, self.exp_v4_racl_pkt, [self.devports[5]])
            send_packet(self, self.devports[4], self.v6_racl_pkt)
            verify_packets(self, self.exp_v6_racl_pkt, [self.devports[5]])
            send_packet(self, self.devports[4], self.mirror_acl_pkt)
            verify_packets(self, self.exp_mirror_acl_pkt, [self.devports[5]])
            send_packet(self, self.devports[4], self.qos_acl_pkt)
            verify_packets(self, self.exp_qos_acl_pkt, [self.devports[5]])
            #send_packet(self, self.devports[4], self.dtel_acl_pkt)
            #verify_packets(self, self.exp_dtel_acl_pkt, [self.devports[5]])
        finally:
            pass

    def AclGroup1Test(self):
        try:
            print("AclGroup1Test")
            # MAC, IPv4 RACL, Mirror, IP QOS
            self.attribute_set(self.port4, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, self.acl_group1)

            self.TestMacAclTable()
            self.TestIPv4RaclTable()
            self.TestMirrorAclTable()
            self.TestIPQosAclTable

            # Test packets which would hit entries in other ACLs if they were bound
            print("Sending IPv4 packets port 4 -> port 5")
            send_packet(self, self.devports[4], self.v4_acl_pkt)
            verify_packets(self, self.exp_v4_acl_pkt, [self.devports[5]])
            send_packet(self, self.devports[4], self.v6_acl_pkt)
            verify_packets(self, self.exp_v6_acl_pkt, [self.devports[5]])
            send_packet(self, self.devports[4], self.v6_racl_pkt)
            verify_packets(self, self.exp_v6_racl_pkt, [self.devports[5]])
            send_packet(self, self.devports[4], self.dtel_acl_pkt)
            verify_packets(self, self.exp_dtel_acl_pkt, [self.devports[5]])
        finally:
            self.attribute_set(self.port4, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            pass

    def AclGroup2Test(self):
        try:
            print("AclGroup2Test")
            # IPv4, IPv4 RACL, Mirror, IP QOS (instead of Dtel)
            self.attribute_set(self.port4, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, self.acl_group2)

            self.TestIPv4AclTable()
            self.TestIPv4RaclTable()
            self.TestMirrorAclTable()
            #self.TestDtelAclTable()
            self.TestIPQosAclTable

            # Test packets which would hit entries in other ACLs if they were bound
            print("Sending IPv4 packets port 4 -> port 5")
            send_packet(self, self.devports[4], self.mac_acl_pkt)
            verify_packets(self, self.exp_mac_acl_pkt, [self.devports[5]])
            send_packet(self, self.devports[4], self.v6_acl_pkt)
            verify_packets(self, self.exp_v6_acl_pkt, [self.devports[5]])
            send_packet(self, self.devports[4], self.v6_racl_pkt)
            verify_packets(self, self.exp_v6_racl_pkt, [self.devports[5]])
            # Uncomment the below when IP QOS ACLtest is replaced with Dtel
            # send_packet(self, self.devports[4], self.qos_acl_pkt)
            # verify_packets(self, self.exp_qos_acl_pkt, [self.devports[5]])
        finally:
            self.attribute_set(self.port4, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            pass

    def AclGroup3Test(self):
        try:
            print("AclGroup3Test")
            # IPv6, IPv6 RACL, Mirror, IP QOS
            self.attribute_set(self.port4, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, self.acl_group3)

            self.TestIPv6AclTable()
            self.TestIPv6RaclTable()
            self.TestMirrorAclTable()
            self.TestIPQosAclTable()

            # Test packets which would hit entries in other ACLs if they were bound
            print("Sending IPv4 packets port 4 -> port 5")
            send_packet(self, self.devports[4], self.mac_acl_pkt)
            verify_packets(self, self.exp_mac_acl_pkt, [self.devports[5]])
            send_packet(self, self.devports[4], self.v4_acl_pkt)
            verify_packets(self, self.exp_v4_acl_pkt, [self.devports[5]])
            send_packet(self, self.devports[4], self.v4_racl_pkt)
            verify_packets(self, self.exp_v4_racl_pkt, [self.devports[5]])
            #send_packet(self, self.devports[4], self.dtel_acl_pkt)
            #verify_packets(self, self.exp_dtel_acl_pkt, [self.devports[5]])
        finally:
            self.attribute_set(self.port4, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            pass

    def MacAclConfigure(self, acl_group_list):
        if (self.client.is_feature_enable(SWITCH_FEATURE_INGRESS_MAC_ACL) == 0):
            return

        # Create ACL tables and entries
        acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_MAC,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_entry = self.add_acl_entry(self.device,
            src_mac='00:22:22:22:22:25',
            src_mac_mask='ff:ff:ff:ff:ff:ff',
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry)

        # Create ACL group member
        for acl_group in acl_group_list:
            acl_grp_member = self.add_acl_group_member(self.device,
                acl_table_handle=acl_table,
                acl_group_handle=acl_group)

    def IPv4AclConfigure(self, acl_group_list):
        table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IPV4
        if self.client.is_feature_enable(SWITCH_FEATURE_SHARED_INGRESS_IP_ACL):
            table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IP

        acl_table = self.add_acl_table(self.device,
            type=table_type,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_entry = self.add_acl_entry(self.device,
            dst_ip='10.10.10.2',
            dst_ip_mask='255.255.255.255',
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry)

        # Create ACL group member
        for acl_group in acl_group_list:
            acl_grp_member = self.add_acl_group_member(self.device,
                acl_table_handle=acl_table,
                acl_group_handle=acl_group)

    def IPv6AclConfigure(self, acl_group_list):
        table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IPV6
        if self.client.is_feature_enable(SWITCH_FEATURE_SHARED_INGRESS_IP_ACL):
            table_type = SWITCH_ACL_TABLE_ATTR_TYPE_IP

        acl_table = self.add_acl_table(self.device,
            type=table_type,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_entry = self.add_acl_entry(self.device,
            dst_ip='1234:5678:9abc:def0:4422:1133:5577:99aa',
            dst_ip_mask='FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF',
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry)

        # Create ACL group member
        for acl_group in acl_group_list:
            acl_grp_member = self.add_acl_group_member(self.device,
                acl_table_handle=acl_table,
                acl_group_handle=acl_group)

    def IPv4RaclConfigure(self, acl_group_list):
        if (self.client.is_feature_enable(SWITCH_FEATURE_INGRESS_IPV4_RACL) == 0):
            return
        acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IPV4_RACL,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_entry = self.add_acl_entry(self.device,
            dst_ip='10.10.10.3',
            dst_ip_mask='255.255.255.255',
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry)

        # Create ACL group member
        for acl_group in acl_group_list:
            acl_grp_member = self.add_acl_group_member(self.device,
                acl_table_handle=acl_table,
                acl_group_handle=acl_group)

    def IPv6RaclConfigure(self, acl_group_list):
        if (self.client.is_feature_enable(SWITCH_FEATURE_INGRESS_IPV6_RACL) == 0):
            return
        acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IPV6_RACL,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_entry = self.add_acl_entry(self.device,
            dst_ip='1234:5678:9abc:def0:4422:1133:5577:99bb',
            dst_ip_mask='FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF',
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry)

        # Create ACL group member
        for acl_group in acl_group_list:
            acl_grp_member = self.add_acl_group_member(self.device,
                acl_table_handle=acl_table,
                acl_group_handle=acl_group)

    def MirrorAclConfigure(self, acl_group_list):
        if (self.client.is_feature_enable(SWITCH_FEATURE_INGRESS_MIRROR_ACL) == 0):
            return
        mirror = self.add_mirror(self.device,
            type=SWITCH_MIRROR_ATTR_TYPE_LOCAL,
            direction=SWITCH_MIRROR_ATTR_DIRECTION_INGRESS,
            egress_port_handle=self.port3)
        acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IP_MIRROR,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_entry = self.add_acl_entry(self.device,
            dst_ip='10.10.10.4',
            dst_ip_mask='255.255.255.255',
            action_ingress_mirror_handle=mirror,
            table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry)

        # Create ACL group member
        for acl_group in acl_group_list:
            acl_grp_member = self.add_acl_group_member(self.device,
                acl_table_handle=acl_table,
                acl_group_handle=acl_group)

    def IPQosAclConfigure(self, acl_group_list):
        if (self.client.is_feature_enable(SWITCH_FEATURE_INGRESS_IP_QOS_ACL) == 0):
            return

        qos_map1 = self.add_qos_map(self.device, pcp=1, tc=20)
        qos_map2 = self.add_qos_map(self.device, pcp=2, tc=24)
        pcp_tc_maps = []
        pcp_tc_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=qos_map1))
        pcp_tc_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=qos_map2))
        self.pcp_tc_map_ingress = self.add_qos_map_ingress(self.device,
            type=SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_PCP_TO_TC, qos_map_list=pcp_tc_maps)

        qos_map5 = self.add_qos_map(self.device, tc=20, pcp=1)
        qos_map6 = self.add_qos_map(self.device, tc=24, pcp=2)
        tc_pcp_maps = []
        tc_pcp_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=qos_map5))
        tc_pcp_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=qos_map6))
        self.tc_pcp_map_egress = self.add_qos_map_egress(self.device,
            type=SWITCH_QOS_MAP_EGRESS_ATTR_TYPE_TC_TO_PCP, qos_map_list=tc_pcp_maps)
        acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IP_QOS,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_entry = self.add_acl_entry(self.device,
            dst_ip='10.10.10.5',
            dst_ip_mask='255.255.255.255',
            action_set_tc=24,
            table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry)

        # Create ACL group member
        for acl_group in acl_group_list:
            acl_grp_member = self.add_acl_group_member(self.device,
                acl_table_handle=acl_table,
                acl_group_handle=acl_group)

        self.attribute_set(self.port5, SWITCH_PORT_ATTR_EGRESS_L2_QOS_GROUP, self.tc_pcp_map_egress)

    def DtelConfigure(self, acl_group_list):
        if (self.client.is_feature_enable(SWITCH_FEATURE_FLOW_REPORT) == 0):
            return

        self.dtel_params = SwitchConfig_Params()
        self.dtel_params.switch_id = 0x11111111
        self.dtel_params.mac_self = '00:77:66:55:44:33'
        self.dtel_params.nports = 2
        self.dtel_params.ipaddr_inf = ['172.16.0.1']
        self.dtel_params.ipaddr_nbr = ['172.16.0.11']
        self.dtel_params.mac_nbr = ['00:11:22:33:44:55']
        self.dtel_params.report_ports = [0]
        self.dtel_params.ipaddr_report_src = ['4.4.4.1']
        self.dtel_params.ipaddr_report_dst = ['4.4.4.3']
        self.dtel_params.device = 0
        self.dtel_params.swports = [4, 5]

        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_SWITCH_ID, self.dtel_params.switch_id)

        # enable flow report
        self.dtel = self.add_dtel(self.device, flow_report=True)

        # add report session information
        report_dst_ip_list = []
        for ip in self.dtel_params.ipaddr_report_dst:
            report_dst_ip_list.append(switcht_list_val_t(type=switcht_value_type.IP_ADDRESS, ip_addr=ip))

        self.report_session = self.add_report_session(
            self.device, udp_dst_port=UDP_PORT_DTEL_REPORT,
            src_ip=self.dtel_params.ipaddr_report_src[0], vrf_handle=self.vrf10,
            dst_ip_list=report_dst_ip_list, truncate_size=512, ttl=64)

        #workaround for model bug that reports high latency values
        if test_param_get('target') == "asic-model":
            self.attribute_set(
                self.device, SWITCH_DEVICE_ATTR_LATENCY_SENSITIVITY, 30)

        self.fp = []
        mult = 4
        if self.arch == 'tofino2':
            mult = 8
        for port in self.port_list:
            fport = self.attribute_get(port, SWITCH_PORT_ATTR_CONNECTOR_ID) * mult
            fport = fport + self.attribute_get(port, SWITCH_PORT_ATTR_CHANNEL_ID)
            self.fp.append(fport)

        # configure collector port
        rif1 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT,
            port_handle=self.port0, vrf_handle=self.vrf10, src_mac=self.rmac)
        nhop1 = self.add_nexthop(self.device, handle=rif1, dest_ip='20.20.20.1')
        neighbor1 = self.add_neighbor(self.device, mac_address=self.dtel_params.mac_nbr[self.dtel_params.report_ports[0]], handle=rif1, dest_ip='20.20.20.1')
        route1 = self.add_route(self.device, ip_prefix='4.4.4.0/24', vrf_handle=self.vrf10, nexthop_handle=nhop1)

        acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_DTEL,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_entry = self.add_acl_entry(self.device,
            dtel_action_type=SWITCH_ACL_ENTRY_ATTR_DTEL_ACTION_TYPE_DTEL_REPORT,
            dst_ip='10.10.10.6',
            dst_ip_mask='255.255.255.255',
            action_report_type=SWITCH_DTEL_REPORT_TYPE_FLOW,
            action_report_all_packets=True,
            table_handle=acl_table)
        self.client.object_counters_clear_all(acl_entry)

        # Create ACL group member
        for acl_group in acl_group_list:
            acl_grp_member = self.add_acl_group_member(self.device,
                acl_table_handle=acl_table,
                acl_group_handle=acl_group)

    def TestMacAclTable(self):
        print("TestMacAclTable()")
        if (self.client.is_feature_enable(SWITCH_FEATURE_INGRESS_MAC_ACL) == 0):
            print("MAC ACL feature not enabled, skipping")
            return
        try:
            send_packet(self, self.devports[4], self.mac_acl_pkt)
            verify_no_other_packets(self, timeout=2)
        finally:
            pass

    def TestIPv4AclTable(self):
        print("TestIPv4AclTable()")
        try:
            send_packet(self, self.devports[4], self.v4_acl_pkt)
            verify_no_other_packets(self, timeout=2)
        finally:
            pass

    def TestIPv6AclTable(self):
        print("TestIPv6AclTable()")
        try:
            send_packet(self, self.devports[4], self.v6_acl_pkt)
            verify_no_other_packets(self, timeout=2)
        finally:
            pass

    def TestIPv4RaclTable(self):
        print("TestIPv4RaclTable()")
        if (self.client.is_feature_enable(SWITCH_FEATURE_INGRESS_IPV4_RACL) == 0):
            print("IPV4 RACL feature not enabled, skipping")
            return
        try:
            send_packet(self, self.devports[4], self.v4_racl_pkt)
            verify_no_other_packets(self, timeout=2)
        finally:
            pass

    def TestIPv6RaclTable(self):
        print("TestIPv6RaclTable()")
        if (self.client.is_feature_enable(SWITCH_FEATURE_INGRESS_IPV6_RACL) == 0):
            print("IPV6 RACL feature not enabled, skipping")
            return
        try:
            send_packet(self, self.devports[4], self.v6_racl_pkt)
            verify_no_other_packets(self, timeout=2)
        finally:
            pass

    def TestMirrorAclTable(self):
        print("TestMirrorAclTable()")
        if (self.client.is_feature_enable(SWITCH_FEATURE_INGRESS_MIRROR_ACL) == 0):
            print("MirrorAcl feature not enabled, skipping")
            return
        try:
            send_packet(self, self.devports[4], self.mirror_acl_pkt)
            verify_each_packet_on_each_port(self, [self.exp_mirror_acl_pkt, self.exp_mirror_acl_pkt],
                                [self.devports[3], self.devports[5]])
        finally:
            pass

    def TestIPQosAclTable(self):
        print("TestIPQosAclTable()")
        if (self.client.is_feature_enable(SWITCH_FEATURE_INGRESS_IP_QOS_ACL) == 0):
            print("IPQosAcl feature not enabled, skipping")
            return
        try:
            exp_pkt = self.qos_acl_pkt.copy()
            exp_pkt[Dot1Q].prio = 2

            send_packet(self, self.devports[4], self.qos_acl_pkt)
            verify_packets(self, exp_pkt, [self.devports[5]])
        finally:
            self.attribute_set(self.port5, SWITCH_PORT_ATTR_EGRESS_L2_QOS_GROUP, 0)

    def TestDtelAclTable(self):
        print("TestDtelAclTable()")
        if (self.client.is_feature_enable(SWITCH_FEATURE_FLOW_REPORT) == 0):
            print("Dtel feature not enabled, skipping")
            return
        try:
            if (self.client.is_feature_enable(SWITCH_FEATURE_INT_V2) == 0):
                self.int_v2 = False
            else:
                self.int_v2 = True

            bind_postcard_pkt(self.int_v2)

            if self.int_v2:
                exp_e2e_pkt = ipv4_dtel_v2_pkt(
                    eth_dst=self.dtel_params.mac_nbr[self.dtel_params.report_ports[0]],
                    eth_src=self.dtel_params.mac_self,
                    ip_src=self.dtel_params.ipaddr_report_src[0],
                    ip_dst=self.dtel_params.ipaddr_report_dst[0],
                    ip_id=0,
                    ip_ttl=63,
                    next_proto=DTEL_REPORT_NEXT_PROTO_SWITCH_LOCAL,
                    dropped=0,
                    congested_queue=0,
                    path_tracking_flow=1,
                    hw_id=1,
                    switch_id=self.dtel_params.switch_id,
                    ingress_port=self.fp[4],
                    egress_port=self.fp[5],
                    queue_id=0,
                    queue_depth=0,
                    ingress_tstamp=0,
                    egress_tstamp=0,
                    inner_frame=self.dtel_acl_pkt)

            else:
                exp_pc_inner = postcard_report(
                    packet=self.dtel_acl_pkt,
                    switch_id=self.dtel_params.switch_id,
                    ingress_port=self.fp[4],
                    egress_port=self.fp[5],
                    queue_id=0,
                    queue_depth=0,
                    egress_tstamp=0)

                exp_e2e_pkt = ipv4_dtel_pkt(
                    eth_dst=self.dtel_params.mac_nbr[self.dtel_params.report_ports[0]],
                    eth_src=self.dtel_params.mac_self,
                    ip_src=self.dtel_params.ipaddr_report_src[0],
                    ip_dst=self.dtel_params.ipaddr_report_dst[0],
                    ip_id=0,
                    ip_ttl=63,
                    next_proto=DTEL_REPORT_NEXT_PROTO_SWITCH_LOCAL,
                    dropped=0,
                    congested_queue=0,
                    path_tracking_flow=1,
                    hw_id=1,
                    inner_frame=exp_pc_inner)

            send_packet(self, self.devports[4], self.dtel_acl_pkt)
            verify_packet(self, self.exp_dtel_acl_pkt, self.devports[5])
            verify_postcard_packet(self, self.int_v2, exp_e2e_pkt, self.devports[0])
        finally:
            pass

###############################################################################

@group('acl')
class AclLagBindingTest(L2LagTest):

    def runTest(self):
        self.LagIngressAclTest()
        self.LagEgressAclTest()

    def LagIngressAclTest(self):
        print("LagIngressAclTest()")
        print("Test LAG members ACL test")
        if(self.client.is_feature_enable(SWITCH_FEATURE_ACL_PORT_GROUP) == 1):
            print("LAG ingress acl is not supported with ACL portgroup")
            return
        mac2 = self.add_mac_entry(self.device, vlan_handle=self.vlan20, mac_address='00:77:77:77:77:77', destination_handle=self.port9)
        mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan20, mac_address='00:88:88:88:88:88', destination_handle=self.lag3)
        vlan_mbr10 = self.add_vlan_member(self.device, vlan_handle=self.vlan20, member_handle=self.lag3)
        self.attribute_set(self.lag3, SWITCH_LAG_ATTR_PORT_VLAN_ID, 20)
        self.add_lag_member(self.device, lag_handle=self.lag3, port_handle=self.port11)

        if(self.client.is_feature_enable(SWITCH_FEATURE_INGRESS_IPV4_ACL) == 1):
            acl_table = self.add_acl_table(self.device,
                                           type=SWITCH_ACL_TABLE_ATTR_TYPE_IPV4,
                                           bind_point_type=[acl_table_bp_port],
                                           direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        else:
            acl_table = self.add_acl_table(self.device,
                                           type=SWITCH_ACL_TABLE_ATTR_TYPE_IP,
                                           bind_point_type=[acl_table_bp_port],
                                           direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_entry = self.add_acl_entry(self.device,
            dst_ip='10.0.0.1',
            dst_ip_mask='255.255.255.255',
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            table_handle=acl_table)
        pre_counter = self.client.object_counters_get(acl_entry)

        try:
            pkt = simple_tcp_packet(
                eth_dst='00:77:77:77:77:77',
                eth_src='00:88:88:88:88:88',
                ip_dst='10.0.0.1',
                ip_ttl=64,
                pktlen=100)
            exp_pkt = simple_tcp_packet(
                eth_dst='00:77:77:77:77:77',
                eth_src='00:88:88:88:88:88',
                ip_dst='10.0.0.1',
                dl_vlan_enable=True,
                vlan_vid=20,
                ip_ttl=64,
                pktlen=104)
            print("Allow packet before binding ingress ACL to lag")
            send_packet(self, self.devports[11], pkt)
            verify_packets(self, exp_pkt, [self.devports[9]])

            print("Bind ACL to lag")
            self.attribute_set(self.lag3, SWITCH_LAG_ATTR_INGRESS_ACL_HANDLE, acl_table)
            send_packet(self, self.devports[11], pkt)
            verify_no_other_packets(self, timeout=2)
            print("Add a 2nd member. Should drop on this member also")
            lag_mbr1 = self.add_lag_member(self.device, lag_handle=self.lag3, port_handle=self.port12)
            send_packet(self, self.devports[12], pkt)
            verify_no_other_packets(self, timeout=2)
            print("Sleeping for 2 sec before fetching stats")
            time.sleep(2)
            post_counter = self.client.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 2)

            print("Remove binding, packet should be forwarded")
            self.attribute_set(self.lag3, SWITCH_LAG_ATTR_INGRESS_ACL_HANDLE, 0)
            send_packet(self, self.devports[11], pkt)
            verify_packets(self, exp_pkt, [self.devports[9]])
            send_packet(self, self.devports[12], pkt)
            verify_packets(self, exp_pkt, [self.devports[9]])
        finally:
            self.attribute_set(self.lag3, SWITCH_LAG_ATTR_INGRESS_ACL_HANDLE, 0)
            # remove the acl entry and table
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.attribute_set(self.lag3, SWITCH_LAG_ATTR_PORT_VLAN_ID, 0)
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

    def LagEgressAclTest(self):
        print("LagEgressAclTest()")
        print("Test LAG members ACL test")
        if (self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_IP_ACL) == 0):
            print("EgressIPv4Acl  feature not enabled, skipping")
            return
        mac2 = self.add_mac_entry(self.device, vlan_handle=self.vlan20, mac_address='00:77:77:77:77:77', destination_handle=self.port9)
        mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan20, mac_address='00:88:88:88:88:88', destination_handle=self.lag3)
        vlan_mbr10 = self.add_vlan_member(self.device, vlan_handle=self.vlan20, member_handle=self.lag3)
        self.attribute_set(self.lag3, SWITCH_LAG_ATTR_PORT_VLAN_ID, 20)

        acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IPV4,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_EGRESS)
        acl_entry = self.add_acl_entry(self.device,
            dst_ip='10.0.0.1',
            dst_ip_mask='255.255.255.255',
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            table_handle=acl_table)
        pre_counter = self.client.object_counters_get(acl_entry)

        self.add_lag_member(self.device, lag_handle=self.lag3, port_handle=self.port11)

        try:
            pkt = simple_tcp_packet(
                eth_src='00:77:77:77:77:77',
                eth_dst='00:88:88:88:88:88',
                ip_dst='10.0.0.1',
                dl_vlan_enable=True,
                vlan_vid=20,
                ip_ttl=64,
                pktlen=104)
            exp_pkt = simple_tcp_packet(
                eth_src='00:77:77:77:77:77',
                eth_dst='00:88:88:88:88:88',
                ip_dst='10.0.0.1',
                ip_ttl=64,
                pktlen=100)
            print("Allow packet before binding egress ACL to lag")
            send_packet(self, self.devports[9], pkt)
            verify_packets(self, exp_pkt, [self.devports[11]])

            print("Bind ACL to lag")
            self.attribute_set(self.lag3, SWITCH_LAG_ATTR_EGRESS_ACL_HANDLE, acl_table)
            send_packet(self, self.devports[9], pkt)
            verify_no_other_packets(self, timeout=2)
            print("Add a 2nd member. Should drop on this member also")
            self.cleanlast()
            lag_mbr1 = self.add_lag_member(self.device, lag_handle=self.lag3, port_handle=self.port12)
            send_packet(self, self.devports[9], pkt)
            verify_no_other_packets(self, timeout=2)
            print("Sleeping for 2 sec before fetching stats")
            time.sleep(2)
            post_counter = self.client.object_counters_get(acl_entry)
            self.assertEqual(post_counter[0].count - pre_counter[0].count, 2)

            print("Remove binding, packet should be forwarded")
            self.attribute_set(self.lag3, SWITCH_LAG_ATTR_EGRESS_ACL_HANDLE, 0)
            send_packet(self, self.devports[9], pkt)
            verify_packets(self, exp_pkt, [self.devports[12]])
        finally:
            self.attribute_set(self.lag2, SWITCH_LAG_ATTR_EGRESS_ACL_HANDLE, 0)
            # remove the acl entry and table
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.attribute_set(self.lag3, SWITCH_LAG_ATTR_PORT_VLAN_ID, 0)
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

###############################################################################

@group('acl')
class ACLFieldComplexityTest(ApiHelper):
    '''
    TODO - routed cases for v4/v6
    IPV4
      - Routed
      - Bridged
    IPV6
      - Routed
      - Bridged
    NON_IP
      ARP
       - Broadcast
       - Unicast
      LACP
    '''

    def label_supp_for_racl_mac_qos_acl(self):
        if ((self.client.is_feature_enable(SWITCH_FEATURE_INGRESS_MAC_ACL)) or
            (self.client.is_feature_enable(SWITCH_FEATURE_INGRESS_IP_QOS_ACL)) or
            (self.client.is_feature_enable(SWITCH_FEATURE_INGRESS_IPV4_RACL)) or
            (self.client.is_feature_enable(SWITCH_FEATURE_INGRESS_IPV6_RACL)) or
            (self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_MAC_ACL))):
            return True
        else:
            return False

    # unlike above tests, this test tries complex acl entries to test the pipemgr
    # hitless configuration
    def setUp(self):
        print()
        self.configure()
        rule_begin = 0
        # skip ethertype tests if etype not an ACL match key
        # the single rule that get's added acts as a catch all
        if (self.client.is_feature_enable(SWITCH_FEATURE_ETYPE_IN_ACL) == 0):
            rule_begin = 1

        vlan_member1 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port0)
        vlan_member2 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port1)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:44:44:44:44:44', destination_handle=self.port0)
        self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:55:55:55:55:55', destination_handle=self.port1)
        self.rif0 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_VLAN, vlan_handle=self.vlan10, vrf_handle=self.vrf10, src_mac=self.rmac)

        fwd = SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_PERMIT
        deny = SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP
        if (self.client.is_feature_enable(SWITCH_FEATURE_ETYPE_IN_ACL)):
            v4_rules = [
                ["{'eth_type': 2054, 'eth_type_mask': 0x7FFF, 'priority': 100}", deny],  # arp
                ["{'eth_type': 2057, 'eth_type_mask': 0x7FFF, 'priority': 100}", deny],  # lacp
                ["{'eth_type': 2048, 'eth_type_mask': 0x7FFF, 'priority': 100}", deny],  # catch all
                ["{'dst_ip': '200.1.1.2', 'dst_ip_mask': '255.255.255.0', 'src_ip': '100.1.1.2', 'src_ip_mask': '255.255.255.0', 'priority': 1, 'tcp_flags': 2, 'tcp_flags_mask': 14}", fwd],
                ["{'dst_ip': '200.1.1.2', 'dst_ip_mask': '255.255.255.0', 'src_ip': '100.1.1.2', 'src_ip_mask': '255.255.255.0', 'priority': 1, 'tcp_flags': 4, 'tcp_flags_mask': 14}", deny],
                ["{'dst_ip': '200.1.2.2', 'dst_ip_mask': '255.255.255.0', 'src_ip': '100.1.2.2', 'src_ip_mask': '255.255.255.0', 'ip_proto': 1, 'ip_proto_mask': 127, 'priority': 2}", fwd],
                ["{'dst_ip': '200.1.3.2', 'dst_ip_mask': '255.255.255.0', 'src_ip': '100.1.3.2', 'src_ip_mask': '255.255.255.0', 'ip_proto': 6, 'ip_proto_mask': 127, 'tcp_flags': 6, 'tcp_flags_mask': 14, 'l4_dst_port': 20000, 'l4_dst_port_mask': 32759, 'l4_src_port': 10000, 'l4_src_port_mask': 32759, 'priority': 3}", fwd],
                ["{'dst_ip': '200.1.3.2', 'dst_ip_mask': '255.255.255.0', 'src_ip': '100.1.3.2', 'src_ip_mask': '255.255.255.0', 'ip_proto': 6, 'ip_proto_mask': 127, 'tcp_flags': 6, 'tcp_flags_mask': 14, 'l4_dst_port': 10000, 'l4_dst_port_mask': 32759, 'l4_src_port': 20000, 'l4_src_port_mask': 32759, 'priority': 3}", deny],
                ["{'dst_ip': '200.1.4.2', 'dst_ip_mask': '255.255.255.0', 'src_ip': '100.1.4.2', 'src_ip_mask': '255.255.255.0', 'l4_src_port': 12345, 'l4_src_port_mask': 32759, 'priority': 4}", fwd],
                ["{'dst_ip': '200.1.5.2', 'dst_ip_mask': '255.255.255.0', 'src_ip': '100.1.5.2', 'src_ip_mask': '255.255.255.0', 'l4_dst_port': 12345, 'l4_dst_port_mask': 32759, 'priority': 5}", fwd],
                ["{'dst_ip': '200.1.5.2', 'dst_ip_mask': '255.255.255.0', 'src_ip': '100.1.5.2', 'src_ip_mask': '255.255.255.0', 'priority': 99}", deny],
                ["{'dst_ip': '200.1.11.2', 'dst_ip_mask': '255.255.255.0', 'src_ip': '100.1.11.2', 'src_ip_mask': '255.255.255.0', 'priority': 11}", deny],
                ["{'dst_ip': '200.1.12.2', 'dst_ip_mask': '255.255.255.0', 'src_ip': '100.1.12.2', 'src_ip_mask': '255.255.255.0', 'ip_proto': 1, 'ip_proto_mask': 127, 'priority': 12}", deny],
                ["{'dst_ip': '200.1.13.2', 'dst_ip_mask': '255.255.255.0', 'src_ip': '100.1.13.2', 'src_ip_mask': '255.255.255.0', 'ip_proto': 6, 'ip_proto_mask': 127, 'tcp_flags': 6, 'tcp_flags_mask': 14, 'l4_dst_port': 20000, 'l4_dst_port_mask': 32759, 'l4_src_port': 10000, 'l4_src_port_mask': 32759, 'priority': 13}", deny],
                ["{'dst_ip': '200.1.14.2', 'dst_ip_mask': '255.255.255.0', 'src_ip': '100.1.14.2', 'src_ip_mask': '255.255.255.0', 'l4_src_port': 12345, 'l4_src_port_mask': 32759, 'priority': 14}", deny],
                ["{'dst_ip': '200.1.15.2', 'dst_ip_mask': '255.255.255.0', 'src_ip': '100.1.15.2', 'src_ip_mask': '255.255.255.0', 'l4_dst_port': 12345, 'l4_dst_port_mask': 32759, 'priority': 15}", deny]
            ]
        else:
            v4_rules = [
                ["{'eth_type': 2054, 'eth_type_mask': 0x7FFF, 'priority': 100}", deny],  # arp
                ["{'eth_type': 2048, 'eth_type_mask': 0x7FFF, 'priority': 100}", deny],  # catch all
                ["{'dst_ip': '200.1.1.2', 'dst_ip_mask': '255.255.255.0', 'src_ip': '100.1.1.2', 'src_ip_mask': '255.255.255.0', 'priority': 1, 'tcp_flags': 2, 'tcp_flags_mask': 14}", fwd],
                ["{'dst_ip': '200.1.1.2', 'dst_ip_mask': '255.255.255.0', 'src_ip': '100.1.1.2', 'src_ip_mask': '255.255.255.0', 'priority': 1, 'tcp_flags': 4, 'tcp_flags_mask': 14}", deny],
                ["{'dst_ip': '200.1.2.2', 'dst_ip_mask': '255.255.255.0', 'src_ip': '100.1.2.2', 'src_ip_mask': '255.255.255.0', 'ip_proto': 1, 'ip_proto_mask': 127, 'priority': 2}", fwd],
                ["{'dst_ip': '200.1.3.2', 'dst_ip_mask': '255.255.255.0', 'src_ip': '100.1.3.2', 'src_ip_mask': '255.255.255.0', 'ip_proto': 6, 'ip_proto_mask': 127, 'tcp_flags': 6, 'tcp_flags_mask': 14, 'l4_dst_port': 20000, 'l4_dst_port_mask': 32759, 'l4_src_port': 10000, 'l4_src_port_mask': 32759, 'priority': 3}", fwd],
                ["{'dst_ip': '200.1.3.2', 'dst_ip_mask': '255.255.255.0', 'src_ip': '100.1.3.2', 'src_ip_mask': '255.255.255.0', 'ip_proto': 6, 'ip_proto_mask': 127, 'tcp_flags': 6, 'tcp_flags_mask': 14, 'l4_dst_port': 10000, 'l4_dst_port_mask': 32759, 'l4_src_port': 20000, 'l4_src_port_mask': 32759, 'priority': 3}", deny],
                ["{'dst_ip': '200.1.4.2', 'dst_ip_mask': '255.255.255.0', 'src_ip': '100.1.4.2', 'src_ip_mask': '255.255.255.0', 'l4_src_port': 12345, 'l4_src_port_mask': 32759, 'priority': 4}", fwd],
                ["{'dst_ip': '200.1.5.2', 'dst_ip_mask': '255.255.255.0', 'src_ip': '100.1.5.2', 'src_ip_mask': '255.255.255.0', 'l4_dst_port': 12345, 'l4_dst_port_mask': 32759, 'priority': 5}", fwd],
                ["{'dst_ip': '200.1.5.2', 'dst_ip_mask': '255.255.255.0', 'src_ip': '100.1.5.2', 'src_ip_mask': '255.255.255.0', 'priority': 99}", deny],
                ["{'dst_ip': '200.1.11.2', 'dst_ip_mask': '255.255.255.0', 'src_ip': '100.1.11.2', 'src_ip_mask': '255.255.255.0', 'priority': 11}", deny],
                ["{'dst_ip': '200.1.12.2', 'dst_ip_mask': '255.255.255.0', 'src_ip': '100.1.12.2', 'src_ip_mask': '255.255.255.0', 'ip_proto': 1, 'ip_proto_mask': 127, 'priority': 12}", deny],
                ["{'dst_ip': '200.1.13.2', 'dst_ip_mask': '255.255.255.0', 'src_ip': '100.1.13.2', 'src_ip_mask': '255.255.255.0', 'ip_proto': 6, 'ip_proto_mask': 127, 'tcp_flags': 6, 'tcp_flags_mask': 14, 'l4_dst_port': 20000, 'l4_dst_port_mask': 32759, 'l4_src_port': 10000, 'l4_src_port_mask': 32759, 'priority': 13}", deny],
                ["{'dst_ip': '200.1.14.2', 'dst_ip_mask': '255.255.255.0', 'src_ip': '100.1.14.2', 'src_ip_mask': '255.255.255.0', 'l4_src_port': 12345, 'l4_src_port_mask': 32759, 'priority': 14}", deny],
                ["{'dst_ip': '200.1.15.2', 'dst_ip_mask': '255.255.255.0', 'src_ip': '100.1.15.2', 'src_ip_mask': '255.255.255.0', 'l4_dst_port': 12345, 'l4_dst_port_mask': 32759, 'priority': 15}", deny]
            ]

        v6_rules = [
            ["{'eth_type': 2054, 'eth_type_mask': 0x7FFF, 'priority': 100}", deny],  # arp
            ["{'eth_type': 2057, 'eth_type_mask': 0x7FFF, 'priority': 100}", deny],  # lacp
            ["{'eth_type': 1757, 'eth_type_mask': 0xFFF, 'priority': 100}", deny],   # catch all
            ["{'dst_ip': '1200::1:0:0:1:2', 'dst_ip_mask': 'FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:0', 'src_ip': '1100::1:0:0:1:2', 'src_ip_mask': 'FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:0', 'priority': 1, 'tcp_flags': 2, 'tcp_flags_mask': 14}", fwd],
            ["{'dst_ip': '1200::1:0:0:1:2', 'dst_ip_mask': 'FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:0', 'src_ip': '1100::1:0:0:1:2', 'src_ip_mask': 'FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:0', 'priority': 1, 'tcp_flags': 4, 'tcp_flags_mask': 14}", deny],
            ["{'dst_ip': '1200::1:0:0:2:2', 'dst_ip_mask': 'FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:0', 'src_ip': '1100::1:0:0:2:2', 'src_ip_mask': 'FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:0', 'ip_proto': 58, 'ip_proto_mask': 127, 'priority': 2}", fwd],
            ["{'dst_ip': '1200::1:0:0:3:2', 'dst_ip_mask': 'FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:0', 'src_ip': '1100::1:0:0:3:2', 'src_ip_mask': 'FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:0', 'ip_proto': 6, 'ip_proto_mask': 127, 'tcp_flags': 6, 'tcp_flags_mask': 14, 'l4_dst_port': 20000, 'l4_dst_port_mask': 32759, 'l4_src_port': 10000, 'l4_src_port_mask': 32759, 'priority': 3}", fwd],
            ["{'dst_ip': '1200::1:0:0:3:2', 'dst_ip_mask': 'FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:0', 'src_ip': '1100::1:0:0:3:2', 'src_ip_mask': 'FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:0', 'ip_proto': 6, 'ip_proto_mask': 127, 'tcp_flags': 6, 'tcp_flags_mask': 14, 'l4_dst_port': 10000, 'l4_dst_port_mask': 32759, 'l4_src_port': 20000, 'l4_src_port_mask': 32759, 'priority': 3}", deny],
            ["{'dst_ip': '1200::1:0:0:4:2', 'dst_ip_mask': 'FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:0', 'src_ip': '1100::1:0:0:4:2', 'src_ip_mask': 'FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:0', 'l4_src_port': 12345, 'l4_src_port_mask': 32759, 'priority': 4}", fwd],
            ["{'dst_ip': '1200::1:0:0:5:2', 'dst_ip_mask': 'FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:0', 'src_ip': '1100::1:0:0:5:2', 'src_ip_mask': 'FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:0', 'l4_dst_port': 12345, 'l4_dst_port_mask': 32759, 'priority': 5}", fwd],
            ["{'dst_ip': '1200::1:0:0:5:2', 'dst_ip_mask': 'FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:0', 'src_ip': '1100::1:0:0:5:2', 'src_ip_mask': 'FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:0', 'priority': 99}", deny],
            ["{'dst_ip': '1200::1:0:0:11:2', 'dst_ip_mask': 'FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:0', 'src_ip': '1100::1:0:0:11:2', 'src_ip_mask': 'FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:0', 'priority': 11}", deny],
            ["{'dst_ip': '1200::1:0:0:12:2', 'dst_ip_mask': 'FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:0', 'src_ip': '1100::1:0:0:12:2', 'src_ip_mask': 'FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:0', 'ip_proto': 58, 'ip_proto_mask': 127, 'priority': 12}", deny],
            ["{'dst_ip': '1200::1:0:0:13:2', 'dst_ip_mask': 'FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:0', 'src_ip': '1100::1:0:0:13:2', 'src_ip_mask': 'FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:0', 'ip_proto': 6, 'ip_proto_mask': 127, 'tcp_flags': 6, 'tcp_flags_mask': 14, 'l4_dst_port': 20000, 'l4_dst_port_mask': 32759, 'l4_src_port': 10000, 'l4_src_port_mask': 32759, 'priority': 13}", deny],
            ["{'dst_ip': '1200::1:0:0:14:2', 'dst_ip_mask': 'FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:0', 'src_ip': '1100::1:0:0:14:2', 'src_ip_mask': 'FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:0', 'l4_src_port': 12345, 'l4_src_port_mask': 32759, 'priority': 14}", deny],
            ["{'dst_ip': '1200::1:0:0:15:2', 'dst_ip_mask': 'FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:0', 'src_ip': '1100::1:0:0:15:2', 'src_ip_mask': 'FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:0', 'l4_dst_port': 12345, 'l4_dst_port_mask': 32759, 'priority': 15}", deny]
        ]
        self.acl_group = self.add_acl_group(self.device, bind_point_type=[acl_group_bp_port])
        self.acl_v4_group = self.add_acl_group(self.device, bind_point_type=[acl_group_bp_port])
        self.acl_v6_group = self.add_acl_group(self.device, bind_point_type=[acl_group_bp_port])

        if self.client.is_feature_enable(SWITCH_FEATURE_SHARED_INGRESS_IP_ACL):
            table = self.add_acl_table(self.device,
              type=SWITCH_ACL_TABLE_ATTR_TYPE_IP,
              bind_point_type=[acl_table_bp_port],
              direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
            for rule in v4_rules[rule_begin:]:
                acl_entry = self.add_acl_entry(self.device,
                    packet_action=rule[1],
                    table_handle=table,
                    **literal_eval(rule[0]))
            for rule in v6_rules[rule_begin:]:
                acl_entry = self.add_acl_entry(self.device,
                    packet_action=rule[1],
                    table_handle=table,
                    **literal_eval(rule[0]))
        else:
            v4_table = self.add_acl_table(self.device,
              type=SWITCH_ACL_TABLE_ATTR_TYPE_IPV4,
              bind_point_type=[acl_table_bp_port],
              direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
            v6_table = self.add_acl_table(self.device,
              type=SWITCH_ACL_TABLE_ATTR_TYPE_IPV6,
              bind_point_type=[acl_table_bp_port],
              direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
            for rule in v4_rules[rule_begin:]:
                acl_entry = self.add_acl_entry(self.device,
                    packet_action=rule[1],
                    table_handle=v4_table,
                    **literal_eval(rule[0]))
            for rule in v6_rules[rule_begin:]:
                acl_entry = self.add_acl_entry(self.device,
                    packet_action=rule[1],
                    table_handle=v6_table,
                    **literal_eval(rule[0]))
        # if m1 profile, then acl_group can be shared
        if self.label_supp_for_racl_mac_qos_acl():
            print("Using non-shared groups")
            self.add_acl_group_member(self.device, acl_table_handle=v4_table, acl_group_handle=self.acl_v4_group)
            self.add_acl_group_member(self.device, acl_table_handle=v6_table, acl_group_handle=self.acl_v6_group)
        else:
            print("Using shared group")
            if self.client.is_feature_enable(SWITCH_FEATURE_SHARED_INGRESS_IP_ACL):
                self.add_acl_group_member(self.device, acl_table_handle=table, acl_group_handle=self.acl_group)
            else:
                self.add_acl_group_member(self.device, acl_table_handle=v4_table, acl_group_handle=self.acl_group)
                self.add_acl_group_member(self.device, acl_table_handle=v6_table, acl_group_handle=self.acl_group)

    def runTest(self):
        if self.label_supp_for_racl_mac_qos_acl():
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, self.acl_v4_group)
        else:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, self.acl_group)
        pkt_data = [
            [['200.1.1.2', '100.1.1.2', 'S', 3333, 4444, 6], False],
            [['200.1.1.2', '100.1.1.2', 'R', 3333, 4444, 6], True],
            [['200.1.2.2', '100.1.2.2', 'S', 3333, 4444, 1], False],
            [['200.1.3.2', '100.1.3.2', 'RS', 10000, 20000, 6], False],
            [['200.1.3.2', '100.1.3.2', 'RS', 20000, 10000, 6], True],
            [['200.1.4.2', '100.1.4.2', 'S', 12345, 4444, 6], False],
            [['200.1.5.2', '100.1.5.2', 'S', 4444, 12345, 6], False],
            [['200.1.5.2', '100.1.5.2', 'S', 3333, 4444, 6], True],
            [['200.1.11.2', '100.1.11.2', 'S', 3333, 4444, 6], True],
            [['200.1.12.2', '100.1.12.2', 'S', 3333, 4444, 1], True],
            [['200.1.13.2', '100.1.13.2', 'RS', 10000, 20000, 6], True],
            [['200.1.14.2', '100.1.14.2', 'S', 12345, 4444, 6], True],
            [['200.1.15.2', '100.1.15.2', 'S', 4444, 12345, 6], True],
            [['2.1.1.2', '1.1.1.2', 'S', 3333, 4444, 6], True],
        ]
        if (self.client.is_feature_enable(SWITCH_FEATURE_ETYPE_IN_ACL)):
            print('Data: ARP bcast, action: drop')
            pkt = simple_arp_packet(pktlen=pkt_len, arp_op=1)
            send_packet(self, self.devports[0], pkt)
            verify_no_other_packets(self, timeout=1)
            print('Data: ARP unicast request, action: drop')
            pkt = simple_arp_packet(pktlen=pkt_len, arp_op=1, eth_dst=self.rmac)
            send_packet(self, self.devports[0], pkt)
            verify_no_other_packets(self, timeout=1)
            print('Data: ARP unicast response, action: drop')
            pkt = simple_arp_packet(pktlen=pkt_len, arp_op=2, eth_dst=self.rmac)
            send_packet(self, self.devports[0], pkt)
            verify_no_other_packets(self, timeout=1)
        for content in pkt_data:
            data = content[0]
            drop = content[1]
            print(('Data: %s, action: %s') % (data, 'drop' if drop else 'fwd'))
            pkt = simple_tcp_packet(pktlen=pkt_len,
                eth_dst='00:55:55:55:55:55',
                eth_src='00:44:44:44:44:44',
                ip_dst=data[0],
                ip_src=data[1],
                tcp_sport=data[3],
                tcp_dport=data[4],
                tcp_flags=data[2],
                ip_ttl=64)
            pkt['IP'].proto = data[5]
            send_packet(self, self.devports[0], pkt)
            if drop:
                verify_no_other_packets(self, timeout=1)
            else:
                verify_packet(self, pkt, self.devports[1])

        if self.label_supp_for_racl_mac_qos_acl():
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, self.acl_v6_group)
        else:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, self.acl_group)
        pkt_data = [
            [['1200::1:0:0:1:2', '1100::1:0:0:1:2', 'S', 3333, 4444, 6], False],
            [['1200::1:0:0:1:2', '1100::1:0:0:1:2', 'R', 3333, 4444, 6], True],
            [['1200::1:0:0:2:2', '1100::1:0:0:2:2', 'S', 3333, 4444, 58], False],
            [['1200::1:0:0:3:2', '1100::1:0:0:3:2', 'RS', 10000, 20000, 6], False],
            [['1200::1:0:0:3:2', '1100::1:0:0:3:2', 'RS', 20000, 10000, 6], True],
            [['1200::1:0:0:4:2', '1100::1:0:0:4:2', 'S', 12345, 4444, 6], False],
            [['1200::1:0:0:5:2', '1100::1:0:0:5:2', 'S', 4444, 12345, 6], False],
            [['1200::1:0:0:5:2', '1100::1:0:0:5:2', 'S', 3333, 4444, 6], True],
            [['1200::1:0:0:11:2', '1100::1:0:0:11:2', 'S', 3333, 4444, 6], True],
            [['1200::1:0:0:12:2', '1100::1:0:0:12:2', 'S', 3333, 4444, 58], True],
            [['1200::1:0:0:13:2', '1100::1:0:0:13:2', 'RS', 10000, 20000, 6], True],
            [['1200::1:0:0:14:2', '1100::1:0:0:14:2', 'S', 12345, 4444, 6], True],
            [['1200::1:0:0:15:2', '1100::1:0:0:15:2', 'S', 4444, 12345, 6], True],
            [['2000::2', '1000::2', 'S', 3333, 4444, 6], True],
        ]
        # awkward method to not run ethertype match tests on X2
        if (self.client.is_feature_enable(SWITCH_FEATURE_ETYPE_IN_ACL) and
            self.client.is_feature_enable(SWITCH_FEATURE_NAT) == 0):
            print('Data: LACP, action: drop')
            pkt = simple_eth_packet(eth_type=0x8809, pktlen=100)
            send_packet(self, self.devports[0], pkt)
            verify_no_other_packets(self, timeout=1)
        for content in pkt_data:
            data = content[0]
            drop = content[1]
            print(('Data: %s, action: %s') % (data, 'drop' if drop else 'fwd'))
            pkt = simple_tcpv6_packet(pktlen=pkt_len,
                eth_dst='00:55:55:55:55:55',
                eth_src='00:44:44:44:44:44',
                ipv6_dst=data[0],
                ipv6_src=data[1],
                tcp_sport=data[3],
                tcp_dport=data[4],
                tcp_flags=data[2],
                ipv6_hlim=64)
            pkt['IPv6'].nh = data[5]
            send_packet(self, self.devports[0], pkt)
            if drop:
                verify_no_other_packets(self, timeout=1)
            else:
                verify_packet(self, pkt, self.devports[1])

    def tearDown(self):
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.cleanup()

