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
Thrift SAI interface Tunnel tests
"""
import ipaddress

from sai_base_test import *


#
#           p2mp_tunnel
#           (encap_map)
#             vlan10
#               to
#             vni1000
#      |                  |
#      |                  |
#      |                  |
#   p2p_tunnel1       p2p_tunnel2
#   (vlan10)            (vlan10)
#           (encap_map)
#   vlan10               vlan10
#     to                   to
#   vni1000              vni2000
#
@group('l2-vxlan')
@group('downstream-vni')
class L2VxLanEncapDifferentMapTest(SaiHelper):
    '''
    This class contains tests for L2 VxLAN encapsulation
    using different tunnel mappers with the same vlan key.
    One encap map contains entries created as a reverse
    to the decap map entries.
    '''
    def setUp(self):
        super(L2VxLanEncapDifferentMapTest, self).setUp()

        self.tun1_ip = "10.0.0.65"
        self.tun2_ip = "10.0.0.33"
        self.lpb_ip = "10.1.1.10"
        self.uport1_ip = "10.0.0.64"
        self.uport2_ip = "10.0.0.32"
        self.customer1_ip = "192.168.100.1"
        self.customer2_ip = "192.168.100.2"
        self.customer1_mac = "00:22:22:22:22:11"
        self.customer2_mac = "00:22:22:22:22:22"
        self.unbor1_mac = "00:33:33:33:33:11"
        self.unbor2_mac = "00:33:33:33:33:22"
        self.vni1 = 1000
        self.vni2 = 2000

        self.oport = self.port0
        self.oport_dev = self.dev_port0
        self.oport_bp = self.port0_bp
        self.uport1 = self.port10
        self.uport1_dev = self.dev_port10
        self.uport1_rif = self.port10_rif
        self.uport2 = self.port11
        self.uport2_dev = self.dev_port11
        self.uport2_rif = self.port11_rif

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
        sai_thrift_create_neighbor_entry(self.client,
                                         self.unbor1,
                                         dst_mac_address=self.unbor1_mac)
        self.unbor2 = sai_thrift_neighbor_entry_t(
            ip_address=sai_ipaddress(self.tun2_ip),
            rif_id=self.uport2_rif,
            switch_id=self.switch_id)
        sai_thrift_create_neighbor_entry(self.client,
                                         self.unbor2,
                                         dst_mac_address=self.unbor2_mac)

        # tunnel configuration
        self.tunnel_type = SAI_TUNNEL_TYPE_VXLAN

        self.encap_tunnel_map_vlan = sai_thrift_create_tunnel_map(
            self.client, type=SAI_TUNNEL_MAP_TYPE_VLAN_ID_TO_VNI)

        self.encap_tunnel_map_vlan2 = sai_thrift_create_tunnel_map(
            self.client, type=SAI_TUNNEL_MAP_TYPE_VLAN_ID_TO_VNI)

        self.decap_tunnel_map_vlan = sai_thrift_create_tunnel_map(
            self.client, type=SAI_TUNNEL_MAP_TYPE_VNI_TO_VLAN_ID)

        encap_maps = sai_thrift_object_list_t(
            count=1,
            idlist=[self.encap_tunnel_map_vlan])
        encap_maps2 = sai_thrift_object_list_t(
            count=1,
            idlist=[self.encap_tunnel_map_vlan2])
        decap_maps = sai_thrift_object_list_t(
            count=1,
            idlist=[self.decap_tunnel_map_vlan])

        self.p2mp_tunnel = sai_thrift_create_tunnel(
            self.client,
            type=self.tunnel_type,
            underlay_interface=self.urif_lpb,
            decap_mappers=decap_maps,
            encap_mappers=encap_maps,
            encap_src_ip=sai_ipaddress(self.lpb_ip),
            peer_mode=SAI_TUNNEL_PEER_MODE_P2MP)

        self.tunnel_term = sai_thrift_create_tunnel_term_table_entry(
            self.client,
            type=SAI_TUNNEL_TERM_TABLE_ENTRY_TYPE_P2MP,
            vr_id=self.default_vrf,
            dst_ip=sai_ipaddress(self.lpb_ip),
            tunnel_type=self.tunnel_type,
            action_tunnel_id=self.p2mp_tunnel)

        self.p2p_tunnel1 = sai_thrift_create_tunnel(
            self.client,
            type=self.tunnel_type,
            underlay_interface=self.urif_lpb,
            decap_mappers=decap_maps,
            encap_mappers=encap_maps,
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

        self.p2p_tun1_vlan10_member = sai_thrift_create_vlan_member(
            self.client,
            vlan_id=self.vlan10,
            bridge_port_id=self.p2p_tunnel1_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)

        self.p2p_tunnel2 = sai_thrift_create_tunnel(
            self.client,
            type=self.tunnel_type,
            underlay_interface=self.urif_lpb,
            decap_mappers=decap_maps,
            encap_mappers=encap_maps2,
            encap_src_ip=sai_ipaddress(self.lpb_ip),
            peer_mode=SAI_TUNNEL_PEER_MODE_P2P,
            encap_dst_ip=sai_ipaddress(self.tun2_ip))

        # create map entries after the creation of tunnels
        self.decap_tun_map_entry_vlan10 = sai_thrift_create_tunnel_map_entry(
            self.client,
            tunnel_map_type=SAI_TUNNEL_MAP_TYPE_VNI_TO_VLAN_ID,
            tunnel_map=self.decap_tunnel_map_vlan,
            vni_id_key=self.vni1,
            vlan_id_value=10)

        self.encap_tun_map_entry_vlan10_2 = sai_thrift_create_tunnel_map_entry(
            self.client,
            tunnel_map_type=SAI_TUNNEL_MAP_TYPE_VLAN_ID_TO_VNI,
            tunnel_map=self.encap_tunnel_map_vlan2,
            vlan_id_key=10,
            vni_id_value=self.vni2)

        self.p2p_tunnel2_bp = sai_thrift_create_bridge_port(
            self.client,
            type=SAI_BRIDGE_PORT_TYPE_TUNNEL,
            tunnel_id=self.p2p_tunnel2,
            bridge_id=self.default_1q_bridge,
            admin_state=True,
            fdb_learning_mode=SAI_BRIDGE_PORT_FDB_LEARNING_MODE_DISABLE)

        self.p2p_tun2_vlan10_member = sai_thrift_create_vlan_member(
            self.client,
            vlan_id=self.vlan10,
            bridge_port_id=self.p2p_tunnel2_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)

        # test packets
        self.pkt = simple_udp_packet(eth_dst=self.customer2_mac,
                                     eth_src=self.customer1_mac,
                                     ip_dst=self.customer2_ip,
                                     ip_src=self.customer1_ip,
                                     ip_id=108,
                                     ip_ttl=64,
                                     pktlen=100)

    def runTest(self):
        try:
            self.differentVniTest()
        finally:
            pass

    def tearDown(self):
        sai_thrift_flush_fdb_entries(
            self.client, entry_type=SAI_FDB_FLUSH_ENTRY_TYPE_ALL)

        sai_thrift_remove_vlan_member(self.client, self.p2p_tun2_vlan10_member)
        sai_thrift_remove_bridge_port(self.client, self.p2p_tunnel2_bp)
        sai_thrift_remove_tunnel(self.client, self.p2p_tunnel2)
        sai_thrift_remove_vlan_member(self.client, self.p2p_tun1_vlan10_member)
        sai_thrift_remove_bridge_port(self.client, self.p2p_tunnel1_bp)
        sai_thrift_remove_tunnel(self.client, self.p2p_tunnel1)
        sai_thrift_remove_tunnel_map_entry(
            self.client, self.decap_tun_map_entry_vlan10)
        sai_thrift_remove_tunnel_map_entry(
            self.client, self.encap_tun_map_entry_vlan10_2)
        sai_thrift_remove_tunnel_term_table_entry(
            self.client, self.tunnel_term)
        sai_thrift_remove_tunnel(self.client, self.p2mp_tunnel)
        sai_thrift_remove_tunnel_map(self.client, self.encap_tunnel_map_vlan)
        sai_thrift_remove_tunnel_map(self.client, self.decap_tunnel_map_vlan)
        sai_thrift_remove_tunnel_map(self.client, self.encap_tunnel_map_vlan2)
        sai_thrift_remove_neighbor_entry(self.client, self.unbor2)
        sai_thrift_remove_neighbor_entry(self.client, self.unbor1)
        sai_thrift_remove_router_interface(self.client, self.urif_lpb)

        super(L2VxLanEncapDifferentMapTest, self).tearDown()

    def differentVniTest(self):
        '''
        Verify L2 VxLAN packets for two tunnels with different vlan to vni
        mappers within the same vlan.
        '''
        print("\ndifferentVniTest()")

        rx_port_vlan10 = self.dev_port4  # untagged in VLAN 10
        try:
            print("Creating MAC entry for P2P tunnels")
            fdb_entry1 = sai_thrift_fdb_entry_t(
                switch_id=self.switch_id,
                mac_address=self.customer2_mac,
                bv_id=self.vlan10)
            sai_thrift_create_fdb_entry(
                self.client,
                fdb_entry1,
                type=SAI_FDB_ENTRY_TYPE_STATIC,
                bridge_port_id=self.p2p_tunnel1_bp)

            vxlan_pkt = Mask(
                simple_vxlan_packet(eth_dst=self.unbor1_mac,
                                    eth_src=ROUTER_MAC,
                                    ip_dst=self.tun1_ip,
                                    ip_src=self.lpb_ip,
                                    ip_id=0,
                                    ip_ttl=64,
                                    ip_flags=0x2,
                                    with_udp_chksum=False,
                                    vxlan_vni=self.vni1,
                                    inner_frame=self.pkt))
            vxlan_pkt.set_do_not_care_scapy(UDP, 'sport')

            vxlan_pkt2 = Mask(
                simple_vxlan_packet(eth_dst=self.unbor2_mac,
                                    eth_src=ROUTER_MAC,
                                    ip_dst=self.tun2_ip,
                                    ip_src=self.lpb_ip,
                                    ip_id=0,
                                    ip_ttl=64,
                                    ip_flags=0x2,
                                    with_udp_chksum=False,
                                    vxlan_vni=self.vni2,
                                    inner_frame=self.pkt))
            vxlan_pkt2.set_do_not_care_scapy(UDP, 'sport')

            print("Sending packet on port %d, forward to port %d, VNI: %d"
                  % (rx_port_vlan10, self.uport1_dev, self.vni1))
            send_packet(self, rx_port_vlan10, self.pkt)
            verify_packet(self, vxlan_pkt, self.uport1_dev)
            verify_no_other_packets(self)
            print("Packet was L2 forwarded to port %d" % self.uport1_dev)

            status = sai_thrift_set_fdb_entry_attribute(
                self.client, fdb_entry1, bridge_port_id=self.p2p_tunnel2_bp)
            self.assertEqual(status, SAI_STATUS_SUCCESS)

            print("Sending packet on port %d, forward to port %d, VNI: %d"
                  % (rx_port_vlan10, self.uport2_dev, self.vni2))
            send_packet(self, rx_port_vlan10, self.pkt)
            verify_packet(self, vxlan_pkt2, self.uport2_dev)
            verify_no_other_packets(self)
            print("Packet was L2 forwarded to port %d" % self.uport2_dev)

        finally:
            sai_thrift_flush_fdb_entries(
                self.client, entry_type=SAI_FDB_FLUSH_ENTRY_TYPE_ALL)


@group('l2-vxlan')
class L2VxLanEncapReverseMapTest(SaiHelper):
    '''
    Class containg tests that verify reverse map is created correctly
    depending on the order of creating objects.
    '''

    def setUp(self):
        super(L2VxLanEncapReverseMapTest, self).setUp()

        self.oport_dev = self.dev_port0
        self.oport_bp = self.port0_bp
        self.uport_dev = self.dev_port10
        self.uport_rif = self.port10_rif

        self.vni1 = 1000
        self.vni2 = 2000
        self.tun1_ip = "10.0.0.65"
        self.lpb_ip = "10.1.0.32"
        self.tun_ip_mask = "/32"
        self.uport1_myip = "10.0.0.64"
        self.uport_mask = "/31"
        self.customer1_ip = "192.168.100.1"
        self.customer1_ipv6 = "2001:0db8::100:2"
        self.customer2_ip = "192.168.100.8"
        self.customer2_ipv6 = "2001:0db8::100:9"
        self.customer1_mac = "00:22:22:22:22:11"
        self.customer2_mac = "00:22:22:22:22:22"
        self.unbor1_mac = "00:33:33:33:33:11"
        self.unbor2_mac = "00:33:33:33:33:22"

        # underlay loopback rif for tunnels
        self.urif_lpb = sai_thrift_create_router_interface(
            self.client,
            type=SAI_ROUTER_INTERFACE_TYPE_LOOPBACK,
            virtual_router_id=self.default_vrf,
            mtu=9100)

        self.uport1_prefix_route = sai_thrift_route_entry_t(
            destination=sai_ipprefix(self.uport1_myip + self.uport_mask),
            switch_id=self.switch_id,
            vr_id=self.default_vrf)
        sai_thrift_create_route_entry(self.client,
                                      self.uport1_prefix_route,
                                      next_hop_id=self.uport_rif)

        # underlay neighbor
        self.unbor = sai_thrift_neighbor_entry_t(
            ip_address=sai_ipaddress(self.tun1_ip),
            rif_id=self.uport_rif,
            switch_id=self.switch_id)
        sai_thrift_create_neighbor_entry(self.client,
                                         self.unbor,
                                         dst_mac_address=self.unbor1_mac)

        # underlay nexthop
        self.unhop = sai_thrift_create_next_hop(
            self.client,
            type=SAI_NEXT_HOP_TYPE_IP,
            ip=sai_ipaddress(self.tun1_ip),
            router_interface_id=self.uport_rif)

        # create overlay vrf
        self.ovrf = sai_thrift_create_virtual_router(self.client)

        # create fdb entry, neighbor, nexthop for customer 1 on port0, vlan 10
        self.customer1_fdb_entry = sai_thrift_fdb_entry_t(
            bv_id=self.vlan10,
            mac_address=self.customer1_mac,
            switch_id=self.switch_id)
        sai_thrift_create_fdb_entry(self.client,
                                    self.customer1_fdb_entry,
                                    type=SAI_FDB_ENTRY_TYPE_STATIC,
                                    bridge_port_id=self.oport_bp)

        # L2 forwarding into VXLAN tunnel
        self.pkt = simple_udp_packet(eth_dst=self.customer2_mac,
                                     eth_src=self.customer1_mac,
                                     ip_dst=self.customer2_ip,
                                     ip_src=self.customer1_ip,
                                     ip_id=108,
                                     ip_ttl=64)
        self.vxlan_pkt = Mask(
            simple_vxlan_packet(eth_dst=self.unbor1_mac,
                                eth_src=ROUTER_MAC,
                                ip_dst=self.tun1_ip,
                                ip_src=self.lpb_ip,
                                ip_id=0,
                                ip_ttl=64,
                                ip_flags=0x2,
                                with_udp_chksum=False,
                                vxlan_vni=self.vni1,
                                inner_frame=self.pkt))
        self.vxlan_pkt.set_do_not_care_scapy(UDP, 'sport')

        self.vxlan_pkt2 = Mask(
            simple_vxlan_packet(eth_dst=self.unbor1_mac,
                                eth_src=ROUTER_MAC,
                                ip_dst=self.tun1_ip,
                                ip_src=self.lpb_ip,
                                ip_id=0,
                                ip_ttl=64,
                                ip_flags=0x2,
                                with_udp_chksum=False,
                                vxlan_vni=self.vni2,
                                inner_frame=self.pkt))
        self.vxlan_pkt2.set_do_not_care_scapy(UDP, 'sport')

    def runTest(self):
        try:
            self.reverseMapTunnelBeforeEntries()
            self.reverseMapTunnelAfterEntries()
            self.reverseMapRemovingDecapMapBeforeEncap()
            self.reverseMapOverwritingTunnelAfterEntries()
            self.reverseMapOverwritingTunnelBeforeEntries()
        finally:
            pass

    def tearDown(self):
        sai_thrift_remove_virtual_router(self.client, self.ovrf)
        sai_thrift_remove_next_hop(self.client, self.unhop)
        sai_thrift_remove_neighbor_entry(self.client, self.unbor)
        sai_thrift_remove_route_entry(self.client, self.uport1_prefix_route)
        sai_thrift_remove_router_interface(self.client, self.urif_lpb)

        super(L2VxLanEncapReverseMapTest, self).tearDown()

    def reverseMapTunnelBeforeEntries(self):
        '''
        Test verifies that reverse map is created when tunnel
        is created before tunnel mapper entries.
        '''
        print("\nreverseMapTunnelBeforeEntries()")

        # L2 VxLAN configuration below
        tunnel_type = SAI_TUNNEL_TYPE_VXLAN
        peer_mode = SAI_TUNNEL_PEER_MODE_P2MP
        term_type = SAI_TUNNEL_TERM_TABLE_ENTRY_TYPE_P2MP

        # create tunnel maps
        decap_tunnel_map_vlan = sai_thrift_create_tunnel_map(
            self.client, type=SAI_TUNNEL_MAP_TYPE_VNI_TO_VLAN_ID)

        encap_tunnel_map_vlan = sai_thrift_create_tunnel_map(
            self.client, type=SAI_TUNNEL_MAP_TYPE_VLAN_ID_TO_VNI)

        # create tunnel
        decap_maps = sai_thrift_object_list_t(
            count=1,
            idlist=[decap_tunnel_map_vlan])
        encap_maps = sai_thrift_object_list_t(
            count=1,
            idlist=[encap_tunnel_map_vlan])

        tunnel1 = sai_thrift_create_tunnel(
            self.client,
            type=tunnel_type,
            underlay_interface=self.urif_lpb,
            decap_mappers=decap_maps,
            encap_mappers=encap_maps,
            encap_src_ip=sai_ipaddress(self.lpb_ip),
            peer_mode=peer_mode)

        tunnel_term1 = sai_thrift_create_tunnel_term_table_entry(
            self.client,
            type=term_type,
            vr_id=self.default_vrf,
            dst_ip=sai_ipaddress(self.lpb_ip),
            tunnel_type=tunnel_type,
            action_tunnel_id=tunnel1)

        # p2p tunnel is created when the first evpn type 3 route
        # is received from a remote vtep
        tunnel2 = sai_thrift_create_tunnel(
            self.client,
            type=tunnel_type,
            underlay_interface=self.urif_lpb,
            decap_mappers=decap_maps,
            encap_mappers=encap_maps,
            encap_src_ip=sai_ipaddress(self.lpb_ip),
            peer_mode=SAI_TUNNEL_PEER_MODE_P2P,
            encap_dst_ip=sai_ipaddress(self.tun1_ip))

        # bridge port is created on p2p tunnel
        tun2_bp = sai_thrift_create_bridge_port(
            self.client,
            type=SAI_BRIDGE_PORT_TYPE_TUNNEL,
            tunnel_id=tunnel2,
            bridge_id=self.default_1q_bridge,
            admin_state=True,
            fdb_learning_mode=SAI_BRIDGE_PORT_FDB_LEARNING_MODE_DISABLE)

        # vlan_member is created using tunnel bridge_port
        tun2_vlan_member = sai_thrift_create_vlan_member(
            self.client,
            vlan_id=self.vlan10,
            bridge_port_id=tun2_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)

        # fdb entry for L2 forwarding into VXLAN tunnel
        fdb_entry = sai_thrift_fdb_entry_t(
            bv_id=self.vlan10,
            mac_address=self.customer2_mac,
            switch_id=self.switch_id)
        sai_thrift_create_fdb_entry(self.client,
                                    fdb_entry,
                                    type=SAI_FDB_ENTRY_TYPE_STATIC,
                                    bridge_port_id=tun2_bp)

        # create tunnel map entries for vlan
        decap_tunnel_map_entry_vlan = sai_thrift_create_tunnel_map_entry(
            self.client,
            tunnel_map_type=SAI_TUNNEL_MAP_TYPE_VNI_TO_VLAN_ID,
            tunnel_map=decap_tunnel_map_vlan,
            vlan_id_value=10,
            vni_id_key=self.vni1)

        try:
            print("Sending packet from port %d -> VNI %d" %
                  (self.oport_dev, self.vni1))
            send_packet(self, self.oport_dev, self.pkt)
            verify_packet(self, self.vxlan_pkt, self.uport_dev)
            print("Packet was L2 forwarded into the VXLAN tunnel")
        finally:
            sai_thrift_flush_fdb_entries(
                self.client, entry_type=SAI_FDB_FLUSH_ENTRY_TYPE_ALL)

            sai_thrift_remove_vlan_member(self.client, tun2_vlan_member)
            sai_thrift_remove_bridge_port(self.client, tun2_bp)

            sai_thrift_remove_tunnel(self.client, tunnel2)
            sai_thrift_remove_tunnel_map_entry(
                self.client, decap_tunnel_map_entry_vlan)
            sai_thrift_remove_tunnel_term_table_entry(
                self.client, tunnel_term1)
            sai_thrift_remove_tunnel(self.client, tunnel1)
            sai_thrift_remove_tunnel_map(self.client, encap_tunnel_map_vlan)
            sai_thrift_remove_tunnel_map(self.client, decap_tunnel_map_vlan)

    def reverseMapTunnelAfterEntries(self):
        '''
        Test verifies that reverse map is created when tunnel
        is created after tunnel mapper entries.
        '''
        print("\nreverseMapTunnelAfterEntries()")

        # L2 VxLAN configuration below
        tunnel_type = SAI_TUNNEL_TYPE_VXLAN
        peer_mode = SAI_TUNNEL_PEER_MODE_P2MP
        term_type = SAI_TUNNEL_TERM_TABLE_ENTRY_TYPE_P2MP

        # create tunnel maps
        decap_tunnel_map_vlan = sai_thrift_create_tunnel_map(
            self.client, type=SAI_TUNNEL_MAP_TYPE_VNI_TO_VLAN_ID)

        encap_tunnel_map_vlan = sai_thrift_create_tunnel_map(
            self.client, type=SAI_TUNNEL_MAP_TYPE_VLAN_ID_TO_VNI)

        # create tunnel
        decap_maps = sai_thrift_object_list_t(
            count=1,
            idlist=[decap_tunnel_map_vlan])
        encap_maps = sai_thrift_object_list_t(
            count=1,
            idlist=[encap_tunnel_map_vlan])

        # create tunnel map entries for vlan
        decap_tunnel_map_entry_vlan = sai_thrift_create_tunnel_map_entry(
            self.client,
            tunnel_map_type=SAI_TUNNEL_MAP_TYPE_VNI_TO_VLAN_ID,
            tunnel_map=decap_tunnel_map_vlan,
            vlan_id_value=10,
            vni_id_key=self.vni1)

        tunnel1 = sai_thrift_create_tunnel(
            self.client,
            type=tunnel_type,
            underlay_interface=self.urif_lpb,
            decap_mappers=decap_maps,
            encap_mappers=encap_maps,
            encap_src_ip=sai_ipaddress(self.lpb_ip),
            peer_mode=peer_mode)

        tunnel_term1 = sai_thrift_create_tunnel_term_table_entry(
            self.client,
            type=term_type,
            vr_id=self.default_vrf,
            dst_ip=sai_ipaddress(self.lpb_ip),
            tunnel_type=tunnel_type,
            action_tunnel_id=tunnel1)

        # p2p tunnel is created when the first evpn type 3 route
        # is received from a remote vtep
        tunnel2 = sai_thrift_create_tunnel(
            self.client,
            type=tunnel_type,
            underlay_interface=self.urif_lpb,
            decap_mappers=decap_maps,
            encap_mappers=encap_maps,
            encap_src_ip=sai_ipaddress(self.lpb_ip),
            peer_mode=SAI_TUNNEL_PEER_MODE_P2P,
            encap_dst_ip=sai_ipaddress(self.tun1_ip))

        # bridge port is created on p2p tunnel
        tun2_bp = sai_thrift_create_bridge_port(
            self.client,
            type=SAI_BRIDGE_PORT_TYPE_TUNNEL,
            tunnel_id=tunnel2,
            bridge_id=self.default_1q_bridge,
            admin_state=True,
            fdb_learning_mode=SAI_BRIDGE_PORT_FDB_LEARNING_MODE_DISABLE)

        # vlan_member is created using tunnel bridge_port
        tun2_vlan_member = sai_thrift_create_vlan_member(
            self.client,
            vlan_id=self.vlan10,
            bridge_port_id=tun2_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)

        # fdb entry for L2 forwarding into VXLAN tunnel
        fdb_entry = sai_thrift_fdb_entry_t(
            bv_id=self.vlan10,
            mac_address=self.customer2_mac,
            switch_id=self.switch_id)
        sai_thrift_create_fdb_entry(self.client,
                                    fdb_entry,
                                    type=SAI_FDB_ENTRY_TYPE_STATIC,
                                    bridge_port_id=tun2_bp)

        try:
            print("Sending packet from port %d -> VNI %d" %
                  (self.oport_dev, self.vni1))
            send_packet(self, self.oport_dev, self.pkt)
            verify_packet(self, self.vxlan_pkt, self.uport_dev)
            print("Packet was L2 forwarded into the VXLAN tunnel")
        finally:
            sai_thrift_flush_fdb_entries(
                self.client, entry_type=SAI_FDB_FLUSH_ENTRY_TYPE_ALL)

            sai_thrift_remove_vlan_member(self.client, tun2_vlan_member)
            sai_thrift_remove_bridge_port(self.client, tun2_bp)

            sai_thrift_remove_tunnel(self.client, tunnel2)
            sai_thrift_remove_tunnel_map_entry(
                self.client, decap_tunnel_map_entry_vlan)
            sai_thrift_remove_tunnel_term_table_entry(
                self.client, tunnel_term1)
            sai_thrift_remove_tunnel(self.client, tunnel1)
            sai_thrift_remove_tunnel_map(self.client, encap_tunnel_map_vlan)
            sai_thrift_remove_tunnel_map(self.client, decap_tunnel_map_vlan)

    def reverseMapRemovingDecapMapBeforeEncap(self):
        '''
        Test verifies that encap map is correclty removed after
        the removal of decap map.
        '''
        print("\nreverseMapRemovingDecapMapBeforeEncap()")

        # L2 VxLAN configuration below
        tunnel_type = SAI_TUNNEL_TYPE_VXLAN
        peer_mode = SAI_TUNNEL_PEER_MODE_P2MP
        term_type = SAI_TUNNEL_TERM_TABLE_ENTRY_TYPE_P2MP

        # create tunnel maps
        decap_tunnel_map_vlan = sai_thrift_create_tunnel_map(
            self.client, type=SAI_TUNNEL_MAP_TYPE_VNI_TO_VLAN_ID)

        encap_tunnel_map_vlan = sai_thrift_create_tunnel_map(
            self.client, type=SAI_TUNNEL_MAP_TYPE_VLAN_ID_TO_VNI)

        # create tunnel
        decap_maps = sai_thrift_object_list_t(
            count=1,
            idlist=[decap_tunnel_map_vlan])
        encap_maps = sai_thrift_object_list_t(
            count=1,
            idlist=[encap_tunnel_map_vlan])

        tunnel1 = sai_thrift_create_tunnel(
            self.client,
            type=tunnel_type,
            underlay_interface=self.urif_lpb,
            decap_mappers=decap_maps,
            encap_mappers=encap_maps,
            encap_src_ip=sai_ipaddress(self.lpb_ip),
            peer_mode=peer_mode)

        tunnel_term1 = sai_thrift_create_tunnel_term_table_entry(
            self.client,
            type=term_type,
            vr_id=self.default_vrf,
            dst_ip=sai_ipaddress(self.lpb_ip),
            tunnel_type=tunnel_type,
            action_tunnel_id=tunnel1)

        # p2p tunnel is created when the first evpn type 3 route
        # is received from a remote vtep
        tunnel2 = sai_thrift_create_tunnel(
            self.client,
            type=tunnel_type,
            underlay_interface=self.urif_lpb,
            decap_mappers=decap_maps,
            encap_mappers=encap_maps,
            encap_src_ip=sai_ipaddress(self.lpb_ip),
            peer_mode=SAI_TUNNEL_PEER_MODE_P2P,
            encap_dst_ip=sai_ipaddress(self.tun1_ip))

        # bridge port is created on p2p tunnel
        tun2_bp = sai_thrift_create_bridge_port(
            self.client,
            type=SAI_BRIDGE_PORT_TYPE_TUNNEL,
            tunnel_id=tunnel2,
            bridge_id=self.default_1q_bridge,
            admin_state=True,
            fdb_learning_mode=SAI_BRIDGE_PORT_FDB_LEARNING_MODE_DISABLE)

        # vlan_member is created using tunnel bridge_port
        tun2_vlan_member = sai_thrift_create_vlan_member(
            self.client,
            vlan_id=self.vlan10,
            bridge_port_id=tun2_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)

        # fdb entry for L2 forwarding into VXLAN tunnel
        fdb_entry = sai_thrift_fdb_entry_t(
            bv_id=self.vlan10,
            mac_address=self.customer2_mac,
            switch_id=self.switch_id)
        sai_thrift_create_fdb_entry(self.client,
                                    fdb_entry,
                                    type=SAI_FDB_ENTRY_TYPE_STATIC,
                                    bridge_port_id=tun2_bp)

        # create tunnel map entries for vlan
        decap_tunnel_map_entry_vlan = sai_thrift_create_tunnel_map_entry(
            self.client,
            tunnel_map_type=SAI_TUNNEL_MAP_TYPE_VNI_TO_VLAN_ID,
            tunnel_map=decap_tunnel_map_vlan,
            vlan_id_value=10,
            vni_id_key=self.vni1)

        try:
            print("Sending packet from port %d -> VNI %d" %
                  (self.oport_dev, self.vni1))
            send_packet(self, self.oport_dev, self.pkt)
            verify_packet(self, self.vxlan_pkt, self.uport_dev)
            print("Packet was L2 forwarded into the VXLAN tunnel")
        finally:
            sai_thrift_flush_fdb_entries(
                self.client, entry_type=SAI_FDB_FLUSH_ENTRY_TYPE_ALL)

            sai_thrift_remove_vlan_member(self.client, tun2_vlan_member)
            sai_thrift_remove_bridge_port(self.client, tun2_bp)

            sai_thrift_remove_tunnel(self.client, tunnel2)
            sai_thrift_remove_tunnel_map_entry(
                self.client, decap_tunnel_map_entry_vlan)
            sai_thrift_remove_tunnel_term_table_entry(
                self.client, tunnel_term1)
            sai_thrift_remove_tunnel(self.client, tunnel1)
            status = sai_thrift_remove_tunnel_map(self.client,
                                                  decap_tunnel_map_vlan)
            self.assertEqual(status, SAI_STATUS_SUCCESS)
            status = sai_thrift_remove_tunnel_map(self.client,
                                                  encap_tunnel_map_vlan)
            self.assertEqual(status, SAI_STATUS_SUCCESS)

    def reverseMapOverwritingTunnelAfterEntries(self):
        '''
        Test verifies that reverse map is overwritten by different
        encap map in case tunnel is created after entries.
        '''
        print("\nreverseMapOverwritingTunnelAfterEntries()")

        # L2 VxLAN configuration below
        tunnel_type = SAI_TUNNEL_TYPE_VXLAN
        peer_mode = SAI_TUNNEL_PEER_MODE_P2MP
        term_type = SAI_TUNNEL_TERM_TABLE_ENTRY_TYPE_P2MP

        # create tunnel maps
        decap_tunnel_map_vlan = sai_thrift_create_tunnel_map(
            self.client, type=SAI_TUNNEL_MAP_TYPE_VNI_TO_VLAN_ID)

        encap_tunnel_map_vlan = sai_thrift_create_tunnel_map(
            self.client, type=SAI_TUNNEL_MAP_TYPE_VLAN_ID_TO_VNI)

        # create tunnel
        decap_maps = sai_thrift_object_list_t(
            count=1,
            idlist=[decap_tunnel_map_vlan])
        encap_maps = sai_thrift_object_list_t(
            count=1,
            idlist=[encap_tunnel_map_vlan])

        # create tunnel map entries for vlan
        decap_tunnel_map_entry_vlan = sai_thrift_create_tunnel_map_entry(
            self.client,
            tunnel_map_type=SAI_TUNNEL_MAP_TYPE_VNI_TO_VLAN_ID,
            tunnel_map=decap_tunnel_map_vlan,
            vlan_id_value=10,
            vni_id_key=self.vni1)

        encap_tunnel_map_entry_vlan = sai_thrift_create_tunnel_map_entry(
            self.client,
            tunnel_map_type=SAI_TUNNEL_MAP_TYPE_VLAN_ID_TO_VNI,
            tunnel_map=encap_tunnel_map_vlan,
            vlan_id_key=10,
            vni_id_value=self.vni2)

        tunnel1 = sai_thrift_create_tunnel(
            self.client,
            type=tunnel_type,
            underlay_interface=self.urif_lpb,
            decap_mappers=decap_maps,
            encap_mappers=encap_maps,
            encap_src_ip=sai_ipaddress(self.lpb_ip),
            peer_mode=peer_mode)

        tunnel_term1 = sai_thrift_create_tunnel_term_table_entry(
            self.client,
            type=term_type,
            vr_id=self.default_vrf,
            dst_ip=sai_ipaddress(self.lpb_ip),
            tunnel_type=tunnel_type,
            action_tunnel_id=tunnel1)

        # p2p tunnel is created when the first evpn type 3 route
        # is received from a remote vtep
        tunnel2 = sai_thrift_create_tunnel(
            self.client,
            type=tunnel_type,
            underlay_interface=self.urif_lpb,
            decap_mappers=decap_maps,
            encap_mappers=encap_maps,
            encap_src_ip=sai_ipaddress(self.lpb_ip),
            peer_mode=SAI_TUNNEL_PEER_MODE_P2P,
            encap_dst_ip=sai_ipaddress(self.tun1_ip))

        # bridge port is created on p2p tunnel
        tun2_bp = sai_thrift_create_bridge_port(
            self.client,
            type=SAI_BRIDGE_PORT_TYPE_TUNNEL,
            tunnel_id=tunnel2,
            bridge_id=self.default_1q_bridge,
            admin_state=True,
            fdb_learning_mode=SAI_BRIDGE_PORT_FDB_LEARNING_MODE_DISABLE)

        # vlan_member is created using tunnel bridge_port
        tun2_vlan_member = sai_thrift_create_vlan_member(
            self.client,
            vlan_id=self.vlan10,
            bridge_port_id=tun2_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)

        # fdb entry for L2 forwarding into VXLAN tunnel
        fdb_entry = sai_thrift_fdb_entry_t(
            bv_id=self.vlan10,
            mac_address=self.customer2_mac,
            switch_id=self.switch_id)
        sai_thrift_create_fdb_entry(self.client,
                                    fdb_entry,
                                    type=SAI_FDB_ENTRY_TYPE_STATIC,
                                    bridge_port_id=tun2_bp)

        try:
            print("Sending packet from port %d -> VNI %d" %
                  (self.oport_dev, self.vni2))
            send_packet(self, self.oport_dev, self.pkt)
            verify_packet(self, self.vxlan_pkt2, self.uport_dev)
            print("Packet was L2 forwarded into the VXLAN tunnel")
        finally:
            sai_thrift_flush_fdb_entries(
                self.client, entry_type=SAI_FDB_FLUSH_ENTRY_TYPE_ALL)

            sai_thrift_remove_vlan_member(self.client, tun2_vlan_member)
            sai_thrift_remove_bridge_port(self.client, tun2_bp)

            sai_thrift_remove_tunnel(self.client, tunnel2)
            sai_thrift_remove_tunnel_map_entry(
                self.client, encap_tunnel_map_entry_vlan)
            sai_thrift_remove_tunnel_map_entry(
                self.client, decap_tunnel_map_entry_vlan)
            sai_thrift_remove_tunnel_term_table_entry(
                self.client, tunnel_term1)
            sai_thrift_remove_tunnel(self.client, tunnel1)
            sai_thrift_remove_tunnel_map(self.client, encap_tunnel_map_vlan)
            sai_thrift_remove_tunnel_map(self.client, decap_tunnel_map_vlan)

    def reverseMapOverwritingTunnelBeforeEntries(self):
        '''
        Test verifies that reverse map is overwritten by different
        encap map in case tunnel is created before entries.
        '''
        print("\nreverseMapOverwritingTunnelBeforeEntries()")

        # L2 VxLAN configuration below
        tunnel_type = SAI_TUNNEL_TYPE_VXLAN
        peer_mode = SAI_TUNNEL_PEER_MODE_P2MP
        term_type = SAI_TUNNEL_TERM_TABLE_ENTRY_TYPE_P2MP

        # create tunnel maps
        decap_tunnel_map_vlan = sai_thrift_create_tunnel_map(
            self.client, type=SAI_TUNNEL_MAP_TYPE_VNI_TO_VLAN_ID)

        encap_tunnel_map_vlan = sai_thrift_create_tunnel_map(
            self.client, type=SAI_TUNNEL_MAP_TYPE_VLAN_ID_TO_VNI)

        # create tunnel
        decap_maps = sai_thrift_object_list_t(
            count=1,
            idlist=[decap_tunnel_map_vlan])
        encap_maps = sai_thrift_object_list_t(
            count=1,
            idlist=[encap_tunnel_map_vlan])

        tunnel1 = sai_thrift_create_tunnel(
            self.client,
            type=tunnel_type,
            underlay_interface=self.urif_lpb,
            decap_mappers=decap_maps,
            encap_mappers=encap_maps,
            encap_src_ip=sai_ipaddress(self.lpb_ip),
            peer_mode=peer_mode)

        tunnel_term1 = sai_thrift_create_tunnel_term_table_entry(
            self.client,
            type=term_type,
            vr_id=self.default_vrf,
            dst_ip=sai_ipaddress(self.lpb_ip),
            tunnel_type=tunnel_type,
            action_tunnel_id=tunnel1)

        # p2p tunnel is created when the first evpn type 3 route
        # is received from a remote vtep
        tunnel2 = sai_thrift_create_tunnel(
            self.client,
            type=tunnel_type,
            underlay_interface=self.urif_lpb,
            decap_mappers=decap_maps,
            encap_mappers=encap_maps,
            encap_src_ip=sai_ipaddress(self.lpb_ip),
            peer_mode=SAI_TUNNEL_PEER_MODE_P2P,
            encap_dst_ip=sai_ipaddress(self.tun1_ip))

        # bridge port is created on p2p tunnel
        tun2_bp = sai_thrift_create_bridge_port(
            self.client,
            type=SAI_BRIDGE_PORT_TYPE_TUNNEL,
            tunnel_id=tunnel2,
            bridge_id=self.default_1q_bridge,
            admin_state=True,
            fdb_learning_mode=SAI_BRIDGE_PORT_FDB_LEARNING_MODE_DISABLE)

        # vlan_member is created using tunnel bridge_port
        tun2_vlan_member = sai_thrift_create_vlan_member(
            self.client,
            vlan_id=self.vlan10,
            bridge_port_id=tun2_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)

        # fdb entry for L2 forwarding into VXLAN tunnel
        fdb_entry = sai_thrift_fdb_entry_t(
            bv_id=self.vlan10,
            mac_address=self.customer2_mac,
            switch_id=self.switch_id)
        sai_thrift_create_fdb_entry(self.client,
                                    fdb_entry,
                                    type=SAI_FDB_ENTRY_TYPE_STATIC,
                                    bridge_port_id=tun2_bp)

        # create tunnel map entries for vlan
        decap_tunnel_map_entry_vlan = sai_thrift_create_tunnel_map_entry(
            self.client,
            tunnel_map_type=SAI_TUNNEL_MAP_TYPE_VNI_TO_VLAN_ID,
            tunnel_map=decap_tunnel_map_vlan,
            vlan_id_value=10,
            vni_id_key=self.vni1)

        encap_tunnel_map_entry_vlan = sai_thrift_create_tunnel_map_entry(
            self.client,
            tunnel_map_type=SAI_TUNNEL_MAP_TYPE_VLAN_ID_TO_VNI,
            tunnel_map=encap_tunnel_map_vlan,
            vlan_id_key=10,
            vni_id_value=self.vni2)

        try:
            print("Sending packet from port %d -> VNI %d" %
                  (self.oport_dev, self.vni2))
            send_packet(self, self.oport_dev, self.pkt)
            verify_packet(self, self.vxlan_pkt2, self.uport_dev)
            print("Packet was L2 forwarded into the VXLAN tunnel")
        finally:
            sai_thrift_flush_fdb_entries(
                self.client, entry_type=SAI_FDB_FLUSH_ENTRY_TYPE_ALL)

            sai_thrift_remove_vlan_member(self.client, tun2_vlan_member)
            sai_thrift_remove_bridge_port(self.client, tun2_bp)

            sai_thrift_remove_tunnel(self.client, tunnel2)
            sai_thrift_remove_tunnel_map_entry(
                self.client, encap_tunnel_map_entry_vlan)
            sai_thrift_remove_tunnel_map_entry(
                self.client, decap_tunnel_map_entry_vlan)
            sai_thrift_remove_tunnel_term_table_entry(
                self.client, tunnel_term1)
            sai_thrift_remove_tunnel(self.client, tunnel1)
            sai_thrift_remove_tunnel_map(self.client, encap_tunnel_map_vlan)
            sai_thrift_remove_tunnel_map(self.client, decap_tunnel_map_vlan)


@group('l2-vxlan')
class L2VxLanEncapDecapTest(SaiHelper):
    '''
    This class contains tests for L2 VxLAN encapsulation
    and decapsulation. Encap map entries are created as a
    reverse for decap map entries.
    '''

    def setUp(self):
        super(L2VxLanEncapDecapTest, self).setUp()

        self.oport_dev = self.dev_port0
        self.oport_bp = self.port0_bp
        self.uport_dev = self.dev_port10
        self.uport_rif = self.port10_rif

        self.vni1 = 1000
        self.tun1_ip = "10.0.0.65"
        self.lpb_ip = "10.1.0.32"
        self.tun_ip_mask = "/32"
        self.uport1_myip = "10.0.0.64"
        self.uport_mask = "/31"
        self.customer1_ip = "192.168.100.1"
        self.customer1_ipv6 = "2001:0db8::100:2"
        self.customer2_ip = "192.168.100.8"
        self.customer2_ipv6 = "2001:0db8::100:9"
        self.customer1_mac = "00:22:22:22:22:11"
        self.customer2_mac = "00:22:22:22:22:22"
        self.unbor1_mac = "00:33:33:33:33:11"
        self.unbor2_mac = "00:33:33:33:33:22"

        # underlay loopback rif for tunnels
        self.urif_lpb = sai_thrift_create_router_interface(
            self.client,
            type=SAI_ROUTER_INTERFACE_TYPE_LOOPBACK,
            virtual_router_id=self.default_vrf,
            mtu=9100)

        self.uport1_prefix_route = sai_thrift_route_entry_t(
            destination=sai_ipprefix(self.uport1_myip + self.uport_mask),
            switch_id=self.switch_id,
            vr_id=self.default_vrf)
        sai_thrift_create_route_entry(self.client,
                                      self.uport1_prefix_route,
                                      next_hop_id=self.uport_rif)

        # underlay neighbor
        self.unbor = sai_thrift_neighbor_entry_t(
            ip_address=sai_ipaddress(self.tun1_ip),
            rif_id=self.uport_rif,
            switch_id=self.switch_id)
        sai_thrift_create_neighbor_entry(self.client,
                                         self.unbor,
                                         dst_mac_address=self.unbor1_mac)

        # underlay nexthop
        self.unhop = sai_thrift_create_next_hop(
            self.client,
            type=SAI_NEXT_HOP_TYPE_IP,
            ip=sai_ipaddress(self.tun1_ip),
            router_interface_id=self.uport_rif)

        # create overlay vrf
        self.ovrf = sai_thrift_create_virtual_router(self.client)

        # create fdb entry, neighbor, nexthop for customer 1 on port0, vlan 10
        self.customer1_fdb_entry = sai_thrift_fdb_entry_t(
            bv_id=self.vlan10,
            mac_address=self.customer1_mac,
            switch_id=self.switch_id)
        sai_thrift_create_fdb_entry(self.client,
                                    self.customer1_fdb_entry,
                                    type=SAI_FDB_ENTRY_TYPE_STATIC,
                                    bridge_port_id=self.oport_bp)

        # L2 VxLAN configuration below
        tunnel_type = SAI_TUNNEL_TYPE_VXLAN
        peer_mode = SAI_TUNNEL_PEER_MODE_P2MP
        term_type = SAI_TUNNEL_TERM_TABLE_ENTRY_TYPE_P2MP

        # create tunnel maps
        self.decap_tunnel_map_vlan = sai_thrift_create_tunnel_map(
            self.client, type=SAI_TUNNEL_MAP_TYPE_VNI_TO_VLAN_ID)

        self.encap_tunnel_map_vlan = sai_thrift_create_tunnel_map(
            self.client, type=SAI_TUNNEL_MAP_TYPE_VLAN_ID_TO_VNI)

        # create tunnel
        decap_maps = sai_thrift_object_list_t(
            count=1,
            idlist=[self.decap_tunnel_map_vlan])
        encap_maps = sai_thrift_object_list_t(
            count=1,
            idlist=[self.encap_tunnel_map_vlan])

        self.tunnel1 = sai_thrift_create_tunnel(
            self.client,
            type=tunnel_type,
            underlay_interface=self.urif_lpb,
            decap_mappers=decap_maps,
            encap_mappers=encap_maps,
            encap_src_ip=sai_ipaddress(self.lpb_ip),
            peer_mode=peer_mode)

        self.tunnel_term1 = sai_thrift_create_tunnel_term_table_entry(
            self.client,
            type=term_type,
            vr_id=self.default_vrf,
            dst_ip=sai_ipaddress(self.lpb_ip),
            tunnel_type=tunnel_type,
            action_tunnel_id=self.tunnel1)

        # create tunnel map entries for vlan
        self.decap_tunnel_map_entry_vlan = sai_thrift_create_tunnel_map_entry(
            self.client,
            tunnel_map_type=SAI_TUNNEL_MAP_TYPE_VNI_TO_VLAN_ID,
            tunnel_map=self.decap_tunnel_map_vlan,
            vlan_id_value=10,
            vni_id_key=self.vni1)

        # p2p tunnel is created when the first evpn type 3 route
        # is received from a remote vtep
        self.tunnel2 = sai_thrift_create_tunnel(
            self.client,
            type=tunnel_type,
            underlay_interface=self.urif_lpb,
            decap_mappers=decap_maps,
            encap_mappers=encap_maps,
            encap_src_ip=sai_ipaddress(self.lpb_ip),
            peer_mode=SAI_TUNNEL_PEER_MODE_P2P,
            encap_dst_ip=sai_ipaddress(self.tun1_ip))

        # bridge port is created on p2p tunnel
        self.tun2_bp = sai_thrift_create_bridge_port(
            self.client,
            type=SAI_BRIDGE_PORT_TYPE_TUNNEL,
            tunnel_id=self.tunnel2,
            bridge_id=self.default_1q_bridge,
            admin_state=True,
            fdb_learning_mode=SAI_BRIDGE_PORT_FDB_LEARNING_MODE_DISABLE)

        # vlan_member is created using tunnel bridge_port
        self.tun2_vlan_member = sai_thrift_create_vlan_member(
            self.client,
            vlan_id=self.vlan10,
            bridge_port_id=self.tun2_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)

        # fdb entry for L2 forwarding into VXLAN tunnel
        fdb_entry = sai_thrift_fdb_entry_t(
            bv_id=self.vlan10,
            mac_address=self.customer2_mac,
            switch_id=self.switch_id)
        sai_thrift_create_fdb_entry(self.client,
                                    fdb_entry,
                                    type=SAI_FDB_ENTRY_TYPE_STATIC,
                                    bridge_port_id=self.tun2_bp)

    def runTest(self):
        try:
            self.l2VxLanEncapDecapTest()
            self.l2VxLanEncapLAGTest()
            self.l2VxLanDecapToLAGTest()
            self.l2VxLanDecapFromLAGTest()
            self.l2VxLanEncapECMPTest()
        finally:
            pass

    def tearDown(self):
        sai_thrift_flush_fdb_entries(
            self.client, entry_type=SAI_FDB_FLUSH_ENTRY_TYPE_ALL)

        sai_thrift_remove_vlan_member(self.client, self.tun2_vlan_member)
        sai_thrift_remove_bridge_port(self.client, self.tun2_bp)
        sai_thrift_remove_tunnel(self.client, self.tunnel2)
        sai_thrift_remove_tunnel_map_entry(
            self.client, self.decap_tunnel_map_entry_vlan)
        sai_thrift_remove_tunnel_term_table_entry(
            self.client, self.tunnel_term1)
        sai_thrift_remove_tunnel(self.client, self.tunnel1)
        sai_thrift_remove_tunnel_map(self.client, self.encap_tunnel_map_vlan)
        sai_thrift_remove_tunnel_map(self.client, self.decap_tunnel_map_vlan)

        sai_thrift_remove_virtual_router(self.client, self.ovrf)
        sai_thrift_remove_next_hop(self.client, self.unhop)
        sai_thrift_remove_neighbor_entry(self.client, self.unbor)
        sai_thrift_remove_route_entry(self.client, self.uport1_prefix_route)
        sai_thrift_remove_router_interface(self.client, self.urif_lpb)

        super(L2VxLanEncapDecapTest, self).tearDown()

    def l2VxLanEncapDecapTest(self):
        '''
        This test verifies that packets are properly encapsulated and
        decapsulated in case of L2 VxLAN operations on regular port.
        '''
        print("\nl2VxLanEncapDecapTest()")

        # L2 forwarding into VXLAN tunnel
        pkt = simple_udp_packet(eth_dst=self.customer2_mac,
                                eth_src=self.customer1_mac,
                                ip_dst=self.customer2_ip,
                                ip_src=self.customer1_ip,
                                ip_id=108,
                                ip_ttl=64)
        vxlan_pkt = Mask(
            simple_vxlan_packet(eth_dst=self.unbor1_mac,
                                eth_src=ROUTER_MAC,
                                ip_dst=self.tun1_ip,
                                ip_src=self.lpb_ip,
                                ip_id=0,
                                ip_ttl=64,
                                ip_flags=0x2,
                                with_udp_chksum=False,
                                vxlan_vni=self.vni1,
                                inner_frame=pkt))
        vxlan_pkt.set_do_not_care_scapy(UDP, 'sport')

        print("Sending packet from port %d -> VNI %d" %
              (self.oport_dev, self.vni1))
        send_packet(self, self.oport_dev, pkt)
        verify_packet(self, vxlan_pkt, self.uport_dev)
        print("Packet was L2 forwarded into the VXLAN tunnel")

        print("Sending from LAG")
        print("Sending packet from LAG port %d -> VNI %d" %
              (self.dev_port4, self.vni1))
        send_packet(self, self.dev_port4, pkt)
        verify_packet(self, vxlan_pkt, self.uport_dev)
        print("Packet was L2 forwarded into the VXLAN tunnel")
        print("Sending packet from LAG port %d -> VNI %d" %
              (self.dev_port5, self.vni1))
        send_packet(self, self.dev_port5, pkt)
        verify_packet(self, vxlan_pkt, self.uport_dev)
        print("Packet was L2 forwarded into the VXLAN tunnel")
        print("Sending packet from LAG port %d -> VNI %d" %
              (self.dev_port6, self.vni1))
        send_packet(self, self.dev_port6, pkt)
        verify_packet(self, vxlan_pkt, self.uport_dev)
        print("Packet was L2 forwarded into the VXLAN tunnel")

        # L2 forwarding into VXLAN tunnel IPv6
        pkt_2 = simple_udpv6_packet(eth_dst=self.customer2_mac,
                                    eth_src=self.customer1_mac,
                                    ipv6_dst=self.customer2_ipv6,
                                    ipv6_src=self.customer1_ipv6,
                                    ipv6_hlim=64)
        vxlan_pkt_2 = Mask(
            simple_vxlan_packet(eth_dst=self.unbor1_mac,
                                eth_src=ROUTER_MAC,
                                ip_dst=self.tun1_ip,
                                ip_src=self.lpb_ip,
                                ip_id=0,
                                ip_ttl=64,
                                ip_flags=0x2,
                                with_udp_chksum=False,
                                vxlan_vni=self.vni1,
                                inner_frame=pkt_2))
        vxlan_pkt_2.set_do_not_care_scapy(UDP, 'sport')

        print("Sending packet from port %d -> VNI %d (IPv6)" %
              (self.oport_dev, self.vni1))
        send_packet(self, self.oport_dev, pkt_2)
        verify_packet(self, vxlan_pkt_2, self.uport_dev)
        print("Packet was L2 forwarded into the VXLAN tunnel (IPv6)")

        print("Sending from LAG")
        print("Sending packet from port %d -> VNI %d (IPv6)" %
              (self.dev_port4, self.vni1))
        send_packet(self, self.dev_port4, pkt_2)
        verify_packet(self, vxlan_pkt_2, self.uport_dev)
        print("Packet was L2 forwarded into the VXLAN tunnel (IPv6)")
        print("Sending packet from port %d -> VNI %d (IPv6)" %
              (self.dev_port5, self.vni1))
        send_packet(self, self.dev_port5, pkt_2)
        verify_packet(self, vxlan_pkt_2, self.uport_dev)
        print("Packet was L2 forwarded into the VXLAN tunnel (IPv6)")
        print("Sending packet from port %d -> VNI %d (IPv6)" %
              (self.dev_port6, self.vni1))
        send_packet(self, self.dev_port6, pkt_2)
        verify_packet(self, vxlan_pkt_2, self.uport_dev)
        print("Packet was L2 forwarded into the VXLAN tunnel (IPv6)")

        # L2 decap into vlan
        pkt_3 = simple_udp_packet(eth_dst=self.customer1_mac,
                                  eth_src=self.customer2_mac,
                                  ip_dst=self.customer1_ip,
                                  ip_src=self.customer2_ip,
                                  ip_id=108,
                                  ip_ttl=64)
        vxlan_pkt_3 = simple_vxlan_packet(eth_dst=ROUTER_MAC,
                                          eth_src=self.unbor1_mac,
                                          ip_dst=self.lpb_ip,
                                          ip_src=self.tun1_ip,
                                          ip_id=0,
                                          ip_ttl=64,
                                          ip_flags=0x2,
                                          with_udp_chksum=False,
                                          vxlan_vni=self.vni1,
                                          inner_frame=pkt_3)

        print("Sending packet from VNI %d -> port %d" %
              (self.vni1, self.oport_dev))
        send_packet(self, self.uport_dev, vxlan_pkt_3)
        verify_packet(self, pkt_3, self.oport_dev)
        print("Packet was L2 forwarded out of the VXLAN tunnel")

        # L2 decap into vlan IPv6
        pkt_4 = simple_udpv6_packet(eth_dst=self.customer1_mac,
                                    eth_src=self.customer2_mac,
                                    ipv6_dst=self.customer1_ipv6,
                                    ipv6_src=self.customer2_ipv6,
                                    ipv6_hlim=64)
        vxlan_pkt_4 = simple_vxlan_packet(eth_dst=ROUTER_MAC,
                                          eth_src=self.unbor1_mac,
                                          ip_dst=self.lpb_ip,
                                          ip_src=self.tun1_ip,
                                          ip_id=0,
                                          ip_ttl=64,
                                          ip_flags=0x2,
                                          with_udp_chksum=False,
                                          vxlan_vni=self.vni1,
                                          inner_frame=pkt_4)

        print("Sending packet from VNI %d -> port %d (IPv6)" %
              (self.vni1, self.oport_dev))
        send_packet(self, self.uport_dev, vxlan_pkt_4)
        verify_packet(self, pkt_4, self.oport_dev)
        print("Packet was L2 forwarded out of the VXLAN tunnel (IPv6)")

    def l2VxLanEncapLAGTest(self):
        '''
        This test verifies proper L2 VxLAN encapsulation when nexthop
        to which packets are forwarded is a LAG.
        '''
        print("\nl2VxLanEncapLAGTest()")

        sai_thrift_remove_neighbor_entry(self.client, self.unbor)
        sai_thrift_remove_next_hop(self.client, self.unhop)

        uport_dev_list = [self.dev_port14, self.dev_port15, self.dev_port16]
        self.uport_rif = self.lag3_rif

        # underlay neighbor
        self.unbor = sai_thrift_neighbor_entry_t(
            ip_address=sai_ipaddress(self.tun1_ip),
            rif_id=self.uport_rif,
            switch_id=self.switch_id)
        sai_thrift_create_neighbor_entry(self.client,
                                         self.unbor,
                                         dst_mac_address=self.unbor1_mac)

        # underlay nexthop
        self.unhop = sai_thrift_create_next_hop(
            self.client,
            type=SAI_NEXT_HOP_TYPE_IP,
            ip=sai_ipaddress(self.tun1_ip),
            router_interface_id=self.uport_rif)

        customer1_ip_list = []
        customer1_ipv6_list = []
        itrs = 51
        for i in range(0, itrs):
            customer1_ip_list.append("192.168.101.%d" % (i + 1))
            customer1_ipv6_list.append("2001:0db8::101:%d" % (i + 1))

        try:
            # L2 forwarding into VXLAN tunnel
            count = [0, 0, 0]
            for i in range(0, itrs):
                pkt = simple_udp_packet(eth_dst=self.customer2_mac,
                                        eth_src=self.customer1_mac,
                                        ip_dst=self.customer2_ip,
                                        ip_src=customer1_ip_list[i],
                                        ip_id=108,
                                        ip_ttl=64)
                vxlan_pkt = Mask(
                    simple_vxlan_packet(eth_dst=self.unbor1_mac,
                                        eth_src=ROUTER_MAC,
                                        ip_dst=self.tun1_ip,
                                        ip_src=self.lpb_ip,
                                        ip_id=0,
                                        ip_ttl=64,
                                        ip_flags=0x2,
                                        with_udp_chksum=False,
                                        vxlan_vni=self.vni1,
                                        inner_frame=pkt))
                vxlan_pkt.set_do_not_care_scapy(UDP, 'sport')

                print("Sending packet from port %d -> VNI %d" %
                      (self.oport_dev, self.vni1))
                send_packet(self, self.oport_dev, pkt)
                idx = verify_packet_any_port(
                    self, vxlan_pkt, uport_dev_list)
                count[idx[0]] += 1
                print("Packet was L2 forwarded into the VXLAN tunnel")

            for i in range(0, 3):
                self.assertTrue(count[i] > (itrs / 3) * 0.5)

            # L2 forwarding into VXLAN tunnel IPv6
            count = [0, 0, 0]
            for i in range(0, itrs):
                vxlan_pkt.set_do_not_care_scapy(UDP, 'sport')
                pkt_2 = simple_udpv6_packet(eth_dst=self.customer2_mac,
                                            eth_src=self.customer1_mac,
                                            ipv6_dst=self.customer2_ipv6,
                                            ipv6_src=customer1_ipv6_list[i],
                                            ipv6_hlim=64)
                vxlan_pkt_2 = Mask(
                    simple_vxlan_packet(eth_dst=self.unbor1_mac,
                                        eth_src=ROUTER_MAC,
                                        ip_dst=self.tun1_ip,
                                        ip_src=self.lpb_ip,
                                        ip_id=0,
                                        ip_ttl=64,
                                        ip_flags=0x2,
                                        with_udp_chksum=False,
                                        vxlan_vni=self.vni1,
                                        inner_frame=pkt_2))
                vxlan_pkt_2.set_do_not_care_scapy(UDP, 'sport')

                print("Sending packet from port %d -> VNI %d (IPv6)" %
                      (self.oport_dev, self.vni1))
                send_packet(self, self.oport_dev, pkt_2)
                idx = verify_packet_any_port(
                    self, vxlan_pkt_2, uport_dev_list)
                count[idx[0]] += 1
                print("Packet was L2 forwarded into the VXLAN tunnel (IPv6)")

            for i in range(0, 3):
                self.assertTrue(count[i] > (itrs / 3) * 0.5)

        finally:
            sai_thrift_remove_neighbor_entry(self.client, self.unbor)
            sai_thrift_remove_next_hop(self.client, self.unhop)

            self.uport_dev = self.dev_port10
            self.uport_rif = self.port10_rif

            # underlay neighbor
            self.unbor = sai_thrift_neighbor_entry_t(
                ip_address=sai_ipaddress(self.tun1_ip),
                rif_id=self.uport_rif,
                switch_id=self.switch_id)
            sai_thrift_create_neighbor_entry(self.client,
                                             self.unbor,
                                             dst_mac_address=self.unbor1_mac)

            # underlay nexthop
            self.unhop = sai_thrift_create_next_hop(
                self.client,
                type=SAI_NEXT_HOP_TYPE_IP,
                ip=sai_ipaddress(self.tun1_ip),
                router_interface_id=self.uport_rif)

    def l2VxLanDecapToLAGTest(self):
        '''
        This test verifies proper L2 VxLAN decapsulation when packets
        are being sent to a LAG.
        '''
        print("\nl2VxLanDecapToLAGTest()")

        sai_thrift_remove_fdb_entry(self.client, self.customer1_fdb_entry)

        oport_dev_list = [self.dev_port4, self.dev_port5, self.dev_port6]
        oport_bp = self.lag1_bp

        self.customer1_fdb_entry = sai_thrift_fdb_entry_t(
            bv_id=self.vlan10,
            mac_address=self.customer1_mac,
            switch_id=self.switch_id)
        sai_thrift_create_fdb_entry(self.client,
                                    self.customer1_fdb_entry,
                                    type=SAI_FDB_ENTRY_TYPE_STATIC,
                                    bridge_port_id=oport_bp)

        customer2_ip_list = []
        customer2_ipv6_list = []
        itrs = 30
        for i in range(0, itrs):
            customer2_ip_list.append("192.168.101.%d" % (i + 1))
            customer2_ipv6_list.append("2001:0db8::101:%d" % (i + 1))

        try:
            # L2 forwarding into VXLAN tunnel
            count = [0, 0, 0]
            for i in range(0, itrs):
                pkt = simple_udp_packet(eth_dst=self.customer1_mac,
                                        eth_src=self.customer2_mac,
                                        ip_dst=self.customer1_ip,
                                        ip_src=customer2_ip_list[i],
                                        ip_id=108,
                                        ip_ttl=64)
                vxlan_pkt = simple_vxlan_packet(
                    eth_dst=ROUTER_MAC,
                    eth_src=self.unbor1_mac,
                    ip_dst=self.lpb_ip,
                    ip_src=self.tun1_ip,
                    ip_id=0,
                    ip_ttl=64,
                    ip_flags=0x2,
                    with_udp_chksum=False,
                    vxlan_vni=self.vni1,
                    inner_frame=pkt)

                print("Sending packet from VNI %d -> port %d" %
                      (self.vni1, self.oport_dev))
                send_packet(self, self.uport_dev, vxlan_pkt)
                idx = verify_packet_any_port(
                    self, pkt, oport_dev_list)
                count[idx[0]] += 1
                print("Packet was L2 forwarded into the VXLAN tunnel")

            for i in range(0, 3):
                self.assertTrue(count[i] > (itrs / 3) * 0.5)

            # L2 forwarding into VXLAN tunnel
            count = [0, 0, 0]
            for i in range(0, itrs):
                pkt_2 = simple_udpv6_packet(eth_dst=self.customer1_mac,
                                            eth_src=self.customer2_mac,
                                            ipv6_dst=self.customer1_ipv6,
                                            ipv6_src=customer2_ipv6_list[i],
                                            ipv6_hlim=64)
                vxlan_pkt_2 = simple_vxlan_packet(
                    eth_dst=ROUTER_MAC,
                    eth_src=self.unbor1_mac,
                    ip_dst=self.lpb_ip,
                    ip_src=self.tun1_ip,
                    ip_id=0,
                    ip_ttl=64,
                    ip_flags=0x2,
                    with_udp_chksum=False,
                    vxlan_vni=self.vni1,
                    inner_frame=pkt_2)

                print("Sending packet from VNI %d -> port %d (IPv6)" %
                      (self.vni1, self.oport_dev))
                send_packet(self, self.uport_dev, vxlan_pkt_2)
                idx = verify_packet_any_port(
                    self, pkt_2, oport_dev_list)
                count[idx[0]] += 1
                print("Packet was L2 forwarded into the VXLAN tunnel (IPv6)")

            for i in range(0, 3):
                self.assertTrue(count[i] > (itrs / 3) * 0.5)

        finally:
            sai_thrift_remove_fdb_entry(self.client, self.customer1_fdb_entry)

            self.customer1_fdb_entry = sai_thrift_fdb_entry_t(
                bv_id=self.vlan10,
                mac_address=self.customer1_mac,
                switch_id=self.switch_id)
            sai_thrift_create_fdb_entry(self.client,
                                        self.customer1_fdb_entry,
                                        type=SAI_FDB_ENTRY_TYPE_STATIC,
                                        bridge_port_id=self.oport_bp)

    def l2VxLanDecapFromLAGTest(self):
        '''
        This test verifies proper L2 VxLAN decapsulation when packets
        are being sent from a LAG.
        '''
        print("\nl2VxLanDecapFromLAGTest()")

        sai_thrift_remove_neighbor_entry(self.client, self.unbor)
        sai_thrift_remove_next_hop(self.client, self.unhop)

        uport_dev_list = [self.dev_port14, self.dev_port15, self.dev_port16]
        uport_rif = self.lag3_rif

        # underlay neighbor
        self.unbor = sai_thrift_neighbor_entry_t(
            ip_address=sai_ipaddress(self.tun1_ip),
            rif_id=uport_rif,
            switch_id=self.switch_id)
        sai_thrift_create_neighbor_entry(self.client,
                                         self.unbor,
                                         dst_mac_address=self.unbor1_mac)

        # underlay nexthop
        self.unhop = sai_thrift_create_next_hop(
            self.client,
            type=SAI_NEXT_HOP_TYPE_IP,
            ip=sai_ipaddress(self.tun1_ip),
            router_interface_id=uport_rif)

        try:
            # L2 decap into vlan
            pkt = simple_udp_packet(eth_dst=self.customer1_mac,
                                    eth_src=self.customer2_mac,
                                    ip_dst=self.customer1_ip,
                                    ip_src=self.customer2_ip,
                                    ip_id=108,
                                    ip_ttl=64)
            vxlan_pkt = simple_vxlan_packet(
                eth_dst=ROUTER_MAC,
                eth_src=self.unbor1_mac,
                ip_dst=self.lpb_ip,
                ip_src=self.tun1_ip,
                ip_id=0,
                ip_ttl=64,
                ip_flags=0x2,
                with_udp_chksum=False,
                vxlan_vni=self.vni1,
                inner_frame=pkt)

            print("Sending packet from VNI %d -> port %d" %
                  (self.vni1, self.oport_dev))
            send_packet(self, uport_dev_list[0], vxlan_pkt)
            verify_packet(self, pkt, self.oport_dev)
            print("Packet was L2 forwarded out of VXLAN tunnel")
            print("Sending packet from VNI %d -> port %d" %
                  (self.vni1, self.oport_dev))
            send_packet(self, uport_dev_list[1], vxlan_pkt)
            verify_packet(self, pkt, self.oport_dev)
            print("Packet was L2 forwarded out of VXLAN tunnel")
            print("Sending packet from VNI %d -> port %d" %
                  (self.vni1, self.oport_dev))
            send_packet(self, uport_dev_list[2], vxlan_pkt)
            verify_packet(self, pkt, self.oport_dev)
            print("Packet was L2 forwarded out of VXLAN tunnel")

            pkt_2 = simple_udpv6_packet(eth_dst=self.customer1_mac,
                                        eth_src=self.customer2_mac,
                                        ipv6_dst=self.customer1_ipv6,
                                        ipv6_src=self.customer2_ipv6,
                                        ipv6_hlim=64)
            vxlan_pkt_2 = simple_vxlan_packet(
                eth_dst=ROUTER_MAC,
                eth_src=self.unbor1_mac,
                ip_dst=self.lpb_ip,
                ip_src=self.tun1_ip,
                ip_id=0,
                ip_ttl=64,
                ip_flags=0x2,
                with_udp_chksum=False,
                vxlan_vni=self.vni1,
                inner_frame=pkt_2)

            print("Sending packet from VNI %d -> port %d (IPv6)" %
                  (self.vni1, self.oport_dev))
            send_packet(self, uport_dev_list[0], vxlan_pkt_2)
            verify_packet(self, pkt_2, self.oport_dev)
            print("Packet was L2 forwarded out of VXLAN tunnel (IPv6)")
            print("Sending packet from VNI %d -> port %d (IPv6)" %
                  (self.vni1, self.oport_dev))
            send_packet(self, uport_dev_list[1], vxlan_pkt_2)
            verify_packet(self, pkt_2, self.oport_dev)
            print("Packet was L2 forwarded out of VXLAN tunnel (IPv6)")
            print("Sending packet from VNI %d -> port %d (IPv6)" %
                  (self.vni1, self.oport_dev))
            send_packet(self, uport_dev_list[2], vxlan_pkt_2)
            verify_packet(self, pkt_2, self.oport_dev)
            print("Packet was L2 forwarded out of VXLAN tunnel (IPv6)")

        finally:
            sai_thrift_remove_neighbor_entry(self.client, self.unbor)
            sai_thrift_remove_next_hop(self.client, self.unhop)

            # underlay neighbor
            self.unbor = sai_thrift_neighbor_entry_t(
                ip_address=sai_ipaddress(self.tun1_ip),
                rif_id=self.uport_rif,
                switch_id=self.switch_id)
            sai_thrift_create_neighbor_entry(self.client,
                                             self.unbor,
                                             dst_mac_address=self.unbor1_mac)

            # underlay nexthop
            self.unhop = sai_thrift_create_next_hop(
                self.client,
                type=SAI_NEXT_HOP_TYPE_IP,
                ip=sai_ipaddress(self.tun1_ip),
                router_interface_id=self.uport_rif)

    def l2VxLanEncapECMPTest(self):
        '''
        This test verifies proper L2 VxLAN encapsulation when nexthop
        to which packets are forwarded is a member of ECMP.
        '''
        print("\nl2VxLanEncapECMPTest()")

        sai_thrift_remove_neighbor_entry(self.client, self.unbor)

        uport_dev_list = [self.dev_port10, self.dev_port11]
        uport2_rif = self.port11_rif
        unbor1_ip = "10.0.0.62"
        unbor2_ip = "10.0.0.63"

        # underlay nexthop1
        unhop1 = sai_thrift_create_next_hop(
            self.client,
            type=SAI_NEXT_HOP_TYPE_IP,
            ip=sai_ipaddress(unbor1_ip),
            router_interface_id=self.uport_rif)

        unbor1 = sai_thrift_neighbor_entry_t(
            ip_address=sai_ipaddress(unbor1_ip),
            rif_id=self.uport_rif,
            switch_id=self.switch_id)
        sai_thrift_create_neighbor_entry(self.client,
                                         unbor1,
                                         dst_mac_address=self.unbor1_mac)

        # underlay nexthop2
        unhop2 = sai_thrift_create_next_hop(
            self.client,
            type=SAI_NEXT_HOP_TYPE_IP,
            ip=sai_ipaddress(unbor2_ip),
            router_interface_id=uport2_rif)

        unbor2 = sai_thrift_neighbor_entry_t(
            ip_address=sai_ipaddress(unbor2_ip),
            rif_id=uport2_rif,
            switch_id=self.switch_id)
        sai_thrift_create_neighbor_entry(self.client,
                                         unbor2,
                                         dst_mac_address=self.unbor2_mac)

        # nexthop group
        nhop_group = sai_thrift_create_next_hop_group(
            self.client, type=SAI_NEXT_HOP_GROUP_TYPE_ECMP)

        # nexthop group members
        nhop_group_member1 = sai_thrift_create_next_hop_group_member(
            self.client,
            next_hop_group_id=nhop_group,
            next_hop_id=unhop1)

        nhop_group_member2 = sai_thrift_create_next_hop_group_member(
            self.client,
            next_hop_group_id=nhop_group,
            next_hop_id=unhop2)

        # override self.uport1_prefix_route
        uport_prefix_route = sai_thrift_route_entry_t(
            destination=sai_ipprefix(self.tun1_ip + self.tun_ip_mask),
            switch_id=self.switch_id,
            vr_id=self.default_vrf)
        sai_thrift_create_route_entry(self.client,
                                      uport_prefix_route,
                                      next_hop_id=nhop_group)

        customer1_ip_list = []
        customer1_ipv6_list = []
        itrs = 30
        for i in range(0, itrs):
            customer1_ip_list.append("192.168.1.%d" % (i + 1))
            customer1_ipv6_list.append("2001:0db8::1101:%d" % (i + 1))

        try:
            # L2 forwarding into VXLAN tunnel
            count = [0, 0]
            for i in range(0, itrs):
                pkt = simple_udp_packet(eth_dst=self.customer2_mac,
                                        eth_src=self.customer1_mac,
                                        ip_dst=self.customer2_ip,
                                        ip_src=customer1_ip_list[i],
                                        ip_id=108,
                                        ip_ttl=64)
                vxlan_pkt_1 = Mask(
                    simple_vxlan_packet(eth_dst=self.unbor1_mac,
                                        eth_src=ROUTER_MAC,
                                        ip_dst=self.tun1_ip,
                                        ip_src=self.lpb_ip,
                                        ip_id=0,
                                        ip_ttl=64,
                                        ip_flags=0x2,
                                        with_udp_chksum=False,
                                        vxlan_vni=self.vni1,
                                        inner_frame=pkt))
                vxlan_pkt_1.set_do_not_care_scapy(UDP, 'sport')
                vxlan_pkt_2 = Mask(
                    simple_vxlan_packet(eth_dst=self.unbor2_mac,
                                        eth_src=ROUTER_MAC,
                                        ip_dst=self.tun1_ip,
                                        ip_src=self.lpb_ip,
                                        ip_id=0,
                                        ip_ttl=64,
                                        ip_flags=0x2,
                                        with_udp_chksum=False,
                                        vxlan_vni=self.vni1,
                                        inner_frame=pkt))
                vxlan_pkt_2.set_do_not_care_scapy(UDP, 'sport')

                print("Sending packet from port %d -> VNI %d" %
                      (self.oport_dev, self.vni1))
                send_packet(self, self.oport_dev, pkt)
                rcv_idx = verify_any_packet_any_port(
                    self, [vxlan_pkt_1, vxlan_pkt_2], uport_dev_list)
                verify_no_other_packets(self)
                count[rcv_idx] += 1
                print("Packet was L2 forwarded into the VXLAN tunnel")

            print(count)
            for i in range(0, 2):
                self.assertTrue(count[i] > (itrs / 2) * 0.5)

            # L2 forwarding into VXLAN tunnel IPv6
            count = [0, 0]
            for i in range(0, itrs):
                pkt = simple_udpv6_packet(eth_dst=self.customer2_mac,
                                          eth_src=self.customer1_mac,
                                          ipv6_dst=self.customer2_ipv6,
                                          ipv6_src=customer1_ipv6_list[i],
                                          ipv6_hlim=64)
                vxlan_pkt_1 = Mask(
                    simple_vxlan_packet(eth_dst=self.unbor1_mac,
                                        eth_src=ROUTER_MAC,
                                        ip_dst=self.tun1_ip,
                                        ip_src=self.lpb_ip,
                                        ip_id=0,
                                        ip_ttl=64,
                                        ip_flags=0x2,
                                        with_udp_chksum=False,
                                        vxlan_vni=self.vni1,
                                        inner_frame=pkt))
                vxlan_pkt_1.set_do_not_care_scapy(UDP, 'sport')
                vxlan_pkt_2 = Mask(
                    simple_vxlan_packet(eth_dst=self.unbor2_mac,
                                        eth_src=ROUTER_MAC,
                                        ip_dst=self.tun1_ip,
                                        ip_src=self.lpb_ip,
                                        ip_id=0,
                                        ip_ttl=64,
                                        ip_flags=0x2,
                                        with_udp_chksum=False,
                                        vxlan_vni=self.vni1,
                                        inner_frame=pkt))
                vxlan_pkt_2.set_do_not_care_scapy(UDP, 'sport')

                print("Sending packet from port %d -> VNI %d (IPv6)" %
                      (self.oport_dev, self.vni1))
                send_packet(self, self.oport_dev, pkt)
                rcv_idx = verify_any_packet_any_port(
                    self, [vxlan_pkt_1, vxlan_pkt_2], uport_dev_list)
                count[rcv_idx] += 1
                print("Packet was L2 forwarded into the VXLAN tunnel (IPv6)")

            print(count)
            for i in range(0, 2):
                self.assertTrue(count[i] > (itrs / 2) * 0.5)

        finally:
            sai_thrift_remove_route_entry(self.client, uport_prefix_route)
            sai_thrift_remove_next_hop_group_member(self.client,
                                                    nhop_group_member1)
            sai_thrift_remove_next_hop_group_member(self.client,
                                                    nhop_group_member2)
            sai_thrift_remove_next_hop_group(self.client, nhop_group)
            sai_thrift_remove_next_hop(self.client, unhop1)
            sai_thrift_remove_neighbor_entry(self.client, unbor1)
            sai_thrift_remove_next_hop(self.client, unhop2)
            sai_thrift_remove_neighbor_entry(self.client, unbor2)

            # underlay neighbor
            self.unbor = sai_thrift_neighbor_entry_t(
                ip_address=sai_ipaddress(self.tun1_ip),
                rif_id=self.uport_rif,
                switch_id=self.switch_id)
            sai_thrift_create_neighbor_entry(self.client,
                                             self.unbor,
                                             dst_mac_address=self.unbor1_mac)


@group('l2-vxlan')
@group('mclag')
class MCLagVxLanTest(L2VxLanEncapDecapTest):
    '''
    This class contains tests for MC-Lag peer_link to L2 VxLAN encapsulation
    Traffic from peer_link should not be encapsulated in to any tunnel.
    '''

    def setUp(self):
        super(MCLagVxLanTest, self).setUp()

        self.customer3_mac = "00:33:33:33:33:33"
        self.unknown_mac = "00:77:77:77:77:77"

        # fdb entry for L2 forwarding into VXLAN tunnel
        fdb_entry = sai_thrift_fdb_entry_t(
            bv_id=self.vlan10,
            mac_address=self.customer3_mac,
            switch_id=self.switch_id)
        sai_thrift_create_fdb_entry(self.client,
                                    fdb_entry,
                                    type=SAI_FDB_ENTRY_TYPE_STATIC,
                                    bridge_port_id=self.lag1_bp)

        self.isol_group = sai_thrift_create_isolation_group(
            self.client, type=SAI_ISOLATION_GROUP_TYPE_BRIDGE_PORT)

        self.isol_group_mbr_lag1 = sai_thrift_create_isolation_group_member(
            self.client,
            isolation_group_id=self.isol_group,
            isolation_object=self.lag1_bp)

        self.isol_group2 = sai_thrift_create_isolation_group(
            self.client, type=SAI_ISOLATION_GROUP_TYPE_BRIDGE_PORT)

        self.isol_group2_mbr_port1 = sai_thrift_create_isolation_group_member(
            self.client,
            isolation_group_id=self.isol_group2,
            isolation_object=self.port1_bp)

        # unicast pkt to tunnel
        self.pkt = simple_udp_packet(eth_dst=self.customer2_mac,
                                     eth_src=self.customer1_mac,
                                     ip_dst=self.customer2_ip,
                                     ip_src=self.customer1_ip,
                                     ip_id=108,
                                     ip_ttl=64)
        self.vxlan_pkt = Mask(
            simple_vxlan_packet(eth_dst=self.unbor1_mac,
                                eth_src=ROUTER_MAC,
                                ip_dst=self.tun1_ip,
                                ip_src=self.lpb_ip,
                                ip_id=0,
                                ip_ttl=64,
                                ip_flags=0x2,
                                with_udp_chksum=False,
                                vxlan_vni=self.vni1,
                                inner_frame=self.pkt))
        self.vxlan_pkt.set_do_not_care_scapy(UDP, 'sport')

        # unicast pkt to lag1
        self.pkt_lag1 = simple_udp_packet(eth_dst=self.customer3_mac,
                                          eth_src=self.customer1_mac,
                                          ip_dst=self.customer2_ip,
                                          ip_src=self.customer1_ip,
                                          ip_id=108,
                                          ip_ttl=64)

        # pkt to flood to all members
        self.pkt_flood = simple_udp_packet(eth_dst=self.unknown_mac,
                                           eth_src=self.customer1_mac,
                                           ip_dst=self.customer2_ip,
                                           ip_src=self.customer1_ip,
                                           ip_id=108,
                                           ip_ttl=64)
        self.vxlan_pkt_flood = Mask(
            simple_vxlan_packet(eth_dst=self.unbor1_mac,
                                eth_src=ROUTER_MAC,
                                ip_dst=self.tun1_ip,
                                ip_src=self.lpb_ip,
                                ip_id=0,
                                ip_ttl=64,
                                ip_flags=0x2,
                                with_udp_chksum=False,
                                vxlan_vni=self.vni1,
                                inner_frame=self.pkt_flood))
        self.vxlan_pkt_flood.set_do_not_care_scapy(UDP, 'sport')

        self.tagged_pkt = simple_udp_packet(eth_dst=self.unknown_mac,
                                            eth_src=self.customer1_mac,
                                            ip_dst=self.customer2_ip,
                                            ip_src=self.customer1_ip,
                                            vlan_vid=10,
                                            dl_vlan_enable=True,
                                            ip_id=108,
                                            ip_ttl=64,
                                            pktlen=104)

        attr_value = sai_thrift_attribute_value_t(booldata=True)
        self.attr_true = sai_thrift_attribute_t(
            id=(SAI_BRIDGE_PORT_ATTR_END),
            value=attr_value)

        attr_value = sai_thrift_attribute_value_t(booldata=False)
        self.attr_false = sai_thrift_attribute_t(
            id=(SAI_BRIDGE_PORT_ATTR_END),
            value=attr_value)

    def runTest(self):
        try:
            self.setIsolationGroupFirstTest()
            self.setCustomAttrFirstTest()
            self.changeIsolationGroupTest()
            self.createISLBridgePortTest()
        finally:
            pass

    def tearDown(self):
        attr_value = sai_thrift_attribute_value_t(booldata=False)
        attr = sai_thrift_attribute_t(
            id=(SAI_BRIDGE_PORT_ATTR_END),
            value=attr_value)

        status = sai_thrift_set_bridge_port_attribute(
            self.client,
            self.port0_bp,
            custom_attribute=attr)

        status = sai_thrift_set_bridge_port_attribute(
            self.client,
            self.port0_bp,
            isolation_group=int(SAI_NULL_OBJECT_ID))
        self.assertEqual(status, SAI_STATUS_SUCCESS)

        sai_thrift_remove_isolation_group_member(self.client,
                                                 self.isol_group_mbr_lag1)
        sai_thrift_remove_isolation_group_member(self.client,
                                                 self.isol_group2_mbr_port1)

        sai_thrift_remove_isolation_group(self.client, self.isol_group)
        sai_thrift_remove_isolation_group(self.client, self.isol_group2)

        super(MCLagVxLanTest, self).tearDown()

    def setIsolationGroupFirstTest(self):
        '''
        This test verifies that packets are not encapsulated
        in to tunnel if it is received on peer link.
        Bridge port isolation group is set before custom attr.
        '''
        print("\nsetIsolationGroupFirstTest()")
        try:
            # unicast pkt into VXLAN tunnel
            print("Sending packet from port %d -> VNI %d" %
                  (self.oport_dev, self.vni1))
            send_packet(self, self.oport_dev, self.pkt)
            verify_packet(self, self.vxlan_pkt, self.uport_dev)

            # unicast pkt to lag1
            print("Sending packet from port %d -> LAG1(port %d,%d,%d)" %
                  (self.oport_dev,
                   self.dev_port4,
                   self.dev_port5,
                   self.dev_port6))
            send_packet(self, self.oport_dev, self.pkt_lag1)
            verify_packets_any(
                self,
                self.pkt_lag1,
                [self.dev_port4, self.dev_port5, self.dev_port6])

            # flood pkt to all members
            print("Sending packet from port %d -> VLAN10. Will flood" %
                  (self.oport_dev))
            send_packet(self, self.oport_dev, self.pkt_flood)
            verify_each_packet_on_multiple_port_lists(
                self,
                [self.vxlan_pkt_flood, self.pkt_flood, self.tagged_pkt],
                [[self.dev_port10],
                 [self.dev_port4, self.dev_port5, self.dev_port6],
                 [self.dev_port1]])

            print("Bind isolation group to port %d" % (self.oport_dev))
            status = sai_thrift_set_bridge_port_attribute(
                self.client,
                self.port0_bp,
                isolation_group=self.isol_group)
            self.assertEqual(status, SAI_STATUS_SUCCESS)

            # unicast pkt into VXLAN tunnel
            print("Sending packet from port %d -> VNI %d" %
                  (self.oport_dev, self.vni1))
            send_packet(self, self.oport_dev, self.pkt)
            verify_packet(self, self.vxlan_pkt, self.uport_dev)

            # unicast pkt to lag1
            print("Sending packet from port %d -> LAG1(port %d,%d,%d)" %
                  (self.oport_dev,
                   self.dev_port4,
                   self.dev_port5,
                   self.dev_port6))
            send_packet(self, self.oport_dev, self.pkt_lag1)
            verify_no_other_packets(self)

            # flood pkt to all members
            print("Sending packet from port %d -> VLAN10. Will flood" %
                  (self.oport_dev))
            send_packet(self, self.oport_dev, self.pkt_flood)
            verify_each_packet_on_multiple_port_lists(
                self,
                [self.vxlan_pkt_flood, self.tagged_pkt],
                [[self.dev_port10], [self.dev_port1]])

            print("Set custom bridge_port attr on port %d" % (self.oport_dev))
            status = sai_thrift_set_bridge_port_attribute(
                self.client,
                self.port0_bp,
                custom_attribute=self.attr_true)

            # unicast pkt into VXLAN tunnel
            print("Sending packet from port %d -> VNI %d" %
                  (self.oport_dev, self.vni1))
            send_packet(self, self.oport_dev, self.pkt)
            verify_no_other_packets(self)

            # unicast pkt to lag1
            print("Sending packet from port %d -> LAG1(port %d,%d,%d)" %
                  (self.oport_dev,
                   self.dev_port4,
                   self.dev_port5,
                   self.dev_port6))
            send_packet(self, self.oport_dev, self.pkt_lag1)
            verify_no_other_packets(self)

            # flood pkt to all members
            print("Sending packet from port %d -> VLAN10. Will flood" %
                  (self.oport_dev))
            send_packet(self, self.oport_dev, self.pkt_flood)
            verify_packet(self, self.tagged_pkt, self.dev_port1)

            print("Set back custom bridge_port attr on port %d" %
                  (self.oport_dev))
            status = sai_thrift_set_bridge_port_attribute(
                self.client,
                self.port0_bp,
                custom_attribute=self.attr_false)

            # unicast pkt into VXLAN tunnel
            print("Sending packet from port %d -> VNI %d" %
                  (self.oport_dev, self.vni1))
            send_packet(self, self.oport_dev, self.pkt)
            verify_packet(self, self.vxlan_pkt, self.uport_dev)

            # unicast pkt to lag1
            print("Sending packet from port %d -> LAG1(port %d,%d,%d)" %
                  (self.oport_dev,
                   self.dev_port4,
                   self.dev_port5,
                   self.dev_port6))
            send_packet(self, self.oport_dev, self.pkt_lag1)
            verify_no_other_packets(self)

            # flood pkt to all members
            print("Sending packet from port %d -> VLAN10. Will flood" %
                  (self.oport_dev))
            send_packet(self, self.oport_dev, self.pkt_flood)
            verify_each_packet_on_multiple_port_lists(
                self,
                [self.vxlan_pkt_flood, self.tagged_pkt],
                [[self.dev_port10], [self.dev_port1]])

            status = sai_thrift_set_bridge_port_attribute(
                self.client,
                self.port0_bp,
                isolation_group=int(SAI_NULL_OBJECT_ID))
            self.assertEqual(status, SAI_STATUS_SUCCESS)

            # unicast pkt into VXLAN tunnel
            print("Sending packet from port %d -> VNI %d" %
                  (self.oport_dev, self.vni1))
            send_packet(self, self.oport_dev, self.pkt)
            verify_packet(self, self.vxlan_pkt, self.uport_dev)

            # unicast pkt to lag1
            print("Sending packet from port %d -> LAG1(port %d,%d,%d)" %
                  (self.oport_dev,
                   self.dev_port4,
                   self.dev_port5,
                   self.dev_port6))
            send_packet(self, self.oport_dev, self.pkt_lag1)
            verify_packets_any(
                self,
                self.pkt_lag1,
                [self.dev_port4, self.dev_port5, self.dev_port6])

            # flood pkt to all members
            print("Sending packet from port %d -> VLAN10. Will flood" %
                  (self.oport_dev))
            send_packet(self, self.oport_dev, self.pkt_flood)
            verify_each_packet_on_multiple_port_lists(
                self, [self.vxlan_pkt_flood, self.pkt_flood, self.tagged_pkt],
                [[self.dev_port10],
                 [self.dev_port4, self.dev_port5, self.dev_port6],
                 [self.dev_port1]])

        finally:
            status = sai_thrift_set_bridge_port_attribute(
                self.client,
                self.port0_bp,
                custom_attribute=self.attr_false)

            status = sai_thrift_set_bridge_port_attribute(
                self.client,
                self.port0_bp,
                isolation_group=int(SAI_NULL_OBJECT_ID))
            self.assertEqual(status, SAI_STATUS_SUCCESS)

    def setCustomAttrFirstTest(self):
        '''
        This test verifies that packets are not encapsulated
        in to tunnel if it is received on peer link.
        Bridge port custom attr is set before isolation group.
        '''
        print("\nsetCustomAttrFirstTest()")

        try:
            # unicast pkt into VXLAN tunnel
            print("Sending packet from port %d -> VNI %d" %
                  (self.oport_dev, self.vni1))
            send_packet(self, self.oport_dev, self.pkt)
            verify_packet(self, self.vxlan_pkt, self.uport_dev)

            # unicast pkt to lag1
            print("Sending packet from port %d -> LAG1(port %d,%d,%d)" %
                  (self.oport_dev,
                   self.dev_port4,
                   self.dev_port5,
                   self.dev_port6))
            send_packet(self, self.oport_dev, self.pkt_lag1)
            verify_packets_any(
                self,
                self.pkt_lag1,
                [self.dev_port4, self.dev_port5, self.dev_port6])

            # flood pkt to all members
            print("Sending packet from port %d -> VLAN10. Will flood" %
                  (self.oport_dev))
            send_packet(self, self.oport_dev, self.pkt_flood)
            verify_each_packet_on_multiple_port_lists(
                self,
                [self.vxlan_pkt_flood, self.pkt_flood, self.tagged_pkt],
                [[self.dev_port10],
                 [self.dev_port4, self.dev_port5, self.dev_port6],
                 [self.dev_port1]])

            print("Set custom bridge_port attr on port %d" % (self.oport_dev))
            status = sai_thrift_set_bridge_port_attribute(
                self.client,
                self.port0_bp,
                custom_attribute=self.attr_true)

            # unicast pkt into VXLAN tunnel
            print("Sending packet from port %d -> VNI %d" %
                  (self.oport_dev, self.vni1))
            send_packet(self, self.oport_dev, self.pkt)
            verify_packet(self, self.vxlan_pkt, self.uport_dev)

            # unicast pkt to lag1
            print("Sending packet from port %d -> LAG1(port %d,%d,%d)" %
                  (self.oport_dev,
                   self.dev_port4,
                   self.dev_port5,
                   self.dev_port6))
            send_packet(self, self.oport_dev, self.pkt_lag1)
            verify_packets_any(
                self,
                self.pkt_lag1,
                [self.dev_port4, self.dev_port5, self.dev_port6])

            # flood pkt to all members
            print("Sending packet from port %d -> VLAN10. Will flood" %
                  (self.oport_dev))
            send_packet(self, self.oport_dev, self.pkt_flood)
            verify_each_packet_on_multiple_port_lists(
                self,
                [self.vxlan_pkt_flood, self.pkt_flood, self.tagged_pkt],
                [[self.dev_port10],
                 [self.dev_port4, self.dev_port5, self.dev_port6],
                 [self.dev_port1]])

            print("Bind isolation group to port %d" % (self.oport_dev))
            status = sai_thrift_set_bridge_port_attribute(
                self.client,
                self.port0_bp,
                isolation_group=self.isol_group)
            self.assertEqual(status, SAI_STATUS_SUCCESS)

            # unicast pkt into VXLAN tunnel
            print("Sending packet from port %d -> VNI %d" %
                  (self.oport_dev, self.vni1))
            send_packet(self, self.oport_dev, self.pkt)
            verify_no_other_packets(self)

            # unicast pkt to lag1
            print("Sending packet from port %d -> LAG1(port %d,%d,%d)" %
                  (self.oport_dev,
                   self.dev_port4,
                   self.dev_port5,
                   self.dev_port6))
            send_packet(self, self.oport_dev, self.pkt_lag1)
            verify_no_other_packets(self)

            # flood pkt to all members
            print("Sending packet from port %d -> VLAN10. Will flood" %
                  (self.oport_dev))
            send_packet(self, self.oport_dev, self.pkt_flood)
            verify_packet(self, self.tagged_pkt, self.dev_port1)

            print("Unbind isolation group to port %d" % (self.oport_dev))
            status = sai_thrift_set_bridge_port_attribute(
                self.client,
                self.port0_bp,
                isolation_group=int(SAI_NULL_OBJECT_ID))
            self.assertEqual(status, SAI_STATUS_SUCCESS)

            # unicast pkt into VXLAN tunnel
            print("Sending packet from port %d -> VNI %d" %
                  (self.oport_dev, self.vni1))
            send_packet(self, self.oport_dev, self.pkt)
            verify_packet(self, self.vxlan_pkt, self.uport_dev)

            # unicast pkt to lag1
            print("Sending packet from port %d -> LAG1(port %d,%d,%d)" %
                  (self.oport_dev,
                   self.dev_port4,
                   self.dev_port5,
                   self.dev_port6))
            send_packet(self, self.oport_dev, self.pkt_lag1)
            verify_packets_any(
                self,
                self.pkt_lag1,
                [self.dev_port4, self.dev_port5, self.dev_port6])

            # flood pkt to all members
            print("Sending packet from port %d -> VLAN10. Will flood" %
                  (self.oport_dev))
            send_packet(self, self.oport_dev, self.pkt_flood)
            verify_each_packet_on_multiple_port_lists(
                self,
                [self.vxlan_pkt_flood, self.pkt_flood, self.tagged_pkt],
                [[self.dev_port10],
                 [self.dev_port4, self.dev_port5, self.dev_port6],
                 [self.dev_port1]])

            print("Set back custom bridge_port attr on port %d" %
                  (self.oport_dev))
            status = sai_thrift_set_bridge_port_attribute(
                self.client,
                self.port0_bp,
                custom_attribute=self.attr_false)

            # unicast pkt into VXLAN tunnel
            print("Sending packet from port %d -> VNI %d" %
                  (self.oport_dev, self.vni1))
            send_packet(self, self.oport_dev, self.pkt)
            verify_packet(self, self.vxlan_pkt, self.uport_dev)

            # unicast pkt to lag1
            print("Sending packet from port %d -> LAG1(port %d,%d,%d)" %
                  (self.oport_dev,
                   self.dev_port4,
                   self.dev_port5,
                   self.dev_port6))
            send_packet(self, self.oport_dev, self.pkt_lag1)
            verify_packets_any(
                self,
                self.pkt_lag1,
                [self.dev_port4, self.dev_port5, self.dev_port6])

            # flood pkt to all members
            print("Sending packet from port %d -> VLAN10. Will flood" %
                  (self.oport_dev))
            send_packet(self, self.oport_dev, self.pkt_flood)
            verify_each_packet_on_multiple_port_lists(
                self,
                [self.vxlan_pkt_flood, self.pkt_flood, self.tagged_pkt],
                [[self.dev_port10],
                 [self.dev_port4, self.dev_port5, self.dev_port6],
                 [self.dev_port1]])

        finally:
            status = sai_thrift_set_bridge_port_attribute(
                self.client,
                self.port0_bp,
                custom_attribute=self.attr_false)

            status = sai_thrift_set_bridge_port_attribute(
                self.client,
                self.port0_bp,
                isolation_group=int(SAI_NULL_OBJECT_ID))
            self.assertEqual(status, SAI_STATUS_SUCCESS)

    def changeIsolationGroupTest(self):
        '''
        This test verifies that packets are not encapsulated
        in to tunnel if it is received on peer link.
        Bridge port isolation group and custom attr are set,
        Than isolation group is change to another non zero value.
        '''
        print("\nchangeIsolationGroupTest()")
        try:
            # unicast pkt into VXLAN tunnel
            print("Sending packet from port %d -> VNI %d" %
                  (self.oport_dev, self.vni1))
            send_packet(self, self.oport_dev, self.pkt)
            verify_packet(self, self.vxlan_pkt, self.uport_dev)

            # unicast pkt to lag1
            print("Sending packet from port %d -> LAG1(port %d,%d,%d)" %
                  (self.oport_dev,
                   self.dev_port4,
                   self.dev_port5,
                   self.dev_port6))
            send_packet(self, self.oport_dev, self.pkt_lag1)
            verify_packets_any(
                self,
                self.pkt_lag1,
                [self.dev_port4, self.dev_port5, self.dev_port6])

            # flood pkt to all members
            print("Sending packet from port %d -> VLAN10. Will flood" %
                  (self.oport_dev))
            send_packet(self, self.oport_dev, self.pkt_flood)
            verify_each_packet_on_multiple_port_lists(
                self,
                [self.vxlan_pkt_flood, self.pkt_flood, self.tagged_pkt],
                [[self.dev_port10],
                 [self.dev_port4, self.dev_port5, self.dev_port6],
                 [self.dev_port1]])

            print("Set custom bridge_port attr on port %d" % (self.oport_dev))
            status = sai_thrift_set_bridge_port_attribute(
                self.client,
                self.port0_bp,
                custom_attribute=self.attr_true)

            print("Bind isolation group to port %d" % (self.oport_dev))
            status = sai_thrift_set_bridge_port_attribute(
                self.client,
                self.port0_bp,
                isolation_group=self.isol_group)
            self.assertEqual(status, SAI_STATUS_SUCCESS)

            # unicast pkt into VXLAN tunnel
            print("Sending packet from port %d -> VNI %d" %
                  (self.oport_dev, self.vni1))
            send_packet(self, self.oport_dev, self.pkt)
            verify_no_other_packets(self)

            # unicast pkt to lag1
            print("Sending packet from port %d -> LAG1(port %d,%d,%d)" %
                  (self.oport_dev,
                   self.dev_port4,
                   self.dev_port5,
                   self.dev_port6))
            send_packet(self, self.oport_dev, self.pkt_lag1)
            verify_no_other_packets(self)

            # flood pkt to all members
            print("Sending packet from port %d -> VLAN10. Will flood" %
                  (self.oport_dev))
            send_packet(self, self.oport_dev, self.pkt_flood)
            verify_packet(self, self.tagged_pkt, self.dev_port1)
            verify_no_other_packets(self)

            print("Bind another isolation group to port %d" % (self.oport_dev))
            status = sai_thrift_set_bridge_port_attribute(
                self.client,
                self.port0_bp,
                isolation_group=self.isol_group2)
            self.assertEqual(status, SAI_STATUS_SUCCESS)

            # unicast pkt into VXLAN tunnel
            print("Sending packet from port %d -> VNI %d" %
                  (self.oport_dev, self.vni1))
            send_packet(self, self.oport_dev, self.pkt)
            verify_no_other_packets(self)

            # unicast pkt to lag1
            print("Sending packet from port %d -> LAG1(port %d,%d,%d)" %
                  (self.oport_dev,
                   self.dev_port4,
                   self.dev_port5,
                   self.dev_port6))
            send_packet(self, self.oport_dev, self.pkt_lag1)
            verify_packets_any(
                self,
                self.pkt_lag1,
                [self.dev_port4, self.dev_port5, self.dev_port6])

            # flood pkt to all members
            print("Sending packet from port %d -> VLAN10. Will flood" %
                  (self.oport_dev))
            send_packet(self, self.oport_dev, self.pkt_flood)
            verify_packets_any(
                self,
                self.pkt_flood,
                [self.dev_port4, self.dev_port5, self.dev_port6])

        finally:
            status = sai_thrift_set_bridge_port_attribute(
                self.client,
                self.port0_bp,
                custom_attribute=self.attr_false)

            status = sai_thrift_set_bridge_port_attribute(
                self.client,
                self.port0_bp,
                isolation_group=int(SAI_NULL_OBJECT_ID))
            self.assertEqual(status, SAI_STATUS_SUCCESS)

    def createISLBridgePortTest(self):
        '''
        This test verifies that packets are not encapsulated
        in to tunnel if it is received on peer link.
        Bridge port isolation group and custom attr are set on create.
        '''
        print("\ncreateISLBridgePortTest()")
        # create bridge port
        port24_bp = sai_thrift_create_bridge_port(
            self.client,
            bridge_id=self.default_1q_bridge,
            port_id=self.port24,
            type=SAI_BRIDGE_PORT_TYPE_PORT,
            admin_state=True,
            isolation_group=self.isol_group,
            custom_attribute=self.attr_true)
        self.assertTrue(port24_bp != 0)

        sai_thrift_set_port_attribute(self.client,
                                      self.port24,
                                      port_vlan_id=10)

        vlan10_member3 = sai_thrift_create_vlan_member(
            self.client,
            vlan_id=self.vlan10,
            bridge_port_id=port24_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)

        try:
            # unicast pkt into VXLAN tunnel
            print("Sending packet from port %d -> VNI %d" %
                  (self.dev_port24, self.vni1))
            send_packet(self, self.dev_port24, self.pkt)
            verify_no_other_packets(self)

            # unicast pkt to lag1
            print("Sending packet from port %d -> LAG1(port %d,%d,%d)" %
                  (self.dev_port24,
                   self.dev_port4,
                   self.dev_port5,
                   self.dev_port6))
            send_packet(self, self.dev_port24, self.pkt_lag1)
            verify_no_other_packets(self)

            # flood pkt to all members
            print("Sending packet from port %d -> VLAN10. Will flood" %
                  (self.dev_port24))
            send_packet(self, self.dev_port24, self.pkt_flood)
            verify_each_packet_on_multiple_port_lists(
                self, [self.pkt_flood, self.tagged_pkt],
                [[self.dev_port0],
                 [self.dev_port1]])

            print("Bind another isolation group to port %d" % (self.oport_dev))
            status = sai_thrift_set_bridge_port_attribute(
                self.client,
                port24_bp,
                isolation_group=self.isol_group2)
            self.assertEqual(status, SAI_STATUS_SUCCESS)

            # unicast pkt into VXLAN tunnel
            print("Sending packet from port %d -> VNI %d" %
                  (self.dev_port24, self.vni1))
            send_packet(self, self.dev_port24, self.pkt)
            verify_no_other_packets(self)

            # unicast pkt to lag1
            print("Sending packet from port %d -> LAG1(port %d,%d,%d)" %
                  (self.dev_port24,
                   self.dev_port4,
                   self.dev_port5,
                   self.dev_port6))
            send_packet(self, self.dev_port24, self.pkt_lag1)
            verify_packets_any(
                self,
                self.pkt_lag1,
                [self.dev_port4, self.dev_port5, self.dev_port6])

            # flood pkt to all members
            print("Sending packet from port %d -> VLAN10. Will flood" %
                  (self.dev_port24))
            send_packet(self, self.dev_port24, self.pkt_flood)
            verify_each_packet_on_multiple_port_lists(
                self,
                [self.pkt_flood, self.pkt_flood],
                [[self.dev_port0],
                 [self.dev_port4, self.dev_port5, self.dev_port6]])
        finally:
            status = sai_thrift_set_bridge_port_attribute(
                self.client,
                port24_bp,
                isolation_group=int(SAI_NULL_OBJECT_ID))
            self.assertEqual(status, SAI_STATUS_SUCCESS)

            sai_thrift_set_port_attribute(
                self.client,
                self.port24,
                port_vlan_id=0)
            if port24_bp != 0:
                sai_thrift_remove_bridge_port(self.client, port24_bp)
            if vlan10_member3 != 0:
                sai_thrift_remove_vlan_member(self.client, vlan10_member3)

    def makeBeforeBreakTest(self):
        '''
        This test verifies that packets are not encapsulated
        in to tunnel if it is received on peer link.
        Bridge port isolation group and custom attr are set on create.
        '''
        print("\ncreateISLBridgePortTest()")
        # create bridge port
        port24_bp = sai_thrift_create_bridge_port(
            self.client,
            bridge_id=self.default_1q_bridge,
            port_id=self.port24,
            type=SAI_BRIDGE_PORT_TYPE_PORT,
            admin_state=True,
            isolation_group=self.isol_group,
            custom_attribute=self.attr_true)
        self.assertTrue(port24_bp != 0)

        sai_thrift_set_port_attribute(
            self.client, self.port24, port_vlan_id=10)

        vlan10_member3 = sai_thrift_create_vlan_member(
            self.client,
            vlan_id=self.vlan10,
            bridge_port_id=port24_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)

        try:
            # check traffic for current peer link bridge_port
            # unicast pkt into VXLAN tunnel
            print("Sending packet from port %d -> VNI %d" %
                  (self.dev_port24, self.vni1))
            send_packet(self, self.dev_port24, self.pkt)
            verify_no_other_packets(self)

            # unicast pkt to lag1
            print("Sending packet from port %d -> LAG1(port %d,%d,%d)" %
                  (self.dev_port24,
                   self.dev_port4,
                   self.dev_port5,
                   self.dev_port6))
            send_packet(self, self.dev_port24, self.pkt_lag1)
            verify_no_other_packets(self)

            # flood pkt to all members
            print("Sending packet from port %d -> VLAN10. Will flood" %
                  (self.dev_port24))
            send_packet(self, self.dev_port24, self.pkt_flood)
            verify_each_packet_on_multiple_port_lists(
                self,
                [self.pkt_flood, self.tagged_pkt],
                [[self.dev_port0],
                 [self.dev_port1]])

            # check traffic for new bridge_port
            # unicast pkt into VXLAN tunnel
            print("Sending packet from port %d -> VNI %d" %
                  (self.oport_dev, self.vni1))
            send_packet(self, self.oport_dev, self.pkt)
            verify_packet(self, self.vxlan_pkt, self.uport_dev)

            # unicast pkt to lag1
            print("Sending packet from port %d -> LAG1(port %d,%d,%d)" %
                  (self.oport_dev,
                   self.dev_port4,
                   self.dev_port5,
                   self.dev_port6))
            send_packet(self, self.oport_dev, self.pkt_lag1)
            verify_packets_any(
                self,
                self.pkt_lag1,
                [self.dev_port4, self.dev_port5, self.dev_port6])

            # flood pkt to all members
            print("Sending packet from port %d -> VLAN10. Will flood" %
                  (self.oport_dev))
            send_packet(self, self.oport_dev, self.pkt_flood)
            verify_each_packet_on_multiple_port_lists(
                self,
                [self.vxlan_pkt_flood,
                 self.pkt_flood,
                 self.tagged_pkt,
                 self.pkt_flood],
                [[self.dev_port10],
                 [self.dev_port4, self.dev_port5, self.dev_port6],
                 [self.dev_port1],
                 [self.dev_port24]])

            # make new bridge_port the peer link bridge_port
            print("Set custom bridge_port attr on port %d" % (self.oport_dev))
            status = sai_thrift_set_bridge_port_attribute(
                self.client,
                self.port0_bp,
                custom_attribute=self.attr_true)

            print("Bind isolation group to port %d" % (self.oport_dev))
            status = sai_thrift_set_bridge_port_attribute(
                self.client,
                self.port0_bp,
                isolation_group=self.isol_group)
            self.assertEqual(status, SAI_STATUS_SUCCESS)

            # unicast pkt into VXLAN tunnel
            print("Sending packet from port %d -> VNI %d" %
                  (self.oport_dev, self.vni1))
            send_packet(self, self.oport_dev, self.pkt)
            verify_no_other_packets(self)

            # unicast pkt to lag1
            print("Sending packet from port %d -> LAG1(port %d,%d,%d)" %
                  (self.oport_dev,
                   self.dev_port4,
                   self.dev_port5,
                   self.dev_port6))
            send_packet(self, self.oport_dev, self.pkt_lag1)
            verify_no_other_packets(self)

            # flood pkt to all members
            print("Sending packet from port %d -> VLAN10. Will flood" %
                  (self.oport_dev))
            send_packet(self, self.oport_dev, self.pkt_flood)
            verify_each_packet_on_multiple_port_lists(
                self,
                [self.tagged_pkt, self.pkt_flood],
                [[self.dev_port1],
                 [self.dev_port24]])

            # check traffic for prev peer link bridge_port
            # unicast pkt into VXLAN tunnel
            print("Sending packet from port %d -> VNI %d" %
                  (self.dev_port24, self.vni1))
            send_packet(self, self.dev_port24, self.pkt)
            verify_no_other_packets(self)

            # unicast pkt to lag1
            print("Sending packet from port %d -> LAG1(port %d,%d,%d)" %
                  (self.dev_port24,
                   self.dev_port4,
                   self.dev_port5,
                   self.dev_port6))
            send_packet(self, self.dev_port24, self.pkt_lag1)
            verify_no_other_packets(self)

            # flood pkt to all members
            print("Sending packet from port %d -> VLAN10. Will flood" %
                  (self.dev_port24))
            send_packet(self, self.dev_port24, self.pkt_flood)
            verify_each_packet_on_multiple_port_lists(
                self,
                [self.pkt_flood, self.tagged_pkt],
                [[self.dev_port0],
                 [self.dev_port1]])

            # remove is_peer_link from prev peer link bridge_port
            print("Set custom bridge_port attr on port %d" % (self.dev_port24))
            status = sai_thrift_set_bridge_port_attribute(
                self.client,
                self.port24_bp,
                custom_attribute=self.attr_false)

            # unicast pkt into VXLAN tunnel
            print("Sending packet from port %d -> VNI %d" %
                  (self.dev_port24, self.vni1))
            send_packet(self, self.dev_port24, self.pkt)
            verify_packet(self, self.vxlan_pkt, self.uport_dev)

            # unicast pkt to lag1
            print("Sending packet from port %d -> LAG1(port %d,%d,%d)" %
                  (self.dev_port24,
                   self.dev_port4,
                   self.dev_port5,
                   self.dev_port6))
            send_packet(self, self.dev_port24, self.pkt_lag1)
            verify_no_other_packets(self)

            # flood pkt to all members
            print("Sending packet from port %d -> VLAN10. Will flood" %
                  (self.dev_port24))
            send_packet(self, self.dev_port24, self.pkt_flood)
            verify_each_packet_on_multiple_port_lists(
                self,
                [self.vxlan_pkt_flood, self.pkt_flood, self.tagged_pkt],
                [[self.dev_port10], [self.dev_port0], [self.dev_port1]])

        finally:
            status = sai_thrift_set_bridge_port_attribute(
                self.client,
                self.port0_bp,
                custom_attribute=self.attr_false)

            status = sai_thrift_set_bridge_port_attribute(
                self.client,
                self.port0_bp,
                isolation_group=int(SAI_NULL_OBJECT_ID))
            self.assertEqual(status, SAI_STATUS_SUCCESS)

            status = sai_thrift_set_bridge_port_attribute(
                self.client,
                port24_bp,
                isolation_group=int(SAI_NULL_OBJECT_ID))
            self.assertEqual(status, SAI_STATUS_SUCCESS)

            sai_thrift_set_port_attribute(
                self.client,
                self.port24,
                port_vlan_id=0)
            if port24_bp != 0:
                sai_thrift_remove_bridge_port(self.client, port24_bp)
            if vlan10_member3 != 0:
                sai_thrift_remove_vlan_member(self.client, vlan10_member3)


@group('l2-vxlan')
class VxlanVtepsScaleTest(SaiHelper):
    '''
    This class contains tests for L2 VxLAN scale
    VTEPs
    '''
    def setUp(self):
        super(VxlanVtepsScaleTest, self).setUp()

        self.neighbors = []
        self.nexthops = []
        self.count_p2mp_tunnel_objects = 512
        if "count_p2mp_tunnel_objects" in self.test_params:
            self.count_p2mp_tunnel_objects = \
                self.test_params["count_p2mp_tunnel_objects"]

        count_vlan_members = self.count_p2mp_tunnel_objects//512
        self.count_p2mp_tunnel_objects = self.count_p2mp_tunnel_objects - 1
        # count_p2mp_tunnel_objects depends on rid table size.
        # At that moment RID_TABLE_SIZE is 4096 by default but before this test
        # was filled on 19 entries.
        # Jira: SWI-5903
        if self.count_p2mp_tunnel_objects > 4076:
            self.count_p2mp_tunnel_objects = 4076
        self.oport_dev = self.dev_port0
        self.oport_bp = self.port0_bp
        self.uport_dev = self.dev_port10
        self.uport_rif = self.port10_rif

        self.tun1_ip = ipaddress.IPv4Address("10.0.0.65")
        self.lpb_ip = "10.1.0.32"
        self.tun_ip_mask = "/32"
        self.uport1_myip = "10.0.0.64"
        self.uport_mask = "/31"
        self.customer1_ip = "192.168.100.1"
        self.customer1_ipv6 = "2001:0db8::100:2"
        self.customer2_ip = "192.168.100.8"
        self.customer2_ipv6 = "2001:0db8::100:9"
        self.customer1_mac = "00:22:22:22:22:11"
        self.customer2_mac = "00:22:22:22:22:22"
        self.customer2_mac_int = 146601550370
        self.unbor1_mac = "00:33:33:33:33:11"
        self.unbor2_mac = "00:33:33:33:33:22"

        # underlay loopback rif for tunnels
        self.urif_lpb = sai_thrift_create_router_interface(
            self.client,
            type=SAI_ROUTER_INTERFACE_TYPE_LOOPBACK,
            virtual_router_id=self.default_vrf,
            mtu=9100)

        self.uport1_prefix_route = sai_thrift_route_entry_t(
            destination=sai_ipprefix(self.uport1_myip + self.uport_mask),
            switch_id=self.switch_id,
            vr_id=self.default_vrf)
        sai_thrift_create_route_entry(self.client,
                                      self.uport1_prefix_route,
                                      next_hop_id=self.uport_rif)

        for i in range(0, self.count_p2mp_tunnel_objects):
            # underlay neighbor
            self.neighbors.append(sai_thrift_neighbor_entry_t(
                ip_address=sai_ipaddress(str(self.tun1_ip+i)),
                rif_id=self.uport_rif,
                switch_id=self.switch_id))
            sai_thrift_create_neighbor_entry(self.client,
                                             self.neighbors[i],
                                             dst_mac_address=self.unbor1_mac)

            # underlay nexthop
            self.nexthops.append(sai_thrift_create_next_hop(
                self.client,
                type=SAI_NEXT_HOP_TYPE_IP,
                ip=sai_ipaddress(str(self.tun1_ip + i)),
                router_interface_id=self.uport_rif))

        # create overlay vrf
        self.ovrf = sai_thrift_create_virtual_router(self.client)

        # L2 VxLAN configuration below
        self.tunnel_type = SAI_TUNNEL_TYPE_VXLAN
        peer_mode = SAI_TUNNEL_PEER_MODE_P2MP

        # create tunnel maps
        self.decap_tunnel_map_vlan = sai_thrift_create_tunnel_map(
            self.client, type=SAI_TUNNEL_MAP_TYPE_VNI_TO_VLAN_ID)

        self.encap_tunnel_map_vlan = sai_thrift_create_tunnel_map(
            self.client, type=SAI_TUNNEL_MAP_TYPE_VLAN_ID_TO_VNI)

        # create tunnel
        self.decap_maps = sai_thrift_object_list_t(
            count=1,
            idlist=[self.decap_tunnel_map_vlan])
        self.encap_maps = sai_thrift_object_list_t(
            count=1,
            idlist=[self.encap_tunnel_map_vlan])

        self.tunnel1 = sai_thrift_create_tunnel(
            self.client,
            type=self.tunnel_type,
            underlay_interface=self.urif_lpb,
            decap_mappers=self.decap_maps,
            encap_mappers=self.encap_maps,
            encap_src_ip=sai_ipaddress(self.lpb_ip),
            peer_mode=peer_mode)

        self.vlans = []
        self.vnis = []
        self.tunnelmapers = []
        # added map for fast get vlan id without call bfrts API
        self.vlan_ids = {}

        if count_vlan_members == 0:
            count_vlan_members = 1

        for i in range(count_vlan_members):
            # create vlan 40 + i with port0, port1 and lag1
            # create tunnel map entry with vni 1000 + i
            vlan_and_members = {}
            tunnelmaper = {}
            vlan_id = (40 + i)
            self.vnis.append(1000+i)

            vlan_and_members["vlan"] = \
                sai_thrift_create_vlan(self.client, vlan_id=vlan_id)
            self.vlan_ids[vlan_and_members["vlan"]] = vlan_id
            vlan_and_members["vlan_member"] = sai_thrift_create_vlan_member(
                self.client,
                vlan_id=vlan_and_members.get("vlan"),
                bridge_port_id=self.port1_bp,
                vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_TAGGED)

            # create tunnel map entries for vlan
            tunnelmaper["decap_tunnel_map_entry_vlan"] = \
                sai_thrift_create_tunnel_map_entry(
                    self.client,
                    tunnel_map_type=SAI_TUNNEL_MAP_TYPE_VNI_TO_VLAN_ID,
                    tunnel_map=self.decap_tunnel_map_vlan,
                    vlan_id_value=vlan_id,
                    vni_id_key=self.vnis[i])

            tunnelmaper["encap_tunnel_map_entry_vlan"] = \
                sai_thrift_create_tunnel_map_entry(
                    self.client,
                    tunnel_map_type=SAI_TUNNEL_MAP_TYPE_VLAN_ID_TO_VNI,
                    tunnel_map=self.encap_tunnel_map_vlan,
                    vlan_id_key=vlan_id,
                    vni_id_value=self.vnis[i])

            self.tunnelmapers.append(tunnelmaper)
            self.vlans.append(vlan_and_members)

    def runTest(self):
        try:
            self.tunnelP2MPScaleTest()
            self.tunnelP2PScaleTest()
        finally:
            pass

    def tearDown(self):

        sai_thrift_flush_fdb_entries(
            self.client, entry_type=SAI_FDB_FLUSH_ENTRY_TYPE_ALL)

        for i in self.tunnelmapers:
            if i.get('encap_tunnel_map_entry_vlan') != 0:
                sai_thrift_remove_tunnel_map_entry(
                    self.client, i.get('decap_tunnel_map_entry_vlan'))
            if i.get('encap_tunnel_map_entry_vlan') != 0:
                sai_thrift_remove_tunnel_map_entry(
                    self.client, i.get('encap_tunnel_map_entry_vlan'))
        sai_thrift_remove_tunnel(self.client, self.tunnel1)
        sai_thrift_remove_tunnel_map(self.client, self.encap_tunnel_map_vlan)
        sai_thrift_remove_tunnel_map(self.client, self.decap_tunnel_map_vlan)

        sai_thrift_remove_virtual_router(self.client, self.ovrf)
        for i in self.nexthops:
            sai_thrift_remove_next_hop(self.client, i)
        for i in self.neighbors:
            sai_thrift_remove_neighbor_entry(self.client, i)
        sai_thrift_remove_route_entry(self.client, self.uport1_prefix_route)
        sai_thrift_remove_router_interface(self.client, self.urif_lpb)

        for i in self.vlans:
            if i.get('vlan_member') != 0:
                sai_thrift_remove_vlan_member(self.client,
                                              i.get('vlan_member'))
            if i.get('vlan') != 0:
                sai_thrift_remove_vlan(self.client, i.get('vlan'))
        super(VxlanVtepsScaleTest, self).tearDown()

    def tunnelP2PScaleTest(self):
        '''
        This test verifies the number of l2vxlan tunnel vteps that
        can be supported. Create a single p2mp tunnel, but then add
        a large number of p2p tunnels, each to a different dest ip address.
        '''
        print("\ntunnelP2PScaleTest()")
        tunnel_terms_list = []
        vni = 2000

        decap_tunnel_map_entry_vlan = sai_thrift_create_tunnel_map_entry(
            self.client,
            tunnel_map_type=SAI_TUNNEL_MAP_TYPE_VNI_TO_VLAN_ID,
            tunnel_map=self.decap_tunnel_map_vlan,
            vlan_id_value=10,
            vni_id_key=vni)

        encap_tunnel_map_entry_vlan = sai_thrift_create_tunnel_map_entry(
            self.client,
            tunnel_map_type=SAI_TUNNEL_MAP_TYPE_VLAN_ID_TO_VNI,
            tunnel_map=self.encap_tunnel_map_vlan,
            vlan_id_key=10,
            vni_id_value=vni)

        term_type = SAI_TUNNEL_TERM_TABLE_ENTRY_TYPE_P2P
        # count p2p tunnels are 510 because for terminations,
        # the maximum number is IPV4_DST_VTEP_TABLE_SIZE.
        # And previouslly 2 tunnel was created
        count_p2p_tunnel_objects = 510
        try:
            print("\tCreating %d tunnels" % count_p2p_tunnel_objects)
            for i in range(0, count_p2p_tunnel_objects):
                mac = str(hex(self.customer2_mac_int+i)).replace("0x", "00")
                mac = ':'.join(mac[j]+mac[j+1] for j in range(0, len(mac), 2))
                tunnel = {}
                # Create N p2p tunnels each to a different dest ip address
                tunnel['tunnel_term'] = (
                    sai_thrift_create_tunnel_term_table_entry(
                        self.client,
                        type=term_type,
                        vr_id=self.default_vrf,
                        dst_ip=sai_ipaddress(self.lpb_ip),
                        src_ip=sai_ipaddress(str(self.tun1_ip+i)),
                        tunnel_type=SAI_TUNNEL_TYPE_VXLAN,
                        action_tunnel_id=self.tunnel1))
                tunnel['tunnel'] = sai_thrift_create_tunnel(
                    self.client,
                    type=self.tunnel_type,
                    underlay_interface=self.urif_lpb,
                    decap_mappers=self.decap_maps,
                    encap_mappers=self.encap_maps,
                    encap_src_ip=sai_ipaddress(self.lpb_ip),
                    peer_mode=SAI_TUNNEL_PEER_MODE_P2P,
                    encap_dst_ip=sai_ipaddress(str(self.tun1_ip+i)))
                # bridge port is created on p2p tunnel
                tunnel['bridge_port'] = sai_thrift_create_bridge_port(
                    self.client,
                    type=SAI_BRIDGE_PORT_TYPE_TUNNEL,
                    tunnel_id=tunnel.get('tunnel'),
                    bridge_id=self.default_1q_bridge,
                    admin_state=True,
                    fdb_learning_mode=(
                        SAI_BRIDGE_PORT_FDB_LEARNING_MODE_DISABLE))

                # vlan_member is created using tunnel bridge_port
                tunnel['vlan_member'] = sai_thrift_create_vlan_member(
                    self.client,
                    vlan_id=self.vlan10,
                    bridge_port_id=tunnel.get('bridge_port'),
                    vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)

                # fdb entry for L2 forwarding into VXLAN tunnel
                fdb_entry = sai_thrift_fdb_entry_t(
                    bv_id=self.vlan10,
                    mac_address=mac,
                    switch_id=self.switch_id)
                sai_thrift_create_fdb_entry(self.client,
                                            fdb_entry,
                                            type=SAI_FDB_ENTRY_TYPE_STATIC,
                                            bridge_port_id=tunnel.get(
                                                'bridge_port'))

                tunnel_terms_list.append(tunnel)
                self.assertNotEqual(tunnel.get('bridge_port'), 0)
                self.assertNotEqual(tunnel.get('tunnel'), 0)
                self.assertNotEqual(tunnel.get('vlan_member'), 0)
                self.assertNotEqual(tunnel.get('tunnel_term'), 0)

                # L2 forwarding into VXLAN tunnel
                pkt = simple_udp_packet(eth_dst=mac,
                                        eth_src=self.customer1_mac,
                                        ip_dst=self.customer2_ip,
                                        ip_src=self.customer1_ip,
                                        ip_id=108,
                                        ip_ttl=64)
                vxlan_pkt = Mask(
                    simple_vxlan_packet(eth_dst=self.unbor1_mac,
                                        eth_src=ROUTER_MAC,
                                        ip_dst=str(self.tun1_ip+i),
                                        ip_src=self.lpb_ip,
                                        ip_id=0,
                                        ip_ttl=64,
                                        ip_flags=0x2,
                                        with_udp_chksum=False,
                                        vxlan_vni=vni,
                                        inner_frame=pkt))
                vxlan_pkt.set_do_not_care_scapy(UDP, 'sport')
                if i % 100 == 0:
                    print("Sending packet from port %d -> VNI %d" %
                          (self.oport_dev, vni))
                send_packet(self, self.oport_dev, pkt)
                verify_packet(self, vxlan_pkt, self.uport_dev)
                if i % 100 == 0:
                    print("Packet was L2 forwarded into the VXLAN tunnel")

                # L2 decap into vlan
                pkt_3 = simple_udp_packet(eth_dst=self.customer1_mac,
                                          eth_src=mac,
                                          ip_dst=self.customer1_ip,
                                          ip_src=self.customer2_ip,
                                          ip_id=108,
                                          ip_ttl=64)
                vxlan_pkt_3 = simple_vxlan_packet(eth_dst=ROUTER_MAC,
                                                  eth_src=self.unbor1_mac,
                                                  ip_dst=self.lpb_ip,
                                                  ip_src=str(self.tun1_ip+i),
                                                  ip_id=0,
                                                  ip_ttl=64,
                                                  ip_flags=0x2,
                                                  with_udp_chksum=False,
                                                  vxlan_vni=vni,
                                                  inner_frame=pkt_3)
                if i % 100 == 0:
                    print("Sending packet from VNI %d -> port %d" %
                          (vni, self.oport_dev))
                send_packet(self, self.uport_dev, vxlan_pkt_3)
                verify_packet(self, pkt_3, self.oport_dev)
                if i % 100 == 0:
                    print("Packet was L2 forwarded out of the VXLAN tunnel")
            print("\tOK")
        finally:
            print("\tCleanup")
            sai_thrift_flush_fdb_entries(
                self.client, entry_type=SAI_FDB_FLUSH_ENTRY_TYPE_ALL)

            for i in tunnel_terms_list:
                if i.get('vlan_member') != 0:
                    sai_thrift_remove_vlan_member(self.client,
                                                  i.get('vlan_member'))
                if i.get('bridge_port') != 0:
                    sai_thrift_remove_bridge_port(self.client,
                                                  i.get('bridge_port'))
                if i.get('tunnel') != 0:
                    sai_thrift_remove_tunnel(self.client, i.get('tunnel'))
                if i.get('tunnel_term') != 0:
                    sai_thrift_remove_tunnel_term_table_entry(
                        self.client, i.get('tunnel_term'))

            sai_thrift_remove_tunnel_map_entry(
                self.client, decap_tunnel_map_entry_vlan)
            sai_thrift_remove_tunnel_map_entry(
                self.client, encap_tunnel_map_entry_vlan)

    def tunnelP2MPScaleTest(self):
        '''
        This test verifies the number of l2vxlan tunnel vteps that
        can be supported. Create a single p2mp tunnel, but then add
        a large number of p2p tunnels, each to a different dest ip address.
        '''
        print("\ntunnelP2MPScaleTest()")
        tunnel_terms_list = []
        term_type = SAI_TUNNEL_TERM_TABLE_ENTRY_TYPE_P2MP

        tunnel_term1 = sai_thrift_create_tunnel_term_table_entry(
            self.client,
            type=term_type,
            vr_id=self.default_vrf,
            dst_ip=sai_ipaddress(self.lpb_ip),
            tunnel_type=self.tunnel_type,
            action_tunnel_id=self.tunnel1)

        try:
            print("\tCreating %d tunnels" % self.count_p2mp_tunnel_objects)
            for i in range(self.count_p2mp_tunnel_objects):
                mac = str(hex(self.customer2_mac_int+i)).replace("0x", "00")
                mac = ':'.join(mac[j]+mac[j+1] for j in range(0, len(mac), 2))
                # Create N p2p tunnels each to a different dest ip address
                tunnel = {}
                it = i // 512
                vlan = self.vlans[it].get("vlan")

                tunnel['tunnel'] = sai_thrift_create_tunnel(
                    self.client,
                    type=self.tunnel_type,
                    underlay_interface=self.urif_lpb,
                    decap_mappers=self.decap_maps,
                    encap_mappers=self.encap_maps,
                    encap_src_ip=sai_ipaddress(self.lpb_ip),
                    peer_mode=SAI_TUNNEL_PEER_MODE_P2P,
                    encap_dst_ip=sai_ipaddress(str(self.tun1_ip+i)))
                # bridge port is created on p2p tunnel
                tunnel['bridge_port'] = sai_thrift_create_bridge_port(
                    self.client,
                    type=SAI_BRIDGE_PORT_TYPE_TUNNEL,
                    tunnel_id=tunnel.get('tunnel'),
                    bridge_id=self.default_1q_bridge,
                    admin_state=True,
                    fdb_learning_mode=(
                        SAI_BRIDGE_PORT_FDB_LEARNING_MODE_DISABLE))

                # vlan_member is created using tunnel bridge_port
                tunnel['vlan_member'] = sai_thrift_create_vlan_member(
                    self.client,
                    vlan_id=vlan,
                    bridge_port_id=tunnel.get('bridge_port'),
                    vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)

                # fdb entry for L2 forwarding into VXLAN tunnel
                fdb_entry = sai_thrift_fdb_entry_t(
                    bv_id=vlan,
                    mac_address=mac,
                    switch_id=self.switch_id)
                sai_thrift_create_fdb_entry(self.client,
                                            fdb_entry,
                                            type=SAI_FDB_ENTRY_TYPE_STATIC,
                                            bridge_port_id=tunnel.get(
                                                'bridge_port'))

                tunnel_terms_list.append(tunnel)
                self.assertNotEqual(tunnel.get('bridge_port'), 0)
                self.assertNotEqual(tunnel.get('tunnel'), 0)
                self.assertNotEqual(tunnel.get('vlan_member'), 0)
                # L2 forwarding into VXLAN tunnel
                pkt = simple_udp_packet(eth_dst=mac,
                                        eth_src=self.customer1_mac,
                                        ip_dst=self.customer2_ip,
                                        ip_src=self.customer1_ip,
                                        vlan_vid=self.vlan_ids.get(vlan),
                                        dl_vlan_enable=True,
                                        ip_id=108,
                                        ip_ttl=64)
                pkt_inner_exp = simple_udp_packet(eth_dst=mac,
                                                  eth_src=self.customer1_mac,
                                                  ip_dst=self.customer2_ip,
                                                  ip_src=self.customer1_ip,
                                                  ip_id=108,
                                                  ip_ttl=64,
                                                  pktlen=96)
                vxlan_pkt = Mask(
                    simple_vxlan_packet(eth_dst=self.unbor1_mac,
                                        eth_src=ROUTER_MAC,
                                        ip_dst=str(self.tun1_ip+i),
                                        ip_src=self.lpb_ip,
                                        ip_id=0,
                                        ip_ttl=64,
                                        ip_flags=0x2,
                                        with_udp_chksum=False,
                                        vxlan_vni=self.vnis[it],
                                        inner_frame=pkt_inner_exp))
                vxlan_pkt.set_do_not_care_scapy(UDP, 'sport')
                if i % 100 == 0:
                    print("Sending packet from port %d -> VNI %d" %
                          (self.oport_dev, self.vnis[it]))
                send_packet(self, self.dev_port1, pkt)
                verify_packet(self, vxlan_pkt, self.uport_dev)
                if i % 100 == 0:
                    print("Packet was L2 forwarded into the VXLAN tunnel")

                # L2 decap into vlan
                pkt_3 = simple_udp_packet(eth_dst=self.customer1_mac,
                                          eth_src=mac,
                                          ip_dst=self.customer1_ip,
                                          ip_src=self.customer2_ip,
                                          ip_id=108,
                                          ip_ttl=64)
                pkt_3_exp = simple_udp_packet(eth_dst=self.customer1_mac,
                                              eth_src=mac,
                                              ip_dst=self.customer1_ip,
                                              ip_src=self.customer2_ip,
                                              dl_vlan_enable=True,
                                              ip_id=108,
                                              vlan_vid=self.vlan_ids.get(vlan),
                                              ip_ttl=64,
                                              pktlen=104)
                vxlan_pkt_3 = simple_vxlan_packet(eth_dst=ROUTER_MAC,
                                                  eth_src=self.unbor1_mac,
                                                  ip_dst=self.lpb_ip,
                                                  ip_src=str(self.tun1_ip+i),
                                                  ip_id=0,
                                                  ip_ttl=64,
                                                  ip_flags=0x2,
                                                  with_udp_chksum=False,
                                                  vxlan_vni=self.vnis[it],
                                                  inner_frame=pkt_3)

                if i % 100 == 0:
                    print("Sending packet from VNI %d -> port %d" %
                          (self.vnis[it], self.oport_dev))
                send_packet(self, self.uport_dev, vxlan_pkt_3)
                verify_packet(self, pkt_3_exp, self.dev_port1)

                if i % 200 == 0:
                    print("Packet was L2 forwarded out of the VXLAN tunnel")

            print("\tOK")

        finally:
            print("\tCleanup")
            sai_thrift_flush_fdb_entries(
                self.client, entry_type=SAI_FDB_FLUSH_ENTRY_TYPE_ALL)
            for i in tunnel_terms_list:
                if i.get('vlan_member') != 0:
                    sai_thrift_remove_vlan_member(self.client,
                                                  i.get('vlan_member'))
                if i.get('bridge_port') != 0:
                    sai_thrift_remove_bridge_port(self.client,
                                                  i.get('bridge_port'))
                if i.get('tunnel') != 0:
                    sai_thrift_remove_tunnel(self.client, i.get('tunnel'))
            sai_thrift_remove_tunnel_term_table_entry(
                self.client, tunnel_term1)


@group('l2-vxlan')
class L2VxLanFdbTest(SaiHelper):
    '''
    This class contains tests for L2 VxLAN FDB entries
    '''

    def setUp(self):
        super(L2VxLanFdbTest, self).setUp()

        self.tun1_ip = "10.0.0.65"
        self.tun2_ip = "10.0.0.33"
        self.lpb_ip = "10.1.1.10"
        self.uport1_ip = "10.0.0.64"
        self.uport2_ip = "10.0.0.32"
        self.customer1_ip = "192.168.100.1"
        self.customer2_ip = "192.168.100.2"
        self.customer1_mac = "00:22:22:22:22:11"
        self.customer2_mac = "00:22:22:22:22:22"
        self.unbor1_mac = "00:33:33:33:33:11"
        self.unbor2_mac = "00:33:33:33:33:22"
        self.vni = 1000
        self.vlan_id = 10

        self.oport = self.port0
        self.oport_dev = self.dev_port0
        self.oport_bp = self.port0_bp
        self.uport1 = self.port10
        self.uport1_dev = self.dev_port10
        self.uport1_rif = self.port10_rif
        self.uport2 = self.port11
        self.uport2_dev = self.dev_port11
        self.uport2_rif = self.port11_rif

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
        sai_thrift_create_neighbor_entry(self.client,
                                         self.unbor1,
                                         dst_mac_address=self.unbor1_mac)
        self.unbor2 = sai_thrift_neighbor_entry_t(
            ip_address=sai_ipaddress(self.tun2_ip),
            rif_id=self.uport2_rif,
            switch_id=self.switch_id)
        sai_thrift_create_neighbor_entry(self.client,
                                         self.unbor2,
                                         dst_mac_address=self.unbor2_mac)

        # tunnel configuration
        self.tunnel_type = SAI_TUNNEL_TYPE_VXLAN

        self.encap_tunnel_map_vlan = sai_thrift_create_tunnel_map(
            self.client, type=SAI_TUNNEL_MAP_TYPE_VLAN_ID_TO_VNI)

        self.decap_tunnel_map_vlan = sai_thrift_create_tunnel_map(
            self.client, type=SAI_TUNNEL_MAP_TYPE_VNI_TO_VLAN_ID)

        encap_maps = sai_thrift_object_list_t(
            count=1,
            idlist=[self.encap_tunnel_map_vlan])
        decap_maps = sai_thrift_object_list_t(
            count=1,
            idlist=[self.decap_tunnel_map_vlan])

        self.p2mp_tunnel = sai_thrift_create_tunnel(
            self.client,
            type=self.tunnel_type,
            underlay_interface=self.urif_lpb,
            decap_mappers=decap_maps,
            encap_mappers=encap_maps,
            encap_src_ip=sai_ipaddress(self.lpb_ip),
            peer_mode=SAI_TUNNEL_PEER_MODE_P2MP)

        self.tunnel_term = sai_thrift_create_tunnel_term_table_entry(
            self.client,
            type=SAI_TUNNEL_TERM_TABLE_ENTRY_TYPE_P2MP,
            vr_id=self.default_vrf,
            dst_ip=sai_ipaddress(self.lpb_ip),
            tunnel_type=self.tunnel_type,
            action_tunnel_id=self.p2mp_tunnel)

        self.encap_tun_map_entry_vlan10 = sai_thrift_create_tunnel_map_entry(
            self.client,
            tunnel_map_type=SAI_TUNNEL_MAP_TYPE_VLAN_ID_TO_VNI,
            tunnel_map=self.encap_tunnel_map_vlan,
            vlan_id_key=10,
            vni_id_value=self.vni)

        self.decap_tun_map_entry_vlan10 = sai_thrift_create_tunnel_map_entry(
            self.client,
            tunnel_map_type=SAI_TUNNEL_MAP_TYPE_VNI_TO_VLAN_ID,
            tunnel_map=self.decap_tunnel_map_vlan,
            vni_id_key=self.vni,
            vlan_id_value=10)

        self.p2p_tunnel1 = sai_thrift_create_tunnel(
            self.client,
            type=self.tunnel_type,
            underlay_interface=self.urif_lpb,
            decap_mappers=decap_maps,
            encap_mappers=encap_maps,
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

        self.p2p_tun1_vlan10_member = sai_thrift_create_vlan_member(
            self.client,
            vlan_id=self.vlan10,
            bridge_port_id=self.p2p_tunnel1_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)

        self.p2p_tunnel2 = sai_thrift_create_tunnel(
            self.client,
            type=self.tunnel_type,
            underlay_interface=self.urif_lpb,
            decap_mappers=decap_maps,
            encap_mappers=encap_maps,
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

        self.p2p_tun2_vlan10_member = sai_thrift_create_vlan_member(
            self.client,
            vlan_id=self.vlan10,
            bridge_port_id=self.p2p_tunnel2_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)

        # test packets
        self.pkt = simple_udp_packet(eth_dst=self.customer2_mac,
                                     eth_src=self.customer1_mac,
                                     ip_dst=self.customer2_ip,
                                     ip_src=self.customer1_ip,
                                     ip_id=108,
                                     ip_ttl=64,
                                     pktlen=100)
        self.tag_pkt = simple_udp_packet(eth_dst=self.customer2_mac,
                                         eth_src=self.customer1_mac,
                                         ip_dst=self.customer2_ip,
                                         ip_src=self.customer1_ip,
                                         ip_id=108,
                                         ip_ttl=64,
                                         dl_vlan_enable=True,
                                         vlan_vid=self.vlan_id,
                                         pktlen=104)
        # expected packets
        self.vxlan_pkt = Mask(
            simple_vxlan_packet(eth_dst=self.unbor1_mac,
                                eth_src=ROUTER_MAC,
                                ip_dst=self.tun1_ip,
                                ip_src=self.lpb_ip,
                                ip_id=0,
                                ip_ttl=64,
                                ip_flags=0x2,
                                with_udp_chksum=False,
                                vxlan_vni=self.vni,
                                inner_frame=self.pkt))
        self.vxlan_pkt.set_do_not_care_scapy(UDP, 'sport')
        self.vxlan_pkt2 = Mask(
            simple_vxlan_packet(eth_dst=self.unbor2_mac,
                                eth_src=ROUTER_MAC,
                                ip_dst=self.tun2_ip,
                                ip_src=self.lpb_ip,
                                ip_id=0,
                                ip_ttl=64,
                                ip_flags=0x2,
                                with_udp_chksum=False,
                                vxlan_vni=self.vni,
                                inner_frame=self.pkt))
        self.vxlan_pkt2.set_do_not_care_scapy(UDP, 'sport')

    def runTest(self):
        try:
            self.l2VxLanMacMoveTest()
            self.l2VxLanMacMoveByLearningTest()
            self.l2VxLanFdbFlushTest()
        finally:
            pass

    def tearDown(self):
        sai_thrift_flush_fdb_entries(
            self.client, entry_type=SAI_FDB_FLUSH_ENTRY_TYPE_ALL)

        sai_thrift_remove_vlan_member(self.client, self.p2p_tun2_vlan10_member)
        sai_thrift_remove_bridge_port(self.client, self.p2p_tunnel2_bp)
        sai_thrift_remove_tunnel(self.client, self.p2p_tunnel2)
        sai_thrift_remove_vlan_member(self.client, self.p2p_tun1_vlan10_member)
        sai_thrift_remove_bridge_port(self.client, self.p2p_tunnel1_bp)
        sai_thrift_remove_tunnel(self.client, self.p2p_tunnel1)
        sai_thrift_remove_tunnel_map_entry(
            self.client, self.decap_tun_map_entry_vlan10)
        sai_thrift_remove_tunnel_map_entry(
            self.client, self.encap_tun_map_entry_vlan10)
        sai_thrift_remove_tunnel_term_table_entry(
            self.client, self.tunnel_term)
        sai_thrift_remove_tunnel(self.client, self.p2mp_tunnel)
        sai_thrift_remove_tunnel_map(self.client, self.decap_tunnel_map_vlan)
        sai_thrift_remove_tunnel_map(self.client, self.encap_tunnel_map_vlan)
        sai_thrift_remove_neighbor_entry(self.client, self.unbor2)
        sai_thrift_remove_neighbor_entry(self.client, self.unbor1)
        sai_thrift_remove_router_interface(self.client, self.urif_lpb)

        super(L2VxLanFdbTest, self).tearDown()

    def l2VxLanMacMoveTest(self):
        '''
        Verify L2 VxLAN MAC moving using attribute set of fdb_entry's
        bridge_port_id: from one P2P tunnel to another, from P2P tunnel
        to a local port, from local port to P2P tunnel
        '''
        print("\nl2VxLanMacMoveTest()")

        moving_mac = self.customer2_mac
        rx_port = self.dev_port4  # untagged in VLAN 10

        try:
            print("Creating MAC entry for P2P tunnel")
            moving_fdb_entry = sai_thrift_fdb_entry_t(
                switch_id=self.switch_id,
                mac_address=moving_mac,
                bv_id=self.vlan10)
            sai_thrift_create_fdb_entry(
                self.client,
                moving_fdb_entry,
                type=SAI_FDB_ENTRY_TYPE_STATIC,
                bridge_port_id=self.p2p_tunnel1_bp)

            print("Sending packet on port %d, forward to port %d"
                  % (rx_port, self.uport1_dev))
            send_packet(self, rx_port, self.pkt)
            verify_packet(self, self.vxlan_pkt, self.uport1_dev)
            verify_no_other_packets(self)
            print("Packet was L2 forwarded to port %d" % self.uport1_dev)

            print("Moving MAC entry P2P tunnel -> P2P tunnel")
            status = sai_thrift_set_fdb_entry_attribute(
                self.client, moving_fdb_entry,
                bridge_port_id=self.p2p_tunnel2_bp)
            self.assertEqual(status, SAI_STATUS_SUCCESS)

            print("Sending packet on port %d, forward to port %d"
                  % (rx_port, self.uport2_dev))
            send_packet(self, rx_port, self.pkt)
            verify_packet(self, self.vxlan_pkt2, self.uport2_dev)
            verify_no_other_packets(self)
            print("Packet was L2 forwarded to port %d" % self.uport2_dev)

            print("Moving MAC entry P2P tunnel -> local port")
            status = sai_thrift_set_fdb_entry_attribute(
                self.client, moving_fdb_entry,
                bridge_port_id=self.oport_bp)
            self.assertEqual(status, SAI_STATUS_SUCCESS)

            print("Sending packet on port %d, forward to port %d"
                  % (rx_port, self.oport_dev))
            send_packet(self, rx_port, self.pkt)
            verify_packet(self, self.pkt, self.oport_dev)
            verify_no_other_packets(self)
            print("Packet was L2 forwarded to port %d" % self.oport_dev)

            print("Moving MAC entry local port -> P2P tunnel")
            status = sai_thrift_set_fdb_entry_attribute(
                self.client, moving_fdb_entry,
                bridge_port_id=self.p2p_tunnel2_bp)
            self.assertEqual(status, SAI_STATUS_SUCCESS)

            print("Sending packet on port %d, forward to port %d"
                  % (rx_port, self.uport2_dev))
            send_packet(self, rx_port, self.pkt)
            verify_packet(self, self.vxlan_pkt2, self.uport2_dev)
            verify_no_other_packets(self)
            print("Packet was L2 forwarded to port %d" % self.uport2_dev)

        finally:
            sai_thrift_flush_fdb_entries(
                self.client, entry_type=SAI_FDB_FLUSH_ENTRY_TYPE_ALL)

    def l2VxLanMacMoveByLearningTest(self):
        '''
        Verify L2 VxLAN MAC move to and from P2P tunnel by attribute_set
        to P2P tunnel, then back to a local port via learning, respectively.
        Also verify aging after learning.
        '''
        print("\nl2VxLanMacMoveByLearningTest()")

        moving_mac = self.customer2_mac
        moving_fdb_entry = sai_thrift_fdb_entry_t(
            switch_id=self.switch_id,
            mac_address=moving_mac,
            bv_id=self.vlan10)
        rx_port = self.dev_port1  # tagged in VLAN 10

        reply_pkt = simple_udp_packet(eth_dst=self.customer1_mac,
                                      eth_src=self.customer2_mac,
                                      ip_dst=self.customer1_ip,
                                      ip_src=self.customer2_ip,
                                      ip_id=108,
                                      ip_ttl=64,
                                      udp_sport=2,
                                      pktlen=100)

        tag_reply_pkt = simple_udp_packet(eth_dst=self.customer1_mac,
                                          eth_src=self.customer2_mac,
                                          ip_dst=self.customer1_ip,
                                          ip_src=self.customer2_ip,
                                          ip_id=108,
                                          ip_ttl=64,
                                          udp_sport=2,
                                          dl_vlan_enable=True,
                                          vlan_vid=self.vlan_id,
                                          pktlen=104)

        reply_vxlan_pkt = simple_vxlan_packet(eth_dst=ROUTER_MAC,
                                              eth_src=self.unbor1_mac,
                                              ip_dst=self.lpb_ip,
                                              ip_src=self.tun1_ip,
                                              ip_id=0,
                                              ip_ttl=64,
                                              ip_flags=0x2,
                                              with_udp_chksum=False,
                                              vxlan_vni=self.vni,
                                              inner_frame=reply_pkt)

        try:
            # age time used in tests (in sec)
            age_time = 10
            status = sai_thrift_set_switch_attribute(
                self.client, fdb_aging_time=age_time)
            self.assertEqual(status, SAI_STATUS_SUCCESS)
            print("Aging time set to %d sec" % age_time)

            print("Sending packet on port %d, learn and flood"
                  % rx_port)
            send_packet(self, rx_port, self.tag_pkt)
            verify_each_packet_on_multiple_port_lists(
                self,
                [self.pkt, self.pkt, self.vxlan_pkt, self.vxlan_pkt2],
                [[self.dev_port0],
                 [self.dev_port4, self.dev_port5, self.dev_port6],
                 [self.uport1_dev],
                 [self.uport2_dev]])

            print("Receiving reply pkt on port %d, learn & forward to port %d"
                  % (self.oport_dev, rx_port))
            send_packet(self, self.oport_dev, reply_pkt)
            verify_packet(self, tag_reply_pkt, rx_port)
            verify_no_other_packets(self)
            print("Packet was L2 forwarded to port %d" % rx_port)

            time.sleep(2)

            print("Sending packet on port %d, forward to port %d"
                  % (rx_port, self.oport_dev))
            send_packet(self, rx_port, self.tag_pkt)
            verify_packet(self, self.pkt, self.oport_dev)
            verify_no_other_packets(self)
            print("Packet was L2 forwarded to port %d" % self.oport_dev)

            print("Move mac to remote vtep & change type to static allow_move")
            status = sai_thrift_set_fdb_entry_attribute(
                self.client, moving_fdb_entry,
                type=SAI_FDB_ENTRY_TYPE_STATIC)
            self.assertEqual(status, SAI_STATUS_SUCCESS)
            status = sai_thrift_set_fdb_entry_attribute(
                self.client, moving_fdb_entry,
                allow_mac_move=True)
            self.assertEqual(status, SAI_STATUS_SUCCESS)
            status = sai_thrift_set_fdb_entry_attribute(
                self.client, moving_fdb_entry,
                bridge_port_id=self.p2p_tunnel1_bp)
            self.assertEqual(status, SAI_STATUS_SUCCESS)
            status = sai_thrift_set_fdb_entry_attribute(
                self.client, moving_fdb_entry,
                endpoint_ip=sai_ipaddress(self.tun1_ip))
            self.assertEqual(status, SAI_STATUS_SUCCESS)

            print("Sending packet on port %d, forward to port %d"
                  % (rx_port, self.uport1_dev))
            send_packet(self, rx_port, self.tag_pkt)
            verify_packet(self, self.vxlan_pkt, self.uport1_dev)
            verify_no_other_packets(self)
            print("Packet was L2 forwarded to port %d" % self.uport1_dev)

            print("Receiving reply vxlan packet on port %d, forward to port %d"
                  % (self.uport1_dev, rx_port))
            send_packet(self, self.uport1_dev, reply_vxlan_pkt)
            verify_packet(self, tag_reply_pkt, rx_port)
            verify_no_other_packets(self)
            print("Packet was L2 forwarded to port %d" % rx_port)

            print("Receiving reply pkt on port %d, learn & forward to port %d"
                  % (self.oport_dev, rx_port))
            send_packet(self, self.oport_dev, reply_pkt)
            verify_packet(self, tag_reply_pkt, rx_port)
            verify_no_other_packets(self)
            print("Packet was L2 forwarded to port %d" % rx_port)

            time.sleep(2)

            print("Sending packet on port %d, forward to port %d"
                  % (rx_port, self.oport_dev))
            send_packet(self, rx_port, self.tag_pkt)
            verify_packet(self, self.pkt, self.oport_dev)
            verify_no_other_packets(self)
            print("Packet was L2 forwarded to port %d" % self.oport_dev)

            self.saiWaitFdbAge(age_time)

            print("Sending packet on port %d, verify flood after aging"
                  % rx_port)
            send_packet(self, rx_port, self.tag_pkt)
            verify_each_packet_on_multiple_port_lists(
                self,
                [self.pkt, self.pkt, self.vxlan_pkt, self.vxlan_pkt2],
                [[self.dev_port0],
                 [self.dev_port4, self.dev_port5, self.dev_port6],
                 [self.uport1_dev],
                 [self.uport2_dev]])

        finally:
            # disable aging
            status = sai_thrift_set_switch_attribute(
                self.client, fdb_aging_time=0)
            sai_thrift_flush_fdb_entries(
                self.client, entry_type=SAI_FDB_FLUSH_ENTRY_TYPE_ALL)

    def l2VxLanFdbFlushTest(self):
        '''
        Verify flushing of tunnel MAC entry
        '''
        print("\nl2VxLanFdbFlushTest()")

        def _verify_flooding():
            '''
            Helper function verifying flooding to all VLAN 10 ports
            '''
            print("Sending packet on port %d" % self.oport_dev)
            send_packet(self, self.oport_dev, self.pkt)
            verify_each_packet_on_multiple_port_lists(
                self,
                [self.tag_pkt, self.pkt, self.vxlan_pkt, self.vxlan_pkt2],
                [[self.dev_port1],
                 [self.dev_port4, self.dev_port5, self.dev_port6],
                 [self.uport1_dev], [self.uport2_dev]])
            print('Packet was flooded into L2 VXLAN tunnels '
                  'and physical ports')

        def _verify_forwarding():
            '''
            Helper function verifying forwarding to P2P tunnel 1
            '''
            print("Sending packet on port %d" % self.oport_dev)
            send_packet(self, self.oport_dev, self.pkt)
            verify_packet(self, self.vxlan_pkt, self.uport1_dev)
            verify_no_other_packets(self)
            print("Packet was L2 forwarded to port %d" % self.uport1_dev)

        def _add_mac_entry():
            '''
            Helper function that adds P2P tunnel 1 MAC entry
            '''
            print("Adding static MAC entry")
            fdb_entry = sai_thrift_fdb_entry_t(
                switch_id=self.switch_id,
                mac_address=self.customer2_mac,
                bv_id=self.vlan10)
            sai_thrift_create_fdb_entry(
                self.client,
                fdb_entry,
                type=SAI_FDB_ENTRY_TYPE_STATIC,
                bridge_port_id=self.p2p_tunnel1_bp)

        try:
            # no FDB entry - flooding
            _verify_flooding()

            _add_mac_entry()

            # MAC entry exists - forwarding
            _verify_forwarding()

            print("Flushing dynamic MAC entries")
            sai_thrift_flush_fdb_entries(
                self.client, entry_type=SAI_FDB_FLUSH_ENTRY_TYPE_DYNAMIC)

            # MAC entry still exists - forwarding
            _verify_forwarding()

            print("Flushing static MAC entries")
            sai_thrift_flush_fdb_entries(
                self.client, entry_type=SAI_FDB_FLUSH_ENTRY_TYPE_STATIC)

            # no FDB entry - flooding
            _verify_flooding()

            _add_mac_entry()

            # MAC entry exists - forwarding
            _verify_forwarding()

            print("Flushing all types of MAC entries")
            sai_thrift_flush_fdb_entries(
                self.client, entry_type=SAI_FDB_FLUSH_ENTRY_TYPE_ALL)

            # no FDB entry - flooding
            _verify_flooding()

            _add_mac_entry()

            # MAC entry exists - forwarding
            _verify_forwarding()

            print("Flushing MAC entry per bridge port")
            sai_thrift_flush_fdb_entries(
                self.client, entry_type=SAI_FDB_FLUSH_ENTRY_TYPE_ALL,
                bridge_port_id=self.p2p_tunnel1_bp)

            # no FDB entry - flooding
            _verify_flooding()

            _add_mac_entry()

            # MAC entry exists - forwarding
            _verify_forwarding()

            print("Flushing MAC entry per VLAN")
            sai_thrift_flush_fdb_entries(
                self.client, entry_type=SAI_FDB_FLUSH_ENTRY_TYPE_ALL,
                bv_id=self.vlan10)

            # no FDB entry - flooding
            _verify_flooding()

        finally:
            sai_thrift_flush_fdb_entries(
                self.client, entry_type=SAI_FDB_FLUSH_ENTRY_TYPE_ALL)


@group('l2-vxlan')
class L2VxLanMappersTest(SaiHelper):
    '''
    This class verifies L2 VxLAN encap and decap VNI <-> VLAN mapping
    '''

    def setUp(self):
        super(L2VxLanMappersTest, self).setUp()

        self.tun1_ip = "10.0.0.65"
        self.tun2_ip = "10.0.0.33"
        self.lpb_ip = "10.1.1.10"
        self.uport1_ip = "10.0.0.64"
        self.uport2_ip = "10.0.0.32"
        self.customer1_ip = "192.168.100.1"
        self.customer2_ip = "192.168.100.2"
        self.customer3_ip = "192.168.100.8"
        self.customer4_ip = "192.168.200.1"
        self.customer5_ip = "192.168.200.2"
        self.customer6_ip = "192.168.200.8"
        self.customer1_mac = "00:22:22:22:22:11"
        self.customer2_mac = "00:22:22:22:22:22"
        self.customer3_mac = "00:22:22:22:22:33"
        self.customer4_mac = "00:22:22:22:22:44"
        self.customer5_mac = "00:22:22:22:22:55"
        self.customer6_mac = "00:22:22:22:22:66"
        self.unbor1_mac = "00:33:33:33:33:11"
        self.unbor2_mac = "00:33:33:33:33:22"
        self.vni1 = 1000
        self.vni2 = 2000

        self.oport0 = self.port0
        self.oport0_dev = self.dev_port0
        self.oport0_bp = self.port0_bp
        self.oport1 = self.port1
        self.oport1_dev = self.dev_port1
        self.oport1_bp = self.port1_bp
        self.oport2 = self.port2
        self.oport2_dev = self.dev_port2
        self.oport2_bp = self.port2_bp
        self.oport3 = self.port3
        self.oport3_dev = self.dev_port3
        self.oport3_bp = self.port3_bp
        self.uport1 = self.port10  # customer3
        self.uport1_dev = self.dev_port10
        self.uport1_rif = self.port10_rif
        self.uport2 = self.port11  # customer6
        self.uport2_dev = self.dev_port11
        self.uport2_rif = self.port11_rif

        # underlay loopback rif for tunnels
        self.urif_lpb = sai_thrift_create_router_interface(
            self.client,
            type=SAI_ROUTER_INTERFACE_TYPE_LOOPBACK,
            virtual_router_id=self.default_vrf,
            mtu=9100)

        self.uport1_prefix_route = sai_thrift_route_entry_t(
            destination=sai_ipprefix(self.uport1_ip + '/31'),
            switch_id=self.switch_id,
            vr_id=self.default_vrf)
        sai_thrift_create_route_entry(self.client,
                                      self.uport1_prefix_route,
                                      next_hop_id=self.uport1_rif)

        self.uport2_prefix_route = sai_thrift_route_entry_t(
            destination=sai_ipprefix(self.uport2_ip + '/31'),
            switch_id=self.switch_id,
            vr_id=self.default_vrf)
        sai_thrift_create_route_entry(self.client,
                                      self.uport2_prefix_route,
                                      next_hop_id=self.uport2_rif)

        # underlay neighbor
        self.unbor1 = sai_thrift_neighbor_entry_t(
            ip_address=sai_ipaddress(self.tun1_ip),
            rif_id=self.uport1_rif,
            switch_id=self.switch_id)
        sai_thrift_create_neighbor_entry(self.client,
                                         self.unbor1,
                                         dst_mac_address=self.unbor1_mac)
        self.unbor2 = sai_thrift_neighbor_entry_t(
            ip_address=sai_ipaddress(self.tun2_ip),
            rif_id=self.uport2_rif,
            switch_id=self.switch_id)
        sai_thrift_create_neighbor_entry(self.client,
                                         self.unbor2,
                                         dst_mac_address=self.unbor2_mac)

        # underlay nexthop
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

        # tunnel configuration
        tunnel_type = SAI_TUNNEL_TYPE_VXLAN

        self.encap_tunnel_map_vlan = sai_thrift_create_tunnel_map(
            self.client, type=SAI_TUNNEL_MAP_TYPE_VLAN_ID_TO_VNI)

        self.decap_tunnel_map_vlan = sai_thrift_create_tunnel_map(
            self.client, type=SAI_TUNNEL_MAP_TYPE_VNI_TO_VLAN_ID)

        encap_maps = sai_thrift_object_list_t(
            count=1,
            idlist=[self.encap_tunnel_map_vlan])
        decap_maps = sai_thrift_object_list_t(
            count=1,
            idlist=[self.decap_tunnel_map_vlan])

        self.p2mp_tunnel = sai_thrift_create_tunnel(
            self.client,
            type=tunnel_type,
            underlay_interface=self.urif_lpb,
            decap_mappers=decap_maps,
            encap_mappers=encap_maps,
            encap_src_ip=sai_ipaddress(self.lpb_ip),
            peer_mode=SAI_TUNNEL_PEER_MODE_P2MP)

        self.tunnel_term = sai_thrift_create_tunnel_term_table_entry(
            self.client,
            type=SAI_TUNNEL_TERM_TABLE_ENTRY_TYPE_P2MP,
            vr_id=self.default_vrf,
            dst_ip=sai_ipaddress(self.lpb_ip),
            tunnel_type=tunnel_type,
            action_tunnel_id=self.p2mp_tunnel)

        self.p2p_tunnel1 = sai_thrift_create_tunnel(
            self.client,
            type=tunnel_type,
            underlay_interface=self.urif_lpb,
            decap_mappers=decap_maps,
            encap_mappers=encap_maps,
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

        self.p2p_tun1_vlan10_member = sai_thrift_create_vlan_member(
            self.client,
            vlan_id=self.vlan10,
            bridge_port_id=self.p2p_tunnel1_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)

        self.p2p_tunnel2 = sai_thrift_create_tunnel(
            self.client,
            type=tunnel_type,
            underlay_interface=self.urif_lpb,
            decap_mappers=decap_maps,
            encap_mappers=encap_maps,
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

        self.p2p_tun2_vlan20_member = sai_thrift_create_vlan_member(
            self.client,
            vlan_id=self.vlan20,
            bridge_port_id=self.p2p_tunnel2_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)

        self.p2p_tun2_vlan10_member = sai_thrift_create_vlan_member(
            self.client,
            vlan_id=self.vlan10,
            bridge_port_id=self.p2p_tunnel2_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)

        # FDB settings
        #                         overlay | underlay
        #              ..VLAN10................................
        #              : +--------------------------------+   :
        #  customer1 --:-| (utg) port0                    |   :
        #              : |                                |   :
        #              : |                                |   :
        #  customer2 --:-| (tg)  port1   tunnel1 - port10 |---:- customer3
        #              :.|............                    |   :
        #                |           : ...................|.. :
        #                |           : : tunnel2 - port11 |-:-:- customer6
        #                |           :.:..................|.:.:
        #              ..|.............:                  | :
        #  customer4 --:-| (utg) port2                    | :
        #              : |                                | :
        #              : |                                | :
        #  customer5 --:-| (tg)  port3                    | :
        #              : +--------------------------------+ :
        #              :.VLAN20.............................:
        fdb_entry = sai_thrift_fdb_entry_t(
            bv_id=self.vlan10,
            mac_address=self.customer1_mac,
            switch_id=self.switch_id)
        sai_thrift_create_fdb_entry(self.client,
                                    fdb_entry,
                                    type=SAI_FDB_ENTRY_TYPE_STATIC,
                                    bridge_port_id=self.oport0_bp)
        fdb_entry = sai_thrift_fdb_entry_t(
            bv_id=self.vlan10,
            mac_address=self.customer2_mac,
            switch_id=self.switch_id)
        sai_thrift_create_fdb_entry(self.client,
                                    fdb_entry,
                                    type=SAI_FDB_ENTRY_TYPE_STATIC,
                                    bridge_port_id=self.oport1_bp)
        fdb_entry = sai_thrift_fdb_entry_t(
            bv_id=self.vlan10,
            mac_address=self.customer3_mac,
            switch_id=self.switch_id)
        sai_thrift_create_fdb_entry(self.client,
                                    fdb_entry,
                                    type=SAI_FDB_ENTRY_TYPE_STATIC,
                                    bridge_port_id=self.p2p_tunnel1_bp)
        fdb_entry = sai_thrift_fdb_entry_t(
            bv_id=self.vlan20,
            mac_address=self.customer4_mac,
            switch_id=self.switch_id)
        sai_thrift_create_fdb_entry(self.client,
                                    fdb_entry,
                                    type=SAI_FDB_ENTRY_TYPE_STATIC,
                                    bridge_port_id=self.oport2_bp)
        fdb_entry = sai_thrift_fdb_entry_t(
            bv_id=self.vlan20,
            mac_address=self.customer5_mac,
            switch_id=self.switch_id)
        sai_thrift_create_fdb_entry(self.client,
                                    fdb_entry,
                                    type=SAI_FDB_ENTRY_TYPE_STATIC,
                                    bridge_port_id=self.oport3_bp)
        fdb_entry = sai_thrift_fdb_entry_t(
            bv_id=self.vlan20,
            mac_address=self.customer6_mac,
            switch_id=self.switch_id)
        sai_thrift_create_fdb_entry(self.client,
                                    fdb_entry,
                                    type=SAI_FDB_ENTRY_TYPE_STATIC,
                                    bridge_port_id=self.p2p_tunnel2_bp)
        fdb_entry = sai_thrift_fdb_entry_t(
            bv_id=self.vlan10,
            mac_address=self.customer6_mac,
            switch_id=self.switch_id)
        sai_thrift_create_fdb_entry(self.client,
                                    fdb_entry,
                                    type=SAI_FDB_ENTRY_TYPE_STATIC,
                                    bridge_port_id=self.p2p_tunnel2_bp)

    def runTest(self):
        try:
            self.l2VxlanVlanToVniMappingTest()
            self.l2VxlanVniToVlanMappingTest()
            self.l2VxlanVlanToVniMappingFloodTest()
            self.l2VxlanVniToVlanMappingFloodTest()
        finally:
            pass

    def tearDown(self):
        sai_thrift_flush_fdb_entries(
            self.client, entry_type=SAI_FDB_FLUSH_ENTRY_TYPE_ALL)

        sai_thrift_remove_vlan_member(self.client, self.p2p_tun2_vlan10_member)
        sai_thrift_remove_vlan_member(self.client, self.p2p_tun2_vlan20_member)
        sai_thrift_remove_bridge_port(self.client, self.p2p_tunnel2_bp)
        sai_thrift_remove_tunnel(self.client, self.p2p_tunnel2)
        sai_thrift_remove_vlan_member(self.client, self.p2p_tun1_vlan10_member)
        sai_thrift_remove_bridge_port(self.client, self.p2p_tunnel1_bp)
        sai_thrift_remove_tunnel(self.client, self.p2p_tunnel1)
        sai_thrift_remove_tunnel_term_table_entry(
            self.client, self.tunnel_term)
        sai_thrift_remove_tunnel(self.client, self.p2mp_tunnel)
        sai_thrift_remove_tunnel_map(self.client, self.decap_tunnel_map_vlan)
        sai_thrift_remove_tunnel_map(self.client, self.encap_tunnel_map_vlan)
        sai_thrift_remove_next_hop(self.client, self.unhop2)
        sai_thrift_remove_next_hop(self.client, self.unhop1)
        sai_thrift_remove_neighbor_entry(self.client, self.unbor2)
        sai_thrift_remove_neighbor_entry(self.client, self.unbor1)
        sai_thrift_remove_route_entry(self.client, self.uport2_prefix_route)
        sai_thrift_remove_route_entry(self.client, self.uport1_prefix_route)
        sai_thrift_remove_router_interface(self.client, self.urif_lpb)

        super(L2VxLanMappersTest, self).tearDown()

    def l2VxlanVlanToVniMappingTest(self):
        '''
        Verify packets on different VLANs get mapped to different VNIs
        '''
        print("\nl2VxlanVlanToVniMappingTest()")

        try:
            # tunnel map entries for VLAN 10 and VLAN 20
            encap_tun_map_entry_vlan10 = sai_thrift_create_tunnel_map_entry(
                self.client,
                tunnel_map_type=SAI_TUNNEL_MAP_TYPE_VLAN_ID_TO_VNI,
                tunnel_map=self.encap_tunnel_map_vlan,
                vlan_id_key=10,
                vni_id_value=self.vni1)

            encap_tun_map_entry_vlan20 = sai_thrift_create_tunnel_map_entry(
                self.client,
                tunnel_map_type=SAI_TUNNEL_MAP_TYPE_VLAN_ID_TO_VNI,
                tunnel_map=self.encap_tunnel_map_vlan,
                vlan_id_key=20,
                vni_id_value=self.vni2)

            # forwarding in VLAN 10 (-> tunnel 1)
            pkt = simple_udp_packet(eth_dst=self.customer3_mac,
                                    eth_src=self.customer1_mac,
                                    ip_dst=self.customer3_ip,
                                    ip_src=self.customer1_ip,
                                    ip_id=108,
                                    ip_ttl=64,
                                    pktlen=100)
            vxlan_pkt = Mask(
                simple_vxlan_packet(eth_dst=self.unbor1_mac,
                                    eth_src=ROUTER_MAC,
                                    ip_dst=self.tun1_ip,
                                    ip_src=self.lpb_ip,
                                    ip_id=0,
                                    ip_ttl=64,
                                    ip_flags=0x2,
                                    with_udp_chksum=False,
                                    vxlan_vni=self.vni1,
                                    inner_frame=pkt))
            vxlan_pkt.set_do_not_care_scapy(UDP, 'sport')

            print("Sending packet on untagged port %d in VLAN 10 -> VNI %d"
                  % (self.oport0_dev, self.vni1))
            send_packet(self, self.oport0_dev, pkt)
            verify_packet(self, vxlan_pkt, self.uport1_dev)
            print("Packet was L2 forwarded to tunnel 1 with correct VNI")

            tag_pkt = simple_udp_packet(eth_dst=self.customer3_mac,
                                        eth_src=self.customer2_mac,
                                        ip_dst=self.customer3_ip,
                                        ip_src=self.customer2_ip,
                                        dl_vlan_enable=True,
                                        vlan_vid=10,
                                        ip_id=108,
                                        ip_ttl=64,
                                        pktlen=104)
            pkt = simple_udp_packet(eth_dst=self.customer3_mac,
                                    eth_src=self.customer2_mac,
                                    ip_dst=self.customer3_ip,
                                    ip_src=self.customer2_ip,
                                    ip_id=108,
                                    ip_ttl=64,
                                    pktlen=100)
            vxlan_pkt = Mask(
                simple_vxlan_packet(eth_dst=self.unbor1_mac,
                                    eth_src=ROUTER_MAC,
                                    ip_dst=self.tun1_ip,
                                    ip_src=self.lpb_ip,
                                    ip_id=0,
                                    ip_ttl=64,
                                    ip_flags=0x2,
                                    with_udp_chksum=False,
                                    vxlan_vni=self.vni1,
                                    inner_frame=pkt))
            vxlan_pkt.set_do_not_care_scapy(UDP, 'sport')

            print("Sending packet on tagged port %d in VLAN 10 -> VNI %d"
                  % (self.oport1_dev, self.vni1))
            send_packet(self, self.oport1_dev, tag_pkt)
            verify_packet(self, vxlan_pkt, self.uport1_dev)
            print("Packet was L2 forwarded to tunnel 1 with correct VNI")

            # forwarding in VLAN 10 (-> tunnel 2)
            pkt = simple_udp_packet(eth_dst=self.customer6_mac,
                                    eth_src=self.customer1_mac,
                                    ip_dst=self.customer6_ip,
                                    ip_src=self.customer1_ip,
                                    ip_id=108,
                                    ip_ttl=64,
                                    pktlen=100)
            vxlan_pkt = Mask(
                simple_vxlan_packet(eth_dst=self.unbor2_mac,
                                    eth_src=ROUTER_MAC,
                                    ip_dst=self.tun2_ip,
                                    ip_src=self.lpb_ip,
                                    ip_id=0,
                                    ip_ttl=64,
                                    ip_flags=0x2,
                                    with_udp_chksum=False,
                                    vxlan_vni=self.vni1,
                                    inner_frame=pkt))
            vxlan_pkt.set_do_not_care_scapy(UDP, 'sport')

            print("Sending packet on untagged port %d in VLAN 10 -> VNI %d"
                  % (self.oport0_dev, self.vni1))
            send_packet(self, self.oport0_dev, pkt)
            verify_packet(self, vxlan_pkt, self.uport2_dev)
            print("Packet was L2 forwarded to tunnel 2 with correct VNI")

            tag_pkt = simple_udp_packet(eth_dst=self.customer6_mac,
                                        eth_src=self.customer2_mac,
                                        ip_dst=self.customer6_ip,
                                        ip_src=self.customer2_ip,
                                        dl_vlan_enable=True,
                                        vlan_vid=10,
                                        ip_id=108,
                                        ip_ttl=64,
                                        pktlen=104)
            pkt = simple_udp_packet(eth_dst=self.customer6_mac,
                                    eth_src=self.customer2_mac,
                                    ip_dst=self.customer6_ip,
                                    ip_src=self.customer2_ip,
                                    ip_id=108,
                                    ip_ttl=64,
                                    pktlen=100)
            vxlan_pkt = Mask(
                simple_vxlan_packet(eth_dst=self.unbor2_mac,
                                    eth_src=ROUTER_MAC,
                                    ip_dst=self.tun2_ip,
                                    ip_src=self.lpb_ip,
                                    ip_id=0,
                                    ip_ttl=64,
                                    ip_flags=0x2,
                                    with_udp_chksum=False,
                                    vxlan_vni=self.vni1,
                                    inner_frame=pkt))
            vxlan_pkt.set_do_not_care_scapy(UDP, 'sport')

            print("Sending packet on tagged port %d in VLAN 10 -> VNI %d"
                  % (self.oport1_dev, self.vni1))
            send_packet(self, self.oport1_dev, tag_pkt)
            verify_packet(self, vxlan_pkt, self.uport2_dev)
            print("Packet was L2 forwarded to tunnel 2 with correct VNI")

            # forwarding in VLAN 20
            pkt = simple_udp_packet(eth_dst=self.customer6_mac,
                                    eth_src=self.customer4_mac,
                                    ip_dst=self.customer6_ip,
                                    ip_src=self.customer4_ip,
                                    ip_id=108,
                                    ip_ttl=64,
                                    pktlen=100)
            vxlan_pkt = Mask(
                simple_vxlan_packet(eth_dst=self.unbor2_mac,
                                    eth_src=ROUTER_MAC,
                                    ip_dst=self.tun2_ip,
                                    ip_src=self.lpb_ip,
                                    ip_id=0,
                                    ip_ttl=64,
                                    ip_flags=0x2,
                                    with_udp_chksum=False,
                                    vxlan_vni=self.vni2,
                                    inner_frame=pkt))
            vxlan_pkt.set_do_not_care_scapy(UDP, 'sport')

            print("Sending packet on untagged port %d in VLAN 20 -> VNI %d"
                  % (self.oport2_dev, self.vni2))
            send_packet(self, self.oport2_dev, pkt)
            verify_packet(self, vxlan_pkt, self.uport2_dev)
            print("Packet was L2 forwarded with correct VNI")

            tag_pkt = simple_udp_packet(eth_dst=self.customer6_mac,
                                        eth_src=self.customer5_mac,
                                        ip_dst=self.customer6_ip,
                                        ip_src=self.customer5_ip,
                                        dl_vlan_enable=True,
                                        vlan_vid=20,
                                        ip_id=108,
                                        ip_ttl=64,
                                        pktlen=104)
            pkt = simple_udp_packet(eth_dst=self.customer6_mac,
                                    eth_src=self.customer5_mac,
                                    ip_dst=self.customer6_ip,
                                    ip_src=self.customer5_ip,
                                    ip_id=108,
                                    ip_ttl=64,
                                    pktlen=100)
            vxlan_pkt = Mask(
                simple_vxlan_packet(eth_dst=self.unbor2_mac,
                                    eth_src=ROUTER_MAC,
                                    ip_dst=self.tun2_ip,
                                    ip_src=self.lpb_ip,
                                    ip_id=0,
                                    ip_ttl=64,
                                    ip_flags=0x2,
                                    with_udp_chksum=False,
                                    vxlan_vni=self.vni2,
                                    inner_frame=pkt))
            vxlan_pkt.set_do_not_care_scapy(UDP, 'sport')

            print("Sending packet on tagged port %d in VLAN 20 -> VNI %d"
                  % (self.oport3_dev, self.vni2))
            send_packet(self, self.oport3_dev, tag_pkt)
            verify_packet(self, vxlan_pkt, self.uport2_dev)
            print("Packet was L2 forwarded with correct VNI")

            new_vni = 2222
            print("\n\tChanging encap mapper entry for VLAN 20 - new VNI=%d\n"
                  % new_vni)
            sai_thrift_remove_tunnel_map_entry(self.client,
                                               encap_tun_map_entry_vlan20)
            encap_tun_map_entry_vlan20 = sai_thrift_create_tunnel_map_entry(
                self.client,
                tunnel_map_type=SAI_TUNNEL_MAP_TYPE_VLAN_ID_TO_VNI,
                tunnel_map=self.encap_tunnel_map_vlan,
                vlan_id_key=20,
                vni_id_value=new_vni)

            pkt = simple_udp_packet(eth_dst=self.customer6_mac,
                                    eth_src=self.customer4_mac,
                                    ip_dst=self.customer6_ip,
                                    ip_src=self.customer4_ip,
                                    ip_id=108,
                                    ip_ttl=64,
                                    pktlen=100)
            vxlan_pkt = Mask(
                simple_vxlan_packet(eth_dst=self.unbor2_mac,
                                    eth_src=ROUTER_MAC,
                                    ip_dst=self.tun2_ip,
                                    ip_src=self.lpb_ip,
                                    ip_id=0,
                                    ip_ttl=64,
                                    ip_flags=0x2,
                                    with_udp_chksum=False,
                                    vxlan_vni=new_vni,
                                    inner_frame=pkt))
            vxlan_pkt.set_do_not_care_scapy(UDP, 'sport')

            print("Sending packet on untagged port %d in VLAN 20 -> VNI %d"
                  % (self.oport2_dev, new_vni))
            send_packet(self, self.oport2_dev, pkt)
            verify_packet(self, vxlan_pkt, self.uport2_dev)
            print("Packet was L2 forwarded with correct VNI")

            tag_pkt = simple_udp_packet(eth_dst=self.customer6_mac,
                                        eth_src=self.customer5_mac,
                                        ip_dst=self.customer6_ip,
                                        ip_src=self.customer5_ip,
                                        dl_vlan_enable=True,
                                        vlan_vid=20,
                                        ip_id=108,
                                        ip_ttl=64,
                                        pktlen=104)
            pkt = simple_udp_packet(eth_dst=self.customer6_mac,
                                    eth_src=self.customer5_mac,
                                    ip_dst=self.customer6_ip,
                                    ip_src=self.customer5_ip,
                                    ip_id=108,
                                    ip_ttl=64,
                                    pktlen=100)
            vxlan_pkt = Mask(
                simple_vxlan_packet(eth_dst=self.unbor2_mac,
                                    eth_src=ROUTER_MAC,
                                    ip_dst=self.tun2_ip,
                                    ip_src=self.lpb_ip,
                                    ip_id=0,
                                    ip_ttl=64,
                                    ip_flags=0x2,
                                    with_udp_chksum=False,
                                    vxlan_vni=new_vni,
                                    inner_frame=pkt))
            vxlan_pkt.set_do_not_care_scapy(UDP, 'sport')

            print("Sending packet on tagged port %d in VLAN 20 -> VNI %d"
                  % (self.oport3_dev, new_vni))
            send_packet(self, self.oport3_dev, tag_pkt)
            verify_packet(self, vxlan_pkt, self.uport2_dev)
            print("Packet was L2 forwarded with correct VNI")

        finally:
            sai_thrift_remove_tunnel_map_entry(self.client,
                                               encap_tun_map_entry_vlan20)
            sai_thrift_remove_tunnel_map_entry(self.client,
                                               encap_tun_map_entry_vlan10)

    def l2VxlanVniToVlanMappingTest(self):
        '''
        Verify packets on different VNIs get mapped to different VLANs
        '''
        print("\nl2VxlanVniToVlanMappingTest()")

        try:
            # tunnel map entries for VLAN 10 and VLAN 20
            decap_tun_map_entry_vlan10 = sai_thrift_create_tunnel_map_entry(
                self.client,
                tunnel_map_type=SAI_TUNNEL_MAP_TYPE_VNI_TO_VLAN_ID,
                tunnel_map=self.decap_tunnel_map_vlan,
                vlan_id_value=10,
                vni_id_key=self.vni1)

            decap_tun_map_entry_vlan20 = sai_thrift_create_tunnel_map_entry(
                self.client,
                tunnel_map_type=SAI_TUNNEL_MAP_TYPE_VNI_TO_VLAN_ID,
                tunnel_map=self.decap_tunnel_map_vlan,
                vlan_id_value=20,
                vni_id_key=self.vni2)

            # forwarding in VLAN 10 (tunnel 1 ->)
            pkt = simple_udp_packet(eth_dst=self.customer1_mac,
                                    eth_src=self.customer3_mac,
                                    ip_dst=self.customer1_ip,
                                    ip_src=self.customer3_ip,
                                    ip_id=108,
                                    ip_ttl=64,
                                    pktlen=100)
            vxlan_pkt = simple_vxlan_packet(eth_dst=ROUTER_MAC,
                                            eth_src=self.unbor1_mac,
                                            ip_dst=self.lpb_ip,
                                            ip_src=self.tun1_ip,
                                            ip_id=0,
                                            ip_ttl=64,
                                            ip_flags=0x2,
                                            with_udp_chksum=False,
                                            vxlan_vni=self.vni1,
                                            inner_frame=pkt)

            print("Sending packet from VNI %d -> untagged port %d in VLAN 10"
                  % (self.vni1, self.oport0_dev))
            send_packet(self, self.uport1_dev, vxlan_pkt)
            verify_packet(self, pkt, self.oport0_dev)
            print('Packet was L2 forwarded out of tunnel 1 '
                  'with correct VLAN ID')

            tag_pkt = simple_udp_packet(eth_dst=self.customer2_mac,
                                        eth_src=self.customer3_mac,
                                        ip_dst=self.customer2_ip,
                                        ip_src=self.customer3_ip,
                                        dl_vlan_enable=True,
                                        vlan_vid=10,
                                        ip_id=108,
                                        ip_ttl=64,
                                        pktlen=104)
            pkt = simple_udp_packet(eth_dst=self.customer2_mac,
                                    eth_src=self.customer3_mac,
                                    ip_dst=self.customer2_ip,
                                    ip_src=self.customer3_ip,
                                    ip_id=108,
                                    ip_ttl=64,
                                    pktlen=100)
            vxlan_pkt = simple_vxlan_packet(eth_dst=ROUTER_MAC,
                                            eth_src=self.unbor1_mac,
                                            ip_dst=self.lpb_ip,
                                            ip_src=self.tun1_ip,
                                            ip_id=0,
                                            ip_ttl=64,
                                            ip_flags=0x2,
                                            with_udp_chksum=False,
                                            vxlan_vni=self.vni1,
                                            inner_frame=pkt)

            print("Sending packet from VNI %d -> tagged port %d in VLAN 10"
                  % (self.vni1, self.oport1_dev))
            send_packet(self, self.uport1_dev, vxlan_pkt)
            verify_packet(self, tag_pkt, self.oport1_dev)
            print('Packet was L2 forwarded out of tunnel 1 '
                  'with correct VLAN ID')

            # forwarding in VLAN 10 (tunnel 2 ->)
            pkt = simple_udp_packet(eth_dst=self.customer1_mac,
                                    eth_src=self.customer6_mac,
                                    ip_dst=self.customer1_ip,
                                    ip_src=self.customer6_ip,
                                    ip_id=108,
                                    ip_ttl=64,
                                    pktlen=100)
            vxlan_pkt = simple_vxlan_packet(eth_dst=ROUTER_MAC,
                                            eth_src=self.unbor2_mac,
                                            ip_dst=self.lpb_ip,
                                            ip_src=self.tun2_ip,
                                            ip_id=0,
                                            ip_ttl=64,
                                            ip_flags=0x2,
                                            with_udp_chksum=False,
                                            vxlan_vni=self.vni1,
                                            inner_frame=pkt)

            print("Sending packet from VNI %d -> untagged port %d in VLAN 10"
                  % (self.vni1, self.oport0_dev))
            send_packet(self, self.uport2_dev, vxlan_pkt)
            verify_packet(self, pkt, self.oport0_dev)
            print('Packet was L2 forwarded out of tunnel 2 '
                  'with correct VLAN ID')

            tag_pkt = simple_udp_packet(eth_dst=self.customer2_mac,
                                        eth_src=self.customer6_mac,
                                        ip_dst=self.customer2_ip,
                                        ip_src=self.customer6_ip,
                                        dl_vlan_enable=True,
                                        vlan_vid=10,
                                        ip_id=108,
                                        ip_ttl=64,
                                        pktlen=104)
            pkt = simple_udp_packet(eth_dst=self.customer2_mac,
                                    eth_src=self.customer6_mac,
                                    ip_dst=self.customer2_ip,
                                    ip_src=self.customer6_ip,
                                    ip_id=108,
                                    ip_ttl=64,
                                    pktlen=100)
            vxlan_pkt = simple_vxlan_packet(eth_dst=ROUTER_MAC,
                                            eth_src=self.unbor2_mac,
                                            ip_dst=self.lpb_ip,
                                            ip_src=self.tun2_ip,
                                            ip_id=0,
                                            ip_ttl=64,
                                            ip_flags=0x2,
                                            with_udp_chksum=False,
                                            vxlan_vni=self.vni1,
                                            inner_frame=pkt)

            print("Sending packet from VNI %d -> tagged port %d in VLAN 10"
                  % (self.vni1, self.oport1_dev))
            send_packet(self, self.uport2_dev, vxlan_pkt)
            verify_packet(self, tag_pkt, self.oport1_dev)
            print('Packet was L2 forwarded out of tunnel 2 '
                  'with correct VLAN ID')

            # forwarding in VLAN 20
            pkt = simple_udp_packet(eth_dst=self.customer4_mac,
                                    eth_src=self.customer6_mac,
                                    ip_dst=self.customer4_ip,
                                    ip_src=self.customer6_ip,
                                    ip_id=108,
                                    ip_ttl=64,
                                    pktlen=100)
            vxlan_pkt = simple_vxlan_packet(eth_dst=ROUTER_MAC,
                                            eth_src=self.unbor2_mac,
                                            ip_dst=self.lpb_ip,
                                            ip_src=self.tun2_ip,
                                            ip_id=0,
                                            ip_ttl=64,
                                            ip_flags=0x2,
                                            with_udp_chksum=False,
                                            vxlan_vni=self.vni2,
                                            inner_frame=pkt)

            print("Sending packet from VNI %d -> untagged port %d in VLAN 20"
                  % (self.vni2, self.oport2_dev))
            send_packet(self, self.uport2_dev, vxlan_pkt)
            verify_packet(self, pkt, self.oport2_dev)
            print("Packet was L2 forwarded with correct VLAN ID")

            tag_pkt = simple_udp_packet(eth_dst=self.customer5_mac,
                                        eth_src=self.customer6_mac,
                                        ip_dst=self.customer5_ip,
                                        ip_src=self.customer6_ip,
                                        dl_vlan_enable=True,
                                        vlan_vid=20,
                                        ip_id=108,
                                        ip_ttl=64,
                                        pktlen=104)
            pkt = simple_udp_packet(eth_dst=self.customer5_mac,
                                    eth_src=self.customer6_mac,
                                    ip_dst=self.customer5_ip,
                                    ip_src=self.customer6_ip,
                                    ip_id=108,
                                    ip_ttl=64,
                                    pktlen=100)
            vxlan_pkt = simple_vxlan_packet(eth_dst=ROUTER_MAC,
                                            eth_src=self.unbor2_mac,
                                            ip_dst=self.lpb_ip,
                                            ip_src=self.tun2_ip,
                                            ip_id=0,
                                            ip_ttl=64,
                                            ip_flags=0x2,
                                            with_udp_chksum=False,
                                            vxlan_vni=self.vni2,
                                            inner_frame=pkt)

            print("Sending packet from VNI %d -> tagged port %d in VLAN 20"
                  % (self.vni2, self.oport3_dev))
            send_packet(self, self.uport2_dev, vxlan_pkt)
            verify_packet(self, tag_pkt, self.oport3_dev)
            print("Packet was L2 forwarded with correct VLAN ID")

            new_vni = 2222
            print("\n\tChanging decap mapper entry for VLAN 20 - new VNI=%d\n"
                  % new_vni)
            sai_thrift_remove_tunnel_map_entry(self.client,
                                               decap_tun_map_entry_vlan20)
            decap_tun_map_entry_vlan20 = sai_thrift_create_tunnel_map_entry(
                self.client,
                tunnel_map_type=SAI_TUNNEL_MAP_TYPE_VNI_TO_VLAN_ID,
                tunnel_map=self.decap_tunnel_map_vlan,
                vlan_id_value=20,
                vni_id_key=new_vni)

            pkt = simple_udp_packet(eth_dst=self.customer4_mac,
                                    eth_src=self.customer6_mac,
                                    ip_dst=self.customer4_ip,
                                    ip_src=self.customer6_ip,
                                    ip_id=108,
                                    ip_ttl=64,
                                    pktlen=100)
            vxlan_pkt = simple_vxlan_packet(eth_dst=ROUTER_MAC,
                                            eth_src=self.unbor2_mac,
                                            ip_dst=self.lpb_ip,
                                            ip_src=self.tun2_ip,
                                            ip_id=0,
                                            ip_ttl=64,
                                            ip_flags=0x2,
                                            with_udp_chksum=False,
                                            vxlan_vni=new_vni,
                                            inner_frame=pkt)

            print("Sending packet from VNI %d -> untagged port %d in VLAN 20"
                  % (new_vni, self.oport2_dev))
            send_packet(self, self.uport2_dev, vxlan_pkt)
            verify_packet(self, pkt, self.oport2_dev)
            print("Packet was L2 forwarded with correct VLAN ID")

            tag_pkt = simple_udp_packet(eth_dst=self.customer5_mac,
                                        eth_src=self.customer6_mac,
                                        ip_dst=self.customer5_ip,
                                        ip_src=self.customer6_ip,
                                        dl_vlan_enable=True,
                                        vlan_vid=20,
                                        ip_id=108,
                                        ip_ttl=64,
                                        pktlen=104)
            pkt = simple_udp_packet(eth_dst=self.customer5_mac,
                                    eth_src=self.customer6_mac,
                                    ip_dst=self.customer5_ip,
                                    ip_src=self.customer6_ip,
                                    ip_id=108,
                                    ip_ttl=64,
                                    pktlen=100)
            vxlan_pkt = simple_vxlan_packet(eth_dst=ROUTER_MAC,
                                            eth_src=self.unbor2_mac,
                                            ip_dst=self.lpb_ip,
                                            ip_src=self.tun2_ip,
                                            ip_id=0,
                                            ip_ttl=64,
                                            ip_flags=0x2,
                                            with_udp_chksum=False,
                                            vxlan_vni=new_vni,
                                            inner_frame=pkt)

            print("Sending packet from VNI %d -> tagged port %d in VLAN 20"
                  % (new_vni, self.oport3_dev))
            send_packet(self, self.uport2_dev, vxlan_pkt)
            verify_packet(self, tag_pkt, self.oport3_dev)
            print("Packet was L2 forwarded with correct VLAN ID")

        finally:
            sai_thrift_remove_tunnel_map_entry(self.client,
                                               decap_tun_map_entry_vlan20)
            sai_thrift_remove_tunnel_map_entry(self.client,
                                               decap_tun_map_entry_vlan10)

    def l2VxlanVlanToVniMappingFloodTest(self):
        '''
        Verify packets on different VLANs get mapped to different VNIs
        and packets are flooded to all ports, tunnels and LAGs in the VLAN
        '''
        print("\nl2VxlanVlanToVniMappingFloodTest()")

        sai_thrift_flush_fdb_entries(
            self.client, entry_type=SAI_FDB_FLUSH_ENTRY_TYPE_ALL)

        try:
            # tunnel map entries for VLAN 10 and VLAN 20
            encap_tun_map_entry_vlan10 = sai_thrift_create_tunnel_map_entry(
                self.client,
                tunnel_map_type=SAI_TUNNEL_MAP_TYPE_VLAN_ID_TO_VNI,
                tunnel_map=self.encap_tunnel_map_vlan,
                vlan_id_key=10,
                vni_id_value=self.vni1)

            encap_tun_map_entry_vlan20 = sai_thrift_create_tunnel_map_entry(
                self.client,
                tunnel_map_type=SAI_TUNNEL_MAP_TYPE_VLAN_ID_TO_VNI,
                tunnel_map=self.encap_tunnel_map_vlan,
                vlan_id_key=20,
                vni_id_value=self.vni2)

            # flooding in VLAN 10
            pkt = simple_udp_packet(eth_dst=self.customer3_mac,
                                    eth_src=self.customer1_mac,
                                    ip_dst=self.customer3_ip,
                                    ip_src=self.customer1_ip,
                                    ip_id=108,
                                    ip_ttl=64,
                                    pktlen=100)
            # expected packets
            tag_pkt = simple_udp_packet(eth_dst=self.customer3_mac,
                                        eth_src=self.customer1_mac,
                                        ip_dst=self.customer3_ip,
                                        ip_src=self.customer1_ip,
                                        dl_vlan_enable=True,
                                        vlan_vid=10,
                                        ip_id=108,
                                        ip_ttl=64,
                                        pktlen=104)
            vxlan_pkt1 = Mask(
                simple_vxlan_packet(eth_dst=self.unbor1_mac,
                                    eth_src=ROUTER_MAC,
                                    ip_dst=self.tun1_ip,
                                    ip_src=self.lpb_ip,
                                    ip_id=0,
                                    ip_ttl=64,
                                    ip_flags=0x2,
                                    with_udp_chksum=False,
                                    vxlan_vni=self.vni1,
                                    inner_frame=pkt))
            vxlan_pkt1.set_do_not_care_scapy(UDP, 'sport')
            vxlan_pkt2 = Mask(
                simple_vxlan_packet(eth_dst=self.unbor2_mac,
                                    eth_src=ROUTER_MAC,
                                    ip_dst=self.tun2_ip,
                                    ip_src=self.lpb_ip,
                                    ip_id=0,
                                    ip_ttl=64,
                                    ip_flags=0x2,
                                    with_udp_chksum=False,
                                    vxlan_vni=self.vni1,
                                    inner_frame=pkt))
            vxlan_pkt2.set_do_not_care_scapy(UDP, 'sport')

            print("Sending packet on untagged port %d in VLAN 10 -> VNI %d"
                  % (self.oport0_dev, self.vni1))
            send_packet(self, self.oport0_dev, pkt)
            verify_each_packet_on_multiple_port_lists(
                self, [tag_pkt, pkt, vxlan_pkt1, vxlan_pkt2],
                [[self.dev_port1],
                 [self.dev_port4, self.dev_port5, self.dev_port6],
                 [self.uport1_dev], [self.uport2_dev]])
            print("Packet was L2 flooded with correct VNI")

            tag_pkt = simple_udp_packet(eth_dst=self.customer3_mac,
                                        eth_src=self.customer2_mac,
                                        ip_dst=self.customer3_ip,
                                        ip_src=self.customer2_ip,
                                        dl_vlan_enable=True,
                                        vlan_vid=10,
                                        ip_id=108,
                                        ip_ttl=64,
                                        pktlen=104)
            pkt = simple_udp_packet(eth_dst=self.customer3_mac,
                                    eth_src=self.customer2_mac,
                                    ip_dst=self.customer3_ip,
                                    ip_src=self.customer2_ip,
                                    ip_id=108,
                                    ip_ttl=64,
                                    pktlen=100)
            vxlan_pkt1 = Mask(
                simple_vxlan_packet(eth_dst=self.unbor1_mac,
                                    eth_src=ROUTER_MAC,
                                    ip_dst=self.tun1_ip,
                                    ip_src=self.lpb_ip,
                                    ip_id=0,
                                    ip_ttl=64,
                                    ip_flags=0x2,
                                    with_udp_chksum=False,
                                    vxlan_vni=self.vni1,
                                    inner_frame=pkt))
            vxlan_pkt1.set_do_not_care_scapy(UDP, 'sport')
            vxlan_pkt2 = Mask(
                simple_vxlan_packet(eth_dst=self.unbor2_mac,
                                    eth_src=ROUTER_MAC,
                                    ip_dst=self.tun2_ip,
                                    ip_src=self.lpb_ip,
                                    ip_id=0,
                                    ip_ttl=64,
                                    ip_flags=0x2,
                                    with_udp_chksum=False,
                                    vxlan_vni=self.vni1,
                                    inner_frame=pkt))
            vxlan_pkt2.set_do_not_care_scapy(UDP, 'sport')

            print("Sending packet on tagged port %d in VLAN 10 -> VNI %d"
                  % (self.oport1_dev, self.vni1))
            send_packet(self, self.oport1_dev, tag_pkt)
            verify_each_packet_on_multiple_port_lists(
                self, [pkt, pkt, vxlan_pkt1, vxlan_pkt2],
                [[self.dev_port0],
                 [self.dev_port4, self.dev_port5, self.dev_port6],
                 [self.uport1_dev], [self.uport2_dev]])
            print("Packet was L2 flooded with correct VNI")

            # flooding in VLAN 20
            pkt = simple_udp_packet(eth_dst=self.customer6_mac,
                                    eth_src=self.customer4_mac,
                                    ip_dst=self.customer6_ip,
                                    ip_src=self.customer4_ip,
                                    ip_id=108,
                                    ip_ttl=64,
                                    pktlen=100)
            tag_pkt = simple_udp_packet(eth_dst=self.customer6_mac,
                                        eth_src=self.customer4_mac,
                                        ip_dst=self.customer6_ip,
                                        ip_src=self.customer4_ip,
                                        dl_vlan_enable=True,
                                        vlan_vid=20,
                                        ip_id=108,
                                        ip_ttl=64,
                                        pktlen=104)
            vxlan_pkt = Mask(
                simple_vxlan_packet(eth_dst=self.unbor2_mac,
                                    eth_src=ROUTER_MAC,
                                    ip_dst=self.tun2_ip,
                                    ip_src=self.lpb_ip,
                                    ip_id=0,
                                    ip_ttl=64,
                                    ip_flags=0x2,
                                    with_udp_chksum=False,
                                    vxlan_vni=self.vni2,
                                    inner_frame=pkt))
            vxlan_pkt.set_do_not_care_scapy(UDP, 'sport')

            print("Sending packet on untagged port %d in VLAN 20 -> VNI %d"
                  % (self.oport2_dev, self.vni2))
            send_packet(self, self.oport2_dev, pkt)
            verify_each_packet_on_multiple_port_lists(
                self, [tag_pkt, tag_pkt, vxlan_pkt],
                [[self.dev_port3],
                 [self.dev_port7, self.dev_port8, self.dev_port9],
                 [self.uport2_dev]])
            print("Packet was L2 flooded with correct VNI")

            tag_pkt = simple_udp_packet(eth_dst=self.customer6_mac,
                                        eth_src=self.customer5_mac,
                                        ip_dst=self.customer6_ip,
                                        ip_src=self.customer5_ip,
                                        dl_vlan_enable=True,
                                        vlan_vid=20,
                                        ip_id=108,
                                        ip_ttl=64,
                                        pktlen=104)
            pkt = simple_udp_packet(eth_dst=self.customer6_mac,
                                    eth_src=self.customer5_mac,
                                    ip_dst=self.customer6_ip,
                                    ip_src=self.customer5_ip,
                                    ip_id=108,
                                    ip_ttl=64,
                                    pktlen=100)
            vxlan_pkt = Mask(
                simple_vxlan_packet(eth_dst=self.unbor2_mac,
                                    eth_src=ROUTER_MAC,
                                    ip_dst=self.tun2_ip,
                                    ip_src=self.lpb_ip,
                                    ip_id=0,
                                    ip_ttl=64,
                                    ip_flags=0x2,
                                    with_udp_chksum=False,
                                    vxlan_vni=self.vni2,
                                    inner_frame=pkt))
            vxlan_pkt.set_do_not_care_scapy(UDP, 'sport')

            print("Sending packet on tagged port %d in VLAN 20 -> VNI %d"
                  % (self.oport3_dev, self.vni2))
            send_packet(self, self.oport3_dev, tag_pkt)
            verify_each_packet_on_multiple_port_lists(
                self, [pkt, tag_pkt, vxlan_pkt],
                [[self.dev_port2],
                 [self.dev_port7, self.dev_port8, self.dev_port9],
                 [self.uport2_dev]])
            print("Packet was L2 flooded with correct VNI")

            new_vni = 2222
            print("\n\tChanging encap mapper entry for VLAN 20 - new VNI=%d\n"
                  % new_vni)
            sai_thrift_remove_tunnel_map_entry(self.client,
                                               encap_tun_map_entry_vlan20)
            encap_tun_map_entry_vlan20 = sai_thrift_create_tunnel_map_entry(
                self.client,
                tunnel_map_type=SAI_TUNNEL_MAP_TYPE_VLAN_ID_TO_VNI,
                tunnel_map=self.encap_tunnel_map_vlan,
                vlan_id_key=20,
                vni_id_value=new_vni)

            pkt = simple_udp_packet(eth_dst=self.customer6_mac,
                                    eth_src=self.customer4_mac,
                                    ip_dst=self.customer6_ip,
                                    ip_src=self.customer4_ip,
                                    ip_id=108,
                                    ip_ttl=64,
                                    pktlen=100)
            tag_pkt = simple_udp_packet(eth_dst=self.customer6_mac,
                                        eth_src=self.customer4_mac,
                                        ip_dst=self.customer6_ip,
                                        ip_src=self.customer4_ip,
                                        dl_vlan_enable=True,
                                        vlan_vid=20,
                                        ip_id=108,
                                        ip_ttl=64,
                                        pktlen=104)
            vxlan_pkt = Mask(
                simple_vxlan_packet(eth_dst=self.unbor2_mac,
                                    eth_src=ROUTER_MAC,
                                    ip_dst=self.tun2_ip,
                                    ip_src=self.lpb_ip,
                                    ip_id=0,
                                    ip_ttl=64,
                                    ip_flags=0x2,
                                    with_udp_chksum=False,
                                    vxlan_vni=new_vni,
                                    inner_frame=pkt))
            vxlan_pkt.set_do_not_care_scapy(UDP, 'sport')

            print("Sending packet on untagged port %d in VLAN 20 -> VNI %d"
                  % (self.oport2_dev, new_vni))
            send_packet(self, self.oport2_dev, pkt)
            verify_each_packet_on_multiple_port_lists(
                self, [tag_pkt, tag_pkt, vxlan_pkt],
                [[self.dev_port3],
                 [self.dev_port7, self.dev_port8, self.dev_port9],
                 [self.uport2_dev]])
            print("Packet was L2 flooded with correct VNI")

            tag_pkt = simple_udp_packet(eth_dst=self.customer6_mac,
                                        eth_src=self.customer5_mac,
                                        ip_dst=self.customer6_ip,
                                        ip_src=self.customer5_ip,
                                        dl_vlan_enable=True,
                                        vlan_vid=20,
                                        ip_id=108,
                                        ip_ttl=64,
                                        pktlen=104)
            pkt = simple_udp_packet(eth_dst=self.customer6_mac,
                                    eth_src=self.customer5_mac,
                                    ip_dst=self.customer6_ip,
                                    ip_src=self.customer5_ip,
                                    ip_id=108,
                                    ip_ttl=64,
                                    pktlen=100)
            vxlan_pkt = Mask(
                simple_vxlan_packet(eth_dst=self.unbor2_mac,
                                    eth_src=ROUTER_MAC,
                                    ip_dst=self.tun2_ip,
                                    ip_src=self.lpb_ip,
                                    ip_id=0,
                                    ip_ttl=64,
                                    ip_flags=0x2,
                                    with_udp_chksum=False,
                                    vxlan_vni=new_vni,
                                    inner_frame=pkt))
            vxlan_pkt.set_do_not_care_scapy(UDP, 'sport')

            print("Sending packet on tagged port %d in VLAN 20 -> VNI %d"
                  % (self.oport3_dev, new_vni))
            send_packet(self, self.oport3_dev, tag_pkt)
            verify_each_packet_on_multiple_port_lists(
                self, [pkt, tag_pkt, vxlan_pkt],
                [[self.dev_port2],
                 [self.dev_port7, self.dev_port8, self.dev_port9],
                 [self.uport2_dev]])
            print("Packet was L2 flooded with correct VNI")

        finally:
            sai_thrift_remove_tunnel_map_entry(self.client,
                                               encap_tun_map_entry_vlan20)
            sai_thrift_remove_tunnel_map_entry(self.client,
                                               encap_tun_map_entry_vlan10)

    def l2VxlanVniToVlanMappingFloodTest(self):
        '''
        Verify packets on different VNIs get mapped to different VLANs
        and packets are flooded to all ports and LAGs in the VLAN and not to
        tunnels belonging to the VLAN
        '''
        print("\nl2VxlanVniToVlanMappingFloodTest()")

        sai_thrift_flush_fdb_entries(
            self.client, entry_type=SAI_FDB_FLUSH_ENTRY_TYPE_ALL)

        try:
            # tunnel map entries for VLAN 10 and VLAN 20
            decap_tun_map_entry_vlan10 = sai_thrift_create_tunnel_map_entry(
                self.client,
                tunnel_map_type=SAI_TUNNEL_MAP_TYPE_VNI_TO_VLAN_ID,
                tunnel_map=self.decap_tunnel_map_vlan,
                vlan_id_value=10,
                vni_id_key=self.vni1)

            decap_tun_map_entry_vlan20 = sai_thrift_create_tunnel_map_entry(
                self.client,
                tunnel_map_type=SAI_TUNNEL_MAP_TYPE_VNI_TO_VLAN_ID,
                tunnel_map=self.decap_tunnel_map_vlan,
                vlan_id_value=20,
                vni_id_key=self.vni2)

            # flooding in VLAN 10
            pkt = simple_udp_packet(eth_dst=self.customer1_mac,
                                    eth_src=self.customer3_mac,
                                    ip_dst=self.customer1_ip,
                                    ip_src=self.customer3_ip,
                                    ip_id=108,
                                    ip_ttl=64,
                                    pktlen=100)
            tag_pkt = simple_udp_packet(eth_dst=self.customer1_mac,
                                        eth_src=self.customer3_mac,
                                        ip_dst=self.customer1_ip,
                                        ip_src=self.customer3_ip,
                                        dl_vlan_enable=True,
                                        vlan_vid=10,
                                        ip_id=108,
                                        ip_ttl=64,
                                        pktlen=104)
            vxlan_pkt = simple_vxlan_packet(eth_dst=ROUTER_MAC,
                                            eth_src=self.unbor1_mac,
                                            ip_dst=self.lpb_ip,
                                            ip_src=self.tun1_ip,
                                            ip_id=0,
                                            ip_ttl=64,
                                            ip_flags=0x2,
                                            with_udp_chksum=False,
                                            vxlan_vni=self.vni1,
                                            inner_frame=pkt)

            print("Sending packet from VNI %d - flooded into VLAN 10"
                  % (self.vni1))
            send_packet(self, self.uport1_dev, vxlan_pkt)
            verify_each_packet_on_multiple_port_lists(
                self, [pkt, tag_pkt, pkt],
                [[self.dev_port0],
                 [self.dev_port1],
                 [self.dev_port4, self.dev_port5, self.dev_port6]])
            print("Packet was L2 flooded with correct VNI")

            # flooding in VLAN 20
            pkt = simple_udp_packet(eth_dst=self.customer4_mac,
                                    eth_src=self.customer6_mac,
                                    ip_dst=self.customer4_ip,
                                    ip_src=self.customer6_ip,
                                    ip_id=108,
                                    ip_ttl=64,
                                    pktlen=100)
            tag_pkt = simple_udp_packet(eth_dst=self.customer4_mac,
                                        eth_src=self.customer6_mac,
                                        ip_dst=self.customer4_ip,
                                        ip_src=self.customer6_ip,
                                        dl_vlan_enable=True,
                                        vlan_vid=20,
                                        ip_id=108,
                                        ip_ttl=64,
                                        pktlen=104)
            vxlan_pkt = simple_vxlan_packet(eth_dst=ROUTER_MAC,
                                            eth_src=self.unbor2_mac,
                                            ip_dst=self.lpb_ip,
                                            ip_src=self.tun2_ip,
                                            ip_id=0,
                                            ip_ttl=64,
                                            ip_flags=0x2,
                                            with_udp_chksum=False,
                                            vxlan_vni=self.vni2,
                                            inner_frame=pkt)

            print("Sending packet from VNI %d -> flooded into VLAN 20"
                  % (self.vni2))
            send_packet(self, self.uport2_dev, vxlan_pkt)
            verify_each_packet_on_multiple_port_lists(
                self, [pkt, tag_pkt, tag_pkt],
                [[self.dev_port2],
                 [self.dev_port3],
                 [self.dev_port7, self.dev_port8, self.dev_port9]])
            print("Packet was L2 flooded with correct VNI")

            new_vni = 2222
            print("\n\tChanging decap mapper entry for VLAN 20 - new VNI=%d\n"
                  % new_vni)
            sai_thrift_remove_tunnel_map_entry(self.client,
                                               decap_tun_map_entry_vlan20)
            decap_tun_map_entry_vlan20 = sai_thrift_create_tunnel_map_entry(
                self.client,
                tunnel_map_type=SAI_TUNNEL_MAP_TYPE_VNI_TO_VLAN_ID,
                tunnel_map=self.decap_tunnel_map_vlan,
                vlan_id_value=20,
                vni_id_key=new_vni)

            pkt = simple_udp_packet(eth_dst=self.customer4_mac,
                                    eth_src=self.customer6_mac,
                                    ip_dst=self.customer4_ip,
                                    ip_src=self.customer6_ip,
                                    ip_id=108,
                                    ip_ttl=64,
                                    pktlen=100)
            tag_pkt = simple_udp_packet(eth_dst=self.customer4_mac,
                                        eth_src=self.customer6_mac,
                                        ip_dst=self.customer4_ip,
                                        ip_src=self.customer6_ip,
                                        dl_vlan_enable=True,
                                        vlan_vid=20,
                                        ip_id=108,
                                        ip_ttl=64,
                                        pktlen=104)
            vxlan_pkt = simple_vxlan_packet(eth_dst=ROUTER_MAC,
                                            eth_src=self.unbor2_mac,
                                            ip_dst=self.lpb_ip,
                                            ip_src=self.tun2_ip,
                                            ip_id=0,
                                            ip_ttl=64,
                                            ip_flags=0x2,
                                            with_udp_chksum=False,
                                            vxlan_vni=new_vni,
                                            inner_frame=pkt)

            print("Sending packet from VNI %d -> flooded into VLAN 20"
                  % (new_vni))
            send_packet(self, self.uport2_dev, vxlan_pkt)
            verify_each_packet_on_multiple_port_lists(
                self, [pkt, tag_pkt, tag_pkt],
                [[self.dev_port2],
                 [self.dev_port3],
                 [self.dev_port7, self.dev_port8, self.dev_port9]])
            print("Packet was L2 flooded with correct VNI")

        finally:
            sai_thrift_remove_tunnel_map_entry(self.client,
                                               decap_tun_map_entry_vlan20)
            sai_thrift_remove_tunnel_map_entry(self.client,
                                               decap_tun_map_entry_vlan10)


@group('l2-vxlan')
class L2VxLanFloodTest(SaiHelper):
    '''
    This class contains tests for L2 VxLan flooding
    '''

    def setUp(self):
        super(L2VxLanFloodTest, self).setUp()

        self.tun1_ip = "10.0.0.65"
        self.tun2_ip = "10.0.0.33"
        self.tun3_ip = "10.0.0.17"
        self.lpb_ip = "10.1.0.32"
        self.tun_ip_mask = "/32"

        self.oport1 = self.port0
        self.oport1_dev = self.dev_port0
        self.oport1_bp = self.port0_bp
        self.uport1_dev = self.dev_port10
        self.uport2_dev = self.dev_port11
        self.uport3_dev = self.dev_port12
        self.uport1_rif = self.port10_rif
        self.uport2_rif = self.port11_rif
        self.uport3_rif = self.port12_rif

        self.vni1 = 1000
        self.vlan10_myip = "192.168.100.254"
        self.customer1_ip = "192.168.100.1"
        self.customer1_ipv6 = "2001:0db8::100:2"
        self.customer2_ip = "192.168.100.8"
        self.customer2_ipv6 = "2001:0db8::100:9"
        self.customer3_ip = "192.168.250.1"
        self.customer3_ipv6 = "2001:0db8::200:2"
        self.customer1_mac = "00:22:22:22:22:11"
        self.customer2_mac = "00:22:22:22:22:22"
        self.customer3_mac = "00:22:22:22:22:33"
        self.unbor1_mac = "00:33:33:33:33:11"
        self.unbor2_mac = "00:33:33:33:33:22"
        self.unbor3_mac = "00:33:33:33:33:33"

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
        sai_thrift_create_neighbor_entry(self.client,
                                         self.unbor1,
                                         dst_mac_address=self.unbor1_mac)
        self.unhop1 = sai_thrift_create_next_hop(
            self.client,
            type=SAI_NEXT_HOP_TYPE_IP,
            ip=sai_ipaddress(self.tun1_ip),
            router_interface_id=self.uport1_rif)

        self.unbor2 = sai_thrift_neighbor_entry_t(
            ip_address=sai_ipaddress(self.tun2_ip),
            rif_id=self.uport2_rif,
            switch_id=self.switch_id)
        sai_thrift_create_neighbor_entry(self.client,
                                         self.unbor2,
                                         dst_mac_address=self.unbor2_mac)
        self.unhop2 = sai_thrift_create_next_hop(
            self.client,
            type=SAI_NEXT_HOP_TYPE_IP,
            ip=sai_ipaddress(self.tun2_ip),
            router_interface_id=self.uport2_rif)

        self.unbor3 = sai_thrift_neighbor_entry_t(
            ip_address=sai_ipaddress(self.tun3_ip),
            rif_id=self.uport3_rif,
            switch_id=self.switch_id)
        sai_thrift_create_neighbor_entry(self.client,
                                         self.unbor3,
                                         dst_mac_address=self.unbor3_mac,
                                         no_host_route=True)
        self.unhop3 = sai_thrift_create_next_hop(
            self.client,
            type=SAI_NEXT_HOP_TYPE_IP,
            ip=sai_ipaddress(self.tun3_ip),
            router_interface_id=self.uport3_rif)
        # separate configurable route for unbor3
        self.unbor3_route = sai_thrift_route_entry_t(
            destination=sai_ipprefix(self.tun3_ip + self.tun_ip_mask),
            switch_id=self.switch_id,
            vr_id=self.default_vrf)
        sai_thrift_create_route_entry(self.client,
                                      self.unbor3_route,
                                      next_hop_id=self.unhop3)

        # L2 VxLAN configuration below
        tunnel_type = SAI_TUNNEL_TYPE_VXLAN
        term_type = SAI_TUNNEL_TERM_TABLE_ENTRY_TYPE_P2MP

        # create tunnel maps
        self.decap_tunnel_map_vlan = sai_thrift_create_tunnel_map(
            self.client, type=SAI_TUNNEL_MAP_TYPE_VNI_TO_VLAN_ID)

        self.encap_tunnel_map_vlan = sai_thrift_create_tunnel_map(
            self.client, type=SAI_TUNNEL_MAP_TYPE_VLAN_ID_TO_VNI)

        # create tunnel
        decap_maps = sai_thrift_object_list_t(
            count=1,
            idlist=[self.decap_tunnel_map_vlan])
        encap_maps = sai_thrift_object_list_t(
            count=1,
            idlist=[self.encap_tunnel_map_vlan])

        self.p2mp_tunnel = sai_thrift_create_tunnel(
            self.client,
            type=tunnel_type,
            underlay_interface=self.urif_lpb,
            decap_mappers=decap_maps,
            encap_mappers=encap_maps,
            encap_src_ip=sai_ipaddress(self.lpb_ip),
            peer_mode=SAI_TUNNEL_PEER_MODE_P2MP)

        self.tunnel_term = sai_thrift_create_tunnel_term_table_entry(
            self.client,
            type=term_type,
            vr_id=self.default_vrf,
            dst_ip=sai_ipaddress(self.lpb_ip),
            tunnel_type=tunnel_type,
            action_tunnel_id=self.p2mp_tunnel)

        # create tunnel map entries
        self.decap_tunnel_map_entry_vlan = sai_thrift_create_tunnel_map_entry(
            self.client,
            tunnel_map_type=SAI_TUNNEL_MAP_TYPE_VNI_TO_VLAN_ID,
            tunnel_map=self.decap_tunnel_map_vlan,
            vlan_id_value=10,
            vni_id_key=self.vni1)

        self.encap_tunnel_map_entry_vlan = sai_thrift_create_tunnel_map_entry(
            self.client,
            tunnel_map_type=SAI_TUNNEL_MAP_TYPE_VLAN_ID_TO_VNI,
            tunnel_map=self.encap_tunnel_map_vlan,
            vlan_id_key=10,
            vni_id_value=self.vni1)

        # create p2p tunnels
        self.p2p_tunnel1 = sai_thrift_create_tunnel(
            self.client,
            type=tunnel_type,
            underlay_interface=self.urif_lpb,
            decap_mappers=decap_maps,
            encap_mappers=encap_maps,
            encap_src_ip=sai_ipaddress(self.lpb_ip),
            peer_mode=SAI_TUNNEL_PEER_MODE_P2P,
            encap_dst_ip=sai_ipaddress(self.tun1_ip))
        self.p2p_tunnel2 = sai_thrift_create_tunnel(
            self.client,
            type=tunnel_type,
            underlay_interface=self.urif_lpb,
            decap_mappers=decap_maps,
            encap_mappers=encap_maps,
            encap_src_ip=sai_ipaddress(self.lpb_ip),
            peer_mode=SAI_TUNNEL_PEER_MODE_P2P,
            encap_dst_ip=sai_ipaddress(self.tun2_ip))
        self.p2p_tunnel3 = sai_thrift_create_tunnel(
            self.client,
            type=tunnel_type,
            underlay_interface=self.urif_lpb,
            decap_mappers=decap_maps,
            encap_mappers=encap_maps,
            encap_src_ip=sai_ipaddress(self.lpb_ip),
            peer_mode=SAI_TUNNEL_PEER_MODE_P2P,
            encap_dst_ip=sai_ipaddress(self.tun3_ip))

        # bridge port is created on p2p tunnel
        self.p2p_tun1_bp = sai_thrift_create_bridge_port(
            self.client,
            type=SAI_BRIDGE_PORT_TYPE_TUNNEL,
            tunnel_id=self.p2p_tunnel1,
            bridge_id=self.default_1q_bridge,
            admin_state=True,
            fdb_learning_mode=SAI_BRIDGE_PORT_FDB_LEARNING_MODE_DISABLE)
        self.p2p_tun2_bp = sai_thrift_create_bridge_port(
            self.client,
            type=SAI_BRIDGE_PORT_TYPE_TUNNEL,
            tunnel_id=self.p2p_tunnel2,
            bridge_id=self.default_1q_bridge,
            admin_state=True,
            fdb_learning_mode=SAI_BRIDGE_PORT_FDB_LEARNING_MODE_DISABLE)
        self.p2p_tun3_bp = sai_thrift_create_bridge_port(
            self.client,
            type=SAI_BRIDGE_PORT_TYPE_TUNNEL,
            tunnel_id=self.p2p_tunnel3,
            bridge_id=self.default_1q_bridge,
            admin_state=True,
            fdb_learning_mode=SAI_BRIDGE_PORT_FDB_LEARNING_MODE_DISABLE)

        # vlan_member is created using tunnel bridge_port
        self.tun1_vlan_member = sai_thrift_create_vlan_member(
            self.client,
            vlan_id=self.vlan10,
            bridge_port_id=self.p2p_tun1_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)
        self.tun2_vlan_member = sai_thrift_create_vlan_member(
            self.client,
            vlan_id=self.vlan10,
            bridge_port_id=self.p2p_tun2_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)
        self.tun3_vlan_member = sai_thrift_create_vlan_member(
            self.client,
            vlan_id=self.vlan10,
            bridge_port_id=self.p2p_tun3_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)

    def runTest(self):
        try:
            self.l2VxLanFloodTest()
            self.l2VxLanFloodRemoteVtepRouteTest()
            self.l2VxLanFloodFdbEntryTest()
            self.l2VxLanFloodLAGTest()
            self.l2VxLanFloodECMPTest()
            self.l2VxLanFloodECMPAdditionTest()
        finally:
            pass

    def tearDown(self):
        sai_thrift_flush_fdb_entries(
            self.client, entry_type=SAI_FDB_FLUSH_ENTRY_TYPE_ALL)

        sai_thrift_remove_vlan_member(self.client, self.tun1_vlan_member)
        sai_thrift_remove_vlan_member(self.client, self.tun2_vlan_member)
        sai_thrift_remove_vlan_member(self.client, self.tun3_vlan_member)
        sai_thrift_remove_bridge_port(self.client, self.p2p_tun1_bp)
        sai_thrift_remove_bridge_port(self.client, self.p2p_tun2_bp)
        sai_thrift_remove_bridge_port(self.client, self.p2p_tun3_bp)
        sai_thrift_remove_tunnel(self.client, self.p2p_tunnel1)
        sai_thrift_remove_tunnel(self.client, self.p2p_tunnel2)
        sai_thrift_remove_tunnel(self.client, self.p2p_tunnel3)
        sai_thrift_remove_tunnel_map_entry(
            self.client, self.decap_tunnel_map_entry_vlan)
        sai_thrift_remove_tunnel_map_entry(
            self.client, self.encap_tunnel_map_entry_vlan)
        sai_thrift_remove_tunnel_term_table_entry(
            self.client, self.tunnel_term)
        sai_thrift_remove_tunnel(self.client, self.p2mp_tunnel)
        sai_thrift_remove_tunnel_map(self.client, self.encap_tunnel_map_vlan)
        sai_thrift_remove_tunnel_map(self.client, self.decap_tunnel_map_vlan)

        sai_thrift_remove_route_entry(self.client, self.unbor3_route)
        sai_thrift_remove_next_hop(self.client, self.unhop1)
        sai_thrift_remove_next_hop(self.client, self.unhop2)
        sai_thrift_remove_next_hop(self.client, self.unhop3)
        sai_thrift_remove_neighbor_entry(self.client, self.unbor1)
        sai_thrift_remove_neighbor_entry(self.client, self.unbor2)
        sai_thrift_remove_neighbor_entry(self.client, self.unbor3)
        sai_thrift_remove_router_interface(self.client, self.urif_lpb)

        super(L2VxLanFloodTest, self).tearDown()

    def l2VxLanFloodTest(self):
        '''
        Verify flooding with correct tagging on tunnel vlan members and
        port vlan members and after adding and deleting tunnel vlan member.
        '''
        print("\nl2VxLanFloodTest()")

        try:
            # L2 encap + flooding into vlan
            pkt = simple_udp_packet(eth_dst=self.customer2_mac,
                                    eth_src=self.customer1_mac,
                                    ip_dst=self.customer2_ip,
                                    ip_src=self.customer1_ip,
                                    ip_id=108,
                                    ip_ttl=64,
                                    pktlen=100)
            # expected packets
            tag_pkt = simple_udp_packet(eth_dst=self.customer2_mac,
                                        eth_src=self.customer1_mac,
                                        ip_dst=self.customer2_ip,
                                        ip_src=self.customer1_ip,
                                        dl_vlan_enable=True,
                                        vlan_vid=10,
                                        ip_id=108,
                                        ip_ttl=64,
                                        pktlen=104)
            vxlan_pkt = Mask(
                simple_vxlan_packet(eth_dst=self.unbor1_mac,
                                    eth_src=ROUTER_MAC,
                                    ip_dst=self.tun1_ip,
                                    ip_src=self.lpb_ip,
                                    ip_id=0,
                                    ip_ttl=64,
                                    ip_flags=0x2,
                                    with_udp_chksum=False,
                                    vxlan_vni=self.vni1,
                                    inner_frame=pkt))
            vxlan_pkt.set_do_not_care_scapy(UDP, 'sport')
            vxlan_pkt2 = Mask(
                simple_vxlan_packet(eth_dst=self.unbor2_mac,
                                    eth_src=ROUTER_MAC,
                                    ip_dst=self.tun2_ip,
                                    ip_src=self.lpb_ip,
                                    ip_id=0,
                                    ip_ttl=64,
                                    ip_flags=0x2,
                                    with_udp_chksum=False,
                                    vxlan_vni=self.vni1,
                                    inner_frame=pkt))
            vxlan_pkt2.set_do_not_care_scapy(UDP, 'sport')
            vxlan_pkt3 = Mask(
                simple_vxlan_packet(eth_dst=self.unbor3_mac,
                                    eth_src=ROUTER_MAC,
                                    ip_dst=self.tun3_ip,
                                    ip_src=self.lpb_ip,
                                    ip_id=0,
                                    ip_ttl=64,
                                    ip_flags=0x2,
                                    with_udp_chksum=False,
                                    vxlan_vni=self.vni1,
                                    inner_frame=pkt))
            vxlan_pkt3.set_do_not_care_scapy(UDP, 'sport')

            print("Sending packet from port %d" % self.oport1_dev)
            send_packet(self, self.oport1_dev, pkt)
            verify_each_packet_on_multiple_port_lists(
                self, [tag_pkt, pkt, vxlan_pkt, vxlan_pkt2, vxlan_pkt3],
                [[self.dev_port1],
                 [self.dev_port4, self.dev_port5, self.dev_port6],
                 [self.uport1_dev],
                 [self.uport2_dev],
                 [self.uport3_dev]])
            print('Packet was flooded into L2 VXLAN tunnels '
                  'and physical ports')

            # remove a tunnel vlan member
            print("Removing tunnel vlan member")
            sai_thrift_remove_vlan_member(self.client, self.tun1_vlan_member)
            print("Sending packet from port %d" % self.oport1_dev)
            send_packet(self, self.oport1_dev, pkt)
            verify_each_packet_on_multiple_port_lists(
                self, [tag_pkt, pkt, vxlan_pkt2, vxlan_pkt3],
                [[self.dev_port1],
                 [self.dev_port4, self.dev_port5, self.dev_port6],
                 [self.uport2_dev], [self.uport3_dev]])
            print('Packet was flooded into L2 VXLAN tunnels '
                  'and physical ports')

            # add a tunnel vlan member
            print("Adding tunnel vlan member")
            self.tun1_vlan_member = sai_thrift_create_vlan_member(
                self.client,
                vlan_id=self.vlan10,
                bridge_port_id=self.p2p_tun1_bp,
                vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)

            print("Sending packet from port %d" % self.oport1_dev)
            send_packet(self, self.oport1_dev, pkt)
            verify_each_packet_on_multiple_port_lists(
                self, [tag_pkt, pkt, vxlan_pkt, vxlan_pkt2, vxlan_pkt3],
                [[self.dev_port1],
                 [self.dev_port4, self.dev_port5, self.dev_port6],
                 [self.uport1_dev], [self.uport2_dev], [self.uport3_dev]])
            print('Packet was flooded into L2 VXLAN tunnels '
                  'and physical ports')

            # L2 decap + flooding into vlan
            vxlan_pkt = simple_vxlan_packet(eth_dst=ROUTER_MAC,
                                            eth_src=self.unbor1_mac,
                                            ip_dst=self.lpb_ip,
                                            ip_src=self.tun1_ip,
                                            ip_id=0,
                                            ip_ttl=64,
                                            ip_flags=0x2,
                                            with_udp_chksum=False,
                                            vxlan_vni=self.vni1,
                                            inner_frame=pkt)

            print("Sending packet from VNI %d" % self.vni1)
            send_packet(self, self.uport1_dev, vxlan_pkt)
            verify_each_packet_on_multiple_port_lists(
                self, [pkt, tag_pkt, pkt],
                [[self.dev_port0], [self.dev_port1],
                 [self.dev_port4, self.dev_port5, self.dev_port6]])
            print("Packet was flooded out of L2 VXLAN tunnel")

            # remove a tunnel vlan member
            print("Removing tunnel vlan member")
            sai_thrift_remove_vlan_member(self.client, self.tun1_vlan_member)
            print("Sending packet from VNI %d" % self.vni1)
            send_packet(self, self.uport1_dev, vxlan_pkt)
            verify_each_packet_on_multiple_port_lists(
                self, [pkt, tag_pkt, pkt],
                [[self.dev_port0], [self.dev_port1],
                 [self.dev_port4, self.dev_port5, self.dev_port6]])
            print("Packet was flooded out of L2 VXLAN tunnel")

            # add a tunnel vlan member
            print("Adding tunnel vlan member")
            self.tun1_vlan_member = sai_thrift_create_vlan_member(
                self.client,
                vlan_id=self.vlan10,
                bridge_port_id=self.p2p_tun1_bp,
                vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)

            print("Sending packet from VNI %d" % self.vni1)
            send_packet(self, self.uport1_dev, vxlan_pkt)
            verify_each_packet_on_multiple_port_lists(
                self, [pkt, tag_pkt, pkt],
                [[self.dev_port0], [self.dev_port1],
                 [self.dev_port4, self.dev_port5, self.dev_port6]])
            print("Packet was flooded out of L2 VXLAN tunnel")

        finally:
            sai_thrift_flush_fdb_entries(
                self.client, entry_type=SAI_FDB_FLUSH_ENTRY_TYPE_ALL)

    def l2VxLanFloodRemoteVtepRouteTest(self):
        '''
        Verify flooding after underlay route to remote vtep is deleted and
        after it reappears.
        '''
        print("\nl2VxLanFloodRemoteVtepRouteTest()")

        try:
            # L2 encap + flooding into vlan
            pkt = simple_udp_packet(eth_dst=self.customer2_mac,
                                    eth_src=self.customer1_mac,
                                    ip_dst=self.customer2_ip,
                                    ip_src=self.customer1_ip,
                                    ip_id=108,
                                    ip_ttl=64,
                                    pktlen=100)
            # expected packets
            tag_pkt = simple_udp_packet(eth_dst=self.customer2_mac,
                                        eth_src=self.customer1_mac,
                                        ip_dst=self.customer2_ip,
                                        ip_src=self.customer1_ip,
                                        dl_vlan_enable=True,
                                        vlan_vid=10,
                                        ip_id=108,
                                        ip_ttl=64,
                                        pktlen=104)
            vxlan_pkt = Mask(
                simple_vxlan_packet(eth_dst=self.unbor1_mac,
                                    eth_src=ROUTER_MAC,
                                    ip_dst=self.tun1_ip,
                                    ip_src=self.lpb_ip,
                                    ip_id=0,
                                    ip_ttl=64,
                                    ip_flags=0x2,
                                    with_udp_chksum=False,
                                    vxlan_vni=self.vni1,
                                    inner_frame=pkt))
            vxlan_pkt.set_do_not_care_scapy(UDP, 'sport')
            vxlan_pkt2 = Mask(
                simple_vxlan_packet(eth_dst=self.unbor2_mac,
                                    eth_src=ROUTER_MAC,
                                    ip_dst=self.tun2_ip,
                                    ip_src=self.lpb_ip,
                                    ip_id=0,
                                    ip_ttl=64,
                                    ip_flags=0x2,
                                    with_udp_chksum=False,
                                    vxlan_vni=self.vni1,
                                    inner_frame=pkt))
            vxlan_pkt2.set_do_not_care_scapy(UDP, 'sport')
            vxlan_pkt3 = Mask(
                simple_vxlan_packet(eth_dst=self.unbor3_mac,
                                    eth_src=ROUTER_MAC,
                                    ip_dst=self.tun3_ip,
                                    ip_src=self.lpb_ip,
                                    ip_id=0,
                                    ip_ttl=64,
                                    ip_flags=0x2,
                                    with_udp_chksum=False,
                                    vxlan_vni=self.vni1,
                                    inner_frame=pkt))
            vxlan_pkt3.set_do_not_care_scapy(UDP, 'sport')

            print("Sending packet from port %d" % self.oport1_dev)
            send_packet(self, self.oport1_dev, pkt)
            verify_each_packet_on_multiple_port_lists(
                self, [tag_pkt, pkt, vxlan_pkt, vxlan_pkt2, vxlan_pkt3],
                [[self.dev_port1],
                 [self.dev_port4, self.dev_port5, self.dev_port6],
                 [self.uport1_dev], [self.uport2_dev], [self.uport3_dev]])
            print('Packet was flooded into L2 VXLAN tunnels '
                  'and physical ports')

            print("Removing route to remote vtep")
            sai_thrift_remove_route_entry(self.client, self.unbor3_route)

            print("Sending packet from port %d" % self.oport1_dev)
            send_packet(self, self.oport1_dev, pkt)
            verify_each_packet_on_multiple_port_lists(
                self, [tag_pkt, pkt, vxlan_pkt, vxlan_pkt2],
                [[self.dev_port1],
                 [self.dev_port4, self.dev_port5, self.dev_port6],
                 [self.uport1_dev], [self.uport2_dev]])
            print('Packet was flooded into L2 VXLAN tunnels '
                  'and physical ports')

            print("Adding route to remote vtep")
            # separate configurable route for unbor3
            self.unbor3_route = sai_thrift_route_entry_t(
                destination=sai_ipprefix(self.tun3_ip + self.tun_ip_mask),
                switch_id=self.switch_id,
                vr_id=self.default_vrf)
            sai_thrift_create_route_entry(self.client,
                                          self.unbor3_route,
                                          next_hop_id=self.unhop3)

            print("Sending packet from port %d" % self.oport1_dev)
            send_packet(self, self.oport1_dev, pkt)
            verify_each_packet_on_multiple_port_lists(
                self, [tag_pkt, pkt, vxlan_pkt, vxlan_pkt2, vxlan_pkt3],
                [[self.dev_port1],
                 [self.dev_port4, self.dev_port5, self.dev_port6],
                 [self.uport1_dev], [self.uport2_dev], [self.uport3_dev]])
            print('Packet was flooded into L2 VXLAN tunnels '
                  'and physical ports')

        finally:
            sai_thrift_flush_fdb_entries(
                self.client, entry_type=SAI_FDB_FLUSH_ENTRY_TYPE_ALL)

    def l2VxLanFloodFdbEntryTest(self):
        '''
        Verify known unicast fdb entry exists and flooding when it is deleted.
        '''
        print("\nl2VxLanFloodFdbEntryTest()")

        try:
            # L2 encap + flooding into vlan
            pkt = simple_udp_packet(eth_dst=self.customer2_mac,
                                    eth_src=self.customer1_mac,
                                    ip_dst=self.customer2_ip,
                                    ip_src=self.customer1_ip,
                                    ip_id=108,
                                    ip_ttl=64,
                                    pktlen=100)
            # expected packets
            tag_pkt = simple_udp_packet(eth_dst=self.customer2_mac,
                                        eth_src=self.customer1_mac,
                                        ip_dst=self.customer2_ip,
                                        ip_src=self.customer1_ip,
                                        dl_vlan_enable=True,
                                        vlan_vid=10,
                                        ip_id=108,
                                        ip_ttl=64,
                                        pktlen=104)
            vxlan_pkt = Mask(
                simple_vxlan_packet(eth_dst=self.unbor1_mac,
                                    eth_src=ROUTER_MAC,
                                    ip_dst=self.tun1_ip,
                                    ip_src=self.lpb_ip,
                                    ip_id=0,
                                    ip_ttl=64,
                                    ip_flags=0x2,
                                    with_udp_chksum=False,
                                    vxlan_vni=self.vni1,
                                    inner_frame=pkt))
            vxlan_pkt.set_do_not_care_scapy(UDP, 'sport')
            vxlan_pkt2 = Mask(
                simple_vxlan_packet(eth_dst=self.unbor2_mac,
                                    eth_src=ROUTER_MAC,
                                    ip_dst=self.tun2_ip,
                                    ip_src=self.lpb_ip,
                                    ip_id=0,
                                    ip_ttl=64,
                                    ip_flags=0x2,
                                    with_udp_chksum=False,
                                    vxlan_vni=self.vni1,
                                    inner_frame=pkt))
            vxlan_pkt2.set_do_not_care_scapy(UDP, 'sport')
            vxlan_pkt3 = Mask(
                simple_vxlan_packet(eth_dst=self.unbor3_mac,
                                    eth_src=ROUTER_MAC,
                                    ip_dst=self.tun3_ip,
                                    ip_src=self.lpb_ip,
                                    ip_id=0,
                                    ip_ttl=64,
                                    ip_flags=0x2,
                                    with_udp_chksum=False,
                                    vxlan_vni=self.vni1,
                                    inner_frame=pkt))
            vxlan_pkt3.set_do_not_care_scapy(UDP, 'sport')

            print("Sending packet from port %d" % self.oport1_dev)
            send_packet(self, self.oport1_dev, pkt)
            verify_each_packet_on_multiple_port_lists(
                self, [tag_pkt, pkt, vxlan_pkt, vxlan_pkt2, vxlan_pkt3],
                [[self.dev_port1],
                 [self.dev_port4, self.dev_port5, self.dev_port6],
                 [self.uport1_dev], [self.uport2_dev], [self.uport3_dev]])
            print('Packet was flooded into L2 VXLAN tunnels '
                  'and physical ports')

            print("Adding fdb entry")
            # fdb entry for L2 forwarding into VXLAN tunnel
            fdb_entry = sai_thrift_fdb_entry_t(
                bv_id=self.vlan10,
                mac_address=self.customer2_mac,
                switch_id=self.switch_id)
            sai_thrift_create_fdb_entry(self.client,
                                        fdb_entry,
                                        type=SAI_FDB_ENTRY_TYPE_STATIC,
                                        bridge_port_id=self.p2p_tun1_bp)

            print("Sending packet from port %d -> VNI %d" %
                  (self.oport1_dev, self.vni1))
            send_packet(self, self.oport1_dev, pkt)
            verify_packet(self, vxlan_pkt, self.uport1_dev)
            print("Packet was L2 forwarded into the VXLAN tunnel")

            print("Removing fdb entry")
            sai_thrift_remove_fdb_entry(self.client, fdb_entry)
            print("Sending packet from port %d" % self.oport1_dev)
            send_packet(self, self.oport1_dev, pkt)
            verify_each_packet_on_multiple_port_lists(
                self, [tag_pkt, pkt, vxlan_pkt, vxlan_pkt2, vxlan_pkt3],
                [[self.dev_port1],
                 [self.dev_port4, self.dev_port5, self.dev_port6],
                 [self.uport1_dev], [self.uport2_dev], [self.uport3_dev]])
            print('Packet was flooded into L2 VXLAN tunnels '
                  'and physical ports')

        finally:
            sai_thrift_flush_fdb_entries(
                self.client, entry_type=SAI_FDB_FLUSH_ENTRY_TYPE_ALL)

    def l2VxLanFloodLAGTest(self):
        '''
        Verify flooding with correct tagging on tunnel vlan members and
        port vlan members with one tunnel pointing to a LAG.
        '''
        print("\nl2VxLanFloodLAGTest()")

        lag_unbor_mac = "00:aa:aa:aa:aa:11"

        lag_unbor = sai_thrift_neighbor_entry_t(
            ip_address=sai_ipaddress(self.tun3_ip),
            rif_id=self.lag3_rif,
            switch_id=self.switch_id)
        sai_thrift_create_neighbor_entry(self.client,
                                         lag_unbor,
                                         dst_mac_address=lag_unbor_mac,
                                         no_host_route=True)
        lag_unhop = sai_thrift_create_next_hop(
            self.client,
            type=SAI_NEXT_HOP_TYPE_IP,
            ip=sai_ipaddress(self.tun3_ip),
            router_interface_id=self.lag3_rif)

        # change route for tunnel3 to use LAG
        sai_thrift_set_route_entry_attribute(
            self.client, self.unbor3_route, next_hop_id=lag_unhop)

        uport3_dev_list = [self.dev_port14, self.dev_port15, self.dev_port16]
        itrs = 30
        customer1_ip_list = []
        for i in range(0, itrs):
            customer1_ip_list.append("192.168.101.%d" % (i + 1))

        try:
            count = [0, 0, 0]
            for i in range(0, itrs):
                # L2 encap + flooding into vlan
                pkt = simple_udp_packet(eth_dst=self.customer2_mac,
                                        eth_src=self.customer1_mac,
                                        ip_dst=self.customer2_ip,
                                        ip_src=customer1_ip_list[i],
                                        ip_id=108,
                                        ip_ttl=64,
                                        pktlen=100)
                # expected packets
                tag_pkt = simple_udp_packet(eth_dst=self.customer2_mac,
                                            eth_src=self.customer1_mac,
                                            ip_dst=self.customer2_ip,
                                            ip_src=customer1_ip_list[i],
                                            dl_vlan_enable=True,
                                            vlan_vid=10,
                                            ip_id=108,
                                            ip_ttl=64,
                                            pktlen=104)
                vxlan_pkt = Mask(
                    simple_vxlan_packet(eth_dst=self.unbor1_mac,
                                        eth_src=ROUTER_MAC,
                                        ip_dst=self.tun1_ip,
                                        ip_src=self.lpb_ip,
                                        ip_id=0,
                                        ip_ttl=64,
                                        ip_flags=0x2,
                                        with_udp_chksum=False,
                                        vxlan_vni=self.vni1,
                                        inner_frame=pkt))
                vxlan_pkt.set_do_not_care_scapy(UDP, 'sport')
                vxlan_pkt2 = Mask(
                    simple_vxlan_packet(eth_dst=self.unbor2_mac,
                                        eth_src=ROUTER_MAC,
                                        ip_dst=self.tun2_ip,
                                        ip_src=self.lpb_ip,
                                        ip_id=0,
                                        ip_ttl=64,
                                        ip_flags=0x2,
                                        with_udp_chksum=False,
                                        vxlan_vni=self.vni1,
                                        inner_frame=pkt))
                vxlan_pkt2.set_do_not_care_scapy(UDP, 'sport')
                vxlan_pkt3 = Mask(
                    simple_vxlan_packet(eth_dst=lag_unbor_mac,
                                        eth_src=ROUTER_MAC,
                                        ip_dst=self.tun3_ip,
                                        ip_src=self.lpb_ip,
                                        ip_id=0,
                                        ip_ttl=64,
                                        ip_flags=0x2,
                                        with_udp_chksum=False,
                                        vxlan_vni=self.vni1,
                                        inner_frame=pkt))
                vxlan_pkt3.set_do_not_care_scapy(UDP, 'sport')

                print("Sending packet from port %d" % self.oport1_dev)
                send_packet(self, self.oport1_dev, pkt)
                ret = verify_each_packet_on_multiple_port_lists(
                    self, [tag_pkt, pkt, vxlan_pkt, vxlan_pkt2, vxlan_pkt3],
                    [[self.dev_port1],
                     [self.dev_port4, self.dev_port5, self.dev_port6],
                     [self.uport1_dev], [self.uport2_dev], uport3_dev_list])
                count[ret[4].pop()] += 1
                print('Packet was flooded into L2 VXLAN tunnels '
                      'and physical ports')

            for i in range(0, 3):
                self.assertTrue(count[i] >= (itrs / 3) * 0.5,
                                "LAG ports not equally balanced "
                                + str(count))
        finally:
            # revert original route for tunnel3
            sai_thrift_set_route_entry_attribute(
                self.client, self.unbor3_route, next_hop_id=self.unhop3)

            sai_thrift_remove_next_hop(self.client, lag_unhop)
            sai_thrift_remove_neighbor_entry(self.client, lag_unbor)

    def l2VxLanFloodECMPTest(self):
        '''
        Verify flooding with correct tagging on tunnel vlan members and
        port vlan members when remote vtep is reachable through ECMP.
        '''
        print("\nl2VxLanFloodECMPTest()")

        port24_ip = "10.0.0.24"
        port25_ip = "10.0.0.25"
        unbor_p24_mac = "00:aa:aa:aa:aa:24"
        unbor_p25_mac = "00:aa:aa:aa:aa:25"

        port24_rif = sai_thrift_create_router_interface(
            self.client,
            type=SAI_ROUTER_INTERFACE_TYPE_PORT,
            virtual_router_id=self.default_vrf,
            port_id=self.port24)
        port25_rif = sai_thrift_create_router_interface(
            self.client,
            type=SAI_ROUTER_INTERFACE_TYPE_PORT,
            virtual_router_id=self.default_vrf,
            port_id=self.port25)

        unbor_p24 = sai_thrift_neighbor_entry_t(
            ip_address=sai_ipaddress(port24_ip),
            rif_id=port24_rif,
            switch_id=self.switch_id)
        sai_thrift_create_neighbor_entry(self.client,
                                         unbor_p24,
                                         dst_mac_address=unbor_p24_mac)
        unhop_p24 = sai_thrift_create_next_hop(
            self.client,
            type=SAI_NEXT_HOP_TYPE_IP,
            ip=sai_ipaddress(port24_ip),
            router_interface_id=port24_rif)

        unbor_p25 = sai_thrift_neighbor_entry_t(
            ip_address=sai_ipaddress(port25_ip),
            rif_id=port25_rif,
            switch_id=self.switch_id)
        sai_thrift_create_neighbor_entry(self.client,
                                         unbor_p25,
                                         dst_mac_address=unbor_p25_mac)
        unhop_p25 = sai_thrift_create_next_hop(
            self.client,
            type=SAI_NEXT_HOP_TYPE_IP,
            ip=sai_ipaddress(port25_ip),
            router_interface_id=port25_rif)

        # nexthop group
        nhop_group = sai_thrift_create_next_hop_group(
            self.client, type=SAI_NEXT_HOP_GROUP_TYPE_ECMP)

        # nexthop group members
        nhop_group_member1 = sai_thrift_create_next_hop_group_member(
            self.client,
            next_hop_group_id=nhop_group,
            next_hop_id=unhop_p24)
        ecmp_ports = [self.dev_port24]

        # change route for tunnel3 to use ECMP
        sai_thrift_set_route_entry_attribute(
            self.client, self.unbor3_route, next_hop_id=nhop_group)

        try:
            # L2 encap + flooding into vlan
            pkt = simple_udp_packet(eth_dst=self.customer2_mac,
                                    eth_src=self.customer1_mac,
                                    ip_dst=self.customer2_ip,
                                    ip_src=self.customer1_ip,
                                    ip_id=108,
                                    ip_ttl=64,
                                    pktlen=100)
            # expected packets
            tag_pkt = simple_udp_packet(eth_dst=self.customer2_mac,
                                        eth_src=self.customer1_mac,
                                        ip_dst=self.customer2_ip,
                                        ip_src=self.customer1_ip,
                                        dl_vlan_enable=True,
                                        vlan_vid=10,
                                        ip_id=108,
                                        ip_ttl=64,
                                        pktlen=104)
            vxlan_pkt = Mask(
                simple_vxlan_packet(eth_dst=self.unbor1_mac,
                                    eth_src=ROUTER_MAC,
                                    ip_dst=self.tun1_ip,
                                    ip_src=self.lpb_ip,
                                    ip_id=0,
                                    ip_ttl=64,
                                    ip_flags=0x2,
                                    with_udp_chksum=False,
                                    vxlan_vni=self.vni1,
                                    inner_frame=pkt))
            vxlan_pkt.set_do_not_care_scapy(UDP, 'sport')
            vxlan_pkt2 = Mask(
                simple_vxlan_packet(eth_dst=self.unbor2_mac,
                                    eth_src=ROUTER_MAC,
                                    ip_dst=self.tun2_ip,
                                    ip_src=self.lpb_ip,
                                    ip_id=0,
                                    ip_ttl=64,
                                    ip_flags=0x2,
                                    with_udp_chksum=False,
                                    vxlan_vni=self.vni1,
                                    inner_frame=pkt))
            vxlan_pkt2.set_do_not_care_scapy(UDP, 'sport')
            vxlan_pkt3 = Mask(
                simple_vxlan_packet(eth_dst=self.unbor3_mac,
                                    eth_src=ROUTER_MAC,
                                    ip_dst=self.tun3_ip,
                                    ip_src=self.lpb_ip,
                                    ip_id=0,
                                    ip_ttl=64,
                                    ip_flags=0x2,
                                    with_udp_chksum=False,
                                    vxlan_vni=self.vni1,
                                    inner_frame=pkt))
            vxlan_pkt3.set_do_not_care_scapy(UDP, 'sport')
            vxlan_pkt3.set_do_not_care_scapy(Ether, 'dst')

            print("Sending packet from port %d" % self.oport1_dev)
            send_packet(self, self.oport1_dev, pkt)
            verify_each_packet_on_multiple_port_lists(
                self, [tag_pkt, pkt, vxlan_pkt, vxlan_pkt2, vxlan_pkt3],
                [[self.dev_port1],
                 [self.dev_port4, self.dev_port5, self.dev_port6],
                 [self.uport1_dev], [self.uport2_dev], ecmp_ports])
            print('Packet was flooded into L2 VXLAN tunnels '
                  'and physical ports')

            print("Adding ECMP member")
            nhop_group_member2 = sai_thrift_create_next_hop_group_member(
                self.client,
                next_hop_group_id=nhop_group,
                next_hop_id=unhop_p25)
            ecmp_ports.append(self.dev_port25)
            print("Sending packet from port %d" % self.oport1_dev)
            send_packet(self, self.oport1_dev, pkt)
            verify_each_packet_on_multiple_port_lists(
                self, [tag_pkt, pkt, vxlan_pkt, vxlan_pkt2, vxlan_pkt3],
                [[self.dev_port1],
                 [self.dev_port4, self.dev_port5, self.dev_port6],
                 [self.uport1_dev], [self.uport2_dev], ecmp_ports])
            print('Packet was flooded into L2 VXLAN tunnels '
                  'and physical ports')

            print("Removing ECMP member")
            sai_thrift_remove_next_hop_group_member(
                self.client, nhop_group_member1)
            ecmp_ports.remove(self.dev_port24)
            print("Sending packet from port %d" % self.oport1_dev)
            send_packet(self, self.oport1_dev, pkt)
            verify_each_packet_on_multiple_port_lists(
                self, [tag_pkt, pkt, vxlan_pkt, vxlan_pkt2, vxlan_pkt3],
                [[self.dev_port1],
                 [self.dev_port4, self.dev_port5, self.dev_port6],
                 [self.uport1_dev], [self.uport2_dev], ecmp_ports])
            print('Packet was flooded into L2 VXLAN tunnels '
                  'and physical ports')

        finally:
            # revert original route for tunnel3
            sai_thrift_set_route_entry_attribute(
                self.client, self.unbor3_route, next_hop_id=self.unhop3)

            sai_thrift_remove_next_hop_group_member(
                self.client, nhop_group_member2)
            sai_thrift_remove_next_hop_group(self.client, nhop_group)
            sai_thrift_remove_next_hop(self.client, unhop_p25)
            sai_thrift_remove_neighbor_entry(self.client, unbor_p25)
            sai_thrift_remove_next_hop(self.client, unhop_p24)
            sai_thrift_remove_neighbor_entry(self.client, unbor_p24)
            sai_thrift_remove_router_interface(self.client, port25_rif)
            sai_thrift_remove_router_interface(self.client, port24_rif)

    def l2VxLanFloodECMPAdditionTest(self):
        '''
        Verify flooding with correct tagging on tunnel vlan members and
        port vlan members after multiple nexthops are added to ECMP group.
        Verify also if designated member is changing with ECMP group changes.
        '''
        print("\nl2VxLanFloodECMPAdditionTest()")

        port24_ip = "10.0.0.24"
        port25_ip = "10.0.0.25"
        port26_ip = "10.0.0.26"
        port27_ip = "10.0.0.27"
        unbor_p24_mac = "00:aa:aa:aa:aa:24"
        unbor_p25_mac = "00:aa:aa:aa:aa:25"
        unbor_p26_mac = "00:aa:aa:aa:aa:26"
        unbor_p27_mac = "00:aa:aa:aa:aa:27"

        port24_rif = sai_thrift_create_router_interface(
            self.client,
            type=SAI_ROUTER_INTERFACE_TYPE_PORT,
            virtual_router_id=self.default_vrf,
            port_id=self.port24)
        port25_rif = sai_thrift_create_router_interface(
            self.client,
            type=SAI_ROUTER_INTERFACE_TYPE_PORT,
            virtual_router_id=self.default_vrf,
            port_id=self.port25)
        port26_rif = sai_thrift_create_router_interface(
            self.client,
            type=SAI_ROUTER_INTERFACE_TYPE_PORT,
            virtual_router_id=self.default_vrf,
            port_id=self.port26)
        port27_rif = sai_thrift_create_router_interface(
            self.client,
            type=SAI_ROUTER_INTERFACE_TYPE_PORT,
            virtual_router_id=self.default_vrf,
            port_id=self.port27)

        unbor_p24 = sai_thrift_neighbor_entry_t(
            ip_address=sai_ipaddress(port24_ip),
            rif_id=port24_rif,
            switch_id=self.switch_id)
        sai_thrift_create_neighbor_entry(self.client,
                                         unbor_p24,
                                         dst_mac_address=unbor_p24_mac)
        unhop_p24 = sai_thrift_create_next_hop(
            self.client,
            type=SAI_NEXT_HOP_TYPE_IP,
            ip=sai_ipaddress(port24_ip),
            router_interface_id=port24_rif)

        unbor_p25 = sai_thrift_neighbor_entry_t(
            ip_address=sai_ipaddress(port25_ip),
            rif_id=port25_rif,
            switch_id=self.switch_id)
        sai_thrift_create_neighbor_entry(self.client,
                                         unbor_p25,
                                         dst_mac_address=unbor_p25_mac)
        unhop_p25 = sai_thrift_create_next_hop(
            self.client,
            type=SAI_NEXT_HOP_TYPE_IP,
            ip=sai_ipaddress(port25_ip),
            router_interface_id=port25_rif)

        unbor_p26 = sai_thrift_neighbor_entry_t(
            ip_address=sai_ipaddress(port26_ip),
            rif_id=port26_rif,
            switch_id=self.switch_id)
        sai_thrift_create_neighbor_entry(self.client,
                                         unbor_p26,
                                         dst_mac_address=unbor_p26_mac)
        unhop_p26 = sai_thrift_create_next_hop(
            self.client,
            type=SAI_NEXT_HOP_TYPE_IP,
            ip=sai_ipaddress(port26_ip),
            router_interface_id=port26_rif)

        unbor_p27 = sai_thrift_neighbor_entry_t(
            ip_address=sai_ipaddress(port27_ip),
            rif_id=port27_rif,
            switch_id=self.switch_id)
        sai_thrift_create_neighbor_entry(self.client,
                                         unbor_p27,
                                         dst_mac_address=unbor_p27_mac)
        unhop_p27 = sai_thrift_create_next_hop(
            self.client,
            type=SAI_NEXT_HOP_TYPE_IP,
            ip=sai_ipaddress(port27_ip),
            router_interface_id=port27_rif)

        # nexthop group
        nhop_group = sai_thrift_create_next_hop_group(
            self.client, type=SAI_NEXT_HOP_GROUP_TYPE_ECMP)

        # nexthop group members
        nhop_group_member1 = sai_thrift_create_next_hop_group_member(
            self.client,
            next_hop_group_id=nhop_group,
            next_hop_id=unhop_p24)
        ecmp_ports = [self.dev_port24]

        # change route for tunnel3 to use ECMP
        sai_thrift_set_route_entry_attribute(
            self.client, self.unbor3_route, next_hop_id=nhop_group)

        try:
            # L2 encap + flooding into vlan
            pkt = simple_udp_packet(eth_dst=self.customer2_mac,
                                    eth_src=self.customer1_mac,
                                    ip_dst=self.customer2_ip,
                                    ip_src=self.customer1_ip,
                                    ip_id=108,
                                    ip_ttl=64,
                                    pktlen=100)
            # expected packets
            tag_pkt = simple_udp_packet(eth_dst=self.customer2_mac,
                                        eth_src=self.customer1_mac,
                                        ip_dst=self.customer2_ip,
                                        ip_src=self.customer1_ip,
                                        dl_vlan_enable=True,
                                        vlan_vid=10,
                                        ip_id=108,
                                        ip_ttl=64,
                                        pktlen=104)
            vxlan_pkt = Mask(
                simple_vxlan_packet(eth_dst=self.unbor1_mac,
                                    eth_src=ROUTER_MAC,
                                    ip_dst=self.tun1_ip,
                                    ip_src=self.lpb_ip,
                                    ip_id=0,
                                    ip_ttl=64,
                                    ip_flags=0x2,
                                    with_udp_chksum=False,
                                    vxlan_vni=self.vni1,
                                    inner_frame=pkt))
            vxlan_pkt.set_do_not_care_scapy(UDP, 'sport')
            vxlan_pkt2 = Mask(
                simple_vxlan_packet(eth_dst=self.unbor2_mac,
                                    eth_src=ROUTER_MAC,
                                    ip_dst=self.tun2_ip,
                                    ip_src=self.lpb_ip,
                                    ip_id=0,
                                    ip_ttl=64,
                                    ip_flags=0x2,
                                    with_udp_chksum=False,
                                    vxlan_vni=self.vni1,
                                    inner_frame=pkt))
            vxlan_pkt2.set_do_not_care_scapy(UDP, 'sport')
            # pkt to be received on one of ECMP ports - ingore dst MAC
            vxlan_pkt3 = Mask(
                simple_vxlan_packet(eth_dst=unbor_p24_mac,
                                    eth_src=ROUTER_MAC,
                                    ip_dst=self.tun3_ip,
                                    ip_src=self.lpb_ip,
                                    ip_id=0,
                                    ip_ttl=64,
                                    ip_flags=0x2,
                                    with_udp_chksum=False,
                                    vxlan_vni=self.vni1,
                                    inner_frame=pkt))
            vxlan_pkt3.set_do_not_care_scapy(UDP, 'sport')
            vxlan_pkt3.set_do_not_care_scapy(Ether, 'dst')

            print("Sending packet from port %d" % self.oport1_dev)
            send_packet(self, self.oport1_dev, pkt)
            rcv_ports = verify_each_packet_on_multiple_port_lists(
                self, [tag_pkt, pkt, vxlan_pkt, vxlan_pkt2, vxlan_pkt3],
                [[self.dev_port1],
                 [self.dev_port4, self.dev_port5, self.dev_port6],
                 [self.uport1_dev], [self.uport2_dev], ecmp_ports])
            # retrieve ECMP member index
            rcv_ecmp_port1 = rcv_ports[4].pop()
            print('Packet was flooded into L2 VXLAN tunnels '
                  'and physical ports')

            print("Adding 2nd ECMP member")
            nhop_group_member2 = sai_thrift_create_next_hop_group_member(
                self.client,
                next_hop_group_id=nhop_group,
                next_hop_id=unhop_p25)
            ecmp_ports.append(self.dev_port25)
            print("Sending packet from port %d" % self.oport1_dev)
            send_packet(self, self.oport1_dev, pkt)
            rcv_ports = verify_each_packet_on_multiple_port_lists(
                self, [tag_pkt, pkt, vxlan_pkt, vxlan_pkt2, vxlan_pkt3],
                [[self.dev_port1],
                 [self.dev_port4, self.dev_port5, self.dev_port6],
                 [self.uport1_dev], [self.uport2_dev], ecmp_ports])
            # retrieve ECMP member index
            rcv_ecmp_port2 = rcv_ports[4].pop()
            print('Packet was flooded into L2 VXLAN tunnels '
                  'and physical ports')

            print("Adding 3rd ECMP member")
            nhop_group_member3 = sai_thrift_create_next_hop_group_member(
                self.client,
                next_hop_group_id=nhop_group,
                next_hop_id=unhop_p26)
            ecmp_ports.append(self.dev_port26)

            print("Sending packet from port %d" % self.oport1_dev)
            send_packet(self, self.oport1_dev, pkt)
            rcv_ports = verify_each_packet_on_multiple_port_lists(
                self, [tag_pkt, pkt, vxlan_pkt, vxlan_pkt2, vxlan_pkt3],
                [[self.dev_port1],
                 [self.dev_port4, self.dev_port5, self.dev_port6],
                 [self.uport1_dev], [self.uport2_dev], ecmp_ports])
            # retrieve ECMP member index
            rcv_ecmp_port3 = rcv_ports[4].pop()
            print('Packet was flooded into L2 VXLAN tunnels '
                  'and physical ports')

            print("Adding 4th ECMP member")
            nhop_group_member4 = sai_thrift_create_next_hop_group_member(
                self.client,
                next_hop_group_id=nhop_group,
                next_hop_id=unhop_p27)
            ecmp_ports.append(self.dev_port27)

            print("Sending packet from port %d" % self.oport1_dev)
            send_packet(self, self.oport1_dev, pkt)
            rcv_ports = verify_each_packet_on_multiple_port_lists(
                self, [tag_pkt, pkt, vxlan_pkt, vxlan_pkt2, vxlan_pkt3],
                [[self.dev_port1],
                 [self.dev_port4, self.dev_port5, self.dev_port6],
                 [self.uport1_dev], [self.uport2_dev], ecmp_ports])
            # retrieve ECMP member index
            rcv_ecmp_port4 = rcv_ports[4].pop()
            print('Packet was flooded into L2 VXLAN tunnels '
                  'and physical ports')

            self.assertTrue((rcv_ecmp_port1 != rcv_ecmp_port2) or
                            (rcv_ecmp_port1 != rcv_ecmp_port3) or
                            (rcv_ecmp_port1 != rcv_ecmp_port4))

        finally:
            # revert original route for tunnel3
            sai_thrift_set_route_entry_attribute(
                self.client, self.unbor3_route, next_hop_id=self.unhop3)

            sai_thrift_remove_next_hop_group_member(
                self.client, nhop_group_member4)
            sai_thrift_remove_next_hop_group_member(
                self.client, nhop_group_member3)
            sai_thrift_remove_next_hop_group_member(
                self.client, nhop_group_member2)
            sai_thrift_remove_next_hop_group_member(
                self.client, nhop_group_member1)
            sai_thrift_remove_next_hop_group(self.client, nhop_group)
            sai_thrift_remove_next_hop(self.client, unhop_p27)
            sai_thrift_remove_neighbor_entry(self.client, unbor_p27)
            sai_thrift_remove_next_hop(self.client, unhop_p26)
            sai_thrift_remove_neighbor_entry(self.client, unbor_p26)
            sai_thrift_remove_next_hop(self.client, unhop_p25)
            sai_thrift_remove_neighbor_entry(self.client, unbor_p25)
            sai_thrift_remove_next_hop(self.client, unhop_p24)
            sai_thrift_remove_neighbor_entry(self.client, unbor_p24)
            sai_thrift_remove_router_interface(self.client, port27_rif)
            sai_thrift_remove_router_interface(self.client, port26_rif)
            sai_thrift_remove_router_interface(self.client, port25_rif)
            sai_thrift_remove_router_interface(self.client, port24_rif)


@group('l2-vxlan')
class L2VxLanObjectCreationSequenceBase(SaiHelper):
    '''
    This class contains base configuration for tests for different sequences
    of VxLAN objects creation

    Config for regular L3 underlay interface:
                   overlay | underlay
                +---------------------+
                |              port10 |-- unbor1
    customer1 --| port0               |           [customer2]
                |              port11 |-- unbor2
                +---------------------+

    Config for L3 LAG underlay interface:
                   overlay | underlay
                +---------------------+
                |              port14 |
                |              port15 |--LAG3-- unbor1
                |              port16 |
    customer1 --| port0               |                 [customer2]
                |              port17 |
                |              port18 |--LAG4-- unbor2
                |              port19 |
                +---------------------+

    Config for L3 subport underlay interface:
                  overlay | underlay
                +----------------------------+
                |                 subport1   |-- unbor1          :
                |                 : .........|..........VLAN 100.:
    customer1 --| port0    port24            |                      [customer2]
                |                 : .........|..........VLAN 200.:
                |                 subport2   |-- unbor2          :
                +----------------------------+

    Config for SVI underlay interface:
                  overlay | underlay
                +----------------------------+
                |        :            port20 |-- unbor1
                |        :            port21 |-- unbor2 / unbor1 after MAC move
                |        :....VLAN 30 RIF....|
    customer1 --| port0                      |                     [customer2]
                |        :....VLAN 20 RIF....|                     [customer3]
                |        :                   |
                |        :             port2 |-- unbor1 after route fallback
                +----------------------------+

    Args:
        ipv6 (bool): ipv6 underlay configuraton indicator
    '''

    def __init__(self, ipv6=False):
        super(L2VxLanObjectCreationSequenceBase, self).__init__()
        self.ipv6 = ipv6

        if ipv6 is True:
            self.tun_ip = "2001:0db8::10:1"
            self.lpb_ip = "2001:0db8::20:1"
            self.uport_ip = "2001:0db8::10:10"
            self.tun_ip_mask = "/128"
        else:
            self.tun_ip = "10.0.0.65"
            self.lpb_ip = "10.1.0.32"
            self.uport_ip = "10.0.0.64"
            self.tun_ip_mask = "/32"

    def setUp(self):
        super(L2VxLanObjectCreationSequenceBase, self).setUp()

        self.customer1_ip = "192.168.100.1"
        self.customer1_ipv6 = "2001:0db8::aaaa:1"
        self.customer2_ip = "192.168.100.2"
        self.customer2_ipv6 = "2001:0db8::bbbb:1"
        self.customer3_ip = "192.168.100.3"
        self.customer3_ipv6 = "2001:0db8::cccc:1"
        self.customer1_mac = "00:22:22:22:22:11"
        self.customer2_mac = "00:22:22:22:22:22"
        self.customer3_mac = "00:22:22:22:22:33"
        self.unbor1_mac = "00:33:33:33:33:11"
        self.unbor2_mac = "00:33:33:33:33:22"
        self.vni = 1000

        # ports configuration according to assumed schemas
        self.oport = self.port0
        self.oport_dev = self.dev_port0
        self.oport_bp = self.port0_bp
        self.uport1 = self.port10
        self.uport1_dev = self.dev_port10
        self.uport1_rif = self.port10_rif
        self.uport2 = self.port11
        self.uport2_dev = self.dev_port11
        self.uport2_rif = self.port11_rif
        self.lag_uport1 = self.lag3
        self.lag_uport1_rif = self.lag3_rif
        self.lag_uport1_dev = [self.dev_port14, self.dev_port15,
                               self.dev_port16]
        self.lag_uport2 = self.lag4
        self.lag_uport2_rif = self.lag4_rif
        self.lag_uport2_dev = [self.dev_port17, self.dev_port18,
                               self.dev_port19]
        self.svi_uport = self.vlan30  # ports 20, 21(tag), lag5=[ports 22, 23]
        self.svi_uport_rif = self.vlan30_rif
        self.svi_uport1_dev = self.dev_port20
        self.svi_uport2_dev = self.dev_port21

        # underlay loopback rif for tunnels
        self.urif_lpb = sai_thrift_create_router_interface(
            self.client,
            type=SAI_ROUTER_INTERFACE_TYPE_LOOPBACK,
            virtual_router_id=self.default_vrf,
            mtu=9100)

        # tunnel configuration
        self.tunnel_type = SAI_TUNNEL_TYPE_VXLAN

        self.encap_tunnel_map_vlan = sai_thrift_create_tunnel_map(
            self.client, type=SAI_TUNNEL_MAP_TYPE_VLAN_ID_TO_VNI)

        self.decap_tunnel_map_vlan = sai_thrift_create_tunnel_map(
            self.client, type=SAI_TUNNEL_MAP_TYPE_VNI_TO_VLAN_ID)

        self.encap_maps = sai_thrift_object_list_t(
            count=1,
            idlist=[self.encap_tunnel_map_vlan])
        self.decap_maps = sai_thrift_object_list_t(
            count=1,
            idlist=[self.decap_tunnel_map_vlan])

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

        self.encap_tun_map_entry_vlan10 = sai_thrift_create_tunnel_map_entry(
            self.client,
            tunnel_map_type=SAI_TUNNEL_MAP_TYPE_VLAN_ID_TO_VNI,
            tunnel_map=self.encap_tunnel_map_vlan,
            vlan_id_key=10,
            vni_id_value=self.vni)

        self.decap_tun_map_entry_vlan10 = sai_thrift_create_tunnel_map_entry(
            self.client,
            tunnel_map_type=SAI_TUNNEL_MAP_TYPE_VNI_TO_VLAN_ID,
            tunnel_map=self.decap_tunnel_map_vlan,
            vni_id_key=self.vni,
            vlan_id_value=10)

    def tearDown(self):
        sai_thrift_remove_tunnel_map_entry(
            self.client, self.decap_tun_map_entry_vlan10)
        sai_thrift_remove_tunnel_map_entry(
            self.client, self.encap_tun_map_entry_vlan10)
        sai_thrift_remove_tunnel_term_table_entry(
            self.client, self.tunnel_term)
        sai_thrift_remove_tunnel(self.client, self.p2mp_tunnel)
        sai_thrift_remove_tunnel_map(self.client, self.decap_tunnel_map_vlan)
        sai_thrift_remove_tunnel_map(self.client, self.encap_tunnel_map_vlan)
        sai_thrift_remove_router_interface(self.client, self.urif_lpb)

        super(L2VxLanObjectCreationSequenceBase, self).tearDown()

    # it is intentional to define following objects inside the function below
    # noqa pylint: disable=attribute-defined-outside-init
    def _configureL2Tunnel(self):
        '''
        Helper function to configure VxLAN P2P tunnel-related objects
        '''

        self.p2p_tunnel = sai_thrift_create_tunnel(
            self.client,
            type=self.tunnel_type,
            underlay_interface=self.urif_lpb,
            decap_mappers=self.decap_maps,
            encap_mappers=self.encap_maps,
            encap_src_ip=sai_ipaddress(self.lpb_ip),
            peer_mode=SAI_TUNNEL_PEER_MODE_P2P,
            encap_dst_ip=sai_ipaddress(self.tun_ip))

        self.p2p_tunnel_bp = sai_thrift_create_bridge_port(
            self.client,
            type=SAI_BRIDGE_PORT_TYPE_TUNNEL,
            tunnel_id=self.p2p_tunnel,
            bridge_id=self.default_1q_bridge,
            admin_state=True,
            fdb_learning_mode=SAI_BRIDGE_PORT_FDB_LEARNING_MODE_DISABLE)

        self.p2p_tun1_vlan10_member = sai_thrift_create_vlan_member(
            self.client,
            vlan_id=self.vlan10,
            bridge_port_id=self.p2p_tunnel_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)

        self.fdb_entry = sai_thrift_fdb_entry_t(
            switch_id=self.switch_id,
            mac_address=self.customer2_mac,
            bv_id=self.vlan10)
        sai_thrift_create_fdb_entry(
            self.client,
            self.fdb_entry,
            type=SAI_FDB_ENTRY_TYPE_STATIC,
            bridge_port_id=self.p2p_tunnel_bp)

    def _cleanupL2Tunnel(self, clean_fdb=True):
        '''
        Helper function to cleanup VxLAN P2P tunnel-related objects

        Args:
            clean_fdb (bool): True if FDB entry is to be removed
        '''

        if clean_fdb:
            sai_thrift_remove_fdb_entry(self.client, self.fdb_entry)

        sai_thrift_remove_vlan_member(self.client, self.p2p_tun1_vlan10_member)
        sai_thrift_remove_bridge_port(self.client, self.p2p_tunnel_bp)
        sai_thrift_remove_tunnel(self.client, self.p2p_tunnel)

    def _trafficTest(self, rcv_port, vlan_no=None, move=False, multi=False):
        '''
        Helper function for checking IPv4 and IPv6 packets forwarding

        Args:
            rcv_port (list): a list of underlay port device numbers
            vlan_no (int): vlan number of out port (None if not tagged)
            move (bool): MAC moving case indicator
            multi (bool): multiple FDB entries case indicator
        '''

        # determine neighbor MAC according to assumed conditions
        if rcv_port in [[self.uport1_dev], self.lag_uport1_dev,
                        [self.svi_uport1_dev]] \
                or vlan_no == 100 \
                or (rcv_port == [self.svi_uport2_dev] and move):
            uport_mac = self.unbor1_mac
        else:
            uport_mac = self.unbor2_mac

        if not multi:
            customer_mac = self.customer2_mac
            customer_ip = self.customer2_ip
            customer_ipv6 = self.customer2_ipv6
        else:
            customer_mac = self.customer3_mac
            customer_ip = self.customer3_ip
            customer_ipv6 = self.customer3_ipv6

        drop = not bool(rcv_port)

        v4_pkt = simple_udp_packet(eth_dst=customer_mac,
                                   eth_src=self.customer1_mac,
                                   ip_dst=customer_ip,
                                   ip_src=self.customer1_ip,
                                   ip_id=108,
                                   ip_ttl=64,
                                   pktlen=100)

        v6_pkt = simple_udpv6_packet(eth_dst=customer_mac,
                                     eth_src=self.customer1_mac,
                                     ipv6_dst=customer_ipv6,
                                     ipv6_src=self.customer1_ipv6,
                                     ipv6_hlim=64,
                                     pktlen=100)

        for ver, pkt in zip(["IPv4", "IPv6"], [v4_pkt, v6_pkt]):
            if not drop:
                if self.ipv6 is False:
                    vxlan_pkt = Mask(
                        simple_vxlan_packet(
                            eth_dst=uport_mac,
                            eth_src=ROUTER_MAC,
                            ip_dst=self.tun_ip,
                            ip_src=self.lpb_ip,
                            ip_id=0,
                            dl_vlan_enable=bool(vlan_no),
                            vlan_vid=vlan_no,
                            ip_ttl=64,
                            ip_flags=0x2,
                            with_udp_chksum=False,
                            vxlan_vni=self.vni,
                            inner_frame=pkt))
                    vxlan_pkt.set_do_not_care_scapy(UDP, 'sport')
                else:
                    vxlan_pkt = Mask(
                        simple_vxlanv6_packet(
                            eth_dst=uport_mac,
                            eth_src=ROUTER_MAC,
                            ipv6_dst=self.tun_ip,
                            ipv6_src=self.lpb_ip,
                            dl_vlan_enable=bool(vlan_no),
                            vlan_vid=vlan_no,
                            ipv6_hlim=64,
                            with_udp_chksum=False,
                            vxlan_vni=self.vni,
                            inner_frame=pkt))
                    vxlan_pkt.set_do_not_care_scapy(UDP, 'sport')

                print("Sending %s packet on port %d" % (ver, self.oport_dev))
                send_packet(self, self.oport_dev, pkt)
                verify_packet_any_port(self, vxlan_pkt, rcv_port)
                print("Packet was L2 forwarded")
            else:
                print("Sending %s packet on port %d" % (ver, self.oport_dev))
                send_packet(self, self.oport_dev, pkt)
                verify_no_other_packets(self)
                print("Packet was dropped")

    def _floodTest(self, uports, vlan_no=None, move=False, tunnel_route=True):
        '''
        Helper function for checking IPv4 and IPv6 packets flooding when tunnel
        FDB entry doesn't exist

        Args:
            uports (list): a list of underlay port device numbers
            vlan_no (int): vlan number of out port (None if not tagged)
            move (bool): MAC moving case indicator
            tunnel_route (bool): tunnel route existence indicator
                                 if route does not exists packet is not flooded
                                 out of underlay port
        '''

        # determine neighbor MAC according to assumed conditions
        if uports in [[self.uport1_dev], self.lag_uport1_dev,
                      [self.svi_uport1_dev]] \
                or vlan_no == 100 \
                or (uports == [self.svi_uport2_dev] and move):
            uport_mac = self.unbor1_mac
        else:
            uport_mac = self.unbor2_mac

        # VLAN 10 ports to be flooded + tunnel underlay
        if tunnel_route:
            rcv_ports = [[self.dev_port1],
                         [self.dev_port4, self.dev_port5, self.dev_port6],
                         uports]
        else:
            rcv_ports = [[self.dev_port1],
                         [self.dev_port4, self.dev_port5, self.dev_port6]]

        v4_pkt = simple_udp_packet(eth_dst=self.customer2_mac,
                                   eth_src=self.customer1_mac,
                                   ip_dst=self.customer2_ip,
                                   ip_src=self.customer1_ip,
                                   ip_id=108,
                                   ip_ttl=64,
                                   pktlen=100)

        tag_v4_pkt = simple_udp_packet(eth_dst=self.customer2_mac,
                                       eth_src=self.customer1_mac,
                                       ip_dst=self.customer2_ip,
                                       ip_src=self.customer1_ip,
                                       dl_vlan_enable=True,
                                       vlan_vid=10,
                                       ip_id=108,
                                       ip_ttl=64,
                                       pktlen=104)

        v6_pkt = simple_udpv6_packet(eth_dst=self.customer2_mac,
                                     eth_src=self.customer1_mac,
                                     ipv6_dst=self.customer2_ipv6,
                                     ipv6_src=self.customer1_ipv6,
                                     ipv6_hlim=64,
                                     pktlen=100)

        tag_v6_pkt = simple_udpv6_packet(eth_dst=self.customer2_mac,
                                         eth_src=self.customer1_mac,
                                         ipv6_dst=self.customer2_ipv6,
                                         ipv6_src=self.customer1_ipv6,
                                         dl_vlan_enable=True,
                                         vlan_vid=10,
                                         ipv6_hlim=64,
                                         pktlen=104)

        for ver, pkt, tag_pkt in zip(["IPv4", "IPv6"], [v4_pkt, v6_pkt],
                                     [tag_v4_pkt, tag_v6_pkt]):
            if tunnel_route:
                if self.ipv6 is False:
                    vxlan_pkt = Mask(
                        simple_vxlan_packet(
                            eth_dst=uport_mac,
                            eth_src=ROUTER_MAC,
                            ip_dst=self.tun_ip,
                            ip_src=self.lpb_ip,
                            ip_id=0,
                            dl_vlan_enable=bool(vlan_no),
                            vlan_vid=vlan_no,
                            ip_ttl=64,
                            ip_flags=0x2,
                            with_udp_chksum=False,
                            vxlan_vni=self.vni,
                            inner_frame=pkt))
                    vxlan_pkt.set_do_not_care_scapy(UDP, 'sport')
                else:
                    vxlan_pkt = Mask(
                        simple_vxlanv6_packet(
                            eth_dst=uport_mac,
                            eth_src=ROUTER_MAC,
                            ipv6_dst=self.tun_ip,
                            ipv6_src=self.lpb_ip,
                            dl_vlan_enable=bool(vlan_no),
                            vlan_vid=vlan_no,
                            ipv6_hlim=64,
                            with_udp_chksum=False,
                            vxlan_vni=self.vni,
                            inner_frame=pkt))
                    vxlan_pkt.set_do_not_care_scapy(UDP, 'sport')

                # packets corresponding to ports on port_list
                pkt_list = [tag_pkt, pkt, vxlan_pkt]
            else:
                pkt_list = [tag_pkt, pkt]

            print("Sending %s packet on port %d" % (ver, self.oport_dev))
            send_packet(self, self.oport_dev, pkt)
            verify_each_packet_on_multiple_port_lists(
                self, pkt_list, rcv_ports)
            print("Packet was flooded")

    def _sequenceTest1(self, urif, uport, vlan_no=None):
        '''
        This verifies L2 VxLAN functionalities with following sequence of
        objects creation:
        underlay neighbor -> underlay nexthop -> tunnel route -> P2P tunnel

        Args:
            urif (oid): underlay RIF object ID
            uport (int): underlay port number
            vlan_no (int): vlan number of out port (None if not tagged)
        '''
        print("sequence 1")

        try:
            unbor = sai_thrift_neighbor_entry_t(
                ip_address=sai_ipaddress(self.tun_ip),
                rif_id=urif,
                switch_id=self.switch_id)
            sai_thrift_create_neighbor_entry(self.client,
                                             unbor,
                                             dst_mac_address=self.unbor1_mac,
                                             no_host_route=True)

            unhop = sai_thrift_create_next_hop(
                self.client,
                type=SAI_NEXT_HOP_TYPE_IP,
                ip=sai_ipaddress(self.tun_ip),
                router_interface_id=urif)

            self._floodTest(uport, vlan_no, tunnel_route=False)

            tunnel_route = sai_thrift_route_entry_t(
                vr_id=self.default_vrf,
                destination=sai_ipprefix(self.tun_ip + self.tun_ip_mask))
            sai_thrift_create_route_entry(self.client,
                                          tunnel_route,
                                          next_hop_id=unhop)

            self._configureL2Tunnel()

            self._trafficTest(uport, vlan_no)
            sai_thrift_remove_fdb_entry(self.client, self.fdb_entry)
            self._floodTest(uport, vlan_no)

        finally:
            self._cleanupL2Tunnel(clean_fdb=False)
            sai_thrift_remove_route_entry(self.client, tunnel_route)
            sai_thrift_remove_next_hop(self.client, unhop)
            sai_thrift_remove_neighbor_entry(self.client, unbor)

    def _sequenceTest2(self, urif, uport, vlan_no=None):
        '''
        This verifies L2 VxLAN functionalities with following sequence of
        objects creation:
        underlay neighbor -> P2P tunnel -> underlay nexthop -> tunnel route

        Args:
            urif (oid): underlay RIF object ID
            uport (int): underlay port number
            vlan_no (int): vlan number of out port (None if not tagged)
        '''
        print("sequence 2")

        try:
            unbor = sai_thrift_neighbor_entry_t(
                ip_address=sai_ipaddress(self.tun_ip),
                rif_id=urif,
                switch_id=self.switch_id)
            sai_thrift_create_neighbor_entry(self.client,
                                             unbor,
                                             dst_mac_address=self.unbor1_mac,
                                             no_host_route=True)

            self._configureL2Tunnel()

            unhop = sai_thrift_create_next_hop(
                self.client,
                type=SAI_NEXT_HOP_TYPE_IP,
                ip=sai_ipaddress(self.tun_ip),
                router_interface_id=urif)

            tunnel_route = sai_thrift_route_entry_t(
                vr_id=self.default_vrf,
                destination=sai_ipprefix(self.tun_ip + self.tun_ip_mask))
            sai_thrift_create_route_entry(self.client,
                                          tunnel_route,
                                          next_hop_id=unhop)

            self._trafficTest(uport, vlan_no)

        finally:
            sai_thrift_remove_route_entry(self.client, tunnel_route)
            sai_thrift_remove_next_hop(self.client, unhop)
            self._cleanupL2Tunnel()
            sai_thrift_remove_neighbor_entry(self.client, unbor)

    def _sequenceTest3(self, urif, uport, vlan_no=None):
        '''
        This verifies L2 VxLAN functionalities with following sequence of
        objects creation:
        underlay neighbor -> underlay nexthop -> P2P tunnel -> tunnel route

        Args:
            urif (oid): underlay RIF object ID
            uport (int): underlay port number
            vlan_no (int): vlan number of out port (None if not tagged)
        '''
        print("sequence 3")

        try:
            unbor = sai_thrift_neighbor_entry_t(
                ip_address=sai_ipaddress(self.tun_ip),
                rif_id=urif,
                switch_id=self.switch_id)
            sai_thrift_create_neighbor_entry(self.client,
                                             unbor,
                                             dst_mac_address=self.unbor1_mac,
                                             no_host_route=True)

            unhop = sai_thrift_create_next_hop(
                self.client,
                type=SAI_NEXT_HOP_TYPE_IP,
                ip=sai_ipaddress(self.tun_ip),
                router_interface_id=urif)

            self._configureL2Tunnel()

            tunnel_route = sai_thrift_route_entry_t(
                vr_id=self.default_vrf,
                destination=sai_ipprefix(self.tun_ip + self.tun_ip_mask))
            sai_thrift_create_route_entry(self.client,
                                          tunnel_route,
                                          next_hop_id=unhop)
            self._trafficTest(uport, vlan_no)

        finally:
            sai_thrift_remove_route_entry(self.client, tunnel_route)
            self._cleanupL2Tunnel()
            sai_thrift_remove_next_hop(self.client, unhop)
            sai_thrift_remove_neighbor_entry(self.client, unbor)

    def _sequenceTest4(self, urif, uport, vlan_no=None):
        '''
        This verifies L2 VxLAN functionalities with following sequence of
        objects creation:
        underlay nexthop -> tunnel route -> underlay neighbor -> P2P tunnel

        Args:
            urif (oid): underlay RIF object ID
            uport (int): underlay port number
            vlan_no (int): vlan number of out port (None if not tagged)
        '''
        print("sequence 4")

        try:
            unhop = sai_thrift_create_next_hop(
                self.client,
                type=SAI_NEXT_HOP_TYPE_IP,
                ip=sai_ipaddress(self.tun_ip),
                router_interface_id=urif)

            tunnel_route = sai_thrift_route_entry_t(
                vr_id=self.default_vrf,
                destination=sai_ipprefix(self.tun_ip + self.tun_ip_mask))
            sai_thrift_create_route_entry(self.client,
                                          tunnel_route,
                                          next_hop_id=unhop)

            unbor = sai_thrift_neighbor_entry_t(
                ip_address=sai_ipaddress(self.tun_ip),
                rif_id=urif,
                switch_id=self.switch_id)
            sai_thrift_create_neighbor_entry(self.client,
                                             unbor,
                                             dst_mac_address=self.unbor1_mac,
                                             no_host_route=True)

            self._configureL2Tunnel()

            self._trafficTest(uport, vlan_no)

        finally:
            self._cleanupL2Tunnel()
            sai_thrift_remove_neighbor_entry(self.client, unbor)
            sai_thrift_remove_route_entry(self.client, tunnel_route)
            sai_thrift_remove_next_hop(self.client, unhop)

    def _sequenceTest5(self, urif, uport, vlan_no=None):
        '''
        This verifies L2 VxLAN functionalities with following sequence of
        objects creation:
        underlay nexthop -> tunnel route -> P2P tunnel -> underlay neighbor

        Args:
            urif (oid): underlay RIF object ID
            uport (int): underlay port number
            vlan_no (int): vlan number of out port (None if not tagged)
        '''
        print("sequence 5")

        try:
            unhop = sai_thrift_create_next_hop(
                self.client,
                type=SAI_NEXT_HOP_TYPE_IP,
                ip=sai_ipaddress(self.tun_ip),
                router_interface_id=urif)

            tunnel_route = sai_thrift_route_entry_t(
                vr_id=self.default_vrf,
                destination=sai_ipprefix(self.tun_ip + self.tun_ip_mask))
            sai_thrift_create_route_entry(self.client,
                                          tunnel_route,
                                          next_hop_id=unhop)

            self._configureL2Tunnel()

            unbor = sai_thrift_neighbor_entry_t(
                ip_address=sai_ipaddress(self.tun_ip),
                rif_id=urif,
                switch_id=self.switch_id)
            sai_thrift_create_neighbor_entry(self.client,
                                             unbor,
                                             dst_mac_address=self.unbor1_mac,
                                             no_host_route=True)

            self._trafficTest(uport, vlan_no)

        finally:
            sai_thrift_remove_neighbor_entry(self.client, unbor)
            self._cleanupL2Tunnel()
            sai_thrift_remove_route_entry(self.client, tunnel_route)
            sai_thrift_remove_next_hop(self.client, unhop)

    def _sequenceTest6(self, urif, uport, vlan_no=None):
        '''
        This verifies L2 VxLAN functionalities with following sequence of
        objects creation:
        underlay nexthop -> underlay neighbor -> tunnel route -> P2P tunnel

        Args:
            urif (oid): underlay RIF object ID
            uport (int): underlay port number
            vlan_no (int): vlan number of out port (None if not tagged)
        '''
        print("sequence 6")

        try:
            unhop = sai_thrift_create_next_hop(
                self.client,
                type=SAI_NEXT_HOP_TYPE_IP,
                ip=sai_ipaddress(self.tun_ip),
                router_interface_id=urif)

            unbor = sai_thrift_neighbor_entry_t(
                ip_address=sai_ipaddress(self.tun_ip),
                rif_id=urif,
                switch_id=self.switch_id)
            sai_thrift_create_neighbor_entry(self.client,
                                             unbor,
                                             dst_mac_address=self.unbor1_mac,
                                             no_host_route=True)

            tunnel_route = sai_thrift_route_entry_t(
                vr_id=self.default_vrf,
                destination=sai_ipprefix(self.tun_ip + self.tun_ip_mask))
            sai_thrift_create_route_entry(self.client,
                                          tunnel_route,
                                          next_hop_id=unhop)

            self._configureL2Tunnel()

            self._trafficTest(uport, vlan_no)

        finally:
            self._cleanupL2Tunnel()
            sai_thrift_remove_route_entry(self.client, tunnel_route)
            sai_thrift_remove_neighbor_entry(self.client, unbor)
            sai_thrift_remove_next_hop(self.client, unhop)

    def _sequenceTest7(self, urif, uport, vlan_no=None):
        '''
        This verifies L2 VxLAN functionalities with following sequence of
        objects creation:
        underlay nexthop -> underlay neighbor -> P2P tunnel -> tunnel route

        Args:
            urif (oid): underlay RIF object ID
            uport (int): underlay port number
            vlan_no (int): vlan number of out port (None if not tagged)
        '''
        print("sequence 7")

        try:
            unhop = sai_thrift_create_next_hop(
                self.client,
                type=SAI_NEXT_HOP_TYPE_IP,
                ip=sai_ipaddress(self.tun_ip),
                router_interface_id=urif)

            unbor = sai_thrift_neighbor_entry_t(
                ip_address=sai_ipaddress(self.tun_ip),
                rif_id=urif,
                switch_id=self.switch_id)
            sai_thrift_create_neighbor_entry(self.client,
                                             unbor,
                                             dst_mac_address=self.unbor1_mac,
                                             no_host_route=True)

            self._configureL2Tunnel()

            tunnel_route = sai_thrift_route_entry_t(
                vr_id=self.default_vrf,
                destination=sai_ipprefix(self.tun_ip + self.tun_ip_mask))
            sai_thrift_create_route_entry(self.client,
                                          tunnel_route,
                                          next_hop_id=unhop)

            self._trafficTest(uport, vlan_no)

        finally:
            sai_thrift_remove_route_entry(self.client, tunnel_route)
            self._cleanupL2Tunnel()
            sai_thrift_remove_neighbor_entry(self.client, unbor)
            sai_thrift_remove_next_hop(self.client, unhop)

    def _sequenceTest8(self, urif, uport, vlan_no=None):
        '''
        This verifies L2 VxLAN functionalities with following sequence of
        objects creation:
        underlay nexthop -> P2P tunnel -> tunnel route -> underlay neighbor

        Args:
            urif (oid): underlay RIF object ID
            uport (int): underlay port number
            vlan_no (int): vlan number of out port (None if not tagged)
        '''
        print("sequence 8")

        try:
            unhop = sai_thrift_create_next_hop(
                self.client,
                type=SAI_NEXT_HOP_TYPE_IP,
                ip=sai_ipaddress(self.tun_ip),
                router_interface_id=urif)

            self._configureL2Tunnel()

            tunnel_route = sai_thrift_route_entry_t(
                vr_id=self.default_vrf,
                destination=sai_ipprefix(self.tun_ip + self.tun_ip_mask))
            sai_thrift_create_route_entry(self.client,
                                          tunnel_route,
                                          next_hop_id=unhop)

            unbor = sai_thrift_neighbor_entry_t(
                ip_address=sai_ipaddress(self.tun_ip),
                rif_id=urif,
                switch_id=self.switch_id)
            sai_thrift_create_neighbor_entry(self.client,
                                             unbor,
                                             dst_mac_address=self.unbor1_mac,
                                             no_host_route=True)

            self._trafficTest(uport, vlan_no)

        finally:
            sai_thrift_remove_neighbor_entry(self.client, unbor)
            sai_thrift_remove_route_entry(self.client, tunnel_route)
            self._cleanupL2Tunnel()
            sai_thrift_remove_next_hop(self.client, unhop)

    def _sequenceTest9(self, urif, uport, vlan_no=None):
        '''
        This verifies L2 VxLAN functionalities with following sequence of
        objects creation:
        underlay nexthop -> P2P tunnel -> underlay neighbor -> tunnel route

        Args:
            urif (oid): underlay RIF object ID
            uport (int): underlay port number
            vlan_no (int): vlan number of out port (None if not tagged)
        '''
        print("sequence 9")

        try:
            unhop = sai_thrift_create_next_hop(
                self.client,
                type=SAI_NEXT_HOP_TYPE_IP,
                ip=sai_ipaddress(self.tun_ip),
                router_interface_id=urif)

            self._configureL2Tunnel()

            unbor = sai_thrift_neighbor_entry_t(
                ip_address=sai_ipaddress(self.tun_ip),
                rif_id=urif,
                switch_id=self.switch_id)
            sai_thrift_create_neighbor_entry(self.client,
                                             unbor,
                                             dst_mac_address=self.unbor1_mac,
                                             no_host_route=True)

            tunnel_route = sai_thrift_route_entry_t(
                vr_id=self.default_vrf,
                destination=sai_ipprefix(self.tun_ip + self.tun_ip_mask))
            sai_thrift_create_route_entry(self.client,
                                          tunnel_route,
                                          next_hop_id=unhop)

            self._trafficTest(uport, vlan_no)

        finally:
            sai_thrift_remove_route_entry(self.client, tunnel_route)
            sai_thrift_remove_neighbor_entry(self.client, unbor)
            self._cleanupL2Tunnel()
            sai_thrift_remove_next_hop(self.client, unhop)

    def _sequenceTest10(self, urif, uport, vlan_no=None):
        '''
        This verifies L2 VxLAN functionalities with following sequence of
        objects creation:
        P2P tunnel -> underlay neighbor -> underlay nexthop -> tunnel route

        Args:
            urif (oid): underlay RIF object ID
            uport (int): underlay port number
            vlan_no (int): vlan number of out port (None if not tagged)
        '''
        print("sequence 10")

        try:
            self._configureL2Tunnel()

            unbor = sai_thrift_neighbor_entry_t(
                ip_address=sai_ipaddress(self.tun_ip),
                rif_id=urif,
                switch_id=self.switch_id)
            sai_thrift_create_neighbor_entry(self.client,
                                             unbor,
                                             dst_mac_address=self.unbor1_mac,
                                             no_host_route=True)

            unhop = sai_thrift_create_next_hop(
                self.client,
                type=SAI_NEXT_HOP_TYPE_IP,
                ip=sai_ipaddress(self.tun_ip),
                router_interface_id=urif)

            tunnel_route = sai_thrift_route_entry_t(
                vr_id=self.default_vrf,
                destination=sai_ipprefix(self.tun_ip + self.tun_ip_mask))
            sai_thrift_create_route_entry(self.client,
                                          tunnel_route,
                                          next_hop_id=unhop)

            self._trafficTest(uport, vlan_no)
            sai_thrift_remove_fdb_entry(self.client, self.fdb_entry)

        finally:
            sai_thrift_remove_route_entry(self.client, tunnel_route)
            sai_thrift_remove_next_hop(self.client, unhop)
            sai_thrift_remove_neighbor_entry(self.client, unbor)
            self._cleanupL2Tunnel(clean_fdb=False)

    def _sequenceTest11(self, urif, uport, vlan_no=None):
        '''
        This verifies L2 VxLAN functionalities with following sequence of
        objects creation:
        P2P tunnel -> underlay nexthop -> tunnel route -> underlay neighbor

        Args:
            urif (oid): underlay RIF object ID
            uport (int): underlay port number
            vlan_no (int): vlan number of out port (None if not tagged)
        '''
        print("sequence 11")

        try:
            self._configureL2Tunnel()

            unhop = sai_thrift_create_next_hop(
                self.client,
                type=SAI_NEXT_HOP_TYPE_IP,
                ip=sai_ipaddress(self.tun_ip),
                router_interface_id=urif)

            tunnel_route = sai_thrift_route_entry_t(
                vr_id=self.default_vrf,
                destination=sai_ipprefix(self.tun_ip + self.tun_ip_mask))
            sai_thrift_create_route_entry(self.client,
                                          tunnel_route,
                                          next_hop_id=unhop)

            unbor = sai_thrift_neighbor_entry_t(
                ip_address=sai_ipaddress(self.tun_ip),
                rif_id=urif,
                switch_id=self.switch_id)
            sai_thrift_create_neighbor_entry(self.client,
                                             unbor,
                                             dst_mac_address=self.unbor1_mac,
                                             no_host_route=True)

            self._trafficTest(uport, vlan_no)

        finally:
            sai_thrift_remove_neighbor_entry(self.client, unbor)
            sai_thrift_remove_route_entry(self.client, tunnel_route)
            sai_thrift_remove_next_hop(self.client, unhop)
            self._cleanupL2Tunnel()

    def _sequenceTest12(self, urif, uport, vlan_no=None):
        '''
        This verifies L2 VxLAN functionalities with following sequence of
        objects creation:
        P2P tunnel -> underlay nexthop -> underlay neighbor -> tunnel route

        Args:
            urif (oid): underlay RIF object ID
            uport (int): underlay port number
            vlan_no (int): vlan number of out port (None if not tagged)
        '''
        print("sequence 12")

        try:
            self._configureL2Tunnel()

            unhop = sai_thrift_create_next_hop(
                self.client,
                type=SAI_NEXT_HOP_TYPE_IP,
                ip=sai_ipaddress(self.tun_ip),
                router_interface_id=urif)

            unbor = sai_thrift_neighbor_entry_t(
                ip_address=sai_ipaddress(self.tun_ip),
                rif_id=urif,
                switch_id=self.switch_id)
            sai_thrift_create_neighbor_entry(self.client,
                                             unbor,
                                             dst_mac_address=self.unbor1_mac,
                                             no_host_route=True)

            tunnel_route = sai_thrift_route_entry_t(
                vr_id=self.default_vrf,
                destination=sai_ipprefix(self.tun_ip + self.tun_ip_mask))
            sai_thrift_create_route_entry(self.client,
                                          tunnel_route,
                                          next_hop_id=unhop)

            self._trafficTest(uport, vlan_no)

        finally:
            sai_thrift_remove_route_entry(self.client, tunnel_route)
            sai_thrift_remove_neighbor_entry(self.client, unbor)
            sai_thrift_remove_next_hop(self.client, unhop)
            self._cleanupL2Tunnel()

    def _tunnelRouteUpdateTest(self, urif, uport, vlan_no=None):
        '''
        This verifies tunnel route update

        Args:
            urif (oid): underlay RIF object ID
            uport (int): underlay port number
            vlan_no (int): vlan number of out port (None if not tagged)
        '''
        print("tunnel route update")

        try:
            unbor = sai_thrift_neighbor_entry_t(
                ip_address=sai_ipaddress(self.tun_ip),
                rif_id=urif,
                switch_id=self.switch_id)
            sai_thrift_create_neighbor_entry(self.client,
                                             unbor,
                                             dst_mac_address=self.unbor1_mac,
                                             no_host_route=True)

            unhop = sai_thrift_create_next_hop(
                self.client,
                type=SAI_NEXT_HOP_TYPE_IP,
                ip=sai_ipaddress(self.tun_ip),
                router_interface_id=urif)

            print("\tRoute and FDB entry don't exist")
            self._floodTest(uport, vlan_no, tunnel_route=False)

            print("\tAdding route and FDB entry")
            tunnel_route = sai_thrift_route_entry_t(
                vr_id=self.default_vrf,
                destination=sai_ipprefix(self.tun_ip + self.tun_ip_mask))
            sai_thrift_create_route_entry(self.client,
                                          tunnel_route,
                                          next_hop_id=unhop)

            self._configureL2Tunnel()

            self._trafficTest(uport, vlan_no)
            print("\tRemoving FDB entry")
            sai_thrift_remove_fdb_entry(self.client, self.fdb_entry)
            self._floodTest(uport, vlan_no)
            print("\tAdding FDB entry")
            sai_thrift_create_fdb_entry(
                self.client,
                self.fdb_entry,
                type=SAI_FDB_ENTRY_TYPE_STATIC,
                bridge_port_id=self.p2p_tunnel_bp)

            print("\tRemoving route")
            sai_thrift_remove_route_entry(self.client, tunnel_route)
            self._trafficTest(None)
            print("\tRemoving FDB entry")
            sai_thrift_remove_fdb_entry(self.client, self.fdb_entry)
            self._floodTest(uport, vlan_no, tunnel_route=False)
            print("\tAdding FDB entry")
            sai_thrift_create_fdb_entry(
                self.client,
                self.fdb_entry,
                type=SAI_FDB_ENTRY_TYPE_STATIC,
                bridge_port_id=self.p2p_tunnel_bp)

            print("\tAdding new route")
            tunnel_route = sai_thrift_route_entry_t(
                vr_id=self.default_vrf,
                destination=sai_ipprefix(self.tun_ip + self.tun_ip_mask))
            sai_thrift_create_route_entry(self.client,
                                          tunnel_route,
                                          next_hop_id=unhop)
            self._trafficTest(uport, vlan_no)
            print("\tRemoving FDB entry")
            sai_thrift_remove_fdb_entry(self.client, self.fdb_entry)
            self._floodTest(uport, vlan_no)
            print("\tAdding FDB entry")
            sai_thrift_create_fdb_entry(
                self.client,
                self.fdb_entry,
                type=SAI_FDB_ENTRY_TYPE_STATIC,
                bridge_port_id=self.p2p_tunnel_bp)

            print("\tRemoving route and adding drop route")
            sai_thrift_remove_route_entry(self.client, tunnel_route)
            tunnel_route = sai_thrift_route_entry_t(
                vr_id=self.default_vrf,
                destination=sai_ipprefix(self.tun_ip + self.tun_ip_mask))
            sai_thrift_create_route_entry(self.client,
                                          tunnel_route,
                                          packet_action=SAI_PACKET_ACTION_DROP)
            self._trafficTest(None)

        finally:
            self._cleanupL2Tunnel()
            sai_thrift_remove_route_entry(self.client, tunnel_route)
            sai_thrift_remove_next_hop(self.client, unhop)
            sai_thrift_remove_neighbor_entry(self.client, unbor)

    # noqa pylint: disable=too-many-arguments
    def _tunnelRouteFallbackTest(self, urif1, urif2, uport1, uport2,
                                 vlan_no1=None, vlan_no2=None):
        '''
        This verifies tunnel route fallback when LPM and host route exist and
        then host route is removed

        Args:
            urif1 (oid): first underlay RIF object ID
            urif2 (oid): second underlay RIF object ID
            uport1 (int): first underlay port number
            uport2 (int): second underlay port number
            vlan_no1 (int): first vlan number of out port (None if not tagged)
            vlan_no2 (int): second vlan number of out port (None if not tagged)
        '''
        print("tunnel route fallback")

        if self.ipv6 is True:
            lpm_mask = "/112"
        else:
            lpm_mask = "/24"

        try:
            self._configureL2Tunnel()

            unbor1 = sai_thrift_neighbor_entry_t(
                ip_address=sai_ipaddress(self.tun_ip),
                rif_id=urif1,
                switch_id=self.switch_id)
            sai_thrift_create_neighbor_entry(self.client,
                                             unbor1,
                                             dst_mac_address=self.unbor1_mac,
                                             no_host_route=True)

            unhop1 = sai_thrift_create_next_hop(
                self.client,
                type=SAI_NEXT_HOP_TYPE_IP,
                ip=sai_ipaddress(self.tun_ip),
                router_interface_id=urif1)

            tunnel_route1 = sai_thrift_route_entry_t(
                vr_id=self.default_vrf,
                destination=sai_ipprefix(self.tun_ip + lpm_mask))
            sai_thrift_create_route_entry(self.client,
                                          tunnel_route1,
                                          next_hop_id=unhop1)

            print("\tUsing LPM route")
            self._trafficTest(uport1, vlan_no1)
            sai_thrift_remove_fdb_entry(self.client, self.fdb_entry)
            self._floodTest(uport1, vlan_no1)
            sai_thrift_create_fdb_entry(
                self.client,
                self.fdb_entry,
                type=SAI_FDB_ENTRY_TYPE_STATIC,
                bridge_port_id=self.p2p_tunnel_bp)

            unbor2 = sai_thrift_neighbor_entry_t(
                ip_address=sai_ipaddress(self.tun_ip),
                rif_id=urif2,
                switch_id=self.switch_id)
            sai_thrift_create_neighbor_entry(self.client,
                                             unbor2,
                                             dst_mac_address=self.unbor2_mac,
                                             no_host_route=True)

            unhop2 = sai_thrift_create_next_hop(
                self.client,
                type=SAI_NEXT_HOP_TYPE_IP,
                ip=sai_ipaddress(self.tun_ip),
                router_interface_id=urif2)

            print("\tAdding host route")
            tunnel_route2 = sai_thrift_route_entry_t(
                vr_id=self.default_vrf,
                destination=sai_ipprefix(self.tun_ip + self.tun_ip_mask))
            sai_thrift_create_route_entry(self.client,
                                          tunnel_route2,
                                          next_hop_id=unhop2)

            self._trafficTest(uport2, vlan_no2, move=True)
            sai_thrift_remove_fdb_entry(self.client, self.fdb_entry)
            self._floodTest(uport2, vlan_no2)
            sai_thrift_create_fdb_entry(
                self.client,
                self.fdb_entry,
                type=SAI_FDB_ENTRY_TYPE_STATIC,
                bridge_port_id=self.p2p_tunnel_bp)

            print("\tRemoving host route")
            sai_thrift_remove_route_entry(self.client, tunnel_route2)
            self._trafficTest(uport1, vlan_no1)
            sai_thrift_remove_fdb_entry(self.client, self.fdb_entry)
            self._floodTest(uport1, vlan_no1)

        finally:
            self._cleanupL2Tunnel(clean_fdb=False)
            sai_thrift_remove_next_hop(self.client, unhop2)
            sai_thrift_remove_neighbor_entry(self.client, unbor2)
            sai_thrift_remove_route_entry(self.client, tunnel_route1)
            sai_thrift_remove_next_hop(self.client, unhop1)
            sai_thrift_remove_neighbor_entry(self.client, unbor1)

    # noqa pylint: disable=too-many-arguments
    def _sviMacMoveTest(self, urif, uport1, uport2, vlan_no, mac_entry,
                        new_bp):
        '''
        This verifies VxLAN packets forwarding in case of underlay MAC moving

        Args:
            urif (oid): underlay RIF object ID
            uport1 (int): first underlay port number
            uport2 (int): second underlay port number
            vlan_no (int): vlan number of out port (None if not tagged)
            mac_entry (oid): oid of MAC entry to be moved
            new_bp (oid): new BP to be set for MAC entry
        '''
        print("SVI MAC move test")

        try:
            unbor = sai_thrift_neighbor_entry_t(
                ip_address=sai_ipaddress(self.tun_ip),
                rif_id=urif,
                switch_id=self.switch_id)
            sai_thrift_create_neighbor_entry(self.client,
                                             unbor,
                                             dst_mac_address=self.unbor1_mac,
                                             no_host_route=True)

            unhop = sai_thrift_create_next_hop(
                self.client,
                type=SAI_NEXT_HOP_TYPE_IP,
                ip=sai_ipaddress(self.tun_ip),
                router_interface_id=urif)

            tunnel_route = sai_thrift_route_entry_t(
                vr_id=self.default_vrf,
                destination=sai_ipprefix(self.tun_ip + self.tun_ip_mask))
            sai_thrift_create_route_entry(self.client,
                                          tunnel_route,
                                          next_hop_id=unhop)

            self._configureL2Tunnel()

            self._trafficTest(uport1)
            sai_thrift_remove_fdb_entry(self.client, self.fdb_entry)
            self._floodTest(uport1)
            sai_thrift_create_fdb_entry(
                self.client,
                self.fdb_entry,
                type=SAI_FDB_ENTRY_TYPE_STATIC,
                bridge_port_id=self.p2p_tunnel_bp)

            print("Moving MAC entry")
            mac_attr = sai_thrift_get_fdb_entry_attribute(
                self.client, mac_entry, bridge_port_id=True)
            initial_bp = mac_attr['bridge_port_id']
            sai_thrift_set_fdb_entry_attribute(self.client,
                                               mac_entry,
                                               bridge_port_id=new_bp)

            self._trafficTest(uport2, vlan_no=vlan_no, move=True)
            sai_thrift_remove_fdb_entry(self.client, self.fdb_entry)
            self._floodTest(uport2, vlan_no, move=True)

        finally:
            # revert initial MAC entry bridge port
            sai_thrift_set_fdb_entry_attribute(self.client,
                                               mac_entry,
                                               bridge_port_id=initial_bp)
            self._cleanupL2Tunnel(clean_fdb=False)
            sai_thrift_remove_route_entry(self.client, tunnel_route)
            sai_thrift_remove_next_hop(self.client, unhop)
            sai_thrift_remove_neighbor_entry(self.client, unbor)

    def l2VxlanL3InterfaceTest(self):
        '''
        This runs all tests for the case when underlay RIF is L3 interface
        '''
        print("\nl2VxlanL3InterfaceTest()")

        # ports configuration according to assumed schemas
        uport1_dev = self.uport1_dev
        uport1_rif = self.uport1_rif
        uport2_dev = self.uport2_dev
        uport2_rif = self.uport2_rif

        self._sequenceTest1(uport1_rif, [uport1_dev])
        self._sequenceTest2(uport1_rif, [uport1_dev])
        self._sequenceTest3(uport1_rif, [uport1_dev])
        self._sequenceTest4(uport1_rif, [uport1_dev])
        self._sequenceTest5(uport1_rif, [uport1_dev])
        self._sequenceTest6(uport1_rif, [uport1_dev])
        self._sequenceTest7(uport1_rif, [uport1_dev])
        self._sequenceTest8(uport1_rif, [uport1_dev])
        self._sequenceTest9(uport1_rif, [uport1_dev])
        self._sequenceTest10(uport1_rif, [uport1_dev])
        self._sequenceTest11(uport1_rif, [uport1_dev])
        self._sequenceTest12(uport1_rif, [uport1_dev])
        self._tunnelRouteUpdateTest(uport1_rif, [uport1_dev])
        self._tunnelRouteFallbackTest(uport1_rif, uport2_rif,
                                      [uport1_dev], [uport2_dev])

    def l2VxlanL3LagInterfaceTest(self):
        '''
        This runs all tests for the case when underlay RIF is L3 interface
        '''
        print("\nl2VxlanL3LagInterfaceTest()")

        # ports configuration according to assumed schemas
        uport1_rif = self.lag_uport1_rif
        uport1_dev = self.lag_uport1_dev
        uport2_rif = self.lag_uport2_rif
        uport2_dev = self.lag_uport2_dev

        self._sequenceTest1(uport1_rif, uport1_dev)
        self._sequenceTest2(uport1_rif, uport1_dev)
        self._sequenceTest3(uport1_rif, uport1_dev)
        self._sequenceTest4(uport1_rif, uport1_dev)
        self._sequenceTest5(uport1_rif, uport1_dev)
        self._sequenceTest6(uport1_rif, uport1_dev)
        self._sequenceTest7(uport1_rif, uport1_dev)
        self._sequenceTest8(uport1_rif, uport1_dev)
        self._sequenceTest9(uport1_rif, uport1_dev)
        self._sequenceTest10(uport1_rif, uport1_dev)
        self._sequenceTest11(uport1_rif, uport1_dev)
        self._sequenceTest12(uport1_rif, uport1_dev)
        self._tunnelRouteUpdateTest(uport1_rif, uport1_dev)
        self._tunnelRouteFallbackTest(uport1_rif, uport2_rif,
                                      uport1_dev, uport2_dev)

    def l2VxlanL3SubPortInterfaceTest(self):
        '''
        This runs all tests for the case when underlay RIF is L3 subport
        interface
        '''
        print("\nl2VxlanL3SubPortInterfaceTest()")

        # ports configuration according to assumed schemas
        uport = self.port24
        uport_dev = self.dev_port24

        vlan_no1 = 100
        vlan_no2 = 200

        try:
            urif1 = sai_thrift_create_router_interface(
                self.client,
                type=SAI_ROUTER_INTERFACE_TYPE_SUB_PORT,
                virtual_router_id=self.default_vrf,
                port_id=uport,
                outer_vlan_id=vlan_no1)

            urif2 = sai_thrift_create_router_interface(
                self.client,
                type=SAI_ROUTER_INTERFACE_TYPE_SUB_PORT,
                virtual_router_id=self.default_vrf,
                port_id=uport,
                outer_vlan_id=vlan_no2)

            self._sequenceTest1(urif1, [uport_dev], vlan_no1)
            self._sequenceTest2(urif1, [uport_dev], vlan_no1)
            self._sequenceTest3(urif1, [uport_dev], vlan_no1)
            self._sequenceTest4(urif1, [uport_dev], vlan_no1)
            self._sequenceTest5(urif1, [uport_dev], vlan_no1)
            self._sequenceTest6(urif1, [uport_dev], vlan_no1)
            self._sequenceTest7(urif1, [uport_dev], vlan_no1)
            self._sequenceTest8(urif1, [uport_dev], vlan_no1)
            self._sequenceTest9(urif1, [uport_dev], vlan_no1)
            self._sequenceTest10(urif1, [uport_dev], vlan_no1)
            self._sequenceTest11(urif1, [uport_dev], vlan_no1)
            self._sequenceTest12(urif1, [uport_dev], vlan_no1)
            self._tunnelRouteUpdateTest(urif1, [uport_dev], vlan_no1)
            self._tunnelRouteFallbackTest(urif1, urif2,
                                          [uport_dev], [uport_dev],
                                          vlan_no1, vlan_no2)

        finally:
            sai_thrift_remove_router_interface(self.client, urif2)
            sai_thrift_remove_router_interface(self.client, urif1)

    def l2VxlanSviInterfaceTest(self):
        '''
        This runs all tests for the case when underlay RIF is SVI
        '''
        print("\nl2VxlanSviInterfaceTest()")

        # ports configuration according to assumed schemas
        urif = self.svi_uport_rif
        mac_port1 = self.svi_uport1_dev  # port20
        mac_port1_bp = self.port20_bp
        mac_port2 = self.svi_uport2_dev  # port21
        mac_port2_bp = self.port21_bp
        vlan_id = self.svi_uport
        vlan_no = 30

        try:
            # yet another MAC entries for unicast forwarding within VLAN
            mac_entry1 = sai_thrift_fdb_entry_t(switch_id=self.switch_id,
                                                mac_address=self.unbor1_mac,
                                                bv_id=vlan_id)
            sai_thrift_create_fdb_entry(self.client,
                                        mac_entry1,
                                        type=SAI_FDB_ENTRY_TYPE_STATIC,
                                        bridge_port_id=mac_port1_bp)

            mac_entry2 = sai_thrift_fdb_entry_t(switch_id=self.switch_id,
                                                mac_address=self.unbor2_mac,
                                                bv_id=vlan_id)
            sai_thrift_create_fdb_entry(self.client,
                                        mac_entry2,
                                        type=SAI_FDB_ENTRY_TYPE_STATIC,
                                        bridge_port_id=mac_port1_bp)

            self._sequenceTest1(urif, [mac_port1])
            self._sequenceTest2(urif, [mac_port1])
            self._sequenceTest3(urif, [mac_port1])
            self._sequenceTest4(urif, [mac_port1])
            self._sequenceTest5(urif, [mac_port1])
            self._sequenceTest6(urif, [mac_port1])
            self._sequenceTest7(urif, [mac_port1])
            self._sequenceTest8(urif, [mac_port1])
            self._sequenceTest9(urif, [mac_port1])
            self._sequenceTest10(urif, [mac_port1])
            self._sequenceTest11(urif, [mac_port1])
            self._sequenceTest12(urif, [mac_port1])
            self._tunnelRouteUpdateTest(urif, [mac_port1])
            self._sviMacMoveTest(urif, [mac_port1], [mac_port2],
                                 vlan_no, mac_entry1, mac_port2_bp)

            sai_thrift_remove_fdb_entry(self.client, mac_entry2)

            urif2 = sai_thrift_create_router_interface(
                self.client,
                type=SAI_ROUTER_INTERFACE_TYPE_VLAN,
                virtual_router_id=self.default_vrf,
                vlan_id=self.vlan20)

            mac_port2 = self.dev_port2
            mac_entry2 = sai_thrift_fdb_entry_t(switch_id=self.switch_id,
                                                mac_address=self.unbor2_mac,
                                                bv_id=self.vlan20)
            sai_thrift_create_fdb_entry(self.client,
                                        mac_entry2,
                                        type=SAI_FDB_ENTRY_TYPE_STATIC,
                                        bridge_port_id=self.port2_bp)

            self._tunnelRouteFallbackTest(urif, urif2,
                                          [mac_port1], [mac_port2])

        finally:
            sai_thrift_remove_router_interface(self.client, urif2)
            sai_thrift_remove_fdb_entry(self.client, mac_entry2)
            sai_thrift_remove_fdb_entry(self.client, mac_entry1)

    def l2VxlanMultiFdbEntriesTest(self):
        '''
        This verifies co-existence of two FDB entries pointing to the same
        tunnel with following sequence of objects creation:
        FDB entry 1 -> FDB entry 2 -> underlay neighbor -> underlay nexthop ->
        tunnel route
        '''
        print("\nl2VxlanMultiFdbEntriesTest()")

        uport_dev = self.uport1_dev
        uport_rif = self.uport1_rif

        try:
            self._configureL2Tunnel()

            fdb_entry2 = sai_thrift_fdb_entry_t(
                switch_id=self.switch_id,
                mac_address=self.customer3_mac,
                bv_id=self.vlan10)
            sai_thrift_create_fdb_entry(
                self.client,
                fdb_entry2,
                type=SAI_FDB_ENTRY_TYPE_STATIC,
                bridge_port_id=self.p2p_tunnel_bp)

            unbor = sai_thrift_neighbor_entry_t(
                ip_address=sai_ipaddress(self.tun_ip),
                rif_id=uport_rif,
                switch_id=self.switch_id)
            sai_thrift_create_neighbor_entry(self.client,
                                             unbor,
                                             dst_mac_address=self.unbor1_mac,
                                             no_host_route=True)

            unhop = sai_thrift_create_next_hop(
                self.client,
                type=SAI_NEXT_HOP_TYPE_IP,
                ip=sai_ipaddress(self.tun_ip),
                router_interface_id=uport_rif)

            tunnel_route = sai_thrift_route_entry_t(
                vr_id=self.default_vrf,
                destination=sai_ipprefix(self.tun_ip + self.tun_ip_mask))
            sai_thrift_create_route_entry(self.client,
                                          tunnel_route,
                                          next_hop_id=unhop)

            self._trafficTest([uport_dev])
            sai_thrift_remove_fdb_entry(self.client, self.fdb_entry)
            self._floodTest([uport_dev])
            self._trafficTest([uport_dev], multi=True)

        finally:
            sai_thrift_remove_route_entry(self.client, tunnel_route)
            sai_thrift_remove_next_hop(self.client, unhop)
            sai_thrift_remove_neighbor_entry(self.client, unbor)
            sai_thrift_remove_fdb_entry(self.client, fdb_entry2)
            self._cleanupL2Tunnel(clean_fdb=False)


@group('l2-vxlan')
class L2VxLanObjectCreationSequenceIpv4UnderlayTest(
        L2VxLanObjectCreationSequenceBase):
    '''
    This class contains tests for different sequences of VxLAN objects creation
    '''

    def runTest(self):
        try:
            self.l2VxlanL3InterfaceTest()
            self.l2VxlanL3LagInterfaceTest()
            self.l2VxlanL3SubPortInterfaceTest()
            self.l2VxlanSviInterfaceTest()
            self.l2VxlanMultiFdbEntriesTest()
        finally:
            pass


@group('l2-vxlan')
@group('ipv6-underlay')
class L2VxLanObjectCreationSequenceIpv6UnderlayTest(
        L2VxLanObjectCreationSequenceBase):
    '''
    This class contains tests for different sequences of VxLAN objects creation
    '''

    def __init__(self):
        super(L2VxLanObjectCreationSequenceIpv6UnderlayTest, self).__init__(
            ipv6=True)

    def runTest(self):
        try:
            self.l2VxlanL3InterfaceTest()
            self.l2VxlanL3LagInterfaceTest()
            self.l2VxlanL3SubPortInterfaceTest()
            self.l2VxlanSviInterfaceTest()
            self.l2VxlanMultiFdbEntriesTest()
        finally:
            pass


@group('l2-vxlan')
class L2VxLanAttrTest(SaiHelper):
    '''
    This class verifies various L2 VxLAN-related attributes
    '''

    def setUp(self):
        super(L2VxLanAttrTest, self).setUp()

        self.tun_ip = "10.0.0.65"
        self.lpb_ip = "10.1.1.10"
        self.uport_ip = "10.0.0.64"
        self.customer1_ip = "192.168.100.1"
        self.customer2_ip = "192.168.100.2"
        self.customer1_mac = "00:22:22:22:22:11"
        self.customer2_mac = "00:22:22:22:22:22"
        self.unbor_mac = "00:33:33:33:33:11"
        self.vni = 1000

        self.oport = self.port0
        self.oport_dev = self.dev_port0
        self.oport_bp = self.port0_bp
        self.uport = self.port10
        self.uport_dev = self.dev_port10
        self.uport_rif = self.port10_rif

        # underlay loopback rif for tunnels
        self.urif_lpb = sai_thrift_create_router_interface(
            self.client,
            type=SAI_ROUTER_INTERFACE_TYPE_LOOPBACK,
            virtual_router_id=self.default_vrf,
            mtu=9100)

        # tunnel configuration
        self.tunnel_type = SAI_TUNNEL_TYPE_VXLAN

        self.encap_tunnel_map_vlan = sai_thrift_create_tunnel_map(
            self.client, type=SAI_TUNNEL_MAP_TYPE_VLAN_ID_TO_VNI)

        self.decap_tunnel_map_vlan = sai_thrift_create_tunnel_map(
            self.client, type=SAI_TUNNEL_MAP_TYPE_VNI_TO_VLAN_ID)

        encap_maps = sai_thrift_object_list_t(
            count=1,
            idlist=[self.encap_tunnel_map_vlan])
        decap_maps = sai_thrift_object_list_t(
            count=1,
            idlist=[self.decap_tunnel_map_vlan])

        self.p2mp_tunnel = sai_thrift_create_tunnel(
            self.client,
            type=self.tunnel_type,
            underlay_interface=self.urif_lpb,
            decap_mappers=decap_maps,
            encap_mappers=encap_maps,
            encap_src_ip=sai_ipaddress(self.lpb_ip),
            peer_mode=SAI_TUNNEL_PEER_MODE_P2MP)

        self.tunnel_term = sai_thrift_create_tunnel_term_table_entry(
            self.client,
            type=SAI_TUNNEL_TERM_TABLE_ENTRY_TYPE_P2P,
            vr_id=self.default_vrf,
            dst_ip=sai_ipaddress(self.lpb_ip),
            tunnel_type=self.tunnel_type,
            action_tunnel_id=self.p2mp_tunnel)

        self.encap_tun_map_entry_vlan10 = sai_thrift_create_tunnel_map_entry(
            self.client,
            tunnel_map_type=SAI_TUNNEL_MAP_TYPE_VLAN_ID_TO_VNI,
            tunnel_map=self.encap_tunnel_map_vlan,
            vlan_id_key=10,
            vni_id_value=self.vni)

        self.decap_tun_map_entry_vlan10 = sai_thrift_create_tunnel_map_entry(
            self.client,
            tunnel_map_type=SAI_TUNNEL_MAP_TYPE_VNI_TO_VLAN_ID,
            tunnel_map=self.decap_tunnel_map_vlan,
            vlan_id_key=10,
            vni_id_value=self.vni)

        self.p2p_tunnel = sai_thrift_create_tunnel(
            self.client,
            type=self.tunnel_type,
            underlay_interface=self.urif_lpb,
            decap_mappers=decap_maps,
            encap_mappers=encap_maps,
            encap_src_ip=sai_ipaddress(self.lpb_ip),
            peer_mode=SAI_TUNNEL_PEER_MODE_P2P,
            encap_dst_ip=sai_ipaddress(self.tun_ip))

        self.p2p_tunnel_bp = sai_thrift_create_bridge_port(
            self.client,
            type=SAI_BRIDGE_PORT_TYPE_TUNNEL,
            tunnel_id=self.p2p_tunnel,
            bridge_id=self.default_1q_bridge,
            admin_state=True,
            fdb_learning_mode=SAI_BRIDGE_PORT_FDB_LEARNING_MODE_DISABLE)

        self.p2p_tun_vlan10_member = sai_thrift_create_vlan_member(
            self.client,
            vlan_id=self.vlan10,
            bridge_port_id=self.p2p_tunnel_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)

        # FDB settings
        fdb_entry = sai_thrift_fdb_entry_t(
            bv_id=self.vlan10,
            mac_address=self.customer1_mac,
            switch_id=self.switch_id)
        sai_thrift_create_fdb_entry(self.client,
                                    fdb_entry,
                                    type=SAI_FDB_ENTRY_TYPE_STATIC,
                                    bridge_port_id=self.oport_bp)

    def runTest(self):
        try:
            self.l2VxlanTunnelTermDstIpTest()
            self.l2VxlanTunnelTermSrcIpTest()
            self.l2VxlanTunnelTermVrIdTest()
        finally:
            pass

    def tearDown(self):
        sai_thrift_flush_fdb_entries(
            self.client, entry_type=SAI_FDB_FLUSH_ENTRY_TYPE_ALL)

        sai_thrift_remove_vlan_member(self.client, self.p2p_tun_vlan10_member)
        sai_thrift_remove_bridge_port(self.client, self.p2p_tunnel_bp)
        sai_thrift_remove_tunnel(self.client, self.p2p_tunnel)
        sai_thrift_remove_tunnel_map_entry(
            self.client, self.decap_tun_map_entry_vlan10)
        sai_thrift_remove_tunnel_map_entry(
            self.client, self.encap_tun_map_entry_vlan10)
        sai_thrift_remove_tunnel_term_table_entry(
            self.client, self.tunnel_term)
        sai_thrift_remove_tunnel(self.client, self.p2mp_tunnel)
        sai_thrift_remove_tunnel_map(self.client, self.decap_tunnel_map_vlan)
        sai_thrift_remove_tunnel_map(self.client, self.encap_tunnel_map_vlan)
        sai_thrift_remove_router_interface(self.client, self.urif_lpb)

        super(L2VxLanAttrTest, self).tearDown()

    def l2VxlanTunnelTermDstIpTest(self):
        '''
        Verify tunnel termination dst IP attribute
        '''
        print("\nl2VxlanTunnelTermDstIpTest()")

        try:
            pkt = simple_udp_packet(eth_dst=self.customer1_mac,
                                    eth_src=self.customer2_mac,
                                    ip_dst=self.customer1_ip,
                                    ip_src=self.customer2_ip,
                                    ip_id=108,
                                    ip_ttl=64,
                                    pktlen=100)
            vxlan_pkt = simple_vxlan_packet(eth_dst=ROUTER_MAC,
                                            eth_src=self.unbor_mac,
                                            ip_dst=self.lpb_ip,
                                            ip_src=self.tun_ip,
                                            ip_id=0,
                                            ip_ttl=64,
                                            ip_flags=0x2,
                                            with_udp_chksum=False,
                                            vxlan_vni=self.vni,
                                            inner_frame=pkt)

            print("Sending packet with initial dst IP")
            send_packet(self, self.uport_dev, vxlan_pkt)
            verify_packet(self, pkt, self.oport_dev)
            print("Packet was L2 forwarded")

            print("Change tunnel termination dst IP")
            new_ip = "20.20.20.1"
            sai_thrift_remove_tunnel_term_table_entry(self.client,
                                                      self.tunnel_term)
            self.tunnel_term = sai_thrift_create_tunnel_term_table_entry(
                self.client,
                type=SAI_TUNNEL_TERM_TABLE_ENTRY_TYPE_P2MP,
                vr_id=self.default_vrf,
                dst_ip=sai_ipaddress(new_ip),
                tunnel_type=self.tunnel_type,
                action_tunnel_id=self.p2mp_tunnel)

            print("Sending packet with initial dst IP")
            send_packet(self, self.uport_dev, vxlan_pkt)
            verify_no_other_packets(self)
            print("Packet was dropped")

            vxlan_pkt = simple_vxlan_packet(eth_dst=ROUTER_MAC,
                                            eth_src=self.unbor_mac,
                                            ip_dst=new_ip,
                                            ip_src=self.tun_ip,
                                            ip_id=0,
                                            ip_ttl=64,
                                            ip_flags=0x2,
                                            with_udp_chksum=False,
                                            vxlan_vni=self.vni,
                                            inner_frame=pkt)

            print("Sending packet with new dst IP")
            send_packet(self, self.uport_dev, vxlan_pkt)
            verify_packet(self, pkt, self.oport_dev)
            print("Packet was L2 forwarded")

        finally:
            # restore original tunnel termination
            sai_thrift_remove_tunnel_term_table_entry(self.client,
                                                      self.tunnel_term)
            self.tunnel_term = sai_thrift_create_tunnel_term_table_entry(
                self.client,
                type=SAI_TUNNEL_TERM_TABLE_ENTRY_TYPE_P2MP,
                vr_id=self.default_vrf,
                dst_ip=sai_ipaddress(self.lpb_ip),
                tunnel_type=self.tunnel_type,
                action_tunnel_id=self.p2mp_tunnel)

    def l2VxlanTunnelTermSrcIpTest(self):
        '''
        Verify tunnel termination src IP attribute
        '''
        print("\nl2VxlanTunnelTermSrcIpTest()")

        try:
            pkt = simple_udp_packet(eth_dst=self.customer1_mac,
                                    eth_src=self.customer2_mac,
                                    ip_dst=self.customer1_ip,
                                    ip_src=self.customer2_ip,
                                    ip_id=108,
                                    ip_ttl=64,
                                    pktlen=100)
            vxlan_pkt = simple_vxlan_packet(eth_dst=ROUTER_MAC,
                                            eth_src=self.unbor_mac,
                                            ip_dst=self.lpb_ip,
                                            ip_src=self.tun_ip,
                                            ip_id=0,
                                            ip_ttl=64,
                                            ip_flags=0x2,
                                            with_udp_chksum=False,
                                            vxlan_vni=self.vni,
                                            inner_frame=pkt)

            print("Sending packet with default src IP")
            send_packet(self, self.uport_dev, vxlan_pkt)
            verify_packet(self, pkt, self.oport_dev)
            print("Packet was L2 forwarded")

            print("Change tunnel termination src IP")
            new_ip = "20.20.20.1"
            sai_thrift_remove_tunnel_term_table_entry(self.client,
                                                      self.tunnel_term)
            self.tunnel_term = sai_thrift_create_tunnel_term_table_entry(
                self.client,
                type=SAI_TUNNEL_TERM_TABLE_ENTRY_TYPE_P2P,
                vr_id=self.default_vrf,
                dst_ip=sai_ipaddress(self.lpb_ip),
                src_ip=sai_ipaddress(new_ip),
                tunnel_type=self.tunnel_type,
                action_tunnel_id=self.p2mp_tunnel)

            print("Sending packet with initial src IP")
            send_packet(self, self.uport_dev, vxlan_pkt)
            verify_no_other_packets(self)
            print("Packet was dropped")

            vxlan_pkt = simple_vxlan_packet(eth_dst=ROUTER_MAC,
                                            eth_src=self.unbor_mac,
                                            ip_dst=self.lpb_ip,
                                            ip_src=new_ip,
                                            ip_id=0,
                                            ip_ttl=64,
                                            ip_flags=0x2,
                                            with_udp_chksum=False,
                                            vxlan_vni=self.vni,
                                            inner_frame=pkt)

            print("Sending packet with new tunnel termination src IP")
            send_packet(self, self.uport_dev, vxlan_pkt)
            verify_packet(self, pkt, self.oport_dev)
            print("Packet was L2 forwarded")

        finally:
            # restore original tunnel termination
            sai_thrift_remove_tunnel_term_table_entry(self.client,
                                                      self.tunnel_term)
            self.tunnel_term = sai_thrift_create_tunnel_term_table_entry(
                self.client,
                type=SAI_TUNNEL_TERM_TABLE_ENTRY_TYPE_P2P,
                vr_id=self.default_vrf,
                dst_ip=sai_ipaddress(self.lpb_ip),
                tunnel_type=self.tunnel_type,
                action_tunnel_id=self.p2mp_tunnel)

    def l2VxlanTunnelTermVrIdTest(self):
        '''
        Verify tunnel termination virtual router ID attribute
        '''
        print("\nl2VxlanTunnelTermVrIdTest()")

        # config in another VRF
        uvrf = sai_thrift_create_virtual_router(self.client)

        uport2 = self.port24
        uport2_dev = self.dev_port24

        try:
            uport2_rif = sai_thrift_create_router_interface(
                self.client,
                type=SAI_ROUTER_INTERFACE_TYPE_PORT,
                virtual_router_id=uvrf,
                port_id=uport2)

            pkt = simple_udp_packet(eth_dst=self.customer1_mac,
                                    eth_src=self.customer2_mac,
                                    ip_dst=self.customer1_ip,
                                    ip_src=self.customer2_ip,
                                    ip_id=108,
                                    ip_ttl=64,
                                    pktlen=100)
            vxlan_pkt = simple_vxlan_packet(eth_dst=ROUTER_MAC,
                                            eth_src=self.unbor_mac,
                                            ip_dst=self.lpb_ip,
                                            ip_src=self.tun_ip,
                                            ip_id=0,
                                            ip_ttl=64,
                                            ip_flags=0x2,
                                            with_udp_chksum=False,
                                            vxlan_vni=self.vni,
                                            inner_frame=pkt)

            print("Sending packet in default VRF")
            send_packet(self, self.uport_dev, vxlan_pkt)
            verify_packet(self, pkt, self.oport_dev)
            print("Packet was L2 forwarded")

            print("Sending packet in another VRF")
            send_packet(self, uport2_dev, vxlan_pkt)
            verify_no_other_packets(self)
            print("Packet was dropped")

        finally:
            sai_thrift_flush_fdb_entries(
                self.client, entry_type=SAI_FDB_FLUSH_ENTRY_TYPE_ALL)
            sai_thrift_remove_router_interface(self.client, uport2_rif)
            sai_thrift_remove_virtual_router(self.client, uvrf)


@group('l2-vxlan')
@group('ttl_mode')
class L2VxLanTunnelModeTest(SaiHelper):
    '''
    This class contains tests for L2 VxLAN tunnel modes
    '''

    def setUp(self):
        super(L2VxLanTunnelModeTest, self).setUp()

        self.oport1 = self.port0
        self.oport1_dev = self.dev_port0
        self.oport1_bp = self.port0_bp
        self.uport1 = self.port10
        self.uport1_dev = self.dev_port10
        self.uport1_rif = self.port10_rif

        self.vni1 = 1000
        self.tun1_ip = "10.0.0.65"
        self.lpb_ip = "10.1.0.32"
        self.tun_ip_mask = "/32"
        self.uport1_myip = "10.0.0.64"
        self.uport_mask = "/31"

        self.customer1_ip = "192.168.100.1"
        self.customer1_ipv6 = "2001:0db8::100:2"
        self.customer2_ip = "192.168.100.8"
        self.customer2_ipv6 = "2001:0db8::100:9"
        self.customer1_mac = "00:22:22:22:22:11"
        self.customer2_mac = "00:22:22:22:22:22"
        self.unbor1_mac = "00:33:33:33:33:11"

        # underlay loopback rif for tunnels
        self.urif_lpb = sai_thrift_create_router_interface(
            self.client,
            type=SAI_ROUTER_INTERFACE_TYPE_LOOPBACK,
            virtual_router_id=self.default_vrf,
            mtu=9100)

        # underlay neighbor
        self.unbor = sai_thrift_neighbor_entry_t(
            ip_address=sai_ipaddress(self.tun1_ip),
            rif_id=self.uport1_rif,
            switch_id=self.switch_id)
        sai_thrift_create_neighbor_entry(self.client,
                                         self.unbor,
                                         dst_mac_address=self.unbor1_mac)

        # underlay nexthop
        self.unhop = sai_thrift_create_next_hop(
            self.client,
            type=SAI_NEXT_HOP_TYPE_IP,
            ip=sai_ipaddress(self.tun1_ip),
            router_interface_id=self.uport1_rif)

        # create fdb entry
        self.customer1_fdb_entry = sai_thrift_fdb_entry_t(
            bv_id=self.vlan10,
            mac_address=self.customer1_mac,
            switch_id=self.switch_id)
        sai_thrift_create_fdb_entry(self.client,
                                    self.customer1_fdb_entry,
                                    type=SAI_FDB_ENTRY_TYPE_STATIC,
                                    bridge_port_id=self.oport1_bp)

        # L2 VxLAN configuration below
        tunnel_type = SAI_TUNNEL_TYPE_VXLAN
        peer_mode = SAI_TUNNEL_PEER_MODE_P2MP
        term_type = SAI_TUNNEL_TERM_TABLE_ENTRY_TYPE_P2MP

        # create tunnel maps
        self.decap_tunnel_map_vlan = sai_thrift_create_tunnel_map(
            self.client, type=SAI_TUNNEL_MAP_TYPE_VNI_TO_VLAN_ID)

        self.encap_tunnel_map_vlan = sai_thrift_create_tunnel_map(
            self.client, type=SAI_TUNNEL_MAP_TYPE_VLAN_ID_TO_VNI)

        # create tunnel
        decap_maps = sai_thrift_object_list_t(
            count=1,
            idlist=[self.decap_tunnel_map_vlan])
        encap_maps = sai_thrift_object_list_t(
            count=1,
            idlist=[self.encap_tunnel_map_vlan])

        self.tunnel1 = sai_thrift_create_tunnel(
            self.client,
            type=tunnel_type,
            underlay_interface=self.urif_lpb,
            decap_mappers=decap_maps,
            encap_mappers=encap_maps,
            encap_src_ip=sai_ipaddress(self.lpb_ip),
            peer_mode=peer_mode)

        self.tunnel_term1 = sai_thrift_create_tunnel_term_table_entry(
            self.client,
            type=term_type,
            vr_id=self.default_vrf,
            dst_ip=sai_ipaddress(self.lpb_ip),
            tunnel_type=tunnel_type,
            action_tunnel_id=self.tunnel1)

        # create tunnel map entries for vlan
        self.decap_tunnel_map_entry_vlan = sai_thrift_create_tunnel_map_entry(
            self.client,
            tunnel_map_type=SAI_TUNNEL_MAP_TYPE_VNI_TO_VLAN_ID,
            tunnel_map=self.decap_tunnel_map_vlan,
            vlan_id_value=10,
            vni_id_key=self.vni1)

        self.encap_tunnel_map_entry_vlan = sai_thrift_create_tunnel_map_entry(
            self.client,
            tunnel_map_type=SAI_TUNNEL_MAP_TYPE_VLAN_ID_TO_VNI,
            tunnel_map=self.encap_tunnel_map_vlan,
            vlan_id_key=10,
            vni_id_value=self.vni1)

        # create p2p tunnel
        self.tunnel2 = sai_thrift_create_tunnel(
            self.client,
            type=tunnel_type,
            underlay_interface=self.urif_lpb,
            decap_mappers=decap_maps,
            encap_mappers=encap_maps,
            encap_src_ip=sai_ipaddress(self.lpb_ip),
            peer_mode=SAI_TUNNEL_PEER_MODE_P2P,
            encap_dst_ip=sai_ipaddress(self.tun1_ip))

        # bridge port is created on p2p tunnel
        self.tun2_bp = sai_thrift_create_bridge_port(
            self.client,
            type=SAI_BRIDGE_PORT_TYPE_TUNNEL,
            tunnel_id=self.tunnel2,
            bridge_id=self.default_1q_bridge,
            admin_state=True,
            fdb_learning_mode=SAI_BRIDGE_PORT_FDB_LEARNING_MODE_DISABLE)

        # vlan_member is created using tunnel bridge_port
        self.tun2_vlan_member = sai_thrift_create_vlan_member(
            self.client,
            vlan_id=self.vlan10,
            bridge_port_id=self.tun2_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)

        # fdb entry for L2 forwarding into VXLAN tunnel
        fdb_entry = sai_thrift_fdb_entry_t(
            bv_id=self.vlan10,
            mac_address=self.customer2_mac,
            switch_id=self.switch_id)
        sai_thrift_create_fdb_entry(self.client,
                                    fdb_entry,
                                    type=SAI_FDB_ENTRY_TYPE_STATIC,
                                    bridge_port_id=self.tun2_bp)

    def runTest(self):
        try:
            self.l2VxLanDefaultEncapDecapTtlTest()
            self.l2VxLanDefaultEncapDecapDscpTest()
            self.l2VxLanEncapChangingTtlTest()
            self.l2VxLanDecapChangingTtlTest()
            self.l2VxLanEncapChangingDscpTest()
            self.l2VxLanDecapChangingDscpTest()
        finally:
            pass

    def tearDown(self):
        sai_thrift_flush_fdb_entries(
            self.client, entry_type=SAI_FDB_FLUSH_ENTRY_TYPE_ALL)

        sai_thrift_remove_vlan_member(self.client, self.tun2_vlan_member)
        sai_thrift_remove_bridge_port(self.client, self.tun2_bp)
        sai_thrift_remove_tunnel(self.client, self.tunnel2)
        sai_thrift_remove_tunnel_map_entry(
            self.client, self.decap_tunnel_map_entry_vlan)
        sai_thrift_remove_tunnel_map_entry(
            self.client, self.encap_tunnel_map_entry_vlan)
        sai_thrift_remove_tunnel_term_table_entry(
            self.client, self.tunnel_term1)
        sai_thrift_remove_tunnel(self.client, self.tunnel1)
        sai_thrift_remove_tunnel_map(self.client, self.encap_tunnel_map_vlan)
        sai_thrift_remove_tunnel_map(self.client, self.decap_tunnel_map_vlan)

        sai_thrift_flush_fdb_entries(
            self.client, entry_type=SAI_FDB_FLUSH_ENTRY_TYPE_ALL)

        sai_thrift_remove_next_hop(self.client, self.unhop)
        sai_thrift_remove_neighbor_entry(self.client, self.unbor)
        sai_thrift_remove_router_interface(self.client, self.urif_lpb)

        super(L2VxLanTunnelModeTest, self).tearDown()

    def l2VxLanDefaultEncapDecapTtlTest(self):
        '''
        This test verifies correct outer TTL value for IPv4 and IPv6
        inner packet in TTL uniform mode with L2 VxLAN encapsulation
        and decapsulation.
        It also verifies if the value is correct after setting TTL mode.
        '''
        print("\nl2VxLanDefaultEncapDecapTtlTest()")

        # L2 forwarding into VXLAN tunnel
        pkt_encap = simple_udp_packet(
            eth_dst=self.customer2_mac,
            eth_src=self.customer1_mac,
            ip_dst=self.customer2_ip,
            ip_src=self.customer1_ip,
            ip_id=108,
            ip_ttl=60)

        vxlan_pkt_encap = Mask(
            simple_vxlan_packet(eth_dst=self.unbor1_mac,
                                eth_src=ROUTER_MAC,
                                ip_dst=self.tun1_ip,
                                ip_src=self.lpb_ip,
                                ip_id=0,
                                ip_ttl=60,
                                ip_flags=0x2,
                                with_udp_chksum=False,
                                vxlan_vni=self.vni1,
                                inner_frame=pkt_encap))
        vxlan_pkt_encap.set_do_not_care_scapy(UDP, 'sport')

        print("Sending packet from port %d -> VNI %d" %
              (self.oport1_dev, self.vni1))
        send_packet(self, self.oport1_dev, pkt_encap)
        verify_packet(self, vxlan_pkt_encap, self.uport1_dev)
        print("Packet was L2 forwarded into the VXLAN tunnel")

        # L2 forwarding into VXLAN tunnel IPv6
        pkt_ipv6_encap = simple_udpv6_packet(
            eth_dst=self.customer2_mac,
            eth_src=self.customer1_mac,
            ipv6_dst=self.customer2_ipv6,
            ipv6_src=self.customer1_ipv6,
            ipv6_hlim=60)
        vxlan_pkt_ipv6_encap = Mask(
            simple_vxlan_packet(eth_dst=self.unbor1_mac,
                                eth_src=ROUTER_MAC,
                                ip_dst=self.tun1_ip,
                                ip_src=self.lpb_ip,
                                ip_id=0,
                                ip_ttl=60,
                                ip_flags=0x2,
                                with_udp_chksum=False,
                                vxlan_vni=self.vni1,
                                inner_frame=pkt_ipv6_encap))
        vxlan_pkt_ipv6_encap.set_do_not_care_scapy(UDP, 'sport')

        print("Sending packet from port %d -> VNI %d (IPv6)" %
              (self.oport1_dev, self.vni1))
        send_packet(self, self.oport1_dev, pkt_ipv6_encap)
        verify_packet(self, vxlan_pkt_ipv6_encap, self.uport1_dev)
        print("Packet was L2 forwarded into the VXLAN tunnel (IPv6)")

        print("setting encap_ttl_mode to UNIFORM")
        sai_thrift_set_tunnel_attribute(
            self.client, self.tunnel1,
            encap_ttl_mode=SAI_TUNNEL_TTL_MODE_UNIFORM_MODEL)

        print("Sending packet from port %d -> VNI %d" %
              (self.oport1_dev, self.vni1))
        send_packet(self, self.oport1_dev, pkt_encap)
        verify_packet(self, vxlan_pkt_encap, self.uport1_dev)
        print("Packet was L2 forwarded into the VXLAN tunnel")

        print("Sending packet from port %d -> VNI %d (IPv6)" %
              (self.oport1_dev, self.vni1))
        send_packet(self, self.oport1_dev, pkt_ipv6_encap)
        verify_packet(self, vxlan_pkt_ipv6_encap, self.uport1_dev)
        print("Packet was L2 forwarded into the VXLAN tunnel (IPv6)")

        # L2 decap into vlan
        pkt_decap = simple_udp_packet(
            eth_dst=self.customer1_mac,
            eth_src=self.customer2_mac,
            ip_dst=self.customer1_ip,
            ip_src=self.customer2_ip,
            ip_id=108,
            ip_ttl=60)
        inner_pkt_decap = simple_udp_packet(
            eth_dst=self.customer1_mac,
            eth_src=self.customer2_mac,
            ip_dst=self.customer1_ip,
            ip_src=self.customer2_ip,
            ip_id=108,
            ip_ttl=64)
        vxlan_pkt_decap = simple_vxlan_packet(
            eth_dst=ROUTER_MAC,
            eth_src=self.unbor1_mac,
            ip_dst=self.lpb_ip,
            ip_src=self.tun1_ip,
            ip_id=0,
            ip_ttl=60,
            ip_flags=0x2,
            with_udp_chksum=False,
            vxlan_vni=self.vni1,
            inner_frame=inner_pkt_decap)

        print("Sending packet from VNI %d -> port %d" %
              (self.vni1, self.oport1_dev))
        send_packet(self, self.uport1_dev, vxlan_pkt_decap)
        verify_packet(self, pkt_decap, self.oport1_dev)
        print("Packet was L2 forwarded out of the VXLAN tunnel")

        # L2 decap into vlan IPv6
        pkt_ipv6_decap = simple_udpv6_packet(
            eth_dst=self.customer1_mac,
            eth_src=self.customer2_mac,
            ipv6_dst=self.customer1_ipv6,
            ipv6_src=self.customer2_ipv6,
            ipv6_hlim=60)
        inner_pkt_ipv6_decap = simple_udpv6_packet(
            eth_dst=self.customer1_mac,
            eth_src=self.customer2_mac,
            ipv6_dst=self.customer1_ipv6,
            ipv6_src=self.customer2_ipv6,
            ipv6_hlim=64)
        vxlan_pkt_ipv6_decap = simple_vxlan_packet(
            eth_dst=ROUTER_MAC,
            eth_src=self.unbor1_mac,
            ip_dst=self.lpb_ip,
            ip_src=self.tun1_ip,
            ip_id=0,
            ip_ttl=60,
            ip_flags=0x2,
            with_udp_chksum=False,
            vxlan_vni=self.vni1,
            inner_frame=inner_pkt_ipv6_decap)

        print("Sending packet from VNI %d -> port %d (IPv6)" %
              (self.vni1, self.oport1_dev))
        send_packet(self, self.uport1_dev, vxlan_pkt_ipv6_decap)
        verify_packet(self, pkt_ipv6_decap, self.oport1_dev)
        print("Packet was L2 forwarded out of the VXLAN tunnel (IPv6)")

        print("Setting decap_ttl_mode to UNIFORM")
        sai_thrift_set_tunnel_attribute(
            self.client, self.tunnel1,
            decap_ttl_mode=SAI_TUNNEL_TTL_MODE_UNIFORM_MODEL)

        # L2 decap into vlan
        print("Sending packet from VNI %d -> port %d" %
              (self.vni1, self.oport1_dev))
        send_packet(self, self.uport1_dev, vxlan_pkt_decap)
        verify_packet(self, pkt_decap, self.oport1_dev)
        print("Packet was L2 forwarded out of the VXLAN tunnel")

        # L2 decap into vlan IPv6
        print("Sending packet from VNI %d -> port %d (IPv6)" %
              (self.vni1, self.oport1_dev))
        send_packet(self, self.uport1_dev, vxlan_pkt_ipv6_decap)
        verify_packet(self, pkt_ipv6_decap, self.oport1_dev)
        print("Packet was L2 forwarded out of the VXLAN tunnel (IPv6)")

    def l2VxLanDefaultEncapDecapDscpTest(self):
        '''
        This test verifies correct outer DSCP value for IPv4 and IPv6
        inner packet in DSCP uniform mode with L2 VxLAN encapsulation
        and decapsulation.
        It also verifies if the value is correct after setting DSCP mode.
        '''
        print("\nl2VxLanDefaultEncapDecapDscpTest()")

        # L2 forwarding into VXLAN tunnel
        pkt_encap = simple_udp_packet(
            eth_dst=self.customer2_mac,
            eth_src=self.customer1_mac,
            ip_dst=self.customer2_ip,
            ip_src=self.customer1_ip,
            ip_id=108,
            ip_ttl=64,
            ip_dscp=20)

        vxlan_pkt_encap = Mask(
            simple_vxlan_packet(eth_dst=self.unbor1_mac,
                                eth_src=ROUTER_MAC,
                                ip_dst=self.tun1_ip,
                                ip_src=self.lpb_ip,
                                ip_id=0,
                                ip_ttl=64,
                                ip_dscp=20,
                                ip_flags=0x2,
                                with_udp_chksum=False,
                                vxlan_vni=self.vni1,
                                inner_frame=pkt_encap))
        vxlan_pkt_encap.set_do_not_care_scapy(UDP, 'sport')

        print("Sending packet from port %d -> VNI %d" %
              (self.oport1_dev, self.vni1))
        send_packet(self, self.oport1_dev, pkt_encap)
        verify_packet(self, vxlan_pkt_encap, self.uport1_dev)
        print("Packet was L2 forwarded into the VXLAN tunnel")

        # L2 forwarding into VXLAN tunnel IPv6
        pkt_ipv6_encap = simple_udpv6_packet(
            eth_dst=self.customer2_mac,
            eth_src=self.customer1_mac,
            ipv6_dst=self.customer2_ipv6,
            ipv6_src=self.customer1_ipv6,
            ipv6_hlim=64,
            ipv6_dscp=20)
        vxlan_pkt_ipv6_encap = Mask(
            simple_vxlan_packet(eth_dst=self.unbor1_mac,
                                eth_src=ROUTER_MAC,
                                ip_dst=self.tun1_ip,
                                ip_src=self.lpb_ip,
                                ip_id=0,
                                ip_ttl=64,
                                ip_dscp=20,
                                ip_flags=0x2,
                                with_udp_chksum=False,
                                vxlan_vni=self.vni1,
                                inner_frame=pkt_ipv6_encap))
        vxlan_pkt_ipv6_encap.set_do_not_care_scapy(UDP, 'sport')

        print("Sending packet from port %d -> VNI %d (IPv6)" %
              (self.oport1_dev, self.vni1))
        send_packet(self, self.oport1_dev, pkt_ipv6_encap)
        verify_packet(self, vxlan_pkt_ipv6_encap, self.uport1_dev)
        print("Packet was L2 forwarded into the VXLAN tunnel (IPv6)")

        print("setting encap_dscp_mode to UNIFORM")
        sai_thrift_set_tunnel_attribute(
            self.client, self.tunnel1,
            encap_dscp_mode=SAI_TUNNEL_DSCP_MODE_UNIFORM_MODEL)

        # L2 forwarding into VXLAN tunnel
        print("Sending packet from port %d -> VNI %d" %
              (self.oport1_dev, self.vni1))
        send_packet(self, self.oport1_dev, pkt_encap)
        verify_packet(self, vxlan_pkt_encap, self.uport1_dev)
        print("Packet was L2 forwarded into the VXLAN tunnel")

        # L2 forwarding into VXLAN tunnel IPv6
        print("Sending packet from port %d -> VNI %d (IPv6)" %
              (self.oport1_dev, self.vni1))
        send_packet(self, self.oport1_dev, pkt_ipv6_encap)
        verify_packet(self, vxlan_pkt_ipv6_encap, self.uport1_dev)
        print("Packet was L2 forwarded into the VXLAN tunnel (IPv6)")

        # L2 decap into vlan
        pkt_decap = simple_udp_packet(
            eth_dst=self.customer1_mac,
            eth_src=self.customer2_mac,
            ip_dst=self.customer1_ip,
            ip_src=self.customer2_ip,
            ip_id=108,
            ip_ttl=60,
            ip_dscp=20)
        inner_pkt_decap = simple_udp_packet(
            eth_dst=self.customer1_mac,
            eth_src=self.customer2_mac,
            ip_dst=self.customer1_ip,
            ip_src=self.customer2_ip,
            ip_id=108,
            ip_ttl=64,
            ip_dscp=10)
        vxlan_pkt_decap = simple_vxlan_packet(
            eth_dst=ROUTER_MAC,
            eth_src=self.unbor1_mac,
            ip_dst=self.lpb_ip,
            ip_src=self.tun1_ip,
            ip_id=0,
            ip_ttl=60,
            ip_dscp=20,
            ip_flags=0x2,
            with_udp_chksum=False,
            vxlan_vni=self.vni1,
            inner_frame=inner_pkt_decap)

        print("Sending packet from VNI %d -> port %d" %
              (self.vni1, self.oport1_dev))
        send_packet(self, self.uport1_dev, vxlan_pkt_decap)
        verify_packet(self, pkt_decap, self.oport1_dev)
        print("Packet was L2 forwarded out of the VXLAN tunnel")

        # L2 decap into vlan IPv6
        pkt_ipv6_decap = simple_udpv6_packet(
            eth_dst=self.customer1_mac,
            eth_src=self.customer2_mac,
            ipv6_dst=self.customer1_ipv6,
            ipv6_src=self.customer2_ipv6,
            ipv6_hlim=60,
            ipv6_dscp=20)
        inner_pkt_ipv6_decap = simple_udpv6_packet(
            eth_dst=self.customer1_mac,
            eth_src=self.customer2_mac,
            ipv6_dst=self.customer1_ipv6,
            ipv6_src=self.customer2_ipv6,
            ipv6_hlim=64,
            ipv6_dscp=10)
        vxlan_pkt_ipv6_decap = simple_vxlan_packet(
            eth_dst=ROUTER_MAC,
            eth_src=self.unbor1_mac,
            ip_dst=self.lpb_ip,
            ip_src=self.tun1_ip,
            ip_id=0,
            ip_ttl=60,
            ip_dscp=20,
            ip_flags=0x2,
            with_udp_chksum=False,
            vxlan_vni=self.vni1,
            inner_frame=inner_pkt_ipv6_decap)

        print("Sending packet from VNI %d -> port %d (IPv6)" %
              (self.vni1, self.oport1_dev))
        send_packet(self, self.uport1_dev, vxlan_pkt_ipv6_decap)
        verify_packet(self, pkt_ipv6_decap, self.oport1_dev)
        print("Packet was L2 forwarded out of the VXLAN tunnel (IPv6)")

        print("Setting decap_dscp_mode to UNIFORM")
        sai_thrift_set_tunnel_attribute(
            self.client, self.tunnel1,
            decap_dscp_mode=SAI_TUNNEL_DSCP_MODE_UNIFORM_MODEL)

        print("Sending packet from VNI %d -> port %d" %
              (self.vni1, self.oport1_dev))
        send_packet(self, self.uport1_dev, vxlan_pkt_decap)
        verify_packet(self, pkt_decap, self.oport1_dev)
        print("Packet was L2 forwarded out of the VXLAN tunnel")

        print("Sending packet from VNI %d -> port %d (IPv6)" %
              (self.vni1, self.oport1_dev))
        send_packet(self, self.uport1_dev, vxlan_pkt_ipv6_decap)
        verify_packet(self, pkt_ipv6_decap, self.oport1_dev)
        print("Packet was L2 forwarded out of the VXLAN tunnel (IPv6)")

    def l2VxLanEncapChangingTtlTest(self):
        '''
        This test verifies correct outer TTL value for IPv4 and IPv6
        inner packet with L2 VxLAN encapsulation in case of changing
        TTL mode.
        '''
        print("\nl2VxLanEncapChangingTtlTest()")

        try:
            print("setting encap_ttl_mode to UNIFORM")
            sai_thrift_set_tunnel_attribute(
                self.client, self.tunnel1,
                encap_ttl_mode=SAI_TUNNEL_TTL_MODE_UNIFORM_MODEL)

            # L2 forwarding into VXLAN tunnel
            inner_pkt = simple_udp_packet(
                eth_dst=self.customer2_mac,
                eth_src=self.customer1_mac,
                ip_dst=self.customer2_ip,
                ip_src=self.customer1_ip,
                ip_id=108,
                ip_ttl=60)
            vxlan_pkt_uniform = Mask(
                simple_vxlan_packet(
                    eth_dst=self.unbor1_mac,
                    eth_src=ROUTER_MAC,
                    ip_dst=self.tun1_ip,
                    ip_src=self.lpb_ip,
                    ip_id=0,
                    ip_ttl=60,
                    ip_flags=0x2,
                    with_udp_chksum=False,
                    vxlan_vni=self.vni1,
                    inner_frame=inner_pkt))
            vxlan_pkt_uniform.set_do_not_care_scapy(UDP, 'sport')

            print("Sending packet from port %d -> VNI %d" %
                  (self.oport1_dev, self.vni1))
            send_packet(self, self.oport1_dev, inner_pkt)
            verify_packet(self, vxlan_pkt_uniform, self.uport1_dev)
            print("Packet was L2 forwarded into the VXLAN tunnel")

            # L2 forwarding into VXLAN tunnel IPv6
            inner_pkt_ipv6 = simple_udpv6_packet(
                eth_dst=self.customer2_mac,
                eth_src=self.customer1_mac,
                ipv6_dst=self.customer2_ipv6,
                ipv6_src=self.customer1_ipv6,
                ipv6_hlim=60)
            vxlan_pkt_ipv6_uniform = Mask(
                simple_vxlan_packet(
                    eth_dst=self.unbor1_mac,
                    eth_src=ROUTER_MAC,
                    ip_dst=self.tun1_ip,
                    ip_src=self.lpb_ip,
                    ip_id=0,
                    ip_ttl=60,
                    ip_flags=0x2,
                    with_udp_chksum=False,
                    vxlan_vni=self.vni1,
                    inner_frame=inner_pkt_ipv6))
            vxlan_pkt_ipv6_uniform.set_do_not_care_scapy(UDP, 'sport')

            print("Sending packet from port %d -> VNI %d (IPv6)" %
                  (self.oport1_dev, self.vni1))
            send_packet(self, self.oport1_dev, inner_pkt_ipv6)
            verify_packet(self, vxlan_pkt_ipv6_uniform, self.uport1_dev)
            print("Packet was L2 forwarded into the VXLAN tunnel (IPv6)")

            print("Setting encap_ttl_mode to PIPE")
            sai_thrift_set_tunnel_attribute(
                self.client, self.tunnel1,
                encap_ttl_mode=SAI_TUNNEL_TTL_MODE_PIPE_MODEL)

            # L2 forwarding into VXLAN tunnel
            vxlan_pkt_pipe = Mask(simple_vxlan_packet(
                eth_dst=self.unbor1_mac,
                eth_src=ROUTER_MAC,
                ip_dst=self.tun1_ip,
                ip_src=self.lpb_ip,
                ip_id=0,
                ip_ttl=64,
                ip_flags=0x2,
                with_udp_chksum=False,
                vxlan_vni=self.vni1,
                inner_frame=inner_pkt))
            vxlan_pkt_pipe.set_do_not_care_scapy(UDP, 'sport')

            print("Sending packet from port %d -> VNI %d" %
                  (self.oport1_dev, self.vni1))
            send_packet(self, self.oport1_dev, inner_pkt)
            verify_packet(self, vxlan_pkt_pipe, self.uport1_dev)
            print("Packet was L2 forwarded into the VXLAN tunnel")

            # L2 forwarding into VXLAN tunnel IPv6
            vxlan_pkt_ipv6_pipe = Mask(simple_vxlan_packet(
                eth_dst=self.unbor1_mac,
                eth_src=ROUTER_MAC,
                ip_dst=self.tun1_ip,
                ip_src=self.lpb_ip,
                ip_id=0,
                ip_ttl=64,
                ip_flags=0x2,
                with_udp_chksum=False,
                vxlan_vni=self.vni1,
                inner_frame=inner_pkt_ipv6))
            vxlan_pkt_ipv6_pipe.set_do_not_care_scapy(UDP, 'sport')

            print("Sending packet from port %d -> VNI %d (IPv6)" %
                  (self.oport1_dev, self.vni1))
            send_packet(self, self.oport1_dev, inner_pkt_ipv6)
            verify_packet(self, vxlan_pkt_ipv6_pipe, self.uport1_dev)
            print("Packet was L2 forwarded into the VXLAN tunnel (IPv6)")

            print("Change default encap_ttl_val")
            sai_thrift_set_tunnel_attribute(
                self.client, self.tunnel1, encap_ttl_val=25)

            vxlan_pkt_pipe = Mask(simple_vxlan_packet(
                eth_dst=self.unbor1_mac,
                eth_src=ROUTER_MAC,
                ip_dst=self.tun1_ip,
                ip_src=self.lpb_ip,
                ip_id=0,
                ip_ttl=25,
                ip_flags=0x2,
                with_udp_chksum=False,
                vxlan_vni=self.vni1,
                inner_frame=inner_pkt))
            vxlan_pkt_pipe.set_do_not_care_scapy(UDP, 'sport')

            print("Sending packet from port %d -> VNI %d" %
                  (self.oport1_dev, self.vni1))
            send_packet(self, self.oport1_dev, inner_pkt)
            verify_packet(self, vxlan_pkt_pipe, self.uport1_dev)
            print("Packet was L2 forwarded into the VXLAN tunnel")

            vxlan_pkt_ipv6_pipe = Mask(simple_vxlan_packet(
                eth_dst=self.unbor1_mac,
                eth_src=ROUTER_MAC,
                ip_dst=self.tun1_ip,
                ip_src=self.lpb_ip,
                ip_id=0,
                ip_ttl=25,
                ip_flags=0x2,
                with_udp_chksum=False,
                vxlan_vni=self.vni1,
                inner_frame=inner_pkt_ipv6))
            vxlan_pkt_ipv6_pipe.set_do_not_care_scapy(UDP, 'sport')

            print("Sending packet from port %d -> VNI %d (IPv6)" %
                  (self.oport1_dev, self.vni1))
            send_packet(self, self.oport1_dev, inner_pkt_ipv6)
            verify_packet(self, vxlan_pkt_ipv6_pipe, self.uport1_dev)
            print("Packet was L2 forwarded into the VXLAN tunnel (IPv6)")

            print("Reset encap_ttl_val")
            sai_thrift_set_tunnel_attribute(
                self.client, self.tunnel1, encap_ttl_val=64)

            print("setting encap_ttl_mode to UNIFORM")
            sai_thrift_set_tunnel_attribute(
                self.client, self.tunnel1,
                encap_ttl_mode=SAI_TUNNEL_TTL_MODE_UNIFORM_MODEL)

            print("Sending packet from port %d -> VNI %d" %
                  (self.oport1_dev, self.vni1))
            send_packet(self, self.oport1_dev, inner_pkt)
            verify_packet(self, vxlan_pkt_uniform, self.uport1_dev)
            print("Packet was L2 forwarded into the VXLAN tunnel")

            print("Sending packet from port %d -> VNI %d (IPv6)" %
                  (self.oport1_dev, self.vni1))
            send_packet(self, self.oport1_dev, inner_pkt_ipv6)
            verify_packet(self, vxlan_pkt_ipv6_uniform, self.uport1_dev)
            print("Packet was L2 forwarded into the VXLAN tunnel (IPv6)")

        finally:
            sai_thrift_set_tunnel_attribute(
                self.client, self.tunnel1,
                encap_ttl_mode=SAI_TUNNEL_TTL_MODE_UNIFORM_MODEL)

    def l2VxLanDecapChangingTtlTest(self):
        '''
        This test verifies correct outer TTL value for IPv4 and IPv6
        inner packet with L2 VxLAN decapsulation in case of changing
        TTL mode.
        '''
        print("\nl2VxLanDecapChangingTtlTest()")

        try:
            print("setting decap_ttl_mode to UNIFORM")
            sai_thrift_set_tunnel_attribute(
                self.client, self.tunnel1,
                decap_ttl_mode=SAI_TUNNEL_TTL_MODE_UNIFORM_MODEL)

            # L2 decap into vlan
            pkt_ipv4 = simple_udp_packet(
                eth_dst=self.customer1_mac,
                eth_src=self.customer2_mac,
                ip_dst=self.customer1_ip,
                ip_src=self.customer2_ip,
                ip_id=108,
                ip_ttl=64)
            vxlan_pkt = simple_vxlan_packet(
                eth_dst=ROUTER_MAC,
                eth_src=self.unbor1_mac,
                ip_dst=self.lpb_ip,
                ip_src=self.tun1_ip,
                ip_id=0,
                ip_ttl=60,
                ip_flags=0x2,
                with_udp_chksum=False,
                vxlan_vni=self.vni1,
                inner_frame=pkt_ipv4)

            exp_pkt_uniform = simple_udp_packet(
                eth_dst=self.customer1_mac,
                eth_src=self.customer2_mac,
                ip_dst=self.customer1_ip,
                ip_src=self.customer2_ip,
                ip_id=108,
                ip_ttl=60)

            print("Sending packet from VNI %d -> port %d" %
                  (self.vni1, self.oport1_dev))
            send_packet(self, self.uport1_dev, vxlan_pkt)
            verify_packet(self, exp_pkt_uniform, self.oport1_dev)
            print("Packet was L2 forwarded out of the VXLAN tunnel")

            # L2 decap into vlan IPv6
            pkt_ipv6 = simple_udpv6_packet(
                eth_dst=self.customer1_mac,
                eth_src=self.customer2_mac,
                ipv6_dst=self.customer1_ipv6,
                ipv6_src=self.customer2_ipv6,
                ipv6_hlim=64)
            vxlan_pkt_ipv6 = simple_vxlan_packet(
                eth_dst=ROUTER_MAC,
                eth_src=self.unbor1_mac,
                ip_dst=self.lpb_ip,
                ip_src=self.tun1_ip,
                ip_id=0,
                ip_ttl=60,
                ip_flags=0x2,
                with_udp_chksum=False,
                vxlan_vni=self.vni1,
                inner_frame=pkt_ipv6)

            exp_pkt_ipv6 = simple_udpv6_packet(
                eth_dst=self.customer1_mac,
                eth_src=self.customer2_mac,
                ipv6_dst=self.customer1_ipv6,
                ipv6_src=self.customer2_ipv6,
                ipv6_hlim=60)

            print("Sending packet from VNI %d -> port %d (IPv6)" %
                  (self.vni1, self.oport1_dev))
            send_packet(self, self.uport1_dev, vxlan_pkt_ipv6)
            verify_packet(self, exp_pkt_ipv6, self.oport1_dev)
            print("Packet was L2 forwarded out of the VXLAN tunnel (IPv6)")

            print("setting decap_ttl_mode to PIPE")
            sai_thrift_set_tunnel_attribute(
                self.client, self.tunnel1,
                decap_ttl_mode=SAI_TUNNEL_TTL_MODE_PIPE_MODEL)

            # L2 decap into vlan
            print("Sending packet from VNI %d -> port %d" %
                  (self.vni1, self.oport1_dev))
            send_packet(self, self.uport1_dev, vxlan_pkt)
            verify_packet(self, pkt_ipv4, self.oport1_dev)
            print("Packet was L2 forwarded out of the VXLAN tunnel")

            # L2 decap into vlan IPv6
            print("Sending packet from VNI %d -> port %d (IPv6)" %
                  (self.vni1, self.oport1_dev))
            send_packet(self, self.uport1_dev, vxlan_pkt_ipv6)
            verify_packet(self, pkt_ipv6, self.oport1_dev)
            print("Packet was L2 forwarded out of the VXLAN tunnel (IPv6)")

            print("Setting decap_ttl_mode to UNIFORM")
            sai_thrift_set_tunnel_attribute(
                self.client, self.tunnel1,
                decap_ttl_mode=SAI_TUNNEL_TTL_MODE_UNIFORM_MODEL)

            print("Sending packet from VNI %d -> port %d" %
                  (self.vni1, self.oport1_dev))
            send_packet(self, self.uport1_dev, vxlan_pkt)
            verify_packet(self, exp_pkt_uniform, self.oport1_dev)
            print("Packet was L2 forwarded out of the VXLAN tunnel")

            print("Sending packet from VNI %d -> port %d (IPv6)" %
                  (self.vni1, self.oport1_dev))
            send_packet(self, self.uport1_dev, vxlan_pkt_ipv6)
            verify_packet(self, exp_pkt_ipv6, self.oport1_dev)
            print("Packet was L2 forwarded out of the VXLAN tunnel (IPv6)")

        finally:
            sai_thrift_set_tunnel_attribute(
                self.client, self.tunnel1,
                decap_ttl_mode=SAI_TUNNEL_TTL_MODE_UNIFORM_MODEL)

    def l2VxLanEncapChangingDscpTest(self):
        '''
        This test verifies correct outer DSCP value for IPv4 and IPv6
        inner packet with L2 VxLAN encapsulation in case of changing
        DSCP mode.
        '''
        print("\nl2VxLanEncapChangingDscpTest()")

        try:
            print("setting encap_dscp_mode to PIPE")
            sai_thrift_set_tunnel_attribute(
                self.client, self.tunnel1,
                encap_dscp_mode=SAI_TUNNEL_DSCP_MODE_PIPE_MODEL)

            # L2 forwarding into VXLAN tunnel
            inner_pkt = simple_udp_packet(
                eth_dst=self.customer2_mac,
                eth_src=self.customer1_mac,
                ip_dst=self.customer2_ip,
                ip_src=self.customer1_ip,
                ip_id=108,
                ip_dscp=20,
                ip_ttl=64)
            vxlan_pkt_pipe = Mask(
                simple_vxlan_packet(
                    eth_dst=self.unbor1_mac,
                    eth_src=ROUTER_MAC,
                    ip_dst=self.tun1_ip,
                    ip_src=self.lpb_ip,
                    ip_id=0,
                    ip_ttl=64,
                    ip_dscp=0,
                    ip_flags=0x2,
                    with_udp_chksum=False,
                    vxlan_vni=self.vni1,
                    inner_frame=inner_pkt))
            vxlan_pkt_pipe.set_do_not_care_scapy(UDP, 'sport')

            print("Sending packet from port %d -> VNI %d" %
                  (self.oport1_dev, self.vni1))
            send_packet(self, self.oport1_dev, inner_pkt)
            verify_packet(self, vxlan_pkt_pipe, self.uport1_dev)
            print("Packet was L2 forwarded into the VXLAN tunnel")

            # L2 forwarding into VXLAN tunnel IPv6
            inner_pkt_ipv6 = simple_udpv6_packet(
                eth_dst=self.customer2_mac,
                eth_src=self.customer1_mac,
                ipv6_dst=self.customer2_ipv6,
                ipv6_src=self.customer1_ipv6,
                ipv6_hlim=64,
                ipv6_dscp=20)
            vxlan_pkt_ipv6_pipe = Mask(
                simple_vxlan_packet(
                    eth_dst=self.unbor1_mac,
                    eth_src=ROUTER_MAC,
                    ip_dst=self.tun1_ip,
                    ip_src=self.lpb_ip,
                    ip_id=0,
                    ip_ttl=64,
                    ip_dscp=0,
                    ip_flags=0x2,
                    with_udp_chksum=False,
                    vxlan_vni=self.vni1,
                    inner_frame=inner_pkt_ipv6))
            vxlan_pkt_ipv6_pipe.set_do_not_care_scapy(UDP, 'sport')

            print("Sending packet from port %d -> VNI %d (IPv6)" %
                  (self.oport1_dev, self.vni1))
            send_packet(self, self.oport1_dev, inner_pkt_ipv6)
            verify_packet(self, vxlan_pkt_ipv6_pipe, self.uport1_dev)
            print("Packet was L2 forwarded into the VXLAN tunnel (IPv6)")

            print("Change default encap_dscp_val")
            sai_thrift_set_tunnel_attribute(
                self.client, self.tunnel1, encap_dscp_val=12)

            vxlan_pkt_pipe2 = Mask(
                simple_vxlan_packet(
                    eth_dst=self.unbor1_mac,
                    eth_src=ROUTER_MAC,
                    ip_dst=self.tun1_ip,
                    ip_src=self.lpb_ip,
                    ip_id=0,
                    ip_ttl=64,
                    ip_dscp=12,
                    ip_flags=0x2,
                    with_udp_chksum=False,
                    vxlan_vni=self.vni1,
                    inner_frame=inner_pkt))
            vxlan_pkt_pipe2.set_do_not_care_scapy(UDP, 'sport')

            print("Sending packet from port %d -> VNI %d" %
                  (self.oport1_dev, self.vni1))
            send_packet(self, self.oport1_dev, inner_pkt)
            verify_packet(self, vxlan_pkt_pipe2, self.uport1_dev)
            print("Packet was L2 forwarded into the VXLAN tunnel")

            vxlan_pkt_ipv6_pipe2 = Mask(
                simple_vxlan_packet(
                    eth_dst=self.unbor1_mac,
                    eth_src=ROUTER_MAC,
                    ip_dst=self.tun1_ip,
                    ip_src=self.lpb_ip,
                    ip_id=0,
                    ip_ttl=64,
                    ip_dscp=12,
                    ip_flags=0x2,
                    with_udp_chksum=False,
                    vxlan_vni=self.vni1,
                    inner_frame=inner_pkt_ipv6))
            vxlan_pkt_ipv6_pipe2.set_do_not_care_scapy(UDP, 'sport')

            print("Sending packet from port %d -> VNI %d (IPv6)" %
                  (self.oport1_dev, self.vni1))
            send_packet(self, self.oport1_dev, inner_pkt_ipv6)
            verify_packet(self, vxlan_pkt_ipv6_pipe2, self.uport1_dev)
            print("Packet was L2 forwarded into the VXLAN tunnel (IPv6)")

            print("Reset default encap_dscp_val")
            sai_thrift_set_tunnel_attribute(
                self.client, self.tunnel1, encap_dscp_val=0)

            print("setting encap_dscp_mode to UNIFORM")
            sai_thrift_set_tunnel_attribute(
                self.client, self.tunnel1,
                encap_dscp_mode=SAI_TUNNEL_DSCP_MODE_UNIFORM_MODEL)

            # L2 forwarding into VXLAN tunnel
            vxlan_pkt_uniform = Mask(
                simple_vxlan_packet(
                    eth_dst=self.unbor1_mac,
                    eth_src=ROUTER_MAC,
                    ip_dst=self.tun1_ip,
                    ip_src=self.lpb_ip,
                    ip_id=0,
                    ip_ttl=64,
                    ip_dscp=20,
                    ip_flags=0x2,
                    with_udp_chksum=False,
                    vxlan_vni=self.vni1,
                    inner_frame=inner_pkt))
            vxlan_pkt_uniform.set_do_not_care_scapy(UDP, 'sport')

            print("Sending packet from port %d -> VNI %d" %
                  (self.oport1_dev, self.vni1))
            send_packet(self, self.oport1_dev, inner_pkt)
            verify_packet(self, vxlan_pkt_uniform, self.uport1_dev)
            print("Packet was L2 forwarded into the VXLAN tunnel")

            # L2 forwarding into VXLAN tunnel IPv6
            vxlan_pkt_ipv6_uniform = Mask(
                simple_vxlan_packet(
                    eth_dst=self.unbor1_mac,
                    eth_src=ROUTER_MAC,
                    ip_dst=self.tun1_ip,
                    ip_src=self.lpb_ip,
                    ip_id=0,
                    ip_ttl=64,
                    ip_dscp=20,
                    ip_flags=0x2,
                    with_udp_chksum=False,
                    vxlan_vni=self.vni1,
                    inner_frame=inner_pkt_ipv6))
            vxlan_pkt_ipv6_uniform.set_do_not_care_scapy(UDP, 'sport')

            print("Sending packet from port %d -> VNI %d (IPv6)" %
                  (self.oport1_dev, self.vni1))
            send_packet(self, self.oport1_dev, inner_pkt_ipv6)
            verify_packet(self, vxlan_pkt_ipv6_uniform, self.uport1_dev)
            print("Packet was L2 forwarded into the VXLAN tunnel (IPv6)")

            print("setting encap_dscp_mode to PIPE")
            sai_thrift_set_tunnel_attribute(
                self.client, self.tunnel1,
                encap_dscp_mode=SAI_TUNNEL_DSCP_MODE_PIPE_MODEL)

            print("Sending packet from port %d -> VNI %d" %
                  (self.oport1_dev, self.vni1))
            send_packet(self, self.oport1_dev, inner_pkt)
            verify_packet(self, vxlan_pkt_pipe, self.uport1_dev)
            print("Packet was L2 forwarded into the VXLAN tunnel")

            print("Sending packet from port %d -> VNI %d (IPv6)" %
                  (self.oport1_dev, self.vni1))
            send_packet(self, self.oport1_dev, inner_pkt_ipv6)
            verify_packet(self, vxlan_pkt_ipv6_pipe, self.uport1_dev)
            print("Packet was L2 forwarded into the VXLAN tunnel (IPv6)")

        finally:
            sai_thrift_set_tunnel_attribute(
                self.client, self.tunnel1,
                encap_dscp_mode=SAI_TUNNEL_DSCP_MODE_UNIFORM_MODEL)

    def l2VxLanDecapChangingDscpTest(self):
        '''
        This test verifies correct outer DSCP value for IPv4 and IPv6
        inner packet with L2 VxLAN decapsulation in case of changing
        DSCP mode.
        '''
        print("\nl2VxLanDecapChangingDscpTest()")

        try:
            print("setting decap_dscp_mode to PIPE")
            sai_thrift_set_tunnel_attribute(
                self.client, self.tunnel1,
                decap_dscp_mode=SAI_TUNNEL_DSCP_MODE_PIPE_MODEL)

            # L2 decap into vlan
            pkt_ipv4 = simple_udp_packet(
                eth_dst=self.customer1_mac,
                eth_src=self.customer2_mac,
                ip_dst=self.customer1_ip,
                ip_src=self.customer2_ip,
                ip_id=108,
                ip_ttl=60,
                ip_dscp=10)
            vxlan_pkt = simple_vxlan_packet(
                eth_dst=ROUTER_MAC,
                eth_src=self.unbor1_mac,
                ip_dst=self.lpb_ip,
                ip_src=self.tun1_ip,
                ip_id=0,
                ip_ttl=64,
                ip_dscp=20,
                ip_flags=0x2,
                with_udp_chksum=False,
                vxlan_vni=self.vni1,
                inner_frame=pkt_ipv4)

            exp_pkt_pipe = simple_udp_packet(
                eth_dst=self.customer1_mac,
                eth_src=self.customer2_mac,
                ip_dst=self.customer1_ip,
                ip_src=self.customer2_ip,
                ip_id=108,
                ip_ttl=64,
                ip_dscp=10)

            print("Sending packet from VNI %d -> port %d" %
                  (self.vni1, self.oport1_dev))
            send_packet(self, self.uport1_dev, vxlan_pkt)
            verify_packet(self, exp_pkt_pipe, self.oport1_dev)
            print("Packet was L2 forwarded out of the VXLAN tunnel")

            # L2 decap into vlan IPv6
            pkt_ipv6 = simple_udpv6_packet(
                eth_dst=self.customer1_mac,
                eth_src=self.customer2_mac,
                ipv6_dst=self.customer1_ipv6,
                ipv6_src=self.customer2_ipv6,
                ipv6_hlim=60,
                ipv6_dscp=10)
            vxlan_pkt_ipv6 = simple_vxlan_packet(
                eth_dst=ROUTER_MAC,
                eth_src=self.unbor1_mac,
                ip_dst=self.lpb_ip,
                ip_src=self.tun1_ip,
                ip_id=0,
                ip_ttl=64,
                ip_dscp=20,
                ip_flags=0x2,
                with_udp_chksum=False,
                vxlan_vni=self.vni1,
                inner_frame=pkt_ipv6)

            exp_pkt_ipv6_pipe = simple_udpv6_packet(
                eth_dst=self.customer1_mac,
                eth_src=self.customer2_mac,
                ipv6_dst=self.customer1_ipv6,
                ipv6_src=self.customer2_ipv6,
                ipv6_hlim=64,
                ipv6_dscp=10)

            print("Sending packet from VNI %d -> port %d (IPv6)" %
                  (self.vni1, self.oport1_dev))
            send_packet(self, self.uport1_dev, vxlan_pkt_ipv6)
            verify_packet(self, exp_pkt_ipv6_pipe, self.oport1_dev)
            print("Packet was L2 forwarded out of the VXLAN tunnel (IPv6)")

            print("setting decap_dscp_mode to UNIFORM")
            sai_thrift_set_tunnel_attribute(
                self.client, self.tunnel1,
                decap_dscp_mode=SAI_TUNNEL_DSCP_MODE_UNIFORM_MODEL)

            # L2 decap into vlan
            exp_pkt_uniform = simple_udp_packet(
                eth_dst=self.customer1_mac,
                eth_src=self.customer2_mac,
                ip_dst=self.customer1_ip,
                ip_src=self.customer2_ip,
                ip_id=108,
                ip_ttl=64,
                ip_dscp=20)

            print("Sending packet from VNI %d -> port %d" %
                  (self.vni1, self.oport1_dev))
            send_packet(self, self.uport1_dev, vxlan_pkt)
            verify_packet(self, exp_pkt_uniform, self.oport1_dev)
            print("Packet was L2 forwarded out of the VXLAN tunnel")
            print("setting decap_ttl_mode to UNIFORM")

            # L2 decap into vlan IPv6
            exp_pkt_ipv6_uniform = simple_udpv6_packet(
                eth_dst=self.customer1_mac,
                eth_src=self.customer2_mac,
                ipv6_dst=self.customer1_ipv6,
                ipv6_src=self.customer2_ipv6,
                ipv6_hlim=64,
                ipv6_dscp=20)

            print("Sending packet from VNI %d -> port %d (IPv6)" %
                  (self.vni1, self.oport1_dev))
            send_packet(self, self.uport1_dev, vxlan_pkt_ipv6)
            verify_packet(self, exp_pkt_ipv6_uniform, self.oport1_dev)
            print("Packet was L2 forwarded out of the VXLAN tunnel (IPv6)")

            print("setting decap_dscp_mode to PIPE")
            sai_thrift_set_tunnel_attribute(
                self.client, self.tunnel1,
                decap_dscp_mode=SAI_TUNNEL_DSCP_MODE_PIPE_MODEL)

            print("Sending packet from VNI %d -> port %d" %
                  (self.vni1, self.oport1_dev))
            send_packet(self, self.uport1_dev, vxlan_pkt)
            verify_packet(self, exp_pkt_pipe, self.oport1_dev)
            print("Packet was L2 forwarded out of the VXLAN tunnel")

            print("Sending packet from VNI %d -> port %d (IPv6)" %
                  (self.vni1, self.oport1_dev))
            send_packet(self, self.uport1_dev, vxlan_pkt_ipv6)
            verify_packet(self, exp_pkt_ipv6_pipe, self.oport1_dev)
            print("Packet was L2 forwarded out of the VXLAN tunnel (IPv6)")

        finally:
            sai_thrift_set_tunnel_attribute(
                self.client, self.tunnel1,
                decap_dscp_mode=SAI_TUNNEL_DSCP_MODE_UNIFORM_MODEL)


@group('l2-vxlan')
class L2VxLanNonIpHopLimitTest(SaiHelper):
    '''
    This class contains tests Hop limit for L2 VxLAN Non-IP traffic
    '''

    def setUp(self):
        super(L2VxLanNonIpHopLimitTest, self).setUp()

        self.oport1 = self.port0
        self.oport1_dev = self.dev_port0
        self.oport1_bp = self.port0_bp
        self.uport1 = self.port10
        self.uport1_dev = self.dev_port10
        self.uport1_rif = self.port10_rif

        self.vni1 = 1000
        self.tun1_ip = "10.0.0.65"
        self.lpb_ip = "10.1.0.32"
        self.tun_ip_mask = "/32"
        self.uport1_myip = "10.0.0.64"
        self.uport_mask = "/31"
        self.customer1_ip = "192.168.100.1"
        self.customer1_ipv6 = "2001:09db8::100:2"
        self.customer2_ip = "192.168.100.8"
        self.customer2_ipv6 = "2001:0db8::100:9"
        self.customer1_mac = "00:22:22:22:22:11"
        self.customer2_mac = "00:22:22:22:22:22"
        self.unbor1_mac = "00:33:33:33:33:11"

        # underlay loopback rif for tunnels
        self.urif_lpb = sai_thrift_create_router_interface(
            self.client,
            type=SAI_ROUTER_INTERFACE_TYPE_LOOPBACK,
            virtual_router_id=self.default_vrf,
            mtu=9100)

        self.uport1_prefix_route = sai_thrift_route_entry_t(
            destination=sai_ipprefix(self.uport1_myip + self.uport_mask),
            switch_id=self.switch_id,
            vr_id=self.default_vrf)
        sai_thrift_create_route_entry(self.client,
                                      self.uport1_prefix_route,
                                      next_hop_id=self.uport1_rif)

        # underlay neighbor
        self.unbor = sai_thrift_neighbor_entry_t(
            ip_address=sai_ipaddress(self.tun1_ip),
            rif_id=self.uport1_rif,
            switch_id=self.switch_id)
        sai_thrift_create_neighbor_entry(self.client,
                                         self.unbor,
                                         dst_mac_address=self.unbor1_mac)

        # underlay nexthop
        self.unhop = sai_thrift_create_next_hop(
            self.client,
            type=SAI_NEXT_HOP_TYPE_IP,
            ip=sai_ipaddress(self.tun1_ip),
            router_interface_id=self.uport1_rif)

        # create fdb entry customer 1 on port0, vlan 10
        self.customer1_fdb_entry = sai_thrift_fdb_entry_t(
            bv_id=self.vlan10,
            mac_address=self.customer1_mac,
            switch_id=self.switch_id)
        sai_thrift_create_fdb_entry(self.client,
                                    self.customer1_fdb_entry,
                                    type=SAI_FDB_ENTRY_TYPE_STATIC,
                                    bridge_port_id=self.oport1_bp)

        # L2 VxLAN configuration below
        tunnel_type = SAI_TUNNEL_TYPE_VXLAN
        peer_mode = SAI_TUNNEL_PEER_MODE_P2MP
        term_type = SAI_TUNNEL_TERM_TABLE_ENTRY_TYPE_P2MP

        # create tunnel maps
        self.decap_tunnel_map_vlan = sai_thrift_create_tunnel_map(
            self.client, type=SAI_TUNNEL_MAP_TYPE_VNI_TO_VLAN_ID)

        self.encap_tunnel_map_vlan = sai_thrift_create_tunnel_map(
            self.client, type=SAI_TUNNEL_MAP_TYPE_VLAN_ID_TO_VNI)

        # create tunnel
        decap_maps = sai_thrift_object_list_t(
            count=1,
            idlist=[self.decap_tunnel_map_vlan])
        encap_maps = sai_thrift_object_list_t(
            count=1,
            idlist=[self.encap_tunnel_map_vlan])

        ttl_mode = SAI_TUNNEL_TTL_MODE_UNIFORM_MODEL
        self.tunnel1 = sai_thrift_create_tunnel(
            self.client,
            type=tunnel_type,
            underlay_interface=self.urif_lpb,
            decap_mappers=decap_maps,
            encap_mappers=encap_maps,
            encap_ttl_mode=ttl_mode,
            encap_src_ip=sai_ipaddress(self.lpb_ip),
            peer_mode=peer_mode)

        self.tunnel_term1 = sai_thrift_create_tunnel_term_table_entry(
            self.client,
            type=term_type,
            vr_id=self.default_vrf,
            dst_ip=sai_ipaddress(self.lpb_ip),
            tunnel_type=tunnel_type,
            action_tunnel_id=self.tunnel1)

        # create tunnel map entries
        self.decap_tunnel_map_entry_vlan = sai_thrift_create_tunnel_map_entry(
            self.client,
            tunnel_map_type=SAI_TUNNEL_MAP_TYPE_VNI_TO_VLAN_ID,
            tunnel_map=self.decap_tunnel_map_vlan,
            vlan_id_value=10,
            vni_id_key=self.vni1)

        self.encap_tunnel_map_entry_vlan = sai_thrift_create_tunnel_map_entry(
            self.client,
            tunnel_map_type=SAI_TUNNEL_MAP_TYPE_VLAN_ID_TO_VNI,
            tunnel_map=self.encap_tunnel_map_vlan,
            vlan_id_key=10,
            vni_id_value=self.vni1)

        # p2p tunnel is created when the first evpn type 3 route
        # is received from a remote vtep
        self.tunnel2 = sai_thrift_create_tunnel(
            self.client,
            type=tunnel_type,
            underlay_interface=self.urif_lpb,
            decap_mappers=decap_maps,
            encap_mappers=encap_maps,
            encap_src_ip=sai_ipaddress(self.lpb_ip),
            peer_mode=SAI_TUNNEL_PEER_MODE_P2P,
            encap_dst_ip=sai_ipaddress(self.tun1_ip))

        # bridge port is created on p2p tunnel
        self.tun2_bp = sai_thrift_create_bridge_port(
            self.client,
            type=SAI_BRIDGE_PORT_TYPE_TUNNEL,
            tunnel_id=self.tunnel2,
            bridge_id=self.default_1q_bridge,
            admin_state=True,
            fdb_learning_mode=SAI_BRIDGE_PORT_FDB_LEARNING_MODE_DISABLE)

        # vlan_member is created using tunnel bridge_port
        self.tun2_vlan_member = sai_thrift_create_vlan_member(
            self.client,
            vlan_id=self.vlan10,
            bridge_port_id=self.tun2_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)

        # fdb entry for L2 forwarding into VXLAN tunnel
        fdb_entry = sai_thrift_fdb_entry_t(
            bv_id=self.vlan10,
            mac_address=self.customer2_mac,
            switch_id=self.switch_id)
        sai_thrift_create_fdb_entry(self.client,
                                    fdb_entry,
                                    type=SAI_FDB_ENTRY_TYPE_STATIC,
                                    bridge_port_id=self.tun2_bp)

    def runTest(self):
        try:
            self.l2VxLanNonIpEncapTTLDscpTest()
            self.l2VxLanNonIpDecapTTLDscpTest()
        finally:
            pass

    def tearDown(self):
        sai_thrift_flush_fdb_entries(
            self.client, entry_type=SAI_FDB_FLUSH_ENTRY_TYPE_ALL)

        sai_thrift_remove_vlan_member(self.client, self.tun2_vlan_member)
        sai_thrift_remove_bridge_port(self.client, self.tun2_bp)
        sai_thrift_remove_tunnel(self.client, self.tunnel2)
        sai_thrift_remove_tunnel_map_entry(
            self.client, self.decap_tunnel_map_entry_vlan)
        sai_thrift_remove_tunnel_map_entry(
            self.client, self.encap_tunnel_map_entry_vlan)
        sai_thrift_remove_tunnel_term_table_entry(
            self.client, self.tunnel_term1)
        sai_thrift_remove_tunnel(self.client, self.tunnel1)
        sai_thrift_remove_tunnel_map(self.client, self.encap_tunnel_map_vlan)
        sai_thrift_remove_tunnel_map(self.client, self.decap_tunnel_map_vlan)

        sai_thrift_remove_next_hop(self.client, self.unhop)
        sai_thrift_remove_neighbor_entry(self.client, self.unbor)
        sai_thrift_remove_route_entry(self.client, self.uport1_prefix_route)
        sai_thrift_remove_router_interface(self.client, self.urif_lpb)

        super(L2VxLanNonIpHopLimitTest, self).tearDown()

    def l2VxLanNonIpEncapTTLDscpTest(self):
        '''
        This test verifies that packets are properly use
        TTL and DSCP in pipe mode
        '''
        print("\nl2VxLanNonIpEncapTTLDscpTest()")

        # L2 forwarding into VXLAN tunnel

        eth_type = 0x0806
        pkt = simple_eth_packet(eth_dst=self.customer2_mac,
                                eth_src=self.customer1_mac,
                                eth_type=eth_type,
                                pktlen=100)

        vxlan_pkt = Mask(
            simple_vxlan_packet(eth_dst=self.unbor1_mac,
                                eth_src=ROUTER_MAC,
                                ip_dst=self.tun1_ip,
                                ip_src=self.lpb_ip,
                                ip_id=0,
                                ip_ttl=64,
                                ip_flags=0x2,
                                with_udp_chksum=False,
                                vxlan_vni=self.vni1,
                                inner_frame=pkt))
        vxlan_pkt.set_do_not_care_scapy(UDP, 'sport')
        vxlan_pkt.set_do_not_care_scapy(UDP, "len")
        vxlan_pkt.set_do_not_care_scapy(ptf.packet.IP, "len")
        vxlan_pkt.set_do_not_care_scapy(ptf.packet.IP, "chksum")

        print("Sending packet from port %d -> VNI %d" %
              (self.oport1_dev, self.vni1))
        send_packet(self, self.oport1_dev, pkt)
        verify_packet(self, vxlan_pkt, self.uport1_dev)
        print("Packet was L2 forwarded into the VXLAN tunnel")

        print("Modifying ttl_mode to pipe")
        ttl_mode = SAI_TUNNEL_TTL_MODE_PIPE_MODEL
        sai_thrift_set_tunnel_attribute(self.client,
                                        self.tunnel1,
                                        encap_ttl_mode=ttl_mode)

        ttl_val = 32
        print("Modifying encap TTL value to: {}".format(ttl_val))
        sai_thrift_set_tunnel_attribute(self.client,
                                        self.tunnel1,
                                        encap_ttl_val=ttl_val)

        vxlan_pkt_pipe = Mask(
            simple_vxlan_packet(eth_dst=self.unbor1_mac,
                                eth_src=ROUTER_MAC,
                                ip_dst=self.tun1_ip,
                                ip_src=self.lpb_ip,
                                ip_id=0,
                                ip_ttl=ttl_val,
                                ip_flags=0x2,
                                with_udp_chksum=False,
                                vxlan_vni=self.vni1,
                                inner_frame=pkt))
        vxlan_pkt_pipe.set_do_not_care_scapy(UDP, 'sport')
        vxlan_pkt_pipe.set_do_not_care_scapy(UDP, "len")
        vxlan_pkt_pipe.set_do_not_care_scapy(ptf.packet.IP, "len")
        vxlan_pkt_pipe.set_do_not_care_scapy(ptf.packet.IP, "chksum")

        print("Sending packet from port %d -> VNI %d in pipe mode" %
              (self.oport1_dev, self.vni1))
        send_packet(self, self.oport1_dev, pkt)
        verify_packet(self, vxlan_pkt_pipe, self.uport1_dev)
        print("Packet received with ttl value: {}".format(ttl_val))

    def l2VxLanNonIpDecapTTLDscpTest(self):
        '''
        This test verifies that Non-IP packets are properly
        decapsulated in case of L2 VxLAN operations on regular port
        '''
        print("\nl2VxLanNonIpDecapTTLDscpTest()")

        #  L2 decap from vxlan port
        eth_type = 0x0806
        pkt = simple_eth_packet(eth_dst=self.customer1_mac,
                                eth_src=self.customer2_mac,
                                eth_type=eth_type,
                                pktlen=100)

        vxlan_pkt = simple_vxlan_packet(eth_dst=ROUTER_MAC,
                                        eth_src=self.unbor1_mac,
                                        ip_dst=self.lpb_ip,
                                        ip_src=self.tun1_ip,
                                        ip_id=0,
                                        ip_ttl=64,
                                        ip_flags=0x2,
                                        with_udp_chksum=False,
                                        vxlan_vni=self.vni1,
                                        inner_frame=pkt)

        print("Sending packet from VNI %d -> port %d " %
              (self.vni1, self.oport1_dev))
        send_packet(self, self.uport1_dev, vxlan_pkt)
        verify_packet(self, pkt, self.oport1_dev)
        print("Packet was L2 forwarded from the VXLAN tunnel")


@group('l2-vxlan')
class L2VxLanCoExistenceTest(SaiHelper):
    '''
    This class contains tests for L2 and L3 VxLAN tunnels co-existence with
    different orders of objects creation and deletion.

    Note: this test does not verify correct behaviour of ttl decrementation as
    there are some profiles that support tunnels but don't support ttl_mode.
    In these profiles ttl does not get decremented but is being set to default
    value of 64. That's why some packets are being sent with ttl being equal
    to 65 to support both cases - when ttl_mode is and is not supported.
    '''

    def setUp(self):
        super(L2VxLanCoExistenceTest, self).setUp()

        self.tun_ip = "10.0.0.65"
        self.lpb_ip = "10.1.1.10"
        self.uport_ip = "10.0.0.64"
        self.customer1_ip = "192.168.100.1"
        self.customer2_ip = "192.168.100.8"
        self.customer3_ip = "192.168.200.1"
        self.vlan10_ip = "192.168.100.254"
        self.inner_dmac = "00:11:11:11:11:11"
        self.customer1_mac = "00:22:22:22:22:11"
        self.customer2_mac = "00:22:22:22:22:22"
        self.unbor_mac = "00:33:33:33:33:11"
        self.vni1 = 1000
        self.vni2 = 2000

        self.oport = self.port0
        self.oport_dev = self.dev_port0
        self.oport_bp = self.port0_bp
        self.uport = self.port10
        self.uport_dev = self.dev_port10
        self.uport_rif = self.port10_rif

        # underlay loopback rif for tunnels
        self.urif_lpb = sai_thrift_create_router_interface(
            self.client,
            type=SAI_ROUTER_INTERFACE_TYPE_LOOPBACK,
            virtual_router_id=self.default_vrf,
            mtu=9100)

        self.uport_prefix_route = sai_thrift_route_entry_t(
            destination=sai_ipprefix(self.uport_ip + '/31'),
            switch_id=self.switch_id,
            vr_id=self.default_vrf)
        sai_thrift_create_route_entry(self.client,
                                      self.uport_prefix_route,
                                      next_hop_id=self.uport_rif)

        # underlay neighbor
        self.unbor = sai_thrift_neighbor_entry_t(
            ip_address=sai_ipaddress(self.tun_ip),
            rif_id=self.uport_rif,
            switch_id=self.switch_id)
        sai_thrift_create_neighbor_entry(self.client,
                                         self.unbor,
                                         dst_mac_address=self.unbor_mac)

        # underlay nexthop
        self.unhop = sai_thrift_create_next_hop(
            self.client,
            type=SAI_NEXT_HOP_TYPE_IP,
            ip=sai_ipaddress(self.tun_ip),
            router_interface_id=self.uport_rif)

        # overlay vrf
        self.ovrf = sai_thrift_create_virtual_router(self.client)

        self.vlan10_rif = sai_thrift_create_router_interface(
            self.client,
            virtual_router_id=self.ovrf,
            src_mac_address=ROUTER_MAC,
            type=SAI_ROUTER_INTERFACE_TYPE_VLAN,
            vlan_id=self.vlan10,
            mtu=9100,
            nat_zone_id=0)

        self.vlan10_prefix_route = sai_thrift_route_entry_t(
            destination=sai_ipprefix(self.vlan10_ip + '/24'),
            switch_id=self.switch_id,
            vr_id=self.ovrf)
        sai_thrift_create_route_entry(self.client,
                                      self.vlan10_prefix_route,
                                      next_hop_id=self.vlan10_rif)

        # fdb entry, neighbor, nexthop for customer 1 on port0, vlan 10
        self.customer1_fdb_entry = sai_thrift_fdb_entry_t(
            bv_id=self.vlan10,
            mac_address=self.customer1_mac,
            switch_id=self.switch_id)
        sai_thrift_create_fdb_entry(self.client,
                                    self.customer1_fdb_entry,
                                    type=SAI_FDB_ENTRY_TYPE_DYNAMIC,
                                    bridge_port_id=self.oport_bp)

        self.customer1_nbor = sai_thrift_neighbor_entry_t(
            ip_address=sai_ipaddress(self.customer1_ip),
            rif_id=self.vlan10_rif,
            switch_id=self.switch_id)
        sai_thrift_create_neighbor_entry(self.client,
                                         self.customer1_nbor,
                                         dst_mac_address=self.customer1_mac)

        self.customer1_nhop = sai_thrift_create_next_hop(
            self.client,
            type=SAI_NEXT_HOP_TYPE_IP,
            ip=sai_ipaddress(self.customer1_ip),
            router_interface_id=self.vlan10_rif)

        # tunnel configuration
        self.tunnel_type = SAI_TUNNEL_TYPE_VXLAN
        peer_mode = SAI_TUNNEL_PEER_MODE_P2MP
        term_type = SAI_TUNNEL_TERM_TABLE_ENTRY_TYPE_P2MP

        self.decap_tunnel_map_vrf = sai_thrift_create_tunnel_map(
            self.client, type=SAI_TUNNEL_MAP_TYPE_VNI_TO_VIRTUAL_ROUTER_ID)

        self.encap_tunnel_map_vrf = sai_thrift_create_tunnel_map(
            self.client, type=SAI_TUNNEL_MAP_TYPE_VIRTUAL_ROUTER_ID_TO_VNI)

        self.decap_tunnel_map_vlan = sai_thrift_create_tunnel_map(
            self.client, type=SAI_TUNNEL_MAP_TYPE_VNI_TO_VLAN_ID)

        self.encap_tunnel_map_vlan = sai_thrift_create_tunnel_map(
            self.client, type=SAI_TUNNEL_MAP_TYPE_VLAN_ID_TO_VNI)

        self.decap_maps = sai_thrift_object_list_t(
            count=2,
            idlist=[self.decap_tunnel_map_vlan, self.decap_tunnel_map_vrf])
        self.encap_maps = sai_thrift_object_list_t(
            count=2,
            idlist=[self.encap_tunnel_map_vlan, self.encap_tunnel_map_vrf])

        self.p2mp_tunnel = sai_thrift_create_tunnel(
            self.client,
            type=self.tunnel_type,
            underlay_interface=self.urif_lpb,
            decap_mappers=self.decap_maps,
            encap_mappers=self.encap_maps,
            encap_src_ip=sai_ipaddress(self.lpb_ip),
            peer_mode=peer_mode)

        self.tunnel_term = sai_thrift_create_tunnel_term_table_entry(
            self.client,
            type=term_type,
            vr_id=self.default_vrf,
            dst_ip=sai_ipaddress(self.lpb_ip),
            tunnel_type=self.tunnel_type,
            action_tunnel_id=self.p2mp_tunnel)

        self.encap_tunnel_map_entry_vrf = sai_thrift_create_tunnel_map_entry(
            self.client,
            tunnel_map_type=SAI_TUNNEL_MAP_TYPE_VIRTUAL_ROUTER_ID_TO_VNI,
            tunnel_map=self.encap_tunnel_map_vrf,
            virtual_router_id_key=self.ovrf,
            vni_id_value=self.vni2)

        self.decap_tunnel_map_entry_vrf = sai_thrift_create_tunnel_map_entry(
            self.client,
            tunnel_map_type=SAI_TUNNEL_MAP_TYPE_VNI_TO_VIRTUAL_ROUTER_ID,
            tunnel_map=self.decap_tunnel_map_vrf,
            virtual_router_id_value=self.ovrf,
            vni_id_key=self.vni2)

    def runTest(self):
        try:
            self.l2VxlanCoExistenceTest1()
            self.l2VxlanCoExistenceTest2()
            self.l2VxlanCoExistenceTest3()
            self.l2VxlanCoExistenceTest4()
            self.l2VxlanCoExistenceFloodTest1()
            self.l2VxlanCoExistenceFloodTest2()
            self.l2VxlanCoExistenceFloodTest3()
            self.l2VxlanCoExistenceFloodTest4()
        finally:
            pass

    def tearDown(self):
        sai_thrift_flush_fdb_entries(
            self.client, entry_type=SAI_FDB_FLUSH_ENTRY_TYPE_ALL)
        sai_thrift_remove_tunnel_map_entry(
            self.client, self.decap_tunnel_map_entry_vrf)
        sai_thrift_remove_tunnel_map_entry(
            self.client, self.encap_tunnel_map_entry_vrf)
        sai_thrift_remove_tunnel_term_table_entry(
            self.client, self.tunnel_term)
        sai_thrift_remove_tunnel(self.client, self.p2mp_tunnel)
        sai_thrift_remove_tunnel_map(
            self.client, self.encap_tunnel_map_vlan)
        sai_thrift_remove_tunnel_map(
            self.client, self.decap_tunnel_map_vlan)
        sai_thrift_remove_tunnel_map(
            self.client, self.encap_tunnel_map_vrf)
        sai_thrift_remove_tunnel_map(self.client, self.decap_tunnel_map_vrf)
        sai_thrift_remove_next_hop(self.client, self.customer1_nhop)
        sai_thrift_remove_neighbor_entry(self.client, self.customer1_nbor)
        sai_thrift_remove_route_entry(self.client, self.vlan10_prefix_route)
        sai_thrift_remove_router_interface(self.client, self.vlan10_rif)
        sai_thrift_remove_virtual_router(self.client, self.ovrf)
        sai_thrift_remove_next_hop(self.client, self.unhop)
        sai_thrift_remove_neighbor_entry(self.client, self.unbor)
        sai_thrift_remove_route_entry(self.client, self.uport_prefix_route)
        sai_thrift_remove_router_interface(self.client, self.urif_lpb)

        super(L2VxLanCoExistenceTest, self).tearDown()

    # it is intentional to define following objects inside the function below
    # noqa pylint: disable=attribute-defined-outside-init
    def _createP2PTunnel(self, create_fdb=True):
        '''
        Helper function to create P2P tunnel for L2 VxLAN

        Args:
            create_fdb (bool): True if FDB entry for tunnel is to be created
        '''

        self.decap_tunnel_map_entry_vlan = sai_thrift_create_tunnel_map_entry(
            self.client,
            tunnel_map_type=SAI_TUNNEL_MAP_TYPE_VNI_TO_VLAN_ID,
            tunnel_map=self.decap_tunnel_map_vlan,
            vlan_id_value=10,
            vni_id_key=self.vni1)

        self.encap_tunnel_map_entry_vlan = sai_thrift_create_tunnel_map_entry(
            self.client,
            tunnel_map_type=SAI_TUNNEL_MAP_TYPE_VLAN_ID_TO_VNI,
            tunnel_map=self.encap_tunnel_map_vlan,
            vlan_id_key=10,
            vni_id_value=self.vni1)

        self.p2p_tunnel = sai_thrift_create_tunnel(
            self.client,
            type=self.tunnel_type,
            underlay_interface=self.urif_lpb,
            decap_mappers=self.decap_maps,
            encap_mappers=self.encap_maps,
            encap_src_ip=sai_ipaddress(self.lpb_ip),
            peer_mode=SAI_TUNNEL_PEER_MODE_P2P,
            encap_dst_ip=sai_ipaddress(self.tun_ip))

        self.p2p_tunnel_bp = sai_thrift_create_bridge_port(
            self.client,
            type=SAI_BRIDGE_PORT_TYPE_TUNNEL,
            tunnel_id=self.p2p_tunnel,
            bridge_id=self.default_1q_bridge,
            admin_state=True,
            fdb_learning_mode=SAI_BRIDGE_PORT_FDB_LEARNING_MODE_DISABLE)

        self.p2p_tun_vlan_member = sai_thrift_create_vlan_member(
            self.client,
            vlan_id=self.vlan10,
            bridge_port_id=self.p2p_tunnel_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)

        if create_fdb:
            self.fdb_entry = sai_thrift_fdb_entry_t(
                bv_id=self.vlan10,
                mac_address=self.customer2_mac,
                switch_id=self.switch_id)
            sai_thrift_create_fdb_entry(self.client,
                                        self.fdb_entry,
                                        type=SAI_FDB_ENTRY_TYPE_STATIC,
                                        bridge_port_id=self.p2p_tunnel_bp)

        print("P2P Tunnel configured")

    def _createL3TunnelNhop(self):
        '''
        Helper function to create L3 tunnel nexthop
        '''
        self.tunnel_nhop = sai_thrift_create_next_hop(
            self.client,
            type=SAI_NEXT_HOP_TYPE_TUNNEL_ENCAP,
            ip=sai_ipaddress(self.tun_ip),
            tunnel_id=self.p2mp_tunnel,
            tunnel_vni=self.vni2,
            tunnel_mac=self.inner_dmac)

        self.customer3_route = sai_thrift_route_entry_t(
            destination=sai_ipprefix(self.customer3_ip + '/24'),
            switch_id=self.switch_id,
            vr_id=self.ovrf)
        sai_thrift_create_route_entry(self.client,
                                      self.customer3_route,
                                      next_hop_id=self.tunnel_nhop)

        print("Tunnel nexthop configured")

    def _removeP2PTunnel(self, remove_fdb=True):
        '''
        Helper function to remove P2P tunnel

        Args:
            remove_fdb (bool): True if FDB entry for tunnel is to be removed
        '''
        if remove_fdb:
            sai_thrift_remove_fdb_entry(self.client, self.fdb_entry)
        sai_thrift_remove_vlan_member(self.client, self.p2p_tun_vlan_member)
        sai_thrift_remove_bridge_port(self.client, self.p2p_tunnel_bp)
        sai_thrift_remove_tunnel(self.client, self.p2p_tunnel)
        sai_thrift_remove_tunnel_map_entry(
            self.client, self.encap_tunnel_map_entry_vlan)
        sai_thrift_remove_tunnel_map_entry(
            self.client, self.decap_tunnel_map_entry_vlan)

        print("P2P Tunnel removed")

    def _removeL3TunnelNhop(self):
        '''
        Helper function to remove L3 tunnel nexthop
        '''
        sai_thrift_remove_route_entry(self.client, self.customer3_route)
        sai_thrift_remove_next_hop(self.client, self.tunnel_nhop)

        print("Tunnel nexthop removed")

    def _l2VxlanTrafficTest(self, unicast=True, vxlan_flood=False):
        '''
        Helper function to verify L2 VxLAN packets forwarding

        Args:
            unicast (bool): True if only unicast packet should be L2 forwarded
            vxlan_flood (bool): True if packet should be flooded into all VLAN
                                ports and tunnels;
                                *valid only for unicast == False
        '''
        print("l2VxlanTrafficTest()")

        unknown_mac = "00:aa:aa:aa:aa:aa"

        # L2 forwarding into VXLAN tunnel
        pkt = simple_udp_packet(eth_dst=self.customer2_mac if not vxlan_flood
                                else unknown_mac,
                                eth_src=self.customer1_mac,
                                ip_dst=self.customer2_ip,
                                ip_src=self.customer1_ip,
                                ip_id=108,
                                ip_ttl=64,
                                pktlen=100)
        vxlan_pkt = Mask(
            simple_vxlan_packet(eth_dst=self.unbor_mac,
                                eth_src=ROUTER_MAC,
                                ip_dst=self.tun_ip,
                                ip_src=self.lpb_ip,
                                ip_id=0,
                                ip_ttl=64,
                                ip_flags=0x2,
                                with_udp_chksum=False,
                                vxlan_vni=self.vni1,
                                inner_frame=pkt))
        vxlan_pkt.set_do_not_care_scapy(UDP, 'sport')

        print("Sending packet from port %d -> VNI %d" %
              (self.oport_dev, self.vni1))
        send_packet(self, self.oport_dev, pkt)
        if unicast:
            verify_packet(self, vxlan_pkt, self.uport_dev)
            print("Packet was L2 forwarded into the VXLAN tunnel")
        else:
            tag_pkt = simple_udp_packet(
                eth_dst=self.customer2_mac if not vxlan_flood else unknown_mac,
                eth_src=self.customer1_mac,
                ip_dst=self.customer2_ip,
                ip_src=self.customer1_ip,
                dl_vlan_enable=True,
                vlan_vid=10,
                ip_id=108,
                ip_ttl=64,
                pktlen=104)

            if vxlan_flood:
                # packet will be flooded to VLAN 10 ports and tunnel
                verify_each_packet_on_multiple_port_lists(
                    self, [tag_pkt, pkt, vxlan_pkt],
                    [[self.dev_port1],
                     [self.dev_port4, self.dev_port5, self.dev_port6],
                     [self.uport_dev]])
                print("Packet was flooded into VLAN 10 ports and VXLAN tunnel")
            else:
                # packet will be flooded to VLAN 10 ports
                verify_each_packet_on_multiple_port_lists(
                    self, [tag_pkt, pkt],
                    [[self.dev_port1],
                     [self.dev_port4, self.dev_port5, self.dev_port6]])
                print("Packet was flooded into VLAN 10 ports")

        # L2 decap into vlan
        if unicast or vxlan_flood:
            pkt = simple_udp_packet(
                eth_dst=self.customer1_mac if not vxlan_flood else unknown_mac,
                eth_src=self.customer2_mac,
                ip_dst=self.customer1_ip,
                ip_src=self.customer2_ip,
                ip_id=108,
                ip_ttl=64,
                pktlen=100)
            vxlan_pkt = simple_vxlan_packet(eth_dst=ROUTER_MAC,
                                            eth_src=self.unbor_mac,
                                            ip_dst=self.lpb_ip,
                                            ip_src=self.tun_ip,
                                            ip_id=0,
                                            ip_ttl=64,
                                            ip_flags=0x2,
                                            with_udp_chksum=False,
                                            vxlan_vni=self.vni1,
                                            inner_frame=pkt)

            print("Sending packet from VNI %d -> port %d" %
                  (self.vni1, self.oport_dev))
            send_packet(self, self.uport_dev, vxlan_pkt)
            if unicast:
                verify_packet(self, pkt, self.oport_dev)
                print("Packet was L2 forwarded out of the VXLAN tunnel")
            else:
                tag_pkt = simple_udp_packet(
                    eth_dst=self.customer1_mac if not vxlan_flood
                    else unknown_mac,
                    eth_src=self.customer2_mac,
                    ip_dst=self.customer1_ip,
                    ip_src=self.customer2_ip,
                    dl_vlan_enable=True,
                    vlan_vid=10,
                    ip_id=108,
                    ip_ttl=64,
                    pktlen=104)
                verify_each_packet_on_multiple_port_lists(
                    self, [pkt, tag_pkt, pkt],
                    [[self.dev_port0], [self.dev_port1],
                     [self.dev_port4, self.dev_port5, self.dev_port6]])
                print("Packet was flooded out of the VXLAN tunnel")

    def _l3VxlanTrafficTest(self, positive=True):
        '''
        Helper function to verify L3 VxLAN packets forwarding

        Args:
            positive (bool): True if packet should not be dropped
        '''
        print("l3VxlanTrafficTest()")

        # L3 forwarding into VxLAN tunnel
        pkt = simple_udp_packet(eth_dst=ROUTER_MAC,
                                eth_src=self.customer1_mac,
                                ip_dst=self.customer3_ip,
                                ip_src=self.customer1_ip,
                                ip_id=108,
                                ip_ttl=65)
        inner_pkt = simple_udp_packet(eth_dst=self.inner_dmac,
                                      eth_src=ROUTER_MAC,
                                      ip_dst=self.customer3_ip,
                                      ip_src=self.customer1_ip,
                                      ip_id=108,
                                      ip_ttl=64)
        vxlan_pkt = Mask(
            simple_vxlan_packet(eth_dst=self.unbor_mac,
                                eth_src=ROUTER_MAC,
                                ip_dst=self.tun_ip,
                                ip_src=self.lpb_ip,
                                ip_id=0,
                                ip_ttl=64,
                                ip_flags=0x2,
                                with_udp_chksum=False,
                                vxlan_vni=self.vni2,
                                inner_frame=inner_pkt))
        vxlan_pkt.set_do_not_care_scapy(UDP, 'sport')
        print("Sending packet from port %d -> VNI %d" %
              (self.oport_dev, self.vni2))
        send_packet(self, self.oport_dev, pkt)
        if positive:
            verify_packet(self, vxlan_pkt, self.uport_dev)
            print("Packet was L3 forwarded into the VXLAN tunnel")
        else:
            verify_no_other_packets(self)
            print("Packet was dropped")

        # L3 decap into vrf
        if positive:
            pkt = simple_udp_packet(eth_dst=self.customer1_mac,
                                    eth_src=ROUTER_MAC,
                                    ip_dst=self.customer1_ip,
                                    ip_src=self.customer3_ip,
                                    ip_id=108,
                                    ip_ttl=63,
                                    pktlen=100)
            inner_pkt = simple_udp_packet(eth_dst=ROUTER_MAC,
                                          eth_src=self.inner_dmac,
                                          ip_dst=self.customer1_ip,
                                          ip_src=self.customer3_ip,
                                          ip_id=108,
                                          ip_ttl=64)
            vxlan_pkt = simple_vxlan_packet(eth_dst=ROUTER_MAC,
                                            eth_src=self.unbor_mac,
                                            ip_dst=self.lpb_ip,
                                            ip_src=self.tun_ip,
                                            ip_id=0,
                                            ip_ttl=64,
                                            ip_flags=0x2,
                                            with_udp_chksum=False,
                                            vxlan_vni=self.vni2,
                                            inner_frame=inner_pkt)

            print("Sending packet from VNI %d -> port %d" %
                  (self.vni2, self.oport_dev))
            send_packet(self, self.uport_dev, vxlan_pkt)
            verify_packet(self, pkt, self.oport_dev)
            print("Packet was L3 forwarded out of the VXLAN tunnel")

    def l2VxlanCoExistenceTest1(self):
        '''
        Verify L2/L3 VxLAN co-existence with the following scenario:
            1) create P2P tunnel
            2) create L3 tunnel nexthop
            3) remove P2P tunnel
            4) remove L3 tunnel nexthop
        with traffic verification between each step
        '''
        print("\nl2VxlanCoExistenceTest1()")
        print("\t### No tunnels")
        self._l2VxlanTrafficTest(False)
        self._l3VxlanTrafficTest(False)

        print("\t### L2 only")
        self._createP2PTunnel()
        self._l2VxlanTrafficTest()
        self._l3VxlanTrafficTest(False)

        print("\t### L2 + L3")
        self._createL3TunnelNhop()
        self._l2VxlanTrafficTest()
        self._l3VxlanTrafficTest()

        print("\t### L3 only")
        self._removeP2PTunnel()
        self._l2VxlanTrafficTest(False)
        self._l3VxlanTrafficTest()

        print("\t### No tunnels")
        self._removeL3TunnelNhop()
        self._l2VxlanTrafficTest(False)
        self._l3VxlanTrafficTest(False)

    def l2VxlanCoExistenceTest2(self):
        '''
        Verify L2/L3 VxLAN co-existence with the following scenario:
            1) create P2P tunnel
            2) create L3 tunnel nexthop
            3) remove L3 tunnel nexthop
            4) remove P2P tunnel
        with traffic verification between each step
        '''
        print("\nl2VxlanCoExistenceTest2()")
        print("\t### No tunnels")
        self._l2VxlanTrafficTest(False)
        self._l3VxlanTrafficTest(False)

        print("\t### L2 only")
        self._createP2PTunnel()
        self._l2VxlanTrafficTest()
        self._l3VxlanTrafficTest(False)

        print("\t### L2 + L3")
        self._createL3TunnelNhop()
        self._l2VxlanTrafficTest()
        self._l3VxlanTrafficTest()

        print("\t### L2 only")
        self._removeL3TunnelNhop()
        self._l2VxlanTrafficTest()
        self._l3VxlanTrafficTest(False)

        print("\t### No tunnels")
        self._removeP2PTunnel()
        self._l2VxlanTrafficTest(False)
        self._l3VxlanTrafficTest(False)

    def l2VxlanCoExistenceTest3(self):
        '''
        Verify L2/L3 VxLAN co-existence with the following scenario:
            1) create L3 tunnel nexthop
            2) create P2P tunnel
            3) remove L3 tunnel nexthop
            4) remove P2P tunnel
        with traffic verification between each step
        '''
        print("\nl2VxlanCoExistenceTest3()")
        print("\t### No tunnels")
        self._l2VxlanTrafficTest(False)
        self._l3VxlanTrafficTest(False)

        print("\t### L3 only")
        self._createL3TunnelNhop()
        self._l2VxlanTrafficTest(False)
        self._l3VxlanTrafficTest()

        print("\t### L2 + L3")
        self._createP2PTunnel()
        self._l2VxlanTrafficTest()
        self._l3VxlanTrafficTest()

        print("\t### L2 only")
        self._removeL3TunnelNhop()
        self._l2VxlanTrafficTest()
        self._l3VxlanTrafficTest(False)

        print("\t### No tunnels")
        self._removeP2PTunnel()
        self._l2VxlanTrafficTest(False)
        self._l3VxlanTrafficTest(False)

    def l2VxlanCoExistenceTest4(self):
        '''
        Verify L2/L3 VxLAN co-existence with the following scenario:
            1) create L3 tunnel nexthop
            2) create P2P tunnel
            3) remove P2P tunnel
            4) remove L3 tunnel nexthop
        with traffic verification between each step
        '''
        print("\nl2VxlanCoExistenceTest4()")
        print("\t### No tunnels")
        self._l2VxlanTrafficTest(False)
        self._l3VxlanTrafficTest(False)

        print("\t### L3 only")
        self._createL3TunnelNhop()
        self._l2VxlanTrafficTest(False)
        self._l3VxlanTrafficTest()

        print("\t### L2 + L3")
        self._createP2PTunnel()
        self._l2VxlanTrafficTest()
        self._l3VxlanTrafficTest()

        print("\t### L3 only")
        self._removeP2PTunnel()
        self._l2VxlanTrafficTest(False)
        self._l3VxlanTrafficTest()

        print("\t### No tunnels")
        self._removeL3TunnelNhop()
        self._l2VxlanTrafficTest(False)
        self._l3VxlanTrafficTest(False)

    def l2VxlanCoExistenceFloodTest1(self):
        '''
        Verify L2/L3 VxLAN co-existence with the following scenario:
            1) create P2P tunnel
            2) create L3 tunnel nexthop
            3) remove P2P tunnel
            4) remove L3 tunnel nexthop
        with traffic verification between each step
        '''
        print("\nl2VxlanCoExistenceFloodTest1()")
        print("\t### No tunnels")
        self._l2VxlanTrafficTest(False)
        self._l3VxlanTrafficTest(False)

        print("\t### L2 only")
        self._createP2PTunnel(create_fdb=False)
        self._l2VxlanTrafficTest(False, vxlan_flood=True)
        self._l3VxlanTrafficTest(False)

        print("\t### L2 + L3")
        self._createL3TunnelNhop()
        self._l2VxlanTrafficTest(False, vxlan_flood=True)
        self._l3VxlanTrafficTest()

        print("\t### L3 only")
        self._removeP2PTunnel(remove_fdb=False)
        self._l2VxlanTrafficTest(False)
        self._l3VxlanTrafficTest()

        print("\t### No tunnels")
        self._removeL3TunnelNhop()
        verify_no_other_packets(self)
        self._l2VxlanTrafficTest(False)
        self._l3VxlanTrafficTest(False)

    def l2VxlanCoExistenceFloodTest2(self):
        '''
        Verify L2/L3 VxLAN co-existence with the following scenario:
            1) create P2P tunnel
            2) create L3 tunnel nexthop
            3) remove L3 tunnel nexthop
            4) remove P2P tunnel
        with traffic verification between each step
        '''
        print("\nl2VxlanCoExistenceFloodTest2()")
        print("\t### No tunnels")
        self._l2VxlanTrafficTest(False)
        self._l3VxlanTrafficTest(False)

        print("\t### L2 only")
        self._createP2PTunnel(create_fdb=False)
        self._l2VxlanTrafficTest(False, vxlan_flood=True)
        self._l3VxlanTrafficTest(False)

        print("\t### L2 + L3")
        self._createL3TunnelNhop()
        self._l2VxlanTrafficTest(False, vxlan_flood=True)
        self._l3VxlanTrafficTest()

        print("\t### L2 only")
        self._removeL3TunnelNhop()
        self._l2VxlanTrafficTest(False, vxlan_flood=True)
        self._l3VxlanTrafficTest(False)

        print("\t### No tunnels")
        self._removeP2PTunnel(remove_fdb=False)
        self._l2VxlanTrafficTest(False)
        self._l3VxlanTrafficTest(False)

    def l2VxlanCoExistenceFloodTest3(self):
        '''
        Verify L2/L3 VxLAN co-existence with the following scenario:
            1) create L3 tunnel nexthop
            2) create P2P tunnel
            3) remove L3 tunnel nexthop
            4) remove P2P tunnel
        with traffic verification between each step
        '''
        print("\nl2VxlanCoExistenceFloodTest3()")
        print("\t### No tunnels")
        self._l2VxlanTrafficTest(False)
        self._l3VxlanTrafficTest(False)

        print("\t### L3 only")
        self._createL3TunnelNhop()
        self._l2VxlanTrafficTest(False)
        self._l3VxlanTrafficTest()

        print("\t### L2 + L3")
        self._createP2PTunnel(create_fdb=False)
        self._l2VxlanTrafficTest(False, vxlan_flood=True)
        self._l3VxlanTrafficTest()

        print("\t### L2 only")
        self._removeL3TunnelNhop()
        self._l2VxlanTrafficTest(False, vxlan_flood=True)
        self._l3VxlanTrafficTest(False)

        print("\t### No tunnels")
        self._removeP2PTunnel(remove_fdb=False)
        self._l2VxlanTrafficTest(False)
        self._l3VxlanTrafficTest(False)

    def l2VxlanCoExistenceFloodTest4(self):
        '''
        Verify L2/L3 VxLAN co-existence with the following scenario:
            1) create L3 tunnel nexthop
            2) create P2P tunnel
            3) remove P2P tunnel
            4) remove L3 tunnel nexthop
        with traffic verification between each step
        '''
        print("\nl2VxlanCoExistenceFloodTest4()")
        print("\t### No tunnels")
        self._l2VxlanTrafficTest(False)
        self._l3VxlanTrafficTest(False)

        print("\t### L3 only")
        self._createL3TunnelNhop()
        self._l2VxlanTrafficTest(False)
        self._l3VxlanTrafficTest()

        print("\t### L2 + L3")
        self._createP2PTunnel(create_fdb=False)
        self._l2VxlanTrafficTest(False, vxlan_flood=True)
        self._l3VxlanTrafficTest()

        print("\t### L3 only")
        self._removeP2PTunnel(remove_fdb=False)
        self._l2VxlanTrafficTest(False)
        self._l3VxlanTrafficTest()

        print("\t### No tunnels")
        self._removeL3TunnelNhop()
        self._l2VxlanTrafficTest(False)
        self._l3VxlanTrafficTest(False)


@group('l2-vxlan')
class L2VxLanEvpnTest(SaiHelper):
    '''
    This class contains tests for L2 EVPN support
    '''

    def __init__(self):
        super(L2VxLanEvpnTest, self).__init__()

        self.tun1_ip = "10.0.0.65"
        self.tun2_ip = "10.0.0.33"
        self.tun3_ip = "10.0.0.17"
        self.lpb_ip = "10.1.0.32"
        self.tun_ip_mask = "/32"
        self.uport1_myip = "10.0.0.64"
        self.uport2_myip = "10.0.0.32"
        self.uport3_myip = "10.0.0.16"
        self.uport_mask = "/31"

    def setUp(self):
        super(L2VxLanEvpnTest, self).setUp()

        self.oport1 = self.port0
        self.oport1_dev = self.dev_port0
        self.oport1_bp = self.port0_bp
        self.uport1 = self.port10
        self.uport1_dev = self.dev_port10
        self.uport2_dev = self.dev_port11
        self.uport3_dev = self.dev_port12
        self.uport1_rif = self.port10_rif
        self.uport2_rif = self.port11_rif
        self.uport3_rif = self.port12_rif

        self.vni1 = 1000
        self.vni2 = 2000
        self.vni_for_ovrf = 5000
        self.vlan10_id = 10  # pre-configured VLAN used in tests
        self.vlan20_id = 20  # pre-configured VLAN for full SONiC configuration
        self.dummy_vlan_id = 50  # additional VLAN for full SONiC configuration
        self.vlan10_myip = "192.168.100.254"
        self.vlan10_dir_br = "192.168.100.255"
        self.vlan20_myip = "192.168.200.254"
        self.vlan20_dir_br = "192.168.200.255"
        self.customer1_ip = "192.168.100.1"
        self.customer1_ipv6 = "2001:0db8::100:2"
        self.customer2_ip = "192.168.100.8"
        self.customer2_ipv6 = "2001:0db8::100:9"
        self.customer3_ip = "192.168.250.1"
        self.customer3_ipv6 = "2001:0db8::200:2"
        self.inner_dmac = "00:11:11:11:11:11"
        self.customer1_mac = "00:22:22:22:22:11"
        self.customer2_mac = "00:22:22:22:22:22"
        self.customer3_mac = "00:22:22:22:22:33"
        self.unbor1_mac = "00:33:33:33:33:11"
        self.unbor2_mac = "00:33:33:33:33:22"
        self.unbor3_mac = "00:33:33:33:33:33"
        self.bcast_mac = "ff:ff:ff:ff:ff:ff"

        # underlay loopback rif for tunnels
        self.urif_lpb = sai_thrift_create_router_interface(
            self.client,
            type=SAI_ROUTER_INTERFACE_TYPE_LOOPBACK,
            virtual_router_id=self.default_vrf,
            mtu=9100)

        # underlay my ip address + prefix
        self.uport1_my_route = sai_thrift_route_entry_t(
            destination=sai_ipprefix(self.uport1_myip + '/32'),
            switch_id=self.switch_id,
            vr_id=self.default_vrf)
        sai_thrift_create_route_entry(self.client,
                                      self.uport1_my_route,
                                      packet_action=SAI_PACKET_ACTION_FORWARD,
                                      next_hop_id=self.cpu_port_hdl)
        self.uport2_my_route = sai_thrift_route_entry_t(
            destination=sai_ipprefix(self.uport2_myip + '/32'),
            switch_id=self.switch_id,
            vr_id=self.default_vrf)
        sai_thrift_create_route_entry(self.client,
                                      self.uport2_my_route,
                                      packet_action=SAI_PACKET_ACTION_FORWARD,
                                      next_hop_id=self.cpu_port_hdl)
        self.uport3_my_route = sai_thrift_route_entry_t(
            destination=sai_ipprefix(self.uport3_myip + '/32'),
            switch_id=self.switch_id,
            vr_id=self.default_vrf)
        sai_thrift_create_route_entry(self.client,
                                      self.uport3_my_route,
                                      packet_action=SAI_PACKET_ACTION_FORWARD,
                                      next_hop_id=self.cpu_port_hdl)

        self.uport1_prefix_route = sai_thrift_route_entry_t(
            destination=sai_ipprefix(self.uport1_myip + self.uport_mask),
            switch_id=self.switch_id,
            vr_id=self.default_vrf)
        sai_thrift_create_route_entry(self.client,
                                      self.uport1_prefix_route,
                                      next_hop_id=self.uport1_rif)

        self.uport2_prefix_route = sai_thrift_route_entry_t(
            destination=sai_ipprefix(self.uport2_myip + self.uport_mask),
            switch_id=self.switch_id,
            vr_id=self.default_vrf)
        sai_thrift_create_route_entry(self.client,
                                      self.uport2_prefix_route,
                                      next_hop_id=self.uport2_rif)

        self.uport3_prefix_route = sai_thrift_route_entry_t(
            destination=sai_ipprefix(self.uport3_myip + self.uport_mask),
            switch_id=self.switch_id,
            vr_id=self.default_vrf)
        sai_thrift_create_route_entry(self.client,
                                      self.uport3_prefix_route,
                                      next_hop_id=self.uport3_rif)

        # underlay neighbor
        self.unbor = sai_thrift_neighbor_entry_t(
            ip_address=sai_ipaddress(self.tun1_ip),
            rif_id=self.uport1_rif,
            switch_id=self.switch_id)
        sai_thrift_create_neighbor_entry(self.client,
                                         self.unbor,
                                         dst_mac_address=self.unbor1_mac)
        self.unbor2 = sai_thrift_neighbor_entry_t(
            ip_address=sai_ipaddress(self.tun2_ip),
            rif_id=self.uport2_rif,
            switch_id=self.switch_id)
        sai_thrift_create_neighbor_entry(self.client,
                                         self.unbor2,
                                         dst_mac_address=self.unbor2_mac)
        self.unbor3 = sai_thrift_neighbor_entry_t(
            ip_address=sai_ipaddress(self.tun3_ip),
            rif_id=self.uport3_rif,
            switch_id=self.switch_id)
        sai_thrift_create_neighbor_entry(self.client,
                                         self.unbor3,
                                         dst_mac_address=self.unbor3_mac)

        # underlay nexthop
        self.unhop = sai_thrift_create_next_hop(
            self.client,
            type=SAI_NEXT_HOP_TYPE_IP,
            ip=sai_ipaddress(self.tun1_ip),
            router_interface_id=self.uport1_rif)
        self.unhop2 = sai_thrift_create_next_hop(
            self.client,
            type=SAI_NEXT_HOP_TYPE_IP,
            ip=sai_ipaddress(self.tun2_ip),
            router_interface_id=self.uport2_rif)
        self.unhop3 = sai_thrift_create_next_hop(
            self.client,
            type=SAI_NEXT_HOP_TYPE_IP,
            ip=sai_ipaddress(self.tun3_ip),
            router_interface_id=self.uport3_rif)

        # create overlay vrf
        self.ovrf = sai_thrift_create_virtual_router(self.client)

        # create vlan10 rif, ip address, prefix
        self.vlan10_rif = sai_thrift_create_router_interface(
            self.client,
            virtual_router_id=self.ovrf,
            src_mac_address=ROUTER_MAC,
            type=SAI_ROUTER_INTERFACE_TYPE_VLAN,
            vlan_id=self.vlan10,
            mtu=9100,
            nat_zone_id=0)

        self.vlan10_my_route = sai_thrift_route_entry_t(
            destination=sai_ipprefix(self.vlan10_myip + '/32'),
            switch_id=self.switch_id,
            vr_id=self.ovrf)
        sai_thrift_create_route_entry(self.client,
                                      self.vlan10_my_route,
                                      packet_action=SAI_PACKET_ACTION_FORWARD,
                                      next_hop_id=self.cpu_port_hdl)

        self.vlan10_nbor_dir_br = sai_thrift_neighbor_entry_t(
            ip_address=sai_ipaddress(self.vlan10_dir_br),
            rif_id=self.vlan10_rif,
            switch_id=self.switch_id)
        sai_thrift_create_neighbor_entry(self.client,
                                         self.vlan10_nbor_dir_br,
                                         dst_mac_address=self.bcast_mac)

        self.vlan10_prefix_route = sai_thrift_route_entry_t(
            destination=sai_ipprefix(self.vlan10_myip + '/24'),
            switch_id=self.switch_id,
            vr_id=self.ovrf)
        sai_thrift_create_route_entry(self.client,
                                      self.vlan10_prefix_route,
                                      next_hop_id=self.vlan10_rif)

        # create vlan20 rif, ip address, prefix
        self.vlan20_rif = sai_thrift_create_router_interface(
            self.client,
            virtual_router_id=self.default_vrf,
            src_mac_address=ROUTER_MAC,
            type=SAI_ROUTER_INTERFACE_TYPE_VLAN,
            vlan_id=self.vlan20,
            mtu=9100,
            nat_zone_id=0)

        self.vlan20_my_route = sai_thrift_route_entry_t(
            destination=sai_ipprefix(self.vlan20_myip + '/32'),
            switch_id=self.switch_id,
            vr_id=self.default_vrf)
        sai_thrift_create_route_entry(self.client,
                                      self.vlan20_my_route,
                                      packet_action=SAI_PACKET_ACTION_FORWARD,
                                      next_hop_id=self.cpu_port_hdl)

        self.vlan20_nbor_dir_br = sai_thrift_neighbor_entry_t(
            ip_address=sai_ipaddress(self.vlan20_dir_br),
            rif_id=self.vlan20_rif,
            switch_id=self.switch_id)
        sai_thrift_create_neighbor_entry(self.client,
                                         self.vlan20_nbor_dir_br,
                                         dst_mac_address=self.bcast_mac)

        self.vlan20_prefix_route = sai_thrift_route_entry_t(
            destination=sai_ipprefix(self.vlan20_myip + '/24'),
            switch_id=self.switch_id,
            vr_id=self.default_vrf)
        sai_thrift_create_route_entry(self.client,
                                      self.vlan20_prefix_route,
                                      next_hop_id=self.vlan20_rif)

        # create fdb entry, neighbor, nexthop for customer 1 on port0, vlan 10
        self.customer1_fdb_entry = sai_thrift_fdb_entry_t(
            bv_id=self.vlan10,
            mac_address=self.customer1_mac,
            switch_id=self.switch_id)
        sai_thrift_create_fdb_entry(self.client,
                                    self.customer1_fdb_entry,
                                    type=SAI_FDB_ENTRY_TYPE_DYNAMIC,
                                    bridge_port_id=self.oport1_bp)

        self.customer1_nbor = sai_thrift_neighbor_entry_t(
            ip_address=sai_ipaddress(self.customer1_ip),
            rif_id=self.vlan10_rif,
            switch_id=self.switch_id)
        sai_thrift_create_neighbor_entry(self.client,
                                         self.customer1_nbor,
                                         dst_mac_address=self.customer1_mac)

        self.customer1_nhop = sai_thrift_create_next_hop(
            self.client,
            type=SAI_NEXT_HOP_TYPE_IP,
            ip=sai_ipaddress(self.customer1_ip),
            router_interface_id=self.vlan10_rif)

        # L2 VxLAN configuration below
        tunnel_type = SAI_TUNNEL_TYPE_VXLAN
        peer_mode = SAI_TUNNEL_PEER_MODE_P2MP
        term_type = SAI_TUNNEL_TERM_TABLE_ENTRY_TYPE_P2MP

        # create dummy vlan for vrf
        self.dummy_vlan = sai_thrift_create_vlan(self.client,
                                                 vlan_id=self.dummy_vlan_id)

        # create rif for dummy vlan
        self.dummy_vlan_rif = sai_thrift_create_router_interface(
            self.client,
            virtual_router_id=self.ovrf,
            src_mac_address=ROUTER_MAC,
            type=SAI_ROUTER_INTERFACE_TYPE_VLAN,
            vlan_id=self.dummy_vlan,
            mtu=9100,
            nat_zone_id=0)

        # create tunnel maps
        self.decap_tunnel_map_vlan = sai_thrift_create_tunnel_map(
            self.client, type=SAI_TUNNEL_MAP_TYPE_VNI_TO_VLAN_ID)

        self.encap_tunnel_map_vlan = sai_thrift_create_tunnel_map(
            self.client, type=SAI_TUNNEL_MAP_TYPE_VLAN_ID_TO_VNI)

        self.decap_tunnel_map_vrf = sai_thrift_create_tunnel_map(
            self.client, type=SAI_TUNNEL_MAP_TYPE_VNI_TO_VIRTUAL_ROUTER_ID)

        self.encap_tunnel_map_vrf = sai_thrift_create_tunnel_map(
            self.client, type=SAI_TUNNEL_MAP_TYPE_VIRTUAL_ROUTER_ID_TO_VNI)

        # create tunnel
        decap_maps = sai_thrift_object_list_t(
            count=2,
            idlist=[self.decap_tunnel_map_vlan,
                    self.decap_tunnel_map_vrf])
        encap_maps = sai_thrift_object_list_t(
            count=2,
            idlist=[self.encap_tunnel_map_vlan,
                    self.encap_tunnel_map_vrf])

        self.p2mp_tunnel = sai_thrift_create_tunnel(
            self.client,
            type=tunnel_type,
            underlay_interface=self.urif_lpb,
            decap_mappers=decap_maps,
            encap_mappers=encap_maps,
            encap_src_ip=sai_ipaddress(self.lpb_ip),
            peer_mode=peer_mode)

        self.tunnel_term = sai_thrift_create_tunnel_term_table_entry(
            self.client,
            type=term_type,
            vr_id=self.default_vrf,
            dst_ip=sai_ipaddress(self.lpb_ip),
            tunnel_type=tunnel_type,
            action_tunnel_id=self.p2mp_tunnel)

        # bridge port is created on p2mp tunnel
        self.p2mp_tun_bp = sai_thrift_create_bridge_port(
            self.client,
            type=SAI_BRIDGE_PORT_TYPE_TUNNEL,
            tunnel_id=self.p2mp_tunnel,
            bridge_id=self.default_1q_bridge,
            admin_state=True,
            fdb_learning_mode=SAI_BRIDGE_PORT_FDB_LEARNING_MODE_DISABLE)

        # create tunnel map entries for vrf and vlans
        self.decap_tunnel_map_entry_vlan10 = \
            sai_thrift_create_tunnel_map_entry(
                self.client,
                tunnel_map_type=SAI_TUNNEL_MAP_TYPE_VNI_TO_VLAN_ID,
                tunnel_map=self.decap_tunnel_map_vlan,
                vlan_id_value=self.vlan10_id,
                vni_id_key=self.vni1)

        self.encap_tunnel_map_entry_vlan10 = \
            sai_thrift_create_tunnel_map_entry(
                self.client,
                tunnel_map_type=SAI_TUNNEL_MAP_TYPE_VLAN_ID_TO_VNI,
                tunnel_map=self.encap_tunnel_map_vlan,
                vlan_id_key=self.vlan10_id,
                vni_id_value=self.vni1)

        self.decap_tunnel_map_entry_vlan20 = \
            sai_thrift_create_tunnel_map_entry(
                self.client,
                tunnel_map_type=SAI_TUNNEL_MAP_TYPE_VNI_TO_VLAN_ID,
                tunnel_map=self.decap_tunnel_map_vlan,
                vlan_id_value=self.vlan20_id,
                vni_id_key=self.vni2)

        self.encap_tunnel_map_entry_vlan20 = \
            sai_thrift_create_tunnel_map_entry(
                self.client,
                tunnel_map_type=SAI_TUNNEL_MAP_TYPE_VLAN_ID_TO_VNI,
                tunnel_map=self.encap_tunnel_map_vlan,
                vlan_id_key=self.vlan20_id,
                vni_id_value=self.vni2)

        self.decap_tunnel_map_entry_dummy_vlan = \
            sai_thrift_create_tunnel_map_entry(
                self.client,
                tunnel_map_type=SAI_TUNNEL_MAP_TYPE_VNI_TO_VLAN_ID,
                tunnel_map=self.decap_tunnel_map_vlan,
                vlan_id_value=self.dummy_vlan_id,
                vni_id_key=self.vni_for_ovrf)

        self.encap_tunnel_map_entry_dummy_vlan = \
            sai_thrift_create_tunnel_map_entry(
                self.client,
                tunnel_map_type=SAI_TUNNEL_MAP_TYPE_VLAN_ID_TO_VNI,
                tunnel_map=self.encap_tunnel_map_vlan,
                vlan_id_key=self.dummy_vlan_id,
                vni_id_value=self.vni_for_ovrf)

        self.encap_tunnel_map_entry_ovrf = sai_thrift_create_tunnel_map_entry(
            self.client,
            tunnel_map_type=SAI_TUNNEL_MAP_TYPE_VIRTUAL_ROUTER_ID_TO_VNI,
            tunnel_map=self.encap_tunnel_map_vrf,
            virtual_router_id_key=self.ovrf,
            vni_id_value=self.vni_for_ovrf)

        self.decap_tunnel_map_entry_ovrf = sai_thrift_create_tunnel_map_entry(
            self.client,
            tunnel_map_type=SAI_TUNNEL_MAP_TYPE_VNI_TO_VIRTUAL_ROUTER_ID,
            tunnel_map=self.decap_tunnel_map_vrf,
            virtual_router_id_value=self.ovrf,
            vni_id_key=self.vni_for_ovrf)

        self.encap_tunnel_map_entry_vrf = sai_thrift_create_tunnel_map_entry(
            self.client,
            tunnel_map_type=SAI_TUNNEL_MAP_TYPE_VIRTUAL_ROUTER_ID_TO_VNI,
            tunnel_map=self.encap_tunnel_map_vrf,
            virtual_router_id_key=self.default_vrf,
            vni_id_value=self.vni2)

        self.decap_tunnel_map_entry_vrf = sai_thrift_create_tunnel_map_entry(
            self.client,
            tunnel_map_type=SAI_TUNNEL_MAP_TYPE_VNI_TO_VIRTUAL_ROUTER_ID,
            tunnel_map=self.decap_tunnel_map_vrf,
            virtual_router_id_value=self.default_vrf,
            vni_id_key=self.vni2)

        # when first route comes down for a remote vtep,
        # p2p tunnel comes down first
        self.p2p_tunnel1 = sai_thrift_create_tunnel(
            self.client,
            type=tunnel_type,
            underlay_interface=self.urif_lpb,
            decap_mappers=decap_maps,
            encap_mappers=encap_maps,
            encap_src_ip=sai_ipaddress(self.lpb_ip),
            peer_mode=SAI_TUNNEL_PEER_MODE_P2P,
            encap_dst_ip=sai_ipaddress(self.tun1_ip))
        self.p2p_tunnel2 = sai_thrift_create_tunnel(
            self.client,
            type=tunnel_type,
            underlay_interface=self.urif_lpb,
            decap_mappers=decap_maps,
            encap_mappers=encap_maps,
            encap_src_ip=sai_ipaddress(self.lpb_ip),
            peer_mode=SAI_TUNNEL_PEER_MODE_P2P,
            encap_dst_ip=sai_ipaddress(self.tun2_ip))
        self.p2p_tunnel3 = sai_thrift_create_tunnel(
            self.client,
            type=tunnel_type,
            underlay_interface=self.urif_lpb,
            decap_mappers=decap_maps,
            encap_mappers=encap_maps,
            encap_src_ip=sai_ipaddress(self.lpb_ip),
            peer_mode=SAI_TUNNEL_PEER_MODE_P2P,
            encap_dst_ip=sai_ipaddress(self.tun3_ip))

        # bridge port is created on p2p tunnel
        self.p2p_tun1_bp = sai_thrift_create_bridge_port(
            self.client,
            type=SAI_BRIDGE_PORT_TYPE_TUNNEL,
            tunnel_id=self.p2p_tunnel1,
            bridge_id=self.default_1q_bridge,
            admin_state=True,
            fdb_learning_mode=SAI_BRIDGE_PORT_FDB_LEARNING_MODE_DISABLE)
        self.p2p_tun2_bp = sai_thrift_create_bridge_port(
            self.client,
            type=SAI_BRIDGE_PORT_TYPE_TUNNEL,
            tunnel_id=self.p2p_tunnel2,
            bridge_id=self.default_1q_bridge,
            admin_state=True,
            fdb_learning_mode=SAI_BRIDGE_PORT_FDB_LEARNING_MODE_DISABLE)
        self.p2p_tun3_bp = sai_thrift_create_bridge_port(
            self.client,
            type=SAI_BRIDGE_PORT_TYPE_TUNNEL,
            tunnel_id=self.p2p_tunnel3,
            bridge_id=self.default_1q_bridge,
            admin_state=True,
            fdb_learning_mode=SAI_BRIDGE_PORT_FDB_LEARNING_MODE_DISABLE)

        # vlan_member is created using tunnel bridge_port
        self.tun2_vlan_member = sai_thrift_create_vlan_member(
            self.client,
            vlan_id=self.vlan10,
            bridge_port_id=self.p2p_tun1_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)
        self.tun3_vlan_member = sai_thrift_create_vlan_member(
            self.client,
            vlan_id=self.vlan10,
            bridge_port_id=self.p2p_tun2_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)
        self.tun4_vlan_member = sai_thrift_create_vlan_member(
            self.client,
            vlan_id=self.vlan10,
            bridge_port_id=self.p2p_tun3_bp,
            vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)

        # Next hop for L3 symmetric operation
        self.tunnel_nhop = sai_thrift_create_next_hop(
            self.client,
            type=SAI_NEXT_HOP_TYPE_TUNNEL_ENCAP,
            ip=sai_ipaddress(self.tun1_ip),
            tunnel_id=self.p2mp_tunnel,
            tunnel_vni=self.vni_for_ovrf,
            tunnel_mac=self.inner_dmac)

    def runTest(self):
        try:
            self.l2VxLanEvpnTest()
            self.l2VxLanEvpnFloodTest()
        finally:
            pass

    def tearDown(self):
        sai_thrift_remove_next_hop(self.client, self.tunnel_nhop)
        sai_thrift_remove_vlan_member(self.client, self.tun2_vlan_member)
        sai_thrift_remove_vlan_member(self.client, self.tun3_vlan_member)
        sai_thrift_remove_vlan_member(self.client, self.tun4_vlan_member)
        sai_thrift_remove_bridge_port(self.client, self.p2p_tun1_bp)
        sai_thrift_remove_bridge_port(self.client, self.p2p_tun2_bp)
        sai_thrift_remove_bridge_port(self.client, self.p2p_tun3_bp)
        sai_thrift_remove_tunnel(self.client, self.p2p_tunnel1)
        sai_thrift_remove_tunnel(self.client, self.p2p_tunnel2)
        sai_thrift_remove_tunnel(self.client, self.p2p_tunnel3)
        sai_thrift_remove_tunnel_map_entry(
            self.client, self.decap_tunnel_map_entry_vrf)
        sai_thrift_remove_tunnel_map_entry(
            self.client, self.encap_tunnel_map_entry_vrf)
        sai_thrift_remove_tunnel_map_entry(
            self.client, self.decap_tunnel_map_entry_ovrf)
        sai_thrift_remove_tunnel_map_entry(
            self.client, self.encap_tunnel_map_entry_ovrf)
        sai_thrift_remove_tunnel_map_entry(
            self.client, self.decap_tunnel_map_entry_dummy_vlan)
        sai_thrift_remove_tunnel_map_entry(
            self.client, self.encap_tunnel_map_entry_dummy_vlan)
        sai_thrift_remove_tunnel_map_entry(
            self.client, self.decap_tunnel_map_entry_vlan20)
        sai_thrift_remove_tunnel_map_entry(
            self.client, self.encap_tunnel_map_entry_vlan20)
        sai_thrift_remove_tunnel_map_entry(
            self.client, self.decap_tunnel_map_entry_vlan10)
        sai_thrift_remove_tunnel_map_entry(
            self.client, self.encap_tunnel_map_entry_vlan10)
        sai_thrift_remove_bridge_port(self.client, self.p2mp_tun_bp)
        sai_thrift_remove_tunnel_term_table_entry(
            self.client, self.tunnel_term)
        sai_thrift_remove_tunnel(self.client, self.p2mp_tunnel)
        sai_thrift_remove_tunnel_map(self.client, self.encap_tunnel_map_vrf)
        sai_thrift_remove_tunnel_map(self.client, self.decap_tunnel_map_vrf)
        sai_thrift_remove_tunnel_map(self.client, self.encap_tunnel_map_vlan)
        sai_thrift_remove_tunnel_map(self.client, self.decap_tunnel_map_vlan)
        sai_thrift_remove_router_interface(self.client, self.dummy_vlan_rif)
        sai_thrift_remove_vlan(self.client, self.dummy_vlan)

        sai_thrift_flush_fdb_entries(
            self.client, entry_type=SAI_FDB_FLUSH_ENTRY_TYPE_ALL)

        sai_thrift_remove_next_hop(self.client, self.customer1_nhop)
        sai_thrift_remove_neighbor_entry(self.client, self.customer1_nbor)
        sai_thrift_remove_route_entry(self.client, self.vlan10_prefix_route)
        sai_thrift_remove_neighbor_entry(self.client, self.vlan10_nbor_dir_br)
        sai_thrift_remove_route_entry(self.client, self.vlan10_my_route)
        sai_thrift_remove_router_interface(self.client, self.vlan10_rif)
        sai_thrift_remove_route_entry(self.client, self.vlan20_prefix_route)
        sai_thrift_remove_neighbor_entry(self.client, self.vlan20_nbor_dir_br)
        sai_thrift_remove_route_entry(self.client, self.vlan20_my_route)
        sai_thrift_remove_router_interface(self.client, self.vlan20_rif)
        sai_thrift_remove_virtual_router(self.client, self.ovrf)
        sai_thrift_remove_next_hop(self.client, self.unhop)
        sai_thrift_remove_next_hop(self.client, self.unhop2)
        sai_thrift_remove_next_hop(self.client, self.unhop3)
        sai_thrift_remove_neighbor_entry(self.client, self.unbor)
        sai_thrift_remove_neighbor_entry(self.client, self.unbor2)
        sai_thrift_remove_neighbor_entry(self.client, self.unbor3)
        sai_thrift_remove_route_entry(self.client, self.uport1_prefix_route)
        sai_thrift_remove_route_entry(self.client, self.uport2_prefix_route)
        sai_thrift_remove_route_entry(self.client, self.uport3_prefix_route)
        sai_thrift_remove_route_entry(self.client, self.uport1_my_route)
        sai_thrift_remove_route_entry(self.client, self.uport2_my_route)
        sai_thrift_remove_route_entry(self.client, self.uport3_my_route)
        sai_thrift_remove_router_interface(self.client, self.urif_lpb)

        super(L2VxLanEvpnTest, self).tearDown()

    def l2VxLanEvpnTest(self):
        '''
        This test verifies that packets are properly encapsulated and
        decapsulated after SAI API calls emulating SONiC L2 EVPN operation.
        '''
        print("\nl2VxLanEvpnTest()")

        try:
            # Route for L3 symmetric operation
            customer3_prefix_route = sai_thrift_route_entry_t(
                destination=sai_ipprefix(self.customer3_ip + '/24'),
                switch_id=self.switch_id,
                vr_id=self.ovrf)
            sai_thrift_create_route_entry(self.client,
                                          customer3_prefix_route,
                                          next_hop_id=self.tunnel_nhop)

            # fdb entry for L2 forwarding into VXLAN tunnel
            fdb_entry = sai_thrift_fdb_entry_t(
                bv_id=self.vlan10,
                mac_address=self.customer2_mac,
                switch_id=self.switch_id)
            sai_thrift_create_fdb_entry(self.client,
                                        fdb_entry,
                                        type=SAI_FDB_ENTRY_TYPE_STATIC,
                                        bridge_port_id=self.p2p_tun1_bp)

            # L2 forwarding into VXLAN tunnel
            pkt = simple_udp_packet(eth_dst=self.customer2_mac,
                                    eth_src=self.customer1_mac,
                                    ip_dst=self.customer2_ip,
                                    ip_src=self.customer1_ip,
                                    ip_id=108,
                                    ip_ttl=64)
            vxlan_pkt = Mask(
                simple_vxlan_packet(eth_dst=self.unbor1_mac,
                                    eth_src=ROUTER_MAC,
                                    ip_dst=self.tun1_ip,
                                    ip_src=self.lpb_ip,
                                    ip_id=0,
                                    ip_ttl=64,
                                    ip_flags=0x2,
                                    with_udp_chksum=False,
                                    vxlan_vni=self.vni1,
                                    inner_frame=pkt))
            vxlan_pkt.set_do_not_care_scapy(UDP, 'sport')

            print("Sending packet from port %d -> VNI %d" %
                  (self.oport1_dev, self.vni1))
            send_packet(self, self.oport1_dev, pkt)
            verify_packet(self, vxlan_pkt, self.uport1_dev)
            print("Packet was L2 forwarded into the VXLAN tunnel")

            # L3 forwarding into VXLAN tunnel
            pkt_2 = simple_udp_packet(eth_dst=ROUTER_MAC,
                                      eth_src=self.customer1_mac,
                                      ip_dst=self.customer3_ip,
                                      ip_src=self.customer1_ip,
                                      ip_id=108,
                                      ip_ttl=65)
            inner_pkt_2 = simple_udp_packet(eth_dst=self.inner_dmac,
                                            eth_src=ROUTER_MAC,
                                            ip_dst=self.customer3_ip,
                                            ip_src=self.customer1_ip,
                                            ip_id=108,
                                            ip_ttl=64)
            vxlan_pkt_2 = Mask(
                simple_vxlan_packet(eth_dst=self.unbor1_mac,
                                    eth_src=ROUTER_MAC,
                                    ip_dst=self.tun1_ip,
                                    ip_src=self.lpb_ip,
                                    ip_id=0,
                                    ip_ttl=64,
                                    ip_flags=0x2,
                                    with_udp_chksum=False,
                                    vxlan_vni=self.vni_for_ovrf,
                                    inner_frame=inner_pkt_2))
            vxlan_pkt_2.set_do_not_care_scapy(UDP, 'sport')
            print("Sending packet from port %d -> VNI %d" %
                  (self.oport1_dev, self.vni_for_ovrf))
            send_packet(self, self.oport1_dev, pkt_2)
            verify_packet(self, vxlan_pkt_2, self.uport1_dev)
            print("Packet was L3 forwarded into the VXLAN tunnel")

            # L2 decap into vlan
            pkt_3 = simple_udp_packet(eth_dst=self.customer1_mac,
                                      eth_src=self.customer2_mac,
                                      ip_dst=self.customer1_ip,
                                      ip_src=self.customer2_ip,
                                      ip_id=108,
                                      ip_ttl=64)
            vxlan_pkt_3 = simple_vxlan_packet(eth_dst=ROUTER_MAC,
                                              eth_src=self.unbor1_mac,
                                              ip_dst=self.lpb_ip,
                                              ip_src=self.tun1_ip,
                                              ip_id=0,
                                              ip_ttl=64,
                                              ip_flags=0x2,
                                              with_udp_chksum=False,
                                              vxlan_vni=self.vni1,
                                              inner_frame=pkt_3)

            print("Sending packet from VNI %d -> port %d" %
                  (self.vni1, self.oport1_dev))
            send_packet(self, self.uport1_dev, vxlan_pkt_3)
            verify_packet(self, pkt_3, self.oport1_dev)
            print("Packet was L2 forwarded out of the VXLAN tunnel")

            # L3 decap into vrf
            pkt_4 = simple_udp_packet(eth_dst=self.customer1_mac,
                                      eth_src=ROUTER_MAC,
                                      ip_dst=self.customer1_ip,
                                      ip_src=self.customer3_ip,
                                      ip_id=108,
                                      ip_ttl=63)
            inner_pkt_4 = simple_udp_packet(eth_dst=ROUTER_MAC,
                                            eth_src=self.inner_dmac,
                                            ip_dst=self.customer1_ip,
                                            ip_src=self.customer3_ip,
                                            ip_id=108,
                                            ip_ttl=64)
            vxlan_pkt_4 = simple_vxlan_packet(eth_dst=ROUTER_MAC,
                                              eth_src=self.unbor1_mac,
                                              ip_dst=self.lpb_ip,
                                              ip_src=self.tun1_ip,
                                              ip_id=0,
                                              ip_ttl=64,
                                              ip_flags=0x2,
                                              with_udp_chksum=False,
                                              vxlan_vni=self.vni_for_ovrf,
                                              inner_frame=inner_pkt_4)

            print("Sending packet from VNI %d -> port %d" %
                  (self.vni_for_ovrf, self.oport1_dev))
            send_packet(self, self.uport1_dev, vxlan_pkt_4)
            verify_packet(self, pkt_4, self.oport1_dev)
            print("Packet was L3 forwarded out of the VXLAN tunnel")

        finally:
            sai_thrift_flush_fdb_entries(
                self.client, entry_type=SAI_FDB_FLUSH_ENTRY_TYPE_ALL)
            sai_thrift_remove_route_entry(self.client,
                                          customer3_prefix_route)

    def l2VxLanEvpnFloodTest(self):
        '''
        This test verifies that packets are properly encapsulated and flodded
        to all VLAN members (when there is no FBD entry) after SAI API calls
        emulating SONiC L2 EVPN operation.
        '''
        print("\nl2VxLanEvpnFloodTest()")

        try:
            # L2 encap + flooding into vlan
            pkt = simple_udp_packet(eth_dst=self.customer2_mac,
                                    eth_src=self.customer1_mac,
                                    ip_dst=self.customer2_ip,
                                    ip_src=self.customer1_ip,
                                    ip_id=108,
                                    ip_ttl=64,
                                    pktlen=100)
            # expected packets
            tag_pkt = simple_udp_packet(eth_dst=self.customer2_mac,
                                        eth_src=self.customer1_mac,
                                        ip_dst=self.customer2_ip,
                                        ip_src=self.customer1_ip,
                                        dl_vlan_enable=True,
                                        vlan_vid=self.vlan10_id,
                                        ip_id=108,
                                        ip_ttl=64,
                                        pktlen=104)
            vxlan_pkt = Mask(
                simple_vxlan_packet(eth_dst=self.unbor1_mac,
                                    eth_src=ROUTER_MAC,
                                    ip_dst=self.tun1_ip,
                                    ip_src=self.lpb_ip,
                                    ip_id=0,
                                    ip_ttl=64,
                                    ip_flags=0x2,
                                    with_udp_chksum=False,
                                    vxlan_vni=self.vni1,
                                    inner_frame=pkt))
            vxlan_pkt.set_do_not_care_scapy(UDP, 'sport')
            vxlan_pkt2 = Mask(
                simple_vxlan_packet(eth_dst=self.unbor2_mac,
                                    eth_src=ROUTER_MAC,
                                    ip_dst=self.tun2_ip,
                                    ip_src=self.lpb_ip,
                                    ip_id=0,
                                    ip_ttl=64,
                                    ip_flags=0x2,
                                    with_udp_chksum=False,
                                    vxlan_vni=self.vni1,
                                    inner_frame=pkt))
            vxlan_pkt2.set_do_not_care_scapy(UDP, 'sport')
            vxlan_pkt3 = Mask(
                simple_vxlan_packet(eth_dst=self.unbor3_mac,
                                    eth_src=ROUTER_MAC,
                                    ip_dst=self.tun3_ip,
                                    ip_src=self.lpb_ip,
                                    ip_id=0,
                                    ip_ttl=64,
                                    ip_flags=0x2,
                                    with_udp_chksum=False,
                                    vxlan_vni=self.vni1,
                                    inner_frame=pkt))
            vxlan_pkt3.set_do_not_care_scapy(UDP, 'sport')

            print("Sending packet from port %d" % self.oport1_dev)
            send_packet(self, self.oport1_dev, pkt)
            verify_each_packet_on_multiple_port_lists(
                self, [tag_pkt, pkt, vxlan_pkt, vxlan_pkt2, vxlan_pkt3],
                [[self.dev_port1],
                 [self.dev_port4, self.dev_port5, self.dev_port6],
                 [self.uport1_dev], [self.uport2_dev], [self.uport3_dev]])
            print("Packet was flooded into L2 VXLAN tunnels "
                  "and physical ports")

            # L2 decap + flooding into vlan
            vxlan_pkt = simple_vxlan_packet(eth_dst=ROUTER_MAC,
                                            eth_src=self.unbor1_mac,
                                            ip_dst=self.lpb_ip,
                                            ip_src=self.tun1_ip,
                                            ip_id=0,
                                            ip_ttl=64,
                                            ip_flags=0x2,
                                            with_udp_chksum=False,
                                            vxlan_vni=self.vni1,
                                            inner_frame=pkt)

            print("Sending packet from VNI %d" % self.vni1)
            send_packet(self, self.uport1_dev, vxlan_pkt)
            verify_each_packet_on_multiple_port_lists(
                self, [pkt, tag_pkt, pkt],
                [[self.dev_port0], [self.dev_port1],
                 [self.dev_port4, self.dev_port5, self.dev_port6]])
            print("Packet was flooded out of L2 VXLAN tunnel")

        finally:
            sai_thrift_flush_fdb_entries(
                self.client, entry_type=SAI_FDB_FLUSH_ENTRY_TYPE_ALL)


# The reason for this test to be in sail2vxlan.py is because
# L3 asymmetric forwarding into VxLAN is only supported when
# L2 VxLANs are supported
class VxLanL3AsymmetricTest(SaiHelper):
    '''
    This class contains tests for L3 asymmetric forwarding into VxLAN
    '''
    def setUp(self):
        super(VxLanL3AsymmetricTest, self).setUp()

        self.oport1 = self.port0
        self.oport1_dev = self.dev_port0
        self.oport1_bp = self.port0_bp
        self.uport1 = self.port10
        self.uport1_dev = self.dev_port10
        self.uport1_rif = self.port10_rif
        self.uport2 = self.port11
        self.uport2_dev = self.dev_port11
        self.uport2_rif = self.port11_rif
        self.tun1_ip = "10.0.0.65"
        self.tun2_ip = "10.0.0.67"
        self.lpb_ip = "10.1.0.32"
        self.tun_ip_mask = "/32"
        self.uport1_myip = "10.0.0.64"
        self.uport2_myip = "10.0.0.66"
        self.uport_mask = "/31"

        self.vlan10_id = 10
        self.vlan20_id = 20
        self.vni1 = 1000
        self.vni2 = 2000
        self.vni_for_ovrf = 5000
        self.dummy_vlan_id = 50
        self.vlan10_myip = "192.168.100.254"
        self.vlan10_dir_br = "192.168.100.255"
        self.vlan20_myip = "192.168.200.254"
        self.vlan20_dir_br = "192.168.200.255"
        self.customer1_ip = "192.168.100.1"
        self.customer1_ipv6 = "2001:0db8::100:2"
        self.customer2_ip = "192.168.200.1"
        self.customer2_ipv6 = "2001:0db8::200:2"
        self.inner_dmac = "00:11:11:11:11:11"
        self.customer1_mac = "00:22:22:22:22:11"
        self.customer2_mac = "00:22:22:22:22:22"
        self.unbor1_mac = "00:33:33:33:33:11"
        self.unbor2_mac = "00:33:33:33:33:22"
        self.bcast_mac = "ff:ff:ff:ff:ff:ff"

        # underlay loopback rif for tunnels
        self.urif_lpb = sai_thrift_create_router_interface(
            self.client,
            type=SAI_ROUTER_INTERFACE_TYPE_LOOPBACK,
            virtual_router_id=self.default_vrf,
            mtu=9100)

        # underlay my ip address + prefix
        self.uport1_my_route = sai_thrift_route_entry_t(
            destination=sai_ipprefix(self.uport1_myip + '/32'),
            switch_id=self.switch_id,
            vr_id=self.default_vrf)
        sai_thrift_create_route_entry(self.client,
                                      self.uport1_my_route,
                                      packet_action=SAI_PACKET_ACTION_FORWARD,
                                      next_hop_id=self.cpu_port_hdl)

        self.uport1_prefix_route = sai_thrift_route_entry_t(
            destination=sai_ipprefix(self.uport1_myip + self.uport_mask),
            switch_id=self.switch_id,
            vr_id=self.default_vrf)
        sai_thrift_create_route_entry(self.client,
                                      self.uport1_prefix_route,
                                      next_hop_id=self.uport1_rif)

        # underlay neighbor
        self.unbor = sai_thrift_neighbor_entry_t(
            ip_address=sai_ipaddress(self.tun1_ip),
            rif_id=self.uport1_rif,
            switch_id=self.switch_id)
        sai_thrift_create_neighbor_entry(self.client,
                                         self.unbor,
                                         dst_mac_address=self.unbor1_mac)

        # underlay nexthop
        self.unhop = sai_thrift_create_next_hop(
            self.client,
            type=SAI_NEXT_HOP_TYPE_IP,
            ip=sai_ipaddress(self.tun1_ip),
            router_interface_id=self.uport1_rif)

        # underlay my ip address + prefix
        self.uport2_my_route = sai_thrift_route_entry_t(
            destination=sai_ipprefix(self.uport2_myip + '/32'),
            switch_id=self.switch_id,
            vr_id=self.default_vrf)
        sai_thrift_create_route_entry(self.client,
                                      self.uport2_my_route,
                                      packet_action=SAI_PACKET_ACTION_FORWARD,
                                      next_hop_id=self.cpu_port_hdl)

        self.uport2_prefix_route = sai_thrift_route_entry_t(
            destination=sai_ipprefix(self.uport2_myip + self.uport_mask),
            switch_id=self.switch_id,
            vr_id=self.default_vrf)
        sai_thrift_create_route_entry(self.client,
                                      self.uport2_prefix_route,
                                      next_hop_id=self.uport2_rif)

        # underlay neighbor
        self.unbor2 = sai_thrift_neighbor_entry_t(
            ip_address=sai_ipaddress(self.tun2_ip),
            rif_id=self.uport2_rif,
            switch_id=self.switch_id)
        sai_thrift_create_neighbor_entry(self.client,
                                         self.unbor2,
                                         dst_mac_address=self.unbor2_mac)

        # underlay nexthop
        self.unhop2 = sai_thrift_create_next_hop(
            self.client,
            type=SAI_NEXT_HOP_TYPE_IP,
            ip=sai_ipaddress(self.tun2_ip),
            router_interface_id=self.uport2_rif)

        # create overlay vrf
        self.ovrf = sai_thrift_create_virtual_router(self.client)

        # create vlan10 rif, ip address, prefix
        self.vlan10_rif = sai_thrift_create_router_interface(
            self.client,
            virtual_router_id=self.ovrf,
            src_mac_address=ROUTER_MAC,
            type=SAI_ROUTER_INTERFACE_TYPE_VLAN,
            vlan_id=self.vlan10,
            mtu=9100,
            nat_zone_id=0)

        self.vlan10_my_route = sai_thrift_route_entry_t(
            destination=sai_ipprefix(self.vlan10_myip + '/32'),
            switch_id=self.switch_id,
            vr_id=self.ovrf)
        sai_thrift_create_route_entry(self.client,
                                      self.vlan10_my_route,
                                      packet_action=SAI_PACKET_ACTION_FORWARD,
                                      next_hop_id=self.cpu_port_hdl)

        self.vlan10_nbor_dir_br = sai_thrift_neighbor_entry_t(
            ip_address=sai_ipaddress(self.vlan10_dir_br),
            rif_id=self.vlan10_rif,
            switch_id=self.switch_id)
        sai_thrift_create_neighbor_entry(self.client,
                                         self.vlan10_nbor_dir_br,
                                         dst_mac_address=self.bcast_mac)

        self.vlan10_prefix_route = sai_thrift_route_entry_t(
            destination=sai_ipprefix(self.vlan10_myip + '/24'),
            switch_id=self.switch_id,
            vr_id=self.ovrf)
        sai_thrift_create_route_entry(self.client,
                                      self.vlan10_prefix_route,
                                      next_hop_id=self.vlan10_rif)

        # create vlan20 rif, ip address, prefix
        self.vlan20_rif = sai_thrift_create_router_interface(
            self.client,
            virtual_router_id=self.ovrf,
            src_mac_address=ROUTER_MAC,
            type=SAI_ROUTER_INTERFACE_TYPE_VLAN,
            vlan_id=self.vlan20,
            mtu=9100,
            nat_zone_id=0)

        self.vlan20_my_route = sai_thrift_route_entry_t(
            destination=sai_ipprefix(self.vlan20_myip + '/32'),
            switch_id=self.switch_id,
            vr_id=self.ovrf)
        sai_thrift_create_route_entry(self.client,
                                      self.vlan20_my_route,
                                      packet_action=SAI_PACKET_ACTION_FORWARD,
                                      next_hop_id=self.cpu_port_hdl)

        self.vlan20_nbor_dir_br = sai_thrift_neighbor_entry_t(
            ip_address=sai_ipaddress(self.vlan20_dir_br),
            rif_id=self.vlan20_rif,
            switch_id=self.switch_id)
        sai_thrift_create_neighbor_entry(self.client,
                                         self.vlan20_nbor_dir_br,
                                         dst_mac_address=self.bcast_mac)

        self.vlan20_prefix_route = sai_thrift_route_entry_t(
            destination=sai_ipprefix(self.vlan20_myip + '/24'),
            switch_id=self.switch_id,
            vr_id=self.ovrf)
        sai_thrift_create_route_entry(self.client,
                                      self.vlan20_prefix_route,
                                      next_hop_id=self.vlan20_rif)

        # create fdb entry, neighbor, nexthop for customer 1 on port0, vlan 10
        self.customer1_fdb_entry = sai_thrift_fdb_entry_t(
            bv_id=self.vlan10,
            mac_address=self.customer1_mac,
            switch_id=self.switch_id)
        sai_thrift_create_fdb_entry(self.client,
                                    self.customer1_fdb_entry,
                                    type=SAI_FDB_ENTRY_TYPE_DYNAMIC,
                                    bridge_port_id=self.oport1_bp)

        self.customer1_nbor = sai_thrift_neighbor_entry_t(
            ip_address=sai_ipaddress(self.customer1_ip),
            rif_id=self.vlan10_rif,
            switch_id=self.switch_id)
        sai_thrift_create_neighbor_entry(self.client,
                                         self.customer1_nbor,
                                         dst_mac_address=self.customer1_mac)

        self.customer1_nhop = sai_thrift_create_next_hop(
            self.client,
            type=SAI_NEXT_HOP_TYPE_IP,
            ip=sai_ipaddress(self.customer1_ip),
            router_interface_id=self.vlan10_rif)

    def runTest(self):
        try:
            self.vxLanL3AsymmetricTestOldVersion()
            self.vxLanL3AsymmetricTest()
            self.vxLanL3AsymmetricEcmpTest()
            self.vxLanL3AsymmetricMacMoveTest()
        finally:
            pass

    def tearDown(self):
        sai_thrift_flush_fdb_entries(
            self.client, entry_type=SAI_FDB_FLUSH_ENTRY_TYPE_ALL)

        sai_thrift_remove_next_hop(self.client, self.customer1_nhop)
        sai_thrift_remove_neighbor_entry(self.client, self.customer1_nbor)
        sai_thrift_remove_route_entry(self.client, self.vlan10_prefix_route)
        sai_thrift_remove_neighbor_entry(self.client, self.vlan10_nbor_dir_br)
        sai_thrift_remove_route_entry(self.client, self.vlan10_my_route)
        sai_thrift_remove_router_interface(self.client, self.vlan10_rif)
        sai_thrift_remove_route_entry(self.client, self.vlan20_prefix_route)
        sai_thrift_remove_neighbor_entry(self.client, self.vlan20_nbor_dir_br)
        sai_thrift_remove_route_entry(self.client, self.vlan20_my_route)
        sai_thrift_remove_router_interface(self.client, self.vlan20_rif)
        sai_thrift_remove_virtual_router(self.client, self.ovrf)
        sai_thrift_remove_next_hop(self.client, self.unhop2)
        sai_thrift_remove_neighbor_entry(self.client, self.unbor2)
        sai_thrift_remove_route_entry(self.client, self.uport2_prefix_route)
        sai_thrift_remove_route_entry(self.client, self.uport2_my_route)
        sai_thrift_remove_next_hop(self.client, self.unhop)
        sai_thrift_remove_neighbor_entry(self.client, self.unbor)
        sai_thrift_remove_route_entry(self.client, self.uport1_prefix_route)
        sai_thrift_remove_route_entry(self.client, self.uport1_my_route)
        sai_thrift_remove_router_interface(self.client, self.urif_lpb)

        super(VxLanL3AsymmetricTest, self).tearDown()

    def vxLanL3AsymmetricTestOldVersion(self):
        '''
        This test verifies that packets are properly encapsulated and
        decapsulated in case of L2 asymmetric forwarding to VxLAN.
        This test uses nexthop of type tunnel with vni and mac set
        to create proper configuration.
        '''
        print("\nvxLanL3AsymmetricTestOldVersion()")

        tunnel_type = SAI_TUNNEL_TYPE_VXLAN
        peer_mode = SAI_TUNNEL_PEER_MODE_P2MP
        term_type = SAI_TUNNEL_TERM_TABLE_ENTRY_TYPE_P2MP

        try:
            # create dummy vlan for vrf
            dummy_vlan = sai_thrift_create_vlan(self.client,
                                                vlan_id=self.dummy_vlan_id)

            # create rif for dummy vlan
            dummy_vlan_rif = sai_thrift_create_router_interface(
                self.client,
                virtual_router_id=self.ovrf,
                src_mac_address=ROUTER_MAC,
                type=SAI_ROUTER_INTERFACE_TYPE_VLAN,
                vlan_id=dummy_vlan,
                mtu=9100,
                nat_zone_id=0)

            # create tunnel maps
            decap_tunnel_map_vlan = sai_thrift_create_tunnel_map(
                self.client, type=SAI_TUNNEL_MAP_TYPE_VNI_TO_VLAN_ID)

            encap_tunnel_map_vlan = sai_thrift_create_tunnel_map(
                self.client, type=SAI_TUNNEL_MAP_TYPE_VLAN_ID_TO_VNI)

            # create tunnel
            # Note: encap_maps order of vlan first then vrf exposes SWI-3916,
            #       with the reverse order the test passes even without a fix
            decap_maps = sai_thrift_object_list_t(
                count=1,
                idlist=[decap_tunnel_map_vlan])
            encap_maps = sai_thrift_object_list_t(
                count=1,
                idlist=[encap_tunnel_map_vlan])
            tunnel0 = sai_thrift_create_tunnel(
                self.client,
                type=tunnel_type,
                underlay_interface=self.urif_lpb,
                decap_mappers=decap_maps,
                encap_mappers=encap_maps,
                encap_src_ip=sai_ipaddress(self.lpb_ip),
                peer_mode=peer_mode)

            tunnel_term = sai_thrift_create_tunnel_term_table_entry(
                self.client,
                type=term_type,
                vr_id=self.default_vrf,
                dst_ip=sai_ipaddress(self.lpb_ip),
                tunnel_type=tunnel_type,
                action_tunnel_id=tunnel0)

            # create tunnel map entries for vlans
            decap_tunnel_map_entry_vlan1 = sai_thrift_create_tunnel_map_entry(
                self.client,
                tunnel_map_type=SAI_TUNNEL_MAP_TYPE_VNI_TO_VLAN_ID,
                tunnel_map=decap_tunnel_map_vlan,
                vlan_id_value=self.dummy_vlan_id,
                vni_id_key=self.vni_for_ovrf)

            decap_tunnel_map_entry_vlan10 = sai_thrift_create_tunnel_map_entry(
                self.client,
                tunnel_map_type=SAI_TUNNEL_MAP_TYPE_VNI_TO_VLAN_ID,
                tunnel_map=decap_tunnel_map_vlan,
                vlan_id_value=self.vlan10_id,
                vni_id_key=self.vni1)

            decap_tunnel_map_entry_vlan20 = sai_thrift_create_tunnel_map_entry(
                self.client,
                tunnel_map_type=SAI_TUNNEL_MAP_TYPE_VNI_TO_VLAN_ID,
                tunnel_map=decap_tunnel_map_vlan,
                vlan_id_value=self.vlan20_id,
                vni_id_key=self.vni2)

            # when first route comes down for a remote vtep,
            # p2p tunnel comes down first
            tunnel1 = sai_thrift_create_tunnel(
                self.client,
                type=tunnel_type,
                underlay_interface=self.urif_lpb,
                decap_mappers=decap_maps,
                encap_mappers=encap_maps,
                encap_src_ip=sai_ipaddress(self.lpb_ip),
                peer_mode=SAI_TUNNEL_PEER_MODE_P2P,
                encap_dst_ip=sai_ipaddress(self.tun1_ip))

            # verify p2p tunnel creation with peer_mode P2P
            attr = sai_thrift_get_tunnel_attribute(
                self.client, tunnel1, peer_mode=True, encap_dst_ip=True)
            self.assertEqual(attr['peer_mode'],
                             SAI_TUNNEL_PEER_MODE_P2P,
                             "p2p tunnel peer mode is not correct")
            self.assertEqual(attr['encap_dst_ip'].addr.ip4,
                             sai_ipaddress(self.tun1_ip).addr.ip4,
                             "p2p tunnel dst ip is not correct")

            # bridge port is created on p2p tunnel
            tun1_bp = sai_thrift_create_bridge_port(
                self.client,
                type=SAI_BRIDGE_PORT_TYPE_TUNNEL,
                tunnel_id=tunnel1,
                bridge_id=self.default_1q_bridge,
                admin_state=True,
                fdb_learning_mode=SAI_BRIDGE_PORT_FDB_LEARNING_MODE_DISABLE)

            # verify bridge port on p2p tunnel
            attr = sai_thrift_get_bridge_port_attribute(
                self.client, tun1_bp, tunnel_id=True, fdb_learning_mode=True)
            self.assertEqual(attr['tunnel_id'],
                             tunnel1,
                             "bridge port tunnel_id is not correct")
            self.assertEqual(attr['fdb_learning_mode'],
                             SAI_BRIDGE_PORT_FDB_LEARNING_MODE_DISABLE,
                             "bridge port fdb learning mode is not correct")

            # vlan_member is created using tunnel bridge_port
            tun1_vlan_member = sai_thrift_create_vlan_member(
                self.client,
                vlan_id=self.vlan10,
                bridge_port_id=tun1_bp,
                vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)

            # verify vlan member on p2p tunnel
            attr = sai_thrift_get_vlan_member_attribute(
                self.client, tun1_vlan_member, bridge_port_id=True)
            self.assertEqual(attr['bridge_port_id'],
                             tun1_bp,
                             "vlan member bridge_port_id is not correct")

            # Next hop for asymmetric operation
            tunnel_nhop_asym = sai_thrift_create_next_hop(
                self.client,
                type=SAI_NEXT_HOP_TYPE_TUNNEL_ENCAP,
                ip=sai_ipaddress(self.tun1_ip),
                tunnel_id=tunnel0,
                tunnel_vni=self.vni2,
                tunnel_mac=self.customer2_mac)

            # do we need this route? customer is not creating it
            customer2_prefix_route = sai_thrift_route_entry_t(
                destination=sai_ipprefix(self.customer2_ip + '/32'),
                switch_id=self.switch_id,
                vr_id=self.ovrf)
            sai_thrift_create_route_entry(self.client,
                                          customer2_prefix_route,
                                          next_hop_id=tunnel_nhop_asym)

            pkt = simple_udp_packet(eth_dst=ROUTER_MAC,
                                    eth_src=self.customer1_mac,
                                    ip_dst=self.customer2_ip,
                                    ip_src=self.customer1_ip,
                                    ip_id=108,
                                    ip_ttl=65)
            inner_pkt = simple_udp_packet(eth_dst=self.customer2_mac,
                                          eth_src=ROUTER_MAC,
                                          ip_dst=self.customer2_ip,
                                          ip_src=self.customer1_ip,
                                          ip_id=108,
                                          ip_ttl=64)
            vxlan_pkt = Mask(
                simple_vxlan_packet(eth_dst=self.unbor1_mac,
                                    eth_src=ROUTER_MAC,
                                    ip_dst=self.tun1_ip,
                                    ip_src=self.lpb_ip,
                                    ip_id=0,
                                    ip_ttl=64,
                                    ip_flags=0x2,
                                    with_udp_chksum=False,
                                    vxlan_vni=self.vni2,
                                    inner_frame=inner_pkt))
            vxlan_pkt.set_do_not_care_scapy(UDP, 'sport')

            print("Sending packet from port %d -> VNI %d" %
                  (self.oport1_dev, self.vni2))
            send_packet(self, self.oport1_dev, pkt)
            verify_packet(self, vxlan_pkt, self.uport1_dev)

            pkt_2 = simple_udp_packet(eth_dst=self.customer1_mac,
                                      eth_src=self.inner_dmac,
                                      ip_dst=self.customer1_ip,
                                      ip_src=self.customer2_ip,
                                      ip_id=108,
                                      ip_ttl=64)
            inner_pkt_2 = simple_udp_packet(eth_dst=self.customer1_mac,
                                            eth_src=self.inner_dmac,
                                            ip_dst=self.customer1_ip,
                                            ip_src=self.customer2_ip,
                                            ip_id=108,
                                            ip_ttl=64)
            vxlan_pkt_2 = simple_vxlan_packet(eth_dst=ROUTER_MAC,
                                              eth_src=self.unbor1_mac,
                                              ip_dst=self.lpb_ip,
                                              ip_src=self.tun1_ip,
                                              ip_id=0,
                                              ip_ttl=64,
                                              ip_flags=0x2,
                                              with_udp_chksum=False,
                                              vxlan_vni=self.vni1,
                                              inner_frame=inner_pkt_2)

            print("Sending packet from VNI %d -> port %d" %
                  (self.vni1, self.oport1_dev))
            send_packet(self, self.uport1_dev, vxlan_pkt_2)
            verify_packet(self, pkt_2, self.oport1_dev)

        finally:
            sai_thrift_remove_route_entry(self.client,
                                          customer2_prefix_route)
            sai_thrift_remove_next_hop(self.client, tunnel_nhop_asym)
            if 'tun1_vlan_member' in locals() and tun1_vlan_member:
                sai_thrift_remove_vlan_member(self.client, tun1_vlan_member)
            if 'tun1_bp' in locals() and tun1_bp:
                sai_thrift_remove_bridge_port(self.client, tun1_bp)
            if 'tunnel1' in locals() and tunnel1:
                sai_thrift_remove_tunnel(self.client, tunnel1)
            sai_thrift_remove_tunnel_map_entry(self.client,
                                               decap_tunnel_map_entry_vlan1)
            sai_thrift_remove_tunnel_map_entry(self.client,
                                               decap_tunnel_map_entry_vlan10)
            sai_thrift_remove_tunnel_map_entry(self.client,
                                               decap_tunnel_map_entry_vlan20)
            sai_thrift_remove_tunnel_term_table_entry(self.client, tunnel_term)
            sai_thrift_remove_tunnel(self.client, tunnel0)
            sai_thrift_remove_tunnel_map(self.client, encap_tunnel_map_vlan)
            sai_thrift_remove_tunnel_map(self.client, decap_tunnel_map_vlan)
            sai_thrift_remove_router_interface(self.client, dummy_vlan_rif)
            sai_thrift_remove_vlan(self.client, dummy_vlan)

    def vxLanL3AsymmetricTest(self):
        '''
        This test verifies that packets are properly encapsulated and
        decapsulated in case of L2 asymmetric forwarding to VxLAN.
        This test uses nexthop of type IP with RIF of type VLAN, having
        a MAC_ENTRY with a destination handle of type TUNNEL to create
        proper configuration.
        '''
        print("\nvxLanL3AsymmetricTest()")

        tunnel_type = SAI_TUNNEL_TYPE_VXLAN
        peer_mode = SAI_TUNNEL_PEER_MODE_P2MP
        term_type = SAI_TUNNEL_TERM_TABLE_ENTRY_TYPE_P2MP

        try:
            # create dummy vlan for vrf
            dummy_vlan = sai_thrift_create_vlan(self.client,
                                                vlan_id=self.dummy_vlan_id)

            # create rif for dummy vlan
            dummy_vlan_rif = sai_thrift_create_router_interface(
                self.client,
                virtual_router_id=self.ovrf,
                src_mac_address=ROUTER_MAC,
                type=SAI_ROUTER_INTERFACE_TYPE_VLAN,
                vlan_id=dummy_vlan,
                mtu=9100,
                nat_zone_id=0)

            # create tunnel maps
            decap_tunnel_map_vlan = sai_thrift_create_tunnel_map(
                self.client, type=SAI_TUNNEL_MAP_TYPE_VNI_TO_VLAN_ID)

            encap_tunnel_map_vlan = sai_thrift_create_tunnel_map(
                self.client, type=SAI_TUNNEL_MAP_TYPE_VLAN_ID_TO_VNI)

            # create tunnel
            # Note: encap_maps order of vlan first then vrf exposes SWI-3916,
            #       with the reverse order the test passes even without a fix
            decap_maps = sai_thrift_object_list_t(
                count=1,
                idlist=[decap_tunnel_map_vlan])
            encap_maps = sai_thrift_object_list_t(
                count=1,
                idlist=[encap_tunnel_map_vlan])
            tunnel0 = sai_thrift_create_tunnel(
                self.client,
                type=tunnel_type,
                underlay_interface=self.urif_lpb,
                decap_mappers=decap_maps,
                encap_mappers=encap_maps,
                encap_src_ip=sai_ipaddress(self.lpb_ip),
                peer_mode=peer_mode)

            tunnel_term = sai_thrift_create_tunnel_term_table_entry(
                self.client,
                type=term_type,
                vr_id=self.default_vrf,
                dst_ip=sai_ipaddress(self.lpb_ip),
                tunnel_type=tunnel_type,
                action_tunnel_id=tunnel0)

            # create tunnel map entries for vlans
            decap_tunnel_map_entry_vlan1 = sai_thrift_create_tunnel_map_entry(
                self.client,
                tunnel_map_type=SAI_TUNNEL_MAP_TYPE_VNI_TO_VLAN_ID,
                tunnel_map=decap_tunnel_map_vlan,
                vlan_id_value=self.dummy_vlan_id,
                vni_id_key=self.vni_for_ovrf)

            decap_tunnel_map_entry_vlan10 = sai_thrift_create_tunnel_map_entry(
                self.client,
                tunnel_map_type=SAI_TUNNEL_MAP_TYPE_VNI_TO_VLAN_ID,
                tunnel_map=decap_tunnel_map_vlan,
                vlan_id_value=self.vlan10_id,
                vni_id_key=self.vni1)

            decap_tunnel_map_entry_vlan20 = sai_thrift_create_tunnel_map_entry(
                self.client,
                tunnel_map_type=SAI_TUNNEL_MAP_TYPE_VNI_TO_VLAN_ID,
                tunnel_map=decap_tunnel_map_vlan,
                vlan_id_value=self.vlan20_id,
                vni_id_key=self.vni2)

            # when first route comes down for a remote vtep,
            # p2p tunnel comes down first
            tunnel1 = sai_thrift_create_tunnel(
                self.client,
                type=tunnel_type,
                underlay_interface=self.urif_lpb,
                decap_mappers=decap_maps,
                encap_mappers=encap_maps,
                encap_src_ip=sai_ipaddress(self.lpb_ip),
                peer_mode=SAI_TUNNEL_PEER_MODE_P2P,
                encap_dst_ip=sai_ipaddress(self.tun1_ip))

            # verify p2p tunnel creation with peer_mode P2P
            attr = sai_thrift_get_tunnel_attribute(
                self.client, tunnel1, peer_mode=True, encap_dst_ip=True)
            self.assertEqual(attr['peer_mode'],
                             SAI_TUNNEL_PEER_MODE_P2P,
                             "p2p tunnel peer mode is not correct")
            self.assertEqual(attr['encap_dst_ip'].addr.ip4,
                             sai_ipaddress(self.tun1_ip).addr.ip4,
                             "p2p tunnel dst ip is not correct")

            # bridge port is created on p2p tunnel
            tun1_bp = sai_thrift_create_bridge_port(
                self.client,
                type=SAI_BRIDGE_PORT_TYPE_TUNNEL,
                tunnel_id=tunnel1,
                bridge_id=self.default_1q_bridge,
                admin_state=True,
                fdb_learning_mode=SAI_BRIDGE_PORT_FDB_LEARNING_MODE_DISABLE)

            # verify bridge port on p2p tunnel
            attr = sai_thrift_get_bridge_port_attribute(
                self.client, tun1_bp, tunnel_id=True, fdb_learning_mode=True)
            self.assertEqual(attr['tunnel_id'],
                             tunnel1,
                             "bridge port tunnel_id is not correct")
            self.assertEqual(attr['fdb_learning_mode'],
                             SAI_BRIDGE_PORT_FDB_LEARNING_MODE_DISABLE,
                             "bridge port fdb learning mode is not correct")

            # vlan_member is created using tunnel bridge_port
            tun1_vlan_member = sai_thrift_create_vlan_member(
                self.client,
                vlan_id=self.vlan10,
                bridge_port_id=tun1_bp,
                vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)

            # verify vlan member on p2p tunnel
            attr = sai_thrift_get_vlan_member_attribute(
                self.client, tun1_vlan_member, bridge_port_id=True)
            self.assertEqual(attr['bridge_port_id'],
                             tun1_bp,
                             "vlan member bridge_port_id is not correct")

            nbor = sai_thrift_neighbor_entry_t(
                rif_id=self.vlan20_rif,
                ip_address=sai_ipaddress(self.customer2_ip))
            sai_thrift_create_neighbor_entry(
                self.client, nbor, dst_mac_address=self.customer2_mac)

            tunnel_nhop_asym = sai_thrift_create_next_hop(
                self.client,
                type=SAI_NEXT_HOP_TYPE_IP,
                ip=sai_ipaddress(self.customer2_ip),
                router_interface_id=self.vlan20_rif)

            fdb_entry = sai_thrift_fdb_entry_t(
                bv_id=self.vlan20,
                mac_address=self.customer2_mac,
                switch_id=self.switch_id)
            sai_thrift_create_fdb_entry(
                self.client,
                fdb_entry,
                type=SAI_FDB_ENTRY_TYPE_STATIC,
                bridge_port_id=tun1_bp,
                endpoint_ip=sai_ipaddress(self.tun1_ip))

            pkt = simple_udp_packet(eth_dst=ROUTER_MAC,
                                    eth_src=self.customer1_mac,
                                    ip_dst=self.customer2_ip,
                                    ip_src=self.customer1_ip,
                                    ip_id=108,
                                    ip_ttl=65)
            inner_pkt = simple_udp_packet(eth_dst=self.customer2_mac,
                                          eth_src=ROUTER_MAC,
                                          ip_dst=self.customer2_ip,
                                          ip_src=self.customer1_ip,
                                          ip_id=108,
                                          ip_ttl=64)
            vxlan_pkt = Mask(
                simple_vxlan_packet(eth_dst=self.unbor1_mac,
                                    eth_src=ROUTER_MAC,
                                    ip_dst=self.tun1_ip,
                                    ip_src=self.lpb_ip,
                                    ip_id=0,
                                    ip_ttl=64,
                                    ip_flags=0x2,
                                    with_udp_chksum=False,
                                    vxlan_vni=self.vni2,
                                    inner_frame=inner_pkt))
            vxlan_pkt.set_do_not_care_scapy(UDP, 'sport')

            print("Sending packet from port %d -> VNI %d" %
                  (self.oport1_dev, self.vni2))
            send_packet(self, self.oport1_dev, pkt)
            verify_packet(self, vxlan_pkt, self.uport1_dev)

            pkt_2 = simple_udp_packet(eth_dst=self.customer1_mac,
                                      eth_src=self.inner_dmac,
                                      ip_dst=self.customer1_ip,
                                      ip_src=self.customer2_ip,
                                      ip_id=108,
                                      ip_ttl=64)
            inner_pkt_2 = simple_udp_packet(eth_dst=self.customer1_mac,
                                            eth_src=self.inner_dmac,
                                            ip_dst=self.customer1_ip,
                                            ip_src=self.customer2_ip,
                                            ip_id=108,
                                            ip_ttl=64)
            vxlan_pkt_2 = simple_vxlan_packet(eth_dst=ROUTER_MAC,
                                              eth_src=self.unbor1_mac,
                                              ip_dst=self.lpb_ip,
                                              ip_src=self.tun1_ip,
                                              ip_id=0,
                                              ip_ttl=64,
                                              ip_flags=0x2,
                                              with_udp_chksum=False,
                                              vxlan_vni=self.vni1,
                                              inner_frame=inner_pkt_2)

            print("Sending packet from VNI %d -> port %d" %
                  (self.vni1, self.oport1_dev))
            send_packet(self, self.uport1_dev, vxlan_pkt_2)
            verify_packet(self, pkt_2, self.oport1_dev)

        finally:
            sai_thrift_flush_fdb_entries(
                self.client, entry_type=SAI_FDB_FLUSH_ENTRY_TYPE_ALL)

            sai_thrift_remove_neighbor_entry(self.client, nbor)
            sai_thrift_remove_next_hop(self.client, tunnel_nhop_asym)
            if 'tun1_vlan_member' in locals() and tun1_vlan_member:
                sai_thrift_remove_vlan_member(self.client, tun1_vlan_member)
            if 'tun1_bp' in locals() and tun1_bp:
                sai_thrift_remove_bridge_port(self.client, tun1_bp)
            if 'tunnel1' in locals() and tunnel1:
                sai_thrift_remove_tunnel(self.client, tunnel1)
            sai_thrift_remove_tunnel_map_entry(self.client,
                                               decap_tunnel_map_entry_vlan1)
            sai_thrift_remove_tunnel_map_entry(self.client,
                                               decap_tunnel_map_entry_vlan10)
            sai_thrift_remove_tunnel_map_entry(self.client,
                                               decap_tunnel_map_entry_vlan20)
            sai_thrift_remove_tunnel_term_table_entry(self.client, tunnel_term)
            sai_thrift_remove_tunnel(self.client, tunnel0)
            sai_thrift_remove_tunnel_map(self.client, encap_tunnel_map_vlan)
            sai_thrift_remove_tunnel_map(self.client, decap_tunnel_map_vlan)
            sai_thrift_remove_router_interface(self.client, dummy_vlan_rif)
            sai_thrift_remove_vlan(self.client, dummy_vlan)

    def vxLanL3AsymmetricEcmpTest(self):
        '''
        This test verifies that packets are properly encapsulated
        in the case of L2 asymmetric forwarding to VxLAN. This test
        creates an ecmp group of two nexthops with type IP and RIFs of
        type VLAN, having a MAC_ENTRY with a destination handle of type
        TUNNEL to create proper configuration.
        '''
        print("\nvxLanL3AsymmetricEcmpTest()")

        tunnel_type = SAI_TUNNEL_TYPE_VXLAN
        peer_mode = SAI_TUNNEL_PEER_MODE_P2MP
        term_type = SAI_TUNNEL_TERM_TABLE_ENTRY_TYPE_P2MP

        overlay_nbor1_ip = "192.168.200.2"
        overlay_nbor1_mac = "00:22:22:22:22:33"

        overlay_nbor2_ip = "192.168.200.3"
        overlay_nbor2_mac = "00:22:22:22:22:44"

        try:
            # create dummy vlan for vrf
            dummy_vlan = sai_thrift_create_vlan(self.client,
                                                vlan_id=self.dummy_vlan_id)

            # create rif for dummy vlan
            dummy_vlan_rif = sai_thrift_create_router_interface(
                self.client,
                virtual_router_id=self.ovrf,
                src_mac_address=ROUTER_MAC,
                type=SAI_ROUTER_INTERFACE_TYPE_VLAN,
                vlan_id=dummy_vlan,
                mtu=9100,
                nat_zone_id=0)

            # create tunnel maps
            decap_tunnel_map_vlan = sai_thrift_create_tunnel_map(
                self.client, type=SAI_TUNNEL_MAP_TYPE_VNI_TO_VLAN_ID)

            encap_tunnel_map_vlan = sai_thrift_create_tunnel_map(
                self.client, type=SAI_TUNNEL_MAP_TYPE_VLAN_ID_TO_VNI)

            # create tunnel
            # Note: encap_maps order of vlan first then vrf exposes SWI-3916,
            #       with the reverse order the test passes even without a fix
            decap_maps = sai_thrift_object_list_t(
                count=1,
                idlist=[decap_tunnel_map_vlan])
            encap_maps = sai_thrift_object_list_t(
                count=1,
                idlist=[encap_tunnel_map_vlan])
            tunnel0 = sai_thrift_create_tunnel(
                self.client,
                type=tunnel_type,
                underlay_interface=self.urif_lpb,
                decap_mappers=decap_maps,
                encap_mappers=encap_maps,
                encap_src_ip=sai_ipaddress(self.lpb_ip),
                peer_mode=peer_mode)

            tunnel_term = sai_thrift_create_tunnel_term_table_entry(
                self.client,
                type=term_type,
                vr_id=self.default_vrf,
                dst_ip=sai_ipaddress(self.lpb_ip),
                tunnel_type=tunnel_type,
                action_tunnel_id=tunnel0)

            # create tunnel map entries for vlans
            decap_tunnel_map_entry_vlan1 = sai_thrift_create_tunnel_map_entry(
                self.client,
                tunnel_map_type=SAI_TUNNEL_MAP_TYPE_VNI_TO_VLAN_ID,
                tunnel_map=decap_tunnel_map_vlan,
                vlan_id_value=self.dummy_vlan_id,
                vni_id_key=self.vni_for_ovrf)

            decap_tunnel_map_entry_vlan10 = sai_thrift_create_tunnel_map_entry(
                self.client,
                tunnel_map_type=SAI_TUNNEL_MAP_TYPE_VNI_TO_VLAN_ID,
                tunnel_map=decap_tunnel_map_vlan,
                vlan_id_value=self.vlan10_id,
                vni_id_key=self.vni1)

            decap_tunnel_map_entry_vlan20 = sai_thrift_create_tunnel_map_entry(
                self.client,
                tunnel_map_type=SAI_TUNNEL_MAP_TYPE_VNI_TO_VLAN_ID,
                tunnel_map=decap_tunnel_map_vlan,
                vlan_id_value=self.vlan20_id,
                vni_id_key=self.vni2)

            # when first route comes down for a remote vtep,
            # p2p tunnel comes down first
            tunnel1 = sai_thrift_create_tunnel(
                self.client,
                type=tunnel_type,
                underlay_interface=self.urif_lpb,
                decap_mappers=decap_maps,
                encap_mappers=encap_maps,
                encap_src_ip=sai_ipaddress(self.lpb_ip),
                peer_mode=SAI_TUNNEL_PEER_MODE_P2P,
                encap_dst_ip=sai_ipaddress(self.tun1_ip))

            # verify p2p tunnel creation with peer_mode P2P
            attr = sai_thrift_get_tunnel_attribute(
                self.client, tunnel1, peer_mode=True, encap_dst_ip=True)
            self.assertEqual(attr['peer_mode'],
                             SAI_TUNNEL_PEER_MODE_P2P,
                             "p2p tunnel peer mode is not correct")
            self.assertEqual(attr['encap_dst_ip'].addr.ip4,
                             sai_ipaddress(self.tun1_ip).addr.ip4,
                             "p2p tunnel dst ip is not correct")

            # bridge port is created on p2p tunnel
            tun1_bp = sai_thrift_create_bridge_port(
                self.client,
                type=SAI_BRIDGE_PORT_TYPE_TUNNEL,
                tunnel_id=tunnel1,
                bridge_id=self.default_1q_bridge,
                admin_state=True,
                fdb_learning_mode=SAI_BRIDGE_PORT_FDB_LEARNING_MODE_DISABLE)

            # verify bridge port on p2p tunnel
            attr = sai_thrift_get_bridge_port_attribute(
                self.client, tun1_bp, tunnel_id=True, fdb_learning_mode=True)
            self.assertEqual(attr['tunnel_id'],
                             tunnel1,
                             "bridge port tunnel_id is not correct")
            self.assertEqual(attr['fdb_learning_mode'],
                             SAI_BRIDGE_PORT_FDB_LEARNING_MODE_DISABLE,
                             "bridge port fdb learning mode is not correct")

            # vlan_member is created using tunnel bridge_port
            tun1_vlan10_member = sai_thrift_create_vlan_member(
                self.client,
                vlan_id=self.vlan10,
                bridge_port_id=tun1_bp,
                vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)

            # verify vlan member on p2p tunnel
            attr = sai_thrift_get_vlan_member_attribute(
                self.client, tun1_vlan10_member, bridge_port_id=True)
            self.assertEqual(attr['bridge_port_id'],
                             tun1_bp,
                             "vlan member bridge_port_id is not correct")

            # vlan_member is created using tunnel bridge_port
            tun1_vlan20_member = sai_thrift_create_vlan_member(
                self.client,
                vlan_id=self.vlan20,
                bridge_port_id=tun1_bp,
                vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)

            # verify vlan member on p2p tunnel
            attr = sai_thrift_get_vlan_member_attribute(
                self.client, tun1_vlan20_member, bridge_port_id=True)
            self.assertEqual(attr['bridge_port_id'],
                             tun1_bp,
                             "vlan member bridge_port_id is not correct")

            tunnel2 = sai_thrift_create_tunnel(
                self.client,
                type=tunnel_type,
                underlay_interface=self.urif_lpb,
                decap_mappers=decap_maps,
                encap_mappers=encap_maps,
                encap_src_ip=sai_ipaddress(self.lpb_ip),
                peer_mode=SAI_TUNNEL_PEER_MODE_P2P,
                encap_dst_ip=sai_ipaddress(self.tun2_ip))

            # verify p2p tunnel creation with peer_mode P2P
            attr = sai_thrift_get_tunnel_attribute(
                self.client, tunnel2, peer_mode=True, encap_dst_ip=True)
            self.assertEqual(attr['peer_mode'],
                             SAI_TUNNEL_PEER_MODE_P2P,
                             "p2p tunnel peer mode is not correct")
            self.assertEqual(attr['encap_dst_ip'].addr.ip4,
                             sai_ipaddress(self.tun2_ip).addr.ip4,
                             "p2p tunnel dst ip is not correct")

            # bridge port is created on p2p tunnel
            tun2_bp = sai_thrift_create_bridge_port(
                self.client,
                type=SAI_BRIDGE_PORT_TYPE_TUNNEL,
                tunnel_id=tunnel2,
                bridge_id=self.default_1q_bridge,
                admin_state=True,
                fdb_learning_mode=SAI_BRIDGE_PORT_FDB_LEARNING_MODE_DISABLE)

            # verify bridge port on p2p tunnel
            attr = sai_thrift_get_bridge_port_attribute(
                self.client, tun2_bp, tunnel_id=True, fdb_learning_mode=True)
            self.assertEqual(attr['tunnel_id'],
                             tunnel2,
                             "bridge port tunnel_id is not correct")
            self.assertEqual(attr['fdb_learning_mode'],
                             SAI_BRIDGE_PORT_FDB_LEARNING_MODE_DISABLE,
                             "bridge port fdb learning mode is not correct")

            # vlan_member is created using tunnel bridge_port
            tun2_vlan10_member = sai_thrift_create_vlan_member(
                self.client,
                vlan_id=self.vlan10,
                bridge_port_id=tun2_bp,
                vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)

            # verify vlan member on p2p tunnel
            attr = sai_thrift_get_vlan_member_attribute(
                self.client, tun2_vlan10_member, bridge_port_id=True)
            self.assertEqual(attr['bridge_port_id'],
                             tun2_bp,
                             "vlan member bridge_port_id is not correct")

            # vlan_member is created using tunnel bridge_port
            tun2_vlan20_member = sai_thrift_create_vlan_member(
                self.client,
                vlan_id=self.vlan20,
                bridge_port_id=tun2_bp,
                vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)

            # verify vlan member on p2p tunnel
            attr = sai_thrift_get_vlan_member_attribute(
                self.client, tun2_vlan20_member, bridge_port_id=True)
            self.assertEqual(attr['bridge_port_id'],
                             tun2_bp,
                             "vlan member bridge_port_id is not correct")

            nbor = sai_thrift_neighbor_entry_t(
                rif_id=self.vlan20_rif,
                ip_address=sai_ipaddress(overlay_nbor1_ip))
            sai_thrift_create_neighbor_entry(
                self.client,
                nbor,
                dst_mac_address=overlay_nbor1_mac,
                no_host_route=True)

            tunnel_nhop_asym1 = sai_thrift_create_next_hop(
                self.client,
                type=SAI_NEXT_HOP_TYPE_IP,
                ip=sai_ipaddress(overlay_nbor1_ip),
                router_interface_id=self.vlan20_rif)

            nbor2 = sai_thrift_neighbor_entry_t(
                rif_id=self.vlan20_rif,
                ip_address=sai_ipaddress(overlay_nbor2_ip))
            sai_thrift_create_neighbor_entry(
                self.client,
                nbor2,
                dst_mac_address=overlay_nbor2_mac,
                no_host_route=True)

            tunnel_nhop_asym2 = sai_thrift_create_next_hop(
                self.client,
                type=SAI_NEXT_HOP_TYPE_IP,
                ip=sai_ipaddress(overlay_nbor2_ip),
                router_interface_id=self.vlan20_rif)

            fdb_entry = sai_thrift_fdb_entry_t(
                bv_id=self.vlan20,
                mac_address=overlay_nbor1_mac,
                switch_id=self.switch_id)
            sai_thrift_create_fdb_entry(
                self.client,
                fdb_entry,
                type=SAI_FDB_ENTRY_TYPE_STATIC,
                bridge_port_id=tun1_bp,
                endpoint_ip=sai_ipaddress(self.tun1_ip))

            fdb_entry2 = sai_thrift_fdb_entry_t(
                bv_id=self.vlan20,
                mac_address=overlay_nbor2_mac,
                switch_id=self.switch_id)
            sai_thrift_create_fdb_entry(
                self.client,
                fdb_entry2,
                type=SAI_FDB_ENTRY_TYPE_STATIC,
                bridge_port_id=tun2_bp,
                endpoint_ip=sai_ipaddress(self.tun2_ip))

            tunnel_ecmp = sai_thrift_create_next_hop_group(
                self.client, type=SAI_NEXT_HOP_GROUP_TYPE_ECMP)

            tunnel_ecmp_member1 = sai_thrift_create_next_hop_group_member(
                self.client,
                next_hop_group_id=tunnel_ecmp,
                next_hop_id=tunnel_nhop_asym1)

            customer2_route = sai_thrift_route_entry_t(
                vr_id=self.ovrf,
                destination=sai_ipprefix(self.customer2_ip + '/32'))
            sai_thrift_create_route_entry(self.client,
                                          customer2_route,
                                          next_hop_id=tunnel_ecmp)

            pkt = simple_udp_packet(eth_dst=ROUTER_MAC,
                                    eth_src=self.customer1_mac,
                                    ip_dst=self.customer2_ip,
                                    ip_src=self.customer1_ip,
                                    ip_id=108,
                                    ip_ttl=65)
            inner_pkt_1 = simple_udp_packet(eth_dst=overlay_nbor1_mac,
                                            eth_src=ROUTER_MAC,
                                            ip_dst=self.customer2_ip,
                                            ip_src=self.customer1_ip,
                                            ip_id=108,
                                            ip_ttl=64)
            inner_pkt_2 = simple_udp_packet(eth_dst=overlay_nbor2_mac,
                                            eth_src=ROUTER_MAC,
                                            ip_dst=self.customer2_ip,
                                            ip_src=self.customer1_ip,
                                            ip_id=108,
                                            ip_ttl=64)
            vxlan_pkt_1 = Mask(
                simple_vxlan_packet(eth_dst=self.unbor1_mac,
                                    eth_src=ROUTER_MAC,
                                    ip_dst=self.tun1_ip,
                                    ip_src=self.lpb_ip,
                                    ip_id=0,
                                    ip_ttl=64,
                                    ip_flags=0x2,
                                    with_udp_chksum=False,
                                    vxlan_vni=self.vni2,
                                    inner_frame=inner_pkt_1))
            vxlan_pkt_1.set_do_not_care_scapy(UDP, 'sport')

            vxlan_pkt_2 = Mask(
                simple_vxlan_packet(eth_dst=self.unbor2_mac,
                                    eth_src=ROUTER_MAC,
                                    ip_dst=self.tun2_ip,
                                    ip_src=self.lpb_ip,
                                    ip_id=0,
                                    ip_ttl=64,
                                    ip_flags=0x2,
                                    with_udp_chksum=False,
                                    vxlan_vni=self.vni2,
                                    inner_frame=inner_pkt_2))
            vxlan_pkt_2.set_do_not_care_scapy(UDP, 'sport')

            print("Sending packet from port %d -> VNI %d" %
                  (self.oport1_dev, self.vni2))
            send_packet(self, self.oport1_dev, pkt)
            verify_packet(self, vxlan_pkt_1, self.uport1_dev)

            print("\nAdding additional member to the ecmp group")
            tunnel_ecmp_member2 = sai_thrift_create_next_hop_group_member(
                self.client,
                next_hop_group_id=tunnel_ecmp,
                next_hop_id=tunnel_nhop_asym2)

            customer1_ip_list = []
            itrs = 30
            count = [0, 0]
            for i in range(0, itrs):
                customer1_ip_list.append("192.168.100.%d" % (i + 1))

                pkt = simple_udp_packet(eth_dst=ROUTER_MAC,
                                        eth_src=self.customer1_mac,
                                        ip_dst=self.customer2_ip,
                                        ip_src=customer1_ip_list[i],
                                        ip_id=108,
                                        ip_ttl=65)
                inner_pkt_1 = simple_udp_packet(eth_dst=overlay_nbor1_mac,
                                                eth_src=ROUTER_MAC,
                                                ip_dst=self.customer2_ip,
                                                ip_src=customer1_ip_list[i],
                                                ip_id=108,
                                                ip_ttl=64)
                inner_pkt_2 = simple_udp_packet(eth_dst=overlay_nbor2_mac,
                                                eth_src=ROUTER_MAC,
                                                ip_dst=self.customer2_ip,
                                                ip_src=customer1_ip_list[i],
                                                ip_id=108,
                                                ip_ttl=64)
                vxlan_pkt_1 = Mask(
                    simple_vxlan_packet(eth_dst=self.unbor1_mac,
                                        eth_src=ROUTER_MAC,
                                        ip_dst=self.tun1_ip,
                                        ip_src=self.lpb_ip,
                                        ip_id=0,
                                        ip_ttl=64,
                                        ip_flags=0x2,
                                        with_udp_chksum=False,
                                        vxlan_vni=self.vni2,
                                        inner_frame=inner_pkt_1))
                vxlan_pkt_1.set_do_not_care_scapy(UDP, 'sport')

                vxlan_pkt_2 = Mask(
                    simple_vxlan_packet(eth_dst=self.unbor2_mac,
                                        eth_src=ROUTER_MAC,
                                        ip_dst=self.tun2_ip,
                                        ip_src=self.lpb_ip,
                                        ip_id=0,
                                        ip_ttl=64,
                                        ip_flags=0x2,
                                        with_udp_chksum=False,
                                        vxlan_vni=self.vni2,
                                        inner_frame=inner_pkt_2))
                vxlan_pkt_2.set_do_not_care_scapy(UDP, 'sport')

                print("Sending packet from port %d -> VNI %d" %
                      (self.oport1_dev, self.vni2))
                send_packet(self, self.oport1_dev, pkt)
                rcv_idx = verify_any_packet_any_port(
                    self, [vxlan_pkt_1, vxlan_pkt_2],
                    [self.uport1_dev, self.uport2_dev])
                count[rcv_idx] += 1

            for i in range(0, 2):
                self.assertTrue(count[i] > (itrs / 2) * 0.5)

            print("\nRemoving the original member from the ecmp group")
            sai_thrift_remove_next_hop_group_member(self.client,
                                                    tunnel_ecmp_member1)

            print("Sending packet from port %d -> VNI %d" %
                  (self.oport1_dev, self.vni2))
            send_packet(self, self.oport1_dev, pkt)
            verify_packet(self, vxlan_pkt_2, self.uport2_dev)

        finally:
            sai_thrift_remove_next_hop_group_member(self.client,
                                                    tunnel_ecmp_member2)
            sai_thrift_remove_route_entry(self.client, customer2_route)
            sai_thrift_remove_next_hop_group(self.client, tunnel_ecmp)
            sai_thrift_remove_fdb_entry(self.client, fdb_entry2)
            sai_thrift_remove_fdb_entry(self.client, fdb_entry)
            sai_thrift_remove_next_hop(self.client, tunnel_nhop_asym2)
            sai_thrift_remove_neighbor_entry(self.client, nbor2)
            sai_thrift_remove_next_hop(self.client, tunnel_nhop_asym1)
            sai_thrift_remove_neighbor_entry(self.client, nbor)
            if 'tun2_vlan20_member' in locals() and tun2_vlan20_member:
                sai_thrift_remove_vlan_member(self.client, tun2_vlan20_member)
            if 'tun2_vlan10_member' in locals() and tun2_vlan10_member:
                sai_thrift_remove_vlan_member(self.client, tun2_vlan10_member)
            if 'tun2_bp' in locals() and tun2_bp:
                sai_thrift_remove_bridge_port(self.client, tun2_bp)
            if 'tunnel2' in locals() and tunnel2:
                sai_thrift_remove_tunnel(self.client, tunnel2)
            if 'tun1_vlan20_member' in locals() and tun1_vlan20_member:
                sai_thrift_remove_vlan_member(self.client, tun1_vlan20_member)
            if 'tun1_vlan10_member' in locals() and tun1_vlan10_member:
                sai_thrift_remove_vlan_member(self.client, tun1_vlan10_member)
            if 'tun1_bp' in locals() and tun1_bp:
                sai_thrift_remove_bridge_port(self.client, tun1_bp)
            if 'tunnel1' in locals() and tunnel1:
                sai_thrift_remove_tunnel(self.client, tunnel1)
            sai_thrift_remove_tunnel_map_entry(self.client,
                                               decap_tunnel_map_entry_vlan1)
            sai_thrift_remove_tunnel_map_entry(self.client,
                                               decap_tunnel_map_entry_vlan10)
            sai_thrift_remove_tunnel_map_entry(self.client,
                                               decap_tunnel_map_entry_vlan20)
            sai_thrift_remove_tunnel_term_table_entry(self.client, tunnel_term)
            sai_thrift_remove_tunnel(self.client, tunnel0)
            sai_thrift_remove_tunnel_map(self.client, encap_tunnel_map_vlan)
            sai_thrift_remove_tunnel_map(self.client, decap_tunnel_map_vlan)
            sai_thrift_remove_router_interface(self.client, dummy_vlan_rif)
            sai_thrift_remove_vlan(self.client, dummy_vlan)

    def vxLanL3AsymmetricMacMoveTest(self):
        '''
        This test verifies that packets are properly encapsulated and
        decapsulated in case of L2 asymmetric forwarding to VxLAN.
        This test uses nexthop of type IP with RIF of type VLAN, having
        a MAC_ENTRY with a destination handle of type TUNNEL to create
        proper configuration.
        Test also verifies if packet is properly forwarded when mac entry
        is moved from tunnel to local and then back to tunnel.
        '''
        print("\nvxLanL3AsymmetricMacMoveTest()")

        tunnel_type = SAI_TUNNEL_TYPE_VXLAN
        peer_mode = SAI_TUNNEL_PEER_MODE_P2MP
        term_type = SAI_TUNNEL_TERM_TABLE_ENTRY_TYPE_P2MP

        try:
            # create dummy vlan for vrf
            dummy_vlan = sai_thrift_create_vlan(self.client,
                                                vlan_id=self.dummy_vlan_id)

            # create rif for dummy vlan
            dummy_vlan_rif = sai_thrift_create_router_interface(
                self.client,
                virtual_router_id=self.ovrf,
                src_mac_address=ROUTER_MAC,
                type=SAI_ROUTER_INTERFACE_TYPE_VLAN,
                vlan_id=dummy_vlan,
                mtu=9100,
                nat_zone_id=0)

            # create tunnel maps
            decap_tunnel_map_vlan = sai_thrift_create_tunnel_map(
                self.client, type=SAI_TUNNEL_MAP_TYPE_VNI_TO_VLAN_ID)

            encap_tunnel_map_vlan = sai_thrift_create_tunnel_map(
                self.client, type=SAI_TUNNEL_MAP_TYPE_VLAN_ID_TO_VNI)

            # create tunnel
            # Note: encap_maps order of vlan first then vrf exposes SWI-3916,
            #       with the reverse order the test passes even without a fix
            decap_maps = sai_thrift_object_list_t(
                count=1,
                idlist=[decap_tunnel_map_vlan])
            encap_maps = sai_thrift_object_list_t(
                count=1,
                idlist=[encap_tunnel_map_vlan])
            tunnel0 = sai_thrift_create_tunnel(
                self.client,
                type=tunnel_type,
                underlay_interface=self.urif_lpb,
                decap_mappers=decap_maps,
                encap_mappers=encap_maps,
                encap_src_ip=sai_ipaddress(self.lpb_ip),
                peer_mode=peer_mode)

            tunnel_term = sai_thrift_create_tunnel_term_table_entry(
                self.client,
                type=term_type,
                vr_id=self.default_vrf,
                dst_ip=sai_ipaddress(self.lpb_ip),
                tunnel_type=tunnel_type,
                action_tunnel_id=tunnel0)

            # create tunnel map entries for vlans
            decap_tunnel_map_entry_vlan1 = sai_thrift_create_tunnel_map_entry(
                self.client,
                tunnel_map_type=SAI_TUNNEL_MAP_TYPE_VNI_TO_VLAN_ID,
                tunnel_map=decap_tunnel_map_vlan,
                vlan_id_value=self.dummy_vlan_id,
                vni_id_key=self.vni_for_ovrf)

            decap_tunnel_map_entry_vlan10 = sai_thrift_create_tunnel_map_entry(
                self.client,
                tunnel_map_type=SAI_TUNNEL_MAP_TYPE_VNI_TO_VLAN_ID,
                tunnel_map=decap_tunnel_map_vlan,
                vlan_id_value=self.vlan10_id,
                vni_id_key=self.vni1)

            decap_tunnel_map_entry_vlan20 = sai_thrift_create_tunnel_map_entry(
                self.client,
                tunnel_map_type=SAI_TUNNEL_MAP_TYPE_VNI_TO_VLAN_ID,
                tunnel_map=decap_tunnel_map_vlan,
                vlan_id_value=self.vlan20_id,
                vni_id_key=self.vni2)

            # when first route comes down for a remote vtep,
            # p2p tunnel comes down first
            tunnel1 = sai_thrift_create_tunnel(
                self.client,
                type=tunnel_type,
                underlay_interface=self.urif_lpb,
                decap_mappers=decap_maps,
                encap_mappers=encap_maps,
                encap_src_ip=sai_ipaddress(self.lpb_ip),
                peer_mode=SAI_TUNNEL_PEER_MODE_P2P,
                encap_dst_ip=sai_ipaddress(self.tun1_ip))

            # verify p2p tunnel creation with peer_mode P2P
            attr = sai_thrift_get_tunnel_attribute(
                self.client, tunnel1, peer_mode=True, encap_dst_ip=True)
            self.assertEqual(attr['peer_mode'],
                             SAI_TUNNEL_PEER_MODE_P2P,
                             "p2p tunnel peer mode is not correct")
            self.assertEqual(attr['encap_dst_ip'].addr.ip4,
                             sai_ipaddress(self.tun1_ip).addr.ip4,
                             "p2p tunnel dst ip is not correct")

            # bridge port is created on p2p tunnel
            tun1_bp = sai_thrift_create_bridge_port(
                self.client,
                type=SAI_BRIDGE_PORT_TYPE_TUNNEL,
                tunnel_id=tunnel1,
                bridge_id=self.default_1q_bridge,
                admin_state=True,
                fdb_learning_mode=SAI_BRIDGE_PORT_FDB_LEARNING_MODE_DISABLE)

            # verify bridge port on p2p tunnel
            attr = sai_thrift_get_bridge_port_attribute(
                self.client, tun1_bp, tunnel_id=True, fdb_learning_mode=True)
            self.assertEqual(attr['tunnel_id'],
                             tunnel1,
                             "bridge port tunnel_id is not correct")
            self.assertEqual(attr['fdb_learning_mode'],
                             SAI_BRIDGE_PORT_FDB_LEARNING_MODE_DISABLE,
                             "bridge port fdb learning mode is not correct")

            # vlan_member is created using tunnel bridge_port
            tun1_vlan_member = sai_thrift_create_vlan_member(
                self.client,
                vlan_id=self.vlan10,
                bridge_port_id=tun1_bp,
                vlan_tagging_mode=SAI_VLAN_TAGGING_MODE_UNTAGGED)

            # verify vlan member on p2p tunnel
            attr = sai_thrift_get_vlan_member_attribute(
                self.client, tun1_vlan_member, bridge_port_id=True)
            self.assertEqual(attr['bridge_port_id'],
                             tun1_bp,
                             "vlan member bridge_port_id is not correct")

            nbor = sai_thrift_neighbor_entry_t(
                rif_id=self.vlan20_rif,
                ip_address=sai_ipaddress(self.customer2_ip))
            sai_thrift_create_neighbor_entry(
                self.client, nbor, dst_mac_address=self.customer2_mac)

            tunnel_nhop_asym = sai_thrift_create_next_hop(
                self.client,
                type=SAI_NEXT_HOP_TYPE_IP,
                ip=sai_ipaddress(self.customer2_ip),
                router_interface_id=self.vlan20_rif)

            fdb_entry = sai_thrift_fdb_entry_t(
                bv_id=self.vlan20,
                mac_address=self.customer2_mac,
                switch_id=self.switch_id)
            sai_thrift_create_fdb_entry(
                self.client,
                fdb_entry,
                type=SAI_FDB_ENTRY_TYPE_STATIC,
                bridge_port_id=tun1_bp,
                endpoint_ip=sai_ipaddress(self.tun1_ip))

            pkt = simple_udp_packet(eth_dst=ROUTER_MAC,
                                    eth_src=self.customer1_mac,
                                    ip_dst=self.customer2_ip,
                                    ip_src=self.customer1_ip,
                                    ip_id=108,
                                    ip_ttl=65,
                                    pktlen=100)
            inner_pkt = simple_udp_packet(eth_dst=self.customer2_mac,
                                          eth_src=ROUTER_MAC,
                                          ip_dst=self.customer2_ip,
                                          ip_src=self.customer1_ip,
                                          ip_id=108,
                                          ip_ttl=64)
            vxlan_pkt = Mask(
                simple_vxlan_packet(eth_dst=self.unbor1_mac,
                                    eth_src=ROUTER_MAC,
                                    ip_dst=self.tun1_ip,
                                    ip_src=self.lpb_ip,
                                    ip_id=0,
                                    ip_ttl=64,
                                    ip_flags=0x2,
                                    with_udp_chksum=False,
                                    vxlan_vni=self.vni2,
                                    inner_frame=inner_pkt))
            vxlan_pkt.set_do_not_care_scapy(UDP, 'sport')

            print("Sending packet from port %d -> VNI %d" %
                  (self.oport1_dev, self.vni2))
            send_packet(self, self.oport1_dev, pkt)
            verify_packet(self, vxlan_pkt, self.uport1_dev)

            pkt_2 = simple_udp_packet(eth_dst=self.customer1_mac,
                                      eth_src=self.inner_dmac,
                                      ip_dst=self.customer1_ip,
                                      ip_src=self.customer2_ip,
                                      ip_id=108,
                                      ip_ttl=64)
            inner_pkt_2 = simple_udp_packet(eth_dst=self.customer1_mac,
                                            eth_src=self.inner_dmac,
                                            ip_dst=self.customer1_ip,
                                            ip_src=self.customer2_ip,
                                            ip_id=108,
                                            ip_ttl=64)
            vxlan_pkt_2 = simple_vxlan_packet(eth_dst=ROUTER_MAC,
                                              eth_src=self.unbor1_mac,
                                              ip_dst=self.lpb_ip,
                                              ip_src=self.tun1_ip,
                                              ip_id=0,
                                              ip_ttl=64,
                                              ip_flags=0x2,
                                              with_udp_chksum=False,
                                              vxlan_vni=self.vni1,
                                              inner_frame=inner_pkt_2)

            print("Sending packet from VNI %d -> port %d" %
                  (self.vni1, self.oport1_dev))
            send_packet(self, self.uport1_dev, vxlan_pkt_2)
            verify_packet(self, pkt_2, self.oport1_dev)

            sai_thrift_set_fdb_entry_attribute(
                self.client,
                fdb_entry,
                bridge_port_id=self.port1_bp)

            attr = sai_thrift_get_fdb_entry_attribute(
                self.client,
                fdb_entry,
                endpoint_ip=True)
            zeroed_ip = sai_thrift_ip_address_t(
                addr_family=0,
                addr=sai_thrift_ip_addr_t(ip4='0.0.0.0', ip6=''))
            self.assertEqual(attr['endpoint_ip'], zeroed_ip)

            inner_pkt_vlan = simple_udp_packet(eth_dst=self.customer2_mac,
                                               eth_src=ROUTER_MAC,
                                               ip_dst=self.customer2_ip,
                                               ip_src=self.customer1_ip,
                                               dl_vlan_enable=True,
                                               vlan_vid=20,
                                               ip_id=108,
                                               ip_ttl=64,
                                               pktlen=104)
            print("Sending packet from port %d -> port %d" %
                  (self.oport1_dev, self.dev_port1))
            send_packet(self, self.oport1_dev, pkt)
            verify_packet(self, inner_pkt_vlan, self.dev_port1)

            sai_thrift_set_fdb_entry_attribute(
                self.client,
                fdb_entry,
                bridge_port_id=tun1_bp)

            sai_thrift_set_fdb_entry_attribute(
                self.client,
                fdb_entry,
                endpoint_ip=sai_ipaddress(self.tun1_ip))

            print("Sending packet from port %d -> VNI %d" %
                  (self.oport1_dev, self.vni2))
            send_packet(self, self.oport1_dev, pkt)
            verify_packet(self, vxlan_pkt, self.uport1_dev)

            print("Sending packet from VNI %d -> port %d" %
                  (self.vni1, self.oport1_dev))
            send_packet(self, self.uport1_dev, vxlan_pkt_2)
            verify_packet(self, pkt_2, self.oport1_dev)

        finally:
            sai_thrift_flush_fdb_entries(
                self.client, entry_type=SAI_FDB_FLUSH_ENTRY_TYPE_ALL)

            sai_thrift_remove_neighbor_entry(self.client, nbor)
            sai_thrift_remove_next_hop(self.client, tunnel_nhop_asym)
            if 'tun1_vlan_member' in locals() and tun1_vlan_member:
                sai_thrift_remove_vlan_member(self.client, tun1_vlan_member)
            if 'tun1_bp' in locals() and tun1_bp:
                sai_thrift_remove_bridge_port(self.client, tun1_bp)
            if 'tunnel1' in locals() and tunnel1:
                sai_thrift_remove_tunnel(self.client, tunnel1)
            sai_thrift_remove_tunnel_map_entry(self.client,
                                               decap_tunnel_map_entry_vlan1)
            sai_thrift_remove_tunnel_map_entry(self.client,
                                               decap_tunnel_map_entry_vlan10)
            sai_thrift_remove_tunnel_map_entry(self.client,
                                               decap_tunnel_map_entry_vlan20)
            sai_thrift_remove_tunnel_term_table_entry(self.client, tunnel_term)
            sai_thrift_remove_tunnel(self.client, tunnel0)
            sai_thrift_remove_tunnel_map(self.client, encap_tunnel_map_vlan)
            sai_thrift_remove_tunnel_map(self.client, decap_tunnel_map_vlan)
            sai_thrift_remove_router_interface(self.client, dummy_vlan_rif)
            sai_thrift_remove_vlan(self.client, dummy_vlan)
