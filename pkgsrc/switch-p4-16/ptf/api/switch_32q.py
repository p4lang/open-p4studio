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
from ptf.thriftutils import *

import os
import ptf.mask

from bf_switcht_api_thrift.ttypes import *
from bf_switcht_api_thrift.model_headers import *

# bfrt imports
try:
    from bfruntime_client_base_tests import BfRuntimeTest
    import bfrt_grpc.bfruntime_pb2 as bfruntime_pb2
    import bfrt_grpc.client as client
except ImportError as e:
    pass

this_dir = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(this_dir, '..'))
from common.utils import *

###############################################################################

try:

    @disabled
    class Test32Q(ApiHelper, BfRuntimeTest):
        def runTest(self):
            print()
            print("Skipping")
            self.dst_ip = '11.11.11.1'
            self.src_ip = '12.12.12.1'
            self.dport = 17
            self.sport = 16
            self.dmac = ['00:44:44:44:44:11', '00:44:44:44:44:22']
            self.dip = ['10.10.10.1', '10.10.10.2']
            self.dip_dport = [171, 172]
            self.members = [1, 2]
            self.member_status = [True, True]
            self.group_id = 1
            self.internal_port = 136
            BfRuntimeTest.setUp(self, 0, "switch", notifications=client.Notifications(enable_learn=False))
            self.bfrt_info = self.interface.bfrt_info_get("switch")
            try:
                self.configure()
                self.rif0 = self.add_rif(
                    self.device,
                    type=SWITCH_RIF_ATTR_TYPE_PORT,
                    port_handle=self.port0,
                    vrf_handle=self.vrf10,
                    src_mac=self.rmac)

                self.configure_vip_dip()
                self.LBTest()
            finally:
                self.cleanup()
                self.clean_vip_dip()
                BfRuntimeTest.tearDown(self)

        def clean_vip_dip(self):
            target = client.Target(device_id=0, pipe_id=0xffff)
            for i in range(0, 2):
                self.entry_del(target, 'L4LBIngress.fib', [
                    self.KeyField('hdr.ipv4.dst_addr',
                                  self.ipv4_to_bytes(self.dip[i]))
                ])

            self.delete_table_entry(target, 'L4LBEgress.l4lb.dip_pool', [
                self.KeyField('pool_version', self.to_bytes(0, 1)),
                self.KeyField('vip', self.ipv4_to_bytes(self.dst_ip)),
                self.KeyField('local_md.lkp.l4_dst_port',
                              self.to_bytes(self.dport, 2)),
            ])
            self.delete_table_entry(
                target, 'L4LBEgress.l4lb.dip_selector_sel', [
                    self.KeyField('$SELECTOR_GROUP_ID',
                                  self.to_bytes(self.group_id, 4))
                ])
            for i in range(0, 2):
                self.delete_table_entry(
                    target, 'L4LBEgress.l4lb.dip_selector', [
                        self.KeyField('$ACTION_MEMBER_ID',
                                      self.to_bytes(self.members[i], 4))
                    ])

            self.delete_table_entry(target, 'SwitchIngress.vip', [
                self.KeyField('hdr.ipv4.dst_addr',
                              self.ipv4_to_bytes(self.dst_ip)),
                self.KeyField('local_md.lkp.l4_dst_port',
                              self.to_bytes(self.dport, 2)),
                self.KeyField('local_md.lkp.ip_proto', self.to_bytes(6, 1))
            ])

        def configure_vip_dip(self):
            # Add entry to vip table in ingress
            target = client.Target(device_id=0, pipe_id=0xffff)
            vip = self.bfrt_info.table_get("SwitchIngress.vip")
            key = vip.make_key([
                    client.KeyTuple('hdr.ipv4.dst_addr', 0x0b0b0b01),
                    client.KeyTuple('local_md.lkp.l4_dst_port', self.dport),
                    client.KeyTuple('local_md.lkp.ip_proto', 6)])
            data = vip.make_data([client.DataTuple('port', self.internal_port)],
                    'SwitchIngress.vip_hit')
            vip.entry_add(target, [key], [data])

            '''
            # Add entry to dip_selector in internal egress pipe
            for i in range(0, 2):
                self.insert_table_entry(
                    target, 'L4LBEgress.l4lb.dip_selector', [
                        self.KeyField('$ACTION_MEMBER_ID',
                                      self.to_bytes(self.members[i], 4))
                    ], 'L4LBEgress.l4lb.set_dip', [
                        self.DataField('dip', self.ipv4_to_bytes(self.dip[i])),
                        self.DataField('dst_port',
                                       self.to_bytes(self.dip_dport[i], 2))
                    ])

            self.insert_table_entry(
                target, 'L4LBEgress.l4lb.dip_selector_sel', [
                    self.KeyField('$SELECTOR_GROUP_ID',
                                  self.to_bytes(self.group_id, 4))
                ], None, [
                    self.DataField('$MAX_GROUP_SIZE', self.to_bytes(4, 4)),
                    self.DataField(
                        '$ACTION_MEMBER_ID', int_arr_val=self.members),
                    self.DataField(
                        '$ACTION_MEMBER_STATUS',
                        bool_arr_val=self.member_status)
                ])

            self.insert_table_entry(target, 'L4LBEgress.l4lb.dip_pool', [
                self.KeyField('pool_version', self.to_bytes(0, 1)),
                self.KeyField('vip', self.ipv4_to_bytes(self.dst_ip)),
                self.KeyField('local_md.lkp.l4_dst_port',
                              self.to_bytes(self.dport, 2)),
            ], None, [
                self.DataField('$SELECTOR_GROUP_ID',
                               self.to_bytes(self.group_id, 4))
            ])

            # add entry to internal ingress pipe
            for i in range(0, 2):
                self.insert_table_entry(target, 'L4LBIngress.fib', [
                    self.KeyField('hdr.ipv4.dst_addr',
                                  self.ipv4_to_bytes(self.dip[i]))
                ], 'L4LBIngress.fib_hit', [
                    self.DataField('dmac', self.mac_to_bytes(self.dmac[i])),
                    self.DataField('port',
                                   self.to_bytes(self.devports[i + 1], 2))
                ])
            '''

        def LBTest(self):
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst=self.dst_ip,
                tcp_dport=self.dport,
                ip_src=self.src_ip,
                ip_ttl=64)
            exp_1_pkt = ptf.mask.Mask(
                simple_tcp_packet(
                    eth_dst=self.dmac[0],
                    eth_src='00:22:22:22:22:22',
                    ip_dst=self.dip[0],
                    tcp_dport=self.dip_dport[0],
                    ip_src=self.src_ip,
                    ip_ttl=64))
            exp_2_pkt = ptf.mask.Mask(
                simple_tcp_packet(
                    eth_dst=self.dmac[1],
                    eth_src='00:22:22:22:22:22',
                    ip_dst=self.dip[1],
                    tcp_dport=self.dip_dport[1],
                    ip_src=self.src_ip,
                    ip_ttl=64))
            mask_set_do_not_care_packet(exp_1_pkt, IP, "chksum")
            mask_set_do_not_care_packet(exp_2_pkt, IP, "chksum")
            mask_set_do_not_care_packet(exp_1_pkt, TCP, "chksum")
            mask_set_do_not_care_packet(exp_2_pkt, TCP, "chksum")

            try:
                time.sleep(2)
                print("Sending L2 packet from %d: expect on %d or %d" % (
                    self.devports[0], self.devports[1], self.devports[2]))
                send_packet(self, self.devports[0], pkt)
                verify_any_packet_on_ports_list(
                    self, [exp_1_pkt, exp_2_pkt],
                    [[self.devports[1], self.devports[2]]],
                    timeout=3)
            finally:
                pass
except NameError as e:
    pass
