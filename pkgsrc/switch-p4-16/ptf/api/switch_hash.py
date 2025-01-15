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
Thrift BF_SWITCH API hash API
"""

import bf_switcht_api_thrift

import binascii

import time
import sys
import logging

import unittest
import random

import ptf.dataplane as dataplane
import ptf.packet as packet
import api_base_tests
import pd_base_tests
from switch_helpers import *
import model_utils as u
from p4testutils.misc_utils import mask_set_do_not_care_packet

from ptf import config
from ptf.testutils import *
from ptf.packet import *
from ptf.thriftutils import *

import os
import ptf.mask

from bf_switcht_api_thrift.ttypes import *
from bf_switcht_api_thrift.model_headers import *
from ipaddress import ip_address, ip_network

this_dir = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(this_dir, '..'))
from common.utils import *
from common.pktpy_utils import pktpy_skip # noqa pylint: disable=wrong-import-position

from common.fib import *
from common.lpm import *

#Global defines
HASH_TRAFFIC_DISTRIBUTION_TOLERANCE = 0.7

def verify_lb_changed(ecmp_count1, ecmp_count2):
    if len(ecmp_count1) != len(ecmp_count2):
        # list must have equal len
        return False

    diff = 0
    for count, value in enumerate(ecmp_count1):
        diff += abs(ecmp_count1[count] - ecmp_count2[count])

    if diff == 0:
        # value 0 indicate the hashing did not change
        return False

    # value other then 0 means detected hasing change
    return True

class HashHelper():

    '''
    Global hash variable
    '''
    hash_obj = 0
    fg_hash_obj_list = []
    hash_attr_list = []
    max_itrs  = 120
    src_ip_enable = 0
    dst_ip_enable = 0
    sport_enable = 0
    dport_enable = 0
    ipv6_fl_enable = 0

    def setMaxItrs(self, value):
        self.max_itrs = value

    def setTrafficParam(self, src=0, dst=0, sport=0, dport=0, ipv6_fl=0):
        self.src_ip_enable = src
        self.dst_ip_enable = dst
        self.sport_enable = sport
        self.dport_enable = dport
        self.ipv6_fl_enable = ipv6_fl

    def verify_equaly_balanced(self, count, expected_base=HASH_TRAFFIC_DISTRIBUTION_TOLERANCE, log="Traffic"):
        error_message = log + " are not equally balanced"
        for i in range(0, 4):
            self.assertTrue((count[i] >= ((self.max_itrs / 4) * expected_base)), error_message)

    def verify_single_port_traffic(self, count, log="Traffic"):
        error_message = log + " should only use one port"
        for i in range(0, 4):
            self.assertTrue((count[i] == self.max_itrs) or (count[i] == 0), error_message)

    def ipv6Traffic(self):
        try:
            print('V6 Lag has test with', self.max_itrs , 'packets')
            pkt = simple_tcpv6_packet(
                eth_src='00:11:11:11:11:11',
                eth_dst='00:22:22:22:22:22',
                ipv6_dst='4000:5678:9abc:def0:4422:1133:5577:1111',
                ipv6_src='2000:5678:9abc:def0:4422:1133:5577:1111',
                ipv6_hlim=63,
                ipv6_fl=10)

            count = [0, 0, 0, 0]
            dst_ip = int(binascii.hexlify(socket.inet_pton(socket.AF_INET6, '4000:5678:9abc:def0:4422:1133:5577:1111')), 16)
            src_ip = int(binascii.hexlify(socket.inet_pton(socket.AF_INET6, '2000:5678:9abc:def0:4422:1133:5577:1111')), 16)
            ipv6_fl_value = 10
            for i in range(0, self.max_itrs):
                #Vary the Destination IP
                if self.dst_ip_enable == 1:
                    dst_ip_addr = socket.inet_ntop(socket.AF_INET6,
                        binascii.unhexlify(format(dst_ip, 'x').zfill(8)))
                    pkt['IPv6'].dst = dst_ip_addr
                #Vary the Source IP
                if self.src_ip_enable == 1:
                    src_ip_addr = socket.inet_ntop(socket.AF_INET6,
                         binascii.unhexlify(format(src_ip, 'x').zfill(8)))
                    pkt['IPv6'].src = src_ip_addr
                #Vary ipv6 flow label value
                if self.ipv6_fl_enable == 1:
                   pkt['IPv6'].ipv6_fl = ipv6_fl_value

                send_packet(self, self.devports[0], pkt)
                rcv_idx = verify_any_packet_any_port(
                     self, [pkt, pkt, pkt, pkt], [
                     self.devports[1], self.devports[2], self.devports[3], self.devports[4]
                     ], timeout=2)
                count[rcv_idx] += 1
                if self.src_ip_enable == 1:
                    src_ip += 1
                if self.dst_ip_enable == 1:
                    dst_ip += 1
                if self.ipv6_fl_enable == 1:
                   ipv6_fl_value += 10
            return count
        finally:
            pass

    def createFgLagHashList(self,
                            field_name_list=None,
                            p_src_port_mask=0,
                            p_dst_port_mask=0,
                            p_ip_proto_mask=0,
                            p_dst_ip_mask='0.0.0.0',
                            p_src_ip_mask='0.0.0.0',
                            p_ipv6_label_mask=0):

        if (field_name_list is None):
            field_name_list = [SWITCH_HASH_ATTR_FIELD_SRC_ADDR,
                         SWITCH_HASH_ATTR_FIELD_DST_ADDR,
                         SWITCH_HASH_ATTR_FIELD_IP_PROTO,
                         SWITCH_HASH_ATTR_FIELD_DST_PORT,
                         SWITCH_HASH_ATTR_FIELD_SRC_PORT,
                         SWITCH_HASH_ATTR_FIELD_IPV6_FLOW_LABEL]

        #Creating the fine grained hash object list
        index = 1

        for field_attr in field_name_list:
            fg_hash_field_id = 0;
            if (field_attr == SWITCH_HASH_ATTR_FIELD_SRC_ADDR):
                fg_hash_field_id = self.add_fine_grained_hash(self.device,
                                 field_name=field_attr,
                                 src_ip_mask=p_src_ip_mask,
                                 sequence=index)

            if (field_attr == SWITCH_HASH_ATTR_FIELD_DST_ADDR):
                fg_hash_field_id = self.add_fine_grained_hash(self.device,
                                 field_name=field_attr,
                                 dst_ip_mask=p_dst_ip_mask,
                                 sequence=index)

            if (field_attr == SWITCH_HASH_ATTR_FIELD_IP_PROTO):
                fg_hash_field_id = self.add_fine_grained_hash(self.device,
                                 field_name=field_attr,
                                 ip_proto_mask=p_ip_proto_mask,
                                 sequence=index)

            if (field_attr == SWITCH_HASH_ATTR_FIELD_DST_PORT):
                fg_hash_field_id = self.add_fine_grained_hash(self.device,
                                 field_name=field_attr,
                                 dst_port_mask=p_dst_port_mask,
                                 sequence=index)

            if (field_attr == SWITCH_HASH_ATTR_FIELD_SRC_PORT):
                fg_hash_field_id = self.add_fine_grained_hash(self.device,
                                 field_name=field_attr,
                                 src_port_mask=p_src_port_mask,
                                 sequence=index)

            if (field_attr == SWITCH_HASH_ATTR_FIELD_IPV6_FLOW_LABEL):
                fg_hash_field_id = self.add_fine_grained_hash(self.device,
                                 field_name=field_attr,
                                 ipv6_flow_label_mask=p_ipv6_label_mask,
                                 sequence=index)

            fg_hash_obj_id = switcht_list_val_t(type=switcht_value_type.OBJECT_ID,
                                                            oid=fg_hash_field_id)
            (self.fg_hash_obj_list).append(fg_hash_obj_id)
            index += 1

    def createLagHashList(self, field_name_list=None):

        if (field_name_list is None):
            field_name_list = [SWITCH_HASH_ATTR_FIELD_SRC_ADDR,
                         SWITCH_HASH_ATTR_FIELD_DST_ADDR,
                         SWITCH_HASH_ATTR_FIELD_IP_PROTO,
                         SWITCH_HASH_ATTR_FIELD_DST_PORT,
                         SWITCH_HASH_ATTR_FIELD_SRC_PORT,
                         SWITCH_HASH_ATTR_FIELD_IPV6_FLOW_LABEL]

        for attr in field_name_list:
            if(attr == SWITCH_HASH_ATTR_FIELD_SRC_ADDR):
                self.hash_attr_list.append(switcht_list_val_t(type=switcht_value_type.UINT32,
                                        u32data=SWITCH_HASH_ATTR_FIELD_SRC_ADDR))

            if(attr == SWITCH_HASH_ATTR_FIELD_DST_ADDR):
                self.hash_attr_list.append(switcht_list_val_t(type=switcht_value_type.UINT32,
                                        u32data=SWITCH_HASH_ATTR_FIELD_DST_ADDR))

            if(attr == SWITCH_HASH_ATTR_FIELD_IP_PROTO):
                self.hash_attr_list.append(switcht_list_val_t(type=switcht_value_type.UINT32,
                                        u32data=SWITCH_HASH_ATTR_FIELD_IP_PROTO))

            if(attr == SWITCH_HASH_ATTR_FIELD_DST_PORT):
                self.hash_attr_list.append(switcht_list_val_t(type=switcht_value_type.UINT32,
                                        u32data=SWITCH_HASH_ATTR_FIELD_DST_PORT))

            if(attr == SWITCH_HASH_ATTR_FIELD_SRC_PORT):
                self.hash_attr_list.append(switcht_list_val_t(type=switcht_value_type.UINT32,
                                        u32data=SWITCH_HASH_ATTR_FIELD_SRC_PORT))

            if(attr == SWITCH_HASH_ATTR_FIELD_IPV6_FLOW_LABEL):
                self.hash_attr_list.append(switcht_list_val_t(type=switcht_value_type.UINT32,
                                        u32data=SWITCH_HASH_ATTR_FIELD_SRC_PORT))


###############################################################################

@disabled
# drv-7209
class L2LagSeedTest(ApiHelper):

    def runTest(self):
        print()
        if self.client.is_feature_enable(SWITCH_FEATURE_INNER_HASH):
            print("Dynamic Hashing not yet supported when inner packet hash is used")
            return
        self.configure()
        self.seed = 0xabcdef
        self.offset = 2
        self.default_seed = 0
        self.default_offset = 0
        self.lb_factor = 0.5

        self.lag0 = self.add_lag(self.device)
        self.lag0_mbr0 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port1)
        self.lag0_mbr1 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port2)
        self.lag0_mbr2 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port3)
        self.lag0_mbr3 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port4)

        mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan10,
            mac_address='00:11:11:11:11:11', destination_handle=self.port0)
        mac1 = self.add_mac_entry(self.device, vlan_handle=self.vlan10,
            mac_address='00:22:22:22:22:00', destination_handle=self.lag0)
        dst_mac_start = '00:22:22:22:22:'
        for i in range(1,121):
            dst_mac = dst_mac_start + hex(i)[2:]
            mac1 = self.add_mac_entry(self.device, vlan_handle=self.vlan10,
                mac_address=dst_mac, destination_handle=self.lag0)
        self.default_seed = self.attribute_get(self.device, SWITCH_DEVICE_ATTR_LAG_DEFAULT_HASH_SEED)
        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_LAG_DEFAULT_HASH_SEED, self.seed)
        self.default_offset = self.attribute_get(self.device, SWITCH_DEVICE_ATTR_LAG_DEFAULT_HASH_OFFSET)
        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_LAG_DEFAULT_HASH_OFFSET, self.offset)

        vlan_mbr0 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port0)
        vlan_mbr1 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.lag0)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.attribute_set(self.lag0, SWITCH_LAG_ATTR_PORT_VLAN_ID, 10)

        try:
            self.IPv4Test()
            self.IPv6Test()
        finally:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.lag0, SWITCH_LAG_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.device, SWITCH_DEVICE_ATTR_LAG_DEFAULT_HASH_SEED, self.default_seed)
            self.attribute_set(self.device, SWITCH_DEVICE_ATTR_LAG_DEFAULT_HASH_OFFSET, self.default_offset)
            self.cleanup()

    def IPv6Test(self):
        print("L2 Lag basic test with 160 ipv6 packets")

        try:
            max_itrs = 160
            def packetTest():
                pkt = simple_tcpv6_packet(
                    eth_src='00:11:11:11:11:11',
                    eth_dst='00:22:22:22:22:00',
                    ipv6_dst='4000:5678:9abc:def0:4422:1133:5577:1111',
                    ipv6_src='2000:5678:9abc:def0:4422:1133:5577:1111',
                    ipv6_hlim=64)
                count = [0, 0, 0, 0]
                dst_ip = int(binascii.hexlify(socket.inet_pton(socket.AF_INET6, '4000:5678:9abc:def0:4422:1133:5577:1111')), 16)
                src_ip = int(binascii.hexlify(socket.inet_pton(socket.AF_INET6, '2000:5678:9abc:def0:4422:1133:5577:1111')), 16)
                for i in range(0, max_itrs):
                    dst_ip_addr = socket.inet_ntop(socket.AF_INET6,
                        binascii.unhexlify(format(dst_ip, 'x').zfill(8)))
                    src_ip_addr = socket.inet_ntop(socket.AF_INET6,
                        binascii.unhexlify(format(src_ip, 'x').zfill(8)))
                    pkt['IPv6'].dst = dst_ip_addr
                    pkt['IPv6'].src = src_ip_addr
                    send_packet(self, self.devports[0], pkt)
                    rcv_idx = verify_any_packet_any_port(
                        self, [pkt, pkt, pkt, pkt],
                        [self.devports[1], self.devports[2], self.devports[3], self.devports[4]])
                    count[rcv_idx] += 1
                    dst_ip += 1000000
                    src_ip += 1000000
                return count

            prev_lb = [0]*4
            for i in range(1,4):
                self.seed = self.seed + (i * 17)
                self.attribute_set(self.device, SWITCH_DEVICE_ATTR_LAG_DEFAULT_HASH_SEED, self.seed)
                lb_count = packetTest()
                print('Seed: ', self.seed, ', offset ', self.offset, ', lag-count:', lb_count)
                for i in range(0, 4):
                    self.assertTrue((lb_count[i] >= ((max_itrs / 4) * self.lb_factor)),
                                    "Lag paths are not equally balanced")
                self.assertTrue(verify_lb_changed(lb_count, prev_lb),
                                    "Lag LB did not change post seed change")
                prev_lb = lb_count

            prev_lb = [0]*4
            for i in range(1,4):
                self.offset = self.offset + 2
                self.attribute_set(self.device, SWITCH_DEVICE_ATTR_LAG_DEFAULT_HASH_OFFSET, self.offset)
                lb_count = packetTest()
                print('Seed: ', self.seed, ', offset ', self.offset, ', lag-count:', lb_count)
                for i in range(0, 4):
                    self.assertTrue((lb_count[i] >= ((max_itrs / 4) * self.lb_factor)),
                                    "Lag paths are not equally balanced")
                self.assertTrue(verify_lb_changed(lb_count, prev_lb),
                                    "Lag LB did not change post offset change")
                prev_lb = lb_count
        finally:
            pass

    def IPv4Test(self):
        print("L2 Lag basic test with 160 ipv4 packets")

        try:
            max_itrs = 160
            def packetTest():
                pkt = simple_tcp_packet(
                    eth_src='00:11:11:11:11:11',
                    eth_dst='00:22:22:22:22:00',
                    ip_dst='10.10.10.1',
                    ip_src='192.168.8.1',
                    ip_id=109,
                    ip_ttl=64)
                exp_pkt = simple_tcp_packet(
                    eth_src='00:11:11:11:11:11',
                    eth_dst='00:22:22:22:22:00',
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
                    dst_ip += 1000
                    src_ip += 1000
                return count

            prev_lb = [0]*4
            for i in range(1,4):
                self.seed = self.seed + (i * 17)
                self.attribute_set(self.device, SWITCH_DEVICE_ATTR_LAG_DEFAULT_HASH_SEED, self.seed)
                lb_count = packetTest()
                print('Seed: ', self.seed, ', offset ', self.offset, ', lag-count:', lb_count)
                for i in range(0, 4):
                    self.assertTrue((lb_count[i] >= ((max_itrs / 4) * self.lb_factor)),
                                    "Lag paths are not equally balanced")
                self.assertTrue(verify_lb_changed(lb_count, prev_lb),
                                    "Lag LB did not change post seed change")
                prev_lb = lb_count
            prev_lb = [0]*4
            for i in range(1,4):
                self.offset = self.offset + 2
                self.attribute_set(self.device, SWITCH_DEVICE_ATTR_LAG_DEFAULT_HASH_OFFSET, self.offset)
                lb_count = packetTest()
                print('Seed: ', self.seed, ', offset ', self.offset, ', lag-count:', lb_count)
                for i in range(0, 4):
                    self.assertTrue((lb_count[i] >= ((max_itrs / 4) * self.lb_factor)),
                                    "Lag paths are not equally balanced")
                self.assertTrue(verify_lb_changed(lb_count, prev_lb),
                                    "Lag LB did not change post offset change")
                prev_lb = lb_count
        finally:
            pass

###############################################################################

@group('hash')
class L3ECMPSeedTest(ApiHelper):
    def runTest(self):
        print()

        self.configure()
        self.seed = 0xabcdef
        self.offset = 2
        self.default_seed = 0
        self.default_offset = 0
        self.lb_factor = 0.5

        self.rif0 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port0,
            vrf_handle=self.vrf10, src_mac=self.rmac)
        self.ecmp = self.add_ecmp(self.device)
        self.route4 = self.add_route(self.device, ip_prefix='13.0.0.0/8', vrf_handle=self.vrf10, nexthop_handle=self.ecmp)
        self.route6 = self.add_route(self.device, ip_prefix='4000:5678:9abc:def0:0000:0000:0000:0000/64', vrf_handle=self.vrf10, nexthop_handle=self.ecmp)
        for l_port in [self.port1, self.port2, self.port3, self.port4]:
            l_rif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=l_port, vrf_handle=self.vrf10, src_mac=self.rmac)
            l_ip = '13.0.0.' + str(l_port & 0xFF)
            l_mac = '00:22:22:33:44:' + str(l_port & 0xFF)
            l_nhop = self.add_nexthop(self.device, handle=l_rif, dest_ip=l_ip)
            l_neighbor = self.add_neighbor(self.device, mac_address=l_mac, handle=l_rif, dest_ip=l_ip)
            ecmp_member = self.add_ecmp_member(self.device, nexthop_handle=l_nhop, ecmp_handle=self.ecmp)
        self.default_seed = self.attribute_get(self.device, SWITCH_DEVICE_ATTR_ECMP_DEFAULT_HASH_SEED)
        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_ECMP_DEFAULT_HASH_SEED, self.seed)
        self.default_offset = self.attribute_get(self.device, SWITCH_DEVICE_ATTR_ECMP_DEFAULT_HASH_OFFSET)
        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_ECMP_DEFAULT_HASH_OFFSET, self.offset)

        try:
            self.Testv4()
            self.Testv6()
        finally:
            self.attribute_set(self.device, SWITCH_DEVICE_ATTR_ECMP_DEFAULT_HASH_SEED, self.default_seed)
            self.attribute_set(self.device, SWITCH_DEVICE_ATTR_ECMP_DEFAULT_HASH_OFFSET, self.default_offset)
            self.cleanup()

    def Testv4(self):
        print("Test to verify v4 ECMP load balancing after changing seed")

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
            max_itrs = 160
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
                    dst_ip += 1000
                    src_ip += 1000
                return count

            self.seed = 0xabcdef
            prev_lb = [0]*4
            for i in range(1,4):
                self.seed = self.seed + (i * 17)
                self.attribute_set(self.device, SWITCH_DEVICE_ATTR_ECMP_DEFAULT_HASH_SEED, self.seed)
                lb_count = packetTest()
                print('Seed: ', self.seed, ', offset ', self.offset, ', ecmp-count:', lb_count)
                for i in range(0, 4):
                    self.assertTrue((lb_count[i] >= ((max_itrs / 4) * self.lb_factor)),
                                    "Ecmp paths are not equally balanced")
                self.assertTrue(verify_lb_changed(lb_count, prev_lb),
                                    "ECMP LB did not change post seed change")
                prev_lb = lb_count
            prev_lb = [0]*4
            for i in range(1,4):
                self.offset = self.offset + 2
                self.attribute_set(self.device, SWITCH_DEVICE_ATTR_ECMP_DEFAULT_HASH_OFFSET, self.offset)
                lb_count = packetTest()
                print('Seed: ', self.seed, ', offset ', self.offset, ', ecmp-count:', lb_count)
                for i in range(0, 4):
                    self.assertTrue((lb_count[i] >= ((max_itrs / 4) * self.lb_factor)),
                                    "Ecmp paths are not equally balanced")
                self.assertTrue(verify_lb_changed(lb_count, prev_lb),
                                    "ECMP LB did not change post offset change")
                prev_lb = lb_count
        finally:
            pass

    def Testv6(self):
        print("Test to verify v6 ECMP load balancing after changing seed")

        pkt = simple_tcpv6_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ipv6_dst='4000:5678:9abc:def0:4422:1133:5577:1111',
            ipv6_src='2000:5678:9abc:def0:4422:1133:5577:1111',
            ipv6_hlim=64)
        exp_pkt1 = simple_tcpv6_packet(
            eth_dst='00:22:22:33:44:' + str(self.port1 & 0xFF),
            eth_src='00:77:66:55:44:33',
            ipv6_dst='4000:5678:9abc:def0:4422:1133:5577:1111',
            ipv6_src='2000:5678:9abc:def0:4422:1133:5577:1111',
            ipv6_hlim=63)
        exp_pkt2 = simple_tcpv6_packet(
            eth_dst='00:22:22:33:44:' + str(self.port2 & 0xFF),
            eth_src='00:77:66:55:44:33',
            ipv6_dst='4000:5678:9abc:def0:4422:1133:5577:1111',
            ipv6_src='2000:5678:9abc:def0:4422:1133:5577:1111',
            ipv6_hlim=63)
        exp_pkt3 = simple_tcpv6_packet(
            eth_dst='00:22:22:33:44:' + str(self.port3 & 0xFF),
            eth_src='00:77:66:55:44:33',
            ipv6_dst='4000:5678:9abc:def0:4422:1133:5577:1111',
            ipv6_src='2000:5678:9abc:def0:4422:1133:5577:1111',
            ipv6_hlim=63)
        exp_pkt4 = simple_tcpv6_packet(
            eth_dst='00:22:22:33:44:' + str(self.port4 & 0xFF),
            eth_src='00:77:66:55:44:33',
            ipv6_dst='4000:5678:9abc:def0:4422:1133:5577:1111',
            ipv6_src='2000:5678:9abc:def0:4422:1133:5577:1111',
            ipv6_hlim=63)
        try:
            max_itrs = 160
            def packetTest():
                count = [0, 0, 0, 0]
                dst_ip = int(binascii.hexlify(socket.inet_pton(socket.AF_INET6, '4000:5678:9abc:def0:4422:1133:5577:1111')), 16)
                src_ip = int(binascii.hexlify(socket.inet_pton(socket.AF_INET6, '2000:5678:9abc:def0:4422:1133:5577:1111')), 16)
                for i in range(0, max_itrs):
                    dst_ip_addr = socket.inet_ntop(socket.AF_INET6,
                        binascii.unhexlify(format(dst_ip, 'x').zfill(8)))
                    src_ip_addr = socket.inet_ntop(socket.AF_INET6,
                        binascii.unhexlify(format(src_ip, 'x').zfill(8)))
                    pkt['IPv6'].dst = dst_ip_addr
                    pkt['IPv6'].src = src_ip_addr
                    exp_pkt1['IPv6'].dst = dst_ip_addr
                    exp_pkt1['IPv6'].src = src_ip_addr
                    exp_pkt2['IPv6'].dst = dst_ip_addr
                    exp_pkt2['IPv6'].src = src_ip_addr
                    exp_pkt3['IPv6'].dst = dst_ip_addr
                    exp_pkt3['IPv6'].src = src_ip_addr
                    exp_pkt4['IPv6'].dst = dst_ip_addr
                    exp_pkt4['IPv6'].src = src_ip_addr

                    send_packet(self, self.devports[0], pkt)
                    rcv_idx = verify_any_packet_any_port(
                        self, [exp_pkt1, exp_pkt2, exp_pkt3, exp_pkt4], [
                            self.devports[1], self.devports[2], self.devports[3], self.devports[4]
                        ], timeout=2)
                    count[rcv_idx] += 1
                    dst_ip += 1000000
                    src_ip += 1000000
                return count

            self.seed = 0xabcdef
            prev_lb = [0]*4
            for i in range(1,4):
                self.seed = self.seed + (i * 17)
                self.attribute_set(self.device, SWITCH_DEVICE_ATTR_ECMP_DEFAULT_HASH_SEED, self.seed)
                lb_count = packetTest()
                print('Seed: ', self.seed, ', offset ', self.offset, ', ecmp-count:', lb_count)
                for i in range(0, 4):
                    self.assertTrue((lb_count[i] >= ((max_itrs / 4) * self.lb_factor)),
                                    "Ecmp paths are not equally balanced")
                self.assertTrue(verify_lb_changed(lb_count, prev_lb),
                                    "ECMP LB did not change post seed change")
                prev_lb = lb_count
            prev_lb = [0]*4
            for i in range(1,4):
                self.offset = self.offset + 2
                self.attribute_set(self.device, SWITCH_DEVICE_ATTR_ECMP_DEFAULT_HASH_OFFSET, self.offset)
                lb_count = packetTest()
                print('Seed: ', self.seed, ', offset ', self.offset, ', ecmp-count:', lb_count)
                for i in range(0, 4):
                    self.assertTrue((lb_count[i] >= ((max_itrs / 4) * self.lb_factor)),
                                    "Ecmp paths are not equally balanced")
                self.assertTrue(verify_lb_changed(lb_count, prev_lb),
                                    "ECMP LB did not change post offset change")
                prev_lb = lb_count
        finally:
            pass

###############################################################################

@group('hash')
class NonIPHashTest(ApiHelper):
    '''
    This performs Non-IP traffic load balancing
    @test - Non-IP Hash Test with selecting all fields
    @test - Non-IP Hash using only SRC MAC field
    '''
    hash_obj = 0;
    max_itrs = 200

    def selectNonIpHashField(self, attr_list, attr='all'):

        if(attr == 'all' or attr == SWITCH_HASH_ATTR_FIELD_INGRESS_PORT):
            attr_list.append(switcht_list_val_t(type=switcht_value_type.UINT32,
                                        u32data=SWITCH_HASH_ATTR_FIELD_INGRESS_PORT))

        if(attr == 'all' or attr == SWITCH_HASH_ATTR_FIELD_MAC_TYPE):
            attr_list.append(switcht_list_val_t(type=switcht_value_type.UINT32,
                                        u32data=SWITCH_HASH_ATTR_FIELD_MAC_TYPE))

        if(attr == 'all' or attr == SWITCH_HASH_ATTR_FIELD_SRC_MAC):
            attr_list.append(switcht_list_val_t(type=switcht_value_type.UINT32,
                                         u32data=SWITCH_HASH_ATTR_FIELD_SRC_MAC))

        if(attr == 'all' or attr == SWITCH_HASH_ATTR_FIELD_DST_MAC):
            attr_list.append(switcht_list_val_t(type=switcht_value_type.UINT32,
                                         u32data=SWITCH_HASH_ATTR_FIELD_DST_MAC))

    def setUp(self):
        print("")
        self.configure()

        #Configuring LAG
        self.lag0 = self.add_lag(self.device)
        self.lag0_mbr0 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port1)
        self.lag0_mbr1 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port2)
        self.lag0_mbr2 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port3)
        self.lag0_mbr3 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port4)

        #Populating the MAC table
        mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan10,
            mac_address='00:11:11:11:11:00', destination_handle=self.port0)
        mac1 = self.add_mac_entry(self.device, vlan_handle=self.vlan10,
            mac_address='00:22:22:22:22:22', destination_handle=self.lag0)

        src_mac_start = '00:11:11:11:11:'
        for i in range(1,201):
            src_mac = src_mac_start + hex(i)[2:]
            mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan10,
                mac_address=src_mac, destination_handle=self.port0)

        #configuring the VLAN
        vlan_mbr0 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port0)
        vlan_mbr1 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.lag0)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.attribute_set(self.lag0, SWITCH_LAG_ATTR_PORT_VLAN_ID, 10)

        #Selecting all the attributes for non-ip hash
        attr_list = []
        fg_attr_list = []
        self.selectNonIpHashField(attr_list)

        #creating the hash_obj
        print ("Setting the NON-IP hash object with all fields..")
        NonIPHashTest.hash_obj = self.add_hash(self.device, field_list=attr_list,
                                             fine_grained_field_list=fg_attr_list)

        #Associating the hash obj to the device
        print("Setting the device object with NON-IP HASH")
        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_NON_IP_HASH, NonIPHashTest.hash_obj)

    def runTest(self):
        try:
            self.basicNonIpHashTest()

            if self.client.is_feature_enable(SWITCH_FEATURE_INNER_HASH):
                print("Dynamic Hashing not yet supported when inner packet hash is used")
                return
            print("Waiting for 5 Secs..")
            time.sleep(5)

            self.nonIpSrcMacHashTest()

            print("Waiting for 5 Secs..")
            time.sleep(5)

            self.nonIpDstMacHashTest()

        finally:
            pass

    def tearDown(self):
        attr_list = []
        self.selectNonIpHashField(attr_list)

        print ("Resetting the Non-IP hash object to use all fields")
        self.attribute_set(NonIPHashTest.hash_obj, SWITCH_HASH_ATTR_FIELD_LIST, attr_list)

        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_NON_IP_HASH, 0)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.lag0, SWITCH_LAG_ATTR_PORT_VLAN_ID, 0)
        self.cleanup()

    def nonIpDstMacHashTest(self):
        print("")
        print("Non-IP Hash with DST MAC field consideration ..")

        #Selecting dst_mac parameters
        attr_list = []
        self.selectNonIpHashField(attr_list, SWITCH_HASH_ATTR_FIELD_DST_MAC)

        print ("Setting Non-IP hash object with SRC MAC field")
        self.attribute_set(NonIPHashTest.hash_obj, SWITCH_HASH_ATTR_FIELD_LIST, attr_list)

        lb_count = self.hashTraffic()
        print('NonIP-pkt-count:', lb_count)
        zero_count = 0
        for i in lb_count:
            if i == 0:
                zero_count += 1
        self.assertEqual(zero_count, 3)

    def nonIpSrcMacHashTest(self):
        print("")
        print("Non-IP Hash with SRC MAC field consideration ..")

        #Selecting dst_mac parameters
        attr_list = []
        self.selectNonIpHashField(attr_list, SWITCH_HASH_ATTR_FIELD_SRC_MAC)

        print ("Setting Non-IP hash object with SRC MAC field")
        self.attribute_set(NonIPHashTest.hash_obj, SWITCH_HASH_ATTR_FIELD_LIST, attr_list)

        lb_count = self.hashTraffic()
        print('NonIP-pkt-count:', lb_count)
        for i in range(0, 4):
            self.assertTrue((lb_count[i] >= ((NonIPHashTest.max_itrs / 4) * 0.7)),
                  "Traffic is not equally balanced using Non-IP hash")


    def basicNonIpHashTest(self):
        print("")
        print("Non-IP HASH Test with all the field selected..")

        lb_count = self.hashTraffic()
        print('NonIP-pkt-count:', lb_count)
        for i in range(0, 4):
            self.assertTrue((lb_count[i] >= ((NonIPHashTest.max_itrs / 4) * 0.7)),
                  "Traffic is not equally balanced using Non-IP hash")

    def hashTraffic(self):

        try:
            print('Non-IP hash test with ', NonIPHashTest.max_itrs , ' packets')
            pkt = simple_arp_packet(
                arp_op=2,
                eth_dst='00:22:22:22:22:22',
                eth_src='00:11:11:11:11:11',
                pktlen=100)
            count = [0, 0, 0, 0]
            src_mac_start = '00:11:11:11:11:'
            for i in range(0, NonIPHashTest.max_itrs):
                src_mac = src_mac_start + hex(i)[2:]
                pkt['Ethernet'].src = src_mac
                send_packet(self, self.devports[0], pkt)
                rcv_idx = verify_any_packet_any_port(
                    self, [pkt, pkt, pkt, pkt],
                    [self.devports[1], self.devports[2], self.devports[3], self.devports[4]])
                count[rcv_idx] += 1
            return count

        finally:
            pass

###############################################################################
@group('hash')
class FGLagV6HashTest(ApiHelper, HashHelper):
    '''
    This performs V6 LAG load balancing with Fine Grained config
    @test - Fine-grained V6 LAG hash Test with all hash fields
    '''

    def setUp(self):
        print("")
        self.configure()

        #Configuring LAG
        self.lag0 = self.add_lag(self.device)
        self.lag0_mbr0 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port1)
        self.lag0_mbr1 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port2)
        self.lag0_mbr2 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port3)
        self.lag0_mbr3 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port4)

        #Populating the MAC table
        mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan10,
            mac_address='00:11:11:11:11:11', destination_handle=self.port0)
        mac1 = self.add_mac_entry(self.device, vlan_handle=self.vlan10,
            mac_address='00:22:22:22:22:22', destination_handle=self.lag0)

        #configuring the VLAN
        vlan_mbr0 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port0)
        vlan_mbr1 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.lag0)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.attribute_set(self.lag0, SWITCH_LAG_ATTR_PORT_VLAN_ID, 10)

        #Selecting all the parameters for fine-grained hash
        attr_list = []
        self.createFgLagHashList()

        #creating the hash_obj
        print("")
        print ("Setting the V6 LAG hash object with all fields..")
        self.hash_obj = self.add_hash(self.device, field_list=attr_list,
                                 fine_grained_field_list=self.fg_hash_obj_list)

        #Associating the hash obj to the device
        print("Setting the device object with V6 LAG Hash")
        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_LAG_IPV6_HASH, self.hash_obj)

    def runTest(self):
        try:
            self.setMaxItrs(150)
            self.basicHashTest()
            if (self.client.is_feature_enable(SWITCH_FEATURE_INNER_HASH) == 0):
                print("Dynamic Hashing not yet supported when inner packet hash is used")
                return
            self.configSrcIp()
            self.configDstIp()
            self.configSrcPort()
            self.configDstPort()
            self.configIpProto()
            self.configIpv6FlowLabel()
        finally:
            pass

    def basicHashTest(self):
        print("")
        self.setTrafficParam(src=1)
        count = self.ipv6Traffic()
        print('ipv6-pkt-count:', count)
        self.verify_equaly_balanced(count, log="V6 LAG paths")

    def configIpv6FlowLabel(self):
        #Selecting ipv6_flow_label parameters
        prev_arr_list = self.fg_hash_obj_list
        self.fg_hash_obj_list = []
        attr_list = [SWITCH_HASH_ATTR_FIELD_IPV6_FLOW_LABEL]
        self.createFgLagHashList(field_name_list=attr_list, p_ipv6_label_mask=7)

        print("")
        print ("Setting the V6 LAG hash object with IPV6 Flow Label")
        self.attribute_set(self.hash_obj,
                           SWITCH_HASH_ATTR_FINE_GRAINED_FIELD_LIST,
                           self.fg_hash_obj_list)

        self.setTrafficParam(ipv6_fl=1)
        count = self.ipv6Traffic()
        self.verify_single_port_traffic(count, log="V6 LAG paths")
        print('Count:', count)


    def configSrcIp(self):
        #Selecting src_addr parameters
        prev_arr_list = self.fg_hash_obj_list
        self.fg_hash_obj_list = []
        attr_list = [SWITCH_HASH_ATTR_FIELD_SRC_ADDR]
        self.createFgLagHashList(field_name_list=attr_list)

        print("")
        print ("Setting the V6 LAG hash object with Source IP field")
        self.attribute_set(self.hash_obj,
                           SWITCH_HASH_ATTR_FINE_GRAINED_FIELD_LIST,
                           self.fg_hash_obj_list)
        self.setTrafficParam(src=1)
        count = self.ipv6Traffic()
        print('Count:', count)
        self.verify_equaly_balanced(count, log="V6 LAG paths")

        #Selecting dst_addr parameters
        self.fg_hash_obj_list = []
        attr_list = [SWITCH_HASH_ATTR_FIELD_SRC_ADDR]
        ip_mask='ffff:ffff:ffff:ffff:ffff:ffff:ffff:0000'
        self.createFgLagHashList(field_name_list=attr_list,p_src_ip_mask=ip_mask)

        print("")
        print ("Setting the V6 LAG hash object with Masked Source IP field")
        self.attribute_set(self.hash_obj,
                           SWITCH_HASH_ATTR_FINE_GRAINED_FIELD_LIST,
                           self.fg_hash_obj_list)
        self.setTrafficParam(src=1)
        count = self.ipv6Traffic()
        print('Count:', count)
        self.verify_single_port_traffic(count, log="V6 LAG paths")

    def configDstIp(self):
        #Selecting src_addr parameters
        prev_arr_list = self.fg_hash_obj_list
        self.fg_hash_obj_list = []
        attr_list = [SWITCH_HASH_ATTR_FIELD_DST_ADDR]
        self.createFgLagHashList(field_name_list=attr_list)

        print("")
        print ("Setting the V6 LAG hash object with Destination IP field")
        self.attribute_set(self.hash_obj,
                           SWITCH_HASH_ATTR_FINE_GRAINED_FIELD_LIST,
                           self.fg_hash_obj_list)

        self.setTrafficParam(dst=1)
        count = self.ipv6Traffic()
        self.verify_equaly_balanced(count, log="V6 LAG paths")
        print('Count:', count)

        #Selecting dst_addr parameters
        self.fg_hash_obj_list = []
        attr_list = [SWITCH_HASH_ATTR_FIELD_DST_ADDR]
        ip_mask='ffff:ffff:ffff:ffff:ffff:ffff:ffff:0000'
        self.createFgLagHashList(field_name_list=attr_list,p_dst_ip_mask=ip_mask)

        print("")
        print ("Setting the V6 LAG hash object with Masked Destination IP field")
        self.attribute_set(self.hash_obj,
                           SWITCH_HASH_ATTR_FINE_GRAINED_FIELD_LIST,
                           self.fg_hash_obj_list)
        self.setTrafficParam(dst=1)
        count = self.ipv6Traffic()
        print('Count:', count)
        self.verify_single_port_traffic(count, log="V6 LAG paths")

    def configSrcPort(self):
        #Selecting src_addr parameters
        prev_arr_list = self.fg_hash_obj_list
        self.fg_hash_obj_list = []
        attr_list = [SWITCH_HASH_ATTR_FIELD_SRC_PORT]
        src_port_mask = 12
        self.createFgLagHashList(field_name_list=attr_list, p_src_port_mask=src_port_mask)

        print("")
        print ("Setting the V6 LAG hash object with Source Port field")
        self.attribute_set(self.hash_obj,
                           SWITCH_HASH_ATTR_FINE_GRAINED_FIELD_LIST,
                           self.fg_hash_obj_list)

        self.setTrafficParam(src=1, dst=1)
        count = self.ipv6Traffic()
        print('Count:', count)
        self.verify_single_port_traffic(count, log="V6 LAG paths")

    def configDstPort(self):
        #Selecting src_addr parameters
        prev_arr_list = self.fg_hash_obj_list
        self.fg_hash_obj_list = []
        attr_list = [SWITCH_HASH_ATTR_FIELD_DST_PORT]
        self.createFgLagHashList(field_name_list=attr_list)

        print("")
        print ("Setting the V6 LAG hash object with Destination Port field")
        self.attribute_set(self.hash_obj,
                           SWITCH_HASH_ATTR_FINE_GRAINED_FIELD_LIST,
                           self.fg_hash_obj_list)

        count = self.ipv6Traffic()
        print('Count:', count)
        self.verify_single_port_traffic(count, log="V6 LAG paths")

    def configIpProto(self):
        #Selecting src_addr parameters
        prev_arr_list = self.fg_hash_obj_list
        self.fg_hash_obj_list = []
        attr_list = [SWITCH_HASH_ATTR_FIELD_IP_PROTO]
        self.createFgLagHashList(field_name_list=attr_list)

        print("")
        print ("Setting the V6 LAG hash object with IP Proto field")
        self.attribute_set(self.hash_obj,
                           SWITCH_HASH_ATTR_FINE_GRAINED_FIELD_LIST,
                           self.fg_hash_obj_list)

        self.setTrafficParam(src=1, dst=1)
        count = self.ipv6Traffic()
        print('Count:', count)
        self.verify_single_port_traffic(count, log="V6 LAG paths")

    def tearDown(self):
        fg_hash_obj = []

        print ("Resetting the V6 Lag hash object to use all fields")
        self.attribute_set(self.hash_obj,
          SWITCH_HASH_ATTR_FINE_GRAINED_FIELD_LIST, fg_hash_obj)

        self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.lag0, SWITCH_LAG_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_LAG_IPV6_HASH, 0)
        self.cleanup()


###############################################################################

@group('hash')
class FGLagHashTest(ApiHelper):
    '''
    This performs LAG load balancing with Fine Grained config
    @test - Fine-grained hash Test with all hash fields
    @test - Fine-grianed hash Test with src address field
    @test - Fine-grianed hash Test with masked address field
    '''
    hash_obj = 0
    fg_hash_obj_list = []
    max_itrs  = 120


    def createFineGrainedHashObjList(self,
                              field_name_list=None,
                              p_src_port_mask=0,
                              p_dst_port_mask=0,
                              p_ip_proto_mask=0,
                              p_dst_ip_mask='0.0.0.0',
                              p_src_ip_mask='0.0.0.0'):

        if (field_name_list is None):
            field_name_list = [SWITCH_HASH_ATTR_FIELD_SRC_ADDR,
                         SWITCH_HASH_ATTR_FIELD_DST_ADDR,
                         SWITCH_HASH_ATTR_FIELD_IP_PROTO,
                         SWITCH_HASH_ATTR_FIELD_DST_PORT,
                         SWITCH_HASH_ATTR_FIELD_SRC_PORT]

        #Creating the fine grained hash object list
        index = 1

        for field_attr in field_name_list:
            fg_hash_field_id = 0;
            if (field_attr == SWITCH_HASH_ATTR_FIELD_SRC_ADDR):
                fg_hash_field_id = self.add_fine_grained_hash(self.device,
                                 field_name=field_attr,
                                 src_ip_mask=p_src_ip_mask,
                                 sequence=index)

            if (field_attr == SWITCH_HASH_ATTR_FIELD_DST_ADDR):
                fg_hash_field_id = self.add_fine_grained_hash(self.device,
                                 field_name=field_attr,
                                 dst_ip_mask=p_dst_ip_mask,
                                 sequence=index)

            if (field_attr == SWITCH_HASH_ATTR_FIELD_IP_PROTO):
                fg_hash_field_id = self.add_fine_grained_hash(self.device,
                                 field_name=field_attr,
                                 ip_proto_mask=p_ip_proto_mask,
                                 sequence=index)

            if (field_attr == SWITCH_HASH_ATTR_FIELD_DST_PORT):
                fg_hash_field_id = self.add_fine_grained_hash(self.device,
                                 field_name=field_attr,
                                 dst_port_mask=p_dst_port_mask,
                                 sequence=index)

            if (field_attr == SWITCH_HASH_ATTR_FIELD_SRC_PORT):
                fg_hash_field_id = self.add_fine_grained_hash(self.device,
                                 field_name=field_attr,
                                 src_port_mask=p_src_port_mask,
                                 sequence=index)
            fg_hash_obj_id = switcht_list_val_t(type=switcht_value_type.OBJECT_ID,
                                                            oid=fg_hash_field_id)
            (FGLagHashTest.fg_hash_obj_list).append(fg_hash_obj_id)
            index += 1

    def setUp(self):
        print("")
        self.configure()

        #Configuring LAG
        self.lag0 = self.add_lag(self.device)
        self.lag0_mbr0 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port1)
        self.lag0_mbr1 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port2)
        self.lag0_mbr2 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port3)
        self.lag0_mbr3 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port4)

        #Populating the MAC table
        mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan10,
            mac_address='00:11:11:11:11:11', destination_handle=self.port0)
        mac1 = self.add_mac_entry(self.device, vlan_handle=self.vlan10,
            mac_address='00:22:22:22:22:22', destination_handle=self.lag0)

        #configuring the VLAN
        vlan_mbr0 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port0)
        vlan_mbr1 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.lag0)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.attribute_set(self.lag0, SWITCH_LAG_ATTR_PORT_VLAN_ID, 10)

        #Selecting all the parameters for fine-grained hash
        attr_list = []
        self.createFineGrainedHashObjList()

        #creating the hash_obj
        print ("Setting the LAG hash object with all fields..")
        FGLagHashTest.hash_obj = self.add_hash(self.device, field_list=attr_list,
                                 fine_grained_field_list=FGLagHashTest.fg_hash_obj_list)

        #Associating the hash obj to the device
        print("Setting the device object with LAG Hash")
        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_LAG_IPV4_HASH, FGLagHashTest.hash_obj)

    def basicHashTest (self):
        print("")
        print("Fine-grained LAG HASH Test with selecting all the fields..")
        lb_count = self.hashTrafficVaryingSrcIP()
        print('lag-count:', lb_count)
        for i in range(0, 4):
            self.assertTrue((lb_count[i] >= ((FGLagHashTest.max_itrs / 4) * 0.7)),
                  "Lag paths are not equally balanced")

    def srcIPHashTest(self):
        print("")
        print("Fine-grained hashing with only source IP ")

        #Selecting src_addr parameters
        prev_arr_list = FGLagHashTest.fg_hash_obj_list
        FGLagHashTest.fg_hash_obj_list = []
        attr_list = [SWITCH_HASH_ATTR_FIELD_SRC_ADDR]
        self.createFineGrainedHashObjList(field_name_list=attr_list)

        print ("Setting the LAG hash object with Source IP field")
        self.attribute_set(FGLagHashTest.hash_obj,
                           SWITCH_HASH_ATTR_FINE_GRAINED_FIELD_LIST,
                           FGLagHashTest.fg_hash_obj_list)

        lb_count = self.hashTrafficVaryingSrcIP()
        print('lag-count:', lb_count)
        for i in range(0, 4):
            self.assertTrue((lb_count[i] >= ((FGLagHashTest.max_itrs / 4) * 0.7)),
                  "Lag paths are not equally balanced")

    def dstPortHashTest(self):
        print("")
        print("Fine-grained hashing with only destination port")

        #Selecting src_addr parameters
        prev_arr_list = FGLagHashTest.fg_hash_obj_list
        FGLagHashTest.fg_hash_obj_list = []
        attr_list = [SWITCH_HASH_ATTR_FIELD_DST_PORT]
        self.createFineGrainedHashObjList(field_name_list=attr_list)

        print ("Setting the LAG hash object with Source IP field")
        self.attribute_set(FGLagHashTest.hash_obj,
                           SWITCH_HASH_ATTR_FINE_GRAINED_FIELD_LIST,
                           FGLagHashTest.fg_hash_obj_list)

        lb_count = self.hashTrafficVaryingSrcIP()
        print('lag-count:', lb_count)
        for i in range(0, 4):
            self.assertTrue((lb_count[i] == FGLagHashTest.max_itrs) or (lb_count[i] == 0),
                 "Traffic should be present in one of the port of Lag")

    def srcIPMaskedHashTest(self):
        print("")
        print("Fine-grained hashing with 255.255.255.0 mask source IP ")

        #Selecting src_addr parameters
        prev_arr_list = FGLagHashTest.fg_hash_obj_list
        FGLagHashTest.fg_hash_obj_list = []
        attr_list = [SWITCH_HASH_ATTR_FIELD_SRC_ADDR]
        self.createFineGrainedHashObjList(field_name_list=attr_list, p_src_ip_mask='255.255.255.0')

        print ("Setting the LAG hash object with Source IP field")
        self.attribute_set(FGLagHashTest.hash_obj,
                           SWITCH_HASH_ATTR_FINE_GRAINED_FIELD_LIST,
                           FGLagHashTest.fg_hash_obj_list)

        lb_count = self.hashTrafficVaryingSrcIP()
        print('lag-count:', lb_count)
        for i in range(0, 4):
            self.assertTrue((lb_count[i] == FGLagHashTest.max_itrs) or (lb_count[i] == 0),
                 "Traffic should be present in one of the port of Lag")

        print("")
        print("Fine-grained hashing with 255.255.0.255 mask source IP ")

        #Selecting src_addr parameters
        prev_arr_list = FGLagHashTest.fg_hash_obj_list
        FGLagHashTest.fg_hash_obj_list = []
        attr_list = [SWITCH_HASH_ATTR_FIELD_SRC_ADDR]
        self.createFineGrainedHashObjList(field_name_list=attr_list, p_src_ip_mask='255.255.0.255')

        print ("Setting the LAG hash object with Source IP field")
        self.attribute_set(FGLagHashTest.hash_obj,
                           SWITCH_HASH_ATTR_FINE_GRAINED_FIELD_LIST,
                           FGLagHashTest.fg_hash_obj_list)

        lb_count = self.hashTrafficVaryingSrcIP()
        print('lag-count:', lb_count)
        for i in range(0, 4):
            self.assertTrue((lb_count[i] >= ((FGLagHashTest.max_itrs / 4) * 0.7)),
                  "Lag paths are not equally balanced")

    def hashTrafficVaryingSrcIP (self):
        try:
            print('Lag has test with', FGLagHashTest.max_itrs , 'packets')
            pkt = simple_tcp_packet(
                eth_src='00:11:11:11:11:11',
                eth_dst='00:22:22:22:22:22',
                ip_dst='10.10.10.1',
                ip_src='13.1.1.1',
                ip_id=109,
                ip_ttl=64)

            count = [0, 0, 0, 0]
            src_ip = int(binascii.hexlify(socket.inet_aton('13.1.1.1')), 16)
            for i in range(0, FGLagHashTest.max_itrs):
                #With new source IP
                src_ip_addr = socket.inet_ntoa(
                     binascii.unhexlify(format(src_ip, 'x').zfill(8)))
                pkt['IP'].src = src_ip_addr

                send_packet(self, self.devports[0], pkt)
                rcv_idx = verify_any_packet_any_port(
                     self, [pkt, pkt, pkt, pkt],
                     [self.devports[1], self.devports[2], self.devports[3], self.devports[4]])
                count[rcv_idx] += 1
                src_ip += 1
            return count
        finally:
            pass

    def runTest(self):
        try:
            self.basicHashTest()
            if (self.client.is_feature_enable(SWITCH_FEATURE_INNER_HASH) == 0):
                print("Dynamic Hashing not yet supported when inner packet hash is used")
                return
            self.srcIPHashTest()
            self.srcIPMaskedHashTest()
            self.dstPortHashTest()
        finally:
            pass

    def tearDown(self):
        FGLagHashTest.fg_hash_obj_list = []

        print ("Resetting the Lag hash object to use all fields")
        self.attribute_set(FGLagHashTest.hash_obj,
          SWITCH_HASH_ATTR_FINE_GRAINED_FIELD_LIST,
                    FGLagHashTest.fg_hash_obj_list)

        self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.lag0, SWITCH_LAG_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_LAG_IPV4_HASH, 0)
        self.cleanup()


###############################################################################

@group('hash')
class FGEcmpIPV4Hash(ApiHelper):
    '''
    This performs ECMP load balancing
    @test - ECMP IPv4 Hash Test with selecting all fields
    @test - ECMP IPv4 Hash Test with selcting SRC address field
    '''
    hash_obj = 0
    fg_hash_obj_list = []
    max_itrs  = 120

    def createFineGrainedHashObjList(self,
                              field_name_list=None,
                              p_src_port_mask=0,
                              p_dst_port_mask=0,
                              p_ip_proto_mask=0,
                              p_dst_ip_mask='0.0.0.0',
                              p_src_ip_mask='0.0.0.0'):

        if (field_name_list is None):
            field_name_list = [SWITCH_HASH_ATTR_FIELD_SRC_ADDR,
                         SWITCH_HASH_ATTR_FIELD_DST_ADDR,
                         SWITCH_HASH_ATTR_FIELD_IP_PROTO,
                         SWITCH_HASH_ATTR_FIELD_DST_PORT,
                         SWITCH_HASH_ATTR_FIELD_SRC_PORT]

        #Creating the fine grained hash object list
        index = 1

        for field_attr in field_name_list:
            fg_hash_field_id = 0;
            if (field_attr == SWITCH_HASH_ATTR_FIELD_SRC_ADDR):
                fg_hash_field_id = self.add_fine_grained_hash(self.device,
                                 field_name=field_attr,
                                 src_ip_mask=p_src_ip_mask,
                                 sequence=index)

            if (field_attr == SWITCH_HASH_ATTR_FIELD_DST_ADDR):
                fg_hash_field_id = self.add_fine_grained_hash(self.device,
                                 field_name=field_attr,
                                 dst_ip_mask=p_dst_ip_mask,
                                 sequence=index)

            if (field_attr == SWITCH_HASH_ATTR_FIELD_IP_PROTO):
                fg_hash_field_id = self.add_fine_grained_hash(self.device,
                                 field_name=field_attr,
                                 ip_proto_mask=p_ip_proto_mask,
                                 sequence=index)

            if (field_attr == SWITCH_HASH_ATTR_FIELD_DST_PORT):
                fg_hash_field_id = self.add_fine_grained_hash(self.device,
                                 field_name=field_attr,
                                 dst_port_mask=p_dst_port_mask,
                                 sequence=index)

            if (field_attr == SWITCH_HASH_ATTR_FIELD_SRC_PORT):
                fg_hash_field_id = self.add_fine_grained_hash(self.device,
                                 field_name=field_attr,
                                 src_port_mask=p_src_port_mask,
                                 sequence=index)
            fg_hash_obj_id = switcht_list_val_t(type=switcht_value_type.OBJECT_ID,
                                                            oid=fg_hash_field_id)
            (FGEcmpIPV4Hash.fg_hash_obj_list).append(fg_hash_obj_id)
            index += 1

    def setUp(self):
        print("")
        self.configure()
        self.ecmp = self.add_ecmp(self.device)
        self.rif0 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT,
                                port_handle=self.port0, vrf_handle=self.vrf10,
                                src_mac=self.rmac)
        #IPV4 Route
        self.route4 = self.add_route(self.device, ip_prefix='13.1.1.0/24',
                           vrf_handle=self.vrf10, nexthop_handle=self.ecmp)

        #Selecting all the parameters
        attr_list = []
        self.createFineGrainedHashObjList()

        #creating the hash_obj
        print ("Setting the ECMP IPV4 hash object with all fields..")
        FGEcmpIPV4Hash.hash_obj = self.add_hash(self.device, field_list=attr_list,
                                 fine_grained_field_list=FGEcmpIPV4Hash.fg_hash_obj_list)

        #Associating the hash obj to the device
        print("Setting the device object with IPV4 Hash")
        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_ECMP_IPV4_HASH, FGEcmpIPV4Hash.hash_obj)

        for l_port in [self.port1, self.port2, self.port3, self.port4]:
            l_rif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, \
                                 port_handle=l_port, vrf_handle=self.vrf10, \
                                 src_mac=self.rmac)
            l_ip = '13.0.0.' + str(l_port & 256)
            l_mac = '00:22:22:33:44:' + str(l_port & 0xFF)
            l_nhop = self.add_nexthop(self.device, handle=l_rif, dest_ip=l_ip)
            l_neighbor = self.add_neighbor(self.device, mac_address=l_mac, handle=l_rif, dest_ip=l_ip)
            ecmp_member = self.add_ecmp_member(self.device, nexthop_handle=l_nhop, ecmp_handle=self.ecmp)

    def runTest(self):
        try:
            self.BasicHashTest()
            if (self.client.is_feature_enable(SWITCH_FEATURE_INNER_HASH) == 0):
                print("Dynamic Hashing not yet supported when inner packet hash is used")
                return
            self.SrcAddrHashTest()
            self.DstAddrHashTest()
            self.MaskedDstAddrHashTest()

        finally:
            pass

    def tearDown(self):
        fg_hash_element_obj_list = []

        print ("Resetting the ECMP IPV4 hash object to use all fields")
        self.attribute_set(FGEcmpIPV4Hash.hash_obj,
          SWITCH_HASH_ATTR_FINE_GRAINED_FIELD_LIST,
                           fg_hash_element_obj_list)

        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_ECMP_IPV4_HASH, 0)
        self.cleanup()

    def BasicHashTest(self):
        print("")
        print("Fine-grained IPv4 hashing with all the fields..")

        lb_count = self.hashTraffic()
        print('ecmp-ipv4-pkt-count:', lb_count)
        for i in range(0, 4):
            self.assertTrue((lb_count[i] >= ((FGEcmpIPV4Hash.max_itrs / 4) * 0.7)),
                  "Ecmp paths are not equally balanced")

    def SrcAddrHashTest(self):
        print("")
        print("Fine-grained IPv4 hashing with Source IP field..")

        #Selecting src_addr parameters
        FGEcmpIPV4Hash.fg_hash_obj_list = []
        attr_list = [SWITCH_HASH_ATTR_FIELD_SRC_ADDR]
        self.createFineGrainedHashObjList(field_name_list=attr_list)

        print ("Setting the IPv4 hash object with src_addr field..")
        self.attribute_set(FGEcmpIPV4Hash.hash_obj,
               SWITCH_HASH_ATTR_FINE_GRAINED_FIELD_LIST,
               FGEcmpIPV4Hash.fg_hash_obj_list)

        count = self.hashFixedSrcTraffic()
        print('ecmp-ipv4-pkt-count:', count)
        for i in range(0, 4):
            self.assertTrue((count[i] == FGEcmpIPV4Hash.max_itrs) or (count[i] == 0),
                 "Traffic should use only of the port")

    def DstAddrHashTest(self):
        print("")
        print("Fine-grained IPV4 hashing with Dest IP field..")

        #Selecting dst_addr parameters
        FGEcmpIPV4Hash.fg_hash_obj_list = []
        attr_list = [SWITCH_HASH_ATTR_FIELD_DST_ADDR]
        self.createFineGrainedHashObjList(field_name_list=attr_list)

        print ("Setting the IPv4 hash object with dst_addr field..")
        self.attribute_set(FGEcmpIPV4Hash.hash_obj,
               SWITCH_HASH_ATTR_FINE_GRAINED_FIELD_LIST,
               FGEcmpIPV4Hash.fg_hash_obj_list)

        count = self.hashFixedSrcTraffic()
        print('ecmp-ipv4-pkt-count:', count)
        for i in range(0, 4):
            self.assertTrue((count[i] >= ((FGEcmpIPV4Hash.max_itrs / 4) * 0.7)),
                  "Ecmp paths are not equally balanced")

    def MaskedDstAddrHashTest(self):
        print("")
        print("Fine-grained IPV4 hashing with Dest IP field..")

        #Selecting dst_addr parameters
        FGEcmpIPV4Hash.fg_hash_obj_list = []
        attr_list = [SWITCH_HASH_ATTR_FIELD_DST_ADDR]
        self.createFineGrainedHashObjList(field_name_list=attr_list, p_dst_ip_mask='255.255.255.0')

        print ("Setting the IPv4 hash object with dst_addr field..")
        self.attribute_set(FGEcmpIPV4Hash.hash_obj,
               SWITCH_HASH_ATTR_FINE_GRAINED_FIELD_LIST,
               FGEcmpIPV4Hash.fg_hash_obj_list)

        count = self.hashFixedSrcTraffic()
        print('ecmp-ipv4-pkt-count:', count)
        for i in range(0, 4):
            self.assertTrue((count[i] == FGEcmpIPV4Hash.max_itrs) or (count[i] == 0),
                  "Traffic should only use one parth in the ECMP")

    def hashFixedSrcTraffic(self):

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
            print('IPv4 hash test with', EcmpIPV4HashTest.max_itrs, 'packets')
            count = [0, 0, 0, 0]
            dst_ip = int(binascii.hexlify(socket.inet_aton('13.1.1.1')), 16)
            for i in range(0, EcmpIPV4HashTest.max_itrs):
                dst_ip_addr = socket.inet_ntoa(
                    binascii.unhexlify(format(dst_ip, 'x').zfill(8)))
                pkt['IP'].dst = dst_ip_addr
                exp_pkt1['IP'].dst = dst_ip_addr
                exp_pkt2['IP'].dst = dst_ip_addr
                exp_pkt3['IP'].dst = dst_ip_addr
                exp_pkt4['IP'].dst = dst_ip_addr

                send_packet(self, self.devports[0], pkt)
                rcv_idx = verify_any_packet_any_port(
                    self, [exp_pkt1, exp_pkt2, exp_pkt3, exp_pkt4], [
                    self.devports[1], self.devports[2], self.devports[3], self.devports[4]
                    ], timeout=2)
                count[rcv_idx] += 1
                dst_ip += 1
            return count

        finally:
            pass

    def hashTraffic(self):

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
            print('IPv4 hash test with', EcmpIPV4HashTest.max_itrs, 'packets')
            count = [0, 0, 0, 0]
            dst_ip = int(binascii.hexlify(socket.inet_aton('13.1.1.1')), 16)
            src_ip = int(binascii.hexlify(socket.inet_aton('192.168.8.1')), 16)
            for i in range(0, EcmpIPV4HashTest.max_itrs):
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

        finally:
            pass

###############################################################################

@group('hash')
class FGEcmpIPV6Hash(ApiHelper):
    '''
    This performs ECMP IPv6 load balancing
    @test - ECMP IPv6 hash test with selecting all fields
    @test - ECMP IPv6 hash test with selecting Source IP field
    '''
    max_itrs = 120
    fg_hash_obj_list = []
    hash_obj = 0
    def createFineGrainedHashObjList(self,
                              field_name_list=None,
                              p_src_port_mask=0,
                              p_dst_port_mask=0,
                              p_ip_proto_mask=0,
                              p_dst_ip_mask='0.0.0.0',
                              p_src_ip_mask='0.0.0.0'):

        if (field_name_list is None):
            field_name_list = [SWITCH_HASH_ATTR_FIELD_SRC_ADDR,
                         SWITCH_HASH_ATTR_FIELD_DST_ADDR,
                         SWITCH_HASH_ATTR_FIELD_IP_PROTO,
                         SWITCH_HASH_ATTR_FIELD_DST_PORT,
                         SWITCH_HASH_ATTR_FIELD_SRC_PORT]

        #Creating the fine grained hash object list
        index = 1

        for field_attr in field_name_list:
            fg_hash_field_id = 0;
            if (field_attr == SWITCH_HASH_ATTR_FIELD_SRC_ADDR):
                fg_hash_field_id = self.add_fine_grained_hash(self.device,
                                 field_name=field_attr,
                                 src_ip_mask=p_src_ip_mask,
                                 sequence=index)

            if (field_attr == SWITCH_HASH_ATTR_FIELD_DST_ADDR):
                fg_hash_field_id = self.add_fine_grained_hash(self.device,
                                 field_name=field_attr,
                                 dst_ip_mask=p_dst_ip_mask,
                                 sequence=index)

            if (field_attr == SWITCH_HASH_ATTR_FIELD_IP_PROTO):
                fg_hash_field_id = self.add_fine_grained_hash(self.device,
                                 field_name=field_attr,
                                 ip_proto_mask=p_ip_proto_mask,
                                 sequence=index)

            if (field_attr == SWITCH_HASH_ATTR_FIELD_DST_PORT):
                fg_hash_field_id = self.add_fine_grained_hash(self.device,
                                 field_name=field_attr,
                                 dst_port_mask=p_dst_port_mask,
                                 sequence=index)

            if (field_attr == SWITCH_HASH_ATTR_FIELD_SRC_PORT):
                fg_hash_field_id = self.add_fine_grained_hash(self.device,
                                 field_name=field_attr,
                                 src_port_mask=p_src_port_mask,
                                 sequence=index)
            fg_hash_obj_id = switcht_list_val_t(type=switcht_value_type.OBJECT_ID,
                                                            oid=fg_hash_field_id)
            (FGEcmpIPV6Hash.fg_hash_obj_list).append(fg_hash_obj_id)
            index += 1

    def setUp(self):
        print("")
        self.configure()
        self.ecmp = self.add_ecmp(self.device)
        self.rif0 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT,
                                 port_handle=self.port0, vrf_handle=self.vrf10,
                                 src_mac=self.rmac)
        #IPv6 Route
        self.route6 = self.add_route(self.device,
                       ip_prefix='4000:5678:9abc:def0:4422:1133:0000:0000/96',
                       vrf_handle=self.vrf10, nexthop_handle=self.ecmp)

        #Selecting all the parameters
        attr_list = []
        self.createFineGrainedHashObjList()

        #creating the hash_obj
        print ("Setting the ECMP IPV6 hash object with all fields..")
        FGEcmpIPV6Hash.hash_obj = self.add_hash(self.device, field_list=attr_list,
                                 fine_grained_field_list=FGEcmpIPV6Hash.fg_hash_obj_list)

        #Associating the hash obj to the device
        print("Setting the device object with IPV6 Hash")
        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_ECMP_IPV6_HASH, FGEcmpIPV6Hash.hash_obj)

        for l_port in [self.port1, self.port2, self.port3, self.port4]:
            l_rif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, \
                                 port_handle=l_port, vrf_handle=self.vrf10, \
                                 src_mac=self.rmac)
            l_ip = '13.0.0.' + str(l_port & 256)
            l_mac = '00:22:22:33:44:' + str(l_port & 0xFF)
            l_nhop = self.add_nexthop(self.device, handle=l_rif, dest_ip=l_ip)
            l_neighbor = self.add_neighbor(self.device, mac_address=l_mac, handle=l_rif, dest_ip=l_ip)
            ecmp_member = self.add_ecmp_member(self.device, nexthop_handle=l_nhop, ecmp_handle=self.ecmp)

    def runTest(self):
        try:
            self.ipv6BasicHashTest()
        finally:
            pass

    def tearDown(self):
        fg_hash_element_obj_list = []

        print ("Resetting the ECMP IPV4 hash object to use all fields")
        self.attribute_set(FGEcmpIPV6Hash.hash_obj,
          SWITCH_HASH_ATTR_FINE_GRAINED_FIELD_LIST,
                          fg_hash_element_obj_list)

        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_ECMP_IPV6_HASH, 0)
        self.cleanup()

    def ipv6BasicHashTest(self):

        print("")
        print("IPv6 Hash Test with all the fields..")

        lb_count = self.hashTraffic()
        print('ecmp-ipv6-pkt-count:', lb_count)
        for i in range(0, 4):
            self.assertTrue((lb_count[i] >= ((FGEcmpIPV6Hash.max_itrs / 4) * 0.7)),
                 "Ecmp paths are not equally balanced")

        if (self.client.is_feature_enable(SWITCH_FEATURE_INNER_HASH) == 0):
            print("Dynamic Hashing not yet supported when inner packet hash is used")
        return
        print("")
        print("IPv6 Hashing using destination ip addr.")

        #Selecting dst_addr parameters
        FGEcmpIPV6Hash.fg_hash_obj_list = []
        attr_list = [SWITCH_HASH_ATTR_FIELD_DST_ADDR]
        self.createFineGrainedHashObjList(field_name_list=attr_list)

        print ("Setting the IPv6 hash object with dst_addr field..")
        self.attribute_set(FGEcmpIPV6Hash.hash_obj,
               SWITCH_HASH_ATTR_FINE_GRAINED_FIELD_LIST,
               FGEcmpIPV6Hash.fg_hash_obj_list)

        lb_count = self.hashTraffic()
        print('ecmp-ipv6-pkt-count:', lb_count)
        for i in range(0, 4):
            self.assertTrue((lb_count[i] >= ((FGEcmpIPV6Hash.max_itrs / 4) * 0.7)),
                 "Ecmp paths are not equally balanced")

        print("")
        print("IPv6 Hashing using masked destination ip addr.")

        #Selecting dst_addr parameters
        FGEcmpIPV6Hash.fg_hash_obj_list = []
        attr_list = [SWITCH_HASH_ATTR_FIELD_DST_ADDR]
        ip_mask='ffff:ffff:ffff:ffff:ffff:ffff:fff7:0000'
        self.createFineGrainedHashObjList(field_name_list=attr_list,p_dst_ip_mask=ip_mask)

        print ("Setting the IPv6 hash object with dst_addr field..")
        self.attribute_set(FGEcmpIPV6Hash.hash_obj,
               SWITCH_HASH_ATTR_FINE_GRAINED_FIELD_LIST,
               FGEcmpIPV6Hash.fg_hash_obj_list)

        lb_count = self.hashTraffic()
        print('ecmp-ipv6-pkt-count:', lb_count)
        for i in range(0, 4):
            self.assertTrue((lb_count[i] == FGEcmpIPV4Hash.max_itrs) or (lb_count[i] == 0),
                 "Traffic should use only of the port")

    def hashTraffic(self):

        pkt = simple_tcpv6_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ipv6_dst='4000:5678:9abc:def0:4422:1133:5577:1111',
            ipv6_src='2000:5678:9abc:def0:4422:1133:5577:1111',
            ipv6_hlim=64)
        exp_pkt1 = simple_tcpv6_packet(
            eth_dst='00:22:22:33:44:' + str(self.port1 & 0xFF),
            eth_src='00:77:66:55:44:33',
            ipv6_dst='4000:5678:9abc:def0:4422:1133:5577:1111',
            ipv6_src='2000:5678:9abc:def0:4422:1133:5577:1111',
            ipv6_hlim=63)
        exp_pkt2 = simple_tcpv6_packet(
            eth_dst='00:22:22:33:44:' + str(self.port2 & 0xFF),
            eth_src='00:77:66:55:44:33',
            ipv6_dst='4000:5678:9abc:def0:4422:1133:5577:1111',
            ipv6_src='2000:5678:9abc:def0:4422:1133:5577:1111',
            ipv6_hlim=63)
        exp_pkt3 = simple_tcpv6_packet(
            eth_dst='00:22:22:33:44:' + str(self.port3 & 0xFF),
            eth_src='00:77:66:55:44:33',
            ipv6_dst='4000:5678:9abc:def0:4422:1133:5577:1111',
            ipv6_src='2000:5678:9abc:def0:4422:1133:5577:1111',
            ipv6_hlim=63)
        exp_pkt4 = simple_tcpv6_packet(
            eth_dst='00:22:22:33:44:' + str(self.port4 & 0xFF),
            eth_src='00:77:66:55:44:33',
            ipv6_dst='4000:5678:9abc:def0:4422:1133:5577:1111',
            ipv6_src='2000:5678:9abc:def0:4422:1133:5577:1111',
            ipv6_hlim=63)
        try:
            print('IPv6 hash test with', FGEcmpIPV6Hash.max_itrs, 'packets')
            count = [0, 0, 0, 0]
            dst_ip = int(binascii.hexlify(socket.inet_pton(socket.AF_INET6, '4000:5678:9abc:def0:4422:1133:5577:1111')), 16)
            src_ip = int(binascii.hexlify(socket.inet_pton(socket.AF_INET6, '2000:5678:9abc:def0:4422:1133:5577:1111')), 16)
            for i in range(0, EcmpIPV6HashTest.max_itrs):
                dst_ip_addr = socket.inet_ntop(socket.AF_INET6,
                   binascii.unhexlify(format(dst_ip, 'x').zfill(8)))
                src_ip_addr = socket.inet_ntop(socket.AF_INET6,
                   binascii.unhexlify(format(src_ip, 'x').zfill(8)))
                pkt['IPv6'].dst = dst_ip_addr
                pkt['IPv6'].src = src_ip_addr
                exp_pkt1['IPv6'].dst = dst_ip_addr
                exp_pkt1['IPv6'].src = src_ip_addr
                exp_pkt2['IPv6'].dst = dst_ip_addr
                exp_pkt2['IPv6'].src = src_ip_addr
                exp_pkt3['IPv6'].dst = dst_ip_addr
                exp_pkt3['IPv6'].src = src_ip_addr
                exp_pkt4['IPv6'].dst = dst_ip_addr
                exp_pkt4['IPv6'].src = src_ip_addr

                send_packet(self, self.devports[0], pkt)
                rcv_idx = verify_any_packet_any_port(
                     self, [exp_pkt1, exp_pkt2, exp_pkt3, exp_pkt4], [
                     self.devports[1], self.devports[2], self.devports[3], self.devports[4]
                     ], timeout=2)
                count[rcv_idx] += 1
                dst_ip += 1
                src_ip += 1
            return count
        finally:
            pass

###############################################################################

@group('hash')
class LagHashTest(ApiHelper):
    '''
    This performs LAG load balancing
    @test - LAG Hash Test with selecting all fields
    @test - LAG Hash Test with selecting SRC MAC field
    '''

    hash_obj = 0
    max_itrs  = 120

    def selectLagHashField(self, attr_list, attr='all'):

        if(attr == 'all' or attr == SWITCH_HASH_ATTR_FIELD_SRC_ADDR):
            attr_list.append(switcht_list_val_t(type=switcht_value_type.UINT32,
                                        u32data=SWITCH_HASH_ATTR_FIELD_SRC_ADDR))

        if(attr == 'all' or attr == SWITCH_HASH_ATTR_FIELD_DST_ADDR):
            attr_list.append(switcht_list_val_t(type=switcht_value_type.UINT32,
                                        u32data=SWITCH_HASH_ATTR_FIELD_DST_ADDR))

        if(attr == 'all' or attr == SWITCH_HASH_ATTR_FIELD_IP_PROTO):
            attr_list.append(switcht_list_val_t(type=switcht_value_type.UINT32,
                                        u32data=SWITCH_HASH_ATTR_FIELD_IP_PROTO))

        if(attr == 'all' or attr == SWITCH_HASH_ATTR_FIELD_DST_PORT):
            attr_list.append(switcht_list_val_t(type=switcht_value_type.UINT32,
                                        u32data=SWITCH_HASH_ATTR_FIELD_DST_PORT))

        if(attr == 'all' or attr == SWITCH_HASH_ATTR_FIELD_SRC_PORT):
            attr_list.append(switcht_list_val_t(type=switcht_value_type.UINT32,
                                        u32data=SWITCH_HASH_ATTR_FIELD_SRC_PORT))

    def setUp(self):
        print("")
        self.configure()

        #Configuring LAG
        self.lag0 = self.add_lag(self.device)
        self.lag0_mbr0 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port1)
        self.lag0_mbr1 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port2)
        self.lag0_mbr2 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port3)
        self.lag0_mbr3 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port4)

        #Populating the MAC table
        mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan10,
            mac_address='00:11:11:11:11:11', destination_handle=self.port0)
        mac1 = self.add_mac_entry(self.device, vlan_handle=self.vlan10,
            mac_address='00:22:22:22:22:22', destination_handle=self.lag0)

        #configuring the VLAN
        vlan_mbr0 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port0)
        vlan_mbr1 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.lag0)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.attribute_set(self.lag0, SWITCH_LAG_ATTR_PORT_VLAN_ID, 10)

        #Selecting all the parameters
        attr_list = []
        fg_attr_list = []
        self.selectLagHashField(attr_list)
        print ('attr_list: ', attr_list)

        #creating the hash_obj
        print ("Setting the LAG hash object with all fields..")
        LagHashTest.hash_obj = self.add_hash(self.device, field_list=attr_list,
                                           fine_grained_field_list=fg_attr_list)

        #Associating the hash obj to the device
        print("Setting the device object with LAG Hash")
        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_LAG_IPV4_HASH, LagHashTest.hash_obj)

    def basicLagHashTest (self):
        print("")
        print("LAG HASH Test with selecting all the fields..")
        lb_count = self.hashTrafficVaryingSrcIP()
        print('lag-count:', lb_count)
        for i in range(0, 4):
            self.assertTrue((lb_count[i] >= ((LagHashTest.max_itrs / 4) * 0.7)),
                  "Lag paths are not equally balanced")

    def srcIPLagHashTest(self):
        print("")
        print("LAG HASH Test with fixed Src MAC and varying Source IP")

        #Selecting src_addr parameters
        attr_list = []
        self.selectLagHashField(attr_list, SWITCH_HASH_ATTR_FIELD_SRC_ADDR)

        print ("Setting the LAG hash object with Source IP field")
        self.attribute_set(LagHashTest.hash_obj, SWITCH_HASH_ATTR_FIELD_LIST, attr_list)

        lb_count = self.hashTrafficVaryingSrcIP()
        print('lag-count:', lb_count)
        for i in range(0, 4):
            self.assertTrue((lb_count[i] >= ((LagHashTest.max_itrs / 4) * 0.7)),
                  "Lag paths are not equally balanced")

    def dstIPLagHashTest(self):
        print("")
        print("LAG HASH Test with fixed Src MAC and varying Dst IP")

        #Selecting src_addr parameters
        attr_list = []
        self.selectLagHashField(attr_list, SWITCH_HASH_ATTR_FIELD_DST_ADDR)

        print ("Setting the LAG hash object with Source IP field")
        self.attribute_set(LagHashTest.hash_obj, SWITCH_HASH_ATTR_FIELD_LIST, attr_list)

        lb_count = self.hashTrafficVaryingSrcIP()
        print('lag-count:', lb_count)
        for i in range(0, 4):
            self.assertTrue((lb_count[i] == LagHashTest.max_itrs) or (lb_count[i] == 0),
                 "Traffic should be present in one of the port of Lag")

    def hashTrafficVaryingSrcIP (self):
        try:
            print('Lag has test with', LagHashTest.max_itrs , 'packets')
            pkt = simple_tcp_packet(
                eth_src='00:11:11:11:11:11',
                eth_dst='00:22:22:22:22:22',
                ip_dst='10.10.10.1',
                ip_src='13.1.1.1',
                ip_id=109,
                ip_ttl=64)

            count = [0, 0, 0, 0]
            src_ip = int(binascii.hexlify(socket.inet_aton('13.1.1.1')), 16)
            for i in range(0, LagHashTest.max_itrs):
                #With new source IP
                src_ip_addr = socket.inet_ntoa(
                     binascii.unhexlify(format(src_ip, 'x').zfill(8)))
                pkt['IP'].src = src_ip_addr

                send_packet(self, self.devports[0], pkt)
                rcv_idx = verify_any_packet_any_port(
                     self, [pkt, pkt, pkt, pkt],
                     [self.devports[1], self.devports[2], self.devports[3], self.devports[4]])
                count[rcv_idx] += 1
                src_ip += 1
            return count
        finally:
            pass

    def runTest(self):
        try:
            self.basicLagHashTest()
            if self.client.is_feature_enable(SWITCH_FEATURE_INNER_HASH):
                print("Dynamic Hashing not yet supported when inner packet hash is used")
                return
            self.srcIPLagHashTest()
            self.dstIPLagHashTest()
        finally:
            pass

    def tearDown(self):
        attr_list = []
        self.selectLagHashField(attr_list)

        print ("Resetting the Lag hash object to use all fields")
        self.attribute_set(LagHashTest.hash_obj, SWITCH_HASH_ATTR_FIELD_LIST, attr_list)

        self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.lag0, SWITCH_LAG_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_LAG_IPV4_HASH, 0)
        self.cleanup()

###############################################################################

@group('hash')
class EcmpIPV4HashTest(ApiHelper):
    '''
    This performs ECMP load balancing
    @test - ECMP IPv4 Hash Test with selecting all fields
    @test - ECMP IPv4 Hash Test with selcting SRC address field
    '''
    hash_obj = 0
    max_itrs = 120

    def selectIpHashField(self, attr_list, attr='all'):
        if(attr == 'all' or attr == SWITCH_HASH_ATTR_FIELD_SRC_ADDR):
            attr_list.append(switcht_list_val_t(type=switcht_value_type.UINT32,
                                       u32data=SWITCH_HASH_ATTR_FIELD_SRC_ADDR))

        if(attr == 'all' or attr == SWITCH_HASH_ATTR_FIELD_DST_ADDR):
            attr_list.append(switcht_list_val_t(type=switcht_value_type.UINT32,
                                        u32data=SWITCH_HASH_ATTR_FIELD_DST_ADDR))

        if(attr == 'all' or attr == SWITCH_HASH_ATTR_FIELD_IP_PROTO):
            attr_list.append(switcht_list_val_t(type=switcht_value_type.UINT32,
                                        u32data=SWITCH_HASH_ATTR_FIELD_IP_PROTO))

        if(attr == 'all' or attr == SWITCH_HASH_ATTR_FIELD_DST_PORT):
            attr_list.append(switcht_list_val_t(type=switcht_value_type.UINT32,
                                         u32data=SWITCH_HASH_ATTR_FIELD_DST_PORT))

        if(attr == 'all' or attr == SWITCH_HASH_ATTR_FIELD_SRC_PORT):
            attr_list.append(switcht_list_val_t(type=switcht_value_type.UINT32,
                                        u32data=SWITCH_HASH_ATTR_FIELD_SRC_PORT))

    def setUp(self):
        print("")
        self.configure()
        self.ecmp = self.add_ecmp(self.device)
        self.rif0 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT,
                                 port_handle=self.port0, vrf_handle=self.vrf10,
                                 src_mac=self.rmac)
        #IPV4 Route
        self.route4 = self.add_route(self.device, ip_prefix='13.1.1.0/16',
                                     vrf_handle=self.vrf10, nexthop_handle=self.ecmp)

        #Selecting all the parameters
        attr_list = []
        fg_attr_list = []
        self.selectIpHashField(attr_list)

        print ("Setting the IPv4 hash object with all fields..")
        EcmpIPV4HashTest.hash_obj = self.add_hash(self.device, field_list=attr_list,
                                                fine_grained_field_list=fg_attr_list)

        #Associating the hash obj to the device
        print("Setting the device object with IPv4 Hash")
        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_ECMP_IPV4_HASH, EcmpIPV4HashTest.hash_obj)

        for l_port in [self.port1, self.port2, self.port3, self.port4]:
            l_rif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, \
                                 port_handle=l_port, vrf_handle=self.vrf10, \
                                 src_mac=self.rmac)
            l_ip = '13.0.0.' + str(l_port & 256)
            l_mac = '00:22:22:33:44:' + str(l_port & 0xFF)
            l_nhop = self.add_nexthop(self.device, handle=l_rif, dest_ip=l_ip)
            l_neighbor = self.add_neighbor(self.device, mac_address=l_mac, handle=l_rif, dest_ip=l_ip)
            ecmp_member = self.add_ecmp_member(self.device, nexthop_handle=l_nhop, ecmp_handle=self.ecmp)

    def runTest(self):
        try:
            self.ipv4BasicHashTest()

            if self.client.is_feature_enable(SWITCH_FEATURE_INNER_HASH):
                print("Dynamic Hashing not yet supported when inner packet hash is used")
                return
            print("Waiting for 5 Secs..")
            time.sleep(5)

            self.ipv4SrcAddrHashTest()

            print("Waiting for 5 Secs..")
            time.sleep(5)

            self.ipv4DstAddrHashTest()

        finally:
            pass

    def tearDown(self):
        attr_list = []
        self.selectIpHashField(attr_list)

        print ("Resetting IPV4 hash object to use all fields")
        self.attribute_set(EcmpIPV4HashTest.hash_obj, SWITCH_HASH_ATTR_FIELD_LIST, attr_list)

        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_ECMP_IPV4_HASH, 0)
        self.cleanup()

    def ipv4SrcAddrHashTest(self):
        print("")
        print("IPv4 Hash with selecting only Source IP field..")

        #Selecting src_addr parameters
        attr_list = []
        self.selectIpHashField(attr_list, SWITCH_HASH_ATTR_FIELD_SRC_ADDR)

        print ("Setting the IPv4 hash object with src_addr field..")
        self.attribute_set(EcmpIPV4HashTest.hash_obj, SWITCH_HASH_ATTR_FIELD_LIST, attr_list)

        count = self.hashFixedSrcTraffic()
        print('ecmp-ipv4-pkt-count:', count)
        for i in range(0, 4):
            self.assertTrue((count[i] == EcmpIPV4HashTest.max_itrs) or (count[i] == 0),
                 "Traffic should use only of the port inside LAG")

    def ipv4DstAddrHashTest(self):
        print("")
        print("IPv4 Hash with selecting only Dest IP field..")

        #Selecting src_addr parameters
        attr_list = []
        self.selectIpHashField(attr_list, SWITCH_HASH_ATTR_FIELD_DST_ADDR)

        print ("Setting the IPv4 hash object with dst_addr field..")
        self.attribute_set(EcmpIPV4HashTest.hash_obj, SWITCH_HASH_ATTR_FIELD_LIST, attr_list)

        count = self.hashFixedSrcTraffic()
        print('ecmp-ipv4-pkt-count:', count)
        for i in range(0, 4):
            self.assertTrue((count[i] >= ((EcmpIPV4HashTest.max_itrs / 4) * 0.7)),
                  "Ecmp paths are not equally balanced")

    def ipv4BasicHashTest(self):
        print("")
        print("IPv4 Hash Test with selecting all the fields..")

        lb_count = self.hashTraffic()
        print('ecmp-ipv4-pkt-count:', lb_count)
        for i in range(0, 4):
            self.assertTrue((lb_count[i] >= ((EcmpIPV4HashTest.max_itrs / 4) * 0.7)),
                  "Ecmp paths are not equally balanced")

    def hashFixedSrcTraffic(self):

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
            print('IPv4 hash test with', EcmpIPV4HashTest.max_itrs, 'packets')
            count = [0, 0, 0, 0]
            dst_ip = int(binascii.hexlify(socket.inet_aton('13.1.1.1')), 16)
            for i in range(0, EcmpIPV4HashTest.max_itrs):
                dst_ip_addr = socket.inet_ntoa(
                    binascii.unhexlify(format(dst_ip, 'x').zfill(8)))
                pkt['IP'].dst = dst_ip_addr
                exp_pkt1['IP'].dst = dst_ip_addr
                exp_pkt2['IP'].dst = dst_ip_addr
                exp_pkt3['IP'].dst = dst_ip_addr
                exp_pkt4['IP'].dst = dst_ip_addr

                send_packet(self, self.devports[0], pkt)
                rcv_idx = verify_any_packet_any_port(
                    self, [exp_pkt1, exp_pkt2, exp_pkt3, exp_pkt4], [
                    self.devports[1], self.devports[2], self.devports[3], self.devports[4]
                    ], timeout=2)
                count[rcv_idx] += 1
                dst_ip += 100
            return count

        finally:
            pass

    def hashTraffic(self):

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
            print('IPv4 hash test with', EcmpIPV4HashTest.max_itrs, 'packets')
            count = [0, 0, 0, 0]
            dst_ip = int(binascii.hexlify(socket.inet_aton('13.1.1.1')), 16)
            src_ip = int(binascii.hexlify(socket.inet_aton('192.168.8.1')), 16)
            for i in range(0, EcmpIPV4HashTest.max_itrs):
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

        finally:
            pass



@group('hash')
class EcmpIPV4SymmetricHashTest(ApiHelper):
    '''
    This performs ECMP IPv4 symmetric hashing
    @test - ECMP IPv4 Symmetric Hash Test with selecting all fields
    '''
    hash_obj = 0
    max_itrs = 120
    ip_dst = '13.1.1.1'
    ip_src = '192.168.0.1'
    sport = 1234
    dport = 4321
    hash_flag = 0;

    def selectIpHashField(self, attr_list, attr='all'):
        if(attr == 'all' or attr == SWITCH_HASH_ATTR_FIELD_SRC_ADDR):
            attr_list.append(switcht_list_val_t(type=switcht_value_type.UINT32,
                                       u32data=SWITCH_HASH_ATTR_FIELD_SRC_ADDR))

        if(attr == 'all' or attr == SWITCH_HASH_ATTR_FIELD_DST_ADDR):
            attr_list.append(switcht_list_val_t(type=switcht_value_type.UINT32,
                                        u32data=SWITCH_HASH_ATTR_FIELD_DST_ADDR))

        if(attr == 'all' or attr == SWITCH_HASH_ATTR_FIELD_IP_PROTO):
            attr_list.append(switcht_list_val_t(type=switcht_value_type.UINT32,
                                        u32data=SWITCH_HASH_ATTR_FIELD_IP_PROTO))

        if(attr == 'all' or attr == SWITCH_HASH_ATTR_FIELD_DST_PORT):
            attr_list.append(switcht_list_val_t(type=switcht_value_type.UINT32,
                                         u32data=SWITCH_HASH_ATTR_FIELD_DST_PORT))

        if(attr == 'all' or attr == SWITCH_HASH_ATTR_FIELD_SRC_PORT):
            attr_list.append(switcht_list_val_t(type=switcht_value_type.UINT32,
                                        u32data=SWITCH_HASH_ATTR_FIELD_SRC_PORT))

    def setUp(self):
        print("")
        self.configure()
        self.ecmp = self.add_ecmp(self.device)
        self.rif0 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT,
                                 port_handle=self.port0, vrf_handle=self.vrf10,
                                 src_mac=self.rmac)
        #IPV4 Route
        self.route4 = self.add_route(self.device, ip_prefix='13.1.1.0/24',
                                     vrf_handle=self.vrf10, nexthop_handle=self.ecmp)
        self.route4 = self.add_route(self.device, ip_prefix='192.168.0.0/24',
                                     vrf_handle=self.vrf10, nexthop_handle=self.ecmp)

        #Selecting all the parameters
        attr_list = []
        fg_attr_list = []
        self.selectIpHashField(attr_list)

        print ("Setting the IPv4 hash object with all fields and symmetric hash")
        EcmpIPV4SymmetricHashTest.hash_obj = self.add_hash(self.device, field_list=attr_list, symmetric_hash=1)

        #Associating the hash obj to the device
        print("Setting the device object with IPv4 Hash")
        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_ECMP_IPV4_HASH, EcmpIPV4SymmetricHashTest.hash_obj)

        for l_port in [self.port1, self.port2, self.port3, self.port4]:
            l_rif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, \
                                 port_handle=l_port, vrf_handle=self.vrf10, \
                                 src_mac=self.rmac)
            l_ip = '11.0.0.' + str(l_port & 256)
            l_mac = '00:22:22:33:44:' + str(l_port & 0xFF)
            l_nhop = self.add_nexthop(self.device, handle=l_rif, dest_ip=l_ip)
            l_neighbor = self.add_neighbor(self.device, mac_address=l_mac, handle=l_rif, dest_ip=l_ip)
            ecmp_member = self.add_ecmp_member(self.device, nexthop_handle=l_nhop, ecmp_handle=self.ecmp)

    def runTest(self):
        try:
            self.ipv4BasicHashTest()

        finally:
            pass

    def tearDown(self):
        attr_list = []
        self.selectIpHashField(attr_list)

        print ("Resetting IPV4 hash object to use all fields")
        self.attribute_set(EcmpIPV4SymmetricHashTest.hash_obj, SWITCH_HASH_ATTR_FIELD_LIST, attr_list)

        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_ECMP_IPV4_HASH, 0)
        self.cleanup()

    def ipv4BasicHashTest(self):
        print("")
        print("IPv4 Hash Test with selecting all the fields..")

        lb_count1 = self.hashTraffic(l3=True, l4=True)
        print('ecmp-ipv4-pkt-count:', lb_count1)

        # flip params
        self.ip_src = '13.1.1.1'
        self.ip_dst = '192.168.0.1'
        self.sport = 4321
        self.dport = 1234
        self.hash_flag = True

        lb_count2 = self.hashTraffic(l3=True, l4=True)
        print('ecmp-ipv4-pkt-count:', lb_count2)

        print ("Verified symmetricity for IP and L4 fields")
        self.assertTrue(verify_lb_changed(lb_count1, lb_count2) == 0,
               "lb_count1 & lb_count2 are not equal, not symmetrically hashed")

        # reset parameters
        self.ip_src = '192.168.0.1'
        self.ip_dst = '13.1.1.1'
        self.sport = 1234
        self.dport = 4321
        self.hash_flag = False

        lb_count1 = self.hashTraffic(l3=True)
        print('ecmp-ipv4-pkt-count:', lb_count1)

        # flip params
        self.ip_dst = '192.168.0.1'
        self.ip_src = '13.1.1.1'
        self.hash_flag = True

        lb_count2 = self.hashTraffic(l3=True)
        print('ecmp-ipv4-pkt-count:', lb_count2)

        print ("Verified symmetricity for L3 fields only")
        self.assertTrue(verify_lb_changed(lb_count1, lb_count2) == 0,
               "lb_count1 & lb_count2 are not equal, not symmetrically hashed")

        # reset parameters
        self.ip_src = '192.168.0.1'
        self.ip_dst = '13.1.1.1'
        self.sport = 1234
        self.dport = 4321
        self.hash_flag = False

        lb_count1 = self.hashTraffic(l4=True)
        print('ecmp-ipv4-pkt-count:', lb_count1)

        # flip params
        self.sport = 4321
        self.dport = 1234

        lb_count2 = self.hashTraffic(l4=True)
        print('ecmp-ipv4-pkt-count:', lb_count2)

        print ("Verified symmetricity for L4 fields only")
        self.assertTrue(verify_lb_changed(lb_count1, lb_count2) == 0,
               "lb_count1 & lb_count2 are not equal, not symmetrically hashed")

    def hashTraffic(self, l3=False, l4=False):
        pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst=self.ip_dst,
            ip_src=self.ip_src,
            tcp_sport=self.sport,
            tcp_dport=self.dport,
            ip_id=106,
            ip_ttl=64)
        exp_pkt1 = simple_tcp_packet(
            eth_dst='00:22:22:33:44:' + str(self.port1 & 0xFF),
            eth_src='00:77:66:55:44:33',
            ip_dst=self.ip_dst,
            ip_src=self.ip_src,
            tcp_sport=self.sport,
            tcp_dport=self.dport,
            ip_id=106,
            ip_ttl=63)
        exp_pkt2 = simple_tcp_packet(
            eth_dst='00:22:22:33:44:' + str(self.port2 & 0xFF),
            eth_src='00:77:66:55:44:33',
            ip_dst=self.ip_dst,
            ip_src=self.ip_src,
            tcp_sport=self.sport,
            tcp_dport=self.dport,
            ip_id=106,
            ip_ttl=63)
        exp_pkt3 = simple_tcp_packet(
            eth_dst='00:22:22:33:44:' + str(self.port3 & 0xFF),
            eth_src='00:77:66:55:44:33',
            ip_dst=self.ip_dst,
            ip_src=self.ip_src,
            tcp_sport=self.sport,
            tcp_dport=self.dport,
            ip_id=106,
            ip_ttl=63)
        exp_pkt4 = simple_tcp_packet(
            eth_dst='00:22:22:33:44:' + str(self.port4 & 0xFF),
            eth_src='00:77:66:55:44:33',
            ip_dst=self.ip_dst,
            ip_src=self.ip_src,
            tcp_sport=self.sport,
            tcp_dport=self.dport,
            ip_id=106,
            ip_ttl=63)
        try:
            print('IPv4 hash test with', EcmpIPV4SymmetricHashTest.max_itrs, 'packets')
            count = [0, 0, 0, 0]
            dst_ip = int(binascii.hexlify(socket.inet_aton(self.ip_dst)), 16)
            src_ip = int(binascii.hexlify(socket.inet_aton(self.ip_src)), 16)
            sport = self.sport
            dport = self.dport
            for i in range(0, EcmpIPV4SymmetricHashTest.max_itrs):
                dst_ip_addr = socket.inet_ntoa(
                    binascii.unhexlify(format(dst_ip, 'x').zfill(8)))
                src_ip_addr = socket.inet_ntoa(
                    binascii.unhexlify(format(src_ip, 'x').zfill(8)))
                pkt['IP'].dst = dst_ip_addr
                pkt['IP'].src = src_ip_addr
                pkt['TCP'].src = sport
                pkt['TCP'].dst = dport
                exp_pkt1['IP'].dst = dst_ip_addr
                exp_pkt1['IP'].src = src_ip_addr
                exp_pkt1['TCP'].src = sport
                exp_pkt1['TCP'].dst = dport
                exp_pkt2['IP'].dst = dst_ip_addr
                exp_pkt2['IP'].src = src_ip_addr
                exp_pkt2['TCP'].src = sport
                exp_pkt2['TCP'].dst = dport
                exp_pkt3['IP'].dst = dst_ip_addr
                exp_pkt3['IP'].src = src_ip_addr
                exp_pkt3['TCP'].src = sport
                exp_pkt3['TCP'].dst = dport
                exp_pkt4['IP'].dst = dst_ip_addr
                exp_pkt4['IP'].src = src_ip_addr
                exp_pkt4['TCP'].src = sport
                exp_pkt4['TCP'].dst = dport

                send_packet(self, self.devports[0], pkt)
                rcv_idx = verify_any_packet_any_port(
                    self, [exp_pkt1, exp_pkt2, exp_pkt3, exp_pkt4], [
                    self.devports[1], self.devports[2], self.devports[3], self.devports[4]
                    ], timeout=2)
                count[rcv_idx] += 1
                if(l3):
                    if self.hash_flag:
                        dst_ip += 1
                    else:
                        src_ip += 1
                if(l4):
                    dport += 1
                    sport += 1
            return count

        finally:
            pass

@group('hash')
class EcmpIPV6SymmetricHashTest(ApiHelper):
    '''
    This performs ECMP IPv6 symmetric hashing
    @test - ECMP IPv6 symmetric hash test with selecting all fields
    '''
    max_itrs = 120
    hash_obj = 0
    ipv6_dst='4000:5678:9abc:def0:4422:1133:5577:1111'
    ipv6_src='2000:5678:9abc:def0:4422:1133:5577:1111'
    hash_flag = 0;

    def selectIpHashField(self, attr_list, attr='all'):
        if(attr == 'all' or attr == SWITCH_HASH_ATTR_FIELD_SRC_ADDR):
            attr_list.append(switcht_list_val_t(type=switcht_value_type.UINT32,
                                       u32data=SWITCH_HASH_ATTR_FIELD_SRC_ADDR))

        if(attr == 'all' or attr == SWITCH_HASH_ATTR_FIELD_DST_ADDR):
            attr_list.append(switcht_list_val_t(type=switcht_value_type.UINT32,
                                        u32data=SWITCH_HASH_ATTR_FIELD_DST_ADDR))

        if(attr == 'all' or attr == SWITCH_HASH_ATTR_FIELD_IP_PROTO):
            attr_list.append(switcht_list_val_t(type=switcht_value_type.UINT32,
                                        u32data=SWITCH_HASH_ATTR_FIELD_IP_PROTO))

        if(attr == 'all' or attr == SWITCH_HASH_ATTR_FIELD_DST_PORT):
            attr_list.append(switcht_list_val_t(type=switcht_value_type.UINT32,
                                         u32data=SWITCH_HASH_ATTR_FIELD_DST_PORT))

        if(attr == 'all' or attr == SWITCH_HASH_ATTR_FIELD_SRC_PORT):
            attr_list.append(switcht_list_val_t(type=switcht_value_type.UINT32,
                                        u32data=SWITCH_HASH_ATTR_FIELD_SRC_PORT))

    def setUp(self):
        print("")
        self.configure()
        self.ecmp = self.add_ecmp(self.device)
        self.rif0 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT,
                                 port_handle=self.port0, vrf_handle=self.vrf10,
                                 src_mac=self.rmac)
        #IPv6 Route
        self.route6 = self.add_route(self.device,
                       ip_prefix='4000:5678:9abc:def0:4422:1133:0000:0000/96',
                       vrf_handle=self.vrf10, nexthop_handle=self.ecmp)

        #Selecting all the parameters
        attr_list = []
        fg_attr_list = []
        self.selectIpHashField(attr_list)

        print ("Setting the IPv6 hash object with all attributes..")
        EcmpIPV6SymmetricHashTest.hash_obj = self.add_hash(self.device, field_list=attr_list,
                                                symmetric_hash=1)

        #Associating the hash obj to the device
        print("Setting the device object with IPV6 HASH")
        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_ECMP_IPV6_HASH, EcmpIPV6SymmetricHashTest.hash_obj)

        for l_port in [self.port1, self.port2, self.port3, self.port4]:
            l_rif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, \
                                 port_handle=l_port, vrf_handle=self.vrf10, \
                                 src_mac=self.rmac)
            l_ip = '13.0.0.' + str(l_port & 256)
            l_mac = '00:22:22:33:44:' + str(l_port & 0xFF)
            l_nhop = self.add_nexthop(self.device, handle=l_rif, dest_ip=l_ip)
            l_neighbor = self.add_neighbor(self.device, mac_address=l_mac, handle=l_rif, dest_ip=l_ip)
            ecmp_member = self.add_ecmp_member(self.device, nexthop_handle=l_nhop, ecmp_handle=self.ecmp)

    def runTest(self):
        try:
            self.ipv6BasicHashTest()
        finally:
            pass

    def tearDown(self):
        attr_list = []
        self.selectIpHashField(attr_list)

        print ("Resetting IPV6 hash object to use all fields")
        self.attribute_set(EcmpIPV6SymmetricHashTest.hash_obj, SWITCH_HASH_ATTR_FIELD_LIST, attr_list)

        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_ECMP_IPV6_HASH, 0)
        self.cleanup()

    def ipv6BasicHashTest(self):

        print("")
        print("IPv6 Hash Test with all the fields..")

        lb_count1 = self.hashTraffic()
        print('ecmp-ipv6-pkt-count:', lb_count1)

        self.route6 = self.add_route(self.device,
                       ip_prefix='2000:5678:9abc:def0:4422:1133:0000:0000/96',
                       vrf_handle=self.vrf10, nexthop_handle=self.ecmp)

        self.ipv6_src='4000:5678:9abc:def0:4422:1133:5577:1111'
        self.ipv6_dst='2000:5678:9abc:def0:4422:1133:5577:1111'
        self.hash_flag = 1

        lb_count2 = self.hashTraffic()
        print('ecmp-ipv6-pkt-count:', lb_count2)

        self.assertTrue(verify_lb_changed(lb_count1, lb_count2) == 0,
               "lb_count1 & lb_count2 are not equal, not symmentrically hashed")

    def hashTraffic(self):

        pkt = simple_tcpv6_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ipv6_dst=self.ipv6_dst,
            ipv6_src=self.ipv6_src,
            ipv6_hlim=64)
        exp_pkt1 = simple_tcpv6_packet(
            eth_dst='00:22:22:33:44:' + str(self.port1 & 0xFF),
            eth_src='00:77:66:55:44:33',
            ipv6_dst=self.ipv6_dst,
            ipv6_src=self.ipv6_src,
            ipv6_hlim=63)
        exp_pkt2 = simple_tcpv6_packet(
            eth_dst='00:22:22:33:44:' + str(self.port2 & 0xFF),
            eth_src='00:77:66:55:44:33',
            ipv6_dst=self.ipv6_dst,
            ipv6_src=self.ipv6_src,
            ipv6_hlim=63)
        exp_pkt3 = simple_tcpv6_packet(
            eth_dst='00:22:22:33:44:' + str(self.port3 & 0xFF),
            eth_src='00:77:66:55:44:33',
            ipv6_dst=self.ipv6_dst,
            ipv6_src=self.ipv6_src,
            ipv6_hlim=63)
        exp_pkt4 = simple_tcpv6_packet(
            eth_dst='00:22:22:33:44:' + str(self.port4 & 0xFF),
            eth_src='00:77:66:55:44:33',
            ipv6_dst=self.ipv6_dst,
            ipv6_src=self.ipv6_src,
            ipv6_hlim=63)
        try:
            print('IPv6 hash test with', EcmpIPV6SymmetricHashTest.max_itrs, 'packets')
            count = [0, 0, 0, 0]
            dst_ip = int(binascii.hexlify(socket.inet_pton(socket.AF_INET6, self.ipv6_dst)), 16)
            src_ip = int(binascii.hexlify(socket.inet_pton(socket.AF_INET6, self.ipv6_src)), 16)
            for i in range(0, EcmpIPV6SymmetricHashTest.max_itrs):
                dst_ip_addr = socket.inet_ntop(socket.AF_INET6,
                   binascii.unhexlify(format(dst_ip, 'x').zfill(8)))
                src_ip_addr = socket.inet_ntop(socket.AF_INET6,
                   binascii.unhexlify(format(src_ip, 'x').zfill(8)))
                pkt['IPv6'].dst = dst_ip_addr
                pkt['IPv6'].src = src_ip_addr
                exp_pkt1['IPv6'].dst = dst_ip_addr
                exp_pkt1['IPv6'].src = src_ip_addr
                exp_pkt2['IPv6'].dst = dst_ip_addr
                exp_pkt2['IPv6'].src = src_ip_addr
                exp_pkt3['IPv6'].dst = dst_ip_addr
                exp_pkt3['IPv6'].src = src_ip_addr
                exp_pkt4['IPv6'].dst = dst_ip_addr
                exp_pkt4['IPv6'].src = src_ip_addr

                send_packet(self, self.devports[0], pkt)
                rcv_idx = verify_any_packet_any_port(
                     self, [exp_pkt1, exp_pkt2, exp_pkt3, exp_pkt4], [
                     self.devports[1], self.devports[2], self.devports[3], self.devports[4]
                     ], timeout=2)
                count[rcv_idx] += 1
                if(self.hash_flag == 0):
                    dst_ip += 1
                else:
                    src_ip += 1
            return count
        finally:
            pass
###############################################################################

@group('hash')
class EcmpIPV6HashTest(ApiHelper):
    '''
    This performs ECMP IPv6 load balancing
    @test - ECMP IPv6 hash test with selecting all fields
    @test - ECMP IPv6 hash test with selecting Source IP field
    '''
    max_itrs = 120
    hash_obj = 0

    def selectIpHashField(self, attr_list, attr='all'):
        if(attr == 'all' or attr == SWITCH_HASH_ATTR_FIELD_SRC_ADDR):
            attr_list.append(switcht_list_val_t(type=switcht_value_type.UINT32,
                                       u32data=SWITCH_HASH_ATTR_FIELD_SRC_ADDR))

        if(attr == 'all' or attr == SWITCH_HASH_ATTR_FIELD_DST_ADDR):
            attr_list.append(switcht_list_val_t(type=switcht_value_type.UINT32,
                                        u32data=SWITCH_HASH_ATTR_FIELD_DST_ADDR))

        if(attr == 'all' or attr == SWITCH_HASH_ATTR_FIELD_IP_PROTO):
            attr_list.append(switcht_list_val_t(type=switcht_value_type.UINT32,
                                        u32data=SWITCH_HASH_ATTR_FIELD_IP_PROTO))

        if(attr == 'all' or attr == SWITCH_HASH_ATTR_FIELD_DST_PORT):
            attr_list.append(switcht_list_val_t(type=switcht_value_type.UINT32,
                                         u32data=SWITCH_HASH_ATTR_FIELD_DST_PORT))

        if(attr == 'all' or attr == SWITCH_HASH_ATTR_FIELD_SRC_PORT):
            attr_list.append(switcht_list_val_t(type=switcht_value_type.UINT32,
                                        u32data=SWITCH_HASH_ATTR_FIELD_SRC_PORT))

    def setUp(self):
        print("")
        self.configure()
        self.ecmp = self.add_ecmp(self.device)
        self.rif0 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT,
                                 port_handle=self.port0, vrf_handle=self.vrf10,
                                 src_mac=self.rmac)
        #IPv6 Route
        self.route6 = self.add_route(self.device,
                       ip_prefix='4000:5678:9abc:def0:4422:1133:0000:0000/96',
                       vrf_handle=self.vrf10, nexthop_handle=self.ecmp)

        #Selecting all the parameters
        attr_list = []
        fg_attr_list = []
        self.selectIpHashField(attr_list)

        print ("Setting the IPv6 hash object with all attributes..")
        EcmpIPV6HashTest.hash_obj = self.add_hash(self.device, field_list=attr_list,
                                                fine_grained_field_list=fg_attr_list)

        #Associating the hash obj to the device
        print("Setting the device object with IPV6 HASH")
        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_ECMP_IPV6_HASH, EcmpIPV6HashTest.hash_obj)

        for l_port in [self.port1, self.port2, self.port3, self.port4]:
            l_rif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, \
                                 port_handle=l_port, vrf_handle=self.vrf10, \
                                 src_mac=self.rmac)
            l_ip = '13.0.0.' + str(l_port & 256)
            l_mac = '00:22:22:33:44:' + str(l_port & 0xFF)
            l_nhop = self.add_nexthop(self.device, handle=l_rif, dest_ip=l_ip)
            l_neighbor = self.add_neighbor(self.device, mac_address=l_mac, handle=l_rif, dest_ip=l_ip)
            ecmp_member = self.add_ecmp_member(self.device, nexthop_handle=l_nhop, ecmp_handle=self.ecmp)

    def runTest(self):
        try:
            self.ipv6BasicHashTest()

            if self.client.is_feature_enable(SWITCH_FEATURE_INNER_HASH):
                print("Dynamic Hashing not yet supported when inner packet hash is used")
                return
            print("Waiting for 5 Secs..")
            time.sleep(5)

            self.ipv6SrcAddrHashTest()
        finally:
            pass

    def tearDown(self):
        attr_list = []
        self.selectIpHashField(attr_list)

        print ("Resetting IPV6 hash object to use all fields")
        self.attribute_set(EcmpIPV6HashTest.hash_obj, SWITCH_HASH_ATTR_FIELD_LIST, attr_list)

        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_ECMP_IPV6_HASH, 0)
        self.cleanup()

    def ipv6SrcAddrHashTest(self):
        print("")
        print("IPv6 Hash with Source IP field..")

        #Selecting src_addr parameters
        attr_list = []
        self.selectIpHashField(attr_list, SWITCH_HASH_ATTR_FIELD_SRC_ADDR)

        print ("Setting the IPv6 hash object with src address attributes..")
        self.attribute_set(EcmpIPV6HashTest.hash_obj, SWITCH_HASH_ATTR_FIELD_LIST, attr_list)

        lb_count = self.hashFixedSrcTraffic()
        print('ecmp-ipv6-pkt-count:', lb_count)
        for i in range(0, 4):
            self.assertTrue((lb_count[i] == EcmpIPV6HashTest.max_itrs) or (lb_count[i] == 0),
                 "Traffic should use only of the port")

    def ipv6BasicHashTest(self):

        print("")
        print("IPv6 Hash Test with all the fields..")

        lb_count = self.hashTraffic()
        print('ecmp-ipv6-pkt-count:', lb_count)
        for i in range(0, 4):
            self.assertTrue((lb_count[i] >= ((EcmpIPV6HashTest.max_itrs / 4) * 0.7)),
                 "Ecmp paths are not equally balanced")

    def hashFixedSrcTraffic(self):

        pkt = simple_tcpv6_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ipv6_dst='4000:5678:9abc:def0:4422:1133:5577:1111',
            ipv6_src='2000:5678:9abc:def0:4422:1133:5577:1111',
            ipv6_hlim=64)
        exp_pkt1 = simple_tcpv6_packet(
            eth_dst='00:22:22:33:44:' + str(self.port1 & 0xFF),
            eth_src='00:77:66:55:44:33',
            ipv6_dst='4000:5678:9abc:def0:4422:1133:5577:1111',
            ipv6_src='2000:5678:9abc:def0:4422:1133:5577:1111',
            ipv6_hlim=63)
        exp_pkt2 = simple_tcpv6_packet(
            eth_dst='00:22:22:33:44:' + str(self.port2 & 0xFF),
            eth_src='00:77:66:55:44:33',
            ipv6_dst='4000:5678:9abc:def0:4422:1133:5577:1111',
            ipv6_src='2000:5678:9abc:def0:4422:1133:5577:1111',
            ipv6_hlim=63)
        exp_pkt3 = simple_tcpv6_packet(
            eth_dst='00:22:22:33:44:' + str(self.port3 & 0xFF),
            eth_src='00:77:66:55:44:33',
            ipv6_dst='4000:5678:9abc:def0:4422:1133:5577:1111',
            ipv6_src='2000:5678:9abc:def0:4422:1133:5577:1111',
            ipv6_hlim=63)
        exp_pkt4 = simple_tcpv6_packet(
            eth_dst='00:22:22:33:44:' + str(self.port4 & 0xFF),
            eth_src='00:77:66:55:44:33',
            ipv6_dst='4000:5678:9abc:def0:4422:1133:5577:1111',
            ipv6_src='2000:5678:9abc:def0:4422:1133:5577:1111',
            ipv6_hlim=63)
        try:
            print('IPv6 hash test with', EcmpIPV6HashTest.max_itrs, 'packets')
            count = [0, 0, 0, 0]
            dst_ip = int(binascii.hexlify(socket.inet_pton(socket.AF_INET6, '4000:5678:9abc:def0:4422:1133:5577:1111')), 16)
            for i in range(0, EcmpIPV6HashTest.max_itrs):
                dst_ip_addr = socket.inet_ntop(socket.AF_INET6,
                   binascii.unhexlify(format(dst_ip, 'x').zfill(8)))
                pkt['IPv6'].dst = dst_ip_addr
                exp_pkt1['IPv6'].dst = dst_ip_addr
                exp_pkt2['IPv6'].dst = dst_ip_addr
                exp_pkt3['IPv6'].dst = dst_ip_addr
                exp_pkt4['IPv6'].dst = dst_ip_addr

                send_packet(self, self.devports[0], pkt)
                rcv_idx = verify_any_packet_any_port(
                     self, [exp_pkt1, exp_pkt2, exp_pkt3, exp_pkt4], [
                     self.devports[1], self.devports[2], self.devports[3], self.devports[4]
                     ], timeout=2)
                count[rcv_idx] += 1
                dst_ip += 1
            return count
        finally:
            pass

    def hashTraffic(self):

        pkt = simple_tcpv6_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ipv6_dst='4000:5678:9abc:def0:4422:1133:5577:1111',
            ipv6_src='2000:5678:9abc:def0:4422:1133:5577:1111',
            ipv6_hlim=64)
        exp_pkt1 = simple_tcpv6_packet(
            eth_dst='00:22:22:33:44:' + str(self.port1 & 0xFF),
            eth_src='00:77:66:55:44:33',
            ipv6_dst='4000:5678:9abc:def0:4422:1133:5577:1111',
            ipv6_src='2000:5678:9abc:def0:4422:1133:5577:1111',
            ipv6_hlim=63)
        exp_pkt2 = simple_tcpv6_packet(
            eth_dst='00:22:22:33:44:' + str(self.port2 & 0xFF),
            eth_src='00:77:66:55:44:33',
            ipv6_dst='4000:5678:9abc:def0:4422:1133:5577:1111',
            ipv6_src='2000:5678:9abc:def0:4422:1133:5577:1111',
            ipv6_hlim=63)
        exp_pkt3 = simple_tcpv6_packet(
            eth_dst='00:22:22:33:44:' + str(self.port3 & 0xFF),
            eth_src='00:77:66:55:44:33',
            ipv6_dst='4000:5678:9abc:def0:4422:1133:5577:1111',
            ipv6_src='2000:5678:9abc:def0:4422:1133:5577:1111',
            ipv6_hlim=63)
        exp_pkt4 = simple_tcpv6_packet(
            eth_dst='00:22:22:33:44:' + str(self.port4 & 0xFF),
            eth_src='00:77:66:55:44:33',
            ipv6_dst='4000:5678:9abc:def0:4422:1133:5577:1111',
            ipv6_src='2000:5678:9abc:def0:4422:1133:5577:1111',
            ipv6_hlim=63)
        try:
            print('IPv6 hash test with', EcmpIPV6HashTest.max_itrs, 'packets')
            count = [0, 0, 0, 0]
            dst_ip = int(binascii.hexlify(socket.inet_pton(socket.AF_INET6, '4000:5678:9abc:def0:4422:1133:5577:1111')), 16)
            src_ip = int(binascii.hexlify(socket.inet_pton(socket.AF_INET6, '2000:5678:9abc:def0:4422:1133:5577:1111')), 16)
            for i in range(0, EcmpIPV6HashTest.max_itrs):
                dst_ip_addr = socket.inet_ntop(socket.AF_INET6,
                   binascii.unhexlify(format(dst_ip, 'x').zfill(8)))
                src_ip_addr = socket.inet_ntop(socket.AF_INET6,
                   binascii.unhexlify(format(src_ip, 'x').zfill(8)))
                pkt['IPv6'].dst = dst_ip_addr
                pkt['IPv6'].src = src_ip_addr
                exp_pkt1['IPv6'].dst = dst_ip_addr
                exp_pkt1['IPv6'].src = src_ip_addr
                exp_pkt2['IPv6'].dst = dst_ip_addr
                exp_pkt2['IPv6'].src = src_ip_addr
                exp_pkt3['IPv6'].dst = dst_ip_addr
                exp_pkt3['IPv6'].src = src_ip_addr
                exp_pkt4['IPv6'].dst = dst_ip_addr
                exp_pkt4['IPv6'].src = src_ip_addr

                send_packet(self, self.devports[0], pkt)
                rcv_idx = verify_any_packet_any_port(
                     self, [exp_pkt1, exp_pkt2, exp_pkt3, exp_pkt4], [
                     self.devports[1], self.devports[2], self.devports[3], self.devports[4]
                     ], timeout=2)
                count[rcv_idx] += 1
                dst_ip += 1
                src_ip += 1
            return count
        finally:
            pass

###############################################################################

@group('hash')
class HashConfigTest(ApiHelper):
    '''
    Hash config test class for reboot test
    @test - ECMP IPv4 Hash Test with only Source IP field
    @test - ECMP IPv6 Hash Test with Dst IP field
    '''
    ipv4_hash_obj = 0
    ipv6_hash_obj = 0
    max_itrs = 160

    def selectIpHashField(self, attr_list, attr='all'):
        if(attr == 'all' or attr == SWITCH_HASH_ATTR_FIELD_SRC_ADDR):
            attr_list.append(switcht_list_val_t(type=switcht_value_type.UINT32,
                                       u32data=SWITCH_HASH_ATTR_FIELD_SRC_ADDR))

        if(attr == 'all' or attr == SWITCH_HASH_ATTR_FIELD_DST_ADDR):
            attr_list.append(switcht_list_val_t(type=switcht_value_type.UINT32,
                                        u32data=SWITCH_HASH_ATTR_FIELD_DST_ADDR))

        if(attr == 'all' or attr == SWITCH_HASH_ATTR_FIELD_IP_PROTO):
            attr_list.append(switcht_list_val_t(type=switcht_value_type.UINT32,
                                        u32data=SWITCH_HASH_ATTR_FIELD_IP_PROTO))

        if(attr == 'all' or attr == SWITCH_HASH_ATTR_FIELD_DST_PORT):
            attr_list.append(switcht_list_val_t(type=switcht_value_type.UINT32,
                                         u32data=SWITCH_HASH_ATTR_FIELD_DST_PORT))

        if(attr == 'all' or attr == SWITCH_HASH_ATTR_FIELD_SRC_PORT):
            attr_list.append(switcht_list_val_t(type=switcht_value_type.UINT32,
                                        u32data=SWITCH_HASH_ATTR_FIELD_SRC_PORT))

    def setUp(self):
        print("")
        self.configure()
        self.ecmp = self.add_ecmp(self.device)
        self.rif0 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT,
                                 port_handle=self.port0, vrf_handle=self.vrf10,
                                 src_mac=self.rmac)
        #IPV4 Route
        self.route4 = self.add_route(self.device, ip_prefix='13.1.0.0/16',
                                     vrf_handle=self.vrf10, nexthop_handle=self.ecmp)

        #IPv6 Route
        self.route6 = self.add_route(self.device,
               ip_prefix='4000:5678:9abc:def0:4422:1133:0000:0000/96',
               vrf_handle=self.vrf10, nexthop_handle=self.ecmp)

        #Selecting IPV4 hash parameter
        ipv4_attr_list = []
        fg_attr_list = []
        self.selectIpHashField(ipv4_attr_list, SWITCH_HASH_ATTR_FIELD_SRC_ADDR)

        print ("Setting the IPv4 hash object with source IP field..")
        HashConfigTest.ipv4_hash_obj = self.add_hash(self.device, field_list=ipv4_attr_list,
                                                        fine_grained_field_list=fg_attr_list)

        #Associating the hash obj to the device
        print("Setting the device object with IPv4 Hash")
        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_ECMP_IPV4_HASH,
                           HashConfigTest.ipv4_hash_obj)

        #Selecting IPV6 hash parameter
        ipv6_attr_list = []
        self.selectIpHashField(ipv6_attr_list, SWITCH_HASH_ATTR_FIELD_DST_ADDR)

        print ("Setting the IPv6 hash object with Dest IP field..")
        HashConfigTest.ipv6_hash_obj = self.add_hash(self.device, field_list=ipv6_attr_list,
                                                        fine_grained_field_list=fg_attr_list)

        #Associating the hash obj to the device
        print("Setting the device object with IPv6 Hash")
        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_ECMP_IPV6_HASH,
                           HashConfigTest.ipv6_hash_obj)

        for l_port in [self.port1, self.port2, self.port3, self.port4]:
            l_rif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, \
                                 port_handle=l_port, vrf_handle=self.vrf10, \
                                 src_mac=self.rmac)
            l_ip = '13.0.0.' + str(l_port & 256)
            l_mac = '00:22:22:33:44:' + str(l_port & 0xFF)
            l_nhop = self.add_nexthop(self.device, handle=l_rif, dest_ip=l_ip)
            l_neighbor = self.add_neighbor(self.device, mac_address=l_mac, handle=l_rif, dest_ip=l_ip)
            ecmp_member = self.add_ecmp_member(self.device, nexthop_handle=l_nhop, ecmp_handle=self.ecmp)

    def runTest(self):
        try:
            #Basic Traffic Test
            if self.client.is_feature_enable(SWITCH_FEATURE_INNER_HASH):
                print("Dynamic Hashing not yet supported when inner packet hash is used")
                return
            self.ipv4BasicHashTest()

            print("Waiting for 5 Secs..")
            time.sleep(5)

            #Traffic test with fixed source IP
            self.ipv4SrcIPHashTest()

            print("Waiting for 5 Secs..")
            time.sleep(5)

            #Basic Traffic Test
            self.ipv6BasicHashTest()

            print("Waiting for 5 Secs..")
            time.sleep(5)

            #Traffic test with fixed dest IP
            self.ipv6DstIPHashTest()

        finally:
            pass

    def tearDown(self):
        attr_list = []
        self.selectIpHashField(attr_list)

        print ("Resetting IPV4 hash object to use all fields")
        self.attribute_set(HashConfigTest.ipv4_hash_obj, SWITCH_HASH_ATTR_FIELD_LIST, attr_list)

        print ("Resetting IPV6 hash object to use all fields")
        self.attribute_set(HashConfigTest.ipv6_hash_obj, SWITCH_HASH_ATTR_FIELD_LIST, attr_list)

        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_ECMP_IPV4_HASH, 0)
        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_ECMP_IPV6_HASH, 0)
        self.cleanup()

    def ipv6BasicHashTest(self):
        print("")
        print("IPv6 Hash test with varying Dest IP")

        count = self.ipv6HashTraffic(dst=1)
        print('ecmp-ipv6-pkt-count:', count)
        for i in range(0, 4):
            self.assertTrue((count[i] >= ((HashConfigTest.max_itrs / 4) * 0.5)),
                  "Ecmp paths are not equally balanced")

    def ipv6DstIPHashTest(self):
        print("")
        print("IPv6 Hash test with fixed Dest IP")

        count = self.ipv6HashTraffic(src=1)
        print('ecmp-ipv6-pkt-count:', count)
        for i in range(0, 4):
            self.assertTrue((count[i] == HashConfigTest.max_itrs) or (count[i] == 0),
                 "Traffic should use only of the port")

    def ipv4BasicHashTest(self):
        print("")
        print("IPv4 Hash test with varying Source IP")

        count = self.ipv4HashTraffic(src=1)
        print('ecmp-ipv4-pkt-count:', count)
        for i in range(0, 4):
            self.assertTrue((count[i] >= ((HashConfigTest.max_itrs / 4) * 0.5)),
                  "Ecmp paths are not equally balanced")

    def ipv4SrcIPHashTest(self):
        print("")
        print("IPv4 Hash test with fixed Source IP")

        count = self.ipv4HashTraffic(dst=1)
        print('ecmp-ipv4-pkt-count:', count)
        for i in range(0, 4):
            self.assertTrue((count[i] == HashConfigTest.max_itrs) or (count[i] == 0),
                 "Traffic should use only of the port")

    def ipv4HashTraffic(self, src=0, dst=0):

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
            print('IPv4 hash test with', HashConfigTest.max_itrs, 'packets')
            count = [0, 0, 0, 0]
            dst_ip = int(binascii.hexlify(socket.inet_aton('13.1.1.1')), 16)
            src_ip = int(binascii.hexlify(socket.inet_aton('192.168.8.1')), 16)
            for i in range(0, HashConfigTest.max_itrs):
                #Vary the Destination IP
                if dst == 1:
                    dst_ip_addr = socket.inet_ntoa(
                        binascii.unhexlify(format(dst_ip, 'x').zfill(8)))
                    pkt['IP'].dst = dst_ip_addr
                    exp_pkt1['IP'].dst = dst_ip_addr
                    exp_pkt2['IP'].dst = dst_ip_addr
                    exp_pkt3['IP'].dst = dst_ip_addr
                    exp_pkt4['IP'].dst = dst_ip_addr

                #Vary the Source IP
                if src == 1:
                    src_ip_addr = socket.inet_ntoa(
                        binascii.unhexlify(format(src_ip, 'x').zfill(8)))
                    pkt['IP'].src = src_ip_addr
                    exp_pkt1['IP'].src = src_ip_addr
                    exp_pkt2['IP'].src = src_ip_addr
                    exp_pkt3['IP'].src = src_ip_addr
                    exp_pkt4['IP'].src = src_ip_addr

                send_packet(self, self.devports[0], pkt)
                rcv_idx = verify_any_packet_any_port(
                    self, [exp_pkt1, exp_pkt2, exp_pkt3, exp_pkt4], [
                    self.devports[1], self.devports[2], self.devports[3], self.devports[4]
                    ], timeout=2)
                count[rcv_idx] += 1
                if src == 1:
                    src_ip += 100
                if dst == 1:
                    dst_ip += 100
            return count

        finally:
            pass

    def ipv6HashTraffic(self, src=0, dst=0):

        pkt = simple_tcpv6_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ipv6_dst='4000:5678:9abc:def0:4422:1133:5577:1111',
            ipv6_src='2000:5678:9abc:def0:4422:1133:5577:1111',
            ipv6_hlim=64)
        exp_pkt1 = simple_tcpv6_packet(
            eth_dst='00:22:22:33:44:' + str(self.port1 & 0xFF),
            eth_src='00:77:66:55:44:33',
            ipv6_dst='4000:5678:9abc:def0:4422:1133:5577:1111',
            ipv6_src='2000:5678:9abc:def0:4422:1133:5577:1111',
            ipv6_hlim=63)
        exp_pkt2 = simple_tcpv6_packet(
            eth_dst='00:22:22:33:44:' + str(self.port2 & 0xFF),
            eth_src='00:77:66:55:44:33',
            ipv6_dst='4000:5678:9abc:def0:4422:1133:5577:1111',
            ipv6_src='2000:5678:9abc:def0:4422:1133:5577:1111',
            ipv6_hlim=63)
        exp_pkt3 = simple_tcpv6_packet(
            eth_dst='00:22:22:33:44:' + str(self.port3 & 0xFF),
            eth_src='00:77:66:55:44:33',
            ipv6_dst='4000:5678:9abc:def0:4422:1133:5577:1111',
            ipv6_src='2000:5678:9abc:def0:4422:1133:5577:1111',
            ipv6_hlim=63)
        exp_pkt4 = simple_tcpv6_packet(
            eth_dst='00:22:22:33:44:' + str(self.port4 & 0xFF),
            eth_src='00:77:66:55:44:33',
            ipv6_dst='4000:5678:9abc:def0:4422:1133:5577:1111',
            ipv6_src='2000:5678:9abc:def0:4422:1133:5577:1111',
            ipv6_hlim=63)
        try:
            print('IPv6 hash test with', HashConfigTest.max_itrs, 'packets')
            count = [0, 0, 0, 0]
            dst_ip = int(binascii.hexlify(socket.inet_pton(socket.AF_INET6, '4000:5678:9abc:def0:4422:1133:5577:1111')), 16)
            src_ip = int(binascii.hexlify(socket.inet_pton(socket.AF_INET6, '2000:5678:9abc:def0:4422:1133:5577:1111')), 16)
            for i in range(0, HashConfigTest.max_itrs):
                #Vary the Destination IP
                if dst == 1:
                    dst_ip_addr = socket.inet_ntop(socket.AF_INET6,
                        binascii.unhexlify(format(dst_ip, 'x').zfill(8)))
                    pkt['IPv6'].dst = dst_ip_addr
                    exp_pkt1['IPv6'].dst = dst_ip_addr
                    exp_pkt2['IPv6'].dst = dst_ip_addr
                    exp_pkt3['IPv6'].dst = dst_ip_addr
                    exp_pkt4['IPv6'].dst = dst_ip_addr

                #Vary the Source IP
                if src == 1:
                    src_ip_addr = socket.inet_ntop(socket.AF_INET6,
                         binascii.unhexlify(format(src_ip, 'x').zfill(8)))
                    pkt['IPv6'].src = src_ip_addr
                    exp_pkt1['IPv6'].src = src_ip_addr
                    exp_pkt2['IPv6'].src = src_ip_addr
                    exp_pkt3['IPv6'].src = src_ip_addr
                    exp_pkt4['IPv6'].src = src_ip_addr

                send_packet(self, self.devports[0], pkt)
                rcv_idx = verify_any_packet_any_port(
                     self, [exp_pkt1, exp_pkt2, exp_pkt3, exp_pkt4], [
                     self.devports[1], self.devports[2], self.devports[3], self.devports[4]
                     ], timeout=2)
                count[rcv_idx] += 1
                if src == 1:
                    src_ip += 1000
                if dst == 1:
                    dst_ip += 1000
            return count
        finally:
            pass

###############################################################################

@disabled
class HashFieldsTest(ApiHelper):
    '''
    This test is left disabled. The total packets are around 20000 and the whole
    test takes around 40min to execute.
    '''
    max_itrs = 1000
    L3_HASH_KEYS = ['src-ip', 'dst-ip', 'ip-proto', 'src-port', 'dst-port']
    SRC_IP_RANGE = ['8.0.0.0', '8.255.255.255']
    DST_IP_RANGE = ['9.0.0.0', '9.255.255.255']
    SRC_IPV6_RANGE = ['20D0:A800:0:00::', '20D0:A800:0:00::FFFF']
    DST_IPV6_RANGE = ['20D0:A800:0:01::', '20D0:A800:0:01::FFFF']
    IP_PROTO_RANGE = [x for x in list(range(0,256)) if x not in (0, 2, 4, 254)]
    VLANIDS = list(range(1032, 1279))
    VLANIP = '192.168.{}.1/24'

    # proto/sip/dip/sport/dport in that order
    def selectHashField(self, attr_list, attr='all'):
        if(attr == 'all' or attr == 'ip-proto'):
            attr_list.append(switcht_list_val_t(type=switcht_value_type.UINT32,
                                        u32data=SWITCH_HASH_ATTR_FIELD_IP_PROTO))
        if(attr == 'all' or attr == 'src-ip'):
            attr_list.append(switcht_list_val_t(type=switcht_value_type.UINT32,
                                        u32data=SWITCH_HASH_ATTR_FIELD_SRC_ADDR))
        if(attr == 'all' or attr == 'dst-ip'):
            attr_list.append(switcht_list_val_t(type=switcht_value_type.UINT32,
                                        u32data=SWITCH_HASH_ATTR_FIELD_DST_ADDR))
        if(attr == 'all' or attr == 'dst-port'):
            attr_list.append(switcht_list_val_t(type=switcht_value_type.UINT32,
                                        u32data=SWITCH_HASH_ATTR_FIELD_SRC_PORT))
        if(attr == 'all' or attr == 'src-port'):
            attr_list.append(switcht_list_val_t(type=switcht_value_type.UINT32,
                                        u32data=SWITCH_HASH_ATTR_FIELD_DST_PORT))

    def setUp(self):
        print()
        self.configure()
        self.ecmp_list = [self.devports[1], self.devports[2], self.devports[3], self.devports[4]]
        self.lag_list  = [self.devports[5], self.devports[6], self.devports[7], self.devports[8]]
        self.rif0 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_VLAN,
                                vlan_handle=self.vlan10, vrf_handle=self.vrf10,
                                src_mac=self.rmac)

        self.ecmp = self.add_ecmp(self.device)
        for l_port in [self.port1, self.port2, self.port3, self.port4]:
            l_rif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, \
                                 port_handle=l_port, vrf_handle=self.vrf10, \
                                 src_mac=self.rmac)
            l_ip = '13.0.0.' + str(l_port & 256)
            l_mac = '00:22:22:33:44:' + str(l_port & 0xFF)
            l_nhop = self.add_nexthop(self.device, handle=l_rif, dest_ip=l_ip)
            l_neighbor = self.add_neighbor(self.device, mac_address=l_mac, handle=l_rif, dest_ip=l_ip)
            ecmp_member = self.add_ecmp_member(self.device, nexthop_handle=l_nhop, ecmp_handle=self.ecmp)

        self.lag0 = self.add_lag(self.device)
        self.lag0_mbr0 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port5)
        self.lag0_mbr1 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port6)
        self.lag0_mbr2 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port7)
        self.lag0_mbr3 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port8)

        vlan_mbr0 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port0)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        vlan_mbr1 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.lag0)

        self.route4 = self.add_route(self.device, ip_prefix='9.0.0.0/8',
                                    vrf_handle=self.vrf10, nexthop_handle=self.ecmp)
        self.route6 = self.add_route(self.device, ip_prefix='20D0::0/16',
                                    vrf_handle=self.vrf10, nexthop_handle=self.ecmp)
        mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan10,
                mac_address='00:11:22:33:44:55', destination_handle=self.lag0)

        #Selecting all the parameters
        attr_list = []
        fg_attr_list = []
        self.selectHashField(attr_list, 'all')
        self.ecmp_v4_oid = self.add_hash(self.device, field_list=attr_list,
                                       fine_grained_field_list=fg_attr_list)
        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_ECMP_IPV4_HASH, self.ecmp_v4_oid)

        self.ecmp_v6_oid = self.add_hash(self.device, field_list=attr_list,
                                       fine_grained_field_list=fg_attr_list)
        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_ECMP_IPV6_HASH, self.ecmp_v6_oid)

        self.lag_v4_oid = self.add_hash(self.device, field_list=attr_list,
                                      fine_grained_field_list=fg_attr_list)
        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_LAG_IPV4_HASH, self.lag_v4_oid)

        self.lag_v6_oid = self.add_hash(self.device, field_list=attr_list,
                                      fine_grained_field_list=fg_attr_list)
        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_LAG_IPV6_HASH, self.lag_v6_oid)

    def tearDown(self):
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_ECMP_IPV4_HASH, 0)
        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_ECMP_IPV6_HASH, 0)
        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_LAG_IPV4_HASH, 0)
        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_LAG_IPV6_HASH, 0)
        self.cleanup()

    def runTest(self):
        try:
            for hash_key in self.L3_HASH_KEYS:
                self.TrafficTest(False, hash_key, 'v4', self.ecmp_list)

            for hash_key in self.L3_HASH_KEYS:
                self.TrafficTest(False, hash_key, 'v6', self.ecmp_list)

            for hash_key in self.L3_HASH_KEYS:
                self.TrafficTest(True, hash_key, 'v4', self.lag_list)

            for hash_key in self.L3_HASH_KEYS:
                self.TrafficTest(True, hash_key, 'v6', self.lag_list)
        finally:
            pass

    def HashTest(self, ip_type, hash_key):
        try:
            self.TrafficTest(False, hash_key, ip_type, self.ecmp_list)
            self.TrafficTest(True, hash_key, ip_type, self.lag_list)
        finally:
            pass

    def _get_ip_protos(self):
       for proto in self.IP_PROTO_RANGE:
            yield proto

    ip_protos = None
    def TrafficTest(self, bridge, hash_key, ip_type, dst_port_list):
        count = [0,0,0,0]
        if hash_key == 'ip-proto':
            iters = len(self.IP_PROTO_RANGE)
            self.ip_protos = self._get_ip_protos()
        else:
            iters = self.max_itrs
        for i in range(iters):
            rcv_idx, _ = self.check_packet(hash_key, ip_type, dst_port_list, bridge)
            count[rcv_idx] += 1
        print(('ecmp ' if bridge == False else 'lag  ') + ip_type + ' LB for ' + hash_key, count)
        for i in range(0, 4):
            self.assertTrue((count[i] >= ((iters / 4) * 0.7)),
                 "%s paths are not equally balanced for %s, %s" % ('ecmp' if bridge == False else 'lag', ip_type, hash_key))

    def check_packet(self, hash_key, ip_type, dst_port_list, bridge=False):
        base_mac = "00:11:11:11:11:11"
        if ip_type == 'v4':
            self.src_ip_range = [six.ensure_text(x) for x in self.SRC_IP_RANGE]
            self.dst_ip_range = [six.ensure_text(x) for x in self.DST_IP_RANGE]
            self.src_ip_interval = LpmDict.IpInterval(ip_address(self.src_ip_range[0]), ip_address(self.src_ip_range[1]))
            self.dst_ip_interval = LpmDict.IpInterval(ip_address(self.dst_ip_range[0]), ip_address(self.dst_ip_range[1]))
        else:
            self.src_ip_range = [six.ensure_text(x) for x in self.SRC_IPV6_RANGE]
            self.dst_ip_range = [six.ensure_text(x) for x in self.DST_IPV6_RANGE]
            self.src_ip_interval = LpmDict.IpInterval(ip_address(self.src_ip_range[0]), ip_address(self.src_ip_range[1]))
            self.dst_ip_interval = LpmDict.IpInterval(ip_address(self.dst_ip_range[0]), ip_address(self.dst_ip_range[1]))
        ip_src = self.src_ip_interval.get_random_ip() if hash_key == 'src-ip' else self.src_ip_interval.get_first_ip()
        ip_dst = self.dst_ip_interval.get_random_ip() if hash_key == 'dst-ip' else self.dst_ip_interval.get_first_ip()
        sport = random.randint(0, 65535) if hash_key == 'src-port' else 1234
        dport = random.randint(0, 65535) if hash_key == 'dst-port' else 80
        src_mac = (base_mac[:-5] + "%02x" % random.randint(0, 255) + ":" + "%02x" % random.randint(0, 255)) if hash_key == 'src-mac' else base_mac
        dst_mac = "00:11:22:33:44:55" if bridge == True else self.rmac
        ip_proto = self.get_ip_proto() if hash_key == 'ip-proto' else None
        if ip_type == 'v4':
            pkt = simple_tcp_packet(
                            eth_dst=dst_mac,
                            eth_src=src_mac,
                            ip_src=ip_src,
                            ip_dst=ip_dst,
                            tcp_sport=sport,
                            tcp_dport=dport,
                            ip_ttl=64)
            exp_pkt = simple_tcp_packet(
                            eth_src=self.rmac if bridge == False else src_mac,
                            ip_src=ip_src,
                            ip_dst=ip_dst,
                            tcp_sport=sport,
                            tcp_dport=dport,
                            ip_ttl=63 if bridge == False else 64)
        else:
            pkt = simple_tcpv6_packet(
                            eth_dst=dst_mac,
                            eth_src=src_mac,
                            ipv6_dst=ip_dst,
                            ipv6_src=ip_src,
                            tcp_sport=sport,
                            tcp_dport=dport,
                            ipv6_hlim=64)
            exp_pkt = simple_tcpv6_packet(
                            eth_src=self.rmac if bridge == False else src_mac,
                            ipv6_dst=ip_dst,
                            ipv6_src=ip_src,
                            tcp_sport=sport,
                            tcp_dport=dport,
                            ipv6_hlim=63 if bridge == False else 64)

        if hash_key == 'ip-proto':
            if ip_type == 'v4':
                pkt['IP'].proto = ip_proto
                exp_pkt['IP'].proto = ip_proto
            else:
                pkt['IPv6'].nh = ip_proto
                exp_pkt['IPv6'].nh = ip_proto
        masked_exp_pkt = ptf.mask.Mask(exp_pkt)
        mask_set_do_not_care_packet(masked_exp_pkt, ptf.packet.Ether, "dst")

        send_packet(self, self.devports[0], pkt)
        return verify_packet_any_port(self, masked_exp_pkt, dst_port_list)

    def get_ip_proto(self):
        for proto in self.ip_protos:
            return proto

###############################################################################

@disabled  # enable this once p4 logic can fit into x1 profile
@group('hash')
class IPFragHashingTest(ApiHelper):
    '''
    All V4 fragment packets must take the same path
    '''
    def setUp(self):
        print()
        self.configure()

        self.rif0 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT,
                                port_handle=self.port0, vrf_handle=self.vrf10,
                                src_mac=self.rmac)

        self.ecmp = self.add_ecmp(self.device)
        self.ecmp_list = [self.devports[1], self.devports[2], self.devports[3], self.devports[4]]
        for l_port in [self.port1, self.port2, self.port3, self.port4]:
            l_rif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, \
                                 port_handle=l_port, vrf_handle=self.vrf10, \
                                 src_mac=self.rmac)
            l_ip = '13.0.0.' + str(l_port & 256)
            l_mac = '00:22:22:33:44:' + str(l_port & 0xFF)
            l_nhop = self.add_nexthop(self.device, handle=l_rif, dest_ip=l_ip)
            l_neighbor = self.add_neighbor(self.device, mac_address=l_mac, handle=l_rif, dest_ip=l_ip)
            ecmp_member = self.add_ecmp_member(self.device, nexthop_handle=l_nhop, ecmp_handle=self.ecmp)

        self.route4 = self.add_route(self.device, ip_prefix='10.0.0.0/8',
                                    vrf_handle=self.vrf10, nexthop_handle=self.ecmp)

    def runTest(self):
        if self.client.is_feature_enable(SWITCH_FEATURE_INNER_HASH):
            print("Dynamic Hashing not yet supported when inner packet hash is used")
            return
        pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.10.10.1',
            ip_src='20.20.20.1',
            tcp_flags="R",
            ip_ttl=64)
        exp_pkt = simple_tcp_packet(
            eth_src='00:77:66:55:44:33',
            ip_dst='10.10.10.1',
            ip_src='20.20.20.1',
            tcp_flags="R",
            ip_ttl=63)

        print("Non fragmented valid ipv4 packet")
        masked_exp_pkt = ptf.mask.Mask(exp_pkt)
        mask_set_do_not_care_packet(masked_exp_pkt, ptf.packet.Ether, "dst")
        send_packet(self, self.devports[0], pkt)
        idx, _ = verify_packet_any_port(self, masked_exp_pkt, self.ecmp_list)
        print(" --> Tx port %s" %(idx))

        print("First fragment ipv4 packet")
        pkt['IP'].flags = 1
        pkt['IP'].frag = 0
        exp_pkt['IP'].flags = 1
        exp_pkt['IP'].frag = 0
        masked_exp_pkt = ptf.mask.Mask(exp_pkt)
        mask_set_do_not_care_packet(masked_exp_pkt, ptf.packet.Ether, "dst")
        send_packet(self, self.devports[0], pkt)
        f_fg_idx, _ = verify_packet_any_port(self, masked_exp_pkt, self.ecmp_list)
        print(" --> Tx port %s" %(f_fg_idx))
        #self.assertEqual(rcv_idx, idx)

        print("Non first/last fragment ipv4 packet")
        pkt['IP'].flags = 1
        pkt['IP'].frag = 20
        exp_pkt['IP'].flags = 1
        exp_pkt['IP'].frag = 20
        masked_exp_pkt = ptf.mask.Mask(exp_pkt)
        mask_set_do_not_care_packet(masked_exp_pkt, ptf.packet.Ether, "dst")
        send_packet(self, self.devports[0], pkt)
        fg_idx, _ = verify_packet_any_port(self, masked_exp_pkt, self.ecmp_list)
        self.assertEqual(f_fg_idx, fg_idx)
        print(" --> Same Tx Port %s as first fragment" %(fg_idx))

        print("Last fragment ipv4 packet")
        pkt['IP'].flags = 0
        pkt['IP'].frag = 20
        exp_pkt['IP'].flags = 0
        exp_pkt['IP'].frag = 20
        masked_exp_pkt = ptf.mask.Mask(exp_pkt)
        mask_set_do_not_care_packet(masked_exp_pkt, ptf.packet.Ether, "dst")
        send_packet(self, self.devports[0], pkt)
        fg_idx, _ = verify_packet_any_port(self, masked_exp_pkt, self.ecmp_list)
        self.assertEqual(f_fg_idx, fg_idx)
        print(" --> Same Tx Port %s as other fragments" %(fg_idx))

    def tearDown(self):
        self.cleanup()

###############################################################################


@group('hash')
class IPProtoHashingTest(ApiHelper):
    max_itrs = 300
    lb_factor = 0.6
    DST_IP_RANGE = ['11.0.0.0', '11.255.255.255']

    def setUp(self):
        print()
        self.configure()

        self.dst_ip_range = [six.ensure_text(x) for x in self.DST_IP_RANGE]
        self.dst_ip_interval = LpmDict.IpInterval(ip_address(self.dst_ip_range[0]), ip_address(self.dst_ip_range[1]))

        self.ecmp_list = [self.devports[1], self.devports[2], self.devports[3], self.devports[4]]
        self.rif0 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT,
                                port_handle=self.port0, vrf_handle=self.vrf10,
                                src_mac=self.rmac)

        self.ecmp = self.add_ecmp(self.device)
        for l_port in [self.port1, self.port2, self.port3, self.port4]:
            l_rif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, \
                                 port_handle=l_port, vrf_handle=self.vrf10, \
                                 src_mac=self.rmac)
            l_ip = '13.0.0.' + str(l_port & 256)
            l_mac = '00:22:22:33:44:' + str(l_port & 0xFF)
            l_nhop = self.add_nexthop(self.device, handle=l_rif, dest_ip=l_ip)
            l_neighbor = self.add_neighbor(self.device, mac_address=l_mac, handle=l_rif, dest_ip=l_ip)
            ecmp_member = self.add_ecmp_member(self.device, nexthop_handle=l_nhop, ecmp_handle=self.ecmp)

        self.route4 = self.add_route(self.device, ip_prefix='11.0.0.0/8',
                                    vrf_handle=self.vrf10, nexthop_handle=self.ecmp)

    def runTest(self):
        try:
            self.attribute_set(self.vrf10, SWITCH_VRF_ATTR_IP_OPTIONS_ACTION, SWITCH_VRF_ATTR_IP_OPTIONS_ACTION_FORWARD)
            self.UDPTest()
            self.TCPTest()
        finally:
            self.attribute_set(self.vrf10, SWITCH_VRF_ATTR_IP_OPTIONS_ACTION, SWITCH_VRF_ATTR_IP_OPTIONS_ACTION_TRAP)
            pass

    def tearDown(self):
        self.cleanup()

    def UDPTest(self):
        print("UDP hashing test")
        count = [0,0,0,0]
        pkt = simple_udp_packet(eth_dst=self.rmac,
                                ip_src='8.1.1.1',
                                ip_dst='11.1.1.1',
                                ip_ihl=6,
                                ip_options=[IPOption(b'\x14\x04\x00\x00')],
                                ip_ttl=64)
        exp_pkt = simple_udp_packet(
                                eth_src=self.rmac,
                                ip_src='8.1.1.1',
                                ip_dst='11.1.1.1',
                                ip_ihl=6,
                                ip_options=[IPOption(b'\x14\x04\x00\x00')],
                                ip_ttl=63)
        for i in range(0, self.max_itrs):
            ip_dst = self.dst_ip_interval.get_random_ip()
            sport = random.randint(0, 65535)
            dport = random.randint(0, 65535)
            pkt['IP'].dst = ip_dst
            exp_pkt['IP'].dst = ip_dst
            pkt['UDP'].dst = dport
            pkt['UDP'].src = sport
            exp_pkt['UDP'].dst = dport
            exp_pkt['UDP'].src = sport
            masked_exp_pkt = ptf.mask.Mask(exp_pkt)
            mask_set_do_not_care_packet(masked_exp_pkt, ptf.packet.Ether, "dst")
            send_packet(self, self.devports[0], pkt)
            rcv_idx, _ = verify_packet_any_port(self, masked_exp_pkt, self.ecmp_list)
            count[rcv_idx] += 1
        print('ecmp-ipv4-pkt-count:', count)
        for i in range(0, 4):
            self.assertTrue((count[i] >= ((self.max_itrs / 4) * self.lb_factor)),
                "UDP ECMP paths not equally balanced")

    def TCPTest(self):
        print("TCP hashing test")
        count = [0,0,0,0]
        pkt = simple_tcp_packet(eth_dst=self.rmac,
                                ip_src='8.1.1.1',
                                ip_dst='11.1.1.1',
                                ip_ihl=6,
                                ip_options=[IPOption(b'\x14\x04\x00\x00')],
                                ip_ttl=64)
        exp_pkt = simple_tcp_packet(
                                eth_src=self.rmac,
                                ip_src='8.1.1.1',
                                ip_dst='11.1.1.1',
                                ip_ihl=6,
                                ip_options=[IPOption(b'\x14\x04\x00\x00')],
                                ip_ttl=63)
        for i in range(0, self.max_itrs):
            ip_dst = self.dst_ip_interval.get_random_ip()
            sport = random.randint(0, 65535)
            dport = random.randint(0, 65535)
            pkt['IP'].dst = ip_dst
            exp_pkt['IP'].dst = ip_dst
            pkt['TCP'].dst = dport
            pkt['TCP'].src = sport
            exp_pkt['TCP'].dst = dport
            exp_pkt['TCP'].src = sport
            masked_exp_pkt = ptf.mask.Mask(exp_pkt)
            mask_set_do_not_care_packet(masked_exp_pkt, ptf.packet.Ether, "dst")
            send_packet(self, self.devports[0], pkt)
            rcv_idx, _ = verify_packet_any_port(self, masked_exp_pkt, self.ecmp_list)
            count[rcv_idx] += 1
        print('ecmp-ipv4-pkt-count:', count)
        for i in range(0, 4):
            self.assertTrue((count[i] >= ((self.max_itrs / 4) * self.lb_factor)),
                "TCP ECMP paths not equally balanced")

###############################################################################

@group('hash')
class LagHashAlgorithmTest(ApiHelper):
    '''
    This performs LAG load balancing
    @test - LAG Hash Test with different types of Algorithm CRC_8, CRC_32 and RANDOM
    '''

    max_itrs  = 200
    default_lag_algorithm = 0
    lb_factor = 0.5
    custom_algos = [
    {"reverse":False,"polynomial":0x04c11db7,"init":0xffffffff,"final_xor":0xffffffff,"hash_bit_width":32},
    {"reverse":False,"polynomial":0xabcdef,"init":0xabcdef,"final_xor":0xabcdef,"hash_bit_width":24},
    {"reverse":True,"polynomial":0x1021,"init":0xffff,"final_xor":0,"hash_bit_width":16},
    ]
    def setUp(self):
        print("")
        self.configure()

        #Configuring LAG
        self.lag0 = self.add_lag(self.device)
        self.lag0_mbr0 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port1)
        self.lag0_mbr1 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port2)
        self.lag0_mbr2 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port3)
        self.lag0_mbr3 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port4)

        #Populating the MAC table
        mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan10,
            mac_address='00:11:11:11:11:11', destination_handle=self.port0)
        mac1 = self.add_mac_entry(self.device, vlan_handle=self.vlan10,
            mac_address='00:22:22:22:22:22', destination_handle=self.lag0)

        #configuring the VLAN
        vlan_mbr0 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port0)
        vlan_mbr1 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.lag0)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.attribute_set(self.lag0, SWITCH_LAG_ATTR_PORT_VLAN_ID, 10)

        #Reading the default value
        LagHashAlgorithmTest.default_lag_algorithm = self.attribute_get(self.device, SWITCH_DEVICE_ATTR_LAG_DEFAULT_HASH_ALGO)
        print("default_lag_algorithm", LagHashAlgorithmTest.default_lag_algorithm)


    def basicLagHashTest (self):
        print("")
        lb_count = self.hashTraffic()
        print('lag-count:', lb_count)
        for i in range(0, 4):
            self.assertTrue((lb_count[i] >= ((LagHashAlgorithmTest.max_itrs / 4) * self.lb_factor)),
                  "Lag paths are not equally balanced")
        return lb_count

    def hashTraffic (self):
        try:
            print('Lag has test with', LagHashAlgorithmTest.max_itrs , 'packets')
            pkt = simple_tcp_packet(
                eth_src='00:11:11:11:11:11',
                eth_dst='00:22:22:22:22:22',
                ip_dst='10.10.10.1',
                ip_src='13.1.1.1',
                ip_id=109,
                ip_ttl=64)

            count = [0, 0, 0, 0]
            src_ip = int(binascii.hexlify(socket.inet_aton('13.1.1.1')), 16)
            for i in range(0, LagHashAlgorithmTest.max_itrs):
                #With new source IP
                src_ip_addr = socket.inet_ntoa(
                     binascii.unhexlify(format(src_ip, 'x').zfill(8)))
                pkt['IP'].src = src_ip_addr

                send_packet(self, self.devports[0], pkt)
                rcv_idx = verify_any_packet_any_port(
                     self, [pkt, pkt, pkt, pkt],
                     [self.devports[1], self.devports[2], self.devports[3], self.devports[4]])
                count[rcv_idx] += 1
                src_ip += 1000
            return count
        finally:
            pass

    def setHashAlgorithm(self, attr, type):
        hash_algo_obj = self.add_hash_algorithm(self.device,
                                                type=SWITCH_HASH_ALGORITHM_ATTR_TYPE_PRE_DEFINED,
                                                algorithm=type)
        self.attribute_set(self.device, attr, hash_algo_obj)
        algo = self.attribute_get(self.device, attr)
        algorithm = self.attribute_get(algo, SWITCH_HASH_ALGORITHM_ATTR_ALGORITHM)
        self.assertEqual(type, algorithm)
        print("")
        print("LAG hash algorithm set to: ", type )

    def setCustomHashAlgorithm(self, attr, reverse, polynomial, init, final_xor, hash_bit_width):
        hash_algo_obj = self.add_hash_algorithm(self.device,
                                                type=SWITCH_HASH_ALGORITHM_ATTR_TYPE_USER_DEFINED,
                                                reverse=reverse,
                                                polynomial=polynomial,
                                                init=init,
                                                final_xor=final_xor,
                                                hash_bit_width=hash_bit_width)
        self.attribute_set(self.device, attr, hash_algo_obj)
        algo = self.attribute_get(self.device, attr)
        _reverse = self.attribute_get(algo, SWITCH_HASH_ALGORITHM_ATTR_REVERSE)
        _polynomial = self.attribute_get(algo, SWITCH_HASH_ALGORITHM_ATTR_POLYNOMIAL)
        _init = self.attribute_get(algo, SWITCH_HASH_ALGORITHM_ATTR_INIT)
        _final_xor = self.attribute_get(algo, SWITCH_HASH_ALGORITHM_ATTR_FINAL_XOR)
        _hash_bit_width = self.attribute_get(algo, SWITCH_HASH_ALGORITHM_ATTR_HASH_BIT_WIDTH)
        self.assertEqual(reverse, _reverse)
        self.assertEqual(polynomial, _polynomial)
        self.assertEqual(init, _init)
        self.assertEqual(final_xor, _final_xor)
        self.assertEqual(hash_bit_width, _hash_bit_width)
        print("")
        print("LAG hash algorithm programmed with reverse: ", reverse, " polynomial: ", polynomial,
              " init: ", init, " final_xor: ", final_xor, " hash_bit_width: ", hash_bit_width)

    def runTest(self):
        try:
            #setting the algorithm to CRC_32
            self.setHashAlgorithm(SWITCH_DEVICE_ATTR_LAG_DEFAULT_HASH_ALGO, SWITCH_HASH_ALGORITHM_ATTR_ALGORITHM_CRC_32)
            self.basicLagHashTest()

            #setting the algorithm to RANDOM
            self.setHashAlgorithm(SWITCH_DEVICE_ATTR_LAG_DEFAULT_HASH_ALGO, SWITCH_HASH_ALGORITHM_ATTR_ALGORITHM_RANDOM)
            self.basicLagHashTest()

            lb_counts = []
            for algo in LagHashAlgorithmTest.custom_algos:
                self.setCustomHashAlgorithm(SWITCH_DEVICE_ATTR_LAG_DEFAULT_HASH_ALGO,
                                            algo["reverse"], algo["polynomial"],
                                            algo["init"], algo["final_xor"],
                                            algo["hash_bit_width"])
                lb_counts.append(self.basicLagHashTest())
            same_distribution = True
            for index, count in enumerate(lb_counts):
                same_distribution &= (count == lb_counts[index-1])
            self.assertFalse(same_distribution, "Traffic distribution for custom algos:{} are all same:{} (should be"
            "different)".format(LagHashAlgorithmTest.custom_algos, lb_counts))

        finally:
            self.attribute_set(self.device, SWITCH_DEVICE_ATTR_LAG_DEFAULT_HASH_ALGO, 0)
            pass

    def tearDown(self):
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.lag0, SWITCH_LAG_ATTR_PORT_VLAN_ID, 0)
        self.cleanup()

###############################################################################

@group('hash')
class EcmpHashAlgorithmTest(ApiHelper):
    '''
    Hash test with different types of Algorithm CRC_8, CRC_32 and RANDOM
    @test - Algorithm selection for ECMP IPv4 Hash Test
    @test - Algorithm selection for ECMP IPv6 Hash Test
    '''
    max_itrs = 200
    default_ecmp_algorithm = 0
    custom_algos = [
    {"reverse":False,"polynomial":0x04c11db7,"init":0xffffffff,"final_xor":0xffffffff,"hash_bit_width":32},
    {"reverse":False,"polynomial":0x117123,"init":0xffffff,"final_xor":0x123456,"hash_bit_width":24},
    {"reverse":True,"polynomial":0x1021,"init":0xffff,"final_xor":0,"hash_bit_width":16},
    ]

    def setUp(self):
        print("")
        self.configure()
        self.ecmp = self.add_ecmp(self.device)
        self.rif0 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT,
                                 port_handle=self.port0, vrf_handle=self.vrf10,
                                 src_mac=self.rmac)
        #IPV4 Route
        self.route4 = self.add_route(self.device, ip_prefix='13.0.0.0/8',
                          vrf_handle=self.vrf10, nexthop_handle=self.ecmp)

        #IPv6 Route
        self.route6 = self.add_route(self.device,
               ip_prefix='4000:5678:9abc:def0:0000:0000:0000:0000/64',
               vrf_handle=self.vrf10, nexthop_handle=self.ecmp)

        #Reading the default value
        EcmpHashAlgorithmTest.default_ecmp_algorithm = \
              self.attribute_get(self.device, SWITCH_DEVICE_ATTR_ECMP_DEFAULT_HASH_ALGO)
        print("default_ecmp_algorithm", EcmpHashAlgorithmTest.default_ecmp_algorithm)

        for l_port in [self.port1, self.port2, self.port3, self.port4]:
            l_rif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, \
                                 port_handle=l_port, vrf_handle=self.vrf10, \
                                 src_mac=self.rmac)
            l_ip = '13.0.0.' + str(l_port & 256)
            l_mac = '00:22:22:33:44:' + str(l_port & 0xFF)
            l_nhop = self.add_nexthop(self.device, handle=l_rif, dest_ip=l_ip)
            l_neighbor = self.add_neighbor(self.device, mac_address=l_mac, handle=l_rif, dest_ip=l_ip)
            ecmp_member = self.add_ecmp_member(self.device, nexthop_handle=l_nhop, ecmp_handle=self.ecmp)

    def setHashAlgorithm(self, attr, type):
        hash_algo_obj = self.add_hash_algorithm(self.device,
                                                type=SWITCH_HASH_ALGORITHM_ATTR_TYPE_PRE_DEFINED,
                                                algorithm=type)
        self.attribute_set(self.device, attr, hash_algo_obj)
        algo = self.attribute_get(self.device, attr)
        algorithm = self.attribute_get(algo, SWITCH_HASH_ALGORITHM_ATTR_ALGORITHM)
        self.assertEqual(type, algorithm)
        print("")
        print("ECMP hash algorithm set to: ", type )
        return hash_algo_obj

    def setCustomHashAlgorithm(self, attr, reverse, polynomial, init, final_xor, hash_bit_width):
        hash_algo_obj = self.add_hash_algorithm(self.device,
                                                type=SWITCH_HASH_ALGORITHM_ATTR_TYPE_USER_DEFINED,
                                                reverse=reverse,
                                                polynomial=polynomial,
                                                init=init,
                                                final_xor=final_xor,
                                                hash_bit_width=hash_bit_width)
        self.attribute_set(self.device, attr, hash_algo_obj)
        algo = self.attribute_get(self.device, attr)
        _reverse = self.attribute_get(algo, SWITCH_HASH_ALGORITHM_ATTR_REVERSE)
        _polynomial = self.attribute_get(algo, SWITCH_HASH_ALGORITHM_ATTR_POLYNOMIAL)
        _init = self.attribute_get(algo, SWITCH_HASH_ALGORITHM_ATTR_INIT)
        _final_xor = self.attribute_get(algo, SWITCH_HASH_ALGORITHM_ATTR_FINAL_XOR)
        _hash_bit_width = self.attribute_get(algo, SWITCH_HASH_ALGORITHM_ATTR_HASH_BIT_WIDTH)
        self.assertEqual(reverse, _reverse)
        self.assertEqual(polynomial, _polynomial)
        self.assertEqual(init, _init)
        self.assertEqual(final_xor, _final_xor)
        self.assertEqual(hash_bit_width, _hash_bit_width)
        print("")
        print("ECMP hash algorithm programmed with reverse: ", reverse, " polynomial: ", polynomial,
              " init: ", init, " final_xor: ", final_xor, " hash_bit_width: ", hash_bit_width)

    def runTest(self):
        try:

            #setting the algorithm to CRC_32
            self.setHashAlgorithm(SWITCH_DEVICE_ATTR_ECMP_DEFAULT_HASH_ALGO,
                                     SWITCH_HASH_ALGORITHM_ATTR_ALGORITHM_CRC_32)

            self.ipv4BasicHashTest()
            self.ipv6BasicHashTest()

            #setting the algorithm to RANDOM
            self.setHashAlgorithm(SWITCH_DEVICE_ATTR_ECMP_DEFAULT_HASH_ALGO,
                                   SWITCH_HASH_ALGORITHM_ATTR_ALGORITHM_RANDOM)

            self.ipv4BasicHashTest()
            self.ipv6BasicHashTest()

            v4_lb_counts = []
            v6_lb_counts = []
            for algo in EcmpHashAlgorithmTest.custom_algos:
                self.setCustomHashAlgorithm(SWITCH_DEVICE_ATTR_ECMP_DEFAULT_HASH_ALGO,
                                            algo["reverse"], algo["polynomial"],
                                            algo["init"], algo["final_xor"],
                                            algo["hash_bit_width"])
                v4_lb_counts.append(self.ipv4BasicHashTest())
                v6_lb_counts.append(self.ipv6BasicHashTest())

            same_distribution = True
            for index, count in enumerate(v4_lb_counts):
                same_distribution &= (count == v4_lb_counts[index-1])
            self.assertFalse(same_distribution, "Traffic distribution for custom algos:{} are all same:{} for v4"
            "traffic(should be different)".format(EcmpHashAlgorithmTest.custom_algos, v4_lb_counts))

            same_distribution = True
            for index, count in enumerate(v6_lb_counts):
                same_distribution &= (count == v6_lb_counts[index-1])
            self.assertFalse(same_distribution, "Traffic distribution for custom algos:{} are all same:{} for v6"
            "traffic(should be different)".format(EcmpHashAlgorithmTest.custom_algos, v6_lb_counts))

        finally:
            self.attribute_set(self.device, SWITCH_DEVICE_ATTR_ECMP_DEFAULT_HASH_ALGO, 0)
            pass

    def tearDown(self):
        print("Setting back the algorithm to default value")
        self.cleanup()

    def ipv6BasicHashTest(self):
        print("")
        count = self.ipv6HashTraffic(dst=1)
        print('ecmp-ipv6-pkt-count:', count)
        for i in range(0, 4):
            self.assertTrue((count[i] >= ((EcmpHashAlgorithmTest.max_itrs / 4) * 0.6)),
                  "Ecmp paths are not equally balanced")
        return count

    def ipv4BasicHashTest(self):
        print("")
        count = self.ipv4HashTraffic(src=1)
        print('ecmp-ipv4-pkt-count:', count)
        for i in range(0, 4):
            self.assertTrue((count[i] >= ((EcmpHashAlgorithmTest.max_itrs / 4) * 0.6)),
                  "Ecmp paths are not equally balanced")
        return count

    def ipv4HashTraffic(self, src=0, dst=0):

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
            print('IPv4 hash test with', EcmpHashAlgorithmTest.max_itrs, 'packets')
            count = [0, 0, 0, 0]
            dst_ip = int(binascii.hexlify(socket.inet_aton('13.1.1.1')), 16)
            src_ip = int(binascii.hexlify(socket.inet_aton('192.168.8.1')), 16)
            for i in range(0, EcmpHashAlgorithmTest.max_itrs):
                #Vary the Destination IP
                if dst == 1:
                    dst_ip_addr = socket.inet_ntoa(
                        binascii.unhexlify(format(dst_ip, 'x').zfill(8)))
                    pkt['IP'].dst = dst_ip_addr
                    exp_pkt1['IP'].dst = dst_ip_addr
                    exp_pkt2['IP'].dst = dst_ip_addr
                    exp_pkt3['IP'].dst = dst_ip_addr
                    exp_pkt4['IP'].dst = dst_ip_addr

                #Vary the Source IP
                if src == 1:
                    src_ip_addr = socket.inet_ntoa(
                        binascii.unhexlify(format(src_ip, 'x').zfill(8)))
                    pkt['IP'].src = src_ip_addr
                    exp_pkt1['IP'].src = src_ip_addr
                    exp_pkt2['IP'].src = src_ip_addr
                    exp_pkt3['IP'].src = src_ip_addr
                    exp_pkt4['IP'].src = src_ip_addr

                send_packet(self, self.devports[0], pkt)
                rcv_idx = verify_any_packet_any_port(
                    self, [exp_pkt1, exp_pkt2, exp_pkt3, exp_pkt4], [
                    self.devports[1], self.devports[2], self.devports[3], self.devports[4]
                    ], timeout=2)
                count[rcv_idx] += 1
                if src == 1:
                    src_ip += 1000
                if dst == 1:
                    dst_ip += 1000
            return count

        finally:
            pass

    def ipv6HashTraffic(self, src=0, dst=0):

        pkt = simple_tcpv6_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ipv6_dst='4000:5678:9abc:def0:4422:1133:5577:1111',
            ipv6_src='2000:5678:9abc:def0:4422:1133:5577:1111',
            ipv6_hlim=64)
        exp_pkt1 = simple_tcpv6_packet(
            eth_dst='00:22:22:33:44:' + str(self.port1 & 0xFF),
            eth_src='00:77:66:55:44:33',
            ipv6_dst='4000:5678:9abc:def0:4422:1133:5577:1111',
            ipv6_src='2000:5678:9abc:def0:4422:1133:5577:1111',
            ipv6_hlim=63)
        exp_pkt2 = simple_tcpv6_packet(
            eth_dst='00:22:22:33:44:' + str(self.port2 & 0xFF),
            eth_src='00:77:66:55:44:33',
            ipv6_dst='4000:5678:9abc:def0:4422:1133:5577:1111',
            ipv6_src='2000:5678:9abc:def0:4422:1133:5577:1111',
            ipv6_hlim=63)
        exp_pkt3 = simple_tcpv6_packet(
            eth_dst='00:22:22:33:44:' + str(self.port3 & 0xFF),
            eth_src='00:77:66:55:44:33',
            ipv6_dst='4000:5678:9abc:def0:4422:1133:5577:1111',
            ipv6_src='2000:5678:9abc:def0:4422:1133:5577:1111',
            ipv6_hlim=63)
        exp_pkt4 = simple_tcpv6_packet(
            eth_dst='00:22:22:33:44:' + str(self.port4 & 0xFF),
            eth_src='00:77:66:55:44:33',
            ipv6_dst='4000:5678:9abc:def0:4422:1133:5577:1111',
            ipv6_src='2000:5678:9abc:def0:4422:1133:5577:1111',
            ipv6_hlim=63)
        try:
            print('IPv6 hash test with', EcmpHashAlgorithmTest.max_itrs, 'packets')
            count = [0, 0, 0, 0]
            dst_ip = int(binascii.hexlify(socket.inet_pton(socket.AF_INET6, '4000:5678:9abc:def0:4422:1133:5577:1111')), 16)
            src_ip = int(binascii.hexlify(socket.inet_pton(socket.AF_INET6, '2000:5678:9abc:def0:4422:1133:5577:1111')), 16)
            for i in range(0, EcmpHashAlgorithmTest.max_itrs):
                #Vary the Destination IP
                if dst == 1:
                    dst_ip_addr = socket.inet_ntop(socket.AF_INET6,
                        binascii.unhexlify(format(dst_ip, 'x').zfill(8)))
                    pkt['IPv6'].dst = dst_ip_addr
                    exp_pkt1['IPv6'].dst = dst_ip_addr
                    exp_pkt2['IPv6'].dst = dst_ip_addr
                    exp_pkt3['IPv6'].dst = dst_ip_addr
                    exp_pkt4['IPv6'].dst = dst_ip_addr

                #Vary the Source IP
                if src == 1:
                    src_ip_addr = socket.inet_ntop(socket.AF_INET6,
                         binascii.unhexlify(format(src_ip, 'x').zfill(8)))
                    pkt['IPv6'].src = src_ip_addr
                    exp_pkt1['IPv6'].src = src_ip_addr
                    exp_pkt2['IPv6'].src = src_ip_addr
                    exp_pkt3['IPv6'].src = src_ip_addr
                    exp_pkt4['IPv6'].src = src_ip_addr

                send_packet(self, self.devports[0], pkt)
                rcv_idx = verify_any_packet_any_port(
                     self, [exp_pkt1, exp_pkt2, exp_pkt3, exp_pkt4], [
                     self.devports[1], self.devports[2], self.devports[3], self.devports[4]
                     ], timeout=2)
                count[rcv_idx] += 1
                if src == 1:
                    src_ip += 1000000
                if dst == 1:
                    dst_ip += 1000000
            return count
        finally:
            pass

###############################################################################
@group('hash')
class LagV6HashTest(ApiHelper, HashHelper):
    '''
    This performs V6 LAG load balancing
    @test - V6 LAG hash Test with all hash fields
    '''

    def setUp(self):
        print("")
        self.configure()

        #Configuring LAG
        self.lag0 = self.add_lag(self.device)
        self.lag0_mbr0 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port1)
        self.lag0_mbr1 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port2)
        self.lag0_mbr2 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port3)
        self.lag0_mbr3 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port4)

        #Populating the MAC table
        mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan10,
            mac_address='00:11:11:11:11:11', destination_handle=self.port0)
        mac1 = self.add_mac_entry(self.device, vlan_handle=self.vlan10,
            mac_address='00:22:22:22:22:22', destination_handle=self.lag0)

        #configuring the VLAN
        vlan_mbr0 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port0)
        vlan_mbr1 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.lag0)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.attribute_set(self.lag0, SWITCH_LAG_ATTR_PORT_VLAN_ID, 10)

        #Selecting all the parameters for fine-grained hash
        self.createLagHashList()

        #creating the hash_obj
        print("")
        print ("Setting the LAG hash object with all fields..")
        self.hash_obj = self.add_hash(self.device, field_list=self.hash_attr_list,
                                 fine_grained_field_list=self.fg_hash_obj_list)

        #Associating the hash obj to the device
        print("Setting the device object with V6 LAG Hash")
        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_LAG_IPV6_HASH, self.hash_obj)

    def runTest(self):
        try:
            self.basicHashTest()
            self.configIpv6FlowLabel()

        finally:
            pass

    def basicHashTest(self):
        print("")
        self.setTrafficParam(src=1)
        count = self.ipv6Traffic()
        print('ipv6-pkt-count:', count)
        self.verify_equaly_balanced(count, log="V6 LAG paths")

    def configIpv6FlowLabel(self):
        #Selecting ipv6_flow_label parameters
        prev_arr_list = self.hash_attr_list
        self.hash_attr_list = []
        attr_list = [SWITCH_HASH_ATTR_FIELD_IPV6_FLOW_LABEL]
        self.createLagHashList(field_name_list=attr_list)

        print("")
        print ("Setting the V6 LAG hash object with IPV6 Flow Label")
        self.attribute_set(self.hash_obj, SWITCH_HASH_ATTR_FIELD_LIST, self.hash_attr_list)

        self.setTrafficParam()
        count = self.ipv6Traffic()
        self.verify_single_port_traffic(count, log="V6 LAG paths")
        print('Count:', count)

    def tearDown(self):
        self.createLagHashList()
        print("")
        print ("Resetting the V6 Lag hash object to use all fields")
        self.attribute_set(self.hash_obj,
          SWITCH_HASH_ATTR_FIELD_LIST, self.hash_attr_list)

        self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.lag0, SWITCH_LAG_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_LAG_IPV6_HASH, 0)
        self.cleanup()


