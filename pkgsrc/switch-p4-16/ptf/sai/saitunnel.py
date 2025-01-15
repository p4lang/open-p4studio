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
Thrift SAI Tunnel tests
"""

import socket
import sys
from struct import pack, unpack

from switch_utils import *

import sai_base_test
from ptf.mask import Mask
from ptf import config
from ptf.testutils import *
from ptf.packet import *
from ptf.thriftutils import *
from switchsai_thrift.sai_headers import  *
from switch_utils import *


import os
this_dir = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(this_dir, '..'))
from common.utils import *
from common.pktpy_utils import pktpy_skip # noqa pylint: disable=wrong-import-position


def sai_thrift_create_tunnel_map(client, map_type):
    attr_list = []
    #tunnel map type
    attr_value = sai_thrift_attribute_value_t(s32=map_type)
    attr = sai_thrift_attribute_t(
        id=SAI_TUNNEL_MAP_ATTR_TYPE, value=attr_value)
    attr_list.append(attr)
    tunnel_map_id = client.sai_thrift_create_tunnel_map(attr_list)
    return tunnel_map_id


def sai_thrift_create_tunnel_map_entry(client, map_type, tunnel_map_id, ln,
                                       vlan, vrf, vni):
    attr_list = []
    #tunnel map type
    attr_value = sai_thrift_attribute_value_t(s32=map_type)
    attr = sai_thrift_attribute_t(
        id=SAI_TUNNEL_MAP_ENTRY_ATTR_TUNNEL_MAP_TYPE, value=attr_value)
    attr_list.append(attr)
    # tunnel map
    attr_value = sai_thrift_attribute_value_t(oid=tunnel_map_id)
    attr = sai_thrift_attribute_t(
        id=SAI_TUNNEL_MAP_ENTRY_ATTR_TUNNEL_MAP, value=attr_value)
    attr_list.append(attr)
    if (map_type == SAI_TUNNEL_MAP_TYPE_VNI_TO_BRIDGE_IF):
        #ln handle
        attr_value = sai_thrift_attribute_value_t(oid=ln)
        attr = sai_thrift_attribute_t(
            id=SAI_TUNNEL_MAP_ENTRY_ATTR_BRIDGE_ID_VALUE, value=attr_value)
        attr_list.append(attr)
    if (map_type == SAI_TUNNEL_MAP_TYPE_VNI_TO_VLAN_ID):
        #vlan handle
        attr_value = sai_thrift_attribute_value_t(u16=vlan)
        attr = sai_thrift_attribute_t(
            id=SAI_TUNNEL_MAP_ENTRY_ATTR_VLAN_ID_VALUE, value=attr_value)
        attr_list.append(attr)
    if (map_type == SAI_TUNNEL_MAP_TYPE_VNI_TO_VIRTUAL_ROUTER_ID):
        #vrf handle
        attr_value = sai_thrift_attribute_value_t(oid=vrf)
        attr = sai_thrift_attribute_t(
            id=SAI_TUNNEL_MAP_ENTRY_ATTR_VIRTUAL_ROUTER_ID_VALUE,
            value=attr_value)
        attr_list.append(attr)
    #vni handle
    attr_value = sai_thrift_attribute_value_t(u32=vni)
    attr = sai_thrift_attribute_t(
        id=SAI_TUNNEL_MAP_ENTRY_ATTR_VNI_ID_VALUE, value=attr_value)
    attr_list.append(attr)

    if (map_type == SAI_TUNNEL_MAP_TYPE_BRIDGE_IF_TO_VNI):
        #ln handle
        attr_value = sai_thrift_attribute_value_t(oid=ln)
        attr = sai_thrift_attribute_t(
            id=SAI_TUNNEL_MAP_ENTRY_ATTR_BRIDGE_ID_KEY, value=attr_value)
        attr_list.append(attr)
    if (map_type == SAI_TUNNEL_MAP_TYPE_VLAN_ID_TO_VNI):
        #vlan handle
        attr_value = sai_thrift_attribute_value_t(u16=vlan)
        attr = sai_thrift_attribute_t(
            id=SAI_TUNNEL_MAP_ENTRY_ATTR_VLAN_ID_KEY, value=attr_value)
        attr_list.append(attr)
    if (map_type == SAI_TUNNEL_MAP_TYPE_VIRTUAL_ROUTER_ID_TO_VNI):
        #vrf handle
        attr_value = sai_thrift_attribute_value_t(oid=vrf)
        attr = sai_thrift_attribute_t(
            id=SAI_TUNNEL_MAP_ENTRY_ATTR_VIRTUAL_ROUTER_ID_KEY,
            value=attr_value)
        attr_list.append(attr)
    #vni handle
    attr_value = sai_thrift_attribute_value_t(u32=vni)
    attr = sai_thrift_attribute_t(
        id=SAI_TUNNEL_MAP_ENTRY_ATTR_VNI_ID_KEY, value=attr_value)
    attr_list.append(attr)
    tunnel_map_entry = client.sai_thrift_create_tunnel_map_entry(attr_list)
    return tunnel_map_entry


def sai_thrift_create_tunnel_fdb(client,
                                 bv_id,
                                 mac,
                                 bport_oid,
                                 mac_action,
                                 dst_ip='0.0.0.0'):
    fdb_entry = sai_thrift_fdb_entry_t(mac_address=mac, bv_id=bv_id)
    #value 0 represents static entry, id=0, represents entry type
    fdb_attribute1_value = sai_thrift_attribute_value_t(s32=SAI_FDB_ENTRY_TYPE_STATIC)
    fdb_attribute1 = sai_thrift_attribute_t(id=SAI_FDB_ENTRY_ATTR_TYPE,
                                            value=fdb_attribute1_value)
    #value oid represents object id, id=1 represents port id
    fdb_attribute2_value = sai_thrift_attribute_value_t(oid=bport_oid)
    fdb_attribute2 = sai_thrift_attribute_t(id=SAI_FDB_ENTRY_ATTR_BRIDGE_PORT_ID,
                                            value=fdb_attribute2_value)
    #value oid represents object id, id=1 represents port id
    fdb_attribute3_value = sai_thrift_attribute_value_t(s32=mac_action)
    fdb_attribute3 = sai_thrift_attribute_t(id=SAI_FDB_ENTRY_ATTR_PACKET_ACTION,
                                            value=fdb_attribute3_value)
    #value oid represents object id, id=2 represents endpoint ip
    addr = sai_thrift_ip_t(ip4=dst_ip)
    ip_addr = sai_thrift_ip_address_t(
        addr_family=SAI_IP_ADDR_FAMILY_IPV4, addr=addr)
    fdb_attribute4_value = sai_thrift_attribute_value_t(ipaddr=ip_addr)
    fdb_attribute4 = sai_thrift_attribute_t(
        id=SAI_FDB_ENTRY_ATTR_ENDPOINT_IP, value=fdb_attribute4_value)
    fdb_attr_list = [
        fdb_attribute1, fdb_attribute2, fdb_attribute3, fdb_attribute4
    ]
    client.sai_thrift_create_fdb_entry(
        thrift_fdb_entry=fdb_entry, thrift_attr_list=fdb_attr_list)


def sai_thrift_remove_tunnel_fdb(client, bv_id, mac):
    fdb_entry = sai_thrift_fdb_entry_t(mac_address=mac, bv_id=bv_id)
    client.sai_thrift_delete_fdb_entry(thrift_fdb_entry=fdb_entry)


@pktpy_skip  # TODO bf-pktpy
@group('tunnel')
class IPinIPEncapDecap(sai_base_test.ThriftInterfaceDataPlane):
    '''
    This performs ip-in-ip tunnel encap/decap
    11.11.11.1 | 2.2.2.3 | 2.2.2.2 | 10.10.10.1
    '''

    def runTest(self):
        try:
            print
            switch_init(self.client)
            port1 = port_list[0]
            port2 = port_list[1]
            tunnel_type = SAI_TUNNEL_TYPE_IPINIP
            term_type = SAI_TUNNEL_TERM_TABLE_ENTRY_TYPE_P2P
            remote_vtep_ip = '2.2.2.2'
            my_vtep_ip = '2.2.2.3'

            v4 = 1
            v6 = 1
            vrf = sai_thrift_create_virtual_router(self.client, v4, v6)

            #overlay rif
            rif1 = sai_thrift_create_router_interface(self.client, vrf, SAI_ROUTER_INTERFACE_TYPE_PORT,
                                                      port1, 0, v4, v6, 0)
            #underlay rif
            rif2 = sai_thrift_create_router_interface(self.client, vrf, SAI_ROUTER_INTERFACE_TYPE_PORT,
                                                      port2, 0, v4, v6, 0)
            #tunnel creation
            orif = sai_thrift_create_router_interface(self.client, vrf, SAI_ROUTER_INTERFACE_TYPE_LOOPBACK,
                                                      0, 0, v4, v6, router_mac)
            urif = sai_thrift_create_router_interface(self.client, vrf, SAI_ROUTER_INTERFACE_TYPE_LOOPBACK,
                                                      0, 0, v4, v6, router_mac)

            tunnel_id = sai_thrift_create_tunnel(self.client, tunnel_type, SAI_IP_ADDR_FAMILY_IPV4, my_vtep_ip, urif, orif)
            tunnel_term_id = sai_thrift_create_tunnel_term_table(
                self.client, tunnel_type, SAI_IP_ADDR_FAMILY_IPV4, vrf, my_vtep_ip, remote_vtep_ip, tunnel_id, term_type)

            # static route to tunnel
            tunnel_nhop = sai_thrift_create_nhop(
                self.client, SAI_IP_ADDR_FAMILY_IPV4, remote_vtep_ip, tunnel_id, True, 0, '00:55:55:55:55:55')
            sai_thrift_create_route(self.client, vrf, SAI_IP_ADDR_FAMILY_IPV4,
                                    '10.10.10.0', '255.255.255.0', tunnel_nhop)

            # static route to underlay rif
            rif2_nhop = sai_thrift_create_nhop(
                self.client, SAI_IP_ADDR_FAMILY_IPV4, remote_vtep_ip, rif2)
            sai_thrift_create_neighbor(self.client, SAI_IP_ADDR_FAMILY_IPV4,
                                       rif2, remote_vtep_ip, '00:44:44:44:44:44')

            # static route to overlay rif
            rif1_nhop = sai_thrift_create_nhop(
                self.client, SAI_IP_ADDR_FAMILY_IPV4, '11.11.11.1', rif1)
            sai_thrift_create_neighbor(self.client, SAI_IP_ADDR_FAMILY_IPV4,
                                       rif1, '11.11.11.1', '00:22:22:22:22:22')

            print "Verifying 4in4 (ip in ip encap)"
            pkt = simple_tcp_packet(
                eth_dst=router_mac,
                eth_src='00:22:22:22:22:22',
                ip_dst='10.10.10.1',
                ip_src='11.11.11.1',
                ip_id=105,
                ip_ttl=64)
            pkt2 = simple_tcp_packet(
                eth_dst='00:44:44:44:44:44',
                eth_src=router_mac,
                ip_dst='10.10.10.1',
                ip_src='11.11.11.1',
                ip_id=105,
                ip_ttl=63)
            ipip_pkt = simple_ipv4ip_packet(
                eth_src=router_mac,
                eth_dst='00:44:44:44:44:44',
                ip_id=0,
                ip_src='2.2.2.3',
                ip_dst='2.2.2.2',
                ip_ttl=64,
                inner_frame=pkt2['IP'])
            send_packet(self,switch_ports[0], str(pkt))
            verify_packet(self, ipip_pkt, switch_ports[1])

            print "Verifying 4in4 (ip in ip decap)"
            pkt1 = simple_tcp_packet(
                eth_src=router_mac,
                eth_dst='00:22:22:22:22:22',
                ip_dst='11.11.11.1',
                ip_src='10.10.10.1',
                ip_id=108,
                ip_ttl=64)
            ipip_pkt = simple_ipv4ip_packet(
                eth_dst=router_mac,
                eth_src='00:44:44:44:44:44',
                ip_id=0,
                ip_src='2.2.2.2',
                ip_dst='2.2.2.3',
                ip_ttl=64,
                inner_frame=pkt1['IP'])
            pkt2 = simple_tcp_packet(
                eth_src=router_mac,
                eth_dst='00:22:22:22:22:22',
                ip_dst='11.11.11.1',
                ip_src='10.10.10.1',
                ip_id=108,
                ip_ttl=63)
            send_packet(self,switch_ports[1], str(ipip_pkt))
            verify_packet(self, pkt2, switch_ports[0])

        finally:
            #cleanup
            sai_thrift_remove_neighbor(self.client, SAI_IP_ADDR_FAMILY_IPV4,
                                       rif1, '11.11.11.1', '00:22:22:22:22:22')
            sai_thrift_remove_nhop(self.client, [rif1_nhop])
            sai_thrift_remove_neighbor(self.client, SAI_IP_ADDR_FAMILY_IPV4,
                                       rif2, '2.2.2.2', '00:44:44:44:44:44')
            sai_thrift_remove_nhop(self.client, [rif2_nhop])
            sai_thrift_remove_route(self.client, vrf, SAI_IP_ADDR_FAMILY_IPV4,
                                    '10.10.10.0', '255.255.255.0', tunnel_nhop)
            sai_thrift_remove_nhop(self.client, [tunnel_nhop])
            self.client.sai_thrift_remove_tunnel_term(tunnel_term_id)
            self.client.sai_thrift_remove_tunnel(tunnel_id)
            self.client.sai_thrift_remove_router_interface(orif)
            self.client.sai_thrift_remove_router_interface(urif)
            self.client.sai_thrift_remove_router_interface(rif1)
            self.client.sai_thrift_remove_router_interface(rif2)
            self.client.sai_thrift_remove_virtual_router(vrf)

@group('tunnel')
@disabled
class VlanVxlanEncapDecap(sai_base_test.ThriftInterfaceDataPlane):
    '''
    This performs L2 Vxlan tunnel encap/decap
    '''

    def runTest(self):
        try:
            print
            switch_init(self.client)
            port1 = port_list[0]
            port2 = port_list[1]
            tunnel_type = SAI_TUNNEL_TYPE_VXLAN
            term_type = SAI_TUNNEL_TERM_TABLE_ENTRY_TYPE_P2P
            remote_vtep_ip = '2.2.2.2'
            my_vtep_ip = '2.2.2.3'
            decap_map_type = SAI_TUNNEL_MAP_TYPE_VNI_TO_VLAN_ID
            encap_map_type = SAI_TUNNEL_MAP_TYPE_VLAN_ID_TO_VNI
            vni = 10000

            vlan_id = 10
            vlan_oid = sai_thrift_create_vlan(self.client, vlan_id)
            vlan_member1 = sai_thrift_create_vlan_member(
                self.client, vlan_oid, port1, SAI_VLAN_TAGGING_MODE_UNTAGGED)

            attr_value = sai_thrift_attribute_value_t(u16=vlan_id)
            attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
            self.client.sai_thrift_set_port_attribute(port1, attr)

            v4 = 1
            v6 = 1
            uvrf = sai_thrift_create_virtual_router(self.client, v4, v6)

            #underlay rif
            rif2 = sai_thrift_create_router_interface(
                self.client, uvrf, SAI_ROUTER_INTERFACE_TYPE_PORT, port2, 0, v4, v6, router_mac)

            #tunnel creation
            urif = sai_thrift_create_router_interface(self.client, uvrf, SAI_ROUTER_INTERFACE_TYPE_LOOPBACK,
                                                  0, 0, v4, v6, router_mac)

            decap_tun_map = sai_thrift_create_tunnel_map(self.client, decap_map_type)
            decap_tun_map_entry = sai_thrift_create_tunnel_map_entry(
                self.client, decap_map_type, decap_tun_map, 0, vlan_id, 0, vni)
            encap_tun_map = sai_thrift_create_tunnel_map(self.client, encap_map_type)
            encap_tun_map_entry = sai_thrift_create_tunnel_map_entry(
                self.client, encap_map_type, encap_tun_map, 0, vlan_id, 0, vni)

            tunnel_id = sai_thrift_create_tunnel(
                self.client,
                tunnel_type,
                SAI_IP_ADDR_FAMILY_IPV4,
                my_vtep_ip,
                urif,
                0,
                imap=encap_tun_map,
                emap=decap_tun_map)
            tunnel_term_id = sai_thrift_create_tunnel_term_table(
                self.client, tunnel_type, SAI_IP_ADDR_FAMILY_IPV4, uvrf, my_vtep_ip, remote_vtep_ip, tunnel_id, term_type)

            # this basically fetches the tunnel interface reference
            br_port_id = sai_thrift_create_bridge_port(
                self.client, SAI_BRIDGE_PORT_TYPE_TUNNEL, tunnel_id, 0)

            sai_thrift_create_fdb(
                self.client,
                vlan_oid,
                '00:22:22:22:22:22',
                port1,
                SAI_PACKET_ACTION_FORWARD)

            # static route to underlay rif
            sai_thrift_create_neighbor(self.client, SAI_IP_ADDR_FAMILY_IPV4,
                                       rif2, remote_vtep_ip, '00:44:44:44:44:44')
            rif2_nhop = sai_thrift_create_nhop(self.client, SAI_IP_ADDR_FAMILY_IPV4,
                                               remote_vtep_ip, rif2)
            sai_thrift_create_route(self.client, uvrf, SAI_IP_ADDR_FAMILY_IPV4,
                                    '2.2.2.0', '255.255.255.0', rif2_nhop)

            tun_nhop = sai_thrift_create_nhop(self.client, SAI_IP_ADDR_FAMILY_IPV4, remote_vtep_ip, tunnel_id, True)

            sai_thrift_create_tunnel_fdb(
                                 self.client,
                                 vlan_oid,
                                 '00:11:11:11:11:11',
                                 br_port_id,
                                 SAI_PACKET_ACTION_FORWARD,
                                 remote_vtep_ip)

            print "Verifying 4in4 (Vlan ip in vxlan encap)"
            pkt = simple_tcp_packet(
                eth_dst='00:11:11:11:11:11',
                eth_src='00:22:22:22:22:22',
                ip_dst='10.10.10.1',
                ip_src='11.11.11.1',
                ip_id=105,
                ip_ttl=64)
            vxlan_pkt = Mask(simple_vxlan_packet(
                eth_src=router_mac,
                eth_dst='00:44:44:44:44:44',
                ip_id=0,
                ip_src=my_vtep_ip,
                ip_dst=remote_vtep_ip,
                ip_ttl=64,
                ip_flags=0x2,
                with_udp_chksum=False,
                vxlan_vni=vni,
                inner_frame=pkt))
            vxlan_pkt.set_do_not_care_scapy(UDP, 'sport')
            send_packet(self,switch_ports[0], str(pkt))
            verify_packet(self, vxlan_pkt, switch_ports[1])

            print "Verifying 4in4 (Vlan ip in vxlan decap)"
            pkt1 = simple_tcp_packet(
                eth_dst='00:22:22:22:22:22',
                eth_src='00:11:11:11:11:11',
                ip_dst='11.11.11.1',
                ip_src='10.10.10.1',
                ip_id=108,
                ip_ttl=64)
            vxlan_pkt = simple_vxlan_packet(
                eth_dst=router_mac,
                eth_src='00:44:44:44:44:44',
                ip_id=0,
                ip_src=remote_vtep_ip,
                ip_dst=my_vtep_ip,
                ip_ttl=64,
                ip_flags=0x2,
                udp_sport=11638,
                vxlan_vni=vni,
                with_udp_chksum=False,
                inner_frame=pkt1)
            send_packet(self,switch_ports[1], str(vxlan_pkt))
            verify_packet(self, pkt1, switch_ports[0])

        finally:
            #cleanup
            sai_thrift_remove_route(self.client, uvrf, SAI_IP_ADDR_FAMILY_IPV4,
                                    '2.2.2.0', '255.255.255.0', rif2_nhop)
            sai_thrift_remove_nhop(self.client, [rif2_nhop])
            sai_thrift_remove_neighbor(self.client, SAI_IP_ADDR_FAMILY_IPV4,
                                       rif2, remote_vtep_ip, '00:44:44:44:44:44')
            sai_thrift_remove_tunnel_fdb(self.client, vlan_oid, '00:11:11:11:11:11')
            sai_thrift_remove_nhop(self.client, [tun_nhop])
            sai_thrift_delete_fdb(self.client, vlan_oid, '00:22:22:22:22:22', port1)
            self.client.sai_thrift_remove_bridge_port(br_port_id)
            self.client.sai_thrift_remove_tunnel_term(tunnel_term_id)
            self.client.sai_thrift_remove_tunnel(tunnel_id)
            self.client.sai_thrift_remove_tunnel_map_entry(encap_tun_map_entry)
            self.client.sai_thrift_remove_tunnel_map_entry(decap_tun_map_entry)
            self.client.sai_thrift_remove_tunnel_map(encap_tun_map)
            self.client.sai_thrift_remove_tunnel_map(decap_tun_map)
            self.client.sai_thrift_remove_router_interface(urif)
            self.client.sai_thrift_remove_router_interface(rif2)
            self.client.sai_thrift_remove_virtual_router(uvrf)
            attr_value = sai_thrift_attribute_value_t(u16=1)
            attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
            self.client.sai_thrift_set_port_attribute(port1, attr)
            self.client.sai_thrift_remove_vlan_member(vlan_member1)
            self.client.sai_thrift_remove_vlan(vlan_oid)


@pktpy_skip  # TODO bf-pktpy
@group('tunnel')
class L3VxlanEncapDecap(sai_base_test.ThriftInterfaceDataPlane):
    '''
    This performs ip-in-vxlan tunnel encap/decap
    '''

    def runTest(self):
        try:
            print
            switch_init(self.client)
            port1 = port_list[0]
            port2 = port_list[1]
            tunnel_type = SAI_TUNNEL_TYPE_VXLAN
            term_type = SAI_TUNNEL_TERM_TABLE_ENTRY_TYPE_P2P
            remote_vtep_ip = '2.2.2.2'
            my_vtep_ip = '2.2.2.3'
            encap_map_type = SAI_TUNNEL_MAP_TYPE_VIRTUAL_ROUTER_ID_TO_VNI
            decap_map_type = SAI_TUNNEL_MAP_TYPE_VNI_TO_VIRTUAL_ROUTER_ID
            vni = 10000

            v4 = 1
            v6 = 1
            ovrf = sai_thrift_create_virtual_router(self.client, v4, v6)
            uvrf = sai_thrift_create_virtual_router(self.client, v4, v6)

            #overlay rif
            rif1 = sai_thrift_create_router_interface(self.client, ovrf, SAI_ROUTER_INTERFACE_TYPE_PORT,
                                                      port1, 0, v4, v6, router_mac)
            #underlay rif
            rif2 = sai_thrift_create_router_interface(self.client, uvrf, SAI_ROUTER_INTERFACE_TYPE_PORT,
                                                      port2, 0, v4, v6, router_mac)

            #tunnel creation
            orif = sai_thrift_create_router_interface(self.client, ovrf, SAI_ROUTER_INTERFACE_TYPE_LOOPBACK,
                                                  0, 0, v4, v6, router_mac)
            urif = sai_thrift_create_router_interface(self.client, uvrf, SAI_ROUTER_INTERFACE_TYPE_LOOPBACK,
                                                  0, 0, v4, v6, router_mac)

            encap_tun_map = sai_thrift_create_tunnel_map(self.client, encap_map_type)
            encap_tun_map_entry = sai_thrift_create_tunnel_map_entry(
                self.client, encap_map_type, encap_tun_map, 0, 0, ovrf, vni)
            decap_tun_map = sai_thrift_create_tunnel_map(self.client, decap_map_type)
            decap_tun_map_entry = sai_thrift_create_tunnel_map_entry(
                self.client, decap_map_type, decap_tun_map, 0, 0, ovrf, vni)

            tunnel_id = sai_thrift_create_tunnel(
                self.client,
                tunnel_type,
                SAI_IP_ADDR_FAMILY_IPV4,
                my_vtep_ip,
                urif,
                orif,
                imap=encap_tun_map,
                emap=decap_tun_map)
            tunnel_term_id = sai_thrift_create_tunnel_term_table(
                self.client, tunnel_type, SAI_IP_ADDR_FAMILY_IPV4, uvrf, my_vtep_ip, remote_vtep_ip, tunnel_id, term_type)

            # static route to tunnel
            tunnel_nhop = sai_thrift_create_nhop(self.client, SAI_IP_ADDR_FAMILY_IPV4, remote_vtep_ip, tunnel_id,
                True, vni, '00:55:55:55:55:55')
            sai_thrift_create_route(self.client, ovrf, SAI_IP_ADDR_FAMILY_IPV4,
                                    '10.10.10.0', '255.255.255.0', tunnel_nhop)

            # static route to underlay rif
            rif2_nhop = sai_thrift_create_nhop(
                self.client, SAI_IP_ADDR_FAMILY_IPV4, remote_vtep_ip, rif2)
            sai_thrift_create_neighbor(self.client, SAI_IP_ADDR_FAMILY_IPV4,
                                       rif2, remote_vtep_ip, '00:44:44:44:44:44')
            sai_thrift_create_route(self.client, uvrf, SAI_IP_ADDR_FAMILY_IPV4,
                                    '2.2.2.0', '255.255.255.0', rif2_nhop)

            # static route to overlay rif
            rif1_nhop = sai_thrift_create_nhop(
                self.client, SAI_IP_ADDR_FAMILY_IPV4, '11.11.11.1', rif1)
            sai_thrift_create_neighbor(self.client, SAI_IP_ADDR_FAMILY_IPV4,
                                       rif1, '11.11.11.1', '00:22:22:22:22:22')
            sai_thrift_create_route(self.client, ovrf, SAI_IP_ADDR_FAMILY_IPV4,
                                    '11.11.11.1', '255.255.255.0', rif1_nhop)

            print "Verifying 4in4 (L3 ip in vxlan encap)"
            pkt = simple_tcp_packet(
                eth_dst=router_mac,
                eth_src='00:22:22:22:22:22',
                ip_dst='10.10.10.1',
                ip_src='11.11.11.1',
                ip_id=105,
                ip_ttl=64)
            pkt2 = simple_tcp_packet(
                eth_dst='00:55:55:55:55:55',
                eth_src=router_mac,
                ip_dst='10.10.10.1',
                ip_src='11.11.11.1',
                ip_id=105,
                ip_ttl=63)
            vxlan_pkt = Mask(simple_vxlan_packet(
                eth_src=router_mac,
                eth_dst='00:44:44:44:44:44',
                ip_id=0,
                ip_src='2.2.2.3',
                ip_dst='2.2.2.2',
                ip_ttl=64,
                ip_flags=0x2,
                with_udp_chksum=False,
                vxlan_vni=vni,
                inner_frame=pkt2))
            vxlan_pkt.set_do_not_care_scapy(UDP, 'sport')
            send_packet(self,switch_ports[0], str(pkt))
            verify_packet(self, vxlan_pkt, switch_ports[1])

            print "Verifying 4in4 (L3 ip in vxlan decap)"
            pkt1 = simple_tcp_packet(
                eth_dst=router_mac,
                eth_src='00:55:55:55:55:55',
                ip_dst='11.11.11.1',
                ip_src='10.10.10.1',
                ip_id=108,
                ip_ttl=64)
            vxlan_pkt = simple_vxlan_packet(
                eth_dst=router_mac,
                eth_src='00:44:44:44:44:44',
                ip_id=0,
                ip_src='2.2.2.2',
                ip_dst='2.2.2.3',
                ip_ttl=64,
                ip_flags=0x2,
                udp_sport=11638,
                vxlan_vni=vni,
                with_udp_chksum=False,
                inner_frame=pkt1)
            pkt2 = simple_tcp_packet(
                eth_src=router_mac,
                eth_dst='00:22:22:22:22:22',
                ip_dst='11.11.11.1',
                ip_src='10.10.10.1',
                ip_id=108,
                ip_ttl=63)
            send_packet(self,switch_ports[1], str(vxlan_pkt))
            verify_packet(self, pkt2, switch_ports[0])

        finally:
            #cleanup
            sai_thrift_remove_route(self.client, ovrf, SAI_IP_ADDR_FAMILY_IPV4,
                                    '11.11.11.1', '255.255.255.0', rif1)
            sai_thrift_remove_route(self.client, uvrf, SAI_IP_ADDR_FAMILY_IPV4,
                                    '2.2.2.0', '255.255.255.0', rif2_nhop)
            sai_thrift_remove_nhop(self.client, [rif1_nhop])
            sai_thrift_remove_neighbor(self.client, SAI_IP_ADDR_FAMILY_IPV4,
                                       rif1, '11.11.11.1', '00:22:22:22:22:22')
            sai_thrift_remove_nhop(self.client, [rif2_nhop])
            sai_thrift_remove_neighbor(self.client, SAI_IP_ADDR_FAMILY_IPV4,
                                       rif2, '2.2.2.2', '00:44:44:44:44:44')
            sai_thrift_remove_route(self.client, ovrf, SAI_IP_ADDR_FAMILY_IPV4,
                                    '10.10.10.0', '255.255.255.0', tunnel_nhop)
            sai_thrift_remove_nhop(self.client, [tunnel_nhop])
            self.client.sai_thrift_remove_tunnel_term(tunnel_term_id)
            self.client.sai_thrift_remove_tunnel(tunnel_id)
            self.client.sai_thrift_remove_tunnel_map_entry(encap_tun_map_entry)
            self.client.sai_thrift_remove_tunnel_map_entry(decap_tun_map_entry)
            self.client.sai_thrift_remove_tunnel_map(encap_tun_map)
            self.client.sai_thrift_remove_tunnel_map(decap_tun_map)
            self.client.sai_thrift_remove_router_interface(orif)
            self.client.sai_thrift_remove_router_interface(urif)
            self.client.sai_thrift_remove_router_interface(rif1)
            self.client.sai_thrift_remove_router_interface(rif2)
            self.client.sai_thrift_remove_virtual_router(ovrf)
            self.client.sai_thrift_remove_virtual_router(uvrf)


@pktpy_skip  # TODO bf-pktpy
@group('tunnel')
class L3VxlanDmacGlobalEncapDecap(sai_base_test.ThriftInterfaceDataPlane):
    '''
    This performs ip-in-vxlan tunnel encap/decap
    '''

    def runTest(self):
        try:
            print
            switch_init(self.client)
            port1 = port_list[0]
            port2 = port_list[1]
            tunnel_type = SAI_TUNNEL_TYPE_VXLAN
            term_type = SAI_TUNNEL_TERM_TABLE_ENTRY_TYPE_P2P
            remote_vtep_ip = '2.2.2.2'
            my_vtep_ip = '2.2.2.3'
            encap_map_type = SAI_TUNNEL_MAP_TYPE_VIRTUAL_ROUTER_ID_TO_VNI
            decap_map_type = SAI_TUNNEL_MAP_TYPE_VNI_TO_VIRTUAL_ROUTER_ID
            vni = 10000

            attr_value = sai_thrift_attribute_value_t(mac=vxlan_router_mac)
            attr = sai_thrift_attribute_t(id=SAI_SWITCH_ATTR_VXLAN_DEFAULT_ROUTER_MAC, value=attr_value)
            self.client.sai_thrift_set_switch_attribute(attr)

            v4 = 1
            v6 = 1
            ovrf = sai_thrift_create_virtual_router(self.client, v4, v6)
            uvrf = sai_thrift_create_virtual_router(self.client, v4, v6)

            #overlay rif
            rif1 = sai_thrift_create_router_interface(self.client, ovrf, SAI_ROUTER_INTERFACE_TYPE_PORT,
                                                      port1, 0, v4, v6, 0)
            #underlay rif
            rif2 = sai_thrift_create_router_interface(self.client, uvrf, SAI_ROUTER_INTERFACE_TYPE_PORT,
                                                      port2, 0, v4, v6, 0)

            #tunnel creation
            orif = sai_thrift_create_router_interface(self.client, ovrf, SAI_ROUTER_INTERFACE_TYPE_LOOPBACK,
                                                  0, 0, v4, v6, router_mac)
            urif = sai_thrift_create_router_interface(self.client, uvrf, SAI_ROUTER_INTERFACE_TYPE_LOOPBACK,
                                                  0, 0, v4, v6, router_mac)

            i_tun_map = sai_thrift_create_tunnel_map(self.client, encap_map_type)
            i_tun_map_entry = sai_thrift_create_tunnel_map_entry(
                self.client, encap_map_type, i_tun_map, 0, 0, ovrf, vni)
            e_tun_map = sai_thrift_create_tunnel_map(self.client, decap_map_type)
            e_tun_map_entry = sai_thrift_create_tunnel_map_entry(
                self.client, decap_map_type, e_tun_map, 0, 0, ovrf, vni)

            tunnel_id = sai_thrift_create_tunnel(
                self.client,
                tunnel_type,
                SAI_IP_ADDR_FAMILY_IPV4,
                my_vtep_ip,
                urif,
                orif,
                imap=i_tun_map,
                emap=e_tun_map)
            tunnel_term_id = sai_thrift_create_tunnel_term_table(
                self.client, tunnel_type, SAI_IP_ADDR_FAMILY_IPV4, uvrf, my_vtep_ip, remote_vtep_ip, tunnel_id, term_type)

            # static route to tunnel
            tunnel_nhop = sai_thrift_create_nhop(
                self.client, SAI_IP_ADDR_FAMILY_IPV4, remote_vtep_ip, tunnel_id, True, vni)
            sai_thrift_create_route(self.client, ovrf, SAI_IP_ADDR_FAMILY_IPV4,
                                    '10.10.10.0', '255.255.255.0', tunnel_nhop)

            # static route to underlay rif
            rif2_nhop = sai_thrift_create_nhop(
                self.client, SAI_IP_ADDR_FAMILY_IPV4, '2.2.2.2', rif2)
            sai_thrift_create_neighbor(self.client, SAI_IP_ADDR_FAMILY_IPV4,
                                       rif2, '2.2.2.2', '00:44:44:44:44:44')
            sai_thrift_create_route(self.client, uvrf, SAI_IP_ADDR_FAMILY_IPV4,
                                    '2.2.2.0', '255.255.255.0', rif2_nhop)

            # static route to overlay rif
            rif1_nhop = sai_thrift_create_nhop(
                self.client, SAI_IP_ADDR_FAMILY_IPV4, '11.11.11.1', rif1)
            sai_thrift_create_neighbor(self.client, SAI_IP_ADDR_FAMILY_IPV4,
                                       rif1, '11.11.11.1', '00:22:22:22:22:22')
            sai_thrift_create_route(self.client, ovrf, SAI_IP_ADDR_FAMILY_IPV4,
                                    '11.11.11.1', '255.255.255.0', rif1_nhop)

            print "Verifying 4in4 (L3 ip in vxlan encap)"
            pkt = simple_tcp_packet(
                eth_dst=router_mac,
                eth_src='00:22:22:22:22:22',
                ip_dst='10.10.10.1',
                ip_src='11.11.11.1',
                ip_id=105,
                ip_ttl=64)
            pkt2 = simple_tcp_packet(
                eth_dst=vxlan_router_mac,
                eth_src=router_mac,
                ip_dst='10.10.10.1',
                ip_src='11.11.11.1',
                ip_id=105,
                ip_ttl=63)
            vxlan_pkt = Mask(simple_vxlan_packet(
                eth_src=router_mac,
                eth_dst='00:44:44:44:44:44',
                ip_id=0,
                ip_src='2.2.2.3',
                ip_dst='2.2.2.2',
                ip_ttl=64,
                ip_flags=0x2,
                with_udp_chksum=False,
                vxlan_vni=vni,
                inner_frame=pkt2))
            vxlan_pkt.set_do_not_care_scapy(UDP, 'sport')
            send_packet(self,switch_ports[0], str(pkt))
            verify_packet(self, vxlan_pkt, switch_ports[1])

            print "Verifying 4in4 (L3 ip in vxlan decap)"
            pkt1 = simple_tcp_packet(
                eth_dst=router_mac,
                eth_src='00:55:55:55:55:55',
                ip_dst='11.11.11.1',
                ip_src='10.10.10.1',
                ip_id=108,
                ip_ttl=64)
            vxlan_pkt = simple_vxlan_packet(
                eth_dst=router_mac,
                eth_src='00:44:44:44:44:44',
                ip_id=0,
                ip_src='2.2.2.2',
                ip_dst='2.2.2.3',
                ip_ttl=64,
                ip_flags=0x2,
                udp_sport=11638,
                vxlan_vni=vni,
                with_udp_chksum=False,
                inner_frame=pkt1)
            pkt2 = simple_tcp_packet(
                eth_src=router_mac,
                eth_dst='00:22:22:22:22:22',
                ip_dst='11.11.11.1',
                ip_src='10.10.10.1',
                ip_id=108,
                ip_ttl=63)
            send_packet(self,switch_ports[1], str(vxlan_pkt))
            verify_packet(self, pkt2, switch_ports[0])

        finally:
            #cleanup
            sai_thrift_remove_route(self.client, ovrf, SAI_IP_ADDR_FAMILY_IPV4,
                                    '11.11.11.1', '255.255.255.0', rif1_nhop)
            sai_thrift_remove_nhop(self.client, [rif1_nhop])
            sai_thrift_remove_neighbor(self.client, SAI_IP_ADDR_FAMILY_IPV4,
                                       rif1, '11.11.11.1', '00:22:22:22:22:22')
            sai_thrift_remove_route(self.client, uvrf, SAI_IP_ADDR_FAMILY_IPV4,
                                    '2.2.2.0', '255.255.255.0', rif2_nhop)
            sai_thrift_remove_nhop(self.client, [rif2_nhop])
            sai_thrift_remove_neighbor(self.client, SAI_IP_ADDR_FAMILY_IPV4,
                                       rif2, '2.2.2.2', '00:44:44:44:44:44')
            sai_thrift_remove_route(self.client, ovrf, SAI_IP_ADDR_FAMILY_IPV4,
                                    '10.10.10.0', '255.255.255.0', tunnel_nhop)
            sai_thrift_remove_nhop(self.client, [tunnel_nhop])
            self.client.sai_thrift_remove_tunnel_term(tunnel_term_id)
            self.client.sai_thrift_remove_tunnel(tunnel_id)
            self.client.sai_thrift_remove_tunnel_map_entry(e_tun_map_entry)
            self.client.sai_thrift_remove_tunnel_map_entry(i_tun_map_entry)
            self.client.sai_thrift_remove_tunnel_map(e_tun_map)
            self.client.sai_thrift_remove_tunnel_map(i_tun_map)
            self.client.sai_thrift_remove_router_interface(orif)
            self.client.sai_thrift_remove_router_interface(urif)
            self.client.sai_thrift_remove_router_interface(rif1)
            self.client.sai_thrift_remove_router_interface(rif2)
            self.client.sai_thrift_remove_virtual_router(ovrf)
            self.client.sai_thrift_remove_virtual_router(uvrf)


@pktpy_skip  # TODO bf-pktpy
@group('tunnel')
class TunnelVniEncapDecap(sai_base_test.ThriftInterfaceDataPlane):
    '''
    This performs ip-in-vxlan tunnel encap/decap with no mapper
    '''

    def runTest(self):
        try:
            print
            switch_init(self.client)
            port1 = port_list[0]
            port2 = port_list[1]
            tunnel_type = SAI_TUNNEL_TYPE_VXLAN
            term_type = SAI_TUNNEL_TERM_TABLE_ENTRY_TYPE_P2P
            remote_vtep_ip = '2.2.2.2'
            my_vtep_ip = '2.2.2.3'
            encap_map_type = SAI_TUNNEL_MAP_TYPE_VIRTUAL_ROUTER_ID_TO_VNI
            decap_map_type = SAI_TUNNEL_MAP_TYPE_VNI_TO_VIRTUAL_ROUTER_ID
            vni = 10000

            v4 = 1
            v6 = 1
            ovrf = sai_thrift_create_virtual_router(self.client, v4, v6)
            uvrf = sai_thrift_create_virtual_router(self.client, v4, v6)

            #overlay rif
            rif1 = sai_thrift_create_router_interface(self.client, ovrf, SAI_ROUTER_INTERFACE_TYPE_PORT,
                                                      port1, 0, v4, v6, 0)
            #underlay rif
            rif2 = sai_thrift_create_router_interface(self.client, uvrf, SAI_ROUTER_INTERFACE_TYPE_PORT,
                                                      port2, 0, v4, v6, 0)

            #tunnel creation
            orif = sai_thrift_create_router_interface(self.client, ovrf, SAI_ROUTER_INTERFACE_TYPE_LOOPBACK,
                                                  0, 0, v4, v6, router_mac)
            urif = sai_thrift_create_router_interface(self.client, uvrf, SAI_ROUTER_INTERFACE_TYPE_LOOPBACK,
                                                  0, 0, v4, v6, router_mac)

            encap_tun_map = sai_thrift_create_tunnel_map(self.client, encap_map_type)
            encap_tun_map_entry = sai_thrift_create_tunnel_map_entry(
                self.client, encap_map_type, encap_tun_map, 0, 0, ovrf, vni)
            decap_tun_map = sai_thrift_create_tunnel_map(self.client, decap_map_type)
            decap_tun_map_entry = sai_thrift_create_tunnel_map_entry(
                self.client, decap_map_type, decap_tun_map, 0, 0, ovrf, vni)

            tunnel_id = sai_thrift_create_tunnel(
                self.client, tunnel_type, SAI_IP_ADDR_FAMILY_IPV4, my_vtep_ip,
                urif, orif, imap=encap_tun_map, emap=decap_tun_map)
            tunnel_term_id = sai_thrift_create_tunnel_term_table(
                self.client, tunnel_type, SAI_IP_ADDR_FAMILY_IPV4, uvrf, my_vtep_ip, remote_vtep_ip, tunnel_id, term_type)

            # static route to tunnel
            tunnel_nhop = sai_thrift_create_nhop(
                self.client, SAI_IP_ADDR_FAMILY_IPV4, remote_vtep_ip, tunnel_id, True, vni, '00:55:55:55:55:55')
            sai_thrift_create_route(self.client, ovrf, SAI_IP_ADDR_FAMILY_IPV4,
                                    '10.10.10.0', '255.255.255.0', tunnel_nhop)

            # static route to underlay rif
            rif2_nhop = sai_thrift_create_nhop(
                self.client, SAI_IP_ADDR_FAMILY_IPV4, '2.2.2.2', rif2)
            sai_thrift_create_neighbor(self.client, SAI_IP_ADDR_FAMILY_IPV4,
                                       rif2, '2.2.2.2', '00:44:44:44:44:44')
            sai_thrift_create_route(self.client, uvrf, SAI_IP_ADDR_FAMILY_IPV4,
                                    '2.2.2.0', '255.255.255.0', rif2_nhop)

            # static route to overlay rif
            rif1_nhop = sai_thrift_create_nhop(
                self.client, SAI_IP_ADDR_FAMILY_IPV4, '11.11.11.1', rif1)
            sai_thrift_create_neighbor(self.client, SAI_IP_ADDR_FAMILY_IPV4,
                                       rif1, '11.11.11.1', '00:22:22:22:22:22')
            sai_thrift_create_route(self.client, ovrf, SAI_IP_ADDR_FAMILY_IPV4,
                                    '11.11.11.1', '255.255.255.0', rif1_nhop)

            print "Verifying 4in4 (ip in vxlan encap no mapper)"
            pkt = simple_tcp_packet(
                eth_dst=router_mac,
                eth_src='00:22:22:22:22:22',
                ip_dst='10.10.10.1',
                ip_src='11.11.11.1',
                ip_id=105,
                ip_ttl=64)
            pkt2 = simple_tcp_packet(
                eth_dst='00:55:55:55:55:55',
                eth_src=router_mac,
                ip_dst='10.10.10.1',
                ip_src='11.11.11.1',
                ip_id=105,
                ip_ttl=63)
            vxlan_pkt = Mask(simple_vxlan_packet(
                eth_src=router_mac,
                eth_dst='00:44:44:44:44:44',
                ip_id=0,
                ip_src='2.2.2.3',
                ip_dst='2.2.2.2',
                ip_ttl=64,
                ip_flags=0x2,
                with_udp_chksum=False,
                vxlan_vni=vni,
                inner_frame=pkt2))
            vxlan_pkt.set_do_not_care_scapy(UDP, 'sport')
            send_packet(self,switch_ports[0], str(pkt))
            verify_packet(self, vxlan_pkt, switch_ports[1])

            print "Verifying 4in4 (ip in vxlan decap)"
            pkt1 = simple_tcp_packet(
                eth_dst=router_mac,
                eth_src='00:55:55:55:55:55',
                ip_dst='11.11.11.1',
                ip_src='10.10.10.1',
                ip_id=108,
                ip_ttl=64)
            vxlan_pkt = simple_vxlan_packet(
                eth_dst=router_mac,
                eth_src='00:44:44:44:44:44',
                ip_id=0,
                ip_src='2.2.2.2',
                ip_dst='2.2.2.3',
                ip_ttl=64,
                ip_flags=0x2,
                udp_sport=11638,
                vxlan_vni=vni,
                with_udp_chksum=False,
                inner_frame=pkt1)
            pkt2 = simple_tcp_packet(
                eth_src=router_mac,
                eth_dst='00:22:22:22:22:22',
                ip_dst='11.11.11.1',
                ip_src='10.10.10.1',
                ip_id=108,
                ip_ttl=63)
            send_packet(self,switch_ports[1], str(vxlan_pkt))
            verify_packet(self, pkt2, switch_ports[0])

        finally:
            #cleanup
            sai_thrift_remove_route(self.client, ovrf, SAI_IP_ADDR_FAMILY_IPV4,
                                    '11.11.11.1', '255.255.255.0', rif1_nhop)
            sai_thrift_remove_nhop(self.client, [rif1_nhop])
            sai_thrift_remove_neighbor(self.client, SAI_IP_ADDR_FAMILY_IPV4,
                                       rif1, '11.11.11.1', '00:22:22:22:22:22')
            sai_thrift_remove_route(self.client, uvrf, SAI_IP_ADDR_FAMILY_IPV4,
                                    '2.2.2.0', '255.255.255.0', rif2_nhop)
            sai_thrift_remove_nhop(self.client, [rif2_nhop])
            sai_thrift_remove_neighbor(self.client, SAI_IP_ADDR_FAMILY_IPV4,
                                       rif2, '2.2.2.2', '00:44:44:44:44:44')
            sai_thrift_remove_route(self.client, ovrf, SAI_IP_ADDR_FAMILY_IPV4,
                                    '10.10.10.0', '255.255.255.0', tunnel_nhop)
            sai_thrift_remove_nhop(self.client, [tunnel_nhop])
            self.client.sai_thrift_remove_tunnel_term(tunnel_term_id)
            self.client.sai_thrift_remove_tunnel(tunnel_id)
            self.client.sai_thrift_remove_tunnel_map_entry(decap_tun_map_entry)
            self.client.sai_thrift_remove_tunnel_map(decap_tun_map)
            self.client.sai_thrift_remove_tunnel_map_entry(encap_tun_map_entry)
            self.client.sai_thrift_remove_tunnel_map(encap_tun_map)
            self.client.sai_thrift_remove_router_interface(orif)
            self.client.sai_thrift_remove_router_interface(urif)
            self.client.sai_thrift_remove_router_interface(rif1)
            self.client.sai_thrift_remove_router_interface(rif2)
            self.client.sai_thrift_remove_virtual_router(ovrf)
            self.client.sai_thrift_remove_virtual_router(uvrf)


@pktpy_skip  # TODO bf-pktpy
@group('tunnel')
class L3VxlanP2MPTunnel(sai_base_test.ThriftInterfaceDataPlane):
    '''
    --------------------  --------------------  --------------------
    | VM: 100.100.1.1  |  | VM: 100.100.2.1  |  | VM: 100.101.2.1  |
    | VNI: 2000        |  | VNI: 2001        |  | VNI: 2005        |
    | Host: 10.10.10.1 |  | Host: 10.10.10.2 |  | Host: 10.10.10.3 |
    --------------------  --------------------  --------------------
                      \           |            /
      ------------------------------------------------------------
      |                        | p5 |                            |
      |                        ------                            |
      |                          |                               |
      |                -----------------------                   |
      |                | Tunnel: 10.10.10.10 |                   |
      |                -----------------------                   |
      |     |             |                |                     |
      |   ----           ----            ----           ----     |
      |   rif1           rif2            rif3           rif4     |
      ------------------------------------------------------------
            |             |               |               |
  ---------------  ---------------  ---------------  ---------------
  C1: 100.100.3.1  C2: 100.100.4.1  C3: 100.102.1.1  C4: 100.101.1.1
  ---------------  ---------------  ---------------  ---------------
    '''
    # Routing in-and-out of Vxlan tunnels
    def setUp(self):
        super(self.__class__, self).setUp()
        print
        switch_init(self.client)
        self.C1_port = port_list[1]
        self.C2_port = port_list[2]
        self.C3_port = port_list[3]
        self.C4_port = port_list[4]
        self.port5 = port_list[5]

        # One endpoint in each Customer Subnet
        self.C_ip = [0]*5
        self.C_ip[1] = '100.100.3.1'
        self.C_ip[2] = '100.100.4.1'
        self.C_ip[3] = '100.102.1.1'
        self.C_ip[4] = '100.101.1.1'

        self.C_mac = [0]*5
        self.C_mac[1] = '00:00:00:00:00:01'
        self.C_mac[2] = '00:00:00:00:00:02'
        self.C_mac[3] = '00:00:00:00:00:03'
        self.C_mac[4] = '00:00:00:00:00:04'

        # VMs and their hosts
        self.VM_ip = [0]*4
        self.VM_ip[1] = '100.100.1.1'
        self.VM_ip[2] = '100.100.2.1'
        self.VM_ip[3] = '100.101.2.1'

        self.host_ip = [0]*4
        self.host_ip[1] = '10.10.10.1'
        self.host_ip[2] = '10.10.10.2'
        self.host_ip[3] = '10.10.10.3'

        # Inner Destination MACs
        self.vxlan_default_router_mac='00:11:11:11:11:11'

        self.host_inner_dmac = [0]*4
        self.host_inner_dmac[1] = self.vxlan_default_router_mac
        self.host_inner_dmac[2] = '00:12:34:56:78:9a'
        self.host_inner_dmac[3] = self.vxlan_default_router_mac


        # Create Default VRF ( also used as underlay vrf )
        v4_enabled = 1
        self.uvrf = sai_thrift_create_virtual_router(self.client, v4_enabled, 0)

        # Create Underlay loopback RIF ( required for tunnel object creation )
        self.urif_lb = sai_thrift_create_router_interface(self.client, self.uvrf, SAI_ROUTER_INTERFACE_TYPE_LOOPBACK,
                                                  0, 0, v4_enabled, 0, router_mac)

        #
        # Create Overlay VRFs
        #
        self.ovrf = [0]*4
        self.ovrf[1] = sai_thrift_create_virtual_router(self.client, v4_enabled, 0)
        self.ovrf[2] = sai_thrift_create_virtual_router(self.client, v4_enabled, 0)
        self.ovrf[3] = sai_thrift_create_virtual_router(self.client, v4_enabled, 0)

        self.ovrf_vni = [0]*4
        self.ovrf_vni[1] = 2000
        self.ovrf_vni[2] = 2001
        self.ovrf_vni[3] = 2005

        #
        # Setup underlay default route, the nexthop is 1.1.1.1
        #
        self.underlay_neighbor_mac = '00:55:55:55:55:55'
        self.underlay_nhop_ip = '1.1.1.1'
        self.underlay_route_addr = '0.0.0.0'
        self.underlay_route_mask = '0.0.0.0'
        self.underlay_rif = sai_thrift_create_router_interface(self.client, self.uvrf, SAI_ROUTER_INTERFACE_TYPE_PORT, self.port5, 0, 1, 0 , 0)
        self.underlay_nhop = sai_thrift_create_nhop(self.client, SAI_IP_ADDR_FAMILY_IPV4, self.underlay_nhop_ip, self.underlay_rif)
        self.underlay_neighbor = sai_thrift_create_neighbor(self.client, SAI_IP_ADDR_FAMILY_IPV4,
                                                       self.underlay_rif, self.underlay_nhop_ip, self.underlay_neighbor_mac)
        self.underlay_default_route = sai_thrift_create_route(self.client, self.uvrf, SAI_IP_ADDR_FAMILY_IPV4,
                                                         self.underlay_route_addr, self.underlay_route_mask, self.underlay_nhop)

        #
        # Setup overlay routes
        #

        # create port-based router interface for C1 
        self.rif1 = sai_thrift_create_router_interface(self.client, self.ovrf[1], SAI_ROUTER_INTERFACE_TYPE_PORT, self.C1_port, 0, 1, 1, '')
        sai_thrift_create_neighbor(self.client, SAI_IP_ADDR_FAMILY_IPV4, self.rif1, self.C_ip[1], self.C_mac[1])

        # create port-based router interface for C2 
        self.rif2 = sai_thrift_create_router_interface(self.client, self.ovrf[1], SAI_ROUTER_INTERFACE_TYPE_PORT, self.C2_port, 0, 1, 1, '')
        sai_thrift_create_neighbor(self.client, SAI_IP_ADDR_FAMILY_IPV4, self.rif2, self.C_ip[2], self.C_mac[2])

        # create port-based router interface for C3
        self.rif3 = sai_thrift_create_router_interface(self.client, self.ovrf[2], SAI_ROUTER_INTERFACE_TYPE_PORT, self.C3_port, 0, 1, 1, '')
        sai_thrift_create_neighbor(self.client, SAI_IP_ADDR_FAMILY_IPV4, self.rif3, self.C_ip[3], self.C_mac[3])

        # create port-based router interface for C4 
        self.rif4 = sai_thrift_create_router_interface(self.client, self.ovrf[3], SAI_ROUTER_INTERFACE_TYPE_PORT, self.C4_port, 0, 1, 1, '')
        sai_thrift_create_neighbor(self.client, SAI_IP_ADDR_FAMILY_IPV4, self.rif4, self.C_ip[4], self.C_mac[4])

        #
        # Create Tunnel
        #

        # Create Encap/decap mappers
        self.encap_tunnel_map = sai_thrift_create_tunnel_map(self.client, SAI_TUNNEL_MAP_TYPE_VIRTUAL_ROUTER_ID_TO_VNI)
        self.decap_tunnel_map = sai_thrift_create_tunnel_map(self.client, SAI_TUNNEL_MAP_TYPE_VNI_TO_VIRTUAL_ROUTER_ID)

        # Create Tunnel object
        self.my_lb_ip_addr = '10.10.10.10'
        self.my_lb_ip_mask = '255.255.255.255'
        self.tunnel_id = sai_thrift_create_tunnel(self.client, SAI_TUNNEL_TYPE_VXLAN, SAI_IP_ADDR_FAMILY_IPV4, self.my_lb_ip_addr, self.urif_lb, 0, self.encap_tunnel_map, self.decap_tunnel_map)

        # Create Tunnel Map entries for C1, C2
        self.C1_encap_tunnel_map_entry = sai_thrift_create_tunnel_map_entry(self.client, SAI_TUNNEL_MAP_TYPE_VIRTUAL_ROUTER_ID_TO_VNI, self.encap_tunnel_map, 0, 0, self.ovrf[1], self.ovrf_vni[1])
        self.C1_decap_tunnel_map_entry = sai_thrift_create_tunnel_map_entry(self.client, SAI_TUNNEL_MAP_TYPE_VNI_TO_VIRTUAL_ROUTER_ID, self.decap_tunnel_map, 0, 0, self.ovrf[1], self.ovrf_vni[1])

        # Create Tunnel Map entries for C3
        self.C3_encap_tunnel_map_entry = sai_thrift_create_tunnel_map_entry(self.client, SAI_TUNNEL_MAP_TYPE_VIRTUAL_ROUTER_ID_TO_VNI, self.encap_tunnel_map, 0, 0, self.ovrf[2], self.ovrf_vni[2])
        self.C3_decap_tunnel_map_entry = sai_thrift_create_tunnel_map_entry(self.client, SAI_TUNNEL_MAP_TYPE_VNI_TO_VIRTUAL_ROUTER_ID, self.decap_tunnel_map, 0, 0, self.ovrf[2], self.ovrf_vni[2])

        # Create Tunnel Map entries for C4
        self.C4_encap_tunnel_map_entry = sai_thrift_create_tunnel_map_entry(self.client, SAI_TUNNEL_MAP_TYPE_VIRTUAL_ROUTER_ID_TO_VNI, self.encap_tunnel_map, 0, 0, self.ovrf[3], self.ovrf_vni[3])
        self.C4_decap_tunnel_map_entry = sai_thrift_create_tunnel_map_entry(self.client, SAI_TUNNEL_MAP_TYPE_VNI_TO_VIRTUAL_ROUTER_ID, self.decap_tunnel_map, 0, 0, self.ovrf[3], self.ovrf_vni[3])

        # Create tunnel decap for VM to customer server 
        self.tunnel_term_id = sai_thrift_create_tunnel_term_table(self.client, SAI_TUNNEL_TYPE_VXLAN, SAI_IP_ADDR_FAMILY_IPV4, self.uvrf,
                                                       self.my_lb_ip_addr, None, self.tunnel_id, SAI_TUNNEL_TERM_TABLE_ENTRY_TYPE_P2MP)

        # create tunnel nexthop for VM1, VM2 and VM3 */
        self.tunnel_nexthop_id1 = sai_thrift_create_nhop(self.client, SAI_IP_ADDR_FAMILY_IPV4, self.host_ip[1], self.tunnel_id, True, 0, self.host_inner_dmac[1])
        self.tunnel_nexthop_id2 = sai_thrift_create_nhop(self.client, SAI_IP_ADDR_FAMILY_IPV4, self.host_ip[2], self.tunnel_id, True, self.ovrf_vni[2], self.host_inner_dmac[2])
        self.tunnel_nexthop_id3 = sai_thrift_create_nhop(self.client, SAI_IP_ADDR_FAMILY_IPV4, self.host_ip[3], self.tunnel_id, True, 0, self.host_inner_dmac[3])

        # Create routes for vrid 1 ingress */
        VM1_vnet1_route = sai_thrift_create_route(self.client, self.ovrf[1], SAI_IP_ADDR_FAMILY_IPV4, '100.100.1.1', '255.255.255.255', self.tunnel_nexthop_id1)
        VM2_vnet1_route = sai_thrift_create_route(self.client, self.ovrf[1], SAI_IP_ADDR_FAMILY_IPV4, '100.100.2.1', '255.255.255.255', self.tunnel_nexthop_id2)
        C1_vnet1_route  = sai_thrift_create_route(self.client, self.ovrf[1], SAI_IP_ADDR_FAMILY_IPV4, '100.100.3.0', '255.255.255.0', self.rif1)
        C2_vnet1_route  = sai_thrift_create_route(self.client, self.ovrf[1], SAI_IP_ADDR_FAMILY_IPV4, '100.100.4.0', '255.255.255.0', self.rif2)
        C3_vnet1_route  = sai_thrift_create_route(self.client, self.ovrf[1], SAI_IP_ADDR_FAMILY_IPV4, '100.102.1.0', '255.255.255.0', self.rif3)

        # Create routes for vrid 1 egress

        # create routes for vrid 2 ingress 
        VM1_vnet2_route  = sai_thrift_create_route(self.client, self.ovrf[2], SAI_IP_ADDR_FAMILY_IPV4, '100.100.1.1', '255.255.255.255', self.tunnel_nexthop_id1)
        VM2_vnet2_route  = sai_thrift_create_route(self.client, self.ovrf[2], SAI_IP_ADDR_FAMILY_IPV4, '100.100.2.1', '255.255.255.255', self.tunnel_nexthop_id2)
        C1_vnet2_route   = sai_thrift_create_route(self.client, self.ovrf[2], SAI_IP_ADDR_FAMILY_IPV4, '100.100.3.0', '255.255.255.0', self.rif1)
        C2_vnet2_route   = sai_thrift_create_route(self.client, self.ovrf[2], SAI_IP_ADDR_FAMILY_IPV4, '100.100.4.0', '255.255.255.0', self.rif2)
        C3_vnet2_route   = sai_thrift_create_route(self.client, self.ovrf[2], SAI_IP_ADDR_FAMILY_IPV4, '100.102.1.0', '255.255.255.0', self.rif3)

        # Create routes fro vrid 3 ingress 
        VM3_vnet3_route = sai_thrift_create_route(self.client, self.ovrf[3], SAI_IP_ADDR_FAMILY_IPV4, '100.101.2.1', '255.255.255.255', self.tunnel_nexthop_id3)
        C4_vnet3_route  = sai_thrift_create_route(self.client, self.ovrf[3], SAI_IP_ADDR_FAMILY_IPV4, '100.101.1.0', '255.255.255.0', self.rif4)


    def runTest(self):
        try:
            print "Tunnel Decap:"
            # VM1/2 -> C1/C2/C3
            for c_id in range(1,4):
                for vm_id in range(1,3):
                    if c_id==3:
                        pkt_vni = self.ovrf_vni[2]
                    else:
                        pkt_vni = self.ovrf_vni[1]
                    print "sending packet from VM%d to C%d" % (vm_id, c_id)
                    pkt1 = simple_tcp_packet(
                        eth_dst=router_mac,
                        eth_src=self.vxlan_default_router_mac,
                        ip_dst=self.C_ip[c_id],
                        ip_src=self.VM_ip[vm_id],
                        ip_id=108,
                        ip_ttl=64)
                    vxlan_pkt = simple_vxlan_packet(
                        eth_dst=router_mac,
                        eth_src=self.underlay_neighbor_mac,
                        ip_id=0,
                        ip_src=self.host_ip[vm_id],
                        ip_dst=self.my_lb_ip_addr,
                        ip_ttl=64,
                        ip_flags=0x2,
                        udp_sport=11638,
                        vxlan_vni=pkt_vni,
                        with_udp_chksum=False,
                        inner_frame=pkt1)
                    pkt2 = simple_tcp_packet(
                        eth_src=router_mac,
                        eth_dst=self.C_mac[c_id],
                        ip_dst=self.C_ip[c_id],
                        ip_src=self.VM_ip[vm_id],
                        ip_id=108,
                        ip_ttl=63)
                    send_packet(self, switch_ports[5], str(vxlan_pkt))
                    verify_packet(self, pkt2, switch_ports[c_id])

            # VM3 -> C4
            vm_id = 3
            c_id = 4
            pkt_vni = self.ovrf_vni[3]
            print "sending packet from VM%d to C%d" % (vm_id, c_id)
            pkt1 = simple_tcp_packet(
                eth_dst=router_mac,
                eth_src=self.vxlan_default_router_mac,
                ip_dst=self.C_ip[c_id],
                ip_src=self.VM_ip[vm_id],
                ip_id=108,
                ip_ttl=64)
            vxlan_pkt = simple_vxlan_packet(
                eth_dst=router_mac,
                eth_src=self.underlay_neighbor_mac,
                ip_id=0,
                ip_src=self.host_ip[vm_id],
                ip_dst=self.my_lb_ip_addr,
                ip_ttl=64,
                ip_flags=0x2,
                udp_sport=11638,
                vxlan_vni=pkt_vni,
                with_udp_chksum=False,
                inner_frame=pkt1)
            pkt2 = simple_tcp_packet(
                eth_src=router_mac,
                eth_dst=self.C_mac[c_id],
                ip_dst=self.C_ip[c_id],
                ip_src=self.VM_ip[vm_id],
                ip_id=108,
                ip_ttl=63)
            send_packet(self, switch_ports[5], str(vxlan_pkt))
            verify_packet(self, pkt2, switch_ports[c_id])

            print "Tunnel Encap:"
            # C1/C2/C3 -> VM1/2
            for c_id in range(1,4):
                for vm_id in range(1,3):
                    if vm_id==2:
                        pkt_vni = self.ovrf_vni[2]
                    elif c_id==3:
                        pkt_vni = self.ovrf_vni[2]
                    else:
                        pkt_vni = self.ovrf_vni[1]
                    print "sending packet from C%d to VM%d" % (c_id, vm_id)
                    pkt = simple_tcp_packet(
                              eth_dst=router_mac,
                              eth_src=self.C_mac[c_id],
                              ip_dst=self.VM_ip[vm_id],
                              ip_src=self.C_ip[c_id],
                              ip_id=105,
                              ip_ttl=64)
                    pkt2 = simple_tcp_packet(
                        eth_dst=self.host_inner_dmac[vm_id],
                        eth_src=router_mac,
                        ip_dst=self.VM_ip[vm_id],
                        ip_src=self.C_ip[c_id],
                        ip_id=105,
                        ip_ttl=63)
                    udp_sport = entropy_hash(pkt)
                    vxlan_pkt = Mask(simple_vxlan_packet(
                        eth_src=router_mac,
                        eth_dst=self.underlay_neighbor_mac,
                        ip_id=0,
                        ip_src=self.my_lb_ip_addr,
                        ip_dst=self.host_ip[vm_id],
                        ip_ttl=64,
                        ip_flags=0x2,
                        udp_sport=udp_sport,
                        with_udp_chksum=False,
                        vxlan_vni=pkt_vni,
                        inner_frame=pkt2))
                    vxlan_pkt.set_do_not_care_scapy(UDP, 'sport')
                    send_packet(self, switch_ports[c_id], str(pkt))
                    verify_packet(self, vxlan_pkt, switch_ports[5])

            # C4 -> VM3
            vm_id = 3
            c_id = 4
            pkt_vni = self.ovrf_vni[3]
            print "sending packet from C%d to VM%d" % (c_id, vm_id)
            pkt = simple_tcp_packet(
                      eth_dst=router_mac,
                      eth_src=self.C_mac[c_id],
                      ip_dst=self.VM_ip[vm_id],
                      ip_src=self.C_ip[c_id],
                      ip_id=105,
                      ip_ttl=64)
            pkt2 = simple_tcp_packet(
                eth_dst=self.host_inner_dmac[vm_id],
                eth_src=router_mac,
                ip_dst=self.VM_ip[vm_id],
                ip_src=self.C_ip[c_id],
                ip_id=105,
                ip_ttl=63)
            udp_sport = entropy_hash(pkt)
            vxlan_pkt = Mask(simple_vxlan_packet(
                eth_src=router_mac,
                eth_dst=self.underlay_neighbor_mac,
                ip_id=0,
                ip_src=self.my_lb_ip_addr,
                ip_dst=self.host_ip[vm_id],
                ip_ttl=64,
                ip_flags=0x2,
                udp_sport=udp_sport,
                with_udp_chksum=False,
                vxlan_vni=pkt_vni,
                inner_frame=pkt2))
            vxlan_pkt.set_do_not_care_scapy(UDP, 'sport')
            send_packet(self, switch_ports[c_id], str(pkt))
            verify_packet(self, vxlan_pkt, switch_ports[5])

        finally:
            print

    def tearDown(self):
        #cleanup
        sai_thrift_remove_route(self.client, self.ovrf[1], SAI_IP_ADDR_FAMILY_IPV4, '100.100.1.1', '255.255.255.255', self.tunnel_nexthop_id1)
        sai_thrift_remove_route(self.client, self.ovrf[1], SAI_IP_ADDR_FAMILY_IPV4, '100.100.2.1', '255.255.255.255', self.tunnel_nexthop_id2)
        sai_thrift_remove_route(self.client, self.ovrf[1], SAI_IP_ADDR_FAMILY_IPV4, '100.100.3.0', '255.255.255.0',   self.rif1)
        sai_thrift_remove_route(self.client, self.ovrf[1], SAI_IP_ADDR_FAMILY_IPV4, '100.100.4.0', '255.255.255.0',   self.rif2)
        sai_thrift_remove_route(self.client, self.ovrf[1], SAI_IP_ADDR_FAMILY_IPV4, '100.102.1.0', '255.255.255.0',   self.rif3)
        sai_thrift_remove_route(self.client, self.ovrf[2], SAI_IP_ADDR_FAMILY_IPV4, '100.100.1.1', '255.255.255.255', self.tunnel_nexthop_id1)
        sai_thrift_remove_route(self.client, self.ovrf[2], SAI_IP_ADDR_FAMILY_IPV4, '100.100.2.1', '255.255.255.255', self.tunnel_nexthop_id2)
        sai_thrift_remove_route(self.client, self.ovrf[2], SAI_IP_ADDR_FAMILY_IPV4, '100.100.3.0', '255.255.255.0',   self.rif1)
        sai_thrift_remove_route(self.client, self.ovrf[2], SAI_IP_ADDR_FAMILY_IPV4, '100.100.4.0', '255.255.255.0',   self.rif2)
        sai_thrift_remove_route(self.client, self.ovrf[2], SAI_IP_ADDR_FAMILY_IPV4, '100.102.1.0', '255.255.255.0',   self.rif3)
        sai_thrift_remove_route(self.client, self.ovrf[3], SAI_IP_ADDR_FAMILY_IPV4, '100.101.2.1', '255.255.255.255', self.tunnel_nexthop_id1)
        sai_thrift_remove_route(self.client, self.ovrf[3], SAI_IP_ADDR_FAMILY_IPV4, '100.101.1.0', '255.255.255.0',   self.rif4)

        sai_thrift_remove_nhop(self.client, [self.tunnel_nexthop_id1, self.tunnel_nexthop_id2, self.tunnel_nexthop_id3])

        self.client.sai_thrift_remove_tunnel_term(self.tunnel_term_id)
        self.client.sai_thrift_remove_tunnel_map_entry(self.C1_encap_tunnel_map_entry)
        self.client.sai_thrift_remove_tunnel_map_entry(self.C3_encap_tunnel_map_entry)
        self.client.sai_thrift_remove_tunnel_map_entry(self.C4_encap_tunnel_map_entry)
        self.client.sai_thrift_remove_tunnel_map_entry(self.C1_decap_tunnel_map_entry)
        self.client.sai_thrift_remove_tunnel_map_entry(self.C3_decap_tunnel_map_entry)
        self.client.sai_thrift_remove_tunnel_map_entry(self.C4_decap_tunnel_map_entry)
        self.client.sai_thrift_remove_tunnel(self.tunnel_id)
        self.client.sai_thrift_remove_tunnel_map(self.encap_tunnel_map)
        self.client.sai_thrift_remove_tunnel_map(self.decap_tunnel_map)

        sai_thrift_remove_neighbor(self.client, SAI_IP_ADDR_FAMILY_IPV4, self.rif1, "100.100.3.1", '00:00:00:00:00:01')
        sai_thrift_remove_neighbor(self.client, SAI_IP_ADDR_FAMILY_IPV4, self.rif2, "100.100.4.1", "00:00:00:00:00:02")
        sai_thrift_remove_neighbor(self.client, SAI_IP_ADDR_FAMILY_IPV4, self.rif3, "100.102.1.1", "00:00:00:00:00:03")
        sai_thrift_remove_neighbor(self.client, SAI_IP_ADDR_FAMILY_IPV4, self.rif4, "100.101.1.1", "00:00:00:00:00:04")
        self.client.sai_thrift_remove_router_interface(self.rif1)
        self.client.sai_thrift_remove_router_interface(self.rif2)
        self.client.sai_thrift_remove_router_interface(self.rif3)
        self.client.sai_thrift_remove_router_interface(self.rif4)

        sai_thrift_remove_route(self.client, self.uvrf, SAI_IP_ADDR_FAMILY_IPV4, '0.0.0.0', '0.0.0.0', self.rif4)
        sai_thrift_remove_nhop(self.client, [self.underlay_nhop])
        sai_thrift_remove_neighbor(self.client, SAI_IP_ADDR_FAMILY_IPV4, self.underlay_rif, "1.1.1.1", '00:55:55:55:55:55')
        self.client.sai_thrift_remove_router_interface(self.underlay_rif)
        self.client.sai_thrift_remove_router_interface(self.urif_lb)
        self.client.sai_thrift_remove_virtual_router(self.ovrf[1])
        self.client.sai_thrift_remove_virtual_router(self.ovrf[2])
        self.client.sai_thrift_remove_virtual_router(self.ovrf[3])
        self.client.sai_thrift_remove_virtual_router(self.uvrf)
        super(self.__class__, self).tearDown()


@pktpy_skip  # TODO bf-pktpy
@group('tunnel')
class L3VxlanP2MPIPv6Tunnel(sai_base_test.ThriftInterfaceDataPlane):
    # Routing in-and-out of V6 Vxlan tunnels
    def setUp(self):
        super(self.__class__, self).setUp()
        print
        switch_init(self.client)
        self.C1_port = port_list[1]
        self.C2_port = port_list[2]
        self.C3_port = port_list[3]
        self.C4_port = port_list[4]
        self.port5 = port_list[5]

        # One endpoint in each Customer Subnet
        self.C_ip = [0]*5
        self.C_ip[1] = '100.100.3.1'
        self.C_ip[2] = '100.100.4.1'
        self.C_ip[3] = '100.102.1.1'
        self.C_ip[4] = '100.101.1.1'

        self.C_mac = [0]*5
        self.C_mac[1] = '00:00:00:00:00:01'
        self.C_mac[2] = '00:00:00:00:00:02'
        self.C_mac[3] = '00:00:00:00:00:03'
        self.C_mac[4] = '00:00:00:00:00:04'

        # VMs and their hosts
        self.VM_ip = [0]*4
        self.VM_ip[1] = '100.100.1.1'
        self.VM_ip[2] = '100.100.2.1'
        self.VM_ip[3] = '100.101.2.1'

        self.host_ip = [0]*4
        self.host_ip[1] = '1000::1'
        self.host_ip[2] = '1000::2'
        self.host_ip[3] = '1000::3'

        # Inner Destination MACs
        self.vxlan_default_router_mac='00:11:11:11:11:11'

        self.host_inner_dmac = [0]*4
        self.host_inner_dmac[1] = self.vxlan_default_router_mac
        self.host_inner_dmac[2] = '00:12:34:56:78:9a'
        self.host_inner_dmac[3] = self.vxlan_default_router_mac

        # Create Default VRF ( also used as underlay vrf )
        v4_enabled = 1
        v6_enabled = 1
        self.uvrf = sai_thrift_create_virtual_router(self.client, v4_enabled, v6_enabled)

        # Create Underlay loopback RIF ( required for tunnel object creation )
        self.urif_lb = sai_thrift_create_router_interface(self.client, self.uvrf, SAI_ROUTER_INTERFACE_TYPE_LOOPBACK,
                                                  0, 0, v4_enabled, v6_enabled, router_mac)

        #
        # Create Overlay VRFs
        #
        self.ovrf = [0]*4
        self.ovrf[1] = sai_thrift_create_virtual_router(self.client, v4_enabled, v6_enabled)
        self.ovrf[2] = sai_thrift_create_virtual_router(self.client, v4_enabled, v6_enabled)
        self.ovrf[3] = sai_thrift_create_virtual_router(self.client, v4_enabled, v6_enabled)

        self.ovrf_vni = [0]*4
        self.ovrf_vni[1] = 2000
        self.ovrf_vni[2] = 2001
        self.ovrf_vni[3] = 2005

        #
        # Setup underlay default route, the nexthop is 5678::11
        #
        self.underlay_neighbor_mac = '00:55:55:55:55:55'
        self.underlay_nhop_ip = '5678::11'
        self.underlay_route_addr = '0::0'
        self.underlay_route_mask = '0::0'
        self.underlay_rif = sai_thrift_create_router_interface(self.client, self.uvrf, SAI_ROUTER_INTERFACE_TYPE_PORT, self.port5, 0, 1, 0 , 0)
        self.underlay_nhop = sai_thrift_create_nhop(self.client, SAI_IP_ADDR_FAMILY_IPV6, self.underlay_nhop_ip, self.underlay_rif)
        self.underlay_neighbor = sai_thrift_create_neighbor(self.client, SAI_IP_ADDR_FAMILY_IPV6,
                                                       self.underlay_rif, self.underlay_nhop_ip, self.underlay_neighbor_mac)
        self.underlay_default_route = sai_thrift_create_route(self.client, self.uvrf, SAI_IP_ADDR_FAMILY_IPV6,
                                                         self.underlay_route_addr, self.underlay_route_mask, self.underlay_nhop)

        #
        # Setup overlay routes
        #

        # create port-based router interface for C1 
        self.rif1 = sai_thrift_create_router_interface(self.client, self.ovrf[1], SAI_ROUTER_INTERFACE_TYPE_PORT, self.C1_port, 0, 1, 1, '')
        sai_thrift_create_neighbor(self.client, SAI_IP_ADDR_FAMILY_IPV4, self.rif1, self.C_ip[1], self.C_mac[1])

        # create port-based router interface for C2 
        self.rif2 = sai_thrift_create_router_interface(self.client, self.ovrf[1], SAI_ROUTER_INTERFACE_TYPE_PORT, self.C2_port, 0, 1, 1, '')
        sai_thrift_create_neighbor(self.client, SAI_IP_ADDR_FAMILY_IPV4, self.rif2, self.C_ip[2], self.C_mac[2])

        # create port-based router interface for C3
        self.rif3 = sai_thrift_create_router_interface(self.client, self.ovrf[2], SAI_ROUTER_INTERFACE_TYPE_PORT, self.C3_port, 0, 1, 1, '')
        sai_thrift_create_neighbor(self.client, SAI_IP_ADDR_FAMILY_IPV4, self.rif3, self.C_ip[3], self.C_mac[3])

        # create port-based router interface for C4 
        self.rif4 = sai_thrift_create_router_interface(self.client, self.ovrf[3], SAI_ROUTER_INTERFACE_TYPE_PORT, self.C4_port, 0, 1, 1, '')
        sai_thrift_create_neighbor(self.client, SAI_IP_ADDR_FAMILY_IPV4, self.rif4, self.C_ip[4], self.C_mac[4])

        #
        # Create Tunnel
        #

        # Create Encap/decap mappers
        self.encap_tunnel_map = sai_thrift_create_tunnel_map(self.client, SAI_TUNNEL_MAP_TYPE_VIRTUAL_ROUTER_ID_TO_VNI)
        self.decap_tunnel_map = sai_thrift_create_tunnel_map(self.client, SAI_TUNNEL_MAP_TYPE_VNI_TO_VIRTUAL_ROUTER_ID)

        # Create Tunnel object
        self.my_lb_ip_addr = '1000:1000:1000:1000:1000:1000:1000:1000'
        self.my_lb_ip_mask = '255:255:255:255:255:255:255:255'
        self.tunnel_id = sai_thrift_create_tunnel(self.client, SAI_TUNNEL_TYPE_VXLAN, SAI_IP_ADDR_FAMILY_IPV6, self.my_lb_ip_addr, self.urif_lb, 0, self.encap_tunnel_map, self.decap_tunnel_map)

        # Create Tunnel Map entries for C1, C2
        self.C1_encap_tunnel_map_entry = sai_thrift_create_tunnel_map_entry(self.client, SAI_TUNNEL_MAP_TYPE_VIRTUAL_ROUTER_ID_TO_VNI, self.encap_tunnel_map, 0, 0, self.ovrf[1], self.ovrf_vni[1])
        self.C1_decap_tunnel_map_entry = sai_thrift_create_tunnel_map_entry(self.client, SAI_TUNNEL_MAP_TYPE_VNI_TO_VIRTUAL_ROUTER_ID, self.decap_tunnel_map, 0, 0, self.ovrf[1], self.ovrf_vni[1])

        # Create Tunnel Map entries for C3
        self.C3_encap_tunnel_map_entry = sai_thrift_create_tunnel_map_entry(self.client, SAI_TUNNEL_MAP_TYPE_VIRTUAL_ROUTER_ID_TO_VNI, self.encap_tunnel_map, 0, 0, self.ovrf[2], self.ovrf_vni[2])
        self.C3_decap_tunnel_map_entry = sai_thrift_create_tunnel_map_entry(self.client, SAI_TUNNEL_MAP_TYPE_VNI_TO_VIRTUAL_ROUTER_ID, self.decap_tunnel_map, 0, 0, self.ovrf[2], self.ovrf_vni[2])

        # Create Tunnel Map entries for C4
        self.C4_encap_tunnel_map_entry = sai_thrift_create_tunnel_map_entry(self.client, SAI_TUNNEL_MAP_TYPE_VIRTUAL_ROUTER_ID_TO_VNI, self.encap_tunnel_map, 0, 0, self.ovrf[3], self.ovrf_vni[3])
        self.C4_decap_tunnel_map_entry = sai_thrift_create_tunnel_map_entry(self.client, SAI_TUNNEL_MAP_TYPE_VNI_TO_VIRTUAL_ROUTER_ID, self.decap_tunnel_map, 0, 0, self.ovrf[3], self.ovrf_vni[3])

        # Create tunnel decap for VM to customer server 
        self.tunnel_term_id = sai_thrift_create_tunnel_term_table(self.client, SAI_TUNNEL_TYPE_VXLAN, SAI_IP_ADDR_FAMILY_IPV6, self.uvrf,
                                                       self.my_lb_ip_addr, None, self.tunnel_id, SAI_TUNNEL_TERM_TABLE_ENTRY_TYPE_P2MP)

        # create tunnel nexthop for VM1, VM2 and VM3 */
        self.tunnel_nexthop_id1 = sai_thrift_create_nhop(self.client, SAI_IP_ADDR_FAMILY_IPV6, self.host_ip[1], self.tunnel_id, True, 0, self.host_inner_dmac[1])
        self.tunnel_nexthop_id2 = sai_thrift_create_nhop(self.client, SAI_IP_ADDR_FAMILY_IPV6, self.host_ip[2], self.tunnel_id, True, self.ovrf_vni[2], self.host_inner_dmac[2])
        self.tunnel_nexthop_id3 = sai_thrift_create_nhop(self.client, SAI_IP_ADDR_FAMILY_IPV6, self.host_ip[3], self.tunnel_id, True, 0, self.host_inner_dmac[3])

        # Create routes for vrid 1 ingress */
        VM1_vnet1_route = sai_thrift_create_route(self.client, self.ovrf[1], SAI_IP_ADDR_FAMILY_IPV4, '100.100.1.1', '255.255.255.255', self.tunnel_nexthop_id1)
        VM2_vnet1_route = sai_thrift_create_route(self.client, self.ovrf[1], SAI_IP_ADDR_FAMILY_IPV4, '100.100.2.1', '255.255.255.255', self.tunnel_nexthop_id2)
        C1_vnet1_route  = sai_thrift_create_route(self.client, self.ovrf[1], SAI_IP_ADDR_FAMILY_IPV4, '100.100.3.0', '255.255.255.0', self.rif1)
        C2_vnet1_route  = sai_thrift_create_route(self.client, self.ovrf[1], SAI_IP_ADDR_FAMILY_IPV4, '100.100.4.0', '255.255.255.0', self.rif2)
        C3_vnet1_route  = sai_thrift_create_route(self.client, self.ovrf[1], SAI_IP_ADDR_FAMILY_IPV4, '100.102.1.0', '255.255.255.0', self.rif3)

        # Create routes for vrid 1 egress

        # create routes for vrid 2 ingress 
        VM1_vnet2_route  = sai_thrift_create_route(self.client, self.ovrf[2], SAI_IP_ADDR_FAMILY_IPV4, '100.100.1.1', '255.255.255.255', self.tunnel_nexthop_id1)
        VM2_vnet2_route  = sai_thrift_create_route(self.client, self.ovrf[2], SAI_IP_ADDR_FAMILY_IPV4, '100.100.2.1', '255.255.255.255', self.tunnel_nexthop_id2)
        C1_vnet2_route   = sai_thrift_create_route(self.client, self.ovrf[2], SAI_IP_ADDR_FAMILY_IPV4, '100.100.3.0', '255.255.255.0', self.rif1)
        C2_vnet2_route   = sai_thrift_create_route(self.client, self.ovrf[2], SAI_IP_ADDR_FAMILY_IPV4, '100.100.4.0', '255.255.255.0', self.rif2)
        C3_vnet2_route   = sai_thrift_create_route(self.client, self.ovrf[2], SAI_IP_ADDR_FAMILY_IPV4, '100.102.1.0', '255.255.255.0', self.rif3)

        # Create routes fro vrid 3 ingress 
        VM3_vnet3_route = sai_thrift_create_route(self.client, self.ovrf[3], SAI_IP_ADDR_FAMILY_IPV4, '100.101.2.1', '255.255.255.255', self.tunnel_nexthop_id3)
        C4_vnet3_route  = sai_thrift_create_route(self.client, self.ovrf[3], SAI_IP_ADDR_FAMILY_IPV4, '100.101.1.0', '255.255.255.0', self.rif4)

    def runTest(self):
        try:
            print "Tunnel Decap:"
            # VM1/2 -> C1/C2/C3
            for c_id in range(1,4):
                for vm_id in range(1,3):
                    if c_id==3:
                        pkt_vni = self.ovrf_vni[2]
                    else:
                        pkt_vni = self.ovrf_vni[1]
                    print "sending packet from VM%d to C%d" % (vm_id, c_id)
                    pkt1 = simple_tcp_packet(
                        eth_dst=router_mac,
                        eth_src=self.vxlan_default_router_mac,
                        ip_dst=self.C_ip[c_id],
                        ip_src=self.VM_ip[vm_id],
                        ip_id=108,
                        ip_ttl=64)
                    udp_sport = entropy_hash(pkt1)
                    vxlan_pkt = simple_vxlanv6_packet(
                        eth_dst=router_mac,
                        eth_src=self.underlay_neighbor_mac,
                        ipv6_src=self.host_ip[vm_id],
                        ipv6_dst=self.my_lb_ip_addr,
                        ipv6_hlim=64,
                        udp_sport=udp_sport,
                        vxlan_vni=pkt_vni,
                        with_udp_chksum=False,
                        inner_frame=pkt1)
                    pkt2 = simple_tcp_packet(
                        eth_src=router_mac,
                        eth_dst=self.C_mac[c_id],
                        ip_dst=self.C_ip[c_id],
                        ip_src=self.VM_ip[vm_id],
                        ip_id=108,
                        ip_ttl=63)
                    send_packet(self, switch_ports[5], str(vxlan_pkt))
                    verify_packet(self, pkt2, switch_ports[c_id])

            # VM3 -> C4
            vm_id = 3
            c_id = 4
            pkt_vni = self.ovrf_vni[3]
            print "sending packet from VM%d to C%d" % (vm_id, c_id)
            pkt1 = simple_tcp_packet(
                eth_dst=router_mac,
                eth_src=self.vxlan_default_router_mac,
                ip_dst=self.C_ip[c_id],
                ip_src=self.VM_ip[vm_id],
                ip_id=108,
                ip_ttl=64)
            udp_sport = entropy_hash(pkt1)
            vxlan_pkt = simple_vxlanv6_packet(
                eth_dst=router_mac,
                eth_src=self.underlay_neighbor_mac,
                ipv6_src=self.host_ip[vm_id],
                ipv6_dst=self.my_lb_ip_addr,
                ipv6_hlim=64,
                udp_sport=udp_sport,
                vxlan_vni=pkt_vni,
                with_udp_chksum=False,
                inner_frame=pkt1)
            pkt2 = simple_tcp_packet(
                eth_src=router_mac,
                eth_dst=self.C_mac[c_id],
                ip_dst=self.C_ip[c_id],
                ip_src=self.VM_ip[vm_id],
                ip_id=108,
                ip_ttl=63)
            send_packet(self, switch_ports[5], str(vxlan_pkt))
            verify_packet(self, pkt2, switch_ports[c_id])

            print "Tunnel Encap:"
            # C1/C2/C3 -> VM1/2
            for c_id in range(1,4):
                for vm_id in range(1,3):
                    if vm_id==2:
                        pkt_vni = self.ovrf_vni[2]
                    elif c_id==3:
                        pkt_vni = self.ovrf_vni[2]
                    else:
                        pkt_vni = self.ovrf_vni[1]
                    print "sending packet from C%d to VM%d" % (c_id, vm_id)
                    pkt = simple_tcp_packet(
                              eth_dst=router_mac,
                              eth_src=self.C_mac[c_id],
                              ip_dst=self.VM_ip[vm_id],
                              ip_src=self.C_ip[c_id],
                              ip_id=105,
                              ip_ttl=64)
                    pkt2 = simple_tcp_packet(
                        eth_dst=self.host_inner_dmac[vm_id],
                        eth_src=router_mac,
                        ip_dst=self.VM_ip[vm_id],
                        ip_src=self.C_ip[c_id],
                        ip_id=105,
                        ip_ttl=63)
                    udp_sport = entropy_hash(pkt2)
                    vxlan_pkt = Mask(simple_vxlanv6_packet(
                        eth_src=router_mac,
                        eth_dst=self.underlay_neighbor_mac,
                        ipv6_src=self.my_lb_ip_addr,
                        ipv6_dst=self.host_ip[vm_id],
                        ipv6_hlim=64,
                        udp_sport=udp_sport,
                        with_udp_chksum=False,
                        vxlan_vni=pkt_vni,
                        inner_frame=pkt2))
                    vxlan_pkt.set_do_not_care_scapy(UDP, 'sport')
                    send_packet(self, switch_ports[c_id], str(pkt))
                    verify_packet(self, vxlan_pkt, switch_ports[5])

            # C4 -> VM3
            vm_id = 3
            c_id = 4
            pkt_vni = self.ovrf_vni[3]
            print "sending packet from C%d to VM%d" % (c_id, vm_id)
            pkt = simple_tcp_packet(
                      eth_dst=router_mac,
                      eth_src=self.C_mac[c_id],
                      ip_dst=self.VM_ip[vm_id],
                      ip_src=self.C_ip[c_id],
                      ip_id=105,
                      ip_ttl=64)
            pkt2 = simple_tcp_packet(
                eth_dst=self.host_inner_dmac[vm_id],
                eth_src=router_mac,
                ip_dst=self.VM_ip[vm_id],
                ip_src=self.C_ip[c_id],
                ip_id=105,
                ip_ttl=63)
            udp_sport = entropy_hash(pkt2)
            vxlan_pkt = Mask(simple_vxlanv6_packet(
                eth_src=router_mac,
                eth_dst=self.underlay_neighbor_mac,
                ipv6_src=self.my_lb_ip_addr,
                ipv6_dst=self.host_ip[vm_id],
                ipv6_hlim=64,
                udp_sport=udp_sport,
                with_udp_chksum=False,
                vxlan_vni=pkt_vni,
                inner_frame=pkt2))
            vxlan_pkt.set_do_not_care_scapy(UDP, 'sport')
            send_packet(self, switch_ports[c_id], str(pkt))
            verify_packet(self, vxlan_pkt, switch_ports[5])

        finally:
            print

    def tearDown(self):
        #cleanup
        sai_thrift_remove_route(self.client, self.ovrf[1], SAI_IP_ADDR_FAMILY_IPV4, '100.100.1.1', '255.255.255.255', self.tunnel_nexthop_id1)
        sai_thrift_remove_route(self.client, self.ovrf[1], SAI_IP_ADDR_FAMILY_IPV4, '100.100.2.1', '255.255.255.255', self.tunnel_nexthop_id2)
        sai_thrift_remove_route(self.client, self.ovrf[1], SAI_IP_ADDR_FAMILY_IPV4, '100.100.3.0', '255.255.255.0',   self.rif1)
        sai_thrift_remove_route(self.client, self.ovrf[1], SAI_IP_ADDR_FAMILY_IPV4, '100.100.4.0', '255.255.255.0',   self.rif2)
        sai_thrift_remove_route(self.client, self.ovrf[1], SAI_IP_ADDR_FAMILY_IPV4, '100.102.1.0', '255.255.255.0',   self.rif3)
        sai_thrift_remove_route(self.client, self.ovrf[2], SAI_IP_ADDR_FAMILY_IPV4, '100.100.1.1', '255.255.255.255', self.tunnel_nexthop_id1)
        sai_thrift_remove_route(self.client, self.ovrf[2], SAI_IP_ADDR_FAMILY_IPV4, '100.100.2.1', '255.255.255.255', self.tunnel_nexthop_id2)
        sai_thrift_remove_route(self.client, self.ovrf[2], SAI_IP_ADDR_FAMILY_IPV4, '100.100.3.0', '255.255.255.0',   self.rif1)
        sai_thrift_remove_route(self.client, self.ovrf[2], SAI_IP_ADDR_FAMILY_IPV4, '100.100.4.0', '255.255.255.0',   self.rif2)
        sai_thrift_remove_route(self.client, self.ovrf[2], SAI_IP_ADDR_FAMILY_IPV4, '100.102.1.0', '255.255.255.0',   self.rif3)
        sai_thrift_remove_route(self.client, self.ovrf[3], SAI_IP_ADDR_FAMILY_IPV4, '100.101.2.1', '255.255.255.255', self.tunnel_nexthop_id1)
        sai_thrift_remove_route(self.client, self.ovrf[3], SAI_IP_ADDR_FAMILY_IPV4, '100.101.1.0', '255.255.255.0',   self.rif4)

        sai_thrift_remove_nhop(self.client, [self.tunnel_nexthop_id1, self.tunnel_nexthop_id2, self.tunnel_nexthop_id3])

        self.client.sai_thrift_remove_tunnel_term(self.tunnel_term_id)
        self.client.sai_thrift_remove_tunnel_map_entry(self.C1_encap_tunnel_map_entry)
        self.client.sai_thrift_remove_tunnel_map_entry(self.C3_encap_tunnel_map_entry)
        self.client.sai_thrift_remove_tunnel_map_entry(self.C4_encap_tunnel_map_entry)
        self.client.sai_thrift_remove_tunnel_map_entry(self.C1_decap_tunnel_map_entry)
        self.client.sai_thrift_remove_tunnel_map_entry(self.C3_decap_tunnel_map_entry)
        self.client.sai_thrift_remove_tunnel_map_entry(self.C4_decap_tunnel_map_entry)
        self.client.sai_thrift_remove_tunnel(self.tunnel_id)
        self.client.sai_thrift_remove_tunnel_map(self.encap_tunnel_map)
        self.client.sai_thrift_remove_tunnel_map(self.decap_tunnel_map)

        sai_thrift_remove_neighbor(self.client, SAI_IP_ADDR_FAMILY_IPV4, self.rif1, "100.100.3.1", '00:00:00:00:00:01')
        sai_thrift_remove_neighbor(self.client, SAI_IP_ADDR_FAMILY_IPV4, self.rif2, "100.100.4.1", "00:00:00:00:00:02")
        sai_thrift_remove_neighbor(self.client, SAI_IP_ADDR_FAMILY_IPV4, self.rif3, "100.102.1.1", "00:00:00:00:00:03")
        sai_thrift_remove_neighbor(self.client, SAI_IP_ADDR_FAMILY_IPV4, self.rif4, "100.101.1.1", "00:00:00:00:00:04")
        self.client.sai_thrift_remove_router_interface(self.rif1)
        self.client.sai_thrift_remove_router_interface(self.rif2)
        self.client.sai_thrift_remove_router_interface(self.rif3)
        self.client.sai_thrift_remove_router_interface(self.rif4)

        sai_thrift_remove_route(self.client, self.uvrf, SAI_IP_ADDR_FAMILY_IPV6, '0::0', '0::0', self.rif4)
        sai_thrift_remove_nhop(self.client, [self.underlay_nhop])
        sai_thrift_remove_neighbor(self.client, SAI_IP_ADDR_FAMILY_IPV6, self.underlay_rif, "5678::11", '00:55:55:55:55:55')
        self.client.sai_thrift_remove_router_interface(self.underlay_rif)
        self.client.sai_thrift_remove_router_interface(self.urif_lb)
        self.client.sai_thrift_remove_virtual_router(self.ovrf[1])
        self.client.sai_thrift_remove_virtual_router(self.ovrf[2])
        self.client.sai_thrift_remove_virtual_router(self.ovrf[3])
        self.client.sai_thrift_remove_virtual_router(self.uvrf)
        super(self.__class__, self).tearDown()


@pktpy_skip  # TODO bf-pktpy
@disabled
@group('tunnel')
class QinQVxlanEncapDecap(sai_base_test.ThriftInterfaceDataPlane):
    '''
    This performs qinq rif <> vxlan tunnel encap/decap 
    '''

    def runTest(self):
        try:
            print
            switch_init(self.client)
            port1 = port_list[0]
            port2 = port_list[1]
            tunnel_type = SAI_TUNNEL_TYPE_VXLAN
            term_type = SAI_TUNNEL_TERM_TABLE_ENTRY_TYPE_P2P
            remote_vtep_ip = '2.2.2.2'
            my_vtep_ip = '2.2.2.3'
            decap_map_type = SAI_TUNNEL_MAP_TYPE_VNI_TO_VIRTUAL_ROUTER_ID
            encap_map_type = SAI_TUNNEL_MAP_TYPE_VIRTUAL_ROUTER_ID_TO_VNI
            vni = 10000
            outer_vlan = 0x123
            inner_vlan = 0x456

            v4 = 1
            v6 = 1
            ovrf = sai_thrift_create_virtual_router(self.client, v4, v6)
            uvrf = sai_thrift_create_virtual_router(self.client, v4, v6)

            #overlay rif
            rif1 = sai_thrift_create_router_interface(self.client, ovrf, SAI_ROUTER_INTERFACE_TYPE_QINQ_PORT,
                                                      port1, 0, v4, v6, router_mac, outer_vlan, inner_vlan)
            #underlay rif
            rif2 = sai_thrift_create_router_interface(self.client, uvrf, SAI_ROUTER_INTERFACE_TYPE_PORT,
                                                      port2, 0, v4, v6, router_mac)

            #tunnel creation
            orif = sai_thrift_create_router_interface(self.client, ovrf, SAI_ROUTER_INTERFACE_TYPE_LOOPBACK,
                                                  0, 0, v4, v6, router_mac)
            urif = sai_thrift_create_router_interface(self.client, uvrf, SAI_ROUTER_INTERFACE_TYPE_LOOPBACK,
                                                  0, 0, v4, v6, router_mac)

            decap_tun_map = sai_thrift_create_tunnel_map(self.client, decap_map_type)
            decap_tun_map_entry = sai_thrift_create_tunnel_map_entry(
                self.client, decap_map_type, decap_tun_map, 0, 0, ovrf, vni)
            encap_tun_map = sai_thrift_create_tunnel_map(self.client, encap_map_type)
            encap_tun_map_entry = sai_thrift_create_tunnel_map_entry(
                self.client, encap_map_type, encap_tun_map, 0, 0, ovrf, vni)

            tunnel_id = sai_thrift_create_tunnel(
                self.client,
                tunnel_type,
                SAI_IP_ADDR_FAMILY_IPV4,
                my_vtep_ip,
                urif,
                orif,
                imap=decap_tun_map,
                emap=encap_tun_map)
            tunnel_term_id = sai_thrift_create_tunnel_term_table(
                self.client, tunnel_type, SAI_IP_ADDR_FAMILY_IPV4, uvrf, my_vtep_ip, remote_vtep_ip, tunnel_id, term_type)

            # static route to tunnel
            tunnel_nhop = sai_thrift_create_nhop(self.client, SAI_IP_ADDR_FAMILY_IPV4, remote_vtep_ip, tunnel_id,
                True, vni, '00:55:55:55:55:55')
            sai_thrift_create_route(self.client, ovrf, SAI_IP_ADDR_FAMILY_IPV4,
                                    '10.10.10.0', '255.255.255.0', tunnel_nhop)

            # static route to underlay rif
            rif2_nhop = sai_thrift_create_nhop(
                self.client, SAI_IP_ADDR_FAMILY_IPV4, remote_vtep_ip, rif2)
            sai_thrift_create_neighbor(self.client, SAI_IP_ADDR_FAMILY_IPV4,
                                       rif2, remote_vtep_ip, '00:44:44:44:44:44')
            sai_thrift_create_route(self.client, uvrf, SAI_IP_ADDR_FAMILY_IPV4,
                                    '2.2.2.0', '255.255.255.0', rif2_nhop)

            # static route to overlay rif
            rif1_nhop = sai_thrift_create_nhop(
                self.client, SAI_IP_ADDR_FAMILY_IPV4, '11.11.11.1', rif1)
            sai_thrift_create_neighbor(self.client, SAI_IP_ADDR_FAMILY_IPV4,
                                       rif1, '11.11.11.1', '00:22:22:22:22:22')
            sai_thrift_create_route(self.client, ovrf, SAI_IP_ADDR_FAMILY_IPV4,
                                    '11.11.11.1', '255.255.255.0', rif1_nhop)

            print "Verifying 4in4 (L3 ip in vxlan encap)"
            pkt = simple_qinq_tcp_packet(
                eth_dst=router_mac,
                eth_src='00:22:22:22:22:22',
                dl_vlan_outer=outer_vlan,
                vlan_vid=inner_vlan,
                ip_dst='10.10.10.1',
                ip_src='11.11.11.1',
                ip_ttl=64)
            pkt2 = simple_tcp_packet(
                pktlen=92,
                eth_dst='00:55:55:55:55:55',
                eth_src=router_mac,
                ip_dst='10.10.10.1',
                ip_src='11.11.11.1',
                ip_ttl=63)
            vxlan_pkt = Mask(simple_vxlan_packet(
                eth_src=router_mac,
                eth_dst='00:44:44:44:44:44',
                ip_id=0,
                ip_src='2.2.2.3',
                ip_dst='2.2.2.2',
                ip_ttl=64,
                ip_flags=0x2,
                with_udp_chksum=False,
                vxlan_vni=vni,
                inner_frame=pkt2))
            vxlan_pkt.set_do_not_care_scapy(UDP, 'sport')
            send_packet(self,switch_ports[0], str(pkt))
            verify_packet(self, vxlan_pkt, switch_ports[1])

            print "Verifying 4in4 (L3 ip in vxlan decap)"
            pkt1 = simple_tcp_packet(
                eth_dst=router_mac,
                eth_src='00:55:55:55:55:55',
                ip_dst='11.11.11.1',
                ip_src='10.10.10.1',
                ip_ttl=64)
            vxlan_pkt = simple_vxlan_packet(
                eth_dst=router_mac,
                eth_src='00:44:44:44:44:44',
                ip_id=0,
                ip_src='2.2.2.2',
                ip_dst='2.2.2.3',
                ip_ttl=64,
                ip_flags=0x2,
                udp_sport=11638,
                vxlan_vni=vni,
                with_udp_chksum=False,
                inner_frame=pkt1)
            pkt2 = simple_tcp_packet(
                pktlen=108,
                eth_src=router_mac,
                eth_dst='00:22:22:22:22:22',
                dl_vlan_outer=outer_vlan,
                vlan_vid=inner_vlan,
                ip_dst='11.11.11.1',
                ip_src='10.10.10.1',
                ip_ttl=63)
            send_packet(self,switch_ports[1], str(vxlan_pkt))
            verify_packet(self, pkt2, switch_ports[0])

        finally:
            #cleanup
            sai_thrift_remove_route(self.client, ovrf, SAI_IP_ADDR_FAMILY_IPV4,
                                    '11.11.11.1', '255.255.255.0', rif1)
            sai_thrift_remove_route(self.client, uvrf, SAI_IP_ADDR_FAMILY_IPV4,
                                    '2.2.2.0', '255.255.255.0', rif2_nhop)
            sai_thrift_remove_nhop(self.client, [rif1_nhop])
            sai_thrift_remove_neighbor(self.client, SAI_IP_ADDR_FAMILY_IPV4,
                                       rif1, '11.11.11.1', '00:22:22:22:22:22')
            sai_thrift_remove_nhop(self.client, [rif2_nhop])
            sai_thrift_remove_neighbor(self.client, SAI_IP_ADDR_FAMILY_IPV4,
                                       rif2, '2.2.2.2', '00:44:44:44:44:44')
            sai_thrift_remove_route(self.client, ovrf, SAI_IP_ADDR_FAMILY_IPV4,
                                    '10.10.10.0', '255.255.255.0', tunnel_nhop)
            sai_thrift_remove_nhop(self.client, [tunnel_nhop])
            self.client.sai_thrift_remove_tunnel_term(tunnel_term_id)
            self.client.sai_thrift_remove_tunnel(tunnel_id)
            self.client.sai_thrift_remove_tunnel_map_entry(encap_tun_map_entry)
            self.client.sai_thrift_remove_tunnel_map_entry(decap_tun_map_entry)
            self.client.sai_thrift_remove_tunnel_map(encap_tun_map)
            self.client.sai_thrift_remove_tunnel_map(decap_tun_map)
            self.client.sai_thrift_remove_router_interface(orif)
            self.client.sai_thrift_remove_router_interface(urif)
            self.client.sai_thrift_remove_router_interface(rif1)
            self.client.sai_thrift_remove_router_interface(rif2)
            self.client.sai_thrift_remove_virtual_router(ovrf)
            self.client.sai_thrift_remove_virtual_router(uvrf)


@pktpy_skip  # TODO bf-pktpy
@group('tunnel')
class L3VxlanP2MPSeparateVrfTunnel(sai_base_test.ThriftInterfaceDataPlane):
    # Routing in-and-out of Vxlan tunnels
    def setUp(self):
        super(self.__class__, self).setUp()
        print
        switch_init(self.client)
        self.C1_port = port_list[1]
        self.C2_port = port_list[2]
        self.C3_port = port_list[3]
        self.C4_port = port_list[4]
        self.port5 = port_list[5]

        # One endpoint in each Customer Subnet
        self.C_ip = [0]*5
        self.C_ip[1] = '100.100.3.1'
        self.C_ip[2] = '100.100.4.1'
        self.C_ip[3] = '100.102.1.1'
        self.C_ip[4] = '100.101.1.1'

        self.C_mac = [0]*5
        self.C_mac[1] = '00:00:00:00:00:01'
        self.C_mac[2] = '00:00:00:00:00:02'
        self.C_mac[3] = '00:00:00:00:00:03'
        self.C_mac[4] = '00:00:00:00:00:04'

        # VMs and their hosts
        self.VM_ip = [0]*4
        self.VM_ip[1] = '100.100.1.1'
        self.VM_ip[2] = '100.100.2.1'
        self.VM_ip[3] = '100.101.2.1'

        self.host_ip = [0]*4
        self.host_ip[1] = '10.10.10.1'
        self.host_ip[2] = '10.10.10.2'
        self.host_ip[3] = '10.10.10.3'

        # Inner Destination MACs
        self.vxlan_default_router_mac='00:11:11:11:11:11'

        self.host_inner_dmac = [0]*4
        self.host_inner_dmac[1] = self.vxlan_default_router_mac
        self.host_inner_dmac[2] = '00:12:34:56:78:9a'
        self.host_inner_dmac[3] = self.vxlan_default_router_mac

        attr_value = sai_thrift_attribute_value_t(mac=self.vxlan_default_router_mac)
        attr = sai_thrift_attribute_t(id=SAI_SWITCH_ATTR_VXLAN_DEFAULT_ROUTER_MAC, value=attr_value)
        self.client.sai_thrift_set_switch_attribute(attr)

        # Create Default VRF ( also used as underlay vrf )
        v4_enabled = 1
        v6_enabled = 1
        self.uvrf = sai_thrift_create_virtual_router(self.client, v4_enabled, v6_enabled)
        sai_thrift_remove_default_bridge_ports(self.client, [self.C1_port, self.C2_port, self.C3_port, self.C4_port, self.port5])

        # Create Underlay loopback RIF ( required for tunnel object creation )
        self.urif_lb = sai_thrift_create_router_interface(self.client, self.uvrf, SAI_ROUTER_INTERFACE_TYPE_LOOPBACK,
                                                  0, 0, v4_enabled, v6_enabled, router_mac)

        #
        # Create Overlay VRFs
        #
        self.ovrf_ingress = [0]*4
        self.ovrf_ingress[1] = sai_thrift_create_virtual_router(self.client, v4_enabled, v6_enabled)
        self.ovrf_ingress[2] = sai_thrift_create_virtual_router(self.client, v4_enabled, v6_enabled)
        self.ovrf_ingress[3] = sai_thrift_create_virtual_router(self.client, v4_enabled, v6_enabled)
        self.ovrf_egress = [0]*4
        self.ovrf_egress[1] = sai_thrift_create_virtual_router(self.client, v4_enabled, v6_enabled)
        self.ovrf_egress[2] = sai_thrift_create_virtual_router(self.client, v4_enabled, v6_enabled)
        self.ovrf_egress[3] = sai_thrift_create_virtual_router(self.client, v4_enabled, v6_enabled)

        self.ovrf_ingress_vni = [0]*4
        self.ovrf_ingress_vni[1] = 2000
        self.ovrf_ingress_vni[2] = 2001
        self.ovrf_ingress_vni[3] = 2005

        #
        # Setup underlay default route, the nexthop is 1.1.1.1
        #
        self.underlay_neighbor_mac = '00:55:55:55:55:55'
        self.underlay_nhop_ip = '1.1.1.1'
        self.underlay_route_addr = '0.0.0.0'
        self.underlay_route_mask = '0.0.0.0'
        self.underlay_rif = sai_thrift_create_router_interface(self.client, self.uvrf, SAI_ROUTER_INTERFACE_TYPE_PORT, self.port5, 0, 1, 0, 0)
        self.underlay_nhop = sai_thrift_create_nhop(self.client, SAI_IP_ADDR_FAMILY_IPV4, self.underlay_nhop_ip, self.underlay_rif)
        self.underlay_neighbor = sai_thrift_create_neighbor(self.client, SAI_IP_ADDR_FAMILY_IPV4,
                                                       self.underlay_rif, self.underlay_nhop_ip, self.underlay_neighbor_mac)
        self.underlay_default_route = sai_thrift_create_route(self.client, self.uvrf, SAI_IP_ADDR_FAMILY_IPV4,
                                                         self.underlay_route_addr, self.underlay_route_mask, self.underlay_nhop)

        #
        # Setup overlay routes
        #

        # create port-based router interface for C1
        self.rif1 = sai_thrift_create_router_interface(self.client, self.ovrf_ingress[1], SAI_ROUTER_INTERFACE_TYPE_PORT, self.C1_port, 0, 1, 1, '')
        sai_thrift_create_neighbor(self.client, SAI_IP_ADDR_FAMILY_IPV4, self.rif1, self.C_ip[1], self.C_mac[1])

        # create port-based router interface for C2
        self.rif2 = sai_thrift_create_router_interface(self.client, self.ovrf_ingress[1], SAI_ROUTER_INTERFACE_TYPE_PORT, self.C2_port, 0, 1, 1, '')
        sai_thrift_create_neighbor(self.client, SAI_IP_ADDR_FAMILY_IPV4, self.rif2, self.C_ip[2], self.C_mac[2])

        # create port-based router interface for C3
        self.rif3 = sai_thrift_create_router_interface(self.client, self.ovrf_ingress[2], SAI_ROUTER_INTERFACE_TYPE_PORT, self.C3_port, 0, 1, 1, '')
        sai_thrift_create_neighbor(self.client, SAI_IP_ADDR_FAMILY_IPV4, self.rif3, self.C_ip[3], self.C_mac[3])

        # create port-based router interface for C4
        self.rif4 = sai_thrift_create_router_interface(self.client, self.ovrf_ingress[3], SAI_ROUTER_INTERFACE_TYPE_PORT, self.C4_port, 0, 1, 1, '')
        # create neighbor entry for self.C_ip[4], self.C_mac[4] after route creation to verify route export

        #
        # Create Tunnel
        #

        # Create Encap/decap mappers
        self.encap_tunnel_map = sai_thrift_create_tunnel_map(self.client, SAI_TUNNEL_MAP_TYPE_VIRTUAL_ROUTER_ID_TO_VNI)
        self.decap_tunnel_map = sai_thrift_create_tunnel_map(self.client, SAI_TUNNEL_MAP_TYPE_VNI_TO_VIRTUAL_ROUTER_ID)

        # Create Tunnel object
        self.my_lb_ip_addr = '10.10.10.10'
        self.my_lb_ip_mask = '255.255.255.255'
        self.tunnel_id = sai_thrift_create_tunnel(self.client, SAI_TUNNEL_TYPE_VXLAN, SAI_IP_ADDR_FAMILY_IPV4, self.my_lb_ip_addr,
            self.urif_lb, 0, self.encap_tunnel_map, self.decap_tunnel_map)

        # Create Tunnel Map entries for C1, C2
        self.C1_encap_tunnel_map_entry = sai_thrift_create_tunnel_map_entry(self.client, SAI_TUNNEL_MAP_TYPE_VIRTUAL_ROUTER_ID_TO_VNI,
            self.encap_tunnel_map, 0, 0, self.ovrf_ingress[1], self.ovrf_ingress_vni[1])
        self.C1_decap_tunnel_map_entry = sai_thrift_create_tunnel_map_entry(self.client, SAI_TUNNEL_MAP_TYPE_VNI_TO_VIRTUAL_ROUTER_ID,
            self.decap_tunnel_map, 0, 0, self.ovrf_egress[1], self.ovrf_ingress_vni[1])

        # Create Tunnel Map entries for C3
        self.C3_encap_tunnel_map_entry = sai_thrift_create_tunnel_map_entry(self.client, SAI_TUNNEL_MAP_TYPE_VIRTUAL_ROUTER_ID_TO_VNI,
            self.encap_tunnel_map, 0, 0, self.ovrf_ingress[2], self.ovrf_ingress_vni[2])
        self.C3_decap_tunnel_map_entry = sai_thrift_create_tunnel_map_entry(self.client, SAI_TUNNEL_MAP_TYPE_VNI_TO_VIRTUAL_ROUTER_ID,
            self.decap_tunnel_map, 0, 0, self.ovrf_egress[2], self.ovrf_ingress_vni[2])

        # Create Tunnel Map entries for C4
        self.C4_encap_tunnel_map_entry = sai_thrift_create_tunnel_map_entry(self.client, SAI_TUNNEL_MAP_TYPE_VIRTUAL_ROUTER_ID_TO_VNI,
            self.encap_tunnel_map, 0, 0, self.ovrf_ingress[3], self.ovrf_ingress_vni[3])
        self.C4_decap_tunnel_map_entry = sai_thrift_create_tunnel_map_entry(self.client, SAI_TUNNEL_MAP_TYPE_VNI_TO_VIRTUAL_ROUTER_ID,
            self.decap_tunnel_map, 0, 0, self.ovrf_egress[3], self.ovrf_ingress_vni[3])

        # Create tunnel decap for VM to customer server
        self.tunnel_term_id = sai_thrift_create_tunnel_term_table(self.client, SAI_TUNNEL_TYPE_VXLAN, SAI_IP_ADDR_FAMILY_IPV4, self.uvrf,
                                                       self.my_lb_ip_addr, None, self.tunnel_id, SAI_TUNNEL_TERM_TABLE_ENTRY_TYPE_P2MP)


        # create tunnel nexthop for VM1, VM2 and VM3 */
        # global vxlan router mac will be used unless specific mac in tunnel nexthop
        # create nexthop with optional VNI
        self.tunnel_nexthop_id1 = sai_thrift_create_nhop(self.client, SAI_IP_ADDR_FAMILY_IPV4, self.host_ip[1], self.tunnel_id, True)
        self.tunnel_nexthop_id2 = sai_thrift_create_nhop(self.client, SAI_IP_ADDR_FAMILY_IPV4, self.host_ip[2], self.tunnel_id, True,
            self.ovrf_ingress_vni[2], self.host_inner_dmac[2])
        self.tunnel_nexthop_id3 = sai_thrift_create_nhop(self.client, SAI_IP_ADDR_FAMILY_IPV4, self.host_ip[3], self.tunnel_id, True)

        # Create routes for vrid 1 ingress */
        VM1_vnet1_ingress_route = sai_thrift_create_route(self.client, self.ovrf_ingress[1], SAI_IP_ADDR_FAMILY_IPV4,
            '100.100.1.1', '255.255.255.255', self.tunnel_nexthop_id1)
        VM2_vnet1_ingress_route = sai_thrift_create_route(self.client, self.ovrf_ingress[1], SAI_IP_ADDR_FAMILY_IPV4,
            '100.100.2.1', '255.255.255.255', self.tunnel_nexthop_id2)
        C1_vnet1_ingress_route  = sai_thrift_create_route(self.client, self.ovrf_ingress[1], SAI_IP_ADDR_FAMILY_IPV4,
            '100.100.3.0', '255.255.255.0', self.rif1)
        C2_vnet1_ingress_route  = sai_thrift_create_route(self.client, self.ovrf_ingress[1], SAI_IP_ADDR_FAMILY_IPV4,
            '100.100.4.0', '255.255.255.0', self.rif2)
        C3_vnet1_ingress_route  = sai_thrift_create_route(self.client, self.ovrf_ingress[1], SAI_IP_ADDR_FAMILY_IPV4,
            '100.102.1.0', '255.255.255.0', self.rif3)

        # Create routes for vrid 1 egress
        C1_vnet1_egress_route  = sai_thrift_create_route(self.client, self.ovrf_egress[1], SAI_IP_ADDR_FAMILY_IPV4,
            '100.100.3.0', '255.255.255.0', self.rif1)
        C2_vnet1_egress_route  = sai_thrift_create_route(self.client, self.ovrf_egress[1], SAI_IP_ADDR_FAMILY_IPV4,
            '100.100.4.0', '255.255.255.0', self.rif2)

        # create routes for vrid 2 ingress
        VM1_vnet2_ingress_route  = sai_thrift_create_route(self.client, self.ovrf_ingress[2], SAI_IP_ADDR_FAMILY_IPV4,
            '100.100.1.1', '255.255.255.255', self.tunnel_nexthop_id1)
        VM2_vnet2_ingress_route  = sai_thrift_create_route(self.client, self.ovrf_ingress[2], SAI_IP_ADDR_FAMILY_IPV4,
            '100.100.2.1', '255.255.255.255', self.tunnel_nexthop_id2)
        C1_vnet2_ingress_route   = sai_thrift_create_route(self.client, self.ovrf_ingress[2], SAI_IP_ADDR_FAMILY_IPV4,
            '100.100.3.0', '255.255.255.0', self.rif1)
        C2_vnet2_ingress_route   = sai_thrift_create_route(self.client, self.ovrf_ingress[2], SAI_IP_ADDR_FAMILY_IPV4,
            '100.100.4.0', '255.255.255.0', self.rif2)
        C3_vnet2_ingress_route   = sai_thrift_create_route(self.client, self.ovrf_ingress[2], SAI_IP_ADDR_FAMILY_IPV4,
            '100.102.1.0', '255.255.255.0', self.rif3)

        # Create routes for vrid 2 egress
        C3_vnet2_egress_route   = sai_thrift_create_route(self.client, self.ovrf_egress[2], SAI_IP_ADDR_FAMILY_IPV4,
            '100.102.1.0', '255.255.255.0', self.rif3)

        # Create routes fro vrid 3 ingress
        VM3_vnet3_ingress_route = sai_thrift_create_route(self.client, self.ovrf_ingress[3], SAI_IP_ADDR_FAMILY_IPV4,
            '100.101.2.1', '255.255.255.255', self.tunnel_nexthop_id3)
        C4_vnet3_ingress_route  = sai_thrift_create_route(self.client, self.ovrf_ingress[3], SAI_IP_ADDR_FAMILY_IPV4,
            '100.101.1.0', '255.255.255.0', self.rif4)

        # Create routes for vrid 3 egress
        C4_vnet3_egress_route  = sai_thrift_create_route(self.client, self.ovrf_egress[3], SAI_IP_ADDR_FAMILY_IPV4,
            '100.101.1.0', '255.255.255.0', self.rif4)

        # create neighbor to exercise export/import of routes to other vrf
        sai_thrift_create_neighbor(self.client, SAI_IP_ADDR_FAMILY_IPV4, self.rif4, self.C_ip[4], self.C_mac[4])

    def runTest(self):
        try:
            print "Tunnel Decap:"
            # VM1/2 -> C1/C2/C3
            for c_id in range(1,4):
                for vm_id in range(1,3):
                    if c_id==3:
                        pkt_vni = self.ovrf_ingress_vni[2]
                    else:
                        pkt_vni = self.ovrf_ingress_vni[1]
                    print "sending packet from VM%d to C%d" % (vm_id, c_id)
                    pkt1 = simple_tcp_packet(
                        eth_dst=self.vxlan_default_router_mac,
                        eth_src=self.vxlan_default_router_mac,
                        ip_dst=self.C_ip[c_id],
                        ip_src=self.VM_ip[vm_id],
                        ip_id=108,
                        ip_ttl=64)
                    vxlan_pkt = simple_vxlan_packet(
                        eth_dst=router_mac,
                        eth_src=self.underlay_neighbor_mac,
                        ip_id=0,
                        ip_src=self.host_ip[vm_id],
                        ip_dst=self.my_lb_ip_addr,
                        ip_ttl=64,
                        ip_flags=0x2,
                        udp_sport=11638,
                        vxlan_vni=pkt_vni,
                        with_udp_chksum=False,
                        inner_frame=pkt1)
                    pkt2 = simple_tcp_packet(
                        eth_src=router_mac,
                        eth_dst=self.C_mac[c_id],
                        ip_dst=self.C_ip[c_id],
                        ip_src=self.VM_ip[vm_id],
                        ip_id=108,
                        ip_ttl=63)
                    send_packet(self, switch_ports[5], str(vxlan_pkt))
                    verify_packet(self, pkt2, switch_ports[c_id])

            # VM3 -> C4
            vm_id = 3
            c_id = 4
            pkt_vni = self.ovrf_ingress_vni[3]
            print "sending packet from VM%d to C%d" % (vm_id, c_id)
            pkt1 = simple_tcp_packet(
                eth_dst=router_mac,
                eth_src=self.vxlan_default_router_mac,
                ip_dst=self.C_ip[c_id],
                ip_src=self.VM_ip[vm_id],
                ip_id=108,
                ip_ttl=64)
            vxlan_pkt = simple_vxlan_packet(
                eth_dst=router_mac,
                eth_src=self.underlay_neighbor_mac,
                ip_id=0,
                ip_src=self.host_ip[vm_id],
                ip_dst=self.my_lb_ip_addr,
                ip_ttl=64,
                ip_flags=0x2,
                udp_sport=11638,
                vxlan_vni=pkt_vni,
                with_udp_chksum=False,
                inner_frame=pkt1)
            pkt2 = simple_tcp_packet(
                eth_src=router_mac,
                eth_dst=self.C_mac[c_id],
                ip_dst=self.C_ip[c_id],
                ip_src=self.VM_ip[vm_id],
                ip_id=108,
                ip_ttl=63)
            send_packet(self, switch_ports[5], str(vxlan_pkt))
            verify_packet(self, pkt2, switch_ports[c_id])

            print "Tunnel Encap:"
            # C1/C2/C3 -> VM1/2
            for c_id in range(1,4):
                for vm_id in range(1,3):
                    if vm_id==2:
                        pkt_vni = self.ovrf_ingress_vni[2]
                    elif c_id==3:
                        pkt_vni = self.ovrf_ingress_vni[2]
                    else:
                        pkt_vni = self.ovrf_ingress_vni[1]
                    print "sending packet from C%d to VM%d" % (c_id, vm_id)
                    pkt = simple_tcp_packet(
                              eth_dst=router_mac,
                              eth_src=self.C_mac[c_id],
                              ip_dst=self.VM_ip[vm_id],
                              ip_src=self.C_ip[c_id],
                              ip_id=105,
                              ip_ttl=64)
                    pkt2 = simple_tcp_packet(
                        eth_dst=self.host_inner_dmac[vm_id],
                        eth_src=router_mac,
                        ip_dst=self.VM_ip[vm_id],
                        ip_src=self.C_ip[c_id],
                        ip_id=105,
                        ip_ttl=63)
                    vxlan_pkt = Mask(simple_vxlan_packet(
                        eth_src=router_mac,
                        eth_dst=self.underlay_neighbor_mac,
                        ip_id=0,
                        ip_src=self.my_lb_ip_addr,
                        ip_dst=self.host_ip[vm_id],
                        ip_ttl=64,
                        ip_flags=0x2,
                        with_udp_chksum=False,
                        vxlan_vni=pkt_vni,
                        inner_frame=pkt2))
                    vxlan_pkt.set_do_not_care_scapy(UDP, 'sport')
                    send_packet(self, switch_ports[c_id], str(pkt))
                    verify_packet(self, vxlan_pkt, switch_ports[5])

            # C1/C2/C3 -> VM1/2
            vm_id = 3
            c_id = 4
            pkt_vni = self.ovrf_ingress_vni[3]
            print "sending packet from C%d to VM%d" % (c_id, vm_id)
            pkt = simple_tcp_packet(
                      eth_dst=router_mac,
                      eth_src=self.C_mac[c_id],
                      ip_dst=self.VM_ip[vm_id],
                      ip_src=self.C_ip[c_id],
                      ip_id=105,
                      ip_ttl=64)
            pkt2 = simple_tcp_packet(
                eth_dst=self.host_inner_dmac[vm_id],
                eth_src=router_mac,
                ip_dst=self.VM_ip[vm_id],
                ip_src=self.C_ip[c_id],
                ip_id=105,
                ip_ttl=63)
            vxlan_pkt = Mask(simple_vxlan_packet(
                eth_src=router_mac,
                eth_dst=self.underlay_neighbor_mac,
                ip_id=0,
                ip_src=self.my_lb_ip_addr,
                ip_dst=self.host_ip[vm_id],
                ip_ttl=64,
                ip_flags=0x2,
                with_udp_chksum=False,
                vxlan_vni=pkt_vni,
                inner_frame=pkt2))
            vxlan_pkt.set_do_not_care_scapy(UDP, 'sport')
            send_packet(self, switch_ports[c_id], str(pkt))
            verify_packet(self, vxlan_pkt, switch_ports[5])

        finally:
            print

    def tearDown(self):
        #cleanup
        sai_thrift_remove_route(self.client, self.ovrf_ingress[1], SAI_IP_ADDR_FAMILY_IPV4, '100.100.1.1', '255.255.255.255', self.tunnel_nexthop_id1)
        sai_thrift_remove_route(self.client, self.ovrf_ingress[1], SAI_IP_ADDR_FAMILY_IPV4, '100.100.2.1', '255.255.255.255', self.tunnel_nexthop_id2)
        sai_thrift_remove_route(self.client, self.ovrf_ingress[1], SAI_IP_ADDR_FAMILY_IPV4, '100.100.3.0', '255.255.255.0',   self.rif1)
        sai_thrift_remove_route(self.client, self.ovrf_ingress[1], SAI_IP_ADDR_FAMILY_IPV4, '100.100.4.0', '255.255.255.0',   self.rif2)
        sai_thrift_remove_route(self.client, self.ovrf_ingress[1], SAI_IP_ADDR_FAMILY_IPV4, '100.102.1.0', '255.255.255.0',   self.rif3)
        sai_thrift_remove_route(self.client, self.ovrf_ingress[2], SAI_IP_ADDR_FAMILY_IPV4, '100.100.1.1', '255.255.255.255', self.tunnel_nexthop_id1)
        sai_thrift_remove_route(self.client, self.ovrf_ingress[2], SAI_IP_ADDR_FAMILY_IPV4, '100.100.2.1', '255.255.255.255', self.tunnel_nexthop_id2)
        sai_thrift_remove_route(self.client, self.ovrf_ingress[2], SAI_IP_ADDR_FAMILY_IPV4, '100.100.3.0', '255.255.255.0',   self.rif1)
        sai_thrift_remove_route(self.client, self.ovrf_ingress[2], SAI_IP_ADDR_FAMILY_IPV4, '100.100.4.0', '255.255.255.0',   self.rif2)
        sai_thrift_remove_route(self.client, self.ovrf_ingress[2], SAI_IP_ADDR_FAMILY_IPV4, '100.102.1.0', '255.255.255.0',   self.rif3)
        sai_thrift_remove_route(self.client, self.ovrf_ingress[3], SAI_IP_ADDR_FAMILY_IPV4, '100.101.2.1', '255.255.255.255', self.tunnel_nexthop_id1)
        sai_thrift_remove_route(self.client, self.ovrf_ingress[3], SAI_IP_ADDR_FAMILY_IPV4, '100.101.1.0', '255.255.255.0',   self.rif4)

        sai_thrift_remove_route(self.client, self.ovrf_egress[1], SAI_IP_ADDR_FAMILY_IPV4, '100.100.3.0', '255.255.255.0',   self.rif1)
        sai_thrift_remove_route(self.client, self.ovrf_egress[1], SAI_IP_ADDR_FAMILY_IPV4, '100.100.4.0', '255.255.255.0',   self.rif2)
        sai_thrift_remove_route(self.client, self.ovrf_egress[2], SAI_IP_ADDR_FAMILY_IPV4, '100.102.1.0', '255.255.255.0',   self.rif3)
        sai_thrift_remove_route(self.client, self.ovrf_egress[3], SAI_IP_ADDR_FAMILY_IPV4, '100.101.1.0', '255.255.255.0',   self.rif4)

        sai_thrift_remove_nhop(self.client, [self.tunnel_nexthop_id1, self.tunnel_nexthop_id2, self.tunnel_nexthop_id3])

        self.client.sai_thrift_remove_tunnel_term(self.tunnel_term_id)
        self.client.sai_thrift_remove_tunnel_map_entry(self.C1_encap_tunnel_map_entry)
        self.client.sai_thrift_remove_tunnel_map_entry(self.C3_encap_tunnel_map_entry)
        self.client.sai_thrift_remove_tunnel_map_entry(self.C4_encap_tunnel_map_entry)
        self.client.sai_thrift_remove_tunnel_map_entry(self.C1_decap_tunnel_map_entry)
        self.client.sai_thrift_remove_tunnel_map_entry(self.C3_decap_tunnel_map_entry)
        self.client.sai_thrift_remove_tunnel_map_entry(self.C4_decap_tunnel_map_entry)
        self.client.sai_thrift_remove_tunnel(self.tunnel_id)
        self.client.sai_thrift_remove_tunnel_map(self.encap_tunnel_map)
        self.client.sai_thrift_remove_tunnel_map(self.decap_tunnel_map)

        sai_thrift_remove_neighbor(self.client, SAI_IP_ADDR_FAMILY_IPV4, self.rif1, "100.100.3.1", '00:00:00:00:00:01')
        sai_thrift_remove_neighbor(self.client, SAI_IP_ADDR_FAMILY_IPV4, self.rif2, "100.100.4.1", "00:00:00:00:00:02")
        sai_thrift_remove_neighbor(self.client, SAI_IP_ADDR_FAMILY_IPV4, self.rif3, "100.102.1.1", "00:00:00:00:00:03")
        sai_thrift_remove_neighbor(self.client, SAI_IP_ADDR_FAMILY_IPV4, self.rif4, "100.101.1.1", "00:00:00:00:00:04")
        self.client.sai_thrift_remove_router_interface(self.rif1)
        self.client.sai_thrift_remove_router_interface(self.rif2)
        self.client.sai_thrift_remove_router_interface(self.rif3)
        self.client.sai_thrift_remove_router_interface(self.rif4)

        sai_thrift_remove_route(self.client, self.uvrf, SAI_IP_ADDR_FAMILY_IPV4, '0.0.0.0', '0.0.0.0', self.rif4)
        sai_thrift_remove_neighbor(self.client, SAI_IP_ADDR_FAMILY_IPV4, self.underlay_rif, "1.1.1.1", '00:55:55:55:55:55')
        sai_thrift_remove_nhop(self.client, [self.underlay_nhop])
        self.client.sai_thrift_remove_router_interface(self.underlay_rif)
        self.client.sai_thrift_remove_router_interface(self.urif_lb)
        self.client.sai_thrift_remove_virtual_router(self.ovrf_ingress[1])
        self.client.sai_thrift_remove_virtual_router(self.ovrf_ingress[2])
        self.client.sai_thrift_remove_virtual_router(self.ovrf_ingress[3])
        self.client.sai_thrift_remove_virtual_router(self.uvrf)
        sai_thrift_create_default_bridge_ports(self.client, [self.C1_port, self.C2_port, self.C3_port, self.C4_port, self.port5])
        super(self.__class__, self).tearDown()


@pktpy_skip  # TODO bf-pktpy
@disabled
class L3QinQVxlanTunnel(sai_base_test.ThriftInterfaceDataPlane):
    # QinQ RIF and Routing in-and-out of Vxlan tunnels
    def setUp(self):
        super(self.__class__, self).setUp()
        print
        switch_init(self.client)
        self.C1_port = port_list[1]
        self.C2_port = port_list[2]
        self.C3_port = port_list[3]
        self.C4_port = port_list[4]
        self.port5 = port_list[5]
        
        # One endpoint in each Customer Subnet
        self.C_ip = [0]*5
        self.C_ip[1] = '100.100.3.1'
        self.C_ip[2] = '100.100.4.1'
        self.C_ip[3] = '100.102.1.1'
        self.C_ip[4] = '100.101.1.1'
        
        self.C_mac = [0]*5
        self.C_mac[1] = '00:00:00:00:00:01'
        self.C_mac[2] = '00:00:00:00:00:02'
        self.C_mac[3] = '00:00:00:00:00:03'
        self.C_mac[4] = '00:00:00:00:00:04'

        self.C_outer_vlan = [0]*5
        self.C_outer_vlan[1] = 0x100
        self.C_outer_vlan[2] = 0x200
#        self.C_outer_vlan[3] = 0x300
        self.C_outer_vlan[4] = 0x400

        self.C_inner_vlan = [0]*5
        self.C_inner_vlan[1] = 0x101
        self.C_inner_vlan[2] = 0x201
#        self.C_inner_vlan[3] = 0x301
        self.C_inner_vlan[4] = 0x401
        
        # VMs and their hosts
        self.VM_ip = [0]*4
        self.VM_ip[1] = '100.100.1.1'
        self.VM_ip[2] = '100.100.2.1'
        self.VM_ip[3] = '100.101.2.1'

        self.host_ip = [0]*4
        self.host_ip[1] = '10.10.10.1'
        self.host_ip[2] = '10.10.10.2'
        self.host_ip[3] = '10.10.10.3'

        # Inner Destination MACs
        self.vxlan_default_router_mac='00:11:11:11:11:11'
        
        self.host_inner_dmac = [0]*4
        self.host_inner_dmac[1] = self.vxlan_default_router_mac
        self.host_inner_dmac[2] = '00:12:34:56:78:9a'
        self.host_inner_dmac[3] = self.vxlan_default_router_mac
        
        
        # Create Default VRF ( also used as underlay vrf )
        v4_enabled = 1
        self.uvrf = sai_thrift_create_virtual_router(self.client, v4_enabled, 0)
        
        # Create Underlay loopback RIF ( required for tunnel object creation )
        self.urif_lb = sai_thrift_create_router_interface(self.client, self.uvrf, SAI_ROUTER_INTERFACE_TYPE_LOOPBACK,
                                                          0, 0, 1, 1, router_mac)
        
        #
        # Create Overlay VRFs
        #
        self.ovrf = [0]*4
        self.ovrf[1] = sai_thrift_create_virtual_router(self.client, v4_enabled, 0)
        self.ovrf[2] = sai_thrift_create_virtual_router(self.client, v4_enabled, 0)
        self.ovrf[3] = sai_thrift_create_virtual_router(self.client, v4_enabled, 0)
        
        self.ovrf_vni = [0]*4
        self.ovrf_vni[1] = 2000
        self.ovrf_vni[2] = 2001
        self.ovrf_vni[3] = 2005

        #
        # Setup underlay default route, the nexthop is 1.1.1.1
        #
        self.underlay_neighbor_mac = '00:55:55:55:55:55'
        self.underlay_nhop_ip = '1.1.1.1'
        self.underlay_route_addr = '0.0.0.0'
        self.underlay_route_mask = '0.0.0.0'
        self.underlay_rif = sai_thrift_create_router_interface(self.client, self.uvrf, SAI_ROUTER_INTERFACE_TYPE_PORT, self.port5, 0, 1, 0 , 0)
        self.underlay_nhop = sai_thrift_create_nhop(self.client, SAI_IP_ADDR_FAMILY_IPV4, self.underlay_nhop_ip, self.underlay_rif)
        self.underlay_neighbor = sai_thrift_create_neighbor(self.client, SAI_IP_ADDR_FAMILY_IPV4,
                                                       self.underlay_rif, self.underlay_nhop_ip, self.underlay_neighbor_mac)
        self.underlay_default_route = sai_thrift_create_route(self.client, self.uvrf, SAI_IP_ADDR_FAMILY_IPV4,
                                                         self.underlay_route_addr, self.underlay_route_mask, self.underlay_nhop)
        
        #
        # Setup overlay routes
        #
        
        # create QinQ router interface for C1 
        self.rif1 = sai_thrift_create_router_interface(self.client, self.ovrf[1], SAI_ROUTER_INTERFACE_TYPE_QINQ_PORT, self.C1_port, 0, 1, 1, '', self.C_outer_vlan[1], self.C_inner_vlan[1])
        sai_thrift_create_neighbor(self.client, SAI_IP_ADDR_FAMILY_IPV4, self.rif1, self.C_ip[1], self.C_mac[1])
        
        # create QinQ-based router interface for C2 
        self.rif2 = sai_thrift_create_router_interface(self.client, self.ovrf[1], SAI_ROUTER_INTERFACE_TYPE_QINQ_PORT, self.C2_port, 0, 1, 1, '', self.C_outer_vlan[2], self.C_inner_vlan[2])
        sai_thrift_create_neighbor(self.client, SAI_IP_ADDR_FAMILY_IPV4, self.rif2, self.C_ip[2], self.C_mac[2])
        
        # create port-based router interface for C3
        self.rif3 = sai_thrift_create_router_interface(self.client, self.ovrf[2], SAI_ROUTER_INTERFACE_TYPE_PORT, self.C3_port, 0, 1, 1, '')
        sai_thrift_create_neighbor(self.client, SAI_IP_ADDR_FAMILY_IPV4, self.rif3, self.C_ip[3], self.C_mac[3])
        
        # create QinQ-based router interface for C4 
        self.rif4 = sai_thrift_create_router_interface(self.client, self.ovrf[3], SAI_ROUTER_INTERFACE_TYPE_QINQ_PORT, self.C4_port, 0, 1, 1, '', self.C_outer_vlan[4], self.C_inner_vlan[4])
        sai_thrift_create_neighbor(self.client, SAI_IP_ADDR_FAMILY_IPV4, self.rif4, self.C_ip[4], self.C_mac[4])
        
        
        #
        # Create Tunnel
        #
        
        # Create Encap/decap mappers
        self.encap_tunnel_map = sai_thrift_create_tunnel_map(self.client, SAI_TUNNEL_MAP_TYPE_VIRTUAL_ROUTER_ID_TO_VNI)
        self.decap_tunnel_map = sai_thrift_create_tunnel_map(self.client, SAI_TUNNEL_MAP_TYPE_VNI_TO_VIRTUAL_ROUTER_ID)
        
        # Create Tunnel object
        self.my_lb_ip_addr = '10.10.10.10'
        self.my_lb_ip_mask = '255.255.255.255'
        self.tunnel_id = sai_thrift_create_tunnel(self.client, SAI_TUNNEL_TYPE_VXLAN, SAI_IP_ADDR_FAMILY_IPV4, self.my_lb_ip_addr, self.urif_lb, 0, self.encap_tunnel_map, self.decap_tunnel_map)
        
        # Create Tunnel Map entries for C1, C2
        self.C1_encap_tunnel_map_entry = sai_thrift_create_tunnel_map_entry(self.client, SAI_TUNNEL_MAP_TYPE_VIRTUAL_ROUTER_ID_TO_VNI, self.encap_tunnel_map, 0, 0, self.ovrf[1], self.ovrf_vni[1])
        self.C1_decap_tunnel_map_entry = sai_thrift_create_tunnel_map_entry(self.client, SAI_TUNNEL_MAP_TYPE_VNI_TO_VIRTUAL_ROUTER_ID, self.decap_tunnel_map, 0, 0, self.ovrf[1], self.ovrf_vni[1])
        
        # Create Tunnel Map entries for C3
        self.C3_encap_tunnel_map_entry = sai_thrift_create_tunnel_map_entry(self.client, SAI_TUNNEL_MAP_TYPE_VIRTUAL_ROUTER_ID_TO_VNI, self.encap_tunnel_map, 0, 0, self.ovrf[2], self.ovrf_vni[2])
        self.C3_decap_tunnel_map_entry = sai_thrift_create_tunnel_map_entry(self.client, SAI_TUNNEL_MAP_TYPE_VNI_TO_VIRTUAL_ROUTER_ID, self.decap_tunnel_map, 0, 0, self.ovrf[2], self.ovrf_vni[2])
        
        # Create Tunnel Map entries for C4
        self.C4_encap_tunnel_map_entry = sai_thrift_create_tunnel_map_entry(self.client, SAI_TUNNEL_MAP_TYPE_VIRTUAL_ROUTER_ID_TO_VNI, self.encap_tunnel_map, 0, 0, self.ovrf[3], self.ovrf_vni[3])
        self.C4_decap_tunnel_map_entry = sai_thrift_create_tunnel_map_entry(self.client, SAI_TUNNEL_MAP_TYPE_VNI_TO_VIRTUAL_ROUTER_ID, self.decap_tunnel_map, 0, 0, self.ovrf[3], self.ovrf_vni[3])
        
        # Create tunnel decap for VM to customer server 
        self.tunnel_term_id = sai_thrift_create_tunnel_term_table(self.client, SAI_TUNNEL_TYPE_VXLAN, SAI_IP_ADDR_FAMILY_IPV4, self.uvrf,
                                                       self.my_lb_ip_addr, self.my_lb_ip_mask, self.tunnel_id, SAI_TUNNEL_TERM_TABLE_ENTRY_TYPE_P2MP)
        
        # create tunnel nexthop for VM1, VM2 and VM3 */
        self.tunnel_nexthop_id1 = sai_thrift_create_nhop(self.client, SAI_IP_ADDR_FAMILY_IPV4, self.host_ip[1], self.tunnel_id, True, self.ovrf_vni[1], self.host_inner_dmac[1])
        self.tunnel_nexthop_id2 = sai_thrift_create_nhop(self.client, SAI_IP_ADDR_FAMILY_IPV4, self.host_ip[2], self.tunnel_id, True, self.ovrf_vni[2], self.host_inner_dmac[2])
        self.tunnel_nexthop_id3 = sai_thrift_create_nhop(self.client, SAI_IP_ADDR_FAMILY_IPV4, self.host_ip[1], self.tunnel_id, True, self.ovrf_vni[3], self.host_inner_dmac[3])
        
        # Create routes for vrid 1 ingress */
        VM1_vnet1_route = sai_thrift_create_route(self.client, self.ovrf[1], SAI_IP_ADDR_FAMILY_IPV4, '100.100.1.1', '255.255.255.255', self.tunnel_nexthop_id1)
        VM2_vnet1_route = sai_thrift_create_route(self.client, self.ovrf[1], SAI_IP_ADDR_FAMILY_IPV4, '100.100.2.1', '255.255.255.255', self.tunnel_nexthop_id2)
        C1_vnet1_route  = sai_thrift_create_route(self.client, self.ovrf[1], SAI_IP_ADDR_FAMILY_IPV4, '100.100.3.0', '255.255.255.0', self.rif1)
        C2_vnet1_route  = sai_thrift_create_route(self.client, self.ovrf[1], SAI_IP_ADDR_FAMILY_IPV4, '100.100.4.0', '255.255.255.0', self.rif2)
        C3_vnet1_route  = sai_thrift_create_route(self.client, self.ovrf[1], SAI_IP_ADDR_FAMILY_IPV4, '100.102.1.0', '255.255.255.0', self.rif3)
        
        # Create routes for vrid 1 egress
        
        # create routes for vrid 2 ingress 
        VM1_vnet2_route  = sai_thrift_create_route(self.client, self.ovrf[2], SAI_IP_ADDR_FAMILY_IPV4, '100.100.1.1', '255.255.255.255', self.tunnel_nexthop_id1)
        VM2_vnet2_route  = sai_thrift_create_route(self.client, self.ovrf[2], SAI_IP_ADDR_FAMILY_IPV4, '100.100.2.1', '255.255.255.255', self.tunnel_nexthop_id2)
        C1_vnet2_route   = sai_thrift_create_route(self.client, self.ovrf[2], SAI_IP_ADDR_FAMILY_IPV4, '100.100.3.0', '255.255.255.0', self.rif1)
        C2_vnet2_route   = sai_thrift_create_route(self.client, self.ovrf[2], SAI_IP_ADDR_FAMILY_IPV4, '100.100.4.0', '255.255.255.0', self.rif2)
        C3_vnet2_route   = sai_thrift_create_route(self.client, self.ovrf[2], SAI_IP_ADDR_FAMILY_IPV4, '100.102.1.0', '255.255.255.0', self.rif3)
        
        # Create routes fro vrid 3 ingress 
        VM3_vnet3_route = sai_thrift_create_route(self.client, self.ovrf[3], SAI_IP_ADDR_FAMILY_IPV4, '100.101.2.1', '255.255.255.255', self.tunnel_nexthop_id3)
        C4_vnet3_route  = sai_thrift_create_route(self.client, self.ovrf[3], SAI_IP_ADDR_FAMILY_IPV4, '100.101.1.0', '255.255.255.0', self.rif4)
        
    def runTest(self):
        try:
            print "QinQ RIF -> QinQ RIF"
            pkt = simple_qinq_tcp_packet(
                eth_dst=router_mac,
                dl_vlan_outer=self.C_outer_vlan[1],
                vlan_vid=self.C_inner_vlan[1],
                eth_src=self.C_mac[1],
                ip_dst=self.C_ip[2],
                ip_src=self.C_ip[1],
                ip_ttl=64)
            
            exp_pkt = simple_qinq_tcp_packet(
                eth_dst=self.C_mac[2],
                eth_src=router_mac,
                dl_vlan_outer=self.C_outer_vlan[2],
                vlan_vid=self.C_inner_vlan[2],
                ip_dst=self.C_ip[2],
                ip_src=self.C_ip[1],
                ip_ttl=63)

            send_packet(self,switch_ports[1], str(pkt))
            verify_packet(self, exp_pkt, switch_ports[2])
            
            print "QinQ RIF -> Port based RIF"
            pkt = simple_qinq_tcp_packet(
                eth_dst=router_mac,
                dl_vlan_outer=self.C_outer_vlan[2],
                vlan_vid=self.C_inner_vlan[2],
                eth_src=self.C_mac[2],
                ip_dst=self.C_ip[3],
                ip_src=self.C_ip[2],
                ip_ttl=64)
            
            exp_pkt = simple_tcp_packet(
                pktlen=92,
                eth_dst=self.C_mac[3],
                eth_src=router_mac,
                ip_dst=self.C_ip[3],
                ip_src=self.C_ip[2],
                ip_ttl=63)

            send_packet(self, switch_ports[2], str(pkt))
            verify_packet(self, exp_pkt, switch_ports[3])

            print "Port Based RIF -> QinQ RIF"
            pkt = simple_tcp_packet(
                eth_dst=router_mac,
                eth_src=self.C_mac[3],
                ip_dst=self.C_ip[1],
                ip_src=self.C_ip[3],
                ip_ttl=64)

            exp_pkt = simple_qinq_tcp_packet(
                pktlen=108,
                eth_dst=self.C_mac[1],
                eth_src=router_mac,
                dl_vlan_outer=self.C_outer_vlan[1],
                vlan_vid=self.C_inner_vlan[1],
                ip_dst=self.C_ip[1],
                ip_src=self.C_ip[3],
                ip_ttl=63)
            
            send_packet(self, switch_ports[3], str(pkt))
            verify_packet(self, exp_pkt, switch_ports[1])


            print "Tunnel Decap:"
            # VM1/2 -> C1/C2/C3
            for c_id in range(1,4):
                for vm_id in range(1,3):
                    if c_id==3:
                        pkt_vni = self.ovrf_vni[2]
                    else:
                        pkt_vni = self.ovrf_vni[1]
                    print "sending packet from VM%d to C%d" % (vm_id, c_id)
                    pkt1 = simple_tcp_packet(
                        eth_dst=router_mac,
                        eth_src=self.vxlan_default_router_mac,
                        ip_dst=self.C_ip[c_id],
                        ip_src=self.VM_ip[vm_id],
                        ip_ttl=64)
                    vxlan_pkt = simple_vxlan_packet(
                        eth_dst=router_mac,
                        eth_src=self.underlay_neighbor_mac,
                        ip_id=0,
                        ip_src=self.host_ip[vm_id],
                        ip_dst=self.my_lb_ip_addr,
                        ip_ttl=64,
                        ip_flags=0x0,
                        udp_sport=11638,
                        vxlan_vni=pkt_vni,
                        with_udp_chksum=False,
                        inner_frame=pkt1)
                    pkt2 = simple_qinq_tcp_packet(
                        pktlen=108,
                        eth_src=router_mac,
                        eth_dst=self.C_mac[c_id],
                        dl_vlan_outer=self.C_outer_vlan[c_id],
                        vlan_vid=self.C_inner_vlan[c_id],
                        ip_dst=self.C_ip[c_id],
                        ip_src=self.VM_ip[vm_id],
                        ip_ttl=63)
                    send_packet(self, switch_ports[5], str(vxlan_pkt))
                    verify_packet(self, pkt2, switch_ports[c_id])
                    
            # VM3 -> C4
            vm_id = 3
            c_id = 4
            pkt_vni = self.ovrf_vni[3]
            print "sending packet from VM%d to C%d" % (vm_id, c_id)
            pkt1 = simple_tcp_packet(
                eth_dst=router_mac,
                eth_src=self.vxlan_default_router_mac,
                ip_dst=self.C_ip[c_id],
                ip_src=self.VM_ip[vm_id],
                ip_ttl=64)
            vxlan_pkt = simple_vxlan_packet(
                eth_dst=router_mac,
                eth_src=self.underlay_neighbor_mac,
                ip_id=0,
                ip_src=self.host_ip[vm_id],
                ip_dst=self.my_lb_ip_addr,
                ip_ttl=64,
                ip_flags=0x0,
                udp_sport=11638,
                vxlan_vni=pkt_vni,
                with_udp_chksum=False,
                inner_frame=pkt1)
            pkt2 = simple_qinq_tcp_packet(
                pktlen=108,
                eth_src=router_mac,
                eth_dst=self.C_mac[c_id],
                dl_vlan_outer=self.C_outer_vlan[c_id],
                vlan_vid=self.C_inner_vlan[c_id],
                ip_dst=self.C_ip[c_id],
                ip_src=self.VM_ip[vm_id],
                ip_ttl=63)
            send_packet(self, switch_ports[5], str(vxlan_pkt))
            verify_packet(self, pkt2, switch_ports[c_id])
        
            print "Tunnel Encap:"
            # C1/C2/C3 -> VM1/2
            for c_id in range(1,4):
                for vm_id in range(1,3):
                    if vm_id==2:
                        pkt_vni = self.ovrf_vni[2]
                    elif c_id==3:
                        pkt_vni = self.ovrf_vni[2]
                    else:
                        pkt_vni = self.ovrf_vni[1]
                    print "sending packet from C%d to VM%d" % (c_id, vm_id)
                    pkt = simple_qinq_tcp_packet(
                              eth_dst=router_mac,
                              dl_vlan_outer=self.C_outer_vlan[c_id],
                              vlan_vid=self.C_inner_vlan[c_id],
                              eth_src=self.C_mac[c_id],
                              ip_dst=self.VM_ip[vm_id],
                              ip_src=self.C_ip[c_id],
                              ip_ttl=64)
                    pkt2 = simple_tcp_packet(
                        pktlen=92,
                        eth_dst=self.host_inner_dmac[vm_id],
                        eth_src=router_mac,
                        ip_dst=self.VM_ip[vm_id],
                        ip_src=self.C_ip[c_id],
                        ip_ttl=63)
                    udp_sport = entropy_hash(pkt)
                    vxlan_pkt = simple_vxlan_packet(
                        eth_src=router_mac,
                        eth_dst=self.underlay_neighbor_mac,
                        ip_id=0,
                        ip_src=self.my_lb_ip_addr,
                        ip_dst=self.host_ip[vm_id],
                        ip_ttl=64,
                        ip_flags=0x0,
                        udp_sport=udp_sport,
                        with_udp_chksum=False,
                        vxlan_vni=pkt_vni,
                        inner_frame=pkt2)
                    send_packet(self, switch_ports[c_id], str(pkt))
                    verify_packet(self, vxlan_pkt, switch_ports[5])
                    
            # C1/C2/C3 -> VM1/2
            vm_id = 3
            c_id = 4
            pkt_vni = self.ovrf_vni[3]
            print "sending packet from C%d to VM%d" % (c_id, vm_id)
            pkt = simple_qinq_tcp_packet(
                      eth_dst=router_mac,
                      eth_src=self.C_mac[c_id],
                      dl_vlan_outer=self.C_outer_vlan[c_id],
                      vlan_vid=self.C_inner_vlan[c_id],
                      ip_dst=self.VM_ip[vm_id],
                      ip_src=self.C_ip[c_id],
                      ip_ttl=64)
            pkt2 = simple_tcp_packet(
                pktlen=92,
                eth_dst=self.host_inner_dmac[vm_id],
                eth_src=router_mac,
                ip_dst=self.VM_ip[vm_id],
                ip_src=self.C_ip[c_id],
                ip_ttl=63)
            udp_sport = entropy_hash(pkt)
            vxlan_pkt = simple_vxlan_packet(
                eth_src=router_mac,
                eth_dst=self.underlay_neighbor_mac,
                ip_id=0,
                ip_src=self.my_lb_ip_addr,
                ip_dst=self.host_ip[vm_id],
                ip_ttl=64,
                ip_flags=0x0,
                udp_sport=udp_sport,
                with_udp_chksum=False,
                vxlan_vni=pkt_vni,
                inner_frame=pkt2)
            send_packet(self, switch_ports[c_id], str(pkt))
            verify_packet(self, vxlan_pkt, switch_ports[5])
                    
        finally:
            print

    def tearDown(self):
        #cleanup
        sai_thrift_remove_route(self.client, self.ovrf[1], SAI_IP_ADDR_FAMILY_IPV4, '100.100.1.1', '255.255.255.255', self.tunnel_nexthop_id1)
        sai_thrift_remove_route(self.client, self.ovrf[1], SAI_IP_ADDR_FAMILY_IPV4, '100.100.2.1', '255.255.255.255', self.tunnel_nexthop_id2)
        sai_thrift_remove_route(self.client, self.ovrf[1], SAI_IP_ADDR_FAMILY_IPV4, '100.100.3.0', '255.255.255.0',   self.rif1)
        sai_thrift_remove_route(self.client, self.ovrf[1], SAI_IP_ADDR_FAMILY_IPV4, '100.100.4.0', '255.255.255.0',   self.rif2)
        sai_thrift_remove_route(self.client, self.ovrf[1], SAI_IP_ADDR_FAMILY_IPV4, '100.102.1.0', '255.255.255.0',   self.rif3)
        sai_thrift_remove_route(self.client, self.ovrf[2], SAI_IP_ADDR_FAMILY_IPV4, '100.100.1.1', '255.255.255.255', self.tunnel_nexthop_id1)
        sai_thrift_remove_route(self.client, self.ovrf[2], SAI_IP_ADDR_FAMILY_IPV4, '100.100.2.1', '255.255.255.255', self.tunnel_nexthop_id2)
        sai_thrift_remove_route(self.client, self.ovrf[2], SAI_IP_ADDR_FAMILY_IPV4, '100.100.3.0', '255.255.255.0',   self.rif1)
        sai_thrift_remove_route(self.client, self.ovrf[2], SAI_IP_ADDR_FAMILY_IPV4, '100.100.4.0', '255.255.255.0',   self.rif2)
        sai_thrift_remove_route(self.client, self.ovrf[2], SAI_IP_ADDR_FAMILY_IPV4, '100.102.1.0', '255.255.255.0',   self.rif3)
        sai_thrift_remove_route(self.client, self.ovrf[3], SAI_IP_ADDR_FAMILY_IPV4, '100.101.2.1', '255.255.255.255', self.tunnel_nexthop_id3)
        sai_thrift_remove_route(self.client, self.ovrf[3], SAI_IP_ADDR_FAMILY_IPV4, '100.101.1.0', '255.255.255.0',   self.rif4)
        
        sai_thrift_remove_nhop(self.client, [self.tunnel_nexthop_id1, self.tunnel_nexthop_id2, self.tunnel_nexthop_id3])
        
        self.client.sai_thrift_remove_tunnel_term(self.tunnel_term_id)
        self.client.sai_thrift_remove_tunnel_map_entry(self.C1_encap_tunnel_map_entry)
        self.client.sai_thrift_remove_tunnel_map_entry(self.C3_encap_tunnel_map_entry)
        self.client.sai_thrift_remove_tunnel_map_entry(self.C4_encap_tunnel_map_entry)
        self.client.sai_thrift_remove_tunnel_map_entry(self.C1_decap_tunnel_map_entry)
        self.client.sai_thrift_remove_tunnel_map_entry(self.C3_decap_tunnel_map_entry)
        self.client.sai_thrift_remove_tunnel_map_entry(self.C4_decap_tunnel_map_entry)
        self.client.sai_thrift_remove_tunnel_map(self.encap_tunnel_map)
        self.client.sai_thrift_remove_tunnel_map(self.decap_tunnel_map)
        self.client.sai_thrift_remove_tunnel(self.tunnel_id)
        
        sai_thrift_remove_neighbor(self.client, SAI_IP_ADDR_FAMILY_IPV4, self.rif1, "100.100.3.1", '00:00:00:00:00:01')
        sai_thrift_remove_neighbor(self.client, SAI_IP_ADDR_FAMILY_IPV4, self.rif2, "100.100.4.1", "00:00:00:00:00:02")
        sai_thrift_remove_neighbor(self.client, SAI_IP_ADDR_FAMILY_IPV4, self.rif3, "100.102.1.1", "00:00:00:00:00:03")
        sai_thrift_remove_neighbor(self.client, SAI_IP_ADDR_FAMILY_IPV4, self.rif4, "100.101.1.1", "00:00:00:00:00:04")
        self.client.sai_thrift_remove_router_interface(self.rif1)
        self.client.sai_thrift_remove_router_interface(self.rif2)
        self.client.sai_thrift_remove_router_interface(self.rif3)
        self.client.sai_thrift_remove_router_interface(self.rif4)
        
        sai_thrift_remove_route(self.client, self.uvrf, SAI_IP_ADDR_FAMILY_IPV4, '0.0.0.0', '0.0.0.0', self.rif4)
        sai_thrift_remove_neighbor(self.client, SAI_IP_ADDR_FAMILY_IPV4, self.underlay_rif, "1.1.1.1", '00:55:55:55:55:55')
        sai_thrift_remove_nhop(self.client, [self.underlay_nhop])
        self.client.sai_thrift_remove_router_interface(self.underlay_rif)
        self.client.sai_thrift_remove_router_interface(self.urif_lb)
        self.client.sai_thrift_remove_virtual_router(self.ovrf[1])
        self.client.sai_thrift_remove_virtual_router(self.ovrf[2])
        self.client.sai_thrift_remove_virtual_router(self.ovrf[3])
        self.client.sai_thrift_remove_virtual_router(self.uvrf)
        super(self.__class__, self).tearDown()

@disabled  # msft fg-ecmp
#  This tests the double tagged [0x8100 over 0x8100] L2 switching
#  Tests egress out of both tag/untag port
class ParseQinQL2ForwTest(sai_base_test.ThriftInterfaceDataPlane):
    def runTest(self):
        print
        print "ParseQinQL2ForwTest"
        switch_init(self.client)
        port1 = port_list[0]
        port2 = port_list[1]
        port3 = port_list[2]
        v4_enabled = 1
        v6_enabled = 1

        mac = ''
        inner_vlan = 255
    
        self.vr1 = sai_thrift_create_virtual_router(self.client, v4_enabled, v6_enabled)

        vlan = 500
        self.vlan500 = sai_thrift_create_vlan(self.client, vlan)
        self.vlan502_member = sai_thrift_create_vlan_member(self.client, self.vlan500, port2, SAI_VLAN_TAGGING_MODE_TAGGED)
        self.vlan503_member = sai_thrift_create_vlan_member(self.client, self.vlan500, port3, SAI_VLAN_TAGGING_MODE_TAGGED)
        self.vlan501_member = sai_thrift_create_vlan_member(self.client, self.vlan500, port1, SAI_VLAN_TAGGING_MODE_UNTAGGED)

        attr_value = sai_thrift_attribute_value_t(u16=vlan)
        attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
        self.client.sai_thrift_set_port_attribute(port1, attr)

        self.rif_1 = sai_thrift_create_router_interface(self.client, self.vr1, SAI_ROUTER_INTERFACE_TYPE_VLAN, 0, self.vlan500, v4_enabled, v6_enabled, mac)
        
        addr_family = SAI_IP_ADDR_FAMILY_IPV4
        ip_add_1 = '10.10.10.1'
        ip_add_1_subnet = '10.10.10.0'
        ip_mask1 = '255.255.255.0'

        ip_add_2 = '172.16.31.1'
        ip_add_2_subnet = '172.16.31.0'
        ip_mask2 = '255.255.255.0'

        mac1 = '00:11:22:33:44:55'
        mac2 = '00:11:22:33:44:56'

        # tx the test packet
        print "QinQ -> L2 VLAN"
        qq_pkt = simple_qinq_tcp_packet(eth_dst=mac2,
                                    eth_src=mac1,
                                    dl_vlan_outer=vlan,
                                    vlan_vid=inner_vlan,
                                    ip_dst=ip_add_1,
                                    ip_src=ip_add_2,
                                    ip_ttl=64,
                                    tcp_sport=0x80,
                                    tcp_dport=0x40)
        out_qq_pkt = simple_qinq_tcp_packet(
                                    eth_dst=mac2,
                                    eth_src=mac1,
                                    dl_vlan_outer=vlan,
                                    vlan_vid=inner_vlan,
                                    ip_dst=ip_add_1,
                                    ip_src=ip_add_2,
                                    ip_ttl=64,
                                    tcp_sport=0x80,
                                    tcp_dport=0x40)
        out_pkt = simple_tcp_packet(eth_dst=mac2,
                                    eth_src=mac1,
                                    dl_vlan_enable=True,
                                    vlan_vid=inner_vlan,
                                    ip_dst=ip_add_1,
                                    ip_src=ip_add_2,
                                    ip_ttl=64,
                                    tcp_sport=0x80,
                                    tcp_dport=0x40,
                                    pktlen=96)

        try:
            # qinq 0x8100 over 0x8100 packet
            print "Tx qinq packet - outer vlan 500, inner vlan 255"
            print "  Flood to untag port 1[itag-255], tag port 3[otag-500, itag-255]"
            send_packet(self, switch_ports[1], str(qq_pkt))
            #verify_packets(self, out_qq_pkt, [switch_ports[2]])
            verify_each_packet_on_each_port(self, [out_qq_pkt, out_pkt], [switch_ports[2], switch_ports[0]])

            sai_thrift_create_fdb(self.client, self.vlan500, mac2, port1, SAI_PACKET_ACTION_FORWARD)
            print "Tx qinq packet - outer vlan 500, inner vlan 255"
            print "  Forw to untag port 1[itag-255]"
            send_packet(self, switch_ports[1], str(qq_pkt))
            verify_packets(self, out_pkt, [switch_ports[0]])
            sai_thrift_delete_fdb(self.client, self.vlan500, mac2, port1)

            sai_thrift_create_fdb(self.client, self.vlan500, mac2, port3, SAI_PACKET_ACTION_FORWARD)
            print "Tx qinq packet - outer vlan 500, inner vlan 255"
            print "  Forw to tag port 3[otag-500, itag-255]"
            send_packet(self, switch_ports[1], str(qq_pkt))
            verify_packets(self, out_qq_pkt, [switch_ports[2]])
            sai_thrift_delete_fdb(self.client, self.vlan500, mac2, port3)

        finally:
            sai_thrift_flush_fdb(self.client, flush_type=SAI_FDB_FLUSH_ENTRY_TYPE_ALL)

            self.client.sai_thrift_remove_router_interface(self.rif_1)

            attr_value = sai_thrift_attribute_value_t(u16=1)
            attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
            self.client.sai_thrift_set_port_attribute(port1, attr)

            self.client.sai_thrift_remove_vlan_member(self.vlan501_member)
            self.client.sai_thrift_remove_vlan_member(self.vlan502_member)
            self.client.sai_thrift_remove_vlan_member(self.vlan503_member)
            self.client.sai_thrift_remove_vlan(self.vlan500)

            self.client.sai_thrift_remove_virtual_router(self.vr1)

@disabled  # msft fg-ecmp
###########################################################################################################

# This tests Parsing/Routing/Ecmp with consistent-hash for the below tunnel
# packet combinations - uses inner pkt to calculate consistent hash
# [8 vxlan + 8 nvgre + 2 nvgre-st combinations]
#   Vxlan -> outer v4/v6 - Vxlan - Inner Eth - Inner v4/v6 - Inner Tcp/Udp
#   Nvgre ->  outer v4/v6 - Nvgre [st-key!=0x6400 or st-key!=config] - Inner Eth - Inner v4/v6 - Inner Tcp/Udp
#   Nvgre-st -> outer v4/v6 - Nvgre [st-key==0x6400 or st-key==config] - Inner eth - Inner v4/v6 - Inner Tcp/Udp

# Routing of tunnel packets [all above type] based on outer IP - using regular Ecmp table with 
# consistent inner-pkt hash i.e. Verify if the same Ecmp group member is used for the same flow 
# flowing in either directions [Between inner IP-A to inner IP-B or inner IP-B to inner IP-A]

class L3EcmpTunnelsInnerConsHashTest(sai_base_test.ThriftInterfaceDataPlane):
    def runTest(self):
        print
        print "L3EcmpTunnelsInnerConsHashTest"
        switch_init(self.client)
        port1 = port_list[1]
        port2 = port_list[2]
        port3 = port_list[3]
        port4 = port_list[4]
        v4_enabled = 1
        v6_enabled = 1

        mac = ''
        self.vr1 = sai_thrift_create_virtual_router(self.client, v4_enabled, v6_enabled)
        pkt_elist = [None] * 3

        vlan1 = 50
        self.vlan50 = sai_thrift_create_vlan(self.client, vlan1)
        self.vlan50_member = sai_thrift_create_vlan_member(self.client, self.vlan50, port4, SAI_VLAN_TAGGING_MODE_TAGGED)

        vlan2 = 500
        self.vlan500 = sai_thrift_create_vlan(self.client, vlan2)
        self.vlan502_member = sai_thrift_create_vlan_member(self.client, self.vlan500, port2, SAI_VLAN_TAGGING_MODE_TAGGED)
        self.vlan503_member = sai_thrift_create_vlan_member(self.client, self.vlan500, port3, SAI_VLAN_TAGGING_MODE_TAGGED)
        self.vlan501_member = sai_thrift_create_vlan_member(self.client, self.vlan500, port1, SAI_VLAN_TAGGING_MODE_UNTAGGED)

        attr_value = sai_thrift_attribute_value_t(u16=vlan2)
        attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
        self.client.sai_thrift_set_port_attribute(port1, attr)

        self.rif_1 = sai_thrift_create_router_interface(self.client, self.vr1, SAI_ROUTER_INTERFACE_TYPE_VLAN, 0, self.vlan500, v4_enabled, v6_enabled, mac)
        self.rif_2 = sai_thrift_create_router_interface(self.client, self.vr1, SAI_ROUTER_INTERFACE_TYPE_VLAN, 0, self.vlan50, v4_enabled, v6_enabled, mac)
        
        sm = '00:44:44:44:44:44'
        tm = '00:88:88:88:88:88'
        nvgre_st_key = 0x6400

        v4_family = SAI_IP_ADDR_FAMILY_IPV4
        nhop_mac1 = '00:55:55:55:55:50'
        nhop_mac2 = '00:55:55:55:55:55'
        nhop_mac3 = '00:55:55:55:55:58'

        # v4 nhop, neighbor setup
        nhop_ip1 = '50.50.50.1'
        nhop_ip2 = '50.50.50.5'
        nhop_ip3 = '50.50.50.10'
        mac_action = SAI_PACKET_ACTION_FORWARD

        sai_thrift_create_fdb(self.client, self.vlan500, nhop_mac1, port1, mac_action)
        sai_thrift_create_neighbor(self.client, v4_family, self.rif_1, nhop_ip1, nhop_mac1)
        nhop1 = sai_thrift_create_nhop(self.client, v4_family, nhop_ip1, self.rif_1)

        sai_thrift_create_fdb(self.client, self.vlan500, nhop_mac2, port2, mac_action)
        sai_thrift_create_neighbor(self.client, v4_family, self.rif_1, nhop_ip2, nhop_mac2)
        nhop2 = sai_thrift_create_nhop(self.client, v4_family, nhop_ip2, self.rif_1)

        sai_thrift_create_fdb(self.client, self.vlan500, nhop_mac3, port3, mac_action)
        sai_thrift_create_neighbor(self.client, v4_family, self.rif_1, nhop_ip3, nhop_mac3)
        nhop3 = sai_thrift_create_nhop(self.client, v4_family, nhop_ip3, self.rif_1)

        # v4 ecmp setup
        ecmp = sai_thrift_create_next_hop_group(self.client)
        ecmp_mem1 = sai_thrift_create_next_hop_group_member(self.client, ecmp, nhop1)
        ecmp_mem2 = sai_thrift_create_next_hop_group_member(self.client, ecmp, nhop2)
        ecmp_mem3 = sai_thrift_create_next_hop_group_member(self.client, ecmp, nhop3)

        # repeating the same set of nhop in new ecmp members
        ecmp_mem4 = sai_thrift_create_next_hop_group_member(self.client, ecmp, nhop1)
        ecmp_mem5 = sai_thrift_create_next_hop_group_member(self.client, ecmp, nhop3)

        v6_family = SAI_IP_ADDR_FAMILY_IPV6

        # v6 nhop, neighbor setup
        nhop_ip_6_1 = '5000::5:1'
        nhop_ip_6_2 = '5000::5:5'
        nhop_ip_6_3 = '5000::5:10'
        mac_action = SAI_PACKET_ACTION_FORWARD

        sai_thrift_create_neighbor(self.client, v6_family, self.rif_1, nhop_ip_6_1, nhop_mac1)
        nhop_6_1 = sai_thrift_create_nhop(self.client, v6_family, nhop_ip_6_1, self.rif_1)

        sai_thrift_create_neighbor(self.client, v6_family, self.rif_1, nhop_ip_6_2, nhop_mac2)
        nhop_6_2 = sai_thrift_create_nhop(self.client, v6_family, nhop_ip_6_2, self.rif_1)

        sai_thrift_create_neighbor(self.client, v6_family, self.rif_1, nhop_ip_6_3, nhop_mac3)
        nhop_6_3 = sai_thrift_create_nhop(self.client, v6_family, nhop_ip_6_3, self.rif_1)

        # v6 ecmp setup
        v6_ecmp = sai_thrift_create_next_hop_group(self.client)
        v6_ecmp_mem1 = sai_thrift_create_next_hop_group_member(self.client, v6_ecmp, nhop_6_1)
        v6_ecmp_mem2 = sai_thrift_create_next_hop_group_member(self.client, v6_ecmp, nhop_6_2)
        v6_ecmp_mem3 = sai_thrift_create_next_hop_group_member(self.client, v6_ecmp, nhop_6_3)

        # repeating the same set of nhop in new ecmp members
        v6_ecmp_mem4 = sai_thrift_create_next_hop_group_member(self.client, v6_ecmp, nhop_6_1)
        
        # v4 route - outer ip 1
        o_ip_A = '172.16.31.1'
        o_ip_A_subnet = '172.16.31.0'
        o_ip_mask = '255.255.255.255'
        sai_thrift_create_route(self.client, self.vr1, v4_family, o_ip_A, o_ip_mask, ecmp)

        # v4 route - outer ip 2
        o_ip_B = '104.104.14.55'
        o_ip_B_subnet = '104.104.14.0'
        sai_thrift_create_route(self.client, self.vr1, v4_family, o_ip_B, o_ip_mask, ecmp)

        # v6 route - outer ip 1
        o_v6_ip_A = '2222::5:5'
        o_v6_ip_A_subnet = '2222::5:0'
        o_v6_ip_mask = 'ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff'
        sai_thrift_create_route(self.client, self.vr1, v6_family, o_v6_ip_A, o_v6_ip_mask, v6_ecmp)

        # v6 route - outer ip 2
        o_v6_ip_B = '8888::8080:AA'
        o_v6_ip_B_subnet = '8888::8080:0'
        sai_thrift_create_route(self.client, self.vr1, v6_family, o_v6_ip_B, o_v6_ip_mask, v6_ecmp)

        o_l4_port_A = 14014
        o_l4_port_B = 15155
        src_v4 = '10.10.10.1'
        src_v6 = '1111::4:4'

        def GetInnerPkt(v4, tcp, src_port, tar_port, sip, dip):
            inner_pkt = None
            if (v4):
                if (tcp == False) :
                    inner_pkt = simple_udp_packet(
                                                eth_dst=tm,
                                                eth_src=sm,
                                                ip_dst=dip,
                                                ip_src=sip,
                                                ip_ttl=55,
                                                ip_id=108,
                                                udp_sport=src_port,
                                                udp_dport=tar_port)
                else:
                    inner_pkt = simple_tcp_packet(
                                                eth_dst=tm,
                                                eth_src=sm,
                                                ip_dst=dip,
                                                ip_src=sip,
                                                ip_ttl=55,
                                                ip_id=108,
                                                tcp_sport=src_port,
                                                tcp_dport=tar_port)
            else:
                if (tcp == False) :
                    inner_pkt = simple_udpv6_packet(
                                                eth_dst=tm,
                                                eth_src=sm,
                                                ipv6_dst=dip,
                                                ipv6_src=sip,
                                                ipv6_hlim=55,
                                                udp_sport=src_port,
                                                udp_dport=tar_port)
                else:
                    inner_pkt = simple_tcpv6_packet(
                                                eth_dst=tm,
                                                eth_src=sm,
                                                ipv6_dst=dip,
                                                ipv6_src=sip,
                                                ipv6_hlim=55,
                                                tcp_sport=src_port,
                                                tcp_dport=tar_port)
            return inner_pkt
        
        def GetVxlanPkt(smac, tmac, vlan, v4, ttl, i_pkt, rx_pkt, opp_flow):
            vx_vi = 10000
            if (opp_flow):
                o_ip = o_ip_B
                o_src_port = o_l4_port_B
                o_v6_ip = o_v6_ip_B
            else:
                o_ip = o_ip_A
                o_src_port = o_l4_port_A
                o_v6_ip = o_v6_ip_A

            if (v4):
                if (vlan == None):
                    vxlan_pkt = simple_vxlan_packet(eth_dst=tmac,
                                                eth_src=smac,
                                                ip_src=src_v4,
                                                ip_dst=o_ip,
                                                ip_ttl=ttl,
                                                ip_flags=0x2,
                                                udp_sport=o_src_port,
                                                vxlan_vni=vx_vi,
                                                with_udp_chksum=False,
                                                inner_frame=i_pkt)
                else:
                    vxlan_pkt = simple_vxlan_packet(eth_dst=tmac,
                                                eth_src=smac,
                                                dl_vlan_enable=True,
                                                vlan_vid=vlan,
                                                ip_src=src_v4,
                                                ip_dst=o_ip,
                                                ip_ttl=ttl,
                                                ip_flags=0x2,
                                                udp_sport=o_src_port,
                                                vxlan_vni=vx_vi,
                                                with_udp_chksum=False,
                                                inner_frame=i_pkt)
            else:
                if (vlan == None):
                    vxlan_pkt = simple_vxlanv6_packet(eth_dst=tmac,
                                                eth_src=smac,
                                                ipv6_src=src_v6,
                                                ipv6_dst=o_v6_ip,
                                                ipv6_hlim=ttl,
                                                udp_sport=o_src_port,
                                                vxlan_vni=vx_vi,
                                                with_udp_chksum=False,
                                                inner_frame=i_pkt)
                else:
                    vxlan_pkt = simple_vxlanv6_packet(eth_dst=tmac,
                                                eth_src=smac,
                                                dl_vlan_enable=True,
                                                vlan_vid=vlan,
                                                ipv6_src=src_v6,
                                                ipv6_dst=o_v6_ip,
                                                ipv6_hlim=ttl,
                                                udp_sport=o_src_port,
                                                vxlan_vni=vx_vi,
                                                with_udp_chksum=False,
                                                inner_frame=i_pkt)
            
            if (rx_pkt):
                vxlan_pkt = Mask(vxlan_pkt)
                vxlan_pkt.set_do_not_care_scapy(UDP, 'chksum')

            return vxlan_pkt
        

        def GetNvgrePkt(smac, tmac, vlan, v4, ttl, grekey, i_pkt, opp_flow):
            nvgre_pkt = None
            
            if (opp_flow):
                o_ip = o_ip_B
                o_v6_ip = o_v6_ip_B
            else:
                o_ip = o_ip_A
                o_v6_ip = o_v6_ip_A

            #  For nvgre_st_pkt
            #  - outer v4 -> nvgre [key=100 or config] -> inner v6 [without inner ethernet]
            # if (grekey == nvgre_st_key and v4 and i_pkt['IPv6']):
            #    i_pkt = i_pkt['IPv6']

            if (v4):
                # gre key = 32 bits i.e. combination of tni [vs], flow-id
                # nvgre_v4_pkt - outer v4 -> nvgre [key!=0x6400 or key!=config] -> inner v4/v6
                # nvgre_st_pkt - outer v4 -> nvgre [key=0x6400 or config] -> inner v4/v6
                if (vlan == None):
                    nvgre_pkt = simple_nvgre_packet(eth_dst=tmac,
                                                eth_src=smac,
                                                ip_src=src_v4,
                                                ip_dst=o_ip,
                                                ip_ttl=ttl,
                                                nvgre_tni=grekey>>8,
                                                nvgre_flowid=grekey&0xFF,
                                                inner_frame=i_pkt)
                else:
                    nvgre_pkt = simple_nvgre_packet(eth_dst=tmac,
                                                eth_src=smac,
                                                dl_vlan_enable=True,
                                                vlan_vid=vlan,
                                                ip_src=src_v4,
                                                ip_dst=o_ip,
                                                ip_ttl=ttl,
                                                nvgre_tni=grekey>>8,
                                                nvgre_flowid=grekey&0xFF,
                                                inner_frame=i_pkt)
            else: 
                # gre key = 32 bits i.e. combination of tni [vs], flow-id 
                # nvgre_v6_pkt - outer v6 -> nvgre [key!=0x6400 or key!=config] -> inner v4/v6
                # nvgre_st_pkt - outer v6 -> nvgre [key=0x6400 or config] -> inner v4/v6
                if (vlan == None):
                    nvgre_pkt = simple_nvgrev6_packet(eth_dst=tmac,
                                                eth_src=smac,
                                                ipv6_src=src_v6,
                                                ipv6_dst=o_v6_ip,
                                                ipv6_hlim=ttl,
                                                nvgre_tni=grekey>>8,
                                                nvgre_flowid=grekey&0xFF,
                                                inner_frame=i_pkt)
                else:
                    nvgre_pkt = simple_nvgrev6_packet(eth_dst=tmac,
                                                eth_src=smac,
                                                dl_vlan_enable=True,
                                                vlan_vid=vlan,
                                                ipv6_src=src_v6,
                                                ipv6_dst=o_v6_ip,
                                                ipv6_hlim=ttl,
                                                nvgre_tni=grekey>>8,
                                                nvgre_flowid=grekey&0xFF,
                                                inner_frame=i_pkt)

            return nvgre_pkt


        def VxlanTestConsistentInnerHash(i_v4, i_ip_A, i_ip_B, i_tcp, i_l4_port_A, i_l4_port_B, o_v4):
            # Function  def - for parameter reference
            # - GetInnerPkt(v4, tcp, src_port, tar_port, sip, dip)
            # - GetVxlanPkt(smac, tmac, vlan, v4, ttl, i_pkt, rx_pkt, opp_flow)

            print " Vxlan [outer %s, inner %s, inner %s]" \
                %("v4" if o_v4==True else "v6", "v4" if i_v4==True else "v6", "Tcp" if i_tcp==True else "Udp")
            print "  Tx outer flow Z to A"
            print "  -- %s outer [%s --> %s, src port 0x%x]" \
                %("v4" if o_v4==True else "v6", src_v4 if o_v4==True else src_v6, \
                o_ip_A if o_v4==True else o_v6_ip_A, o_l4_port_A)
            print "  -- %s inner [%s --> %s, %s src port 0x%x, tar port 0x%x]" \
                %("v4" if i_v4==True else "v6", i_ip_A, i_ip_B, "Tcp" if i_tcp==True else "Udp", \
                i_l4_port_A, i_l4_port_B)

            i_pkt = GetInnerPkt(i_v4, i_tcp, i_l4_port_A, i_l4_port_B, i_ip_A, i_ip_B)
            vpkt = GetVxlanPkt(sm, router_mac, 50, o_v4, 64,  i_pkt, False, False)
            pkt_elist[0] = GetVxlanPkt(router_mac, nhop_mac1, None, o_v4, 63,  i_pkt, True, False)
            pkt_elist[1] = GetVxlanPkt(router_mac, nhop_mac2, 500, o_v4, 63,  i_pkt, True, False)
            pkt_elist[2] = GetVxlanPkt(router_mac, nhop_mac3, 500, o_v4, 63,  i_pkt, True, False)

            send_packet(self, switch_ports[4], str(vpkt))
            rx_port = verify_packet_any_port_get_port(self, 
                            [switch_ports[1], switch_ports[2], switch_ports[3]])
            print "  Ecmp path using inner pkt consistent hash -> outgoing port %d" %(rx_port)
            print ""
            send_packet(self, switch_ports[4], str(vpkt))
            verify_packet(self, pkt_elist[rx_port-1], rx_port)


            print "  Tx outer flow Z to B --> Reversing the inner IP flow"
            print "  -- %s outer [%s --> %s, src port 0x%x]" \
                %("v4" if o_v4==True else "v6", src_v4 if o_v4==True else src_v6, \
                    o_ip_B if o_v4==True else o_v6_ip_B, o_l4_port_B)
            print "  -- %s inner [%s --> %s, %s src port 0x%x, tar port 0x%x]" \
                %("v4" if i_v4==True else "v6", i_ip_B, i_ip_A, "Tcp" if i_tcp==True else "Udp", \
                i_l4_port_B, i_l4_port_A)
               
            i_pkt = GetInnerPkt(i_v4, i_tcp, i_l4_port_B, i_l4_port_A, i_ip_B, i_ip_A)
            vpkt = GetVxlanPkt(sm, router_mac, 50, o_v4, 64, i_pkt, False, True)
            pkt_elist[0] = GetVxlanPkt(router_mac, nhop_mac1, None, o_v4, 63,  i_pkt, True, True)
            pkt_elist[1] = GetVxlanPkt(router_mac, nhop_mac2, 500, o_v4, 63,  i_pkt, True, True)
            pkt_elist[2] = GetVxlanPkt(router_mac, nhop_mac3, 500, o_v4, 63,  i_pkt, True, True)
            
            send_packet(self, switch_ports[4], str(vpkt))
            verify_packet(self, pkt_elist[rx_port-1], rx_port)
            print "  Ecmp path using inner pkt consistent hash -> outgoing port %d" %(rx_port)
            print " Inner IP in both directions points to same nexthop outgoing port %d" %(rx_port)
            print ""

        def NvgreTestConsistentInnerHash(i_v4, i_ip_A, i_ip_B, i_tcp, i_l4_port_A, i_l4_port_B, o_v4, grekey):
            # Function  def - for parameter reference
            # - GetInnerPkt(v4, tcp, src_port, tar_port, sip, dip)
            # - GetNvgrePkt(smac, tmac, vlan, v4, ttl, grekey, i_pkt, opp_flow)

            print " Nvgre [outer %s, inner %s, inner %s]" \
                %("v4" if o_v4==True else "v6", "v4" if i_v4==True else "v6", "Tcp" if i_tcp==True else "Udp")
            print "  Tx outer flow Z to A"
            print "  -- %s outer [%s --> %s, Gre Key 0x%x]" \
                %("v4" if o_v4==True else "v6", src_v4 if o_v4==True else src_v6, \
                    o_ip_A if o_v4==True else o_v6_ip_A, grekey)
            print "  -- %s inner [%s --> %s, %s src port 0x%x, tar port 0x%x]" \
                %("v4" if i_v4==True else "v6", i_ip_A, i_ip_B, "Tcp" if i_tcp==True else "Udp", \
                i_l4_port_A, i_l4_port_B)
            
            i_pkt = GetInnerPkt(i_v4, i_tcp, i_l4_port_A, i_l4_port_B, i_ip_A, i_ip_B)
            vpkt = GetNvgrePkt(sm, router_mac, 50, o_v4, 64, grekey, i_pkt, False)
            pkt_elist[0] = GetNvgrePkt(router_mac, nhop_mac1, None, o_v4, 63, grekey, i_pkt, False)
            pkt_elist[1] = GetNvgrePkt(router_mac, nhop_mac2, 500, o_v4, 63, grekey, i_pkt, False)
            pkt_elist[2] = GetNvgrePkt(router_mac, nhop_mac3, 500, o_v4, 63, grekey, i_pkt, False)

            send_packet(self, switch_ports[4], str(vpkt))
            rx_port = verify_packet_any_port_get_port(self, 
                            [switch_ports[1], switch_ports[2], switch_ports[3]])
            print "  Ecmp path using inner pkt consistent hash -> outgoing port %d" %(rx_port)
            print ""
            send_packet(self, switch_ports[4], str(vpkt))
            verify_packet(self, pkt_elist[rx_port-1], rx_port)


            print "  Tx outer flow Z to B --> Reversing the inner IP flow"
            print "  -- %s outer [%s --> %s, Gre Key 0x%x]" \
                %("v4" if o_v4==True else "v6", src_v4 if o_v4==True else src_v6, 
                o_ip_B if o_v4==True else o_v6_ip_B, grekey)
            print "  -- %s inner [%s --> %s, %s src port 0x%x, tar port 0x%x]" \
                %("v4" if i_v4==True else "v6", i_ip_B, i_ip_A, "Tcp" if i_tcp==True else "Udp", \
                i_l4_port_B, i_l4_port_A)
            
            i_pkt = GetInnerPkt(i_v4, i_tcp, i_l4_port_B, i_l4_port_A, i_ip_B, i_ip_A)
            vpkt = GetNvgrePkt(sm, router_mac, 50, o_v4, 64, grekey, i_pkt, True)
            pkt_elist[0] = GetNvgrePkt(router_mac, nhop_mac1, None, o_v4, 63, grekey, i_pkt, True)
            pkt_elist[1] = GetNvgrePkt(router_mac, nhop_mac2, 500, o_v4, 63, grekey, i_pkt, True)
            pkt_elist[2] = GetNvgrePkt(router_mac, nhop_mac3, 500, o_v4, 63, grekey, i_pkt, True)
            
            send_packet(self, switch_ports[4], str(vpkt))
            verify_packet(self, pkt_elist[rx_port-1], rx_port)
            print "  Ecmp path using inner pkt consistent hash -> outgoing port %d" %(rx_port)
            print " Inner IP in both directions points to same nexthop outgoing port %d" %(rx_port)
            print ""


        try:
            # Function  def - for parameter reference
            # - GetInnerPkt(v4, tcp, src_port, tar_port, sip, dip)
            # - GetVxlanPkt(smac, tmac, vlan, v4, ttl, i_pkt, rx_pkt, opp_flow)
            # - GetNvgrePkt(smac, tmac, vlan, v4, ttl, grekey, i_pkt, opp_flow)

            # nhop_mac1 --> switch_ports[1] --> untag port 1
            # nhop_mac2 --> switch_ports[2] --> tag port 2
            # nhop_mac3 --> switch_ports[3] --> tag port 3
            
            #  vxlan - all combinations
            print ""
            print "--------------------------------------------------------------------------"
            print("Testing ecmp consistent hash for all combinations of Vxlan tunnel packets ")
            #  VxlanTestConsistentInnerHash(i_v4, i_ip_A, i_ip_B, i_tcp, i_l4_port_A, i_l4_port_B, o_v4)
            #  out-pkt v4
            VxlanTestConsistentInnerHash(True, '58.58.58.5', '10.80.120.20', False, 40004, 22445, True) 
            VxlanTestConsistentInnerHash(True, '158.100.25.55', '155.20.140.155', True, 505, 14888, True) 
            VxlanTestConsistentInnerHash(False,'4444::A855:8BBB:1', '1515::5225:28', True, 28455, 14888, True) 
            VxlanTestConsistentInnerHash(False,'5884::515:8004:85A', '5555::55:100:A41B', False, 8888, 5555, True) 
            #  out-pkt v6
            VxlanTestConsistentInnerHash(True, '120.140.150.55', '55.100.120.150', False, 22222, 44444, False) 
            VxlanTestConsistentInnerHash(True, '5.100.8.1', '88.155.55.44', True, 50505, 11558, False) 
            VxlanTestConsistentInnerHash(False,'88E5:588B:A::AB55:AB5', 'ABC0::52A5:A2', True, 11558, 18514, False) 
            VxlanTestConsistentInnerHash(False,'588E::5E15:8AB4:88', '55A5:8FB2::5B15:100', False, 8884, 25745, False) 


            #  nvgre - all combinations
            print ""
            print "--------------------------------------------------------------------------"
            print("Testing ecmp consistent hash for all combinations of Nvgre tunnel packets ")
            #   - gre key = 32 bits i.e. combination of tni [vs], flow-id 
            #  nvgre_pkt    ->  outer v4/v6 - Nvgre [st-key!=0x6400 or st-key!=config] - Inner Eth - Inner v4/v6 - Inner Tcp/Udp
            #  nvgre_st_pkt -> outer v4/v6 - Nvgre [st-key==0x6400 or st-key==config] - Inner eth - Inner v4/v6 - Inner Tcp/Udp

            #  NvgreTestConsistentInnerHash(i_v4, i_ip_A, i_ip_B, i_tcp, i_l4_port_A, i_l4_port_B, o_v4, grekey)
            #   out-pkt v4
            NvgreTestConsistentInnerHash(True, '58.58.58.5', '10.80.120.20', False, 40004, 22445, True, 0xA64) 
            NvgreTestConsistentInnerHash(True, '158.100.25.55', '155.20.140.155', True, 505, 14888, True, 0x64A) 
            NvgreTestConsistentInnerHash(False,'4444::A855:8BBB:1', '1515::5225:28', True, 28455, 14888, True, 0x5564) 
            NvgreTestConsistentInnerHash(False,'5884::515:8004:85A', '5555::55:100:A41B', False, 8888, 5555, True, 0xAAF5)
            #   out-pkt v6
            NvgreTestConsistentInnerHash(True, '120.140.150.55', '55.100.120.150', False, 22222, 44444, False, 0x6455) 
            NvgreTestConsistentInnerHash(False,'88E5:588B:A::AB55:AB5', 'ABC0::52A5:A2', True, 11558, 18514, False, 0xFF01) 


            #  nvgre-st - all combinations
            print ""
            print "----------------------------------------------------------------------------"
            print("Testing ecmp consistent hash for all combinations of Nvgre-st tunnel packets")
            #  use gre key value 0x6400 for nvgre-st in this ptf
            #  If you want to use any other key value, then change the nvgre_st_key above
            #  Also requires P4 value set configuration of nvgre_st_key
            NvgreTestConsistentInnerHash(False,'4444::A855:8BBB:1', '1515::5225:28', True, 28455, 14888, True, nvgre_st_key) 
            NvgreTestConsistentInnerHash(False,'5884::515:8004:85A', '5555::55:100:A41B', False, 8888, 5555, True, nvgre_st_key)
            #  using nvgre-st key value with v6 outer pkt, v4 inner pkt - this also has to be parsed  as nvgre-st [new req]
            NvgreTestConsistentInnerHash(True, '5.100.8.14', '88.155.55.44', True, 50505, 1000, False, nvgre_st_key)
            NvgreTestConsistentInnerHash(False,'588E::5E15:8AB4:88', '55A5:8FB2::5B15:100', False, 8884, 25745, False, nvgre_st_key)
        

            #  Test ecmp consistent hash with Inner V6 pkt - vxlan/nvgre/nvgre-st
            #  All combinations of inner v6 sip/dip  - mainly to test crc32 hash using high/low sip/dip 
            print ""
            print "------------------------------------------------------------------------------------------"
            print("Testing ecmp consistent hash for all combinations using inner v6 sip/dip")
            print("Ecmp consistent crc32 hash - sequence [low-ip, high-ip, protocol, low-l4-port, high-l4-port]")
            #  VxlanTestConsistentInnerHash(i_v4, i_ip_A, i_ip_B, i_tcp, i_l4_port_A, i_l4_port_B, o_v4)
            VxlanTestConsistentInnerHash(False,'4444::A855:8BBB:1AB1', '4444::A855:8BBB:5118', True, 28455, 14888, True) 
            VxlanTestConsistentInnerHash(False,'4444::A855:8BBB:1AB1', '4444::A855:ABFE:1AB1', False, 8888, 5555, True) 
            VxlanTestConsistentInnerHash(False,'88E5::1481:EFAB:AB55:AB5', '88E5::1481:1111:AB55:AB5', True, 11558, 18514, False) 
            VxlanTestConsistentInnerHash(False,'88E5::8AF4:EFAB:AB55:AB5', '88E5::8AF5:EFAB:AB55:AB5', False, 8884, 25745, False)

            #  NvgreTestConsistentInnerHash(i_v4, i_ip_A, i_ip_B, i_tcp, i_l4_port_A, i_l4_port_B, o_v4, grekey)
            NvgreTestConsistentInnerHash(False,'85AF:0000:EF14:84FA::A855:1', '85AF:0000:EF14:84FB::A855:1', True, 28455, 14888, True, 0x564) 
            NvgreTestConsistentInnerHash(False,'85AF:0000:0505:84FA::A855:1', '85AF:0000:0508:84FA::A855:1', False, 8888, 5555, True, 0x10006455)
            NvgreTestConsistentInnerHash(False,'AAAA:1488::588B:4588', 'AAAA:1588::588B:4588', True, 11558, 18514, False, 0xFF01) 

            #  nvgre-st scnearios - uses only lower 32 bits for hash
            NvgreTestConsistentInnerHash(False,'85AF:0000:0505:84FA::A855:1', '85AF:0000:0508:84FA::A855:1', False, 8888, 5555, True, nvgre_st_key)
            NvgreTestConsistentInnerHash(False,'85AF:0000:0505:84FA::A855:E585', '85AF:0000:0508:84FA::A855:5A5B', False, 8888, 5555, True, nvgre_st_key)
            #  using nvgre-st key value with v6 outer pkt, v4 inner pkt - this also has to be parsed  as nvgre-st [new req]
            NvgreTestConsistentInnerHash(False,'AABB:1488::588B:4588', 'AABE:1488::588B:4588', False, 8884, 25745, False, nvgre_st_key) 
        
        finally:
            sai_thrift_remove_route(self.client, self.vr1, v6_family, o_v6_ip_A, o_v6_ip_mask, v6_ecmp)
            sai_thrift_remove_route(self.client, self.vr1, v6_family, o_v6_ip_B, o_v6_ip_mask, v6_ecmp)
            
            self.client.sai_thrift_remove_next_hop_group_member(v6_ecmp_mem1)
            self.client.sai_thrift_remove_next_hop_group_member(v6_ecmp_mem2)
            self.client.sai_thrift_remove_next_hop_group_member(v6_ecmp_mem3)
            self.client.sai_thrift_remove_next_hop_group_member(v6_ecmp_mem4)
            self.client.sai_thrift_remove_next_hop_group(v6_ecmp)
            self.client.sai_thrift_remove_next_hop(nhop_6_2)
            self.client.sai_thrift_remove_next_hop(nhop_6_3)
            self.client.sai_thrift_remove_next_hop(nhop_6_1)
            sai_thrift_remove_neighbor(self.client, v6_family, self.rif_1, nhop_ip_6_1, nhop_mac1)
            sai_thrift_remove_neighbor(self.client, v6_family, self.rif_1, nhop_ip_6_3, nhop_mac3)
            sai_thrift_remove_neighbor(self.client, v6_family, self.rif_1, nhop_ip_6_2, nhop_mac2)
            

            sai_thrift_remove_route(self.client, self.vr1, v4_family, o_ip_A, o_ip_mask, ecmp)
            sai_thrift_remove_route(self.client, self.vr1, v4_family, o_ip_B, o_ip_mask, ecmp)
            self.client.sai_thrift_remove_next_hop_group_member(ecmp_mem1)
            self.client.sai_thrift_remove_next_hop_group_member(ecmp_mem2)
            self.client.sai_thrift_remove_next_hop_group_member(ecmp_mem3)
            self.client.sai_thrift_remove_next_hop_group_member(ecmp_mem4)
            self.client.sai_thrift_remove_next_hop_group_member(ecmp_mem5)
            self.client.sai_thrift_remove_next_hop_group(ecmp)
            self.client.sai_thrift_remove_next_hop(nhop1)
            self.client.sai_thrift_remove_next_hop(nhop3)
            self.client.sai_thrift_remove_next_hop(nhop2)
            sai_thrift_remove_neighbor(self.client, v4_family, self.rif_1, nhop_ip1, nhop_mac1)
            sai_thrift_remove_neighbor(self.client, v4_family, self.rif_1, nhop_ip3, nhop_mac3)
            sai_thrift_remove_neighbor(self.client, v4_family, self.rif_1, nhop_ip2, nhop_mac2)

            sai_thrift_flush_fdb_by_vlan(self.client, self.vlan50)
            sai_thrift_flush_fdb_by_vlan(self.client, self.vlan500)
            sai_thrift_flush_fdb(self.client, flush_type=SAI_FDB_FLUSH_ENTRY_TYPE_ALL)

            self.client.sai_thrift_remove_router_interface(self.rif_1)
            self.client.sai_thrift_remove_router_interface(self.rif_2)

            attr_value = sai_thrift_attribute_value_t(u16=1)
            attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
            self.client.sai_thrift_set_port_attribute(port1, attr)
            self.client.sai_thrift_remove_vlan_member(self.vlan501_member)
            self.client.sai_thrift_remove_vlan_member(self.vlan502_member)
            self.client.sai_thrift_remove_vlan_member(self.vlan503_member)
            self.client.sai_thrift_remove_vlan(self.vlan500)

            self.client.sai_thrift_remove_vlan_member(self.vlan50_member)
            self.client.sai_thrift_remove_vlan(self.vlan50)

            self.client.sai_thrift_remove_virtual_router(self.vr1)

@disabled  # msft fg-ecmp
##################################################################################################
# FG Ecmp - This tests Parsing/Routing/FG-Ecmp with consistent-hash for the below tunnel 
# packet combinations - uses inner pkt to calculate consistent hash 
# [8 vxlan + 8 nvgre + 2 nvgre-st combinations]
#   Vxlan -> outer v4/v6 - Vxlan - Inner Eth - Inner v4/v6 - Inner Tcp/Udp
#   Nvgre ->  outer v4/v6 - Nvgre [st-key!=0x6400 or st-key!=config] - Inner Eth - Inner v4/v6 - Inner Tcp/Udp
#   Nvgre-st -> outer v4/v6 - Nvgre [st-key==0x6400 or st-key==config] - Inner eth - Inner v4/v6 - Inner Tcp/Udp

# Routing of tunnel packets [all above type] based on outer IP - using Fine Grain Ecmp type with 
# consistent inner-pkt hash i.e. Verify if the same Ecmp group member is used for the same flow 
# flowing in either directions [Between inner IP-A to inner IP-B or inner IP-B to inner IP-A]

# This test uses 2 FG Ecmp groups [one for v4, one for v6], 14 Nhops, with scale of 140 Ecmp-members in a Ecmp
# Same 14 nhops are repitively program in 140 Ecmp-members [action profile] within a Ecmp
#   - Ecmp1 with 140 members - 14 nhops [First 10 mem - nhop1, Next 10 mem - nhop2 ...... Last 10 members - nhop14]
#   - Ecmp2 with 140 members - 14 nhops [Nhop 0,1,2,3,4,5,6,7,8,9,10,11,12,13, 0,1,2,3,4 .... etc]

class FGEcmpTunnelsConsHashScaleEMemSharingNhopTest(sai_base_test.ThriftInterfaceDataPlane):
    def runTest(self):
        print
        print "FGEcmpTunnelsConsHashScaleEMemSharingNhopTest"
        switch_init(self.client)
        v4_enabled = 1
        v6_enabled = 1

        num_nhop = 14
        num_ecmp_mem = 512
        num_port = num_nhop
        i_port = []
        e_port = []
        #  using port 0 - 13
        for cnt in range(0, num_port):
            i_port.append(port_list[cnt])
            e_port.append(switch_ports[cnt])

        port14 = port_list[14]

        mac = ''
        self.vr1 = sai_thrift_create_virtual_router(self.client, v4_enabled, v6_enabled)
        pkt_elist = [None] * 3

        vlan1 = 50
        self.vlan50 = sai_thrift_create_vlan(self.client, vlan1)
        self.vlan50_member = sai_thrift_create_vlan_member(self.client, self.vlan50, port14, SAI_VLAN_TAGGING_MODE_TAGGED)

        vlan2 = 500
        self.vlan500 = sai_thrift_create_vlan(self.client, vlan2)
        v500_tag_list = [ SAI_VLAN_TAGGING_MODE_UNTAGGED, SAI_VLAN_TAGGING_MODE_TAGGED, SAI_VLAN_TAGGING_MODE_TAGGED,
                          SAI_VLAN_TAGGING_MODE_UNTAGGED, SAI_VLAN_TAGGING_MODE_TAGGED, SAI_VLAN_TAGGING_MODE_TAGGED,
                          SAI_VLAN_TAGGING_MODE_UNTAGGED, SAI_VLAN_TAGGING_MODE_TAGGED, SAI_VLAN_TAGGING_MODE_TAGGED,
                          SAI_VLAN_TAGGING_MODE_UNTAGGED, SAI_VLAN_TAGGING_MODE_TAGGED, SAI_VLAN_TAGGING_MODE_TAGGED,
                          SAI_VLAN_TAGGING_MODE_UNTAGGED, SAI_VLAN_TAGGING_MODE_TAGGED]
        v500_mem_list = []
        attr_value = sai_thrift_attribute_value_t(u16=vlan2)
        attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
        for cnt in range(0, num_port):
            vmem = sai_thrift_create_vlan_member(self.client, self.vlan500, i_port[cnt], v500_tag_list[cnt])
            v500_mem_list.append(vmem)
            if v500_tag_list[cnt] == SAI_VLAN_TAGGING_MODE_UNTAGGED  :
                self.client.sai_thrift_set_port_attribute(i_port[cnt], attr)

        self.rif_1 = sai_thrift_create_router_interface(self.client, self.vr1, SAI_ROUTER_INTERFACE_TYPE_VLAN, 0, self.vlan500, v4_enabled, v6_enabled, mac)
        self.rif_2 = sai_thrift_create_router_interface(self.client, self.vr1, SAI_ROUTER_INTERFACE_TYPE_VLAN, 0, self.vlan50, v4_enabled, v6_enabled, mac)
        
        sm = '00:44:44:44:44:44'
        tm = '00:88:88:88:88:88'
        nvgre_st_key = 0x6400

        v4_family = SAI_IP_ADDR_FAMILY_IPV4
        nhop_mac_list = ['00:55:55:55:55:50', '00:55:55:55:55:51', '00:55:55:55:55:52',  
                         '00:55:55:55:55:53', '00:55:55:55:55:54', '00:55:55:55:55:55', 
                         '00:55:55:55:55:56', '00:55:55:55:55:57', '00:55:55:55:55:58',
                         '00:55:55:55:55:59', '00:55:55:55:55:5A', '00:55:55:55:55:5B',
                         '00:55:55:55:55:5C', '00:55:55:55:55:5F']

        # v4 nhop, neighbor setup
        nhop_ip_list = ['50.50.50.15', '50.50.50.1', '50.50.50.2', '50.50.50.3',
                        '50.50.50.4',  '50.50.50.5', '50.50.50.6', '50.50.50.7',  
                        '50.50.50.8',  '50.50.50.9', '50.50.50.10', '50.50.50.11', 
                        '50.50.50.12', '50.50.50.13']
        
        mac_action = SAI_PACKET_ACTION_FORWARD
        nhop_list = []
        for cnt in range(0, num_nhop):
            sai_thrift_create_fdb(self.client, self.vlan500, nhop_mac_list[cnt], i_port[cnt], mac_action)
            sai_thrift_create_neighbor(self.client, v4_family, self.rif_1, nhop_ip_list[cnt], nhop_mac_list[cnt])
            nhop = sai_thrift_create_nhop(self.client, v4_family, nhop_ip_list[cnt], self.rif_1)
            nhop_list.append(nhop)
        print "Number of v4-neighbor/nexthop entries - %d" %num_nhop 

        # v4 ecmp setup
        # ecmp - member using nhops in below repetitve pattern
        # [First 14 members - nhop 0, next 14 mem - nhop 1 ... last 14 mem - nhop 13]
        ecmp_mem_list = []
        nhop_i = 0
        fg_ecmp = sai_thrift_create_next_hop_group(self.client, type=SAI_NEXT_HOP_GROUP_TYPE_FINE_GRAIN_ECMP)
        for cnt in range(0, num_ecmp_mem):
            ecmp_mem = sai_thrift_create_next_hop_group_member(self.client, fg_ecmp, nhop_list[nhop_i], index=cnt)
            if cnt % num_nhop == 0:
                nhop_i = nhop_i + 1
                if nhop_i == num_nhop:
                    nhop_i = 0
            ecmp_mem_list.append(ecmp_mem)
        print "Number of v4-ecmp-mem entries - %d" %num_ecmp_mem 

        v6_family = SAI_IP_ADDR_FAMILY_IPV6

        # v6 nhop, neighbor setup
        nhop_ip_6_list = ['5000::5:15', '5000::5:1', '5000::5:2', '5000::5:3', 
                          '5000::5:4',  '5000::5:5', '5000::5:6', '5000::5:7', 
                          '5000::5:8',  '5000::5:9', '5000::5:10', '5000::5:11', 
                          '5000::5:12', '5000::5:13']
        mac_action = SAI_PACKET_ACTION_FORWARD

        nhop_6_list = []
        for cnt in range(0, num_nhop):
            sai_thrift_create_neighbor(self.client, v6_family, self.rif_1, nhop_ip_6_list[cnt], nhop_mac_list[cnt])
            nhop = sai_thrift_create_nhop(self.client, v6_family, nhop_ip_6_list[cnt], self.rif_1)
            nhop_6_list.append(nhop)
        print "Number of v6-neighbor/nexthop entries - %d" %num_nhop 

        # v6 ecmp setup
        # ecmp - member using nhops in below repetitve pattern
        # [0,1,2,3,4,5,6,7,8,9,10,11,12,13,0,1,2 .... ]
        ecmp_mem_6_list = []
        fg_v6_ecmp = sai_thrift_create_next_hop_group(self.client, type=SAI_NEXT_HOP_GROUP_TYPE_FINE_GRAIN_ECMP)
        for cnt in range(0, num_ecmp_mem):
            ecmp_mem = sai_thrift_create_next_hop_group_member(self.client, fg_v6_ecmp, nhop_6_list[cnt % num_nhop], index=cnt)
            ecmp_mem_6_list.append(ecmp_mem)
        print "Number of v6-ecmp-mem entries - %d" %num_ecmp_mem

        # v4 route - outer ip 1
        o_ip_A = '172.16.31.1'
        o_ip_A_subnet = '172.16.31.0'
        o_ip_mask = '255.255.255.255'
        sai_thrift_create_route(self.client, self.vr1, v4_family, o_ip_A, o_ip_mask, fg_ecmp)

        # v4 route - outer ip 2
        o_ip_B = '104.104.14.55'
        o_ip_B_subnet = '104.104.14.0'
        sai_thrift_create_route(self.client, self.vr1, v4_family, o_ip_B, o_ip_mask, fg_ecmp)

        # v6 route - outer ip 1
        o_v6_ip_A = '2222::5:5'
        o_v6_ip_A_subnet = '2222::5:0'
        o_v6_ip_mask = 'ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff'
        sai_thrift_create_route(self.client, self.vr1, v6_family, o_v6_ip_A, o_v6_ip_mask, fg_v6_ecmp)

        # v6 route - outer ip 2
        o_v6_ip_B = '8888::8080:AA'
        o_v6_ip_B_subnet = '8888::8080:0'
        sai_thrift_create_route(self.client, self.vr1, v6_family, o_v6_ip_B, o_v6_ip_mask, fg_v6_ecmp)

        o_l4_port_A = 14014
        o_l4_port_B = 15155
        src_v4 = '10.10.10.1'
        src_v6 = '1111::4:4'

        
        def GetInnerPkt(v4, tcp, src_port, tar_port, sip, dip):
            inner_pkt = None
            if (v4):
                if (tcp == False) :
                    inner_pkt = simple_udp_packet(
                                                eth_dst=tm,
                                                eth_src=sm,
                                                ip_dst=dip,
                                                ip_src=sip,
                                                ip_ttl=55,
                                                ip_id=108,
                                                udp_sport=src_port,
                                                udp_dport=tar_port)
                else:
                    inner_pkt = simple_tcp_packet(
                                                eth_dst=tm,
                                                eth_src=sm,
                                                ip_dst=dip,
                                                ip_src=sip,
                                                ip_ttl=55,
                                                ip_id=108,
                                                tcp_sport=src_port,
                                                tcp_dport=tar_port)
            else:
                if (tcp == False) :
                    inner_pkt = simple_udpv6_packet(
                                                eth_dst=tm,
                                                eth_src=sm,
                                                ipv6_dst=dip,
                                                ipv6_src=sip,
                                                ipv6_hlim=55,
                                                udp_sport=src_port,
                                                udp_dport=tar_port)
                else:
                    inner_pkt = simple_tcpv6_packet(
                                                eth_dst=tm,
                                                eth_src=sm,
                                                ipv6_dst=dip,
                                                ipv6_src=sip,
                                                ipv6_hlim=55,
                                                tcp_sport=src_port,
                                                tcp_dport=tar_port)
            return inner_pkt
        
        def GetVxlanPkt(smac, tmac, vlan, v4, ttl, i_pkt, rx_pkt, opp_flow):
            vx_vi = 10000
            if (opp_flow):
                o_ip = o_ip_B
                o_src_port = o_l4_port_B
                o_v6_ip = o_v6_ip_B
            else:
                o_ip = o_ip_A
                o_src_port = o_l4_port_A
                o_v6_ip = o_v6_ip_A

            if (v4):
                if (vlan == None):
                    vxlan_pkt = simple_vxlan_packet(eth_dst=tmac,
                                                eth_src=smac,
                                                ip_src=src_v4,
                                                ip_dst=o_ip,
                                                ip_ttl=ttl,
                                                ip_flags=0x2,
                                                udp_sport=o_src_port,
                                                vxlan_vni=vx_vi,
                                                with_udp_chksum=False,
                                                inner_frame=i_pkt)
                else:
                    vxlan_pkt = simple_vxlan_packet(eth_dst=tmac,
                                                eth_src=smac,
                                                dl_vlan_enable=True,
                                                vlan_vid=vlan,
                                                ip_src=src_v4,
                                                ip_dst=o_ip,
                                                ip_ttl=ttl,
                                                ip_flags=0x2,
                                                udp_sport=o_src_port,
                                                vxlan_vni=vx_vi,
                                                with_udp_chksum=False,
                                                inner_frame=i_pkt)
            else:
                if (vlan == None):
                    vxlan_pkt = simple_vxlanv6_packet(eth_dst=tmac,
                                                eth_src=smac,
                                                ipv6_src=src_v6,
                                                ipv6_dst=o_v6_ip,
                                                ipv6_hlim=ttl,
                                                udp_sport=o_src_port,
                                                vxlan_vni=vx_vi,
                                                with_udp_chksum=False,
                                                inner_frame=i_pkt)
                else:
                    vxlan_pkt = simple_vxlanv6_packet(eth_dst=tmac,
                                                eth_src=smac,
                                                dl_vlan_enable=True,
                                                vlan_vid=vlan,
                                                ipv6_src=src_v6,
                                                ipv6_dst=o_v6_ip,
                                                ipv6_hlim=ttl,
                                                udp_sport=o_src_port,
                                                vxlan_vni=vx_vi,
                                                with_udp_chksum=False,
                                                inner_frame=i_pkt)
            
            if (rx_pkt):
                vxlan_pkt = Mask(vxlan_pkt)
                vxlan_pkt.set_do_not_care_scapy(UDP, 'chksum')

            return vxlan_pkt
    
        def GetNvgrePkt(smac, tmac, vlan, v4, ttl, grekey, i_pkt, opp_flow):
            nvgre_pkt = None
            
            if (opp_flow):
                o_ip = o_ip_B
                o_v6_ip = o_v6_ip_B
            else:
                o_ip = o_ip_A
                o_v6_ip = o_v6_ip_A

            #  For nvgre_st_pkt
            #  - outer v4 -> nvgre [key=100 or config] -> inner v6 [without inner ethernet]
            #  if (grekey == nvgre_st_key and v4 and i_pkt['IPv6']):
            #    i_pkt = i_pkt['IPv6']

            if (v4):
                # gre key = 32 bits i.e. combination of tni [vs], flow-id
                # nvgre_v4_pkt - outer v4 -> nvgre [key!=0x6400 or key!=config] -> inner v4/v6
                # nvgre_st_pkt - outer v4 -> nvgre [key=0x6400 or config] -> inner v4/v6
                if (vlan == None):
                    nvgre_pkt = simple_nvgre_packet(eth_dst=tmac,
                                                eth_src=smac,
                                                ip_src=src_v4,
                                                ip_dst=o_ip,
                                                ip_ttl=ttl,
                                                nvgre_tni=grekey>>8,
                                                nvgre_flowid=grekey&0xFF,
                                                inner_frame=i_pkt)
                else:
                    nvgre_pkt = simple_nvgre_packet(eth_dst=tmac,
                                                eth_src=smac,
                                                dl_vlan_enable=True,
                                                vlan_vid=vlan,
                                                ip_src=src_v4,
                                                ip_dst=o_ip,
                                                ip_ttl=ttl,
                                                nvgre_tni=grekey>>8,
                                                nvgre_flowid=grekey&0xFF,
                                                inner_frame=i_pkt)
            else: 
                # gre key = 32 bits i.e. combination of tni [vs], flow-id 
                # nvgre_v6_pkt - outer v6 -> nvgre [key!=0x6400 or key!=config] -> inner v4/v6
                # nvgre_st_pkt - outer v6 -> nvgre [key=0x6400 or config] -> inner v4/v6
                if (vlan == None):
                    nvgre_pkt = simple_nvgrev6_packet(eth_dst=tmac,
                                                eth_src=smac,
                                                ipv6_src=src_v6,
                                                ipv6_dst=o_v6_ip,
                                                ipv6_hlim=ttl,
                                                nvgre_tni=grekey>>8,
                                                nvgre_flowid=grekey&0xFF,
                                                inner_frame=i_pkt)
                else:
                    nvgre_pkt = simple_nvgrev6_packet(eth_dst=tmac,
                                                eth_src=smac,
                                                dl_vlan_enable=True,
                                                vlan_vid=vlan,
                                                ipv6_src=src_v6,
                                                ipv6_dst=o_v6_ip,
                                                ipv6_hlim=ttl,
                                                nvgre_tni=grekey>>8,
                                                nvgre_flowid=grekey&0xFF,
                                                inner_frame=i_pkt)

            return nvgre_pkt

        
        def VxlanTestConsistentInnerHash(i_v4, i_ip_A, i_ip_B, i_tcp, i_l4_port_A, i_l4_port_B, o_v4):
            # Function  def - for parameter reference
            # - GetInnerPkt(v4, tcp, src_port, tar_port, sip, dip)
            # - GetVxlanPkt(smac, tmac, vlan, v4, ttl, i_pkt, rx_pkt, opp_flow)

            print " Vxlan [outer %s, inner %s, inner %s]" \
                %("v4" if o_v4==True else "v6", "v4" if i_v4==True else "v6", "Tcp" if i_tcp==True else "Udp")
            print "  Tx outer flow Z to A"
            print "  -- %s outer [%s --> %s, src port 0x%x]" \
                %("v4" if o_v4==True else "v6", src_v4 if o_v4==True else src_v6, \
                o_ip_A if o_v4==True else o_v6_ip_A, o_l4_port_A)
            print "  -- %s inner [%s --> %s, %s src port 0x%x, tar port 0x%x]" \
                %("v4" if i_v4==True else "v6", i_ip_A, i_ip_B, "Tcp" if i_tcp==True else "Udp", \
                i_l4_port_A, i_l4_port_B)

            i_pkt = GetInnerPkt(i_v4, i_tcp, i_l4_port_A, i_l4_port_B, i_ip_A, i_ip_B)
            vpkt = GetVxlanPkt(sm, router_mac, 50, o_v4, 64,  i_pkt, False, False)
            pkt_elist = []
            for cnt in range(0, num_nhop):
                vlan = None
                if v500_tag_list[cnt] == SAI_VLAN_TAGGING_MODE_TAGGED  :
                    vlan = 500
                pkt_e = GetVxlanPkt(router_mac, nhop_mac_list[cnt], vlan, o_v4, 63,  i_pkt, True, False)
                pkt_elist.append(pkt_e)

            send_packet(self, switch_ports[14], str(vpkt))
            rx_port = verify_packet_any_port_get_port(self, e_port)
            print "  Ecmp path using inner pkt consistent hash -> outgoing port %d" %(rx_port)
            print ""
            send_packet(self, switch_ports[14], str(vpkt))
            verify_packet(self, pkt_elist[rx_port], rx_port)


            print "  Tx outer flow Z to B --> Reversing the inner IP flow"
            print "  -- %s outer [%s --> %s, src port 0x%x]" \
                %("v4" if o_v4==True else "v6", src_v4 if o_v4==True else src_v6, \
                    o_ip_B if o_v4==True else o_v6_ip_B, o_l4_port_B)
            print "  -- %s inner [%s --> %s, %s src port 0x%x, tar port 0x%x]" \
                %("v4" if i_v4==True else "v6", i_ip_B, i_ip_A, "Tcp" if i_tcp==True else "Udp", \
                i_l4_port_B, i_l4_port_A)
               
            i_pkt = GetInnerPkt(i_v4, i_tcp, i_l4_port_B, i_l4_port_A, i_ip_B, i_ip_A)
            vpkt = GetVxlanPkt(sm, router_mac, 50, o_v4, 64, i_pkt, False, True)
            for cnt in range(0, num_nhop):
                vlan = None
                if v500_tag_list[cnt] == SAI_VLAN_TAGGING_MODE_TAGGED  :
                    vlan = 500
                pkt_elist[cnt] = GetVxlanPkt(router_mac, nhop_mac_list[cnt], vlan, o_v4, 63,  i_pkt, True, True)
            
            send_packet(self, switch_ports[14], str(vpkt))
            verify_packet(self, pkt_elist[rx_port], rx_port)
            print "  Ecmp path using inner pkt consistent hash -> outgoing port %d" %(rx_port)
            print " Inner IP in both directions points to same nexthop outgoing port %d" %(rx_port)
            print ""

        
        
        def NvgreTestConsistentInnerHash(i_v4, i_ip_A, i_ip_B, i_tcp, i_l4_port_A, i_l4_port_B, o_v4, grekey):
            # Function  def - for parameter reference
            # - GetInnerPkt(v4, tcp, src_port, tar_port, sip, dip)
            # - GetNvgrePkt(smac, tmac, vlan, v4, ttl, grekey, i_pkt, opp_flow)

            print " Nvgre [outer %s, inner %s, inner %s]" \
                %("v4" if o_v4==True else "v6", "v4" if i_v4==True else "v6", "Tcp" if i_tcp==True else "Udp")
            print "  Tx outer flow Z to A"
            print "  -- %s outer [%s --> %s, Gre Key 0x%x]" \
                %("v4" if o_v4==True else "v6", src_v4 if o_v4==True else src_v6, \
                    o_ip_A if o_v4==True else o_v6_ip_A, grekey)
            print "  -- %s inner [%s --> %s, %s src port 0x%x, tar port 0x%x]" \
                %("v4" if i_v4==True else "v6", i_ip_A, i_ip_B, "Tcp" if i_tcp==True else "Udp", \
                i_l4_port_A, i_l4_port_B)
            
            i_pkt = GetInnerPkt(i_v4, i_tcp, i_l4_port_A, i_l4_port_B, i_ip_A, i_ip_B)
            vpkt = GetNvgrePkt(sm, router_mac, 50, o_v4, 64, grekey, i_pkt, False)
            pkt_elist = []
            for cnt in range(0, num_nhop):
                vlan = None
                if v500_tag_list[cnt] == SAI_VLAN_TAGGING_MODE_TAGGED:
                    vlan = 500
                pkt_e = GetNvgrePkt(router_mac, nhop_mac_list[cnt], vlan, o_v4, 63, grekey, i_pkt, False)
                pkt_elist.append(pkt_e)

            send_packet(self, switch_ports[14], str(vpkt))
            rx_port = verify_packet_any_port_get_port(self, e_port)
            print "  Ecmp path using inner pkt consistent hash -> outgoing port %d" %(rx_port)
            print ""
            send_packet(self, switch_ports[14], str(vpkt))
            verify_packet(self, pkt_elist[rx_port], rx_port)


            print "  Tx outer flow Z to B --> Reversing the inner IP flow"
            print "  -- %s outer [%s --> %s, Gre Key 0x%x]" \
                %("v4" if o_v4==True else "v6", src_v4 if o_v4==True else src_v6, 
                o_ip_B if o_v4==True else o_v6_ip_B, grekey)
            print "  -- %s inner [%s --> %s, %s src port 0x%x, tar port 0x%x]" \
                %("v4" if i_v4==True else "v6", i_ip_B, i_ip_A, "Tcp" if i_tcp==True else "Udp", \
                i_l4_port_B, i_l4_port_A)
            
            i_pkt = GetInnerPkt(i_v4, i_tcp, i_l4_port_B, i_l4_port_A, i_ip_B, i_ip_A)
            vpkt = GetNvgrePkt(sm, router_mac, 50, o_v4, 64, grekey, i_pkt, True)
            for cnt in range(0, num_nhop):
                vlan = None
                if v500_tag_list[cnt] == SAI_VLAN_TAGGING_MODE_TAGGED:
                    vlan = 500
                pkt_elist[cnt] = GetNvgrePkt(router_mac, nhop_mac_list[cnt], vlan, o_v4, 63, grekey, i_pkt, True)
            
            send_packet(self, switch_ports[14], str(vpkt))
            verify_packet(self, pkt_elist[rx_port], rx_port)
            print "  Ecmp path using inner pkt consistent hash -> outgoing port %d" %(rx_port)
            print " Inner IP in both directions points to same nexthop outgoing port %d" %(rx_port)
            print ""
  
        
        def TestWithTunnelTraffic():
            # Function  def - for parameter reference
            # - GetInnerPkt(v4, tcp, src_port, tar_port, sip, dip)
            # - GetVxlanPkt(smac, tmac, vlan, v4, ttl, i_pkt, rx_pkt, opp_flow)
            # - GetNvgrePkt(smac, tmac, vlan, v4, ttl, grekey, i_pkt, opp_flow)

            # nhop_mac1 --> switch_ports[1] --> untag port 1
            # nhop_mac2 --> switch_ports[2] --> tag port 2
            # nhop_mac3 --> switch_ports[3] --> tag port 3
            
            print "Traffic Test"

            #  vxlan - all combinations
            print ""
            print "--------------------------------------------------------------------------"
            print("Testing ecmp consistent hash for all combinations of Vxlan tunnel packets ")
            #  VxlanTestConsistentInnerHash(i_v4, i_ip_A, i_ip_B, i_tcp, i_l4_port_A, i_l4_port_B, o_v4)
            #  out-pkt v4
            VxlanTestConsistentInnerHash(True, '58.58.58.5', '10.80.120.20', False, 40004, 22445, True) 
            VxlanTestConsistentInnerHash(True, '158.100.25.55', '155.20.140.155', True, 505, 14888, True)
            VxlanTestConsistentInnerHash(False,'4444::A855:8BBB:1', '1515::5225:28', True, 28455, 14888, True) 
            VxlanTestConsistentInnerHash(False,'5884::515:8004:85A', '5555::55:100:A41B', False, 8888, 5555, True) 
            #  out-pkt v6
            VxlanTestConsistentInnerHash(True, '120.140.150.55', '55.100.120.150', False, 22222, 44444, False) 
            VxlanTestConsistentInnerHash(True, '5.100.8.1', '88.155.55.44', True, 50505, 11558, False) 
            VxlanTestConsistentInnerHash(False,'88E5:588B:A::AB55:AB5', 'ABC0::52A5:A2', True, 11558, 18514, False) 
            VxlanTestConsistentInnerHash(False,'588E::5E15:8AB4:88', '55A5:8FB2::5B15:100', False, 8884, 25745, False)

            #  nvgre - all combinations
            print ""
            print "--------------------------------------------------------------------------"
            print("Testing ecmp consistent hash for all combinations of Nvgre tunnel packets ")
            #   - gre key = 32 bits i.e. combination of tni [vs], flow-id 
            #  nvgre_pkt    ->  outer v4/v6 - Nvgre [st-key!=0x6400 or st-key!=config] - Inner Eth - Inner v4/v6 - Inner Tcp/Udp
            #  nvgre_st_pkt -> outer v4/v6 - Nvgre [st-key==0x6400 or st-key==config] - Inner eth - Inner v4/v6 - Inner Tcp/Udp

            #  NvgreTestConsistentInnerHash(i_v4, i_ip_A, i_ip_B, i_tcp, i_l4_port_A, i_l4_port_B, o_v4, grekey)
            #   out-pkt v4
            NvgreTestConsistentInnerHash(True, '58.58.58.5', '10.80.120.20', False, 40004, 22445, True, 0xA64) 
            NvgreTestConsistentInnerHash(True, '158.100.25.55', '155.20.140.155', True, 505, 14888, True, 0x641) 
            NvgreTestConsistentInnerHash(False,'4444::A855:8BBB:1', '1515::5225:28', True, 28455, 14888, True, 0x6455) 
            NvgreTestConsistentInnerHash(False,'5884::515:8004:85A', '5555::55:100:A41B', False, 8888, 5555, True, 0xAAF5)
            #   out-pkt v6
            NvgreTestConsistentInnerHash(True, '120.140.150.55', '55.100.120.150', False, 22222, 44444, False, 0x564) 
            NvgreTestConsistentInnerHash(False,'88E5:588B:A::AB55:AB5', 'ABC0::52A5:A2', True, 11558, 18514, False, 0xFF01)  

            #  nvgre-st - all combinations
            print ""
            print "----------------------------------------------------------------------------"
            print("Testing ecmp consistent hash for all combinations of Nvgre-st tunnel packets")
            #  use gre key value 0x6400 for nvgre-st in this ptf
            #  If you want to use any other key value, then change the nvgre_st_key above
            #  Also requires P4 value set configuration of nvgre_st_key
            NvgreTestConsistentInnerHash(False,'4444::A855:8BBB:1', '1515::5225:28', True, 28455, 14888, True, nvgre_st_key) 
            NvgreTestConsistentInnerHash(False,'5884::515:8004:85A', '5555::55:100:A41B', False, 8888, 5555, True, nvgre_st_key)
            #  using nvgre-st key value with v6 outer pkt, v4 inner pkt - this also has to be parsed  as nvgre-st [new req]
            NvgreTestConsistentInnerHash(True, '5.100.8.14', '88.155.55.44', True, 50505, 1000, False, nvgre_st_key) 
            NvgreTestConsistentInnerHash(False,'588E::5E15:8AB4:88', '55A5:8FB2::5B15:100', False, 8884, 25745, False, nvgre_st_key)
            
            
            #  Test ecmp consistent hash with Inner V6 pkt - vxlan/nvgre/nvgre-st
            #  All combinations of inner v6 sip/dip  - mainly to test crc32 hash using high/low sip/dip 
            print ""
            print "------------------------------------------------------------------------------------------"
            print("Testing ecmp consistent hash for all combinations using inner v6 sip/dip")
            print("Ecmp consistent crc32 hash - sequence [low-ip, high-ip, protocol, low-l4-port, high-l4-port]")
            #  VxlanTestConsistentInnerHash(i_v4, i_ip_A, i_ip_B, i_tcp, i_l4_port_A, i_l4_port_B, o_v4)
            VxlanTestConsistentInnerHash(False,'4444::A855:8BBB:1AB1', '4444::A855:8BBB:5118', True, 28455, 14888, True) 
            VxlanTestConsistentInnerHash(False,'4444::A855:8BBB:1AB1', '4444::A855:ABFE:1AB1', False, 8888, 5555, True) 
            VxlanTestConsistentInnerHash(False,'88E5::1481:EFAB:AB55:AB5', '88E5::1481:1111:AB55:AB5', True, 11558, 18514, False) 
            VxlanTestConsistentInnerHash(False,'88E5::8AF4:EFAB:AB55:AB5', '88E5::8AF5:EFAB:AB55:AB5', False, 8884, 25745, False)

            #  NvgreTestConsistentInnerHash(i_v4, i_ip_A, i_ip_B, i_tcp, i_l4_port_A, i_l4_port_B, o_v4, grekey)
            NvgreTestConsistentInnerHash(False,'85AF:0000:EF14:84FA::A855:1', '85AF:0000:EF14:84FB::A855:1', True, 28455, 14888, True, 0x564) 
            NvgreTestConsistentInnerHash(False,'85AF:0000:0505:84FA::A855:1', '85AF:0000:0508:84FA::A855:1', False, 8888, 5555, True, 0x10006400)
            NvgreTestConsistentInnerHash(False,'AAAA:1488::588B:4588', 'AAAA:1588::588B:4588', True, 11558, 18514, False, 0xFF01) 

            #  nvgre-st scnearios - uses only lower 32 bits for hash
            NvgreTestConsistentInnerHash(False,'85AF:0000:0505:84FA::A855:1', '85AF:0000:0508:84FA::A855:1', False, 8888, 5555, True, nvgre_st_key)
            NvgreTestConsistentInnerHash(False,'85AF:0000:0505:84FA::A855:E585', '85AF:0000:0508:84FA::A855:5A5B', False, 8888, 5555, True, nvgre_st_key)
            #  using nvgre-st key value with v6 outer pkt, v4 inner pkt - this also has to be parsed  as nvgre-st [new req]
            NvgreTestConsistentInnerHash(False,'AABB:1488::588B:4588', 'AABE:1488::588B:4588', False, 8884, 25745, False, nvgre_st_key) 
        
        try:
            TestWithTunnelTraffic()
            
            #  Enable below with less number of ecmp-member operations 
            #  curently num_ecmp_mem is 280
            #  There is a chance PTF might skip because of pkt-rx error
            '''
            print "Perform Ecmp member/nhop operations"
            print "----------------------------------------------------------------------"
            print "Keep removing 20 members from both v4/v6 ecmp group, then test traffic"
            temp = 0
            for cnt in range(0, num_ecmp_mem):
                # v4 removal mem
                self.client.sai_thrift_remove_next_hop_group_member(ecmp_mem_list[cnt])
                ecmp_mem_list[cnt] = None
                # v6 removal mem
                self.client.sai_thrift_remove_next_hop_group_member(ecmp_mem_6_list[cnt])
                ecmp_mem_6_list[cnt] = None

                temp = temp + 1
                if temp % 20 == 0:
                    print " removing ecmp members [%d-%d] from both v4/v6 ecmp group" %((cnt+1)-temp, cnt)
                    time.sleep(1)
                    TestWithTunnelTraffic()
                    temp = 0
                if cnt == num_ecmp_mem-2:
                    print " removing ecmp members [%d-%d] from both v4/v6 ecmp group" %((cnt+1)-temp, cnt)
                    TestWithTunnelTraffic()
                    break

            print "----------------------------------------------------------------------"
            print " re-insert all the members back to v4/v6 ecmp group, then test traffic"
            # v4 ecmp setup
            # ecmp - member using nhops in below repetitve pattern
            # v4 ecmp setup
            #  [First 14 members - nhop 0, next 14 mem - nhop 1 ... last 14 mem - nhop 13]
            # v6 ecmp setup
            #  [0,1,2,3,4,5,6,7,8,9,10,11,12,13,0,1,2 .... ]
            nhop_i = 0
            for cnt in range(0, num_ecmp_mem-2):
                # v4 reinserting mem
                ecmp_mem = sai_thrift_create_next_hop_group_member(self.client, fg_ecmp, nhop_list[nhop_i], index=cnt)
                ecmp_mem_list[cnt] = ecmp_mem
                # v6 reinserting mem
                ecmp_mem = sai_thrift_create_next_hop_group_member(self.client, fg_v6_ecmp, nhop_6_list[cnt % num_nhop], index=cnt)
                ecmp_mem_6_list[cnt] = ecmp_mem
                
                if cnt % num_nhop == 0:
                    nhop_i = nhop_i + 1
                    if nhop_i == num_nhop:
                        nhop_i = 0
            
            TestWithTunnelTraffic()
            '''
        
        finally:
            sai_thrift_remove_route(self.client, self.vr1, v6_family, o_v6_ip_A, o_v6_ip_mask, fg_v6_ecmp)
            sai_thrift_remove_route(self.client, self.vr1, v6_family, o_v6_ip_B, o_v6_ip_mask, fg_v6_ecmp)
            
            for cnt in range(0, num_ecmp_mem):
                if ecmp_mem_6_list[cnt] != None:
                    self.client.sai_thrift_remove_next_hop_group_member(ecmp_mem_6_list[cnt])
            self.client.sai_thrift_remove_next_hop_group(fg_v6_ecmp)
            for cnt in range(0, num_nhop):
                self.client.sai_thrift_remove_next_hop(nhop_6_list[cnt])
                sai_thrift_remove_neighbor(self.client, v6_family, self.rif_1, nhop_ip_6_list[cnt], nhop_mac_list[cnt])
            
            sai_thrift_remove_route(self.client, self.vr1, v4_family, o_ip_A, o_ip_mask, fg_ecmp)
            sai_thrift_remove_route(self.client, self.vr1, v4_family, o_ip_B, o_ip_mask, fg_ecmp)
            for cnt in range(0, num_ecmp_mem):
                if ecmp_mem_list[cnt] != None:
                    self.client.sai_thrift_remove_next_hop_group_member(ecmp_mem_list[cnt])
            self.client.sai_thrift_remove_next_hop_group(fg_ecmp)
            for cnt in range(0, num_nhop):
                self.client.sai_thrift_remove_next_hop(nhop_list[cnt])
                sai_thrift_remove_neighbor(self.client, v4_family, self.rif_1, nhop_ip_list[cnt], nhop_mac_list[cnt])
            
            sai_thrift_flush_fdb_by_vlan(self.client, self.vlan50)
            sai_thrift_flush_fdb_by_vlan(self.client, self.vlan500)
            sai_thrift_flush_fdb(self.client, flush_type=SAI_FDB_FLUSH_ENTRY_TYPE_ALL)

            self.client.sai_thrift_remove_router_interface(self.rif_1)
            self.client.sai_thrift_remove_router_interface(self.rif_2)

            attr_value = sai_thrift_attribute_value_t(u16=1)
            attr = sai_thrift_attribute_t(id=SAI_PORT_ATTR_PORT_VLAN_ID, value=attr_value)
            for cnt in range(0, num_port):
                if v500_tag_list[cnt] == SAI_VLAN_TAGGING_MODE_UNTAGGED  :
                    self.client.sai_thrift_set_port_attribute(i_port[cnt], attr)
                self.client.sai_thrift_remove_vlan_member(v500_mem_list[cnt])
            self.client.sai_thrift_remove_vlan(self.vlan500)
            self.client.sai_thrift_remove_vlan_member(self.vlan50_member)
            self.client.sai_thrift_remove_vlan(self.vlan50)

            self.client.sai_thrift_remove_virtual_router(self.vr1)
