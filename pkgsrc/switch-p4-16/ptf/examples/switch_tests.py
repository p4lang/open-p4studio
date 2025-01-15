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
from ptf import config
from ptf.testutils import *
from ptf.packet import *
from ptf.thriftutils import *

import os
import ptf.mask

this_dir = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(this_dir, '..'))
sys.path.append(os.path.join(this_dir, '../api'))
from common.utils import *
from api.api_base_tests import *
from api.switch_helpers import *
import api.model_utils as u

from bf_switcht_api_thrift.ttypes import *
from bf_switcht_api_thrift.model_headers import *

###############################################################################


class TestConnection(ApiHelper):
    '''
    This simply prints the below message if connection is established with DUT
    if "--no-status-srv" is specified, the test fails with no connection
    '''
    def runTest(self):
        print('Connected to server')
        test_params = ptf.testutils.test_params_get()
        print(config["interfaces"])
        print("Connected to server: %s" % test_params["thrift_server"])


###############################################################################


class L2PortTest(ApiHelper):
    '''
    Add ports 0-3 to vlan 10 and flood uknown unicast packets
    '''
    def runTest(self):
        self.configure()

        # self.vlan10 is created in the configure call
        self.add_vlan_member(self.device,
                             vlan_handle=self.vlan10,
                             member_handle=self.port0)
        self.add_vlan_member(self.device,
                             vlan_handle=self.vlan10,
                             member_handle=self.port1)
        self.add_vlan_member(self.device,
                             vlan_handle=self.vlan10,
                             member_handle=self.port2)
        self.add_vlan_member(self.device,
                             vlan_handle=self.vlan10,
                             member_handle=self.port3)

        # set default vlan for port 0 on ingress
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)

        try:
            pkt = simple_tcp_packet(eth_dst='00:33:33:33:33:33')
            print("Sending unknown unicast packet from %d, flood on vlan 10" %
                  (self.devports[0]))
            send_packet(self, self.devports[0], str(pkt))
            verify_packets(
                self, pkt,
                [self.devports[1], self.devports[2], self.devports[3]])
        finally:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.cleanup()


###############################################################################


class L3InterfaceTest(ApiHelper):
    '''
    This is a simple L3 interface test. We will configure an L3 interface
    with an IP address and also configure a nexthop for each of the connected
    interfaces
    In addition a route is also added for myip for each interface
        Server                          DUT
    10.10.10.2 - if0 | -------- | port0 - 10.10.10.1
    11.11.11.2 - if1 | -------- | port1 - 11.11.11.1
    '''
    def runTest(self):
        self.configure()

        # create a nexthop for myip packets
        self.nhop_glean = self.add_nexthop(self.device,
                                           type=SWITCH_NEXTHOP_ATTR_TYPE_GLEAN)

        # create an RIF object on port0 and assign an IP address
        self.rif0 = self.add_rif(self.device,
                                 type=SWITCH_RIF_ATTR_TYPE_PORT,
                                 port_handle=self.port0,
                                 vrf_handle=self.vrf10,
                                 rmac_group_handle=self.rmac_group)
        self.rif0_myip_route = self.add_route(self.device,
                                              ip_prefix='10.10.10.1',
                                              vrf_handle=self.vrf10,
                                              nexthop_handle=self.nhop_glean)

        # create an RIF object on port1 and assign an IP address
        self.rif1 = self.add_rif(self.device,
                                 type=SWITCH_RIF_ATTR_TYPE_PORT,
                                 port_handle=self.port1,
                                 vrf_handle=self.vrf10,
                                 rmac_group_handle=self.rmac_group)
        self.rif1_myip_route = self.add_route(self.device,
                                              ip_prefix='11.11.11.1',
                                              vrf_handle=self.vrf10,
                                              nexthop_handle=self.nhop_glean)

        # simulate a nexthop on if0 with a host route
        self.nhop0 = self.add_nexthop(self.device,
                                      handle=self.rif0,
                                      dest_ip='10.10.10.2')
        self.neighbor0 = self.add_neighbor(self.device,
                                           mac_address='00:10:22:33:44:55',
                                           handle=self.rif0,
                                           dest_ip='10.10.10.2')
        self.add_route(self.device,
                       ip_prefix='10.10.10.2',
                       vrf_handle=self.vrf10,
                       nexthop_handle=self.nhop0)

        # simulate a nexthop on if1 with a host route
        self.nhop1 = self.add_nexthop(self.device,
                                      handle=self.rif1,
                                      dest_ip='11.11.11.2')
        self.neighbor1 = self.add_neighbor(self.device,
                                           mac_address='00:11:22:33:44:55',
                                           handle=self.rif1,
                                           dest_ip='11.11.11.2')
        self.add_route(self.device,
                       ip_prefix='11.11.11.2',
                       vrf_handle=self.vrf10,
                       nexthop_handle=self.nhop1)

        try:
            print("if1 -> eth1(DUT) - eth0(DUT) -> if0")
            pkt = simple_tcp_packet(eth_dst='00:77:66:55:44:33',
                                    eth_src='00:22:22:22:22:22',
                                    ip_dst='10.10.10.2',
                                    ip_src='11.11.11.2',
                                    ip_ttl=64)
            exp_pkt = simple_tcp_packet(eth_dst='00:10:22:33:44:55',
                                        eth_src='00:77:66:55:44:33',
                                        ip_dst='10.10.10.2',
                                        ip_src='11.11.11.2',
                                        ip_ttl=63)
            send_packet(self, self.devports[1], str(pkt))
            verify_packet(self, exp_pkt, self.devports[0])

            # the dst ip is 11.11.11.1 which is myip. The packet is redirected
            # to the local CPU with fabric header prefixed to the packet.
            print("if1 -> eth1(DUT) CPU redirect for myip")
            pkt = simple_tcp_packet(eth_dst='00:77:66:55:44:33',
                                    eth_src='00:22:22:22:22:22',
                                    ip_dst='11.11.11.1',
                                    ip_ttl=64)
            cpu_pkt = simple_cpu_packet(
                packet_type=0,
                ingress_port=swport_to_devport(self, self.devports[1]),
                ingress_ifindex=(self.port1 & 0xFFFF),
                reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_GLEAN,
                ingress_bd=0x0,
                inner_pkt=pkt)
            cpu_pkt = cpu_packet_mask_ingress_bd(cpu_pkt)
            if test_params["target"] != "hw":
                send_packet(self, self.devports[1], str(pkt))
                verify_packet(self, cpu_pkt, self.cpu_port)
        finally:
            self.cleanup()
