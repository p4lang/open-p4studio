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

from faulthandler import disable
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
import ptf.mask as mask

from bf_switcht_api_thrift.ttypes import *
from bf_switcht_api_thrift.model_headers import *

this_dir = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(this_dir, '..'))
from common.utils import *

acl_table_bp_port = switcht_list_val_t(type=switcht_value_type.ENUM, enumdata=SWITCH_ACL_TABLE_ATTR_BIND_POINT_TYPE_PORT)

###############################################################################

class L2fwdWithIPFieldsTest(ApiHelper):
    '''
    This perform L2 fwd test with IP fields
    '''
    def setUp(self):
        print()
        self.configure()

        vlan_mbr2 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port2, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
        vlan_mbr3 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port3, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)

        mac2 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:33:33:33:33:33', destination_handle=self.port3)
        mac3 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:44:44:44:44:44', destination_handle=self.port2)

    def runTest(self):
        try:
            self.L2FwdWithDIPZeroTest()
            self.L2FwdWithSIPZeroTest()
            self.L2FwdWithDIPLoopbackTest()
            self.L2FwdWithSIPLoopbackTest()
            self.L2FwdWithTTLZeroTest()

        finally:
            pass

    def tearDown(self):
        self.attribute_set(self.port3, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.port2, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.cleanup()

    def L2FwdWithDIPZeroTest(self):
        tt_pkt = simple_tcp_packet(
            eth_dst='00:33:33:33:33:33',
            eth_src='00:44:44:44:44:44',
            dl_vlan_enable=True,
            vlan_vid=10,
            ip_dst='0.0.0.0',
            ip_ttl=64)
        exp_tt_pkt = simple_tcp_packet(
            eth_dst='00:33:33:33:33:33',
            eth_src='00:44:44:44:44:44',
            ip_dst='0.0.0.0',
            dl_vlan_enable=True,
            vlan_vid=10,
            ip_ttl=64)

        try:
            #Send pkt with DIP as 0
            print("Sending L2 packet with DIP as 0 from %d -> %d " % (self.devports[2], self.devports[3]))
            send_packet(self, self.devports[2], tt_pkt)
            verify_packet(self, exp_tt_pkt, self.devports[3])
        finally:
            pass

    def L2FwdWithSIPZeroTest(self):
        tt_pkt = simple_tcp_packet(
            eth_dst='00:33:33:33:33:33',
            eth_src='00:44:44:44:44:44',
            dl_vlan_enable=True,
            vlan_vid=10,
            ip_src='0.0.0.0',
            ip_dst='10.0.0.2',
            ip_ttl=64)
        exp_tt_pkt = simple_tcp_packet(
            eth_dst='00:33:33:33:33:33',
            eth_src='00:44:44:44:44:44',
            ip_src='0.0.0.0',
            ip_dst='10.0.0.2',
            dl_vlan_enable=True,
            vlan_vid=10,
            ip_ttl=64)

        try:
            #Send pkt with SIP as 0
            print("Sending L2 packet with SIP as 0 from %d -> %d " % (self.devports[2], self.devports[3]))
            send_packet(self, self.devports[2], tt_pkt)
            verify_packet(self, exp_tt_pkt, self.devports[3])
        finally:
            pass

    def L2FwdWithDIPLoopbackTest(self):
        tt_pkt = simple_tcp_packet(
            eth_dst='00:33:33:33:33:33',
            eth_src='00:44:44:44:44:44',
            dl_vlan_enable=True,
            vlan_vid=10,
            ip_dst='127.0.0.1',
            ip_ttl=64)
        exp_tt_pkt = simple_tcp_packet(
            eth_dst='00:33:33:33:33:33',
            eth_src='00:44:44:44:44:44',
            ip_dst='127.0.0.1',
            dl_vlan_enable=True,
            vlan_vid=10,
            ip_ttl=64)

        try:
            #Send pkt with DIP as loopback
            print("Sending L2 packet DIP as 127.0.0.1 from %d -> %d " % (self.devports[2], self.devports[3]))
            send_packet(self, self.devports[2], tt_pkt)
            verify_packet(self, exp_tt_pkt, self.devports[3])
        finally:
            pass

    def L2FwdWithSIPLoopbackTest(self):
        tt_pkt = simple_tcp_packet(
            eth_dst='00:33:33:33:33:33',
            eth_src='00:44:44:44:44:44',
            dl_vlan_enable=True,
            vlan_vid=10,
            ip_src="127.0.0.1",
            ip_dst='10.0.0.2',
            ip_ttl=64)
        exp_tt_pkt = simple_tcp_packet(
            eth_dst='00:33:33:33:33:33',
            eth_src='00:44:44:44:44:44',
            ip_src="127.0.0.1",
            ip_dst='10.0.0.2',
            dl_vlan_enable=True,
            vlan_vid=10,
            ip_ttl=64)

        try:
            #Send pkt with SIP as loopback ip
            print("Sending L2 packet with SIP as 127.0.0.1 from %d -> %d " % (self.devports[2], self.devports[3]))
            send_packet(self, self.devports[2], tt_pkt)
            verify_packet(self, exp_tt_pkt, self.devports[3])
        finally:
            pass

    def L2FwdWithTTLZeroTest(self):
        tt_pkt = simple_tcp_packet(
            eth_dst='00:33:33:33:33:33',
            eth_src='00:44:44:44:44:44',
            dl_vlan_enable=True,
            vlan_vid=10,
            ip_src="10.0.0.1",
            ip_dst='10.0.0.2',
            ip_ttl=0)
        exp_tt_pkt = simple_tcp_packet(
            eth_dst='00:33:33:33:33:33',
            eth_src='00:44:44:44:44:44',
            ip_src="10.0.0.1",
            ip_dst='10.0.0.2',
            dl_vlan_enable=True,
            vlan_vid=10,
            ip_ttl=0)

        try:
            #Send pkt with TTL=0
            print("Sending L2 packet with TTL=0  from %d -> %d " % (self.devports[2], self.devports[3]))
            send_packet(self, self.devports[2], tt_pkt)
            verify_packet(self, exp_tt_pkt, self.devports[3])
        finally:
            pass

###############################################################################
class L2VlanTest(ApiHelper):
    '''
    This performs basic Vlan testing
        port0, port1, lag0 - Access
        port2, port3, lag1 - Trunk
    @test - AccessToAccessTest
    @test - NativeAccessToAccessTest
    @test - AccessToNativeAccessTest
    @test - TrunkToTrunkTest
    @test - AccessToTrunkTest
    @test - TrunkToAccessTest
    @test - AccessToTrunkJumboTest
    @test - TrunkToAccessJumboTest
    @test - PVDropTest
    @test - VlanStatsTest
    @test - TagToUntagTest
    @test - UnTagToTagTest
    '''
    def setUp(self):
        print()
        self.configure()

        self.i_pkt_count = 0;
        self.i_byte_count = 0;
        self.e_pkt_count = 0;
        self.e_byte_count = 0;

        self.lag0 = self.add_lag(self.device)
        lag_mbr0 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port4)
        self.lag1 = self.add_lag(self.device)
        lag_mbr1 = self.add_lag_member(self.device, lag_handle=self.lag1, port_handle=self.port5)

        vlan_mbr0 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port0)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)

        vlan_mbr1 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port1)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        vlan_mbr2 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port2, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
        vlan_mbr3 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port3, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
        vlan_mbr4 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.lag0)
        self.attribute_set(self.lag0, SWITCH_LAG_ATTR_PORT_VLAN_ID, 10)
        vlan_mbr5 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.lag1, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
        vlan_mbr6 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port6)
        self.attribute_set(self.port6, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        vlan_mbr7 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port7, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
        vlan_mbr8 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port8, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_PRIORITY_TAGGED)

        #set PVID before create VLAN
        self.lag2 = self.add_lag(self.device)
        self.vlan50 = self.add_vlan(self.device, vlan_id=50)
        self.attribute_set(self.port13, SWITCH_PORT_ATTR_PORT_VLAN_ID, 50)
        self.attribute_set(self.lag2, SWITCH_LAG_ATTR_PORT_VLAN_ID, 50)
        lag_mbr2 = self.add_lag_member(self.device, lag_handle=self.lag2, port_handle=self.port14)
        vlan_mbr14 = self.add_vlan_member(self.device, vlan_handle=self.vlan50, member_handle=self.lag2, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_UNTAGGED)
        vlan_mbr13 = self.add_vlan_member(self.device, vlan_handle=self.vlan50, member_handle=self.port13, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_UNTAGGED)
        vlan_mbr13 = self.add_vlan_member(self.device, vlan_handle=self.vlan50, member_handle=self.port15, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)

        mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:11:11:11:11:11', destination_handle=self.port1)
        mac1 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:22:22:22:22:22', destination_handle=self.port0)
        mac2 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:33:33:33:33:33', destination_handle=self.port3)
        mac3 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:44:44:44:44:44', destination_handle=self.port2)
        mac4 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:55:55:55:55:55', destination_handle=self.lag0)
        mac5 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:66:66:66:66:66', destination_handle=self.lag1)
        mac6 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:77:77:77:77:77', destination_handle=self.port6)
        mac7 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:88:88:88:88:88', destination_handle=self.port7)
        mac8 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:99:99:99:99:99', destination_handle=self.port8)
        mac8 = self.add_mac_entry(self.device, vlan_handle=self.vlan50, mac_address='00:13:13:13:13:13', destination_handle=self.port13)
        mac8 = self.add_mac_entry(self.device, vlan_handle=self.vlan50, mac_address='00:14:14:14:14:14', destination_handle=self.lag2)
        mac8 = self.add_mac_entry(self.device, vlan_handle=self.vlan50, mac_address='00:15:15:15:15:15', destination_handle=self.port15)

    def runTest(self):
        try:
            self.AccessToAccessConfigureTest()
            self.AccessToTrunkConfigureTest()
            self.TrunkToAccessConfigureTest()
            self.AccessToAccessTest()
            self.TrunkToTrunkTest()
            self.AccessToTrunkTest()
            self.TrunkToAccessTest()
            if self.test_params['target'] != 'hw':
                self.attribute_set(self.port0, SWITCH_PORT_ATTR_RX_MTU, 9216)
                self.attribute_set(self.port0, SWITCH_PORT_ATTR_TX_MTU, 9216)
                self.attribute_set(self.port1, SWITCH_PORT_ATTR_RX_MTU, 9216)
                self.attribute_set(self.port1, SWITCH_PORT_ATTR_TX_MTU, 9216)
                self.attribute_set(self.port2, SWITCH_PORT_ATTR_RX_MTU, 9216)
                self.attribute_set(self.port2, SWITCH_PORT_ATTR_TX_MTU, 9216)
                self.attribute_set(self.port3, SWITCH_PORT_ATTR_RX_MTU, 9216)
                self.attribute_set(self.port3, SWITCH_PORT_ATTR_TX_MTU, 9216)
                self.wait_for_interface_up([self.port0, self.port1, self.port2, self.port3])
                self.AccessToTrunkJumboTest()
                self.TrunkToAccessJumboTest()
            self.AccessToPriorityTest()
            self.PriorityToTrunkTest()
            self.PriorityPcpToTrunkTest()
            self.PriorityToAccessTest()
            self.TrunkToPriorityTest()
            self.TagToUntagTest()
            self.UnTagToTagTest()
            self.PriorityTaggingTest()
            self.PriorityTagQoSTest()
            self.NativeVlanTest()
            self.PVDropTest()
            self.VlanStatsTest()
        finally:
            pass

    def tearDown(self):
        self.attribute_set(self.port3, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.lag0, SWITCH_LAG_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.port6, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.port10, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.cleanup()

    def AccessToAccessTest(self):
        # Access to Access (0 -> 1)
        aa_pkt = simple_udp_packet(
            eth_dst='00:11:11:11:11:11',
            eth_src='00:22:22:22:22:22',
            ip_ttl=64)
        exp_aa_pkt = simple_udp_packet(
            eth_dst='00:11:11:11:11:11',
            eth_src='00:22:22:22:22:22',
            ip_ttl=64)

        try:
            print("Sending L2 packet from %d -> %d : Access to Access" % (self.devports[0], self.devports[1]))
            send_packet(self, self.devports[0], aa_pkt)
            verify_packet(self, exp_aa_pkt, self.devports[1])
            self.i_pkt_count += 1
            self.i_byte_count += len(aa_pkt)
            self.e_pkt_count += 1
            self.e_byte_count += len(exp_aa_pkt)
        finally:
            pass

    def AccessToAccessConfigureTest(self):
        # Access to Access (14 -> 13)
        aa_pkt = simple_udp_packet(
            eth_dst='00:13:13:13:13:13',
            eth_src='00:14:14:14:14:14',
            ip_ttl=64)
        exp_aa_pkt = simple_udp_packet(
            eth_dst='00:13:13:13:13:13',
            eth_src='00:14:14:14:14:14',
            ip_ttl=64)

        try:
            print("Sending L2 packet from %d -> %d : Access to Access" % (self.devports[14], self.devports[13]))
            send_packet(self, self.devports[14], aa_pkt)
            verify_packet(self, exp_aa_pkt, self.devports[13])
        finally:
            pass

    def AccessToTrunkConfigureTest(self):
        # Access to Trunk (13 -> 15)
        at_pkt = simple_tcp_packet(
            eth_dst='00:15:15:15:15:15',
            eth_src='00:13:13:13:13:13',
            ip_ttl=64)
        exp_at_pkt = simple_tcp_packet(
            eth_dst='00:15:15:15:15:15',
            eth_src='00:13:13:13:13:13',
            ip_ttl=64,
            dl_vlan_enable=True,
            vlan_vid=50,
            pktlen=104)

        try:
            print("Sending L2 packet from %d -> %d : Access to Trunk" % (self.devports[13], self.devports[15]))
            send_packet(self, self.devports[13], at_pkt)
            verify_packet(self, exp_at_pkt, self.devports[15])
        finally:
            pass

    def TrunkToAccessConfigureTest(self):
        # Trunk to Access (15 -> 14)
        ta_pkt = simple_tcp_packet(
            eth_dst='00:14:14:14:14:14',
            eth_src='00:15:15:15:15:15',
            dl_vlan_enable=True,
            vlan_vid=50,
            ip_ttl=64)
        exp_ta_pkt = simple_tcp_packet(
            eth_dst='00:14:14:14:14:14',
            eth_src='00:15:15:15:15:15',
            ip_ttl=64,
            pktlen=96)

        try:
            print("Sending L2 packet from %d -> %d : Trunk to Access" % (self.devports[15], self.devports[14]))
            send_packet(self, self.devports[15], ta_pkt)
            verify_packet(self, exp_ta_pkt, self.devports[14])
        finally:
            pass

    def TrunkToTrunkTest(self):
        # Trunk to Trunk (2 -> 3)
        tt_pkt = simple_tcp_packet(
            eth_dst='00:33:33:33:33:33',
            eth_src='00:44:44:44:44:44',
            dl_vlan_enable=True,
            vlan_vid=10,
            ip_ttl=64)
        exp_tt_pkt = simple_tcp_packet(
            eth_dst='00:33:33:33:33:33',
            eth_src='00:44:44:44:44:44',
            dl_vlan_enable=True,
            vlan_vid=10,
            ip_ttl=64)

        try:
            print("Sending L2 packet from %d -> %d : Trunk to Trunk" % (self.devports[2], self.devports[3]))
            send_packet(self, self.devports[2], tt_pkt)
            verify_packet(self, exp_tt_pkt, self.devports[3])
            self.i_pkt_count += 1
            self.i_byte_count += len(tt_pkt)
            self.e_pkt_count += 1
            self.e_byte_count += len(exp_tt_pkt)
        finally:
            pass

    def AccessToTrunkTest(self):
        # Access to Trunk (0 -> 2)
        at_pkt = simple_tcp_packet(
            eth_dst='00:44:44:44:44:44',
            eth_src='00:22:22:22:22:22',
            ip_ttl=64)
        exp_at_pkt = simple_tcp_packet(
            eth_dst='00:44:44:44:44:44',
            eth_src='00:22:22:22:22:22',
            ip_ttl=64,
            dl_vlan_enable=True,
            vlan_vid=10,
            pktlen=104)

        try:
            print("Sending L2 packet from %d -> %d : Access to Trunk" % (self.devports[0], self.devports[2]))
            send_packet(self, self.devports[0], at_pkt)
            verify_packet(self, exp_at_pkt, self.devports[2])
            self.i_pkt_count += 1
            self.i_byte_count += len(at_pkt)
            self.e_pkt_count += 1
            self.e_byte_count += len(exp_at_pkt)
        finally:
            pass

    def AccessToTrunkJumboTest(self):
        # Access to Trunk (0 -> 2)
        at_pkt = simple_tcp_packet(
            eth_dst='00:44:44:44:44:44',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.0.0.1',
            ip_id=102,
            ip_ttl=64,
            pktlen=6000)
        exp_at_pkt = simple_tcp_packet(
            eth_dst='00:44:44:44:44:44',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.0.0.1',
            ip_id=102,
            ip_ttl=64,
            dl_vlan_enable=True,
            vlan_vid=10,
            pktlen=6004)

        try:
            print("Sending L2 packet from %d -> %d : Access to Trunk Jumbo" % (self.devports[0], self.devports[2]))
            send_packet(self, self.devports[0], at_pkt)
            verify_packet(self, exp_at_pkt, self.devports[2])
            self.i_pkt_count += 1
            self.i_byte_count += len(at_pkt)
            self.e_pkt_count += 1
            self.e_byte_count += len(exp_at_pkt)
        finally:
            pass

    def TrunkToAccessTest(self):
        # Trunk to Access (3 -> 1)
        ta_pkt = simple_tcp_packet(
            eth_dst='00:11:11:11:11:11',
            eth_src='00:33:33:33:33:33',
            dl_vlan_enable=True,
            vlan_vid=10,
            ip_ttl=64)
        exp_ta_pkt = simple_tcp_packet(
            eth_dst='00:11:11:11:11:11',
            eth_src='00:33:33:33:33:33',
            ip_ttl=64,
            pktlen=96)

        try:
            print("Sending L2 packet from %d -> %d : Trunk to Access" % (self.devports[3], self.devports[1]))
            send_packet(self, self.devports[3], ta_pkt)
            verify_packet(self, exp_ta_pkt, self.devports[1])
            self.i_pkt_count += 1
            self.i_byte_count += len(ta_pkt)
            self.e_pkt_count += 1
            self.e_byte_count += len(exp_ta_pkt)
        finally:
            pass

    def TrunkToAccessJumboTest(self):
        #Trunk to Access(3->1)
        ta_pkt = simple_tcp_packet(
            eth_dst='00:11:11:11:11:11',
            eth_src='00:33:33:33:33:33',
            dl_vlan_enable=True,
            vlan_vid=10,
            ip_dst='10.0.0.1',
            ip_ttl=64,
            pktlen=6004)
        exp_ta_pkt = simple_tcp_packet(
            eth_dst='00:11:11:11:11:11',
            eth_src='00:33:33:33:33:33',
            ip_dst='10.0.0.1',
            ip_ttl=64,
            pktlen=6000)

        try:
            print("Sending L2 packet from %d -> %d : Trunk to Access Jumbo" % (self.devports[3], self.devports[1]))
            send_packet(self, self.devports[3], ta_pkt)
            verify_packet(self, exp_ta_pkt, self.devports[1])
            self.i_pkt_count += 1
            self.i_byte_count += len(ta_pkt)
            self.e_pkt_count += 1
            self.e_byte_count += len(exp_ta_pkt)
        finally:
            pass


    def AccessToPriorityTest(self):
        # Access to Access (0 -> 8)
        aa_pkt = simple_udp_packet(
            eth_dst='00:99:99:99:99:99',
            eth_src='00:22:22:22:22:22',
            pktlen=100)
        exp_prio_pkt = simple_udp_packet(
            eth_dst='00:99:99:99:99:99',
            eth_src='00:22:22:22:22:22',
            dl_vlan_enable=True,
            vlan_vid=0,
            pktlen=104)

        try:
            print("Sending L2 packet from %d -> %d : Access to Priority" % (self.devports[0], self.devports[8]))
            send_packet(self, self.devports[0], aa_pkt)
            verify_packet(self, exp_prio_pkt, self.devports[8])
            self.i_pkt_count += 1
            self.i_byte_count += len(aa_pkt)
            self.e_pkt_count += 1
            self.e_byte_count += len(exp_prio_pkt)
        finally:
            pass

    def PriorityToTrunkTest(self):
        # Priority to Trunk (1 -> 3)
        prio_pkt = simple_tcp_packet(
            eth_dst='00:33:33:33:33:33',
            eth_src='00:11:11:11:11:11',
            dl_vlan_enable=True,
            vlan_vid=0)
        exp_tt_pkt = simple_tcp_packet(
            eth_dst='00:33:33:33:33:33',
            eth_src='00:11:11:11:11:11',
            dl_vlan_enable=True,
            vlan_vid=10)

        try:
            print("Sending L2 packet from %d -> %d : Priority to Trunk" % (self.devports[1], self.devports[3]))
            send_packet(self, self.devports[1], prio_pkt)
            verify_packet(self, exp_tt_pkt, self.devports[3])
            self.i_pkt_count += 1
            self.i_byte_count += len(prio_pkt)
            self.e_pkt_count += 1
            self.e_byte_count += len(exp_tt_pkt)
        finally:
            pass

    def PriorityPcpToTrunkTest(self):
        # Priority PCP to Trunk (1 -> 3)
        pcp_pkt = simple_tcp_packet(
            eth_dst='00:33:33:33:33:33',
            eth_src='00:11:11:11:11:11',
            dl_vlan_enable=True,
            vlan_vid=0,
            vlan_pcp=6)
        exp_pcp_pkt = simple_tcp_packet(
            eth_dst='00:33:33:33:33:33',
            eth_src='00:11:11:11:11:11',
            dl_vlan_enable=True,
            vlan_vid=10,
            vlan_pcp=6)

        try:
            print("Sending L2 packet from %d -> %d : Priority to Trunk, preserve PCP" % (self.devports[1], self.devports[3]))
            send_packet(self, self.devports[1], pcp_pkt)
            verify_packet(self, exp_pcp_pkt, self.devports[3])
            self.i_pkt_count += 1
            self.i_byte_count += len(pcp_pkt)
            self.e_pkt_count += 1
            self.e_byte_count += len(exp_pcp_pkt)
        finally:
            pass

    def PriorityToAccessTest(self):
        # Priority to Trunk (1 -> 0)
        prio_pkt = simple_tcp_packet(
            eth_dst='00:22:22:22:22:22',
            eth_src='00:11:11:11:11:11',
            dl_vlan_enable=True,
            vlan_vid=0,
            pktlen=104)
        exp_at_pkt = simple_tcp_packet(
            eth_dst='00:22:22:22:22:22',
            eth_src='00:11:11:11:11:11',
            pktlen=100)

        try:
            print("Sending L2 packet from %d -> %d : Priority to Access" % (self.devports[1], self.devports[0]))
            send_packet(self, self.devports[1], prio_pkt)
            verify_packet(self, exp_at_pkt, self.devports[0])
            self.i_pkt_count += 1
            self.i_byte_count += len(prio_pkt)
            self.e_pkt_count += 1
            self.e_byte_count += len(exp_at_pkt)
        finally:
            pass


    def TrunkToPriorityTest(self):
        # Trunk to Priority (3 -> 8)
        ta_pkt = simple_tcp_packet(
            eth_dst='00:99:99:99:99:99',
            eth_src='00:33:33:33:33:33',
            dl_vlan_enable=True,
            vlan_vid=10,
            ip_ttl=64)
        exp_prio_pkt = simple_tcp_packet(
            eth_dst='00:99:99:99:99:99',
            eth_src='00:33:33:33:33:33',
            dl_vlan_enable=True,
            vlan_vid=0,
            ip_ttl=64)

        try:
            print("Sending L2 packet from %d -> %d : Trunk to Priority" % (self.devports[3], self.devports[8]))
            send_packet(self, self.devports[3], ta_pkt)
            verify_packet(self, exp_prio_pkt, self.devports[8])
            self.i_pkt_count += 1
            self.i_byte_count += len(ta_pkt)
            self.e_pkt_count += 1
            self.e_byte_count += len(exp_prio_pkt)
        finally:
            pass

    def PVDropTest(self):
        v100_pkt = simple_tcp_packet(
            eth_dst='00:11:11:11:11:11',
            eth_src='00:33:33:33:33:33',
            dl_vlan_enable=True,
            vlan_vid=100,
            ip_dst='10.0.0.1',
            ip_ttl=64)
        untagged_pkt = simple_tcp_packet(
            eth_dst='00:11:11:11:11:11',
            eth_src='00:33:33:33:33:33',
            ip_dst='10.0.0.1',
            ip_ttl=64)
        v10_pkt = simple_tcp_packet(
            eth_dst='00:44:44:44:44:44',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.0.0.1',
            dl_vlan_enable=True,
            vlan_vid=10,
            ip_ttl=64)
        exp_at_pkt = simple_tcp_packet(
            eth_dst='00:44:44:44:44:44',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.0.0.1',
            dl_vlan_enable=True,
            vlan_vid=10,
            ip_ttl=64)
        v11_pkt = simple_tcp_packet(
            eth_dst='00:44:44:44:44:44',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.0.0.1',
            dl_vlan_enable=True,
            vlan_vid=11,
            ip_ttl=64)
        try:
            print("Sending L2 vlan100 tagged packet to vlan10 tagged port %d, drop" % (self.devports[3]))
            send_packet(self, self.devports[3], v100_pkt)
            verify_no_other_packets(self, timeout=1)
            print("Sending L2 untagged packet to vlan10 tagged port %d, drop" % (self.devports[3]))
            send_packet(self, self.devports[3], untagged_pkt)
            verify_no_other_packets(self, timeout=1)
            print("Sending L2 vlan10 tagged packet to vlan10 untagged port %d, forward" % (self.devports[0]))
            send_packet(self, self.devports[0], v10_pkt)
            verify_packets(self, exp_at_pkt, [self.devports[2]])
            self.i_pkt_count += 1
            self.i_byte_count += len(v10_pkt)
            self.e_pkt_count += 1
            self.e_byte_count += len(exp_at_pkt)
            print("Sending L2 vlan11 tagged packet to vlan10 untagged port %d, drop" % (self.devports[0]))
            send_packet(self, self.devports[0], v11_pkt)
            verify_no_other_packets(self, timeout=1)
        finally:
            pass

    def TagToUntagTest(self):
        pkt = simple_tcp_packet(
            eth_dst='00:77:77:77:77:77',
            eth_src='00:66:66:66:66:66',
            dl_vlan_enable=True,
            vlan_vid=10,
            ip_dst='10.0.0.1',
            ip_ttl=64)
        exp_pkt = simple_tcp_packet(
            eth_dst='00:77:77:77:77:77',
            eth_src='00:66:66:66:66:66',
            ip_dst='10.0.0.1',
            ip_ttl=64,
            pktlen=96)
        pkt1 = simple_tcp_packet(
            eth_dst='00:55:55:55:55:55',
            eth_src='00:88:88:88:88:88',
            dl_vlan_enable=True,
            vlan_vid=10,
            ip_dst='10.0.0.1',
            ip_ttl=64)
        exp_pkt1 = simple_tcp_packet(
            eth_dst='00:55:55:55:55:55',
            eth_src='00:88:88:88:88:88',
            ip_dst='10.0.0.1',
            ip_ttl=64,
            pktlen=96)
        try:
            # lag1 to port
            print("Sending L2 packet from lag1 %d -> port %d vlan 10" % (self.devports[5], self.devports[6]))
            send_packet(self, self.devports[5], pkt)
            verify_packet(self, exp_pkt, self.devports[6])
            self.i_pkt_count += 1
            self.i_byte_count += len(pkt)
            self.e_pkt_count += 1
            self.e_byte_count += len(exp_pkt)
            # port to lag0
            print("Sending L2 packet from port %d -> lag0 port %d vlan 10" % (self.devports[7], self.devports[4]))
            send_packet(self, self.devports[7], pkt1)
            verify_packet(self, exp_pkt1, self.devports[4])
            self.i_pkt_count += 1
            self.i_byte_count += len(pkt1)
            self.e_pkt_count += 1
            self.e_byte_count += len(exp_pkt1)
        finally:
            pass

    def UnTagToTagTest(self):
        pkt = simple_tcp_packet(
            eth_src='00:55:55:55:55:55',
            eth_dst='00:88:88:88:88:88',
            ip_dst='10.0.0.1',
            ip_ttl=64)
        exp_pkt = simple_tcp_packet(
            eth_src='00:55:55:55:55:55',
            eth_dst='00:88:88:88:88:88',
            ip_dst='10.0.0.1',
            dl_vlan_enable=True,
            vlan_vid=10,
            ip_ttl=64,
            pktlen=104)
        pkt1 = simple_tcp_packet(
            eth_src='00:77:77:77:77:77',
            eth_dst='00:66:66:66:66:66',
            ip_dst='10.0.0.1',
            ip_ttl=64)
        exp_pkt1 = simple_tcp_packet(
            eth_src='00:77:77:77:77:77',
            eth_dst='00:66:66:66:66:66',
            ip_dst='10.0.0.1',
            dl_vlan_enable=True,
            vlan_vid=10,
            ip_ttl=64,
            pktlen=104)
        try:
            # lag0 to port
            print("Sending L2 packet from lag0 port %d -> port %d vlan 10" % (self.devports[4], self.devports[7]))
            send_packet(self, self.devports[4], pkt)
            verify_packet(self, exp_pkt, self.devports[7])
            self.i_pkt_count += 1
            self.i_byte_count += len(pkt)
            self.e_pkt_count += 1
            self.e_byte_count += len(exp_pkt)
            # port to lag1
            print("Sending L2 packet from port %d -> lag1 port %d vlan 10" % (self.devports[6], self.devports[5]))
            send_packet(self, self.devports[6], pkt1)
            verify_packet(self, exp_pkt1, self.devports[5])
            self.i_pkt_count += 1
            self.i_byte_count += len(pkt1)
            self.e_pkt_count += 1
            self.e_byte_count += len(exp_pkt1)
        finally:
            pass

    def PriorityTaggingTest(self):
        try:
            # access port testing
            pkt = simple_udp_packet(
                eth_dst='00:11:11:11:11:11',
                eth_src='00:22:22:22:22:22',
                dl_vlan_enable=True,
                vlan_vid=0,
                ip_ttl=64,
                pktlen=104)
            exp_pkt = simple_udp_packet(
                eth_dst='00:11:11:11:11:11',
                eth_src='00:22:22:22:22:22',
                ip_ttl=64,
                pktlen=100)
            print("Sending priority tagged packet on access port %d -> access port %d" % (self.devports[0], self.devports[1]))
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt, self.devports[1])
            self.i_pkt_count += 1
            self.i_byte_count += len(pkt)
            self.e_pkt_count += 1
            self.e_byte_count += len(exp_pkt)

            pkt = simple_udp_packet(
                eth_dst='00:33:33:33:33:33',
                eth_src='00:22:22:22:22:22',
                dl_vlan_enable=True,
                vlan_vid=0,
                ip_ttl=64)
            exp_pkt = simple_udp_packet(
                eth_dst='00:33:33:33:33:33',
                eth_src='00:22:22:22:22:22',
                dl_vlan_enable=True,
                vlan_vid=10,
                ip_ttl=64)
            print("Sending priority tagged packet on access port %d -> trunk port %d" % (self.devports[0], self.devports[3]))
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt, self.devports[3])
            self.i_pkt_count += 1
            self.i_byte_count += len(pkt)
            self.e_pkt_count += 1
            self.e_byte_count += len(exp_pkt)

            # trunk port testing starts here. All of them should be repeated after enabling native vlan
            pkt = simple_udp_packet(
                eth_dst='00:44:44:44:44:44',
                eth_src='00:33:33:33:33:33',
                dl_vlan_enable=True,
                vlan_vid=0,
                ip_ttl=64)
            print("Sending priority tagged packet on trunk port %d to trunk, dropped" % (self.devports[3]))
            send_packet(self, self.devports[3], pkt)
            verify_no_other_packets(self, timeout=1)

            pkt = simple_udp_packet(
                eth_dst='00:11:11:11:11:11',
                eth_src='00:33:33:33:33:33',
                dl_vlan_enable=True,
                vlan_vid=0,
                ip_ttl=64)
            print("Sending priority tagged packet on trunk port %d to access, dropped" % (self.devports[3]))
            send_packet(self, self.devports[3], pkt)
            verify_no_other_packets(self, timeout=1)

            # enable native vlan
            print("Update native vlan of trunk port %d with vlan 10" % (self.devports[3]))
            self.attribute_set(self.port3, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)

            pkt = simple_udp_packet(
                eth_dst='00:44:44:44:44:44',
                eth_src='00:33:33:33:33:33',
                dl_vlan_enable=True,
                vlan_vid=0,
                ip_ttl=64)
            exp_pkt = simple_udp_packet(
                eth_dst='00:44:44:44:44:44',
                eth_src='00:33:33:33:33:33',
                dl_vlan_enable=True,
                vlan_vid=10,
                ip_ttl=64)
            print("Sending priority tagged packet on trunk port %d -> trunk port %d" % (self.devports[3], self.devports[2]))
            send_packet(self, self.devports[3], pkt)
            verify_packet(self, exp_pkt, self.devports[2])
            self.i_pkt_count += 1
            self.i_byte_count += len(pkt)
            self.e_pkt_count += 1
            self.e_byte_count += len(exp_pkt)

            pkt = simple_udp_packet(
                eth_dst='00:11:11:11:11:11',
                eth_src='00:33:33:33:33:33',
                dl_vlan_enable=True,
                vlan_vid=0,
                ip_ttl=64,
                pktlen=104)
            exp_pkt = simple_udp_packet(
                eth_dst='00:11:11:11:11:11',
                eth_src='00:33:33:33:33:33',
                ip_ttl=64,
                pktlen=100)
            print("Sending priority tagged packet on trunk port %d -> access port %d" % (self.devports[3], self.devports[1]))
            send_packet(self, self.devports[3], pkt)
            verify_packet(self, exp_pkt, self.devports[1])
            self.i_pkt_count += 1
            self.i_byte_count += len(pkt)
            self.e_pkt_count += 1
            self.e_byte_count += len(exp_pkt)

        finally:
            print("Update native vlan of trunk port %d - reset to zero " % (self.devports[3]))
            self.attribute_set(self.port3, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            pass

    def PriorityTagQoSTest(self):
        if (self.client.is_feature_enable(SWITCH_FEATURE_INGRESS_QOS_MAP) == 0):
            print("Ingress QoS map disabled, returning")
            return
        try:
            qos_map1 = self.add_qos_map(self.device, pcp=1, tc=20)
            pcp_tc_maps = []
            pcp_tc_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=qos_map1))
            self.pcp_tc_map_ingress = self.add_qos_map_ingress(self.device,
                type=SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_PCP_TO_TC, qos_map_list=pcp_tc_maps)

            qos_map5 = self.add_qos_map(self.device, tc=20, pcp=4)
            tc_pcp_maps = []
            tc_pcp_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=qos_map5))
            self.tc_pcp_map_egress = self.add_qos_map_egress(self.device,
                type=SWITCH_QOS_MAP_EGRESS_ATTR_TYPE_TC_TO_PCP, qos_map_list=tc_pcp_maps)
            pkt = simple_udp_packet(
                eth_dst='00:33:33:33:33:33',
                eth_src='00:22:22:22:22:22',
                dl_vlan_enable=True,
                vlan_vid=0)
            exp_pkt = simple_udp_packet(
                eth_dst='00:33:33:33:33:33',
                eth_src='00:22:22:22:22:22',
                dl_vlan_enable=True,
                vlan_vid=10)
            print("Sending priority tagged packet (pcp=0) on access port %d -> trunk port %d" % (self.devports[0], self.devports[3]))
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt, self.devports[3])
            self.i_pkt_count += 1
            self.i_byte_count += len(pkt)
            self.e_pkt_count += 1
            self.e_byte_count += len(exp_pkt)

            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_QOS_PCP_GROUP, self.pcp_tc_map_ingress)
            self.attribute_set(self.port3, SWITCH_PORT_ATTR_EGRESS_L2_QOS_GROUP, self.tc_pcp_map_egress)
            pkt = simple_udp_packet(
                eth_dst='00:33:33:33:33:33',
                eth_src='00:22:22:22:22:22',
                dl_vlan_enable=True,
                vlan_pcp=1,
                vlan_vid=0)
            exp_pkt = simple_udp_packet(
                eth_dst='00:33:33:33:33:33',
                eth_src='00:22:22:22:22:22',
                dl_vlan_enable=True,
                vlan_pcp=4,
                vlan_vid=10)
            print("Sending priority tagged packet on access port %d (pcp=1) -> trunk port %d (pcp=4)" % (self.devports[0], self.devports[3]))
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt, self.devports[3])
            self.i_pkt_count += 1
            self.i_byte_count += len(pkt)
            self.e_pkt_count += 1
            self.e_byte_count += len(exp_pkt)

        finally:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_QOS_PCP_GROUP, 0)
            self.attribute_set(self.port3, SWITCH_PORT_ATTR_EGRESS_L2_QOS_GROUP, 0)
            print("Update native vlan of trunk port %d - reset to zero " % (self.devports[3]))
            self.attribute_set(self.port3, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            pass

    def NativeVlanTest(self):
        try:
            # trunk port testing starts here. All of them should be repeated after enabling native vlan
            print("Configure Vlan 10 with access/trunk members ")
            tag_pkt = simple_udp_packet(
                eth_dst='00:44:44:44:44:44',
                eth_src='00:33:33:33:33:33',
                dl_vlan_enable=True,
                vlan_vid=10,
                ip_ttl=64,
                pktlen=104)
            untag_pkt = simple_udp_packet(
                eth_dst='00:44:44:44:44:44',
                eth_src='00:33:33:33:33:33',
                ip_ttl=64,
                pktlen=100)
            print("Tx tag packet on trunk port %d -> trunk port %d" % (self.devports[3], self.devports[2]))
            send_packet(self, self.devports[3], tag_pkt)
            verify_packet(self, tag_pkt, self.devports[2])
            self.i_pkt_count += 1
            self.i_byte_count += len(tag_pkt)
            self.e_pkt_count += 1
            self.e_byte_count += len(tag_pkt)

            tag_pkt1 = simple_udp_packet(
                eth_dst='00:11:11:11:11:11',
                eth_src='00:33:33:33:33:33',
                dl_vlan_enable=True,
                vlan_vid=10,
                ip_ttl=64,
                pktlen=104)
            untag_pkt1 = simple_udp_packet(
                eth_dst='00:11:11:11:11:11',
                eth_src='00:33:33:33:33:33',
                ip_ttl=64,
                pktlen=100)
            print("Tx tag packet on trunk port %d -> access port %d" % (self.devports[3], self.devports[1]))
            send_packet(self, self.devports[3], tag_pkt1)
            verify_packet(self, untag_pkt1, self.devports[1])
            self.i_pkt_count += 1
            self.i_byte_count += len(tag_pkt1)
            self.e_pkt_count += 1
            self.e_byte_count += len(untag_pkt1)

            print("Tx untag packet on trunk port %d, drop because no native vlan is set" % (self.devports[3]))
            send_packet(self, self.devports[3], untag_pkt)
            verify_no_other_packets(self, timeout=1)

            tag_pkt_40 = simple_udp_packet(
                eth_dst='00:44:44:44:44:44',
                eth_src='00:33:33:33:33:33',
                dl_vlan_enable=True,
                vlan_vid=40,
                ip_ttl=64,
                pktlen=104)
            print("Tx incorrect tag [i.e. vlan 40] packet on trunk port %d, dropped" % (self.devports[3]))
            send_packet(self, self.devports[3], tag_pkt_40)
            verify_no_other_packets(self, timeout=1)

            # Enable port's native vlan as vlan 10, which is same as port's trunk vlan
            print("Update native vlan of trunk port %d with vlan 10" % (self.devports[3]))
            self.attribute_set(self.port3, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)

            print("Tx tag packet on trunk port %d -> trunk port %d" % (self.devports[3], self.devports[2]))
            send_packet(self, self.devports[3], tag_pkt)
            verify_packet(self, tag_pkt, self.devports[2])
            self.i_pkt_count += 1
            self.i_byte_count += len(tag_pkt)
            self.e_pkt_count += 1
            self.e_byte_count += len(tag_pkt)

            print("Tx tag packet on trunk port %d -> access port %d" % (self.devports[3], self.devports[1]))
            send_packet(self, self.devports[3], tag_pkt1)
            verify_packet(self, untag_pkt1, self.devports[1])
            self.i_pkt_count += 1
            self.i_byte_count += len(tag_pkt1)
            self.e_pkt_count += 1
            self.e_byte_count += len(untag_pkt1)

            print("Tx untag packet on trunk port %d -> trunk port %d" % (self.devports[3], self.devports[2]))
            send_packet(self, self.devports[3], untag_pkt)
            verify_packet(self, tag_pkt, self.devports[2])
            self.i_pkt_count += 1
            self.i_byte_count += len(untag_pkt)
            self.e_pkt_count += 1
            self.e_byte_count += len(tag_pkt)

            print("Tx untag packet on trunk port %d -> access port %d" % (self.devports[3], self.devports[1]))
            send_packet(self, self.devports[3], untag_pkt1)
            verify_packet(self, untag_pkt1, self.devports[1])
            self.i_pkt_count += 1
            self.i_byte_count += len(untag_pkt1)
            self.e_pkt_count += 1
            self.e_byte_count += len(untag_pkt1)

            print("Tx incorrect tag [i.e. vlan 40] packet on trunk port %d, dropped" % (self.devports[3]))
            send_packet(self, self.devports[3], tag_pkt_40)
            verify_no_other_packets(self, timeout=1)

            print("Enable vlan 20 with members - access-port10, trunk-lag1 ")
            self.attribute_set(self.vlan20, SWITCH_VLAN_ATTR_LEARNING, False)
            self.attribute_set(self.vlan20, SWITCH_VLAN_ATTR_UNKNOWN_UCAST_FLOOD_TYPE, SWITCH_VLAN_ATTR_UNKNOWN_UCAST_FLOOD_TYPE_ALL)
            self.attribute_set(self.vlan20, SWITCH_VLAN_ATTR_UNKNOWN_MCAST_FLOOD_TYPE, SWITCH_VLAN_ATTR_UNKNOWN_MCAST_FLOOD_TYPE_ALL)
            self.attribute_set(self.vlan20, SWITCH_VLAN_ATTR_BCAST_FLOOD_TYPE, SWITCH_VLAN_ATTR_BCAST_FLOOD_TYPE_ALL)
            vlan_mbr9 = self.add_vlan_member(self.device, vlan_handle=self.vlan20, member_handle=self.lag1, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
            vlan_mbr10 = self.add_vlan_member(self.device, vlan_handle=self.vlan20, member_handle=self.port10)
            self.attribute_set(self.port10, SWITCH_PORT_ATTR_PORT_VLAN_ID, 20)

            # Enable port's native vlan as vlan 20
            print("Update native vlan of trunk port %d with vlan 20" % (self.devports[3]))
            self.attribute_set(self.port3, SWITCH_PORT_ATTR_PORT_VLAN_ID, 20)

            tag_pkt_20 = simple_udp_packet(
                eth_dst='00:44:44:44:44:44',
                eth_src='00:33:33:33:33:33',
                dl_vlan_enable=True,
                vlan_vid=20,
                ip_ttl=64,
                pktlen=104)
            l1 = [self.devports[5]]
            print("Tx untag packet on trunk port 3 -> Flood to all members of vlan 20 i.e. lag1, port10")
            send_packet(self, self.devports[3], untag_pkt)
            verify_packets_on_multiple_port_lists(self, [tag_pkt_20, untag_pkt],
                [l1, [self.devports[10]]])

            print("Tx tag [vlan 10] packet on trunk port %d -> trunk port %d of vlan 10" % (self.devports[3], self.devports[2]))
            send_packet(self, self.devports[3], tag_pkt)
            verify_packet(self, tag_pkt, self.devports[2])
            self.i_pkt_count += 1
            self.i_byte_count += len(tag_pkt)
            self.e_pkt_count += 1
            self.e_byte_count += len(tag_pkt)

            print("Update native vlan of trunk port %d - reset to zero " % (self.devports[3]))
            self.attribute_set(self.port3, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)

            print("Tx untag packet on trunk port 3, drop because no native vlan is set")
            send_packet(self, self.devports[3], untag_pkt)
            verify_no_other_packets(self, timeout=1)

            print("Tx tag [vlan 10] packet on trunk port %d -> trunk port %d of vlan 10" % (self.devports[3], self.devports[2]))
            send_packet(self, self.devports[3], tag_pkt)
            verify_packet(self, tag_pkt, self.devports[2])
            self.i_pkt_count += 1
            self.i_byte_count += len(tag_pkt)
            self.e_pkt_count += 1
            self.e_byte_count += len(tag_pkt)


        finally:
            print("Update native vlan of trunk port %d - reset to zero " % (self.devports[3]))
            self.attribute_set(self.port3, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.port10, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            pass

    def VlanStatsTest(self):
        '''
        Count the number of packets from the above tests
        '''
        time.sleep(4)
        cntrs = self.client.object_counters_get(self.vlan10)
        # Ingress and egress u/m/b
        self.assertEqual(len(cntrs), 12)
        # 0 is SWITCH_VLAN_COUNTER_ID_IN_UCAST_PKTS
        # 6 is SWITCH_VLAN_COUNTER_ID_OUT_UCAST_PKTS
        # 3 is SWITCH_VLAN_COUNTER_ID_IN_UCAST_BYTES
        # 9 is SWITCH_VLAN_COUNTER_ID_OUT_UCAST_BYTES
        print("IN_UCAST_PKTS counted: %d expected: %d" % (cntrs[0].count, self.i_pkt_count))
        print("OUT_UCAST_PKTS counted: %d expected: %d" % (cntrs[6].count, self.e_pkt_count))
        print("IN_UCAST_BYTES counted: %d expected: %d" % (cntrs[3].count, self.i_byte_count))
        print("OUT_UCAST_BYTES counted: %d expected: %d" % (cntrs[9].count, self.e_byte_count))
        self.assertTrue(cntrs[0].count == self.i_pkt_count)
        self.assertTrue(cntrs[6].count == self.e_pkt_count)
        # add ethernet fcs to the expected size
        self.assertTrue(cntrs[3].count == (self.i_byte_count + (4*self.i_pkt_count)))
        # vlan header invalidated in egress TBD
        #self.assertTrue(cntrs[9].count == (self.e_byte_count + (4*self.e_pkt_count)))

###############################################################################
@group('l2')
class L2VlanMiscTest(ApiHelper):
    '''
    This performs vlan miscellaneous/error testing
        port0, port1, lag0 - Access
        port2, port3, lag1 - Trunk
    @test - AccessToAccessTest
    '''
    def runTest(self):
        print()
        self.configure()

        self.i_pkt_count = 0;
        self.e_pkt_count = 0;

        self.lag0 = self.add_lag(self.device)
        lag_mbr0 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port4)
        self.lag1 = self.add_lag(self.device)
        lag_mbr1 = self.add_lag_member(self.device, lag_handle=self.lag1, port_handle=self.port5)
        lag_mbr2 = self.add_lag_member(self.device, lag_handle=self.lag1, port_handle=self.port7)

        try:
                #self.defaultVlanTest()     // there is currently no support for this in bf_switch
            self.VlansConfig()
            self.VlanMbrUpdTest()
            self.TrunkAccessUpdTest()
        finally:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.lag0, SWITCH_LAG_ATTR_PORT_VLAN_ID, 0)
            self.cleanup()

    def VlansConfig(self):
        try:
            # L2 configure vlan with vlan members - vlan 10, 20
            print("Vlan 20 with members - trunk-port3, trunk-lag1 ")
            vlan_mbr23 = self.add_vlan_member(self.device, vlan_handle=self.vlan20, member_handle=self.port3, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
            vlan_mbr25 = self.add_vlan_member(self.device, vlan_handle=self.vlan20, member_handle=self.lag1, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)

            print("Vlan 10 with members - access-port0, trunk-port2, trunk-port3, access-lag0, trunk-lag1 ")
            self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_LEARNING, False)
            self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_UNKNOWN_UCAST_FLOOD_TYPE, SWITCH_VLAN_ATTR_UNKNOWN_UCAST_FLOOD_TYPE_ALL)
            self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_UNKNOWN_MCAST_FLOOD_TYPE, SWITCH_VLAN_ATTR_UNKNOWN_MCAST_FLOOD_TYPE_ALL)
            self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_BCAST_FLOOD_TYPE, SWITCH_VLAN_ATTR_BCAST_FLOOD_TYPE_ALL)
            vlan_mbr0 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port0)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
            vlan_mbr1 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port2, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
            vlan_mbr3 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port3, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
            vlan_mbr4 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.lag0)
            self.attribute_set(self.lag0, SWITCH_LAG_ATTR_PORT_VLAN_ID, 10)
            vlan_mbr5 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.lag1, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)

        finally:
            #  have no cleanup here because this is just a function to configure
            pass

    #  there is currently no support for this in bf_switch, so not enabling
    def defaultVlanTest(self):
        try:
            # L2 Flood Test, with default  vlan members - add/del or error operations
            tx_pkt = simple_tcp_packet(
                eth_dst='00:33:33:33:33:33',
                eth_src='00:44:44:44:44:44',
                ip_dst='10.0.0.1',
                ip_ttl=64,
                pktlen=96)
            utag_pkt = simple_tcp_packet(
                eth_dst='00:33:33:33:33:33',
                eth_src='00:44:44:44:44:44',
                ip_dst='10.0.0.1',
                ip_ttl=64,
                pktlen=96)

            l0 = [self.devports[4]]
            l1 = [self.devports[5], self.devports[7]]
            print(" ")
            print("VlanMbrUpdTest.... Testing vlan members for default vlan i.e. vlan1  ")
            print("Tx packet from port0 -> Flood to all lag0, lag1, port2, port3")
            send_packet(self, self.devports[0], tx_pkt)
            verify_packets_on_multiple_port_lists(self, [utag_pkt] * 4,
                [l0, l1, [self.devports[2]], [self.devports[3]]])
            #verify_no_other_packets(self, timeout=1)

        finally:
            pass

    def VlanMbrUpdTest(self):
        try:
            # L2 Flood Test, with vlan members - add/del or error operations
            tx_pkt = simple_tcp_packet(
                eth_dst='00:33:33:33:33:33',
                eth_src='00:44:44:44:44:44',
                ip_dst='10.0.0.1',
                ip_ttl=64,
                pktlen=96)
            tag_pkt = simple_tcp_packet(
                eth_dst='00:33:33:33:33:33',
                eth_src='00:44:44:44:44:44',
                ip_dst='10.0.0.1',
                dl_vlan_enable=True,
                vlan_vid=10,
                ip_ttl=64,
                pktlen=100)
            utag_pkt = simple_tcp_packet(
                eth_dst='00:33:33:33:33:33',
                eth_src='00:44:44:44:44:44',
                ip_dst='10.0.0.1',
                ip_ttl=64,
                pktlen=96)

            l0 = [self.devports[4]]
            l1 = [self.devports[5], self.devports[7]]
            print(" ")
            print("VlanMbrUpdTest.... Testing vlan members for vlan - add/del")
            print("Tx packet  from port0 -> Flood to all lag0, lag1, port2, port3")
            send_packet(self, self.devports[0], tx_pkt)
            verify_packets_on_multiple_port_lists(self, [utag_pkt, tag_pkt, tag_pkt, tag_pkt],
                [l0, l1, [self.devports[2]], [self.devports[3]]])

            print("Vlan members remove - access-lag0, trunk-lag1, trunk-port3")
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.attribute_set(self.lag0, SWITCH_LAG_ATTR_PORT_VLAN_ID, 0)
            print("Tx packet from port0 -> Flood to just port2")
            send_packet(self, self.devports[0], tx_pkt)
            verify_packets(self, tag_pkt, [self.devports[2]])

            print("Add vlan member back - trunk-lag1")
            vlan_mbr5 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.lag1, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
            print("Tx packet from port0 -> Flood to just lag1, port2")
            send_packet(self, self.devports[0], tx_pkt)
            verify_packets_on_multiple_port_lists(self, [tag_pkt, tag_pkt],
                [l1, [self.devports[2]]])

            print("Add vlan members back - access-lag0, trunk-port3")
            vlan_mbr3 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port3, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
            vlan_mbr4 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.lag0)
            self.attribute_set(self.lag0, SWITCH_LAG_ATTR_PORT_VLAN_ID, 10)
            print("Tx packet from port0 -> Flood to all lag0, lag1, port2, port3")
            send_packet(self, self.devports[0], tx_pkt)
            verify_packets_on_multiple_port_lists(self, [utag_pkt, tag_pkt, tag_pkt, tag_pkt],
                [l0, l1, [self.devports[2]], [self.devports[3]]])

            print("Try to re add existing vlan members - access-lag0, trunk-port3")
            print("Check for failures as entry exists, but check if flood continues to work")
            vlan_mbr3 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port3, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
            vlan_mbr4 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.lag0)
            print("Tx packet from port0 -> Flood to all lag0, lag1, port2, port3")
            send_packet(self, self.devports[0], tx_pkt)
            verify_packets_on_multiple_port_lists(self, [utag_pkt, tag_pkt, tag_pkt, tag_pkt],
                [l0, l1, [self.devports[2]], [self.devports[3]]])

        finally:
            #  have no cleanup here because runTest cleanup cleans this test case configurations
            pass

    def TrunkAccessUpdTest(self):
        try:
            # L2 Flood Test - access/trunk/error operations
            tx_pkt_v10 = simple_tcp_packet(
                eth_dst='00:33:33:33:33:33',
                eth_src='00:44:44:44:44:44',
                ip_dst='10.0.0.1',
                ip_ttl=64,
                pktlen=96)
            tag_pkt_v10 = simple_tcp_packet(
                eth_dst='00:33:33:33:33:33',
                eth_src='00:44:44:44:44:44',
                ip_dst='10.0.0.1',
                dl_vlan_enable=True,
                vlan_vid=10,
                ip_ttl=64,
                pktlen=100)
            utag_pkt_v10 = simple_tcp_packet(
                eth_dst='00:33:33:33:33:33',
                eth_src='00:44:44:44:44:44',
                ip_dst='10.0.0.1',
                ip_ttl=64,
                pktlen=96)

            l0 = [self.devports[4]]
            l1 = [self.devports[5], self.devports[7]]

            print(" ")
            print("VlanTrunkAccessUpdTest.... Testing vlan members - access/trunk/error operations")
            print("Vlan members trunk-port3, trunk-lag1 are part of both vlan 10, vlan 20")
            print("Add access port to vlan 10 - access-port6")
            vlan_mbr6 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port6)
            self.attribute_set(self.port6, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
            print("Tx packet on vlan 10 from port0 -> Flood to all members - lag0, lag1, port2, port3, port6")
            send_packet(self, self.devports[0], tx_pkt_v10)
            verify_packets_on_multiple_port_lists(self, [utag_pkt_v10, tag_pkt_v10, tag_pkt_v10, tag_pkt_v10, utag_pkt_v10],
                [l0, l1, [self.devports[2]], [self.devports[3]], [self.devports[6]]])

            print("Add same access port to another vlan 20 - access-port6 as only a egress member of vlan 20")
            vlan_mbr7 = self.add_vlan_member(self.device, vlan_handle=self.vlan20, member_handle=self.port6)

            print("Tx packet on vlan 10 from port0 -> Flood to all members - lag0, lag1, port2, port3, port6")
            send_packet(self, self.devports[0], tx_pkt_v10)
            verify_packets_on_multiple_port_lists(self, [utag_pkt_v10, tag_pkt_v10, tag_pkt_v10, tag_pkt_v10, utag_pkt_v10],
                [l0, l1, [self.devports[2]], [self.devports[3]], [self.devports[6]]])

            tx_pkt_v20 = simple_tcp_packet(
                eth_dst='00:33:33:33:33:33',
                eth_src='00:44:44:44:44:44',
                ip_dst='10.0.0.1',
                dl_vlan_enable=True,
                vlan_vid=20,
                ip_ttl=64,
                pktlen=100)
            tag_pkt_v20 = simple_tcp_packet(
                eth_dst='00:33:33:33:33:33',
                eth_src='00:44:44:44:44:44',
                ip_dst='10.0.0.1',
                dl_vlan_enable=True,
                vlan_vid=20,
                ip_ttl=64,
                pktlen=100)
            utag_pkt_v20 = simple_tcp_packet(
                eth_dst='00:33:33:33:33:33',
                eth_src='00:44:44:44:44:44',
                ip_dst='10.0.0.1',
                ip_ttl=64,
                pktlen=96)

            print("Vlan 20 must continue to flood to all current members, incl port6")
            print("Tx packet on vlan 20 from port3 -> Flood to all members - lag1, port6")
            send_packet(self, self.devports[3], tx_pkt_v20)
            verify_packets_on_multiple_port_lists(self, [tag_pkt_v20, utag_pkt_v20],
                [l1, [self.devports[6]]])

            print("Packet ingress from port6 - must continue to classify to vlan 10, Flood to vlan 10 members ")
            print("Tx packet on from port6 -> Flood to all members of vlan 10 - port0, lag0, lag1, port2, port3")
            send_packet(self, self.devports[6], utag_pkt_v10)
            verify_packets_on_multiple_port_lists(self, [utag_pkt_v10, utag_pkt_v10, tag_pkt_v10, tag_pkt_v10, tag_pkt_v10],
                [[self.devports[0]], l0, l1, [self.devports[2]], [self.devports[3]]])

        finally:
            self.attribute_set(self.port6, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.cleanlast()
            self.cleanlast()
            pass

###############################################################################

@group('l2')
class L2FloodTest(ApiHelper):
    '''
    This performs basic L2 flooding
    2 groups of flood lists are created on vlan 10 and 20 with vlan10 having
        vlan10 - port0, port1, port2, port6
        vlan20 - port3, port4, port5, port7
    access ports and vlan20 having trunk ports
    @test - Flood4MemberTest
        Packets are sent from all ports with flooding on happening on all other
        vlan members
    @test - Flood3MemberTest
        Same test as above with 1 member removed from each vlan
    @test - FloodStatsTest
    '''
    def setUp(self):
        print()
        self.configure()

        self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_LEARNING, False)
        self.attribute_set(self.vlan20, SWITCH_VLAN_ATTR_LEARNING, False)

        vlan_mbr0 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port0)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        vlan_mbr1 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port1)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        vlan_mbr2 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port2)
        self.attribute_set(self.port2, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        vlan_mbr3 = self.add_vlan_member(self.device, vlan_handle=self.vlan20, member_handle=self.port3, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
        vlan_mbr4 = self.add_vlan_member(self.device, vlan_handle=self.vlan20, member_handle=self.port4, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
        vlan_mbr5 = self.add_vlan_member(self.device, vlan_handle=self.vlan20, member_handle=self.port5, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
        vlan_mbr6 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port6)
        self.attribute_set(self.port6, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        vlan_mbr7 = self.add_vlan_member(self.device, vlan_handle=self.vlan20, member_handle=self.port7, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
        self.vlan10_i_ucast = 0
        self.vlan10_e_ucast = 0
        self.vlan10_i_bcast = 0
        self.vlan10_e_bcast = 0
        self.vlan10_i_mcast = 0
        self.vlan10_e_mcast = 0
        self.initial_cntrs = self.client.object_counters_get(self.vlan10)

    def runTest(self):
        try:
            self.FloodDisableTest()
            self.Flood4MemberTest()
            print("Remove 1 member from vlan10 and vlan20")
            self.cleanlast()
            self.attribute_set(self.port6, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.cleanlast()
            self.Flood3MemberTest()
            self.FloodStatsTest()
        finally:
            pass

    def tearDown(self):
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.port2, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.port6, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.cleanup()

    def FloodDisableTest(self):
        arp = simple_arp_packet(arp_op=1, pktlen=100)
        ucast_pkt = simple_tcp_packet(
                  eth_dst='00:11:11:11:11:11',
                  eth_src='00:22:22:22:22:22',
                  ip_dst='10.0.0.1',
                  ip_id=107,
                  ip_ttl=64)
        mcast_pkt = simple_tcp_packet(
                  eth_dst='01:11:11:11:11:11',
                  eth_src='00:22:22:22:22:22',
                  ip_dst='231.0.0.1',
                  ip_id=107,
                  ip_ttl=64)
        try:
            print("Sending packets from port %d to %d, %d, %d" % (
                self.devports[0], self.devports[1], self.devports[2], self.devports[6]))
            send_packet(self, self.devports[0], ucast_pkt)
            verify_packets(self, ucast_pkt, [self.devports[1], self.devports[2], self.devports[6]])
            self.vlan10_i_ucast += 1
            self.vlan10_e_ucast += 3
            send_packet(self, self.devports[0], mcast_pkt)
            verify_packets(self, mcast_pkt, [self.devports[1], self.devports[2], self.devports[6]])
            self.vlan10_i_mcast += 1
            self.vlan10_e_mcast += 3
            send_packet(self, self.devports[0], arp)
            verify_packets(self, arp, [self.devports[1], self.devports[2], self.devports[6]])
            self.vlan10_i_bcast += 1
            self.vlan10_e_bcast += 3

            print("\nDisable unknown unicast flooding on vlan 10")
            self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_UNKNOWN_UCAST_FLOOD_TYPE, SWITCH_VLAN_ATTR_UNKNOWN_UCAST_FLOOD_TYPE_NONE)
            print("Sending ucast packet from port %d, no flood" % (self.devports[0]))
            send_packet(self, self.devports[0], ucast_pkt)
            verify_no_other_packets(self, timeout=1)
            self.vlan10_i_ucast += 1
            print("Sending mcast packets from port %d to %d, %d, %d" % (
                self.devports[0], self.devports[1], self.devports[2], self.devports[6]))
            send_packet(self, self.devports[0], mcast_pkt)
            verify_packets(self, mcast_pkt, [self.devports[1], self.devports[2], self.devports[6]])
            self.vlan10_i_mcast += 1
            self.vlan10_e_mcast += 3
            print("Sending bcast packets from port %d to %d, %d, %d" % (
                self.devports[0], self.devports[1], self.devports[2], self.devports[6]))
            send_packet(self, self.devports[0], arp)
            verify_packets(self, arp, [self.devports[1], self.devports[2], self.devports[6]])
            self.vlan10_i_bcast += 1
            self.vlan10_e_bcast += 3
            self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_UNKNOWN_UCAST_FLOOD_TYPE, SWITCH_VLAN_ATTR_UNKNOWN_UCAST_FLOOD_TYPE_ALL)

            print("\nDisable unknown multicast flooding on vlan 10")
            self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_UNKNOWN_MCAST_FLOOD_TYPE, SWITCH_VLAN_ATTR_UNKNOWN_MCAST_FLOOD_TYPE_NONE)
            print("Sending mcast packet from port %d, no flood" % (self.devports[0]))
            send_packet(self, self.devports[0], mcast_pkt)
            verify_no_other_packets(self, timeout=1)
            self.vlan10_i_mcast += 1
            print("Sending ucast packets from port %d to %d, %d, %d" % (
                self.devports[0], self.devports[1], self.devports[2], self.devports[6]))
            send_packet(self, self.devports[0], ucast_pkt)
            verify_packets(self, ucast_pkt, [self.devports[1], self.devports[2], self.devports[6]])
            self.vlan10_i_ucast += 1
            self.vlan10_e_ucast += 3
            print("Sending bcast packets from port %d to %d, %d, %d" % (
                self.devports[0], self.devports[1], self.devports[2], self.devports[6]))
            send_packet(self, self.devports[0], arp)
            verify_packets(self, arp, [self.devports[1], self.devports[2], self.devports[6]])
            self.vlan10_i_bcast += 1
            self.vlan10_e_bcast += 3
            self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_UNKNOWN_MCAST_FLOOD_TYPE, SWITCH_VLAN_ATTR_UNKNOWN_MCAST_FLOOD_TYPE_ALL)

            print("\nDisable broadcast flooding on vlan 10")
            self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_BCAST_FLOOD_TYPE, SWITCH_VLAN_ATTR_BCAST_FLOOD_TYPE_NONE)
            print("Sending broadcast packet from port %d, no flood" % (self.devports[0]))
            send_packet(self, self.devports[0], arp)
            verify_no_other_packets(self, timeout=1)
            self.vlan10_i_bcast += 1
            print("Sending ucast packets from port %d to %d, %d, %d" % (
                self.devports[0], self.devports[1], self.devports[2], self.devports[6]))
            send_packet(self, self.devports[0], ucast_pkt)
            verify_packets(self, ucast_pkt, [self.devports[1], self.devports[2], self.devports[6]])
            self.vlan10_i_ucast += 1
            self.vlan10_e_ucast += 3
            print("Sending mcast packets from port %d to %d, %d, %d" % (
                self.devports[0], self.devports[1], self.devports[2], self.devports[6]))
            send_packet(self, self.devports[0], mcast_pkt)
            verify_packets(self, mcast_pkt, [self.devports[1], self.devports[2], self.devports[6]])
            self.vlan10_i_mcast += 1
            self.vlan10_e_mcast += 3
            self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_BCAST_FLOOD_TYPE, SWITCH_VLAN_ATTR_BCAST_FLOOD_TYPE_ALL)

        finally:
            pass

    def Flood4MemberTest(self):
        arp = simple_arp_packet(arp_op=1, pktlen=100)
        tagged_arp = simple_arp_packet(arp_op=1, vlan_vid=20, pktlen=104)
        pkt = simple_tcp_packet(
            eth_dst='00:11:11:11:11:11',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.0.0.1',
            ip_id=107,
            ip_ttl=64)
        tagged_pkt = simple_tcp_packet(
            eth_dst='00:11:11:11:11:11',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.0.0.1',
            ip_id=107,
            dl_vlan_enable=True,
            vlan_vid=20,
            ip_ttl=64,
            pktlen=104)
        try:
            #untagged ports
            print("Sending packets from port %d to %d, %d, %d" % (
                self.devports[0], self.devports[1], self.devports[2], self.devports[6]))
            send_packet(self, self.devports[0], pkt)
            verify_packets(self, pkt, [self.devports[1], self.devports[2], self.devports[6]])
            self.vlan10_i_ucast += 1
            self.vlan10_e_ucast += 3
            send_packet(self, self.devports[0], arp)
            verify_packets(self, arp, [self.devports[1], self.devports[2], self.devports[6]])
            self.vlan10_i_bcast += 1
            self.vlan10_e_bcast += 3
            print("Sending packets from port %d to %d, %d, %d" % (
                self.devports[1], self.devports[0], self.devports[3], self.devports[6]))
            send_packet(self, self.devports[1], pkt)
            verify_packets(self, pkt, [self.devports[0], self.devports[2], self.devports[6]])
            self.vlan10_i_ucast += 1
            self.vlan10_e_ucast += 3
            send_packet(self, self.devports[1], arp)
            verify_packets(self, arp, [self.devports[0], self.devports[2], self.devports[6]])
            self.vlan10_i_bcast += 1
            self.vlan10_e_bcast += 3
            print("Sending packets from port %d to %d, %d, %d" % (
                self.devports[2], self.devports[0], self.devports[1], self.devports[6]))
            send_packet(self, self.devports[2], pkt)
            verify_packets(self, pkt, [self.devports[0], self.devports[1], self.devports[6]])
            self.vlan10_i_ucast += 1
            self.vlan10_e_ucast += 3
            send_packet(self, self.devports[2], arp)
            verify_packets(self, arp, [self.devports[0], self.devports[1], self.devports[6]])
            self.vlan10_i_bcast += 1
            self.vlan10_e_bcast += 3
            print("Sending packets from port %d to %d, %d, %d" % (
                self.devports[6], self.devports[0], self.devports[1], self.devports[2]))
            send_packet(self, self.devports[6], pkt)
            verify_packets(self, pkt, [self.devports[0], self.devports[1], self.devports[2]])
            self.vlan10_i_ucast += 1
            self.vlan10_e_ucast += 3
            send_packet(self, self.devports[6], arp)
            verify_packets(self, arp, [self.devports[0], self.devports[1], self.devports[2]])
            self.vlan10_i_bcast += 1
            self.vlan10_e_bcast += 3
            #tagged ports
            print("Sending packets from port %d to %d, %d, %d" % (
                self.devports[3], self.devports[4], self.devports[5], self.devports[7]))
            send_packet(self, self.devports[3], tagged_pkt)
            verify_packets(self, tagged_pkt, [self.devports[4], self.devports[5], self.devports[7]])
            send_packet(self, self.devports[3], tagged_arp)
            verify_packets(self, tagged_arp, [self.devports[4], self.devports[5], self.devports[7]])
            print("Sending packets from port %d to %d, %d, %d" % (
                self.devports[4], self.devports[3], self.devports[5], self.devports[7]))
            send_packet(self, self.devports[4], tagged_pkt)
            verify_packets(self, tagged_pkt, [self.devports[3], self.devports[5], self.devports[7]])
            send_packet(self, self.devports[4], tagged_arp)
            verify_packets(self, tagged_arp, [self.devports[3], self.devports[5], self.devports[7]])
            print("Sending packets from port %d to %d, %d, %d" % (
                self.devports[5], self.devports[3], self.devports[4], self.devports[7]))
            send_packet(self, self.devports[5], tagged_pkt)
            verify_packets(self, tagged_pkt, [self.devports[3], self.devports[4], self.devports[7]])
            send_packet(self, self.devports[5], tagged_arp)
            verify_packets(self, tagged_arp, [self.devports[3], self.devports[4], self.devports[7]])
            print("Sending packets from port %d to %d, %d, %d" % (
                self.devports[7], self.devports[3], self.devports[4], self.devports[5]))
            send_packet(self, self.devports[7], tagged_pkt)
            verify_packets(self, tagged_pkt, [self.devports[3], self.devports[4], self.devports[5]])
            send_packet(self, self.devports[7], tagged_arp)
            verify_packets(self, tagged_arp, [self.devports[3], self.devports[4], self.devports[5]])
        finally:
            pass

    def Flood3MemberTest(self):
        arp = simple_arp_packet(arp_op=1, pktlen=100)
        tagged_arp = simple_arp_packet(arp_op=1, vlan_vid=20, pktlen=104)
        pkt = simple_tcp_packet(
            eth_dst='00:11:11:11:11:11',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.0.0.1',
            ip_id=107,
            ip_ttl=64)
        tagged_pkt = simple_tcp_packet(
            eth_dst='00:11:11:11:11:11',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.0.0.1',
            ip_id=107,
            dl_vlan_enable=True,
            vlan_vid=20,
            ip_ttl=64,
            pktlen=104)
        try:
            #untagged ports
            print("Sending packets from port %d to %d, %d" % (
                self.devports[0], self.devports[1], self.devports[2]))
            send_packet(self, self.devports[0], pkt)
            verify_packets(self, pkt, [self.devports[1], self.devports[2]])
            self.vlan10_i_ucast += 1
            self.vlan10_e_ucast += 2
            send_packet(self, self.devports[0], arp)
            verify_packets(self, arp, [self.devports[1], self.devports[2]])
            self.vlan10_i_bcast += 1
            self.vlan10_e_bcast += 2
            print("Sending packets from port %d to %d, %d" % (
                self.devports[1], self.devports[0], self.devports[3]))
            send_packet(self, self.devports[1], pkt)
            verify_packets(self, pkt, [self.devports[0], self.devports[2]])
            self.vlan10_i_ucast += 1
            self.vlan10_e_ucast += 2
            send_packet(self, self.devports[1], arp)
            verify_packets(self, arp, [self.devports[0], self.devports[2]])
            self.vlan10_i_bcast += 1
            self.vlan10_e_bcast += 2
            print("Sending packets from port %d to %d, %d" % (
                self.devports[2], self.devports[0], self.devports[1]))
            send_packet(self, self.devports[2], pkt)
            verify_packets(self, pkt, [self.devports[0], self.devports[1]])
            self.vlan10_i_ucast += 1
            self.vlan10_e_ucast += 2
            send_packet(self, self.devports[2], arp)
            verify_packets(self, arp, [self.devports[0], self.devports[1]])
            self.vlan10_i_bcast += 1
            self.vlan10_e_bcast += 2
            #tagged ports
            print("Sending packets from port %d to %d, %d" % (
                self.devports[3], self.devports[4], self.devports[5]))
            send_packet(self, self.devports[3], tagged_pkt)
            verify_packets(self, tagged_pkt, [self.devports[4], self.devports[5]])
            send_packet(self, self.devports[3], tagged_arp)
            verify_packets(self, tagged_arp, [self.devports[4], self.devports[5]])
            print("Sending packets from port %d to %d, %d" % (
                self.devports[4], self.devports[3], self.devports[5]))
            send_packet(self, self.devports[4], tagged_pkt)
            verify_packets(self, tagged_pkt, [self.devports[3], self.devports[5]])
            send_packet(self, self.devports[4], tagged_arp)
            verify_packets(self, tagged_arp, [self.devports[3], self.devports[5]])
            print("Sending packets from port %d to %d, %d" % (
                self.devports[5], self.devports[3], self.devports[4]))
            send_packet(self, self.devports[5], tagged_pkt)
            verify_packets(self, tagged_pkt, [self.devports[3], self.devports[4]])
            send_packet(self, self.devports[5], tagged_arp)
            verify_packets(self, tagged_arp, [self.devports[3], self.devports[4]])
        finally:
            pass

    def FloodStatsTest(self):
        time.sleep(4)
        final_cntrs = self.client.object_counters_get(self.vlan10)
        iu = SWITCH_VLAN_COUNTER_ID_IN_UCAST_PKTS
        eu = SWITCH_VLAN_COUNTER_ID_OUT_UCAST_PKTS
        ib = SWITCH_VLAN_COUNTER_ID_IN_BCAST_PKTS
        eb = SWITCH_VLAN_COUNTER_ID_OUT_BCAST_PKTS
        im = SWITCH_VLAN_COUNTER_ID_IN_MCAST_PKTS
        em = SWITCH_VLAN_COUNTER_ID_OUT_MCAST_PKTS
        iu_final = final_cntrs[iu].count - self.initial_cntrs[iu].count
        eu_final = final_cntrs[eu].count - self.initial_cntrs[eu].count
        ib_final = final_cntrs[ib].count - self.initial_cntrs[ib].count
        eb_final = final_cntrs[eb].count - self.initial_cntrs[eb].count
        im_final = final_cntrs[im].count - self.initial_cntrs[im].count
        em_final = final_cntrs[em].count - self.initial_cntrs[em].count
        print("IN_UCAST  vlan10 expected: %d counted: %d" % (self.vlan10_i_ucast, iu_final))
        self.assertEqual(iu_final, self.vlan10_i_ucast)
        print("OUT_UCAST vlan10 expected: %d counted: %d" % (self.vlan10_e_ucast, eu_final))
        self.assertEqual(eu_final, self.vlan10_e_ucast)
        print("IN_BCAST  vlan10 expected: %d counted: %d" % (self.vlan10_i_bcast, ib_final))
        self.assertEqual(ib_final, self.vlan10_i_bcast)
        print("OUT_BCAST vlan10 expected: %d counted: %d" % (self.vlan10_e_bcast, eb_final))
        self.assertEqual(eb_final, self.vlan10_e_bcast)
        print("IN_MCAST  vlan10 expected: %d counted: %d" % (self.vlan10_i_mcast, im_final))
        self.assertEqual(ib_final, self.vlan10_i_bcast)
        print("OUT_MCAST vlan10 expected: %d counted: %d" % (self.vlan10_e_mcast, em_final))
        self.assertEqual(em_final, self.vlan10_e_mcast)

###############################################################################

@disabled
class L2BridgeTest(ApiHelper):
    '''
    This performs basic bridge sub port testing
    Bridge 0 - port0/vlan60, port1/vlan60, port2/vlan80
    Bridge 1 - port0/vlan70, port1/vlan70
    '''
    def runTest(self):
        print()
        self.configure()

        br0 = self.add_bridge(self.device, type=SWITCH_BRIDGE_ATTR_TYPE_DOT1D)
        br1 = self.add_bridge(self.device, type=SWITCH_BRIDGE_ATTR_TYPE_DOT1D)

        if0 = self.add_interface(self.device, port_lag_handle=self.port0, type=SWITCH_INTERFACE_ATTR_TYPE_PORT_VLAN,
            vlan_id=60, bridge_handle=br0)
        if1 = self.add_interface(self.device, port_lag_handle=self.port1, type=SWITCH_INTERFACE_ATTR_TYPE_PORT_VLAN,
            vlan_id=60, bridge_handle=br0)
        if2 = self.add_interface(self.device, port_lag_handle=self.port2, type=SWITCH_INTERFACE_ATTR_TYPE_PORT_VLAN,
            vlan_id=80, bridge_handle=br0)
        if3 = self.add_interface(self.device, port_lag_handle=self.port0, type=SWITCH_INTERFACE_ATTR_TYPE_PORT_VLAN,
            vlan_id=70, bridge_handle=br1)
        if4 = self.add_interface(self.device, port_lag_handle=self.port1, type=SWITCH_INTERFACE_ATTR_TYPE_PORT_VLAN,
            vlan_id=70, bridge_handle=br1)

        mac0 = self.add_mac_entry(self.device, vlan_handle=br0, mac_address='00:11:11:11:11:11', interface_handle=if1)
        mac1 = self.add_mac_entry(self.device, vlan_handle=br0, mac_address='00:22:22:22:22:22', interface_handle=if0)
        mac0 = self.add_mac_entry(self.device, vlan_handle=br1, mac_address='00:33:33:33:33:33', interface_handle=if4)
        mac1 = self.add_mac_entry(self.device, vlan_handle=br1, mac_address='00:44:44:44:44:44', interface_handle=if3)

        try:
            self.L2BridgeUnicastTest()
            self.L2BridgeFloodTest()
        finally:
            self.cleanup()

    def L2BridgeUnicastTest(self):
        print("L2BridgeUnicastTest")
        pkt_60 = simple_udp_packet(
            eth_dst='00:11:11:11:11:11',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.0.0.1',
            dl_vlan_enable=True,
            vlan_vid=60,
            ip_ttl=64)
        pkt_70 = simple_udp_packet(
            eth_dst='00:33:33:33:33:33',
            eth_src='00:44:44:44:44:44',
            ip_dst='10.0.0.1',
            dl_vlan_enable=True,
            vlan_vid=70,
            ip_ttl=64)

        try:
            print("Sending L2 packet from 0 -> 1 : port0/vlan60 to port1/vlan60")
            send_packet(self, self.devports[0], pkt_60)
            verify_packet(self, pkt_60, self.devports[1])
            print("Sending L2 packet from 0 -> 1 : port0/vlan70 to port1/vlan70")
            send_packet(self, self.devports[0], pkt_70)
            verify_packet(self, pkt_70, self.devports[1])
        finally:
            pass

    def L2BridgeFloodTest(self):
        print("L2BridgeFloodTest")
        pkt_60 = simple_udp_packet(
            eth_dst='00:22:11:11:11:11',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.0.0.1',
            dl_vlan_enable=True,
            vlan_vid=60,
            ip_ttl=64)
        pkt_80 = simple_udp_packet(
            eth_dst='00:22:11:11:11:11',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.0.0.1',
            dl_vlan_enable=True,
            vlan_vid=80,
            ip_ttl=64)

        try:
            print("Sending L2 packet from 0 -> 1, 2 : port0/vlan60 to port1/vlan60, port2/vlan80")
            send_packet(self, self.devports[0], pkt_60)
            verify_each_packet_on_each_port(self, [pkt_60, pkt_80],
                                       [self.devports[1], self.devports[2]])
        finally:
            pass

###############################################################################

@group('l2')
class L2MacTest(ApiHelper):
    '''
    @test - StaticMacMoveTest
    @test - NoLearnTest
    @test - NoLearnTest2
    @test - MacLearnTest
    @test - DynamicMacLearnTest
    @test - DynamicMacMoveTest
    @test - DynamicLearnAgeTest
    @test - MacMoveErrorTest
    @test - MacLearnErrorTest
    '''
    def setUp(self):
        print()
        self.configure()

        self.iuc = self.euc = 0
        vlan_mbr0 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port0)
        vlan_mbr1 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port1)
        vlan_mbr2 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port2)
        vlan_mbr3 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port3)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.attribute_set(self.port2, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.attribute_set(self.port3, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        vlan_mbr4 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port4, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
        vlan_mbr5 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port5, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
        vlan_mbr6 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port6, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
        vlan_mbr7 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port7, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)

        self.attribute_set(self.port0, SWITCH_PORT_ATTR_LEARNING, True)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_LEARNING, True)
        self.attribute_set(self.port2, SWITCH_PORT_ATTR_LEARNING, True)
        self.attribute_set(self.port3, SWITCH_PORT_ATTR_LEARNING, True)
        self.attribute_set(self.port4, SWITCH_PORT_ATTR_LEARNING, True)
        self.attribute_set(self.port5, SWITCH_PORT_ATTR_LEARNING, True)
        self.attribute_set(self.port6, SWITCH_PORT_ATTR_LEARNING, True)
        self.attribute_set(self.port7, SWITCH_PORT_ATTR_LEARNING, True)
        self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_LEARNING, True)

    def runTest(self):
        try:
            self.client.object_counters_clear_all(self.vlan10)
            self.StaticMacMoveTest()
            self.NoLearnTest()
            self.NoLearnTest2()
            self.MacLearnTest()
            self.DynamicMacLearnTest()
            self.DynamicMacMoveTest()
            self.DynamicLearnAgeTest()
            self.MacMoveErrorTest() #Few test cases still requires fix
            self.MacLearnErrorTest()
            self.VlanStatsTest()

        finally:
            pass

    def tearDown(self):
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_LEARNING, False)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_LEARNING, False)
        self.attribute_set(self.port2, SWITCH_PORT_ATTR_LEARNING, False)
        self.attribute_set(self.port3, SWITCH_PORT_ATTR_LEARNING, False)
        self.attribute_set(self.port4, SWITCH_PORT_ATTR_LEARNING, False)
        self.attribute_set(self.port5, SWITCH_PORT_ATTR_LEARNING, False)
        self.attribute_set(self.port6, SWITCH_PORT_ATTR_LEARNING, False)
        self.attribute_set(self.port7, SWITCH_PORT_ATTR_LEARNING, False)
        self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_LEARNING, False)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.port2, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.port3, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.cleanup()

    def StaticMacMoveTest(self):
        '''
        This performs static MAC move test
        We send a packet from port0 with DMAC installed on port1
        Then the MAC is statically moved from port1 to port2
        A new packet sent from port0 goes to port2
        '''
        print("StaticMacMoveTest()")
        self.lag0 = self.add_lag(self.device)
        lag_mbr0 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port8)
        lag_mbr1 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port9)
        self.lag1 = self.add_lag(self.device)
        lag_mbr2 = self.add_lag_member(self.device, lag_handle=self.lag1, port_handle=self.port10)

        vlan_mbr8 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.lag0)
        self.attribute_set(self.lag0, SWITCH_LAG_ATTR_PORT_VLAN_ID, 10)
        vlan_mbr10 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.lag1, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)

        mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan10,
            mac_address='00:11:11:11:11:11', destination_handle=self.port1)
        mac1 = self.add_mac_entry(self.device, vlan_handle=self.vlan10,
            mac_address='00:22:22:22:22:22', destination_handle=self.port0)
        mac2 = self.add_mac_entry(self.device, vlan_handle=self.vlan10,
            mac_address='00:33:33:33:33:33', destination_handle=self.port5)
        mac3 = self.add_mac_entry(self.device, vlan_handle=self.vlan10,
            mac_address='00:44:44:44:44:44', destination_handle=self.port4)
        mac4 = self.add_mac_entry(self.device, vlan_handle=self.vlan10,
            mac_address='00:55:55:55:55:55', destination_handle=self.lag1)
        pkt = simple_tcp_packet(
            eth_dst='00:11:11:11:11:11',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.0.0.1',
            ip_id=103,
            ip_ttl=64)
        exp_pkt = simple_tcp_packet(
            eth_dst='00:11:11:11:11:11',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.0.0.1',
            ip_id=103,
            ip_ttl=64)
        tag_pkt = simple_tcp_packet(
            eth_dst='00:33:33:33:33:33',
            eth_src='00:44:44:44:44:44',
            ip_dst='10.0.0.1',
            dl_vlan_enable=True,
            vlan_vid=10,
            ip_id=103,
            ip_ttl=64)
        tag_exp_pkt = simple_tcp_packet(
            eth_dst='00:33:33:33:33:33',
            eth_src='00:44:44:44:44:44',
            ip_dst='10.0.0.1',
            dl_vlan_enable=True,
            vlan_vid=10,
            ip_id=103,
            ip_ttl=64)
        untag_pkt1 = simple_tcp_packet(
            eth_dst='00:55:55:55:55:55',
            eth_src='00:11:11:11:11:11',
            ip_dst='10.0.0.1',
            ip_id=103,
            ip_ttl=64,
            pktlen=100)
        tag_pkt1 = simple_tcp_packet(
            eth_dst='00:55:55:55:55:55',
            eth_src='00:11:11:11:11:11',
            ip_dst='10.0.0.1',
            dl_vlan_enable=True,
            vlan_vid=10,
            ip_id=103,
            ip_ttl=64,
            pktlen=104)
        try:
            print("Tx packet port %d" % self.devports[0], "-> port %d" % self.devports[
                1], "[00:22:22:22:22:22 -> 00:11:11:11:11:11]")
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt, self.devports[1])
            self.iuc += 1
            self.euc += 1

            print("Moving static mac [00:11:11:11:11:11] from port %d" % self.devports[
                1], " to port %d" % self.devports[2])
            self.attribute_set(mac0, SWITCH_MAC_ENTRY_ATTR_DESTINATION_HANDLE, self.port2)
            print("Tx packet port %d" % self.devports[0], "-> port %d" % self.devports[
                2], "[00:22:22:22:22:22 -> 00:11:11:11:11:11]")
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, exp_pkt, self.devports[2])
            self.iuc += 1
            self.euc += 1

            print("Moving static mac [00:11:11:11:11:11] from port %d" % self.devports[
                2], " to access-lag0")
            self.attribute_set(mac0, SWITCH_MAC_ENTRY_ATTR_DESTINATION_HANDLE, self.lag0)
            l0 = [self.devports[8], self.devports[9]]
            print("Tx packet port %d" % self.devports[0], "-> lag0 [00:22:22:22:22:22 -> 00:11:11:11:11:11]")
            send_packet(self, self.devports[0], pkt)
            verify_any_packet_any_port(self, [exp_pkt, exp_pkt], l0)
            self.iuc += 1
            self.euc += 1

            print("Tx packet port %d" % self.devports[4], "-> port %d" % self.devports[
                5], "[00:44:44:44:44:44 -> 00:33:33:33:33:33]")
            send_packet(self, self.devports[4], tag_pkt)
            verify_packet(self, tag_exp_pkt, self.devports[5])
            self.iuc += 1
            self.euc += 1

            print("Moving static mac [00:33:33:33:33:33] from port %d" % self.devports[
                5], " to port %d" % self.devports[6])
            self.attribute_set(mac2, SWITCH_MAC_ENTRY_ATTR_DESTINATION_HANDLE, self.port6)

            print("Tx packet port %d" % self.devports[4], "-> port %d" % self.devports[
                6], "[00:44:44:44:44:44 -> 00:33:33:33:33:33]")
            send_packet(self, self.devports[4], tag_pkt)
            verify_packet(self, tag_exp_pkt, self.devports[6])
            self.iuc += 1
            self.euc += 1

            print("Tx packet lag0 [member port8] -> lag1 [00:11:11:11:11:11 -> 00:55:55:55:55:55]")
            send_packet(self, self.devports[8], untag_pkt1)
            verify_packet(self, tag_pkt1, self.devports[10])
            self.iuc += 1
            self.euc += 1

            print("Tx packet lag0 [member port9] -> lag1 [00:11:11:11:11:11 -> 00:55:55:55:55:55]")
            send_packet(self, self.devports[9], untag_pkt1)
            verify_packet(self, tag_pkt1, self.devports[10])
            self.iuc += 1
            self.euc += 1

            print("Moving static mac [00:11:11:11:11:11] from access-lag0 to port1")
            self.attribute_set(mac0, SWITCH_MAC_ENTRY_ATTR_DESTINATION_HANDLE, self.port1)
            print("Moving static mac [00:55:55:55:55:55] from trunk-lag1 to access-lag0")
            self.attribute_set(mac4, SWITCH_MAC_ENTRY_ATTR_DESTINATION_HANDLE, self.lag0)

            print("Tx packet port %d" % self.devports[1], "-> lag0 [00:11:11:11:11:11 -> 00:55:55:55:55:55]")
            send_packet(self, self.devports[1], untag_pkt1)
            verify_any_packet_any_port(self, [untag_pkt1, untag_pkt1], l0)
            self.iuc += 1
            self.euc += 1
        finally:
            self.attribute_set(self.lag0, SWITCH_LAG_ATTR_PORT_VLAN_ID, 0)
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

    def NoLearnTest(self):
        print("NoLearnTest()")
        self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_AGING_INTERVAL, 20000)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_LEARNING, False)
        self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_LEARNING, False)
        pkt1 = simple_udp_packet(
            eth_dst='00:33:33:33:33:33',
            eth_src='00:11:11:11:11:11',
            ip_dst='10.0.0.1',
            ip_ttl=64,
            pktlen=100)
        tag_pkt1 = simple_udp_packet(
            eth_dst='00:33:33:33:33:33',
            eth_src='00:11:11:11:11:11',
            ip_dst='10.0.0.1',
            ip_ttl=64,
            dl_vlan_enable=True,
            vlan_vid=10,
            pktlen=104)
        pkt2 = simple_udp_packet(
            eth_src='00:33:33:33:33:33',
            eth_dst='00:11:11:11:11:11',
            ip_dst='10.0.0.1',
            ip_ttl=64,
            pktlen=100)
        tag_pkt2 = simple_udp_packet(
            eth_src='00:33:33:33:33:33',
            eth_dst='00:11:11:11:11:11',
            ip_dst='10.0.0.1',
            dl_vlan_enable=True,
            vlan_vid=10,
            ip_ttl=64,
            pktlen=104)
        try:
            print("Mac-Learning disabled both on vlan, port")
            print("Tx packet from port %d" % self.devports[0], " --> Flood [00:11:11:11:11:11 -> 00:33:33:33:33:33]")
            send_packet(self, self.devports[0], pkt1)
            verify_each_packet_on_each_port(self, [pkt1, pkt1, pkt1, tag_pkt1, tag_pkt1, tag_pkt1, tag_pkt1],
                [self.devports[1], self.devports[2], self.devports[3], self.devports[4], self.devports[5], self.devports[6], self.devports[7]])
            time.sleep(3)
            self.iuc += 1
            self.euc += 7
            print("Tx packet [With target Mac 00:11:11:11:11:11], Mac not learnt -> will flood")
            send_packet(self, self.devports[1], pkt2)
            verify_each_packet_on_each_port(self, [pkt2, pkt2, pkt2, tag_pkt2, tag_pkt2, tag_pkt2, tag_pkt2],
                [self.devports[0], self.devports[2], self.devports[3], self.devports[4], self.devports[5], self.devports[6], self.devports[7]])
            self.iuc += 1
            self.euc += 7

            print("Enable learning on port %d" % self.devports[0])
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_LEARNING, True)
            print("Tx packet from port %d" % self.devports[0], " --> Flood [00:11:11:11:11:11 -> 00:33:33:33:33:33]")
            send_packet(self, self.devports[0], pkt1)
            verify_each_packet_on_each_port(self, [pkt1, pkt1, pkt1, tag_pkt1, tag_pkt1, tag_pkt1, tag_pkt1],
                [self.devports[1], self.devports[2], self.devports[3], self.devports[4], self.devports[5], self.devports[6], self.devports[7]])
            time.sleep(3)
            self.iuc += 1
            self.euc += 7
            print("Tx packet [With target Mac 00:11:11:11:11:11], Mac still not learnt --> will flood")
            send_packet(self, self.devports[2], pkt2)
            verify_each_packet_on_each_port(self, [pkt2, pkt2, pkt2, tag_pkt2, tag_pkt2, tag_pkt2, tag_pkt2],
                [self.devports[0], self.devports[1], self.devports[3], self.devports[4], self.devports[5], self.devports[6], self.devports[7]])
            self.iuc += 1
            self.euc += 7

            print("Enable learning on vlan 10")
            self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_LEARNING, True)
            print("Tx packet from port %d" % self.devports[0], " --> Flood [00:11:11:11:11:11 -> 00:33:33:33:33:33]")
            send_packet(self, self.devports[0], pkt1)
            verify_each_packet_on_each_port(self, [pkt1, pkt1, pkt1, tag_pkt1, tag_pkt1, tag_pkt1, tag_pkt1],
                [self.devports[1], self.devports[2], self.devports[3], self.devports[4], self.devports[5], self.devports[6], self.devports[7]])
            time.sleep(3)
            self.iuc += 1
            self.euc += 7
            print("Tx packet [With target Mac 00:11:11:11:11:11], Mac will now be learnt on port 0 --> Forw to port %d" % self.devports[0])
            send_packet(self, self.devports[2], pkt2)
            verify_packet(self, pkt2, self.devports[0])
            self.iuc += 1
            self.euc += 1
        finally:
            time.sleep(3)
            self.client.object_flush_all(SWITCH_OBJECT_TYPE_MAC_ENTRY)

    def NoLearnTest2(self):
        print("NoLearnTest2()")

        self.attribute_set(self.vlan20, SWITCH_VLAN_ATTR_AGING_INTERVAL, 20000)
        self.attribute_set(self.port8, SWITCH_PORT_ATTR_LEARNING, True)
        self.attribute_set(self.port9, SWITCH_PORT_ATTR_LEARNING, True)

        vlan_mbr1 = self.add_vlan_member(self.device, vlan_handle=self.vlan20, member_handle=self.port8)
        self.attribute_set(self.port8, SWITCH_PORT_ATTR_PORT_VLAN_ID, 20)
        mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan20,
            mac_address='00:33:33:33:33:33', destination_handle=self.port8)
        vlan_mbr2 = self.add_vlan_member(self.device, vlan_handle=self.vlan20, member_handle=self.port9)
        self.attribute_set(self.port9, SWITCH_PORT_ATTR_PORT_VLAN_ID, 20)
        pkt = simple_udp_packet(
            eth_dst='00:33:33:33:33:33',
            eth_src='00:11:11:11:11:11',
            ip_dst='10.0.0.1',
            ip_ttl=64,
            pktlen=100)
        pkt1 = simple_udp_packet(
            eth_dst='00:11:11:11:11:11',
            eth_src='00:33:33:33:33:33',
            ip_dst='10.0.0.1',
            ip_ttl=64,
            pktlen=100)
        tag_pkt1 = simple_udp_packet(
            eth_dst='00:11:11:11:11:11',
            eth_src='00:33:33:33:33:33',
            ip_dst='10.0.0.1',
            ip_ttl=64,
            dl_vlan_enable=True,
            vlan_vid=20,
            pktlen=104)

        try:
            send_packet(self, self.devports[9], pkt)
            verify_packet(self, pkt, self.devports[8])
            time.sleep(3)
            send_packet(self, self.devports[8], pkt1)
            verify_packet(self, pkt1, self.devports[9])
            print("Remove vlan member on port 9 and resend")
            self.cleanlast()
            # if the MAC were not flushed, then the packet would still be sent
            # with a vlan tag added since the pv_to_bd is a miss
            send_packet(self, self.devports[8], pkt1)
            verify_packet(self, tag_pkt1, self.devports[9])
            print("Flush MAC, no packet should be seen")
            self.client.object_flush_all(SWITCH_OBJECT_TYPE_MAC_ENTRY)
            send_packet(self, self.devports[8], pkt1)
            verify_no_other_packets(self, timeout=1)
        finally:
            self.attribute_set(self.port8, SWITCH_PORT_ATTR_LEARNING, False)
            self.attribute_set(self.port9, SWITCH_PORT_ATTR_LEARNING, False)
            self.attribute_set(self.port9, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.cleanlast()
            time.sleep(3)
            self.client.object_flush_all(SWITCH_OBJECT_TYPE_MAC_ENTRY)
            self.attribute_set(self.port8, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.cleanlast()

    def MacLearnTest(self):
        print("MacLearnTest()")
        self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_AGING_INTERVAL, 20000)
        mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan10,
            mac_address='00:33:33:33:33:33', destination_handle=self.port2)
        mac1 = self.add_mac_entry(self.device, vlan_handle=self.vlan10,
            mac_address='00:66:66:66:66:66', destination_handle=self.port6)
        pkt1 = simple_udp_packet(
            eth_dst='00:33:33:33:33:33',
            eth_src='00:11:11:11:11:11',
            ip_dst='10.0.0.1',
            ip_ttl=64)
        pkt2 = simple_udp_packet(
            eth_dst='00:33:33:33:33:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.0.0.1',
            ip_ttl=64)
        pkt3 = simple_udp_packet(
            eth_src='00:11:11:11:11:11',
            eth_dst='00:22:22:22:22:22',
            ip_dst='10.0.0.1',
            ip_ttl=64)
        tag_pkt1 = simple_udp_packet(
            eth_dst='00:66:66:66:66:66',
            eth_src='00:44:44:44:44:44',
            ip_dst='10.0.0.1',
            dl_vlan_enable=True,
            vlan_vid=10,
            ip_ttl=64)
        tag_pkt2 = simple_udp_packet(
            eth_dst='00:66:66:66:66:66',
            eth_src='00:55:55:55:55:55',
            ip_dst='10.0.0.1',
            dl_vlan_enable=True,
            vlan_vid=10,
            ip_ttl=64)
        tag_pkt3 = simple_udp_packet(
            eth_src='00:44:44:44:44:44',
            eth_dst='00:55:55:55:55:55',
            ip_dst='10.0.0.1',
            dl_vlan_enable=True,
            vlan_vid=10,
            ip_ttl=64)
        try:
            print("Sending packet port %d" % self.devports[0], "-> port %d" % self.devports[
                2], " (00:11:11:11:11:11 -> 00:33:33:33:33:33)")
            send_packet(self, self.devports[0], pkt1)
            verify_packet(self, pkt1, self.devports[2])
            time.sleep(3)
            self.iuc += 1
            self.euc += 1
            print("Sending packet port %d" % self.devports[1], "-> port %d" % self.devports[
                2], " (00:22:22:22:22:22 -> 00:33:33:33:33:33)")
            send_packet(self, self.devports[1], pkt2)
            verify_packet(self, pkt2, self.devports[2])
            time.sleep(3)
            self.iuc += 1
            self.euc += 1
            print("Sending packet port %d" % self.devports[0], "-> port %d" % self.devports[
                1], " (00:11:11:11:11:11 -> 00:22:22:22:22:22)")
            send_packet(self, self.devports[0], pkt3)
            verify_packet(self, pkt3, self.devports[1])
            self.iuc += 1
            self.euc += 1

            print("Sending packet port %d" % self.devports[4], "-> port %d" % self.devports[
                6], " (00:44:44:44:44:44 -> 00:66:66:66:66:66)")
            send_packet(self, self.devports[4], tag_pkt1)
            verify_packet(self, tag_pkt1, self.devports[6])
            time.sleep(3)
            self.iuc += 1
            self.euc += 1
            print("Sending packet port %d" % self.devports[5], "-> port %d" % self.devports[
                6], " (00:55:55:55:55:55 -> 00:66:66:66:66:66)")
            send_packet(self, self.devports[5], tag_pkt2)
            verify_packet(self, tag_pkt2, self.devports[6])
            time.sleep(3)
            self.iuc += 1
            self.euc += 1
            print("Sending packet port %d" % self.devports[4], "-> port %d" % self.devports[
                5], " (00:44:44:44:44:44 -> 00:55:55:55:55:55)")
            send_packet(self, self.devports[4], tag_pkt3)
            verify_packet(self, tag_pkt3, self.devports[5])
            time.sleep(3)
            self.iuc += 1
            self.euc += 1

            print("Sending packet port %d" % self.devports[7], "-> port %d" % self.devports[
                6], " (00:55:55:55:55:55 -> 00:66:66:66:66:66)")
            send_packet(self, self.devports[7], tag_pkt2)
            verify_packet(self, tag_pkt2, self.devports[6])
            time.sleep(3)
            self.iuc += 1
            self.euc += 1

            print("Mac 00:55:55:55:55:55 must now move from port 5 to port 7")
            print("Sending packet port %d" % self.devports[4], "-> port %d" % self.devports[
                7], " (00:44:44:44:44:44 -> 00:55:55:55:55:55)")
            send_packet(self, self.devports[4], tag_pkt3)
            verify_packet(self, tag_pkt3, self.devports[7])
            self.iuc += 1
            self.euc += 1

        finally:
            self.cleanlast()
            self.cleanlast()
            self.client.object_flush_all(SWITCH_OBJECT_TYPE_MAC_ENTRY)

    def DynamicMacLearnTest(self):
        print("DynamicMacLearnTest()")
        self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_AGING_INTERVAL, 90000)

        for port in range(0, 8):
            port_list = [self.devports[p] for p in range(0,8) if p != port]
            for mac_offset in range(1, 5):
                dst_mac = '00:33:33:33:' + str(port) + ':' + str(mac_offset)
                src_mac = '00:22:22:22:' + str(port) + ':' + str(mac_offset)
                pkt = simple_tcp_packet(
                    eth_dst=dst_mac,
                    eth_src=src_mac,
                    ip_dst='10.10.10.1',
                    ip_src='20.20.20.1',
                    ip_id=108,
                    ip_ttl=64,
                    pktlen=100)
                tag_pkt = simple_tcp_packet(
                    eth_dst=dst_mac,
                    eth_src=src_mac,
                    ip_dst='10.10.10.1',
                    ip_src='20.20.20.1',
                    ip_id=108,
                    dl_vlan_enable=True,
                    vlan_vid=10,
                    ip_ttl=64,
                    pktlen=104)
                pkt_list = []
                send_pkt = pkt
                for p in range(0,8):
                    if p != port:
                        if p < 4:
                            pkt_list.append(pkt)
                        else:
                            pkt_list.append(tag_pkt)
                            send_pkt = tag_pkt
                send_packet(self, self.devports[port], send_pkt)
                verify_each_packet_on_each_port(self, pkt_list, port_list)
                self.iuc += 1
                self.euc += 7
        print("Learning complete")

        try:
            for dst_port in range(0, 8):
                for src_port in range(0, 8):
                    for mac_offset in range(1, 5):
                        if src_port == dst_port:
                            continue
                        dst_mac = '00:22:22:22:' + \
                            str(dst_port) + ':' + str(mac_offset)
                        src_mac = '00:22:22:22:' + \
                            str(src_port) + ':' + str(mac_offset)
                        pkt = simple_tcp_packet(
                            eth_dst=dst_mac,
                            eth_src=src_mac,
                            ip_dst='10.10.10.1',
                            ip_src='20.20.20.1',
                            ip_id=108,
                            ip_ttl=64,
                            pktlen=100)
                        tag_pkt = simple_tcp_packet(
                            eth_dst=dst_mac,
                            eth_src=src_mac,
                            ip_dst='10.10.10.1',
                            ip_src='20.20.20.1',
                            ip_id=108,
                            dl_vlan_enable=True,
                            vlan_vid=10,
                            ip_ttl=64,
                            pktlen=104)
                        send_pkt = pkt
                        rcv_pkt = pkt
                        if dst_port > 3:
                            rcv_pkt = tag_pkt
                        if src_port > 3:
                            send_pkt = tag_pkt
                        send_packet(self, self.devports[src_port], send_pkt)
                        verify_packet(self, rcv_pkt, self.devports[dst_port], timeout=4)
                        self.iuc += 1
                        self.euc += 1
        finally:
            self.client.object_flush_all(SWITCH_OBJECT_TYPE_MAC_ENTRY)

    def DynamicMacMoveTest(self):
        '''
        This performs dynamic MAC move test
        00:22:22:22:22:22 is first learnt on port0 and then later moved to port2
        Finally send a packet with above MAC and expect a packet on port2
        '''
        print("DynamicMacMoveTest()")
        pkt1 = simple_tcp_packet(
            eth_dst='00:11:11:11:11:11',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.0.0.1',
            ip_id=104,
            ip_ttl=64,
            pktlen=100)
        tag_pkt1 = simple_tcp_packet(
            eth_dst='00:11:11:11:11:11',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.0.0.1',
            ip_id=104,
            dl_vlan_enable=True,
            vlan_vid=10,
            ip_ttl=64,
            pktlen=104)
        pkt2 = simple_tcp_packet(
            eth_dst='00:22:22:22:22:22',
            eth_src='00:11:11:11:11:11',
            ip_dst='10.0.0.1',
            ip_id=104,
            ip_ttl=64,
            pktlen=100)

        pkt3 = simple_tcp_packet(
            eth_dst='00:33:33:33:33:33',
            eth_src='00:44:44:44:44:44',
            ip_dst='10.0.0.1',
            ip_id=104,
            ip_ttl=64,
            pktlen=100)
        tag_pkt3 = simple_tcp_packet(
            eth_dst='00:33:33:33:33:33',
            eth_src='00:44:44:44:44:44',
            ip_dst='10.0.0.1',
            ip_id=104,
            dl_vlan_enable=True,
            vlan_vid=10,
            ip_ttl=64,
            pktlen=104)
        tag_pkt4 = simple_tcp_packet(
            eth_dst='00:44:44:44:44:44',
            eth_src='00:33:33:33:33:33',
            ip_dst='10.0.0.1',
            ip_id=104,
            dl_vlan_enable=True,
            vlan_vid=10,
            ip_ttl=64,
            pktlen=104)

        try:
            print("Sending packet port %d" % self.devports[
                0], " -> port %d" % self.devports[
                    1], " (00:22:22:22:22:22 -> 00:11:11:11:11:11)")
            send_packet(self, self.devports[0], pkt1)
            verify_each_packet_on_each_port(self, [pkt1, pkt1, pkt1, tag_pkt1, tag_pkt1, tag_pkt1, tag_pkt1],
                [self.devports[1], self.devports[2], self.devports[3], self.devports[4], self.devports[5], self.devports[6], self.devports[7]])
            time.sleep(3)
            self.iuc += 1
            self.euc += 7

            print("Sending packet port %d" % self.devports[
                1], " -> port %d" % self.devports[
                    0], " (00:11:11:11:11:11 -> 00:22:22:22:22:22)")
            send_packet(self, self.devports[1], pkt2)
            verify_packet(self, pkt2, self.devports[0])
            time.sleep(3)
            self.iuc += 1
            self.euc += 1

            print("Moving mac (00:22:22:22:22:22) from port %d" % self.devports[
                0], " to port %d" % self.devports[2], " ")
            print("Sending packet port %d" % self.devports[
                2], " -> port %d" % self.devports[
                    1], " (00:22:22:22:22:22 -> 00:11:11:11:11:11)")
            send_packet(self, self.devports[2], pkt1)
            verify_packet(self, pkt1, self.devports[1])
            time.sleep(3)
            self.iuc += 1
            self.euc += 1

            print("Sending packet port %d" % self.devports[
                1], " -> port %d" % self.devports[
                    2], " (00:11:11:11:11:11 -> 00:22:22:22:22:22)")
            send_packet(self, self.devports[1], pkt2)
            verify_packet(self, pkt2, self.devports[2])
            self.iuc += 1
            self.euc += 1

            print("Sending packet port %d" % self.devports[
                4], " -> port %d" % self.devports[
                    5], " (00:44:44:44:44:44 -> 00:33:33:33:33:33)")
            send_packet(self, self.devports[4], tag_pkt3)
            verify_each_packet_on_each_port(self, [pkt3, pkt3, pkt3, pkt3, tag_pkt3, tag_pkt3, tag_pkt3],
                [self.devports[0], self.devports[1], self.devports[2], self.devports[3], self.devports[5], self.devports[6], self.devports[7]])
            time.sleep(3)
            self.iuc += 1
            self.euc += 7

            print("Sending packet port %d" % self.devports[
                5], " -> port %d" % self.devports[
                    4], " (00:33:33:33:33:33 -> 00:44:44:44:44:44)")
            send_packet(self, self.devports[5], tag_pkt4)
            verify_packet(self, tag_pkt4, self.devports[4])
            time.sleep(3)
            self.iuc += 1
            self.euc += 1

            print("Moving mac (00:44:44:44:44:44) from port %d" % self.devports[
                4], " to port %d" % self.devports[2], " ")
            print("Sending packet port %d" % self.devports[
                6], " -> port %d" % self.devports[
                    5], " (00:44:44:44:44:44 -> 00:33:33:33:33:33)")
            send_packet(self, self.devports[6], tag_pkt3)
            verify_packet(self, tag_pkt3, self.devports[5])
            time.sleep(3)
            self.iuc += 1
            self.euc += 1

            print("Sending packet port %d" % self.devports[
                5], " -> port %d" % self.devports[
                    6], " (00:33:33:33:33:33 -> 00:44:44:44:44:44)")
            send_packet(self, self.devports[5], tag_pkt4)
            verify_packet(self, tag_pkt4, self.devports[6])
            self.iuc += 1
            self.euc += 1
        finally:
            self.client.object_flush_all(SWITCH_OBJECT_TYPE_MAC_ENTRY)

    def DynamicLearnAgeTest(self):
        print("DynamicLearnAgeTest()")
        self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_AGING_INTERVAL, 15000)
        pkt1 = simple_tcp_packet(
            eth_dst='00:66:66:66:66:66',
            eth_src='00:77:77:77:77:77',
            ip_dst='10.0.0.1',
            ip_id=115,
            ip_ttl=64,
            pktlen=100)
        tag_pkt1 = simple_tcp_packet(
            eth_dst='00:66:66:66:66:66',
            eth_src='00:77:77:77:77:77',
            ip_dst='10.0.0.1',
            ip_id=115,
            dl_vlan_enable=True,
            vlan_vid=10,
            ip_ttl=64,
            pktlen=104)

        pkt2 = simple_tcp_packet(
            eth_src='00:66:66:66:66:66',
            eth_dst='00:77:77:77:77:77',
            ip_dst='10.0.0.1',
            ip_id=115,
            ip_ttl=64,
            pktlen=100)
        tag_pkt2 = simple_tcp_packet(
            eth_src='00:66:66:66:66:66',
            eth_dst='00:77:77:77:77:77',
            ip_dst='10.0.0.1',
            ip_id=115,
            dl_vlan_enable=True,
            vlan_vid=10,
            ip_ttl=64,
            pktlen=104)

        try:
            print("Sending packet from port %d" % self.devports[
                0], " -> (00:77:77:77:77:77 -> 00:66:66:66:66:66)")
            send_packet(self, self.devports[0], pkt1)
            verify_each_packet_on_each_port(self, [pkt1, pkt1, pkt1, tag_pkt1, tag_pkt1, tag_pkt1, tag_pkt1],
                [self.devports[1], self.devports[2], self.devports[3], self.devports[4], self.devports[5], self.devports[6], self.devports[7]])
            self.iuc += 1
            self.euc += 7

            # allow it to learn. Next set of packets should be unicast
            time.sleep(5)

            print("Sending packet port %d" % self.devports[
                1], " -> port %d" % self.devports[
                    0], " (00:66:66:66:66:66 -> 00:77:77:77:77:77)")
            send_packet(self, self.devports[1], pkt2)
            verify_packet(self, pkt2, self.devports[0])
            self.iuc += 1
            self.euc += 1

            print("Sending packet port %d" % self.devports[
                5], " -> port %d" % self.devports[
                    0], " (00:66:66:66:66:66 -> 00:77:77:77:77:77)")
            send_packet(self, self.devports[5], tag_pkt2)
            verify_packet(self, pkt2, self.devports[0])
            self.iuc += 1
            self.euc += 1

            # allow it to age. Next set of packets should be flooded
            time.sleep(30)

            print("Sending packet port %d" % self.devports[
                1], " -> (00:66:66:66:66:66 -> 00:77:77:77:77:77)")
            send_packet(self, self.devports[1], pkt2)
            verify_each_packet_on_each_port(self, [pkt2, pkt2, pkt2, tag_pkt2, tag_pkt2, tag_pkt2, tag_pkt2],
                [self.devports[0], self.devports[2], self.devports[3], self.devports[4], self.devports[5], self.devports[6], self.devports[7]])
            self.iuc += 1
            self.euc += 7

        finally:
            time.sleep(3)
            self.client.object_flush_all(SWITCH_OBJECT_TYPE_MAC_ENTRY)

    def MacMoveErrorTest(self):
        print("MacMoveErrorTest()")
        self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_AGING_INTERVAL, 20000)
        print("Add static mac 00:33:33:33:33:33 --> forw port 2")
        mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan10,
            mac_address='00:33:33:33:33:33', destination_handle=self.port2)
        print("Add static mac 00:44:44:44:44:44 --> forw port 6")
        mac1 = self.add_mac_entry(self.device, vlan_handle=self.vlan10,
            mac_address='00:44:44:44:44:44', destination_handle=self.port6)

        try:
            pkt = simple_udp_packet(
                eth_dst='00:33:33:33:33:33',
                eth_src='00:11:11:11:11:11',
                ip_dst='10.0.0.1',
                ip_ttl=64)
            utag_pkt = simple_udp_packet(
                eth_dst='00:44:44:44:44:44',
                eth_src='00:33:33:33:33:33',
                ip_dst='10.0.0.1',
                ip_ttl=64,
                pktlen=100)
            tag_pkt = simple_udp_packet(
                eth_dst='00:44:44:44:44:44',
                eth_src='00:33:33:33:33:33',
                ip_dst='10.0.0.1',
                dl_vlan_enable=True,
                vlan_vid=10,
                ip_ttl=64,
                pktlen=104)

            print("Test scenario 1 - static mac must not move/upd if same mac is learnt on another port")
            print("Tx packet [00:11:11:11:11:11 -> 00:33:33:33:33:33] from port 0 -> forw to port 2")
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, pkt, self.devports[2])
            time.sleep(3)
            self.iuc += 1
            self.euc += 1

            print("Tx packet [00:33:33:33:33:33 -> 00:44:44:44:44:44] from port 1 -> forw to port 6")
            send_packet(self, self.devports[1], utag_pkt)
            verify_packet(self, tag_pkt, self.devports[6])
            time.sleep(3)
            self.iuc += 1
            self.euc += 1
            print("Mac learning must not update/move static mac 00:33:33:33:33:33 from port 2 to port 1")

            print("Tx packet [00:11:11:11:11:11 -> 00:33:33:33:33:33] from port 0 -> continue forw to port 2")
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, pkt, self.devports[2])
            self.iuc += 1
            self.euc += 1

            '''
            # This requires fix to allow progarmming of static mac when the same mac has been learnt
            tag_pkt = simple_udp_packet(
                eth_dst='00:44:44:44:44:44',
                eth_src='00:55:55:55:55:55',
                ip_dst='10.0.0.1',
                dl_vlan_enable=True,
                vlan_vid=10,
                ip_ttl=64,
                pktlen=104)
            tag_pkt1 = simple_udp_packet(
                eth_dst='00:55:55:55:55:55',
                eth_src='00:11:11:11:11:11',
                ip_dst='10.0.0.1',
                dl_vlan_enable=True,
                vlan_vid=10,
                ip_ttl=64,
                pktlen=104)

            print("Test scenario 2 - Learn a mac, then program static mac for the same mac")
            print("Tx packet [00:55:55:55:55:55 -> 00:44:44:44:44:44] from port 5 -> forw to port 6")
            send_packet(self, self.devports[5], tag_pkt)
            verify_packets(self, tag_pkt, [self.devports[6]])
            time.sleep(3)
            print("Mac 00:55:55:55:55:55 learnt on port 5")

            print("Tx packet [00:11:11:11:11:11 -> 00:55:55:55:55:55] from port 4 -> forw to port 5")
            send_packet(self, self.devports[4], tag_pkt1)
            verify_packets(self, tag_pkt1, [self.devports[5]])

            print("Add static mac 00:55:55:55:55:55 --> forw port 6")
            mac1 = self.add_mac_entry(self.device, vlan_handle=self.vlan10,
                mac_address='00:55:55:55:55:55', destination_handle=self.port6)
            print("Learnt mac 00:55:55:55:55:55 will be updated as static mac, moves to port 6")

            print("Tx packet [00:11:11:11:11:11 -> 00:55:55:55:55:55] from port 4 -> forw to port 6")
            send_packet(self, self.devports[4], tag_pkt1)
            verify_packets(self, tag_pkt1, [self.devports[6]])
            '''

        finally:
            #self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.client.object_flush_all(SWITCH_OBJECT_TYPE_MAC_ENTRY)

    def MacLearnErrorTest(self):
        print("MacLearnErrorTest()")
        self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_AGING_INTERVAL, 30000)
        vlan_mbr5 = self.add_vlan_member(self.device, vlan_handle=self.vlan20, member_handle=self.port8, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
        vlan_mbr6 = self.add_vlan_member(self.device, vlan_handle=self.vlan20, member_handle=self.port9, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
        vlan_mbr7 = self.add_vlan_member(self.device, vlan_handle=self.vlan20, member_handle=self.port10, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)

        print("Add static mac 00:44:44:44:44:44 --> port 7")
        mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan10,
            mac_address='00:44:44:44:44:44', destination_handle=self.port7)

        print("Add static mac 00:55:55:55:55:55 --> port 8")
        mac1 = self.add_mac_entry(self.device, vlan_handle=self.vlan10,
            mac_address='00:55:55:55:55:55', destination_handle=self.port8)

        try:
            tag_pkt_invalid = simple_udp_packet(
                eth_dst='00:44:44:44:44:44',
                eth_src='00:88:88:88:88:88',
                ip_dst='10.0.0.1',
                dl_vlan_enable=True,
                vlan_vid=20,
                ip_ttl=64,
                pktlen=104)
            tag_pkt_valid = simple_udp_packet(
                eth_dst='00:44:44:44:44:44',
                eth_src='00:88:88:88:88:88',
                ip_dst='10.0.0.1',
                dl_vlan_enable=True,
                vlan_vid=10,
                ip_ttl=64,
                pktlen=104)
            tag_pkt1 = simple_udp_packet(
                eth_dst='00:88:88:88:88:88',
                eth_src='00:44:44:44:44:44',
                ip_dst='10.0.0.1',
                dl_vlan_enable=True,
                vlan_vid=10,
                ip_ttl=64,
                pktlen=104)
            pkt1 = simple_udp_packet(
                eth_dst='00:88:88:88:88:88',
                eth_src='00:44:44:44:44:44',
                ip_dst='10.0.0.1',
                ip_ttl=64,
                pktlen=100)
            vlan20_tag_pkt = simple_udp_packet(
                eth_dst='00:88:88:88:88:88',
                eth_src='00:55:55:55:55:55',
                ip_dst='10.0.0.1',
                dl_vlan_enable=True,
                vlan_vid=20,
                ip_ttl=64,
                pktlen=104)

            print("Tx packet [00:88:88:88:88:88 -> 00:44:44:44:44:44] from port 4 -> vlan member check fail, drop ")
            send_packet(self, self.devports[4], tag_pkt_invalid)
            verify_no_other_packets(self, timeout=1)
            time.sleep(3)
            print("Mac 00:88:88:88:88:88 must not be learnt on port 4 - vlan member check fail")

            print("Tx packet [00:44:44:44:44:44 -> 00:88:88:88:88:88] from port 7, vlan 10, flood ")
            send_packet(self, self.devports[7], tag_pkt1)
            verify_each_packet_on_each_port(self, [pkt1, pkt1, pkt1, pkt1, tag_pkt1, tag_pkt1, tag_pkt1],
                [self.devports[0], self.devports[1], self.devports[2], self.devports[3], self.devports[4], self.devports[5], self.devports[6]])
            self.iuc += 1
            self.euc += 7

            print("Tx packet [00:55:55:55:55:55 -> 00:88:88:88:88:88] from port 8, vlan 20, flood ")
            send_packet(self, self.devports[8], vlan20_tag_pkt)
            verify_packets(self, vlan20_tag_pkt, [self.devports[9], self.devports[10]])

            print("Tx packet [00:88:88:88:88:88 -> 00:44:44:44:44:44] from port 4 -> fwd to port 7")
            send_packet(self, self.devports[4], tag_pkt_valid)
            verify_packet(self, tag_pkt_valid, self.devports[7])
            time.sleep(3)
            self.iuc += 1
            self.euc += 1
            print("Mac 00:88:88:88:88:88 must be learnt on port 4 - member of vlan 20")

            print("Tx packet [00:44:44:44:44:44 -> 00:88:88:88:88:88] from port 7 -> port 4")
            send_packet(self, self.devports[7], tag_pkt1)
            verify_packet(self, tag_pkt1, self.devports[4])
            self.iuc += 1
            self.euc += 1

            print("Tx packet [00:88:88:88:88:88 -> 00:44:44:44:44:44] from port 6 -> vlan member check fail, drop")
            send_packet(self, self.devports[6], tag_pkt_invalid)
            verify_no_other_packets(self, timeout=1)
            time.sleep(3)
            print("Mac 00:88:88:88:88:88 must remain on port 4, must not move to port 6 ")

            print("Tx packet [00:44:44:44:44:44 -> 00:88:88:88:88:88] from port 7 -> port 4")
            send_packet(self, self.devports[7], tag_pkt1)
            verify_packet(self, tag_pkt1, self.devports[4])
            self.iuc += 1
            self.euc += 1

            # Verify if the multicast source mac is not learnt
            m_pkt = simple_udp_packet(
                eth_dst='00:44:44:44:44:44',
                eth_src='01:00:5e:11:11:11',
                ip_dst='10.0.0.1',
                dl_vlan_enable=True,
                vlan_vid=10,
                ip_ttl=64,
                pktlen=104)
            m_pkt1 = simple_udp_packet(
                eth_dst='01:00:5e:11:11:11',
                eth_src='00:44:44:44:44:44',
                ip_dst='10.0.0.1',
                dl_vlan_enable=True,
                vlan_vid=10,
                ip_ttl=64,
                pktlen=104)
            m_pkt2 = simple_udp_packet(
                eth_dst='01:00:5e:11:11:11',
                eth_src='00:44:44:44:44:44',
                ip_dst='10.0.0.1',
                ip_ttl=64,
                pktlen=100)

            print("Tx packet [01:00:5e:11:11:11 -> 00:44:44:44:44:44] from port 4 -> mcast src mac, drop")
            send_packet(self, self.devports[4], m_pkt)
            verify_no_other_packets(self, timeout=1)
            time.sleep(3)
            self.iuc += 1
            print("Mac 01:00:5e:11:11:11 must not be learnt on port 4 - mcast src mac ")

            # print("Tx packet [00:44:44:44:44:44 -> 01:00:5e:11:11:11] from port 7 -> flood")
            # send_packet(self, self.devports[7], m_pkt1)
            # verify_each_packet_on_each_port(self, [m_pkt2, m_pkt2, m_pkt2, m_pkt2, m_pkt1, m_pkt1, m_pkt1],
            #     [self.devports[0], self.devports[1], self.devports[2], self.devports[3], self.devports[4], self.devports[5], self.devports[6]])

        finally:
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.client.object_flush_all(SWITCH_OBJECT_TYPE_MAC_ENTRY)

    def VlanStatsTest(self):
        print("VlanStatsTest()")
        time.sleep(4)
        cntrs = self.client.object_counters_get(self.vlan10)
        # Lookup the unicast packet count
        for cntr in cntrs:
            if cntr.counter_id == SWITCH_VLAN_COUNTER_ID_IN_UCAST_PKTS:
                print(('Ingress: Expected: %d, Received: %d') % (self.iuc, cntr.count))
                self.assertEqual(cntr.count, self.iuc)
            if cntr.counter_id == SWITCH_VLAN_COUNTER_ID_OUT_UCAST_PKTS:
                print(('Egress:  Expected: %d, Received: %d') % (self.euc, cntr.count))
                self.assertEqual(cntr.count, self.euc)


###############################################################################
class L2LagMemberEgressDisable(ApiHelper):
    def runTest(self):
        print()
        self.configure()

        self.lag0 = self.add_lag(self.device)
        self.lag0_mbr0 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port1)
        self.lag0_mbr1 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port2)
        self.lag0_mbr2 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port3,
        egress_disable=True)

        try:
            self.BasicTest()
            self.LagMbrDisableMirrorTest()
        finally:
            self.cleanup()

    def BasicTest(self):
        print("BasicTest()")
        print("Verifying Basic Lag member disable functionality")
        try:
            print("Disabling (Egress) Lag member %x for Lag %x" %(self.lag0_mbr0, self.lag0))
            ret = self.attribute_set(self.lag0_mbr0, SWITCH_LAG_MEMBER_ATTR_EGRESS_DISABLE, True)
            self.assertTrue((ret.status == 0), "Lag member %x disable failed"%(self.lag0_mbr0))
            print("Disabling (Egress) Lag member %x for Lag %x" %(self.lag0_mbr1, self.lag0))
            ret = self.attribute_set(self.lag0_mbr1, SWITCH_LAG_MEMBER_ATTR_EGRESS_DISABLE, True)
            self.assertTrue((ret.status == 0), "Lag member %x disable failed"%(self.lag0_mbr1))
            print("Enabling (Egress) Lag member %x for Lag %x" %(self.lag0_mbr2, self.lag0))
            ret = self.attribute_set(self.lag0_mbr2, SWITCH_LAG_MEMBER_ATTR_EGRESS_DISABLE, False)
            self.assertTrue((ret.status == 0), "Lag member %x enable failed"%(self.lag0_mbr2))
        finally:
            self.attribute_set(self.lag0_mbr0, SWITCH_LAG_MEMBER_ATTR_EGRESS_DISABLE, False)
            self.attribute_set(self.lag0_mbr1, SWITCH_LAG_MEMBER_ATTR_EGRESS_DISABLE, False)
            self.attribute_set(self.lag0_mbr2, SWITCH_LAG_MEMBER_ATTR_EGRESS_DISABLE, True)

    def LagMbrDisableMirrorTest(self):
        print("LagMbrDisableMirrorTest()")
        if (self.client.is_feature_enable(SWITCH_FEATURE_INGRESS_MIRROR_ACL) == 0):
            print("MirrorAcl feature not enabled, skipping")
            return
        print("ACL mirroring from port0(%x) to lag0(%x)"%(self.port0, self.lag0))

        vlan10_mbr0 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.lag0)
        vlan10_mbr1 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port0)
        vlan10_mbr1 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port4)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.attribute_set(self.port4, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.attribute_set(self.lag0, SWITCH_LAG_ATTR_PORT_VLAN_ID, 10)

        mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:11:11:11:11:11', destination_handle=self.port4)
        mac1 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:22:22:22:22:22', destination_handle=self.port0)
        mac2 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:33:33:33:33:33',
        destination_handle=self.lag0)


        mirror = self.add_mirror(self.device,
            type=SWITCH_MIRROR_ATTR_TYPE_LOCAL,
            direction=SWITCH_MIRROR_ATTR_DIRECTION_INGRESS,
            egress_port_handle=self.lag0)
        mirror_p = self.add_mirror(self.device,
            type=SWITCH_MIRROR_ATTR_TYPE_LOCAL,
            direction=SWITCH_MIRROR_ATTR_DIRECTION_INGRESS,
            egress_port_handle=self.port4)
        acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IP_MIRROR,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        acl_entry = self.add_acl_entry(self.device,
            dst_ip='10.10.10.1',
            dst_ip_mask='255.255.255.255',
            action_ingress_mirror_handle=mirror,
            table_handle=acl_table)
        acl_entry_p = self.add_acl_entry(self.device,
            dst_ip='10.10.10.2',
            dst_ip_mask='255.255.255.255',
            action_ingress_mirror_handle=mirror_p,
            table_handle=acl_table)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, acl_table)
        pkt = simple_tcp_packet(
            eth_dst='00:11:11:11:11:11',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        pkt2 = simple_tcp_packet(
            eth_dst='00:33:33:33:33:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.10.10.2',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        try:
            active_lag_mbrs = {
                self.devports[1]:self.lag0_mbr0,
                self.devports[2]:self.lag0_mbr1
            }
            dev_port_list = list(active_lag_mbrs.keys())
            print("Sending packet port0 -> port4, Mirror to Lag0 (Active Members %s)"%
            ','.join(map(str, dev_port_list)))
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, pkt, self.devports[4])
            idx = verify_any_packet_any_port(self, [pkt], dev_port_list)

            print("Sending packet port0 -> Lag0 (Active Members %s), Mirror to port4"%','.join(map(str,
            dev_port_list)))
            send_packet(self, self.devports[0], pkt2)
            verify_packet(self, pkt2, self.devports[4])
            verify_any_packet_any_port(self, [pkt2], dev_port_list)

            print("Disabling (Egress) Lag member %x for Lag %x" %(active_lag_mbrs[dev_port_list[idx]], self.lag0))
            self.attribute_set(active_lag_mbrs[dev_port_list[idx]], SWITCH_LAG_MEMBER_ATTR_EGRESS_DISABLE, True)
            active_lag_mbrs.pop(dev_port_list[idx])
            dev_port_list = list(active_lag_mbrs.keys())

            print("Sending packet port0 -> port4, Mirror to Lag0 (Active Members %s)"%
            ','.join(map(str, dev_port_list)))
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, pkt, self.devports[4])
            idx = verify_any_packet_any_port(self, [pkt], dev_port_list)

            print("Disabling (Egress) Lag member %x for Lag %x" %(active_lag_mbrs[dev_port_list[idx]], self.lag0))
            self.attribute_set(active_lag_mbrs[dev_port_list[idx]], SWITCH_LAG_MEMBER_ATTR_EGRESS_DISABLE, True)
            active_lag_mbrs.pop(dev_port_list[idx])

            print("Sending packet port0 -> port4, Mirror to Lag0 (Active Members %s)"%
            ','.join(map(str, dev_port_list)))
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, pkt, self.devports[4])
            verify_no_other_packets(self, timeout=1)

            print("Sending packet port0 -> Lag0 (Active Members %s), Mirror to port4"%','.join(map(str,
            dev_port_list)))
            send_packet(self, self.devports[0], pkt2)
            verify_packet(self, pkt2, self.devports[4])
            verify_no_other_packets(self, timeout=1)

            print("Enabling (Egress) Lag member %x for Lag %x" %(self.lag0_mbr2, self.lag0))
            self.attribute_set(self.lag0_mbr2, SWITCH_LAG_MEMBER_ATTR_EGRESS_DISABLE, False)
            active_lag_mbrs[self.devports[3]] = self.lag0_mbr2
            dev_port_list = list(active_lag_mbrs.keys())

            print("Sending packet port0 -> port4, Mirror to Lag0 (Active Members %s)"%
            ','.join(map(str, dev_port_list)))
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, pkt, self.devports[4])
            idx = verify_any_packet_any_port(self, [pkt], dev_port_list)

            print("Sending packet port0 -> Lag0 (Active Members %s), Mirror to port4"%','.join(map(str,
            dev_port_list)))
            send_packet(self, self.devports[0], pkt2)
            verify_packet(self, pkt2, self.devports[4])
            idx = verify_any_packet_any_port(self, [pkt2], dev_port_list)
        finally:
            self.attribute_set(self.lag0_mbr0, SWITCH_LAG_MEMBER_ATTR_EGRESS_DISABLE, False)
            self.attribute_set(self.lag0_mbr1, SWITCH_LAG_MEMBER_ATTR_EGRESS_DISABLE, False)
            self.attribute_set(self.lag0_mbr2, SWITCH_LAG_MEMBER_ATTR_EGRESS_DISABLE, True)
            # VLAN, ACL and MIRROR cleanup
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.port4, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.lag0, SWITCH_LAG_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

###############################################################################
@group('l2')
@group('lag')
class L2LagTest(ApiHelper):
    '''
    This performs basic LAG testing
        lag0 with port1, port2, port3, port4
        lag1 with port5, port6
    Packets are sent from port0
    PV checking and tagging test
        lag2 with port11 and port12
        lag3 with port11
    Packets are sent from port9 and port10
    @test - LagBasicTest
        200 packets from port0 are balanced across lag0 members
    @test - LagFloodingTest
        lag0 and lag1 are vlan members along with port0, port7 and port8
        Flooding is verified across the lags and individual ports
    @twst - LagPVCheckTest
    '''
    def setUp(self):
        print()
        self.configure()

        self.lag0 = self.add_lag(self.device)
        self.lag0_mbr0 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port1)
        self.lag0_mbr1 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port2)
        self.lag0_mbr2 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port3)
        self.lag0_mbr3 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port4)

        self.lag1 = self.add_lag(self.device)
        self.lag1_mbr1 = self.add_lag_member(self.device, lag_handle=self.lag1, port_handle=self.port5,
        ingress_disable=True)
        self.lag1_mbr2 = self.add_lag_member(self.device, lag_handle=self.lag1, port_handle=self.port6)

        vlan_mbr0 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port0)
        vlan_mbr1 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.lag0)
        vlan_mbr2 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.lag1)
        vlan_mbr3 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port7)
        vlan_mbr4 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port8)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.attribute_set(self.lag0, SWITCH_LAG_ATTR_PORT_VLAN_ID, 10)
        self.attribute_set(self.lag1, SWITCH_LAG_ATTR_PORT_VLAN_ID, 10)
        self.attribute_set(self.port7, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.attribute_set(self.port8, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)

        vlan_mbr5 = self.add_vlan_member(self.device, vlan_handle=self.vlan20, member_handle=self.port9, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
        vlan_mbr6 = self.add_vlan_member(self.device, vlan_handle=self.vlan30, member_handle=self.port9, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
        vlan_mbr7 = self.add_vlan_member(self.device, vlan_handle=self.vlan40, member_handle=self.port10)
        self.attribute_set(self.port10, SWITCH_PORT_ATTR_PORT_VLAN_ID, 40)

        self.lag2 = self.add_lag(self.device)
        self.lag3 = self.add_lag(self.device)

        mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:11:11:11:11:11', destination_handle=self.port0)
        mac1 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:22:22:22:22:22', destination_handle=self.lag0)

        self.ing_vlan10_uc = 0
        self.eg_vlan10_uc = 0
        self.ing_vlan10_bc = 0
        self.eg_vlan10_bc = 0

    def runTest(self):
        try:
            self.LagBasicTest()
            self.LagMemberActivateFloodTest()
            self.LagMemberActivateBridgeTest()
            self.LagLearnFlagTest()
            self.LagMacLearnTest()
            self.LagFloodTest()
            self.LagFloodPruneTest()
            self.LagStatsTest()
            self.LagPVMissTest()
            self.AccessModeTest()
            self.TrunkModeTest()
            # self.LagQoSGroupTest()  # qos-map supp only on port
            self.LagMemberIngressDisableTest()
        finally:
            pass

    def tearDown(self):
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.lag0, SWITCH_LAG_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.lag1, SWITCH_LAG_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.port7, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.port8, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.port10, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.cleanup()

    def LagMemberIngressDisableTest(self):
        print("LagMemberIngressDisable()")
        try:
            pkt = simple_arp_packet(arp_op=1, pktlen=100)
            lag0 = [self.devports[1], self.devports[2], self.devports[3], self.devports[4]]

            print("Sending Packet from Lag1 member 2 (ingress enabled) to Lag0, port7 and port8")
            send_packet(self, self.devports[6], pkt)
            verify_packets_on_multiple_port_lists(self, [pkt] * 4, [
                lag0, [self.devports[0]], [self.devports[7]], [self.devports[8]]])

            print("Sending Packet from Lag1 member 1 (ingress disabled) to Lag0, port7 and port8")
            send_packet(self, self.devports[5], pkt)
            verify_no_other_packets(self, timeout=1)

            print("Disabling Ingress on port 6 i.e lag1 member 2")
            self.attribute_set(self.lag1_mbr2, SWITCH_LAG_MEMBER_ATTR_INGRESS_DISABLE, True)
            print("Sending Packet from Lag1 member 2 to Lag0, port7 and port8")
            send_packet(self, self.devports[6], pkt)
            verify_no_other_packets(self, timeout=1)
            print("Sending Packet from Lag1 member 1 to Lag0, port7 and port8")
            send_packet(self, self.devports[5], pkt)
            verify_no_other_packets(self, timeout=1)

            print("Enabling Ingress on port 6 i.e lag1 member 2")
            self.attribute_set(self.lag1_mbr2, SWITCH_LAG_MEMBER_ATTR_INGRESS_DISABLE, False)
            print("Sending Packet from Lag1 member 2 to Lag0, port7 and port8")
            send_packet(self, self.devports[6], pkt)
            verify_packets_on_multiple_port_lists(self, [pkt] * 4, [
                lag0, [self.devports[0]], [self.devports[7]], [self.devports[8]]])
            print("Sending Packet from Lag1 member 1 to Lag0, port7 and port8")
            send_packet(self, self.devports[5], pkt)
            verify_no_other_packets(self, timeout=1)

            print("Enabling Ingress on port 5 i.e lag1 member 1")
            self.attribute_set(self.lag1_mbr1, SWITCH_LAG_MEMBER_ATTR_INGRESS_DISABLE, False)
            print("Sending Packet from Lag1 port 6 to Lag0, port7 and port8")
            send_packet(self, self.devports[6], pkt)
            verify_packets_on_multiple_port_lists(self, [pkt] * 4, [
                lag0, [self.devports[0]], [self.devports[7]], [self.devports[8]]])
            print("Sending Packet from Lag1 port 5 to Lag0, port7 and port8")
            send_packet(self, self.devports[5], pkt)
            verify_packets_on_multiple_port_lists(self, [pkt] * 4, [
                lag0, [self.devports[0]], [self.devports[7]], [self.devports[8]]])
        finally:
            self.attribute_set(self.lag1_mbr1, SWITCH_LAG_MEMBER_ATTR_INGRESS_DISABLE, True)
            self.attribute_set(self.lag1_mbr2, SWITCH_LAG_MEMBER_ATTR_INGRESS_DISABLE, False)

    def LagBasicTest(self):
        print("LagBasicTest()")
        print("Lag basic test with 200 packets")

        try:
            count = [0, 0, 0, 0]
            dst_ip = int(binascii.hexlify(socket.inet_aton('10.10.10.1')), 16)
            src_ip = int(binascii.hexlify(socket.inet_aton('192.168.8.1')), 16)
            max_itrs = 200
            pkt = simple_tcp_packet(
                    eth_src='00:11:11:11:11:11',
                    eth_dst='00:22:22:22:22:22',
                    ip_dst='10.10.10.1',
                    ip_src='192.168.8.1',
                    ip_id=109,
                    ip_ttl=64)
            for i in range(0, max_itrs):
                dst_ip_addr = socket.inet_ntoa(
                    binascii.unhexlify(format(dst_ip, 'x').zfill(8)))
                src_ip_addr = socket.inet_ntoa(
                    binascii.unhexlify(format(src_ip, 'x').zfill(8)))
                pkt['IP'].dst = dst_ip_addr
                pkt['IP'].src = src_ip_addr

                send_packet(self, self.devports[0], pkt)
                rcv_idx = verify_any_packet_any_port(
                    self, [pkt, pkt, pkt, pkt],
                    [self.devports[1], self.devports[2], self.devports[3], self.devports[4]])
                count[rcv_idx] += 1
                dst_ip += 1
                src_ip += 1

            self.ing_vlan10_uc += max_itrs
            self.eg_vlan10_uc += max_itrs

            print('L2 Basic Test:', count)
            for i in range(0, 4):
                self.assertTrue((count[i] >= ((max_itrs / 4) * 0.6)),
                                "Not all paths are equally balanced")

            pkt = simple_tcp_packet(
                eth_dst='00:11:11:11:11:11',
                eth_src='00:22:22:22:22:22',
                ip_dst='10.0.0.1',
                ip_id=109,
                ip_ttl=64)
            exp_pkt = simple_tcp_packet(
                eth_dst='00:11:11:11:11:11',
                eth_src='00:22:22:22:22:22',
                ip_dst='10.0.0.1',
                ip_id=109,
                ip_ttl=64)
            print("Sending packet port %d" % self.devports[
                1], " (lag member) -> port %d" % self.devports[0], "")
            send_packet(self, self.devports[1], pkt)
            verify_packet(self, exp_pkt, self.devports[0])
            print("Sending packet port %d" % self.devports[
                2], " (lag member) -> port %d" % self.devports[0], "")
            send_packet(self, self.devports[2], pkt)
            verify_packet(self, exp_pkt, self.devports[0])
            print("Sending packet port %d" % self.devports[
                3], " (lag member) -> port %d" % self.devports[0], "")
            send_packet(self, self.devports[3], pkt)
            verify_packet(self, exp_pkt, self.devports[0])
            print("Sending packet port %d" % self.devports[
                4], " (lag member) -> port %d" % self.devports[0], "")
            send_packet(self, self.devports[4], pkt)
            verify_packet(self, exp_pkt, self.devports[0])
            self.ing_vlan10_uc += 4
            self.eg_vlan10_uc += 4
        finally:
            pass

    def LagMemberActivateBridgeTest(self):
        print("LagMemberActivateBridgeTest()")

        try:
            max_itrs = 48
            def packetTest():
                pkt = simple_tcp_packet(
                    eth_src='00:11:11:11:11:11',
                    eth_dst='00:22:22:22:22:22',
                    ip_dst='10.10.10.1',
                    ip_src='192.168.8.1',
                    ip_id=109,
                    ip_ttl=64)
                exp_pkt = simple_tcp_packet(
                    eth_src='00:11:11:11:11:11',
                    eth_dst='00:22:22:22:22:22',
                    ip_dst='10.10.10.1',
                    ip_src='192.168.8.1',
                    ip_id=109,
                    ip_ttl=64)
                count = [0, 0, 0, 0]
                dst_ip = int(binascii.hexlify(socket.inet_aton('10.10.10.1')), 16)
                src_ip = int(binascii.hexlify(socket.inet_aton('192.168.8.1')), 16)
                for i in range(0, max_itrs):
                    dst_ip_addr = socket.inet_ntoa(
                        binascii.unhexlify(format(dst_ip, 'x').zfill(8)))
                    src_ip_addr = socket.inet_ntoa(
                        binascii.unhexlify(format(src_ip, 'x').zfill(8)))
                    pkt['IP'].src = src_ip_addr
                    pkt['IP'].dst = dst_ip_addr
                    exp_pkt['IP'].src = src_ip_addr
                    exp_pkt['IP'].dst = dst_ip_addr
                    send_packet(self, self.devports[0], pkt)
                    rcv_idx = verify_any_packet_any_port(
                        self, [exp_pkt, exp_pkt, exp_pkt, exp_pkt],
                        [self.devports[1], self.devports[2], self.devports[3], self.devports[4]])
                    count[rcv_idx] += 1
                    dst_ip += 1
                    src_ip += 1
                self.ing_vlan10_uc += max_itrs
                self.eg_vlan10_uc += max_itrs
                return count

            count = packetTest()
            print('Test with 4 mbrs enabled:', count)
            for i in range(0, 4):
                self.assertTrue((count[i] >= ((max_itrs / 4) * 0.5)),
                                "Not all paths are equally balanced")
            self.attribute_set(self.lag0_mbr3, SWITCH_LAG_MEMBER_ATTR_EGRESS_DISABLE, True)
            count = packetTest()
            print('Test with 3 mbrs enabled after 4th member disable:', count)
            for i in range(0, 3):
                self.assertTrue((count[i] >= ((max_itrs / 3) * 0.5)),
                                "Not all paths are equally balanced")
            self.assertEqual(count[3], 0)
            self.attribute_set(self.lag0_mbr2, SWITCH_LAG_MEMBER_ATTR_EGRESS_DISABLE, True)
            count = packetTest()
            print('Test with 2 mbrs enabled after 3th member disable:', count)
            for i in range(0, 2):
                self.assertTrue((count[i] >= ((max_itrs / 2) * 0.5)),
                                "Not all paths are equally balanced")
            self.assertEqual(count[2], 0)
            self.assertEqual(count[3], 0)
            self.attribute_set(self.lag0_mbr3, SWITCH_LAG_MEMBER_ATTR_EGRESS_DISABLE, False)
            count = packetTest()
            print('Test with 3 mbrs enabled after 4th member enable:', count)
            for i in [0,1,3]:
                self.assertTrue((count[i] >= ((max_itrs / 3) * 0.5)),
                                "Not all paths are equally balanced")
            self.assertEqual(count[2], 0)
            self.attribute_set(self.lag0_mbr2, SWITCH_LAG_MEMBER_ATTR_EGRESS_DISABLE, False)
            count = packetTest()
            print('Test with 4 mbrs enabled after 3th member enable:', count)
            for i in range(0, 4):
                self.assertTrue((count[i] >= ((max_itrs / 4) * 0.5)),
                                "Not all paths are equally balanced")
        finally:
            pass

    def LagMemberActivateFloodTest(self):
        print("LagMemberActivateFloodTest()")

        try:
            l1 = [self.devports[1], self.devports[2], self.devports[3], self.devports[4]]
            l2 = [self.devports[5], self.devports[6]]
            max_itrs = 4
            def packetTest():
                pkt = simple_arp_packet(
                    arp_op=1,
                    pktlen=100,
                    eth_src='00:11:11:11:11:11',
                    hw_snd='00:11:11:11:11:11',
                    ip_snd='10.10.10.1',
                    ip_tgt='10.10.10.2')
                count = [0, 0, 0, 0]
                for i in range(0, max_itrs):
                    send_packet(self, self.devports[0], pkt)
                    rcv_idx = verify_packets_on_multiple_port_lists(self, [pkt] * 4, [
                        l1, l2, [self.devports[7]], [self.devports[8]]])
                    for v in rcv_idx[0]:
                        count[v] += 1
                self.ing_vlan10_bc += max_itrs
                self.eg_vlan10_bc += (max_itrs * 4)
                return count

            mbrs = {0: self.lag0_mbr0, 1: self.lag0_mbr1, 2: self.lag0_mbr2, 3: self.lag0_mbr3}
            disabled_mbrs = set()
            # for each iteration
            #  - check if packets are recieved on only lag member
            #  - check if packets are not recieved on disabled lag member
            count = packetTest()
            self.assertEqual(len([i for i in count if i != 0]), 1)
            print('Test with 4 mbrs enabled:', count)
            index = [i for i in range(len(count)) if count[i] != 0]
            self.attribute_set(mbrs[index[0]], SWITCH_LAG_MEMBER_ATTR_EGRESS_DISABLE, True)
            print(('Disabled Lag member %d') % (index[0]))
            disabled_mbrs.add(index[0])

            count = packetTest()
            self.assertEqual(len([i for i in count if i != 0]), 1)
            print('Test with 3 mbrs enabled:', count)
            index = [i for i in range(len(count)) if count[i] != 0]
            self.assertTrue(index[0] not in disabled_mbrs)
            self.attribute_set(mbrs[index[0]], SWITCH_LAG_MEMBER_ATTR_EGRESS_DISABLE, True)
            print(('Disabled Lag member %d') % (index[0]))
            disabled_mbrs.add(index[0])

            count = packetTest()
            self.assertEqual(len([i for i in count if i != 0]), 1)
            print('Test with 2 mbrs enabled:', count)
            index = [i for i in range(len(count)) if count[i] != 0]
            self.assertTrue(index[0] not in disabled_mbrs)
            self.attribute_set(mbrs[index[0]], SWITCH_LAG_MEMBER_ATTR_EGRESS_DISABLE, True)
            print(('Disabled Lag member %d') % (index[0]))
            disabled_mbrs.add(index[0])

            count = packetTest()
            self.assertEqual(len([i for i in count if i != 0]), 1)
            print('Test with 1 mbrs enabled:', count)
            index = [i for i in range(len(count)) if count[i] != 0]
            self.assertTrue(index[0] not in disabled_mbrs)
        finally:
            for mbr in list(mbrs.values()):
                self.attribute_set(mbr, SWITCH_LAG_MEMBER_ATTR_EGRESS_DISABLE, False)

    def LagMacLearnTest(self):
        print("LagMacLearnTest()")
        self.attribute_set(self.lag0, SWITCH_LAG_ATTR_LEARNING, True)
        self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_LEARNING, True)
        self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_AGING_INTERVAL, 10000)
        pkt1 = simple_udp_packet(
            eth_dst='00:11:11:11:11:11', eth_src='00:66:66:66:66:66',
            ip_dst='10.0.0.1', ip_ttl=64)
        pkt2 = simple_udp_packet(
            eth_dst='00:11:11:11:11:11', eth_src='00:33:33:33:33:33',
            ip_dst='10.0.0.1', ip_ttl=64)
        pkt3 = simple_udp_packet(
            eth_dst='00:11:11:11:11:11', eth_src='00:44:44:44:44:44',
            ip_dst='10.0.0.1', ip_ttl=64)
        pkt4 = simple_udp_packet(
            eth_dst='00:11:11:11:11:11', eth_src='00:55:55:55:55:55',
            ip_dst='10.0.0.1', ip_ttl=64)

        try:
            print("Sending packet lag port %d" % self.devports[1], "-> port %d" % self.devports[
                0], " (00:66:66:66:66:66 -> 00:11:11:11:11:11)")
            send_packet(self, self.devports[1], pkt1)
            verify_packet(self, pkt1, self.devports[0])
            print("Sending packet lag port %d" % self.devports[2], "-> port %d" % self.devports[
                0], " (00:33:33:33:33:33 -> 00:11:11:11:11:11)")
            send_packet(self, self.devports[2], pkt2)
            verify_packet(self, pkt2, self.devports[0])
            print("Sending packet lag port %d" % self.devports[3], "-> port %d" % self.devports[
                0], " (00:44:44:44:44:44 -> 00:11:11:11:11:11)")
            send_packet(self, self.devports[3], pkt3)
            verify_packet(self, pkt3, self.devports[0])
            print("Sending packet lag port %d" % self.devports[4], "-> port %d" % self.devports[
                0], " (00:55:55:55:55:55 -> 00:11:11:11:11:11)")
            send_packet(self, self.devports[4], pkt4)
            verify_packet(self, pkt4, self.devports[0])
            time.sleep(5)
            print("MACs now learnt on all 4 ports of lag0")
            pkt = simple_udp_packet(
                eth_dst='00:66:66:66:66:66', eth_src='00:11:11:11:11:11',
                ip_dst='10.0.0.1', ip_ttl=64)
            print("Sending packet port %d" % self.devports[0], "-> lag0 (00:11:11:11:11:11 -> 00:66:66:66:66:66)")
            send_packet(self, self.devports[0], pkt)
            verify_any_packet_any_port(self, [pkt, pkt, pkt, pkt],
                    [self.devports[1], self.devports[2], self.devports[3], self.devports[4]])
            pkt = simple_udp_packet(
                eth_dst='00:33:33:33:33:33', eth_src='00:11:11:11:11:11',
                ip_dst='10.0.0.1', ip_ttl=64)
            print("Sending packet port %d" % self.devports[0], "-> lag0 (00:11:11:11:11:11 -> 00:33:33:33:33:33)")
            send_packet(self, self.devports[0], pkt)
            verify_any_packet_any_port(self, [pkt, pkt, pkt, pkt],
                    [self.devports[1], self.devports[2], self.devports[3], self.devports[4]])
            pkt = simple_udp_packet(
                eth_dst='00:44:44:44:44:44', eth_src='00:11:11:11:11:11',
                ip_dst='10.0.0.1', ip_ttl=64)
            print("Sending packet port %d" % self.devports[0], "-> lag0 (00:11:11:11:11:11 -> 00:44:44:44:44:44)")
            send_packet(self, self.devports[0], pkt)
            verify_any_packet_any_port(self, [pkt, pkt, pkt, pkt],
                    [self.devports[1], self.devports[2], self.devports[3], self.devports[4]])
            pkt = simple_udp_packet(
                eth_dst='00:55:55:55:55:55', eth_src='00:11:11:11:11:11',
                ip_dst='10.0.0.1', ip_ttl=64)
            print("Sending packet port %d" % self.devports[0], "-> lag0 (00:11:11:11:11:11 -> 00:55:55:55:55:55)")
            send_packet(self, self.devports[0], pkt)
            verify_any_packet_any_port(self, [pkt, pkt, pkt, pkt],
                    [self.devports[1], self.devports[2], self.devports[3], self.devports[4]])
            self.ing_vlan10_uc += 8
            self.eg_vlan10_uc += 8
        finally:
            print("Waiting 15s for entries to age out")
            time.sleep(15)
            self.attribute_set(self.lag0, SWITCH_LAG_ATTR_LEARNING, False)
            self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_LEARNING, True)

    def LagLearnFlagTest(self):
        print("LagLearnFlagTest()")
        self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_LEARNING, True)
        self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_AGING_INTERVAL, 10000)
        pkt1 = simple_udp_packet(
            eth_dst='00:11:11:11:11:11', eth_src='00:66:66:66:66:66',
            ip_dst='10.0.0.1', ip_ttl=64)
        pkt2 = simple_udp_packet(
            eth_dst='00:11:11:11:11:11', eth_src='00:33:33:33:33:33',
            ip_dst='10.0.0.1', ip_ttl=64)
        l1 = [self.devports[1], self.devports[2], self.devports[3], self.devports[4]]
        l2 = [self.devports[5], self.devports[6]]

        try:
            print("LAG learning disabled, no digests should be generated")
            print("Sending packet lag port %d" % self.devports[1], "-> port %d" % self.devports[
                0], " (00:66:66:66:66:66 -> 00:11:11:11:11:11)")
            send_packet(self, self.devports[1], pkt1)
            verify_packet(self, pkt1, self.devports[0])
            time.sleep(1)
            print("Sending packet lag port %d" % self.devports[2], "-> port %d" % self.devports[
                0], " (00:33:33:33:33:33 -> 00:11:11:11:11:11)")
            send_packet(self, self.devports[2], pkt2)
            verify_packet(self, pkt2, self.devports[0])
            time.sleep(1)
            print("MACs not learnt on any ports of lag0")
            pkt = simple_udp_packet(
                eth_dst='00:66:66:66:66:66', eth_src='00:11:11:11:11:11',
                ip_dst='10.0.0.1', ip_ttl=64)
            print("Sending packet port %d" % self.devports[0], "-> lag0 (00:11:11:11:11:11 -> 00:66:66:66:66:66), flooded")
            send_packet(self, self.devports[0], pkt)
            verify_packets_on_multiple_port_lists(self, [pkt] * 4, [
                l1, l2, [self.devports[7]], [self.devports[8]]])
            time.sleep(1)
            pkt = simple_udp_packet(
                eth_dst='00:33:33:33:33:33', eth_src='00:11:11:11:11:11',
                ip_dst='10.0.0.1', ip_ttl=64)
            print("Sending packet port %d" % self.devports[0], "-> lag0 (00:11:11:11:11:11 -> 00:33:33:33:33:33), flooded")
            send_packet(self, self.devports[0], pkt)
            verify_packets_on_multiple_port_lists(self, [pkt] * 4, [
                l1, l2, [self.devports[7]], [self.devports[8]]])
            time.sleep(1)

            print("LAG learning enabled")
            self.attribute_set(self.lag0, SWITCH_LAG_ATTR_LEARNING, True)
            print("Sending packet lag port %d" % self.devports[1], "-> port %d" % self.devports[
                0], " (00:66:66:66:66:66 -> 00:11:11:11:11:11)")
            send_packet(self, self.devports[1], pkt1)
            verify_packet(self, pkt1, self.devports[0])
            time.sleep(1)
            print("Sending packet lag port %d" % self.devports[2], "-> port %d" % self.devports[
                0], " (00:33:33:33:33:33 -> 00:11:11:11:11:11)")
            send_packet(self, self.devports[2], pkt2)
            verify_packet(self, pkt2, self.devports[0])
            time.sleep(1)
            print("MACs now learnt on 2 ports of lag0")
            pkt = simple_udp_packet(
                eth_dst='00:66:66:66:66:66', eth_src='00:11:11:11:11:11',
                ip_dst='10.0.0.1', ip_ttl=64)
            print("Sending packet port %d" % self.devports[0], "-> lag0 (00:11:11:11:11:11 -> 00:66:66:66:66:66)")
            send_packet(self, self.devports[0], pkt)
            verify_any_packet_any_port(self, [pkt, pkt, pkt, pkt],
                    [self.devports[1], self.devports[2], self.devports[3], self.devports[4]])
            pkt = simple_udp_packet(
                eth_dst='00:33:33:33:33:33', eth_src='00:11:11:11:11:11',
                ip_dst='10.0.0.1', ip_ttl=64)
            print("Sending packet port %d" % self.devports[0], "-> lag0 (00:11:11:11:11:11 -> 00:33:33:33:33:33)")
            send_packet(self, self.devports[0], pkt)
            verify_any_packet_any_port(self, [pkt, pkt, pkt, pkt],
                    [self.devports[1], self.devports[2], self.devports[3], self.devports[4]])
            self.ing_vlan10_uc += 8
            self.eg_vlan10_uc += 14
        finally:
            print("Waiting 30s for entries to age out")
            time.sleep(30)
            self.attribute_set(self.lag0, SWITCH_LAG_ATTR_LEARNING, False)
            self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_LEARNING, True)

    def LagFloodTest(self):
        print("LagFloodtest()")
        self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_LEARNING, False)
        try:
            pkt = simple_tcp_packet(
                eth_src='00:77:77:77:77:77',
                eth_dst='00:66:66:66:66:66',
                ip_dst='10.0.0.1',
                ip_id=107,
                ip_ttl=64)
            exp_pkt = simple_tcp_packet(
                eth_src='00:77:77:77:77:77',
                eth_dst='00:66:66:66:66:66',
                ip_dst='10.0.0.1',
                ip_id=107,
                ip_ttl=64)

            l0 = [self.devports[1], self.devports[2], self.devports[3], self.devports[4]]
            l1 = [self.devports[5], self.devports[6]]
            print("Sending packet from port0 -> lag0, lag1, port7, port8")
            send_packet(self, self.devports[0], pkt)
            verify_packets_on_multiple_port_lists(self, [exp_pkt] * 4, [
                l0, l1, [self.devports[7]], [self.devports[8]]])
            print("Sending packet from lag0/port1 -> port0, lag1, port7, port8")
            send_packet(self, self.devports[1], pkt)
            verify_packets_on_multiple_port_lists(self, [exp_pkt] * 4, [
                [self.devports[0]], l1, [self.devports[7]], [self.devports[8]]])
            print("Sending packet from lag0/port2 -> port0, lag1, port7, port8")
            send_packet(self, self.devports[2], pkt)
            verify_packets_on_multiple_port_lists(self, [exp_pkt] * 4, [
                [self.devports[0]], l1, [self.devports[7]], [self.devports[8]]])
            print("Sending packet from lag0/port3 -> port0, lag1, port7, port8")
            send_packet(self, self.devports[3], pkt)
            verify_packets_on_multiple_port_lists(self, [exp_pkt] * 4, [
                [self.devports[0]], l1, [self.devports[7]], [self.devports[8]]])
            print("Sending packet from lag0/port4 -> port0, lag1, port7, port8")
            send_packet(self, self.devports[4], pkt)
            verify_packets_on_multiple_port_lists(self, [exp_pkt] * 4, [
                [self.devports[0]], l1, [self.devports[7]], [self.devports[8]]])
            # Lag1/Port5 is in ingress disable state so skip it
            print("Sending packet from lag1/port6 -> port0, lag0, port7, port8")
            send_packet(self, self.devports[6], pkt)
            verify_packets_on_multiple_port_lists(self, [exp_pkt] * 4, [
                [self.devports[0]], l0, [self.devports[7]], [self.devports[8]]])
            print("Sending packet from port7 -> port0, lag0, lag1, port8")
            send_packet(self, self.devports[7], pkt)
            verify_packets_on_multiple_port_lists(self, [exp_pkt] * 4, [
                l0, [self.devports[0]], l1, [self.devports[8]]])
            print("Sending packet from port8 -> port0, lag0, lag1, port7")
            send_packet(self, self.devports[8], pkt)
            verify_packets_on_multiple_port_lists(self, [exp_pkt] * 4, [
                l0, [self.devports[0]], l1, [self.devports[7]]])

            self.ing_vlan10_uc += 8
            self.eg_vlan10_uc += 32
        finally:
            self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_LEARNING, True)
            pass

    def LagFloodPruneTest(self):
        print("LagFloodPruneTest()")
        print("Lag flood test with members added after vlan_member created")
        self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_LEARNING, False)
        try:
            pkt = simple_tcp_packet(
                eth_dst='00:66:66:66:66:66',
                ip_dst='10.0.0.1',
                ip_id=107,
                ip_ttl=64)
            exp_pkt = simple_tcp_packet(
                eth_dst='00:66:66:66:66:66',
                ip_dst='10.0.0.1',
                ip_id=107,
                ip_ttl=64)

            l0 = [self.devports[1], self.devports[2], self.devports[3], self.devports[4]]
            l1 = [self.devports[5], self.devports[6]]
            print("Sending packet from port0 -> lag0, lag1, port7, port8")
            send_packet(self, self.devports[0], pkt)
            verify_packets_on_multiple_port_lists(self, [exp_pkt] * 4, [
                l0, l1, [self.devports[7]], [self.devports[8]]])

            print("Add new lag with no members to vlan 10. Packet output will not change")
            self.lag4 = self.add_lag(self.device)
            vlan_mbr6 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.lag4)
            self.attribute_set(self.lag4, SWITCH_LAG_ATTR_PORT_VLAN_ID, 10)
            print("Sending packet from port0 -> lag0, lag1, port7, port8")
            send_packet(self, self.devports[0], pkt)
            verify_packets_on_multiple_port_lists(self, [exp_pkt] * 4, [
                l0, l1, [self.devports[7]], [self.devports[8]]])

            print("Add port %d to lag4" % (self.devports[11]))
            lag_mbr4 = self.add_lag_member(self.device, lag_handle=self.lag4, port_handle=self.port11)
            l4 = [self.devports[11]]
            print("Sending packet from port0 -> lag0, lag1, lag4, port7, port8")
            send_packet(self, self.devports[0], pkt)
            verify_packets_on_multiple_port_lists(self, [exp_pkt] * 5, [
                l0, l1, l4, [self.devports[7]], [self.devports[8]]])
            print("Sending packet from port11 -> lag0, lag1, port7, port8, port0")
            send_packet(self, self.devports[11], pkt)
            verify_packets_on_multiple_port_lists(self, [exp_pkt] * 5, [
                l0, l1, [self.devports[7]], [self.devports[8]], [self.devports[0]]])

            print("Add port %d to lag4" % (self.devports[12]))
            lag_mbr5 = self.add_lag_member(self.device, lag_handle=self.lag4, port_handle=self.port12)
            l4 = [self.devports[11], self.devports[12]]
            print("Sending packet from port0 -> lag0, lag1, lag4, port7, port8")
            send_packet(self, self.devports[0], pkt)
            verify_packets_on_multiple_port_lists(self, [exp_pkt] * 5, [
                l0, l1, l4, [self.devports[7]], [self.devports[8]]])
            print("Sending packet from port12 -> lag0, lag1, port7, port8, port0")
            send_packet(self, self.devports[12], pkt)
            verify_packets_on_multiple_port_lists(self, [exp_pkt] * 5, [
                l0, l1, [self.devports[7]], [self.devports[8]], [self.devports[0]]])

            print("Remove port %d from lag4" % (self.devports[12]))
            self.cleanlast()
            l4 = [self.devports[11]]
            print("Sending packet from port0 -> lag0, lag1, lag4, port7, port8")
            send_packet(self, self.devports[0], pkt)
            verify_packets_on_multiple_port_lists(self, [exp_pkt] * 5, [
                l0, l1, l4, [self.devports[7]], [self.devports[8]]])

            print("Add port %d to vlan 10. Verify if it is now part of flood list" % (self.devports[12]))
            vlan_mbr6 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port12)
            self.attribute_set(self.port12, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
            l4 = [self.devports[11]]
            print("Sending packet from port0 -> lag0, lag1, lag4, port7, port8, port12")
            send_packet(self, self.devports[0], pkt)
            verify_packets_on_multiple_port_lists(self, [exp_pkt] * 6, [
                l0, l1, l4, [self.devports[7]], [self.devports[8]], [self.devports[12]]])
            print("Sending packet from port12 -> lag0, lag1, lag4, port7, port8, port0")
            send_packet(self, self.devports[12], pkt)
            verify_packets_on_multiple_port_lists(self, [exp_pkt] * 6, [
                l0, l1, l4, [self.devports[7]], [self.devports[8]], [self.devports[0]]])

            self.attribute_set(self.port12, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.cleanlast() # remove port 12 vlan member
            self.cleanlast() # remove port 11 lag member
            print("Remove port %d from lag4. No more packets should be seen now" % (self.devports[11]))
            print("Sending packet from port0 -> lag0, lag1, port7, port8")
            send_packet(self, self.devports[0], pkt)
            verify_packets_on_multiple_port_lists(self, [exp_pkt] * 4, [
                l0, l1, [self.devports[7]], [self.devports[8]]])

            self.ing_vlan10_uc += 10
            self.eg_vlan10_uc += 49
        finally:
            self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_LEARNING, True)
            self.attribute_set(self.lag4, SWITCH_LAG_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.port12, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.cleanlast()
            self.cleanlast()

    def LagPVMissTest(self):
        print("LagPVMissTest()")
        print("Test LAG memnbers PV miss")
        mac2 = self.add_mac_entry(self.device, vlan_handle=self.vlan20, mac_address='00:77:77:77:77:77', destination_handle=self.port9)
        mac3 = self.add_mac_entry(self.device, vlan_handle=self.vlan30, mac_address='00:88:88:88:88:88', destination_handle=self.port9)
        mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan20, mac_address='00:99:99:99:99:99', destination_handle=self.lag2)
        vlan_mbr10 = self.add_vlan_member(self.device, vlan_handle=self.vlan20, member_handle=self.lag2, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
        self.add_lag_member(self.device, lag_handle=self.lag2, port_handle=self.port11)
        self.add_lag_member(self.device, lag_handle=self.lag2, port_handle=self.port12)
        try:
            print("Send packet with vlan 20 from port%d to port%d, valid" % (self.devports[11], self.devports[9]))
            pkt = simple_tcp_packet(
                eth_dst='00:77:77:77:77:77',
                eth_src='00:99:99:99:99:99',
                dl_vlan_enable=True,
                vlan_vid=20,
                ip_dst='10.0.0.1',
                ip_ttl=64)
            send_packet(self, self.devports[11], pkt)
            verify_packets(self, pkt, [self.devports[9]])
            print("Send packet with vlan 40 from port%d, dropped" % (self.devports[12]))
            pkt = simple_tcp_packet(
                eth_dst='00:88:88:88:88:88',
                eth_src='00:99:99:99:99:99',
                dl_vlan_enable=True,
                vlan_vid=40,
                ip_dst='10.0.0.1',
                ip_ttl=64)
            send_packet(self, self.devports[12], pkt)
            verify_no_other_packets(self, timeout=1)
            print("Send packet with vlan 50 from port%d, dropped" % (self.devports[11]))
            pkt = simple_tcp_packet(
                eth_dst='00:88:88:88:88:88',
                eth_src='00:99:99:99:99:99',
                dl_vlan_enable=True,
                vlan_vid=50,
                ip_dst='10.0.0.1',
                ip_ttl=64)
            send_packet(self, self.devports[11], pkt)
            verify_no_other_packets(self, timeout=1)
        finally:
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

    def testAccessPacket(self):
        try:
            print("Send packet from port%d to lag3" % (self.devports[10]))
            pkt = simple_tcp_packet(
                eth_dst='00:55:55:55:55:55',
                eth_src='00:99:99:99:99:99',
                ip_dst='10.0.0.1',
                ip_ttl=64)
            send_packet(self, self.devports[10], pkt)
            verify_packets(self, pkt, [self.devports[12]])
            print("Send packet from lag3 to port%d" % (self.devports[10]))
            pkt = simple_tcp_packet(
                eth_dst='00:66:66:66:66:66',
                eth_src='00:99:99:99:99:99',
                ip_dst='10.0.0.1',
                ip_ttl=64)
            send_packet(self, self.devports[12], pkt)
            verify_packets(self, pkt, [self.devports[10]])
        finally:
            pass

    def AccessModeTest(self):
        print("Test LAG access mode")
        print("LAG -> vlan_member40 -> lag_member")
        vlan_mbr10 = self.add_vlan_member(self.device, vlan_handle=self.vlan40, member_handle=self.lag3)
        self.attribute_set(self.lag3, SWITCH_LAG_ATTR_PORT_VLAN_ID, 40)
        self.add_lag_member(self.device, lag_handle=self.lag3, port_handle=self.port12)
        mac4 = self.add_mac_entry(self.device, vlan_handle=self.vlan40, mac_address='00:55:55:55:55:55', destination_handle=self.lag3)
        mac5 = self.add_mac_entry(self.device, vlan_handle=self.vlan40, mac_address='00:99:99:99:99:99', destination_handle=self.port10)
        try:
            self.testAccessPacket()
        finally:
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.attribute_set(self.lag3, SWITCH_LAG_ATTR_PORT_VLAN_ID, 0)
            self.cleanlast()

        print("LAG -> lag_member -> vlan_member40")
        self.add_lag_member(self.device, lag_handle=self.lag3, port_handle=self.port12)
        vlan_mbr10 = self.add_vlan_member(self.device, vlan_handle=self.vlan40, member_handle=self.lag3)
        self.attribute_set(self.lag3, SWITCH_LAG_ATTR_PORT_VLAN_ID, 40)
        mac4 = self.add_mac_entry(self.device, vlan_handle=self.vlan40, mac_address='00:55:55:55:55:55', destination_handle=self.lag3)
        mac5 = self.add_mac_entry(self.device, vlan_handle=self.vlan40, mac_address='00:99:99:99:99:99', destination_handle=self.port10)
        try:
            self.testAccessPacket()
        finally:
            self.cleanlast()
            self.cleanlast()
            self.attribute_set(self.lag3, SWITCH_LAG_ATTR_PORT_VLAN_ID, 0)
            self.cleanlast()
            self.cleanlast()

    def testTrunkPacket(self):
        try:
            print("Send packet with vlan 20 from port%d to lag2" % (self.devports[9]))
            pkt_10 = simple_tcp_packet(
                eth_dst='00:66:66:66:66:66',
                eth_src='00:77:77:77:77:77',
                dl_vlan_enable=True,
                vlan_vid=20,
                ip_dst='10.0.0.1',
                ip_ttl=64)
            send_packet(self, self.devports[9], pkt_10)
            verify_packets(self, pkt_10, [self.devports[11]])
            print("Send packet with vlan 20 from lag2 to port%d" % (self.devports[9]))
            pkt_10 = simple_tcp_packet(
                eth_dst='00:77:77:77:77:77',
                eth_src='00:66:66:66:66:66',
                dl_vlan_enable=True,
                vlan_vid=20,
                ip_dst='10.0.0.1',
                ip_ttl=64)
            send_packet(self, self.devports[11], pkt_10)
            verify_packets(self, pkt_10, [self.devports[9]])
            print("Send packet with vlan 30 from port%d to lag2" % (self.devports[9]))
            pkt_20 = simple_tcp_packet(
                eth_dst='00:99:99:99:99:99',
                eth_src='00:88:88:88:88:88',
                dl_vlan_enable=True,
                vlan_vid=30,
                ip_dst='10.0.0.1',
                ip_ttl=64)
            send_packet(self, self.devports[9], pkt_20)
            verify_packets(self, pkt_20, [self.devports[11]])
            print("Send packet with vlan 30 from lag2 to port%d" % (self.devports[9]))
            pkt_20 = simple_tcp_packet(
                eth_dst='00:88:88:88:88:88',
                eth_src='00:99:99:99:99:99',
                dl_vlan_enable=True,
                vlan_vid=30,
                ip_dst='10.0.0.1',
                ip_ttl=64)
            send_packet(self, self.devports[11], pkt_20)
            verify_packets(self, pkt_20, [self.devports[9]])
        finally:
            pass

    def TrunkModeTest(self):
        print("Test LAG trunk mode")
        print("LAG -> vlan_members -> lag_member")
        mac2 = self.add_mac_entry(self.device, vlan_handle=self.vlan20, mac_address='00:77:77:77:77:77', destination_handle=self.port9)
        mac3 = self.add_mac_entry(self.device, vlan_handle=self.vlan30, mac_address='00:88:88:88:88:88', destination_handle=self.port9)
        vlan_mbr10 = self.add_vlan_member(self.device, vlan_handle=self.vlan20, member_handle=self.lag2, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
        vlan_mbr11 = self.add_vlan_member(self.device, vlan_handle=self.vlan30, member_handle=self.lag2, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
        self.add_lag_member(self.device, lag_handle=self.lag2, port_handle=self.port11)
        mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan20, mac_address='00:66:66:66:66:66', destination_handle=self.lag2)
        mac1 = self.add_mac_entry(self.device, vlan_handle=self.vlan30, mac_address='00:99:99:99:99:99', destination_handle=self.lag2)
        try:
            self.testTrunkPacket()
        finally:
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
        print("LAG -> lag_member -> vlan_members")
        mac2 = self.add_mac_entry(self.device, vlan_handle=self.vlan20, mac_address='00:77:77:77:77:77', destination_handle=self.port9)
        mac3 = self.add_mac_entry(self.device, vlan_handle=self.vlan30, mac_address='00:88:88:88:88:88', destination_handle=self.port9)
        self.add_lag_member(self.device, lag_handle=self.lag2, port_handle=self.port11)
        vlan_mbr10 = self.add_vlan_member(self.device, vlan_handle=self.vlan20, member_handle=self.lag2, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
        vlan_mbr11 = self.add_vlan_member(self.device, vlan_handle=self.vlan30, member_handle=self.lag2, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
        mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan20, mac_address='00:66:66:66:66:66', destination_handle=self.lag2)
        mac1 = self.add_mac_entry(self.device, vlan_handle=self.vlan30, mac_address='00:99:99:99:99:99', destination_handle=self.lag2)
        try:
            self.testTrunkPacket()
        finally:
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

    #  dscp-to-tc/pcp-to-tc qos mapping are currently applied only on port
    def LagQoSGroupTest(self):
        print("LagQoSGroupTest()")
        print("Test LAG members qos group test")
        if (self.client.is_feature_enable(SWITCH_FEATURE_INGRESS_QOS_MAP) == 0):
            print("Ingress qos mapping feature not enabled, skipping")
            return

        mac1 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:33:33:33:33:33', destination_handle=self.lag1)

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
                eth_src='00:22:22:22:22:22',
                eth_dst='00:33:33:33:33:33',
                ip_dst='172.20.10.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_tos=4, # dscp 1
                ip_ttl=64)
            exp_pkt = simple_tcp_packet(
                eth_src='00:22:22:22:22:22',
                eth_dst='00:33:33:33:33:33',
                ip_dst='172.20.10.1',
                ip_src='192.168.0.1',
                ip_id=105,
                ip_tos=36, # dscp 9
                ip_ttl=64)
            print("Allow packet before binding qos groups to lags")
            for port in [self.devports[1], self.devports[2], self.devports[3], self.devports[4]]:
                dst_ip = int(binascii.hexlify(socket.inet_aton('10.10.10.1')), 16)
                src_ip = int(binascii.hexlify(socket.inet_aton('192.168.8.1')), 16)
                for i in range(0, 12):
                    dst_ip_addr = socket.inet_ntoa(binascii.unhexlify(format(dst_ip, 'x').zfill(8)))
                    src_ip_addr = socket.inet_ntoa(binascii.unhexlify(format(src_ip, 'x').zfill(8)))
                    pkt['IP'].src = src_ip_addr
                    pkt['IP'].dst = dst_ip_addr
                    send_packet(self, port, pkt)
                    verify_any_packet_any_port(self, [pkt, pkt], [self.devports[5], self.devports[6]])
                    dst_ip += 1
                    src_ip += 1

            print("Bind ingress qos group to lag0 and egress qos group to lag1")
            self.attribute_set(self.lag0, SWITCH_LAG_ATTR_INGRESS_QOS_GROUP, self.dscp_tc_map_ingress)
            self.attribute_set(self.lag1, SWITCH_LAG_ATTR_EGRESS_QOS_GROUP, self.tc_dscp_map_egress)
            for port in [self.devports[1], self.devports[2], self.devports[3], self.devports[4]]:
                dst_ip = int(binascii.hexlify(socket.inet_aton('10.10.10.1')), 16)
                src_ip = int(binascii.hexlify(socket.inet_aton('192.168.8.1')), 16)
                for i in range(0, 12):
                    dst_ip_addr = socket.inet_ntoa(binascii.unhexlify(format(dst_ip, 'x').zfill(8)))
                    src_ip_addr = socket.inet_ntoa(binascii.unhexlify(format(src_ip, 'x').zfill(8)))
                    pkt['IP'].src = src_ip_addr
                    pkt['IP'].dst = dst_ip_addr
                    exp_pkt['IP'].src = src_ip_addr
                    exp_pkt['IP'].dst = dst_ip_addr
                    send_packet(self, port, pkt)
                    verify_any_packet_any_port(self, [exp_pkt, exp_pkt], [self.devports[5], self.devports[6]])
                    dst_ip += 1
                    src_ip += 1
            print("Add a 5th member to lag0. Should update dscp for this pkt also")
            lag_mbr5 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port11)
            for port in [self.devports[1], self.devports[2], self.devports[3], self.devports[4], self.devports[11]]:
                dst_ip = int(binascii.hexlify(socket.inet_aton('10.10.10.1')), 16)
                src_ip = int(binascii.hexlify(socket.inet_aton('192.168.8.1')), 16)
                for i in range(0, 12):
                    dst_ip_addr = socket.inet_ntoa(binascii.unhexlify(format(dst_ip, 'x').zfill(8)))
                    src_ip_addr = socket.inet_ntoa(binascii.unhexlify(format(src_ip, 'x').zfill(8)))
                    pkt['IP'].src = src_ip_addr
                    pkt['IP'].dst = dst_ip_addr
                    exp_pkt['IP'].src = src_ip_addr
                    exp_pkt['IP'].dst = dst_ip_addr
                    send_packet(self, port, pkt)
                    verify_any_packet_any_port(self, [exp_pkt, exp_pkt], [self.devports[5], self.devports[6]])
                    dst_ip += 1
                    src_ip += 1
            print("Add a 3rd member to lag1. Should update dscp for this pkt also")
            lag_mbr3 = self.add_lag_member(self.device, lag_handle=self.lag1, port_handle=self.port12)
            for port in [self.devports[1], self.devports[2], self.devports[3], self.devports[4], self.devports[11]]:
                dst_ip = int(binascii.hexlify(socket.inet_aton('10.10.10.1')), 16)
                src_ip = int(binascii.hexlify(socket.inet_aton('192.168.8.1')), 16)
                for i in range(0, 12):
                    dst_ip_addr = socket.inet_ntoa(binascii.unhexlify(format(dst_ip, 'x').zfill(8)))
                    src_ip_addr = socket.inet_ntoa(binascii.unhexlify(format(src_ip, 'x').zfill(8)))
                    pkt['IP'].src = src_ip_addr
                    pkt['IP'].dst = dst_ip_addr
                    exp_pkt['IP'].src = src_ip_addr
                    exp_pkt['IP'].dst = dst_ip_addr
                    send_packet(self, port, pkt)
                    verify_any_packet_any_port(self, [exp_pkt, exp_pkt, exp_pkt], [self.devports[5], self.devports[6], self.devports[12]])
                    dst_ip += 1
                    src_ip += 1

            print("Remove binding, packet should be forwarded")
            self.attribute_set(self.lag0, SWITCH_LAG_ATTR_INGRESS_QOS_GROUP, 0)
            self.attribute_set(self.lag1, SWITCH_LAG_ATTR_EGRESS_QOS_GROUP, 0)
            for port in [self.devports[1], self.devports[2], self.devports[3], self.devports[4], self.devports[11]]:
                dst_ip = int(binascii.hexlify(socket.inet_aton('10.10.10.1')), 16)
                src_ip = int(binascii.hexlify(socket.inet_aton('192.168.8.1')), 16)
                for i in range(0, 12):
                    dst_ip_addr = socket.inet_ntoa(binascii.unhexlify(format(dst_ip, 'x').zfill(8)))
                    src_ip_addr = socket.inet_ntoa(binascii.unhexlify(format(src_ip, 'x').zfill(8)))
                    pkt['IP'].src = src_ip_addr
                    pkt['IP'].dst = dst_ip_addr
                    send_packet(self, port, pkt)
                    verify_any_packet_any_port(self, [pkt, pkt, pkt], [self.devports[5], self.devports[6], self.devports[12]])
                    dst_ip += 1
                    src_ip += 1
        finally:
            self.attribute_set(self.lag0, SWITCH_LAG_ATTR_INGRESS_QOS_GROUP, 0)
            self.attribute_set(self.lag1, SWITCH_LAG_ATTR_EGRESS_QOS_GROUP, 0)
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

    def LagStatsTest(self):
        '''
        Count the number of packets from the above tests
        '''
        time.sleep(4)
        cntrs = self.client.object_counters_get(self.vlan10)
        # Ingress and egress u/m/b
        self.assertEqual(len(cntrs), 12)
        iu = SWITCH_VLAN_COUNTER_ID_IN_UCAST_PKTS
        print("ING UCAST: recieved %d expected %d" % (cntrs[iu].count, self.ing_vlan10_uc))
        ou = SWITCH_VLAN_COUNTER_ID_OUT_UCAST_PKTS
        print("EG UCAST : recieved %d expected %d" % (cntrs[ou].count, self.eg_vlan10_uc))
        ib = SWITCH_VLAN_COUNTER_ID_IN_BCAST_PKTS
        print("ING BCAST: recieved %d expected %d" % (cntrs[ib].count, self.ing_vlan10_bc))
        ob = SWITCH_VLAN_COUNTER_ID_OUT_BCAST_PKTS
        print("EG BCAST : recieved %d expected %d" % (cntrs[ob].count, self.eg_vlan10_bc))
        self.assertEqual(cntrs[iu].count, self.ing_vlan10_uc)
        self.assertEqual(cntrs[ou].count, self.eg_vlan10_uc)
        self.assertEqual(cntrs[ib].count, self.ing_vlan10_bc)
        self.assertEqual(cntrs[ob].count, self.eg_vlan10_bc)

###############################################################################

@group('l2')
@group('lag')
@group('mlag')
@disabled
class L2MlagTest(ApiHelper):
    def runTest(self):
        print()
        self.configure()

        print("mLAG flooding - peer-link[%d,%d], mlag1[%d,%d], mlag2[%d], normal lag[%d,%d], port %d" % (
            self.devports[1], self.devports[2], self.devports[3], self.devports[4],
            self.devports[5], self.devports[6], self.devports[7], self.devports[0]))
        # Create peer-link
        self.peer_lag = self.add_lag(self.device, is_peer_link=True)
        peer_lag_mbr0 = self.add_lag_member(self.device, lag_handle=self.peer_lag, port_handle=self.port1)
        peer_lag_mbr1 = self.add_lag_member(self.device, lag_handle=self.peer_lag, port_handle=self.port2)

        # create a mLAG with two member ports on current switch
        self.mlag1 = self.add_lag(self.device, peer_link_handle=self.peer_lag)
        mlag1_mbr0 = self.add_lag_member(self.device, lag_handle=self.mlag1, port_handle=self.port3)
        mlag1_mbr1 = self.add_lag_member(self.device, lag_handle=self.mlag1, port_handle=self.port4)

        # create a mLAG with a single member port on current switch
        self.mlag2 = self.add_lag(self.device, peer_link_handle=self.peer_lag)
        mlag2_mbr0 = self.add_lag_member(self.device, lag_handle=self.mlag2, port_handle=self.port5)

        # create a regular LAG with two member ports
        self.simple_lag = self.add_lag(self.device)
        lag_mbr0 = self.add_lag_member(self.device, lag_handle=self.simple_lag, port_handle=self.port6)
        lag_mbr1 = self.add_lag_member(self.device, lag_handle=self.simple_lag, port_handle=self.port7)

        self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port0)
        self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.peer_lag)
        self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.mlag1)
        self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.mlag2)
        self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.simple_lag)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.attribute_set(self.peer_lag, SWITCH_LAG_ATTR_PORT_VLAN_ID, 10)
        self.attribute_set(self.mlag1, SWITCH_LAG_ATTR_PORT_VLAN_ID, 10)
        self.attribute_set(self.mlag2, SWITCH_LAG_ATTR_PORT_VLAN_ID, 10)
        self.attribute_set(self.simple_lag, SWITCH_LAG_ATTR_PORT_VLAN_ID, 10)

        try:
            self.PeerLinkIngressFloodTest()
            self.MlagIngressFloodTest()
            self.NonMlagIngressFloodTest()
            self.MlagMemberUpdateTest()
            self.PeerLinkMemberUpdateTest()
            self.PeerLinkIngressUnicastTest()
            self.PeerLinkEgressUnicastTest()
        finally:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.peer_lag, SWITCH_LAG_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.mlag1, SWITCH_LAG_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.mlag2, SWITCH_LAG_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.simple_lag, SWITCH_LAG_ATTR_PORT_VLAN_ID, 0)
            self.cleanup()

    def PeerLinkIngressFloodTest(self):
        print("PeerLinkIngressFloodTest()")
        try:
            # Send from peer_link, should not be received on mlag ports
            pkt = simple_tcp_packet(
                eth_src='00:11:11:11:11:11',
                eth_dst='00:22:22:22:22:22',
                ip_dst='172.16.0.1',
                ip_id=107,
                ip_ttl=33)
            exp_pkt = simple_tcp_packet(
                eth_src='00:11:11:11:11:11',
                eth_dst='00:22:22:22:22:22',
                ip_dst='172.16.0.1',
                ip_id=107,
                ip_ttl=33)

            print("Sending packet from peer_lag port%d -> simple_lag, port%d" % (self.devports[1], self.devports[0]))
            send_packet(self, self.devports[1], pkt)
            verify_any_packet_on_ports_list(self, [exp_pkt], [[self.devports[6], self.devports[7]], [self.devports[0]]])
            print("Sending packet from peer_lag port%d -> simple_lag, port%d" % (self.devports[2], self.devports[0]))
            send_packet(self, self.devports[2], pkt)
            verify_any_packet_on_ports_list(self, [exp_pkt], [[self.devports[6], self.devports[7]], [self.devports[0]]])
        finally:
            pass

    def MlagIngressFloodTest(self):
        print("MlagIngressFloodTest()")
        try:
            # Send from mlag1, should be received everywhere
            pkt = simple_tcp_packet(
                eth_dst='00:33:33:33:33:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='172.16.0.3',
                ip_id=107,
                ip_ttl=33)
            exp_pkt = simple_tcp_packet(
                eth_dst='00:33:33:33:33:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='172.16.0.3',
                ip_id=107,
                ip_ttl=33)

            print("Sending packet from mlag1 port%d -> peer_lag, mlag2, simple_lag, port%d"  % (self.devports[3], self.devports[0]))
            send_packet(self, self.devports[3], pkt)
            verify_any_packet_on_ports_list(self, [exp_pkt], [[self.devports[1], self.devports[2]], [self.devports[5]], [self.devports[6], self.devports[7]], [self.devports[0]]])
            print("Sending packet from mlag1 port%d -> peer_lag, mlag2, simple_lag, port%d" % (self.devports[4], self.devports[0]))
            send_packet(self, self.devports[4], pkt)
            verify_any_packet_on_ports_list(self, [exp_pkt], [[self.devports[1], self.devports[2]], [self.devports[5]], [self.devports[6], self.devports[7]], [self.devports[0]]])
        finally:
            pass

    def NonMlagIngressFloodTest(self):
        print("NonMlagIngressFloodTest()")
        try:
            # Send from simple_lag, should be received everywhere
            pkt = simple_tcp_packet(
                eth_dst='00:33:33:33:33:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='172.16.0.3',
                ip_id=107,
                ip_ttl=33)
            exp_pkt = simple_tcp_packet(
                eth_dst='00:33:33:33:33:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='172.16.0.3',
                ip_id=107,
                ip_ttl=33)

            print("Sending packet from simple_lag port%d -> peer_lag, mlag1, mlag2, port%d"  % (self.devports[6], self.devports[0]))
            send_packet(self, self.devports[6], pkt)
            verify_any_packet_on_ports_list(self, [exp_pkt], [[self.devports[1], self.devports[2]], [self.devports[3], self.devports[4]], [self.devports[5]], [self.devports[0]]])
            print("Sending packet from simple_lag port%d -> peer_lag, mlag1, mlag2, port%d" % (self.devports[7], self.devports[0]))
            send_packet(self, self.devports[7], pkt)
            verify_any_packet_on_ports_list(self, [exp_pkt], [[self.devports[1], self.devports[2]], [self.devports[3], self.devports[4]], [self.devports[5]], [self.devports[0]]])
        finally:
            pass

    def MlagMemberUpdateTest(self):
        print("MlagMemberUpdateTest()")
        # add a member to mlag1, send on new member, should be received everywhere
        mlag1_mbr2 = self.add_lag_member(self.device, lag_handle=self.mlag1, port_handle=self.port8)
        print("After adding port %d to mlag1 - peer-link[%d,%d], mlag1[%d,%d,%d], mlag2[%d], normal lag[%d,%d], port %d" % (
            self.devports[8], self.devports[1], self.devports[2], self.devports[3], self.devports[4],
            self.devports[8], self.devports[5], self.devports[6], self.devports[7], self.devports[0]))

        try:
            # Send from mlag1, should be received everywhere
            pkt = simple_tcp_packet(
                eth_dst='00:33:33:33:33:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='172.16.0.3',
                ip_id=107,
                ip_ttl=33)
            exp_pkt = simple_tcp_packet(
                eth_dst='00:33:33:33:33:33',
                eth_src='00:22:22:22:22:22',
                ip_dst='172.16.0.3',
                ip_id=107,
                ip_ttl=33)

            print("Sending packet from mlag1 port%d -> peer_lag, mlag2, simple_lag, port%d" % (self.devports[8], self.devports[0]))
            send_packet(self, self.devports[8], pkt)
            verify_any_packet_on_ports_list(self, [exp_pkt], [
                [self.devports[1], self.devports[2]], [self.devports[5]],
              [self.devports[6], self.devports[7]], [self.devports[0]]])
        finally:
            pass

    def PeerLinkMemberUpdateTest(self):
        print("PeerLinkMemberUpdateTest()")
        # Add port 9 to peer lag
        peer_lag_mbr2 = self.add_lag_member(self.device, lag_handle=self.peer_lag, port_handle=self.port9)
        print("After adding port %d to peer lag - peer-link[%d,%d,%d], mlag1[%d,%d,%d], mlag2[%d], normal lag[%d,%d], port %d" % (
            self.devports[9], self.devports[1], self.devports[2], self.devports[9], self.devports[3], self.devports[4],
            self.devports[8], self.devports[5], self.devports[6], self.devports[7], self.devports[0]))

        try:
            # Send from peer_link, should not be received on mlag ports
            pkt = simple_tcp_packet(
                eth_src='00:11:11:11:11:11',
                eth_dst='00:33:33:33:33:33',
                ip_dst='172.16.0.1',
                ip_id=107,
                ip_ttl=33)
            exp_pkt = simple_tcp_packet(
                eth_src='00:11:11:11:11:11',
                eth_dst='00:33:33:33:33:33',
                ip_dst='172.16.0.1',
                ip_id=107,
                ip_ttl=33)

            print("Sending packet from peer_lag port%d -> simple_lag, port%d" % (self.devports[9], self.devports[0]))
            send_packet(self, self.devports[9], pkt)
            verify_any_packet_on_ports_list(self, [exp_pkt], [[self.devports[6], self.devports[7]], [self.devports[0]]])
        finally:
            pass

    def PeerLinkIngressUnicastTest(self):
        print("PeerLinkIngressUnicastTest()")
        mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan10,
            mac_address='00:22:22:22:22:22', destination_handle=self.mlag1)
        mac1 = self.add_mac_entry(self.device, vlan_handle=self.vlan10,
            mac_address='00:33:33:33:33:33', destination_handle=self.simple_lag)
        try:
            pkt = simple_tcp_packet(
                eth_src='00:11:11:11:11:11',
                eth_dst='00:22:22:22:22:22',
                ip_dst='172.16.0.1',
                ip_id=107,
                ip_ttl=33)
            pkt2 = simple_tcp_packet(
                eth_src='00:11:11:11:11:11',
                eth_dst='00:33:33:33:33:33',
                ip_dst='172.16.0.1',
                ip_id=107,
                ip_ttl=33)

            print("Sending packet from peer_lag port%d -> mlag1, drop" % (self.devports[1]))
            send_packet(self, self.devports[1], pkt)
            verify_no_other_packets(self, timeout=1)
            print("Sending packet from peer_lag port%d -> expect packet on simple_lag" % (self.devports[1]))
            send_packet(self, self.devports[1], pkt2)
            verify_any_packet_on_ports_list(self, [pkt2], [[self.devports[6], self.devports[7]]])
        finally:
            self.cleanlast()
            self.cleanlast()
            pass

    def PeerLinkEgressUnicastTest(self):
        print("PeerLinkEgressUnicastTest()")
        return
        '''
        Add an mlag, send a unicast packet to it
        Remove all members of new mlag, now packets goto peer_link
        '''
        print("Adding new mlag3 with members port%d, port%d"  % (self.devports[10], self.devports[11]))
        self.mlag3 = self.add_lag(self.device, peer_link_handle=self.peer_lag)
        self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.mlag3)
        self.attribute_set(self.mlag3, SWITCH_LAG_ATTR_PORT_VLAN_ID, 10)
        mlag3_mbr0 = self.add_lag_member(self.device, lag_handle=self.mlag3, port_handle=self.port10)
        mlag3_mbr1 = self.add_lag_member(self.device, lag_handle=self.mlag3, port_handle=self.port11)
        mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan10,
            mac_address='00:22:22:22:22:22', destination_handle=self.mlag3)
        try:
            pkt = simple_tcp_packet(
                eth_src='00:11:11:11:11:11',
                eth_dst='00:22:22:22:22:22',
                ip_dst='172.16.0.1',
                ip_id=107,
                ip_ttl=33)

            print("Sending packet from port%d -> mlag3"  % (self.devports[0]))
            send_packet(self, self.devports[0], pkt)
            verify_any_packet_on_ports_list(self, [pkt], [[self.devports[10], self.devports[11]]])

            # Remove mac, mlag member and re-add mac
            print("Remove all mlag members and resend packet")
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan10,
                mac_address='00:22:22:22:22:22', destination_handle=self.mlag3)

            print("Sending packet from port%d -> expect packet on peer_link now"  % (self.devports[0]))
            send_packet(self, self.devports[0], pkt)
            verify_any_packet_on_ports_list(self, [pkt], [[self.devports[1], self.devports[2], self.devports[9]]])
        finally:
            self.attribute_set(self.mlag3, SWITCH_LAG_ATTR_PORT_VLAN_ID, 0)
            pass

###############################################################################

@group('l2')
@group('stp')
class L2StpTest(ApiHelper):
    def runTest(self):
        print()
        if (self.client.is_feature_enable(SWITCH_FEATURE_STP) == 0):
            print("STP feature not enabled, skipping")
            return
        self.configure()

        self.attribute_set(self.port0, SWITCH_PORT_ATTR_LEARNING, True)
        vlan_mbr0 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port0)
        vlan_mbr1 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port1)
        vlan_mbr2 = self.add_vlan_member(self.device, vlan_handle=self.vlan20, member_handle=self.port2)
        vlan_mbr3 = self.add_vlan_member(self.device, vlan_handle=self.vlan20, member_handle=self.port3)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.attribute_set(self.port2, SWITCH_PORT_ATTR_PORT_VLAN_ID, 20)
        self.attribute_set(self.port3, SWITCH_PORT_ATTR_PORT_VLAN_ID, 20)
        vlan_mbr4 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port4, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
        vlan_mbr5 = self.add_vlan_member(self.device, vlan_handle=self.vlan20, member_handle=self.port4, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)

        stp = self.add_stp(self.device)

        self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_STP_HANDLE, stp)
        self.attribute_set(self.vlan20, SWITCH_VLAN_ATTR_STP_HANDLE, stp)

        self.stp_port0 = self.add_stp_port(self.device, state=SWITCH_STP_PORT_ATTR_STATE_FORWARDING, stp_handle=stp, port_lag_handle=self.port0)
        self.stp_port1 = self.add_stp_port(self.device, state=SWITCH_STP_PORT_ATTR_STATE_FORWARDING, stp_handle=stp, port_lag_handle=self.port1)
        self.stp_port2 = self.add_stp_port(self.device, state=SWITCH_STP_PORT_ATTR_STATE_FORWARDING, stp_handle=stp, port_lag_handle=self.port2)
        self.stp_port3 = self.add_stp_port(self.device, state=SWITCH_STP_PORT_ATTR_STATE_FORWARDING, stp_handle=stp, port_lag_handle=self.port3)
        self.stp_port4 = self.add_stp_port(self.device, state=SWITCH_STP_PORT_ATTR_STATE_FORWARDING, stp_handle=stp, port_lag_handle=self.port4)

        mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:11:11:11:11:11', destination_handle=self.port1)
        mac1 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:22:22:22:22:22', destination_handle=self.port0)
        mac2 = self.add_mac_entry(self.device, vlan_handle=self.vlan20, mac_address='00:33:33:33:33:33', destination_handle=self.port3)
        mac3 = self.add_mac_entry(self.device, vlan_handle=self.vlan20, mac_address='00:44:44:44:44:44', destination_handle=self.port2)
        mac4 = self.add_mac_entry(self.device, vlan_handle=self.vlan20, mac_address='00:55:55:55:55:55', destination_handle=self.port4)

        try:
            self.StpBasicTest()
            self.StpLearningTest()
            self.StpEgressBlockingTest()
            self.StpArpTest()
        finally:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_LEARNING, False)
            self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_STP_HANDLE, 0)
            self.attribute_set(self.vlan20, SWITCH_VLAN_ATTR_STP_HANDLE, 0)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.port2, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.port3, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.cleanup()

    def StpBasicTest(self):
        pkt = simple_tcp_packet(
            eth_dst='00:11:11:11:11:11',
            eth_src='00:22:22:22:22:22')

        pkt2 = simple_tcp_packet(
            eth_dst='00:33:33:33:33:33',
            eth_src='00:44:44:44:44:44')
        try:
            print("Sending packet port %d" % self.devports[
                0], " (forwarding)-> port %d" % self.devports[
                    1], " (192.168.0.1 -> 10.0.0.1 [id = 101])")
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, pkt, self.devports[1])

            print("Sending packet port %d" % self.devports[
                0], " (forwarding) -> port %d" % self.devports[
                    3], " (192.168.0.1 -> 11.0.0.1 [id = 101])")
            send_packet(self, self.devports[2], pkt2)
            verify_packet(self, pkt2, self.devports[3])

            self.attribute_set(self.stp_port2, SWITCH_STP_PORT_ATTR_STATE, SWITCH_STP_PORT_ATTR_STATE_BLOCKING)
            print("Sending packet port %d" % self.devports[
                2], " (blocking) -> port %d" % self.devports[
                    3], " (192.168.0.1 -> 11.0.0.1 [id = 101])")
            send_packet(self, self.devports[2], pkt2)
            verify_no_other_packets(self, timeout=1)
        finally:
            self.attribute_set(self.stp_port2, SWITCH_STP_PORT_ATTR_STATE, SWITCH_STP_PORT_ATTR_STATE_FORWARDING)

    def StpLearningTest(self):
        pkt = simple_tcp_packet(
            eth_dst='00:11:11:11:11:11',
            eth_src='00:22:22:22:22:22')

        unknown_pkt = simple_tcp_packet(
            eth_dst='00:11:11:11:11:11',
            eth_src='00:77:77:77:77:77')

        pkt2 = simple_tcp_packet(
            eth_src='00:11:11:11:11:11',
            eth_dst='00:77:77:77:77:77')
        try:
            self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_AGING_INTERVAL, 10000)
            print("Port 0 in forwarding, send to port 1")
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, pkt, self.devports[1])

            self.attribute_set(self.stp_port0, SWITCH_STP_PORT_ATTR_STATE, SWITCH_STP_PORT_ATTR_STATE_LEARNING)
            print("Port 0 in learning, drop")
            send_packet(self, self.devports[0], pkt)
            verify_no_other_packets(self, timeout=1)

            print("Port 0 in learning, learn and drop")
            send_packet(self, self.devports[0], unknown_pkt)
            verify_no_other_packets(self, timeout=1)
            time.sleep(5)

            print("Pkt from port 1 to port 0 should be dropped, since state is learning")
            send_packet(self, self.devports[1], pkt2)
            verify_no_other_packets(self, timeout=1)

            self.attribute_set(self.stp_port0, SWITCH_STP_PORT_ATTR_STATE, SWITCH_STP_PORT_ATTR_STATE_FORWARDING)
            print("Pkt from port 1 to port 0 should now be forwarded")
            send_packet(self, self.devports[1], pkt2)
            verify_packet(self, pkt2, self.devports[0])
        finally:
            time.sleep(15)
            self.attribute_set(self.stp_port0, SWITCH_STP_PORT_ATTR_STATE, SWITCH_STP_PORT_ATTR_STATE_FORWARDING)

    def StpEgressBlockingTest(self):
        unknown_pkt = simple_tcp_packet(
            eth_dst='00:11:11:11:11:12',
            eth_src='00:22:22:22:22:22',
            ip_ttl=64)
        unknown_exp_pkt = simple_tcp_packet(
            eth_dst='00:11:11:11:11:12',
            eth_src='00:22:22:22:22:22',
            ip_ttl=64)
        unknown_exp_vlan_pkt = simple_tcp_packet(
            eth_dst='00:11:11:11:11:12',
            eth_src='00:22:22:22:22:22',
            dl_vlan_enable=True,
            vlan_vid=10,
            pktlen=104,
            ip_ttl=64)
        unknown_pkt2 = simple_tcp_packet(
            eth_dst='00:11:11:11:11:12',
            eth_src='00:44:44:44:44:44',
            ip_ttl=64)
        unknown_exp_pkt2 = simple_tcp_packet(
            eth_dst='00:11:11:11:11:12',
            eth_src='00:44:44:44:44:44',
            ip_ttl=64)
        unknown_exp_vlan_pkt2 = simple_tcp_packet(
            eth_dst='00:11:11:11:11:12',
            eth_src='00:44:44:44:44:44',
            dl_vlan_enable=True,
            vlan_vid=20,
            pktlen=104,
            ip_ttl=64)

        try:
            print("Sending unknown unicast packet from port0, flood on port1 and port4")
            send_packet(self, self.devports[0], unknown_pkt)
            verify_each_packet_on_each_port(self,
                                            [unknown_exp_pkt, unknown_exp_vlan_pkt],
                                            [self.devports[1], self.devports[4]])

            print("Port 1 in stp blocking state")
            self.attribute_set(self.stp_port1, SWITCH_STP_PORT_ATTR_STATE, SWITCH_STP_PORT_ATTR_STATE_BLOCKING)

            print("Sending unknown unicast packet from port0, port1 blocked, flood to port4")
            send_packet(self, self.devports[0], unknown_pkt)
            verify_packets(self, unknown_exp_vlan_pkt, [self.devports[4]])

            print("Port 4 in stp blocking state")
            self.attribute_set(self.stp_port4, SWITCH_STP_PORT_ATTR_STATE, SWITCH_STP_PORT_ATTR_STATE_BLOCKING)

            print("Sending unknown unicast packet from port0, port1, port4 blocked")
            send_packet(self, self.devports[0], unknown_pkt)
            verify_no_other_packets(self, timeout=1)

            print("Sending unknown unicast packet from port2, flood on port3, port4 blocked")
            send_packet(self, self.devports[2], unknown_pkt2)
            verify_packets(self, unknown_exp_pkt2, [self.devports[3]])

            print("Port 4 in stp forwarding state")
            self.attribute_set(self.stp_port4, SWITCH_STP_PORT_ATTR_STATE, SWITCH_STP_PORT_ATTR_STATE_FORWARDING)

            print("Sending unknown unicast packet from port2, flood on port3 and port4")
            send_packet(self, self.devports[2], unknown_pkt2)
            verify_each_packet_on_each_port(self,
                                            [unknown_exp_pkt2, unknown_exp_vlan_pkt2],
                                            [self.devports[3], self.devports[4]])

        finally:
            self.attribute_set(self.stp_port1, SWITCH_STP_PORT_ATTR_STATE, SWITCH_STP_PORT_ATTR_STATE_FORWARDING)
            pass

    def StpArpTest(self):
        arp_pkt = simple_arp_packet(eth_src='00:22:22:22:22:22', pktlen=100)
        arp_tagged_pkt = simple_arp_packet(eth_src='00:22:22:22:22:22', vlan_vid=10, pktlen=104)
        try:
            print("Send ARP packet from port %d, flood on port %d %d" % (self.devports[0], self.devports[1], self.devports[4]))
            send_packet(self, self.devports[0], arp_pkt)
            verify_each_packet_on_each_port(self, [arp_pkt, arp_tagged_pkt], [self.devports[1], self.devports[4]])

            print("Port 0 in stp blocking state")
            self.attribute_set(self.stp_port0, SWITCH_STP_PORT_ATTR_STATE, SWITCH_STP_PORT_ATTR_STATE_BLOCKING)

            print("Send ARP packet from port drop")
            send_packet(self, self.devports[0], arp_pkt)
            verify_no_other_packets(self, timeout=1)

            print("Port 0 in stp forwarding state")
            self.attribute_set(self.stp_port0, SWITCH_STP_PORT_ATTR_STATE, SWITCH_STP_PORT_ATTR_STATE_FORWARDING)

            print("Send ARP packet from port %d, flood on port %d %d" % (self.devports[0], self.devports[1], self.devports[4]))
            send_packet(self, self.devports[0], arp_pkt)
            verify_each_packet_on_each_port(self, [arp_pkt, arp_tagged_pkt], [self.devports[1], self.devports[4]])
        finally:
            self.attribute_set(self.stp_port0, SWITCH_STP_PORT_ATTR_STATE, SWITCH_STP_PORT_ATTR_STATE_FORWARDING)
            pass

###############################################################################

@group('l2')
class StatsTest(ApiHelper):
    def runTest(self):
        print()
        self.configure()

        vlan_mbr0 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port0)
        vlan_mbr1 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port1)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)

        try:
            self.PortStatsClearConfigTest()
            self.VlanStatsClearTest()
            # The tests below should be run manually on simulator
            # as rmon counters poll thread is running on HW only
            # so that they are commented out.
            #self.sendPktSizes()
            #self.PortStatsTest()
            #self.PortStatsClearTest()

        finally:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_IPV4_MULTICAST, False)
            self.cleanup()

    def CheckCounter(self, oid, cntr_id, count, counter_descr="", positive_check=True):
        counters = self.client.object_counters_get(oid)
        for cntr in counters:
            if cntr.counter_id == cntr_id:
                if positive_check:
                    self.assertTrue(cntr.count == count,
                        '{}: number of packets/bytes is {}, expected {}' \
                            .format(counter_descr, cntr.count, count))
                else:
                    self.assertTrue(cntr.count != count,
                        '{}: number of packets/bytes should not be equal to {}' \
                            .format(counter_descr, count))
                break


    def PortStatsClearConfigTest(self):
        '''
        This test tries to verify that port per counter ID clear doesn't fail
        '''
        print("PortStatsClearConfigTest()")

        try:
            for i in range(SWITCH_PORT_COUNTER_ID_IN_GOOD_OCTETS, SWITCH_PORT_COUNTER_ID_MAX):
                cntr_ids = [i]
                self.client.object_counters_clear(self.port0, cntr_ids)
                self.assertEqual(self.status(), 0, "Clear of counter id {} failed".format(i))
        finally:
            pass

    def VlanStatsClearTest(self):
        '''
        This test tries to verify that vlan stats is cleared correctly by all counters and per counter ID clear
        '''
        print("VlanStatsClearTest()")

        pkt = simple_udp_packet(
            eth_dst='00:11:11:11:11:11',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.0.0.1',
            ip_ttl=64)

        exp_pkt = pkt.copy()

        pkt2 = simple_udp_packet(
            eth_dst='00:22:22:22:22:22',
            eth_src='00:11:11:11:11:11',
            ip_dst='20.0.0.1',
            ip_ttl=64)

        exp_pkt2 = pkt2.copy()

        mcast_pkt = simple_udp_packet(
            eth_dst='01:22:22:22:22:22',
            eth_src='00:11:11:11:11:11',
            ip_dst='20.0.0.1',
            ip_ttl=64)

        exp_mcast_pkt = mcast_pkt.copy()

        bcast_pkt = simple_udp_packet(
            eth_dst='ff:ff:ff:ff:ff:ff',
            eth_src='00:11:11:11:11:11',
            ip_dst='20.0.0.1',
            ip_ttl=64)

        exp_bcast_pkt = bcast_pkt.copy()

        try:
            print("Sending L2 packet from {} -> {}".format(self.devports[0], self.devports[1]))
            send_packet(self, self.devports[0], pkt2)
            verify_packets(self, exp_pkt2, [self.devports[1]])

            print("Sending L2 broadcast packet from {} -> {}".format(self.devports[0], self.devports[1]))
            send_packet(self, self.devports[0], bcast_pkt)
            verify_packets(self, exp_bcast_pkt, [self.devports[1]])

            # print("Sending L2 multicast packet from {} -> {}".format(self.devports[0], self.devports[1]))
            # send_packet(self, self.devports[0], mcast_pkt)
            # verify_packets(self, exp_mcast_pkt, [self.devports[1]])

            self.CheckCounter(self.vlan10, SWITCH_VLAN_COUNTER_ID_IN_UCAST_PKTS, 1, "IN_UCAST_PKTS")
            self.CheckCounter(self.vlan10, SWITCH_VLAN_COUNTER_ID_IN_UCAST_BYTES, 0, "IN_UCAST_BYTES", False)
            self.CheckCounter(self.vlan10, SWITCH_VLAN_COUNTER_ID_IN_BCAST_PKTS, 1, "IN_BCAST_PKTS")
            self.CheckCounter(self.vlan10, SWITCH_VLAN_COUNTER_ID_IN_BCAST_BYTES, 0, "IN_BCAST_BYTES", False)
            self.CheckCounter(self.vlan10, SWITCH_VLAN_COUNTER_ID_IN_MCAST_PKTS, 0, "IN_MCAST_PKTS")
            self.CheckCounter(self.vlan10, SWITCH_VLAN_COUNTER_ID_IN_MCAST_BYTES, 0, "IN_MCAST_BYTES")

            self.CheckCounter(self.vlan10, SWITCH_VLAN_COUNTER_ID_OUT_UCAST_PKTS, 1, "OUT_UCAST_PKTS")
            self.CheckCounter(self.vlan10, SWITCH_VLAN_COUNTER_ID_OUT_UCAST_BYTES, 0, "OUT_UCAST_BYTES", False)
            self.CheckCounter(self.vlan10, SWITCH_VLAN_COUNTER_ID_OUT_BCAST_PKTS, 1, "OUT_BCAST_PKTS")
            self.CheckCounter(self.vlan10, SWITCH_VLAN_COUNTER_ID_OUT_BCAST_BYTES, 0, "OUT_BCAST_BYTES", False)
            self.CheckCounter(self.vlan10, SWITCH_VLAN_COUNTER_ID_OUT_MCAST_PKTS, 0, "OUT_MCAST_PKTS")
            self.CheckCounter(self.vlan10, SWITCH_VLAN_COUNTER_ID_OUT_MCAST_BYTES, 0, "OUT_MCAST_BYTES")


            cntr_ids = [SWITCH_VLAN_COUNTER_ID_IN_UCAST_BYTES]
            self.client.object_counters_clear(self.vlan10, cntr_ids)

            cntr_ids = [SWITCH_VLAN_COUNTER_ID_OUT_BCAST_BYTES]
            self.client.object_counters_clear(self.vlan10, cntr_ids)

            self.CheckCounter(self.vlan10, SWITCH_VLAN_COUNTER_ID_IN_UCAST_PKTS, 1, "IN_UCAST_PKTS")
            self.CheckCounter(self.vlan10, SWITCH_VLAN_COUNTER_ID_IN_UCAST_BYTES, 0, "IN_UCAST_BYTES")
            self.CheckCounter(self.vlan10, SWITCH_VLAN_COUNTER_ID_IN_BCAST_PKTS, 1, "IN_BCAST_PKTS")
            self.CheckCounter(self.vlan10, SWITCH_VLAN_COUNTER_ID_IN_BCAST_BYTES, 0, "IN_BCAST_BYTES", False)
            self.CheckCounter(self.vlan10, SWITCH_VLAN_COUNTER_ID_IN_MCAST_PKTS, 0, "IN_MCAST_PKTS")
            self.CheckCounter(self.vlan10, SWITCH_VLAN_COUNTER_ID_IN_MCAST_BYTES, 0, "IN_MCAST_BYTES")

            self.CheckCounter(self.vlan10, SWITCH_VLAN_COUNTER_ID_OUT_UCAST_PKTS, 1, "OUT_UCAST_PKTS")
            self.CheckCounter(self.vlan10, SWITCH_VLAN_COUNTER_ID_OUT_UCAST_BYTES, 0, "OUT_UCAST_BYTES", False)
            self.CheckCounter(self.vlan10, SWITCH_VLAN_COUNTER_ID_OUT_BCAST_PKTS, 1, "OUT_BCAST_PKTS")
            self.CheckCounter(self.vlan10, SWITCH_VLAN_COUNTER_ID_OUT_BCAST_BYTES, 0, "OUT_BCAST_BYTES")
            self.CheckCounter(self.vlan10, SWITCH_VLAN_COUNTER_ID_OUT_MCAST_PKTS, 0, "OUT_MCAST_PKTS")
            self.CheckCounter(self.vlan10, SWITCH_VLAN_COUNTER_ID_OUT_MCAST_BYTES, 0, "OUT_MCAST_BYTES")

            self.client.object_counters_clear_all(self.vlan10)

            for i in range(SWITCH_VLAN_COUNTER_ID_IN_UCAST_PKTS, SWITCH_VLAN_COUNTER_ID_MAX):
                cntr_ids = [i]
                self.CheckCounter(self.vlan10, i, 0, str(i))

            for i in range(SWITCH_VLAN_COUNTER_ID_IN_UCAST_PKTS, SWITCH_VLAN_COUNTER_ID_MAX):
                cntr_ids = [i]
                self.client.object_counters_clear(self.vlan10, cntr_ids)
                self.assertEqual(self.status(), 0, "Clear of counter id {} failed".format(i))

        finally:
            pass

    def sendPktSizes(self):
        '''
        This test tries to verify that port stats works correctly
        '''
        print("StatsTest for different size pkts()")

        pkt4 = simple_arp_packet(pktlen=60)
        pkt6 = simple_udp_packet(
            eth_dst='00:11:11:11:11:11',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.0.0.1',
            ip_ttl=64,
            pktlen=70)
        pkt7 = simple_udp_packet(
            eth_dst='00:11:11:11:11:11',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.0.0.1',
            ip_ttl=64,
            pktlen=250)
        pkt8 = simple_udp_packet(
            eth_dst='00:11:11:11:11:11',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.0.0.1',
            ip_ttl=64,
            pktlen=300)
        pkt9 = simple_udp_packet(
            eth_dst='00:11:11:11:11:11',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.0.0.1',
            ip_ttl=64,
            pktlen=600)
        pkt10 = simple_udp_packet(
            eth_dst='00:11:11:11:11:11',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.0.0.1',
            ip_ttl=64,
            pktlen=1500)
        pkt11 = simple_udp_packet(
            eth_dst='00:11:11:11:11:11',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.0.0.1',
            ip_ttl=64,
            pktlen=1600)
        pkt12 = simple_udp_packet(
            eth_dst='00:11:11:11:11:11',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.0.0.1',
            ip_ttl=64,
            pktlen=2500)
        pkt13 = simple_udp_packet(
            eth_dst='00:11:11:11:11:11',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.0.0.1',
            ip_ttl=64,
            pktlen=5000)
        pkt14 = simple_udp_packet(
            eth_dst='00:11:11:11:11:11',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.0.0.1',
            ip_ttl=64,
            pktlen=9000)
        pkt15 = simple_udp_packet(
            eth_dst='00:11:11:11:11:11',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.0.0.1',
            ip_ttl=64,
            pktlen=9212)

        exp_pkt4 = pkt4.copy()
        exp_pkt6 = pkt6.copy()
        exp_pkt7 = pkt7.copy()
        exp_pkt8 = pkt8.copy()
        exp_pkt9 = pkt9.copy()
        exp_pkt10 = pkt10.copy()
        exp_pkt11 = pkt11.copy()
        exp_pkt12 = pkt12.copy()
        exp_pkt13 = pkt13.copy()
        exp_pkt14 = pkt14.copy()
        exp_pkt15 = pkt15.copy()
        try:
            send_packet(self, self.devports[0], pkt4)
            verify_packets(self, exp_pkt4, [self.devports[1]])
            send_packet(self, self.devports[0], pkt6)
            verify_packets(self, exp_pkt6, [self.devports[1]])
            send_packet(self, self.devports[0], pkt7)
            verify_packets(self, exp_pkt7, [self.devports[1]])
            send_packet(self, self.devports[0], pkt8)
            verify_packets(self, exp_pkt8, [self.devports[1]])
            send_packet(self, self.devports[0], pkt9)
            verify_packets(self, exp_pkt9, [self.devports[1]])
            send_packet(self, self.devports[0], pkt10)
            verify_packets(self, exp_pkt10, [self.devports[1]])
            #change the mtu of PTF server
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_RX_MTU, 9500)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_TX_MTU, 9500)
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_RX_MTU, 9500)
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_TX_MTU, 9500)
            send_packet(self, self.devports[0], pkt11)
            verify_packets(self, exp_pkt11, [self.devports[1]])
            send_packet(self, self.devports[0], pkt12)
            verify_packets(self, exp_pkt12, [self.devports[1]])
            send_packet(self, self.devports[0], pkt13)
            verify_packets(self, exp_pkt13, [self.devports[1]])
            send_packet(self, self.devports[0], pkt14)
            verify_packets(self, exp_pkt14, [self.devports[1]])
            send_packet(self, self.devports[0], pkt15)
            verify_packets(self, exp_pkt15, [self.devports[1]])
            time.sleep(2)

            self.CheckCounter(self.port0, SWITCH_PORT_COUNTER_ID_IN_PKTS_EQ_64, 1, "ID_IN_PKTS_64")
            self.CheckCounter(self.port1, SWITCH_PORT_COUNTER_ID_OUT_PKTS_EQ_64, 1, "ID_IN_PKTS_64")
            self.CheckCounter(self.port0, SWITCH_PORT_COUNTER_ID_IN_PKTS_65_TO_127, 1, "ID_IN_PKTS_64_TO_127")
            self.CheckCounter(self.port1, SWITCH_PORT_COUNTER_ID_OUT_PKTS_65_TO_127, 1, "ID_OUT_PKTS_64_TO_127")
            self.CheckCounter(self.port0, SWITCH_PORT_COUNTER_ID_IN_PKTS_128_TO_255, 1, "ID_IN_PKTS_128_TO_255")
            self.CheckCounter(self.port1, SWITCH_PORT_COUNTER_ID_OUT_PKTS_128_TO_255, 1, "ID_OUT_PKTS_128_TO_255")
            self.CheckCounter(self.port0, SWITCH_PORT_COUNTER_ID_IN_PKTS_256_TO_511, 1, "ID_IN_PKTS_256_TO_511")
            self.CheckCounter(self.port1, SWITCH_PORT_COUNTER_ID_OUT_PKTS_256_TO_511, 1, "ID_OUT_PKTS_256_TO_511")
            self.CheckCounter(self.port0, SWITCH_PORT_COUNTER_ID_IN_PKTS_512_TO_1023, 1, "ID_IN_PKTS_512_TO_1023")
            self.CheckCounter(self.port1, SWITCH_PORT_COUNTER_ID_OUT_PKTS_512_TO_1023, 1, "ID_OUT_PKTS_512_TO_1023")
            self.CheckCounter(self.port0, SWITCH_PORT_COUNTER_ID_IN_PKTS_1024_TO_1518, 1, "ID_IN_PKTS_1024_TO_1518")
            self.CheckCounter(self.port1, SWITCH_PORT_COUNTER_ID_OUT_PKTS_1024_TO_1518, 1, "ID_OUT_PKTS_1024_TO_1518")
            self.CheckCounter(self.port0, SWITCH_PORT_COUNTER_ID_IN_PKTS_1519_TO_2047, 1, "ID_IN_PKTS_1519_TO_2047")
            self.CheckCounter(self.port1, SWITCH_PORT_COUNTER_ID_OUT_PKTS_1519_TO_2047, 1, "ID_OUT_PKTS_1519_TO_2047")
            self.CheckCounter(self.port0, SWITCH_PORT_COUNTER_ID_IN_PKTS_2048_TO_4095, 1, "ID_IN_PKTS_2048_TO_4095")
            self.CheckCounter(self.port1, SWITCH_PORT_COUNTER_ID_OUT_PKTS_2048_TO_4095, 1, "ID_OUT_PKTS_2048_TO_4095")
            self.CheckCounter(self.port0, SWITCH_PORT_COUNTER_ID_IN_PKTS_4096_TO_8191, 1, "ID_IN_PKTS_4096_TO_8191")
            self.CheckCounter(self.port1, SWITCH_PORT_COUNTER_ID_OUT_PKTS_4096_TO_8191, 1, "ID_OUT_PKTS_4096_TO_8191")
            self.CheckCounter(self.port0, SWITCH_PORT_COUNTER_ID_IN_PKTS_8192_TO_9215, 1, "ID_IN_PKTS_8192_TO_9215")
            self.CheckCounter(self.port1, SWITCH_PORT_COUNTER_ID_OUT_PKTS_8192_TO_9215, 1, "ID_OUT_PKTS_8192_TO_9215")
            self.CheckCounter(self.port0, SWITCH_PORT_COUNTER_ID_IN_PKTS_9216, 1, "ID_IN_PKTS_9216")
            self.CheckCounter(self.port1, SWITCH_PORT_COUNTER_ID_OUT_PKTS_9216, 1, "ID_OUT_PKTS_9216")
            self.CheckCounter(self.port1, SWITCH_PORT_COUNTER_ID_OUT_OVER_SIZED_PKTS, 5, "ID_OUT_OVER_SIZED_PKTS")
            #import pdb
            #pdb.set_trace()
        finally:
            self.client.object_counters_clear_all(self.port0)
            self.client.object_counters_clear_all(self.port1)


    def PortStatsTest(self):
        '''
        This test tries to verify that port stats works correctly
        '''
        print("StatsTest()")

        pkt = simple_udp_packet(
            eth_dst='00:11:11:11:11:11',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.0.0.1',
            ip_ttl=64)

        exp_pkt = pkt.copy()

        pkt2 = simple_udp_packet(
            eth_dst='00:22:22:22:22:22',
            eth_src='00:11:11:11:11:11',
            ip_dst='20.0.0.1',
            ip_ttl=64)

        pkt3 = simple_udp_packet(
            eth_dst='00:22:22:22:22:22',
            eth_src='00:11:11:11:11:11',
            ip_dst='20.0.0.1',
            ip_ttl=64,
            pktlen=1000)


        exp_pkt3 = pkt3.copy()

        exp_pkt2 = pkt2.copy()

        mcast_pkt = simple_udp_packet(
            eth_dst='01:22:22:22:22:22',
            eth_src='00:11:11:11:11:11',
            ip_dst='22.22.22.22',
            ip_ttl=64)

        exp_mcast_pkt = mcast_pkt.copy()

        bcast_pkt = simple_udp_packet(
            eth_dst='ff:ff:ff:ff:ff:ff',
            eth_src='00:11:11:11:11:11',
            ip_dst='20.0.0.1',
            ip_ttl=64)

        exp_bcast_pkt = bcast_pkt.copy()
        '''packet = Ether()/IP()/ICMP()
        chksum = b"\0\0\0\0"
        packet /= chksum'''

        try:
            self.client.object_counters_clear_all(self.port0)
            self.client.object_counters_clear_all(self.port1)

            print("Sending L2 packet from {} -> {}".format(self.devports[0], self.devports[1]))
            send_packet(self, self.devports[0], pkt2)
            verify_packets(self, exp_pkt2, [self.devports[1]])
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_RX_MTU, 921)
            print("Sending L2 packet3 from {} -> {}".format(self.devports[0], self.devports[1]))
            send_packet(self, self.devports[0], pkt3)
            time.sleep(2)

            self.CheckCounter(self.port0, SWITCH_PORT_COUNTER_ID_IN_GOOD_PKTS, 1, "IN_GOOD_PKTS")
            self.CheckCounter(self.port0, SWITCH_PORT_COUNTER_ID_IN_GOOD_OCTETS, 0, "IN_GOOD_OCTETS", False)
            self.CheckCounter(self.port0, SWITCH_PORT_COUNTER_ID_IN_GOOD_OCTETS, 104, "IN_GOOD_OCTETS")
            self.CheckCounter(self.port0, SWITCH_PORT_COUNTER_ID_IN_ALL_OCTETS, 1108, "IN_ALL_OCTETS")
            self.CheckCounter(self.port0, SWITCH_PORT_COUNTER_ID_IN_OVER_SIZED_PKTS, 1, "IN_OVER_SIZED_PKTS")
            self.CheckCounter(self.port0, SWITCH_PORT_COUNTER_ID_IN_UCAST_PKTS, 1, "IN_UCAST_PKTS")
            self.CheckCounter(self.port0, SWITCH_PORT_COUNTER_ID_IN_MCAST_PKTS, 0, "IN_MCAST_PKTS")
            self.CheckCounter(self.port0, SWITCH_PORT_COUNTER_ID_IN_BCAST_PKTS, 0, "IN_BCAST_PKTS")
            self.CheckCounter(self.port0, SWITCH_PORT_COUNTER_ID_IN_NON_UCAST_PKTS, 0, "IN_NON_UCAST_PKTS")
            self.CheckCounter(self.port0, SWITCH_PORT_COUNTER_ID_OUT_GOOD_PKTS, 0, "OUT_GOOD_PKTS")
            self.CheckCounter(self.port0, SWITCH_PORT_COUNTER_ID_OUT_GOOD_OCTETS, 0, "OUT_GOOD_OCTETS")
            self.CheckCounter(self.port0, SWITCH_PORT_COUNTER_ID_IN_ERROR_PKTS, 1, "IN_ERROR_PKTS")

            self.CheckCounter(self.port1, SWITCH_PORT_COUNTER_ID_OUT_GOOD_PKTS, 1, "OUT_GOOD_PKTS")
            self.CheckCounter(self.port1, SWITCH_PORT_COUNTER_ID_OUT_GOOD_OCTETS, 0, "OUT_GOOD_OCTETS", False)
            self.CheckCounter(self.port1, SWITCH_PORT_COUNTER_ID_OUT_GOOD_OCTETS, 104, "OUT_GOOD_OCTETS")
            self.CheckCounter(self.port1, SWITCH_PORT_COUNTER_ID_OUT_ALL_OCTETS, 104, "OUT_ALL_OCTETS")
            self.CheckCounter(self.port1, SWITCH_PORT_COUNTER_ID_OUT_UCAST_PKTS, 1, "OUT_UCAST_PKTS")
            self.CheckCounter(self.port1, SWITCH_PORT_COUNTER_ID_OUT_MCAST_PKTS, 0, "OUT_MCAST_PKTS")
            self.CheckCounter(self.port1, SWITCH_PORT_COUNTER_ID_OUT_BCAST_PKTS, 0, "OUT_BCAST_PKTS")
            self.CheckCounter(self.port1, SWITCH_PORT_COUNTER_ID_OUT_NON_UCAST_PKTS, 0, "OUT_NON_UCAST_PKTS")
            self.CheckCounter(self.port1, SWITCH_PORT_COUNTER_ID_IN_GOOD_PKTS, 0, "IN_GOOD_PKTS")
            self.CheckCounter(self.port1, SWITCH_PORT_COUNTER_ID_IN_GOOD_OCTETS, 0, "IN_GOOD_OCTETS")

            print("Sending L2 multicast packet from {} -> {}".format(self.devports[0], self.devports[1]))
            send_packet(self, self.devports[0], mcast_pkt)
            verify_packets(self, exp_mcast_pkt, [self.devports[1]])

            print("Sending L2 broadcast packet from {} -> {}".format(self.devports[0], self.devports[1]))
            send_packet(self, self.devports[0], bcast_pkt)
            verify_packets(self, exp_bcast_pkt, [self.devports[1]])

            self.CheckCounter(self.port0, SWITCH_PORT_COUNTER_ID_IN_GOOD_PKTS, 3, "IN_GOOD_PKTS")
            self.CheckCounter(self.port0, SWITCH_PORT_COUNTER_ID_IN_GOOD_OCTETS, 0, "IN_GOOD_OCTETS", False)
            self.CheckCounter(self.port0, SWITCH_PORT_COUNTER_ID_IN_UCAST_PKTS, 1, "IN_GOOD_PKTS")
            self.CheckCounter(self.port0, SWITCH_PORT_COUNTER_ID_IN_MCAST_PKTS, 1, "IN_MCAST_PKTS")
            self.CheckCounter(self.port0, SWITCH_PORT_COUNTER_ID_IN_BCAST_PKTS, 1, "IN_BCAST_PKTS")
            self.CheckCounter(self.port0, SWITCH_PORT_COUNTER_ID_IN_NON_UCAST_PKTS, 2, "IN_NON_UCAST_PKTS")
            self.CheckCounter(self.port0, SWITCH_PORT_COUNTER_ID_OUT_GOOD_PKTS, 0, "OUT_GOOD_PKTS")
            self.CheckCounter(self.port0, SWITCH_PORT_COUNTER_ID_OUT_GOOD_OCTETS, 0, "OUT_GOOD_OCTETS")

            self.CheckCounter(self.port1, SWITCH_PORT_COUNTER_ID_OUT_GOOD_PKTS, 3, "OUT_GOOD_PKTS")
            self.CheckCounter(self.port1, SWITCH_PORT_COUNTER_ID_OUT_GOOD_OCTETS, 0, "OUT_GOOD_OCTETS", False)
            self.CheckCounter(self.port1, SWITCH_PORT_COUNTER_ID_OUT_UCAST_PKTS, 1, "OUT_GOOD_PKTS")
            self.CheckCounter(self.port1, SWITCH_PORT_COUNTER_ID_OUT_MCAST_PKTS, 1, "OUT_MCAST_PKTS")
            self.CheckCounter(self.port1, SWITCH_PORT_COUNTER_ID_OUT_BCAST_PKTS, 1, "OUT_BCAST_PKTS")
            self.CheckCounter(self.port1, SWITCH_PORT_COUNTER_ID_OUT_NON_UCAST_PKTS, 2, "OUT_NON_UCAST_PKTS")
            self.CheckCounter(self.port1, SWITCH_PORT_COUNTER_ID_IN_GOOD_PKTS, 0, "IN_GOOD_PKTS")
            self.CheckCounter(self.port1, SWITCH_PORT_COUNTER_ID_IN_GOOD_OCTETS, 0, "IN_GOOD_OCTETS")

        finally:
            pass

    def PortStatsClearTest(self):
        '''
        This test tries to verify that port stats is cleared correctly by all counters and per counter ID clear
        '''
        print("PortStatsClearTest()")

        pkt = simple_udp_packet(
            eth_dst='00:11:11:11:11:11',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.0.0.1',
            ip_ttl=64)

        exp_pkt = pkt.copy()

        pkt2 = simple_udp_packet(
            eth_dst='00:22:22:22:22:22',
            eth_src='00:11:11:11:11:11',
            ip_dst='20.0.0.1',
            ip_ttl=64)

        exp_pkt2 = pkt2.copy()

        mcast_pkt = simple_udp_packet(
            eth_dst='01:22:22:22:22:22',
            eth_src='00:11:11:11:11:11',
            ip_dst='20.0.0.1',
            ip_ttl=64)

        exp_mcast_pkt = mcast_pkt.copy()

        bcast_pkt = simple_udp_packet(
            eth_dst='ff:ff:ff:ff:ff:ff',
            eth_src='00:11:11:11:11:11',
            ip_dst='20.0.0.1',
            ip_ttl=64)

        exp_bcast_pkt = bcast_pkt.copy()

        try:
            self.client.object_counters_clear_all(self.port0)
            self.client.object_counters_clear_all(self.port1)

            print("Sending L2 packet from {} -> {}".format(self.devports[0], self.devports[1]))
            send_packet(self, self.devports[0], pkt2)
            verify_packets(self, exp_pkt2, [self.devports[1]])

            print("Sending L2 broadcast packet from {} -> {}".format(self.devports[0], self.devports[1]))
            send_packet(self, self.devports[0], bcast_pkt)
            verify_packets(self, exp_bcast_pkt, [self.devports[1]])

            self.CheckCounter(self.port0, SWITCH_PORT_COUNTER_ID_IN_GOOD_PKTS, 2, "IN_GOOD_PKTS")
            self.CheckCounter(self.port0, SWITCH_PORT_COUNTER_ID_IN_GOOD_OCTETS, 0, "IN_GOOD_OCTETS", False)
            self.CheckCounter(self.port0, SWITCH_PORT_COUNTER_ID_IN_UCAST_PKTS, 1, "IN_UCAST_PKTS")
            self.CheckCounter(self.port0, SWITCH_PORT_COUNTER_ID_IN_MCAST_PKTS, 0, "IN_MCAST_PKTS")
            self.CheckCounter(self.port0, SWITCH_PORT_COUNTER_ID_IN_BCAST_PKTS, 1, "IN_BCAST_PKTS")
            self.CheckCounter(self.port0, SWITCH_PORT_COUNTER_ID_IN_NON_UCAST_PKTS, 1, "IN_NON_UCAST_PKTS")

            self.CheckCounter(self.port1, SWITCH_PORT_COUNTER_ID_OUT_GOOD_PKTS, 2, "OUT_GOOD_PKTS")
            self.CheckCounter(self.port1, SWITCH_PORT_COUNTER_ID_OUT_GOOD_OCTETS, 0, "OUT_GOOD_OCTETS", False)
            self.CheckCounter(self.port1, SWITCH_PORT_COUNTER_ID_OUT_UCAST_PKTS, 1, "OUT_UCAST_PKTS")
            self.CheckCounter(self.port1, SWITCH_PORT_COUNTER_ID_OUT_MCAST_PKTS, 0, "OUT_MCAST_PKTS")
            self.CheckCounter(self.port1, SWITCH_PORT_COUNTER_ID_OUT_BCAST_PKTS, 1, "OUT_BCAST_PKTS")
            self.CheckCounter(self.port1, SWITCH_PORT_COUNTER_ID_OUT_NON_UCAST_PKTS, 1, "OUT_NON_UCAST_PKTS")

            # print("Sending L2 multicast packet from {} -> {}".format(self.devports[0], self.devports[1]))
            # send_packet(self, self.devports[0], mcast_pkt)
            # verify_packets(self, exp_mcast_pkt, [self.devports[1]])

            cntr_ids = []
            cntr_ids.append(SWITCH_PORT_COUNTER_ID_IN_NON_UCAST_PKTS)
            self.client.object_counters_clear(self.port0, cntr_ids)

            self.CheckCounter(self.port0, SWITCH_PORT_COUNTER_ID_IN_GOOD_PKTS, 2, "IN_GOOD_PKTS")
            self.CheckCounter(self.port0, SWITCH_PORT_COUNTER_ID_IN_GOOD_OCTETS, 0, "IN_GOOD_OCTETS", False)
            self.CheckCounter(self.port0, SWITCH_PORT_COUNTER_ID_IN_UCAST_PKTS, 1, "IN_UCAST_PKTS")
            self.CheckCounter(self.port0, SWITCH_PORT_COUNTER_ID_IN_MCAST_PKTS, 0, "IN_MCAST_PKTS")
            self.CheckCounter(self.port0, SWITCH_PORT_COUNTER_ID_IN_BCAST_PKTS, 0, "IN_BCAST_PKTS")
            self.CheckCounter(self.port0, SWITCH_PORT_COUNTER_ID_IN_NON_UCAST_PKTS, 0, "IN_NON_UCAST_PKTS")

            cntr_ids = []
            cntr_ids.append(SWITCH_PORT_COUNTER_ID_OUT_NON_UCAST_PKTS)
            self.client.object_counters_clear(self.port1, cntr_ids)

            self.CheckCounter(self.port1, SWITCH_PORT_COUNTER_ID_OUT_GOOD_PKTS, 2, "OUT_GOOD_PKTS")
            self.CheckCounter(self.port1, SWITCH_PORT_COUNTER_ID_OUT_GOOD_OCTETS, 0, "OUT_GOOD_OCTETS", False)
            self.CheckCounter(self.port1, SWITCH_PORT_COUNTER_ID_OUT_UCAST_PKTS, 1, "OUT_UCAST_PKTS")
            self.CheckCounter(self.port1, SWITCH_PORT_COUNTER_ID_OUT_MCAST_PKTS, 0, "OUT_MCAST_PKTS")
            self.CheckCounter(self.port1, SWITCH_PORT_COUNTER_ID_OUT_BCAST_PKTS, 0, "OUT_BCAST_PKTS")
            self.CheckCounter(self.port1, SWITCH_PORT_COUNTER_ID_OUT_NON_UCAST_PKTS, 0, "OUT_NON_UCAST_PKTS")

            self.client.object_counters_clear_all(self.port0)

            self.CheckCounter(self.port0, SWITCH_PORT_COUNTER_ID_IN_GOOD_PKTS, 0, "IN_GOOD_PKTS")
            self.CheckCounter(self.port0, SWITCH_PORT_COUNTER_ID_IN_GOOD_OCTETS, 0, "IN_GOOD_OCTETS")
            self.CheckCounter(self.port0, SWITCH_PORT_COUNTER_ID_IN_UCAST_PKTS, 0, "IN_UCAST_PKTS")
            self.CheckCounter(self.port0, SWITCH_PORT_COUNTER_ID_IN_MCAST_PKTS, 0, "IN_MCAST_PKTS")
            self.CheckCounter(self.port0, SWITCH_PORT_COUNTER_ID_IN_BCAST_PKTS, 0, "IN_BCAST_PKTS")
            self.CheckCounter(self.port0, SWITCH_PORT_COUNTER_ID_IN_NON_UCAST_PKTS, 0, "IN_NON_UCAST_PKTS")

        finally:
            pass
@disabled
class PortSerdes(ApiHelper):
    def setUp(self):
        print()
        self.configure()

    def runTest(self):
        try:
            tx_fir_pre1_list = []
            tx_fir_pre2_list = []
            tx_fir_main_list = []
            tx_fir_post1_list = []
            tx_fir_post2_list = []
            tx_fir_attn_list = []
            for i in range(4):
                tx_fir_pre1_list.append(switcht_list_val_t(type=switcht_value_type.UINT32, u32data=6))
                tx_fir_pre2_list.append(switcht_list_val_t(type=switcht_value_type.UINT32, u32data=0))
                tx_fir_main_list.append(switcht_list_val_t(type=switcht_value_type.UINT32, u32data=0))
                tx_fir_post1_list.append(switcht_list_val_t(type=switcht_value_type.UINT32, u32data=0))
                tx_fir_post2_list.append(switcht_list_val_t(type=switcht_value_type.UINT32, u32data=0))
                tx_fir_attn_list.append(switcht_list_val_t(type=switcht_value_type.UINT32, u32data=0))
            self.add_port_serdes(self.device, tx_fir_pre1=tx_fir_pre1_list, tx_fir_main =tx_fir_main_list, tx_fir_post1=tx_fir_post1_list, tx_fir_post2=tx_fir_post2_list, tx_fir_attn=tx_fir_attn_list,port_id=self.port0,tx_fir_pre2=tx_fir_pre2_list)

        finally:
            pass

    def tearDown(self):
        self.cleanup()
        pass
