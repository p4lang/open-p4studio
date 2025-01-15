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
Thrift SAI interface Multicast extensions tests
"""

import os
import sys

THIS_DIR = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(THIS_DIR, '..'))

from sai_base_test import *


class IPMCSVIMembersTest(SaiHelperBase):
    '''
    Test that verifies IPMC group with members that are RIFs of type VLAN.
    For VLAN 10 RIF IPMC member there is an L2MC group specified - multicast
    packets should be forwarded to members of this group.
    For VLAN 20 RIF IPMC member multicast packets should be forwarded
    to all VLAN members.
    '''
    def setUp(self):
        super(IPMCSVIMembersTest, self).setUp()

        self.default_rmac = "00:77:66:55:44:00"
        self.dst_mac = "01:00:5e:01:01:05"
        self.src_mac = "00:22:22:22:22:22"
        self.grp_ip = "230.1.1.5"
        self.src_ip = "10.0.10.5"

        self.vlan10_ports = [self.port0, self.port1, self.port2, self.port3,
                             self.port4]
        self.vlan20_ports = [self.port5, self.port6, self.port7]
        self.bridge_ports = []
        self.vlan_members = []

        self.vlan10 = sai_thrift_create_vlan(self.client, vlan_id=10)
        for port in self.vlan10_ports:
            bp = sai_thrift_create_bridge_port(
                self.client,
                bridge_id=self.default_1q_bridge,
                port_id=port,
                type=SAI_BRIDGE_PORT_TYPE_PORT,
                admin_state=True)
            self.bridge_ports.append(bp)
            self.vlan_members.append(
                sai_thrift_create_vlan_member(
                    self.client,
                    vlan_id=self.vlan10,
                    bridge_port_id=bp,
                    vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED))
            sai_thrift_set_port_attribute(self.client, port, port_vlan_id=10)

        self.vlan10_rif = sai_thrift_create_router_interface(
            self.client,
            type=SAI_ROUTER_INTERFACE_TYPE_VLAN,
            virtual_router_id=self.default_vrf,
            vlan_id=self.vlan10)

        self.vlan20 = sai_thrift_create_vlan(self.client, vlan_id=20)
        for port in self.vlan20_ports:
            bp = sai_thrift_create_bridge_port(
                self.client,
                bridge_id=self.default_1q_bridge,
                port_id=port,
                type=SAI_BRIDGE_PORT_TYPE_PORT,
                admin_state=True)
            self.bridge_ports.append(bp)
            self.vlan_members.append(
                sai_thrift_create_vlan_member(
                    self.client,
                    vlan_id=self.vlan20,
                    bridge_port_id=bp,
                    vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED))
            sai_thrift_set_port_attribute(self.client, port, port_vlan_id=20)

        self.vlan20_rif = sai_thrift_create_router_interface(
            self.client,
            type=SAI_ROUTER_INTERFACE_TYPE_VLAN,
            virtual_router_id=self.default_vrf,
            vlan_id=self.vlan20)

        # configure L2MC with VLAN 10 members: port0, port1, port2
        self.l2mc_grp = sai_thrift_create_l2mc_group(self.client)
        self.l2mc_grp_mbrs = []
        for i in range(0, 3):
            self.l2mc_grp_mbrs.append(
                sai_thrift_create_l2mc_group_member(
                    self.client,
                    l2mc_group_id=self.l2mc_grp,
                    l2mc_output_id=self.bridge_ports[i]))

        # configure RIFs
        self.rif8 = sai_thrift_create_router_interface(
            self.client,
            type=SAI_ROUTER_INTERFACE_TYPE_PORT,
            virtual_router_id=self.default_vrf,
            src_mac_address=self.default_rmac,
            v4_mcast_enable=True,
            port_id=self.port8)

        self.rif9 = sai_thrift_create_router_interface(
            self.client,
            type=SAI_ROUTER_INTERFACE_TYPE_PORT,
            virtual_router_id=self.default_vrf,
            src_mac_address=self.default_rmac,
            v4_mcast_enable=True,
            port_id=self.port9)

        # configure IPMC group
        self.ipmc_grp = sai_thrift_create_ipmc_group(self.client)

        self.ipmc_mbr0 = sai_thrift_create_ipmc_group_member(
            self.client,
            ipmc_group_id=self.ipmc_grp, ipmc_output_id=self.rif8)
        self.ipmc_mbr1 = sai_thrift_create_ipmc_group_member(
            self.client,
            ipmc_group_id=self.ipmc_grp, ipmc_output_id=self.rif9)
        self.ipmc_mbr2 = sai_thrift_create_ipmc_group_member(
            self.client,
            ipmc_group_id=self.ipmc_grp, ipmc_output_id=self.vlan20_rif)

        # prepare extensions_l2mc_output_id attribute
        attr_value = sai_thrift_attribute_value_t(oid=self.l2mc_grp)
        l2mc_output = sai_thrift_attribute_t(
            id=(SAI_IPMC_GROUP_MEMBER_ATTR_END),  # from saiipmcgroupextensions.h
            value=attr_value)

        self.ipmc_mbr3 = sai_thrift_create_ipmc_group_member(
            self.client,
            ipmc_group_id=self.ipmc_grp, ipmc_output_id=self.vlan10_rif,
            custom_attribute=l2mc_output)

        self.rpf_grp = sai_thrift_create_rpf_group(self.client)
        self.rpf_mbr = sai_thrift_create_rpf_group_member(
            self.client,
            rpf_group_id=self.rpf_grp, rpf_interface_id=self.rif8)

        self.ipmc_route = sai_thrift_ipmc_entry_t(
            vr_id=self.default_vrf,
            type=SAI_IPMC_ENTRY_TYPE_XG,
            destination=sai_ipaddress(self.grp_ip))
        sai_thrift_create_ipmc_entry(
            self.client,
            self.ipmc_route,
            rpf_group_id=self.rpf_grp,
            output_group_id=self.ipmc_grp)

    def runTest(self):
        try:
            self.sviMcastMembersTest()
        finally:
            pass

    def tearDown(self):
        sai_thrift_remove_ipmc_entry(self.client, self.ipmc_route)
        sai_thrift_remove_rpf_group_member(self.client, self.rpf_mbr)
        sai_thrift_remove_rpf_group(self.client, self.rpf_grp)

        sai_thrift_remove_ipmc_group_member(self.client, self.ipmc_mbr3)
        sai_thrift_remove_ipmc_group_member(self.client, self.ipmc_mbr2)
        sai_thrift_remove_ipmc_group_member(self.client, self.ipmc_mbr1)
        sai_thrift_remove_ipmc_group_member(self.client, self.ipmc_mbr0)
        sai_thrift_remove_ipmc_group(self.client, self.ipmc_grp)

        sai_thrift_remove_router_interface(self.client, self.rif9)
        sai_thrift_remove_router_interface(self.client, self.rif8)

        for member in self.l2mc_grp_mbrs:
            sai_thrift_remove_l2mc_group_member(self.client, member)
        sai_thrift_remove_l2mc_group(self.client, self.l2mc_grp)

        sai_thrift_remove_router_interface(self.client, self.vlan20_rif)
        sai_thrift_remove_router_interface(self.client, self.vlan10_rif)

        for member in self.vlan_members:
            sai_thrift_remove_vlan_member(self.client, member)

        for bp in self.bridge_ports:
            sai_thrift_remove_bridge_port(self.client, bp)

        for port in self.vlan10_ports:
            sai_thrift_set_port_attribute(self.client, port, port_vlan_id=0)

        for port in self.vlan20_ports:
            sai_thrift_set_port_attribute(self.client, port, port_vlan_id=0)

        sai_thrift_remove_vlan(self.client, self.vlan20)
        sai_thrift_remove_vlan(self.client, self.vlan10)

        super(IPMCSVIMembersTest, self).tearDown()

    def sviMcastMembersTest(self):
        '''
        Exercice multicast group with members that are RIFs of type VLAN
        '''
        print("\nsviMcastMembersTest()")

        pkt = simple_udp_packet(
            eth_dst=self.dst_mac,
            eth_src=self.src_mac,
            ip_src=self.src_ip,
            ip_dst=self.grp_ip,
            ip_ttl=64)
        exp_pkt = simple_udp_packet(
            eth_dst=self.dst_mac,
            eth_src=self.default_rmac,
            ip_src=self.src_ip,
            ip_dst=self.grp_ip,
            ip_ttl=63)

        print("Sending packet to multicast group with SVI members")
        send_packet(self, self.dev_port8, pkt)
        verify_packets(
            self, exp_pkt,
            # VLAN 10 ports being members of L2MC group
            [self.dev_port0, self.dev_port1, self.dev_port2,
             # all VLAN 20 ports
             self.dev_port5, self.dev_port6, self.dev_port7,
             # regular IPMC group port
             self.dev_port9])
        verify_no_other_packets(self)
        print("\tOK")


@group('tunnel')
class IPMCSVIMembersTunnelTest(IPMCSVIMembersTest):
    '''
    This class extends IPMCSVIMembersTest with additional VLAN members of type
    tunnel. Tunnels are created on port 10 and 11. Tunnel 1 is a member of both:
    VLAN 10 and VLAN 20, tunnel 2 is only a member of VLAN 10.
    '''
    def setUp(self):
        super(IPMCSVIMembersTunnelTest, self).setUp()

        self.vni1 = 1000
        self.vni2 = 2000
        self.tun1_ip = "10.0.0.31"
        self.tun2_ip = "10.0.0.65"
        self.lpb_ip = "10.1.0.32"
        self.src_ip = "192.168.100.1"
        self.src_mac = "00:12:34:56:78:90"
        self.unbor1_mac = "00:11:11:11:11:11"
        self.unbor2_mac = "00:22:22:22:22:22"

        ## tunnels configuration
        self.uport1_dev = self.dev_port10
        self.uport1_rif = sai_thrift_create_router_interface(
            self.client,
            type=SAI_ROUTER_INTERFACE_TYPE_PORT,
            virtual_router_id=self.default_vrf,
            port_id=self.port10)
        self.uport2_dev = self.dev_port11
        self.uport2_rif = sai_thrift_create_router_interface(
            self.client,
            type=SAI_ROUTER_INTERFACE_TYPE_PORT,
            virtual_router_id=self.default_vrf,
            port_id=self.port11)

        # underlay loopback rif for tunnels
        self.urif_lpb = sai_thrift_create_router_interface(
            self.client,
            type=SAI_ROUTER_INTERFACE_TYPE_LOOPBACK,
            virtual_router_id=self.default_vrf,
            mtu=9100)

        # underlay neighbors
        self.unbor1 = sai_thrift_neighbor_entry_t(
            ip_address=sai_ipaddress(self.tun1_ip),
            rif_id=self.uport1_rif,
            switch_id=self.switch_id)
        sai_thrift_create_neighbor_entry(
            self.client, self.unbor1, dst_mac_address=self.unbor1_mac)

        self.unbor2 = sai_thrift_neighbor_entry_t(
            ip_address=sai_ipaddress(self.tun2_ip),
            rif_id=self.uport2_rif,
            switch_id=self.switch_id)
        sai_thrift_create_neighbor_entry(
            self.client, self.unbor2, dst_mac_address=self.unbor2_mac)

        # underlay nexthops
        self.unhop1 = sai_thrift_create_next_hop(
            self.client,
            type=SAI_NEXT_HOP_TYPE_IP,
            ip=sai_ipaddress(self.tun1_ip),
            router_interface_id=self.uport1_rif)

        self.unhop2 = sai_thrift_create_next_hop(
            self.client,
            type=SAI_NEXT_HOP_TYPE_IP,
            ip=sai_ipaddress(self.tun2_ip),
            router_interface_id=self.uport2_rif)

        self.tunnel_type = SAI_TUNNEL_TYPE_VXLAN

        # create tunnel maps
        self.decap_tunnel_map_vlan = sai_thrift_create_tunnel_map(
            self.client, type=SAI_TUNNEL_MAP_TYPE_VNI_TO_VLAN_ID)

        self.encap_tunnel_map_vlan = sai_thrift_create_tunnel_map(
            self.client, type=SAI_TUNNEL_MAP_TYPE_VLAN_ID_TO_VNI)

        # create tunnel
        self.decap_maps = sai_thrift_object_list_t(
            count=1, idlist=[self.decap_tunnel_map_vlan])
        self.encap_maps = sai_thrift_object_list_t(
            count=1, idlist=[self.encap_tunnel_map_vlan])

        self.p2mp_tunnel = sai_thrift_create_tunnel(
            self.client,
            type=self.tunnel_type,
            underlay_interface=self.urif_lpb,
            decap_mappers=self.decap_maps,
            encap_mappers=self.encap_maps,
            encap_src_ip=sai_ipaddress(self.lpb_ip),
            peer_mode=SAI_TUNNEL_PEER_MODE_P2MP)

        self.tunnel_term = sai_thrift_create_tunnel_term_table_entry(
            self.client,
            type=SAI_TUNNEL_TERM_TABLE_ENTRY_TYPE_P2MP,
            vr_id=self.default_vrf,
            dst_ip=sai_ipaddress(self.lpb_ip),
            tunnel_type=self.tunnel_type,
            action_tunnel_id=self.p2mp_tunnel)

        # create tunnel map entries for vlans
        self.decap_tunnel_map_entry_vlan10 = sai_thrift_create_tunnel_map_entry(
            self.client,
            tunnel_map_type=SAI_TUNNEL_MAP_TYPE_VNI_TO_VLAN_ID,
            tunnel_map=self.decap_tunnel_map_vlan,
            vlan_id_value=10,
            vni_id_key=self.vni1)

        self.encap_tunnel_map_entry_vlan10 = sai_thrift_create_tunnel_map_entry(
            self.client,
            tunnel_map_type=SAI_TUNNEL_MAP_TYPE_VLAN_ID_TO_VNI,
            tunnel_map=self.encap_tunnel_map_vlan,
            vlan_id_key=10,
            vni_id_value=self.vni1)

        self.decap_tunnel_map_entry_vlan20 = sai_thrift_create_tunnel_map_entry(
            self.client,
            tunnel_map_type=SAI_TUNNEL_MAP_TYPE_VNI_TO_VLAN_ID,
            tunnel_map=self.decap_tunnel_map_vlan,
            vlan_id_value=20,
            vni_id_key=self.vni2)

        self.encap_tunnel_map_entry_vlan20 = sai_thrift_create_tunnel_map_entry(
            self.client,
            tunnel_map_type=SAI_TUNNEL_MAP_TYPE_VLAN_ID_TO_VNI,
            tunnel_map=self.encap_tunnel_map_vlan,
            vlan_id_key=20,
            vni_id_value=self.vni2)

        self.p2p_tunnel1 = sai_thrift_create_tunnel(
            self.client,
            type=self.tunnel_type,
            underlay_interface=self.urif_lpb,
            decap_mappers=self.decap_maps,
            encap_mappers=self.encap_maps,
            encap_src_ip=sai_ipaddress(self.lpb_ip),
            peer_mode=SAI_TUNNEL_PEER_MODE_P2P,
            encap_dst_ip=sai_ipaddress(self.tun1_ip))

        self.p2p_tunnel1_bp = sai_thrift_create_bridge_port(
            self.client,
            type=SAI_BRIDGE_PORT_TYPE_TUNNEL,
            tunnel_id=self.p2p_tunnel1,
            bridge_id=self.default_1q_bridge,
            admin_state=True,
            fdb_learning_mode=SAI_BRIDGE_PORT_FDB_LEARNING_MODE_DISABLE)

        self.p2p_tunnel2 = sai_thrift_create_tunnel(
            self.client,
            type=self.tunnel_type,
            underlay_interface=self.urif_lpb,
            decap_mappers=self.decap_maps,
            encap_mappers=self.encap_maps,
            encap_src_ip=sai_ipaddress(self.lpb_ip),
            peer_mode=SAI_TUNNEL_PEER_MODE_P2P,
            encap_dst_ip=sai_ipaddress(self.tun2_ip))

        self.p2p_tunnel2_bp = sai_thrift_create_bridge_port(
            self.client,
            type=SAI_BRIDGE_PORT_TYPE_TUNNEL,
            tunnel_id=self.p2p_tunnel2,
            bridge_id=self.default_1q_bridge,
            admin_state=True,
            fdb_learning_mode=SAI_BRIDGE_PORT_FDB_LEARNING_MODE_DISABLE)

        # vlan_members created using tunnel bridge_ports
        self.p2p_tunnel1_vlan10_mbr = sai_thrift_create_vlan_member(
            self.client,
            vlan_id=self.vlan10,
            bridge_port_id=self.p2p_tunnel1_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)

        self.p2p_tunnel2_vlan10_mbr = sai_thrift_create_vlan_member(
            self.client,
            vlan_id=self.vlan10,
            bridge_port_id=self.p2p_tunnel2_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)

        self.p2p_tunnel_vlan20_mbr = sai_thrift_create_vlan_member(
            self.client,
            vlan_id=self.vlan20,
            bridge_port_id=self.p2p_tunnel1_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)

    def runTest(self):
        try:
            self.sviMcastMembersTunnelTest()
            self.multipleSvisMcastMembersTunnelTest()
        finally:
            pass

    def tearDown(self):
        sai_thrift_flush_fdb_entries(
            self.client, entry_type=SAI_FDB_FLUSH_ENTRY_TYPE_ALL)

        sai_thrift_remove_vlan_member(self.client, self.p2p_tunnel_vlan20_mbr)
        sai_thrift_remove_vlan_member(self.client, self.p2p_tunnel2_vlan10_mbr)
        sai_thrift_remove_vlan_member(self.client, self.p2p_tunnel1_vlan10_mbr)
        sai_thrift_remove_bridge_port(self.client, self.p2p_tunnel2_bp)
        sai_thrift_remove_tunnel(self.client, self.p2p_tunnel2)
        sai_thrift_remove_bridge_port(self.client, self.p2p_tunnel1_bp)
        sai_thrift_remove_tunnel(self.client, self.p2p_tunnel1)
        sai_thrift_remove_tunnel_map_entry(
            self.client, self.encap_tunnel_map_entry_vlan20)
        sai_thrift_remove_tunnel_map_entry(
            self.client, self.decap_tunnel_map_entry_vlan20)
        sai_thrift_remove_tunnel_map_entry(
            self.client, self.encap_tunnel_map_entry_vlan10)
        sai_thrift_remove_tunnel_map_entry(
            self.client, self.decap_tunnel_map_entry_vlan10)
        sai_thrift_remove_tunnel_term_table_entry(
            self.client, self.tunnel_term)
        sai_thrift_remove_tunnel(self.client, self.p2mp_tunnel)
        sai_thrift_remove_tunnel_map(self.client, self.encap_tunnel_map_vlan)
        sai_thrift_remove_tunnel_map(self.client, self.decap_tunnel_map_vlan)

        sai_thrift_remove_next_hop(self.client, self.unhop2)
        sai_thrift_remove_next_hop(self.client, self.unhop1)
        sai_thrift_remove_neighbor_entry(self.client, self.unbor2)
        sai_thrift_remove_neighbor_entry(self.client, self.unbor1)
        sai_thrift_remove_router_interface(self.client, self.urif_lpb)
        sai_thrift_remove_router_interface(self.client, self.uport2_rif)
        sai_thrift_remove_router_interface(self.client, self.uport1_rif)

        super(IPMCSVIMembersTunnelTest, self).tearDown()

    def sviMcastMembersTunnelTest(self):
        '''
        Exercise multicast group with members that are RIFs of type VLAN
        and the VLANs contain a members of type tunnel.
        In the test, VLAN 10 has l2mc_group attribute specified so packets are
        forwarded only to members of this group. For VLAN 20 the attribute is
        not specified so packets are forwarded to all members of this VLAN.
        The test verifies also addition and deletion of members of l2mc_group
        that corresponds to ipmc_group and the case when there are several
        tunnels in such group.
        '''
        print("\nsviMcastMembersTunnelTest()")

        pkt = simple_udp_packet(
            eth_dst=self.dst_mac,
            eth_src=self.src_mac,
            ip_src=self.src_ip,
            ip_dst=self.grp_ip,
            ip_ttl=64)
        exp_pkt = simple_udp_packet(
            eth_dst=self.dst_mac,
            eth_src=self.default_rmac,
            ip_src=self.src_ip,
            ip_dst=self.grp_ip,
            ip_ttl=63)
        vxlan_pkt10_1 = Mask(
            simple_vxlan_packet(eth_dst=self.unbor1_mac,
                                eth_src=ROUTER_MAC,
                                ip_dst=self.tun1_ip,
                                ip_src=self.lpb_ip,
                                ip_id=0,
                                ip_ttl=63,
                                ip_flags=0x2,
                                with_udp_chksum=False,
                                vxlan_vni=self.vni1,
                                inner_frame=exp_pkt))
        vxlan_pkt10_1.set_do_not_care_scapy(UDP, 'sport')
        vxlan_pkt10_2 = Mask(
            simple_vxlan_packet(eth_dst=self.unbor2_mac,
                                eth_src=ROUTER_MAC,
                                ip_dst=self.tun2_ip,
                                ip_src=self.lpb_ip,
                                ip_id=0,
                                ip_ttl=63,
                                ip_flags=0x2,
                                with_udp_chksum=False,
                                vxlan_vni=self.vni1,
                                inner_frame=exp_pkt))
        vxlan_pkt10_2.set_do_not_care_scapy(UDP, 'sport')
        vxlan_pkt20 = Mask(
            simple_vxlan_packet(eth_dst=self.unbor1_mac,
                                eth_src=ROUTER_MAC,
                                ip_dst=self.tun1_ip,
                                ip_src=self.lpb_ip,
                                ip_id=0,
                                ip_ttl=63,
                                ip_flags=0x2,
                                with_udp_chksum=False,
                                vxlan_vni=self.vni2,
                                inner_frame=exp_pkt))
        vxlan_pkt20.set_do_not_care_scapy(UDP, 'sport')

        # tunnel is a member of VLAN10 and VLAN 20 but it is not a member of
        # L2MC group corresponding to VLAN 10 RIF
        print("Sending packet to multicast group with SVI members with tunnel")
        send_packet(self, self.dev_port8, pkt)
        # tunnel as a member of VLAN 20
        verify_packet(self, vxlan_pkt20, self.uport1_dev)
        print("Packet was received on tunnel port of VLAN 20")
        verify_packets(
            self, exp_pkt,
            # VLAN 10 ports being members of L2MC group
            [self.dev_port0, self.dev_port1, self.dev_port2,
             # all VLAN 20 ports
             self.dev_port5, self.dev_port6, self.dev_port7,
             # regular IPMC group port
             self.dev_port9])
        verify_no_other_packets(self)
        print("\tOK")

        try:
            print("Adding tunnel to the L2MC group corresponding to VLAN 10 RIF")
            l2mc_tun1_mbr = sai_thrift_create_l2mc_group_member(
                self.client,
                l2mc_group_id=self.l2mc_grp,
                l2mc_output_id=self.p2p_tunnel1_bp)

            print("Sending packet to multicast group with SVI members with tunnel")
            send_packet(self, self.dev_port8, pkt)
            # tunnel as a member of VLAN 10
            verify_packet(self, vxlan_pkt10_1, self.uport1_dev)
            print("Packet was received on tunnel port of VLAN 10")
            # tunnel as a member of VLAN 20
            verify_packet(self, vxlan_pkt20, self.uport1_dev)
            print("Packet was received on tunnel port of VLAN 20")
            verify_packets(
                self, exp_pkt,
                # VLAN 10 ports being members of L2MC group
                [self.dev_port0, self.dev_port1, self.dev_port2,
                 # all VLAN 20 ports
                 self.dev_port5, self.dev_port6, self.dev_port7,
                 # regular IPMC group port
                 self.dev_port9])
            verify_no_other_packets(self)
            print("\tOK")

            print("Adding another tunnel to the L2MC group corresponding to VLAN 10 RIF")
            l2mc_tun2_mbr = sai_thrift_create_l2mc_group_member(
                self.client,
                l2mc_group_id=self.l2mc_grp,
                l2mc_output_id=self.p2p_tunnel2_bp)

            print("Sending packet to multicast group with SVI members with tunnels")
            send_packet(self, self.dev_port8, pkt)
            # tunnels as members of VLAN 10
            verify_packet(self, vxlan_pkt10_1, self.uport1_dev)
            verify_packet(self, vxlan_pkt10_2, self.uport2_dev)
            print("Packet was received on tunnel ports of VLAN 10")
            # tunnel as a members of VLAN 20
            verify_packet(self, vxlan_pkt20, self.uport1_dev)
            print("Packet was received on tunnel port of VLAN 20")
            verify_packets(
                self, exp_pkt,
                # VLAN 10 ports being members of L2MC group
                [self.dev_port0, self.dev_port1, self.dev_port2,
                 # all VLAN 20 ports
                 self.dev_port5, self.dev_port6, self.dev_port7,
                 # regular IPMC group port
                 self.dev_port9])
            verify_no_other_packets(self)
            print("\tOK")

            print("Removing one tunnel from the L2MC group corresponding to VLAN 10 RIF")
            sai_thrift_remove_l2mc_group_member(self.client, l2mc_tun1_mbr)

            print("Sending packet to multicast group with SVI members with tunnel")
            send_packet(self, self.dev_port8, pkt)
            # tunnel as a member of VLAN 10
            verify_packet(self, vxlan_pkt10_2, self.uport2_dev)
            print("Packet was received on tunnel port of VLAN 10")
            # tunnel as a member of VLAN 20
            verify_packet(self, vxlan_pkt20, self.uport1_dev)
            print("Packet was received on tunnel port of VLAN 20")
            verify_packets(
                self, exp_pkt,
                # VLAN 10 ports being members of L2MC group
                [self.dev_port0, self.dev_port1, self.dev_port2,
                 # all VLAN 20 ports
                 self.dev_port5, self.dev_port6, self.dev_port7,
                 # regular IPMC group port
                 self.dev_port9])
            verify_no_other_packets(self)
            print("\tOK")

        finally:
            sai_thrift_remove_l2mc_group_member(self.client, l2mc_tun2_mbr)

    def multipleSvisMcastMembersTunnelTest(self):
        '''
        Verify the case when IPMC group contains several SVI members and these
        members have l2mc_group attribute specified and there are tunnels in
        each of the l2mc_groups.
        To test this additional SVI is created with ports 15 and 16 and tunnel
        that points to a nexthop leading to port 17. An ipmc_member for this
        SVI has l2mc_group attribute specified and the assigned l2mc_group
        contains port 15 and tunnel only.
        '''
        print("\nmultipleSvisMcastMembersTunnelTest()")

        vni3 = 3000

        try:
            # create VLAN 30 with ports 15 and 16
            vlan30 = sai_thrift_create_vlan(self.client, vlan_id=30)
            port15_bp = sai_thrift_create_bridge_port(
                self.client,
                bridge_id=self.default_1q_bridge,
                port_id=self.port15,
                type=SAI_BRIDGE_PORT_TYPE_PORT,
                admin_state=True)
            port15_vlan_member = sai_thrift_create_vlan_member(
                self.client,
                vlan_id=vlan30,
                bridge_port_id=port15_bp,
                vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)
            sai_thrift_set_port_attribute(self.client, self.port15, port_vlan_id=30)

            port16_bp = sai_thrift_create_bridge_port(
                self.client,
                bridge_id=self.default_1q_bridge,
                port_id=self.port16,
                type=SAI_BRIDGE_PORT_TYPE_PORT,
                admin_state=True)
            port16_vlan_member = sai_thrift_create_vlan_member(
                self.client,
                vlan_id=vlan30,
                bridge_port_id=port16_bp,
                vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)
            sai_thrift_set_port_attribute(self.client, self.port16, port_vlan_id=30)

            vlan30_rif = sai_thrift_create_router_interface(
                self.client,
                type=SAI_ROUTER_INTERFACE_TYPE_VLAN,
                virtual_router_id=self.default_vrf,
                vlan_id=vlan30)

            l2mc_grp = sai_thrift_create_l2mc_group(self.client)
            l2mc_mbr = sai_thrift_create_l2mc_group_member(
                self.client,
                l2mc_group_id=l2mc_grp,
                l2mc_output_id=port15_bp)

            # adding tunnel to the L2MC group corresponding to VLAN 10 RIF
            l2mc_tun1_mbr = sai_thrift_create_l2mc_group_member(
                self.client,
                l2mc_group_id=self.l2mc_grp,
                l2mc_output_id=self.p2p_tunnel1_bp)

            # adding tunnel to the L2MC group corresponding to VLAN 30 RIF
            l2mc_tun2_mbr = sai_thrift_create_l2mc_group_member(
                self.client,
                l2mc_group_id=l2mc_grp,
                l2mc_output_id=self.p2p_tunnel2_bp)

            # prepare extensions_l2mc_output_id attribute
            attr_value = sai_thrift_attribute_value_t(oid=l2mc_grp)
            l2mc_output = sai_thrift_attribute_t(
                id=(SAI_IPMC_GROUP_MEMBER_ATTR_END),  # from saiipmcgroupextensions.h
                value=attr_value)

            ipmc_mbr4 = sai_thrift_create_ipmc_group_member(
                self.client,
                ipmc_group_id=self.ipmc_grp, ipmc_output_id=vlan30_rif,
                custom_attribute=l2mc_output)

            # set VLAN to VNI mapping
            decap_tunnel_map_entry_vlan30 = sai_thrift_create_tunnel_map_entry(
                self.client,
                tunnel_map_type=SAI_TUNNEL_MAP_TYPE_VNI_TO_VLAN_ID,
                tunnel_map=self.decap_tunnel_map_vlan,
                vlan_id_value=30,
                vni_id_key=vni3)

            encap_tunnel_map_entry_vlan30 = sai_thrift_create_tunnel_map_entry(
                self.client,
                tunnel_map_type=SAI_TUNNEL_MAP_TYPE_VLAN_ID_TO_VNI,
                tunnel_map=self.encap_tunnel_map_vlan,
                vlan_id_key=30,
                vni_id_value=vni3)

            pkt = simple_udp_packet(
                eth_dst=self.dst_mac,
                eth_src=self.src_mac,
                ip_src=self.src_ip,
                ip_dst=self.grp_ip,
                ip_ttl=64)
            exp_pkt = simple_udp_packet(
                eth_dst=self.dst_mac,
                eth_src=self.default_rmac,
                ip_src=self.src_ip,
                ip_dst=self.grp_ip,
                ip_ttl=63)
            vxlan_pkt10 = Mask(
                simple_vxlan_packet(eth_dst=self.unbor1_mac,
                                    eth_src=ROUTER_MAC,
                                    ip_dst=self.tun1_ip,
                                    ip_src=self.lpb_ip,
                                    ip_id=0,
                                    ip_ttl=63,
                                    ip_flags=0x2,
                                    with_udp_chksum=False,
                                    vxlan_vni=self.vni1,
                                    inner_frame=exp_pkt))
            vxlan_pkt10.set_do_not_care_scapy(UDP, 'sport')
            vxlan_pkt20 = Mask(
                simple_vxlan_packet(eth_dst=self.unbor1_mac,
                                    eth_src=ROUTER_MAC,
                                    ip_dst=self.tun1_ip,
                                    ip_src=self.lpb_ip,
                                    ip_id=0,
                                    ip_ttl=63,
                                    ip_flags=0x2,
                                    with_udp_chksum=False,
                                    vxlan_vni=self.vni2,
                                    inner_frame=exp_pkt))
            vxlan_pkt20.set_do_not_care_scapy(UDP, 'sport')
            vxlan_pkt30 = Mask(
                simple_vxlan_packet(eth_dst=self.unbor2_mac,
                                    eth_src=ROUTER_MAC,
                                    ip_dst=self.tun2_ip,
                                    ip_src=self.lpb_ip,
                                    ip_id=0,
                                    ip_ttl=63,
                                    ip_flags=0x2,
                                    with_udp_chksum=False,
                                    vxlan_vni=vni3,
                                    inner_frame=exp_pkt))
            vxlan_pkt30.set_do_not_care_scapy(UDP, 'sport')

            print("Sending packet to multicast group with SVI members with tunnel")
            send_packet(self, self.dev_port8, pkt)
            # tunnel as a member of VLAN 10
            verify_packet(self, vxlan_pkt10, self.uport1_dev)
            print("Packet was received on tunnel port of VLAN 10")
            # tunnel as a member of VLAN 20
            verify_packet(self, vxlan_pkt20, self.uport1_dev)
            print("Packet was received on tunnel port of VLAN 20")
            # tunnel as a member of VLAN 30
            verify_packet(self, vxlan_pkt30, self.uport2_dev)
            print("Packet was received on tunnel port of VLAN 30")
            verify_packets(
                self, exp_pkt,
                # VLAN 10 ports being members of L2MC group
                [self.dev_port0, self.dev_port1, self.dev_port2,
                 # all VLAN 20 ports
                 self.dev_port5, self.dev_port6, self.dev_port7,
                 # VLAN 30 port being a member of L2MC group
                 self.dev_port15,
                 # regular IPMC group port
                 self.dev_port9])
            verify_no_other_packets(self)
            print("\tOK")

        finally:
            sai_thrift_remove_tunnel_map_entry(
                self.client, encap_tunnel_map_entry_vlan30)
            sai_thrift_remove_tunnel_map_entry(
                self.client, decap_tunnel_map_entry_vlan30)
            sai_thrift_remove_ipmc_group_member(self.client, ipmc_mbr4)
            sai_thrift_remove_l2mc_group_member(self.client, l2mc_tun2_mbr)
            sai_thrift_remove_l2mc_group_member(self.client, l2mc_tun1_mbr)
            sai_thrift_remove_l2mc_group_member(self.client,l2mc_mbr)
            sai_thrift_remove_l2mc_group(self.client, l2mc_grp)
            sai_thrift_remove_router_interface(self.client, vlan30_rif)
            sai_thrift_set_port_attribute(self.client, self.port16, port_vlan_id=0)
            sai_thrift_remove_vlan_member(self.client, port16_vlan_member)
            sai_thrift_remove_bridge_port(self.client, port16_bp)
            sai_thrift_set_port_attribute(self.client, self.port15, port_vlan_id=0)
            sai_thrift_remove_vlan_member(self.client, port15_vlan_member)
            sai_thrift_remove_bridge_port(self.client, port15_bp)
            sai_thrift_remove_vlan(self.client, vlan30)
