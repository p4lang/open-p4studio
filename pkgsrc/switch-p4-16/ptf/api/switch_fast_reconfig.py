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
from ptf.packet import *
from ptf.thriftutils import *
import ptf.mask as mask

import os

from bf_switcht_api_thrift.ttypes import *
from bf_switcht_api_thrift.model_headers import *
from devport_mgr_pd_rpc.ttypes import *

this_dir = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(this_dir, '..'))
from common.utils import *
from ast import literal_eval

import switch_l2
import switch_qos
import switch_hostif
import switch_l3
import switch_qos
import switch_mirror
import switch_pfcwd
import switch_dtel
import switch_acl
import switch_tests
import switch_etrap
import switch_tunnel
import switch_hash
import switch_mpls
import switch_nat
import switch_scale

###############################################################################
#Utility Function
def reboot_type():
    test_params = ptf.testutils.test_params_get()
    hitless = False
    if 'reboot' in list(test_params.keys()):
        if test_params['reboot'] == 'hitless':
            hitless = True
    return hitless

###############################################################################
class Replay:

    @staticmethod
    def startReplay(self):
        hitless = reboot_type()
        try:
            if hitless:
                print("Start hitless")
                self.warm_init_begin(self.dev_id)
                print("End hitless")
                self.warm_init_end(self.dev_id)
            else:
                print("Start fast reconfig")
                self.fast_reconfig_begin(self.dev_id)
                print("End fast reconfig")
                self.fast_reconfig_end(self.dev_id)
        finally:
            pass

###############################################################################
@disabled
class PortBasicTest(ApiHelper):

    def runTest(self):
        print()
        try:
            self.TrafficTestConfig()
            Replay.startReplay(self)
        finally:
            self.cleanup()

    def TrafficTestConfig(self):
        self.device = self.get_device_handle(0)
        self.port0 = self.add_port(self.device, lane_list=u.lane_list_t([0]), speed=25000)

###############################################################################
@disabled
class PortScaleTest(ApiHelper):
    def setUp(self):
        self.currTest = switch_scale.PortScaleTest()
        self.currTest.setUp()
    def tearDown(self):
        self.currTest.tearDown()
    def runBaseTests(self):
        self.currTest.runTest()

    def runTest(self):
        try:
            self.runBaseTests()
            Replay.startReplay(self.currTest)
            self.runBaseTests()
        finally:
            pass

###############################################################################
@disabled
class LagScaleTest(ApiHelper):
    def setUp(self):
        self.currTest = switch_scale.LagScaleTest()
        self.currTest.setUp()
    def tearDown(self):
        self.currTest.tearDown()
    def runBaseTests(self):
        self.currTest.runTest()

    def runTest(self):
        try:
            self.runBaseTests()
            Replay.startReplay(self.currTest)
            self.runBaseTests()
        finally:
            pass

###############################################################################
@disabled
class L2MacTest(ApiHelper):
    def setUp(self):
        self.currTest = switch_l2.L2MacTest()
        self.currTest.setUp()
    def tearDown(self):
        self.currTest.tearDown()
    def runBaseTests(self):
        self.currTest.StaticMacMoveTest()
        self.currTest.MacLearnTest()
        self.currTest.DynamicMacLearnTest()
        self.currTest.DynamicMacMoveTest()
        self.currTest.DynamicLearnAgeTest()

    def runTest(self):
        try:
            self.runBaseTests()
            Replay.startReplay(self.currTest)
            self.runBaseTests()
        finally:
            pass

###############################################################################
@disabled
class L2VlanTest(ApiHelper):
    def setUp(self):
        self.currTest = switch_l2.L2VlanTest()
        self.currTest.setUp()
    def tearDown(self):
        self.currTest.tearDown()
    def runBaseTests(self):
        self.currTest.AccessToAccessConfigureTest()
        self.currTest.AccessToTrunkConfigureTest()
        self.currTest.TrunkToAccessConfigureTest()
        self.currTest.AccessToAccessTest()
        self.currTest.TrunkToTrunkTest()
        self.currTest.AccessToTrunkTest()
        self.currTest.TrunkToAccessTest()
        self.currTest.AccessToPriorityTest()
        self.currTest.PriorityToTrunkTest()
        self.currTest.PriorityPcpToTrunkTest()
        self.currTest.PriorityToAccessTest()
        self.currTest.TrunkToPriorityTest()
        self.currTest.TagToUntagTest()
        self.currTest.UnTagToTagTest()
        self.currTest.PVDropTest()

    def runTest(self):
        try:
            self.runBaseTests()
            Replay.startReplay(self.currTest)
            self.runBaseTests()
        finally:
            pass

###############################################################################
@disabled
class VlanBasicTest(ApiHelper):

    def runTest(self):
        print()
        try:
            self.TrafficTestConfig()
            Replay.startReplay(self)
        finally:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.cleanup()

    def TrafficTestConfig(self):
        self.device = self.get_device_handle(0)
        self.vlan10 = self.add_vlan(self.device, vlan_id=10)
        self.port0 = self.add_port(self.device, lane_list=u.lane_list_t([0]), speed=25000)
        self.port1 = self.add_port(self.device, lane_list=u.lane_list_t([1]), speed=25000)
        self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port0)
        self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port1,
            tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan10,
            mac_address='00:11:22:33:44:55', destination_handle=self.port0)
        mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan10,
            mac_address='00:11:22:33:44:66', destination_handle=self.port1)

###############################################################################
@disabled
class L2FloodTest(ApiHelper):
    def setUp(self):
        self.currTest = switch_l2.L2FloodTest()
        self.currTest.setUp()
    def tearDown(self):
        self.currTest.tearDown()
    def runBaseTests(self):
        self.currTest.FloodDisableTest()
        self.currTest.Flood4MemberTest()

    def runTest(self):
        try:
            self.runBaseTests()
            Replay.startReplay(self.currTest)
            self.runBaseTests()
        finally:
            pass

###############################################################################
@disabled
class L2LagTest(ApiHelper):
    def setUp(self):
        self.currTest = switch_l2.L2LagTest()
        self.currTest.setUp()
    def tearDown(self):
        self.currTest.tearDown()
    def runBaseTests(self):
        #self.currTest.LagBasicTest()
        self.currTest.LagLearnFlagTest()
        self.currTest.LagMacLearnTest()
        self.currTest.LagFloodTest()
        self.currTest.LagFloodPruneTest()
        self.currTest.LagPVMissTest()
        self.currTest.LagMemberActivateBridgeTest()
        self.currTest.LagMemberActivateFloodTest()
        self.currTest.AccessModeTest()
        self.currTest.TrunkModeTest()
        self.currTest.LagIngressAclTest()
        self.currTest.LagEgressAclTest()
        self.currTest.LagQoSGroupTest()

    def runTest(self):
        try:
            self.runBaseTests()
            Replay.startReplay(self.currTest)
            self.runBaseTests()
        finally:
            pass

###############################################################################
@disabled
class L3LagTest(ApiHelper):
    def setUp(self):
        self.currTest = switch_l3.L3LagTest()
        self.currTest.setUp()
    def tearDown(self):
        self.currTest.tearDown()
    def runBaseTests(self):
        self.currTest.runTest()
    def runTest(self):
        try:
            self.runBaseTests()
            Replay.startReplay(self.currTest)
            self.runBaseTests()
        finally:
            pass

###############################################################################
@disabled
class L3LagAddDeleteTest(ApiHelper):
    def setUp(self):
        self.currTest = switch_l3.L3LagAddDeleteTest()
        self.currTest.setUp()
    def tearDown(self):
        self.currTest.tearDown()
    def runTest(self):
        try:
            self.currTest.runPreTest()
            Replay.startReplay(self.currTest)
            self.currTest.runPostTest()
        finally:
            pass

###############################################################################
@disabled
class LagBasicTest(ApiHelper):

    def runTest(self):
        print()
        try:
            self.TrafficTestConfig()
            self.Test()
            Replay.startReplay(self)
            self.Test()
        finally:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.lag0, SWITCH_LAG_ATTR_PORT_VLAN_ID, 0)
            self.cleanup()

    def TrafficTestConfig(self):
        self.device = self.get_device_handle(0)
        self.vlan10 = self.add_vlan(self.device, vlan_id=10)
        self.port0 = self.add_port(self.device, lane_list=u.lane_list_t([0]), speed=25000)
        self.port1 = self.add_port(self.device, lane_list=u.lane_list_t([1]), speed=25000)
        self.port2 = self.add_port(self.device, lane_list=u.lane_list_t([2]), speed=25000)
        self.lag0 = self.add_lag(self.device)
        self.lag0_mbr0 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port1)
        self.lag0_mbr1 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port2)
        self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port0)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.lag0)
        self.attribute_set(self.lag0, SWITCH_LAG_ATTR_PORT_VLAN_ID, 10)
        mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan10,
            mac_address='00:11:22:33:44:55', destination_handle=self.port0)
        mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan10,
            mac_address='00:11:22:33:44:66', destination_handle=self.lag0)
    def Test(self):
        pkt = simple_tcp_packet(eth_dst='00:11:22:33:44:66', eth_src='00:11:22:33:44:55')
        try:
            send_packet(self, 0, pkt)
            verify_any_packet_any_port(self, [pkt, pkt], [1,2])
        finally:
            pass

###############################################################################
@disabled
class L3TableFillTest(ApiHelper):
    def setUp(self):
        self.currTest = switch_l3.L3TableFillTest()
        self.currTest.setUp()
    def tearDown(self):
        self.currTest.tearDown()
    def runBaseTests(self):
        self.currTest.runTest()

    def runTest(self):
        try:
            self.runBaseTests()
            Replay.startReplay(self.currTest)
            self.runBaseTests()
        finally:
            pass

###############################################################################
@disabled
class L3MymacTest(ApiHelper):
    def setUp(self):
        self.currTest = switch_l3.L3MymacTest()
        self.currTest.setUp()
    def tearDown(self):
        self.currTest.tearDown()
    def runBaseTests(self):
        self.currTest.MymacUpdateTest()
        self.currTest.MymacAnyRifTest()
        self.currTest.MymacSVITest()
        self.currTest.MymacDropL2Test()
        self.currTest.MymacSubPortTest()

    def runTest(self):
        try:
            self.runBaseTests()
            Replay.startReplay(self.currTest)
            self.runBaseTests()
        finally:
            pass

###############################################################################
@disabled
class L3RouteTest(ApiHelper):
    def setUp(self):
        self.currTest = switch_l3.L3RouteTest()
        self.currTest.setUp()
    def tearDown(self):
        self.currTest.tearDown()
    def runBaseTests(self):
        self.currTest.routeActionsTest()

    def runTest(self):
        try:
            self.runBaseTests()
            Replay.startReplay(self.currTest)
            self.runBaseTests()
        finally:
            pass

###############################################################################
@disabled
class L3WCMPTest(ApiHelper):
    def setUp(self):
        self.currTest = switch_l3.L3WCMPTest()
        self.currTest.setUp()
    def tearDown(self):
        self.currTest.tearDown()
    def runBaseTests(self):
        self.currTest.IPv4Test(False)
        self.currTest.AddDeleteTest()

    def runTest(self):
        try:
            self.runBaseTests()
            Replay.startReplay(self.currTest)
            self.runBaseTests()
        finally:
            pass

###############################################################################
@disabled
class L3InterfaceTest(ApiHelper):
    def setUp(self):
        self.currTest = switch_l3.L3InterfaceTest()
        self.currTest.setUp()
    def tearDown(self):
        self.currTest.tearDown()
    def runBaseTests(self):
        self.currTest.RifVrfUpdateTest()
        self.currTest.RifIPv4DisableTest()
        self.currTest.RifIPv6DisableTest()
        self.currTest.RifRmacUpdateTest()
        self.currTest.IPv4FibTest()
        self.currTest.IPv4FibLocalHostTest()
        self.currTest.IPv4FibJumboTest()
        self.currTest.IPv4FibLPMTest()
        self.currTest.IPv4MtuTest()
        self.currTest.IPv6MtuTest()
        self.currTest.IPv4FibModifyTest()
        self.currTest.IPv4FibResolutionTest()
        self.currTest.IPv6FibTest()
        self.currTest.IPv6FibLPMTest()
        self.currTest.IPv6FibModifyTest()
        self.currTest.IPv6FibResolutionTest()
        self.currTest.IPv4FibLagTest()
        self.currTest.IPv6FibLagTest()
        self.currTest.IPv4FibGleanTest()

    def runTest(self):
        try:
            self.runBaseTests()
            Replay.startReplay(self.currTest)
            self.runBaseTests()
        finally:
            pass

###############################################################################
@disabled
class L3SubPortTest(ApiHelper):
    def setUp(self):
        self.currTest = switch_l3.L3SubPortTest()
        self.currTest.setUp()
    def tearDown(self):
        self.currTest.tearDown()
    def runBaseTests(self):
        self.currTest.RifVrfUpdateTest()
        self.currTest.RifToSubPortTest()
        self.currTest.SubPortToRifTest()
        self.currTest.SubPortToSubPortTest()
        self.currTest.SubPortIP2METest()
        self.currTest.SubPortECMPTest()
        self.currTest.PVMissTest()
        self.currTest.SubPortAdminStatusTest()
        self.currTest.SubportIngressAclTest()
        self.currTest.SubportEgressAclTest()
        self.currTest.SubportQoSGroupTest()

    def runTest(self):
        try:
            self.runBaseTests()
            Replay.startReplay(self.currTest)
            self.runBaseTests()
        finally:
            pass

###############################################################################

@disabled
class L3BasicTest(ApiHelper):

    def runTest(self):
        print()
        try:
            self.TrafficTestConfig()
            Replay.startReplay(self)
        finally:
            self.attribute_set(self.rif0, SWITCH_RIF_ATTR_MTU, 0)
            self.cleanup()

    def TrafficTestConfig(self):
        self.device = self.get_device_handle(0)
        self.rmac = '00:77:66:55:44:33'
        self.vrf10 = self.add_vrf(self.device, id=10, src_mac=self.rmac)
        self.port0 = self.add_port(self.device, lane_list=u.lane_list_t([0]), speed=25000)
        self.rif0 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT,
            port_handle=self.port0, vrf_handle=self.vrf10, src_mac=self.rmac)
        nhop0 = self.add_nexthop(self.device, handle=self.rif0, dest_ip='10.10.0.2')
        neighbor0 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:55',
            handle=self.rif0, dest_ip='10.10.0.2')
        self.add_route(self.device, ip_prefix='11.11.11.1', vrf_handle=self.vrf10, nexthop_handle=nhop0)
        self.attribute_set(self.rif0, SWITCH_RIF_ATTR_MTU, 600)

###############################################################################
@disabled
class RouteBasicTest(ApiHelper):

    def runTest(self):
        print()
        try:
            self.TrafficTestConfig()
            Replay.startReplay(self)
        finally:
            self.cleanup()

    def TrafficTestConfig(self):
        self.device = self.get_device_handle(0)
        self.rmac = '00:77:66:55:44:33'
        self.vrf10 = self.add_vrf(self.device, id=10, src_mac=self.rmac)
        self.port0 = self.add_port(self.device, lane_list=u.lane_list_t([0]), speed=25000)
        rif0 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT,
            port_handle=self.port0, vrf_handle=self.vrf10, src_mac=self.rmac)
        nhop0 = self.add_nexthop(self.device, handle=rif0, dest_ip='10.10.0.2')
        neighbor0 = self.add_neighbor(self.device, mac_address='00:11:22:33:44:55', handle=rif0, dest_ip='10.10.0.2')
        self.route0 = self.add_route(self.device, ip_prefix='11.11.11.1', vrf_handle=self.vrf10, nexthop_handle=nhop0)
        self.route0_lpm = self.add_route(self.device, ip_prefix='12.12.12.0/24', vrf_handle=self.vrf10, nexthop_handle=nhop0)

###############################################################################
@disabled
class L3MultipleSVITest(ApiHelper):
    def setUp(self):
        self.currTest = switch_l3.L3MultipleSVITest()
        self.currTest.setUp()
    def tearDown(self):
        self.currTest.tearDown()
    def runBaseTests(self):
        self.currTest.runTest()

    def runTest(self):
        try:
            self.runBaseTests()
            Replay.startReplay(self.currTest)
            self.runBaseTests()
        finally:
            pass


###############################################################################
@disabled
class L3SVITest(ApiHelper):
    def setUp(self):
        self.currTest = switch_l3.L3SVITest()
        self.currTest.setUp()
    def tearDown(self):
        self.currTest.tearDown()
    def runBaseTests(self):
        self.currTest.SVIRifVrfUpdateTest()
        self.currTest.SVIRifIPv4DisableTest()
        self.currTest.SVIRifIPv6DisableTest()
        self.currTest.SVIHostIngressVlanZeroTest()
        self.currTest.SVIBridgingTest()
        self.currTest.SVIHostTest()
        self.currTest.SVIHostVlanTaggingTest()
        self.currTest.SVIGleanTest()
        self.currTest.SVIHostPostRoutedGleanTest()
        self.currTest.SVIHostStaticMacMoveTest()
        self.currTest.SVIRouteDynamicMacTest()
        self.currTest.SVIRouteDynamicMacMoveTest()
        self.currTest.SVIArpMoveTest()
        self.currTest.IPv4SVILagHostTest()
        self.currTest.IPv4SVILagHostDynamicMacTest()
        self.currTest.IPv6SVILagHostTest()
        self.currTest.SVIMtuTest()

    def runTest(self):
        try:
            self.runBaseTests()
            Replay.startReplay(self.currTest)
            self.runBaseTests()
        finally:
            pass

###############################################################################
@disabled
class L3SharedNeighborTest(ApiHelper):
    def setUp(self):
        self.currTest = switch_l3.L3SharedNeighborTest()
        self.currTest.setUp()
    def tearDown(self):
        self.currTest.tearDown()
    def runBaseTests(self):
        self.currTest.runTest()

    def runTest(self):
        try:
            self.runBaseTests()
            Replay.startReplay(self.currTest)
            self.runBaseTests()
        finally:
            pass

###############################################################################
@disabled
class SVIBasicTest(ApiHelper):

    def runTest(self):
        print()
        try:
            self.TrafficTestConfig()
            Replay.startReplay(self)
        finally:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.cleanup()

    def TrafficTestConfig(self):
        self.device = self.get_device_handle(0)
        self.rmac = '00:77:66:55:44:33'
        self.vrf10 = self.add_vrf(self.device, id=10, src_mac=self.rmac)
        self.port0 = self.add_port(self.device, lane_list=u.lane_list_t([0]), speed=25000)
        self.vlan10 = self.add_vlan(self.device, vlan_id=10)
        self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port0)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        rif0 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_VLAN,
            vlan_handle=self.vlan10, vrf_handle=self.vrf10, src_mac=self.rmac)

###############################################################################
@disabled
class L3ECMPTest(ApiHelper):
    def setUp(self):
        self.currTest = switch_l3.L3ECMPTest()
        self.currTest.setUp()
    def tearDown(self):
        self.currTest.tearDown()
    def runBaseTests(self):
        self.currTest.EcmpIPv4HostTest()
        self.currTest.EcmpIPv4LpmTest()
        self.currTest.EcmpIPv6HostTest()
        self.currTest.EcmpIPv6LpmTest()
        self.currTest.EcmpIPv4LagTest()

    def runTest(self):
        try:
            self.runBaseTests()
            Replay.startReplay(self.currTest)
            self.runBaseTests()
        finally:
            pass

###############################################################################
@disabled
class L3ECMPDynamicOrderingTest(ApiHelper):
    def setUp(self):
        self.currTest = switch_l3.L3ECMPDynamicOrderingTest()
        self.currTest.setUp()
    def tearDown(self):
        self.currTest.tearDown()
    def runBaseTests(self):
        self.currTest.runTest()

    def runTest(self):
        try:
            self.runBaseTests()
            Replay.startReplay(self.currTest)
            self.runBaseTests()
        finally:
            pass

###############################################################################
@disabled
class L3ECMPLBTest(ApiHelper):
    def setUp(self):
        self.currTest = switch_l3.L3ECMPLBTest()
        self.currTest.setUp()
    def tearDown(self):
        self.currTest.tearDown()
    def runBaseTests(self):
        self.currTest.LBTest()

    def runTest(self):
        try:
            self.runBaseTests()
            Replay.startReplay(self.currTest)
            self.runBaseTests()
        finally:
            pass

###############################################################################
@disabled
class L3ECMPAddDeleteTest(ApiHelper):
    def setUp(self):
        self.currTest = switch_l3.L3ECMPAddDeleteTest()
        self.currTest.setUp()
    def tearDown(self):
        self.currTest.tearDown()
    def runBaseTests(self):
        self.currTest.runTest()

    def runTest(self):
        try:
            self.runBaseTests()
            Replay.startReplay(self.currTest)
            self.runBaseTests()
        finally:
            pass

###############################################################################
@disabled
class L3ECMPSharedNhopTest(ApiHelper):
    def setUp(self):
        self.currTest = switch_l3.L3ECMPSharedNhopTest()
        self.currTest.setUp()
    def tearDown(self):
        self.currTest.tearDown()
    def runBaseTests(self):
        self.currTest.runTest()

    def runTest(self):
        try:
            self.runBaseTests()
            Replay.startReplay(self.currTest)
            self.runBaseTests()
        finally:
            pass

###############################################################################
@disabled
class ECMPBasicTest(ApiHelper):

    def runTest(self):
        print()
        try:
            self.TrafficTestConfig()
            Replay.startReplay(self)
        finally:
            self.cleanup()

    def TrafficTestConfig(self):
        self.device = self.get_device_handle(0)
        self.rmac = '00:77:66:55:44:33'
        self.vrf10 = self.add_vrf(self.device, id=10, src_mac=self.rmac)
        self.port0 = self.add_port(self.device, lane_list=u.lane_list_t([0]), speed=25000)
        rif0 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT,
            port_handle=self.port0, vrf_handle=self.vrf10, src_mac=self.rmac)
        nhop0 = self.add_nexthop(self.device, handle=rif0, dest_ip='10.10.0.2')
        ecmp0 = self.add_ecmp(self.device)
        ecmp_member01 = self.add_ecmp_member(self.device, nexthop_handle=nhop0, ecmp_handle=ecmp0)
        ecmp_member02 = self.add_ecmp_member(self.device, nexthop_handle=nhop0, ecmp_handle=ecmp0)
        ecmp_member03 = self.add_ecmp_member(self.device, nexthop_handle=nhop0, ecmp_handle=ecmp0)

###############################################################################

@disabled
class L3MultiVrfTest(ApiHelper):
    def setUp(self):
        self.currTest = switch_l3.L3MultiVrfTest()
        self.currTest.setUp()

    def tearDown(self):
        self.currTest.tearDown()

    def runBaseTests(self):
        self.currTest.L3VrfRmacTest()
        self.currTest.L3VrfIsolationTests()
        self.currTest.L3InterVrfAclRedirectTest()
        self.currTest.L3VrfInterfacesTest()
        self.currTest.L3VrfScaleTest()

    def runTest(self):
        try:
            self.runBaseTests()
            Replay.startReplay(self.currTest)
            self.runBaseTests()
        finally:
            pass

###############################################################################
@disabled
class HostifTrapTest(ApiHelper):
    def runTest(self):
        print()
        try:
            self.TrafficTestConfig()
            Replay.startReplay(self)
        finally:
            self.cleanup()

    def TrafficTestConfig(self):
        self.device = self.get_device_handle(0)
        self.cpu_port_hdl = self.attribute_get(self.device, SWITCH_DEVICE_ATTR_CPU_PORT)
        self.meter1 = self.add_meter(self.device,
            mode=SWITCH_METER_ATTR_MODE_TWO_RATE_THREE_COLOR,
            pbs=16,cbs=8,cir=1000,pir=1000,
            type=SWITCH_METER_ATTR_TYPE_PACKETS,
            color_source=SWITCH_METER_ATTR_COLOR_SOURCE_BLIND)
        queue_handles = self.attribute_get(self.cpu_port_hdl, SWITCH_PORT_ATTR_QUEUE_HANDLES)
        self.hostif_trap_group0 = self.add_hostif_trap_group(self.device,
                                                             queue_handle=queue_handles[0].oid,
                                                             admin_state=True,
                                                             policer_handle=self.meter1)
        self.add_hostif_trap(self.device, type=SWITCH_HOSTIF_TRAP_ATTR_TYPE_ARP_REQUEST,
                             hostif_trap_group_handle=self.hostif_trap_group0,
                             packet_action=SWITCH_HOSTIF_TRAP_ATTR_PACKET_ACTION_COPY_TO_CPU)
        self.port0 = self.add_port(self.device, lane_list=u.lane_list_t([0]), speed=25000)
        hostif_name1 = "Ethernet0"
        self.hostif0 = self.add_hostif(self.device, name=hostif_name1, handle=self.port0, oper_status=True)

###############################################################################
@disabled
class HostIfRxTest(ApiHelper):
    def setUp(self):
        self.currTest = switch_hostif.HostIfRxTest()
        self.currTest.setUp()

    def tearDown(self):
        self.currTest.tearDown()

    def runBaseTests(self):
        self.currTest.sock = open_packet_socket(self.currTest.hostif_name1)
        self.currTest.rx_cnt = 0
        self.currTest.pre_counter = self.currTest.client.object_counters_get(self.currTest.hostif0)
        self.currTest.sock2 = open_packet_socket(self.currTest.hostif_name2)
        self.currTest.rx_cnt2 = 0
        self.currTest.pre_counter2 = self.currTest.client.object_counters_get(self.currTest.hostif1)
        self.currTest.sock3 = open_packet_socket(self.currTest.hostif_name3)
        self.currTest.rx_cnt3 = 0
        self.currTest.pre_counter3 = self.currTest.client.object_counters_get(self.currTest.hostif2)
        self.currTest.ArpTest()
        self.currTest.OspfTest()
        self.currTest.IgmpTest()
        self.currTest.PimTest()
        self.currTest.StpTest()
        self.currTest.UdldTest()
        self.currTest.DhcpTest()
        self.currTest.BgpTest()
        self.currTest.ICMPv6Test()
        #self.currTest.BfdRxTest()
        self.currTest.LacpTest()
        self.currTest.LLDPTest()
        self.currTest.DHCPv6Test()
        self.currTest.HostifStatsTest()
    def runTest(self):
        try:
            self.runBaseTests()
            Replay.startReplay(self.currTest)
            self.runBaseTests()
        finally:
            pass

###############################################################################
@disabled
class HostIfPingTest(ApiHelper):
    def setUp(self):
        self.currTest = switch_hostif.HostIfPingTest()
        self.currTest.setUp()

    def tearDown(self):
        self.currTest.tearDown()

    def runBaseTests(self):
        print("Test ping on rif0")
        self.currTest.PingPortTest('30.30.0.1', '30.30.0.3', '00:06:07:08:09:0a',
                                   self.currTest.devports[0], [self.currTest.devports[0]])
        print("Test ping on rif1")
        #self.currTest.PingPortTest('20.20.0.1', '20.20.0.3', '00:06:07:08:09:0b',
        #                           self.currTest.devports[1], [self.currTest.devports[1],
        #                           self.currTest.devports[2],
        #                           self.currTest.devports[3], self.currTest.devports[4]])

    def runTest(self):
        try:
            self.runBaseTests()
            Replay.startReplay(self.currTest)
            self.runBaseTests()
        finally:
            pass

###############################################################################
@disabled
class HostIfRxFilterTest(ApiHelper):
    def setUp(self):
        self.currTest = switch_hostif.HostIfRxFilterTest()
        self.currTest.setUp()

    def tearDown(self):
        self.currTest.tearDown()

    def runBaseTests(self):
        try:
            print("run StpTest()")
            #self.currTest.StpTest()
            print("Test ping on svi_rif0")
            self.currTest.SviPingTest('30.30.0.1', '30.30.0.3', '00:06:07:08:09:0a',
                                      self.currTest.devports[2], [self.currTest.devports[2]])
        finally:
            pass

    def runTest(self):
        try:
            self.runBaseTests()
            Replay.startReplay(self.currTest)
            self.runBaseTests()
        finally:
            pass

###############################################################################
@disabled
class HostIfCoppTest(ApiHelper):
    def setUp(self):
        self.currTest = switch_hostif.HostIfCoppTest()
        self.currTest.setUp()

    def tearDown(self):
        self.currTest.tearDown()

    def runBaseTests(self, check_stats_count):
        self.currTest.HostIfCoppBasicTest(check_stats_count)

    def runTest(self):
        try:
            self.runBaseTests(4)
            Replay.startReplay(self.currTest)
            print("Checking if counters are restored across fast-reconfig")
            self.runBaseTests(8)
        finally:
            pass

###############################################################################
@disabled
class ExceptionPacketsTest(ApiHelper):
    def setUp(self):
        self.currTest = switch_hostif.ExceptionPacketsTest()
        self.currTest.setUp()

    def tearDown(self):
        self.currTest.tearDown()

    def runBaseTests(self):
        self.currTest.IPv4ExceptionTest()
        self.currTest.IPv6ExceptionTest()

    def runTest(self):
        try:
            self.runBaseTests()
            Replay.startReplay(self.currTest)
            self.runBaseTests()
        finally:
            pass

###############################################################################
@disabled
class IPv4MalformedPacketsTest(ApiHelper):
    def setUp(self):
        self.currTest = switch_hostif.IPv4MalformedPacketsTest()
        self.currTest.setUp()

    def tearDown(self):
        self.currTest.tearDown()

    def runBaseTests(self, post_hiteless=False):
        return self.currTest.runTest(post_hiteless)

    def runTest(self):
        try:
            pre_counter_count = self.runBaseTests()
            Replay.startReplay(self.currTest)
            counter_count = self.runBaseTests(True)
            print("Checking if counters are restored across fast-reconfig")
            print("Pre fast-reconfig counter count: %d" % pre_counter_count)
            print("Post fast-reconfig counter count: %d" % counter_count)
            self.assertGreater(counter_count, pre_counter_count)
        finally:
            pass

###############################################################################
@disabled
class IPv6MalformedPacketsTest(ApiHelper):
    def setUp(self):
        self.currTest = switch_hostif.IPv6MalformedPacketsTest()
        self.currTest.setUp()

    def tearDown(self):
        self.currTest.tearDown()

    def runBaseTests(self, post_hiteless=False):
        return self.currTest.runTest(post_hiteless)

    def runTest(self):
        try:
            pre_counter_count = self.runBaseTests()
            Replay.startReplay(self.currTest)
            counter_count = self.runBaseTests(True)
            print("Checking if counters are restored across fast-reconfig")
            print("Pre fast-reconfig counter count: %d" % pre_counter_count)
            print("Post fast-reconfig counter count: %d" % counter_count)
            self.assertGreater(counter_count, pre_counter_count)
        finally:
            pass

###############################################################################
@disabled
class CpuTxTest(ApiHelper):
    def setUp(self):
        self.currTest = switch_hostif.CpuTxTest()
        self.currTest.setUp()

    def tearDown(self):
        self.currTest.tearDown()

    def runBaseTests(self):
        print()
        try:
            self.currTest.CPUTxBypass()
            self.currTest.L2CPUTxAccessPortTest()
            self.currTest.L2CPUTxTrunkPortTest()
            #self.currTest.L2CPUTxAccessLagTest()
            self.currTest.L2CPUTxTrunkLagTest()
            self.currTest.L2CPUTxFloodTest()
            self.currTest.L3CPUTxPortTest()
            self.currTest.L3CPUTxLagTest()
        finally:
            pass

    def runTest(self):
        try:
            self.runBaseTests()
            Replay.startReplay(self.currTest)
            self.runBaseTests()
        finally:
            pass

###############################################################################
@disabled
class ACLMultipleTablesTest(ApiHelper):
    def setUp(self):
        self.currTest = switch_acl.ACLMultipleTablesTest()
        self.currTest.setUp()

    def tearDown(self):
        self.currTest.tearDown()

    def runBaseTests(self):
        print()
        try:
            self.currTest.NoAclBoundTest()
            self.currTest.AclGroup1Test()
            self.currTest.AclGroup2Test()
            self.currTest.AclGroup3Test()
        finally:
            pass

    def runTest(self):
        try:
            self.runBaseTests()
            Replay.startReplay(self.currTest)
            self.runBaseTests()
        finally:
            pass

###############################################################################
@disabled
class ACLFieldComplexityTest(ApiHelper):
    def setUp(self):
        self.currTest = switch_acl.ACLFieldComplexityTest()
        self.currTest.setUp()
    def tearDown(self):
        self.currTest.tearDown()
    def runBaseTests(self):
        try:
            self.currTest.runTest()
        finally:
            pass
    def runTest(self):
        try:
            self.runBaseTests()
            Replay.startReplay(self.currTest)
            self.runBaseTests()
        finally:
            pass

###############################################################################
@disabled
class TheBigOne(ApiHelper):
    def setUp(self):
        self.currTest = switch_tests.OneTestToRuleThemAll()
        self.currTest.setUp()
    def tearDown(self):
        self.currTest.tearDown()
    def runBaseTests(self):
        try:
            self.currTest.runTest()
        finally:
            pass
    def runTest(self):
        try:
            self.runBaseTests()
            Replay.startReplay(self.currTest)
            self.runBaseTests()
        finally:
            pass

###############################################################################
@disabled
class BufferTest(ApiHelper):
    def setUp(self):
        self.BufferTest = switch_qos.BufferAttribute()
        self.BufferTest.setUp()
    def tearDown(self):
        self.BufferTest.tearDown()
    def runBaseTests(self):
        self.BufferTest.IngressBufferTest()
        self.BufferTest.EgressBufferTest()

    def runTest(self):
        try:
            self.runBaseTests()
            Replay.startReplay(self.BufferTest)
            self.runBaseTests()
        finally:
            pass

###############################################################################
@disabled
class PortPPGTest(ApiHelper):
    def setUp(self):
        self.PortPPGTest = switch_qos.PortPPGAttribute()
        self.PortPPGTest.setUp()
    def tearDown(self):
        self.PortPPGTest.tearDown()
    def runBaseTests(self):
        self.PortPPGTest.PortPPGTest()
        self.PortPPGTest.PortPPGTest2()

    def runTest(self):
        try:
            self.runBaseTests()
            Replay.startReplay(self.PortPPGTest)
            self.runBaseTests()
        finally:
            pass

###############################################################################
@disabled
class SchedulerTest(ApiHelper):
    def setUp(self):
        self.SchedulerTest = switch_qos.schedulerGroupAttribute()
        self.SchedulerTest.setUp()
    def tearDown(self):
        self.SchedulerTest.tearDown()
    def runBaseTests(self):
        self.SchedulerTest.StrictSchedulerShaperTest()
        self.SchedulerTest.DWRRSchedulerShaperTest()
        self.SchedulerTest.PortShaperTest()

    def runTest(self):
        try:
            self.runBaseTests()
            Replay.startReplay(self.SchedulerTest)
            self.runBaseTests()
        finally:
            pass

###############################################################################
@disabled
class StormControlTest(ApiHelper):
    def setUp(self):
        self.currTest = switch_qos.StormControlTest()
        self.currTest.setUp()

    def tearDown(self):
        self.currTest.tearDown()

    def runBaseTests(self):
        self.currTest.StormControlUcastTest()
        self.currTest.StormControlBcastTest()

    def runTest(self):
        if (self.currTest.client.is_feature_enable(SWITCH_FEATURE_STORM_CONTROL) == 0):
            return
        try:
            self.runBaseTests()
            Replay.startReplay(self.currTest)
            self.runBaseTests()
        finally:
            pass

###############################################################################
@disabled
class PpgQueueStatTest(ApiHelper):
    def setUp(self):
        self.PpgQueueStatTest = switch_qos.PpgQueueStatTest()
        self.PpgQueueStatTest.setUp()
    def tearDown(self):
        self.PpgQueueStatTest.tearDown()
    def runBaseTests(self, check_stats_count):
        self.PpgQueueStatTest.PortPPGStatTest(check_stats_count)
        self.PpgQueueStatTest.PortQueueStatTest()

    def runTest(self):
        if(self.PpgQueueStatTest._feature_check):
            try:
                self.runBaseTests(10)
                Replay.startReplay(self.PpgQueueStatTest)
                hitless = reboot_type()
                if (hitless):
                    self.runBaseTests(20)
                else:
                    self.runBaseTests(10)
            finally:
                self.PpgQueueStatTest.QosMapCleanup()
                pass

###############################################################################

@disabled
class MirrorBasicTest(ApiHelper):

    def runTest(self):
        print()
        try:
            self.TrafficTestConfig()
            Replay.startReplay(self)
        finally:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.port2, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_MIRROR_HANDLE, 0)
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_INGRESS_MIRROR_HANDLE, 0)
            self.cleanup()

    def TrafficTestConfig(self):
        self.device = self.get_device_handle(0)
        self.vlan10 = self.add_vlan(self.device, vlan_id=10)
        self.vlan40 = self.add_vlan(self.device, vlan_id=40)
        self.port0 = self.add_port(self.device, lane_list=u.lane_list_t([0]), speed=25000)
        self.port1 = self.add_port(self.device, lane_list=u.lane_list_t([1]), speed=25000)
        self.port2 = self.add_port(self.device, lane_list=u.lane_list_t([2]), speed=25000)
        self.port3 = self.add_port(self.device, lane_list=u.lane_list_t([3]), speed=25000)
        self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_LEARNING, False)
        vlan_mbr0 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port0)
        vlan_mbr1 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port1)
        vlan_mbr2 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port2)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.attribute_set(self.port2, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:11:11:11:11:11', destination_handle=self.port1)
        mac1 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:22:22:22:22:22', destination_handle=self.port0)

        mirror = self.add_mirror(self.device,
            type=SWITCH_MIRROR_ATTR_TYPE_LOCAL,
            direction=SWITCH_MIRROR_ATTR_DIRECTION_INGRESS,
            egress_port_handle=self.port2)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_INGRESS_MIRROR_HANDLE, mirror)

        mirror = self.add_mirror(self.device,
            type=SWITCH_MIRROR_ATTR_TYPE_ENHANCED_REMOTE,
            direction=SWITCH_MIRROR_ATTR_DIRECTION_INGRESS,
            rspan_type=SWITCH_MIRROR_ATTR_RSPAN_TYPE_VLAN_HANDLE,
            erspan_type=SWITCH_MIRROR_ATTR_ERSPAN_TYPE_ERSPAN_2,
            vlan_handle=self.vlan40,
            egress_port_handle=self.port3,
            ttl=64,
            src_mac_address='00:33:33:33:33:33',
            dest_mac_address='00:44:44:44:44:44',
            src_ip='10.10.10.1',
            dest_ip='20.20.20.1')
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_INGRESS_MIRROR_HANDLE, mirror)

###############################################################################
@disabled
class LocalMirrorTest(ApiHelper):
    def setUp(self):
        self.currTest = switch_mirror.LocalMirrorTest()
        self.currTest.setUp()
    def tearDown(self):
        self.currTest.tearDown()
    def runBaseTests(self):
        self.currTest.IngressLocalMirrorTest()
        self.currTest.EgressLocalMirrorTest()

    def runTest(self):
        try:
            self.runBaseTests()
            Replay.startReplay(self.currTest)
            self.runBaseTests()
        finally:
            pass

###############################################################################
@disabled
class MeterTest(ApiHelper):
    def setUp(self):
        self.currTest = switch_qos.QosAclTest()
        self.currTest.setUp()

    def tearDown(self):
        if (self.currTest.client.is_feature_enable(SWITCH_FEATURE_INGRESS_IP_QOS_ACL) == 0):
            return
        self.currTest.tearDown()

    def runBaseTests(self):
        print()
        self.currTest.SetColorAndMeterConfigTest()

    def runTest(self):
        if (self.currTest.client.is_feature_enable(SWITCH_FEATURE_INGRESS_IP_QOS_ACL) == 0):
            return
        try:
            self.runBaseTests()
            Replay.startReplay(self.currTest)
        finally:
            pass

###############################################################################
@disabled
class EnhancedRemote2MirrorTest(ApiHelper):
    def setUp(self):
        self.currTest = switch_mirror.EnhancedRemote2MirrorTest()
        self.currTest.setUp()
    def tearDown(self):
        self.currTest.tearDown()
    def runBaseTests(self):
        self.currTest.MirrorTest()
        self.currTest.MirrorVlanIDTest()
        self.currTest.MirrorVlanHandleTest()
        self.currTest.EgressMirrorTest()

    def runTest(self):
        try:
            self.runBaseTests()
            Replay.startReplay(self.currTest)
            self.runBaseTests()
        finally:
            pass

###############################################################################
@disabled
class EnhancedRemote3MirrorTest(ApiHelper):
    def setUp(self):
        self.currTest = switch_mirror.EnhancedRemote3MirrorTest()
        self.currTest.setUp()
    def tearDown(self):
        self.currTest.tearDown()
    def runBaseTests(self):
        self.currTest.MirrorTest()
        self.currTest.MirrorVlanIDTest()
        self.currTest.MirrorVlanHandleTest()
        self.currTest.EgressMirrorTest()

    def runTest(self):
        try:
            self.runBaseTests()
            Replay.startReplay(self.currTest)
            self.runBaseTests()
        finally:
            pass

###############################################################################

@disabled
class WredTest(ApiHelper):
    def setUp(self):
        self.WredTest = switch_qos.WredTest()
        self.WredTest.setUp()
    def tearDown(self):
        self.WredTest.tearDown()
    def runBaseTests(self):
        self.WredTest.WredIPv4Test()
        self.WredTest.WredIPv6Test()
        self.WredTest.WredDropIPv4Test()

    def runTest(self):
        if(self.WredTest._feature_check):
            try:
                self.runBaseTests()
                Replay.startReplay(self.WredTest)
                self.runBaseTests()
            finally:
                pass

###############################################################################

@disabled
class PFCWDTest(ApiHelper):
    def setUp(self):
        self.PFCWDTest = switch_pfcwd.PFCWDTest()
        self.PFCWDTest.setUp()
    def tearDown(self):
        self.PFCWDTest.tearDown()
    def runBaseTests(self):
        self.PFCWDTest.TableBindUnbindTest()
        self.PFCWDTest.TableMultiplePortsBindUnbindTest()
        self.PFCWDTest.GroupBindUnbindTest()
        self.PFCWDTest.GroupMemberAddRemoveTest()
        self.PFCWDTest.EntryCreateRemoveTest()
        self.PFCWDTest.TrafficDropTest()
        self.PFCWDTest.TrafficDropTest(True)

    def runTest(self):
        try:
            self.runBaseTests()
            Replay.startReplay(self.PFCWDTest)
            self.runBaseTests()
        finally:
            pass

###############################################################################
@disabled
class DtelBasicTest(ApiHelper):

    def runTest(self):
        print()
        try:
            self.TrafficTestConfig()
            Replay.startReplay(self)
        finally:
            self.attribute_set(self.port3, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.port4, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.cleanup()

    def TrafficTestConfig(self):
        switch_dtel.configure_dtel(self, d=True, q=True, f=True)

###############################################################################
@disabled
class MoDNoDropTest(ApiHelper):
    def setUp(self):
        self.currTest = switch_dtel.MoDNoDropTest()
        self.currTest.setUp()
    def tearDown(self):
        self.currTest.tearDown()
    def runBaseTests(self):
        self.currTest.runTest()

    def runTest(self):
        try:
            self.runBaseTests()
            Replay.startReplay(self.currTest)
            self.runBaseTests()
        finally:
            pass

###############################################################################
@disabled
class MoDHostifReasonCodeDropTest(ApiHelper):
    def setUp(self):
        self.currTest = switch_dtel.MoDHostifReasonCodeDropTest()
        self.currTest.setUp()
    def tearDown(self):
        self.currTest.tearDown()
    def runBaseTests(self):
        self.currTest.runTest()

    def runTest(self):
        try:
            self.runBaseTests()
            Replay.startReplay(self.currTest)
            self.runBaseTests()
        finally:
            pass

###############################################################################
@disabled
class MoDIngressACLDropTest(ApiHelper):
    def setUp(self):
        self.currTest = switch_dtel.MoDIngressACLDropTest()
        self.currTest.setUp()
    def tearDown(self):
        self.currTest.tearDown()
    def runBaseTests(self):
        self.currTest.runTest()

    def runTest(self):
        try:
            self.runBaseTests()
            Replay.startReplay(self.currTest)
            self.runBaseTests()
        finally:
            pass

###############################################################################
@disabled
class MoDEgressACLDropTest(ApiHelper):
    def setUp(self):
        self.currTest = switch_dtel.MoDEgressACLDropTest()
        self.currTest.setUp()
    def tearDown(self):
        self.currTest.tearDown()
    def runBaseTests(self):
        self.currTest.runTest()

    def runTest(self):
        try:
            self.runBaseTests()
            Replay.startReplay(self.currTest)
            self.runBaseTests()
        finally:
            pass

###############################################################################
@disabled
class DtelWatchListUnitTest(ApiHelper):
    def setUp(self):
        self.currTest = switch_dtel.DtelWatchListUnitTest()
        self.currTest.setUp()
    def tearDown(self):
        self.currTest.tearDown()
    def runBaseTests(self):
        self.currTest.runTest()

    def runTest(self):
        try:
            self.runBaseTests()
            Replay.startReplay(self.currTest)
            self.runBaseTests()
        finally:
            pass

###############################################################################
@disabled
class MoDL2MalformedPacketsTest(ApiHelper):
    def setUp(self):
        self.currTest = switch_dtel.MoDL2MalformedPacketsTest()
        self.currTest.setUp()
    def tearDown(self):
        self.currTest.tearDown()
    def runBaseTests(self):
        self.currTest.runTest()

    def runTest(self):
        try:
            self.runBaseTests()
            Replay.startReplay(self.currTest)
            self.runBaseTests()
        finally:
            pass

###############################################################################

@disabled
class TcIcosQueueMarkingStatsTest(ApiHelper):
    def setUp(self):
        self.currTest =  switch_qos.TcIcosQueueMarkingStatsTest()
        self.currTest.setUp()
        self.currTest.DscpIcosQueueTest2Configure()
    def tearDown(self):
        self.currTest.DscpIcosQueueTest2Cleanup()
        self.currTest.tearDown()
    def runBaseTests(self):
        self.currTest.DscpIcosQueueTest2TrafficTest()

    def runTest(self):
        try:
            self.runBaseTests()
            Replay.startReplay(self.currTest)
            self.runBaseTests()
        finally:
            pass
@disabled
class EtrapBasicTest(ApiHelper):
    def TrafficTestConfig(self):
        print()
        self.configure()

        self.tc_val = 20
        self.qid = 5

        # tc to qid qos_map
        self.qos_map = self.add_qos_map(self.device, tc=self.tc_val, qid=self.qid)
        tc_queue_maps = []
        tc_queue_maps.append(switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=self.qos_map))
        self.tc_qid_map_ingress = self.add_qos_map_ingress(self.device,
            type=SWITCH_QOS_MAP_INGRESS_ATTR_TYPE_TC_TO_QUEUE, qos_map_list=tc_queue_maps)

        self.queue_handle = 0
        self.default_queue_handle = 0

        queue_list = self.attribute_get(self.port0, SWITCH_PORT_ATTR_QUEUE_HANDLES)
        for queue in queue_list:
            queue_id = self.attribute_get(queue.oid, SWITCH_QUEUE_ATTR_QUEUE_ID)
            if self.qid == queue_id:
                self.queue_handle = queue.oid
            if 0 == queue_id:
                self.default_queue_handle = queue.oid

        self.meter1 = self.add_meter(self.device,
            mode=SWITCH_METER_ATTR_MODE_TWO_RATE_THREE_COLOR,
            pir=80,cir=80,
            color_source=SWITCH_METER_ATTR_COLOR_SOURCE_BLIND)

        # Etrap ACL tables and entries
        acl_table_bp_switch = switcht_list_val_t(type=switcht_value_type.ENUM, enumdata=SWITCH_ACL_TABLE_ATTR_BIND_POINT_TYPE_SWITCH)
        self.etrap_v4_tbl = self.add_acl_table(self.device,
            type=SWITCH_ACL_TABLE_ATTR_TYPE_IPV4_ETRAP,
            bind_point_type=[acl_table_bp_switch],
            direction=SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS)

        # Create etrap ACL entry with source subnet only
        self.acl_entry1 = self.add_acl_entry(self.device,
            src_ip='10.10.10.0',
            src_ip_mask='255.255.255.224',
            action_meter_handle=self.meter1,
            tc=20,
            table_handle=self.etrap_v4_tbl)

    def runTest(self):
        print()
        try:
            self.TrafficTestConfig()
            Replay.startReplay(self)
        finally:
            self.cleanup()

###############################################################################

@disabled
class EtrapConfigTest(ApiHelper):
    def setUp(self):
        self.EtrapConfigTest = switch_etrap.EtrapConfigTest()
        self.EtrapConfigTest.setUp()
    def tearDown(self):
        self.EtrapConfigTest.tearDown()
    def runBaseTests(self):
        self.EtrapConfigTest.EtrapAclEntryCreateRemoveTest()
        self.EtrapConfigTest.EtrapAclEntryCreateRemoveTest(host_ip=True)
        self.EtrapConfigTest.EtrapAclEntryCreateRemoveTest(v6=True)
        self.EtrapConfigTest.EtrapAclEntryCreateRemoveTest(v6=True, host_ip=True)
        self.EtrapConfigTest.EtrapAclEntryAttrUpdateTest()
        self.EtrapConfigTest.EtrapAclEntryCreateNegativeTest()

    def runTest(self):
        try:
            self.runBaseTests()
            Replay.startReplay(self.EtrapConfigTest)
            self.runBaseTests()
        finally:
            pass

###############################################################################

@disabled
class HashConfigTest(ApiHelper):
    def setUp(self):
        self.HashConfigTest = switch_hash.HashConfigTest()
        self.HashConfigTest.setUp()

    def tearDown(self):
        self.HashConfigTest.tearDown()

    def runBaseTests(self):
        self.HashConfigTest.ipv4BasicHashTest()
        self.HashConfigTest.ipv4SrcIPHashTest()
        self.HashConfigTest.ipv6BasicHashTest()
        self.HashConfigTest.ipv6DstIPHashTest()

    def runTest(self):
        try:
            self.runBaseTests()
            Replay.startReplay(self.HashConfigTest)
            self.runBaseTests()
        finally:
            pass

###############################################################################

@disabled
class TunnelNhopResolutionTest(ApiHelper):
    def setUp(self):
        self.TunnelNhopResolutionTest = switch_tunnel.TunnelNhopResolutionTest()
        self.TunnelNhopResolutionTest.setUp()

    def tearDown(self):
        self.TunnelNhopResolutionTest.tearDown()

    def runBaseTests(self):
        self.TunnelNhopResolutionTest.MultiTunnelTests()
        self.TunnelNhopResolutionTest.TunnelL3IntfTests()
        self.TunnelNhopResolutionTest.TunnelL3SubPortTests()
        self.TunnelNhopResolutionTest.TunnelSVITests()
        self.TunnelNhopResolutionTest.TunnelECMPTests()
        self.TunnelNhopResolutionTest.TunnelUpdateTests()

    def runTest(self):
        try:
            self.runBaseTests()
            Replay.startReplay(self.TunnelNhopResolutionTest)
            self.runBaseTests()
        finally:
            pass

###############################################################################

@disabled
class L3MultiVrfVniMapperTest(ApiHelper):
    def setUp(self):
        self.L3MultiVrfVniMapperTest = switch_tunnel.L3MultiVrfVniMapperTest()
        self.L3MultiVrfVniMapperTest.setUp()

    def tearDown(self):
        self.L3MultiVrfVniMapperTest.tearDown()

    def runBaseTests(self):
        self.L3MultiVrfVniMapperTest.runTest()

    def runTest(self):
        try:
            self.runBaseTests()
            Replay.startReplay(self.L3MultiVrfVniMapperTest)
            self.runBaseTests()
        finally:
            pass

###############################################################################

@disabled
class L3IPv4TunnelTest(ApiHelper):
    def setUp(self):
        self.L3IPv4TunnelTest = switch_tunnel.L3IPv4TunnelTest()
        self.L3IPv4TunnelTest.setUp()

    def tearDown(self):
        self.L3IPv4TunnelTest.tearDown()

    def runBaseTests(self):
        pass
        self.L3IPv4TunnelTest.L3IntfIPv4Test()
        self.L3IPv4TunnelTest.L3IntfIPv6Test()
        self.L3IPv4TunnelTest.L3SubPortIPv4Test()
        self.L3IPv4TunnelTest.L3SubPortIPv6Test()
        self.L3IPv4TunnelTest.SVITunnelIPv4Test()
        self.L3IPv4TunnelTest.SVITunnelIPv6Test()
        self.L3IPv4TunnelTest.ECMPTunnelIPv4Test()
        self.L3IPv4TunnelTest.LAGTunnelIPv4Test()
        self.L3IPv4TunnelTest.LAGTunnelIPv6Test()
        self.L3IPv4TunnelTest.EcmpLagTunnelIPv4Test()
        self.L3IPv4TunnelTest.EcmpLagTunnelIPv6Test()
        self.L3IPv4TunnelTest.SVIIngressTunnelTests()

    def runTest(self):
        if (self.L3IPv4TunnelTest.client.is_feature_enable(SWITCH_FEATURE_IPV4_TUNNEL) == 0):
            print("IPv4 tunnel feature not enabled, skipping")
            return
        try:
            self.runBaseTests()
            Replay.startReplay(self.L3IPv4TunnelTest)
            self.runBaseTests()
        finally:
            pass

###############################################################################

@disabled
class L3IPv6TunnelTest(ApiHelper):
    def setUp(self):
        self.L3IPv6TunnelTest = switch_tunnel.L3IPv6TunnelTest()
        self.L3IPv6TunnelTest.setUp()

    def tearDown(self):
        self.L3IPv6TunnelTest.tearDown()

    def runBaseTests(self):
        self.L3IPv6TunnelTest.L3IPv6TunnelBaseTests()
        self.L3IPv6TunnelTest.EcmpLagTunnelLagUpdateTests()
        self.L3IPv6TunnelTest.SVILagTunnelBaseTests()
        self.L3IPv6TunnelTest.SVIEcmpTunnelTests()

    def runTest(self):
        if (self.L3IPv6TunnelTest.client.is_feature_enable(SWITCH_FEATURE_IPV6_TUNNEL) == 0):
            print("IPv6 tunnel feature not enabled, skipping")
            return
        try:
            self.runBaseTests()
            Replay.startReplay(self.L3IPv6TunnelTest)
            self.runBaseTests()
        finally:
            pass

###############################################################################

@disabled
class L3TunnelIPinIPv4Test(ApiHelper):
    def setUp(self):
        self.L3TunnelIPinIPv4Test = switch_tunnel.L3TunnelIPinIPv4Test()
        self.L3TunnelIPinIPv4Test.setUp()

    def tearDown(self):
        self.L3TunnelIPinIPv4Test.tearDown()

    def runBaseTests(self):
        self.L3TunnelIPinIPv4Test.IPv4inIPv4Test()
        self.L3TunnelIPinIPv4Test.IPv6inIPv4Test()

    def runTest(self):
        if (self.L3TunnelIPinIPv4Test.client.is_feature_enable(SWITCH_FEATURE_IPV4_TUNNEL) == 0):
            print("IPv4 tunnel feature not enabled, skipping")
            return
        try:
            self.runBaseTests()
            Replay.startReplay(self.L3TunnelIPinIPv4Test)
            self.runBaseTests()
        finally:
            pass

###############################################################################

@disabled
class L3TunnelIPinIPv6Test(ApiHelper):
    def setUp(self):
        self.L3TunnelIPinIPv6Test = switch_tunnel.L3TunnelIPinIPv6Test()
        self.L3TunnelIPinIPv6Test.setUp()

    def tearDown(self):
        self.L3TunnelIPinIPv6Test.tearDown()

    def runBaseTests(self):
        self.L3TunnelIPinIPv6Test.IPv4inIPv6Test()
        self.L3TunnelIPinIPv6Test.IPv6inIPv6Test()

    def runTest(self):
        if (self.L3TunnelIPinIPv6Test.client.is_feature_enable(SWITCH_FEATURE_IPV6_TUNNEL) == 0):
            print("IPv6 tunnel feature not enabled, skipping")
            return
        try:
            self.runBaseTests()
            Replay.startReplay(self.L3TunnelIPinIPv6Test)
            self.runBaseTests()
        finally:
            pass

###############################################################################

@disabled
class TunnelMalformedPacketsTest(ApiHelper):
    def setUp(self):
        self.TunnelMalformedPacketsTest = switch_tunnel.TunnelMalformedPacketsTest()
        self.TunnelMalformedPacketsTest.setUp()

    def tearDown(self):
        self.TunnelMalformedPacketsTest.tearDown()

    def runBaseTests(self):
        self.TunnelMalformedPacketsTest.BasicTest()
        self.TunnelMalformedPacketsTest.L2IPv4Test()
        self.TunnelMalformedPacketsTest.L3V6DecapTest()
        self.TunnelMalformedPacketsTest.L3IPv4EncapTest()
        self.TunnelMalformedPacketsTest.L3V6EncapTest()
        self.TunnelMalformedPacketsTest.L3V6DecapTest()

    def runTest(self):
        if (self.TunnelMalformedPacketsTest.client.is_feature_enable(SWITCH_FEATURE_IPV4_TUNNEL) == 0):
            print("IPv4 tunnel feature not enabled, skipping")
            return
        try:
            self.runBaseTests()
            Replay.startReplay(self.TunnelMalformedPacketsTest)
            self.runBaseTests()
        finally:
            pass

###############################################################################

@disabled
class TunnelMultiNhopResolutionTest(ApiHelper):
    def setUp(self):
        self.configure()
        if (self.client.is_feature_enable(SWITCH_FEATURE_IPV4_TUNNEL) == 0):
            print("IPv4 tunnel feature not enabled, skipping")
            return
        if (self.client.is_feature_enable(SWITCH_FEATURE_VXLAN) == 0):
            print("VXLAN feature not enabled, skipping")
            return
        print()

        self.vni = 2000
        self.tunnel_lb_ip = '10.10.10.10'
        self.tunnel_type = SWITCH_TUNNEL_ATTR_TYPE_VXLAN
        self.encap_ttl_mode = SWITCH_TUNNEL_ATTR_ENCAP_TTL_MODE_PIPE_MODEL
        self.decap_ttl_mode = SWITCH_TUNNEL_ATTR_DECAP_TTL_MODE_PIPE_MODEL
        self.customer_ip = '100.100.50.1'
        self.default_rmac = "00:BA:7E:F0:00:00"  # from bf_switch_device_add

        # Create underlay and overlay vrfs
        self.uvrf = self.add_vrf(self.device, id=100, src_mac=self.rmac)
        self.ovrf = self.add_vrf(self.device, id=200, src_mac=self.rmac)

        # create Encap/Decap mappers
        self.encap_tunnel_map = self.add_tunnel_mapper(self.device,
                type=SWITCH_TUNNEL_MAPPER_ATTR_TYPE_VRF_HANDLE_TO_VNI)
        self.decap_tunnel_map = self.add_tunnel_mapper(self.device,
                type=SWITCH_TUNNEL_MAPPER_ATTR_TYPE_VNI_TO_VRF_HANDLE)

        # create Encap/Decap mapper entries for ovrf
        self.ecnap_tunnel_map_entry = self.add_tunnel_mapper_entry(self.device,
                type=SWITCH_TUNNEL_MAPPER_ENTRY_ATTR_TYPE_VRF_HANDLE_TO_VNI,
                tunnel_mapper_handle=self.encap_tunnel_map,
                network_handle=self.ovrf, tunnel_vni=self.vni)
        self.decap_tunnel_map_entry = self.add_tunnel_mapper_entry(self.device,
                type=SWITCH_TUNNEL_MAPPER_ENTRY_ATTR_TYPE_VNI_TO_VRF_HANDLE,
                tunnel_mapper_handle=self.decap_tunnel_map,
                network_handle=self.ovrf, tunnel_vni=self.vni)

        # create underlay loopback rif for tunnel
        self.urif_lb = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_LOOPBACK, vrf_handle=self.uvrf,
                src_mac=self.rmac)

        ingress_mapper_list = [switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=self.encap_tunnel_map)]
        egress_mapper_list = [switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=self.decap_tunnel_map)]

        #Create Tunnel
        self.tunnel = self.add_tunnel(self.device, type=self.tunnel_type, src_ip=self.tunnel_lb_ip,
                ingress_mapper_handles=ingress_mapper_list, egress_mapper_handles=egress_mapper_list,
                encap_ttl_mode=self.encap_ttl_mode, decap_ttl_mode=self.decap_ttl_mode,
                underlay_rif_handle=self.urif_lb)

        #Add Customer routes with Tunnel Nexthops
        self.orif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT,
                port_handle=self.port0, vrf_handle=self.ovrf, src_mac=self.rmac)
        self.customer_config =[{'customer_route':{'ip_prefix':'100.100.1.1'},
                               'tunnel_nexthop':{'dest_ip':'10.10.10.1','mac_address':'00:00:10:10:10:1'}},
                               #{'customer_route':{'ip_prefix':'100.100.2.1'}
                               #'tunnel_nexthop':{'dest_ip':'10.10.10.2','mac_address':'00:00:10:10:10:2'}}
                               ]
        self.tunnel_nexthops = []
        self.customer_routes = []
        for config in self.customer_config:
            tunnel_nexthop=config['tunnel_nexthop']
            nhop=self.add_nexthop(self.device, type=SWITCH_NEXTHOP_ATTR_TYPE_TUNNEL,
                    rw_type=SWITCH_NEXTHOP_ATTR_RW_TYPE_L3_VNI, handle=self.tunnel, tunnel_vni=2000, **tunnel_nexthop)
            route=config['customer_route']
            self.customer_routes.append(self.add_route(self.device, vrf_handle=self.ovrf, nexthop_handle=
                nhop, **route))
            self.tunnel_nexthops.append(nhop)

        #Add Underlay config
        self.underlay_config = [
                {'urif':{'port_handle':self.port1},
                'uneighbor':{'mac_address':'00:20:20:20:20:1','dest_ip':'20.20.20.1'},
                'unhop':{'dest_ip':'20.20.20.1'},
                'tunnel_route':{'ip_prefix':'10.0.0.0/8'}},
                {'urif':{'port_handle':self.port2},
                'uneighbor':{'mac_address':'00:30:30:30:30:1','dest_ip':'30.30.30.1'},
                'unhop':{'dest_ip':'30.30.30.1'},
                'tunnel_route':{'ip_prefix':'10.10.0.0/16'}},
                {'urif':{'port_handle':self.port3},
                'uneighbor':{'mac_address':'00:40:40:40:40:1','dest_ip':'40.40.40.1'},
                'unhop':{'dest_ip':'40.40.40.1'},
                'tunnel_route':{'ip_prefix':'10.10.10.24/24'}},
                {'urif':{'port_handle':self.port4},
                'uneighbor':{'mac_address':'00:10:10:10:10:1','dest_ip':'10.10.10.1'},
                'unhop':{'dest_ip':'10.10.10.1'},
                'tunnel_route':{'ip_prefix':'10.10.10.1/32'}},
                #{'urif':{'port_handle':self.port5},
                #'uneighbor':{'mac_address':'00:10:10:10:10:2','dest_ip':'10.10.10.2'}
                #'unhop':{'dest_ip':'10.10.10.2'},
                #'tunnel_route':{'ip_prefix'='10.10.10.2/32'}
                ]

        self.configUnderlay(self.underlay_config)

    def configUnderlay(self, config):
        for uconfig in self.underlay_config:
            urif = uconfig['urif']
            uneighbor = uconfig['uneighbor']
            unhop = uconfig['unhop']
            tunnel_route = uconfig['tunnel_route']

            rif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, vrf_handle=self.uvrf,
                src_mac=self.rmac, **urif)
            nbr = self.add_neighbor(self.device, handle=rif, **uneighbor)
            nhop = self.add_nexthop(self.device, handle=rif, **unhop)
            route = self.add_route(self.device, vrf_handle=self.uvrf, nexthop_handle=nhop, **tunnel_route)

            uconfig['urif_handle'] = rif

    def runTest(self):
        try:
            if (self.client.is_feature_enable(SWITCH_FEATURE_VXLAN) == 0):
                print("VXLAN feature not enabled, skipping")
                return
            self.TrafficTest()
            Replay.startReplay(self)
            self.TrafficTest()
        finally:
            pass

    def TrafficTest(self):
        customer_pkts = []
        inner_pkts = []
        for config in self.customer_config:
            ip_dst=config['customer_route']['ip_prefix']
            eth_dst=config['tunnel_nexthop']['mac_address']
            pkt= simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst=ip_dst,
                ip_src=self.customer_ip,
                ip_id=108,
                ip_ttl=64)
            inner_pkt=simple_tcp_packet(
                eth_dst=eth_dst,
               eth_src=self.default_rmac,
               ip_dst=ip_dst,
               ip_src=self.customer_ip,
               ip_id=108,
               ip_ttl=63)
            for uconfig in reversed(self.underlay_config):
                underlay_neighbor_mac = uconfig['uneighbor']['mac_address']
                tunnel_ep_ip = config['tunnel_nexthop']['dest_ip']
                out_port = uconfig['urif']['port_handle']
                dev_port = self.attribute_get(out_port, SWITCH_PORT_ATTR_DEV_PORT)
                vxlan_pkt = mask.Mask(simple_vxlan_packet(
                    eth_src='00:77:66:55:44:33',
                    eth_dst=underlay_neighbor_mac,
                    ip_id=0,
                    ip_src=self.tunnel_lb_ip,
                    ip_dst=tunnel_ep_ip,
                    ip_ttl=64,
                    ip_flags=0x2,
                    with_udp_chksum=False,
                    vxlan_vni=self.vni,
                    inner_frame=inner_pkt))
                mask_set_do_not_care_packet(vxlan_pkt, UDP, "sport")
                print("Sending packet from port 0 to VxLan port {}".format(devport_to_swport(self, dev_port)))
                try:
                    send_packet(self, self.devports[0], pkt)
                    verify_packets(self, vxlan_pkt, [dev_port])
                finally:
                    self.clean_to_object(uconfig['urif_handle'])
                    uconfig['urif_handle'] = 0

        self.configUnderlay(self.underlay_config)

    def tearDown(self):
        self.cleanup()

###############################################################################

@disabled
class IcosPpgQosMapIngressUpdateTest(ApiHelper):
    def setUp(self):
        self.IcosPpgQosMapIngressUpdateTest = switch_qos.IcosPpgQosMapIngressUpdateTest()
        self.IcosPpgQosMapIngressUpdateTest.setUp()

    def tearDown(self):
        self.IcosPpgQosMapIngressUpdateTest.tearDown()

    def runBaseTests(self):
        self.IcosPpgQosMapIngressUpdateTest.QosMapAttrSetTest()
        self.IcosPpgQosMapIngressUpdateTest.QosMapAttrUpdateTest1()
        self.IcosPpgQosMapIngressUpdateTest.QosMapAttrUpdateTest2()

    def runTest(self):
        try:
            self.runBaseTests()
            Replay.startReplay(self.IcosPpgQosMapIngressUpdateTest)
            #Set expected ppg stats to 0, until counter restore feature is merged
            for index in range(len(self.IcosPpgQosMapIngressUpdateTest.expected_ppg_stats)):
                self.IcosPpgQosMapIngressUpdateTest.expected_ppg_stats[index]=0
            self.runBaseTests()
        finally:
            pass

@disabled
class DscpTcQosMapIngressUpdateTest(ApiHelper):
    def setUp(self):
        self.DscpTcQosMapIngressUpdateTest = switch_qos.DscpTcQosMapIngressUpdateTest()
        self.DscpTcQosMapIngressUpdateTest.setUp()

    def tearDown(self):
        self.DscpTcQosMapIngressUpdateTest.tearDown()

    def runBaseTests(self):
        self.DscpTcQosMapIngressUpdateTest.QosMapAttrSetTest()
        self.DscpTcQosMapIngressUpdateTest.QosMapAttrUpdateTest1()
        self.DscpTcQosMapIngressUpdateTest.QosMapAttrUpdateTest2()

    def runTest(self):
        try:
            self.runBaseTests()
            Replay.startReplay(self.DscpTcQosMapIngressUpdateTest)
            #Set expected ppg stats to 0, until counter restore feature is merged
            for index in range(len(self.DscpTcQosMapIngressUpdateTest.expected_ppg_stats)):
                self.DscpTcQosMapIngressUpdateTest.expected_ppg_stats[index]=0
            self.runBaseTests()
        finally:
            pass

@disabled
class PcpTcQosMapIngressUpdateTest(ApiHelper):
    def setUp(self):
        self.PcpTcQosMapIngressUpdateTest = switch_qos.PcpTcQosMapIngressUpdateTest()
        self.PcpTcQosMapIngressUpdateTest.setUp()

    def tearDown(self):
        self.PcpTcQosMapIngressUpdateTest.tearDown()

    def runBaseTests(self):
        self.PcpTcQosMapIngressUpdateTest.QosMapAttrSetTest()
        self.PcpTcQosMapIngressUpdateTest.QosMapAttrUpdateTest1()
        self.PcpTcQosMapIngressUpdateTest.QosMapAttrUpdateTest2()

    def runTest(self):
        try:
            self.runBaseTests()
            Replay.startReplay(self.PcpTcQosMapIngressUpdateTest)
            #Set expected ppg stats to 0, until counter restore feature is merged
            for index in range(len(self.PcpTcQosMapIngressUpdateTest.expected_ppg_stats)):
                self.PcpTcQosMapIngressUpdateTest.expected_ppg_stats[index]=0
            self.runBaseTests()
        finally:
            pass

@disabled
class PortIcosPpgQosMapUpdateTest(ApiHelper):
    def setUp(self):
        self.PortIcosPpgQosMapUpdateTest = switch_qos.PortIcosPpgQosMapUpdateTest()
        self.PortIcosPpgQosMapUpdateTest.setUp()

    def tearDown(self):
        self.PortIcosPpgQosMapUpdateTest.tearDown()

    def runBaseTests(self):
        self.PortIcosPpgQosMapUpdateTest.portQosMapAttrSetTest()
        self.PortIcosPpgQosMapUpdateTest.portQosMapAttrUpdateTest1()
        self.PortIcosPpgQosMapUpdateTest.portQosMapAttrUpdateTest2()

    def runTest(self):
        try:
            self.runBaseTests()
            Replay.startReplay(self.PortIcosPpgQosMapUpdateTest)
            #Set expected ppg stats to 0, until counter restore feature is merged
            for index in range(len(self.PortIcosPpgQosMapUpdateTest.expected_ppg_stats)):
                self.PortIcosPpgQosMapUpdateTest.expected_ppg_stats[index]=0
            self.runBaseTests()
        finally:
            pass

@disabled
class PortDscpTcQosMapUpdateTest(ApiHelper):
    def setUp(self):
        self.PortDscpTcQosMapUpdateTest = switch_qos.PortDscpTcQosMapUpdateTest()
        self.PortDscpTcQosMapUpdateTest.setUp()

    def tearDown(self):
        self.PortDscpTcQosMapUpdateTest.tearDown()

    def runBaseTests(self):
        self.PortDscpTcQosMapUpdateTest.portQosMapAttrSetTest()
        self.PortDscpTcQosMapUpdateTest.portQosMapAttrUpdateTest1()
        self.PortDscpTcQosMapUpdateTest.portQosMapAttrUpdateTest2()

    def runTest(self):
        try:
            self.runBaseTests()
            Replay.startReplay(self.PortDscpTcQosMapUpdateTest)
            #Set expected ppg stats to 0, until counter restore feature is merged
            for index in range(len(self.PortDscpTcQosMapUpdateTest.expected_ppg_stats)):
                self.PortDscpTcQosMapUpdateTest.expected_ppg_stats[index]=0
            self.runBaseTests()
        finally:
            pass

@disabled
class PortPcpTcQosMapUpdateTest(ApiHelper):
    def setUp(self):
        self.PortPcpTcQosMapUpdateTest = switch_qos.PortPcpTcQosMapUpdateTest()
        self.PortPcpTcQosMapUpdateTest.setUp()

    def tearDown(self):
        self.PortPcpTcQosMapUpdateTest.tearDown()

    def runBaseTests(self):
        self.PortPcpTcQosMapUpdateTest.portQosMapAttrSetTest()
        self.PortPcpTcQosMapUpdateTest.portQosMapAttrUpdateTest1()
        self.PortPcpTcQosMapUpdateTest.portQosMapAttrUpdateTest2()

    def runTest(self):
        try:
            self.runBaseTests()
            Replay.startReplay(self.PortPcpTcQosMapUpdateTest)
            #Set expected ppg stats to 0, until counter restore feature is merged
            for index in range(len(self.PortPcpTcQosMapUpdateTest.expected_ppg_stats)):
                self.PortPcpTcQosMapUpdateTest.expected_ppg_stats[index]=0
            self.runBaseTests()
        finally:
            pass

@disabled
class MplsCpuTrapTest(ApiHelper):
    def setUp(self):
        self.currentTest = switch_mpls.MplsCpuTrapTest()
        self.currentTest.setUp()
    def tearDown(self):
        self.currentTest.tearDown()
    def runBaseTests(self):
        self.currentTest.MplsTTLErrorDrop()
        self.currentTest.MplsRouterAlertToCpu()
        self.currentTest.MplsImplicitNullLabelTrapDrop()
    def runTest(self):
        if (self.currentTest.client.is_feature_enable(SWITCH_FEATURE_MPLS) == 0):
            print("MPLS feature not enabled, skipping")
            return
        try:
            self.runBaseTests()
            Replay.startReplay(self.currentTest)
            self.runBaseTests()
        finally:
            pass

@disabled
class MplsIpv4Test(ApiHelper):
    def setUp(self):
        self.currentTest = switch_mpls.MplsIpv4Test()
        self.currentTest.setUp()
    def tearDown(self):
        self.currentTest.tearDown()
    def runBaseTests(self):
        self.currentTest.MplsIngressLER()
        self.currentTest.MplsEgressLERTerm()
        self.currentTest.MplsEgressLERTermUpdateMplsRifVrf()
        self.currentTest.MplsEgressLERNullTerm()
        self.currentTest.MplsEgressPhp()
        self.currentTest.MplsEgressPhpSwapNull()
        self.currentTest.MplsTransitSwap()
    def runTest(self):
        if (self.currentTest.client.is_feature_enable(SWITCH_FEATURE_MPLS) == 0):
            print("MPLS feature not enabled, skipping")
            return
        try:
            self.runBaseTests()
            Replay.startReplay(self.currentTest)
            self.runBaseTests()
        finally:
            pass

@disabled
class MplsIpv6Test(ApiHelper):
    def setUp(self):
        self.currentTest = switch_mpls.MplsIpv6Test()
        self.currentTest.setUp()
    def tearDown(self):
        self.currentTest.tearDown()
    def runBaseTests(self):
        self.currentTest.MplsIngressLER()
        self.currentTest.MplsEgressLERTerm()
        self.currentTest.MplsEgressLERNullTerm()
        self.currentTest.MplsEgressPhp()
        self.currentTest.MplsEgressPhpSwapNull()
        self.currentTest.MplsTransitSwap()
    def runTest(self):
        if (self.currentTest.client.is_feature_enable(SWITCH_FEATURE_MPLS) == 0):
            print("MPLS feature not enabled, skipping")
            return
        try:
            self.runBaseTests()
            Replay.startReplay(self.currentTest)
            self.runBaseTests()
        finally:
            pass

@disabled
class NatTest(ApiHelper):
    def setUp(self):
        self.currentTest = switch_nat.SimpleNatTest()
        self.currentTest.setUp()
    def tearDown(self):
        self.currentTest.tearDown()
    def runBaseTests(self):
        self.currentTest.validateSNAPT()
        self.currentTest.validateDNAPT()
        self.currentTest.validateSNAT()
        self.currentTest.validateDNAT()
        self.currentTest.validateDoNotNatAcl()
    def runTest(self):
        if (self.currentTest.client.is_feature_enable(SWITCH_FEATURE_NAT) == 0):
            print("NAT feature not enabled, skipping")
            return
        self.runBaseTests()
        Replay.startReplay(self.currentTest)
        self.runBaseTests()
