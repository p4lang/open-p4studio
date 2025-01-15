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
Thrift BF_SWITCH API QOS queue, buffer, PPG and Scheduler basic tests
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
from switch_helpers import *
import model_utils as u

from ptf import config
from ptf.testutils import *
from ptf.packet import *
from ptf.thriftutils import *

import os
import ptf.mask

from bf_switcht_api_thrift.ttypes import *
from bf_switcht_api_thrift.model_headers import *

this_dir = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(this_dir, '..'))
from common.utils import *
import copy

SFLOW_SESSION_REASON_CODE = 0x1000
LOCAL_PORT_START = 8

###############################################################################

@group('sflow')
class SflowTest(ApiHelper):
    def runTest(self):

        if (self.client.is_feature_enable(SWITCH_FEATURE_SFLOW) == 0):
            print("SFLOW feature not enabled, skipping")
            return

        print("")
        self.configure()

        if self.arch == 'tofino':
            global LOCAL_PORT_START
            LOCAL_PORT_START = 0

        vlan_mbr0 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port0)
        vlan_mbr1 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port1)
        vlan_mbr1 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port2)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.attribute_set(self.port2, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:11:11:11:11:11', destination_handle=self.port0)
        mac1 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:22:22:22:22:22', destination_handle=self.port1)
        mac1 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:33:33:33:33:33', destination_handle=self.port2)

        queue_handles = self.attribute_get(self.cpu_port_hdl, SWITCH_PORT_ATTR_QUEUE_HANDLES)
        self.hostif_trap_group = self.add_hostif_trap_group(self.device,
            queue_handle=queue_handles[0].oid,
            admin_state=True)

        '''
    hostif_name0 = "test_host_if0"
    self.hostif0 = self.add_hostif(self.device, name=hostif_name0, handle=self.port0, oper_status=True)
    hostif_name1 = "test_host_if1"
    self.hostif1 = self.add_hostif(self.device, name=hostif_name1, handle=self.port1, oper_status=True)
    time.sleep(5)

    self.genl_hostif = self.add_hostif(self.device, type=SWITCH_HOSTIF_ATTR_TYPE_GENETLINK,
        name="psample", genl_mcgrp_name="packets")

    self.add_hostif_rx_filter(self.device, type=SWITCH_HOSTIF_RX_FILTER_ATTR_TYPE_TRAP,
        hostif_trap_handle=self.sflow_trap, hostif=self.genl_hostif)
    '''

        self.pkt = simple_udp_packet(
            eth_dst='00:33:33:33:33:33',
            eth_src='00:11:11:11:11:11',
            ip_dst='10.0.0.1',
            ip_ttl=64)
        cpu_pkt = simple_cpu_packet(
            ingress_port=self.devports[0],
            packet_type=0,
            ingress_ifindex=0xFFFF & self.port0,
            reason_code=SFLOW_SESSION_REASON_CODE + self.devports[0] - LOCAL_PORT_START, #SWITCH_HOSTIF_TRAP_ATTR_TYPE_SAMPLEPACKET,
            ingress_bd=0x0a,
            inner_pkt=self.pkt)
        self.cpu_pkt_port0 = cpu_packet_mask_ingress_bd(cpu_pkt)
        self.pkt1 = simple_udp_packet(
            eth_dst='00:33:33:33:33:33',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.0.0.1',
            ip_ttl=64)
        cpu_pkt = simple_cpu_packet(
            ingress_port=self.devports[1],
            packet_type=0,
            ingress_ifindex=0xFFFF & self.port1,
            reason_code=SFLOW_SESSION_REASON_CODE + self.devports[1] - LOCAL_PORT_START, #SWITCH_HOSTIF_TRAP_ATTR_TYPE_SAMPLEPACKET,
            ingress_bd=0x0a,
            inner_pkt=self.pkt1)
        self.cpu_pkt_port1 = cpu_packet_mask_ingress_bd(cpu_pkt)

        try:
            self.SflowBasicTest()
            self.SflowTrapActionTest()
            self.SflowExclusiveTest()
            self.SflowSharedTest()
        finally:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.port2, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.cleanup()

    def SflowBasicTest(self):
        print("SflowBasicTest()")
        rate = 10
        sflow_session = self.add_sflow_session(self.device,
            mode=SWITCH_SFLOW_SESSION_ATTR_MODE_EXCLUSIVE, sample_rate=rate)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_SFLOW_HANDLE, sflow_session)
        self.sflow_trap = self.add_hostif_trap(self.device,
            type=SWITCH_HOSTIF_TRAP_ATTR_TYPE_SAMPLEPACKET,
            hostif_trap_group_handle=self.hostif_trap_group,
            packet_action=SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_COPY_TO_CPU)

        try:
            print("10 packets sent")
            for i in range(0,rate):
                send_packet(self, self.devports[0], self.pkt)
                verify_packet(self, self.pkt, self.devports[2])
            print("11th packet sent to CPU")
            send_packet(self, self.devports[0], self.pkt)
            verify_each_packet_on_each_port(self, [self.cpu_pkt_port0, self.pkt],
                                           [self.cpu_port, self.devports[2]])
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_SFLOW_HANDLE, 0)
            print("10 packets sent, after sflow session removed")
            for i in range(0,rate):
                send_packet(self, self.devports[0], self.pkt)
                verify_packet(self, self.pkt, self.devports[2])
            print("11th packet not sent to CPU")
            send_packet(self, self.devports[0], self.pkt)
            verify_packets(self, self.pkt, [self.devports[2]])
        finally:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_SFLOW_HANDLE, 0)
            self.cleanlast()
            self.cleanlast()

    def SflowTrapActionTest(self):
        print("SflowTrapActionTest()")
        rate = 10
        sflow_session = self.add_sflow_session(self.device,
            mode=SWITCH_SFLOW_SESSION_ATTR_MODE_EXCLUSIVE, sample_rate=rate)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_SFLOW_HANDLE, sflow_session)
        self.sflow_trap = self.add_hostif_trap(self.device,
            type=SWITCH_HOSTIF_TRAP_ATTR_TYPE_SAMPLEPACKET,
            hostif_trap_group_handle=self.hostif_trap_group,
            packet_action=SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_COPY_TO_CPU)

        try:
            print("Test copy to cpu action")
            for i in range(0,rate):
                send_packet(self, self.devports[0], self.pkt)
                verify_packet(self, self.pkt, self.devports[2])
            send_packet(self, self.devports[0], self.pkt)
            verify_each_packet_on_each_port(self, [self.cpu_pkt_port0, self.pkt],
                                           [self.cpu_port, self.devports[2]])
            self.cleanlast()
            self.sflow_trap = self.add_hostif_trap(self.device,
                type=SWITCH_HOSTIF_TRAP_ATTR_TYPE_SAMPLEPACKET,
                hostif_trap_group_handle=self.hostif_trap_group,
                packet_action=SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_REDIRECT_TO_CPU)
            print("Test redirect to cpu action")
            for i in range(0,rate):
                send_packet(self, self.devports[0], self.pkt)
                verify_packet(self, self.pkt, self.devports[2])
            send_packet(self, self.devports[0], self.pkt)
            verify_packets(self, self.cpu_pkt_port0, [self.cpu_port])
        finally:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_SFLOW_HANDLE, 0)
            self.cleanlast()
            self.cleanlast()

    def SflowExclusiveTest(self):
        print("SflowExclusiveTest()")
        rate = 10
        self.sflow_trap = self.add_hostif_trap(self.device,
            type=SWITCH_HOSTIF_TRAP_ATTR_TYPE_SAMPLEPACKET,
            hostif_trap_group_handle=self.hostif_trap_group,
            packet_action=SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_COPY_TO_CPU)
        sflow_session = self.add_sflow_session(self.device,
            mode=SWITCH_SFLOW_SESSION_ATTR_MODE_EXCLUSIVE, sample_rate=rate)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_SFLOW_HANDLE, sflow_session)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_INGRESS_SFLOW_HANDLE, sflow_session)

        try:
            print("10 pkts each on port %d and port %d" % (self.devports[0], self.devports[1]))
            for i in range(0,rate):
                send_packet(self, self.devports[0], self.pkt)
                verify_packet(self, self.pkt, self.devports[2])
                send_packet(self, self.devports[1], self.pkt1)
                verify_packet(self, self.pkt1, self.devports[2])
            print("11th and 12th packets generate report")
            send_packet(self, self.devports[0], self.pkt)
            verify_each_packet_on_each_port(self, [self.cpu_pkt_port0, self.pkt],
                                           [self.cpu_port, self.devports[2]])
            send_packet(self, self.devports[1], self.pkt1)
            verify_each_packet_on_each_port(self, [self.cpu_pkt_port1, self.pkt1],
                                           [self.cpu_port, self.devports[2]])
        finally:
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_INGRESS_SFLOW_HANDLE, 0)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_SFLOW_HANDLE, 0)
            self.cleanlast()
            self.cleanlast()

    def SflowSharedTest(self):
        print("SflowSharedTest()")
        rate = 10
        self.sflow_trap = self.add_hostif_trap(self.device,
            type=SWITCH_HOSTIF_TRAP_ATTR_TYPE_SAMPLEPACKET,
            hostif_trap_group_handle=self.hostif_trap_group,
            packet_action=SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_COPY_TO_CPU)
        sflow_session = self.add_sflow_session(self.device,
            mode=SWITCH_SFLOW_SESSION_ATTR_MODE_SHARED, sample_rate=rate)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_SFLOW_HANDLE, sflow_session)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_INGRESS_SFLOW_HANDLE, sflow_session)

        # first 64 are reserved for exclusive sessions
        cpu_pkt = simple_cpu_packet(
            ingress_port=self.devports[0],
            packet_type=0,
            ingress_ifindex=0xFFFF & self.port0,
            reason_code=SFLOW_SESSION_REASON_CODE + 63 + (0xFF & sflow_session),
            ingress_bd=0x0a,
            inner_pkt=self.pkt)
        cpu_pkt_port0 = cpu_packet_mask_ingress_bd(cpu_pkt)
        cpu_pkt = simple_cpu_packet(
            ingress_port=self.devports[1],
            packet_type=0,
            ingress_ifindex=0xFFFF & self.port1,
            reason_code=SFLOW_SESSION_REASON_CODE + 63 + (0xFF & sflow_session),
            ingress_bd=0x0a,
            inner_pkt=self.pkt1)
        cpu_pkt_port1 = cpu_packet_mask_ingress_bd(cpu_pkt)

        try:
            print("5 pkts each on port %d and port %d" % (self.devports[0], self.devports[1]))
            for i in range(0,5):
                send_packet(self, self.devports[0], self.pkt)
                verify_packet(self, self.pkt, self.devports[2])
            for i in range(0,5):
                send_packet(self, self.devports[1], self.pkt1)
                verify_packet(self, self.pkt1, self.devports[2])
            print("11th packet generate report")
            send_packet(self, self.devports[0], self.pkt)
            verify_each_packet_on_each_port(self, [cpu_pkt_port0, self.pkt],
                                           [self.cpu_port, self.devports[2]])
            print("5 pkts each on port %d and port %d" % (self.devports[0], self.devports[1]))
            for i in range(0,5):
                send_packet(self, self.devports[0], self.pkt)
                verify_packet(self, self.pkt, self.devports[2])
            for i in range(0,5):
                send_packet(self, self.devports[1], self.pkt1)
                verify_packet(self, self.pkt1, self.devports[2])
            print("22th packet generate report")
            send_packet(self, self.devports[1], self.pkt1)
            verify_each_packet_on_each_port(self, [cpu_pkt_port1, self.pkt1],
                                           [self.cpu_port, self.devports[2]])
        finally:
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_INGRESS_SFLOW_HANDLE, 0)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_SFLOW_HANDLE, 0)
            self.cleanlast()
            self.cleanlast()

###############################################################################

@group('sflow')
class SflowTrapPriorityTest(ApiHelper):
    def runTest(self):

        if (self.client.is_feature_enable(SWITCH_FEATURE_SFLOW) == 0):
            print("SFLOW feature not enabled, skipping")
            return

        print("")
        self.configure()

        if self.arch == 'tofino':
            global LOCAL_PORT_START
            LOCAL_PORT_START = 0

        self.prefix_len = 16

        # create sflow session
        self.rate = 5
        sflow_session = self.add_sflow_session(self.device,
            mode=SWITCH_SFLOW_SESSION_ATTR_MODE_EXCLUSIVE, sample_rate=self.rate)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_SFLOW_HANDLE, sflow_session)

        # create l3 interface on self.port0
        self.rif0 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port0,
            vrf_handle=self.vrf10, src_mac=self.rmac)
        self.rif0_ip = '30.30.0.3'

        # create hostif for rif0
        rif0_hostif_name = "rif0"
        rif0_ip_addr = self.rif0_ip + '/' + str(self.prefix_len)
        self.rif0_hostif0 = self.add_hostif(self.device, name=rif0_hostif_name,
                 handle=self.port0, oper_status=True, ip_addr=rif0_ip_addr,  mac=self.rmac)
        self.assertTrue(self.rif0_hostif0 != 0)
        time.sleep(5)
        self.sock = open_packet_socket(rif0_hostif_name)

        # create trap group and traps
        queue_handles = self.attribute_get(self.cpu_port_hdl, SWITCH_PORT_ATTR_QUEUE_HANDLES)
        self.hostif_trap_group = self.add_hostif_trap_group(self.device, queue_handle=queue_handles[0].oid, admin_state=1)

        self.port0_ingress_ifindex = self.get_port_ifindex(self.port0)
        self.pkt = simple_arp_packet(arp_op=1, eth_dst=self.rmac, pktlen=100)

        try:
            self.ArpTest1()
            self.ArpTest2()
        finally:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_SFLOW_HANDLE, 0)
            self.cleanup()

    def ArpTest1(self):
        # arp has higher priority
        trap_list = [
            [SWITCH_HOSTIF_TRAP_ATTR_TYPE_ARP_REQUEST, SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_COPY_TO_CPU, 1],
          [SWITCH_HOSTIF_TRAP_ATTR_TYPE_SAMPLEPACKET, SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_COPY_TO_CPU, 2]
        ]
        traps = []
        for trap in trap_list:
            traps.append(self.add_hostif_trap(self.device, type=trap[0], priority=trap[2],
                        hostif_trap_group_handle=self.hostif_trap_group, packet_action=trap[1]))
        try:
            exp_pkt = simple_cpu_packet(
                packet_type=0,
                ingress_port=self.devports[0],
                ingress_ifindex=self.port0_ingress_ifindex,
                reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_ARP_REQUEST,
                ingress_bd=2,
                inner_pkt=self.pkt)
            self.exp_arpq_uc_pkt = cpu_packet_mask_ingress_bd(exp_pkt)

            print("5 arp requests sent to linux interface")
            for i in range(0, self.rate):
                send_packet(self, self.devports[0], self.pkt)
                verify_packets(self, self.exp_arpq_uc_pkt, [self.cpu_port])
                self.assertTrue(socket_verify_packet(self.pkt, self.sock))
            print("6th packet also sent to linux intf since arp has higher priority")
            send_packet(self, self.devports[0], self.pkt)
            verify_packets(self, self.exp_arpq_uc_pkt, [self.cpu_port])
            self.assertTrue(socket_verify_packet(self.pkt, self.sock))
        finally:
            self.cleanlast()
            self.cleanlast()

    def ArpTest2(self):
        # sflow has higher priority
        trap_list = [
            [SWITCH_HOSTIF_TRAP_ATTR_TYPE_ARP_REQUEST, SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_COPY_TO_CPU, 2],
          [SWITCH_HOSTIF_TRAP_ATTR_TYPE_SAMPLEPACKET, SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_COPY_TO_CPU, 1]
        ]
        traps = []
        for trap in trap_list:
            traps.append(self.add_hostif_trap(self.device, type=trap[0], priority=trap[2],
                        hostif_trap_group_handle=self.hostif_trap_group, packet_action=trap[1]))

        '''
        self.genl_hostif = self.add_hostif(self.device, type=SWITCH_HOSTIF_ATTR_TYPE_GENETLINK,
            name="genl_test", genl_mcgrp_name="genl_mcgrp0")
        self.add_hostif_rx_filter(self.device, type=SWITCH_HOSTIF_RX_FILTER_ATTR_TYPE_TRAP,
            hostif_trap_handle=traps[1], hostif=self.genl_hostif)
        '''

        try:
            exp_pkt = simple_cpu_packet(
                packet_type=0,
                ingress_port=self.devports[0],
                ingress_ifindex=self.port0_ingress_ifindex,
                reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_ARP_REQUEST,
                ingress_bd=2,
                inner_pkt=self.pkt)
            self.exp_arpq_uc_pkt = cpu_packet_mask_ingress_bd(exp_pkt)
            cpu_sflow_pkt = simple_cpu_packet(
                packet_type=0,
                ingress_port=self.devports[0],
                ingress_ifindex=self.port0_ingress_ifindex,
                reason_code=SFLOW_SESSION_REASON_CODE + self.devports[0] - LOCAL_PORT_START, #SWITCH_HOSTIF_TRAP_ATTR_TYPE_SAMPLEPACKET,
                ingress_bd=0x0a,
                inner_pkt=self.pkt)
            self.exp_sflow_pkt = cpu_packet_mask_ingress_bd(cpu_sflow_pkt)

            print("5 arp requests sent to linux interface")
            for i in range(0, self.rate):
                send_packet(self, self.devports[0], self.pkt)
                verify_packets(self, self.exp_arpq_uc_pkt, [self.cpu_port])
                self.assertTrue(socket_verify_packet(self.pkt, self.sock))
            print("6th packet sent to genl since sflow has higher priority")
            send_packet(self, self.devports[0], self.pkt)
            verify_packets(self, self.exp_sflow_pkt, [self.cpu_port])
        finally:
            #self.cleanlast()
            #self.cleanlast()
            self.cleanlast()
            self.cleanlast()
