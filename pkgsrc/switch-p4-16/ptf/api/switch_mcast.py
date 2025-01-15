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
Thrift API interface basic multicast tests
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

from ptf import config
from ptf.testutils import *
from ptf.packet import *
from ptf.thriftutils import *

import os

from bf_switcht_api_thrift.ttypes import *
from bf_switcht_api_thrift.model_headers import *

this_dir = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(this_dir, '..'))
from common.utils import *


@group('mcast')
class SimpleL2MCIpv4Test(ApiHelper):
    '''
    Tests for L2 multicast functionalities
    Topology:
        Port    Mode    VLAN    MC grp
        port0    A       10      1, 2
        port1    A       10       1
        port2    A       10      1, 2
        port3    T      10, 20   1, 3
        lag0     A       10       2
          port4
          port5
        port6    T      10, 20    3
        port7    A       20       3
        port8    A       10      None
    '''

    def is_test_disabled(self):
        if (self.client.is_feature_enable(SWITCH_FEATURE_MULTICAST) == 0):
            return True
        return False

    def setUp(self):
        self.configure()

        if (self.is_test_disabled()):
            print("Multicast feature not enabled, skipping")
            return

        self.dst_mac = "01:00:5e:01:01:05"
        self.src_mac = "00:22:22:22:22:22"
        self.grp_ip1 = "230.1.1.5"  # used for (*, G) and (S, G) paths
        self.grp_ip2 = "230.1.1.10"  # used for (*, G) only
        self.src_ip1 = "10.0.10.5"  # used for (*, G) only
        self.src_ip2 = "10.0.10.10"  # used for (S, G) only

        # configure VLAN 10
        self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port0)
        self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port1)
        self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port2)
        self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port3, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
        self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port6, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
        self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port8)

        self.lag0 = self.add_lag(self.device)
        self.lag_mbr = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port4)
        self.lag_mbr = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port5)

        self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.lag0)

        for port in [self.port0, self.port1, self.port2, self.port8]:
            self.attribute_set(port, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)

        self.attribute_set(self.lag0, SWITCH_LAG_ATTR_PORT_VLAN_ID, 10)

        # configure VLAN 20
        self.add_vlan_member(self.device, vlan_handle=self.vlan20, member_handle=self.port3, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
        self.add_vlan_member(self.device, vlan_handle=self.vlan20, member_handle=self.port6, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
        self.add_vlan_member(self.device, vlan_handle=self.vlan20, member_handle=self.port7)

        self.attribute_set(self.port7, SWITCH_PORT_ATTR_PORT_VLAN_ID, 20)

        # create l2mc groups and members
        # group 1: port0, port1, port2, port3
        self.l2mc_grp1 = self.add_l2mc_group(self.device)

        self.l2mc_mbr = self.add_l2mc_member(
            self.device, l2mc_group_handle=self.l2mc_grp1, output_handle=self.port0)
        self.l2mc_mbr = self.add_l2mc_member(
            self.device, l2mc_group_handle=self.l2mc_grp1, output_handle=self.port1)
        self.l2mc_mbr = self.add_l2mc_member(
            self.device, l2mc_group_handle=self.l2mc_grp1, output_handle=self.port2)
        self.l2mc_mbr = self.add_l2mc_member(
            self.device, l2mc_group_handle=self.l2mc_grp1, output_handle=self.port3)

        # group 2: port0, port2, lag0 (port4, port5)
        self.l2mc_grp2 = self.add_l2mc_group(self.device)

        self.l2mc_mbr = self.add_l2mc_member(
            self.device, l2mc_group_handle=self.l2mc_grp2, output_handle=self.port0)
        self.l2mc_mbr = self.add_l2mc_member(
            self.device, l2mc_group_handle=self.l2mc_grp2, output_handle=self.port2)
        self.l2mc_mbr = self.add_l2mc_member(
            self.device, l2mc_group_handle=self.l2mc_grp2, output_handle=self.lag0)

        # group 3: port3, port6, port7
        self.l2mc_grp3 = self.add_l2mc_group(self.device)

        self.l2mc_mbr = self.add_l2mc_member(
            self.device, l2mc_group_handle=self.l2mc_grp3, output_handle=self.port3)
        self.l2mc_mbr = self.add_l2mc_member(
            self.device, l2mc_group_handle=self.l2mc_grp3, output_handle=self.port6)
        self.l2mc_mbr = self.add_l2mc_member(
            self.device, l2mc_group_handle=self.l2mc_grp3, output_handle=self.port7)

        # configure L2MC forwarding paths
        # (*, G) path to group 1
        self.l2mc_bridge = self.add_l2mc_bridge(
            self.device, grp_ip=self.grp_ip1, vlan_handle=self.vlan10, group_handle=self.l2mc_grp1)
        # (*, G) path to group 2
        self.l2mc_bridge = self.add_l2mc_bridge(
            self.device, grp_ip=self.grp_ip2, vlan_handle=self.vlan10, group_handle=self.l2mc_grp2)
        # (S, G) path to group 3
        self.l2mc_bridge = self.add_l2mc_bridge(
            self.device, grp_ip=self.grp_ip1, src_ip=self.src_ip2, vlan_handle=self.vlan20, group_handle=self.l2mc_grp3)

    def runTest(self):
        if (self.is_test_disabled()):
            print("Multicast feature not enabled, skipping")
            return

        self.L2MCTaggingTest()
        self.L2MCLagTest()
        self.L2MCSGTest()

    def tearDown(self):
        for port in [self.port0, self.port1, self.port2, self.port3, self.port7, self.port8]:
            self.attribute_set(port, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)

        self.cleanup()

    def L2MCTaggingTest(self):
        '''
        Exercise ipv4_multicast_bridge_star_g with a group containing tagged port
        Tagged packet is sent with a dest IP address set for two groups:
        Group 1 pointed by (*, G) bridge and Group 3 pointed by (S, G) bridge
        - should be received only on ports in Group 1
        '''
        print("\nL2MCTaggingTest()")
        pkt = simple_udp_packet(
            eth_dst=self.dst_mac,
            eth_src=self.src_mac,
            ip_src=self.src_ip1,
            ip_dst=self.grp_ip1,
            pktlen=100)
        tag_pkt = simple_udp_packet(
            eth_dst=self.dst_mac,
            eth_src=self.src_mac,
            ip_src=self.src_ip1,
            ip_dst=self.grp_ip1,
            dl_vlan_enable=True,
            vlan_vid=10,
            pktlen=104)

        print("Sending untagged packet to multicast group 1")
        send_packet(self, self.devports[0], pkt)
        verify_each_packet_on_each_port(self, [pkt, pkt, tag_pkt],
            [self.devports[1], self.devports[2], self.devports[3]])
        print("\tOK")

        print("Sending tagged packet to multicast group 1")
        send_packet(self, self.devports[3], tag_pkt)
        verify_packets(self, pkt, [self.devports[0], self.devports[1], self.devports[2]])
        print("\tOK")

        print("Sending packet to port not belonging to the group - flood within group 1")
        send_packet(self, self.devports[8], pkt)
        verify_each_packet_on_each_port(
            self, [pkt, pkt, pkt, tag_pkt],
            [self.devports[0], self.devports[1], self.devports[2], self.devports[3]])
        print("\tOK")

    def L2MCLagTest(self):
        '''
        Exercise ipv4_multicast_bridge_star_g with a group containing LAG
        '''
        print("\nL2MCLagTest()")
        pkt = simple_udp_packet(
            eth_dst=self.dst_mac,
            eth_src=self.src_mac,
            ip_src=self.src_ip1,
            ip_dst=self.grp_ip2,
            pktlen=100)
        tag_pkt = simple_udp_packet(
            eth_dst=self.dst_mac,
            eth_src=self.src_mac,
            ip_src=self.src_ip1,
            ip_dst=self.grp_ip2,
            dl_vlan_enable=True,
            vlan_vid=20,
            pktlen=104)

        print("Sending packet to multicast group 2")
        send_packet(self, self.devports[0], pkt)
        verify_each_packet_on_multiple_port_lists(
            self, [pkt, pkt], [[self.devports[2]], [self.devports[4], self.devports[5]]])
        print("\tOK")

        print("Sending packet to port not belonging to the VLAN - flood within group 3")
        send_packet(self, self.devports[7], pkt)
        verify_packets(self, tag_pkt, [self.devports[3], self.devports[6]])
        print("\tOK")

    def L2MCSGTest(self):
        '''
        Exercise ipv4_multicast_bridge_s_g with a group containing tagged ports
        grp_ip is the same as for path directing to l2mc_grp1 but src_ip should point to l2mc_grp3
        Packet is sent with a dest IP address set for groups: Group 1 pointed by (*, G) bridge
        and Group 3 pointed by (S, G) bridge - should be received only on ports in Group 3
        '''
        print("\nL2MCSGTest()")
        pkt = simple_udp_packet(
            eth_dst=self.dst_mac,
            eth_src=self.src_mac,
            ip_src=self.src_ip2,
            ip_dst=self.grp_ip1,
            pktlen=100)
        tag_pkt = simple_udp_packet(
            eth_dst=self.dst_mac,
            eth_src=self.src_mac,
            ip_src=self.src_ip2,
            ip_dst=self.grp_ip1,
            dl_vlan_enable=True,
            vlan_vid=20,
            pktlen=104)

        print("Sending packet to multicast group 3")
        send_packet(self, self.devports[3], tag_pkt)
        verify_each_packet_on_each_port(
            self, [tag_pkt, pkt], [self.devports[6], self.devports[7]])
        print("\tOK")

@group('mcast')
class SimpleL2MCIpv6Test(ApiHelper):
    '''
    Tests for L2 multicast functionalities
    Topology:
        Port    Mode    VLAN    MC grp
        port0    A       10      1, 2
        port1    A       10       1
        port2    A       10      1, 2
        port3    T      10, 20   1, 3
        lag0     A       10       2
          port4
          port5
        port6    T      10, 20    3
        port7    A       20       3
        port8    A       10      None
    '''

    def is_test_disabled(self):
        if (self.client.is_feature_enable(SWITCH_FEATURE_MULTICAST) == 0):
            return True
        return False

    def setUp(self):
        self.configure()

        if (self.is_test_disabled()):
            print("Multicast feature not enabled, skipping")
            return

        self.dst_mac = "01:00:5e:01:01:05"
        self.src_mac = "00:22:22:22:22:22"
        self.grp_ip1 = "ffbf::5"  # used for (*, G) and (S, G) paths
        self.grp_ip2 = "ffbf::10"  # used for (*, G) only
        self.src_ip1 = "2001:0db8::5"  # used for (*, G) only
        self.src_ip2 = "2001:0db8::10"  # used for (S, G) only

        # configure VLAN 10
        self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port0)
        self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port1)
        self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port2)
        self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port3, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
        self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port6, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
        self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port8)

        self.lag0 = self.add_lag(self.device)
        self.lag_mbr = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port4)
        self.lag_mbr = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port5)

        self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.lag0)

        for port in [self.port0, self.port1, self.port2, self.port8]:
            self.attribute_set(port, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)

        self.attribute_set(self.lag0, SWITCH_LAG_ATTR_PORT_VLAN_ID, 10)

        # configure VLAN 20
        self.add_vlan_member(self.device, vlan_handle=self.vlan20, member_handle=self.port3, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
        self.add_vlan_member(self.device, vlan_handle=self.vlan20, member_handle=self.port6, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
        self.add_vlan_member(self.device, vlan_handle=self.vlan20, member_handle=self.port7)

        self.attribute_set(self.port7, SWITCH_PORT_ATTR_PORT_VLAN_ID, 20)

        # create l2mc groups and members
        # group 1: port0, port1, port2, port3
        self.l2mc_grp1 = self.add_l2mc_group(self.device)

        self.l2mc_mbr = self.add_l2mc_member(
            self.device, l2mc_group_handle=self.l2mc_grp1, output_handle=self.port0)
        self.l2mc_mbr = self.add_l2mc_member(
            self.device, l2mc_group_handle=self.l2mc_grp1, output_handle=self.port1)
        self.l2mc_mbr = self.add_l2mc_member(
            self.device, l2mc_group_handle=self.l2mc_grp1, output_handle=self.port2)
        self.l2mc_mbr = self.add_l2mc_member(
            self.device, l2mc_group_handle=self.l2mc_grp1, output_handle=self.port3)

        # group 2: port0, port2, lag0 (port4, port5)
        self.l2mc_grp2 = self.add_l2mc_group(self.device)

        self.l2mc_mbr = self.add_l2mc_member(
            self.device, l2mc_group_handle=self.l2mc_grp2, output_handle=self.port0)
        self.l2mc_mbr = self.add_l2mc_member(
            self.device, l2mc_group_handle=self.l2mc_grp2, output_handle=self.port2)
        self.l2mc_mbr = self.add_l2mc_member(
            self.device, l2mc_group_handle=self.l2mc_grp2, output_handle=self.lag0)

        # group 3: port3, port6, port7
        self.l2mc_grp3 = self.add_l2mc_group(self.device)

        self.l2mc_mbr = self.add_l2mc_member(
            self.device, l2mc_group_handle=self.l2mc_grp3, output_handle=self.port3)
        self.l2mc_mbr = self.add_l2mc_member(
            self.device, l2mc_group_handle=self.l2mc_grp3, output_handle=self.port6)
        self.l2mc_mbr = self.add_l2mc_member(
            self.device, l2mc_group_handle=self.l2mc_grp3, output_handle=self.port7)

        # configure L2MC forwarding paths
        # (*, G) path to group 1
        self.l2mc_bridge = self.add_l2mc_bridge(
            self.device, grp_ip=self.grp_ip1, vlan_handle=self.vlan10, group_handle=self.l2mc_grp1)
        # (*, G) path to group 2
        self.l2mc_bridge = self.add_l2mc_bridge(
            self.device, grp_ip=self.grp_ip2, vlan_handle=self.vlan10, group_handle=self.l2mc_grp2)
        # (S, G) path to group 3
        self.l2mc_bridge = self.add_l2mc_bridge(
            self.device, grp_ip=self.grp_ip1, src_ip=self.src_ip2, vlan_handle=self.vlan20, group_handle=self.l2mc_grp3)

    def runTest(self):
        if (self.is_test_disabled()):
            print("Multicast feature not enabled, skipping")
            return

        self.L2MCTaggingTest()
        self.L2MCLagTest()
        self.L2MCSGTest()

    def tearDown(self):
        for port in [self.port0, self.port1, self.port2, self.port3, self.port7, self.port8]:
            self.attribute_set(port, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)

        self.cleanup()

    def L2MCTaggingTest(self):
        '''
        Exercise ipv6_multicast_bridge_star_g with a group containing tagged port
        Tagged packet is sent with a dest IP address set for groups:
        Group 1 pointed by (*, G) bridge and Group 3 pointed by (S, G) bridge
        - should be received only on ports in Group 1
        '''
        print("\nL2MCTaggingTest()")
        pkt = simple_udpv6_packet(
            eth_dst=self.dst_mac,
            eth_src=self.src_mac,
            ipv6_src=self.src_ip1,
            ipv6_dst=self.grp_ip1,
            pktlen=100)
        tag_pkt = simple_udpv6_packet(
            eth_dst=self.dst_mac,
            eth_src=self.src_mac,
            ipv6_src=self.src_ip1,
            ipv6_dst=self.grp_ip1,
            dl_vlan_enable=True,
            vlan_vid=10,
            pktlen=104)

        print("Sending untagged packet to multicast group 1")
        send_packet(self, self.devports[0], pkt)
        verify_each_packet_on_each_port(self, [pkt, pkt, tag_pkt],
            [self.devports[1], self.devports[2], self.devports[3]])
        print("\tOK")

        print("Sending tagged packet to multicast group 1")
        send_packet(self, self.devports[3], tag_pkt)
        verify_packets(self, pkt, [self.devports[0], self.devports[1], self.devports[2]])
        print("\tOK")

        print("Sending packet to port not belonging to the group - flood within group 1")
        send_packet(self, self.devports[8], pkt)
        verify_each_packet_on_each_port(
            self, [pkt, pkt, pkt, tag_pkt],
            [self.devports[0], self.devports[1], self.devports[2], self.devports[3]])
        print("\tOK")

    def L2MCLagTest(self):
        '''
        Exercise ipv6_multicast_bridge_star_g with a group containing LAG
        '''
        print("\nL2MCLagTest()")
        pkt = simple_udpv6_packet(
            eth_dst=self.dst_mac,
            eth_src=self.src_mac,
            ipv6_src=self.src_ip1,
            ipv6_dst=self.grp_ip2,
            pktlen=100)
        tag_pkt = simple_udpv6_packet(
            eth_dst=self.dst_mac,
            eth_src=self.src_mac,
            ipv6_src=self.src_ip1,
            ipv6_dst=self.grp_ip2,
            dl_vlan_enable=True,
            vlan_vid=20,
            pktlen=104)

        print("Sending packet to multicast group 2")
        send_packet(self, self.devports[0], pkt)
        verify_each_packet_on_multiple_port_lists(
            self, [pkt, pkt], [[self.devports[2]], [self.devports[4], self.devports[5]]])
        print("\tOK")

        print("Sending packet to port not belonging to the VLAN - flood within group 3")
        send_packet(self, self.devports[7], pkt)
        verify_packets(self, tag_pkt, [self.devports[3], self.devports[6]])
        print("\tOK")

    def L2MCSGTest(self):
        '''
        Exercise ipv6_multicast_bridge_s_g with a group containing tagged ports
        grp_ip is the same as for path directing to l2mc_grp1 but src_ip should point to l2mc_grp3
        Packet is sent with a dest IP address set for groups: Group 1 pointed by (*, G) bridge
        and Group 3 pointed by (S, G) bridge - should be received only on ports in Group 3
        '''
        print("\nL2MCSGTest()")
        pkt = simple_udpv6_packet(
            eth_dst=self.dst_mac,
            eth_src=self.src_mac,
            ipv6_src=self.src_ip2,
            ipv6_dst=self.grp_ip1,
            pktlen=100)
        tag_pkt = simple_udpv6_packet(
            eth_dst=self.dst_mac,
            eth_src=self.src_mac,
            ipv6_src=self.src_ip2,
            ipv6_dst=self.grp_ip1,
            dl_vlan_enable=True,
            vlan_vid=20,
            pktlen=104)

        print("Sending packet to multicast group 3")
        send_packet(self, self.devports[3], tag_pkt)
        verify_each_packet_on_each_port(
            self, [tag_pkt, pkt], [self.devports[6], self.devports[7]])
        print("\tOK")


@group('mcast')
class SimpleIPMCIpv4Test(ApiHelper):
    '''
    Tests for IPv4 multicast functionalities
    Topology:
        Port        MC grp
        port0        1, 2
        port1         1
        port2        1, 2
        port3        1, 3
        lag0          2
          port4
          port5
        port6         3
        port7         3
    '''

    def is_test_disabled(self):
        if (self.client.is_feature_enable(SWITCH_FEATURE_MULTICAST) == 0):
            return True
        return False

    def setUp(self):
        self.configure()

        if (self.is_test_disabled()):
            print("Multicast feature not enabled, skipping")
            return

        self.dst_mac = "01:00:5e:01:01:05"
        self.src_mac = "00:22:22:22:22:22"
        self.grp_ip1 = "230.1.1.5"  # used for (*, G) and (S, G) paths
        self.grp_ip2 = "230.1.1.10"  # used for (*, G) only
        self.src_ip1 = "10.0.10.5"  # used for (*, G) only
        self.src_ip2 = "10.0.10.10"  # used for (S, G) only

        # configure RIFs
        self.rif0 = self.add_rif(
            self.device, vrf_handle=self.default_vrf, type=SWITCH_RIF_ATTR_TYPE_PORT,
            port_handle=self.port0, src_mac=self.rmac, ipv4_multicast=True)
        self.rif1 = self.add_rif(
            self.device, vrf_handle=self.default_vrf, type=SWITCH_RIF_ATTR_TYPE_PORT,
            port_handle=self.port1, src_mac=self.rmac, ipv4_multicast=True)
        self.rif2 = self.add_rif(
            self.device, vrf_handle=self.default_vrf, type=SWITCH_RIF_ATTR_TYPE_PORT,
            port_handle=self.port2, src_mac=self.rmac, ipv4_multicast=True)
        self.rif3 = self.add_rif(
            self.device, vrf_handle=self.default_vrf, type=SWITCH_RIF_ATTR_TYPE_PORT,
            port_handle=self.port3, src_mac=self.rmac, ipv4_multicast=True)

        self.lag0 = self.add_lag(self.device)
        self.lag_mbr = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port4)
        self.lag_mbr = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port5)
        self.lag_rif = self.add_rif(
            self.device, vrf_handle=self.default_vrf, type=SWITCH_RIF_ATTR_TYPE_PORT,
            port_handle=self.lag0, src_mac=self.rmac, ipv4_multicast=True)

        self.rif6 = self.add_rif(
            self.device, vrf_handle=self.default_vrf, type=SWITCH_RIF_ATTR_TYPE_PORT,
            port_handle=self.port6, src_mac=self.rmac, ipv4_multicast=True)
        self.rif7 = self.add_rif(
            self.device, vrf_handle=self.default_vrf, type=SWITCH_RIF_ATTR_TYPE_PORT,
            port_handle=self.port7, src_mac=self.rmac, ipv4_multicast=True)

        # create ipmc groups and members
        # group 1: port0, port1, port2, port3)
        self.ipmc_grp1 = self.add_ipmc_group(self.device)

        self.ipmc_member = self.add_ipmc_member(
            self.device, ipmc_group_handle=self.ipmc_grp1, output_handle=self.rif0)
        self.ipmc_member = self.add_ipmc_member(
            self.device, ipmc_group_handle=self.ipmc_grp1, output_handle=self.rif1)
        self.ipmc_member = self.add_ipmc_member(
            self.device, ipmc_group_handle=self.ipmc_grp1, output_handle=self.rif2)
        self.ipmc_member = self.add_ipmc_member(
            self.device, ipmc_group_handle=self.ipmc_grp1, output_handle=self.rif3)

        # group 2: port0, port2, lag0 (port4, port5)
        self.ipmc_grp2 = self.add_ipmc_group(self.device)

        self.ipmc_member = self.add_ipmc_member(
            self.device, ipmc_group_handle=self.ipmc_grp2, output_handle=self.rif0)
        self.ipmc_member = self.add_ipmc_member(
            self.device, ipmc_group_handle=self.ipmc_grp2, output_handle=self.rif2)
        self.ipmc_member = self.add_ipmc_member(
            self.device, ipmc_group_handle=self.ipmc_grp2, output_handle=self.lag_rif)

        # group 3: port3, port6, port7
        self.ipmc_grp3 = self.add_ipmc_group(self.device)

        self.ipmc_member = self.add_ipmc_member(
            self.device, ipmc_group_handle=self.ipmc_grp3, output_handle=self.rif3)
        self.ipmc_member = self.add_ipmc_member(
            self.device, ipmc_group_handle=self.ipmc_grp3, output_handle=self.rif6)
        self.ipmc_member = self.add_ipmc_member(
            self.device, ipmc_group_handle=self.ipmc_grp3, output_handle=self.rif7)

        # configure IPMC forwarding paths and rpf groups - in case of SM PIM mode there may be only one ingress rif
        # (*, G) route to group 1
        self.rpf_grp1 = self.add_rpf_group(self.device, pim_mode=SWITCH_IPMC_ROUTE_ATTR_PIM_MODE_SM)
        self.rpm_mbr = self.add_rpf_member(self.device, rpf_group_handle=self.rpf_grp1, handle=self.rif0)
        self.ipmc_route = self.add_ipmc_route(
            self.device, grp_ip=self.grp_ip1, vrf_handle=self.default_vrf, group_handle=self.ipmc_grp1,
            pim_mode=SWITCH_IPMC_ROUTE_ATTR_PIM_MODE_SM, rpf_group_handle=self.rpf_grp1)
        # (*, G) route to group 2
        self.ipmc_route = self.add_ipmc_route(
            self.device, grp_ip=self.grp_ip2, vrf_handle=self.default_vrf, group_handle=self.ipmc_grp2,
            pim_mode=SWITCH_IPMC_ROUTE_ATTR_PIM_MODE_SM, rpf_group_handle=self.rpf_grp1)
        # (S, G) route to group 3
        self.rpf_grp2 = self.add_rpf_group(self.device, pim_mode=SWITCH_IPMC_ROUTE_ATTR_PIM_MODE_SM)
        self.rpm_mbr = self.add_rpf_member(self.device, rpf_group_handle=self.rpf_grp2, handle=self.rif6)
        self.ipmc_route = self.add_ipmc_route(
            self.device, grp_ip=self.grp_ip1, src_ip=self.src_ip2, vrf_handle=self.default_vrf,
            group_handle=self.ipmc_grp3, pim_mode=SWITCH_IPMC_ROUTE_ATTR_PIM_MODE_SM, rpf_group_handle=self.rpf_grp2)

    def runTest(self):
        if (self.is_test_disabled()):
            print("Multicast feature not enabled, skipping")
            return

        self.IPMCXGTest()
        self.IPMCXGLagTest()
        self.IPMCSGTest()
        self.IPv4McastAdminStateTest()

    def tearDown(self):
        self.cleanup()

    def IPMCXGTest(self):
        '''
        Exercice ipv4_multicast_route_star_g
        Packet is sent with a dest IP address set for groups:
        Group 1 pointed by (*, G) route and Group 3 pointed by (S, G) route -
        should be received only on ports in Group 1
        '''
        print("\nIPMCXGTest()")

        pkt = simple_udp_packet(
            eth_dst=self.dst_mac,
            eth_src=self.src_mac,
            ip_src=self.src_ip1,
            ip_dst=self.grp_ip1,
            ip_ttl=64)
        exp_pkt = simple_udp_packet(
            eth_dst=self.dst_mac,
            eth_src=self.rmac,
            ip_src=self.src_ip1,
            ip_dst=self.grp_ip1,
            ip_ttl=63)

        print("Sending packet to multicast group 1 \n"
              "IPv4 multicast hit, RPF pass, action route")
        send_packet(self, self.devports[0], pkt)
        verify_packets(self, exp_pkt, [self.devports[1], self.devports[2], self.devports[3]])
        print("\tOK")

        print("Sending packet to multicast group 1 \n"
              "IPv4 multicast hit, RPF fail, action drop")
        send_packet(self, self.devports[1], pkt)
        verify_no_other_packets(self)
        print("\tOK")

    def IPMCXGLagTest(self):
        '''
        Exercice ipv4_multicast_route_star_g with a group containing LAG
        '''
        print("\nIPMCXGLagTest()")

        pkt = simple_udp_packet(
            eth_dst=self.dst_mac,
            eth_src=self.src_mac,
            ip_src=self.src_ip1,
            ip_dst=self.grp_ip2,
            ip_ttl=64)
        exp_pkt = simple_udp_packet(
            eth_dst=self.dst_mac,
            eth_src=self.rmac,
            ip_src=self.src_ip1,
            ip_dst=self.grp_ip2,
            ip_ttl=63)

        print("Sending packet to multicast group 2 \n"
              "IPv4 multicast hit, RPF pass, action route")
        send_packet(self, self.devports[0], pkt)
        verify_each_packet_on_multiple_port_lists(
            self, [exp_pkt, exp_pkt], [[self.devports[2]], [self.devports[4], self.devports[5]]])
        print("\tOK")

        print("Sending packet to multicast group 2 \n"
              "IPv4 multicast hit, RPF fail, action drop")
        send_packet(self, self.devports[4], pkt)
        verify_no_other_packets(self)
        print("\tOK")

    def IPMCSGTest(self):
        '''
        Exercice ipv4_multicast_route_s_g
        Packet is sent with a dest IP address set for groups:
        Group 1 pointed by (*, G) route and Group 3 pointed by (S, G) route -
        should be received only on ports in Group 3
        '''
        print("\nIPMCSGTest()")

        pkt = simple_udp_packet(
            eth_dst=self.dst_mac,
            eth_src=self.src_mac,
            ip_src=self.src_ip2,
            ip_dst=self.grp_ip1,
            ip_ttl=64)
        exp_pkt = simple_udp_packet(
            eth_dst=self.dst_mac,
            eth_src=self.rmac,
            ip_src=self.src_ip2,
            ip_dst=self.grp_ip1,
            ip_ttl=63)

        print("Sending packet to multicast group 3 \n"
              "IPv4 multicast hit, RPF pass, action route")
        send_packet(self, self.devports[6], pkt)
        verify_packets(self, exp_pkt, [self.devports[3], self.devports[7]])
        print("\tOK")

        print("Sending packet to multicast group 3 \n"
              "IPv4 multicast hit, RPF fail, action drop")
        send_packet(self, self.devports[7], pkt)
        verify_no_other_packets(self)
        print("\tOK")

    def IPv4McastAdminStateTest(self):
        '''
        Verify multicast is disabled when IPv4 admin state on an ingress RIF is False
        '''
        print("\nIPv4McastAdminStateTest()")

        pkt = simple_udp_packet(
            eth_dst=self.dst_mac,
            eth_src=self.src_mac,
            ip_src=self.src_ip1,
            ip_dst=self.grp_ip1,
            ip_ttl=64)
        exp_pkt = simple_udp_packet(
            eth_dst=self.dst_mac,
            eth_src=self.rmac,
            ip_src=self.src_ip1,
            ip_dst=self.grp_ip1,
            ip_ttl=63)

        print("Ingress RIF multicast disabled")
        self.attribute_set(self.rif0, SWITCH_RIF_ATTR_IPV4_MULTICAST, False)
        print("Sending packet to multicast group 1 - drop")
        send_packet(self, self.devports[0], pkt)
        verify_no_other_packets(self)
        print("\tOK")

        print("Ingress RIF multicast enabled")
        self.attribute_set(self.rif0, SWITCH_RIF_ATTR_IPV4_MULTICAST, True)
        print("Sending packet to multicast group 1 - route")
        send_packet(self, self.devports[0], pkt)
        verify_packets(self, exp_pkt, [self.devports[1], self.devports[2], self.devports[3]])
        print("\tOK")


@group('mcast')
class SimpleIPMCIpv6Test(ApiHelper):
    '''
    Tests for IPv6 multicast functionalities
    Topology:
        Port        MC grp
        port0        1, 2
        port1         1
        port2        1, 2
        port3        1, 3
        lag0          2
          port4
          port5
        port6         3
        port7         3
    '''

    def is_test_disabled(self):
        if (self.client.is_feature_enable(SWITCH_FEATURE_MULTICAST) == 0):
            return True
        return False

    def setUp(self):
        self.configure()

        if (self.is_test_disabled()):
            print("Multicast feature not enabled, skipping")
            return

        self.dst_mac = "01:00:5e:01:01:05"
        self.src_mac = "00:22:22:22:22:22"
        self.grp_ip1 = "ffbf::5"  # used for (*, G) and (S, G) paths
        self.grp_ip2 = "ffbf::10"  # used for (*, G) only
        self.src_ip1 = "2001:0db8::5"  # used for (*, G) only
        self.src_ip2 = "2001:0db8::10"  # used for (S, G) only

        # configure RIFs
        self.rif0 = self.add_rif(
            self.device, vrf_handle=self.default_vrf, type=SWITCH_RIF_ATTR_TYPE_PORT,
            port_handle=self.port0, src_mac=self.rmac, ipv6_multicast=True)
        self.rif1 = self.add_rif(
            self.device, vrf_handle=self.default_vrf, type=SWITCH_RIF_ATTR_TYPE_PORT,
            port_handle=self.port1, src_mac=self.rmac, ipv6_multicast=True)
        self.rif2 = self.add_rif(
            self.device, vrf_handle=self.default_vrf, type=SWITCH_RIF_ATTR_TYPE_PORT,
            port_handle=self.port2, src_mac=self.rmac, ipv6_multicast=True)
        self.rif3 = self.add_rif(
            self.device, vrf_handle=self.default_vrf, type=SWITCH_RIF_ATTR_TYPE_PORT,
            port_handle=self.port3, src_mac=self.rmac, ipv6_multicast=True)

        self.lag0 = self.add_lag(self.device)
        self.lag_mbr = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port4)
        self.lag_mbr = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port5)
        self.lag_rif = self.add_rif(
            self.device, vrf_handle=self.default_vrf, type=SWITCH_RIF_ATTR_TYPE_PORT,
            port_handle=self.lag0, src_mac=self.rmac, ipv6_multicast=True)

        self.rif6 = self.add_rif(
            self.device, vrf_handle=self.default_vrf, type=SWITCH_RIF_ATTR_TYPE_PORT,
            port_handle=self.port6, src_mac=self.rmac, ipv6_multicast=True)
        self.rif7 = self.add_rif(
            self.device, vrf_handle=self.default_vrf, type=SWITCH_RIF_ATTR_TYPE_PORT,
            port_handle=self.port7, src_mac=self.rmac, ipv6_multicast=True)

        # create ipmc groups and members
        # group 1: port0, port1, port2, port3)
        self.ipmc_grp1 = self.add_ipmc_group(self.device)

        self.ipmc_member = self.add_ipmc_member(
            self.device, ipmc_group_handle=self.ipmc_grp1, output_handle=self.rif0)
        self.ipmc_member = self.add_ipmc_member(
            self.device, ipmc_group_handle=self.ipmc_grp1, output_handle=self.rif1)
        self.ipmc_member = self.add_ipmc_member(
            self.device, ipmc_group_handle=self.ipmc_grp1, output_handle=self.rif2)
        self.ipmc_member = self.add_ipmc_member(
            self.device, ipmc_group_handle=self.ipmc_grp1, output_handle=self.rif3)

        # group 2: port0, port2, lag0 (port4, port5)
        self.ipmc_grp2 = self.add_ipmc_group(self.device)

        self.ipmc_member = self.add_ipmc_member(
            self.device, ipmc_group_handle=self.ipmc_grp2, output_handle=self.rif0)
        self.ipmc_member = self.add_ipmc_member(
            self.device, ipmc_group_handle=self.ipmc_grp2, output_handle=self.rif2)
        self.ipmc_member = self.add_ipmc_member(
            self.device, ipmc_group_handle=self.ipmc_grp2, output_handle=self.lag_rif)

        # group 3: port3, port6, port7
        self.ipmc_grp3 = self.add_ipmc_group(self.device)

        self.ipmc_member = self.add_ipmc_member(
            self.device, ipmc_group_handle=self.ipmc_grp3, output_handle=self.rif3)
        self.ipmc_member = self.add_ipmc_member(
            self.device, ipmc_group_handle=self.ipmc_grp3, output_handle=self.rif6)
        self.ipmc_member = self.add_ipmc_member(
            self.device, ipmc_group_handle=self.ipmc_grp3, output_handle=self.rif7)

        # configure IPMC forwarding paths and rpf groups - in case of SM PIM mode there may be only one ingress rif
        # (*, G) route to group 1
        self.rpf_grp1 = self.add_rpf_group(self.device, pim_mode=SWITCH_IPMC_ROUTE_ATTR_PIM_MODE_SM)
        self.rpm_mbr = self.add_rpf_member(self.device, rpf_group_handle=self.rpf_grp1, handle=self.rif0)
        self.ipmc_route = self.add_ipmc_route(
            self.device, grp_ip=self.grp_ip1, vrf_handle=self.default_vrf, group_handle=self.ipmc_grp1,
            pim_mode=SWITCH_IPMC_ROUTE_ATTR_PIM_MODE_SM, rpf_group_handle=self.rpf_grp1)
        # (*, G) route to group 2
        self.ipmc_route = self.add_ipmc_route(
            self.device, grp_ip=self.grp_ip2, vrf_handle=self.default_vrf, group_handle=self.ipmc_grp2,
            pim_mode=SWITCH_IPMC_ROUTE_ATTR_PIM_MODE_SM, rpf_group_handle=self.rpf_grp1)
        # (S, G) route to group 3
        self.rpf_grp2 = self.add_rpf_group(self.device, pim_mode=SWITCH_IPMC_ROUTE_ATTR_PIM_MODE_SM)
        self.rpm_mbr = self.add_rpf_member(self.device, rpf_group_handle=self.rpf_grp2, handle=self.rif6)
        self.ipmc_route = self.add_ipmc_route(
            self.device, grp_ip=self.grp_ip1, src_ip=self.src_ip2, vrf_handle=self.default_vrf,
            group_handle=self.ipmc_grp3, pim_mode=SWITCH_IPMC_ROUTE_ATTR_PIM_MODE_SM, rpf_group_handle=self.rpf_grp2)

    def runTest(self):
        if (self.is_test_disabled()):
            print("Multicast feature not enabled, skipping")
            return

        self.IPMCXGTest()
        self.IPMCXGLagTest()
        self.IPMCSGTest()
        self.IPv6McastAdminStateTest()

    def tearDown(self):
        self.cleanup()

    def IPMCXGTest(self):
        '''
        Exercice ipv6_multicast_route_star_g
        Packet is sent with a dest IP address set for groups:
        Group 1 pointed by (*, G) route and Group 3 pointed by (S, G) route -
        should be received only on ports in Group 1
        '''
        print("\nIPMCXGTest()")

        pkt = simple_udpv6_packet(
            eth_dst=self.dst_mac,
            eth_src=self.src_mac,
            ipv6_src=self.src_ip1,
            ipv6_dst=self.grp_ip1,
            ipv6_hlim=64)
        exp_pkt = simple_udpv6_packet(
            eth_dst=self.dst_mac,
            eth_src=self.rmac,
            ipv6_src=self.src_ip1,
            ipv6_dst=self.grp_ip1,
            ipv6_hlim=63)

        print("Sending packet to multicast group 1 \n"
              "IPv6 multicast hit, RPF pass, action route")
        send_packet(self, self.devports[0], pkt)
        verify_packets(self, exp_pkt, [self.devports[1], self.devports[2], self.devports[3]])
        print("\tOK")

        print("Sending packet to multicast group 1 \n"
              "IPv6 multicast hit, RPF fail, action drop")
        send_packet(self, self.devports[1], pkt)
        verify_no_other_packets(self)
        print("\tOK")

    def IPMCXGLagTest(self):
        '''
        Exercice ipv6_multicast_route_star_g with a group containing LAG
        '''
        print("\nIPMCXGLagTest()")

        pkt = simple_udpv6_packet(
            eth_dst=self.dst_mac,
            eth_src=self.src_mac,
            ipv6_src=self.src_ip1,
            ipv6_dst=self.grp_ip2,
            ipv6_hlim=64)
        exp_pkt = simple_udpv6_packet(
            eth_dst=self.dst_mac,
            eth_src=self.rmac,
            ipv6_src=self.src_ip1,
            ipv6_dst=self.grp_ip2,
            ipv6_hlim=63)

        print("Sending packet to multicast group 2 \n"
              "IPv6 multicast hit, RPF pass, action route")
        send_packet(self, self.devports[0], pkt)
        verify_each_packet_on_multiple_port_lists(
            self, [exp_pkt, exp_pkt], [[self.devports[2]], [self.devports[4], self.devports[5]]])
        print("\tOK")

        print("Sending packet to multicast group 2 \n"
              "IPv6 multicast hit, RPF fail, action drop")
        send_packet(self, self.devports[4], pkt)
        verify_no_other_packets(self)
        print("\tOK")

    def IPMCSGTest(self):
        '''
        Exercice ipv6_multicast_route_s_g
        Packet is sent with a dest IP address set for groups:
        Group 1 pointed by (*, G) route and Group 3 pointed by (S, G) route -
        should be received only on ports in Group 3
        '''
        print("\nIPMCSGTest()")

        pkt = simple_udpv6_packet(
            eth_dst=self.dst_mac,
            eth_src=self.src_mac,
            ipv6_src=self.src_ip2,
            ipv6_dst=self.grp_ip1,
            ipv6_hlim=64)
        exp_pkt = simple_udpv6_packet(
            eth_dst=self.dst_mac,
            eth_src=self.rmac,
            ipv6_src=self.src_ip2,
            ipv6_dst=self.grp_ip1,
            ipv6_hlim=63)

        print("Sending packet to multicast group 3 \n"
              "IPv6 multicast hit, RPF pass, action route")
        send_packet(self, self.devports[6], pkt)
        verify_packets(self, exp_pkt, [self.devports[3], self.devports[7]])
        print("\tOK")

        print("Sending packet to multicast group 3 \n"
              "IPv6 multicast hit, RPF fail, action drop")
        send_packet(self, self.devports[7], pkt)
        verify_no_other_packets(self)
        print("\tOK")

    def IPv6McastAdminStateTest(self):
        '''
        Verify multicast is disabled when IPv6 admin state on an ingress RIF is False
        '''
        print("\nIPv6McastAdminStateTest()")

        pkt = simple_udpv6_packet(
            eth_dst=self.dst_mac,
            eth_src=self.src_mac,
            ipv6_src=self.src_ip1,
            ipv6_dst=self.grp_ip1,
            ipv6_hlim=64)
        exp_pkt = simple_udpv6_packet(
            eth_dst=self.dst_mac,
            eth_src=self.rmac,
            ipv6_src=self.src_ip1,
            ipv6_dst=self.grp_ip1,
            ipv6_hlim=63)

        print("Ingress RIF multicast disabled")
        self.attribute_set(self.rif0, SWITCH_RIF_ATTR_IPV6_MULTICAST, False)
        print("Sending packet to multicast group 1 - drop")
        send_packet(self, self.devports[0], pkt)
        verify_no_other_packets(self)
        print("\tOK")

        print("Ingress RIF multicast enabled")
        self.attribute_set(self.rif0, SWITCH_RIF_ATTR_IPV6_MULTICAST, True)
        print("Sending packet to multicast group 1 - route")
        send_packet(self, self.devports[0], pkt)
        verify_packets(self, exp_pkt, [self.devports[1], self.devports[2], self.devports[3]])
        print("\tOK")


@group('mcast')
class L2L3McastCoExistenceTest(ApiHelper):
    '''
    Tests for co-existing L2 and L3 Multicast entries
    Topology:
        Port      Vlan            Rif   L2MC        IPMC
        port0  A  10                     1
        port1  T  10, 20, 30             1, 2, 3
        port2  A      20                 2
        port3                    rif3                1
        port4                    rif4                1, 2
        port5                    rif5                2
        port6                    rif6                2
        port7  T  10, 20, 30             1, 2
        port8  A          30             3
    '''

    def is_test_disabled(self):
        if (self.client.is_feature_enable(SWITCH_FEATURE_MULTICAST) == 0):
            return True
        return False

    def setUp(self):
        self.configure()

        if (self.is_test_disabled()):
            print("Multicast feature not enabled, skipping")
            return

        # vlan10 -> if0, if1, if7
        self.vlan_mbr100 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port0)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.vlan_mbr101 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port1, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
        self.vlan_mbr107 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port7, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)

        # vlan20 -> if1, if2, if7
        self.vlan_mbr201 = self.add_vlan_member(self.device, vlan_handle=self.vlan20, member_handle=self.port1, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
        self.vlan_mbr202 = self.add_vlan_member(self.device, vlan_handle=self.vlan20, member_handle=self.port2)
        self.attribute_set(self.port2, SWITCH_PORT_ATTR_PORT_VLAN_ID, 20)
        self.vlan_mbr207 = self.add_vlan_member(self.device, vlan_handle=self.vlan20, member_handle=self.port7, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)

        # vlan30 -> if1, if7, if8
        self.vlan_mbr301 = self.add_vlan_member(self.device, vlan_handle=self.vlan30, member_handle=self.port1, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
        self.vlan_mbr307 = self.add_vlan_member(self.device, vlan_handle=self.vlan30, member_handle=self.port7, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
        self.vlan_mbr308 = self.add_vlan_member(self.device, vlan_handle=self.vlan30, member_handle=self.port8)
        self.attribute_set(self.port8, SWITCH_PORT_ATTR_PORT_VLAN_ID, 30)

        # L3 interfaces - if3, if4, if5, if6
        self.rif3 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port3, vrf_handle=self.vrf10, src_mac=self.rmac, ipv4_multicast=True)
        self.rif4 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port4, vrf_handle=self.vrf10, src_mac=self.rmac, ipv4_multicast=True)
        self.rif5 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port5, vrf_handle=self.vrf10, src_mac=self.rmac, ipv4_multicast=True)
        self.rif6 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port6, vrf_handle=self.vrf10, src_mac=self.rmac, ipv4_multicast=True)

        # L2MC groups
        self.l2mc_grp1 = self.add_l2mc_group(self.device)
        self.l2mc_member10 = self.add_l2mc_member(self.device, output_handle=self.port0, l2mc_group_handle=self.l2mc_grp1)
        self.l2mc_member11 = self.add_l2mc_member(self.device, output_handle=self.port1, l2mc_group_handle=self.l2mc_grp1)
        self.l2mc_member17 = self.add_l2mc_member(self.device, output_handle=self.port7, l2mc_group_handle=self.l2mc_grp1)

        self.l2mc_grp2 = self.add_l2mc_group(self.device)
        self.l2mc_member21 = self.add_l2mc_member(self.device, output_handle=self.port1, l2mc_group_handle=self.l2mc_grp2)
        self.l2mc_member27 = self.add_l2mc_member(self.device, output_handle=self.port7, l2mc_group_handle=self.l2mc_grp2)

        self.l2mc_grp3 = self.add_l2mc_group(self.device)
        self.l2mc_member31 = self.add_l2mc_member(self.device, output_handle=self.port1, l2mc_group_handle=self.l2mc_grp3)
        self.l2mc_member38 = self.add_l2mc_member(self.device, output_handle=self.port8, l2mc_group_handle=self.l2mc_grp3)

        # IPMC
        self.ipmc_grp1 = self.add_ipmc_group(self.device)
        self.ipmc_member13 = self.add_ipmc_member(self.device, output_handle=self.rif3, ipmc_group_handle=self.ipmc_grp1)
        self.ipmc_member14 = self.add_ipmc_member(self.device, output_handle=self.rif4, ipmc_group_handle=self.ipmc_grp1)

        self.ipmc_grp2 = self.add_ipmc_group(self.device)
        self.ipmc_member24= self.add_ipmc_member(self.device, output_handle=self.rif4, ipmc_group_handle=self.ipmc_grp2)
        self.ipmc_member25 = self.add_ipmc_member(self.device, output_handle=self.rif5, ipmc_group_handle=self.ipmc_grp2)
        self.ipmc_member26 = self.add_ipmc_member(self.device, output_handle=self.rif6, ipmc_group_handle=self.ipmc_grp2)

        # Multicast paths
        # L2MC bridges
        # ipv4_multicast_bridge_s_g
        self.l2mc_bridge1 = self.add_l2mc_bridge(self.device, grp_ip='230.1.1.5', src_ip='10.0.10.5', vlan_handle=self.vlan10, group_handle=self.l2mc_grp1)

        # ipv4_multicast_bridge_star_g
        self.l2mc_bridge2 = self.add_l2mc_bridge(self.device, grp_ip='230.1.1.10', src_ip='10.0.10.5', vlan_handle=self.vlan20, group_handle=self.l2mc_grp2)

        # IPMC routes
        # ipv4_multicast_route_s_g
        self.rpf1 = self.add_rpf_group(self.device, pim_mode=SWITCH_IPMC_ROUTE_ATTR_PIM_MODE_SM)
        self.rpf2_member = self.add_rpf_member(self.device, rpf_group_handle=self.rpf1, handle=self.rif3)
        self.ipmc_route1 = self.add_ipmc_route(self.device, vrf_handle=self.vrf10, rpf_group_handle=self.rpf1,
                grp_ip='230.1.1.5', src_ip='10.0.10.5', group_handle=self.ipmc_grp1)

        # ipv4_multicast_route_star_g
        self.rpf2 = self.add_rpf_group(self.device, pim_mode=SWITCH_IPMC_ROUTE_ATTR_PIM_MODE_SM)
        self.rpf2_member = self.add_rpf_member(self.device, rpf_group_handle=self.rpf2, handle=self.rif5)
        self.ipmc_route2 = self.add_ipmc_route(self.device, vrf_handle=self.vrf10, rpf_group_handle=self.rpf2,
                grp_ip='230.1.1.10', group_handle=self.ipmc_grp2)

    def runTest(self):
        if (self.is_test_disabled()):
            print("Multicast feature not enabled, skipping")
            return

        self.McastCoExistenceTest()

    def tearDown(self):
        for port in [self.port0, self.port2, self.port8]:
            self.attribute_set(port, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)

        self.cleanup()

    def McastCoExistenceTest(self):
        '''
        Exercise forwarding paths when both L2MC bridges and IPMC routes are configured
        '''
        print("\nMcastCoExistenceTest()")

        pkt = simple_udp_packet(
            eth_dst='01:00:5e:01:01:05',
            eth_src='00:22:22:22:22:22',
            ip_src='10.0.10.5',
            ip_dst='230.1.1.5',
            ip_ttl=64,
            pktlen=100)
        pkt_routed = simple_udp_packet(
            eth_dst='01:00:5e:01:01:05',
            eth_src=self.rmac,
            ip_src='10.0.10.5',
            ip_dst='230.1.1.5',
            ip_ttl=63,
            pktlen=100)
        pkt_bridged = simple_udp_packet(
            eth_dst='01:00:5e:01:01:05',
            eth_src='00:22:22:22:22:22',
            ip_src='10.0.10.5',
            ip_dst='230.1.1.5',
            dl_vlan_enable=True,
            vlan_vid=10,
            ip_ttl=64,
            pktlen=104)
        pkt_flood_30 = simple_udp_packet(
            eth_dst='01:00:5e:01:01:05',
            eth_src='00:22:22:22:22:22',
            ip_src='10.0.10.5',
            ip_dst='230.1.1.5',
            dl_vlan_enable=True,
            vlan_vid=30,
            ip_ttl=64,
            pktlen=104)

        print("(S, G) IPv4 multicast route hit, RPF pass")
        send_packet(self, self.devports[3], pkt)
        verify_packets(self, pkt_routed, [self.devports[4]])
        print("\tOK")

        print("(S, G) IPv4 multicast bridge hit")
        send_packet(self, self.devports[0], pkt)
        verify_packets(self, pkt_bridged, [self.devports[1], self.devports[7]])
        print("\tOK")

        print("IPv4 multicast miss - flood within VLAN 30")
        send_packet(self, self.devports[8], pkt)
        verify_packets(self, pkt_flood_30, [self.devports[1], self.devports[7]])
        print("\tOK")

        pkt = simple_udp_packet(
            eth_dst='01:00:5e:01:01:05',
            eth_src='00:22:22:22:22:22',
            ip_src='10.0.10.5',
            ip_dst='230.1.1.10',
            ip_ttl=64,
            pktlen=100)
        pkt_routed = simple_udp_packet(
            eth_dst='01:00:5e:01:01:05',
            eth_src=self.rmac,
            ip_src='10.0.10.5',
            ip_dst='230.1.1.10',
            ip_ttl=63,
            pktlen=100)
        pkt_bridged = simple_udp_packet(
            eth_dst='01:00:5e:01:01:05',
            eth_src='00:22:22:22:22:22',
            ip_src='10.0.10.5',
            ip_dst='230.1.1.10',
            dl_vlan_enable=True,
            vlan_vid=20,
            ip_ttl=64,
            pktlen=104)
        pkt_flood_30 = simple_udp_packet(
            eth_dst='01:00:5e:01:01:05',
            eth_src='00:22:22:22:22:22',
            ip_src='10.0.10.5',
            ip_dst='230.1.1.10',
            dl_vlan_enable=True,
            vlan_vid=30,
            ip_ttl=64,
            pktlen=104)

        print("(*, G) IPv4 multicast route hit, RPF pass")
        send_packet(self, self.devports[5], pkt)
        verify_packets(self, pkt_routed, [self.devports[4], self.devports[6]])
        print("\tOK")

        print("(*, G) IPv4 multicast bridge hit")
        send_packet(self, self.devports[2], pkt)
        verify_packets(self, pkt_bridged, [self.devports[1], self.devports[7]])
        print("\tOK")

        print("IPv4 multicast miss - flood within VLAN 30")
        send_packet(self, self.devports[8], pkt)
        verify_packets(self, pkt_flood_30, [self.devports[1], self.devports[7]])
        print("\tOK")


@group('mcast')
class IPMCSVIMembersTest(ApiHelper):
    '''
    Test that verifies IPMC group with members that are RIFs of type VLAN.
    For VLAN 10 RIF IPMC member there is an L2MC group specified
    - multicast packets should be forwarded to members of this group.
    For VLAN 20 RIF IPMC member multicast packets should be forwarded to all VLAN members.
    '''

    def is_test_disabled(self):
        if (self.client.is_feature_enable(SWITCH_FEATURE_MULTICAST) == 0):
            return True
        return False

    def setUp(self):
        self.configure()

        if (self.is_test_disabled()):
            print("Multicast feature not enabled, skipping")
            return

        self.dst_mac = "01:00:5e:01:01:05"
        self.src_mac = "00:22:22:22:22:22"
        self.grp_ip = "230.1.1.5"
        self.src_ip = "10.0.10.5"

        # configure VLANs
        self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port0)
        self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port1)
        self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port2)
        self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port3)
        self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port4)

        for port in [self.port0, self.port1, self.port2, self.port3, self.port4]:
            self.attribute_set(port, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)

        self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_LEARNING, False)
        self.vlan10_rif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_VLAN,
            vlan_handle=self.vlan10, vrf_handle=self.default_vrf, src_mac=self.rmac, ipv4_multicast=True)

        self.add_vlan_member(self.device, vlan_handle=self.vlan20, member_handle=self.port5)
        self.add_vlan_member(self.device, vlan_handle=self.vlan20, member_handle=self.port6)
        self.add_vlan_member(self.device, vlan_handle=self.vlan20, member_handle=self.port7)

        for port in [self.port5, self.port6, self.port7]:
            self.attribute_set(port, SWITCH_PORT_ATTR_PORT_VLAN_ID, 20)

        self.attribute_set(self.vlan20, SWITCH_VLAN_ATTR_LEARNING, False)
        self.vlan20_rif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_VLAN,
            vlan_handle=self.vlan20, vrf_handle=self.default_vrf, src_mac=self.rmac, ipv4_multicast=True)

        # configure L2MC with VLAN 10 members
        self.l2mc_grp = self.add_l2mc_group(self.device)

        self.l2mc_mbr = self.add_l2mc_member(
            self.device, l2mc_group_handle=self.l2mc_grp, output_handle=self.port0)
        self.l2mc_mbr = self.add_l2mc_member(
            self.device, l2mc_group_handle=self.l2mc_grp, output_handle=self.port1)
        self.l2mc_mbr = self.add_l2mc_member(
            self.device, l2mc_group_handle=self.l2mc_grp, output_handle=self.port2)

        # configure RIFs
        self.rif8 = self.add_rif(
            self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port8,
            vrf_handle=self.default_vrf, src_mac=self.rmac, ipv4_multicast=True)
        self.rif9 = self.add_rif(
            self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port9,
            vrf_handle=self.default_vrf, src_mac=self.rmac, ipv4_multicast=True)

        # configure IPMC group
        self.ipmc_grp = self.add_ipmc_group(self.device)

        self.ipmc_member = self.add_ipmc_member(
            self.device, ipmc_group_handle=self.ipmc_grp, output_handle=self.rif8)
        self.ipmc_member = self.add_ipmc_member(
            self.device, ipmc_group_handle=self.ipmc_grp, output_handle=self.rif9)
        self.ipmc_member = self.add_ipmc_member(
            self.device, ipmc_group_handle=self.ipmc_grp, output_handle=self.vlan10_rif, l2mc_group_handle=self.l2mc_grp)
        self.ipmc_member = self.add_ipmc_member(
            self.device, ipmc_group_handle=self.ipmc_grp, output_handle=self.vlan20_rif)

        self.rpf_grp = self.add_rpf_group(
            self.device, pim_mode=SWITCH_IPMC_ROUTE_ATTR_PIM_MODE_SM)
        self.rpm_mbr = self.add_rpf_member(
            self.device, rpf_group_handle=self.rpf_grp, handle=self.rif8)
        self.ipmc_route = self.add_ipmc_route(
            self.device, grp_ip=self.grp_ip, vrf_handle=self.default_vrf, group_handle=self.ipmc_grp,
            pim_mode=SWITCH_IPMC_ROUTE_ATTR_PIM_MODE_SM, rpf_group_handle=self.rpf_grp)

    def runTest(self):
        if (self.is_test_disabled()):
            print("Multicast feature not enabled, skipping")
            return

        self.SVIMcastMembersTest()

    def tearDown(self):
        for port in [self.port0, self.port1, self.port2, self.port3, self.port4, self.port5, self.port6, self.port7]:
            self.attribute_set(port, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)

        self.cleanup()

    def SVIMcastMembersTest(self):
        '''
        Exercice multicast group with members that are RIFs of type VLAN
        '''
        print("\nSVIMcastMembersTest")

        pkt = simple_udp_packet(
            eth_dst=self.dst_mac,
            eth_src=self.src_mac,
            ip_src=self.src_ip,
            ip_dst=self.grp_ip,
            ip_ttl=64)
        exp_pkt = simple_udp_packet(
            eth_dst=self.dst_mac,
            eth_src=self.rmac,
            ip_src=self.src_ip,
            ip_dst=self.grp_ip,
            ip_ttl=63)

        print("Sending packet to multicast group 1")
        send_packet(self, self.devports[8], pkt)
        verify_packets(
            self, exp_pkt,
            [self.devports[0], self.devports[1], self.devports[2], self.devports[5],
             self.devports[6], self.devports[7], self.devports[9]])
        verify_no_other_packets(self)
        print("\tOK")


@group('mcast')
@group('vrf-action')
class L3VrfUnknownL3MulticastAction(ApiHelper):
    def is_test_disabled(self):
        if (self.client.is_feature_enable(SWITCH_FEATURE_MULTICAST) == 0):
            return True
        return False

    def setUp(self):
        print()
        self.configure()
        if self.is_test_disabled():
            print("\nL3 Multicast feature not enabled, skipping")
            return
        self.vrf11 = self.add_vrf(self.device, id=11, src_mac=self.rmac)
        self.rif0 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT,
            port_handle=self.port0, vrf_handle=self.vrf11, src_mac=self.rmac, ipv4_multicast=True)
        self.rif1 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT,
            port_handle=self.port1, vrf_handle=self.vrf11, src_mac=self.rmac, ipv4_multicast=True)
        self.mc_grp = self.add_ipmc_group(self.device)
        self.mc_member = self.add_ipmc_member(
            self.device, ipmc_group_handle=self.mc_grp, output_handle=self.rif1)
        self.rpf_grp = self.add_rpf_group(
            self.device, pim_mode=SWITCH_IPMC_ROUTE_ATTR_PIM_MODE_SM)
        self.rpf_member = self.add_rpf_member(self.device,
            rpf_group_handle=self.rpf_grp,
            handle=self.rif0)
        self.route = self.add_ipmc_route(self.device,
            vrf_handle=self.vrf11,
            rpf_group_handle=self.rpf_grp,
            src_ip='10.0.10.5',
            grp_ip='230.1.1.5',
            group_handle=self.mc_grp)

    def runTest(self):
        if self.is_test_disabled():
            return

        self.VrfUnknownL3McastActionTest()

    def tearDown(self):
        self.cleanup()

    def VrfUnknownL3McastActionTest(self):
        ingress_port = self.devports[0]
        route_egress_port = self.devports[1]

        pkt_unknown = simple_udp_packet(
            eth_src='00:22:22:22:22:22',
            eth_dst='01:00:5E:01:01:05',
            ip_src='10.0.10.6', # NOTE ip src unknown
            ip_dst='230.1.1.5',
            ip_ttl=64)
        cpu_pkt_unknown = cpu_packet_mask_ingress_bd(
            simple_cpu_packet(
                packet_type=0,
                ingress_port=swport_to_devport(self, ingress_port),
                ingress_ifindex=self.get_port_ifindex(self.port0),
                ingress_bd=0x00,
                reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_MAX+SWITCH_HOSTIF_TRAP_ATTR_INTERNAL_TRAP_UNKNOWN_L3_MULTICAST,
                inner_pkt=pkt_unknown))
        pkt_known = simple_udp_packet(
            eth_src='00:22:22:22:22:22',
            eth_dst='01:00:5E:01:01:05',
            ip_ttl=64,
            ip_src='10.0.10.5',
            ip_dst='230.1.1.5')
        pkt_known_routed = simple_udp_packet(
            eth_src=self.rmac,
            eth_dst='01:00:5E:01:01:05',
            ip_ttl=63,
            ip_src='10.0.10.5',
            ip_dst='230.1.1.5')

        def set_actions(action):
            self.attribute_set(self.vrf11,
                SWITCH_VRF_ATTR_UNKNOWN_L3_MCAST_ACTION,
                action)

        print("Verify VRF unknown L3 mcast action TRAP")
        set_actions(SWITCH_VRF_ATTR_UNKNOWN_L3_MCAST_ACTION_TRAP)
        send_packet(self, ingress_port, pkt_unknown)
        verify_packet(self, cpu_pkt_unknown, self.cpu_port)
        print("Unknown packet routed to CPU")
        send_packet(self, ingress_port, pkt_known)
        verify_packet(self, pkt_known_routed, route_egress_port)
        print("Known packet routed to egress port")

        print("Verify VRF unknown L3 mcast action DROP")
        set_actions(SWITCH_VRF_ATTR_UNKNOWN_L3_MCAST_ACTION_DROP)
        send_packet(self, ingress_port, pkt_unknown)
        verify_no_other_packets(self)
        print("Unknown packet dropped")
        send_packet(self, ingress_port, pkt_known)
        verify_packet(self, pkt_known_routed, route_egress_port)
        print("Known packet routed to egress port")


@group('ipv6')
@group('mcast')
@group('vrf-action')
class L3VrfUnknownL3MulticastActionIPv6(ApiHelper):
    def is_test_disabled(self):
        if (self.client.is_feature_enable(SWITCH_FEATURE_MULTICAST) == 0):
            return True
        return False

    def setUp(self):
        print()
        self.configure()
        if self.is_test_disabled():
            print("L3 Multicast feature not enabled, skipping")
            return
        self.vrf11 = self.add_vrf(self.device, id=11, src_mac=self.rmac)
        self.rif0 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT,
            port_handle=self.port0, vrf_handle=self.vrf11, src_mac=self.rmac, ipv6_multicast=True)
        self.rif1 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT,
            port_handle=self.port1, vrf_handle=self.vrf11, src_mac=self.rmac, ipv6_multicast=True)
        self.mc_grp = self.add_ipmc_group(self.device)
        self.mc_member = self.add_ipmc_member(
            self.device, ipmc_group_handle=self.mc_grp, output_handle=self.rif1)
        self.rpf_grp = self.add_rpf_group(
            self.device, pim_mode=SWITCH_IPMC_ROUTE_ATTR_PIM_MODE_SM)
        self.rpf_member = self.add_rpf_member(self.device,
            rpf_group_handle=self.rpf_grp,
            handle=self.rif0)
        self.route = self.add_ipmc_route(self.device,
            vrf_handle=self.vrf11,
            rpf_group_handle=self.rpf_grp,
            src_ip='2::a00:a05',
            grp_ip='ff00::e601:0105',
            group_handle=self.mc_grp)

    def runTest(self):
        if self.is_test_disabled():
            return
        self.VrfUnknownL3McastActionTestIPv6()

    def tearDown(self):
        self.cleanup()

    def VrfUnknownL3McastActionTestIPv6(self):
        ingress_port = self.devports[0]
        route_egress_port = self.devports[1]

        pkt_unknown = simple_udpv6_packet(
            eth_src='00:22:22:22:22:22',
            eth_dst='01:00:5E:01:01:05',
            ipv6_src='2::a00:a06', # NOTE src ip is unknown
            ipv6_dst='ff00::e601:0105')
        cpu_pkt_unknown = cpu_packet_mask_ingress_bd(
            simple_cpu_packet(
                packet_type=0,
                ingress_bd=0x00,
                ingress_port=swport_to_devport(self, self.devports[0]),
                ingress_ifindex=(self.port0 & 0xFFFF),
                reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_MAX+SWITCH_HOSTIF_TRAP_ATTR_INTERNAL_TRAP_UNKNOWN_L3_MULTICAST,
                inner_pkt=pkt_unknown))
        pkt_known = simple_udpv6_packet(
            eth_src='22:22:22:22:22:22',
            eth_dst='33:33:E6:01:01:05',
            ipv6_hlim=64,
            ipv6_src='2::a00:a05',
            ipv6_dst='ff00::e601:0105')
        pkt_known_routed = simple_udpv6_packet(
            eth_src=self.rmac,
            eth_dst='33:33:E6:01:01:05',
            ipv6_hlim=63,
            ipv6_src='2::a00:a05',
            ipv6_dst='ff00::e601:0105')

        def set_actions(action):
            self.attribute_set(self.vrf11,
                SWITCH_VRF_ATTR_UNKNOWN_L3_MCAST_ACTION,
                action)

        print("verify VRF unknown L3 mcast action TRAP")
        set_actions(SWITCH_VRF_ATTR_UNKNOWN_L3_MCAST_ACTION_TRAP)
        send_packet(self, ingress_port, pkt_unknown)
        verify_packet(self, cpu_pkt_unknown, self.cpu_port)
        print("Unknown packet routed to CPU")
        send_packet(self, ingress_port, pkt_known)
        verify_packet(self, pkt_known_routed, route_egress_port)
        print("Known packet routed to egress port")

        print("Verify VRF unknown L3 mcast action DROP")
        set_actions(SWITCH_VRF_ATTR_UNKNOWN_L3_MCAST_ACTION_DROP)
        send_packet(self, ingress_port, pkt_unknown)
        verify_no_other_packets(self)
        print("Unknown packet dropped")
        send_packet(self, ingress_port, pkt_known)
        verify_packet(self, pkt_known_routed, route_egress_port)
        print("Known packet routed to egress port")


@group('mcast')
@group('vrf-action')
class L3VrfUnknownL3MulticastActionL2NotAffected(ApiHelper):
    def is_test_disabled(self):
        if (self.client.is_feature_enable(SWITCH_FEATURE_MULTICAST) == 0):
            return True
        return False

    def setUp(self):
        print()
        self.configure()
        if self.is_test_disabled():
            print("L3 Multicast feature not enabled, skipping")
            return
        self.vlan = self.add_vlan(self.device,
            vlan_id=1996,
            ipv4_multicast=True)
        self.vrf = self.add_vrf(self.device, id=11, src_mac=self.rmac)
        self.svi = self.add_rif(self.device,
            type=SWITCH_RIF_ATTR_TYPE_VLAN,
            vrf_handle=self.vrf,
            vlan_handle=self.vlan,
            src_mac=self.rmac,
            ipv4_multicast=True)
        for p in [self.port0, self.port1, self.port2, self.port3]:
            self.add_vlan_member(self.device,
                vlan_handle=self.vlan,
                member_handle=p,
                tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_UNTAGGED)
            self.attribute_set(p, SWITCH_PORT_ATTR_PORT_VLAN_ID, 1996)
        self.mc_grp = self.add_l2mc_group(self.device)
        self.mc_member0 = self.add_l2mc_member(
            self.device, output_handle=self.port0, l2mc_group_handle=self.mc_grp)
        self.mc_member0 = self.add_l2mc_member(
            self.device, output_handle=self.port1, l2mc_group_handle=self.mc_grp)
        self.mc_member1 = self.add_l2mc_member(
            self.device, output_handle=self.port2, l2mc_group_handle=self.mc_grp)
        self.route = self.add_l2mc_bridge(
            self.device, grp_ip='230.1.1.5', src_ip='10.0.10.5', vlan_handle=self.vlan, group_handle=self.mc_grp)

    def runTest(self):
        if self.is_test_disabled():
            return

        self.VrfUnknownL3McastActionL2NotAffectedTest()

    def tearDown(self):
        for p in [self.port0, self.port1, self.port2, self.port3]:
            self.attribute_set(p, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.cleanup()

    def VrfUnknownL3McastActionL2NotAffectedTest(self):
        pkt_unknown_flooded = simple_udp_packet(
            eth_src='00:22:22:22:22:22',
            eth_dst='01:00:5E:01:01:05',
            ip_ttl=64,
            ip_src='10.0.10.6', # NOTE ip src unknown
            ip_dst='230.1.1.5')
        pkt_known = simple_udp_packet(
            eth_src='00:22:22:22:22:22',
            eth_dst='01:00:5E:01:01:05',
            ip_ttl=64,
            ip_src='10.0.10.5',
            ip_dst='230.1.1.5')
        pkt_known_mcasted = simple_udp_packet(
            eth_src='00:22:22:22:22:22',
            eth_dst='01:00:5E:01:01:05',
            ip_ttl=64,
            ip_src='10.0.10.5',
            ip_dst='230.1.1.5')

        def set_actions(action):
            self.attribute_set(self.vrf,
                SWITCH_VRF_ATTR_UNKNOWN_L3_MCAST_ACTION,
                action)

        print("Verify VRF unknown L3 mcast action DROP does not affect L2 traffic")
        set_actions(SWITCH_VRF_ATTR_UNKNOWN_L3_MCAST_ACTION_DROP)
        send_packet(self, self.devports[0], pkt_known)
        verify_packets(self, pkt_known_mcasted, [self.devports[1], self.devports[2]])
        print("Known packet multicasted")
        send_packet(self, self.devports[0], pkt_unknown_flooded)
        verify_packets(self, pkt_unknown_flooded, [self.devports[1], self.devports[2], self.devports[3]])
        print("Unknown packet flooded")

        # IGMP snooping does not affect L2MC as bridging goes first
        print("Setting IGMP snooping on VLAN")
        self.attribute_set(self.vlan, SWITCH_VLAN_ATTR_IGMP_SNOOPING, True)
        send_packet(self, self.devports[0], pkt_known)
        verify_packets(self, pkt_known_mcasted, [self.devports[1], self.devports[2]])
        print("Known packet multicasted")
        send_packet(self, self.devports[0], pkt_unknown_flooded)
        verify_packets(self, pkt_unknown_flooded, [self.devports[1], self.devports[2], self.devports[3]])
        print("Unknown packet flooded")


@group('ipv6')
@group('mcast')
@group('vrf-action')
class L3VrfUnknownL3MulticastActionL2NotAffectedIPv6(ApiHelper):
    def is_test_disabled(self):
        if (self.client.is_feature_enable(SWITCH_FEATURE_MULTICAST) == 0):
            return True
        return False

    def setUp(self):
        print()
        self.configure()
        if self.is_test_disabled():
            print("L3 Multicast feature not enabled, skipping")
            return
        self.vlan = self.add_vlan(self.device,
            vlan_id=1996,
            ipv6_multicast=True)
        self.vrf = self.add_vrf(self.device, id=11, src_mac=self.rmac)
        self.svi = self.add_rif(self.device,
            type=SWITCH_RIF_ATTR_TYPE_VLAN,
            vrf_handle=self.vrf,
            vlan_handle=self.vlan,
            src_mac=self.rmac,
            ipv6_multicast=True)
        for p in [self.port0, self.port1, self.port2, self.port3]:
            self.add_vlan_member(self.device,
                vlan_handle=self.vlan,
                member_handle=p,
                tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_UNTAGGED)
            self.attribute_set(p, SWITCH_PORT_ATTR_PORT_VLAN_ID, 1996)
        self.mc_grp = self.add_l2mc_group(self.device)
        self.mc_member0 = self.add_l2mc_member(
            self.device, output_handle=self.port0, l2mc_group_handle=self.mc_grp)
        self.mc_member0 = self.add_l2mc_member(
            self.device, output_handle=self.port1, l2mc_group_handle=self.mc_grp)
        self.mc_member1 = self.add_l2mc_member(
            self.device, output_handle=self.port2, l2mc_group_handle=self.mc_grp)
        self.route = self.add_l2mc_bridge(
            self.device, grp_ip='ff00::e601:0105', src_ip='2::a00:a05', vlan_handle=self.vlan, group_handle=self.mc_grp)

    def runTest(self):
        if self.is_test_disabled():
            return

        self.VrfUnknownL3McastActionL2NotAffectedIPv6Test()

    def tearDown(self):
        for p in [self.port0, self.port1, self.port2, self.port3]:
            self.attribute_set(p, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.cleanup()

    def VrfUnknownL3McastActionL2NotAffectedIPv6Test(self):
        pkt_unknown_flooded = simple_udpv6_packet(
            eth_src='00:22:22:22:22:22',
            eth_dst='01:00:5E:01:01:05',
            ipv6_src='2::a00:a06', # NOTE src ip is unknown
            ipv6_dst='ff00::e601:0105')
        pkt_known = simple_udpv6_packet(
            eth_src='22:22:22:22:22:22',
            eth_dst='33:33:E6:01:01:05',
            ipv6_hlim=64,
            ipv6_src='2::a00:a05',
            ipv6_dst='ff00::e601:0105')
        pkt_known_mcasted = simple_udpv6_packet(
            eth_src='22:22:22:22:22:22',
            eth_dst='33:33:E6:01:01:05',
            ipv6_hlim=64,
            ipv6_src='2::a00:a05',
            ipv6_dst='ff00::e601:0105')

        def set_actions(action):
            self.attribute_set(self.vrf,
                SWITCH_VRF_ATTR_UNKNOWN_L3_MCAST_ACTION,
                action)

        print("Verify VRF unknown L3 mcast action DROP does not affect L2 traffic")
        set_actions(SWITCH_VRF_ATTR_UNKNOWN_L3_MCAST_ACTION_DROP)
        send_packet(self, self.devports[0], pkt_known)
        verify_packets(self, pkt_known_mcasted, [self.devports[1], self.devports[2]])
        print("Known packet multicasted")
        send_packet(self, self.devports[0], pkt_unknown_flooded)
        verify_packets(self, pkt_unknown_flooded, [self.devports[1], self.devports[2], self.devports[3]])
        print("Unknown packet flooded")

        # IGMP snooping does not affect L2MC as bridging goes first
        print("Setting IGMP snooping on VLAN")
        self.attribute_set(self.vlan, SWITCH_VLAN_ATTR_MLD_SNOOPING, True)
        send_packet(self, self.devports[0], pkt_known)
        verify_packets(self, pkt_known_mcasted, [self.devports[1], self.devports[2]])
        print("Known packet multicasted")
        send_packet(self, self.devports[0], pkt_unknown_flooded)
        verify_packets(self, pkt_unknown_flooded, [self.devports[1], self.devports[2], self.devports[3]])
        print("Unknown packet flooded")


@group('mcast')
class IPMulticastDMACValidationTest(ApiHelper):
    def is_test_disabled(self):
        if (self.client.is_feature_enable(SWITCH_FEATURE_IPMC_DMAC_VALIDATION) == 0
            or self.client.is_feature_enable(SWITCH_FEATURE_MULTICAST) == 0):
            return True
        return False

    def setUp(self):
        print()
        self.configure()
        if self.is_test_disabled():
            print("IP Multicast DMAC validation feature is not enabled, skipping")
            return

    def runTest(self):
        if self.is_test_disabled():
            return

        ipv4mc_mismatch_dmac_pkt = simple_udp_packet(
            eth_src='22:22:22:22:22:22',
            eth_dst='33:33:A6:31:31:05',
            ip_dst='239.1.23.3')
        ipv4mc_pkt = simple_udp_packet(
            eth_src='22:22:22:22:22:22',
            eth_dst='01:00:5E:01:17:03',
            ip_dst='239.1.23.3')

        cnt_pre = self.client.object_counters_get(self.port0)
        send_packet(self, self.devports[0], ipv4mc_mismatch_dmac_pkt)
        time.sleep(5)
        cnt_post = self.client.object_counters_get(self.port0)
        self.assertEqual(
            cnt_pre[SWITCH_PORT_COUNTER_ID_IF_IN_MC_DMAC_MISMATCH].count,
            cnt_post[SWITCH_PORT_COUNTER_ID_IF_IN_MC_DMAC_MISMATCH].count - 1)

        cnt_pre = self.client.object_counters_get(self.port0)
        send_packet(self, self.devports[0], ipv4mc_pkt)
        time.sleep(5)
        cnt_post = self.client.object_counters_get(self.port0)
        self.assertEqual(
            cnt_pre[SWITCH_PORT_COUNTER_ID_IF_IN_MC_DMAC_MISMATCH].count,
            cnt_post[SWITCH_PORT_COUNTER_ID_IF_IN_MC_DMAC_MISMATCH].count)

    def tearDown(self):
        self.cleanup()


@group('mcast')
@group('ipv6')
class IPMulticastDMACValidationIPv6Test(ApiHelper):
    def is_test_disabled(self):
        if (self.client.is_feature_enable(SWITCH_FEATURE_IPMC_DMAC_VALIDATION) == 0
            or self.client.is_feature_enable(SWITCH_FEATURE_MULTICAST) == 0):
            return True
        return False

    def setUp(self):
        print()
        self.configure()
        if self.is_test_disabled():
            print("IP Multicast DMAC validation feature is not enabled, skipping")
            return

    def runTest(self):
        if self.is_test_disabled():
            return

        ipv6mc_mismatch_dmac_pkt = simple_udpv6_packet(
            eth_src='22:22:22:22:22:22',
            eth_dst='33:33:A6:31:31:05',
            ipv6_dst='ff00::e601:0105')
        ipv6mc_pkt = simple_udpv6_packet(
            eth_src='22:22:22:22:22:22',
            eth_dst='33:33:E6:01:01:05',
            ipv6_dst='ff00::e601:0105')
        cnt_pre = self.client.object_counters_get(self.port0)
        send_packet(self, self.devports[0], ipv6mc_mismatch_dmac_pkt)
        time.sleep(5)
        cnt_post = self.client.object_counters_get(self.port0)
        self.assertEqual(
            cnt_pre[SWITCH_PORT_COUNTER_ID_IF_IN_MC_DMAC_MISMATCH].count,
            cnt_post[SWITCH_PORT_COUNTER_ID_IF_IN_MC_DMAC_MISMATCH].count - 1)

        cnt_pre = self.client.object_counters_get(self.port0)
        send_packet(self, self.devports[0], ipv6mc_pkt)
        time.sleep(5)
        cnt_post = self.client.object_counters_get(self.port0)
        self.assertEqual(
            cnt_pre[SWITCH_PORT_COUNTER_ID_IF_IN_MC_DMAC_MISMATCH].count,
            cnt_post[SWITCH_PORT_COUNTER_ID_IF_IN_MC_DMAC_MISMATCH].count)

    def tearDown(self):
        self.cleanup()
