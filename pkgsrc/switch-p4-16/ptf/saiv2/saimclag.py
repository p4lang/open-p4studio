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


'''
Thrift SAI interface MCLAG tests
'''

from sai_base_test import *


class MclagTest(SaiHelper):
    '''
    Basic MCLAG test class
    '''

    def runTest(self):
        try:
            self.peerSrcMacAddressAttr()
            self.peerSrcMacAddressAttrSvi()
            self.peerSrcMacAddressAttrSubPort()
            self.srcMacAddressRifUpdateAttr()
            self.srcMacAddressRifUpdateAttrSvi()
            self.srcMacAddressRifUpdateAttrSubPort()
            self.anycastMacSupportAttr()
            self.anycastMacSupportSetAttr()
            self.anycastMacSupportSetSrcMacAttr()
            self.anycastMacSupportRemoveAnycastRif()
            self.anycastMacSupportAttrSvi()
            self.anycastMacSupportSetAttrSvi()
            self.anycastMacSupportSetSrcMacAttrSvi()
            self.anycastMacSupportAttrSubPort()
            self.anycastMacSupportSetAttrSubPort()
            self.anycastMacSupportSetSrcMacAttrSubPort()
        finally:
            pass

    def peerSrcMacAddressAttr(self):
        '''
        Verify if packets are forwarded if coming on ingress with
        SAI_ROUTER_INTERFACE_ATTR_PEER_SRC_MAC_ADDRESS as dst_addr
        for L3 Interface RIF
        '''
        print("peerSrcMacAddressAttr()")

        dmac1 = '00:11:22:33:44:55'
        peer_src_mac = '00:22:22:33:44:55'

        nhop = sai_thrift_create_next_hop(
            self.client,
            ip=sai_ipaddress('10.10.10.1'),
            router_interface_id=self.port10_rif,
            type=SAI_NEXT_HOP_TYPE_IP)
        neighbor_entry = sai_thrift_neighbor_entry_t(
            rif_id=self.port10_rif, ip_address=sai_ipaddress('10.10.10.1'))
        sai_thrift_create_neighbor_entry(
            self.client, neighbor_entry, dst_mac_address=dmac1)

        route_entry = sai_thrift_route_entry_t(
            vr_id=self.default_vrf, destination=sai_ipprefix('10.10.10.1/32'))
        sai_thrift_create_route_entry(
            self.client, route_entry, next_hop_id=nhop)

        pkt1 = simple_tcp_packet(
            eth_dst=ROUTER_MAC,
            eth_src='00:22:22:22:22:22',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        pkt2 = simple_tcp_packet(
            eth_dst=peer_src_mac,
            eth_src='00:22:22:22:22:22',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        exp_pkt = simple_tcp_packet(
            eth_dst=dmac1,
            eth_src=ROUTER_MAC,
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63)

        try:
            print("Sending packet on port %d, forward" % self.dev_port11)
            send_packet(self, self.dev_port11, pkt1)
            verify_packet(self, exp_pkt, self.dev_port10)

            print("Sending packet on port %d, drop "
                  "(eth_dst=%s)" % (self.dev_port11, peer_src_mac))
            send_packet(self, self.dev_port11, pkt2)
            verify_no_other_packets(self, timeout=2)

            print("Setting peer_src_mac for rif on port %d to %s"
                  % (self.dev_port11, peer_src_mac))
            attr_value = sai_thrift_attribute_value_t(mac=peer_src_mac)
            attr = sai_thrift_attribute_t(
                id=(SAI_ROUTER_INTERFACE_ATTR_END + 5), value=attr_value)
            sai_thrift_set_router_interface_attribute(
                self.client,
                self.port11_rif,
                custom_attribute=attr)

            print("Sending packet on port %d, forward (eth_dst=%s)" %
                  (self.dev_port11, peer_src_mac))
            send_packet(self, self.dev_port11, pkt2)
            verify_packet(self, exp_pkt, self.dev_port10)

        finally:
            sai_thrift_remove_route_entry(self.client, route_entry)
            sai_thrift_remove_neighbor_entry(self.client, neighbor_entry)
            sai_thrift_remove_next_hop(self.client, nhop)
            attr_value = sai_thrift_attribute_value_t(mac="00:00:00:00:00:00")
            attr = sai_thrift_attribute_t(
                id=(SAI_ROUTER_INTERFACE_ATTR_END + 5), value=attr_value)
            sai_thrift_set_router_interface_attribute(
                self.client,
                self.port11_rif,
                custom_attribute=attr)

    def peerSrcMacAddressAttrSvi(self):
        '''
        Verify if packets are forwarded if coming on ingress with
        SAI_ROUTER_INTERFACE_ATTR_PEER_SRC_MAC_ADDRESS as dst_addr
        for SVI RIF
        '''
        print("peerSrcMacAddressAttrSvi()")

        dmac1 = '00:11:22:33:44:55'
        dmac2 = '00:11:22:33:44:66'
        peer_src_mac = '00:22:22:33:44:55'

        nhop1 = sai_thrift_create_next_hop(
            self.client,
            ip=sai_ipaddress('10.10.10.2'),
            router_interface_id=self.port10_rif,
            type=SAI_NEXT_HOP_TYPE_IP)
        neighbor_entry1 = sai_thrift_neighbor_entry_t(
            rif_id=self.port10_rif, ip_address=sai_ipaddress('10.10.10.2'))
        sai_thrift_create_neighbor_entry(
            self.client, neighbor_entry1, dst_mac_address=dmac1)

        route_entry1 = sai_thrift_route_entry_t(
            vr_id=self.default_vrf, destination=sai_ipprefix('10.10.10.1/32'))
        sai_thrift_create_route_entry(
            self.client, route_entry1, next_hop_id=nhop1)

        nhop2 = sai_thrift_create_next_hop(
            self.client,
            ip=sai_ipaddress('30.10.10.1'),
            router_interface_id=self.vlan30_rif,
            type=SAI_NEXT_HOP_TYPE_IP)
        neighbor_entry2 = sai_thrift_neighbor_entry_t(
            rif_id=self.vlan30_rif, ip_address=sai_ipaddress('30.10.10.1'))
        sai_thrift_create_neighbor_entry(
            self.client, neighbor_entry2, dst_mac_address=dmac2)

        route_entry2 = sai_thrift_route_entry_t(
            vr_id=self.default_vrf, destination=sai_ipprefix('30.10.10.1/32'))
        sai_thrift_create_route_entry(
            self.client, route_entry2, next_hop_id=nhop2)

        pkt1 = simple_tcp_packet(
            eth_dst=ROUTER_MAC,
            eth_src='00:22:22:22:22:22',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        pkt2 = simple_tcp_packet(
            eth_dst=peer_src_mac,
            eth_src='00:22:22:22:22:22',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        pkt3 = simple_tcp_packet(
            eth_dst=ROUTER_MAC,
            eth_src='00:22:22:22:22:22',
            ip_dst='30.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        vlan_pkt1 = simple_tcp_packet(
            eth_dst=ROUTER_MAC,
            eth_src='00:22:22:22:22:22',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            dl_vlan_enable=True,
            vlan_vid=30,
            ip_id=105,
            ip_ttl=64,
            pktlen=104)
        vlan_pkt2 = simple_tcp_packet(
            eth_dst=peer_src_mac,
            eth_src='00:22:22:22:22:22',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            dl_vlan_enable=True,
            vlan_vid=30,
            ip_id=105,
            ip_ttl=64,
            pktlen=104)
        exp_pkt1 = simple_tcp_packet(
            eth_dst=dmac1,
            eth_src=ROUTER_MAC,
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63,
            pktlen=100)
        exp_pkt2 = simple_tcp_packet(
            eth_dst=dmac2,
            eth_src=ROUTER_MAC,
            ip_dst='30.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63,
            pktlen=100)
        try:
            print("Sending tagged packet on port %d, forward" %
                  self.dev_port21)
            send_packet(self, self.dev_port21, vlan_pkt1)
            verify_packet(self, exp_pkt1, self.dev_port10)

            print("Sending untagged packet on port %d, forward" %
                  self.dev_port20)
            send_packet(self, self.dev_port20, pkt1)
            verify_packet(self, exp_pkt1, self.dev_port10)

            print("Setting peer_src_mac for SVI 30 to %s" % peer_src_mac)
            attr_value = sai_thrift_attribute_value_t(mac=peer_src_mac)
            attr = sai_thrift_attribute_t(
                id=(SAI_ROUTER_INTERFACE_ATTR_END + 5), value=attr_value)
            sai_thrift_set_router_interface_attribute(
                self.client,
                self.vlan30_rif,
                custom_attribute=attr)

            print("Sending tagged packet on port %d, forward "
                  "(eth_dst=%s)" % (self.dev_port20, peer_src_mac))
            send_packet(self, self.dev_port21, vlan_pkt2)
            verify_packet(self, exp_pkt1, self.dev_port10)

            print("Sending untagged packet on port %d, forward "
                  "(eth_dst=%s)" % (self.dev_port20, peer_src_mac))
            send_packet(self, self.dev_port20, pkt2)
            verify_packet(self, exp_pkt1, self.dev_port10)

            fdb_entry = sai_thrift_fdb_entry_t(
                switch_id=self.switch_id,
                mac_address=dmac2,
                bv_id=self.vlan30)
            sai_thrift_create_fdb_entry(
                self.client,
                fdb_entry,
                type=SAI_FDB_ENTRY_TYPE_STATIC,
                bridge_port_id=self.port20_bp,
                packet_action=SAI_PACKET_ACTION_FORWARD)

            print("Sending packet untagged on port %d, forward" %
                  self.dev_port10)
            send_packet(self, self.dev_port10, pkt3)
            verify_packet(self, exp_pkt2, self.dev_port20)

        finally:
            sai_thrift_remove_fdb_entry(self.client, fdb_entry)
            sai_thrift_remove_route_entry(self.client, route_entry2)
            sai_thrift_remove_neighbor_entry(self.client, neighbor_entry2)
            sai_thrift_remove_next_hop(self.client, nhop2)
            sai_thrift_remove_route_entry(self.client, route_entry1)
            sai_thrift_remove_neighbor_entry(self.client, neighbor_entry1)
            sai_thrift_remove_next_hop(self.client, nhop1)
            attr_value = sai_thrift_attribute_value_t(mac="00:00:00:00:00:00")
            attr = sai_thrift_attribute_t(
                id=(SAI_ROUTER_INTERFACE_ATTR_END + 5), value=attr_value)
            sai_thrift_set_router_interface_attribute(
                self.client,
                self.vlan30_rif,
                custom_attribute=attr)

    def peerSrcMacAddressAttrSubPort(self):
        '''
        Verify if packets are forwarded if coming on ingress with
        SAI_ROUTER_INTERFACE_ATTR_PEER_SRC_MAC_ADDRESS as dst_addr
        for Sub Port RIF
        '''
        print("peerSrcMacAddressAttrSubPort()")

        peer_src_mac = '00:22:22:33:44:55'

        subport10_100 = sai_thrift_create_router_interface(
            self.client,
            type=SAI_ROUTER_INTERFACE_TYPE_SUB_PORT,
            virtual_router_id=self.default_vrf,
            port_id=self.port10,
            outer_vlan_id=100)
        nhop_sp10_100 = sai_thrift_create_next_hop(
            self.client,
            ip=sai_ipaddress('20.20.0.10'),
            router_interface_id=subport10_100,
            type=SAI_NEXT_HOP_TYPE_IP)
        nbr_entry_sp10_100 = sai_thrift_neighbor_entry_t(
            rif_id=subport10_100,
            ip_address=sai_ipaddress('20.20.0.10'))
        sai_thrift_create_neighbor_entry(
            self.client,
            nbr_entry_sp10_100,
            dst_mac_address='00:33:33:33:01:00')
        route_entry_sp10_100 = sai_thrift_route_entry_t(
            vr_id=self.default_vrf,
            destination=sai_ipprefix('30.30.0.10/32'))
        sai_thrift_create_route_entry(
            self.client,
            route_entry_sp10_100,
            next_hop_id=nhop_sp10_100)

        subport10_200 = sai_thrift_create_router_interface(
            self.client,
            type=SAI_ROUTER_INTERFACE_TYPE_SUB_PORT,
            virtual_router_id=self.default_vrf,
            port_id=self.port10,
            outer_vlan_id=200)
        nhop_sp10_200 = sai_thrift_create_next_hop(
            self.client,
            ip=sai_ipaddress('20.20.0.20'),
            router_interface_id=subport10_200,
            type=SAI_NEXT_HOP_TYPE_IP)
        nbr_entry_sp10_200 = sai_thrift_neighbor_entry_t(
            rif_id=subport10_200,
            ip_address=sai_ipaddress('20.20.0.20'))
        sai_thrift_create_neighbor_entry(
            self.client,
            nbr_entry_sp10_200,
            dst_mac_address='00:33:33:33:02:00')
        route_entry_sp10_200 = sai_thrift_route_entry_t(
            vr_id=self.default_vrf,
            destination=sai_ipprefix('30.30.0.20/32'))
        sai_thrift_create_route_entry(
            self.client,
            route_entry_sp10_200,
            next_hop_id=nhop_sp10_200)

        pkt1 = simple_tcp_packet(
            eth_dst=ROUTER_MAC,
            eth_src='00:22:22:22:22:22',
            ip_dst='30.30.0.20',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64,
            dl_vlan_enable=True,
            vlan_vid=100,
            pktlen=104)
        pkt2 = simple_tcp_packet(
            eth_dst=peer_src_mac,
            eth_src='00:22:22:22:22:22',
            ip_dst='30.30.0.20',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64,
            dl_vlan_enable=True,
            vlan_vid=100,
            pktlen=104)
        exp_pkt = simple_tcp_packet(
            eth_dst='00:33:33:33:02:00',
            eth_src=ROUTER_MAC,
            ip_dst='30.30.0.20',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63,
            dl_vlan_enable=True,
            vlan_vid=200,
            pktlen=104)

        try:
            print("Sending packet on port %d, forward" % self.dev_port10)
            send_packet(self, self.dev_port10, pkt1)
            verify_packet(self, exp_pkt, self.dev_port10)

            print("Sending packet on port %d, drop "
                  "(eth_dst=%s)" % (self.dev_port10, peer_src_mac))
            send_packet(self, self.dev_port10, pkt2)
            verify_no_other_packets(self, timeout=1)

            print("Setting peer_src_mac for Sub Port 10 Vlan 100 to %s"
                  % peer_src_mac)
            attr_value = sai_thrift_attribute_value_t(mac=peer_src_mac)
            attr = sai_thrift_attribute_t(
                id=(SAI_ROUTER_INTERFACE_ATTR_END + 5), value=attr_value)
            sai_thrift_set_router_interface_attribute(
                self.client,
                subport10_100,
                custom_attribute=attr)

            print("Sending packet on port %d, forward (eth_dst=%s)" %
                  (self.dev_port10, peer_src_mac))
            send_packet(self, self.dev_port10, pkt2)
            verify_packet(self, exp_pkt, self.dev_port10)

        finally:
            sai_thrift_remove_route_entry(self.client, route_entry_sp10_200)
            sai_thrift_remove_neighbor_entry(self.client, nbr_entry_sp10_200)
            sai_thrift_remove_next_hop(self.client, nhop_sp10_200)
            sai_thrift_remove_router_interface(self.client, subport10_200)
            sai_thrift_remove_route_entry(self.client, route_entry_sp10_100)
            sai_thrift_remove_neighbor_entry(self.client, nbr_entry_sp10_100)
            sai_thrift_remove_next_hop(self.client, nhop_sp10_100)
            sai_thrift_remove_router_interface(self.client, subport10_100)

    def srcMacAddressRifUpdateAttr(self):
        '''
        Verify if packets are forwarded if coming on ingress with
        SAI_ROUTER_INTERFACE_ATTR_SRC_MAC_ADDRESS_RIF_UPDATE as dst_addr
        for L3 Interface RIF and if src_addr is correctly set on egress
        '''
        print("srcMacAddressRifUpdateAttr()")

        dmac1 = '00:11:22:33:44:55'
        rif_update_mac = '00:33:22:33:44:55'

        nhop = sai_thrift_create_next_hop(
            self.client,
            ip=sai_ipaddress('10.10.10.2'),
            router_interface_id=self.port10_rif,
            type=SAI_NEXT_HOP_TYPE_IP)
        neighbor_entry = sai_thrift_neighbor_entry_t(
            rif_id=self.port10_rif, ip_address=sai_ipaddress('10.10.10.2'))
        sai_thrift_create_neighbor_entry(
            self.client, neighbor_entry, dst_mac_address=dmac1)

        route_entry = sai_thrift_route_entry_t(
            vr_id=self.default_vrf, destination=sai_ipprefix('10.10.10.1/32'))
        sai_thrift_create_route_entry(
            self.client, route_entry, next_hop_id=nhop)

        pkt1 = simple_tcp_packet(
            eth_dst=ROUTER_MAC,
            eth_src='00:22:22:22:22:22',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        pkt2 = simple_tcp_packet(
            eth_dst=rif_update_mac,
            eth_src='00:22:22:22:22:22',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        exp_pkt1 = simple_tcp_packet(
            eth_dst=dmac1,
            eth_src=ROUTER_MAC,
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63)
        exp_pkt2 = simple_tcp_packet(
            eth_dst=dmac1,
            eth_src=rif_update_mac,
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63)

        try:
            print("Sending packet on port %d, forward" % self.dev_port11)
            send_packet(self, self.dev_port11, pkt1)
            verify_packet(self, exp_pkt1, self.dev_port10)

            print("Sending packet on port %d, drop "
                  "(eth_dst=%s)" % (self.dev_port11, rif_update_mac))
            send_packet(self, self.dev_port11, pkt2)
            verify_no_other_packets(self, timeout=2)

            print("Setting src_mac_rif_update for rif on port %d to %s"
                  % (self.dev_port11, rif_update_mac))
            attr_value = sai_thrift_attribute_value_t(mac=rif_update_mac)
            attr = sai_thrift_attribute_t(
                id=(SAI_ROUTER_INTERFACE_ATTR_END + 1), value=attr_value)
            sai_thrift_set_router_interface_attribute(
                self.client,
                self.port11_rif,
                custom_attribute=attr)

            print("Sending packet on port %d, forward "
                  "(eth_dst=%s)" % (self.dev_port11, rif_update_mac))
            send_packet(self, self.dev_port11, pkt2)
            verify_packet(self, exp_pkt1, self.dev_port10)

            print("Setting src_mac_rif_update for rif on port %d to %s"
                  % (self.dev_port10, rif_update_mac))
            sai_thrift_set_router_interface_attribute(
                self.client,
                self.port10_rif,
                custom_attribute=attr)

            print("Sending packet on port %d, forward "
                  "(expected eth_src=%s)"
                  % (self.dev_port11, rif_update_mac))
            send_packet(self, self.dev_port11, pkt1)
            verify_packet(self, exp_pkt2, self.dev_port10)

            print("Sending packet on port %d, forward "
                  "(eth_dst=%s and expected eth_src=%s)"
                  % (self.dev_port11, rif_update_mac, rif_update_mac))
            send_packet(self, self.dev_port11, pkt2)
            verify_packet(self, exp_pkt2, self.dev_port10)

        finally:
            sai_thrift_remove_route_entry(self.client, route_entry)
            sai_thrift_remove_neighbor_entry(self.client, neighbor_entry)
            sai_thrift_remove_next_hop(self.client, nhop)
            attr_value = sai_thrift_attribute_value_t(mac="00:00:00:00:00:00")
            attr = sai_thrift_attribute_t(
                id=(SAI_ROUTER_INTERFACE_ATTR_END + 1), value=attr_value)
            sai_thrift_set_router_interface_attribute(
                self.client,
                self.port11_rif,
                custom_attribute=attr)
            sai_thrift_set_router_interface_attribute(
                self.client,
                self.port10_rif,
                custom_attribute=attr)

    def srcMacAddressRifUpdateAttrSvi(self):
        '''
        Verify if packets are forwarded if coming on ingress with
        SAI_ROUTER_INTERFACE_ATTR_SRC_MAC_ADDRESS_RIF_UPDATE as dst_addr
        for L3 Interface RIF and if src_addr is correctly set on egress
        '''
        print("srcMacAddressRifUpdateAttrSvi()")

        dmac1 = '00:11:22:33:44:55'
        dmac2 = '00:11:22:33:44:66'
        rif_update_mac = '00:33:22:33:44:55'

        nhop1 = sai_thrift_create_next_hop(
            self.client,
            ip=sai_ipaddress('10.10.10.2'),
            router_interface_id=self.port10_rif,
            type=SAI_NEXT_HOP_TYPE_IP)
        neighbor_entry1 = sai_thrift_neighbor_entry_t(
            rif_id=self.port10_rif, ip_address=sai_ipaddress('10.10.10.2'))
        sai_thrift_create_neighbor_entry(
            self.client, neighbor_entry1, dst_mac_address=dmac1)

        route_entry1 = sai_thrift_route_entry_t(
            vr_id=self.default_vrf, destination=sai_ipprefix('10.10.10.1/32'))
        sai_thrift_create_route_entry(
            self.client, route_entry1, next_hop_id=nhop1)

        nhop2 = sai_thrift_create_next_hop(
            self.client,
            ip=sai_ipaddress('30.10.10.1'),
            router_interface_id=self.vlan30_rif,
            type=SAI_NEXT_HOP_TYPE_IP)
        neighbor_entry2 = sai_thrift_neighbor_entry_t(
            rif_id=self.vlan30_rif, ip_address=sai_ipaddress('30.10.10.1'))
        sai_thrift_create_neighbor_entry(
            self.client, neighbor_entry2, dst_mac_address=dmac2)

        route_entry2 = sai_thrift_route_entry_t(
            vr_id=self.default_vrf, destination=sai_ipprefix('30.10.10.1/32'))
        sai_thrift_create_route_entry(
            self.client, route_entry2, next_hop_id=nhop2)

        pkt1 = simple_tcp_packet(
            eth_dst=ROUTER_MAC,
            eth_src='00:22:22:22:22:22',
            ip_dst='30.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        pkt2 = simple_tcp_packet(
            eth_dst=ROUTER_MAC,
            eth_src='00:22:22:22:22:22',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        pkt3 = simple_tcp_packet(
            eth_dst=rif_update_mac,
            eth_src='00:22:22:22:22:22',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        vlan_pkt1 = simple_tcp_packet(
            eth_dst=ROUTER_MAC,
            eth_src='00:22:22:22:22:22',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            dl_vlan_enable=True,
            vlan_vid=30,
            ip_id=105,
            ip_ttl=64,
            pktlen=104)
        vlan_pkt2 = simple_tcp_packet(
            eth_dst=rif_update_mac,
            eth_src='00:22:22:22:22:22',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            dl_vlan_enable=True,
            vlan_vid=30,
            ip_id=105,
            ip_ttl=64,
            pktlen=104)
        exp_pkt1 = simple_tcp_packet(
            eth_dst=dmac1,
            eth_src=ROUTER_MAC,
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63,
            pktlen=100)
        exp_pkt2 = simple_tcp_packet(
            eth_dst=dmac2,
            eth_src=rif_update_mac,
            ip_dst='30.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63,
            pktlen=100)
        try:
            print("Sending tagged packet on port %d, forward" %
                  self.dev_port20)
            send_packet(self, self.dev_port21, vlan_pkt1)
            verify_packet(self, exp_pkt1, self.dev_port10)

            print("Sending untagged packet on port %d, forward" %
                  self.dev_port20)
            send_packet(self, self.dev_port20, pkt2)
            verify_packet(self, exp_pkt1, self.dev_port10)

            print("Setting src_mac_rif_update for SVI 30 to %s"
                  % rif_update_mac)
            attr_value = sai_thrift_attribute_value_t(mac=rif_update_mac)
            attr = sai_thrift_attribute_t(
                id=(SAI_ROUTER_INTERFACE_ATTR_END + 1), value=attr_value)
            sai_thrift_set_router_interface_attribute(
                self.client,
                self.vlan30_rif,
                custom_attribute=attr)

            print("Sending tagged packet on port %d, forward "
                  "(eth_dst=%s)" % (self.dev_port20, rif_update_mac))
            send_packet(self, self.dev_port21, vlan_pkt2)
            verify_packet(self, exp_pkt1, self.dev_port10)

            print("Sending untagged packet on port %d, forward "
                  "(eth_dst=%s)" % (self.dev_port20, rif_update_mac))
            send_packet(self, self.dev_port20, pkt3)
            verify_packet(self, exp_pkt1, self.dev_port10)

            print("Creating an fdb entry for SVI 30 port %d to %s"
                  % (self.dev_port20, rif_update_mac))
            fdb_entry = sai_thrift_fdb_entry_t(
                switch_id=self.switch_id,
                mac_address=dmac2,
                bv_id=self.vlan30)
            sai_thrift_create_fdb_entry(
                self.client,
                fdb_entry,
                type=SAI_FDB_ENTRY_TYPE_STATIC,
                bridge_port_id=self.port20_bp,
                packet_action=SAI_PACKET_ACTION_FORWARD)
            print("Sending untagged packet on port %d, forward "
                  "(expected eth_src=%s)" % (self.dev_port10, rif_update_mac))
            send_packet(self, self.dev_port10, pkt1)
            verify_packet(self, exp_pkt2, self.dev_port20)

        finally:
            sai_thrift_remove_fdb_entry(self.client, fdb_entry)
            sai_thrift_remove_route_entry(self.client, route_entry2)
            sai_thrift_remove_neighbor_entry(self.client, neighbor_entry2)
            sai_thrift_remove_next_hop(self.client, nhop2)
            sai_thrift_remove_route_entry(self.client, route_entry1)
            sai_thrift_remove_neighbor_entry(self.client, neighbor_entry1)
            sai_thrift_remove_next_hop(self.client, nhop1)
            attr_value = sai_thrift_attribute_value_t(mac="00:00:00:00:00:00")
            attr = sai_thrift_attribute_t(
                id=(SAI_ROUTER_INTERFACE_ATTR_END + 1), value=attr_value)
            sai_thrift_set_router_interface_attribute(
                self.client,
                self.vlan30_rif,
                custom_attribute=attr)

    def srcMacAddressRifUpdateAttrSubPort(self):
        '''
        Verify if packets are forwarded if coming on ingress with
        SAI_ROUTER_INTERFACE_ATTR_SRC_MAC_ADDRESS_RIF_UPDATE as dst_addr
        for Sub Port RIF
        '''
        print("srcMacAddressRifUpdateAttrSubPort()")

        rif_update_mac = '00:33:22:33:44:55'

        subport10_100 = sai_thrift_create_router_interface(
            self.client,
            type=SAI_ROUTER_INTERFACE_TYPE_SUB_PORT,
            virtual_router_id=self.default_vrf,
            port_id=self.port10,
            outer_vlan_id=100)
        nhop_sp10_100 = sai_thrift_create_next_hop(
            self.client,
            ip=sai_ipaddress('20.20.0.10'),
            router_interface_id=subport10_100,
            type=SAI_NEXT_HOP_TYPE_IP)
        nbr_entry_sp10_100 = sai_thrift_neighbor_entry_t(
            rif_id=subport10_100,
            ip_address=sai_ipaddress('20.20.0.10'))
        sai_thrift_create_neighbor_entry(
            self.client,
            nbr_entry_sp10_100,
            dst_mac_address='00:33:33:33:01:00')
        route_entry_sp10_100 = sai_thrift_route_entry_t(
            vr_id=self.default_vrf,
            destination=sai_ipprefix('30.30.0.10/32'))
        sai_thrift_create_route_entry(
            self.client,
            route_entry_sp10_100,
            next_hop_id=nhop_sp10_100)

        subport10_200 = sai_thrift_create_router_interface(
            self.client,
            type=SAI_ROUTER_INTERFACE_TYPE_SUB_PORT,
            virtual_router_id=self.default_vrf,
            port_id=self.port10,
            outer_vlan_id=200)
        nhop_sp10_200 = sai_thrift_create_next_hop(
            self.client,
            ip=sai_ipaddress('20.20.0.20'),
            router_interface_id=subport10_200,
            type=SAI_NEXT_HOP_TYPE_IP)
        nbr_entry_sp10_200 = sai_thrift_neighbor_entry_t(
            rif_id=subport10_200,
            ip_address=sai_ipaddress('20.20.0.20'))
        sai_thrift_create_neighbor_entry(
            self.client,
            nbr_entry_sp10_200,
            dst_mac_address='00:33:33:33:02:00')
        route_entry_sp10_200 = sai_thrift_route_entry_t(
            vr_id=self.default_vrf,
            destination=sai_ipprefix('30.30.0.20/32'))
        sai_thrift_create_route_entry(
            self.client,
            route_entry_sp10_200,
            next_hop_id=nhop_sp10_200)

        pkt1 = simple_tcp_packet(
            eth_dst=ROUTER_MAC,
            eth_src='00:22:22:22:22:22',
            ip_dst='30.30.0.20',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64,
            dl_vlan_enable=True,
            vlan_vid=100,
            pktlen=104)
        pkt2 = simple_tcp_packet(
            eth_dst=rif_update_mac,
            eth_src='00:22:22:22:22:22',
            ip_dst='30.30.0.20',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64,
            dl_vlan_enable=True,
            vlan_vid=100,
            pktlen=104)
        exp_pkt1 = simple_tcp_packet(
            eth_dst='00:33:33:33:02:00',
            eth_src=ROUTER_MAC,
            ip_dst='30.30.0.20',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63,
            dl_vlan_enable=True,
            vlan_vid=200,
            pktlen=104)
        exp_pkt2 = simple_tcp_packet(
            eth_dst='00:33:33:33:02:00',
            eth_src=rif_update_mac,
            ip_dst='30.30.0.20',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63,
            dl_vlan_enable=True,
            vlan_vid=200,
            pktlen=104)

        try:
            print("Sending packet on port %d, forward" % self.dev_port10)
            send_packet(self, self.dev_port10, pkt1)
            verify_packet(self, exp_pkt1, self.dev_port10)

            print("Sending packet on port %d, drop "
                  "(eth_dst=%s)" % (self.dev_port10, rif_update_mac))
            send_packet(self, self.dev_port10, pkt2)
            verify_no_other_packets(self, timeout=1)

            print("Setting src_mac_rif_update for Sub Port 10 Vlan 100")
            attr_value = sai_thrift_attribute_value_t(mac=rif_update_mac)
            attr = sai_thrift_attribute_t(
                id=(SAI_ROUTER_INTERFACE_ATTR_END + 1), value=attr_value)
            sai_thrift_set_router_interface_attribute(
                self.client,
                subport10_100,
                custom_attribute=attr)

            print("Sending packet on port %d, forward "
                  "(eth_dst=%s)"
                  % (self.dev_port10, rif_update_mac))
            send_packet(self, self.dev_port10, pkt2)
            verify_packet(self, exp_pkt1, self.dev_port10)

            print("Setting src_mac_rif_update for Sub Port 10 Vlan 100 to %s"
                  % rif_update_mac)
            attr_value = sai_thrift_attribute_value_t(mac=rif_update_mac)
            attr = sai_thrift_attribute_t(
                id=(SAI_ROUTER_INTERFACE_ATTR_END + 1), value=attr_value)
            sai_thrift_set_router_interface_attribute(
                self.client,
                subport10_200,
                custom_attribute=attr)

            print("Sending packet on port %d, forward "
                  "(expected eth_src=%s)"
                  % (self.dev_port10, rif_update_mac))
            send_packet(self, self.dev_port10, pkt2)
            verify_packet(self, exp_pkt2, self.dev_port10)

        finally:
            sai_thrift_remove_route_entry(self.client, route_entry_sp10_200)
            sai_thrift_remove_neighbor_entry(self.client, nbr_entry_sp10_200)
            sai_thrift_remove_next_hop(self.client, nhop_sp10_200)
            sai_thrift_remove_router_interface(self.client, subport10_200)
            sai_thrift_remove_route_entry(self.client, route_entry_sp10_100)
            sai_thrift_remove_neighbor_entry(self.client, nbr_entry_sp10_100)
            sai_thrift_remove_next_hop(self.client, nhop_sp10_100)
            sai_thrift_remove_router_interface(self.client, subport10_100)

    def anycastMacSupportAttr(self):
        '''
        Verify if packets are forwarded if coming on ingress with
        anycast_mac as dst_addr for L3 Interface RIF and if src_addr
        is correctly set on egress. Tested for both cases when
        virtual RIF is created before regular RIF and when regular RIF
        is created before virtual RIF.
        '''
        print("anycastMacSupportAttr()")

        dmac1 = '00:11:22:33:44:55'
        new_router_mac1 = '00:44:22:33:44:55'
        new_router_mac2 = '00:55:22:33:44:55'

        attr_value = sai_thrift_attribute_value_t(booldata=True)
        attr = sai_thrift_attribute_t(
            id=(SAI_ROUTER_INTERFACE_ATTR_END + 0), value=attr_value)

        print("Creating first virtual RIF on port25 with src_mac=%s "
              "and anycast_mac_support set to true" % new_router_mac1)
        virtual_rif1 = sai_thrift_create_router_interface(
            self.client,
            type=SAI_ROUTER_INTERFACE_TYPE_PORT,
            virtual_router_id=self.default_vrf,
            port_id=self.port24,
            is_virtual=True,
            src_mac_address=new_router_mac1,
            custom_attribute=attr)
        rif1 = sai_thrift_create_router_interface(
            self.client,
            type=SAI_ROUTER_INTERFACE_TYPE_PORT,
            virtual_router_id=self.default_vrf,
            port_id=self.port24)

        rif2 = sai_thrift_create_router_interface(
            self.client,
            type=SAI_ROUTER_INTERFACE_TYPE_PORT,
            virtual_router_id=self.default_vrf,
            port_id=self.port25)

        nhop = sai_thrift_create_next_hop(
            self.client,
            ip=sai_ipaddress('10.10.10.2'),
            router_interface_id=rif2,
            type=SAI_NEXT_HOP_TYPE_IP)
        neighbor_entry = sai_thrift_neighbor_entry_t(
            rif_id=rif2, ip_address=sai_ipaddress('10.10.10.2'))
        sai_thrift_create_neighbor_entry(
            self.client, neighbor_entry, dst_mac_address=dmac1)
        route_entry = sai_thrift_route_entry_t(
            vr_id=self.default_vrf, destination=sai_ipprefix('10.10.10.1/32'))
        sai_thrift_create_route_entry(
            self.client, route_entry, next_hop_id=nhop)

        pkt1 = simple_tcp_packet(
            eth_dst=ROUTER_MAC,
            eth_src='00:22:22:22:22:22',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        pkt2 = simple_tcp_packet(
            eth_dst=new_router_mac1,
            eth_src='00:22:22:22:22:22',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        exp_pkt1 = simple_tcp_packet(
            eth_dst=dmac1,
            eth_src=ROUTER_MAC,
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63)
        exp_pkt2 = simple_tcp_packet(
            eth_dst=dmac1,
            eth_src=new_router_mac2,
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63)

        try:
            print("Sending packet on port %d, forward" % self.dev_port11)
            send_packet(self, self.dev_port24, pkt1)
            verify_packet(self, exp_pkt1, self.dev_port25)

            sai_thrift_remove_route_entry(self.client, route_entry)
            sai_thrift_remove_neighbor_entry(self.client, neighbor_entry)
            sai_thrift_remove_next_hop(self.client, nhop)
            sai_thrift_remove_router_interface(self.client, rif2)

            print("Creating second virtual RIF on port25 with src_mac=%s "
                  "and anycast_mac_support set to true" % new_router_mac2)
            virtual_rif2 = sai_thrift_create_router_interface(
                self.client,
                type=SAI_ROUTER_INTERFACE_TYPE_PORT,
                virtual_router_id=self.default_vrf,
                port_id=self.port25,
                is_virtual=True,
                src_mac_address=new_router_mac2,
                custom_attribute=attr)

            print("Creating regular RIF on port25")
            rif2 = sai_thrift_create_router_interface(
                self.client,
                type=SAI_ROUTER_INTERFACE_TYPE_PORT,
                virtual_router_id=self.default_vrf,
                port_id=self.port25)

            nhop = sai_thrift_create_next_hop(
                self.client,
                ip=sai_ipaddress('10.10.10.2'),
                router_interface_id=rif2,
                type=SAI_NEXT_HOP_TYPE_IP)
            neighbor_entry = sai_thrift_neighbor_entry_t(
                rif_id=rif2, ip_address=sai_ipaddress('10.10.10.2'))
            sai_thrift_create_neighbor_entry(
                self.client, neighbor_entry, dst_mac_address=dmac1)
            route_entry = sai_thrift_route_entry_t(
                vr_id=self.default_vrf,
                destination=sai_ipprefix('10.10.10.1/32'))
            sai_thrift_create_route_entry(
                self.client, route_entry, next_hop_id=nhop)

            print("Sending packet on port %d, forward "
                  "(eth_dst=%s)" % (self.dev_port11, new_router_mac1))
            send_packet(self, self.dev_port24, pkt2)
            verify_packet(self, exp_pkt2, self.dev_port25)

            print("Sending packet on port %d, forward "
                  "(expected eth_src=%s)" % (self.dev_port11, new_router_mac2))
            send_packet(self, self.dev_port24, pkt1)
            verify_packet(self, exp_pkt2, self.dev_port25)

            sai_thrift_remove_route_entry(self.client, route_entry)
            sai_thrift_remove_neighbor_entry(self.client, neighbor_entry)
            sai_thrift_remove_next_hop(self.client, nhop)

            print("Removing virtual and regular RIFs on port25")
            sai_thrift_remove_router_interface(self.client, virtual_rif2)
            sai_thrift_remove_router_interface(self.client, rif2)

            print("Creating regular RIF on port25")
            rif2 = sai_thrift_create_router_interface(
                self.client,
                type=SAI_ROUTER_INTERFACE_TYPE_PORT,
                virtual_router_id=self.default_vrf,
                port_id=self.port25)

            print("Creating second virtual RIF on port25 with src_mac=%s "
                  "and anycast_mac_support set to true" % new_router_mac2)
            virtual_rif2 = sai_thrift_create_router_interface(
                self.client,
                type=SAI_ROUTER_INTERFACE_TYPE_PORT,
                virtual_router_id=self.default_vrf,
                port_id=self.port25,
                is_virtual=True,
                src_mac_address=new_router_mac2,
                custom_attribute=attr)

            nhop = sai_thrift_create_next_hop(
                self.client,
                ip=sai_ipaddress('10.10.10.2'),
                router_interface_id=rif2,
                type=SAI_NEXT_HOP_TYPE_IP)
            neighbor_entry = sai_thrift_neighbor_entry_t(
                rif_id=rif2, ip_address=sai_ipaddress('10.10.10.2'))
            sai_thrift_create_neighbor_entry(
                self.client, neighbor_entry, dst_mac_address=dmac1)
            route_entry = sai_thrift_route_entry_t(
                vr_id=self.default_vrf,
                destination=sai_ipprefix('10.10.10.1/32'))
            sai_thrift_create_route_entry(
                self.client, route_entry, next_hop_id=nhop)

            print("Sending packet on port %d, forward "
                  "(eth_dst=%s)" % (self.dev_port11, new_router_mac1))
            send_packet(self, self.dev_port24, pkt2)
            verify_packet(self, exp_pkt2, self.dev_port25)

            print("Sending packet on port %d, forward "
                  "(expected eth_src=%s)" % (self.dev_port11, new_router_mac2))
            send_packet(self, self.dev_port24, pkt1)
            verify_packet(self, exp_pkt2, self.dev_port25)

        finally:
            sai_thrift_remove_route_entry(self.client, route_entry)
            sai_thrift_remove_neighbor_entry(self.client, neighbor_entry)
            sai_thrift_remove_next_hop(self.client, nhop)
            sai_thrift_remove_router_interface(self.client, virtual_rif2)
            sai_thrift_remove_router_interface(self.client, rif2)
            sai_thrift_remove_router_interface(self.client, virtual_rif1)
            sai_thrift_remove_router_interface(self.client, rif1)

    def anycastMacSupportSetAttr(self):
        '''
        Verify if packets are forwarded if coming on ingress with
        anycast_mac as dst_addr for L3 Interface RIF and if src_addr
        is correctly set on egress.
        Test verifies the case when anycast_mac_support is set
        after both RIFs are created.
        '''
        print("anycastMacSupportSetAttr()")

        dmac1 = '00:11:22:33:44:55'
        new_router_mac1 = '00:44:22:33:44:55'
        new_router_mac2 = '00:55:22:33:44:55'

        attr_value = sai_thrift_attribute_value_t(booldata=True)
        attr = sai_thrift_attribute_t(
            id=(SAI_ROUTER_INTERFACE_ATTR_END + 0), value=attr_value)

        print("Creating first virtual RIF on port25 with src_mac=%s "
              "and anycast_mac_support set to true" % new_router_mac1)
        virtual_rif1 = sai_thrift_create_router_interface(
            self.client,
            type=SAI_ROUTER_INTERFACE_TYPE_PORT,
            virtual_router_id=self.default_vrf,
            port_id=self.port24,
            is_virtual=True,
            src_mac_address=new_router_mac1,
            custom_attribute=attr)
        rif1 = sai_thrift_create_router_interface(
            self.client,
            type=SAI_ROUTER_INTERFACE_TYPE_PORT,
            virtual_router_id=self.default_vrf,
            port_id=self.port24)

        rif2 = sai_thrift_create_router_interface(
            self.client,
            type=SAI_ROUTER_INTERFACE_TYPE_PORT,
            virtual_router_id=self.default_vrf,
            port_id=self.port25)

        nhop = sai_thrift_create_next_hop(
            self.client,
            ip=sai_ipaddress('10.10.10.2'),
            router_interface_id=rif2,
            type=SAI_NEXT_HOP_TYPE_IP)
        neighbor_entry = sai_thrift_neighbor_entry_t(
            rif_id=rif2, ip_address=sai_ipaddress('10.10.10.2'))
        sai_thrift_create_neighbor_entry(
            self.client, neighbor_entry, dst_mac_address=dmac1)
        route_entry = sai_thrift_route_entry_t(
            vr_id=self.default_vrf, destination=sai_ipprefix('10.10.10.1/32'))
        sai_thrift_create_route_entry(
            self.client, route_entry, next_hop_id=nhop)

        pkt1 = simple_tcp_packet(
            eth_dst=ROUTER_MAC,
            eth_src='00:22:22:22:22:22',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        pkt2 = simple_tcp_packet(
            eth_dst=new_router_mac1,
            eth_src='00:22:22:22:22:22',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        exp_pkt1 = simple_tcp_packet(
            eth_dst=dmac1,
            eth_src=ROUTER_MAC,
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63)
        exp_pkt2 = simple_tcp_packet(
            eth_dst=dmac1,
            eth_src=new_router_mac2,
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63)

        try:
            print("Sending packet on port %d, forward" % self.dev_port11)
            send_packet(self, self.dev_port24, pkt1)
            verify_packet(self, exp_pkt1, self.dev_port25)

            sai_thrift_remove_route_entry(self.client, route_entry)
            sai_thrift_remove_neighbor_entry(self.client, neighbor_entry)
            sai_thrift_remove_next_hop(self.client, nhop)
            sai_thrift_remove_router_interface(self.client, rif2)

            print("Creating second virtual RIF on port25 with src_mac=%s "
                  "and anycast_mac_support set to false" % new_router_mac2)
            virtual_rif2 = sai_thrift_create_router_interface(
                self.client,
                type=SAI_ROUTER_INTERFACE_TYPE_PORT,
                virtual_router_id=self.default_vrf,
                port_id=self.port25,
                is_virtual=True,
                src_mac_address=new_router_mac2)

            print("Creating regular RIF on port25")
            rif2 = sai_thrift_create_router_interface(
                self.client,
                type=SAI_ROUTER_INTERFACE_TYPE_PORT,
                virtual_router_id=self.default_vrf,
                port_id=self.port25)

            nhop = sai_thrift_create_next_hop(
                self.client,
                ip=sai_ipaddress('10.10.10.2'),
                router_interface_id=rif2,
                type=SAI_NEXT_HOP_TYPE_IP)
            neighbor_entry = sai_thrift_neighbor_entry_t(
                rif_id=rif2, ip_address=sai_ipaddress('10.10.10.2'))
            sai_thrift_create_neighbor_entry(
                self.client, neighbor_entry, dst_mac_address=dmac1)
            route_entry = sai_thrift_route_entry_t(
                vr_id=self.default_vrf,
                destination=sai_ipprefix('10.10.10.1/32'))
            sai_thrift_create_route_entry(
                self.client, route_entry, next_hop_id=nhop)

            print("Sending packet on port %d, forward" % self.dev_port11)
            send_packet(self, self.dev_port24, pkt1)
            verify_packet(self, exp_pkt1, self.dev_port25)

            print("Setting anycast_mac_support to true for second virtual RIF")
            sai_thrift_set_router_interface_attribute(
                self.client,
                virtual_rif2,
                custom_attribute=attr)

            print("Sending packet on port %d, forward "
                  "(eth_dst=%s)" % (self.dev_port11, new_router_mac1))
            send_packet(self, self.dev_port24, pkt2)
            verify_packet(self, exp_pkt2, self.dev_port25)

            print("Sending packet on port %d, forward "
                  "(expected eth_src=%s)" % (self.dev_port11, new_router_mac2))
            send_packet(self, self.dev_port24, pkt1)
            verify_packet(self, exp_pkt2, self.dev_port25)

            attr_value = sai_thrift_attribute_value_t(booldata=False)
            attr = sai_thrift_attribute_t(
                id=(SAI_ROUTER_INTERFACE_ATTR_END + 0), value=attr_value)

            print("Setting anycast_mac_support to false "
                  "for second virtual RIF")
            sai_thrift_set_router_interface_attribute(
                self.client,
                virtual_rif2,
                custom_attribute=attr)

            print("Sending packet on port %d, forward" % self.dev_port11)
            send_packet(self, self.dev_port24, pkt1)
            verify_packet(self, exp_pkt1, self.dev_port25)

        finally:
            sai_thrift_remove_route_entry(self.client, route_entry)
            sai_thrift_remove_neighbor_entry(self.client, neighbor_entry)
            sai_thrift_remove_next_hop(self.client, nhop)
            sai_thrift_remove_router_interface(self.client, virtual_rif2)
            sai_thrift_remove_router_interface(self.client, rif2)
            sai_thrift_remove_router_interface(self.client, virtual_rif1)
            sai_thrift_remove_router_interface(self.client, rif1)

    def anycastMacSupportSetSrcMacAttr(self):
        '''
        Verify if packets are forwarded if coming on ingress with
        anycast_mac as dst_addr for L3 Interface RIF and if src_addr
        is correctly set on egress.
        Test verifies the case when src_mac of virtual RIF is modified.
        '''
        print("anycastMacSupportSetSrcMacAttr()")

        dmac1 = '00:11:22:33:44:55'
        new_router_mac1 = '00:44:22:33:44:55'
        new_router_mac2 = '00:55:22:33:44:55'
        new_router_mac3 = '00:66:22:33:44:55'

        attr_value = sai_thrift_attribute_value_t(booldata=True)
        attr = sai_thrift_attribute_t(
            id=(SAI_ROUTER_INTERFACE_ATTR_END + 0), value=attr_value)

        print("Creating first virtual RIF on port25 with src_mac=%s "
              "and anycast_mac_support set to true" % new_router_mac1)
        virtual_rif1 = sai_thrift_create_router_interface(
            self.client,
            type=SAI_ROUTER_INTERFACE_TYPE_PORT,
            virtual_router_id=self.default_vrf,
            port_id=self.port24,
            is_virtual=True,
            src_mac_address=new_router_mac1,
            custom_attribute=attr)
        rif1 = sai_thrift_create_router_interface(
            self.client,
            type=SAI_ROUTER_INTERFACE_TYPE_PORT,
            virtual_router_id=self.default_vrf,
            port_id=self.port24)

        rif2 = sai_thrift_create_router_interface(
            self.client,
            type=SAI_ROUTER_INTERFACE_TYPE_PORT,
            virtual_router_id=self.default_vrf,
            port_id=self.port25)

        nhop = sai_thrift_create_next_hop(
            self.client,
            ip=sai_ipaddress('10.10.10.2'),
            router_interface_id=rif2,
            type=SAI_NEXT_HOP_TYPE_IP)
        neighbor_entry = sai_thrift_neighbor_entry_t(
            rif_id=rif2, ip_address=sai_ipaddress('10.10.10.2'))
        sai_thrift_create_neighbor_entry(
            self.client, neighbor_entry, dst_mac_address=dmac1)
        route_entry = sai_thrift_route_entry_t(
            vr_id=self.default_vrf, destination=sai_ipprefix('10.10.10.1/32'))
        sai_thrift_create_route_entry(
            self.client, route_entry, next_hop_id=nhop)

        pkt1 = simple_tcp_packet(
            eth_dst=ROUTER_MAC,
            eth_src='00:22:22:22:22:22',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        pkt2 = simple_tcp_packet(
            eth_dst=new_router_mac1,
            eth_src='00:22:22:22:22:22',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        exp_pkt1 = simple_tcp_packet(
            eth_dst=dmac1,
            eth_src=ROUTER_MAC,
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63)
        exp_pkt2 = simple_tcp_packet(
            eth_dst=dmac1,
            eth_src=new_router_mac2,
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63)
        exp_pkt3 = simple_tcp_packet(
            eth_dst=dmac1,
            eth_src=new_router_mac3,
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63)

        try:
            print("Sending packet on port %d, forward" % self.dev_port11)
            send_packet(self, self.dev_port24, pkt1)
            verify_packet(self, exp_pkt1, self.dev_port25)

            sai_thrift_remove_route_entry(self.client, route_entry)
            sai_thrift_remove_neighbor_entry(self.client, neighbor_entry)
            sai_thrift_remove_next_hop(self.client, nhop)
            sai_thrift_remove_router_interface(self.client, rif2)

            print("Creating second virtual RIF on port25 with src_mac=%s "
                  "and anycast_mac_support set to true" % new_router_mac2)
            virtual_rif2 = sai_thrift_create_router_interface(
                self.client,
                type=SAI_ROUTER_INTERFACE_TYPE_PORT,
                virtual_router_id=self.default_vrf,
                port_id=self.port25,
                is_virtual=True,
                src_mac_address=new_router_mac2,
                custom_attribute=attr)

            print("Creating regular RIF on port25")
            rif2 = sai_thrift_create_router_interface(
                self.client,
                type=SAI_ROUTER_INTERFACE_TYPE_PORT,
                virtual_router_id=self.default_vrf,
                port_id=self.port25)

            nhop = sai_thrift_create_next_hop(
                self.client,
                ip=sai_ipaddress('10.10.10.2'),
                router_interface_id=rif2,
                type=SAI_NEXT_HOP_TYPE_IP)
            neighbor_entry = sai_thrift_neighbor_entry_t(
                rif_id=rif2, ip_address=sai_ipaddress('10.10.10.2'))
            sai_thrift_create_neighbor_entry(
                self.client, neighbor_entry, dst_mac_address=dmac1)
            route_entry = sai_thrift_route_entry_t(
                vr_id=self.default_vrf,
                destination=sai_ipprefix('10.10.10.1/32'))
            sai_thrift_create_route_entry(
                self.client, route_entry, next_hop_id=nhop)

            print("Sending packet on port %d, forward "
                  "(eth_dst=%s)" % (self.dev_port11, new_router_mac1))
            send_packet(self, self.dev_port24, pkt2)
            verify_packet(self, exp_pkt2, self.dev_port25)

            print("Sending packet on port %d, forward "
                  "(expected eth_src=%s)" % (self.dev_port11, new_router_mac2))
            send_packet(self, self.dev_port24, pkt1)
            verify_packet(self, exp_pkt2, self.dev_port25)

            print("Setting virtual RIF src mac to %s" + new_router_mac3)
            sai_thrift_set_router_interface_attribute(
                self.client,
                virtual_rif2,
                src_mac_address=new_router_mac3)

            print("Sending packet on port %d, forward "
                  "(eth_dst=%s)" % (self.dev_port11, new_router_mac1))
            send_packet(self, self.dev_port24, pkt2)
            verify_packet(self, exp_pkt3, self.dev_port25)

            print("Sending packet on port %d, forward "
                  "(expected eth_src=%s)" % (self.dev_port11, new_router_mac3))
            send_packet(self, self.dev_port24, pkt1)
            verify_packet(self, exp_pkt3, self.dev_port25)

        finally:
            sai_thrift_remove_route_entry(self.client, route_entry)
            sai_thrift_remove_neighbor_entry(self.client, neighbor_entry)
            sai_thrift_remove_next_hop(self.client, nhop)
            sai_thrift_remove_router_interface(self.client, virtual_rif2)
            sai_thrift_remove_router_interface(self.client, rif2)
            sai_thrift_remove_router_interface(self.client, virtual_rif1)
            sai_thrift_remove_router_interface(self.client, rif1)

    def anycastMacSupportRemoveAnycastRif(self):
        '''
        Verify if packets are forwarded if coming on ingress with
        anycast_mac as dst_addr for L3 Interface RIF and if src_addr
        is correctly set on egress. Tested for both cases when
        virtual RIF is created before regular RIF and when regular RIF
        is created before virtual RIF.
        '''
        print("anycastMacSupportRemoveAnycastRif()")

        dmac1 = '00:11:22:33:44:55'
        new_router_mac1 = '00:44:22:33:44:55'
        new_router_mac2 = '00:55:22:33:44:55'

        attr_value = sai_thrift_attribute_value_t(booldata=True)
        attr = sai_thrift_attribute_t(
            id=(SAI_ROUTER_INTERFACE_ATTR_END + 0), value=attr_value)

        print("Creating first virtual RIF on port25 with src_mac=%s "
              "and anycast_mac_support set to true" % new_router_mac1)
        virtual_rif1 = sai_thrift_create_router_interface(
            self.client,
            type=SAI_ROUTER_INTERFACE_TYPE_PORT,
            virtual_router_id=self.default_vrf,
            port_id=self.port24,
            is_virtual=True,
            src_mac_address=new_router_mac1,
            custom_attribute=attr)
        rif1 = sai_thrift_create_router_interface(
            self.client,
            type=SAI_ROUTER_INTERFACE_TYPE_PORT,
            virtual_router_id=self.default_vrf,
            port_id=self.port24)

        pkt1 = simple_tcp_packet(
            eth_dst=ROUTER_MAC,
            eth_src='00:22:22:22:22:22',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        pkt2 = simple_tcp_packet(
            eth_dst=new_router_mac1,
            eth_src='00:22:22:22:22:22',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        exp_pkt1 = simple_tcp_packet(
            eth_dst=dmac1,
            eth_src=ROUTER_MAC,
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63)
        exp_pkt2 = simple_tcp_packet(
            eth_dst=dmac1,
            eth_src=new_router_mac2,
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63)

        try:
            print("Creating second virtual RIF on port25 with src_mac=%s "
                  "and anycast_mac_support set to true" % new_router_mac2)
            virtual_rif2 = sai_thrift_create_router_interface(
                self.client,
                type=SAI_ROUTER_INTERFACE_TYPE_PORT,
                virtual_router_id=self.default_vrf,
                port_id=self.port25,
                is_virtual=True,
                src_mac_address=new_router_mac2,
                custom_attribute=attr)

            print("Creating regular RIF on port25")
            rif2 = sai_thrift_create_router_interface(
                self.client,
                type=SAI_ROUTER_INTERFACE_TYPE_PORT,
                virtual_router_id=self.default_vrf,
                port_id=self.port25)

            nhop = sai_thrift_create_next_hop(
                self.client,
                ip=sai_ipaddress('10.10.10.2'),
                router_interface_id=rif2,
                type=SAI_NEXT_HOP_TYPE_IP)
            neighbor_entry = sai_thrift_neighbor_entry_t(
                rif_id=rif2, ip_address=sai_ipaddress('10.10.10.2'))
            sai_thrift_create_neighbor_entry(
                self.client, neighbor_entry, dst_mac_address=dmac1)
            route_entry = sai_thrift_route_entry_t(
                vr_id=self.default_vrf,
                destination=sai_ipprefix('10.10.10.1/32'))
            sai_thrift_create_route_entry(
                self.client, route_entry, next_hop_id=nhop)

            print("Sending packet on port %d, forward "
                  "(eth_dst=%s)" % (self.dev_port11, new_router_mac1))
            send_packet(self, self.dev_port24, pkt2)
            verify_packet(self, exp_pkt2, self.dev_port25)

            print("Sending packet on port %d, forward "
                  "(expected eth_src=%s)" % (self.dev_port11, new_router_mac2))
            send_packet(self, self.dev_port24, pkt1)
            verify_packet(self, exp_pkt2, self.dev_port25)

            print("Removing virtual RIF on port25")
            sai_thrift_remove_router_interface(self.client, virtual_rif2)

#            print("Sending packet on port %d, drop " % (self.dev_port11))
#            send_packet(self, self.dev_port24, pkt2)
#            verify_no_other_packets(self)

            print("Sending packet on port %d, forward "
                  "(expected eth_src=%s)" % (self.dev_port11, ROUTER_MAC))
            send_packet(self, self.dev_port24, pkt1)
            verify_packet(self, exp_pkt1, self.dev_port25)
        finally:
            sai_thrift_remove_route_entry(self.client, route_entry)
            sai_thrift_remove_neighbor_entry(self.client, neighbor_entry)
            sai_thrift_remove_next_hop(self.client, nhop)
            sai_thrift_remove_router_interface(self.client, rif2)
            sai_thrift_remove_router_interface(self.client, virtual_rif1)
            sai_thrift_remove_router_interface(self.client, rif1)

    def anycastMacSupportAttrSvi(self):
        '''
        Verify if packets are forwarded if coming on ingress with
        anycast_mac as dst_addr for SVI RIF and if src_addr
        is correctly set on egress. Tested for both cases when
        virtual RIF is created before regular RIF and when regular RIF
        is created before virtual RIF.
        '''
        print("anycastMacSupportAttrSvi()")

        dmac1 = '00:11:22:33:44:55'
        dmac2 = '00:11:22:33:44:66'
        new_router_mac = '00:33:22:33:44:55'

        nhop1 = sai_thrift_create_next_hop(
            self.client,
            ip=sai_ipaddress('10.10.10.2'),
            router_interface_id=self.port10_rif,
            type=SAI_NEXT_HOP_TYPE_IP)
        neighbor_entry1 = sai_thrift_neighbor_entry_t(
            rif_id=self.port10_rif, ip_address=sai_ipaddress('10.10.10.2'))
        sai_thrift_create_neighbor_entry(
            self.client, neighbor_entry1, dst_mac_address=dmac1)

        route_entry1 = sai_thrift_route_entry_t(
            vr_id=self.default_vrf, destination=sai_ipprefix('10.10.10.1/32'))
        sai_thrift_create_route_entry(
            self.client, route_entry1, next_hop_id=nhop1)

        nhop2 = sai_thrift_create_next_hop(
            self.client,
            ip=sai_ipaddress('30.10.10.1'),
            router_interface_id=self.vlan30_rif,
            type=SAI_NEXT_HOP_TYPE_IP)
        neighbor_entry2 = sai_thrift_neighbor_entry_t(
            rif_id=self.vlan30_rif, ip_address=sai_ipaddress('30.10.10.1'))
        sai_thrift_create_neighbor_entry(
            self.client, neighbor_entry2, dst_mac_address=dmac2)

        route_entry2 = sai_thrift_route_entry_t(
            vr_id=self.default_vrf, destination=sai_ipprefix('30.10.10.1/32'))
        sai_thrift_create_route_entry(
            self.client, route_entry2, next_hop_id=nhop2)

        pkt1 = simple_tcp_packet(
            eth_dst=ROUTER_MAC,
            eth_src='00:22:22:22:22:22',
            ip_dst='30.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        pkt2 = simple_tcp_packet(
            eth_dst=ROUTER_MAC,
            eth_src='00:22:22:22:22:22',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        pkt3 = simple_tcp_packet(
            eth_dst=new_router_mac,
            eth_src='00:22:22:22:22:22',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        vlan_pkt1 = simple_tcp_packet(
            eth_dst=ROUTER_MAC,
            eth_src='00:22:22:22:22:22',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            dl_vlan_enable=True,
            vlan_vid=30,
            ip_id=105,
            ip_ttl=64,
            pktlen=104)
        vlan_pkt2 = simple_tcp_packet(
            eth_dst=new_router_mac,
            eth_src='00:22:22:22:22:22',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            dl_vlan_enable=True,
            vlan_vid=30,
            ip_id=105,
            ip_ttl=64,
            pktlen=104)
        exp_pkt1 = simple_tcp_packet(
            eth_dst=dmac1,
            eth_src=ROUTER_MAC,
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63,
            pktlen=100)
        exp_pkt2 = simple_tcp_packet(
            eth_dst=dmac2,
            eth_src=new_router_mac,
            ip_dst='30.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63,
            pktlen=100)
        try:
            print("Creating virtual RIF on VLAN30 with src_mac=%s "
                  "and anycast_mac_support set to true" % new_router_mac)
            attr_value = sai_thrift_attribute_value_t(booldata=True)
            attr = sai_thrift_attribute_t(
                id=(SAI_ROUTER_INTERFACE_ATTR_END + 0), value=attr_value)
            virtual_rif = sai_thrift_create_router_interface(
                self.client,
                type=SAI_ROUTER_INTERFACE_TYPE_VLAN,
                virtual_router_id=self.default_vrf,
                vlan_id=self.vlan30,
                is_virtual=True,
                src_mac_address=new_router_mac,
                custom_attribute=attr)

            print("Sending tagged packet on port %d, forward" %
                  self.dev_port20)
            send_packet(self, self.dev_port21, vlan_pkt1)
            verify_packet(self, exp_pkt1, self.dev_port10)

            print("Sending untagged packet on port %d, forward" %
                  self.dev_port20)
            send_packet(self, self.dev_port20, pkt2)
            verify_packet(self, exp_pkt1, self.dev_port10)

            print("Sending tagged packet on port %d, forward "
                  "(eth_dst=%s)" % (self.dev_port20, new_router_mac))
            send_packet(self, self.dev_port21, vlan_pkt2)
            verify_packet(self, exp_pkt1, self.dev_port10)

            print("Sending untagged packet on port %d, forward "
                  "(eth_dst=%s)" % (self.dev_port20, new_router_mac))
            send_packet(self, self.dev_port20, pkt3)
            verify_packet(self, exp_pkt1, self.dev_port10)

            print("Creating an fdb entry for SVI 30 port %d" % self.dev_port20)
            fdb_entry = sai_thrift_fdb_entry_t(
                switch_id=self.switch_id,
                mac_address=dmac2,
                bv_id=self.vlan30)
            sai_thrift_create_fdb_entry(
                self.client,
                fdb_entry,
                type=SAI_FDB_ENTRY_TYPE_STATIC,
                bridge_port_id=self.port20_bp,
                packet_action=SAI_PACKET_ACTION_FORWARD)
            print("Sending untagged packet on port %d, forward "
                  "(expected eth_src=%s)" % (self.dev_port10, new_router_mac))
            send_packet(self, self.dev_port10, pkt1)
            verify_packet(self, exp_pkt2, self.dev_port20)

        finally:
            sai_thrift_remove_fdb_entry(self.client, fdb_entry)
            sai_thrift_remove_router_interface(self.client, virtual_rif)
            sai_thrift_remove_route_entry(self.client, route_entry2)
            sai_thrift_remove_neighbor_entry(self.client, neighbor_entry2)
            sai_thrift_remove_next_hop(self.client, nhop2)
            sai_thrift_remove_route_entry(self.client, route_entry1)
            sai_thrift_remove_neighbor_entry(self.client, neighbor_entry1)
            sai_thrift_remove_next_hop(self.client, nhop1)

    def anycastMacSupportSetAttrSvi(self):
        '''
        Verify if packets are forwarded if coming on ingress with
        anycast_mac as dst_addr for SVI RIF and if src_addr
        is correctly set on egress.
        Test verifies the case when anycast_mac_support is set
        after both RIFs are created.
        '''
        print("anycastMacSupportSetAttrSvi()")

        dmac1 = '00:11:22:33:44:55'
        dmac2 = '00:11:22:33:44:66'
        new_router_mac = '00:33:22:33:44:55'

        nhop1 = sai_thrift_create_next_hop(
            self.client,
            ip=sai_ipaddress('10.10.10.2'),
            router_interface_id=self.port10_rif,
            type=SAI_NEXT_HOP_TYPE_IP)
        neighbor_entry1 = sai_thrift_neighbor_entry_t(
            rif_id=self.port10_rif, ip_address=sai_ipaddress('10.10.10.2'))
        sai_thrift_create_neighbor_entry(
            self.client, neighbor_entry1, dst_mac_address=dmac1)

        route_entry1 = sai_thrift_route_entry_t(
            vr_id=self.default_vrf, destination=sai_ipprefix('10.10.10.1/32'))
        sai_thrift_create_route_entry(
            self.client, route_entry1, next_hop_id=nhop1)

        nhop2 = sai_thrift_create_next_hop(
            self.client,
            ip=sai_ipaddress('30.10.10.1'),
            router_interface_id=self.vlan30_rif,
            type=SAI_NEXT_HOP_TYPE_IP)
        neighbor_entry2 = sai_thrift_neighbor_entry_t(
            rif_id=self.vlan30_rif, ip_address=sai_ipaddress('30.10.10.1'))
        sai_thrift_create_neighbor_entry(
            self.client, neighbor_entry2, dst_mac_address=dmac2)

        route_entry2 = sai_thrift_route_entry_t(
            vr_id=self.default_vrf, destination=sai_ipprefix('30.10.10.1/32'))
        sai_thrift_create_route_entry(
            self.client, route_entry2, next_hop_id=nhop2)

        pkt1 = simple_tcp_packet(
            eth_dst=ROUTER_MAC,
            eth_src='00:22:22:22:22:22',
            ip_dst='30.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        pkt2 = simple_tcp_packet(
            eth_dst=ROUTER_MAC,
            eth_src='00:22:22:22:22:22',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        pkt3 = simple_tcp_packet(
            eth_dst=new_router_mac,
            eth_src='00:22:22:22:22:22',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        vlan_pkt1 = simple_tcp_packet(
            eth_dst=ROUTER_MAC,
            eth_src='00:22:22:22:22:22',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            dl_vlan_enable=True,
            vlan_vid=30,
            ip_id=105,
            ip_ttl=64,
            pktlen=104)
        vlan_pkt2 = simple_tcp_packet(
            eth_dst=new_router_mac,
            eth_src='00:22:22:22:22:22',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            dl_vlan_enable=True,
            vlan_vid=30,
            ip_id=105,
            ip_ttl=64,
            pktlen=104)
        exp_pkt1 = simple_tcp_packet(
            eth_dst=dmac1,
            eth_src=ROUTER_MAC,
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63,
            pktlen=100)
        exp_pkt2 = simple_tcp_packet(
            eth_dst=dmac2,
            eth_src=new_router_mac,
            ip_dst='30.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63,
            pktlen=100)
        try:
            print("Creating virtual RIF on VLAN30 with src_mac=%s "
                  "and anycast_mac_support set to false" % new_router_mac)
            virtual_rif = sai_thrift_create_router_interface(
                self.client,
                type=SAI_ROUTER_INTERFACE_TYPE_VLAN,
                virtual_router_id=self.default_vrf,
                vlan_id=self.vlan30,
                is_virtual=True,
                src_mac_address=new_router_mac)

            print("Setting anycast_mac_support to true for virtual SVI 30")
            attr_value = sai_thrift_attribute_value_t(booldata=True)
            attr = sai_thrift_attribute_t(
                id=(SAI_ROUTER_INTERFACE_ATTR_END + 0), value=attr_value)
            sai_thrift_set_router_interface_attribute(
                self.client,
                virtual_rif,
                custom_attribute=attr)

            print("Sending tagged packet on port %d, forward" %
                  self.dev_port20)
            send_packet(self, self.dev_port21, vlan_pkt1)
            verify_packet(self, exp_pkt1, self.dev_port10)

            print("Sending untagged packet on port %d, forward" %
                  self.dev_port20)
            send_packet(self, self.dev_port20, pkt2)
            verify_packet(self, exp_pkt1, self.dev_port10)

            print("Sending tagged packet on port %d, forward "
                  "(eth_dst=%s)" % (self.dev_port20, new_router_mac))
            send_packet(self, self.dev_port21, vlan_pkt2)
            verify_packet(self, exp_pkt1, self.dev_port10)

            print("Sending untagged packet on port %d, forward "
                  "(eth_dst=%s)" % (self.dev_port20, new_router_mac))
            send_packet(self, self.dev_port20, pkt3)
            verify_packet(self, exp_pkt1, self.dev_port10)

            print("Creating an fdb entry for SVI 30 port %d" % self.dev_port20)
            fdb_entry = sai_thrift_fdb_entry_t(
                switch_id=self.switch_id,
                mac_address=dmac2,
                bv_id=self.vlan30)
            sai_thrift_create_fdb_entry(
                self.client,
                fdb_entry,
                type=SAI_FDB_ENTRY_TYPE_STATIC,
                bridge_port_id=self.port20_bp,
                packet_action=SAI_PACKET_ACTION_FORWARD)

            print("Sending untagged packet on port %d, forward "
                  "(expected eth_src=%s)" % (self.dev_port10, new_router_mac))
            send_packet(self, self.dev_port10, pkt1)
            verify_packet(self, exp_pkt2, self.dev_port20)

        finally:
            sai_thrift_remove_fdb_entry(self.client, fdb_entry)
            sai_thrift_remove_router_interface(self.client, virtual_rif)
            sai_thrift_remove_route_entry(self.client, route_entry2)
            sai_thrift_remove_neighbor_entry(self.client, neighbor_entry2)
            sai_thrift_remove_next_hop(self.client, nhop2)
            sai_thrift_remove_route_entry(self.client, route_entry1)
            sai_thrift_remove_neighbor_entry(self.client, neighbor_entry1)
            sai_thrift_remove_next_hop(self.client, nhop1)

    def anycastMacSupportSetSrcMacAttrSvi(self):
        '''
        Verify if packets are forwarded if coming on ingress with
        anycast_mac as dst_addr for SVI RIF and if src_addr
        is correctly set on egress.
        Test verifies the case when src_mac of virtual RIF is modified.
        '''
        print("anycastMacSupportSetSrcMacAttrSvi()")

        dmac1 = '00:11:22:33:44:55'
        dmac2 = '00:11:22:33:44:66'
        new_router_mac1 = '00:33:22:33:44:55'
        new_router_mac2 = '00:44:22:33:44:55'

        nhop1 = sai_thrift_create_next_hop(
            self.client,
            ip=sai_ipaddress('10.10.10.2'),
            router_interface_id=self.port10_rif,
            type=SAI_NEXT_HOP_TYPE_IP)
        neighbor_entry1 = sai_thrift_neighbor_entry_t(
            rif_id=self.port10_rif, ip_address=sai_ipaddress('10.10.10.2'))
        sai_thrift_create_neighbor_entry(
            self.client, neighbor_entry1, dst_mac_address=dmac1)

        route_entry1 = sai_thrift_route_entry_t(
            vr_id=self.default_vrf, destination=sai_ipprefix('10.10.10.1/32'))
        sai_thrift_create_route_entry(
            self.client, route_entry1, next_hop_id=nhop1)

        nhop2 = sai_thrift_create_next_hop(
            self.client,
            ip=sai_ipaddress('30.10.10.1'),
            router_interface_id=self.vlan30_rif,
            type=SAI_NEXT_HOP_TYPE_IP)
        neighbor_entry2 = sai_thrift_neighbor_entry_t(
            rif_id=self.vlan30_rif, ip_address=sai_ipaddress('30.10.10.1'))
        sai_thrift_create_neighbor_entry(
            self.client, neighbor_entry2, dst_mac_address=dmac2)

        route_entry2 = sai_thrift_route_entry_t(
            vr_id=self.default_vrf, destination=sai_ipprefix('30.10.10.1/32'))
        sai_thrift_create_route_entry(
            self.client, route_entry2, next_hop_id=nhop2)

        pkt1 = simple_tcp_packet(
            eth_dst=ROUTER_MAC,
            eth_src='00:22:22:22:22:22',
            ip_dst='30.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        pkt2 = simple_tcp_packet(
            eth_dst=ROUTER_MAC,
            eth_src='00:22:22:22:22:22',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        pkt3 = simple_tcp_packet(
            eth_dst=new_router_mac1,
            eth_src='00:22:22:22:22:22',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        pkt4 = simple_tcp_packet(
            eth_dst=new_router_mac2,
            eth_src='00:22:22:22:22:22',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64)
        vlan_pkt1 = simple_tcp_packet(
            eth_dst=ROUTER_MAC,
            eth_src='00:22:22:22:22:22',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            dl_vlan_enable=True,
            vlan_vid=30,
            ip_id=105,
            ip_ttl=64,
            pktlen=104)
        vlan_pkt2 = simple_tcp_packet(
            eth_dst=new_router_mac1,
            eth_src='00:22:22:22:22:22',
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            dl_vlan_enable=True,
            vlan_vid=30,
            ip_id=105,
            ip_ttl=64,
            pktlen=104)
        exp_pkt1 = simple_tcp_packet(
            eth_dst=dmac1,
            eth_src=ROUTER_MAC,
            ip_dst='10.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63,
            pktlen=100)
        exp_pkt2 = simple_tcp_packet(
            eth_dst=dmac2,
            eth_src=new_router_mac1,
            ip_dst='30.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63,
            pktlen=100)
        exp_pkt3 = simple_tcp_packet(
            eth_dst=dmac2,
            eth_src=new_router_mac2,
            ip_dst='30.10.10.1',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63,
            pktlen=100)
        try:
            print("Creating virtual RIF on VLAN30 with src_mac=%s "
                  "and anycast_mac_support set to false" % new_router_mac1)
            virtual_rif = sai_thrift_create_router_interface(
                self.client,
                type=SAI_ROUTER_INTERFACE_TYPE_VLAN,
                virtual_router_id=self.default_vrf,
                vlan_id=self.vlan30,
                is_virtual=True,
                src_mac_address=new_router_mac1)

            print("Setting anycast_mac_support to true for virtual SVI 30")
            attr_value = sai_thrift_attribute_value_t(booldata=True)
            attr = sai_thrift_attribute_t(
                id=(SAI_ROUTER_INTERFACE_ATTR_END + 0), value=attr_value)
            sai_thrift_set_router_interface_attribute(
                self.client,
                virtual_rif,
                custom_attribute=attr)

            print("Sending tagged packet on port %d, forward" %
                  self.dev_port20)
            send_packet(self, self.dev_port21, vlan_pkt1)
            verify_packet(self, exp_pkt1, self.dev_port10)

            print("Sending untagged packet on port %d, forward" %
                  self.dev_port20)
            send_packet(self, self.dev_port20, pkt2)
            verify_packet(self, exp_pkt1, self.dev_port10)

            print("Sending tagged packet on port %d, forward "
                  "(eth_dst=%s)" % (self.dev_port20, new_router_mac1))
            send_packet(self, self.dev_port21, vlan_pkt2)
            verify_packet(self, exp_pkt1, self.dev_port10)

            print("Sending untagged packet on port %d, forward "
                  "(eth_dst=%s)" % (self.dev_port20, new_router_mac1))
            send_packet(self, self.dev_port20, pkt3)
            verify_packet(self, exp_pkt1, self.dev_port10)

            print("Creating an fdb entry for SVI 30 port %d" % self.dev_port20)
            fdb_entry = sai_thrift_fdb_entry_t(
                switch_id=self.switch_id,
                mac_address=dmac2,
                bv_id=self.vlan30)
            sai_thrift_create_fdb_entry(
                self.client,
                fdb_entry,
                type=SAI_FDB_ENTRY_TYPE_STATIC,
                bridge_port_id=self.port20_bp,
                packet_action=SAI_PACKET_ACTION_FORWARD)

            print("Sending untagged packet on port %d, forward "
                  "(expected eth_src=%s)" % (self.dev_port10, new_router_mac1))
            send_packet(self, self.dev_port10, pkt1)
            verify_packet(self, exp_pkt2, self.dev_port20)

            print("Setting virtual RIF src mac to " + new_router_mac2)
            sai_thrift_set_router_interface_attribute(
                self.client,
                virtual_rif,
                src_mac_address=new_router_mac2)
            print("Sending untagged packet on port %d, forward "
                  "(eth_dst=%s)" % (self.dev_port20, new_router_mac2))
            send_packet(self, self.dev_port20, pkt4)
            verify_packet(self, exp_pkt1, self.dev_port10)

            print("Sending untagged packet on port %d, forward "
                  "(expected eth_src=%s)" % (self.dev_port10, new_router_mac2))
            send_packet(self, self.dev_port10, pkt1)
            verify_packet(self, exp_pkt3, self.dev_port20)

        finally:
            sai_thrift_remove_fdb_entry(self.client, fdb_entry)
            sai_thrift_remove_router_interface(self.client, virtual_rif)
            sai_thrift_remove_route_entry(self.client, route_entry2)
            sai_thrift_remove_neighbor_entry(self.client, neighbor_entry2)
            sai_thrift_remove_next_hop(self.client, nhop2)
            sai_thrift_remove_route_entry(self.client, route_entry1)
            sai_thrift_remove_neighbor_entry(self.client, neighbor_entry1)
            sai_thrift_remove_next_hop(self.client, nhop1)

    def anycastMacSupportAttrSubPort(self):
        '''
        Verify if packets are forwarded if coming on ingress with
        anycast_mac as dst_addr for Sub Port RIF and if src_addr
        is correctly set on egress. Tested for both cases when
        virtual RIF is created before regular RIF and when regular RIF
        is created before virtual RIF.
        '''
        print("anycastMacSupportAttrSubPort()")

        new_router_mac = '00:22:22:33:44:55'

        subport10_100 = sai_thrift_create_router_interface(
            self.client,
            type=SAI_ROUTER_INTERFACE_TYPE_SUB_PORT,
            virtual_router_id=self.default_vrf,
            port_id=self.port10,
            outer_vlan_id=100)
        nhop_sp10_100 = sai_thrift_create_next_hop(
            self.client,
            ip=sai_ipaddress('20.20.0.10'),
            router_interface_id=subport10_100,
            type=SAI_NEXT_HOP_TYPE_IP)
        nbr_entry_sp10_100 = sai_thrift_neighbor_entry_t(
            rif_id=subport10_100,
            ip_address=sai_ipaddress('20.20.0.10'))
        sai_thrift_create_neighbor_entry(
            self.client,
            nbr_entry_sp10_100,
            dst_mac_address='00:33:33:33:01:00')
        route_entry_sp10_100 = sai_thrift_route_entry_t(
            vr_id=self.default_vrf,
            destination=sai_ipprefix('30.30.0.10/32'))
        sai_thrift_create_route_entry(
            self.client,
            route_entry_sp10_100,
            next_hop_id=nhop_sp10_100)

        subport10_200 = sai_thrift_create_router_interface(
            self.client,
            type=SAI_ROUTER_INTERFACE_TYPE_SUB_PORT,
            virtual_router_id=self.default_vrf,
            port_id=self.port10,
            outer_vlan_id=200)
        nhop_sp10_200 = sai_thrift_create_next_hop(
            self.client,
            ip=sai_ipaddress('20.20.0.20'),
            router_interface_id=subport10_200,
            type=SAI_NEXT_HOP_TYPE_IP)
        nbr_entry_sp10_200 = sai_thrift_neighbor_entry_t(
            rif_id=subport10_200,
            ip_address=sai_ipaddress('20.20.0.20'))
        sai_thrift_create_neighbor_entry(
            self.client,
            nbr_entry_sp10_200,
            dst_mac_address='00:33:33:33:02:00')
        route_entry_sp10_200 = sai_thrift_route_entry_t(
            vr_id=self.default_vrf,
            destination=sai_ipprefix('30.30.0.20/32'))
        sai_thrift_create_route_entry(
            self.client,
            route_entry_sp10_200,
            next_hop_id=nhop_sp10_200)

        pkt1 = simple_tcp_packet(
            eth_dst=ROUTER_MAC,
            eth_src='00:22:22:22:22:22',
            ip_dst='30.30.0.20',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64,
            dl_vlan_enable=True,
            vlan_vid=100,
            pktlen=104)
        pkt2 = simple_tcp_packet(
            eth_dst=new_router_mac,
            eth_src='00:22:22:22:22:22',
            ip_dst='30.30.0.20',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64,
            dl_vlan_enable=True,
            vlan_vid=100,
            pktlen=104)
        exp_pkt1 = simple_tcp_packet(
            eth_dst='00:33:33:33:02:00',
            eth_src=ROUTER_MAC,
            ip_dst='30.30.0.20',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63,
            dl_vlan_enable=True,
            vlan_vid=200,
            pktlen=104)
        exp_pkt2 = simple_tcp_packet(
            eth_dst='00:33:33:33:02:00',
            eth_src=new_router_mac,
            ip_dst='30.30.0.20',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63,
            dl_vlan_enable=True,
            vlan_vid=200,
            pktlen=104)

        try:
            print("Sending packet on port %d, forward" % self.dev_port10)
            send_packet(self, self.dev_port10, pkt1)
            verify_packet(self, exp_pkt1, self.dev_port10)

            print("Sending packet on port %d, drop "
                  "(eth_dst=%s)" % (self.dev_port10, new_router_mac))
            send_packet(self, self.dev_port10, pkt2)
            verify_no_other_packets(self, timeout=1)

            print("Creating first virtual RIF with src_mac=%s "
                  % new_router_mac)
            virtual_rif1 = sai_thrift_create_router_interface(
                self.client,
                type=SAI_ROUTER_INTERFACE_TYPE_SUB_PORT,
                virtual_router_id=self.default_vrf,
                port_id=self.port10,
                is_virtual=True,
                src_mac_address=new_router_mac,
                outer_vlan_id=100)

            print("Sending packet on port %d, "
                  "forward (eth_dst=%s)" %
                  (self.dev_port10, new_router_mac))
            send_packet(self, self.dev_port10, pkt2)
            verify_packet(self, exp_pkt1, self.dev_port10)

            print("Creating second virtual RIF with src_mac=%s "
                  "and anycast_mac_support set to true" % new_router_mac)
            attr_value = sai_thrift_attribute_value_t(booldata=True)
            attr = sai_thrift_attribute_t(
                id=(SAI_ROUTER_INTERFACE_ATTR_END + 0), value=attr_value)
            virtual_rif2 = sai_thrift_create_router_interface(
                self.client,
                type=SAI_ROUTER_INTERFACE_TYPE_SUB_PORT,
                virtual_router_id=self.default_vrf,
                port_id=self.port10,
                is_virtual=True,
                src_mac_address=new_router_mac,
                outer_vlan_id=200,
                custom_attribute=attr)

            print("Sending packet on port %d, "
                  "forward (expected eth_src=%s)" %
                  (self.dev_port10, new_router_mac))
            send_packet(self, self.dev_port10, pkt2)
            verify_packet(self, exp_pkt2, self.dev_port10)

        finally:
            sai_thrift_remove_router_interface(self.client, virtual_rif2)
            sai_thrift_remove_router_interface(self.client, virtual_rif1)
            sai_thrift_remove_route_entry(self.client, route_entry_sp10_200)
            sai_thrift_remove_neighbor_entry(self.client, nbr_entry_sp10_200)
            sai_thrift_remove_next_hop(self.client, nhop_sp10_200)
            sai_thrift_remove_router_interface(self.client, subport10_200)
            sai_thrift_remove_route_entry(self.client, route_entry_sp10_100)
            sai_thrift_remove_neighbor_entry(self.client, nbr_entry_sp10_100)
            sai_thrift_remove_next_hop(self.client, nhop_sp10_100)
            sai_thrift_remove_router_interface(self.client, subport10_100)

    def anycastMacSupportSetAttrSubPort(self):
        '''
        Verify if packets are forwarded if coming on ingress with
        anycast_mac as dst_addr for Sub Port RIF and if src_addr
        is correctly set on egress.
        Test verifies the case when anycast_mac_support is set
        after both RIFs are created.
        '''
        print("anycastMacSupportSetAttrSubPort()")

        new_router_mac = '00:22:22:33:44:55'

        subport10_100 = sai_thrift_create_router_interface(
            self.client,
            type=SAI_ROUTER_INTERFACE_TYPE_SUB_PORT,
            virtual_router_id=self.default_vrf,
            port_id=self.port10,
            outer_vlan_id=100)
        nhop_sp10_100 = sai_thrift_create_next_hop(
            self.client,
            ip=sai_ipaddress('20.20.0.10'),
            router_interface_id=subport10_100,
            type=SAI_NEXT_HOP_TYPE_IP)
        nbr_entry_sp10_100 = sai_thrift_neighbor_entry_t(
            rif_id=subport10_100,
            ip_address=sai_ipaddress('20.20.0.10'))
        sai_thrift_create_neighbor_entry(
            self.client,
            nbr_entry_sp10_100,
            dst_mac_address='00:33:33:33:01:00')
        route_entry_sp10_100 = sai_thrift_route_entry_t(
            vr_id=self.default_vrf,
            destination=sai_ipprefix('30.30.0.10/32'))
        sai_thrift_create_route_entry(
            self.client,
            route_entry_sp10_100,
            next_hop_id=nhop_sp10_100)

        subport10_200 = sai_thrift_create_router_interface(
            self.client,
            type=SAI_ROUTER_INTERFACE_TYPE_SUB_PORT,
            virtual_router_id=self.default_vrf,
            port_id=self.port10,
            outer_vlan_id=200)
        nhop_sp10_200 = sai_thrift_create_next_hop(
            self.client,
            ip=sai_ipaddress('20.20.0.20'),
            router_interface_id=subport10_200,
            type=SAI_NEXT_HOP_TYPE_IP)
        nbr_entry_sp10_200 = sai_thrift_neighbor_entry_t(
            rif_id=subport10_200,
            ip_address=sai_ipaddress('20.20.0.20'))
        sai_thrift_create_neighbor_entry(
            self.client,
            nbr_entry_sp10_200,
            dst_mac_address='00:33:33:33:02:00')
        route_entry_sp10_200 = sai_thrift_route_entry_t(
            vr_id=self.default_vrf,
            destination=sai_ipprefix('30.30.0.20/32'))
        sai_thrift_create_route_entry(
            self.client,
            route_entry_sp10_200,
            next_hop_id=nhop_sp10_200)

        pkt1 = simple_tcp_packet(
            eth_dst=ROUTER_MAC,
            eth_src='00:22:22:22:22:22',
            ip_dst='30.30.0.20',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64,
            dl_vlan_enable=True,
            vlan_vid=100,
            pktlen=104)
        pkt2 = simple_tcp_packet(
            eth_dst=new_router_mac,
            eth_src='00:22:22:22:22:22',
            ip_dst='30.30.0.20',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64,
            dl_vlan_enable=True,
            vlan_vid=100,
            pktlen=104)
        exp_pkt1 = simple_tcp_packet(
            eth_dst='00:33:33:33:02:00',
            eth_src=ROUTER_MAC,
            ip_dst='30.30.0.20',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63,
            dl_vlan_enable=True,
            vlan_vid=200,
            pktlen=104)
        exp_pkt2 = simple_tcp_packet(
            eth_dst='00:33:33:33:02:00',
            eth_src=new_router_mac,
            ip_dst='30.30.0.20',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63,
            dl_vlan_enable=True,
            vlan_vid=200,
            pktlen=104)

        try:
            print("Sending packet on port %d, forward" % self.dev_port10)
            send_packet(self, self.dev_port10, pkt1)
            verify_packet(self, exp_pkt1, self.dev_port10)

            print("Sending packet on port %d, drop "
                  "(eth_dst=%s)" % (self.dev_port10, new_router_mac))
            send_packet(self, self.dev_port10, pkt2)
            verify_no_other_packets(self, timeout=1)

            print("Creating first virtual RIF with src_mac=%s"
                  % new_router_mac)
            virtual_rif1 = sai_thrift_create_router_interface(
                self.client,
                type=SAI_ROUTER_INTERFACE_TYPE_SUB_PORT,
                virtual_router_id=self.default_vrf,
                port_id=self.port10,
                is_virtual=True,
                src_mac_address=new_router_mac,
                outer_vlan_id=100)

            print("Sending packet on port %d, "
                  "forward (eth_dst=%s)" %
                  (self.dev_port10, new_router_mac))
            send_packet(self, self.dev_port10, pkt2)
            verify_packet(self, exp_pkt1, self.dev_port10)

            print("Creating second virtual RIF with src_mac=%s "
                  "and anycast_mac_support set to false" % new_router_mac)
            virtual_rif2 = sai_thrift_create_router_interface(
                self.client,
                type=SAI_ROUTER_INTERFACE_TYPE_SUB_PORT,
                virtual_router_id=self.default_vrf,
                port_id=self.port10,
                is_virtual=True,
                src_mac_address=new_router_mac,
                outer_vlan_id=200)

            print("Sending packet on port %d, "
                  "forward (expected eth_src=%s)" %
                  (self.dev_port10, ROUTER_MAC))
            send_packet(self, self.dev_port10, pkt2)
            verify_packet(self, exp_pkt1, self.dev_port10)

            print("Setting second virtual RIF anycas_mac_support to true")
            attr_value = sai_thrift_attribute_value_t(booldata=True)
            attr = sai_thrift_attribute_t(
                id=(SAI_ROUTER_INTERFACE_ATTR_END + 0), value=attr_value)
            sai_thrift_set_router_interface_attribute(
                self.client,
                virtual_rif2,
                custom_attribute=attr)

            print("Sending packet on port %d, "
                  "forward (expected eth_src=%s)" %
                  (self.dev_port10, new_router_mac))
            send_packet(self, self.dev_port10, pkt2)
            verify_packet(self, exp_pkt2, self.dev_port10)

        finally:
            sai_thrift_remove_router_interface(self.client, virtual_rif2)
            sai_thrift_remove_router_interface(self.client, virtual_rif1)
            sai_thrift_remove_route_entry(self.client, route_entry_sp10_200)
            sai_thrift_remove_neighbor_entry(self.client, nbr_entry_sp10_200)
            sai_thrift_remove_next_hop(self.client, nhop_sp10_200)
            sai_thrift_remove_router_interface(self.client, subport10_200)
            sai_thrift_remove_route_entry(self.client, route_entry_sp10_100)
            sai_thrift_remove_neighbor_entry(self.client, nbr_entry_sp10_100)
            sai_thrift_remove_next_hop(self.client, nhop_sp10_100)
            sai_thrift_remove_router_interface(self.client, subport10_100)

    def anycastMacSupportSetSrcMacAttrSubPort(self):
        '''
        Verify if packets are forwarded if coming on ingress with
        anycast_mac as dst_addr for Sub Port RIF and if src_addr
        is correctly set on egress.
        Test verifies the case when src_mac of virtual RIF is modified.
        '''
        print("anycastMacSupportSetSrcMacAttrSubPort()")

        new_router_mac = '00:22:22:33:44:55'
        new_router_mac2 = '00:33:22:33:44:55'
        new_router_mac3 = '00:44:22:33:44:55'

        subport10_100 = sai_thrift_create_router_interface(
            self.client,
            type=SAI_ROUTER_INTERFACE_TYPE_SUB_PORT,
            virtual_router_id=self.default_vrf,
            port_id=self.port10,
            outer_vlan_id=100)
        nhop_sp10_100 = sai_thrift_create_next_hop(
            self.client,
            ip=sai_ipaddress('20.20.0.10'),
            router_interface_id=subport10_100,
            type=SAI_NEXT_HOP_TYPE_IP)
        nbr_entry_sp10_100 = sai_thrift_neighbor_entry_t(
            rif_id=subport10_100,
            ip_address=sai_ipaddress('20.20.0.10'))
        sai_thrift_create_neighbor_entry(
            self.client,
            nbr_entry_sp10_100,
            dst_mac_address='00:33:33:33:01:00')
        route_entry_sp10_100 = sai_thrift_route_entry_t(
            vr_id=self.default_vrf,
            destination=sai_ipprefix('30.30.0.10/32'))
        sai_thrift_create_route_entry(
            self.client,
            route_entry_sp10_100,
            next_hop_id=nhop_sp10_100)

        subport10_200 = sai_thrift_create_router_interface(
            self.client,
            type=SAI_ROUTER_INTERFACE_TYPE_SUB_PORT,
            virtual_router_id=self.default_vrf,
            port_id=self.port10,
            outer_vlan_id=200)
        nhop_sp10_200 = sai_thrift_create_next_hop(
            self.client,
            ip=sai_ipaddress('20.20.0.20'),
            router_interface_id=subport10_200,
            type=SAI_NEXT_HOP_TYPE_IP)
        nbr_entry_sp10_200 = sai_thrift_neighbor_entry_t(
            rif_id=subport10_200,
            ip_address=sai_ipaddress('20.20.0.20'))
        sai_thrift_create_neighbor_entry(
            self.client,
            nbr_entry_sp10_200,
            dst_mac_address='00:33:33:33:02:00')
        route_entry_sp10_200 = sai_thrift_route_entry_t(
            vr_id=self.default_vrf,
            destination=sai_ipprefix('30.30.0.20/32'))
        sai_thrift_create_route_entry(
            self.client,
            route_entry_sp10_200,
            next_hop_id=nhop_sp10_200)

        pkt1 = simple_tcp_packet(
            eth_dst=ROUTER_MAC,
            eth_src='00:22:22:22:22:22',
            ip_dst='30.30.0.20',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64,
            dl_vlan_enable=True,
            vlan_vid=100,
            pktlen=104)
        pkt2 = simple_tcp_packet(
            eth_dst=new_router_mac,
            eth_src='00:22:22:22:22:22',
            ip_dst='30.30.0.20',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64,
            dl_vlan_enable=True,
            vlan_vid=100,
            pktlen=104)
        pkt3 = simple_tcp_packet(
            eth_dst=new_router_mac2,
            eth_src='00:22:22:22:22:22',
            ip_dst='30.30.0.20',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=64,
            dl_vlan_enable=True,
            vlan_vid=100,
            pktlen=104)
        exp_pkt1 = simple_tcp_packet(
            eth_dst='00:33:33:33:02:00',
            eth_src=ROUTER_MAC,
            ip_dst='30.30.0.20',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63,
            dl_vlan_enable=True,
            vlan_vid=200,
            pktlen=104)
        exp_pkt2 = simple_tcp_packet(
            eth_dst='00:33:33:33:02:00',
            eth_src=new_router_mac,
            ip_dst='30.30.0.20',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63,
            dl_vlan_enable=True,
            vlan_vid=200,
            pktlen=104)
        exp_pkt3 = simple_tcp_packet(
            eth_dst='00:33:33:33:02:00',
            eth_src=new_router_mac3,
            ip_dst='30.30.0.20',
            ip_src='192.168.0.1',
            ip_id=105,
            ip_ttl=63,
            dl_vlan_enable=True,
            vlan_vid=200,
            pktlen=104)

        try:
            print("Sending packet on port %d, forward" % self.dev_port10)
            send_packet(self, self.dev_port10, pkt1)
            verify_packet(self, exp_pkt1, self.dev_port10)

            print("Sending packet on port %d, drop "
                  "(eth_dst=%s)" % (self.dev_port10, new_router_mac))
            send_packet(self, self.dev_port10, pkt2)
            verify_no_other_packets(self, timeout=1)

            print("Creating first virtual RIF with src_mac=%s"
                  % new_router_mac)
            virtual_rif1 = sai_thrift_create_router_interface(
                self.client,
                type=SAI_ROUTER_INTERFACE_TYPE_SUB_PORT,
                virtual_router_id=self.default_vrf,
                port_id=self.port10,
                is_virtual=True,
                src_mac_address=new_router_mac,
                outer_vlan_id=100)

            print("Sending packet on port %d, "
                  "forward (eth_dst=%s)" %
                  (self.dev_port10, new_router_mac))
            send_packet(self, self.dev_port10, pkt2)
            verify_packet(self, exp_pkt1, self.dev_port10)

            print("Creating second virtual RIF with src_mac=%s"
                  % new_router_mac)
            attr_value = sai_thrift_attribute_value_t(booldata=True)
            attr = sai_thrift_attribute_t(
                id=(SAI_ROUTER_INTERFACE_ATTR_END + 0), value=attr_value)
            virtual_rif2 = sai_thrift_create_router_interface(
                self.client,
                type=SAI_ROUTER_INTERFACE_TYPE_SUB_PORT,
                virtual_router_id=self.default_vrf,
                port_id=self.port10,
                is_virtual=True,
                src_mac_address=new_router_mac,
                outer_vlan_id=200,
                custom_attribute=attr)

            print("Sending packet on port %d, "
                  "forward (expected eth_src=%s)" %
                  (self.dev_port10, new_router_mac))
            send_packet(self, self.dev_port10, pkt2)
            verify_packet(self, exp_pkt2, self.dev_port10)

            print("Setting first virtual RIF src mac to " + new_router_mac2)
            sai_thrift_set_router_interface_attribute(
                self.client,
                virtual_rif1,
                src_mac_address=new_router_mac2)

            print("Sending packet on port %d, "
                  "forward (expected eth_src=%s)" %
                  (self.dev_port10, new_router_mac2))
            send_packet(self, self.dev_port10, pkt3)
            verify_packet(self, exp_pkt2, self.dev_port10)

            print("Setting second virtual RIF src mac to " + new_router_mac3)
            sai_thrift_set_router_interface_attribute(
                self.client,
                virtual_rif2,
                src_mac_address=new_router_mac3)

            print("Sending packet on port %d, "
                  "forward (expected eth_src=%s)" %
                  (self.dev_port10, new_router_mac3))
            send_packet(self, self.dev_port10, pkt3)
            verify_packet(self, exp_pkt3, self.dev_port10)

        finally:
            sai_thrift_remove_router_interface(self.client, virtual_rif2)
            sai_thrift_remove_router_interface(self.client, virtual_rif1)
            sai_thrift_remove_route_entry(self.client, route_entry_sp10_200)
            sai_thrift_remove_neighbor_entry(self.client, nbr_entry_sp10_200)
            sai_thrift_remove_next_hop(self.client, nhop_sp10_200)
            sai_thrift_remove_router_interface(self.client, subport10_200)
            sai_thrift_remove_route_entry(self.client, route_entry_sp10_100)
            sai_thrift_remove_neighbor_entry(self.client, nbr_entry_sp10_100)
            sai_thrift_remove_next_hop(self.client, nhop_sp10_100)
            sai_thrift_remove_router_interface(self.client, subport10_100)
