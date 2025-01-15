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

import os
import sys
import time
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
import ptf.mask as mask

from bf_switcht_api_thrift.ttypes import *
from bf_switcht_api_thrift.model_headers import *

this_dir = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(this_dir, '..'))
from common.utils import *
from datetime import datetime

acl_table_bp_port = switcht_list_val_t(type=switcht_value_type.ENUM, enumdata=SWITCH_ACL_TABLE_ATTR_BIND_POINT_TYPE_PORT)
pkt_len = int(test_param_get('pkt_size'))

@disabled
class NatPerfTest(ApiHelper):
    def runTest(self):
        self.configure()
        self.client.nat_perf_test(self.device, 40000);

@disabled
class NatPerfBatchTest(ApiHelper):
    def runTest(self):
        self.configure()
        self.client.nat_perf_batch_test(self.device, 40000);

class NatTest(ApiHelper):
    ''' Test NAT feature

    Topology:
    - rif1: ingress port used in all tests
    - rif2: egress port for traffic to 172.10.x.x
    - rif3: egress port for traffic to 10.10.x.x

    The rifs are moved in different NAT zones depending on the test case
    using SetNatZones().

    An Acl rule is used to force disable NAT translation. This is controlled
    using AttachNatDisableAcl() and DetachNatDisableAcl().

    NAT rules are programmed in each individual test case.

    DNAT Pool is configured using ConfigureDestNatPool().
    '''

    def setUp(self):
        ''' Test configuration
        Note: functions must be called in this order.
        '''
        self.configure()

        if not self.HasFeature(SWITCH_FEATURE_NAT):
            print("NAT feature is not supported - Skip test set up")
            return

        self.ConfigureRoute()
        self.CreateCommonPkts()
        self.ConfigureDNATMissTrap()
        self.ConfigureSNATMissTrap()
        self.ConfigureNatDisableAcl()
        self.ConfigureDestNatPool()

    def runTest(self):
        if not self.HasFeature(SWITCH_FEATURE_NAT):
            print("NAT feature is not supported")
            return

        try:
            self.PrintNatTableSizes()
            # NAT test cases
            self.VerifyDestNaptMissTest()
            self.VerifyDoubleNaptTest()
            self.VerifyDoubleNatTest()
            self.VerifyDestNaptTest()
            self.VerifyDestNatTest()
            # Dest NAT Pool before must be cleared before SNAT tests
            self.ClearDestNatPool()
            self.VerifySrcNaptTest()
            self.VerifySrcNatTest()
        finally:
            pass

    def tearDown(self):
        if not self.HasFeature(SWITCH_FEATURE_NAT):
            print("NAT feature is not supported - Skip test tear down")
            self.cleanup()
            return

        self.SetNatZones(rif1=0, rif2=0, rif3=0)
        self.DetachNatDisableAcl()
        self.ClearDestNatPool()
        self.cleanup()

    def PrintNatTableSizes(self):
        table_info = self.client.table_info_get(SWITCH_DEVICE_ATTR_TABLE_DNAT)
        print("DNAT table size %d"%(table_info.size))
        table_info = self.client.table_info_get(SWITCH_DEVICE_ATTR_TABLE_DNAPT)
        print("DNAPT table size %d"%(table_info.size))
        table_info = self.client.table_info_get(SWITCH_DEVICE_ATTR_TABLE_SNAT)
        print("SNAT table size %d"%(table_info.size))
        table_info = self.client.table_info_get(SWITCH_DEVICE_ATTR_TABLE_SNAPT)
        print("SNAPT table size %d"%(table_info.size))
        table_info = self.client.table_info_get(SWITCH_DEVICE_ATTR_TABLE_FLOW_NAT)
        print("Flow NAT table size %d"%(table_info.size))
        table_info = self.client.table_info_get(SWITCH_DEVICE_ATTR_TABLE_FLOW_NAPT)
        print("Flow NAPT table size %d"%(table_info.size))

    def AddRif(self, port):
        return self.add_rif(self.device,
                            type=SWITCH_RIF_ATTR_TYPE_PORT,
                            port_handle=port,
                            vrf_handle=self.vrf10,
                            src_mac=self.rmac,
                            nat_zone=0)

    def ConfigureRoute(self):
        self.rif1 = self.AddRif(self.port0)
        self.rif2 = self.AddRif(self.port1)
        self.rif3 = self.AddRif(self.port2)

        #Add NAT translated route for 172.10.10.0/24 to RIF2
        nat_nhop = self.add_nexthop(self.device, handle=self.rif2, dest_ip='172.10.0.2')
        self.add_neighbor(self.device, mac_address='00:55:44:33:22:11', handle=self.rif2, dest_ip='172.10.0.2')
        nat_route = self.add_route(self.device, ip_prefix='172.10.10.0/24', vrf_handle=self.vrf10, nexthop_handle=nat_nhop)

        #Add LPM route for 10.10.10.0/24 to RIF3
        lpm_nhop = self.add_nexthop(self.device, handle=self.rif3, dest_ip='10.10.0.2')
        self.add_neighbor(self.device, mac_address='00:11:22:33:44:66', handle=self.rif3, dest_ip='10.10.0.2')
        lpm_route = self.add_route(self.device, ip_prefix='10.10.10.0/24', vrf_handle=self.vrf10, nexthop_handle=lpm_nhop)

    def SetNatZones(self, rif1=0, rif2=0, rif3=0):
        self.attribute_set(self.rif1, SWITCH_RIF_ATTR_NAT_ZONE, rif1)
        self.attribute_set(self.rif2, SWITCH_RIF_ATTR_NAT_ZONE, rif2)
        self.attribute_set(self.rif3, SWITCH_RIF_ATTR_NAT_ZONE, rif3)

    def CreateCommonPkts(self):
        # Create common packets used by multiple tests
        pkt = PktBuilder(pktlen=pkt_len,
                         eth_dst=self.rmac,
                         ip_dst='10.10.10.1',
                         tcp_dport=100,
                         ip_ttl=64, with_tcp_chksum=True)

        self.send_pkt = pkt.build(simple_tcp_packet)

        pkt.eth_dst = '00:11:22:33:44:66'
        pkt.eth_src = self.rmac
        pkt.ip_ttl -= 1;
        self.exact_pkt = pkt.build(simple_tcp_packet)

        pkt.eth_dst = self.rmac
        pkt.ip_dst = '172.10.10.1'
        send_pkt = pkt.build(simple_tcp_packet)

        pkt.ip_ttl -= 1;
        pkt.eth_dst = '00:55:44:33:22:11'
        pkt.eth_src = self.rmac
        exact_pkt = pkt.build(simple_tcp_packet)

        cpu_pkt = simple_cpu_packet(packet_type=0,
                                    ingress_port=self.devports[0],
                                    ingress_ifindex=(self.port0 & 0xFFFF),
                                    reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_SNAT_MISS,
                                    ingress_bd=2,
                                    inner_pkt=self.send_pkt)
        self.exp_cpu_pkt = cpu_packet_mask_ingress_bd(cpu_pkt)

    def ConfigureNatDisableAcl(self):
        self.acl_table = self.add_acl_table(self.device,
                                            type=SWITCH_ACL_TABLE_ATTR_TYPE_IPV4,
                                            bind_point_type=[acl_table_bp_port],
                                            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        self.acl_entry = self.add_acl_entry(self.device,
                                            table_handle=self.acl_table,
                                            dst_ip='10.10.10.1', dst_ip_mask='255.255.255.255',
                                            action_disable_nat=0)
        self.disable_nat = False

    def AttachNatDisableAcl(self):
        self.attribute_set(self.acl_entry, SWITCH_ACL_ENTRY_ATTR_ACTION_DISABLE_NAT, 1);
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, self.acl_table)
        self.disable_nat = True

    def DetachNatDisableAcl(self):
        self.attribute_set(self.acl_entry, SWITCH_ACL_ENTRY_ATTR_ACTION_DISABLE_NAT, 0);
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
        self.disable_nat = False

    def CheckNatAclCounter(self):
        ''' Check that ACL counter is equal 1 and reset it '''
        counter = self.client.object_counters_get(self.acl_entry)
        self.assertEqual(counter[SWITCH_ACL_ENTRY_COUNTER_ID_PKTS].count, 1)
        cntr_ids = [SWITCH_ACL_ENTRY_COUNTER_ID_PKTS]
        self.client.object_counters_clear(self.acl_entry, cntr_ids)

    def ConfigureDestNatPool(self):
        self.dnat_pool_obj = self.add_nat_entry(
            self.device, dst_ip_key='10.10.10.1',
            type=SWITCH_NAT_ENTRY_ATTR_TYPE_DESTINATION_NAT_POOL)
        # remove it from stack since it is manually deleted
        self.stack.pop()

    def ClearDestNatPool(self):
        if hasattr(self, 'dnat_pool_obj') and self.dnat_pool_obj:
            self.client.object_delete(self.dnat_pool_obj)
            self.dnat_pool_obj = None

    def ConfigureMissTrap(self, trap_type):
        meter_hdl = self.add_meter(self.device,
                                   mode=SWITCH_METER_ATTR_MODE_TWO_RATE_THREE_COLOR,
                                   pbs=16, cbs=8, cir=1000, pir=1000,
                                   type=SWITCH_METER_ATTR_TYPE_PACKETS,
                                   color_source=SWITCH_METER_ATTR_COLOR_SOURCE_BLIND)
        queue_handles = self.attribute_get(self.cpu_port_hdl, SWITCH_PORT_ATTR_QUEUE_HANDLES)
        hostif_trap_group = self.add_hostif_trap_group(self.device,
                                                       queue_handle=queue_handles[0].oid,
                                                       admin_state=True,
                                                       policer_handle=meter_hdl)
        self.add_hostif_trap(self.device, type=trap_type,
                             hostif_trap_group_handle=hostif_trap_group,
                             packet_action=SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_REDIRECT_TO_CPU)

    def ConfigureSNATMissTrap(self):
        self.ConfigureMissTrap(SWITCH_HOSTIF_TRAP_ATTR_TYPE_SNAT_MISS)

    def ConfigureDNATMissTrap(self):
        self.ConfigureMissTrap(SWITCH_HOSTIF_TRAP_ATTR_TYPE_DNAT_MISS)

    def VerifyDestNaptMissTest(self):
        print("DNAT pool match, but DNAT table miss -> Redirect to CPU")
        if not self.HasFeature(SWITCH_FEATURE_NAPT):
            print("- Feature is not supported - skip test case")
            return
        send_pkt = simple_tcp_packet(pktlen=pkt_len,
            eth_dst=self.rmac,
            ip_dst='10.10.10.1',
            ip_ttl=64,tcp_dport=100, with_tcp_chksum=True)
        cpu_nat_pkt = simple_cpu_packet(packet_type=0,
                                        ingress_port=self.devports[0],
                                        ingress_ifindex=(self.port0 & 0xffff),
                                        reason_code=SWITCH_HOSTIF_TRAP_ATTR_TYPE_DNAT_MISS,
                                        ingress_bd=2,
                                        inner_pkt=send_pkt)
        self.SetNatZones(rif1=1, rif2=0, rif3=1)
        exp_cpu_nat_pkt = cpu_packet_mask_ingress_bd(cpu_nat_pkt)
        send_packet(self, self.devports[0], send_pkt)
        verify_packets(self, exp_cpu_nat_pkt, [self.cpu_port])

    def VerifyDoubleNaptTest(self):
        print("Verify Double NAPT")
        if not self.HasFeature(SWITCH_FEATURE_FLOW_NAT):
            print("- Feature is not supported - skip test case")
            return
        try:
            flow_napt_obj = self.add_nat_entry(
                self.device, ip_proto_key=6,
                src_ip_key='20.20.20.1', dst_ip_key='10.10.10.1',
                l4_src_port_key=80, l4_dst_port_key=100,
                nat_dst_ip='172.10.10.10', nat_src_ip='200.200.200.1',
                nat_l4_src_port=2000, nat_l4_dst_port=200,
                type=SWITCH_NAT_ENTRY_ATTR_TYPE_DOUBLE_NAPT)

            pkt = PktBuilder(pktlen=pkt_len,
                             eth_dst=self.rmac,
                             ip_src='20.20.20.1',
                             ip_dst='10.10.10.1',
                             tcp_sport=80, tcp_dport=100,
                             ip_ttl=64, with_tcp_chksum=True)
            send_pkt = pkt.build(simple_tcp_packet)

            pkt.ip_ttl -= 1
            pkt.eth_dst = '00:11:22:33:44:66'
            pkt.eth_src = self.rmac
            normal_l3_recv_pkt = pkt.build(simple_tcp_packet)

            pkt.eth_dst = '00:55:44:33:22:11'
            pkt.ip_src = '200.200.200.1'
            pkt.ip_dst = '172.10.10.10'
            pkt.tcp_sport = 2000
            pkt.tcp_dport = 200
            nat_pkt = pkt.build(simple_tcp_packet)

            self.SetNatZones(rif1=1, rif2=0, rif3=0)

            print("- With NAT disabled")
            self.AttachNatDisableAcl()
            send_packet(self, self.devports[0], send_pkt)
            verify_packets(self, normal_l3_recv_pkt, [self.devports[2]])
            self.CheckNatAclCounter()

            print("- With NAT enabled")
            self.DetachNatDisableAcl()
            send_packet(self, self.devports[0], send_pkt)
            verify_packets(self, nat_pkt, [self.devports[1]])
            self.AssertCounterEq(flow_napt_obj, 1, clear=True)

        finally:
            self.cleanlast()


    def VerifyDoubleNatTest(self):
        print("Verify Double NAT")
        if not self.HasFeature(SWITCH_FEATURE_FLOW_NAT):
            print("- Feature is not supported - skip test case")
            return
        try:
            flow_nat_obj = self.add_nat_entry(
                self.device, ip_proto_key=6,
                src_ip_key='20.20.20.1', dst_ip_key='10.10.10.1',
                nat_dst_ip='172.10.10.10', nat_src_ip='200.200.200.1',
                type=SWITCH_NAT_ENTRY_ATTR_TYPE_DOUBLE_NAT)

            send_pkt = simple_tcp_packet(pktlen=pkt_len,
                eth_dst=self.rmac,
                ip_src='20.20.20.1',
                ip_dst='10.10.10.1',
                ip_ttl=64,with_tcp_chksum=True)
            nat_pkt = simple_tcp_packet(pktlen=pkt_len,
                eth_dst='00:55:44:33:22:11',
                eth_src=self.rmac,
                ip_src='200.200.200.1',
                ip_dst='172.10.10.10',
                ip_ttl=63, with_tcp_chksum=True)
            normal_l3_recv_pkt = simple_tcp_packet(pktlen=pkt_len,
                eth_dst='00:11:22:33:44:66',
                eth_src=self.rmac,
                ip_dst='10.10.10.1',
                ip_src='20.20.20.1',
                ip_ttl=63, with_tcp_chksum=True)

            self.SetNatZones(rif1=1, rif2=0, rif3=0)

            print("- With NAT disabled")
            self.AttachNatDisableAcl()
            send_packet(self, self.devports[0], send_pkt)
            verify_packets(self, normal_l3_recv_pkt, [self.devports[2]])
            self.CheckNatAclCounter()

            print("- With NAT enabled")
            self.DetachNatDisableAcl()
            send_packet(self, self.devports[0], send_pkt)
            verify_packets(self, nat_pkt, [self.devports[1]])
            self.AssertCounterEq(flow_nat_obj, 1, clear=True)

        finally:
            self.cleanlast()


    def VerifyDestNaptTest(self, skipMissTest=False):
        print("Verify DNAPT")
        if not self.HasFeature(SWITCH_FEATURE_NAPT):
            print("- Feature is not supported - skip test case")
            return
        try:
            dnapt_obj = self.add_nat_entry(
                self.device, ip_proto_key=6,
                dst_ip_key='10.10.10.1', l4_dst_port_key=100,
                nat_dst_ip='172.10.10.10', nat_l4_dst_port=200,
                type=SWITCH_NAT_ENTRY_ATTR_TYPE_DESTINATION_NAPT)

            nat_pkt = simple_tcp_packet(pktlen=pkt_len,
                eth_dst='00:55:44:33:22:11',
                eth_src=self.rmac,
                ip_dst='172.10.10.10',
                ip_ttl=63,tcp_dport=200, with_tcp_chksum=True)

            self.SetNatZones(rif1=1, rif2=0, rif3=0)

            print("- With NAT disabled")
            self.AttachNatDisableAcl()
            send_packet(self, self.devports[0], self.send_pkt)
            verify_packets(self, self.exact_pkt, [self.devports[2]])
            self.CheckNatAclCounter()

            print("- With NAT enabled")
            self.DetachNatDisableAcl()
            send_packet(self, self.devports[0], self.send_pkt)
            verify_packets(self, nat_pkt, [self.devports[1]])
            self.AssertCounterEq(dnapt_obj, 1, clear=True)

            print("- Same zone with NAT enabled, regular fowarding and no translation")
            send_packet(self, self.devports[1], self.send_pkt)
            verify_packets(self, self.exact_pkt, [self.devports[2]])

            if not skipMissTest:
                print("- Inside to Outside zone -> SNAPT Miss")
                # Change NAT zones and send pkt from inside to outside -> SNAPT
                # Verify that SNAPT table misses and pkt forwarded to CPU port
                # Goal: make sure pkt doesn't match the DNAPT rule configured above
                self.SetNatZones(rif1=0, rif2=1, rif3=1)
                send_packet(self, self.devports[0], self.send_pkt)
                verify_packets(self, self.exp_cpu_pkt, [self.cpu_port])

        finally:
            self.cleanlast()

    def VerifyDestNatTest(self, skipMissTest=False):
        print("Verify DNAT")
        if not self.HasFeature(SWITCH_FEATURE_BASIC_NAT):
            print("- Feature is not supported - skip test case")
            return
        try:
            dnat_obj = self.add_nat_entry(
                self.device,
                dst_ip_key='10.10.10.1', nat_dst_ip='172.10.10.20',
                type=SWITCH_NAT_ENTRY_ATTR_TYPE_DESTINATION_NAT)

            nat_pkt = simple_tcp_packet(pktlen=pkt_len,
                eth_dst='00:55:44:33:22:11',
                eth_src=self.rmac,
                ip_dst='172.10.10.20',
                ip_ttl=63,tcp_dport=100, with_tcp_chksum=True)

            icmp_pkt = simple_icmp_packet(
                pktlen=pkt_len,
                eth_dst=self.rmac,
                ip_dst='10.10.10.1', ip_ttl=64)

            nat_icmp_pkt = simple_icmp_packet(
                pktlen=pkt_len,
                eth_dst='00:55:44:33:22:11',
                eth_src=self.rmac,
                ip_dst='172.10.10.20', ip_ttl=63)

            self.SetNatZones(rif1=1, rif2=0, rif3=0)

            print("- With NAT disabled")
            self.AttachNatDisableAcl()
            send_packet(self, self.devports[0], self.send_pkt)
            verify_packets(self, self.exact_pkt, [self.devports[2]])
            self.CheckNatAclCounter()

            print("- With NAT enabled")
            self.DetachNatDisableAcl()
            send_packet(self, self.devports[0], self.send_pkt)
            verify_packets(self, nat_pkt, [self.devports[1]])
            self.AssertCounterEq(dnat_obj, 1, clear=True)

            send_packet(self, self.devports[0], icmp_pkt)
            verify_packets(self, nat_icmp_pkt, [self.devports[1]])
            self.AssertCounterEq(dnat_obj, 1, clear=True)

            if not skipMissTest:
                print("- Inside to Outside zone -> SNAT Miss")
                self.SetNatZones(rif1=0, rif2=1, rif3=1)
                # Change NAT zones and send same pkt from inside to outside -> SNAT
                # Verify that SNAT table misses and pkt forwarded to CPU port
                # Goal: make sure pkt doesn't match the DNAT rule configured above
                send_packet(self, self.devports[0], self.send_pkt)
                verify_packets(self, self.exp_cpu_pkt, [self.cpu_port])

        finally:
            self.cleanlast()

    def VerifySrcNaptTest(self, skipMissTest=False):
        print("Verify SNAPT")
        if not self.HasFeature(SWITCH_FEATURE_NAPT):
            print("- Feature is not supported - skip test case")
            return
        try:
            snapt_tcp_obj = self.add_nat_entry(
                self.device, ip_proto_key=6,
                src_ip_key='20.20.20.1', l4_src_port_key=100,
                nat_src_ip='200.10.10.10', nat_l4_src_port=200,
                type=SWITCH_NAT_ENTRY_ATTR_TYPE_SOURCE_NAPT)

            snapt_udp_obj = self.add_nat_entry(
                self.device, ip_proto_key=17,
                src_ip_key='20.20.20.1', l4_src_port_key=100,
                nat_src_ip='200.10.10.10', nat_l4_src_port=200,
                type=SWITCH_NAT_ENTRY_ATTR_TYPE_SOURCE_NAPT)

            send_pkt = simple_tcp_packet(pktlen=pkt_len,
                eth_dst=self.rmac,
                ip_src='20.20.20.1',
                ip_dst='10.10.10.1',
                ip_ttl=64,tcp_sport=100, with_tcp_chksum=True)
            udp_send_pkt = simple_udp_packet(pktlen=pkt_len,
                eth_dst=self.rmac,
                ip_src='20.20.20.1',
                ip_dst='10.10.10.1',
                ip_ttl=64,udp_sport=100)
            nat_pkt = simple_tcp_packet(pktlen=pkt_len,
                eth_dst='00:11:22:33:44:66',
                eth_src=self.rmac,
                ip_src='200.10.10.10',
                ip_dst='10.10.10.1',
                ip_ttl=63,tcp_sport=200, with_tcp_chksum=True)
            udp_nat_pkt = simple_udp_packet(pktlen=pkt_len,
                eth_dst='00:11:22:33:44:66',
                eth_src=self.rmac,
                ip_src='200.10.10.10',
                ip_dst='10.10.10.1',
                ip_ttl=63,udp_sport=200)
            normal_l3_recv_pkt = simple_tcp_packet(pktlen=pkt_len,
                eth_dst='00:11:22:33:44:66',
                eth_src=self.rmac,
                ip_src='20.20.20.1',
                ip_dst='10.10.10.1',
                ip_ttl=63,tcp_sport=100, with_tcp_chksum=True)

            self.SetNatZones(rif1=0, rif2=1, rif3=1)

            print("- With NAT disabled")
            self.AttachNatDisableAcl()
            send_packet(self, self.devports[0], send_pkt)
            verify_packets(self, normal_l3_recv_pkt, [self.devports[2]])
            self.CheckNatAclCounter()

            print("- With NAT enabled")
            self.DetachNatDisableAcl()
            send_packet(self, self.devports[0], send_pkt)
            verify_packets(self, nat_pkt, [self.devports[2]])
            self.AssertCounterEq(snapt_tcp_obj, 1, clear=True)

            print("- Same zone with NAT enabled, regular fowarding and no translation")
            send_packet(self, self.devports[1], send_pkt)
            verify_packets(self, normal_l3_recv_pkt, [self.devports[2]])

            #Send UDP packet
            send_packet(self, self.devports[0], udp_send_pkt)
            verify_packets(self, udp_nat_pkt, [self.devports[2]])
            self.AssertCounterEq(snapt_udp_obj, 1, clear=True)

            if not skipMissTest:
                print("- Outside to Inside zone -> DNAT Miss")
                # Change NAT zones and send same pkt from outside to inside -> DNAT
                # DNAT pool miss and DNAT table miss -> normal l3 forwarding
                # Goal: make sure pkt doesn't match the SNAPT rule configured above
                self.SetNatZones(rif1=1, rif2=0, rif3=0)
                send_packet(self, self.devports[0], send_pkt)
                verify_packets(self, normal_l3_recv_pkt, [self.devports[2]])

        finally:
            self.cleanlast()
            self.cleanlast()

    def VerifySrcNatTest(self, skipMissTest=False):
        print("Verify SNAT")
        if not self.HasFeature(SWITCH_FEATURE_BASIC_NAT):
            print("- Feature is not supported - skip test case")
            return
        try:
            snat_obj = self.add_nat_entry(
                self.device,
                src_ip_key='20.20.20.1', nat_src_ip='150.10.10.10',
                type=SWITCH_NAT_ENTRY_ATTR_TYPE_SOURCE_NAT)

            send_pkt = simple_tcp_packet(pktlen=pkt_len,
                eth_dst=self.rmac,
                ip_src='20.20.20.1',
                ip_dst='10.10.10.1',
                ip_ttl=64, with_tcp_chksum=True)
            nat_pkt = simple_tcp_packet(pktlen=pkt_len,
                eth_dst='00:11:22:33:44:66',
                eth_src=self.rmac,
                ip_src='150.10.10.10',
                ip_dst='10.10.10.1',
                ip_ttl=63, with_tcp_chksum=True)
            normal_l3_recv_pkt = simple_tcp_packet(pktlen=pkt_len,
                eth_dst='00:11:22:33:44:66',
                eth_src=self.rmac,
                ip_src='20.20.20.1',
                ip_dst='10.10.10.1',
                ip_ttl=63, with_tcp_chksum=True)

            icmp_pkt = simple_icmp_packet(pktlen=pkt_len, eth_dst=self.rmac,
                                          ip_src='20.20.20.1', ip_dst='10.10.10.1', ip_ttl=64)
            nat_icmp_pkt = simple_icmp_packet(pktlen=pkt_len, eth_src=self.rmac, eth_dst='00:11:22:33:44:66',
                                              ip_src='150.10.10.10', ip_dst='10.10.10.1', ip_ttl=63)

            self.SetNatZones(rif1=0, rif2=1, rif3=1)

            print("- With NAT disabled")
            self.AttachNatDisableAcl()
            send_packet(self, self.devports[0], send_pkt)
            verify_packets(self, normal_l3_recv_pkt, [self.devports[2]])
            self.CheckNatAclCounter()

            print("- With NAT enabled")
            self.DetachNatDisableAcl()
            send_packet(self, self.devports[0], send_pkt)
            verify_packets(self, nat_pkt, [self.devports[2]])
            self.AssertCounterEq(snat_obj, 1, clear=True)
            send_packet(self, self.devports[0], icmp_pkt)
            verify_packets(self, nat_icmp_pkt, [self.devports[2]])
            self.AssertCounterEq(snat_obj, 1, clear=True)

            if not skipMissTest:
                print("- Outside to Inside zone -> DNAT Miss")
                # Change NAT zones and send same pkt from outside to inside -> DNAT
                # DNAT pool miss and DNAT table miss -> normal l3 forwarding
                # Goal: make sure pkt doesn't match the SNAT rule configured above
                self.SetNatZones(rif1=1, rif2=0, rif3=0)
                send_packet(self, self.devports[0], send_pkt)
                verify_packets(self, normal_l3_recv_pkt, [self.devports[2]])

        finally:
            self.cleanlast()

    def AssertCounterEq(self, counter_obj, val, dump=False, clear=True):
        """ Assert value of counter and optionally dump it and clear it """
        cntr_ids = [SWITCH_NAT_ENTRY_COUNTER_ID_PKTS]

        counter = self.client.object_counters_get(counter_obj)
        self.assertEqual(counter[0].count, val)
        if clear:
            self.client.object_counters_clear(counter_obj, cntr_ids)
            counter = self.client.object_counters_get(counter_obj)
            self.assertEqual(counter[0].count, 0)

    def HasFeature(self, feature):
        return self.client.is_feature_enable(feature) != 0

class NatLagTest(NatTest):
    ''' Test NAT feature with LAG ports

    The main difference from the original NAT Test is the creation of RIF:
    instead of using a simple port, this test creates a LAG with a single port
    in it.

    The execution flow is the same, although for simplicity some of the tests
    are skipped.
    '''
    def runTest(self):
        if not self.HasFeature(SWITCH_FEATURE_NAT):
            print("NAT feature is not supported - Skip test run")
            return
        try:
            self.PrintNatTableSizes()
            # NAT test cases
            self.VerifyDestNaptTest(skipMissTest=True)
            self.VerifyDestNatTest(skipMissTest=True)
            # Dest NAT Pool before must be cleared before SNAT tests
            self.ClearDestNatPool()
            self.VerifySrcNaptTest(skipMissTest=True)
            self.VerifySrcNatTest(skipMissTest=True)

        finally:
            pass

    def AddRif(self, port):
        lag = self.add_lag(self.device)
        lag_mbr = self.add_lag_member(self.device, lag_handle=lag, port_handle=port)

        # Store lag for port0 so it can be used to attach/detach ACL
        if port == self.port0:
            self.lag = lag

        return self.add_rif(self.device,
                            type=SWITCH_RIF_ATTR_TYPE_PORT,
                            port_handle=lag,
                            vrf_handle=self.vrf10,
                            src_mac=self.rmac,
                            nat_zone=0)

    def AttachNatDisableAcl(self):
        # Note: to attach the ACL we need to specify the LAG attribute
        self.attribute_set(self.acl_entry, SWITCH_ACL_ENTRY_ATTR_ACTION_DISABLE_NAT, 1);
        self.attribute_set(self.lag, SWITCH_LAG_ATTR_INGRESS_ACL_HANDLE, self.acl_table)
        self.disable_nat = True

    def DetachNatDisableAcl(self):
        # Note: to detach the ACL we need to specify the LAG attribute
        self.attribute_set(self.acl_entry, SWITCH_ACL_ENTRY_ATTR_ACTION_DISABLE_NAT, 0);
        self.attribute_set(self.lag, SWITCH_LAG_ATTR_INGRESS_ACL_HANDLE, 0)
        self.disable_nat = False

class SimpleNatTest(ApiHelper):
    def setUp(self):
        self.configure()
        if (self.client.is_feature_enable(SWITCH_FEATURE_NAT) == 0):
            print("NAT feature is not supported - Skip test run")
            return
        self.rif1 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port0, vrf_handle=self.vrf10, src_mac=self.rmac, nat_zone=0)
        self.rif2 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port1, vrf_handle=self.vrf10, src_mac=self.rmac, nat_zone=1)
        self.rif3 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port2, vrf_handle=self.vrf10, src_mac=self.rmac, nat_zone=0)
        self.client_ip = '10.10.10.1'
        self.client_l4_port = 1000
        self.nat_client_ip = '100.100.0.1'
        self.client_nat_l4_port = 1111
        self.server_ip = '200.200.0.1'
        nat_nhop = self.add_nexthop(self.device, handle=self.rif2, dest_ip=self.server_ip)
        self.add_neighbor(self.device, mac_address='00:11:22:33:44:66', handle=self.rif2, dest_ip=self.server_ip)
        nat_route = self.add_route(self.device, ip_prefix='200.200.0.0/24', vrf_handle=self.vrf10, nexthop_handle=nat_nhop)

        lpm_nhop = self.add_nexthop(self.device, handle=self.rif1, dest_ip=self.client_ip)
        self.add_neighbor(self.device, mac_address='00:11:22:33:44:66', handle=self.rif1, dest_ip=self.client_ip)
        lpm_route = self.add_route(self.device, ip_prefix='10.10.10.0/24', vrf_handle=self.vrf10, nexthop_handle=lpm_nhop)

        self.snapt_obj = self.add_nat_entry(self.device, ip_proto_key = 6,
                                            src_ip_key=self.client_ip, l4_src_port_key=self.client_l4_port,
                                            nat_src_ip=self.nat_client_ip, nat_l4_src_port=self.client_nat_l4_port,
                                            type=SWITCH_NAT_ENTRY_ATTR_TYPE_SOURCE_NAPT)
        self.dnapt_obj = self.add_nat_entry(self.device, ip_proto_key=6,
                                            dst_ip_key=self.nat_client_ip, l4_dst_port_key=self.client_nat_l4_port,
                                            nat_dst_ip=self.client_ip, nat_l4_dst_port=self.client_l4_port,
                                            type=SWITCH_NAT_ENTRY_ATTR_TYPE_DESTINATION_NAPT)
        self.snat_obj = self.add_nat_entry(self.device,
                                            src_ip_key=self.client_ip,
                                            nat_src_ip=self.nat_client_ip,
                                            type=SWITCH_NAT_ENTRY_ATTR_TYPE_SOURCE_NAT)
        self.dnat_obj = self.add_nat_entry(self.device,
                                            dst_ip_key=self.nat_client_ip,
                                            nat_dst_ip=self.client_ip,
                                            type=SWITCH_NAT_ENTRY_ATTR_TYPE_DESTINATION_NAT)
        self.dnat_pool_obj = self.add_nat_entry(
            self.device, dst_ip_key=self.nat_client_ip,
            type=SWITCH_NAT_ENTRY_ATTR_TYPE_DESTINATION_NAT_POOL)
        self.acl_table = self.add_acl_table(self.device,
                                            type=SWITCH_ACL_TABLE_ATTR_TYPE_IPV4,
                                            bind_point_type=[acl_table_bp_port],
                                            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)
        self.acl_entry = self.add_acl_entry(self.device,
                                            table_handle=self.acl_table,
                                            dst_ip=self.server_ip, dst_ip_mask='255.255.255.255',
                                            action_disable_nat=0)

    def runTest(self):
        if (self.client.is_feature_enable(SWITCH_FEATURE_NAT) == 0):
            print("NAT feature is not supported - Skip test run")
            return
        self.validateSNAPT()
        self.validateDNAPT()
        self.validateSNAT()
        self.validateDNAT()
        self.validateDoNotNatAcl()

    def validateSNAPT(self):
        self.client_to_srv_pkt = simple_tcp_packet(pktlen=pkt_len,
              eth_dst=self.rmac,
              ip_src=self.client_ip,
              ip_dst=self.server_ip,
              ip_ttl=64,tcp_sport=self.client_l4_port, with_tcp_chksum=True)
        self.client_nat_to_srv_pkt = simple_tcp_packet(pktlen=pkt_len,
              eth_dst='00:11:22:33:44:66',
              eth_src=self.rmac,
              ip_src=self.nat_client_ip,
              ip_dst=self.server_ip,
              ip_ttl=63,tcp_sport=self.client_nat_l4_port, with_tcp_chksum=True)
        try:
            print("Validate source NAPT")
            send_packet(self, self.devports[0], self.client_to_srv_pkt)
            verify_packet(self, self.client_nat_to_srv_pkt, self.devports[1])
            counter = self.client.object_counters_get(self.snapt_obj)
            print("Counter %d"%(counter[SWITCH_NAT_ENTRY_COUNTER_ID_PKTS].count))
        finally:
            pass

    def validateDNAPT(self):
        self.srv_to_client_pkt = simple_tcp_packet(pktlen=pkt_len,
              eth_dst=self.rmac,
              ip_src=self.server_ip,
              ip_dst=self.nat_client_ip,
              ip_ttl=64,tcp_dport=self.client_nat_l4_port, with_tcp_chksum=True)
        self.srv_to_client_nat_pkt = simple_tcp_packet(pktlen=pkt_len,
              eth_dst='00:11:22:33:44:66',
              eth_src=self.rmac,
              ip_src=self.server_ip,
              ip_dst=self.client_ip,
              ip_ttl=63,tcp_dport=self.client_l4_port, with_tcp_chksum=True)
        try:
            print("Validate destination NAPT")
            send_packet(self, self.devports[1], self.srv_to_client_pkt)
            verify_packet(self, self.srv_to_client_nat_pkt, self.devports[0])
            counter = self.client.object_counters_get(self.dnapt_obj)
            print("Counter %d"%(counter[SWITCH_NAT_ENTRY_COUNTER_ID_PKTS].count))
        finally:
            pass

    def validateDoNotNatAcl(self):
        self.client_to_srv_pkt = simple_tcp_packet(pktlen=pkt_len,
              eth_dst=self.rmac,
              ip_src=self.client_ip,
              ip_dst=self.server_ip,
              ip_ttl=64,tcp_sport=self.client_l4_port, with_tcp_chksum=True)
        self.client_nat_to_srv_pkt = simple_tcp_packet(pktlen=pkt_len,
              eth_dst='00:11:22:33:44:66',
              eth_src=self.rmac,
              ip_src=self.nat_client_ip,
              ip_dst=self.server_ip,
              ip_ttl=63,tcp_sport=self.client_nat_l4_port, with_tcp_chksum=True)
        self.client_no_nat_to_srv_pkt = simple_tcp_packet(pktlen=pkt_len,
              eth_dst='00:11:22:33:44:66',
              eth_src=self.rmac,
              ip_src=self.client_ip,
              ip_dst=self.server_ip,
              ip_ttl=63,tcp_sport=self.client_l4_port, with_tcp_chksum=True)
        try:
            print("Validate source NAPT")
            send_packet(self, self.devports[2], self.client_to_srv_pkt)
            verify_packet(self, self.client_nat_to_srv_pkt, self.devports[1])
            print("Apply ACL to disable NAT")
            self.attribute_set(self.acl_entry, SWITCH_ACL_ENTRY_ATTR_ACTION_DISABLE_NAT, 1);
            self.attribute_set(self.port2, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, self.acl_table)
            send_packet(self, self.devports[2], self.client_to_srv_pkt)
            verify_packet(self, self.client_no_nat_to_srv_pkt, self.devports[1])
            counter = self.client.object_counters_get(self.acl_entry)
            self.assertEqual(counter[SWITCH_ACL_ENTRY_COUNTER_ID_PKTS].count, 1)
            cntr_ids = [SWITCH_ACL_ENTRY_COUNTER_ID_PKTS]
            self.client.object_counters_clear(self.acl_entry, cntr_ids)
        finally:
            self.attribute_set(self.acl_entry, SWITCH_ACL_ENTRY_ATTR_ACTION_DISABLE_NAT, 0);
            self.attribute_set(self.port2, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            pass

    def validateSNAT(self):
        self.client_to_srv_udp_pkt = simple_udp_packet(pktlen=pkt_len,
              eth_dst=self.rmac,
              ip_src=self.client_ip,
              ip_dst=self.server_ip,
              ip_ttl=64)
        self.client_nat_to_srv_udp_pkt = simple_udp_packet(pktlen=pkt_len,
              eth_dst='00:11:22:33:44:66',
              eth_src=self.rmac,
              ip_src=self.nat_client_ip,
              ip_dst=self.server_ip,
              ip_ttl=63)
        try:
            print("Validate source NAT")
            send_packet(self, self.devports[0], self.client_to_srv_udp_pkt)
            verify_packet(self, self.client_nat_to_srv_udp_pkt, self.devports[1])
            counter = self.client.object_counters_get(self.snat_obj)
            print("Counter %d"%(counter[SWITCH_NAT_ENTRY_COUNTER_ID_PKTS].count))
        finally:
            pass

    def validateDNAT(self):
        self.srv_to_client_udp_pkt = simple_udp_packet(pktlen=pkt_len,
              eth_dst=self.rmac,
              ip_src=self.server_ip,
              ip_dst=self.nat_client_ip,
              ip_ttl=64)
        self.srv_to_client_nat_udp_pkt = simple_udp_packet(pktlen=pkt_len,
              eth_dst='00:11:22:33:44:66',
              eth_src=self.rmac,
              ip_src=self.server_ip,
              ip_dst=self.client_ip,
              ip_ttl=63)
        try:
            print("Validate destination NAT")
            send_packet(self, self.devports[1], self.srv_to_client_udp_pkt)
            verify_packet(self, self.srv_to_client_nat_udp_pkt, self.devports[0])
            counter = self.client.object_counters_get(self.dnat_obj)
            print("Counter %d"%(counter[SWITCH_NAT_ENTRY_COUNTER_ID_PKTS].count))
        finally:
            pass

    def tearDown(self):
        self.attribute_set(self.port2, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
        self.cleanup()

class PktBuilder(dict):
    ''' Helper class to build packets.

    It behaves like a Python dictionary.
    Fields are added/edited using dot notation.
    The actual packet is created using build() method.
    '''
    def __getattr__(self, key):
        if key not in self:
            raise AttributeError(key)
        return self[key]

    def __setattr__(self, key, value):
        self[key] = value

    def build(self, create_pkt_function):
        return create_pkt_function(**self)

