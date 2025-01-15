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


from bfruntime_client_base_tests import BfRuntimeTest
import bfrt_grpc.bfruntime_pb2 as bfruntime_pb2
import bfrt_grpc.client as client

this_dir = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(this_dir, '..'))
from common.utils import *
import json
import model_utils as u

def step_speed_fec_from_port_mode(port_mode):
    port_step = 1
    if port_mode == '10g':
        port_step = 1
        port_speed = 10000
        fec = SWITCH_PORT_ATTR_FEC_TYPE_NONE
    elif port_mode == '25g':
        port_step = 1
        port_speed = 25000
        fec = SWITCH_PORT_ATTR_FEC_TYPE_NONE
    elif port_mode == '50g' or port_mode == '50g-r2':
        port_step = 2
        port_speed = 50000
        fec = SWITCH_PORT_ATTR_FEC_TYPE_NONE
    elif port_mode == '50g-r1':
        port_step = 1
        port_speed = 50000
        fec = SWITCH_PORT_ATTR_FEC_TYPE_RS
    elif port_mode == '100g' or port_mode == '100g-r4':
        port_step = 1
        port_speed = 100000
        fec = SWITCH_PORT_ATTR_FEC_TYPE_RS
    elif port_mode == '100g-r2':
        port_step = 2
        port_speed = 100000
        fec = SWITCH_PORT_ATTR_FEC_TYPE_RS
    elif port_mode == '100g-r1':
        port_step = 2
        port_speed = 100000
        fec = SWITCH_PORT_ATTR_FEC_TYPE_RS
    elif port_mode == '200g' or port_mode == '200g-r4':
        port_step = 4
        port_speed = 200000
        fec = SWITCH_PORT_ATTR_FEC_TYPE_RS
    elif port_mode == '200g-r2':
        port_step = 4
        port_speed = 200000
        fec = SWITCH_PORT_ATTR_FEC_TYPE_RS
    elif port_mode == '400g':
        port_step = 8
        port_speed = 400000
        fec = SWITCH_PORT_ATTR_FEC_TYPE_RS
    elif port_mode == '400g-r4':
        port_step = 8
        port_speed = 400000
        fec = SWITCH_PORT_ATTR_FEC_TYPE_RS
    return port_step, port_speed, fec

@disabled
class DvTest(ApiHelper):
    def runTest(self):
        print()
        if test_param_get('target') != 'hw': return
        num_pipes = int(test_param_get('num_pipes'))
        port_mode = test_param_get("port_mode")

        self.device = self.get_device_handle(0)
        self.vlan = self.add_vlan(self.device, vlan_id=1027)
        self.attribute_set(self.vlan, SWITCH_VLAN_ATTR_LEARNING, False)

        port_max = 64 * num_pipes
        port_step = 1
        if port_mode == '10g':
            port_step = 1
            port_speed = 10000
            fec = SWITCH_PORT_ATTR_FEC_TYPE_NONE
        elif port_mode == '25g':
            port_step = 1
            port_speed = 25000
            fec = SWITCH_PORT_ATTR_FEC_TYPE_NONE
        elif port_mode == '50g' or port_mode == '50g-r2':
            port_step = 2
            port_speed = 50000
            fec = SWITCH_PORT_ATTR_FEC_TYPE_NONE
        elif port_mode == '50g-r1':
            port_step = 1
            port_speed = 50000
            fec = SWITCH_PORT_ATTR_FEC_TYPE_RS
        elif port_mode == '100g' or port_mode == '100g-r4':
            port_step = 4
            port_speed = 100000
            fec = SWITCH_PORT_ATTR_FEC_TYPE_RS
        elif port_mode == '100g-r2':
            port_step = 2
            port_speed = 100000
            fec = SWITCH_PORT_ATTR_FEC_TYPE_RS
        elif port_mode == '200g' or port_mode == '200g-r4':
            port_step = 4
            port_speed = 200000
            fec = SWITCH_PORT_ATTR_FEC_TYPE_RS
        elif port_mode == '400g':
            port_step = 8
            port_speed = 400000
            fec = SWITCH_PORT_ATTR_FEC_TYPE_RS
        else:
            self.assertTrue(False, "Unsupported port-mode %s" % (port_mode))

        for port in range(0, port_max, port_step):
            port_hdl = self.add_port(self.device, lane_list=u.lane_list_t([port]), speed=port_speed, fec_type=fec)
            self.add_vlan_member(self.device, vlan_handle=self.vlan, member_handle=port_hdl)

            dev_port = self.attribute_get(port_hdl, SWITCH_PORT_ATTR_DEV_PORT)
            pipe = dev_port >> 7
            local_port = dev_port & 0x7F

            dmac = '00:00:00:11:%02x:%02x' % (pipe, local_port)
            print("Switch port %3d maps to dev port %3d and mac %s" % (port, dev_port, dmac))
            self.add_mac_entry(self.device, vlan_handle=self.vlan, mac_address=dmac, destination_handle=port_hdl)

        #tx_pkt = simple_udp_packet(eth_dst='00:00:00:11:00:47', eth_src='00:00:00:11:00:38', ip_ttl=64)
        #rx_pkt = simple_udp_packet(eth_dst='00:00:00:11:00:47', eth_src='00:00:00:11:00:38', ip_ttl=64)
        #send_packet(self, 64, tx_pkt)
        #verify_packet(self, rx_pkt, 79)

###############################################################################

@disabled
class DvL3Test(ApiHelper):
    def runTest(self):
        print()
        if test_param_get('target') != 'hw': return
        if test_param_get('arch') != 'tofino2':
            return
        num_pipes = int(test_param_get('num_pipes'))
        port_mode = test_param_get("port_mode")

        self.device = self.get_device_handle(0)

        port_max = 64 * num_pipes
        port_step = 1
        port_step, port_speed, fec = step_speed_fec_from_port_mode(port_mode)

        # first port
        port_hdl = 0
        vrf_hdl = 0
        rif = 0

        port = 0
        prev_rmac = '00:11:22:33:44:%02x' % port
        prev_port_hdl = self.add_port(self.device, lane_list=u.lane_list_t([port]), speed=port_speed, fec_type=fec)
        prev_vrf_hdl = self.add_vrf(self.device, id=port, src_mac=prev_rmac)
        prev_rif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=prev_port_hdl, vrf_handle=prev_vrf_hdl, src_mac=prev_rmac)

        # rest of the ports
        for port in range(port_step, port_max, port_step):
            rmac = '00:11:22:33:44:%02x' % port

            port_hdl = self.add_port(self.device, lane_list=u.lane_list_t([port]), speed=port_speed, fec_type=fec)
            vrf_hdl = self.add_vrf(self.device, id=port, src_mac=rmac)
            rif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=port_hdl, vrf_handle=vrf_hdl, src_mac=rmac)

            nhop = self.add_nexthop(self.device, handle=rif, dest_ip='10.10.0.2')
            neighbor = self.add_neighbor(self.device, mac_address=rmac, handle=rif, dest_ip='10.10.0.2')
            route = self.add_route(self.device, ip_prefix='10.10.0.2', vrf_handle=prev_vrf_hdl, nexthop_handle=nhop)
            prev_vrf_hdl = vrf_hdl
            print("Port: ", port, "Port-hdl:", port_hdl, "MAC: ", rmac, "VRF: ", vrf_hdl, "RIF: ", rif)

        # complete snake with redirect to first port
        nhop = self.add_nexthop(self.device, handle=prev_rif, dest_ip='10.10.0.2')
        neighbor = self.add_neighbor(self.device, mac_address=prev_rmac, handle=prev_rif, dest_ip='10.10.0.2')
        route = self.add_route(self.device, ip_prefix='10.10.0.2', vrf_handle=prev_vrf_hdl, nexthop_handle=nhop)


###############################################################################
@disabled
class DvL3OneToOneTest(ApiHelper):
    def runTest(self):
        print()
        # Sets up config to forward pkt back to the incoming port.
        # Used on emulator
        if test_param_get('target') != 'hw': return
        if test_param_get('arch') != 'tofino2':
            return
        num_pipes = int(test_param_get('num_pipes'))
        port_mode = test_param_get("port_mode")

        self.device = self.get_device_handle(0)

        port_max = 64 * num_pipes
        port_step = 1
        port_step, port_speed, fec = step_speed_fec_from_port_mode(port_mode)

        first_dev_port = 8
        # all ports
        for port in range(0, port_max, port_step):
            # logical port 0 is dev-port 8
            dev_port = port + first_dev_port

            rmac = '00:11:22:33:44:%02x' % (dev_port)
            port_hdl = self.add_port(self.device, lane_list=u.lane_list_t([port]), speed=port_speed, fec_type=fec)
            ingress_vrf = self.add_vrf(self.device, id=port, src_mac=rmac)
            rif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT,
                    port_handle=port_hdl, vrf_handle=ingress_vrf, src_mac=rmac,
                    mtu=9900)
            print("Port: ", port, "Port-hdl:", port_hdl, "MAC: ", rmac, "VRF: ", ingress_vrf, "RIF: ", rif)
            nhop = self.add_nexthop(self.device, handle=rif, dest_ip='10.10.0.2')
            # Increment mac by 1
            new_dst_mac = (dev_port+1)%port_max
            neighbor = self.add_neighbor(self.device, mac_address='00:11:22:33:44:%02x' % (new_dst_mac), handle=rif, dest_ip='10.10.0.2')
            route = self.add_route(self.device, ip_prefix='10.10.0.2', vrf_handle=ingress_vrf, nexthop_handle=nhop)

            if test_param_get('target') != 'hw':
                # Send pkt
                print("Sending pkt to port ", dev_port, "with mac-dst=00:11:22:33:44:08, ip-dst=10.10.0.2, dest-port=", dev_port)
                tx_pkt = simple_tcp_packet(eth_dst='00:11:22:33:44:%02x'% (dev_port), eth_src='00:00:00:11:00:38', ip_dst='10.10.0.2', ip_src='192.168.0.1', ip_ttl=64)
                rx_pkt = simple_tcp_packet(eth_dst='00:11:22:33:44:%02x'% (new_dst_mac), eth_src='00:11:22:33:44:%02x' % (dev_port), ip_dst='10.10.0.2', ip_src='192.168.0.1', ip_ttl=63)
                send_packet(self, dev_port, tx_pkt)
                verify_packet(self, rx_pkt, dev_port)

