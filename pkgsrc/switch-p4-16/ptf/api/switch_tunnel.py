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
from common.pktpy_utils import pktpy_skip

acl_table_bp_port = switcht_list_val_t(type=switcht_value_type.ENUM, enumdata=SWITCH_ACL_TABLE_ATTR_BIND_POINT_TYPE_PORT)
acl_group_bp_port = switcht_list_val_t(type=switcht_value_type.ENUM, enumdata=SWITCH_ACL_GROUP_ATTR_BIND_POINT_TYPE_PORT)


@group('tunnel')
class TunnelMalformedPacketsTest(ApiHelper):
    def setUp(self):
        self.configure()
        print()
        if (self.client.is_feature_enable(SWITCH_FEATURE_IPV4_TUNNEL) == 0):
            print("IPv4 tunnel feature not enabled, skipping")
            return
        if (self.client.is_feature_enable(SWITCH_FEATURE_VXLAN) == 0):
            print("VXLAN feature not enabled, skipping")
            return

        # underlay config input
        self.uvrf = self.add_vrf(self.device, id=100, src_mac=self.rmac)
        self.vm_ip = '100.100.1.1'
        self.vm_ip6 = '1234:5678:9abc:def0:4422:1133:5577:9911'
        self.customer_ip = '100.100.3.1'
        self.customer_ip6 = '1234:5678:9abc:def0:4422:1133:5577:0011'
        self.vni = 0x1234
        self.my_lb_ip = '1.1.1.3'
        self.tunnel_ip = '1.1.1.1'
        self.inner_dmac = "00:11:11:11:11:11"
        self.underlay_neighbor_mac = '00:33:33:33:33:33'

        # overlay configuration
        self.ovrf = self.add_vrf(self.device, id = 200, src_mac=self.rmac)
        self.tunnel_type = SWITCH_TUNNEL_ATTR_TYPE_VXLAN
        self.term_type = SWITCH_TUNNEL_TERM_ATTR_TERMINATION_TYPE_P2MP
        self.encap_ttl_mode = SWITCH_TUNNEL_ATTR_ENCAP_TTL_MODE_PIPE_MODEL
        self.decap_ttl_mode = SWITCH_TUNNEL_ATTR_DECAP_TTL_MODE_PIPE_MODEL

        # create underlay loopback rif for tunnel
        self.urif_lb = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_LOOPBACK, vrf_handle=self.uvrf, src_mac=self.rmac)

        self.urif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port1, vrf_handle=self.uvrf, src_mac=self.rmac)
        self.orif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port0, vrf_handle=self.ovrf, src_mac=self.rmac)

        # Configure MTUs for the RIFs
        tcpv4_pkt = simple_tcp_packet()
        tcpv6_pkt = simple_tcpv6_packet()
        self.v4mtu_offset = len(simple_vxlan_packet(inner_frame=tcpv4_pkt)) - len(tcpv4_pkt)
        self.v6mtu_offset = len(simple_vxlan_packet(inner_frame=tcpv6_pkt)) - len(tcpv6_pkt)
        self.overlay_v4_mtu = 200
        self.overlay_v6_mtu = 500
        self.underlay_v4_mtu = self.overlay_v4_mtu + self.v4mtu_offset - 1
        self.underlay_v6_mtu = self.overlay_v6_mtu + self.v6mtu_offset - 1

        self.attribute_set(self.orif, SWITCH_RIF_ATTR_MTU, self.overlay_v4_mtu)
        self.attribute_set(self.urif, SWITCH_RIF_ATTR_MTU, self.underlay_v4_mtu)

        # create Encap/Decap mappers
        self.encap_tunnel_map = self.add_tunnel_mapper(self.device,
            type=SWITCH_TUNNEL_MAPPER_ATTR_TYPE_VRF_HANDLE_TO_VNI)
        self.decap_tunnel_map = self.add_tunnel_mapper(self.device,
            type=SWITCH_TUNNEL_MAPPER_ATTR_TYPE_VNI_TO_VRF_HANDLE)

       # create Encap/Decap mapper entries for ovrf
        self.encap_tunnel_map_entry = self.add_tunnel_mapper_entry(self.device,
            type=SWITCH_TUNNEL_MAPPER_ENTRY_ATTR_TYPE_VRF_HANDLE_TO_VNI,
            tunnel_mapper_handle=self.encap_tunnel_map,
            network_handle=self.ovrf,
            tunnel_vni=self.vni)
        self.decap_tunnel_map_entry = self.add_tunnel_mapper_entry(self.device,
            type=SWITCH_TUNNEL_MAPPER_ENTRY_ATTR_TYPE_VNI_TO_VRF_HANDLE,
            tunnel_mapper_handle=self.decap_tunnel_map,
            network_handle=self.ovrf,
            tunnel_vni=self.vni)

        ingress_mapper_list = [switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=self.encap_tunnel_map)]
        egress_mapper_list = [switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=self.decap_tunnel_map)]

        # create Tunnel
        self.tunnel = self.add_tunnel(self.device,
            type=self.tunnel_type,
            src_ip=self.my_lb_ip,
            ingress_mapper_handles=ingress_mapper_list,
            egress_mapper_handles=egress_mapper_list,
            encap_ttl_mode=self.encap_ttl_mode,
            decap_ttl_mode=self.decap_ttl_mode,
            underlay_rif_handle=self.urif_lb)

        # Create tunnel_termination entry
        self.tunnel_term = self.add_tunnel_term(self.device,
            type=self.tunnel_type,
            vrf_handle=self.uvrf,
            tunnel_handle=self.tunnel,
            termination_type=self.term_type,
            dst_ip=self.my_lb_ip)

        # Create route to customer from VM
        self.onhop = self.add_nexthop(self.device, handle=self.orif, dest_ip='100.100.3.1')
        self.oneighbor = self.add_neighbor(self.device,
            mac_address='00:22:22:22:22:22',
            handle=self.orif,
            dest_ip=self.customer_ip)  # 100.100.3.1
        self.customer_route = self.add_route(self.device, ip_prefix=self.customer_ip,
            vrf_handle=self.ovrf, nexthop_handle=self.onhop)
        self.customev6_route = self.add_route(self.device, ip_prefix=self.customer_ip6,
            vrf_handle=self.ovrf, nexthop_handle=self.onhop)

        # Create tunnel nexthop for VM 100.100.1.1
        self.tunnel_nexthop = self.add_nexthop(self.device,
            type=SWITCH_NEXTHOP_ATTR_TYPE_TUNNEL,
            rw_type=SWITCH_NEXTHOP_ATTR_RW_TYPE_L3_VNI,
            handle=self.tunnel,
            dest_ip=self.tunnel_ip,  # 1.1.1.1
            mac_address=self.inner_dmac,
            tunnel_vni=self.vni)
        # Add routes for VM (v4 & v6 adress) via tunnel nexthop
        self.customerv4_route = self.add_route(self.device,
            ip_prefix=self.vm_ip,
            vrf_handle=self.ovrf,
            nexthop_handle=self.tunnel_nexthop)
        self.customerv6_route = self.add_route(self.device,
            ip_prefix=self.vm_ip6,
            vrf_handle=self.ovrf,
            nexthop_handle=self.tunnel_nexthop)

        # Create route to tunnel ip 1.1.1.1
        self.unhop = self.add_nexthop(self.device, handle=self.urif, dest_ip=self.tunnel_ip)
        self.uneighbor = self.add_neighbor(self.device,
            mac_address=self.underlay_neighbor_mac,
            handle=self.urif,
            dest_ip=self.tunnel_ip)
        self.tunnel_route = self.add_route(self.device, ip_prefix=self.tunnel_ip,
            vrf_handle=self.uvrf, nexthop_handle=self.unhop)

        self.num_drops = 0

    def runTest(self):
        try:
            if (self.client.is_feature_enable(SWITCH_FEATURE_IPV4_TUNNEL) == 0):
                return
            if (self.client.is_feature_enable(SWITCH_FEATURE_VXLAN) == 0):
                return
            self.BasicTest()
            self.L2IPv4Test()
            self.L3IPv4DecapTest()
            self.L3IPv4EncapTest()
            self.L3V6EncapTest()
            self.L3V6DecapTest()
        finally:
            pass

    def tearDown(self):
        self.cleanup()

    def BasicTest(self):
        try:
            #init_drop_stats = self.client.switch_api_drop_stats_get(device)
            print("Verifying Dataplane correctly set up before sending malformed packets")
            print("Valid v4 packet from Vxlan port 1 to Access port 0")
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src=self.inner_dmac,
                ip_dst=self.customer_ip,
                ip_src=self.vm_ip,
                ip_id=108,
                ip_ttl=64)
            pkt2 = simple_tcp_packet(
                eth_dst='00:22:22:22:22:22',
                eth_src='00:77:66:55:44:33',
                ip_dst=self.customer_ip,
                ip_src=self.vm_ip,
                ip_id=108,
                ip_ttl=63)
            vxlan_pkt = simple_vxlan_packet(
                eth_src=self.underlay_neighbor_mac,
                eth_dst='00:77:66:55:44:33',
                ip_id=0,
                ip_dst=self.my_lb_ip,
                ip_src=self.tunnel_ip,
                ip_ttl=64,
                ip_flags=0x2,
                udp_sport=11638,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=pkt)
            send_packet(self, self.devports[1], vxlan_pkt)
            verify_packet(self, pkt2, self.devports[0])

            if not self.client.is_feature_enable(SWITCH_FEATURE_TUNNEL_ENCAP):
                print(" Tunnel encap not enabled, skipping")
            else:
                print("Valid v4 packet from Access port 0 to VxLAN port 1")
                pkt = simple_tcp_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src='00:22:22:22:22:22',
                    ip_src=self.customer_ip,
                    ip_dst=self.vm_ip,
                    ip_id=108,
                    ip_ttl=64)
                pkt2 = simple_tcp_packet(
                    eth_dst=self.inner_dmac,
                    eth_src=self.default_rmac,
                    ip_dst=self.vm_ip,
                    ip_src=self.customer_ip,
                    ip_id=108,
                    ip_ttl=63)
                vxlan_pkt =  mask.Mask(simple_vxlan_packet(
                    eth_dst=self.underlay_neighbor_mac,
                    eth_src='00:77:66:55:44:33',
                    ip_id=0,
                    ip_src=self.my_lb_ip,
                    ip_dst=self.tunnel_ip,
                    ip_ttl=64,
                    ip_flags=0x2,
                    with_udp_chksum=False,
                    vxlan_vni=self.vni,
                    inner_frame=pkt2))
                mask_set_do_not_care_packet(vxlan_pkt, UDP, "sport")
                send_packet(self, self.devports[0], pkt)
                verify_packet(self, vxlan_pkt, self.devports[1])

            print("Valid v6 packet from Vxlan port 1 to Access port 0")
            pkt = simple_tcpv6_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src=self.inner_dmac,
                ipv6_dst=self.customer_ip6,
                ipv6_src=self.vm_ip6,
                ipv6_hlim=64)
            pkt2 = simple_tcpv6_packet(
                eth_dst='00:22:22:22:22:22',
                eth_src='00:77:66:55:44:33',
                ipv6_dst=self.customer_ip6,
                ipv6_src=self.vm_ip6,
                ipv6_hlim=63)
            vxlan_pkt = simple_vxlan_packet(
                eth_src=self.underlay_neighbor_mac,
                eth_dst='00:77:66:55:44:33',
                ip_id=0,
                ip_dst=self.my_lb_ip,
                ip_src=self.tunnel_ip,
                ip_ttl=64,
                ip_flags=0x2,
                udp_sport=11638,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=pkt)
            send_packet(self, self.devports[1], vxlan_pkt)
            verify_packet(self, pkt2, self.devports[0])

            if not self.client.is_feature_enable(SWITCH_FEATURE_TUNNEL_ENCAP):
                print(" Tunnel encap not enabled, skipping")
            else:
                print("Valid v6 packet from Access port 0 to VxLAN port 1")
                pkt = simple_tcpv6_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src='00:22:22:22:22:22',
                    ipv6_dst=self.vm_ip6,
                    ipv6_src=self.customer_ip6,
                    ipv6_hlim=64)
                pkt2 = simple_tcpv6_packet(
                    eth_dst=self.inner_dmac,
                    eth_src=self.default_rmac,
                    ipv6_dst=self.vm_ip6,
                    ipv6_src=self.customer_ip6,
                    ipv6_hlim=63)
                vxlan_pkt =  mask.Mask(simple_vxlan_packet(
                    eth_dst=self.underlay_neighbor_mac,
                    eth_src='00:77:66:55:44:33',
                    ip_id=0,
                    ip_src=self.my_lb_ip,
                    ip_dst=self.tunnel_ip,
                    ip_ttl=64,
                    ip_flags=0x2,
                    with_udp_chksum=False,
                    vxlan_vni=self.vni,
                    inner_frame=pkt2))
                mask_set_do_not_care_packet(vxlan_pkt, UDP, "sport")
                send_packet(self, self.devports[0], pkt)
                verify_packet(self, vxlan_pkt, self.devports[1])
        finally:
            pass

    def L2IPv4Test(self):
        try:
            print("Inner MAC DA zeros, drop")
            pkt = simple_tcp_packet(
                eth_dst='00:00:00:00:00:00',
                eth_src='00:22:22:22:22:22',
                ip_dst=self.customer_ip,
                ip_id=108,
                ip_ttl=64)
            vxlan_pkt = simple_vxlan_packet(
                eth_src='00:33:33:33:33:33',
                eth_dst='00:77:66:55:44:33',
                ip_id=0,
                ip_dst='1.1.1.3',
                ip_src='1.1.1.1',
                ip_ttl=64,
                ip_flags=0x2,
                udp_sport=11638,
                with_udp_chksum=False,
                vxlan_vni=0x1234,
                inner_frame=pkt)
            send_packet(self, self.devports[1], vxlan_pkt)
            verify_no_other_packets(self, timeout=1)
            self.num_drops += 1

            # Inner MAC SA validation not supported
            '''
            print("Inner MAC SA zeros, drop")
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:00:00:00:00:00',
                ip_dst=self.customer_ip,
                ip_src=self.vm_ip,
                ip_id=108,
                ip_ttl=64)
            vxlan_pkt = simple_vxlan_packet(
                eth_src='00:33:33:33:33:33',
                eth_dst='00:77:66:55:44:33',
                ip_id=0,
                ip_dst='1.1.1.3',
                ip_src='1.1.1.1',
                ip_ttl=64,
                ip_flags=0x2,
                udp_sport=11638,
                with_udp_chksum=False,
                vxlan_vni=0x1234,
                inner_frame=pkt)
            send_packet(self, self.devports[1], vxlan_pkt)
            verify_no_other_packets(self, timeout=1)
            self.num_drops += 1

            print("Inner MAC SA broadcast, drop")
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='ff:ff:ff:ff:ff:ff',
                ip_dst=self.customer_ip,
                ip_src=self.vm_ip,
                ip_id=108,
                ip_ttl=64)
            vxlan_pkt = simple_vxlan_packet(
                eth_src='00:33:33:33:33:33',
                eth_dst='00:77:66:55:44:33',
                ip_id=0,
                ip_dst='1.1.1.3',
                ip_src='1.1.1.1',
                ip_ttl=64,
                ip_flags=0x2,
                udp_sport=11638,
                with_udp_chksum=False,
                vxlan_vni=0x1234,
                inner_frame=pkt)
            send_packet(self, self.devports[1], vxlan_pkt)
            verify_no_other_packets(self, timeout=1)
            self.num_drops += 1

            print("Inner MAC SA multicast, drop")
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='01:00:5e:00:00:05',
                ip_dst=self.customer_ip,
                ip_src=self.vm_ip,
                ip_id=108,
                ip_ttl=64)
            vxlan_pkt = simple_vxlan_packet(
                eth_src='00:33:33:33:33:33',
                eth_dst='00:77:66:55:44:33',
                ip_id=0,
            ip_dst='1.1.1.3',
            ip_src='1.1.1.1',
            ip_ttl=64,
            udp_sport=11638,
            with_udp_chksum=False,
            vxlan_vni=0x1234,
            inner_frame=pkt)
            send_packet(self, self.devports[1], vxlan_pkt)
            verify_no_other_packets(self, timeout=1)
            self.num_drops += 1
            '''
        finally:
            pass

    def L3IPv4EncapTest(self):
        try:
            print("Encap Malformed packet Tests")
            print("IHL 0, drop")
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_src=self.customer_ip,
                ip_dst=self.vm_ip,
                ip_id=108,
                ip_ihl=0)
            send_packet(self, self.devports[0], pkt)
            verify_no_other_packets(self, timeout=1)
            self.num_drops += 1

            print("IHL 1, drop")
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_src=self.customer_ip,
                ip_dst=self.vm_ip,
                ip_id=108,
                ip_ihl=1)
            send_packet(self, self.devports[0], pkt)
            verify_no_other_packets(self, timeout=1)
            self.num_drops += 1

            print("IHL 2, drop")
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_src=self.customer_ip,
                ip_dst=self.vm_ip,
                ip_id=108,
                ip_ihl=2)
            send_packet(self, self.devports[0], pkt)
            verify_no_other_packets(self, timeout=1)
            self.num_drops += 1

            print("IHL 3, drop")
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_src=self.customer_ip,
                ip_dst=self.vm_ip,
                ip_id=108,
                ip_ihl=3)
            send_packet(self, self.devports[0], pkt)
            verify_no_other_packets(self, timeout=1)
            self.num_drops += 1

            print("IHL 4, drop")
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_src=self.customer_ip,
                ip_dst=self.vm_ip,
                ip_id=108,
                ip_ihl=4)
            send_packet(self, self.devports[0], pkt)
            verify_no_other_packets(self, timeout=1)
            self.num_drops += 1

            print("IPv4 TTL 0, drop")
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_src=self.customer_ip,
                ip_dst=self.vm_ip,
                ip_id=108,
                ip_ttl=0)
            send_packet(self, self.devports[0], pkt)
            verify_no_other_packets(self, timeout=1)
            self.num_drops += 1

            print("IPv4 invalid version, drop")
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_src=self.customer_ip,
                ip_dst=self.vm_ip,
                ip_id=108,
                ip_ttl=64)
            pkt[IP].version = 6
            send_packet(self, self.devports[0], pkt)
            verify_no_other_packets(self, timeout=1)
            self.num_drops += 1

            print("IPv4 src is multicast, drop")
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_src=self.customer_ip,
                ip_dst='225.1.1.1',
                ip_id=108,
                ip_ttl=64)
            send_packet(self, self.devports[0], pkt)
            verify_no_other_packets(self, timeout=1)
            self.num_drops += 1

            print("IPv4 src is loopback, drop")
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_src=self.customer_ip,
                ip_dst='127.1.1.1',
                ip_id=108,
                ip_ttl=64)
            send_packet(self, self.devports[0], pkt)
            verify_no_other_packets(self, timeout=1)
            self.num_drops += 1

            # overlay_pkt_size <  overlay_mtu < underlay_pkt_size < underlay_mtu
            pkt = simple_tcp_packet(
                pktlen=self.overlay_v4_mtu-2 + 14,
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_src=self.customer_ip,
                ip_dst=self.vm_ip,
                ip_id=108)
            pkt2 = simple_tcp_packet(
                pktlen=self.overlay_v4_mtu-2 + 14,
                eth_dst=self.inner_dmac,
                eth_src=self.default_rmac,
                ip_dst=self.vm_ip,
                ip_src=self.customer_ip,
                ip_id=108,
                ip_ttl=63)
            vxlan_pkt =  mask.Mask(simple_vxlan_packet(
                eth_dst=self.underlay_neighbor_mac,
                eth_src='00:77:66:55:44:33',
                ip_id=0,
                ip_src=self.my_lb_ip,
                ip_dst=self.tunnel_ip,
                ip_ttl=64,
                ip_flags=0x2,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=pkt2))
            mask_set_do_not_care_packet(vxlan_pkt, UDP, "sport")
            print((("Overlay MTU: {} Underlay MTU: {}, Overlay Paket Size: {},"
                  "Expected Underlay Packet Size {}").format(self.overlay_v4_mtu,
                  self.underlay_v4_mtu, len(pkt)-14, len(vxlan_pkt.exp_pkt)-14)))
            print("Sending packet from Access port 0 to VxLAN Access port 1")
            if not self.client.is_feature_enable(SWITCH_FEATURE_TUNNEL_ENCAP):
                print(" Tunnel encap not enabled, skipping")
            else:
                send_packet(self, self.devports[0], pkt)
                verify_packet(self, vxlan_pkt, self.devports[1])

            # overlay_pkt_size <  overlay_mtu < underlay_pkt_size = underlay_mtu
            pkt = simple_tcp_packet(
                pktlen=self.overlay_v4_mtu-1 + 14,
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_src=self.customer_ip,
                ip_dst=self.vm_ip,
                ip_id=108)
            pkt2 = simple_tcp_packet(
                pktlen=self.overlay_v4_mtu-1 + 14,
                eth_dst=self.inner_dmac,
                eth_src=self.default_rmac,
                ip_dst=self.vm_ip,
                ip_src=self.customer_ip,
                ip_id=108,
                ip_ttl=63)
            vxlan_pkt =  mask.Mask(simple_vxlan_packet(
                eth_dst=self.underlay_neighbor_mac,
                eth_src='00:77:66:55:44:33',
                ip_id=0,
                ip_src=self.my_lb_ip,
                ip_dst=self.tunnel_ip,
                ip_ttl=64,
                ip_flags=0x2,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=pkt2))
            mask_set_do_not_care_packet(vxlan_pkt, UDP, "sport")
            print((("Overlay MTU: {} Underlay MTU: {}, Overlay Paket Size: {},"
                  "Expected Underlay Packet Size {}").format(self.overlay_v4_mtu,
                  self.underlay_v4_mtu, len(pkt)-14, len(vxlan_pkt.exp_pkt)-14)))
            print("Sending packet from Access port 0 to VxLAN Access port 1")
            if not self.client.is_feature_enable(SWITCH_FEATURE_TUNNEL_ENCAP):
                print(" Tunnel encap not enabled, skipping")
            else:
                send_packet(self, self.devports[0], pkt)
                verify_packet(self, vxlan_pkt, self.devports[1])

            # overlay_pkt_size <  overlay_mtu < underlay_mtu < underlay_pkt_size
            pkt = simple_tcp_packet(
                pktlen=self.overlay_v4_mtu + 14,
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_src=self.customer_ip,
                ip_dst=self.vm_ip,
                ip_id=108)
            pkt2 = simple_tcp_packet(
                pktlen=self.overlay_v4_mtu + 14,
                eth_dst=self.inner_dmac,
                eth_src=self.default_rmac,
                ip_dst=self.vm_ip,
                ip_src=self.customer_ip,
                ip_id=108,
                ip_ttl=63)
            vxlan_pkt =  mask.Mask(simple_vxlan_packet(
                eth_dst=self.underlay_neighbor_mac,
                eth_src='00:77:66:55:44:33',
                ip_id=0,
                ip_src=self.my_lb_ip,
                ip_dst=self.tunnel_ip,
                ip_ttl=64,
                ip_flags=0x2,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=pkt2))
            mask_set_do_not_care_packet(vxlan_pkt, UDP, "sport")
            print((("Overlay MTU: {} Underlay MTU: {}, Overlay Paket Size: {},"
                  "Expected Underlay Packet Size {} ---> MTU DROP").format(self.overlay_v4_mtu,
                  self.underlay_v4_mtu, len(pkt)-14, len(vxlan_pkt.exp_pkt)-14)))
            print("Sending packet from Access port 0 to VxLAN Access port 1")
            send_packet(self, self.devports[0], pkt)
            verify_no_other_packets(self, timeout=1)
        finally:
            pass

    def L3IPv4DecapTest(self):
        try:
            print("Decap Malformed packet Tests")
            print("Inner DMAC not equal to RMAC")
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:34',
                eth_src=self.inner_dmac,
                ip_dst=self.customer_ip,
                ip_src=self.vm_ip,
                ip_id=108)
            vxlan_pkt = simple_vxlan_packet(
                eth_src='00:33:33:33:33:33',
                eth_dst='00:77:66:55:44:33',
                ip_id=0,
                ip_dst='1.1.1.3',
                ip_src='1.1.1.1',
                ip_ttl=64,
                ip_flags=0x2,
                udp_sport=11638,
                with_udp_chksum=False,
                vxlan_vni=0x1234,
                inner_frame=pkt)
            send_packet(self, self.devports[1], vxlan_pkt)
            verify_no_other_packets(self, timeout=1)
            self.num_drops += 1

            print("Inner IHL 0, drop")
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src=self.inner_dmac,
                ip_dst=self.customer_ip,
                ip_src=self.vm_ip,
                ip_id=108,
                ip_ihl=0)
            vxlan_pkt = simple_vxlan_packet(
                eth_src='00:33:33:33:33:33',
                eth_dst='00:77:66:55:44:33',
                ip_id=0,
                ip_dst='1.1.1.3',
                ip_src='1.1.1.1',
                ip_ttl=64,
                ip_flags=0x2,
                udp_sport=11638,
                with_udp_chksum=False,
                vxlan_vni=0x1234,
                inner_frame=pkt)
            send_packet(self, self.devports[1], vxlan_pkt)
            verify_no_other_packets(self, timeout=1)
            self.num_drops += 1

            print("Inner IHL 1, drop")
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src=self.inner_dmac,
                ip_dst=self.customer_ip,
                ip_src=self.vm_ip,
                ip_id=108,
                ip_ihl=1)
            vxlan_pkt = simple_vxlan_packet(
                eth_src='00:33:33:33:33:33',
                eth_dst='00:77:66:55:44:33',
                ip_id=0,
                ip_dst='1.1.1.3',
                ip_src='1.1.1.1',
                ip_ttl=64,
                ip_flags=0x2,
                udp_sport=11638,
                with_udp_chksum=False,
                vxlan_vni=0x1234,
                inner_frame=pkt)
            send_packet(self, self.devports[1], vxlan_pkt)
            verify_no_other_packets(self, timeout=1)
            self.num_drops += 1

            print("Inner IHL 2, drop")
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src=self.inner_dmac,
                ip_dst=self.customer_ip,
                ip_src=self.vm_ip,
                ip_id=108,
                ip_ihl=2)
            vxlan_pkt = simple_vxlan_packet(
                eth_src='00:33:33:33:33:33',
                eth_dst='00:77:66:55:44:33',
                ip_id=0,
                ip_dst='1.1.1.3',
                ip_src='1.1.1.1',
                ip_ttl=64,
                ip_flags=0x2,
                udp_sport=11638,
                with_udp_chksum=False,
                vxlan_vni=0x1234,
                inner_frame=pkt)
            send_packet(self, self.devports[1], vxlan_pkt)
            verify_no_other_packets(self, timeout=1)
            self.num_drops += 1

            print("Inner IHL 3, drop")
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src=self.inner_dmac,
                ip_dst=self.customer_ip,
                ip_src=self.vm_ip,
                ip_id=108,
                ip_ihl=3)
            vxlan_pkt = simple_vxlan_packet(
                eth_src='00:33:33:33:33:33',
                eth_dst='00:77:66:55:44:33',
                ip_id=0,
                ip_dst='1.1.1.3',
                ip_src='1.1.1.1',
                ip_ttl=64,
                ip_flags=0x2,
                udp_sport=11638,
                with_udp_chksum=False,
                vxlan_vni=0x1234,
                inner_frame=pkt)
            send_packet(self, self.devports[1], vxlan_pkt)
            verify_no_other_packets(self, timeout=1)
            self.num_drops += 1

            print("Inner IHL 4, drop")
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src=self.inner_dmac,
                ip_dst=self.customer_ip,
                ip_src=self.vm_ip,
                ip_id=108,
                ip_ihl=4)
            vxlan_pkt = simple_vxlan_packet(
                eth_src='00:33:33:33:33:33',
                eth_dst='00:77:66:55:44:33',
                ip_id=0,
                ip_dst='1.1.1.3',
                ip_src='1.1.1.1',
                ip_ttl=64,
                ip_flags=0x2,
                udp_sport=11638,
                with_udp_chksum=False,
                vxlan_vni=0x1234,
                inner_frame=pkt)
            send_packet(self, self.devports[1], vxlan_pkt)
            verify_no_other_packets(self, timeout=1)
            self.num_drops += 1

            print("Inner IPv4 TTL 0, drop")
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src=self.inner_dmac,
                ip_dst=self.customer_ip,
                ip_src=self.vm_ip,
                ip_id=108,
                ip_ttl=0)
            vxlan_pkt = simple_vxlan_packet(
                eth_src='00:33:33:33:33:33',
                eth_dst='00:77:66:55:44:33',
                ip_id=0,
                ip_dst='1.1.1.3',
                ip_src='1.1.1.1',
                ip_ttl=64,
                ip_flags=0x2,
                udp_sport=11638,
                with_udp_chksum=False,
                vxlan_vni=0x1234,
                inner_frame=pkt)
            send_packet(self, self.devports[1], vxlan_pkt)
            verify_no_other_packets(self, timeout=1)
            self.num_drops += 1

            print("Inner IPv4 invalid version, drop")
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src=self.inner_dmac,
                ip_dst=self.customer_ip,
                ip_src=self.vm_ip,
                ip_id=108,
                ip_ttl=64)
            pkt[IP].version = 6
            vxlan_pkt = simple_vxlan_packet(
                eth_src='00:33:33:33:33:33',
                eth_dst='00:77:66:55:44:33',
                ip_id=0,
                ip_dst='1.1.1.3',
                ip_src='1.1.1.1',
                ip_ttl=64,
                ip_flags=0x2,
                udp_sport=11638,
                with_udp_chksum=False,
                vxlan_vni=0x1234,
                inner_frame=pkt)
            send_packet(self, self.devports[1], vxlan_pkt)
            verify_no_other_packets(self, timeout=1)
            self.num_drops += 1

            print("Outer IPv4 src is multicast, drop")
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src=self.inner_dmac,
                ip_dst=self.customer_ip,
                ip_src=self.vm_ip,
                ip_id=108,
                ip_ttl=64)
            vxlan_pkt = simple_vxlan_packet(
                eth_src='00:33:33:33:33:33',
                eth_dst='00:77:66:55:44:33',
                ip_id=0,
                ip_dst='1.1.1.3',
                ip_src='225.1.1.1',
                ip_ttl=64,
                ip_flags=0x2,
                udp_sport=11638,
                with_udp_chksum=False,
                vxlan_vni=0x1234,
                inner_frame=pkt)
            send_packet(self, self.devports[1], vxlan_pkt)
            verify_no_other_packets(self, timeout=1)
            self.num_drops += 1

            # Inner IPV4 SA validation is not supported
            '''
            print("IPv4 src is loopback, drop")
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src=self.inner_dmac,
                ip_dst=self.customer_ip,
                ip_src=self.vm_ip,
                ip_id=108,
                ip_ttl=64)
            vxlan_pkt = simple_vxlan_packet(
                eth_src='00:33:33:33:33:33',
                eth_dst='00:77:66:55:44:33',
                ip_id=0,
                ip_dst='1.1.1.3',
                ip_src='127.1.1.1',
                ip_ttl=64,
                ip_flags=0x2,
                udp_sport=11638,
                with_udp_chksum=False,
                vxlan_vni=0x1234,
                inner_frame=pkt)
            send_packet(self, self.devports[1], vxlan_pkt)
            verify_no_other_packets(self, timeout=1)
            self.num_drops += 1

            print("Inner IPv4 src is loopback, drop")
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src=self.inner_dmac,
                ip_dst=self.customer_ip,
                ip_src='127.10.10.1',
                ip_id=108,
                ip_ttl=64)
            vxlan_pkt = simple_vxlan_packet(
                eth_src='00:33:33:33:33:33',
                eth_dst='00:77:66:55:44:33',
                ip_id=0,
                ip_dst='1.1.1.3',
                ip_src='1.1.1.1',
                ip_ttl=64,
                ip_flags=0x2,
                udp_sport=11638,
                with_udp_chksum=False,
                vxlan_vni=0x1234,
                inner_frame=pkt)
            send_packet(self, self.devports[1], vxlan_pkt)
            verify_no_other_packets(self, timeout=1)
            self.num_drops += 1

            print("IPv4 src is loopback, drop")
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src=self.inner_dmac,
                ip_dst=self.customer_ip,
                ip_src=self.vm_ip,
                ip_id=108,
                ip_ttl=64)
            send_packet(self, self.devports[0], pkt)
            verify_no_other_packets(self, timeout=1)
            self.num_drops += 1
            '''
        finally:
            pass

    def L3V6EncapTest(self):
        try:
            print("v6 Encap Malformed packet Tests")
            print("IPv6 TTL 0, drop")
            pkt = simple_tcpv6_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ipv6_dst=self.vm_ip6,
                ipv6_src=self.customer_ip6,
                ipv6_hlim=0)
            send_packet(self, self.devports[0], pkt)
            verify_no_other_packets(self, timeout=1)
            self.num_drops += 1

            print("IPv6 invalid version, drop")
            pkt = simple_tcpv6_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ipv6_dst=self.vm_ip6,
                ipv6_src=self.customer_ip6,
                ipv6_hlim=64)
            pkt[IPv6].version = 4
            send_packet(self, self.devports[0], pkt)
            verify_no_other_packets(self, timeout=1)
            self.num_drops += 1

            print("IPv6 src multicast, drop")
            pkt = simple_tcpv6_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ipv6_dst=self.vm_ip6,
                ipv6_src='ff02::1',
                ipv6_hlim=64)
            send_packet(self, self.devports[0], pkt)
            verify_no_other_packets(self, timeout=1)
            self.num_drops += 1
        finally:
            pass

    def L3V6DecapTest(self):
        try:
            print("v6 Decap Malformed packet Tests")
            print("Inner IPv6 TTL 0, drop")
            pkt = simple_tcpv6_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src=self.inner_dmac,
                ipv6_dst=self.customer_ip6,
                ipv6_src=self.vm_ip6,
                ipv6_hlim=0)
            vxlan_pkt = simple_vxlan_packet(
                eth_src=self.underlay_neighbor_mac,
                eth_dst='00:77:66:55:44:33',
                ip_id=0,
                ip_dst=self.my_lb_ip,
                ip_src=self.tunnel_ip,
                ip_ttl=64,
                ip_flags=0x2,
                udp_sport=11638,
                with_udp_chksum=False,
                vxlan_vni=0x1234,
                inner_frame=pkt)
            send_packet(self, self.devports[1], vxlan_pkt)
            verify_no_other_packets(self, timeout=1)
            self.num_drops += 1

            print("Inner IPv6 invalid version, drop")
            pkt = simple_tcpv6_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src=self.inner_dmac,
                ipv6_dst=self.customer_ip6,
                ipv6_src=self.vm_ip6,
                ipv6_hlim=64)
            pkt[IPv6].version = 4
            vxlan_pkt = simple_vxlan_packet(
                eth_src=self.underlay_neighbor_mac,
                eth_dst='00:77:66:55:44:33',
                ip_id=0,
                ip_dst=self.my_lb_ip,
                ip_src=self.tunnel_ip,
                ip_ttl=64,
                ip_flags=0x2,
                udp_sport=11638,
                with_udp_chksum=False,
                vxlan_vni=0x1234,
                inner_frame=pkt)
            send_packet(self, self.devports[1], vxlan_pkt)
            verify_no_other_packets(self, timeout=1)
            self.num_drops += 1

            print("Inner IPv6 src multicast, drop (not supported skipping)")
            pkt = simple_tcpv6_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src=self.inner_dmac,
                ipv6_dst=self.customer_ip6,
                ipv6_src='ff02::1',
                ipv6_hlim=64)
            vxlan_pkt = simple_vxlan_packet(
                eth_src=self.underlay_neighbor_mac,
                eth_dst='00:77:66:55:44:33',
                ip_id=0,
                ip_dst=self.my_lb_ip,
                ip_src=self.tunnel_ip,
                ip_ttl=64,
                ip_flags=0x2,
                udp_sport=11638,
                with_udp_chksum=False,
                vxlan_vni=0x1234,
                inner_frame=pkt)
            #send_packet(self, self.devports[1], vxlan_pkt)
            #verify_no_other_packets(self, timeout=1)
            #self.num_drops += 1

            print("Inner IPv6 dst is loopback, drop")
            pkt = simple_tcpv6_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src=self.inner_dmac,
                ipv6_dst='::1',
                ipv6_src=self.vm_ip6,
                ipv6_hlim=64)
            vxlan_pkt = simple_vxlan_packet(
                eth_src=self.underlay_neighbor_mac,
                eth_dst='00:77:66:55:44:33',
                ip_id=0,
                ip_dst=self.my_lb_ip,
                ip_src=self.tunnel_ip,
                ip_ttl=64,
                ip_flags=0x2,
                udp_sport=11638,
                with_udp_chksum=False,
                vxlan_vni=0x1234,
                inner_frame=pkt)
            send_packet(self, self.devports[1], pkt)
            verify_no_other_packets(self, timeout=1)
            self.num_drops += 1
        finally:
            pass

###############################################################################

@disabled
class TunnelScaleTest(ApiHelper):
    def setUp(self):
        self.configure()
        print()
        if (self.client.is_feature_enable(SWITCH_FEATURE_IPV4_TUNNEL) == 0):
            print("IPv4 tunnel feature not enabled, skipping")
            return

        # underlay config input
        self.uvrf = self.add_vrf(self.device, id=100, src_mac=self.rmac)
        self.vm_ip = '100.100.1.1'
        self.vm_ip6 = '1234:5678:9abc:def0:4422:1133:5577:9911'
        self.customer_ip = '100.100.3.1'
        self.customer_ip6 = '1234:5678:9abc:def0:4422:1133:5577:0011'
        self.vni = 0x1234
        self.l2_vni = 0x4321
        self.my_lb_ip = '1.1.1.3'
        self.tunnel_ip = '1.1.1.1'
        self.inner_dmac = "00:11:11:11:11:11"
        self.underlay_neighbor_mac = '00:33:33:33:33:33'

        # overlay configuration
        self.ovrf = self.add_vrf(self.device, id = 200, src_mac=self.rmac)

        # create underlay/overlay loopback rif for tunnel
        self.urif_lb = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_LOOPBACK, vrf_handle=self.uvrf, src_mac=self.rmac)
        self.orif_lb = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_LOOPBACK, vrf_handle=self.ovrf, src_mac=self.rmac)

        self.urif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port1, vrf_handle=self.uvrf, src_mac=self.rmac)
        self.orif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port0, vrf_handle=self.ovrf, src_mac=self.rmac)

        # create Encap/Decap mappers
        self.encap_tunnel_map = self.add_tunnel_mapper(self.device,
            type=SWITCH_TUNNEL_MAPPER_ATTR_TYPE_VRF_HANDLE_TO_VNI)
        self.decap_tunnel_map = self.add_tunnel_mapper(self.device,
            type=SWITCH_TUNNEL_MAPPER_ATTR_TYPE_VNI_TO_VRF_HANDLE)

        # create Encap/Decap mapper entries for ovrf
        self.encap_tunnel_map_entry = self.add_tunnel_mapper_entry(self.device,
            type=SWITCH_TUNNEL_MAPPER_ENTRY_ATTR_TYPE_VRF_HANDLE_TO_VNI,
            tunnel_mapper_handle=self.encap_tunnel_map,
            network_handle=self.ovrf,
            tunnel_vni=self.vni)
        self.decap_tunnel_map_entry = self.add_tunnel_mapper_entry(self.device,
            type=SWITCH_TUNNEL_MAPPER_ENTRY_ATTR_TYPE_VNI_TO_VRF_HANDLE,
            tunnel_mapper_handle=self.decap_tunnel_map,
            network_handle=self.ovrf,
            tunnel_vni=self.vni)

    def runTest(self):
        self.encap_ttl_mode = SWITCH_TUNNEL_ATTR_ENCAP_TTL_MODE_PIPE_MODEL
        self.decap_ttl_mode = SWITCH_TUNNEL_ATTR_DECAP_TTL_MODE_PIPE_MODEL
        if (self.client.is_feature_enable(SWITCH_FEATURE_IPV4_TUNNEL) == 0):
            return
        try:
            self.P2MPTest()
            self.P2PTest()
        finally:
            pass

    def P2MPTest(self):
        print('P2MPTest()')
        if (self.client.is_feature_enable(SWITCH_FEATURE_VXLAN) == 0):
            print("VXLAN Tunnel feature not enabled, skipping")
            return
        tunnel_table_info = self.client.table_info_get(SWITCH_DEVICE_ATTR_TABLE_TUNNEL)
        try:
            i = 0
            self.tunnel_type = SWITCH_TUNNEL_ATTR_TYPE_VXLAN
            ingress_mapper_list = [switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=self.encap_tunnel_map)]
            egress_mapper_list = [switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=self.decap_tunnel_map)]

            for _ in range(0, tunnel_table_info.size - 1):
              self.tunnel = self.add_tunnel(self.device,
                type=self.tunnel_type,
                src_ip=self.my_lb_ip,
                ingress_mapper_handles=ingress_mapper_list,
                egress_mapper_handles=egress_mapper_list,
                encap_ttl_mode=self.encap_ttl_mode,
                decap_ttl_mode=self.decap_ttl_mode,
                underlay_rif_handle=self.urif_lb)
              if self.status() != 0:
                break
              i += 1
        finally:
            tunnel_table_info = self.client.table_info_get(SWITCH_DEVICE_ATTR_TABLE_TUNNEL)
            fill = int((tunnel_table_info.usage / tunnel_table_info.size) * 100)
            print("TUNNEL: size {} usage {} fill {} %".format(tunnel_table_info.size, tunnel_table_info.usage, fill))
            for _ in range(0, i):
              self.cleanlast()
            if tunnel_table_info.size > 250:
              self.assertTrue(fill >= 99)

    def P2PTest(self):
        print('P2PTest()')
        if (self.client.is_feature_enable(SWITCH_FEATURE_IPV4_TUNNEL) == 0):
            print("IPINIP Tunnel feature not enabled, skipping")
            return
        tunnel_table_info = self.client.table_info_get(SWITCH_DEVICE_ATTR_TABLE_TUNNEL)
        try:
            i = 0
            self.tunnel_type = SWITCH_TUNNEL_ATTR_TYPE_IPIP
            for _ in range(0, tunnel_table_info.size - 1):
              self.tunnel = self.add_tunnel(self.device,
                type=self.tunnel_type,
                src_ip=self.my_lb_ip,
                encap_ttl_mode=self.encap_ttl_mode,
                decap_ttl_mode=self.decap_ttl_mode,
                underlay_rif_handle=self.urif_lb,
                overlay_rif_handle=self.orif_lb)
              if self.status() != 0:
                break
              i += 1
        finally:
            tunnel_table_info = self.client.table_info_get(SWITCH_DEVICE_ATTR_TABLE_TUNNEL)
            fill = int((tunnel_table_info.usage / tunnel_table_info.size) * 100)
            print("TUNNEL: size {} usage {} fill {} %".format(tunnel_table_info.size, tunnel_table_info.usage, fill))
            for _ in range(0, i):
              self.cleanlast()
            if tunnel_table_info.size > 250:
              self.assertTrue(fill >= 99)

    def tearDown(self):
        self.cleanup()

###############################################################################

@group('tunnel')
class VxlanTransitTest(ApiHelper):
    def setUp(self):
        self.configure()
        print()
        if (self.client.is_feature_enable(SWITCH_FEATURE_IPV4_TUNNEL) == 0):
            print("IPv4 tunnel feature not enabled, skipping")
            return
        if (self.client.is_feature_enable(SWITCH_FEATURE_VXLAN) == 0):
            print("VXLAN feature not enabled, skipping")
            return

        # underlay config input
        self.uvrf = self.add_vrf(self.device, id=100, src_mac=self.rmac)
        self.vm_ip = '100.100.1.1'
        self.vm_ip6 = '1234:5678:9abc:def0:4422:1133:5577:9911'
        self.customer_ip = '100.100.3.1'
        self.customer_ip6 = '1234:5678:9abc:def0:4422:1133:5577:0011'
        self.vni = 0x1234
        self.l2_vni = 0x4321
        self.my_lb_ip = '1.1.1.3'
        self.tunnel_ip = '1.1.1.1'
        self.inner_dmac = "00:11:11:11:11:11"
        self.underlay_neighbor_mac = '00:33:33:33:33:33'

        # overlay configuration
        self.ovrf = self.add_vrf(self.device, id = 200, src_mac=self.rmac)
        self.tunnel_type = SWITCH_TUNNEL_ATTR_TYPE_VXLAN
        self.term_type = SWITCH_TUNNEL_TERM_ATTR_TERMINATION_TYPE_P2MP
        self.encap_ttl_mode = SWITCH_TUNNEL_ATTR_ENCAP_TTL_MODE_PIPE_MODEL
        self.decap_ttl_mode = SWITCH_TUNNEL_ATTR_DECAP_TTL_MODE_PIPE_MODEL

        # create underlay loopback rif for tunnel
        self.urif_lb = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_LOOPBACK, vrf_handle=self.uvrf, src_mac=self.rmac)

        self.urif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port1, vrf_handle=self.uvrf, src_mac=self.rmac)
        self.orif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port0, vrf_handle=self.ovrf, src_mac=self.rmac)

        # Configure MTUs for the RIFs
        tcpv4_pkt = simple_tcp_packet()
        tcpv6_pkt = simple_tcpv6_packet()
        self.v4mtu_offset = len(simple_vxlan_packet(inner_frame=tcpv4_pkt)) - len(tcpv4_pkt)
        self.v6mtu_offset = len(simple_vxlan_packet(inner_frame=tcpv6_pkt)) - len(tcpv6_pkt)
        self.overlay_v4_mtu = 200
        self.overlay_v6_mtu = 500
        self.underlay_v4_mtu = self.overlay_v4_mtu + self.v4mtu_offset - 1
        self.underlay_v6_mtu = self.overlay_v6_mtu + self.v6mtu_offset - 1

        self.attribute_set(self.orif, SWITCH_RIF_ATTR_MTU, self.overlay_v4_mtu)
        self.attribute_set(self.urif, SWITCH_RIF_ATTR_MTU, self.underlay_v4_mtu)

        # create Encap/Decap mappers
        self.encap_tunnel_map = self.add_tunnel_mapper(self.device,
            type=SWITCH_TUNNEL_MAPPER_ATTR_TYPE_VRF_HANDLE_TO_VNI)
        self.decap_tunnel_map = self.add_tunnel_mapper(self.device,
            type=SWITCH_TUNNEL_MAPPER_ATTR_TYPE_VNI_TO_VRF_HANDLE)

        # create Encap/Decap mapper entries for ovrf
        self.encap_tunnel_map_entry = self.add_tunnel_mapper_entry(self.device,
            type=SWITCH_TUNNEL_MAPPER_ENTRY_ATTR_TYPE_VRF_HANDLE_TO_VNI,
            tunnel_mapper_handle=self.encap_tunnel_map,
            network_handle=self.ovrf,
            tunnel_vni=self.vni)
        self.decap_tunnel_map_entry = self.add_tunnel_mapper_entry(self.device,
            type=SWITCH_TUNNEL_MAPPER_ENTRY_ATTR_TYPE_VNI_TO_VRF_HANDLE,
            tunnel_mapper_handle=self.decap_tunnel_map,
            network_handle=self.ovrf,
            tunnel_vni=self.vni)

        ingress_mapper_list = [switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=self.encap_tunnel_map)]
        egress_mapper_list = [switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=self.decap_tunnel_map)]

        # create Tunnel
        self.tunnel = self.add_tunnel(self.device,
            type=self.tunnel_type,
            src_ip=self.my_lb_ip,
            ingress_mapper_handles=ingress_mapper_list,
            egress_mapper_handles=egress_mapper_list,
            encap_ttl_mode=self.encap_ttl_mode,
            decap_ttl_mode=self.decap_ttl_mode,
            underlay_rif_handle=self.urif_lb)

        # Create tunnel_termination entry
        self.tunnel_term = self.add_tunnel_term(self.device,
            type=self.tunnel_type,
            vrf_handle=self.uvrf,
            tunnel_handle=self.tunnel,
            termination_type=self.term_type,
            dst_ip=self.my_lb_ip)

        # Create route to customer from VM
        self.onhop = self.add_nexthop(self.device, handle=self.orif, dest_ip='100.100.3.1')
        self.oneighbor = self.add_neighbor(self.device,
            mac_address='00:22:22:22:22:22',
            handle=self.orif,
            dest_ip=self.customer_ip)  # 100.100.3.1
        self.customer_route = self.add_route(self.device, ip_prefix=self.customer_ip,
            vrf_handle=self.ovrf, nexthop_handle=self.onhop)
        self.customev6_route = self.add_route(self.device, ip_prefix=self.customer_ip6,
            vrf_handle=self.ovrf, nexthop_handle=self.onhop)

        # Create tunnel nexthop for VM 100.100.1.1
        self.tunnel_nexthop = self.add_nexthop(self.device,
            type=SWITCH_NEXTHOP_ATTR_TYPE_TUNNEL,
            rw_type=SWITCH_NEXTHOP_ATTR_RW_TYPE_L3_VNI,
            handle=self.tunnel,
            dest_ip=self.tunnel_ip,  # 1.1.1.1
            mac_address=self.inner_dmac,
            tunnel_vni=self.vni)
        # Add routes for VM (v4 & v6 adress) via tunnel nexthop
        self.customerv4_route = self.add_route(self.device,
            ip_prefix=self.vm_ip,
            vrf_handle=self.ovrf,
            nexthop_handle=self.tunnel_nexthop)
        self.customerv6_route = self.add_route(self.device,
            ip_prefix=self.vm_ip6,
            vrf_handle=self.ovrf,
            nexthop_handle=self.tunnel_nexthop)

        # Create route to tunnel ip 1.1.1.1
        self.unhop = self.add_nexthop(self.device, handle=self.urif, dest_ip=self.tunnel_ip)
        self.uneighbor = self.add_neighbor(self.device,
            mac_address=self.underlay_neighbor_mac,
            handle=self.urif,
            dest_ip=self.tunnel_ip)
        self.tunnel_route = self.add_route(self.device, ip_prefix=self.tunnel_ip,
            vrf_handle=self.uvrf, nexthop_handle=self.unhop)

        # Create more egress RIFs
        rif2 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port2, vrf_handle=self.uvrf, src_mac=self.rmac)
        nhop2 = self.add_nexthop(self.device, handle=rif2, dest_ip='20.20.0.1')
        self.add_neighbor(self.device, mac_address='00:22:22:22:22:33', handle=rif2, dest_ip='20.20.0.1')
        self.add_route(self.device, ip_prefix='20.20.1.1', vrf_handle=self.uvrf, nexthop_handle=nhop2)

        # vlan 10 configuration
        self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port3)
        self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port4)
        self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port5)
        self.attribute_set(self.port3, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_VLAN, vlan_handle=self.vlan10, vrf_handle=self.uvrf, src_mac=self.rmac)

        # vlan 20 configuration
        self.add_vlan_member(self.device, vlan_handle=self.vlan20, member_handle=self.port6)
        self.add_vlan_member(self.device, vlan_handle=self.vlan20, member_handle=self.port7)
        self.add_vlan_member(self.device, vlan_handle=self.vlan20, member_handle=self.port8)
        self.attribute_set(self.port6, SWITCH_PORT_ATTR_PORT_VLAN_ID, 20)
        self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_VLAN, vlan_handle=self.vlan20, vrf_handle=self.uvrf, src_mac=self.rmac)
        self.vlan_decap_tunnel_map = self.add_tunnel_mapper(self.device,
            type=SWITCH_TUNNEL_MAPPER_ATTR_TYPE_VNI_TO_VLAN_HANDLE)
        self.vlan_decap_tunnel_map_entry = self.add_tunnel_mapper_entry(self.device,
            type=SWITCH_TUNNEL_MAPPER_ENTRY_ATTR_TYPE_VNI_TO_VLAN_HANDLE,
            tunnel_mapper_handle=self.vlan_decap_tunnel_map,
            network_handle=self.vlan20,
            tunnel_vni=self.l2_vni)

    def runTest(self):
        try:
            if (self.client.is_feature_enable(SWITCH_FEATURE_IPV4_TUNNEL) == 0):
                return
            if (self.client.is_feature_enable(SWITCH_FEATURE_VXLAN) == 0):
                return
            self.TransitTest()
        finally:
            pass

    def tearDown(self):
        self.attribute_set(self.port3, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.port6, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.cleanup()

    def TransitTest(self):
        try:
            unknown_vni = 0x3333

            # inner packets
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src=self.inner_dmac,
                ip_dst=self.customer_ip,
                ip_src=self.vm_ip,
                ip_id=108,
                ip_ttl=64)
            pkt2 = simple_tcp_packet(
                eth_dst='00:22:22:22:22:22',
                eth_src='00:77:66:55:44:33',
                ip_dst=self.customer_ip,
                ip_src=self.vm_ip,
                ip_id=108,
                ip_ttl=63)

            # basic vxlan packet with valid outer and inner headers
            vxlan_pkt = simple_vxlan_packet(
                eth_src=self.underlay_neighbor_mac,
                eth_dst='00:77:66:55:44:33',
                ip_id=0,
                ip_dst=self.my_lb_ip,
                ip_src=self.tunnel_ip,
                ip_ttl=64,
                ip_flags=0x2,
                udp_sport=11638,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=pkt)
            print("Valid v4 packet, vni 0x1234 from L3 intf port 1")
            send_packet(self, self.devports[1], vxlan_pkt)
            verify_packet(self, pkt2, self.devports[0])
            print("Valid v4 packet, vni 0x1234 from SVI port 3")
            send_packet(self, self.devports[3], vxlan_pkt)
            verify_packet(self, pkt2, self.devports[0])

            vxlan_pkt = simple_vxlan_packet(
                eth_src=self.underlay_neighbor_mac,
                eth_dst='00:77:66:55:44:33',
                ip_id=0,
                ip_dst='20.20.1.1',
                ip_src=self.tunnel_ip,
                ip_ttl=64,
                ip_flags=0x2,
                udp_sport=11638,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=pkt)
            vxlan_exp_pkt = mask.Mask(simple_vxlan_packet(
                eth_dst='00:22:22:22:22:33',
                eth_src='00:77:66:55:44:33',
                ip_id=0,
                ip_dst='20.20.1.1',
                ip_src=self.tunnel_ip,
                ip_ttl=63,
                ip_flags=0x2,
                udp_sport=11638,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=pkt))
            mask_set_do_not_care_packet(vxlan_exp_pkt, UDP, "chksum")
            print("Valid v4 packet, vni 0x1234 from L3 intf port 1, outer ip miss dst_vtep, routed")
            send_packet(self, self.devports[1], vxlan_pkt)
            verify_packet(self, vxlan_exp_pkt, self.devports[2])

            vxlan_pkt = simple_vxlan_packet(
                eth_src=self.underlay_neighbor_mac,
                eth_dst='00:77:66:55:44:44',
                ip_id=0,
                ip_dst=self.customer_ip,
                ip_src=self.tunnel_ip,
                ip_ttl=64,
                ip_flags=0x2,
                udp_sport=11638,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=pkt)
            vxlan_exp_pkt = mask.Mask(vxlan_pkt)
            mask_set_do_not_care_packet(vxlan_exp_pkt, UDP, "chksum")
            print("Valid v4 packet, vni 0x1234 from L3 intf port 1, outer rmac miss, drop")
            send_packet(self, self.devports[1], vxlan_pkt)
            verify_no_other_packets(self, timeout=1)
            print("Valid v4 packet, vni 0x1234 from SVI port 3, outer rmac miss, flood")
            send_packet(self, self.devports[3], vxlan_pkt)
            verify_packets(self, vxlan_exp_pkt, [self.devports[4], self.devports[5]])

            vxlan_pkt = simple_vxlan_packet(
                eth_src=self.underlay_neighbor_mac,
                eth_dst='00:77:66:55:44:33',
                ip_id=0,
                ip_dst=self.my_lb_ip,
                ip_src=self.tunnel_ip,
                ip_ttl=64,
                ip_flags=0x2,
                udp_sport=11638,
                with_udp_chksum=False,
                vxlan_vni=unknown_vni,
                inner_frame=pkt)
            vxlan_exp_pkt = mask.Mask(vxlan_pkt)
            mask_set_do_not_care_packet(vxlan_exp_pkt, UDP, "chksum")
            print("Valid v4 packet, vni 0x3333 from L3 intf port 1, unknown vni, drop")
            send_packet(self, self.devports[1], vxlan_pkt)
            verify_no_other_packets(self, timeout=1)
            print("Valid v4 packet, vni 0x3333 from SVI port 3, unknown vni, drop")
            send_packet(self, self.devports[3], vxlan_pkt)
            verify_no_other_packets(self, timeout=1)

            vxlan_pkt = simple_vxlan_packet(
                eth_src=self.underlay_neighbor_mac,
                eth_dst='00:77:66:55:44:33',
                ip_id=0,
                ip_dst=self.my_lb_ip,
                ip_src=self.tunnel_ip,
                ip_ttl=64,
                ip_flags=0x2,
                udp_sport=11638,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=pkt2)
            print("Valid v4 packet, vni 0x1234 from L3 intf port 1, inner rmac miss, drop")
            send_packet(self, self.devports[1], vxlan_pkt)
            verify_no_other_packets(self, timeout=1)
            print("Valid v4 packet, vni 0x1234 from SVI port 3, inner rmac miss, flood")
            send_packet(self, self.devports[3], vxlan_pkt)
            verify_no_other_packets(self, timeout=1)

            vxlan_pkt = simple_vxlan_packet(
                eth_src=self.underlay_neighbor_mac,
                eth_dst='00:77:66:55:44:33',
                ip_id=0,
                ip_dst=self.my_lb_ip,
                ip_src=self.tunnel_ip,
                ip_ttl=64,
                ip_flags=0x2,
                udp_sport=11638,
                with_udp_chksum=False,
                vxlan_vni=self.l2_vni,
                inner_frame=pkt2)
            print("Valid v4 packet, vni 0x4321 from SVI port 7, inner rmac miss, flood")
            send_packet(self, self.devports[6], vxlan_pkt)
            verify_packets(self, pkt2, [self.devports[7], self.devports[8]])
            print("Add an FDB entry on port 7")
            self.add_mac_entry(self.device, vlan_handle=self.vlan20, mac_address='00:22:22:22:22:22', destination_handle=self.port7)
            print("Valid v4 packet, vni 0x4321 from SVI port 3, inner rmac miss, bridge")
            send_packet(self, self.devports[6], vxlan_pkt)
            verify_packets(self, pkt2, [self.devports[7]])
            self.cleanlast()
        finally:
            pass

###############################################################################


@group('tunnel')
class TunnelNhopResolutionTest(ApiHelper):
    def setUp(self):
        self.configure()
        print()
        if (self.client.is_feature_enable(SWITCH_FEATURE_IPV4_TUNNEL) == 0):
            print("IPv4 tunnel feature not enabled, skipping")
            return
        if not self.client.is_feature_enable(SWITCH_FEATURE_TUNNEL_ENCAP):
            print("Tunnel encap not enabled, skipping")
            return
        if (self.client.is_feature_enable(SWITCH_FEATURE_VXLAN) == 0):
            print("VXLAN feature not enabled, skipping")
            return

        # underlay config input
        self.uvrf = self.add_vrf(self.device, id=100, src_mac=self.rmac)
        self.vm_ip = '100.100.1.1'
        self.vm_ip2 = '100.100.2.1'
        self.customer_ip = '100.100.3.1'
        self.vni = 2000
        self.my_lb_ip = '10.10.10.10'
        self.tunnel_ip = '10.10.10.1'
        self.tunnel_ip2 = '10.10.10.3'
        self.tunnel_ip3 = '10.10.10.4'
        self.inner_dmac = "00:33:33:33:33:33"
        self.inner_dmac2 = "00:33:33:33:33:44"
        self.inner_dmac3 = "00:33:33:33:33:55"
        self.underlay_neighbor_mac = '00:11:11:11:11:11'
        self.underlay_neighbor_mac2 = '00:11:11:11:11:22'
        self.underlay_neighbor_mac3 = '00:11:11:11:11:33'

        # overlay configuration
        self.ovrf = self.add_vrf(self.device, id = 200, src_mac=self.rmac)
        self.tunnel_type = SWITCH_TUNNEL_ATTR_TYPE_VXLAN
        self.term_type = SWITCH_TUNNEL_TERM_ATTR_TERMINATION_TYPE_P2MP
        self.encap_ttl_mode = SWITCH_TUNNEL_ATTR_ENCAP_TTL_MODE_PIPE_MODEL
        self.decap_ttl_mode = SWITCH_TUNNEL_ATTR_DECAP_TTL_MODE_PIPE_MODEL

        # create underlay loopback rif for tunnel
        self.urif_lb = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_LOOPBACK, vrf_handle=self.uvrf, src_mac=self.rmac)
        self.orif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port0, vrf_handle=self.ovrf, src_mac=self.rmac)

        # Encap configuration follows

        # create Encap/Decap mappers
        self.encap_tunnel_map = self.add_tunnel_mapper(self.device,
            type=SWITCH_TUNNEL_MAPPER_ATTR_TYPE_VRF_HANDLE_TO_VNI)
        self.decap_tunnel_map = self.add_tunnel_mapper(self.device,
            type=SWITCH_TUNNEL_MAPPER_ATTR_TYPE_VNI_TO_VRF_HANDLE)

        # create Encap/Decap mapper entries for ovrf
        self.encap_tunnel_map_entry = self.add_tunnel_mapper_entry(self.device,
            type=SWITCH_TUNNEL_MAPPER_ENTRY_ATTR_TYPE_VRF_HANDLE_TO_VNI,
            tunnel_mapper_handle=self.encap_tunnel_map,
            network_handle=self.ovrf,
            tunnel_vni=self.vni)
        self.decap_tunnel_map_entry = self.add_tunnel_mapper_entry(self.device,
            type=SWITCH_TUNNEL_MAPPER_ENTRY_ATTR_TYPE_VNI_TO_VRF_HANDLE,
            tunnel_mapper_handle=self.decap_tunnel_map,
            network_handle=self.ovrf,
            tunnel_vni=self.vni)

        ingress_mapper_list = [switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=self.encap_tunnel_map)]
        egress_mapper_list = [switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=self.decap_tunnel_map)]

        # create Tunnel
        self.tunnel = self.add_tunnel(self.device,
            type=self.tunnel_type,
            src_ip=self.my_lb_ip,
            ingress_mapper_handles=ingress_mapper_list,
            egress_mapper_handles=egress_mapper_list,
            encap_ttl_mode=self.encap_ttl_mode,
            decap_ttl_mode=self.decap_ttl_mode,
            underlay_rif_handle=self.urif_lb)

    def runTest(self):
        if (self.client.is_feature_enable(SWITCH_FEATURE_IPV4_TUNNEL) == 0):
            return
        if not self.client.is_feature_enable(SWITCH_FEATURE_TUNNEL_ENCAP):
            return
        if (self.client.is_feature_enable(SWITCH_FEATURE_VXLAN) == 0):
            return
        try:
            self.MultiTunnelTests()
            self.TunnelL3IntfTests()
            self.TunnelL3SubPortTests()
            self.TunnelSVITests()
            self.TunnelECMPTests()
            self.ECMPTunnelTests()
            self.TunnelUpdateTests()
            self.IncorrectNexthopTests()
        finally:
            pass

    def tearDown(self):
        self.cleanup()

    def MultiTunnelTests(self):
        try:
            self.MultiTunnelNexthopTest1()
            self.MultiTunnelNexthopTest2()
            self.MultiTunnelNexthopTest3()
            self.MultiTunnelNexthopFallBackTest()
            #self.MultiTunnelEncapDecapTest()
            self.TunnelVNINexthopTest()
        finally:
            pass

    def IncorrectNexthopTests(self):
        print("IncorrectNexthopTests")
        try:
            self.api_segment_list1 = []
            ipV6 = '2001:db8:0:a4::'
            self.api_segment_list1.append(switcht_list_val_t(type=switcht_value_type.IP_ADDRESS, ip_addr=ipV6))
            self.urif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port1, vrf_handle=self.uvrf, src_mac=self.rmac)
            self.uneighbor = self.add_neighbor(self.device, mac_address=self.underlay_neighbor_mac, handle=self.urif, dest_ip=self.tunnel_ip)  # 10.10.10.1
            self.unhop = self.add_nexthop(self.device, handle=self.urif, dest_ip='10.10.10.1')
            self.tunnel_route = self.add_route(self.device, ip_prefix="10.10.10.0/24", vrf_handle=self.uvrf, nexthop_handle=self.unhop)

            # create tunnel nexthop for VM
            count_tunnel_dest_ip = len(self.object_get_all_handles(SWITCH_OBJECT_TYPE_TUNNEL_DEST_IP))
            count_nexthop = len(self.object_get_all_handles(SWITCH_OBJECT_TYPE_NEXTHOP))
            print("tries to create incorrect tunnel nexthop object")
            self.tunnel_nexthop = self.add_nexthop(self.device, type=SWITCH_NEXTHOP_ATTR_TYPE_TUNNEL, rw_type=SWITCH_NEXTHOP_ATTR_RW_TYPE_L3_VNI,
                handle=self.tunnel, dest_ip='10.10.10.2', mac_address=self.inner_dmac, tunnel_vni=-2)
            self.customer_route = self.add_route(self.device, ip_prefix=self.vm_ip, vrf_handle=self.ovrf, nexthop_handle=self.tunnel_nexthop)
            if count_nexthop == len(self.object_get_all_handles(SWITCH_OBJECT_TYPE_NEXTHOP)):
                print("verifies remove tunnel dest ip object")
                self.assertTrue(count_tunnel_dest_ip == len(self.object_get_all_handles(SWITCH_OBJECT_TYPE_TUNNEL_DEST_IP)))

            count_nexthop = len(self.object_get_all_handles(SWITCH_OBJECT_TYPE_NEXTHOP))
            count_tunnel_dest_ip == len(self.object_get_all_handles(SWITCH_OBJECT_TYPE_TUNNEL_DEST_IP))
            print("tries to create incorrect srv6 sidlist nexthop object")
            self.sidlist_id1 = self.add_segmentroute_sidlist(self.device,
                                                             type=SWITCH_SEGMENTROUTE_SIDLIST_ATTR_TYPE_H_ENCAPS_RED,
                                                             segment_list=self.api_segment_list1)
            self.srv6_nexthop1 = self.add_nexthop(self.device,
                                                  type=SWITCH_NEXTHOP_ATTR_TYPE_SRV6_SIDLIST,
                                                  handle=self.tunnel,
                                                  srv6_sidlist_id=self.sidlist_id1,
                                                  port_lag_handle =-2,
                                                  tunnel_rif_handle = -2)
            if count_nexthop == len(self.object_get_all_handles(SWITCH_OBJECT_TYPE_NEXTHOP)):
                print("verifies remove tunnel dest ip object")
                self.assertTrue(count_tunnel_dest_ip == len(self.object_get_all_handles(SWITCH_OBJECT_TYPE_TUNNEL_DEST_IP)))
        finally:
            self.cleanlast()


    def TunnelL3IntfTests(self):
        # Create route to tunnel ip
        self.urif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port1, vrf_handle=self.uvrf, src_mac=self.rmac)
        self.uneighbor = self.add_neighbor(self.device,
            mac_address=self.underlay_neighbor_mac,
            handle=self.urif,
            dest_ip=self.tunnel_ip)  # 10.10.10.1
        self.unhop = self.add_nexthop(self.device, handle=self.urif, dest_ip='10.10.10.1')
        try:
            self.RouteBeforeTunnelTest()
            self.TunnelBeforeRouteTest()
            self.TunnelRouteUpdateTest()
            self.TunnelRouteFallbackTest()
        finally:
            # remove the ulay rif, neighbor, nhop
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

    def TunnelL3SubPortTests(self):
        # Create route to tunnel ip
        self.urif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_SUB_PORT, port_handle=self.port1, outer_vlan_id=200, vrf_handle=self.uvrf, src_mac=self.rmac)
        self.uneighbor = self.add_neighbor(self.device,
            mac_address=self.underlay_neighbor_mac,
            handle=self.urif,
            dest_ip=self.tunnel_ip)  # 10.10.10.1
        self.unhop = self.add_nexthop(self.device, handle=self.urif, dest_ip='10.10.10.1')
        try:
            self.RouteBeforeTunnelTest(vlan_id=200)
            self.TunnelBeforeRouteTest(vlan_id=200)
            self.TunnelRouteUpdateTest(vlan_id=200)
            self.TunnelRouteFallbackTest(vlan_id=200)
        finally:
            # remove the ulay rif, neighbor, nhop
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

    def TunnelSVITests(self):
        # Create underlay SVI
        self.urif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_VLAN, vlan_handle=self.vlan10, vrf_handle=self.uvrf, src_mac=self.rmac)
        vlan_mbr1 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port1)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        try:
            self.SVISequenceTest1()
            self.SVISequenceTest2()
            self.SVISequenceTest3()
            self.SVISequenceTest4()
            self.SVISequenceTest5()
            self.SVISequenceTest6()
            self.SVIMacMoveTest()
            self.SVIMacFloodTest()
        finally:
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.cleanlast()
            self.cleanlast()

    def ECMPTunnelTests(self):
        self.urif1 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port1, vrf_handle=self.uvrf, src_mac=self.rmac)
        self.urif2 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port2, vrf_handle=self.uvrf, src_mac=self.rmac)
        self.urif3 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port3, vrf_handle=self.uvrf, src_mac=self.rmac)
        try:
            self.ECMPTunnelSequenceTest1()
            self.ECMPTunnelSequenceTest2()
            self.ECMPTunnelSequenceTest3()
            self.ECMPTunnelUpdateTest()
        finally:
            # remove the ulay rifs
            self.clean_to_object(self.urif1)

    def ECMPTunnelSequenceTest1(self):
        print ("ECMPTunnelSequenceTest1()")
        print("tunnel_nhop -> overlay ecmp - > overlay ecmp_members -> overlay_route -> underlay route")
        self.tunnel_nexthop1 = self.add_nexthop(self.device, type=SWITCH_NEXTHOP_ATTR_TYPE_TUNNEL, rw_type=SWITCH_NEXTHOP_ATTR_RW_TYPE_L3_VNI,
            handle=self.tunnel, dest_ip=self.tunnel_ip, mac_address=self.inner_dmac, tunnel_vni=2000)
        self.tunnel_nexthop2 = self.add_nexthop(self.device, type=SWITCH_NEXTHOP_ATTR_TYPE_TUNNEL, rw_type=SWITCH_NEXTHOP_ATTR_RW_TYPE_L3_VNI,
            handle=self.tunnel, dest_ip=self.tunnel_ip2, mac_address=self.inner_dmac2, tunnel_vni=2000)
        self.tunnel_nexthop3 = self.add_nexthop(self.device, type=SWITCH_NEXTHOP_ATTR_TYPE_TUNNEL, rw_type=SWITCH_NEXTHOP_ATTR_RW_TYPE_L3_VNI,
            handle=self.tunnel, dest_ip=self.tunnel_ip3, mac_address=self.inner_dmac3, tunnel_vni=2000)

        tunnel_ecmp = self.add_ecmp(self.device)
        tunnel_ecmp_member1 = self.add_ecmp_member(self.device, nexthop_handle=self.tunnel_nexthop1,
                ecmp_handle=tunnel_ecmp)
        tunnel_ecmp_member2 = self.add_ecmp_member(self.device, nexthop_handle=self.tunnel_nexthop2, ecmp_handle=tunnel_ecmp)
        tunnel_ecmp_member3 = self.add_ecmp_member(self.device, nexthop_handle=self.tunnel_nexthop3, ecmp_handle=tunnel_ecmp)
        self.customer_route = self.add_route(self.device, ip_prefix=self.vm_ip, vrf_handle=self.ovrf,
                nexthop_handle=tunnel_ecmp)

        self.uneighbor1 = self.add_neighbor(self.device, mac_address=self.underlay_neighbor_mac, handle=self.urif1, dest_ip=self.tunnel_ip)  # 10.10.10.1
        self.unhop1 = self.add_nexthop(self.device, handle=self.urif1, dest_ip=self.tunnel_ip)
        self.tunnel_route1 = self.add_route(self.device, ip_prefix=self.tunnel_ip, vrf_handle=self.uvrf,
                nexthop_handle=self.unhop1)

        self.uneighbor2 = self.add_neighbor(self.device, mac_address=self.underlay_neighbor_mac2, handle=self.urif2,
                dest_ip=self.tunnel_ip2)  # 10.10.10.3
        self.unhop2 = self.add_nexthop(self.device, handle=self.urif2, dest_ip=self.tunnel_ip2)
        self.tunnel_route2 = self.add_route(self.device, ip_prefix=self.tunnel_ip2, vrf_handle=self.uvrf,
                nexthop_handle=self.unhop2)

        self.uneighbor3 = self.add_neighbor(self.device, mac_address=self.underlay_neighbor_mac3, handle=self.urif3,
                dest_ip=self.tunnel_ip3)  # 10.10.10.4
        self.unhop3 = self.add_nexthop(self.device, handle=self.urif3, dest_ip=self.tunnel_ip3)
        self.tunnel_route3 = self.add_route(self.device, ip_prefix=self.tunnel_ip3, vrf_handle=self.uvrf,
                nexthop_handle=self.unhop3)
        try:
            self.ECMPTunnelTrafficTest()
        finally:
            self.clean_to_object(self.tunnel_nexthop1)
            pass

    def ECMPTunnelSequenceTest2(self):
        print ("ECMPTunnelSequenceTest2()")
        print("underlay route -> overlay ecmp -> tunnel_nhop -> overlay ecmp_members -> overlay_route")
        self.uneighbor1 = self.add_neighbor(self.device, mac_address=self.underlay_neighbor_mac, handle=self.urif1, dest_ip=self.tunnel_ip)  # 10.10.10.1
        self.unhop1 = self.add_nexthop(self.device, handle=self.urif1, dest_ip=self.tunnel_ip)
        self.tunnel_route1 = self.add_route(self.device, ip_prefix=self.tunnel_ip, vrf_handle=self.uvrf,
                nexthop_handle=self.unhop1)

        self.uneighbor2 = self.add_neighbor(self.device, mac_address=self.underlay_neighbor_mac2, handle=self.urif2,
                dest_ip=self.tunnel_ip2)  # 10.10.10.3
        self.unhop2 = self.add_nexthop(self.device, handle=self.urif2, dest_ip=self.tunnel_ip2)
        self.tunnel_route2 = self.add_route(self.device, ip_prefix=self.tunnel_ip2, vrf_handle=self.uvrf,
                nexthop_handle=self.unhop2)

        self.uneighbor3 = self.add_neighbor(self.device, mac_address=self.underlay_neighbor_mac3, handle=self.urif3,
                dest_ip=self.tunnel_ip3)  # 10.10.10.4
        self.unhop3 = self.add_nexthop(self.device, handle=self.urif3, dest_ip=self.tunnel_ip3)
        self.tunnel_route3 = self.add_route(self.device, ip_prefix=self.tunnel_ip3, vrf_handle=self.uvrf,
                nexthop_handle=self.unhop3)

        tunnel_ecmp = self.add_ecmp(self.device)
        self.tunnel_nexthop1 = self.add_nexthop(self.device, type=SWITCH_NEXTHOP_ATTR_TYPE_TUNNEL, rw_type=SWITCH_NEXTHOP_ATTR_RW_TYPE_L3_VNI,
            handle=self.tunnel, dest_ip=self.tunnel_ip, mac_address=self.inner_dmac, tunnel_vni=2000)
        self.tunnel_nexthop2 = self.add_nexthop(self.device, type=SWITCH_NEXTHOP_ATTR_TYPE_TUNNEL, rw_type=SWITCH_NEXTHOP_ATTR_RW_TYPE_L3_VNI,
            handle=self.tunnel, dest_ip=self.tunnel_ip2, mac_address=self.inner_dmac2, tunnel_vni=2000)
        self.tunnel_nexthop3 = self.add_nexthop(self.device, type=SWITCH_NEXTHOP_ATTR_TYPE_TUNNEL, rw_type=SWITCH_NEXTHOP_ATTR_RW_TYPE_L3_VNI,
            handle=self.tunnel, dest_ip=self.tunnel_ip3, mac_address=self.inner_dmac3, tunnel_vni=2000)

        tunnel_ecmp_member1 = self.add_ecmp_member(self.device, nexthop_handle=self.tunnel_nexthop1,
                ecmp_handle=tunnel_ecmp)
        tunnel_ecmp_member2 = self.add_ecmp_member(self.device, nexthop_handle=self.tunnel_nexthop2, ecmp_handle=tunnel_ecmp)
        tunnel_ecmp_member3 = self.add_ecmp_member(self.device, nexthop_handle=self.tunnel_nexthop3, ecmp_handle=tunnel_ecmp)

        self.customer_route = self.add_route(self.device, ip_prefix=self.vm_ip, vrf_handle=self.ovrf,
                nexthop_handle=tunnel_ecmp)
        try:
            self.ECMPTunnelTrafficTest()
        finally:
            self.clean_to_object(self.uneighbor1)
            pass

    def ECMPTunnelSequenceTest3(self):
        print ("ECMPTunnelSequenceTest3()")
        print("underlay route -> overlay ecmp -> overlay_route -> tunnel_nhop -> overlay ecmp_members")
        self.uneighbor1 = self.add_neighbor(self.device, mac_address=self.underlay_neighbor_mac, handle=self.urif1, dest_ip=self.tunnel_ip)  # 10.10.10.1
        self.unhop1 = self.add_nexthop(self.device, handle=self.urif1, dest_ip=self.tunnel_ip)
        self.tunnel_route1 = self.add_route(self.device, ip_prefix=self.tunnel_ip, vrf_handle=self.uvrf,
                nexthop_handle=self.unhop1)

        self.uneighbor2 = self.add_neighbor(self.device, mac_address=self.underlay_neighbor_mac2, handle=self.urif2,
                dest_ip=self.tunnel_ip2)  # 10.10.10.3
        self.unhop2 = self.add_nexthop(self.device, handle=self.urif2, dest_ip=self.tunnel_ip2)
        self.tunnel_route2 = self.add_route(self.device, ip_prefix=self.tunnel_ip2, vrf_handle=self.uvrf,
                nexthop_handle=self.unhop2)

        self.uneighbor3 = self.add_neighbor(self.device, mac_address=self.underlay_neighbor_mac3, handle=self.urif3,
                dest_ip=self.tunnel_ip3)  # 10.10.10.4
        self.unhop3 = self.add_nexthop(self.device, handle=self.urif3, dest_ip=self.tunnel_ip3)
        self.tunnel_route3 = self.add_route(self.device, ip_prefix=self.tunnel_ip3, vrf_handle=self.uvrf,
                nexthop_handle=self.unhop3)

        tunnel_ecmp = self.add_ecmp(self.device)
        self.customer_route = self.add_route(self.device, ip_prefix=self.vm_ip, vrf_handle=self.ovrf,
                nexthop_handle=tunnel_ecmp)

        self.tunnel_nexthop1 = self.add_nexthop(self.device, type=SWITCH_NEXTHOP_ATTR_TYPE_TUNNEL, rw_type=SWITCH_NEXTHOP_ATTR_RW_TYPE_L3_VNI,
            handle=self.tunnel, dest_ip=self.tunnel_ip, mac_address=self.inner_dmac, tunnel_vni=2000)
        self.tunnel_nexthop2 = self.add_nexthop(self.device, type=SWITCH_NEXTHOP_ATTR_TYPE_TUNNEL, rw_type=SWITCH_NEXTHOP_ATTR_RW_TYPE_L3_VNI,
            handle=self.tunnel, dest_ip=self.tunnel_ip2, mac_address=self.inner_dmac2, tunnel_vni=2000)
        self.tunnel_nexthop3 = self.add_nexthop(self.device, type=SWITCH_NEXTHOP_ATTR_TYPE_TUNNEL, rw_type=SWITCH_NEXTHOP_ATTR_RW_TYPE_L3_VNI,
            handle=self.tunnel, dest_ip=self.tunnel_ip3, mac_address=self.inner_dmac3, tunnel_vni=2000)

        tunnel_ecmp_member1 = self.add_ecmp_member(self.device, nexthop_handle=self.tunnel_nexthop1,
                ecmp_handle=tunnel_ecmp)
        tunnel_ecmp_member2 = self.add_ecmp_member(self.device, nexthop_handle=self.tunnel_nexthop2, ecmp_handle=tunnel_ecmp)
        tunnel_ecmp_member3 = self.add_ecmp_member(self.device, nexthop_handle=self.tunnel_nexthop3, ecmp_handle=tunnel_ecmp)

        try:
            self.ECMPTunnelTrafficTest()
        finally:
            self.clean_to_object(self.uneighbor1)
            pass
        pass

    def ECMPTunnelUpdateTest(self):
        print ("ECMPTunnelUpdateTest()")
        print("Verify overlay ecmp tunnel members add delete")
        self.uneighbor1 = self.add_neighbor(self.device, mac_address=self.underlay_neighbor_mac, handle=self.urif1, dest_ip=self.tunnel_ip)  # 10.10.10.1
        self.unhop1 = self.add_nexthop(self.device, handle=self.urif1, dest_ip=self.tunnel_ip)
        self.tunnel_route1 = self.add_route(self.device, ip_prefix=self.tunnel_ip, vrf_handle=self.uvrf,
                nexthop_handle=self.unhop1)

        self.uneighbor2 = self.add_neighbor(self.device, mac_address=self.underlay_neighbor_mac2, handle=self.urif2,
                dest_ip=self.tunnel_ip2)  # 10.10.10.3
        self.unhop2 = self.add_nexthop(self.device, handle=self.urif2, dest_ip=self.tunnel_ip2)
        self.tunnel_route2 = self.add_route(self.device, ip_prefix=self.tunnel_ip2, vrf_handle=self.uvrf,
                nexthop_handle=self.unhop2)

        self.uneighbor3 = self.add_neighbor(self.device, mac_address=self.underlay_neighbor_mac3, handle=self.urif3,
                dest_ip=self.tunnel_ip3)  # 10.10.10.4
        self.unhop3 = self.add_nexthop(self.device, handle=self.urif3, dest_ip=self.tunnel_ip3)
        self.tunnel_route3 = self.add_route(self.device, ip_prefix=self.tunnel_ip3, vrf_handle=self.uvrf,
                nexthop_handle=self.unhop3)

        tunnel_ecmp = self.add_ecmp(self.device)
        self.customer_route = self.add_route(self.device, ip_prefix=self.vm_ip, vrf_handle=self.ovrf,
                nexthop_handle=tunnel_ecmp)

        self.tunnel_nexthop1 = self.add_nexthop(self.device, type=SWITCH_NEXTHOP_ATTR_TYPE_TUNNEL, rw_type=SWITCH_NEXTHOP_ATTR_RW_TYPE_L3_VNI,
            handle=self.tunnel, dest_ip=self.tunnel_ip, mac_address=self.inner_dmac, tunnel_vni=2000)
        self.tunnel_nexthop2 = self.add_nexthop(self.device, type=SWITCH_NEXTHOP_ATTR_TYPE_TUNNEL, rw_type=SWITCH_NEXTHOP_ATTR_RW_TYPE_L3_VNI,
            handle=self.tunnel, dest_ip=self.tunnel_ip2, mac_address=self.inner_dmac2, tunnel_vni=2000)
        self.tunnel_nexthop3 = self.add_nexthop(self.device, type=SWITCH_NEXTHOP_ATTR_TYPE_TUNNEL, rw_type=SWITCH_NEXTHOP_ATTR_RW_TYPE_L3_VNI,
            handle=self.tunnel, dest_ip=self.tunnel_ip3, mac_address=self.inner_dmac3, tunnel_vni=2000)

        tunnel_ecmp_member1 = self.add_ecmp_member(self.device, nexthop_handle=self.tunnel_nexthop1,
                ecmp_handle=tunnel_ecmp)
        tunnel_ecmp_member2 = self.add_ecmp_member(self.device, nexthop_handle=self.tunnel_nexthop2, ecmp_handle=tunnel_ecmp)
        tunnel_ecmp_member3 = self.add_ecmp_member(self.device, nexthop_handle=self.tunnel_nexthop3, ecmp_handle=tunnel_ecmp)
        try:
            self.ECMPTunnelTrafficTest()
            print("Delete ECMP tunnel member3")
            self.clean_to_object(tunnel_ecmp_member3)
            self.ECMPTunnelTrafficTest([self.devports[1], self.devports[2]])
            print("Delete ECMP tunnel member2")
            self.clean_to_object(tunnel_ecmp_member2)
            self.ECMPTunnelTrafficTest([self.devports[1]])
            print("Delete ECMP tunnel member1")
            self.clean_to_object(tunnel_ecmp_member1)
            print("Add back ECMP tunnel member3")
            tunnel_ecmp_member3 = self.add_ecmp_member(self.device, nexthop_handle=self.tunnel_nexthop3,
                    ecmp_handle=tunnel_ecmp)
            self.ECMPTunnelTrafficTest([self.devports[3]])
            print("Add back ECMP tunnel member2")
            tunnel_ecmp_member2 = self.add_ecmp_member(self.device, nexthop_handle=self.tunnel_nexthop2, ecmp_handle=tunnel_ecmp)
            self.ECMPTunnelTrafficTest([self.devports[2], self.devports[3]])
            print("Add back ECMP tunnel member1")
            tunnel_ecmp_member1 = self.add_ecmp_member(self.device, nexthop_handle=self.tunnel_nexthop1, ecmp_handle=tunnel_ecmp)
            self.ECMPTunnelTrafficTest([self.devports[1], self.devports[2], self.devports[3]])
        finally:
            self.clean_to_object(self.tunnel_nexthop1)
            pass

    def ECMPTunnelTrafficTest(self, ports=None):
        if ports is None:
            ports = [self.devports[1], self.devports[2], self.devports[3]]
        print("Sending packet from Access port 0 to VxLan ports {}".format(ports))
        pkt = simple_tcp_packet(
            eth_dst='00:77:66:55:44:33',
            eth_src='00:22:22:22:22:22',
            ip_dst=self.vm_ip,
            ip_src=self.customer_ip,
            ip_id=108,
            ip_ttl=64)
        exp_pkts = []
        if self.devports[1] in ports:
            inner_pkt1 = simple_tcp_packet(
                eth_dst=self.inner_dmac,  # 00:33:33:33:33:33
                eth_src=self.default_rmac,
                ip_dst=self.vm_ip,
                ip_src=self.customer_ip,
                ip_id=108,
                ip_ttl=63)
            vxlan_pkt1 = mask.Mask(simple_vxlan_packet(
                eth_src='00:77:66:55:44:33',
                eth_dst=self.underlay_neighbor_mac,  # 00:11:11:11:11:11
                ip_id=0,
                ip_src=self.my_lb_ip,
                ip_dst=self.tunnel_ip,
                ip_ttl=64,
                ip_flags=0x2,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=inner_pkt1))
            mask_set_do_not_care_packet(vxlan_pkt1, UDP, "sport")
            exp_pkts.append(vxlan_pkt1)
        if self.devports[2] in ports:
            inner_pkt2 = simple_tcp_packet(
                eth_dst=self.inner_dmac2,  # 00:33:33:33:33:33
                eth_src=self.default_rmac,
                ip_dst=self.vm_ip,
                ip_src=self.customer_ip,
                ip_id=108,
                ip_ttl=63)
            vxlan_pkt2 = mask.Mask(simple_vxlan_packet(
                eth_src='00:77:66:55:44:33',
                eth_dst=self.underlay_neighbor_mac2,  # 00:11:11:11:11:11
                ip_id=0,
                ip_src=self.my_lb_ip,
                ip_dst=self.tunnel_ip2,
                ip_ttl=64,
                ip_flags=0x2,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=inner_pkt2))
            mask_set_do_not_care_packet(vxlan_pkt2, UDP, "sport")
            exp_pkts.append(vxlan_pkt2)
        if self.devports[3] in ports:
            inner_pkt3 = simple_tcp_packet(
                eth_dst=self.inner_dmac3,  # 00:33:33:33:33:33
                eth_src=self.default_rmac,
                ip_dst=self.vm_ip,
                ip_src=self.customer_ip,
                ip_id=108,
                ip_ttl=63)
            vxlan_pkt3 = mask.Mask(simple_vxlan_packet(
                eth_src='00:77:66:55:44:33',
                eth_dst=self.underlay_neighbor_mac3,  # 00:11:11:11:11:11
                ip_id=0,
                ip_src=self.my_lb_ip,
                ip_dst=self.tunnel_ip3,
                ip_ttl=64,
                ip_flags=0x2,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=inner_pkt3))
            mask_set_do_not_care_packet(vxlan_pkt3, UDP, "sport")
            exp_pkts.append(vxlan_pkt3)
        send_packet(self, self.devports[0], pkt)
        rcv_idx = verify_any_packet_on_ports_list(
            self, exp_pkts, [ports], timeout=2)

    def TunnelECMPTests(self):
        self.urif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port1, vrf_handle=self.uvrf, src_mac=self.rmac)
        self.unhop = self.add_nexthop(self.device, handle=self.urif, dest_ip='10.10.10.1')
        self.uneighbor = self.add_neighbor(self.device, mac_address=self.underlay_neighbor_mac,
            handle=self.urif, dest_ip=self.tunnel_ip)  # 10.10.10.1
        try:
            self.ECMPSequenceTest1()
            self.ECMPSequenceTest2()
            self.ECMPSequenceTest3()
            self.ECMPNhopFallbackTest()
        finally:
            # remove the ulay rif, neighbor, nhop
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

    def TunnelUpdateTests(self):
        #Few Tunnel update tests
        try:
            self.TunnelEndpointUpdateTest()
            self.TunnelNbrResolveAfterRouteTest()
            self.TunnelUnderlayRouteResolveAfterTunnelNhopTest()
        finally:
            pass


    def MultiTunnelNexthopTest1(self):
        print("MultiTunnelNexthopTest1() - Add outer nexthop before tunnel nexthops")
        if not self.client.is_feature_enable(SWITCH_FEATURE_TUNNEL_ENCAP):
            print(" Tunnel encap not enabled, skipping")
            return
        self.urif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port1, vrf_handle=self.uvrf, src_mac=self.rmac)
        self.uneighbor = self.add_neighbor(self.device, mac_address=self.underlay_neighbor_mac, handle=self.urif, dest_ip=self.tunnel_ip)  # 10.10.10.1
        self.unhop = self.add_nexthop(self.device, handle=self.urif, dest_ip='10.10.10.1')
        self.tunnel_route = self.add_route(self.device, ip_prefix="10.10.10.0/24", vrf_handle=self.uvrf, nexthop_handle=self.unhop)

        # create tunnel nexthop for VM
        self.tunnel_nexthop = self.add_nexthop(self.device, type=SWITCH_NEXTHOP_ATTR_TYPE_TUNNEL, rw_type=SWITCH_NEXTHOP_ATTR_RW_TYPE_L3_VNI,
            handle=self.tunnel, dest_ip='10.10.10.2', mac_address=self.inner_dmac, tunnel_vni=2000)
        self.customer_route = self.add_route(self.device, ip_prefix=self.vm_ip, vrf_handle=self.ovrf, nexthop_handle=self.tunnel_nexthop)
        self.tunnel_nexthop = self.add_nexthop(self.device, type=SWITCH_NEXTHOP_ATTR_TYPE_TUNNEL, rw_type=SWITCH_NEXTHOP_ATTR_RW_TYPE_L3_VNI,
            handle=self.tunnel, dest_ip='10.10.10.3', mac_address=self.inner_dmac2, tunnel_vni=2000)
        self.customer_route = self.add_route(self.device, ip_prefix=self.vm_ip2, vrf_handle=self.ovrf, nexthop_handle=self.tunnel_nexthop)

        try:
            self.TrafficTest3(self.devports[1])
        finally:
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

    def MultiTunnelNexthopTest2(self):
        print("MultiTunnelNexthopTest2() - Add tunnel nexthops before outer nexthop")
        # create tunnel nexthop for VM
        self.tunnel_nexthop = self.add_nexthop(self.device, type=SWITCH_NEXTHOP_ATTR_TYPE_TUNNEL, rw_type=SWITCH_NEXTHOP_ATTR_RW_TYPE_L3_VNI,
            handle=self.tunnel, dest_ip='10.10.10.2', mac_address=self.inner_dmac, tunnel_vni=2000)
        self.customer_route = self.add_route(self.device, ip_prefix=self.vm_ip, vrf_handle=self.ovrf, nexthop_handle=self.tunnel_nexthop)
        self.tunnel_nexthop = self.add_nexthop(self.device, type=SWITCH_NEXTHOP_ATTR_TYPE_TUNNEL, rw_type=SWITCH_NEXTHOP_ATTR_RW_TYPE_L3_VNI,
            handle=self.tunnel, dest_ip='10.10.10.3', mac_address=self.inner_dmac2, tunnel_vni=2000)
        self.customer_route = self.add_route(self.device, ip_prefix=self.vm_ip2, vrf_handle=self.ovrf, nexthop_handle=self.tunnel_nexthop)

        self.urif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port1, vrf_handle=self.uvrf, src_mac=self.rmac)
        self.uneighbor = self.add_neighbor(self.device, mac_address=self.underlay_neighbor_mac, handle=self.urif, dest_ip=self.tunnel_ip)  # 10.10.10.1
        self.unhop = self.add_nexthop(self.device, handle=self.urif, dest_ip='10.10.10.1')
        self.tunnel_route = self.add_route(self.device, ip_prefix="10.10.10.0/24", vrf_handle=self.uvrf, nexthop_handle=self.unhop)

        try:
            self.TrafficTest3(self.devports[1])
        finally:
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

    def MultiTunnelNexthopTest3(self):
        print("MultiTunnelNexthopTest3() - Add tunnel nexthop before and after outer nexthop")
        # add first tunnel nexthop
        self.tunnel_nexthop = self.add_nexthop(self.device, type=SWITCH_NEXTHOP_ATTR_TYPE_TUNNEL, rw_type=SWITCH_NEXTHOP_ATTR_RW_TYPE_L3_VNI,
            handle=self.tunnel, dest_ip='10.10.10.2', mac_address=self.inner_dmac, tunnel_vni=2000)
        self.customer_route = self.add_route(self.device, ip_prefix=self.vm_ip, vrf_handle=self.ovrf, nexthop_handle=self.tunnel_nexthop)

        # add outer nexthop
        self.urif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port1, vrf_handle=self.uvrf, src_mac=self.rmac)
        self.uneighbor = self.add_neighbor(self.device, mac_address=self.underlay_neighbor_mac, handle=self.urif, dest_ip=self.tunnel_ip)  # 10.10.10.1
        self.unhop = self.add_nexthop(self.device, handle=self.urif, dest_ip='10.10.10.1')
        self.tunnel_route = self.add_route(self.device, ip_prefix="10.10.10.0/24", vrf_handle=self.uvrf, nexthop_handle=self.unhop)

        # add 2nd tunnel nexthop
        self.tunnel_nexthop = self.add_nexthop(self.device, type=SWITCH_NEXTHOP_ATTR_TYPE_TUNNEL, rw_type=SWITCH_NEXTHOP_ATTR_RW_TYPE_L3_VNI,
            handle=self.tunnel, dest_ip='10.10.10.3', mac_address=self.inner_dmac2, tunnel_vni=2000)
        self.customer_route = self.add_route(self.device, ip_prefix=self.vm_ip2, vrf_handle=self.ovrf, nexthop_handle=self.tunnel_nexthop)

        try:
            self.TrafficTest3(self.devports[1])
        finally:
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

    def MultiTunnelNexthopFallBackTest(self):
        print("MultiTunnelNexthopFallBackTest()")
        # add tunnel nexthops
        self.tunnel_nexthop = self.add_nexthop(self.device, type=SWITCH_NEXTHOP_ATTR_TYPE_TUNNEL, rw_type=SWITCH_NEXTHOP_ATTR_RW_TYPE_L3_VNI,
            handle=self.tunnel, dest_ip='10.10.10.2', mac_address=self.inner_dmac, tunnel_vni=2000)
        self.customer_route = self.add_route(self.device, ip_prefix=self.vm_ip, vrf_handle=self.ovrf, nexthop_handle=self.tunnel_nexthop)
        self.tunnel_nexthop = self.add_nexthop(self.device, type=SWITCH_NEXTHOP_ATTR_TYPE_TUNNEL, rw_type=SWITCH_NEXTHOP_ATTR_RW_TYPE_L3_VNI,
            handle=self.tunnel, dest_ip='10.10.10.3', mac_address=self.inner_dmac2, tunnel_vni=2000)
        self.customer_route = self.add_route(self.device, ip_prefix=self.vm_ip2, vrf_handle=self.ovrf, nexthop_handle=self.tunnel_nexthop)

        # add outer nexthop
        self.urif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port1, vrf_handle=self.uvrf, src_mac=self.rmac)
        self.uneighbor = self.add_neighbor(self.device, mac_address=self.underlay_neighbor_mac, handle=self.urif, dest_ip=self.tunnel_ip)  # 10.10.10.1
        self.unhop = self.add_nexthop(self.device, handle=self.urif, dest_ip='10.10.10.1')
        self.tunnel_route = self.add_route(self.device, ip_prefix="10.10.0.0/16", vrf_handle=self.uvrf, nexthop_handle=self.unhop)

        try:
            print("Packets send to port1 via 10.10.0.0/16")
            self.TrafficTest3(self.devports[1])
        finally:
            pass

        self.urif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port2, vrf_handle=self.uvrf, src_mac=self.rmac)
        self.uneighbor = self.add_neighbor(self.device, mac_address=self.underlay_neighbor_mac, handle=self.urif, dest_ip=self.tunnel_ip)  # 10.10.10.1
        self.unhop = self.add_nexthop(self.device, handle=self.urif, dest_ip='10.10.10.1')
        self.tunnel_route = self.add_route(self.device, ip_prefix="10.10.10.0/24", vrf_handle=self.uvrf, nexthop_handle=self.unhop)

        try:
            print("Packets send to port2 via 10.10.10.0/24")
            self.TrafficTest3(self.devports[2])
        finally:
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

        try:
            print("Packets send to port1 via 10.10.0.0/16")
            self.TrafficTest3(self.devports[1])
        finally:
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

    def MultiTunnelEncapDecapTest(self):
        '''
        This test simulates a scenario in which a switch acts as a tunnel termination for tunnel1
        and nexthop for overlay destination is tunnel2. Both tunnel1 and tunnel2 are linked to the common
        overlay vrf. The test covers two cases, tunnel1 and tunnel2 in same and different underlay vrfs.
        On the Ingress VxLAN packet with VNI 1000 is received which is terminated on the switch and
        the inner packet is then routed to another VxLAN tunnel with VNI=3000 on egress.
                   --------------------
                   | VM: 200.200.2.1  |
                   | TP: 20.20.20.1   |
                   --------------------
                          ^
                          | VxLAN Pkt Out:VNI=3000
          -----------------------------------
          |            | p1 |               |
          |            ------               |
          |               |                 |
          |               |                 |
          |  -----------------------        |
          |  | Tunnel: 20.20.20.10 |        |
          |  -----------------------        |
          |              /                  |
          |              /                  |
          |              /                  |
          |  -----------------------        |
          |  | Tunnel: 10.10.10.10 |        |
          |  -----------------------        |
          |        |                        |
          |      ------                     |
          |      | p0 |                     |
          -----------------------------------
                    ^
                    |VxLAN Pkt In
            -------------------
            |    VNI = 2000   |
            -------------------
        '''
        print("MultiTunnelEncapDecapTest() - Terminate ingress VxLAN packet and then re-encap the inner packet")
        self.vni2 = 3000
        self.uvrf2 = self.add_vrf(self.device, id=300, src_mac=self.rmac)
        self.tunnel2_lb_ip = '20.20.20.10'
        self.tunnel2_endpoint_ip = '20.20.20.1'
        self.underlay2_neighbor_mac = '00:11:11:11:11:22'
        self.customer2_ip = '200.200.2.1'

        self.tunnel_endpoint_ip2 = '10.10.10.2'
        self.customer3_ip = '100.100.1.3'
        self.underlay_neighbor_mac3 = '00:11:11:11:11:55'

        #create underlay loopback rif for tunnel2 (20.20.20.10)
        self.urif2_lb = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_LOOPBACK, vrf_handle=self.uvrf2, src_mac=self.rmac)

        ingress_mapper_list = [switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=self.encap_tunnel_map)]
        egress_mapper_list = [switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=self.decap_tunnel_map)]

        # create Tunnel
        self.tunnel2 = self.add_tunnel(self.device,
            type=self.tunnel_type,
            src_ip=self.tunnel2_lb_ip,
            ingress_mapper_handles=ingress_mapper_list,
            egress_mapper_handles=egress_mapper_list,
            encap_ttl_mode=self.encap_ttl_mode,
            decap_ttl_mode=self.decap_ttl_mode,
            underlay_rif_handle=self.urif2_lb)

        # create tunnel nexthop
        self.tunnel_nexthop = self.add_nexthop(self.device,
            type=SWITCH_NEXTHOP_ATTR_TYPE_TUNNEL,
            rw_type=SWITCH_NEXTHOP_ATTR_RW_TYPE_L3_VNI,
            handle=self.tunnel2,
            dest_ip=self.tunnel2_endpoint_ip,  # 20.20.20.1
            mac_address=self.inner_dmac2,
            tunnel_vni=self.vni2)

        # add route to tunnel_nexthop
        self.customer_route = self.add_route(self.device,
            ip_prefix=self.customer2_ip,
            vrf_handle=self.ovrf,
            nexthop_handle=self.tunnel_nexthop)

        #Add route for tunnel2 endpoint IP (outer IP)
        self.urif2 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port3, vrf_handle=self.uvrf2, src_mac=self.rmac)
        self.uneighbor = self.add_neighbor(self.device,
            mac_address=self.underlay2_neighbor_mac,
            handle=self.urif2,
            dest_ip=self.tunnel2_endpoint_ip)  # 20.20.20.1
        self.unhop = self.add_nexthop(self.device, handle=self.urif2, dest_ip='20.20.20.1')
        self.tunnel2_route = self.add_route(self.device, ip_prefix="20.20.20.0/24", vrf_handle=self.uvrf2, nexthop_handle=self.unhop)

        # Create tunnel_termination entry
        self.tunnel_term = self.add_tunnel_term(self.device,
            type=self.tunnel_type,
            vrf_handle=self.uvrf,
            tunnel_handle=self.tunnel,
            termination_type=self.term_type,
            dst_ip=self.my_lb_ip)

        self.urif_in = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port1, vrf_handle=self.uvrf, src_mac=self.rmac)

        # Re-encapsulate inner pkt with the same tunnel src_ip with which it came in
        # create tunnel nexthop
        self.tunnel_nexthop = self.add_nexthop(self.device,
            type=SWITCH_NEXTHOP_ATTR_TYPE_TUNNEL,
            rw_type=SWITCH_NEXTHOP_ATTR_RW_TYPE_L3_VNI,
            handle=self.tunnel,
            dest_ip=self.tunnel_endpoint_ip2,  # 10.10.10.2
            mac_address=self.inner_dmac2,
            tunnel_vni=self.vni2)

        # add route to tunnel_nexthop
        self.customer_route = self.add_route(self.device,
            ip_prefix=self.customer3_ip,
            vrf_handle=self.ovrf,
            nexthop_handle=self.tunnel_nexthop)

        #Add route for tunnel endpoint IP (outer IP: 10.10.10.2)
        self.urif3 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port4, vrf_handle=self.uvrf, src_mac=self.rmac)
        self.uneighbor = self.add_neighbor(self.device,
            mac_address=self.underlay_neighbor_mac3,
            handle=self.urif3,
            dest_ip=self.tunnel_endpoint_ip2)  # 10.10.10.2
        self.unhop = self.add_nexthop(self.device, handle=self.urif3, dest_ip='10.10.10.2')
        self.tunnel_route2 = self.add_route(self.device, ip_prefix="10.10.10.2", vrf_handle=self.uvrf, nexthop_handle=self.unhop)

        try:
            self.EncapDecapTrafficTest(in_port=self.devports[1], out_ports=self.devports[3])
            self.EncapDecapTrafficTest(in_port=self.devports[1], customer2_ip=self.customer3_ip,
                                       tunnel2_ep_ip=self.tunnel_endpoint_ip2,
                                       tunnel2_lb_ip=self.my_lb_ip, tunnel2_dmac=self.underlay_neighbor_mac3,
                                       out_ports=self.devports[4])
        finally:
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

    def TunnelVNINexthopTest(self):
        '''
        This test simulates the case when a customer/provider Service is homed
        in a VNI(network) different than customers own VNI. In this case instead
        of the tunnel mapperentry, the tunnel nexthop entry provides the outgoing VNI ID.
        Also we test tunnel nexthop vni update case)
       --------------------        --------------------
       | VM: 100.100.1.1  |        | VM: 200.200.2.2  |
       | TP1: 10.10.10.1  |        | TP2: 30.30.30.3  |
       |     VNI:2000     |        |      VNI:3000    |
       --------------------        --------------------
                 |                    |
          -----------------------------------
          |    | p1 |          | p3 |       |
          |    ------          ------       |
          |      |               |          |
          |      |               |          |
          |        \           /            |
          |          \      /               |
          |            \  /                 |
          |  -----------------------        |
          |  | Tunnel: 10.10.10.10 |        |
          |  -----------------------        |
          |        |                        |
          |      ------                     |
          |      | p0 |                     |
          -----------------------------------
                    |
            -------------------
            | C1: 100.100.0.1 |
            -------------------
        '''
        print("TunnelVNINexthopTest() - Underlay Encap VNI based on tunnel nexthop rather than tunnel mapper")
        service_vm_ip = "200.200.200.2"
        service_vtep_ip = "30.30.30.3"
        service_vni = 3000
        service_vni2 = 4000

        #Tunnel endpoint1 (10.10.10.1) configuration
        self.urif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port1, vrf_handle=self.uvrf, src_mac=self.rmac)
        self.uneighbor = self.add_neighbor(self.device,
            mac_address=self.underlay_neighbor_mac,
            handle=self.urif,
            dest_ip=self.tunnel_ip)  # 10.10.10.1
        self.unhop = self.add_nexthop(self.device, handle=self.urif, dest_ip='10.10.10.1')
        # add route to tunnel endpoint1 ip via port1
        self.tunnel_route = self.add_route(self.device, ip_prefix="10.10.10.1",
            vrf_handle=self.uvrf, nexthop_handle=self.unhop)

        #Tunnel endpoint2 (30.30.30.3) configuration
        self.urif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port2, vrf_handle=self.uvrf, src_mac=self.rmac)
        self.uneighbor = self.add_neighbor(self.device, mac_address=self.underlay_neighbor_mac2, handle=self.urif, dest_ip=service_vtep_ip)  # 30.30.30.3
        self.unhop = self.add_nexthop(self.device, handle=self.urif, dest_ip='30.30.30.3')
        # add route to tunnel endpoint2 ip via port2
        self.tunnel_route2 = self.add_route(self.device, ip_prefix="30.30.30.3", vrf_handle=self.uvrf, nexthop_handle=self.unhop)

        # create tunnel nexthop for VM (tunnel endpoint1)
        self.tunnel_nexthop = self.add_nexthop(self.device,
            type=SWITCH_NEXTHOP_ATTR_TYPE_TUNNEL,
            rw_type=SWITCH_NEXTHOP_ATTR_RW_TYPE_L3_VNI,
            handle=self.tunnel,
            dest_ip=self.tunnel_ip,  # 10.10.10.1
            mac_address=self.inner_dmac,
            tunnel_vni=2000)

        # create tunnel nexthop for VM (tunnel endpoint2)
        self.tunnel_nexthop2 = self.add_nexthop(self.device,
            type=SWITCH_NEXTHOP_ATTR_TYPE_TUNNEL,
            rw_type=SWITCH_NEXTHOP_ATTR_RW_TYPE_L3_VNI,
            handle=self.tunnel,
            dest_ip=service_vtep_ip,  # 30.30.30.3
            mac_address=self.inner_dmac2,
            tunnel_vni=service_vni)

        # add route(100.100.1.1) to tunnel_nexthop
        self.customer_route = self.add_route(self.device,
            ip_prefix=self.vm_ip,
            vrf_handle=self.ovrf,
            nexthop_handle=self.tunnel_nexthop)

        # add route(300.300.300.3) to tunnel_nexthop2
        self.customer_route = self.add_route(self.device,
            ip_prefix=service_vm_ip,
            vrf_handle=self.ovrf,
            nexthop_handle=self.tunnel_nexthop2)

        try:
            self.CustomTrafficTest(out_ports=self.devports[1])
            self.CustomTrafficTest(vm_ip=service_vm_ip,
                                   inner_dmac=self.inner_dmac2,
                                   tunnel_nbr_mac=self.underlay_neighbor_mac2,
                                   tunnel_dst_ip=service_vtep_ip,
                                   vni=service_vni,
                                   out_ports=self.devports[2])
            print("Updating nexthop tunnel {} -> {}".format(service_vni, service_vni2))
            self.attribute_set(self.tunnel_nexthop2, SWITCH_NEXTHOP_ATTR_TUNNEL_VNI, service_vni2)
            self.CustomTrafficTest(vm_ip=service_vm_ip,
                                   inner_dmac=self.inner_dmac2,
                                   tunnel_nbr_mac=self.underlay_neighbor_mac2,
                                   tunnel_dst_ip=service_vtep_ip,
                                   vni=service_vni2,
                                   out_ports=self.devports[2])
        finally:
            #Remove both Underlay rifs, nhops, nbrs, tunnel routes and both overlay tunnel nhops & customer routes
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

    def RouteBeforeTunnelTest(self, vlan_id=0):
        print("RouteBeforeTunnelTest()")
        # add route to tunnel ip
        self.tunnel_route = self.add_route(self.device, ip_prefix="10.10.10.1",
            vrf_handle=self.uvrf, nexthop_handle=self.unhop)

        # create tunnel nexthop for VM
        self.tunnel_nexthop = self.add_nexthop(self.device,
            type=SWITCH_NEXTHOP_ATTR_TYPE_TUNNEL,
            rw_type=SWITCH_NEXTHOP_ATTR_RW_TYPE_L3_VNI,
            handle=self.tunnel,
            dest_ip=self.tunnel_ip,  # 10.10.10.1
            mac_address=self.inner_dmac,
            tunnel_vni=2000)

        # add route to tunnel_nexthop
        self.customer_route = self.add_route(self.device,
            ip_prefix=self.vm_ip,
            vrf_handle=self.ovrf,
            nexthop_handle=self.tunnel_nexthop)

        try:
            self.TrafficTest1(vlan_id)
        finally:
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()


    def TunnelEndpointUpdateTest(self):
        '''
        This test simulates the case when a customer VM moves from
        from Tunnel Enpdoint1(TP1) to Tunnel Endpoint 2(TP2). (Tunnel Nexthop update
        case)
       --------------------        --------------------
       | VM: 100.100.1.1  | -----> | VM: 100.100.1.1  |
       | TP1: 10.10.10.1  |        | TP2: 10.10.10.3  |
       --------------------        --------------------
                 |                    |
          -----------------------------------
          |    | p1 |          | p3 |       |
          |    ------          ------       |
          |      |               |          |
          |      | ------------> |          |
          |        \           /            |
          |          \      /               |
          |            \  /                 |
          |  -----------------------        |
          |  | Tunnel: 10.10.10.10 |        |
          |  -----------------------        |
          |        |                        |
          |      ------                     |
          |      | p0 |                     |
          -----------------------------------
                    |
            -------------------
            | C1: 100.100.0.1 |
            -------------------
        '''
        print("TunnelEndpointUpdateTest() - Update tunnel endpoint for overlay network 10.10.10.1 -> 10.10.10.3")

        #Tunnel endpoint1 (10.10.10.1) configuration
        self.urif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port1, vrf_handle=self.uvrf, src_mac=self.rmac)
        self.uneighbor = self.add_neighbor(self.device,
            mac_address=self.underlay_neighbor_mac,
            handle=self.urif,
            dest_ip=self.tunnel_ip)  # 10.10.10.1
        self.unhop = self.add_nexthop(self.device, handle=self.urif, dest_ip='10.10.10.1')
        # add route to tunnel endpoint1 ip via port1
        self.tunnel_route = self.add_route(self.device, ip_prefix="10.10.10.1",
            vrf_handle=self.uvrf, nexthop_handle=self.unhop)

        #Tunnel endpoint2 (10.0.0.3) configuration
        self.urif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port2, vrf_handle=self.uvrf, src_mac=self.rmac)
        self.uneighbor = self.add_neighbor(self.device, mac_address=self.underlay_neighbor_mac2, handle=self.urif, dest_ip=self.tunnel_ip2)  # 10.10.10.3
        self.unhop = self.add_nexthop(self.device, handle=self.urif, dest_ip='10.10.10.3')
        # add route to tunnel endpoint2 ip via port2
        self.tunnel_route2 = self.add_route(self.device, ip_prefix="10.10.10.3", vrf_handle=self.uvrf, nexthop_handle=self.unhop)

        # create tunnel nexthop for VM (tunnel endpoint1)
        self.tunnel_nexthop = self.add_nexthop(self.device,
            type=SWITCH_NEXTHOP_ATTR_TYPE_TUNNEL,
            rw_type=SWITCH_NEXTHOP_ATTR_RW_TYPE_L3_VNI,
            handle=self.tunnel,
            dest_ip=self.tunnel_ip,  # 10.10.10.1
            mac_address=self.inner_dmac,
            tunnel_vni=2000)

        # create tunnel nexthop for VM (tunnel endpoint2)
        self.tunnel_nexthop2 = self.add_nexthop(self.device,
            type=SWITCH_NEXTHOP_ATTR_TYPE_TUNNEL,
            rw_type=SWITCH_NEXTHOP_ATTR_RW_TYPE_L3_VNI,
            handle=self.tunnel,
            dest_ip=self.tunnel_ip2,  # 10.10.10.3
            mac_address=self.inner_dmac,
            tunnel_vni=2000)

        # add route to tunnel_nexthop
        self.customer_route = self.add_route(self.device,
            ip_prefix=self.vm_ip,
            vrf_handle=self.ovrf,
            nexthop_handle=self.tunnel_nexthop)

        try:
            self.TrafficTest1()
            self.attribute_set(self.customer_route, SWITCH_ROUTE_ATTR_NEXTHOP_HANDLE, self.tunnel_nexthop2)
            self.TrafficTest4()
        finally:
            # Remove underlay rifs, neighbors, nhops and tunnel routes, overlay tunnel nhops and customer route
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()


    def TunnelNbrResolveAfterRouteTest(self):
        '''
        This test simulates a scenario in which a customer route pointes to a tunnel.
        The tunnel route and its nexthop are installed. While the neighbor entry is resolved
        after installation of nhop and route entries (for eg. via arp response from the nbr)
        '''
        print("TunnelNbrResolveAfterRouteTest() - Resolve underlay nexthop's(tunnel_ip:port1) neighbor(tunnel_ip:uneighbor_mac) after tunnel route(tunnel_ip:nexthop) is installed")

        #Tunnel endpoint1 (10.10.10.1) configuration
        self.urif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port1, vrf_handle=self.uvrf, src_mac=self.rmac)
        self.unhop = self.add_nexthop(self.device, handle=self.urif, dest_ip='10.10.10.1')
        # add route to tunnel endpoint1 ip via port1
        self.tunnel_route = self.add_route(self.device, ip_prefix="10.10.10.1",
            vrf_handle=self.uvrf, nexthop_handle=self.unhop)

        # create tunnel nexthop for VM (tunnel endpoint1)
        self.tunnel_nexthop = self.add_nexthop(self.device,
            type=SWITCH_NEXTHOP_ATTR_TYPE_TUNNEL,
            rw_type=SWITCH_NEXTHOP_ATTR_RW_TYPE_L3_VNI,
            handle=self.tunnel,
            dest_ip=self.tunnel_ip,  # 10.10.10.1
            mac_address=self.inner_dmac,
            tunnel_vni=2000)

        self.uneighbor = self.add_neighbor(self.device,
            mac_address=self.underlay_neighbor_mac,
            handle=self.urif,
            dest_ip=self.tunnel_ip)  # 10.10.10.1

        # add route to tunnel_nexthop
        self.customer_route = self.add_route(self.device,
            ip_prefix=self.vm_ip,
            vrf_handle=self.ovrf,
            nexthop_handle=self.tunnel_nexthop)

        try:
            self.TrafficTest1()
        finally:
            # Remove underlay rifs, neighbors, nhops and tunnel routes, overlay tunnel nhops and customer route
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

    def TunnelUnderlayRouteResolveAfterTunnelNhopTest(self):
        '''
        This test simulates a scenario in which a customer route pointes to a tunnel.
        Tunnel Underlay Nexthop is initially Nhop:Drop. The underlay nhop and nbr entries are
        installed after overlay tunnel nexthop is installed.
        '''
        print("TunnelUnderlayRouteResolveAfterTunnelNhopTest - Resolve underlay route entry nhop and neigbor after tunnel route(tunnel_ip:nexthop) is installed")

        #Tunnel endpoint1 (10.10.10.1) configuration
        self.urif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port1, vrf_handle=self.uvrf, src_mac=self.rmac)
        unhop_drop = self.add_nexthop(self.device, handle=self.urif,type=SWITCH_NEXTHOP_ATTR_TYPE_DROP)
        # add route to tunnel endpoint1 ip via port1
        self.tunnel_route = self.add_route(self.device, ip_prefix="10.10.10.1",
            vrf_handle=self.uvrf, nexthop_handle=unhop_drop)

        # create tunnel nexthop for VM (tunnel endpoint1)
        self.tunnel_nexthop = self.add_nexthop(self.device,
            type=SWITCH_NEXTHOP_ATTR_TYPE_TUNNEL,
            rw_type=SWITCH_NEXTHOP_ATTR_RW_TYPE_L3_VNI,
            handle=self.tunnel,
            dest_ip=self.tunnel_ip,  # 10.10.10.1
            mac_address=self.inner_dmac,
            tunnel_vni=2000)

        # add route to tunnel_nexthop
        self.customer_route = self.add_route(self.device,
            ip_prefix=self.vm_ip,
            vrf_handle=self.ovrf,
            nexthop_handle=self.tunnel_nexthop)

        #Resovle underlay route neighbor and nexthop
        self.uneighbor = self.add_neighbor(self.device,
            mac_address=self.underlay_neighbor_mac,
            handle=self.urif,
            dest_ip=self.tunnel_ip)  # 10.10.10.1
        self.unhop = self.add_nexthop(self.device, handle=self.urif, dest_ip='10.10.10.1')
        # Point underlay  route to tunnel endpoint1 ip via port1
        self.attribute_set(self.tunnel_route, SWITCH_ROUTE_ATTR_NEXTHOP_HANDLE, self.unhop)
        try:
            self.TrafficTest1()
        finally:
            # Remove underlay rifs, neighbors, nhops and drop nhop and tunnel routes,
            # overlay tunnel nhops and customer route
            self.attribute_set(self.tunnel_route, SWITCH_ROUTE_ATTR_NEXTHOP_HANDLE, unhop_drop)
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

    def TunnelBeforeRouteTest(self, vlan_id=0):
        print("TunnelBeforeRouteTest()")
        # create tunnel nexthop for VM
        self.tunnel_nexthop = self.add_nexthop(self.device,
            type=SWITCH_NEXTHOP_ATTR_TYPE_TUNNEL, handle=self.tunnel,
            rw_type=SWITCH_NEXTHOP_ATTR_RW_TYPE_L3_VNI,
            dest_ip=self.tunnel_ip,  # 10.10.10.1
            mac_address=self.inner_dmac, tunnel_vni=2000)

        # add route to tunnel_nexthop
        self.customer_route = self.add_route(self.device,
            ip_prefix=self.vm_ip, vrf_handle=self.ovrf,
            nexthop_handle=self.tunnel_nexthop)

        # add route to tunnel ip
        self.tunnel_route = self.add_route(self.device, ip_prefix="10.10.10.1",
            vrf_handle=self.uvrf, nexthop_handle=self.unhop)

        try:
            self.TrafficTest1(vlan_id)
        finally:
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

    def TunnelRouteUpdateTest(self, vlan_id=0):
        print("TunnelRouteUpdateTest()")
        # create tunnel nexthop for VM
        self.tunnel_nexthop = self.add_nexthop(self.device,
            type=SWITCH_NEXTHOP_ATTR_TYPE_TUNNEL, handle=self.tunnel,
            rw_type=SWITCH_NEXTHOP_ATTR_RW_TYPE_L3_VNI,
            dest_ip=self.tunnel_ip,  # 10.10.10.1
            mac_address=self.inner_dmac, tunnel_vni=2000)

        # add route to tunnel_nexthop
        self.customer_route = self.add_route(self.device,
            ip_prefix=self.vm_ip, vrf_handle=self.ovrf,
            nexthop_handle=self.tunnel_nexthop)

        # add route to tunnel ip
        self.tunnel_route = self.add_route(self.device, ip_prefix="10.10.10.1",
            vrf_handle=self.uvrf, nexthop_handle=self.unhop)

        try:
            self.TrafficTest1(vlan_id)
        finally:
            pass

        self.cleanlast()
        # add new route to tunnel ip
        self.tunnel_route = self.add_route(self.device, ip_prefix="10.0.0.0/8",
            vrf_handle=self.uvrf, nexthop_handle=self.unhop)

        try:
            self.TrafficTest1(vlan_id)
        finally:
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

    def TunnelRouteFallbackTest(self, vlan_id=0):
        print("TunnelRouteFallbackTest()")
        # create tunnel nexthop for VM
        self.tunnel_nexthop = self.add_nexthop(self.device,
            type=SWITCH_NEXTHOP_ATTR_TYPE_TUNNEL, handle=self.tunnel,
            rw_type=SWITCH_NEXTHOP_ATTR_RW_TYPE_L3_VNI,
            dest_ip=self.tunnel_ip,  # 10.10.10.1
            mac_address=self.inner_dmac, tunnel_vni=2000)

        # add route to tunnel_nexthop
        self.customer_route = self.add_route(self.device,
            ip_prefix=self.vm_ip, vrf_handle=self.ovrf,
            nexthop_handle=self.tunnel_nexthop)

        print("add lpm route to port 2")
        # route 1
        self.urif1 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port2, vrf_handle=self.uvrf, src_mac=self.rmac)
        self.unhop1 = self.add_nexthop(self.device, handle=self.urif1, dest_ip='10.10.10.1')
        self.uneighbor1 = self.add_neighbor(self.device, mac_address=self.underlay_neighbor_mac2,
            handle=self.urif1, dest_ip=self.tunnel_ip)  # 10.10.10.1
        self.tunnel_route = self.add_route(self.device, ip_prefix="10.0.0.0/8",
            vrf_handle=self.uvrf, nexthop_handle=self.unhop1)

        print("add host route to port 1")
        # route 2
        self.tunnel_route = self.add_route(self.device, ip_prefix="10.10.10.1",
            vrf_handle=self.uvrf, nexthop_handle=self.unhop)

        try:
            print("Packet sent to port 1 via host route")
            self.TrafficTest1(vlan_id)
        finally:
            pass

        # delete route 2
        print("Removing host route")
        self.cleanlast()

        try:
            print("Packet sent to port 2 via lpm route")
            self.TrafficTest2()
        finally:
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

    def SVIMacMoveTest(self):
        '''
        This test simulates a scenario in which a customer route points to a tunnel.
        The tunnel route has a nexthop over a SVI. SVI consists of two ports port1 and port2.
        And the nexthop neighbor moves from port1 to port2 (Mac Move)
        '''
        print("SVIMacMoveTest()")
        vlan_mbr1 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port2)
        self.attribute_set(self.port2, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.attribute_set(self.port1, SWITCH_PORT_ATTR_LEARNING, True)
        self.attribute_set(self.port2, SWITCH_PORT_ATTR_LEARNING, True)
        lag0 = self.add_lag(self.device)
        vlan_mbr2 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=lag0)
        self.attribute_set(lag0, SWITCH_LAG_ATTR_PORT_VLAN_ID, 10)
        lag_mbr0 = self.add_lag_member(self.device, lag_handle=lag0, port_handle=self.port3)
        lag_mbr1 = self.add_lag_member(self.device, lag_handle=lag0, port_handle=self.port4)
        self.attribute_set(self.port5, SWITCH_PORT_ATTR_LEARNING, True)
        self.attribute_set(lag0, SWITCH_LAG_ATTR_LEARNING, True)
        self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_LEARNING, True)

        # create underlay nhop config
        self.unhop = self.add_nexthop(self.device, handle=self.urif, dest_ip='10.10.10.1')
        self.uneighbor = self.add_neighbor(self.device, mac_address=self.underlay_neighbor_mac,
            handle=self.urif, dest_ip=self.tunnel_ip)  # 10.10.10.1
        print("Sending Ether Packet [SrcMac: {}] for MAC learning from Port1".format(self.underlay_neighbor_mac))
        arp = simple_arp_packet(
            eth_src=self.underlay_neighbor_mac,
            ip_tgt='10.10.10.17')
        send_packet(self, self.devports[1], arp)
        # Allow the mac entry to to be learned
        time.sleep(3)
        self.tunnel_route = self.add_route(self.device, ip_prefix="10.10.10.1",
            vrf_handle=self.uvrf, nexthop_handle=self.unhop)

        # create tunnel nexthop for VM
        self.tunnel_nexthop = self.add_nexthop(self.device,
            type=SWITCH_NEXTHOP_ATTR_TYPE_TUNNEL, handle=self.tunnel,
            rw_type=SWITCH_NEXTHOP_ATTR_RW_TYPE_L3_VNI,
            dest_ip=self.tunnel_ip,  # 10.10.10.1
            mac_address=self.inner_dmac, tunnel_vni=2000)

        # add route to tunnel_nexthop
        self.customer_route = self.add_route(self.device,
            ip_prefix=self.vm_ip, vrf_handle=self.ovrf,
            nexthop_handle=self.tunnel_nexthop)

        try:
            print("Verify Underlay packet is sent via Vlan 10 Port1")
            self.TrafficTest1()
            print("Sending Ether Packet [SrcMac: {}] for MAC learning from Port2".format(self.underlay_neighbor_mac))
            send_packet(self, self.devports[2], arp)
            # Allow the mac entry to to be learned
            time.sleep(3)
            print("Verify Underlay packet is sent via Vlan 10 Port2")
            self.TrafficTest5()

            print("Sending Ether Packet [SrcMac: {}] for MAC learning from Lag-Port3 ".format(self.underlay_neighbor_mac))
            send_packet(self, self.devports[3], arp)
            # Allow the mac entry to to be learned
            time.sleep(3)
            print("Verify Underlay packet is sent via Lag-[Port3/Port4]")
            self.CustomTrafficTest(out_ports= [self.devports[3], self.devports[4]])
        finally:
            self.client.object_flush_all(SWITCH_OBJECT_TYPE_MAC_ENTRY)
            # Remove customer route, tunnel route, uneighbor, unhop
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.attribute_set(self.port2, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(lag0, SWITCH_LAG_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.vlan10, SWITCH_VLAN_ATTR_LEARNING, False)
            self.attribute_set(lag0, SWITCH_LAG_ATTR_LEARNING, False)
            self.attribute_set(self.port2, SWITCH_PORT_ATTR_LEARNING, False)
            self.attribute_set(self.port1, SWITCH_PORT_ATTR_LEARNING, False)
            # Remove lag from vlan, lag members and lag
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            # Remove vlan10_member-port2
            self.cleanlast()

    def SVIMacFloodTest(self):
        '''
        This test simulates a scenario in which a customer route points to a tunnel.
        The tunnel route has a nexthop over a SVI. SVI consists of two ports port1 and port2.
        And the nexthop neighbor MAC has not been learned
        '''
        print("SVIMacFloodTest()")
        vlan_mbr1 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port2)
        self.attribute_set(self.port2, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        # create underlay nhop config
        self.unhop = self.add_nexthop(self.device, handle=self.urif, dest_ip='10.10.10.1')
        self.uneighbor = self.add_neighbor(self.device, mac_address=self.underlay_neighbor_mac,
            handle=self.urif, dest_ip=self.tunnel_ip)  # 10.10.10.1
        self.tunnel_route = self.add_route(self.device, ip_prefix="10.10.10.1",
            vrf_handle=self.uvrf, nexthop_handle=self.unhop)

        # create tunnel nexthop for VM
        self.tunnel_nexthop = self.add_nexthop(self.device,
            type=SWITCH_NEXTHOP_ATTR_TYPE_TUNNEL, handle=self.tunnel,
            rw_type=SWITCH_NEXTHOP_ATTR_RW_TYPE_L3_VNI,
            dest_ip=self.tunnel_ip,  # 10.10.10.1
            mac_address=self.inner_dmac, tunnel_vni=2000)

        # add route to tunnel_nexthop
        self.customer_route = self.add_route(self.device,
            ip_prefix=self.vm_ip, vrf_handle=self.ovrf,
            nexthop_handle=self.tunnel_nexthop)

        try:
            #p4 support does not exist for flood case
            #print("Verify Underlay packet is flooded on Vlan 10 Ports - Port1 & Port2")
            print("Verify Underlay packet is dropped")
            self.TrafficTest6()
        finally:
            # Remove customer route, tunnel route, uneighbor, unhop and vlan10_member-por2
            self.attribute_set(self.port2, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

    def SVISequenceTest1(self):
        print("SVISequenceTest1()")
        # ulay_nhop -> uneighbor -> mac -> route -> tunnel_nhop
        # create underlay nhop config
        self.unhop = self.add_nexthop(self.device, handle=self.urif, dest_ip='10.10.10.1')
        self.uneighbor = self.add_neighbor(self.device, mac_address=self.underlay_neighbor_mac,
            handle=self.urif, dest_ip=self.tunnel_ip)  # 10.10.10.1
        self.mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan10,
            mac_address=self.underlay_neighbor_mac, destination_handle=self.port1)
        self.tunnel_route = self.add_route(self.device, ip_prefix="10.10.10.1",
            vrf_handle=self.uvrf, nexthop_handle=self.unhop)

        # create tunnel nexthop for VM
        self.tunnel_nexthop = self.add_nexthop(self.device,
            type=SWITCH_NEXTHOP_ATTR_TYPE_TUNNEL, handle=self.tunnel,
            rw_type=SWITCH_NEXTHOP_ATTR_RW_TYPE_L3_VNI,
            dest_ip=self.tunnel_ip,  # 10.10.10.1
            mac_address=self.inner_dmac, tunnel_vni=2000)

        # add route to tunnel_nexthop
        self.customer_route = self.add_route(self.device,
            ip_prefix=self.vm_ip, vrf_handle=self.ovrf,
            nexthop_handle=self.tunnel_nexthop)

        try:
            print("ulay_nhop -> uneighbor -> mac -> route -> tunnel_nhop")
            self.TrafficTest1()
        finally:
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

    def SVISequenceTest2(self):
        print("SVISequenceTest2()")
        # tunnel_nhop -> ulay_nhop -> uneighbor -> mac -> route
        # create tunnel nexthop for VM
        self.tunnel_nexthop = self.add_nexthop(self.device,
            type=SWITCH_NEXTHOP_ATTR_TYPE_TUNNEL, handle=self.tunnel,
            rw_type=SWITCH_NEXTHOP_ATTR_RW_TYPE_L3_VNI,
            dest_ip=self.tunnel_ip,  # 10.10.10.1
            mac_address=self.inner_dmac, tunnel_vni=2000)

        # add route to tunnel_nexthop
        self.customer_route = self.add_route(self.device,
            ip_prefix=self.vm_ip, vrf_handle=self.ovrf,
            nexthop_handle=self.tunnel_nexthop)

        # create underlay nhop config
        self.unhop = self.add_nexthop(self.device, handle=self.urif, dest_ip='10.10.10.1')
        self.uneighbor = self.add_neighbor(self.device, mac_address=self.underlay_neighbor_mac,
            handle=self.urif, dest_ip=self.tunnel_ip)  # 10.10.10.1
        self.mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan10,
            mac_address=self.underlay_neighbor_mac,
            destination_handle=self.port1)
        self.tunnel_route = self.add_route(self.device, ip_prefix="10.10.10.1",
            vrf_handle=self.uvrf, nexthop_handle=self.unhop)

        try:
            print("tunnel_nhop -> ulay_nhop -> uneighbor -> mac -> route")
            self.TrafficTest1()
        finally:
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

    def SVISequenceTest3(self):
        print("SVISequenceTest3()")
        # ulay_nhop -> tunnel_nhop -> uneighbor -> mac -> route
        # create underlay nhop config
        self.unhop = self.add_nexthop(self.device, handle=self.urif, dest_ip='10.10.10.1')

        # create tunnel nexthop for VM
        self.tunnel_nexthop = self.add_nexthop(self.device,
            type=SWITCH_NEXTHOP_ATTR_TYPE_TUNNEL, handle=self.tunnel,
            rw_type=SWITCH_NEXTHOP_ATTR_RW_TYPE_L3_VNI,
            dest_ip=self.tunnel_ip,  # 10.10.10.1
            mac_address=self.inner_dmac, tunnel_vni=2000)

        # add route to tunnel_nexthop
        self.customer_route = self.add_route(self.device,
            ip_prefix=self.vm_ip, vrf_handle=self.ovrf,
            nexthop_handle=self.tunnel_nexthop)

        self.uneighbor = self.add_neighbor(self.device, mac_address=self.underlay_neighbor_mac,
            handle=self.urif, dest_ip=self.tunnel_ip)  # 10.10.10.1
        self.mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan10,
            mac_address=self.underlay_neighbor_mac,
            destination_handle=self.port1)
        self.tunnel_route = self.add_route(self.device, ip_prefix="10.10.10.1",
            vrf_handle=self.uvrf, nexthop_handle=self.unhop)

        try:
            print("ulay_nhop -> tunnel_nhop -> uneighbor -> mac -> route")
            self.TrafficTest1()
        finally:
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

    def SVISequenceTest4(self):
        print("SVISequenceTest4()")
        # ulay_nhop -> uneighbor -> tunnel_nhop-> mac -> route
        # create underlay nhop config
        self.unhop = self.add_nexthop(self.device, handle=self.urif, dest_ip='10.10.10.1')
        self.uneighbor = self.add_neighbor(self.device, mac_address=self.underlay_neighbor_mac,
            handle=self.urif, dest_ip=self.tunnel_ip)  # 10.10.10.1

        # create tunnel nexthop for VM
        self.tunnel_nexthop = self.add_nexthop(self.device,
            type=SWITCH_NEXTHOP_ATTR_TYPE_TUNNEL, handle=self.tunnel,
            rw_type=SWITCH_NEXTHOP_ATTR_RW_TYPE_L3_VNI,
            dest_ip=self.tunnel_ip,  # 10.10.10.1
            mac_address=self.inner_dmac, tunnel_vni=2000)

        # add route to tunnel_nexthop
        self.customer_route = self.add_route(self.device,
            ip_prefix=self.vm_ip, vrf_handle=self.ovrf,
            nexthop_handle=self.tunnel_nexthop)

        self.mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan10,
            mac_address=self.underlay_neighbor_mac,
            destination_handle=self.port1)
        self.tunnel_route = self.add_route(self.device, ip_prefix="10.10.10.1",
            vrf_handle=self.uvrf, nexthop_handle=self.unhop)

        try:
            print("ulay_nhop -> uneighbor -> tunnel_nhop -> mac -> route")
            self.TrafficTest1()
        finally:
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

    def SVISequenceTest5(self):
        print("SVISequenceTest5()")
        # ulay_nhop -> mac -> tunnel_nhop-> uneighbor -> route
        # create underlay nhop config
        self.unhop = self.add_nexthop(self.device, handle=self.urif, dest_ip='10.10.10.1')
        self.mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan10,
            mac_address=self.underlay_neighbor_mac,
            destination_handle=self.port1)

        # create tunnel nexthop for VM
        self.tunnel_nexthop = self.add_nexthop(self.device,
            type=SWITCH_NEXTHOP_ATTR_TYPE_TUNNEL, handle=self.tunnel,
            rw_type=SWITCH_NEXTHOP_ATTR_RW_TYPE_L3_VNI,
            dest_ip=self.tunnel_ip,  # 10.10.10.1
            mac_address=self.inner_dmac, tunnel_vni=2000)

        # add route to tunnel_nexthop
        self.customer_route = self.add_route(self.device,
            ip_prefix=self.vm_ip, vrf_handle=self.ovrf,
            nexthop_handle=self.tunnel_nexthop)

        self.uneighbor = self.add_neighbor(self.device, mac_address=self.underlay_neighbor_mac,
            handle=self.urif, dest_ip=self.tunnel_ip)  # 10.10.10.1
        self.tunnel_route = self.add_route(self.device, ip_prefix="10.10.10.1",
            vrf_handle=self.uvrf, nexthop_handle=self.unhop)

        try:
            print("ulay_nhop -> mac -> tunnel_nhop -> uneighbor -> route")
            self.TrafficTest1()
        finally:
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

    def SVISequenceTest6(self):
        print("SVISequenceTest6()")
        # ulay_nhop -> uneighbor -> mac -> tunnel_nhop -> route
        # create underlay nhop config
        self.unhop = self.add_nexthop(self.device, handle=self.urif, dest_ip='10.10.10.1')
        self.uneighbor = self.add_neighbor(self.device, mac_address=self.underlay_neighbor_mac,
            handle=self.urif, dest_ip=self.tunnel_ip)  # 10.10.10.1
        self.mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan10,
            mac_address=self.underlay_neighbor_mac,
            destination_handle=self.port1)

        # create tunnel nexthop for VM
        self.tunnel_nexthop = self.add_nexthop(self.device,
            type=SWITCH_NEXTHOP_ATTR_TYPE_TUNNEL, handle=self.tunnel,
            rw_type=SWITCH_NEXTHOP_ATTR_RW_TYPE_L3_VNI,
            dest_ip=self.tunnel_ip,  # 10.10.10.1
            mac_address=self.inner_dmac, tunnel_vni=2000)

        # add route to tunnel_nexthop
        self.customer_route = self.add_route(self.device,
            ip_prefix=self.vm_ip, vrf_handle=self.ovrf,
            nexthop_handle=self.tunnel_nexthop)

        self.tunnel_route = self.add_route(self.device, ip_prefix="10.10.10.1",
            vrf_handle=self.uvrf, nexthop_handle=self.unhop)

        try:
            print("ulay_nhop -> uneighbor -> mac -> tunnel_nhop -> route")
            self.TrafficTest1()
        finally:
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

    def ECMPSequenceTest1(self):
        print("ECMPSequenceTest1()")
        # ecmp -> ecmp_member -> tunnel_nhop -> route
        # create underlay nhop config
        ecmp0 = self.add_ecmp(self.device)
        ecmp_member01 = self.add_ecmp_member(self.device, nexthop_handle=self.unhop, ecmp_handle=ecmp0)

        # create tunnel nexthop for VM
        self.tunnel_nexthop = self.add_nexthop(self.device,
            type=SWITCH_NEXTHOP_ATTR_TYPE_TUNNEL, handle=self.tunnel,
            rw_type=SWITCH_NEXTHOP_ATTR_RW_TYPE_L3_VNI,
            dest_ip=self.tunnel_ip,  # 10.10.10.1
            mac_address=self.inner_dmac, tunnel_vni=2000)

        # add route to tunnel_nexthop
        self.customer_route = self.add_route(self.device,
            ip_prefix=self.vm_ip, vrf_handle=self.ovrf,
            nexthop_handle=self.tunnel_nexthop)

        self.tunnel_route = self.add_route(self.device, ip_prefix="10.10.10.1",
            vrf_handle=self.uvrf, nexthop_handle=ecmp0)

        try:
            print("ecmp -> ecmp_member -> tunnel_nhop -> route")
            self.TrafficTest1()
        finally:
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

    def ECMPSequenceTest2(self):
        print("ECMPSequenceTest2()")
        # ecmp -> tunnel_nhop -> route -> ecmp_member
        ecmp0 = self.add_ecmp(self.device)

        # create tunnel nexthop for VM
        self.tunnel_nexthop = self.add_nexthop(self.device,
            type=SWITCH_NEXTHOP_ATTR_TYPE_TUNNEL, handle=self.tunnel,
            rw_type=SWITCH_NEXTHOP_ATTR_RW_TYPE_L3_VNI,
            dest_ip=self.tunnel_ip,  # 10.10.10.1
            mac_address=self.inner_dmac, tunnel_vni=2000)

        # add route to tunnel_nexthop
        self.customer_route = self.add_route(self.device,
            ip_prefix=self.vm_ip, vrf_handle=self.ovrf,
            nexthop_handle=self.tunnel_nexthop)

        self.tunnel_route = self.add_route(self.device, ip_prefix="10.10.10.1",
            vrf_handle=self.uvrf, nexthop_handle=ecmp0)

        ecmp_member01 = self.add_ecmp_member(self.device, nexthop_handle=self.unhop, ecmp_handle=ecmp0)

        try:
            print("ecmp -> tunnel_nhop -> route -> ecmp_member")
            self.TrafficTest1()
        finally:
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

    def ECMPSequenceTest3(self):
        print("ECMPSequenceTest3()")
        # tunnel_nhop -> ecmp -> ecmp_member -> route
        # create tunnel nexthop for VM
        self.tunnel_nexthop = self.add_nexthop(self.device,
            type=SWITCH_NEXTHOP_ATTR_TYPE_TUNNEL, handle=self.tunnel,
            rw_type=SWITCH_NEXTHOP_ATTR_RW_TYPE_L3_VNI,
            dest_ip=self.tunnel_ip,  # 10.10.10.1
            mac_address=self.inner_dmac, tunnel_vni=2000)

        # add route to tunnel_nexthop
        self.customer_route = self.add_route(self.device,
            ip_prefix=self.vm_ip, vrf_handle=self.ovrf,
            nexthop_handle=self.tunnel_nexthop)

        ecmp0 = self.add_ecmp(self.device)
        ecmp_member01 = self.add_ecmp_member(self.device, nexthop_handle=self.unhop, ecmp_handle=ecmp0)

        self.tunnel_route = self.add_route(self.device, ip_prefix="10.10.10.1",
            vrf_handle=self.uvrf, nexthop_handle=ecmp0)

        try:
            print("tunnel_nhop -> ecmp -> ecmp_member -> route")
            self.TrafficTest1()
        finally:
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

    def ECMPNhopFallbackTest(self):
        print("ECMPNhopFallbackTest()")
        # create tunnel nexthop for VM
        self.tunnel_nexthop = self.add_nexthop(self.device,
            type=SWITCH_NEXTHOP_ATTR_TYPE_TUNNEL, handle=self.tunnel,
            rw_type=SWITCH_NEXTHOP_ATTR_RW_TYPE_L3_VNI,
            dest_ip=self.tunnel_ip,  # 10.10.10.1
            mac_address=self.inner_dmac, tunnel_vni=2000)

        # add route to tunnel_nexthop
        self.customer_route = self.add_route(self.device,
            ip_prefix=self.vm_ip, vrf_handle=self.ovrf,
            nexthop_handle=self.tunnel_nexthop)

        ecmp0 = self.add_ecmp(self.device)
        ecmp_member01 = self.add_ecmp_member(self.device, nexthop_handle=self.unhop, ecmp_handle=ecmp0)

        self.urif1 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port2, vrf_handle=self.uvrf, src_mac=self.rmac)
        self.unhop1 = self.add_nexthop(self.device, handle=self.urif1, dest_ip='10.10.10.1')
        self.uneighbor1 = self.add_neighbor(self.device, mac_address=self.underlay_neighbor_mac2,
            handle=self.urif1, dest_ip=self.tunnel_ip)  # 10.10.10.1
        self.default_route = self.add_route(self.device, ip_prefix="10.0.0.0/8",
            vrf_handle=self.uvrf, nexthop_handle=self.unhop1)

        self.tunnel_route = self.add_route(self.device, ip_prefix="10.10.10.1",
            vrf_handle=self.uvrf, nexthop_handle=ecmp0)

        try:
            print("Use tunnel_route")
            self.TrafficTest1()
        finally:
            pass

        self.cleanlast()

        try:
            print("Tunnel route remove, fallback to use default_route")
            self.TrafficTest2()
        finally:
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

    def TrafficTest1(self, vlan_id=0):
        try:
            enable = False
            if vlan_id != 0:
                enable = True
            print("Sending packet from Access port 0 to VxLan port 1")
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst=self.vm_ip,
                ip_src=self.customer_ip,
                ip_id=108,
                ip_ttl=64)
            inner_pkt = simple_tcp_packet(
                eth_dst=self.inner_dmac,  # 00:33:33:33:33:33
                eth_src=self.default_rmac,
                ip_dst=self.vm_ip,
                ip_src=self.customer_ip,
                ip_id=108,
                ip_ttl=63)
            vxlan_pkt = mask.Mask(simple_vxlan_packet(
                eth_src='00:77:66:55:44:33',
                eth_dst=self.underlay_neighbor_mac,  # 00:11:11:11:11:11
                ip_id=0,
                ip_src=self.my_lb_ip,
                ip_dst=self.tunnel_ip,
                dl_vlan_enable=enable,
                vlan_vid=vlan_id,
                ip_ttl=64,
                ip_flags=0x2,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=inner_pkt))
            mask_set_do_not_care_packet(vxlan_pkt, UDP, "sport")
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, vxlan_pkt, self.devports[1])
        finally:
            pass

    def TrafficTest2(self):
        try:
            print("Sending packet from Access port 0 to VxLan port 2")
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst=self.vm_ip,
                ip_src=self.customer_ip,
                ip_id=108,
                ip_ttl=64)
            inner_pkt = simple_tcp_packet(
                eth_dst=self.inner_dmac,  # 00:33:33:33:33:33
                eth_src=self.default_rmac,
                ip_dst=self.vm_ip,
                ip_src=self.customer_ip,
                ip_id=108,
                ip_ttl=63)
            vxlan_pkt = mask.Mask(simple_vxlan_packet(
                eth_src='00:77:66:55:44:33',
                eth_dst=self.underlay_neighbor_mac2,  # 00:11:11:11:11:22
                ip_id=0,
                ip_src=self.my_lb_ip,
                ip_dst=self.tunnel_ip,
                ip_ttl=64,
                ip_flags=0x2,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=inner_pkt))
            mask_set_do_not_care_packet(vxlan_pkt, UDP, "sport")
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, vxlan_pkt, self.devports[2])
        finally:
            pass

    def TrafficTest3(self, port):
        try:
            print("Sending packet from Access port 0 to VxLan port 1, 10.10.10.2")
            if not self.client.is_feature_enable(SWITCH_FEATURE_TUNNEL_ENCAP):
                print(" Tunnel encap not enabled, skipping")
            else:
                pkt = simple_tcp_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src='00:22:22:22:22:22',
                    ip_dst=self.vm_ip,
                    ip_src=self.customer_ip,
                    ip_id=108,
                    ip_ttl=64)
                inner_pkt = simple_tcp_packet(
                    eth_dst=self.inner_dmac,  # 00:33:33:33:33:33
                    eth_src=self.default_rmac,
                    ip_dst=self.vm_ip,
                    ip_src=self.customer_ip,
                    ip_id=108,
                    ip_ttl=63)
                vxlan_pkt = mask.Mask(simple_vxlan_packet(
                    eth_src='00:77:66:55:44:33',
                    eth_dst=self.underlay_neighbor_mac,  # 00:11:11:11:11:11
                    ip_id=0,
                    ip_src=self.my_lb_ip,
                    ip_dst='10.10.10.2',
                    ip_ttl=64,
                    ip_flags=0x2,
                    with_udp_chksum=False,
                    vxlan_vni=self.vni,
                    inner_frame=inner_pkt))
                mask_set_do_not_care_packet(vxlan_pkt, UDP, "sport")
                send_packet(self, self.devports[0], pkt)
                verify_packet(self, vxlan_pkt, port)

            print("Sending packet from Access port 0 to VxLan port 1, 10.10.10.3")
            if not self.client.is_feature_enable(SWITCH_FEATURE_TUNNEL_ENCAP):
                print(" Tunnel encap not enabled, skipping")
            else:
                pkt = simple_tcp_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src='00:22:22:22:22:22',
                    ip_dst=self.vm_ip2,
                    ip_src=self.customer_ip,
                    ip_id=108,
                    ip_ttl=64)
                inner_pkt = simple_tcp_packet(
                    eth_dst=self.inner_dmac2,  # 00:33:33:33:33:44
                    eth_src=self.default_rmac,
                    ip_dst=self.vm_ip2,
                    ip_src=self.customer_ip,
                    ip_id=108,
                    ip_ttl=63)
                vxlan_pkt = mask.Mask(simple_vxlan_packet(
                    eth_src='00:77:66:55:44:33',
                    eth_dst=self.underlay_neighbor_mac,  # 00:11:11:11:11:11
                    ip_id=0,
                    ip_src=self.my_lb_ip,
                    ip_dst='10.10.10.3',
                    ip_ttl=64,
                    ip_flags=0x2,
                    with_udp_chksum=False,
                    vxlan_vni=self.vni,
                    inner_frame=inner_pkt))
                mask_set_do_not_care_packet(vxlan_pkt, UDP, "sport")
                send_packet(self, self.devports[0], pkt)
                verify_packet(self, vxlan_pkt, port)
        finally:
            pass

    def TrafficTest4(self):
        try:
            print("Sending packet from Access port 0 to VxLan port 2")
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst=self.vm_ip,
                ip_src=self.customer_ip,
                ip_id=108,
                ip_ttl=64)
            inner_pkt = simple_tcp_packet(
                eth_dst=self.inner_dmac,  # 00:33:33:33:33:33
                eth_src=self.default_rmac,
                ip_dst=self.vm_ip,
                ip_src=self.customer_ip,
                ip_id=108,
                ip_ttl=63)
            vxlan_pkt = mask.Mask(simple_vxlan_packet(
                eth_src='00:77:66:55:44:33',
                eth_dst=self.underlay_neighbor_mac2,  # 00:11:11:11:11:22
                ip_id=0,
                ip_src=self.my_lb_ip,
                ip_dst=self.tunnel_ip2,
                ip_ttl=64,
                ip_flags=0x2,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=inner_pkt))
            mask_set_do_not_care_packet(vxlan_pkt, UDP, "sport")
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, vxlan_pkt, self.devports[2])
        finally:
            pass

    def TrafficTest5(self):
        try:
            print("Sending packet[DstIP:{}/SrcIP:{} -> TunnelEndpoint[DstIP:{}/SrcIP:{}] from Access port 0 to VxLan port 2".format(self.vm_ip, self.customer_ip, self.tunnel_ip, self.my_lb_ip))
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst=self.vm_ip,
                ip_src=self.customer_ip,
                ip_id=108,
                ip_ttl=64)
            inner_pkt = simple_tcp_packet(
                eth_dst=self.inner_dmac,  # 00:33:33:33:33:33
                eth_src=self.default_rmac,
                ip_dst=self.vm_ip,
                ip_src=self.customer_ip,
                ip_id=108,
                ip_ttl=63)
            vxlan_pkt = mask.Mask(simple_vxlan_packet(
                eth_src='00:77:66:55:44:33',
                eth_dst=self.underlay_neighbor_mac,  # 00:11:11:11:11:11
                ip_id=0,
                ip_src=self.my_lb_ip,
                ip_dst=self.tunnel_ip,
                ip_ttl=64,
                ip_flags=0x2,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=inner_pkt))
            mask_set_do_not_care_packet(vxlan_pkt, UDP, "sport")
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, vxlan_pkt, self.devports[2])
        finally:
            pass

    def TrafficTest6(self):
        try:
            print("Sending packet from Access port 0 to VxLan port 1")
            print("Sending packet[DstIP:{}/SrcIP:{} -> TunnelEndpoint[DstIP:{}/SrcIP:{}] from Access port 0. Tunnel neighbor unresolved-dropped".format(self.vm_ip, self.customer_ip, self.tunnel_ip, self.my_lb_ip))
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst=self.vm_ip,
                ip_src=self.customer_ip,
                ip_id=108,
                ip_ttl=64)
            inner_pkt = simple_tcp_packet(
                eth_dst=self.inner_dmac,  # 00:33:33:33:33:33
                eth_src=self.default_rmac,
                ip_dst=self.vm_ip,
                ip_src=self.customer_ip,
                ip_id=108,
                ip_ttl=63)
            vxlan_pkt = mask.Mask(simple_vxlan_packet(
                eth_src='00:77:66:55:44:33',
                eth_dst=self.underlay_neighbor_mac,  # 00:11:11:11:11:11
                ip_id=0,
                ip_src=self.my_lb_ip,
                ip_dst=self.tunnel_ip,
                ip_ttl=64,
                ip_flags=0x2,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=inner_pkt))
            mask_set_do_not_care_packet(vxlan_pkt, UDP, "sport")
            send_packet(self, self.devports[0], pkt)
            #verify_packets(self, vxlan_pkt, [self.devports[1], self.devports[2]])
            verify_no_other_packets(self, timeout=1)
        finally:
            pass

    def CustomTrafficTest(self,
                          customer_ip=None,
                          vm_ip=None,
                          inner_dmac=None,
                          inner_smac=None,
                          tunnel_src_ip=None,
                          tunnel_dst_ip=None,
                          tunnel_nbr_mac=None,
                          vni=None,
                          in_port=None,
                          out_ports=None,
                          flood=False):

        try:
            if customer_ip is None:
                customer_ip=self.customer_ip
            if vm_ip is None:
                vm_ip = self.vm_ip
            if inner_dmac is None:
                inner_dmac = self.inner_dmac  # 00:33:33:33:33:33
            if inner_smac is None:
                inner_smac = self.default_rmac #00:BA:7E:F0:00:00
            if tunnel_src_ip is None:
                tunnel_src_ip = self.my_lb_ip
            if tunnel_dst_ip is None:
                tunnel_dst_ip=self.tunnel_ip
            if tunnel_nbr_mac is None:
                tunnel_nbr_mac=self.underlay_neighbor_mac  # 00:11:11:11:11:11
            if vni is None:
                vni=self.vni  #2000
            if in_port is None:
                in_port=self.devports[0]

            print("Sending packet[DstIP:{}/SrcIP:{} -> [VNI: {}]->TunnelEndpoint[DstIP:{}/SrcIP:{}] from port {}. Packet expected on port {}".format(vm_ip, customer_ip, vni, tunnel_dst_ip, tunnel_src_ip, in_port, out_ports))
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst=vm_ip,
                ip_src=customer_ip,
                ip_id=108,
                ip_ttl=64)
            inner_pkt = simple_tcp_packet(
                eth_dst=inner_dmac,
                eth_src=inner_smac,
                ip_dst=vm_ip,
                ip_src=customer_ip,
                ip_id=108,
                ip_ttl=63)
            vxlan_pkt = mask.Mask(simple_vxlan_packet(
                eth_src='00:77:66:55:44:33',
                eth_dst=tunnel_nbr_mac,
                ip_id=0,
                ip_src=tunnel_src_ip,
                ip_dst=tunnel_dst_ip,
                ip_ttl=64,
                ip_flags=0x2,
                with_udp_chksum=False,
                vxlan_vni=vni,
                inner_frame=inner_pkt))
            mask_set_do_not_care_packet(vxlan_pkt, UDP, "sport")
            send_packet(self, in_port, pkt)
            if out_ports is None:
                verify_no_other_packets(self, timeout=1)
            elif isinstance(out_ports, list):
                if flood is True:
                    verify_packets(self, vxlan_pkt, out_ports)
                else:
                    verify_packets_any(self, vxlan_pkt, out_ports)
            else:
                verify_packet(self, vxlan_pkt, out_ports)
        finally:
            pass

    def EncapDecapTrafficTest(self,
                              customer1_ip=None,
                              customer2_ip=None,
                              customer1_smac=None, #inner smac
                              customer2_dmac=None, #inner dmac
                              tunnel2_smac=None, #underlay tunnel2 smac
                              tunnel2_dmac=None, #underlay tunnel2 nbr mac
                              tunnel1_term_ip=None,
                              tunnel1_ep_ip=None,
                              tunnel2_ep_ip=None,
                              tunnel2_lb_ip=None,
                              tunnel1_smac=None,
                              tunnel1_vni=None,
                              tunnel2_vni=None,
                              in_port=None,
                              out_ports=None,
                              flood=False):
        #dst_vtep->tunnel1 src_vtep->tunnel2
        #customer1-> [tunnel1] -> [tunnel2] -> customer2
        if customer1_ip is None:
            customer1_ip = self.vm_ip #100.100.1.1
        if customer2_ip is None:
            customer2_ip = '200.200.2.1'
        if customer1_smac is None:
            customer1_smac = '00:33:33:33:33:11'
        if customer2_dmac is None:
            customer2_dmac = self.inner_dmac2 #00:33:33:33:33:44
        if tunnel1_vni is None:
            tunnel1_vni = self.vni
        if tunnel2_vni is None:
            tunnel2_vni = 3000
        if tunnel2_smac is None:
            tunnel2_smac = self.rmac #00:77:66:55:44:33
        if tunnel2_dmac is None: #tunnel2 underlay nbr mac
            tunnel2_dmac = self.underlay_neighbor_mac2
        if tunnel1_term_ip is None:
            tunnel1_term_ip = self.my_lb_ip #10.10.10.10
        if tunnel2_lb_ip is None:
            tunnel2_lb_ip = '20.20.20.10'
        if tunnel1_ep_ip is None:
            tunnel1_ep_ip = self.tunnel_ip #10.10.10.1
        if tunnel2_ep_ip is None:
            tunnel2_ep_ip = '20.20.20.1'
        if tunnel1_smac is None:
            tunnel1_smac = '00:77:66:55:44:33'
        if in_port is None:
            in_port = self.devports[0]
        # if out ports is none, then we expect the packet to be dropped
        try:
            print("Sending VxLAN pkt [Outer:[DstIP:{}/SrcIP:{}/VNI:{} (Inner:(DstMac:{}/SrcMac:{}/DstIP:{}/SrcIP:{}))] on port {}\
-> Terminate Tunnel -> ReEncap -> [Outer:[DstIP:{}/SrcIP:{}/VNI:{} (Inner:(DstMac:{}/SrcMac:{}/DstIP:{}/SrcIP:{}))]\
from port {}".format(tunnel1_term_ip, tunnel1_ep_ip, tunnel1_vni, self.rmac, customer1_smac, customer2_ip,\
customer1_ip, in_port, tunnel2_ep_ip, tunnel2_lb_ip, tunnel2_vni, customer2_dmac, self.default_rmac, customer2_ip, customer1_ip, out_ports))
            ingress_inner_pkt = simple_tcp_packet(
                eth_dst=self.rmac, #'00:77:66:55:44:33'
                eth_src=customer1_smac,
                ip_dst=customer2_ip,
                ip_src=customer1_ip,
                ip_id=108,
                ip_ttl=63)
            ingress_vxlan_pkt = simple_vxlan_packet(
                eth_dst=tunnel1_smac,
                eth_src=self.underlay_neighbor_mac,
                ip_id=0,
                ip_dst=tunnel1_term_ip,
                ip_src=tunnel1_ep_ip,
                ip_ttl=64,
                ip_flags=0x2,
                udp_sport=11638,
                with_udp_chksum=False,
                vxlan_vni=tunnel1_vni,
                inner_frame=ingress_inner_pkt)
            egress_inner_pkt = simple_tcp_packet(
                eth_dst=customer2_dmac,
                eth_src=self.default_rmac,
                ip_dst=customer2_ip,
                ip_src=customer1_ip,
                ip_id=108,
                ip_ttl=62)
            egress_vxlan_pkt = mask.Mask(simple_vxlan_packet(
                eth_src=tunnel2_smac,
                eth_dst=tunnel2_dmac,
                ip_id=0,
                ip_dst=tunnel2_ep_ip,
                ip_src=tunnel2_lb_ip,
                ip_ttl=64,
                ip_flags=0x2,
                udp_sport=11638,
                with_udp_chksum=False,
                vxlan_vni=tunnel2_vni,
                inner_frame=egress_inner_pkt))
            mask_set_do_not_care_packet(egress_vxlan_pkt, UDP, "sport")
            send_packet(self, in_port, ingress_vxlan_pkt)
            if out_ports is None:
                verify_no_other_packets(self, timeout=1)
            elif isinstance(out_ports, list):
                if flood is True:
                    verify_packets(self, egress_vxlan_pkt, out_ports)
                else:
                    verify_packets_any(self, egress_vxlan_pkt, out_ports)
            else:
                verify_packet(self, egress_vxlan_pkt, out_ports)
        finally:
            pass


###############################################################################

@group('tunnel')
class OverlayVrfMacTest(ApiHelper):
    '''
        Test that verifies if inner smac is taken from the overlay vrf and not
        from the device mac.
    '''
    def setUp(self):
        self.configure()
        if (self.client.is_feature_enable(SWITCH_FEATURE_IPV4_TUNNEL) == 0):
            print("IPv4 tunnel feature not enabled, skipping")
            return
        if not self.client.is_feature_enable(SWITCH_FEATURE_TUNNEL_ENCAP):
            print("Tunnel encap not enabled, skipping")
            return
        if (self.client.is_feature_enable(SWITCH_FEATURE_VXLAN) == 0):
            print("VXLAN feature not enabled, skipping")
            return
        print()

        # underlay config input
        self.uvrf = self.add_vrf(self.device, id=100, src_mac=self.rmac)
        self.vm1_ip4 = '100.100.1.1'
        self.customer_ip4 = '100.100.0.1'
        self.customer_ip6 = '1234:5678:9abc:def0:4422:1133:5577:0011'
        self.customer2_ip4 = '100.100.6.1'
        self.customer2_ip6 = '1234:5678:9abc:def0:4422:1133:5577:0022'
        self.vni = 2000
        self.my_lb_ip = '10.10.10.10'
        self.tunnel1_ip = '10.10.10.1'
        self.inner_smac = "00:22:44:66:88:AA"
        self.inner_dmac = "00:33:33:33:33:33"
        self.underlay_neighbor_mac1 = '00:11:11:11:11:11'

        self.attribute_set(self.device,
                           SWITCH_DEVICE_ATTR_INNER_SRC_MAC_FROM_OVERLAY_VRF,
                           True);

        # overlay configuration
        self.ovrf = self.add_vrf(self.device, id = 200, src_mac=self.inner_smac)
        self.tunnel_type = SWITCH_TUNNEL_ATTR_TYPE_VXLAN
        self.term_type = SWITCH_TUNNEL_TERM_ATTR_TERMINATION_TYPE_P2MP
        self.encap_ttl_mode = SWITCH_TUNNEL_ATTR_ENCAP_TTL_MODE_PIPE_MODEL
        self.decap_ttl_mode = SWITCH_TUNNEL_ATTR_DECAP_TTL_MODE_PIPE_MODEL

        # create underlay loopback rif for tunnel
        self.urif_lb = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_LOOPBACK, vrf_handle=self.uvrf, src_mac=self.rmac)
        self.orif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port0, vrf_handle=self.ovrf, src_mac=self.rmac)

        # Create route to tunnel ip 10.10.10.1
        self.urif1 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port1, vrf_handle=self.uvrf, src_mac=self.rmac)
        self.unhop1 = self.add_nexthop(self.device, handle=self.urif1, dest_ip=self.tunnel1_ip)

        self.uneighbor1 = self.add_neighbor(self.device,
            mac_address=self.underlay_neighbor_mac1,
            handle=self.urif1,
            dest_ip=self.tunnel1_ip)  # 10.10.10.1
        self.tunnel_route1 = self.add_route(self.device, ip_prefix="10.10.10.1",
            vrf_handle=self.uvrf, nexthop_handle=self.unhop1)

        # Encap configuration follows

        # create Encap/Decap mappers
        self.encap_tunnel_map = self.add_tunnel_mapper(self.device,
            type=SWITCH_TUNNEL_MAPPER_ATTR_TYPE_VRF_HANDLE_TO_VNI)
        self.decap_tunnel_map = self.add_tunnel_mapper(self.device,
            type=SWITCH_TUNNEL_MAPPER_ATTR_TYPE_VNI_TO_VRF_HANDLE)

        # create Encap/Decap mapper entries for ovrf
        self.encap_tunnel_map_entry = self.add_tunnel_mapper_entry(self.device,
            type=SWITCH_TUNNEL_MAPPER_ENTRY_ATTR_TYPE_VRF_HANDLE_TO_VNI,
            tunnel_mapper_handle=self.encap_tunnel_map,
            network_handle=self.ovrf,
            tunnel_vni=self.vni)
        self.decap_tunnel_map_entry = self.add_tunnel_mapper_entry(self.device,
            type=SWITCH_TUNNEL_MAPPER_ENTRY_ATTR_TYPE_VNI_TO_VRF_HANDLE,
            tunnel_mapper_handle=self.decap_tunnel_map,
            network_handle=self.ovrf,
            tunnel_vni=self.vni)

        ingress_mapper_list = [switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=self.encap_tunnel_map)]
        egress_mapper_list = [switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=self.decap_tunnel_map)]

        # create Tunnel
        self.tunnel = self.add_tunnel(self.device,
            type=self.tunnel_type,
            src_ip=self.my_lb_ip,
            ingress_mapper_handles=ingress_mapper_list,
            egress_mapper_handles=egress_mapper_list,
            encap_ttl_mode=self.encap_ttl_mode,
            decap_ttl_mode=self.decap_ttl_mode,
            underlay_rif_handle=self.urif_lb)

        # Decap configuration follows

        # Create tunnel_termination entry
        self.tunnel_term = self.add_tunnel_term(self.device,
            type=self.tunnel_type,
            vrf_handle=self.uvrf,
            tunnel_handle=self.tunnel,
            termination_type=self.term_type,
            dst_ip=self.my_lb_ip)

        # create tunnel nexthop for VM 100.100.1.1
        self.tunnel_nexthop = self.add_nexthop(self.device,
            type=SWITCH_NEXTHOP_ATTR_TYPE_TUNNEL,
            rw_type=SWITCH_NEXTHOP_ATTR_RW_TYPE_L3_VNI,
            handle=self.tunnel,
            dest_ip=self.tunnel1_ip,  # 10.10.10.1
            mac_address=self.inner_dmac,
            tunnel_vni=2000)
        # add route to tunnel_nexthop
        self.customer_route = self.add_route(self.device,
            ip_prefix=self.vm1_ip4,
            vrf_handle=self.ovrf,
            nexthop_handle=self.tunnel_nexthop)

        # Create route to customer from VM
        self.onhop = self.add_nexthop(self.device, handle=self.orif, dest_ip=self.customer_ip4)
        self.oneighbor = self.add_neighbor(self.device,
            mac_address='00:22:22:22:22:22',
            handle=self.orif,
            dest_ip=self.customer_ip4)  # 100.100.0.1
        self.customer_route = self.add_route(self.device, ip_prefix=self.customer_ip4,
            vrf_handle=self.ovrf, nexthop_handle=self.onhop)

    def runTest(self):
        if (self.client.is_feature_enable(SWITCH_FEATURE_IPV4_TUNNEL) == 0):
            return
        if not self.client.is_feature_enable(SWITCH_FEATURE_TUNNEL_ENCAP):
            return
        if (self.client.is_feature_enable(SWITCH_FEATURE_VXLAN) == 0):
            return
        try:
            self.OverlayVrfMacTest()
        finally:
            pass

    def tearDown(self):
        if self.client.is_feature_enable(SWITCH_FEATURE_IPV4_TUNNEL) or \
            self.client.is_feature_enable(SWITCH_FEATURE_TUNNEL_ENCAP):
            self.attribute_set(self.port2, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.attribute_set(self.device,
                           SWITCH_DEVICE_ATTR_INNER_SRC_MAC_FROM_OVERLAY_VRF,
                           False);
        self.cleanup()

    def OverlayVrfMacTest(self):
        print("OverlayVrfMacTest()")
        try:
            print("Sending packet from Access port 0 to VxLan port 1")
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst=self.vm1_ip4,
                ip_src=self.customer_ip4,
                ip_id=108,
                ip_ttl=64)
            inner_pkt = simple_tcp_packet(
                eth_dst=self.inner_dmac,  # 00:33:33:33:33:33
                eth_src=self.inner_smac,
                ip_dst=self.vm1_ip4,
                ip_src=self.customer_ip4,
                ip_id=108,
                ip_ttl=63)
            vxlan_pkt = mask.Mask(simple_vxlan_packet(
                eth_src='00:77:66:55:44:33',
                eth_dst=self.underlay_neighbor_mac1,  # 00:11:11:11:11:11
                ip_id=0,
                ip_src=self.my_lb_ip,
                ip_dst=self.tunnel1_ip,
                ip_ttl=64,
                ip_flags=0x2,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=inner_pkt))
            mask_set_do_not_care_packet(vxlan_pkt, UDP, "sport")
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, vxlan_pkt, self.devports[1])

        finally:
            pass

###############################################################################


@group('tunnel')
class L3IPv4TunnelTest(ApiHelper):
    '''
    Reach VM from C1 with vni 2000                       vlan subport 200 on p3
   --------------------  --------------------           --------------------
   | VM: 100.100.1.1  |  | VM: 100.100.2.1  |           | VM: 100.100.6.1  |
   | Host: 10.10.10.1 |  | Host: 10.10.10.2 |    |----  | Host: 10.10.10.6 | - subport
   --------------------  --------------------    |      --------------------
             |                    |              |      --------------------
      ----------------------------------------------    | VM: 100.100.3.1  |
      |    | p1 |          | p2 |            -- p3 | -- | Host: 10.10.10.3 | - ecmp
      |    ------          ------               p4 |    --------------------
      |      |               |                     |
      |  -----------------------                   |    --------------------
      |  | Tunnel: 10.10.10.10 |             -- p5 | -- | VM: 100.100.4.1  | - lag
      |  -----------------------                p6 |    | Host: 10.10.10.4 |
      |     |                 \                    |    --------------------
      |  ------               -------        -- p7 |
      |  | p0 |               | SVI |           p8 |    --------------------
      ---------------------------------------------- -- | VM: 100.100.5.1  | - ecmp_lag
            |                    |                      | Host: 10.10.10.5 |
    -------------------     -------------------         --------------------
    | C1: 100.100.0.1 |     | C2: 100.100.6.1 |
    -------------------     -------------------
    '''
    def setUp(self):
        self.configure()
        if (self.client.is_feature_enable(SWITCH_FEATURE_IPV4_TUNNEL) == 0):
            print("IPv4 tunnel feature not enabled, skipping")
            return
        if not self.client.is_feature_enable(SWITCH_FEATURE_TUNNEL_ENCAP):
            print("Tunnel encap not enabled, skipping")
            return
        if (self.client.is_feature_enable(SWITCH_FEATURE_VXLAN) == 0):
            print("VXLAN feature not enabled, skipping")
            return
        print()

        # underlay config input
        self.uvrf = self.add_vrf(self.device, id=100, src_mac=self.rmac)
        self.vm1_ip4 = '100.100.1.1'
        self.vm1_ip6 = '1234:5678:9abc:def0:4422:1133:5577:9911'
        self.vm2_ip4 = '100.100.2.1'
        self.vm2_ip6 = '1234:5678:9abc:def0:4422:1133:5577:9921'
        self.vm3_ip4 = '100.100.3.1'
        self.vm3_ip6 = '1234:5678:9abc:def0:4422:1133:5577:9931'
        self.vm4_ip4 = '100.100.4.1'
        self.vm4_ip6 = '1234:5678:9abc:def0:4422:1133:5577:9941'
        self.vm5_ip4 = '100.100.5.1'
        self.vm5_ip6 = '1234:5678:9abc:def0:4422:1133:5577:9951'
        self.vm6_ip4 = '100.100.6.1'
        self.vm6_ip6 = '1234:5678:9abc:def0:4422:1133:5577:9961'
        self.vm7_ip4 = '100.100.7.1'
        self.vm7_ip6 = '1234:5678:9abc:def0:4422:1133:5577:9971'
        self.vm8_ip4 = '100.100.8.1'
        self.vm8_ip6 = '1234:5678:9abc:def0:4422:1133:5577:9981'
        self.customer_ip4 = '100.100.0.1'
        self.customer_ip6 = '1234:5678:9abc:def0:4422:1133:5577:0011'
        self.customer2_ip4 = '100.100.6.1'
        self.customer2_ip6 = '1234:5678:9abc:def0:4422:1133:5577:0022'
        self.vni = 2000
        self.my_lb_ip = '10.10.10.10'
        self.tunnel1_ip = '10.10.10.1'
        self.tunnel2_ip = '10.10.10.2'
        self.tunnel3_ip = '10.10.10.3'
        self.tunnel4_ip = '10.10.10.4'
        self.tunnel5_ip = '10.10.10.5'
        self.tunnel6_ip = '10.10.10.6'
        self.inner_dmac = "00:33:33:33:33:33"
        self.underlay_neighbor_mac1 = '00:11:11:11:11:11'
        self.underlay_neighbor_mac2 = '00:11:11:11:11:22'
        self.underlay_neighbor_mac3 = '00:11:11:11:11:33'
        self.underlay_neighbor_mac4 = '00:11:11:11:11:44'
        self.underlay_neighbor_mac5 = '00:11:11:11:11:55'
        self.underlay_neighbor_mac6 = '00:11:11:11:11:66'
        self.underlay_neighbor_mac7 = '00:11:11:11:11:77'

        # overlay configuration
        self.ovrf = self.add_vrf(self.device, id = 200, src_mac=self.rmac)
        self.tunnel_type = SWITCH_TUNNEL_ATTR_TYPE_VXLAN
        self.term_type = SWITCH_TUNNEL_TERM_ATTR_TERMINATION_TYPE_P2MP
        self.encap_ttl_mode = SWITCH_TUNNEL_ATTR_ENCAP_TTL_MODE_PIPE_MODEL
        self.decap_ttl_mode = SWITCH_TUNNEL_ATTR_DECAP_TTL_MODE_PIPE_MODEL

        lag0 = self.add_lag(self.device)
        lag_port5 = self.add_lag_member(self.device, lag_handle=lag0, port_handle=self.port5)
        lag_port6 = self.add_lag_member(self.device, lag_handle=lag0, port_handle=self.port6)
        lag1 = self.add_lag(self.device)
        lag_port7 = self.add_lag_member(self.device, lag_handle=lag1, port_handle=self.port7)
        lag_port8 = self.add_lag_member(self.device, lag_handle=lag1, port_handle=self.port8)

        # create underlay loopback rif for tunnel
        self.urif_lb = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_LOOPBACK, vrf_handle=self.uvrf, src_mac=self.rmac)
        self.orif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port0, vrf_handle=self.ovrf, src_mac=self.rmac)

        # Create route to tunnel ip 10.10.10.1
        self.urif1 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port1, vrf_handle=self.uvrf, src_mac=self.rmac)
        self.unhop1 = self.add_nexthop(self.device, handle=self.urif1, dest_ip=self.tunnel1_ip)

        self.uneighbor1 = self.add_neighbor(self.device,
            mac_address=self.underlay_neighbor_mac1,
            handle=self.urif1,
            dest_ip=self.tunnel1_ip)  # 10.10.10.1
        self.tunnel_route1 = self.add_route(self.device, ip_prefix="10.10.10.1",
            vrf_handle=self.uvrf, nexthop_handle=self.unhop1)

        # Create route to tunnel ip 10.10.10.2
        self.urif2 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_VLAN, vlan_handle=self.vlan10, vrf_handle=self.uvrf, src_mac=self.rmac)
        vlan_mbr2 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port2)
        self.attribute_set(self.port2, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.unhop2 = self.add_nexthop(self.device, handle=self.urif2, dest_ip=self.tunnel2_ip)
        self.uneighbor = self.add_neighbor(self.device,
            mac_address=self.underlay_neighbor_mac2,
            handle=self.urif2,
            dest_ip=self.tunnel2_ip)  # 10.10.10.2
        self.tunnel_route2 = self.add_route(self.device, ip_prefix="10.10.10.2",
            vrf_handle=self.uvrf, nexthop_handle=self.unhop2)
        mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan10,
            mac_address=self.underlay_neighbor_mac2,
            destination_handle=self.port2)

        # Create route to tunnel ip 10.10.10.3
        self.urif3 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port3, vrf_handle=self.uvrf, src_mac=self.rmac)
        self.uneighbor3 = self.add_neighbor(self.device,
            mac_address=self.underlay_neighbor_mac3,
            handle=self.urif3, dest_ip="11.11.11.1")
        self.unhop3 = self.add_nexthop(self.device, handle=self.urif3, dest_ip="11.11.11.1")

        self.urif4 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port4, vrf_handle=self.uvrf, src_mac=self.rmac)
        self.uneighbor4 = self.add_neighbor(self.device,
            mac_address=self.underlay_neighbor_mac4,
            handle=self.urif4, dest_ip="11.11.11.2")
        self.unhop4 = self.add_nexthop(self.device, handle=self.urif4, dest_ip="11.11.11.2")

        ecmp0 = self.add_ecmp(self.device)
        ecmp_member01 = self.add_ecmp_member(self.device, nexthop_handle=self.unhop3, ecmp_handle=ecmp0)
        ecmp_member02 = self.add_ecmp_member(self.device, nexthop_handle=self.unhop4, ecmp_handle=ecmp0)
        self.tunnel_route3 = self.add_route(self.device, ip_prefix="10.10.10.3",
            vrf_handle=self.uvrf, nexthop_handle=ecmp0)

        # Create route to tunnel ip 10.10.10.4
        self.urif5 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=lag0, vrf_handle=self.uvrf, src_mac=self.rmac)
        self.unhop5 = self.add_nexthop(self.device, handle=self.urif5, dest_ip=self.tunnel4_ip)
        self.uneighbor5 = self.add_neighbor(self.device,
            mac_address=self.underlay_neighbor_mac5,
            handle=self.urif5, dest_ip=self.tunnel4_ip)
        self.tunnel_route4 = self.add_route(self.device, ip_prefix="10.10.10.4",
            vrf_handle=self.uvrf, nexthop_handle=self.unhop5)

        # Create route to tunnel ip 10.10.10.5
        self.urif6 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=lag1, vrf_handle=self.uvrf, src_mac=self.rmac)
        self.uneighbor6 = self.add_neighbor(self.device,
            mac_address=self.underlay_neighbor_mac6,
            handle=self.urif6, dest_ip="12.11.11.1")
        self.unhop6 = self.add_nexthop(self.device, handle=self.urif6, dest_ip="12.11.11.1")

        ecmp1 = self.add_ecmp(self.device)
        ecmp_member11 = self.add_ecmp_member(self.device, nexthop_handle=self.unhop6, ecmp_handle=ecmp1)
        self.tunnel_route5 = self.add_route(self.device, ip_prefix="10.10.10.5",
            vrf_handle=self.uvrf, nexthop_handle=ecmp1)

        # Create route to tunnel ip 10.10.10.6
        self.urif7 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_SUB_PORT, port_handle=self.port3, outer_vlan_id=200, vrf_handle=self.uvrf, src_mac=self.rmac)
        self.unhop7 = self.add_nexthop(self.device, handle=self.urif7, dest_ip=self.tunnel6_ip)
        self.uneighbor7 = self.add_neighbor(self.device,
            mac_address=self.underlay_neighbor_mac7,
            handle=self.urif7,
            dest_ip=self.tunnel6_ip)  # 10.10.10.6
        self.tunnel_route7 = self.add_route(self.device, ip_prefix="10.10.10.6",
            vrf_handle=self.uvrf, nexthop_handle=self.unhop7)

        # Create overlay ECMP
        self.tunnel_ecmp1 = self.add_ecmp(self.device)
        self.tunnel_ecmp2 = self.add_ecmp(self.device)

        # Encap configuration follows

        # create Encap/Decap mappers
        self.encap_tunnel_map = self.add_tunnel_mapper(self.device,
            type=SWITCH_TUNNEL_MAPPER_ATTR_TYPE_VRF_HANDLE_TO_VNI)
        self.decap_tunnel_map = self.add_tunnel_mapper(self.device,
            type=SWITCH_TUNNEL_MAPPER_ATTR_TYPE_VNI_TO_VRF_HANDLE)

        # create Encap/Decap mapper entries for ovrf
        self.encap_tunnel_map_entry = self.add_tunnel_mapper_entry(self.device,
            type=SWITCH_TUNNEL_MAPPER_ENTRY_ATTR_TYPE_VRF_HANDLE_TO_VNI,
            tunnel_mapper_handle=self.encap_tunnel_map,
            network_handle=self.ovrf,
            tunnel_vni=self.vni)
        self.decap_tunnel_map_entry = self.add_tunnel_mapper_entry(self.device,
            type=SWITCH_TUNNEL_MAPPER_ENTRY_ATTR_TYPE_VNI_TO_VRF_HANDLE,
            tunnel_mapper_handle=self.decap_tunnel_map,
            network_handle=self.ovrf,
            tunnel_vni=self.vni)

        ingress_mapper_list = [switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=self.encap_tunnel_map)]
        egress_mapper_list = [switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=self.decap_tunnel_map)]

        # create Tunnel
        self.tunnel = self.add_tunnel(self.device,
            type=self.tunnel_type,
            src_ip=self.my_lb_ip,
            ingress_mapper_handles=ingress_mapper_list,
            egress_mapper_handles=egress_mapper_list,
            encap_ttl_mode=self.encap_ttl_mode,
            decap_ttl_mode=self.decap_ttl_mode,
            underlay_rif_handle=self.urif_lb)

        # Decap configuration follows

        # Create tunnel_termination entry
        self.tunnel_term = self.add_tunnel_term(self.device,
            type=self.tunnel_type,
            vrf_handle=self.uvrf,
            tunnel_handle=self.tunnel,
            termination_type=self.term_type,
            dst_ip=self.my_lb_ip)

        # create tunnel nexthop for VM 100.100.1.1
        self.tunnel_nexthop = self.add_nexthop(self.device,
            type=SWITCH_NEXTHOP_ATTR_TYPE_TUNNEL,
            rw_type=SWITCH_NEXTHOP_ATTR_RW_TYPE_L3_VNI,
            handle=self.tunnel,
            dest_ip=self.tunnel1_ip,  # 10.10.10.1
            mac_address=self.inner_dmac,
            tunnel_vni=2000)
        self.tunnel_ecmp1_member1 = self.add_ecmp_member(self.device, nexthop_handle=self.tunnel_nexthop,
                ecmp_handle=self.tunnel_ecmp1)
        # add route to tunnel_nexthop
        self.customer_route = self.add_route(self.device,
            ip_prefix=self.vm1_ip4,
            vrf_handle=self.ovrf,
            nexthop_handle=self.tunnel_nexthop)
        self.customer_route = self.add_route(self.device,
            ip_prefix=self.vm1_ip6,
            vrf_handle=self.ovrf,
            nexthop_handle=self.tunnel_nexthop)

        # create tunnel nexthop for VM 100.100.2.1
        self.tunnel_nexthop = self.add_nexthop(self.device,
            type=SWITCH_NEXTHOP_ATTR_TYPE_TUNNEL,
            rw_type=SWITCH_NEXTHOP_ATTR_RW_TYPE_L3_VNI,
            handle=self.tunnel,
            dest_ip=self.tunnel2_ip,  # 10.10.10.2
            mac_address=self.inner_dmac,
            tunnel_vni=2000)
        self.tunnel_ecmp1_member2 = self.add_ecmp_member(self.device, nexthop_handle=self.tunnel_nexthop,
                ecmp_handle=self.tunnel_ecmp1)
        # add route to tunnel_nexthop
        self.customer_route = self.add_route(self.device,
            ip_prefix=self.vm2_ip4,
            vrf_handle=self.ovrf,
            nexthop_handle=self.tunnel_nexthop)
        self.customer_route = self.add_route(self.device,
            ip_prefix=self.vm2_ip6,
            vrf_handle=self.ovrf,
            nexthop_handle=self.tunnel_nexthop)

        # create tunnel nexthop for VM 100.100.3.1
        self.tunnel_nexthop = self.add_nexthop(self.device,
            type=SWITCH_NEXTHOP_ATTR_TYPE_TUNNEL,
            rw_type=SWITCH_NEXTHOP_ATTR_RW_TYPE_L3_VNI,
            handle=self.tunnel,
            dest_ip=self.tunnel3_ip,  # 10.10.10.3
            mac_address=self.inner_dmac,
            tunnel_vni=2000)
        self.tunnel_ecmp2_member1 = self.add_ecmp_member(self.device, nexthop_handle=self.tunnel_nexthop,
                ecmp_handle=self.tunnel_ecmp2)
        # add route to tunnel_nexthop
        self.customer_route = self.add_route(self.device,
            ip_prefix=self.vm3_ip4,
            vrf_handle=self.ovrf,
            nexthop_handle=self.tunnel_nexthop)
        self.customer_route = self.add_route(self.device,
            ip_prefix=self.vm3_ip6,
            vrf_handle=self.ovrf,
            nexthop_handle=self.tunnel_nexthop)

        # create tunnel nexthop for VM 100.100.4.1
        self.tunnel_nexthop = self.add_nexthop(self.device,
            type=SWITCH_NEXTHOP_ATTR_TYPE_TUNNEL,
            rw_type=SWITCH_NEXTHOP_ATTR_RW_TYPE_L3_VNI,
            handle=self.tunnel,
            dest_ip=self.tunnel4_ip,  # 10.10.10.4
            mac_address=self.inner_dmac,
            tunnel_vni=2000)
        # add route to tunnel_nexthop
        self.customer_route = self.add_route(self.device,
            ip_prefix=self.vm4_ip4,
            vrf_handle=self.ovrf,
            nexthop_handle=self.tunnel_nexthop)
        self.customer_route = self.add_route(self.device,
            ip_prefix=self.vm4_ip6,
            vrf_handle=self.ovrf,
            nexthop_handle=self.tunnel_nexthop)

        # create tunnel nexthop for VM 100.100.5.1
        self.tunnel_nexthop = self.add_nexthop(self.device,
            type=SWITCH_NEXTHOP_ATTR_TYPE_TUNNEL,
            rw_type=SWITCH_NEXTHOP_ATTR_RW_TYPE_L3_VNI,
            handle=self.tunnel,
            dest_ip=self.tunnel5_ip,  # 10.10.10.5
            mac_address=self.inner_dmac,
            tunnel_vni=2000)
        self.tunnel_ecmp2_member2 = self.add_ecmp_member(self.device, nexthop_handle=self.tunnel_nexthop,
                ecmp_handle=self.tunnel_ecmp2)
        # add route to tunnel_nexthop
        self.customer_route = self.add_route(self.device,
            ip_prefix=self.vm5_ip4,
            vrf_handle=self.ovrf,
            nexthop_handle=self.tunnel_nexthop)
        self.customer_route = self.add_route(self.device,
            ip_prefix=self.vm5_ip6,
            vrf_handle=self.ovrf,
            nexthop_handle=self.tunnel_nexthop)

        # create tunnel nexthop for VM 100.100.6.1
        self.tunnel_nexthop = self.add_nexthop(self.device,
            type=SWITCH_NEXTHOP_ATTR_TYPE_TUNNEL,
            rw_type=SWITCH_NEXTHOP_ATTR_RW_TYPE_L3_VNI,
            handle=self.tunnel,
            dest_ip=self.tunnel6_ip,  # 10.10.10.6
            mac_address=self.inner_dmac,
            tunnel_vni=2000)
        # add route to tunnel_nexthop
        self.customer_route = self.add_route(self.device,
            ip_prefix=self.vm6_ip4,
            vrf_handle=self.ovrf,
            nexthop_handle=self.tunnel_nexthop)
        self.customer_route = self.add_route(self.device,
            ip_prefix=self.vm6_ip6,
            vrf_handle=self.ovrf,
            nexthop_handle=self.tunnel_nexthop)

        # Create route to customer from VM
        self.onhop = self.add_nexthop(self.device, handle=self.orif, dest_ip=self.customer_ip4)
        self.oneighbor = self.add_neighbor(self.device,
            mac_address='00:22:22:22:22:22',
            handle=self.orif,
            dest_ip=self.customer_ip4)  # 100.100.0.1
        self.customer_route = self.add_route(self.device, ip_prefix=self.customer_ip4,
            vrf_handle=self.ovrf, nexthop_handle=self.onhop)
        self.onhop = self.add_nexthop(self.device, handle=self.orif, dest_ip=self.customer_ip6)
        self.oneighbor = self.add_neighbor(self.device,
            mac_address='00:22:22:22:22:22',
            handle=self.orif,
            dest_ip=self.customer_ip6)  # 100.100.0.1
        self.customer_route = self.add_route(self.device, ip_prefix=self.customer_ip6,
            vrf_handle=self.ovrf, nexthop_handle=self.onhop)

        # Add customer routes pointing to overaly tunnel ecmps
        customer_route = self.add_route(self.device,
                ip_prefix=self.vm7_ip4,
                vrf_handle=self.ovrf,
                nexthop_handle=self.tunnel_ecmp1)
        customer_route = self.add_route(self.device,
                ip_prefix=self.vm7_ip6,
                vrf_handle=self.ovrf,
                nexthop_handle=self.tunnel_ecmp1)
        customer_route = self.add_route(self.device,
                ip_prefix=self.vm8_ip4,
                vrf_handle=self.ovrf,
                nexthop_handle=self.tunnel_ecmp2)
        customer_route = self.add_route(self.device,
                ip_prefix=self.vm8_ip6,
                vrf_handle=self.ovrf,
                nexthop_handle=self.tunnel_ecmp2)

    def runTest(self):
        if (self.client.is_feature_enable(SWITCH_FEATURE_IPV4_TUNNEL) == 0):
            return
        if not self.client.is_feature_enable(SWITCH_FEATURE_TUNNEL_ENCAP):
            return
        if (self.client.is_feature_enable(SWITCH_FEATURE_VXLAN) == 0):
            return
        try:
            self.L3IntfIPv4Test()
            self.L3IntfIPv6Test()
            self.L3SubPortIPv4Test()
            self.L3SubPortIPv6Test()
            self.SVITunnelIPv4Test()
            self.SVITunnelIPv6Test()
            self.ECMPTunnelIPv4Test()
            self.ECMPTunnelIPv6Test()
            self.LAGTunnelIPv4Test()
            self.LAGTunnelIPv6Test()
            self.EcmpLagTunnelIPv4Test()
            self.EcmpLagTunnelIPv6Test()
            self.OverlayEcmpTunnelTest(distribution=True)
            self.OverlayEcmpTunnelTest(ip_version=6, distribution=True)
            self.OverlayUnderlayEcmpTunnelTest(distribution=True)
            self.OverlayUnderlayEcmpTunnelTest(ip_version=6, distribution=True)
            self.SVIIngressTunnelTests()
        finally:
            pass

    def OverlayUnderlayEcmpTunnelTest(self, ip_version=4, port_list=[], distribution=False):
        print("OverlayEcmpTunnelTest(), ip_version:{}".format(ip_version))
        if ip_version == 4:
            dst_ip=self.vm8_ip4
            src_ip=self.customer_ip4
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst=dst_ip,
                ip_src=src_ip,
                ip_id=108,
                ip_ttl=64)
            inner_pkt = simple_tcp_packet(
                eth_dst=self.inner_dmac,  # 00:33:33:33:33:33
                eth_src=self.default_rmac,
                ip_dst=dst_ip,
                ip_src=src_ip,
                ip_id=108,
                ip_ttl=63)
        elif ip_version == 6:
            dst_ip=self.vm8_ip6
            src_ip=self.customer_ip6
            pkt = simple_tcpv6_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ipv6_dst=dst_ip,
                ipv6_src=src_ip,
                ipv6_hlim=64)
            inner_pkt = simple_tcpv6_packet(
                eth_dst=self.inner_dmac,  # 00:33:33:33:33:33
                eth_src=self.default_rmac,
                ipv6_dst=dst_ip,
                ipv6_src=src_ip,
                ipv6_hlim=63)
        else:
            print("OverlayEcmpTunnelTest() ip_version:{} invalid, skipping test".format(ip_version))
            return

        if not port_list:
            port_list = [self.devports[3], self.devports[4], self.devports[7], self.devports[8]]
        if distribution is True:
            print("Running Distribution Tests")
        try:
            print("Sending packets from Access port 0 to VxLan ports {}".format(port_list))
            count = [0] * len(port_list)
            max_itrs = 1
            if distribution is True:
                max_itrs = 120
            if ip_version==4:
                src_ip = int(binascii.hexlify(socket.inet_aton(self.customer_ip4)), 16)
            else:
                src_ip = int(binascii.hexlify(socket.inet_pton(socket.AF_INET6, self.customer_ip6)), 16)
            for i in range(0, max_itrs):
                if ip_version == 4:
                    src_ip_addr = socket.inet_ntoa(
                    binascii.unhexlify(format(src_ip, 'x').zfill(8)))
                    pkt[IP].src=src_ip_addr
                    inner_pkt[IP].src=src_ip_addr
                else:
                    src_ip_addr = socket.inet_ntop(socket.AF_INET6,
                    binascii.unhexlify(format(src_ip, 'x').zfill(16)))
                    pkt[IPv6].src=src_ip_addr
                    inner_pkt[IPv6].src=src_ip_addr
                exp_vxlan_pkt = []
                if self.devports[3] in port_list:
                    vxlan_pkt1 = mask.Mask(simple_vxlan_packet(
                        eth_src='00:77:66:55:44:33',
                        eth_dst=self.underlay_neighbor_mac3,
                        ip_id=0,
                        ip_src=self.my_lb_ip,
                        ip_dst=self.tunnel3_ip,
                        ip_ttl=64,
                        ip_flags=0x2,
                        with_udp_chksum=False,
                        vxlan_vni=self.vni,
                        inner_frame=inner_pkt))
                    mask_set_do_not_care_packet(vxlan_pkt1, UDP, "sport")
                    exp_vxlan_pkt.append(vxlan_pkt1)
                if self.devports[4] in port_list:
                    vxlan_pkt2 = mask.Mask(simple_vxlan_packet(
                        eth_src='00:77:66:55:44:33',
                        eth_dst=self.underlay_neighbor_mac4,
                        ip_id=0,
                        ip_src=self.my_lb_ip,
                        ip_dst=self.tunnel3_ip,
                        ip_ttl=64,
                        ip_flags=0x2,
                        with_udp_chksum=False,
                        vxlan_vni=self.vni,
                        inner_frame=inner_pkt))
                    mask_set_do_not_care_packet(vxlan_pkt2, UDP, "sport")
                    exp_vxlan_pkt.append(vxlan_pkt2)
                if self.devports[7] in port_list or self.devports[8] in port_list:
                    vxlan_pkt2 = mask.Mask(simple_vxlan_packet(
                        eth_src='00:77:66:55:44:33',
                        eth_dst=self.underlay_neighbor_mac6,
                        ip_id=0,
                        ip_src=self.my_lb_ip,
                        ip_dst=self.tunnel5_ip,
                        ip_ttl=64,
                        ip_flags=0x2,
                        with_udp_chksum=False,
                        vxlan_vni=self.vni,
                        inner_frame=inner_pkt))
                    mask_set_do_not_care_packet(vxlan_pkt2, UDP, "sport")
                    exp_vxlan_pkt.append(vxlan_pkt2)
                send_packet(self, self.devports[0], pkt)
                rcv_idx = verify_any_packet_on_ports_list(
                    self, exp_vxlan_pkt, [port_list], timeout=1, no_flood=True)
                self.assertTrue((len(rcv_idx) == 1 and len(rcv_idx[0]) == 1), "Packet received on more than one ecmp paths")
                count[rcv_idx[0].pop()] += 1
                src_ip += 1
            print("Packet distribution with {} packets on ports {} = {}".format(max_itrs, port_list, count))
            if distribution is True:
                for c in count:
                    self.assertTrue((c >= ((max_itrs / len(port_list)) * 0.5)),
                                    "Ecmp path is not equally balanced")
        finally:
            pass

    def OverlayEcmpTunnelTest(self, ip_version=4, port_list=[], distribution=False):
        print("OverlayEcmpTunnelTest(), ip_version:{}".format(ip_version))
        if ip_version == 4:
            dst_ip=self.vm7_ip4
            src_ip=self.customer_ip4
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst=dst_ip,
                ip_src=src_ip,
                ip_id=108,
                ip_ttl=64)
            inner_pkt = simple_tcp_packet(
                eth_dst=self.inner_dmac,  # 00:33:33:33:33:33
                eth_src=self.default_rmac,
                ip_dst=dst_ip,
                ip_src=src_ip,
                ip_id=108,
                ip_ttl=63)
        elif ip_version == 6:
            dst_ip=self.vm7_ip6
            src_ip=self.customer_ip6
            pkt = simple_tcpv6_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ipv6_dst=dst_ip,
                ipv6_src=src_ip,
                ipv6_hlim=64)
            inner_pkt = simple_tcpv6_packet(
                eth_dst=self.inner_dmac,  # 00:33:33:33:33:33
                eth_src=self.default_rmac,
                ipv6_dst=dst_ip,
                ipv6_src=src_ip,
                ipv6_hlim=63)
        else:
            print("OverlayEcmpTunnelTest() ip_version:{} invalid, skipping test".format(ip_version))
            return

        if not port_list:
            port_list = [self.devports[1], self.devports[2]]
        if distribution is True:
            print("Running Distribution Tests")
        try:
            print("Sending packets from Access port 0 to VxLan ports {}".format(port_list))
            count = [0] * len(port_list)
            max_itrs = 1
            if distribution is True:
                max_itrs = 40
            if ip_version==4:
                src_ip = int(binascii.hexlify(socket.inet_aton(self.customer_ip4)), 16)
            else:
                src_ip = int(binascii.hexlify(socket.inet_pton(socket.AF_INET6, self.customer_ip6)), 16)
            for i in range(0, max_itrs):
                if ip_version == 4:
                    src_ip_addr = socket.inet_ntoa(
                    binascii.unhexlify(format(src_ip, 'x').zfill(8)))
                    pkt[IP].src=src_ip_addr
                    inner_pkt[IP].src=src_ip_addr
                else:
                    src_ip_addr = socket.inet_ntop(socket.AF_INET6,
                    binascii.unhexlify(format(src_ip, 'x').zfill(16)))
                    pkt[IPv6].src=src_ip_addr
                    inner_pkt[IPv6].src=src_ip_addr
                exp_vxlan_pkt = []
                if self.devports[1] in port_list:
                    vxlan_pkt1 = mask.Mask(simple_vxlan_packet(
                        eth_src='00:77:66:55:44:33',
                        eth_dst=self.underlay_neighbor_mac1,
                        ip_id=0,
                        ip_src=self.my_lb_ip,
                        ip_dst=self.tunnel1_ip,
                        ip_ttl=64,
                        ip_flags=0x2,
                        with_udp_chksum=False,
                        vxlan_vni=self.vni,
                        inner_frame=inner_pkt))
                    mask_set_do_not_care_packet(vxlan_pkt1, UDP, "sport")
                    exp_vxlan_pkt.append(vxlan_pkt1)
                if self.devports[2] in port_list:
                    vxlan_pkt2 = mask.Mask(simple_vxlan_packet(
                        eth_src='00:77:66:55:44:33',
                        eth_dst=self.underlay_neighbor_mac2,
                        ip_id=0,
                        ip_src=self.my_lb_ip,
                        ip_dst=self.tunnel2_ip,
                        ip_ttl=64,
                        ip_flags=0x2,
                        with_udp_chksum=False,
                        vxlan_vni=self.vni,
                        inner_frame=inner_pkt))
                    mask_set_do_not_care_packet(vxlan_pkt2, UDP, "sport")
                    exp_vxlan_pkt.append(vxlan_pkt2)
                send_packet(self, self.devports[0], pkt)
                rcv_idx = verify_any_packet_on_ports_list(
                    self, exp_vxlan_pkt, [port_list], timeout=1, no_flood=True)
                self.assertTrue((len(rcv_idx) == 1 and len(rcv_idx[0]) == 1), "Packet received on more than one ecmp paths")
                count[rcv_idx[0].pop()] += 1
                src_ip += 1
            print("Packet distribution with {} packets on ports {} = {}".format(max_itrs, port_list, count))
            if distribution is True:
                for c in count:
                    self.assertTrue((c >= ((max_itrs / len(port_list)) * 0.5)),
                                    "Ecmp path is not equally balanced")
        finally:
            pass

    def SVIIngressTunnelTests(self):
        #Create overlay rif in Vlan20 with members - port9, port10(trunk). lag2 (port11/port12)
        self.rif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_VLAN, vlan_handle=self.vlan20, vrf_handle=self.ovrf, src_mac=self.rmac)
        lag2 = self.add_lag(self.device)
        lag2_port11 = self.add_lag_member(self.device, lag_handle=lag2, port_handle=self.port11)
        lag2_port12 = self.add_lag_member(self.device, lag_handle=lag2, port_handle=self.port12)
        vlan_mbr = self.add_vlan_member(self.device, vlan_handle=self.vlan20, member_handle=self.port9)
        self.attribute_set(self.port9, SWITCH_PORT_ATTR_PORT_VLAN_ID, 20)
        vlan_mbr = self.add_vlan_member(self.device, vlan_handle=self.vlan20, member_handle=self.port10, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
        vlan_mbr = self.add_vlan_member(self.device, vlan_handle=self.vlan20, member_handle=lag2)
        self.attribute_set(lag2, SWITCH_LAG_ATTR_PORT_VLAN_ID, 20)

        try:
            #Send untagged packet on SVI access port 9 (ovrf)
            self.SVIIngressTunnelIPv4Test(ingress_port=self.devports[9], egress_ports=self.devports[1])
            self.SVIIngressTunnelIPv6Test(ingress_port=self.devports[9], egress_ports=self.devports[1])
            #Send tagged packet on SVI trunk port 10 (ovrf)
            self.SVIIngressTunnelIPv4Test(ingress_port=self.devports[10], tag_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED,
                                                vlan_id=20, egress_ports=self.devports[1])
            self.SVIIngressTunnelIPv6Test(ingress_port=self.devports[10], tag_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED,
                                                vlan_id=20, egress_ports=self.devports[1])
            #Send untagged packet on SVI lag port 11 (ovrf)
            print("Testing packet ingress on SVI Lag ports")
            self.SVIIngressTunnelIPv4Test(ingress_port=self.devports[11], egress_ports=self.devports[1])
            self.SVIIngressTunnelIPv6Test(ingress_port=self.devports[11], egress_ports=self.devports[1])
            self.SVIIngressTunnelIPv4Test(ingress_port=self.devports[12], egress_ports=self.devports[1])
            self.SVIIngressTunnelIPv6Test(ingress_port=self.devports[12], egress_ports=self.devports[1])
        finally:
            pass

        # Remove lag2 (port11/port12) from VLAN and create a RIF on top of lag
        self.cleanlast()
        self.rif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=lag2, vrf_handle=self.ovrf, src_mac=self.rmac)
        try:
            print("Testing packet ingress on L3 Lag ports")
            self.LagIngressTunnelIPv4Test(ingress_port=self.devports[11], egress_ports=self.devports[1])
            self.LagIngressTunnelIPv6Test(ingress_port=self.devports[11], egress_ports=self.devports[1])
            self.LagIngressTunnelIPv4Test(ingress_port=self.devports[12], egress_ports=self.devports[1])
            self.LagIngressTunnelIPv6Test(ingress_port=self.devports[12], egress_ports=self.devports[1])
        finally:
            # Remove L3 RIF
            self.cleanlast()
            self.attribute_set(lag2, SWITCH_LAG_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.port9, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.cleanlast() #vlan20 member port10
            self.cleanlast() #vlan20 member port9
            self.cleanlast() #lag2 lag member port12
            self.cleanlast() #lag2 lag member port11
            self.cleanlast() #lag2
            self.cleanlast() #Overlay SVI VLAN20

    def tearDown(self):
        if self.client.is_feature_enable(SWITCH_FEATURE_IPV4_TUNNEL) or \
            self.client.is_feature_enable(SWITCH_FEATURE_TUNNEL_ENCAP):
            self.attribute_set(self.port2, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.cleanup()

    def SVIIngressTunnelIPv4Test(self,
                                 tag_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_UNTAGGED,
                                 vlan_id=0,
                                 ingress_port=None,
                                 egress_ports=None):
        print("SVIAccessIngressTunnelIPv4Test() -Sending packet from {} port {} to VxLan port {}".format("Access" if tag_mode is SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_UNTAGGED else "Trunk", ingress_port, egress_ports))
        if ingress_port is None:
            print("Skipping test as no ingress port specified")
            return
        try:
            if tag_mode is SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED:
                pkt = simple_tcp_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src='00:22:22:22:22:22',
                    ip_dst=self.vm1_ip4,
                    ip_src=self.customer2_ip4,
                    ip_id=108,
                    dl_vlan_enable=True,
                    vlan_vid=vlan_id,
                    ip_ttl=64,
                    pktlen=104)
                inner_pkt = simple_tcp_packet(
                    eth_dst=self.inner_dmac,  # 00:33:33:33:33:33
                    eth_src=self.default_rmac,
                    ip_dst=self.vm1_ip4,
                    ip_src=self.customer2_ip4,
                    ip_id=108,
                    ip_ttl=63,
                    pktlen=100)
            else:
                pkt = simple_tcp_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src='00:22:22:22:22:22',
                    ip_dst=self.vm1_ip4,
                    ip_src=self.customer2_ip4,
                    ip_id=108,
                    ip_ttl=64)
                inner_pkt = simple_tcp_packet(
                    eth_dst=self.inner_dmac,  # 00:33:33:33:33:33
                    eth_src=self.default_rmac,
                    ip_dst=self.vm1_ip4,
                    ip_src=self.customer2_ip4,
                    ip_id=108,
                    ip_ttl=63)
            vxlan_pkt = mask.Mask(simple_vxlan_packet(
                eth_src='00:77:66:55:44:33',
                eth_dst=self.underlay_neighbor_mac1,  # 00:11:11:11:11:11
                ip_id=0,
                ip_src=self.my_lb_ip,
                ip_dst=self.tunnel1_ip,
                ip_ttl=64,
                ip_flags=0x2,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=inner_pkt))
            mask_set_do_not_care_packet(vxlan_pkt, UDP, "sport")
            send_packet(self, ingress_port, pkt)
            if egress_ports is None:
                verify_no_other_packets(self, timeout=1)
            elif isinstance(egress_ports, list):
                if flood is True:
                    verify_packets(self, vxlan_pkt, egress_ports)
                else:
                    verify_packets_any(self, vxlan_pkt, egress_ports)
            else:
                verify_packet(self, vxlan_pkt, egress_ports)
        finally:
            pass

    def SVIIngressTunnelIPv6Test(self,
                                 tag_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_UNTAGGED,
                                 vlan_id=0,
                                 ingress_port=None,
                                 egress_ports=None):
        print("SVIAccessIngressTunnelIPv6Test() -Sending packet from {} port {} to VxLan port {}".format("Access" if tag_mode is SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_UNTAGGED else "Trunk", ingress_port, egress_ports))
        if ingress_port is None:
            print("Skipping test as no ingress port specified")
            return

        try:
            if tag_mode is SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED:
                pkt = simple_tcpv6_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src='00:22:22:22:22:22',
                    ipv6_dst=self.vm1_ip6,
                    ipv6_src=self.customer_ip6,
                    ipv6_hlim=64)
                pkt = simple_tcpv6_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src='00:22:22:22:22:22',
                    ipv6_dst=self.vm1_ip6,
                    ipv6_src=self.customer2_ip6,
                    dl_vlan_enable=True,
                    vlan_vid=vlan_id,
                    ipv6_hlim=64,
                    pktlen=104)
                inner_pkt = simple_tcpv6_packet(
                    eth_dst=self.inner_dmac,  # 00:33:33:33:33:33
                    eth_src=self.default_rmac,
                    ipv6_dst=self.vm1_ip6,
                    ipv6_src=self.customer2_ip6,
                    ipv6_hlim=63,
                    pktlen=100)
            else:
                pkt = simple_tcpv6_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src='00:22:22:22:22:22',
                    ipv6_dst=self.vm1_ip6,
                    ipv6_src=self.customer2_ip6,
                    ipv6_hlim=64)
                inner_pkt = simple_tcpv6_packet(
                    eth_dst=self.inner_dmac,  # 00:33:33:33:33:33
                    eth_src=self.default_rmac,
                    ipv6_dst=self.vm1_ip6,
                    ipv6_src=self.customer2_ip6,
                    ipv6_hlim=63)
            vxlan_pkt = mask.Mask(simple_vxlan_packet(
                eth_src='00:77:66:55:44:33',
                eth_dst=self.underlay_neighbor_mac1,  # 00:11:11:11:11:11
                ip_id=0,
                ip_src=self.my_lb_ip,
                ip_dst=self.tunnel1_ip,
                ip_ttl=64,
                ip_flags=0x2,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=inner_pkt))
            mask_set_do_not_care_packet(vxlan_pkt, UDP, "sport")
            send_packet(self, ingress_port, pkt)
            if egress_ports is None:
                verify_no_other_packets(self, timeout=1)
            elif isinstance(egress_ports, list):
                if flood is True:
                    verify_packets(self, vxlan_pkt, egress_ports)
                else:
                    verify_packets_any(self, vxlan_pkt, egress_ports)
            else:
                verify_packet(self, vxlan_pkt, egress_ports)
        finally:
            pass

    def LagIngressTunnelIPv4Test(self,
                                 ingress_port=None,
                                 egress_ports=None):
        print("LagIngressTunnelIPv4Test() -Sending packet from Lag port {} to VxLan port {}".format(ingress_port, egress_ports))
        if ingress_port is None:
            print("Skipping test as no ingress port specified")
            return
        try:
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst=self.vm1_ip4,
                ip_src=self.customer2_ip4,
                ip_id=108,
                ip_ttl=64)
            inner_pkt = simple_tcp_packet(
                eth_dst=self.inner_dmac,  # 00:33:33:33:33:33
                eth_src=self.default_rmac,
                ip_dst=self.vm1_ip4,
                ip_src=self.customer2_ip4,
                ip_id=108,
                ip_ttl=63)
            vxlan_pkt = mask.Mask(simple_vxlan_packet(
                eth_src='00:77:66:55:44:33',
                eth_dst=self.underlay_neighbor_mac1,  # 00:11:11:11:11:11
                ip_id=0,
                ip_src=self.my_lb_ip,
                ip_dst=self.tunnel1_ip,
                ip_ttl=64,
                ip_flags=0x2,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=inner_pkt))
            mask_set_do_not_care_packet(vxlan_pkt, UDP, "sport")
            send_packet(self, ingress_port, pkt)
            if egress_ports is None:
                verify_no_other_packets(self, timeout=1)
            elif isinstance(egress_ports, list):
                if flood is True:
                    verify_packets(self, vxlan_pkt, egress_ports)
                else:
                    verify_packets_any(self, vxlan_pkt, egress_ports)
            else:
                verify_packet(self, vxlan_pkt, egress_ports)
        finally:
            pass

    def LagIngressTunnelIPv6Test(self,
                                 ingress_port=None,
                                 egress_ports=None):
        print("LagIngressTunnelIPv6Test() -Sending packet from Lag port {} to VxLan port {}".format(ingress_port, egress_ports))
        if ingress_port is None:
            print("Skipping test as no ingress port specified")
            return

        try:
            pkt = simple_tcpv6_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ipv6_dst=self.vm1_ip6,
                ipv6_src=self.customer2_ip6,
                ipv6_hlim=64)
            inner_pkt = simple_tcpv6_packet(
                eth_dst=self.inner_dmac,  # 00:33:33:33:33:33
                eth_src=self.default_rmac,
                ipv6_dst=self.vm1_ip6,
                ipv6_src=self.customer2_ip6,
                ipv6_hlim=63)
            vxlan_pkt = mask.Mask(simple_vxlan_packet(
                eth_src='00:77:66:55:44:33',
                eth_dst=self.underlay_neighbor_mac1,  # 00:11:11:11:11:11
                ip_id=0,
                ip_src=self.my_lb_ip,
                ip_dst=self.tunnel1_ip,
                ip_ttl=64,
                ip_flags=0x2,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=inner_pkt))
            mask_set_do_not_care_packet(vxlan_pkt, UDP, "sport")
            send_packet(self, ingress_port, pkt)
            if egress_ports is None:
                verify_no_other_packets(self, timeout=1)
            elif isinstance(egress_ports, list):
                if flood is True:
                    verify_packets(self, vxlan_pkt, egress_ports)
                else:
                    verify_packets_any(self, vxlan_pkt, egress_ports)
            else:
                verify_packet(self, vxlan_pkt, egress_ports)
        finally:
            pass

    def L3IntfIPv4Test(self):
        print("L3IntfIPv4Test()")
        try:
            print("Sending packet from Access port 0 to VxLan port 1")
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst=self.vm1_ip4,
                ip_src=self.customer_ip4,
                ip_id=108,
                ip_ttl=64)
            inner_pkt = simple_tcp_packet(
                eth_dst=self.inner_dmac,  # 00:33:33:33:33:33
                eth_src=self.default_rmac,
                ip_dst=self.vm1_ip4,
                ip_src=self.customer_ip4,
                ip_id=108,
                ip_ttl=63)
            vxlan_pkt = mask.Mask(simple_vxlan_packet(
                eth_src='00:77:66:55:44:33',
                eth_dst=self.underlay_neighbor_mac1,  # 00:11:11:11:11:11
                ip_id=0,
                ip_src=self.my_lb_ip,
                ip_dst=self.tunnel1_ip,
                ip_ttl=64,
                ip_flags=0x2,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=inner_pkt))
            mask_set_do_not_care_packet(vxlan_pkt, UDP, "sport")
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, vxlan_pkt, self.devports[1])

            print("Sending packet from Vxlan port 1 to Access port 0")
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src=self.inner_dmac,
                ip_dst=self.customer_ip4,
                ip_src=self.vm1_ip4,
                ip_id=108,
                ip_ttl=64)
            pkt2 = simple_tcp_packet(
                eth_dst='00:22:22:22:22:22',
                eth_src='00:77:66:55:44:33',
                ip_dst=self.customer_ip4,
                ip_src=self.vm1_ip4,
                ip_id=108,
                ip_ttl=63)
            vxlan_pkt = simple_vxlan_packet(
                eth_src=self.underlay_neighbor_mac1,
                eth_dst='00:77:66:55:44:33',
                ip_id=0,
                ip_dst=self.my_lb_ip,
                ip_src=self.tunnel1_ip,
                ip_ttl=64,
                ip_flags=0x2,
                udp_sport=11638,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=pkt)
            send_packet(self, self.devports[1], vxlan_pkt)
            verify_packet(self, pkt2, self.devports[0])
        finally:
            pass

    def L3IntfIPv6Test(self):
        print("L3IntfIPv6Test()")
        if (self.client.is_feature_enable(SWITCH_FEATURE_IPV6_TUNNEL) == 0):
            print("IPv6 tunnel feature not enabled, skipping")
            return
        try:
            print("Sending packet from Access port 0 to VxLan port 1")
            pkt = simple_tcpv6_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ipv6_dst=self.vm1_ip6,
                ipv6_src=self.customer_ip6,
                ipv6_hlim=64)
            inner_pkt = simple_tcpv6_packet(
                eth_dst=self.inner_dmac,  # 00:33:33:33:33:33
                eth_src=self.default_rmac,
                ipv6_dst=self.vm1_ip6,
                ipv6_src=self.customer_ip6,
                ipv6_hlim=63)
            vxlan_pkt = mask.Mask(simple_vxlan_packet(
                eth_src='00:77:66:55:44:33',
                eth_dst=self.underlay_neighbor_mac1,  # 00:11:11:11:11:11
                ip_id=0,
                ip_src=self.my_lb_ip,
                ip_dst=self.tunnel1_ip,
                ip_ttl=64,
                ip_flags=0x2,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=inner_pkt))
            mask_set_do_not_care_packet(vxlan_pkt, UDP, "sport")
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, vxlan_pkt, self.devports[1])

            print("Sending packet from Vxlan port 1 to Access port 0")
            pkt = simple_tcpv6_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src=self.inner_dmac,
                ipv6_dst=self.customer_ip6,
                ipv6_src=self.vm1_ip6,
                ipv6_hlim=64)
            pkt2 = simple_tcpv6_packet(
                eth_dst='00:22:22:22:22:22',
                eth_src='00:77:66:55:44:33',
                ipv6_dst=self.customer_ip6,
                ipv6_src=self.vm1_ip6,
                ipv6_hlim=63)
            vxlan_pkt = simple_vxlan_packet(
                eth_src=self.underlay_neighbor_mac1,
                eth_dst='00:77:66:55:44:33',
                ip_id=0,
                ip_dst=self.my_lb_ip,
                ip_src=self.tunnel1_ip,
                ip_ttl=64,
                ip_flags=0x2,
                udp_sport=11638,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=pkt)
            send_packet(self, self.devports[1], vxlan_pkt)
            verify_packet(self, pkt2, self.devports[0])
        finally:
            pass

    def L3SubPortIPv4Test(self):
        print("L3SubPortIPv4Test()")
        try:
            print("Sending packet from Access port 0 to VxLan sub port 3 vlan 200")
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst=self.vm6_ip4,
                ip_src=self.customer_ip4,
                ip_id=108,
                ip_ttl=64)
            inner_pkt = simple_tcp_packet(
                eth_dst=self.inner_dmac,  # 00:33:33:33:33:33
                eth_src=self.default_rmac,
                ip_dst=self.vm6_ip4,
                ip_src=self.customer_ip4,
                ip_id=108,
                ip_ttl=63)
            vxlan_pkt = mask.Mask(simple_vxlan_packet(
                eth_src='00:77:66:55:44:33',
                eth_dst=self.underlay_neighbor_mac7,  # 00:11:11:11:11:11
                ip_id=0,
                ip_src=self.my_lb_ip,
                ip_dst=self.tunnel6_ip,
                dl_vlan_enable=True,
                vlan_vid=200,
                ip_ttl=64,
                ip_flags=0x2,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=inner_pkt))
            mask_set_do_not_care_packet(vxlan_pkt, UDP, "sport")
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, vxlan_pkt, self.devports[3])

            print("Sending packet from Vxlan subport port 3 vlan 200 to Access port 0")
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src=self.inner_dmac,
                ip_dst=self.customer_ip4,
                ip_src=self.vm6_ip4,
                ip_id=108,
                ip_ttl=64)
            pkt2 = simple_tcp_packet(
                eth_dst='00:22:22:22:22:22',
                eth_src='00:77:66:55:44:33',
                ip_dst=self.customer_ip4,
                ip_src=self.vm6_ip4,
                ip_id=108,
                ip_ttl=63)
            vxlan_pkt = simple_vxlan_packet(
                eth_src=self.underlay_neighbor_mac7,
                eth_dst='00:77:66:55:44:33',
                ip_id=0,
                ip_dst=self.my_lb_ip,
                ip_src=self.tunnel6_ip,
                dl_vlan_enable=True,
                vlan_vid=200,
                ip_ttl=64,
                ip_flags=0x2,
                udp_sport=11638,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=pkt)
            send_packet(self, self.devports[3], vxlan_pkt)
            verify_packet(self, pkt2, self.devports[0])
        finally:
            pass

    def L3SubPortIPv6Test(self):
        print("L3SubPortIPv6Test()")
        if (self.client.is_feature_enable(SWITCH_FEATURE_IPV6_TUNNEL) == 0):
            print("IPv6 tunnel feature not enabled, skipping")
            return
        try:
            print("Sending packet from Access port 0 to VxLan sub port 3 vlan 200")
            pkt = simple_tcpv6_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ipv6_dst=self.vm6_ip6,
                ipv6_src=self.customer_ip6,
                ipv6_hlim=64)
            inner_pkt = simple_tcpv6_packet(
                eth_dst=self.inner_dmac,  # 00:33:33:33:33:33
                eth_src=self.default_rmac,
                ipv6_dst=self.vm6_ip6,
                ipv6_src=self.customer_ip6,
                ipv6_hlim=63)
            vxlan_pkt = mask.Mask(simple_vxlan_packet(
                eth_src='00:77:66:55:44:33',
                eth_dst=self.underlay_neighbor_mac7,  # 00:11:11:11:11:11
                ip_id=0,
                ip_src=self.my_lb_ip,
                ip_dst=self.tunnel6_ip,
                dl_vlan_enable=True,
                vlan_vid=200,
                ip_ttl=64,
                ip_flags=0x2,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=inner_pkt))
            mask_set_do_not_care_packet(vxlan_pkt, UDP, "sport")
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, vxlan_pkt, self.devports[3])

            print("Sending packet from Vxlan sub port 3 vlan 200 to Access port 0")
            pkt = simple_tcpv6_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src=self.inner_dmac,
                ipv6_dst=self.customer_ip6,
                ipv6_src=self.vm6_ip6,
                ipv6_hlim=64)
            pkt2 = simple_tcpv6_packet(
                eth_dst='00:22:22:22:22:22',
                eth_src='00:77:66:55:44:33',
                ipv6_dst=self.customer_ip6,
                ipv6_src=self.vm6_ip6,
                ipv6_hlim=63)
            vxlan_pkt = simple_vxlan_packet(
                eth_src=self.underlay_neighbor_mac7,
                eth_dst='00:77:66:55:44:33',
                ip_id=0,
                ip_dst=self.my_lb_ip,
                ip_src=self.tunnel6_ip,
                dl_vlan_enable=True,
                vlan_vid=200,
                ip_ttl=64,
                ip_flags=0x2,
                udp_sport=11638,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=pkt)
            send_packet(self, self.devports[3], vxlan_pkt)
            verify_packet(self, pkt2, self.devports[0])
        finally:
            pass

    def SVITunnelIPv4Test(self):
        print("SVITunnelIPv4Test()")
        try:
            print("Sending packet from Access port 0 to VxLan port 2")
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst=self.vm2_ip4,
                ip_src=self.customer_ip4,
                ip_id=108,
                ip_ttl=64)
            inner_pkt = simple_tcp_packet(
                eth_dst=self.inner_dmac,  # 00:33:33:33:33:33
                eth_src=self.default_rmac,
                ip_dst=self.vm2_ip4,
                ip_src=self.customer_ip4,
                ip_id=108,
                ip_ttl=63)
            vxlan_pkt = mask.Mask(simple_vxlan_packet(
                eth_src='00:77:66:55:44:33',
                eth_dst=self.underlay_neighbor_mac2,  # 00:11:11:11:11:22
                ip_id=0,
                ip_src=self.my_lb_ip,
                ip_dst=self.tunnel2_ip,
                ip_ttl=64,
                ip_flags=0x2,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=inner_pkt))
            mask_set_do_not_care_packet(vxlan_pkt, UDP, "sport")
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, vxlan_pkt, self.devports[2])

            print("Sending packet from Vxlan port 2 to Access port 0")
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src=self.inner_dmac,
                ip_dst=self.customer_ip4,
                ip_src=self.vm2_ip4,
                ip_id=108,
                ip_ttl=64)
            pkt2 = simple_tcp_packet(
                eth_dst='00:22:22:22:22:22',
                eth_src='00:77:66:55:44:33',
                ip_dst=self.customer_ip4,
                ip_src=self.vm2_ip4,
                ip_id=108,
                ip_ttl=63)
            vxlan_pkt = simple_vxlan_packet(
                eth_src=self.underlay_neighbor_mac2,
                eth_dst='00:77:66:55:44:33',
                ip_id=0,
                ip_dst=self.my_lb_ip,
                ip_src=self.tunnel2_ip,
                ip_ttl=64,
                ip_flags=0x2,
                udp_sport=11638,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=pkt)
            send_packet(self, self.devports[2], vxlan_pkt)
            verify_packet(self, pkt2, self.devports[0])
        finally:
            pass

    def SVITunnelIPv6Test(self):
        print("SVITunnelIPv6Test()")
        if (self.client.is_feature_enable(SWITCH_FEATURE_IPV6_TUNNEL) == 0):
            print("IPv6 tunnel feature not enabled, skipping")
            return
        try:
            print("Sending packet from Access port 0 to VxLan port 2")
            pkt = simple_tcpv6_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ipv6_dst=self.vm2_ip6,
                ipv6_src=self.customer_ip6,
                ipv6_hlim=64)
            inner_pkt = simple_tcpv6_packet(
                eth_dst=self.inner_dmac,  # 00:33:33:33:33:33
                eth_src=self.default_rmac,
                ipv6_dst=self.vm2_ip6,
                ipv6_src=self.customer_ip6,
                ipv6_hlim=63)
            vxlan_pkt = mask.Mask(simple_vxlan_packet(
                eth_src='00:77:66:55:44:33',
                eth_dst=self.underlay_neighbor_mac2,  # 00:11:11:11:11:22
                ip_id=0,
                ip_src=self.my_lb_ip,
                ip_dst=self.tunnel2_ip,
                ip_ttl=64,
                ip_flags=0x2,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=inner_pkt))
            mask_set_do_not_care_packet(vxlan_pkt, UDP, "sport")
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, vxlan_pkt, self.devports[2])

            print("Sending packet from Vxlan port 2 to Access port 0")
            pkt = simple_tcpv6_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src=self.inner_dmac,
                ipv6_dst=self.customer_ip6,
                ipv6_src=self.vm2_ip6,
                ipv6_hlim=64)
            pkt2 = simple_tcpv6_packet(
                eth_dst='00:22:22:22:22:22',
                eth_src='00:77:66:55:44:33',
                ipv6_dst=self.customer_ip6,
                ipv6_src=self.vm2_ip6,
                ipv6_hlim=63)
            vxlan_pkt = simple_vxlan_packet(
                eth_src=self.underlay_neighbor_mac2,
                eth_dst='00:77:66:55:44:33',
                ip_id=0,
                ip_dst=self.my_lb_ip,
                ip_src=self.tunnel2_ip,
                ip_ttl=64,
                ip_flags=0x2,
                udp_sport=11638,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=pkt)
            send_packet(self, self.devports[2], vxlan_pkt)
            verify_packet(self, pkt2, self.devports[0])
        finally:
            pass

    def ECMPTunnelIPv4Test(self):
        print("ECMPTunnelIPv4Test()")
        try:
            print("Sending packet from Access port 0 to VxLan port 3, 4")
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst=self.vm3_ip4,
                ip_src=self.customer_ip4,
                ip_id=108,
                ip_ttl=64)
            inner_pkt = simple_tcp_packet(
                eth_dst=self.inner_dmac,  # 00:33:33:33:33:33
                eth_src=self.default_rmac,
                ip_dst=self.vm3_ip4,
                ip_src=self.customer_ip4,
                ip_id=108,
                ip_ttl=63)
            vxlan_pkt1 = mask.Mask(simple_vxlan_packet(
                eth_src='00:77:66:55:44:33',
                eth_dst=self.underlay_neighbor_mac3,  # 00:11:11:11:11:33
                ip_id=0,
                ip_src=self.my_lb_ip,
                ip_dst=self.tunnel3_ip,
                ip_ttl=64,
                ip_flags=0x2,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=inner_pkt))
            mask_set_do_not_care_packet(vxlan_pkt1, UDP, "sport")
            vxlan_pkt2 = mask.Mask(simple_vxlan_packet(
                eth_src='00:77:66:55:44:33',
                eth_dst=self.underlay_neighbor_mac4,  # 00:11:11:11:11:44
                ip_id=0,
                ip_src=self.my_lb_ip,
                ip_dst=self.tunnel3_ip,
                ip_ttl=64,
                ip_flags=0x2,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=inner_pkt))
            mask_set_do_not_care_packet(vxlan_pkt2, UDP, "sport")
            send_packet(self, self.devports[0], pkt)
            rcv_idx = verify_any_packet_on_ports_list(
                self, [vxlan_pkt1, vxlan_pkt2], [[self.devports[3], self.devports[4]]], timeout=2)

            print("Sending packet from Vxlan port 3 to Access port 0")
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src=self.inner_dmac,
                ip_dst=self.customer_ip4,
                ip_src=self.vm3_ip4,
                ip_id=108,
                ip_ttl=64)
            pkt2 = simple_tcp_packet(
                eth_dst='00:22:22:22:22:22',
                eth_src='00:77:66:55:44:33',
                ip_dst=self.customer_ip4,
                ip_src=self.vm3_ip4,
                ip_id=108,
                ip_ttl=63)
            vxlan_pkt = simple_vxlan_packet(
                eth_src=self.underlay_neighbor_mac3,
                eth_dst='00:77:66:55:44:33',
                ip_id=0,
                ip_dst=self.my_lb_ip,
                ip_src=self.tunnel3_ip,
                ip_ttl=64,
                ip_flags=0x2,
                udp_sport=11638,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=pkt)
            send_packet(self, self.devports[3], vxlan_pkt)
            verify_packet(self, pkt2, self.devports[0])

            print("Sending packet from Vxlan port 4 to Access port 0")
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src=self.inner_dmac,
                ip_dst=self.customer_ip4,
                ip_src=self.vm3_ip4,
                ip_id=108,
                ip_ttl=64)
            pkt2 = simple_tcp_packet(
                eth_dst='00:22:22:22:22:22',
                eth_src='00:77:66:55:44:33',
                ip_dst=self.customer_ip4,
                ip_src=self.vm3_ip4,
                ip_id=108,
                ip_ttl=63)
            vxlan_pkt = simple_vxlan_packet(
                eth_src=self.underlay_neighbor_mac4,
                eth_dst='00:77:66:55:44:33',
                ip_id=0,
                ip_dst=self.my_lb_ip,
                ip_src=self.tunnel3_ip,
                ip_ttl=64,
                ip_flags=0x2,
                udp_sport=11638,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=pkt)
            send_packet(self, self.devports[4], vxlan_pkt)
            verify_packet(self, pkt2, self.devports[0])
        finally:
            pass

    def ECMPTunnelIPv6Test(self):
        print("ECMPTunnelIPv6Test()")
        if (self.client.is_feature_enable(SWITCH_FEATURE_IPV6_TUNNEL) == 0):
            print("IPv6 tunnel feature not enabled, skipping")
            return
        try:
            print("Sending packet from Access port 0 to VxLan port 3, 4")
            pkt = simple_tcpv6_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ipv6_dst=self.vm3_ip6,
                ipv6_src=self.customer_ip6,
                ipv6_hlim=64)
            inner_pkt = simple_tcpv6_packet(
                eth_dst=self.inner_dmac,  # 00:33:33:33:33:33
                eth_src=self.default_rmac,
                ipv6_dst=self.vm3_ip6,
                ipv6_src=self.customer_ip6,
                ipv6_hlim=63)
            vxlan_pkt1 = mask.Mask(simple_vxlan_packet(
                eth_src='00:77:66:55:44:33',
                eth_dst=self.underlay_neighbor_mac3,  # 00:11:11:11:11:33
                ip_id=0,
                ip_src=self.my_lb_ip,
                ip_dst=self.tunnel3_ip,
                ip_ttl=64,
                ip_flags=0x2,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=inner_pkt))
            mask_set_do_not_care_packet(vxlan_pkt1, UDP, "sport")
            vxlan_pkt2 = mask.Mask(simple_vxlan_packet(
                eth_src='00:77:66:55:44:33',
                eth_dst=self.underlay_neighbor_mac4,  # 00:11:11:11:11:44
                ip_id=0,
                ip_src=self.my_lb_ip,
                ip_dst=self.tunnel3_ip,
                ip_ttl=64,
                ip_flags=0x2,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=inner_pkt))
            mask_set_do_not_care_packet(vxlan_pkt2, UDP, "sport")
            send_packet(self, self.devports[0], pkt)
            rcv_idx = verify_any_packet_on_ports_list(
                self, [vxlan_pkt1, vxlan_pkt2], [[self.devports[3], self.devports[4]]], timeout=2)

            print("Sending packet from Vxlan port 3 to Access port 0")
            pkt = simple_tcpv6_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src=self.inner_dmac,
                ipv6_dst=self.customer_ip6,
                ipv6_src=self.vm3_ip6,
                ipv6_hlim=64)
            pkt2 = simple_tcpv6_packet(
                eth_dst='00:22:22:22:22:22',
                eth_src='00:77:66:55:44:33',
                ipv6_dst=self.customer_ip6,
                ipv6_src=self.vm3_ip6,
                ipv6_hlim=63)
            vxlan_pkt = simple_vxlan_packet(
                eth_src=self.underlay_neighbor_mac3,
                eth_dst='00:77:66:55:44:33',
                ip_id=0,
                ip_dst=self.my_lb_ip,
                ip_src=self.tunnel3_ip,
                ip_ttl=64,
                ip_flags=0x2,
                udp_sport=11638,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=pkt)
            send_packet(self, self.devports[3], vxlan_pkt)
            verify_packet(self, pkt2, self.devports[0])

            print("Sending packet from Vxlan port 4 to Access port 0")
            pkt = simple_tcpv6_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src=self.inner_dmac,
                ipv6_dst=self.customer_ip6,
                ipv6_src=self.vm3_ip6,
                ipv6_hlim=64)
            pkt2 = simple_tcpv6_packet(
                eth_dst='00:22:22:22:22:22',
                eth_src='00:77:66:55:44:33',
                ipv6_dst=self.customer_ip6,
                ipv6_src=self.vm3_ip6,
                ipv6_hlim=63)
            vxlan_pkt = simple_vxlan_packet(
                eth_src=self.underlay_neighbor_mac4,
                eth_dst='00:77:66:55:44:33',
                ip_id=0,
                ip_dst=self.my_lb_ip,
                ip_src=self.tunnel3_ip,
                ip_ttl=64,
                ip_flags=0x2,
                udp_sport=11638,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=pkt)
            send_packet(self, self.devports[4], vxlan_pkt)
            verify_packet(self, pkt2, self.devports[0])
        finally:
            pass

    def LAGTunnelIPv4Test(self):
        print("LAGTunnelIPv4Test()")
        try:
            print("Sending packet from Access port 0 to VxLan port 5, 6")
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst=self.vm4_ip4,
                ip_src=self.customer_ip4,
                ip_id=108,
                ip_ttl=64)
            inner_pkt = simple_tcp_packet(
                eth_dst=self.inner_dmac,  # 00:33:33:33:33:33
                eth_src=self.default_rmac,
                ip_dst=self.vm4_ip4,
                ip_src=self.customer_ip4,
                ip_id=108,
                ip_ttl=63)
            vxlan_pkt1 = mask.Mask(simple_vxlan_packet(
                eth_src='00:77:66:55:44:33',
                eth_dst=self.underlay_neighbor_mac5,  # 00:11:11:11:11:55
                ip_id=0,
                ip_src=self.my_lb_ip,
                ip_dst=self.tunnel4_ip,
                ip_ttl=64,
                ip_flags=0x2,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=inner_pkt))
            mask_set_do_not_care_packet(vxlan_pkt1, UDP, "sport")
            send_packet(self, self.devports[0], pkt)
            rcv_idx = verify_packet_any_port(
                self, vxlan_pkt1, [self.devports[5], self.devports[6]], timeout=2)

            print("Sending packet from Vxlan port 5 to Access port 0")
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src=self.inner_dmac,
                ip_dst=self.customer_ip4,
                ip_src=self.vm4_ip4,
                ip_id=108,
                ip_ttl=64)
            pkt2 = simple_tcp_packet(
                eth_dst='00:22:22:22:22:22',
                eth_src='00:77:66:55:44:33',
                ip_dst=self.customer_ip4,
                ip_src=self.vm4_ip4,
                ip_id=108,
                ip_ttl=63)
            vxlan_pkt = simple_vxlan_packet(
                eth_src=self.underlay_neighbor_mac5,
                eth_dst='00:77:66:55:44:33',
                ip_id=0,
                ip_dst=self.my_lb_ip,
                ip_src=self.tunnel4_ip,
                ip_ttl=64,
                ip_flags=0x2,
                udp_sport=11638,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=pkt)
            send_packet(self, self.devports[5], vxlan_pkt)
            verify_packet(self, pkt2, self.devports[0])

            print("Sending packet from Vxlan port 6 to Access port 0")
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src=self.inner_dmac,
                ip_dst=self.customer_ip4,
                ip_src=self.vm4_ip4,
                ip_id=108,
                ip_ttl=64)
            pkt2 = simple_tcp_packet(
                eth_dst='00:22:22:22:22:22',
                eth_src='00:77:66:55:44:33',
                ip_dst=self.customer_ip4,
                ip_src=self.vm4_ip4,
                ip_id=108,
                ip_ttl=63)
            vxlan_pkt = simple_vxlan_packet(
                eth_src=self.underlay_neighbor_mac5,
                eth_dst='00:77:66:55:44:33',
                ip_id=0,
                ip_dst=self.my_lb_ip,
                ip_src=self.tunnel4_ip,
                ip_ttl=64,
                ip_flags=0x2,
                udp_sport=11638,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=pkt)
            send_packet(self, self.devports[6], vxlan_pkt)
            verify_packet(self, pkt2, self.devports[0])
        finally:
            pass

    def LAGTunnelIPv6Test(self):
        print("LAGTunnelIPv6Test()")
        if (self.client.is_feature_enable(SWITCH_FEATURE_IPV6_TUNNEL) == 0):
            print("IPv6 tunnel feature not enabled, skipping")
            return
        try:
            print("Sending packet from Access port 0 to VxLan port 5, 6")
            pkt = simple_tcpv6_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ipv6_dst=self.vm4_ip6,
                ipv6_src=self.customer_ip6,
                ipv6_hlim=64)
            inner_pkt = simple_tcpv6_packet(
                eth_dst=self.inner_dmac,  # 00:33:33:33:33:33
                eth_src=self.default_rmac,
                ipv6_dst=self.vm4_ip6,
                ipv6_src=self.customer_ip6,
                ipv6_hlim=63)
            vxlan_pkt1 = mask.Mask(simple_vxlan_packet(
                eth_src='00:77:66:55:44:33',
                eth_dst=self.underlay_neighbor_mac5,  # 00:11:11:11:11:55
                ip_id=0,
                ip_src=self.my_lb_ip,
                ip_dst=self.tunnel4_ip,
                ip_ttl=64,
                ip_flags=0x2,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=inner_pkt))
            mask_set_do_not_care_packet(vxlan_pkt1, UDP, "sport")
            send_packet(self, self.devports[0], pkt)
            rcv_idx = verify_packet_any_port(
                self, vxlan_pkt1, [self.devports[5], self.devports[6]], timeout=2)

            print("Sending packet from Vxlan port 5 to Access port 0")
            pkt = simple_tcpv6_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src=self.inner_dmac,
                ipv6_dst=self.customer_ip6,
                ipv6_src=self.vm4_ip6,
                ipv6_hlim=64)
            pkt2 = simple_tcpv6_packet(
                eth_dst='00:22:22:22:22:22',
                eth_src='00:77:66:55:44:33',
                ipv6_dst=self.customer_ip6,
                ipv6_src=self.vm4_ip6,
                ipv6_hlim=63)
            vxlan_pkt = simple_vxlan_packet(
                eth_src=self.underlay_neighbor_mac5,
                eth_dst='00:77:66:55:44:33',
                ip_id=0,
                ip_dst=self.my_lb_ip,
                ip_src=self.tunnel4_ip,
                ip_ttl=64,
                ip_flags=0x2,
                udp_sport=11638,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=pkt)
            send_packet(self, self.devports[5], vxlan_pkt)
            verify_packet(self, pkt2, self.devports[0])

            print("Sending packet from Vxlan port 6 to Access port 0")
            pkt = simple_tcpv6_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src=self.inner_dmac,
                ipv6_dst=self.customer_ip6,
                ipv6_src=self.vm4_ip6,
                ipv6_hlim=64)
            pkt2 = simple_tcpv6_packet(
                eth_dst='00:22:22:22:22:22',
                eth_src='00:77:66:55:44:33',
                ipv6_dst=self.customer_ip6,
                ipv6_src=self.vm4_ip6,
                ipv6_hlim=63)
            vxlan_pkt = simple_vxlan_packet(
                eth_src=self.underlay_neighbor_mac5,
                eth_dst='00:77:66:55:44:33',
                ip_id=0,
                ip_dst=self.my_lb_ip,
                ip_src=self.tunnel4_ip,
                ip_ttl=64,
                ip_flags=0x2,
                udp_sport=11638,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=pkt)
            send_packet(self, self.devports[6], vxlan_pkt)
            verify_packet(self, pkt2, self.devports[0])
        finally:
            pass

    def EcmpLagTunnelIPv4Test(self):
        print("EcmpLagTunnelIPv4Test()")
        try:
            print("Sending packet from Access port 0 to VxLan port 7, 8")
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst=self.vm5_ip4,
                ip_src=self.customer_ip4,
                ip_id=108,
                ip_ttl=64)
            inner_pkt = simple_tcp_packet(
                eth_dst=self.inner_dmac,  # 00:33:33:33:33:33
                eth_src=self.default_rmac,
                ip_dst=self.vm5_ip4,
                ip_src=self.customer_ip4,
                ip_id=108,
                ip_ttl=63)
            vxlan_pkt1 = mask.Mask(simple_vxlan_packet(
                eth_src='00:77:66:55:44:33',
                eth_dst=self.underlay_neighbor_mac6,  # 00:11:11:11:11:66
                ip_id=0,
                ip_src=self.my_lb_ip,
                ip_dst=self.tunnel5_ip,
                ip_ttl=64,
                ip_flags=0x2,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=inner_pkt))
            mask_set_do_not_care_packet(vxlan_pkt1, UDP, "sport")
            send_packet(self, self.devports[0], pkt)
            rcv_idx = verify_packet_any_port(
                self, vxlan_pkt1, [self.devports[7], self.devports[8]], timeout=2)

            print("Sending packet from Vxlan port 7 to Access port 0")
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src=self.inner_dmac,
                ip_dst=self.customer_ip4,
                ip_src=self.vm5_ip4,
                ip_id=108,
                ip_ttl=64)
            pkt2 = simple_tcp_packet(
                eth_dst='00:22:22:22:22:22',
                eth_src='00:77:66:55:44:33',
                ip_dst=self.customer_ip4,
                ip_src=self.vm5_ip4,
                ip_id=108,
                ip_ttl=63)
            vxlan_pkt = simple_vxlan_packet(
                eth_src=self.underlay_neighbor_mac6,
                eth_dst='00:77:66:55:44:33',
                ip_id=0,
                ip_dst=self.my_lb_ip,
                ip_src=self.tunnel5_ip,
                ip_ttl=64,
                ip_flags=0x2,
                udp_sport=11638,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=pkt)
            send_packet(self, self.devports[7], vxlan_pkt)
            verify_packet(self, pkt2, self.devports[0])

            print("Sending packet from Vxlan port 8 to Access port 0")
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src=self.inner_dmac,
                ip_dst=self.customer_ip4,
                ip_src=self.vm5_ip4,
                ip_id=108,
                ip_ttl=64)
            pkt2 = simple_tcp_packet(
                eth_dst='00:22:22:22:22:22',
                eth_src='00:77:66:55:44:33',
                ip_dst=self.customer_ip4,
                ip_src=self.vm5_ip4,
                ip_id=108,
                ip_ttl=63)
            vxlan_pkt = simple_vxlan_packet(
                eth_src=self.underlay_neighbor_mac6,
                eth_dst='00:77:66:55:44:33',
                ip_id=0,
                ip_dst=self.my_lb_ip,
                ip_src=self.tunnel5_ip,
                ip_ttl=64,
                ip_flags=0x2,
                udp_sport=11638,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=pkt)
            send_packet(self, self.devports[8], vxlan_pkt)
            verify_packet(self, pkt2, self.devports[0])
        finally:
            pass

    def EcmpLagTunnelIPv6Test(self):
        print("EcmpLagTunnelIPv6Test()")
        if (self.client.is_feature_enable(SWITCH_FEATURE_IPV6_TUNNEL) == 0):
            print("IPv6 tunnel feature not enabled, skipping")
            return
        try:
            print("Sending packet from Access port 0 to VxLan port 7, 8")
            pkt = simple_tcpv6_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ipv6_dst=self.vm5_ip6,
                ipv6_src=self.customer_ip6,
                ipv6_hlim=64)
            inner_pkt = simple_tcpv6_packet(
                eth_dst=self.inner_dmac,  # 00:33:33:33:33:33
                eth_src=self.default_rmac,
                ipv6_dst=self.vm5_ip6,
                ipv6_src=self.customer_ip6,
                ipv6_hlim=63)
            vxlan_pkt1 = mask.Mask(simple_vxlan_packet(
                eth_src='00:77:66:55:44:33',
                eth_dst=self.underlay_neighbor_mac6,  # 00:11:11:11:11:66
                ip_id=0,
                ip_src=self.my_lb_ip,
                ip_dst=self.tunnel5_ip,
                ip_ttl=64,
                ip_flags=0x2,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=inner_pkt))
            mask_set_do_not_care_packet(vxlan_pkt1, UDP, "sport")
            send_packet(self, self.devports[0], pkt)
            rcv_idx = verify_packet_any_port(
                self, vxlan_pkt1, [self.devports[7], self.devports[8]], timeout=2)

            print("Sending packet from Vxlan port 7 to Access port 0")
            pkt = simple_tcpv6_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src=self.inner_dmac,
                ipv6_dst=self.customer_ip6,
                ipv6_src=self.vm5_ip6,
                ipv6_hlim=64)
            pkt2 = simple_tcpv6_packet(
                eth_dst='00:22:22:22:22:22',
                eth_src='00:77:66:55:44:33',
                ipv6_dst=self.customer_ip6,
                ipv6_src=self.vm5_ip6,
                ipv6_hlim=63)
            vxlan_pkt = simple_vxlan_packet(
                eth_src=self.underlay_neighbor_mac6,
                eth_dst='00:77:66:55:44:33',
                ip_id=0,
                ip_dst=self.my_lb_ip,
                ip_src=self.tunnel5_ip,
                ip_ttl=64,
                ip_flags=0x2,
                udp_sport=11638,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=pkt)
            send_packet(self, self.devports[7], vxlan_pkt)
            verify_packet(self, pkt2, self.devports[0])

            print("Sending packet from Vxlan port 8 to Access port 0")
            pkt = simple_tcpv6_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src=self.inner_dmac,
                ipv6_dst=self.customer_ip6,
                ipv6_src=self.vm5_ip6,
                ipv6_hlim=64)
            pkt2 = simple_tcpv6_packet(
                eth_dst='00:22:22:22:22:22',
                eth_src='00:77:66:55:44:33',
                ipv6_dst=self.customer_ip6,
                ipv6_src=self.vm5_ip6,
                ipv6_hlim=63)
            vxlan_pkt = simple_vxlan_packet(
                eth_src=self.underlay_neighbor_mac6,
                eth_dst='00:77:66:55:44:33',
                ip_id=0,
                ip_dst=self.my_lb_ip,
                ip_src=self.tunnel5_ip,
                ip_ttl=64,
                ip_flags=0x2,
                udp_sport=11638,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=pkt)
            send_packet(self, self.devports[8], vxlan_pkt)
            verify_packet(self, pkt2, self.devports[0])
        finally:
            pass

###############################################################################


@group('tunnel')
class L3IPv6TunnelTest(ApiHelper):
    '''
    Reach VM from C1 with vni 2000                       vlan subport 200 on p3
   --------------------  --------------------           --------------------
   | VM: 100.100.1.1  |  | VM: 100.100.2.1  |           | VM: 100.100.7.1  |
   | Host: 4444::1    |  | Host: 4444::2    |    |----  | Host: 4444::7    | - subport
   --------------------  --------------------    |      --------------------
             |                    |              |      --------------------
      -----------------------------------------------    | VM: 100.100.3.1 |
      |    | p1 |          | p2 |           --  p3  | -- | Host: 4444::3   | - ecmp
      |    ------          ------               p4  |    -------------------
      |      |               |                      |
      |  -----------------------                    |    -------------------
      |  | Tunnel: 4444::44    |            --  p5  | -- | VM: 100.100.4.1 | - lag
      |  -----------------------                p6  |    | Host: 4444::4   |
      |        |                                    |    -------------------
      |      ------                         --  p7  |
      |      | p0 |                             p8  |    -------------------
      ----------------------------------------------- -- | VM: 100.100.5.1 | - ecmp_lag
                |                      |                 | Host: 4444::5   |
        -------------------    --------------------      -------------------
        | C1: 100.100.0.1 |    | VM: 100.100.6.1  |
        -------------------    | Host: 4444::6    | - svi_lag
                               --------------------
    '''
    def setUp(self):
        self.configure()
        print()
        if (self.client.is_feature_enable(SWITCH_FEATURE_IPV6_TUNNEL) == 0):
            print("IPv6 tunnel feature not enabled, skipping")
            return
        if not self.client.is_feature_enable(SWITCH_FEATURE_TUNNEL_ENCAP):
            print("Tunnel encap not enabled, skipping")
            return

        if not self.client.is_feature_enable(SWITCH_FEATURE_VXLAN):
            print("VXLAN feature not enabled, skipping")
            return

        # underlay config input
        self.uvrf = self.add_vrf(self.device, id=100, src_mac=self.rmac)
        self.vm1_ip4 = '100.100.1.1'
        self.vm1_ip6 = '1234:5678:9abc:def0:4422:1133:5577:9911'
        self.vm2_ip4 = '100.100.2.1'
        self.vm2_ip6 = '1234:5678:9abc:def0:4422:1133:5577:9921'
        self.vm3_ip4 = '100.100.3.1'
        self.vm3_ip6 = '1234:5678:9abc:def0:4422:1133:5577:9931'
        self.vm4_ip4 = '100.100.4.1'
        self.vm4_ip6 = '1234:5678:9abc:def0:4422:1133:5577:9941'
        self.vm5_ip4 = '100.100.5.1'
        self.vm5_ip6 = '1234:5678:9abc:def0:4422:1133:5577:9951'
        self.vm6_ip4 = '100.100.6.1'
        self.vm6_ip6 = '1234:5678:9abc:def0:4422:1133:5577:9961'
        self.vm7_ip4 = '100.100.7.1'
        self.vm7_ip6 = '1234:5678:9abc:def0:4422:1133:5577:9971'
        self.vm8_ip4 = '100.100.8.1'
        self.vm8_ip6 = '1234:5678:9abc:def0:4422:1133:5577:9981'
        self.vm9_ip4 = '100.100.9.1'
        self.vm9_ip6 = '1234:5678:9abc:def0:4422:1133:5577:9991'
        self.customer_ip4 = '100.100.0.1'
        self.customer_ip6 = '1234:5678:9abc:def0:4422:1133:5577:0011'
        self.vni = 2000
        self.my_lb_ip = '4444:5678:9abc:def0:4422:1133:5577:44'
        self.tunnel1_ip = '4444:5678:9abc:def0:4422:1133:5577:1'
        self.tunnel2_ip = '4444:5678:9abc:def0:4422:1133:5577:2'
        self.tunnel3_ip = '4444:5678:9abc:def0:4422:1133:5577:3'
        self.tunnel4_ip = '4444:5678:9abc:def0:4422:1133:5577:4'
        self.tunnel5_ip = '4444:5678:9abc:def0:4422:1133:5577:5'
        self.tunnel6_ip = '4444:5678:9abc:def0:4422:1133:5577:6'
        self.tunnel7_ip = '4444:5678:9abc:def0:4422:1133:5577:7'
        self.inner_dmac = "00:33:33:33:33:33"
        self.underlay_neighbor_mac1 = '00:11:11:11:11:11'
        self.underlay_neighbor_mac2 = '00:11:11:11:11:22'
        self.underlay_neighbor_mac3 = '00:11:11:11:11:33' #4444::3 via 11.11.11.1 (ecmp)
        self.underlay_neighbor_mac4 = '00:11:11:11:11:44' #4444::3 via 11.11.11.2 (ecmp)
        self.underlay_neighbor_mac5 = '00:11:11:11:11:55'
        self.underlay_neighbor_mac6 = '00:11:11:11:11:66'
        self.underlay_neighbor_mac7 = '00:11:11:11:11:77' #4444::6 via svi - vlan20
        self.underlay_neighbor_mac8 = '00:11:11:11:11:88' #4444::6 via svi - vlan30
        self.underlay_neighbor_mac9 = '00:11:11:11:11:99' #4444::7 via l3 subport port3 vlan 200

        # overlay configuration
        self.ovrf = self.add_vrf(self.device, id = 200, src_mac=self.rmac)
        self.tunnel_type = SWITCH_TUNNEL_ATTR_TYPE_VXLAN
        self.term_type = SWITCH_TUNNEL_TERM_ATTR_TERMINATION_TYPE_P2MP
        self.encap_ttl_mode = SWITCH_TUNNEL_ATTR_ENCAP_TTL_MODE_PIPE_MODEL
        self.decap_ttl_mode = SWITCH_TUNNEL_ATTR_DECAP_TTL_MODE_PIPE_MODEL

        self.lag0 = self.add_lag(self.device)
        self.lag_port5 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port5)
        self.lag_port6 = self.add_lag_member(self.device, lag_handle=self.lag0, port_handle=self.port6)
        self.lag1 = self.add_lag(self.device)
        self.lag_port7 = self.add_lag_member(self.device, lag_handle=self.lag1, port_handle=self.port7)
        self.lag_port8 = self.add_lag_member(self.device, lag_handle=self.lag1, port_handle=self.port8)

        # create underlay loopback rif for tunnel
        self.urif_lb = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_LOOPBACK, vrf_handle=self.uvrf, src_mac=self.rmac)
        self.orif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port0, vrf_handle=self.ovrf, src_mac=self.rmac)

        # Create route to tunnel ip 4444::1
        self.urif1 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port1, vrf_handle=self.uvrf, src_mac=self.rmac)
        self.unhop1 = self.add_nexthop(self.device, handle=self.urif1, dest_ip=self.tunnel1_ip)
        self.uneighbor1 = self.add_neighbor(self.device,
            mac_address=self.underlay_neighbor_mac1,
            handle=self.urif1,
            dest_ip=self.tunnel1_ip)  # 4444::1
        self.tunnel_route1 = self.add_route(self.device, ip_prefix=self.tunnel1_ip,
            vrf_handle=self.uvrf, nexthop_handle=self.unhop1)

        # Create route to tunnel ip 4444::2
        self.urif2 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_VLAN, vlan_handle=self.vlan10, vrf_handle=self.uvrf, src_mac=self.rmac)
        vlan_mbr2 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port2)
        self.attribute_set(self.port2, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.unhop2 = self.add_nexthop(self.device, handle=self.urif2, dest_ip=self.tunnel2_ip)
        self.uneighbor = self.add_neighbor(self.device,
            mac_address=self.underlay_neighbor_mac2,
            handle=self.urif2,
            dest_ip=self.tunnel2_ip)  # 4444::2
        self.tunnel_route2 = self.add_route(self.device, ip_prefix=self.tunnel2_ip,
            vrf_handle=self.uvrf, nexthop_handle=self.unhop2)

        # Create route to tunnel ip 4444::3
        self.urif3 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port3, vrf_handle=self.uvrf, src_mac=self.rmac)
        self.uneighbor3 = self.add_neighbor(self.device,
            mac_address=self.underlay_neighbor_mac3,
            handle=self.urif3, dest_ip="11.11.11.1")
        self.unhop3 = self.add_nexthop(self.device, handle=self.urif3, dest_ip="11.11.11.1")

        self.urif4 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port4, vrf_handle=self.uvrf, src_mac=self.rmac)
        self.uneighbor4 = self.add_neighbor(self.device,
            mac_address=self.underlay_neighbor_mac4,
            handle=self.urif4, dest_ip="11.11.11.2")
        self.unhop4 = self.add_nexthop(self.device, handle=self.urif4, dest_ip="11.11.11.2")

        ecmp0 = self.add_ecmp(self.device)
        ecmp_member01 = self.add_ecmp_member(self.device, nexthop_handle=self.unhop3, ecmp_handle=ecmp0)
        ecmp_member02 = self.add_ecmp_member(self.device, nexthop_handle=self.unhop4, ecmp_handle=ecmp0)
        self.tunnel_route3 = self.add_route(self.device, ip_prefix=self.tunnel3_ip,
            vrf_handle=self.uvrf, nexthop_handle=ecmp0)

        # Create route to tunnel ip 4444::4
        self.urif5 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.lag0, vrf_handle=self.uvrf, src_mac=self.rmac)
        self.unhop5 = self.add_nexthop(self.device, handle=self.urif5, dest_ip=self.tunnel4_ip)
        self.uneighbor5 = self.add_neighbor(self.device,
            mac_address=self.underlay_neighbor_mac5,
            handle=self.urif5, dest_ip=self.tunnel4_ip)
        self.tunnel_route4 = self.add_route(self.device, ip_prefix=self.tunnel4_ip,
            vrf_handle=self.uvrf, nexthop_handle=self.unhop5)

        # Create route to tunnel ip 4444::5
        self.urif6 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.lag1, vrf_handle=self.uvrf, src_mac=self.rmac)
        self.uneighbor6 = self.add_neighbor(self.device,
            mac_address=self.underlay_neighbor_mac6,
            handle=self.urif6, dest_ip="12.11.11.1")
        self.unhop6 = self.add_nexthop(self.device, handle=self.urif6, dest_ip="12.11.11.1")

        ecmp1 = self.add_ecmp(self.device)
        ecmp_member11 = self.add_ecmp_member(self.device, nexthop_handle=self.unhop6, ecmp_handle=ecmp1)
        self.tunnel_route5 = self.add_route(self.device, ip_prefix=self.tunnel5_ip,
            vrf_handle=self.uvrf, nexthop_handle=ecmp1)

        # Create route to tunnel ip 4444::7
        self.urif7 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_SUB_PORT, port_handle=self.port3, outer_vlan_id=200, vrf_handle=self.uvrf, src_mac=self.rmac)
        self.unhop7 = self.add_nexthop(self.device, handle=self.urif7, dest_ip=self.tunnel7_ip)
        self.uneighbor7 = self.add_neighbor(self.device,
            mac_address=self.underlay_neighbor_mac9,
            handle=self.urif7,
            dest_ip=self.tunnel7_ip)  # 4444::7
        self.tunnel_route7 = self.add_route(self.device, ip_prefix=self.tunnel7_ip,
            vrf_handle=self.uvrf, nexthop_handle=self.unhop7)

        # Create overlay ECMP
        self.tunnel_ecmp1 = self.add_ecmp(self.device)
        self.tunnel_ecmp2 = self.add_ecmp(self.device)

        # Encap configuration follows

        # create Encap/Decap mappers
        self.encap_tunnel_map = self.add_tunnel_mapper(self.device,
            type=SWITCH_TUNNEL_MAPPER_ATTR_TYPE_VRF_HANDLE_TO_VNI)
        self.decap_tunnel_map = self.add_tunnel_mapper(self.device,
            type=SWITCH_TUNNEL_MAPPER_ATTR_TYPE_VNI_TO_VRF_HANDLE)

        # create Encap/Decap mapper entries for ovrf
        self.encap_tunnel_map_entry = self.add_tunnel_mapper_entry(self.device,
            type=SWITCH_TUNNEL_MAPPER_ENTRY_ATTR_TYPE_VRF_HANDLE_TO_VNI,
            tunnel_mapper_handle=self.encap_tunnel_map,
            network_handle=self.ovrf,
            tunnel_vni=self.vni)
        self.decap_tunnel_map_entry = self.add_tunnel_mapper_entry(self.device,
            type=SWITCH_TUNNEL_MAPPER_ENTRY_ATTR_TYPE_VNI_TO_VRF_HANDLE,
            tunnel_mapper_handle=self.decap_tunnel_map,
            network_handle=self.ovrf,
            tunnel_vni=self.vni)

        ingress_mapper_list = [switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=self.encap_tunnel_map)]
        egress_mapper_list = [switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=self.decap_tunnel_map)]

        # create Tunnel
        self.tunnel = self.add_tunnel(self.device,
            type=self.tunnel_type,
            src_ip=self.my_lb_ip,
            ingress_mapper_handles=ingress_mapper_list,
            egress_mapper_handles=egress_mapper_list,
            encap_ttl_mode=self.encap_ttl_mode,
            decap_ttl_mode=self.decap_ttl_mode,
            underlay_rif_handle=self.urif_lb)

        # Decap configuration follows

        # Create tunnel_termination entry
        self.tunnel_term = self.add_tunnel_term(self.device,
            type=self.tunnel_type,
            vrf_handle=self.uvrf,
            tunnel_handle=self.tunnel,
            termination_type=self.term_type,
            dst_ip=self.my_lb_ip)

        # create tunnel nexthop for VM 100.100.1.1
        self.tunnel_nexthop = self.add_nexthop(self.device,
            type=SWITCH_NEXTHOP_ATTR_TYPE_TUNNEL,
            rw_type=SWITCH_NEXTHOP_ATTR_RW_TYPE_L3_VNI,
            handle=self.tunnel,
            dest_ip=self.tunnel1_ip, # 4444:1
            mac_address=self.inner_dmac,
            tunnel_vni=2000)
        self.tunnel_ecmp1_member1 = self.add_ecmp_member(self.device, nexthop_handle=self.tunnel_nexthop,
                ecmp_handle=self.tunnel_ecmp1)
        # add route to tunnel_nexthop
        self.customer_route = self.add_route(self.device,
            ip_prefix=self.vm1_ip4,
            vrf_handle=self.ovrf,
            nexthop_handle=self.tunnel_nexthop)
        self.customer_route = self.add_route(self.device,
            ip_prefix=self.vm1_ip6,
            vrf_handle=self.ovrf,
            nexthop_handle=self.tunnel_nexthop)

        # create tunnel nexthop for VM 100.100.2.1
        self.tunnel_nexthop = self.add_nexthop(self.device,
            type=SWITCH_NEXTHOP_ATTR_TYPE_TUNNEL,
            rw_type=SWITCH_NEXTHOP_ATTR_RW_TYPE_L3_VNI,
            handle=self.tunnel,
            dest_ip=self.tunnel2_ip,  # 4444::2
            mac_address=self.inner_dmac,
            tunnel_vni=2000)
        self.tunnel_ecmp1_member2 = self.add_ecmp_member(self.device, nexthop_handle=self.tunnel_nexthop,
                ecmp_handle=self.tunnel_ecmp1)
        # add route to tunnel_nexthop
        self.customer_route = self.add_route(self.device,
            ip_prefix=self.vm2_ip4,
            vrf_handle=self.ovrf,
            nexthop_handle=self.tunnel_nexthop)
        self.customer_route = self.add_route(self.device,
            ip_prefix=self.vm2_ip6,
            vrf_handle=self.ovrf,
            nexthop_handle=self.tunnel_nexthop)

        # create tunnel nexthop for VM 100.100.3.1
        self.tunnel_nexthop = self.add_nexthop(self.device,
            type=SWITCH_NEXTHOP_ATTR_TYPE_TUNNEL,
            rw_type=SWITCH_NEXTHOP_ATTR_RW_TYPE_L3_VNI,
            handle=self.tunnel,
            dest_ip=self.tunnel3_ip,  # 4444::3
            mac_address=self.inner_dmac,
            tunnel_vni=2000)
        self.tunnel_ecmp2_member1 = self.add_ecmp_member(self.device, nexthop_handle=self.tunnel_nexthop,
                ecmp_handle=self.tunnel_ecmp2)
        # add route to tunnel_nexthop
        self.customer_route = self.add_route(self.device,
            ip_prefix=self.vm3_ip4,
            vrf_handle=self.ovrf,
            nexthop_handle=self.tunnel_nexthop)
        self.customer_route = self.add_route(self.device,
            ip_prefix=self.vm3_ip6,
            vrf_handle=self.ovrf,
            nexthop_handle=self.tunnel_nexthop)

        # create tunnel nexthop for VM 100.100.4.1
        self.tunnel_nexthop = self.add_nexthop(self.device,
            type=SWITCH_NEXTHOP_ATTR_TYPE_TUNNEL,
            rw_type=SWITCH_NEXTHOP_ATTR_RW_TYPE_L3_VNI,
            handle=self.tunnel,
            dest_ip=self.tunnel4_ip,  # 4444::4
            mac_address=self.inner_dmac,
            tunnel_vni=2000)
        # add route to tunnel_nexthop
        self.customer_route = self.add_route(self.device,
            ip_prefix=self.vm4_ip4,
            vrf_handle=self.ovrf,
            nexthop_handle=self.tunnel_nexthop)
        self.customer_route = self.add_route(self.device,
            ip_prefix=self.vm4_ip6,
            vrf_handle=self.ovrf,
            nexthop_handle=self.tunnel_nexthop)

        # create tunnel nexthop for VM 100.100.5.1
        self.tunnel_nexthop = self.add_nexthop(self.device,
            type=SWITCH_NEXTHOP_ATTR_TYPE_TUNNEL,
            rw_type=SWITCH_NEXTHOP_ATTR_RW_TYPE_L3_VNI,
            handle=self.tunnel,
            dest_ip=self.tunnel5_ip,  # 4444::5
            mac_address=self.inner_dmac,
            tunnel_vni=2000)
        self.tunnel_ecmp2_member2 = self.add_ecmp_member(self.device, nexthop_handle=self.tunnel_nexthop,
                ecmp_handle=self.tunnel_ecmp2)
        # add route to tunnel_nexthop
        self.customer_route = self.add_route(self.device,
            ip_prefix=self.vm5_ip4,
            vrf_handle=self.ovrf,
            nexthop_handle=self.tunnel_nexthop)
        self.customer_route = self.add_route(self.device,
            ip_prefix=self.vm5_ip6,
            vrf_handle=self.ovrf,
            nexthop_handle=self.tunnel_nexthop)

        # create tunnel nexthop for VM 100.100.6.1
        self.tunnel_nexthop = self.add_nexthop(self.device,
            type=SWITCH_NEXTHOP_ATTR_TYPE_TUNNEL,
            rw_type=SWITCH_NEXTHOP_ATTR_RW_TYPE_L3_VNI,
            handle=self.tunnel,
            dest_ip=self.tunnel6_ip,  # 4444::6
            mac_address=self.inner_dmac,
            tunnel_vni=2000)
        # add route to tunnel_nexthop
        self.customer_route = self.add_route(self.device,
            ip_prefix=self.vm6_ip4,
            vrf_handle=self.ovrf,
            nexthop_handle=self.tunnel_nexthop)
        self.customer_route = self.add_route(self.device,
            ip_prefix=self.vm6_ip6,
            vrf_handle=self.ovrf,
            nexthop_handle=self.tunnel_nexthop)

        # create tunnel nexthop for VM 100.100.7.1
        self.tunnel_nexthop = self.add_nexthop(self.device,
            type=SWITCH_NEXTHOP_ATTR_TYPE_TUNNEL,
            rw_type=SWITCH_NEXTHOP_ATTR_RW_TYPE_L3_VNI,
            handle=self.tunnel,
            dest_ip=self.tunnel7_ip,  # 4444::7
            mac_address=self.inner_dmac,
            tunnel_vni=2000)
        # add route to tunnel_nexthop
        self.customer_route = self.add_route(self.device,
            ip_prefix=self.vm7_ip4,
            vrf_handle=self.ovrf,
            nexthop_handle=self.tunnel_nexthop)
        self.customer_route = self.add_route(self.device,
            ip_prefix=self.vm7_ip6,
            vrf_handle=self.ovrf,
            nexthop_handle=self.tunnel_nexthop)

        # Create route to customer from VM
        self.onhop4 = self.add_nexthop(self.device, handle=self.orif, dest_ip=self.customer_ip4)
        self.oneighbor4 = self.add_neighbor(self.device,
            mac_address='00:22:22:22:22:22',
            handle=self.orif,
            dest_ip=self.customer_ip4)  # 100.100.0.1
        self.customer_route = self.add_route(self.device, ip_prefix=self.customer_ip4,
            vrf_handle=self.ovrf, nexthop_handle=self.onhop4)
        self.onhop6 = self.add_nexthop(self.device, handle=self.orif, dest_ip=self.customer_ip6)
        self.oneighbor6 = self.add_neighbor(self.device,
            mac_address='00:22:22:22:22:22',
            handle=self.orif,
            dest_ip=self.customer_ip6)  # 100.100.0.1
        self.customer_route = self.add_route(self.device, ip_prefix=self.customer_ip6,
            vrf_handle=self.ovrf, nexthop_handle=self.onhop6)


        # add Customer routes pointing to overlay tunnel ecmps
        self.customer_route = self.add_route(self.device,
            ip_prefix=self.vm8_ip4,
            vrf_handle=self.ovrf,
            nexthop_handle=self.tunnel_ecmp1)
        self.customer_route = self.add_route(self.device,
            ip_prefix=self.vm8_ip6,
            vrf_handle=self.ovrf,
            nexthop_handle=self.tunnel_ecmp1)

        self.customer_route = self.add_route(self.device,
            ip_prefix=self.vm9_ip4,
            vrf_handle=self.ovrf,
            nexthop_handle=self.tunnel_ecmp2)
        self.customer_route = self.add_route(self.device,
            ip_prefix=self.vm9_ip6,
            vrf_handle=self.ovrf,
            nexthop_handle=self.tunnel_ecmp2)


    def runTest(self):
        if (self.client.is_feature_enable(SWITCH_FEATURE_IPV6_TUNNEL) == 0):
            return
        if not self.client.is_feature_enable(SWITCH_FEATURE_TUNNEL_ENCAP):
            return
        if not self.client.is_feature_enable(SWITCH_FEATURE_VXLAN):
            return
        try:
            self.L3IPv6TunnelBaseTests()
            self.EcmpLagTunnelLagUpdateTests()
            self.SVILagTunnelBaseTests()
            self.SVIEcmpTunnelTests()
        finally:
            pass

    def L3IPv6TunnelBaseTests(self):
        mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan10,
            mac_address=self.underlay_neighbor_mac2,
            destination_handle=self.port2)
        try:
            self.L3IntfIPv4Test()
            self.L3IntfIPv6Test()
            self.L3SubPortIPv4Test()
            self.L3SubPortIPv6Test()
            self.SVITunnelIPv4Test()
            self.SVITunnelIPv6Test()
            self.ECMPTunnelIPv4Test()
            self.ECMPTunnelIPv6Test()
            self.OverlayEcmpTunnelTest(distribution=True)
            self.OverlayEcmpTunnelTest(ip_version=6, distribution=True)
            self.OverlayUnderlayEcmpTunnelTest(distribution=True)
            self.OverlayUnderlayEcmpTunnelTest(ip_version=6, distribution=True)
            self.LAGTunnelIPv4Test()
            self.LAGTunnelIPv6Test()
            self.EcmpLagTunnelIPv4Test(port_list=[7, 8])
            self.EcmpLagTunnelIPv6Test(port_list=[7, 8])
        finally:
            # Delete the MAC entry for SVI
            print("Removing MAC entry for mac {} on VLAN 10".format(self.underlay_neighbor_mac2))
            self.cleanlast()
            pass

        try:
            print("Performing negative tests on VLAN 10 to ensure packet is not forwarded after mac delete")
            self.SVITunnelIPv4Test(negative_test=True)
            self.SVITunnelIPv6Test(negative_test=True)
        finally:
            pass


    def OverlayUnderlayEcmpTunnelTest(self, ip_version=4, port_list=[], distribution=False):
        print("OverlayUnderlayEcmpTunnelTest(), ip_version:{}".format(ip_version))
        if ip_version == 4:
            dst_ip=self.vm9_ip4
            src_ip=self.customer_ip4
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst=dst_ip,
                ip_src=src_ip,
                ip_id=108,
                ip_ttl=64)
            inner_pkt = simple_tcp_packet(
                eth_dst=self.inner_dmac,  # 00:33:33:33:33:33
                eth_src=self.default_rmac,
                ip_dst=dst_ip,
                ip_src=src_ip,
                ip_id=108,
                ip_ttl=63)
        elif ip_version == 6:
            dst_ip=self.vm9_ip6
            src_ip=self.customer_ip6
            pkt = simple_tcpv6_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ipv6_dst=dst_ip,
                ipv6_src=src_ip,
                ipv6_hlim=64)
            inner_pkt = simple_tcpv6_packet(
                eth_dst=self.inner_dmac,  # 00:33:33:33:33:33
                eth_src=self.default_rmac,
                ipv6_dst=dst_ip,
                ipv6_src=src_ip,
                ipv6_hlim=63)
        else:
            print("OverlayUnderlayEcmpTunnelTest() ip_version:{} invalid, skipping test".format(ip_version))
            return

        if not port_list:
            port_list = [self.devports[3], self.devports[4], self.devports[7], self.devports[8]]
        if distribution is True:
            print("Running Distribution Tests")
        try:
            print("Sending packets from Access port 0 to VxLan ports {}".format(port_list))
            count = [0] * len(port_list)
            max_itrs = 1
            if distribution is True:
                max_itrs = 120
            if ip_version==4:
                src_ip = int(binascii.hexlify(socket.inet_aton(self.customer_ip4)), 16)
            else:
                src_ip = int(binascii.hexlify(socket.inet_pton(socket.AF_INET6, self.customer_ip6)), 16)
            for i in range(0, max_itrs):
                if ip_version == 4:
                    src_ip_addr = socket.inet_ntoa(
                    binascii.unhexlify(format(src_ip, 'x').zfill(8)))
                    pkt[IP].src=src_ip_addr
                    inner_pkt[IP].src=src_ip_addr
                else:
                    src_ip_addr = socket.inet_ntop(socket.AF_INET6,
                    binascii.unhexlify(format(src_ip, 'x').zfill(16)))
                    pkt[IPv6].src=src_ip_addr
                    inner_pkt[IPv6].src=src_ip_addr
                exp_vxlan_pkt = []
                if self.devports[3] in port_list:
                    vxlan_pkt1 = mask.Mask(simple_vxlanv6_packet(
                        eth_src='00:77:66:55:44:33',
                        eth_dst=self.underlay_neighbor_mac3,
                        ipv6_src=self.my_lb_ip,
                        ipv6_dst=self.tunnel3_ip,
                        ipv6_hlim=64,
                        with_udp_chksum=False,
                        vxlan_vni=self.vni,
                        inner_frame=inner_pkt))
                    mask_set_do_not_care_packet(vxlan_pkt1, UDP, "sport")
                    exp_vxlan_pkt.append(vxlan_pkt1)
                if self.devports[4] in port_list:
                    vxlan_pkt2 = mask.Mask(simple_vxlanv6_packet(
                        eth_src='00:77:66:55:44:33',
                        eth_dst=self.underlay_neighbor_mac4,
                        ipv6_src=self.my_lb_ip,
                        ipv6_dst=self.tunnel3_ip,
                        ipv6_hlim=64,
                        with_udp_chksum=False,
                        vxlan_vni=self.vni,
                        inner_frame=inner_pkt))
                    mask_set_do_not_care_packet(vxlan_pkt2, UDP, "sport")
                    exp_vxlan_pkt.append(vxlan_pkt2)
                if self.devports[7] in port_list or self.devports[8] in port_list:
                    vxlan_pkt3 = mask.Mask(simple_vxlanv6_packet(
                        eth_src='00:77:66:55:44:33',
                        eth_dst=self.underlay_neighbor_mac6,
                        ipv6_src=self.my_lb_ip,
                        ipv6_dst=self.tunnel5_ip,
                        ipv6_hlim=64,
                        with_udp_chksum=False,
                        vxlan_vni=self.vni,
                        inner_frame=inner_pkt))
                    mask_set_do_not_care_packet(vxlan_pkt3, UDP, "sport")
                    exp_vxlan_pkt.append(vxlan_pkt3)
                send_packet(self, self.devports[0], pkt)
                rcv_idx = verify_any_packet_on_ports_list(
                    self, exp_vxlan_pkt, [port_list], timeout=1, no_flood=True)
                self.assertTrue((len(rcv_idx) == 1 and len(rcv_idx[0]) == 1), "Packet received on more than one ecmp paths")
                count[rcv_idx[0].pop()] += 1
                src_ip += 1
            print("Packet distribution with {} packets on ports {} = {}".format(max_itrs, port_list, count))
            if distribution is True:
                for c in count:
                    self.assertTrue((c >= ((max_itrs / len(port_list)) * 0.5)),
                                    "Ecmp path is not equally balanced")
        finally:
            pass

    def OverlayEcmpTunnelTest(self, ip_version=4, port_list=[], distribution=False):
        print("OverlayEcmpTunnelTest(), ip_version:{}".format(ip_version))
        if ip_version == 4:
            dst_ip=self.vm8_ip4
            src_ip=self.customer_ip4
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst=dst_ip,
                ip_src=src_ip,
                ip_id=108,
                ip_ttl=64)
            inner_pkt = simple_tcp_packet(
                eth_dst=self.inner_dmac,  # 00:33:33:33:33:33
                eth_src=self.default_rmac,
                ip_dst=dst_ip,
                ip_src=src_ip,
                ip_id=108,
                ip_ttl=63)
        elif ip_version == 6:
            dst_ip=self.vm8_ip6
            src_ip=self.customer_ip6
            pkt = simple_tcpv6_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ipv6_dst=dst_ip,
                ipv6_src=src_ip,
                ipv6_hlim=64)
            inner_pkt = simple_tcpv6_packet(
                eth_dst=self.inner_dmac,  # 00:33:33:33:33:33
                eth_src=self.default_rmac,
                ipv6_dst=dst_ip,
                ipv6_src=src_ip,
                ipv6_hlim=63)
        else:
            print("OverlayEcmpTunnelTest() ip_version:{} invalid, skipping test".format(ip_version))
            return

        if not port_list:
            port_list = [self.devports[1], self.devports[2]]
        if distribution is True:
            print("Running Distribution Tests")
        try:
            print("Sending packets from Access port 0 to VxLan ports {}".format(port_list))
            count = [0] * len(port_list)
            max_itrs = 1
            if distribution is True:
                max_itrs = 40
            if ip_version==4:
                src_ip = int(binascii.hexlify(socket.inet_aton(self.customer_ip4)), 16)
            else:
                src_ip = int(binascii.hexlify(socket.inet_pton(socket.AF_INET6, self.customer_ip6)), 16)
            for i in range(0, max_itrs):
                if ip_version == 4:
                    src_ip_addr = socket.inet_ntoa(
                    binascii.unhexlify(format(src_ip, 'x').zfill(8)))
                    pkt[IP].src=src_ip_addr
                    inner_pkt[IP].src=src_ip_addr
                else:
                    src_ip_addr = socket.inet_ntop(socket.AF_INET6,
                    binascii.unhexlify(format(src_ip, 'x').zfill(16)))
                    pkt[IPv6].src=src_ip_addr
                    inner_pkt[IPv6].src=src_ip_addr
                exp_vxlan_pkt = []
                if self.devports[1] in port_list:
                    vxlan_pkt1 = mask.Mask(simple_vxlanv6_packet(
                        eth_src='00:77:66:55:44:33',
                        eth_dst=self.underlay_neighbor_mac1,
                        ipv6_src=self.my_lb_ip,
                        ipv6_dst=self.tunnel1_ip,
                        ipv6_hlim=64,
                        with_udp_chksum=False,
                        vxlan_vni=self.vni,
                        inner_frame=inner_pkt))
                    mask_set_do_not_care_packet(vxlan_pkt1, UDP, "sport")
                    exp_vxlan_pkt.append(vxlan_pkt1)
                if self.devports[2] in port_list:
                    vxlan_pkt2 = mask.Mask(simple_vxlanv6_packet(
                        eth_src='00:77:66:55:44:33',
                        eth_dst=self.underlay_neighbor_mac2,
                        ipv6_src=self.my_lb_ip,
                        ipv6_dst=self.tunnel2_ip,
                        ipv6_hlim=64,
                        with_udp_chksum=False,
                        vxlan_vni=self.vni,
                        inner_frame=inner_pkt))
                    mask_set_do_not_care_packet(vxlan_pkt2, UDP, "sport")
                    exp_vxlan_pkt.append(vxlan_pkt2)
                send_packet(self, self.devports[0], pkt)
                rcv_idx = verify_any_packet_on_ports_list(
                    self, exp_vxlan_pkt, [port_list], timeout=1, no_flood=True)
                self.assertTrue((len(rcv_idx) == 1 and len(rcv_idx[0]) == 1), "Packet received on more than one ecmp paths")
                count[rcv_idx[0].pop()] += 1
                src_ip += 1
            print("Packet distribution with {} packets on ports {} = {}".format(max_itrs, port_list, count))
            if distribution is True:
                for c in count:
                    self.assertTrue((c >= ((max_itrs / len(port_list)) * 0.5)),
                                    "Ecmp path is not equally balanced")
        finally:
            pass

    def EcmpLagTunnelLagUpdateTests(self):
        try:
            print("Adding port 9 to lag1")
            lag_port9 = self.add_lag_member(self.device, lag_handle=self.lag1, port_handle=self.port9)
            print("Disabling port7 on lag1")
            self.attribute_set(self.lag_port7, SWITCH_LAG_MEMBER_ATTR_EGRESS_DISABLE, True)
            self.EcmpLagTunnelIPv4Test(port_list=[8, 9])
            self.EcmpLagTunnelIPv6Test(port_list=[8, 9])
            print("Disabling port8 on lag1")
            self.attribute_set(self.lag_port8, SWITCH_LAG_MEMBER_ATTR_EGRESS_DISABLE, True)
            self.EcmpLagTunnelIPv4Test(port_list=[9])
            self.EcmpLagTunnelIPv6Test(port_list=[9])
            print("Re-enabling port8 on lag1")
            self.attribute_set(self.lag_port8, SWITCH_LAG_MEMBER_ATTR_EGRESS_DISABLE, False)
            self.EcmpLagTunnelIPv4Test(port_list=[9, 8])
            self.EcmpLagTunnelIPv6Test(port_list=[9, 8])
            print("Re-enabling port7 on lag1")
            self.attribute_set(self.lag_port7, SWITCH_LAG_MEMBER_ATTR_EGRESS_DISABLE, False)
            self.EcmpLagTunnelIPv4Test(port_list=[9, 8, 7], distribution=True)
            self.EcmpLagTunnelIPv6Test(port_list=[9, 8, 7], distribution=True)
        finally:
            self.attribute_set(self.lag_port7, SWITCH_LAG_MEMBER_ATTR_EGRESS_DISABLE, False)
            self.attribute_set(self.lag_port8, SWITCH_LAG_MEMBER_ATTR_EGRESS_DISABLE, False)
            self.attribute_set(lag_port9, SWITCH_LAG_MEMBER_ATTR_EGRESS_DISABLE, False)
            # Remove port9 from lag 1
            print("Removing port 9 from lag1")
            self.cleanlast()

    def SVILagTunnelBaseTests(self):
        #Create underlay rif in Vlan20 with member - lag2 (port11/port12) and port9 (access), port10(trunk)
        lag2 = self.add_lag(self.device)
        lag2_port11 = self.add_lag_member(self.device, lag_handle=lag2, port_handle=self.port11)
        lag2_port12 = self.add_lag_member(self.device, lag_handle=lag2, port_handle=self.port12)
        self.attribute_set(lag2, SWITCH_LAG_ATTR_LEARNING, True)
        self.attribute_set(self.port9, SWITCH_PORT_ATTR_LEARNING, True)
        self.attribute_set(self.port10, SWITCH_PORT_ATTR_LEARNING, True)
        self.attribute_set(self.vlan20, SWITCH_VLAN_ATTR_LEARNING, True)
        self.urif7 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_VLAN, vlan_handle=self.vlan20, vrf_handle=self.uvrf, src_mac=self.rmac)
        vlan_mbr = self.add_vlan_member(self.device, vlan_handle=self.vlan20, member_handle=self.port9)
        self.attribute_set(self.port9, SWITCH_PORT_ATTR_PORT_VLAN_ID, 20)
        vlan_mbr = self.add_vlan_member(self.device, vlan_handle=self.vlan20, member_handle=self.port10, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
        vlan_mbr = self.add_vlan_member(self.device, vlan_handle=self.vlan20, member_handle=lag2)
        self.attribute_set(lag2, SWITCH_LAG_ATTR_PORT_VLAN_ID, 20)

        # Create route to tunnel ip 4444::6
        self.unhop7 = self.add_nexthop(self.device, handle=self.urif7, dest_ip=self.tunnel6_ip)
        self.uneighbor7 = self.add_neighbor(self.device,
            mac_address=self.underlay_neighbor_mac7,
            handle=self.urif7, dest_ip=self.tunnel6_ip)
        # Mac entry for the SVI will be resolved at a later point of time via MAC Learning
        self.tunnel_route6 = self.add_route(self.device, ip_prefix=self.tunnel6_ip,
            vrf_handle=self.uvrf, nexthop_handle=self.unhop7)
        try:
            self.SVILagTunnelIPv4Test()
            self.SVILagTunnelIPv6Test()
        finally:
            # Remove tunnel route, unbr, unhop
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            # Flush the dynamic mac entries before vlan members are deleted
            self.client.object_flush_all(SWITCH_OBJECT_TYPE_MAC_ENTRY)
            # Delete the vlan members (port9, port10 and lag2)
            self.attribute_set(lag2, SWITCH_LAG_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.port9, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()
            # Delete the SVI
            self.cleanlast()
            # Reset the port/lag attributes
            self.attribute_set(self.port9, SWITCH_PORT_ATTR_LEARNING, False)
            self.attribute_set(self.port10, SWITCH_PORT_ATTR_LEARNING, False)
            self.attribute_set(self.vlan20, SWITCH_VLAN_ATTR_LEARNING, False)
            self.attribute_set(lag2, SWITCH_LAG_ATTR_LEARNING, False)
            # Delete the lag and lag members
            self.cleanlast()
            self.cleanlast()
            self.cleanlast()

    def SVIEcmpTunnelTests(self):
        def SVIEcmpTunnelUntaggedTests():
            mac = self.add_mac_entry(self.device, vlan_handle=self.vlan20,
                mac_address=self.underlay_neighbor_mac7,
                destination_handle=self.port9)
            mac = self.add_mac_entry(self.device, vlan_handle=self.vlan30,
                mac_address=self.underlay_neighbor_mac8,
                destination_handle=self.port11)
            try:
                self.SVIEcmpTunnelIPv4Test(ecmp_port1=9, ecmp_port2=11)
                self.SVIEcmpTunnelIPv6Test(ecmp_port1=9, ecmp_port2=11)
            finally:
                # Delete mac entries
                self.cleanlast()
                self.cleanlast()

        def SVIEcmpTunnelTaggedTests():
            mac = self.add_mac_entry(self.device, vlan_handle=self.vlan20,
                mac_address=self.underlay_neighbor_mac7,
                destination_handle=self.port10)
            mac = self.add_mac_entry(self.device, vlan_handle=self.vlan30,
                mac_address=self.underlay_neighbor_mac8,
                destination_handle=self.port12)
            try:
                self.SVIEcmpTunnelIPv4Test(ecmp_port1=10, tag_mode1=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED, vid1=20, ecmp_port2=12, tag_mode2=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED, vid2=30)
                self.SVIEcmpTunnelIPv6Test(ecmp_port1=10, tag_mode1=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED, vid1=20, ecmp_port2=12, tag_mode2=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED, vid2=30)
            finally:
                # Delete mac entries
                self.cleanlast()
                self.cleanlast()

        def SVIEcmpTunnelUpdateTests():
            print("Verifying ECMP nexthop member add/deletes")
            print("Removing nhop (SVI[port11/port12]) from ecmp")
            # Delete ECMP member2
            self.cleanlast()
            print("Removing nhop (SVI[port9/port10]) from ecmp")
            # Delete ECMP member1
            self.cleanlast()
            print("Adding nhop (SVI[port11/port12]) back to ecmp")
            # Add back ECMP member2
            ecmp2_member02 = self.add_ecmp_member(self.device, nexthop_handle=self.unhop8, ecmp_handle=ecmp2)
            # Add back MAC entry for port11 (ecmp member2)
            mac = self.add_mac_entry(self.device, vlan_handle=self.vlan30,
                mac_address=self.underlay_neighbor_mac8,
                destination_handle=self.port11)
            try:
                self.SVIEcmpTunnelIPv4Test(ecmp_port1=None, ecmp_port2=11)
                self.SVIEcmpTunnelIPv6Test(ecmp_port1=None, ecmp_port2=11)
            finally:
                #Delete Mac Entry for port11 (ecmp member2)
                self.cleanlast()

            print("Adding nhop (SVI[port9/port10]) back to ecmp")
            # Add back ECMP member1
            ecmp2_member01 = self.add_ecmp_member(self.device, nexthop_handle=self.unhop7, ecmp_handle=ecmp2)
            # Add back MAC entry for port9 (ecmp member1)
            mac = self.add_mac_entry(self.device, vlan_handle=self.vlan20,
                mac_address=self.underlay_neighbor_mac7,
                destination_handle=self.port9)
            # Add back MAC entry for port11 (ecmp member2)
            mac = self.add_mac_entry(self.device, vlan_handle=self.vlan30,
                mac_address=self.underlay_neighbor_mac8,
                destination_handle=self.port11)
            try:
                self.SVIEcmpTunnelIPv4Test(ecmp_port1=9, ecmp_port2=11)
                self.SVIEcmpTunnelIPv6Test(ecmp_port1=9, ecmp_port2=11)
            finally:
                # Delete Mac Entries
                self.cleanlast()
                self.cleanlast()

        # Create ECMP with two SVI (VLAN20, VLAN30)  underlay nexthops
        vlan_mbr = self.add_vlan_member(self.device, vlan_handle=self.vlan20, member_handle=self.port9)
        vlan_mbr = self.add_vlan_member(self.device, vlan_handle=self.vlan30, member_handle=self.port11)
        self.attribute_set(self.port9, SWITCH_PORT_ATTR_PORT_VLAN_ID, 20)
        self.attribute_set(self.port11, SWITCH_PORT_ATTR_PORT_VLAN_ID, 30)

        # Create route to tunnel ip 4444::6
        self.urif7 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_VLAN, vlan_handle=self.vlan20, vrf_handle=self.uvrf, src_mac=self.rmac)
        self.uneighbor7 = self.add_neighbor(self.device,
            mac_address=self.underlay_neighbor_mac7,
            handle=self.urif7, dest_ip="12.12.12.1")
        self.unhop7 = self.add_nexthop(self.device, handle=self.urif7, dest_ip="12.12.12.1")

        self.urif8 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_VLAN, vlan_handle=self.vlan30, vrf_handle=self.uvrf, src_mac=self.rmac)
        self.uneighbor8 = self.add_neighbor(self.device,
            mac_address=self.underlay_neighbor_mac8,
            handle=self.urif8, dest_ip="12.12.12.2")
        self.unhop8 = self.add_nexthop(self.device, handle=self.urif8, dest_ip="12.12.12.2")

        ecmp2 = self.add_ecmp(self.device)
        self.tunnel_route6 = self.add_route(self.device, ip_prefix=self.tunnel6_ip,
            vrf_handle=self.uvrf, nexthop_handle=ecmp2)
        ecmp2_member01 = self.add_ecmp_member(self.device, nexthop_handle=self.unhop7, ecmp_handle=ecmp2)
        ecmp2_member02 = self.add_ecmp_member(self.device, nexthop_handle=self.unhop8, ecmp_handle=ecmp2)
        try:
            try:
                vlan_mbr = self.add_vlan_member(self.device, vlan_handle=self.vlan20, member_handle=self.port10, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
                vlan_mbr = self.add_vlan_member(self.device, vlan_handle=self.vlan30, member_handle=self.port12, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
                SVIEcmpTunnelUntaggedTests()
                SVIEcmpTunnelTaggedTests()
            finally:
                # Remove port10 and port12 from VLAN20 and VLAN30
                self.cleanlast()
                self.cleanlast()

            try:
                SVIEcmpTunnelUpdateTests()
            finally:
                pass
        finally:
            self.clean_to_object(self.urif7)
            self.attribute_set(self.port9, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.port11, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            # Remove port9 and port11 from VLAN20 and VLAN30
            self.cleanlast()
            self.cleanlast()

    def tearDown(self):
        if self.client.is_feature_enable(SWITCH_FEATURE_IPV6_TUNNEL) or \
            self.client.is_feature_enable(SWITCH_FEATURE_TUNNEL_ENCAP):
            self.attribute_set(self.port2, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.cleanup()

    def L3IntfIPv4Test(self):
        print("L3IntfIPv4Test()")
        try:
            print("Sending packet from Access port 0 to VxLan port 1")
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst=self.vm1_ip4,
                ip_src=self.customer_ip4,
                ip_id=108,
                ip_ttl=64)
            inner_pkt = simple_tcp_packet(
                eth_dst=self.inner_dmac,  # 00:33:33:33:33:33
                eth_src=self.default_rmac,
                ip_dst=self.vm1_ip4,
                ip_src=self.customer_ip4,
                ip_id=108,
                ip_ttl=63)
            vxlan_pkt = mask.Mask(simple_vxlanv6_packet(
                eth_src='00:77:66:55:44:33',
                eth_dst=self.underlay_neighbor_mac1,  # 00:11:11:11:11:11
                ipv6_src=self.my_lb_ip,
                ipv6_dst=self.tunnel1_ip,
                ipv6_hlim=64,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=inner_pkt))
            mask_set_do_not_care_packet(vxlan_pkt, UDP, "sport")
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, vxlan_pkt, self.devports[1])

            print("Sending packet from Vxlan port 1 to Access port 0")
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src=self.inner_dmac,
                ip_dst=self.customer_ip4,
                ip_src=self.vm1_ip4,
                ip_id=108,
                ip_ttl=64)
            pkt2 = simple_tcp_packet(
                eth_dst='00:22:22:22:22:22',
                eth_src='00:77:66:55:44:33',
                ip_dst=self.customer_ip4,
                ip_src=self.vm1_ip4,
                ip_id=108,
                ip_ttl=63)
            vxlan_pkt = simple_vxlanv6_packet(
                eth_src=self.underlay_neighbor_mac1,
                eth_dst='00:77:66:55:44:33',
                ipv6_dst=self.my_lb_ip,
                ipv6_src=self.tunnel1_ip,
                ipv6_hlim=64,
                udp_sport=11638,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=pkt)
            send_packet(self, self.devports[1], vxlan_pkt)
            verify_packet(self, pkt2, self.devports[0])
        finally:
            print()
            pass

    def L3IntfIPv6Test(self):
        print("L3IntfIPVv6Test()")
        try:
            print("Sending packet from Access port 0 to VxLan port 1")
            pkt = simple_tcpv6_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ipv6_dst=self.vm1_ip6,
                ipv6_src=self.customer_ip6,
                ipv6_hlim=64)
            inner_pkt = simple_tcpv6_packet(
                eth_dst=self.inner_dmac,  # 00:33:33:33:33:33
                eth_src=self.default_rmac,
                ipv6_dst=self.vm1_ip6,
                ipv6_src=self.customer_ip6,
                ipv6_hlim=63)
            vxlan_pkt = mask.Mask(simple_vxlanv6_packet(
                eth_src='00:77:66:55:44:33',
                eth_dst=self.underlay_neighbor_mac1,  # 00:11:11:11:11:11
                ipv6_src=self.my_lb_ip,
                ipv6_dst=self.tunnel1_ip,
                ipv6_hlim=64,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=inner_pkt))
            mask_set_do_not_care_packet(vxlan_pkt, UDP, "sport")
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, vxlan_pkt, self.devports[1])

            print("Sending packet from Vxlan port 1 to Access port 0")
            pkt = simple_tcpv6_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src=self.inner_dmac,
                ipv6_dst=self.customer_ip6,
                ipv6_src=self.vm1_ip6,
                ipv6_hlim=64)
            pkt2 = simple_tcpv6_packet(
                eth_dst='00:22:22:22:22:22',
                eth_src='00:77:66:55:44:33',
                ipv6_dst=self.customer_ip6,
                ipv6_src=self.vm1_ip6,
                ipv6_hlim=63)
            vxlan_pkt = simple_vxlanv6_packet(
                eth_src=self.underlay_neighbor_mac1,
                eth_dst='00:77:66:55:44:33',
                ipv6_dst=self.my_lb_ip,
                ipv6_src=self.tunnel1_ip,
                ipv6_hlim=64,
                udp_sport=11638,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=pkt)
            send_packet(self, self.devports[1], vxlan_pkt)
            verify_packet(self, pkt2, self.devports[0])
        finally:
            print()
            pass

    def L3SubPortIPv4Test(self):
        print("L3SubPortIPv4Test()")
        try:
            print("Sending packet from Access port 0 to VxLan subport 3 vlan 200")
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst=self.vm7_ip4,
                ip_src=self.customer_ip4,
                ip_id=108,
                ip_ttl=64)
            inner_pkt = simple_tcp_packet(
                eth_dst=self.inner_dmac,  # 00:33:33:33:33:33
                eth_src=self.default_rmac,
                ip_dst=self.vm7_ip4,
                ip_src=self.customer_ip4,
                ip_id=108,
                ip_ttl=63)
            vxlan_pkt = mask.Mask(simple_vxlanv6_packet(
                eth_src='00:77:66:55:44:33',
                eth_dst=self.underlay_neighbor_mac9,  # 00:11:11:11:11:11
                ipv6_src=self.my_lb_ip,
                ipv6_dst=self.tunnel7_ip,
                dl_vlan_enable=True,
                vlan_vid=200,
                ipv6_hlim=64,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=inner_pkt))
            mask_set_do_not_care_packet(vxlan_pkt, UDP, "sport")
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, vxlan_pkt, self.devports[3])

            print("Sending packet from Vxlan subport 3 vlan 200 to Access port 0")
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src=self.inner_dmac,
                ip_dst=self.customer_ip4,
                ip_src=self.vm7_ip4,
                ip_id=108,
                ip_ttl=64)
            pkt2 = simple_tcp_packet(
                eth_dst='00:22:22:22:22:22',
                eth_src='00:77:66:55:44:33',
                ip_dst=self.customer_ip4,
                ip_src=self.vm7_ip4,
                ip_id=108,
                ip_ttl=63)
            vxlan_pkt = simple_vxlanv6_packet(
                eth_src=self.underlay_neighbor_mac9,
                eth_dst='00:77:66:55:44:33',
                ipv6_dst=self.my_lb_ip,
                ipv6_src=self.tunnel7_ip,
                dl_vlan_enable=True,
                vlan_vid=200,
                ipv6_hlim=64,
                udp_sport=11638,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=pkt)
            send_packet(self, self.devports[3], vxlan_pkt)
            verify_packet(self, pkt2, self.devports[0])
        finally:
            print()
            pass

    def L3SubPortIPv6Test(self):
        print("L3SubPortIPVv6Test()")
        try:
            print("Sending packet from Access port 0 to VxLan subport 3 vlan 200")
            pkt = simple_tcpv6_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ipv6_dst=self.vm7_ip6,
                ipv6_src=self.customer_ip6,
                ipv6_hlim=64)
            inner_pkt = simple_tcpv6_packet(
                eth_dst=self.inner_dmac,  # 00:33:33:33:33:33
                eth_src=self.default_rmac,
                ipv6_dst=self.vm7_ip6,
                ipv6_src=self.customer_ip6,
                ipv6_hlim=63)
            vxlan_pkt = mask.Mask(simple_vxlanv6_packet(
                eth_src='00:77:66:55:44:33',
                eth_dst=self.underlay_neighbor_mac9,  # 00:11:11:11:11:11
                ipv6_src=self.my_lb_ip,
                ipv6_dst=self.tunnel7_ip,
                dl_vlan_enable=True,
                vlan_vid=200,
                ipv6_hlim=64,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=inner_pkt))
            mask_set_do_not_care_packet(vxlan_pkt, UDP, "sport")
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, vxlan_pkt, self.devports[3])

            print("Sending packet from Vxlan subport 3 vlan 200 to Access port 0")
            pkt = simple_tcpv6_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src=self.inner_dmac,
                ipv6_dst=self.customer_ip6,
                ipv6_src=self.vm7_ip6,
                ipv6_hlim=64)
            pkt2 = simple_tcpv6_packet(
                eth_dst='00:22:22:22:22:22',
                eth_src='00:77:66:55:44:33',
                ipv6_dst=self.customer_ip6,
                ipv6_src=self.vm7_ip6,
                ipv6_hlim=63)
            vxlan_pkt = simple_vxlanv6_packet(
                eth_src=self.underlay_neighbor_mac9,
                eth_dst='00:77:66:55:44:33',
                ipv6_dst=self.my_lb_ip,
                ipv6_src=self.tunnel7_ip,
                dl_vlan_enable=True,
                vlan_vid=200,
                ipv6_hlim=64,
                udp_sport=11638,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=pkt)
            send_packet(self, self.devports[3], vxlan_pkt)
            verify_packet(self, pkt2, self.devports[0])
        finally:
            print()
            pass

    def SVITunnelIPv4Test(self, negative_test=False):
        print("SVITunnelIPv4Test()")
        try:
            print("Sending packet from Access port 0 to VxLan port 2")
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst=self.vm2_ip4,
                ip_src=self.customer_ip4,
                ip_id=108,
                ip_ttl=64)
            inner_pkt = simple_tcp_packet(
                eth_dst=self.inner_dmac,  # 00:33:33:33:33:33
                eth_src=self.default_rmac,
                ip_dst=self.vm2_ip4,
                ip_src=self.customer_ip4,
                ip_id=108,
                ip_ttl=63)
            vxlan_pkt = mask.Mask(simple_vxlanv6_packet(
                eth_src='00:77:66:55:44:33',
                eth_dst=self.underlay_neighbor_mac2,  # 00:11:11:11:11:22
                ipv6_src=self.my_lb_ip,
                ipv6_dst=self.tunnel2_ip,
                ipv6_hlim=64,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=inner_pkt))
            mask_set_do_not_care_packet(vxlan_pkt, UDP, "sport")
            send_packet(self, self.devports[0], pkt)
            if(negative_test):
                print("Verifying Packet is not forwarded to any port")
                verify_no_other_packets(self, timeout=1)
            else:
                verify_packet(self, vxlan_pkt, self.devports[2])

            if(not negative_test):
                print("Sending packet from Vxlan port 2 to Access port 0")
                pkt = simple_tcp_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src=self.inner_dmac,
                    ip_dst=self.customer_ip4,
                    ip_src=self.vm2_ip4,
                    ip_id=108,
                    ip_ttl=64)
                pkt2 = simple_tcp_packet(
                    eth_dst='00:22:22:22:22:22',
                    eth_src='00:77:66:55:44:33',
                    ip_dst=self.customer_ip4,
                    ip_src=self.vm2_ip4,
                    ip_id=108,
                    ip_ttl=63)
                vxlan_pkt = simple_vxlanv6_packet(
                    eth_src=self.underlay_neighbor_mac2,
                    eth_dst='00:77:66:55:44:33',
                    ipv6_dst=self.my_lb_ip,
                    ipv6_src=self.tunnel2_ip,
                    ipv6_hlim=64,
                    udp_sport=11638,
                    with_udp_chksum=False,
                    vxlan_vni=self.vni,
                    inner_frame=pkt)
                send_packet(self, self.devports[2], vxlan_pkt)
                verify_packet(self, pkt2, self.devports[0])
        finally:
            print()
            pass

    def SVITunnelIPv6Test(self, negative_test=False):
        print("SVITunnelIPv6Test()")
        try:
            print("Sending packet from Access port 0 to VxLan port 2")
            pkt = simple_tcpv6_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ipv6_dst=self.vm2_ip6,
                ipv6_src=self.customer_ip6,
                ipv6_hlim=64)
            inner_pkt = simple_tcpv6_packet(
                eth_dst=self.inner_dmac,  # 00:33:33:33:33:33
                eth_src=self.default_rmac,
                ipv6_dst=self.vm2_ip6,
                ipv6_src=self.customer_ip6,
                ipv6_hlim=63)
            vxlan_pkt = mask.Mask(simple_vxlanv6_packet(
                eth_src='00:77:66:55:44:33',
                eth_dst=self.underlay_neighbor_mac2,  # 00:11:11:11:11:22
                ipv6_src=self.my_lb_ip,
                ipv6_dst=self.tunnel2_ip,
                ipv6_hlim=64,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=inner_pkt))
            mask_set_do_not_care_packet(vxlan_pkt, UDP, "sport")
            send_packet(self, self.devports[0], pkt)
            if(negative_test):
                print("Verifying Packet is not forwarded to any port")
                verify_no_other_packets(self, timeout=1)
            else:
                verify_packet(self, vxlan_pkt, self.devports[2])

            if(not negative_test):
                print("Sending packet from Vxlan port 2 to Access port 0")
                pkt = simple_tcpv6_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src=self.inner_dmac,
                    ipv6_dst=self.customer_ip6,
                    ipv6_src=self.vm2_ip6,
                    ipv6_hlim=64)
                pkt2 = simple_tcpv6_packet(
                    eth_dst='00:22:22:22:22:22',
                    eth_src='00:77:66:55:44:33',
                    ipv6_dst=self.customer_ip6,
                    ipv6_src=self.vm2_ip6,
                    ipv6_hlim=63)
                vxlan_pkt = simple_vxlanv6_packet(
                    eth_src=self.underlay_neighbor_mac2,
                    eth_dst='00:77:66:55:44:33',
                    ipv6_dst=self.my_lb_ip,
                    ipv6_src=self.tunnel2_ip,
                    ipv6_hlim=64,
                    udp_sport=11638,
                    with_udp_chksum=False,
                    vxlan_vni=self.vni,
                    inner_frame=pkt)
                send_packet(self, self.devports[2], vxlan_pkt)
                verify_packet(self, pkt2, self.devports[0])
        finally:
            print()
            pass

    def ECMPTunnelIPv4Test(self):
        print("ECMPTunnelIPv4Test()")
        try:
            print("Sending packet from Access port 0 to VxLan port 3, 4")
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst=self.vm3_ip4,
                ip_src=self.customer_ip4,
                ip_id=108,
                ip_ttl=64)
            inner_pkt = simple_tcp_packet(
                eth_dst=self.inner_dmac,  # 00:33:33:33:33:33
                eth_src=self.default_rmac,
                ip_dst=self.vm3_ip4,
                ip_src=self.customer_ip4,
                ip_id=108,
                ip_ttl=63)
            vxlan_pkt1 = mask.Mask(simple_vxlanv6_packet(
                eth_src='00:77:66:55:44:33',
                eth_dst=self.underlay_neighbor_mac3,  # 00:11:11:11:11:33
                ipv6_src=self.my_lb_ip,
                ipv6_dst=self.tunnel3_ip,
                ipv6_hlim=64,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=inner_pkt))
            mask_set_do_not_care_packet(vxlan_pkt1, UDP, "sport")
            vxlan_pkt2 = mask.Mask(simple_vxlanv6_packet(
                eth_src='00:77:66:55:44:33',
                eth_dst=self.underlay_neighbor_mac4,  # 00:11:11:11:11:44
                ipv6_src=self.my_lb_ip,
                ipv6_dst=self.tunnel3_ip,
                ipv6_hlim=64,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=inner_pkt))
            mask_set_do_not_care_packet(vxlan_pkt2, UDP, "sport")
            send_packet(self, self.devports[0], pkt)
            rcv_idx = verify_any_packet_on_ports_list(
                self, [vxlan_pkt1, vxlan_pkt2], [[self.devports[3], self.devports[4]]], timeout=2)

            print("Sending packet from Vxlan port 3 to Access port 0")
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src=self.inner_dmac,
                ip_dst=self.customer_ip4,
                ip_src=self.vm3_ip4,
                ip_id=108,
                ip_ttl=64)
            pkt2 = simple_tcp_packet(
                eth_dst='00:22:22:22:22:22',
                eth_src='00:77:66:55:44:33',
                ip_dst=self.customer_ip4,
                ip_src=self.vm3_ip4,
                ip_id=108,
                ip_ttl=63)
            vxlan_pkt = simple_vxlanv6_packet(
                eth_src=self.underlay_neighbor_mac3,
                eth_dst='00:77:66:55:44:33',
                ipv6_dst=self.my_lb_ip,
                ipv6_src=self.tunnel3_ip,
                ipv6_hlim=64,
                udp_sport=11638,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=pkt)
            send_packet(self, self.devports[3], vxlan_pkt)
            verify_packet(self, pkt2, self.devports[0])

            print("Sending packet from Vxlan port 4 to Access port 0")
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src=self.inner_dmac,
                ip_dst=self.customer_ip4,
                ip_src=self.vm3_ip4,
                ip_id=108,
                ip_ttl=64)
            pkt2 = simple_tcp_packet(
                eth_dst='00:22:22:22:22:22',
                eth_src='00:77:66:55:44:33',
                ip_dst=self.customer_ip4,
                ip_src=self.vm3_ip4,
                ip_id=108,
                ip_ttl=63)
            vxlan_pkt = simple_vxlanv6_packet(
                eth_src=self.underlay_neighbor_mac4,
                eth_dst='00:77:66:55:44:33',
                ipv6_dst=self.my_lb_ip,
                ipv6_src=self.tunnel3_ip,
                ipv6_hlim=64,
                udp_sport=11638,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=pkt)
            send_packet(self, self.devports[4], vxlan_pkt)
            verify_packet(self, pkt2, self.devports[0])
        finally:
            print()
            pass

    def ECMPTunnelIPv6Test(self):
        print("ECMPTunnelIPv6Test()")
        try:
            print("Sending packet from Access port 0 to VxLan port 3, 4")
            pkt = simple_tcpv6_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ipv6_dst=self.vm3_ip6,
                ipv6_src=self.customer_ip6,
                ipv6_hlim=64)
            inner_pkt = simple_tcpv6_packet(
                eth_dst=self.inner_dmac,  # 00:33:33:33:33:33
                eth_src=self.default_rmac,
                ipv6_dst=self.vm3_ip6,
                ipv6_src=self.customer_ip6,
                ipv6_hlim=63)
            vxlan_pkt1 = mask.Mask(simple_vxlanv6_packet(
                eth_src='00:77:66:55:44:33',
                eth_dst=self.underlay_neighbor_mac3,  # 00:11:11:11:11:33
                ipv6_src=self.my_lb_ip,
                ipv6_dst=self.tunnel3_ip,
                ipv6_hlim=64,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=inner_pkt))
            mask_set_do_not_care_packet(vxlan_pkt1, UDP, "sport")
            vxlan_pkt2 = mask.Mask(simple_vxlanv6_packet(
                eth_src='00:77:66:55:44:33',
                eth_dst=self.underlay_neighbor_mac4,  # 00:11:11:11:11:44
                ipv6_src=self.my_lb_ip,
                ipv6_dst=self.tunnel3_ip,
                ipv6_hlim=64,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=inner_pkt))
            mask_set_do_not_care_packet(vxlan_pkt2, UDP, "sport")
            send_packet(self, self.devports[0], pkt)
            rcv_idx = verify_any_packet_on_ports_list(
                self, [vxlan_pkt1, vxlan_pkt2], [[self.devports[3], self.devports[4]]], timeout=2)

            print("Sending packet from Vxlan port 3 to Access port 0")
            pkt = simple_tcpv6_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src=self.inner_dmac,
                ipv6_dst=self.customer_ip6,
                ipv6_src=self.vm3_ip6,
                ipv6_hlim=64)
            pkt2 = simple_tcpv6_packet(
                eth_dst='00:22:22:22:22:22',
                eth_src='00:77:66:55:44:33',
                ipv6_dst=self.customer_ip6,
                ipv6_src=self.vm3_ip6,
                ipv6_hlim=63)
            vxlan_pkt = simple_vxlanv6_packet(
                eth_src=self.underlay_neighbor_mac3,
                eth_dst='00:77:66:55:44:33',
                ipv6_dst=self.my_lb_ip,
                ipv6_src=self.tunnel3_ip,
                ipv6_hlim=64,
                udp_sport=11638,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=pkt)
            send_packet(self, self.devports[3], vxlan_pkt)
            verify_packet(self, pkt2, self.devports[0])

            print("Sending packet from Vxlan port 4 to Access port 0")
            pkt = simple_tcpv6_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src=self.inner_dmac,
                ipv6_dst=self.customer_ip6,
                ipv6_src=self.vm3_ip6,
                ipv6_hlim=64)
            pkt2 = simple_tcpv6_packet(
                eth_dst='00:22:22:22:22:22',
                eth_src='00:77:66:55:44:33',
                ipv6_dst=self.customer_ip6,
                ipv6_src=self.vm3_ip6,
                ipv6_hlim=63)
            vxlan_pkt = simple_vxlanv6_packet(
                eth_src=self.underlay_neighbor_mac4,
                eth_dst='00:77:66:55:44:33',
                ipv6_dst=self.my_lb_ip,
                ipv6_src=self.tunnel3_ip,
                ipv6_hlim=64,
                udp_sport=11638,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=pkt)
            send_packet(self, self.devports[4], vxlan_pkt)
            verify_packet(self, pkt2, self.devports[0])
        finally:
            print()
            pass

    def LAGTunnelIPv4Test(self):
        print("LAGTunnelIPv4Test()")
        try:
            print("Sending packet from Access port 0 to VxLan port 5, 6")
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst=self.vm4_ip4,
                ip_src=self.customer_ip4,
                ip_id=108,
                ip_ttl=64)
            inner_pkt = simple_tcp_packet(
                eth_dst=self.inner_dmac,  # 00:33:33:33:33:33
                eth_src=self.default_rmac,
                ip_dst=self.vm4_ip4,
                ip_src=self.customer_ip4,
                ip_id=108,
                ip_ttl=63)
            vxlan_pkt1 = mask.Mask(simple_vxlanv6_packet(
                eth_src='00:77:66:55:44:33',
                eth_dst=self.underlay_neighbor_mac5,  # 00:11:11:11:11:55
                ipv6_src=self.my_lb_ip,
                ipv6_dst=self.tunnel4_ip,
                ipv6_hlim=64,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=inner_pkt))
            mask_set_do_not_care_packet(vxlan_pkt1, UDP, "sport")
            send_packet(self, self.devports[0], pkt)
            rcv_idx = verify_any_packet_on_ports_list(
                self, [vxlan_pkt1], [[self.devports[5], self.devports[6]]], timeout=2)

            print("Sending packet from Vxlan port 5 to Access port 0")
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src=self.inner_dmac,
                ip_dst=self.customer_ip4,
                ip_src=self.vm4_ip4,
                ip_id=108,
                ip_ttl=64)
            pkt2 = simple_tcp_packet(
                eth_dst='00:22:22:22:22:22',
                eth_src='00:77:66:55:44:33',
                ip_dst=self.customer_ip4,
                ip_src=self.vm4_ip4,
                ip_id=108,
                ip_ttl=63)
            vxlan_pkt = simple_vxlanv6_packet(
                eth_src=self.underlay_neighbor_mac5,
                eth_dst='00:77:66:55:44:33',
                ipv6_dst=self.my_lb_ip,
                ipv6_src=self.tunnel4_ip,
                ipv6_hlim=64,
                udp_sport=11638,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=pkt)
            send_packet(self, self.devports[5], vxlan_pkt)
            verify_packet(self, pkt2, self.devports[0])

            print("Sending packet from Vxlan port 6 to Access port 0")
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src=self.inner_dmac,
                ip_dst=self.customer_ip4,
                ip_src=self.vm4_ip4,
                ip_id=108,
                ip_ttl=64)
            pkt2 = simple_tcp_packet(
                eth_dst='00:22:22:22:22:22',
                eth_src='00:77:66:55:44:33',
                ip_dst=self.customer_ip4,
                ip_src=self.vm4_ip4,
                ip_id=108,
                ip_ttl=63)
            vxlan_pkt = simple_vxlanv6_packet(
                eth_src=self.underlay_neighbor_mac5,
                eth_dst='00:77:66:55:44:33',
                ipv6_dst=self.my_lb_ip,
                ipv6_src=self.tunnel4_ip,
                ipv6_hlim=64,
                udp_sport=11638,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=pkt)
            send_packet(self, self.devports[6], vxlan_pkt)
            verify_packet(self, pkt2, self.devports[0])
        finally:
            print()
            pass

    def LAGTunnelIPv6Test(self):
        print("LAGTunnelIPv6Test()")
        try:
            print("Sending packet from Access port 0 to VxLan port 5, 6")
            pkt = simple_tcpv6_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ipv6_dst=self.vm4_ip6,
                ipv6_src=self.customer_ip6,
                ipv6_hlim=64)
            inner_pkt = simple_tcpv6_packet(
                eth_dst=self.inner_dmac,  # 00:33:33:33:33:33
                eth_src=self.default_rmac,
                ipv6_dst=self.vm4_ip6,
                ipv6_src=self.customer_ip6,
                ipv6_hlim=63)
            vxlan_pkt1 = mask.Mask(simple_vxlanv6_packet(
                eth_src='00:77:66:55:44:33',
                eth_dst=self.underlay_neighbor_mac5,  # 00:11:11:11:11:55
                ipv6_src=self.my_lb_ip,
                ipv6_dst=self.tunnel4_ip,
                ipv6_hlim=64,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=inner_pkt))
            mask_set_do_not_care_packet(vxlan_pkt1, UDP, "sport")
            send_packet(self, self.devports[0], pkt)
            rcv_idx = verify_any_packet_on_ports_list(
                self, [vxlan_pkt1], [[self.devports[5], self.devports[6]]], timeout=2)

            print("Sending packet from Vxlan port 5 to Access port 0")
            pkt = simple_tcpv6_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src=self.inner_dmac,
                ipv6_dst=self.customer_ip6,
                ipv6_src=self.vm4_ip6,
                ipv6_hlim=64)
            pkt2 = simple_tcpv6_packet(
                eth_dst='00:22:22:22:22:22',
                eth_src='00:77:66:55:44:33',
                ipv6_dst=self.customer_ip6,
                ipv6_src=self.vm4_ip6,
                ipv6_hlim=63)
            vxlan_pkt = simple_vxlanv6_packet(
                eth_src=self.underlay_neighbor_mac5,
                eth_dst='00:77:66:55:44:33',
                ipv6_dst=self.my_lb_ip,
                ipv6_src=self.tunnel4_ip,
                ipv6_hlim=64,
                udp_sport=11638,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=pkt)
            send_packet(self, self.devports[5], vxlan_pkt)
            verify_packet(self, pkt2, self.devports[0])

            print("Sending packet from Vxlan port 6 to Access port 0")
            pkt = simple_tcpv6_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src=self.inner_dmac,
                ipv6_dst=self.customer_ip6,
                ipv6_src=self.vm4_ip6,
                ipv6_hlim=64)
            pkt2 = simple_tcpv6_packet(
                eth_dst='00:22:22:22:22:22',
                eth_src='00:77:66:55:44:33',
                ipv6_dst=self.customer_ip6,
                ipv6_src=self.vm4_ip6,
                ipv6_hlim=63)
            vxlan_pkt = simple_vxlanv6_packet(
                eth_src=self.underlay_neighbor_mac5,
                eth_dst='00:77:66:55:44:33',
                ipv6_dst=self.my_lb_ip,
                ipv6_src=self.tunnel4_ip,
                ipv6_hlim=64,
                udp_sport=11638,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=pkt)
            send_packet(self, self.devports[6], vxlan_pkt)
            verify_packet(self, pkt2, self.devports[0])
        finally:
            print()
            pass

    def EcmpLagTunnelIPv4Test(self, port_list=[], distribution=False):
        print("EcmpLagTunnelIPv4Test()")
        if not port_list:
            print("Port list unspecified skipping")
            return
        if distribution is True:
            print("Running Distribution Tests")
        try:
            print("Sending packets from Access port 0 to VxLan ports {}".format(port_list))
            ports=[]
            for port in port_list:
                ports.append(self.devports[port])
            count = [0] * len(port_list)
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst=self.vm5_ip4,
                ip_src=self.customer_ip4,
                ip_id=108,
                ip_ttl=64)
            inner_pkt = simple_tcp_packet(
                eth_dst=self.inner_dmac,  # 00:33:33:33:33:33
                eth_src=self.default_rmac,
                ip_dst=self.vm5_ip4,
                ip_src=self.customer_ip4,
                ip_id=108,
                ip_ttl=63)
            max_itrs = 1
            if distribution is True:
                max_itrs = 40
            src_ip = int(binascii.hexlify(socket.inet_aton(self.customer_ip4)), 16)
            for i in range(0, max_itrs):
                src_ip_addr = socket.inet_ntoa(
                    binascii.unhexlify(format(src_ip, 'x').zfill(8)))
                pkt[IP].src=src_ip_addr
                inner_pkt[IP].src=src_ip_addr
                vxlan_pkt1 = mask.Mask(simple_vxlanv6_packet(
                    eth_src='00:77:66:55:44:33',
                    eth_dst=self.underlay_neighbor_mac6,  # 00:11:11:11:11:66
                    ipv6_src=self.my_lb_ip,
                    ipv6_dst=self.tunnel5_ip,
                    ipv6_hlim=64,
                    with_udp_chksum=False,
                    vxlan_vni=self.vni,
                    inner_frame=inner_pkt))
                mask_set_do_not_care_packet(vxlan_pkt1, UDP, "sport")
                send_packet(self, self.devports[0], pkt)
                rcv_idx, _ = verify_packet_any_port(self, vxlan_pkt1, ports, timeout=2)
                count[rcv_idx] += 1
                src_ip += 1
            print("Packet distribution with {} packets on ports {} = {}".format(max_itrs, port_list, count))
            if distribution is True:
                for c in count:
                    self.assertTrue((c >= ((max_itrs / len(port_list)) * 0.5)),
                                    "Lag path is not equally balanced")

            for port, devport in zip(port_list, ports):
                print("Sending packet from Vxlan port {} to Access port 0".format(port))
                pkt = simple_tcp_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src=self.inner_dmac,
                    ip_dst=self.customer_ip4,
                    ip_src=self.vm5_ip4,
                    ip_id=108,
                    ip_ttl=64)
                pkt2 = simple_tcp_packet(
                    eth_dst='00:22:22:22:22:22',
                    eth_src='00:77:66:55:44:33',
                    ip_dst=self.customer_ip4,
                    ip_src=self.vm5_ip4,
                    ip_id=108,
                    ip_ttl=63)
                vxlan_pkt = simple_vxlanv6_packet(
                    eth_src=self.underlay_neighbor_mac6,
                    eth_dst='00:77:66:55:44:33',
                    ipv6_dst=self.my_lb_ip,
                    ipv6_src=self.tunnel5_ip,
                    ipv6_hlim=64,
                    udp_sport=11638,
                    with_udp_chksum=False,
                    vxlan_vni=self.vni,
                    inner_frame=pkt)
                send_packet(self, devport, vxlan_pkt)
                verify_packet(self, pkt2, self.devports[0])
        finally:
            print()
            pass

    def EcmpLagTunnelIPv6Test(self, port_list=[], distribution=False):
        print("EcmpLagTunnelIPv6Test()")
        if not port_list:
            print("Port list unspecified skipping")
            return
        if distribution is True:
            print("Running Distribution Tests")
        try:
            print("Sending packets from Access port 0 to VxLan ports {}".format(port_list))
            ports=[]
            for port in port_list:
                ports.append(self.devports[port])
            count = [0] * len(port_list)
            pkt = simple_tcpv6_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ipv6_dst=self.vm5_ip6,
                ipv6_src=self.customer_ip6,
                ipv6_hlim=64)
            inner_pkt = simple_tcpv6_packet(
                eth_dst=self.inner_dmac,  # 00:33:33:33:33:33
                eth_src=self.default_rmac,
                ipv6_dst=self.vm5_ip6,
                ipv6_src=self.customer_ip6,
                ipv6_hlim=63)
            max_itrs=1
            if distribution is True:
                max_itrs=40
            src_ip6 = int(binascii.hexlify(socket.inet_pton(socket.AF_INET6, self.customer_ip6)), 16)
            for i in range(0, max_itrs):
                src_ip6_addr = socket.inet_ntop(socket.AF_INET6,
                    binascii.unhexlify(format(src_ip6, 'x').zfill(16)))
                pkt[IPv6].src=src_ip6_addr
                inner_pkt[IPv6].src=src_ip6_addr
                vxlan_pkt1 = mask.Mask(simple_vxlanv6_packet(
                    eth_src='00:77:66:55:44:33',
                    eth_dst=self.underlay_neighbor_mac6,  # 00:11:11:11:11:66
                    ipv6_src=self.my_lb_ip,
                    ipv6_dst=self.tunnel5_ip,
                    ipv6_hlim=64,
                    with_udp_chksum=False,
                    vxlan_vni=self.vni,
                    inner_frame=inner_pkt))
                mask_set_do_not_care_packet(vxlan_pkt1, UDP, "sport")
                send_packet(self, self.devports[0], pkt)
                rcv_idx, _ = verify_packet_any_port(self, vxlan_pkt1, ports, timeout=2)
                count[rcv_idx] += 1
                src_ip6 += 1
            print("Packet distribution with {} packets on ports {} = {}".format(max_itrs, port_list, count))
            if distribution is True:
                for c in count:
                    self.assertTrue((c >= ((max_itrs / len(port_list)) * 0.5)),
                                    "Lag path is not equally balanced")

            for port, devport in zip(port_list, ports):
                print("Sending packet from Vxlan port {} to Access port 0".format(port))
                pkt = simple_tcpv6_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src=self.inner_dmac,
                    ipv6_dst=self.customer_ip6,
                    ipv6_src=self.vm5_ip6,
                    ipv6_hlim=64)
                pkt2 = simple_tcpv6_packet(
                    eth_dst='00:22:22:22:22:22',
                    eth_src='00:77:66:55:44:33',
                    ipv6_dst=self.customer_ip6,
                    ipv6_src=self.vm5_ip6,
                    ipv6_hlim=63)
                vxlan_pkt = simple_vxlanv6_packet(
                    eth_src=self.underlay_neighbor_mac6,
                    eth_dst='00:77:66:55:44:33',
                    ipv6_dst=self.my_lb_ip,
                    ipv6_src=self.tunnel5_ip,
                    ipv6_hlim=64,
                    udp_sport=11638,
                    with_udp_chksum=False,
                    vxlan_vni=self.vni,
                    inner_frame=pkt)
                send_packet(self, devport, vxlan_pkt)
                verify_packet(self, pkt2, self.devports[0])
        finally:
            print()
            pass

    def SVILagTunnelIPv4Test(self):
        print("SVILagTunnelIPv4Test()")
        try:
            # We will send the VxLAN packet first so that the neighbor entry for underslay SVI rif is resolved
            # after the mac entry is learned
            upkt_in = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src=self.inner_dmac,
                ip_dst=self.customer_ip4,
                ip_src=self.vm6_ip4,
                ip_id=108,
                ip_ttl=64)
            opkt_out = simple_tcp_packet(
                eth_dst='00:22:22:22:22:22',
                eth_src='00:77:66:55:44:33',
                ip_dst=self.customer_ip4,
                ip_src=self.vm6_ip4,
                ip_id=108,
                ip_ttl=63)
            vxlan_pkt_in = simple_vxlanv6_packet(
                eth_src=self.underlay_neighbor_mac7,
                eth_dst='00:77:66:55:44:33',
                ipv6_dst=self.my_lb_ip,
                ipv6_src=self.tunnel6_ip,
                ipv6_hlim=64,
                udp_sport=11638,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=upkt_in)

            opkt_in = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst=self.vm6_ip4,
                ip_src=self.customer_ip4,
                ip_id=108,
                ip_ttl=64)
            inner_pkt = simple_tcp_packet(
                eth_dst=self.inner_dmac,  # 00:33:33:33:33:33
                eth_src=self.default_rmac,
                ip_dst=self.vm6_ip4,
                ip_src=self.customer_ip4,
                ip_id=108,
                ip_ttl=63)
            vxlan_pkt_out = mask.Mask(simple_vxlanv6_packet(
                eth_src='00:77:66:55:44:33',
                eth_dst=self.underlay_neighbor_mac7,  # 00:77:77:77:77:77
                ipv6_src=self.my_lb_ip,
                ipv6_dst=self.tunnel6_ip,
                ipv6_hlim=64,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=inner_pkt))
            mask_set_do_not_care_packet(vxlan_pkt_out, UDP, "sport")

            print("Sending packet from Vxlan Access Port 9 to Access port 0")
            send_packet(self, self.devports[9], vxlan_pkt_in)
            verify_packet(self, opkt_out, self.devports[0])
            # Glean action is not supported for outer_fib table
            # Hence we explicitly learn the mac in underlay SVI case via
            # Gratuitous ARP
            print("Sending Gratuitous ARP packet from Access Port 9")
            arp_pkt = simple_arp_packet(eth_src=self.underlay_neighbor_mac7, pktlen=100)
            send_packet(self, self.devports[9], arp_pkt)
            print("Wait for underlay RIF Nexthop entry to be resolved")
            time.sleep(2)
            print("Sending packet from Access port 0 to VxLan Access Port 9")
            send_packet(self, self.devports[0], opkt_in)
            verify_packet(self, vxlan_pkt_out, self.devports[9])

            print("Sending packet from Vxlan Lag(Port11/Port12) Access Port 11 to Access port 0")
            send_packet(self, self.devports[11], vxlan_pkt_in)
            verify_packet(self, opkt_out, self.devports[0])
            # Gratious ARP
            print("Sending Gratuitous ARP packet from Lag(Port11/Port12) Access Port 11")
            send_packet(self, self.devports[11], arp_pkt)
            print("Wait for underlay RIF Nexthop entry to be resolved")
            time.sleep(2)
            print("Sending packet from Access port 0 to VxLan Lag(Port11/Por12) port")
            send_packet(self, self.devports[0], opkt_in)
            verify_packets_any(self, vxlan_pkt_out, [self.devports[11], self.devports[12]])
        finally:
            print()
            pass

    def SVILagTunnelIPv6Test(self):
        print("SVILagTunnelIPv6Test()")
        try:
            # We will send the VxLAN packet first so that the neighbor entry for underslay SVI rif is resolved
            # after the mac entry is learned
            upkt_in = simple_tcpv6_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src=self.inner_dmac,
                ipv6_dst=self.customer_ip6,
                ipv6_src=self.vm6_ip6,
                ipv6_hlim=64)
            opkt_out = simple_tcpv6_packet(
                eth_dst='00:22:22:22:22:22',
                eth_src='00:77:66:55:44:33',
                ipv6_dst=self.customer_ip6,
                ipv6_src=self.vm6_ip6,
                ipv6_hlim=63)
            vxlan_pkt_in = simple_vxlanv6_packet(
                eth_src=self.underlay_neighbor_mac7,
                eth_dst='00:77:66:55:44:33',
                ipv6_dst=self.my_lb_ip,
                ipv6_src=self.tunnel6_ip,
                ipv6_hlim=64,
                udp_sport=11638,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=upkt_in)

            opkt_in = simple_tcpv6_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ipv6_dst=self.vm6_ip6,
                ipv6_src=self.customer_ip6,
                ipv6_hlim=64)
            inner_pkt = simple_tcpv6_packet(
                eth_dst=self.inner_dmac,  # 00:33:33:33:33:33
                eth_src=self.default_rmac,
                ipv6_dst=self.vm6_ip6,
                ipv6_src=self.customer_ip6,
                ipv6_hlim=63)
            vxlan_pkt_out = mask.Mask(simple_vxlanv6_packet(
                eth_src='00:77:66:55:44:33',
                eth_dst=self.underlay_neighbor_mac7,  # 00:77:77:77:77:77
                ipv6_src=self.my_lb_ip,
                ipv6_dst=self.tunnel6_ip,
                ipv6_hlim=64,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=inner_pkt))
            mask_set_do_not_care_packet(vxlan_pkt_out, UDP, "sport")

            print("Sending packet from Vxlan Access Port 9 to Access port 0")
            send_packet(self, self.devports[9], vxlan_pkt_in)
            verify_packet(self, opkt_out, self.devports[0])
            # Glean action is not supported for outer_fib table
            # Hence we explicitly learn the mac in underlay SVI case via
            # Gratious ARP
            print("Sending Gratious ARP packet from Access Port 9")
            arp_pkt = simple_arp_packet(eth_src=self.underlay_neighbor_mac7, pktlen=100)
            send_packet(self, self.devports[9], arp_pkt)
            print("Wait for underlay RIF Nexthop entry to be resolved")
            time.sleep(2)
            print("Sending packet from Access port 0 to VxLan Access Port 9")
            send_packet(self, self.devports[0], opkt_in)
            verify_packet(self, vxlan_pkt_out, self.devports[9])

            print("Sending packet from Vxlan Lag(Port11/Port12) Access Port 11 to Access port 0")
            send_packet(self, self.devports[11], vxlan_pkt_in)
            verify_packet(self, opkt_out, self.devports[0])
            # Gratious ARP
            print("Sending Gratious ARP packet from Lag(Port11/Port12) Access Port 11")
            send_packet(self, self.devports[11], arp_pkt)
            print("Wait for underlay RIF Nexthop entry to be resolved")
            time.sleep(2)
            print("Sending packet from Access port 0 to VxLan Lag(Port11/Por12) port")
            send_packet(self, self.devports[0], opkt_in)
            verify_packets_any(self, vxlan_pkt_out, [self.devports[11], self.devports[12]])
        finally:
            print()
            pass

    def SVIEcmpTunnelIPv4Test(self, ecmp_port1=None, tag_mode1=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_UNTAGGED, vid1=0, ecmp_port2=None, tag_mode2=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_UNTAGGED, vid2=0):
        print("SVIEcmpTunnelIPv4Test()")
        try:
            print("Sending packet from Access port 0 to VxLan port {}, {}".format(ecmp_port1, ecmp_port2))
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst=self.vm6_ip4,
                ip_src=self.customer_ip4,
                ip_id=108,
                ip_ttl=64)
            inner_pkt = simple_tcp_packet(
                eth_dst=self.inner_dmac,  # 00:33:33:33:33:33
                eth_src=self.default_rmac,
                ip_dst=self.vm6_ip4,
                ip_src=self.customer_ip4,
                ip_id=108,
                ip_ttl=63)
            if tag_mode1 is SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED:
                vxlan_pkt1 = mask.Mask(simple_vxlanv6_packet(
                    eth_src='00:77:66:55:44:33',
                    eth_dst=self.underlay_neighbor_mac7,  # 00:11:11:11:11:77
                    dl_vlan_enable=True,
                    vlan_vid=vid1,
                    ipv6_src=self.my_lb_ip,
                    ipv6_dst=self.tunnel6_ip,
                    ipv6_hlim=64,
                    with_udp_chksum=False,
                    vxlan_vni=self.vni,
                    inner_frame=inner_pkt))
                mask_set_do_not_care_packet(vxlan_pkt1, UDP, "sport")
            else:
                vxlan_pkt1 = mask.Mask(simple_vxlanv6_packet(
                    eth_src='00:77:66:55:44:33',
                    eth_dst=self.underlay_neighbor_mac7,  # 00:11:11:11:11:77
                    ipv6_src=self.my_lb_ip,
                    ipv6_dst=self.tunnel6_ip,
                    ipv6_hlim=64,
                    with_udp_chksum=False,
                    vxlan_vni=self.vni,
                    inner_frame=inner_pkt))
                mask_set_do_not_care_packet(vxlan_pkt1, UDP, "sport")
            if tag_mode2 is SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED:
                vxlan_pkt2 = mask.Mask(simple_vxlanv6_packet(
                    eth_src='00:77:66:55:44:33',
                    eth_dst=self.underlay_neighbor_mac8,  # 00:11:11:11:11:88
                    dl_vlan_enable=True,
                    vlan_vid=vid2,
                    ipv6_src=self.my_lb_ip,
                    ipv6_dst=self.tunnel6_ip,
                    ipv6_hlim=64,
                    with_udp_chksum=False,
                    vxlan_vni=self.vni,
                    inner_frame=inner_pkt))
                mask_set_do_not_care_packet(vxlan_pkt2, UDP, "sport")
            else:
                vxlan_pkt2 = mask.Mask(simple_vxlanv6_packet(
                    eth_src='00:77:66:55:44:33',
                    eth_dst=self.underlay_neighbor_mac8,  # 00:11:11:11:11:88
                    ipv6_src=self.my_lb_ip,
                    ipv6_dst=self.tunnel6_ip,
                    ipv6_hlim=64,
                    with_udp_chksum=False,
                    vxlan_vni=self.vni,
                    inner_frame=inner_pkt))
                mask_set_do_not_care_packet(vxlan_pkt2, UDP, "sport")
            send_packet(self, self.devports[0], pkt)
            port_list = []
            pkt_list = []
            if ecmp_port1 is not None:
                port_list.append(self.devports[ecmp_port1])
                pkt_list.append(vxlan_pkt1)
            if ecmp_port2 is not None:
                port_list.append(self.devports[ecmp_port2])
                pkt_list.append(vxlan_pkt2)
            rcv_idx = verify_any_packet_on_ports_list(self, pkt_list, [port_list], timeout=2)

            if ecmp_port1 is not None:
                print("Sending packet from Vxlan port {} to Access port 0".format(ecmp_port1))
                pkt = simple_tcp_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src=self.inner_dmac,
                    ip_dst=self.customer_ip4,
                    ip_src=self.vm6_ip4,
                    ip_id=108,
                    ip_ttl=64)
                pkt2 = simple_tcp_packet(
                    eth_dst='00:22:22:22:22:22',
                    eth_src='00:77:66:55:44:33',
                    ip_dst=self.customer_ip4,
                    ip_src=self.vm6_ip4,
                    ip_id=108,
                    ip_ttl=63)
                if tag_mode1 is SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED:
                    vxlan_pkt = simple_vxlanv6_packet(
                        eth_src=self.underlay_neighbor_mac7,
                        eth_dst='00:77:66:55:44:33',
                        dl_vlan_enable=True,
                        vlan_vid=vid1,
                        ipv6_dst=self.my_lb_ip,
                        ipv6_src=self.tunnel6_ip,
                        ipv6_hlim=64,
                        udp_sport=11638,
                        with_udp_chksum=False,
                        vxlan_vni=self.vni,
                        inner_frame=pkt)
                else:
                    vxlan_pkt = simple_vxlanv6_packet(
                        eth_src=self.underlay_neighbor_mac7,
                        eth_dst='00:77:66:55:44:33',
                        ipv6_dst=self.my_lb_ip,
                        ipv6_src=self.tunnel6_ip,
                        ipv6_hlim=64,
                        udp_sport=11638,
                        with_udp_chksum=False,
                        vxlan_vni=self.vni,
                        inner_frame=pkt)
                send_packet(self, self.devports[ecmp_port1], vxlan_pkt)
                verify_packet(self, pkt2, self.devports[0])

            if ecmp_port2 is not None:
                print("Sending packet from Vxlan port {} to Access port 0".format(ecmp_port2))
                pkt = simple_tcp_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src=self.inner_dmac,
                    ip_dst=self.customer_ip4,
                    ip_src=self.vm6_ip4,
                    ip_id=108,
                    ip_ttl=64)
                pkt2 = simple_tcp_packet(
                    eth_dst='00:22:22:22:22:22',
                    eth_src='00:77:66:55:44:33',
                    ip_dst=self.customer_ip4,
                    ip_src=self.vm6_ip4,
                    ip_id=108,
                    ip_ttl=63)
                if tag_mode2 is SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED:
                    vxlan_pkt = simple_vxlanv6_packet(
                        eth_src=self.underlay_neighbor_mac8,
                        eth_dst='00:77:66:55:44:33',
                        dl_vlan_enable=True,
                        vlan_vid=vid2,
                        ipv6_dst=self.my_lb_ip,
                        ipv6_src=self.tunnel6_ip,
                        ipv6_hlim=64,
                        udp_sport=11638,
                        with_udp_chksum=False,
                        vxlan_vni=self.vni,
                        inner_frame=pkt)
                else:
                    vxlan_pkt = simple_vxlanv6_packet(
                        eth_src=self.underlay_neighbor_mac8,
                        eth_dst='00:77:66:55:44:33',
                        ipv6_dst=self.my_lb_ip,
                        ipv6_src=self.tunnel6_ip,
                        ipv6_hlim=64,
                        udp_sport=11638,
                        with_udp_chksum=False,
                        vxlan_vni=self.vni,
                        inner_frame=pkt)
                send_packet(self, self.devports[ecmp_port2], vxlan_pkt)
                verify_packet(self, pkt2, self.devports[0])
        finally:
            print()
            pass

    def SVIEcmpTunnelIPv6Test(self, ecmp_port1=None, tag_mode1=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_UNTAGGED, vid1=0, ecmp_port2=None, tag_mode2=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_UNTAGGED, vid2=0):
        print("SVIEcmpTunnelIPv6Test()")
        try:
            print("Sending packet from Access port 0 to VxLan port {}, {}".format(ecmp_port1, ecmp_port2))
            pkt = simple_tcpv6_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ipv6_dst=self.vm6_ip6,
                ipv6_src=self.customer_ip6,
                ipv6_hlim=64)
            inner_pkt = simple_tcpv6_packet(
                eth_dst=self.inner_dmac,  # 00:33:33:33:33:33
                eth_src=self.default_rmac,
                ipv6_dst=self.vm6_ip6,
                ipv6_src=self.customer_ip6,
                ipv6_hlim=63)
            if tag_mode1 is SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED:
                vxlan_pkt1 = mask.Mask(simple_vxlanv6_packet(
                    eth_src='00:77:66:55:44:33',
                    eth_dst=self.underlay_neighbor_mac7,  # 00:11:11:11:11:77
                    dl_vlan_enable=True,
                    vlan_vid=vid1,
                    ipv6_src=self.my_lb_ip,
                    ipv6_dst=self.tunnel6_ip,
                    ipv6_hlim=64,
                    with_udp_chksum=False,
                    vxlan_vni=self.vni,
                    inner_frame=inner_pkt))
                mask_set_do_not_care_packet(vxlan_pkt1, UDP, "sport")
            else:
                vxlan_pkt1 = mask.Mask(simple_vxlanv6_packet(
                    eth_src='00:77:66:55:44:33',
                    eth_dst=self.underlay_neighbor_mac7,  # 00:11:11:11:11:77
                    ipv6_src=self.my_lb_ip,
                    ipv6_dst=self.tunnel6_ip,
                    ipv6_hlim=64,
                    with_udp_chksum=False,
                    vxlan_vni=self.vni,
                    inner_frame=inner_pkt))
                mask_set_do_not_care_packet(vxlan_pkt1, UDP, "sport")
            if tag_mode2 is SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED:
                vxlan_pkt2 = mask.Mask(simple_vxlanv6_packet(
                    eth_src='00:77:66:55:44:33',
                    eth_dst=self.underlay_neighbor_mac8,  # 00:11:11:11:11:88
                    dl_vlan_enable=True,
                    vlan_vid=vid2,
                    ipv6_src=self.my_lb_ip,
                    ipv6_dst=self.tunnel6_ip,
                    ipv6_hlim=64,
                    with_udp_chksum=False,
                    vxlan_vni=self.vni,
                    inner_frame=inner_pkt))
                mask_set_do_not_care_packet(vxlan_pkt2, UDP, "sport")
            else:
                vxlan_pkt2 = mask.Mask(simple_vxlanv6_packet(
                    eth_src='00:77:66:55:44:33',
                    eth_dst=self.underlay_neighbor_mac8,  # 00:11:11:11:11:88
                    ipv6_src=self.my_lb_ip,
                    ipv6_dst=self.tunnel6_ip,
                    ipv6_hlim=64,
                    with_udp_chksum=False,
                    vxlan_vni=self.vni,
                    inner_frame=inner_pkt))
                mask_set_do_not_care_packet(vxlan_pkt2, UDP, "sport")
            send_packet(self, self.devports[0], pkt)
            port_list = []
            pkt_list = []
            if ecmp_port1 is not None:
                port_list.append(self.devports[ecmp_port1])
                pkt_list.append(vxlan_pkt1)
            if ecmp_port2 is not None:
                port_list.append(self.devports[ecmp_port2])
                pkt_list.append(vxlan_pkt2)
            rcv_idx = verify_any_packet_on_ports_list(self, pkt_list, [port_list], timeout=2)

            if ecmp_port1 is not None:
                print("Sending packet from Vxlan port {} to Access port 0".format(ecmp_port1))
                pkt = simple_tcpv6_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src=self.inner_dmac,
                    ipv6_dst=self.customer_ip6,
                    ipv6_src=self.vm6_ip6,
                    ipv6_hlim=64)
                pkt2 = simple_tcpv6_packet(
                    eth_dst='00:22:22:22:22:22',
                    eth_src='00:77:66:55:44:33',
                    ipv6_dst=self.customer_ip6,
                    ipv6_src=self.vm6_ip6,
                    ipv6_hlim=63)
                if tag_mode1 is SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED:
                    vxlan_pkt = simple_vxlanv6_packet(
                        eth_src=self.underlay_neighbor_mac7,
                        eth_dst='00:77:66:55:44:33',
                        dl_vlan_enable=True,
                        vlan_vid=vid1,
                        ipv6_dst=self.my_lb_ip,
                        ipv6_src=self.tunnel6_ip,
                        ipv6_hlim=64,
                        udp_sport=11638,
                        with_udp_chksum=False,
                        vxlan_vni=self.vni,
                        inner_frame=pkt)
                else:
                    vxlan_pkt = simple_vxlanv6_packet(
                        eth_src=self.underlay_neighbor_mac7,
                        eth_dst='00:77:66:55:44:33',
                        ipv6_dst=self.my_lb_ip,
                        ipv6_src=self.tunnel6_ip,
                        ipv6_hlim=64,
                        udp_sport=11638,
                        with_udp_chksum=False,
                        vxlan_vni=self.vni,
                        inner_frame=pkt)
                send_packet(self, self.devports[ecmp_port1], vxlan_pkt)
                verify_packet(self, pkt2, self.devports[0])

            if ecmp_port2 is not None:
                print("Sending packet from Vxlan port {} to Access port 0".format(ecmp_port2))
                pkt = simple_tcpv6_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src=self.inner_dmac,
                    ipv6_dst=self.customer_ip6,
                    ipv6_src=self.vm6_ip6,
                    ipv6_hlim=64)
                pkt2 = simple_tcpv6_packet(
                    eth_dst='00:22:22:22:22:22',
                    eth_src='00:77:66:55:44:33',
                    ipv6_dst=self.customer_ip6,
                    ipv6_src=self.vm6_ip6,
                    ipv6_hlim=63)
                if tag_mode2 is SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED:
                    vxlan_pkt = simple_vxlanv6_packet(
                        eth_src=self.underlay_neighbor_mac8,
                        eth_dst='00:77:66:55:44:33',
                        dl_vlan_enable=True,
                        vlan_vid=vid2,
                        ipv6_dst=self.my_lb_ip,
                        ipv6_src=self.tunnel6_ip,
                        ipv6_hlim=64,
                        udp_sport=11638,
                        with_udp_chksum=False,
                        vxlan_vni=self.vni,
                        inner_frame=pkt)
                else:
                    vxlan_pkt = simple_vxlanv6_packet(
                        eth_src=self.underlay_neighbor_mac8,
                        eth_dst='00:77:66:55:44:33',
                        ipv6_dst=self.my_lb_ip,
                        ipv6_src=self.tunnel6_ip,
                        ipv6_hlim=64,
                        udp_sport=11638,
                        with_udp_chksum=False,
                        vxlan_vni=self.vni,
                        inner_frame=pkt)
                send_packet(self, self.devports[ecmp_port2], vxlan_pkt)
                verify_packet(self, pkt2, self.devports[0])
        finally:
            print()
            pass

###############################################################################

#@group('tunnel')
# Currently there is no notion of tunnel id (tunnel object) in the switching pipeline
# Tunnel identification is based on Tunnel Type | IP Address Family : IPv4/IPv6 | VxLAN/IPIP
# Re-enable this test when multiple tunnel support is added
@disabled
class L3MultiTunnelTest(ApiHelper):
    def runTest(self):
        print()
        if (self.client.is_feature_enable(SWITCH_FEATURE_IPV4_TUNNEL) == 0):
            print("IPv4 tunnel feature not enabled, skipping")
            return

        self.configure()

        # underlay config input
        self.uvrf1 = self.add_vrf(self.device, id=100, src_mac=self.rmac)
        self.uvrf2 = self.add_vrf(self.device, id=200, src_mac=self.rmac)
        self.vm1_ip = '100.100.1.1'
        self.vm2_ip = '200.200.1.1'
        self.customer1_ip = '100.100.0.1'
        self.customer2_ip = '200.200.0.1'
        self.vni1 = 1000
        self.vni2 = 2000
        self.my_lb1_ip = '10.10.10.10'
        self.my_lb2_ip = '20.20.20.20'
        self.tunnel1_ip = '10.10.10.1'
        self.tunnel2_ip = '20.20.20.1'
        self.vxlan_global_mac = "00:22:22:22:22:22"
        self.inner_dmac = "00:33:33:33:33:33"
        self.underlay_neighbor_mac1 = '00:11:11:11:11:11'
        self.underlay_neighbor_mac2 = '00:11:11:11:11:22'
        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_TUNNEL_DMAC, self.vxlan_global_mac)

        # overlay configuration
        self.ovrf = self.add_vrf(self.device, id = 300, src_mac=self.rmac)
        self.tunnel_type = SWITCH_TUNNEL_ATTR_TYPE_VXLAN
        self.term_type = SWITCH_TUNNEL_TERM_ATTR_TERMINATION_TYPE_P2MP
        self.encap_ttl_mode = SWITCH_TUNNEL_ATTR_ENCAP_TTL_MODE_PIPE_MODEL
        self.decap_ttl_mode = SWITCH_TUNNEL_ATTR_DECAP_TTL_MODE_PIPE_MODEL

        # create underlay loopback rif for tunnel
        self.urif1_lb = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_LOOPBACK, vrf_handle=self.uvrf1, src_mac=self.rmac)
        self.urif2_lb = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_LOOPBACK, vrf_handle=self.uvrf2, src_mac=self.rmac)
        self.orif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port0, vrf_handle=self.ovrf, src_mac=self.rmac)

        # Create route to tunnel ip 10.10.10.1
        self.urif1 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port1, vrf_handle=self.uvrf1, src_mac=self.rmac)
        self.unhop1 = self.add_nexthop(self.device, handle=self.urif1, dest_ip=self.tunnel1_ip)
        self.uneighbor1 = self.add_neighbor(self.device,
            mac_address=self.underlay_neighbor_mac1,
            handle=self.urif1,
            dest_ip=self.tunnel1_ip)  # 10.10.10.1
        self.tunnel_route1 = self.add_route(self.device, ip_prefix="10.10.10.1",
            vrf_handle=self.uvrf1, nexthop_handle=self.unhop1)

        # Create route to tunnel ip 20.20.20.1
        self.urif2 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port2, vrf_handle=self.uvrf2, src_mac=self.rmac)
        self.unhop2 = self.add_nexthop(self.device, handle=self.urif2, dest_ip=self.tunnel2_ip)
        self.uneighbor2 = self.add_neighbor(self.device,
            mac_address=self.underlay_neighbor_mac2,
            handle=self.urif2,
            dest_ip=self.tunnel2_ip)  # 20.20.20.1
        self.tunnel_route1 = self.add_route(self.device, ip_prefix="20.20.20.1",
            vrf_handle=self.uvrf2, nexthop_handle=self.unhop2)

        # Encap configuration follows

        # create Encap/Decap mappers
        self.encap_tunnel_map = self.add_tunnel_mapper(self.device,
            type=SWITCH_TUNNEL_MAPPER_ATTR_TYPE_VRF_HANDLE_TO_VNI)
        self.decap_tunnel_map = self.add_tunnel_mapper(self.device,
            type=SWITCH_TUNNEL_MAPPER_ATTR_TYPE_VNI_TO_VRF_HANDLE)

        # create Encap/Decap mapper entries for ovrf
        self.encap_tunnel_map_entry = self.add_tunnel_mapper_entry(self.device,
            type=SWITCH_TUNNEL_MAPPER_ENTRY_ATTR_TYPE_VRF_HANDLE_TO_VNI,
            tunnel_mapper_handle=self.encap_tunnel_map,
            network_handle=self.ovrf,
            tunnel_vni=self.vni1)
        self.decap_tunnel_map_entry1 = self.add_tunnel_mapper_entry(self.device,
            type=SWITCH_TUNNEL_MAPPER_ENTRY_ATTR_TYPE_VNI_TO_VRF_HANDLE,
            tunnel_mapper_handle=self.decap_tunnel_map,
            network_handle=self.ovrf,
            tunnel_vni=self.vni1)

        ingress_mapper_list = [switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=self.encap_tunnel_map)]
        egress_mapper_list = [switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=self.decap_tunnel_map)]

        # create Tunnel
        self.tunnel1 = self.add_tunnel(self.device,
            type=self.tunnel_type,
            src_ip=self.my_lb1_ip,
            ingress_mapper_handles=ingress_mapper_list,
            egress_mapper_handles=egress_mapper_list,
            encap_ttl_mode=self.encap_ttl_mode,
            decap_ttl_mode=self.decap_ttl_mode,
            underlay_rif_handle=self.urif1_lb)
        self.tunnel2 = self.add_tunnel(self.device,
            type=self.tunnel_type,
            src_ip=self.my_lb2_ip,
            ingress_mapper_handles=ingress_mapper_list,
            egress_mapper_handles=egress_mapper_list,
            encap_ttl_mode=self.encap_ttl_mode,
            decap_ttl_mode=self.decap_ttl_mode,
            underlay_rif_handle=self.urif2_lb)

        # Decap configuration follows

        # Create tunnel_termination entry
        self.tunnel_term1 = self.add_tunnel_term(self.device,
            type=self.tunnel_type,
            vrf_handle=self.uvrf1,
            tunnel_handle=self.tunnel1,
            termination_type=self.term_type,
            dst_ip=self.my_lb1_ip)
        self.tunnel_term2 = self.add_tunnel_term(self.device,
            type=self.tunnel_type,
            vrf_handle=self.uvrf2,
            tunnel_handle=self.tunnel2,
            termination_type=self.term_type,
            dst_ip=self.my_lb2_ip)

        # create tunnel nexthop for VM 100.100.1.1
        # uses global vxlan dmac
        self.tunnel_nexthop1 = self.add_nexthop(self.device,
            type=SWITCH_NEXTHOP_ATTR_TYPE_TUNNEL,
            rw_type=SWITCH_NEXTHOP_ATTR_RW_TYPE_L3_VNI,
            handle=self.tunnel1,
            dest_ip=self.tunnel1_ip,  # 10.10.10.1
            tunnel_vni=self.vni1)
        # add route to tunnel_nexthop
        self.customer_route = self.add_route(self.device,
            ip_prefix=self.vm1_ip,
            vrf_handle=self.ovrf,
            nexthop_handle=self.tunnel_nexthop1)

        # create tunnel nexthop for VM 200.200.1.1
        # use vni from tunnel mapper
        self.tunnel_nexthop2 = self.add_nexthop(self.device,
            type=SWITCH_NEXTHOP_ATTR_TYPE_TUNNEL,
            rw_type=SWITCH_NEXTHOP_ATTR_RW_TYPE_L3,
            handle=self.tunnel2,
            dest_ip=self.tunnel2_ip,  # 20.20.20.1
            mac_address=self.inner_dmac)
        # add route to tunnel_nexthop
        self.customer_route = self.add_route(self.device,
            ip_prefix=self.vm2_ip,
            vrf_handle=self.ovrf,
            nexthop_handle=self.tunnel_nexthop2)

        # Create route to customer from VM
        self.onhop1 = self.add_nexthop(self.device, handle=self.orif, dest_ip=self.customer1_ip)
        self.oneighbor = self.add_neighbor(self.device,
            mac_address='00:22:22:22:22:22',
            handle=self.orif,
            dest_ip=self.customer1_ip)  # 100.100.0.1
        self.onhop2 = self.add_nexthop(self.device, handle=self.orif, dest_ip=self.customer2_ip)
        self.oneighbor = self.add_neighbor(self.device,
            mac_address='00:22:22:22:22:33',
            handle=self.orif,
            dest_ip=self.customer2_ip)  # 200.200.0.1
        self.customer_route1 = self.add_route(self.device, ip_prefix=self.customer1_ip,
            vrf_handle=self.ovrf, nexthop_handle=self.onhop1)
        self.customer_route2 = self.add_route(self.device, ip_prefix=self.customer2_ip,
            vrf_handle=self.ovrf, nexthop_handle=self.onhop2)

        try:
            self.Tunnel1Test()
            self.Tunnel2Test()
        finally:
            self.cleanup()

    def Tunnel1Test(self):
        print("Tunnel1Test()")
        try:
            print("Sending packet from Access port 0 to VxLan port 1")
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst=self.vm1_ip,
                ip_src=self.customer1_ip,
                ip_id=108,
                ip_ttl=64)
            inner_pkt = simple_tcp_packet(
                eth_dst=self.vxlan_global_mac,  # 00:22:22:22:22:22
                eth_src=self.default_rmac,
                ip_dst=self.vm1_ip,
                ip_src=self.customer1_ip,
                ip_id=108,
                ip_ttl=63)
            vxlan_pkt = mask.Mask(simple_vxlan_packet(
                eth_src='00:77:66:55:44:33',
                eth_dst=self.underlay_neighbor_mac1,  # 00:11:11:11:11:11
                ip_id=0,
                ip_src=self.my_lb1_ip,
                ip_dst=self.tunnel1_ip,
                ip_ttl=64,
                ip_flags=0x2,
                with_udp_chksum=False,
                vxlan_vni=self.vni1,
                inner_frame=inner_pkt))
            mask_set_do_not_care_packet(vxlan_pkt, UDP, "sport")
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, vxlan_pkt, self.devports[1])

            print("Sending packet from Vxlan port 1 to Access port 0")
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src=self.vxlan_global_mac,
                ip_dst=self.customer1_ip,
                ip_src=self.vm1_ip,
                ip_id=108,
                ip_ttl=64)
            pkt2 = simple_tcp_packet(
                eth_dst='00:22:22:22:22:22',
                eth_src='00:77:66:55:44:33',
                ip_dst=self.customer1_ip,
                ip_src=self.vm1_ip,
                ip_id=108,
                ip_ttl=63)
            vxlan_pkt = simple_vxlan_packet(
                eth_src=self.underlay_neighbor_mac1,
                eth_dst='00:77:66:55:44:33',
                ip_id=0,
                ip_dst=self.my_lb1_ip,
                ip_src=self.tunnel1_ip,
                ip_ttl=64,
                ip_flags=0x2,
                udp_sport=11638,
                with_udp_chksum=False,
                vxlan_vni=self.vni1,
                inner_frame=pkt)
            send_packet(self, self.devports[1], vxlan_pkt)
            verify_packet(self, pkt2, self.devports[0])
        finally:
            pass

    def Tunnel2Test(self):
        print("Tunnel2Test()")
        try:
            print("Sending packet from Access port 0 to VxLan port 2")
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:33',
                ip_dst=self.vm2_ip,
                ip_src=self.customer2_ip,
                ip_id=108,
                ip_ttl=64)
            inner_pkt = simple_tcp_packet(
                eth_dst=self.inner_dmac,  # 00:33:33:33:33:33
                eth_src='00:77:66:55:44:33',
                ip_dst=self.vm2_ip,
                ip_src=self.customer2_ip,
                ip_id=108,
                ip_ttl=63)
            vxlan_pkt = mask.Mask(simple_vxlan_packet(
                eth_src='00:77:66:55:44:33',
                eth_dst=self.underlay_neighbor_mac2,  # 00:11:11:11:11:11
                ip_id=0,
                ip_src=self.my_lb2_ip,
                ip_dst=self.tunnel2_ip,
                ip_ttl=64,
                ip_flags=0x2,
                with_udp_chksum=False,
                vxlan_vni=self.vni1,
                inner_frame=inner_pkt))
            mask_set_do_not_care_packet(vxlan_pkt, UDP, "sport")
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, vxlan_pkt, self.devports[2])

            print("Sending packet from Vxlan port 2 to Access port 0")
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src=self.inner_dmac,
                ip_dst=self.customer2_ip,
                ip_src=self.vm2_ip,
                ip_id=108,
                ip_ttl=64)
            pkt2 = simple_tcp_packet(
                eth_dst='00:22:22:22:22:33',
                eth_src='00:77:66:55:44:33',
                ip_dst=self.customer2_ip,
                ip_src=self.vm2_ip,
                ip_id=108,
                ip_ttl=63)
            vxlan_pkt = simple_vxlan_packet(
                eth_src=self.underlay_neighbor_mac1,
                eth_dst='00:77:66:55:44:33',
                ip_id=0,
                ip_dst=self.my_lb2_ip,
                ip_src=self.tunnel2_ip,
                ip_ttl=64,
                ip_flags=0x2,
                udp_sport=11638,
                with_udp_chksum=False,
                vxlan_vni=self.vni1,
                inner_frame=pkt)
            send_packet(self, self.devports[2], vxlan_pkt)
            verify_packet(self, pkt2, self.devports[0])
        finally:
            pass

###############################################################################


@group('tunnel')
class L3MultiVrfVniMapperTest(ApiHelper):
    '''
              --------------------
              | VM: 2.2.0.1      |
              | Host: 10.10.10.1 |
              --------------------
                         |
      ----------------------------------------------
      |                | p0 |                      |
      |                ------                      |
      |                   |                        |
      |          -----------------------           |
      |          | Tunnel: 10.10.10.10 |           |
      |          -----------------------           |
      |    /        |        |        |      \     |
      |  ------  -------  -------  ------  ------  |
      |  | L3 |  | SVI |  | SVI |  | SB |  | SB |  |
      |  | p1 |  | p2  |  | p3  |  | p4 |  | p5 |  |
      ----------------------------------------------
            |       |         |       |      |
    --------------- | --------------- | -------------------
    | C1: 2.2.1.1 | | | C3: 2.2.3.1 | | | C5: 2.2.5.1 500 |
    --------------- | --------------- | | C6: 2.2.6.1 600 |
                    |                 | -------------------
                    |                 |
          ---------------       -------------------
          | C2: 2.2.2.1 |       | C4: 2.2.4.1 400 |
          ---------------       -------------------
    L3 : L3 interface(c1)
    SVI: SVI on vlan 100, tagged(c2) and untagged(c3)
    SB : L3 subport on vlan 200(c4), vlan 300(c5)
    VRF1 <-> 1000
    VRF2 <-> 2000
    VRF3 <-> 3000
    VRF4 <-> 4000
    VRF5 <-> 5000
    '''
    def setUp(self):
        self.configure()
        print()
        if (self.client.is_feature_enable(SWITCH_FEATURE_IPV4_TUNNEL) == 0):
            print("IPv4 tunnel feature not enabled, skipping")
            return
        if (self.client.is_feature_enable(SWITCH_FEATURE_VXLAN) == 0):
            print("VXLAN feature not enabled, skipping")
            return

        # underlay config input
        self.uvrf = self.add_vrf(self.device, id=100, src_mac=self.rmac)
        # create underlay loopback rif for tunnel
        self.urif_lb = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_LOOPBACK, vrf_handle=self.uvrf, src_mac=self.rmac)
        self.vm_ip4 = '2.2.0.1'
        self.c1_ip4 = '2.2.1.1'
        self.c1_mac = '00:22:11:11:11:01'
        self.c2_ip4 = '2.2.2.1'
        self.c2_mac = '00:22:11:11:11:02'
        self.c3_ip4 = '2.2.3.1'
        self.c3_mac = '00:22:11:11:11:03'
        self.c4_ip4 = '2.2.4.1'
        self.c4_mac = '00:22:11:11:11:04'
        self.c5_ip4 = '2.2.5.1'
        self.c5_mac = '00:22:11:11:11:05'
        self.c6_ip4 = '2.2.6.1'
        self.c6_mac = '00:22:11:11:11:06'
        self.c7_ip4 = '2.2.7.1'
        self.c7_mac = '00:22:11:11:11:07'
        self.my_lb_ip = '10.10.10.10'
        self.tunnel_ip = '10.10.10.1'
        self.inner_dmac = "00:33:33:33:33:33"
        self.underlay_neighbor_mac = '00:11:11:11:11:11'

        # overlay configuration
        self.ovrf1 = self.add_vrf(self.device, id = 100, src_mac=self.rmac)
        self.vni1 = 1000
        self.ovrf2 = self.add_vrf(self.device, id = 200, src_mac=self.rmac)
        self.vni2 = 2000
        self.ovrf4 = self.add_vrf(self.device, id = 400, src_mac=self.rmac)
        self.vni4 = 4000
        self.ovrf5 = self.add_vrf(self.device, id = 500, src_mac=self.rmac)
        self.vni5 = 5000
        self.ovrf7 = self.add_vrf(self.device, id = 700, src_mac=self.rmac)
        self.vni7 = 7000
        self.tunnel_type = SWITCH_TUNNEL_ATTR_TYPE_VXLAN
        self.term_type = SWITCH_TUNNEL_TERM_ATTR_TERMINATION_TYPE_P2MP
        self.encap_ttl_mode = SWITCH_TUNNEL_ATTR_ENCAP_TTL_MODE_PIPE_MODEL
        self.decap_ttl_mode = SWITCH_TUNNEL_ATTR_DECAP_TTL_MODE_PIPE_MODEL

        self.orif1 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port1, vrf_handle=self.ovrf1, src_mac=self.rmac)
        self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port2, tagging_mode=SWITCH_VLAN_MEMBER_ATTR_TAGGING_MODE_TAGGED)
        self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port3)
        self.attribute_set(self.port3, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.orif2 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_VLAN, vlan_handle=self.vlan10, vrf_handle=self.ovrf2, src_mac=self.rmac)
        self.orif4 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_SUB_PORT, port_handle=self.port4, outer_vlan_id=400, vrf_handle=self.ovrf4, src_mac=self.rmac)
        self.orif5 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_SUB_PORT, port_handle=self.port5, outer_vlan_id=500, vrf_handle=self.ovrf5, src_mac=self.rmac)
        self.orif6 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_SUB_PORT, port_handle=self.port5, outer_vlan_id=600, vrf_handle=self.ovrf5, src_mac=self.rmac)
        self.orif7 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port7, vrf_handle=self.ovrf7, src_mac=self.rmac)

        # Create route to tunnel ip 10.10.10.1
        self.urif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port0, vrf_handle=self.uvrf, src_mac=self.rmac)
        self.unhop = self.add_nexthop(self.device, handle=self.urif, dest_ip=self.tunnel_ip)
        self.add_neighbor(self.device, mac_address=self.underlay_neighbor_mac, handle=self.urif, dest_ip=self.tunnel_ip)  # 10.10.10.1
        ecmp = self.add_ecmp(self.device)
        self.add_ecmp_member(self.device, nexthop_handle=self.unhop, ecmp_handle=ecmp)
        self.tunnel_route = self.add_route(self.device, ip_prefix="10.10.10.1", vrf_handle=self.uvrf, nexthop_handle=ecmp)

        # Encap configuration follows
        # create Encap/Decap mappers
        self.encap_tunnel_map = self.add_tunnel_mapper(self.device,
            type=SWITCH_TUNNEL_MAPPER_ATTR_TYPE_VRF_HANDLE_TO_VNI)
        self.decap_tunnel_map = self.add_tunnel_mapper(self.device,
            type=SWITCH_TUNNEL_MAPPER_ATTR_TYPE_VNI_TO_VRF_HANDLE)

        # create Encap/Decap mapper entries
        for vrf, vni in zip([self.ovrf1, self.ovrf2, self.ovrf4, self.ovrf5], [self.vni1, self.vni2, self.vni4, self.vni5]):
            self.encap_tunnel_map_entry = self.add_tunnel_mapper_entry(self.device,
                type=SWITCH_TUNNEL_MAPPER_ENTRY_ATTR_TYPE_VRF_HANDLE_TO_VNI,
                tunnel_mapper_handle=self.encap_tunnel_map,
                network_handle=vrf,
                tunnel_vni=vni)
            self.decap_tunnel_map_entry = self.add_tunnel_mapper_entry(self.device,
                type=SWITCH_TUNNEL_MAPPER_ENTRY_ATTR_TYPE_VNI_TO_VRF_HANDLE,
                tunnel_mapper_handle=self.decap_tunnel_map,
                network_handle=vrf,
                tunnel_vni=vni)

        ingress_mapper_list = [switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=self.encap_tunnel_map)]
        egress_mapper_list = [switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=self.decap_tunnel_map)]

        # create Tunnel
        self.tunnel = self.add_tunnel(self.device,
            type=self.tunnel_type,
            src_ip=self.my_lb_ip,
            ingress_mapper_handles=ingress_mapper_list,
            egress_mapper_handles=egress_mapper_list,
            encap_ttl_mode=self.encap_ttl_mode,
            decap_ttl_mode=self.decap_ttl_mode,
            underlay_rif_handle=self.urif_lb)

        # Decap configuration follows
        # Create tunnel_termination entry
        self.tunnel_term = self.add_tunnel_term(self.device,
            type=self.tunnel_type,
            vrf_handle=self.uvrf,
            tunnel_handle=self.tunnel,
            termination_type=self.term_type,
            dst_ip=self.my_lb_ip)

        # Create route to customer from VM
        self.onhop1 = self.add_nexthop(self.device, handle=self.orif1, dest_ip=self.c1_ip4)
        self.add_neighbor(self.device, mac_address=self.c1_mac, handle=self.orif1, dest_ip=self.c1_ip4)
        self.add_route(self.device, ip_prefix=self.c1_ip4, vrf_handle=self.ovrf1, nexthop_handle=self.onhop1)

        self.onhop2 = self.add_nexthop(self.device, handle=self.orif2, dest_ip=self.c2_ip4)
        self.add_neighbor(self.device, mac_address=self.c2_mac, handle=self.orif2, dest_ip=self.c2_ip4)
        self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address=self.c2_mac, destination_handle=self.port2)
        self.add_route(self.device, ip_prefix=self.c2_ip4, vrf_handle=self.ovrf2, nexthop_handle=self.onhop2)

        self.onhop3 = self.add_nexthop(self.device, handle=self.orif2, dest_ip=self.c3_ip4)
        self.add_neighbor(self.device, mac_address=self.c3_mac, handle=self.orif2, dest_ip=self.c3_ip4)
        self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address=self.c3_mac, destination_handle=self.port3)
        self.add_route(self.device, ip_prefix=self.c3_ip4, vrf_handle=self.ovrf2, nexthop_handle=self.onhop3)

        self.onhop4 = self.add_nexthop(self.device, handle=self.orif4, dest_ip=self.c4_ip4)
        self.add_neighbor(self.device, mac_address=self.c4_mac, handle=self.orif4, dest_ip=self.c4_ip4)
        self.add_route(self.device, ip_prefix=self.c4_ip4, vrf_handle=self.ovrf4, nexthop_handle=self.onhop4)

        self.onhop5 = self.add_nexthop(self.device, handle=self.orif5, dest_ip=self.c5_ip4)
        self.add_neighbor(self.device, mac_address=self.c5_mac, handle=self.orif5, dest_ip=self.c5_ip4)
        self.add_route(self.device, ip_prefix=self.c5_ip4, vrf_handle=self.ovrf5, nexthop_handle=self.onhop5)

        self.onhop6 = self.add_nexthop(self.device, handle=self.orif6, dest_ip=self.c6_ip4)
        self.add_neighbor(self.device, mac_address=self.c6_mac, handle=self.orif6, dest_ip=self.c6_ip4)
        self.add_route(self.device, ip_prefix=self.c6_ip4, vrf_handle=self.ovrf5, nexthop_handle=self.onhop6)

        self.onhop7 = self.add_nexthop(self.device, handle=self.orif7, dest_ip=self.c7_ip4)
        self.add_neighbor(self.device, mac_address=self.c7_mac, handle=self.orif7, dest_ip=self.c7_ip4)
        self.add_route(self.device, ip_prefix=self.c7_ip4, vrf_handle=self.ovrf7, nexthop_handle=self.onhop7)

        # create tunnel nexthop for VM 2.2.0.1
        self.tunnel_nexthop = self.add_nexthop(self.device,
            type=SWITCH_NEXTHOP_ATTR_TYPE_TUNNEL,
            rw_type=SWITCH_NEXTHOP_ATTR_RW_TYPE_L3,
            handle=self.tunnel,
            dest_ip=self.tunnel_ip,
            mac_address=self.inner_dmac)
        # add route to tunnel_nexthop for all VRFs
        for vrf in [self.ovrf1, self.ovrf2, self.ovrf4, self.ovrf5, self.ovrf7]:
            self.customer_route = self.add_route(self.device,
                ip_prefix=self.vm_ip4,
                vrf_handle=vrf,
                nexthop_handle=self.tunnel_nexthop)

    def runTest(self):
        if (self.client.is_feature_enable(SWITCH_FEATURE_IPV4_TUNNEL) == 0):
            return
        if (self.client.is_feature_enable(SWITCH_FEATURE_VXLAN) == 0):
            return
        try:
            self.EncapTest()
            self.DecapTest()
            self.useTunnelNexthopsWithVni()
            self.EncapTest()
            #self.EncapDropTest()
            #self.DecapDropTest()
        finally:
            pass

    def useTunnelNexthopsWithVni(self):
        print("Replacing single tunnel nexthop with tunnel nexthop per vni")
        # Remove tunnel nexthop and associated routes
        for vrf in [self.ovrf1, self.ovrf2, self.ovrf4, self.ovrf5, self.ovrf7]:
            self.cleanlast()
        self.cleanlast()
        # Recreate tunnel nexthops with vni along with associated routes
        for vrf, vni in zip([self.ovrf1, self.ovrf2, self.ovrf4, self.ovrf5], [self.vni1, self.vni2, self.vni4, self.vni5]):
            # create tunnel nexthop for VM 2.2.0.1
            self.tunnel_nexthop = self.add_nexthop(self.device,
                type=SWITCH_NEXTHOP_ATTR_TYPE_TUNNEL,
                rw_type=SWITCH_NEXTHOP_ATTR_RW_TYPE_L3_VNI,
                handle=self.tunnel,
                dest_ip=self.tunnel_ip,
                mac_address=self.inner_dmac,
                tunnel_vni=vni)
            # add route to tunnel_nexthop for all VRFs
            self.customer_route = self.add_route(self.device,
                ip_prefix=self.vm_ip4,
                vrf_handle=vrf,
                nexthop_handle=self.tunnel_nexthop)

    def EncapTest(self):
        print("EncapTest()")
        if not self.client.is_feature_enable(SWITCH_FEATURE_TUNNEL_ENCAP):
            print("Tunnel encap not enabled, skipping")
            return
        pkt_data = [
            [self.devports[1], self.c1_ip4, False, 0, self.vni1],
            [self.devports[2], self.c2_ip4, True, 10, self.vni2],
            [self.devports[3], self.c3_ip4, False, 0, self.vni2],
            [self.devports[4], self.c4_ip4, True, 400, self.vni4],
            [self.devports[5], self.c5_ip4, True, 500, self.vni5],
            [self.devports[5], self.c6_ip4, True, 600, self.vni5],
        ]
        try:
            for data in pkt_data:
                plen = 104
                dec = 0
                print("Sending packet from port %d -> VNI %d " % (data[0], data[4]))
                if data[2]:
                    dec = 4
                pkt = simple_tcp_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src='00:22:22:22:22:22',
                    ip_dst=self.vm_ip4,
                    ip_src=data[1],
                    dl_vlan_enable=data[2],
                    vlan_vid=data[3],
                    ip_id=108,
                    ip_ttl=64,
                    pktlen=plen)
                inner_pkt = simple_tcp_packet(
                    eth_dst=self.inner_dmac,  # 00:33:33:33:33:33
                    eth_src=self.default_rmac,
                    ip_dst=self.vm_ip4,
                    ip_src=data[1],
                    ip_id=108,
                    ip_ttl=63,
                    pktlen=plen-dec)
                vxlan_pkt = mask.Mask(simple_vxlan_packet(
                    eth_src='00:77:66:55:44:33',
                    eth_dst=self.underlay_neighbor_mac,  # 00:11:11:11:11:11
                    ip_id=0,
                    ip_src=self.my_lb_ip,
                    ip_dst=self.tunnel_ip,
                    ip_ttl=64,
                    ip_flags=0x2,
                    with_udp_chksum=False,
                    vxlan_vni=data[4],
                    inner_frame=inner_pkt))
                mask_set_do_not_care_packet(vxlan_pkt, UDP, "sport")
                send_packet(self, data[0], pkt)
                verify_packet(self, vxlan_pkt, self.devports[0])
        finally:
            pass

    def EncapDropTest(self):
        print("EncapTest()")
        if not self.client.is_feature_enable(SWITCH_FEATURE_TUNNEL_ENCAP):
            print("Tunnel encap not enabled, skipping")
            return
        try:
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst=self.vm_ip4,
                ip_src=self.c7_ip4,
                ip_id=108,
                ip_ttl=64)
            inner_pkt = simple_tcp_packet(
                eth_dst=self.inner_dmac,  # 00:33:33:33:33:33
                eth_src='00:77:66:55:44:33',
                ip_dst=self.vm_ip4,
                ip_src=self.c7_ip4,
                ip_id=108,
                ip_ttl=63)
            vxlan_pkt = mask.Mask(simple_vxlan_packet(
                eth_src='00:77:66:55:44:33',
                eth_dst=self.underlay_neighbor_mac,  # 00:11:11:11:11:11
                ip_id=0,
                ip_src=self.my_lb_ip,
                ip_dst=self.tunnel_ip,
                ip_ttl=64,
                ip_flags=0x2,
                with_udp_chksum=False,
                vxlan_vni=self.vni7,
                inner_frame=inner_pkt))
            mask_set_do_not_care_packet(vxlan_pkt, UDP, "sport")
            send_packet(self, self.devports[7], pkt)
            verify_no_other_packets(self, timeout=1)
            #verify_packet(self, vxlan_pkt, self.devports[0])
        finally:
            pass

    def DecapTest(self):
        print("DecapTest()")
        pkt_data = [
            [self.devports[1], self.c1_ip4, False, 0, self.vni1, self.c1_mac],
            [self.devports[2], self.c2_ip4, True, 10, self.vni2, self.c2_mac],
            [self.devports[3], self.c3_ip4, False, 0, self.vni2, self.c3_mac],
            [self.devports[4], self.c4_ip4, True, 400, self.vni4, self.c4_mac],
            [self.devports[5], self.c5_ip4, True, 500, self.vni5, self.c5_mac],
            [self.devports[5], self.c6_ip4, True, 600, self.vni5, self.c6_mac],
        ]
        try:
            for data in pkt_data:
                plen = 100
                inc = 0
                if data[2]:
                    inc = 4
                print("Sending packet from vni %d -> port %d" % (data[4], data[0]))
                pkt = simple_tcp_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src=self.inner_dmac,
                    ip_dst=data[1],
                    ip_src=self.vm_ip4,
                    ip_id=108,
                    ip_ttl=64,
                    pktlen=plen)
                pkt2 = simple_tcp_packet(
                    eth_dst=data[5],
                    eth_src='00:77:66:55:44:33',
                    ip_dst=data[1],
                    ip_src=self.vm_ip4,
                    dl_vlan_enable=data[2],
                    vlan_vid=data[3],
                    ip_id=108,
                    ip_ttl=63,
                    pktlen=plen+inc)
                vxlan_pkt = simple_vxlan_packet(
                    eth_src=self.underlay_neighbor_mac,
                    eth_dst='00:77:66:55:44:33',
                    ip_id=0,
                    ip_dst=self.my_lb_ip,
                    ip_src=self.tunnel_ip,
                    ip_ttl=64,
                    ip_flags=0x2,
                    udp_sport=11638,
                    with_udp_chksum=False,
                    vxlan_vni=data[4],
                    inner_frame=pkt)
                send_packet(self, self.devports[0], vxlan_pkt)
                verify_packet(self, pkt2, data[0])
        finally:
            pass

    def DecapDropTest(self):
        print("DecapTest()")
        try:
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src=self.inner_dmac,
                ip_dst=self.c7_ip4,
                ip_src=self.vm_ip4,
                ip_id=108,
                ip_ttl=64)
            pkt2 = simple_tcp_packet(
                eth_dst=self.c7_mac,
                eth_src='00:77:66:55:44:33',
                ip_dst=self.c7_ip4,
                ip_src=self.vm_ip4,
                ip_id=108,
                ip_ttl=63)
            vxlan_pkt = simple_vxlan_packet(
                eth_src=self.underlay_neighbor_mac,
                eth_dst='00:77:66:55:44:33',
                ip_id=0,
                ip_dst=self.my_lb_ip,
                ip_src=self.tunnel_ip,
                ip_ttl=64,
                ip_flags=0x2,
                udp_sport=11638,
                with_udp_chksum=False,
                vxlan_vni=self.vni7,
                inner_frame=pkt)
            send_packet(self, self.devports[0], vxlan_pkt)
            verify_no_other_packets(self, timeout=1)
            #verify_packet(self, pkt2, self.devports[7])
        finally:
            pass

    def tearDown(self):
        self.attribute_set(self.port3, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
        self.cleanup()


###############################################################################


@group('tunnel')
class L3TunnelDmacUpdateTest(ApiHelper):
    '''
               --------------------
               | VM: 2.2.0.1      |
               | Host: 10.10.10.1 |
               --------------------
                         |
      -------------------------------------------
      |                | p0 |                   |
      |                ------                   |
      |                   |                     |
      |         -----------------------         |
      |         | Tunnel: 10.10.10.10 |         |
      |         -----------------------         |
      |                   |                     |
      |                -------                  |
      |                | L3 |                   |
      |                | p1 |                   |
      -------------------------------------------
                          |
                    ---------------
                    | C1: 2.2.1.1 |
                    ---------------

    L3 : L3 interface(c1)
    VRF1 <-> 2000
    '''
    def setUp(self):
        self.configure()
        print()
        if (self.client.is_feature_enable(SWITCH_FEATURE_IPV4_TUNNEL) == 0):
            print("IPv4 tunnel feature not enabled, skipping")
            return
        if (self.client.is_feature_enable(SWITCH_FEATURE_VXLAN) == 0):
            print("VXLAN feature not enabled, skipping")
            return

        # underlay config input
        self.uvrf = self.add_vrf(self.device, id=100, src_mac=self.rmac)
        # create underlay loopback rif for tunnel
        self.urif_lb = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_LOOPBACK, vrf_handle=self.uvrf, src_mac=self.rmac)
        self.vm_ip4 = '2.2.0.1'
        self.c1_ip4 = '2.2.1.1'
        self.c1_mac = '00:22:11:11:11:01'
        self.my_lb_ip = '10.10.10.10'
        self.tunnel_ip = '10.10.10.1'
        self.inner_dmac1 = "00:44:44:44:44:44"
        self.inner_dmac2 = "00:55:55:55:55:55"
        self.underlay_neighbor_mac = '00:11:11:11:11:11'
        self.attribute_set(self.device, SWITCH_DEVICE_ATTR_TUNNEL_DMAC, self.inner_dmac1)

        # overlay configuration
        self.ovrf1 = self.add_vrf(self.device, id = 200, src_mac=self.rmac)
        self.vni1 = 2000
        self.tunnel_type = SWITCH_TUNNEL_ATTR_TYPE_VXLAN
        self.term_type = SWITCH_TUNNEL_TERM_ATTR_TERMINATION_TYPE_P2MP
        self.encap_ttl_mode = SWITCH_TUNNEL_ATTR_ENCAP_TTL_MODE_PIPE_MODEL
        self.decap_ttl_mode = SWITCH_TUNNEL_ATTR_DECAP_TTL_MODE_PIPE_MODEL

        self.orif1 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port1, vrf_handle=self.ovrf1, src_mac=self.rmac)

        # Create route to tunnel ip 10.10.10.1
        self.urif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port0, vrf_handle=self.uvrf, src_mac=self.rmac)
        self.unhop = self.add_nexthop(self.device, handle=self.urif, dest_ip=self.tunnel_ip)
        self.add_neighbor(self.device, mac_address=self.underlay_neighbor_mac, handle=self.urif, dest_ip=self.tunnel_ip)  # 10.10.10.1
        self.tunnel_route = self.add_route(self.device, ip_prefix="10.10.10.1", vrf_handle=self.uvrf, nexthop_handle=self.unhop)

        # Encap configuration follows
        # create Encap/Decap mappers
        self.encap_tunnel_map = self.add_tunnel_mapper(self.device,
            type=SWITCH_TUNNEL_MAPPER_ATTR_TYPE_VRF_HANDLE_TO_VNI)
        self.decap_tunnel_map = self.add_tunnel_mapper(self.device,
            type=SWITCH_TUNNEL_MAPPER_ATTR_TYPE_VNI_TO_VRF_HANDLE)

        # create Encap/Decap mapper entries
        self.encap_tunnel_map_entry = self.add_tunnel_mapper_entry(self.device,
            type=SWITCH_TUNNEL_MAPPER_ENTRY_ATTR_TYPE_VRF_HANDLE_TO_VNI,
            tunnel_mapper_handle=self.encap_tunnel_map,
            network_handle=self.ovrf1,
            tunnel_vni=self.vni1)
        self.decap_tunnel_map_entry = self.add_tunnel_mapper_entry(self.device,
            type=SWITCH_TUNNEL_MAPPER_ENTRY_ATTR_TYPE_VNI_TO_VRF_HANDLE,
            tunnel_mapper_handle=self.decap_tunnel_map,
            network_handle=self.ovrf1,
            tunnel_vni=self.vni1)

        ingress_mapper_list = [switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=self.encap_tunnel_map)]
        egress_mapper_list = [switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=self.decap_tunnel_map)]

        # create Tunnel
        self.tunnel = self.add_tunnel(self.device,
            type=self.tunnel_type,
            src_ip=self.my_lb_ip,
            ingress_mapper_handles=ingress_mapper_list,
            egress_mapper_handles=egress_mapper_list,
            encap_ttl_mode=self.encap_ttl_mode,
            decap_ttl_mode=self.decap_ttl_mode,
            underlay_rif_handle=self.urif_lb)

        # Decap configuration follows
        # Create tunnel_termination entry
        self.tunnel_term = self.add_tunnel_term(self.device,
            type=self.tunnel_type,
            vrf_handle=self.uvrf,
            tunnel_handle=self.tunnel,
            termination_type=self.term_type,
            dst_ip=self.my_lb_ip)

        # create tunnel nexthop for VM 2.2.0.1
        self.tunnel_nexthop = self.add_nexthop(self.device,
            type=SWITCH_NEXTHOP_ATTR_TYPE_TUNNEL,
            rw_type=SWITCH_NEXTHOP_ATTR_RW_TYPE_L3,
            handle=self.tunnel,
            dest_ip=self.tunnel_ip)

        # add route to tunnel_nexthop
        self.customer_route = self.add_route(self.device,
            ip_prefix=self.vm_ip4,
            vrf_handle=self.ovrf1,
            nexthop_handle=self.tunnel_nexthop)

        # Create route to customer from VM
        self.onhop1 = self.add_nexthop(self.device, handle=self.orif1, dest_ip=self.c1_ip4)
        self.add_neighbor(self.device, mac_address=self.c1_mac, handle=self.orif1, dest_ip=self.c1_ip4)
        self.add_route(self.device, ip_prefix=self.c1_ip4, vrf_handle=self.ovrf1, nexthop_handle=self.onhop1)

    def runTest(self):
        if (self.client.is_feature_enable(SWITCH_FEATURE_IPV4_TUNNEL) == 0):
            return
        if (self.client.is_feature_enable(SWITCH_FEATURE_VXLAN) == 0):
            return
        try:
            self.EncapTest(inner_dmac=self.inner_dmac1) # 00:44:44:44:44:44
            self.EncapTest(inner_dmac=self.inner_dmac2, update_dmac=True) # 00:55:55:55:55:55
            self.DecapTest()
            # Tests for device src_mac == device tunnel_dmac
            self.EncapTest(inner_dmac=self.inner_dmac2, update_src_mac=True)
            self.attribute_set(
                self.device, SWITCH_DEVICE_ATTR_SRC_MAC, self.default_rmac)
            self.EncapTest(inner_dmac=self.inner_dmac2)
            self.EncapTest(inner_dmac=self.default_rmac, update_dmac=True)

        finally:
            self.attribute_set(self.device, SWITCH_DEVICE_ATTR_TUNNEL_DMAC, "00:00:00:00:00:00")
            self.attribute_set(
                self.device, SWITCH_DEVICE_ATTR_SRC_MAC, self.default_rmac)
            pass

    def EncapTest(self, inner_dmac, update_dmac=False, update_src_mac=False):
        print("EncapTest()")
        if not self.client.is_feature_enable(SWITCH_FEATURE_TUNNEL_ENCAP):
            print("Tunnel encap not enabled, skipping")
            return

        try:
            if update_dmac:
                ret = self.attribute_set(
                    self.device, SWITCH_DEVICE_ATTR_TUNNEL_DMAC, inner_dmac)
                self.assertTrue((ret.status == 0),
                                "Error during attribute_set of tunnel_dmac")
            if update_src_mac:
                ret = self.attribute_set(
                    self.device, SWITCH_DEVICE_ATTR_SRC_MAC, inner_dmac)
                self.assertTrue((ret.status == 0),
                                "Error during attribute_set of device src_mac")

            plen = 104
            print("Sending packet from port %d -> VNI %d " % (self.devports[1], self.vni1))
            pkt = simple_tcp_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src='00:22:22:22:22:22',
                ip_dst=self.vm_ip4,
                ip_src=self.c1_ip4,
                ip_id=108,
                ip_ttl=64,
                pktlen=plen)
            inner_pkt = simple_tcp_packet(
                eth_dst=inner_dmac,
                eth_src=self.default_rmac,
                ip_dst=self.vm_ip4,
                ip_src=self.c1_ip4,
                ip_id=108,
                ip_ttl=63,
                pktlen=plen)
            vxlan_pkt = mask.Mask(simple_vxlan_packet(
                eth_src='00:77:66:55:44:33',
                eth_dst=self.underlay_neighbor_mac,  # 00:11:11:11:11:11
                ip_id=0,
                ip_src=self.my_lb_ip,
                ip_dst=self.tunnel_ip,
                ip_ttl=64,
                ip_flags=0x2,
                with_udp_chksum=False,
                vxlan_vni=self.vni1,
                inner_frame=inner_pkt))
            mask_set_do_not_care_packet(vxlan_pkt, UDP, "sport")
            send_packet(self, self.devports[1], pkt)
            verify_packet(self, vxlan_pkt, self.devports[0])
        finally:
            pass

    def DecapTest(self):
        print("DecapTest()")
        try:
            plen = 100
            pkt = simple_tcp_packet(
                eth_dst=self.default_rmac,
                eth_src=self.inner_dmac1,
                ip_dst=self.c1_ip4,
                ip_src=self.vm_ip4,
                ip_id=108,
                ip_ttl=64,
                pktlen=plen)
            pkt1 = simple_tcp_packet(
                eth_dst=self.rmac,
                eth_src=self.inner_dmac1,
                ip_dst=self.c1_ip4,
                ip_src=self.vm_ip4,
                ip_id=108,
                ip_ttl=64,
                pktlen=plen)
            pkt2 = simple_tcp_packet(
                eth_dst=self.inner_dmac2,
                eth_src=self.inner_dmac1,
                ip_dst=self.c1_ip4,
                ip_src=self.vm_ip4,
                ip_id=108,
                ip_ttl=64,
                pktlen=plen)
            exp_pkt = simple_tcp_packet(
                eth_dst=self.c1_mac,
                eth_src='00:77:66:55:44:33',
                ip_dst=self.c1_ip4,
                ip_src=self.vm_ip4,
                ip_id=108,
                ip_ttl=63,
                pktlen=plen)
            vxlan_pkt = simple_vxlan_packet(
                eth_src=self.underlay_neighbor_mac,
                eth_dst='00:77:66:55:44:33',
                ip_id=0,
                ip_dst=self.my_lb_ip,
                ip_src=self.tunnel_ip,
                ip_ttl=64,
                ip_flags=0x2,
                udp_sport=11638,
                with_udp_chksum=False,
                vxlan_vni=self.vni1,
                inner_frame=pkt)
            vxlan_pkt1 = simple_vxlan_packet(
                eth_src=self.underlay_neighbor_mac,
                eth_dst='00:77:66:55:44:33',
                ip_id=0,
                ip_dst=self.my_lb_ip,
                ip_src=self.tunnel_ip,
                ip_ttl=64,
                ip_flags=0x2,
                udp_sport=11638,
                with_udp_chksum=False,
                vxlan_vni=self.vni1,
                inner_frame=pkt1)
            vxlan_pkt2 = simple_vxlan_packet(
                eth_src=self.underlay_neighbor_mac,
                eth_dst='00:77:66:55:44:33',
                ip_id=0,
                ip_dst=self.my_lb_ip,
                ip_src=self.tunnel_ip,
                ip_ttl=64,
                ip_flags=0x2,
                udp_sport=11638,
                with_udp_chksum=False,
                vxlan_vni=self.vni1,
                inner_frame=pkt2)
            print("Sending packet inner_dmac device rmac from vni %d -> port %d" % (self.vni1, self.devports[1]))
            send_packet(self, self.devports[0], vxlan_pkt)
            verify_packet(self, exp_pkt, self.devports[1])
            print("Sending packet inner_dmac vrf rmac from vni %d -> port %d" % (self.vni1, self.devports[1]))
            send_packet(self, self.devports[0], vxlan_pkt1)
            verify_packet(self, exp_pkt, self.devports[1])
            print("Sending packet inner_dmac tunnel dmac from vni %d -> port %d" % (self.vni1, self.devports[1]))
            send_packet(self, self.devports[0], vxlan_pkt2)
            verify_packet(self, exp_pkt, self.devports[1])
        finally:
            pass

    def tearDown(self):
        self.cleanup()

###############################################################################

@group('tunnel')
class L3TunnelIPinIPv4Test(ApiHelper):
    def setUp(self):
        self.configure()
        if (self.client.is_feature_enable(SWITCH_FEATURE_IPV4_TUNNEL) == 0):
            print("IPv4 tunnel feature not enabled, skipping")
            return
        print()

        # underlay config input
        self.uvrf = self.add_vrf(self.device, id=100, src_mac=self.rmac)
        self.vm_ip4 = '100.100.1.1'
        self.vm_ip6 = '1234:5678:9abc:def0:4422:1133:5577:9951'
        self.customer_ip4 = '100.100.0.1'
        self.customer_ip6 = '1234:5678:9abc:def0:4422:1133:5577:0011'
        self.my_lb_ip = '10.10.10.10'
        self.tunnel_ip = '10.10.10.1'
        self.inner_dmac = "00:33:33:33:33:33"
        self.underlay_neighbor_mac = '00:11:11:11:11:11'

        # overlay configuration
        self.ovrf = self.add_vrf(self.device, id = 300, src_mac=self.rmac)
        self.tunnel_type = SWITCH_TUNNEL_ATTR_TYPE_IPIP
        self.term_type = SWITCH_TUNNEL_TERM_ATTR_TERMINATION_TYPE_P2P
        self.encap_ttl_mode = SWITCH_TUNNEL_ATTR_ENCAP_TTL_MODE_PIPE_MODEL
        self.decap_ttl_mode = SWITCH_TUNNEL_ATTR_DECAP_TTL_MODE_PIPE_MODEL

        self.urif_lb = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_LOOPBACK, vrf_handle=self.uvrf, src_mac=self.rmac)

        # create underlay loopback rif for tunnel
        self.orif_lb = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_LOOPBACK, vrf_handle=self.ovrf, src_mac=self.rmac)

        # Create route to tunnel ip 10.10.10.1
        self.urif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port1, vrf_handle=self.uvrf, src_mac=self.rmac)
        self.unhop = self.add_nexthop(self.device, handle=self.urif, dest_ip=self.tunnel_ip)
        self.uneighbor = self.add_neighbor(self.device, mac_address=self.underlay_neighbor_mac,
            handle=self.urif, dest_ip=self.tunnel_ip)  # 10.10.10.1
        self.tunnel_route1 = self.add_route(self.device, ip_prefix=self.tunnel_ip,
            vrf_handle=self.uvrf, nexthop_handle=self.unhop)

        # create Tunnel
        self.tunnel = self.add_tunnel(self.device, type=self.tunnel_type,
            src_ip=self.my_lb_ip,
            encap_ttl_mode=self.encap_ttl_mode, decap_ttl_mode=self.decap_ttl_mode,
            underlay_rif_handle=self.urif_lb, overlay_rif_handle=self.orif_lb)

        # Create tunnel_termination entry
        self.tunnel_term = self.add_tunnel_term(self.device, type=self.tunnel_type,
            vrf_handle=self.uvrf, tunnel_handle=self.tunnel,
            termination_type=self.term_type, src_ip=self.tunnel_ip, dst_ip=self.my_lb_ip)

        # create tunnel nexthop for VM 100.100.1.1
        self.tunnel_nexthop = self.add_nexthop(self.device,
            type=SWITCH_NEXTHOP_ATTR_TYPE_TUNNEL,
            rw_type=SWITCH_NEXTHOP_ATTR_RW_TYPE_L3,
            handle=self.tunnel,
            dest_ip=self.tunnel_ip,  # 10.10.10.1
            mac_address=self.inner_dmac)
        # add route to tunnel_nexthop
        self.customer_route = self.add_route(self.device,
            ip_prefix=self.vm_ip4, vrf_handle=self.ovrf,
            nexthop_handle=self.tunnel_nexthop)
        self.customer_route = self.add_route(self.device,
            ip_prefix=self.vm_ip6, vrf_handle=self.ovrf,
            nexthop_handle=self.tunnel_nexthop)

        # Create route to customer from VM
        self.orif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port0, vrf_handle=self.ovrf, src_mac=self.rmac)
        self.onhop4 = self.add_nexthop(self.device, handle=self.orif, dest_ip=self.customer_ip4)
        self.oneighbor4 = self.add_neighbor(self.device,
            mac_address='00:22:22:22:22:33',
            handle=self.orif,
            dest_ip=self.customer_ip4)  # 100.100.0.1
        self.customer_route = self.add_route(self.device, ip_prefix=self.customer_ip4,
            vrf_handle=self.ovrf, nexthop_handle=self.onhop4)
        self.onhop6 = self.add_nexthop(self.device, handle=self.orif, dest_ip=self.customer_ip6)
        self.oneighbor6 = self.add_neighbor(self.device,
            mac_address='00:22:22:22:22:33',
            handle=self.orif,
            dest_ip=self.customer_ip6)  # 100.100.0.1
        self.customer_route = self.add_route(self.device, ip_prefix=self.customer_ip6,
            vrf_handle=self.ovrf, nexthop_handle=self.onhop6)

    def runTest(self):
        if (self.client.is_feature_enable(SWITCH_FEATURE_IPV4_TUNNEL) == 0):
            return
        try:
            self.IPv4inIPv4Test()
            self.IPv6inIPv4Test()
        finally:
            pass

    def tearDown(self):
        self.cleanup()

    def IPv4inIPv4Test(self):
        print("IPv4inIPv4Test()")
        try:
            print("Verifying IP 4in4 (encap)")
            if not self.client.is_feature_enable(SWITCH_FEATURE_TUNNEL_ENCAP):
                print(" Tunnel encap not enabled, skipping")
            else:
                pkt = simple_tcp_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src='00:22:22:22:22:33',
                    ip_dst=self.vm_ip4,
                    ip_src=self.customer_ip4,
                    ip_id=108,
                    ip_ttl=64)
                pkt2 = simple_tcp_packet(
                    eth_dst=self.inner_dmac,  # 00:33:33:33:33:33
                    eth_src=self.default_rmac,
                    ip_dst=self.vm_ip4,
                    ip_src=self.customer_ip4,
                    ip_id=108,
                    ip_ttl=63)
                ipip_pkt = simple_ipv4ip_packet(
                    eth_dst=self.underlay_neighbor_mac,  # 00:11:11:11:11:11
                    eth_src='00:77:66:55:44:33',
                    ip_id=0,
                    ip_src='10.10.10.10',
                    ip_dst='10.10.10.1',
                    ip_ttl=64,
                    inner_frame=pkt2['IP'])
                send_packet(self, self.devports[0], pkt)
                verify_packet(self, ipip_pkt, self.devports[1])

            print("Verifying IP 4in4 (decap)")
            pkt1 = simple_tcp_packet(
                eth_src='00:77:66:55:44:33',
                eth_dst=self.inner_dmac,  # 00:33:33:33:33:33
                ip_dst=self.customer_ip4,
                ip_src=self.vm_ip4,
                ip_id=108,
                ip_ttl=64)
            ipip_pkt = simple_ipv4ip_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src=self.underlay_neighbor_mac,  # 00:11:11:11:11:11
                ip_id=0,
                ip_src='10.10.10.1',
                ip_dst='10.10.10.10',
                ip_ttl=64,
                inner_frame=pkt1['IP'])
            pkt2 = simple_tcp_packet(
                eth_src='00:77:66:55:44:33',
                eth_dst='00:22:22:22:22:33',
                ip_dst=self.customer_ip4,
                ip_src=self.vm_ip4,
                ip_id=108,
                ip_ttl=63)
            send_packet(self, self.devports[1], ipip_pkt)
            verify_packet(self, pkt2, self.devports[0])
        finally:
            pass

    def IPv6inIPv4Test(self):
        print("IPv6inIPv4Test()")
        try:
            print("Verifying IP 6in4 (encap)")
            if not self.client.is_feature_enable(SWITCH_FEATURE_TUNNEL_ENCAP):
                print(" Tunnel encap not enabled, skipping")
            else:
                pkt = simple_tcpv6_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src='00:22:22:22:22:33',
                    ipv6_dst=self.vm_ip6,
                    ipv6_src=self.customer_ip6,
                    ipv6_hlim=64)
                pkt2 = simple_tcpv6_packet(
                    eth_dst=self.inner_dmac,  # 00:33:33:33:33:33
                    eth_src=self.default_rmac,
                    ipv6_dst=self.vm_ip6,
                    ipv6_src=self.customer_ip6,
                    ipv6_hlim=63)
                ipip_pkt = simple_ipv4ip_packet(
                    eth_dst=self.underlay_neighbor_mac,  # 00:11:11:11:11:11
                    eth_src='00:77:66:55:44:33',
                    ip_id=0,
                    ip_src='10.10.10.10',
                    ip_dst='10.10.10.1',
                    ip_ttl=64,
                    inner_frame=pkt2['IPv6'])
                send_packet(self, self.devports[0], pkt)
                verify_packet(self, ipip_pkt, self.devports[1])

            print("Verifying IP 6in4 (decap)")
            pkt1 = simple_tcpv6_packet(
                eth_src='00:77:66:55:44:33',
                eth_dst=self.inner_dmac,  # 00:33:33:33:33:33
                ipv6_dst=self.customer_ip6,
                ipv6_src=self.vm_ip6,
                ipv6_hlim=64)
            ipip_pkt = simple_ipv4ip_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src=self.underlay_neighbor_mac,  # 00:11:11:11:11:11
                ip_id=0,
                ip_src='10.10.10.1',
                ip_dst='10.10.10.10',
                ip_ttl=64,
                inner_frame=pkt1['IPv6'])
            pkt2 = simple_tcpv6_packet(
                eth_src='00:77:66:55:44:33',
                eth_dst='00:22:22:22:22:33',
                ipv6_dst=self.customer_ip6,
                ipv6_src=self.vm_ip6,
                ipv6_hlim=63)
            send_packet(self, self.devports[1], ipip_pkt)
            verify_packet(self, pkt2, self.devports[0])
        finally:
            pass

###############################################################################

@group('tunnel')
class L3TunnelIPinIPv6Test(ApiHelper):
    def setUp(self):
        self.configure()
        if (self.client.is_feature_enable(SWITCH_FEATURE_IPV6_TUNNEL) == 0):
            print("IPv6 tunnel feature not enabled, skipping")
            return
        print()

        # underlay config input
        self.uvrf = self.add_vrf(self.device, id=100, src_mac=self.rmac)
        self.vm_ip4 = '100.100.1.1'
        self.vm_ip6 = '1234:5678:9abc:def0:4422:1133:5577:9951'
        self.customer_ip4 = '100.100.0.1'
        self.customer_ip6 = '1234:5678:9abc:def0:4422:1133:5577:0011'
        self.my_lb_ip = '4444:5678:9abc:def0:4422:1133:5577:44'
        self.tunnel_ip = '4444:5678:9abc:def0:4422:1133:5577:1'
        self.inner_dmac = "00:33:33:33:33:33"
        self.underlay_neighbor_mac = '00:11:11:11:11:11'

        # overlay configuration
        self.ovrf = self.add_vrf(self.device, id = 300, src_mac=self.rmac)
        self.tunnel_type = SWITCH_TUNNEL_ATTR_TYPE_IPIP
        self.term_type = SWITCH_TUNNEL_TERM_ATTR_TERMINATION_TYPE_P2P
        self.encap_ttl_mode = SWITCH_TUNNEL_ATTR_ENCAP_TTL_MODE_PIPE_MODEL
        self.decap_ttl_mode = SWITCH_TUNNEL_ATTR_DECAP_TTL_MODE_PIPE_MODEL

        self.urif_lb = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_LOOPBACK, vrf_handle=self.uvrf, src_mac=self.rmac)

        # create underlay loopback rif for tunnel
        self.orif_lb = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_LOOPBACK, vrf_handle=self.ovrf, src_mac=self.rmac)

        # Create route to tunnel ip 10.10.10.1
        self.urif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port1, vrf_handle=self.uvrf, src_mac=self.rmac)
        self.unhop = self.add_nexthop(self.device, handle=self.urif, dest_ip=self.tunnel_ip)
        self.uneighbor = self.add_neighbor(self.device, mac_address=self.underlay_neighbor_mac,
            handle=self.urif, dest_ip=self.tunnel_ip)
        self.tunnel_route1 = self.add_route(self.device, ip_prefix=self.tunnel_ip,
            vrf_handle=self.uvrf, nexthop_handle=self.unhop)

        # create Tunnel
        self.tunnel = self.add_tunnel(self.device, type=self.tunnel_type,
            src_ip=self.my_lb_ip,
            encap_ttl_mode=self.encap_ttl_mode, decap_ttl_mode=self.decap_ttl_mode,
            underlay_rif_handle=self.urif_lb, overlay_rif_handle=self.orif_lb)

        # Create tunnel_termination entry
        self.tunnel_term = self.add_tunnel_term(self.device, type=self.tunnel_type,
            vrf_handle=self.uvrf, tunnel_handle=self.tunnel,
            termination_type=self.term_type, src_ip=self.tunnel_ip, dst_ip=self.my_lb_ip)

        # create tunnel nexthop for VM 100.100.1.1
        self.tunnel_nexthop = self.add_nexthop(self.device,
            type=SWITCH_NEXTHOP_ATTR_TYPE_TUNNEL,
            rw_type=SWITCH_NEXTHOP_ATTR_RW_TYPE_L3,
            handle=self.tunnel,
            dest_ip=self.tunnel_ip,
            mac_address=self.inner_dmac)
        # add route to tunnel_nexthop
        self.customer_route = self.add_route(self.device,
            ip_prefix=self.vm_ip4, vrf_handle=self.ovrf,
            nexthop_handle=self.tunnel_nexthop)
        self.customer_route = self.add_route(self.device,
            ip_prefix=self.vm_ip6, vrf_handle=self.ovrf,
            nexthop_handle=self.tunnel_nexthop)

        # Create route to customer from VM
        self.orif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port0, vrf_handle=self.ovrf, src_mac=self.rmac)
        self.onhop4 = self.add_nexthop(self.device, handle=self.orif, dest_ip=self.customer_ip4)
        self.oneighbor4 = self.add_neighbor(self.device,
            mac_address='00:22:22:22:22:33',
            handle=self.orif,
            dest_ip=self.customer_ip4)  # 100.100.0.1
        self.customer_route = self.add_route(self.device, ip_prefix=self.customer_ip4,
            vrf_handle=self.ovrf, nexthop_handle=self.onhop4)
        self.onhop6 = self.add_nexthop(self.device, handle=self.orif, dest_ip=self.customer_ip6)
        self.oneighbor6 = self.add_neighbor(self.device,
            mac_address='00:22:22:22:22:33',
            handle=self.orif,
            dest_ip=self.customer_ip6)  # 100.100.0.1
        self.customer_route = self.add_route(self.device, ip_prefix=self.customer_ip6,
            vrf_handle=self.ovrf, nexthop_handle=self.onhop6)

    def runTest(self):
        if (self.client.is_feature_enable(SWITCH_FEATURE_IPV6_TUNNEL) == 0):
            return
        try:
            self.IPv4inIPv6Test()
            self.IPv6inIPv6Test()
        finally:
            pass

    def tearDown(self):
        self.cleanup()

    def IPv4inIPv6Test(self):
        print("IPv4inIPv6Test()")
        try:
            print("Verifying IP 4in6 (encap)")
            if not self.client.is_feature_enable(SWITCH_FEATURE_TUNNEL_ENCAP):
                print(" Tunnel encap not enabled, skipping")
            else:
                pkt = simple_tcp_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src='00:22:22:22:22:33',
                    ip_dst=self.vm_ip4,
                    ip_src=self.customer_ip4,
                    ip_id=108,
                    ip_ttl=64)
                pkt2 = simple_tcp_packet(
                    eth_dst=self.inner_dmac,  # 00:33:33:33:33:33
                    eth_src=self.default_rmac,
                    ip_dst=self.vm_ip4,
                    ip_src=self.customer_ip4,
                    ip_id=108,
                    ip_ttl=63)
                ipip_pkt = simple_ipv6ip_packet(
                    eth_dst=self.underlay_neighbor_mac,  # 00:11:11:11:11:11
                    eth_src='00:77:66:55:44:33',
                    ipv6_src=self.my_lb_ip,
                    ipv6_dst=self.tunnel_ip,
                    ipv6_hlim=64,
                    inner_frame=pkt2['IP'])
                send_packet(self, self.devports[0], pkt)
                verify_packet(self, ipip_pkt, self.devports[1])

            print("Verifying IP 4in6 (decap)")
            pkt1 = simple_tcp_packet(
                eth_src='00:77:66:55:44:33',
                eth_dst=self.inner_dmac,  # 00:33:33:33:33:33
                ip_dst=self.customer_ip4,
                ip_src=self.vm_ip4,
                ip_id=108,
                ip_ttl=64)
            ipip_pkt = simple_ipv6ip_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src=self.underlay_neighbor_mac,  # 00:11:11:11:11:11
                ipv6_src=self.tunnel_ip,
                ipv6_dst=self.my_lb_ip,
                ipv6_hlim=64,
                inner_frame=pkt1['IP'])
            pkt2 = simple_tcp_packet(
                eth_src='00:77:66:55:44:33',
                eth_dst='00:22:22:22:22:33',
                ip_dst=self.customer_ip4,
                ip_src=self.vm_ip4,
                ip_id=108,
                ip_ttl=63)
            send_packet(self, self.devports[1], ipip_pkt)
            verify_packet(self, pkt2, self.devports[0])
        finally:
            pass

    def IPv6inIPv6Test(self):
        print("IPv6inIPv6Test()")
        try:
            print("Verifying IP 6in6 (encap)")
            if not self.client.is_feature_enable(SWITCH_FEATURE_TUNNEL_ENCAP):
                print(" Tunnel encap not enabled, skipping")
            else:
                pkt = simple_tcpv6_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src='00:22:22:22:22:33',
                    ipv6_dst=self.vm_ip6,
                    ipv6_src=self.customer_ip6,
                    ipv6_hlim=64)
                pkt2 = simple_tcpv6_packet(
                    eth_dst=self.inner_dmac,  # 00:33:33:33:33:33
                    eth_src=self.default_rmac,
                    ipv6_dst=self.vm_ip6,
                    ipv6_src=self.customer_ip6,
                    ipv6_hlim=63)
                ipip_pkt = simple_ipv6ip_packet(
                    eth_dst=self.underlay_neighbor_mac,  # 00:11:11:11:11:11
                    eth_src='00:77:66:55:44:33',
                    ipv6_src=self.my_lb_ip,
                    ipv6_dst=self.tunnel_ip,
                    ipv6_hlim=64,
                    inner_frame=pkt2['IPv6'])
                send_packet(self, self.devports[0], pkt)
                verify_packet(self, ipip_pkt, self.devports[1])

            print("Verifying IP 6in6 (decap)")
            pkt1 = simple_tcpv6_packet(
                eth_src='00:77:66:55:44:33',
                eth_dst=self.inner_dmac,  # 00:33:33:33:33:33
                ipv6_dst=self.customer_ip6,
                ipv6_src=self.vm_ip6,
                ipv6_hlim=64)
            ipip_pkt = simple_ipv6ip_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src=self.underlay_neighbor_mac,  # 00:11:11:11:11:11
                ipv6_src=self.tunnel_ip,
                ipv6_dst=self.my_lb_ip,
                ipv6_hlim=64,
                inner_frame=pkt1['IPv6'])
            pkt2 = simple_tcpv6_packet(
                eth_src='00:77:66:55:44:33',
                eth_dst='00:22:22:22:22:33',
                ipv6_dst=self.customer_ip6,
                ipv6_src=self.vm_ip6,
                ipv6_hlim=63)
            send_packet(self, self.devports[1], ipip_pkt)
            verify_packet(self, pkt2, self.devports[0])
        finally:
            pass

###############################################################################

@disabled
class L3TunnelIPinIPv4GRETest(ApiHelper):
    def setUp(self):
        self.configure()
        if ((self.client.is_feature_enable(SWITCH_FEATURE_IPV4_TUNNEL) == 0) or
            (self.client.is_feature_enable(SWITCH_FEATURE_IPGRE) == 0)):
            print("IPv4 tunnel feature not enabled, skipping")
            return
        print()

        # underlay config input
        self.uvrf = self.add_vrf(self.device, id=100, src_mac=self.rmac)
        self.vm_ip4 = '100.100.1.1'
        self.vm_ip6 = '1234:5678:9abc:def0:4422:1133:5577:9951'
        self.customer_ip4 = '100.100.0.1'
        self.customer_ip6 = '1234:5678:9abc:def0:4422:1133:5577:0011'
        self.my_lb_ip = '10.10.10.10'
        self.tunnel_ip = '10.10.10.1'
        self.inner_dmac = "00:33:33:33:33:33"
        self.underlay_neighbor_mac = '00:11:11:11:11:11'

        # overlay configuration
        self.ovrf = self.add_vrf(self.device, id = 300, src_mac=self.rmac)
        self.tunnel_type = SWITCH_TUNNEL_ATTR_TYPE_IPGRE
        self.term_type = SWITCH_TUNNEL_TERM_ATTR_TERMINATION_TYPE_P2P
        self.encap_ttl_mode = SWITCH_TUNNEL_ATTR_ENCAP_TTL_MODE_PIPE_MODEL
        self.decap_ttl_mode = SWITCH_TUNNEL_ATTR_DECAP_TTL_MODE_PIPE_MODEL

        self.urif_lb = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_LOOPBACK, vrf_handle=self.uvrf, src_mac=self.rmac)

        # create underlay loopback rif for tunnel
        self.orif_lb = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_LOOPBACK, vrf_handle=self.ovrf, src_mac=self.rmac)

        # Create route to tunnel ip 10.10.10.1
        self.urif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port1, vrf_handle=self.uvrf, src_mac=self.rmac)
        self.unhop = self.add_nexthop(self.device, handle=self.urif, dest_ip=self.tunnel_ip)
        self.uneighbor = self.add_neighbor(self.device, mac_address=self.underlay_neighbor_mac,
            handle=self.urif, dest_ip=self.tunnel_ip)  # 10.10.10.1
        self.tunnel_route1 = self.add_route(self.device, ip_prefix=self.tunnel_ip,
            vrf_handle=self.uvrf, nexthop_handle=self.unhop)

        # create Tunnel
        self.tunnel = self.add_tunnel(self.device, type=self.tunnel_type,
            src_ip=self.my_lb_ip,
            encap_ttl_mode=self.encap_ttl_mode, decap_ttl_mode=self.decap_ttl_mode,
            underlay_rif_handle=self.urif_lb, overlay_rif_handle=self.orif_lb)

        # Create tunnel_termination entry
        self.tunnel_term = self.add_tunnel_term(self.device, type=self.tunnel_type,
            vrf_handle=self.uvrf, tunnel_handle=self.tunnel,
            termination_type=self.term_type, src_ip=self.tunnel_ip, dst_ip=self.my_lb_ip)

        # create tunnel nexthop for VM 100.100.1.1
        self.tunnel_nexthop = self.add_nexthop(self.device,
            type=SWITCH_NEXTHOP_ATTR_TYPE_TUNNEL,
            rw_type=SWITCH_NEXTHOP_ATTR_RW_TYPE_L3,
            handle=self.tunnel,
            dest_ip=self.tunnel_ip,  # 10.10.10.1
            mac_address=self.inner_dmac)
        # add route to tunnel_nexthop
        self.customer_route = self.add_route(self.device,
            ip_prefix=self.vm_ip4, vrf_handle=self.ovrf,
            nexthop_handle=self.tunnel_nexthop)
        self.customer_route = self.add_route(self.device,
            ip_prefix=self.vm_ip6, vrf_handle=self.ovrf,
            nexthop_handle=self.tunnel_nexthop)

        # Create route to customer from VM
        self.orif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port0, vrf_handle=self.ovrf, src_mac=self.rmac)
        self.onhop4 = self.add_nexthop(self.device, handle=self.orif, dest_ip=self.customer_ip4)
        self.oneighbor4 = self.add_neighbor(self.device,
            mac_address='00:22:22:22:22:33',
            handle=self.orif,
            dest_ip=self.customer_ip4)  # 100.100.0.1
        self.customer_route = self.add_route(self.device, ip_prefix=self.customer_ip4,
            vrf_handle=self.ovrf, nexthop_handle=self.onhop4)
        self.onhop6 = self.add_nexthop(self.device, handle=self.orif, dest_ip=self.customer_ip6)
        self.oneighbor6 = self.add_neighbor(self.device,
            mac_address='00:22:22:22:22:33',
            handle=self.orif,
            dest_ip=self.customer_ip6)  # 100.100.0.1
        self.customer_route = self.add_route(self.device, ip_prefix=self.customer_ip6,
            vrf_handle=self.ovrf, nexthop_handle=self.onhop6)

    def runTest(self):
        if ((self.client.is_feature_enable(SWITCH_FEATURE_IPV4_TUNNEL) == 0) or
            (self.client.is_feature_enable(SWITCH_FEATURE_IPGRE) == 0)):
            return
        try:
            self.IPv4inIPv4GRETest()
            self.IPv6inIPv4GRETest()
        finally:
            pass

    def tearDown(self):
        self.cleanup()

    def IPv4inIPv4GRETest(self):
        print("IPv4inIPv4GRETest()")
        try:
            print("Verifying IP-GRE 4in4 (encap)")
            if not self.client.is_feature_enable(SWITCH_FEATURE_TUNNEL_ENCAP) :
                print(" Tunnel encap not enabled, skipping")
            else:
                pkt = simple_tcp_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src='00:22:22:22:22:33',
                    ip_dst=self.vm_ip4,
                    ip_src=self.customer_ip4,
                    ip_id=108,
                    ip_ttl=64)
                pkt2 = simple_tcp_packet(
                    eth_dst=self.inner_dmac,  # 00:33:33:33:33:33
                    eth_src=self.default_rmac,
                    ip_dst=self.vm_ip4,
                    ip_src=self.customer_ip4,
                    ip_id=108,
                    ip_ttl=63)
                ipgre_pkt = simple_gre_packet(
                    eth_dst=self.underlay_neighbor_mac,  # 00:11:11:11:11:11
                    eth_src='00:77:66:55:44:33',
                    ip_id=0,
                    ip_src='10.10.10.10',
                    ip_dst='10.10.10.1',
                    ip_ttl=64,
                    inner_frame=pkt2['IP'])
                send_packet(self, self.devports[0], pkt)
                verify_packet(self, ipgre_pkt, self.devports[1])

            print("Verifying IP-GRE 4in4 (decap)")
            pkt1 = simple_tcp_packet(
                eth_src='00:77:66:55:44:33',
                eth_dst=self.inner_dmac,  # 00:33:33:33:33:33
                ip_dst=self.customer_ip4,
                ip_src=self.vm_ip4,
                ip_id=108,
                ip_ttl=64)
            ipgre_pkt = simple_gre_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src=self.underlay_neighbor_mac,  # 00:11:11:11:11:11
                ip_id=0,
                ip_src='10.10.10.1',
                ip_dst='10.10.10.10',
                ip_ttl=64,
                inner_frame=pkt1['IP'])
            pkt2 = simple_tcp_packet(
                eth_src='00:77:66:55:44:33',
                eth_dst='00:22:22:22:22:33',
                ip_dst=self.customer_ip4,
                ip_src=self.vm_ip4,
                ip_id=108,
                ip_ttl=63)
            send_packet(self, self.devports[1], ipgre_pkt)
            verify_packet(self, pkt2, self.devports[0])
        finally:
            pass

    def IPv6inIPv4GRETest(self):
        print("IPv6inIPv4GRETest()")
        try:
            print("Verifying IP-GRE 6in4 (encap)")
            if not self.client.is_feature_enable(SWITCH_FEATURE_TUNNEL_ENCAP):
                print(" Tunnel encap not enabled, skipping")
            else:
                pkt = simple_tcpv6_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src='00:22:22:22:22:33',
                    ipv6_dst=self.vm_ip6,
                    ipv6_src=self.customer_ip6,
                    ipv6_hlim=64)
                pkt2 = simple_tcpv6_packet(
                    eth_dst=self.inner_dmac,  # 00:33:33:33:33:33
                    eth_src=self.default_rmac,
                    ipv6_dst=self.vm_ip6,
                    ipv6_src=self.customer_ip6,
                    ipv6_hlim=63)
                ipgre_pkt = simple_gre_packet(
                    eth_dst=self.underlay_neighbor_mac,  # 00:11:11:11:11:11
                    eth_src='00:77:66:55:44:33',
                    ip_id=0,
                    ip_src='10.10.10.10',
                    ip_dst='10.10.10.1',
                    ip_ttl=64,
                    inner_frame=pkt2['IPv6'])
                send_packet(self, self.devports[0], pkt)
                verify_packet(self, ipgre_pkt, self.devports[1])

            print("Verifying IP-GRE 6in4 (decap)")
            pkt1 = simple_tcpv6_packet(
                eth_src='00:77:66:55:44:33',
                eth_dst=self.inner_dmac,  # 00:33:33:33:33:33
                ipv6_dst=self.customer_ip6,
                ipv6_src=self.vm_ip6,
                ipv6_hlim=64)
            ipgre_pkt = simple_gre_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src=self.underlay_neighbor_mac,  # 00:11:11:11:11:11
                ip_id=0,
                ip_src='10.10.10.1',
                ip_dst='10.10.10.10',
                ip_ttl=64,
                inner_frame=pkt1['IPv6'])
            pkt2 = simple_tcpv6_packet(
                eth_src='00:77:66:55:44:33',
                eth_dst='00:22:22:22:22:33',
                ipv6_dst=self.customer_ip6,
                ipv6_src=self.vm_ip6,
                ipv6_hlim=63)
            send_packet(self, self.devports[1], ipgre_pkt)
            verify_packet(self, pkt2, self.devports[0])
        finally:
            pass

###############################################################################

@disabled
class L3TunnelIPinIPv6GRETest(ApiHelper):
    def setUp(self):
        self.configure()
        if ((self.client.is_feature_enable(SWITCH_FEATURE_IPV6_TUNNEL) == 0) or
            (self.client.is_feature_enable(SWITCH_FEATURE_IPGRE) == 0)):
            print("IPv6 tunnel feature not enabled, skipping")
            return
        print()

        # underlay config input
        self.uvrf = self.add_vrf(self.device, id=100, src_mac=self.rmac)
        self.vm_ip4 = '100.100.1.1'
        self.vm_ip6 = '1234:5678:9abc:def0:4422:1133:5577:9951'
        self.customer_ip4 = '100.100.0.1'
        self.customer_ip6 = '1234:5678:9abc:def0:4422:1133:5577:0011'
        self.my_lb_ip = '4444:5678:9abc:def0:4422:1133:5577:44'
        self.tunnel_ip = '4444:5678:9abc:def0:4422:1133:5577:1'
        self.inner_dmac = "00:33:33:33:33:33"
        self.underlay_neighbor_mac = '00:11:11:11:11:11'

        # overlay configuration
        self.ovrf = self.add_vrf(self.device, id = 300, src_mac=self.rmac)
        self.tunnel_type = SWITCH_TUNNEL_ATTR_TYPE_IPGRE
        self.term_type = SWITCH_TUNNEL_TERM_ATTR_TERMINATION_TYPE_P2P
        self.encap_ttl_mode = SWITCH_TUNNEL_ATTR_ENCAP_TTL_MODE_PIPE_MODEL
        self.decap_ttl_mode = SWITCH_TUNNEL_ATTR_DECAP_TTL_MODE_PIPE_MODEL

        self.urif_lb = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_LOOPBACK, vrf_handle=self.uvrf, src_mac=self.rmac)

        # create underlay loopback rif for tunnel
        self.orif_lb = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_LOOPBACK, vrf_handle=self.ovrf, src_mac=self.rmac)

        # Create route to tunnel ip 10.10.10.1
        self.urif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port1, vrf_handle=self.uvrf, src_mac=self.rmac)
        self.unhop = self.add_nexthop(self.device, handle=self.urif, dest_ip=self.tunnel_ip)
        self.uneighbor = self.add_neighbor(self.device, mac_address=self.underlay_neighbor_mac,
            handle=self.urif, dest_ip=self.tunnel_ip)
        self.tunnel_route1 = self.add_route(self.device, ip_prefix=self.tunnel_ip,
            vrf_handle=self.uvrf, nexthop_handle=self.unhop)

        # create Tunnel
        self.tunnel = self.add_tunnel(self.device, type=self.tunnel_type,
            src_ip=self.my_lb_ip,
            encap_ttl_mode=self.encap_ttl_mode, decap_ttl_mode=self.decap_ttl_mode,
            underlay_rif_handle=self.urif_lb, overlay_rif_handle=self.orif_lb)

        # Create tunnel_termination entry
        self.tunnel_term = self.add_tunnel_term(self.device, type=self.tunnel_type,
            vrf_handle=self.uvrf, tunnel_handle=self.tunnel,
            termination_type=self.term_type, src_ip=self.tunnel_ip, dst_ip=self.my_lb_ip)

        # create tunnel nexthop for VM 100.100.1.1
        self.tunnel_nexthop = self.add_nexthop(self.device,
            type=SWITCH_NEXTHOP_ATTR_TYPE_TUNNEL,
            rw_type=SWITCH_NEXTHOP_ATTR_RW_TYPE_L3,
            handle=self.tunnel,
            dest_ip=self.tunnel_ip,
            mac_address=self.inner_dmac)
        # add route to tunnel_nexthop
        self.customer_route = self.add_route(self.device,
            ip_prefix=self.vm_ip4, vrf_handle=self.ovrf,
            nexthop_handle=self.tunnel_nexthop)
        self.customer_route = self.add_route(self.device,
            ip_prefix=self.vm_ip6, vrf_handle=self.ovrf,
            nexthop_handle=self.tunnel_nexthop)

        # Create route to customer from VM
        self.orif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port0, vrf_handle=self.ovrf, src_mac=self.rmac)
        self.onhop4 = self.add_nexthop(self.device, handle=self.orif, dest_ip=self.customer_ip4)
        self.oneighbor4 = self.add_neighbor(self.device,
            mac_address='00:22:22:22:22:33',
            handle=self.orif,
            dest_ip=self.customer_ip4)  # 100.100.0.1
        self.customer_route = self.add_route(self.device, ip_prefix=self.customer_ip4,
            vrf_handle=self.ovrf, nexthop_handle=self.onhop4)
        self.onhop6 = self.add_nexthop(self.device, handle=self.orif, dest_ip=self.customer_ip6)
        self.oneighbor6 = self.add_neighbor(self.device,
            mac_address='00:22:22:22:22:33',
            handle=self.orif,
            dest_ip=self.customer_ip6)  # 100.100.0.1
        self.customer_route = self.add_route(self.device, ip_prefix=self.customer_ip6,
            vrf_handle=self.ovrf, nexthop_handle=self.onhop6)

    def runTest(self):
        if ((self.client.is_feature_enable(SWITCH_FEATURE_IPV6_TUNNEL) == 0) or
            (self.client.is_feature_enable(SWITCH_FEATURE_IPGRE) == 0)):
            return
        try:
            self.IPv4inIPv6GRETest()
            self.IPv6inIPv6GRETest()
        finally:
            pass

    def tearDown(self):
        self.cleanup()

    def IPv4inIPv6GRETest(self):
        print("IPv4inIPv6GRETest()")
        try:
            print("Verifying IP-GRE 4in6 (encap)")
            if not self.client.is_feature_enable(SWITCH_FEATURE_TUNNEL_ENCAP):
                print(" Tunnel encap not enabled, skipping")
            else:
                pkt = simple_tcp_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src='00:22:22:22:22:33',
                    ip_dst=self.vm_ip4,
                    ip_src=self.customer_ip4,
                    ip_id=108,
                    ip_ttl=64)
                pkt2 = simple_tcp_packet(
                    eth_dst=self.inner_dmac,  # 00:33:33:33:33:33
                    eth_src=self.default_rmac,
                    ip_dst=self.vm_ip4,
                    ip_src=self.customer_ip4,
                    ip_id=108,
                    ip_ttl=63)
                ipgre_pkt = simple_grev6_packet(
                    eth_dst=self.underlay_neighbor_mac,  # 00:11:11:11:11:11
                    eth_src='00:77:66:55:44:33',
                    ipv6_src=self.my_lb_ip,
                    ipv6_dst=self.tunnel_ip,
                    ipv6_hlim=64,
                    inner_frame=pkt2['IP'])
                send_packet(self, self.devports[0], pkt)
                verify_packet(self, ipgre_pkt, self.devports[1])

            print("Verifying IP-GRE 4in6 (decap)")
            pkt1 = simple_tcp_packet(
                eth_src='00:77:66:55:44:33',
                eth_dst=self.inner_dmac,  # 00:33:33:33:33:33
                ip_dst=self.customer_ip4,
                ip_src=self.vm_ip4,
                ip_id=108,
                ip_ttl=64)
            ipgre_pkt = simple_grev6_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src=self.underlay_neighbor_mac,  # 00:11:11:11:11:11
                ipv6_src=self.tunnel_ip,
                ipv6_dst=self.my_lb_ip,
                ipv6_hlim=64,
                inner_frame=pkt1['IP'])
            pkt2 = simple_tcp_packet(
                eth_src='00:77:66:55:44:33',
                eth_dst='00:22:22:22:22:33',
                ip_dst=self.customer_ip4,
                ip_src=self.vm_ip4,
                ip_id=108,
                ip_ttl=63)
            send_packet(self, self.devports[1], ipgre_pkt)
            verify_packet(self, pkt2, self.devports[0])
        finally:
            pass

    def IPv6inIPv6GRETest(self):
        print("IPv6inIPv6GRETest()")
        try:
            print("Verifying IP-GRE 6in6 (encap)")
            if not self.client.is_feature_enable(SWITCH_FEATURE_TUNNEL_ENCAP):
                print(" Tunnel encap not enabled, skipping")
            else:
                pkt = simple_tcpv6_packet(
                    eth_dst='00:77:66:55:44:33',
                    eth_src='00:22:22:22:22:33',
                    ipv6_dst=self.vm_ip6,
                    ipv6_src=self.customer_ip6,
                    ipv6_hlim=64)
                pkt2 = simple_tcpv6_packet(
                    eth_dst=self.inner_dmac,  # 00:33:33:33:33:33
                    eth_src=self.default_rmac,
                    ipv6_dst=self.vm_ip6,
                    ipv6_src=self.customer_ip6,
                    ipv6_hlim=63)
                ipgre_pkt = simple_grev6_packet(
                    eth_dst=self.underlay_neighbor_mac,  # 00:11:11:11:11:11
                    eth_src='00:77:66:55:44:33',
                    ipv6_src=self.my_lb_ip,
                    ipv6_dst=self.tunnel_ip,
                    ipv6_hlim=64,
                    inner_frame=pkt2['IPv6'])
                send_packet(self, self.devports[0], pkt)
                verify_packet(self, ipgre_pkt, self.devports[1])

            print("Verifying IP-GRE 6in6 (decap)")
            pkt1 = simple_tcpv6_packet(
                eth_src='00:77:66:55:44:33',
                eth_dst=self.inner_dmac,  # 00:33:33:33:33:33
                ipv6_dst=self.customer_ip6,
                ipv6_src=self.vm_ip6,
                ipv6_hlim=64)
            ipgre_pkt = simple_grev6_packet(
                eth_dst='00:77:66:55:44:33',
                eth_src=self.underlay_neighbor_mac,  # 00:11:11:11:11:11
                ipv6_src=self.tunnel_ip,
                ipv6_dst=self.my_lb_ip,
                ipv6_hlim=64,
                inner_frame=pkt1['IPv6'])
            pkt2 = simple_tcpv6_packet(
                eth_src='00:77:66:55:44:33',
                eth_dst='00:22:22:22:22:33',
                ipv6_dst=self.customer_ip6,
                ipv6_src=self.vm_ip6,
                ipv6_hlim=63)
            send_packet(self, self.devports[1], ipgre_pkt)
            verify_packet(self, pkt2, self.devports[0])
        finally:
            pass

###############################################################################

    '''
               --------------------
               | VM: 100.100.1.1  |
               | Host: 10.10.10.1 |
               --------------------
                         |
      -------------------------------------------
      |                | p1 |                   |
      |                ------                   |
      |                   |                     |
      |         -----------------------         |
      |         | Tunnel: 10.10.10.10 |         |
      |         -----------------------         |
      |                   |                     |
      |                ---------                |
      |                | SVI   |                |
      |                | p0 p2 |                |
      -------------------------------------------
                         |   |
        ------------------- -------------------
        | C1: 100.100.3.1 | | C2: 100.100.3.2 |
        ------------------- -------------------

    SVI : L3 VLAN interface(c1, c2)
    VLAN 10 <-> VNI 2000
    '''
@disabled
class L2TunnelTest(ApiHelper):
    def runTest(self):
        print()
        if (self.client.is_feature_enable(SWITCH_FEATURE_IPV4_TUNNEL) == 0):
            print("IPv4 tunnel feature not enabled, skipping")
            return

        self.configure()

        # underlay config input
        self.uvrf = self.add_vrf(self.device, id=100, src_mac=self.rmac)
        self.vm_ip = '100.100.1.1'
        self.customer_ip = '100.100.3.1'
        self.customer_ip2 = '100.100.3.2'
        self.vni = 2000
        self.my_lb_ip = '10.10.10.10'
        self.tunnel_ip = '10.10.10.1'
        self.inner_dmac = "00:33:33:33:33:33"
        self.underlay_neighbor_mac = '00:11:11:11:11:11'

        # overlay configuration
        self.svi_rif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_VLAN, vlan_handle=self.vlan10, vrf_handle=self.uvrf, src_mac=self.rmac)
        vlan_mbr0 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port0)
        vlan_mbr1 = self.add_vlan_member(self.device, vlan_handle=self.vlan10, member_handle=self.port2)
        self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)
        self.attribute_set(self.port2, SWITCH_PORT_ATTR_PORT_VLAN_ID, 10)

        self.tunnel_type = SWITCH_TUNNEL_ATTR_TYPE_VXLAN
        self.term_type = SWITCH_TUNNEL_TERM_ATTR_TERMINATION_TYPE_P2MP
        self.encap_ttl_mode = SWITCH_TUNNEL_ATTR_ENCAP_TTL_MODE_PIPE_MODEL
        self.decap_ttl_mode = SWITCH_TUNNEL_ATTR_DECAP_TTL_MODE_PIPE_MODEL

        # create underlay loopback rif for tunnel
        self.urif_lb = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_LOOPBACK, vrf_handle=self.uvrf, src_mac=self.rmac)

        # Create route to tunnel ip 10.10.10.1
        self.urif = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.port1, vrf_handle=self.uvrf, src_mac=self.rmac)
        self.unhop = self.add_nexthop(self.device, handle=self.urif, dest_ip=self.tunnel_ip)
        self.uneighbor = self.add_neighbor(self.device,
            mac_address=self.underlay_neighbor_mac,
            handle=self.urif, dest_ip=self.tunnel_ip)  # 10.10.10.1
        self.tunnel_route1 = self.add_route(self.device, ip_prefix="10.10.10.1",
            vrf_handle=self.uvrf, nexthop_handle=self.unhop)

        # Encap configuration follows

        # create Encap/Decap mappers
        self.encap_tunnel_map = self.add_tunnel_mapper(self.device,
            type=SWITCH_TUNNEL_MAPPER_ATTR_TYPE_VLAN_HANDLE_TO_VNI)
        self.decap_tunnel_map = self.add_tunnel_mapper(self.device,
            type=SWITCH_TUNNEL_MAPPER_ATTR_TYPE_VNI_TO_VLAN_HANDLE)

        # create Encap/Decap mapper entries for vlan10
        self.encap_tunnel_map_entry = self.add_tunnel_mapper_entry(self.device,
            type=SWITCH_TUNNEL_MAPPER_ENTRY_ATTR_TYPE_VLAN_HANDLE_TO_VNI,
            tunnel_mapper_handle=self.encap_tunnel_map,
            network_handle=self.vlan10,
            tunnel_vni=self.vni)
        self.decap_tunnel_map_entry = self.add_tunnel_mapper_entry(self.device,
            type=SWITCH_TUNNEL_MAPPER_ENTRY_ATTR_TYPE_VNI_TO_VLAN_HANDLE,
            tunnel_mapper_handle=self.decap_tunnel_map,
            network_handle=self.vlan10,
            tunnel_vni=self.vni)

        ingress_mapper_list = [switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=self.encap_tunnel_map)]
        egress_mapper_list = [switcht_list_val_t(type=switcht_value_type.OBJECT_ID, oid=self.decap_tunnel_map)]

        # create Tunnel
        self.tunnel = self.add_tunnel(self.device,
            type=self.tunnel_type,
            src_ip=self.my_lb_ip,
            ingress_mapper_handles=ingress_mapper_list,
            egress_mapper_handles=egress_mapper_list,
            encap_ttl_mode=self.encap_ttl_mode,
            decap_ttl_mode=self.decap_ttl_mode,
            underlay_rif_handle=self.urif_lb)

        # Decap configuration follows

        # Create tunnel_termination entry
        self.tunnel_term = self.add_tunnel_term(self.device,
            type=self.tunnel_type,
            vrf_handle=self.uvrf,
            tunnel_handle=self.tunnel,
            termination_type=self.term_type,
            dst_ip=self.my_lb_ip)

        # create tunnel nexthop for VM
        self.tunnel_nexthop = self.add_nexthop(self.device,
            type=SWITCH_NEXTHOP_ATTR_TYPE_TUNNEL,
            rw_type=SWITCH_NEXTHOP_ATTR_RW_TYPE_L2,
            handle=self.tunnel,
            dest_ip=self.tunnel_ip,  # 10.10.10.1
            mac_address=self.inner_dmac,
            tunnel_vni=self.vni)

        # Create mac entry to tunnel vm
        mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:44:44:44:44:44', dest_ip=self.tunnel_ip, destination_handle=self.tunnel)
        # Create mac entry to customer
        mac0 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:22:22:22:22:22', destination_handle=self.port0)
        mac1 = self.add_mac_entry(self.device, vlan_handle=self.vlan10, mac_address='00:55:55:55:55:55', destination_handle=self.port2)

        try:
            self.UnicastTest()
            self.DecapSviTest()
        finally:
            self.attribute_set(self.port0, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.attribute_set(self.port2, SWITCH_PORT_ATTR_PORT_VLAN_ID, 0)
            self.cleanup()

    def UnicastTest(self):
        try:
            print("Sending L2 packet from Access port 0 to VxLan port 1")
            pkt = simple_udp_packet(
                eth_dst='00:44:44:44:44:44',
                eth_src='00:22:22:22:22:22',
                ip_dst=self.vm_ip,
                ip_src=self.customer_ip,
                ip_ttl=64)
            inner_pkt = simple_udp_packet(
                eth_dst='00:44:44:44:44:44',
                eth_src='00:22:22:22:22:22',
                ip_dst=self.vm_ip,
                ip_src=self.customer_ip,
                ip_ttl=64)
            vxlan_pkt = mask.Mask(simple_vxlan_packet(
                eth_src='00:77:66:55:44:33',
                eth_dst=self.underlay_neighbor_mac,  # 00:11:11:11:11:11
                ip_id=0,
                ip_src=self.my_lb_ip,
                ip_dst=self.tunnel_ip,
                ip_ttl=64,
                ip_flags=0x2,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=inner_pkt))
            mask_set_do_not_care_packet(vxlan_pkt, UDP, "sport")
            send_packet(self, self.devports[0], pkt)
            verify_packet(self, vxlan_pkt, self.devports[1])

            print("Sending L2 packet from Vxlan port 2 to Access port 0")
            pkt = simple_tcp_packet(
                eth_dst='00:22:22:22:22:22',
                eth_src='00:44:44:44:44:44',
                ip_dst=self.customer_ip2,
                ip_src=self.vm_ip,
                ip_ttl=64)
            vxlan_pkt = simple_vxlan_packet(
                eth_src=self.underlay_neighbor_mac,
                eth_dst='00:77:66:55:44:33',
                ip_id=0,
                ip_dst=self.my_lb_ip,
                ip_src=self.tunnel_ip,
                ip_ttl=64,
                ip_flags=0x2,
                udp_sport=11638,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=pkt)
            send_packet(self, self.devports[1], vxlan_pkt)
            verify_packet(self, pkt, self.devports[0])
        finally:
            pass

    def DecapSviTest(self):
        try:
            print("Sending Vxlan packet from access port 2 to access port 0")
            pkt = simple_tcp_packet(
                eth_dst='00:22:22:22:22:22',
                eth_src='00:44:44:44:44:44',
                ip_dst=self.customer_ip,
                ip_src=self.vm_ip,
                ip_ttl=64)
            vxlan_pkt = simple_vxlan_packet(
                eth_src=self.underlay_neighbor_mac,
                eth_dst='00:77:66:55:44:33',
                ip_id=0,
                ip_dst=self.my_lb_ip,
                ip_src=self.tunnel_ip,
                ip_ttl=64,
                ip_flags=0x2,
                udp_sport=11638,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=pkt)
            send_packet(self, self.devports[2], vxlan_pkt)
            verify_packet(self, pkt, self.devports[0])

            print("Sending Vxlan packet from access port 2 to access port 2")
            pkt = simple_tcp_packet(
                eth_dst='00:55:55:55:55:55',
                eth_src='00:44:44:44:44:44',
                ip_dst=self.customer_ip,
                ip_src=self.vm_ip,
                ip_ttl=64)
            vxlan_pkt = simple_vxlan_packet(
                eth_src=self.underlay_neighbor_mac,
                eth_dst='00:77:66:55:44:33',
                ip_id=0,
                ip_dst=self.my_lb_ip,
                ip_src=self.tunnel_ip,
                ip_ttl=64,
                ip_flags=0x2,
                udp_sport=11638,
                with_udp_chksum=False,
                vxlan_vni=self.vni,
                inner_frame=pkt)
            send_packet(self, self.devports[2], vxlan_pkt)
            verify_packet(self, pkt, self.devports[2])
        finally:
            pass

@disabled
class PowerProfileTest(ApiHelper):
    def add_snake_table_entry(self, ingress_dev_port, egress_dev_port):
        return self.add_snake(self.device, ingress_port_handle = self.devport_port_hdl_dict[ingress_dev_port],
                egress_port_handle = self.devport_port_hdl_dict[egress_dev_port])

    def test_configure(self):
        num_pipes = int(test_param_get('num_pipes'))
        port_mode = test_param_get("port_mode")

        self.device = self.get_device_handle(0)
        port_max = 64 * num_pipes
        port_step = 1

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
        elif port_mode == '49g-r1':
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
            self.assertTrue(False, "Unsupported psrt-modedVd %s" % (port_mode))

        self.ports = []
        self.devports = []
        self.devport_port_hdl_dict = {}
        for port in range(0, port_max, port_step):
            port_hdl = self.add_port(self.device, lane_list=u.lane_list_t([port]), speed=port_speed, fec_type=fec)
            self.ports.append(port_hdl)

            dev_port = self.attribute_get(port_hdl, SWITCH_PORT_ATTR_DEV_PORT)
            self.devports.append(dev_port)
            self.devport_port_hdl_dict[dev_port] = port_hdl

            pipe = dev_port >> 6
            local_port = dev_port & 0x7F
            print("Switch port %3d maps to dev port %3d" % (port, dev_port))

        #Setup the Snake Table as was setup with constant entries in P4 before
        '''
        const entries = {
        (8)   : set_egress_port(16);
        (16)  : set_egress_port(24);
        (24)  : set_egress_port(32);
        (32)  : set_egress_port(40);
        (40)  : set_egress_port(48);
        (48)  : set_egress_port(56);
        (56)  : set_egress_port(64);
        (64)  : set_egress_port(136);
        (136) : set_egress_port(144);
        (144) : set_egress_port(152);
        (152) : set_egress_port(160);
        (160) : set_egress_port(168);
        (168) : set_egress_port(176);
        (176) : set_egress_port(184);
        (184) : set_egress_port(192);
        (192) : set_egress_port(264);
        (264) : set_egress_port(272);
        (272) : set_egress_port(280);
        (280) : set_egress_port(288);
        (288) : set_egress_port(296);
        (296) : set_egress_port(304);
        (304) : set_egress_port(312);
        (312) : set_egress_port(320);
        (320) : set_egress_port(392);
        (392) : set_egress_port(400);
        (400) : set_egress_port(408);
        (408) : set_egress_port(416);
        (416) : set_egress_port(424);
        (424) : set_egress_port(432);
        (432) : set_egress_port(440);
        (440) : set_egress_port(448);
        (448) : set_egress_port(8);
        }
        '''
        self.snake_table_entries = []
        for index in range(len(self.devports)):
            in_port = self.devports[index]
            if index == (len(self.devports) - 1):
                out_port = self.devports[0]
            else:
                out_port = self.devports[index + 1]
            self.snake_table_entries.append(self.add_snake_table_entry(in_port, out_port))

        # Define host IPs for end to end communication
        self.customer_vms = 120
        v4_customer_ip_start = '100.100.100.0'
        v6_customer_ip_start = '1234:5678:9abc:dee0:1234:5678:9abc:0'
        v4_vm_ip_start = '100.100.100.128'
        v6_vm_ip_start = '1234:5678:9abc:def0:1234:5678:9abc:0'
        self.v4_customer_ips = []
        self.v6_customer_ips = []
        self.v4_vm_ips = []
        self.v6_vm_ips = []
        v4_customer_ip = int(binascii.hexlify(socket.inet_aton(v4_customer_ip_start)), 16)
        v6_customer_ip = int(binascii.hexlify(socket.inet_pton(socket.AF_INET6, v6_customer_ip_start)), 16)
        v4_vm_ip = int(binascii.hexlify(socket.inet_aton(v4_vm_ip_start)), 16)
        v6_vm_ip = int(binascii.hexlify(socket.inet_pton(socket.AF_INET6, v6_vm_ip_start)), 16)
        for address in range(0, 120):
            v4_customer_ip_address = socket.inet_ntoa(binascii.unhexlify(format(v4_customer_ip, 'x').zfill(8)))
            v6_customer_ip_address = socket.inet_ntop(socket.AF_INET6, binascii.unhexlify(format(v6_customer_ip, 'x').zfill(16)))
            v4_vm_ip_address = socket.inet_ntoa(binascii.unhexlify(format(v4_vm_ip, 'x').zfill(8)))
            v6_vm_ip_address = socket.inet_ntop(socket.AF_INET6, binascii.unhexlify(format(v6_vm_ip, 'x').zfill(16)))
            self.v4_customer_ips.append(v4_customer_ip_address)
            self.v6_customer_ips.append(v6_customer_ip_address)
            self.v4_vm_ips.append(v4_vm_ip_address)
            self.v6_vm_ips.append(v6_vm_ip_address)
            v4_customer_ip += 1
            v6_customer_ip += 1
            v4_vm_ip += 1
            v6_vm_ip += 1

        # Define Tunnel endpoints
        # Customer packets reach DUT via Tunnel Endpoint 1 with IPinIP encapsulationand are terminated (decap) on the DUT
        self.tunnel_endpoint1 = '10.10.10.1'
        # Terminated packets are then routed to their destination via an IPinIP tunnel (encap) through Endpoint 2 or
        # Endpoint 3
        self.tunnel_endpoint2 = '10.10.10.2'
        self.tunnel_endpoint3 = '10.10.10.3'

        #Configure Underlay Network on DUT
        self.rmac = '00:77:66:55:44:33'
        self.uvrf = self.add_vrf(self.device, id=100, src_mac=self.rmac)
        self.tunnel_lb_ip = '10.10.10.128'
        self.default_rmac = '00:BA:7E:F0:00:00' #Default Router MAC address added during SDK init
        self.urif_lb = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_LOOPBACK, vrf_handle=self.uvrf,
                src_mac=self.rmac)

        #Configure Overlay Network
        self.ovrf = self.add_vrf(self.device, id=300, src_mac=self.rmac)
        # create underlay loopback rif for tunnel
        self.orif_lb = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_LOOPBACK, vrf_handle=self.ovrf, src_mac=self.rmac)

        # create Tunnel
        self.tunnel_type = SWITCH_TUNNEL_ATTR_TYPE_IPIP
        self.term_type = SWITCH_TUNNEL_TERM_ATTR_TERMINATION_TYPE_P2P
        self.encap_ttl_mode = SWITCH_TUNNEL_ATTR_ENCAP_TTL_MODE_PIPE_MODEL
        self.decap_ttl_mode = SWITCH_TUNNEL_ATTR_DECAP_TTL_MODE_PIPE_MODEL
        self.tunnel = self.add_tunnel(self.device, type=self.tunnel_type,
            src_ip=self.tunnel_lb_ip,
            encap_ttl_mode=self.encap_ttl_mode, decap_ttl_mode=self.decap_ttl_mode,
            underlay_rif_handle=self.urif_lb, overlay_rif_handle=self.orif_lb)

        #Create Tunnel Termination Entry for tunnel endpoint 1,2 &3.
        #Although we have 3 termination entries only one(termination 1) is used in the test case below, as we have only unidirectional
        #traffic.
        self.tunnel_term1 = self.add_tunnel_term(self.device, type=self.tunnel_type, vrf_handle=self.uvrf,
                tunnel_handle=self.tunnel, termination_type = self.term_type, src_ip=self.tunnel_endpoint1,
                dst_ip=self.tunnel_lb_ip)
        self.tunnel_term2 = self.add_tunnel_term(self.device, type=self.tunnel_type, vrf_handle=self.uvrf,
                tunnel_handle=self.tunnel, termination_type = self.term_type, src_ip=self.tunnel_endpoint2,
                dst_ip=self.tunnel_lb_ip)
        self.tunnel_term3 = self.add_tunnel_term(self.device, type=self.tunnel_type, vrf_handle=self.uvrf,
                tunnel_handle=self.tunnel, termination_type = self.term_type, src_ip=self.tunnel_endpoint3,
                dst_ip=self.tunnel_lb_ip)
        self.tunnel_term4 = self.add_tunnel_term(self.device, type=self.tunnel_type, vrf_handle=self.uvrf,
                tunnel_handle=self.tunnel, termination_type = self.term_type, dst_ip=self.tunnel_endpoint1,
                src_ip=self.tunnel_lb_ip)
        self.tunnel_term5 = self.add_tunnel_term(self.device, type=self.tunnel_type, vrf_handle=self.uvrf,
                tunnel_handle=self.tunnel, termination_type = self.term_type, dst_ip=self.tunnel_endpoint2,
                src_ip=self.tunnel_lb_ip)
        self.tunnel_term6 = self.add_tunnel_term(self.device, type=self.tunnel_type, vrf_handle=self.uvrf,
                tunnel_handle=self.tunnel, termination_type = self.term_type, dst_ip=self.tunnel_endpoint3,
                src_ip=self.tunnel_lb_ip)

        # Create tunnel nexthop entry to tunnel endpoint 1 & 2
        self.inner_dmac1 = '00:11:11:11:11:01'
        self.inner_dmac2 = '00:11:11:11:11:02'
        self.inner_dmac3 = '00:11:11:11:11:03'
        self.tunnel_nexthop1 = self.add_nexthop(self.device, type=SWITCH_NEXTHOP_ATTR_TYPE_TUNNEL,
                rw_type=SWITCH_NEXTHOP_ATTR_RW_TYPE_L3, handle=self.tunnel, dest_ip=self.tunnel_endpoint1,
                mac_address=self.inner_dmac1)
        self.tunnel_nexthop2 = self.add_nexthop(self.device, type=SWITCH_NEXTHOP_ATTR_TYPE_TUNNEL,
                rw_type=SWITCH_NEXTHOP_ATTR_RW_TYPE_L3, handle=self.tunnel, dest_ip=self.tunnel_endpoint2,
                mac_address=self.inner_dmac2)
        self.tunnel_nexthop3 = self.add_nexthop(self.device, type=SWITCH_NEXTHOP_ATTR_TYPE_TUNNEL,
                rw_type=SWITCH_NEXTHOP_ATTR_RW_TYPE_L3, handle=self.tunnel, dest_ip=self.tunnel_endpoint3,
                mac_address=self.inner_dmac3)

        # Create Overlay ECMP with two tunnel nexthops
        self.overlay_ecmp = self.add_ecmp(self.device)
        self.overlay_ecmp_member1 = self.add_ecmp_member(self.device, nexthop_handle=self.tunnel_nexthop2,
                ecmp_handle=self.overlay_ecmp)
        self.overlay_ecmp_member2 = self.add_ecmp_member(self.device, nexthop_handle=self.tunnel_nexthop3,
                ecmp_handle=self.overlay_ecmp)

        #Create Customer & VM routes pointing to Tunnel nexthop
        self.customer_v4_route = self.add_route(self.device, ip_prefix=v4_customer_ip_start+'/25', vrf_handle=self.ovrf, nexthop_handle =
                self.tunnel_nexthop1)
        self.vm_v4_route = self.add_route(self.device, ip_prefix=v4_vm_ip_start+'/25', vrf_handle=self.ovrf, nexthop_handle =
                self.overlay_ecmp)
        self.customer_v6_route = self.add_route(self.device, ip_prefix=v6_customer_ip_start+'/60', vrf_handle=self.ovrf, nexthop_handle =
                self.tunnel_nexthop1)
        self.vm_v6_route = self.add_route(self.device, ip_prefix=v6_vm_ip_start+'/60', vrf_handle=self.ovrf, nexthop_handle =
                self.overlay_ecmp)

        self.urifs = []
        for port in self.ports:
            self.urifs.append(self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=port,
                vrf_handle=self.uvrf, src_mac=self.rmac))

        # Create route to tunnel endpoints
        # Create regular L3 nexthop for tunnel endpoint 1
        #self.underlay_neighbor0_mac = '00:33:33:33:33:01'
        self.underlay_neighbor0_mac = '00:77:66:55:44:33'
        #self.urif[0] = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.ports[0],
        #        vrf_handle=self.uvrf, src_mac=self.rmac)
        self.unhop0 = self.add_nexthop(self.device, handle=self.urifs[0], dest_ip=self.tunnel_endpoint1)
        self.uneighbor0 = self.add_neighbor(self.device, mac_address=self.underlay_neighbor0_mac,
            handle=self.urifs[0], dest_ip=self.tunnel_endpoint1)
        self.tunnel_route0 = self.add_route(self.device, ip_prefix=self.tunnel_endpoint1,
            vrf_handle=self.uvrf, nexthop_handle=self.unhop0)

        # For tunnel endpoint 2 & 3 we use an ECMP nexthop with 3 members (Port1, Port2, Port3)
        self.underlay_ecmp1 = self.add_ecmp(self.device)

        self.underlay_neighbor1_mac = '00:77:66:55:44:33'
        self.underlay_nhop1_ip = '11.11.11.2'
        #self.urif1 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.ports[1],
        #        vrf_handle=self.uvrf, src_mac=self.rmac)
        self.unhop1 = self.add_nexthop(self.device, handle=self.urifs[1], dest_ip=self.underlay_nhop1_ip)
        self.uneighbor1 = self.add_neighbor(self.device, mac_address=self.underlay_neighbor1_mac,
            handle=self.urifs[1], dest_ip=self.underlay_nhop1_ip)
        self.underlay_ecmp1_member1 = self.add_ecmp_member(self.device, ecmp_handle = self.underlay_ecmp1, nexthop_handle = self.unhop1)

        self.underlay_neighbor2_mac = '00:77:66:55:44:33'
        self.underlay_nhop2_ip = '11.11.11.3'
        #self.urif2 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.ports[2],
        #        vrf_handle=self.uvrf, src_mac=self.rmac)
        self.unhop2 = self.add_nexthop(self.device, handle=self.urifs[2], dest_ip=self.underlay_nhop2_ip)
        self.uneighbor2 = self.add_neighbor(self.device, mac_address=self.underlay_neighbor2_mac,
            handle=self.urifs[2], dest_ip=self.underlay_nhop2_ip)
        self.underlay_ecmp1_member2 = self.add_ecmp_member(self.device, ecmp_handle = self.underlay_ecmp1, nexthop_handle = self.unhop2)

        self.underlay_neighbor3_mac = '00:77:66:55:44:33'
        self.underlay_nhop3_ip = '11.11.11.4'
        #self.urif3 = self.add_rif(self.device, type=SWITCH_RIF_ATTR_TYPE_PORT, port_handle=self.ports[3],
        #        vrf_handle=self.uvrf, src_mac=self.rmac)
        self.unhop3 = self.add_nexthop(self.device, handle=self.urifs[3], dest_ip=self.underlay_nhop3_ip)
        self.uneighbor3 = self.add_neighbor(self.device, mac_address=self.underlay_neighbor3_mac,
            handle=self.urifs[3], dest_ip=self.underlay_nhop3_ip)
        self.underlay_ecmp1_member3 = self.add_ecmp_member(self.device, ecmp_handle = self.underlay_ecmp1, nexthop_handle = self.unhop3)

        self.tunnel_route1 = self.add_route(self.device, ip_prefix=self.tunnel_endpoint2,
            vrf_handle=self.uvrf, nexthop_handle=self.underlay_ecmp1)
        self.tunnel_route1 = self.add_route(self.device, ip_prefix=self.tunnel_endpoint3,
            vrf_handle=self.uvrf, nexthop_handle=self.underlay_ecmp1)

        # Configure ACLs
        # Configure Ingress ACLs
        # Step 1: Create ingress ACL group
        self.ingress_acl_group = self.add_acl_group(self.device, bind_point_type=[acl_group_bp_port])
        # Step 2: Create ACL tables (IPv4, IPv6, Mirror) and add to above group
        acl_direction = SWITCH_ACL_TABLE_ATTR_DIRECTION_INGRESS
        self.ipv4_acl_table = self.add_acl_table(self.device, type=SWITCH_ACL_TABLE_ATTR_TYPE_IPV4, bind_point_type =
                [acl_table_bp_port], direction = acl_direction)
        self.ipv6_acl_table = self.add_acl_table(self.device, type=SWITCH_ACL_TABLE_ATTR_TYPE_IPV6, bind_point_type =
                [acl_table_bp_port], direction = acl_direction)
        self.mirror_acl_table = self.add_acl_table(self.device, type=SWITCH_ACL_TABLE_ATTR_TYPE_IP_MIRROR, bind_point_type =
                [acl_table_bp_port], direction = acl_direction)
        # Step 3: Add ACL tables to ingress acl group
        self.acl_grp_member1 = self.add_acl_group_member(self.device, acl_table_handle=self.ipv4_acl_table,
                acl_group_handle=self.ingress_acl_group)
        self.acl_grp_member2 = self.add_acl_group_member(self.device, acl_table_handle=self.ipv6_acl_table,
                acl_group_handle=self.ingress_acl_group)
        self.acl_grp_member3 = self.add_acl_group_member(self.device, acl_table_handle=self.mirror_acl_table,
                acl_group_handle=self.ingress_acl_group)
        # Step 4: Create Mirror session to be used as destination for Mirror ACL action
        self.mirror = self.add_mirror(self.device, type=SWITCH_MIRROR_ATTR_TYPE_LOCAL,
                direction=SWITCH_MIRROR_ATTR_DIRECTION_INGRESS, egress_port_handle=self.ports[-1])
        # Step 5: Add acl entries to ACL tables
        for ip_addr in self.v4_customer_ips:
            self.add_acl_entry(self.device, src_ip=ip_addr, src_ip_mask='255.255.255.255',
                    packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_PERMIT, table_handle=self.ipv4_acl_table)
        for ip_addr in self.v6_customer_ips:
            self.add_acl_entry(self.device, src_ip=ip_addr, src_ip_mask='FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF',
                    packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_PERMIT, table_handle=self.ipv6_acl_table)
        for ip_addr in self.v4_vm_ips:
            self.add_acl_entry(self.device, dst_ip=ip_addr, dst_ip_mask='255.255.255.255',
                    action_ingress_mirror_handle=self.mirror, table_handle=self.mirror_acl_table)
        for ip_addr in self.v6_vm_ips:
            self.add_acl_entry(self.device, dst_ip=ip_addr, dst_ip_mask='FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF',
                    action_ingress_mirror_handle=self.mirror, table_handle=self.mirror_acl_table)
        # Step 6: Attach ingress acl group to port0
        for port in self.ports:
            self.attribute_set(port, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, self.ingress_acl_group)
        # Configure Egress ACLs
        # Step 1: Create ingress ACL group
        self.egress_acl_group = self.add_acl_group(self.device, bind_point_type=[acl_group_bp_port])
        # Step 2: Create ACL tables (IPv4, IPv6) and add to above group
        acl_direction = SWITCH_ACL_TABLE_ATTR_DIRECTION_EGRESS
        self.eg_ipv4_acl_table = self.add_acl_table(self.device, type=SWITCH_ACL_TABLE_ATTR_TYPE_IPV4, bind_point_type =
                [acl_table_bp_port], direction = acl_direction)
        self.eg_ipv6_acl_table = self.add_acl_table(self.device, type=SWITCH_ACL_TABLE_ATTR_TYPE_IPV6, bind_point_type =
                [acl_table_bp_port], direction = acl_direction)
        # Step 3: Add ACL tables to ingress acl group
        self.eg_acl_grp_member1 = self.add_acl_group_member(self.device, acl_table_handle=self.eg_ipv4_acl_table,
                acl_group_handle=self.egress_acl_group)
        self.eg_acl_grp_member2 = self.add_acl_group_member(self.device, acl_table_handle=self.eg_ipv6_acl_table,
                acl_group_handle=self.egress_acl_group)
        # Step 4: Add acl entries to ACL tables
        for ip_addr in self.v4_customer_ips:
            self.add_acl_entry(self.device, src_ip=ip_addr, src_ip_mask='255.255.255.255',
                    packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_PERMIT, table_handle=self.eg_ipv4_acl_table)
        for ip_addr in self.v6_customer_ips:
            self.add_acl_entry(self.device, src_ip=ip_addr, src_ip_mask='FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF',
                    packet_action=SWITCH_ACL_ENTRY_ATTR_PACKET_ACTION_PERMIT, table_handle=self.eg_ipv6_acl_table)
        # Step 5: Attach ingress acl group to port 1, 2 and 3
        for port in self.ports:
            self.attribute_set(port, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, self.egress_acl_group)

    def runTest(self):
        if(self.client.is_feature_enable(SWITCH_FEATURE_IPV4_TUNNEL)==0):
            return
        self.test_configure()
        try:
            self.IPinIPv4Test()
        finally:
            pass

    def tearDown(self):
        for port in self.ports:
            self.attribute_set(port, SWITCH_PORT_ATTR_INGRESS_ACL_HANDLE, 0)
            self.attribute_set(port, SWITCH_PORT_ATTR_EGRESS_ACL_HANDLE, 0)
        self.cleanup()

    def IPinIPv4Test(self):
        print("IPinIPv4Test()")
        try:
            if not self.client.is_feature_enable(SWITCH_FEATURE_TUNNEL_ENCAP):
                print("Tunnel encap not enabled, skpping")
            else:
                #for v4 in self.v4_customer_ips:
                #    print("customer ip {}".format(v4))
                #for v6 in self.v6_customer_ips:
                #    print("customer ip {}".format(v6))
                #for v4 in self.v4_vm_ips:
                #    print("customer ip {}".format(v4))
                #for v6 in self.v6_vm_ips:
                #    print("customer ip {}".format(v6))
                for src_addr, dst_addr in zip(self.v4_customer_ips, self.v4_vm_ips):
                    v4_pkt_in = simple_tcp_packet(eth_dst='00:77:66:55:44:33', eth_src=self.inner_dmac1, ip_dst=dst_addr,
                            ip_src=src_addr, ip_id=108,ip_ttl=64)
                    ip4ip4_pkt_in = simple_ipv4ip_packet(eth_dst='00:77:66:55:44:33', eth_src=self.underlay_neighbor0_mac,
                            ip_id=0, ip_src=self.tunnel_endpoint1, ip_dst=self.tunnel_lb_ip, ip_ttl=64, inner_frame=v4_pkt_in['IP'])
                    pkt_out = simple_tcp_packet(eth_dst=self.inner_dmac2, eth_src=self.default_rmac, ip_dst=dst_addr,
                            ip_src=src_addr, ip_id=108, ip_ttl=63)
                    pkt_out2 = simple_tcp_packet(eth_dst=self.inner_dmac3, eth_src=self.default_rmac, ip_dst=dst_addr,
                            ip_src=src_addr, ip_id=108, ip_ttl=63)
                    # Egress Packets one for each ECMP Member
                    ip4ip4_pkt_out1 = simple_ipv4ip_packet(eth_dst=self.underlay_neighbor1_mac, eth_src='00:77:66:55:44:33',
                        ip_id=0, ip_src=self.tunnel_lb_ip, ip_dst=self.tunnel_endpoint2, ip_ttl=64,
                        inner_frame=pkt_out['IP'])
                    ip4ip4_pkt_out2 = simple_ipv4ip_packet(eth_dst=self.underlay_neighbor2_mac, eth_src='00:77:66:55:44:33',
                        ip_id=0, ip_src=self.tunnel_lb_ip, ip_dst=self.tunnel_endpoint2, ip_ttl=64,
                        inner_frame=pkt_out['IP'])
                    ip4ip4_pkt_out3 = simple_ipv4ip_packet(eth_dst=self.underlay_neighbor3_mac, eth_src='00:77:66:55:44:33',
                        ip_id=0, ip_src=self.tunnel_lb_ip, ip_dst=self.tunnel_endpoint2, ip_ttl=64,
                        inner_frame=pkt_out['IP'])
                    ip4ip4_pkt_out4 = simple_ipv4ip_packet(eth_dst=self.underlay_neighbor1_mac, eth_src='00:77:66:55:44:33',
                        ip_id=0, ip_src=self.tunnel_lb_ip, ip_dst=self.tunnel_endpoint3, ip_ttl=64,
                        inner_frame=pkt_out2['IP'])
                    ip4ip4_pkt_out5 = simple_ipv4ip_packet(eth_dst=self.underlay_neighbor2_mac, eth_src='00:77:66:55:44:33',
                        ip_id=0, ip_src=self.tunnel_lb_ip, ip_dst=self.tunnel_endpoint3, ip_ttl=64,
                        inner_frame=pkt_out2['IP'])
                    ip4ip4_pkt_out6 = simple_ipv4ip_packet(eth_dst=self.underlay_neighbor3_mac, eth_src='00:77:66:55:44:33',
                        ip_id=0, ip_src=self.tunnel_lb_ip, ip_dst=self.tunnel_endpoint3, ip_ttl=64,
                        inner_frame=pkt_out2['IP'])
                    send_packet(self, self.devports[0], ip4ip4_pkt_in)
                    #Uncomment this for packet verification.
                    #Commented due to presence of snake table at the end of test pipeline
                    #verify_any_packet_on_ports_list(
                    #                        self, [ip4ip4_pkt_out1, ip4ip4_pkt_out2, ip4ip4_pkt_out3, ip4ip4_pkt_out4,
                    #                            ip4ip4_pkt_out5, ip4ip4_pkt_out6],
                    #                        [[self.devports[1], self.devports[2], self.devports[3]]],
                    #                        timeout=2)
                for src_addr, dst_addr in zip(self.v6_customer_ips, self.v6_vm_ips):
                    v6_pkt_in = simple_tcpv6_packet(eth_dst='00:77:66:55:44:33', eth_src=self.inner_dmac1,
                            ipv6_dst=dst_addr,
                            ipv6_src=src_addr, ipv6_hlim=64)
                    ip4ip6_pkt_in = simple_ipv4ip_packet(eth_dst='00:77:66:55:44:33', eth_src=self.underlay_neighbor0_mac,
                            ip_id=0, ip_src=self.tunnel_endpoint1, ip_dst=self.tunnel_lb_ip, ip_ttl=64,
                            inner_frame=v6_pkt_in['IPv6'])
                    pkt_out = simple_tcpv6_packet(eth_dst=self.inner_dmac2, eth_src=self.default_rmac, ipv6_dst=dst_addr,
                            ipv6_src=src_addr, ipv6_hlim=63)
                    pkt_out2 = simple_tcpv6_packet(eth_dst=self.inner_dmac3, eth_src=self.default_rmac, ipv6_dst=dst_addr,
                            ipv6_src=src_addr, ipv6_hlim=63)
                    # Egress Packets one for each ECMP Member
                    ip4ip6_pkt_out1 = simple_ipv4ip_packet(eth_dst=self.underlay_neighbor1_mac, eth_src='00:77:66:55:44:33',
                        ip_id=0, ip_src=self.tunnel_lb_ip, ip_dst=self.tunnel_endpoint2, ip_ttl=64,
                        inner_frame=pkt_out['IPv6'])
                    ip4ip6_pkt_out2 = simple_ipv4ip_packet(eth_dst=self.underlay_neighbor2_mac, eth_src='00:77:66:55:44:33',
                        ip_id=0, ip_src=self.tunnel_lb_ip, ip_dst=self.tunnel_endpoint2, ip_ttl=64,
                        inner_frame=pkt_out['IPv6'])
                    ip4ip6_pkt_out3 = simple_ipv4ip_packet(eth_dst=self.underlay_neighbor3_mac, eth_src='00:77:66:55:44:33',
                        ip_id=0, ip_src=self.tunnel_lb_ip, ip_dst=self.tunnel_endpoint2, ip_ttl=64,
                        inner_frame=pkt_out['IPv6'])
                    ip4ip6_pkt_out4 = simple_ipv4ip_packet(eth_dst=self.underlay_neighbor1_mac, eth_src='00:77:66:55:44:33',
                        ip_id=0, ip_src=self.tunnel_lb_ip, ip_dst=self.tunnel_endpoint3, ip_ttl=64,
                        inner_frame=pkt_out2['IPv6'])
                    ip4ip6_pkt_out5 = simple_ipv4ip_packet(eth_dst=self.underlay_neighbor2_mac, eth_src='00:77:66:55:44:33',
                        ip_id=0, ip_src=self.tunnel_lb_ip, ip_dst=self.tunnel_endpoint3, ip_ttl=64,
                        inner_frame=pkt_out2['IPv6'])
                    ip4ip6_pkt_out6 = simple_ipv4ip_packet(eth_dst=self.underlay_neighbor3_mac, eth_src='00:77:66:55:44:33',
                        ip_id=0, ip_src=self.tunnel_lb_ip, ip_dst=self.tunnel_endpoint3, ip_ttl=64,
                        inner_frame=pkt_out2['IPv6'])
                    send_packet(self, self.devports[0], ip4ip6_pkt_in)
                    #Uncomment this for packet verification.
                    #Commented due to presence of snake table at the end of test pipeline
                    #verify_any_packet_on_ports_list(
                    #                        self, [ip4ip6_pkt_out1, ip4ip6_pkt_out2, ip4ip6_pkt_out3, ip4ip6_pkt_out4,
                    #                            ip4ip6_pkt_out5, ip4ip6_pkt_out6],
                    #                        [[self.devports[1], self.devports[2], self.devports[3]]],
                    #                        timeout=2)
        finally:
            pass
