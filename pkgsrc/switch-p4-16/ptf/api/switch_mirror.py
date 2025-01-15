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

import ptf.dataplane as dataplane
import api_base_tests
import pd_base_tests
from switch_helpers import ApiHelper
import model_utils as u
from p4testutils.misc_utils import mask_set_do_not_care_packet

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
from common.pktpy_utils import pktpy_skip  # noqa pylint: disable=wrong-import-position

pkt_len = int(test_param_get('pkt_size'))

class MirrorTestHelper(ApiHelper):
    port0_pkt = None
    port1_pkt = None
    num_ports = 9

    def queue_id_to_handle(self, port_handle, qid):
        queue_list = self.attribute_get(port_handle, SWITCH_PORT_ATTR_QUEUE_HANDLES)
        for queue_handle in queue_list:
            queue_id = self.attribute_get(queue_handle.oid, SWITCH_QUEUE_ATTR_QUEUE_ID)
            if qid == queue_id:
                return queue_handle.oid
        print("Invalid queue id: %d, queue_handle does not exists" % qid)
        return 0

    def clearQueueStats(self, port_handle, queue_id):
        queue_handle = self.queue_id_to_handle(port_handle, queue_id)
        self.client.object_counters_clear_all(queue_handle)

    def verifyQueueStats(self, port_handle, queue_id, num_packets):
        queue_handle = self.queue_id_to_handle(port_handle, queue_id)
        queue_cntrs = self.object_counters_get(queue_handle)
        for counter in queue_cntrs:
            if counter.counter_id == SWITCH_QUEUE_COUNTER_ID_STAT_PACKETS:
                print("Queue count %d"%(counter.count))
                self.assertTrue(counter.count == num_packets)
        self.clearQueueStats(port_handle, queue_id)

    def setUp(self):
        print()
        self.configure()

        self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_LEARNING, False)

        for idx, port in enumerate(self.port_list[:self.num_ports]):
            self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=port)
            self.attribute_set(port, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
            self.add_mac_entry(self.device, vlan_handle=self.vlan10,
                               mac_address=('00:%02x:11:11:11:11' % idx), destination_handle=port)

        self.port0_pkt = simple_udp_packet(
            pktlen=pkt_len,
            eth_dst='00:01:11:11:11:11',
            eth_src='00:00:11:11:11:11',
            ip_dst='10.0.0.1',
            ip_ttl=64)

        self.port1_pkt = simple_udp_packet(
            pktlen=pkt_len,
            eth_dst='00:00:11:11:11:11',
            eth_src='00:01:11:11:11:11',
            ip_dst='10.0.0.1',
            ip_ttl=64)

    def tearDown(self):
        for port in self.port_list[:self.num_ports]:
            self.attribute_set(port, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_MIRROR_HANDLE, 0)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_EGRESS_MIRROR_HANDLE, 0)
        self.cleanup()


@group('mirror')
class CpuMirrorTest(MirrorTestHelper):
    def setUp(self):
        super(CpuMirrorTest, self).setUp()
        self.port0_ingress_ifindex = self.get_port_ifindex(self.port0)
        self.port1_ingress_ifindex = self.get_port_ifindex(self.port1)

        #self.hostif_name1 = "test_host_if0"
        #self.hostif0 = self.add_hostif(self.device, name=self.hostif_name1, handle=self.port0, oper_status=True)
        #self.assertTrue(self.hostif0 != 0)

        queue_handles = self.attribute_get(self.cpu_port_hdl, SWITCH_PORT_ATTR_QUEUE_HANDLES)
        self.hostif_trap_group0 = self.add_hostif_trap_group(self.device, queue_handle=queue_handles[0].oid, admin_state=True)
        self.add_hostif_trap(self.device, type=SWITCH_HOSTIF_TRAP_ATTR_TYPE_LLDP,
                             hostif_trap_group_handle=self.hostif_trap_group0,
                             packet_action=SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_REDIRECT_TO_CPU,
                             priority=100)

    def tearDown(self):
        super(CpuMirrorTest, self).tearDown()

    def runTest(self):
        try:
            self.IngressLocalMirrorTest()
            self.CPUIngressLocalMirrorTest()
            self.CPUEgressLocalMirrorTest()
            self.EgressLocalMirrorTest()
        finally:
            pass

    def CPUIngressLocalMirrorTest(self):
        print("CPUIngressLocalMirrorTest()")
        self.ing_mirror = self.add_mirror(self.device,
            type=SWITCH_MIRROR_ATTR_TYPE_LOCAL,
            direction=SWITCH_MIRROR_ATTR_DIRECTION_INGRESS,
            egress_port_handle=self.port2)

        pkt = simple_tcp_packet(
              eth_dst='00:11:11:11:11:11',
              eth_src='00:00:00:00:00:01',
              ip_dst='192.17.10.1',
	          ip_ttl=64)
        cpu_pkt = simple_cpu_packet(
	              dst_device=0,
	              ingress_ifindex=0xFFFF & self.port1,
	              ingress_port=0,
	              ingress_bd=0,
	              tx_bypass=True,
	              reason_code=0xFFFF,
	              inner_pkt=pkt)

        try:
            self.attribute_set(self.cpu_port_hdl, SWITCH_PORT_ATTR_INGRESS_MIRROR_HANDLE, self.ing_mirror)
            print("Local mirror packet from CPU -> 2, mirror from cpu")
            send_packet(self, self.cpu_port, cpu_pkt)
            if self.test_params['target'] != 'hw':
                verify_each_packet_on_each_port(self, [pkt,cpu_pkt], [self.devports[1], self.devports[2]])
        finally:
            self.attribute_set(self.cpu_port_hdl, SWITCH_PORT_ATTR_INGRESS_MIRROR_HANDLE, 0)
            self.cleanlast()

    def CPUEgressLocalMirrorTest(self):
        print("CPUEgressLocalMirrorTest()")
        self.eg_mirror = self.add_mirror(self.device,
            type=SWITCH_MIRROR_ATTR_TYPE_LOCAL,
            direction=SWITCH_MIRROR_ATTR_DIRECTION_EGRESS,
            egress_port_handle=self.port2)

        lldp_pkt = simple_eth_packet(eth_dst='01:80:C2:00:00:0e', pktlen=100)
        exp_pkt = simple_cpu_packet(
              packet_type=0,
              ingress_port=self.devports[0],
              ingress_ifindex=self.port0_ingress_ifindex,
              reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_LLDP,
              ingress_bd=2,
              inner_pkt=lldp_pkt)
        exp_lldp_cpu_pkt = cpu_packet_mask_ingress_bd_and_ifindex(exp_pkt)

        try:
            self.attribute_set(self.cpu_port_hdl, SWITCH_PORT_ATTR_EGRESS_MIRROR_HANDLE, self.eg_mirror)
            print("Local egress mirror packet from 0 -> CPU, mirror to port 2")
            if self.test_params['target'] != 'hw':
                send_packet(self, self.devports[0], lldp_pkt)
                verify_each_packet_on_each_port(self, [exp_lldp_cpu_pkt, exp_lldp_cpu_pkt], [self.devports[2],
                self.cpu_port])
        finally:
            self.attribute_set(self.cpu_port_hdl, SWITCH_PORT_ATTR_EGRESS_MIRROR_HANDLE, 0)
            self.cleanlast()

    def IngressLocalMirrorTest(self):
        print("IngressLocalMirrorTest()")
        self.ing_mirror = self.add_mirror(self.device,
            type=SWITCH_MIRROR_ATTR_TYPE_LOCAL,
            direction=SWITCH_MIRROR_ATTR_DIRECTION_INGRESS,
            egress_port_handle=self.cpu_port_hdl)

        # mirrored packet will have reason code 0 and ingress port as inport
        exp_pkt = simple_cpu_packet(
            packet_type=0,
            ingress_port=self.devports[0],
            ingress_ifindex=self.port0_ingress_ifindex,
            reason_code=0,
            ingress_bd=2,
            inner_pkt=self.port0_pkt)
        cpu_pkt = cpu_packet_mask_ingress_bd_and_ifindex(exp_pkt)

        lldp_pkt = simple_eth_packet(eth_dst='01:80:C2:00:00:0e', pktlen=100)
        exp_pkt = simple_cpu_packet(
              packet_type=0,
              ingress_port=self.devports[0],
              ingress_ifindex=self.port0_ingress_ifindex,
              reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_LLDP,
              ingress_bd=2,
              inner_pkt=lldp_pkt)
        exp_lldp_cpu_pkt = cpu_packet_mask_ingress_bd_and_ifindex(exp_pkt)
        exp_pkt = simple_cpu_packet(
              packet_type=0,
              ingress_port=self.devports[0],
              ingress_ifindex=self.port0_ingress_ifindex,
              reason_code=0,
              ingress_bd=2,
              inner_pkt=lldp_pkt)
        exp_lldp_mirr_pkt = cpu_packet_mask_ingress_bd_and_ifindex(exp_pkt)
        try:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_MIRROR_HANDLE, self.ing_mirror)
            print("Local mirror packet from 0 -> 1, mirror to cpu")
            send_packet(self, self.devports[0], self.port0_pkt)
            if self.test_params['target'] != 'hw':
              verify_each_packet_on_each_port(self, [self.port0_pkt, cpu_pkt], [self.devports[1], self.cpu_port])
            else:
              verify_packets(self, self.port0_pkt, [self.devports[1]])

            if self.test_params['target'] != 'hw':
              print("Local lldp packet from 0, 2 copies to cpu")
              send_packet(self, self.devports[0], lldp_pkt)
              verify_multiple_packets_on_ports(self, [
                  (self.cpu_port, [exp_lldp_cpu_pkt, exp_lldp_mirr_pkt])
              ])
              #verify_packets(self, exp_lldp_cpu_pkt, [self.cpu_port, self.cpu_port])
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_MIRROR_HANDLE, 0)

            print("Mirror session removed, packet from 0 -> 1 only")
            send_packet(self, self.devports[0], self.port0_pkt)
            verify_packets(self, self.port0_pkt, [self.devports[1]])

            print("Mirror session removed, only 1 lldp pkt to cpu")
            send_packet(self, self.devports[0], lldp_pkt)
            verify_packets(self, exp_lldp_cpu_pkt, [self.cpu_port])
        finally:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_MIRROR_HANDLE, 0)
            self.cleanlast()

    def EgressLocalMirrorTest(self):
        print("EgressLocalMirrorTest()")
        if (self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_PORT_MIRROR) == 0):
            print("Egress mirror feature not enabled, skipping")
            return

        self.eg_mirror = self.add_mirror(self.device,
            type=SWITCH_MIRROR_ATTR_TYPE_LOCAL,
            direction=SWITCH_MIRROR_ATTR_DIRECTION_EGRESS,
            egress_port_handle=self.cpu_port_hdl)

        # mirrored packet will have reason code 0 and egress port as inport
        exp_pkt = simple_cpu_packet(
            packet_type=0,
            ingress_port=self.devports[0],
            ingress_ifindex=self.port0_ingress_ifindex,
            reason_code=0,
            ingress_bd=2,
            inner_pkt=self.port1_pkt)
        cpu_pkt = cpu_packet_mask_ingress_bd_and_ifindex(exp_pkt)
        try:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_EGRESS_MIRROR_HANDLE, self.eg_mirror)
            print("Local mirror packet from 1 -> 0, mirror to cpu")
            send_packet(self, self.devports[1], self.port1_pkt)
            if self.test_params['target'] != 'hw':
              verify_each_packet_on_each_port(self, [self.port1_pkt, cpu_pkt], [self.devports[0], self.cpu_port])
            else:
              verify_packets(self, self.port1_pkt, [self.devports[0]])
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_EGRESS_MIRROR_HANDLE, 0)

            print("Mirror session removed, packet from 1 -> 0 only ")
            send_packet(self, self.devports[1], self.port1_pkt)
            verify_packets(self, self.port1_pkt, [self.devports[0]])
        finally:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_EGRESS_MIRROR_HANDLE, 0)
            self.cleanlast()


@group('mirror')
class LocalMirrorTest(MirrorTestHelper):
    def setUp(self):
        super(LocalMirrorTest, self).setUp()
        # IngressLocalMirrorTest()
        if(self.client.is_feature_enable(SWITCH_FEATURE_MIRROR_METER)):
            self.mirror_meter_feature = 1
        else:
            self.mirror_meter_feature = 0

        self.mirror_queue_id = 5
        if self.mirror_meter_feature == 1:
            self.ingress_mirror_meter = self.add_meter(self.device,
                mode=SWITCH_METER_ATTR_MODE_TWO_RATE_THREE_COLOR,
                cbs=10,cir=10,
                type=SWITCH_METER_ATTR_TYPE_PACKETS,
                color_source=SWITCH_METER_ATTR_COLOR_SOURCE_BLIND)
            self.ing_mirror = self.add_mirror(self.device,
                type=SWITCH_MIRROR_ATTR_TYPE_LOCAL,
                direction=SWITCH_MIRROR_ATTR_DIRECTION_INGRESS,
                egress_port_handle=self.port2, meter_handle=self.ingress_mirror_meter,
                queue_id=self.mirror_queue_id)
        else:
            self.ing_mirror = self.add_mirror(self.device,
                type=SWITCH_MIRROR_ATTR_TYPE_LOCAL,
                direction=SWITCH_MIRROR_ATTR_DIRECTION_INGRESS,
                egress_port_handle=self.port2,
                queue_id=self.mirror_queue_id)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_MIRROR_HANDLE, self.ing_mirror)

        if (self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_PORT_MIRROR)):
            # EgressLocalMirrorTest(), EgressLocalMirrorPacketDropTest()

            if self.mirror_meter_feature == 1:
                self.egress_mirror_meter = self.add_meter(self.device,
                    mode=SWITCH_METER_ATTR_MODE_TWO_RATE_THREE_COLOR,
                    cbs=10,cir=10,
                    type=SWITCH_METER_ATTR_TYPE_PACKETS,
                    color_source=SWITCH_METER_ATTR_COLOR_SOURCE_BLIND)

                self.eg_mirror = self.add_mirror(self.device,
                    type=SWITCH_MIRROR_ATTR_TYPE_LOCAL,
                    direction=SWITCH_MIRROR_ATTR_DIRECTION_EGRESS,
                    egress_port_handle=self.port2, meter_handle=self.egress_mirror_meter,
                    queue_id=self.mirror_queue_id)
            else:
                self.eg_mirror = self.add_mirror(self.device,
                    type=SWITCH_MIRROR_ATTR_TYPE_LOCAL,
                    direction=SWITCH_MIRROR_ATTR_DIRECTION_EGRESS,
                    egress_port_handle=self.port2,
                    queue_id=self.mirror_queue_id)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_EGRESS_MIRROR_HANDLE, self.eg_mirror)

            # IngressEgressLocalMirrorTest()
            if self.arch == 'tofino':
                self.both_mirror = self.add_mirror(self.device,
                    type=SWITCH_MIRROR_ATTR_TYPE_LOCAL,
                    direction=SWITCH_MIRROR_ATTR_DIRECTION_BOTH,
                    egress_port_handle=self.port2)

    def tearDown(self):
        super(LocalMirrorTest, self).tearDown()

    def runTest(self):
        try:
            self.IngressLocalMirrorTest()
            self.EgressLocalMirrorTest()
            self.EgressLocalMirrorPacketDropTest()
            if self.arch == 'tofino':
                self.IngressEgressLocalMirrorTest()
        finally:
            pass

    def IngressLocalMirrorTest(self):
        try:
            print("Local mirror packet from 0 -> 1, mirror to 2")
            self.clearQueueStats(self.port2, self.mirror_queue_id)
            send_packet(self, self.devports[0], self.port0_pkt)
            verify_packets(self, self.port0_pkt, [self.devports[1], self.devports[2]])
            self.verifyQueueStats(self.port2, self.mirror_queue_id, 1)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_MIRROR_HANDLE, 0)
            if self.mirror_meter_feature:
                counter = self.object_counters_get(self.ingress_mirror_meter)
                print (counter)
                pkt_counter = 0
                for i in range(0, 3):
                    pkt_counter += counter[i].count
                self.assertEqual(pkt_counter, 1)

            print("Mirror session removed, packet from 0 -> 1 only")
            send_packet(self, self.devports[0], self.port0_pkt)
            verify_packets(self, self.port0_pkt, [self.devports[1]])
            self.verifyQueueStats(self.port2, self.mirror_queue_id, 0)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_MIRROR_HANDLE, self.ing_mirror)
        finally:
            pass

    def EgressLocalMirrorTest(self):
        print("EgressLocalMirrorTest()")
        if (self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_PORT_MIRROR) == 0):
            print("Egress mirror feature not enabled, skipping")
            return

        try:
            print("Local mirror packet from 1 -> 0, mirror to 2")
            self.clearQueueStats(self.port2, self.mirror_queue_id)
            send_packet(self, self.devports[1], self.port1_pkt)
            verify_packets(self, self.port1_pkt, [self.devports[0], self.devports[2]])
            self.verifyQueueStats(self.port2, self.mirror_queue_id, 1)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_EGRESS_MIRROR_HANDLE, 0)
            if self.mirror_meter_feature:
                counter = self.object_counters_get(self.egress_mirror_meter)
                pkt_counter = 0
                for i in range(0, 3):
                    pkt_counter += counter[i].count
                self.assertEqual(pkt_counter, 1)

            print("Mirror session removed, packet from 1 -> 0 only ")
            send_packet(self, self.devports[1], self.port1_pkt)
            verify_packets(self, self.port1_pkt, [self.devports[0]])
            self.verifyQueueStats(self.port2, self.mirror_queue_id, 0)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_EGRESS_MIRROR_HANDLE, self.eg_mirror)
        finally:
            pass

    def EgressLocalMirrorPacketDropTest(self):
        print("EgressLocalMirrorPacketDropTest()")
        if (self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_PORT_MIRROR) == 0
            or self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_IP_ACL) == 0):
            print("Egress mirror or egress IP ACL feature not enabled, skipping")
            return

        acl_table_bp_port = switcht_list_val_t(type=switcht_value_type.ENUM, enumdata=SWITCH_ACL_TABLE_ATTR_BIND_POINT_TYPE_PORT)
        acl_table = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IPV4,
            bind_point_type=[acl_table_bp_port],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_EGRESS)
        acl_entry = self.add_acl_entry(self.device,
            dst_ip='10.0.0.1',
            dst_ip_mask='255.255.255.255',
            packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_DROP,
            table_handle=acl_table)

        try:
            print("Local mirror packet from 1 -> 0, mirror to 2")
            send_packet(self, self.devports[1], self.port1_pkt)
            verify_packets(self, self.port1_pkt, [self.devports[0], self.devports[2]])

            # Apply ACL to deny packet
            self.attribute_set(
                self.port0, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, acl_table)

            print("Deny packet from 1 -> 0, should be dropped and not mirrored")
            send_packet(self, self.devports[1], self.port1_pkt)
            verify_no_other_packets(self, timeout=2)
        finally:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, 0)
            self.cleanlast()
            self.cleanlast()

    def IngressEgressLocalMirrorTest(self):
        print("IngressEgressLocalMirrorTest()")
        if (self.client.is_feature_enable(SWITCH_FEATURE_EGRESS_PORT_MIRROR) == 0):
            print("Egress mirror feature not enabled, skipping")
            return

        try:
            print("Using same mirror session both ingress and egress mirroring on different ports")
            print("Ingress mirror packet from 0 -> 1, mirror to 2")
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_MIRROR_HANDLE, 0)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_MIRROR_HANDLE, self.both_mirror)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_EGRESS_MIRROR_HANDLE, 0)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_EGRESS_MIRROR_HANDLE, self.both_mirror)
            try:
                send_packet(self, self.devports[0], self.port0_pkt)
                verify_packets(self, self.port0_pkt, [self.devports[1], self.devports[2]])

                print("Egress mirror packet from 1 -> 0, mirror to 2")
                send_packet(self, self.devports[1], self.port1_pkt)
                verify_packets(self, self.port1_pkt, [self.devports[0], self.devports[2]])
            finally:
                self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_MIRROR_HANDLE, self.ing_mirror)
                self.attribute_set(self.port0, SWITCH_PORT_ATTR_EGRESS_MIRROR_HANDLE, self.eg_mirror)
        finally:
            pass


class RemoteMirrorHelper(MirrorTestHelper):
    def __init__(self, erspan_type, platform_info=False):
        super(RemoteMirrorHelper, self).__init__()

        self.erspan_type = erspan_type
        self.platform_info = platform_info
        self.sgt_other = 0
        if erspan_type == SWITCH_MIRROR_ATTR_ERSPAN_TYPE_ERSPAN_2:
            self.erspan_version = 1
            self.erspan_gra = 0
            self.erspan_o = 0
        else:
            self.erspan_version = 2
            self.erspan_gra = 2
            self.erspan_o = 1 if platform_info else 0

        self.mirror0 = None
        self.egr_mirror0 = None
        self.mirror1 = None
        self.mirror2 = None
        self.mirror3 = None
        self.mirror4 = None
        self.port0_pkt = self.exp_mirrored_port0_pkt = None
        self.port0_pkt1 = self.exp_mirrored_port0_pkt1 = None
        self.port1_pkt = self.exp_mirrored_port1_pkt = None
        self.port2_pkt = self.exp_mirrored_port2_pkt = None
        self.port3_pkt = self.exp_mirrored_port3_pkt = None
        self.port4_pkt = self.exp_mirrored_port4_pkt = None

    def setUp(self):
        super(RemoteMirrorHelper, self).setUp()

        self.mirror_queue_id = 5
        self.port0_pkt = simple_udp_packet(
            pktlen=pkt_len,
            eth_dst='00:04:11:11:11:11',
            eth_src='00:00:11:11:11:11',
            ip_dst='10.0.0.1',
            ip_ttl=64)

        self.port0_pkt1 = simple_udp_packet(
            pktlen=pkt_len,
            eth_dst='00:03:11:11:11:11',
            eth_src='00:00:11:11:11:11',
            ip_dst='10.0.0.1',
            ip_ttl=64)

        self.port1_pkt = simple_udp_packet(
            pktlen=pkt_len,
            eth_dst='00:04:11:11:11:11',
            eth_src='00:01:11:11:11:11',
            ip_dst='10.0.0.1',
            ip_ttl=64)

        self.port2_pkt = simple_udp_packet(
            pktlen=pkt_len,
            eth_dst='00:04:11:11:11:11',
            eth_src='00:02:11:11:11:11',
            ip_dst='10.0.0.1',
            ip_ttl=64)

        self.pktlen_trunk = 256
        self.port5_pkt = simple_udp_packet(
            pktlen=self.pktlen_trunk,
            eth_dst='00:05:11:11:11:11',
            eth_src='00:00:11:11:11:11',
            ip_dst='10.0.0.1',
            ip_ttl=64)
        self.port6_pkt = simple_udp_packet(
            pktlen=self.pktlen_trunk,
            eth_dst='00:06:11:11:11:11',
            eth_src='00:00:11:11:11:11',
            ip_dst='10.0.0.1',
            ip_ttl=64)

        self.mirror0 = self.add_mirror(self.device,
            type=SWITCH_MIRROR_ATTR_TYPE_ENHANCED_REMOTE,
            direction=SWITCH_MIRROR_ATTR_DIRECTION_INGRESS,
            rspan_type=SWITCH_MIRROR_ATTR_RSPAN_TYPE_NONE,
            erspan_type=self.erspan_type,
            egress_port_handle=self.port7,
            ttl=64,
            platform_info=self.platform_info,
            src_mac_address='00:33:33:33:33:33',
            dest_mac_address='00:44:44:44:44:44',
            src_ip='10.10.10.1',
            dest_ip='20.20.20.1', queue_id=self.mirror_queue_id)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_MIRROR_HANDLE, self.mirror0)
        session_id = self.attribute_get(self.mirror0, SWITCH_MIRROR_ATTR_SESSION_ID)

        self.exp_mirrored_port0_pkt = ptf.mask.Mask(ipv4_erspan_platform_pkt(
            pktlen=pkt_len,
            eth_dst='00:44:44:44:44:44',
            eth_src='00:33:33:33:33:33',
            ip_dst='20.20.20.1',
            ip_src='10.10.10.1',
            ip_id=0,
            ip_ttl=64,
            ip_flags=0x0,
            version=self.erspan_version,
            mirror_id=session_id,
            sgt_other=self.sgt_other,
            erspan_gra=self.erspan_gra,
            erspan_o=self.erspan_o,
            inner_frame=self.port0_pkt))
        # IPv4.total_length is different on Model and Hardware because of 4B CRC.
        if test_param_get('target') != "asic-model":
            mask_set_do_not_care_packet(self.exp_mirrored_port0_pkt, IP, "len")
        mask_set_do_not_care_packet(self.exp_mirrored_port0_pkt, IP, "chksum")
        if self.erspan_version == 2:
            mask_set_do_not_care_packet(self.exp_mirrored_port0_pkt, ERSPAN_III, "timestamp")

        self.mirror1 = self.add_mirror(self.device,
            type=SWITCH_MIRROR_ATTR_TYPE_ENHANCED_REMOTE,
            direction=SWITCH_MIRROR_ATTR_DIRECTION_INGRESS,
            rspan_type=SWITCH_MIRROR_ATTR_RSPAN_TYPE_VLAN_ID,
            erspan_type=self.erspan_type,
            vlan_id=50,
            egress_port_handle=self.port7,
            ttl=64,
            platform_info=self.platform_info,
            src_mac_address='00:33:33:33:33:33',
            dest_mac_address='00:44:44:44:44:44',
            src_ip='10.10.10.1',
            dest_ip='20.20.20.1', queue_id=self.mirror_queue_id)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_INGRESS_MIRROR_HANDLE, self.mirror1)
        session_id = self.attribute_get(self.mirror1, SWITCH_MIRROR_ATTR_SESSION_ID)

        self.exp_mirrored_port1_pkt = ptf.mask.Mask(ipv4_erspan_platform_pkt(
            pktlen=pkt_len,
            eth_dst='00:44:44:44:44:44',
            eth_src='00:33:33:33:33:33',
            ip_dst='20.20.20.1',
            ip_src='10.10.10.1',
            dl_vlan_enable=True,
            vlan_vid=50,
            ip_id=0,
            ip_ttl=64,
            ip_flags=0x0,
            version=self.erspan_version,
            mirror_id=session_id,
            sgt_other=self.sgt_other,
            erspan_gra=self.erspan_gra,
            erspan_o=self.erspan_o,
            inner_frame=self.port1_pkt))
        # IPv4.total_length is different on Model and Hardware because of 4B CRC.
        if test_param_get('target') != "asic-model":
            mask_set_do_not_care_packet(self.exp_mirrored_port1_pkt, IP, "len")
        mask_set_do_not_care_packet(self.exp_mirrored_port1_pkt, IP, "chksum")
        if self.erspan_version == 2:
            mask_set_do_not_care_packet(self.exp_mirrored_port1_pkt, ERSPAN_III, "timestamp")

        self.mirror2 = self.add_mirror(self.device,
            type=SWITCH_MIRROR_ATTR_TYPE_ENHANCED_REMOTE,
            direction=SWITCH_MIRROR_ATTR_DIRECTION_INGRESS,
            rspan_type=SWITCH_MIRROR_ATTR_RSPAN_TYPE_VLAN_HANDLE,
            erspan_type=self.erspan_type,
            vlan_handle=self.vlan40,
            egress_port_handle=self.port7,
            ttl=64,
            platform_info=self.platform_info,
            src_mac_address='00:33:33:33:33:33',
            dest_mac_address='00:44:44:44:44:44',
            src_ip='10.10.10.1',
            dest_ip='20.20.20.1', queue_id=self.mirror_queue_id)
        self.attribute_set(self.port2, SWITCH_PORT_ATTR_INGRESS_MIRROR_HANDLE, self.mirror2)
        session_id = self.attribute_get(self.mirror2, SWITCH_MIRROR_ATTR_SESSION_ID)

        self.exp_mirrored_port2_pkt = ptf.mask.Mask(ipv4_erspan_platform_pkt(
            pktlen=pkt_len,
            eth_dst='00:44:44:44:44:44',
            eth_src='00:33:33:33:33:33',
            ip_dst='20.20.20.1',
            ip_src='10.10.10.1',
            dl_vlan_enable=True,
            vlan_vid=40,
            ip_id=0,
            ip_ttl=64,
            ip_flags=0x0,
            version=self.erspan_version,
            mirror_id=session_id,
            sgt_other=self.sgt_other,
            erspan_gra=self.erspan_gra,
            erspan_o=self.erspan_o,
            inner_frame=self.port2_pkt))
        # IPv4.total_length is different on Model and Hardware because of 4B CRC.
        if test_param_get('target') != "asic-model":
            mask_set_do_not_care_packet(self.exp_mirrored_port2_pkt, IP, "len")
        mask_set_do_not_care_packet(self.exp_mirrored_port2_pkt, IP, "chksum")
        if self.erspan_version == 2:
            mask_set_do_not_care_packet(self.exp_mirrored_port2_pkt, ERSPAN_III, "timestamp")

        # this is a totally non-random value
        # any other value may cause test to fail on one of tf 1/2/3
        # it is good enough to just allow the tests to pass on all tf
        truncate_size = 131
        max_pkt_len = truncate_size

        self.mirror5 = self.add_mirror(self.device,
            max_pkt_len=max_pkt_len,
            type=SWITCH_MIRROR_ATTR_TYPE_ENHANCED_REMOTE,
            direction=SWITCH_MIRROR_ATTR_DIRECTION_INGRESS,
            rspan_type=SWITCH_MIRROR_ATTR_RSPAN_TYPE_NONE,
            session_type=SWITCH_MIRROR_ATTR_SESSION_TYPE_TRUNCATE,
            erspan_type=self.erspan_type,
            egress_port_handle=self.port7,
            ttl=64,
            platform_info=self.platform_info,
            src_mac_address='00:33:33:33:33:33',
            dest_mac_address='00:44:44:44:44:44',
            src_ip='10.10.10.1',
            dest_ip='20.20.20.1',
            queue_id=self.mirror_queue_id)
        self.attribute_set(self.port6, SWITCH_PORT_ATTR_INGRESS_MIRROR_HANDLE, self.mirror5)
        session_id = self.attribute_get(self.mirror5, SWITCH_MIRROR_ATTR_SESSION_ID)

        self.exp_mirrored_port5_pkt_full = ipv4_erspan_platform_pkt(
            eth_dst='00:44:44:44:44:44',
            eth_src='00:33:33:33:33:33',
            ip_dst='20.20.20.1',
            ip_src='10.10.10.1',
            ip_id=0,
            ip_ttl=64,
            ip_flags=0x0,
            version=self.erspan_version,
            mirror_id=session_id,
            sgt_other=self.sgt_other,
            erspan_gra=self.erspan_gra,
            erspan_o=self.erspan_o,
            inner_frame=self.port5_pkt)

        truncated_amount = self.pktlen_trunk - truncate_size
        self.exp_mirrored =  truncate_packet(self.exp_mirrored_port5_pkt_full, truncated_amount)

        self.exp_mirrored["IP"].len -= truncated_amount
        self.exp_mirrored_port5_pkt = ptf.mask.Mask(self.exp_mirrored)

        mask_set_do_not_care_packet(self.exp_mirrored_port5_pkt, IP, "chksum")
        if self.erspan_version == 2:
            mask_set_do_not_care_packet(self.exp_mirrored_port5_pkt, ERSPAN_III, "timestamp")

        self.mirror6 = self.add_mirror(self.device,
            max_pkt_len=max_pkt_len,
            type=SWITCH_MIRROR_ATTR_TYPE_ENHANCED_REMOTE,
            direction=SWITCH_MIRROR_ATTR_DIRECTION_EGRESS,
            rspan_type=SWITCH_MIRROR_ATTR_RSPAN_TYPE_NONE,
            session_type=SWITCH_MIRROR_ATTR_SESSION_TYPE_TRUNCATE,
            erspan_type=self.erspan_type,
            egress_port_handle=self.port7,
            ttl=64,
            platform_info=self.platform_info,
            src_mac_address='00:33:33:33:33:33',
            dest_mac_address='00:44:44:44:44:44',
            src_ip='10.10.10.1',
            dest_ip='20.20.20.1',
            queue_id=self.mirror_queue_id)
        self.attribute_set(self.port6, SWITCH_PORT_ATTR_EGRESS_MIRROR_HANDLE, self.mirror6)
        session_id = self.attribute_get(self.mirror6, SWITCH_MIRROR_ATTR_SESSION_ID)

        self.exp_mirrored_port6_pkt_full = ipv4_erspan_platform_pkt(
            eth_dst='00:44:44:44:44:44',
            eth_src='00:33:33:33:33:33',
            ip_dst='20.20.20.1',
            ip_src='10.10.10.1',
            ip_id=0,
            ip_ttl=64,
            ip_flags=0x0,
            version=self.erspan_version,
            mirror_id=session_id,
            sgt_other=self.sgt_other,
            erspan_gra=self.erspan_gra,
            erspan_o=self.erspan_o,
            inner_frame=self.port6_pkt)

        truncated_amount = self.pktlen_trunk - truncate_size
        self.exp_mirrored =  truncate_packet(self.exp_mirrored_port6_pkt_full, truncated_amount)

        self.exp_mirrored["IP"].len -= truncated_amount
        self.exp_mirrored_port6_pkt = ptf.mask.Mask(self.exp_mirrored)

        mask_set_do_not_care_packet(self.exp_mirrored_port6_pkt, IP, "chksum")
        if self.erspan_version == 2:
            mask_set_do_not_care_packet(self.exp_mirrored_port6_pkt, ERSPAN_III, "timestamp")

        self.egr_mirror0 = self.add_mirror(self.device,
            type=SWITCH_MIRROR_ATTR_TYPE_ENHANCED_REMOTE,
            direction=SWITCH_MIRROR_ATTR_DIRECTION_EGRESS,
            rspan_type=SWITCH_MIRROR_ATTR_RSPAN_TYPE_NONE,
            erspan_type=self.erspan_type,
            egress_port_handle=self.port7,
            ttl=64,
            platform_info=self.platform_info,
            src_mac_address='00:33:33:33:33:33',
            dest_mac_address='00:44:44:44:44:44',
            src_ip='10.10.10.1',
            dest_ip='20.20.20.1', queue_id=self.mirror_queue_id)
        self.attribute_set(self.port3, SWITCH_PORT_ATTR_EGRESS_MIRROR_HANDLE, self.egr_mirror0)
        session_id = self.attribute_get(self.egr_mirror0, SWITCH_MIRROR_ATTR_SESSION_ID)

        self.exp_mirrored_port0_pkt1 = ptf.mask.Mask(ipv4_erspan_platform_pkt(
            pktlen=pkt_len,
            eth_dst='00:44:44:44:44:44',
            eth_src='00:33:33:33:33:33',
            ip_dst='20.20.20.1',
            ip_src='10.10.10.1',
            ip_id=0,
            ip_ttl=64,
            ip_flags=0x0,
            version=self.erspan_version,
            mirror_id=session_id,
            sgt_other=self.sgt_other,
            erspan_gra=self.erspan_gra,
            erspan_o=self.erspan_o,
            inner_frame=self.port0_pkt1))
        # IPv4.total_length is different on Model and Hardware because of 4B CRC.
        if test_param_get('target') != "asic-model":
            mask_set_do_not_care_packet(self.exp_mirrored_port0_pkt1, IP, "len")
        mask_set_do_not_care_packet(self.exp_mirrored_port0_pkt1, IP, "chksum")
        if self.erspan_version == 2:
            mask_set_do_not_care_packet(self.exp_mirrored_port0_pkt1, ERSPAN_III, "timestamp")

    def tearDown(self):
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_MIRROR_HANDLE, 0)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_INGRESS_MIRROR_HANDLE, 0)
        self.attribute_set(self.port2, SWITCH_PORT_ATTR_INGRESS_MIRROR_HANDLE, 0)
        self.attribute_set(self.port3, SWITCH_PORT_ATTR_EGRESS_MIRROR_HANDLE, 0)
        self.attribute_set(self.port6, SWITCH_PORT_ATTR_INGRESS_MIRROR_HANDLE, 0)
        self.attribute_set(self.port6, SWITCH_PORT_ATTR_EGRESS_MIRROR_HANDLE, 0)
        super(RemoteMirrorHelper, self).tearDown()

    def MirrorTest(self):
        print("MirrorTest()")
        try:
            print("Erspan mirror packet from 0 -> 4, mirror to 7")
            self.clearQueueStats(self.port7, self.mirror_queue_id)
            send_packet(self, self.devports[0], self.port0_pkt)
            p1 = [self.devports[4], self.port0_pkt]
            p2 = [self.devports[7], self.exp_mirrored_port0_pkt]
            verify_each_packet_on_each_port(self, [p1[1], p2[1]], [p1[0], p2[0]])
            self.verifyQueueStats(self.port7, self.mirror_queue_id, 1)
        finally:
            pass

    def MirrorIngressTruncateTest(self):
        print("MirrorIngressTruncateTest()")
        try:
            print("Erspan mirror packet from 0 -> 5, trunked mirror packet to 7")
            self.clearQueueStats(self.port7, self.mirror_queue_id)
            p1 = [self.devports[5], self.port5_pkt]
            p2 = [self.devports[7], self.exp_mirrored_port5_pkt]
            send_packet(self, self.devports[6], self.port5_pkt)
            verify_each_packet_on_each_port(self, [p1[1], p2[1]], [p1[0], p2[0]])
            self.verifyQueueStats(self.port7, self.mirror_queue_id, 1)
        finally:
            pass

    def MirrorEgressTruncateTest(self):
        print("MirrorEgressTruncateTest()")
        try:
            print("Erspan mirror packet from 0 -> 6, trunked mirror packet to 7")
            self.clearQueueStats(self.port7, self.mirror_queue_id)
            p1 = [self.devports[6], self.port6_pkt]
            p2 = [self.devports[7], self.exp_mirrored_port6_pkt]
            send_packet(self, self.devports[8], self.port6_pkt)
            verify_each_packet_on_each_port(self, [p1[1], p2[1]], [p1[0], p2[0]])
            self.verifyQueueStats(self.port7, self.mirror_queue_id, 1)
        finally:
            pass


    def MirrorVlanIDTest(self):
        print("MirrorVlanIDTest()")
        try:
            print("Erspan mirror packet from 1 -> 4, mirror to 7")
            self.clearQueueStats(self.port7, self.mirror_queue_id)
            send_packet(self, self.devports[1], self.port1_pkt)
            p1 = [self.devports[4], self.port1_pkt]
            p2 = [self.devports[7], self.exp_mirrored_port1_pkt]
            verify_each_packet_on_each_port(self, [p1[1], p2[1]], [p1[0], p2[0]])
            self.verifyQueueStats(self.port7, self.mirror_queue_id, 1)
        finally:
            pass

    def MirrorVlanHandleTest(self):
        print("MirrorVlanHandleTest()")
        try:
            print("Erspan mirror packet from 2 -> 4, mirror to 7")
            self.clearQueueStats(self.port7, self.mirror_queue_id)
            send_packet(self, self.devports[2], self.port2_pkt)
            p1 = [self.devports[4], self.port2_pkt]
            p2 = [self.devports[7], self.exp_mirrored_port2_pkt]
            verify_each_packet_on_each_port(self, [p1[1], p2[1]], [p1[0], p2[0]])
            self.verifyQueueStats(self.port7, self.mirror_queue_id, 1)
        finally:
            pass

    def EgressMirrorTest(self):
        print("EgressMirrorTest()")
        try:
            print("Erspan mirror packet from 0 -> 3, mirror to 7")
            self.clearQueueStats(self.port7, self.mirror_queue_id)
            send_packet(self, self.devports[0], self.port0_pkt1)
            p1 = [self.devports[3], self.port0_pkt1]
            p2 = [self.devports[7], self.exp_mirrored_port0_pkt1]
            verify_each_packet_on_each_port(self, [p1[1], p2[1]], [p1[0], p2[0]])
            self.verifyQueueStats(self.port7, self.mirror_queue_id, 2)
        finally:
            pass


@group('mirror')
class EnhancedRemote2MirrorTest(RemoteMirrorHelper):
    def __init__(self):
        super(EnhancedRemote2MirrorTest, self).__init__(SWITCH_MIRROR_ATTR_ERSPAN_TYPE_ERSPAN_2)

    def runTest(self):
        try:
            self.MirrorTest()
            self.MirrorVlanIDTest()
            self.MirrorVlanHandleTest()
            self.EgressMirrorTest()
            self.MirrorIngressTruncateTest()
            self.MirrorEgressTruncateTest()
        finally:
            pass


@group('mirror')
class RspanTest(MirrorTestHelper):
    def setUp(self):
        super(RspanTest, self).setUp()
        self.add_vlan_member(self.device, vlan_handle=self.vlan20, member_handle=self.port10, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
        self.add_mac_entry(self.device, vlan_handle=self.vlan20, mac_address='00:10:10:10:10:10', destination_handle=self.port10)
        self.add_vlan_member(self.device, vlan_handle=self.vlan20, member_handle=self.port11, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
        self.add_mac_entry(self.device, vlan_handle=self.vlan20, mac_address='00:10:10:10:10:11', destination_handle=self.port11)

    def runTest(self):
        try:
            if (self.client.is_feature_enable(SWITCH_FEATURE_RSPAN) == 0):
                print("Rspan mirror feature not enabled, skipping")
                return
            self.IngressRemoteMirrorVlanIDTest()
            self.EgressRemoteMirrorVlanIDTest()
            self.IngressRemoteMirrorVlanHandleTest()
            self.EgressRemoteMirrorVlanHandleTest()
        finally:
            pass

    def tearDown(self):
        self.cleanlast()
        self.cleanlast()
        self.cleanlast()
        self.cleanlast()
        super(RspanTest, self).tearDown()

    def IngressRemoteMirrorVlanIDTest(self):
        print("IngressRemoteMirrorVlanIDTest()")
        mirror = self.add_mirror(self.device,
            type=SWITCH_MIRROR_ATTR_TYPE_REMOTE,
            direction=SWITCH_MIRROR_ATTR_DIRECTION_INGRESS,
            rspan_type=SWITCH_MIRROR_ATTR_RSPAN_TYPE_VLAN_ID,
            vlan_id=50,
            egress_port_handle=self.port3)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_MIRROR_HANDLE, mirror)
        self.attribute_set(self.port10, SWITCH_PORT_ATTR_INGRESS_MIRROR_HANDLE, mirror)

        pkt = simple_udp_packet(
            pktlen=pkt_len,
            eth_dst='00:01:11:11:11:11',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.0.0.1',
            ip_ttl=64)
        exp_pkt = simple_udp_packet(
            pktlen=pkt_len,
            eth_dst='00:01:11:11:11:11',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.0.0.1',
            ip_ttl=64)
        mirror_pkt = simple_udp_packet(
            pktlen=pkt_len+4,
            eth_dst='00:01:11:11:11:11',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.0.0.1',
            dl_vlan_enable=True,
            vlan_vid=50,
            ip_ttl=64)

        tagged_pkt = simple_udp_packet(
            pktlen=pkt_len,
            eth_dst='00:10:10:10:10:11',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.0.0.1',
            dl_vlan_enable=True,
            vlan_vid=20,
            ip_ttl=64)
        exp_tagged_pkt = simple_udp_packet(
            pktlen=pkt_len,
            eth_dst='00:10:10:10:10:11',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.0.0.1',
            dl_vlan_enable=True,
            vlan_vid=20,
            ip_ttl=64)
        mirror_tagged_pkt = simple_udp_packet(
            pktlen=pkt_len,
            eth_dst='00:10:10:10:10:11',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.0.0.1',
            dl_vlan_enable=True,
            vlan_vid=50,
            ip_ttl=64)

        try:
            print("Remote Vlan mirror packet from 0 -> 1, mirror to 3")
            send_packet(self, self.devports[0], pkt)
            verify_each_packet_on_each_port(self, [exp_pkt, mirror_pkt], [self.devports[1], self.devports[3]])
            print("Remote Vlan mirror tagged packet from 10 -> 11, mirror to 3")
            send_packet(self, self.devports[10], tagged_pkt)
            verify_each_packet_on_each_port(self, [exp_tagged_pkt, mirror_tagged_pkt], [self.devports[11], self.devports[3]])
        finally:
            self.attribute_set(self.port10, SWITCH_PORT_ATTR_INGRESS_MIRROR_HANDLE, 0)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_MIRROR_HANDLE, 0)
            self.cleanlast()
            pass

    def EgressRemoteMirrorVlanIDTest(self):
        print("EgressRemoteMirrorVlanIDTest()")
        mirror = self.add_mirror(self.device,
            type=SWITCH_MIRROR_ATTR_TYPE_REMOTE,
            direction=SWITCH_MIRROR_ATTR_DIRECTION_EGRESS,
            rspan_type=SWITCH_MIRROR_ATTR_RSPAN_TYPE_VLAN_ID,
            vlan_id=50,
            egress_port_handle=self.port3)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_EGRESS_MIRROR_HANDLE, mirror)
        self.attribute_set(self.port10, SWITCH_PORT_ATTR_EGRESS_MIRROR_HANDLE, mirror)

        pkt = simple_udp_packet(
            pktlen=pkt_len,
            eth_dst='00:00:11:11:11:11',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.0.0.1',
            ip_ttl=64)
        exp_pkt = simple_udp_packet(
            pktlen=pkt_len,
            eth_dst='00:00:11:11:11:11',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.0.0.1',
            ip_ttl=64)
        mirror_pkt = simple_udp_packet(
            pktlen=pkt_len+4,
            eth_dst='00:00:11:11:11:11',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.0.0.1',
            dl_vlan_enable=True,
            vlan_vid=50,
            ip_ttl=64)

        tagged_pkt = simple_udp_packet(
            pktlen=pkt_len,
            eth_dst='00:10:10:10:10:10',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.0.0.1',
            dl_vlan_enable=True,
            vlan_vid=20,
            ip_ttl=64)
        exp_tagged_pkt = simple_udp_packet(
            pktlen=pkt_len,
            eth_dst='00:10:10:10:10:10',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.0.0.1',
            dl_vlan_enable=True,
            vlan_vid=20,
            ip_ttl=64)
        mirror_tagged_pkt = simple_udp_packet(
            pktlen=pkt_len,
            eth_dst='00:10:10:10:10:10',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.0.0.1',
            dl_vlan_enable=True,
            vlan_vid=50,
            ip_ttl=64)

        try:
            print("Remote Vlan mirror packet from 1 -> 0, mirror to 3")
            send_packet(self, self.devports[1], pkt)
            verify_each_packet_on_each_port(self, [exp_pkt, mirror_pkt], [self.devports[0], self.devports[3]])
            print("Remote Vlan mirror tagged packet from 11 -> 10, mirror to 3")
            send_packet(self, self.devports[11], tagged_pkt)
            verify_each_packet_on_each_port(self, [exp_tagged_pkt, mirror_tagged_pkt], [self.devports[10], self.devports[3]])
        finally:
            self.attribute_set(self.port10, SWITCH_PORT_ATTR_EGRESS_MIRROR_HANDLE, 0)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_EGRESS_MIRROR_HANDLE, 0)
            self.cleanlast()
            pass

    def IngressRemoteMirrorVlanHandleTest(self):
        print("IngressRemoteMirrorVlanHandleTest()")
        mirror = self.add_mirror(self.device,
            type=SWITCH_MIRROR_ATTR_TYPE_REMOTE,
            direction=SWITCH_MIRROR_ATTR_DIRECTION_INGRESS,
            rspan_type=SWITCH_MIRROR_ATTR_RSPAN_TYPE_VLAN_HANDLE,
            vlan_handle=self.vlan40,
            egress_port_handle=self.port3)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_MIRROR_HANDLE, mirror)

        pkt = simple_udp_packet(
            pktlen=pkt_len,
            eth_dst='00:01:11:11:11:11',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.0.0.1',
            ip_ttl=64)
        exp_pkt = simple_udp_packet(
            pktlen=pkt_len,
            eth_dst='00:01:11:11:11:11',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.0.0.1',
            ip_ttl=64)
        mirror_pkt = simple_udp_packet(
            pktlen=pkt_len+4,
            eth_dst='00:01:11:11:11:11',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.0.0.1',
            dl_vlan_enable=True,
            vlan_vid=40,
            ip_ttl=64)

        try:
            p1 = [self.devports[1], [exp_pkt]]
            p2 = [self.devports[3], [mirror_pkt]]
            print("Remote Vlan mirror packet from 0 -> 1, mirror to 3")
            send_packet(self, self.devports[0], pkt)
            verify_each_packet_on_each_port(self, [exp_pkt, mirror_pkt], [self.devports[1], self.devports[3]])
        finally:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_MIRROR_HANDLE, 0)
            self.cleanlast()
            pass

    def EgressRemoteMirrorVlanHandleTest(self):
        print("EgressRemoteMirrorVlanHandleTest()")
        mirror = self.add_mirror(self.device,
            type=SWITCH_MIRROR_ATTR_TYPE_REMOTE,
            direction=SWITCH_MIRROR_ATTR_DIRECTION_EGRESS,
            rspan_type=SWITCH_MIRROR_ATTR_RSPAN_TYPE_VLAN_HANDLE,
            vlan_handle=self.vlan40,
            egress_port_handle=self.port3)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_EGRESS_MIRROR_HANDLE, mirror)

        pkt = simple_udp_packet(
            pktlen=pkt_len,
            eth_dst='00:00:11:11:11:11',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.0.0.1',
            ip_ttl=64)
        exp_pkt = simple_udp_packet(
            pktlen=pkt_len,
            eth_dst='00:00:11:11:11:11',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.0.0.1',
            ip_ttl=64)
        mirror_pkt = simple_udp_packet(
            pktlen=pkt_len+4,
            eth_dst='00:00:11:11:11:11',
            eth_src='00:22:22:22:22:22',
            ip_dst='10.0.0.1',
            dl_vlan_enable=True,
            vlan_vid=40,
            ip_ttl=64)

        try:
            print("Remote Vlan mirror packet from 1 -> 0, mirror to 3")
            send_packet(self, self.devports[1], pkt)
            verify_each_packet_on_each_port(self, [exp_pkt, mirror_pkt], [self.devports[0], self.devports[3]])
        finally:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_EGRESS_MIRROR_HANDLE, 0)
            self.cleanlast()
            pass


@group('mirror')
class EnhancedRemote3MirrorTest(RemoteMirrorHelper):
    def __init__(self):
        super(EnhancedRemote3MirrorTest, self).__init__(SWITCH_MIRROR_ATTR_ERSPAN_TYPE_ERSPAN_3)

    def runTest(self):
        try:
            self.MirrorTest()
            self.MirrorVlanIDTest()
            self.MirrorVlanHandleTest()
            self.MirrorTestDstMAC()
            self.EgressMirrorTest()
            self.MirrorIngressTruncateTest()
            self.MirrorEgressTruncateTest()
        finally:
            pass

    def MirrorTestDstMAC(self):
        print("MirrorTestDstMAC()")
        try:
            print("Erspan mirror packet from 0 -> 4, mirror to 7")
            send_packet(self, self.devports[0], self.port0_pkt)
            p1 = [self.devports[4], self.port0_pkt]
            p2 = [self.devports[7], self.exp_mirrored_port0_pkt]
            verify_each_packet_on_each_port(self, [p1[1], p2[1]], [p1[0], p2[0]])

            print("Change mirror dmac")
            self.attribute_set(self.mirror0, SWITCH_MIRROR_ATTR_DEST_MAC_ADDRESS, '00:44:44:44:44:66')
            self.exp_mirrored_port0_pkt.exp_pkt['Ethernet'].dst = '00:44:44:44:44:66'
            print("Erspan mirror packet from 0 -> 4, mirror to 7")
            send_packet(self, self.devports[0], self.port0_pkt)
            verify_each_packet_on_each_port(self, [p1[1], p2[1]], [p1[0], p2[0]])
        finally:
            pass


@group('mirror')
class EnhancedRemote3PlatMirrorTest(RemoteMirrorHelper):
    def __init__(self):
        super(EnhancedRemote3PlatMirrorTest, self).__init__(SWITCH_MIRROR_ATTR_ERSPAN_TYPE_ERSPAN_3, True)

    def runTest(self):
        try:
            if (self.client.is_feature_enable(SWITCH_FEATURE_ERSPAN_PLATFORM_INFO) == 0):
                print("Erspan platform info unsupported. Skipping")
            else:
                self.MirrorTest()
                self.MirrorVlanIDTest()
                self.MirrorVlanHandleTest()
                self.EgressMirrorTest()
                self.MirrorIngressTruncateTest()
                self.MirrorEgressTruncateTest()
        finally:
            pass

###############################################################################
