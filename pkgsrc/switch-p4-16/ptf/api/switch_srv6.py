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

###############################################################################

@group('srv6')
class SRv6IPv4inIPv6Test(ApiHelper):
    def setUp(self):
        self.configure()
        if self.client.is_feature_enable(SWITCH_FEATURE_SRV6) == 0:
            print("SRv6 not enabled, skipping")
            return
        print()

        # underlay config
        # create a srv6 nexthop
        # create a policy which points to a srv6 nexthop

        # underlay config
        self.uvrf = self.add_vrf(self.device, id=100, src_mac=self.rmac)
        self.my_lb_ip = '2001:db8:0:a1::'
        self.remote_lb_ip = '2001:db8:0:a4::'
        self.underlay_neighbor_mac = '00:11:11:11:11:11'
        self.overlay_neighbor_mac = '00:22:22:22:22:22'
        self.underlay_xconnect_nbor_mac = '00:33:33:33:33:33'
        self.default_rmac = "00:BA:7E:F0:00:00"  # from bf_switch_device_add
        self.urif_lb = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_LOOPBACK,
                                    vrf_handle=self.uvrf, src_mac=self.rmac)

        # overlay configuration
        self.custA_ip4 = '100.100.1.1'
        self.custA_ip6 = '2001:db8:100:100::1:1'
        self.custB_ip4 = '100.100.0.1'
        self.custB_ip6 = '2001:db8:100:100::0:1'
        self.ovrf = self.add_vrf(self.device, id=300, src_mac=self.rmac)
        self.tunnel_type = SWITCH_TUNNEL_ATTR_TYPE_SRV6
        self.encap_ttl_mode = SWITCH_TUNNEL_ATTR_ENCAP_TTL_MODE_PIPE_MODEL
        self.decap_ttl_mode = SWITCH_TUNNEL_ATTR_DECAP_TTL_MODE_PIPE_MODEL

        # Create segments lists
        self.node0_prefix_sid = '2001:db8:0:a1::'
        self.node1_prefix_sid = '2001:db8:0:a2::'
        self.node2_prefix_sid = '2001:db8:0:a3::'
        self.node3_prefix_sid = '2001:db8:0:a4::'
        self.node4_prefix_sid = '2001:db8:0:a5::'
        self.node5_prefix_sid = '2001:db8:0:a6::'
        self.node6_prefix_sid = '2001:db8:0:a7::'
        self.node0_dt4_sid = '2001:db8:0:a1:100:0:0:0'
        self.node0_dx4_sid = '2001:db8:0:a1:200:0:0:0'
        self.node0_dx6_sid = '2001:db8:0:a1:300:0:0:0'

        # H.Encaps.Red sidlists
        self.segment_list1 = [self.node3_prefix_sid]
        self.segment_list2 = [self.node2_prefix_sid, self.node3_prefix_sid]
        self.segment_list3 = [self.node1_prefix_sid, self.node2_prefix_sid, self.node3_prefix_sid]

        # H.Insert.Red sidlists
        self.segment_list4 = [self.node5_prefix_sid]
        self.segment_list5 = [self.node4_prefix_sid, self.node5_prefix_sid]

        self.locator_list = [self.node1_prefix_sid + '/64', self.node2_prefix_sid + '/64', self.node3_prefix_sid + '/64', self.node4_prefix_sid + '/64', self.node5_prefix_sid + '/64']

        self.api_segment_list1 = []
        self.api_segment_list2 = []
        self.api_segment_list3 = []
        self.api_segment_list4 = []
        self.api_segment_list5 = []

        for ip in self.segment_list1:
            self.api_segment_list1.append(switcht_list_val_t(type=switcht_value_type.IP_ADDRESS, ip_addr=ip))
        for ip in self.segment_list2:
            self.api_segment_list2.append(switcht_list_val_t(type=switcht_value_type.IP_ADDRESS, ip_addr=ip))
        for ip in self.segment_list3:
            self.api_segment_list3.append(switcht_list_val_t(type=switcht_value_type.IP_ADDRESS, ip_addr=ip))
        for ip in self.segment_list4:
            self.api_segment_list4.append(switcht_list_val_t(type=switcht_value_type.IP_ADDRESS, ip_addr=ip))
        for ip in self.segment_list5:
            self.api_segment_list5.append(switcht_list_val_t(type=switcht_value_type.IP_ADDRESS, ip_addr=ip))

        self.orif_lb = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_LOOPBACK, vrf_handle=self.ovrf, src_mac=self.rmac)

        # Create route to next node
        self.urif_port1 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port1, vrf_handle=self.uvrf, src_mac=self.rmac)
        self.urif_port2 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port2, vrf_handle=self.uvrf, src_mac=self.rmac)
        self.unhop_port1 = self.add_nexthop(self.device, handle=self.urif_port1, dest_ip='10.10.10.10')
        self.unhop_port2 = self.add_nexthop(self.device, handle=self.urif_port2, dest_ip='10.10.10.11')
        self.uneighbor_port1 = self.add_neighbor(self.device, mac_address=self.underlay_neighbor_mac,
                                                 handle=self.urif_port1, dest_ip='10.10.10.10')
        self.uneighbor_port2 = self.add_neighbor(self.device, mac_address=self.underlay_neighbor_mac,
                                                 handle=self.urif_port2, dest_ip='10.10.10.11')
        self.tunnel_route1 = self.add_route(self.device, ip_prefix=self.locator_list[0],
                                            vrf_handle=self.uvrf, nexthop_handle=self.unhop_port1)
        self.tunnel_route2 = self.add_route(self.device, ip_prefix=self.locator_list[1],
                                            vrf_handle=self.uvrf, nexthop_handle=self.unhop_port1)
        self.tunnel_route3 = self.add_route(self.device, ip_prefix=self.locator_list[2],
                                            vrf_handle=self.uvrf, nexthop_handle=self.unhop_port1)
        self.tunnel_route4 = self.add_route(self.device, ip_prefix=self.locator_list[3],
                                            vrf_handle=self.uvrf, nexthop_handle=self.unhop_port1)
        self.tunnel_route5 = self.add_route(self.device, ip_prefix=self.locator_list[4],
                                            vrf_handle=self.uvrf, nexthop_handle=self.unhop_port1)

        self.urif_port3 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port3, vrf_handle=self.uvrf, src_mac=self.rmac)
        self.xconnect_nhop = self.add_nexthop(self.device, handle=self.urif_port3, dest_ip='10.10.10.12')
        self.xconnect_nbor = self.add_neighbor(self.device, mac_address=self.underlay_xconnect_nbor_mac,
                                               handle=self.urif_port3, dest_ip='10.10.10.12')
        # create Tunnel
        self.tunnel = self.add_tunnel(self.device, type=self.tunnel_type,
                                      src_ip=self.my_lb_ip,
                                      encap_ttl_mode=self.encap_ttl_mode,
                                      decap_ttl_mode=self.decap_ttl_mode,
                                      underlay_rif_handle=self.urif_lb, overlay_rif_handle=self.orif_lb)

        # Create local my_sid entries
        self.end_sid = self.add_my_sid_entry(self.device, endpoint_type=SWITCH_MY_SID_ENTRY_ATTR_ENDPOINT_TYPE_END,
                                          endpoint_flavor=SWITCH_MY_SID_ENTRY_ATTR_ENDPOINT_FLAVOR_PSP_AND_USD,
                                          sid_vrf_handle=self.uvrf, sid=self.segment_list3[0])
        self.end_dt4_sid = self.add_my_sid_entry(self.device, endpoint_type=SWITCH_MY_SID_ENTRY_ATTR_ENDPOINT_TYPE_END_DT46,
                                          sid_vrf_handle=self.uvrf, sid=self.node0_dt4_sid,
                                          vrf_handle=self.ovrf)
        self.end_dx4_sid = self.add_my_sid_entry(self.device, endpoint_type=SWITCH_MY_SID_ENTRY_ATTR_ENDPOINT_TYPE_END_DX4,
                                              nexthop_handle=self.xconnect_nhop,
                                              sid_vrf_handle=self.uvrf, sid=self.node0_dx4_sid)
        self.end_dx6_sid = self.add_my_sid_entry(self.device, endpoint_type=SWITCH_MY_SID_ENTRY_ATTR_ENDPOINT_TYPE_END_DX6,
                                              nexthop_handle=self.xconnect_nhop,
                                              sid_vrf_handle=self.uvrf, sid=self.node0_dx6_sid)

        # Create Local config for overlay
        self.orif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port0, vrf_handle=self.ovrf, src_mac=self.rmac)
        self.onhop4 = self.add_nexthop(self.device, handle=self.orif, dest_ip=self.custA_ip4)
        self.oneighbor4 = self.add_neighbor(self.device,
                                            mac_address=self.overlay_neighbor_mac,
                                            handle=self.orif,
                                            dest_ip=self.custA_ip4)  # 100.100.1.1
        self.custA_ipv4_route = self.add_route(self.device, ip_prefix=self.custA_ip4,
                                               vrf_handle=self.ovrf, nexthop_handle=self.onhop4)

        self.onhop6 = self.add_nexthop(self.device, handle=self.orif, dest_ip=self.custA_ip6)
        self.oneighbor6 = self.add_neighbor(self.device,
                                            mac_address=self.overlay_neighbor_mac,
                                            handle=self.orif,
                                            dest_ip=self.custA_ip6)  # 2001:db8:0:a1:100:0:0:0
        self.custA_ipv6_route = self.add_route(self.device, ip_prefix=self.custA_ip6,
                                               vrf_handle=self.ovrf, nexthop_handle=self.onhop6)
        # Create route for USD
        self.custA_ipv4_route_usd = self.add_route(self.device, ip_prefix=self.custA_ip4,
                                                   vrf_handle=self.uvrf, nexthop_handle=self.unhop_port1)

        self.end_sid_pkt_cntr = 0

    def runTest(self):
        if self.client.is_feature_enable(SWITCH_FEATURE_SRV6) == 0:
            return
        try:
            self.EncapsRed1Test()
            self.EncapsRed2Test()
            self.EncapsRed3Test()
            self.InsertRed1Test()
            self.InsertRed2Test()
            self.SidlistCounterTest()
            self.EndTest()
            self.EndPSPTest()
            self.EndUSDTest()
            self.EndSidCounterTest()
            self.EndDT4NoSRHTest()
            self.B6EncapsRed1Test()
            self.B6EncapsRed2Test()
            self.B6InsertRed1Test()
            self.B6InsertRed2Test()
            self.EndDXTest()
        finally:
            pass

    def tearDown(self):
        self.cleanup()

    def EncapsRed1Test(self):
        print("EncapsRed1Test()")
        try:
            print("Verifying H.Encaps.Red with a single SID")
            if not self.client.is_feature_enable(SWITCH_FEATURE_SRV6):
                print("SRv6 not enabled, skipping")
            else:
                # create srv6 nexthop for custB's IP 100.100.1.1
                self.sidlist_id1 = self.add_segmentroute_sidlist(self.device,
                                                                 type=SWITCH_SEGMENTROUTE_SIDLIST_ATTR_TYPE_H_ENCAPS_RED,
                                                                 segment_list=self.api_segment_list1)
                self.srv6_nexthop1 = self.add_nexthop(self.device,
                                                      type=SWITCH_NEXTHOP_ATTR_TYPE_SRV6_SIDLIST,
                                                      handle=self.tunnel,
                                                      srv6_sidlist_id=self.sidlist_id1)
                self.custB_ipv4_route = self.add_route(self.device,
                                                       ip_prefix=self.custB_ip4, vrf_handle=self.ovrf,
                                                       nexthop_handle=self.srv6_nexthop1)
                pkt = simple_tcp_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src=self.overlay_neighbor_mac,
                    ip_dst=self.custB_ip4,
                    ip_src=self.custA_ip4,
                    ip_id=108,
                    ip_ttl=64)
                pkt2 = simple_tcp_packet(
                    ip_dst=self.custB_ip4,
                    ip_src=self.custA_ip4,
                    ip_id=108,
                    ip_ttl=63)
                ipip_pkt = simple_ipv6ip_packet(
                    eth_src='00:77:66:55:44:33',
                    eth_dst=self.underlay_neighbor_mac,  # 00:11:11:11:11:11
                    ipv6_src=self.my_lb_ip,
                    ipv6_dst=self.segment_list1[0],
                    ipv6_hlim=64,
                    inner_frame=pkt2['IP'])
                send_packet(self, self.devports[0], pkt)
                verify_packet(self, ipip_pkt, self.devports[1])

                self._SidlistStatsTest(self.sidlist_id1)

        finally:
            pass

    def EncapsRed2Test(self):
        print("EncapsRed2Test()")
        try:
            print("Verifying H.Encaps.Red with 2 SIDs")
            if not self.client.is_feature_enable(SWITCH_FEATURE_SRV6):
                print("SRv6 not enabled, skipping")
            else:
                # create srv6 nexthop for custB's IP 100.100.1.1
                self.sidlist_id2 = self.add_segmentroute_sidlist(self.device,
                                                                 type=SWITCH_SEGMENTROUTE_SIDLIST_ATTR_TYPE_H_ENCAPS_RED,
                                                                 segment_list=self.api_segment_list2)
                self.srv6_nexthop2 = self.add_nexthop(self.device,
                                                      type=SWITCH_NEXTHOP_ATTR_TYPE_SRV6_SIDLIST,
                                                      handle=self.tunnel,
                                                      srv6_sidlist_id=self.sidlist_id2)
                self.attribute_set(self.custB_ipv4_route, SWITCH_ROUTE_ATTR_NEXTHOP_HANDLE, self.srv6_nexthop2)
                pkt = simple_tcp_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src=self.overlay_neighbor_mac,
                    ip_dst=self.custB_ip4,
                    ip_src=self.custA_ip4,
                    ip_id=108,
                    ip_ttl=64)
                pkt2 = simple_tcp_packet(
                    ip_dst=self.custB_ip4,
                    ip_src=self.custA_ip4,
                    ip_id=108,
                    ip_ttl=63)
                sr_pkt = simple_ipv6_sr_packet(
                    eth_dst=self.underlay_neighbor_mac,  # 00:11:11:11:11:11
                    eth_src='00:77:66:55:44:33',
                    ipv6_src=self.my_lb_ip,
                    ipv6_dst=self.segment_list2[0],
                    ipv6_hlim=64,
                    srh_seg_left=len(self.segment_list2) - 1,
                    srh_first_seg=len(self.segment_list2) - 2,
                    srh_nh=0x4,
                    srh_seg_list=[self.segment_list2[1]],
                    inner_frame=pkt2['IP'])
                send_packet(self, self.devports[0], pkt)
                verify_packet(self, sr_pkt, self.devports[1])

                self._SidlistStatsTest(self.sidlist_id2)

        finally:
            self.attribute_set(self.custB_ipv4_route, SWITCH_ROUTE_ATTR_NEXTHOP_HANDLE, self.srv6_nexthop1)
            pass

    def EncapsRed3Test(self):
        print("EncapsRed3Test()")
        try:
            print("Verifying H.Encaps.Red with 3 SIDs")
            if not self.client.is_feature_enable(SWITCH_FEATURE_SRV6):
                print("SRv6 not enabled, skipping")
            else:
                # create srv6 nexthop for custB's IP 100.100.1.1
                self.sidlist_id3 = self.add_segmentroute_sidlist(self.device,
                                                                 type=SWITCH_SEGMENTROUTE_SIDLIST_ATTR_TYPE_H_ENCAPS_RED,
                                                                 segment_list=self.api_segment_list3)
                self.srv6_nexthop3 = self.add_nexthop(self.device,
                                                      type=SWITCH_NEXTHOP_ATTR_TYPE_SRV6_SIDLIST,
                                                      handle=self.tunnel,
                                                      srv6_sidlist_id=self.sidlist_id3)
                self.attribute_set(self.custB_ipv4_route, SWITCH_ROUTE_ATTR_NEXTHOP_HANDLE, self.srv6_nexthop3)
                pkt = simple_tcp_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src=self.overlay_neighbor_mac,
                    ip_dst=self.custB_ip4,
                    ip_src=self.custA_ip4,
                    ip_id=108,
                    ip_ttl=64)
                pkt2 = simple_tcp_packet(
                    ip_dst=self.custB_ip4,
                    ip_src=self.custA_ip4,
                    ip_id=108,
                    ip_ttl=63)
                sr_pkt = simple_ipv6_sr_packet(
                    eth_dst=self.underlay_neighbor_mac,  # 00:11:11:11:11:11
                    eth_src='00:77:66:55:44:33',
                    ipv6_src=self.my_lb_ip,
                    ipv6_dst=self.segment_list3[0],
                    ipv6_hlim=64,
                    srh_seg_left=len(self.segment_list3) - 1,
                    srh_first_seg=len(self.segment_list3) - 2,
                    srh_nh=0x4,
                    srh_seg_list=[self.segment_list3[2], self.segment_list3[1]],
                    inner_frame=pkt2['IP'])
                send_packet(self, self.devports[0], pkt)
                verify_packet(self, sr_pkt, self.devports[1])

                self._SidlistStatsTest(self.sidlist_id3)

        finally:
            self.attribute_set(self.custB_ipv4_route, SWITCH_ROUTE_ATTR_NEXTHOP_HANDLE, self.srv6_nexthop1)
            pass

    def InsertRed1Test(self):
        print("InsertRed1Test()")
        try:
            print("Verifying H.Insert.Red with a single SID")
            if not self.client.is_feature_enable(SWITCH_FEATURE_SRV6):
                print("SRv6 not enabled, skipping")
            else:
                self.sidlist_id4 = self.add_segmentroute_sidlist(self.device,
                                                                 type=SWITCH_SEGMENTROUTE_SIDLIST_ATTR_TYPE_H_INSERT_RED,
                                                                 segment_list=self.api_segment_list4)
                self.srv6_nexthop4 = self.add_nexthop(self.device,
                                                      type=SWITCH_NEXTHOP_ATTR_TYPE_SRV6_SIDLIST,
                                                      handle=self.tunnel,
                                                      srv6_sidlist_id=self.sidlist_id4)
                self.custB_ipv6_route = self.add_route(self.device,
                                                       ip_prefix=self.custB_ip6, vrf_handle=self.ovrf,
                                                       nexthop_handle=self.srv6_nexthop4)
                pkt = simple_tcpv6_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src=self.overlay_neighbor_mac,
                    ipv6_dst=self.custB_ip6,
                    ipv6_src=self.custA_ip6,
                    ipv6_hlim=64)
                exp_pkt = simple_ipv6_sr_packet(
                    eth_dst=self.underlay_neighbor_mac,  # 00:11:11:11:11:11
                    eth_src='00:77:66:55:44:33',
                    ipv6_src=self.custA_ip6,
                    ipv6_dst=self.segment_list4[0],
                    ipv6_hlim=63,
                    srh_seg_left=1,
                    srh_first_seg=0,
                    srh_nh=0x6,  # TCP
                    srh_seg_list=[self.custB_ip6],
                    inner_frame=pkt['TCP'])
                send_packet(self, self.devports[0], pkt)
                verify_packet(self, exp_pkt, self.devports[1])

                self._SidlistStatsTest(self.sidlist_id4)
        finally:
            pass

    def InsertRed2Test(self):
        print("InsertRed1Test()")
        try:
            print("Verifying H.Insert.Red with two SIDs")
            if not self.client.is_feature_enable(SWITCH_FEATURE_SRV6):
                print("SRv6 not enabled, skipping")
            else:
                self.sidlist_id5 = self.add_segmentroute_sidlist(self.device,
                                                                 type=SWITCH_SEGMENTROUTE_SIDLIST_ATTR_TYPE_H_INSERT_RED,
                                                                 segment_list=self.api_segment_list5)
                self.srv6_nexthop5 = self.add_nexthop(self.device,
                                                      type=SWITCH_NEXTHOP_ATTR_TYPE_SRV6_SIDLIST,
                                                      handle=self.tunnel,
                                                      srv6_sidlist_id=self.sidlist_id5)
                self.attribute_set(self.custB_ipv6_route, SWITCH_ROUTE_ATTR_NEXTHOP_HANDLE, self.srv6_nexthop5)
                pkt = simple_tcpv6_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src=self.overlay_neighbor_mac,
                    ipv6_dst=self.custB_ip6,
                    ipv6_src=self.custA_ip6,
                    ipv6_hlim=64)
                pkt['TCP'].chksum = 0
                exp_sr_pkt = simple_ipv6_sr_packet(
                    eth_dst=self.underlay_neighbor_mac,  # 00:11:11:11:11:11
                    eth_src='00:77:66:55:44:33',
                    ipv6_src=self.custA_ip6,
                    ipv6_dst=self.segment_list5[0],
                    ipv6_hlim=63,
                    srh_seg_left=2,
                    srh_first_seg=1,
                    srh_nh=0x6,  # TCP
                    srh_seg_list=[self.custB_ip6, self.segment_list5[1]],
                    inner_frame=pkt['TCP'])
                send_packet(self, self.devports[0], pkt)
                verify_packet(self, exp_sr_pkt, self.devports[1])

                self._SidlistStatsTest(self.sidlist_id5)
        finally:
            self.attribute_set(self.custB_ipv6_route, SWITCH_ROUTE_ATTR_NEXTHOP_HANDLE, self.srv6_nexthop4)

    def _SidlistStatsTest(self, sidlist_id, pkt_no=1):
        counters = self.client.object_counters_get(sidlist_id)
        for cntr in counters:
            if cntr.counter_id == SWITCH_SEGMENTROUTE_SIDLIST_COUNTER_ID_PKTS:
                self.assertEqual(cntr.count, pkt_no, "Improper packet counter value")
            elif cntr.counter_id == SWITCH_SEGMENTROUTE_SIDLIST_COUNTER_ID_BYTES:
                self.assertNotEqual(cntr.count, 0, "Bytes counter value should be non-zero")
            else:
                self.fail("Unknown sidlist packet counter ID")

        self.client.object_counters_clear(sidlist_id, [SWITCH_SEGMENTROUTE_SIDLIST_COUNTER_ID_BYTES])
        counters = self.client.object_counters_get(sidlist_id)
        for cntr in counters:
            if cntr.counter_id == SWITCH_SEGMENTROUTE_SIDLIST_COUNTER_ID_PKTS:
                self.assertEqual(pkt_no, cntr.count, "Improper packet counter value")
            elif cntr.counter_id == SWITCH_SEGMENTROUTE_SIDLIST_COUNTER_ID_BYTES:
                self.assertEqual(cntr.count, 0, "Bytes counter value should be zero")
            else:
                self.fail("Unknown sidlist packet counter ID")

        self.client.object_counters_clear_all(sidlist_id)
        counters = self.client.object_counters_get(sidlist_id)
        for cntr in counters:
            self.assertEqual(cntr.count, 0, "sidlist counters not cleared")

    def SidlistCounterTest(self):
        print("SidlistCounterTest()")
        try:
            print("Verifying H.Encaps.Red with a single SID")
            if not self.client.is_feature_enable(SWITCH_FEATURE_SRV6):
                print("SRv6 not enabled, skipping")
            else:
                # create srv6 nexthop for custB's IP 100.100.1.
                # use sidlist_id1
                cust_ip = "192.168.100.1"
                cust_route1 = self.add_route(self.device,
                                             ip_prefix=cust_ip, vrf_handle=self.ovrf,
                                             nexthop_handle=self.srv6_nexthop1)
                pkt = simple_tcp_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src=self.overlay_neighbor_mac,
                    ip_dst=cust_ip,
                    ip_src=self.custA_ip4,
                    ip_id=108,
                    ip_ttl=64)
                pkt2 = simple_tcp_packet(
                    ip_dst=cust_ip,
                    ip_src=self.custA_ip4,
                    ip_id=108,
                    ip_ttl=63)
                ipip_pkt = simple_ipv6ip_packet(
                    eth_src='00:77:66:55:44:33',
                    eth_dst=self.underlay_neighbor_mac,  # 00:11:11:11:11:11
                    ipv6_src=self.my_lb_ip,
                    ipv6_dst=self.segment_list1[0],
                    ipv6_hlim=64,
                    inner_frame=pkt2['IP'])
                send_packet(self, self.devports[0], pkt)
                verify_packet(self, ipip_pkt, self.devports[1])

                # verify counter
                counters = self.client.object_counters_get(self.sidlist_id1)
                for cntr in counters:
                    if cntr.counter_id == SWITCH_SEGMENTROUTE_SIDLIST_COUNTER_ID_PKTS:
                        self.assertEqual(cntr.count, 1, "Improper packet counter value")
                    elif cntr.counter_id == SWITCH_SEGMENTROUTE_SIDLIST_COUNTER_ID_BYTES:
                        self.assertNotEqual(cntr.count, 0, "Bytes counter value should be non-zero")

                # create another nexthop
                my_lb_ip = '2001:db8:0:f1::'
                self.tunnel = self.add_tunnel(self.device, type=self.tunnel_type,
                                         src_ip=my_lb_ip,
                                         encap_ttl_mode=self.encap_ttl_mode,
                                         decap_ttl_mode=self.decap_ttl_mode,
                                         underlay_rif_handle=self.urif_lb, overlay_rif_handle=self.orif_lb)
                self.srv6_nexthop = self.add_nexthop(self.device,
                                                type=SWITCH_NEXTHOP_ATTR_TYPE_SRV6_SIDLIST,
                                                handle=self.tunnel,
                                                srv6_sidlist_id=self.sidlist_id1)
                self.attribute_set(cust_route1, SWITCH_ROUTE_ATTR_NEXTHOP_HANDLE, self.srv6_nexthop)

                ipip_pkt = simple_ipv6ip_packet(
                    eth_src='00:77:66:55:44:33',
                    eth_dst=self.underlay_neighbor_mac,  # 00:11:11:11:11:11
                    ipv6_src=my_lb_ip,
                    ipv6_dst=self.segment_list1[0],
                    ipv6_hlim=64,
                    inner_frame=pkt2['IP'])

                send_packet(self, self.devports[0], pkt)
                verify_packet(self, ipip_pkt, self.devports[1])

                # verify counter - should be incremented
                counters = self.client.object_counters_get(self.sidlist_id1)
                for cntr in counters:
                    if cntr.counter_id == SWITCH_SEGMENTROUTE_SIDLIST_COUNTER_ID_PKTS:
                        self.assertEqual(cntr.count, 2, "Improper packet counter value")
                    elif cntr.counter_id == SWITCH_SEGMENTROUTE_SIDLIST_COUNTER_ID_BYTES:
                        self.assertNotEqual(cntr.count, 0, "Bytes counter value should be non-zero")

                self.attribute_set(cust_route1, SWITCH_ROUTE_ATTR_NEXTHOP_HANDLE, self.srv6_nexthop1)

                # remove nexthop and verify counter - counter should be unchanged
                self.client.object_delete(self.srv6_nexthop)
                counters = self.client.object_counters_get(self.sidlist_id1)
                for cntr in counters:
                    if cntr.counter_id == SWITCH_SEGMENTROUTE_SIDLIST_COUNTER_ID_PKTS:
                        self.assertEqual(cntr.count, 2, "Improper packet counter value")
                    elif cntr.counter_id == SWITCH_SEGMENTROUTE_SIDLIST_COUNTER_ID_BYTES:
                        self.assertNotEqual(cntr.count, 0, "Bytes counter value should be non-zero")

                self.client.object_counters_clear_all(self.sidlist_id1)
                counters = self.client.object_counters_get(self.sidlist_id1)
                for cntr in counters:
                    self.assertEqual(cntr.count, 0, "sidlist counters not cleared")

        finally:
            pass

    def EndTest(self):
        print("EndTest()")
        try:
            print("Verifying End function with SL>1")
            if not self.client.is_feature_enable(SWITCH_FEATURE_SRV6):
                print("SRv6 not enabled, skipping")
            else:
                pkt1 = simple_tcp_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src=self.underlay_neighbor_mac,  # 00:11:11:11:11:11
                    ip_src=self.custB_ip4,
                    ip_dst=self.custA_ip4,
                    ip_id=108,
                    ip_ttl=63)
                sr_pkt = simple_ipv6_sr_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src=self.underlay_neighbor_mac,  # 00:11:11:11:11:11
                    ipv6_src=self.my_lb_ip,
                    ipv6_dst=self.segment_list3[0],
                    ipv6_hlim=64,
                    srh_seg_left=len(self.segment_list3) - 1,
                    srh_first_seg=len(self.segment_list3) - 2,
                    srh_nh=0x4,
                    srh_seg_list=[self.segment_list3[2], self.segment_list3[1]],
                    inner_frame=pkt1['IP'])
                exp_pkt = simple_ipv6_sr_packet(
                    eth_src='00:77:66:55:44:33',
                    eth_dst=self.underlay_neighbor_mac,  # 00:11:11:11:11:11
                    ipv6_src=self.my_lb_ip,
                    ipv6_dst=self.segment_list3[1],
                    ipv6_hlim=63,
                    srh_seg_left=len(self.segment_list3) - 2,
                    srh_first_seg=len(self.segment_list3) - 2,
                    srh_nh=0x4,
                    srh_seg_list=[self.segment_list3[2], self.segment_list3[1]],
                    inner_frame=pkt1['IP'])
                send_packet(self, self.devports[2], sr_pkt)
                verify_packet(self, exp_pkt, self.devports[1])

                self.end_sid_pkt_cntr += 1

        finally:
            pass

    def EndPSPTest(self):
        print("EndPSPTest()")
        try:
            print("Verifying End function with SL=1")
            if not self.client.is_feature_enable(SWITCH_FEATURE_SRV6):
                print("SRv6 not enabled, skipping")
            else:
                pkt1 = simple_tcp_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src=self.underlay_neighbor_mac,  # 00:11:11:11:11:11
                    ip_src=self.custB_ip4,
                    ip_dst=self.custA_ip4,
                    ip_id=108,
                    ip_ttl=63)
                sr_pkt = simple_ipv6_sr_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src=self.underlay_neighbor_mac,  # 00:11:11:11:11:11
                    ipv6_src=self.my_lb_ip,
                    ipv6_dst=self.segment_list3[0],
                    ipv6_hlim=63,
                    srh_seg_left=len(self.segment_list3) - 2,
                    srh_first_seg=len(self.segment_list3) - 2,
                    srh_nh=0x4,
                    srh_seg_list=[self.segment_list3[2], self.segment_list3[1]],
                    inner_frame=pkt1['IP'])
                ipip_pkt = simple_ipv6ip_packet(
                    eth_src='00:77:66:55:44:33',
                    eth_dst=self.underlay_neighbor_mac,  # 00:11:11:11:11:11
                    ipv6_src=self.my_lb_ip,
                    ipv6_dst=self.segment_list3[2],
                    ipv6_hlim=62,
                    inner_frame=pkt1['IP'])
                send_packet(self, self.devports[2], sr_pkt)
                verify_packet(self, ipip_pkt, self.devports[1])

                self.end_sid_pkt_cntr += 1

        finally:
            pass

    def EndUSDTest(self):
        print("EndUSDTest()")
        try:
            print("Verifying End function with SL=0")
            if not self.client.is_feature_enable(SWITCH_FEATURE_SRV6):
                print("SRv6 not enabled, skipping")
            else:
                pkt1 = simple_tcp_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src=self.underlay_neighbor_mac,  # 00:11:11:11:11:11
                    ip_src=self.custB_ip4,
                    ip_dst=self.custA_ip4,
                    ip_id=108,
                    ip_ttl=63)
                sr_pkt = simple_ipv6_sr_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src=self.underlay_neighbor_mac,  # 00:11:11:11:11:11
                    ipv6_src=self.my_lb_ip,
                    ipv6_dst=self.segment_list3[0],
                    ipv6_hlim=64,
                    srh_seg_left=len(self.segment_list3) - 3,
                    srh_first_seg=len(self.segment_list3) - 2,
                    srh_nh=0x4,
                    srh_seg_list=[self.segment_list3[2], self.segment_list3[1]],
                    inner_frame=pkt1['IP'])
                pkt2 = simple_tcp_packet(
                    eth_dst=self.underlay_neighbor_mac,
                    eth_src='00:77:66:55:44:33',
                    ip_dst=self.custA_ip4,
                    ip_src=self.custB_ip4,
                    ip_id=108,
                    ip_ttl=62)
                send_packet(self, self.devports[2], sr_pkt)
                verify_packet(self, pkt2, self.devports[1])

                self.end_sid_pkt_cntr += 1

        finally:
            pass

    def EndSidCounterTest(self):
        print("EndSidCounterTest()")

        if not self.client.is_feature_enable(SWITCH_FEATURE_SRV6):
            print("SRv6 not enabled, skipping")
        else:
            counters = self.client.object_counters_get(self.end_sid)

            for cntr in counters:
                if cntr.counter_id == SWITCH_MY_SID_ENTRY_COUNTER_ID_PKTS:
                    self.assertEqual(self.end_sid_pkt_cntr, cntr.count,
                                     "Improper packet counter value")
                elif cntr.counter_id == SWITCH_MY_SID_ENTRY_COUNTER_ID_BYTES:
                    self.assertNotEqual(cntr.count, 0, "Bytes counter value should be non-zero")
                else:
                    self.fail("Unknown SID packet counter ID")

            self.client.object_counters_clear(self.end_sid, [SWITCH_MY_SID_ENTRY_COUNTER_ID_BYTES])
            counters = self.client.object_counters_get(self.end_sid)
            for cntr in counters:
                if cntr.counter_id == SWITCH_MY_SID_ENTRY_COUNTER_ID_PKTS:
                    self.assertEqual(self.end_sid_pkt_cntr, cntr.count,
                                    "Improper packet counter value")
                elif cntr.counter_id == SWITCH_MY_SID_ENTRY_COUNTER_ID_BYTES:
                    self.assertEqual(cntr.count, 0, "Bytes counter value should be zero")
                else:
                    self.fail("Unknown SID packet counter ID")

            self.client.object_counters_clear_all(self.end_sid)
            counters = self.client.object_counters_get(self.end_sid)
            for cntr in counters:
                self.assertEqual(cntr.count, 0, "my_sid counters not cleared")

    def EndDT4NoSRHTest(self):
        print("EndDT4NoSRHTest()")
        try:
            print("Verifying End.DT4 function with no SRH")
            if not self.client.is_feature_enable(SWITCH_FEATURE_SRV6):
                print("SRv6 not enabled, skipping")
            else:
                pkt1 = simple_tcp_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src=self.underlay_neighbor_mac,  # 00:11:11:11:11:11
                    ip_src=self.custB_ip4,
                    ip_dst=self.custA_ip4,
                    ip_id=108,
                    ip_ttl=63)
                ipip_pkt = simple_ipv6ip_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src=self.underlay_neighbor_mac,  # 00:11:11:11:11:11
                    ipv6_src=self.remote_lb_ip,
                    ipv6_dst=self.node0_dt4_sid,
                    ipv6_hlim=63,
                    inner_frame=pkt1['IP'])
                pkt2 = simple_tcp_packet(
                    eth_src='00:77:66:55:44:33',
                    eth_dst=self.overlay_neighbor_mac,
                    ip_dst=self.custA_ip4,
                    ip_src=self.custB_ip4,
                    ip_id=108,
                    ip_ttl=62)
                send_packet(self, self.devports[1], ipip_pkt)
                verify_packet(self, pkt2, self.devports[0])

        finally:
            pass

    def B6EncapsRed1Test(self):
        print("B6EncapsRed1Test()")
        try:
            print("Verifying B6.Encaps.Red with a single SID")
            if not self.client.is_feature_enable(SWITCH_FEATURE_SRV6):
                print("SRv6 not enabled, skipping")
            else:
                self.attribute_set(self.tunnel_route3, SWITCH_ROUTE_ATTR_NEXTHOP_HANDLE, self.unhop_port2)
                self.node0_bsid = '2001:db8:0:a1::100'
                self.my_sid = self.add_my_sid_entry(self.device, endpoint_type=SWITCH_MY_SID_ENTRY_ATTR_ENDPOINT_TYPE_END_B6_ENCAPS_RED,
                                                    sid=self.node0_bsid,
                                                    nexthop_handle=self.srv6_nexthop1, sid_vrf_handle=self.uvrf)

                pkt1 = simple_tcp_packet(
                    eth_dst='00:44:44:44:44:44',
                    eth_src='00:33:33:33:33:33',
                    ip_src=self.custB_ip4,
                    ip_dst=self.custA_ip4,
                    ip_id=108,
                    ip_ttl=63)
                # SRv6 packet from Node 6
                sr_pkt = simple_ipv6_sr_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src=self.underlay_neighbor_mac,  # 00:11:11:11:11:11
                    ipv6_src=self.node6_prefix_sid,
                    ipv6_dst=self.node0_bsid,
                    ipv6_hlim=60,
                    srh_seg_left=2,
                    srh_first_seg=1,
                    srh_nh=0x4,
                    srh_seg_list=[self.node5_prefix_sid, self.node4_prefix_sid],
                    inner_frame=pkt1['IP'])
                # After End operation
                inner_sr_pkt = simple_ipv6_sr_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src=self.underlay_neighbor_mac,  # 00:11:11:11:11:11
                    ipv6_src=self.node6_prefix_sid,
                    ipv6_dst=self.node4_prefix_sid,
                    ipv6_hlim=59,
                    srh_seg_left=1,
                    srh_first_seg=1,
                    srh_nh=0x4,
                    srh_seg_list=[self.node5_prefix_sid, self.node4_prefix_sid],
                    inner_frame=pkt1['IP'])
                # After Encaps operation
                ipip_pkt = simple_ipv6ip_packet(
                    eth_src='00:77:66:55:44:33',
                    eth_dst=self.underlay_neighbor_mac,  # 00:11:11:11:11:11
                    ipv6_src=self.my_lb_ip,
                    ipv6_dst=self.segment_list1[0],
                    ipv6_hlim=64,
                    inner_frame=inner_sr_pkt['IPv6'])
                send_packet(self, self.devports[1], sr_pkt)
                verify_packet(self, ipip_pkt, self.devports[2])

        finally:
            pass

    def B6EncapsRed2Test(self):
        print("B6EncapsRed2Test()")
        try:
            print("Verifying B6.Encaps.Red with two SIDs")
            if not self.client.is_feature_enable(SWITCH_FEATURE_SRV6):
                print("SRv6 not enabled, skipping")
            else:
                self.node0_bsid = '2001:db8:0:a1::200'
                self.my_sid = self.add_my_sid_entry(self.device, endpoint_type=SWITCH_MY_SID_ENTRY_ATTR_ENDPOINT_TYPE_END_B6_ENCAPS_RED,
                                                    sid=self.node0_bsid,
                                                    nexthop_handle=self.srv6_nexthop2,
                                                    sid_vrf_handle=self.uvrf)

                pkt1 = simple_tcp_packet(
                    eth_dst='00:44:44:44:44:44',
                    eth_src='00:33:33:33:33:33',
                    ip_src=self.custB_ip4,
                    ip_dst=self.custA_ip4,
                    ip_id=108,
                    ip_ttl=63)
                # SRv6 packet from Node 6
                sr_pkt = simple_ipv6_sr_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src=self.underlay_neighbor_mac,  # 00:11:11:11:11:11
                    ipv6_src=self.node6_prefix_sid,
                    ipv6_dst=self.node0_bsid,
                    ipv6_hlim=60,
                    srh_seg_left=2,
                    srh_first_seg=1,
                    srh_nh=0x4,
                    srh_seg_list=[self.node5_prefix_sid, self.node4_prefix_sid],
                    inner_frame=pkt1['IP'])
                # After End operation
                inner_sr_pkt = simple_ipv6_sr_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src=self.underlay_neighbor_mac,  # 00:11:11:11:11:11
                    ipv6_src=self.node6_prefix_sid,
                    ipv6_dst=self.node4_prefix_sid,
                    ipv6_hlim=59,
                    srh_seg_left=1,
                    srh_first_seg=1,
                    srh_nh=0x4,
                    srh_seg_list=[self.node5_prefix_sid, self.node4_prefix_sid],
                    inner_frame=pkt1['IP'])
                # After Encaps operation
                sr_in_sr_pkt = simple_ipv6_sr_packet(
                    eth_src='00:77:66:55:44:33',
                    eth_dst=self.underlay_neighbor_mac,  # 00:11:11:11:11:11
                    ipv6_src=self.my_lb_ip,
                    ipv6_dst=self.segment_list2[0],
                    ipv6_hlim=64,
                    srh_seg_left=1,
                    srh_first_seg=0,
                    srh_nh=0x29,
                    srh_seg_list=[self.segment_list2[1]],
                    inner_frame=inner_sr_pkt['IPv6'])

                send_packet(self, self.devports[2], sr_pkt)
                verify_packet(self, sr_in_sr_pkt, self.devports[1])

        finally:
            pass

    def B6InsertRed1Test(self):
        print("B6InsertRed1Test")
        try:
            print("Verifying B6.Insert.Red with a single SID")
            if not self.client.is_feature_enable(SWITCH_FEATURE_SRV6):
                print("SRv6 not enabled, skipping")
            else:
                self.node0_bsid = '2001:db8:0:a1::300'
                self.my_sid = self.add_my_sid_entry(self.device, endpoint_type=SWITCH_MY_SID_ENTRY_ATTR_ENDPOINT_TYPE_END_B6_INSERT_RED,
                                                    sid=self.node0_bsid,
                                                    nexthop_handle=self.srv6_nexthop4,
                                                    sid_vrf_handle=self.uvrf)
                pkt = simple_tcp_packet(
                    eth_dst='00:44:44:44:44:44',
                    eth_src='00:33:33:33:33:33',
                    ip_src=self.custB_ip4,
                    ip_dst=self.custA_ip4,
                    ip_id=108,
                    ip_ttl=63)
                # SRv6 packet from Node 6
                sr_pkt = simple_ipv6_sr_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src=self.underlay_neighbor_mac,  # 00:11:11:11:11:11
                    ipv6_src=self.node6_prefix_sid,
                    ipv6_dst=self.node0_bsid,
                    ipv6_hlim=60,
                    srh_seg_left=2,
                    srh_first_seg=0,
                    srh_nh=0x4,
                    srh_seg_list=[self.node5_prefix_sid, self.node4_prefix_sid],
                    inner_frame=pkt['IP'])
                exp_sr_pkt = simple_ipv6_sr_packet(
                    eth_dst=self.underlay_neighbor_mac,  # 00:11:11:11:11:11
                    eth_src='00:77:66:55:44:33',
                    ipv6_src=self.node6_prefix_sid,
                    ipv6_dst=self.segment_list4[0],
                    ipv6_hlim=59,
                    srh_seg_left=1,
                    srh_first_seg=0,
                    srh_nh=0x2b,  # = 43 => SRH
                    srh_seg_list=[self.node0_bsid],
                    inner_frame=None)
                exp_sr_pkt /= IPv6ExtHdrRouting(
                    nh=0x4,
                    type=4,
                    segleft=2,
                    reserved = 0,
                    addresses=[self.node5_prefix_sid, self.node4_prefix_sid])
                exp_sr_pkt /= pkt["IP"]
                send_packet(self, self.devports[1], sr_pkt)
                verify_packet(self, exp_sr_pkt, self.devports[1])

                # IPinIP case
                sr_pkt = simple_ipv6ip_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src=self.underlay_neighbor_mac,  # 00:11:11:11:11:11
                    ipv6_src=self.node6_prefix_sid,
                    ipv6_dst=self.node0_bsid,
                    ipv6_hlim=60,
                    inner_frame=pkt['IP'])
                exp_sr_pkt = simple_ipv6_sr_packet(
                    eth_dst=self.underlay_neighbor_mac,  # 00:11:11:11:11:11
                    eth_src='00:77:66:55:44:33',
                    ipv6_src=self.node6_prefix_sid,
                    ipv6_dst=self.segment_list4[0],
                    ipv6_hlim=59,
                    srh_seg_left=1,
                    srh_first_seg=0,
                    srh_nh=0x4,
                    srh_seg_list=[self.node0_bsid],
                    inner_frame=pkt["IP"])
                send_packet(self, self.devports[1], sr_pkt)
                verify_packet(self, exp_sr_pkt, self.devports[1])
        finally:
            pass

    def B6InsertRed2Test(self):
        print("B6InsertRed2Test")
        try:
            print("Verifying B6.Insert.Red with two SIDs")
            if not self.client.is_feature_enable(SWITCH_FEATURE_SRV6):
                print("SRv6 not enabled, skipping")
            else:
                self.node0_bsid = '2001:db8:0:a1::400'
                self.my_sid = self.add_my_sid_entry(self.device, endpoint_type=SWITCH_MY_SID_ENTRY_ATTR_ENDPOINT_TYPE_END_B6_INSERT_RED,
                                                    sid=self.node0_bsid,
                                                    nexthop_handle=self.srv6_nexthop5,
                                                    sid_vrf_handle=self.uvrf)
                pkt = simple_tcp_packet(
                    eth_dst='00:44:44:44:44:44',
                    eth_src='00:33:33:33:33:33',
                    ip_src=self.custB_ip4,
                    ip_dst=self.custA_ip4,
                    ip_id=108,
                    ip_ttl=63)
                # SRv6 packet from Node 6
                sr_pkt = simple_ipv6_sr_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src=self.underlay_neighbor_mac,  # 00:11:11:11:11:11
                    ipv6_src=self.node6_prefix_sid,
                    ipv6_dst=self.node0_bsid,
                    ipv6_hlim=60,
                    srh_seg_left=2,
                    srh_first_seg=1,
                    srh_nh=0x4,
                    srh_seg_list=[self.node5_prefix_sid, self.node4_prefix_sid],
                    inner_frame=pkt['IP'])
                exp_sr_pkt = simple_ipv6_sr_packet(
                    eth_dst=self.underlay_neighbor_mac,  # 00:11:11:11:11:11
                    eth_src='00:77:66:55:44:33',
                    ipv6_src=self.node6_prefix_sid,
                    ipv6_dst=self.segment_list5[0],
                    ipv6_hlim=59,
                    srh_seg_left=2,
                    srh_first_seg=1,
                    srh_nh=0x2b,  # = 43 => SRH
                    srh_seg_list=[self.node0_bsid, self.segment_list5[1]],
                    inner_frame=None)
                exp_sr_pkt /= IPv6ExtHdrRouting(
                    nh=0x4,
                    type=4,
                    segleft=2,
                    reserved=(1 << 24),  # to set lastentry field
                    addresses=[self.node5_prefix_sid, self.node4_prefix_sid])
                exp_sr_pkt /= pkt['IP']
                send_packet(self, self.devports[2], sr_pkt)
                verify_packet(self, exp_sr_pkt, self.devports[1])

                # IPinIP case
                sr_pkt = simple_ipv6ip_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src=self.underlay_neighbor_mac,  # 00:11:11:11:11:11
                    ipv6_src=self.node6_prefix_sid,
                    ipv6_dst=self.node0_bsid,
                    ipv6_hlim=60,
                    inner_frame=pkt['IP'])
                exp_sr_pkt = simple_ipv6_sr_packet(
                    eth_dst=self.underlay_neighbor_mac,  # 00:11:11:11:11:11
                    eth_src='00:77:66:55:44:33',
                    ipv6_src=self.node6_prefix_sid,
                    ipv6_dst=self.segment_list5[0],
                    ipv6_hlim=59,
                    srh_seg_left=2,
                    srh_first_seg=1,
                    srh_nh=0x4,
                    srh_seg_list=[self.node0_bsid, self.segment_list5[1]],
                    inner_frame=pkt["IP"])
                send_packet(self, self.devports[1], sr_pkt)
                verify_packet(self, exp_sr_pkt, self.devports[1])
        finally:
            pass

    def EndDXTest(self):
        print("EndDXTest()")
        try:
            if not self.client.is_feature_enable(SWITCH_FEATURE_SRV6):
                print("SRv6 not enabled, skipping")
            else:
                print("Verifying End.DX4 function with IPv4 inner packet")
                v4_pkt = simple_tcp_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src=self.underlay_neighbor_mac,
                    ip_dst=self.custA_ip4,
                    ip_src=self.custB_ip4,
                    ip_id=108,
                    ip_ttl=64)
                sr_pkt = simple_ipv6ip_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src=self.underlay_neighbor_mac,
                    ipv6_src=self.remote_lb_ip,
                    ipv6_dst=self.node0_dx4_sid,
                    ipv6_hlim=64,
                    inner_frame=v4_pkt['IP'])
                exp_v4_pkt = simple_tcp_packet(
                    eth_dst=self.underlay_xconnect_nbor_mac,
                    eth_src='00:77:66:55:44:33',
                    ip_dst=self.custA_ip4,
                    ip_src=self.custB_ip4,
                    ip_id=108,
                    ip_ttl=63)

                send_packet(self, self.devports[2], sr_pkt)
                verify_packet(self, exp_v4_pkt, self.devports[3])

                print("Verifying End.DX6 function with IPv6 inner packet")
                v6_pkt = simple_tcpv6_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src=self.underlay_neighbor_mac,
                    ipv6_dst=self.custA_ip6,
                    ipv6_src=self.custB_ip6,
                    ipv6_hlim=64)
                sr_pkt = simple_ipv6ip_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src=self.underlay_neighbor_mac,
                    ipv6_src=self.remote_lb_ip,
                    ipv6_dst=self.node0_dx6_sid,
                    ipv6_hlim=64,
                    inner_frame=v6_pkt['IPv6'])
                exp_v6_pkt = simple_tcpv6_packet(
                    eth_dst=self.underlay_xconnect_nbor_mac,
                    eth_src='00:77:66:55:44:33',
                    ipv6_dst=self.custA_ip6,
                    ipv6_src=self.custB_ip6,
                    ipv6_hlim=63)

                send_packet(self, self.devports[2], sr_pkt)
                verify_packet(self, exp_v6_pkt, self.devports[3])

        finally:
            pass


@group('srv6')
class SRv6IPv4inIPv6uSIDTest(ApiHelper):
    def setUp(self):
        self.configure()
        if self.client.is_feature_enable(SWITCH_FEATURE_SRV6) == 0:
            print("SRv6 not enabled, skipping")
            return
        print()

        # underlay config
        # create a srv6 nexthop
        # create a policy which points to a srv6 nexthop

        # underlay config
        self.uvrf = self.add_vrf(self.device, id=100, src_mac=self.rmac)
        self.my_lb_ip = '2001:db8:0:a1::'
        self.underlay_neighbor_mac = '00:11:11:11:11:11'
        self.overlay_neighbor_mac = '00:22:22:22:22:22'
        self.underlay_xconnect_nbor_mac = '00:33:33:33:33:33'
        self.urif_lb = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_LOOPBACK,
                                    vrf_handle=self.uvrf, src_mac=self.rmac)

        # overlay configuration
        self.custA_ip4 = '100.100.1.1'
        self.custB_ip4 = '100.100.0.1'
        self.ovrf = self.add_vrf(self.device, id=300, src_mac=self.rmac)
        self.tunnel_type = SWITCH_TUNNEL_ATTR_TYPE_SRV6
        self.encap_ttl_mode = SWITCH_TUNNEL_ATTR_ENCAP_TTL_MODE_PIPE_MODEL
        self.decap_ttl_mode = SWITCH_TUNNEL_ATTR_DECAP_TTL_MODE_PIPE_MODEL

        # Create segments lists
        self.in_uN_local_usid = '2001:db8:100::'
        self.in_uN_usid = '2001:db8:100:200:300:400:500:600'
        self.out_uN_usid = '2001:db8:200:300:400:500:600::'
        self.in_uA_local_usid = '2001:db8:700:800::'
        self.in_uA_usid = '2001:db8:700:800:900:a00:b00:c00'
        self.out_uA_usid = '2001:db8:800:900:a00:b00:c00::'
        self.next_usid1 = '2001:db8:1100:1200:1300:1400:1500:1600'
        self.next_usid2 = '2001:db8:2100:2200:2300:2400:2500:2600'

        self.usid_segment_list = [self.next_usid2, self.next_usid1]
        self.usid_locator_list = [self.in_uN_local_usid + '/48', self.out_uN_usid + '/48', self.next_usid1 + '/48', self.next_usid2 + '/48']

        self.orif_lb = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_LOOPBACK, vrf_handle=self.ovrf, src_mac=self.rmac)

        # Create route to next node
        self.urif_port1 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port1, vrf_handle=self.uvrf, src_mac=self.rmac)
        self.urif_port2 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port2, vrf_handle=self.uvrf, src_mac=self.rmac)
        self.unhop_port1 = self.add_nexthop(self.device, handle=self.urif_port1, dest_ip='10.10.10.10')
        self.unhop_port2 = self.add_nexthop(self.device, handle=self.urif_port2, dest_ip='10.10.10.11')
        self.uneighbor_port1 = self.add_neighbor(self.device, mac_address=self.underlay_neighbor_mac,
                                                 handle=self.urif_port1, dest_ip='10.10.10.10')
        self.uneighbor_port2 = self.add_neighbor(self.device, mac_address=self.underlay_neighbor_mac,
                                                 handle=self.urif_port2, dest_ip='10.10.10.11')

        self.tunnel_route_uN_usid1 = self.add_route(self.device, ip_prefix=self.usid_locator_list[0],
                                                    vrf_handle=self.uvrf, nexthop_handle=self.unhop_port1)
        self.tunnel_route_uN_usid2 = self.add_route(self.device, ip_prefix=self.usid_locator_list[1],
                                                    vrf_handle=self.uvrf, nexthop_handle=self.unhop_port1)
        self.tunnel_route_uN_usid3 = self.add_route(self.device, ip_prefix=self.usid_locator_list[2],
                                                    vrf_handle=self.uvrf, nexthop_handle=self.unhop_port1)
        self.tunnel_route_uN_usid4 = self.add_route(self.device, ip_prefix=self.usid_locator_list[3],
                                                    vrf_handle=self.uvrf, nexthop_handle=self.unhop_port1)

        self.urif_port3 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port3, vrf_handle=self.uvrf, src_mac=self.rmac)
        self.xconnect_nhop = self.add_nexthop(self.device, handle=self.urif_port3, dest_ip='10.10.10.12')
        self.xconnect_nbor = self.add_neighbor(self.device, mac_address=self.underlay_xconnect_nbor_mac,
                                               handle=self.urif_port3, dest_ip='10.10.10.12')
        # create Tunnel
        self.tunnel = self.add_tunnel(self.device, type=self.tunnel_type,
                                      src_ip=self.my_lb_ip,
                                      encap_ttl_mode=self.encap_ttl_mode,
                                      decap_ttl_mode=self.decap_ttl_mode,
                                      underlay_rif_handle=self.urif_lb, overlay_rif_handle=self.orif_lb)

        # Create my my_sid entries
        self.end_uN_sid1 = self.add_my_sid_entry(self.device, endpoint_type=SWITCH_MY_SID_ENTRY_ATTR_ENDPOINT_TYPE_END_UN,
                                              sid_vrf_handle=self.uvrf, sid=self.in_uN_local_usid)
        self.end_uA_sid1 = self.add_my_sid_entry(self.device, endpoint_type=SWITCH_MY_SID_ENTRY_ATTR_ENDPOINT_TYPE_END_UA,
                                              nexthop_handle=self.xconnect_nhop,
                                              sid_vrf_handle=self.uvrf, sid=self.in_uA_local_usid)

        # Create Local config for overlay
        self.orif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port0, vrf_handle=self.ovrf, src_mac=self.rmac)
        self.onhop4 = self.add_nexthop(self.device, handle=self.orif, dest_ip=self.custA_ip4)
        self.oneighbor4 = self.add_neighbor(self.device,
                                            mac_address=self.overlay_neighbor_mac,
                                            handle=self.orif,
                                            dest_ip=self.custA_ip4)  # 100.100.1.1
        self.custA_ipv4_route = self.add_route(self.device, ip_prefix=self.custA_ip4,
                                               vrf_handle=self.ovrf, nexthop_handle=self.onhop4)
        # Create route for USD
        self.custA_ipv4_route_usd = self.add_route(self.device, ip_prefix=self.custA_ip4,
                                                   vrf_handle=self.uvrf, nexthop_handle=self.unhop_port1)

    def runTest(self):
        if self.client.is_feature_enable(SWITCH_FEATURE_SRV6) == 0:
            return
        try:
            self.EndUNTest()
            self.EndUNPSPTest()
            self.EndUNUSDTest()
            self.EndUNUSDNoSRHTest()
            self.EndUATest()
            self.EndUAPSPTest()
            self.EndUAUSDTest()
            self.EndUAUSDNoSRHTest()
        finally:
            pass

    def tearDown(self):
        self.cleanup()

    def EndUNTest(self):
        print("EndUNTest()")
        try:
            if not self.client.is_feature_enable(SWITCH_FEATURE_SRV6):
                print("SRv6 not enabled, skipping")
            else:
                print("Verifying End.uN function with SL>1 and >1 uSIDs left")
                pkt1 = simple_tcp_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src=self.underlay_neighbor_mac,
                    ip_src=self.custB_ip4,
                    ip_dst=self.custA_ip4,
                    ip_id=108,
                    ip_ttl=63)
                sr_pkt = simple_ipv6_sr_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src=self.underlay_neighbor_mac,
                    ipv6_src=self.my_lb_ip,
                    ipv6_dst=self.in_uN_usid,
                    ipv6_hlim=64,
                    srh_seg_left=len(self.usid_segment_list),       # 2
                    srh_first_seg=len(self.usid_segment_list) - 1,  # 1
                    srh_nh=0x4,
                    srh_seg_list=self.usid_segment_list,
                    inner_frame=pkt1['IP'])
                exp_pkt = simple_ipv6_sr_packet(
                    eth_src='00:77:66:55:44:33',
                    eth_dst=self.underlay_neighbor_mac,
                    ipv6_src=self.my_lb_ip,
                    ipv6_dst=self.out_uN_usid,
                    ipv6_hlim=63,
                    srh_seg_left=len(self.usid_segment_list),       # 2
                    srh_first_seg=len(self.usid_segment_list) - 1,  # 1
                    srh_nh=0x4,
                    srh_seg_list=self.usid_segment_list,
                    inner_frame=pkt1['IP'])
                send_packet(self, self.devports[2], sr_pkt)
                verify_packet(self, exp_pkt, self.devports[1])

                print("Verifying End.uN function with SL>1 and last uSID in use")
                sr_pkt = simple_ipv6_sr_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src=self.underlay_neighbor_mac,
                    ipv6_src=self.my_lb_ip,
                    ipv6_dst=self.in_uN_local_usid,
                    ipv6_hlim=64,
                    srh_seg_left=len(self.usid_segment_list),       # 2
                    srh_first_seg=len(self.usid_segment_list) - 1,  # 1
                    srh_nh=0x4,
                    srh_seg_list=self.usid_segment_list,
                    inner_frame=pkt1['IP'])
                exp_pkt = simple_ipv6_sr_packet(
                    eth_src='00:77:66:55:44:33',
                    eth_dst=self.underlay_neighbor_mac,
                    ipv6_src=self.my_lb_ip,
                    ipv6_dst=self.next_usid1,
                    ipv6_hlim=63,
                    srh_seg_left=len(self.usid_segment_list) - 1,   # 1
                    srh_first_seg=len(self.usid_segment_list) - 1,  # 1
                    srh_nh=0x4,
                    srh_seg_list=self.usid_segment_list,
                    inner_frame=pkt1['IP'])
                send_packet(self, self.devports[2], sr_pkt)
                verify_packet(self, exp_pkt, self.devports[1])

        finally:
            pass

    def EndUNPSPTest(self):
        print("EndUNPSPTest()")
        try:
            if not self.client.is_feature_enable(SWITCH_FEATURE_SRV6):
                print("SRv6 not enabled, skipping")
            else:
                print("Verifying End.uN function with SL=1 and >1 uSIDs left")
                pkt1 = simple_tcp_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src=self.underlay_neighbor_mac,
                    ip_dst=self.custA_ip4,
                    ip_src=self.custB_ip4,
                    ip_id=108,
                    ip_ttl=63)
                sr_pkt = simple_ipv6_sr_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src=self.underlay_neighbor_mac,
                    ipv6_src=self.my_lb_ip,
                    ipv6_dst=self.in_uN_usid,
                    ipv6_hlim=63,
                    srh_seg_left=len(self.usid_segment_list) - 1,   # 1
                    srh_first_seg=len(self.usid_segment_list) - 1,  # 1
                    srh_nh=0x4,
                    srh_seg_list=self.usid_segment_list,
                    inner_frame=pkt1['IP'])
                exp_pkt = simple_ipv6_sr_packet(
                    eth_src='00:77:66:55:44:33',
                    eth_dst=self.underlay_neighbor_mac,
                    ipv6_src=self.my_lb_ip,
                    ipv6_dst=self.out_uN_usid,
                    ipv6_hlim=62,
                    srh_seg_left=len(self.usid_segment_list) - 1,   # 1
                    srh_first_seg=len(self.usid_segment_list) - 1,  # 1
                    srh_nh=0x4,
                    srh_seg_list=self.usid_segment_list,
                    inner_frame=pkt1['IP'])
                send_packet(self, self.devports[2], sr_pkt)
                verify_packet(self, exp_pkt, self.devports[1])

                print("Verifying End.uN function with SL=1 and last uSID in use")
                sr_pkt = simple_ipv6_sr_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src=self.underlay_neighbor_mac,
                    ipv6_src=self.my_lb_ip,
                    ipv6_dst=self.in_uN_local_usid,
                    ipv6_hlim=63,
                    srh_seg_left=len(self.usid_segment_list) - 1,   # 1
                    srh_first_seg=len(self.usid_segment_list) - 1,  # 1
                    srh_nh=0x4,
                    srh_seg_list=self.usid_segment_list,
                    inner_frame=pkt1['IP'])
                ipip_pkt = simple_ipv6ip_packet(
                    eth_src='00:77:66:55:44:33',
                    eth_dst=self.underlay_neighbor_mac,
                    ipv6_src=self.my_lb_ip,
                    ipv6_dst=self.usid_segment_list[0],
                    ipv6_hlim=62,
                    inner_frame=pkt1['IP'])
                send_packet(self, self.devports[2], sr_pkt)
                verify_packet(self, ipip_pkt, self.devports[1])

        finally:
            pass

    def EndUNUSDTest(self):
        print("EndUNUSDTest()")
        try:
            if not self.client.is_feature_enable(SWITCH_FEATURE_SRV6):
                print("SRv6 not enabled, skipping")
            else:
                print("Verifying End.uN function with SL=0 and >1 uSIDs left")
                pkt1 = simple_tcp_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src=self.underlay_neighbor_mac,
                    ip_dst=self.custA_ip4,
                    ip_src=self.custB_ip4,
                    ip_id=108,
                    ip_ttl=63)
                sr_pkt = simple_ipv6_sr_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src=self.underlay_neighbor_mac,
                    ipv6_src=self.my_lb_ip,
                    ipv6_dst=self.in_uN_usid,
                    ipv6_hlim=62,
                    srh_seg_left=len(self.usid_segment_list) - 2,   # 0
                    srh_first_seg=len(self.usid_segment_list) - 1,  # 1
                    srh_nh=0x4,
                    srh_seg_list=self.usid_segment_list,
                    inner_frame=pkt1['IP'])
                exp_pkt = simple_ipv6_sr_packet(
                    eth_src='00:77:66:55:44:33',
                    eth_dst=self.underlay_neighbor_mac,
                    ipv6_src=self.my_lb_ip,
                    ipv6_dst=self.out_uN_usid,
                    ipv6_hlim=61,
                    srh_seg_left=len(self.usid_segment_list) - 2,   # 0
                    srh_first_seg=len(self.usid_segment_list) - 1,  # 1
                    srh_nh=0x4,
                    srh_seg_list=self.usid_segment_list,
                    inner_frame=pkt1['IP'])
                send_packet(self, self.devports[2], sr_pkt)
                verify_packet(self, exp_pkt, self.devports[1])

                print("Verifying End.uN function with SL=0 and last uSID in use")
                sr_pkt = simple_ipv6_sr_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src=self.underlay_neighbor_mac,
                    ipv6_src=self.my_lb_ip,
                    ipv6_dst=self.in_uN_local_usid,
                    ipv6_hlim=62,
                    srh_seg_left=len(self.usid_segment_list) - 2,   # 0
                    srh_first_seg=len(self.usid_segment_list) - 1,  # 1
                    srh_nh=0x4,
                    srh_seg_list=self.usid_segment_list,
                    inner_frame=pkt1['IP'])
                pkt2 = simple_tcp_packet(
                    eth_dst=self.underlay_neighbor_mac,
                    eth_src='00:77:66:55:44:33',
                    ip_dst=self.custA_ip4,
                    ip_src=self.custB_ip4,
                    ip_id=108,
                    ip_ttl=62)
                send_packet(self, self.devports[2], sr_pkt)
                verify_packet(self, pkt2, self.devports[1])

        finally:
            pass

    def EndUNUSDNoSRHTest(self):
        print("EndUNUSDNoSRHTest()")
        try:
            if not self.client.is_feature_enable(SWITCH_FEATURE_SRV6):
                print("SRv6 not enabled, skipping")
            else:
                print("Verifying End.uN function with SL=0 and >1 uSIDs left")
                pkt1 = simple_tcp_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src=self.underlay_neighbor_mac,
                    ip_dst=self.custA_ip4,
                    ip_src=self.custB_ip4,
                    ip_id=108,
                    ip_ttl=63)
                sr_pkt = simple_ipv6ip_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src=self.underlay_neighbor_mac,
                    ipv6_src=self.my_lb_ip,
                    ipv6_dst=self.in_uN_usid,
                    ipv6_hlim=62,
                    inner_frame=pkt1['IP'])
                exp_pkt = simple_ipv6ip_packet(
                    eth_src='00:77:66:55:44:33',
                    eth_dst=self.underlay_neighbor_mac,
                    ipv6_src=self.my_lb_ip,
                    ipv6_dst=self.out_uN_usid,
                    ipv6_hlim=61,
                    inner_frame=pkt1['IP'])
                send_packet(self, self.devports[2], sr_pkt)
                verify_packet(self, exp_pkt, self.devports[1])

                print("Verifying End.uN function with SL=0 and last uSID in use")
                sr_pkt = simple_ipv6ip_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src=self.underlay_neighbor_mac,
                    ipv6_src=self.my_lb_ip,
                    ipv6_dst=self.in_uN_local_usid,
                    ipv6_hlim=62,
                    inner_frame=pkt1['IP'])
                pkt2 = simple_tcp_packet(
                    eth_dst=self.underlay_neighbor_mac,
                    eth_src='00:77:66:55:44:33',
                    ip_dst=self.custA_ip4,
                    ip_src=self.custB_ip4,
                    ip_id=108,
                    ip_ttl=62)
                send_packet(self, self.devports[2], sr_pkt)
                verify_packet(self, pkt2, self.devports[1])

        finally:
            pass

    def EndUATest(self):
        print("EndUATest()")
        try:
            if not self.client.is_feature_enable(SWITCH_FEATURE_SRV6):
                print("SRv6 not enabled, skipping")
            else:
                print("Verifying End.uA function with SL>1 and >1 uSIDs left")
                pkt1 = simple_tcp_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src=self.underlay_neighbor_mac,
                    ip_src=self.custB_ip4,
                    ip_dst=self.custA_ip4,
                    ip_id=108,
                    ip_ttl=63)
                sr_pkt = simple_ipv6_sr_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src=self.underlay_neighbor_mac,
                    ipv6_src=self.my_lb_ip,
                    ipv6_dst=self.in_uA_usid,
                    ipv6_hlim=64,
                    srh_seg_left=len(self.usid_segment_list),       # 2
                    srh_first_seg=len(self.usid_segment_list) - 1,  # 1
                    srh_nh=0x4,
                    srh_seg_list=self.usid_segment_list,
                    inner_frame=pkt1['IP'])
                exp_pkt = simple_ipv6_sr_packet(
                    eth_src='00:77:66:55:44:33',
                    eth_dst=self.underlay_xconnect_nbor_mac,
                    ipv6_src=self.my_lb_ip,
                    ipv6_dst=self.out_uA_usid,
                    ipv6_hlim=63,
                    srh_seg_left=len(self.usid_segment_list),       # 2
                    srh_first_seg=len(self.usid_segment_list) - 1,  # 1
                    srh_nh=0x4,
                    srh_seg_list=self.usid_segment_list,
                    inner_frame=pkt1['IP'])
                send_packet(self, self.devports[2], sr_pkt)
                verify_packet(self, exp_pkt, self.devports[3])

                print("Verifying End.uA function with SL>1 and last uSID in use")
                sr_pkt = simple_ipv6_sr_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src=self.underlay_neighbor_mac,
                    ipv6_src=self.my_lb_ip,
                    ipv6_dst=self.in_uA_local_usid,
                    ipv6_hlim=64,
                    srh_seg_left=len(self.usid_segment_list),       # 2
                    srh_first_seg=len(self.usid_segment_list) - 1,  # 1
                    srh_nh=0x4,
                    srh_seg_list=self.usid_segment_list,
                    inner_frame=pkt1['IP'])
                exp_pkt = simple_ipv6_sr_packet(
                    eth_src='00:77:66:55:44:33',
                    eth_dst=self.underlay_xconnect_nbor_mac,
                    ipv6_src=self.my_lb_ip,
                    ipv6_dst=self.next_usid1,
                    ipv6_hlim=63,
                    srh_seg_left=len(self.usid_segment_list) - 1,   # 1
                    srh_first_seg=len(self.usid_segment_list) - 1,  # 1
                    srh_nh=0x4,
                    srh_seg_list=self.usid_segment_list,
                    inner_frame=pkt1['IP'])
                send_packet(self, self.devports[2], sr_pkt)
                verify_packet(self, exp_pkt, self.devports[3])

        finally:
            pass

    def EndUAPSPTest(self):
        print("EndUAPSPTest()")
        try:
            if not self.client.is_feature_enable(SWITCH_FEATURE_SRV6):
                print("SRv6 not enabled, skipping")
            else:
                print("Verifying End.uA function with SL=1 and >1 uSIDs left")
                pkt1 = simple_tcp_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src=self.underlay_neighbor_mac,
                    ip_dst=self.custA_ip4,
                    ip_src=self.custB_ip4,
                    ip_id=108,
                    ip_ttl=63)
                sr_pkt = simple_ipv6_sr_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src=self.underlay_neighbor_mac,
                    ipv6_src=self.my_lb_ip,
                    ipv6_dst=self.in_uA_usid,
                    ipv6_hlim=63,
                    srh_seg_left=len(self.usid_segment_list) - 1,   # 1
                    srh_first_seg=len(self.usid_segment_list) - 1,  # 1
                    srh_nh=0x4,
                    srh_seg_list=self.usid_segment_list,
                    inner_frame=pkt1['IP'])
                exp_pkt = simple_ipv6_sr_packet(
                    eth_src='00:77:66:55:44:33',
                    eth_dst=self.underlay_xconnect_nbor_mac,
                    ipv6_src=self.my_lb_ip,
                    ipv6_dst=self.out_uA_usid,
                    ipv6_hlim=62,
                    srh_seg_left=len(self.usid_segment_list) - 1,   # 1
                    srh_first_seg=len(self.usid_segment_list) - 1,  # 1
                    srh_nh=0x4,
                    srh_seg_list=self.usid_segment_list,
                    inner_frame=pkt1['IP'])
                send_packet(self, self.devports[2], sr_pkt)
                verify_packet(self, exp_pkt, self.devports[3])

                print("Verifying End.uA function with SL=1 and last uSID in use")
                sr_pkt = simple_ipv6_sr_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src=self.underlay_neighbor_mac,
                    ipv6_src=self.my_lb_ip,
                    ipv6_dst=self.in_uA_local_usid,
                    ipv6_hlim=63,
                    srh_seg_left=len(self.usid_segment_list) - 1,   # 1
                    srh_first_seg=len(self.usid_segment_list) - 1,  # 1
                    srh_nh=0x4,
                    srh_seg_list=self.usid_segment_list,
                    inner_frame=pkt1['IP'])
                ipip_pkt = simple_ipv6ip_packet(
                    eth_src='00:77:66:55:44:33',
                    eth_dst=self.underlay_xconnect_nbor_mac,
                    ipv6_src=self.my_lb_ip,
                    ipv6_dst=self.usid_segment_list[0],
                    ipv6_hlim=62,
                    inner_frame=pkt1['IP'])
                send_packet(self, self.devports[2], sr_pkt)
                verify_packet(self, ipip_pkt, self.devports[3])

        finally:
            pass

    def EndUAUSDTest(self):
        print("EndUAUSDTest()")
        try:
            if not self.client.is_feature_enable(SWITCH_FEATURE_SRV6):
                print("SRv6 not enabled, skipping")
            else:
                print("Verifying End.uA function with SL=0 and >1 uSIDs left")
                pkt1 = simple_tcp_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src=self.underlay_neighbor_mac,
                    ip_dst=self.custA_ip4,
                    ip_src=self.custB_ip4,
                    ip_id=108,
                    ip_ttl=63)
                sr_pkt = simple_ipv6_sr_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src=self.underlay_neighbor_mac,
                    ipv6_src=self.my_lb_ip,
                    ipv6_dst=self.in_uA_usid,
                    ipv6_hlim=62,
                    srh_seg_left=len(self.usid_segment_list) - 2,   # 0
                    srh_first_seg=len(self.usid_segment_list) - 1,  # 1
                    srh_nh=0x4,
                    srh_seg_list=self.usid_segment_list,
                    inner_frame=pkt1['IP'])
                exp_pkt = simple_ipv6_sr_packet(
                    eth_src='00:77:66:55:44:33',
                    eth_dst=self.underlay_xconnect_nbor_mac,
                    ipv6_src=self.my_lb_ip,
                    ipv6_dst=self.out_uA_usid,
                    ipv6_hlim=61,
                    srh_seg_left=len(self.usid_segment_list) - 2,   # 0
                    srh_first_seg=len(self.usid_segment_list) - 1,  # 1
                    srh_nh=0x4,
                    srh_seg_list=self.usid_segment_list,
                    inner_frame=pkt1['IP'])
                send_packet(self, self.devports[2], sr_pkt)
                verify_packet(self, exp_pkt, self.devports[3])

                print("Verifying End.uA function with SL=0 and last uSID in use")
                sr_pkt = simple_ipv6_sr_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src=self.underlay_neighbor_mac,
                    ipv6_src=self.my_lb_ip,
                    ipv6_dst=self.in_uA_local_usid,
                    ipv6_hlim=62,
                    srh_seg_left=len(self.usid_segment_list) - 2,   # 0
                    srh_first_seg=len(self.usid_segment_list) - 1,  # 1
                    srh_nh=0x4,
                    srh_seg_list=self.usid_segment_list,
                    inner_frame=pkt1['IP'])
                pkt2 = simple_tcp_packet(
                    eth_dst=self.underlay_xconnect_nbor_mac,
                    eth_src='00:77:66:55:44:33',
                    ip_dst=self.custA_ip4,
                    ip_src=self.custB_ip4,
                    ip_id=108,
                    ip_ttl=62)
                send_packet(self, self.devports[2], sr_pkt)
                verify_packet(self, pkt2, self.devports[3])

        finally:
            pass

    def EndUAUSDNoSRHTest(self):
        print("EndUAUSDNoSRHTest()")
        try:
            if not self.client.is_feature_enable(SWITCH_FEATURE_SRV6):
                print("SRv6 not enabled, skipping")
            else:
                print("Verifying End.uA function with SL=0 and >1 uSIDs left")
                pkt1 = simple_tcp_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src=self.underlay_neighbor_mac,
                    ip_dst=self.custA_ip4,
                    ip_src=self.custB_ip4,
                    ip_id=108,
                    ip_ttl=63)
                sr_pkt = simple_ipv6ip_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src=self.underlay_neighbor_mac,
                    ipv6_src=self.my_lb_ip,
                    ipv6_dst=self.in_uA_usid,
                    ipv6_hlim=62,
                    inner_frame=pkt1['IP'])
                exp_pkt = simple_ipv6ip_packet(
                    eth_src='00:77:66:55:44:33',
                    eth_dst=self.underlay_xconnect_nbor_mac,
                    ipv6_src=self.my_lb_ip,
                    ipv6_dst=self.out_uA_usid,
                    ipv6_hlim=61,
                    inner_frame=pkt1['IP'])
                send_packet(self, self.devports[2], sr_pkt)
                verify_packet(self, exp_pkt, self.devports[3])

                print("Verifying End.uA function with SL=0 and last uSID in use")
                sr_pkt = simple_ipv6ip_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src=self.underlay_neighbor_mac,
                    ipv6_src=self.my_lb_ip,
                    ipv6_dst=self.in_uA_local_usid,
                    ipv6_hlim=62,
                    inner_frame=pkt1['IP'])
                pkt2 = simple_tcp_packet(
                    eth_dst=self.underlay_xconnect_nbor_mac,
                    eth_src='00:77:66:55:44:33',
                    ip_dst=self.custA_ip4,
                    ip_src=self.custB_ip4,
                    ip_id=108,
                    ip_ttl=62)
                send_packet(self, self.devports[2], sr_pkt)
                verify_packet(self, pkt2, self.devports[3])

        finally:
            pass

###############################################################################
