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
import math
import ipaddress

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
from devport_mgr_pd_rpc.ttypes import *

this_dir = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(this_dir, '..'))
from common.utils import *

###############################################################################


class breakOut(Exception):
    pass

@group('scale')
@disabled
class PortScaleTest(ApiHelper):
    def setUp(self):
        super(self.__class__, self).setUp()
        if (self.client.is_feature_enable(SWITCH_FEATURE_ASYMMETRIC_FOLDED_PIPELINE) == 1
            or self.client.is_feature_enable(SWITCH_FEATURE_FOLDED_SWITCH_PIPELINE) == 1):
            print("AFP/FP - not all ports are external, skipping")
            return
        self.device = self.get_device_handle(0)
        self.ports = []
        self.devports = []
        if ptf.testutils.test_params_get()['target'] == 'hw':
            return
        max_ports = 256
        start = 0
        self.vlan10 = self.add_vlan(self.device, vlan_id=10)
        if self.client.thrift_ports_present():
            self.port_list = self.client.port_list_get(self.device)
            start = 16
            for i, port in zip(list(range(0, len(self.port_list))), self.port_list):
                self.ports.append(port)
                self.devports.append(self.attribute_get(port, SWITCH_PORT_ATTR_DEV_PORT))
        for i in range(start, max_ports):
            port = self.add_port(self.device, lane_list=u.lane_list_t([i]), speed=25000)
            self.assertEqual(self.status(), 0)
            self.ports.append(port)
            self.devports.append(self.attribute_get(port, SWITCH_PORT_ATTR_DEV_PORT))
        for i in range(0, max_ports):
            self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.ports[i])
            self.assertEqual(self.status(), 0)
            self.attribute_set(self.ports[i], SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
            self.add_mac_entry(self.device, vlan_handle=self.vlan10,
                mac_address='00:11:11:11:11:'+str(hex(i)[2:]), destination_handle=self.ports[i])
    def runTest(self):
        try:
            self.trafficTest()
        finally:
            pass

    def trafficTest(self):
        if ptf.testutils.test_params_get()['target'] == 'hw':
            return
        if (self.client.is_feature_enable(SWITCH_FEATURE_ASYMMETRIC_FOLDED_PIPELINE) == 1
            or self.client.is_feature_enable(SWITCH_FEATURE_FOLDED_SWITCH_PIPELINE) == 1):
            return
        self.i_pkt_count = 0
        self.e_pkt_count = 0
        self.client.object_counters_clear_all(self.vlan10)
        for i in range(1,16):
            pkt = simple_udp_packet(
                eth_dst='00:11:11:11:11:'+str(hex(i)[2:]),
                eth_src='00:11:11:11:11:00',
                ip_dst='10.0.0.1',
                ip_ttl=64)
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, pkt, self.devports[i])
            self.i_pkt_count += 1
            self.e_pkt_count += 1
        time.sleep(4)
        cntrs = self.client.object_counters_get(self.vlan10)
        # Lookup the unicast packet count
        for cntr in cntrs:
            if cntr.counter_id == SWITCH_VLAN_COUNTER_ID_IN_UCAST_PKTS:
                self.assertEqual(cntr.count, self.i_pkt_count)
            if cntr.counter_id == SWITCH_VLAN_COUNTER_ID_OUT_UCAST_PKTS:
                self.assertEqual(cntr.count, self.e_pkt_count)

    def tearDown(self):
        if (self.client.is_feature_enable(SWITCH_FEATURE_ASYMMETRIC_FOLDED_PIPELINE) == 0
            and self.client.is_feature_enable(SWITCH_FEATURE_FOLDED_SWITCH_PIPELINE) == 0):
            for port in self.ports:
                self.attribute_set(port, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.cleanup()

###############################################################################

@group('scale')
@disabled
class LagScaleTest(ApiHelper):
    def setUp(self):
        self.configure()
        if (self.client.is_feature_enable(SWITCH_FEATURE_ASYMMETRIC_FOLDED_PIPELINE) == 1
            or self.client.is_feature_enable(SWITCH_FEATURE_FOLDED_SWITCH_PIPELINE) == 1):
            print("AFP/FP - not all ports are external, skipping")
            return
        max_ports = 250
        self.lags = []
        for i in range(0, max_ports):
            self.lags.append(self.add_lag(self.device))
            self.assertEqual(self.status(), 0)
        self.random_lags = []
        randomlist = random.sample(list(range(0, max_ports)), 16)
        print(randomlist)
        i = 0
        # add 16 ports to random lags
        for port in self.port_list:
            random_lag = self.lags[randomlist[i]]
            self.random_lags.append(random_lag)
            self.add_lag_member(self.device, lag_handle=random_lag, port_handle=port)
            self.assertEqual(self.status(), 0)
            self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=random_lag)
            self.assertEqual(self.status(), 0)
            self.add_mac_entry(self.device, vlan_handle=self.vlan10,
                mac_address='00:11:11:11:11:'+str(hex(i)[2:]), destination_handle=random_lag)
            self.attribute_set(random_lag, SWITCH_LAG_ATTR_PORT_VLAN_ID, 10)
            i += 1
        # add rest of the ports to other lags
        for lag in self.lags:
            if lag in self.random_lags:
                continue
            port = self.add_port(self.device, lane_list=u.lane_list_t([i]), speed=25000)
            self.assertEqual(self.status(), 0)
            self.add_lag_member(self.device, lag_handle=lag, port_handle=port)
            self.assertEqual(self.status(), 0)
            self.add_vlan_member(self.device, vlan_handle=self.vlan20, member_handle=lag)
            self.assertEqual(self.status(), 0)
            i += 1
    def runTest(self):
        if (self.client.is_feature_enable(SWITCH_FEATURE_ASYMMETRIC_FOLDED_PIPELINE) == 1
            or self.client.is_feature_enable(SWITCH_FEATURE_FOLDED_SWITCH_PIPELINE) == 1):
            return
        self.i_pkt_count = 0
        self.e_pkt_count = 0
        self.client.object_counters_clear_all(self.vlan10)
        for i in range(1,16):
            pkt = simple_udp_packet(
                eth_dst='00:11:11:11:11:'+str(hex(i)[2:]),
                eth_src='00:11:11:11:11:00',
                ip_dst='10.0.0.1',
                ip_ttl=64)
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, pkt, self.devports[i])
            self.i_pkt_count += 1
            self.e_pkt_count += 1
        time.sleep(4)
        cntrs = self.client.object_counters_get(self.vlan10)
        # Lookup the unicast packet count
        for cntr in cntrs:
            if cntr.counter_id == SWITCH_VLAN_COUNTER_ID_IN_UCAST_PKTS:
                self.assertEqual(cntr.count, self.i_pkt_count)
            if cntr.counter_id == SWITCH_VLAN_COUNTER_ID_OUT_UCAST_PKTS:
                self.assertEqual(cntr.count, self.e_pkt_count)
    def tearDown(self):
        if (self.client.is_feature_enable(SWITCH_FEATURE_ASYMMETRIC_FOLDED_PIPELINE) == 0
            and self.client.is_feature_enable(SWITCH_FEATURE_FOLDED_SWITCH_PIPELINE) == 0):
            for lag in self.random_lags:
                self.attribute_set(lag, SWITCH_LAG_ATTR_PORT_VLAN_ID, 0)
        self.cleanup()

###############################################################################

# requires more testing before enabling
@disabled
class RifScaleTest(ApiHelper):
    def runTest(self):
        print()
        if ptf.testutils.test_params_get()['target'] == 'hw':
            return
        if (self.client.is_feature_enable(SWITCH_FEATURE_ASYMMETRIC_FOLDED_PIPELINE) == 1
            or self.client.is_feature_enable(SWITCH_FEATURE_FOLDED_SWITCH_PIPELINE) == 1):
            print("AFP/FP - not all ports are external, skipping")
            return
        try:
            # 5k entries in total should be possible
            self.device = self.get_device_handle(0)
            mac = '00:11:22:33:44:%02x' % 0
            vrf = self.add_vrf(self.device, id=10, src_mac=mac)
            ports = []
            start = 0
            # 256 L3 RIFs
            if self.client.thrift_ports_present():
                ports = self.client.port_list_get(self.device)
                start = 16
            for i in range(start, 256):
                port_hdl = self.add_port(self.device, lane_list=u.lane_list_t([i]), speed=25000)
                ports.append(port_hdl)
                self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=port_hdl, vrf_handle=vrf, src_mac=mac)
                self.assertEqual(self.status(), 0, "Failed to insert port RIF id {}".format(i))
            # 256 subports on a single port
            # not 768 because 10 entries used by internal ports
            for vlan_id in range(512, 758):
                self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_SUB_PORT, port_handle=ports[0], outer_vlan_id=vlan_id, vrf_handle=vrf, src_mac=mac)
                self.assertEqual(self.status(), 0, "Failed to insert subport RIF on port id {}".format(i))
            # 256 subports on a single vlan 100
            for port in ports:
                self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_SUB_PORT, port_handle=port, outer_vlan_id=1024, vrf_handle=vrf, src_mac=mac)
                self.assertEqual(self.status(), 0, "Failed to insert subport RIF on vlan id {}".format(i))
        finally:
            self.cleanup()

###############################################################################

@group('scale')
@disabled
class VlanScaleTest(ApiHelper):
    def runTest(self):
        print()
        if ptf.testutils.test_params_get()['target'] == 'hw':
            return
        if (self.client.is_feature_enable(SWITCH_FEATURE_ASYMMETRIC_FOLDED_PIPELINE) == 1
            or self.client.is_feature_enable(SWITCH_FEATURE_FOLDED_SWITCH_PIPELINE) == 1):
            print("AFP/FP - not all ports are external, skipping")
            return
        try:
            # 5k entries in total should be possible
            self.device = self.get_device_handle(0)
            # 4k VLANs
            for i in range(2, 4090):
                self.add_vlan(self.device, vlan_id=i)
                self.assertEqual(self.status(), 0, "Failed to insert vlan id {}".format(i))
        finally:
            self.cleanup()


###############################################################################

@group('scale')
@disabled
class VlanMemberScaleTest(ApiHelper):
    def runTest(self):
        print()
        if ptf.testutils.test_params_get()['target'] == 'hw':
            return
        if (self.client.is_feature_enable(SWITCH_FEATURE_ASYMMETRIC_FOLDED_PIPELINE) == 1
            or self.client.is_feature_enable(SWITCH_FEATURE_FOLDED_SWITCH_PIPELINE) == 1):
            print("AFP/FP - not all ports are external, skipping")
            return
        try:
            # 5k entries in total should be possible
            self.device = self.get_device_handle(0)
            mac = '00:11:22:33:44:%02x' % 0
            ports = []
            vlans = []
            start = 0
            # 128 ports
            if self.client.thrift_ports_present():
                ports = self.client.port_list_get(self.device)
                start = 16
            for i in range(start, 128):
                ports.append(self.add_port(self.device, lane_list=u.lane_list_t([i]), speed=25000))
            # 4k VLANs
            for i in range(2, 4092):
                vlans.append(self.add_vlan(self.device, vlan_id=i))
                self.assertEqual(self.status(), 0, "Failed to create vlan id {}".format(i))
            for port in range(0, 128):
                for vlan_id in range(2, 4090):
                    #print(port, vlan_id)
                    self.add_vlan_member(self.device,
                                         vlan_handle=vlans[vlan_id],
                                         member_handle=ports[port],
                                         tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
                    self.assertEqual(self.status(), 0, "Failed to create vlan member for port {} vlan {}".format(port, vlan_id))
        finally:
            self.cleanup()

###############################################################################

@group('scale')
@disabled
class NexthopScaleTest(ApiHelper):
    def setUp(self):
        print()

        self.configure()
        self.rif_list = []
        for x, port_oid in zip(list(range(0, len(self.port_list))), self.port_list):
            rif_oid = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=port_oid, vrf_handle=self.vrf10, src_mac=self.rmac)
            self.rif_list.append(rif_oid)

    def runTest(self):
        try:
            nhop_table_info = self.client.table_info_get(SWITCH_DEVICE_ATTR_TABLE_NEXTHOP)
            neigh_table_info = self.client.table_info_get(SWITCH_DEVICE_ATTR_TABLE_NEIGHBOR)
            mac_list = generate_mac_addresses(neigh_table_info.size)
            nhop_ip = int(binascii.hexlify(socket.inet_aton('11.11.11.1')), 16)
            for i in range(0, nhop_table_info.size - 2):
              rif = self.rif_list[random.randint(0, len(self.rif_list) - 1)]
              nhop_ip_addr = socket.inet_ntoa(binascii.unhexlify(format(nhop_ip, 'x').zfill(8)))
              self.add_nexthop(self.device, handle=rif, dest_ip=nhop_ip_addr)
              self.assertEqual(self.status(), 0)
              self.add_neighbor(self.device, mac_address=mac_list[i], handle=rif, dest_ip=nhop_ip_addr)
              self.assertEqual(self.status(), 0)
              nhop_ip += 1
        finally:
            nhop_table_info = self.client.table_info_get(SWITCH_DEVICE_ATTR_TABLE_NEXTHOP)
            print("NEXTHOP: size {} usage {}".format(nhop_table_info.size, nhop_table_info.usage))
            neigh_table_info = self.client.table_info_get(SWITCH_DEVICE_ATTR_TABLE_NEIGHBOR)
            print("NEIGHBOR: size {} usage {}".format(neigh_table_info.size, neigh_table_info.usage))
            pass

    def tearDown(self):
        self.cleanup()

###############################################################################

@group('scale')
@disabled
class ECMPScaleTest(ApiHelper):
    def setUp(self):
        print()

        self.configure()

        # ingress port
        self.rif0 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port0, vrf_handle=self.vrf10, src_mac=self.rmac)

    def runTest(self):
        try:
            # DRV-5243
            self.EcmpScaleTest()
            self.EcmpUniformFillTest()
            # re-enable after move to nightly
            #self.EcmpUniformFillTest2()
            #self.EcmpRandomFillTest()
        finally:
            pass

    def tearDown(self):
        self.cleanup()

    def EcmpScaleTest(self):
        print("EcmpScaleTest()")
        max_group_size = 64 # this should come from P4
        try:
            ecmp_scale = self.add_ecmp(self.device)
            nhop = self.add_nexthop(self.device, handle=self.rif0, dest_ip='13.13.13.2')
            for _ in range(0, max_group_size):
                self.add_ecmp_member(self.device, nexthop_handle=nhop, ecmp_handle=ecmp_scale)
        finally:
            for _ in range(0, max_group_size):
                self.cleanlast()
            self.cleanlast()
            self.cleanlast()

    def EcmpUniformFillTest(self):
        print("EcmpUniformFillTest()")
        '''
        create max ecmp groups and fill each group with 2 members
        '''
        i = 0
        try:
            nhop = self.add_nexthop(self.device, handle=self.rif0, dest_ip='13.13.13.2')
            ecmp_group_size = self.client.table_info_get(SWITCH_DEVICE_ATTR_TABLE_ECMP_GROUP)
            ecmp_group_members_size = self.client.table_info_get(SWITCH_DEVICE_ATTR_TABLE_ECMP_GROUP_MEMBERS)
            members = int(ecmp_group_members_size.size / ecmp_group_size.size)
            # 2 less for already created
            dst_ip = int(binascii.hexlify(socket.inet_aton('10.100.10.1')), 16)
            for _ in range(0, ecmp_group_size.size - 2):
                ecmp_scale = self.add_ecmp(self.device)
                i += 1
                self.add_ecmp_member(self.device, nexthop_handle=nhop, ecmp_handle=ecmp_scale)
                assert(self.status() == 0)
                dst_ip_addr = socket.inet_ntoa(binascii.unhexlify(format(dst_ip, 'x').zfill(8)))
                nhop2 = self.add_nexthop(self.device, handle=self.rif0, dest_ip=dst_ip_addr)
                assert(self.status() == 0)
                self.add_ecmp_member(self.device, nexthop_handle=nhop2, ecmp_handle=ecmp_scale)
                assert(self.status() == 0)
                i += 3
                dst_ip += 1
        finally:
            for _ in range(0, i):
                self.cleanlast()
            self.cleanlast()

    def EcmpUniformFillTest2(self):
        print("EcmpUniformFillTest2()")
        '''
        create max ecmp groups and fill each group with average members
        '''
        i = 0
        try:
            nhop_table_info = self.client.table_info_get(SWITCH_DEVICE_ATTR_TABLE_NEXTHOP)
            ecmp_group_size = self.client.table_info_get(SWITCH_DEVICE_ATTR_TABLE_ECMP_GROUP)
            ecmp_group_members_size = self.client.table_info_get(SWITCH_DEVICE_ATTR_TABLE_ECMP_GROUP_MEMBERS)
            members = int(ecmp_group_members_size.size / ecmp_group_size.size)

            nhop = self.add_nexthop(self.device, handle=self.rif0, dest_ip='13.13.13.2')
            # 2 less for already created
            dst_ip = int(binascii.hexlify(socket.inet_aton('10.100.10.1')), 16)
            for _ in range(0, ecmp_group_size.size - 2):
                ecmp_scale = self.add_ecmp(self.device)
                i += 1
                for _ in range(0, members):
                    dst_ip_addr = socket.inet_ntoa(binascii.unhexlify(format(dst_ip, 'x').zfill(8)))
                    nhop2 = self.add_nexthop(self.device, handle=self.rif0, dest_ip=dst_ip_addr)
                    assert(self.status() == 0)
                    self.add_ecmp_member(self.device, nexthop_handle=nhop2, ecmp_handle=ecmp_scale)
                    assert(self.status() == 0)
                    i += 2
                    dst_ip += 1
        finally:
            for _ in range(0, i):
                self.cleanlast()
            self.cleanlast()

    def EcmpRandomFillTest(self):
        print("EcmpRandomFillTest()")
        # test fails due to fragmentation
        return
        i = 0
        try:
            nhop = self.add_nexthop(self.device, handle=self.rif0, dest_ip='13.13.13.2')
            ecmp_group_size = self.client.table_info_get(SWITCH_DEVICE_ATTR_TABLE_ECMP_GROUP)
            ecmp_group_members_size = self.client.table_info_get(SWITCH_DEVICE_ATTR_TABLE_ECMP_GROUP_MEMBERS)
            groups = []
            members = 0
            while members < (ecmp_group_members_size.size - 4):
                count = random.randint(0, 64)
                members += count
                groups.append(count)
            for group in groups:
                ecmp_scale = self.add_ecmp(self.device)
                assert(self.status() == 0)
                i += 1
                for _ in range(0, group):
                    self.add_ecmp_member(self.device, nexthop_handle=nhop, ecmp_handle=ecmp_scale)
                    assert(self.status() == 0)
                    i += 1
        finally:
            for i in range(0, i):
                self.cleanlast()
            self.cleanlast()

###############################################################################

@group('scale')
@disabled
class RouteScaleTest2(ApiHelper):
    def toIPv4(self, addr, prefix):
        addr4 = [0, 0, 0, 0]
        fmt = "%d.%d.%d.%d/%d"
        addr4[0] = (addr & 0xFF000000) >> 24
        addr4[1] = (addr & 0x00FF0000) >> 16
        addr4[2] = (addr & 0x0000FF00) >> 8
        addr4[3] = (addr & 0x000000FF)
        addr = fmt % (addr4[0], addr4[1], addr4[2], addr4[3], prefix)
        return addr

    def toIPv6(self, i, prefix):
        addr6 = [0, 0, 0 ,0 ,0 ,0, 0, 0]
        fmt = "%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x/%d"
        half = int(prefix / 16)
        offset = prefix % 16
        if offset == 0:
            addr6[half - 1] = i
        else:
            val = i << (16 - offset)
            addr6[half] = val & 0xFFFF
            addr6[half - 1] = val >> 16
        addr = fmt % (addr6[0], addr6[1], addr6[2], addr6[3], addr6[4], addr6[5], addr6[6], addr6[7], prefix)
        return addr

    def genRoutePrefix(self, routeDict):
        v4_list = []
        v6_list = []
        for item in routeDict["v4"]:
            for prefix, count in list(item.items()):
                for i in range(1, count + 1):
                    x = i << (32 - prefix)
                    v4_list.append(self.toIPv4(x, prefix))
        for item in routeDict["v6"]:
            for prefix, count in list(item.items()):
                for i in range(1, count + 1):
                    v6_list.append(self.toIPv6(i, prefix))
        return v4_list, v6_list

    def runTest(self):
        print()
        leaf_routes = {
            "v6": [
                {46: 96},
               {54: 624},
               {66: 96},
               {57: 16},
               {59: 96},
               {60: 96},
               {64: 3718},
               {127: 128},
               {128: 100}
            ],
            "v4": [
                {19: 80},
               {24: 592},
               {26: 1},
               {31: 128},
               {32: 2176}
            ]
        }
        spine_routes = {
            "v6": [
                {48: 100},
               {52: 200},
               {56: 100},
               {64: 3550},
               {80: 300},
               {96: 200},
               {112: 100},
               {127: 100},
               {128: 3350}
            ],
            "v4": [
                {15: 200},
               {24: 2000},
               {26: 1000},
               {28: 200},
               {31: 100},
               {32: 4500}
            ]
        }
        alpm_routes = {
            "v6": [
                {48: 200},
               {52: 200},
               {56: 200},
               {64: 10000},
               {80: 200},
               {96: 200},
               {112: 200},
               {120: 200},
               {128: 10000}
            ],
            "v4": [
                {15: 400},
               {24: 400},
               {26: 400},
               {28: 400},
               {30: 400},
               {32: 10000}
            ]
        }
        self.configure()
        self.rif0 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port0, vrf_handle=self.vrf10, src_mac=self.rmac)
        self.rif1 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port1, vrf_handle=self.vrf10, src_mac=self.rmac)
        nhop = self.add_nexthop(self.device, handle=self.rif1, dest_ip='10.10.0.2')
        self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=self.rif1, dest_ip='10.10.0.2')

        try:
            v4_host = self.client.table_info_get(SWITCH_DEVICE_ATTR_TABLE_IPV4_HOST)
            v4_lpm  = self.client.table_info_get(SWITCH_DEVICE_ATTR_TABLE_IPV4_LPM)
            v6_host = self.client.table_info_get(SWITCH_DEVICE_ATTR_TABLE_IPV6_HOST)
            v6_lpm  = self.client.table_info_get(SWITCH_DEVICE_ATTR_TABLE_IPV6_LPM)
            print("IPV4 Host/LPM: %d usage: %d", v4_host.size, v4_lpm.size)
            print("IPV6 Host/LPM: %d usage: %d", v6_host.size, v6_lpm.size)

            v4, v6 = self.genRoutePrefix(leaf_routes)
            v4, v6 = self.genRoutePrefix(spine_routes)
            v4, v6 = self.genRoutePrefix(alpm_routes)

            for prefix in v4:
                self.add_route(self.device, ip_prefix=prefix, vrf_handle=self.vrf10, nexthop_handle=nhop)
            for prefix in v6:
                self.add_route(self.device, ip_prefix=prefix, vrf_handle=self.vrf10, nexthop_handle=nhop)
        finally:
            self.cleanup()

###############################################################################

@group('scale')
@disabled
class RouteScaleTest(ApiHelper):
    expected_util = 80

    def runTest(self):
        print()
        self.configure()

        try:
            with open(this_dir+"/ipv4_route_table.txt","r") as v4, open(this_dir + "/ipv6_route_table.txt","r") as v6,\
                open(this_dir + "/ipv6_lpm64_route_table.txt","r") as v6_lpm:
                self.ipv4_pre = v4.readlines()
                self.ipv6_pre = v6.readlines()
                self.ipv6_lpm64 = v6_lpm.readlines()
            #random.shuffle(self.ipv4_pre)
            #random.shuffle(self.ipv6_pre)
            #random.shuffle(self.ipv6_lpm64)

            self.nhop_list = []
            for port in self.port_list:
                nhop_ip = '20.20.20.' + str(port & 0xFFFF)
                neigh_mac = '00:11:22:33:44:' + str(port & 0xFFFF)
                rif0 = self.add_rif(
                    self.device,
                    type=SWITCH_RIF_ATTR_TYPE_PORT,
                    port_handle=port,
                    vrf_handle=self.vrf10,
                    src_mac=self.rmac)
                self.nhop_list.append(
                    self.add_nexthop(
                        self.device, handle=rif0, dest_ip=nhop_ip))
                self.add_neighbor(
                    self.device,
                    mac_address=neigh_mac,
                    handle=rif0,
                    dest_ip=nhop_ip)
            self.MacScaleTest()
            self.HostV4ScaleTest()
            self.LpmV4ScaleTest()
            self.HostV6ScaleTest1()
            self.HostV6ScaleTest2()
            self.LpmV6ScaleTest()
        finally:
            self.cleanup()

    def MacScaleTest(self):
        print()
        print("MacScaleTest()")
        table_info = self.client.table_info_get(SWITCH_DEVICE_ATTR_TABLE_DMAC)
        print("DMAC table size: %d usage: %d" % (table_info.size,
                                                 table_info.usage))
        if table_info.size == 0:
            return
        try:
            mac_prefix = '00:22:'
            for mac1 in range(1, 256):
                for vlan, vlan_id in zip(
                    [self.vlan10, self.vlan20, self.vlan30, self.vlan40],
                    [10, 20, 30, 40]):
                    for port in self.port_list:
                        for mac2 in range(1, 256):
                            mac_address = mac_prefix + str(
                                format(mac1, '02x')) + ":" + str(
                                    format(mac2, '02x')) + ":" + str(
                                        vlan_id) + ":" + str(port & 0xFFFF)
                            self.add_mac_entry(
                                self.device,
                                vlan_handle=vlan,
                                mac_address=mac_address,
                                destination_handle=port)
                            if self.status() != 0:
                                print("MAC add failed at mac ", mac_address)
                                raise breakOut
        except breakOut:
            pass
        finally:
            table_info = self.client.table_info_get(
                SWITCH_DEVICE_ATTR_TABLE_DMAC)
            print("DMAC table size: %d usage: %d" % (table_info.size,
                                                     table_info.usage))
            print("Table fill data: %.2f%%" % ((
                float(table_info.usage) / float(table_info.size)) * 100))
            util = table_info.usage * 100.0/table_info.size
            print("Utilization: %.2f%%" % (util))
            if util < self.expected_util:
                self.assertTrue(0,
                 "Table Utilization failed to meet the Expected Utilization bar of %.2f%%" %(self.expected_util))

    def HostV4ScaleTest(self):
        print()
        print("HostV4ScaleTest()")
        table_info = self.client.table_info_get(
            SWITCH_DEVICE_ATTR_TABLE_IPV4_HOST)
        print("IPV4 Host table size: %d usage: %d" % (table_info.size,
                                                      table_info.usage))
        if table_info.size == 0:
            return
        try:
            ip_pre = '10.'
            for ip1 in range(1, 256):
                for nhop in self.nhop_list:
                    for ip2 in range(1, 256):
                        ip_address = ip_pre + str(ip1) + "." + str(
                            nhop & 0xFFFF) + "." + str(ip2)
                        #for each indivadual IP, create a route and loop through all nhop created
                        self.add_route(
                            self.device,
                            ip_prefix=ip_address,
                            vrf_handle=self.vrf10,
                            nexthop_handle=nhop)
                        if self.status() != 0:
                            print("IPv4 host add failed at IP ", ip_address)
                            raise breakOut
        except breakOut:
            pass
        finally:
            table_info = self.client.table_info_get(
                SWITCH_DEVICE_ATTR_TABLE_IPV4_HOST)
            print("IPV4 Host table size: %d usage: %d" % (table_info.size,
                                                          table_info.usage))
            print("Table fill data: %.2f%%" % ((
                float(table_info.usage) / float(table_info.size)) * 100))
            util = table_info.usage * 100.0/table_info.size
            print("Utilization: %.2f%%" % (util))
            if util < self.expected_util:
                self.assertTrue(0,
                 "Table Utilization failed to meet the Expected Utilization bar of %.2f%%" %(self.expected_util))

    def partitionSpace(self, size):
        return math.ceil(float(size) / float(14))

    def LpmV4ScaleTest(self):
        print()
        print("LpmV4ScaleTest()")
        table_info = self.client.table_info_get(
            SWITCH_DEVICE_ATTR_TABLE_IPV4_LPM)
        print("IPV4 LPM table size: %d usage: %d" % (table_info.size,
                                                     table_info.usage))
        if table_info.size == 0:
            return
        try:
            for ip in self.ipv4_pre:
                ip_address = ip.strip().replace(" ","/")
                try: ipaddress.ip_network(ip_address)
                except: continue
                lpmV4 = self.add_route(
                    self.device,
                    ip_prefix=ip_address,
                    vrf_handle=self.vrf10,
                    nexthop_handle=random.choice(self.nhop_list))
                if self.status() != 0:
                    print("IPv4 LPM add failed at IP ", ip_address)
                    raise breakOut
        except breakOut:
            pass
        finally:
            table_info = self.client.table_info_get(
                SWITCH_DEVICE_ATTR_TABLE_IPV4_LPM)
            print("IPV4 LPM table size: %d usage: %d" % (table_info.size,
                                                         table_info.usage))
            print("Table fill data: %.2f%%" % ((
                float(table_info.usage) / float(table_info.size)) * 100))
            util = table_info.usage * 100.0/table_info.size
            print("Utilization: %.2f%%" % (util))
            if util < self.expected_util:
                self.assertTrue(0,
                 "Table Utilization failed to meet the Expected Utilization bar of %.2f%%" %(self.expected_util))

    def HostV6ScaleTest1(self):
        print()
        print("HostV6ScaleTest1() - Test IPv6 addresses with upto 6 bytes variance")
        table_info = self.client.table_info_get(
            SWITCH_DEVICE_ATTR_TABLE_IPV6_HOST)
        print("IPV6 Host table size: %d usage: %d" % (table_info.size,
                                                      table_info.usage))
        if table_info.size == 0:
            return
        try:
            ip_pre = '1234:5678:9abc:def0:4422:'
            print("Seed IP: ", ip_pre)
            for ip1 in range(1, 256):
                for nhop in self.nhop_list:
                    for ip2 in range(1, 256):
                        ip_address = ip_pre + str(ip1) + ":" + str(
                            nhop & 0xFFFF) + ":" + str(ip2)
                        route = self.add_route(
                            self.device,
                            ip_prefix=ip_address,
                            vrf_handle=self.vrf10,
                            nexthop_handle=nhop)
                        if self.status() != 0:
                            print("IPv6 host add failed at IP ", ip_address)
                            raise breakOut
        except breakOut:
            pass
        finally:
            table_info = self.client.table_info_get(
                SWITCH_DEVICE_ATTR_TABLE_IPV6_HOST)
            print("IPV6 Host table size: %d usage: %d" % (table_info.size,
                                                          table_info.usage))
            print("Table fill data: %.2f%%" % ((
                float(table_info.usage) / float(table_info.size)) * 100))

            util = table_info.usage * 100.0/table_info.size
            print("Utilization: %.2f%%" % (util))
            if util < self.expected_util:
                self.assertTrue(0,
                 "Table Utilization failed to meet the Expected Utilization bar of %.2f%%" %(self.expected_util))
            for _ in range(0, table_info.usage+1):
                self.cleanlast()

    def HostV6ScaleTest2(self):
        print()
        print("HostV6ScaleTest2() - Test IPv6 addresses with <1 bytes variance")
        table_info = self.client.table_info_get(
            SWITCH_DEVICE_ATTR_TABLE_IPV6_HOST)
        print("IPV6 Host table size: %d usage: %d" % (table_info.size,
                                                      table_info.usage))
        if table_info.size == 0:
            return
        try:
            ip_pre = '7:1:1::'
            print("Seed IP: ", ip_pre)
            count = 0
            for ip1 in range(1, 256):
                for nhop in self.nhop_list:
                    for ip2 in range(1, 256):
                        count += 1
                        ip_address = ip_pre + str(hex(count))[2:]
                        route = self.add_route(
                            self.device,
                            ip_prefix=ip_address,
                            vrf_handle=self.vrf10,
                            nexthop_handle=nhop)
                        if self.status() != 0:
                            print("IPv6 host add failed at IP ", ip_address)
                            raise breakOut
        except breakOut:
            pass
        finally:
            table_info = self.client.table_info_get(
                SWITCH_DEVICE_ATTR_TABLE_IPV6_HOST)
            print("IPV6 Host table size: %d usage: %d" % (table_info.size,
                                                          table_info.usage))
            print("Table fill data: %.2f%%" % ((
                float(table_info.usage) / float(table_info.size)) * 100))

            util = table_info.usage * 100.0/table_info.size
            print("Utilization: %.2f%%" % (util))
            if util < self.expected_util:
                self.assertTrue(0,
                 "Table Utilization failed to meet the Expected Utilization bar of %.2f%%" %(self.expected_util))

    def LpmV6ScaleTest(self):
        print()
        print("LpmV6ScaleTest()")
        self.vrf20 = self.add_vrf(self.device, id=20, src_mac=self.rmac)
        lpm = self.client.table_info_get(SWITCH_DEVICE_ATTR_TABLE_IPV6_LPM)
        lpm_tcam = self.client.table_info_get(SWITCH_DEVICE_ATTR_TABLE_IPV6_LPM_TCAM)
        lpm64 = self.client.table_info_get(SWITCH_DEVICE_ATTR_TABLE_IPV6_LPM64)
        print("IPV6 LPM table size: %d usage: %d" % (lpm.size, lpm.usage))
        print("IPV6 LPM_TCAM table size: %d usage: %d" % (lpm_tcam.size, lpm_tcam.usage))
        print("IPV6 LPM64 table size: %d usage: %d" % (lpm64.size, lpm64.usage))
        try:
            print("== Adding to V6 LPM ==")
            for ip in self.ipv6_pre:
                lpmV6 = self.add_route(
                    self.device,
                    ip_prefix=ip,
                    vrf_handle=self.vrf10,
                    nexthop_handle=random.choice(self.nhop_list))
                if self.status() != 0:
                    print("IPv6 LPM add failed at IP ", ip)
                    break

            print("== Adding to V6 LPM 64 ==")
            # the ipv6 lmp64 table dump is only 146K long. Add to multiple vrfs for larger scale profiles
            for ip in self.ipv6_lpm64:
                if int(ip.split('/')[1]) > 64:
                    continue
                self.add_route(
                    self.device,
                    ip_prefix=ip,
                    vrf_handle=self.vrf10,
                    nexthop_handle=random.choice(self.nhop_list))
                if self.status() != 0:
                    print("IPv6 LPM64 add failed at vrf 10 IP ", ip)
                    break
                self.add_route(
                    self.device,
                    ip_prefix=ip,
                    vrf_handle=self.vrf20,
                    nexthop_handle=random.choice(self.nhop_list))
                if self.status() != 0:
                    print("IPv6 LPM64 add failed at vrf 20 IP ", ip)
                    break

        except breakOut:
            pass
        finally:
            lpm = self.client.table_info_get(SWITCH_DEVICE_ATTR_TABLE_IPV6_LPM)
            lpm_tcam = self.client.table_info_get(SWITCH_DEVICE_ATTR_TABLE_IPV6_LPM_TCAM)
            lpm64 = self.client.table_info_get(SWITCH_DEVICE_ATTR_TABLE_IPV6_LPM64)
            print("IPV6 LPM table size: %d usage: %d" % (lpm.size, lpm.usage))
            print("IPV6 LPM_TCAM table size: %d usage: %d" % (lpm_tcam.size, lpm_tcam.usage))
            print("IPV6 LPM64 table size: %d usage: %d" % (lpm64.size, lpm64.usage))
            lpm_util = ((lpm.usage + lpm_tcam.usage) *100.0) / (lpm.size + lpm_tcam.size)
            print("LPM Utilization: %.2f%%" % (lpm_util))
            lpm64_util = (lpm64.usage *100.0) / lpm64.size
            print("LPM64 Utilization: %.2f%%" % (lpm64_util))
            if lpm64_util < self.expected_util:
                self.assertTrue(0,
                 "Table Utilization failed to meet the Expected Utilization bar of %.2f%%" %(self.expected_util))
