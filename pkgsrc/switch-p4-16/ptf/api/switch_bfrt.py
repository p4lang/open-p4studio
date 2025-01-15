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
import ptf.mask

from bf_switcht_api_thrift.ttypes import *
from bf_switcht_api_thrift.model_headers import *

# bfrt imports
try:
    from bfruntime_base_tests import BfRuntimeTest
    import bfrt_grpc.bfruntime_pb2 as bfruntime_pb2
    import bfrt_grpc.client as client
except ImportError as e:
    pass

this_dir = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(this_dir, '..'))
from common.utils import *

###############################################################################

try:
    class SMIBfRtTest(ApiHelper, BfRuntimeTest):
        def runTest(self):
            print()
            print("Skipping")
            return
            BfRuntimeTest.setUp(self, 0, "switch")
            self.configure()
            self.pkt_count = 0

            self.set_bfrt_info(self.parse_bfrt_info(self.get_bfrt_info("switch")))

            vlan_mbr0 = self.add_vlan_member(self.device, vlan_handle=self.vlan10,
                member_handle=self.port0)
            vlan_mbr1 = self.add_vlan_member(self.device, vlan_handle=self.vlan10,
                member_handle=self.port1)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)

            mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan10,
                mac_address='00:11:11:11:11:11', destination_handle=self.port1)
            mac1 = self.add_mac_entry(self.device, vlan_handle=self.vlan10,
                mac_address='00:22:22:22:22:22', destination_handle=self.port0)

            try:
                self.AccessToAccessTest()
                self.VlanStatsTest()
            finally:
                self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
                self.attribute_set(self.port1, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
                self.cleanup()
                BfRuntimeTest.tearDown(self)

        def AccessToAccessTest(self):
            # Access to Access (0 -> 1)
            aa_pkt = simple_udp_packet(
                eth_dst='00:11:11:11:11:11',
                eth_src='00:22:22:22:22:22',
                ip_dst='10.0.0.1',
                ip_ttl=64)
            exp_aa_pkt = simple_udp_packet(
                eth_dst='00:11:11:11:11:11',
                eth_src='00:22:22:22:22:22',
                ip_dst='10.0.0.1',
                ip_ttl=64)

            try:
                print("Sending L2 packet from %d -> %d : Access to Access" % (self.devports[0], self.devports[1]))
                for i in range(0,10):
                    send_packet(self, self.devports[0], aa_pkt)
                    verify_packet(self, exp_aa_pkt, self.devports[1])
                    self.pkt_count += 1
            finally:
                pass

        def VlanStatsTest(self):
            '''
            Count the number of packets from the above tests
            '''
            thrift_count = 0
            grpc_count = 0

            # Get counters from SMI using thrift
            cntrs = self.client.object_counters_get(self.vlan10)
            # Ingress and egress u/m/b
            self.assertEqual(len(cntrs), 6)
            # Lookup the unicast packet count
            for cntr in cntrs:
                if cntr.counter_id == SWITCH_VLAN_ATTR_ID_IN_UCAST:
                    thrift_count = cntr.num_packets
                    self.assertEqual(cntr.num_packets, self.pkt_count)
                if cntr.counter_id == SWITCH_VLAN_ATTR_ID_OUT_UCAST:
                    self.assertEqual(cntr.num_packets, self.pkt_count)

            # Get counters from bfrt using grpc
            target = self.Target(device_id=0, pipe_id=0xffff)
            resp = self.get_table_entry(target, 'SwitchIngress.bd_stats.bd_stats',
                [self.KeyField('bd', self.to_bytes(10, 2)),
                 self.KeyField('pkt_type', self.to_bytes(SWITCH_VLAN_ATTR_ID_IN_UCAST, 1))],
                {"from_hw":True}, None)

            pkts_field_id = self.get_data_field("SwitchIngress.bd_stats.bd_stats", None, "$COUNTER_SPEC_PKTS")
            data_dict = next(self.parseEntryGetResponse(resp))
            recv_pkts = ''.join(binascii.hexlify(x) for x in str(data_dict[pkts_field_id]))
            grpc_count = int(recv_pkts, 16)
            self.assertEqual(thrift_count, grpc_count)
except NameError as e:
    pass
